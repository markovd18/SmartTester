#include <iostream>
#include <fstream>
#include "LogFilterUnitTester.h"
#include "../../smartcgms/src/common/rtl/FilterLib.h"

LogFilterUnitTester::LogFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid) : GenericUnitTester(library, testFilter, guid){
	//
}

/**
	Executes all tests specific to filter tested by this UnitTester.
*/
void LogFilterUnitTester::executeSpecificTests() {
	executeTest(L"empty log file name test", std::bind(&LogFilterUnitTester::emptyLogFileNameTest, this));
	executeTest(L"correct log file name test", std::bind(&LogFilterUnitTester::correctLogFileNameTest, this));
	executeTest(L"log file generation test", std::bind(&LogFilterUnitTester::logFileGenerationTest, this));
}

/**
	Tests if filter can be configured with empty file name.
*/
HRESULT LogFilterUnitTester::emptyLogFileNameTest()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter created! Cannot execute test.\n";
		exit(E_FAIL);
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{c0e942b9-3928-4b81-9b43-a347668200ba}]\n\nLog_File = ";

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	result = testedFilter->Configure(begin[0], errors.get());

	if (!SUCCEEDED(result)) {	// test shouldn't succeed, we don't want to configure output log without name
		return S_OK;
	}
	else {
		return E_FAIL;
	}
}

/**
	Tests correct configuration of the filter.
*/
HRESULT LogFilterUnitTester::correctLogFileNameTest()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter created! Cannot execute test.\n";
		exit(E_FAIL);
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{c0e942b9-3928-4b81-9b43-a347668200ba}]\n\nLog_File = log.log";

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	result = testedFilter->Configure(begin[0], errors.get());

	if (SUCCEEDED(result)) {
		return S_OK;
	}
	else {
		return E_FAIL;
	}
}

/**
	Tests if log file is created.
*/
HRESULT LogFilterUnitTester::logFileGenerationTest()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter created! Cannot execute test.\n";
		exit(E_FAIL);
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{c0e942b9-3928-4b81-9b43-a347668200ba}]\n\nLog_File = log.log";

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	result = testedFilter->Configure(begin[0], errors.get());

	if (SUCCEEDED(result)) {
		scgms::SFilter_Executor filterExecutor = { configuration.get(), nullptr, nullptr, errors };
		if (!filterExecutor)
		{
			std::wcerr << L"Could not execute configuration!\n";
			return E_FAIL;
		}
		else {
			filterExecutor->Terminate();
			std::ifstream file("../bin/log.log");
			if (file.good())
			{
				return S_OK;
			}
			else {
				return E_FAIL;
			}
		}
	}
	else {
		return E_FAIL;
	}
}
