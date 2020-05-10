//
// Created by Martin on 22.4.2020.
//
#include "RegressionTester.h"
#include "../../smartcgms/src/common/iface/DeviceIface.h"
#include "../../smartcgms/src/common/rtl/FilterLib.h""
#include "../../smartcgms/src/common/rtl/scgmsLib.h"
#include "../../smartcgms/src/common/rtl/referencedImpl.h"
#include <iostream>

RegressionTester::RegressionTester(CDynamic_Library* library, std::wstring config_filepath) {
    this->library = library;
    this->config_filepath = config_filepath;
	this->loadLibrary();
}

void RegressionTester::compareLogs()
{

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
	library->Load(L"../scgms");
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
	gFilter_Executor->Wait_For_Shutdown_and_Terminate();
	//std::wcerr << L"Everything went well" << std::endl;
}

