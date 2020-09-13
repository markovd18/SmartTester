#pragma once

#include "../GuidFileMapper.h"
#include "../../utils/constants.h"

GuidFileMapper::GuidFileMapper() {
	guidFileMap.insert(std::pair<GUID, const wchar_t*>(LOG_GUID, LOG_LIBRARY));
	guidFileMap.insert(std::pair<GUID, const wchar_t*>(DRAWING_GUID, DRAWING_LIBRARY));
	guidFileMap.insert(std::pair<GUID, const wchar_t*>(MAPPING_GUID, SIGNAL_LIBRARY));
	guidFileMap.insert(std::pair<GUID, const wchar_t*>(MASKING_GUID, SIGNAL_LIBRARY));
}

/**
 * Returns the instance of GuidFileMapper.
 *
 * @return the instance of GuidFileMapper
 */
GuidFileMapper& GuidFileMapper::GetInstance() {
	static GuidFileMapper instance;
	return instance;
}

/**
 * Returns the file name of shared library associated with given GUID.
 *
 * @param guid filter GUID
 * @return name of shared library containing implementation of filter with given GUID
 */
const wchar_t* GuidFileMapper::getFileName(const GUID& guid) {
	return guidFileMap[guid];
}

/**
 * Returns the map with mapped information. The map takes GUID as a key. If given key exists in this map,
 * it returns mapped const wchar_t* as value.
 *
 * @return map with mapped information
 */
std::map<GUID, const wchar_t*> GuidFileMapper::getMap() {
	return guidFileMap;
}