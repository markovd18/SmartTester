#include <iostream>
#include "DrawingFilterUnitTester.h"
#include "../../smartcgms/src/common/rtl/FilterLib.h"

DrawingFilterUnitTester::DrawingFilterUnitTester(CDynamic_Library* library, TestFilter* testFilter, const GUID* guid) : GenericUnitTester(library, testFilter, guid) {
	//
}

/**
	Executes all tests specific to filter tested by this UnitTester.
*/
void DrawingFilterUnitTester::executeSpecificTests() {
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
		std::wcerr << L"No filter created! Cannot execute test.\n";
		exit(E_FAIL);
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{850a122c-8943-a211-c514-25baa9143574}]\n\nCanvas_Width = \n\nCanvas_Height = ";

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	result = testedFilter->Configure(begin[0], errors.get());

	if (!SUCCEEDED(result)) {	// test shouldn't succeed, we don't want to configure canvas without size
		return S_OK;
	}
	else {
		return E_FAIL;
	}
}

/**
	Tests if filter can be configured with correct canvas size.
*/
HRESULT DrawingFilterUnitTester::correctCanvasSizeTest()
{
	if (!isFilterLoaded())
	{
		std::wcerr << L"No filter created! Cannot execute test.\n";
		exit(E_FAIL);
	}

	scgms::SPersistent_Filter_Chain_Configuration configuration;
	refcnt::Swstr_list errors;
	std::string memory = "[Filter_001_{850a122c-8943-a211-c514-25baa9143574}]\n\nCanvas_Width = 1200\n\nCanvas_Height = 800";

	HRESULT result = configuration ? S_OK : E_FAIL;
	if (result == S_OK)
	{
		configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
	}

	scgms::IFilter_Configuration_Link** begin, ** end;
	configuration->get(&begin, &end);

	result = testedFilter->Configure(begin[0], errors.get());

	if (SUCCEEDED(result)) {	// test shouldn't succeed, we don't want to configure canvas without size
		return S_OK;
	}
	else {
		return E_FAIL;
	}
}