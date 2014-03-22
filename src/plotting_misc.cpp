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

namespace lib
{

  using namespace std;

  void set_plot(EnvT* e) // modifies !D system variable
  {
    SizeT nParam=e->NParam();
    if ( nParam<1 )
      e->Throw("Incorrect number of arguments.");
    DString device;
    e->AssureScalarPar<DStringGDL>(0, device);

    // this is the device name
    device=StrUpCase(device);

    bool success=GraphicsDevice::SetDevice(device);
    if ( !success ) {
      GraphicsDevice::ListDevice();
#ifndef _MSC_VER
      if (device=="X") {
	cout << "This system seems to be a X11 capable one where GDL was compiled without X11 lib." << endl;
      }
#endif
      e->Throw("Device not supported/unknown: "+device);
    }

    if ( device=="PS" ||device=="SVG" )
    {
      static DStructGDL* pStruct=SysVar::P();
      // SA: this does not comply with IDL behaviour, see testsuite/test_pmulti.pro
      //static unsigned noEraseTag = pStruct->Desc()->TagIndex( "NOERASE");
      //(*static_cast<DLongGDL*>( pStruct->GetTag( noEraseTag, 0)))[0] = 1;
        static unsigned colorTag=pStruct->Desc()->TagIndex("COLOR");
        (*static_cast<DLongGDL*>(pStruct->GetTag(colorTag, 0)))[0]=0; //PLEASE DO NOT CHANGE values here until a better color
                                                                        // handling has been found.
         static unsigned bckTag=pStruct->Desc()->TagIndex("BACKGROUND");
        (*static_cast<DLongGDL*>(pStruct->GetTag(bckTag, 0)))[0]=255; //PLEASE DO NOT CHANGE values here. This default is OK.
    }
    else
    {
      static DStructGDL* pStruct=SysVar::P();
      static unsigned colorTag=pStruct->Desc()->TagIndex("COLOR");
      (*static_cast<DLongGDL*>(pStruct->GetTag(colorTag, 0)))[0]=16777215;
        static unsigned bckTag=pStruct->Desc()->TagIndex("BACKGROUND");
      (*static_cast<DLongGDL*>(pStruct->GetTag(bckTag, 0)))[0]=0;
    }
  }

  void tvlct(EnvT* e)
  {
    int nParam=e->NParam(1); //, "TVLCT");

    static int getKW=e->KeywordIx("GET");
    bool get=e->KeywordSet(getKW);
    static int hlsKW=e->KeywordIx("HLS");
    bool hls=e->KeywordSet(hlsKW);
    static int hsvKW=e->KeywordIx("HSV");
    bool hsv=e->KeywordSet(hsvKW);

    DLong start=0;
    GDLCT* actCT=GraphicsDevice::GetCT();
    if ( nParam<=2 ) // TVLCT,I[,Start]
    {
      if ( nParam==2 )
        e->AssureLongScalarPar(1, start);
      if ( start<0 ) start=0;

      if ( get ) // always RGB
      {
        BaseGDL*& p0=e->GetParGlobal(0);

        SizeT nCol=ctSize-start;

        DByteGDL* rgb=new DByteGDL(dimension(nCol, 3), BaseGDL::NOZERO);

        for ( SizeT i=start, ii=0; i<ctSize; ++i, ++ii )
          actCT->Get(i, (*rgb)[ ii], (*rgb)[ ii+nCol], (*rgb)[ ii+2*nCol]);

        GDLDelete(p0);
        p0=rgb;
      }
      else
      {
        if ( hls||hsv )
        {
          DFloatGDL* rgb=e->GetParAs< DFloatGDL>(0);
          if ( rgb->Dim(1)!=3 )
            e->Throw("Array must have dimensions of (1, 3): "+
                     e->GetParString(0));

          SizeT nCol=rgb->Dim(0);
          SizeT setCol=nCol+start;
          if ( setCol>ctSize ) setCol=ctSize;

          if ( hls )
            for ( SizeT i=start, ii=0; i<setCol; ++i, ++ii )
              actCT->SetHLS(i,
                            (*rgb)[ ii],
                            (*rgb)[ ii+nCol],
                            (*rgb)[ ii+2*nCol]);
          else
            for ( SizeT i=start, ii=0; i<setCol; ++i, ++ii )
              actCT->SetHSV(i,
                            (*rgb)[ ii],
                            (*rgb)[ ii+nCol],
                            (*rgb)[ ii+2*nCol]);
        }
        else
        {
          DByteGDL* rgb=e->GetParAs< DByteGDL>(0);
          if ( rgb->Dim(1)!=3 )
            e->Throw("Array must have dimensions of (1, 3): "+
                     e->GetParString(0));

          SizeT nCol=rgb->Dim(0);
          SizeT setCol=nCol+start;
          if ( setCol>ctSize ) setCol=ctSize;

          for ( SizeT i=start, ii=0; i<setCol; ++i, ++ii )
            actCT->Set(i,
                       (*rgb)[ ii],
                       (*rgb)[ ii+nCol],
                       (*rgb)[ ii+2*nCol]);
        }
      }
    }
    else // TVLCT,I1,I2,I3[,Start]
    {
      if ( nParam==4 )
        e->AssureLongScalarPar(3, start);
      if ( start<0 ) start=0;

      if ( get ) // always RGB
      {
        BaseGDL*& p0=e->GetParGlobal(0);
        BaseGDL*& p1=e->GetParGlobal(1);
        BaseGDL*& p2=e->GetParGlobal(2);

        SizeT nCol=ctSize-start;

        DByteGDL* r=new DByteGDL(dimension(nCol), BaseGDL::NOZERO);
        DByteGDL* g=new DByteGDL(dimension(nCol), BaseGDL::NOZERO);
        DByteGDL* b=new DByteGDL(dimension(nCol), BaseGDL::NOZERO);

        for ( SizeT i=start, ii=0; i<ctSize; ++i, ++ii )
          actCT->Get(i, (*r)[ ii], (*g)[ ii], (*b)[ ii]);

        GDLDelete(p0);
        p0=r;
        GDLDelete(p1);
        p1=g;
        GDLDelete(p2);
        p2=b;
      }
      else
      {
        if ( hls||hsv )
        {
          DFloatGDL* r=e->GetParAs< DFloatGDL>(0);
          DFloatGDL* g=e->GetParAs< DFloatGDL>(1);
          DFloatGDL* b=e->GetParAs< DFloatGDL>(2);
          SizeT rCol=r->N_Elements();
          SizeT gCol=g->N_Elements();
          SizeT bCol=b->N_Elements();
          SizeT nCol=rCol;
          if ( gCol<nCol ) nCol=gCol;
          if ( bCol<nCol ) nCol=bCol;

          SizeT setCol=nCol+start;
          if ( setCol>ctSize ) setCol=ctSize;

          if ( hls )
            for ( SizeT i=start, ii=0; i<setCol; ++i, ++ii )
              actCT->SetHLS(i, (*r)[ ii], (*g)[ ii], (*b)[ ii]);
          else
            for ( SizeT i=start, ii=0; i<setCol; ++i, ++ii )
              actCT->SetHSV(i, (*r)[ ii], (*g)[ ii], (*b)[ ii]);
        }
        else
        {
          DByteGDL* r=e->GetParAs< DByteGDL>(0);
          DByteGDL* g=e->GetParAs< DByteGDL>(1);
          DByteGDL* b=e->GetParAs< DByteGDL>(2);
          SizeT rCol=r->N_Elements();
          SizeT gCol=g->N_Elements();
          SizeT bCol=b->N_Elements();
          SizeT nCol=rCol;
          if ( gCol<nCol ) nCol=gCol;
          if ( bCol<nCol ) nCol=bCol;

          SizeT setCol=nCol+start;
          if ( setCol>ctSize ) setCol=ctSize;

          for ( SizeT i=start, ii=0; i<setCol; ++i, ++ii )
            actCT->Set(i, (*r)[ ii], (*g)[ ii], (*b)[ ii]);
        }
      }
    }

    // AC, 07/02/2012, please report any unexpected side effect (see test_tvlct.pro)
    if ( !get )
    {
      GDLGStream* actStream=GraphicsDevice::GetDevice()->GetStream(false);
      if ( actStream!=NULL )
      {
        PLINT red[ctSize], green[ctSize], blue[ctSize];
        actCT->Get(red, green, blue);
        actStream->scmap1(red, green, blue, ctSize);
      }
    }
  }

} // namespace
