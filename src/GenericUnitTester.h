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

    std::mutex testMutex;
    std::condition_variable testCv;
    HRESULT lastTestResult;

    void loadFilter();
    HRESULT runTestInThread(HRESULT(*test)());
    void runTest(HRESULT(* test)());
    void executeTest(HRESULT(* test)());
    void printResult(HRESULT result);
public:
    GenericUnitTester(CDynamic_Library* library, TestFilter* testFilter, GUID* guid);
    HRESULT infoEventTest();
    bool isFilterLoaded();
    void executeAllTests();
    void executeGenericTests();
    // Executes all tests for a specific filter. Needs to be implemented by derived class.
    virtual void executeSpecificTests() = 0;
    //...

};


#endif // !_GENERIC_UNIT_TESTER_H_

