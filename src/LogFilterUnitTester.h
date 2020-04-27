#pragma once

#ifndef _LOG_FILTER_UNIT_TESTER_H_
#define _LOG_FILTER_UNIT_TESTER_H_
#include "GenericUnitTester.h"

class LogFilterUnitTester : public GenericUnitTester{
public:
	LogFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter, GUID* guid);
	virtual void executeSpecificTests();
};
#endif // !_LOG_FILTER_UNIT_TESTER_H_
