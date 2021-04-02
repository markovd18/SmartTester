//
// Author: markovd@students.zcu.cz
//

#include <rtl/FilterLib.h>
#include "../LogReplayFilterUnitTester.h"
#include "../../utils/constants.h"
#include "../../utils/LogUtils.h"
#include "../../utils/scgmsLibUtils.h"

const char* VALID_LOG_REPLAY_PATH = "../../testFiles/validLogReplay.csv";
const char* VALID_LOG_REPLAYS_DIR_PATH = "../../testFiles/validLogReplays";

tester::LogReplayFilterUnitTester::LogReplayFilterUnitTester() : FilterUnitTester(cnst::LOG_REPLAY_GUID, EntityType::FILTER){
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
    executeTest(L"invalid header log replay file test", std::bind(&LogReplayFilterUnitTester::invalidHeaderLogFileTest, this));
    executeTest(L"invalid body log replay file test", std::bind(&LogReplayFilterUnitTester::invalidBodyLogFileTest, this));
    executeTest(L"emitted events count test", std::bind(&LogReplayFilterUnitTester::emittedEventCountTest, this));
    executeTest(L"emit shut down flag test", std::bind(&LogReplayFilterUnitTester::emitShutdownFlagTest, this));
    executeTest(L"log replay folder test", std::bind(&LogReplayFilterUnitTester::logReplayFolderTest, this));
    executeTest(L"filename as segment id test", std::bind(&LogReplayFilterUnitTester::filenameAsSegmentIdTest, this));
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
    errors.for_each([] (const std::wstring& error) { Logger::getInstance().error(error); });

    Logger::getInstance().debug(L"Creating filter executor...");
    scgms::SFilter_Executor executor(configuration, nullptr, nullptr, errors, &getTestFilter());
    if (!executor) {
        Logger::getInstance().error(L"Error while executing configuration!");
        errors.for_each([] (const std::wstring& error) { Logger::getInstance().error(error); });
        return E_FAIL;
    }

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
    if (event == nullptr) {
        Logger::getInstance().error(L"No event was sent by the filter!");
        return E_FAIL;
    }

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
    tester::LogReplayFilterConfig config(VALID_LOG_REPLAY_PATH);

    Logger::getInstance().debug(L"Creating configuration...");
    scgms::SPersistent_Filter_Chain_Configuration configuration;
    if (!configuration) {
        Logger::getInstance().error(L"Error while creating configuration!");
        return E_FAIL;
    }

    std::size_t loggedEventsCount = logs::getLoggedLinesCount(VALID_LOG_REPLAY_PATH);
    refcnt::Swstr_list errors;
    std::string memory = config.toString();
    configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
    errors.for_each([] (const std::wstring& error) { Logger::getInstance().error(error); });

    scgms::SFilter_Executor executor(configuration, nullptr, nullptr, errors, &getTestFilter());
    if (!executor) {
        Logger::getInstance().error(L"Error while executing configuration!");
        errors.for_each([] (const std::wstring& error) { Logger::getInstance().error(error); });
        return E_FAIL;
    }

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

    std::size_t receivedEvents = getTestFilter().getReceivedEventsCount();
    if (receivedEvents != loggedEventsCount) {
        Logger::getInstance().error(L"Unexpected number of events received!");
        Logger::getInstance().error(L"Expected count: " + std::to_wstring(loggedEventsCount));
        Logger::getInstance().error(L"Actual count: " + std::to_wstring(receivedEvents));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Expected number of events received.");
    return S_OK;

}

HRESULT tester::LogReplayFilterUnitTester::emitShutdownFlagTest() {
    tester::LogReplayFilterConfig config("../../testFiles/validLogReplay.csv");
    config.setEmitShutdown(true);

    Logger::getInstance().debug(L"Creating configuration...");
    scgms::SPersistent_Filter_Chain_Configuration configuration;
    if (!configuration) {
        Logger::getInstance().error(L"Error while creating configuration!");
        return E_FAIL;
    }

    refcnt::Swstr_list errors;
    std::string memory = config.toString();
    configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
    errors.for_each([] (const std::wstring& error) { Logger::getInstance().error(error); });

    scgms::SFilter_Executor executor(configuration, nullptr, nullptr, errors, &getTestFilter());
    executor->Terminate(TRUE);

    scgms::TDevice_Event lastEvent = getTestFilter().getLastReceivedEvent();
    if (lastEvent.event_code != scgms::NDevice_Event_Code::Shut_Down) {
        Logger::getInstance().error(L"Unexpected last event received!");
        Logger::getInstance().error(L"Expected code: " + describeEvent(scgms::NDevice_Event_Code::Shut_Down));
        Logger::getInstance().error(L"Actual code: " + describeEvent(lastEvent.event_code));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Shut down event acquired.");
    return S_OK;
}

HRESULT tester::LogReplayFilterUnitTester::logReplayFolderTest() {
    tester::LogReplayFilterConfig config(VALID_LOG_REPLAYS_DIR_PATH);

    Logger::getInstance().debug(L"Creating configuration...");
    scgms::SPersistent_Filter_Chain_Configuration configuration;
    if (!configuration) {
        Logger::getInstance().error(L"Error while creating configuration!");
        return E_FAIL;
    }

    std::size_t loggedLinesCount = 0;
    auto iterator = filesystem::directory_iterator(VALID_LOG_REPLAYS_DIR_PATH);
    for (const auto &item : iterator) {
        loggedLinesCount += logs::getLoggedLinesCount(item.path());
    }

    refcnt::Swstr_list errors;
    std::string memory = config.toString();
    configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
    errors.for_each([] (const std::wstring& error) { Logger::getInstance().error(error); });

    scgms::SFilter_Executor executor(configuration, nullptr, nullptr, errors, &getTestFilter());
    if (!executor) {
        Logger::getInstance().error(L"Error while executing configuration!");
        errors.for_each([] (const std::wstring& error) { Logger::getInstance().error(error); });
        return E_FAIL;
    }

    std::size_t receivedEventsCount = getTestFilter().getReceivedEventsCount();
    if (receivedEventsCount != loggedLinesCount) {
        Logger::getInstance().error(L"Unexpected number of events received from logs in " + Widen_Char(VALID_LOG_REPLAYS_DIR_PATH) + L" directory!");
        Logger::getInstance().error(L"Expected number of events: " + std::to_wstring(loggedLinesCount));
        Logger::getInstance().error(L"Actual number of events: " + std::to_wstring(receivedEventsCount));
        return E_FAIL;
    }

    Logger::getInstance().info(L"All log files correctly replayed.");
    return S_OK;
}

HRESULT tester::LogReplayFilterUnitTester::filenameAsSegmentIdTest() {
    tester::LogReplayFilterConfig config(VALID_LOG_REPLAYS_DIR_PATH);

    Logger::getInstance().debug(L"Creating configuration...");
    scgms::SPersistent_Filter_Chain_Configuration configuration;
    if (!configuration) {
        Logger::getInstance().error(L"Error while creating configuration!");
        return E_FAIL;
    }

    std::size_t replayedLogFiles = 0;
    auto iterator = filesystem::directory_iterator(VALID_LOG_REPLAYS_DIR_PATH);
    while (iterator != filesystem::end(iterator)) {
        replayedLogFiles++;
        iterator++;
    }

    refcnt::Swstr_list errors;
    std::string memory = config.toString();
    configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
    errors.for_each([] (const std::wstring& error) { Logger::getInstance().error(error); });

    scgms::SFilter_Executor executor(configuration, nullptr, nullptr, errors, &getTestFilter());
    if (!executor) {
        Logger::getInstance().error(L"Error while executing configuration!");
        errors.for_each([] (const std::wstring& error) { Logger::getInstance().error(error); });
        return E_FAIL;
    }

    std::size_t uniqueSegmentIdsCount = getTestFilter().getUniqueSegmentIdsFromReceivedEventsCount();
    if (uniqueSegmentIdsCount < replayedLogFiles) {
        Logger::getInstance().error(L"Unexpected number of unique segment id's acquired!");
        Logger::getInstance().error(L"Expected count: >= " + std::to_wstring(replayedLogFiles));
        Logger::getInstance().error(L"Actual count: " + std::to_wstring(uniqueSegmentIdsCount));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Expected number of unique signal id's acquired.");
    return S_OK;
}




