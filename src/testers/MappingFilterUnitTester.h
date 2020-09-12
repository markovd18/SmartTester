#pragma once

#ifndef _CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
#define _CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
#include <rtl/hresult.h>
#include "../testers/GenericUnitTester.h"

class MappingFilterUnitTester : public GenericUnitTester {
private:
	HRESULT configureFilterCorrectly();
public:
    static const std::string FILTER_CONFIG;
    static const std::string SIGNAL_SRC_ID_STR;
    static const GUID SIGNAL_SRC_ID_GUID;
    static const std::string SIGNAL_DST_ID_STR;
    static const GUID SIGNAL_DST_ID_GUID;

	MappingFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter, const GUID* guid);
	void executeSpecificTests() override;
	HRESULT levelEventMappingTest();
	HRESULT infoEventMappingTest();
	HRESULT parametersEventMappingTest();
};
#endif // !_CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
