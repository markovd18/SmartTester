# CMakeList.txt : CMake project for SmartTester, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.0)

project ("SmartTester")

FIND_PACKAGE(Threads REQUIRED)
SET(REQUIRED_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

SET(LINK_STDCPPFS FALSE)
# setting this macro, so compiler uses external filesystem library
# used, because of linker issues with stdc++fs on gcc and clang between SmartCGMS and SmartTester binaries
ADD_DEFINITIONS(-DSTDCPP_FS_DISABLED)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    SET(LINK_STDCPPFS TRUE)
    IF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Apple" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11.0)
		SET(LINK_STDCPPFS FALSE)
	ENDIF()
    # on GCC and Clang, we need to add libdl and and libm
	SET(REQUIRED_LIBRARIES ${REQUIRED_LIBRARIES} ${CMAKE_DL_LIBS} m)
    ADD_DEFINITIONS(-Wall -Wextra -Wno-ignored-attributes -Wno-deprecated-declarations -Wno-unknown-pragmas -Wno-unused-parameter -fPIC)
    # gcc reports a specific use case of copy semantics as deprecated, unline Clang
	IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		ADD_DEFINITIONS(-Wno-deprecated-copy -Wimplicit-fallthrough=0 -Wno-class-memaccess)
	ENDIF()
    # debug build = no optimization, add debug symbols
	SET(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_STANDARD 17)
ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_STANDARD 17)
ELSE()
    set(CMAKE_CXX_STANDARD 17)
ENDIF()
    
IF(LINK_STDCPPFS)
	SET(REQUIRED_LIBRARIES ${REQUIRED_LIBRARIES} stdc++fs)
ENDIF()

LINK_LIBRARIES(${REQUIRED_LIBRARIES})

SET(SMARTCGMS_COMMON_DIR "../smartcgms/src/common/" CACHE PATH "SmartCGMS 'common' directory location")
INCLUDE_DIRECTORIES("${SMARTCGMS_COMMON_DIR}")

add_library(scgms_common "${SMARTCGMS_COMMON_DIR}/rtl/Dynamic_Library.cpp"
        "${SMARTCGMS_COMMON_DIR}/rtl/referencedImpl.cpp"
        "${SMARTCGMS_COMMON_DIR}/rtl/FilterLib.cpp"
        "${SMARTCGMS_COMMON_DIR}/rtl/scgmsLib.cpp"
        "${SMARTCGMS_COMMON_DIR}/rtl/DeviceLib.cpp"
        "${SMARTCGMS_COMMON_DIR}/utils/string_utils.cpp"
        "${SMARTCGMS_COMMON_DIR}/utils/winapi_mapping.c"
        "${SMARTCGMS_COMMON_DIR}/rtl/hresult.c"
        "${SMARTCGMS_COMMON_DIR}/rtl/FilesystemLib.cpp"
        "${SMARTCGMS_COMMON_DIR}/iface/DeviceIface.cpp")


file(GLOB APP ${PROJECT_SOURCE_DIR}/src/app/*.cpp)
file(GLOB MAPPERS ${PROJECT_SOURCE_DIR}/src/mappers/impl/*.cpp ${PROJECT_SOURCE_DIR}/src/mappers/*.h)
file(GLOB TESTERS ${PROJECT_SOURCE_DIR}/src/testers/impl/*.cpp ${PROJECT_SOURCE_DIR}/src/testers/*.h)
file(GLOB UTILS ${PROJECT_SOURCE_DIR}/src/utils/impl/*.cpp ${PROJECT_SOURCE_DIR}/src/utils/*.h)
add_executable (SmartTester ${APP} ${MAPPERS} ${TESTERS} ${UTILS})
        
target_link_libraries(SmartTester PUBLIC scgms_common)

IF(WIN32)
    set_target_properties( SmartTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/windows_64/x86_64 )
ELSEIF(APPLE)
    set_target_properties( SmartTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/macos_64/x86_64 )
ELSE()
    set_target_properties( SmartTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/debian_64/x86_64 )
ENDIF()

