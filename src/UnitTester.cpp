//
// Created by David on 11.04.2020.
//
#include "UnitTester.h"
#include <iostream>

UnitTester::UnitTester(CDynamic_Library* library, TestFilter* testFilter){
    this->library = library;
    this->testFilter = testFilter;
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
    if (library->Is_Loaded()) {
        std::cout << "done";
    }
}
void UnitTester::infoEventTest() {

}
