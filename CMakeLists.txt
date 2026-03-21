
cmake_minimum_required(VERSION 3.23)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(sample LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/core)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE core)
target_compile_definitions(app PRIVATE
    $<$<CONFIG:Debug>:MY_DEBUG>
)

include(CTest)
enable_testing()
find_package(GTest CONFIG REQUIRED)
add_executable(tests tests/test_main.cpp)
target_compile_definitions(tests PRIVATE
    $<$<CONFIG:Debug>:MY_DEBUG>
)
 
target_link_libraries(tests
  PRIVATE
    GTest::gtest
    GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(tests)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  file(GLOB_RECURSE ALL_CXX_SOURCE_FILES src/*.cpp tests/*.cpp)
  add_custom_target(format COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_CXX_SOURCE_FILES})
endif()
