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

#undef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#undef MAX
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#undef ABS
#define ABS(a) (((a) < 0) ? -(a) : (a))

namespace lib {

  using namespace std;

  BaseGDL* map_proj_forward_fun( EnvT* e);
  BaseGDL* map_proj_inverse_fun( EnvT* e);

#if defined(USE_LIBPROJ)

  //dummy functions for compatibility support of GCTP projections 
  void map_proj_gctp_forinit (EnvT* e);
  void map_proj_gctp_revinit (EnvT* e);

#define GDL_COMPLEX COMPLEX2

  extern "C" {
#if LIBPROJ_MAJOR_VERSION >= 5
#include "proj.h"
#else
#include "proj_api.h"
#endif
  }
#if LIBPROJ_MAJOR_VERSION >= 5
#define LPTYPE PJ_LP
#define XYTYPE PJ_XY
#define PROJTYPE PJ*
#define PROJDATA PJ_UV

  PJ_XY protect_proj_fwd_lp (PJ_LP idata, PJ *proj);
  PJ_XY protect_proj_fwd_xy (PJ_XY idata, PJ *proj);
  PJ_LP protect_proj_inv_xy (XYTYPE idata, PJ *proj);
  static PJ_XY badProjXY={sqrt(-1),sqrt(-1)};
  static PJ_LP badProjLP={sqrt(-1),sqrt(-1)};

  static PJ *ref;
  static PJ *prev_ref;

#define DEG_TO_RAD   .017453292519943296
#define RAD_TO_DEG    57.295779513082321
#else
#define LPTYPE projLP
#define XYTYPE projXY
#define PROJTYPE projPJ
#define PROJDATA projUV

  PROJDATA protect_proj_fwd (PROJDATA idata, PROJTYPE proj);
  PROJDATA protect_proj_inv (PROJDATA idata, PROJTYPE proj);
  static PROJDATA badProj={sqrt(-1),sqrt(-1)};

#define PJ_FWD protect_proj_fwd
#define LIB_PJ_FWD pj_fwd
#define PJ_INV protect_proj_inv 
#define LIB_PJ_INV pj_inv
  static volatile PROJTYPE ref;
  static volatile PROJTYPE prev_ref;
#endif
  PROJTYPE map_init(DStructGDL *map=SysVar::Map());

  //general reprojecting functions
  DDoubleGDL* gdlApplyFullProjection(PROJTYPE ref, DStructGDL* map,
			     DDoubleGDL* lon, DDoubleGDL *lat);
  void gdlFullProjectionTransformation(PROJTYPE ref, DStructGDL* map,
			     DDoubleGDL* lon, DDoubleGDL *lat);
  DDoubleGDL* gdlProjForward(PROJTYPE ref, DStructGDL* map,
			     DDoubleGDL* lon, DDoubleGDL *lat,  DDoubleGDL *z, DLongGDL* connectivity,
			     bool doConn, DLongGDL* &gons, bool doGons,
			     DLongGDL* &lines, bool doLines, bool doFill, bool transpose);
  DStructGDL *GetMapAsMapStructureKeyword(EnvT *e, bool &externalMap); //not static since KW is at same place for all uses.

#define COMPLEX2 GDL_COMPLEX
#else //NOT USE_LIBPROJ : define some more or less dummy values:
#define PROJTYPE void*
#define DEG_TO_RAD 0.017453292
#endif //USE_LIBPROJ

} // namespace

#endif
