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

using namespace std;

void LibInit_jmg()
{
  const char KLISTEND[] = "";
  // the following by Joel Gales <jomoga@users.sourceforge.net>
  const string sizeKey[]={"L64","DIMENSIONS","FILE_LUN","N_DIMENSIONS",
			  "N_ELEMENTS","STRUCTURE","TNAME","TYPE",KLISTEND};
  new DLibFun(lib::size,string("SIZE"),1,sizeKey);
  new DLibFun(lib::fstat,string("FSTAT"),1);
  const string routine_namesKey[]={"LEVEL","VARIABLES","FETCH","ARG_NAME",
				   "STORE","S_FUNCTIONS","S_PROCEDURES",
				   KLISTEND};
  new DLibFun(lib::routine_names,string("ROUTINE_NAMES"),-1,routine_namesKey);

#if defined(HAVE_LIBGSL) && defined(HAVE_LIBGSLCBLAS)
  const string invertKey[]={"DOUBLE",KLISTEND};
  new DLibFun(lib::invert_fun,string("INVERT"),2,invertKey);

  const string fftKey[]={"DOUBLE","INVERSE","OVERWRITE","DIMENSION",KLISTEND};
#if defined(USE_FFTW)
  new DLibFun(lib::fftw_fun,string("FFT"),2,fftKey);
#else
  new DLibFun(lib::fft_fun,string("FFT"),2,fftKey);
#endif

  const string randomKey[]={"DOUBLE","GAMMA","LONG","NORMAL",
			    "BINOMIAL","POISSON","UNIFORM",KLISTEND};
  new DLibFun(lib::random_fun,string("RANDOMU"),MAXRANK,randomKey);
  new DLibFun(lib::random_fun,string("RANDOMN"),MAXRANK,randomKey);

  const string checkmathKey[]={"MASK","NOCLEAR","PRINT",KLISTEND};
  new DLibFun(lib::check_math_fun,string("CHECK_MATH"),2,checkmathKey);

  const string histogramKey[]={"BINSIZE","INPUT","MAX","MIN","NBINS",
			       "OMAX","OMIN","REVERSE_INDICES",
			       "LOCATIONS",KLISTEND};
  new DLibFun(lib::histogram_fun,string("HISTOGRAM"),1,histogramKey);

  const string interpolateKey[]={"CUBIC","GRID","MISSING",KLISTEND};
  new DLibFun(lib::interpolate_fun,string("INTERPOLATE"),4,interpolateKey);

  const string la_triredKey[]={"DOUBLE","UPPER",KLISTEND};
  new DLibPro(lib::la_trired_pro,string("LA_TRIRED"),3,la_triredKey);
#endif

  const string macharKey[]={"DOUBLE",KLISTEND};
  new DLibFun(lib::machar_fun,string("MACHAR"),0,macharKey);

  const string rk4Key[]={"DOUBLE",KLISTEND};
  new DLibFun(lib::rk4jmg_fun,string("RK4JMG"),5,rk4Key);

#if defined(USE_LIBPROJ4)
  const string map_proj_forwardKey[]={"CONNECTIVITY","FILL",
				      "MAP_STRUCTURE","POLYGONS",
				      "POLYLINES","RADIANS",KLISTEND};
  new DLibFun(lib::map_proj_forward_fun,
	      string("MAP_PROJ_FORWARD"),3,map_proj_forwardKey);

  const string map_proj_inverseKey[]={"RADIANS",KLISTEND};
  new DLibFun(lib::map_proj_inverse_fun,
	      string("MAP_PROJ_INVERSE"),3,map_proj_inverseKey);
#endif

  const string convert_coordKey[]={"DATA","DEVICE","NORMAL","T3D","DOUBLE",
				   "TO_DATA","TO_DEVICE","TO_NORMAL",KLISTEND};
  new DLibFun(lib::convert_coord,string("CONVERT_COORD"),3,convert_coordKey);


  const string finiteKey[]={"INFINITY","NAN",KLISTEND};
  new DLibFun(lib::finite_fun,string("FINITE"),1,finiteKey);

  const string radonKey[]={"BACKPROJECT","DOUBLE","DRHO","DX","DY",
			   "GRAY","LINEAR","NRHO","NTHETA","NX","NY",
			   "RHO","RMIN","THETA","XMIN","YMIN",KLISTEND};
  new DLibFun(lib::radon_fun,string("RADON"),1,radonKey);

  const string trigridKey[]={"MAX_VALUE","MISSING","NX","NY","MAP",
			     KLISTEND};
  new DLibFun(lib::trigrid_fun,string("TRIGRID"),6,trigridKey);

  const string poly_2dKey[]={"CUBIC","MISSING",KLISTEND};
  new DLibFun(lib::poly_2d_fun,string("POLY_2D"),6,poly_2dKey);

  const string make_arrayKey[]={"NOZERO","DIMENSION","INDEX","SIZE",
				"TYPE","VALUE","BYTE","INTEGER",
				"UINT","LONG","ULONG","L64","UL64",
				"FLOAT","DOUBLE","COMPLEX",
				"DCOMPLEX","STRING","PTR","OBJ",KLISTEND};
  new DLibFun(lib::make_array,string("MAKE_ARRAY"),MAXRANK,make_arrayKey);

  const string reformKey[]={"OVERWRITE",KLISTEND};
  new DLibFun(lib::reform,string("REFORM"),MAXRANK,reformKey);

  new DLibPro(lib::point_lun,string("POINT_LUN"),2);

  new DLibPro(lib::linkimage,string("LINKIMAGE"),4);

  new DLibPro(lib::wait,string("WAIT"),1);

#if defined(USE_HDF)
  const string hdf_openKey[]={"READ","RDWR","CREATE",KLISTEND};
  new DLibFun(lib::hdf_open_fun,string("HDF_OPEN"),2,hdf_openKey);

  new DLibFun(lib::hdf_vg_getid_fun,string("HDF_VG_GETID"),2);

  const string hdf_vg_attachKey[]={"READ","WRITE",KLISTEND};
  new DLibFun(lib::hdf_vg_attach_fun,string("HDF_VG_ATTACH"),2,
	      hdf_vg_attachKey);

  const string hdf_vd_attachKey[]={"READ","WRITE",KLISTEND};
  new DLibFun(lib::hdf_vd_attach_fun,string("HDF_VD_ATTACH"),2,
	      hdf_vd_attachKey);
  new DLibFun(lib::hdf_vd_find_fun,string("HDF_VD_FIND"),2);

  const string hdf_vd_readKey[]={"FIELDS","NRECORDS","FULL_INTERLACE",
				 "NO_INTERLACE",KLISTEND};
  new DLibFun(lib::hdf_vd_read_fun,string("HDF_VD_READ"),2,hdf_vd_readKey);

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
  new DLibFun(lib::hdf_sd_create_fun,string("HDF_SD_CREATE"),3,
	      hdf_sd_createKey);

  const string hdf_sd_startKey[]={"READ","RDWR","CREATE",KLISTEND};
  new DLibFun(lib::hdf_sd_start_fun,string("HDF_SD_START"),2,hdf_sd_startKey);
  new DLibFun(lib::hdf_sd_nametoindex_fun,string("HDF_SD_NAMETOINDEX"),2);
  new DLibFun(lib::hdf_sd_attrfind_fun,string("HDF_SD_ATTRFIND"),2);
  new DLibFun(lib::hdf_sd_select_fun,string("HDF_SD_SELECT"),2);

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
  
  new DLibFun(lib::hdf_sd_dimgetid_fun, string("HDF_SD_DIMGETID"), 2);
  const string hdf_sd_dimgetKey[] = {
    "NAME", "NATTR", "SCALE", "COUNT", KLISTEND
  };
  const string hdf_sd_dimgetWarnKey[] = {
    "COMPATIBILITY", "FORMAT", "LABEL", "TYPE", "UNIT", KLISTEND
  };
  new DLibPro(lib::hdf_sd_dimget_pro, string("HDF_SD_DIMGET"), 1, 
    hdf_sd_dimgetKey, hdf_sd_dimgetWarnKey);
 
#endif

  const string tvKey[]={"TRUE","NORMAL","CHANNEL","XSIZE","YSIZE","ORDER",KLISTEND};
  new DLibPro(lib::tv,string("TV"),4,tvKey);

  const string tvrdKey[]={"CHANNEL","ORDER","TRUE","WORDS",KLISTEND};
  new DLibFun(lib::tvrd,string("TVRD"),5,tvrdKey);

  const string loadctKey[]={"GET_NAMES",KLISTEND};
  new DLibPro(lib::loadct,string("LOADCT_INTERNALGDL"),1,loadctKey);


#ifdef HAVE_LIBWXWIDGETS
  const string widget_baseKey[] = {"ALIGN_BOTTOM","ALIGN_CENTER","ALIGN_LEFT","ALIGN_RIGHT","ALIGN_TOP","MBAR","MODAL","BASE_ALIGN_BOTTOM","BASE_ALIGN_CENTER","BASE_ALIGN_LEFT","BASE_ALIGN_RIGHT","BASE_ALIGN_TOP","COLUMN","ROW","CONTEXT_EVENTS","CONTEXT_MENU","EVENT_FUNC","EVENT_PRO","EXCLUSIVE","NONEXCLUSIVE","FLOATING","FRAME","FUNC_GET_VALUE","GRID_LAYOUT","GROUP_LEADER","KBRD_FOCUS_EVENTS","KILL_NOTIFY","MAP","NO_COPY","NOTIFY_REALIZE","PRO_SET_VALUE","SCR_XSIZE","SCR_YSIZE","SCROLL","SENSITIVE","SPACE","TITLE","TLB_FRAME_ATTR","TLB_ICONIFY_EVENTS","TLB_KILL_REQUEST_EVENTS","TLB_MOVE_EVENTS","TLB_SIZE_EVENTS","TOOLBAR","TRACKING_EVENTS","UNITS","UNAME","UVALUE","XOFFSET","XPAD","XSIZE","X_SCROLL_SIZE","YOFFSET","YPAD","YSIZE","Y_SCROLL_SIZE","DISPLAY_NAME","RESOURCE_NAME","RNAME_MBAR",KLISTEND};
  new DLibFun(lib::widget_base,string("WIDGET_BASE"),0,widget_baseKey);

  const string widget_buttonKey[] = {"ACCELERATOR","ALIGN_CENTER","ALIGN_LEFT","ALIGN_RIGHT","BITMAP","CHECKED_MENU","DYNAMIC_RESIZE","EVENT_FUNC","EVENT_PRO","FONT","FRAME","FUNC_GET_VALUE","GROUP_LEADER","HELP","KILL_NOTIFY","MENU","NO_COPY","NO_RELEASE","NOTIFY_REALIZE","PRO_SET_VALUE","PUSHBUTTON_EVENTS","SCR_XSIZE","SCR_YSIZE","SENSITIVE","SEPARATOR","TAB_MODE","TOOLTIP","TRACKING_EVENTS","UNAME","UNITS","UVALUE","VALUE","X_BITMAP_EXTRA","XOFFSET","XSIZE","YOFFSET","YSIZE",KLISTEND};
  new DLibFun(lib::widget_button,string("WIDGET_BUTTON"),1,widget_buttonKey);

  const string widget_textKey[] = {"ALL_EVENTS","CONTEXT_EVENTS","EDITABLE","EVENT_FUNC","EVENT_PRO","FONT","FRAME","FUNC_GET_VALUE","GROUP_LEADER","IGNORE_ACCELERATORS","KBRD_FOCUS_EVENTS","KILL_NOTIFY","NO_COPY","NO_NEWLINE","NOTIFY_REALIZE","PRO_SET_VALUE","RESOURCE_NAME","SCR_XSIZE","SCR_YSIZE","SCROLL","SENSITIVE","TAB_MODE","TRACKING_EVENTS","UNAME","UNITS","UVALUE","VALUE","WRAP","XOFFSET","XSIZE","YOFFSET","YSIZE",KLISTEND};
  new DLibFun(lib::widget_text,string("WIDGET_TEXT"),1,widget_textKey);


  const string widget_infoKey[] = {"VALID","MODAL","MANAGED","XMANAGER_BLOCK","CHILD","VERSION",
				   KLISTEND};
  new DLibFun(lib::widget_info,string("WIDGET_INFO"),1,widget_infoKey);

  const string widget_eventKey[] = {"XMANAGER_BLOCK","DESTROY",
				    KLISTEND};
  new DLibFun(lib::widget_event,string("WIDGET_EVENT"),1,widget_eventKey);

  const string widget_controlKey[] = {"REALIZE","MANAGED","EVENT_PRO",
				      "XMANAGER_ACTIVE_COMMAND","DESTROY",
				      "GET_UVALUE","SET_UVALUE","SET_VALUE","MAP",
				      KLISTEND};
  new DLibPro(lib::widget_control,string("WIDGET_CONTROL"),1, 
	      widget_controlKey);
#endif

}


