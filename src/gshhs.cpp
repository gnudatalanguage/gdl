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

#include "includefirst.hpp"
#include "gshhs.hpp"
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
private:

  bool handle_args( EnvT* e ) {
    return true;
  }

private:

  void old_body( EnvT* e, GDLGStream * actStream ) {
#ifndef USE_GSHHS
    e->Throw( "GDL was compiled without support for GSHHS" );
#else
    static struct POINT p;
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


    // SA: the code below is based on the gshhs.c by Paul Wessel
    // here's the original copyright notice:

    /*
     *	Copyright (c) 1996-2009 by P. Wessel and W. H. F. Smith
     *	See COPYING file for copying and redistribution conditions.
     *
     * PROGRAM:	gshhs.c
     * AUTHOR:	Paul Wessel (pwessel@hawaii.edu)
     * CREATED:	JAN. 28, 1996
     * PURPOSE:	To extract ASCII data from the binary GSHHS shoreline data
     *		as described in the 1996 Wessel & Smith JGR Data Analysis Note.
     * VERSION:	1.1 (Byte flipping added)
     *		1.2 18-MAY-1999:
     *		   Explicit binary open for DOS systems
     *		   POSIX.1 compliant
     *		1.3 08-NOV-1999: Released under GNU GPL
     *		1.4 05-SEPT-2000: Made a GMT supplement; FLIP no longer needed
     *		1.5 14-SEPT-2004: Updated to deal with latest GSHHS database (1.3)
     *		1.6 02-MAY-2006: Updated to deal with latest GSHHS database (1.4)
     *		1.7 11-NOV-2006: Fixed bug in computing level (&& vs &)
     *		1.8 31-MAR-2007: Updated to deal with latest GSHHS database (1.5)
     *		1.9 27-AUG-2007: Handle line data as well as polygon data
     *		1.10 15-FEB-2008: Updated to deal with latest GSHHS database (1.6)
     *		1.12 15-JUN-2009: Now contains information on container polygon,
     *				the polygons ancestor in the full resolution, and
     *				a flag to tell if a lake is a riverlake.
     *				Updated to deal with latest GSHHS database (2.0)
     *
     *	This program is free software; you can redistribute it and/or modify
     *	it under the terms of the GNU General Public License as published by
     *	the Free Software Foundation; version 2 of the License.
     *
     *	This program is distributed in the hope that it will be useful,
     *	but WITHOUT ANY WARRANTY; without even the implied warranty of
     *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     *	GNU General Public License for more details.
     *
     *	Contact info: www.soest.hawaii.edu/pwessel 
     */
    
    string dir = SysVar::GshhsDir();

    cout << dir << endl;

    //    string dir = string( GSHHS_DATA_DIR ); // + "/../gshhs/";

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

    actStream->OnePageSaveLayout(); // one page
//    
    gdlSetGraphicsForegroundColorFromKw( e, actStream );
    actStream->NoSub( );

    DDouble *sx, *sy;
    GetSFromPlotStructs( &sx, &sy );

    DFloat *wx, *wy;
    GetWFromPlotStructs( &wx, &wy );

    DDouble xStart, xEnd, yStart, yEnd;
    DataCoordLimits( sx, sy, wx, wy, &xStart, &xEnd, &yStart, &yEnd, true );

    actStream->vpor( wx[0], wx[1], wy[0], wy[1] );
    actStream->wind( xStart, xEnd, yStart, yEnd );

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
          if ( fseek( fp, (long) (h.n * sizeof (struct POINT)), SEEK_CUR ) != 0 ) {
            actStream->RestoreLayout();
            e->Throw( "Error reading file" + files[i] + " for " + (line ? "line" : "polygon") + i2s( h.id ) );
          }
        } else {
          DDoubleGDL *lons, *lats;
          lons = new DDoubleGDL( h.n, BaseGDL::NOZERO );
          lats = new DDoubleGDL( h.n, BaseGDL::NOZERO );

          for ( SizeT k = 0; k < h.n; k++ ) {
            if ( fread( (void *) &p, (size_t)sizeof (struct POINT), (size_t) 1, fp ) != 1 ) {
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

          GDLgrProjectedPolygonPlot(e, actStream, ref, map, lons, lats, false, do_fill, NULL);

          max_east = 180000000; /* Only Eurasia needs 270 */
          GDLDelete( lons );
          GDLDelete( lats );
        }
        n_read = fread( (void *) &h, (size_t)sizeof (struct GSHHS), (size_t) 1, fp );
      }
    }

    actStream->lsty( 1 ); //reset linestyle
//    // reset the viewport and world coordinates to the original values
    actStream->RestoreLayout();
#endif
  } // old_body

private:

  void call_plplot( EnvT* e, GDLGStream * actStream ) {
  }

private:

  void post_call( EnvT* e, GDLGStream * actStream ) {
  }

}; // class definition


  BaseGDL* gshhg_exists( EnvT* e )
  {
#ifdef USE_GSHHS
    //    e->Message( "GDL was compiled with support for GSHHG" );
    return new DIntGDL(1);
#else
    //e->Message( "GDL was compiled without support for GSHHG" );
    return new DIntGDL(0);
#endif
  }
  
  BaseGDL* proj4_exists( EnvT* e )
  {
#if defined(USE_LIBPROJ4)
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }
  
  BaseGDL* proj4new_exists( EnvT* e )
  {
#if defined(USE_LIBPROJ4_NEW)
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  void map_continents( EnvT* e ) {
    map_continents_call map_continents;
    map_continents.call( e, 0 );
  }

} // namespace

