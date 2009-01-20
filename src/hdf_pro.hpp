/***************************************************************************
                          hdf_pro.hpp  -  HDF4 GDL library function
                             -------------------
    begin                : Jan 20 2004
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

#ifndef HDF_PRO_HPP_
#define HDF_PRO_HPP_

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

  void hdf_sd_getdata_pro( EnvT* e);
  void hdf_sd_adddata_pro( EnvT* e);
  void hdf_sd_fileinfo_pro( EnvT* e);
  void hdf_sd_getinfo_pro( EnvT* e);
  void hdf_sd_attrinfo_pro( EnvT* e);
  void hdf_sd_dimget_pro( EnvT* e);
  void hdf_sd_endaccess_pro( EnvT* e);
  void hdf_sd_end_pro( EnvT* e);
  void hdf_close_pro( EnvT* e);
  void hdf_vg_getinfo_pro( EnvT* e);
  void hdf_vg_gettrs_pro( EnvT* e);
  void hdf_vg_detach_pro( EnvT* e);
  void hdf_vd_detach_pro( EnvT* e);
  void hdf_vd_get_pro( EnvT* e);

} // namespace

#endif
