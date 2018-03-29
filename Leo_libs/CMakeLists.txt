### === 1. CMAKE-RELATED SETUP === ###
# Project name
PROJECT ( Leo )

# Needed so that CMAKE won't complain about it
CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

# ${PROJECT_SOURCE_DIR}

### === 2. USER-CONFIGURABLE OPTIONS === ###
# Set project compilation flags
SET ( PROJECT_NAME_COVERAGE "cpp" CACHE STRING "Name (filename header) for coverage application" )

# Specify static or shared library-building option
OPTION ( BUILD_SHARED_LIBS "Build shared libraries" ON )

### === 3. NON-CONFIGURABLE OPTIONS === ###
# Specify WinXP target
IF ( MSVC )
  ADD_DEFINITIONS ( -D_WIN32_WINNT=0x0501 -D__MSVC__ )
ENDIF ()

# Set debug mode (-g) and enable all warnings (-Wall) for GNU compilers
IF ( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
  SET ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wno-deprecated" )
  SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-deprecated" )
ENDIF ()


### === 4. SOURCE CODE DECLARATIONS === ###
SET ( SOURCES
  ${CMAKE_SOURCE_DIR}/src/Point2D.cpp
  ${CMAKE_SOURCE_DIR}/src/ReebGraph.cpp
  ${CMAKE_SOURCE_DIR}/src/RegionData.cpp
)

SET ( INCLUDE_FILES
  ${CMAKE_SOURCE_DIR}/include/Point2D.h
  ${CMAKE_SOURCE_DIR}/include/ReebGraph.h
  ${CMAKE_SOURCE_DIR}/include/RegionData.h
)

# Set other parameters
SET ( LIBRARY_NAME "Leo" )
SET ( EXECUTABLE_NAME "Leo" )
SET ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

### === 5. CHECK FOR LIBRARIES === ###

# Check and load Boost
#1
SET ( Boost_USE_MULTITHREADED OFF )
FIND_PACKAGE( Boost COMPONENTS date_time graph filesystem system thread REQUIRED )
IF ( NOT Boost_FOUND )
  MESSAGE ( STATUS "Boost library not found." )
  MESSAGE ( FATAL_ERROR "Please install libboost and the following modules: date_time, graph, filesystem, system, thread." )
ENDIF ()

#2
# Check and load OpenCV
FIND_PACKAGE ( OpenCV 2.2 REQUIRED )
IF ( NOT OpenCV_FOUND )
  MESSAGE ( STATUS "OpenCV library not found." )
  MESSAGE ( FATAL_ERROR "Please install opencv v2.2+ (strongly recommended from source)." )
ENDIF ()

#3
# Check and load Qt
#SET ( QT_MAX_VERSION "4.9.9" )
#FIND_PACKAGE ( Qt4 REQUIRED )
#IF ( NOT QT_FOUND )
#  MESSAGE ( STATUS "Qt4 library not found." )
#  MESSAGE ( FATAL_ERROR "Please install libqt4." )
#ENDIF ()
#SET ( QT_USE_QTOPENGL TRUE )
#INCLUDE( ${QT_USE_FILE} )
SET ( QT_MAX_VERSION "5.5.5" )
FIND_PACKAGE ( Qt5 COMPONENTS Core Widgets Test Concurrent OpenGL REQUIRED)
#FIND_PACKAGE ( Qt5Widgets REQUIRED )
#IF ( NOT QT_FOUND )
#  MESSAGE ( STATUS "Qt5 library not found." )
#  MESSAGE ( FATAL_ERROR "Please install libqt5." )
#ENDIF ()
SET ( QT_USE_QTOPENGL TRUE )
#INCLUDE( ${QT_USE_FILE} )


### === 6. CREATE LIBRARIES === ###
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${QT_INCLUDE_DIRS})

message(STATUS "START INCLUDE_DIRECTORIES")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "  dir='${dir}'")
endforeach()
message(STATUS "END INCLUDE_DIRECTORIES")

add_library( ${LIBRARY_NAME} SHARED ${SOURCES})
target_link_libraries(${LIBRARY_NAME} ${OpenCV_LIBS})
target_link_libraries(${LIBRARY_NAME} ${Boost_LIBRARIES})
target_link_libraries(${LIBRARY_NAME} ${QT_LIBRARIES})

install(FILES "libLeo.so" DESTINATION /usr/local/lib/Leo)
install(DIRECTORY include/ DESTINATION /usr/local/include/Leo FILES_MATCHING PATTERN "*.h")
install(FILES "LeoConfig.cmake" DESTINATION /usr/lib/cmake/Leo)
install(FILES "Leo.conf" DESTINATION /etc/ld.so.conf.d)
