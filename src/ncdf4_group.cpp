/***************************************************************************
                          ncdf4_group.cpp  -  NetCDF GDL library function
                             -------------------
    begin                : January 8 2014
    copyright            : (C) 2014 by Alain Coulais
    email                : 
    purpose              : this file contains some extensions 
                           provided by NetCDF-4, e.g. group
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

//#include <string>
//#include <fstream>
//#include <memory>

#include "datatypes.hpp"
#include "envt.hpp"

#include "ncdf_cl.hpp"

#define GDL_DEBUG
//#undef GDL_DEBUG

namespace lib {

  using namespace std;
  using namespace antlr;
  
  BaseGDL* ncdf_groupsinq(EnvT* e)
  {
    size_t nParam=e->NParam(1);
     
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);
    
    int status;
    int numgrps;
    int ncids[NC_MAX_VAR_DIMS];

    status=nc_inq_grps(grpid, &numgrps, (int *) &ncids);
    ncdf_handle_error(e, status,"NCDF_GROUPSINQ");
    
    int debug=0;
    if (debug) {
      cout << "NetCDF status : " << status <<endl;
      cout << "numgrps :" << numgrps << endl;
      cout << "ncids : ";
      for (size_t i=0; i<numgrps; ++i) cout <<  ncids[i];
      cout << endl;
    }

    if (numgrps > 0) { 
      dimension dim(numgrps);
      DLongGDL *res = new DLongGDL(dim,BaseGDL::NOZERO);
      for (size_t i=0; i<numgrps; ++i) (*res)[ i] = ncids[i];
      return res;
    } else {
      return new DLongGDL(-1);
    }    
  }

  BaseGDL* ncdf_groupdef(EnvT* e)
  {
    size_t nParam=e->NParam(2);

    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    DString s;
    e->AssureScalarPar<DStringGDL>(1, s);

    int status;
    int new_grpid;

    status=nc_def_grp(grpid, s.c_str(), &new_grpid);
    ncdf_handle_error(e, status,"NCDF_GROUPDEF");

    return new DLongGDL(new_grpid);
}


  BaseGDL* ncdf_groupname(EnvT* e)
  {
    size_t nParam=e->NParam(1);

    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    int status;
    char groupname[NC_MAX_NAME];

    status=nc_inq_grpname(grpid, groupname);
    ncdf_handle_error(e, status,"NCDF_GROUPNAME");

    return new DStringGDL(groupname);

  }

  BaseGDL* ncdf_fullgroupname(EnvT* e)
  {
    size_t nParam=e->NParam(1);
    
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    int status;
    char fullgroupname[NC_MAX_NAME];
    size_t lenp;
    status=nc_inq_grpname_full(grpid, &lenp, fullgroupname);
    ncdf_handle_error(e, status,"NCDF_GROUPFULLNAME");

    return new DStringGDL(fullgroupname);
  }

  BaseGDL* ncdf_groupparent(EnvT* e)
  {
    size_t nParam=e->NParam(1);
    
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    int status;
    int new_grpid;
    status=nc_inq_grp_parent(grpid, &new_grpid);
    ncdf_handle_error(e, status,"NCDF_GROUPPARENT");

    return new DLongGDL(new_grpid);
  }

  BaseGDL* ncdf_dimidsinq(EnvT* e)
  {
    size_t nParam=e->NParam();
    
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    int include_parents=0;
    if (e->KeywordSet("INCLUDE_PARENTS")) {include_parents=1;}

    int status;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];

    status=nc_inq_dimids(grpid, &ndims, (int *) &dimids, include_parents);
    ncdf_handle_error(e, status,"NCDF_DIMIDSINQ");

    dimension dim(ndims);
    DLongGDL *res = new DLongGDL(dim,BaseGDL::NOZERO);
    for (size_t i=0; i<ndims; ++i) (*res)[ i] = dimids[i];

    return res;   
  }

  BaseGDL* ncdf_ncidinq(EnvT* e)
  {
 
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    DString s;
    e->AssureScalarPar<DStringGDL>(1, s);



    DLong ncid;
    e->AssureLongScalarPar( 0, ncid);
 
    // before going further we have to chech the file format, must be NetCDF-4

    int status;
    int fileformat;
    status=nc_inq_format(ncid, &fileformat);
    ncdf_handle_error(e, status,"NCDF_NCISINQ");

    if (fileformat == NC_FORMAT_CLASSIC) Warning("NetCDF 3 Classic format found. not OK");
    if (fileformat == NC_FORMAT_64BIT) Warning("NetCDF 3 64BIT format found. not OK");
    
    if ((fileformat == NC_FORMAT_64BIT) || (fileformat == NC_FORMAT_CLASSIC)) {
	return new DLongGDL(-1);

    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    DString s;
    e->AssureScalarPar<DStringGDL>(1, s);


    }


   cout << "not ready " << endl;
    return new DLongGDL(-105);
  }

  BaseGDL* ncdf_varidsinq(EnvT* e)
  {
    cout << "not ready " << endl;
    return new DLongGDL(-106);
  }

  BaseGDL* ncdf_unlimdimsinq(EnvT* e)
  {
    cout << "not ready " << endl;
    return new DLongGDL(-106);
  }

}
#endif
