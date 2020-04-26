//
// Created by Martin on 22.4.2020.
//
#include "ReggressionTester.h"
#include "../../smartcgms/src/common/iface/DeviceIface.h"
#include <iostream>

UnitTester::UnitTester(CDynamic_Library* library, String) {
    this->library = library;
    this->str_konfig = str_konfig;
    //TODO dopsat spuštìní všech testù, pokud nezadán guid
}



void ReggressionTester::loadLibrary() {
    GuidFileMapper mapper = GuidFileMapper();

    library->Load(mapper.getFileName(*(this->tested_guid)));

    if (!library->Is_Loaded()) {
        std::wcerr << L"Couldn't load library!\n";
        exit(S_FALSE);
    }

    auto creator = library->Resolve<scgms::TCreate_Filter>("do_create_filter");

    scgms::IFilter* created_filter = nullptr;
    auto result = creator(tested_guid, testFilter, &created_filter);
    if (result != S_OK) {
        std::wcerr << L"Failed creating filter!\n";
        exit(S_FALSE);
    }
    this->testedFilter = created_filter;
}

void UnitTester::infoEventTest() {
    // poslat info event do testovaného filtru, èekat, jestli vypadne ven a jestli je stále info
}