cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(techlog)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

configure_file(
        "${PROJECT_SOURCE_DIR}/src/version.h.in"
        "${PROJECT_SOURCE_DIR}/src/version.h"
)

set_target_properties(
        ${TARGET} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wctor-dtor-privacy")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winit-self")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code")

set(SOURCE_FILES src/main.cpp src/parser.cpp src/file.cpp src/standardinput.cpp src/options.cpp)
add_executable(techlog ${SOURCE_FILES})

find_package(Boost 1.64.0 COMPONENTS program_options filesystem)

if(Boost_FOUND)

    set(Boost_USE_STATIC_LIBS    ON)
    set(Boost_USE_MULTITHREADED  ON)
    set(Boost_USE_STATIC_RUNTIME OFF)

    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(techlog ${Boost_LIBRARIES} pcre2-8)

endif()

include(PVS-Studio)

pvs_studio_add_target(
        TARGET PVS-Analyze
        OUTPUT FORMAT errorfile
        ANALYZE techlog
        CONFIG "/usr/bin/PVS-Studio.cfg"
        LOG techlog.plog
)