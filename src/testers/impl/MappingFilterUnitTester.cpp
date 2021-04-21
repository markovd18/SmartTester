#include <iostream>
#include <rtl/FilterLib.h>
#include <utils/string_utils.h>
#include "../MappingFilterUnitTester.h"
#include "../../utils/scgmsLibUtils.h"
#include "../../utils/LogUtils.h"

namespace tester {

    const GUID INVALID_SIGNAL_ID_GUID = { 0xe1cd0715, 0xb079, 0x4911, {0xb7, 0x9b, 0xd2, 0x03, 0x48, 0x61, 0x01, 0xc8} };

    MappingFilterUnitTester::MappingFilterUnitTester() : FilterUnitTester(cnst::MAPPING_GUID, EntityType::FILTER) {
        //
    }

    void MappingFilterUnitTester::executeSpecificTests(){
        Logger::getInstance().info(L"Executing specific tests...");

        /// Configuration tests
        tester::MappingFilterConfig config;
        config.setSignalDstId(scgms::signal_BG);
        executeConfigTest(L"empty source id test", config, E_INVALIDARG);

        config.setSignalDstId(Invalid_GUID);
        config.setSignalSrcId(scgms::signal_COB);
        executeConfigTest(L"empty destination id test", config, E_INVALIDARG);

        config.setSignalDstId(INVALID_SIGNAL_ID_GUID);
        executeConfigTest(L"invalid destination id test", config, E_INVALIDARG);

        config.setSignalDstId(scgms::signal_Acceleration);
        config.setSignalSrcId(INVALID_SIGNAL_ID_GUID);
        executeConfigTest(L"invalid source id test", config, E_INVALIDARG);

        config.setSignalSrcId(scgms::signal_Air_Temperature);
        executeConfigTest(L"correct id's test", config, S_OK);

        config.setSignalSrcId(scgms::signal_All);
        executeConfigTest(L"all signal source id test", config, S_OK);

        config.setSignalSrcId(scgms::signal_Calibration);
        config.setSignalDstId(scgms::signal_Null);
        executeConfigTest(L"null signal destination id test", config, S_OK);

        config.setSignalSrcId(scgms::signal_Null);
        config.setSignalDstId(scgms::signal_Carb_Intake);
        executeConfigTest(L"null signal source id test", config, E_INVALIDARG);

        config.setSignalSrcId(scgms::signal_Carb_Ratio);
        config.setSignalDstId(scgms::signal_All);
        executeConfigTest(L"all signal destination id test", config, E_INVALIDARG);

        /// Functional tests
        executeTest(L"level event mapping test", std::bind(&MappingFilterUnitTester::levelEventMappingTest, this));
        executeTest(L"info event mapping test", std::bind(&MappingFilterUnitTester::infoEventMappingTest, this));
        executeTest(L"parameters event mapping test", std::bind(&MappingFilterUnitTester::parametersEventMappingTest, this));
        executeTest(L"not configured signal mapping test", std::bind(&MappingFilterUnitTester::notConfiguredSignalMappingTest, this));
        executeTest(L"null destination id test", std::bind(&MappingFilterUnitTester::nullDestinationIdTest, this));
        executeTest(L"different source id null destination id test", std::bind(&MappingFilterUnitTester::differentSrcIdNullDestinationIdTest, this));
        executeTest(L"all source id test", std::bind(&MappingFilterUnitTester::allSourceIdTest, this));
    }

    HRESULT MappingFilterUnitTester::levelEventMappingTest() {
        return eventMappingTest(tester::MappingFilterConfig(scgms::signal_Air_Temperature, scgms::signal_Acceleration),
                                scgms::NDevice_Event_Code::Level);
    }

    HRESULT MappingFilterUnitTester::infoEventMappingTest() {
        return eventMappingTest(tester::MappingFilterConfig(scgms::signal_Carb_Ratio, scgms::signal_Carb_Intake),
                                scgms::NDevice_Event_Code::Information);
    }

    HRESULT MappingFilterUnitTester::parametersEventMappingTest() {
        return eventMappingTest(tester::MappingFilterConfig(scgms::signal_BG, scgms::signal_COB),
                                scgms::NDevice_Event_Code::Parameters);
    }

    HRESULT MappingFilterUnitTester::eventMappingTest(const tester::MappingFilterConfig &config,
                                                      scgms::NDevice_Event_Code eventCode,
                                                      const GUID& signalId) {
        HRESULT configResult = configureFilter(config);

        if (!Succeeded(configResult)) {
            logs::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        scgms::IDevice_Event* event = createEvent(eventCode);
        if (!event) {
            Logger::getInstance().error(L"Error while creating " + describeEvent(eventCode));
            return E_FAIL;
        }

        scgms::TDevice_Event* raw_event;
        event->Raw(&raw_event);
        raw_event->signal_id = signalId == Invalid_GUID ? config.getSignalSrcId() : signalId;
        scgms::TDevice_Event src_event = *raw_event;

        Logger::getInstance().debug(L"Executing " + describeEvent(eventCode));
        HRESULT execResult = getTestedEntity()->Execute(event);

        if (!Succeeded(execResult)) {
            Logger::getInstance().error(L"Error while executing " + describeEvent(eventCode));
            return E_FAIL;
        }

        scgms::TDevice_Event receivedEvent = getTestFilter().getLastReceivedEvent();
        if (receivedEvent.signal_id != config.getSignalDstId()) {
            Logger::getInstance().error(L"Event was incorrectly mapped!");
            Logger::getInstance().error(L"expected result: " + GUID_To_WString(config.getSignalDstId()));
            Logger::getInstance().error(L"actual result: " + GUID_To_WString(receivedEvent.signal_id));
            return E_FAIL;
        }

        if ((src_event.device_time == receivedEvent.device_time)   /// Checking no other attribute has been modified
            && (src_event.device_id == receivedEvent.device_id)
            && (src_event.event_code == receivedEvent.event_code)
            && (src_event.info == receivedEvent.info)
//            && (src_event.level == receivedEvent.level)           /// Comparing NaN vs. NaN with info event returns false
            && (src_event.logical_time == receivedEvent.logical_time)
            && (src_event.parameters == receivedEvent.parameters)
            && (src_event.segment_id == receivedEvent.segment_id)) {
            Logger::getInstance().info(L"Event's signal_id successfully mapped!");
            return S_OK;
        } else {
            Logger::getInstance().error(L"Other event attributes were modified!");
            return E_FAIL;
        }
    }

    HRESULT MappingFilterUnitTester::notConfiguredSignalMappingTest() {
        tester::MappingFilterConfig config(scgms::signal_COB, scgms::signal_BG);
        HRESULT configResult = configureFilter(config);

        if (!Succeeded(configResult)) {
            logs::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        scgms::NDevice_Event_Code eventCode = scgms::NDevice_Event_Code::Information;
        scgms::IDevice_Event* event = createEvent(eventCode);
        if (!event) {
            Logger::getInstance().error(L"Error while creating " + describeEvent(eventCode));
            return E_FAIL;
        }

        scgms::TDevice_Event* raw_event;
        event->Raw(&raw_event);
        raw_event->signal_id = scgms::signal_Acceleration;  /// Setting the signal id to something other than the configured source id
        scgms::TDevice_Event src_event = *raw_event;

        Logger::getInstance().debug(L"Executing " + describeEvent(eventCode));
        HRESULT execResult = getTestedEntity()->Execute(event);

        if (!Succeeded(execResult)) {
            Logger::getInstance().error(L"Error while executing " + describeEvent(eventCode));
            return E_FAIL;
        }

        scgms::TDevice_Event receivedEvent = getTestFilter().getLastReceivedEvent();
        if (receivedEvent.signal_id != scgms::signal_Acceleration) {
            Logger::getInstance().error(L"Event was incorrectly mapped!");
            Logger::getInstance().error(L"expected result: " + GUID_To_WString(scgms::signal_Acceleration));
            Logger::getInstance().error(L"actual result: " + GUID_To_WString(receivedEvent.signal_id));
            return E_FAIL;
        }

        if ((src_event.device_time == receivedEvent.device_time)   /// Checking no other attribute has been modified
            && (src_event.device_id == receivedEvent.device_id)
            && (src_event.event_code == receivedEvent.event_code)
            && (src_event.info == receivedEvent.info)
            //            && (src_event.level == receivedEvent.level)           /// Comparing NaN vs. NaN with info event returns false
            && (src_event.logical_time == receivedEvent.logical_time)
            && (src_event.parameters == receivedEvent.parameters)
            && (src_event.segment_id == receivedEvent.segment_id)) {
            Logger::getInstance().info(L"Event's signal_id successfully mapped.");
            return S_OK;
        } else {
            Logger::getInstance().error(L"Other event attributes were modified!");
            return E_FAIL;
        }
    }

    HRESULT MappingFilterUnitTester::nullDestinationIdTest() {
        tester::MappingFilterConfig config(scgms::signal_Carb_Intake, scgms::signal_Null);
        HRESULT configResult = configureFilter(config);

        if (!Succeeded(configResult)) {
            logs::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        scgms::NDevice_Event_Code eventCode = scgms::NDevice_Event_Code::Parameters;
        scgms::IDevice_Event* event = createEvent(eventCode);
        if (!event) {
            Logger::getInstance().error(L"Error while creating " + describeEvent(eventCode));
            return E_FAIL;
        }

        scgms::TDevice_Event* raw_event;
        event->Raw(&raw_event);
        raw_event->signal_id = config.getSignalSrcId();

        Logger::getInstance().debug(L"Executing " + describeEvent(eventCode));
        HRESULT execResult = getTestedEntity()->Execute(event);

        if (!Succeeded(execResult)) {
            Logger::getInstance().error(L"Error while executing " + describeEvent(eventCode));
            return E_FAIL;
        }

        if (getTestFilter().getReceivedEventsCount() > 0) {
            scgms::TDevice_Event receivedEvent = getTestFilter().getLastReceivedEvent();
            if (receivedEvent.event_code == eventCode) {
                Logger::getInstance().error(L"Executed event arrived to the appended filter!");
                return E_FAIL;
            }
        }

        Logger::getInstance().info(L"Executed event did not arrive to the appended filter.");
        return S_OK;
    }

    HRESULT MappingFilterUnitTester::differentSrcIdNullDestinationIdTest() {
        tester::MappingFilterConfig config(scgms::signal_Air_Temperature, scgms::signal_Calibration);
        HRESULT configResult = configureFilter(config);

        if (!Succeeded(configResult)) {
            logs::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        scgms::NDevice_Event_Code eventCode = scgms::NDevice_Event_Code::Warm_Reset;
        scgms::IDevice_Event* event = createEvent(eventCode);
        if (!event) {
            Logger::getInstance().error(L"Error while creating " + describeEvent(eventCode));
            return E_FAIL;
        }

        scgms::TDevice_Event* raw_event;
        event->Raw(&raw_event);
        raw_event->signal_id = scgms::signal_Carb_Intake;   /// Setting the signal_id to something different than configured source id

        scgms::TDevice_Event src_event = *raw_event;
        Logger::getInstance().debug(L"Executing " + describeEvent(eventCode));
        HRESULT execResult = getTestedEntity()->Execute(event);

        if (!Succeeded(execResult)) {
            Logger::getInstance().error(L"Error while executing " + describeEvent(eventCode));
            return E_FAIL;
        }

        scgms::TDevice_Event receivedEvent = getTestFilter().getLastReceivedEvent();
        if (receivedEvent.event_code != eventCode) {
            Logger::getInstance().error(L"Executed event did not arrive to the appended filter!");
            Logger::getInstance().error(L"Expected event code: " + describeEvent(eventCode) +
                                            L", actual event code: " + describeEvent(receivedEvent.event_code));
            return E_FAIL;
        }

        Logger::getInstance().info(L"Executed event arrived to the appended filter.");
        if ((src_event.device_time == receivedEvent.device_time)   /// Checking no other attribute has been modified
            && (src_event.device_id == receivedEvent.device_id)
            && (src_event.event_code == receivedEvent.event_code)
            && (src_event.info == receivedEvent.info)
            //            && (src_event.level == receivedEvent.level)           /// Comparing NaN vs. NaN with info event returns false
            && (src_event.logical_time == receivedEvent.logical_time)
            && (src_event.parameters == receivedEvent.parameters)
            && (src_event.segment_id == receivedEvent.segment_id)) {
            Logger::getInstance().info(L"No other event attributes were modified.");
            return S_OK;
        } else {
            Logger::getInstance().error(L"Other event attributes were modified!");
            return E_FAIL;
        }
    }

    HRESULT MappingFilterUnitTester::allSourceIdTest() {
        tester::MappingFilterConfig config(scgms::signal_All, scgms::signal_BG);
        std::size_t eventCount = 4;
        HRESULT testResult = S_OK;

        for (std::size_t i = 0; i < eventCount; ++i) {
            scgms::NDevice_Event_Code eventCode;
            GUID signalId;

            switch (i) {
                case 0:
                    eventCode = scgms::NDevice_Event_Code::Information;
                    signalId = scgms::signal_Carb_Intake;
                    break;
                case 1:
                    eventCode = scgms::NDevice_Event_Code::Warm_Reset;
                    signalId = scgms::signal_COB;
                    break;
                case 2:
                    eventCode = scgms::NDevice_Event_Code::Parameters;
                    signalId = scgms::signal_Air_Temperature;
                    break;
                default:
                    eventCode = scgms::NDevice_Event_Code::Error;
                    signalId = scgms::signal_Carb_Ratio;
                    break;
            }

            testResult = eventMappingTest(config, eventCode, signalId);
        }

        return testResult;
    }
}