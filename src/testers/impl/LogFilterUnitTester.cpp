#include <iostream>
#include <rtl/FilterLib.h>
#include <rtl/hresult.h>
#include "../LogFilterUnitTester.h"
#include "../../utils/UnitTestExecUtils.h"
#include "../../utils/scgmsLibUtils.h"
#include "../../utils/LogUtils.h"

namespace tester {
    const char* LOG_FILE_GENERATION_TEST_LOG = "logFileGenerationTestLog.csv";
    const char* CORRECT_LOG_FILE_NAME_TEST_LOG = "correctLogFileNameTestLog.csv";
    const char* LOG_ROW_COUNT_TEST_LOG = "logRowCountTestLog.csv";
    const char* FILTER_OUTPUT_TEST_LOG = "filterOutputTestLog.csv";
    const char* EVENT_ORDER_TEST_LOG = "eventOrderTestLog.csv";
    const char* POP_RESULT_REPEATING_TEST_LOG = "pushResultRepeatingTestLog.csv";
    const char* POP_EVENT_COUNT_TEST_LOG = "popEventCountTEstLog.csv";

    LogFilterUnitTester::LogFilterUnitTester(const GUID& guid) : GenericUnitTester(guid){
        //
    }

    /**
        Executes all tests specific to filter tested by this UnitTester.
    */
    void LogFilterUnitTester::executeSpecificTests() {
        Logger::getInstance().info(L"Executing specific tests...");

        /// Configuration tests
        tester::LogFilterConfig config;
        executeConfigTest(L"empty log file name test", config, E_INVALIDARG);

        config.setLogFile(CORRECT_LOG_FILE_NAME_TEST_LOG);
        executeConfigTest(L"correct log file name test", config, S_OK);
        moveToTmp(CORRECT_LOG_FILE_NAME_TEST_LOG);    /// the test above should create junk file so we delete it

        /// Functional tests
        executeTest(L"log file generation test", std::bind(&LogFilterUnitTester::logFileGenerationTest, this));
        moveToTmp(LOG_FILE_GENERATION_TEST_LOG);

        executeTest(L"log row count test", std::bind(&LogFilterUnitTester::logRowCountTest, this));
        moveToTmp(LOG_ROW_COUNT_TEST_LOG);

        executeTest(L"filter output test", std::bind(&LogFilterUnitTester::filterOutputTest, this));
        moveToTmp(FILTER_OUTPUT_TEST_LOG);

        executeTest(L"event order test", std::bind(&LogFilterUnitTester::eventOrderTest, this));
        moveToTmp(EVENT_ORDER_TEST_LOG);

        executeTest(L"pop result repeating test", std::bind(&LogFilterUnitTester::popResultRepeatingTest, this));
        moveToTmp(POP_RESULT_REPEATING_TEST_LOG);

        executeTest(L"pop event count test", std::bind(&LogFilterUnitTester::popEventCountTest, this));
        moveToTmp(POP_EVENT_COUNT_TEST_LOG);
    }


    HRESULT LogFilterUnitTester::logFileGenerationTest() {
        tester::LogFilterConfig config(LOG_FILE_GENERATION_TEST_LOG);
        HRESULT configResult = configureFilter(config);
        HRESULT testResult;

        if (Succeeded(configResult)) {  /// File should exist already after successful configuration
            if (filesystem::exists(LOG_FILE_GENERATION_TEST_LOG)) {
                Logger::getInstance().debug(L"Log file created successfully!");
                testResult = S_OK;
            } else {
                Logger::getInstance().error(L"Failed to create log file!");
                testResult = E_FAIL;
            }
        } else {
            log::logConfigurationError(config, S_OK, configResult);
            testResult = E_FAIL;
        }

        return testResult;
    }

    HRESULT LogFilterUnitTester::logRowCountTest() {
        tester::LogFilterConfig config(LOG_ROW_COUNT_TEST_LOG);

        HRESULT configResult = configureFilter(config);
        if (!Succeeded(configResult)) {
            log::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        std::ifstream logFile(LOG_ROW_COUNT_TEST_LOG);
        if (!logFile) {
            Logger::getInstance().error(L"Error while opening generated log file!");
            return E_FAIL;
        }

        scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Level);
        if (!event) {
            Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
            return E_FAIL;
        }

        for (int i = 0; i < 3; ++i) {
            getTestedFilter()->Execute(event);
        }

        event->Release();

        scgms::IDevice_Event *shutDown = createEvent(scgms::NDevice_Event_Code::Shut_Down);
        if (!shutDown) {
            Logger::getInstance().error(L"Error while creating shut down event during Log Filter testing!");
        } else {
            getTestedFilter()->Execute(shutDown);   /// Forcing logs to be displayed in the log
            shutDown->Release();
        }

        std::string logLine;
        int32_t logRows = 0;
        int32_t expectedRowCount = 5;
        while (std::getline(logFile, logLine)) {
            logRows++;
        }

        if (logRows != expectedRowCount) {
            Logger::getInstance().error(L"Log row count doesn't match the expected number of rows!");
            Logger::getInstance().error(L"Expected number: " + std::to_wstring(expectedRowCount) +
                                            L", actual number: " + std::to_wstring(logRows));
            return E_FAIL;
        }

        return S_OK;
    }

    HRESULT LogFilterUnitTester::filterOutputTest() {
        tester::LogFilterConfig config(FILTER_OUTPUT_TEST_LOG);

        HRESULT configResult = configureFilter(config);
        if (!Succeeded(configResult)) {
            log::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Level);
        if (!event) {
            Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
            return E_FAIL;
        }

        HRESULT execResult = getTestedFilter()->Execute(event);
        if (!Succeeded(execResult)) {
            Logger::getInstance().error(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Level));
            event->Release();
            return E_FAIL;
        }

        event->Release();

        try {
            std::vector<std::vector<std::string>> logLines = log::readLogFile(FILTER_OUTPUT_TEST_LOG);
            if (logLines.size() < 2) {
                Logger::getInstance().error(L"Log file does not contain a record of executed event!");
                return E_FAIL;
            }

            if (logLines[1][2] == "Level") {    /// Checking second line - first line is header, third field is event code
                Logger::getInstance().info(L"Level event record found!");
                return S_OK;
            } else {
                Logger::getInstance().error(L"Level event record not found!");
                return E_FAIL;
            }
        } catch (const std::runtime_error &error) {
            Logger::getInstance().error(L"Log file " + Widen_Char(FILTER_OUTPUT_TEST_LOG) + L" does not exist!");
            return E_FAIL;
        }
    }

    HRESULT LogFilterUnitTester::eventOrderTest() {
        tester::LogFilterConfig config(EVENT_ORDER_TEST_LOG);

        HRESULT configResult = configureFilter(config);
        if (!Succeeded(configResult)) {
            log::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        std::size_t eventCount = 3;
        scgms::IDevice_Event* events[eventCount];
        scgms::NDevice_Event_Code eventCode;
        for (std::size_t i = 0; i < eventCount; ++i) {
            switch (i) {
                case 0:
                    eventCode = scgms::NDevice_Event_Code::Level;
                    break;
                case 1:
                    eventCode = scgms::NDevice_Event_Code::Information;
                    break;
                default:
                    eventCode = scgms::NDevice_Event_Code::Parameters;
                    break;
            }

            events[i] = createEvent(eventCode);
            if (!events[i]) {
                Logger::getInstance().error(L"Error while creating " + describeEvent(eventCode));
                for (int j = i; j > 0; --j) {
                    events[i - 1]->Release();
                }
                return E_FAIL;
            }
        }

        HRESULT execResult;
        HRESULT testResult = S_OK;
        for (scgms::IDevice_Event *event : events) {
            execResult = getTestedFilter()->Execute(event);
            if (!Succeeded(execResult)) {
                testResult = E_FAIL;
                break;
            }
        }

        for (scgms::IDevice_Event *event : events) {
            event->Release();
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Error while executing test events!");
            return E_FAIL;
        }

        std::vector<std::vector<std::string>> logLines;
        try {
            logLines = log::readLogFile(EVENT_ORDER_TEST_LOG);
        } catch (const std::runtime_error &error) {
            Logger::getInstance().error(L"Log file " + Widen_Char(EVENT_ORDER_TEST_LOG) + L" does not exist!");
            return E_FAIL;
        }

        if (logLines.size() < (eventCount + 1)) {   /// +1 for the header
            Logger::getInstance().error(L"Log file does not contain a record of some executed event!");
            return E_FAIL;
        }

        std::string eventName;
        for (std::size_t i = 1; i < (eventCount + 1); ++i) {
            switch (i) {
                case 1:
                    eventName = "Level";
                    break;
                case 2:
                    eventName = "Information";
                    break;
                default:
                    eventName = "Parameters";
                    break;
            }

            if (logLines[i][2] != eventName) {  /// Checking second line - first line is header, third field is event code
                Logger::getInstance().error(Widen_String(eventName) + L" event record not found!");
                return E_FAIL;
            }
            Logger::getInstance().info(Widen_String(eventName) + L" event record found!");

        }

        return S_OK;
    }

    HRESULT LogFilterUnitTester::popResultRepeatingTest() {
        tester::LogFilterConfig config(POP_RESULT_REPEATING_TEST_LOG);

        HRESULT configResult = configureFilter(config);
        if (!Succeeded(configResult)) {
            log::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Level);
        if (!event) {
            Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
            return E_FAIL;
        }

        HRESULT execResult = getTestedFilter()->Execute(event);
        event->Release();
        if (!Succeeded(execResult)) {
            Logger::getInstance().error(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Level));
            return E_FAIL;
        }

        scgms::SLog_Filter_Inspection inspection(getTestedFilter());
        if (!inspection) {
            Logger::getInstance().error(L"Error while creating interface inspection!");
            return E_FAIL;
        }


        refcnt::Swstr_list list;
        HRESULT result = inspection.pop(list);
        if (!result) {
            Logger::getInstance().error(L"Executed event was not recognized by the log filter!");
            return E_FAIL;
        }

        result = inspection.pop(list);
        if (!result) {    /// Popping the same event for the second time should return S_FALSE
            Logger::getInstance().info(L"Executed event was not recognized for the second time.");
            return S_OK;
        }

        Logger::getInstance().error(L"Error while popping an event from a log filter!");
        Logger::getInstance().error(std::wstring(L"Expected result: ") + Describe_Error(S_FALSE) +
                                        L", actual result: " + Describe_Error(result));
        return E_FAIL;
    }

    HRESULT LogFilterUnitTester::popEventCountTest() {
        tester::LogFilterConfig config(POP_EVENT_COUNT_TEST_LOG);

        HRESULT configResult = configureFilter(config);
        if (!Succeeded(configResult)) {
            log::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        std::size_t eventCount = 3;
        scgms::IDevice_Event* events[eventCount];
        for (std::size_t i = 0; i < eventCount; ++i) {
            events[i] = createEvent(scgms::NDevice_Event_Code::Level);
            if (!events[i]) {
                Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
                for (int j = i; j > 0 ; --j) {
                    events[i - 1]->Release();
                }
                return E_FAIL;
            }

        }

        HRESULT execResult;
        HRESULT testResult = S_OK;
        for (scgms::IDevice_Event *event : events) {
            execResult = getTestedFilter()->Execute(event);
            if (!Succeeded(execResult)) {
                testResult = E_FAIL;
                break;
            }
        }

        for (scgms::IDevice_Event *event : events) {
            event->Release();
        }

        if (!Succeeded(testResult)) {
            Logger::getInstance().error(L"Error while executing test events!");
            return E_FAIL;
        }

        scgms::SLog_Filter_Inspection inspection(getTestedFilter());
        if (!inspection) {
            Logger::getInstance().error(L"Error while creating interface inspection!");
            return E_FAIL;
        }


        refcnt::Swstr_list list;
        HRESULT result = inspection.pop(list);
        if (!result) {
            Logger::getInstance().error(L"Executed events were not recognized by the log filter!");
            return E_FAIL;
        }

        std::size_t recordCount = 0;
        list.for_each([&recordCount](const std::wstring& record) { recordCount++; } );

        if (recordCount != eventCount) {
            Logger::getInstance().error(L"Invalid number of events recognized by the log filter!");
            Logger::getInstance().error(L"Expected amount: " + std::to_wstring(eventCount) +
                                        L", actual amount: " + std::to_wstring(recordCount));
            return E_FAIL;
        }

        Logger::getInstance().info(L"Expected number of events recognized by the log filter.");
        return S_OK;
    }
}
