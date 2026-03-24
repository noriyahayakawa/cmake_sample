cmake_minimum_required(VERSION 3.23)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(sample_core_tests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE TEST_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
add_executable(tests ${TEST_SOURCE_FILES})
target_link_libraries(tests
  PRIVATE
    GTest::gtest
    GTest::gtest_main
    core
)
target_compile_options(tests
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:MSVC>,$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:/EHsc>
    $<$<OR:$<CXX_COMPILER_ID:MSVC>,$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:/utf-8>
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:-finput-charset=UTF-8>
)
target_compile_definitions(tests PRIVATE
    $<$<CONFIG:Debug>:MY_DEBUG>
)
 
include(GoogleTest)
gtest_discover_tests(tests)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  # tests 配下のテストコードを clang-format で整形するターゲット
  file(GLOB_RECURSE TEST_CXX_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
  add_custom_target(tests-format COMMAND ${CLANG_FORMAT_EXE} -i ${TEST_CXX_SOURCE_FILES})
endif()
