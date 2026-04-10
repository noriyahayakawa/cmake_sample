
cmake_minimum_required(VERSION 3.23)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(sample_core LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
add_library(core src/core.cpp)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(core PRIVATE /utf-8)
else()
  target_compile_options(core PRIVATE -finput-charset=UTF-8)
endif()
target_include_directories(core
  PUBLIC
    ${CORE_PUBLIC_INCLUDE_DIRS}
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${CORE_PRIVATE_INCLUDE_DIRS}
)
target_compile_definitions(core PRIVATE
    $<$<CONFIG:Debug>:MY_DEBUG>
)

find_package(Boost CONFIG REQUIRED COMPONENTS headers program_options filesystem json asio log log_setup)
target_link_libraries(core
  PUBLIC
    Boost::headers
    Boost::filesystem
    Boost::json
    Boost::program_options
    Boost::log
    Boost::log_setup
    Boost::asio
)

# --- DLL export/import マクロの自動生成 ---
include(GenerateExportHeader)  # CMake組み込みモジュール[1](https://cmake.org/cmake/help/latest/module/GenerateExportHeader.html)

# 生成ファイル名を固定（ソース側で include しやすい）
generate_export_header(core
  BASE_NAME CORE
  EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/core_export.hpp
)
# 生成された core_export.h を利用側も include できるようにする
target_include_directories(core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

install(TARGETS core
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/core_export.hpp
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  # core ライブラリ配下のコードを clang-format で整形するターゲット
  file(GLOB_RECURSE CORE_CXX_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
  add_custom_target(core-format COMMAND ${CLANG_FORMAT_EXE} -i ${CORE_CXX_SOURCE_FILES})
endif()
