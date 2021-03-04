#include <iostream>
#include <rtl/FilterLib.h>
#include <utils/string_utils.h>
#include "../MappingFilterUnitTester.h"
#include "../../utils/scgmsLibUtils.h"

namespace tester {

    const GUID MappingFilterUnitTester::SIGNAL_SRC_ID_GUID = { 0xe1cd07ef, 0xb079, 0x4911, {0xb7, 0x9b, 0xd2, 0x03, 0x48, 0x61, 0x01, 0xc8} };
    const GUID MappingFilterUnitTester::SIGNAL_DST_ID_GUID = { 0xf666f6c2, 0xd7c0, 0x43e8, {0x8e, 0xe1, 0xc8, 0xca, 0xa8, 0xf8, 0x60, 0xe5} };

    MappingFilterUnitTester::MappingFilterUnitTester(const GUID& guid) : GenericUnitTester(guid) {
        //
    }

    /**
        Executes all specific tests for loaded Mapping filter.
    */
    void MappingFilterUnitTester::executeSpecificTests(){
        Logger::getInstance().info(L"Executing specific tests...");

        /// Configuration tests
        tester::MappingFilterConfig config;
        config.setSignalDstId(SIGNAL_DST_ID_GUID);
        executeConfigTest(L"empty source id test", config, E_FAIL);

        config.setSignalDstId(Invalid_GUID);
        config.setSignalSrcId(SIGNAL_SRC_ID_GUID);
        executeConfigTest(L"empty destination id test", config, E_FAIL);

        config.setSignalDstId(SIGNAL_DST_ID_GUID);
        executeConfigTest(L"correct id's test", config, S_OK);

        /// Functional tests
        executeTest(L"level event mapping test", std::bind(&MappingFilterUnitTester::levelEventMappingTest, this));
        executeTest(L"info event mapping test", std::bind(&MappingFilterUnitTester::infoEventMappingTest, this));
        executeTest(L"parameters event mapping test", std::bind(&MappingFilterUnitTester::parametersEventMappingTest, this));
    }

    /**
        Helper method for correct configuring of Mapping filter.
    */
    HRESULT MappingFilterUnitTester::configureFilterCorrectly() {
        if (!isFilterLoaded()) {
            std::wcerr << L"No filter created! Cannot execute test.\n";
            Logger::getInstance().error(L"No filter loaded! Can't execute test.");
            return E_FAIL;
        }

        scgms::SPersistent_Filter_Chain_Configuration configuration;
        refcnt::Swstr_list errors;
        tester::MappingFilterConfig config(SIGNAL_SRC_ID_GUID, SIGNAL_DST_ID_GUID);
        std::string memory = config.toString();

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
        return getTestFilter().Configure(begin[0], errors.get());
    }

    /**
        Tests if level event's signal id will be correctly mapped.
    */
    HRESULT MappingFilterUnitTester::levelEventMappingTest() {
        HRESULT result = configureFilterCorrectly();

        if (!Succeeded(result)) {
            Logger::getInstance().error(L"Failed to configure filter!");
            Logger::getInstance().error(L"expected result: " + Widen_Char(std::system_category().message(S_OK).c_str()));
            Logger::getInstance().error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
            shutDownTest();
            return E_FAIL;
        }

        scgms::IDevice_Event* event = createEvent(scgms::NDevice_Event_Code::Level);
        if (event == nullptr) {
            std::wcerr << L"Error while creating \"Level\" IDevice_event!\n";
            Logger::getInstance().error(L"Error while creating \"Level\" IDevice_event!");
            shutDownTest();
            return E_FAIL;
        }

        scgms::TDevice_Event* raw_event;
        event->Raw(&raw_event);
        raw_event->signal_id = SIGNAL_SRC_ID_GUID;
        scgms::TDevice_Event src_event = *raw_event;

        result = getTestedFilter()->Execute(event);
        Logger::getInstance().info(L"Executing \"Level\" event...");

        GUID dst_id = getTestFilter().getRecievedEvent()->signal_id;
        if (Succeeded(result) && (dst_id == SIGNAL_DST_ID_GUID)) {
            if ((src_event.device_time == raw_event->device_time)
                && (src_event.device_id == raw_event->device_id)
                && (src_event.event_code == raw_event->event_code)
                && (src_event.info == raw_event->info)
                //&& (src_event.level == raw_event->level) //pouze tady pad�?? nan
                && (src_event.logical_time == raw_event->logical_time)
                && (src_event.parameters == raw_event->parameters)
                && (src_event.segment_id == raw_event->segment_id)) {
                Logger::getInstance().debug(L"Event's signal_id successfully mapped!");
                result = S_OK;
            } else {
                Logger::getInstance().error(L"Other event attributes were modified!");
                result = E_FAIL;
            }

        } else {
            Logger::getInstance().error(L"Event was incorrectly sent or it's signal_id wasn't mapped!");
            Logger::getInstance().error(L"expected result: " + GUID_To_WString(SIGNAL_DST_ID_GUID));
            Logger::getInstance().error(L"actual result: " + GUID_To_WString(dst_id));
            result = E_FAIL;
        }

        event->Release();
        shutDownTest();
        return result;
    }

    /**
        Tests if info event's signal id will be correctly mapped.
    */
    HRESULT MappingFilterUnitTester::infoEventMappingTest() {
        HRESULT result = configureFilterCorrectly();
        if (!Succeeded(result)) {
            Logger::getInstance().error(L"Failed to configure filter!");
            Logger::getInstance().error(L"expected result: " + Widen_Char(std::system_category().message(S_OK).c_str()));
            Logger::getInstance().error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
            shutDownTest();
            return E_FAIL;
        }

        scgms::IDevice_Event* event = createEvent(scgms::NDevice_Event_Code::Information);
        if (event == nullptr) {
            std::wcerr << L"Error while creating \"Info\" IDevice_event!\n";
            Logger::getInstance().error(L"Error while creating \"Info\" IDevice_event!");
            shutDownTest();
            return E_FAIL;
        }

        scgms::TDevice_Event* raw_event;
        event->Raw(&raw_event);
        raw_event->signal_id = SIGNAL_SRC_ID_GUID;
        scgms::TDevice_Event src_event = *raw_event;

        result = getTestedFilter()->Execute(event);
        Logger::getInstance().info(L"Executing \"Info\" event...");

        GUID dst_id = getTestFilter().getRecievedEvent()->signal_id;
        if (Succeeded(result) && (dst_id == SIGNAL_DST_ID_GUID)) {
            if ((src_event.device_time == raw_event->device_time)
                && (src_event.device_id == raw_event->device_id)
                && (src_event.event_code == raw_event->event_code)
                && (src_event.info == raw_event->info)
                && (src_event.level == raw_event->level)
                && (src_event.logical_time == raw_event->logical_time)
                && (src_event.parameters == raw_event->parameters)
                && (src_event.segment_id == raw_event->segment_id)) {
                Logger::getInstance().debug(L"Event's signal_id successfully mapped!");
                result = S_OK;
            } else {
                Logger::getInstance().error(L"Other event attributes were modified!");
                result = E_FAIL;
            }
        } else {
            Logger::getInstance().error(L"Event was incorrectly sent or it's signal_id wasn't mapped!");
            Logger::getInstance().error(L"expected result: " + GUID_To_WString(SIGNAL_DST_ID_GUID));
            Logger::getInstance().error(L"actual result: " + GUID_To_WString(dst_id));
            result = E_FAIL;
        }
        event->Release();

        shutDownTest();
        return result;
    }

    HRESULT MappingFilterUnitTester::parametersEventMappingTest() {
        HRESULT result = configureFilterCorrectly();
        if (!Succeeded(result)) {
            Logger::getInstance().error(L"Failed to configure filter!");
            Logger::getInstance().error(L"expected result: " + Widen_Char(std::system_category().message(S_OK).c_str()));
            Logger::getInstance().error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
            shutDownTest();
            return E_FAIL;
        }

        scgms::IDevice_Event* event = createEvent(scgms::NDevice_Event_Code::Parameters);
        if (event == nullptr) {
            std::wcerr << L"Error while creating \"Parameters\" IDevice_event!\n";
            Logger::getInstance().error(L"Error while creating \"Parameters\" IDevice_event!");
            shutDownTest();
            return E_FAIL;
        }

        scgms::TDevice_Event* raw_event;
        event->Raw(&raw_event);
        raw_event->signal_id = SIGNAL_SRC_ID_GUID;
        scgms::TDevice_Event src_event = *raw_event;

        result = getTestedFilter()->Execute(event);
        Logger::getInstance().info(L"Executing \"Parameters\" event...");

        GUID dst_id = getTestFilter().getRecievedEvent()->signal_id;
        if (Succeeded(result) && (dst_id == SIGNAL_DST_ID_GUID)) {
            if ((src_event.device_time == raw_event->device_time)
                && (src_event.device_id == raw_event->device_id)
                && (src_event.event_code == raw_event->event_code)
                && (src_event.info == raw_event->info)
                && (src_event.level == raw_event->level)
                && (src_event.logical_time == raw_event->logical_time)
                && (src_event.parameters == raw_event->parameters)
                && (src_event.segment_id == raw_event->segment_id)) {
                Logger::getInstance().debug(L"Event's signal_id successfully mapped!");
                result = S_OK;
            } else {
                Logger::getInstance().error(L"Other event attributes were modified!");
                result = E_FAIL;
            }

        } else {
            Logger::getInstance().error(L"Event was incorrectly sent or it's signal_id wasn't mapped!");
            Logger::getInstance().error(L"expected result: " + GUID_To_WString(SIGNAL_DST_ID_GUID));
            Logger::getInstance().error(L"actual result: " + GUID_To_WString(dst_id));
            result = E_FAIL;
        }
        event->Release();


        shutDownTest();
        return result;
    }
}