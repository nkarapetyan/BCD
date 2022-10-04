set(CMAKE_BUILD_TYPE Debug)
enable_testing()

set(TESTS_NAME bcd_tests)

set(TESTS_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/test_bcd.cpp
)

link_directories(${UTILS_INSTALL_DIR}/lib)

add_executable(${TESTS_NAME} ${TESTS_SRC})

target_link_libraries(${TESTS_NAME}
	BCDLib
	${ORTools_LIB}
  ${OpenCV_LIBS}
	${GTEST_BOTH_LIBRARIES}
	pthread
	${Boost_LIBRARIES}
	${QT_LIBRARIES}
	Qt5::Core
	Qt5::Gui
	Qt5::Concurrent
	Qt5::OpenGL
	Qt5::Test
	Qt5::Widgets
)

add_test(NAME ${TESTS_NAME} COMMAND ${TESTS_NAME})

INSTALL (TARGETS ${TESTS_NAME} DESTINATION ${BCD_INSTALL_DIR}/tests)
