
#include "../GuidFileMapper.h"
#include "../../utils/constants.h"

GuidFileMapper::GuidFileMapper() {
    m_guidFileMap[cnst::LOG_GUID] = cnst::LOG_LIBRARY;
    m_guidFileMap[cnst::DRAWING_GUID] = cnst::DRAWING_LIBRARY;
    m_guidFileMap[cnst::MAPPING_GUID] = cnst::SIGNAL_LIBRARY;
    m_guidFileMap[cnst::MASKING_GUID] = cnst::SIGNAL_LIBRARY;
    m_guidFileMap[cnst::LOG_REPLAY_GUID] = cnst::LOG_LIBRARY;
    m_guidFileMap[cnst::SIGNAL_GEN_GUID] = cnst::SIGNAL_LIBRARY;
}

GuidFileMapper& GuidFileMapper::GetInstance() {
	static GuidFileMapper instance;
	return instance;
}

const wchar_t* GuidFileMapper::getFileName(const GUID& guid) {
	return m_guidFileMap[guid];
}

std::map<GUID, const wchar_t*> GuidFileMapper::getMap() {
	return m_guidFileMap;
}