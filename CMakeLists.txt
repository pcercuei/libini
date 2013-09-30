cmake_minimum_required(VERSION 2.8.8)
project(libini C)

set(VERSION "0.1")

set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" 
	CACHE PATH "Installation directory for libraries")
set(INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include"
	CACHE PATH "Installation directory for headers")
set(INSTALL_PKGCONFIG_DIR "${INSTALL_LIB_DIR}/pkgconfig"
	CACHE PATH "Installation directory for pkgconfig (.pc) files")

set(LIBINI_PC ${CMAKE_CURRENT_BINARY_DIR}/libini.pc)
configure_file(libini.pc.cmakein ${LIBINI_PC} @ONLY)
install(FILES ${LIBINI_PC} DESTINATION "${INSTALL_PKGCONFIG_DIR}")

set(SETUP_PY ${CMAKE_CURRENT_SOURCE_DIR}/python/setup.py)
configure_file(python/setup.py.cmakein ${SETUP_PY} @ONLY)

set(LIBINI_CFILES libini.c)
set(LIBINI_HEADERS ini.h)

add_library(ini SHARED ${LIBINI_CFILES} ${LIBINI_HEADERS})
add_library(inistatic STATIC ${LIBINI_CFILES} ${LIBINI_HEADERS})
set_target_properties(ini inistatic PROPERTIES OUTPUT_NAME ini)
set_target_properties(ini PROPERTIES VERSION ${VERSION})

if (NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL)
	install(FILES ${LIBINI_HEADERS} DESTINATION "${INSTALL_INC_DIR}")
endif()

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
    install(TARGETS ini inistatic
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}")
endif()
