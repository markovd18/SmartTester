#include <rtl/guid.h>
#include "../GuidTesterMapper.h"
#include "../../utils/constants.h"
#include "../../testers/LogFilterUnitTester.h"
#include "../../testers/DrawingFilterUnitTester.h"
#include "../../testers/MappingFilterUnitTester.h"
#include "../../testers/MaskingFilterUnitTester.h"

/**
 * Factory method for creating instances of classes derived from GenericUnitTester. Caller TAKES OWNERSHIP
 * of the returned instance pointer and has to delete it manually.
 * @tparam T derived UnitTester class to instantiate
 * @return pointer to the created tester instance
 */
template<typename T>
tester::FilterUnitTester* createTester() {
    return new T();
}

GuidTesterMapper::GuidTesterMapper() {
	m_guidTesterMap[cnst::LOG_GUID] = &createTester<tester::LogFilterUnitTester>;
	m_guidTesterMap[cnst::DRAWING_GUID] = &createTester<tester::DrawingFilterUnitTester>;
	m_guidTesterMap[cnst::MAPPING_GUID] = &createTester<tester::MappingFilterUnitTester>;
	m_guidTesterMap[cnst::MASKING_GUID] = &createTester<tester::MaskingFilterUnitTester>;
}

GuidTesterMapper& GuidTesterMapper::GetInstance() {
	static GuidTesterMapper instance;
	return instance;
}

tester::FilterUnitTester* GuidTesterMapper::getTesterInstance(const GUID& guid) {
	logger.info(L"Getting unit tester instance..");
	try{
		return m_guidTesterMap[guid]();
	}
	catch (const std::exception&) {
		std::wcerr << L"No filter is matching given guid!\n";
		logger.error(L"No filter is matching given guid!");
		return nullptr;
	}
}