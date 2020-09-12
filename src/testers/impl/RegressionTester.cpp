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

RegressionTester::RegressionTester(const std::wstring& config_filepath) {
    this->config_filepath = config_filepath;
	this->loadConfig();
}

std::vector<std::vector<std::string>> RegressionTester::setLogVector(const std::string& cLog) {
    std::string line;
    std::string divideBy = ";";
    
    std::ifstream myfile;
    myfile.open(cLog.c_str());
    if (!myfile.is_open())
    {
        std::wcerr << "Couldn't open the file \"" << Widen_Char(cLog.c_str()) << "\"\n";
        logger.error(L"Couldn't open the file \"" + Widen_Char(cLog.c_str()) + L"\"");
        exit(EXIT_FAILURE);
    }

    std::vector<std::vector<std::string>> logArr;

    while (getline(myfile, line)) {
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

bool RegressionTester::compareLines(std::vector<std::string> log, std::vector<std::string> result) {
    for (size_t i = firstComparedIndex; i < result.size(); i++) {

        if ((i >= firstNumberValueIndex) && (i <= lastNumberValueIndex))
        {
            std::string expectedField = result.at(i);
            std::string actualField = log.at(i);
            if (expectedField.empty() && actualField.empty())
            {
                continue;
            }
            else if (expectedField.empty() || actualField.empty()) {
                return false;
            }

            double expected = std::stod(result.at(i));
            double actual = std::stod(log.at(i));
            double lambda = 0.0001;

            if (abs(expected - actual) > lambda)
            {
                return false;
            }
        }
        else {
            if (result.at(i) != log.at(i)) {
                return false;
            }
        }
    }

    return true;
}

HRESULT RegressionTester::compareLogs(const std::string& cLog, const std::string& rLog) {

    if (config_filepath.empty())
    {
        std::wcerr << L"Can't compare logs without configuration!\n";
        logger.error(L"Can't compare logs without configuration!");
        return E_FAIL;
    }

    std::vector<std::vector<std::string>> logArr = setLogVector(cLog);
    sort(logArr.begin() + 1, logArr.end(), [](const std::vector<std::string>& lhs, const std::vector<std::string>& rhs) {
        return std::stoi(lhs.at(0)) < std::stoi(rhs.at(0));
        });

    std::vector<std::vector<std::string>> resultArr = setLogVector(rLog);
    sort(resultArr.begin() + 1, resultArr.end(), [](const std::vector<std::string>& lhs, const std::vector<std::string>& rhs) {
        return std::stoi(lhs.at(0)) < std::stoi(rhs.at(0));
        });

    std::vector<std::vector<std::string>> errorResult;
    std::vector<std::string> mismatchLine;
    std::vector<std::string> expectedLine;
    bool errorFlag = false;
    bool mainErrorFlag = true;
    bool firstMismatch = true;
    int iterator = 1;
    if (logArr[0].size() == resultArr[0].size()) {
        for (size_t y1 = 1; y1 < resultArr.size(); y1++) {
            for (size_t y2 = iterator; y2 < logArr.size(); y2++) {
                if (compareLines(logArr.at(y2), resultArr.at(y1))) {
                    logArr.erase(logArr.begin() + y2);
                    iterator = y2;
                    errorFlag = true;
                    break;
                }
            }

            if (!errorFlag) {
                errorResult.push_back(resultArr.at(y1));
                if (firstMismatch)
                {
                    expectedLine = errorResult.at(0);
                    mismatchLine = logArr.at(iterator);
                    firstMismatch = false;
                }
                mainErrorFlag = false;
                
            }
            errorFlag = false;
        }
        if (mainErrorFlag) {
            std::wcout << "Test result is OK!\n";
            logger.info(L"Test result is OK!");
            if (logArr.size() > 1)
            {
                logger.info(L"There were reduntant lines found:");
                std::wcout << L"There were redundant lines found!\n";
                logArr.erase(logArr.begin());
                printLines(logArr);
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
            printLines(errorResult);

            std::wcout << L"There were lines missing in log file!\n";
            std::wcout << L"Test failed!\n";
            return E_FAIL;
        }
    }
    else {  // different number of parametes in line is not correct
        std::wcerr << L"There is different number of parameters in first line!\n";
        logger.error(L"There is different number of parameters in first line!");
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

void RegressionTester::loadConfig() {

    signal(SIGINT, sighandler);

	scgms::SPersistent_Filter_Chain_Configuration configuration;

	refcnt::Swstr_list errors;


	HRESULT rc = configuration ? S_OK : E_FAIL;
	if (rc == S_OK) configuration->Load_From_File(this->config_filepath.c_str(), errors.get());

	if (!SUCCEEDED(rc)) {
		std::wcerr << L"Cannot load the configuration file " << this->config_filepath << std::endl << L"Error code: " << rc << std::endl;
        exit(E_FAIL);
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
	gFilter_Executor->Wait_For_Shutdown_and_Terminate();
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

