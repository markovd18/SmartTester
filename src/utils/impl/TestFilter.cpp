//
// Author: markovd@students.zcu.cz
//
#include "../TestFilter.h"
#include "../constants.h"

TestFilter::TestFilter() : m_receivedEvent() {
}

HRESULT IfaceCalling TestFilter::Configure(IFilter_Configuration* configuration, refcnt::wstr_list *error_description){
    return S_OK;
}

HRESULT IfaceCalling TestFilter::Execute(scgms::IDevice_Event *event) {

    if (event == nullptr) {
        return S_FALSE;
    }

    scgms::TDevice_Event *rawEvent;
    event->Raw(&rawEvent);
    m_receivedEvent = *rawEvent;

    event->Release();   /// Copying acquired data and releasing, so we don't need to manually release in every test
    return S_OK;
}

const scgms::TDevice_Event& TestFilter::getReceivedEvent() {
    return m_receivedEvent;
}