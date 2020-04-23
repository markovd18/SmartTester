#pragma once

#ifndef _GENERIC_UNIT_TESTER_H_
#define _GENERIC_UNIT_TESTER_H_

#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"
#include "TestFilter.h"

/**
    Contains generic tests and methods, which can be applied on any filter.
*/
class GenericUnitTester {
private:
    CDynamic_Library* library;
    TestFilter* testFilter;
    GUID* tested_guid;
    scgms::IFilter* testedFilter;

    void loadFilter();
    HRESULT runTestInThread(HRESULT(*test)());
    void printResult();
public:
    GenericUnitTester(CDynamic_Library* library, TestFilter* testFilter, GUID* guid);
    HRESULT infoEventTest();
    bool isFilterLoaded();
    virtual void executeAllTests();
    //...

};
#endif // !_GENERIC_UNIT_TESTER_H_

