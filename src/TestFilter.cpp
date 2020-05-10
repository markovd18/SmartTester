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
        return E_FAIL;
    }
    event->Raw(&recievedEvent);
    event->Release();
    return S_OK;
}

scgms::TDevice_Event* TestFilter::getRecievedEvent()
{
    return recievedEvent;
}