//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_METRICUNITTESTER_H
#define SMARTTESTER_METRICUNITTESTER_H


#include <iface/SolverIface.h>
#include "GenericUnitTester.h"

namespace tester {

    /**
     * Class for testing @a scgms::IMetric interface.
     */
    class MetricUnitTester : public EntityUnitTester<scgms::IMetric> {
    public:     // public methods
        MetricUnitTester(const GUID& guid, const std::wstring& libraryPath);
        void executeAllTests() override;
        void loadEntity() override;
        HRESULT shutDownTest() override;

        /**
         * Every metric has to meet the identity requirement - if calculated and reference values are equal, calculated metric
         * has to be zero. (@a d(a,a)=0). This test checks  whether the tested metric meets this requirement or not.
         * @return S_OK if metric meets identity requirement, otherwise E_FAIL
         */
        HRESULT identityTest();

        /**
         * Every metric has to meet symmetry requirement - calculated metric from two values has to be equal when first
         * is passed as reference value and second is passed as calculated value and when passed in reversed order. (@a d(a,b)=d(b,a)).
         * This test checks, whether the tested metric meets this requirement or not.
         * @return S_OK if metric meets symmetry requirement, otherwise E_FAIL
         */
        HRESULT symmetryTest();

        /**
         * Every metric has to meet triangle inequality requirement - calculated metric with values @a A and @a C has to be less or equal
         * to the sum of calculated metrics with values @a A and @a B, and @a B and @a C. (@a d(a,c)<=d(a,b)+d(b,c)). This test checks
         * whether the metric meets this requirement or not.
         * @return S_OK if metric meets triable inequality requirement, otherwise E_FAIL
         */
        HRESULT triangleInequalityTest();

    private:    // private methods
        /// Helper method for calculating metric. All vectors must have the same size, otherwise calculating may not work correctly.
        double calculateMetric(const std::vector<double>& times, const std::vector<double>& reference, const std::vector<double>& calculated);
    };
}


#endif //SMARTTESTER_METRICUNITTESTER_H
