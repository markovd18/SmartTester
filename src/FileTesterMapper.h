#pragma once
#ifndef _FILE_TESTER_MAPPER_H_
#define _FILE_TESTER_MAPPER_H_

#include <map>
#include <functional>
#include "GenericUnitTester.h"
#include "../../smartcgms/src/common/rtl/guid.h"

class FileTesterMapper {

private:
	std::map<const wchar_t*, std::function<GenericUnitTester*(CDynamic_Library*, TestFilter*, const GUID*)>> fileTesterMap;
	FileTesterMapper();
	template<typename T>
	GenericUnitTester* createInstance(CDynamic_Library* library, TestFilter* testFilter, const GUID* testedGuid);
public:
	static FileTesterMapper& GetInstance();
	GenericUnitTester* getTesterInstance(const wchar_t* fileName, CDynamic_Library* library, TestFilter* testFilter,const GUID* guid);
	FileTesterMapper(FileTesterMapper const&) = delete;
	void operator=(FileTesterMapper const&) = delete;

};
#endif // !_FILE_TESTER_MAPPER_H_

