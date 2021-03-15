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

namespace tester {

    void FilterUnitTester::loadFilter() {
        if (!m_filterLibrary.Is_Loaded()) {
            std::wcerr << L"Filter library is not loaded! Filter will not be loaded.\n";
            Logger::getInstance().error(L"Filter library is not loaded! Filter will not be loaded.");
            return;
        }

        auto creator = m_filterLibrary.Resolve<scgms::TCreate_Filter>("do_create_filter");

        m_testedFilter = nullptr;
        auto result = creator(&m_testedGuid, &m_testFilter, &m_testedFilter);
        if (result != S_OK) {
            Logger::getInstance().error(L"Error while loading filter from the dynamic library!");
            m_testedFilter = nullptr;   /// Just to be sure
        }

        Logger::getInstance().info(L"Filter loaded from dynamic library.");
    }


    const wchar_t* FilterUnitTester::getFilterName() {
        if (!m_filterLibrary.Is_Loaded()) {
            loadFilterLibrary();
        }

        auto creator = m_filterLibrary.Resolve<scgms::TGet_Filter_Descriptors>("do_get_filter_descriptors");
        scgms::TFilter_Descriptor* begin, * end;
        creator(&begin, &end);
        for (int i = 0; i < (end - begin); i++) {
            if (begin->id == m_testedGuid) {
                return begin->description;
            }
            begin++;
        }

        return nullptr;
    }

    void FilterUnitTester::executeAllTests() {
        const wchar_t* filter_name = getFilterName();

        std::wcout << "****************************************\n"
                   << "Testing " << filter_name << " filter:\n"
                   << "****************************************\n";
        Logger::getInstance().debug(L"****************************************");
        Logger::getInstance().debug(L"Testing " + std::wstring(filter_name) + L" filter:");
        Logger::getInstance().debug(L"****************************************");

        executeGenericTests();
        executeSpecificTests();
    }

    void FilterUnitTester::executeGenericTests() {
        const wchar_t* libraryName = GuidFileMapper::GetInstance().getFileName(m_testedGuid);
        std::wstring file = std::wstring(libraryName) + cnst::LIB_EXTENSION;

        if (!ModuleUnitTester::isModuleTested(Narrow_WString(file))) {   /// Executing module tests if we didn't already

            ModuleUnitTester moduleTester;
            moduleTester.loadModule(Narrow_WString(file));
            moduleTester.executeModuleTests();
        } else {
            Logger::getInstance().info(std::wstring(L"Module ") + libraryName + L" already tested. Skipping module tests.");
        }

        Logger::getInstance().info(L"Executing generic filter tests...");
        executeTest(L"info event test", std::bind(&FilterUnitTester::infoEventTest, this));
        executeTest(L"warning event test", std::bind(&FilterUnitTester::warningEventTest, this));
        executeTest(L"error event test", std::bind(&FilterUnitTester::errorEventTest, this));
        executeTest(L"warm reset event test", std::bind(&FilterUnitTester::warmResetEventTest, this));
        executeTest(L"shut down event test", std::bind(&FilterUnitTester::shutDownEventTest, this));
    }

    void GenericUnitTester::executeTest(const std::wstring& testName, const std::function<HRESULT(void)>& test) {
        Logger::getInstance().info(L"----------------------------------------");
        Logger::getInstance().info(L"Executing " + testName + L"...");
        Logger::getInstance().info(L"----------------------------------------");
        std::wcout << "Executing " << testName << "... ";
        HRESULT result = runTestInThread(test);
        log::printResult(result);
    }

    void FilterUnitTester::executeConfigTest(const std::wstring& testName, const tester::FilterConfig& configuration, const HRESULT expectedResult) {
        Logger::getInstance().info(L"----------------------------------------");
        Logger::getInstance().info(L"Executing " + testName + L"...");
        Logger::getInstance().info(L"----------------------------------------");
        std::wcout << "Executing " << testName << "... ";
        HRESULT result = runConfigTestInThread(configuration, expectedResult);
        log::printResult(result);
    }

    HRESULT FilterUnitTester::shutDownTest() {
        if (!isFilterLoaded()) {
            return E_FAIL;
        }

        scgms::IDevice_Event* shutDown = createEvent(scgms::NDevice_Event_Code::Shut_Down);
        if (shutDown == nullptr) {
            std::wcerr << L"Error while creating " << describeEvent(scgms::NDevice_Event_Code::Shut_Down) << std::endl;
            Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Shut_Down));
            return E_FAIL;
        }

        m_testedFilter->Execute(shutDown);
        m_testedFilter->Release();
        m_testedFilter = nullptr;

        return S_OK;
    }

    HRESULT GenericUnitTester::runTestInThread(const std::function<HRESULT(void)>& test) {
        Logger::getInstance().debug(L"Running test in thread...");
        std::cv_status status;
        HRESULT result;

        {
            std::unique_lock<std::mutex> lock(m_testMutex);

            std::thread thread(&GenericUnitTester::runTest, this, test);

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

    void GenericUnitTester::runTest(const std::function<HRESULT(void)>& test) {
        m_lastTestResult = test();
        m_testCv.notify_all();
    }

    void FilterUnitTester::runConfigTest(const tester::FilterConfig& configuration, const HRESULT expectedResult) {
        if (!m_filterLibrary.Is_Loaded()) {
            loadFilterLibrary();
        }

        if (!isFilterLoaded()) {
            loadFilter();
        }

        if (isFilterLoaded()) {
            m_lastTestResult = configurationTest(configuration, expectedResult);
        } else {
            Logger::getInstance().error(L"Filter is not loaded! Test will not be executed.");
            m_lastTestResult = E_FAIL;
        }

        if (isFilterLoaded()) { /// Need to check, because filter will be unloaded in case of TIMEOUT
            shutDownTest();
        }

        m_testCv.notify_all();
    }

    bool FilterUnitTester::isFilterLoaded() {
        return m_testedFilter != nullptr;
    }


    //      **************************************************
    //                      Generic tests
    //      **************************************************


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

        HRESULT afterShutDownEventResult = m_testedFilter->Execute(infoEvent);
        if (Succeeded(afterShutDownEventResult)) {
            Logger::getInstance().error(L"Execution of event after shutting down did not fail!");
            Logger::getInstance().error(std::wstring(L"Actual result: ") + Describe_Error(afterShutDownEventResult));
            return E_FAIL;
        } else {
            return S_OK;
        }
    }

    HRESULT FilterUnitTester::informativeEventsTest(const scgms::NDevice_Event_Code eventCode) {
        if (!isFilterLoaded()) {
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
        HRESULT result = m_testedFilter->Execute(event);

        if (Succeeded(result)) {
            scgms::TDevice_Event receivedEvent = m_testFilter.getReceivedEvent();
            if (receivedEvent.event_code == eventCode) {
                result = S_OK;
            } else {
                Logger::getInstance().error(L"Event was modified during execution!");
                Logger::getInstance().error(L"expected code: " + describeEvent(eventCode));
                Logger::getInstance().error(L"actual code: " + describeEvent(receivedEvent.event_code));
                result = E_FAIL;
            }
        } else {
            Logger::getInstance().error(L"Error while sending " + describeEvent(scgms::NDevice_Event_Code::Shut_Down));
            Logger::getInstance().error(std::wstring(L"expected result: ") + Describe_Error(S_OK));
            Logger::getInstance().error(std::wstring(L"actual result: ") + Describe_Error(result));
            result = E_FAIL;
        }

        return result;
    }

    HRESULT FilterUnitTester::configureFilter(const tester::FilterConfig &config) {
        if (!isFilterLoaded()) {
            std::wcerr << L"No filter loaded! Can't execute test.\n";
            Logger::getInstance().error(L"No filter loaded! Can't execute test.");
            return E_FAIL;
        }

        scgms::SPersistent_Filter_Chain_Configuration configuration;
        refcnt::Swstr_list errors;

        HRESULT result = configuration ? S_OK : E_FAIL;
        std::string memory = config.toString();
        if (result == S_OK) {
            configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
            Logger::getInstance().debug(L"Loading configuration from memory...");
        } else {
            Logger::getInstance().error(L"Error while creating configuration!");
            return E_FAIL;
        }

        scgms::IFilter_Configuration_Link** begin, ** end;
        configuration->get(&begin, &end);

        Logger::getInstance().info(L"Configuring filter...");
        return m_testedFilter->Configure(begin[0], errors.get());
    }

    HRESULT FilterUnitTester::configurationTest(const tester::FilterConfig &config, const HRESULT expectedResult) {
        HRESULT testResult;

        HRESULT result = configureFilter(config);
        if (result == expectedResult) {
            testResult = S_OK;
        } else {
            log::logConfigurationError(config, expectedResult, result);
            testResult = E_FAIL;
        }

        return testResult;
    }

    TestFilter &FilterUnitTester::getTestFilter() {
        return m_testFilter;
    }

    scgms::IFilter *FilterUnitTester::getTestedFilter() {
        return m_testedFilter;
    }

    void FilterUnitTester::loadFilterLibrary() {
        const wchar_t* file_name = GuidFileMapper::GetInstance().getFileName(m_testedGuid);
        std::wstring file = std::wstring(file_name) + cnst::LIB_EXTENSION;

        m_filterLibrary.Load(file);

        if (!m_filterLibrary.Is_Loaded()) {
            std::wcerr << L"Couldn't load " << file_name << " library!\n";
            Logger::getInstance().error(L"Couldn't load " + std::wstring(file_name) + L" library.");
        }
    }

    void FilterUnitTester::runTest(const std::function<HRESULT(void)> &test) {
        if (!m_filterLibrary.Is_Loaded()) {
            loadFilterLibrary();
        }

        if (!isFilterLoaded()) {
            loadFilter();
        }

        if (isFilterLoaded()) {
            m_lastTestResult = test();
        } else {
            Logger::getInstance().error(L"Filter is not loaded! Test will not be executed.");
            m_lastTestResult = E_FAIL;
        }

        if (isFilterLoaded()) { /// Need to check, because filter will be unloaded in case of TIMEOUT
            shutDownTest();
        }

        m_testCv.notify_all();
    }

    FilterUnitTester::FilterUnitTester(const GUID guid) : m_testedGuid(guid), m_testedFilter(nullptr) {
        //
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

    HRESULT ModuleUnitTester::validFilterCreationTest() {
        scgms::TFilter_Descriptor *begin, *end;
        HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Filter_Descriptors>("do_get_filter_descriptors", &begin, &end);
        if (!Succeeded(descriptorsResult)) {
            Logger::getInstance().error(L"Error while acquiring filter descriptors!");
            return E_FAIL;
        }

        TestFilter testFilter;
        HRESULT testResult = S_OK;
        while (begin != end) {
            scgms::IFilter *filter;

            HRESULT constructionResult = constructEntity<scgms::TCreate_Filter>("do_create_filter", &begin->id, &testFilter, &filter);
            if (!Succeeded(constructionResult)) {
                Logger::getInstance().error(std::wstring(L"Error while creating ") + begin->description + L" filter!");
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(std::wstring(L"Successfully created ") + begin->description + L" filter!");
            }

            begin++;
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some filters could not be created!");
        }

        return testResult;
    }

    HRESULT ModuleUnitTester::validSignalCreationTest() {
        scgms::TSignal_Descriptor *begin, *end;
        HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Signal_Descriptors>("do_get_signal_descriptors", &begin, &end);
        if (!Succeeded(descriptorsResult)) {
            Logger::getInstance().error(L"Error while acquiring signal descriptors!");
            return E_FAIL;
        }

        HRESULT testResult = S_OK;
        while (begin != end) {
            scgms::CTime_Segment timeSegment;
            scgms::ISignal *signal;

            HRESULT constructionResult = constructEntity<scgms::TCreate_Signal>("do_create_signal", &begin->id, &timeSegment,
                                                                                nullptr, &signal);
            if (!Succeeded(constructionResult)) {
                Logger::getInstance().error(std::wstring(L"Error while creating ") + begin->signal_description + L" signal!");
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(std::wstring(L"Successfully created ") + begin->signal_description + L" signal!");
            }

            begin++;
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some signals could not be created!");
        }

        return testResult;
    }

    HRESULT ModuleUnitTester::validMetricCreationTest() {
        scgms::TMetric_Descriptor *begin, *end;
        HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Metric_Descriptors>("do_get_metric_descriptors", &begin, &end);
        if (!Succeeded(descriptorsResult)) {
            Logger::getInstance().error(L"Error while acquiring metric descriptors!");
            return E_FAIL;
        }

        HRESULT testResult = S_OK;
        while (begin != end) {
            scgms::IMetric *metric;
            scgms::TMetric_Parameters params { begin->id, 0, 0, 0, 0.0};
            HRESULT constructionResult = constructEntity<scgms::TCreate_Metric>("do_create_metric", &params, &metric);
            if (!Succeeded(constructionResult)) {
                Logger::getInstance().error(std::wstring(L"Error while creating ") + begin->description + L" metric!");
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(std::wstring(L"Successfully created ") + begin->description + L" metric!");
            }

            begin++;
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some metrics could not be created!");
        }

        return testResult;
    }

    HRESULT ModuleUnitTester::validSolverCreationTest() {
        scgms::TSolver_Descriptor *begin, *end;
        HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Solver_Descriptors>("do_get_solver_descriptors", &begin, &end);
        if (!Succeeded(descriptorsResult)) {
            Logger::getInstance().error(L"Error while acquiring solver descriptors!");
            return E_FAIL;
        }

        HRESULT testResult = S_OK;
        while (begin != end) {
            solver::TSolver_Progress nullProgress = solver::Null_Solver_Progress;
            HRESULT constructionResult = constructEntity<solver::TGeneric_Solver>("do_solve_generic", &begin->id,
                                                                                  &solver::Default_Solver_Setup, &nullProgress);
            if (!Succeeded(constructionResult)) {
                Logger::getInstance().error(std::wstring(L"Error while creating ") + begin->description + L" solver!");
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(std::wstring(L"Successfully created ") + begin->description + L" solver!");
            }

            begin++;
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some solvers could not be created!");
        }

        return testResult;
    }

    HRESULT ModuleUnitTester::validApproxCreationTest() {
        scgms::TApprox_Descriptor *begin, *end;
        HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Approx_Descriptors>("do_get_approximator_descriptors", &begin, &end);
        if (!Succeeded(descriptorsResult)) {
            Logger::getInstance().error(L"Error while acquiring approximator descriptors!");
            return E_FAIL;
        }

        scgms::CTime_Segment timeSegment;
        scgms::ISignal *signal;
        HRESULT signalCreation = constructEntity<scgms::TCreate_Signal>("do_create_signal", &scgms::signal_BG, &timeSegment,
                                                                        nullptr, &signal);
        if (!Succeeded(signalCreation)) {
            Logger::getInstance().error(L"Error while creating signal for approximators!");
            return E_FAIL;
        }

        HRESULT testResult = S_OK;
        while (begin != end) {
            scgms::IApproximator *approx;
            HRESULT creationResult = constructEntity<scgms::TCreate_Approximator>("do_create_approximator", &begin->id,
                                                                                  signal, &approx);
            if (!Succeeded(creationResult)) {
                Logger::getInstance().error(std::wstring(L"Error while creating ") + begin->description + L" approximator!");
                testResult = E_FAIL;
            } else {
                Logger::getInstance().info(std::wstring(L"Successfully created ") + begin->description + L" approximator!");
            }

            begin++;
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Some approximators could not be created!");
        }

        return testResult;
    }

    template <typename T, typename D>
    HRESULT ModuleUnitTester::descriptorsParamsTest(const std::string& symbolName) {
        D *begin, *end;
        HRESULT testResult = S_OK;

        HRESULT validResult = getEntityDescriptors<T>(symbolName, &begin, &end);
        if (!Succeeded(validResult)) {
            Logger::getInstance().error(L"Error while correctly acquiring descriptors from " + Widen_String(symbolName) + L"!");
            testResult = E_FAIL;
        } else {
            Logger::getInstance().info(L"Correctly acquired descriptors from " + Widen_String(symbolName));
        }

        D *invalidBegin;
        HRESULT invalidResult = getEntityDescriptors<T>(symbolName, &invalidBegin, &invalidBegin);
        if (Succeeded(invalidResult)) {
            Logger::getInstance().error(L"Invalid parameters (begin == end) successfully acquired descriptors from " + Widen_String(symbolName) + L"!");
            testResult = E_FAIL;
        } else {
            Logger::getInstance().info(L"Invalid parameters were correctly validated!");
        }

        return testResult;
    }

    template<typename T, typename D>
    HRESULT ModuleUnitTester::getEntityDescriptors(const std::string &symbolName, D **begin, D **end) {
        auto descriptorsCreator = m_moduleLibrary.Resolve<T>(symbolName.c_str());
        if (!descriptorsCreator) {
            Logger::getInstance().error(L"Error while loading " + Widen_String(symbolName) + L" descriptors factory!");
            return E_FAIL;
        }

        return descriptorsCreator(begin, end);
    }

    template<typename T, typename... Args>
    HRESULT ModuleUnitTester::constructEntity(const std::string &symbolName, Args... args) {
        Logger::getInstance().debug(L"Creating entity with " + Widen_String(symbolName) + L" factory.");

        auto entityFactory = m_moduleLibrary.Resolve<T>(symbolName.c_str());
        if (!entityFactory) {
            Logger::getInstance().error(L"Error while loading " + Widen_String(symbolName) + L" factory!");
            return E_FAIL;
        }

        HRESULT creationResult = entityFactory(args...);

        if (!Succeeded(creationResult)) {
            return E_FAIL;
        }


        return S_OK;
    }
}
