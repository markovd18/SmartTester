// main.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"
#include "../../smartcgms/src/common/iface/FilterIface.h"
#include "../../smartcgms/src/common/rtl/guid.h"
#include "TestFilter.h"

#ifdef _WIN32
 wchar_t* lib_dir = L"../smartcgms/windows_64/filters/";
#elif __APPLE__
const wchar_t* lib_dir = L"../smartcgms/macos_64/filters/";
#else
const wchar_t* lib_dir = L"../smartcgms/debian_64/filters/";
#endif


void print_help(){
    std::wcerr << "Execute with two parameters <test_type> <tested_subject>\n"
                  "<test_type> ... u - filter unit tests, r - scenario regression tests\n"
                  "<tested_subject> ... <filter_path> - filter to test with unit tests, "
                  "<config_path> - path to filter chain config file\n"
                  "a) u <filter_path>\n"
                  "b) r <config_path>\n";
}

int execute_unit_testing(const char *filter_name){
    CDynamic_Library::Set_Library_Base(lib_dir);

    if (!CDynamic_Library::Is_Library(L"log.dll")){
        std::wcerr << "Not a library!\n";
        return 3;
    }

    CDynamic_Library library;
    library.Load(L"log.dll");

    auto creator = library.Resolve<scgms::TCreate_Filter>("do_create_filter");
    scgms::IFilter** created_filter; // ??

    // id log filtru
    GUID guid = {0xc0e942b9, 0x3928, 0x4B81, { 0x9b, 0x43, 0xa3, 0x47, 0x66, 0x82, 0x00, 0xba}};

    TestFilter *filter = new TestFilter;
    auto result = creator(&guid, filter, created_filter);
    if (result == S_OK){
        std::cout << "Povedlo se vytvorit.";
    } else {
        std::cout << result;
    }

    return 0;
}

int execute_regression_testing(){
    return 0;
}

int MainCalling main(int argc, char * argv[])
{
    if (argc < 3){
        std::wcerr << L"Wrong parameter count!\n";
        print_help();
        return 1;
    }

    switch (argv[1][0]){
        case 'u':
            //TODO unit testing
            return execute_unit_testing(argv[2]);
        case 'r':
            //TODO regression testing
            break;
        default:
            std::wcerr << L"Unknown type of testing requested!\n";
            print_help();
            return 2;
    }
	return 0;
}
