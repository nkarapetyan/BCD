# Created by Nare Karapetyan
project(BCD)
cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_BUILD_TYPE Debug)

include(cmake/BCDConfig.cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
set(OpenCV_DIR  "/usr/local")

find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)
find_package(GTest REQUIRED)

FIND_PACKAGE( Boost COMPONENTS date_time graph filesystem system thread REQUIRED )
IF ( NOT Boost_FOUND )
  MESSAGE ( STATUS "Boost library not found." )
  MESSAGE ( FATAL_ERROR "Please install libboost and the following modules: date_time, graph, filesystem, system, thread." )
ENDIF ()

SET ( QT_MAX_VERSION "5.5.5" )
FIND_PACKAGE ( Qt5 COMPONENTS Core Gui Widgets Test Concurrent OpenGL REQUIRED)
SET ( QT_USE_QTOPENGL TRUE )


#===============================
set(LIB_NAME BCDLib)
set(LIB_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/src/bcd/BCD.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/bcd/DrawImage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/bcd/OpenImage.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/bcd_utils/Point2D.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/bcd_utils/ReebGraph.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/bcd_utils/RegionData.cpp
)
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
		${Boost_INCLUDE_DIRS}
		${Qt5Core_INCLUDE_DIRS}
		${Qt5Concurrent_INCLUDE_DIRS}
		${Qt5Gui_INCLUDE_DIRS}
		${Qt5Widgets_INCLUDE_DIRS}
		${Qt5Test_INCLUDE_DIRS}
		${Qt5OpenGL_INCLUDE_DIRS}
		${Leo_INCLUDE_DIRS}
)
#===============================

include_directories( ${INCLUDE_DIRS})
add_library(${LIB_NAME} SHARED ${LIB_SRC})
target_link_libraries(${LIB_NAME}
	${OpenCV_LIBS}
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
  Qt5::Core
  Qt5::Gui
  Qt5::Concurrent
  Qt5::OpenGL
  Qt5::Test
  Qt5::Widgets
)

#===============================

install (TARGETS ${LIB_NAME} DESTINATION ${BCD_INSTALL_DIR}/lib)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${BCD_INSTALL_DIR}/include)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ DESTINATION ${BCD_INSTALL_DIR}/cmake)

add_subdirectory(tests)
