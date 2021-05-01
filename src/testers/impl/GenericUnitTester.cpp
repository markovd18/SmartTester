#include <iostream>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <functional>
#include <string>
#include <rtl/hresult.h>
#include <rtl/FilterLib.h>
#include <rtl/scgmsLib.h>
#include <iface/SolverIface.h>
#include <iface/ApproxIface.h>
#include "../GenericUnitTester.h"
#include "../../mappers/GuidFileMapper.h"
#include "../../utils/constants.h"
#include "../../utils/LogUtils.h"
#include "../../utils/scgmsLibUtils.h"
#include "../../utils/EntityUtils.h"

namespace tester {

    void FilterUnitTester::loadEntity() {
        if (!getEntityLib().Is_Loaded()) {
            std::wcerr << L"Filter library is not loaded! Filter will not be loaded.\n";
            Logger::getInstance().error(L"Filter library is not loaded! Filter will not be loaded.");
            return;
        }

        if (getTestedEntity()) {
            setTestedEntity(nullptr);
        }

        m_testFilter.clearReceivedEvents();     /// Resetting so there will not be data from last test present
        scgms::IFilter *testedFilter;
        HRESULT creationResult = constructEntity<scgms::TCreate_Filter>(getEntityLib(), "do_create_filter",
                                                                        &getEntityGuid(), &m_testFilter, &testedFilter);
        if (creationResult != S_OK) {
            Logger::getInstance().error(L"Error while loading filter from the dynamic library!");
        } else {
            setTestedEntity(testedFilter);
            Logger::getInstance().info(L"Filter loaded from dynamic library.");
        }
    }

    void FilterUnitTester::executeAllTests() {
        if (getLibraryPath().empty()) {
            const wchar_t* libraryName = GuidFileMapper::GetInstance().getFileName(getEntityGuid());
            setLibraryPath(std::wstring(libraryName) + cnst::LIB_EXTENSION);
        }

        if (!ModuleUnitTester::isModuleTested(Narrow_WString(getLibraryPath()))) {   /// Executing module tests if we didn't already
            executeModuleTests(Narrow_WString(getLibraryPath()));
        } else {
            Logger::getInstance().info(L"Module " + getLibraryPath() + L" already tested. Skipping module tests.");
        }

        const wchar_t *entityName;
        const wchar_t *entityType;
        if (m_entityType == EntityType::FILTER) {
            entityName = getEntityName<scgms::TGet_Filter_Descriptors, scgms::TFilter_Descriptor>("do_get_filter_descriptors");
            entityType = L"filter";
        } else {
            entityName = getEntityName<scgms::TGet_Model_Descriptors, scgms::TModel_Descriptor>("do_get_model_descriptors");
            entityType = L"model";
        }

        logs::printEntityTestsStartInfo(entityName, entityType);

        if (m_entityType == EntityType::FILTER) {
            executeGenericTests();      /// When testing model, we need to run creation tests first and create model manually during specific tests
        }

        executeSpecificTests();
    }

    void FilterUnitTester::executeGenericTests() {
        Logger::getInstance().info(L"Executing generic filter tests...");
        executeTest(L"info event test", std::bind(&FilterUnitTester::infoEventTest, this));
        executeTest(L"warning event test", std::bind(&FilterUnitTester::warningEventTest, this));
        executeTest(L"error event test", std::bind(&FilterUnitTester::errorEventTest, this));
        executeTest(L"warm reset event test", std::bind(&FilterUnitTester::warmResetEventTest, this));
        executeTest(L"shut down event test", std::bind(&FilterUnitTester::shutDownEventTest, this));
    }

    void TestRunner::executeTest(const std::wstring& testName, const std::function<HRESULT(void)>& test) {
        logs::printTestStartInfo(testName);
        HRESULT result = runTestInThread(test);
        logs::printResult(result);
    }

    void FilterUnitTester::executeConfigTest(const std::wstring& testName, const tester::FilterConfig& configuration, const HRESULT expectedResult) {
        logs::printTestStartInfo(testName);
        HRESULT result = runConfigTestInThread(configuration, expectedResult);
        logs::printResult(result);
    }

    HRESULT FilterUnitTester::shutDownTest() {
        if (!isEntityLoaded()) {
            return E_FAIL;
        }

        scgms::IDevice_Event* shutDown = createEvent(scgms::NDevice_Event_Code::Shut_Down);
        if (shutDown == nullptr) {
            std::wcerr << L"Error while creating " << describeEvent(scgms::NDevice_Event_Code::Shut_Down) << std::endl;
            Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Shut_Down));
            return E_FAIL;
        }

        getTestedEntity()->Execute(shutDown);
        setTestedEntity(nullptr);
        m_testFilter.clearReceivedEvents();
        return S_OK;
    }

    HRESULT TestRunner::runTestInThread(const std::function<HRESULT(void)>& test) {
        Logger::getInstance().debug(L"Running test in thread...");
        std::cv_status status;
        HRESULT result;

        {
            std::unique_lock<std::mutex> lock(m_testMutex);

            std::thread thread(&TestRunner::runTest, this, test);

            status = m_testCv.wait_for(lock, std::chrono::milliseconds(cnst::MAX_EXEC_TIME));
            lock.unlock();

            if (status == std::cv_status::timeout) {
                shutDownTest();
            }

            if (thread.joinable()) {
                thread.join();
            }
        }

        if (status == std::cv_status::timeout) {
            std::wcerr << L"TIMEOUT ";
            Logger::getInstance().error(L"Test in thread timed out!");
            result = E_FAIL;
        } else {
            result = m_lastTestResult;
        }

        return result;
    }

    HRESULT FilterUnitTester::runConfigTestInThread(const tester::FilterConfig& configuration, const HRESULT expectedResult) {
        Logger::getInstance().debug(L"Running test in thread...");
        std::cv_status status;
        HRESULT result;

        {
            std::unique_lock<std::mutex> lock(m_testMutex);

            std::thread thread(&FilterUnitTester::runConfigTest, this, std::ref(configuration), expectedResult);

            status = m_testCv.wait_for(lock, std::chrono::milliseconds(cnst::MAX_EXEC_TIME));
            lock.unlock();

            if (status == std::cv_status::timeout) {
//                shutDownTest();
            }

            if (thread.joinable()) {
                thread.join();
            }
        }

        if (status == std::cv_status::timeout) {
            std::wcerr << L"TIMEOUT ";
            Logger::getInstance().error(L"Test in thread timed out!");
            result = E_FAIL;
        } else {
            result = m_lastTestResult;
        }

        return result;
    }

    void TestRunner::runTest(const std::function<HRESULT(void)>& test) {
        m_lastTestResult = test();
        m_testCv.notify_all();
    }

    FilterUnitTester::FilterUnitTester(const GUID guid, const EntityType& type) : EntityUnitTester<scgms::IFilter>(guid), m_entityType(type){
        //
    }

    void FilterUnitTester::runConfigTest(const tester::FilterConfig& configuration, const HRESULT expectedResult) {
        if (!getEntityLib().Is_Loaded()) {
            loadEntityLibrary();
        }

        if (!isEntityLoaded()) {
            loadEntity();
        }

        if (isEntityLoaded()) {
            m_lastTestResult = configurationTest(configuration, expectedResult);
        } else {
            Logger::getInstance().error(L"Filter is not loaded! Test will not be executed.");
            m_lastTestResult = E_FAIL;
        }

        if (isEntityLoaded()) { /// Need to check, because filter will be unloaded in case of TIMEOUT
            shutDownTest();
        }

        m_testCv.notify_all();
    }

    HRESULT FilterUnitTester::infoEventTest() {
        return informativeEventsTest(scgms::NDevice_Event_Code::Information);
    }

    HRESULT FilterUnitTester::warningEventTest() {
        return informativeEventsTest(scgms::NDevice_Event_Code::Warning);
    }

    HRESULT FilterUnitTester::errorEventTest() {
        return informativeEventsTest(scgms::NDevice_Event_Code::Error);
    }

    HRESULT FilterUnitTester::warmResetEventTest() {
        return informativeEventsTest(scgms::NDevice_Event_Code::Warm_Reset);
    }

    HRESULT FilterUnitTester::shutDownEventTest() {
        HRESULT eventFallthroughResult = informativeEventsTest(scgms::NDevice_Event_Code::Shut_Down);
        if (!Succeeded(eventFallthroughResult)) {
            return eventFallthroughResult;
        }

        Logger::getInstance().info(L"Executing " + describeEvent(scgms::NDevice_Event_Code::Information) + L" after shutting down...");
        scgms::IDevice_Event *infoEvent = createEvent(scgms::NDevice_Event_Code::Information);
        if (!infoEvent) {
            return E_FAIL;
        }

        HRESULT afterShutDownEventResult = getTestedEntity()->Execute(infoEvent);
        if (Succeeded(afterShutDownEventResult)) {
            Logger::getInstance().error(L"Execution of event after shutting down did not fail!");
            Logger::getInstance().error(std::wstring(L"Actual result: ") + Describe_Error(afterShutDownEventResult));
            return E_FAIL;
        } else {
            return S_OK;
        }
    }

    HRESULT FilterUnitTester::informativeEventsTest(const scgms::NDevice_Event_Code& eventCode) {
        if (!isEntityLoaded()) {
            std::wcerr << L"No filter created! Can't execute test.\n";
            Logger::getInstance().error(L"No filter created! Can't execute test...");
            return E_FAIL;
        }

        scgms::IDevice_Event* event = createEvent(eventCode);
        if (event == nullptr) {
            std::wcerr << L"Error while creating " << describeEvent(eventCode) << std::endl;
            Logger::getInstance().error(L"Error while creating " + describeEvent(eventCode));
            return E_FAIL;
        }

        Logger::getInstance().debug(L"Executing event...");
        HRESULT result = getTestedEntity()->Execute(event);

        if (Succeeded(result)) {
            scgms::TDevice_Event receivedEvent = m_testFilter.getLastReceivedEvent();
            if (receivedEvent.event_code == eventCode) {
                result = S_OK;
            } else {
                Logger::getInstance().error(L"Event was modified during execution!");
                Logger::getInstance().error(L"expected code: " + describeEvent(eventCode));
                Logger::getInstance().error(L"actual code: " + describeEvent(receivedEvent.event_code));
                result = E_FAIL;
            }
        } else {
            Logger::getInstance().error(L"Error while sending " + describeEvent(eventCode));
            Logger::getInstance().error(std::wstring(L"expected result: ") + Describe_Error(S_OK));
            Logger::getInstance().error(std::wstring(L"actual result: ") + Describe_Error(result));
            result = E_FAIL;
        }

        return result;
    }

    HRESULT FilterUnitTester::configureFilter(const tester::FilterConfig &config) {
        return tester::configureFilter(getTestedEntity(), config);
    }

    HRESULT FilterUnitTester::configurationTest(const tester::FilterConfig &config, const HRESULT expectedResult) {
        HRESULT testResult;

        HRESULT result = configureFilter(config);
        if (result == expectedResult) {
            testResult = S_OK;
        } else {
            logs::logConfigurationError(config, expectedResult, result);
            testResult = E_FAIL;
        }

        return testResult;
    }

    TestFilter &FilterUnitTester::getTestFilter() {
        return m_testFilter;
    }

    std::vector<std::string> ModuleUnitTester::s_testedModules;
    std::map<std::string, std::string> ModuleUnitTester::s_descriptorsToFactories {
            {"do_get_filter_descriptors", "do_create_filter"},
            {"do_get_signal_descriptors", "do_create_signal"},
            {"do_get_metric_descriptors", "do_create_metric"},
            {"do_get_solver_descriptors", "do_solve_generic"},
            {"do_get_approximator_descriptors", "do_create_approximator"}
    };

    bool ModuleUnitTester::loadModule(const std::string &modulePath) {
        m_modulePath = modulePath;
        bool loaded = m_moduleLibrary.Load(modulePath);

        if (!loaded) {
            m_modulePath.clear();
        }

        return loaded;
    }

    void ModuleUnitTester::executeModuleTests() {
        if (!m_moduleLibrary.Is_Loaded()) {
            Logger::getInstance().error(L"No module loaded! Cannot execute module tests.");
            return;
        }

        Logger::getInstance().info(L"Executing module tests on " + Widen_String(m_modulePath) + L" module...");

        executeTest(L"descriptors methods test", std::bind(&ModuleUnitTester::descriptorsMethodsTest, this));
        executeTest(L"factory methods test", std::bind(&ModuleUnitTester::factoryMethodsTest, this));
        executeTest(L"entity creation test", std::bind(&ModuleUnitTester::entityCreationTest, this));
        executeTest(L"descriptor parameters validation test", std::bind(&ModuleUnitTester::descriptorsMethodParamsTest, this));

        s_testedModules.push_back(m_modulePath);
    }

    bool ModuleUnitTester::isModuleTested(const std::string& module) {
        return std::find(s_testedModules.begin(), s_testedModules.end(), module) != s_testedModules.end();
    }

    HRESULT ModuleUnitTester::shutDownTest() {
        return S_OK;    /// No operations needed
    }

    HRESULT ModuleUnitTester::descriptorsMethodsTest() {
        if (!m_moduleLibrary.Is_Loaded()) {
            Logger::getInstance().error(L"No module loaded! Cannot execute tests.");
            return E_FAIL;
        }

        bool descriptorsMethodFound = false;
        for (const auto &pair : s_descriptorsToFactories) {
            auto symbol = m_moduleLibrary.Resolve(pair.first.c_str());
            if (symbol != nullptr) {
                descriptorsMethodFound = true;
                m_foundDescriptorsMethods.push_back(pair.first);
                Logger::getInstance().info(Widen_String(pair.first) + L" method found!");
            }
        }

        if (!descriptorsMethodFound) {
            Logger::getInstance().error(L"No do_get_*_descriptors method found!");
            return E_FAIL;
        }

        return S_OK;
    }

    HRESULT ModuleUnitTester::descriptorsMethodParamsTest() {
        if (!m_moduleLibrary.Is_Loaded()) {
            Logger::getInstance().error(L"No module loaded! Cannot execute tests.");
            return E_FAIL;
        }

        if (m_foundDescriptorsMethods.empty()) {
            Logger::getInstance().error(L"No do_get_*_descriptors methods previously found! "
                                        "Cannot execute test. Execute descriptors method test first to proceed.");
            return E_FAIL;
        }

        HRESULT testResult = S_OK;
        for (const auto &descriptorsMethod : m_foundDescriptorsMethods) {
            HRESULT validationResult = m_descriptorsToParamsTests[descriptorsMethod](descriptorsMethod);
            if (!Succeeded(validationResult)) {
                testResult = E_FAIL;
            }
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some methods didn't validate input parameters correctly!");
            return E_FAIL;
        }

        Logger::getInstance().info(L"Input parameters validated!");
        return S_OK;
    }

    HRESULT ModuleUnitTester::factoryMethodsTest() {
        if (!m_moduleLibrary.Is_Loaded()) {
            Logger::getInstance().error(L"No module loaded! Cannot execute tests.");
            return E_FAIL;
        }

        if (m_foundDescriptorsMethods.empty()) {
            Logger::getInstance().error(L"No do_get_*_descriptors methods previously found! "
                                        "Cannot execute test. Execute descriptors method test first to proceed.");
            return E_FAIL;
        }

        HRESULT testResult = S_OK;
        for (const auto &descriptorsMethod : m_foundDescriptorsMethods) {
            const std::string& correspondingFactory = s_descriptorsToFactories[descriptorsMethod];
            auto factory = m_moduleLibrary.Resolve(correspondingFactory.c_str());

            if (factory == nullptr) {
                Logger::getInstance().error(L"Module implements " + Widen_String(descriptorsMethod) +
                L", but does not implement " + Widen_String(correspondingFactory));
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(Widen_String(correspondingFactory) + L" method found!");
            }
        }

        return testResult;
    }

    HRESULT ModuleUnitTester::entityCreationTest() {
        if (!m_moduleLibrary.Is_Loaded()) {
            Logger::getInstance().error(L"No module loaded! Cannot execute tests.");
            return E_FAIL;
        }

        if (m_foundDescriptorsMethods.empty()) {
            Logger::getInstance().error(L"No do_get_*_descriptors methods previously found! "
                                        "Cannot execute test. Execute descriptors method test first to proceed.");
            return E_FAIL;
        }

        HRESULT testResult = S_OK;
        for (const auto &descriptorsMethod : m_foundDescriptorsMethods) {
            HRESULT creationResult = m_descriptorsToTests[descriptorsMethod]();
            if (!Succeeded(creationResult)) {
                testResult = E_FAIL;
            }
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some valid entities could not be created!");
            return E_FAIL;
        }

        Logger::getInstance().info(L"All valid entities successfully created!");
        return S_OK;
    }

    HRESULT ModuleUnitTester::filterCreationTest() {
        scgms::TFilter_Descriptor *begin, *end;
        HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Filter_Descriptors>(m_moduleLibrary,"do_get_filter_descriptors",
                                                                                         &begin, &end);
        if (!Succeeded(descriptorsResult)) {
            Logger::getInstance().error(L"Error while acquiring filter descriptors!");
            return E_FAIL;
        }

        TestFilter testFilter;
        HRESULT testResult = S_OK;
        while (begin != end) {
            Logger::getInstance().debug(std::wstring(L"Creating ") + begin->description + L" filter...");

            scgms::IFilter *filter;
            HRESULT constructionResult = constructEntity<scgms::TCreate_Filter>(m_moduleLibrary, "do_create_filter",
                                                                                &begin->id, &testFilter, &filter);
            if (!Succeeded(constructionResult)) {
                Logger::getInstance().error(std::wstring(L"Error while creating ") + begin->description + L" filter!");
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(std::wstring(L"Successfully created ") + begin->description + L" filter!");
            }

            filter->Release();
            Logger::getInstance().info(L"Validating input parameters...");
            HRESULT invalidConstructionResult = constructEntity<scgms::TCreate_Filter>(m_moduleLibrary, "do_create_filter",
                                                                                       &begin->id, nullptr, &filter);
            if (Succeeded(invalidConstructionResult)) {
                Logger::getInstance().error(L"Null pointer accepted as a valid output filter!");
                testResult = E_FAIL;
            }

            filter->Release();
            invalidConstructionResult = constructEntity<scgms::TCreate_Filter>(m_moduleLibrary, "do_create_filter", &begin->id, &testFilter,
                                                                               nullptr);
            if (Succeeded(invalidConstructionResult)) {
                Logger::getInstance().error(L"Null pointer accepted as a valid filter pointer!");
                testResult = E_FAIL;
            }

            begin++;
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some filters could not be created!");
        }

        Logger::getInstance().info(L"Creating invalid filter...");

        scgms::IFilter *filter;
        HRESULT creationResult = constructEntity<scgms::TCreate_Filter>(m_moduleLibrary, "do_create_filter",
                                                                        &Invalid_GUID, &testFilter, &filter);
        if (Succeeded(creationResult)) {
            Logger::getInstance().error(L"Invalid filter was created!");
            testResult = E_FAIL;
        } else {
            Logger::getInstance().info(L"Invalid filter not created.");
        }

        return testResult;
    }

    HRESULT ModuleUnitTester::signalCreationTest() {
        scgms::TSignal_Descriptor *begin, *end;
        HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Signal_Descriptors>(m_moduleLibrary, "do_get_signal_descriptors", &begin, &end);
        if (!Succeeded(descriptorsResult)) {
            Logger::getInstance().error(L"Error while acquiring signal descriptors!");
            return E_FAIL;
        }

        HRESULT testResult = S_OK;
        while (begin != end) {
            Logger::getInstance().debug(std::wstring(L"Creating ") + begin->signal_description + L" signal...");

            scgms::CTime_Segment timeSegment;
            scgms::ISignal *signal;

            HRESULT constructionResult = constructEntity<scgms::TCreate_Signal>(m_moduleLibrary, "do_create_signal",
                                                                                &begin->id, &timeSegment, nullptr, &signal);
            if (!Succeeded(constructionResult)) {
                Logger::getInstance().error(std::wstring(L"Error while creating ") + begin->signal_description + L" signal!");
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(std::wstring(L"Successfully created ") + begin->signal_description + L" signal!");
                signal->Release();
            }

            Logger::getInstance().info(L"Validating input parameters...");
            HRESULT invalidConstructionResult = constructEntity<scgms::TCreate_Signal>(m_moduleLibrary, "do_create_signal",
                                                                                       &begin->id, nullptr, nullptr, &signal);
            if (Succeeded(invalidConstructionResult)) {
                Logger::getInstance().error(L"Null pointer accepted as a valid time segment!");
                testResult = E_FAIL;
                signal->Release();
            }

            invalidConstructionResult = constructEntity<scgms::TCreate_Signal>(m_moduleLibrary, "do_create_signal", &begin->id, &timeSegment,
                                                                               nullptr, nullptr);
            if (Succeeded(invalidConstructionResult)) {
                Logger::getInstance().error(L"Null pointer accepted as a valid signal pointer!");
                testResult = E_FAIL;
            }

            begin++;
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some signals could not be created!");
        }

        Logger::getInstance().info(L"Creating invalid signal...");
        scgms::CTime_Segment timeSegment;
        scgms::ISignal *signal;
        HRESULT creationResult = constructEntity<scgms::TCreate_Signal>(m_moduleLibrary, "do_create_signal",
                                                                        &Invalid_GUID, &timeSegment, nullptr, &signal);
        if (Succeeded(creationResult)) {
            Logger::getInstance().error(L"Invalid signal was created!");
            testResult = E_FAIL;
        } else {
            Logger::getInstance().info(L"Invalid signal not created.");
        }

        return testResult;
    }

    HRESULT ModuleUnitTester::metricCreationTest() {
        scgms::TMetric_Descriptor *begin, *end;
        HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Metric_Descriptors>(m_moduleLibrary, "do_get_metric_descriptors",
                                                                                         &begin, &end);
        if (!Succeeded(descriptorsResult)) {
            Logger::getInstance().error(L"Error while acquiring metric descriptors!");
            return E_FAIL;
        }

        HRESULT testResult = S_OK;
        while (begin != end) {
            Logger::getInstance().debug(std::wstring(L"Creating ") + begin->description + L" metric...");
            scgms::IMetric *metric;
            scgms::TMetric_Parameters params { begin->id, 0, 0, 0, 0.0};
            HRESULT constructionResult = constructEntity<scgms::TCreate_Metric>(m_moduleLibrary, "do_create_metric", &params, &metric);
            if (!Succeeded(constructionResult)) {
                Logger::getInstance().error(std::wstring(L"Error while creating ") + begin->description + L" metric!");
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(std::wstring(L"Successfully created ") + begin->description + L" metric!");
            }

            metric->Release();
            Logger::getInstance().info(L"Validating input parameters...");
            HRESULT invalidCreationResult = constructEntity<scgms::TCreate_Metric>(m_moduleLibrary, "do_create_metric",
                                                                                   nullptr, &metric);
            if (Succeeded(invalidCreationResult)) {
                Logger::getInstance().error(L"Null pointer accepted as a valid metric parameters!");
                testResult = E_FAIL;
            }

            metric->Release();
            invalidCreationResult = constructEntity<scgms::TCreate_Metric>(m_moduleLibrary, "do_create_metric", &params, nullptr);
            if (Succeeded(invalidCreationResult)) {
                Logger::getInstance().error(L"Null pointer accepted as a valid metric pointer!");
                testResult = E_FAIL;
            }
            begin++;
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some metrics could not be created!");
        }

        Logger::getInstance().info(L"Creating invalid metric...");
        scgms::IMetric *metric;
        HRESULT constructionResult = constructEntity<scgms::TCreate_Metric>(m_moduleLibrary, "do_create_metric",
                                                                            &scgms::Null_Metric_Parameters, &metric);
        if (Succeeded(constructionResult)) {
            Logger::getInstance().error(L"Invalid metric was created!");
            testResult = E_FAIL;
        } else {
            Logger::getInstance().info(L"Invalid metric not created.");
        }

        return testResult;
    }

    HRESULT ModuleUnitTester::solverCreationTest() {
        scgms::TSolver_Descriptor *begin, *end;
        HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Solver_Descriptors>(m_moduleLibrary, "do_get_solver_descriptors", &begin, &end);
        if (!Succeeded(descriptorsResult)) {
            Logger::getInstance().error(L"Error while acquiring solver descriptors!");
            return E_FAIL;
        }

        HRESULT testResult = S_OK;
        while (begin != end) {
            Logger::getInstance().debug(std::wstring(L"Creating ") + begin->description + L" solver...");

            solver::TSolver_Progress nullProgress = solver::Null_Solver_Progress;
            HRESULT constructionResult = constructEntity<solver::TGeneric_Solver>(m_moduleLibrary, "do_solve_generic",
                                                                                  &begin->id, &solver::Default_Solver_Setup, &nullProgress);
            if (!Succeeded(constructionResult)) {
                Logger::getInstance().error(std::wstring(L"Error while creating ") + begin->description + L" solver!");
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(std::wstring(L"Successfully created ") + begin->description + L" solver!");
            }

            Logger::getInstance().info(L"Validating input parameters...");
            HRESULT invalidConstructionResult = constructEntity<solver::TGeneric_Solver>(m_moduleLibrary, "do_solve_generic",
                                                                                         &begin->id, nullptr, &nullProgress);
            if (Succeeded(invalidConstructionResult)) {
                Logger::getInstance().error(L"Null pointer accepted as a valid solver setup!");
                testResult = E_FAIL;
            }

            invalidConstructionResult = constructEntity<solver::TGeneric_Solver>(m_moduleLibrary, "do_solve_generic",
                                                                                 &begin->id, &solver::Default_Solver_Setup, nullptr);
            if (Succeeded(invalidConstructionResult)) {
                Logger::getInstance().error(L"Null pointer accepted as a valid solver progress!");
                testResult = E_FAIL;
            }

            begin++;
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some solvers could not be created!");
        }

        Logger::getInstance().info(L"Creating invalid solver...");
        solver::TSolver_Progress nullProgress = solver::Null_Solver_Progress;
        HRESULT creationResult = constructEntity<solver::TGeneric_Solver>(m_moduleLibrary, "do_solve_generic",
                                                                          &Invalid_GUID, &solver::Default_Solver_Setup, &nullProgress);
        if (!Succeeded(creationResult)) {
            Logger::getInstance().error(L"Invalid solver created!");
            testResult = E_FAIL;
        } else {
            Logger::getInstance().info(L"Invalid solver not created.");
        }

        return testResult;
    }

    HRESULT ModuleUnitTester::approxCreationTest() {
        scgms::TApprox_Descriptor *begin, *end;
        HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Approx_Descriptors>(m_moduleLibrary, "do_get_approximator_descriptors", &begin, &end);
        if (!Succeeded(descriptorsResult)) {
            Logger::getInstance().error(L"Error while acquiring approximator descriptors!");
            return E_FAIL;
        }

        scgms::CTime_Segment timeSegment;
        scgms::ISignal *signal;
        HRESULT signalCreation = constructEntity<scgms::TCreate_Signal>(m_moduleLibrary, "do_create_signal",
                                                                        &scgms::signal_BG, &timeSegment, nullptr, &signal);
        if (!Succeeded(signalCreation)) {
            Logger::getInstance().error(L"Error while creating signal for approximators!");
            return E_FAIL;
        }

        HRESULT testResult = S_OK;
        while (begin != end) {
            Logger::getInstance().debug(std::wstring(L"Creating ") + begin->description + L" approximator...");

            scgms::IApproximator *approx;
            HRESULT creationResult = constructEntity<scgms::TCreate_Approximator>(m_moduleLibrary, "do_create_approximator",
                                                                                  &begin->id, signal, &approx);
            if (!Succeeded(creationResult)) {
                Logger::getInstance().error(std::wstring(L"Error while creating ") + begin->description + L" approximator!");
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(std::wstring(L"Successfully created ") + begin->description + L" approximator!");
            }

            approx->Release();
            Logger::getInstance().info(L"Validating input parameters...");
            HRESULT invalidConstructionResult = constructEntity<scgms::TCreate_Approximator>(m_moduleLibrary, "do_create_approximator",
                                                                                             &begin->id, nullptr, &approx);
            if (Succeeded(invalidConstructionResult)) {
                Logger::getInstance().error(L"Null pointer accepted as a valid signal!");
                testResult = E_FAIL;
            }

            approx->Release();
            invalidConstructionResult = constructEntity<scgms::TCreate_Approximator>(m_moduleLibrary, "do_create_approximator", &begin->id, signal,
                                                                                     nullptr);
            if (Succeeded(invalidConstructionResult)) {
                Logger::getInstance().error(L"Null pointer accepted as a valid approximator pointer!");
                testResult = E_FAIL;
            }
            begin++;
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some approximators could not be created!");
        }

        Logger::getInstance().info(L"Creating invalid approximator...");

        scgms::IApproximator *approx;
        HRESULT creationResult = constructEntity<scgms::TCreate_Approximator>(m_moduleLibrary, "do_create_approximator",
                                                                              &Invalid_GUID, signal, &approx);
        if (Succeeded(creationResult)) {
            Logger::getInstance().error(L"Invalid approximator was created!");
            testResult = E_FAIL;
        } else {
            Logger::getInstance().info(L"Invalid approximator not created.");
        }

        return testResult;
    }

    template <typename T, typename D>
    HRESULT ModuleUnitTester::descriptorsParamsTest(const std::string& symbolName) {
        D *begin, *end;
        HRESULT testResult = S_OK;

        HRESULT validResult = getEntityDescriptors<T>(m_moduleLibrary, symbolName, &begin, &end);
        if (!Succeeded(validResult)) {
            Logger::getInstance().error(L"Error while correctly acquiring descriptors from " + Widen_String(symbolName) + L"!");
            testResult = E_FAIL;
        } else {
            Logger::getInstance().info(L"Correctly acquired descriptors from " + Widen_String(symbolName));
        }

        D *invalidBegin;
        HRESULT invalidResult = getEntityDescriptors<T>(m_moduleLibrary, symbolName, &invalidBegin, &invalidBegin);
        if (Succeeded(invalidResult)) {
            Logger::getInstance().error(L"Invalid parameters (begin == end) successfully acquired descriptors from " + Widen_String(symbolName) + L"!");
            testResult = E_FAIL;
        } else {
            Logger::getInstance().info(L"Invalid parameters were correctly validated!");
        }

        return testResult;
    }

    void executeModuleTests(const std::string &modulePath) {
        std::wcout << "****************************************\n"
                   << "Testing " << Widen_String(modulePath) << " module:\n"
                   << "****************************************\n";
        Logger::getInstance().debug(L"****************************************");
        Logger::getInstance().debug(L"Testing " + Widen_String(modulePath) + L" module:");
        Logger::getInstance().debug(L"****************************************");

        ModuleUnitTester moduleTester;
        if (!moduleTester.loadModule(modulePath)) {
            Logger::getInstance().error(L"Error while loading module! Skipping module tests...");
            return;
        }

        moduleTester.executeModuleTests();
    }

}
