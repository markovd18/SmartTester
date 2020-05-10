//
// Created by David on 31.03.2020.
//
#include <iostream>
#include "TestFilter.h"
#include "constants.h"
#include "Logger.h"

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

HRESULT IfaceCalling TestFilter::Execute(scgms::IDevice_Event *event) {
    HRESULT result = S_OK;
    scgms::TDevice_Event* raw_event;

    event->Raw(&raw_event);
    switch (raw_event->event_code)
    {
    case scgms::NDevice_Event_Code::Information:
        result = S_INFO;
        break;
    case scgms::NDevice_Event_Code::Warning:
        result = S_WARNING;
        break;
    case scgms::NDevice_Event_Code::Error:
        result = S_ERROR;
        break;
    default:
        result = E_FAIL;
        break;
    }
    event->Release();
    return result;
}