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
        LogFilterUnitTester(const GUID& guid);
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
    };
}
#endif // !_LOG_FILTER_UNIT_TESTER_H_
