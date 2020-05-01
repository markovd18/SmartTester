
#include "FileTesterMapper.h"
#include "constants.h"
#include "LogFilterUnitTester.h"
#include "../../smartcgms/src/common/rtl/guid.h"

FileTesterMapper::FileTesterMapper() {
	using std::placeholders::_3;

	fileTesterMap[LOG_LIBRARY] = std::bind(&FileTesterMapper::createInstance<LogFilterUnitTester>, this, _3);
}

/**
	Creates and returns instance of class derived from GenericUnitTester based on given guid.
*/
template<typename T>
GenericUnitTester* FileTesterMapper::createInstance(CDynamic_Library* library, TestFilter* testFilter, const GUID* testedGuid) {
	return new T(library, testFilter, testedGuid);
}

/**
	Returns instace of FileTesterMapper.
*/
FileTesterMapper& FileTesterMapper::GetInstance() {
	static FileTesterMapper instance;
	return instance;
}

/**
	Returns instance of class derived from GenericUnitTester based on given file name and guid.
*/
GenericUnitTester* FileTesterMapper::getTesterInstance(const wchar_t* fileName, CDynamic_Library* library, TestFilter* testFilter, const GUID* guid) {
	return fileTesterMap[fileName](library, testFilter, guid);
}