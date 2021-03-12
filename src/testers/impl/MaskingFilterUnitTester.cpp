#include <iostream>
#include <rtl/FilterLib.h>
#include <utils/string_utils.h>
#include "../MaskingFilterUnitTester.h"
#include "../../utils/scgmsLibUtils.h"
#include "../../utils/LogUtils.h"

namespace tester {

    const GUID MaskingFilterUnitTester::SIGNAL_ID_GUID = {0xe1cd07ef, 0xb079, 0x4911,
                                                          {0xb7, 0x9b, 0xd2, 0x03, 0x48, 0x61, 0x01, 0xc8}};

    MaskingFilterUnitTester::MaskingFilterUnitTester(const GUID &guid) : GenericUnitTester(guid) {
        //
    }

    void MaskingFilterUnitTester::executeSpecificTests() {
        Logger::getInstance().info(L"Executing specific tests...");
        executeTest(L"complete bitmask masking test",
                    std::bind(&MaskingFilterUnitTester::completeBitmaskMappingTest, this));
        executeTest(L"info event masking test", std::bind(&MaskingFilterUnitTester::infoEventMaskingTest, this));
    }


    HRESULT MaskingFilterUnitTester::completeBitmaskMappingTest() {
        HRESULT result = S_OK;
        std::string bitmasks[] = {"00000000", "11111111", "01010101", "10101010", "00101111", "11010000"};

        for (const auto& bitmask : bitmasks) {
            if (bitmaskMappingTest(bitmask) != S_OK) {
                result = E_FAIL;
            }
        }

        return result;
    }

    HRESULT MaskingFilterUnitTester::bitmaskMappingTest(const std::string &bitmask) {
        tester::MaskingFilterConfig config(SIGNAL_ID_GUID, bitmask);
        HRESULT configResult = configureFilter(config);
        if (!Succeeded(configResult)) {
            log::logConfigurationError(config, S_OK, configResult);
            return E_FAIL;
        }

        for (const char &i : bitmask) {
            scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Level);
            if (!event) {
                Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Level));
                return E_FAIL;
            }

            scgms::TDevice_Event *raw_event;
            event->Raw(&raw_event);
            raw_event->signal_id = SIGNAL_ID_GUID;

            Logger::getInstance().info(L"Executing " + describeEvent(scgms::NDevice_Event_Code::Level));
            HRESULT execResult = getTestedFilter()->Execute(event);
            if (!Succeeded(execResult)) {
                Logger::getInstance().error(L"Error while executing " + describeEvent(scgms::NDevice_Event_Code::Level));
                return E_FAIL;
            }

            if (i == '0') {
                if (raw_event->event_code != scgms::NDevice_Event_Code::Masked_Level) {
                    Logger::getInstance().error(L"Event wasn't correctly masked!");
                    Logger::getInstance().error(L"expected code: " + describeEvent(scgms::NDevice_Event_Code::Masked_Level));
                    Logger::getInstance().error(L"actual code: " + describeEvent(raw_event->event_code));
                    return E_FAIL;
                }

                Logger::getInstance().debug(L"Event correctly masked!");
            } else {
                if (raw_event->event_code != scgms::NDevice_Event_Code::Level) {
                    Logger::getInstance().error(L"Event shouldn't have been masked!");
                    Logger::getInstance().error(L"expected code: " + describeEvent(scgms::NDevice_Event_Code::Level));
                    Logger::getInstance().error(L"actual code: " + describeEvent(raw_event->event_code));
                    return E_FAIL;
                }

                Logger::getInstance().debug(L"Event not masked correctly!");
            }
        }

        return S_OK;
    }


    HRESULT MaskingFilterUnitTester::infoEventMaskingTest() {
        std::string bitmask = "00101111";
        tester::MaskingFilterConfig config(SIGNAL_ID_GUID, bitmask);
        HRESULT result = configureFilter(config);
        if (!Succeeded(result)) {
            log::logConfigurationError(config, S_OK, result);
            return E_FAIL;
        }

        HRESULT test_result = S_OK;
        for (size_t i = 0; i < bitmask.size(); i++) {
            scgms::IDevice_Event *event = createEvent(scgms::NDevice_Event_Code::Information);
            if (event == nullptr) {
                std::wcerr << L"Error while creating " << describeEvent(scgms::NDevice_Event_Code::Information);
                Logger::getInstance().error(L"Error while creating " + describeEvent(scgms::NDevice_Event_Code::Information));
                return E_FAIL;
            }

            scgms::TDevice_Event *raw_event;
            event->Raw(&raw_event);
            raw_event->signal_id = SIGNAL_ID_GUID;

            result = getTestedFilter()->Execute(event);
            Logger::getInstance().info(L"Executing " + describeEvent(scgms::NDevice_Event_Code::Information));

            if (Succeeded(result)) {
                if (raw_event->event_code != scgms::NDevice_Event_Code::Information) {
                    Logger::getInstance().error(L"Info event was incorrectly masked!");
                    Logger::getInstance().error(L"expected result: " + describeEvent(scgms::NDevice_Event_Code::Information));
                    Logger::getInstance().error(L"actual result: " + describeEvent(raw_event->event_code));
                    test_result = E_FAIL;
                }
            } else {
                Logger::getInstance().error(L"Error while executing event!");
                Logger::getInstance().error(std::wstring(L"expected result: ") + Describe_Error(S_OK));
                Logger::getInstance().error(std::wstring(L"actual result: ") + Describe_Error(result));
                test_result = E_FAIL;
            }

            event->Release();
        }

        return test_result;
    }
}