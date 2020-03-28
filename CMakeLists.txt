# CMakeList.txt : CMake project for SmartTester, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.0)

project ("SmartTester")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

add_library(Dynamic_Library "../smartcgms/src/common/rtl/Dynamic_Library.cpp")

# Add source to this project's executable.
add_executable (SmartTester "main.cpp")


target_link_libraries(SmartTester PUBLIC Dynamic_Library)

set_target_properties( SmartTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR} )


# TODO: Add tests and install targets if needed.