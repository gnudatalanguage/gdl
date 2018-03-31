/***************************************************************************
                   exists_fun.hpp  -  basic GDL library functions
                             -------------------
    begin                : December 2017
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
// default: assume we have ImageMagick
#define USE_MAGICK 1
#endif

#include "includefirst.hpp"

#include <string>
#include <fstream>
#include <memory>

#include "initsysvar.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
//#include "dpro.hpp"
//#include "dinterpreter.hpp"
#include "basic_fun_cl.hpp"
//#include "terminfo.hpp"

#define MAX_DATE_STRING_LENGTH 80

#ifdef _MSC_VER
#include "gtdhelper.hpp"
#else
#include <sys/time.h>
#endif

namespace lib {

  using namespace std;
  using namespace antlr;

  BaseGDL* eigen_exists(EnvT* e)
  {
#ifdef USE_EIGEN
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* fftw_exists(EnvT *e)
  {
#ifdef USE_FFTW
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* glpk_exists(EnvT *e)
  {
#ifdef USE_GLPK
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* grib_exists(EnvT *e)
  {
#ifdef USE_GRIB
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* gshhg_exists( EnvT* e )
  {
#ifdef USE_GSHHS
    //    e->Message( "GDL was compiled with support for GSHHG" );
    return new DIntGDL(1);
#else
    //e->Message( "GDL was compiled without support for GSHHG" );
    return new DIntGDL(0);
#endif
  }
  
  BaseGDL* hdf_exists(EnvT* e)
  {
#ifdef USE_HDF
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }
  
  BaseGDL* hdf5_exists(EnvT* e)
  {
#ifdef USE_HDF5
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* magick_exists(EnvT *e)
  {
#ifdef USE_MAGICK
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* ncdf_exists(EnvT* e)
  {
#ifdef USE_NETCDF
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* ncdf4_exists(EnvT* e)
  {
#ifdef USE_NETCDF4
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* openmp_exists(EnvT* e)
  {
#ifdef USE_OPENMP
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* pnglib_exists(EnvT* e)
  {
#ifdef USE_PNGLIB
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* proj4_exists( EnvT* e )
  {
#if defined(USE_LIBPROJ4)
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }
  
  BaseGDL* proj4new_exists( EnvT* e )
  {
#if defined(USE_LIBPROJ4_NEW)
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* pslib_exists( EnvT* e )
  {
#if defined(USE_PSLIB)
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* python_exists( EnvT* e )
  {
#if defined(USE_PYTHON)
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* udunits_exists( EnvT* e )
  {
#if defined(USE_UDUNITS)
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* x11_exists( EnvT* e )
  {
#if defined(USE_X)
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

}
 // namespace
