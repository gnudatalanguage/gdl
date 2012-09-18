/***************************************************************************
                       plotting.cpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002-2011 by Marc Schellens et al.
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"
#include "plotting.hpp"
#include "math_utl.hpp"

namespace lib {

  using namespace std;

  template< typename T1, typename T2>
  BaseGDL* convert_coord_template( EnvT* e, 
				   BaseGDL* p0, BaseGDL* p1, BaseGDL* p2,
				   DDouble *sx, DDouble *sy, DDouble *sz,
				   DLong xv, DLong yv, DLong xt, DLong yt)
  {
    DLong dims[2]={3,0};
    if( e->NParam() == 1) {
      if (p0->Dim(0) != 2 && p0->Dim(0) != 3)
	e->Throw( "When only 1 param, dims must be (2,n) or (3,n)");
    }

    DType aTy;
    if (p0->Type() == GDL_DOUBLE || e->KeywordSet("DOUBLE"))
      aTy = GDL_DOUBLE;
    else
      aTy = GDL_FLOAT;

    T1* res;
    SizeT nrows;

    if (p0->Rank() == 0) {
      nrows = 1;
      dimension dim((DLong *) dims, 1);
      res = new T1( dim, BaseGDL::ZERO);
    } else if (p0->Rank() == 1) {
      if (e->NParam() == 1) {
	nrows = 1;
	dimension dim((DLong *) dims, 1);
	res = new T1( dim, BaseGDL::ZERO);
      } else {
	nrows = p0->Dim(0);
	dims[1] = nrows;
	dimension dim((DLong *) dims, 2);
	res = new T1( dim, BaseGDL::ZERO);
      }
    } else {
      // rank == 2
      nrows = 1;
      for( SizeT i = 0; i<2; ++i) {	
	nrows  *= p0->Dim(i);
      }
      dims[1] = nrows;
      dimension dim((DLong *) dims, 2);
      res = new T1( dim, BaseGDL::ZERO);
    }

    T1 *in, *in1, *in2, *in3;
    T2 *ptr1, *ptr2, *ptr3;
    DLong deln=1, ires=0;
    bool third = false;
    if( e->NParam() == 1) { 
      in = static_cast<T1*>(p0->Convert2( aTy, BaseGDL::COPY));
      ptr1 = &(*in)[0];
      ptr2 = &(*in)[1];
      if (p0->Dim(0) == 3) {
	ptr3 = &(*in)[2];
	third = true;
      }
      deln = p0->Dim(0);
    } else if( e->NParam() == 2) {
      in1 = static_cast<T1*>(p0->Convert2( aTy, BaseGDL::COPY));
      in2 = static_cast<T1*>(p1->Convert2( aTy, BaseGDL::COPY));
      ptr1 = &(*in1)[0];
      ptr2 = &(*in2)[0];
      ptr3 = NULL;
    } else {
      in1 = static_cast<T1*>(p0->Convert2( aTy, BaseGDL::COPY));
      in2 = static_cast<T1*>(p1->Convert2( aTy, BaseGDL::COPY));
      in3 = static_cast<T1*>(p2->Convert2( aTy, BaseGDL::COPY));
      ptr1 = &(*in1)[0];
      ptr2 = &(*in2)[0];
      ptr3 = &(*in3)[0];
      third = true;
    }

#ifdef USE_LIBPROJ4
    // MAP conversion (xt = 3)
    if (xt == 3) {
      ref = map_init();
      if ( ref == NULL) {
	e->Throw( "Projection initialization failed.");
      }

      // ll -> xy
      // lam = longitude  phi = latitude
      if (e->KeywordSet("DATA") || (!e->KeywordSet("DEVICE") && 
				    !e->KeywordSet("NORMAL"))) {
	if (!e->KeywordSet("TO_DEVICE") && 
	    !e->KeywordSet("TO_NORMAL")) {
	  for( SizeT i = 0; i<nrows; ++i) {	
	    (*res)[ires++] = (*ptr1);
	    (*res)[ires++] = (*ptr2);
	    ptr1++;
	    ptr2++;
	    ires++;
	  }
	} else if (e->KeywordSet("TO_NORMAL")) {
	  LPTYPE idata;
	  XYTYPE odata;
	  for( SizeT i = 0; i<nrows; ++i) {	
	    idata.lam = (*ptr1) * DEG_TO_RAD;
	    idata.phi = (*ptr2) * DEG_TO_RAD;
	    odata = PJ_FWD(idata, ref);
	    (*res)[ires++] = odata.x * sx[1] + sx[0];
	    (*res)[ires++] = odata.y * sy[1] + sy[0];
	    ptr1++;
	    ptr2++;
	    ires++;
	  }
	} else if (e->KeywordSet("TO_DEVICE")) {
	  LPTYPE idata;
	  XYTYPE odata;
	  for( SizeT i = 0; i<nrows; ++i) {	
	    idata.lam = (*ptr1) * DEG_TO_RAD;
	    idata.phi = (*ptr2) * DEG_TO_RAD;
	    odata = PJ_FWD(idata, ref);
	    (*res)[ires++] = xv * (odata.x * sx[1] + sx[0]);
	    (*res)[ires++] = yv * (odata.y * sy[1] + sy[0]);
	    ptr1++;
	    ptr2++;
	    ires++;
	  }
	}
	// xy -> ll
      } else if (e->KeywordSet("NORMAL")) {
        if (e->KeywordSet("TO_DEVICE")) {
          for( SizeT i = 0; i<nrows; ++i) {
            (*res)[ires++] = xv * (*ptr1);
            (*res)[ires++] = yv * (*ptr2);
            if (third)
              (*res)[ires++] = (*ptr3);
            else
              ires++;
            ptr1 += deln;
            ptr2 += deln;
            ptr3 += deln;
          }
        } else {
	  XYTYPE idata;
	  LPTYPE odata;
	  for( SizeT i = 0; i<nrows; ++i) {	
	    idata.x = ((*ptr1) - sx[0]) / sx[1];
	    idata.y = ((*ptr2) - sy[0]) / sy[1];
	    odata = PJ_INV(idata, ref);
	    (*res)[ires++] = odata.lam * RAD_TO_DEG;
	    (*res)[ires++] = odata.phi * RAD_TO_DEG;
	    ptr1++;
	    ptr2++;
	    ires++;
	  }
        }
      } else if (e->KeywordSet("DEVICE")) {
	XYTYPE idata;
	LPTYPE odata;
	for( SizeT i = 0; i<nrows; ++i) {	
	  idata.x = ((*ptr1) / xv - sx[0]) / sx[1];
	  idata.y = ((*ptr2) / yv - sy[0]) / sy[1];
	  odata = PJ_INV(idata, ref);
	  (*res)[ires++] = odata.lam * RAD_TO_DEG;
	  (*res)[ires++] = odata.phi * RAD_TO_DEG;
	  ptr1++;
	  ptr2++;
	  ires++;
	}
      }
      // Change Inf to Nan
      {
        static DStructGDL *Values = SysVar::Values();
        DDouble d_nan = (*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0];
        for( SizeT i = 0; i<res->N_Elements(); ++i) {	
          if (isinf((DDouble) (*res)[i]) != 0) (*res)[i] = d_nan;
        }
      }
      return res;
    }
#endif

    // in: DATA  out: NORMAL/DEVICE
    if (e->KeywordSet("DATA") || (!e->KeywordSet("DEVICE") && 
	!e->KeywordSet("NORMAL"))) {
      for( SizeT i = 0; i<nrows; ++i) {	
	if (xt == 0)
	  (*res)[ires++] = xv * (sx[0] + sx[1] * (*ptr1));
	else
	  (*res)[ires++] = xv * (sx[0] + sx[1] * log10((*ptr1)));

	if (yt == 0)
	  (*res)[ires++] = yv * (sy[0] + sy[1] * (*ptr2));
	else
	  (*res)[ires++] = yv * (sy[0] + sy[1] * log10((*ptr2)));

	if (third)
	  (*res)[ires++] = sz[0] + sz[1] * (*ptr3);
	else
	  ires++;
	ptr1 += deln;
	ptr2 += deln;
	ptr3 += deln;
      }
    // in: NORMAL  out: DEVICE/DATA
    } else if (e->KeywordSet("NORMAL")) {
      if (e->KeywordSet("TO_DEVICE")) {
	for( SizeT i = 0; i<nrows; ++i) {
	  (*res)[ires++] = xv * (*ptr1);
	  (*res)[ires++] = yv * (*ptr2);
	  if (third)
	    (*res)[ires++] = (*ptr3);
	  else
	    ires++;
	  ptr1 += deln;
	  ptr2 += deln;
	  ptr3 += deln;
	}
      } else if (!e->KeywordSet("TO_NORMAL")) {
	for( SizeT i = 0; i<nrows; ++i) {
	  if (xt == 0)
	    (*res)[ires++] = ((*ptr1) - sx[0]) / sx[1];
	  else
	    (*res)[ires++] = pow(10.,((*ptr1) - sx[0]) / sx[1]);
	
	  if (yt == 0)
	    (*res)[ires++] = ((*ptr2) - sy[0]) / sy[1];
	  else
	    (*res)[ires++] = pow(10.,((*ptr2) - sy[0]) / sy[1]);

	  if (third)
	    (*res)[ires++] = ((*ptr3) - sz[0]) / sz[1];
	  else
	    ires++;
	  ptr1 += deln;
	  ptr2 += deln;
	  ptr3 += deln;
	}
      }
    // in: DEVICE  out: NORMAL/DATA
    } else if (e->KeywordSet("DEVICE")) {
      if (e->KeywordSet("TO_NORMAL")) {
	for( SizeT i = 0; i<nrows; ++i) {
	  (*res)[ires++] = (*ptr1) / xv;
	  (*res)[ires++] = (*ptr2) / yv;
	  if (third)
	    (*res)[ires++] = (*ptr3);
	  else
	    ires++;
	  ptr1 += deln;
	  ptr2 += deln;
	  ptr3 += deln;
	}
      } else if (!e->KeywordSet("TO_DEVICE")) {
	for( SizeT i = 0; i<nrows; ++i) {
	  if (xt == 0)
	    (*res)[ires++] = ((*ptr1) / xv - sx[0]) / sx[1];
	  else
	    (*res)[ires++] = pow(10.,((*ptr1) / xv - sx[0]) / sx[1]);
	  
	  if (yt == 0)
	    (*res)[ires++] = ((*ptr2) / yv - sy[0]) / sy[1];
	  else
	    (*res)[ires++] = pow(10.,((*ptr2) / yv - sy[0]) / sy[1]);
	  
	  if (third)
	    (*res)[ires++] = ((*ptr3) - sz[0]) / sz[1];
	  else
	    ires++;
	  ptr1 += deln;
	  ptr2 += deln;
	  ptr3 += deln;
	}
      }	
    }
    return res;
  }

  BaseGDL* convert_coord( EnvT* e) 
  {
    SizeT nParam=e->NParam();
    if( nParam < 1)
      e->Throw( "Incorrect number of arguments.");

    BaseGDL* p0;
    BaseGDL* p1;
    BaseGDL* p2;

    p0 = e->GetParDefined( 0);
    if (nParam >= 2)
      p1 = e->GetParDefined( 1);
    if (nParam == 3)
      p2 = e->GetParDefined( 2);

    DDouble *sx, *sy;
    GetSFromPlotStructs(&sx, &sy);

    static DStructGDL* zStruct = SysVar::Z();
    static unsigned szTag = zStruct->Desc()->TagIndex( "S");
    DDouble *sz = &(*static_cast<DDoubleGDL*>( zStruct->GetTag( szTag, 0)))[0];

    static DStructGDL* xStruct = SysVar::X();
    static DStructGDL* yStruct = SysVar::Y();
    static unsigned xtTag = xStruct->Desc()->TagIndex( "TYPE");
    static unsigned ytTag = yStruct->Desc()->TagIndex( "TYPE");
    static unsigned ztTag = zStruct->Desc()->TagIndex( "TYPE");
    DLong xt = (*static_cast<DLongGDL*>( xStruct->GetTag( xtTag, 0)))[0];
    DLong yt = (*static_cast<DLongGDL*>( yStruct->GetTag( ytTag, 0)))[0];

    DLong xv=1, yv=1;
    int xSize, ySize, xPos, yPos;
    // Use Size in lieu of VSize
    Graphics* actDevice = Graphics::GetDevice();
    DLong wIx = actDevice->ActWin();
    if( wIx == -1) {
      DStructGDL* dStruct = SysVar::D();
      static unsigned xsizeTag = dStruct->Desc()->TagIndex( "X_SIZE");
      static unsigned ysizeTag = dStruct->Desc()->TagIndex( "Y_SIZE");
      xSize = (*static_cast<DLongGDL*>( dStruct->GetTag( xsizeTag, 0)))[0];
      ySize = (*static_cast<DLongGDL*>( dStruct->GetTag( ysizeTag, 0)))[0];
    } else {
      bool success = actDevice->WSize(wIx, &xSize, &ySize, &xPos, &yPos);
    }
    if ( e->KeywordSet("DEVICE") || e->KeywordSet("TO_DEVICE")) {
      xv = xSize;
      yv = ySize;
    }

    /*
    static xVSTag = dSysVarDesc->TagIndex( "X_VSIZE");
    static yVSTag = dSysVarDesc->TagIndex( "Y_VSIZE");

    DLong xv = (*static_cast<DLongGDL*>( dStruct->GetTag( xvTag, 0)))[0];
    DLong yv = (*static_cast<DLongGDL*>( dStruct->GetTag( yvTag, 0)))[0];
    */

    if (p0->Type() == GDL_DOUBLE || e->KeywordSet("DOUBLE")) {
      return convert_coord_template<DDoubleGDL, DDouble>
	( e, p0, p1, p2, sx, sy, sz, xv, yv, xt, yt);
    } else {
      return convert_coord_template<DFloatGDL, DFloat>
	( e, p0, p1, p2, sx, sy, sz, xv, yv, xt, yt);
    }
  }

} // namespace
