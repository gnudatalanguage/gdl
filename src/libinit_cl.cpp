/***************************************************************************
                   libinit_cl.cpp  -  initialization of GDL library routines
                             -------------------
    begin                : March 18 2004
    copyright            : (C) 2004 by Christopher Lee, 
                         : Original by Marc Schellens(2002(C))
    email                : chrislee0@hotmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#else
// default: assume we have netCDF
#define USE_NETCDF 1
#define USE_MAGICK 1
#endif


#include <string>
#include <fstream>

#include "envt.hpp"
#include "dpro.hpp"
#include "gdljournal.hpp"
#include "basic_fun_cl.hpp"
#ifdef USE_MAGICK 
#include "magick_cl.hpp"
#endif
#ifdef USE_NETCDF
#include "ncdf_cl.hpp"
#endif

using namespace std;

void LibInit_cl()
{
  const char KLISTEND[] = "";
    
  const string systimeKey[]={"JULIAN","SECONDS","UTC",KLISTEND};
  new DLibFun(lib::systime,string("SYSTIME"),2,systimeKey);

    const string legendreKey[]={"DOUBLE",KLISTEND};
  new DLibFun(lib::legendre,string("LEGENDRE"),3,legendreKey);

  //  new DLibFun(lib::exp, string("EXP"),1);
  new DLibFun(lib::gamma, string("GAMMA"),1);
  new DLibFun(lib::lngamma, string("LNGAMMA"),1);

  const string igammaKey[]={"DOUBLE","EPS","ITER","ITMAX","METHOD",KLISTEND};
  new DLibFun(lib::igamma, string("IGAMMA"),2);

  const string betaKey[]={"DOUBLE",KLISTEND};
  new DLibFun(lib::beta, string("BETA"),2);

  new DLibFun(lib::ncdf_exists,string("NCDF_EXISTS"));
  new DLibFun(lib::magick_exists,string("MAGICK_EXISTS"));

#ifdef USE_NETCDF
  //open NetCDF
  const string ncdf_openKey[]={"WRITE","NOWRITE",KLISTEND};
  new DLibFun(lib::ncdf_open,string("NCDF_OPEN"),1,ncdf_openKey);

  //create NetCDF
  const string ncdf_createKey[]={"CLOBBER","NOCLOBBER",KLISTEND};
  new DLibFun(lib::ncdf_create,string("NCDF_CREATE"),1,ncdf_createKey);

  //close NetCDF
  new DLibPro(lib::ncdf_close,string("NCDF_CLOSE"),1);

  //reading NetCDF
  //Inquire
  new DLibFun(lib::ncdf_inquire,string("NCDF_INQUIRE"),1);

  //Variable Inquire
  new DLibFun(lib::ncdf_varinq,string("NCDF_VARINQ"),2);

  //Varget 1
  const string ncdf_varget1Key[]={"OFFSET",KLISTEND};
  new DLibPro(lib::ncdf_varget1,string("NCDF_VARGET1"),3,ncdf_varget1Key);

  //Varget
  const string ncdf_vargetKey[]={"COUNT","OFFSET", "STRIDE",KLISTEND};
  new DLibPro(lib::ncdf_varget,string("NCDF_VARGET"),3,ncdf_vargetKey);

  //Variable ID
  new DLibFun(lib::ncdf_varid,string("NCDF_VARID"),2);


  //Dimension Inquire
  new DLibPro(lib::ncdf_diminq,string("NCDF_DIMINQ"),4);

  //Dimension ID
  new DLibFun(lib::ncdf_dimid,string("NCDF_DIMID"),2);

  //Get attribute name
  const string ncdf_attnameKey[]={"GLOBAL",KLISTEND};
  new DLibFun(lib::ncdf_attname,string("NCDF_ATTNAME"),3,ncdf_attnameKey);

  //Get attribute
  const string ncdf_attgetKey[]={"GLOBAL",KLISTEND};
  new DLibPro(lib::ncdf_attget,string("NCDF_ATTGET"),3,ncdf_attgetKey);

  //Inquire attribute (datatype, size?)
  const string ncdf_attinqKey[]={"GLOBAL",KLISTEND};
  new DLibFun(lib::ncdf_attinq,string("NCDF_ATTINQ"),2,ncdf_attinqKey);


  //Control functions, define mode etc.
  const string ncdf_controlKey[]={"ABORT","ENDEF","FILL","NOFILL", "VERBOSE","NOVERBOSE","OLDFILL","REDEF","SYNC",KLISTEND};
  new DLibPro(lib::ncdf_control,string("NCDF_CONTROL"),1,ncdf_controlKey);


  //writing NetCDF
  //Put attribute into file
  const string ncdf_attputKey[]={"GLOBAL","LENGTH","BYTE","CHAR","DOUBLE","FLOAT","LONG","SHORT",KLISTEND};
  new DLibPro(lib::ncdf_attput, string("NCDF_ATTPUT"), 3,ncdf_attputKey);

  //copy attribute between files
  const string ncdf_attcopyKey[]={"IN_GLOBAL","OUT_GLOBAL",KLISTEND};
  new DLibFun(lib::ncdf_attcopy, string("NCDF_ATTCOPY"), 3,ncdf_attcopyKey);

  //delete attribute
  const string ncdf_attdelKey[]={"GLOBAL",KLISTEND};
  new DLibPro(lib::ncdf_attdel, string("NCDF_ATTDEL"), 2,ncdf_attdelKey);

  //rename attribute
  const string ncdf_attrenameKey[]={"GLOBAL",KLISTEND};
  new DLibPro(lib::ncdf_attrename, string("NCDF_ATTRENAME"), 3,ncdf_attrenameKey);

  //define dimension
  const string ncdf_dimdefKey[]={"UNLIMITED",KLISTEND};
  new DLibFun(lib::ncdf_dimdef, string("NCDF_DIMDEF"), 3,ncdf_dimdefKey);

  //rename dimension
  new DLibPro(lib::ncdf_dimrename, string("NCDF_DIMRENAME"), 3);

  //define variable
  const string ncdf_vardefKey[]={"BYTE","CHAR","DOUBLE","FLOAT","LONG","SHORT",KLISTEND};
  new DLibFun(lib::ncdf_vardef, string("NCDF_VARDEF"), 3,ncdf_vardefKey);

  //rename variable
  new DLibPro(lib::ncdf_varrename, string("NCDF_VARRENAME"), 3);

  //put variable
  const string ncdf_varputKey[]={"COUNT","OFFSET", "STRIDE",KLISTEND};
  new DLibPro(lib::ncdf_varput, string("NCDF_VARPUT"), 3,ncdf_varputKey);
#endif

#ifdef USE_MAGICK
  new DLibFun(lib::magick_open,string("MAGICK_OPEN"),1);
  new DLibFun(lib::magick_create,string("MAGICK_CREATE"),3);
  new DLibPro(lib::magick_close,string("MAGICK_CLOSE"),1);

  const string magick_readKey[]={"RGB","SUB_RECT","MAP",KLISTEND};
  new DLibFun(lib::magick_read,string("MAGICK_READ"),1,magick_readKey);
  new DLibFun(lib::magick_readindexes,string("MAGICK_READINDEXES"),1);
  new DLibPro(lib::magick_readcolormapRGB,string("MAGICK_READCOLORMAPRGB"),4);
  const string magick_writeKey[]={"RGB",KLISTEND};
  new DLibPro(lib::magick_write,string("MAGICK_WRITE"),2,magick_writeKey);
  new DLibPro(lib::magick_writefile,string("MAGICK_WRITEFILE"),3);

  //attributes
  new DLibFun(lib::magick_IndexedColor,string("MAGICK_INDEXEDCOLOR"),1);
  new DLibFun(lib::magick_rows,string("MAGICK_ROWS"),1);
  new DLibFun(lib::magick_columns,string("MAGICK_COLUMNS"),1);
  new DLibFun(lib::magick_colormapsize,string("MAGICK_COLORMAPSIZE"),2);
  new DLibFun(lib::magick_magick,string("MAGICK_MAGICK"),2);
  new DLibPro(lib::magick_quality,string("MAGICK_QUALITY"),2);

  const string magick_quantizeKey[]={"TRUECOLOR","YUV","GRAYSCALE","DITHER",KLISTEND};
  new DLibPro(lib::magick_quantize,string("MAGICK_QUANTIZE"),2,magick_quantizeKey);
 
  new DLibPro(lib::magick_writeIndexes, string("MAGICK_WRITEINDEXES"),2);
  new DLibPro(lib::magick_writeColorTable, string("MAGICK_WRITECOLORTABLE"),4);


  //manipulation
  new DLibPro(lib::magick_flip,string("MAGICK_FLIP"),1);

  const string magick_matteKey[]={"TRUE","FALSE",KLISTEND};
  new DLibPro(lib::magick_matte,string("MAGICK_MATTE"),1);

  const string magick_interlaceKey[]={"NOINTERLACE","LINEINTERLACE","PLANEINTERLACE",KLISTEND};

  new DLibPro(lib::magick_interlace,string("MAGICK_INTERLACE"),1,magick_interlaceKey);
  
  const string magick_addnoiseKey[]={"UNIFORMNOISE","GAUSSIANNOISE","MULTIPLICATIVEGAUSSIANNOISE","IMPULSENOISE","LAPLACIANNOISE","POISSONNOISE","NOISE",KLISTEND};
  new DLibPro(lib::magick_addNoise,string("MAGICK_ADDNOISE"),1,magick_addnoiseKey);

  //hmm
  new DLibPro(lib::magick_display,string("MAGICK_DISPLAY"),1);
#endif
}

