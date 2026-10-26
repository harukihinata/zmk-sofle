if(CONFIG_ZMK_DISPLAY AND CONFIG_ZMK_SPLIT AND NOT CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/right_status.c)
  target_include_directories(app PRIVATE ${APPLICATION_SOURCE_DIR}/include)
endif()
