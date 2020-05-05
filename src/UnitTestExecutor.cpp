#include <iostream>
#include "UnitTestExecutor.h"
#include "LogFilterUnitTester.h"
#include "GuidTesterMapper.h"
#include "GuidFileMapper.h"
#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"
#include "../../smartcgms/src/common/rtl/guid.h"
#include "../../smartcgms/src/common/rtl/hresult.h"
#include "constants.h"

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
		exit(E_INVALIDARG);
	}
	//TODO GenericFilterExecutor.executeAllTests() podle zadaného guid
	
	GenericUnitTester* unitTester = getUnitTester(guid);
	unitTester->executeAllTests();
	delete unitTester;
}

/**
	Executes all tests across all filters.
*/
void UnitTestExecutor::executeAllTests() {
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
	CDynamic_Library library = CDynamic_Library();
	TestFilter testFilter = TestFilter();
	const wchar_t* fileName = GuidFileMapper::GetInstance().getFileName(guid);

	GenericUnitTester* unitTester = GuidTesterMapper::GetInstance().getTesterInstance(&library, &testFilter, &guid);
	return unitTester;
}