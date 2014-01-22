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

#include "datatypes.hpp"
#include "envt.hpp"

#include "ncdf_cl.hpp"

#define GDL_DEBUG
//#undef GDL_DEBUG

#ifdef USE_NETCDF4

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
      cout << "NCDF_GROUPSINQ : " << status <<endl;
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
 
    // it is mandatory to have 2 parameters !
    size_t nParam=e->NParam(2);

    // in fact, we can use the "grpid" to check the file format it-self.
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    DString s;
    e->AssureScalarPar<DStringGDL>(1, s);

    // before going further we have to chech the file format, must be NetCDF-4

    int status;
    int fileformat;
    status=nc_inq_format(grpid, &fileformat);
    ncdf_handle_error(e, status,"NCDF_NCIDINQ");

    if (fileformat == NC_FORMAT_CLASSIC) 
      Warning("NCDF_NCIDINQ: NetCDF 3 Classic format found. not OK");
    if (fileformat == NC_FORMAT_64BIT)
      Warning("NCDF_NCIDINQ: NetCDF 3 64-BIT format found. not OK");
    
    if ((fileformat == NC_FORMAT_64BIT) || (fileformat == NC_FORMAT_CLASSIC)) {
      return new DLongGDL(-1);
    }

    int sub_grpid;
    status=nc_inq_ncid(grpid, s.c_str(), &sub_grpid);

    if (status != 0) {
      if (status == -125) {
	Warning("NCDF_NCIDINQ: No group found. (NC_ERROR=-125)");      
	return new DLongGDL(-1);
      } else {
	ncdf_handle_error(e, status,"NCDF_NCIDINQ");
      }
    }
    return new DLongGDL(sub_grpid);

  }
  
  BaseGDL* ncdf_varidsinq(EnvT* e)
  {
    //size_t nParam=e->NParam(1);
    
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    int status;
    int nvars, varids[NC_MAX_VAR_DIMS];

    status=nc_inq_varids(grpid, &nvars, (int *) &varids);

    int debug=0;
    if (debug) {
      cout << "NCDF_VARIDSINQ: status : " << status << endl;
      cout << "NCDF_VARIDSINQ: nvars : " << nvars << endl;
      cout << "NCDF_VARIDSINQ: *varId : ";
      for (size_t i=0; i<nvars; ++i) cout << varids[i];
      cout << endl;
    }
    ncdf_handle_error(e, status,"NCDF_VARIDSINQ");

    if (nvars > 0) { 
      dimension dim(nvars);
      DLongGDL *res = new DLongGDL(dim,BaseGDL::NOZERO);
      for (size_t i=0; i<nvars; ++i) (*res)[ i] = varids[i];
      return res;
    } else {
      return new DLongGDL(-1);
    }   
  }
  
  BaseGDL* ncdf_unlimdimsinq(EnvT* e)
  {
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    // first of all, we check whether the Group ID is a good one
    int status;
    char groupname[NC_MAX_NAME];

    status=nc_inq_grpname(grpid, groupname);
    ncdf_handle_error(e, status,"NCDF_GROUPNAME");

    int nunlimdimsp, unlimdimidsp[NC_MAX_VAR_DIMS];
    status=nc_inq_unlimdims(grpid, &nunlimdimsp, (int *) &unlimdimidsp);
    ncdf_handle_error(e, status,"NCDF_UNLIMDIMSINQ");

    int debug=0;
    if (debug) {
      cout << "NCDF_UNLIMDIMSINQ: status : " << status << endl;
      cout << "NCDF_UNLIMDIMSINQ: nunlimdimsp : " << nunlimdimsp << endl;
      cout << "NCDF_UNLIMDIMSINQ: *unlimdimidsp : ";
      for (size_t i=0; i<nunlimdimsp; ++i) cout << unlimdimidsp[i];
      cout << endl;
    }

    // AC: having no test cases up to now, I don't know if I have to return -1
    static int countIx = e->KeywordIx("COUNT");
    if (e->KeywordPresent(countIx)) {
      int count=0;
      if (nunlimdimsp > 0) count=nunlimdimsp;
       e->SetKW(countIx, new DLongGDL(count));
    }

    if (nunlimdimsp > 0) { 
      dimension dim(nunlimdimsp);
      DLongGDL *res = new DLongGDL(dim,BaseGDL::NOZERO);
      for (size_t i=0; i<nunlimdimsp; ++i) (*res)[ i] =unlimdimidsp[i];
      return res;
    } else {
      return new DLongGDL(-1);
    }    
  }
  
}
#endif //USE_NETCDF4
#endif //USE_NETCDF
