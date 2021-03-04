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
        static const std::string FILTER_CONFIG;
        static const std::string LOG_FILE_GENERATION_TEST_LOG;
        static const std::string CORRECT_LOG_FILE_NAME_TEST_LOG;

        LogFilterUnitTester(const GUID& guid);
        void executeSpecificTests() override;
        /**
         * If LogFilter is successfully configured with Log_File attribute value present, text file with identical name should be created.
         * This method tests if it is true. Returns S_OK only if the log file is created, otherwise returns  E_FAIL.
         *
         * @return S_OK if log file is created, otherwise E_FAIL
         */
        HRESULT logFileGenerationTest();
    };
}
#endif // !_LOG_FILTER_UNIT_TESTER_H_
