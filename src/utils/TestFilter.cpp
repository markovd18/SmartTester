//
// Created by David on 31.03.2020.
//
#include "TestFilter.h"
#include "constants.h"

TestFilter::TestFilter(){
    recievedEvent = nullptr;
}

TestFilter::~TestFilter() = default;

HRESULT IfaceCalling TestFilter::Configure(IFilter_Configuration* configuration, refcnt::wstr_list *error_description){
    //TODO
    return 0;
}

HRESULT IfaceCalling TestFilter::Execute(scgms::IDevice_Event *event) {

    if (event == nullptr)
    {
        return E_FAIL;
    }
    event->Raw(&recievedEvent);
    return S_OK;
}

scgms::TDevice_Event* TestFilter::getRecievedEvent()
{
    return recievedEvent;
}