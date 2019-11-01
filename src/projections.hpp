/* *************************************************************************
   projections.hpp  -  GDL routines for projections
   -------------------
   begin                : July 22 2002
   copyright            : (C) 2002 by Marc Schellens
   email                : m_schellens@users.sf.net
***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROJECTIONS_HPP_
#define PROJECTIONS_HPP_


#include "envt.hpp"
#include "initsysvar.hpp"
#include <limits>
#include <list>

namespace lib {

  using namespace std;

  BaseGDL* map_proj_forward_fun( EnvT* e);
  BaseGDL* map_proj_inverse_fun( EnvT* e);

#if defined(USE_LIBPROJ4) || defined(USE_LIBPROJ4_NEW) 

  //dummy functions for compatibility support of GCTP projections 
  void map_proj_gctp_forinit (EnvT* e);
  void map_proj_gctp_revinit (EnvT* e);

#define GDL_COMPLEX COMPLEX2

#ifdef USE_LIBPROJ4_NEW
  extern "C" {
    //#include "projects.h"
   #define ACCEPT_USE_OF_DEPRECATED_PROJ_API_H 1
#include "proj_api.h"
  }
#define LPTYPE projLP
#define XYTYPE projXY

#define PROJTYPE projPJ
#define PROJDATA projUV
#define PJ_INIT pj_init

  PROJDATA protect_proj_fwd (PROJDATA idata, PROJTYPE proj);
  PROJDATA protect_proj_inv (PROJDATA idata, PROJTYPE proj);

#define PJ_FWD protect_proj_fwd
#define LIB_PJ_FWD pj_fwd
#define PJ_INV protect_proj_inv 
#define LIB_PJ_INV pj_inv
  PROJTYPE map_init(DStructGDL *map=SysVar::Map());
  static volatile PROJTYPE ref;
  static volatile PROJTYPE prev_ref;
  static PROJDATA badProj={sqrt(-1),sqrt(-1)};
#else
  extern "C" {
    //adding this removes the problem with lam,phi vs. x,y and make no diffs between old an new lib proj.4    
#define PROJ_UV_TYPE 1
#include "lib_proj.h"
  }
#define LPTYPE PROJ_LP
#define XYTYPE PROJ_XY

#define PROJTYPE PROJ*
#define PROJDATA PROJ_UV
#define PJ_INIT proj_init

  PROJDATA protect_proj_fwd (PROJDATA idata, PROJTYPE proj);
  PROJDATA protect_proj_inv (PROJDATA idata, PROJTYPE proj);

#define PJ_FWD protect_proj_fwd
#define LIB_PJ_FWD proj_fwd
#define PJ_INV protect_proj_inv
#define LIB_PJ_INV proj_inv
  PROJTYPE map_init(DStructGDL *map=SysVar::Map());
  static PROJTYPE ref;
  static PROJTYPE prev_ref;
  static PROJDATA badProj={sqrt(-1),sqrt(-1)};
  //general reprojecting function
#endif

  //general reprojecting functions
  DDoubleGDL* gdlApplyFullProjection(PROJTYPE ref, DStructGDL* map,
			     DDoubleGDL* lon, DDoubleGDL *lat);
  void gdlFullProjectionTransformation(PROJTYPE ref, DStructGDL* map,
			     DDoubleGDL* lon, DDoubleGDL *lat);
  DDoubleGDL* gdlProjForward(PROJTYPE ref, DStructGDL* map,
			     DDoubleGDL* lon, DDoubleGDL *lat, DLongGDL* connectivity,
			     bool doConn, DLongGDL* &gons, bool doGons,
			     DLongGDL* &lines, bool doLines, bool doFill);
  DStructGDL *GetMapAsMapStructureKeyword(EnvT *e, bool &externalMap); //not static since KW is at same place for all uses.

#define COMPLEX2 GDL_COMPLEX
#else //NOT USE_LIBPROJ4 : define some more or less dummy values:
#define PROJTYPE void*
#define DEG_TO_RAD 0.017453292
#endif //USE_LIBPROJ4

} // namespace

#endif
