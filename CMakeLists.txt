# CMakeList.txt : CMake project for SmartTester, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.0)

project ("SmartTester")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

add_library(Dynamic_Library "../smartcgms/src/common/rtl/Dynamic_Library.cpp")
add_library(referencedImpl "../smartcgms/src/common/rtl/referencedImpl.cpp")
add_library(FilterLib "../smartcgms/src/common/rtl/FilterLib.cpp")
add_library(scgmsLib "../smartcgms/src/common/rtl/scgmsLib.cpp")
add_library(DeviceLib "../smartcgms/src/common/rtl/DeviceLib.cpp")
add_library(string_utils "../smartcgms/src/common/utils/string_utils.cpp")
# Add source to this project's executable.
add_executable (SmartTester "src/main.cpp" src/TestFilter.cpp src/TestFilter.h src/UnitTestExecutor.cpp src/UnitTestExecutor.h 
        src/GuidFileMapper.cpp src/GuidFileMapper.h src/constants.h src/GenericUnitTester.h src/GenericUnitTester.cpp src/LogFilterUnitTester.h 
        src/LogFilterUnitTester.cpp src/GuidTesterMapper.h src/GuidTesterMapper.cpp src/DrawingFilterUnitTester.h src/DrawingFilterUnitTester.cpp src/Logger.h src/Logger.cpp)

target_link_libraries(SmartTester PUBLIC Dynamic_Library referencedImpl FilterLib scgmsLib DeviceLib string_utils)

IF(WIN32)
    set_target_properties( SmartTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/windows_64 )
ELSEIF(APPLE)
    set_target_properties( SmartTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/macos_64 )
ELSE()
    set_target_properties( SmartTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/debian_64 )
ENDIF()

