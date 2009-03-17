/***************************************************************************
                          ncdf_cl.cpp  -  NetCDF GDL library function
                             -------------------
    begin                : March 24 2004
    copyright            : (C) 2004 by Christopher Lee
    email                : leec_gdl@publius.co.uk
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
// default: assume we have netCDF
#define USE_NETCDF 1
#endif
#ifdef USE_NETCDF

#include "includefirst.hpp"

#include <string>
#include <fstream>
#include <memory>
#include <gsl/gsl_sys.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_sf.h>
#include <netcdfcpp.h>

#include "datatypes.hpp"
#include "math_utl.hpp"
#include "envt.hpp"
#include "dpro.hpp"
#include "dinterpreter.hpp"
#include "ncdf_cl.hpp"
#include "terminfo.hpp"
#include "typedefs.hpp"

#define MAX_DATE_STRING_LENGTH 80

#define GDL_DEBUG
//#undef GDL_DEBUG

namespace lib {

  using namespace std;
  using namespace antlr;

  void ncdf_diminq(EnvT* e)
  {
    size_t nParam=e->NParam(4);
    
    int status;
    size_t dim_size;
    char dim_name[NC_MAX_NAME];

    DLong cdfid, dimid;
    e->AssureLongScalarPar(0, cdfid);
    e->AssureLongScalarPar(1, dimid);

    //call the ncdf library
    status=nc_inq_dim(cdfid, dimid,dim_name,&dim_size);

    //handle the error
    ncdf_handle_error(e,status,"NCDF_DIMINQ");

    //fill in the return variables
    //name
    delete e->GetParGlobal(2);
    e->GetParGlobal(2)=new DStringGDL(dim_name);

    //size
    delete e->GetParGlobal(3);
    e->GetParGlobal(3)=new DLongGDL(dim_size);
    return;

  }

  BaseGDL* ncdf_dimid(EnvT* e)
  {
    size_t nParam=e->NParam(2);

    int status, dim_id;

    DInt cdfid;
    DString dim_name;

    e->AssureScalarPar<DIntGDL>(0, cdfid);
    e->AssureStringScalarPar(1,dim_name);
    
    status=nc_inq_dimid(cdfid, dim_name.c_str(),&dim_id);
    ncdf_handle_error(e,status,"NCDF_DIMID");

    return new DLongGDL(dim_id);
  }


  BaseGDL* ncdf_dimdef(EnvT* e)
  {
    size_t nParam=e->NParam(2);

    int status, dim_id;

    DString in_string;
    DInt cdfid;
    e->AssureScalarPar<DIntGDL>(0, cdfid);

    DString dim_name;
    e->AssureStringScalarPar(1,dim_name);

    if(nParam == 3 && e->KeywordSet(0))
      {
	throw GDLException(e->CallingNode(),
			   "NCDF_DIMDEF size and unlimited error");
      } 
    else if(nParam == 2 && e->KeywordSet(0)) //unlimited
      {
	//umlimited is set
	status=nc_def_dim(cdfid,
			  dim_name.c_str(),
			  NC_UNLIMITED,
			  &dim_id);

	ncdf_handle_error(e,status,"NCDF_DIMDEF");
      }
    else if(nParam == 3 && !e->KeywordSet(0)) 
      {

	BaseGDL* s=e->GetParDefined(2);
	size_t ds;
	if(s->Type() == LONG)
	  {
	    DLong lvar;
	    e->AssureScalarPar<DLongGDL>(2, lvar);
	    ds=static_cast<size_t>(lvar);
	    
	  }
	else if(s->Type() == INT)
	  {
	    DInt ivar;
	    e->AssureScalarPar<DIntGDL>(2, ivar);
	    ds=static_cast<size_t>(ivar);
	  
}	else if(s->Type() == FLOAT)
	  {
	    DFloat fvar;
	    e->AssureScalarPar<DFloatGDL>(2, fvar);
	    ds=static_cast<size_t>(fvar);
	  }
	else if(s->Type() == DOUBLE)
	  {
	    DDouble dvar;
	    e->AssureScalarPar<DDoubleGDL>(2, dvar);
	    ds=static_cast<size_t>(dvar);
	  }
	else if(s->Type() == BYTE)
	  {
	    DByte bvar;
	    e->AssureScalarPar<DByteGDL>(2, bvar);
	    ds=static_cast<size_t>(bvar);
	  }
	else if(s->Type() == STRING)
	  {
	    DLongGDL* lsvar;
	    DString sv;
	    e->AssureScalarPar<DStringGDL>(2, sv);
	    lsvar=static_cast<DLongGDL*>(s->Convert2(LONG,BaseGDL::COPY));
	    ds=static_cast<size_t>((*lsvar)[0]);
	  }

	
	status=nc_def_dim(cdfid,
			  dim_name.c_str(),
			  ds,
			  &dim_id);

	ncdf_handle_error(e,status,"NCDF_DIMDEF");

      }

    return new DIntGDL(dim_id);
  }

  void ncdf_dimrename(EnvT* e)
  {
    size_t nParam=e->NParam(3);
    int status;
    
    //get the cdfid, which must be given.
    DString newname;
    DLong cdfid, dimid;
    e->AssureLongScalarPar(0, cdfid);

    BaseGDL* p1 = e->GetParDefined( 1);
    if (p1->Type() != STRING) {
      // Numeric
      e->AssureLongScalarPar(1, dimid);
    } else {
      // String
      DString dim_name;
      e->AssureScalarPar<DStringGDL>(1, dim_name);
      status=nc_inq_dimid(cdfid, dim_name.c_str(),&dimid);
      ncdf_handle_error(e,status,"NCDF_DIMRENAME");
    }

    e->AssureStringScalarPar(2, newname);
    
    //we have the cdfid, dimid, newname

    status=nc_rename_dim(cdfid,dimid,
			 newname.c_str());

    ncdf_handle_error(e, status,"NCDF_DIMRENAME");

    return;
  }

}
#endif
