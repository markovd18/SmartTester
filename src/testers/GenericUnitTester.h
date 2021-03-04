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
#include "FilterConfiguration.h"

namespace tester {
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
        /// Our custom filter for testing
        TestFilter m_testFilter;
        /// GUID of tested filter
        GUID m_testedGuid;
        /// Tested filter itself
        scgms::IFilter* m_testedFilter;

    public: // public methods
        explicit GenericUnitTester(GUID guid);
        virtual ~GenericUnitTester() = default;
        /**
         * If any filter is created, executes an info event upon it. Tested filter should send the info event to
         * the output filter, which will be TestFilter. If the event is not received by TestFilter, test ends with an error.
         */
        HRESULT infoEventTest();
        /**
         * If any filter is created, executes a warning event upon it. Tested filter should send the warning event to
         * the output filter, which will be TestFilter. If the event is not received by TestFilter, test ends with an error.
         */
        HRESULT warningEventTest();
        /**
         * If any filter is created, executes an error event upon it. Tested filter should send the error event to
         * the output filter, which will be TestFilter. If the event is not received by TestFilter, test ends with an error.
         */
        HRESULT errorEventTest();
        /**
         * If any filter is created, executes warm reset event upon it. Tested filter should send the warm reset event to
         * the output filter, which will be TestFilter. If the event is not received by TestFilter, test ends with an error.
         */
        HRESULT warmResetEventTest();
        /**
         * If any filter is created, executes shut down event upon it. Tested filter should send the shut down event to
         * the output filter, which will be TestFilter. If the event is not received by TestFilter, test ends with an error.
         * Shut down event ends filter's execution routine and it should not accept any other event after shutting down - it's Execute
         * method should return an error. If not, the test fails.
         */
        HRESULT shutDownEventTest();
        /**
         * Executes configuration test based on given parameters. Every filter should be successfully configured only if all parts
         * of the configuration are provided. Configuration test will be evaluated as successful and return S_OK only if the result of configuration
         * is identical with given expectedResult parameter, otherwise will return E_FAIL.
         *
         * @param memory configuration string
         * @param expectedResult expected result of this configuration
         * @return S_OK only if the result of given configuration is identical with given expectedResult parameter, otherwise E_FAIL
         */
        HRESULT configurationTest(const tester::FilterConfig& config, HRESULT expectedResult);
        bool isFilterLoaded();
        void executeAllTests();
        void executeGenericTests();
        /// Executes all tests for a specific filter. Needs to be implemented by derived class.
        virtual void executeSpecificTests() = 0;

    protected:
        /**
            Invokes test method passed as a parameter. Invoked method has to take in zero parameters and return HRESULT as a return value.
            @param testName name of the test which will be displayed in logs
            @param test method to be invoked by this method
        */
        void executeTest(const std::wstring& testName, const std::function<HRESULT(void)>& test);
        /**
         * Invokes test method passed as a parameter. Invoked method has to take in two parameters and return HRESULT as a return value.
         * The first parameter of invoked method is string and second one is HRESULT. Main purpose of this method is to invoke
         * configuration test method which takes in configuration string as first parameter and expected result as second parameter.
         * @param testName name of the test which will be displayed in logs
         * @param test method to be invoked by this method
         */
        void executeConfigTest(const std::wstring& testName, const tester::FilterConfig& configuration, HRESULT expectedResult);

        /// Creates shut down event and executes it with tested filter
        HRESULT shutDownTest();

        CDynamic_Library& getFilterLib();
        TestFilter& getTestFilter();
        scgms::IFilter* getTestedFilter();

    private: // private methods
        HRESULT informativeEventsTest(scgms::NDevice_Event_Code eventCode);
        scgms::IFilter* loadFilter();
        const wchar_t* getFilterName();
        HRESULT runTestInThread(const std::function<HRESULT(void)>& test);
        HRESULT runConfigTestInThread(const tester::FilterConfig& configuration, HRESULT expectedResult);
        void runTest(const std::function<HRESULT(void)>& test);
        void runConfigTest(const tester::FilterConfig& configuration, HRESULT expectedResult);
    };
}


#endif // !_GENERIC_UNIT_TESTER_H_

