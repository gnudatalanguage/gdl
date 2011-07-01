/***************************************************************************
                          hdf5_fun.cpp  -  HDF5 GDL library function
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

  // SA: error-handling-realted helper routines / classes
  // --------------------------------------------------------------------

  // helper routine for hdf5_error_message()
  herr_t hdf5_error_message_helper(int n, H5E_error_t *err_desc, void *msg)
  {
    // getting something better than "Inappropriate type" message
#if (H5_VERS_MAJOR < 1) || ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR <= 6))
    if (err_desc->min_num == H5E_BADTYPE) 
      *static_cast<string*>(msg) = H5Eget_major(err_desc->maj_num);
    else 
      *static_cast<string*>(msg) = H5Eget_minor(err_desc->min_num);
#else
    char* tmp;
    if (err_desc->min_num == H5E_BADTYPE) 
      tmp = H5Eget_major(err_desc->maj_num);
    else 
      tmp = H5Eget_minor(err_desc->min_num);
    *static_cast<string*>(msg) = tmp;
    free(tmp);
#endif
  }

  // returns a meaningful message describing last HDF5 error
  // usual usege: 
  //   if (H5X_xxx(...) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  string hdf5_error_message(string &msg)
  {
    H5Ewalk(H5E_WALK_UPWARD, hdf5_error_message_helper, &msg); 
    return msg;
  }

  // auto_ptr-like class for guarding HDF5 spaces
  // usage: 
  //   hid_t h5s_id = H5Dget_space(h5d_id);
  //   if (h5s_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  //   hdf5_space_guard h5s_id_guard = hdf5_space_guard(h5s_id);
  class hdf5_space_guard 
  {
    hid_t space;
    public: 
    hdf5_space_guard(hid_t space_) { space = space_; }
    ~hdf5_space_guard() { H5Sclose(space); }
  };

  // auto_ptr-like class for guarding HDF5 types
  // usage: 
  //   hid_t datatype = H5Dget_type(h5d_id);
  //   if (datatype < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  //   hdf5_type_guard datatype_guard = hdf5_type_guard(datatype);
  class hdf5_type_guard 
  {
    hid_t type;
    public: 
    hdf5_type_guard(hid_t type_) { type = type_; }
    ~hdf5_type_guard() { H5Tclose(type); }
  };
  // --------------------------------------------------------------------

  BaseGDL* h5_get_libversion_fun( EnvT* e)
  {
    unsigned int majnum, minnum, relnum;
    if (H5get_libversion(&majnum, &minnum, &relnum) < 0)
      { string msg; e->Throw(hdf5_error_message(msg)); }
    return new DStringGDL(i2s(majnum) + "." + i2s(minnum) + "." + i2s(relnum));
  }

  BaseGDL* h5f_is_hdf5_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    BaseGDL* id;
    try
    {
      id = h5f_open_fun(e);
    }
    catch (GDLException ex)
    {
      return new DLongGDL(0);
    }
    if (H5Sclose((*static_cast<DLongGDL*>(id))[0]) < 0)
      { string msg; e->Throw(hdf5_error_message(msg)); }
    return new DLongGDL(1);
  }


  BaseGDL* h5f_open_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    DLong h5f_id;
    
    DString h5fFilename;
    e->AssureScalarPar<DStringGDL>( 0, h5fFilename);
    WordExp( h5fFilename);
    
    h5f_id = H5Fopen(h5fFilename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (h5f_id < 0) 
    { 
      string msg; 
      e->Throw(hdf5_error_message(msg)); 
    }
    
    return new DLongGDL( h5f_id );
  }

  
  BaseGDL* h5g_open_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(2);

    DLong h5f_id;
    e->AssureLongScalarPar(0, h5f_id);

    DString h5gGroupname;
    e->AssureScalarPar<DStringGDL>( 1, h5gGroupname);

    DLong h5g_id;
    h5g_id = H5Gopen(h5f_id, h5gGroupname.c_str());
    if (h5g_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    
    return new DLongGDL( h5g_id );
  }

  
  BaseGDL* h5d_open_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(2);

    DLong h5f_id;
    e->AssureLongScalarPar(0, h5f_id);

    DString h5dDatasetname;
    e->AssureScalarPar<DStringGDL>( 1, h5dDatasetname);

    DLong h5d_id;
    h5d_id = H5Dopen(h5f_id, h5dDatasetname.c_str());
    if (h5d_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    
    return new DLongGDL( h5d_id );
  }


  BaseGDL* h5a_open_idx_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(2);

    DLong h5f_id, attr_idx;
    e->AssureLongScalarPar(0, h5f_id);
    e->AssureLongScalarPar(1, attr_idx);

    DLong h5a_id;
    h5a_id = H5Aopen_idx(h5f_id, attr_idx);
    if (h5a_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    
    return new DLongGDL( h5a_id );
  }


  BaseGDL* h5a_get_name_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    DLong h5a_id;
    e->AssureLongScalarPar(0, h5a_id);

    // querying for the length of the name
    char tmp;
    size_t len = H5Aget_name(h5a_id, 1, &tmp);
    if (len < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    // acquireing the name
    len++;
    char* name = static_cast<char*>(malloc(len * sizeof(char)));
    if (name == NULL) e->Throw("Failed to allocate memory!");
    if (H5Aget_name(h5a_id, len, name) < 0) 
    { 
      free(name);
      { string msg; e->Throw(hdf5_error_message(msg)); }
    }
    DStringGDL* ret = new DStringGDL(name);
    free(name);
    return ret;
  }


  BaseGDL* h5a_get_type_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    
    DLong h5a_id;
    e->AssureLongScalarPar(0, h5a_id);

    DLong h5a_type_id;
    h5a_type_id = H5Aget_type( h5a_id );
    if (h5a_type_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    
    return new DLongGDL( h5a_type_id );
  }


  BaseGDL* h5a_open_name_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(2);

    DLong h5f_id;
    e->AssureLongScalarPar(0, h5f_id);

    DString h5aAttrname;
    e->AssureScalarPar<DStringGDL>( 1, h5aAttrname);

    DLong h5a_id;
    h5a_id = H5Aopen_name(h5f_id, h5aAttrname.c_str());
    if (h5a_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    
    return new DLongGDL( h5a_id );
  }
  

  BaseGDL* h5d_get_space_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    
    DLong h5d_id;
    e->AssureLongScalarPar(0, h5d_id);
    
    DLong h5d_space_id;
    h5d_space_id = H5Dget_space( h5d_id );
    if (h5d_space_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    
    return new DLongGDL( h5d_space_id );
  }

  
  BaseGDL* h5a_get_space_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    
    DLong h5a_id;
    e->AssureLongScalarPar(0, h5a_id);
    
    DLong h5a_space_id;
    h5a_space_id = H5Aget_space( h5a_id );
    if (h5a_space_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    
    return new DLongGDL( h5a_space_id );
  }


  BaseGDL* h5a_get_num_attrs_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    
    DLong loc_id;
    e->AssureLongScalarPar(0, loc_id);
    
    int num = H5Aget_num_attrs( loc_id );
    if (num < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    
    return new DLongGDL( num );
  }


  BaseGDL* h5d_get_type_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    
    DLong h5d_id;
    e->AssureLongScalarPar(0, h5d_id);
    
    DLong h5d_type_id;
    h5d_type_id = H5Dget_type( h5d_id );
    if (h5d_type_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    
    return new DLongGDL( h5d_type_id );
  }


  BaseGDL* h5t_get_size_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    
    DLong h5t_id;
    e->AssureLongScalarPar(0, h5t_id);
    
    size_t size = H5Tget_size( h5t_id );
    if (size == 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    
    return new DLongGDL( size );
    
  }


  BaseGDL* h5s_get_simple_extent_dims_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    hsize_t dims_out[MAXRANK];
    
    DLong h5s_id;
    e->AssureLongScalarPar(0, h5s_id);
    
    int rank = H5Sget_simple_extent_ndims(h5s_id);
    if (rank < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    
    if (H5Sget_simple_extent_dims(h5s_id, dims_out, NULL) < 0)
      { string msg; e->Throw(hdf5_error_message(msg)); }
    
    dimension dim(rank);
    DLongGDL* d = new DLongGDL(dim);
    
    for(int i=0; i<rank; i++)
      (*d)[i] = dims_out[rank - 1 - i];
    return d;
  }


  BaseGDL* h5a_read_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    hsize_t dims_out[MAXRANK];

    hid_t h5a_id;
    e->AssureLongScalarPar(0, h5a_id);
    
    hid_t h5s_id = H5Aget_space(h5a_id);
    if (h5s_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    hdf5_space_guard h5s_id_guard = hdf5_space_guard(h5s_id);

    hid_t datatype = H5Aget_type(h5a_id);
    if (datatype < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    hdf5_type_guard datatype_guard = hdf5_type_guard(datatype);

    // determine the rank and dimension of the dataset
    int rank = H5Sget_simple_extent_ndims(h5s_id);
    if (rank < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    if (H5Sget_simple_extent_dims(h5s_id, dims_out, NULL) < 0) 
      { string msg; e->Throw(hdf5_error_message(msg)); }

    // need to reverse indices for column major format
    SizeT count_s[MAXRANK];
    for(int i=0; i<rank; i++) 
      count_s[i] = (SizeT)dims_out[rank - 1  - i ];

    // create the IDL datatypes
    dimension dim(count_s, rank);

    // TODO!
    // this would be the way to select different datatypes and
    // read them    
    //    if(datatype == H5T_IEEE_F64LE){
      DDoubleGDL* data_out = new DDoubleGDL(dim);
      if (H5Aread(h5a_id, H5T_IEEE_F64LE, &(*data_out)[0]) < 0)
        { string msg; e->Throw(hdf5_error_message(msg)); }

      return data_out;
   // } else {
   //   printf("not supported data format %d\n", datatype);
   // }

    return new DIntGDL(-1);
  }
  

  /**
   * h5d_read_fun
   * CAUTION: compatibility only fractional
   * - Supports only one HDF5 data type
   * - Does not allow to select hyperslabs
   */
  BaseGDL* h5d_read_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    hsize_t dims_out[MAXRANK];

    hid_t h5d_id;
    e->AssureLongScalarPar(0, h5d_id);
    
    hid_t h5s_id = H5Dget_space(h5d_id);
    if (h5s_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    hdf5_space_guard h5s_id_guard = hdf5_space_guard(h5s_id);

    hid_t datatype = H5Dget_type(h5d_id);
    if (datatype < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    hdf5_type_guard datatype_guard = hdf5_type_guard(datatype);

    // determine the rank and dimension of the dataset
    int rank = H5Sget_simple_extent_ndims(h5s_id);
    if (rank < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    if (H5Sget_simple_extent_dims(h5s_id, dims_out, NULL) < 0) 
      { string msg; e->Throw(hdf5_error_message(msg)); }

    // define hyperslab in dataset
#if (H5_VERS_MAJOR < 1) || ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR < 6)) ||  ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR == 6) && (H5_VERS_RELEASE <= 3))
    hssize_t offset[MAXRANK];
#else
    hsize_t offset[MAXRANK];
#endif
    hsize_t count[MAXRANK];
 
    for(int i=0; i<rank;i++) offset[i]=0;
    for(int i=0; i<rank;i++) count[i] = dims_out[i];

    if (H5Sselect_hyperslab(h5s_id, H5S_SELECT_SET, offset, NULL, count, NULL) < 0)
      { string msg; e->Throw(hdf5_error_message(msg)); }

    // define memory dataspace
    hid_t memspace = H5Screate_simple(rank, count, NULL); 
    if (memspace < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    hdf5_space_guard memspace_guard = hdf5_space_guard(memspace);
   
    // define memory hyperslab
#if (H5_VERS_MAJOR < 1) || ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR < 6)) ||  ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR == 6) && (H5_VERS_RELEASE <= 3))
    hssize_t offset_out[MAXRANK];
#else
    hsize_t offset_out[MAXRANK];
#endif
    hsize_t count_out[MAXRANK];
    for(int i=0; i<rank; i++) offset_out[i] = 0;
    for(int i=0; i<rank; i++) count_out[i] = dims_out[i];
    if (H5Sselect_hyperslab(memspace, H5S_SELECT_SET, offset_out, NULL, count_out, NULL) < 0)
      { string msg; e->Throw(hdf5_error_message(msg)); }

    SizeT count_s[MAXRANK];
    SizeT rank_s;
   
    rank_s = (SizeT) rank;
    // need to reverse indices for column major format
    for(int i=0; i<rank; i++) 
      count_s[i] = (SizeT)count_out[rank - 1  - i ];

    // create the IDL datatypes
    dimension dim(count_s, rank_s);
    //std::cout << dim << std::endl;

    // TODO!
    // this would be the way to select different datatypes and
    // read them    
    //    if(datatype == H5T_IEEE_F64LE){
      DDoubleGDL* data_out = new DDoubleGDL(dim);
      if (H5Dread(h5d_id, H5T_IEEE_F64LE, memspace, h5s_id, H5P_DEFAULT, &(*data_out)[0]) < 0)
        { string msg; e->Throw(hdf5_error_message(msg)); }

      return data_out;
   // } else {
   //   printf("not supported data format %d\n", datatype);
   // }

    return new DIntGDL(-1);
  }
 
  
  void h5s_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    DLong h5s_id;
    e->AssureLongScalarPar( 0, h5s_id);
    if (H5Sclose(h5s_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }
  

  void h5d_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    DLong h5d_id;
    e->AssureLongScalarPar( 0, h5d_id);
    if (H5Dclose(h5d_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }
  

  void h5f_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    DLong h5f_id;
    e->AssureLongScalarPar( 0, h5f_id);
    if (H5Fclose(h5f_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }


  void h5t_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    DLong h5t_id;
    e->AssureLongScalarPar( 0, h5t_id);
    if (H5Tclose(h5t_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }

  
  void h5g_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    DLong h5g_id;
    e->AssureLongScalarPar( 0, h5g_id);
    if (H5Gclose(h5g_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }


  void h5a_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    DLong h5a_id;
    e->AssureLongScalarPar( 0, h5a_id);
    if (H5Aclose(h5a_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }


} // namespace

#endif
