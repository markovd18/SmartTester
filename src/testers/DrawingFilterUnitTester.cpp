#include <iostream>
#include <rtl/FilterLib.h>
#include <utils/string_utils.h>
#include "DrawingFilterUnitTester.h"

DrawingFilterUnitTester::DrawingFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter, const GUID* guid) : GenericUnitTester(library, testFilter, guid) {
	//
}

/**
	Executes all tests specific to filter tested by this UnitTester.
*/
void DrawingFilterUnitTester::executeSpecificTests() {
	logger.info(L"Executing specific tests...");
	executeTest(L"empty canvas size test", std::bind(&DrawingFilterUnitTester::emptyCanvasSizeTest, this));
	executeTest(L"correct canvas size test", std::bind(&DrawingFilterUnitTester::correctCanvasSizeTest, this));
}

/**
	Tests if filter can be configured with empty canvas size.
*/
HRESULT DrawingFilterUnitTester::emptyCanvasSizeTest()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter loaded! Can't execute test.\n";
		logger.error(L"No filter loaded! Can't execute test.");
		return E_FAIL;
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{850a122c-8943-a211-c514-25baa9143574}]\n\nCanvas_Width = \n\nCanvas_Height = ";

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
		logger.info(L"Loading configuration from memory...");
	}
	else {
		logger.error(L"Error while creating configuration!");
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	result = testedFilter->Configure(begin[0], errors.get());
	logger.info(L"Configuring filter...");

	if (!SUCCEEDED(result)) {	// test shouldn't succeed, we don't want to configure canvas without size
		result = S_OK;
	}
	else {
		logger.error(L"Filter was configured successfully, but shouldn't be!\n"
		L"(" + std::wstring(memory.begin(), memory.end()) + L")");
		logger.error(L"expected result: " + Widen_Char(std::system_category().message(E_FAIL).c_str()));
		logger.error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
		result = E_FAIL;
	}

	shutDownTest();
	return result;
}

/**
	Tests if filter can be configured with correct canvas size.
*/
HRESULT DrawingFilterUnitTester::correctCanvasSizeTest()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter created! Cannot execute test.\n";
		logger.error(L"No filter loaded! Can't execute test.");
		exit(E_FAIL);
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{850a122c-8943-a211-c514-25baa9143574}]\n\nCanvas_Width = 1200\n\nCanvas_Height = 800";

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
		logger.info(L"Loading configuration from memory...");
	}
	else {
		logger.error(L"Error while creating configuration!");
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	result = testedFilter->Configure(begin[0], errors.get());
	logger.info(L"Configuring filter...");

	if (SUCCEEDED(result)) {	
		result = S_OK;
	}
	else {
		logger.error(L"Failed to configure filter!");
		logger.error(L"expected result: " + Widen_Char(std::system_category().message(S_OK).c_str()));
		logger.error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
		result = E_FAIL;
	}

	shutDownTest();
	return result;
}