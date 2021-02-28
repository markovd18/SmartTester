//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_UNITTESTER_H
#define SMARTTESTER_UNITTESTER_H
#include <rtl/guid.h>
#include "../testers/GenericUnitTester.h"
#include "Logger.h"

namespace tester {

    /**
     * Executes all defined unit tests upon a filter with given GUID.
     * @param guid guid of a filter that is to be dested
     */
    void executeFilterTests(const GUID &guid);

    /**
     * Executes all defined unit tests across all filters.
     */
    void executeAllTests();


    /// Returns a unit tester instance based on given guid
    tester::GenericUnitTester *getUnitTester(const GUID &guid);
}
#endif //SMARTTESTER_UNITTESTER_H
