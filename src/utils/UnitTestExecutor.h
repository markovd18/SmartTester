//
// Created by David on 11.04.2020.
//

#ifndef SMARTTESTER_UNITTESTER_H
#define SMARTTESTER_UNITTESTER_H
#include <rtl/guid.h>
#include "../testers/GenericUnitTester.h"
#include "Logger.h"

/**
	Executes all implemented tests for a filter, when passing valid GUID, 
	or executes all implemented tests across all filters.
*/
class UnitTestExecutor {

public:
	Logger& logger = Logger::GetInstance();
	UnitTestExecutor();
	void executeFilterTests(const GUID &guid);
	void executeAllTests();

private:
	GenericUnitTester* getUnitTester(const GUID& guid);
};

#endif //SMARTTESTER_UNITTESTER_H
