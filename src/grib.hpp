/***************************************************************************
                          grib.hpp  -  GRIB GDL library function
                             -------------------
    begin                : Jan 18 2009
    copyright            : (C) 2009 by Sylwester Arabas
    email                : slayoo@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GRIB_FUN_HPP_
#define GRIB_FUN_HPP_

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

  // file related:
  BaseGDL* grib_open_file_fun(EnvT* e);
  void grib_close_file_pro(EnvT* e);
  BaseGDL* grib_count_in_file_fun(EnvT* e);

  // message related:
  BaseGDL* grib_new_from_file_fun(EnvT* e);
  void grib_release_pro(EnvT* e);
  // see comment in grib.cpp 
  // BaseGDL* grib_get_message_size_fun(EnvT* e); 
  BaseGDL* grib_clone_fun(EnvT* e);

  // data related:
  BaseGDL* grib_get_size_fun(EnvT* e);
  void grib_get_pro(EnvT* e);
  void grib_get_data_pro(EnvT* e);

} // namespace

#endif
