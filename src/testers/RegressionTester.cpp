//
// Created by Martin on 22.4.2020.
//
#include "RegressionTester.h"
#include "../../smartcgms/src/common/iface/DeviceIface.h"
#include "../../smartcgms/src/common/rtl/FilterLib.h""
#include "../../smartcgms/src/common/rtl/scgmsLib.h"
#include "../../smartcgms/src/common/rtl/referencedImpl.h"
#include "../../smartcgms/src/common/rtl/hresult.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>
#include<algorithm>

RegressionTester::RegressionTester(CDynamic_Library* library, std::wstring config_filepath) {
    this->library = library;
    this->config_filepath = config_filepath;
	this->loadLibrary();
}

std::vector<std::vector<std::string>> RegressionTester::setLogVector(std::string cLog) {
    using namespace std;
    string line;
    string divideBy = ";";

    ifstream myfile;
    myfile.open(cLog.c_str());
    if (!myfile.is_open())
    {
        cout << "Nepodarilo se soubor otevrit " << cLog << endl;
        exit(EXIT_FAILURE);
    }
    //Alokace vicerozmerneho pole na stringy
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

    vector<vector<string>> logArr = setLogVector(cLog.c_str());
    sort(logArr.begin() + 1, logArr.end(), [](const vector<string>& lhs, const vector<string>& rhs) {
        return std::stoi(lhs.at(0)) < std::stoi(rhs.at(0));
        });
    vector<vector<string>> resultArr = setLogVector(rLog.c_str());
    // TODO sort logArr podle logickych hodin (kontorola) 
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
                //vypis erroru chybi result[y1]
                mainErrorFlag = false;
                cout << "Missing line form result, position: " << y1 << endl;
            }
            errorFlag = false;


        }
        if (mainErrorFlag) {
            cout << "Test probehl uspesne" << endl;
            // vypis logu
            return S_OK;
        }

    }
    else {
        // loogy se neshoduji
        return E_FAIL;
    }


}


void print_and_empty_errors(refcnt::Swstr_list errors) {
	refcnt::wstr_container* wstr;
	if (errors->empty() != S_OK) {
		std::wcerr << "Error description: " << std::endl;
		while (errors->pop(&wstr) == S_OK) {
			std::wcerr << refcnt::WChar_Container_To_WString(wstr) << std::endl;
			wstr->Release();
		}
	}
}

void RegressionTester::loadLibrary() {
	library->Load(L"scgms");
	if (!library->Is_Loaded()) {
		std::wcerr << L"Couldn't load library!\n";
		exit(S_FALSE);
	}
	scgms::SPersistent_Filter_Chain_Configuration configuration;

	refcnt::Swstr_list errors;


	HRESULT rc = configuration ? S_OK : E_FAIL;
	if (rc == S_OK) configuration->Load_From_File(this->config_filepath.c_str(), errors.get());
	print_and_empty_errors(errors);

	if (!SUCCEEDED(rc)) {
		std::wcerr << L"Cannot load the configuration file " << this->config_filepath << std::endl << L"Error code: " << rc << std::endl;
		//return 2;
	}

	if (rc == S_FALSE) {
		std::wcerr << L"Warning: some filters were not loaded!" << std::endl;
	}
	scgms::SFilter_Executor gFilter_Executor;
	gFilter_Executor = scgms::SFilter_Executor{ configuration.get(), nullptr, nullptr, errors };
	print_and_empty_errors(errors);   //prints errors only if there are some

	if (!gFilter_Executor)
	{
		std::wcerr << L"Could not execute the filters!" << std::endl;
		//return 1;
	}

	
	// wait for filters to finish, or user to close the app
	gFilter_Executor->Terminate();
	//std::wcerr << L"Everything went well" << std::endl;
}

