#pragma once

#ifndef _CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
#define _CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
#include "GenericUnitTester.h"
#include "../../smartcgms/src/common/rtl/hresult.h"

class MappingFilterUnitTester : public GenericUnitTester {
private:
	std::string SIGNAL_SRC_ID_STR;
	GUID SIGNAL_SRC_ID_GUID;
	std::string SIGNAL_DST_ID_STR;
	GUID SIGNAL_DST_ID_GUID;
	HRESULT configureFilterCorrectly();
public:
	MappingFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter, const GUID* guid);
	virtual void executeSpecificTests();
	HRESULT emptySourceIdTest();
	HRESULT emptyDestIdTest();
	HRESULT correctIdsTest();
	HRESULT levelEventMappingTest();
	HRESULT infoEventMappingTest();
	HRESULT parametersEventMappingTest();
};
#endif // !_CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
