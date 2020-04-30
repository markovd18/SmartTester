
#ifndef _GUID_FILE_MAPPER_H_
#define _GUID_FILE_MAPPER_H_

#include <map>
#include <string>
#include "../../smartcgms/src/common/rtl/guid.h"

/**
	Singleton class used for mapping GUID to propper dynamic library file name.
*/
class GuidFileMapper {
	
private:
	std::map<GUID, const wchar_t*> guidFileMap;
	GuidFileMapper();

public:
	static GuidFileMapper& GetInstance();
	const wchar_t* getFileName(GUID& guid);

	GuidFileMapper(GuidFileMapper const&) = delete;
	void operator=(GuidFileMapper const&) = delete;

};
#endif _GUID_FILE_MAPPER_H_
