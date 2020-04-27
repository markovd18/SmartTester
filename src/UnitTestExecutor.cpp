#include <iostream>
#include "UnitTestExecutor.h"
#include "LogFilterUnitTester.h"
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
void UnitTestExecutor::executeFilterTests(GUID& guid) {
	if (Is_Invalid_GUID(guid))
	{
		std::wcerr << L"Invalid GUID passed!\n";
		exit(E_INVALIDARG);
	}
	//TODO GenericFilterExecutor.executeAllTests() podle zadaného guid
	CDynamic_Library library = CDynamic_Library();
	TestFilter testFilter = TestFilter();
	LogFilterUnitTester unitTester = LogFilterUnitTester(&library, &testFilter, &guid);
	unitTester.executeGenericTests();
}

/**
	Executes all tests across all filters.
*/
void UnitTestExecutor::executeAllTests() {
	// projít všechny filtry a spustit všechny testy, asi podle mapy
}