//
// Author: markovd@students.zcu.cz
//

#include <iostream>
#include <rtl/guid.h>
#include "../UnitTestExecutor.h"
#include "../../mappers/GuidTesterMapper.h"
#include "../../mappers/GuidFileMapper.h"

void UnitTestExecutor::executeFilterTests(const GUID& guid) {
    if (Is_Invalid_GUID(guid)) {
        std::wcerr << L"Invalid GUID passed!\n";
        Logger::getInstance().error(L"Invalid GUID passed as parameter!");
        return;
    }
	
	GenericUnitTester* unitTester = getUnitTester(guid);
	if ((unitTester) && unitTester->isFilterLoaded()) {
		unitTester->executeAllTests();
	}
}

void UnitTestExecutor::executeAllTests() {
	Logger::getInstance().info(L"Executing all tests across all filters.");
	std::map<GUID, const wchar_t*> map = GuidFileMapper::GetInstance().getMap();

    for (const auto &guidPair : map) {
        executeFilterTests(guidPair.first);
    }
}


GenericUnitTester* UnitTestExecutor::getUnitTester(const GUID& guid) {
	return GuidTesterMapper::GetInstance().getTesterInstance(guid);
}