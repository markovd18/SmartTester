//
// Author: markovd@students.zcu.cz
//

#include <iostream>
#include <rtl/guid.h>
#include <utils/string_utils.h>
#include "../UnitTestExecUtils.h"
#include "../../mappers/GuidTesterMapper.h"
#include "../../mappers/GuidFileMapper.h"
#include "../constants.h"

void tester::executeFilterTests(const GUID& guid) {
    if (Is_Invalid_GUID(guid)) {
        std::wcerr << L"Invalid GUID passed!\n";
        Logger::getInstance().error(L"Invalid GUID passed as parameter!");
        return;
    }
	
	tester::FilterUnitTester* unitTester = getUnitTester(guid);
    unitTester->executeAllTests();
    delete unitTester;
}

void tester::executeAllTests() {
	Logger::getInstance().info(L"Executing all tests across all filters.");
	std::map<GUID, const wchar_t*> map = GuidFileMapper::GetInstance().getMap();

    for (const auto &guidPair : map) {
        executeFilterTests(guidPair.first);
    }
}

tester::FilterUnitTester* tester::getUnitTester(const GUID& guid) {
	return GuidTesterMapper::GetInstance().getTesterInstance(guid);
}

bool moveToTmp(const filesystem::path& filePath) {
    if (!filesystem::exists(filePath)) {
        return false;
    }

    std::string tmpDir = Narrow_WChar(cnst::TMP_DIR);
    filesystem::create_directory(tmpDir);
    if (!filesystem::exists(tmpDir)) {
        return false;
    }

    std::string dest = tmpDir + "/" +  filePath.filename().string();
    filesystem::rename(filePath, dest);
    return filesystem::exists(dest);
}