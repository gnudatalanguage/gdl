if(DO_MAKE_WXWIDGETS_DRIVER)
set(LINK_DIRECTORIES ${LINK_DIRECTORIES} ${wxWidgets_LIBRARY_DIRS})
foreach(WXDEF ${wxWidgets_DEFINITIONS})
    add_definitions(-D${WXDEF})
endforeach(WXDEF ${wxWidgets_DEFINITIONS})
include_directories(${PLPLOT_INCLUDE_DIR}/plplot ${PLPLOT_INCLUDE_DIR} ${wxWidgets_INCLUDE_DIRS})
#add_library(wxwidgets MODULE wxwidgets.cpp wxwidgets_comms.cpp wxwidgets_dev.cpp)
add_library(wxwidgets MODULE deprecated_wxwidgets.cpp deprecated_wxwidgets_app.cpp deprecated_wxwidgets_dc.cpp deprecated_wxwidgets_gc.cpp)
target_link_libraries(
	wxwidgets
	plplot
	${MATH_LIB}
	${RT_LIB}
	${${SOURCE_ROOT_NAME}_LINK_FLAGS}
	${${SOURCE_ROOT_NAME}_TARGETS}
	)
endif(DO_MAKE_WXWIDGETS_DRIVER)
if(DO_MAKE_XWIN_DRIVER)
include_directories(${PLPLOT_INCLUDE_DIR}/plplot ${PLPLOT_INCLUDE_DIR} )
add_library(xwin MODULE xwin.c)
target_link_libraries(
	xwin
	plplot
	${MATH_LIB}
	${RT_LIB}
	${${SOURCE_ROOT_NAME}_LINK_FLAGS}
	${${SOURCE_ROOT_NAME}_TARGETS}
	)
endif(DO_MAKE_XWIN_DRIVER)
if(DO_MAKE_WINGCC_DRIVER)
include_directories(${PLPLOT_INCLUDE_DIR}/plplot ${PLPLOT_INCLUDE_DIR} )
add_library(wingcc MODULE wingcc.c)
target_link_libraries(
	wingcc
	plplot
	${MATH_LIB}
	${RT_LIB}
	${${SOURCE_ROOT_NAME}_LINK_FLAGS}
	${${SOURCE_ROOT_NAME}_TARGETS}
	)
endif(DO_MAKE_WINGCC_DRIVER)
