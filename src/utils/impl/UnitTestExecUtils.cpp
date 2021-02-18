//
// Author: markovd@students.zcu.cz
//

#include <iostream>
#include <rtl/guid.h>
#include "../UnitTestExecUtils.h"
#include "../../mappers/GuidTesterMapper.h"
#include "../../mappers/GuidFileMapper.h"

void tester::executeFilterTests(const GUID& guid) {
    if (Is_Invalid_GUID(guid)) {
        std::wcerr << L"Invalid GUID passed!\n";
        Logger::getInstance().error(L"Invalid GUID passed as parameter!");
        return;
    }
	
	tester::GenericUnitTester* unitTester = getUnitTester(guid);
	if (unitTester->isFilterLoaded()) {
		unitTester->executeAllTests();
	}
}

void tester::executeAllTests() {
	Logger::getInstance().info(L"Executing all tests across all filters.");
	std::map<GUID, const wchar_t*> map = GuidFileMapper::GetInstance().getMap();

    for (const auto &guidPair : map) {
        executeFilterTests(guidPair.first);
    }
}

tester::GenericUnitTester* tester::getUnitTester(const GUID& guid) {
	return GuidTesterMapper::GetInstance().getTesterInstance(guid);
}