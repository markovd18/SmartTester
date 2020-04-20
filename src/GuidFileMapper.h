
#ifndef _GUID_FILE_MAPPER_H_
#define _GUID_FILE_MAPPER_H_

#include <map>
#include <string>
#include "../../smartcgms/src/common/rtl/guid.h"


class GuidFileMapper {
	
private:
	std::map<GUID, const wchar_t*> guidFileMap;

	void init();
public:
	GuidFileMapper();
	const wchar_t* getFileName(GUID& guid);

};
#endif _GUID_FILE_MAPPER_H_
