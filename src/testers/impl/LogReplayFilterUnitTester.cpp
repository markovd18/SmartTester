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

//    /// Functional tests
//    executeTest(L"invalid header log replay file test", std::bind(&LogReplayFilterUnitTester::invalidHeaderLogFileTest, this));
//    executeTest(L"invalid body log replay file test", std::bind(&LogReplayFilterUnitTester::invalidBodyLogFileTest, this));
    executeTest(L"emitted events count test", std::bind(&LogReplayFilterUnitTester::emittedEventCountTest, this));

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

    scgms::IDevice_Event *shutDown = createEvent(scgms::NDevice_Event_Code::Shut_Down);
    if (!shutDown) {
        Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Shut_Down));
        return E_FAIL;
    }

    HRESULT execResult = executor->Execute(shutDown);
    if (!Succeeded(execResult)) {
        Logger::getInstance().error(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Shut_Down));
    }

    executor->Terminate(TRUE);

    const scgms::TDevice_Event *event = getTestFilter().getLastNonShutDownEvent();
    if (event->event_code != expectedCode) {
        Logger::getInstance().error(L"Unexpected event sent after parsing invalid file!");
        Logger::getInstance().error(L"Expected event: " + describeEvent(expectedCode));
        Logger::getInstance().error(L"Actual event: " + describeEvent(event->event_code));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Expected event acquired.");
    return S_OK;
}

HRESULT tester::LogReplayFilterUnitTester::emittedEventCountTest() {
    tester::LogReplayFilterConfig config("../../testFiles/validLogReplay.csv");
    Logger::getInstance().debug(L"Creating configuration...");
    scgms::SPersistent_Filter_Chain_Configuration configuration;
    if (!configuration) {
        Logger::getInstance().error(L"Error while creating configuration!");
        return E_FAIL;
    }

    refcnt::Swstr_list errors;
    std::string memory = config.toString();
    configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());

    scgms::SFilter_Executor executor(configuration, nullptr, nullptr, errors, &getTestFilter());
//    scgms::IDevice_Event *shutDown = createEvent(scgms::NDevice_Event_Code::Shut_Down);
//    if (!shutDown) {
//        Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Shut_Down));
//        return E_FAIL;
//    }
//
//    HRESULT execResult = executor->Execute(shutDown);
//    if (!Succeeded(execResult)) {
//        Logger::getInstance().error(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Shut_Down));
//    }
//    executor->Terminate(TRUE);

    std::size_t receivedEvents = getTestFilter().getReceivedEventsCount();
    if (receivedEvents != 5) {
        Logger::getInstance().error(L"Unexpected number of events received!");
        Logger::getInstance().error(L"Expected count: " + std::to_wstring(5));
        Logger::getInstance().error(L"Actual count: " + std::to_wstring(receivedEvents));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Expected number of events received.");
    return S_OK;

}




