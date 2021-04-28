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
    public:     // public methods
        SignalGeneratorUnitTester();

        void executeSpecificTests() override;

        /// We need to override the loading behavior, because executing events upon signal generator
        /// that is not configured always fails with E_FAIL, therefore all generic tests would fail.
        /// Every test, requiring custom configuration, has to reconfigure the filter.
        void loadEntity() override;

        /**
         * If the model is not synchronized, it should be stepped only as far as the configured maximum time.
         * This test checks, whether this behavior is present or not. That means, that maximum of
         * @a (maximum_time/stepping)*(N+1) events should be emitted. (+1 for the initial state).
         * @return S_OK if the model was stepped only to the maximum time, otherwise E_FAIL
         */
        HRESULT asynchronousModeTest();

        /**
         * In asynchronous mode, when the signal generator is configured with @a shutdown_after_last flag, after
         * the generation is over, filter should emit a @a Shut_Down event. This test checks, whether the @a Shut_Down
         * event is emitted or not.
         * @return S_OK if the @a Shut_Down event was emitted, otherwise E_FAIL
         */
        HRESULT shutdownAfterLastTest();

        /**
         * When executing @a Time_Segment_Start event upon synchronous signal generator, it should emit the current model state.
         * This test checks, whether the current state is emitted or not.
         * @return S_OK if the current state of the model was emitted, otherwise E_FAIL
         */
        HRESULT timeSegmentStartTest();

        /**
         * Executing @a Time_Segment_Start event starts generator's control loop for given segment. That means, that it should
         * be allowed to execute this loop only once for each segment. This test checks, whether executing @a Time_Segment_Start
         * event for the same segment twice will end with failure or not.
         * @return S_OK if the execution of second @a Time_Segment_Start with the same @a segment_id failed, otherwise E_FAIL
         */
        HRESULT twiceIdenticalTimeSegmentStartTest();

        /**
         * Synchronous signal generator works in a way that it calls @a Step method of the model with configured @a stepping value
         * only when there is an event executed upon the signal generator that has @a device_time higher than the event that caused
         * the last calling of the @a Step method by the @a stepping value.
         * <br><br>
         * When the executed event has the @a device_time increased by less than the @a stepping value, no event should be emitted
         * by the signal generator. This test checks, whether this event is emitted or not.
         * @return S_OK if event with @a device_time increased by less than the @a stepping value to the last event, that caused
         * the execution of @a Step method, is not emitted, otherwise E_FAIL
         */
        HRESULT eventDeviceTimeLessThanSteppingTest();

        /**
         * Synchronous signal generator works in a way that it calls @a Step method of the model with configured @a stepping value
         * only when there is an event executed upon the signal generator that has @a device_time higher than the event that caused
         * the last calling of the @a Step method by the @a stepping value.
         * <br><br>
         * When the executed event has the @a device_time increased by a little bit more than the @a stepping value,
         * the @a Step method of the model should be called and next state in time increased by @a stepping should be emitted.
         * This test checks if this next state is emitted or not.
         * @return S_OK if next state in time increased by @a stepping is emitted, otherwise E_FAIL
         */
        HRESULT eventDeviceTimeAsSteppingTest();

        /**
         * Synchronous signal generator works in a way that it calls @a Step method of the model with configured @a stepping value
         * only when there is an event executed upon the signal generator that has @a device_time higher than the event that caused
         * the last calling of the @a Step method by the @a stepping value.
         * <br><br>
         * When the executed event has the @a device_time increased by several @a steppings, the @a Step method of the model
         * should be called as many times with linearly increased stepping by @a stepping value as it fits into the event's
         * @a device_time value and each time the current state in that time should be emitted. This test checks whether
         * these states are emitted or not.
         * @return S_OK if all states in future times are emitted, otherwise E_FAIL
         */
        HRESULT eventDeviceTimeAsSeveralSteppingsTest();

        /**
         * When the synchronous signal generator is configured with the flag @a Echo_Default_Parameters_As_Event, the first event
         * that is emitted by the signal generator should be a @a Parameters event with values of the parameters passed
         * in a vector through the configuration. This test checks whether this event is emitted as the first one or not.
         * @return S_OK if the @a Parameters event is emitted as the first event, otherwise E_FAIL
         */
        HRESULT echoDefaultParametersAsEventTest();
    };
}


#endif //SMARTTESTER_SIGNALGENERATORUNITTESTER_H
