
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
    /**
     * Returns the instance of GuidFileMapper.
     *
     * @return the instance of GuidFileMapper
     */
	static GuidFileMapper& GetInstance();
    /**
     * Returns the file name of shared library associated with given GUID.
     *
     * @param guid filter GUID
     * @return name of shared library containing implementation of filter with given GUID
     */
	const wchar_t* getFileName(const GUID& guid);
    /**
     * Returns the map with mapped information. The map takes GUID as a key. If given key exists in this map,
     * it returns mapped const wchar_t* as value.
     *
     * @return map with mapped information
     */
    std::map<GUID, const wchar_t*> getMap();
	GuidFileMapper(GuidFileMapper const&) = delete;
	void operator=(GuidFileMapper const&) = delete;

};
#endif //_GUID_FILE_MAPPER_H_
