#pragma once

#include "GuidFileMapper.h"
#include "../utils/constants.h"

GuidFileMapper::GuidFileMapper() {
	guidFileMap.insert(std::pair<GUID, const wchar_t*>(LOG_GUID, LOG_LIBRARY));
	guidFileMap.insert(std::pair<GUID, const wchar_t*>(DRAWING_GUID, DRAWING_LIBRARY));
	guidFileMap.insert(std::pair<GUID, const wchar_t*>(MAPPING_GUID, SIGNAL_LIBRARY));
	guidFileMap.insert(std::pair<GUID, const wchar_t*>(MASKING_GUID, SIGNAL_LIBRARY));
}

/**
	Returns instance of GuidFileMapper.
*/
GuidFileMapper& GuidFileMapper::GetInstance() {
	static GuidFileMapper instance;
	return instance;
}

/**
	Returns file name of dynamic library asociated with given GUID.
*/
const wchar_t* GuidFileMapper::getFileName(const GUID& guid) {
	return guidFileMap[guid];
}

std::map<GUID, const wchar_t*> GuidFileMapper::getMap() {
	return guidFileMap;
}