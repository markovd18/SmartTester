#include <iostream>
#include <rtl/Dynamic_Library.h>
#include <rtl/guid.h>
#include <rtl/hresult.h>
#include "../UnitTestExecutor.h"
#include "../../mappers/GuidTesterMapper.h"
#include "../../mappers/GuidFileMapper.h"

UnitTestExecutor::UnitTestExecutor() = default;

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
	if ((unitTester) && unitTester->isFilterLoaded())
	{
		unitTester->executeAllTests();
	}
	
	delete unitTester;
}

/**
	Executes all tests across all filters.
*/
void UnitTestExecutor::executeAllTests() {
	logger.info(L"Executing all tests across all filters.");
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
	auto *library = new CDynamic_Library;
	auto *testFilter = new TestFilter;
	GenericUnitTester* unitTester = GuidTesterMapper::GetInstance().getTesterInstance(library, testFilter, &guid);
	return unitTester;
}