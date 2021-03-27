//
// Author: markovd@students.zcu.cz
//

#include "../ModelUnitTester.h"

tester::ModelUnitTester::ModelUnitTester(const GUID &guid, const std::wstring& libPath) : FilterUnitTester(guid, EntityType::MODEL) {
    setFilterLib(libPath);
}

void tester::ModelUnitTester::executeSpecificTests() {
    //TODO markovda run creation tests and generic tests afterwards
}
