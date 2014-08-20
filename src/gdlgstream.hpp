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
#define GDL_DEBUG_PLSTREAM 0

#include <plplot/plstream.h>
#include <plplot/plstrm.h>
#ifndef HAVE_X
#else
#  include <plplot/plxwd.h>
#endif
#ifdef HAVE_CONFIG_H
// we should not add all the plplot's internal defines (which are 
// added if HAVE_CONFIG_H is defined) to ours. Makes problem with distros.
#undef HAVE_CONFIG_H 
#include <plplot/plplot.h>
#define HAVE_CONFIG_H 1
#endif
#if PLPLOT_PRIVATE_NOT_HIDDEN
#include <plplot/plplotP.h>
#endif
#include <string>
#include <iostream>
#include "typedefs.hpp"
#include "gdlexception.hpp"

#ifdef _MSC_VER
#include <algorithm>
#endif

const double MMToINCH = 0.03937;

using namespace std;

// Graphic Structures:
  typedef struct _P_GRAPHICS {
    DLong background;
    DFloat charSize;
    DFloat charThick;
    DLong clip[6];
    DLong color;
    DLong font;
    DLong lineStyle;
    DLong multi[5];
    DLong noClip;
    DLong noErase;
    DLong nsum;
    DFloat position[4];
    DLong psym;
    DFloat region[4];
    DString subTitle;
    DFloat symSize;
    DDouble t[4][4];
    DLong t3d;
    DFloat thick;
    DString title;
    DFloat ticklen;
    DLong channel;
  } pstruct ;

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
    PLFLT ondx; //offset x of box in device coords
    PLFLT ondy; // in y
    PLFLT sndx; //size of box, x , device
    PLFLT sndy;
    PLFLT dx1; //position in device coords (e.g. pixels)
    PLFLT dx2;
    PLFLT dy1;
    PLFLT dy2;
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
    PLINT nbPages; //nx*ny
    PLINT nx;
    PLINT ny;
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
  } gdlCharInfo;

class GDLGStream: public plstream
{
// protected:  
//   void init(); // prevent plstream::init from being called directly
private:
    gdlpage pageLayout;
    gdlbox boxLayout;
    
protected:
  bool valid;
  gdlCharInfo theCurrentChar;
  gdlCharInfo theDefaultChar;
  int gdlDefaultCharInitialized;
  gdlbox theBox;
  gdlpage thePage;
  PLStream* pls;

public:

   GDLGStream( int nx, int ny, const char *driver, const char *file=NULL)
    : plstream( nx, ny, driver, file), valid( true)
  {
    if (!checkPlplotDriver(driver))
      ThrowGDLException(std::string("PLplot installation lacks the requested driver: ") + driver);
    gdlDefaultCharInitialized=0;
    thePage.nbPages=0;
    theBox.initialized=false;
    plgpls( &pls);
  }

  virtual ~GDLGStream()
  {
// 	plend();
  }

// 	void PlstreamInit()
// 	{
// 	    if( !plstreamInitCalled)
// 		{
// 			this->plstream::init();
// 			plstreamInitCalled = true;
// 		}
// 	}

  // uncomment for plplot >= 5.9.10
//   void wid( PLINT w) { this->width( w);}

  static bool checkPlplotDriver(const char *driver)
  {
    int numdevs_plus_one = 64;
#ifdef HAVE_OLDPLPLOT
    char **devlongnames = NULL;
    char **devnames = NULL;
#else
    const char **devlongnames = NULL;
    const char **devnames = NULL;
#endif

    static vector<std::string> devNames;

    // do only once
    if( devNames.empty())
    {
      // acquireing a list of drivers from plPlot
      for (int maxnumdevs = numdevs_plus_one;; numdevs_plus_one = maxnumdevs += 16)
      {
#ifdef HAVE_OLDPLPLOT
        devlongnames = static_cast<char**>(realloc(devlongnames, maxnumdevs * sizeof(char*)));
        devnames = static_cast<char**>(realloc(devnames, maxnumdevs * sizeof(char*)));
#else
        devlongnames = static_cast<const char**>(realloc(devlongnames, maxnumdevs * sizeof(char*)));
        devnames = static_cast<const char**>(realloc(devnames, maxnumdevs * sizeof(char*)));
#endif
        plgDevs(&devlongnames, &devnames, &numdevs_plus_one);
        numdevs_plus_one++;
        if (numdevs_plus_one < maxnumdevs) break;
        else Message("The above PLPlot warning message, if any, can be ignored");
      } 
      free(devlongnames); // we do not need this information

      for( int i = 0; i < numdevs_plus_one - 1; ++i)
        devNames.push_back(std::string(devnames[ i]));
    
      free(devnames);
    }

    // for debug
    std::vector<std::string> devnamesDbg = devNames;

    return std::find( devNames.begin(), devNames.end(), std::string( driver)) != devNames.end();
  }

  static void SetErrorHandlers();

  virtual void Init()=0;

  // called after draw operation
  virtual void Update() {}
  
  virtual void EventHandler() {}

  virtual void GetGeometry( long& xSize, long& ySize, long& xoff, long& yoff);
  virtual void GetWindowSize( long& xSize, long& ySize);
  virtual unsigned long GetWindowDepth () {return 0;}
  virtual DLong GetVisualDepth() {return -1;}
  virtual DString GetVisualName() {return "";}
  virtual bool SetFocus(){return false;}
  virtual bool UnsetFocus(){return false;}
  virtual bool EnableBackingStore(bool enable){return false;}
  virtual bool SetGraphicsFunction(long value ){return false;}
  virtual bool GetWindowPosition(long& xpos, long& ypos ){return false;}
  virtual bool CursorStandard(int cursorNumber){return false;}
  virtual void eop()          { plstream::eop();}
  virtual void SetDoubleBuffering() {}
  virtual void UnSetDoubleBuffering() {}
  virtual bool HasDoubleBuffering() {return false;}
  virtual bool HasSafeDoubleBuffering() {return false;}
  virtual void Raise()         {}
  virtual void Lower()        {}
  virtual void Iconic()        {}
  virtual void DeIconic()        {}
  virtual bool GetGin(PLGraphicsIn *gin, int mode) {return 0;}
  virtual void WarpPointer(DLong x, DLong y){}
  virtual void Flush() {}
  virtual void Clear()         {}
  virtual void Clear( DLong bColor)          {}

  void SetValid( bool v) { valid = v;}
  bool GetValid() { return valid;}
  bool validWorldBox()
  {
    if (((theBox.wx1==0) && (theBox.wx2==0)) 
	|| ((theBox.wy1==0) && (theBox.wy2==0))) return false; else return true;
  }
  bool validNormdBox()
  {
    if (((theBox.nx1==0) && (theBox.nx2==0)) 
	|| ((theBox.ny1==0) && (theBox.ny2==0))) return false; else return true;
  }
  inline PLFLT charScale(){return theCurrentChar.scale;}
  inline PLFLT nCharLength(){return theCurrentChar.ndsx;}
  inline PLFLT nCharHeight(){return theCurrentChar.ndsy;}
  inline PLFLT dCharLength(){return theCurrentChar.dsx;}
  inline PLFLT dCharHeight(){return theCurrentChar.dsy;}
  inline PLFLT wCharLength(){return theCurrentChar.wsx;}
  inline PLFLT wCharHeight(){return theCurrentChar.wsy;}
  inline DDouble mmCharLength(){return theCurrentChar.mmsx;}
  inline DDouble mmCharHeight(){return theCurrentChar.mmsy;}
  inline PLFLT xResolution(){return thePage.xdpmm;}
  inline PLFLT yResolution(){return thePage.ydpmm;}
  inline PLFLT mmxPageSize(){return thePage.xsizemm;} //size in mm
  inline PLFLT mmyPageSize(){return thePage.ysizemm;}
  inline PLFLT boxnXSize(){return theBox.sndx;}
  inline PLFLT boxnYSize(){return theBox.sndy;}
  inline PLFLT xPageSize(){return thePage.length;} //size in units (alternate:{return pls->xlength;})
  inline PLFLT yPageSize(){return thePage.height;} //alternate: {return pls->ylength;}
  inline PLFLT xSubPageSize(){return thePage.subpage.dxsize;} //size in units
  inline PLFLT ySubPageSize(){return thePage.subpage.dysize;}

  // bunch of conversion functions that should be used in the future now that pls is here!
  // (normed) device coords to physical coords (x,y) 0..1 -> 0->32768
  inline PLFLT nd2px(PLFLT x){ return ( pls->phyxmi + pls->phyxlen * x  );}
  inline PLFLT nd2py(PLFLT y){ return ( pls->phyymi + pls->phyylen * y  );}
  inline void norm2physical(PLFLT devx, PLFLT devy, PLFLT &physx, PLFLT &physy)
  { physx=nd2px(devx); physy=nd2py(devy);}
  // (normed) device to mm
  inline PLFLT nd2mx(PLFLT x){ return (PLFLT) ( x * fabs( pls->phyxma - pls->phyxmi ) / pls->xpmm ) ;}
  inline PLFLT nd2my(PLFLT y){ return (PLFLT) ( y * fabs( pls->phyyma - pls->phyymi ) / pls->ypmm ) ;}
  inline void norm2mm(PLFLT devx, PLFLT devy, PLFLT &mmx, PLFLT &mmy)
  { mmx=nd2mx(devx); mmy=nd2my(devy);}
  //(normed) device to world
  inline PLFLT nd2wx(PLFLT x){return (PLFLT) ( (x- pls->wdxoff) / pls->wdxscl );}
  inline PLFLT nd2wy(PLFLT y){return (PLFLT) ( (y- pls->wdyoff) / pls->wdyscl );}
  inline void norm2world(PLFLT devx, PLFLT devy, PLFLT &wx, PLFLT &wy)
  { wx=nd2wx(devx); wy=nd2wy(devy);}
  // (normed) device coords to subpage coords
  inline PLFLT nd2spx(PLFLT x){ return (PLFLT) ( ( x - pls->spdxmi ) / ( pls->spdxma - pls->spdxmi ) ) ;}
  inline PLFLT nd2spy(PLFLT y){ return (PLFLT) ( ( y - pls->spdymi ) / ( pls->spdyma - pls->spdymi ) ) ;}
  inline void norm2subpage(PLFLT devx, PLFLT devy, PLFLT &spx, PLFLT &spy)
  { spx=nd2spx(devx); spy=nd2spy(devy);}

  // millimeters to physical coords (x,y)
  inline PLFLT mm2px(PLFLT x){ return ( pls->phyxmi + pls->xpmm * x  );}
  inline PLFLT mm2py(PLFLT y){ return ( pls->phyymi + pls->ypmm * y  );}
  inline void mm2physical(PLFLT mmx, PLFLT mmy, PLFLT &physx, PLFLT &physy)
  { physx=mm2px(mmx); physy=mm2py(mmy);}
  // mm to (absolute) device
  inline PLFLT mm2adx(PLFLT x){ return (PLFLT) ( ( x * pls->xpmm ) / abs( pls->phyxma - pls->phyxmi )*thePage.length);}
  inline PLFLT mm2ady(PLFLT y){ return (PLFLT) ( ( y * pls->ypmm ) / abs( pls->phyyma - pls->phyymi )*thePage.height);}
  inline void mm2device(PLFLT mmx, PLFLT mmy, PLFLT &devx, PLFLT &devy)
  { devx=mm2adx(mmx); devy=mm2ady(mmy);}
  // mm to (normed) device
  inline PLFLT mm2ndx(PLFLT x){ return (PLFLT) ( ( x * pls->xpmm ) / abs( pls->phyxma - pls->phyxmi ));}
  inline PLFLT mm2ndy(PLFLT y){ return (PLFLT) ( ( y * pls->ypmm ) / abs( pls->phyyma - pls->phyymi ));}
  inline void mm2norm(PLFLT mmx, PLFLT mmy, PLFLT &devx, PLFLT &devy)
  { devx=mm2ndx(mmx); devy=mm2ndy(mmy);}
  // mm to world
  inline PLFLT mm2wx(PLFLT x){ x=mm2ndx(x); return nd2wx(x);}
  inline PLFLT mm2wy(PLFLT y){ y=mm2ndy(y); return nd2wy(y);}
  inline void mm2world(PLFLT mmx, PLFLT mmy, PLFLT &wx, PLFLT &wy)
  { wx=mm2wx(mmx); wy=mm2wy(mmy);}
  // mm to subpage coord
  inline PLFLT mm2spx(PLFLT x){ x=mm2ndx(x); return nd2spx(x);}
  inline PLFLT mm2spy(PLFLT y){ y=mm2ndy(y); return nd2spy(y);}
  inline void mm2subpage(PLFLT mmx, PLFLT mmy, PLFLT &spx, PLFLT &spy)
  { spx=mm2spx(mmx); spy=mm2spy(mmy);}

  // world to physical coords
  inline PLFLT w2px(PLFLT x){ return ( pls->wpxoff + pls->wpxscl * x  );}
  inline PLFLT w2py(PLFLT y){ return ( pls->wpyoff + pls->wpyscl * y  );}
  inline void world2physical(PLFLT wx, PLFLT wy, PLFLT &physx, PLFLT &physy)
  { physx=w2px(wx); physy=w2py(wy);}
  // world to (normed) device
  inline PLFLT w2ndx(PLFLT x){ return (PLFLT) ( pls->wdxoff + pls->wdxscl * x );}
  inline PLFLT w2ndy(PLFLT y){ return (PLFLT) ( pls->wdyoff + pls->wdyscl * y );}
  inline void world2norm(PLFLT wx, PLFLT wy, PLFLT &devx, PLFLT &devy)
  { devx=w2ndx(wx); devy=w2ndy(wy);}
   // world to (absolute) device
  inline PLFLT w2adx(PLFLT x){ return (PLFLT) ( pls->wdxoff + pls->wdxscl * x )*thePage.length;}
  inline PLFLT w2ady(PLFLT y){ return (PLFLT) ( pls->wdyoff + pls->wdyscl * y )*thePage.height;}
  inline void world2device(PLFLT wx, PLFLT wy, PLFLT &devx, PLFLT &devy)
  { devx=w2adx(wx); devy=w2ady(wy);}
  //world to mm
  inline PLFLT w2mmx(PLFLT x){ return (PLFLT) ( pls->wmxoff + pls->wmxscl * x );}
  inline PLFLT w2mmy(PLFLT y){ return (PLFLT) ( pls->wmyoff + pls->wmyscl * y );}
  inline void world2mm(PLFLT wx, PLFLT wy, PLFLT &mmx, PLFLT &mmy)
  { mmx=w2mmx(wx); mmy=w2mmy(wy);}
  //world to subpage coord
  inline PLFLT w2spx(PLFLT x){ x=w2ndx(x) ; return nd2spx(x);}
  inline PLFLT w2spy(PLFLT y){ y=w2ndy(y) ; return nd2spy(y);}
  inline void world2subpage(PLFLT wx, PLFLT wy, PLFLT &spx, PLFLT &spy)
  { spx=w2spx(wx); spy=w2spy(spy);}

  // physical to (normed) device
  inline PLFLT p2ndx(PLFLT x){ return (PLFLT) ( ( x - pls->phyxmi ) / (double) pls->phyxlen );}
  inline PLFLT p2ndy(PLFLT y){ return (PLFLT) ( ( y - pls->phyymi ) / (double) pls->phyylen );}
  inline void physical2device(PLFLT physx, PLFLT physy, PLFLT &devx, PLFLT &devy)
  { devx=p2ndx(physx); devy=p2ndy(physy);}
  //physical to world
  //physical to mm
  //physical to subpage coord

  // subpage coords to (normed) device coords
  inline PLFLT sp2ndx(PLFLT x){ return (PLFLT) ( pls->spdxmi + ( pls->spdxma - pls->spdxmi ) * x ) ;}
  inline PLFLT sp2ndy(PLFLT y){ return (PLFLT) ( pls->spdymi + ( pls->spdyma - pls->spdymi ) * y ) ;}
  inline void subpage2norm(PLFLT spx, PLFLT spy, PLFLT &devx, PLFLT &devy)
  { devx=sp2ndx(spx); devy=sp2ndy(spy);}
  //subpage to world
  //subpage to mm
  //subpage to physical

#if PLPLOT_PRIVATE_NOT_HIDDEN
  //use simple internal function
  PLFLT gdlGetmmStringLength(const char *string)
  {
    return plstrl(string);
  }
#else
#ifdef PLPLOT_HAS_LEGEND
  //use trick to extract desired value hidden in pllegend!
  PLFLT gdlGetmmStringLength(const char *string)
  {
    if ( pls->has_string_length )
    {
        pls->get_string_length = 1;
        c_plmtex( "t", 0.0, 0.0, 0.0, string );
        pls->get_string_length = 0;
        return (PLFLT) mm2ndx(pls->string_length);
    }
    //else use only possibility without using Private function plstrl(): pllegend!
    PLFLT text_scale = pls->chrht / pls->chrdef;
    PLFLT xwmin_save, xwmax_save, ywmin_save, ywmax_save;
    plgvpw(&xwmin_save, &xwmax_save, &ywmin_save, &ywmax_save);
    PLFLT xdmin_save, xdmax_save, ydmin_save, ydmax_save;
    xdmin_save = ( pls->vpdxmi - pls->spdxmi ) / ( pls->spdxma - pls->spdxmi );
    xdmax_save = ( pls->vpdxma - pls->spdxmi ) / ( pls->spdxma - pls->spdxmi );
    ydmin_save = ( pls->vpdymi - pls->spdymi ) / ( pls->spdyma - pls->spdymi );
    ydmax_save = ( pls->vpdyma - pls->spdymi ) / ( pls->spdyma - pls->spdymi );
    PLFLT mxmin, mxmax, mymin, mymax;
    plgspa( &mxmin, &mxmax, &mymin, &mymax );
    PLFLT x_subpage_per_mm, y_subpage_per_mm;
    x_subpage_per_mm = 1. / ( mxmax - mxmin );
    y_subpage_per_mm = 1. / ( mymax - mymin );
    PLFLT def_mm, charheight_mm;
    plgchr( &def_mm, &charheight_mm );
    PLFLT character_width=charheight_mm/(mymax-mymin );

    plvpor( 0., 1., 0., 1. );
    plwind( 0., 1., 0., 1. );
    PLFLT xdmin_adopted, xdmax_adopted, ydmin_adopted, ydmax_adopted;
    xdmin_adopted = ( pls->vpdxmi - pls->spdxmi ) / ( pls->spdxma - pls->spdxmi );
    xdmax_adopted = ( pls->vpdxma - pls->spdxmi ) / ( pls->spdxma - pls->spdxmi );
    ydmin_adopted = ( pls->vpdymi - pls->spdymi ) / ( pls->spdyma - pls->spdymi );
    ydmax_adopted = ( pls->vpdyma - pls->spdymi ) / ( pls->spdyma - pls->spdymi );
// we have all info, give back box values:
    plvpor( xdmin_save, xdmax_save, ydmin_save, ydmax_save );
    plwind( xwmin_save, xwmax_save, ywmin_save, ywmax_save );
//call pllegend (outside plot)
    PLINT opt_array[1];
    PLINT text_colors[1];
    PLINT line_colors[1];
    PLINT line_styles[1];
    PLINT line_widths[1];
    PLFLT legend_width, legend_height;
    PLFLT plot_width=1.0;
    const char *text[1];
    opt_array[0]   = 0;
    text_colors[0] = 0;
    line_colors[0] = 0;
    line_styles[0] = 1;
    line_widths[0] = 1;
    text[0]=string;
    pllegend (&legend_width , &legend_height ,
            PL_LEGEND_NONE,
            PL_POSITION_VIEWPORT|PL_POSITION_TOP|PL_POSITION_OUTSIDE,
	        1.0 , -0.1 , plot_width , //moved the position farther away since it shows up in postscripts;
            0 , 0 , 1 ,
            1 , 1 ,
            1 , opt_array ,
            0.0 , text_scale , 0.0 , 0.0 , text_colors ,
            text , NULL , NULL , NULL , NULL , NULL ,
            NULL , NULL , NULL , NULL , NULL , NULL);
//with these values: legend_width = 2. * 0.4 *character_width + text_width ;
    //invert pllegend work:
#define subpage_to_adopted_x( nx )    ( ( nx - xdmin_adopted ) / ( ( xdmax_adopted ) - ( xdmin_adopted ) ) )
#define adopted_to_subpage_x( nx )    ( ( xdmin_adopted ) + ( nx ) * ( ( xdmax_adopted ) - ( xdmin_adopted ) ) )
    PLFLT tempsize=adopted_to_subpage_x(legend_width+subpage_to_adopted_x( 0. ));
    tempsize=tempsize-0.8*character_width-adopted_to_subpage_x(plot_width) + adopted_to_subpage_x( 0. );
    return tempsize/x_subpage_per_mm;
  }
#else //we are desperate at this point since the value returned will be false since fonts are proportional fonts.
   PLFLT gdlGetmmStringLength(const char *string)
  {
    return (strlen(string))*theCurrentChar.mmsx;
  }
#endif
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

  void SaveLayout()
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"SaveLayout():\n");
    pageLayout.nbPages=thePage.nbPages;
    pageLayout.nx=thePage.nx;
    pageLayout.ny=thePage.ny;
    pageLayout.curPage=thePage.curPage;
    pageLayout.length=thePage.length;
    pageLayout.height=thePage.height;
    pageLayout.xsizemm=thePage.xsizemm;
    pageLayout.ysizemm=thePage.ysizemm;
    pageLayout.plxoff=thePage.plxoff;
    pageLayout.plyoff=thePage.plyoff;

    boxLayout.nx1=theBox.nx1;
    boxLayout.nx2=theBox.nx2;
    boxLayout.ny1=theBox.ny1;
    boxLayout.ny2=theBox.ny2;
    boxLayout.ndx1=theBox.ndx1;
    boxLayout.ndx2=theBox.ndx2;
    boxLayout.ndy1=theBox.ndy1;
    boxLayout.ndy2=theBox.ndy2;
    boxLayout.ondx=theBox.ondx;
    boxLayout.ondy=theBox.ondy;
    boxLayout.sndx=theBox.sndx;
    boxLayout.sndy=theBox.sndy;
    boxLayout.dx1=theBox.dx1;
    boxLayout.dx2=theBox.dx2;
    boxLayout.dy1=theBox.dy1;
    boxLayout.dy2=theBox.dy2;
    boxLayout.wx1=theBox.wx1;
    boxLayout.wx2=theBox.wx2;
    boxLayout.wy1=theBox.wy1;
    boxLayout.wy2=theBox.wy2;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"saving box [%f,%f,%f,%f] at [%f,%f,%f,%f] in subpage %d of %dx%d (device coords [%f,%f,%f,%f]\n",boxLayout.wx1,boxLayout.wy1,boxLayout.wx2,boxLayout.wy2,boxLayout.nx1,boxLayout.ny1,boxLayout.nx2,boxLayout.ny2,pageLayout.curPage,pageLayout.nx,pageLayout.ny,boxLayout.dx1,boxLayout.dy1,boxLayout.dx2,boxLayout.dy2);
  }

  void RestoreLayout()
  {
      ssub(pageLayout.nx,pageLayout.ny);
      adv(pageLayout.curPage);
      vpor(boxLayout.nx1,boxLayout.nx2,boxLayout.ny1,boxLayout.ny2);
      wind(boxLayout.wx1,boxLayout.wx2,boxLayout.wy1,boxLayout.wy2);
  }

  void OnePageSaveLayout()
  {
      SaveLayout();
      NoSub();
  }

  bool updatePageInfo()
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"updatePageInfo():\n");
    if (thePage.nbPages==0) {if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"            FAILED\n");return false;}
    long xsize,ysize,xoff,yoff;
    GetGeometry(xsize,ysize,xoff,yoff);
    thePage.length=xsize;
    thePage.height=ysize;
    thePage.plxoff=xoff;
    thePage.plyoff=yoff;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"             %fx%f device units.\n",thePage.length, thePage.height);
    return true;
  }

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

  inline void WorldToNorm(PLFLT worldx, PLFLT worldy, PLFLT &normx, PLFLT &normy)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"WorldToNormedDevice()\n");
    DDouble s1,s2;
    s1=(theBox.nx2-theBox.nx1)/(theBox.wx2-theBox.wx1);
    s2=theBox.nx1;
    normx=s1*(worldx-theBox.wx1)+s2;
    s1=(theBox.ny2-theBox.ny1)/(theBox.wy2-theBox.wy1);
    s2=theBox.ny1;
    normy=s1*(worldy-theBox.wy1)+s2;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"     input [%f,%f] output [%f,%f]\n", worldx, worldy, normx, normy);
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
  
  //  void Clear();
  void Color( ULong c, DLong decomposed=0, UInt ix=1);
  void Background( ULong c, DLong decomposed=0);

  void DefaultCharSize();
  void NextPlot( bool erase=true); // handles multi plots

  void NoSub(); // no subwindows (/NORM, /DEVICE)

  void CurrentCharSize(PLFLT scale)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"CurrentCharsize()\n");
    if (gdlDefaultCharInitialized==0)
    {
        if (updatePageInfo()==true)
        {
        GetPlplotDefaultCharSize();
        }
    }
    theCurrentChar.scale=scale;
    theCurrentChar.ndsx=scale*theDefaultChar.ndsx;
    theCurrentChar.ndsy=scale*theDefaultChar.ndsy;
    theCurrentChar.dsx=scale*theDefaultChar.dsx;
    theCurrentChar.dsy=scale*theDefaultChar.dsy;
    theCurrentChar.mmsx=scale*theDefaultChar.mmsx;
    theCurrentChar.mmsy=scale*theDefaultChar.mmsy;
    theCurrentChar.wsx=scale*theDefaultChar.wsx;
    theCurrentChar.wsy=scale*theDefaultChar.wsy;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"            sized by %f is %fx%f mm or %fx%f device or %fx%f world\n",scale,theCurrentChar.mmsx,theCurrentChar.mmsy,theCurrentChar.dsx,theCurrentChar.dsy,theCurrentChar.wsx, theCurrentChar.wsy);
  }

  void UpdateCurrentCharWorldSize()
  {
    PLFLT x,y,dx,dy;
    DeviceToWorld(0,0,x,y);
    DeviceToWorld(theDefaultChar.dsx,theDefaultChar.dsy, dx, dy);
    theDefaultChar.wsx=abs(dx-x);
    theDefaultChar.wsy=abs(dy-y);
    theCurrentChar.wsx=theCurrentChar.scale*theDefaultChar.wsx;
    theCurrentChar.wsy=theCurrentChar.scale*theDefaultChar.wsy;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"UpdateCurrentCharWorldSize(%f,%f)\n",
                                    theCurrentChar.wsx,theCurrentChar.wsy);
  }
  
  void GetPlplotDefaultCharSize()
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GetPlPlotDefaultCharsize()\n");
    if (thePage.nbPages==0)   {return;}
    //dimensions in normalized, device and millimetres
    if (gdlDefaultCharInitialized==1) {if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"     Already initialized\n"); return;}

    PLFLT nxmin, nxmax, nymin, nymax, wxmin, wxmax, wymin, wymax;
    plstream::gvpd(nxmin, nxmax, nymin, nymax); //save norm of current box
    if((nxmin==0.0&&nxmax==0.0)||(nymin==0.0&&nymax==0.0)) //if not initialized, set normalized mode
    {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"          Warning: initializing viewport\n");
      plstream::vpor(0, 1, 0, 1);
      plstream::gvpd(nxmin, nxmax, nymin, nymax);
      plstream::wind(0.0,1.0,0.0,1.0);
    }
    plstream::gvpw(wxmin, wxmax, wymin, wymax); //save world of current box
    PLFLT vpXmin, vpXmax, vpYmin, vpYmax;
    PLFLT vpXmin2, vpXmax2, vpYmin2, vpYmax2;
    plstream::vpor(0, 1, 0, 1);
    plstream::wind(0.0,1.0,0.0,1.0);
    plstream::gvpd(vpXmin, vpXmax, vpYmin, vpYmax);
    plstream::vsta();
    plstream::gvpd(vpXmin2, vpXmax2, vpYmin2, vpYmax2);
    theDefaultChar.ndsx=0.5*((vpXmin2-vpXmin)/8.0+(vpXmax-vpXmax2)/5.0);
    theDefaultChar.ndsy=0.5*((vpYmin2-vpYmin)/5.0+(vpYmax-vpYmax2)/5.0);
    theDefaultChar.dsx=0.5*((vpXmin2-vpXmin)/8.0+(vpXmax-vpXmax2)/5.0)*thePage.length;
    theDefaultChar.dsy=0.5*((vpYmin2-vpYmin)/5.0+(vpYmax-vpYmax2)/5.0)*thePage.height;
    plstream::vpor(nxmin, nxmax, nymin, nymax); //restore norm of current box
    plstream::wind(wxmin, wxmax, wymin, wymax); //restore world of current box
    PLFLT defhmm, scalhmm;
    plgchr(&defhmm, &scalhmm); // height of a letter in millimetres
    theDefaultChar.mmsy=scalhmm;
    theDefaultChar.mmsx=theDefaultChar.ndsx/theDefaultChar.ndsy*scalhmm;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"             %fx%f(mm)\n",theDefaultChar.mmsx,theDefaultChar.mmsy);
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"             %fx%f(norm)\n",theDefaultChar.ndsx,theDefaultChar.ndsy);
    gdlDefaultCharInitialized=1;
  }
  // SA: overloading plplot methods in order to handle IDL-plplot extended
  // text formating syntax conversion
  bool TranslateFormatCodes(const char *text, std::string &out);
  void mtex( const char *side, PLFLT disp, PLFLT pos, PLFLT just,
                         const char *text);
  void ptex( PLFLT x, PLFLT y, PLFLT dx, PLFLT dy, PLFLT just,
                         const char *text);
  void schr( PLFLT def, PLFLT scale );
  void sizeChar(PLFLT scale);
  void vpor( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );
//  void gvpd( PLFLT& xmin, PLFLT& xmax, PLFLT& ymin, PLFLT& ymax );
  void wind( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );
  void ssub( PLINT nx, PLINT ny);
  void adv(PLINT page);
  void gpage(PLFLT& xp, PLFLT& yp, PLINT& xleng, PLINT& yleng,
                PLINT& xoff, PLINT& yoff)
  {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"gpage() [%f,%f]\n",xp,yp);
    if(updatePageInfo()==true)
    {
        xp=thePage.xdpmm/MMToINCH;
        yp=thePage.ydpmm/MMToINCH;
        xleng=(PLINT)thePage.length;
        yleng=(PLINT)thePage.height;
        xoff=(PLINT)thePage.plxoff;
        yoff=(PLINT)thePage.plyoff;
    }
  }

  inline void syncPageInfo()
  {
      if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"SyncPageInfo()\n");
      PLINT level;
      plstream::glevel(level);
      if (level>1 && thePage.nbPages!=0) //we need to have a vpor defined, and a page!
      {
        PLFLT bxsize_mm, bysize_mm, offx_mm, offy_mm;
        PLFLT xmin,ymin,xmax,ymax;
        plstream::gspa(xmin,xmax,ymin,ymax); //subpage in mm
        bxsize_mm=xmax-xmin;
        bysize_mm=ymax-ymin;
        offx_mm=xmin;
        offy_mm=ymin;
        if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"         gspa returned size[%f,%f] at offset [%f,%f] (mm) for subpage %d of %dx%d subpages\n",bxsize_mm,bysize_mm,offx_mm,offy_mm,thePage.curPage,thePage.nx,thePage.ny);
        //we can derive the dpm in x and y which converts mm to device coords:
        thePage.xdpmm=abs(thePage.length/(thePage.nx*bxsize_mm));
        thePage.ydpmm=abs(thePage.height/(thePage.ny*bysize_mm));
        //and the page width and height in mm:
        thePage.xsizemm=thePage.length/thePage.xdpmm;
        thePage.ysizemm=thePage.height/thePage.ydpmm;
        if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"         device resolution [%f,%f]mm^-1, device size [%f,%f], [%f,%f] mm\n",
                thePage.xdpmm,thePage.ydpmm,thePage.length,thePage.height,thePage.xsizemm,thePage.ysizemm);
        thePage.subpage.dxoff=offx_mm*thePage.xdpmm;
        thePage.subpage.dyoff=offy_mm*thePage.ydpmm;
        thePage.subpage.dxsize=bxsize_mm*thePage.xdpmm;
        thePage.subpage.dysize=bysize_mm*thePage.ydpmm;
        if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"         subpage is %fx%f at [%f,%f] device units\n",
                thePage.subpage.dxsize,thePage.subpage.dysize,thePage.subpage.dxoff,thePage.subpage.dyoff);

      } else         if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"       WARNING: not initalized\n");
  }

  inline void updateBoxDeviceCoords()
  {
      if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"updateBoxDeviceCoords()\n");
    // world coordinates of current subpage boundaries and page boundaries
    NormedDeviceToWorld(0.0, 0.0,theBox.pageWorldCoordinates[0],theBox.pageWorldCoordinates[2]);
    NormedDeviceToWorld(1.0, 1.0,theBox.pageWorldCoordinates[1],theBox.pageWorldCoordinates[3]);
    NormToWorld(0.0, 0.0,theBox.subPageWorldCoordinates[0],theBox.subPageWorldCoordinates[2]);
    NormToWorld(1.0, 1.0,theBox.subPageWorldCoordinates[1],theBox.subPageWorldCoordinates[3]);
    NormToDevice(theBox.nx1,theBox.ny1,theBox.dx1,theBox.dy1);
    NormToDevice(theBox.nx2,theBox.ny2,theBox.dx2,theBox.dy2);
  }
};

#endif
