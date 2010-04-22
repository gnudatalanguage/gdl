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

#ifdef USE_GSHHS
#  include <gshhs.h>
#endif

namespace lib {

  using namespace std;

  void map_continents(EnvT* e)
  {
#ifndef USE_GSHHS
    e->Throw("GDL was compiled without support for GSHHS");
#else

    // mapping init
    bool mapSet = false;
#ifdef USE_LIBPROJ4
    LPTYPE idata;
    XYTYPE odata;
    get_mapset(mapSet);
    if (mapSet) 
    {
      ref = map_init();
      if (ref == NULL) e->Throw( "Projection initialization failed.");
    }
#endif
    if (!mapSet) 
      e->Throw("Map transform not established (MAP_SET).");

    // plotting init
    GDLGStream* actStream = GetPlotStream(e);

    gkw_color(e, actStream);

    actStream->NoSub();

    DDouble *sx, *sy; 
    GetSFromPlotStructs(&sx, &sy); 
    
    DFloat *wx, *wy; 
    GetWFromPlotStructs(&wx, &wy); 

    DDouble xStart, xEnd, yStart, yEnd;
    DataCoordLimits(sx, sy, wx, wy, &xStart, &xEnd, &yStart, &yEnd);

    actStream->vpor(wx[0], wx[1], wy[0], wy[1]);
    actStream->wind( xStart, xEnd, yStart, yEnd);

    // GDL stuff
    static int riversIx = e->KeywordIx("RIVERS");
    bool kw_rivers = e->KeywordSet(riversIx);
    static int countriesIx = e->KeywordIx("COUNTRIES");
    bool kw_countries = e->KeywordSet(countriesIx);
    static int hiresIx = e->KeywordIx("HIRES");
    bool kw_hires = e->KeywordSet(hiresIx);
    static int fillIx = e->KeywordIx("FILL_CONTINENTS");
    bool kw_fill = e->KeywordSet(fillIx);
 
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

    string dir = string(GDLDATADIR) + "/../gshhs/"; 
    enum set {continents, countries, rivers, coasts};
    string sufix = kw_hires ? "_h.b" : "_c.b";

    vector<string> files(4);
    files[continents] = dir + "gshhs" + sufix;
    files[countries] = dir + "wdb_borders" + sufix;
    files[rivers] = dir + "wdb_rivers" + sufix;
    files[coasts] = dir + "gshhs" + sufix;

    for (int i = 0; i < files.size(); ++i)
    {
      if (kw_fill && i != continents) continue; 

      if (i == countries && !kw_countries) continue;
      if (i == rivers && !kw_rivers) continue;
      // TODO: coasts, continents

      FILE *fp = NULL;
      if ((fp = fopen (files[i].c_str(), "rb")) == NULL ) 
        e->Throw("GSHHS: Could not open file: " + files[i]); 

      struct GSHHS h;
      int n_read = fread((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, fp);
      int version = (h.flag >> 8) & 255;
      int flip = (version != GSHHS_DATA_RELEASE);	/* Take as sign that byte-swabbing is needed */
  	
      int max_east = 270000000;
      while (n_read == 1) 
      {
        if (flip) 
        {
          h.id = swabi4((unsigned int)h.id);
          h.n = swabi4((unsigned int)h.n);
          h.west = swabi4((unsigned int)h.west);
          h.east = swabi4((unsigned int)h.east);
          h.south = swabi4((unsigned int)h.south);
          h.north = swabi4((unsigned int)h.north);
          h.area = swabi4((unsigned int)h.area);
          h.area_full = swabi4((unsigned int)h.area_full);
          h.flag  = swabi4((unsigned int)h.flag);
          h.container = swabi4((unsigned int)h.container);
          h.ancestor = swabi4((unsigned int)h.ancestor);
        }
  
        int level = h.flag & 255;                    // Level is 1-4 
        version = (h.flag >> 8) & 255;               // Version is 1-7 
        int greenwich = (h.flag >> 16) & 1;          // Greenwich is 0 or 1 
        int src = (h.flag >> 24) & 1;                // Greenwich is 0 (WDBII) or 1 (WVS) 
        int river = (h.flag >> 25) & 1;              // River is 0 (not river) or 1 (is river) 
        double ww = h.west  * GSHHS_SCL;             // Convert from microdegrees to degrees 
        double ee = h.east  * GSHHS_SCL;
        double ss = h.south * GSHHS_SCL;
        double nn = h.north * GSHHS_SCL;
        char source = (src == 1) ? 'W' : 'C';        // Either WVS or CIA (WDBII) pedigree 
        if (river) source = tolower ((int)source);   // Lower case c means river-lake 
        int line = (h.area) ? 0 : 1;                 // Either Polygon (0) or Line (1) (if no area) 

        /*
        double area = 0.1 * h.area;                  // Now im km^2 
        double f_area = 0.1 * h.area_full;           // Now im km^2 
  
        char kind[2] = {'P', 'L'};
        char c = kind[line];
        if (line)
        {
          printf ("%c %6d%8d%2d%2c%10.5f%10.5f%10.5f%10.5f\n", c, h.id, h.n, level, source, ww, ee, ss, nn);
        }
        else 
        {
          char ancestor[8], container[8];
          (h.container == -1) ? sprintf (container, "-") : sprintf (container, "%6d", h.container);
          (h.ancestor == -1) ? sprintf (ancestor, "-") : sprintf (ancestor, "%6d", h.ancestor);
          printf ("%c %6d%8d%2d%2c%13.3f%13.3f%10.5f%10.5f%10.5f%10.5f %s %s\n", c, h.id, h.n, level, source, area, f_area, ww, ee, ss, nn, container, ancestor);
        }
        */

        double lon_last, lat_last;
        PLFLT *lons, *lats;
        if (kw_fill && !line)
        {
          lons = (PLFLT*)malloc(h.n * sizeof(PLFLT));
          if (lons == NULL) 
            e->Throw("Failed to allocate memory (lons)");
          lats = (PLFLT*)malloc(h.n * sizeof(PLFLT));
          if (lats == NULL) 
            e->Throw("Failed to allocate memory (lats)");
        }
        for (int k = 0; k < h.n; k++) 
        {
          struct POINT p;
          if (fread ((void *)&p, (size_t)sizeof(struct POINT), (size_t)1, fp) != 1) 
          {
            e->Throw("Error reading file" + files[i] + " for " + (line ? "line" : "polygon") 
              + i2s(h.id) + ", point " + i2s(k));
          }
          
          if (!(line && kw_fill))
          {
            // byte order
            if (flip) 
            {
              p.x = swabi4((unsigned int)p.x);
              p.y = swabi4((unsigned int)p.y);
            }
  
            // value scaling
            double lon = p.x * GSHHS_SCL;
            if ((greenwich && p.x > max_east) || (h.west > 180000000)) lon -= 360.0;
            double lat = p.y * GSHHS_SCL;
  
            // map projection
            if (mapSet) // ... always true
            {
              idata.lam = lon * DEG_TO_RAD;
              idata.phi = lat * DEG_TO_RAD;
              odata = PJ_FWD(idata, ref);
              lon = odata.x;
              lat = odata.y;
            }

            // drawing line or recording data for drawing a polygon afterwards
            if (!kw_fill)
            {
              if (k != 0) actStream->join(lon_last, lat_last, lon, lat);
              lon_last = lon;
              lat_last = lat;
            }
            else
            {
              lons[k] = lon;
              lats[k] = lat;
            }
          }
        }
        if (kw_fill && !line) 
        {
          actStream->fill(h.n, lons, lats); // TODO: PL_MAXPOLY is 256 :(
          free(lons);
          free(lats);
        }
        max_east = 180000000;	/* Only Eurasia needs 270 */
        n_read = fread((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, fp);
      }
		
      fclose(fp);
    }

    actStream->lsty(1); //reset linestyle
    actStream->flush(); 

#endif
  }

} // namespace

