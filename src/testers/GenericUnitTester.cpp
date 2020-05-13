#include <iostream>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <functional>
#include <rtl/hresult.h>
#include <rtl/FilterLib.h>
#include "../mappers/GuidFileMapper.h"
#include "../utils/constants.h"
#include "GenericUnitTester.h"

/**
    Creates an instance of GenericUnitTesterClass.
    Pass initialized instance of CDynamic_Library, initialized instance of TestFilter
    and GUID of filter that you want to be tested. If default invalid GUID is passed, 
    all tests across all filters will be executed.
*/
GenericUnitTester::GenericUnitTester(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid) {
    this->testedFilter = nullptr;
    this->scgmsLibrary = nullptr;
    this->filterLibrary = library;
    this->testFilter = testFilter;
    this->tested_guid = guid;
    //CDynamic_Library::Set_Library_Base(LIB_DIR);
    loadFilter();
    if (isFilterLoaded())
    {
        loadScgmsLibrary();
    }
    
}

GenericUnitTester::~GenericUnitTester() {
    if (isFilterLoaded())
    {
        filterLibrary->Unload();
        delete filterLibrary;
        scgmsLibrary->Unload();
        delete scgmsLibrary;
    }
    
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

    const wchar_t* file_name = GuidFileMapper::GetInstance().getFileName(*(this->tested_guid));
    filterLibrary->Load(file_name);


    if (!filterLibrary->Is_Loaded()) {
        std::wcerr << L"Couldn't load " << file_name << " library!\n";
        logger.error(L"Couldn't load " + std::wstring(file_name) + L"library.");
        return;
    }

    auto creator = filterLibrary->Resolve<scgms::TCreate_Filter>("do_create_filter");

    scgms::IFilter* created_filter = nullptr;
    auto result = creator(tested_guid, testFilter, &created_filter);
    if (result != S_OK) {
        std::wcerr << L"Failed creating filter!\n";
        logger.error(L"Failed creating filter!");
        return;
    }

    logger.info(L"Filter loaded from dynamic library.");
    this->testedFilter = created_filter;
}

/**
    Loads dynamic library "scgms" which will be needed for testing.
    If loading fails, exits the program.
*/
void GenericUnitTester::loadScgmsLibrary() {
    logger.debug(L"Loading dynamic library scgms...");
    CDynamic_Library *scgms = new CDynamic_Library;

    scgms->Load(SCGMS_LIB);
    if (!scgms->Is_Loaded())
    {
        std::wcerr << L"Couldn't load scgms library!\n";
        logger.error(L"Couldn't load scgms library!");
        exit(E_FAIL);
    }

    logger.debug(L"Dynamic library scgms loaded.");
    scgmsLibrary = scgms;
}

/**
    Returns name of the tested filter.
*/
const wchar_t* GenericUnitTester::getFilterName()
{
    if (!filterLibrary->Is_Loaded())
    {
        return nullptr;
    }

    auto creator = filterLibrary->Resolve<scgms::TGet_Filter_Descriptors>("do_get_filter_descriptors");
    scgms::TFilter_Descriptor* begin, * end;
    creator(&begin, &end);
    for (int i = 0; i < (end - begin); i++)
    {
        if (begin->id == *tested_guid)
        {
            return begin->description;
        }
        begin++;
    }
    return nullptr;
}

/**
    Executes all generic and specific tests for given filter.
*/
void GenericUnitTester::executeAllTests() {
    const wchar_t* filter_name = getFilterName();

    std::wcout << "****************************************\n"
        << "Testing " << filter_name << " filter:\n"
        << "****************************************\n";
    logger.debug(L"****************************************");
    logger.debug(L"Testing " + std::wstring(filter_name) + L" filter:");
    logger.debug(L"****************************************");

    executeGenericTests();
    executeSpecificTests();
}

/**
    Executes only tests which can be applied on every filter.
*/
void GenericUnitTester::executeGenericTests() {
    
    logger.info(L"Executing generic tests...");
    executeTest(L"info event test", std::bind(&GenericUnitTester::infoEventTest, this));

}

/**
    Executes test method passed as a parameter.
*/
void GenericUnitTester::executeTest(std::wstring testName, std::function<HRESULT(void)> test) {
    logger.info(L"----------------------------------------");
    logger.info(L"Executing " + testName + L"...");
    logger.info(L"----------------------------------------");
    std::wcout << "Executing " << testName << "... ";
    HRESULT result = runTestInThread(test);
    printResult(result);
}

/**
    Creates Shut_Down event and executes it.
*/
HRESULT GenericUnitTester::shutDownTest()
{
    scgms::IDevice_Event* shutDown;

    auto creator = scgmsLibrary->Resolve<scgms::TCreate_Device_Event>("create_device_event");
    auto result = creator(scgms::NDevice_Event_Code::Shut_Down, &shutDown);
    return testedFilter->Execute(shutDown);
}

/**
    Runs test method passed as a parameter in a separate thread.
*/
HRESULT GenericUnitTester::runTestInThread(std::function<HRESULT(void)> test) {
    logger.debug(L"Running test in thread...");
    std::cv_status status;
    HRESULT result = S_FALSE;

    {
        std::unique_lock<std::mutex> lock(testMutex);

        std::thread thread(&GenericUnitTester::runTest, this, test);

        status = testCv.wait_for(lock, std::chrono::milliseconds(MAX_EXEC_TIME));
        lock.unlock();

        if (status == std::cv_status::timeout) {
            shutDownTest();
        }

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
        std::wcerr << L"No filter created! Can't execute test.\n";
        logger.error(L"No filter created! Can't execute test...");
        return E_FAIL;
    }
    scgms::IDevice_Event* event;
    
    auto creator = scgmsLibrary->Resolve<scgms::TCreate_Device_Event>("create_device_event");
    auto result = creator(scgms::NDevice_Event_Code::Information, &event);
    if (FAILED(result))
    {
        std::wcerr << L"Error while creating \"Information\" IDevice_event!\n";
        logger.error(L"Error while creating \"Information\" IDevice_event!");
        shutDownTest();
        return E_FAIL;
    }

    logger.info(L"Executing event...");
    result = testedFilter->Execute(event);

    if (SUCCEEDED(result)) {
        scgms::TDevice_Event* recievedEvent = testFilter->getRecievedEvent();
        if (recievedEvent->event_code == scgms::NDevice_Event_Code::Information)
        {
            result = S_OK;
        }
        else {
            logger.error(L"Event was modified during execution!");
            result = E_FAIL;
        }
    }
    else {
        logger.error(L"Event wasn't correctly executed!");
        result = E_FAIL;
    }

    shutDownTest();
    return result;
}