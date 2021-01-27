#include <rtl/guid.h>
#include "../GuidTesterMapper.h"
#include "../../utils/constants.h"
#include "../../testers/LogFilterUnitTester.h"
#include "../../testers/DrawingFilterUnitTester.h"
#include "../../testers/MappingFilterUnitTester.h"
#include "../../testers/MaskingFilterUnitTester.h"

GuidTesterMapper::GuidTesterMapper() {

	m_guidTesterMap[st::LOG_GUID] = new LogFilterUnitTester(st::LOG_GUID);
	m_guidTesterMap[st::DRAWING_GUID] = new DrawingFilterUnitTester(st::DRAWING_GUID);
	m_guidTesterMap[st::MAPPING_GUID] = new MappingFilterUnitTester(st::MAPPING_GUID);
	m_guidTesterMap[st::MASKING_GUID] = new MaskingFilterUnitTester(st::MASKING_GUID);
}

GuidTesterMapper& GuidTesterMapper::GetInstance() {
	static GuidTesterMapper instance;
	return instance;
}

GenericUnitTester* GuidTesterMapper::getTesterInstance(const GUID& guid) {
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