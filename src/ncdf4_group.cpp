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

#include <string>
//#include <fstream>
//#include <memory>

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
  
  BaseGDL* ncdf_groupsinq(EnvT* e)
  {
    size_t nParam=e->NParam(1);
    if(nParam != 1) {
      throw GDLException(e->CallingNode(),
			 "NCDF_GROUPSINQ: Wrong number of arguments.");
    }
    
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);
    
    int status;
    int numgrps;
    int ncids[NC_MAX_VAR_DIMS];
    // nc_inq_grps(int ncid, int *numgrps, int *ncids);

    status=nc_inq_grps(grpid, &numgrps, (int *) &ncids);
    //    cout << "numgrps :" << numgrps << endl;
    //cout << "ncids : " << ncids << endl;

    dimension dim(numgrps);
    DLongGDL *res = new DLongGDL(dim,BaseGDL::NOZERO);
    for (size_t i=0; i<numgrps; ++i) (*res)[ i] = ncids[i];

    return res;
    
  }

  BaseGDL* ncdf_groupdef(EnvT* e)
  {
    size_t nParam=e->NParam();
    if(nParam != 2) {
      throw GDLException(e->CallingNode(),
			 "NCDF_GROUPDEF: Wrong number of arguments.");
    }

    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);
    DString s;
    e->AssureScalarPar<DStringGDL>(1, s);

    int status;
    int new_grpid;

    status=nc_def_grp(grpid, s.c_str(), &new_grpid);
    return new DLongGDL(new_grpid);
}


  BaseGDL* ncdf_groupname(EnvT* e)
  {
    size_t nParam=e->NParam();
    if(nParam != 1) {
      throw GDLException(e->CallingNode(),
			 "NCDF_GROUPNAME: Wrong number of arguments.");
    }

    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    int status;
    char groupname[NC_MAX_NAME];

    status=nc_inq_grpname(grpid, groupname);

    return new DStringGDL(groupname);

  }

  BaseGDL* ncdf_fullgroupname(EnvT* e)
  {
    size_t nParam=e->NParam();
    if(nParam != 1) {
      throw GDLException(e->CallingNode(),
			 "NCDF_FULLGROUPNAME: Wrong number of arguments.");
    }
    
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    int status;
    char fullgroupname[NC_MAX_NAME];
    size_t lenp;
    status=nc_inq_grpname_full(grpid, &lenp, fullgroupname);

    return new DStringGDL(fullgroupname);
  }

  BaseGDL* ncdf_groupparent(EnvT* e)
  {
    size_t nParam=e->NParam();
    if(nParam != 1) {
      throw GDLException(e->CallingNode(),
			 "NCDF_GROUPPARENT: Wrong number of arguments.");
    }
    
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    int status;
    int new_grpid;
    status=nc_inq_grp_parent(grpid, &new_grpid);

    return new DLongGDL(new_grpid);
  }

  BaseGDL* ncdf_dimidsinq(EnvT* e)
  {

    size_t nParam=e->NParam();
    if(nParam != 1) {
      throw GDLException(e->CallingNode(),
			 "NCDF_DIMIDSINQ: Wrong number of arguments.");
    }
    
    DLong grpid;
    e->AssureLongScalarPar( 0, grpid);

    int include_parents=0;
    if (e->KeywordSet("INCLUDE_PARENTS")) {include_parents=1;}

    int status;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];

    status=nc_inq_dimids(grpid, &ndims, (int *) &dimids, include_parents);
    //status=nc_inq_dimids(grpid, int *ndims, int *dimids, include_parents);
 
    dimension dim(ndims);
    DLongGDL *res = new DLongGDL(dim,BaseGDL::NOZERO);
    for (size_t i=0; i<ndims; ++i) (*res)[ i] = dimids[i];

    return res;
   
    //   cout << "not ready " << endl;

    // status=nc_inq_dimids(grpid, &ndims, (int *) &dimids, include_parents);

    //    return new DLongGDL(-104);
  }
  BaseGDL* ncdf_ncidinq(EnvT* e)
  {
    cout << "not ready " << endl;
    return new DLongGDL(-105);
  }
  BaseGDL* ncdf_varidsinq(EnvT* e)
  {
    cout << "not ready " << endl;
    return new DLongGDL(-106);
  }

}
#endif
