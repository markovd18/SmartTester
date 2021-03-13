//
// Author: markovd@students.zcu.cz
//

#ifndef _MASKING_FILTER_UNIT_TESTER_H_
#define _MASKING_FILTER_UNIT_TESTER_H_
#include "../testers/GenericUnitTester.h"
#include <rtl/hresult.h>

namespace tester {

    /**
     * * Derived class from GenericUnitTester responsible for testing of Masking filter.
     */
    class MaskingFilterUnitTester : public GenericUnitTester {
    public: // public methods
        MaskingFilterUnitTester();

        /**
         * Executes unit tests specific for masking filter.
         */
        void executeSpecificTests() override;

        HRESULT completeBitmaskMappingTest();

        /**
         * Executes info event masking unit test. When executed, info event should not be masked. If it is, the test fails.
         * @return S_OK if test passes, otherwise E_FAIL
         */
        HRESULT infoEventMaskingTest();

    private: // private methods
        HRESULT bitmaskMappingTest(const GUID& signalId, const std::string &bitmask);
    };
}

#endif // !_MASKING_FILTER_UNIT_TESTER_H_

