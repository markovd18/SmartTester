//
// Author: markovd@students.zcu.cz
//

#include <iostream>
#include <rtl/guid.h>
#include <utils/string_utils.h>
#include <rtl/FilterLib.h>
#include "../UnitTestExecUtils.h"
#include "../../mappers/GuidTesterMapper.h"
#include "../../testers/ModelUnitTester.h"
#include "../../testers/SecondaryUnitTesters.h"

const wchar_t* MODEL_LIBS[] = { cnst::MODEL_LIBRARY };
const wchar_t* METRIC_LIBS[] = { cnst::METRIC_LIBRARY };
const wchar_t* APPROX_LIBS[] = { cnst::APPROX_LIBRARY };

void tester::executeFilterTests(const GUID& guid) {
    if (Is_Invalid_GUID(guid)) {
        std::wcerr << L"Invalid GUID passed!\n";
        Logger::getInstance().error(L"Invalid GUID passed as parameter!");
        return;
    }
	
	tester::FilterUnitTester* unitTester = getUnitTester(guid);
    if (unitTester == nullptr) {
        Logger::getInstance().error(L"Unit tester for GUID " + GUID_To_WString(guid) + L" not found! Skipping...");
        return;
    }

    unitTester->executeAllTests();
    delete unitTester;
}

void tester::executeModelTests(const wchar_t *lib) {
    CDynamic_Library modelLib;

    std::wstring libPath = std::wstring(lib) + cnst::LIB_EXTENSION;
    if (!modelLib.Load(libPath)) {
        Logger::getInstance().error(std::wstring(L"Error while loading ") + lib + L" library. Skipping it's model tests...");
        return;
    }

    scgms::TModel_Descriptor *begin, *end;
    HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Model_Descriptors>(modelLib, "do_get_model_descriptors", &begin, &end);
    if (!Succeeded(descriptorsResult)) {
        Logger::getInstance().error(std::wstring(L"Error while resolving model descriptors from ") + lib + L"! Cancelling tests...");
        return;
    }

    while (begin != end) {
        tester::ModelUnitTester modelTester(begin->id, libPath);
        modelTester.executeAllTests();
        begin++;
    }
}

void tester::executeMetricTests(const wchar_t *lib) {
    CDynamic_Library metricLib;

    std::wstring libPath = std::wstring(lib) + cnst::LIB_EXTENSION;
    if (!metricLib.Load(libPath)) {
        Logger::getInstance().error(std::wstring(L"Error while loading ") + lib + L" library. Skipping it's metric tests...");
        return;
    }

    scgms::TMetric_Descriptor *begin, *end;
    HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Metric_Descriptors>(metricLib, "do_get_metric_descriptors", &begin, &end);
    if (!Succeeded(descriptorsResult)) {
        Logger::getInstance().error(std::wstring(L"Error while resolving metric descriptors from ") + lib + L"! Cancelling tests...");
        return;
    }

    while (begin != end) {
        tester::MetricUnitTester metricTester(begin->id, libPath);
        metricTester.executeAllTests();
        begin++;
    }
}

void tester::executeApproximatorTests(const wchar_t *lib) {
    CDynamic_Library approxLib;

    std::wstring libPath = std::wstring(lib) + cnst::LIB_EXTENSION;
    if (!approxLib.Load(libPath)) {
        Logger::getInstance().error(std::wstring(L"Error while loading ") + lib + L" library. Skipping it's approximator tests...");
        return;
    }

    scgms::TApprox_Descriptor *begin, *end;
    HRESULT descriptorsResult = getEntityDescriptors<scgms::TGet_Approx_Descriptors>(approxLib, "do_get_approximator_descriptors", &begin, &end);
    if (!Succeeded(descriptorsResult)) {
        Logger::getInstance().error(std::wstring(L"Error while resolving approximator descriptors from ") + lib + L"! Cancelling tests...");
        return;
    }

    while (begin != end) {
        tester::ApproximatorUnitTester approxTester(begin->id, libPath);
        approxTester.executeAllTests();
        begin++;
    }
}

void tester::executeAllTests() {
	Logger::getInstance().info(L"Executing all tests across all entities.");
	std::map<GUID, const wchar_t*> map = GuidFileMapper::GetInstance().getMap();

    for (const auto &guidPair : map) {
        executeFilterTests(guidPair.first);
    }

    for (const auto &modelLib : MODEL_LIBS) {
        executeModelTests(modelLib);
    }

    for (const auto &metricLib : METRIC_LIBS) {
        executeMetricTests(metricLib);
    }

    for (const auto &approxLib : APPROX_LIBS) {
        executeApproximatorTests(approxLib);
    }

}

tester::FilterUnitTester* tester::getUnitTester(const GUID& guid) {
	return GuidTesterMapper::GetInstance().getTesterInstance(guid);
}

HRESULT tester::configureFilter(scgms::IFilter *filter, const tester::FilterConfig &config) {
    if (!filter) {
        std::wcerr << L"No filter passed! Can't configure filter.\n";
        Logger::getInstance().error(L"No filter passed! Can't configure filter.");
        return E_FAIL;
    }

    scgms::SPersistent_Filter_Chain_Configuration configuration;
    refcnt::Swstr_list errors;

    HRESULT result = configuration ? S_OK : E_FAIL;
    std::string memory = config.toString();
    if (result == S_OK) {
        configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
        Logger::getInstance().debug(L"Loading configuration from memory...");
    } else {
        Logger::getInstance().error(L"Error while creating configuration!");
        return E_FAIL;
    }

    scgms::IFilter_Configuration_Link** begin, ** end;
    configuration->get(&begin, &end);

    Logger::getInstance().info(L"Configuring filter...");
    return filter->Configure(begin[0], errors.get());

}

bool moveToTmp(const filesystem::path& filePath) {
    if (!filesystem::exists(filePath)) {
        return false;
    }

    std::string tmpDir = Narrow_WChar(cnst::TMP_DIR);
    filesystem::create_directory(tmpDir);
    if (!filesystem::exists(tmpDir)) {
        return false;
    }

    std::string dest = tmpDir + "/" +  filePath.filename().string();
    filesystem::rename(filePath, dest);
    return filesystem::exists(dest);
}