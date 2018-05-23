/***************************************************************************
            libinit_exists.cpp  -  initialization of GDL library routines
            -------------------
    begin                : 25 December 2017
    copyright            : (C) 2017 by Alain Coulais
    email                :  alaingdl@users.sourceforge.net
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


#include "includefirst.hpp"

#include <string>
#include <fstream>

#include "envt.hpp"
#include "dpro.hpp"

/*
#include "gdljournal.hpp"
#include "basic_fun_cl.hpp"
#ifdef USE_MAGICK 
#include "magick_cl.hpp"
#endif
#ifdef USE_NETCDF
#include "ncdf_cl.hpp"
#endif
*/

#include "exists_fun.hpp"
#include "dialog.hpp"


using namespace std;

void LibInit_exists()
{
  new DLibFunRetNew(lib::eigen_exists,string("EIGEN_EXISTS"));
  new DLibFunRetNew(lib::fftw_exists,string("FFTW_EXISTS"));
  new DLibFunRetNew(lib::glpk_exists,string("GLPK_EXISTS"));
  new DLibFunRetNew(lib::grib_exists,string("GRIB_EXISTS"));
  new DLibFunRetNew(lib::gshhg_exists,string("GSHHG_EXISTS"));
  new DLibFunRetNew(lib::hdf5_exists,string("HDF5_EXISTS"));
  new DLibFunRetNew(lib::hdf_exists,string("HDF_EXISTS"));
  new DLibFunRetNew(lib::magick_exists,string("MAGICK_EXISTS"));
  new DLibFunRetNew(lib::ncdf_exists,string("NCDF_EXISTS"));
  new DLibFunRetNew(lib::ncdf4_exists,string("NCDF4_EXISTS"));
  new DLibFunRetNew(lib::openmp_exists,string("OPENMP_EXISTS"));
  new DLibFunRetNew(lib::pnglib_exists,string("PNGLIB_EXISTS"));
  new DLibFunRetNew(lib::proj4_exists,string("PROJ4_EXISTS"));
  new DLibFunRetNew(lib::proj4new_exists,string("PROJ4NEW_EXISTS"));
  new DLibFunRetNew(lib::pslib_exists,string("PSLIB_EXISTS"));
  new DLibFunRetNew(lib::python_exists,string("PYTHON_EXISTS"));
  new DLibFunRetNew(lib::tiff_exists,string("TIFF_EXISTS"));
  new DLibFunRetNew(lib::udunits_exists,string("UDUNITS_EXISTS"));
  new DLibFunRetNew(lib::wxwidgets_exists, string("WXWIDGETS_EXISTS"));
  new DLibFunRetNew(lib::x11_exists, string("X11_EXISTS"));

}
