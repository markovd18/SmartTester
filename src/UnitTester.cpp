//
// Created by David on 11.04.2020.
//
#include "UnitTester.h"
#include "../../smartcgms/src/common/iface/DeviceIface.h"
#include "../../smartcgms/src/common/rtl/DeviceLib.h"
#include "../../smartcgms/src/common/rtl/referencedImpl.h"
#include "../../smartcgms/src/common/rtl/hresult.h"
#include "constants.h"
#include <chrono>
#include <iostream>

/**
    Creates an instance of UnitTester class. This constructor is intendet do be used, when you want to perform 
    all sets of tests across all filters.
*/
UnitTester::UnitTester(CDynamic_Library* library, TestFilter* testFilter) {
    this->library = library;
    this->testFilter = testFilter;
    //TODO dopsat spuštìní všech testù, pokud nezadán guid
}

/**
    Creates an instance of UnitTester class. This constructor is intended to use, when you want to test specific
    filter, based on passed GUID.
*/
UnitTester::UnitTester(CDynamic_Library* library, TestFilter* testFilter, GUID* guid) {
    this->library = library;
    this->testFilter = testFilter;
    this->tested_guid = guid;
    this->loadFilter();
}

void UnitTester::loadFilter() {
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
    If any filter is created, executes an info event upon it. Tested filter should send the info event to 
    the output filter, which will be TestFilter. If the event is not recieved by TestFilter, test ends with an error.
*/
void UnitTester::infoEventTest() {
    if (!isFilterCreated())
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
        exit(E_FAIL);
    }

    auto start = std::chrono::high_resolution_clock::now();

    result = testedFilter->Execute(event);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (duration > MAX_EXEC_TIME)
    {
        std::wcerr << L"ERROR! Test took longer than maximum allowed execution time! (" << MAX_EXEC_TIME << "ms)\n";
        exit(E_ABORT);
    }
    if (SUCCEEDED(result)) {
        std::wcerr << L"Info-event test passed.\n";
    }
    else {
        std::wcerr << L"Info-event test didn't pass.\n";
    }
}


bool UnitTester::isFilterCreated() {
    return testedFilter != nullptr;
}
