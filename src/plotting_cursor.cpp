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

namespace lib{

using namespace std;

void empty(EnvT* e)
{
  GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
  if (actDevice->Name() == "X")
  {
    GDLGStream *actStream = actDevice->GetStream();
    if (actStream != NULL) actStream->Flush();
  }
}

void tvcrs( EnvT* e)
{
 
  GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
   if (actDevice == NULL) e->Throw("No device available"); 
  GDLGStream *actStream = actDevice->GetStream();
  if (actStream == NULL) e->Throw("Unable to create window.");
  if (!actStream->HasCrossHair())
  {
    e->Throw("Routine is not defined for current graphics device.");
  }

  SizeT nParam = e->NParam(1);

  if (nParam < 2 )
  {
    e->Throw("TVCRS with 1 argument not implemented (fixme)");
  }
  DDoubleGDL *x,*y;

  x = e->GetParAs< DDoubleGDL > (0);
  y = e->GetParAs< DDoubleGDL > (1);

  PLINT plplot_level;
  actStream->glevel(plplot_level);
  // when level < 2, we have to read if ![x|y].crange exist
  // if not, we have to build a [0,1]/[0,1] window
  if (plplot_level < 2)
  {
    actStream->NextPlot();

    actStream->vpor(0, 1, 0, 1);
    actStream->wind(0, 1, 0, 1);

  }

  PLFLT ix,iy;

  if (e->KeywordSet("DATA")) // /DATA
  {
    DDouble tempx,tempy;
    tempx=(*x)[0];
    tempy=(*y)[0];
    bool mapSet = false;
    get_mapset(mapSet);
    if (mapSet)
    {
#ifdef USE_LIBPROJ4
      if (ref == NULL) e->Throw("Projection initialization failed.");
      LPTYPE idataN;
      idataN.u = tempx* DEG_TO_RAD;
      idataN.v = tempy* DEG_TO_RAD;
      XYTYPE odata = PJ_FWD(idataN, ref);
      // norm to world invalid since projection. use !x.s and !y.s directly
      DDouble *sx, *sy;
      GetSFromPlotStructs( &sx, &sy );
      tempx= sx[0] +odata.u * sx[1];
      tempy= sy[0] +odata.v * sy[1]; //normed values
      actStream->NormedDeviceToDevice(tempx,tempy,ix,iy);
      DLong iix=ix;
      DLong iiy=iy;
      actStream->WarpPointer(iix,iiy);
      actStream->Flush();
      actStream->UnsetFocus();
      return;
#endif
    }
     bool xLog, yLog;
     gdlGetAxisType("X", xLog);
     gdlGetAxisType("Y", yLog);
     if(xLog) tempx=pow(10,tempx);
     if(yLog) tempy=pow(10,tempy);
    actStream->WorldToDevice(tempx,tempy,ix,iy);
  }
  else if (e->KeywordSet("NORMAL"))
  {
    actStream->NormedDeviceToDevice((*x)[0],(*y)[0],ix,iy);
  }
  else // (e->KeywordSet("DEVICE"))
  {
    ix=(*x)[0];
    iy=(*y)[0];
  }
  DLong iix=ix;
  DLong iiy=iy;
  actStream->WarpPointer(iix,iiy);
  actStream->Flush();
  actStream->UnsetFocus();

}

// get cursor from plPlot     AC February 2008
// fully compatible with IDL using our own cursor routines GD Jan 2013
void cursor(EnvT* e){
  enum CursorOpt {
    NOWAIT=0, 
    WAIT, //1
    CHANGE, //2
    DOWN, //3
    UP //4
  };
  GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
   if (actDevice == NULL) e->Throw("No device available"); 
  GDLGStream *actStream = actDevice->GetStream();
  if (actStream == NULL) e->Throw("Unable to create window.");
  if (!actStream->HasCrossHair())
  {
    e->Throw("Routine is not defined for current graphics device.");
  }

  SizeT nParam = e->NParam(1);

  if (nParam < 2 || nParam > 3)
  {
    e->Throw("Incorrect number of arguments.");
  }

  e->AssureGlobalPar(0);
  e->AssureGlobalPar(1);

  static PLGraphicsIn gin;

  PLINT plplot_level;
  actStream->glevel(plplot_level);
  // when level < 2, we have to read if ![x|y].crange exist
  // if not, we have to build a [0,1]/[0,1] window
  if (plplot_level < 2)
  {
    actStream->NextPlot();

    actStream->vpor(0, 1, 0, 1);
    actStream->wind(0, 1, 0, 1);

  }
  // mimic idl logic:
  DLong wait = WAIT;
  if (nParam == 3)
  {
    e->AssureLongScalarPar(2, wait);
  }
  if (e->KeywordSet("NOWAIT")) wait=NOWAIT;
  if (e->KeywordSet("CHANGE")) wait=CHANGE;
  if (e->KeywordSet("WAIT")) wait=WAIT;
  if (e->KeywordSet("DOWN")) wait=DOWN;
  if (e->KeywordSet("UP")) wait=UP;
  if(actStream->GetGin(&gin, wait)==false) return;
  // outside window report -1 -1 at least for DEVICE values
  if (gin.pX < 0 || gin.pX > actStream->xPageSize() || gin.pY < 0 || gin.pY > actStream->yPageSize())
  {
    gin.pX = -1;
    gin.pY = -1;
  }
  if (e->KeywordSet("DEVICE"))
  {
    DLongGDL* xLong;
    DLongGDL* yLong;
    xLong = new DLongGDL(gin.pX);
    yLong = new DLongGDL(gin.pY);

    e->SetPar(0, xLong);
    e->SetPar(1, yLong);
  }
  else
  {
    DDoubleGDL* x;
    DDoubleGDL* y;
    if (e->KeywordSet("NORMAL"))
    {
      x = new DDoubleGDL(gin.dX);
      y = new DDoubleGDL(gin.dY);
    }
    else
    { // default (/data)
      DDouble tempx,tempy;
#ifdef USE_LIBPROJ4
      bool mapSet = false;
      get_mapset(mapSet);
      if (!mapSet)
      {
#endif
        actStream->NormToWorld((DDouble)gin.dX, (DDouble)gin.dY, tempx, tempy);
#ifdef USE_LIBPROJ4
      }
      else
      {
        ref = map_init();
        if (ref == NULL) e->Throw("Projection initialization failed.");
        XYTYPE idata, idataN;
        idataN.u = gin.dX;
        idataN.v = gin.dY;
        DDouble *sx, *sy;
        // norm to world invalid since projection. use !x.s and !y.s directly
        // was: actStream->NormToWorld(idataN.u, idataN.v, idata.u, idata.v);
        GetSFromPlotStructs( &sx, &sy );
        idata.u = (idataN.u - sx[0])/sx[1];
        idata.v = (idataN.v - sy[0])/sy[1];
        LPTYPE odata = PJ_INV(idata, ref);
        tempx = odata.u * RAD_TO_DEG;
        tempy = odata.v * RAD_TO_DEG;
      }
#endif
      bool xLog, yLog;
      gdlGetAxisType("X", xLog);
      gdlGetAxisType("Y", yLog);
      if(xLog) tempx=pow(10,tempx);
      if(yLog) tempy=pow(10,tempy);
      x = new DDoubleGDL(tempx);
      y = new DDoubleGDL(tempy);
    }
    e->SetPar(0, x);
    e->SetPar(1, y);
  }

  // we update the !Mouse structure (4 fields, only 3 managed up to now)
  // found on the web:
  //"Information about which mouse button has been used (if) any is stored in the !err variable. A value of 1 corresponds to the left, 2 to middle and 4 to the right button."
  //!err is obsolete but still working:
  DStructGDL* Struct = SysVar::Mouse();
  if (Struct != NULL)
  {
    static unsigned xMouseTag = Struct->Desc()->TagIndex("X");
    (*static_cast<DLongGDL*>(Struct->GetTag(xMouseTag)))[0] = gin.pX;
    static unsigned yMouseTag = Struct->Desc()->TagIndex("Y");
    (*static_cast<DLongGDL*>(Struct->GetTag(yMouseTag)))[0] = gin.pY;
    static unsigned ButtonMouseTag = Struct->Desc()->TagIndex("BUTTON");
    if (gin.button == 5) gin.button = 16; // today we have 5 buttons mouses!
    if (gin.button == 4) gin.button = 8; // but we keep the previous logic:
    if (gin.button == 3) gin.button = 4; // in powers of 2 (0,1,2,4..)
    (*static_cast<DLongGDL*>(Struct->GetTag(ButtonMouseTag)))[0] = gin.button;
  }
  DVar *err=FindInVarList(sysVarList, "ERR");
  (static_cast<DLongGDL*>(err->Data()))[0]=  gin.button;
}

} // namespace
