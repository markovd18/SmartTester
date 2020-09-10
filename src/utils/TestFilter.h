//
// Created by David on 31.03.2020.
//

#ifndef SMARTTESTER_TESTFILTER_H
#define SMARTTESTER_TESTFILTER_H

#include <iface/FilterIface.h>
#include <rtl/referencedImpl.h>

class TestFilter : public virtual scgms::IFilter, public virtual refcnt::CNotReferenced {

    using IFilter_Configuration = refcnt::IVector_Container<scgms::IFilter_Parameter*>;
private:
    scgms::TDevice_Event* recievedEvent;
public:
    explicit TestFilter();
    ~TestFilter() override;

    scgms::TDevice_Event* getRecievedEvent();

    HRESULT IfaceCalling Execute(scgms::IDevice_Event *event) final;
    HRESULT IfaceCalling Configure(IFilter_Configuration* configuration, refcnt::wstr_list *error_description) final;
};
#endif //SMARTTESTER_TESTFILTER_H
