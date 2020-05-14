//
// Created by Martin on 22.4.2020.
//
#include "RegressionTester.h"
#include <iface/DeviceIface.h>
#include <rtl/FilterLib.h>
#include <rtl/scgmsLib.h>
#include <rtl/referencedImpl.h>
#include <rtl/hresult.h>
#include <utils/string_utils.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>
#include<algorithm>

void RegressionTester::printAndEmptyErrors(refcnt::Swstr_list errors) {
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

RegressionTester::RegressionTester(std::wstring config_filepath) {
    this->config_filepath = config_filepath;
	this->loadConfig();
}

std::vector<std::vector<std::string>> RegressionTester::setLogVector(std::string cLog) {
    using namespace std;
    string line;
    string divideBy = ";";
    
    ifstream myfile;
    myfile.open(cLog.c_str());
    if (!myfile.is_open())
    {
        wcerr << "Couldn't open the file \"" << Widen_Char(cLog.c_str()) << "\"\n";
        logger.error(L"Couldn't open the file \"" + Widen_Char(cLog.c_str()) + L"\"");
        exit(EXIT_FAILURE);
    }

    vector<vector<string>> logArr;

    while (getline(myfile, line)) {
        string token;
        size_t pos = 0;
        vector<string> tmpVec;
        while ((pos = line.find(divideBy)) != string::npos) {
            tmpVec.push_back(line.substr(0, pos));
            line.erase(0, pos + divideBy.length() + 1);
        }
        logArr.push_back(tmpVec);
    }

    return logArr;
}

bool RegressionTester::compareLines(std::vector<std::string> log, std::vector<std::string> result) {
    for (int i = 2; i < result.size(); i++) {
        if (result.at(i).compare(log.at(i))) {
            return false;
        }
    }

    return true;
}

HRESULT RegressionTester::compareLogs(std::string cLog, std::string rLog) {
    using namespace std;

    if (config_filepath.empty())
    {
        std::wcerr << L"Can't compare logs without configuration!\n";
        logger.error(L"Can't compare logs without configuration!");
        return E_FAIL;
    }

    vector<vector<string>> logArr = setLogVector(cLog.c_str());
    sort(logArr.begin() + 1, logArr.end(), [](const vector<string>& lhs, const vector<string>& rhs) {
        return std::stoi(lhs.at(0)) < std::stoi(rhs.at(0));
        });

    vector<vector<string>> resultArr = setLogVector(rLog.c_str());
    vector<vector<string>> errorResult;
    bool errorFlag = false;
    bool mainErrorFlag = true;
    int iterator = 1;
    if (logArr[0].size() == resultArr[0].size()) {
        for (int y1 = 1; y1 < resultArr.size(); y1++) {

            for (int y2 = iterator; y2 < logArr.size(); y2++) {
                if (compareLines(logArr.at(y2), resultArr.at(y1))) {
                    logArr.erase(logArr.begin() + y2);
                    iterator = y2;
                    errorFlag = true;
                    break;
                }
            }

            if (errorFlag == false) {

                mainErrorFlag = false;
                errorResult.push_back(resultArr.at(y1));
            }
            errorFlag = false;
        }
        if (mainErrorFlag) {
            wcout << "Test result is OK!" << endl;
            logger.info(L"Test result is OK!");
            if (logArr.size() > 1)
            {
                logger.info(L"There were reduntant lines found:");
                wcout << L"There were redundant lines found!\n";
                logArr.erase(logArr.begin());
                printLines(logArr);
            }

            return S_OK;
        }
        else {
            logger.error(L"There were lines missing in log file!");
            logger.error(L"Test failed!");
            wcout << L"There were lines missing in log file!\n";
            wcout << L"Test failed!\n";
            printLines(errorResult);
            return E_FAIL;
        }
    }
    else {  // different number of parametes in line is not correct
        wcerr << L"There is different number of parameters in first line!\n";
        logger.error(L"There is different number of parameters in first line!");
        return E_FAIL;
    }
}

void RegressionTester::loadConfig() {

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
	scgms::SFilter_Executor gFilter_Executor;
	gFilter_Executor = scgms::SFilter_Executor{ configuration.get(), nullptr, nullptr, errors };

	if (!gFilter_Executor)
	{
		std::wcerr << L"Could not execute the filters!" << std::endl;
        printAndEmptyErrors(errors);
        exit(EXIT_FAILURE);
	}

	
	// wait for filters to finish, or user to close the app
	gFilter_Executor->Terminate();
}

void RegressionTester::printLines(std::vector<std::vector<std::string>> errorResult)
{
    for (int y = 0; y < errorResult.size(); y++) {
        std::string line;
        for (int x = 0; x < errorResult[y].size(); x++) {
            line += errorResult[y][x] + "; ";
        }
        logger.trace(Widen_Char(line.c_str()));
    }
}

