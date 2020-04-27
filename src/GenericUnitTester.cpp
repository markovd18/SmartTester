#include <iostream>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <functional>
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
    this->loadScgmsLibrary();
    this->loadFilter();
}

///     **************************************************
///                     Helper methods
///     **************************************************

/**
    Loads scgms dynamic library required for sending events into filters.
*/
void GenericUnitTester::loadScgmsLibrary() {
    CDynamic_Library::Set_Library_Base(LIB_DIR);
    this->scgmsLib = &CDynamic_Library();

    this->scgmsLib->Load(L"../scgms");    //TODO pokud nebude fungovat, dodìlat platformní pøípony
    if (!scgmsLib->Is_Loaded())
    {
        std::wcerr << L"Error while loading SCGMS dynamic library!\n";
        exit(E_FAIL);
    }
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
    executeTest(std::bind(&GenericUnitTester::infoEventTest, this));

}

/**
    Executes test method passed as a parameter.
*/
void GenericUnitTester::executeTest(std::function<HRESULT(void)> test) {
    
    HRESULT result = runTestInThread(test);
    printResult(result);
}

/**
    Runs test method passed as a parameter in a separate thread.
*/
HRESULT GenericUnitTester::runTestInThread(std::function<HRESULT(void)> test) {

    std::cv_status status;
    HRESULT result = S_FALSE;

    {
        std::unique_lock<std::mutex> lock(testMutex);

        std::thread thread(&GenericUnitTester::runTest, this, test);

        status = testCv.wait_for(lock, std::chrono::milliseconds(MAX_EXEC_TIME));
        lock.unlock();

        // poslat ShutDown
        scgms::IDevice_Event* shutDown;
        auto creator = scgmsLib->Resolve<scgms::TCreate_Device_Event>("create_device_event");
        auto result = creator(scgms::NDevice_Event_Code::Shut_Down, &shutDown);
        testedFilter->Execute(shutDown);

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

/**
    Runs passed test and notifies all other threads.
*/
void GenericUnitTester::runTest(std::function<HRESULT(void)> test) {

    lastTestResult = test();
    testCv.notify_all();
}

/**
    Checks if any filter was successfully loaded.
*/
bool GenericUnitTester::isFilterLoaded() {
    return testedFilter != nullptr;
}

/**
    Prints information based on given HRESULT.
*/
void GenericUnitTester::printResult(HRESULT result) {
    switch (result)
    {
    case S_OK:
        std::wcout << "OK!\n";
        break;
    case S_FALSE:
        std::wcout << "FAIL!\n";
        break;
    case E_FAIL:
        std::wcout << "ERROR!\n";
        break;
    case E_TIMEOUT:
        std::wcout << "TIMEOUT!\n";
        break;
    default:
        std::wcerr << "UNKNOWN!\n";
        break;
    }
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

    scgms::IDevice_Event* event;
    auto creator = this->scgmsLib->Resolve<scgms::TCreate_Device_Event>("create_device_event");
    auto result = creator(scgms::NDevice_Event_Code::Information, &event);
    if (FAILED(result))
    {
        std::wcerr << L"Error while creating \"Information\" IDevice_event!\n";
        return E_FAIL;
    }

    result = testedFilter->Execute(event);

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