
cmake_minimum_required(VERSION 3.23)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(sample_core LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(core STATIC src/core.cpp)

target_include_directories(core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
) 

target_compile_definitions(core PRIVATE
    $<$<CONFIG:Debug>:MY_DEBUG>
)

find_package(Boost CONFIG REQUIRED)
target_link_libraries(core PUBLIC Boost::boost)
