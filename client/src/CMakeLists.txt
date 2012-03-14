FILE(GLOB SRC *.cpp *.h ../../common/*.cpp)
FILE(GLOB RESOURCES_RC ../*.rc)

ADD_EXECUTABLE(tux-target WIN32 ${SRC} ${RESOURCES_RC})

SOURCE_GROUP(Entity	FILES	entity.h 
							entity.cpp 
							entity_lua_proxy.cpp 
							entity_lua_proxy.h
							entity_manager.cpp
							entity_manager.h)

SOURCE_GROUP(Network FILES	../../common/custom_floating_point.cpp
							../../common/custom_floating_point.h
							login_client.cpp
							login_client.h
							net_callbacks.cpp
							net_callbacks.h
							net_message.cpp
							net_message.h
							network_task.cpp
							network_task.h)

SOURCE_GROUP(Level\\EditableElement FILES	../../common/editable_element_common.cpp
										../../common/editable_element_common.cpp
										module.cpp
										module.h
										module_common.cpp
										module_common.h
										module_lua_proxy.cpp
										module_lua_proxy.h
										particles.cpp
										particles.h
										../../common/particles_common.cpp
										../../common/particles_common.h
										particles_lua_proxy.cpp
										particles_lua_proxy.h
										start_point.cpp
										start_point.h
										../../common/start_point_common.cpp
										../../common/start_point_common.h)

SOURCE_GROUP(Level FILES		level.cpp
								level.h
								level_manager.cpp
								level_manager.h
								load_mesh.cpp
								load_mesh.h)

SOURCE_GROUP(Lua FILES		../../common/lua_nel.cpp
							../../common/lua_utility.cpp
							../../common/lua_utility.h)

SOURCE_GROUP(Tasks FILES		3d_task.cpp
								3d_task.h
								background_task.cpp
								background_task.h
								chat_task.cpp
								chat_task.h
								config_file_task.cpp
								config_file_task.h
								editor_task.cpp
								editor_task.h
								game_task.cpp
								game_task.h
								gui_task.cpp
								gui_task.h
								hud_task.cpp
								hud_task.h
								intro_task.cpp
								intro_task.h
								lens_flare_task.cpp
								lens_flare_task.h
								resource_manager.cpp
								resource_manager.h
								resource_manager2.cpp
								resource_manager2.h
								score_task.cpp
								score_task.h
								sky_task.cpp
								sky_task.h
								swap_3d_task.cpp
								swap_3d_task.h
								time_task.cpp
								time_task.h
								water_task.cpp
								water_task.h)

SOURCE_GROUP(GUI FILES		gui.h
							gui_bin.cpp
							gui_bin.h
							gui_bitmap.cpp
							gui_bitmap.h
							gui_box.cpp
							gui_box.h
							gui_button.cpp
							gui_button.h
							gui_container.cpp
							gui_container.h
							gui_custom.cpp
							gui_custom.h
							gui_event_behaviour.cpp
							gui_event_behaviour.h
							gui_frame.cpp
							gui_frame.h
							gui_listview.cpp
							gui_listview.h
							gui_mouse_listener.cpp
							gui_mouse_listener.h
							gui_multiline_text.cpp
							gui_multiline_text.h
							gui_object.cpp
							gui_object.h
							gui_progress_bar.cpp
							gui_progress_bar.h
							gui_quad.cpp
							gui_quad.h
							gui_scale.cpp
							gui_scale.h
							gui_script.cpp
							gui_script.h
							gui_scrollbar.cpp
							gui_scrollbar.h
							gui_spacer.cpp
							gui_spacer.h
							gui_spg.cpp
							gui_spg.h
							gui_stretched_quad.cpp
							gui_stretched_quad.h
							gui_text.cpp
							gui_text.h
							gui_toc.cpp
							gui_toc.h
							gui_xml.cpp
							gui_xml.h)

SOURCE_GROUP(Misc FILES		camera.cpp
							camera.h
							../../common/constant.h
							controler.cpp
							controler.h
							font_manager.cpp
							font_manager.h
							global.h
							interface.cpp
							interface.h
							interpolator.cpp
							interpolator.h
							main.cpp
							main.h
							mouse_listener.cpp
							mouse_listener.h
							mtp_target.cpp
							mtp_target.h
							../../common/pause_flag.h
							resource.h
							../../common/singleton.h
							sound_manager.cpp
							sound_manager.h
							stdpch.cpp
							stdpch.h
							task.h
							task_manager.cpp
							task_manager.h)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}
		${LIBXML2_INCLUDE_DIR}
		${NEL_INCLUDE_DIR}
		${LUA_INCLUDE_DIR}
		${CURL_INCLUDE_DIR}
		${PNG_INCLUDE_DIR}
		${CMAKE_HOME_DIRECTORY}/common/
		)

IF(WIN32)
	TARGET_LINK_LIBRARIES(tux-target	optimized ${NELMISC_LIBRARY}	debug ${NELMISC_LIBRARY_DEBUG} 
									optimized ${NELNET_LIBRARY}		debug ${NELNET_LIBRARY_DEBUG} 
									optimized ${NEL3D_LIBRARY}		debug ${NEL3D_LIBRARY_DEBUG}
									${NELSOUND_LIBRARY}
									${NELSNDDRV_LIBRARY}
									${PLATFORM_LINKFLAGS}
									${LUA_LIBRARIES}
									${CURL_LIBRARY}
									${SSLEAY_LIBRARY}
									${EAY_LIBRARY}
									${LIBXML2_LIBRARIES}
									${PNG_LIBRARIES}
									${FREETYPE_LIBRARY}
									${JPEG_LIBRARY})

 	SET_TARGET_PROPERTIES(tux-target PROPERTIES 
		PROJECT_LABEL "Client: Tux Target"
		LINK_FLAGS_DEBUG "/NODEFAULTLIB:libc;libcmt;libcmtd;msvcrt"
		LINK_FLAGS_RELEASE "/NODEFAULTLIB:libc;libcmt;libcmtd;msvcrtd")

	ADD_DEFINITIONS(-DCURL_STATICLIB)
ELSE(WIN32)
	TARGET_LINK_LIBRARIES(tux-target	${NELMISC_LIBRARY} 
										${NELNET_LIBRARY} 
										${NEL3D_LIBRARY}
										${NELSOUND_LIBRARY}
										${NELSNDDRV_LIBRARY}
										${PLATFORM_LINKFLAGS}
										${LUA_LIBRARIES}
										${CURL_LIBRARY}
										${LIBXML2_LIBRARIES})
ENDIF(WIN32)

INSTALL(TARGETS tux-target RUNTIME DESTINATION games)
