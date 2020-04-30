#pragma once

#include "GuidFileMapper.h"
#include "constants.h"

GuidFileMapper::GuidFileMapper() {

	guidFileMap.insert(std::pair<GUID, const wchar_t*>(LOG_GUID, LOG_LIBRARY));
	
}

/**
	Returns instence of GuidFileMapper.
*/
GuidFileMapper& GuidFileMapper::GetInstance() {
	static GuidFileMapper instance;
	return instance;
}

/**
	Returns file name of dynamic library asociated with given GUID.
*/
const wchar_t* GuidFileMapper::getFileName(GUID& guid) {
	return guidFileMap[guid];
}