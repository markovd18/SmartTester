#pragma once

#ifndef _LOG_FILTER_UNIT_TESTER_H_
#define _LOG_FILTER_UNIT_TESTER_H_
#include "GenericUnitTester.h"
#include "Logger.h"

class LogFilterUnitTester : public GenericUnitTester{
public:
	Logger logger;
	LogFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid);
	virtual void executeSpecificTests();
};
#endif // !_LOG_FILTER_UNIT_TESTER_H_
