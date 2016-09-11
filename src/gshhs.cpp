/***************************************************************************
                          gshhs.cpp  -  GSHHS-related routines
                             -------------------
    begin                : Apr 18 2010
    copyright            : (C) 2010 by Sylwester Arabas 
                           based on the GPL-licensed code (see note below)
                           by P. Wessel and W. H. F. Smith
    email                : slayoo@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
// default: assume we have GSHHS
#define USE_GSHHS 1
#endif

#if defined(USE_LIBPROJ4)||defined(USE_LIBPROJ4_NEW)


#include "includefirst.hpp"
#include "plotting.hpp"
#include "math_utl.hpp"

#include "initsysvar.hpp"

#ifdef USE_GSHHS
#include "gshhs.h"
#endif

#define DPI (double)(4*atan(1.0))
#define DEGTORAD DPI/180.0

namespace lib {

using namespace std;

class map_continents_call: public plotting_routine_call {
bool doT3d;
DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd;
    DDouble zValue;
    DDoubleGDL* plplot3d;
    Guard<BaseGDL> plplot3d_guard;
    ORIENTATION3D axisExchangeCode;
    DDouble az, alt, ay, scale;
private:

  bool handle_args( EnvT* e ) {
      //T3D
      doT3d=(e->KeywordSet("T3D") || T3Denabled());

    return true;
  }

private:

  void old_body( EnvT* e, GDLGStream * actStream ) {
#ifndef USE_GSHHS
    e->Throw( "GDL was compiled without support for GSHHS" );
#else
    static struct GSHHS_POINT p;
    bool externalMap;
    bool mapSet;
    // Get MATRIX 
    DStructGDL* map = GetMapAsMapStructureKeyword( e, externalMap);
    if (!externalMap){
      get_mapset(mapSet);
      if (!mapSet) e->Throw( "Map transform not established." );
    }
    ref = map_init( map );
    if ( ref == NULL ) {
      e->Throw( "Projection initialization failed." );
    }
#ifdef USE_LIBPROJ4
    LPTYPE idata;
    XYTYPE odata;
#endif

    static int continentsIx = e->KeywordIx( "CONTINENTS" );
    bool kw_continents = true; //by default, plot continents even if no kw
    static int riversIx = e->KeywordIx( "RIVERS" );
    bool kw_rivers = e->KeywordSet( riversIx );
    if ( kw_rivers ) kw_continents = e->KeywordSet( continentsIx );
    static int countriesIx = e->KeywordIx( "COUNTRIES" );
    bool kw_countries = e->KeywordSet( countriesIx );
    if ( kw_countries ) kw_continents = e->KeywordSet( continentsIx );
    static int usaIx = e->KeywordIx( "USA" );
    bool kw_usa = e->KeywordSet( usaIx );
    if ( kw_usa ) kw_continents = e->KeywordSet( continentsIx );
    static int coastsIx = e->KeywordIx( "COASTS" );
    bool kw_coasts = e->KeywordSet( coastsIx );
    if ( kw_coasts ) kw_continents = e->KeywordSet( continentsIx );

    static int hiresIx = e->KeywordIx( "HIRES" );
    bool kw_hires = e->KeywordSet( hiresIx );
    static int fillIx = e->KeywordIx( "FILL_CONTINENTS" );
    bool kw_fill = e->KeywordSet( fillIx );
    if ( kw_fill ) kw_continents = true;
    
    string dir = SysVar::GshhsDir();

    enum set {
      continents, countries, rivers, coasts
    };
    string sufix = kw_hires ? "_h.b" : "_c.b"; //easy: should improve resolution based on actual size of exposed area and screen resolution!!
                                               // TODO: use mmap!
    vector<string> files( 4 );
    files[continents] = dir + "gshhs" + sufix;
    files[countries] = dir + "wdb_borders" + sufix;
    files[rivers] = dir + "wdb_rivers" + sufix;
    files[coasts] = dir + "gshhs" + sufix;
    bool do_fill = false;
    
          //get DATA limits (not necessary CRANGE, see AXIS / SAVE behaviour!)
      GetCurrentUserLimits(actStream, xStart, xEnd, yStart, yEnd);
      // get !Z.CRANGE
      gdlGetCurrentAxisRange("Z", zStart, zEnd);
      
if ( doT3d ) { //if X,Y and Z are passed, we will use !P.T and not our plplot "interpretation" of !P.T
                               //if the x and y scaling is OK, using !P.T directly permits to use other projections
                               //than those used implicitly by plplot. See @showhaus example for *DL
        // case where we project 2D data on 3D: use plplot-like matrix.
        static DDouble x0,y0,xs,ys; //conversion to normalized coords

          x0=-xStart;
          y0=-yStart;
          xs=xEnd-xStart;xs=1.0/xs;
          ys=yEnd-yStart;ys=1.0/ys;
        // here zvalue here is zcoord on Z axis, to be scaled between 0 and 1 for compatibility with call of gdlConvertT3DMatrixToPlplotRotationMatrix()
        zValue /= (zEnd - zStart);
        plplot3d = gdlConvertT3DMatrixToPlplotRotationMatrix(zValue, az, alt, ay, scale, axisExchangeCode);
        Data3d.zValue = zValue;
        Data3d.Matrix = plplot3d; //try to change for !P.T in future?
        Data3d.x0 = x0;
        Data3d.y0 = y0;
        Data3d.xs = xs;
        Data3d.ys = ys;
        switch (axisExchangeCode) {
          case NORMAL3D: //X->X Y->Y plane XY
            Data3d.code = code012;
            break;
          case XY: // X->Y Y->X plane XY
            Data3d.code = code102;
            break;
          case XZ: // Y->Y X->Z plane YZ
            Data3d.code = code210;
            break;
          case YZ: // X->X Y->Z plane XZ
            Data3d.code = code021;
            break;
          case XZXY: //X->Y Y->Z plane YZ
            Data3d.code = code120;
            break;
          case XZYZ: //X->Z Y->X plane XZ
            Data3d.code = code201;
            break;
        }
        actStream->stransform(gdl3dTo2dTransform, &Data3d);
      }
else
{
    actStream->OnePageSaveLayout(); // do not use in projections
  
    actStream->NoSub( );

    DDouble *sx, *sy;
    GetSFromPlotStructs( &sx, &sy );

    DFloat *wx, *wy;
    GetWFromPlotStructs( &wx, &wy );

    DDouble pxStart, pxEnd, pyStart, pyEnd;
    DataCoordLimits( sx, sy, wx, wy, &pxStart, &pxEnd, &pyStart, &pyEnd, true );

    actStream->vpor( wx[0], wx[1], wy[0], wy[1] );
    actStream->wind( pxStart, pxEnd, pyStart, pyEnd );
}

    gdlSetGraphicsForegroundColorFromKw( e, actStream );

    for ( int i = 0; i < files.size( ); ++i ) {

      if ( i == countries && !kw_countries && !kw_usa ) continue;
      if ( i == rivers && !kw_rivers ) continue;
      if ( i == continents && !kw_continents ) continue;
      if ( i == coasts && !kw_coasts ) continue;
      do_fill = (kw_fill && i == continents);

      // TODO: coasts, continents

      FILE *fp = NULL;
      if ( (fp = fopen( files[i].c_str( ), "rb" )) == NULL ) {
       actStream->RestoreLayout();
       e->Throw( "GSHHS: Could not open file: " + files[i] );
      }
      FILEGuard fpGuard( fp, fclose );

      struct GSHHS h;
      int n_read = fread( (void *) &h, (size_t)sizeof (struct GSHHS), (size_t) 1, fp );
      int version = (h.flag >> 8) & 255;
      int flip = (version != GSHHS_DATA_RELEASE); /* Take as sign that byte-swapping is needed */

      int max_east = 270000000;
      while ( n_read == 1 ) {
        if ( flip ) {
          h.id = swabi4( (unsigned int) h.id );
          h.n = swabi4( (unsigned int) h.n );
          h.west = swabi4( (unsigned int) h.west );
          h.east = swabi4( (unsigned int) h.east );
          h.south = swabi4( (unsigned int) h.south );
          h.north = swabi4( (unsigned int) h.north );
          h.area = swabi4( (unsigned int) h.area );
          h.area_full = swabi4( (unsigned int) h.area_full );
          h.flag = swabi4( (unsigned int) h.flag );
          h.container = swabi4( (unsigned int) h.container );
          h.ancestor = swabi4( (unsigned int) h.ancestor );
        }

        int level = h.flag & 255; // Level is 1-4 
        version = (h.flag >> 8) & 255; // Version is 1-7 
        int greenwich = (h.flag >> 16) & 1; // Greenwich is 0 or 1 
        int src = (h.flag >> 24) & 1; // Greenwich is 0 (WDBII) or 1 (WVS) 
        int river = (h.flag >> 25) & 1; // River is 0 (not river) or 1 (is river) 
        double ww = h.west * GSHHS_SCL; // Convert from microdegrees to degrees 
        double ee = h.east * GSHHS_SCL;
        double ss = h.south * GSHHS_SCL;
        double nn = h.north * GSHHS_SCL;
        char source = (src == 1) ? 'W' : 'C'; // Either WVS or CIA (WDBII) pedigree 
        if ( river ) source = tolower( (int) source ); // Lower case c means river-lake 
        int line = (h.area) ? 0 : 1; // Either Polygon (0) or Line (1) (if no area) 


        double area = 0.1 * h.area; // Now im km^2 
        double f_area = 0.1 * h.area_full; // Now im km^2
        bool skip = false;
        if ( i == continents && line ) skip = true;
        if ( do_fill && line ) skip = true;
        if ( i == continents && river ) skip = true;
        if ( i == continents && (level > 1) ) skip = true;
        if ( i == countries && (level > 2) ) skip = true;
        if ( i == countries && !kw_usa && (level > 1) ) skip = true;
        if ( i == countries && kw_usa && !kw_countries && (level < 2) ) skip = true;
        if ( i == coasts && level > 2 ) skip = true;
        if ( i == coasts && !kw_hires && area < 100.0 ) skip = true;
        if ( i == rivers && (level > 4) ) skip = true;
        if ( skip ) {
          if ( fseek( fp, (long) (h.n * sizeof (struct GSHHS_POINT)), SEEK_CUR ) != 0 ) {
            actStream->RestoreLayout();
            e->Throw( "Error reading file" + files[i] + " for " + (line ? "line" : "polygon") + i2s( h.id ) );
          }
        } else {
          DDoubleGDL *lons, *lats;
          lons = new DDoubleGDL( h.n, BaseGDL::NOZERO );
          lats = new DDoubleGDL( h.n, BaseGDL::NOZERO );

          for ( SizeT k = 0; k < h.n; k++ ) {
            if ( fread( (void *) &p, (size_t)sizeof (struct GSHHS_POINT), (size_t) 1, fp ) != 1 ) {
              actStream->RestoreLayout();
              e->Throw( "Error reading file" + files[i] + " for " + (line ? "line" : "polygon")  + i2s( h.id ) + ", point " + i2s( k ) );
            }
            // byte order
            if ( flip ) {
              p.x = swabi4( (unsigned int) p.x );
              p.y = swabi4( (unsigned int) p.y );
            }
            (*lons)[k] = p.x * GSHHS_SCL;
            if ( (greenwich && p.x > max_east) || (h.west > 180000000) ) ( *lons )[k] -= 360.0;
            (*lats)[k] = p.y * GSHHS_SCL;
          }

          GDLgrProjectedPolygonPlot(actStream, ref, map, lons, lats, false, do_fill, NULL);

          max_east = 180000000; /* Only Eurasia needs 270 */
          GDLDelete( lons );
          GDLDelete( lats );
        }
        n_read = fread( (void *) &h, (size_t)sizeof (struct GSHHS), (size_t) 1, fp );
      }
    }

    actStream->lsty( 1 ); //reset linestyle
//    // reset the viewport and world coordinates to the original values
    if (!doT3d) actStream->RestoreLayout();
#endif
  } // old_body

private:

  void call_plplot( EnvT* e, GDLGStream * actStream ) {
  }

private:

  void post_call( EnvT* e, GDLGStream * actStream ) {
      if (doT3d)
      {
        plplot3d_guard.Reset(plplot3d);
        actStream->stransform(NULL,NULL);
      }
  }

}; // class definition



  void map_continents( EnvT* e ) {
#if defined(USE_LIBPROJ4) || defined(USE_LIBPROJ4_NEW)
    map_continents_call map_continents;
    map_continents.call( e, 0 );
#else
    Throw("GDL was compiled without Proj4 ou LibProj4 ... no cartography !");
#endif
  }

} // namespace

#endif
