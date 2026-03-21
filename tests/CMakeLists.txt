cmake_minimum_required(VERSION 3.23)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(sample_core_tests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()
find_package(GTest CONFIG REQUIRED)
add_executable(tests test_main.cpp)
target_compile_definitions(tests PRIVATE
    $<$<CONFIG:Debug>:MY_DEBUG>
)
 
target_link_libraries(tests
  PRIVATE
    GTest::gtest
    GTest::gtest_main
    core
)
include(GoogleTest)
gtest_discover_tests(tests)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  # tests 配下のテストコードを clang-format で整形するターゲット
  file(GLOB_RECURSE TEST_CXX_SOURCE_FILES CONFIGURE_DEPENDS *.cpp *.hpp)
  add_custom_target(tests-format COMMAND ${CLANG_FORMAT_EXE} -i ${TEST_CXX_SOURCE_FILES})
endif()
