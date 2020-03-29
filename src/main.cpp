// main.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "../../smartcgms/src/common/rtl/Dynamic_Library.h"

#ifdef _WIN32
const wchar_t* lib_dir = L"../smartcgms/windows_64/filters/";
#elif __APPLE__
const wchar_t* lib_dir = L".../smartcgms/macos_64/filters/";
#else
const wchar_t* lib_dir = L"../smartcgms/debian_64/filters/";
#endif

/**
*	Creates complete path to library from given path to folder and library file name.
*/
wchar_t* get_library_path(const wchar_t* lib_dir, const wchar_t* lib) {
	//zatím nefunguje
	size_t path_len = std::wcslen(lib_dir);
	size_t name_len = std::wcslen(lib);

	wchar_t* path = new wchar_t[(size_t)path_len];
	memcpy(path, lib_dir, path_len);
	wcscat(path, lib);

	std::cout << path;
	return path;
}

int MainCalling main()
{
	const wchar_t* lib = L"..\\smartcgms\\windows_64\\filters\\log.dll";

	CDynamic_Library* library = new CDynamic_Library();
	
	library->Load(lib);

	if (library->Resolve("do_create_filter") != NULL) {
		std::cout << true;
	}
	else {
		std::cout << false << std::endl;
	}


	return 0;
}
