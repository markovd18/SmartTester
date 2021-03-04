#include <iostream>
#include <rtl/FilterLib.h>
#include <rtl/hresult.h>
#include <utils/string_utils.h>
#include "../LogFilterUnitTester.h"
#include "../../utils/UnitTestExecUtils.h"
#include "../FilterConfiguration.h"
#include "../../utils/scgmsLibUtils.h"

namespace tester {

    const std::string LogFilterUnitTester::FILTER_CONFIG = "[Filter_001_{c0e942b9-3928-4b81-9b43-a347668200ba}]";
    const std::string LogFilterUnitTester::LOG_FILE_GENERATION_TEST_LOG = "logFileGenerationTestLog.log";
    const std::string LogFilterUnitTester::CORRECT_LOG_FILE_NAME_TEST_LOG = "correctLogFileNameTestLog.log";

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
        moveToTmp(CORRECT_LOG_FILE_NAME_TEST_LOG.c_str());    /// the test above should create junk file so we delete it

        /// Functional tests
        executeTest(L"log file generation test", std::bind(&LogFilterUnitTester::logFileGenerationTest, this));
        moveToTmp(LOG_FILE_GENERATION_TEST_LOG);    /// Test creates new log, moving it to tmp
    }


    HRESULT LogFilterUnitTester::logFileGenerationTest() {
        if (!isFilterLoaded()) {
            std::wcerr << L"No filter loaded! Can't execute test.\n";
            Logger::getInstance().error(L"No filter loaded! Can't execute test!");
            return E_FAIL;
        }

        scgms::SPersistent_Filter_Chain_Configuration configuration;
        refcnt::Swstr_list errors;

        HRESULT result = configuration ? S_OK : E_FAIL;
        if (!Succeeded(result)) {
            Logger::getInstance().error(L"Error while creating configuration!");
            shutDownTest();
            return E_FAIL;
        }

        tester::LogFilterConfig config(LOG_FILE_GENERATION_TEST_LOG);
        std::string memory = config.toString();
        configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
        Logger::getInstance().info(L"Loading configuration from memory...");

        scgms::IFilter_Configuration_Link** begin, ** end;
        configuration->get(&begin, &end);

        result = getTestedFilter()->Configure(begin[0], errors.get());
        Logger::getInstance().info(L"Configuring filter...");

        if (Succeeded(result)) {
            scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Level);
            if (!event) {
                result = E_FAIL;
            } else {
                getTestedFilter()->Execute(event);
                if (filesystem::exists(LOG_FILE_GENERATION_TEST_LOG)) {
                    Logger::getInstance().debug(L"Log file created successfully!");
                    result = S_OK;

                    moveToTmp(LOG_FILE_GENERATION_TEST_LOG);
                } else {
                    Logger::getInstance().error(L"Failed to create log file!");
                    result = E_FAIL;
                }
            }
        } else {
            Logger::getInstance().error(L"Failed to configure filter:!\n"
                                        L"(" + std::wstring(memory.begin(), memory.end()) + L")");
            Logger::getInstance().error(std::wstring(L"expected result: ") + Describe_Error(S_OK));
            Logger::getInstance().error(std::wstring(L"actual result: ") + Describe_Error(result));
            result = E_FAIL;
        }

        shutDownTest();
        return result;
    }
}
