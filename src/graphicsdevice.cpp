/**************************************************************************
   graphicsdevice.cpp  -  GDL base class for all graphic devices
   renamed from: graphics.cpp                          
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

#include "includefirst.hpp"

#include <cstdlib>

#include "objects.hpp"
#include "graphicsdevice.hpp"

#include "devicewin.hpp"
#include "devicewx.hpp"
#include "devicex.hpp"
#include "deviceps.hpp"
#include "devicesvg.hpp"
#include "devicez.hpp"
#include "devicenull.hpp"
#include "initsysvar.hpp"
#include "color.hpp"

using namespace std;

bool GDLCT::Get( PLINT r_[], PLINT g_[], PLINT b_[], UInt nCol) const
{
  if( nCol > ctSize)
    return false;

  for(UInt i=0; i<nCol; ++i)
    {
      r_[i] = static_cast<PLINT>(r[i]);
      g_[i] = static_cast<PLINT>(g[i]);
      b_[i] = static_cast<PLINT>(b[i]);
    }

  return true;
}

bool GDLCT::Get( UInt ix, DByte& r_, DByte& g_, DByte& b_) const
{
  if( ix >= ctSize)
    return false;
  r_ = r[ix];
  g_ = g[ix];
  b_ = b[ix];
  return true;
}

bool GDLCT::Set( UInt ix, DByte r_, DByte g_, DByte b_)
{
  if( ix >= ctSize)
    return false;
  r[ix] = r_;
  g[ix] = g_;
  b[ix] = b_;
  return true;
}

bool GDLCT::SetHLS( UInt ix, DFloat h, DFloat l, DFloat s)
{
  if( ix >= ctSize)
    return false;

  DByte r_, g_, b_;
  HLS2RGB( h, l, s, r_, g_, b_);

  r[ix] = r_;
  g[ix] = g_;
  b[ix] = b_;
  return true;
}

bool GDLCT::SetHSV( UInt ix, DFloat h, DFloat s, DFloat v)
{
  if( ix >= ctSize)
    return false;

  DByte r_, g_, b_;
  HSV2RGB( h, s, v, r_, g_, b_);

  r[ix] = r_;
  g[ix] = g_;
  b[ix] = b_;
  return true;
}

std::vector<GDLCT> GraphicsDevice::CT;    // predefined colortables
GDLCT              GraphicsDevice::actCT; // actual used colortable

DeviceListT  GraphicsDevice::deviceList;
GraphicsDevice*    GraphicsDevice::actDevice   = NULL;
GraphicsDevice*    GraphicsDevice::actGUIDevice   = NULL;

int GraphicsDevice::wTag;
int GraphicsDevice::xSTag;
int GraphicsDevice::ySTag;
int GraphicsDevice::xVSTag;
int GraphicsDevice::yVSTag;
int GraphicsDevice::n_colorsTag;
DByte  GraphicsDevice::deviceBckColorR;
DByte  GraphicsDevice::deviceBckColorG;
DByte  GraphicsDevice::deviceBckColorB;

GraphicsDevice::~GraphicsDevice() 
{
  // actDevice's dStruct is or will be deleted from sysVarList
  if( actDevice != this) delete dStruct;
} // v-table instatiation

GraphicsDevice::GraphicsDevice(): dStruct( NULL), CopyBufferSize(0)
{
}

void GraphicsDevice::ListDevice(std::ostream& oss)
{
  int size = deviceList.size();
  oss << "Available Graphics Devices: ";
  for( int i=0; i<size; i++) oss << deviceList[ i]->Name() << " ";
  oss << endl;
}

bool GraphicsDevice::ExistDevice( const string& device, int &index)
{
  index=-1;
  int size = deviceList.size();
  for( int i=0; i<size; i++)
    {
      if( deviceList[ i]->Name() == device)
	{
	  index=i;
	  return true;
	}
    }
  return false;
}

bool GraphicsDevice::SetDevice( const string& device)
{
  int size = deviceList.size();
  for( int i=0; i<size; i++)
    {
      if( deviceList[ i]->Name() == device)
	{
	  actDevice=deviceList[ i];
	  // update !D
	  SysVar::SetD( actDevice->DStruct());
	  
	  return true;
	}
    }
  return false;
}

DStructGDL* GraphicsDevice::GetDeviceStruct( const string& device)
{
  int size = deviceList.size();
  for( int i=0; i<size; i++)
    {
      if( deviceList[ i]->Name() == device)
	{
	  return deviceList[ i]->DStruct();
	}
    }
  return NULL;
}
void GraphicsDevice::Init()
{
  InitCT();

  DefineDStructDesc();

  // 4 devices types without surprise !
  deviceList.push_back( new DeviceNULL());
  deviceList.push_back( new DevicePS());
  deviceList.push_back( new DeviceSVG());
  deviceList.push_back( new DeviceZ());
// Normally the following is to be used but for now
// it will be commented out so that Travis tests pass.
#ifdef HAVE_LIBWXWIDGETS
	//GDLWidget::Init();        // initialize widget system.
#endif
  
  //if GDL_USE_WX, and has wxWidgets, the wxWidgets device becomes 'X' or 'WIN' depending on machine,
  // no ther device is defined.
  std::string useWX=StrUpCase(GetEnvString("GDL_USE_WX"));
  if (useWX == "YES" ) {
#ifdef HAVE_LIBWXWIDGETS
	GDLWidget::Init();  // Hide this here from the OSX/CLang travis tests.
	#ifdef NO_WIDGET_DRAW
		#ifdef HAVE_X
		  deviceList.push_back( new DeviceX());
		#endif
		#ifdef _WIN32
		  deviceList.push_back( new DeviceWIN());
		#endif
	#else
		#ifdef HAVE_X
			deviceList.push_back( new DeviceWX("X"));
		#endif
		#ifdef _WIN32
			deviceList.push_back( new DeviceWX("WIN"));
		#endif
	#endif  
#else
	#ifdef HAVE_X
		deviceList.push_back( new DeviceX());
	#endif
	#ifdef _WIN32
		deviceList.push_back( new DeviceWIN());
	#endif
#endif
  } else {
#ifdef HAVE_LIBWXWIDGETS

	#  ifdef NO_WIDGET_DRAW
	GDLWidget::Init();  // Hide this here from the OSX/CLang travis tests.
		  deviceList.push_back( new DeviceWX("MAC"));
	#  else
		  deviceList.push_back( new DeviceWX());
	#  endif
#endif
#ifdef HAVE_X
    deviceList.push_back( new DeviceX());
#endif
#ifdef _WIN32
    deviceList.push_back( new DeviceWIN());
#endif
  }				   // (useWX == "YES" )

  // we try to set X, WIN or WX as default 
  // (and NULL if X11 system (Linux, OSX, Sun) but without X11 at compilation)
#if defined(HAVE_X) // Check X11 first
  if( !SetDevice( "X")) 
#elif defined(_WIN32) // If Windows enable WinGCC driver 
    if( !SetDevice( "WIN")) 
#elif defined (HAVE_LIBWXWIDGETS) // Finally check WX
      if (!SetDevice("MAC"))
#else
	if( !SetDevice( "NULL")) 
#  endif
#  if !defined (HAVE_X) && !defined (HAVE_LIBWXWIDGETS) && !defined (_WIN32)
	  {
	  }
#  else
  {
    cerr << "Error initializing graphics." << endl;
    exit( EXIT_FAILURE);
  }
#  endif
// GDL (at least with device X and Wx) handle equally any types of screens,
// with an equivalent depth of 24 (tested and true). So there is no
// need to return any depth, color number etc that would be anything
// else than a truecolor (24 bits). So we force !P.color to be set 
// to the depth of a truecolor screen. This
// is what 'set_plot' does, but set_plot is not called by default.
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    unsigned colorTag=pStruct->Desc()->TagIndex("COLOR");
    unsigned bckTag=pStruct->Desc()->TagIndex("BACKGROUND");
    (*static_cast<DLongGDL*>(pStruct->GetTag(colorTag, 0)))[0]=16777215;
    (*static_cast<DLongGDL*>(pStruct->GetTag(bckTag, 0)))[0]=0;
#ifdef HAVE_LIBWXWIDGETS
  // some X error message suggested this call
#ifdef HAVE_X
  //  XInitThreads();
#endif
#endif
  int index=0;
  // setting the GUI dev. There is only ONE possibility: the wxWidgets.
  // depending on how we build GDL, wxWidgets may replace X and WIN entirely, or not.
  // If not, it will be called "MAC" (temporarily).
  // If yes (replaces either X or WIN) it will be called 'X' or 'WIN' accordingly.
  if (ExistDevice( "MAC", index)) { //wxWidgets present, in concurrence with others.
    actGUIDevice = deviceList[index];
  } else if (ExistDevice( "X", index)) {
#ifdef HAVE_LIBWXWIDGETS    
    actGUIDevice = deviceList[index]; //true existing device
#else
    actGUIDevice = deviceList[0];  //will be a fake GUI and procedures will complain.
#endif
  } else if (ExistDevice("WIN", index)) {
#ifdef HAVE_LIBWXWIDGETS  
    actGUIDevice = deviceList[index]; //true existing device
#else
    actGUIDevice = deviceList[0];  //will be a fake GUI and procedures will complain.
#endif
  } else {
    actGUIDevice = deviceList[0];  //will be a fake GUI and procedures will complain.
  }
}

void GraphicsDevice::DestroyDevices()
{
	
#ifdef HAVE_LIBWXWIDGETS
  GDLWidget::UnInit();    // un-initialize widget system
#endif
  PurgeContainer( deviceList);
  actDevice = NULL;
}

void GraphicsDevice::DefineDStructDesc()
{
  DStructDesc* dSysVarDesc = FindInStructList( structList, "!DEVICE");
  if( dSysVarDesc != NULL) return; 

  dSysVarDesc = new DStructDesc( "!DEVICE");

  SpDString aString;
  SpDLong   aLong;
  SpDLong   aLongArr2( dimension(2));
  SpDFloat  aFloat;

  dSysVarDesc->AddTag("NAME",       &aString); 
  dSysVarDesc->AddTag("X_SIZE",     &aLong); 
  dSysVarDesc->AddTag("Y_SIZE",     &aLong); 
  dSysVarDesc->AddTag("X_VSIZE",    &aLong); 
  dSysVarDesc->AddTag("Y_VSIZE",    &aLong); 
  dSysVarDesc->AddTag("X_CH_SIZE",  &aLong); 
  dSysVarDesc->AddTag("Y_CH_SIZE",  &aLong); 
  dSysVarDesc->AddTag("X_PX_CM",    &aFloat); 
  dSysVarDesc->AddTag("Y_PX_CM",    &aFloat); 
  dSysVarDesc->AddTag("N_COLORS",   &aLong); 
  dSysVarDesc->AddTag("TABLE_SIZE", &aLong); 
  dSysVarDesc->AddTag("FILL_DIST",  &aLong); 
  dSysVarDesc->AddTag("WINDOW",     &aLong); 
  dSysVarDesc->AddTag("UNIT",       &aLong); 
  dSysVarDesc->AddTag("FLAGS",      &aLong); 
  dSysVarDesc->AddTag("ORIGIN",     &aLongArr2); 
  dSysVarDesc->AddTag("ZOOM",       &aLongArr2); 
  
  structList.push_back( dSysVarDesc);

  // !D tag indices
  wTag   = dSysVarDesc->TagIndex( "WINDOW");
  xSTag  = dSysVarDesc->TagIndex( "X_SIZE");
  ySTag  = dSysVarDesc->TagIndex( "Y_SIZE");
  xVSTag = dSysVarDesc->TagIndex( "X_VSIZE");
  yVSTag = dSysVarDesc->TagIndex( "Y_VSIZE");
  n_colorsTag = dSysVarDesc->TagIndex( "N_COLORS");
}

void GraphicsDevice::HandleEvents()
{
  DeviceListT::iterator i;
  for( i=deviceList.begin(); i != deviceList.end(); ++i)
    {
      (*i)->EventHandler();
    }
}


void GraphicsDevice::LoadCT( UInt iCT)
{
  actCT = CT[iCT];
}


//---------------Multi-Windows Device-------------------------------------------
int GraphicsMultiDevice::actWin;
WindowListT GraphicsMultiDevice::winList;
std::vector<long> GraphicsMultiDevice::oList;
int GraphicsMultiDevice::oIx;

void GraphicsMultiDevice::Init()
{
  GraphicsDevice::Init();
  
  //populate the one and only list of windows.
  winList.reserve(max_win_reserve);
  winList.resize(max_win);
  for (int i = 0; i < max_win; i++) winList[i] = NULL;
  oList.reserve(max_win_reserve);
  oList.resize(max_win);
  for (int i = 0; i < max_win; i++) oList[i] = 0;
  
  actWin=-1;
  oIx=-1;
}

DByteGDL* GraphicsMultiDevice::WindowState() {
  int maxwin = MaxWin();
  if (maxwin > 0) {
    DByteGDL* ret = new DByteGDL(dimension(maxwin < 65 ? 65 : maxwin), BaseGDL::ZERO);
    for (int i = 0; i < maxwin; i++) (*ret)[i] = WState(i);
    return ret;
  } else return NULL;
}

bool GraphicsMultiDevice::WState(int wIx) {
  return wIx >= 0 && wIx < oList.size() && oList[wIx] != 0;
}

int GraphicsMultiDevice::MaxWin() {
  TidyWindowsList();
  return winList.size();
}

void GraphicsMultiDevice::SetActWin(int wIx) {
  // Special behaviour for !D.WINDOW etc:
  // Update window number in all equivalent !D: !D is local to the Device, and sysvar's !D point to it. 
  // So there are actually several !D per GraphicsMultiDevice!
  // we need to update *all* the different !D in action, which are 2 if widgets + X11 device, and ony one if widget + wX device.
  // another option would be to make !D static like !P.
  // At the moment only "sharing" !D.WINDOW seems useful.
  string listOfEquivalentDevices[]={"X","MAC","WIN"};
  for (int i=0; i<3; ++i) {
    DStructGDL *s=GraphicsDevice::GetDeviceStruct( listOfEquivalentDevices[i]);
    if (s) {
      //most important: window number, which CAN BE -1
      (*static_cast<DLongGDL*> (s->GetTag(wTag)))[0] = wIx;
      //Rest of informations 
      if (wIx >= 0 && wIx < winList.size()) {
        assert(winList[ wIx] != NULL);
        long xsize, ysize;
        winList[ wIx]->GetGeometry(xsize, ysize);
        (*static_cast<DLongGDL*> (s->GetTag(xSTag)))[0] = xsize;
        (*static_cast<DLongGDL*> (s->GetTag(ySTag)))[0] = ysize;
        (*static_cast<DLongGDL*> (s->GetTag(xVSTag)))[0] = xsize;
        (*static_cast<DLongGDL*> (s->GetTag(yVSTag)))[0] = ysize;
      }
 
    }
  }

  actWin = wIx;
  if (actWin == -1) oIx=1; //better set it here instead of in all places where actwin is called.
}
// process user deleted windows
// should be done in a thread

void GraphicsMultiDevice::TidyWindowsList() {
  int wLSize = winList.size();

  for (int i = 0; i < wLSize; i++) if (winList[i] != NULL && !winList[i]->GetValid()) {
    
    //general purpose winlist cleaning with destruction of "closed" plstreams and (eventually) associated widgets:
    //in case winList groups X11 streams (or WIN streams) *and* wxWidgets streams (GDL_USE_WX="NO") the following
    //permits to delete the widget_draw also, not only the plplot stream.
#ifdef HAVE_LIBWXWIDGETS
    if (dynamic_cast<GDLWXStream*> (winList[i]) != NULL) {
      GDLDrawPanel* panel = NULL;
      panel = dynamic_cast<GDLDrawPanel*> (static_cast<GDLWXStream*> (winList[i])->GetGDLDrawPanel());
      //test if stream is associated to graphic window or widget_draw. If graphic, destroy directly TLB widget.
      GDLWidgetDraw *draw = panel->GetGDLWidgetDraw();
      if (draw) {
        //parent of panel may be a GDLFrame. If frame is actually made by the WOpen function, destroy everything.
        GDLWidgetBase* container = NULL;
        container = static_cast<GDLWidgetBase*> (draw->GetTopLevelBaseWidget(draw->WidgetID()));
        if (container && container->IsGraphicWindowFrame()) container->SelfDestroy();
        else delete draw;
      } else delete winList[i];
    } else
#endif     
    delete winList[i];
    winList[i] = NULL;
    oList[i] = 0;
  }
  // set new actWin IF NOT VALID ANY MORE
  if (actWin < 0 || actWin >= wLSize || winList[actWin] == NULL || !winList[actWin]->GetValid()) {
    std::vector< long>::iterator mEl = std::max_element(oList.begin(), oList.end()); // most recently created
    if (*mEl == 0) { // no window open
      SetActWin(-1); //sets    oIx = 1;
    } else SetActWin(GraphicsDevice::GetDevice()->GetNonManagedWidgetActWin(false)); //get first non-managed window. false is needed. 
  }
}

void GraphicsMultiDevice::RaiseWin(int wIx) {
  if (wIx >= 0 && wIx < winList.size()) winList[wIx]->Raise();
}

void GraphicsMultiDevice::LowerWin(int wIx) {
  if (wIx >= 0 && wIx < winList.size()) winList[wIx]->Lower();
}

void GraphicsMultiDevice::IconicWin(int wIx) {
  if (wIx >= 0 && wIx < winList.size()) winList[wIx]->Iconic();
}

void GraphicsMultiDevice::DeIconicWin(int wIx) {
  if (wIx >= 0 && wIx < winList.size()) winList[wIx]->DeIconic();
}

void GraphicsMultiDevice::EventHandler() {
  if (actWin < 0) return; //would this have side effects?  
  int wLSize = winList.size();
  for (int i = 0; i < wLSize; i++)
    if (winList[i] != NULL)
      winList[i]->EventHandler();

  // TidyWindowsList(); //removing it here removes a lot of loops but needs TidyWindowsList() to be called at all places needed.
}

bool GraphicsMultiDevice::WDelete(int wIx) {
  if( wIx >= 0 && winList[ wIx] != NULL) {winList[ wIx]->SetValid(false);
    TidyWindowsList();//if WDelete is always called with a valid wIx and is always a window, not a widget, this should be OK.
    return true;
  } else return false;
}
   
bool GraphicsMultiDevice::WSize(int wIx, int *xSize, int *ySize) {
  TidyWindowsList();

  int wLSize = winList.size();
  if (wIx > wLSize || wIx < 0)
    return false;

  long xleng, yleng;
  winList[wIx]->GetGeometry(xleng, yleng);

  *xSize = xleng;
  *ySize = yleng;

  return true;
}

bool GraphicsMultiDevice::WSet(int wIx) {
  TidyWindowsList();

  int wLSize = winList.size();
  if (wIx >= wLSize || wIx < 0 || winList[wIx] == NULL)
    return false;

  SetActWin(wIx);
  return true;
}

bool GraphicsMultiDevice::WShow(int ix, bool show, int iconic) {
  TidyWindowsList();

  int wLSize = winList.size();
  if (ix >= wLSize || ix < 0 || winList[ix] == NULL) return false;

  if (iconic!=-1) { //iconic asked. do nothing else.
		if (iconic==1) IconicWin(ix); else DeIconicWin(ix);
	} else {
  
		if (show) RaiseWin(ix);  else LowerWin(ix);
  }
  UnsetFocus();

  return true;
}

int GraphicsMultiDevice::WAddFree() {
  TidyWindowsList();

  int wLSize = winList.size();
  // plplot allows only 101 windows
  if (wLSize == 101) return -1;

  for (int i = max_win; i < wLSize; i++)
    if (winList[i] == NULL) return i;

  winList.push_back(NULL);
  oList.push_back(0);
  return wLSize;
}

GDLGStream* GraphicsMultiDevice::GetStreamAt(int wIx) const {
  return winList[wIx];
}

void GraphicsMultiDevice::ChangeStreamAt(int wIx, GDLGStream* newStream) {
  if (winList[wIx]==NULL) return;
  GDLGStream* oldStream=winList[wIx];
  winList[wIx]=newStream;
  delete oldStream;
}

bool GraphicsMultiDevice::UnsetFocus() {
  return winList[actWin]->UnsetFocus();
}

bool GraphicsMultiDevice::Decomposed(bool value) {
  decomposed = value;
  return true;
}


DLong GraphicsMultiDevice::GetDecomposed() {
  // initial setting (information from the X-server needed)
  if (this->decomposed == -1) {
    if (actWin < 0) { cerr << "requesting GetDecomposed() on unexistent window " << endl; return 0;} //should not happen
    unsigned long Depth = winList[actWin]->GetWindowDepth();
    decomposed = (Depth >= 15 ? true : false);
    unsigned long nSystemColors = (1 << Depth);
    unsigned long oldColor = (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0];
    unsigned long oldNColor = (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0];
    if (this->decomposed == 1 && oldNColor == 256) {
      (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0] = nSystemColors;
      if (oldColor == 255) (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = nSystemColors - 1;
    } else if (this->decomposed == 0 && oldNColor == nSystemColors) {
      (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0] = 256;
      if (oldColor == nSystemColors - 1) (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = 255;
    }
  }
  if (decomposed) return 1;
  return 0;
}

bool GraphicsMultiDevice::SetBackingStore(int value) {
  backingStoreMode = value;
  return true;
}

bool GraphicsMultiDevice::Hide() //used as a substitute for /PIXMAP in DEVICE
{
  TidyWindowsList();
  winList[ actWin]->UnMapWindow();
  return true;
}

int GraphicsMultiDevice::MaxNonFreeWin() {
  return max_win;
}

int GraphicsMultiDevice::ActWin() {
  TidyWindowsList();
  return actWin;
}

int GraphicsMultiDevice::GetNonManagedWidgetActWin(bool doTidyWindowList) { //for case of "WSET,-1" or cases where the program has to find
  //by itself the first available window: this window must not be an active managed widget.
  //where the returned actwin is the first NON-MANAGED-WIDGET_DRAW available.

  if (doTidyWindowList) TidyWindowsList(); //bool is used in cas this function is called from... TidyWindowList itself

#ifndef HAVE_LIBWXWIDGETS
  for (int i = 0; i < winList.size(); i++) if (winList[i] != NULL ) {
    if (!(winList[i]->IsPixmapWindow())) {return i;}//first non-pixmap window.
  }
  return -1;
#else
  for (int i = 0; i < winList.size(); i++) if (winList[i] != NULL ) {
    if (!(winList[i]->IsPixmapWindow())) {//forget a pixmap window.
      if (dynamic_cast<GDLWXStream*> (winList[i]) == NULL) {
        //found non-widget window, probably an X11 one. Will return it if it is not a PIXMAP window
        if (!(winList[i]->IsPixmapWindow())) return i;
      } else { //special case of a graphic window which is either a draw widget (needs to be not managed) 
        // or a wxWindow "normal window" (in which case we return it immediately).
        GDLDrawPanel* panel = NULL;
        panel = dynamic_cast<GDLDrawPanel*> (static_cast<GDLWXStream*> (winList[i])->GetGDLDrawPanel());
        //test if stream is associated to graphic window or widget_draw. If graphic, return. If widget, return if not managed.
        GDLWidgetDraw *draw = panel->GetGDLWidgetDraw();
        if (draw) { //panel. Managed?
          //parent of panel may be a GDLFrame. 
          GDLWidgetBase* container = NULL;
          container = static_cast<GDLWidgetBase*> (draw->GetTopLevelBaseWidget(draw->WidgetID()));
          if (container && !(container->GetManaged())) return i; //first non-managed draw widget
        } else return i; //a wxWidgets "plot" (as opposed to) "widget" bona fide window to be returned.
      } 
    }
  }
  return -1; //nothing found
#endif     
}

bool GraphicsMultiDevice::CopyRegion(DLongGDL* me) {
  TidyWindowsList();
  DLong xs, ys, nx, ny, xd, yd;
  DLong source;
  xs = (*me)[0];
  ys = (*me)[1];
  nx = (*me)[2];
  ny = (*me)[3];
  xd = (*me)[4];
  yd = (*me)[5];
  if (me->N_Elements() == 7) source = (*me)[6];
  else source = actWin;
  if (!winList[ source]->GetRegion(xs, ys, nx, ny)) return false;
  return winList[ actWin ]->SetRegion(xd, yd, nx, ny);
}
