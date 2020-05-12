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
GenericUnitTester::GenericUnitTester(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid) {
    this->filterLibrary = library;
    this->testFilter = testFilter;
    this->tested_guid = guid;
    //CDynamic_Library::Set_Library_Base(LIB_DIR);
    loadFilter();
    loadScgmsLibrary();
}

GenericUnitTester::~GenericUnitTester() {
    filterLibrary->Unload();
    delete filterLibrary;
    scgmsLibrary->Unload();
    delete scgmsLibrary;
    delete testFilter;
}

///     **************************************************
///                     Helper methods
///     **************************************************

/**
    Loads filter from dynamic library based on given GUID in constructor.
    If loading fails, exits the program.
*/
void GenericUnitTester::loadFilter() {
    filterLibrary->Load(GuidFileMapper::GetInstance().getFileName(*(this->tested_guid)));
    logger.info(L"Loading " + std::wstring(GuidFileMapper::GetInstance().getFileName(*(this->tested_guid))) + L" filter...");


    if (!filterLibrary->Is_Loaded()) {
        std::wcerr << L"Couldn't load filter library!\n";
        logger.error(L"Couldn't load filter library!");
        exit(E_FAIL);
    }

    auto creator = filterLibrary->Resolve<scgms::TCreate_Filter>("do_create_filter");

    scgms::IFilter* created_filter = nullptr;
    auto result = creator(tested_guid, testFilter, &created_filter);
    if (result != S_OK) {
        std::wcerr << L"Failed creating filter!\n";
        logger.error(L"Failed creating filter!");
        exit(E_FAIL);
    }

    logger.info(L"Filter loaded from dynamic library...");
    this->testedFilter = created_filter;
}

/**
    Loads dynamic library "scgms" which will be needed for testing.
    If loading fails, exits the program.
*/
void GenericUnitTester::loadScgmsLibrary() {
    logger.info(L"Loading dynamic library scgms...");
    CDynamic_Library *scgms = new CDynamic_Library;

    scgms->Load(SCGMS_LIB);
    if (!scgms->Is_Loaded())
    {
        std::wcerr << L"Couldn't load scgms library!\n";
        logger.error(L"Couldn't load scgms library!");
        exit(E_FAIL);
    }

    logger.info(L"Dynamic library scgms loaded...");
    scgmsLibrary = scgms;
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
    logger.info(L"Executing generic tests...");
    std::wcout << "Testing " << GuidFileMapper::GetInstance().getFileName(*tested_guid) << " filter:\n";
    logger.info(L"Testing " + std::wstring(GuidFileMapper::GetInstance().getFileName(*tested_guid)) + L" filter...");
    executeTest(L"info event test", std::bind(&GenericUnitTester::infoEventTest, this));

}

/**
    Executes test method passed as a parameter.
*/
void GenericUnitTester::executeTest(std::wstring testName, std::function<HRESULT(void)> test) {
    logger.info(L"Executing " + testName + L"...");
    std::wcout << "Executing " << testName << "... ";
    HRESULT result = runTestInThread(test);
    printResult(result);
}

/**
    Runs test method passed as a parameter in a separate thread.
*/
HRESULT GenericUnitTester::runTestInThread(std::function<HRESULT(void)> test) {
    logger.info(L"Running test in thread...");
    std::cv_status status;
    HRESULT result = S_FALSE;

    {
        std::unique_lock<std::mutex> lock(testMutex);

        std::thread thread(&GenericUnitTester::runTest, this, test);

        status = testCv.wait_for(lock, std::chrono::milliseconds(MAX_EXEC_TIME));
        lock.unlock();

        // poslat ShutDown
        scgms::IDevice_Event* shutDown;

        auto creator = scgmsLibrary->Resolve<scgms::TCreate_Device_Event>("create_device_event");
        auto result = creator(scgms::NDevice_Event_Code::Shut_Down, &shutDown);
        testedFilter->Execute(shutDown);

        if (thread.joinable())
        {
            thread.join();
        }
    }

    if (status == std::cv_status::timeout) {
        std::wcerr << L"TIMEOUT ";
        logger.error(L"Test in thread timed out!");
        result = E_FAIL;
    }
    else {
        result = lastTestResult;
    }

    return result;
}

/**
    Runs passed test and notifies all other threads.
*/
void GenericUnitTester::runTest(std::function<HRESULT(void)> test) {
    logger.info(L"Running passed test from parameter and notifying all other threads...");
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
        logger.info(L"Test result: OK!");
        break;
    case S_FALSE:
        std::wcout << "FAIL!\n";
        logger.error(L"Test result: FAIL!");
        break;
    case E_FAIL:
        std::wcout << "ERROR!\n";
        logger.error(L"Test result: ERROR!");
        break;
    default:
        std::wcerr << "UNKNOWN!\n";
        logger.info(L"Test result: UNKNOWN!");
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
        std::wcerr << L"No filter created! Cannot execute test.\n";
        logger.error(L"No filter created! Cannot execute test...");
        exit(E_FAIL);
    }
    scgms::IDevice_Event* event;
    
    auto creator = scgmsLibrary->Resolve<scgms::TCreate_Device_Event>("create_device_event");
    auto result = creator(scgms::NDevice_Event_Code::Information, &event);
    if (FAILED(result))
    {
        std::wcerr << L"Error while creating \"Information\" IDevice_event!\n";
        logger.error(L"Error while creating \"Information\" IDevice_event!");
        return E_FAIL;
    }

    result = testedFilter->Execute(event);

    if (SUCCEEDED(result)) {
        scgms::TDevice_Event* recievedEvent = testFilter->getRecievedEvent();
        if (recievedEvent->event_code == scgms::NDevice_Event_Code::Information)
        {
            result = creator(scgms::NDevice_Event_Code::Shut_Down, &event);
            if (FAILED(result))
            {
                std::wcerr << L"Error while creating \"Shut_Down\" IDevice_event!\n";
                logger.error(L"Error while creating \"Shut_Down\" IDevice_event!");
                return E_FAIL;
            }
            testedFilter->Execute(event);
            return S_OK;
        }
        else {
            return E_FAIL;
        }
    }
    else {
        return E_FAIL;
    }
}