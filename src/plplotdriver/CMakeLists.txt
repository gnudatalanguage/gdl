#compile a 'local' version of our patched favorite drivers if we can: enable 3D support and fast plots.
#INSTALL_LOCAL_DRIVERS is set if option INTERACTIVE_GRAPHICS is set and plplot is able to load dynamic drivers.
if(INSTALL_LOCAL_DRIVERS)
  include_directories(${PLPLOT_INCLUDE_DIR}/plplot ${PLPLOT_INCLUDE_DIR} )

  #these drivers are always present
  list(APPEND WHAT "ps" "svg" "mem")
  add_library(ps MODULE ps.c)
  # Copy thr driver info file after building the target so we can use the build-dir directly when testing
  add_custom_command(
    TARGET ps POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/src/plplotdriver/ps.driver_info
    ${CMAKE_CURRENT_BINARY_DIR}/ps.driver_info
  )
  target_link_libraries(ps ${PLPLOT_LIBRARIES})
  add_library(svg MODULE svg.c)
  # Copy thr driver info file after building the target so we can use the build-dir directly when testing
  add_custom_command(
    TARGET svg POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/src/plplotdriver/svg.driver_info
    ${CMAKE_CURRENT_BINARY_DIR}/svg.driver_info
  )
  target_link_libraries(svg ${PLPLOT_LIBRARIES})
  add_library(mem MODULE mem.c)
  # Copy thr driver info file after building the target so we can use the build-dir directly when testing
  add_custom_command(
    TARGET mem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/src/plplotdriver/mem.driver_info
    ${CMAKE_CURRENT_BINARY_DIR}/mem.driver_info
  )
  target_link_libraries(mem ${PLPLOT_LIBRARIES})

  #driver wxwidgets
  if (HAVE_LIBWXWIDGETS) 
    set(LINK_DIRECTORIES ${LINK_DIRECTORIES} ${wxWidgets_LIBRARY_DIRS})
    foreach(WXDEF ${wxWidgets_DEFINITIONS})
      add_definitions(-D${WXDEF})
    endforeach(WXDEF ${wxWidgets_DEFINITIONS})
    include_directories( ${wxWidgets_INCLUDE_DIRS})
    add_library(wxwidgets MODULE deprecated_wxwidgets.cpp deprecated_wxwidgets_app.cpp deprecated_wxwidgets_dc.cpp deprecated_wxwidgets_gc.cpp)
    # Copy thr driver info file after building the target so we can use the build-dir directly when testing
    add_custom_command(
      TARGET wxwidgets POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_SOURCE_DIR}/src/plplotdriver/wxwidgets.driver_info
      ${CMAKE_CURRENT_BINARY_DIR}/wxwidgets.driver_info
    )
    target_link_libraries(wxwidgets ${PLPLOT_LIBRARIES} ${wxWidgets_LIBRARIES})
    list(APPEND WHAT "wxwidgets")
  endif(HAVE_LIBWXWIDGETS)
 
  # driver xwin -- check if X11 is present
  find_package(X11 QUIET)
  set(HAVE_X ${X11_FOUND})
  if(X11_FOUND)
    set(LIBRARIES ${LIBRARIES} ${X11_LIBRARIES})
    include_directories(${X11_INCLUDE_DIR})
    add_library(xwin MODULE xwin.c)
    # Copy thr driver info file after building the target so we can use the build-dir directly when testing
    add_custom_command(
      TARGET xwin POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_SOURCE_DIR}/src/plplotdriver/xwin.driver_info
      ${CMAKE_CURRENT_BINARY_DIR}/xwin.driver_info
    )
    target_link_libraries(xwin ${PLPLOT_LIBRARIES} ${X11_LIBRARIES})
    list(APPEND WHAT "xwin")
 endif(X11_FOUND)

  if(WIN32)
    set(DYNAMIC_SUFFIX ".dll")
  else(WIN32)
    set(DYNAMIC_SUFFIX ".so")
  endif(WIN32)

  foreach(v IN LISTS WHAT)
    set_target_properties(${v} PROPERTIES   PREFIX ""  SUFFIX ${DYNAMIC_SUFFIX} )
    install(TARGETS ${v} DESTINATION ${CMAKE_INSTALL_PREFIX}/${GDL_LIB_DIR})
    install(FILES ${v}.driver_info DESTINATION ${CMAKE_INSTALL_PREFIX}/${GDL_LIB_DIR})
  endforeach()

endif(INSTALL_LOCAL_DRIVERS)
