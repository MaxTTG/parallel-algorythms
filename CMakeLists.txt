cmake_minimum_required(VERSION 3.5)

project(pqsort LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core REQUIRED)

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
  src/seq/qsort.hpp
  src/gen.hpp
)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE ${SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core)
