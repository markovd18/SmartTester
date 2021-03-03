//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_SCGMSLIBUTILS_H
#define SMARTTESTER_SCGMSLIBUTILS_H

#include <iface/DeviceIface.h>

/**
 * Creates device event based on given event code. Returned pointer is non-owning and shouldn't be deleted.
 * It will be deleted automatically during it's execution.
 *
 * @param eventCode event code describing
 * @return non-owning pointer to created event
 */
scgms::IDevice_Event *createEvent(const scgms::NDevice_Event_Code eventCode);
/**
 * Returns a string representation of given event code
 * @param event event code to describe
 * @return name of the event code
 */
std::wstring describeEvent(const scgms::NDevice_Event_Code eventCode);
#endif //SMARTTESTER_SCGMSLIBUTILS_H
