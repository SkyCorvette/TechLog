cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(techlog)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++23" COMPILER_SUPPORTS_CXX23)

if(COMPILER_SUPPORTS_CXX23)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++23 support. Please use a different C++ compiler.")
endif()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

configure_file(
        "${PROJECT_SOURCE_DIR}/src/version.h.in"
        "${PROJECT_SOURCE_DIR}/src/version.h"
)

file(GLOB SOURCE_FILES src/*.cpp)
add_executable(techlog ${SOURCE_FILES})

include(FetchContent)

FetchContent_Declare(
        boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.88.0
)
FetchContent_MakeAvailable(boost)

FetchContent_Declare(
        pcre2
        GIT_REPOSITORY https://github.com/PCRE2Project/pcre2.git
        GIT_TAG pcre2-10.45
)
FetchContent_MakeAvailable(pcre2)

set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(Boost_INCLUDE_DIRS "${boost_SOURCE_DIR}/include")
set(Boost_LIBRARY_DIRS "${boost_BINARY_DIR}")

target_link_libraries(techlog PRIVATE Boost::filesystem Boost::program_options pcre2-8)