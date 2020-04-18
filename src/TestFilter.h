//
// Created by David on 31.03.2020.
//

#ifndef SMARTTESTER_TESTFILTER_H
#define SMARTTESTER_TESTFILTER_H

#include "../../smartcgms/src/common/iface/FilterIface.h"
#include "../../smartcgms/src/common/rtl/referencedImpl.h"

class TestFilter : public virtual scgms::IFilter, public virtual refcnt::CNotReferenced {
    using IFilter_Configuration = refcnt::IVector_Container<scgms::IFilter_Parameter*>;
public:
    explicit TestFilter();
    virtual ~TestFilter();

    virtual HRESULT IfaceCalling Execute(scgms::IDevice_Event *event) override final;
    virtual HRESULT IfaceCalling Configure(IFilter_Configuration* configuration, refcnt::wstr_list *error_description) override final;
    //virtual HRESULT IfaceCalling QueryInterface(const GUID*  riid, void ** ppvObj) override final;
    //virtual ULONG IfaceCalling AddRef() override final;
    //virtual ULONG IfaceCalling Release() override final;
};
#endif SMARTTESTER_TESTFILTER_H
