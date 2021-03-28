//
// Author: markovd@students.zcu.cz
//

#include "../ModelUnitTester.h"
#include "../../utils/LogUtils.h"
#include <utils/string_utils.h>
#include <rtl/ApproxLib.h>
#include <random>

tester::ModelUnitTester::ModelUnitTester(const GUID &guid, const std::wstring& libPath) : FilterUnitTester(guid, EntityType::MODEL) {
    setFilterLib(libPath);
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

    executeGenericTests();
}

HRESULT tester::ModelUnitTester::modelFlagTest() {
    scgms::TModel_Descriptor *descriptor = getEntityDescriptor<scgms::TGet_Model_Descriptors, scgms::TModel_Descriptor>(
            getFilterLib(), getFilterGuid(), "do_get_model_descriptors");
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
    if (!getFilterLib().Is_Loaded()) {
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
            getFilterLib(), getFilterGuid(), "do_get_model_descriptors");
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

    return constructEntity<scgms::TCreate_Discrete_Model>(getFilterLib(), "do_create_discrete_model", &getFilterGuid(),
                                                          parameterVector.get(), signalGenerator, model);
}

HRESULT tester::ModelUnitTester::invalidParameterCountTest() {
    scgms::TModel_Descriptor *descriptor = getEntityDescriptor<scgms::TGet_Model_Descriptors, scgms::TModel_Descriptor>(
            getFilterLib(), getFilterGuid(), "get_model_descriptors");
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

void tester::ModelUnitTester::loadFilter() {
    if (!getFilterLib().Is_Loaded()) {
        std::wcerr << L"Model library is not loaded! Model will not be loaded.\n";
        Logger::getInstance().error(L"Model library is not loaded! Model will not be loaded.");
        return;
    }

    if (getTestedFilter()) {
        getTestedFilter()->Release();
    }

    if (!m_signalGenerator) {
        HRESULT sigGenCreationResult = createSignalGeneratorFilter(&m_signalGenerator);
        if (!Succeeded(sigGenCreationResult)) {
            Logger::getInstance().error(L"Error while creating signal generator filter! Model will not be created!");
            return;
        }
    }

    getTestFilter().clearReceivedEvents();     /// Resetting so there will not be data from last test present
    scgms::IDiscrete_Model *model;
    HRESULT creationResult = createModel(&model, m_signalGenerator);
    if (creationResult != S_OK) {
        Logger::getInstance().error(L"Error while loading model from the dynamic library!");
    } else {
        setTestedFilter(model);
        Logger::getInstance().info(L"Model loaded from dynamic library.");
    }
}
