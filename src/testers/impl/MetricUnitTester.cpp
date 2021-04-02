//
// Author: markovd@students.zcu.cz
//

#include "../MetricUnitTester.h"
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
        std::wcerr << L"Filter library is not loaded! Filter will not be loaded.\n";
        Logger::getInstance().error(L"Filter library is not loaded! Filter will not be loaded.");
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
        Logger::getInstance().error(L"Error while loading filter from the dynamic library!");
    } else {
        setTestedEntity(testedMetric);
        Logger::getInstance().info(L"Filter loaded from dynamic library.");
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
