#include <rtl/guid.h>
#include "../GuidTesterMapper.h"
#include "../../utils/constants.h"
#include "../../testers/LogFilterUnitTester.h"
#include "../../testers/DrawingFilterUnitTester.h"
#include "../../testers/MappingFilterUnitTester.h"
#include "../../testers/MaskingFilterUnitTester.h"

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
 * Creates and returns instance of class derived from GenericUnitTester based on given guid.
 * Every instance needs pointer to CDynamic_Library helper class which is used to load shared libraries of filter that
 * is tested by this UnitTester.
 * Pointer to TestFilter instance needs to be passed because it will be appended to tested filter to collect the event
 * that is used during testing.
 *
 * Instance of derived class is created but the destruction of it is done by the USER WHO NEEDS TO DESTROY IT MANUALLY
 * OR MEMORY LEAKS WILL APPEAR!
 *
 * @tparam T derived class of GenericUnitTester to create instance of
 * @param library pointer to CDynamic_Library helper class
 * @param testFilter pointer to TestFilter class to append to tested filter
 * @param testedGuid GUID of tested filter
 * @return instance of UnitTester intended for filter with given GUID
 */
template<typename T>
GenericUnitTester* GuidTesterMapper::createInstance(CDynamic_Library* library, TestFilter* testFilter, const GUID* testedGuid) {
	return new T(library, testFilter, testedGuid);
}

/**
 * Returns the instance of FileTesterMapper.
 *
 * @return instance of FileTesterMapper singleton class
 */
GuidTesterMapper& GuidTesterMapper::GetInstance() {
	static GuidTesterMapper instance;
	return instance;
}

/**
 * Returns instance of class derived from GenericUnitTester based on given guid.
 * Every instance needs pointer to CDynamic_Library helper class which is used to load shared libraries of filter that
 * is tested by this UnitTester.
 * Pointer to TestFilter instance needs to be passed because it will be appended to tested filter to collect the event
 * that is used during testing.
 *
 * Instance of derived class is created but the destruction of it is done by the USER WHO NEEDS TO DESTROY IT MANUALLY
 * OR MEMORY LEAKS WILL APPEAR!
 *
 * @param library pointer to CDynamic_library helper class
 * @param testFilter pointer to TestFilter helper class
 * @param guid guid of tested filter
 * @return instance of class derived from GenericUnitTester to test filter with given GUID
 */
GenericUnitTester* GuidTesterMapper::getTesterInstance(CDynamic_Library* library, TestFilter* testFilter, const GUID* guid) {
	logger.info(L"Getting unit tester instance..");
	try{
		return guidTesterMap[*guid](library, testFilter, guid);
	}
	catch (std::exception&) {
		std::wcerr << L"No filter is matching given guid!\n";
		logger.error(L"No filter is matching given guid!");
		return nullptr;
	}
}