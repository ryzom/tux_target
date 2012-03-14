FILE(GLOB SRC *.cpp *.h ../../common/*.cpp ../../common/*.h)

ADD_EXECUTABLE(tux-target-srv WIN32 ${SRC})

INCLUDE_DIRECTORIES(
		${CMAKE_CURRENT_SOURCE_DIR}
		${LIBXML2_INCLUDE_DIR}
		${NEL_INCLUDE_DIR}
		${LUA_INCLUDE_DIR}
		${ODE_INCLUDE_DIR}
		${CMAKE_HOME_DIRECTORY}/common/
		)

IF(WIN32)
	TARGET_LINK_LIBRARIES(tux-target-srv	optimized ${NELMISC_LIBRARY}	debug ${NELMISC_LIBRARY_DEBUG} 
										optimized ${NELNET_LIBRARY}		debug ${NELNET_LIBRARY_DEBUG} 
										optimized ${NEL3D_LIBRARY}		debug ${NEL3D_LIBRARY_DEBUG}
										${PLATFORM_LINKFLAGS}
										${LUA_LIBRARIES}
										${LIBXML2_LIBRARIES}
										${ODE_LIBRARY}
										${ZLIB_LIBRARY}
										${FREETYPE_LIBRARY}
										${PNG_LIBRARY}
										${JPEG_LIBRARY})

  SET_TARGET_PROPERTIES(tux-target-srv PROPERTIES 
	PROJECT_LABEL "Server: Tux Target FS"
	LINK_FLAGS_DEBUG "/NODEFAULTLIB:libc;libcmt;libcmtd;msvcrt"
	LINK_FLAGS_RELEASE "/NODEFAULTLIB:libc;libcmt;libcmtd;msvcrtd")
ELSE(WIN32)
	TARGET_LINK_LIBRARIES(tux-target-srv	${NELMISC_LIBRARY}
										${NELNET_LIBRARY} 
										${NEL3D_LIBRARY}
										${PLATFORM_LINKFLAGS}
										${LUA_LIBRARIES}
										${LIBXML2_LIBRARIES}
										${ODE_LIBRARY}
										${ZLIB_LIBRARY})
ENDIF(WIN32)

ADD_DEFINITIONS(-DMT_SERVER -DMTPT_SERVER)

INSTALL(TARGETS tux-target-srv RUNTIME DESTINATION games)
