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
#endif

#include "matrix_invert.hpp"

#if defined(USE_LIBPROJ4)||defined(USE_LIBPROJ4_NEW)
#include "gshhs.hpp"
#endif

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
  
  // the 2 following by Alain C. and Levan L.
  const string isaKey[]={"ARRAY","FILE","NULL","NUMBER","SCALAR",KLISTEND};
  new DLibFunRetNew(lib::isa_fun,string("ISA"),2,isaKey);

  new DLibFunRetNew(lib::typename_fun,string("TYPENAME"),1);

  // the following (Chol and Invert) by Alain C. and Nodar K.
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
#endif

#if defined(HAVE_LIBGSL) && defined(HAVE_LIBGSLCBLAS)
  
  const string invertKey[]={"DOUBLE","GSL","EIGEN",KLISTEND};
  new DLibFunRetNew(lib::AC_invert_fun,string("INVERT"),2,invertKey);

  // if FFTw not available, FFT in the GSL used (slower)
  const string fftKey[]={"DOUBLE","INVERSE","OVERWRITE","DIMENSION",KLISTEND};
#if defined(USE_FFTW)
  new DLibFun(lib::fftw_fun,string("FFT"),2,fftKey);
#else
  new DLibFun(lib::fft_fun,string("FFT"),2,fftKey);
#endif

  const string randomKey[]={"DOUBLE","GAMMA","LONG","NORMAL",
			    "BINOMIAL","POISSON","UNIFORM",KLISTEND};
  new DLibFunRetNew(lib::random_fun,string("RANDOMU"),MAXRANK,randomKey);
  new DLibFunRetNew(lib::random_fun,string("RANDOMN"),MAXRANK,randomKey);

  const string checkmathKey[]={"MASK","NOCLEAR","PRINT",KLISTEND};
  new DLibFunRetNew(lib::check_math_fun,string("CHECK_MATH"),2,checkmathKey);

  const string histogramKey[]={"BINSIZE","INPUT","MAX","MIN","NBINS",
			       "OMAX","OMIN","REVERSE_INDICES",
			       "LOCATIONS","NAN",KLISTEND};
  new DLibFunRetNew(lib::histogram_fun,string("HISTOGRAM"),1,histogramKey);

  const string interpolateKey[]={"CUBIC","GRID","MISSING","NEAREST_NEIGHBOUR",KLISTEND};
  new DLibFunRetNew(lib::interpolate_fun,string("INTERPOLATE"),4,interpolateKey);

  const string la_triredKey[]={"DOUBLE","UPPER",KLISTEND};
  new DLibPro(lib::la_trired_pro,string("LA_TRIRED"),3,la_triredKey);
#endif

  const string macharKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::machar_fun,string("MACHAR"),0,macharKey);

  const string rk4Key[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::rk4jmg_fun,string("RK4JMG"),5,rk4Key);


#if defined(USE_LIBPROJ4)||defined(USE_LIBPROJ4_NEW)
  const string map_proj_forwardKey[]={"MAP_STRUCTURE","RADIANS","POLYGONS","POLYLINES","CONNECTIVITY","FILL",KLISTEND};
  new DLibFunRetNew(lib::map_proj_forward_fun,
	      string("MAP_PROJ_FORWARD"),2,map_proj_forwardKey,NULL);

  const string map_proj_inverseKey[]={"RADIANS","MAP_STRUCTURE",KLISTEND};
  new DLibFunRetNew(lib::map_proj_inverse_fun,
	      string("MAP_PROJ_INVERSE"),2,map_proj_inverseKey);
//dummy functions for compatibility support of GCTP projections 
  new DLibPro(lib::map_proj_gctp_forinit,string("MAP_PROJ_GCTP_FORINIT"),4);
  new DLibPro(lib::map_proj_gctp_revinit,string("MAP_PROJ_GCTP_REVINIT"),4);

  // SA: GSHHS dataset
  // TODO: USA, ORIENTATION, LIMIT,
  //       , MLINESTYLE, MLINETHICK, SPACING, T3D, ZVALUE
  const string map_continentsKey[] = {"COLOR", "RIVERS", "COUNTRIES", "COASTS", "CONTINENTS", "USA", 
    "HIRES", "FILL_CONTINENTS", "MAP_STRUCTURE",KLISTEND};
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
#ifdef PL_HAVE_QHULL
  const string triangulateKey[]={"CONNECTIVITY", "SPHERE", "DEGREES", "FVALUE", "REPEATS", "TOLERANCE",KLISTEND};
  new DLibPro(lib::triangulate,string("TRIANGULATE"),4,triangulateKey);

  const string qhullKey[]={"BOUNDS", "CONNECTIVITY", "DELAUNAY", "SPHERE", "VDIAGRAM" ,"VNORMALS", "VVERTICES", KLISTEND};
  new DLibPro(lib::qhull,string("QHULL"),8,qhullKey);

  const string sph_scatKey[]={"BOUNDS", "BOUT", "GOUT", "GS", "NLON", "NLAT", KLISTEND};
  new DLibFunRetNew(lib::sph_scat_fun,string("SPH_SCAT"),3,sph_scatKey);

  const string grid_inputKey[]={"SPHERE", "POLAR", "DEGREES", "DUPLICATES", "EPSILON", "EXCLUDE", KLISTEND};
  new DLibPro(lib::grid_input,string("GRID_INPUT"),6,grid_inputKey);

  const string qgrid3Key[]={"DELTA", "DIMENSION", "MISSING", "START", KLISTEND};
  new DLibFunRetNew(lib::qgrid3_fun,string("QGRID3"),5,qgrid3Key);
#endif

  const string trigridKey[]={"MAX_VALUE","MISSING","NX","NY","MAP",
			     KLISTEND};
  new DLibFunRetNew(lib::trigrid_fun,string("TRIGRID"),6,trigridKey);

  const string poly_2dKey[]={"CUBIC","MISSING",KLISTEND};
  new DLibFunRetNew(lib::poly_2d_fun,string("POLY_2D"),6,poly_2dKey);

  const string make_arrayKey[]={"NOZERO","DIMENSION","INDEX","SIZE",
				"TYPE","VALUE","BYTE","INTEGER",
				"UINT","LONG","ULONG","L64","UL64",
				"FLOAT","DOUBLE","COMPLEX",
				"DCOMPLEX","STRING","PTR","OBJ",KLISTEND};
  new DLibFunRetNew(lib::make_array,string("MAKE_ARRAY"),MAXRANK,make_arrayKey);

  const string reformKey[]={"OVERWRITE",KLISTEND};
  new DLibFun(lib::reform,string("REFORM"),MAXRANK,reformKey);

  new DLibPro(lib::point_lun,string("POINT_LUN"),2);

  new DLibPro(lib::linkimage,string("LINKIMAGE"),4);

  new DLibPro(lib::wait,string("WAIT"),1);

#if defined(USE_HDF)
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
#define WIDGET_COMMON_KEYWORDS "EVENT_FUNC", "EVENT_PRO", "FUNC_GET_VALUE", "KILL_NOTIFY" \
    , "NOTIFY_REALIZE" , "PRO_SET_VALUE" , "NO_COPY" , "GROUP_LEADER"\
    , "SCR_XSIZE" , "SCR_YSIZE" , "SCROLL" , "SENSITIVE" \
    , "UNAME" , "UNITS" , "UVALUE" , "XOFFSET" \
    , "XSIZE" , "YOFFSET" , "YSIZE" , "FRAME"

  const string widget_drawKey[] = { WIDGET_COMMON_KEYWORDS
    , "APP_SCROLL"
    , "BUTTON_EVENTS"
    , "CLASSNAME"
    , "COLOR_MODEL"
    , "COLORS"
    , "DRAG_NOTIFY"
    , "DROP_EVENTS"
    , "EXPOSE_EVENTS"
    , "GRAPHICS_LEVEL"
    , "IGNORE_ACCELERATORS"
    , "KEYBOARD_EVENTS"
    , "MOTION_EVENTS"
    , "RENDERER"
    , "RESOURCE_NAME"
    , "RETAIN"
    , "TOOLTIP"
    , "TRACKING_EVENTS"
    , "VIEWPORT_EVENTS"
    , "WHEEL_EVENTS"
    , "X_SCROLL_SIZE"
    , "Y_SCROLL_SIZE"
    ,KLISTEND};
  new DLibFunRetNew(lib::widget_draw,string("WIDGET_DRAW"),1,widget_drawKey);
  
  const string widget_tableKey[] = {WIDGET_COMMON_KEYWORDS
  , "ALIGNMENT"
  , "ALL_EVENTS"
  , "AM_PM"
  , "BACKGROUND_COLOR"
  , "COLUMN_LABELS"
  , "COLUMN_MAJOR"
  , "ROW_MAJOR"
  , "COLUMN_WIDTHS"
  , "CONTEXT_EVENTS"
  , "DAYS_OF_WEEK"
  , "DISJOINT_SELECTION"
  , "EDITABLE"
  , "FONT"
  , "FOREGROUND_COLOR"
  , "FORMAT"
  , "GROUP_LEADER"
  , "IGNORE_ACCELERATORS"
  , "KBRD_FOCUS_EVENTS"
  , "MONTHS"
  , "NO_COLUMN_HEADERS"
  , "NO_HEADERS"
  , "NO_ROW_HEADERS"
  , "RESIZEABLE_COLUMNS"
  , "RESIZEABLE_ROWS"
  , "ROW_HEIGHTS"
  , "ROW_LABELS"
  , "TAB_MODE"
  , "TRACKING_EVENTS"
  , "VALUE"
  , "X_SCROLL_SIZE"
  , "Y_SCROLL_SIZE"
  , KLISTEND};
  new DLibFunRetNew(lib::widget_table,string("WIDGET_TABLE"),1,widget_tableKey);
  
  const string widget_treeKey[] = {WIDGET_COMMON_KEYWORDS
  , "ALIGN_BOTTOM"
  , "ALIGN_CENTER"
  , "ALIGN_LEFT"
  , "ALIGN_RIGHT"
  , "ALIGN_TOP"
  , "BITMAP"
  , "CHECKBOX"
  , "CHECKED"
  , "DRAG_NOTIFY"
  , "DRAGGABLE"
  , "EXPANDED"
  , "FOLDER"
  , "GROUP_LEADER"
  , "INDEX"
  , "MASK"
  , "MULTIPLE"
  , "NO_BITMAPS"
  , "TAB_MODE"
  , "TOOLTIP"
  , KLISTEND};
//   , "CONTEXT_EVENTS"
//   , "TRACKING_EVENTS"
//   , "DROP_EVENTS"
  new DLibFunRetNew(lib::widget_base,string("WIDGET_TREE"),1,widget_treeKey);
  
  const string widget_baseKey[] = {WIDGET_COMMON_KEYWORDS,"ALIGN_BOTTOM","ALIGN_CENTER","ALIGN_LEFT","ALIGN_RIGHT","ALIGN_TOP","MBAR","MODAL","BASE_ALIGN_BOTTOM","BASE_ALIGN_CENTER","BASE_ALIGN_LEFT","BASE_ALIGN_RIGHT","BASE_ALIGN_TOP","COLUMN","ROW","CONTEXT_EVENTS","CONTEXT_MENU","EXCLUSIVE","NONEXCLUSIVE","FLOATING","GRID_LAYOUT","KBRD_FOCUS_EVENTS","MAP","SPACE","TITLE","TLB_FRAME_ATTR","TLB_ICONIFY_EVENTS","TLB_KILL_REQUEST_EVENTS","TLB_MOVE_EVENTS","TLB_SIZE_EVENTS","TOOLBAR","TRACKING_EVENTS","XPAD","X_SCROLL_SIZE","YPAD","Y_SCROLL_SIZE","DISPLAY_NAME","RESOURCE_NAME","RNAME_MBAR","TAB_MODE",KLISTEND};
  new DLibFunRetNew(lib::widget_base,string("WIDGET_BASE"),1,widget_baseKey);

  const string widget_buttonKey[] = {WIDGET_COMMON_KEYWORDS,"ACCELERATOR","ALIGN_CENTER","ALIGN_LEFT","ALIGN_RIGHT","BITMAP","CHECKED_MENU","DYNAMIC_RESIZE","FONT","HELP","MENU","NO_COPY","NO_RELEASE","PUSHBUTTON_EVENTS","SEPARATOR","TAB_MODE","TOOLTIP","TRACKING_EVENTS","VALUE","X_BITMAP_EXTRA",KLISTEND};
  new DLibFunRetNew(lib::widget_button,string("WIDGET_BUTTON"),1,widget_buttonKey);

  const string widget_droplistKey[] = {WIDGET_COMMON_KEYWORDS,"DYNAMIC_RESIZE","FONT","RESOURCE_NAME","TAB_MODE","TITLE","TRACKING_EVENTS","VALUE",KLISTEND};
  new DLibFunRetNew(lib::widget_droplist,string("WIDGET_DROPLIST"),1,widget_droplistKey);

  const string widget_comboboxKey[] = {WIDGET_COMMON_KEYWORDS,"EDITABLE","DYNAMIC_RESIZE","FONT","RESOURCE_NAME","TAB_MODE","TITLE","TRACKING_EVENTS","VALUE",KLISTEND};
  new DLibFunRetNew(lib::widget_combobox,string("WIDGET_COMBOBOX"),1,widget_comboboxKey);

  const string widget_listKey[] = {WIDGET_COMMON_KEYWORDS,"CONTEXT_EVENTS","FONT","MULTIPLE","RESOURCE_NAME","TAB_MODE","TRACKING_EVENTS","VALUE",KLISTEND};
  new DLibFunRetNew(lib::widget_list,string("WIDGET_LIST"),1,widget_listKey);

// 	const string widget_bgroupKey[] =
// 	{"BUTTON_UVALUE","COLUMN","EVENT_FUNC","EXCLUSIVE","NONEXCLUSIVE","SPACE","XPAD","YPAD","FONT","FRAME","IDS","LABEL_LEFT","LABEL_TOP","MAP","NO_RELEASE","RETURN_ID","RETURN_INDEX","RETURN_NAME","ROW","SCROLL","SET_VALUE","TAB_MODE","X_SCROLL_SIZE","Y_SCROLL_SIZE","SET_VALUE","UNAME","UVALUE","XOFFSET","XSIZE","YOFFSET","YSIZE",KLISTEND};
// 	new DLibFunRetNew(lib::widget_bgroup,string("CW_BGROUP"),2,widget_bgroupKey);

  const string widget_textKey[] = {WIDGET_COMMON_KEYWORDS,"ALL_EVENTS","CONTEXT_EVENTS","EDITABLE","FONT","IGNORE_ACCELERATORS","KBRD_FOCUS_EVENTS","NO_NEWLINE","RESOURCE_NAME","TAB_MODE","TRACKING_EVENTS","VALUE","WRAP",KLISTEND};
  new DLibFunRetNew(lib::widget_text,string("WIDGET_TEXT"),1,widget_textKey);

  const string widget_labelKey[] = {WIDGET_COMMON_KEYWORDS,"ALL_EVENTS","CONTEXT_EVENTS","EDITABLE","FONT","IGNORE_ACCELERATORS","KBRD_FOCUS_EVENTS","NO_NEWLINE","RESOURCE_NAME","TAB_MODE","TRACKING_EVENTS","VALUE","WRAP",KLISTEND};
  const string widget_labelWarnKey[] = {"ALIGN_CENTER","ALIGN_LEFT","ALIGN_RIGHT", KLISTEND};
  new DLibFunRetNew(lib::widget_label,string("WIDGET_LABEL"),1,widget_labelKey,widget_labelWarnKey);

  const string widget_tabKey[] = {WIDGET_COMMON_KEYWORDS,"ALIGN_BOTTOM","ALIGN_CENTER","ALIGN_LEFT","ALIGN_RIGHT","ALIGN_TOP","MULTILINE","LOCATION","TAB_MODE","TRACKING_EVENTS", KLISTEND};
  new DLibFunRetNew(lib::widget_tab,string("WIDGET_TAB"),1,widget_tabKey);

  const string widget_slider[] = {WIDGET_COMMON_KEYWORDS,"TITLE"/*TODO*/,"DRAG","VALUE","MINIMUM","MAXIMUM","VERTICAL","SUPPRESS_VALUE", KLISTEND};
  new DLibFunRetNew(lib::widget_slider,string("WIDGET_SLIDER"),1,widget_slider);

  
  const string widget_infoKey[] = {"VALID","MODAL","MANAGED","XMANAGER_BLOCK","CHILD","VERSION",
				   KLISTEND};
  new DLibFunRetNew(lib::widget_info,string("WIDGET_INFO"),1,widget_infoKey);

  const string widget_eventKey[] = {"XMANAGER_BLOCK","DESTROY",
				    KLISTEND};
  new DLibFunRetNew(lib::widget_event,string("WIDGET_EVENT"),1,widget_eventKey);

  const string widget_ControlKey[] = {"REALIZE","MANAGED","EVENT_FUNC","EVENT_PRO",
				      "XMANAGER_ACTIVE_COMMAND","DESTROY",
				      "GET_UVALUE","SET_UVALUE","SET_VALUE",
				      "MAP","FUNC_GET_VALUE","PRO_SET_VALUE",
				      "SET_UNAME","NO_COPY","SET_BUTTON",
				      "SET_DROPLIST_SELECT","SENSITIVE",
				      "GET_VALUE",KLISTEND};
  const string widget_WarnControlKey[] ={"DEFAULT_FONT","CLEAR_EVENTS",KLISTEND};
  new DLibPro(lib::widget_control,string("WIDGET_CONTROL"),1, 
	      widget_ControlKey,widget_WarnControlKey);

}
