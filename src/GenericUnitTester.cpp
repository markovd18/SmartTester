#include <iostream>
#include <chrono>
#include "../../smartcgms/src/common/rtl/hresult.h"
#include "GuidFileMapper.h"
#include "GenericUnitTester.h"
#include "constants.h"

/**
    Creates an instance of GenericUnitTesterClass.
    Pass initialized instance of CDynamic_Library, initialized instance of TestFilter
    and GUID of filter that you want to be tested. If default invalid GUID is passed, 
    all tests across all filters will be executed.
*/
GenericUnitTester::GenericUnitTester(CDynamic_Library* library, TestFilter* testFilter, GUID* guid) {
    this->library = library;
    this->testFilter = testFilter;
    this->tested_guid = guid;
    this->loadFilter();
}

/**
    Loads filter from dynamic library based on given GUID in constructor.
    If loading fails, exits the program.
*/
void GenericUnitTester::loadFilter() {
    GuidFileMapper mapper = GuidFileMapper();

    library->Load(mapper.getFileName(*(this->tested_guid)));

    if (!library->Is_Loaded()) {
        std::wcerr << L"Couldn't load library!\n";
        exit(S_FALSE);
    }

    auto creator = library->Resolve<scgms::TCreate_Filter>("do_create_filter");

    scgms::IFilter* created_filter = nullptr;
    auto result = creator(tested_guid, testFilter, &created_filter);
    if (result != S_OK) {
        std::wcerr << L"Failed creating filter!\n";
        exit(S_FALSE);
    }
    this->testedFilter = created_filter;
}

/**
    Checks if any filter was successfully loaded.
*/
bool GenericUnitTester::isFilterLoaded() {
    return testedFilter != nullptr;
}

/**
    If any filter is created, executes an info event upon it. Tested filter should send the info event to
    the output filter, which will be TestFilter. If the event is not recieved by TestFilter, test ends with an error.
*/
HRESULT GenericUnitTester::infoEventTest() {
    if (!isFilterLoaded())
    {
        std::wcerr << L"No filter created! Cannot execute tests.\n";
        exit(E_FAIL);
    }

    library->Unload();
    library->Load(L"../scgms");

    scgms::IDevice_Event* event;
    auto creator = library->Resolve<scgms::TCreate_Device_Event>("create_device_event");
    auto result = creator(scgms::NDevice_Event_Code::Information, &event);
    if (FAILED(result))
    {
        std::wcerr << L"Error while creating \"Information\" IDevice_event!\n";
        return E_FAIL;
    }

    auto start = std::chrono::high_resolution_clock::now();

    result = testedFilter->Execute(event);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (duration > MAX_EXEC_TIME)
    {
        std::wcerr << L"ERROR! Test took longer than maximum allowed execution time! (" << MAX_EXEC_TIME << "ms)\n";
        return E_FAIL;
    }
    if (SUCCEEDED(result)) {
        std::wcerr << L"Info-event test passed.\n";
        event->Release();
        result = creator(scgms::NDevice_Event_Code::Shut_Down, &event);
        testedFilter->Execute(event);
        return S_OK;
    }
    else {
        std::wcerr << L"Info-event test didn't pass.\n";
        return E_FAIL;
    }
}