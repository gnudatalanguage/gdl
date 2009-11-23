/***************************************************************************
                          grib_fun.cpp  -  GRIB GDL library function
                             -------------------
    begin                : Jan 18 2009
    copyright            : (C) 2009 by Sylwester Arabas
    email                : slayoo@users.sourceforge.net
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
// default: assume we have GRIB
#define USE_GRIB 1
#endif


#include "includefirst.hpp"
#include "grib.hpp"

#ifdef USE_GRIB
#  include "grib_api.h"
#  include <map>
#endif

namespace lib {

  using namespace std;

#ifdef USE_GRIB
  // GRIB file/handle lists implemented as a global maps
  // perhaps using the code in GRIB_API F90 interface would be better... ?
  map<DLong, grib_handle*> GribHandleList;
  map<DLong, FILE*> GribFileList;
#endif

  /*
   * GRIB: FILE RELATED FUNCTIONS AND PROCEDURES
   */

  BaseGDL* grib_open_file_fun(EnvT* e)
  {
#ifndef USE_GRIB
    e->Throw("GDL was compiled without support for GRIB");
#else
    {
      SizeT nParam = e->NParam(1);
    }

    DString filename;
    e->AssureScalarPar<DStringGDL>( 0, filename); 

    FILE* in = NULL; 
    // TODO: handle RO/RW parameters (but currently anyhow no write methods are implemented...)
    in = fopen(filename.c_str(), "r"); 
    if (!in) e->Throw("unable to open file: " + filename);

    DLong ifile = GribFileList.end()->first;
    GribFileList[ifile] = in;
 
    return new DLongGDL(ifile);
#endif
  }

  void grib_close_file_pro(EnvT* e)
  {
#ifndef USE_GRIB
    e->Throw("GDL was compiled without support for GRIB");
#else
    {
      SizeT nParam = e->NParam(1);
    }

    DLong ifile;
    e->AssureScalarPar<DLongGDL>(0, ifile); 

    if (GribFileList.find(ifile) == GribFileList.end()) e->Throw("unrecognized file id: " + i2s(ifile));

    fclose(GribFileList[ifile]); 
    GribFileList.erase(ifile);
#endif
  }

  BaseGDL* grib_count_in_file_fun(EnvT* e)
  {
#ifndef USE_GRIB
    e->Throw("GDL was compiled without support for GRIB");
#else
    {
      SizeT nParam = e->NParam(1);
    }

    DLong ifile;
    e->AssureScalarPar<DLongGDL>(0, ifile); 

    if (GribFileList.find(ifile) == GribFileList.end()) e->Throw("unrecognized file id: " + i2s(ifile));

    int count;
    grib_count_in_file(NULL, GribFileList[ifile], &count); 

    return new DIntGDL(count);
#endif
  }

  /*
   * GRIB: MESSAGE RELATED FUNCTIONS AND PROCEDURES
   */

  BaseGDL* grib_new_from_file_fun(EnvT* e)
  {
#ifndef USE_GRIB
    e->Throw("GDL was compiled without support for GRIB");
#else
    {
      SizeT nParam = e->NParam(1);
    }

    DLong ifile;
    e->AssureScalarPar<DLongGDL>(0, ifile); 

    if (GribFileList.find(ifile) == GribFileList.end()) e->Throw("unrecognized file id: " + i2s(ifile));

    grib_handle *h = NULL; 
    int err = GRIB_SUCCESS; 
    h = grib_handle_new_from_file(0, GribFileList[ifile], &err); 
    if (h == NULL) e->Throw("unable get message using file id: " + i2s(ifile) + "\n" + 
      "%   GRIB API message: " + grib_get_error_message(err) 
    );

    DLong gribid = GribHandleList.end()->first;
    GribHandleList[gribid] = h;

    return new DLongGDL(gribid);
#endif
  }

  BaseGDL* grib_clone_fun(EnvT* e)
  {
#ifndef USE_GRIB
    e->Throw("GDL was compiled without support for GRIB");
#else
    {
      SizeT nParam = e->NParam(1);
    }

    DLong gribid;
    e->AssureScalarPar<DLongGDL>(0, gribid);

    if (GribHandleList.find(gribid) == GribHandleList.end()) e->Throw("unrecognized message id: " + i2s(gribid));

    DLong gribid_new = GribHandleList.end()->first;
    GribHandleList[gribid_new] = grib_handle_clone(GribHandleList[gribid]);

    if (GribHandleList[gribid_new] == NULL) e->Throw("failed to clone message: " + i2s(gribid));

    return new DLongGDL(gribid_new);
#endif
  }

  void grib_release_pro(EnvT* e)
  {
#ifndef USE_GRIB
    e->Throw("GDL was compiled without support for GRIB");
#else
    {
      SizeT nParam = e->NParam(1);
    }

    DLong gribid;
    e->AssureScalarPar<DLongGDL>(0, gribid); 

    if (GribHandleList.find(gribid) == GribHandleList.end()) e->Throw("unrecognized message id: " + i2s(gribid));

    grib_handle_delete(GribHandleList[gribid]); 
    GribHandleList.erase(gribid);
#endif
  }

/*
  BaseGDL* grib_get_message_size_fun(EnvT* e) 
  {
#ifndef USE_GRIB
    e->Throw("GDL was compiled without support for GRIB");
#else
    {
      SizeT nParam = e->NParam(1);
    }
   
    DLong gribid;
    e->AssureScalarPar<DLongGDL>(0, gribid);
    
    if (GribHandleList.find(gribid) == GribHandleList.end()) e->Throw("unrecognized message id: " + i2s(gribid));

    e->Throw("not implemented yet...");
    //return new DLongGDL(GribHandleList[gribid]->buffer->ulength);
    // the above will not work because grib_handle structure is defined 
    // in an internal library header, not the grib_api.h. It might be 
    // possible to get this info via fortran api function but then it 
    // should be enclosed in #ifdef ... #endif as not everyone compiles 
    // GRIB API with fortran support enabled... but is it that important?
    return new DLongGDL(-1);
#endif
  }
*/

  /*
   * GRIB: DATA RELATED FUNCTIONS AND PROCEDURES
   */

  BaseGDL* grib_get_size_fun(EnvT* e)
  {
#ifndef USE_GRIB
    e->Throw("GDL was compiled without support for GRIB");
#else
    {
      SizeT nParam = e->NParam(2);
    }

    DLong gribid;
    e->AssureScalarPar<DLongGDL>(0, gribid);

    if (GribHandleList.find(gribid) == GribHandleList.end()) e->Throw("unrecognized message id: " + i2s(gribid));

    DString key;
    e->AssureScalarPar<DStringGDL>(1, key);

    size_t size;
    int err = GRIB_SUCCESS;
    if (GRIB_SUCCESS != (err = grib_get_size(GribHandleList[gribid], key.c_str(), &size)))
      e->Throw("failed to get size of: " + key + "\n%   GRIB API message: " + grib_get_error_message(err)); 

    return new DLongGDL(size);
#endif
  }

  void grib_get_pro(EnvT* e)
  {
#ifndef USE_GRIB
    e->Throw("GDL was compiled without support for GRIB");
#else
    {
      SizeT nParam = e->NParam(3);
    }

    DLong gribid;
    e->AssureScalarPar<DLongGDL>(0, gribid);
    
    if (GribHandleList.find(gribid) == GribHandleList.end()) e->Throw("unrecognized message id: " + i2s(gribid));

    DString key;
    e->AssureScalarPar<DStringGDL>(1, key);
    
    int err = GRIB_SUCCESS, type = GRIB_TYPE_UNDEFINED;
    if (GRIB_SUCCESS != (err = grib_get_native_type(GribHandleList[gribid], key.c_str(), &type)))
      e->Throw("failed to get type of: " + key + "\n%   GRIB API message: " + grib_get_error_message(err));

    size_t size;
    if (GRIB_SUCCESS != (err = grib_get_size(GribHandleList[gribid], key.c_str(), &size)))
      e->Throw("failed to get size of: " + key + "\n%   GRIB API message: " + grib_get_error_message(err));

    BaseGDL* data;
 
    switch (type) 
    {
      case GRIB_TYPE_LONG: 
      {
        if (size == 1) 
        {
          data = new DLongGDL(0);
          err = grib_get_long(GribHandleList[gribid], key.c_str(), (long*)&(*(DLongGDL*)data)[0]);
        }
        else 
        {
          data = new DLongGDL(dimension(size), BaseGDL::NOZERO); 
          err = grib_get_long_array( GribHandleList[gribid], key.c_str(), (long*)&(*(DLongGDL*)data)[0], &size);
        }
        break;
      }
      case GRIB_TYPE_DOUBLE: 
      {
        if (size == 1)
        {
          data = new DDoubleGDL(0);
          err = grib_get_double(GribHandleList[gribid], key.c_str(), (double*) &(*(DDoubleGDL*)data)[0]);
        }
        else 
        {
          data = new DDoubleGDL(dimension(size), BaseGDL::NOZERO); 
          err = grib_get_double_array( GribHandleList[gribid], key.c_str(), (double*) &(*(DDoubleGDL*)data)[0], &size);
        }
        break;
      }
/*
      case GRIB_TYPE_STRING: 
      {
        data = new DStringGDL(dim, BaseGDL::NOZERO); 
        break;
      }
      case GRIB_TYPE_BYTES: 
      {
        data = new DByteGDL(dim, BaseGDL::NOZERO);
        break;
      }
*/
      default:
        e->Throw("unsupported native data type: " + string(grib_get_type_name(type)) + " (" + i2s(type) + ")");
    }

    if (err != GRIB_SUCCESS) 
      e->Throw("failed to get value[s] of: " + key + "\n%   GRIB API message: " + grib_get_error_message(err));

    delete e->GetParGlobal(2);
    e->GetPar(2) = data;
#endif
  }

  void grib_get_data_pro(EnvT* e)
  {
#ifndef USE_GRIB
    e->Throw("GDL was compiled without support for GRIB");
#else
    {
      // no more checking is needed as it will be done by get_data()
      SizeT nParam = e->NParam(4);
    }
    
    // substituting to make use of grib_get()
    delete e->GetParGlobal(1);
    e->GetPar(1) = new DStringGDL("values");
    grib_get_pro(e);
    delete e->GetParGlobal(3);
    e->GetPar(3) = e->GetPar(2);
    
    DLong gribid;
    e->AssureScalarPar<DLongGDL>(0, gribid);

    // using the iterator from C interface to retrieve lats/lons
    int err = GRIB_SUCCESS;
    grib_iterator* iter = grib_iterator_new(GribHandleList[gribid], 0, &err);
    if (err != GRIB_SUCCESS) 
      e->Throw("failed to iterate over lat/lons\n%   GRIB API message: " + string(grib_get_error_message(err)));

    delete e->GetPar(1);
    e->GetPar(1) = new DDoubleGDL(((DDoubleGDL*)e->GetPar(3))->Size(), BaseGDL::NOZERO);
    e->GetPar(2) = new DDoubleGDL(((DDoubleGDL*)e->GetPar(3))->Size(), BaseGDL::NOZERO);

    double *lat = (double*) &(*(DDoubleGDL*)e->GetPar(1))[0];
    double *lon = (double*) &(*(DDoubleGDL*)e->GetPar(2))[0];
    double null;
    while (grib_iterator_next(iter, lat++, lon++, &null));
    grib_iterator_delete(iter);
  
#endif
  }

} // namespace

