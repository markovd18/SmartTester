#pragma once

#include "GuidFileMapper.h"
#include "constants.h"

GuidFileMapper::GuidFileMapper() {
	GuidFileMapper::init();
}

void GuidFileMapper::init() {
	guidFileMap.insert(std::pair<GUID, const wchar_t*>(LOG_GUID, LOG_LIBRARY));
}

const wchar_t* GuidFileMapper::getFileName(GUID& guid) {
	return guidFileMap[guid];
}