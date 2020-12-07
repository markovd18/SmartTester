#include <iostream>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <functional>
#include <string>
#include <rtl/hresult.h>
#include <rtl/FilterLib.h>
#include <utils/string_utils.h>
#include "../../mappers/GuidFileMapper.h"
#include "../../utils/constants.h"
#include "../GenericUnitTester.h"

/**
 *     Creates an instance of GenericUnitTesterClass.
    Pass initialized instance of CDynamic_Library, initialized instance of TestFilter
    and GUID of filter that you want to be tested. If default invalid GUID is passed,
    all tests across all filters will be executed.

 * @param library pointer to CDynamic_Library helper class
 * @param testFilter pointer to TestFilter helper class
 * @param guid guid of tested filter
 */
GenericUnitTester::GenericUnitTester(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid) {
    this->testedFilter = nullptr;
    this->scgmsLibrary = nullptr;
    this->filterLibrary = library;
    this->testFilter = testFilter;
    this->tested_guid = guid;
    this->lastTestResult = S_OK;
    loadFilter();
    if (isFilterLoaded())
    {
        loadScgmsLibrary();
    }
    
}

/**
 * Virtual destructor.
 * Returns all allocated memory and loaded libraries by this instance.
 */
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

//     **************************************************
//                     Helper methods
//     **************************************************

/**
    Loads filter from dynamic library based on given GUID in constructor.
    If loading fails, exits the program.
*/
void GenericUnitTester::loadFilter() {

    const wchar_t* file_name = GuidFileMapper::GetInstance().getFileName(*(this->tested_guid));
    std::wstring file = file_name;
    file.append(LIB_EXTENSION);

    filterLibrary->Load(file.c_str());


    if (!filterLibrary->Is_Loaded()) {
        std::wcerr << L"Couldn't load " << file_name << " library!\n";
        logger.error(L"Couldn't load " + std::wstring(file_name) + L" library.");
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
    auto *scgms = new CDynamic_Library;

    std::wstring file = SCGMS_LIB;
    file.append(LIB_EXTENSION);

    scgms->Load(file.c_str());
    if (!scgms->Is_Loaded())
    {
        std::wcerr << L"Couldn't load scgms library!\n";
        logger.error(L"Couldn't load scgms library!");
        exit(EXIT_FAILURE);
    }

    logger.debug(L"Dynamic library scgms loaded.");
    scgmsLibrary = scgms;
}

/**
 * Returns the name of tested filter.
 *
 * @return name of tested filter
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
    Invokes test method passed as a parameter. Invoked method has to take in zero parameters and return HRESULT as a return value.
    @param testName name of the test which will be displayed in logs
    @param test method to be invoked by this method
*/
void GenericUnitTester::executeTest(const std::wstring& testName, const std::function<HRESULT(void)>& test) {
    logger.info(L"----------------------------------------");
    logger.info(L"Executing " + testName + L"...");
    logger.info(L"----------------------------------------");
    std::wcout << "Executing " << testName << "... ";
    HRESULT result = runTestInThread(test);
    printResult(result);
}

/**
 * Invokes test method passed as a parameter. Invoked method has to take in two parameters and return HRESULT as a return value.
 * The first parameter of invoked method is string and second one is HRESULT. Main purpose of this method is to invoke
 * configuration test method which takes in configuration string as first parameter and expected result as second parameter.
 * @param testName name of the test which will be displayed in logs
 * @param test method to be invoked by this method
 */
void GenericUnitTester::executeConfigTest(const std::wstring& testName, const std::string& configuration, const HRESULT expectedResult) {
    logger.info(L"----------------------------------------");
    logger.info(L"Executing " + testName + L"...");
    logger.info(L"----------------------------------------");
    std::wcout << "Executing " << testName << "... ";
    HRESULT result = runConfigTestInThread(configuration, expectedResult);
    printResult(result);
}

/**
    Creates Shut_Down event and executes it.
*/
HRESULT GenericUnitTester::shutDownTest()
{
    scgms::IDevice_Event* shutDown;

    auto creator = scgmsLibrary->Resolve<scgms::TCreate_Device_Event>("create_device_event");
    HRESULT result = creator(scgms::NDevice_Event_Code::Shut_Down, &shutDown);
    if (!Succeeded(result))
    {
        std::wcerr << L"Error while creating \"Shut_Down\" IDevice_event!\n";
        logger.error(L"Error while creating \"Shut_Down\" IDevice_event!");
        return E_FAIL;
    }

    return testedFilter->Execute(shutDown);
}

/**
    Runs test method passed as a parameter in a separate thread.
    Passed method has to take in zero parameters and return HRESULT as a return value.
    @param test method to be run in a separate thread
    @return result of invoked test method
*/
HRESULT GenericUnitTester::runTestInThread(const std::function<HRESULT(void)>& test) {
    logger.debug(L"Running test in thread...");
    std::cv_status status;
    HRESULT result;

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
 * Runs test method passed as a parameter in a separate thread.
   Passed method has to take in two parameters and return HRESULT as a return value.
   The first parameter of invoked test method has to be std::string and the second one HRESULT. The main purpose of this
   method is to run configuration test methods in a separate thread which take in configuration string as the first parameter
   and expected result as the second one.
   @param test method to be run in a separate thread
 * @return result of invoked test method
 */
HRESULT GenericUnitTester::runConfigTestInThread(const std::string& configuration, const HRESULT expectedResult) {
    logger.debug(L"Running test in thread...");
    std::cv_status status;
    HRESULT result;

    {
        std::unique_lock<std::mutex> lock(testMutex);

        std::thread thread(&GenericUnitTester::runConfigTest, this, configuration, expectedResult);

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
void GenericUnitTester::runTest(const std::function<HRESULT(void)>& test) {
    lastTestResult = test();
    testCv.notify_all();
}

/**
 * Runs passed configuration test and notifies all other threads.
 * @param test configuration test to be run
 */
void GenericUnitTester::runConfigTest(const std::string& configuration, HRESULT expectedResult) {
    lastTestResult = configurationTest(configuration, expectedResult);
    testCv.notify_all();
}

/**
    Checks if any filter was successfully loaded.
*/
bool GenericUnitTester::isFilterLoaded() {
    return testedFilter != nullptr;
}

/**
 * Prints information based on given HRESULT.
 *
 * @param result test result to be printed out
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
    if (!Succeeded(result))
    {
        std::wcerr << L"Error while creating \"Information\" IDevice_event!\n";
        logger.error(L"Error while creating \"Information\" IDevice_event!");
        shutDownTest();
        return E_FAIL;
    }

    logger.info(L"Executing event...");
    result = testedFilter->Execute(event);

    if (Succeeded(result)) {
        scgms::TDevice_Event* recievedEvent = testFilter->getRecievedEvent();
        if (recievedEvent->event_code == scgms::NDevice_Event_Code::Information)
        {
            result = S_OK;
        }
        else {
            logger.error(L"Event was modified during execution!");
            logger.error(&L"expected code: " [ (int)scgms::NDevice_Event_Code::Information]);
            logger.error(&L"expected code: " [ (int)recievedEvent->event_code]);
            result = E_FAIL;
        }
    }
    else {
        logger.error(L"expected result: " + Widen_Char(std::system_category().message(S_OK).c_str()));
        logger.error(L"actual result: " + Widen_Char(std::system_category().message(result).c_str()));
        result = E_FAIL;
    }

    event->Release();
    shutDownTest();
    return result;
}

/**
 * Executes configuration test based on given parameters. Every filter should be successfully configured only if all parts
 * of the configuration are provided. Configuration test will be evaluated as successful and return S_OK only if the result of configuration
 * is identical with given expectedResult parameter, otherwise will return E_FAIL.
 *
 * @param memory configuration string
 * @param expectedResult expected result of this configuration
 * @return S_OK only if the result of given configuration is identical with given expectedResult parameter, otherwise E_FAIL
 */
HRESULT GenericUnitTester::configurationTest(const std::string &memory, const HRESULT expectedResult) {
    if (!isFilterLoaded())
    {
        std::wcerr << L"No filter loaded! Can't execute test.\n";
        logger.error(L"No filter loaded! Can't execute test.");
        return E_FAIL;
    }

    scgms::SPersistent_Filter_Chain_Configuration configuration;
    refcnt::Swstr_list errors;

    HRESULT result = configuration ? S_OK : E_FAIL;
    if (result == S_OK)
    {
        configuration->Load_From_Memory(memory.c_str(), memory.size(), errors.get());
        logger.info(L"Loading configuration from memory...");
    }
    else {
        logger.error(L"Error while creating configuration!");
        return E_FAIL;
    }

    scgms::IFilter_Configuration_Link** begin, ** end;
    configuration->get(&begin, &end);

    logger.info(L"Configuring filter...");
    result = testedFilter->Configure(begin[0], errors.get());

    HRESULT testResult;
    if (result == expectedResult) {
        testResult = S_OK;
    } else {
        logger.error(L"Provided configuration:\n"
                     L"(" + std::wstring(memory.begin(), memory.end()) + L")");
        logger.error(L"expected configuration result: " + Widen_Char(std::system_category().message(expectedResult).c_str()));
        logger.error(L"actual configuration result: " + Widen_Char(std::system_category().message(result).c_str()));
        testResult = E_FAIL;
    }

    shutDownTest();
    return testResult;
}