
cmake_minimum_required(VERSION 3.23)

if (MSVC)
  add_compile_options(/utf-8)
endif()

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(sample LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(ENABLE_AUTO_CLANG_FORMAT "Run clang-format before building targets" OFF)
include(GNUInstallDirs)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install path prefix" FORCE)
endif()

include(CTest)
enable_testing()

add_subdirectory(libs/core)

file(GLOB_RECURSE APP_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
add_executable(app ${APP_SOURCE_FILES})
target_link_libraries(app PRIVATE core)
target_include_directories(app
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(app
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:MSVC>,$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:/EHsc>
    $<$<OR:$<CXX_COMPILER_ID:MSVC>,$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:/utf-8>
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<NOT:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>:-finput-charset=UTF-8>
)
target_compile_definitions(app PRIVATE
    $<$<CONFIG:Debug>:MY_DEBUG>
)

install(TARGETS app
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  # app のソースとヘッダを clang-format で整形するターゲット
  file(GLOB_RECURSE APP_CXX_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
  add_custom_target(format-app COMMAND ${CLANG_FORMAT_EXE} -i ${APP_CXX_SOURCE_FILES})
  add_custom_target(format DEPENDS format-app)
elseif(ENABLE_AUTO_CLANG_FORMAT)
  message(WARNING "ENABLE_AUTO_CLANG_FORMAT is ON, but clang-format was not found.")
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
