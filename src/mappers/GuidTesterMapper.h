#pragma once
#ifndef _FILE_TESTER_MAPPER_H_
#define _FILE_TESTER_MAPPER_H_

#include <map>
#include <functional>
#include <rtl/guid.h>
#include "../testers/GenericUnitTester.h"
#include "../utils/Logger.h"

/**
 * Singleton class used to map GUID to appropriate instance of derived GenericUnitTester class.
 * GuidTesterMapper provides method to retrieve this instance but it requires CDynamic_Library helper class pointer
 * that will be used to load shared libraries of that filter, TestFilter class pointer that will be appended to tested filter
 * to collect the event passed to filter during testing and GUID of wanted filter.
 */
class GuidTesterMapper {

private:
	std::map<GUID, std::function<GenericUnitTester*(CDynamic_Library*, TestFilter*, const GUID*)>> guidTesterMap;
	GuidTesterMapper();
	template<typename T>
	GenericUnitTester* createInstance(CDynamic_Library* library, TestFilter* testFilter, const GUID* testedGuid);
public:
	Logger& logger = Logger::GetInstance();
	static GuidTesterMapper& GetInstance();
	GenericUnitTester* getTesterInstance(CDynamic_Library* library, TestFilter* testFilter,const GUID* guid);
	GuidTesterMapper(GuidTesterMapper const&) = delete;
	void operator=(GuidTesterMapper const&) = delete;

};
#endif // !_FILE_TESTER_MAPPER_H_

