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
    scgms::TDevice_Event* m_receivedEvent;
public:
    TestFilter();
    ~TestFilter() override = default;

    /// Returns a pointer to the event we got from the tested filter.
    scgms::TDevice_Event* getReceivedEvent();

    HRESULT IfaceCalling Execute(scgms::IDevice_Event *event) final;
    HRESULT IfaceCalling Configure(IFilter_Configuration* configuration, refcnt::wstr_list *error_description) final;
};
#endif //SMARTTESTER_TESTFILTER_H
