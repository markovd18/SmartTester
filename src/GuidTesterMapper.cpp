
#include "GuidTesterMapper.h"
#include "constants.h"
#include "LogFilterUnitTester.h"
#include "DrawingFilterUnitTester.h"
#include "MappingFilterUnitTester.h"
#include "MaskingFilterUnitTester.h"
#include "../../smartcgms/src/common/rtl/guid.h"

GuidTesterMapper::GuidTesterMapper() {
	//inserting LogFilterUnitTester factory
	guidTesterMap[LOG_GUID] = std::bind<GenericUnitTester*>(&GuidTesterMapper::createInstance<LogFilterUnitTester>, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	//inserting DrawingFilterUnitTester factory
	guidTesterMap[DRAWING_GUID] = std::bind<GenericUnitTester*>(&GuidTesterMapper::createInstance<DrawingFilterUnitTester>, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	//inserting MappingFilterUnitTester factory
	guidTesterMap[MAPPING_GUID] = std::bind<GenericUnitTester*>(&GuidTesterMapper::createInstance<MappingFilterUnitTester>, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	//inserting MaskingFilterUnitTester factory
	guidTesterMap[MASKING_GUID] = std::bind<GenericUnitTester*>(&GuidTesterMapper::createInstance<MaskingFilterUnitTester>, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

/**
	Creates and returns instance of class derived from GenericUnitTester based on given guid.
*/
template<typename T>
GenericUnitTester* GuidTesterMapper::createInstance(CDynamic_Library* library, TestFilter* testFilter, const GUID* testedGuid) {
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
	logger.info(L"Getting unit tester instance..");
	try{
		return guidTesterMap[*guid](library, testFilter, guid);
	}
	catch (std::exception) {
		std::wcerr << L"No filter is matching given guid!\n";
		logger.error(L"No filter is matching given guid!");
		return nullptr;
	}
}