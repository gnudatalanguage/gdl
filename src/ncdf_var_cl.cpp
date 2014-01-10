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
#include <algorithm>

#include <netcdf.h>

#include "datatypes.hpp"
#include "math_utl.hpp"
#include "envt.hpp"
#include "dpro.hpp"
#include "dinterpreter.hpp"

#include "ncdf_cl.hpp"
//#include "terminfo.hpp"
//#include "typedefs.hpp"



#define GDL_DEBUG
//#undef GDL_DEBUG

namespace lib {



  using namespace std;
  using namespace antlr;
  extern bool ncdf_verbose;
  
  void exceed_message(const char * name,int index, int set)
  {
    string mess;
    mess=name;
    mess+="Limit exceeded on index "+i2s(index);
    mess+=", setting to "+i2s(set)+".";
    Message(mess);

  }

  void negzero_message(const char * name,int index, int set)
  {
    int mema=3;
    string mess;
    mess=name;
    mess+="Value of index "+i2s(index);
    if(set > 0) 
      mess+=" is negative or zero, setting to ";
    else if(set == 0) 
      mess+=" is negative , setting to ";
    else
      mess+=" INTERNAL ERROR NCDF_VAR_CL.CPP negzero_message";
    mess+=i2s(set);
    mess+=".";
    Message(mess);

  }


  template <typename T> 
  void ncdf_var_handle_error(EnvT *e, int status, const char *function, T *data)
  {

    cout << function << endl;
    if (data != NULL and status != NC_NOERR) GDLDelete(data);
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
    if (p1->Type() != GDL_STRING) {
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
    SpDLong aLongArr(dimension(var_ndims == 0 ? 1 : var_ndims));

    ncdf_varinq->AddTag("NAME", &aString);
    ncdf_varinq->AddTag("DATATYPE", &aString);
    ncdf_varinq->AddTag("NDIMS",  &aLong);
    ncdf_varinq->AddTag("NATTS",  &aLong);
    if (var_ndims == 0) ncdf_varinq->AddTag("DIM", &aLong);
    else ncdf_varinq->AddTag("DIM", &aLongArr);

    //fill the structure
    DStructGDL* inq=new DStructGDL(ncdf_varinq, dimension());
    inq->InitTag("NAME",DStringGDL(var_name));

    DLongGDL* dims_res;
    if (var_ndims == 0) dims_res = new DLongGDL(0);
    else 
    {
      dims_res = new DLongGDL(dimension(var_ndims));
      for( size_t i=0; i<var_ndims; ++i) 
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

    DLong cdfid;
    e->AssureLongScalarPar(0, cdfid);
    DString dim_name;
    e->AssureScalarPar<DStringGDL>(1, dim_name);
    
    status=nc_inq_varid(cdfid, dim_name.c_str(),&var_id);
    if (status == NC_ENOTVAR) 
    {
      Warning("NCDF_VARID: Variable not found \"" + dim_name + "\"");
      return new DLongGDL(-1);
    }
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
    if (p1->Type() != GDL_STRING) {
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
    status=nc_inq_var(cdfid,varid,var_name, &var_type,&var_ndims, var_dims,&var_natts);

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
	DLongGDL *o=e->GetKWAs<DLongGDL>(0);
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

      	  GDLDelete(e->GetParGlobal(2));
      e->GetParGlobal(2)=new DDoubleGDL(dvar);
    }
    else if(var_type == NC_FLOAT){
      
      float fvar;

	status=nc_get_var1_float(cdfid,
				varid,
				index,
				&fvar);

	  GDLDelete(e->GetParGlobal(2));
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
	  GDLDelete(e->GetParGlobal(2));
	e->GetParGlobal(2) = new DLongGDL((lvar));

	}*/
    else if(var_type == NC_INT)
      {
	
	int ivar;
	status=nc_get_var1_int(cdfid,
				varid,
				index,
				&ivar);
	  GDLDelete(e->GetParGlobal(2));
	e->GetParGlobal(2) = new DLongGDL((ivar));
      }
    else if(var_type == NC_SHORT)
      {
	
	short svar;
	status=nc_get_var1_short(cdfid,
				varid,
				index,
				&svar);
	  GDLDelete(e->GetParGlobal(2));
	e->GetParGlobal(2) = new DIntGDL((svar));
      }
    else if(var_type == NC_CHAR)
    {
      char cvar;
      status=nc_get_var1_text(cdfid, varid, index, &cvar);
      GDLDelete(e->GetParGlobal(2));
      e->GetParGlobal(2) = new DByteGDL(cvar);
    }
    else if(var_type == NC_BYTE){
      unsigned char  bvar;
      status=nc_get_var1_uchar(cdfid,
			       varid,
			       index,
			       &bvar);

      GDLDelete(e->GetParGlobal(2));
      e->GetParGlobal(2) = new DByteGDL((bvar));
    }
    
    ncdf_handle_error(e,status,"NCDF_VARGET1");
  }
    


  void ncdf_varget(EnvT* e)
  {

    size_t nParam=e->NParam(3);
   
    int status,var_ndims,var_dims[NC_MAX_VAR_DIMS],var_natts;
    size_t index[NC_MAX_VAR_DIMS], dim_length[NC_MAX_VAR_DIMS];
    DLong transposed_dim_length[NC_MAX_VAR_DIMS];
    long trans[NC_MAX_VAR_DIMS];

    nc_type var_type;
    char var_name[NC_MAX_NAME];

    DLong cdfid, varid;
    e->AssureLongScalarPar(0, cdfid);

    // Check type of varid
    BaseGDL* p1 = e->GetParDefined( 1);
    if (p1->Type() != GDL_STRING) {
      // Numeric
      e->AssureLongScalarPar(1, varid);
    } else {
      // String
      DString var_name;
      e->AssureScalarPar<DStringGDL>(1, var_name);
      status=nc_inq_varid(cdfid, var_name.c_str(), &varid);
      ncdf_handle_error(e,status,"NCDF_VARGET");
    }

    //inquire
    status = nc_inq_var(cdfid, varid, var_name, &var_type, &var_ndims, var_dims, &var_natts);
    ncdf_handle_error(e, status, "NCDF_VARGET");

    //get the dimension lengths
    if (var_ndims == 0) trans[0] = 0;
    else for (int i=0; i < var_ndims; ++i)
    {
      status = nc_inq_dimlen(cdfid,var_dims[i],&dim_length[i]);
      trans[i] = var_ndims - i - 1;
      transposed_dim_length[trans[i]] = dim_length[i];
      ncdf_handle_error(e, status, "NCDF_VARGET");
    }
    
    for (int i = 0; i < var_ndims; ++i) index[i]=0;//defaults

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

    case 1 we can do seperately, the rest can be handled generically, 
      filling in COUNT, OFFSET and STRIDE as needed.

    */

    //OFFSET = 0
    //COUNT  = 1
    //STRIDE = 2

    if (e->GetKW(0) == NULL && e->GetKW(1) == NULL && e->GetKW(2) == NULL)
    {  
      //get all the data

      size_t array_size = 1;
      for (int i = 0; i < var_ndims; ++i) array_size = array_size * dim_length[i];

      dimension dim(transposed_dim_length, (SizeT) var_ndims);
      if (var_type == NC_DOUBLE)
      {
        DDoubleGDL *temp = new DDoubleGDL(dim,BaseGDL::NOZERO);
        status=nc_get_var_double(cdfid, varid, &(*temp)[0]);
        ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
        GDLDelete(e->GetParGlobal(2));
        e->GetParGlobal(2)=temp;      	
      } 
      else if (var_type == NC_FLOAT)
      {
        DFloatGDL *temp = new DFloatGDL(dim, BaseGDL::NOZERO);
        status = nc_get_var_float(cdfid, varid, &(*temp)[0]);
        ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
        GDLDelete(e->GetParGlobal(2));
        e->GetParGlobal(2)=temp;
      } 
      else if (var_type == NC_SHORT)
      {
        DIntGDL * temp=new DIntGDL(dim,BaseGDL::NOZERO);
        status=nc_get_var_short(cdfid, varid,&(*temp)[0]);
        ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
        GDLDelete(e->GetParGlobal(2));
        e->GetParGlobal(2)=temp;      	
      } 
      else if (var_type == NC_INT)
      {
        DLongGDL* temp=new DLongGDL(dim,BaseGDL::NOZERO);
        status=nc_get_var_int(cdfid, varid,&(*temp)[0]);
        ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
        GDLDelete(e->GetParGlobal(2));
        e->GetParGlobal(2)=temp;      	
      } 
      else if (var_type == NC_BYTE)
      {
        DByteGDL* temp=new DByteGDL(dim,BaseGDL::NOZERO);
        status=nc_get_var_uchar(cdfid, varid, &(*temp)[0]);
	if (status != NC_ERANGE) {
	  ncdf_var_handle_error(e,status,"NCDF_VARGET (ici)", temp);
	} else {
	  Warning("Warning in NCDF_VARGET: NC_ERANGE during BYTE reading");
	  ncdf_var_handle_error(e,status,"NCDF_VARGET (ici)", temp);
	}
	GDLDelete(e->GetParGlobal(2));  
        e->GetParGlobal(2)=temp;      	
      } 
      else if (var_type == NC_CHAR)
      {
        DByteGDL* temp = new DByteGDL(dim, BaseGDL::NOZERO);
        status = nc_get_var_text(cdfid, varid, (char*) &(*temp)[0]);
        ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
        GDLDelete(e->GetParGlobal(2));
        e->GetParGlobal(2) = temp;
      }
      //done read all.
    } 
    else  
    {
      //OFFSET AND/OR COUNT and NOT/ STRIDE
      BaseGDL *offset,*count;
      size_t off[NC_MAX_VAR_DIMS], cou[NC_MAX_VAR_DIMS];
      for (int i = 0; i < NC_MAX_VAR_DIMS; ++i)
      {
        off[i]=0;
        cou[i]=1;
      }
      size_t array_size = 1;
      for (int i = 0; i < var_ndims; ++i) off[i] = 0;

      if (e->GetKW(1) !=NULL)
      {
        DLongGDL *o=e->GetKWAs<DLongGDL>(1);
        int noff=o->N_Elements();
        for (int i = 0; i < noff; ++i) 
        {
          if ((*o)[i] < dim_length[var_ndims-(i+1)])
          {
            off[var_ndims-(i+1)]=(*o)[i];
          }
          else if ((*o)[i] <= 0)
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
      
      for (int i = 0; i < var_ndims; ++i) cou[i]=dim_length[i]-off[i];
	  
      if (e->GetKW(0) !=NULL)
      {
        DLongGDL *c=e->GetKWAs<DLongGDL>(0);
        int ncou=c->N_Elements();

        for (int i = 0; i < ncou; ++i) 
        {
          if ((*c)[i] < dim_length[trans[i]]-off[trans[i]]+1)
          {
            cou[trans[i]]=(*c)[i];
          }
          else if ((*c)[i] <= 0)
          {
            cou[trans[i]]=1;
            negzero_message("NCDF_VARGET: Count on",i,1);
          }
          else
          {
            cou[trans[i]]=dim_length[trans[i]]-off[trans[i]]+1;
            exceed_message("NCDF_VARGET", i, dim_length[trans[i]]-off[trans[i]]+1);
          }
        }
      }
	    
      if (e->GetKW(2) == NULL)
      {
        //NO STRIDE
        array_size = 1;
        for (int i = 0; i < var_ndims; ++i) 
        {
          transposed_dim_length[trans[i]] = cou[i];
          array_size = array_size * cou[i];
        }
        dimension dim(transposed_dim_length, (SizeT) var_ndims);
        if (var_type == NC_DOUBLE)
        {
          DDoubleGDL *temp = new DDoubleGDL(dim, BaseGDL::NOZERO);
          status=nc_get_vara_double(cdfid, varid, off, cou, &(*temp)[0]);
          ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp;      	
        }
        else if (var_type == NC_FLOAT)
        {
          DFloatGDL *temp = new DFloatGDL(dim, BaseGDL::NOZERO);
          status = nc_get_vara_float(cdfid, varid, off,cou,&(*temp)[0]);
          ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp;
        }
        else if (var_type == NC_SHORT)
        {
          DIntGDL *temp = new DIntGDL(dim, BaseGDL::NOZERO);
          status = nc_get_vara_short(cdfid, varid, off, cou, &(*temp)[0]);
          ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp;      	
        }
        else if(var_type == NC_INT)
        {
          DLongGDL *temp = new DLongGDL(dim,BaseGDL::NOZERO);
          status = nc_get_vara_int(cdfid, varid, off, cou, &(*temp)[0]);
          ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp;    
        }
        else if (var_type == NC_BYTE)
        {
          DByteGDL *temp=new DByteGDL(dim,BaseGDL::NOZERO);
          status = nc_get_vara_uchar(cdfid, varid, off, cou, &(*temp)[0]);
          ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp;      	
        }
        else if (var_type == NC_CHAR)
        {
          DByteGDL *temp = new DByteGDL(dim, BaseGDL::NOZERO);
          status = nc_get_vara_text(cdfid, varid, off, cou, (char*) &(*temp)[0]);
          ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp;
        }
      } 
      else 
      {
        //STRIDE
        ptrdiff_t stri[NC_MAX_VAR_DIMS];
        BaseGDL * stride;
        DIntGDL *s=e->GetKWAs<DIntGDL>(2);
        for (int i = 0; i < var_ndims; ++i) stri[i]=1;
        int nstri=s->N_Elements();
        for (int i = 0; i < nstri; ++i) 
        {
          if ((*s)[i]*cou[trans[i]] < dim_length[trans[i]]-off[trans[i]]+1)
          {
            //stride * count < length-offset
            stri[trans[i]]=(*s)[i];
          }
          else if ((*s)[i] <= 0)
          {
            //	stride<0, stop it now.
            stri[trans[i]]=1;
            e->Throw("STRIDE array cannot have negative elements"
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
              mess = "NCDF_VARGET: Requested read is larget than data in dimension ";
              mess+=i2s(i);
              mess+=". Reducing COUNT to ";
              mess+=i2s(cou[trans[i]]);
              mess+=".";
              Message(mess);
            }	
          }
        }

        //now we can get the damn data
        array_size=1;
        for (int i = 0; i < var_ndims; ++i) 
        {
          transposed_dim_length[trans[i]]=cou[i];
          array_size=array_size*cou[i];
        }
        dimension dim(transposed_dim_length, (SizeT) var_ndims);
	    	  
        if (var_type == NC_DOUBLE)
        {
          DDoubleGDL *temp = new DDoubleGDL(dim, BaseGDL::NOZERO);
          status=nc_get_vars_double(cdfid, varid, off, cou, stri, &(*temp)[0]);
          ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp; 
        }
        else if(var_type == NC_FLOAT)
        {
          DFloatGDL *temp = new DFloatGDL(dim,BaseGDL::NOZERO);
          status = nc_get_vars_float(cdfid, varid, off, cou, stri, &(*temp)[0]);
          ncdf_var_handle_error(e,status,"NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp; 
        }
        else if (var_type == NC_SHORT)
        {
          DIntGDL *temp = new DIntGDL(dim, BaseGDL::NOZERO);
          status = nc_get_vars_short(cdfid, varid, off, cou, stri, &(*temp)[0]);
          ncdf_var_handle_error(e,status,"NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp;      	
        }
        else if(var_type == NC_INT)
        {
          DLongGDL *temp = new DLongGDL(dim, BaseGDL::NOZERO);
          status = nc_get_vars_int(cdfid, varid, off,cou, stri, &(*temp)[0]);
          ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp;  
        }
        else if(var_type == NC_BYTE)
        {
          DByteGDL *temp=new DByteGDL(dim, BaseGDL::NOZERO);
          status = nc_get_vars_uchar(cdfid, varid, off, cou, stri, &(*temp)[0]);
          ncdf_var_handle_error(e,status,"NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp;      	
        }
        else if (var_type == NC_CHAR)
        {
          DByteGDL *temp = new DByteGDL(dim, BaseGDL::NOZERO);
          status = nc_get_vars_text(cdfid, varid, off, cou, stri, (char*) &(*temp)[0]);
          ncdf_var_handle_error(e, status, "NCDF_VARGET", temp);
          GDLDelete(e->GetParGlobal(2));
          e->GetParGlobal(2) = temp;      	
        }
      }
    }
  }



  BaseGDL* ncdf_vardef(EnvT* e)
  {
    size_t nParam=e->NParam(2);
    int dims[NC_MAX_VAR_DIMS], status, var_id,i,var_ndims;
    nc_type type;

    DLong cdfid;
    e->AssureLongScalarPar(0, cdfid);

    BaseGDL* v=e->GetParDefined(1);
    DString var_name;
    e->AssureStringScalarPar(1,var_name);

    for (i=0;i<NC_MAX_VAR_DIMS;++i) dims[i]=0;

    if(nParam == 3)
      {
	v=e->GetParDefined(2);
	DIntGDL* dim_in=static_cast<DIntGDL*>(v->Convert2(GDL_INT, BaseGDL::COPY));
	Guard<DIntGDL> dim_in_guard( dim_in);
	var_ndims=dim_in->N_Elements();
	if(var_ndims > NC_MAX_VAR_DIMS)
	  {
	    throw GDLException(e->CallingNode(),
			       "Too many elements error 1 in array"+e->GetParString(0));
	  }					      

	for (i=0; i<var_ndims;++i)
	  dims[var_ndims-i-1]=(*dim_in)[i];
	
	//dims is set
      } 
    else if(nParam == 2)
      {
	var_ndims=0;
	//dims is not set, scalar
      }

    if(e->KeywordSet(0))//GDL_BYTE
      type=NC_BYTE;
    else if(e->KeywordSet(1))//CHAR
      type=NC_CHAR;
    else if(e->KeywordSet(2))//GDL_DOUBLE
      type=NC_DOUBLE;
    else if(e->KeywordSet(4))//GDL_LONG
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
    if (p1->Type() != GDL_STRING) {
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
    int status, status_tr;

    size_t value_nelem, dim_length[MAXRANK];
    long trans[NC_MAX_VAR_DIMS], retrans[NC_MAX_VAR_DIMS];

    int var_dims[NC_MAX_VAR_DIMS], var_natts;
    char var_name[NC_MAX_NAME];

    nc_type var_type;

    //get the cdfid
    BaseGDL* v;

    DLong cdfid, varid;
    e->AssureLongScalarPar(0, cdfid);

    // Check type of varid
    BaseGDL* p1 = e->GetParDefined( 1);
    if (p1->Type() != GDL_STRING) {
      // Numeric
      e->AssureLongScalarPar(1, varid);
    } else {
      // String
      DString var_name;
      e->AssureScalarPar<DStringGDL>(1, var_name);
      status = nc_inq_varid(cdfid, var_name.c_str(), &varid);
      ncdf_handle_error(e, status, "NCDF_VARPUT");
    }

    int var_ndims;
    status = nc_inq_var(cdfid, varid, var_name, &var_type, 
      (int *) &var_ndims, var_dims, &var_natts);

    //get the value
    v = e->GetParDefined(2);
    value_nelem = v->N_Elements();
    for (int i = 0; i < var_ndims; ++i) 
    {
      if (v->Type() != GDL_STRING) dim_length[i] = max(int(v->Dim(i)), 1);
      else dim_length[i] = (*static_cast<DStringGDL*>(v))[0].length();
    }

    //do offset first
    if (var_ndims == 0)//scalar
    {
      trans[0] = 0;
      retrans[0] = 0;
    }
    else
    {
      for (int i=0; i < var_ndims; ++i)
      {
        trans[i] = var_ndims - i - 1;
        retrans[i] = i;
      }
    }

    // SA: no-keyword-arguments case handled by nc_put_var_* removed as
    //     * it relied on correctness of input data shape/length (segfaults),
    //     * it did not allow to insert a new record (in an unlimited dimension),
    //     * it is handled by the code below anyhow.

    //count, offset or stride
    size_t count[NC_MAX_VAR_DIMS], offset[NC_MAX_VAR_DIMS];
    int noff, ncou, nstri;
    ptrdiff_t stride[NC_MAX_VAR_DIMS];

    //setup
    for (int i = NC_MAX_VAR_DIMS; i--;) stride[i] = count[i] = offset[i] = 0; // not needed?
    if (var_ndims <= 1)
    {
      offset[0] = 0;
      count[0] = value_nelem;
      stride[0] = 1;
    }
    else
    {
      for (int i = 0; i < var_ndims; ++i)
      {
        offset[i] = 0;
        count[trans[i]] = dim_length[i];
        stride[i] = 1;
      }
    }

    if (e->GetKW(1) != NULL)
    {
      //offset
      DLongGDL *o = e->GetKWAs<DLongGDL>(1);
      noff = o->N_Elements();
      if (noff < var_ndims) e->Throw("Insufficient number of indices in OFFSET array (" + i2s(noff) + ", should be " + i2s(var_ndims) + ")");

      //  offset[0]=0;
      for (int i = 0; i < noff; ++i)
      {
        if ((*o)[i] > 0) offset[trans[i]] = (*o)[i];
        else if ((*o)[i] < 0)
        {
          offset[trans[i]] = 0;
          negzero_message("NCDF_VARPUT: Offset", i, 0);
        }
      }
    }

    long total = 1;
	  
    if (e->GetKW(0) != NULL)
    {
      DLongGDL *c = e->GetKWAs<DLongGDL>(0);
      ncou = c->N_Elements();
      for (int i = 0; i < ncou; ++i) 
      {
        if ((*c)[i] > 0) count[trans[i]] = (*c)[i];
        else if ((*c)[i] <= 0)
        {
          count[trans[i]] = 1;
          negzero_message("NCDF_VARPUT: Count ", i, 1);
        }
        total = total * count[trans[i]];
      }
    }
    else 
    {
      if (var_ndims == 0) total = value_nelem;
      else for (int i = 0; i < var_ndims; ++i) total = total * count[i];
    }

    if (total > value_nelem) e->Throw("Not enough elements (" 
      + i2s(total) + ">" + i2s(value_nelem) + ")");

    if (e->GetKW(2) != NULL)
    {
      //stride
      DIntGDL *s = e->GetKWAs<DIntGDL>(2);
      nstri=s->N_Elements();

      for (int i = 0; i < nstri; ++i) 
      {
        // stride * count < length-offset
        if ((*s)[i] > 0) stride[trans[i]] = (*s)[i];
        // stride < 0, stop it now.
        else if ((*s)[i] <= 0)
        {
          // stride[trans[i]] = 1;
          throw GDLException(e->CallingNode(),
            "NCDF_VARPUT: STRIDE array cannot have negative elements"
            +e->GetParString(0));
        }
      }
    }

    // TODO netCDF-4 has new data types
    switch (v->Type()) 
    { 
      // using netCDF API functions data type convertion
      case GDL_DOUBLE : 
        status = nc_put_vars_double(cdfid, varid, offset, count, stride, 
          &((*static_cast<DDoubleGDL*>(v))[0]));
        break;
      case GDL_FLOAT :
        status = nc_put_vars_float(cdfid, varid, offset, count, stride, 
          &((*static_cast<DFloatGDL*>(v))[0]));
        break;
      case GDL_INT : 
        status = nc_put_vars_short(cdfid, varid, offset, count, stride, 
          &((*static_cast<DIntGDL*>(v))[0]));
        break;
      case GDL_LONG :
        status = nc_put_vars_int(cdfid, varid, offset, count, stride, 
          &((*static_cast<DLongGDL*>(v))[0]));
        break;
      case GDL_BYTE :
        status = nc_put_vars_uchar(cdfid, varid, offset, count, stride,
          &((*static_cast<DByteGDL*>(v))[0]));
        break;
      // initially using GDL methods for data type convertion
      case GDL_COMPLEXDBL : 
      case GDL_COMPLEX :
      case GDL_UINT :
      case GDL_ULONG :
      case GDL_LONG64:
      case GDL_ULONG64 :
      {
        BaseGDL* val;
        Guard<BaseGDL> val_guard;
        switch (var_type) 
        {
          case NC_BYTE :   // 8-bit signed integer
          case NC_SHORT :  // 16-bit signed integer
            val = v->Convert2(GDL_INT, BaseGDL::COPY);
	    val_guard.Init(val);
            status = nc_put_vars_short(cdfid, varid, offset, count, stride,
              &((*static_cast<DIntGDL*>(val))[0])); 
            break;
          case NC_CHAR :   // 8-bit unsigned integer
            val = v->Convert2(GDL_BYTE, BaseGDL::COPY);
	    val_guard.Init(val);
            status = nc_put_vars_uchar(cdfid, varid, offset, count, stride,
              &((*static_cast<DByteGDL*>(val))[0])); 
            break;
          case NC_INT :    // 32-bit signed integer
            val = v->Convert2(GDL_LONG, BaseGDL::COPY);
	    val_guard.Init(val);
            status = nc_put_vars_int(cdfid, varid, offset, count, stride,
              &((*static_cast<DLongGDL*>(val))[0]));
            break;
          case NC_FLOAT :  // 32-bit floating point
            val = v->Convert2(GDL_FLOAT, BaseGDL::COPY);
	    val_guard.Init(val);
            status = nc_put_vars_float(cdfid, varid, offset, count, stride, 
              &((*static_cast<DFloatGDL*>(val))[0]));
            break;
          case NC_DOUBLE : // 64-bit floating point
            val = v->Convert2(GDL_DOUBLE, BaseGDL::COPY);
	    val_guard.Init(val);
            status = nc_put_vars_double(cdfid, varid, offset, count, stride,
              &((*static_cast<DDoubleGDL*>(val))[0]));
            break;
        }
        break;
      }
      case GDL_STRING :
        status = nc_put_vars_text(cdfid, varid, offset, count, stride, 
          (*static_cast<DStringGDL*>(v))[0].c_str());
        break;
      // reporting illegal types (could be done before...)
      case GDL_STRUCT :  
        e->Throw("Struct expression not allowed in this context: " 
          + e->GetParString(2));
      case GDL_PTR :
        e->Throw("Pointer expression not allowed in this context: " 
          + e->GetParString(2));
      case GDL_OBJ : 
        e->Throw("Object reference expression not allowed in this context: " 
          + e->GetParString(2));
    }
    ncdf_handle_error(e, status, "NCDF_VARPUT");
  }

}

#endif
