#pragma once

#ifndef _DRAWING_FILTER_UNIT_TESTER_H_
#define _DRAWING_FILTER_UNIT_TESTER_H_

#include "GenericUnitTester.h"
#include <rtl/hresult.h>

/**
 * Derived class from GenericUnitTester responsible for unit testing of Drawing filter.
 */
class DrawingFilterUnitTester : public GenericUnitTester {
public:
    static const std::string FILTER_CONFIG;

	DrawingFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter, const GUID* guid);
	void executeSpecificTests() override;
};
#endif // !_DRAWING_FILTER_UNIT_TESTER_H_
