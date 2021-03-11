//
// Author: markovd@students.zcu.cz
//

#pragma once

#ifndef _CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
#define _CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
#include <rtl/hresult.h>
#include "../testers/GenericUnitTester.h"

namespace tester {
    /**
     * Derived class from GenericUnitTester responsible for the testing of Mapping filter.
     */
    class MappingFilterUnitTester : public GenericUnitTester {
    private: // private attributes
        static const GUID SIGNAL_SRC_ID_GUID;
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
    };
}
#endif // !_CSV_LOG_FILE_FILTER_UNIT_TESTER_H_
