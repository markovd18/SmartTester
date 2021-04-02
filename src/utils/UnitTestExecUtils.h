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
     * Executes all defined tests upon models in given module.
     * @param lib path to a module with models
     */
    void executeModelTests(const wchar_t *lib);

    /**
     * Executes all defined tests upon metrics in given module.
     * @param lib path to a module with metrics
     */
    void executeMetricTests(const wchar_t *lib);

    /**
     * Executes all defined unit tests across all entities.
     */
    void executeAllTests();

    /**
     * Configures given filter with passed configuration.
     * @param filter filter to configure
     * @param config filter configuration
     * @return result of the configuration
     */
    HRESULT configureFilter(scgms::IFilter* filter, const tester::FilterConfig& config);

    /// Returns a unit tester instance based on given guid
    tester::FilterUnitTester *getUnitTester(const GUID &guid);
}

/**
 * Moves given file to a tmp folder next to an executable
 * @param filePath path to an existing file
 * @return true if success, otherwise false
 */
bool moveToTmp(const filesystem::path& filePath);
#endif //SMARTTESTER_UNITTESTER_H
