#pragma once

#ifndef _DRAWING_FILTER_UNIT_TESTER_H_
#define _DRAWING_FILTER_UNIT_TESTER_H_

#include "GenericUnitTester.h"
#include "../../smartcgms/src/common/rtl/hresult.h"

class DrawingFilterUnitTester : public GenericUnitTester {
public:
	DrawingFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter, const GUID* guid);
	virtual void executeSpecificTests();
	HRESULT emptyCanvasSizeTest();
	HRESULT correctCanvasSizeTest();
};
#endif // !_DRAWING_FILTER_UNIT_TESTER_H_
