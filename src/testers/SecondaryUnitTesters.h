//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_SECONDARYUNITTESTERS_H
#define SMARTTESTER_SECONDARYUNITTESTERS_H


#include <iface/SolverIface.h>
#include <iface/ApproxIface.h>
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

    /**
     * Class for testing @a scgms::IApproximator interface.
     */
    class ApproximatorUnitTester : public EntityUnitTester<scgms::IApproximator> {
    private:    // private attributes
        /// Number of values we set to the signal
        static constexpr std::size_t s_signalValuesCount = 10;
        /// Times we set to the signal
        static constexpr std::array<double, s_signalValuesCount> s_times { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
        /// Measured levels we set to the signal
        static constexpr std::array<double, s_signalValuesCount> s_levels { 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0, 512.0, 1024.0};
    public:     // public methods
        ApproximatorUnitTester(const GUID& guid, const std::wstring& libraryPath);
        void loadEntity() override;
        void executeAllTests() override;
        HRESULT shutDownTest() override;

        HRESULT middleTimeApproximationTest();

        HRESULT middleTimeDerivativeApproximationTest();

    private:    // private methods
        HRESULT calculateApproximations(const std::size_t derivativeOrder = 0);
    };
}


#endif //SMARTTESTER_SECONDARYUNITTESTERS_H
