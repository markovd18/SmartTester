//
// Created by David on 31.03.2020.
//

#ifndef SMARTTESTER_TESTFILTER_H
#define SMARTTESTER_TESTFILTER_H

#include "../../smartcgms/src/common/rtl/FilterLib.h"
#include "../../smartcgms/src/common/rtl/referencedImpl.h"

#pragma warning( push )
#pragma warning( disable : 4250 )

class TestFilter : public scgms::CBase_Filter{
protected:
    virtual HRESULT Do_Execute(scgms::UDevice_Event event) override final;
    virtual HRESULT Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list& error_description) override final;
public:
    explicit TestFilter(scgms::IFilter *output);
    virtual ~TestFilter();

    virtual HRESULT IfaceCalling QueryInterface(const GUID*  riid, void ** ppvObj) override final;
};
#pragma warning( pop )

#endif //SMARTTESTER_TESTFILTER_H
