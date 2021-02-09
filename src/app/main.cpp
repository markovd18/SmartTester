// main.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <string>
#include <rtl/guid.h>
#include <utils/string_utils.h>
#include <rtl/FilesystemLib.h>
#include "../utils/UnitTestExecutor.h"
#include "../utils/constants.h"
#include "../testers/RegressionTester.h"


/**
    Prints few tips on how to use the application and which parameters to specify.
*/
void print_help() {
    std::wcerr << "Execute with two parameters <test_type> <tested_subject>\n"
        "<test_type> ... '-u' = filter unit tests / '-r' = scenario regression tests\n"
        "<tested_subject> ... <filter_guid> - GUID of filter to test with unit tests / "
        "<config_path> - path to filter chain config file\n"
        "a) -u <filter_guid>\n"
        "b) -r <config_path>\n"
        "If no <filter_guid> is passed, all tests across all filters will be executed.\n";
}

/**
 * Parses guid in string from command-line into hexadecimal numbers and creates GUID structure.
 * If invalid format of guid is passed as parameter, throws an exception and system will shut down.
 * If no guid is passed as parameter, "invalid" guid is returned.
 *
 * @param guid_string command-line parameter to be parsed into GUID
 * @return parsed GUID if correct format is passed as parameter, else "invalid" GUID
 */
GUID parse_guid(std::string guid_string) {

    GUID guid{};
    if (!guid_string.empty()) {
        Logger::getInstance().debug(L"Parsing GUID: " + std::wstring(guid_string.begin(), guid_string.end()) + L"...");
        std::string delimiter = "-";
        std::string token;
        std::string sub_token;
        size_t position;
        int counter = 0;

        try {
            while ((position = guid_string.find(delimiter)) != std::string::npos) {
                token = guid_string.substr(0, position);

                switch (counter) {
                case 0:
                    guid.Data1 = std::stoul(token, nullptr, 16);
                    break;
                case 1:
                    guid.Data2 = std::stoi(token, nullptr, 16);
                    break;
                case 2:
                    guid.Data3 = std::stoi(token, nullptr, 16);
                    break;
                case 3:
                    for (int i = 0; i < 2; i++) {
                        sub_token = token.substr(0, 2 * sizeof(char));
                        guid.Data4[i] = std::stoi(sub_token, nullptr, 16);
                        token.erase(0, 2 * sizeof(char));
                    }
                    break;
                default:
                    std::wcerr << L"Invalid format of GUID passed!\n"
                        << "Expected format: " << st::GUID_FORMAT << "\n";
                    Logger::getInstance().error(L"Invalid format of GUID passed!");
                    exit(2);
                }

                guid_string.erase(0, position + delimiter.length());
                counter++;
            }

            for (int i = 2; i < 8; i++) {
                token = guid_string.substr(0, 2 * sizeof(char));
                guid.Data4[i] = std::stoi(token, nullptr, 16);
                guid_string.erase(0, 2 * sizeof(char));
            }
        }
        catch (std::exception&) {
            std::wcerr << L"Invalid format of GUID passed!\n"
                << L"Expected format: " << st::GUID_FORMAT << "\n";
            Logger::getInstance().error(L"Invalid format of GUID passed!");
            exit(2);
        }
    }
    else {
        Logger::getInstance().info(L"No GUID passed...");
    }
    return guid;
}

/**
 * Executes unit testing on all filters or on specific filter with given GUID.
 *
 * @param guid_string guid passed as command-line argument in string format
 */
void execute_unit_testing(std::string& guid_string) {

    GUID guid = parse_guid(guid_string);

    UnitTestExecutor executor;
    
    if (Is_Invalid_GUID(guid)) {
        executor.executeAllTests();
    } else {
        executor.executeFilterTests(guid);
    }

}

/**
 * Loads filter configuration from given path and executes regression tests.
 *
 * @param config_filepath path to configuration file
 * @return result of regression testing
 */
HRESULT execute_regression_testing(const std::wstring& config_filepath) {
    if (config_filepath.empty()) {
        std::wcerr << L"Inserted empty file path!\n";
        return 1;
    }

    HRESULT result;
    try {
        RegressionTester regTester(config_filepath);
        std::string log_filepath = Narrow_WString(config_filepath);

        log_filepath.erase(log_filepath.size() - Narrow_WChar(st::CONFIG_FILE).size());
        log_filepath += Narrow_WChar(st::LOG_FILE);

        result = regTester.compareLogs(log_filepath);
    } catch (const std::exception& ex) {
        std::wcerr << L"Error while executing configuration!\n" << ex.what() << std::endl;
        return E_FAIL;
    }

    std::filesystem::create_directory(st::TMP_DIR);
    std::ifstream file(Narrow_WChar(st::TMP_LOG_FILE));
    if (file.good()) {
        file.close();
        std::remove(Narrow_WChar(st::TMP_LOG_FILE).c_str());
    }
    file.close();
    std::rename(Narrow_WChar(st::LOG_FILE).c_str(), Narrow_WChar(st::TMP_LOG_FILE).c_str());

    Logger::getInstance().info(L"Shutting down.");
    std::wcerr << L"For detailed information see generated log.\n";
    return result;
}

/**
    Entry point of the application.
*/
int main(int argc, char* argv[]) {
    Logger::getInstance().info(L"Starting SmartTester application...");
    std::wcout << L"Starting SmartTester application...\n";

    if (argc < 2) {
        std::wcerr << L"Wrong parameter count!\n";
        Logger::getInstance().error(L"Wrong parameter count passed!");
        print_help();
        return 1;
    }

    if (argv[1][0] == '-') {
        std::string parameter;
        std::wstring config_filepath;

        switch (argv[1][1]) {
        case 'u':   // unit testing
            if (argv[2] != nullptr) {
                parameter = argv[2];
            }
            Logger::getInstance().info(L"Unit tests will be executed.");
            std::wcout << L"Executing unit tests.\n";
            execute_unit_testing(parameter);
            break;
        case 'r':   // regression testing
            Logger::getInstance().info(L"Regression tests will be executed.");
            std::wcout << L"Executing regression tests.\n";
            config_filepath = argc > 2 ? std::wstring{ argv[2], argv[2] + strlen(argv[2]) } : std::wstring{};
            return execute_regression_testing(config_filepath);
        default:
            std::wcerr << L"Unknown type of testing requested!\n";
            Logger::getInstance().error(L"Unknown type of testing requested!");
            print_help();
            return 2;
        }
    }
    else {
        std::wcerr << L"Unsupported command: " << argv[1] << "\n";
        Logger::getInstance().error(L"Unsupported command passed!");
        print_help();
        return 2;
    }

    Logger::getInstance().info(L"Shutting down.");
    std::wcout << L"For detailed information see generated log.\n";
    return 0;
}
