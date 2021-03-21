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
        /**
         * When configured with @a Emit_Shutdown_Message = @a true, at the end of the log processing, shut down event should
         * be sent. This test checks, if this shut down event is sent in the end.
         * @return S_OK if the shut down event is sent, otherwise E_FAIL
         */
        HRESULT emitShutdownFlagTest();
        /**
         * When configured with a folder as an input, LogReplay filter should replay all valid logs in that folder.
         * This test checks, if all valid logs were replayed.
         * @return S_OK if all logs were replayed, otherwise E_FAIL
         */
        HRESULT logReplayFolderTest();
        /**
         * When configured with a @a Interpret_Filename_As_Segment_ID = @a true, segment id's in every log file
         * should be rewritten to a unique one. This test checks, whether the number of unique segment id's
         * on received events is higher or equal to the number of log files replayed.
         * @return S_OK if the number of unique segment id's on received events id higher or equal to the number of
         * replayed log files, otherwise E_FAIL
         */
        HRESULT filenameAsSegmentIdTest();
    private:    // private methods
        HRESULT invalidLogFileTest(const tester::LogReplayFilterConfig &config, const scgms::NDevice_Event_Code &expectedCode);
    };
}



#endif //SMARTTESTER_LOGREPLAYFILTERUNITTESTER_H
