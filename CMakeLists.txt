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
        src/LogFilterUnitTester.cpp src/FileTesterMapper.h src/FileTesterMapper.cpp)

# set output directory to
SET(output_dir "${PROJECT_SOURCE_DIR}/compiled")

# globally set output directory for single-configuration environments (GCC, ..)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${output_dir}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${output_dir}")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${output_dir}")

# globally set output directory for multi-configuration environments (MSVS, ..)
FOREACH(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    SET(ORIG_OUT_CONFIG ${OUTPUTCONFIG})
    STRING(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${output_dir}/${ORIG_OUT_CONFIG}")
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${output_dir}/${ORIG_OUT_CONFIG}")
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${output_dir}/${ORIG_OUT_CONFIG}")
ENDFOREACH(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

target_link_libraries(SmartTester PUBLIC Dynamic_Library referencedImpl FilterLib scgmsLib DeviceLib string_utils)
set_target_properties( SmartTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR} )


