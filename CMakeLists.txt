cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(union_find)

# Configure compile mode: Debug, Release, etc.
# Alternatively, users can manually select the build type using `cmake -D CMAKE_BUILD_TYPE=Debug ..`
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable C++11 standard
# Check if the compiler supports C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    message(STATUS "Using flag -std=c++11.")
    set(CMAKE_CXX_STANDARD 11)
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Enable optimization 
# Check if the compiler supports this intruction 
# Pass flags using modern CMake, cf. https://stackoverflow.com/a/23995391/11240780
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE) 
    set(CMAKE_CXX_FLAGS_RELEASE "{CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")
endif()

# Configure compile output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)

# enable testing
enable_testing()
# googletest.
find_package(GTest CONFIG REQUIRED)

###############################################################################
# backtracking.

add_executable(0733_floodfill 0733_floodfill.cc)
target_link_libraries(0733_floodfill
  # ${OpenCV_LIBRARIES}
  ${GTest_LIBRARIES}
  GTest::gtest
  GTest::gtest_main
)
add_test(AllTestsIn0733_floodfill 0733_floodfill)