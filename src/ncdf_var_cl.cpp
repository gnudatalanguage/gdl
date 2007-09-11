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



#define GDL_DEBUG
//#undef GDL_DEBUG

namespace lib {



  using namespace std;
  using namespace antlr;
  extern bool ncdf_verbose;
  
  void exceed_message(const char * name,int index, int set)
  {
    int mema=3;
    string mess;
    mess=name;
    mess+="Limit exceeded on index ";
    char * n=new char(mema);
    while (snprintf(n, sizeof n, "%d", index) >= sizeof n)
      {   delete n; mema++; n=new char(mema);
      }
    mess+=n;
    delete n;
    
    mess+=", setting to ";
    mema=3;
    n=new char(mema);
    while (snprintf(n, sizeof n, "%d", set) >= sizeof n)
      {
	delete n;mema++; n=new char(mema);
      }
    mess+=n;
    delete n;
    mess+=".";
    Message(mess);

  }

  void negzero_message(const char * name,int index, int set)
  {
    int mema=3;
    string mess;
    mess=name;
    mess+="Value of index";
    char * n=new char(mema);
    while (snprintf(n, sizeof n, "%d", index) >= sizeof n)
      {   delete n; mema++; n=new char(mema);
      }
    mess+=n;
    delete n;
    if(set > 0) 
      mess+=" is negative or zero, setting to ";
    else if(set == 0) 
      mess+=" is negative , setting to ";
    else
      mess+=" INTERNAL ERROR NCDF_VAR_CL.CPP negzero_message";
    
    mema=3;
    n=new char(mema);
    while (snprintf(n, sizeof n, "%d", set) >= sizeof n)
      {
	delete n;mema++; n=new char(mema);
      }
    mess+=n;
    delete n;
    mess+=".";
    Message(mess);

  }


  template <typename T> void ncdf_var_handle_error(EnvT *e, int status, const char *function,T* data)
  {

    if(data != NULL and status!=NC_NOERR) delete data;
    ncdf_handle_error(e, status, function);
  }
  

  BaseGDL* ncdf_varinq(EnvT* e)
  {
    size_t nParam=e->NParam(2);


    int status,var_ndims,var_dims[NC_MAX_VAR_DIMS],var_natts;
    nc_type var_type;
    char var_name[NC_MAX_NAME];

    BaseGDL* n;

    DLong cdfid, varid;
    e->AssureLongScalarPar(0, cdfid);

    // Check type of varid
    BaseGDL* p1 = e->GetParDefined( 1);
    if (p1->Type() != STRING) {
      // Numeric
      e->AssureLongScalarPar(1, varid);
    } else {
      // String
      DString var_name;
      e->AssureScalarPar<DStringGDL>(1, var_name);
      status=nc_inq_varid(cdfid, var_name.c_str(), &varid);
      ncdf_handle_error(e,status,"NCDF_VARINQ");
    }

    //call the ncdf library
    status=nc_inq_var(cdfid,varid,var_name,&var_type,&var_ndims,
		      var_dims,&var_natts);
    
    //handle the error
    ncdf_handle_error(e,status,"NCDF_VARINQ");

    //build the structure template
    DStructDesc* ncdf_varinq=new DStructDesc("$truct");
    SpDLong aLong;
    SpDString aString;
    ncdf_varinq->AddTag("NAME", &aString);
    ncdf_varinq->AddTag("DATATYPE", &aString);
    ncdf_varinq->AddTag("NDIMS",  &aLong);
    ncdf_varinq->AddTag("NATTS",  &aLong);
    ncdf_varinq->AddTag("DIM",  &aLong);

    structList.push_back(ncdf_varinq);
    

    //fill the structure
    DStructGDL* inq=new DStructGDL(ncdf_varinq);
    inq->InitTag("NAME",DStringGDL(var_name));

    dimension dim( var_ndims);
    DULongGDL* dims_res = new DULongGDL(dim, BaseGDL::NOZERO);
    for( size_t i=0; i<var_ndims; ++i) {
      // reverse index order (fix from Sylwester Arabas)
      (*dims_res)[ i] = var_dims[var_ndims-(i+1)];
    }

    inq->InitTag("DATATYPE",ncdf_gdl_typename(var_type));
    inq->InitTag("NDIMS",DLongGDL(var_ndims));
    inq->InitTag("NATTS",DLongGDL(var_natts));
    inq->InitTag("DIM",*dims_res);
   
    return inq;


  }

  BaseGDL* ncdf_varid(EnvT* e)
  {
    size_t nParam=e->NParam(2);


    int status, var_id;
    DInt cdfid;
    e->AssureScalarPar<DIntGDL>(0, cdfid);
    DString dim_name;
    e->AssureScalarPar<DStringGDL>(1, dim_name);
    
    status=nc_inq_varid(cdfid, dim_name.c_str(),&var_id);
    ncdf_handle_error(e,status,"NCDF_VARID");

    return new DLongGDL(var_id);
  }


  void ncdf_varget1(EnvT* e)
  {
    size_t nParam=e->NParam(3);
     
    int status,var_ndims,var_dims[NC_MAX_VAR_DIMS],var_natts;
    size_t index[NC_MAX_VAR_DIMS];
    size_t reverse_index[NC_MAX_VAR_DIMS];
    size_t dim_length[NC_MAX_VAR_DIMS];


    nc_type var_type;
    char var_name[NC_MAX_NAME], *n;
    size_t i,mema;

    DLong cdfid, varid;
    e->AssureLongScalarPar(0, cdfid);

    // Check type of varid
    BaseGDL* p1 = e->GetParDefined( 1);
    if (p1->Type() != STRING) {
      // Numeric
      e->AssureLongScalarPar(1, varid);
    } else {
      // String
      DString var_name;
      e->AssureScalarPar<DStringGDL>(1, var_name);
      status=nc_inq_varid(cdfid, var_name.c_str(), &varid);
      ncdf_handle_error(e,status,"NCDF_VARGET1");
    }

    //inquire
    status=nc_inq_var(cdfid,varid,var_name,
		      &var_type,&var_ndims,
		      var_dims,&var_natts);

    //handle the error
    ncdf_handle_error(e,status,"NCDF_VARGET1");

    //get the dimension lengths
    for (i=0;i<var_ndims;++i)
      {
	status=nc_inq_dimlen(cdfid,var_dims[i],&dim_length[i]);
	ncdf_handle_error(e,status,"NCDF_VARGET1");
	reverse_index[var_dims[i]]=i;
      }
    
    for (i=0;i<var_ndims;++i)index[i]=0;//defaults


    /*Goddamn column first languages
    */
      
    if(e->GetKW(0)!=NULL)//OFFSET
      {
	DIntGDL *o=e->GetKWAs<DIntGDL>(0);
        int nEl=o->N_Elements();
	for (i=0;i<nEl;++i) 
	  {
	    if((*o)[i] < dim_length[nEl-(i+1)])
	      {
		index[nEl-(i+1)]=(*o)[i];
	      }
	    else if((*o)[i] <= 0)
	      {
		index[nEl-(i+1)]=0;

		string mess;
		negzero_message("NCDF_VARGET1: Offset on",i,0);
	      }
	    else
	      {

		index[nEl-(i+1)]=dim_length[nEl-(i+1)]-1;
		exceed_message("NCDF_VARGET1",i,
			       dim_length[nEl-(i+1)]-1);
	      }
	  }


      }


    dimension dim( 1);
    if(var_type == NC_DOUBLE){

      double dvar;
      status=nc_get_var1_double(cdfid,
				varid,
				index,
				&dvar);

      	  delete e->GetParGlobal(2);
      e->GetParGlobal(2)=new DDoubleGDL(dvar);
    }
    else if(var_type == NC_FLOAT){
      
      float fvar;

	status=nc_get_var1_float(cdfid,
				varid,
				index,
				&fvar);

	  delete e->GetParGlobal(2);
	e->GetParGlobal(2)=new DFloatGDL(fvar);
    }
    /*    This shouldn't get called?
else if(var_type == NC_LONG)
      {

	long lvar;

	status=nc_get_var1_long(cdfid,
				varid,
				index,
				&lvar);
	  delete e->GetParGlobal(2);
	e->GetParGlobal(2) = new DLongGDL((lvar));

	}*/
    else if(var_type == NC_INT)
      {
	
	int ivar;
	status=nc_get_var1_int(cdfid,
				varid,
				index,
				&ivar);
	  delete e->GetParGlobal(2);
	e->GetParGlobal(2) = new DLongGDL((ivar));
      }
    else if(var_type == NC_SHORT)
      {
	
	short svar;
	status=nc_get_var1_short(cdfid,
				varid,
				index,
				&svar);
	  delete e->GetParGlobal(2);
	e->GetParGlobal(2) = new DIntGDL((svar));
      }
    else if(var_type == NC_CHAR){
      unsigned char cvar;
      status=nc_get_var1_uchar(cdfid,
				varid,
			      index,
				&cvar);
	  delete e->GetParGlobal(2);
      e->GetParGlobal(2) = new DStringGDL(cvar);
    }
    else if(var_type == NC_BYTE){
      unsigned char  bvar;
      status=nc_get_var1_uchar(cdfid,
			       varid,
			       index,
			       &bvar);

      delete e->GetParGlobal(2);
      e->GetParGlobal(2) = new DByteGDL((bvar));
    }
    
    ncdf_handle_error(e,status,"NCDF_VARGET1");
  }
    


  void ncdf_varget(EnvT* e)
  {

   size_t nParam=e->NParam(3);
   
    int status,var_ndims,var_dims[NC_MAX_VAR_DIMS],var_natts;
    size_t index[NC_MAX_VAR_DIMS],
      dim_length[NC_MAX_VAR_DIMS];
    SizeT transposed_dim_length[NC_MAX_VAR_DIMS];
    long trans[NC_MAX_VAR_DIMS];

    nc_type var_type;
    char var_name[NC_MAX_NAME];
    size_t i;

    DLong cdfid, varid;
    e->AssureLongScalarPar(0, cdfid);

    // Check type of varid
    BaseGDL* p1 = e->GetParDefined( 1);
    if (p1->Type() != STRING) {
      // Numeric
      e->AssureLongScalarPar(1, varid);
    } else {
      // String
      DString var_name;
      e->AssureScalarPar<DStringGDL>(1, var_name);
      status=nc_inq_varid(cdfid, var_name.c_str(), &varid);
      ncdf_handle_error(e,status,"NCDF_VARGET");
    }

    /*
    e->AssureScalarPar<DIntGDL>(0, cdfid);
    e->AssureScalarPar<DIntGDL>(1, varid);
    */

    //inquire
    status=nc_inq_var(cdfid,varid,var_name,&var_type,&var_ndims,
		      var_dims,&var_natts);

    //handle the error
    ncdf_handle_error(e,status,"NCDF_VARGET");

    //get the dimension lengths
    for (i=0;i<var_ndims;++i)
      {
	status=nc_inq_dimlen(cdfid,var_dims[i],&dim_length[i]);
	trans[i]=var_ndims-i-1;
	transposed_dim_length[trans[i]]=dim_length[i];
	ncdf_handle_error(e,status,"NCDF_VARGET1");

      }
    
    for (i=0;i<var_ndims;++i)index[i]=0;//defaults
  
  

    /*Here we have the minimum required details for getting any kind of data

    We now have 4 cases to handle
case 1: Get all the data (no keywords)
case 2: Get all the data from the OFFSET
case 3: get COUNT data from the OFFSET
case 4: get COUNT DATA from OFFSET with STRIDE

for case 4, we really have 3 subcases
subcase 1: OFFSET is undefined (0,0,0,0..)
subcase 2: COUNT is undefined (all,all,all...)
subcase 3: All keywords are DEFINED
subcase 4(==case 3): STRIDE is undefined (1,1,1,1)...

case 1 we can do seperately, the rest can be handled generically, filling in COUNT, OFFSET and STRIDE as needed.

    */


    //alldata

    //OFFSET = 0
    //COUNT  = 1
    //STRIDE = 2

    if(e->GetKW(0) == NULL && 
       e->GetKW(1) == NULL &&
       e->GetKW(2) == NULL)
      {  
	//get all the data


	size_t array_size;	
	array_size=1;
	
	for(i=0;i<var_ndims;++i) array_size=array_size*dim_length[i];



	dimension dim((const DLong *) transposed_dim_length, (SizeT) var_ndims);
	if(var_type == NC_DOUBLE){
	  double* dvar=new double[array_size];
	  
	  DDoubleGDL *temp=new DDoubleGDL(dim,BaseGDL::NOZERO);
	  status=nc_get_var_double(cdfid, varid,dvar);
	  
	  ncdf_var_handle_error(e,status,"NCDF_VARGET",dvar);


	  memcpy(&(*temp)[0],&(*dvar),array_size*sizeof(double));	      
	
	  delete dvar;
	  /*	  status=transpose_perm((char *)&(*temp)[0], 
		  var_ndims,dim_length,sizeof(double),temp->Type(),trans);*/
	  delete e->GetParGlobal(2);
	  e->GetParGlobal(2)=temp;      	
	  
	} else	if(var_type == NC_FLOAT){
	  float* fvar=new float[array_size];
	  
	  DFloatGDL *temp=new DFloatGDL(dim,BaseGDL::NOZERO);
	  status=nc_get_var_float(cdfid, varid,fvar);
	  
	  ncdf_var_handle_error(e,status,"NCDF_VARGET",fvar);


	  memcpy(&(*temp)[0],&(*fvar),array_size*sizeof(float));	      
	
	  delete fvar;
	  /*	  status=transpose_perm((char *)&(*temp)[0], 
		  var_ndims,dim_length,sizeof(float),temp->Type(),trans);
	  */

	  delete e->GetParGlobal(2);
	  e->GetParGlobal(2)=temp;

	} else	if(var_type == NC_SHORT){
	  short* svar=new short[array_size];
	  
	  DIntGDL * temp=new DIntGDL(dim,BaseGDL::NOZERO);
	  status=nc_get_var_short(cdfid, varid,svar);
	  
	  ncdf_var_handle_error(e,status,"NCDF_VARGET",svar);


	  memcpy(&(*temp)[0],&(*svar),array_size*sizeof(short));	      
	
	  delete svar;
	  /*	  status=transpose_perm((char *)&(*temp)[0], 
		  var_ndims,dim_length,sizeof(short),temp->Type(),trans);*/
	  delete e->GetParGlobal(2);
	e->GetParGlobal(2)=temp;      	
	} else	if(var_type == NC_INT){
	  int* ivar=new int[array_size];
	  
	  DLongGDL* temp=new DLongGDL(dim,BaseGDL::NOZERO);
	  status=nc_get_var_int(cdfid, varid,ivar);
	  
	  ncdf_var_handle_error(e,status,"NCDF_VARGET",ivar);


	  memcpy(&(*temp)[0],&(*ivar),array_size*sizeof(int));	      
	
	  delete ivar;
	  /*	  status=transpose_perm((char *)&(*temp)[0], 
		  var_ndims,dim_length,sizeof(int),temp->Type(),trans);*/
	  delete e->GetParGlobal(2);
	e->GetParGlobal(2)=temp;      	
	} else	if(var_type == NC_BYTE){
	  unsigned char* bvar=new unsigned char[array_size];
	  
	  DByteGDL* temp=new DByteGDL(dim,BaseGDL::NOZERO);
	  status=nc_get_var_uchar(cdfid, varid,bvar);
	  
	  ncdf_var_handle_error(e,status,"NCDF_VARGET",bvar);


	  memcpy(&(*temp)[0],&(*bvar),array_size*sizeof(unsigned char));	      
	  delete bvar;
	  /*	  status=transpose_perm((char *)&(*temp)[0], 
		  var_ndims,dim_length,sizeof(unsigned char),temp->Type(),trans);*/
	
	  delete e->GetParGlobal(2);  
	e->GetParGlobal(2)=temp;      	
	} else	if(var_type == NC_CHAR){
	  unsigned char* cvar=new unsigned char[array_size];
	  
	  DStringGDL* temp=new DStringGDL(dim,BaseGDL::NOZERO);
	  status=nc_get_var_uchar(cdfid, varid,cvar);
	  
	  ncdf_var_handle_error(e,status,"NCDF_VARGET",cvar);
	  

	  memcpy(&(*temp)[0],&(*cvar),array_size*sizeof(unsigned char));	      
	  delete cvar;
	  /*	  status=transpose_perm((char *)&(*temp)[0], 
		  var_ndims,dim_length,sizeof(unsigned char),temp->Type(),trans);*/
	  delete e->GetParGlobal(2);
	e->GetParGlobal(2)=temp;      	
	}
      

	//done read all.

    
      } else  {
	//OFFSET AND/OR COUNT and NOT/ STRIDE
	BaseGDL *offset,*count;
	size_t off[NC_MAX_VAR_DIMS], cou[NC_MAX_VAR_DIMS];
	for (i=0;i<NC_MAX_VAR_DIMS;++i)
	  {
	    off[i]=0;
	    cou[i]=1;
	  }
	size_t array_size;
      
	array_size=1;
	

	for (i=0;i<var_ndims;++i) off[i]=0;


	if(e->GetKW(1) !=NULL)
	  {
	    DIntGDL *o=e->GetKWAs<DIntGDL>(1);
	    int noff=o->N_Elements();
	    for (i=0;i<noff;++i) 
	      {
		if((*o)[i] < dim_length[var_ndims-(i+1)])
		  {
		    off[var_ndims-(i+1)]=(*o)[i];
		  }
		else if((*o)[i] <= 0)
		  {
		    off[trans[i]]=0;
		    negzero_message("NCDF_VARGET: Offset on",i,0);

		  }
		else
		  {
		    off[trans[i]]=dim_length[trans[i]]-1;
		    exceed_message("NCDF_VARGET",i,dim_length[trans[i]]-1);

		  }
		
	      }
	  } 


      
	for (i=0;i<var_ndims;++i) 
  	    cou[i]=dim_length[i]-off[i];

	  
	if(e->GetKW(0) !=NULL)
	{

	  DIntGDL *c=e->GetKWAs<DIntGDL>(0);
	  int ncou=c->N_Elements();

	  for (i=0;i<ncou;++i) 
	      {

		if((*c)[i] < dim_length[trans[i]]-off[trans[i]]+1)
		  {
		    cou[trans[i]]=(*c)[i];
		  }
		else if((*c)[i] <= 0)
		  {
		    cou[trans[i]]=1;
		    negzero_message("NCDF_VARGET: Count on",i,1);

		  }
		else
		  {
		    cou[trans[i]]=dim_length[trans[i]]-off[trans[i]]+1;
		    exceed_message("NCDF_VARGET",i,
				   dim_length[trans[i]]-off[trans[i]]+1);
		  }
		
	      }

	}

	    
	if(e->GetKW(2) == NULL)
	  {

	    //NO STRIDE
	    array_size=1;
	    for(i=0;i<var_ndims;++i) {
	      transposed_dim_length[trans[i]]=cou[i];
	      array_size=array_size*cou[i];
	    }
	    dimension dim((const DLong *) transposed_dim_length, (SizeT) var_ndims);
	    if(var_type == NC_DOUBLE)
	      {
		double* dvar=new double[array_size];
		
		DDoubleGDL *temp=new DDoubleGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vara_double(cdfid, varid,
					  off,cou,dvar);

		ncdf_var_handle_error(e,status,"NCDF_VARGET",dvar);

		memcpy(&(*temp)[0],&(*dvar),array_size*sizeof(double));	      
		
		delete dvar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				var_ndims,cou,sizeof(double),temp->Type(),trans);*/
		delete e->GetParGlobal(2);
		e->GetParGlobal(2)=temp;      	
	      }
	    else if(var_type == NC_FLOAT)
	      {
		float* fvar=new float[array_size];
		
		DFloatGDL *temp=new DFloatGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vara_float(cdfid, varid,
					  off,cou,fvar);

		ncdf_var_handle_error(e,status,"NCDF_VARGET",fvar);
		

		memcpy(&(*temp)[0],&(*fvar),array_size*sizeof(float));	      
		
		delete fvar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				var_ndims,cou,sizeof(float),temp->Type(),trans);*/
		delete e->GetParGlobal(2);
		e->GetParGlobal(2)=temp;      	
		
	      }
	    else if(var_type == NC_SHORT)
	      {
		short* svar=new short[array_size];
		
		DIntGDL *temp=new DIntGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vara_short(cdfid, varid,
					  off,cou,svar);

		ncdf_var_handle_error(e,status,"NCDF_VARGET",svar);

		memcpy(&(*temp)[0],&(*svar),array_size*sizeof(short));	      
		
		delete svar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				var_ndims,cou,sizeof(short),temp->Type(),trans);*/
		delete e->GetParGlobal(2);
		e->GetParGlobal(2)=temp;      	

	      }
	    else if(var_type == NC_INT)
	      {
		int* ivar=new int[array_size];
		
		DLongGDL *temp=new DLongGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vara_int(cdfid, varid,
					  off,cou,ivar);

		ncdf_var_handle_error(e,status,"NCDF_VARGET",ivar);

		memcpy(&(*temp)[0],&(*ivar),array_size*sizeof(int));	      

		delete ivar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				var_ndims,cou,sizeof(int),temp->Type(),trans);*/
		delete e->GetParGlobal(2);
		e->GetParGlobal(2)=temp;      	

	      }
	    else if(var_type == NC_BYTE)
	      {
		unsigned char* bvar=new unsigned char[array_size];
		
		DByteGDL *temp=new DByteGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vara_uchar(cdfid, varid,
					  off,cou,bvar);

		ncdf_var_handle_error(e,status,"NCDF_VARGET",bvar);

		memcpy(&(*temp)[0],&(*bvar),array_size*sizeof(unsigned char));	      

		delete bvar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				var_ndims,cou,sizeof(unsigned char),temp->Type(),trans);*/
		delete e->GetParGlobal(2);
		e->GetParGlobal(2)=temp;      	

	  	      }
	    else if(var_type == NC_CHAR)
	      {
		unsigned char* cvar=new unsigned char[array_size];
		
		DStringGDL *temp=new DStringGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vara_uchar(cdfid, varid,
					  off,cou,cvar);

		ncdf_var_handle_error(e,status,"NCDF_VARGET",cvar);

		memcpy(&(*temp)[0],&(*cvar),array_size*sizeof(unsigned char));	      
		
		delete cvar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				      var_ndims,cou,sizeof(unsigned char),
				      temp->Type(),trans);*/
	      
		delete e->GetParGlobal(2);
		e->GetParGlobal(2)=temp;      	
	      }
	  
	  
	  } else {
	    //STRIDE
	    ptrdiff_t stri[NC_MAX_VAR_DIMS];
	    BaseGDL * stride;
	    DIntGDL *s=e->GetKWAs<DIntGDL>(2);
	    for (i=0;i<var_ndims;++i) 
	      stri[i]=1;
	    
	    int nstri=s->N_Elements();
	    for (i=0;i<nstri;++i) 
	      {
		if((*s)[i]*cou[trans[i]] <
		   dim_length[trans[i]]-off[trans[i]]+1)
		  {
		    //stride * count < length-offset
		    stri[trans[i]]=(*s)[i];

		  }
		else if((*s)[i] <= 0)
		  {
		    //		    stride<0, stop it now.
		    stri[trans[i]]=1;
		    
		    throw GDLException(e->CallingNode(),
				       "NCDF_VARGET: STRIDE array cannot have negative elements"
				       +e->GetParString(0));

		  }
		else
		  {
		  
		    //cou*stride> length-offset
		    //reduce count
		    stri[trans[i]]=(*s)[i];
		    cou[trans[i]]=(dim_length[trans[i]]-off[trans[i]])/
		      stri[trans[i]];

		    //silent if there is no COUNT keyword
		    if(e->GetKW(0) !=NULL)
		      {
			int mema=3;
			string mess;
			mess="NCDF_VARGET: Requested read is larget than data in dimension ";
			char * n=new char(mema);
			while (snprintf(n, sizeof n, "%d", i) >= sizeof n)
			  {   delete n; mema++; n=new char(mema);	      }
			mess+=n;
			delete n;
			
			mess+=". Reducing COUNT to ";
			mema=3;
			n=new char[3];
			while (snprintf(n, sizeof n, "%d",  cou[trans[i]]) >= sizeof n)
			  {   delete n; mema++; n=new char(mema);	      }
			mess+=n;
			delete n;
			mess+=".";
			Message(mess);
		      }	
		  }
		
	      }
		
	    //now we can get the damn data
	    	    array_size=1;
	    for(i=0;i<var_ndims;++i) {
	      transposed_dim_length[trans[i]]=cou[i];
	      array_size=array_size*cou[i];
	    }
	    dimension dim((const DLong *) transposed_dim_length, (SizeT) var_ndims);
	    	  
	    if(var_type == NC_DOUBLE)
	      {
		double* dvar=new double[array_size];
		
		DDoubleGDL *temp=new DDoubleGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vars_double(cdfid, varid,
					  off,cou,stri,dvar);

		ncdf_var_handle_error(e,status,"NCDF_VARGET",dvar);

		memcpy(&(*temp)[0],&(*dvar),array_size*sizeof(double));	      
		
		delete dvar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				var_ndims,cou,sizeof(double),temp->Type(),trans);*/
		e->GetParGlobal(2)=temp;      	
	      }
	    else if(var_type == NC_FLOAT)
	      {
		float* fvar=new float[array_size];
		/*		for (i=0;i<var_ndims;++i)
		  {
		    cout<<off[i]<<","<<cou[i]<<","<<stri[i]<<endl;
		  }
		*/
		DFloatGDL *temp=new DFloatGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vars_float(cdfid, varid,
					  off,cou,stri,fvar);
		
		ncdf_var_handle_error(e,status,"NCDF_VARGET",fvar);
		

		memcpy(&(*temp)[0],&(*fvar),array_size*sizeof(float));	      
		
		delete fvar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				var_ndims,cou,sizeof(float),temp->Type(),trans);*/
		delete e->GetParGlobal(2);
		e->GetParGlobal(2)=temp;      	
		
	      }
	    else if(var_type == NC_SHORT)
	      {
		short* svar=new short[array_size];
		
		DIntGDL *temp=new DIntGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vars_short(cdfid, varid,
					  off,cou,stri,svar);

		ncdf_var_handle_error(e,status,"NCDF_VARGET",svar);

		memcpy(&(*temp)[0],&(*svar),array_size*sizeof(short));	      
		
		delete svar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				var_ndims,cou,sizeof(short),temp->Type(),trans);*/
		delete e->GetParGlobal(2);
		e->GetParGlobal(2)=temp;      	

	  	      }
	    else if(var_type == NC_INT)
	      {
		int* ivar=new int[array_size];
		
		DLongGDL *temp=new DLongGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vars_int(cdfid, varid,
					  off,cou,stri,ivar);

		ncdf_var_handle_error(e,status,"NCDF_VARGET",ivar);

		memcpy(&(*temp)[0],&(*ivar),array_size*sizeof(int));	      
		
		delete ivar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				var_ndims,cou,sizeof(int),temp->Type(),trans);*/
		delete e->GetParGlobal(2);
		e->GetParGlobal(2)=temp;      	
		
	      }
	    else if(var_type == NC_BYTE)
	      {
		unsigned char* bvar=new unsigned char[array_size];
		
		DByteGDL *temp=new DByteGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vars_uchar(cdfid, varid,
					  off,cou,stri,bvar);

		ncdf_var_handle_error(e,status,"NCDF_VARGET",bvar);

		memcpy(&(*temp)[0],&(*bvar),array_size*sizeof(unsigned char));	      
		
		delete bvar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				var_ndims,cou,sizeof(unsigned char),temp->Type(),trans);*/
		delete e->GetParGlobal(2);
		e->GetParGlobal(2)=temp;      	

		}
		else if(var_type == NC_CHAR)
		{
		unsigned char* cvar=new unsigned char[array_size];
		
		DStringGDL *temp=new DStringGDL(dim,BaseGDL::NOZERO);
		status=nc_get_vars_uchar(cdfid, varid,
					  off,cou,stri,cvar);

		ncdf_var_handle_error(e,status,"NCDF_VARGET",cvar);

		memcpy(&(*temp)[0],&(*cvar),array_size*sizeof(unsigned char));	      
		
		delete cvar;
		/*		status=transpose_perm((char *)&(*temp)[0], 
				      var_ndims,cou,sizeof(unsigned char),
				      temp->Type(),trans);*/

		delete e->GetParGlobal(2);
		e->GetParGlobal(2)=temp;      	
		}
	  }
      }
  }



  BaseGDL* ncdf_vardef(EnvT* e)
  {
    size_t nParam=e->NParam(2);
    int dims[NC_MAX_VAR_DIMS], status, var_id,i,var_ndims;
    nc_type type;

    DInt cdfid;
    e->AssureScalarPar<DIntGDL>(0, cdfid);
 

    BaseGDL* v=e->GetParDefined(1);
    DString var_name;
    e->AssureStringScalarPar(1,var_name);

    for (i=0;i<NC_MAX_VAR_DIMS;++i) dims[i]=0;

    if(nParam == 3)
      {
	v=e->GetParDefined(2);
	DIntGDL* dim_in=static_cast<DIntGDL*>(v->Convert2(INT, BaseGDL::COPY));
	var_ndims=dim_in->N_Elements();
	if(var_ndims > NC_MAX_VAR_DIMS)
	  {
	    throw GDLException(e->CallingNode(),
			       "NCDF_VARDEF: Too many elements error 1 in array"				       +e->GetParString(0));
	  }					      

	for (i=0; i<var_ndims;++i)
	  dims[var_ndims-i-1]=(*dim_in)[i];


	//dim is set
      } 
    else if(nParam == 2)
      {
	var_ndims=0;
	//dim is not set, scalar
      }

    
    if(e->KeywordSet(0))//BYTE
      type=NC_BYTE;
    else if(e->KeywordSet(1))//CHAR
      type=NC_CHAR;
    else if(e->KeywordSet(2))//DOUBLE
      type=NC_DOUBLE;
    else if(e->KeywordSet(4))//LONG
      type=NC_INT;
    else if(e->KeywordSet(5))//SHORT
      type=NC_SHORT;
    else
      type=NC_FLOAT;


	status=nc_def_var(cdfid,
			  var_name.c_str(),
			  type,
			  var_ndims,
			  dims,
			  &var_id);

	if(status==NC_ENAMEINUSE)
	throw GDLException(e->CallingNode(),
			   "Unable to define variable, name in use by another variable ("+var_name+")");
	else
	  ncdf_handle_error(e,status,"NCDF_VARDEF");
  

    return new DIntGDL(var_id);


  }

  void ncdf_varrename(EnvT* e)
  {
    size_t nParam=e->NParam(3);
    int status;
    

    //get the cdfid, which must be given.
    BaseGDL* n;

    DLong cdfid, varid;
    e->AssureLongScalarPar(0, cdfid);

    // Check type of varid
    BaseGDL* p1 = e->GetParDefined( 1);
    if (p1->Type() != STRING) {
      // Numeric
      e->AssureLongScalarPar(1, varid);
    } else {
      // String
      DString var_name;
      e->AssureScalarPar<DStringGDL>(1, var_name);
      status=nc_inq_varid(cdfid, var_name.c_str(), &varid);
      ncdf_handle_error(e,status,"NCDF_VARRENAME");
    }

    DString newname;
    e->AssureStringScalarPar(2, newname);
    
    //we have the cdfid, varid, newname

    status=nc_rename_var(cdfid,varid,
			 newname.c_str());

    ncdf_handle_error(e, status,"NCDF_VARRENAME");

    return;
  }


  void ncdf_varput(EnvT* e)
  {

    //definitions
    int status, i, status_tr,total;

    size_t var_ndims, value_nelem,dim_length[MAXRANK];
    SizeT transposed_dim_length[NC_MAX_VAR_DIMS];
    long trans[NC_MAX_VAR_DIMS], retrans[NC_MAX_VAR_DIMS];

    nc_type type;

    //get the cdfid
    BaseGDL* v;

    DLong cdfid, varid;
    e->AssureLongScalarPar(0, cdfid);

    // Check type of varid
    BaseGDL* p1 = e->GetParDefined( 1);
    if (p1->Type() != STRING) {
      // Numeric
      e->AssureLongScalarPar(1, varid);
    } else {
      // String
      DString var_name;
      e->AssureScalarPar<DStringGDL>(1, var_name);
      status=nc_inq_varid(cdfid, var_name.c_str(), &varid);
      ncdf_handle_error(e,status,"NCDF_VARGET");
    }

    //get the value
    v=e->GetParDefined(2);
    var_ndims=v->Rank();
    value_nelem=v->N_Elements();
    for (i=0;i<var_ndims;++i)
      {
	dim_length[i]=v->Dim(i);
      }

    
    //do offset first
    if(var_ndims==0)//scalar
      {
	trans[0]=0;
	retrans[0]=0;
	transposed_dim_length[trans[0]]=dim_length[0];
      }
    else
      {
	for (i=0;i<var_ndims;++i)
	  {
	    trans[i]=var_ndims-i-1;
	    retrans[i]=i;
	    transposed_dim_length[trans[i]]=dim_length[i];
	  }
      }


    //no keywords
    if(e->GetKW(0) == NULL && 
       e->GetKW(1) == NULL &&
       e->GetKW(2) == NULL)
      {
	//put all of the data

	if(v->Type() == DOUBLE)
	  {

	    DDoubleGDL* dvar=static_cast<DDoubleGDL*>(v);

	    /*if(var_ndims > 0) status_tr=transpose_perm((char *)&(*dvar)[0], 
	      var_ndims,dim_length,sizeof(double),dvar->Type(),trans);*/

	    status=nc_put_var_double(cdfid,varid,&((*dvar)[0]));

	    /* if(var_ndims > 0) status_tr=transpose_perm((char *)&(*dvar)[0], 
	       var_ndims,dim_length,sizeof(double),dvar->Type(),retrans);*/
	  }
	else if(v->Type() == FLOAT) 
	  {

	    DFloatGDL* fvar=static_cast<DFloatGDL*>(v);

	    /*if(var_ndims > 0) status_tr=transpose_perm((char *)&(*fvar)[0], 
	      var_ndims,dim_length,sizeof(float),fvar->Type(),trans);*/

	    status=nc_put_var_float(cdfid,varid,&((*fvar)[0]));

	    /*if(var_ndims > 0) status_tr=transpose_perm((char *)&(*fvar)[0], 
	      var_ndims,dim_length,sizeof(float),fvar->Type(),retrans);*/

	  }
	else if(v->Type() == INT) 
	  {

	    DIntGDL* ivar=static_cast<DIntGDL*>(v);

	    /* if(var_ndims > 0) status_tr=transpose_perm((char *)&(*ivar)[0], 
	       var_ndims,dim_length,sizeof(short),ivar->Type(),trans);*/

	    status=nc_put_var_short(cdfid,varid,&((*ivar)[0]));

	    /*if(var_ndims > 0) status_tr=transpose_perm((char *)&(*ivar)[0], 
	      var_ndims,dim_length,sizeof(short),ivar->Type(),retrans);*/

	  }
	else if(v->Type() == LONG) 
	  {
	    DLongGDL* lvar=static_cast<DLongGDL*>(v);

	    /* if(var_ndims > 0) status_tr=transpose_perm((char *)&(*lvar)[0], 
	       var_ndims,dim_length,sizeof(int),lvar->Type(),trans);*/

	    status=nc_put_var_int(cdfid,varid,&((*lvar)[0]));

	    /*if(var_ndims > 0) status_tr=transpose_perm((char *)&(*lvar)[0], 
	      var_ndims,dim_length,sizeof(int),lvar->Type(),retrans);*/

	  }
	else if(v->Type() == BYTE) 
	  {
	    DByteGDL* bvar=static_cast<DByteGDL*>(v);
	    /*if(var_ndims > 0) status_tr=transpose_perm((char *)&(*bvar)[0], 
	      var_ndims,dim_length,sizeof(unsigned char),bvar->Type(),trans);*/

	    status=nc_put_var_uchar(cdfid,varid,&((*bvar)[0]));

	    /*if(var_ndims > 0) status_tr=transpose_perm((char *)&(*bvar)[0], 
	      var_ndims,dim_length,sizeof(unsigned char),bvar->Type(),retrans);*/
	  }

	ncdf_handle_error(e,status, "NCDF_VARPUT");
    //handle the error

	//	return;
      } else {
	//count, offset or stride
	size_t count[NC_MAX_VAR_DIMS];
	size_t  offset[NC_MAX_VAR_DIMS];
	  int noff,ncou,nstri;
	ptrdiff_t stride[NC_MAX_VAR_DIMS];
	
	//setup
	if(var_ndims <= 1)
	  {
	  offset[0]=0;
	  count[0]=0;
	  stride[0]=1;
	  }
	else
	  {
	  for (i=0;i<NC_MAX_VAR_DIMS;++i)
	    {
	      offset[i]=0;
	      count[trans[i]]=dim_length[i];
	      stride[i]=1;
	    }
	  }


	if(e->GetKW(1) != NULL)
	  {
	    //offset
	    DIntGDL *o=e->GetKWAs<DIntGDL>(1);
	    noff=o->N_Elements();

	    //	    offset[0]=0;
	    for(i=0;i<noff;++i)
	      {

		if((*o)[i] > 0)
		  {
		    offset[trans[i]]=(*o)[i];
		  }
		else if((*o)[i] < 0)
		  {
		    offset[trans[i]]=0;
		    negzero_message("NCDF_VARPUT: Offset",i,0);

		  }
	      }
	  }

	if(var_ndims <= 1)
	  {
	    total=value_nelem;
	    count[0]=value_nelem;
	  }
	else 
	  {
	    for (i=0;i<var_ndims;++i) 
	      {
		count[i]=dim_length[i];
		total=total+count[i];
	      }
	  }
	  
	
	if(e->GetKW(0) != NULL)
	{
	  total=0;
	  DIntGDL *c=e->GetKWAs<DIntGDL>(0);

	  ncou=c->N_Elements();

	  for (i=0;i<ncou;++i) 
	      {
		if((*c)[i] > 0)
		  {
		    count[trans[i]]=(*c)[i];
		  }
		else if((*c)[i] <= 0)
		  {
		    count[trans[i]]=1;
		    negzero_message("NCDF_VARGET: Count ",i,1);
		  }
		total=total+count[trans[i]];
	      }

	}

	if(total > value_nelem)
	  {
	    throw GDLException(e->CallingNode(),
			       "NCDF_VARPUT: Not enough elements in write error 1");
	  }
	  

	if(e->GetKW(2) != NULL)
	  {
	    //stride

	    ptrdiff_t stri[NC_MAX_VAR_DIMS];
	    DIntGDL *s=e->GetKWAs<DIntGDL>(2);
	    nstri=s->N_Elements();

	    for (i=0;i<nstri;++i) 
	      {
		if((*s)[i]>0)
		  {
		    //stride * count < length-offset
		    stride[trans[i]]=(*s)[i];
		    
		  }
		else if((*s)[i] <= 0)
		  {
		    //		    stride<0, stop it now.
		    stride[trans[i]]=1;
		    
		    throw GDLException(e->CallingNode(),
				       "NCDF_VARPUT: STRIDE array cannot have negative elements"
				       +e->GetParString(0));
		    
		  }

		
	      }

	  }
	if(v->Type() == DOUBLE)
	  {
	    DDoubleGDL* dvar=static_cast<DDoubleGDL*>(v);
	    
	    /*  if(var_ndims > 0) status_tr=transpose_perm((char *)&(*dvar)[0], var_ndims, dim_length,sizeof(double),  dvar->Type(),trans);*/

	    status=nc_put_vars_double(cdfid,
				      varid,
				      offset,count,stride,
				      &((*dvar)[0]));

	    /*	    if(var_ndims > 0) status_tr=transpose_perm((char *)&(*dvar)[0], var_ndims,  dim_length,sizeof(double),  dvar->Type(), retrans);*/
	  } else  if(v->Type() == FLOAT)  {
	    DFloatGDL* fvar=static_cast<DFloatGDL*>(v);

	    /*	    if(var_ndims > 0) status_tr=transpose_perm((char *)&(*fvar)[0], var_ndims,  dim_length,sizeof(float),  fvar->Type(),trans);*/

	    status=nc_put_vars_float(cdfid,
				      varid,
				      offset,count,stride,
				      &((*fvar)[0]));

	    /*	    if(var_ndims > 0) status_tr=transpose_perm((char *)&(*fvar)[0], var_ndims,    dim_length,sizeof(float),    fvar->Type(), retrans);*/

	  } else  if(v->Type() == INT)  {
	    DIntGDL* ivar=static_cast<DIntGDL*>(v);
	    
	    /*	    if(var_ndims > 0) status_tr=transpose_perm((char *)&(*ivar)[0], var_ndims,    dim_length,sizeof(int),     ivar->Type(),trans);*/

	    status=nc_put_vars_short(cdfid,
				      varid,
				      offset,count,stride,
				      &((*ivar)[0]));

	    /*	    if(var_ndims > 0) status_tr=transpose_perm((char *)&(*ivar)[0], var_ndims,     dim_length,sizeof(int),    ivar->Type(), retrans);*/

	  } else  if(v->Type() == LONG)  {
	    DLongGDL* lvar=static_cast<DLongGDL*>(v);
	    
	    /*	    if(var_ndims > 0) status_tr=transpose_perm((char *)&(*lvar)[0], var_ndims,    dim_length,sizeof(long),    lvar->Type(),trans);*/

	    status=nc_put_vars_int(cdfid,
				      varid,
				      offset,count,stride,
				      &((*lvar)[0]));

	    /*	    if(var_ndims > 0) status_tr=transpose_perm((char *)&(*lvar)[0], var_ndims,     dim_length,sizeof(long),    lvar->Type(), retrans);*/
	  } else  if(v->Type() == BYTE)  {
	    DByteGDL* bvar=static_cast<DByteGDL*>(v);
	    
	    /*	    if(var_ndims > 0) status_tr=transpose_perm((char *)&(*bvar)[0], var_ndims,    dim_length,sizeof(unsigned char),    bvar->Type(),trans);*/

	    status=nc_put_vars_uchar(cdfid,
				      varid,
				      offset,count,stride,
				      &((*bvar)[0]));

	    /*	    if(var_ndims > 0) status_tr=transpose_perm((char *)&(*bvar)[0], var_ndims,    dim_length,sizeof(unsigned char),    bvar->Type(), retrans);*/
	  }
	ncdf_handle_error(e,status,"NCDF_VARPUT");
	return;
      }

 }

}

#endif
