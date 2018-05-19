/***************************************************************************
                          hdf_fun.cpp  -  HDF4 GDL library function
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
#include "hdf_fun.hpp"

namespace lib {

  using namespace std;

  BaseGDL* hdf_open_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    DLong hdf_id;

    DString hdfFilename;
    e->AssureScalarPar<DStringGDL>( 0, hdfFilename); 
    WordExp( hdfFilename);

    static int allIx = e->KeywordIx("ALL");
    static int createIx = e->KeywordIx("CREATE");
    static int num_ddIx = e->KeywordIx("NUM_DD");
    static int rdwrIx = e->KeywordIx("RDWR");
    static int readIx = e->KeywordIx("READ");
    static int writeIx = e->KeywordIx("WRITE");

    intn access;
    if (e->KeywordSet(createIx) || e->KeywordSet(allIx)) access = DFACC_CREATE;
    else if (e->KeywordSet(writeIx) || e->KeywordSet(rdwrIx)) access = DFACC_RDWR;
    else access = DFACC_RDONLY;

    DLong n_adds = DEF_NDDS;
    e->AssureLongScalarKWIfPresent(num_ddIx, n_adds);

    hdf_id = Hopen(hdfFilename.c_str(), access, n_adds);
    Vstart(hdf_id);

    return new DLongGDL( hdf_id );
  }

  BaseGDL* hdf_ishdf( EnvT* e)
  {
    SizeT nParam=e->NParam();
    DLong hdf_id;

    DString hdfFilename;
    e->AssureScalarPar<DStringGDL>( 0, hdfFilename); 
    WordExp( hdfFilename);

    intn access = DFACC_RDONLY;

    hdf_id = Hopen(hdfFilename.c_str(), access, 0);
    if (hdf_id==-1) return new DLongGDL(0); else return new DLongGDL( 1 );
  }

  BaseGDL* hdf_vg_getid_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong hdf_id;
    e->AssureLongScalarPar( 0, hdf_id);
    DLong vg_ref;
    e->AssureLongScalarPar( 1, vg_ref);

    return new DLongGDL( Vgetid(hdf_id, vg_ref) );
  }


  BaseGDL* hdf_vg_attach_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong hdf_id;
    e->AssureScalarPar<DLongGDL>( 0, hdf_id);
    DLong vg_ref;
    e->AssureScalarPar<DLongGDL>( 1, vg_ref);

    static int readIx=e->KeywordIx("READ");
    static int writeIx=e->KeywordIx("WRITE");
    if (e->KeywordSet( readIx )) return new DLongGDL( Vattach(hdf_id, vg_ref, "r"));
    return new DLongGDL( Vattach(hdf_id, vg_ref, "w")); //other cases.
  }


  BaseGDL* hdf_vd_attach_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong hdf_id;
    e->AssureScalarPar<DLongGDL>( 0, hdf_id);
    DLong vd_ref;
    e->AssureScalarPar<DLongGDL>( 1, vd_ref);

    static int readIx=e->KeywordIx("READ");
    static int writeIx=e->KeywordIx("WRITE");
    if (e->KeywordSet( writeIx ) || vd_ref == -1)
      return new DLongGDL( VSattach(hdf_id, vd_ref, "w"));

    if (e->KeywordSet(readIx ))
      return new DLongGDL( VSattach(hdf_id, vd_ref, "r"));

    return new DLongGDL( VSattach(hdf_id, vd_ref, "r"));
  }


  BaseGDL* hdf_vd_find_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong hdf_id;
    e->AssureScalarPar<DLongGDL>( 0, hdf_id);

    DString vdataname;
    e->AssureScalarPar<DStringGDL>( 1, vdataname); 

    return new DLongGDL( VSfind(hdf_id, vdataname.c_str()));
  }


  
  template< typename T>
  void hdf_vd_read_template( EnvT* e, dimension dim, 
			      int32 vd_id, int32 *nRecs, int32 interlace)
  {
    int32 nread;
    T* data = new T( dim, BaseGDL::NOZERO);

    BaseGDL** p1 = &e->GetPar( 1);
    *p1 = data;

    nread = VSread(vd_id, (uint8 *) &(*data)[0], *nRecs, interlace);
    *nRecs = nread;
  }

  BaseGDL* hdf_vd_read_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    char *fields;
    char buffer[FIELDNAMELENMAX+2+1];
    DLong dims[2];
    DLong rank;

    DLong vd_id;
    e->AssureScalarPar<DLongGDL>( 0, vd_id);

    fields = (char *) malloc(VFnfields(vd_id)*(FIELDNAMELENMAX+1)+2);
    fields[0] = ',';

    DLong nRecs;
    DLong interlace_mode;
    DLong vdata_size;
    VSinquire(vd_id, (int32 *) &nRecs, (int32 *) &interlace_mode, 
	      &fields[1], (int32 *) &vdata_size, NULL);

    if(e->KeywordSet(0)) {
      DString Fields;
      e->AssureScalarKW< DStringGDL>( 0, Fields);

      strcpy(&fields[1], Fields.c_str());
    }
    VSsetfields(vd_id, &fields[1]);

    DLong recsize;
    recsize = (int32) VSsizeof(vd_id, &fields[1]);


    // Determine active fields in read
    strcat(fields, ",");
    DByte fieldindex[VSFIELDMAX];
    DLong nFlds = 0;
    for( SizeT i=0; i<VFnfields(vd_id); i++) {
      strcpy(buffer, ",");
      strcat(buffer, VFfieldname(vd_id, i));
      strcat(buffer, ",");
      if (strstr(fields, buffer) != NULL) {
	fieldindex[i] = 1;
	nFlds++;
      } else
	fieldindex[i] = 0;
    }
    fields[strlen(fields)-1] = 0;

    if(e->KeywordSet(1)) {
      e->AssureScalarKW< DLongGDL>( 1, nRecs);
    }


    // Determine type of first field to read
    DLong type;
    DByte diffType = 0;
    for( SizeT i=0; i<VFnfields(vd_id); i++) {
      if (fieldindex[i]) {
	type = VFfieldtype(vd_id, i);
	break;
      }
    }


    // Determine if mixed type read
    for( SizeT i=1; i<VFnfields(vd_id); i++) {
      if (fieldindex[i]) {
	if (type != VFfieldtype(vd_id, i)) {
	  diffType = 1;
	  Warning( "Warning: Mixed data types. Converting to BYTES.");
	  break;
	}
      }
    }


    // Set dimensions
    if (diffType == 1) {
      dims[0] = recsize*nRecs;
      rank = 1;
    } else if (nFlds == 1) {
      dims[0] = nRecs;
      rank = 1;
    } else {
      dims[0] = nFlds;
      dims[1] = nRecs;
      rank = 2;
    }

    dimension dim((DLong *) dims, rank);

    if (diffType == 1) {
      hdf_vd_read_template< DByteGDL>( e, dim, vd_id, 
				       (int32 *) &nRecs, interlace_mode);
    } else if (type == DFNT_FLOAT64) {
      hdf_vd_read_template< DDoubleGDL>( e, dim, vd_id, 
					 (int32 *) &nRecs, interlace_mode);
    } else if (type == DFNT_FLOAT32) {
      hdf_vd_read_template< DFloatGDL>( e, dim, vd_id, 
					(int32 *) &nRecs, interlace_mode);
    } else if (type == DFNT_INT32) {
      hdf_vd_read_template< DLongGDL>( e, dim, vd_id, 
				       (int32 *) &nRecs, interlace_mode);

      //    int32 nread;
      //DLongGDL* data = new DLongGDL( dim, BaseGDL::NOZERO);

      //BaseGDL** p1 = &e->GetPar( 1);
      //*p1 = data;

      // nread = VSread(vd_id, (uint8 *) &(*data)[0], nRecs, interlace_mode);

    } else if (type == DFNT_INT16) {
      hdf_vd_read_template< DIntGDL>( e, dim, vd_id, 
				      (int32 *) &nRecs, interlace_mode);
    }


    free(fields);
    
    /*
    field_order = VFfieldorder(vdata_id, field_index);
    field_type = VFfieldtype(vdata_id, field_index);
    */

    return new DLongGDL( nRecs );
  }

  BaseGDL* hdf_sd_start_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    DLong sd_id;

    DString hdfFilename;
    e->AssureScalarPar<DStringGDL>( 0, hdfFilename); 
    WordExp( hdfFilename);

    static int createIx = e->KeywordIx("CREATE");
    static int rdwrIx = e->KeywordIx("RDWR");
 
    intn access = DFACC_RDONLY;
    if (e->KeywordSet(rdwrIx)) access = DFACC_RDWR;
    else if (e->KeywordSet(createIx)) access = DFACC_CREATE;

    sd_id = SDstart(hdfFilename.c_str(), access);

    return new DLongGDL( sd_id );
  }


  BaseGDL* hdf_sd_create_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    DLong sd_id;
    e->AssureScalarPar<DLongGDL>( 0, sd_id);

    DString sdsname;
    e->AssureScalarPar<DStringGDL>( 1, sdsname); 

    BaseGDL* p2 = e->GetPar( 2);
    SizeT ndim = p2->N_Elements();
    DLongGDL* dims = static_cast<DLongGDL*>
      (p2->Convert2( GDL_LONG, BaseGDL::COPY));

    static int hdftypeIx=e->KeywordIx("HDF_TYPE");
    DLong hdfType=0;
    e->AssureLongScalarKWIfPresent( hdftypeIx , hdfType);

    DLong sds_id;

    // GDL_BYTE
    if (e->KeywordSet(0) || e->KeywordSet(1) || e->KeywordSet(2) ||
	hdfType == DFNT_INT8 || hdfType == DFNT_UINT8) {

      sds_id = SDcreate(sd_id, sdsname.c_str(), DFNT_UINT8, 
			ndim, (int32 *) &(*dims)[0]);

      //GDL_INT
    } else if (e->KeywordSet(3) || e->KeywordSet(4) || e->KeywordSet(5) ||
	       hdfType == DFNT_INT16) {

      sds_id = SDcreate(sd_id, sdsname.c_str(), DFNT_INT16, 
			ndim, (int32 *) &(*dims)[0]);

      // GDL_UINT
    } else if (e->KeywordSet(6) || hdfType == DFNT_UINT16) {

      sds_id = SDcreate(sd_id, sdsname.c_str(), DFNT_UINT16, 
			ndim, (int32 *) &(*dims)[0]);

      // GDL_LONG
    } else if (e->KeywordSet(7) || e->KeywordSet(8) ||
	       hdfType == DFNT_INT32) {

      sds_id = SDcreate(sd_id, sdsname.c_str(), DFNT_INT32, 
			ndim, (int32 *) &(*dims)[0]);

      // GDL_ULONG
    } else if (e->KeywordSet(9) || hdfType == DFNT_UINT32) {

      sds_id = SDcreate(sd_id, sdsname.c_str(), DFNT_UINT32, 
			ndim, (int32 *) &(*dims)[0]);

      // GDL_FLOAT
    } else if (e->KeywordSet(10) || hdfType == DFNT_FLOAT32) {

      sds_id = SDcreate(sd_id, sdsname.c_str(), DFNT_FLOAT32, 
			ndim, (int32 *) &(*dims)[0]);

      // GDL_DOUBLE
    } else if (e->KeywordSet(11) || hdfType == DFNT_FLOAT64) {

      sds_id = SDcreate(sd_id, sdsname.c_str(), DFNT_FLOAT64, 
			ndim, (int32 *) &(*dims)[0]);

      // GDL_STRING
    } else if (e->KeywordSet(12) || e->KeywordSet(13) ||
	       hdfType == DFNT_CHAR) {

      sds_id = SDcreate(sd_id, sdsname.c_str(), DFNT_CHAR, 
			ndim, (int32 *) &(*dims)[0]);

    }
    return new DLongGDL( sds_id );
  }


  BaseGDL* hdf_sd_nametoindex_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong sd_id;
    e->AssureLongScalarPar( 0, sd_id);

    DString sdsName;
    e->AssureScalarPar<DStringGDL>( 1, sdsName);

    DLong index = SDnametoindex(sd_id, sdsName.c_str());
    
    return new DLongGDL( index );
  }


  BaseGDL* hdf_sd_select_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    DLong sd_id;
    e->AssureScalarPar<DLongGDL>( 0, sd_id);
    DLong index;
    // SA: this makes it accept the argument both as a short and as a long integer (as IDL does)
    //e->AssureScalarPar<DLongGDL>( 1, index);
    e->AssureLongScalarPar(1, index);

    return new DLongGDL( SDselect(sd_id, index) );
  }

  BaseGDL* hdf_sd_dimgetid_fun( EnvT* e)
  {
    DLong sds_id, dim_index, dim_id;
    int32 rank, status;
    e->AssureScalarPar<DLongGDL>( 0, sds_id);
    e->AssureLongScalarPar(1, dim_index);

    {
      char fieldname[256];
      int32 null[MAXRANK];
      status = SDgetinfo(sds_id, fieldname, &rank, null, null, null);
    }

    if (status != 0) e->Throw("Invalid SD dataset ID: " + i2s(sds_id));

    // using reverse indices to reflect IDL behaviour
    dim_id = SDgetdimid(sds_id, rank - 1 - dim_index);
    if (dim_id == -1) e->Throw("Invalid dimension index: " + i2s(dim_index) + 
      " (valid indices range from 0 to " + i2s(rank) + ")");

    return new DLongGDL(dim_id);
  }

  BaseGDL* hdf_sd_attrfind_fun( EnvT* e)
  {
    DLong s_id;
    e->AssureLongScalarPar( 0, s_id);

    DString attrName;
    e->AssureScalarPar<DStringGDL>( 1, attrName);

    DLong index = SDfindattr(s_id, attrName.c_str());
    
    return new DLongGDL( index );
  }

} // namespace
#endif



