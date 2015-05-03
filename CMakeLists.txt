cmake_minimum_required(VERSION 3.2)
project(data_download)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/root/cmake)

find_package(ROOT REQUIRED)

link_directories(${ROOT_LIBRARY_DIR})
include_directories(BEFORE ${CORE_INC_DIR} ${ROOT_INCLUDE_DIR})

set(SOURCE_FILES
    src/cern_get.cpp)

add_executable(cern_get ${SOURCE_FILES})
target_link_libraries(cern_get ${ROOT_LIBRARIES})