// main.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"
#include "../../smartcgms/src/common/iface/FilterIface.h"
#include "../../smartcgms/src/common/rtl/guid.h"
#include "TestFilter.h"

#ifdef _WIN32
 wchar_t* LIB_DIR = L"../../smartcgms/windows_64/filters/";
#elif __APPLE__
const wchar_t* LIB_DIR = L"../../smartcgms/macos_64/filters/";
#else
const wchar_t* LIB_DIR = L"../../smartcgms/debian_64/filters/";
#endif

/**
    Prints few types how to use the application and which parameters to specify.
*/
void print_help(){
    std::wcerr << "Execute with two parameters <test_type> <tested_subject>\n"
                  "<test_type> ... '-u' = filter unit tests, '-r' = scenario regression tests\n"
                  "<tested_subject> ... <filter_path> - filter to test with unit tests, "
                  "<config_path> - path to filter chain config file\n"
                  "a) -u <filter_path>\n"
                  "b) -r <config_path>\n";
}

/**
    Loads library of given filter and starts unit tests.
*/
int execute_unit_testing(const char *filter_name){
    CDynamic_Library::Set_Library_Base(LIB_DIR);

    CDynamic_Library library;

    if (!library.Load(L"log.dll")) {
        std::cout << "Couldn't load library!";
        return 3;
    }

    auto creator = library.Resolve<scgms::TCreate_Filter>("do_create_filter");

    scgms::IFilter *created_filter = nullptr;

    // id log filtru
    GUID guid = {0xc0e942b9, 0x3928, 0x4B81, { 0x9b, 0x43, 0xa3, 0x47, 0x66, 0x82, 0x00, 0xba}};

    TestFilter filter = TestFilter();
    auto result = creator(&guid, &filter, &created_filter);
    if (result == S_OK){
        std::cout << "Filter created succesfully.";
    } else {
        std::cout << result;
    }
    return 0;
}

/**
    Loads filter configuration and executes regression tests.
*/
int execute_regression_testing(const char* config_path){
    return 0;
}

/**
    Entry point of application.
*/
int MainCalling main(int argc, char * argv[])
{
    if (argc < 3){
        std::wcerr << L"Wrong parameter count!\n";
        print_help();
        return 1;
    }

    if (argv[1][0] == '-'){
        switch (argv[1][1]){
            case 'u':
                //TODO unit testing
                return execute_unit_testing(argv[2]);
            case 'r':
                //TODO regression testing
                return execute_regression_testing(argv[2]);
            default:
                std::wcerr << L"Unknown type of testing requested!\n";
                print_help();
                return 2;
        }
    }
    else {
        std::wcerr << L"Unsupported command: " << argv[1] << "\n";
        print_help();
        return 2;
    }

	return 0;
}
