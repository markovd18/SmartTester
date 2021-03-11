//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_UNITTESTER_H
#define SMARTTESTER_UNITTESTER_H
#include <rtl/guid.h>
#include <rtl/FilesystemLib.h>
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

/**
 * Moves given file to a tmp folder next to an executable
 * @param filePath path to an existing file
 * @return true if success, otherwise false
 */
bool moveToTmp(const filesystem::path& filePath);
#endif //SMARTTESTER_UNITTESTER_H
