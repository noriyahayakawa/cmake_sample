
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
target_include_directories(app
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
) 
target_compile_definitions(app PRIVATE
    $<$<CONFIG:Debug>:MY_DEBUG>
)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  # app のソースとヘッダを clang-format で整形するターゲット
  file(GLOB_RECURSE APP_CXX_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
  add_custom_target(format COMMAND ${CLANG_FORMAT_EXE} -i ${APP_CXX_SOURCE_FILES})
endif()

add_subdirectory(tests)
