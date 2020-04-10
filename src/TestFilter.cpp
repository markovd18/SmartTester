//
// Created by David on 31.03.2020.
//
#include <iostream>
#include "TestFilter.h"
using IFilter_Configuration = refcnt::IVector_Container<scgms::IFilter_Parameter*>;

TestFilter::TestFilter(){
    //
}
TestFilter::~TestFilter() {
    //
}
HRESULT IfaceCalling TestFilter::Configure(IFilter_Configuration* configuration, refcnt::wstr_list *error_description){
    //TODO
    return 0;
}

HRESULT IfaceCalling TestFilter::Execute(scgms::IDevice_Event *event){
    return 0;
}
ULONG IfaceCalling TestFilter::AddRef(){
    return 0;
}

ULONG IfaceCalling TestFilter::Release(){
    return 0;
}

HRESULT IfaceCalling TestFilter::QueryInterface(const GUID*  riid, void ** ppvObj){
    return 0;
}


