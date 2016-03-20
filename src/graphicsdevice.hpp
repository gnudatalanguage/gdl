/* *************************************************************************
                          graphicsdevice.hpp  -  GDL graphical output
renamed from: graphics.hpp                          
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

/* 

GDL Graphic subsytem:

GraphicsDevice - base subsystem class
DeviceXXX - derived from Graphics, subsystem for device XXX


GDLGStream - base graphic stream class (= windows, printer page)
GDLXXXStream - derived from GDLGStream for XXX type of stream


So for each device XXX there is a:
DeviceXXX
and at least one
GDLXXXStream
 
Devices are (note that on a given platform not all devices are available): 
X   - X windows (GDLXStream, GDLWXStream)
WIN - Windows
PS  - postscript output
SVG - a SVG compliant file.

*/


#ifndef GRAPHICSDEVICE_HPP_
#define GRAPHICSDEVICE_HPP_

#include <vector>

#include "datatypes.hpp" // DByte
#include "dstructgdl.hpp"
#include "gdlgstream.hpp"

#define MAX_WIN 32  //IDL free and widgets start at 32 ...
#define MAX_WIN_RESERVE 256

const UInt max_win = MAX_WIN;
const UInt max_win_reserve = MAX_WIN_RESERVE;
const UInt ctSize = 256;

class GDLCT
{
  DByte  r[ ctSize];
  DByte  g[ ctSize];
  DByte  b[ ctSize];

  UInt    actSize;

  std::string name;

public:
  GDLCT( const std::string& n, DByte* r_, DByte* g_, DByte* b_, SizeT nCol = ctSize):
    actSize( nCol), name( n)
  {
    SizeT i=0;
    for(;i<nCol;++i)
      {
	r[i] = r_[i];
	g[i] = g_[i];
	b[i] = b_[i];
      }
    for(;i<ctSize;++i)
      {
	r[i] = 0;
	g[i] = 0;
	b[i] = 0;
      }
  }

  // default is greyscale
  GDLCT(): actSize( ctSize), name("DEFAULT")
  {
    for( UInt i=0; i<ctSize; i++)
      {
	r[i]=g[i]=b[i]=i;
      }
  }

  ~GDLCT() {}

  bool Get( PLINT r_[], PLINT g_[], PLINT b_[], UInt nCol=ctSize) const;

  bool Get( UInt ix, DByte& r_, DByte& g_, DByte& b_) const;
  bool Set( UInt ix, DByte r_, DByte g_, DByte b_); // RGB
  bool SetHLS( UInt ix, DFloat h, DFloat l, DFloat s);
  bool SetHSV( UInt ix, DFloat h, DFloat s, DFloat v);

  std::string Name() const { return name;}
};

class GDLGStream;
class   GraphicsDevice;
typedef std::vector< GraphicsDevice*> DeviceListT;

class GraphicsDevice
{
  static void InitCT();         // preset CT and actCT

  static GraphicsDevice*    actDevice;
  static DeviceListT  deviceList;
  static GraphicsDevice*    actGUIDevice;
  
  static void DefineDStructDesc(); // modifies structList
  unsigned char* CopyBuffer;
  SizeT CopyBufferSize;
  
protected:
  static int wTag, xSTag, ySTag, xVSTag, yVSTag, n_colorsTag; // !D tag indices

  static std::vector<GDLCT> CT; // predefined colortables
  static GDLCT           actCT; // actual used colortable
  static DByte           deviceBckColorR;
  static DByte           deviceBckColorG;
  static DByte           deviceBckColorB;

  std::string         name;
  DStructGDL*         dStruct;

  void GetWinSize( DLong& x, DLong& y)
  {
    int tag = dStruct->Desc()->TagIndex( "X_SIZE");
    DLongGDL* xSize = static_cast<DLongGDL*>( dStruct->GetTag( tag));
    tag = dStruct->Desc()->TagIndex( "Y_SIZE");
    DLongGDL* ySize = static_cast<DLongGDL*>( dStruct->GetTag( tag));
    x = (*xSize)[0];
    y = (*ySize)[0];
  }


public:
  GraphicsDevice();
  virtual ~GraphicsDevice();

  static void Init();
  static void DestroyDevices();
  static void HandleEvents();

  static void LoadCT(UInt iCT);
  void SetDeviceBckColor(DByte r, DByte g, DByte b)
  {
    deviceBckColorR=r;
    deviceBckColorG=g;
    deviceBckColorB=b;
  }
  DByte BackgroundR() {return deviceBckColorR;}
  DByte BackgroundG() {return deviceBckColorG;}
  DByte BackgroundB() {return deviceBckColorB;}
  
  static GDLCT*      GetCT() { return &actCT;}
  static GDLCT*      GetCT( SizeT ix) { return &CT[ix];}
  static SizeT       N_CT() { return CT.size();}
  static void        ListDevice();
  static bool        ExistDevice( const std::string& device, int &index);
  static bool        SetDevice( const std::string& devName);
  static GraphicsDevice*   GetDevice() { return actDevice;}
  static GraphicsDevice*   GetGUIDevice() { return actGUIDevice;}
  static DStructGDL* DStruct()   { return actDevice->dStruct;} 

  const DString     Name() { return name;}

  unsigned char* GetCopyBuffer() {return CopyBuffer;}
  SizeT GetCopyBufferSize() {return CopyBufferSize;}
  unsigned char* SetCopyBuffer(SizeT size) 
  {
    if (CopyBufferSize != 0) {free (CopyBuffer); CopyBufferSize = 0;}
    CopyBuffer=(unsigned char*)calloc(size, sizeof(char)); //set to zero
    CopyBufferSize = size;
    return CopyBuffer;
  }

  
  virtual GDLGStream* GetStreamAt( int wIx) const     { return NULL;}
  virtual GDLGStream* GetStream( bool open=true)      { return NULL;}
  virtual bool WSet( int ix)                          { return false;}
  virtual int  WAddFree()                                 { return false;}

  // for WIDGET_DRAW
  virtual bool GUIOpen( int wIx, int xSize, int ySize) { return false;} 

  // for plot windows
  virtual bool WOpen( int ix, const std::string& title,
		      int xsize, int ysize, 
		      int xpos, int ypos, bool hide)  { return false;}
  virtual bool WSize( int ix,
		      int* xsize, int* ysize, 
		      int* xpos, int* ypos)           { return false;}
  virtual bool WShow( int ix, bool show, bool iconic) { return false;}
  virtual bool WState( int ix)                        { return false;}
  virtual bool WDelete( int ix)                       { return false;}
  virtual int  MaxWin()                               { return 0;}
  virtual void TidyWindowsList()                      {}
  virtual int  MaxNonFreeWin()                        { return 0;}
  virtual int  ActWin()                               { return -1;}
  virtual void EventHandler() {}
  virtual void DefaultXYSize(DLong *xsize, DLong *ysize) {
							*xsize=640, *ysize=480; return;}
  virtual void MaxXYSize(DLong *xsize, DLong *ysize) {
							*xsize=1200, *ysize=800; return;}
  virtual DLong GetDecomposed()                       { return -1;}
  virtual DLong GetGraphicsFunction()                 { return -1;}
  virtual DIntGDL* GetPageSize()                      { return NULL;}
  virtual DLong GetPixelDepth()                       { return -1;}
  virtual DDoubleGDL* GetScreenResolution(char* disp=NULL)  //fake a basic screen if not implemented:
  {
    DDoubleGDL* res;
    res = new DDoubleGDL(2, BaseGDL::NOZERO);
    (*res)[0]=1.0;
    (*res)[1]=1.0;
    return res;
  }
//  virtual DFloatGDL* GetScreenSize(char* disp=NULL)     { return NULL;}
  virtual DIntGDL* GetScreenSize(char* disp=NULL) //fake a basic screen if not implemented:
  {
    DIntGDL* res;
    res = new DIntGDL(2, BaseGDL::NOZERO);
    (*res)[0]=640;
    (*res)[1]=480;
    return res;
  }
  virtual DLong GetVisualDepth()                      { return -1;}
  virtual DString GetVisualName()                     { return "";}
  virtual DIntGDL* GetWindowPosition()                { return NULL;}
  virtual DLong GetWriteMask()                        { return -1;}
  virtual DByteGDL* WindowState()                     { return NULL;}
  virtual bool CloseFile()                            { return false;}
  virtual bool SetFileName( const std::string& f)     { return false;}
  virtual bool Decomposed( bool value)                { return false;}
  virtual bool SetGraphicsFunction( DLong value)      { return false;}
  virtual bool CursorStandard( int value)             { return false;}
  virtual bool CursorCrosshair()                      { return false;}
  virtual int  getCursorId()                             { return -1;}
  virtual bool UnsetFocus()                           { return false;}
  virtual bool SetFocus()                             { return false;}
  virtual bool SetBackingStore(int value)             { return false;}
  virtual int  getBackingStore()                      { return -1;}
  virtual bool SetXPageSize( const float xs)          { return false;}
  virtual bool SetYPageSize( const float ys)          { return false;}
  virtual bool SetColor(const long color=0)           { return false;}
  virtual bool SetScale(const float)                  { return false;}
  virtual bool SetXOffset(const float)                { return false;}
  virtual bool SetYOffset(const float)                { return false;}
  virtual bool SetPortrait()                          { return false;}
  virtual bool SetLandscape()                         { return false;}
  virtual bool SetEncapsulated(bool val)              { return false;}
  virtual bool SetBPP(const int bpp)                  { return false;}
  virtual bool Hide()                                 { return false;}
  virtual bool CopyRegion(DLongGDL* me)               { return false;}

  // Z buffer device
  virtual bool ZBuffering( bool yes)                  { return false;}
  virtual bool SetResolution( DLong nx, DLong ny)     { return false;}

  virtual void ClearStream( DLong bColor)
  {
    throw GDLException( "Device "+Name()+" does not support ClearStream.");
  }
};


typedef std::vector< GDLGStream*> WindowListT;

class GraphicsMultiDevice : public GraphicsDevice {
private:
public:
    int decomposed; // false -> use color table
    int cursorId; //should be 3 by default.
    long gcFunction;
    int backingStoreMode;

  int getCursorId(){return cursorId;}
  long getGCFunction(){return gcFunction;}
  int GetBackingStore(){return backingStoreMode;}

  static int actWin;
  static WindowListT winList;
  static std::vector<long> oList;
  static int oIx;
  static void Init();
  GraphicsMultiDevice( int _decomposed, int _cursorId, long _gcFunction, int _backingStoreMode) : GraphicsDevice(),
  decomposed(_decomposed),
  cursorId(_cursorId),
  gcFunction(_gcFunction),
  backingStoreMode(_backingStoreMode)
  {
      //pretty much nothing to do...
  }
  ~GraphicsMultiDevice() {
        WindowListT::iterator i;
        for (i = winList.begin(); i != winList.end(); ++i) if ((*i) != NULL) {delete *i; *i = NULL;  } 
  }
  DByteGDL* WindowState();
  bool WState( int ix);
  int  MaxWin();
  void SetActWin(int wIx);
  void TidyWindowsList();
  void RaiseWin(int wIx);
  void LowerWin(int wIx);
  void IconicWin(int wIx);
  void DeIconicWin(int wIx);
  void EventHandler();
  bool WDelete(int wIx);
  bool WSize(int wIx, int *xSize, int *ySize, int *xPos, int *yPos);
  bool WSet(int wIx);
  bool WShow(int ix, bool show, bool iconic);
  int WAddFree();
  GDLGStream* GetStreamAt(int wIx) const;
  bool UnsetFocus();
  bool Decomposed(bool value);
  DLong GetDecomposed();
  bool SetBackingStore(int value);
  bool Hide(); 
  int MaxNonFreeWin();
  int ActWin();
  bool CopyRegion(DLongGDL* me);
  
};

#endif

