cmake_minimum_required(VERSION 3.20)
project(ProtNES LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-Wall -Wextra)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM ENGINE_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_library(protnes_engine STATIC ${ENGINE_SOURCES})
target_include_directories(protnes_engine PUBLIC "${CMAKE_SOURCE_DIR}/include")

add_executable(protnes src/main.cpp)
target_link_libraries(protnes PRIVATE protnes_engine)

option(PROTNES_BUILD_TESTS "Build the protnes_tests suite" ON)

if(PROTNES_BUILD_TESTS)
  enable_testing()
  find_package(GTest REQUIRED)

  file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/*.cpp")
  add_executable(protnes_tests ${TEST_SOURCES})
  target_link_libraries(protnes_tests PRIVATE protnes_engine GTest::gtest GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(protnes_tests)
endif()
