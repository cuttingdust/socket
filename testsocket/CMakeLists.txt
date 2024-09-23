cmake_minimum_required(VERSION 3.12)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(testsocket)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(src SRC)
file(GLOB INCLUDE include/*.h)

add_executable(${PROJECT_NAME} ${INCLUDE} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE include)
