cmake_minimum_required(VERSION 3.12)

project(socket)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_GENERATOR_PLATFORM x64)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CommonTools)

add_subdirectory(xsocket)
add_subdirectory(xtcp)