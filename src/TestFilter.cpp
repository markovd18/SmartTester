//
// Created by David on 31.03.2020.
//
#include <iostream>
#include "TestFilter.h"

TestFilter::TestFilter(scgms::IFilter *output) : CBase_Filter(output) {
    //
}

TestFilter::~TestFilter() {
    //
}

HRESULT TestFilter::QueryInterface(const GUID*  riid, void ** ppvObj) {
    //
    return 0;
}

HRESULT TestFilter::Do_Configure(scgms::SFilter_Configuration configuration, refcnt::Swstr_list& error_description) {
    //
    return 0;
}

HRESULT TestFilter::Do_Execute(scgms::UDevice_Event event){
    //
    std::cout << "hovno";
    return 0;
}


