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

#include "includefirst.hpp"

#include <string>
#include <fstream>

#include "envt.hpp"
#include "dpro.hpp"

#include "basic_fun.hpp"
#include "basic_pro.hpp"

#ifdef USE_HDF5
#include "hdf5_fun.hpp"
#endif

#include "mpi.hpp"

using namespace std;

void LibInit_mes()
{



  const char KLISTEND[] = "";
  
  // general procedures/functions 
  const string strtokKey[]={"EXTRACT","ESCAPE","LENGTH",
			    "PRESERVE_NULL","REGEX","COUNT",KLISTEND};
  new DLibFun(lib::strtok_fun, string("STRTOK"), 2, strtokKey);


  new DLibPro(lib::setenv_pro, string("SETENV"), 1);

  const string getenvKey[]={"ENVIRONMENT", KLISTEND};
  new DLibFun(lib::getenv_fun, string("GETENV"), 1, getenvKey);
  
  const string tagNamesKey[] = {"STRUCTURE_NAME", KLISTEND};
  new DLibFun(lib::tag_names_fun, string("TAG_NAMES"), 1, tagNamesKey);

  const string stregexKey[] = {"BOOLEAN", "EXTRACT", "LENGTH",
         "SUBEXPR", "FOLD_CASE", KLISTEND};
  new DLibFun(lib::stregex_fun, string("STREGEX"), 2, stregexKey);
 
  const string structAssignKey[] = {"NOZERO", "VERBOSE", KLISTEND};
  new DLibPro(lib::struct_assign_pro, string("STRUCT_ASSIGN"), 2, structAssignKey);

  //  new DLibPro(lib::wait_pro, string("WAIT"), 1);

  //  new DLibFun(lib::clock_fun, string("CLOCK"),0);

#ifdef USE_MPI
  // MPI Functions/Procedures
  const string mpiSendKey[] = {"DEST", "TAG", KLISTEND};
  new DLibPro(lib::mpi_send_pro, string("MPIDL_SEND"), 1, mpiSendKey);

  const string mpiRecvKey[] = {"SOURCE", "TAG", "COUNT", KLISTEND};
  new DLibFun(lib::mpi_recv_fun, string("MPIDL_RECV"), 0, mpiRecvKey);

  const string mpiAllreduceKey[] = {"SUM", "PRODUCT", "COUNT", KLISTEND};
  new DLibFun(lib::mpi_allreduce_fun, string("MPIDL_ALLREDUCE"), 1, mpiAllreduceKey);

  new DLibFun(lib::mpi_comm_rank_fun, string("MPIDL_COMM_RANK"), 0);
  new DLibFun(lib::mpi_comm_size_fun, string("MPIDL_COMM_SIZE"), 0);

  new DLibPro(lib::mpi_finalize_pro, string("MPIDL_FINALIZE"), 0);
#endif

#ifdef USE_HDF5
  // hdf5 procedures/functions 
  new DLibFun(lib::h5f_open_fun, string("H5F_OPEN"), 1);
  new DLibFun(lib::h5d_open_fun, string("H5D_OPEN"), 2);
  new DLibFun(lib::h5d_read_fun, string("H5D_READ"), 1); // TODO: 2nd argument & keywords
  new DLibFun(lib::h5d_get_space_fun, string("H5D_GET_SPACE"), 1);
  new DLibFun(lib::h5s_get_simple_extent_dims_fun,
	       string("H5S_GET_SIMPLE_EXTENT_DIMS"), 1);
  new DLibPro(lib::h5f_close_pro,string("H5F_CLOSE"),1);
  new DLibPro(lib::h5d_close_pro,string("H5D_CLOSE"),1);
  new DLibPro(lib::h5s_close_pro,string("H5S_CLOSE"),1);

  // SA:
  new DLibFun(lib::h5f_is_hdf5_fun, string("H5F_IS_HDF5"), 1);
  new DLibFun(lib::h5_get_libversion_fun, string("H5_GET_LIBVERSION"), 0);
  new DLibFun(lib::h5d_get_type_fun, string("H5D_GET_TYPE"), 1);
  new DLibFun(lib::h5t_get_size_fun, string("H5T_GET_SIZE"), 1);
  new DLibFun(lib::h5a_open_name_fun, string("H5A_OPEN_NAME"), 2);
  new DLibFun(lib::h5a_open_idx_fun, string("H5A_OPEN_IDX"), 2);
  new DLibFun(lib::h5a_get_name_fun, string("H5A_GET_NAME"), 1);
  new DLibFun(lib::h5a_get_space_fun, string("H5A_GET_SPACE"), 1);
  new DLibFun(lib::h5a_get_type_fun, string("H5A_GET_TYPE"), 1);
  new DLibFun(lib::h5a_get_num_attrs_fun, string("H5A_GET_NUM_ATTRS"), 1);
  new DLibFun(lib::h5a_read_fun, string("H5A_READ"), 1); // TODO: 2nd argument
  new DLibPro(lib::h5a_close_pro, string("H5A_CLOSE"), 1);
  new DLibPro(lib::h5t_close_pro, string("H5T_CLOSE"), 1);
  new DLibPro(lib::h5g_close_pro, string("H5G_CLOSE"), 1);
  new DLibFun(lib::h5g_open_fun, string("H5G_OPEN"), 2);

  // SA: disabling the default HDF5 error handler (error handling in hdf5_fun.cpp)
  H5Eset_auto(NULL, NULL);
#endif
  
}

