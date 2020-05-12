#include <iostream>
#include "UnitTestExecutor.h"
#include "LogFilterUnitTester.h"
#include "GuidTesterMapper.h"
#include "GuidFileMapper.h"
#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"
#include "../../smartcgms/src/common/rtl/guid.h"
#include "../../smartcgms/src/common/rtl/hresult.h"
#include "constants.h"
#include "Logger.h"

UnitTestExecutor::UnitTestExecutor() {
	//
}

/**
	Executes all tests on a filter represented by given GUID.
*/
void UnitTestExecutor::executeFilterTests(const GUID& guid) {
	if (Is_Invalid_GUID(guid))
	{
		std::wcerr << L"Invalid GUID passed!\n";
		logger.error(L"Invalid GUID passed as parameter!");
		exit(E_INVALIDARG);
	}
	
	GenericUnitTester* unitTester = getUnitTester(guid);
	if (unitTester->isFilterLoaded())
	{
		unitTester->executeAllTests();
	}
	
	delete unitTester;
}

/**
	Executes all tests across all filters.
*/
void UnitTestExecutor::executeAllTests() {
	logger.info(L"Executing all tests...");
	std::map<GUID, const wchar_t*>::iterator iterator;
	std::map<GUID, const wchar_t*> map = GuidFileMapper::GetInstance().getMap();

	for (iterator = map.begin(); iterator != map.end(); iterator++) {
		executeFilterTests(iterator->first);
	}
}

/**
	Returns instance of derived unit tester class based on given guid.
*/
GenericUnitTester* UnitTestExecutor::getUnitTester(const GUID& guid) {
	CDynamic_Library *library = new CDynamic_Library;
	TestFilter *testFilter = new TestFilter;
	GenericUnitTester* unitTester = GuidTesterMapper::GetInstance().getTesterInstance(library, testFilter, &guid);
	return unitTester;
}