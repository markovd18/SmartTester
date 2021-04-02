//
// Author: markovd@students.zcu.cz
//

#include "../ModelUnitTester.h"
#include "../../utils/LogUtils.h"
#include <utils/string_utils.h>
#include <rtl/ApproxLib.h>
#include <random>
#include <rtl/FilterLib.h>

tester::ModelUnitTester::ModelUnitTester(const GUID &guid, const std::wstring& libPath) : FilterUnitTester(guid, EntityType::MODEL) {
    setEntityLib(libPath);
}

void tester::ModelUnitTester::executeSpecificTests() {
    logs::printTestStartInfo(L"model flag test");
    HRESULT flagResult = modelFlagTest();
    if (!Succeeded(flagResult)) {
        logs::printResult(flagResult);
    } else {
        logs::printResult(S_OK);
    }

    logs::printTestStartInfo(L"model creation test");
    HRESULT creationResult;
    switch (flagResult) {
        case S_OK:
            creationResult = modelCreationTest(scgms::NModel_Flags::Discrete_Model);
            break;
        case S_FALSE:
            creationResult = modelCreationTest(scgms::NModel_Flags::Signal_Model);
            break;
        default:
            creationResult = modelCreationTest(scgms::NModel_Flags::None);
    }
    logs::printResult(creationResult);

    if (flagResult != S_OK) {
        return;     /// Continuing with tests for discrete models
    }

    logs::printTestStartInfo(L"invalid parameter count test");
    HRESULT invalidParamTestResult = invalidParameterCountTest();
    logs::printResult(invalidParamTestResult);

    /// Executing generic tests after we executed and passed all the construction tests
    executeGenericTests();

    executeTest(L"initialize negative current time test", std::bind(&ModelUnitTester::initializeNegativeCurrentTimeTest, this));
    executeTest(L"initialize positive current time test", std::bind(&ModelUnitTester::initializePositiveCurrentTimeTest, this));
    executeTest(L"repeated initialize test", std::bind(&ModelUnitTester::repeatedInitializeCallTest, this));
    executeTest(L"step before initialization test", std::bind(&ModelUnitTester::stepBeforeInitializeTest, this));
    executeTest(L"negative step delta time test", std::bind(&ModelUnitTester::negativeStepDeltaTimeTest, this));
    executeTest(L"current time step test", std::bind(&ModelUnitTester::currentTimeStepTest, this));
    executeTest(L"future time step test", std::bind(&ModelUnitTester::futureTimeStepTest, this));
}

HRESULT tester::ModelUnitTester::modelFlagTest() {
    scgms::TModel_Descriptor *descriptor = getEntityDescriptor<scgms::TGet_Model_Descriptors, scgms::TModel_Descriptor>(
            getEntityLib(), getEntityGuid(), "do_get_model_descriptors");
    if (descriptor == nullptr) {
        Logger::getInstance().error(L"Error while acquiring model descriptors!");
        return E_FAIL;
    }

    switch (descriptor->flags) {
        case scgms::NModel_Flags::Discrete_Model:
            Logger::getInstance().info(L"Found model flag: Discrete_Model");
            return S_OK;
        case scgms::NModel_Flags::Signal_Model:
            Logger::getInstance().info(L"Found model flag: Signal_Model");
            return S_FALSE;
        default:
            Logger::getInstance().error(L"Model doesn't have Discrete_Model or Signal_Model flag!");
            return E_FAIL;
    }
}

HRESULT tester::ModelUnitTester::modelCreationTest(const scgms::NModel_Flags &modelFlag) {
    if (!getEntityLib().Is_Loaded()) {
        Logger::getInstance().error(L"Filter library is not loaded! Cannot execute tests.");
        return E_FAIL;
    }

    if (!m_signalGenerator) {
        HRESULT generatorCreationResult = createSignalGeneratorFilter(&m_signalGenerator);
        if (!Succeeded(generatorCreationResult)) {
            Logger::getInstance().error(L"Failed to create signal generator! Cancelling test...");
            return E_FAIL;
        }
    }

    scgms::IDiscrete_Model *model;
    HRESULT modelCreationResult = createModel(&model, m_signalGenerator);
    if (modelFlag == scgms::NModel_Flags::Discrete_Model) {
        if (!Succeeded(modelCreationResult)) {
            Logger::getInstance().error(L"Error while creating discrete model!");
            return E_FAIL;
        }

        Logger::getInstance().info(L"Discrete model created successfully.");
        return S_OK;
    } else {
        if (Succeeded(modelCreationResult)) {
            Logger::getInstance().error(L"Error while creating non-discrete model! Model was created bud should not be!");
            return E_FAIL;
        }

        Logger::getInstance().info(L"Non-discrete model not created.");
        return S_OK;
    }

}

HRESULT tester::ModelUnitTester::createSignalGeneratorFilter(scgms::IFilter **filter) {
    Logger::getInstance().debug(L"Creating signal generator filter...");

    CDynamic_Library library;
    if (!library.Load(std::wstring(cnst::SIGNAL_LIBRARY) + cnst::LIB_EXTENSION)) {
        Logger::getInstance().error(L"Error while loading signal library!");
        return E_FAIL;
    }

    return constructEntity<scgms::TCreate_Filter>(library, "do_create_filter", &cnst::SIGNAL_GEN_GUID, &getTestFilter(), filter);
}

HRESULT tester::ModelUnitTester::createModel(scgms::IDiscrete_Model **model, scgms::IFilter* signalGenerator) {

    scgms::TModel_Descriptor *descriptor = getEntityDescriptor<scgms::TGet_Model_Descriptors, scgms::TModel_Descriptor>(
            getEntityLib(), getEntityGuid(), "do_get_model_descriptors");
    if (!descriptor) {
        Logger::getInstance().error(L"Model descriptor not found!");
        return E_FAIL;
    }

    return createModel(model, signalGenerator, descriptor->number_of_parameters);
}

HRESULT tester::ModelUnitTester::createModel(scgms::IDiscrete_Model **model, scgms::IFilter *signalGenerator,
                                             const std::size_t parameterCount) {
    std::vector<double> params;
    for (std::size_t i = 0; i < parameterCount; ++i) {
        params.push_back(2.0);
    }

    scgms::SModel_Parameter_Vector parameterVector;
    parameterVector.set(params);

    return constructEntity<scgms::TCreate_Discrete_Model>(getEntityLib(), "do_create_discrete_model", &getEntityGuid(),
                                                          parameterVector.get(), signalGenerator, model);
}

HRESULT tester::ModelUnitTester::invalidParameterCountTest() {
    scgms::TModel_Descriptor *descriptor = getEntityDescriptor<scgms::TGet_Model_Descriptors, scgms::TModel_Descriptor>(
            getEntityLib(), getEntityGuid(), "get_model_descriptors");
    if (!descriptor) {
        Logger::getInstance().error(L"Error while acquiring model descriptor!");
        return E_FAIL;
    }

    std::random_device rdev;
    std::uniform_int_distribution<std::size_t> dist(0, descriptor->number_of_parameters);
    std::size_t paramCount;

    do {
        paramCount = dist(rdev);    /// Making sure we get invalid number of parameters
    } while (paramCount == descriptor->number_of_parameters);

    scgms::IDiscrete_Model *model;
    HRESULT creationResult = createModel(&model, m_signalGenerator, paramCount);
    if (Succeeded(creationResult)) {
        Logger::getInstance().error(L"Model was created with invalid number of parameters!");
        return E_FAIL;
    }

    Logger::getInstance().info(L"Model correctly not created.");
    return S_OK;
}

void tester::ModelUnitTester::loadEntity() {
    if (!getEntityLib().Is_Loaded()) {
        std::wcerr << L"Model library is not loaded! Model will not be loaded.\n";
        Logger::getInstance().error(L"Model library is not loaded! Model will not be loaded.");
        return;
    }

    if (getTestedEntity()) {
        getTestedEntity()->Release();
    }

    if (!m_signalGenerator) {
        HRESULT sigGenCreationResult = createSignalGeneratorFilter(&m_signalGenerator);
        if (!Succeeded(sigGenCreationResult)) {
            Logger::getInstance().error(L"Error while creating signal generator filter! Model will not be created!");
            return;
        }
    }

    /// Configuring signal generator to default values
    tester::SignalGeneratorConfig config;
    config.setFeedbackName("fb1");
    config.setTimeSegmentId(1);
    config.setMaximumTime(0.5);
    config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});
    config.setStepping(0.003472222222222222);
    config.setSynchronizeToSignal(true);
    config.setSynchronizationSignal(scgms::signal_BG);
    config.setModelId(getEntityGuid());
    HRESULT configResult = tester::configureFilter(m_signalGenerator, config);
    if (!Succeeded(configResult)) {
        logs::logConfigurationError(config, S_OK, configResult);
        return;
    }

    getTestFilter().clearReceivedEvents();     /// Resetting so there will not be data from last test present
    scgms::IDiscrete_Model *model;
    HRESULT creationResult = createModel(&model, m_signalGenerator);
    if (creationResult != S_OK) {
        Logger::getInstance().error(L"Error while loading model from the dynamic library!");
    } else {
        setTestedEntity(model);
        Logger::getInstance().info(L"Model loaded from dynamic library.");
    }
}

HRESULT tester::ModelUnitTester::initializeNegativeCurrentTimeTest() {
    return initialize(-1.5, 2, false, E_FAIL);
}

HRESULT tester::ModelUnitTester::initializePositiveCurrentTimeTest() {
    return initialize(5.0, 159, true, S_OK);
}

HRESULT tester::ModelUnitTester::repeatedInitializeCallTest() {
    HRESULT initializeResult = initialize(106.2, 420, true, S_OK);
    if (!Succeeded(initializeResult)) {
        return E_FAIL;
    }

    HRESULT secondInitResult = initialize(0.0, 123, false, E_ILLEGAL_STATE_CHANGE);
    if (!Succeeded(secondInitResult)) {
        Logger::getInstance().error(L"Second initialization succeeded!");
        return E_FAIL;
    }

    Logger::getInstance().info(L"Repeated initialization failed with expected result.");
    return S_OK;
}

HRESULT tester::ModelUnitTester::stepBeforeInitializeTest() {
    HRESULT stepResult = step(0.0, false, E_ILLEGAL_METHOD_CALL);    /// Trying to emit current state
    if (!Succeeded(stepResult)) {
        Logger::getInstance().error(L"Calling Step method before Initialize did not fail!");
        return E_FAIL;
    }

    Logger::getInstance().info(L"Calling Step method before Initialize failed.");
    return S_OK;
}

HRESULT tester::ModelUnitTester::negativeStepDeltaTimeTest() {
    HRESULT initializeResult = initialize(2.0, 321, true, S_OK);
    if (!Succeeded(initializeResult)) {
        return E_FAIL;
    }

    HRESULT stepResult = step(-1.8, false, E_INVALIDARG);
    if (!Succeeded(stepResult)) {
        return E_FAIL;
    }

    Logger::getInstance().info(L"Calling Step method with negative time_advance_delta failed.");
    return S_OK;
}

HRESULT tester::ModelUnitTester::initialize(const double currentTime, const uint64_t segmentId,
                                            const bool shouldSucceed, const HRESULT expectedResult) {
    auto *testedModel = dynamic_cast<scgms::IDiscrete_Model*>(getTestedEntity());

    Logger::getInstance().debug(L"Initializing model with currentTime=" + std::to_wstring(currentTime) + L"...");
    HRESULT initializeResult = testedModel->Initialize(currentTime, segmentId);
    if ((shouldSucceed && !Succeeded(initializeResult)) ||
        (!shouldSucceed && Succeeded(initializeResult))) {
        Logger::getInstance().error(L"Error while initializing the model! Initialized with current time: " + std::to_wstring(currentTime));
        Logger::getInstance().error(std::wstring(L"Expected result: ") + Describe_Error(expectedResult));
        Logger::getInstance().error(std::wstring(L"Actual result: ") + Describe_Error(initializeResult));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Model initialization ended as expected.");
    return S_OK;
}

HRESULT tester::ModelUnitTester::step(const double timeAdvanceDelta, const bool shouldSucceed, const HRESULT expectedResult) {
    auto *testedModel = dynamic_cast<scgms::IDiscrete_Model*>(getTestedEntity());

    Logger::getInstance().debug(L"Stepping with time_advance_delta=" + std::to_wstring(timeAdvanceDelta) + L"...");
    HRESULT stepResult = testedModel->Step(timeAdvanceDelta);
    if ((shouldSucceed && !Succeeded(stepResult)) ||
        (!shouldSucceed && Succeeded(stepResult))) {
        Logger::getInstance().error(L"Error while stepping! Stepping with time_advance_delta=" + std::to_wstring(timeAdvanceDelta));
        Logger::getInstance().error(std::wstring(L"Expected result: ") + Describe_Error(expectedResult));
        Logger::getInstance().error(std::wstring(L"Actual result: ") + Describe_Error(stepResult));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Stepping ended as expected.");
    return S_OK;
}

HRESULT tester::ModelUnitTester::currentTimeStepTest() {
    HRESULT initializeResult = initialize(0.0, 18, true, S_OK);
    if (!Succeeded(initializeResult)) {
        return E_FAIL;
    }

    /// Checking state in time 0
    double steppedDeviceTime = 0.0;
    HRESULT stepResult = step(steppedDeviceTime, true, S_OK);
    if (!Succeeded(stepResult)) {
        return E_FAIL;
    }

    scgms::TDevice_Event event = getTestFilter().getLastReceivedEvent();
    if (event.event_code == scgms::NDevice_Event_Code::Nothing) {
        Logger::getInstance().error(L"No event was acquired when emitting current state!");
        return E_FAIL;
    }

    double epsilon = 0.0001;    /// Epsilon for comparing double values
    if ((event.device_time - steppedDeviceTime) > epsilon) {
        Logger::getInstance().error(L"Acquired event's device time does not match stepped device time!");
        Logger::getInstance().error(L"Stepped device time: " + std::to_wstring(steppedDeviceTime));
        Logger::getInstance().error(L"Acquired device time: " + std::to_wstring(event.device_time));
        return E_FAIL;
    }

    steppedDeviceTime += 0.6;
    stepResult = step(steppedDeviceTime, true, S_OK);   /// Moving by 0.6
    if (!Succeeded(stepResult)) {
        return E_FAIL;
    }

    /// Checking state in time += 0.6
    getTestFilter().clearReceivedEvents();      /// Clearing events so we can tell when nothing was sent to us
    stepResult = step(0.0, true, S_OK); /// Checking current state in the stepped time
    if (!Succeeded(stepResult)) {
        return E_FAIL;
    }

    event = getTestFilter().getLastReceivedEvent();
    if (event.event_code == scgms::NDevice_Event_Code::Nothing) {
        Logger::getInstance().error(L"No event was acquired when emitting current state!");
        return E_FAIL;
    }

    if ((event.device_time - steppedDeviceTime) > epsilon) {
        Logger::getInstance().error(L"Acquired event's device time does not match stepped device time!");
        Logger::getInstance().error(L"Stepped device time: " + std::to_wstring(steppedDeviceTime));
        Logger::getInstance().error(L"Acquired device time: " + std::to_wstring(event.device_time));
        return E_FAIL;
    }

    Logger::getInstance().info(L"All states emitted correctly.");
    return S_OK;
}

HRESULT tester::ModelUnitTester::futureTimeStepTest() {
    HRESULT initializeResult = initialize(0.0, 18, true, S_OK);
    if (!Succeeded(initializeResult)) {
        return E_FAIL;
    }

    /// Checking state in time 1
    double steppedDeviceTime = 1.0;
    HRESULT stepResult = step(steppedDeviceTime, true, S_OK);
    if (!Succeeded(stepResult)) {
        return E_FAIL;
    }

    scgms::TDevice_Event event = getTestFilter().getLastReceivedEvent();
    if (event.event_code == scgms::NDevice_Event_Code::Nothing) {
        Logger::getInstance().info(L"No event was emitted to describe state in time " + std::to_wstring(steppedDeviceTime));
        return S_OK;
    }

    double epsilon = 0.0001;    /// Epsilon for comparing double values
    if ((event.device_time - steppedDeviceTime) > epsilon) {
        Logger::getInstance().error(L"Acquired event's device time does not match stepped device time!");
        Logger::getInstance().error(L"Stepped device time: " + std::to_wstring(steppedDeviceTime));
        Logger::getInstance().error(L"Acquired device time: " + std::to_wstring(event.device_time));
        return E_FAIL;
    }

    Logger::getInstance().info(L"State in future time emitted correctly.");
    return S_OK;
}