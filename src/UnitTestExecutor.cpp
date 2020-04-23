#include <iostream>
#include "UnitTestExecutor.h"
#include "GuidFileMapper.h"
#include "../../smartcgms/src/common/rtl/guid.h"

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
		return;
	}
	
}