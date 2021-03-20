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
    m_receivedEvents.push_back(m_receivedEvent);

    event->Release();   /// Copying acquired data and releasing, so we don't need to manually release in every test
    return S_OK;
}

const scgms::TDevice_Event& TestFilter::getReceivedEvent() {
    return m_receivedEvents.back();
}

void TestFilter::resetReceivedEvent() {
    m_receivedEvent = scgms::TDevice_Event();
}

void TestFilter::clearReceivedEvents() noexcept {
    m_receivedEvents.clear();
}

const scgms::TDevice_Event* TestFilter::getLastNonShutDownEvent() {
    auto iterator = m_receivedEvents.end();
    while (iterator != m_receivedEvents.begin()) {
        if (iterator->event_code != scgms::NDevice_Event_Code::Shut_Down) {
            return iterator.base();
        }

        iterator--;
    }

    return nullptr;
}

std::size_t TestFilter::getReceivedEventsCount() {
    return m_receivedEvents.size();
}
