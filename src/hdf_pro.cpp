/***************************************************************************
                          hdf_pro.cpp  -  HDF4 GDL library function
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
// default: assume we have HDF
#define USE_HDF 1
#endif

#ifdef USE_HDF

#include "includefirst.hpp"

#include "mfhdf.h"
#include "hdf_pro.hpp"

namespace lib {

  using namespace std;

  template< typename T>
  BaseGDL* hdf_sd_getdata_template( EnvT* e, dimension dim, int32 sds_id,
				    int32 start[], int32 edges[], int32 stride[],
				    DLongGDL* strideKW)
  {
    T* data = new T( dim, BaseGDL::NOZERO);
    if (strideKW == NULL)
      SDreaddata(sds_id, start, NULL, edges, data->DataAddr());
    else
      SDreaddata(sds_id, start, stride, edges, data->DataAddr());
    BaseGDL** p1 = &e->GetPar( 1);
    *p1 = data;
  }

  void hdf_sd_getdata_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(2);
    char fieldname[256];
    int32 rank;
    int32 dims[MAXRANK];
    int32 start[MAXRANK];
    int32 edges[MAXRANK];
    int32 stride[MAXRANK];
    int32 dtype;
    int32 nattrs;
    int32 status;

    DLong sds_id;
    e->AssureScalarPar<DLongGDL>( 0, sds_id);

    DLongGDL* startKW = e->IfDefGetKWAs<DLongGDL>( 0);
    DLongGDL* strideKW = e->IfDefGetKWAs<DLongGDL>( 1);
    DLongGDL* countKW = e->IfDefGetKWAs<DLongGDL>( 2);

    status = SDgetinfo(sds_id, fieldname, &rank, dims, &dtype, &nattrs);

    if (status == FAIL)
	throw GDLException( e->CallingNode(), 
			    "HDF_SD_GETDATA: Invalid SD dataset ID: " + i2s(sds_id));

    for( SizeT i = 0; i < rank; i++) {
      start[i] = 0;
      stride[i] = 1;
      edges[i] = dims[i];
    }

    if (startKW != NULL) {
      if (startKW->N_Elements() != rank)
	throw GDLException( e->CallingNode(), 
			    "HDF_SD_GETDATA: Incorrect number of START dimensions:"
			    +i2s(startKW->N_Elements()));
      memcpy(start, &(*startKW)[0], rank*sizeof(int32));

      // Switch to C-ordering
      for( SizeT i = 0; i < rank/2; i++) {
	int32 temp = start[i];
	start[i] = start[rank-1];
	start[rank-1] = temp;
      }
    }

    if (countKW != NULL) {
      if (countKW->N_Elements() != rank)
	throw GDLException( e->CallingNode(), 
			    "HDF_SD_GETDATA: Incorrect number of COUNT dimensions:"
			    +i2s(countKW->N_Elements()));
      memcpy(edges, &(*countKW)[0], rank*sizeof(int32));

      // Switch to C-ordering
      for( SizeT i = 0; i < rank/2; i++) {
	int32 temp = edges[i];
	edges[i] = edges[rank-1];
	edges[rank-1] = temp;
      }
    }

    if (strideKW != NULL) {
      if (strideKW->N_Elements() != rank)
	throw GDLException( e->CallingNode(), 
			    "HDF_SD_GETDATA: Incorrect number of STRIDE dimensions:"
			    +i2s(strideKW->N_Elements()));
      memcpy(stride, &(*strideKW)[0], rank*sizeof(int32));

      // Switch to C-ordering
      for( SizeT i = 0; i < rank/2; i++) {
	int32 temp = stride[i];
	stride[i] = stride[rank-1];
	stride[rank-1] = temp;
      }
    }

    // Reverse order of edges for setting up GDL array(
    for( SizeT i = 0; i < rank/2; i++) {
      int32 tempedge = edges[i];
      edges[i] = edges[rank-1];
      edges[rank-1] = tempedge;
    }

    dimension dim((DLong *) edges, rank);

    // Return to original edge values for HDF read
    for( SizeT i = 0; i < rank/2; i++) {
      int32 tempedge = edges[i];
      edges[i] = edges[rank-1];
      edges[rank-1] = tempedge;
    }

    switch ( dtype) {

       case DFNT_FLOAT64: {
	 hdf_sd_getdata_template< DDoubleGDL>( e, dim, sds_id,
					       start, edges, stride, strideKW);
	 break;
       }
       case DFNT_FLOAT32: {
	 hdf_sd_getdata_template< DFloatGDL>( e, dim, sds_id,
					      start, edges, stride, strideKW);
	 break;
       }
       case DFNT_UINT32: {
	 hdf_sd_getdata_template< DULongGDL>( e, dim, sds_id,
					      start, edges, stride, strideKW);
	 break;
       }
       case DFNT_INT32: {
	 hdf_sd_getdata_template< DLongGDL>( e, dim, sds_id,
					     start, edges, stride, strideKW);
	 break;
       }
       case DFNT_UINT16: {
	 hdf_sd_getdata_template< DUIntGDL>( e, dim, sds_id,
					     start, edges, stride, strideKW);
	 break;
       }
       case DFNT_INT16: {
	 hdf_sd_getdata_template< DIntGDL>( e, dim, sds_id,
					    start, edges, stride, strideKW);
	 break;
       }
      case DFNT_UINT8:
      case DFNT_INT8: {
	 hdf_sd_getdata_template< DByteGDL>( e, dim, sds_id,
					     start, edges, stride, strideKW);
	 break;
       }
    }

  }


  void hdf_sd_adddata_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();
    char fieldname[256];
    int32 rank;
    int32 dims[MAXRANK];
    int32 start[MAXRANK];
    int32 edges[MAXRANK];
    int32 stride[MAXRANK];
    int32 dtype;
    int32 nattrs;
    int32 status;

    int32 sds_id;
    e->AssureScalarPar<DLongGDL>( 0, sds_id);

    DLongGDL* startKW = e->IfDefGetKWAs<DLongGDL>( 0);
    DLongGDL* strideKW = e->IfDefGetKWAs<DLongGDL>( 1);
    DLongGDL* countKW = e->IfDefGetKWAs<DLongGDL>( 2);

    status = SDgetinfo(sds_id, fieldname, &rank, dims, &dtype, &nattrs);

    if (status == FAIL)
	throw GDLException( e->CallingNode(), 
			    "HDF_SD_ADDDATA: Invalid SD dataset ID: " + i2s(sds_id));

    for( SizeT i = 0; i < rank; i++) {
      start[i] = 0;
      stride[i] = 1;
      edges[i] = dims[i];
    }

    if (startKW != NULL) {
      if (startKW->N_Elements() != rank)
	throw GDLException( e->CallingNode(), 
			    "HDF_SD_GETDATA: Incorrect number of START dimensions:"
			    +i2s(startKW->N_Elements()));
      memcpy(start, &(*startKW)[0], rank*sizeof(int32));
    }

    if (countKW != NULL) {
      if (countKW->N_Elements() != rank)
	throw GDLException( e->CallingNode(), 
			    "HDF_SD_GETDATA: Incorrect number of COUNT dimensions:"
			    +i2s(countKW->N_Elements()));
      memcpy(edges, &(*countKW)[0], rank*sizeof(int32));
    }

    if (strideKW != NULL) {
      if (strideKW->N_Elements() != rank)
	throw GDLException( e->CallingNode(), 
			    "HDF_SD_GETDATA: Incorrect number of STRIDE dimensions:"
			    +i2s(strideKW->N_Elements()));
      memcpy(stride, &(*strideKW)[0], rank*sizeof(int32));
    }


    BaseGDL* p1 = e->GetParDefined( 1);

    if (strideKW == NULL)
      SDwritedata(sds_id, start, NULL, edges,  p1->DataAddr());
    else
      SDwritedata(sds_id, start, stride, edges, p1->DataAddr());
  }


  void hdf_sd_fileinfo_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong sd_id;
    DLong num_datasets, num_global_attrs;

    e->AssureScalarPar<DLongGDL>( 0, sd_id);

    SDfileinfo(sd_id, (int32 *) &num_datasets, (int32 *) &num_global_attrs);

    if (nParam >= 2) {
      BaseGDL** p1L = &e->GetPar( 1);
      GDLDelete((*p1L)); 
      *p1L = new DLongGDL( num_datasets);
    }

    if (nParam == 3) {
      BaseGDL** p2L = &e->GetPar( 2);
      GDLDelete((*p2L)); 
      *p2L = new DLongGDL( num_global_attrs);
    }
  }


  void hdf_sd_getinfo_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();
    char fieldname[256];
    int32 rank;
    int32 dims[MAXRANK];
    int32 dtype;
    int32 nattrs;
    int32 status;

    DLong sds_id;
    e->AssureScalarPar<DLongGDL>( 0, sds_id);

    status = SDgetinfo(sds_id, fieldname, &rank, dims, &dtype, &nattrs);

    if (status == FAIL)
	throw GDLException( e->CallingNode(), 
			    "HDF_SD_GETINFO: Invalid SD dataset ID: " + i2s(sds_id));

    // Write SDS dimensions array to KW

    if( e->KeywordPresent( 0)) {
      BaseGDL** dimKW = &e->GetKW( 0);
      GDLDelete((*dimKW));
      dimension dim((DLong *) &rank, (SizeT) 1);
      *dimKW = new DLongGDL(dim, BaseGDL::NOZERO);

      // Reverse order of dimensions
      for( SizeT i = 0; i < rank/2; i++) {
	int32 tempdim = dims[i];
	dims[i] = dims[rank-1];
	dims[rank-1] = tempdim;
      }

      memcpy(&(*(DLongGDL*) *dimKW)[0], dims, sizeof(int32)*rank);
    }

    if( e->KeywordPresent( 1)) {
      BaseGDL** hdftypeKW = &e->GetKW( 1);
      GDLDelete((*hdftypeKW)); 
      *hdftypeKW = new DLongGDL( dtype);
    }

    if( e->KeywordPresent( 2)) {
      BaseGDL** nameKW = &e->GetKW( 2);
      delete(*nameKW);
      *nameKW = new DStringGDL( fieldname);
    }

    if( e->KeywordPresent( 3)) {
      BaseGDL** nattrsKW = &e->GetKW( 3);
      GDLDelete((*nattrsKW)); 
      *nattrsKW = new DLongGDL( nattrs);
    }

    if( e->KeywordPresent( 4)) {
      BaseGDL** ndimsKW = &e->GetKW( 4);
      GDLDelete((*ndimsKW)); 
      *ndimsKW = new DLongGDL( rank);
    }

    if( e->KeywordPresent( 5)) {
      BaseGDL** typeKW = &e->GetKW( 5);
      delete(*typeKW);

      switch ( dtype) {

      case DFNT_FLOAT64: {
	*typeKW = new DStringGDL( "DOUBLE");
	 break;
       }
       case DFNT_FLOAT32: {
	 *typeKW = new DStringGDL( "FLOAT");
	 break;
       }
       case DFNT_UINT32: {
	*typeKW = new DStringGDL( "ULONG");
	 break;
       }
       case DFNT_INT32: {
	*typeKW = new DStringGDL( "LONG");
	 break;
       }
       case DFNT_UINT16: {
	*typeKW = new DStringGDL( "UINT");
	 break;
       }
       case DFNT_INT16: {
	*typeKW = new DStringGDL( "INT");
	 break;
       }
      case DFNT_UINT8:
      case DFNT_INT8: {
	*typeKW = new DStringGDL( "BYTE");
	 break;
       }
      }

    }
  }


  void hdf_sd_attrinfo_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();
    char attrname[256];
    int32 num_type;
    int32 count;
    int32 nattrs;
    int32 status;

    DLong s_id;
    e->AssureScalarPar<DLongGDL>( 0, s_id);

    DLong attrindex;
    // SA: this makes it accept both short and long integers as IDL does
    // e->AssureScalarPar<DLongGDL>( 1, attrindex);
    e->AssureLongScalarPar( 1, attrindex);

    status = SDattrinfo(s_id, attrindex, attrname, &num_type, &count);

    if( e->KeywordPresent( 0)) {
      BaseGDL** countKW = &e->GetKW( 0);
      GDLDelete((*countKW)); 
      *countKW = new DLongGDL( count);
    }

    if( e->KeywordPresent( 1)) {
      dimension dim((DLong *) &count, 1);

      BaseGDL** dataKW = &e->GetKW( 1);
      GDLDelete((*dataKW));

      switch ( num_type) {

         case DFNT_FLOAT64: {
	   *dataKW = new DDoubleGDL(dim, BaseGDL::NOZERO);
	   SDreadattr(s_id, attrindex, (*dataKW)->DataAddr());
	   break;
	 }

         case DFNT_FLOAT32: {
	   *dataKW = new DFloatGDL(dim, BaseGDL::NOZERO);
	   SDreadattr(s_id, attrindex, (*dataKW)->DataAddr());
	   break;
	 }

         case DFNT_INT32: {
	   *dataKW = new DLongGDL(dim, BaseGDL::NOZERO);
	   SDreadattr(s_id, attrindex, (*dataKW)->DataAddr());
	   break;
	 }

         case DFNT_INT16: {
	   *dataKW = new DIntGDL(dim, BaseGDL::NOZERO);
	   SDreadattr(s_id, attrindex, (*dataKW)->DataAddr());
	   break;
	 }

         case DFNT_UINT32: {
	   *dataKW = new DULongGDL(dim, BaseGDL::NOZERO);
	   SDreadattr(s_id, attrindex, (*dataKW)->DataAddr());
	   break;
	 }

         case DFNT_UINT16: {
	   *dataKW = new DUIntGDL(dim, BaseGDL::NOZERO);
	   SDreadattr(s_id, attrindex, (*dataKW)->DataAddr());
	   break;
	 }

         case DFNT_CHAR: {
	   char* attrstr = new char[count+1];
	   memset(attrstr, 0, count+1);
	   SDreadattr(s_id, attrindex, (VOIDP) attrstr);
	   *dataKW = new DStringGDL( attrstr);
	   delete attrstr;
	 }
      }
    }

    if( e->KeywordPresent( 2)) {
      BaseGDL** hdftypeKW = &e->GetKW( 2);
      GDLDelete((*hdftypeKW)); 
      *hdftypeKW = new DLongGDL( num_type);
    }

    if( e->KeywordPresent( 3)) {
      BaseGDL** nameKW = &e->GetKW( 3);
      delete(*nameKW);
      *nameKW = new DStringGDL( attrname);
    }
  }


  void hdf_vg_getinfo_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();
    char groupname[256];
    char groupclass[256];
    int32 nentries;

    DLong vg_id;
    e->AssureScalarPar<DLongGDL>( 0, vg_id);

    if( e->KeywordPresent( 0)) {
      Vgetclass(vg_id, groupclass);
      BaseGDL** classKW = &e->GetKW( 0);
      delete(*classKW);
      *classKW = new DStringGDL( groupclass);
    }

    if( e->KeywordPresent( 1)) {
      Vgetname(vg_id, groupname);
      BaseGDL** nameKW = &e->GetKW( 1);
      delete(*nameKW);
      *nameKW = new DStringGDL( groupname);
    }

    if( e->KeywordPresent( 2)) {
      Vinquire(vg_id, &nentries, groupname);
      BaseGDL** nentriesKW = &e->GetKW( 2);
      GDLDelete((*nentriesKW)); 
      *nentriesKW = new DLongGDL( nentries);
    }
  }


  void hdf_vd_get_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();
    char vdataname[256];
    char vdataclass[256];
    int32 nentries;

    DLong vd_id;
    e->AssureScalarPar<DLongGDL>( 0, vd_id);

    if( e->KeywordPresent( 0)) {
      VSgetclass(vd_id, vdataclass);
      BaseGDL** classKW = &e->GetKW( 0);
      delete(*classKW);
      *classKW = new DStringGDL( vdataclass);
    }

    if( e->KeywordPresent( 1)) {
      VSgetname(vd_id, vdataname);
      BaseGDL** nameKW = &e->GetKW( 1);
      delete(*nameKW);
      *nameKW = new DStringGDL( vdataname);
    }

    if( e->KeywordPresent( 2)) {
      nentries = VSelts(vd_id);
      BaseGDL** nentriesKW = &e->GetKW( 2);
      GDLDelete((*nentriesKW)); 
      *nentriesKW = new DLongGDL( nentries);
    }
  }


  void hdf_vg_gettrs_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(3);
    char groupname[256];
    int32 nentries;

    DLong vg_id;
    e->AssureScalarPar<DLongGDL>( 0, vg_id);

    Vinquire(vg_id, &nentries, groupname);

    dimension dim((DLong *) &nentries, (SizeT) 1);


    BaseGDL** p1L = &e->GetPar( 1);
    GDLDelete((*p1L));
    *p1L = new DLongGDL(dim, BaseGDL::NOZERO);

    BaseGDL** p2L = &e->GetPar( 2);
    GDLDelete((*p2L));
    *p2L = new DLongGDL(dim, BaseGDL::NOZERO);

    Vgettagrefs(vg_id, 
		(int32 *) (*p1L)->DataAddr(), 
		(int32 *) (*p2L)->DataAddr(), 
		nentries);
  }


  void hdf_vg_detach_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong vg_id;
    e->AssureScalarPar<DLongGDL>( 0, vg_id);

    Vdetach(vg_id);
  }


  void hdf_vd_detach_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong vd_id;
    e->AssureScalarPar<DLongGDL>( 0, vd_id);

    VSdetach(vd_id);
  }


  void hdf_sd_endaccess_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong sds_id;
    e->AssureScalarPar<DLongGDL>( 0, sds_id);

    SDendaccess(sds_id);
  }


  void hdf_sd_end_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong sd_id;
    e->AssureScalarPar<DLongGDL>( 0, sd_id);

    SDend(sd_id);
  }


  void hdf_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong hdf_id;
    e->AssureScalarPar<DLongGDL>( 0, hdf_id);

    Vend(hdf_id);

    Hclose(hdf_id);
  }

  template< typename T>
  BaseGDL* hdf_sd_getdscl_template(EnvT* e, DLong dim_size, int32 dim_id)
  {
    T* data = new T(dimension(dim_size), BaseGDL::NOZERO);
    SDgetdimscale(dim_id, data->DataAddr());
    BaseGDL** scaleKW = &e->GetKW(2);
    *scaleKW = data;
  }

  void hdf_sd_dimget_pro( EnvT* e)
  {

    DLong dim_id, status;
    int32 n_attrs, data_type, dim_size;
    e->AssureScalarPar<DLongGDL>( 0, dim_id);

    // NAME keyword
    if (e->KeywordPresent(0)) 
    {
      char dim_name[64];
      status = SDdiminfo(dim_id, dim_name, &dim_size, &data_type, &n_attrs);
      if (status != FAIL)
      {
        BaseGDL** nameKW = &e->GetKW(0);
        delete(*nameKW);
        *nameKW = new DStringGDL(dim_name);
      }
    }
    else 
    {
      status = SDdiminfo(dim_id, NULL, &dim_size, &data_type, &n_attrs);
    }

    if (status == FAIL) throw GDLException(e->CallingNode(),
      "HDF_SD_DIMGET: Invalid dimension ID: " + i2s(dim_id));
    // ... or unselected SDS?

    // NATTR keyword
    if (e->KeywordPresent(1))
    {
      BaseGDL** nattrKW = &e->GetKW(1);
      delete(*nattrKW);
      *nattrKW = new DLongGDL(n_attrs);
    }

    // SCALE keyword
    if (e->KeywordPresent(2))
    { 
      BaseGDL** scaleKW = &e->GetKW(2);      
      delete(*scaleKW);
      // TODO: ? if dim_type == 0 scale not set
      switch (data_type) 
      {
        case DFNT_FLOAT64: hdf_sd_getdscl_template< DDoubleGDL>( e, dim_size, dim_id); break;
        case DFNT_FLOAT32: hdf_sd_getdscl_template< DFloatGDL>( e, dim_size, dim_id); break;
        case DFNT_UINT32:  hdf_sd_getdscl_template< DULongGDL>( e, dim_size, dim_id); break;
        case DFNT_INT32:   hdf_sd_getdscl_template< DLongGDL>( e, dim_size, dim_id); break;
        case DFNT_UINT16:  hdf_sd_getdscl_template< DUIntGDL>( e, dim_size, dim_id); break;
        case DFNT_INT16:   hdf_sd_getdscl_template< DIntGDL>( e, dim_size, dim_id); break;
        case DFNT_UINT8:
        case DFNT_INT8:    hdf_sd_getdscl_template< DByteGDL>( e, dim_size, dim_id); break;
      } 
    } // SCALE keyword
 
    // COUNT keyword
    if (e->KeywordPresent(3))
    {
      BaseGDL** countKW = &e->GetKW(3);
      delete(*countKW);
      *countKW = new DLongGDL(dim_size);
    }

  }

} // namespace
#endif

