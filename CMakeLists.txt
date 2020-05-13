# CMakeList.txt : CMake project for SmartTester, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.0)

project ("SmartTester")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

SET(SMARTCGMS_COMMON_DIR "../smartcgms/src/common/" CACHE PATH "SmartCGMS 'common' directory location")
INCLUDE_DIRECTORIES("${SMARTCGMS_COMMON_DIR}")

add_library(Dynamic_Library "${SMARTCGMS_COMMON_DIR}/rtl/Dynamic_Library.cpp")
add_library(referencedImpl "${SMARTCGMS_COMMON_DIR}/rtl/referencedImpl.cpp")
add_library(FilterLib "${SMARTCGMS_COMMON_DIR}/rtl/FilterLib.cpp")
add_library(scgmsLib "${SMARTCGMS_COMMON_DIR}/rtl/scgmsLib.cpp")
add_library(DeviceLib "${SMARTCGMS_COMMON_DIR}/rtl/DeviceLib.cpp")
add_library(string_utils "${SMARTCGMS_COMMON_DIR}/utils/string_utils.cpp")



file(GLOB APP ${PROJECT_SOURCE_DIR}/src/app/*.cpp)
file(GLOB MAPPERS ${PROJECT_SOURCE_DIR}/src/mappers/*.cpp ${PROJECT_SOURCE_DIR}/src/mappers/*.h)
file(GLOB TESTERS ${PROJECT_SOURCE_DIR}/src/testers/*.cpp ${PROJECT_SOURCE_DIR}/src/testers/*.h)
file(GLOB UTILS ${PROJECT_SOURCE_DIR}/src/utils/*.cpp ${PROJECT_SOURCE_DIR}/src/utils/*.h)
add_executable (SmartTester ${APP} ${MAPPERS} ${TESTERS} ${UTILS})
        
target_link_libraries(SmartTester PUBLIC Dynamic_Library referencedImpl FilterLib scgmsLib DeviceLib string_utils)

IF(WIN32)
    set_target_properties( SmartTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/windows_64 )
ELSEIF(APPLE)
    set_target_properties( SmartTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/macos_64 )
ELSE()
    set_target_properties( SmartTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/debian_64 )
ENDIF()

