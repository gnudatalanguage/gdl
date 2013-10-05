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

namespace lib
{

  using namespace std;

  BaseGDL* map_proj_forward_fun(EnvT* e)
  {
#ifdef USE_LIBPROJ4
    // lonlat -> xy

    SizeT nParam=e->NParam();
    if ( nParam<1 )
      e->Throw("Incorrect number of arguments.");

    LPTYPE idata;
    XYTYPE odata;

    ref=map_init();
    if ( ref==NULL )
    {
      e->Throw("Projection initialization failed.");
    }

    BaseGDL* p0;
    BaseGDL* p1;

    DDoubleGDL* lon;
    DDoubleGDL* lat;
    DDoubleGDL* ll;
    DDoubleGDL* res;
    DLong dims[2];

    if ( nParam==1 )
    {
      p0=e->GetParDefined(0);
      DDoubleGDL* ll=static_cast<DDoubleGDL*>
      (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));

      dims[0]=2;
      if ( p0->Rank()==1 )
      {
        dimension dim((DLong *)dims, 1);
        res=new DDoubleGDL(dim, BaseGDL::NOZERO);
      }
      else
      {
        dims[1]=p0->Dim(1);
        dimension dim((DLong *)dims, 2);
        res=new DDoubleGDL(dim, BaseGDL::NOZERO);
      }

      SizeT nEl=p0->N_Elements();
      for ( SizeT i=0; i<nEl/2; ++i )
      {
        idata.lam=(*ll)[2*i]*DEG_TO_RAD;
        idata.phi=(*ll)[2*i+1]*DEG_TO_RAD;
        odata=PJ_FWD(idata, ref);
        (*res)[2*i]=odata.x;
        (*res)[2*i+1]=odata.y;
      }
      return res;

    }
    else if ( nParam==2 )
    {
      p0=e->GetParDefined(0);
      p1=e->GetParDefined(1);
      DDoubleGDL* lon=static_cast<DDoubleGDL*>
      (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      DDoubleGDL* lat=static_cast<DDoubleGDL*>
      (p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));

      dims[0]=2;
      if ( p0->Rank()==0||p0->Rank()==1 )
      {
        dimension dim((DLong *)dims, 1);
        res=new DDoubleGDL(dim, BaseGDL::NOZERO);
      }
      else
      {
        dims[1]=p0->Dim(0);
        dimension dim((DLong *)dims, 2);
        res=new DDoubleGDL(dim, BaseGDL::NOZERO);
      }

      SizeT nEl=p0->N_Elements();
      for ( SizeT i=0; i<nEl; ++i )
      {
        idata.lam=(*lon)[i]*DEG_TO_RAD;
        idata.phi=(*lat)[i]*DEG_TO_RAD;
        odata=PJ_FWD(idata, ref);
        (*res)[2*i]=odata.x;
        (*res)[2*i+1]=odata.y;
      }
      return res;
    }
    e->Throw("More than 2 parameters not handled.");
    return NULL;
#else
    e->Throw("GDL was compiled without support for map projections");
    return NULL;
#endif
  }

  BaseGDL* map_proj_inverse_fun(EnvT* e)
  {
#ifdef USE_LIBPROJ4
    // xy -> lonlat
    SizeT nParam=e->NParam();
    if ( nParam<1 )
      e->Throw("Incorrect number of arguments.");

    XYTYPE idata;
    LPTYPE odata;

    ref=map_init();
    if ( ref==NULL )
    {
      e->Throw("Projection initialization failed.");
    }

    BaseGDL* p0;
    BaseGDL* p1;

    DDoubleGDL* x;
    DDoubleGDL* y;
    DDoubleGDL* xy;
    DDoubleGDL* res;
    DLong dims[2];

    if ( nParam==1 )
    {
      p0=e->GetParDefined(0);
      DDoubleGDL* xy=static_cast<DDoubleGDL*>
      (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));

      dims[0]=2;
      if ( p0->Rank()==1 )
      {
        dimension dim((DLong *)dims, 1);
        res=new DDoubleGDL(dim, BaseGDL::NOZERO);
      }
      else
      {
        dims[1]=p0->Dim(1);
        dimension dim((DLong *)dims, 2);
        res=new DDoubleGDL(dim, BaseGDL::NOZERO);
      }

      SizeT nEl=p0->N_Elements();
      for ( SizeT i=0; i<nEl/2; ++i )
      {
        idata.x=(*xy)[2*i];
        idata.y=(*xy)[2*i+1];
        odata=PJ_INV(idata, ref);
        (*res)[2*i]=odata.lam * RAD_TO_DEG;
        (*res)[2*i+1]=odata.phi * RAD_TO_DEG;
      }
      return res;

    }
    else if ( nParam==2 )
    {
      p0=e->GetParDefined(0);
      p1=e->GetParDefined(1);
      DDoubleGDL* x=static_cast<DDoubleGDL*>
      (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      DDoubleGDL* y=static_cast<DDoubleGDL*>
      (p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));

      dims[0]=2;
      if ( p0->Rank()==0||p0->Rank()==1 )
      {
        dimension dim((DLong *)dims, 1);
        res=new DDoubleGDL(dim, BaseGDL::NOZERO);
      }
      else
      {
        dims[1]=p0->Dim(0);
        dimension dim((DLong *)dims, 2);
        res=new DDoubleGDL(dim, BaseGDL::NOZERO);
      }

      SizeT nEl=p0->N_Elements();
      for ( SizeT i=0; i<nEl; ++i )
      {
        idata.x=(*x)[i];
        idata.y=(*y)[i];
        odata=PJ_INV(idata, ref);
        (*res)[2*i]=odata.lam * RAD_TO_DEG;
        (*res)[2*i+1]=odata.phi * RAD_TO_DEG;
      }
      return res;
    }
    e->Throw("More than 2 parameters not handled.");
    return NULL;
#else
    e->Throw("GDL was compiled without support for map projections");
    return NULL;
#endif
  }

} // namespace

