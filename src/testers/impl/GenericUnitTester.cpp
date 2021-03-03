#include <iostream>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <functional>
#include <string>
#include <rtl/hresult.h>
#include <rtl/FilterLib.h>
#include <utils/string_utils.h>
#include <rtl/scgmsLib.h>
#include "../../mappers/GuidFileMapper.h"
#include "../../utils/constants.h"
#include "../../utils/LogUtils.h"
#include "../GenericUnitTester.h"
#include "../../utils/scgmsLibUtils.h"

namespace tester {

    GenericUnitTester::GenericUnitTester(const GUID guid)
            :  m_lastTestResult(S_OK), m_testedGuid(guid), m_testedFilter(loadFilter()) {
    }

    scgms::IFilter* GenericUnitTester::loadFilter() {

        const wchar_t* file_name = GuidFileMapper::GetInstance().getFileName(m_testedGuid);
        std::wstring file = std::wstring(file_name) + cnst::LIB_EXTENSION;

        m_filterLibrary.Load(file);


        if (!m_filterLibrary.Is_Loaded()) {
            std::wcerr << L"Couldn't load " << file_name << " library!\n";
            Logger::getInstance().error(L"Couldn't load " + std::wstring(file_name) + L" library.");
            return nullptr;
        }

        auto creator = m_filterLibrary.Resolve<scgms::TCreate_Filter>("do_create_filter");

        scgms::IFilter* created_filter = nullptr;
        auto result = creator(&m_testedGuid, &m_testFilter, &created_filter);
        if (result != S_OK) {
            return nullptr;
        }

        Logger::getInstance().info(L"Filter loaded from dynamic library.");
        return created_filter;
    }


    const wchar_t* GenericUnitTester::getFilterName() {
        if (!m_filterLibrary.Is_Loaded()) {
            return nullptr;
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

    /**
        Executes all generic and specific tests for given filter.
    */
    void GenericUnitTester::executeAllTests() {
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

    void GenericUnitTester::executeGenericTests() {

        Logger::getInstance().info(L"Executing generic tests...");
        executeTest(L"info event test", std::bind(&GenericUnitTester::infoEventTest, this));
        executeTest(L"warning event test", std::bind(&GenericUnitTester::warningEventTest, this));
        executeTest(L"error event test", std::bind(&GenericUnitTester::errorEventTest, this));
        executeTest(L"warm reset event test", std::bind(&GenericUnitTester::warmResetEventTest, this));
        executeTest(L"shut down event test", std::bind(&GenericUnitTester::shutDownEventTest, this));
    }

    void GenericUnitTester::executeTest(const std::wstring& testName, const std::function<HRESULT(void)>& test) {
        Logger::getInstance().info(L"----------------------------------------");
        Logger::getInstance().info(L"Executing " + testName + L"...");
        Logger::getInstance().info(L"----------------------------------------");
        std::wcout << "Executing " << testName << "... ";
        HRESULT result = runTestInThread(test);
        log::printResult(result);
    }

    void GenericUnitTester::executeConfigTest(const std::wstring& testName, const std::string& configuration, const HRESULT expectedResult) {
        Logger::getInstance().info(L"----------------------------------------");
        Logger::getInstance().info(L"Executing " + testName + L"...");
        Logger::getInstance().info(L"----------------------------------------");
        std::wcout << "Executing " << testName << "... ";
        HRESULT result = runConfigTestInThread(configuration, expectedResult);
        log::printResult(result);
    }

    HRESULT GenericUnitTester::shutDownTest() {
        scgms::IDevice_Event* shutDown = createEvent(scgms::NDevice_Event_Code::Shut_Down);
        if (shutDown == nullptr) {
            std::wcerr << L"Error while creating " << describeEvent(scgms::NDevice_Event_Code::Shut_Down) << std::endl;
            Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Shut_Down));
            return E_FAIL;
        }

        return m_testedFilter->Execute(shutDown);
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

    HRESULT GenericUnitTester::runConfigTestInThread(const std::string& configuration, const HRESULT expectedResult) {
        Logger::getInstance().debug(L"Running test in thread...");
        std::cv_status status;
        HRESULT result;

        {
            std::unique_lock<std::mutex> lock(m_testMutex);

            std::thread thread(&GenericUnitTester::runConfigTest, this, configuration, expectedResult);

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

    void GenericUnitTester::runTest(const std::function<HRESULT(void)>& test) {
        m_lastTestResult = test();
        m_testCv.notify_all();
    }

    void GenericUnitTester::runConfigTest(const std::string& configuration, const HRESULT expectedResult) {
        m_lastTestResult = configurationTest(configuration, expectedResult);
        m_testCv.notify_all();
    }

    bool GenericUnitTester::isFilterLoaded() {
        return m_testedFilter != nullptr;
    }


    //      **************************************************
    //                      Generic tests
    //      **************************************************


    HRESULT GenericUnitTester::infoEventTest() {
        return informativeEventsTest(scgms::NDevice_Event_Code::Information);
    }

    HRESULT GenericUnitTester::warningEventTest() {
        return informativeEventsTest(scgms::NDevice_Event_Code::Warning);
    }

    HRESULT GenericUnitTester::errorEventTest() {
        return informativeEventsTest(scgms::NDevice_Event_Code::Error);
    }

    HRESULT GenericUnitTester::warmResetEventTest() {
        return informativeEventsTest(scgms::NDevice_Event_Code::Warm_Reset);
    }

    HRESULT GenericUnitTester::shutDownEventTest() {
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
        infoEvent->Release();
        if (Succeeded(afterShutDownEventResult)) {
            Logger::getInstance().error(L"Execution of event after shutting down did not fail!");
            Logger::getInstance().error(std::wstring(L"Actual result: ") + Describe_Error(afterShutDownEventResult));
            return E_FAIL;
        } else {
            return S_OK;
        }
    }

    HRESULT GenericUnitTester::informativeEventsTest(const scgms::NDevice_Event_Code eventCode) {
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
            scgms::TDevice_Event* receivedEvent = m_testFilter.getRecievedEvent();
            if (receivedEvent->event_code == eventCode) {
                result = S_OK;
            } else {
                Logger::getInstance().error(L"Event was modified during execution!");
                Logger::getInstance().error(L"expected code: " + describeEvent(eventCode));
                Logger::getInstance().error(L"expected code: " + describeEvent(receivedEvent->event_code));
                result = E_FAIL;
            }
        } else {
            Logger::getInstance().error(std::wstring(L"expected result: ") + Describe_Error(S_OK));
            Logger::getInstance().error(std::wstring(L"actual result: ") + Describe_Error(result));
            result = E_FAIL;
        }

        if (eventCode != scgms::NDevice_Event_Code::Shut_Down) {
            event->Release();
            shutDownTest();
        }
        return result;
    }

    HRESULT GenericUnitTester::configurationTest(const std::string &memory, const HRESULT expectedResult) {
        if (!isFilterLoaded()) {
            std::wcerr << L"No filter loaded! Can't execute test.\n";
            Logger::getInstance().error(L"No filter loaded! Can't execute test.");
            return E_FAIL;
        }

        scgms::SPersistent_Filter_Chain_Configuration configuration;
        refcnt::Swstr_list errors;

        HRESULT result = configuration ? S_OK : E_FAIL;
        if (result == S_OK) {
            configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
            Logger::getInstance().info(L"Loading configuration from memory...");
        } else {
            Logger::getInstance().error(L"Error while creating configuration!");
            return E_FAIL;
        }

        scgms::IFilter_Configuration_Link** begin, ** end;
        configuration->get(&begin, &end);

        Logger::getInstance().info(L"Configuring filter...");
        result = m_testedFilter->Configure(begin[0], errors.get());

        HRESULT testResult;
        if (result == expectedResult) {
            testResult = S_OK;
        } else {
            Logger::getInstance().error(L"Provided configuration:\n"
                                        L"(" + std::wstring(memory.begin(), memory.end()) + L")");
            Logger::getInstance().error(std::wstring(L"expected configuration result: ") + Describe_Error(expectedResult));
            Logger::getInstance().error(std::wstring(L"actual configuration result: ") + Describe_Error(result));
            testResult = E_FAIL;
        }

        shutDownTest();
        return testResult;
    }

    CDynamic_Library &GenericUnitTester::getFilterLib() {
        return m_filterLibrary;
    }

    TestFilter &GenericUnitTester::getTestFilter() {
        return m_testFilter;
    }

    scgms::IFilter *GenericUnitTester::getTestedFilter() {
        return m_testedFilter;
    }
}
