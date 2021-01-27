//
// Author: markovd@students.zcu.cz
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
	UnitTestExecutor() = default;
	/**
	 * Executes all defined unit tests upon a filter with given GUID.
	 * @param guid guid of a filter that is to be dested
	 */
	void executeFilterTests(const GUID &guid);
	/**
	 * Executes all defined unit tests across all filters.
	 */
	void executeAllTests();

private:
	/// Helper method for retrieving a unit tester instance based on given filter GUID.
	GenericUnitTester* getUnitTester(const GUID& guid);
};

#endif //SMARTTESTER_UNITTESTER_H
