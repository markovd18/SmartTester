
#ifndef _GUID_FILE_MAPPER_H_
#define _GUID_FILE_MAPPER_H_

#include <map>
#include <rtl/guid.h>

/**
	Singleton class used for mapping GUID to proper dynamic library file name.
    GuidFileMapper provides method to retrieve the map which you can pass GUID of filter to get the name of shared library
    that contains the implementation of that filter.
*/
class GuidFileMapper {
	
private:
	std::map<GUID, const wchar_t*> m_guidFileMap;
	GuidFileMapper();

public:
	static GuidFileMapper& GetInstance();
	const wchar_t* getFileName(const GUID& guid);
	std::map<GUID, const wchar_t*> getMap();
	GuidFileMapper(GuidFileMapper const&) = delete;
	void operator=(GuidFileMapper const&) = delete;

};
#endif //_GUID_FILE_MAPPER_H_
