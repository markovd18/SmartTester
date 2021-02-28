#include <rtl/guid.h>
#include "../GuidTesterMapper.h"
#include "../../utils/constants.h"
#include "../../testers/LogFilterUnitTester.h"
#include "../../testers/DrawingFilterUnitTester.h"
#include "../../testers/MappingFilterUnitTester.h"
#include "../../testers/MaskingFilterUnitTester.h"

GuidTesterMapper::GuidTesterMapper() {
	m_guidTesterMap[cnst::LOG_GUID] = new tester::LogFilterUnitTester(cnst::LOG_GUID);
	m_guidTesterMap[cnst::DRAWING_GUID] = new tester::DrawingFilterUnitTester(cnst::DRAWING_GUID);
	m_guidTesterMap[cnst::MAPPING_GUID] = new tester::MappingFilterUnitTester(cnst::MAPPING_GUID);
	m_guidTesterMap[cnst::MASKING_GUID] = new tester::MaskingFilterUnitTester(cnst::MASKING_GUID);
}

GuidTesterMapper& GuidTesterMapper::GetInstance() {
	static GuidTesterMapper instance;
	return instance;
}

tester::GenericUnitTester* GuidTesterMapper::getTesterInstance(const GUID& guid) {
	logger.info(L"Getting unit tester instance..");
	try{
		return m_guidTesterMap[guid];
	}
	catch (const std::exception&) {
		std::wcerr << L"No filter is matching given guid!\n";
		logger.error(L"No filter is matching given guid!");
		return nullptr;
	}
}

GuidTesterMapper::~GuidTesterMapper() {
    for (const auto &guidTesterPair : m_guidTesterMap) {
        delete guidTesterPair.second;
    }
}