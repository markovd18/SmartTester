#pragma once

#ifndef _MASKING_FILTER_UNIT_TESTER_H_
#define _MASKING_FILTER_UNIT_TESTER_H_
#include "../testers/GenericUnitTester.h"
#include <rtl/hresult.h>

class MaskingFilterUnitTester : public GenericUnitTester {
private:
	GUID SIGNAL_ID_GUID;
	std::string SIGNAL_ID_STR;

	HRESULT configureFilterCorrectly(std::string &bitmask);
public:
	MaskingFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter, const GUID* guid);
	void executeSpecificTests() override;
	HRESULT completeBitmaskMappingTest();
	HRESULT bitmaskMappingTest(std::string &bitmask);
	HRESULT infoEventMaskingTest();

};
#endif // !_MASKING_FILTER_UNIT_TESTER_H_
