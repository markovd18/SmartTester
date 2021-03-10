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
        executeConfigTest(L"empty log file name test", config, E_FAIL);

        config.setLogFile(CORRECT_LOG_FILE_NAME_TEST_LOG);
        executeConfigTest(L"correct log file name test", config, S_OK);
        moveToTmp(CORRECT_LOG_FILE_NAME_TEST_LOG);    /// the test above should create junk file so we delete it

        /// Functional tests
        executeTest(L"log file generation test", std::bind(&LogFilterUnitTester::logFileGenerationTest, this));
        moveToTmp(LOG_FILE_GENERATION_TEST_LOG);

        executeTest(L"log row count test", std::bind(&LogFilterUnitTester::logRowCountTest, this));
        moveToTmp(LOG_ROW_COUNT_TEST_LOG);
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
        scgms::IDevice_Event *shutDown = createEvent(scgms::NDevice_Event_Code::Shut_Down);
        if (!shutDown) {
            Logger::getInstance().error(L"Error while creating shut down event during Log Filter testing!");
        } else {
            getTestedFilter()->Execute(shutDown);   /// Forcing logs to be displayed in the log
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
}
