//
// Author: markovd@students.zcu
//

#ifndef SMARTTESTER_LOGREPLAYFILTERUNITTESTER_H
#define SMARTTESTER_LOGREPLAYFILTERUNITTESTER_H

#include "GenericUnitTester.h"

namespace tester {

    /**
     * Derived class for execution of log replay filter unit tests.
     */
    class LogReplayFilterUnitTester : public FilterUnitTester {

    public: // public methods
        LogReplayFilterUnitTester();
        void executeSpecificTests() override;

        /**
         * When configured with log that has invalid header, LogReplay filter should send a warning event to indicate that
         * the header is corrupted. This test checks whether the warning event is sent or not.
         * @return S_OK if the warning event is sent, otherwise E_FAIL
         */
        HRESULT invalidHeaderLogFileTest();
        /**
         * When configured with invalid log, that cannot be parsed, LogReplay filter should send an error event to indicate parsing error.
         * This test checks, whether the error event is sent or not.
         * @return S_OK if the error event is sent, otherwise E_FAIL
         */
        HRESULT invalidBodyLogFileTest();
        /**
         * When configured with valid log, number of emitted events from LogReplay filter should be the same as the number
         * of non-header lines in the log file.
         * This test checks, whether the number of emitted events is the same as the number of log lines.
         * @return S_OK if the event count checks, otherwise false
         */
        HRESULT emittedEventCountTest();
    private:    // private methods
        HRESULT invalidLogFileTest(const tester::LogReplayFilterConfig &config, const scgms::NDevice_Event_Code &expectedCode);
    };
}



#endif //SMARTTESTER_LOGREPLAYFILTERUNITTESTER_H
