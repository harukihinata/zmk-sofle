if(CONFIG_ZMK_DISPLAY AND CONFIG_ZMK_SPLIT AND NOT CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  set(nice_view_widgets ${APPLICATION_SOURCE_DIR}/boards/shields/nice_view/widgets)
  target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/right_status.c)
  target_sources(app PRIVATE
    ${nice_view_widgets}/peripheral_status.c
    ${nice_view_widgets}/util.c
    ${nice_view_widgets}/bolt.c
    ${nice_view_widgets}/art.c)
  target_include_directories(app PRIVATE ${APPLICATION_SOURCE_DIR}/include ${nice_view_widgets})
endif()
