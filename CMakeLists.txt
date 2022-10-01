# Created by Nare Karapetyan
project(CPPlanner)
cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_BUILD_TYPE Debug)

set(INSTALL_DIR /3rdparty)

#include(${CMAKE_CURRENT_SOURCE_DIR}/bcd/cmake)

set(ICVG_INSTALL_DIR ${INSTALL_DIR}/bcd-lib)

include_directories(
      ${CMAKE_CURRENT_SOURCE_DIR}/bcd/include
)

add_subdirectory(bcd)
