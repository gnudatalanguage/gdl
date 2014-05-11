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
#define PLK_Escape            0x1B

namespace lib{

using namespace std;

void empty(EnvT* e)
{
  GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
  if (actDevice->Name() == "X")
  {
    GDLGStream *plg = actDevice->GetStream();
    if (plg != NULL) plg->Flush();
  }
}

void tvcrs( EnvT* e)
{
  GraphicsDevice* actDevice = GraphicsDevice::GetDevice();

  if (actDevice->Name() != "X")
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

  GDLGStream *plg = actDevice->GetStream();
  if (plg == NULL) e->Throw("Unable to create window.");
  PLINT plplot_level;
  plg->glevel(plplot_level);
  // when level < 2, we have to read if ![x|y].crange exist
  // if not, we have to build a [0,1]/[0,1] window
  if (plplot_level < 2)
  {
    plg->NextPlot();

    plg->vpor(0, 1, 0, 1);
    plg->wind(0, 1, 0, 1);

  }

  PLFLT ix,iy;

  if (e->KeywordSet("DATA")) // /DATA
  {
    DDouble tempx,tempy;
    tempx=(*x)[0];
    tempy=(*y)[0];
#ifdef USE_LIBPROJ4
    bool mapSet = false;
    get_mapset(mapSet);
    if (mapSet)
    {
//      PROJTYPE* ref = map_init();
      if (ref == NULL) e->Throw("Projection initialization failed.");
      LPTYPE idataN;
#ifdef USE_LIBPROJ4_NEW
      idataN.u = tempx* RAD_TO_DEG;
      idataN.v = tempy* RAD_TO_DEG;
      XYTYPE odata = PJ_FWD(idataN, ref);
      tempx = odata.u;
      tempy = odata.v;
#else
      idataN.lam = tempx* RAD_TO_DEG;
      idataN.phi = tempy* RAD_TO_DEG;
      XYTYPE odata = PJ_FWD(idataN, ref);
      tempx = odata.x;
      tempy = odata.y;
#endif	  
    }
#endif
    bool xLog, yLog;
    gdlGetAxisType("X", xLog);
    gdlGetAxisType("Y", yLog);
    if(xLog) tempx=pow(10,tempx);
    if(yLog) tempy=pow(10,tempy);
    plg->WorldToDevice(tempx,tempy,ix,iy);
  }
  else if (e->KeywordSet("NORMAL"))
  {
    plg->NormedDeviceToDevice((*x)[0],(*y)[0],ix,iy);
  }
  else // (e->KeywordSet("DEVICE"))
  {
    ix=(*x)[0];
    iy=(*y)[0];
  }
  plg->WarpPointer(ix,iy);
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

  if (actDevice->Name() != "X")
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

  GDLGStream *plg = actDevice->GetStream();
  if (plg == NULL) e->Throw("Unable to create window.");

  static PLGraphicsIn gin;

  PLINT plplot_level;
  plg->glevel(plplot_level);
  // when level < 2, we have to read if ![x|y].crange exist
  // if not, we have to build a [0,1]/[0,1] window
  if (plplot_level < 2)
  {
    plg->NextPlot();

    plg->vpor(0, 1, 0, 1);
    plg->wind(0, 1, 0, 1);

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
  PLFLT xp, yp;
  PLINT xleng, yleng, xoff, yoff;
  plg->gpage(xp, yp, xleng, yleng, xoff, yoff);
  if(plg->GetGin(&gin, wait)==false) return;
  // outside window report -1 -1 at least for DEVICE values
  if (gin.pX < 0 || gin.pX > plg->xPageSize() || gin.pY < 0 || gin.pY > plg->yPageSize())
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
        plg->NormToWorld((DDouble)gin.dX, (DDouble)gin.dY, tempx, tempy);
#ifdef USE_LIBPROJ4
      }
      else
      {
#ifdef USE_LIBPROJ4_NEW
      static PROJTYPE ref;
#else
      static PROJTYPE* ref;
#endif
        ref = map_init();
        if (ref == NULL) e->Throw("Projection initialization failed.");
        XYTYPE idata, idataN;
#ifdef USE_LIBPROJ4_NEW
        idataN.u = gin.dX;
        idataN.v = gin.dY;
        plg->NormToWorld(idataN.u, idataN.v, idata.u, idata.v);
        LPTYPE odata = PJ_INV(idata, ref);
        tempx = odata.u * RAD_TO_DEG;
        tempy = odata.v * RAD_TO_DEG;
#else
        idataN.x = gin.dX;
        idataN.y = gin.dY;
        plg->NormToWorld(idataN.x, idataN.y, idata.x, idata.y);
        LPTYPE odata = PJ_INV(idata, ref);
        tempx = odata.lam * RAD_TO_DEG;
        tempy = odata.phi * RAD_TO_DEG;
#endif
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
