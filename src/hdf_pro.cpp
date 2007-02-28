/***************************************************************************
                          hdf_pro.cpp  -  GSL GDL library function
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
      SDreaddata(sds_id, start, NULL, edges, (VOIDP) &(*data)[0]);
    else
      SDreaddata(sds_id, start, stride, edges, (VOIDP) &(*data)[0]);
    BaseGDL** p1 = &e->GetPar( 1);
    *p1 = data;
  }

  void hdf_sd_getdata_pro( EnvT* e)
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

    DLong sds_id;
    e->AssureScalarPar<DLongGDL>( 0, sds_id);

    DLongGDL* startKW = e->IfDefGetKWAs<DLongGDL>( 0);
    DLongGDL* strideKW = e->IfDefGetKWAs<DLongGDL>( 1);
    DLongGDL* countKW = e->IfDefGetKWAs<DLongGDL>( 2);

    status = SDgetinfo(sds_id, fieldname, &rank, dims, &dtype, &nattrs);

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

    dimension dim((SizeT *) edges, rank);


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

    DLong sds_id;
    e->AssureScalarPar<DLongGDL>( 0, sds_id);

    DLongGDL* startKW = e->IfDefGetKWAs<DLongGDL>( 0);
    DLongGDL* strideKW = e->IfDefGetKWAs<DLongGDL>( 1);
    DLongGDL* countKW = e->IfDefGetKWAs<DLongGDL>( 2);

    status = SDgetinfo(sds_id, fieldname, &rank, dims, &dtype, &nattrs);

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
    DByteGDL* p1B = static_cast<DByteGDL*>( p1);

    if (strideKW == NULL)
      SDwritedata(sds_id, start, NULL, edges, (VOIDP) &(*p1B)[0]);
    else
      SDwritedata(sds_id, start, stride, edges, (VOIDP) &(*p1B)[0]);
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
      delete (*p1L); 
      *p1L = new DLongGDL( num_datasets);
    }

    if (nParam == 3) {
      BaseGDL** p2L = &e->GetPar( 2);
      delete (*p2L); 
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

    // Write SDS dimensions array to KW
    if( e->KeywordPresent( 0)) {
      BaseGDL** dimKW = &e->GetKW( 0);
      delete (*dimKW);
      dimension dim((SizeT *) &rank, (SizeT) 1);
      *dimKW = new DLongGDL(dim, BaseGDL::NOZERO);

      memcpy(&(*(DLongGDL*) *dimKW)[0], dims, sizeof(int32)*rank);
    }

    if( e->KeywordPresent( 1)) {
      BaseGDL** hdftypeKW = &e->GetKW( 1);
      delete (*hdftypeKW); 
      *hdftypeKW = new DLongGDL( dtype);
    }

    if( e->KeywordPresent( 2)) {
      BaseGDL** nameKW = &e->GetKW( 2);
      delete(*nameKW);
      *nameKW = new DStringGDL( fieldname);
    }

    if( e->KeywordPresent( 3)) {
      BaseGDL** nattrsKW = &e->GetKW( 3);
      delete (*nattrsKW); 
      *nattrsKW = new DLongGDL( nattrs);
    }

    if( e->KeywordPresent( 4)) {
      BaseGDL** ndimsKW = &e->GetKW( 4);
      delete (*ndimsKW); 
      *ndimsKW = new DLongGDL( rank);
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
    e->AssureScalarPar<DLongGDL>( 1, attrindex);

    status = SDattrinfo(s_id, attrindex, attrname, &num_type, &count);

    if( e->KeywordPresent( 0)) {
      BaseGDL** countKW = &e->GetKW( 0);
      delete (*countKW); 
      *countKW = new DLongGDL( count);
    }

    if( e->KeywordPresent( 1)) {
      dimension dim((SizeT *) &count, 1);

      BaseGDL** dataKW = &e->GetKW( 1);
      delete (*dataKW);

      switch ( num_type) {

         case DFNT_FLOAT64: {
	   *dataKW = new DDoubleGDL(dim, BaseGDL::NOZERO);
	   SDreadattr(s_id, attrindex, (VOIDP) &(*(DDoubleGDL*) *dataKW)[0]);
	   break;
	 }

         case DFNT_FLOAT32: {
	   *dataKW = new DFloatGDL(dim, BaseGDL::NOZERO);
	   SDreadattr(s_id, attrindex, (VOIDP) &(*(DFloatGDL*) *dataKW)[0]);
	   break;
	 }
      }
    }

    if( e->KeywordPresent( 2)) {
      BaseGDL** hdftypeKW = &e->GetKW( 2);
      delete (*hdftypeKW); 
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
      delete (*nentriesKW); 
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
      delete (*nentriesKW); 
      *nentriesKW = new DLongGDL( nentries);
    }
  }


  void hdf_vg_gettrs_pro( EnvT* e)
  {
    SizeT nParam=e->NParam();
    char groupname[256];
    int32 nentries;

    DLong vg_id;
    e->AssureScalarPar<DLongGDL>( 0, vg_id);

    Vinquire(vg_id, &nentries, groupname);

    dimension dim((SizeT *) &nentries, (SizeT) 1);


    BaseGDL** p1L = &e->GetPar( 1);
    delete (*p1L);
    *p1L = new DLongGDL(dim, BaseGDL::NOZERO);

    BaseGDL** p2L = &e->GetPar( 2);
    delete (*p2L);
    *p2L = new DLongGDL(dim, BaseGDL::NOZERO);

    Vgettagrefs(vg_id, 
		(int32 *) &(*(DLongGDL*) *p1L)[0], 
		(int32 *) &(*(DLongGDL*) *p2L)[0], 
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

} // namespace
#endif
