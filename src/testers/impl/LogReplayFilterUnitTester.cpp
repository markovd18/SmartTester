//
// Author: markovd@students.zcu.cz
//

#include <rtl/FilterLib.h>
#include "../LogReplayFilterUnitTester.h"
#include "../../utils/constants.h"
#include "../../utils/LogUtils.h"
#include "../../utils/scgmsLibUtils.h"

tester::LogReplayFilterUnitTester::LogReplayFilterUnitTester() : FilterUnitTester(cnst::LOG_REPLAY_GUID){
    //
}

void tester::LogReplayFilterUnitTester::executeSpecificTests() {

    /// Configuration tests
    tester::LogReplayFilterConfig config;
    executeConfigTest(L"empty log file test", config, ERROR_FILE_NOT_FOUND);

    config.setLogFile("invalidLogReplay.csv");
    executeConfigTest(L"not existing log replay file test", config, ERROR_FILE_NOT_FOUND);

    config.setLogFile("../../testFiles");
    executeConfigTest(L"valid directory test", config, S_OK);

    config.setLogFile("../../testFiles/validLogReplay.csv");
    executeConfigTest(L"valid file test", config, S_OK);

    /// Functional test
    executeTest(L"invalid header log replay file test", std::bind(&LogReplayFilterUnitTester::invalidHeaderLogFileTest, this));
    executeTest(L"invalid body log replay file test", std::bind(&LogReplayFilterUnitTester::invalidBodyLogFileTest, this));
}

HRESULT tester::LogReplayFilterUnitTester::invalidHeaderLogFileTest() {
    tester::LogReplayFilterConfig config("../../testFiles/invalidHeaderLog.csv");
    return invalidLogFileTest(config, scgms::NDevice_Event_Code::Warning);
}

HRESULT tester::LogReplayFilterUnitTester::invalidBodyLogFileTest() {
    tester::LogReplayFilterConfig config("../../testFiles/invalidLogReplay.csv");
    return invalidLogFileTest(config, scgms::NDevice_Event_Code::Error);
}

HRESULT tester::LogReplayFilterUnitTester::invalidLogFileTest(const tester::LogReplayFilterConfig &config,
                                                              const scgms::NDevice_Event_Code &expectedCode) {
    Logger::getInstance().debug(L"Creating configuration...");
    scgms::SPersistent_Filter_Chain_Configuration configuration;
    if (!configuration) {
        Logger::getInstance().error(L"Error while creating configuration!");
        return E_FAIL;
    }

    refcnt::Swstr_list errors;
    std::string memory = config.toString();
    configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());

    Logger::getInstance().debug(L"Creating filter executor...");
    scgms::SFilter_Executor executor(configuration, nullptr, nullptr, errors, &getTestFilter());

    scgms::TDevice_Event event = getTestFilter().getReceivedEvent();
    if (event.event_code != expectedCode) {
        Logger::getInstance().error(L"Unexpected event sent after parsing invalid header!");
        Logger::getInstance().error(L"Expected event: " + describeEvent(expectedCode));
        Logger::getInstance().error(L"Actual event: " + describeEvent(event.event_code));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Expected event acquired.");
    return S_OK;
}




