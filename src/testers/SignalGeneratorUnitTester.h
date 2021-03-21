//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_SIGNALGENERATORUNITTESTER_H
#define SMARTTESTER_SIGNALGENERATORUNITTESTER_H

#include "GenericUnitTester.h"

namespace tester {

    /**
     * Derived class used for testing of the Signal Generator filter.
     */
    class SignalGeneratorUnitTester : public FilterUnitTester {
    public:
        SignalGeneratorUnitTester();

        void executeSpecificTests() override;
    };
}


#endif //SMARTTESTER_SIGNALGENERATORUNITTESTER_H
