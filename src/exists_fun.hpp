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

#ifndef BASIC_FUN_HPP_CL
#define BASIC_FUN_HPP_CL

namespace lib {
  
 BaseGDL* eigen_exists(EnvT *e);
 BaseGDL* fftw_exists(EnvT *e);
 BaseGDL* glpk_exists(EnvT *e);
 BaseGDL* grib_exists(EnvT *e);
 BaseGDL* gshhg_exists(EnvT *e);
 BaseGDL* hdf5_exists(EnvT *e);
 BaseGDL* hdf_exists(EnvT *e);
 BaseGDL* magick_exists(EnvT *e);
 BaseGDL* ncdf_exists(EnvT *e);
 BaseGDL* openmp_exists(EnvT *e);
 BaseGDL* pnglib_exists(EnvT *e);
 BaseGDL* proj4_exists(EnvT *e);
 BaseGDL* proj4new_exists(EnvT *e);
 BaseGDL* pslib_exists(EnvT *e);
 BaseGDL* python_exists(EnvT *e);
 BaseGDL* udunits_exists(EnvT *e);
  // only exception wxwidgets_exists in "dialop.hpp"
 BaseGDL* x11_exists(EnvT *e);

} // namespace

#endif
