#pragma once

#ifndef _LOG_FILTER_UNIT_TESTER_H_
#define _LOG_FILTER_UNIT_TESTER_H_
#include "GenericUnitTester.h"
#include "../../smartcgms/src/common/rtl/hresult.h"

class LogFilterUnitTester : public GenericUnitTester{
public:
	LogFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid);
	virtual void executeSpecificTests();
	HRESULT emptyLogFileNameTest();
	HRESULT correctLogFileNameTest();
	HRESULT logFileGenerationTest();
};
#endif // !_LOG_FILTER_UNIT_TESTER_H_
