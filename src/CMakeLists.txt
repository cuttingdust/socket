cmake_minimum_required(VERSION 3.12)

project(socket)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_GENERATOR_PLATFORM x64)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CommonTools)


add_subdirectory(xtcp)
add_subdirectory(xhttp)
add_subdirectory(xudp)
add_subdirectory(xsyslog)

if(UNIX AND NOT APPLE OR APPLE)
	add_subdirectory(xepoll)
endif()

add_subdirectory(xsocket)

# 拷贝www目录到输出目录
set(WWW_PATH ${CMAKE_CURRENT_SOURCE_DIR}/www)

file(COPY ${WWW_PATH} DESTINATION ${OUT_RUN_PATH})
