//
// Created by David on 11.04.2020.
//

#ifndef SMARTTESTER_UNITTESTER_H
#define SMARTTESTER_UNITTESTER_H

#include "../../smartcgms/src/common/rtl/guid.h"

/**
	Executes all implemented tests for a filter, when passing valid GUID, 
	or executes all implemented tests across all filters.
*/
class UnitTestExecutor {

public:
	UnitTestExecutor();
	void executeFilterTests(GUID &guid);
	void executeAllTests();
};
#endif //SMARTTESTER_UNITTESTER_H
