
#set(INSTALL_DIR /3rdparty)

# check if INSTALL_DIR has been set
# otherwise set install dirs to default

#FIXME: could have just used CMAKE_INSTALL_PREFIX that is by default /usr/local
#string(COMPARE EQUAL "${INSTALL_DIR}" "" result)
#if(result)
#	message("[NCMAKE]:The Utils package install dir is the default.")
#	set(INSTALL_DIR /usr/local)
#endif()

#set(UTILS_INSTALL_DIR ${INSTALL_DIR}/utils-lib)
set(BCD_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bcd-lib)

#set(UTILS_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include/utils-lib)
#set(UTILS_LIBRARIES ${CMAKE_INSTALL_PREFIX}/lib/utils-lib/UtilsLib.so)#?
