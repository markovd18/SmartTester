//
// Author: markovd@students.zcu.cz
//

#pragma once

#ifndef _CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
#define _CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
#include <rtl/hresult.h>
#include "../testers/GenericUnitTester.h"

/**
 * Derived class from GenericUnitTester responsible for the testing of Mapping filter.
 */
class MappingFilterUnitTester : public GenericUnitTester {
private: // private attributes
    static const std::string FILTER_CONFIG;
    static const std::string SIGNAL_SRC_ID_STR;
    static const GUID SIGNAL_SRC_ID_GUID;
    static const std::string SIGNAL_DST_ID_STR;
    static const GUID SIGNAL_DST_ID_GUID;

public: // public methods
	MappingFilterUnitTester(const GUID& guid);
    /**
     * Executes unit tests specific to mapping filter.
     */
	void executeSpecificTests() override;
	HRESULT levelEventMappingTest();
	HRESULT infoEventMappingTest();
	HRESULT parametersEventMappingTest();

private: // private methods
    /// Helper method for executing correct configuration
    HRESULT configureFilterCorrectly();
};
#endif // !_CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
