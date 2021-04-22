//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_MODELUNITTESTER_H
#define SMARTTESTER_MODELUNITTESTER_H

#include "GenericUnitTester.h"

namespace tester {

    /**
     * Derived class with tests for individual models.
     */
    class ModelUnitTester : public FilterUnitTester {
    private:    // private attributes
        /// Signal generator filter to create models with
        scgms::IFilter * m_signalGenerator = nullptr;
    public:     // public methods
        explicit ModelUnitTester(const GUID& guid, const std::wstring& libPath);
        void executeSpecificTests() override;

        /**
         * Every model should have Discrete_Model of Signal_Model flag in the descriptor.
         * This test checks if one of these flags is in model's descriptor.
         * @return S_OK if Discrete_Model is found, S_FALSE if Signal_Model is found and E_FAIL if error occurred or no valid flag found
         */
        HRESULT modelFlagTest();

        /**
         * If model has Discrete_Model flag, it should be possible to construct its object, otherwise it should not be possible.
         * This test checks, whether the model can be created and returns result based on given flags of the model.
         * @param modelFlag flags of created model
         * @return S_OK if creation result checks with given model flags, otherwise E_FAIL
         */
        HRESULT modelCreationTest(const scgms::NModel_Flags& modelFlag);

        /**
         * When the vector of parameters, passed to the model constructor, is shorter that it's descriptor says it should
         * be, filter should not be created. This test checks, if the size of given vector is validated.
         * @return S_OK if model was not created with invalid parameter count, otherwise E_FAIL
         */
        HRESULT invalidParameterCountTest();

        /**
         * Calling initialize method of a model while passing positive @a current_time value should not result
         * in an error. This test checks, whether the initialization with positive time passes or not.
         * @return S_OK if initialization passes, otherwise E_FAIL
         */
        HRESULT initializePositiveCurrentTimeTest();

        /**
         * Initialization of a model should be allowed to happen only once. When called repeatedly, every other
         * initialization should fail with @a E_ILLEGAL_STATE_CHANGE error. This test checks whether all following
         * initializations fail with expected error or not.
         * @return S_OK if all following initializations fail with @a E_ILLEGAL:STATE_CHANGE, otherwise E_FAIL
         */
        HRESULT repeatedInitializeCallTest();

        /**
         * Step method must not be called before @a Initialize was called. This test checks, whether calling
         * @a Step before @a Initialize returns an error or not.
         * @return S_OK if @a Step returns an error, otherwise E_FAIL
         */
        HRESULT stepBeforeInitializeTest();

        /**
         * Step method has to he called with non-negative @a time_advance_delta, because it says the state in which
         * time should be emitted. This test checks whether calling @a Step method with negative @a time_advance_delta
         * returns an error or not.
         * @return S_OK if calling @a Step with negative @a time_advance_delta returned an error, otherwise E_FAIL
         */
        HRESULT negativeStepDeltaTimeTest();

        /**
         * When Step method is called with @a time_advance_delta @a = @a 0.0, current state should always be emitted.
         * This test checks, whether events emitted by calling @a Step(0.0), even after stepping by @a N, represent
         * current time state of the model.
         * @return S_OK if correct events were emitted, otherwise E_FAIL
         */
        HRESULT currentTimeStepTest();

        /**
         * When Step method is called with positive @a time_advance_delta, future state of N units from now should be emitted.
         * This test checks, whether any events are emitted and if so, whether they represent the right time or not.
         * @return S_OK if correct events were emitted, otherwise E_FAIL
         */
        HRESULT futureTimeStepTest();

    private:    // private methods
        /// Helper method for creating a signal generator filter. It's output filter is set to the @a m_testFilter.
        HRESULT createSignalGeneratorFilter(scgms::IFilter **filter);
        /// Helper function for creating the model that the tester is configured to
        HRESULT createModel(scgms::IDiscrete_Model **model, scgms::IFilter* signalGenerator);
        HRESULT createModel(scgms::IDiscrete_Model **model, scgms::IFilter* signalGenerator, std::size_t parameterCount);
        /// Helper method for testing current time values during initialization
        HRESULT initialize(const double currentTime, const uint64_t segmentId, const bool shouldSucceed, const HRESULT expectedResult);
        /// Helper method for testing the @a Step method
        HRESULT step(const double timeAdvanceDelta, const bool shouldSucceed, const HRESULT expectedResult);
        /// Custom logic for loading the entity with filter interface
        void loadEntity() override;
    };
}


#endif //SMARTTESTER_MODELUNITTESTER_H
