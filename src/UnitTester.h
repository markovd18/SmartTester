//
// Created by David on 11.04.2020.
//

#ifndef SMARTTESTER_UNITTESTER_H
#define SMARTTESTER_UNITTESTER_H

#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"
#include "TestFilter.h"
#include "GuidFileMapper.h"

class UnitTester {
private:
    CDynamic_Library* library;
    TestFilter* testFilter;
    GUID* tested_guid;
    void loadLibrary();
    void loadFilter();
public:
    UnitTester(CDynamic_Library* library, TestFilter* testFilter);
    UnitTester(CDynamic_Library* library, TestFilter* testFilter, GUID* guid);
    void executeAllTests();
    void infoEventTest();
    void log();
    //...

};
#endif //SMARTTESTER_UNITTESTER_H
