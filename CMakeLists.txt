cmake_minimum_required(VERSION 3.5)

project(par-algos LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Concurrent REQUIRED)
find_package(OpenMP REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(
    SRCS

    src/par/qsort.hpp
    src/par/bfs.hpp

    src/seq/qsort.hpp
    src/seq/bfs.hpp

    src/gen.hpp
)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE ${SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Concurrent OpenMP::OpenMP_CXX)
