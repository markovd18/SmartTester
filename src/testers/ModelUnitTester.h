//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_MODELUNITTESTER_H
#define SMARTTESTER_MODELUNITTESTER_H

#include "GenericUnitTester.h"

namespace tester {

    /**
     * Derived class with tests for individual models.
     */
    class ModelUnitTester : FilterUnitTester {
    public:     // public methods
        explicit ModelUnitTester(const GUID& guid, const std::wstring& libPath);
        void executeSpecificTests() override;
    };
}


#endif //SMARTTESTER_MODELUNITTESTER_H
