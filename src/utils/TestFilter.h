//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_TESTFILTER_H
#define SMARTTESTER_TESTFILTER_H

#include <iface/FilterIface.h>
#include <rtl/referencedImpl.h>

/**
 * Class representing our custom test filter. This filter is appended to the tested filter and it's only function
 * is to "catch" executed event and store it in attribute, so we can check it in the test function later.
 */
class TestFilter : public virtual scgms::IFilter, public virtual refcnt::CNotReferenced {

    using IFilter_Configuration = refcnt::IVector_Container<scgms::IFilter_Parameter*>;
private:
    /// Event that the filter we are appended to executed
    std::vector<scgms::TDevice_Event> m_receivedEvents;
public:
    TestFilter() = default;
    ~TestFilter() override = default;

    /// Returns a reference to the event data we got from the tested filter.
    const scgms::TDevice_Event& getLastReceivedEvent();
    /// Returns a reference to the first event we received from the tested filter
    const scgms::TDevice_Event& getFirstReceivedEvent();
    /// Clears the vector of received events
    void clearReceivedEvents() noexcept;
    /// Returns the number of unique segment id's on the vector of received events
    std::size_t getUniqueSegmentIdsFromReceivedEventsCount();

    const scgms::TDevice_Event* getLastNonShutDownEvent();
    std::size_t getReceivedEventsCount();

    HRESULT IfaceCalling Execute(scgms::IDevice_Event *event) final;

    HRESULT IfaceCalling Configure(IFilter_Configuration* configuration, refcnt::wstr_list *error_description) final;
};
#endif //SMARTTESTER_TESTFILTER_H
