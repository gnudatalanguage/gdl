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

  class plots_call: public plotting_routine_call
  {

    DDoubleGDL *xVal, *yVal, *zVal;
    Guard<BaseGDL> xval_guard, yval_guard;
    DLong psym;
    bool xLog, yLog, zLog;
    SizeT xEl, yEl, zEl;
    bool append;
    bool doClip;
    bool restoreClipBox;
    PLFLT savebox[4];

  private:

    bool handle_args(EnvT* e) // {{{
    {
      append=e->KeywordSet("CONTINUE");
      if ( nParam()==1 )
      {
        BaseGDL* p0;
        p0=e->GetParDefined(0);
        if ( p0->Dim(0)!=2 )
          e->Throw("When only 1 param, dims must be (2,n)");

        if ( e->KeywordSet("T3D") ) e->Throw("Only 3-argument case supported for T3D - FIXME!");
        // TODO: the (3,n) case

        DDoubleGDL *val=e->GetParAs< DDoubleGDL>(0);

        xEl=p0->N_Elements()/p0->Dim(0);
        xVal=new DDoubleGDL(dimension(xEl), BaseGDL::NOZERO);
        xval_guard.Reset(xVal); // delete upon exit

        yEl=p0->N_Elements()/p0->Dim(0);
        yVal=new DDoubleGDL(dimension(yEl), BaseGDL::NOZERO);
        yval_guard.Reset(yVal); // delete upon exit

        for ( SizeT i=0; i<xEl; i++ )
        {
          (*xVal)[i]=(*val)[2*i];
          (*yVal)[i]=(*val)[2*i+1];
        }
      }
      else if ( nParam()==2 )
      {
        if ( e->KeywordSet("T3D") ) e->Throw("Only 3-argument case supported for T3D - FIXME!"); // TODO

        xVal=e->GetParAs< DDoubleGDL>(0);
        xEl=xVal->N_Elements();

        yVal=e->GetParAs< DDoubleGDL>(1);
        yEl=yVal->N_Elements();
        //silently drop unmatched values
        if ( yEl!=xEl )
        {
          SizeT size;
          size=min(xEl, yEl);
          xEl=size;
          yEl=size;
        }
      }
      else if ( nParam()==3 )
      {
        zVal=e->GetParAs< DDoubleGDL>(2);
        zEl=zVal->N_Elements();

        //if ((*zVal)[0] == 0 && zEl == 1) {
        xVal=e->GetParAs< DDoubleGDL>(0);
        xEl=xVal->N_Elements();

        yVal=e->GetParAs< DDoubleGDL>(1);
        yEl=yVal->N_Elements();
        if ( !(xEl==yEl&&yEl==zEl) )
        {
          SizeT size;
          size=min(xEl, yEl);
          size=min(size, zEl);
          xEl=size;
          yEl=size;
          zEl=size;
        }
      }
      return false;
    }

  private:

    void old_body(EnvT* e, GDLGStream* actStream)
    {
      int clippingix=e->KeywordIx("CLIP");
      DFloatGDL* clipBox=NULL;

      enum
      {
        DATA=0,
        NORMAL,
        DEVICE
      } coordinateSystem=DATA;
      //check presence of DATA,DEVICE and NORMAL options
      if ( e->KeywordSet("DATA") ) coordinateSystem=DATA;
      if ( e->KeywordSet("DEVICE") ) coordinateSystem=DEVICE;
      if ( e->KeywordSet("NORMAL") ) coordinateSystem=NORMAL;
      // get_axis_type
      gdlGetAxisType("X", xLog);
      gdlGetAxisType("Y", yLog);
      gdlGetAxisType("Z", zLog);

      bool mapSet=false;
#ifdef USE_LIBPROJ4
      // Map Stuff (xtype = 3)

      get_mapset(mapSet);

      if ( mapSet )
      {
        ref=map_init();
        if ( ref==NULL )
        {
          e->Throw("Projection initialization failed.");
        }
      }
#endif
      restoreClipBox=false;
      int noclipvalue=1;
      e->AssureLongScalarKWIfPresent( "NOCLIP", noclipvalue);
      doClip=(noclipvalue==0); //PLOTS by default does not clip, even if clip is defined by CLIP= or !P.CLIP
      clipBox=e->IfDefGetKWAs<DFloatGDL>(clippingix);
      if (doClip && clipBox!=NULL && clipBox->N_Elements()>=4 ) //clipbox exist, will be used: convert to device coords
                                   //and save in !P.CLIP...
      {
        restoreClipBox=true; //restore later
        // save current !P.CLIP box, replace by our current clipbox in whatever coordinates, will
        // give back the !P.CLIP box at end...
        static DStructGDL* pStruct=SysVar::P();
        static unsigned clipTag=pStruct->Desc()->TagIndex("CLIP"); //must be in device coordinates
        static PLFLT tempbox[4];
        for ( int i=0; i<4; ++i ) savebox[i]=(*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[i];
        if ( coordinateSystem==DEVICE )
        {
          for ( int i=0; i<4; ++i ) tempbox[i]=(*clipBox)[i];
        }
        else if ( coordinateSystem==DATA )
        {
          //handle log: if existing box is already in log, use log of clipbox values.
          PLFLT worldbox[4];
          for ( int i=0; i<4; ++i ) worldbox[i]=(*clipBox)[i];
          if (xLog) {worldbox[0]=log10(worldbox[0]); worldbox[2]=log10(worldbox[2]);}
          if (yLog) {worldbox[1]=log10(worldbox[1]); worldbox[3]=log10(worldbox[3]);}
          bool okClipBox=true;
          for ( int i=0; i<4; ++i )
          {
            if (!(worldbox[i]==worldbox[i])) //NaN
            {
              okClipBox=false;restoreClipBox=false;doClip=false;
            }
          }
          if (okClipBox)
          {
            actStream->WorldToDevice(worldbox[0], worldbox[1], tempbox[0], tempbox[1]);
            actStream->WorldToDevice(worldbox[2], worldbox[3], tempbox[2], tempbox[3]);
          }
        }
        else
        {
          actStream->NormedDeviceToDevice((*clipBox)[0],(*clipBox)[1], tempbox[0], tempbox[1]);
          actStream->NormedDeviceToDevice((*clipBox)[2],(*clipBox)[3], tempbox[2], tempbox[3]);
        }
        //place in !P.CLIP
        for ( int i=0; i<4; ++i ) (*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[i]=tempbox[i];
      }

      PLFLT wun, wdeux, wtrois, wquatre;
      if ( coordinateSystem==DATA) //with PLOTS, we can plot *outside* the box(e)s in DATA coordinates.
                                   // convert to device coords in this case
      {
        actStream->pageWorldCoordinates(wun, wdeux, wtrois, wquatre);
      }

      actStream->OnePageSaveLayout(); // one page

      if ( !e->KeywordSet("T3D") )
      {
        actStream->vpor(0, 1, 0, 1);
        if ( coordinateSystem==DEVICE )
        {
          actStream->wind(0.0, actStream->xPageSize(), 0.0, actStream->yPageSize());
          xLog=false;
          yLog=false;
        }
        else if ( coordinateSystem==NORMAL )
        {
          actStream->wind(0, 1, 0, 1);
          xLog=false;
          yLog=false;
        }
        else //with PLOTS, we can plot *outside* the box(e)s in DATA coordinates.
        {
          actStream->wind(wun, wdeux, wtrois, wquatre);
        }
      }
    }

  private:

    void call_plplot(EnvT* e, GDLGStream* actStream) 
    {
      // start drawing. Graphic Keywords accepted: CLIP(YES), COLOR(OK), DATA(YES), DEVICE(YES),
      //LINESTYLE(OK), NOCLIP(YES), NORMAL(YES), PSYM(OK), SYMSIZE(OK), T3D(NO), THICK(OK), Z(NO)
      if ( e->KeywordSet("T3D") ) // TODO: check !P.T3D
      {
        Warning("PLOTS: 3D plotting does not really work yet (!P.T and !P.T3D are ignored for the moment)");

        /*       actStream->w3d(1.2, 1.2, 2.2, // TODO!
                 xStart, xEnd, yStart, yEnd, xStart, zEnd,
                 30, 30  // TODO!
               );
         */

        gdlSetGraphicsForegroundColorFromKw(e, actStream); //COLOR
        gdlGetPsym(e, psym); //PSYM
        gdlSetLineStyle(e, actStream); //LINESTYLE
        gdlSetSymsize(e, actStream); //SYMSIZE
        gdlSetPenThickness(e, actStream); //THICK

        PLINT n=xVal->N_Elements();
        if ( yEl>1&&yEl<n ) n=yEl;
        if ( zEl>1&&zEl<n ) n=zEl;
        ArrayGuard<PLFLT> xGuard, yGuard, zGuard;
        PLFLT *x=new PLFLT[n];
        xGuard.Reset(x);
        {
          int a=(xEl!=1);
          for ( PLINT i=0; i<n; ++i ) x[i]=(xLog)?(*xVal)[a*i]:log10((*xVal)[a*i]);
        }

        PLFLT *y=new PLFLT[n];
        yGuard.Reset(y);
        {
          int a=(yEl!=1);
          for ( PLINT i=0; i<n; ++i ) y[i]=(yLog)?(*yVal)[a*i]:log10((*yVal)[a*i]);
        }

        PLFLT *z=new PLFLT[n];
        zGuard.Reset(z);
        {
          int a=(zEl!=1);
          for ( PLINT i=0; i<n; ++i ) z[i]=(zLog)?(*zVal)[a*i]:log10((*zVal)[a*i]);
        }

        actStream->line3(n, x, y, z);
      }
      else
      {
        // make all clipping computations BEFORE setting graphic properties (color, size)
        bool stopClip=false;
        if ( doClip )  if ( startClipping(e, actStream, true)==TRUE ) stopClip=true;
        //properties
        gdlSetGraphicsForegroundColorFromKw(e, actStream); //COLOR
        gdlGetPsym(e, psym); //PSYM
        gdlSetLineStyle(e, actStream); //LINESTYLE
        gdlSetSymsize(e, actStream); //SYMSIZE
        gdlSetPenThickness(e, actStream); //THICK

        bool valid=draw_polyline(e, actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, append);
        // TODO: handle valid?
        if (stopClip) stopClipping(actStream);
      }
    }

  private:

    virtual void post_call(EnvT*, GDLGStream* actStream)
    {
      actStream->RestoreLayout();
      actStream->lsty(1); //reset linestyle
      if (restoreClipBox)
      {
        static DStructGDL* pStruct=SysVar::P();
        static unsigned clipTag=pStruct->Desc()->TagIndex("CLIP"); //must be in device coordinates
        for ( int i=0; i<4; ++i ) (*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[i]=savebox[i];
      }
    }

  }; 

  void plots(EnvT* e)
  {
    plots_call plots;
    plots.call(e, 1);
  }

} // namespace
