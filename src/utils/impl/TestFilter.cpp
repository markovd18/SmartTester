//
// Author: markovd@students.zcu.cz
//
#include "../TestFilter.h"
#include "../constants.h"

TestFilter::TestFilter() : m_receivedEvent(nullptr) {
}

HRESULT IfaceCalling TestFilter::Configure(IFilter_Configuration* configuration, refcnt::wstr_list *error_description){
    return S_OK;
}

HRESULT IfaceCalling TestFilter::Execute(scgms::IDevice_Event *event) {

    if (event == nullptr)
    {
        return E_FAIL;
    }
    event->Raw(&m_receivedEvent);

    if (m_receivedEvent->event_code == scgms::NDevice_Event_Code::Shut_Down) {
        event->Release();
    }

    return S_OK;
}

scgms::TDevice_Event* TestFilter::getRecievedEvent()
{
    return m_receivedEvent;
}