//
// Author: markovd@students.zcu.cz
//
#include <unordered_set>
#include "../TestFilter.h"
#include "../constants.h"


HRESULT IfaceCalling TestFilter::Configure(IFilter_Configuration* configuration, refcnt::wstr_list *error_description){
    return S_OK;
}

HRESULT IfaceCalling TestFilter::Execute(scgms::IDevice_Event *event) {

    if (event == nullptr) {
        return S_FALSE;
    }

    scgms::TDevice_Event *rawEvent;
    event->Raw(&rawEvent);
    m_receivedEvents.push_back(*rawEvent);

    event->Release();   /// Copying acquired data and releasing, so we don't need to manually release in every test
    return S_OK;
}

const scgms::TDevice_Event& TestFilter::getLastReceivedEvent() {
    return m_receivedEvents.back();
}

const scgms::TDevice_Event &TestFilter::getFirstReceivedEvent() {
    return m_receivedEvents.front();
}

void TestFilter::clearReceivedEvents() noexcept {
    m_receivedEvents.clear();
}

const scgms::TDevice_Event* TestFilter::getLastNonShutDownEvent() {
    auto iterator = m_receivedEvents.rbegin();
    while (iterator != m_receivedEvents.rend()) {
        if (iterator->event_code != scgms::NDevice_Event_Code::Shut_Down) {
            return &*iterator;
        }

        iterator++;
    }

    return nullptr;
}

std::size_t TestFilter::getReceivedEventsCount() {
    return m_receivedEvents.size();
}

std::size_t TestFilter::getUniqueSegmentIdsFromReceivedEventsCount() {
    std::unordered_set<uint64_t> uniqueIds;
    for (const auto &event : m_receivedEvents) {
        uniqueIds.insert(event.segment_id);
    }

    return uniqueIds.size();
}
