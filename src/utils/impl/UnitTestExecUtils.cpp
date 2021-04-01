//
// Author: markovd@students.zcu.cz
//

#include <iostream>
#include <rtl/guid.h>
#include <utils/string_utils.h>
#include <rtl/FilterLib.h>
#include "../UnitTestExecUtils.h"
#include "../../mappers/GuidTesterMapper.h"
#include "../../mappers/GuidFileMapper.h"
#include "../constants.h"
#include "../../testers/ModelUnitTester.h"

const wchar_t* MODEL_LIBS[] = { cnst::MODEL_LIBRARY };

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
        Logger::getInstance().info(std::wstring(L"Error while loading ") + lib + L" library. Skipping it's model tests...");
        return;
    }

    auto descriptorsCreator = modelLib.Resolve<scgms::TGet_Model_Descriptors>("do_get_model_descriptors");
    if (!descriptorsCreator) {
        Logger::getInstance().error(std::wstring(L"Error while resolving model descriptors from ") + lib + L"! Cancelling tests...");
        return;
    }

    scgms::TModel_Descriptor *begin, *end;
    descriptorsCreator(&begin, &end);

    while (begin != end) {
        tester::ModelUnitTester modelTester(begin->id, libPath);
        ((tester::FilterUnitTester*) &modelTester)->executeAllTests();
        begin++;
    }
}

void tester::executeAllTests() {
	Logger::getInstance().info(L"Executing all tests across all entities.");
	std::map<GUID, const wchar_t*> map = GuidFileMapper::GetInstance().getMap();

    for (const auto &guidPair : map) {
//        executeFilterTests(guidPair.first);
//TODO odkomentovat
    }

    for (const auto &modelLib : MODEL_LIBS) {
        executeModelTests(modelLib);
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