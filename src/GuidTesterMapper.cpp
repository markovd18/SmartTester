
#include "GuidTesterMapper.h"
#include "constants.h"
#include "LogFilterUnitTester.h"
#include "../../smartcgms/src/common/rtl/guid.h"

GuidTesterMapper::GuidTesterMapper() {
	guidTesterMap[LOG_GUID] = std::bind<GenericUnitTester*>(&GuidTesterMapper::createInstance<LogFilterUnitTester>, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

/**
	Creates and returns instance of class derived from GenericUnitTester based on given guid.
*/
template<typename T>
GenericUnitTester* GuidTesterMapper::createInstance(CDynamic_Library* library, TestFilter* testFilter, const GUID* testedGuid) {
	//logger.info(L"Creating an instance of class derived from GenericUnitTester based on given guid...");
	return new T(library, testFilter, testedGuid);
}

/**
	Returns instace of FileTesterMapper.
*/
GuidTesterMapper& GuidTesterMapper::GetInstance() {
	static GuidTesterMapper instance;
	return instance;
}

/**
	Returns instance of class derived from GenericUnitTester based on given file name and guid.
*/
GenericUnitTester* GuidTesterMapper::getTesterInstance(CDynamic_Library* library, TestFilter* testFilter, const GUID* guid) {
	//logger.info(L"Getting instance of class derived from GenericUnitTester based on given file name and guid...");
	return guidTesterMap[*guid](library, testFilter, guid);
}