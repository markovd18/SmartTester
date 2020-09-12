#pragma once

#ifndef _GENERIC_UNIT_TESTER_H_
#define _GENERIC_UNIT_TESTER_H_

#include <mutex>
#include <functional>
#include <condition_variable>
#include <rtl/Dynamic_Library.h>
#include <rtl/hresult.h>
#include "../utils/TestFilter.h"
#include "../utils/Logger.h"

/**
    Contains generic tests and methods, which can be applied on any filter.
*/
class GenericUnitTester {
private:
    std::mutex testMutex;
    std::condition_variable testCv;
    HRESULT lastTestResult;

    void loadFilter();
    void loadScgmsLibrary();
    const wchar_t* getFilterName();
    HRESULT runTestInThread(const std::function<HRESULT(void)>& test);
    HRESULT runConfigTestInThread(const std::string& configuration, HRESULT expectedResult);
    void runTest(const std::function<HRESULT(void)>& test);
    void runConfigTest(const std::string& configuration, HRESULT expectedResult);
    void printResult(HRESULT result);

protected:
    CDynamic_Library* filterLibrary;
    CDynamic_Library* scgmsLibrary;
    TestFilter* testFilter;
    const GUID* tested_guid;
    scgms::IFilter* testedFilter;

    void executeTest(const std::wstring& testName, const std::function<HRESULT(void)>& test);
    void executeConfigTest(const std::wstring& testName, const std::string& configuration, HRESULT expectedResult);
    HRESULT shutDownTest();

public:
    Logger& logger = Logger::GetInstance();
    GenericUnitTester(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid);
    virtual ~GenericUnitTester();
    HRESULT infoEventTest();
    HRESULT configurationTest(const std::string& memory, HRESULT expectedResult);
    bool isFilterLoaded();
    void executeAllTests();
    void executeGenericTests();
    // Executes all tests for a specific filter. Needs to be implemented by derived class.
    virtual void executeSpecificTests() = 0;
    //...

};


#endif // !_GENERIC_UNIT_TESTER_H_

