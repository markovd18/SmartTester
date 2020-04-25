#include <iostream>
#include <chrono>
#include <thread>
#include <condition_variable>
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

///     **************************************************
///                     Helper methods
///     **************************************************

/**
    Loads filter from dynamic library based on given GUID in constructor.
    If loading fails, exits the program.
*/
void GenericUnitTester::loadFilter() {
    GuidFileMapper mapper = GuidFileMapper();

    library->Load(mapper.getFileName(*(this->tested_guid)));

    if (!library->Is_Loaded()) {
        std::wcerr << L"Couldn't load library!\n";
        exit(E_FAIL);
    }

    auto creator = library->Resolve<scgms::TCreate_Filter>("do_create_filter");

    scgms::IFilter* created_filter = nullptr;
    auto result = creator(tested_guid, testFilter, &created_filter);
    if (result != S_OK) {
        std::wcerr << L"Failed creating filter!\n";
        exit(E_FAIL);
    }
    this->testedFilter = created_filter;
}

/**
    Executes all generic and specific tests for given filter.
*/
void GenericUnitTester::executeAllTests() {
    executeGenericTests();
    executeSpecificTests();
}

/**
    Executes only tests which can be applied on every filter.
*/
void GenericUnitTester::executeGenericTests() {
    executeTest(infoEventTest);
    //FIXME!! nelze pøedávat ukazatel na èlenskou funkci jako ukazatel na klasickou funkci
}

/**
    Executes test method passed as a parameter.
*/
void GenericUnitTester::executeTest(HRESULT(* test)()) {
    
    HRESULT result = runTestInThread(test);
    printResult(result);
}

/**
    Runs test method passed as a parameter in a separate thread.
*/
HRESULT GenericUnitTester::runTestInThread(HRESULT(*test)()) {

    std::cv_status status;
    HRESULT result = S_FALSE;

    {
        std::unique_lock<std::mutex> lock(testMutex);

        std::thread thread(&runTest, test);

        status = testCv.wait_for(lock, std::chrono::milliseconds(MAX_EXEC_TIME));
        lock.unlock();

        // poslat ShutDown

        if (thread.joinable())
        {
            thread.join();
        }
    }

    if (status == std::cv_status::timeout) {
        result = E_TIMEOUT;
    }
    else {
        result = lastTestResult ? S_OK : S_FALSE;
    }

    return result;
}

void GenericUnitTester::runTest(HRESULT(*test)()) {

    lastTestResult = test();
    testCv.notify_all();
}

/**
    Checks if any filter was successfully loaded.
*/
bool GenericUnitTester::isFilterLoaded() {
    return testedFilter != nullptr;
}


//      **************************************************
//                      Generic tests
//      **************************************************

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