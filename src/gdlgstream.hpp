/* *************************************************************************
                          gdlgstream.hpp  -  graphic stream
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

#ifndef GDLGSTREAM_HPP_
#define GDLGSTREAM_HPP_

//debug aid. Put to 1 to debug
#ifdef GDL_DEBUG
#define GDL_DEBUG_PLSTREAM 1
#else
#define GDL_DEBUG_PLSTREAM 0
#endif

#ifdef HAVE_CONFIG_H
// we should not add all the plplot's internal defines (which are 
// added if HAVE_CONFIG_H is defined) to ours. Makes problem with distros.
#undef HAVE_CONFIG_H 
#include <plplot/plstream.h>
#include <plplot/plstrm.h>
#include <plplot/plplot.h>
#define HAVE_CONFIG_H 1
#endif
//this include should be removed as soon as plplot has a public method to give the size of a string in mm when plotted.
#if PLPLOT_PRIVATE_NOT_HIDDEN
#include <plplot/plplotP.h>
#endif
#include <string>
#include <iostream>
#include "typedefs.hpp"
#include "gdlexception.hpp"

#include "initsysvar.hpp"

#ifdef _MSC_VER
#include <algorithm>
#endif

static const std::string ALLCHARACTERSFORSTRINGLENGTHTEST = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static const float ALLCHARACTERSFORSTRINGLENGTHTEST_NCHARS = 36;
const double INCHToMM = 25.4 ; 
const double INCHToCM = 2.54 ;
const double CM_IN_MM = 10.00000000 ; 
const double DEFAULT_FONT_ASPECT_RATIO = 1.3; // Height / Width
using namespace std;
static std::string internalFontCodes[] = {
    "#fn",      // !0  : unused
    "#fn",      // !1  : unused
    "#fn",      // !2  : unused
    "#fn",      // !3  : simplex Roman (default)
    "#fn",      // !4  : simplex Greek
    "#fn",      // !5  : duplex Roman
    "#fr",      // !6  : complex Roman
    "#fr",      // !7  : complex Greek
    "#fi",      // !8  : complex italic 
    "#fn",      // !9  : math/special characters
    "#fn",      // !10 : special characters
    "#fn",      // !11 : Gothic English 
    "#fs",      // !12 : simplex script
    "#fs",      // !13 : complex script
    "#fn",      // !14 : Gothic Italian
    "#fn",      // !15 : Gothic German
    "#fn",      // !16 : Cyrillic
    "#fr",      // !17 : triplex Roman
    "#fi",      // !18 : triplex Italic
    "#fn",      // !19 : 
    "#fn",      // !20 : miscellaneous
    "#fn",      // !21 :
    "#fn",      // !22 :
    "#fn",      // !23 :
    "#fn",      // !24 :
    "#fn",      // !25 :
    "#fn",      // !26 :
    "#fn",      // !27 :
    "#fn",      // !28 :
    "#fn",      // !29 :
  };
  
// Graphic Structures:
//  typedef struct _P_GRAPHICS {
//    DLong background;
//    DFloat charSize;
//    DFloat charThick;
//    DLong clip[6];
//    DLong color;
//    DLong font;
//    DLong lineStyle;
//    DLong multi[5];
//    DLong noClip;
//    DLong noErase;
//    DLong nsum;
//    DFloat position[4];
//    DLong psym;
//    DFloat region[4];
//    DString subTitle;
//    DFloat symSize;
//    DDouble t[4][4];
//    DLong t3d;
//    DFloat thick;
//    DString title;
//    DFloat ticklen;
//    DLong channel;
//  } pstruct ;

  typedef struct GDL_BOX {
    bool initialized;
    PLFLT wx1; //world coord of x min
    PLFLT wx2;
    PLFLT wy1;
    PLFLT wy2;
    PLFLT nx1; //normalized position in subpage
    PLFLT nx2;
    PLFLT ny1;
    PLFLT ny2;
    PLFLT ndx1; //normalized device position
    PLFLT ndx2;
    PLFLT ndy1;
    PLFLT ndy2;
    PLFLT nxsize; //size of box, x , normed
    PLFLT nysize;
    PLFLT dx1; //position in device coords (e.g. pixels)
    PLFLT dx2;
    PLFLT dy1;
    PLFLT dy2;
    PLFLT dxsize; //size of box, x , normed
    PLFLT dysize;
    PLFLT pageWorldCoordinates[4];
    PLFLT subPageWorldCoordinates[4];
  } gdlbox ;

  typedef struct GDL_SUBPAGE {
    PLFLT dxsize; //subpage x size device units
    PLFLT dysize; //subpage y size device units
    PLFLT dxoff; // subpage x offset
    PLFLT dyoff; // subpage y offset
  } gdlsubpage ;

  typedef struct GDL_PAGE {
    PLFLT xdpmm; // x resolution Dots per mm
    PLFLT ydpmm; // y resolution Dots per mm
    PLFLT length; //x length (device coordinates)
    PLFLT height; //y lenght
    PLFLT plxoff; // x offset
    PLFLT plyoff; // y iffset
    PLFLT xsizemm; // size in mm, x
    PLFLT ysizemm;
    PLINT curPage; //current Page
    PLINT nbPages; //nx*ny*nz
    PLINT nx;
    PLINT ny;
    PLINT nz;
    gdlsubpage subpage;
  } gdlpage ;

  typedef struct GDL_CHARINFO {
    PLFLT scale;
    PLFLT ndsx; // size of char in normalized device units, x direction
    PLFLT ndsy; // idem y
    PLFLT dsx; // size of char in device units, x direction
    PLFLT dsy; // idem y
    DDouble mmsx; //in mm
    DDouble mmsy; //
    PLFLT wsx;  //in current world coordinates
    PLFLT wsy;
    PLFLT convx; //symbol size conversion factor, (for PSYMs, not characters: PSYMs are handled by GDL, CHARS by plplot)
    PLFLT convy; //set only while in 2D mode, used in 2D or 3D mode.
    PLFLT dspacing; // spacing of line, device units, 2nd element of device,set_character_size
    PLFLT nspacing; // in normalized units 
    PLFLT mmspacing; // in mm 
    PLFLT wspacing; // in world 
  } gdlCharInfo;

class GDLGStream: public plstream
{

protected:
  bool valid;
  gdlCharInfo theCurrentChar;
  gdlCharInfo theDefaultChar;
  int gdlDefaultCharInitialized;
  gdlbox theBox;
  gdlpage thePage;
  PLStream* pls;
  DFloat thickFactor;
  PLFLT theCurrentSymSize;
  PLFLT theLineSpacing_in_mm;
  bool usedAsPixmap; //for WINDOW,/PIXMAP retains the fact that this is a pixmap (invisible) window.
  int activeFontCodeNum; //simplex Roman by default.
public:

   GDLGStream( int nx, int ny, const char *driver, const char *file=NULL)
    : plstream( nx, ny, driver, file), valid( true), thickFactor(1.0), usedAsPixmap(false), activeFontCodeNum(3)
  {
    if (!checkPlplotDriver(driver))
      ThrowGDLException(std::string("PLplot installation lacks the requested driver: ") + driver);
    gdlDefaultCharInitialized=0;
    thePage.nbPages=0;
    thePage.length=0;
    thePage.height=0;
    thePage.plxoff=0;
    thePage.plyoff=0;
    theBox.initialized=false;
    plgpls( &pls);
    //you can debug plplot things with
//     pls->debug=1;

	if (GDL_DEBUG_PLSTREAM) printf(" new GDLGstream( %d , %d , %s ):pls=%p \n", nx, ny, driver, (void *)pls);

  }

  virtual ~GDLGStream()
  {
	  if (GDL_DEBUG_PLSTREAM) printf(" retire GDLGstream:pls=%p \n", (void *)pls);
//    plend1(); //strange. Was expected to solve #1342 but 1) I cannot reproduce now and 2) this crashes GDL in the following manner:
//    plot,dist(10) ; using X11 not wxWidgets
//    set_plot,'z'
//    plot,dist(10)
//    device, resolution=[1200,800] ; destroys and recretates the 'z' pls, but this plend1() above destroys the X11 window!!
//      
  }

  static bool checkPlplotDriver(const char *driver) {
  int numdevs = 128;
  const char **devlongnames = (const char**) malloc(numdevs * sizeof (char*));
  const char **devnames = (const char**) malloc(numdevs * sizeof (char*));
  plgDevs(&devlongnames, &devnames, &numdevs);
  bool found = false;
  for (int i = 0; i < numdevs; ++i) {
   if (strcmp(driver, devnames[i])==0){
     found = true;
     break;
    }
  }
    free(devlongnames);
    free(devnames);
    return found;
 }
   std::string getActiveFontCode(){
   return internalFontCodes[activeFontCodeNum];
  }
  
  static void SetErrorHandlers();

  virtual void Init()=0;
  virtual bool IsWxStream(){return false;}
  // called after draw operation
  //virtual void Update() {}
  virtual void Update(){plstream::cmd(PLESC_EXPOSE, NULL);}
  
  virtual void EventHandler() {}

  virtual void GetGeometry( long& xSize, long& ySize);
  virtual unsigned long GetWindowDepth () {return 0;}
  virtual DLong GetVisualDepth() {return -1;}
  virtual DString GetVisualName() {return "";}
  virtual BaseGDL* GetFontnames(DString pattern) {return NULL;}
  virtual DLong GetFontnum(DString pattern) {return 0;}
  virtual bool UnsetFocus(){return false;}
  virtual bool SetBackingStore(int value){return false;}
  virtual bool SetGraphicsFunction(long value ){return false;}
  virtual bool GetWindowPosition(long& xpos, long& ypos ){return false;}
  virtual bool GetScreenResolution(double& resx, double& resy){return false;}
  virtual bool CursorStandard(int cursorNumber){return false;}
  virtual bool CursorImage(char* v, int x, int y, char* m) {return false;}
  virtual void eop()          { plstream::eop();}
  virtual void SetDoubleBuffering() {}
  virtual void UnSetDoubleBuffering() {}
  virtual bool HasDoubleBuffering() {return pls->db;}
  virtual bool HasSafeDoubleBuffering() {return false;}
  virtual void Raise()         {}
  virtual void Lower()        {}
  virtual void Iconic()        {}
  virtual void DeIconic()        {}
  virtual bool GetGin(PLGraphicsIn *gin, int mode) {return 0;}
  virtual void WarpPointer(DLong x, DLong y){}
  virtual void Flush() {}
  virtual void Clear()         {}
  virtual void Clear( DLong chan)          {}
  virtual bool PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos, DLong tru, DLong chan){return false;}
  virtual bool HasCrossHair() {return false;}
  virtual void UnMapWindowAndSetPixmapProperty() {usedAsPixmap=true;} 
  bool IsPixmapWindow() {return usedAsPixmap;}
  virtual bool IsPlot() {return true;} //except some wxWidgets
  virtual BaseGDL* GetBitmapData(int xoff, int yoff, int nx, int ny){return NULL;}
  virtual void SetCurrentFont(std::string fontname){}//do nothing
  bool GetRegion(DLong& xs, DLong& ys, DLong& nx, DLong& ny);//{return false;}
  bool SetRegion(DLong& xd, DLong& yd, DLong& nx, DLong& ny);//{return false;}

  virtual void CheckValid() {}
  void SetValid( bool v) { valid = v;}
  bool GetValid() { return valid;}
  bool validWorldBox()
  {
    if (((theBox.wx1==0) && (theBox.wx2==0)) 
	|| ((theBox.wy1==0) && (theBox.wy2==0))) return false; else return true;
  }
  void getCurrentWorldBox(PLFLT &xmin, PLFLT &xmax, PLFLT &ymin, PLFLT &ymax) {
   xmin=theBox.wx1;
   ymin=theBox.wy1;
   xmax=theBox.wx2;
   ymax=theBox.wy2;
  }
  bool validNormdBox()
  {
    if (((theBox.nx1==0) && (theBox.nx2==0)) 
	|| ((theBox.ny1==0) && (theBox.ny2==0))) return false; else return true;
  }
  void getCurrentNormBox(PLFLT &xmin, PLFLT &xmax, PLFLT &ymin, PLFLT &ymax) {
   xmin=theBox.nx1;
   ymin=theBox.ny1;
   xmax=theBox.nx2;
   ymax=theBox.ny2;
  }
  inline PLFLT getPsymConvX(){return theCurrentChar.convx;}
  inline PLFLT getPsymConvY(){return theCurrentChar.convy;}
  inline void setSymbolSizeConversionFactors(){
    PLFLT symsize=this->getSymbolSize();
    theCurrentChar.convx=(0.5*symsize*(this->wCharLength()/this->charScale())); //be dependent only on symsize!
    theCurrentChar.convy=(0.5*symsize*(this->wCharHeight()/this->charScale()));
    PLFLT wun, wdeux, wtrois, wquatre; //take care of axes world orientation!
    this->pageWorldCoordinates(wun, wdeux, wtrois, wquatre);
    if ((wdeux-wun)<0) theCurrentChar.convx*=-1.0;
    if ((wquatre-wtrois)<0) theCurrentChar.convy*=-1.0;} 
  inline PLFLT charScale(){return theCurrentChar.scale;}
  inline PLFLT nCharLength(){return theCurrentChar.ndsx;}
  inline PLFLT nCharHeight(){return theCurrentChar.ndsy;}
  inline PLFLT nLineSpacing(){return theCurrentChar.nspacing;}
  inline PLFLT dCharLength(){return theCurrentChar.dsx;}
  inline PLFLT dCharHeight(){return theCurrentChar.dsy;}
  inline PLFLT dLineSpacing(){return theCurrentChar.dspacing;}
  inline PLFLT wCharLength(){return theCurrentChar.wsx;}
  inline PLFLT wCharHeight(){return theCurrentChar.wsy;}
  inline PLFLT wLineSpacing(){return theCurrentChar.wspacing;}
  inline DDouble mmCharLength(){return theCurrentChar.mmsx;}
  inline DDouble mmCharHeight(){return theCurrentChar.mmsy;}
  inline DDouble mmLineSpacing(){return theCurrentChar.mmspacing;}
  inline PLFLT mmxPageSize(){return thePage.xsizemm;} //size in mm
  inline PLFLT mmyPageSize(){return thePage.ysizemm;}
  inline PLFLT boxnXSize(){return theBox.nxsize;}
  inline PLFLT boxnYSize(){return theBox.nysize;}
  inline PLFLT xPageSize(){return thePage.length;} //size in units (alternate:{return pls->xlength;})
  inline PLFLT yPageSize(){return thePage.height;} //alternate: {return pls->ylength;}
  inline PLFLT xSubPageSize(){return thePage.subpage.dxsize;} //size in units
  inline PLFLT ySubPageSize(){return thePage.subpage.dysize;}

//  // bunch of conversion functions that should be used in the future now that pls is here!
//  // (normed) device coords to physical coords (x,y) 0..1 -> 0->32768
//  inline PLFLT nd2px(PLFLT x){ return ( pls->phyxmi + pls->phyxlen * x  );}
//  inline PLFLT nd2py(PLFLT y){ return ( pls->phyymi + pls->phyylen * y  );}
//  inline void norm2physical(PLFLT devx, PLFLT devy, PLFLT &physx, PLFLT &physy)
//  { physx=nd2px(devx); physy=nd2py(devy);}
  // (normed) device to mm
  inline PLFLT nd2mx(PLFLT x){ return (PLFLT) ( x * abs( pls->phyxma - pls->phyxmi ) / pls->xpmm ) ;}
  inline PLFLT nd2my(PLFLT y){ return (PLFLT) ( y * abs( pls->phyyma - pls->phyymi ) / pls->ypmm ) ;}
  inline void norm2mm(PLFLT devx, PLFLT devy, PLFLT &mmx, PLFLT &mmy)
  { mmx=nd2mx(devx); mmy=nd2my(devy);}
//  //(normed) device to world
  inline PLFLT nd2wx(PLFLT x){return (PLFLT) ( (x- pls->wdxoff) / pls->wdxscl );}
  inline PLFLT nd2wy(PLFLT y){return (PLFLT) ( (y- pls->wdyoff) / pls->wdyscl );}
//  inline void norm2world(PLFLT devx, PLFLT devy, PLFLT &wx, PLFLT &wy)
//  { wx=nd2wx(devx); wy=nd2wy(devy);}
//  // (normed) device coords to subpage coords
//  inline PLFLT nd2spx(PLFLT x){ return (PLFLT) ( ( x - pls->spdxmi ) / ( pls->spdxma - pls->spdxmi ) ) ;}
//  inline PLFLT nd2spy(PLFLT y){ return (PLFLT) ( ( y - pls->spdymi ) / ( pls->spdyma - pls->spdymi ) ) ;}
//  inline void norm2subpage(PLFLT devx, PLFLT devy, PLFLT &spx, PLFLT &spy)
//  { spx=nd2spx(devx); spy=nd2spy(devy);}
//
//  // millimeters to physical coords (x,y)
//  inline PLFLT mm2px(PLFLT x){ return ( pls->phyxmi + pls->xpmm * x  );}
//  inline PLFLT mm2py(PLFLT y){ return ( pls->phyymi + pls->ypmm * y  );}
//  inline void mm2physical(PLFLT mmx, PLFLT mmy, PLFLT &physx, PLFLT &physy)
//  { physx=mm2px(mmx); physy=mm2py(mmy);}
//  // mm to (absolute) device
  inline PLFLT mm2adx(PLFLT x){ return (PLFLT) ( ( x * pls->xpmm ) / abs( pls->phyxma - pls->phyxmi )*thePage.length);}
  inline PLFLT mm2ady(PLFLT y){ return (PLFLT) ( ( y * pls->ypmm ) / abs( pls->phyyma - pls->phyymi )*thePage.height);}
  inline void mm2device(PLFLT mmx, PLFLT mmy, PLFLT &devx, PLFLT &devy)
  { devx=mm2adx(mmx); devy=mm2ady(mmy);}
//  // mm to (normed) device
  inline PLFLT mm2ndx(PLFLT x){ return (PLFLT) ( ( x * pls->xpmm ) / abs( pls->phyxma - pls->phyxmi ));}
  inline PLFLT mm2ndy(PLFLT y){ return (PLFLT) ( ( y * pls->ypmm ) / abs( pls->phyyma - pls->phyymi ));}
//  inline void mm2norm(PLFLT mmx, PLFLT mmy, PLFLT &devx, PLFLT &devy)
//  { devx=mm2ndx(mmx); devy=mm2ndy(mmy);}
//  // mm to world
  inline PLFLT mm2wx(PLFLT x){ x=mm2ndx(x); return nd2wx(x);}
  inline PLFLT mm2wy(PLFLT y){ y=mm2ndy(y); return nd2wy(y);}
//  inline void mm2world(PLFLT mmx, PLFLT mmy, PLFLT &wx, PLFLT &wy)
//  { wx=mm2wx(mmx); wy=mm2wy(mmy);}
//  // mm to subpage coord
//  inline PLFLT mm2spx(PLFLT x){ x=mm2ndx(x); return nd2spx(x);}
//  inline PLFLT mm2spy(PLFLT y){ y=mm2ndy(y); return nd2spy(y);}
//  inline void mm2subpage(PLFLT mmx, PLFLT mmy, PLFLT &spx, PLFLT &spy)
//  { spx=mm2spx(mmx); spy=mm2spy(mmy);}
//
//  // world to physical coords
//  inline PLFLT w2px(PLFLT x){ return ( pls->wpxoff + pls->wpxscl * x  );}
//  inline PLFLT w2py(PLFLT y){ return ( pls->wpyoff + pls->wpyscl * y  );}
//  inline void world2physical(PLFLT wx, PLFLT wy, PLFLT &physx, PLFLT &physy)
//  { physx=w2px(wx); physy=w2py(wy);}
//  // world to (normed) device
//  inline PLFLT w2ndx(PLFLT x){ return (PLFLT) ( pls->wdxoff + pls->wdxscl * x );}
//  inline PLFLT w2ndy(PLFLT y){ return (PLFLT) ( pls->wdyoff + pls->wdyscl * y );}
//  inline void world2norm(PLFLT wx, PLFLT wy, PLFLT &devx, PLFLT &devy)
//  { devx=w2ndx(wx); devy=w2ndy(wy);}
//   // world to (absolute) device
  inline PLFLT w2adx(PLFLT x){ return (PLFLT) ( pls->wdxoff + pls->wdxscl * x )*thePage.length;}
  inline PLFLT w2ady(PLFLT y){ return (PLFLT) ( pls->wdyoff + pls->wdyscl * y )*thePage.height;}
  inline void world2device(PLFLT wx, PLFLT wy, PLFLT &devx, PLFLT &devy)
  { devx=w2adx(wx); devy=w2ady(wy);}
//  //world to mm
//  inline PLFLT w2mmx(PLFLT x){ return (PLFLT) ( pls->wmxoff + pls->wmxscl * x );}
//  inline PLFLT w2mmy(PLFLT y){ return (PLFLT) ( pls->wmyoff + pls->wmyscl * y );}
//  inline void world2mm(PLFLT wx, PLFLT wy, PLFLT &mmx, PLFLT &mmy)
//  { mmx=w2mmx(wx); mmy=w2mmy(wy);}
//  //world to subpage coord
//  inline PLFLT w2spx(PLFLT x){ x=w2ndx(x) ; return nd2spx(x);}
//  inline PLFLT w2spy(PLFLT y){ y=w2ndy(y) ; return nd2spy(y);}
//  inline void world2subpage(PLFLT wx, PLFLT wy, PLFLT &spx, PLFLT &spy)
//  { spx=w2spx(wx); spy=w2spy(spy);}
//
//  // physical to (normed) device
//  inline PLFLT p2ndx(PLFLT x){ return (PLFLT) ( ( x - pls->phyxmi ) / (double) pls->phyxlen );}
//  inline PLFLT p2ndy(PLFLT y){ return (PLFLT) ( ( y - pls->phyymi ) / (double) pls->phyylen );}
//  inline void physical2device(PLFLT physx, PLFLT physy, PLFLT &devx, PLFLT &devy)
//  { devx=p2ndx(physx); devy=p2ndy(physy);}
//  //physical to world
//  //physical to mm
//  //physical to subpage coord
//
//  // subpage coords to (normed) device coords
//  inline PLFLT sp2ndx(PLFLT x){ return (PLFLT) ( pls->spdxmi + ( pls->spdxma - pls->spdxmi ) * x ) ;}
//  inline PLFLT sp2ndy(PLFLT y){ return (PLFLT) ( pls->spdymi + ( pls->spdyma - pls->spdymi ) * y ) ;}
//  inline void subpage2norm(PLFLT spx, PLFLT spy, PLFLT &devx, PLFLT &devy)
//  { devx=sp2ndx(spx); devy=sp2ndy(spy);}
//  //subpage to world
//  //subpage to mm
//  //subpage to physical

#if PLPLOT_PRIVATE_NOT_HIDDEN
  //use simple internal function
  PLFLT gdlGetStringLength(const std::string &s)
  {
    return plstrl(s.c_str());
  }
#endif

//  void  currentPhysicalPos(PLFLT &x, PLFLT &y)
//  {
//    x=pls->currx; //Physical x-coordinate of current point
//    y=pls->curry;
//  }
//  void  currentWorldPos(PLFLT &x, PLFLT &y)
//  {
//    x=pls->currx; //Physical x-coordinate of current point
//    y=pls->curry;
//    x=(x-pls->wpxoff)/pls->wpxscl;
//    y=(y-pls->wpyoff)/pls->wpyscl;
//  }
//  void  currentNormedDevicePos(PLFLT &x, PLFLT &y)
//  {
//    x=pls->currx; //Physical x-coordinate of current point
//    y=pls->curry;
//    x=p2ndx(x);
//    y=p2ndy(y);
//  }
  
//  void  currentMmPos(PLFLT &x, PLFLT &y)
//  {
//    x=pls->currx; //Physical x-coordinate of current point
//    y=pls->curry;
//    x=p2ndx(x);x=nd2mx(x);
//    y=p2ndy(y);y=nd2my(y);
//  }

  void  pageWorldCoordinates(PLFLT &wxmin, PLFLT &wxmax, PLFLT &wymin, PLFLT &wymax)
  {
      wxmin=theBox.pageWorldCoordinates[0];
      wxmax=theBox.pageWorldCoordinates[1];
      wymin=theBox.pageWorldCoordinates[2];
      wymax=theBox.pageWorldCoordinates[3];
  }
//  void  subPageWorldCoordinates(PLFLT &wxmin, PLFLT &wxmax, PLFLT &wymin, PLFLT &wymax)
//  {
//      wxmin=theBox.subPageWorldCoordinates[0];
//      wxmax=theBox.subPageWorldCoordinates[1];
//      wymin=theBox.subPageWorldCoordinates[2];
//      wymax=theBox.subPageWorldCoordinates[3];
//  }
//  void  boxDeviceCoordinates(PLFLT &wxmin, PLFLT &wxmax, PLFLT &wymin, PLFLT &wymax)
//  {
//      wxmin=theBox.dx1;
//      wxmax=theBox.dx2;
//      wymin=theBox.dy1;
//      wymax=theBox.dy2;
//  }
  PLFLT  boxAspectDevice(){return (theBox.dy2-theBox.dy1)/(theBox.dx2-theBox.dx1);}
  PLFLT  boxAspectWorld(){return fabs(theBox.wy2-theBox.wy1)/fabs(theBox.wx2-theBox.wx1);}

  bool updatePageInfo();
  
  inline void NormToDevice(PLFLT normx, PLFLT normy, PLFLT &devx, PLFLT &devy)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"NormToDevice()\n");
    devx=normx*thePage.subpage.dxsize+thePage.subpage.dxoff;
    devy=normy*thePage.subpage.dysize+thePage.subpage.dyoff;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"        input [%f,%f] output [%f,%f]\n", normx, normy, devx, devy);
  }

  inline void NormedDeviceToDevice(PLFLT normx, PLFLT normy, PLFLT &devx, PLFLT &devy)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"NormedDeviceToDevice()\n");
    devx=normx*thePage.length;
    devy=normy*thePage.height;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"         input [%f,%f] output [%f,%f]\n", normx, normy, devx, devy);
  }

  inline void DeviceToNorm(PLFLT devx, PLFLT devy, PLFLT &normx, PLFLT &normy)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"DeviceToNorm()\n");
    normx=(devx-thePage.subpage.dxoff)/thePage.subpage.dxsize;
    normy=(devy-thePage.subpage.dyoff)/thePage.subpage.dysize;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"          input [%f,%f] output [%f,%f]\n", devx, devy, normx, normy);
  }
  inline void DeviceToNormedDevice(PLFLT devx, PLFLT devy, PLFLT &normx, PLFLT &normy)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"DeviceToNormedDevice()\n");
    normx=devx/thePage.length;
    normy=devy/thePage.height;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"          input [%f,%f] output [%f,%f]\n", devx, devy, normx, normy);
  }
  inline void NormToWorld(PLFLT normx, PLFLT normy, PLFLT &worldx, PLFLT &worldy)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"NormToWorld()\n");
    DDouble s1,s2;
    s1=(theBox.wx2-theBox.wx1)/(theBox.nx2-theBox.nx1);
    s2=theBox.wx1;
    worldx=s1*(normx-theBox.nx1)+s2;
    s1=(theBox.wy2-theBox.wy1)/(theBox.ny2-theBox.ny1);
    s2=theBox.wy1;
    worldy=s1*(normy-theBox.ny1)+s2;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"         input [%f,%f] output [%f,%f]\n", normx, normy, worldx, worldy);
  }

  inline void NormedDeviceToWorld(PLFLT normx, PLFLT normy, PLFLT &worldx, PLFLT &worldy)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"NormedDeviceToWorld()\n");
    DDouble s1,s2;
    s1=(theBox.wx2-theBox.wx1)/(theBox.ndx2-theBox.ndx1);
    s2=theBox.wx1;
    worldx=s1*(normx-theBox.ndx1)+s2;
    s1=(theBox.wy2-theBox.wy1)/(theBox.ndy2-theBox.ndy1);
    s2=theBox.wy1;
    worldy=s1*(normy-theBox.ndy1)+s2;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"          input [%f,%f] (nd) output [%f,%f] (w)\n", normx, normy, worldx, worldy);
  }

  inline void WorldToNormedDevice(PLFLT worldx, PLFLT worldy, PLFLT &normx, PLFLT &normy)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"WorldToNormedDevice()\n");
    DDouble s1,s2;
    s1=(theBox.ndx2-theBox.ndx1)/(theBox.wx2-theBox.wx1);
    s2=theBox.ndx1;
    normx=s1*(worldx-theBox.wx1)+s2;
    s1=(theBox.ndy2-theBox.ndy1)/(theBox.wy2-theBox.wy1);
    s2=theBox.ndy1;
    normy=s1*(worldy-theBox.wy1)+s2;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"    input [%f,%f](w) output [%f,%f](nd)\n", worldx, worldy, normx, normy);
  }


  inline void DeviceToWorld(PLFLT devx, PLFLT devy, PLFLT &worldx, PLFLT &worldy)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"DeviceToWorld()\n");
    PLFLT normx, normy;
    DeviceToNormedDevice(devx, devy, normx, normy);
    NormedDeviceToWorld(normx, normy, worldx, worldy);
  }

  inline void WorldToDevice(PLFLT worldx, PLFLT worldy, PLFLT &devx, PLFLT &devy)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"WorldToDevice()\n");
    PLFLT normx, normy;
    WorldToNormedDevice(worldx, worldy, normx, normy);
    NormedDeviceToDevice(normx, normy,  devx, devy);
  }
  
  void Thick( DFloat thick);
  virtual void Color( ULong c, DLong decomposed=0);
  void Background( ULong c, DLong decomposed=0);
  void DefaultBackground();
  void SetColorMap1SingleColor( ULong color);
  //if decomposed, create a red ramp. If not, copy the colormap0 in colormap1
  void SetColorMap1DefaultColors(PLINT ncolors, DLong decomposed=0);
  //if create a colormap1 color with passed colors, palette will be tableSize and colors assigned like in contour.
  void SetColorMap1Table(PLINT tableSize, DLongGDL *colors, DLong decomposed=0);
  //if create a colormap1 with a black to white ramp.
  DLong ForceColorMap1Ramp(PLFLT minlight=0.5);
  void DefaultCharSize();
  void SetCharSize(DLong chx, DLong chy);
  void NextPlot( bool erase=true); // handles multi plots

  void NoSub(); // no subwindows (/NORM, /DEVICE)

  void CurrentCharSize(PLFLT scale);

  void UpdateCurrentCharWorldSize(); 
  void GetPlplotDefaultCharSize();

  // SA: overloading plplot methods in order to handle IDL-plplot extended
  // text formating syntax conversion
  std::string TranslateFormatCodes(const char *text, double *stringLength);
  void setSymbolSize( PLFLT scale );
  void setLineSpacing( PLFLT spacing );
  PLFLT getSymbolSize();
  void mtex( const char *side, PLFLT disp, PLFLT pos, PLFLT just,
                         const char *text, double *stringCharLength=NULL, double *stringCharHeight=NULL);
  void ptex( PLFLT x, PLFLT y, PLFLT dx, PLFLT dy, PLFLT just,
                         const char *text, double *stringCharLength=NULL );
  void setVariableCharacterSize( PLFLT charwidthpixel, PLFLT scale, PLFLT lineSpacingpixel, PLFLT xpxcm, PLFLT ypxcm);
  void setFixedCharacterSize( PLFLT charwidthpixel, PLFLT scale, PLFLT lineSpacingpixel);
  virtual void fontChanged(){}; //nothing here
  void sizeChar(PLFLT scale);
  bool vpor( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );
  bool isovpor( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT aspect );
//  void gvpd( PLFLT& xmin, PLFLT& xmax, PLFLT& ymin, PLFLT& ymax );
  void wind( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );
  void wind( PLFLT xmin, PLFLT xmax, bool xLog, PLFLT ymin, PLFLT ymax, bool yLog );
  void ssub( PLINT nx, PLINT ny, PLINT nz=1);
  void adv(PLINT page);
  void getSubpageRegion(PLFLT *sxmin, PLFLT *symin, PLFLT *sxmax, PLFLT *symax, PLFLT *zmin=NULL, PLFLT *zmax=NULL);
  void getCurrentSubpageInfo(PLFLT &xratio, PLFLT &yratio, PLFLT &zratio, PLFLT* displacement);
  void SetPageDPMM(float setPsCharFudge=1.0, float setPsSymFudge=1.0);
  void syncPageInfo();
  void updateBoxDeviceCoords();

  inline void setThickFactor(DFloat tf)
  {
      thickFactor=tf;
  }

  //GD: enables overloading scmap0,1... to accelerate plots for X11 and possibly others
  // Set color map 0 colors by 8 bit RGB values
  virtual void SetColorMap0( PLINT *r, PLINT *g, PLINT *b, PLINT ncol0 ) {
   plstream::scmap0( r, g, b, ncol0);
  }
  // Set color map 1 colors by 8 bit RGB values
  virtual void SetColorMap1( PLINT *r, PLINT *g, PLINT *b, PLINT ncol1 ) {
   plstream::scmap1( r, g, b, ncol1);
  }
  // Set color map 1 colors using a piece-wise linear relationship between
  // intensity [0,1] (cmap 1 index) and position in HLS or RGB color space.
  virtual void SetColorMap1l( bool itype, PLINT npts, PLFLT *intensity, PLFLT *coord1, PLFLT *coord2, PLFLT *coord3, const bool *rev = NULL ) {
   plstream::scmap1l(itype,npts,intensity,coord1,coord2,coord3,rev);
  }
  // Set number of colors in cmap 1
  virtual void SetColorMap1n( PLINT ncol1 ){
   plstream::scmap1n(ncol1);
  }


};

#endif
