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
    class ModelUnitTester : FilterUnitTester {
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

    private:    // private methods
        /// Helper method for creating a signal generator filter. It's output filter is set to the @a m_testFilter.
        HRESULT createSignalGeneratorFilter(scgms::IFilter **filter);
        /// Helper function for creating the model that the tester is configured to
        HRESULT createModel(scgms::IDiscrete_Model **model, scgms::IFilter* signalGenerator);
        HRESULT createModel(scgms::IDiscrete_Model **model, scgms::IFilter* signalGenerator, std::size_t parameterCount);
        /// Custom logic for loading the entity with filter interface
        void loadFilter() override;
    };
}


#endif //SMARTTESTER_MODELUNITTESTER_H
