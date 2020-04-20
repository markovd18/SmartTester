//
// Created by David on 11.04.2020.
//
#include "UnitTester.h"
#include "../../smartcgms/src/common/iface/DeviceIface.h"
#include <iostream>

UnitTester::UnitTester(CDynamic_Library* library, TestFilter* testFilter) {
    this->library = library;
    this->testFilter = testFilter;
    //TODO dopsat spuštìní všech testù, pokud nezadán guid
}

UnitTester::UnitTester(CDynamic_Library* library, TestFilter* testFilter, GUID* guid) {
    this->library = library;
    this->testFilter = testFilter;
    this->tested_guid = guid;
    this->loadLibrary();
}

void UnitTester::loadLibrary() {
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
