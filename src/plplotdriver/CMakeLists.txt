#conditionally compile a 'local' version of our (patched?) favorite drivers: safer as we do not depend on plplot's vagaries.
if(USE_LOCAL_DRIVERS)
set(LINK_DIRECTORIES ${LINK_DIRECTORIES} ${wxWidgets_LIBRARY_DIRS})
foreach(WXDEF ${wxWidgets_DEFINITIONS})
    add_definitions(-D${WXDEF})
endforeach(WXDEF ${wxWidgets_DEFINITIONS})
include_directories(${PLPLOT_INCLUDE_DIR}/plplot ${PLPLOT_INCLUDE_DIR} ${wxWidgets_INCLUDE_DIRS})
add_library(wxwidgets MODULE deprecated_wxwidgets.cpp deprecated_wxwidgets_app.cpp deprecated_wxwidgets_dc.cpp deprecated_wxwidgets_gc.cpp)
target_link_libraries(
	wxwidgets
	plplot
${wxWidgets_LIBRARIES}
	)
include_directories(${PLPLOT_INCLUDE_DIR}/plplot ${PLPLOT_INCLUDE_DIR} )
add_library(xwin MODULE xwin.c)
target_link_libraries(
	xwin
	plplot
	)
add_library(ps MODULE ps.c)
target_link_libraries(
	ps
	plplot
	)
add_library(svg MODULE svg.c)
target_link_libraries(
	svg
	plplot
	)

if(WIN32)
  set(DYNAMIC_SUFFIX ".dll")
else(WIN32)
  set(DYNAMIC_SUFFIX ".so")
endif(WIN32)

set_target_properties(
      svg ps xwin wxwidgets
      PROPERTIES
      PREFIX ""
      SUFFIX ${DYNAMIC_SUFFIX}
      )
 install(TARGETS svg ps xwin wxwidgets DESTINATION ${CMAKE_INSTALL_PREFIX}/${GDL_DATA_DIR}/drivers)
    install(
      FILES svg.driver_info ps.driver_info xwin.driver_info wxwidgets.driver_info
      DESTINATION ${CMAKE_INSTALL_PREFIX}/${GDL_DATA_DIR}/drivers
      )

endif(USE_LOCAL_DRIVERS)
