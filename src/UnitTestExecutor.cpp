#include <iostream>
#include "UnitTestExecutor.h"
#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"
#include "../../smartcgms/src/common/rtl/guid.h"
#include "../../smartcgms/src/common/rtl/hresult.h"
#include "constants.h"

UnitTestExecutor::UnitTestExecutor() {
	CDynamic_Library::Set_Library_Base(LIB_DIR);
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
	CDynamic_Library library;
	
}

/**
	Executes all tests across all filters.
*/
void UnitTestExecutor::executeAllTests() {
	// projít všechny filtry a spustit všechny testy, asi podle mapy
}