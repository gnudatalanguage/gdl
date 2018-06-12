/***************************************************************************
                  libinit_jmg.cpp  -  initialization of GDL library routines
                             -------------------
    begin                : 2004
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// the following is already done in "includefirst.hpp"
// #ifdef HAVE_CONFIG_H
// #include <config.h>
// #else
// // default: assume we have HDF
// #define USE_HDF 1
// #define USE_FFTW 1
// #endif

#include "includefirst.hpp"

#include <string>
#include <fstream>

#include "envt.hpp"

#include "basic_fun_jmg.hpp"
#include "math_fun_jmg.hpp"
#include "basic_pro_jmg.hpp"

#include "gsl_fun.hpp"
#include "image.hpp"
#include "plotting.hpp"

#include "widget.hpp"

#if defined(USE_HDF)
#include "hdf_fun.hpp"
#include "hdf_pro.hpp"
#endif

#if defined(USE_FFTW)
#include "fftw.hpp"
#endif

#if defined(USE_EIGEN)
#include "matrix_cholesky.hpp"
#include "least_squares.hpp"
#endif

#include "matrix_invert.hpp"

#if defined(USE_LIBPROJ4)||defined(USE_LIBPROJ4_NEW)
#include "gshhs.hpp"
#endif

#include "triangulation.hpp"

using namespace std;

void LibInit_jmg()
{
  const char KLISTEND[] = "";

  // the following by Joel Gales <jomoga@users.sourceforge.net>
  const string sizeKey[]={"L64","DIMENSIONS","FILE_LUN","FILE_OFFSET",
			  "N_DIMENSIONS","N_ELEMENTS",
			  "SNAME","STRUCTURE","TNAME","TYPE",KLISTEND};
  new DLibFunRetNew(lib::size_fun,string("SIZE"),1,sizeKey);

  new DLibFunRetNew(lib::fstat_fun,string("FSTAT"),1);

  const string routine_namesKey[]={"LEVEL","VARIABLES","FETCH","ARG_NAME",
				   "STORE","S_FUNCTIONS","S_PROCEDURES",
				   KLISTEND};
  new DLibFun(lib::routine_names_value,string("ROUTINE_NAMES"),-1,routine_namesKey);
  
  // the 2 following by Alain C. and Levan L., Summer 2014
  const string isaKey[]={"ARRAY","FILE","NULL","NUMBER","SCALAR","BOOLEAN",
			 "COMPLEX","FLOAT","INTEGER","STRING",KLISTEND};
  new DLibFunRetNew(lib::isa_fun,string("ISA"),2,isaKey);

  new DLibFunRetNew(lib::typename_fun,string("TYPENAME"),1);

  // the following (Chol and Invert) by Alain C. and Nodar K., Summer 2013
  // Since we have a difference between the IDL way and the Eigen way
  // we (temporary) remove these 2 codes in the CVS of GDL
  // Help welcome. 
#if defined(USE_EIGEN)
  const string cholKey[]={"DOUBLE",KLISTEND};
  new DLibPro(lib::choldc_pro,string("CHOLDC"),2,cholKey);
  new DLibFunRetNew(lib::cholsol_fun,string("CHOLSOL"),3,cholKey);

  //
  const string lacholdcKey[]={"DOUBLE","STATUS","UPPER",KLISTEND};
  new DLibPro(lib::la_choldc_pro,string("LA_CHOLDC"),1,lacholdcKey);
  const string lacholsolKey[]={"DOUBLE","STATUS",KLISTEND};
  new DLibFunRetNew(lib::la_cholsol_fun,string("LA_CHOLSOL"),2,lacholsolKey);

  // Ilia N. & Alain C., Summer 2015
  const string laleastsquaresKey[]={"DOUBLE","METHOD","RANK","RCONDITION","RESIDUAL","STATUS",KLISTEND};
  new DLibFunRetNew(lib::la_least_squares_fun,string("LA_LEAST_SQUARES"),2,laleastsquaresKey);

#endif

#if defined(HAVE_LIBGSL) && defined(HAVE_LIBGSLCBLAS)
  
  const string invertKey[]={"DOUBLE","GSL","EIGEN",KLISTEND};
  new DLibFunRetNew(lib::AC_invert_fun,string("INVERT"),2,invertKey);

  // if FFTw not available, FFT in the GSL used (slower)
  const string fftKey[]={"DOUBLE","INVERSE","OVERWRITE","DIMENSION","CENTER",KLISTEND};
#if defined(USE_FFTW)
  new DLibFun(lib::fftw_fun,string("FFT"),2,fftKey);
#else
  new DLibFun(lib::fft_fun,string("FFT"),2,fftKey);
#endif

  const string randomKey[]={"DOUBLE","GAMMA","LONG","NORMAL",
			    "BINOMIAL","POISSON","UNIFORM","ULONG",KLISTEND};
  const string randomWarnKey[]={"RAN1", KLISTEND}; //since IDL 8.2, change of random algo (we used already the good one).
  new DLibFunRetNew(lib::random_fun,string("RANDOMU"),MAXRANK+1,randomKey,randomWarnKey);
  new DLibFunRetNew(lib::random_fun,string("RANDOMN"),MAXRANK+1,randomKey,randomWarnKey);

  const string checkmathKey[]={"MASK","NOCLEAR","PRINT",KLISTEND};
  new DLibFunRetNew(lib::check_math_fun,string("CHECK_MATH"),2,checkmathKey);

  const string histogramKey[]={"BINSIZE","INPUT","MAX","MIN","NBINS",
			       "OMAX","OMIN","REVERSE_INDICES",
			       "LOCATIONS","NAN",KLISTEND};
  const string histogramWarnKey[]={"L64",KLISTEND};
  new DLibFunRetNew(lib::histogram_fun,string("HISTOGRAM"),1,histogramKey,histogramWarnKey);

  const string interpolateKey[]={"CUBIC","DOUBLE","GRID","MISSING","NEAREST_NEIGHBOUR",KLISTEND};
  new DLibFunRetNew(lib::interpolate_fun,string("INTERPOLATE"),4,interpolateKey);

  const string la_triredKey[]={"DOUBLE","UPPER",KLISTEND};
  new DLibPro(lib::la_trired_pro,string("LA_TRIRED"),3,la_triredKey);
#endif

  const string macharKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::machar_fun,string("MACHAR"),0,macharKey);

#if defined(USE_LIBPROJ4)||defined(USE_LIBPROJ4_NEW)
  const string map_proj_forwardKey[]={"MAP_STRUCTURE","RADIANS","POLYGONS","POLYLINES","CONNECTIVITY","FILL",KLISTEND};  //WARNING FIXED ORDER for GetMapAsMapStructureKeyword()
  new DLibFunRetNew(lib::map_proj_forward_fun,
	      string("MAP_PROJ_FORWARD"),2,map_proj_forwardKey,NULL);

  const string map_proj_inverseKey[]={"MAP_STRUCTURE","RADIANS",KLISTEND}; //WARNING FIXED ORDER for GetMapAsMapStructureKeyword()
  new DLibFunRetNew(lib::map_proj_inverse_fun,
	      string("MAP_PROJ_INVERSE"),2,map_proj_inverseKey);
//dummy functions for compatibility support of GCTP projections 
  new DLibPro(lib::map_proj_gctp_forinit,string("MAP_PROJ_GCTP_FORINIT"),4);
  new DLibPro(lib::map_proj_gctp_revinit,string("MAP_PROJ_GCTP_REVINIT"),4);

  // SA: GSHHS dataset
  // TODO: USA, ORIENTATION, LIMIT, MLINESTYLE, MLINETHICK, SPACING, T3D, ZVALUE
  const string map_continentsKey[] = { "MAP_STRUCTURE", "COLOR", "RIVERS", "COUNTRIES", "COASTS", "CONTINENTS", "USA", 
    "HIRES", "FILL_CONTINENTS",KLISTEND};//WARNING FIXED ORDER for GetMapAsMapStructureKeyword()
  const string map_continentsWarnKey[] = {"ORIENTATION", "LIMIT",
         "MLINESTYLE", "MLINETHICK", "SPACING", "T3D", "ZVALUE", KLISTEND};
  new DLibPro(lib::map_continents, string("MAP_CONTINENTS"), 0, 
    map_continentsKey, map_continentsWarnKey);
#endif
  


  const string convert_coordKey[]={"DATA","DEVICE","NORMAL","T3D","DOUBLE",
				   "TO_DATA","TO_DEVICE","TO_NORMAL",KLISTEND};
  new DLibFunRetNew(lib::convert_coord,string("CONVERT_COORD"),3,convert_coordKey);


  const string finiteKey[]={"INFINITY","NAN","SIGN",KLISTEND};
  new DLibFunRetNew(lib::finite_fun,string("FINITE"),1,finiteKey);

  const string radonKey[]={"BACKPROJECT","DOUBLE","DRHO","DX","DY",
			   "GRAY","LINEAR","NRHO","NTHETA","NX","NY",
			   "RHO","RMIN","THETA","XMIN","YMIN",KLISTEND};
  new DLibFunRetNew(lib::radon_fun,string("RADON"),1,radonKey);
  

  const string grid_inputKey[]={"SPHERE", "POLAR", "DEGREES", "DUPLICATES", "EPSILON", "EXCLUDE", KLISTEND};
  new DLibPro(lib::grid_input,string("GRID_INPUT"),6,grid_inputKey);
  
  const string triangulateKey[]={"CONNECTIVITY", "SPHERE", "DEGREES", "FVALUE",KLISTEND};
  const string triangulateWarnKey[]={"REPEATS", "TOLERANCE",KLISTEND};
  new DLibPro(lib::GDL_Triangulate,string("TRIANGULATE"),4,triangulateKey,triangulateWarnKey);

//to be written and do not forget to uncomment QHULL in CMakeLists and config.h.cmake  
#ifdef HAVE_QHULL

  const string qhullKey[]={"BOUNDS", "CONNECTIVITY", "DELAUNAY", "SPHERE", "VDIAGRAM" ,"VNORMALS", "VVERTICES", KLISTEND};
  new DLibPro(lib::qhull,string("QHULL"),8,qhullKey);


  const string qgrid3Key[]={"DELTA", "DIMENSION", "MISSING", "START", KLISTEND};
  new DLibFunRetNew(lib::qgrid3_fun,string("QGRID3"),5,qgrid3Key);
#endif
  
  const string trigridKey[]={"DEGREES","NX","NY","SPHERE","XGRID","XOUT","YGRID","YOUT","MAX_VALUE","MIN_VALUE","MISSING","INPUT",KLISTEND};
  const string trigridWarnKey[]={"EXTRAPOLATE","QUINTIC",KLISTEND};
  new DLibFunRetNew(lib::trigrid_fun,string("TRIGRID"),6,trigridKey,trigridWarnKey);

  const string poly_2dKey[]={"CUBIC","MISSING",KLISTEND};
  new DLibFunRetNew(lib::poly_2d_fun,string("POLY_2D"),6,poly_2dKey);

  const string make_arrayKey[]={"DIMENSION", "INCREMENT", "INDEX", "NOZERO",
                                 "SIZE", "START", "TYPE", "VALUE", "BOOLEAN",
                                 "BYTE", "COMPLEX", "DCOMPLEX", "DOUBLE",
                                 "FLOAT", "INTEGER", "L64", "LONG", "OBJ",
                                 "PTR", "STRING", "UINT", "UL64", "ULONG", KLISTEND};
  new DLibFunRetNew(lib::make_array,string("MAKE_ARRAY"),MAXRANK,make_arrayKey);

  const string reformKey[]={"OVERWRITE",KLISTEND};
  new DLibFun(lib::reform,string("REFORM"),MAXRANK+1,reformKey);

  new DLibPro(lib::point_lun,string("POINT_LUN"),2);

  new DLibPro(lib::linkimage,string("LINKIMAGE"),4);

  new DLibPro(lib::wait_pro,string("WAIT"),1);

#if defined(USE_HDF)
  new DLibFunRetNew(lib::hdf_ishdf,string("HDF_ISHDF"),1);
  const string hdf_openKey[]={"READ","RDWR","CREATE","ALL","NUM_DD","WRITE",KLISTEND};
  new DLibFunRetNew(lib::hdf_open_fun,string("HDF_OPEN"),2,hdf_openKey);

  new DLibFunRetNew(lib::hdf_vg_getid_fun,string("HDF_VG_GETID"),2);

  const string hdf_vg_attachKey[]={"READ","WRITE",KLISTEND};
  new DLibFunRetNew(lib::hdf_vg_attach_fun,string("HDF_VG_ATTACH"),2,
	      hdf_vg_attachKey);

  const string hdf_vd_attachKey[]={"READ","WRITE",KLISTEND};
  new DLibFunRetNew(lib::hdf_vd_attach_fun,string("HDF_VD_ATTACH"),2,
	      hdf_vd_attachKey);
  new DLibFunRetNew(lib::hdf_vd_find_fun,string("HDF_VD_FIND"),2);

  const string hdf_vd_readKey[]={"FIELDS","NRECORDS","FULL_INTERLACE",
				 "NO_INTERLACE",KLISTEND};
  new DLibFunRetNew(lib::hdf_vd_read_fun,string("HDF_VD_READ"),2,hdf_vd_readKey);

  const string hdf_vg_getinfoKey[]={"CLASS","NAME","NENTRIES","REF","TAG",
				    KLISTEND};
  new DLibPro(lib::hdf_vg_getinfo_pro,string("HDF_VG_GETINFO"),1,
	      hdf_vg_getinfoKey);

  const string hdf_vd_getKey[]={"CLASS","NAME","COUNT","REF","TAG",KLISTEND};
  new DLibPro(lib::hdf_vd_get_pro,string("HDF_VD_GET"),1,
	      hdf_vd_getKey);


  new DLibPro(lib::hdf_vg_gettrs_pro,string("HDF_VG_GETTRS"),3);
  new DLibPro(lib::hdf_vg_detach_pro,string("HDF_VG_DETACH"),1);

  new DLibPro(lib::hdf_vd_detach_pro,string("HDF_VD_DETACH"),1);

  const string hdf_sd_createKey[]={"BYTE","DFNT_INT8","DFNT_UINT8",
				   "SHORT","INT","DFNT_INT16","DFNT_UINT16",
				   "LONG","DFNT_INT32","DFNT_UINT32",
				   "FLOAT","DFNT_FLOAT32",
				   "DOUBLE","DFNT_FLOAT64",
				   "STRING","DFNT_CHAR","HDF_TYPE",KLISTEND};
  new DLibFunRetNew(lib::hdf_sd_create_fun,string("HDF_SD_CREATE"),3,
	      hdf_sd_createKey);

  const string hdf_sd_startKey[]={"READ","RDWR","CREATE",KLISTEND};
  new DLibFunRetNew(lib::hdf_sd_start_fun,string("HDF_SD_START"),2,hdf_sd_startKey);
  new DLibFunRetNew(lib::hdf_sd_nametoindex_fun,string("HDF_SD_NAMETOINDEX"),2);
  new DLibFunRetNew(lib::hdf_sd_attrfind_fun,string("HDF_SD_ATTRFIND"),2);
  new DLibFunRetNew(lib::hdf_sd_select_fun,string("HDF_SD_SELECT"),2);

  const string hdf_sd_getdataKey[]={"START","STRIDE","COUNT",KLISTEND};
  new DLibPro(lib::hdf_sd_getdata_pro,string("HDF_SD_GETDATA"),2,
	      hdf_sd_getdataKey);
  const string hdf_sd_adddataKey[]={"START","STRIDE","COUNT",KLISTEND};
  new DLibPro(lib::hdf_sd_adddata_pro,string("HDF_SD_ADDDATA"),2,
	      hdf_sd_adddataKey);
  new DLibPro(lib::hdf_sd_fileinfo_pro,string("HDF_SD_FILEINFO"),3);

  const string hdf_sd_getinfoKey[]={"DIMS","HDF_TYPE","NAME",
				    "NATTS","NDIMS","TYPE",
				    "LABEL","UNIT","FORMAT","COORDSYS",
				    KLISTEND};
  new DLibPro(lib::hdf_sd_getinfo_pro,string("HDF_SD_GETINFO"),1,
	      hdf_sd_getinfoKey);

  const string hdf_sd_attrinfoKey[]={"COUNT","DATA","HDF_TYPE","NAME",
				     "TYPE",KLISTEND};
  new DLibPro(lib::hdf_sd_attrinfo_pro,string("HDF_SD_ATTRINFO"),2,
	      hdf_sd_attrinfoKey);

  new DLibPro(lib::hdf_sd_endaccess_pro,string("HDF_SD_ENDACCESS"),1);
  new DLibPro(lib::hdf_sd_end_pro,string("HDF_SD_END"),1);
  new DLibPro(lib::hdf_close_pro,string("HDF_CLOSE"),1);
  
  new DLibFunRetNew(lib::hdf_sd_dimgetid_fun, string("HDF_SD_DIMGETID"), 2);
  const string hdf_sd_dimgetKey[] = {
    "NAME", "NATTR", "SCALE", "COUNT", KLISTEND
  };
  const string hdf_sd_dimgetWarnKey[] = {
    "COMPATIBILITY", "FORMAT", "LABEL", "TYPE", "UNIT", KLISTEND
  };
  new DLibPro(lib::hdf_sd_dimget_pro, string("HDF_SD_DIMGET"), 1, 
    hdf_sd_dimgetKey, hdf_sd_dimgetWarnKey);
 
#endif

  const string tvKey[]={"TRUE","NORMAL","CHANNEL","XSIZE","YSIZE","ORDER","DEVICE","DATA","T3D","Z","CENTIMETERS","INCHES",KLISTEND};
  const string tvWarnKey[]={"WORDS",KLISTEND};
  new DLibPro(lib::tv_image,string("TV"),4,tvKey,tvWarnKey);

  const string tvrdKey[]={"CHANNEL","ORDER","TRUE","WORDS",KLISTEND};
  new DLibFunRetNew(lib::tvrd,string("TVRD"),5,tvrdKey);

  const string loadctKey[]={"GET_NAMES","BOTTOM","NCOLORS","RGB_TABLE","SILENT",KLISTEND};
  const string loadctWarnKey[]={"FILE",KLISTEND};
  new DLibPro(lib::loadct,string("LOADCT_INTERNALGDL"),1,loadctKey,loadctWarnKey);

  // call_external (by Christoph Fuchs)
  const string call_externalKey[] = {"VALUE", "ALL_VALUE", "RETURN_TYPE",
               "B_VALUE", "I_VALUE", "L_VALUE", "F_VALUE", "D_VALUE",
               "UI_VALUE", "UL_VALUE", "L64_VALUE", "UL64_VALUE", "S_VALUE",
               "UNLOAD", "ALL_GDL", "STRUCT_ALIGN_BYTES", KLISTEND };
  new DLibFunRetNew(lib::call_external, string("CALL_EXTERNAL"), -1, call_externalKey);

// WIDGET_ functions
  new DLibPro(lib::widget_displaycontextmenu,string("WIDGET_DISPLAYCONTEXTMENU"),4);
  
#define WIDGET_COMMON_KEYWORDS "EVENT_FUNC", "EVENT_PRO", "FUNC_GET_VALUE", "KILL_NOTIFY" \
    , "NOTIFY_REALIZE" , "PRO_SET_VALUE" , "NO_COPY" , "GROUP_LEADER"\
    , "SCR_XSIZE" , "SCR_YSIZE" , "SCROLL" , "SENSITIVE" \
    , "UNAME" , "UNITS" , "UVALUE" , "XOFFSET" \
    , "XSIZE" , "YOFFSET" , "YSIZE" , "FRAME" \
    , "ALIGN_LEFT", "ALIGN_RIGHT", "ALIGN_CENTER", "ALIGN_BOTTOM", "ALIGN_TOP", "FONT","RESOURCE_NAME"

//ACTIVEX
//BASE  
  const string widget_baseKey[] = {WIDGET_COMMON_KEYWORDS,"MBAR","APP_MBAR","MODAL","COLUMN","ROW",
  "EXCLUSIVE","NONEXCLUSIVE","FLOATING","MAP","TITLE","XPAD","X_SCROLL_SIZE","YPAD","Y_SCROLL_SIZE","DISPLAY_NAME",
  "RNAME_MBAR","TAB_MODE","CONTEXT_EVENTS","KBRD_FOCUS_EVENTS","TLB_ICONIFY_EVENTS","TLB_KILL_REQUEST_EVENTS",
  "TLB_MOVE_EVENTS","TLB_SIZE_EVENTS","TRACKING_EVENTS","GRID_LAYOUT",
  "BASE_ALIGN_CENTER","BASE_ALIGN_LEFT","BASE_ALIGN_RIGHT","BASE_ALIGN_BOTTOM","BASE_ALIGN_TOP","SPACE","CONTEXT_MENU",KLISTEND};
  const string widget_baseWarnKey[] = {"TLB_FRAME_ATTR",  "TOOLBAR", "BITMAP", KLISTEND};
  new DLibFunRetNew(lib::widget_base,string("WIDGET_BASE"),1,widget_baseKey,widget_baseWarnKey);
//BUTTON
  const string widget_buttonKey[] = {WIDGET_COMMON_KEYWORDS,"MENU","VALUE","HELP","SEPARATOR","INPUT_FOCUS","BITMAP","TOOLTIP",
  "TRACKING_EVENTS","DYNAMIC_RESIZE","X_BITMAP_EXTRA",KLISTEND};
  const string widget_buttonWarnKey[] ={"ACCELERATOR","CHECKED_MENU",
 "TAB_MODE","NO_RELEASE",KLISTEND};
  new DLibFunRetNew(lib::widget_button,string("WIDGET_BUTTON"),1,widget_buttonKey,widget_buttonWarnKey);
//COMBOBOX
  const string widget_comboboxKey[] = {WIDGET_COMMON_KEYWORDS,"EDITABLE","TITLE","VALUE","TRACKING_EVENTS","DYNAMIC_RESIZE",KLISTEND};
//  const string widget_comboboxWarnKey[] = {"TAB_MODE",KLISTEND};
  new DLibFunRetNew(lib::widget_combobox,string("WIDGET_COMBOBOX"),1,widget_comboboxKey);//,widget_comboboxWarnKey);
//CONTROL
  const string widget_ControlKey[] = {"REALIZE","MANAGED","EVENT_FUNC","EVENT_PRO",
  "XMANAGER_ACTIVE_COMMAND","DESTROY",
  "GET_UVALUE","SET_UVALUE","SET_VALUE",
  "MAP","FUNC_GET_VALUE","PRO_SET_VALUE",
  "SET_UNAME","NO_COPY","SET_BUTTON",
  "SET_DROPLIST_SELECT","SET_LIST_SELECT","SET_COMBOBOX_SELECT",
  "SENSITIVE","XOFFSET","YOFFSET",
  "GET_VALUE","NO_NEWLINE","TLB_GET_SIZE",
  "HOURGLASS","TLB_SET_TITLE","INPUT_FOCUS",
  "CLEAR_EVENTS","TLB_SET_XOFFSET","TLB_SET_YOFFSET","TLB_GET_OFFSET",
  "DRAW_BUTTON_EVENTS","DRAW_EXPOSE_EVENTS","DRAW_KEYBOARD_EVENTS", 
  "DRAW_MOTION_EVENTS","DRAW_WHEEL_EVENTS","TRACKING_EVENTS","DRAW_VIEWPORT_EVENTS",
  "SET_DROP_EVENTS","KILL_NOTIFY","SHOW","APPEND","USE_TEXT_SELECT","SET_TEXT_SELECT",
  "XSIZE","YSIZE","SCR_XSIZE","SCR_YSIZE","DRAW_XSIZE","DRAW_YSIZE","FRAME","SCROLL",
  "BITMAP","TIMER","NOTIFY_REALIZE","ALL_TABLE_EVENTS","ALL_TEXT_EVENTS",
  "TLB_KILL_REQUEST_EVENTS","TLB_MOVE_EVENTS","TLB_SIZE_EVENTS","TLB_ICONIFY_EVENTS",
  "CONTEXT_EVENTS","KBRD_FOCUS_EVENTS","ALIGNMENT",
  "BACKGROUND_COLOR","FOREGROUND_COLOR","ROW_LABELS","COLUMN_LABELS",
  "COLUMN_WIDTHS","ROW_HEIGHTS","TABLE_DISJOINT_SELECTION","USE_TABLE_SELECT",
  "INSERT_COLUMNS","INSERT_ROWS","DELETE_COLUMNS","DELETE_ROWS",
  "AM_PM", "DAYS_OF_WEEK", "MONTHS",  "SET_TABLE_SELECT","SET_TABLE_VIEW",
  "UPDATE","FORMAT","EDIT_CELL",  "TABLE_XSIZE","TABLE_YSIZE","SEND_EVENT","BAD_ID",
  "GROUP_LEADER", "COMBOBOX_ADDITEM" ,"COMBOBOX_DELETEITEM" ,"COMBOBOX_INDEX", 
  "GET_DRAW_VIEW","SET_TAB_CURRENT", "UNITS","DYNAMIC_RESIZE","SET_SLIDER_MIN","SET_SLIDER_MAX",
  "X_BITMAP_EXTRA",KLISTEND};
  const string widget_WarnControlKey[] ={"SET_TREE_SELECT","SET_TREE_EXPANDED","SET_TREE_INDEX",
  "SET_TREE_BITMAP","DELAY_DESTROY", "DEFAULT_FONT",
  "PUSHBUTTON_EVENTS","TABLE_BLANK","TAB_MODE","SET_TAB_MULTILINE",KLISTEND}; //LIST NOT CLOSE!!!  
  //IMPORTANT :   
  new DLibPro(lib::widget_control,string("WIDGET_CONTROL"),1, 
	      widget_ControlKey,widget_WarnControlKey);
//DISPLAYCONTEXTMENU
//DRAW
  const string widget_drawKey[] = { WIDGET_COMMON_KEYWORDS, "X_SCROLL_SIZE", "Y_SCROLL_SIZE"
  , "MOTION_EVENTS"
  , "BUTTON_EVENTS"
  , "DROP_EVENTS"
  , "EXPOSE_EVENTS"
  , "KEYBOARD_EVENTS"
  , "TRACKING_EVENTS"
  , "WHEEL_EVENTS"
  , "VIEWPORT_EVENTS"
  , "INPUT_FOCUS"
  , "APP_SCROLL"
  , "TOOLTIP"
  , "RETAIN"  //not taken into account, but not useful, too.
  , KLISTEND};
   const string widget_drawWarnKey[] = {
     "CLASSNAME"
    , "COLOR_MODEL"
    , "COLORS"
//    , "DRAG_NOTIFY" //should be implemented 
    , "GRAPHICS_LEVEL"
    , "IGNORE_ACCELERATORS"
    , "RENDERER"
    ,KLISTEND};
  new DLibFunRetNew(lib::widget_draw,string("WIDGET_DRAW"),1,widget_drawKey,widget_drawWarnKey);
 //DROPLIST 
  const string widget_droplistKey[] = {WIDGET_COMMON_KEYWORDS,"TITLE","VALUE","TRACKING_EVENTS","DYNAMIC_RESIZE",KLISTEND};
//  const string widget_droplistWarnKey[] = {"TAB_MODE",KLISTEND};
  new DLibFunRetNew(lib::widget_droplist,string("WIDGET_DROPLIST"),1,widget_droplistKey);//,widget_droplistWarnKey);
//EVENT  
  const string widget_eventKey[] = {"XMANAGER_BLOCK","DESTROY","SAVE_HOURGLASS","NOWAIT","BAD_ID",KLISTEND};
  new DLibFunRetNew(lib::widget_event,string("WIDGET_EVENT"),1,widget_eventKey); //complete!!
//INFO
  const string widget_infoKey[] = {  "DEBUG","ACTIVE","VALID_ID","MODAL","MANAGED",
  "XMANAGER_BLOCK", //only GDL, used in xmanager.pro , may even not be useful now.
  "CHILD","VERSION","GEOMETRY","UNAME",
  "FONTNAME",//not really supported - returns "".
  "BUTTON_SET","PARENT","TEXT_SELECT","FIND_BY_UNAME","TYPE","NAME",
  "TABLE_DISJOINT_SELECTION","TABLE_SELECT","COLUMN_WIDTHS","ROW_HEIGHTS","USE_TABLE_SELECT","SYSTEM_COLORS",
  "TREE_ROOT","TREE_SELECT","TREE_EXPANDED","TREE_FOLDER","TREE_INDEX","TREE_BITMAP","DROP_EVENTS","DRAGGABLE","DRAG_NOTIFY","MASK",
  "LIST_SELECT","DROPLIST_SELECT","COMBOBOX_GETTEXT",
  "TAB_NUMBER","TAB_MULTILINE","TAB_CURRENT","UPDATE",
  "TLB_KILL_REQUEST_EVENTS","TLB_MOVE_EVENTS","TLB_SIZE_EVENTS","TLB_ICONIFY_EVENTS",
  "EVENT_FUNC","EVENT_PRO","N_CHILDREN","ALL_CHILDREN","REALIZED", "UNITS",KLISTEND};
  const string widget_infoWarnKey[]={
//  "COLUMN_WIDTHS",
//  "COMBOBOX_NUMBER","COMPONENT","CONTEXT_EVENTS",
//  "DRAW_BUTTON_EVENTS","DRAW_EXPOSE_EVENTS","DRAW_KEYBOARD_EVENTS","DRAW_MOTION_EVENTS",
//  "DRAW_VIEWPORT_EVENTS","DRAW_WHEEL_EVENTS","DROPLIST_NUMBER","DROPLIST_SELECT",
//  "DYNAMIC_RESIZE","EVENT_FUNC","EVENT_PRO","FIND_BY_UNAME","KBRD_FOCUS_EVENTS",
//  "LIST_MULTIPLE","LIST_NUMBER","LIST_NUM_VISIBLE","LIST_TOP","MAP",,
//  "MULTIPLE_PROPERTIES",,"PROPERTYSHEET_NSELECTED","PROPERTYSHEET_SELECTED",
//  "PROPERTY_VALID","PROPERTY_VALUE","PUSHBUTTON_EVENTS","ROW_HEIGHTS","SENSITIVE","SIBLING",
//  "SLIDER_MIN_MAX","STRING_SIZE","TABLE_ALL_EVENTS","TABLE_BACKGROUND_COLOR",
//  "TABLE_EDITABLE","TABLE_EDIT_CELL","TABLE_FONT","TABLE_FOREGROUND_COLOR","TABLE_VIEW",
//  "TAB_MODE","TEXT_ALL_EVENTS","TEXT_EDITABLE","TEXT_NUMBER","TEXT_OFFSET_TO_XY",
//  "TEXT_TOP_LINE","TEXT_XY_TO_OFFSET",
//  "TOOLTIP","TRACKING_EVENTS","TREE_DRAG_SELECT","UPDATE","USE_TABLE_SELECT","VISIBLE",
  "TAB_MODE",KLISTEND};
  new DLibFunRetNew(lib::widget_info,string("WIDGET_INFO"),1,widget_infoKey,widget_infoWarnKey);
//LABEL
  const string widget_labelKey[] = {WIDGET_COMMON_KEYWORDS,"VALUE","SUNKEN_FRAME","TRACKING_EVENTS","DYNAMIC_RESIZE",KLISTEND};
  new DLibFunRetNew(lib::widget_label,string("WIDGET_LABEL"),1,widget_labelKey);
//LIST
  const string widget_listKey[] = {WIDGET_COMMON_KEYWORDS,"MULTIPLE","VALUE","CONTEXT_EVENTS","TRACKING_EVENTS",KLISTEND};
  const string widget_listWarnKey[] = {"TAB_MODE",KLISTEND};
  new DLibFunRetNew(lib::widget_list,string("WIDGET_LIST"),1,widget_listKey,widget_listWarnKey);
//MESSAGE
//PROPERTYSHEET (2-columns TABLE)
#ifdef HAVE_WXWIDGETS_PROPERTYGRID
  const string widget_propertyKey[] = {WIDGET_COMMON_KEYWORDS,KLISTEND} //to be finished...
  new DLibFunRetNew(lib::widget_property,string("WIDGET_PROPERTYSHEET"),1,widget_propertyKey);
#endif
  //SLIDER
  const string widget_sliderKey[] = {WIDGET_COMMON_KEYWORDS,"TITLE","DRAG","VALUE","MINIMUM","MAXIMUM","VERTICAL",
  "SUPPRESS_VALUE", "TRACKING_EVENTS", KLISTEND};
  const string widget_sliderWarnKey[] = {"TAB_MODE",KLISTEND};
  new DLibFunRetNew(lib::widget_slider,string("WIDGET_SLIDER"),1,widget_sliderKey,widget_sliderWarnKey);
//STUB
//TAB
  const string widget_tabKey[] = {WIDGET_COMMON_KEYWORDS,"MULTILINE","LOCATION","TRACKING_EVENTS", KLISTEND};
//  const string widget_tabWarnKey[] = {WIDGET_COMMON_KEYWORDS,"TAB_MODE", KLISTEND};
  new DLibFunRetNew(lib::widget_tab,string("WIDGET_TAB"),1,widget_tabKey); //,widget_tabWarnKey);
//TABLE
  const string widget_tableKey[] = {WIDGET_COMMON_KEYWORDS
  , "ALIGNMENT"
  , "ALL_EVENTS"
  , "BACKGROUND_COLOR"
  , "COLUMN_LABELS"
  , "COLUMN_WIDTHS"
  , "CONTEXT_EVENTS"
  , "DISJOINT_SELECTION"
  , "EDITABLE"
  , "FOREGROUND_COLOR"
  , "FORMAT"
  , "KBRD_FOCUS_EVENTS"
  , "RESIZEABLE_COLUMNS"
  , "RESIZEABLE_ROWS"
  , "ROW_HEIGHTS"
  , "ROW_LABELS"
  , "TRACKING_EVENTS"
  , "VALUE"
  , "X_SCROLL_SIZE"
  , "Y_SCROLL_SIZE"
  , "NO_COLUMN_HEADERS"
  , "NO_HEADERS"
  , "NO_ROW_HEADERS"
  , "COLUMN_MAJOR"
  , "ROW_MAJOR"
  , "AM_PM"
  , "DAYS_OF_WEEK"
  , "MONTHS"
  , KLISTEND};
  const string widget_tableWarnKey[] = {
  "IGNORE_ACCELERATORS"
  , "TAB_MODE"
  , KLISTEND};
  new DLibFunRetNew(lib::widget_table,string("WIDGET_TABLE"),1,widget_tableKey,widget_tableWarnKey);
//TEXT
  const string widget_textKey[] = {WIDGET_COMMON_KEYWORDS,"EDITABLE","NO_NEWLINE","VALUE",
  "INPUT_FOCUS","ALL_EVENTS","CONTEXT_EVENTS","KBRD_FOCUS_EVENTS","TRACKING_EVENTS",KLISTEND};
  const string widget_textWarnKey[] = {"IGNORE_ACCELERATORS","TAB_MODE","WRAP",KLISTEND};
  new DLibFunRetNew(lib::widget_text,string("WIDGET_TEXT"),1,widget_textKey,widget_textWarnKey);
//TREE
  const string widget_treeWarnKey[] = { "DRAG_NOTIFY"
  , "MASK"
  , "MULTIPLE"
  , "TAB_MODE"
  , KLISTEND};
  const string widget_treeKey[] = {WIDGET_COMMON_KEYWORDS 
  , "BITMAP"
  , "FOLDER"
  , "EXPANDED"
  , "VALUE"
  , "CONTEXT_EVENTS"
  , "TRACKING_EVENTS"
  , "DROP_EVENTS"
  , "DRAGGABLE"
  , "INDEX"
  , KLISTEND}; 
  new DLibFunRetNew(lib::widget_tree,string("WIDGET_TREE"),1,widget_treeKey,widget_treeWarnKey);
//TREE_MOVE  
// 	const string widget_bgroupKey[] =
// 	{"BUTTON_UVALUE","COLUMN","EVENT_FUNC","EXCLUSIVE","NONEXCLUSIVE","SPACE","XPAD","YPAD","FRAME","IDS","LABEL_LEFT","LABEL_TOP","MAP","NO_RELEASE","RETURN_ID","RETURN_INDEX","RETURN_NAME","ROW","SCROLL","SET_VALUE","TAB_MODE","X_SCROLL_SIZE","Y_SCROLL_SIZE","SET_VALUE","UNAME","UVALUE","XOFFSET","XSIZE","YOFFSET","YSIZE",KLISTEND};
// 	new DLibFunRetNew(lib::widget_bgroup,string("CW_BGROUP"),2,widget_bgroupKey);

}
