/***************************************************************************
                 libinit_mes.cpp  -  initialization of GDL library routines
                             -------------------
    begin                : Aug 02 2004
    copyright            : (C) 2004 by Peter Messmer
    email                : messmer@users.sourceforge.net
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
// default: assume we have HDF5
#define USE_HDF5 1
#endif

#include <string>
#include <fstream>

#include "envt.hpp"
#include "dpro.hpp"

#include "basic_fun.hpp"

#ifdef USE_HDF5
#include "hdf5_fun.hpp"
#endif

using namespace std;

void LibInit_mes()
{
  const char KLISTEND[] = "";
  
  // general procedures/functions 
  const string strtokKey[]={"EXTRACT","ESCAPE","LENGTH",
			    "PRESERVE_NULL",KLISTEND};
  new DLibFun(lib::strtok_fun, string("STRTOK"), 2, strtokKey);
  
#ifdef USE_HDF5
  // hdf5 procedures/functions 
  new DLibFun(lib::h5f_open_fun, string("H5F_OPEN"), 1);
  new DLibFun(lib::h5d_open_fun, string("H5D_OPEN"), 2);
  new DLibFun(lib::h5d_read_fun, string("H5D_READ"), 1);
  new DLibFun(lib::h5d_get_space_fun, string("H5D_GET_SPACE"), 1);
  new DLibFun(lib::h5s_get_simple_extent_dims_fun,
	       string("H5S_GET_SIMPLE_EXTENT_DIMS"), 1);
  new DLibPro(lib::h5f_close_pro,string("H5F_CLOSE"),1);
  new DLibPro(lib::h5d_close_pro,string("H5D_CLOSE"),1);
  new DLibPro(lib::h5s_close_pro,string("H5S_CLOSE"),1);
#endif
  
}

