/***************************************************************************
                          hdf_fun.hpp  -  GSL GDL library function
                             -------------------
    begin                : Jan 20 2004
    copyright            : (C) 2004 by Joel Gales
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HDF_FUN_HPP_
#define HDF_FUN_HPP_

#include "datatypes.hpp"
#include "envt.hpp"
#include "mfhdf.h"

namespace lib {

  BaseGDL* hdf_sd_create_fun( EnvT* e);
  BaseGDL* hdf_sd_start_fun( EnvT* e);
  BaseGDL* hdf_sd_nametoindex_fun( EnvT* e);
  BaseGDL* hdf_sd_attrfind_fun( EnvT* e);
  BaseGDL* hdf_sd_select_fun( EnvT* e);
  BaseGDL* hdf_open_fun( EnvT* e);
  BaseGDL* hdf_vg_getid_fun( EnvT* e);
  BaseGDL* hdf_vg_attach_fun( EnvT* e);
  BaseGDL* hdf_vd_attach_fun( EnvT* e);
  BaseGDL* hdf_vd_find_fun( EnvT* e);
  BaseGDL* hdf_vd_read_fun( EnvT* e);
  
} // namespace

#endif
