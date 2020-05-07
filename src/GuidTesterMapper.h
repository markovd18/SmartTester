#pragma once
#ifndef _FILE_TESTER_MAPPER_H_
#define _FILE_TESTER_MAPPER_H_

#include <map>
#include <functional>
#include "GenericUnitTester.h"
#include "../../smartcgms/src/common/rtl/guid.h"

class GuidTesterMapper {

private:
	std::map<GUID, std::function<GenericUnitTester*(CDynamic_Library*, TestFilter*, const GUID*)>> guidTesterMap;
	GuidTesterMapper();
	template<typename T>
	GenericUnitTester* createInstance(CDynamic_Library* library, TestFilter* testFilter, const GUID* testedGuid);
public:
	static GuidTesterMapper& GetInstance();
	GenericUnitTester* getTesterInstance(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid);
	GuidTesterMapper(GuidTesterMapper const&) = delete;
	void operator=(GuidTesterMapper const&) = delete;

};
#endif // !_FILE_TESTER_MAPPER_H_
