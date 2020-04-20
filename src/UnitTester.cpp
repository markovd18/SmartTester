//
// Created by David on 11.04.2020.
//
#include "UnitTester.h"
#include <iostream>

UnitTester::UnitTester(CDynamic_Library* library, TestFilter* testFilter) {
    this->library = library;
    this->testFilter = testFilter;
}

UnitTester::UnitTester(CDynamic_Library* library, TestFilter* testFilter, GUID* guid) {
    this->library = library;
    this->testFilter = testFilter;
    this->tested_guid = guid;
    this->loadLibrary();
    this->loadFilter();
}

void UnitTester::loadLibrary() {
    GuidFileMapper mapper = GuidFileMapper();

    library->Load(mapper.getFileName(*(this->tested_guid)));

    if (library->Is_Loaded()) {
        std::cout << "Library loaded successfully.\n";
    }
    else {
        std::cout << "Couldn't load library!\n";
    }
}

void UnitTester::loadFilter() {
    GuidFileMapper mapper = GuidFileMapper();

    library->Load(mapper.getFileName(*(this->tested_guid)));
    auto creator = library->Resolve<scgms::TCreate_Filter>("do_create_filter");

    scgms::IFilter* created_filter = nullptr;

    auto result = creator(tested_guid, testFilter, &created_filter);
    if (result == S_OK) {
        std::cout << "Filter created succesfully.";
    }
    else {
        std::cout << result;
    }
}

void UnitTester::infoEventTest() {

}
