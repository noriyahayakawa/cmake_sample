
cmake_minimum_required(VERSION 3.25)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(sample_comm LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_CLANG_TIDY
#   clang-tidy;
#   --extra-arg=/EHsc;
#   --extra-arg-before=--driver-mode=cl
# )
include(GNUInstallDirs)

file(GLOB_RECURSE COMM_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE COMM_PUBLIC_HEADER_FILES CONFIGURE_DEPENDS include/*.h include/*.hpp)
file(GLOB_RECURSE COMM_PRIVATE_HEADER_FILES CONFIGURE_DEPENDS src/*.h src/*.hpp)

set(COMM_PUBLIC_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
foreach(comm_header IN LISTS COMM_PUBLIC_HEADER_FILES)
  get_filename_component(comm_header_dir "${comm_header}" DIRECTORY)
  list(APPEND COMM_PUBLIC_INCLUDE_DIRS "${comm_header_dir}")
endforeach()
list(REMOVE_DUPLICATES COMM_PUBLIC_INCLUDE_DIRS)

set(COMM_PRIVATE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")
foreach(comm_header IN LISTS COMM_PRIVATE_HEADER_FILES)
  get_filename_component(comm_header_dir "${comm_header}" DIRECTORY)
  list(APPEND COMM_PRIVATE_INCLUDE_DIRS "${comm_header_dir}")
endforeach()
list(REMOVE_DUPLICATES COMM_PRIVATE_INCLUDE_DIRS)

add_library(comm ${COMM_SOURCE_FILES})
target_compile_options(comm
  PRIVATE
    # MSVC / clang-cl: C++ 例外処理モデルを有効化する。
    # comm は core 経由で Boost.Asio ヘッダを取り込むため、boost::throw_exception が
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
target_include_directories(comm
  PUBLIC
    ${COMM_PUBLIC_INCLUDE_DIRS}
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${COMM_PRIVATE_INCLUDE_DIRS}
)
target_compile_definitions(comm PRIVATE
    $<$<CONFIG:Debug>:MY_DEBUG>
)

find_package(Boost CONFIG REQUIRED COMPONENTS headers system filesystem json asio thread log)
target_link_libraries(comm
  PUBLIC
    Boost::headers
    Boost::system
    Boost::json
    Boost::filesystem
    Boost::thread
    Boost::log
    Boost::asio
)

# --- DLL export/import マクロの自動生成 ---
include(GenerateExportHeader)  # CMake組み込みモジュール[1](https://cmake.org/cmake/help/latest/module/GenerateExportHeader.html)

# 生成ファイル名を固定（ソース側で include しやすい）
generate_export_header(comm
  BASE_NAME COMM
  EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/comm_export.hpp
)
# 生成された comm_export.h を利用側も include できるようにする
target_include_directories(comm
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

install(TARGETS comm
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/comm_export.hpp
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  # comm ライブラリ配下のコードを clang-format で整形するターゲット
  file(GLOB_RECURSE COMM_CXX_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
  add_custom_target(comm-format COMMAND ${CLANG_FORMAT_EXE} -i ${COMM_CXX_SOURCE_FILES})
endif()
