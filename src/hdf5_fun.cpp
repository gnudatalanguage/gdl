/***************************************************************************
                          hdf5_fun.cpp  -  GSL GDL library function
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

// the following stuff needs some cleanup in order to make it nicely fit
// into the distribution. 

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
// default: assume we have HDF5
#define USE_HDF5 1
#endif

#ifdef USE_HDF5

#include "includefirst.hpp"

#include "hdf5_fun.hpp"

namespace lib {

  using namespace std;

  BaseGDL* h5f_open_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    DLong h5f_id;
    
    DString h5fFilename;
    e->AssureScalarPar<DStringGDL>( 0, h5fFilename);
    // printf("%s\n", h5fFilename.c_str());
    
    h5f_id = H5Fopen(h5fFilename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    
    return new DLongGDL( h5f_id );
  }
  
  BaseGDL* h5d_open_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong h5f_id;
    e->AssureLongScalarPar(0, h5f_id);

    DString h5dDatasetname;
    e->AssureScalarPar<DStringGDL>( 1, h5dDatasetname);

    //printf("%s\n", h5dDatasetname.c_str());
    
    DLong h5d_id;
    h5d_id = H5Dopen(h5f_id, h5dDatasetname.c_str());
    
    return new DLongGDL( h5d_id );
  }
  

  BaseGDL* h5d_get_space_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    
    DLong h5d_id;
    e->AssureLongScalarPar(0, h5d_id);
    
    DLong h5d_space_id;
    h5d_space_id = H5Dget_space( h5d_id );
    
    return new DLongGDL( h5d_space_id );
  }
  
  BaseGDL* h5s_get_simple_extent_dims_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    hsize_t dims_out[MAXRANK];
    
    DLong h5s_id;
    e->AssureLongScalarPar(0, h5s_id);
    
    int rank = H5Sget_simple_extent_ndims(h5s_id);
    
    H5Sget_simple_extent_dims(h5s_id, dims_out, NULL);
    
    dimension dim(rank);
    DLongGDL* d = new DLongGDL(dim);
    
    for(int i=0; i<rank; i++)
      (*d)[i] = dims_out[rank - 1 - i];
    return d;
  }
  

  /**
   * h5d_read_fun
   * CAUTION: compatibility only fractional
   * - Supports only one HDF5 data type
   * - Does not allow to select hyperslabs
   */
  BaseGDL* h5d_read_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    hsize_t dims_out[MAXRANK];

    hid_t h5d_id;
    e->AssureLongScalarPar(0, h5d_id);
    
    hid_t h5s_id = H5Dget_space(h5d_id);
    hid_t datatype = H5Dget_type(h5d_id);

    // determine the rank and dimension of the dataset
    int rank = H5Sget_simple_extent_ndims(h5s_id);
    H5Sget_simple_extent_dims(h5s_id, dims_out, NULL);

    // printf("rank = %d\n", rank);
    // for(int i=0; i< rank; i++) printf("dim = %d\n", dims_out[i]);

    // define hyperslab in dataset
#if (H5_VERS_MAJOR < 1) || ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR < 6)) ||  ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR == 6) && (H5_VERS_RELEASE <= 3))
    hssize_t offset[MAXRANK];
#else
    hsize_t offset[MAXRANK];
#endif

    hsize_t count[MAXRANK];
 
    for(int i=0; i<rank;i++) offset[i]=0;
    for(int i=0; i<rank;i++) count[i] = dims_out[i];

    H5Sselect_hyperslab(h5s_id, H5S_SELECT_SET, offset, NULL, count, NULL);

    // define memory dataspace
    hid_t memspace = H5Screate_simple(rank, count, NULL); 
   
    // define memory hyperslab
#if (H5_VERS_MAJOR < 1) || ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR < 6)) ||  ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR == 6) && (H5_VERS_RELEASE <= 3))
    hssize_t offset_out[MAXRANK];
#else
    hsize_t offset_out[MAXRANK];
#endif

    hsize_t count_out[MAXRANK];
    for(int i=0; i<rank; i++) offset_out[i] = 0;
    for(int i=0; i<rank; i++) count_out[i] = dims_out[i];
    H5Sselect_hyperslab(memspace, H5S_SELECT_SET, offset_out, NULL, count_out, NULL);

    SizeT count_s[MAXRANK];
    SizeT rank_s;
   
    rank_s = (SizeT) rank;
    // need to reverse indices for column major format
    for(int i=0; i<rank; i++) 
      count_s[i] = (SizeT)count_out[rank - 1  - i ];

    // create the IDL datatypes
    dimension dim(count_s, rank_s);
    //std::cout << dim << std::endl;

    // this would be the way to select different datatypes and
    // read them    
    //    if(datatype == H5T_IEEE_F64LE){
      DDoubleGDL* data_out = new DDoubleGDL(dim);
      H5Dread(h5d_id, H5T_IEEE_F64LE, memspace, h5s_id, H5P_DEFAULT, &(*data_out)[0]);

      H5Sclose(h5s_id);
      return data_out;
   // } else {
   //   printf("not supported data format %d\n", datatype);
   // }

    return new DIntGDL(-1);
  }
 
  
  void h5s_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();
    DLong h5s_id;
    e->AssureLongScalarPar( 0, h5s_id);
    H5Sclose(h5s_id);
  }
  

  void h5d_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();
    DLong h5d_id;
    e->AssureLongScalarPar( 0, h5d_id);
    
    H5Dclose(h5d_id);
  }
  

  void h5f_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();
    DLong h5f_id;
    e->AssureLongScalarPar( 0, h5f_id);
    
    H5Fclose(h5f_id);
  }
  
  //*******************************************************
  //*****  end hdf5 related routines
  //*******************************************************


} // namespace

#endif
