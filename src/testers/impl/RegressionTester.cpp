//
// Created by Martin on 22.4.2020.
//
#include "../RegressionTester.h"
#include "../../utils/constants.h"
#include <iface/DeviceIface.h>
#include <rtl/FilterLib.h>
#include <rtl/referencedImpl.h>
#include <rtl/hresult.h>
#include <utils/string_utils.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <utility>
#include <vector>
#include <algorithm>

RegressionTester::RegressionTester(std::wstring config_filepath) : config_filepath(std::move(config_filepath)),
                                                                   resultLog(Narrow_WChar(st::LOG_FILE)){
    loadConfig();
}

void MainCalling RegressionTester::loadConfig() {

    if (config_filepath.empty()) {
        Logger::getInstance().error(L"Cannot load configuration from empty path!");
        throw std::invalid_argument("Cannot load configuration from empty path!");
    }

    scgms::SPersistent_Filter_Chain_Configuration configuration;

    refcnt::Swstr_list errors;


    HRESULT rc = configuration ? S_OK : E_FAIL;
    if (rc == E_FAIL) {
        throw std::runtime_error("Error creating configuration instance");
    }

    configuration->Load_From_File(this->config_filepath.c_str(), errors.get());
    errors.for_each([](auto str) { std::wcerr << str << std::endl; });

    if (!Succeeded(rc)) {
        std::wcerr << L"Cannot load the configuration file " << this->config_filepath << std::endl << std::endl;
        throw std::ios_base::failure("Cannot load the configuration file");
    }

    if (rc == S_FALSE) {
        std::wcerr << L"Warning: some filters were not loaded!" << std::endl;
        Logger::getInstance().warn(L"Warning: some filters were not loaded!");
        printAndEmptyErrors(errors);
    }

    scgms::SFilter_Executor gFilter_Executor { configuration.get(), nullptr, nullptr, errors };

    printAndEmptyErrors(errors);

    if (!gFilter_Executor) {
        std::wcerr << L"Could not execute the filters!" << std::endl;
        throw std::runtime_error("Could not execute the filters!");
    }


    // wait for filters to finish, or user to close the app
    gFilter_Executor->Terminate(true);
}

void RegressionTester::printAndEmptyErrors(const refcnt::Swstr_list& errors) {
    refcnt::wstr_container* wstr;
    if (errors->empty() != S_OK) {
        std::wcerr << "Error description: " << std::endl;
        while (errors->pop(&wstr) == S_OK) {
            std::wcerr << refcnt::WChar_Container_To_WString(wstr) << std::endl;
            Logger::getInstance().error(refcnt::WChar_Container_To_WString(wstr));
            wstr->Release();
        }
    }
}

std::vector<std::vector<std::string>> RegressionTester::readLogFile(const std::string& logPath) {
    std::string line;
    std::string delimiter = ";";

    std::ifstream logFile(logPath);
    if (!logFile) {
        std::wcerr << "Couldn't open the file \"" << Widen_Char(logPath.c_str()) << "\"\n";
        Logger::getInstance().error(L"Couldn't open the file \"" + Widen_Char(logPath.c_str()) + L"\"");
        exit(EXIT_FAILURE);
    }

    std::vector<std::vector<std::string>> loggedData;

    while (getline(logFile, line)) {
        std::string token;
        size_t pos = 0;
        std::vector<std::string> tmpVec;
        while ((pos = line.find(delimiter)) != std::string::npos) {
            tmpVec.push_back(line.substr(0, pos));
            line.erase(0, pos + delimiter.length() + 1);
        }
        loggedData.push_back(tmpVec);
    }

    return loggedData;
}

bool RegressionTester::compareLines(const std::vector<std::string>& resultLogLine, const std::vector<std::string>& referenceLogLine) {
    for (size_t i = st::firstComparedIndex; i < referenceLogLine.size(); i++) {

        if ((i >= st::firstNumberValueIndex) && (i <= st::lastNumberValueIndex)) {
            std::string expectedField = referenceLogLine.at(i);
            std::string actualField = resultLogLine.at(i);

            if (expectedField.empty() && actualField.empty()) {
                continue;
            } else if (expectedField.empty() || actualField.empty()) {
                return false;
            }

            double expected = std::stod(referenceLogLine.at(i));
            double actual = std::stod(resultLogLine.at(i));
            double lambda = 0.0001;

            if (abs(expected - actual) > lambda) {
                return false;
            }
        } else {
            if (referenceLogLine.at(i) != resultLogLine.at(i)) {
                return false;
            }
        }
    }

    return true;
}

HRESULT RegressionTester::compareLogs(const std::string& referenceLog) {

    if (config_filepath.empty()) {
        std::wcerr << L"Can't compare logs without configuration!\n";
        Logger::getInstance().error(L"Can't compare logs without configuration!");
        return E_FAIL;
    }

    std::vector<std::vector<std::string>> resultLogLinesVector = readLogFile(this->resultLog);
    std::sort(resultLogLinesVector.begin() + 1, resultLogLinesVector.end(),
        [](const std::vector<std::string>& first, const std::vector<std::string>& second) {
        return std::stoi(first.at(0)) < std::stoi(second.at(0));
        });

    std::vector<std::vector<std::string>> referenceLogLinesVector = readLogFile(referenceLog);
    std::sort(referenceLogLinesVector.begin() + 1, referenceLogLinesVector.end(),
        [](const std::vector<std::string>& first, const std::vector<std::string>& second) {
        return std::stoi(first.at(0)) < std::stoi(second.at(0));
        });

    std::vector<std::vector<std::string>> missingLines;
    std::vector<std::string> mismatchLine;
    std::vector<std::string> expectedLine;
    bool logsAreEqual = true;
    bool match = false;
    bool firstMismatch = true;
    int lastComparedLine = 1;
    if (resultLogLinesVector[0].size() != referenceLogLinesVector[0].size()) {
        // different number of parametes in line is not correct
        std::wcerr << L"There is different number of parameters in first line!\n";
        Logger::getInstance().error(L"There is different number of parameters in first line!");
        return E_FAIL;
    }

    for (size_t i = 1; i < referenceLogLinesVector.size(); i++) {
        for (size_t j = lastComparedLine; j < resultLogLinesVector.size(); j++) {
            if (compareLines(resultLogLinesVector.at(j), referenceLogLinesVector.at(i))) {
                resultLogLinesVector.erase(resultLogLinesVector.begin() + j);
                lastComparedLine = j;
                match = true;
                break;
            }
        }

        if (!match) {
            missingLines.push_back(referenceLogLinesVector.at(i));
            if (firstMismatch) {
                expectedLine = missingLines.at(0);
                mismatchLine = resultLogLinesVector.at(lastComparedLine);
                firstMismatch = false;
            }
            logsAreEqual = false;

        }
        match = false;
    }

    if (logsAreEqual) {
        std::wcout << "Test result is OK!\n";
        Logger::getInstance().info(L"Test result is OK!");
        if (resultLogLinesVector.size() > 1) {
            Logger::getInstance().info(L"There were reduntant lines found:");
            std::wcout << L"There were redundant lines found!\n";
            resultLogLinesVector.erase(resultLogLinesVector.begin());
            printLines(resultLogLinesVector);
        }

        return S_OK;
    } else {
        Logger::getInstance().error(L"Test failed!");
        Logger::getInstance().error(L"First mismatch:");
        Logger::getInstance().error(L"Expected line:");
        printOneLine(expectedLine);
        Logger::getInstance().error(L"Actual line:");
        printOneLine(mismatchLine);
        Logger::getInstance().error(L"Lines that were not found:");
        printLines(missingLines);

        std::wcout << L"There were lines missing in log file!\n";
        std::wcout << L"Test failed!\n";
        return E_FAIL;
    }
}

void RegressionTester::printOneLine(const std::vector<std::string>& line) {
    std::string printedLine;
    for (auto & i : line) {
        printedLine.append(i + "; ");
    }
    Logger::getInstance().error(Widen_Char(printedLine.c_str()));
}

void RegressionTester::printLines(const std::vector<std::vector<std::string>>& errorResult) {
    for (auto & y : errorResult) {
        std::string line;
        for (auto & x : y) {
            line += x + "; ";
        }
        Logger::getInstance().info(Widen_Char(line.c_str()));
    }
}

