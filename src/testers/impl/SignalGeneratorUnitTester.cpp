//
// Author: markovd@students.zcu.cz
//

#include "../SignalGeneratorUnitTester.h"
#include "../../utils/LogUtils.h"
#include "../../utils/scgmsLibUtils.h"

constexpr GUID INVALID_MODEL_GUID = { 0x811122a6, 0xb4b2, 0x457d, {0xc0, 0x29, 0x62, 0x5c, 0xbd, 0xb6, 0x82, 0xef} };
constexpr GUID INVALID_SIGNAL_ID_GUID = {0xe1cd0700, 0xb079, 0x4911, {0xb7, 0x9b, 0xd2, 0x03, 0x48, 0x61, 0x01, 0xc8}};

tester::SignalGeneratorUnitTester::SignalGeneratorUnitTester() : FilterUnitTester(cnst::SIGNAL_GEN_GUID, EntityType::FILTER) {
    //
}

void tester::SignalGeneratorUnitTester::executeSpecificTests() {
    ///Configuration tests
    tester::SignalGeneratorConfig config;
    executeConfigTest(L"empty configuration test", config, E_INVALIDARG);

    config.setFeedbackName("fb1");
    config.setTimeSegmentId(1);
    config.setMaximumTime(0.5);
    config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});
    config.setStepping(0.035);
    config.setSynchronizeToSignal(true);
    config.setSynchronizationSignal(scgms::signal_BG);
    config.setModelId(INVALID_MODEL_GUID);
    executeConfigTest(L"invalid model id test", config, E_INVALIDARG);

    config.setModelId(cnst::BERGMAN_MODEL_GUID);
    executeConfigTest(L"bergman model id test", config, S_OK);
/// When signal synchronization set to false, crashes on std::invalid_logic exception thrown in the filter
//    config.setSynchronizeToSignal(false);
//    config.setSynchronizationSignal(Invalid_GUID);
//    executeConfigTest(L"no signal synchronization test", config, S_OK);
//
//    config.setSynchronizeToSignal(true);
//    executeConfigTest(L"invalid synchronization signal test", config, E_INVALIDARG);

    config.setSynchronizationSignal(INVALID_SIGNAL_ID_GUID);
    executeConfigTest(L"not existing synchronization signal test", config, E_INVALIDARG);

    config.setSynchronizationSignal(scgms::signal_BG);
    executeConfigTest(L"valid synchronization signal test", config, S_OK);

    config.setSynchronizationSignal(scgms::signal_All);
    executeConfigTest(L"all synchronization signal test", config, S_OK);

    config.setSynchronizationSignal(scgms::signal_Null);
    executeConfigTest(L"null synchronization signal test", config, S_OK);

    config.setStepping(-5);
    executeConfigTest(L"negative stepping test", config, E_INVALIDARG);

    config.setStepping(0);
    executeConfigTest(L"zero stepping test", config, S_OK);

    config.setStepping(0.0354);
    executeConfigTest(L"positive stepping test", config, S_OK);

    config.setMaximumTime(-5);
    executeConfigTest(L"negative maximum time test", config, E_INVALIDARG);

    config.setMaximumTime(1);
    executeConfigTest(L"positive maximum time test", config, S_OK);

    config.setStepping(0.0354);
    config.setMaximumTime(0.001);
    executeConfigTest(L"less than stepping maximum time test", config, E_INVALIDARG);

    /// Functional tests
//    executeTest(L"asynchronous mode test",std::bind(&SignalGeneratorUnitTester::asynchronousModeTest, this));
    executeTest(L"time segment start test", std::bind(&SignalGeneratorUnitTester::timeSegmentStartTest, this));
    executeTest(L"twice identical time segment start test", std::bind(&SignalGeneratorUnitTester::twiceIdenticalTimeSegmentStartTest, this));
    executeTest(L"event time stepped by less than stepping test", std::bind(&SignalGeneratorUnitTester::eventDeviceTimeLessThanSteppingTest, this));
    executeTest(L"event time as stepping test", std::bind(&SignalGeneratorUnitTester::eventDeviceTimeAsSteppingTest, this));
    executeTest(L"event time as several steppings test", std::bind(&SignalGeneratorUnitTester::eventDeviceTimeAsSeveralSteppingsTest, this));
    executeTest(L"echo default parameters as event test", std::bind(&SignalGeneratorUnitTester::echoDefaultParametersAsEventTest, this));
}

HRESULT tester::SignalGeneratorUnitTester::asynchronousModeTest() {
    //TODO otestovat, až bude opravena chyba ve SmartCGMS, které padá na std::logic_error
    tester::SignalGeneratorConfig config;
    config.setFeedbackName("fb1");
    config.setTimeSegmentId(1);
    config.setMaximumTime(scgms::One_Hour);
    config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});
    config.setStepping(5 * scgms::One_Minute);
    config.setSynchronizeToSignal(false);
    config.setSynchronizationSignal(Invalid_GUID);
    config.setModelId(cnst::BERGMAN_MODEL_GUID);

    HRESULT configurationResult = configureFilter(config);
    if (!Succeeded(configurationResult)) {
        logs::logConfigurationError(config, S_OK, configurationResult);
        return E_FAIL;
    }

    if ((getTestFilter().getReceivedEventsCount() % (std::size_t)((config.getMaximumTime() / config.getStepping() + 1)) != 0)) {
        Logger::getInstance().error(L"Unexpected number of events emitted!");
        Logger::getInstance().error(L"Expected count: n * " + std::to_wstring((int)(config.getMaximumTime() / config.getStepping() + 1)));
        Logger::getInstance().error(L"Actual count: " + std::to_wstring(getTestFilter().getReceivedEventsCount()));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Expected number of states emitted.");
    return S_OK;
}

HRESULT tester::SignalGeneratorUnitTester::shutdownAfterLastTest() {
    //TODO otestovat, až bude opravena chyba ve SmartCGMS, které padá na std::logic_error
    tester::SignalGeneratorConfig config;
    config.setFeedbackName("fb1");
    config.setTimeSegmentId(1);
    config.setMaximumTime(scgms::One_Hour);
    config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});
    config.setStepping(5 * scgms::One_Minute);
    config.setSynchronizeToSignal(false);
    config.setSynchronizationSignal(Invalid_GUID);
    config.setModelId(cnst::BERGMAN_MODEL_GUID);
    config.setShutDownAfterLast(true);

    HRESULT configurationResult = configureFilter(config);
    if (!Succeeded(configurationResult)) {
        logs::logConfigurationError(config, S_OK, configurationResult);
        return E_FAIL;
    }

    scgms::TDevice_Event lastEvent = getTestFilter().getLastReceivedEvent();
    if (lastEvent.event_code != scgms::NDevice_Event_Code::Shut_Down) {
        Logger::getInstance().error(describeEvent(scgms::NDevice_Event_Code::Shut_Down) + L" was not emitted as last event!");
        Logger::getInstance().error(L"Actual last event: " + describeEvent(lastEvent.event_code));
        return E_FAIL;
    }

    Logger::getInstance().info(describeEvent(scgms::NDevice_Event_Code::Shut_Down) + L" was emitted as the last event.");
    return S_OK;
}

HRESULT tester::SignalGeneratorUnitTester::timeSegmentStartTest() {
    tester::SignalGeneratorConfig config;
    config.setFeedbackName("fb1");
    config.setTimeSegmentId(1);
    config.setMaximumTime(0.5);
    config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});
    config.setStepping(5 * scgms::One_Minute);
    config.setSynchronizeToSignal(true);
    config.setSynchronizationSignal(scgms::signal_BG);
    config.setModelId(cnst::BERGMAN_MODEL_GUID);

    HRESULT configurationResult = configureFilter(config);
    if (!Succeeded(configurationResult)) {
        logs::logConfigurationError(config, S_OK, configurationResult);
        return E_FAIL;
    }

    scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Time_Segment_Start);
    if (!event) {
        Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
        return E_FAIL;
    }

    scgms::TDevice_Event *rawEvent;
    event->Raw(&rawEvent);
    rawEvent->segment_id = 1;   /// Setting event's segment id to a valid value
    HRESULT execResult = getTestedEntity()->Execute(event);
    if (!Succeeded(execResult)) {
        Logger::getInstance().warn(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
        Logger::getInstance().warn(std::wstring(L"Execution result: ") + Describe_Error(execResult));
    }

    /// Generator should emit current state when registers time segment start - no events is an error
    std::size_t receivedEventsCount = getTestFilter().getReceivedEventsCount();
    if (receivedEventsCount == 0 || receivedEventsCount == 1) {
        Logger::getInstance().error(L"Generator did not emit current state! No events were emitted.");
        return E_FAIL;
    }

    Logger::getInstance().info(L"Generator successfully emitted current state.");
    return S_OK;
}

HRESULT tester::SignalGeneratorUnitTester::twiceIdenticalTimeSegmentStartTest() {
    tester::SignalGeneratorConfig config;
    config.setFeedbackName("fb1");
    config.setTimeSegmentId(1);
    config.setMaximumTime(0.5);
    config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});
    config.setStepping(0.003472222222222222);
    config.setSynchronizeToSignal(true);
    config.setSynchronizationSignal(scgms::signal_BG);
    config.setModelId(cnst::BERGMAN_MODEL_GUID);

    HRESULT configurationResult = configureFilter(config);
    if (!Succeeded(configurationResult)) {
        logs::logConfigurationError(config, S_OK, configurationResult);
        return E_FAIL;
    }

    for (int i = 0; i < 2; ++i) {   /// Time segment start for the same segment_id should return an error
        scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Time_Segment_Start);
        if (!event) {
            Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
            return E_FAIL;
        }

        scgms::TDevice_Event *rawEvent;
        event->Raw(&rawEvent);
        rawEvent->segment_id = 1;   /// Setting event's segment id to a valid value
        HRESULT execResult = getTestedEntity()->Execute(event);
        if (i == 0 && !Succeeded(execResult)) {
            Logger::getInstance().warn(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
            Logger::getInstance().warn(std::wstring(L"Execution result: ") + Describe_Error(execResult));
        } else if (i == 1 && Succeeded(execResult)) {
            Logger::getInstance().error(L"Executing twice " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start) +
                                            L" with the same segment_id did not fail!");
            Logger::getInstance().error(std::wstring(L"Execution result: ") + Describe_Error(execResult));
            return E_FAIL;
        }
    }

    Logger::getInstance().info(L"Executing twice " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start) +
                                L" with the same segment_id failed.");
    return S_OK;
}

HRESULT tester::SignalGeneratorUnitTester::eventDeviceTimeLessThanSteppingTest() {
    tester::SignalGeneratorConfig config;
    config.setFeedbackName("fb1");
    config.setTimeSegmentId(1);
    config.setMaximumTime(0.5);
    config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});

    config.setSynchronizeToSignal(true);
    config.setSynchronizationSignal(scgms::signal_BG);
    config.setModelId(cnst::BERGMAN_MODEL_GUID);

    double stepping = 5 * scgms::One_Minute;
    config.setStepping(stepping);   /// Stepping 5 minutes
    Logger::getInstance().info(L"Configuring signal generator with stepping " + std::to_wstring(stepping));

    HRESULT configurationResult = configureFilter(config);
    if (!Succeeded(configurationResult)) {
        logs::logConfigurationError(config, S_OK, configurationResult);
        return E_FAIL;
    }

    scgms::IDevice_Event *segmentStartEvent = createEvent(scgms::NDevice_Event_Code::Time_Segment_Start);
    if (!segmentStartEvent) {
        Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
        return E_FAIL;
    }

    scgms::TDevice_Event *rawEvent;
    segmentStartEvent->Raw(&rawEvent);
    rawEvent->segment_id = 1;   /// Setting segmentStartEvent's segment id to a valid value
    rawEvent->device_time = 0.1;    /// Starting at this time
    rawEvent->signal_id = scgms::signal_BG;
    HRESULT execResult = getTestedEntity()->Execute(segmentStartEvent);
    if (!Succeeded(execResult)) {
        Logger::getInstance().warn(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
        Logger::getInstance().warn(std::wstring(L"Execution result: ") + Describe_Error(execResult));
    }

    /// Generator emits current state when registers time segment start
    std::size_t currentStateEventCount = getTestFilter().getReceivedEventsCount();

    scgms::IDevice_Event *levelEvent = createEvent(scgms::NDevice_Event_Code::Level);
    if (!levelEvent) {
        Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
        return E_FAIL;
    }

    levelEvent->Raw(&rawEvent);
    rawEvent->segment_id = 1;   /// setting level event's segment id to the same value as the previous event
    rawEvent->device_time = 0.1 + scgms::One_Minute;    /// With stepping 5 minutes, adding one minute should not return any event
    rawEvent->signal_id = scgms::signal_BG;
    Logger::getInstance().info(L"Executing event stepped by " + std::to_wstring(scgms::One_Minute));
    HRESULT levelExecResult = getTestedEntity()->Execute(levelEvent);
    if (!Succeeded(levelExecResult)) {
        Logger::getInstance().warn(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Level));
        Logger::getInstance().warn(std::wstring(L"Execution result: ") + Describe_Error(execResult));
    }

    /// We added less time than the stepping is, so the event should not be emitted, +1 for the executed event
    if (getTestFilter().getReceivedEventsCount() != (currentStateEventCount + 1)) {
        Logger::getInstance().error(L"Received event count after executing " + describeEvent(scgms::NDevice_Event_Code::Level) +
                                    L" does not match received event count after last " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
        Logger::getInstance().error(L"Expected event count: " + std::to_wstring(currentStateEventCount + 1));
        Logger::getInstance().error(L"Actual event count: " + std::to_wstring(getTestFilter().getReceivedEventsCount()));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Event was not emitted.");
    return S_OK;
}

HRESULT tester::SignalGeneratorUnitTester::eventDeviceTimeAsSteppingTest() {
    tester::SignalGeneratorConfig config;
    config.setFeedbackName("fb1");
    config.setTimeSegmentId(1);
    config.setMaximumTime(0.5);
    config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});

    config.setSynchronizeToSignal(true);
    config.setSynchronizationSignal(scgms::signal_BG);
    config.setModelId(cnst::BERGMAN_MODEL_GUID);

    double stepping = 5 * scgms::One_Minute;
    config.setStepping(stepping);   /// Stepping 5 minutes
    Logger::getInstance().info(L"Configuring signal generator with stepping " + std::to_wstring(stepping));

    HRESULT configurationResult = configureFilter(config);
    if (!Succeeded(configurationResult)) {
        logs::logConfigurationError(config, S_OK, configurationResult);
        return E_FAIL;
    }

    scgms::IDevice_Event *segmentStartEvent = createEvent(scgms::NDevice_Event_Code::Time_Segment_Start);
    if (!segmentStartEvent) {
        Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
        return E_FAIL;
    }

    scgms::TDevice_Event *rawEvent;
    segmentStartEvent->Raw(&rawEvent);
    rawEvent->segment_id = 1;   /// Setting segmentStartEvent's segment id to a valid value
    rawEvent->device_time = 0.1;    /// Starting at this time
    rawEvent->signal_id = scgms::signal_BG;
    HRESULT execResult = getTestedEntity()->Execute(segmentStartEvent);
    if (!Succeeded(execResult)) {
        Logger::getInstance().warn(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
        Logger::getInstance().warn(std::wstring(L"Execution result: ") + Describe_Error(execResult));
    }

    /// Generator emits current state when registers time segment start
    std::size_t currentStateEventCount = getTestFilter().getReceivedEventsCount();

    scgms::IDevice_Event *levelEvent = createEvent(scgms::NDevice_Event_Code::Level);
    if (!levelEvent) {
        Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
        return E_FAIL;
    }

    levelEvent->Raw(&rawEvent);
    rawEvent->segment_id = 1;   /// setting level event's segment id to the same value as the previous event
    rawEvent->device_time = 0.1 + 7 * scgms::One_Minute;    /// With stepping 5 minutes, adding seven minutes should call the step method
    rawEvent->signal_id = scgms::signal_BG;

    Logger::getInstance().info(L"Executing event stepped by " + std::to_wstring(6 * scgms::One_Minute));
    HRESULT levelExecResult = getTestedEntity()->Execute(levelEvent);
    if (!Succeeded(levelExecResult)) {
        Logger::getInstance().warn(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Level));
        Logger::getInstance().warn(std::wstring(L"Execution result: ") + Describe_Error(execResult));
    }

    if (getTestFilter().getReceivedEventsCount() == currentStateEventCount ||
        getTestFilter().getReceivedEventsCount() == (currentStateEventCount + 1)) {
        Logger::getInstance().error(L"Next state after stepping was not emitted!");
        return E_FAIL;
    }

    Logger::getInstance().info(L"Next state after stepping was emitted.");
    return S_OK;
}

HRESULT tester::SignalGeneratorUnitTester::eventDeviceTimeAsSeveralSteppingsTest() {
    tester::SignalGeneratorConfig config;
    config.setFeedbackName("fb1");
    config.setTimeSegmentId(1);
    config.setMaximumTime(0.5);
    config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});

    config.setSynchronizeToSignal(true);
    config.setSynchronizationSignal(scgms::signal_BG);
    config.setModelId(cnst::BERGMAN_MODEL_GUID);

    double stepping = 5 * scgms::One_Minute;
    config.setStepping(stepping);   /// Stepping 5 minutes
    Logger::getInstance().info(L"Configuring signal generator with stepping " + std::to_wstring(stepping));

    HRESULT configurationResult = configureFilter(config);
    if (!Succeeded(configurationResult)) {
        logs::logConfigurationError(config, S_OK, configurationResult);
        return E_FAIL;
    }

    scgms::IDevice_Event *segmentStartEvent = createEvent(scgms::NDevice_Event_Code::Time_Segment_Start);
    if (!segmentStartEvent) {
        Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
        return E_FAIL;
    }

    scgms::TDevice_Event *rawEvent;
    segmentStartEvent->Raw(&rawEvent);
    rawEvent->segment_id = 1;   /// Setting segmentStartEvent's segment id to a valid value
    rawEvent->device_time = 0.1;    /// Starting at this time
    rawEvent->signal_id = scgms::signal_BG;

    HRESULT execResult = getTestedEntity()->Execute(segmentStartEvent);
    if (!Succeeded(execResult)) {
        Logger::getInstance().warn(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
        Logger::getInstance().warn(std::wstring(L"Execution result: ") + Describe_Error(execResult));
    }

    /// Generator emits current state when registers time segment start
    std::size_t currentStateEventCount = getTestFilter().getReceivedEventsCount() - 1;  // - 1 for the time segment start event
    Logger::getInstance().info(L"Event count representing one current state: " + std::to_wstring(currentStateEventCount));
    getTestFilter().clearReceivedEvents();  /// Clearing events so we can easily check incoming event count after next execution

    scgms::IDevice_Event *levelEvent = createEvent(scgms::NDevice_Event_Code::Level);
    if (!levelEvent) {
        Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
        return E_FAIL;
    }

    levelEvent->Raw(&rawEvent);
    rawEvent->segment_id = 1;   /// setting level event's segment id to the same value as the previous event
    rawEvent->device_time = 0.1 + 21 * scgms::One_Minute;    /// With stepping 5 minutes, adding twenty minutes should emit states
    rawEvent->signal_id = scgms::signal_BG;                  /// in several other times

    Logger::getInstance().info(L"Executing event stepped by " + std::to_wstring(6 * scgms::One_Minute));
    HRESULT levelExecResult = getTestedEntity()->Execute(levelEvent);
    if (!Succeeded(levelExecResult)) {
        Logger::getInstance().warn(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Level));
        Logger::getInstance().warn(std::wstring(L"Execution result: ") + Describe_Error(execResult));
    }

    if (getTestFilter().getReceivedEventsCount() < (currentStateEventCount * 4)) {  /// states in 4 times should be emitted
        Logger::getInstance().error(L"States throughout the stepping were not emitted!");
        Logger::getInstance().error(L"Multiples of stepping in event's device time: " + std::to_wstring(4));
        Logger::getInstance().error(L"Emitted events: " + std::to_wstring(getTestFilter().getReceivedEventsCount()));
        return E_FAIL;
    }

    Logger::getInstance().info(L"All states throughout the stepping were emitted.");
    return S_OK;
}

HRESULT tester::SignalGeneratorUnitTester::echoDefaultParametersAsEventTest() {
    tester::SignalGeneratorConfig config;
    config.setFeedbackName("fb1");
    config.setTimeSegmentId(1);
    config.setMaximumTime(0.5);
    config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});

    config.setSynchronizeToSignal(true);
    config.setSynchronizationSignal(scgms::signal_BG);
    config.setModelId(cnst::BERGMAN_MODEL_GUID);
    config.setStepping(5 * scgms::One_Minute);   /// Stepping 5 minutes
    config.setEchoDefaultParametersAsEvent(true);

    HRESULT configurationResult = configureFilter(config);
    if (!Succeeded(configurationResult)) {
        logs::logConfigurationError(config, S_OK, configurationResult);
        return E_FAIL;
    }

    scgms::IDevice_Event *segmentStartEvent = createEvent(scgms::NDevice_Event_Code::Time_Segment_Start);
    if (!segmentStartEvent) {
        Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
        return E_FAIL;
    }

    scgms::TDevice_Event *rawEvent;
    segmentStartEvent->Raw(&rawEvent);
    rawEvent->segment_id = 1;   /// Setting segmentStartEvent's segment id to a valid value
    rawEvent->device_time = 0.1;    /// Starting at this time
    HRESULT execResult = getTestedEntity()->Execute(segmentStartEvent);
    if (!Succeeded(execResult)) {
        Logger::getInstance().warn(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Time_Segment_Start));
        Logger::getInstance().warn(std::wstring(L"Execution result: ") + Describe_Error(execResult));
    }

    scgms::TDevice_Event firstEvent = getTestFilter().getFirstReceivedEvent();
    if (firstEvent.event_code != scgms::NDevice_Event_Code::Parameters) {
        Logger::getInstance().error(L"Event with default parameters was not emitted as first!");
        Logger::getInstance().error(L"First emitted event: " + describeEvent(firstEvent.event_code));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Event with default parameters was emitted as first event.");
    return S_OK;
}

void tester::SignalGeneratorUnitTester::loadEntity() {
    if (!getEntityLib().Is_Loaded()) {
        std::wcerr << L"Filter library is not loaded! Filter will not be loaded.\n";
        Logger::getInstance().error(L"Filter library is not loaded! Filter will not be loaded.");
        return;
    }

    if (getTestedEntity()) {
        setTestedEntity(nullptr);
    }

    getTestFilter().clearReceivedEvents();     /// Resetting so there will not be data from last test present
    scgms::IFilter *testedFilter;
    HRESULT creationResult = constructEntity<scgms::TCreate_Filter>(getEntityLib(), "do_create_filter",
                                                                    &getEntityGuid(), &getTestFilter(), &testedFilter);
    if (creationResult != S_OK) {
        Logger::getInstance().error(L"Error while loading filter from the dynamic library!");
    } else {
        tester::SignalGeneratorConfig config;
        config.setFeedbackName("fb1");
        config.setTimeSegmentId(1);
        config.setMaximumTime(0.5);
        config.setParameters({0.005000, 0.005000, 0.000000, 0.100000, 0.001000, 0.001000, 8.000000, 10.000000, 0.050000, 0.010000, 0.010000, 0.010000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.500000, -10.000000, 0.000000, -1.000000, 0.000000, 0.050000, 0.028735, 0.028344, 0.000050, 0.300000, 0.100000, 0.100000, 12.000000, 70.000000, 0.220000, 0.050000, 0.050000, 0.040000, 95.000000, 9.200000, 220.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 95.000000, 0.000000, 0.929000, -0.037000, 0.000000, 0.023310, 0.000000, 0.018600, 0.950000, 0.100000, 0.100000, 0.050000, 1.000000, 0.300000, 0.300000, 18.000000, 100.000000, 1.000000, 1.000000, 1.000000, 1.000000, 200.000000, 20.000000, 300.000000, 300.000000, 100.000000, 200.000000, 150.000000, 150.000000, 50.000000, 300.000000, 5.000000, 2.000000, 0.000000, 5.000000, 0.041667, 0.000000, 0.041667, 1.500000});

        config.setSynchronizeToSignal(true);
        config.setSynchronizationSignal(scgms::signal_BG);
        config.setModelId(cnst::BERGMAN_MODEL_GUID);

        double stepping = 5 * scgms::One_Minute;
        config.setStepping(stepping);
        setTestedEntity(testedFilter);
        Logger::getInstance().info(L"Filter loaded from dynamic library.");

        HRESULT configResult = configureFilter(config);
        if (!Succeeded(configResult)) {
            logs::logConfigurationError(config, configResult, S_OK);
        }
    }
}
