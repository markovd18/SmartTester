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
#include <csignal>

void RegressionTester::printAndEmptyErrors(const refcnt::Swstr_list& errors) {
    refcnt::wstr_container* wstr;
    if (errors->empty() != S_OK) {
        std::wcerr << "Error description: " << std::endl;
        while (errors->pop(&wstr) == S_OK) {
            std::wcerr << refcnt::WChar_Container_To_WString(wstr) << std::endl;
            logger.error(refcnt::WChar_Container_To_WString(wstr));
            wstr->Release();
        }
    }
}

RegressionTester::RegressionTester(std::wstring config_filepath) : config_filepath(std::move(config_filepath)),
                                                            resultLog(Narrow_WChar(LOG_FILE)){
	loadConfig();
}

std::vector<std::vector<std::string>> RegressionTester::readLogFile(const std::string& logPath) {
    std::string line;
    std::string divideBy = ";";
    
    std::ifstream logFile(logPath);
    if (!logFile)
    {
        std::wcerr << "Couldn't open the file \"" << Widen_Char(logPath.c_str()) << "\"\n";
        logger.error(L"Couldn't open the file \"" + Widen_Char(logPath.c_str()) + L"\"");
        exit(EXIT_FAILURE);
    }

    std::vector<std::vector<std::string>> logArr;

    while (getline(logFile, line)) {
        std::string token;
        size_t pos = 0;
        std::vector<std::string> tmpVec;
        while ((pos = line.find(divideBy)) != std::string::npos) {
            tmpVec.push_back(line.substr(0, pos));
            line.erase(0, pos + divideBy.length() + 1);
        }
        logArr.push_back(tmpVec);
    }

    return logArr;
}

bool RegressionTester::compareLines(std::vector<std::string> resultLogLine, std::vector<std::string> referenceLogLine) {
    for (size_t i = firstComparedIndex; i < referenceLogLine.size(); i++) {

        if ((i >= firstNumberValueIndex) && (i <= lastNumberValueIndex))
        {
            std::string expectedField = referenceLogLine.at(i);
            std::string actualField = resultLogLine.at(i);
            if (expectedField.empty() && actualField.empty())
            {
                continue;
            }
            else if (expectedField.empty() || actualField.empty()) {
                return false;
            }

            double expected = std::stod(referenceLogLine.at(i));
            double actual = std::stod(resultLogLine.at(i));
            double lambda = 0.0001;

            if (abs(expected - actual) > lambda)
            {
                return false;
            }
        }
        else {
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
        logger.error(L"Can't compare logs without configuration!");
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
        logger.error(L"There is different number of parameters in first line!");
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
            if (firstMismatch)
            {
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
        logger.info(L"Test result is OK!");
        if (resultLogLinesVector.size() > 1)
        {
            logger.info(L"There were reduntant lines found:");
            std::wcout << L"There were redundant lines found!\n";
            resultLogLinesVector.erase(resultLogLinesVector.begin());
            printLines(resultLogLinesVector);
        }

        return S_OK;
    }
    else {
        logger.error(L"Test failed!");
        logger.error(L"First mismatch:");
        logger.error(L"Expected line:");
        printOneLine(expectedLine);
        logger.error(L"Actual line:");
        printOneLine(mismatchLine);
        logger.error(L"Lines that were not found:");
        printLines(missingLines);

        std::wcout << L"There were lines missing in log file!\n";
        std::wcout << L"Test failed!\n";
        return E_FAIL;
    }
}
scgms::SFilter_Executor gFilter_Executor;

void MainCalling sighandler(int signo) {
    // SIGINT should terminate filters; this will eventually terminate whole app
    if (signo == SIGINT) {
        if (gFilter_Executor) {
            scgms::UDevice_Event shut_down_event{ scgms::NDevice_Event_Code::Shut_Down };
            gFilter_Executor.Execute(std::move(shut_down_event));
        }
    }
}

void MainCalling RegressionTester::loadConfig() {

    if (config_filepath.empty()) {
        logger.error(L"Cannot load configuration from empty path!");
        return;
    }

    signal(SIGINT, sighandler);

	scgms::SPersistent_Filter_Chain_Configuration configuration;

	refcnt::Swstr_list errors;


	HRESULT rc = configuration ? S_OK : E_FAIL;
	if (rc == S_OK) configuration->Load_From_File(this->config_filepath.c_str(), errors.get());

	printAndEmptyErrors(errors);

	if (!Succeeded(rc)) {
		std::wcerr << L"Cannot load the configuration file " << this->config_filepath << std::endl << L"Error code: " << rc << std::endl;
        exit(EXIT_FAILURE);
	}

	if (rc == S_FALSE) {
		std::wcerr << L"Warning: some filters were not loaded!" << std::endl;
        printAndEmptyErrors(errors);
	}

	gFilter_Executor = scgms::SFilter_Executor{ configuration.get(), nullptr, nullptr, errors };

	printAndEmptyErrors(errors);

	if (!gFilter_Executor)
	{
		std::wcerr << L"Could not execute the filters!" << std::endl;
        printAndEmptyErrors(errors);
        exit(EXIT_FAILURE);
	}

	
	// wait for filters to finish, or user to close the app
	gFilter_Executor->Terminate(true);
}

void RegressionTester::printOneLine(const std::vector<std::string>& line)
{
    std::string printedLine;
    for (auto & i : line)
    {
        printedLine.append(i + "; ");
    }
    logger.error(Widen_Char(printedLine.c_str()));
}

void RegressionTester::printLines(const std::vector<std::vector<std::string>>& errorResult)
{
    for (auto & y : errorResult) {
        std::string line;
        for (auto & x : y) {
            line += x + "; ";
        }
        logger.trace(Widen_Char(line.c_str()));
    }
}

