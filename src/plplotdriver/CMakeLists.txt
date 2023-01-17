#compile a 'local' version of our patched favorite drivers if we can: enable 3D support and fast plots.
#INSTALL_LOCAL_DRIVERS is set if option INTERACTIVE_GRAPHICS is set and plplot is able to load dynamic drivers.
if(INSTALL_LOCAL_DRIVERS)
  include_directories(${PLPLOT_INCLUDE_DIR}/plplot ${PLPLOT_INCLUDE_DIR} )

  #these drivers are always present
  list(APPEND WHAT "ps" "svg" "mem")
  list(APPEND WHATFILES "mem.driver_info" "svg.driver_info" "ps.driver_info")
  add_library(ps MODULE ps.c)
  target_link_libraries(
      ps
      ${PLPLOT_LIBRARIES}
      )
  add_library(svg MODULE svg.c)
  target_link_libraries(
      svg
      ${PLPLOT_LIBRARIES}
      )
  add_library(mem MODULE mem.c)
  target_link_libraries(
      mem
      ${PLPLOT_LIBRARIES}
      )

  #driver wxwidgets
  if (HAVE_LIBWXWIDGETS) 
    set(LINK_DIRECTORIES ${LINK_DIRECTORIES} ${wxWidgets_LIBRARY_DIRS})
    foreach(WXDEF ${wxWidgets_DEFINITIONS})
        add_definitions(-D${WXDEF})
    endforeach(WXDEF ${wxWidgets_DEFINITIONS})
    include_directories( ${wxWidgets_INCLUDE_DIRS})
    add_library(wxwidgets MODULE deprecated_wxwidgets.cpp deprecated_wxwidgets_app.cpp deprecated_wxwidgets_dc.cpp deprecated_wxwidgets_gc.cpp)
    target_link_libraries(
        wxwidgets
      ${PLPLOT_LIBRARIES}
        ${wxWidgets_LIBRARIES}
     )
     list(APPEND WHAT "wxwidgets")
     list(APPEND WHATFILES  "wxwidgets.driver_info")
   endif(HAVE_LIBWXWIDGETS)

 
  # driver xwin -- check if X11 is present
  find_package(X11 QUIET)
  set(HAVE_X ${X11_FOUND})
  if(X11_FOUND)
    set(LIBRARIES ${LIBRARIES} ${X11_LIBRARIES})
    include_directories(${X11_INCLUDE_DIR})
    add_library(xwin MODULE xwin.c)
    target_link_libraries(
        xwin
      ${PLPLOT_LIBRARIES}
        ${X11_LIBRARIES}
        )
    list (APPEND WHAT "xwin")
    list(APPEND WHATFILES "wxin.driver_info")
 endif(X11_FOUND)



  if(WIN32)
    set(DYNAMIC_SUFFIX ".dll")
  else(WIN32)
    set(DYNAMIC_SUFFIX ".so")
  endif(WIN32)

  foreach(v IN LISTS WHAT)
  set_target_properties(${v} PROPERTIES   PREFIX ""  SUFFIX ${DYNAMIC_SUFFIX} )
  install(TARGETS ${v} DESTINATION ${CMAKE_INSTALL_PREFIX}/${GDL_LIB_DIR}/drivers)
  install( FILES ${v}.driver_info DESTINATION ${CMAKE_INSTALL_PREFIX}/${GDL_LIB_DIR}/drivers)
  endforeach()

endif(INSTALL_LOCAL_DRIVERS)
