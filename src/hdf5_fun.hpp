/***************************************************************************
                          hdf5_fun.hpp  -  HDF5 GDL library function
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

#ifndef HDF5_FUN_HPP_
#define HDF5_FUN_HPP_

#include "envt.hpp"

/**
 * needs the hdf5 library, obtainable from http://hdf.ncsa.uiuc.edu/HDF5/
 */
#define H5_USE_16_API 1
#include <H5version.h>
#include "hdf5.h"

/*
// We stay in 1.8 version, using the "1" old interface
#define H5Dopen H5Dopen1
#define H5Gopen H5Gopen1
#define H5Ewalk H5Ewalk1
*/

namespace lib {

  /**
   * wrapper routines for hdf5 function calls. Only minimal set currently
   * implemented in order to get things going. The rest is bare-bones work
   */

  BaseGDL* h5f_create_fun( EnvT* e);

  BaseGDL* h5f_open_fun( EnvT* e);
  BaseGDL* h5d_open_fun( EnvT* e);
  BaseGDL* h5d_read_fun( EnvT* e);
  BaseGDL* h5d_get_space_fun( EnvT* e);
  BaseGDL* h5s_get_simple_extent_ndims_fun( EnvT* e);
  BaseGDL* h5s_get_simple_extent_dims_fun( EnvT* e);
  BaseGDL* h5s_create_scalar_fun( EnvT* e);
  BaseGDL* h5s_create_simple_fun( EnvT* e);
  void h5s_select_hyperslab_pro( EnvT* e);
  void h5f_close_pro( EnvT* e);
  void h5d_write_pro( EnvT* e);
  void h5d_extend_pro( EnvT* e);
  void h5d_close_pro( EnvT* e);
  void h5s_close_pro( EnvT* e);

  // SA:
  BaseGDL* h5f_is_hdf5_fun( EnvT* e);
  BaseGDL* h5_get_libversion_fun( EnvT* e);
  BaseGDL* h5d_get_type_fun( EnvT* e);
  BaseGDL* h5d_create_fun( EnvT* e);
  BaseGDL* h5d_get_storage_size_fun( EnvT* e);
  BaseGDL* h5t_get_size_fun( EnvT* e);
  BaseGDL* h5t_array_create_fun( EnvT* e);
  BaseGDL* h5t_idl_create_fun( EnvT* e);
  BaseGDL* h5a_create_fun( EnvT* e);
  BaseGDL* h5a_open_name_fun( EnvT* e);
  BaseGDL* h5a_open_idx_fun( EnvT* e);
  BaseGDL* h5a_get_name_fun( EnvT* e);
  BaseGDL* h5a_get_space_fun( EnvT* e);
  BaseGDL* h5a_get_type_fun( EnvT* e);
  BaseGDL* h5a_get_num_attrs_fun( EnvT* e);
  BaseGDL* h5a_read_fun( EnvT* e);
  BaseGDL* h5g_open_fun( EnvT* e);
  BaseGDL* h5g_create_fun( EnvT* e);
  BaseGDL* h5g_get_nmembers_fun( EnvT* e);
  BaseGDL* h5g_get_member_name_fun( EnvT* e);
  BaseGDL* h5g_get_objinfo_fun( EnvT* e);
  BaseGDL* h5g_get_comment_fun( EnvT* e);
  BaseGDL* h5g_get_num_objs_fun( EnvT* e);
  BaseGDL* h5g_get_obj_name_by_idx_fun( EnvT* e);
  BaseGDL* h5g_get_linkval_fun( EnvT* e);
  BaseGDL* h5i_get_file_id_fun( EnvT* e);
  BaseGDL* h5i_get_type_fun( EnvT* e);
  void h5a_write_pro( EnvT* e );
  void h5a_delete_pro( EnvT* e );
  void h5a_close_pro( EnvT* e );
  void h5t_close_pro( EnvT* e );
  void h5g_close_pro( EnvT* e );
  void h5g_set_comment_pro( EnvT* e );
  void h5g_link_pro( EnvT* e );
  void h5g_move_pro( EnvT* e );
  void h5g_unlink_pro( EnvT* e );

} // namespace

#endif

