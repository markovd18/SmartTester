//
// Author: markovd@students.zcu.cz
//

#include <rtl/scgmsLib.h>
#include <rtl/hresult.h>
#include <iface/DeviceIface.h>
#include "../scgmsLibUtils.h"

scgms::IDevice_Event * scgms::createEvent(const scgms::NDevice_Event_Code eventCode) {
    scgms::IDevice_Event* event;

    auto creator = scgms::factory::resolve_symbol<scgms::TCreate_Device_Event>("create_device_event");
    HRESULT result = creator(eventCode, &event);
    if (!Succeeded(result)) {
        return nullptr;
    }

    return event;
}
