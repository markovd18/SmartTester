#pragma once

#ifndef _LOG_FILTER_UNIT_TESTER_H_
#define _LOG_FILTER_UNIT_TESTER_H_
#include <rtl/hresult.h>
#include "GenericUnitTester.h"

/**
 * Derived class from GenericUnitTester responsible for testing of Log filter.
 */
class LogFilterUnitTester : public GenericUnitTester{
public:
    static const std::string FILTER_CONFIG;
    static const std::string LOG_FILE_GENERATION_TEST_LOG;
    static const std::string CORRECT_LOG_FILE_NAME_TEST_LOG;

	LogFilterUnitTester(const GUID& guid);
	void executeSpecificTests() override;
	HRESULT logFileGenerationTest();
};
#endif // !_LOG_FILTER_UNIT_TESTER_H_
