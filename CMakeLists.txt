cmake_minimum_required(VERSION 3.8)
project(techlog VERSION 1.0.1)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES src/main.cpp)
add_executable(techlog ${SOURCE_FILES})

find_package(Boost 1.64.0 COMPONENTS program_options)

if(Boost_FOUND)

    set(Boost_USE_STATIC_LIBS    ON)
    set(Boost_USE_MULTITHREADED  ON)
    set(Boost_USE_STATIC_RUNTIME OFF)

    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(techlog ${Boost_LIBRARIES})

endif()