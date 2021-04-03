//
// Author: markovd@students.zcu.cz
//

#include <rtl/DeviceLib.h>
#include "../SecondaryUnitTesters.h"
#include "../../utils/LogUtils.h"

tester::MetricUnitTester::MetricUnitTester(const GUID &guid, const std::wstring &libraryPath)
        : EntityUnitTester<scgms::IMetric>(guid) {
    setEntityLib(libraryPath);
}

void tester::MetricUnitTester::executeAllTests() {
    const wchar_t* entityName = getEntityName<scgms::TGet_Metric_Descriptors, scgms::TMetric_Descriptor>("do_get_metric_descriptors");
    logs::printEntityTestsStartInfo(entityName, L"metric");

    executeTest(L"identity test", std::bind(&MetricUnitTester::identityTest, this));
    executeTest(L"symmetry test", std::bind(&MetricUnitTester::symmetryTest, this));
    executeTest(L"triangle inequality test", std::bind(&MetricUnitTester::triangleInequalityTest, this));
}

HRESULT tester::MetricUnitTester::identityTest() {
    std::vector<double> times { 0.01, 0.02, 0.03, 0.04, 0.05};
    std::vector<double> reference { 1, 2, 3, 4, 5 };

    double metric = calculateMetric(times, reference, reference);

    double epsilon = 0.0001;
    /// Metric must have an identity trait - if calculated value = reference value => metric must be 0
    if ((metric - 0.0) > epsilon) {
        Logger::getInstance().error(L"Metric does not meet identity requirement!");
        Logger::getInstance().error(L"Calculated metric: " + std::to_wstring(metric));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Metric meets identity requirement.");
    return S_OK;
}

HRESULT tester::MetricUnitTester::symmetryTest() {
    std::vector<double> times { 0.06, 0.07, 0.08, 0.09, 0.1, 0.11};
    std::vector<double> reference { 6, 7, 8, 9, 10, 11 };
    std::vector<double> calculated { 6.1, 7.1, 8.1, 9.1, 10.1, 11.1};

    double firstMetric = calculateMetric(times, reference, calculated);

    getTestedEntity()->Reset();     /// Resetting so we have the same starting conditions
    double secondMetric = calculateMetric(times, calculated, reference);

    double epsilon = 0.0001;
    /// Metric must have a symmetry trait - d(a,b) must be equal to d(b,a)
    if (std::abs(firstMetric - secondMetric) > epsilon) {
        Logger::getInstance().error(L"Metric does not meet symmetry requirement!");
        Logger::getInstance().error(L"First metric: " + std::to_wstring(firstMetric));
        Logger::getInstance().error(L"Second metric: " + std::to_wstring(secondMetric));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Metric meets symmetry requirement.");
    return S_OK;
}

HRESULT tester::MetricUnitTester::triangleInequalityTest() {
    std::vector<double> times { 0.0, 0.02, 0.04};
    std::vector<double> a { 0, 1, 2 };
    std::vector<double> b { 3, 4, 5 };
    std::vector<double> c { 6, 7, 8 };

    double abMetric = calculateMetric(times, a, b);
    getTestedEntity()->Reset();
    double bcMetric = calculateMetric(times, b, c);
    getTestedEntity()->Reset();
    double acMetric = calculateMetric(times, a, c);

    double epsilon = 0.0001;    /// Epsilon for comparing doubles
    if (acMetric > (abMetric + bcMetric) && (acMetric - (abMetric + bcMetric) > epsilon)) {
        Logger::getInstance().error(L"Metric does not meet triangle inequality requirement!");
        Logger::getInstance().error(L"Metric d(a, b): " + std::to_wstring(abMetric));
        Logger::getInstance().error(L"Metric d(b, c): " + std::to_wstring(bcMetric));
        Logger::getInstance().error(L"Metric d(a, c): " + std::to_wstring(acMetric));
        return E_FAIL;
    }

    Logger::getInstance().info(L"Metric meets triangle inequality requirement.");
    return S_OK;
}

void tester::MetricUnitTester::loadEntity() {
    if (!getEntityLib().Is_Loaded()) {
        std::wcerr << L"Metric library is not loaded! Metric will not be loaded.\n";
        Logger::getInstance().error(L"Metric library is not loaded! Metric will not be loaded.");
        return;
    }

    if (getTestedEntity()) {
        setTestedEntity(nullptr);
    }

    scgms::IMetric *testedMetric;
    scgms::TMetric_Parameters parameters { getEntityGuid(), false, false, false, 0.0};
    HRESULT creationResult = constructEntity<scgms::TCreate_Metric>(getEntityLib(), "do_create_metric",
                                                                    &parameters, &testedMetric);
    if (creationResult != S_OK) {
        Logger::getInstance().error(L"Error while loading metric from the dynamic library!");
    } else {
        setTestedEntity(testedMetric);
        Logger::getInstance().info(L"Metric loaded from dynamic library.");
    }
}

HRESULT tester::MetricUnitTester::shutDownTest() {
    setTestedEntity(nullptr);
    return S_OK;
}

double tester::MetricUnitTester::calculateMetric(const std::vector<double> &times, const std::vector<double> &reference,
                                                 const std::vector<double> &calculated) {
    HRESULT accumulateResult = getTestedEntity()->Accumulate(times.data(), reference.data(), reference.data(), times.size());
    if (!Succeeded(accumulateResult)) {
        Logger::getInstance().warn(L"Metric accumulation did not end correctly.");
        Logger::getInstance().warn(std::wstring(L"Accumulate result: ") + Describe_Error(accumulateResult));
    }

    double metric;
    HRESULT calculateResult = getTestedEntity()->Calculate(&metric, nullptr, 0);
    if (!Succeeded(calculateResult)) {
        Logger::getInstance().warn(L"Metric calculation did not end correctly.");
        Logger::getInstance().warn(std::wstring(L"Calculate result: ") + Describe_Error(calculateResult));
    }

    return metric;
}

tester::ApproximatorUnitTester::ApproximatorUnitTester(const GUID &guid, const std::wstring &libraryPath)
        : EntityUnitTester<scgms::IApproximator>(guid) {
    setEntityLib(libraryPath);
}

void tester::ApproximatorUnitTester::executeAllTests() {
    const wchar_t* entityName = getEntityName<scgms::TGet_Approx_Descriptors , scgms::TApprox_Descriptor>("do_get_approximator_descriptors");
    logs::printEntityTestsStartInfo(entityName, L"approximator");

    executeTest(L"middle time approximation test", std::bind(&ApproximatorUnitTester::middleTimeApproximationTest, this));
    executeTest(L"order 1 middle time derivative approximation test", std::bind(&ApproximatorUnitTester::middleTimeDerivativeApproximationTest, this));
}

HRESULT tester::ApproximatorUnitTester::middleTimeApproximationTest() {
    return calculateApproximations();
}

HRESULT tester::ApproximatorUnitTester::middleTimeDerivativeApproximationTest() {
    return calculateApproximations(1);
}

HRESULT tester::ApproximatorUnitTester::calculateApproximations(const std::size_t derivativeOrder) {
    std::array<double, s_signalValuesCount - 1> times {};
    std::array<double, s_signalValuesCount - 1> levels {};

    for (std::size_t i = 0; i < s_signalValuesCount - 1; ++i) {
        times[i] = (s_times[i] + s_times[i + 1]) / 2.0; /// We want to approximate the half-time between all given times
    }

    HRESULT retrievalResult = getTestedEntity()->GetLevels(times.data(), levels.data(), s_signalValuesCount - 1, derivativeOrder);
    if (!Succeeded(retrievalResult)) {
        Logger::getInstance().error(std::wstring(L"Error while approximating half-time signal ") +
                                    (derivativeOrder ? L"order " + std::to_wstring(derivativeOrder) + L" derivatives" : L"levels") + L")");
        Logger::getInstance().error(std::wstring(L"Actual result: ") + Describe_Error(retrievalResult));
        return E_FAIL;
    }

    HRESULT testResult = S_OK;
    for (std::size_t i = 0; i < s_signalValuesCount - 1; i++) {
        if (levels[i] <= 0) {           /// We are passing positive times and levels to the signal - we can assume that the approximation should always be positive
            Logger::getInstance().error(std::wstring(L"Error while approximating signal ") +
                                        (derivativeOrder ? L"order " + std::to_wstring(derivativeOrder) + L" derivative" : L"level")
                                        + L" in time: " + std::to_wstring(times[i]));
            Logger::getInstance().error(L"Calculated value: " + std::to_wstring(levels[i]));
            testResult = E_FAIL;
        }
    }

    if (testResult == S_OK) {
        Logger::getInstance().info(L"All approximations calculated correctly.");
    }

    return testResult;
}

void tester::ApproximatorUnitTester::loadEntity() {
    if (!getEntityLib().Is_Loaded()) {
        std::wcerr << L"Approximator library is not loaded! Approximator will not be loaded.\n";
        Logger::getInstance().error(L"Approximator library is not loaded! Approximator will not be loaded.");
        return;
    }

    if (getTestedEntity()) {
        setTestedEntity(nullptr);
    }

    CDynamic_Library signalLib;
    if (!signalLib.Load(std::wstring(cnst::SIGNAL_LIBRARY) + cnst::LIB_EXTENSION)) {
        Logger::getInstance().error(L"Error while loading signal library! Approximator will not be loaded...");
        return;
    }

    scgms::CTime_Segment timeSegment;
    scgms::ISignal *signal;
    HRESULT signalCreationResult = constructEntity<scgms::TCreate_Signal>(signalLib, "do_create_signal",
                                                                          &scgms::signal_BG, &timeSegment, nullptr, &signal);
    if (signalCreationResult != S_OK) {
        Logger::getInstance().error(L"Error while loading signal from dynamic library! Approximator will not be loaded...");
        return;
    }

    Logger::getInstance().debug(L"Signal loaded from dynamic library.");

    HRESULT updateResult = signal->Update_Levels(s_times.data(), s_levels.data(), s_signalValuesCount);
    if (!Succeeded(updateResult)) {
        Logger::getInstance().warn(L"Error occurred while updating the signal levels... Testing may not return valid results.");
    }

    scgms::IApproximator *testedApproximator;
    HRESULT creationResult = constructEntity<scgms::TCreate_Approximator>(getEntityLib(), "do_create_approximator",
                                                                    &getEntityGuid(), signal, &testedApproximator);
    if (creationResult != S_OK) {
        Logger::getInstance().error(L"Error while loading approximator from the dynamic library!");
    } else {
        setTestedEntity(testedApproximator);
        Logger::getInstance().info(L"Approximator loaded from dynamic library.");
    }
}

HRESULT tester::ApproximatorUnitTester::shutDownTest() {
    setTestedEntity(nullptr);
    return S_OK;
}
