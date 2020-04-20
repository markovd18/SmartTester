//
// Created by David on 31.03.2020.
//
#include <iostream>
#include "TestFilter.h"

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