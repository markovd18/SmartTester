//
// Author: markovd@students.zcu.cz
//

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
 * Contains generic tests and methods, which can be applied on any filter.
 */
class GenericUnitTester {
private: // private attibutes
    std::mutex m_testMutex;
    std::condition_variable m_testCv;
    HRESULT m_lastTestResult;

    /// Dynamic library of the tested filter
    CDynamic_Library m_filterLibrary;
    /// SCGMS dynamic library containing various functions needed for test execution
    CDynamic_Library m_scgmsLibrary;
    /// Our custom filter for testing
    TestFilter m_testFilter;
    /// GUID of tested filter
    GUID m_testedGuid;
    /// Tested filter itself
    scgms::IFilter* m_testedFilter;

public: // public methods
    GenericUnitTester(GUID guid);
    virtual ~GenericUnitTester() = default;
    /**
     * If any filter is created, executes an info event upon it. Tested filter should send the info event to
     * the output filter, which will be TestFilter. If the event is not received by TestFilter, test ends with an error.
     */
    HRESULT infoEventTest();
    HRESULT configurationTest(const std::string& memory, HRESULT expectedResult);
    bool isFilterLoaded();
    void executeAllTests();
    void executeGenericTests();
    // Executes all tests for a specific filter. Needs to be implemented by derived class.
    virtual void executeSpecificTests() = 0;

    /**
     * Creates device event based on given event code. Returned pointer is non-owning and shouldn't be deleted.
     * It will be deleted automatically during it's execution.
     *
     * @param eventCode event code describing
     * @return non-owning pointer to created event
     */
    scgms::IDevice_Event* createEvent(scgms::NDevice_Event_Code eventCode);

protected:
    void executeTest(const std::wstring& testName, const std::function<HRESULT(void)>& test);
    void executeConfigTest(const std::wstring& testName, const std::string& configuration, HRESULT expectedResult);

    /// Creates shut down event and executes it with tested filter
    HRESULT shutDownTest();

    CDynamic_Library& getScgmsLib();
    CDynamic_Library& getFilterLib();
    TestFilter& getTestFilter();
    scgms::IFilter* getTestedFilter();

private: // private methods
    scgms::IFilter* loadFilter();
    void loadScgmsLibrary();
    const wchar_t* getFilterName();
    HRESULT runTestInThread(const std::function<HRESULT(void)>& test);
    HRESULT runConfigTestInThread(const std::string& configuration, HRESULT expectedResult);
    void runTest(const std::function<HRESULT(void)>& test);
    void runConfigTest(const std::string& configuration, HRESULT expectedResult);
    void printResult(HRESULT result);
};


#endif // !_GENERIC_UNIT_TESTER_H_

