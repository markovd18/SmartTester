//
// Author: markovd@students.zcu.cz
//

#ifndef _DRAWING_FILTER_UNIT_TESTER_H_
#define _DRAWING_FILTER_UNIT_TESTER_H_

#include "GenericUnitTester.h"
#include <rtl/hresult.h>

namespace tester {
    /**
     * Derived class from GenericUnitTester responsible for unit testing of Drawing filter.
     */
    class DrawingFilterUnitTester : public GenericUnitTester {
    public:
        static const std::string FILTER_CONFIG;

        DrawingFilterUnitTester(const GUID& guid);
        void executeSpecificTests() override;
        /**
         * Executes an information event upon created and configured Drawing filter.
         */
        HRESULT imageGenerationTest();
    };
}
#endif // !_DRAWING_FILTER_UNIT_TESTER_H_
