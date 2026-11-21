cmake_minimum_required(VERSION 3.24)
project(ProtNES LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(PROTNES_WARNINGS -Wall -Wextra)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM ENGINE_SOURCES "${PROJECT_SOURCE_DIR}/src/main.cpp")

add_library(protnes_engine STATIC ${ENGINE_SOURCES})
target_include_directories(protnes_engine PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_options(protnes_engine PRIVATE ${PROTNES_WARNINGS})

add_executable(protnes src/main.cpp)
target_link_libraries(protnes PRIVATE protnes_engine)
target_compile_options(protnes PRIVATE ${PROTNES_WARNINGS})

option(PROTNES_BUILD_TESTS "Build the protnes_tests suite" ON)

if(PROTNES_BUILD_TESTS)
  enable_testing()

  # Uses an installed GTest if found, otherwise downloads it
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
    URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
    FIND_PACKAGE_ARGS NAMES GTest
  )
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/tests/*.cpp")
  add_executable(protnes_tests ${TEST_SOURCES})
  target_link_libraries(protnes_tests PRIVATE protnes_engine GTest::gtest GTest::gtest_main)
  target_compile_options(protnes_tests PRIVATE ${PROTNES_WARNINGS})

  include(GoogleTest)
  gtest_discover_tests(protnes_tests)
endif()
