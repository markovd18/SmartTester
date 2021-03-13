#pragma once

#ifndef _LOG_FILTER_UNIT_TESTER_H_
#define _LOG_FILTER_UNIT_TESTER_H_
#include <rtl/hresult.h>
#include "GenericUnitTester.h"

namespace tester {

    /**
     * Derived class from GenericUnitTester responsible for testing of Log filter.
     */
    class LogFilterUnitTester : public GenericUnitTester {
    public:
        LogFilterUnitTester();
        void executeSpecificTests() override;
        /**
         * If LogFilter is successfully configured with Log_File attribute value present, text file with identical name should be created.
         * This method tests if it is true. Returns S_OK only if the log file is created, otherwise returns  E_FAIL.
         *
         * @return S_OK if log file is created, otherwise E_FAIL
         */
        HRESULT logFileGenerationTest();
        /**
         * When there is N events executed upon the LogFilter, there should be a line for every single event
         * that was executed, plus one for the header.
         * This test will execute 3 level events and a shutdown event upon the LogFilter and checks, whether there are
         * 5 lines present in the generated log file.
         * @return S_OK if log file contains 5 lines, otherwise E_FAIL
         */
        HRESULT logRowCountTest();
        /**
         * When there is an event executed upon the LogFilter, there should be a line describing the executed event.
         * This test executes one level event upon the LogFilter and then checks if the event was logged into the
         * log file.
         * @return S_OK if the log file contains record of level event, otherwise E_FAIL
         */
        HRESULT filterOutputTest();
        /**
         * When there are events executed upon LogFilter in specific order, there should be records of these events
         * in the log file in the same order.
         * This test executes level, info and parameters event upon LogFilter and checks if the records of these events
         * in the log file are in correct order.
         * @return S_OK if the log file contains records of executed events in correct order, otherwise E_FAIL
         */
        HRESULT eventOrderTest();
        /**
         * Executing Pop method should always return the difference between this and the last calling.
         * This test executes level event and checks the Pop interface twice. At first Pop should return the executed
         * event and the second time the event should not be returned anymore.
         * @return S_OK if the executed event is not returned twice, otherwise E_FAIL
         */
        HRESULT popResultRepeatingTest();
        /**
         * Executing Pop method should always return a line for every event, that was executed upon the filter.
         * This test executes three level events upon the LogFilter and checks, whether the Pop method returns tree log records.
         * @return S_OK if three log records are returned from the Pop method, otherwise E_FAIL
         */
        HRESULT popEventCountTest();
    };
}
#endif // !_LOG_FILTER_UNIT_TESTER_H_
