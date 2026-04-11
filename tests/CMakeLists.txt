cmake_minimum_required(VERSION 3.23)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(sample_core_tests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE TESTS_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE TESTS_PRIVATE_HEADER_FILES CONFIGURE_DEPENDS src/*.h src/*.hpp)

set(TESTS_PRIVATE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")
foreach(test_header IN LISTS TESTS_PRIVATE_HEADER_FILES)
  get_filename_component(test_header_dir "${test_header}" DIRECTORY)
  list(APPEND TESTS_PRIVATE_INCLUDE_DIRS "${test_header_dir}")
endforeach()
list(REMOVE_DUPLICATES TESTS_PRIVATE_INCLUDE_DIRS)

find_package(GTest CONFIG REQUIRED)
add_executable(tests ${TESTS_SOURCE_FILES})

target_include_directories(tests
  PRIVATE
    ${TESTS_PRIVATE_INCLUDE_DIRS}
)
target_link_libraries(tests
  PRIVATE
    GTest::gtest
    GTest::gtest_main
    core
)
target_compile_options(tests
  PRIVATE
    # MSVC / clang-cl: C++ 例外処理モデルを有効化する。
    # tests は core 経由で Boost.Asio ヘッダを取り込むため、boost::throw_exception が
    # 参照され、/EHsc がないとリンク時に未定義シンボルエラーになる。
    $<$<OR:$<CXX_COMPILER_ID:MSVC>,$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:/EHsc>
    # MSVC / clang-cl: ソースファイルおよび実行ファイルの文字セットを UTF-8 に統一する。
    $<$<OR:$<CXX_COMPILER_ID:MSVC>,$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:/utf-8>
    # clang-cl: /Zc:__cplusplus がないと __cplusplus が常に 199711L になる。
    # Boost.Asio は __cplusplus >= 202002 を条件に awaitable 等を有効化するため必須。
    # /std:c++NN は CMAKE_CXX_STANDARD が自動付与するので重複指定しない。
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>:/Zc:__cplusplus>
    # Linux clang: ソース文字セットを UTF-8 に設定 (/utf-8 相当)。
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
