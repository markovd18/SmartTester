//
// Author: markovd@students.zcu.cz
//

#include <rtl/scgmsLib.h>
#include <rtl/hresult.h>
#include <iface/DeviceIface.h>
#include "../scgmsLibUtils.h"

scgms::IDevice_Event * createEvent(const scgms::NDevice_Event_Code eventCode) {
    scgms::IDevice_Event* event;

    auto creator = scgms::factory::resolve_symbol<scgms::TCreate_Device_Event>("create_device_event");
    HRESULT result = creator(eventCode, &event);
    if (!Succeeded(result)) {
        return nullptr;
    }

    return event;
}

std::wstring describeEvent(const scgms::NDevice_Event_Code eventCode) {

    switch (eventCode) {

        case scgms::NDevice_Event_Code::Nothing: return L"Null event";
        case scgms::NDevice_Event_Code::Shut_Down: return L"Shut down event";
        case scgms::NDevice_Event_Code::Level: return L"Level event";
        case scgms::NDevice_Event_Code::Masked_Level: return L"Masked level event";
        case scgms::NDevice_Event_Code::Parameters: return L"Parameters event";
        case scgms::NDevice_Event_Code::Parameters_Hint: return L"Parameters hint event";
        case scgms::NDevice_Event_Code::Suspend_Parameter_Solving: return L"Suspend parameter solving event";
        case scgms::NDevice_Event_Code::Resume_Parameter_Solving: return L"Resume parameter solving event";
        case scgms::NDevice_Event_Code::Solve_Parameters: return L"Solve parameters event";
        case scgms::NDevice_Event_Code::Time_Segment_Start: return L"Time segment start event";
        case scgms::NDevice_Event_Code::Time_Segment_Stop: return L"Time segment stop event";
        case scgms::NDevice_Event_Code::Warm_Reset: return L"Warm reset event";
        case scgms::NDevice_Event_Code::Information: return L"Information event";
        case scgms::NDevice_Event_Code::Warning: return L"Warning event";
        case scgms::NDevice_Event_Code::Error: return L"Error event";
        case scgms::NDevice_Event_Code::count: return L"Count event";
        default: return L"UNKNOWN event";
    }
}
