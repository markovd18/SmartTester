//
// Author: markovd@students.zcu.cz
//

#pragma once
#ifndef _FILE_TESTER_MAPPER_H_
#define _FILE_TESTER_MAPPER_H_

#include <map>
#include <functional>
#include <rtl/guid.h>
#include "../testers/GenericUnitTester.h"
#include "../utils/Logger.h"

/**
 * Singleton class used to map GUID to appropriate instance of derived GenericUnitTester class.
 * GuidTesterMapper provides method to retrieve this instance but it requires CDynamic_Library helper class pointer
 * that will be used to load shared libraries of that filter, TestFilter class pointer that will be appended to tested filter
 * to collect the event passed to filter during testing and GUID of wanted filter.
 */
class GuidTesterMapper {

private:
    /// Mapped derived class instance pointers to their representing GUID
	std::map<GUID, GenericUnitTester*> m_guidTesterMap;
    /// Private constructor because of this class being a singleton
	GuidTesterMapper();
    Logger& logger = Logger::getInstance();
public:
    /**
    * Returns the instance of FileTesterMapper.
    *
    * @return instance of FileTesterMapper singleton class
    */
	static GuidTesterMapper& GetInstance();
	/**
	 * Returns a pointer to a tester instance based on given guid.
	 *
	 * @param guid guid of a filter, that we want to test
	 * @return non-owning pointer to a tester instance
	 */
	GenericUnitTester* getTesterInstance(const GUID& guid);
	GuidTesterMapper(GuidTesterMapper const&) = delete;
	void operator=(GuidTesterMapper const&) = delete;
	/// Since we are storing pointers to testers, we need to delete them in destructor
	~GuidTesterMapper();

};
#endif // !_FILE_TESTER_MAPPER_H_

