/***************************************************************************
                          ncdf_cl.cpp  -  NetCDF GDL library function
                             -------------------
    begin                : March 24 2004
    copyright            : (C) 2004 by Christopher Lee
    email                : leec_gdl@publius.co.uk

Note: If this file fails to compile, it might be that HDF4 was installed 
after netCDF and overwrote some netCDF header files (netcdf.h) with its
version.
Solution: Just re-install netCDF.
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

#define HDF 1

#include <string>
#include <fstream>
#include <memory>
#include <gsl/gsl_sys.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_sf.h>
#include <netcdf.h>

#include "datatypes.hpp"
#include "math_utl.hpp"
#include "envt.hpp"
#include "dpro.hpp"
#include "dinterpreter.hpp"
#include "ncdf_cl.hpp"
#include "terminfo.hpp"
#include "typedefs.hpp"



//#define GDL_DEBUG
#undef GDL_DEBUG

namespace lib {

  using namespace std;
  using namespace antlr;

  template <typename T> void ncdf_att_handle_error(EnvT *e, int status, const char *function,T* data)
  {

    if(data != NULL and status!=NC_NOERR) delete data;
    ncdf_handle_error(e, status, function);
  }



  BaseGDL* ncdf_attname(EnvT* e)
  {
    size_t nParam=e->NParam(2);
    if(nParam ==3 && e->KeywordSet(0))
      {
	throw GDLException(e->CallingNode(),
			   "NCDF_ATTNAME: The error is Global + varid, not allowed, proper text to come.");
 
      }   else{

	int status;


	char att_name[NC_MAX_NAME];
	DInt cdfid, varid,attnum;
	varid=0;
	attnum=0;
	
	e->AssureScalarPar<DIntGDL>(0,  cdfid);
	e->AssureScalarPar<DIntGDL>(1, varid);



	if(e->KeywordSet(0)) 
	  {
	    attnum=varid;
	    varid=NC_GLOBAL;

	  } else {
	    e->AssureScalarPar<DIntGDL>(2, attnum);
	  }


	//get the att_name variable.

	status=nc_inq_attname(cdfid,
			      varid,
			      attnum,
			      att_name);

	//handle the error
	ncdf_handle_error(e,status,"NCDF_ATTNAME");
	
	return new DStringGDL(att_name);


      }

  }


  BaseGDL* ncdf_attinq(EnvT* e)
  {
    size_t nParam=e->NParam(2);
    if(nParam ==3 && e->KeywordSet(0))
      {
	throw GDLException(e->CallingNode(),
			   "NCDF_ATTINQ: The error is Global + varid, not allowed, proper text to come.");
 
      }   else{

	int status;

	nc_type att_type;
	size_t length;
	DString attname;
	DInt cdfid, varid;

	varid=0;
	e->AssureScalarPar<DIntGDL>(0, cdfid);

	if(e->KeywordSet(0)) 
	  {
	    e->AssureStringScalarPar(1, attname);
	    varid=NC_GLOBAL;

	  } else {
	    e->AssureScalarPar<DIntGDL>(1, varid);
	    e->AssureStringScalarPar(2, attname);
	  }


	//get the attinq data

	status=nc_inq_att(cdfid,
			  varid,
			  attname.c_str(),
			  &att_type,
			  &length);
			  
			  

	//handle the error
	ncdf_handle_error(e,status,"NCDF_ATTNAME");

	// $ is necessary here (see dstructgdl.cpp)
	DStructDesc* ncdf_attinq=new DStructDesc("$truct");
	SpDLong aLong;
	SpDString aString;
	ncdf_attinq->AddTag("DATATYPE", &aString);
	ncdf_attinq->AddTag("LENGTH",  &aLong);

	structList.push_back(ncdf_attinq);
	DStructGDL* inq=new DStructGDL(ncdf_attinq);
	
	inq->InitTag("DATATYPE",ncdf_gdl_typename(att_type));
	inq->InitTag("LENGTH", DLongGDL(length));

	return inq;
	

      }

  }

  void ncdf_attget(EnvT* e)
  {
    size_t nParam=e->NParam(2);
    if(nParam > 4) throw GDLException(e->CallingNode(),
				      "NCDF_ATTGET: Incorrect number of arguments.");
    if(nParam >3 && e->KeywordSet(0))
      {
	throw GDLException(e->CallingNode(),
			   "NCDF_ATTGET: The error is Global + varid, not allowed, proper text to come.");
      } else {

	int status;
	nc_type att_type;
	size_t length;

	DString attname;
	DInt integer,cdfid,varid;
	varid=0;
	e->AssureScalarPar<DIntGDL>(0, cdfid);

	if(e->KeywordSet(0)) 
	  {
	    e->AssureStringScalarPar(1, attname);
	    varid=NC_GLOBAL;

	  } else {
	    e->AssureScalarPar<DIntGDL>(1, varid);
	    e->AssureStringScalarPar(2, attname);
	  }


	//attname, varid, cdfid are set up

	//get the attinq data

	status=nc_inq_att(cdfid,
			  varid,
			  attname.c_str(),
			  &att_type,
			  &length);
			  
			  

	//handle the error
	ncdf_handle_error(e,status,"NCDF_ATTGET");

	if(att_type == NC_INT)
	  {
	    int *ip;
	    ip=new int[length];
	    status=nc_get_att_int(cdfid,
				  varid,
				  attname.c_str(),
				  ip
				  );
	    ncdf_att_handle_error(e,status,"NCDF_ATTGET",ip);


	    dimension dim((size_t *) &length, (size_t) 1);
	    DLongGDL* temp=new DLongGDL(dim, BaseGDL::NOZERO);
	    memcpy(&(*temp)[0],&(*ip),length*sizeof(int));	      
	    delete ip;
	    delete e->GetParGlobal(nParam-1);
	    e->GetParGlobal(nParam-1)=temp;
	  }
	else if (att_type == NC_SHORT) {
	  short *sp;
	  sp=new short[length];
	  status=nc_get_att_short(cdfid,
				  varid,
				  attname.c_str(),
				  sp
				);
	  ncdf_att_handle_error(e,status,"NCDF_ATTGET",sp);
	  
	  
	  dimension dim((size_t *) &length, (size_t) 1);
	  DIntGDL* temp=new DIntGDL(dim, BaseGDL::NOZERO);
	  
	  memcpy(&(*temp)[0],&(*sp),length*sizeof(short));	      
	  
	  delete sp;
	  delete e->GetParGlobal(nParam-1);
	  e->GetParGlobal(nParam-1)=temp;
	  
	}
	
	else if (att_type == NC_FLOAT) {
	    float *fp;
	    fp=new float[length];
	    status=nc_get_att_float(cdfid,
				  varid,
				  attname.c_str(),
				  fp
				  );
	    ncdf_att_handle_error(e,status,"NCDF_ATTGET",fp);


	    dimension dim((size_t *) &length, (size_t) 1);
	    DFloatGDL* temp=new DFloatGDL(dim, BaseGDL::NOZERO);

	    memcpy(&(*temp)[0],&(*fp),length*sizeof(float));	      
	    
	    delete fp;
	  delete e->GetParGlobal(nParam-1);
	    e->GetParGlobal(nParam-1)=temp;
	    return ;

	}
	else if (att_type == NC_DOUBLE) {
	  double *dp;
	  dp=new double[length];
	  status=nc_get_att_double(cdfid,
				   varid,
				   attname.c_str(),
				   dp
				   );
	  
	  ncdf_att_handle_error(e,status,"NCDF_ATTGET",dp);
	  
	  dimension dim((size_t *) &length, (size_t) 1);
	  DDoubleGDL* temp=new DDoubleGDL(dim, BaseGDL::NOZERO);
	  
	  memcpy(&(*temp)[0],&(*dp),length*sizeof(double));	      	 
	  
	  delete dp;
	  delete e->GetParGlobal(nParam-1);
	  e->GetParGlobal(nParam-1)=temp;
	  return ;
	} else if(att_type==NC_CHAR) {
	  char *cp;
	  cp=new char[length];
	  status=nc_get_att_text(cdfid,
				   varid,
				   attname.c_str(),
				   cp
				   );
	  
	  ncdf_att_handle_error(e,status,"NCDF_ATTGET",cp);

	  dimension dim(1, (size_t) 1);
	  DStringGDL* temp=new DStringGDL(cp);
	  //dim, BaseGDL::NOZERO);
	  //memcpy(&(*temp)[0],&(*cp),length*sizeof(char));      
	  delete cp;
	  delete e->GetParGlobal(nParam-1);
	  e->GetParGlobal(nParam-1)=temp;

	  return ;
	} else if(att_type==NC_BYTE) {
	  unsigned char *bp;
	  bp=new unsigned char[length];
	  status=nc_get_att_uchar(cdfid,
				   varid,
				   attname.c_str(),
				   bp
				   );
	  
	  ncdf_att_handle_error(e,status,"NCDF_ATTGET",bp);
	  
	  dimension dim((size_t *) &length, (size_t) 1);
	  DByteGDL* temp=new DByteGDL(dim, BaseGDL::NOZERO);
	  
	  memcpy(&(*temp)[0],&(*bp),length*sizeof(unsigned char));	      	 
	  delete bp;
	  delete e->GetParGlobal(nParam-1);
	  e->GetParGlobal(nParam-1)=temp;
	  return ;
	}

	  
      }
  }

  void ncdf_attput(EnvT* e)
  {
    size_t N_Params=e->NParam(3);
    int status, val_num;
    nc_type xtype;
    size_t length;
    
    BaseGDL* at;//name
    DString attname;
    BaseGDL* val;//value;


    //get the cdfid, which must be given.

    DInt integer,cdfid,varid;
    e->AssureScalarPar<DIntGDL>(0, cdfid);
    varid=0;


    if(e->KeywordSet(0)) 
      {
	e->AssureStringScalarPar(1, attname);
	val=e->GetParDefined(2);
	val_num=2;
	varid=NC_GLOBAL;
	
      } else {
	e->AssureScalarPar<DIntGDL>(1, varid);
	e->AssureStringScalarPar(2, attname);
	val=e->GetParDefined(3);
	val_num=3;

      }
    //we have the cdfid, varid, attname, attval here

    //determine default data type
    xtype=NC_FLOAT;
    if(val->Type() == BYTE) xtype=NC_BYTE;
    if(val->Type() == STRING) xtype=NC_CHAR;
    if(val->Type() == INT) xtype=NC_SHORT;
    if(val->Type() == LONG) xtype=NC_INT;
    if(val->Type() == FLOAT) xtype=NC_FLOAT;
    if(val->Type() == DOUBLE) xtype=NC_DOUBLE;


    if(e->KeywordSet(0))//BYTE
      xtype=NC_BYTE;
    else if(e->KeywordSet(1))//CHAR
      xtype=NC_CHAR;
    else if(e->KeywordSet(2))//DOUBLE
      xtype=NC_DOUBLE;
    else if(e->KeywordSet(3))//FLOAT
      xtype=NC_FLOAT;
    else if(e->KeywordSet(4))//LONG
      xtype=NC_INT;
    else if(e->KeywordSet(4))//SHORT
      xtype=NC_SHORT;

    if(val->Type() == BYTE)
      {

	DByteGDL * bvar=static_cast<DByteGDL*>(val);
	status=nc_put_att_uchar(cdfid,varid,
				attname.c_str(),xtype,
				val->N_Elements(),
				(const unsigned char *)&(*bvar)[0]);
      }
    else if(val->Type() == STRING)
      {
	DString cvar;
	e->AssureScalarPar<DStringGDL>(val_num,cvar);
	status=nc_put_att_text(cdfid,varid,
			       attname.c_str(),//xtype,
				cvar.length()+1,
				(char *)cvar.c_str());
      }
    else if(val->Type() == INT)
      {
	DIntGDL * ivar=static_cast<DIntGDL*>(val);
	status=nc_put_att_short(cdfid,varid,
				  attname.c_str(),xtype,
				  val->N_Elements(),&(*ivar)[0]);
      }
    else if(val->Type() == LONG)
      {
      DLongGDL * lvar=static_cast<DLongGDL*>(val);
      status=nc_put_att_int(cdfid,varid,
			      attname.c_str(),xtype,
				  val->N_Elements(),&(*lvar)[0]);
      }
    else if(val->Type() == FLOAT)
      {
	DFloatGDL * fvar=static_cast<DFloatGDL*>(val);
	status=nc_put_att_float(cdfid,varid,
				  attname.c_str(),xtype,
				  val->N_Elements(),&(*fvar)[0]);
      }
    else if(val->Type() == DOUBLE)
      {
	DDoubleGDL * dvar=static_cast<DDoubleGDL*>(val);
	status=nc_put_att_double(cdfid,varid,
				   attname.c_str(),xtype,
				   val->N_Elements(),&(*dvar)[0]);
      }
    
    ncdf_handle_error(e, status,"NCDF_ATTPUT");

return;


  }

  BaseGDL* ncdf_attcopy(EnvT* e)
  {

    size_t nParam=e->NParam(3);

    int status,add;
    //incdf
    DInt integer,incdf,outcdf,invar, outvar;
    e->AssureScalarPar<DIntGDL>(0, incdf);

    add=0;

    if(e->KeywordSet(0))	//in_global
	invar=NC_GLOBAL;

    if(e->KeywordSet(1))	//out_global
	outvar=NC_GLOBAL;


    if(e->KeywordSet(0) && e->KeywordSet(1) && nParam > 3)
      {
	throw GDLException(e->CallingNode(),
			   "NCDF_ATTCOPY: Too many variables error 1");
      }
    else if((e->KeywordSet(0) || e->KeywordSet(1)) && nParam > 4)
      {
	throw GDLException(e->CallingNode(),
			   "NCDF_ATTCOPY: Too many variables error 2");
      }
    else if(e->KeywordSet(0) && !e->KeywordSet(1) && nParam == 4)
      {
	e->AssureScalarPar<DIntGDL>(1, invar);
      }
    else if(!e->KeywordSet(0) && e->KeywordSet(1) && nParam == 4)
      {
	e->AssureScalarPar<DIntGDL>(3, outvar);
	
      }
    else if(!e->KeywordSet(0) && !e->KeywordSet(1) && nParam == 5)
      {
	e->AssureScalarPar<DIntGDL>(1, invar);
	e->AssureScalarPar<DIntGDL>(4, outvar);
      }


    //Here,
    //we have, incdf, invar, outvar


    DString name;
    if(!e->KeywordSet(0)) add=1;
    e->AssureStringScalarPar(1+add, name);

    //name^^

    
    //outcdf
    e->AssureScalarPar<DIntGDL>(2+add, outcdf);

    //All variables are done.

    status=nc_copy_att(incdf,invar, 
		       name.c_str(),
		       outcdf,outvar);

    ncdf_handle_error(e, status,"NCDF_ATTCOPY");

    if(status == NC_NOERR) return new DIntGDL(outvar);
    return new DIntGDL(-1);

  }


  void ncdf_attdel(EnvT* e)
  {
    size_t nParam=e->NParam(2);
    int status;
    
    BaseGDL* at;//name
    DString attname;
    BaseGDL* val;//value;


    //get the cdfid, which must be given.
    DInt cdfid, varid;
    e->AssureScalarPar<DIntGDL>(0, cdfid);
    varid=0;

    if(e->KeywordSet(0) && nParam == 3)
      {
	throw GDLException(e->CallingNode(),
			   "NCDF_ATTDEL: Too many variables error 1");
      }
    else if(!e->KeywordSet(0) && nParam == 2)
      {
	throw GDLException(e->CallingNode(),
			   "NCDF_ATTDEL: Not enough variables error 2");
      }
    else if(e->KeywordSet(0)) //global
      {
	e->AssureStringScalarPar(1, attname);
	varid=NC_GLOBAL;
	
      } else {
	e->AssureScalarPar<DIntGDL>(1, varid);
	e->AssureStringScalarPar(2, attname);
      }
    //we have the cdfid, varid, attname

    status=nc_del_att(cdfid,varid,attname.c_str());

    ncdf_handle_error(e, status,"NCDF_ATTDEL");

    return;
  }


  void ncdf_attrename(EnvT* e)
  {
    size_t nParam=e->NParam(3);
    int status;
    
    BaseGDL* at;//name
    DString attname;
    DString newname;
    BaseGDL* val;//value;


    //get the cdfid, which must be given.
    DInt cdfid, varid;
    e->AssureScalarPar<DIntGDL>(0, cdfid);

    varid=0;

    if(e->KeywordSet(0) && nParam == 4)
      {
	throw GDLException(e->CallingNode(),
			   "NCDF_ATTRENAME: Too many variables error 1");
      }
    else if(!e->KeywordSet(0) && nParam == 3)
      {
	throw GDLException(e->CallingNode(),
			   "NCDF_ATTRENAME: Not enough variables error 2");
      }
    else if(e->KeywordSet(0)) //global
      {
	e->AssureStringScalarPar(1, attname);
	e->AssureStringScalarPar(2, newname);
	varid=NC_GLOBAL;
	
      } else {
	e->AssureScalarPar<DIntGDL>(1, varid);
	e->AssureStringScalarPar(2, attname);
	e->AssureStringScalarPar(3, newname);
      }
    //we have the cdfid, varid, attname,newname

    status=nc_rename_att(cdfid,varid,
			 attname.c_str(),
			 newname.c_str());

    ncdf_handle_error(e, status,"NCDF_ATTRENAME");

    return;
  }


}
#endif
