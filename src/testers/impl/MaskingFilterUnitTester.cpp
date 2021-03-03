#include <iostream>
#include <rtl/FilterLib.h>
#include <utils/string_utils.h>
#include "../MaskingFilterUnitTester.h"
#include "../../utils/scgmsLibUtils.h"

namespace tester {

    const std::string MaskingFilterUnitTester::FILTER_CONFIG = "[Filter_001_{A1124C89-18A4-F4C1-28E8-A9471A58021E}]";
    const GUID MaskingFilterUnitTester::SIGNAL_ID_GUID = {0xe1cd07ef, 0xb079, 0x4911,
                                                          {0xb7, 0x9b, 0xd2, 0x03, 0x48, 0x61, 0x01, 0xc8}};
    const std::string MaskingFilterUnitTester::SIGNAL_ID_STR = "{E1CD07EF-B079-4911-B79B-D203486101C8}";

    MaskingFilterUnitTester::MaskingFilterUnitTester(const GUID &guid) : GenericUnitTester(guid) {
        //
    }

    void MaskingFilterUnitTester::executeSpecificTests() {
        Logger::getInstance().info(L"Executing specific tests...");
        executeTest(L"complete bitmask masking test",
                    std::bind(&MaskingFilterUnitTester::completeBitmaskMappingTest, this));
        executeTest(L"info event masking test", std::bind(&MaskingFilterUnitTester::infoEventMaskingTest, this));
    }


    HRESULT MaskingFilterUnitTester::configureFilterCorrectly(const std::string &bitmask) {
        if (!isFilterLoaded()) {
            std::wcerr << L"No filter created! Cannot execute test.\n";
            Logger::getInstance().error(L"No filter loaded! Can't execute test.");
            return E_FAIL;
        }

        scgms::SPersistent_Filter_Chain_Configuration configuration;
        refcnt::Swstr_list errors;
        std::string memory = FILTER_CONFIG + "\n\nSignal = " + SIGNAL_ID_STR + "\n\nBitmask = " + bitmask;
        Logger::getInstance().debug(
                L"Signal = " + std::wstring(SIGNAL_ID_STR.begin(), SIGNAL_ID_STR.end()) + L", Bitmask = " +
                std::wstring(bitmask.begin(), bitmask.end()));

        HRESULT result = configuration ? S_OK : E_FAIL;
        if (result == S_OK) {
            configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
            Logger::getInstance().info(L"Loading configuration from memory...");
        } else {
            Logger::getInstance().error(L"Error while creating configuration!");
        }

        scgms::IFilter_Configuration_Link **begin, **end;
        configuration->get(&begin, &end);

        return getTestFilter().Configure(begin[0], errors.get());
    }


    HRESULT MaskingFilterUnitTester::completeBitmaskMappingTest() {
        HRESULT result = S_OK;
        std::string bitmasks[] = {"00000000", "11111111", "01010101", "10101010", "00101111", "11010000"};
        int bitmasks_length = 6;

        for (int i = 0; i < bitmasks_length; i++) {
            if (bitmaskMappingTest(bitmasks[i]) != S_OK) {
                result = E_FAIL;
            }
        }
        shutDownTest();
        return result;
    }

    HRESULT MaskingFilterUnitTester::bitmaskMappingTest(const std::string &bitmask) {

        Logger::getInstance().info(L"Configuring filter...");
        HRESULT result = configureFilterCorrectly(bitmask);
        if (!Succeeded(result)) {
            Logger::getInstance().error(L"Failed to configure filter!");
            return E_FAIL;
        }

        HRESULT test_result = S_OK;
        for (const char &i : bitmask) {
            scgms::IDevice_Event *event = scgms::createEvent(scgms::NDevice_Event_Code::Level);
            if (event == nullptr) {
                std::wcerr << L"Error while creating \"Level\" IDevice_event!\n";
                Logger::getInstance().error(L"Error while creating \"Level\" IDevice_event!");
                shutDownTest();
                return E_FAIL;
            }

            scgms::TDevice_Event *raw_event;
            event->Raw(&raw_event);
            raw_event->signal_id = SIGNAL_ID_GUID;

            result = getTestedFilter()->Execute(event);
            Logger::getInstance().info(L"Executing \"Level\" event...");

            if (Succeeded(result)) {
                if (i == '0') {
                    if (raw_event->event_code != scgms::NDevice_Event_Code::Masked_Level) {
                        Logger::getInstance().error(L"Event wasn't correctly masked!");
                        Logger::getInstance().error(std::wstring(L"expected code: ") + std::to_wstring((int) scgms::NDevice_Event_Code::Masked_Level));
                        Logger::getInstance().error(std::wstring(L"actual code: ") + std::to_wstring((int) raw_event->event_code));
                        test_result = E_FAIL;
                    }
                    Logger::getInstance().debug(L"Event correctly masked!");
                } else {
                    if (raw_event->event_code != scgms::NDevice_Event_Code::Level) {
                        Logger::getInstance().error(L"Event shouldn't have been masked!");
                        Logger::getInstance().error(std::wstring(L"expected code: ") + std::to_wstring((int) scgms::NDevice_Event_Code::Level));
                        Logger::getInstance().error(std::wstring(L"actual code: ") + std::to_wstring((int) raw_event->event_code));
                        test_result = E_FAIL;
                    }
                }
            } else {
                Logger::getInstance().error(L"Error while executing \"Level\" event!");
                test_result = E_FAIL;
            }

            event->Release();
        }

        shutDownTest();
        return test_result;
    }

/**
 * Tests if Info event is correctly not masked by this filter.
 *
 * @return S_OK if Info event is not masked by this filter, otherwise E_FAIL
 */
    HRESULT MaskingFilterUnitTester::infoEventMaskingTest() {
        if (!isFilterLoaded()) {
            std::wcerr << L"No filter created! Cannot execute test.\n";
            Logger::getInstance().error(L"No filter loaded! Can't execute test.");
            return E_FAIL;
        }

        scgms::SPersistent_Filter_Chain_Configuration configuration;
        refcnt::Swstr_list errors;
        std::string bitmask = "00101111";
        std::string memory = FILTER_CONFIG + "\n\nSignal = " + SIGNAL_ID_STR + "\n\nBitmask = " + bitmask;
        Logger::getInstance().debug(
                L"Signal = " + std::wstring(SIGNAL_ID_STR.begin(), SIGNAL_ID_STR.end()) + L", Bitmask = " +
                std::wstring(bitmask.begin(), bitmask.end()));

        HRESULT result = configuration ? S_OK : E_FAIL;
        if (result == S_OK) {
            configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
            Logger::getInstance().info(L"Loading configuration from memory...");
        } else {
            Logger::getInstance().error(L"Error while creating configuration!");
            shutDownTest();
            return E_FAIL;
        }

        scgms::IFilter_Configuration_Link **begin, **end;
        configuration->get(&begin, &end);

        result = getTestedFilter()->Configure(begin[0], errors.get());
        Logger::getInstance().info(L"Configuring filter...");

        if (!Succeeded(result)) {
            Logger::getInstance().error(L"Failed to configure filter!");
            shutDownTest();
            return E_FAIL;
        }

        HRESULT test_result = S_OK;
        for (size_t i = 0; i < bitmask.size(); i++) {
            scgms::IDevice_Event *event = scgms::createEvent(scgms::NDevice_Event_Code::Information);
            if (event == nullptr) {
                std::wcerr << L"Error while creating \"Info\" IDevice_event!\n";
                Logger::getInstance().error(L"Error while creating \"Info\" IDevice_event!");
                shutDownTest();
                return E_FAIL;
            }

            scgms::TDevice_Event *raw_event;
            event->Raw(&raw_event);
            raw_event->signal_id = SIGNAL_ID_GUID;

            result = getTestedFilter()->Execute(event);
            Logger::getInstance().info(L"Executing \"Info\" event...");

            if (Succeeded(result)) {
                if (raw_event->event_code != scgms::NDevice_Event_Code::Information) {
                    Logger::getInstance().error(L"Info event was incorrectly masked!");
                    Logger::getInstance().error(std::wstring(L"expected code: ") + std::to_wstring((int) scgms::NDevice_Event_Code::Information));
                    Logger::getInstance().error(std::wstring(L"actual result: ") + std::to_wstring((int) raw_event->event_code));
                    test_result = E_FAIL;
                }
            } else {
                Logger::getInstance().error(L"Error while executing event!");
                Logger::getInstance().error(
                        L"expected result: " + Widen_Char(std::system_category().message(S_OK).c_str()));
                Logger::getInstance().error(
                        L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
                test_result = E_FAIL;
            }

            event->Release();
        }

        shutDownTest();
        return test_result;
    }
}