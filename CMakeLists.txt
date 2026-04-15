
cmake_minimum_required(VERSION 3.23)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(cmake_sample LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_CLANG_TIDY
#   clang-tidy;
#   --extra-arg=/EHsc;
#   --extra-arg-before=--driver-mode=cl
# )
option(ENABLE_AUTO_CLANG_FORMAT "Run clang-format before building targets" OFF)

include(CTest)
enable_testing()

add_subdirectory(libs/comm)
add_subdirectory(libs/core)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE core)
target_include_directories(app
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(app
  PRIVATE
    # MSVC / clang-cl: C++ 例外処理モデルを有効化する。
    # app は core 経由で Boost.Asio ヘッダを取り込むため、boost::throw_exception が
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
target_compile_definitions(app PRIVATE
    $<$<CONFIG:Debug>:MY_DEBUG>
)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  # app のソースとヘッダを clang-format で整形するターゲット
  file(GLOB_RECURSE APP_CXX_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
  add_custom_target(format-app COMMAND ${CLANG_FORMAT_EXE} -i ${APP_CXX_SOURCE_FILES})
  add_custom_target(format DEPENDS format-app)
elseif(ENABLE_AUTO_CLANG_FORMAT)
  message(WARNING "ENABLE_AUTO_CLANG_FORMAT は ON ですが、clang-format が見つかりませんでした。自動整形は無効になります。")
endif()

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  add_custom_target(sync-compile-commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_BINARY_DIR}/compile_commands.json
      ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Syncing compile_commands.json to source root"
    VERBATIM
  )
endif()

add_subdirectory(tests)

if(TARGET format AND TARGET core-format)
  add_dependencies(format core-format)
endif()

if(TARGET format AND TARGET tests-format)
  add_dependencies(format tests-format)
endif()

if(ENABLE_AUTO_CLANG_FORMAT AND TARGET format)
  add_dependencies(app format)

  if(TARGET tests)
    add_dependencies(tests format)
  endif()
endif()

# app 実行ファイルをインストールする
include(GNUInstallDirs)
install(TARGETS app
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# etc 下のファイルをビルド時に app.exe と同じフォルダへコピーする
file(GLOB_RECURSE ETC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/etc/*)
foreach(_etc_file IN LISTS ETC_FILES)
  cmake_path(GET _etc_file FILENAME _etc_filename)
  add_custom_command(
    TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${_etc_file}
      $<TARGET_FILE_DIR:app>/${_etc_filename}
    COMMENT "Copying etc/${_etc_filename} to build directory"
  )
endforeach()

# etc 下のファイルをインストール先の etc へコピーする
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/etc/
  DESTINATION etc
)

# Doxygen で生成されたドキュメントが存在する場合はインストールに含める
install(CODE "
  if(EXISTS \"${CMAKE_CURRENT_SOURCE_DIR}/out/docs\")
    file(INSTALL \"${CMAKE_CURRENT_SOURCE_DIR}/out/docs/\"
      DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}\"
    )
  endif()
")
