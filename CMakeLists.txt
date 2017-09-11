cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(techlog)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)

if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Weffc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -Ofast -flto -march=native -funroll-loops -ffunction-sections -Wl,--gc-sections -fno-asynchronous-unwind-tables -Wl,--strip-all")
else()
    CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX1z)
    if(COMPILER_SUPPORTS_CXX1z)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Weffc++")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -Ofast -flto -march=native -funroll-loops -ffunction-sections -fno-asynchronous-unwind-tables")
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 or C++1z support. Please use a different C++ compiler.")
    endif()
endif()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

configure_file(
        "${PROJECT_SOURCE_DIR}/src/version.h.in"
        "${PROJECT_SOURCE_DIR}/src/version.h"
)

file(GLOB SOURCE_FILES src/*.cpp)
add_executable(techlog ${SOURCE_FILES})

set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
find_library(PCRE2_LIBRARIES pcre2-8)

find_package(Boost 1.65.1 COMPONENTS program_options filesystem)

if(Boost_FOUND)

    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(techlog ${Boost_LIBRARIES} ${PCRE2_LIBRARIES})

endif()
