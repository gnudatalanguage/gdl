//----------------------------------*-C++-*----------------------------------//
// Copyright (C) 1995 Geoffrey Furnish
// Copyright (C) 1995-2002 Maurice LeBrun
// Copyright (C) 2000-2018 Alan W. Irwin
// Copyright (C) 2003-2013 Andrew Ross
// Copyright (C) 2004-2005 Rafael Laboissiere
// Copyright (C) 2006-2008 Werner Smekal
// Copyright (C) 2009 Hazen Babcock
// Copyright (C) 2010-2011 Hezekiah M. Carty
// Copyright (C) 2014-2015 Phil Rosenberg
//
// This file is part of PLplot.
//
// PLplot is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published
// by the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// PLplot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with PLplot; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

//--------------------------------------------------------------------------
// @> Header file plstream.
//--------------------------------------------------------------------------

#ifndef __plstream_h__
#define __plstream_h__

#include "plplot.h"

class PLS {
public:
    enum stream_id { Next, Current, Specific };
};

enum PLcolor { Black = 0, Red, Yellow, Green,
               Cyan, Pink, Tan, Grey,
               DarkRed, DeepBlue, Purple, LightCyan,
               LightBlue, Orchid, Mauve, White };

// A class for assisting in generalizing the data prescription
// interface to the contouring routines.

class Contourable_Data {
    int _nx, _ny;
public:
    Contourable_Data( int nx, int ny ) : _nx( nx ), _ny( ny ) {}
    virtual void elements( int& nx, int& ny ) const { nx = _nx; ny = _ny; }
    virtual PLFLT operator()( int i, int j ) const = 0;
    virtual ~Contourable_Data() {};
};

PLDLLIMPEXP_CXX PLFLT Contourable_Data_evaluator( PLINT i, PLINT j, PLPointer p );

class PLDLLIMPEXP_CXX Coord_Xformer {
public:
    virtual void xform( PLFLT ox, PLFLT oy, PLFLT& nx, PLFLT& ny ) const = 0;
    virtual ~Coord_Xformer() {};
};

PLDLLIMPEXP_CXX void Coord_Xform_evaluator( PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer );

class Coord_2d {
public:
    virtual PLFLT operator()( int ix, int iy ) const = 0;
    virtual void elements( int& _nx, int& _ny )      = 0;
    virtual void min_max( PLFLT& _min, PLFLT& _max ) = 0;
    virtual ~Coord_2d() {};
};

class PLDLLIMPEXP_CXX cxx_pltr2 : public Coord_Xformer {
    Coord_2d& xg;
    Coord_2d& yg;
public:
    cxx_pltr2( Coord_2d & cx, Coord_2d & cy );
    void xform( PLFLT x, PLFLT y, PLFLT& tx, PLFLT& ty ) const;
};

//--------------------------------------------------------------------------
//Callback functions for passing into various API methods. We provide these
//wrappers to avoid a requirement for linking to the C shared library.
//--------------------------------------------------------------------------

namespace plcallback
{
// Callback for plfill. This will just call the C plfill function

PLDLLIMPEXP_CXX void fill( PLINT n, const PLFLT *x, const PLFLT *y );

// Identity transformation.

PLDLLIMPEXP_CXX void tr0( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );

// Does linear interpolation from singly dimensioned coord arrays.

PLDLLIMPEXP_CXX void tr1( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );

// Does linear interpolation from doubly dimensioned coord arrays
// (column dominant, as per normal C 2d arrays).

PLDLLIMPEXP_CXX void tr2( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );

// Just like pltr2() but uses pointer arithmetic to get coordinates from
// 2d grid tables.

PLDLLIMPEXP_CXX void tr2p( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );
}

//--------------------------------------------------------------------------
// class plstream - C++ class for encapsulating PLplot streams

// Cool stuff.
//--------------------------------------------------------------------------

class PLDLLIMPEXP_CXX plstream {
    PLINT        stream;

    static PLINT active_streams;

private:
    // These have to be disabled till we implement reference counting.

    plstream( const plstream & );
    plstream& operator=( const plstream& );

protected:
    virtual void set_stream( void ) { ::c_plsstrm( stream ); }

public:
    plstream( void );
    plstream( plstream * pls );
    plstream( PLS::stream_id sid, PLINT strm = 0 );
    plstream( PLINT _stream ) : stream( _stream ) {}
    plstream( PLINT nx /*=1*/, PLINT ny /*=1*/,
              const char *driver = NULL, const char *file = NULL );
    plstream( PLINT nx /*=1*/, PLINT ny /*=1*/, PLINT r, PLINT g, PLINT b,
              const char *driver = NULL, const char *file = NULL );

    virtual ~plstream( void );

// Now start miroring the PLplot C API.

    // C routines callable from stub routines come first

// Advance to subpage "page", or to the next one if "page" = 0.

    void adv( PLINT page );

// Plot an arc

    void arc( PLFLT x, PLFLT y, PLFLT a, PLFLT b, PLFLT angle1, PLFLT angle2,
              PLFLT rotate, PLBOOL fill );

    void vect( const PLFLT * const *u, const PLFLT * const *v, PLINT nx, PLINT ny, PLFLT scale,
               PLTRANSFORM_callback pltr, PLPointer pltr_data );

// Set the arrow style
    void svect( const PLFLT *arrow_x = NULL, const PLFLT *arrow_y = NULL, PLINT npts = 0, bool fill = false );

// This functions similarly to plbox() except that the origin of the axes is
// placed at the user-specified point (x0, y0).

    void axes( PLFLT x0, PLFLT y0, const char *xopt, PLFLT xtick, PLINT nxsub,
               const char *yopt, PLFLT ytick, PLINT nysub );

// Plot a histogram using x to store data values and y to store frequencies.

    void bin( PLINT nbin, const PLFLT *x, const PLFLT *y, PLINT center );

// Start new page.  Should only be used with pleop().

    void bop( void );

// This draws a box around the current viewport.

    void box( const char *xopt, PLFLT xtick, PLINT nxsub,
              const char *yopt, PLFLT ytick, PLINT nysub );

// This is the 3-d analogue of plbox().

    void box3( const char *xopt, const char *xlabel, PLFLT xtick, PLINT nsubx,
               const char *yopt, const char *ylabel, PLFLT ytick, PLINT nsuby,
               const char *zopt, const char *zlabel, PLFLT ztick, PLINT nsubz );

//// Calculate broken-down time from continuous time for current stream.
//
//    void btime( PLINT &year, PLINT &month, PLINT &day, PLINT &hour,
//                PLINT &min, PLFLT &sec, PLFLT ctime );

// Calculate world coordinates and subpage from relative device coordinates.

    void calc_world( PLFLT rx, PLFLT ry, PLFLT& wx, PLFLT& wy, PLINT& window );

// Clear the current subpage.

    void clear( void );

// Set color, map 0.  Argument is integer between 0 and 15.

    void col0( PLINT icol0 );

// Set the color using a descriptive name.  Replaces plcol0().  (Except that
// col0 won't die.)

    void col( PLcolor c );

// Set color, map 1.  Argument is a float between 0. and 1.

    void col1( PLFLT c );

#ifdef PL_DEPRECATED
// Previous function was inadvertently named plcol in old versions of
// plplot - this is maintained for backwards compatibility, but is best
// avoided in new code.
    void col( PLFLT c );
#endif //PL_DEPRECATED

//// Configure transformation between continuous and broken-down time (and
//// vice versa) for current stream.
//    void configtime( PLFLT scale, PLFLT offset1, PLFLT offset2,
//                     PLINT ccontrol, PLBOOL ifbtime_offset, PLINT year,
//                     PLINT month, PLINT day, PLINT hour, PLINT min, PLFLT sec );

// Draws a contour plot from data in f(nx,ny).  Is just a front-end to
// plfcont, with a particular choice for f2eval and f2eval_data.

    void cont( const PLFLT * const *f, PLINT nx, PLINT ny, PLINT kx, PLINT lx,
               PLINT ky, PLINT ly, const PLFLT * clevel, PLINT nlevel,
               PLTRANSFORM_callback pltr, PLPointer pltr_data );

// Draws a contour plot using the function evaluator f2eval and data stored
// by way of the f2eval_data pointer.  This allows arbitrary organizations
// of 2d array data to be used.

    void fcont( PLFLT ( *f2eval )( PLINT, PLINT, PLPointer ),
                PLPointer f2eval_data,
                PLINT nx, PLINT ny, PLINT kx, PLINT lx,
                PLINT ky, PLINT ly, const PLFLT * clevel, PLINT nlevel,
                PLTRANSFORM_callback pltr, PLPointer pltr_data );

// Copies state parameters from the reference stream to the current stream.

    void cpstrm( plstream &pls, bool flags );

//// Calculate continuous time from broken-down time for current stream.
//    void ctime( PLINT year, PLINT month, PLINT day, PLINT hour, PLINT min,
//                PLFLT sec, PLFLT &ctime );

// Converts input values from relative device coordinates to relative plot
// coordinates.

    void did2pc( PLFLT& xmin, PLFLT& ymin, PLFLT& xmax, PLFLT& ymax );

// Converts input values from relative plot coordinates to relative device
// coordinates.

    void dip2dc( PLFLT& xmin, PLFLT& ymin, PLFLT& xmax, PLFLT& ymax );

// These shouldn't be needed, are supposed to be handled by ctor/dtor
// semantics of the plstream object.

//  End a plotting session for all open streams.

//     void end();

// End a plotting session for the current stream only.

//     void end1();

// Simple interface for defining viewport and window.

    void env( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
              PLINT just, PLINT axis );

// similar to env() above, but in multiplot mode does not advance
// the subpage, instead the current subpage is cleared

    void env0( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
               PLINT just, PLINT axis );

// End current page.  Should only be used with plbop().

    void eop( void );

// Plot horizontal error bars (xmin(i),y(i)) to (xmax(i),y(i)).

    void errx( PLINT n, const PLFLT *xmin, const PLFLT *xmax, const PLFLT *y );

// Plot vertical error bars (x,ymin(i)) to (x(i),ymax(i)).

    void erry( PLINT n, const PLFLT *x, const PLFLT *ymin, const PLFLT *ymax );

// Advance to the next family file on the next new page.

    void famadv( void );

// Pattern fills the polygon bounded by the input points.

    void fill( PLINT n, const PLFLT *x, const PLFLT *y );

// Pattern fills the 3d polygon bounded by the input points.

    void fill3( PLINT n, const PLFLT *x, const PLFLT *y, const PLFLT *z );

// Flushes the output stream.  Use sparingly, if at all.

    void flush( void );

// Sets the global font flag to 'ifont'.

    void font( PLINT ifont );

// Load specified font set.

    void fontld( PLINT fnt );

// Get character default height and current (scaled) height.

    void gchr( PLFLT& p_def, PLFLT& p_ht );

// Returns 8 bit RGB values for given color from color map 0.

    void gcol0( PLINT icol0, PLINT& r, PLINT& g, PLINT& b );

// Returns 8 bit RGB and 0.0-1.0 alpha values for given color from color map 0.

    void gcol0a( PLINT icol0, PLINT& r, PLINT& g, PLINT& b, PLFLT& a );

// Returns the background color by 8 bit RGB value.

    void gcolbg( PLINT& r, PLINT& g, PLINT& b );

// Returns the background color by 8 bit RGB and 0.0-1.0 alpha values.

    void gcolbga( PLINT& r, PLINT& g, PLINT& b, PLFLT& a );

// Returns the current compression setting

    void gcompression( PLINT& compression );

// Retrieve current window into device space.

    void gdidev( PLFLT& mar, PLFLT& aspect, PLFLT& jx, PLFLT& jy );

// Get plot orientation.

    void gdiori( PLFLT& rot );

// Retrieve current window into plot space.

    void gdiplt( PLFLT& xmin, PLFLT& ymin, PLFLT& xmax, PLFLT& ymax );

// Get FCI (font characterization integer)

    void gfci( PLUNICODE& pfci );

// Get family file parameters.

    void gfam( PLINT& fam, PLINT& num, PLINT& bmax );

// Get the (current) output file name.  Must be preallocated to >80 bytes.

    void gfnam( char *fnam );

// Get the current font family, style and weight

    void gfont( PLINT& family, PLINT& style, PLINT& weight );

// Get the (current) run level.

    void glevel( PLINT &p_level );

// Get output device parameters.

    void gpage( PLFLT& xp, PLFLT& yp, PLINT& xleng, PLINT& yleng,
                PLINT& xoff, PLINT& yoff );

// Switches to graphics screen.

    void gra( void );

// Draw gradient in polygon.

    void gradient( PLINT n, const PLFLT *x, const PLFLT *y, PLFLT angle );

// grid irregularly sampled data

    void griddata( const PLFLT *x, const PLFLT *y, const PLFLT *z, PLINT npts,
                   const PLFLT *xg, PLINT nptsx, const PLFLT *yg, PLINT nptsy,
                   PLFLT **zg, PLINT type, PLFLT data );

// Get subpage boundaries in absolute coordinates.

    void gspa( PLFLT& xmin, PLFLT& xmax, PLFLT& ymin, PLFLT& ymax );

// This shouldn't be needed in this model.

// Get current stream number.

//     void gstrm( PLINT *p_strm );

// Get the current library version number.

    void gver( char *p_ver );

// Get the viewport window in normalized device coordinates

    void gvpd( PLFLT& xmin, PLFLT& xmax, PLFLT& ymin, PLFLT& ymax );

// Get the viewport window in world coordinates

    void gvpw( PLFLT& xmin, PLFLT& xmax, PLFLT& ymin, PLFLT& ymax );

// Get x axis labeling parameters.

    void gxax( PLINT& digmax, PLINT& digits );

// Get y axis labeling parameters.

    void gyax( PLINT& digmax, PLINT& digits );

// Get z axis labeling parameters

    void gzax( PLINT& digmax, PLINT& digits );

// Draws a histogram of n values of a variable in array data[0..n-1]

    void hist( PLINT n, const PLFLT *data, PLFLT datmin, PLFLT datmax,
               PLINT nbin, PLINT oldwin );

// Initializes PLplot, using preset or default options

    void init( void );

// Draws a line segment from (x1, y1) to (x2, y2).

    void join( PLFLT x1, PLFLT y1, PLFLT x2, PLFLT y2 );

// Simple routine for labelling graphs.

    void lab( const char *xlabel, const char *ylabel, const char *tlabel );

// Routine for drawing line, symbol, or cmap0 legends
    void legend( PLFLT *p_legend_width, PLFLT *p_legend_height,
                 PLINT opt, PLINT position, PLFLT x, PLFLT y, PLFLT plot_width,
                 PLINT bg_color, PLINT bb_color, PLINT bb_style,
                 PLINT nrow, PLINT ncolumn,
                 PLINT nlegend, const PLINT *opt_array,
                 PLFLT text_offset, PLFLT text_scale, PLFLT text_spacing,
                 PLFLT text_justification,
                 const PLINT *text_colors, const char * const *text,
                 const PLINT *box_colors, const PLINT *box_patterns,
                 const PLFLT *box_scales, const PLFLT *box_line_widths,
                 const PLINT *line_colors, const PLINT *line_styles,
                 const PLFLT *line_widths,
                 const PLINT *symbol_colors, const PLFLT *symbol_scales,
                 const PLINT *symbol_numbers, const char * const *symbols );

    void colorbar( PLFLT *p_colorbar_width, PLFLT *p_colorbar_height,
                   PLINT opt, PLINT position, PLFLT x, PLFLT y,
                   PLFLT x_length, PLFLT y_length,
                   PLINT bg_color, PLINT bb_color, PLINT bb_style,
                   PLFLT low_cap_color, PLFLT high_cap_color,
                   PLINT cont_color, PLFLT cont_width,
                   PLINT n_labels, PLINT *label_opts, const char * const *label,
                   PLINT n_axes, const char * const *axis_opts,
                   PLFLT *ticks, PLINT *sub_ticks,
                   PLINT *n_values, const PLFLT * const *values );

// Sets position of the light source

    void lightsource( PLFLT x, PLFLT y, PLFLT z );

// Draws line segments connecting a series of points.

    void line( PLINT n, const PLFLT *x, const PLFLT *y );

// Draws a line in 3 space.

    void line3( PLINT n, const PLFLT *x, const PLFLT *y, const PLFLT *z );

// Set line style.

    void lsty( PLINT lin );

//// Plot continental outline in world coordinates
//
//    void map( PLMAPFORM_callback mapform, const char *name,
//              PLFLT minx, PLFLT maxx, PLFLT miny, PLFLT maxy );
//
//// Plot map lines
//
//    void mapline( PLMAPFORM_callback mapform, const char *name,
//                  PLFLT minx, PLFLT maxx, PLFLT miny, PLFLT maxy,
//                  const PLINT *plotentries, PLINT nplotentries );
//
//// Plot map points
//
//    void mapstring( PLMAPFORM_callback mapform,
//                    const char *name, const char *string,
//                    PLFLT minx, PLFLT maxx, PLFLT miny, PLFLT maxy,
//                    const PLINT *plotentries, PLINT nplotentries );
//
//// Plot map text
//
//    void maptex( PLMAPFORM_callback mapform,
//                 const char *name, PLFLT dx, PLFLT dy, PLFLT just, const char *text,
//                 PLFLT minx, PLFLT maxx, PLFLT miny, PLFLT maxy,
//                 PLINT plotentry );
//
//// Plot map fills
//
//    void mapfill( PLMAPFORM_callback mapform,
//                  const char *name, PLFLT minx, PLFLT maxx, PLFLT miny,
//                  PLFLT maxy, const PLINT *plotentries, PLINT nplotentries );
//
//// Plot the latitudes and longitudes on the background.
//
//    void meridians( PLMAPFORM_callback mapform,
//                    PLFLT dlong, PLFLT dlat, PLFLT minlong, PLFLT maxlong,
//                    PLFLT minlat, PLFLT maxlat );

// Plots a mesh representation of the function z[x][y].

    void mesh( const PLFLT *x, const PLFLT *y, const PLFLT * const *z, PLINT nx, PLINT ny, PLINT opt );

// Plots a mesh representation of the function z[x][y] with contour.

    void meshc( const PLFLT *x, const PLFLT *y, const PLFLT * const *z, PLINT nx, PLINT ny, PLINT opt,
                const PLFLT *clevel, PLINT nlevel );

// Creates a new stream and makes it the default.

// void
// mkstrm(PLINT *p_strm);

// Prints out "text" at specified position relative to viewport

    void mtex( const char *side, PLFLT disp, PLFLT pos, PLFLT just,
               const char *text );

// Prints out "text" at specified position relative to viewport (3D)

    void mtex3( const char *side, PLFLT disp, PLFLT pos, PLFLT just,
                const char *text );

// Plots a 3-d representation of the function z[x][y].

    void plot3d( const PLFLT *x, const PLFLT *y, const PLFLT * const *z,
                 PLINT nx, PLINT ny, PLINT opt, bool side );

// Plots a 3-d representation of the function z[x][y] with contour.

    void plot3dc( const PLFLT *x, const PLFLT *y, const PLFLT * const *z,
                  PLINT nx, PLINT ny, PLINT opt,
                  const PLFLT *clevel, PLINT nlevel );

// Plots a 3-d representation of the function z[x][y] with contour
// and y index limits.

    void plot3dcl( const PLFLT *x, const PLFLT *y, const PLFLT * const *z,
                   PLINT nx, PLINT ny, PLINT opt,
                   const PLFLT *clevel, PLINT nlevel,
                   PLINT ixstart, PLINT ixn, const PLINT *indexymin, const PLINT *indexymax, PLINT* shademap );


// Plots a 3-d shaded representation of the function z[x][y].

    void surf3d( const PLFLT *x, const PLFLT *y, const PLFLT * const *z,
                 PLINT nx, PLINT ny, PLINT opt,
                 const PLFLT *clevel, PLINT nlevel, PLINT* shademap  );

// Plots a 3-d shaded representation of the function z[x][y] with y
// index limits

    void surf3dl( const PLFLT *x, const PLFLT *y, const PLFLT * const *z,
                  PLINT nx, PLINT ny, PLINT opt,
                  const PLFLT *clevel, PLINT nlevel,
                  PLINT ixstart, PLINT ixn, const PLINT *indexymin, const PLINT *indexymax, PLINT* shademap );


// Process options list using current options info.

    PLINT parseopts( int *p_argc, char **argv, PLINT mode );

// Set fill pattern directly.

    void pat( PLINT nlin, const PLINT *inc, const PLINT *del );

// Draw a line connecting two points, accounting for coordinate transforms

    void path( PLINT n, PLFLT x1, PLFLT y1, PLFLT x2, PLFLT y2 );

// Plots array y against x for n points using ASCII code "code".

    void poin( PLINT n, const PLFLT *x, const PLFLT *y, PLINT code );

// Draws a series of points in 3 space.

    void poin3( PLINT n, const PLFLT *x, const PLFLT *y, const PLFLT *z, PLINT code );

// Draws a polygon in 3 space.

    void poly3( PLINT n, const PLFLT *x, const PLFLT *y, const PLFLT *z, const bool *draw, bool ifcc );

// Set the floating point precision (in number of places) in numeric labels.

    void prec( PLINT setp, PLINT prec );

// Set fill pattern, using one of the predefined patterns.

    void psty( PLINT patt );

// Prints out "text" at world cooordinate (x,y).

    void ptex( PLFLT x, PLFLT y, PLFLT dx, PLFLT dy, PLFLT just,
               const char *text );

// Prints out "text" at world cooordinate (x,y,z).

    void ptex3( PLFLT wx, PLFLT wy, PLFLT wz, PLFLT dx, PLFLT dy, PLFLT dz,
                PLFLT sx, PLFLT sy, PLFLT sz, PLFLT just, const char *text );

// Get the world coordinates associated with device coordinates

    PLINT translatecursor( PLGraphicsIn *gin );

// Replays contents of plot buffer to current device/file.

    void replot( void );

// Set character height.

    void schr( PLFLT def, PLFLT scale );

// Set number of colors in cmap 0

    void scmap0n( PLINT ncol0 );

// Set number of colors in cmap 1

    void scmap1n( PLINT ncol1 );

// Set the color map 1 range used in continuous plots

    void scmap1_range( PLFLT min_color, PLFLT max_color );

// Get the color map 1 range used in continuous plots

    void gcmap1_range( PLFLT &min_color, PLFLT &max_color );

// Set color map 0 colors by 8 bit RGB values

    void scmap0( const PLINT *r, const PLINT *g, const PLINT *b, PLINT ncol0 );

// Set color map 0 colors by 8 bit RGB values and alpha value (0.0-1.0)

    void scmap0a( const PLINT *r, const PLINT *g, const PLINT *b, const PLFLT *a, PLINT ncol0 );

// Set color map 1 colors by 8 bit RGB values

    void scmap1( const PLINT *r, const PLINT *g, const PLINT *b, PLINT ncol1 );

// Set color map 1 colors by 8 bit RGB values and alpha value (0.0-1.0)

    void scmap1a( const PLINT *r, const PLINT *g, const PLINT *b, const PLFLT *a, PLINT ncol1 );

// Set color map 1 colors using a piece-wise linear relationship between
// intensity [0,1] (cmap 1 index) and position in HLS or RGB color space.

    void scmap1l( bool itype, PLINT npts, const PLFLT *intensity,
                  const PLFLT *coord1, const PLFLT *coord2, const PLFLT *coord3, const bool *alt_hue_path = NULL );

//    void scmap1l( bool itype, PLINT npts, PLFLT *intensity,
//                PLFLT *coord1, PLFLT *coord2, PLFLT *coord3 );

// Set color map 1 colors using a piece-wise linear relationship between
// intensity [0,1] (cmap 1 index) and position in HLS or RGB color space.
// Include alpha value in range 0.0-1.0.

    void scmap1la( bool itype, PLINT npts, const PLFLT *intensity,
                   const PLFLT *coord1, const PLFLT *coord2, const PLFLT *coord3, const PLFLT *a,
                   const bool *alt_hue_path = NULL );

// Set a given color from color map 0 by 8 bit RGB value

    void scol0( PLINT icol0, PLINT r, PLINT g, PLINT b );

// Set a given color from color map 0 by 8 bit RGB value and alpha value (0.0-1.0)

    void scol0a( PLINT icol0, PLINT r, PLINT g, PLINT b, PLFLT a );

// Set the background color by 8 bit RGB value

    void scolbg( PLINT r, PLINT g, PLINT b );

// Set the background color by 8 bit RGB + alpha value

    void scolbga( PLINT r, PLINT g, PLINT b, PLFLT a );

// Used to globally turn color output on/off

    void scolor( PLINT color );

// Set the compression level

    void scompression( PLINT compression );

// Set the device (keyword) name

    void sdev( const char *devname );

// Get the device (keyword) name

    void gdev( char *devname );

// Set window into device space using margin, aspect ratio, and
// justification

    void sdidev( PLFLT mar, PLFLT aspect, PLFLT jx, PLFLT jy );

// Set up transformation from metafile coordinates.

    void sdimap( PLINT dimxmin, PLINT dimxmax,
                 PLINT dimymin, PLINT dimymax,
                 PLFLT dimxpmm, PLFLT dimypmm );

// Set plot orientation, specifying rotation in units of pi/2.

    void sdiori( PLFLT rot );

// Set window into plot space

    void sdiplt( PLFLT xmin, PLFLT ymin, PLFLT xmax, PLFLT ymax );

// Set window into plot space incrementally (zoom)

    void sdiplz( PLFLT xmin, PLFLT ymin, PLFLT xmax, PLFLT ymax );

// Set the escape character for text strings.

    void sesc( char esc );

// Set offset and spacing of contour labels

    void setcontlabelparam( PLFLT offset, PLFLT size, PLFLT spacing,
                            PLINT active );

// Set the format of the contour labels

    void setcontlabelformat( PLINT lexp, PLINT sigdig );

// Set family file parameters

    void sfam( PLINT fam, PLINT num, PLINT bmax );

// Set FCI (font characterization integer)

    void sfci( PLUNICODE fci );

// Set the output file name.

    void sfnam( const char *fnam );

// Set the pointer to the data used in driver initialisation

    void sdevdata( void *data );

// Set the current font family, style and weight

    void sfont( PLINT family, PLINT style, PLINT weight );

// Shade region.

    void shade( const PLFLT * const *a, PLINT nx, PLINT ny,
                PLDEFINED_callback defined,
                PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
                PLFLT shade_min, PLFLT shade_max,
                PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
                PLINT min_color, PLFLT min_width,
                PLINT max_color, PLFLT max_width,
                PLFILL_callback fill, bool rectangular,
                PLTRANSFORM_callback pltr, PLPointer pltr_data );

    void shades( const PLFLT * const *a, PLINT nx, PLINT ny,
                 PLDEFINED_callback defined,
                 PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
                 const PLFLT * clevel, PLINT nlevel, PLFLT fill_width,
                 PLINT cont_color, PLFLT cont_width,
                 PLFILL_callback fill, bool rectangular,
                 PLTRANSFORM_callback pltr, PLPointer pltr_data );

// Would be nice to fix this even more, say by stuffing xmin, xmax,
// ymin, ymax, rectangular, and pcxf all into the contourable data
// class.  Have to think more on that.  Or maybe the coordinate info.

    void shade( Contourable_Data& d, PLFLT xmin, PLFLT xmax,
                PLFLT ymin, PLFLT ymax, PLFLT shade_min, PLFLT shade_max,
                PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
                PLINT min_color, PLFLT min_width,
                PLINT max_color, PLFLT max_width,
                bool rectangular,
                Coord_Xformer *pcxf );

#ifdef PL_DEPRECATED
    void shade1( const PLFLT * a, PLINT nx, PLINT ny,
                 PLDEFINED_callback defined,
                 PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
                 PLFLT shade_min, PLFLT shade_max,
                 PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
                 PLINT min_color, PLFLT min_width,
                 PLINT max_color, PLFLT max_width,
                 PLFILL_callback fill, bool rectangular,
                 PLTRANSFORM_callback pltr, PLPointer pltr_data );

    void shade1( const PLFLT * a, PLINT nx, PLINT ny,
                 PLDEFINED_callback defined,
                 PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
                 PLFLT shade_min, PLFLT shade_max,
                 PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
                 PLINT min_color, PLFLT min_width,
                 PLINT max_color, PLFLT max_width,
                 PLFILL_callback fill, PLINT rectangular,
                 PLTRANSFORM_callback pltr, PLPointer pltr_data );

#endif //PL_DEPRECATED
    void fshade( PLFLT ( *f2eval )( PLINT, PLINT, PLPointer ),
                 PLPointer f2eval_data,
                 PLFLT ( *c2eval )( PLINT, PLINT, PLPointer ),
                 PLPointer c2eval_data,
                 PLINT nx, PLINT ny,
                 PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
                 PLFLT shade_min, PLFLT shade_max,
                 PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
                 PLINT min_color, PLFLT min_width,
                 PLINT max_color, PLFLT max_width,
                 PLFILL_callback fill, bool rectangular,
                 PLTRANSFORM_callback pltr, PLPointer pltr_data );

// Setup a user-provided custom labeling function

    void slabelfunc( PLLABEL_FUNC_callback label_func, PLPointer label_data );

// Set up lengths of major tick marks.

    void smaj( PLFLT def, PLFLT scale );

// Set the RGB memory area to be plotted (with the 'mem' or 'memcairo' drivers)

    void smem( PLINT maxx, PLINT maxy, void *plotmem );

// Set the RGBA memory area to be plotted (with the 'memcairo' driver)

    void smema( PLINT maxx, PLINT maxy, void *plotmem );

// Set up lengths of minor tick marks.

    void smin( PLFLT def, PLFLT scale );

// Set orientation.  Must be done before calling plinit.

    void sori( PLINT ori );

// Set output device parameters.  Usually ignored by the driver.

    void spage( PLFLT xp, PLFLT yp, PLINT xleng, PLINT yleng,
                PLINT xoff, PLINT yoff );

// Set the pause (on end-of-page) status

    void spause( bool pause );

// Set the colors for color table 0 from a cmap0 file

    void spal0( const char *filename );

// Set the colors for color table 1 from a cmap1 file

    void spal1( const char *filename, bool interpolate = true );

// Set stream number.

    void sstrm( PLINT strm );

// Set the number of subwindows in x and y

    void ssub( PLINT nx, PLINT ny );

// Set symbol height.

    void ssym( PLFLT def, PLFLT scale );

// Initialize PLplot, passing in the windows/page settings.

    void star( PLINT nx, PLINT ny );

// Initialize PLplot, passing the device name and windows/page settings.

    void start( const char *devname, PLINT nx, PLINT ny );

// Set the coordinate transform

    void stransform( PLTRANSFORM_callback coordinate_transform, PLPointer coordinate_transform_data );

// Prints out the same string repeatedly at the n points in world
// coordinates given by the x and y arrays.  Supersedes plpoin and
// plsymbol for the case where text refers to a unicode glyph either
// directly as UTF-8 or indirectly via the standard text escape
// sequences allowed for PLplot input strings.

    void string( PLINT n, const PLFLT *x, const PLFLT *y, const char *string );

// Prints out the same string repeatedly at the n points in world
// coordinates given by the x, y, and z arrays.  Supersedes plpoin3
// for the case where text refers to a unicode glyph either directly
// as UTF-8 or indirectly via the standard text escape sequences
// allowed for PLplot input strings.

    void string3( PLINT n, const PLFLT *x, const PLFLT *y, const PLFLT *z, const char *string );

// Create 1d stripchart

    void stripc( PLINT *id, const char *xspec, const char *yspec,
                 PLFLT xmin, PLFLT xmax, PLFLT xjump, PLFLT ymin, PLFLT ymax,
                 PLFLT xlpos, PLFLT ylpos,
                 bool y_ascl, bool acc,
                 PLINT colbox, PLINT collab,
                 const PLINT colline[], const PLINT styline[], const char *legline[],
                 const char *labx, const char *laby, const char *labtop );

// Add a point to a stripchart.

    void stripa( PLINT id, PLINT pen, PLFLT x, PLFLT y );

// Deletes and releases memory used by a stripchart.

    void stripd( PLINT id );

// plots a 2d image (or a matrix too large for plshade() ) - colors
// automatically scaled

    void image( const PLFLT * const *data, PLINT nx, PLINT ny, PLFLT xmin, PLFLT xmax,
                PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
                PLFLT Dxmin, PLFLT Dxmax, PLFLT Dymin, PLFLT Dymax );

// plots a 2d image (or a matrix too large for plshade() )

    void imagefr( const PLFLT * const *data, PLINT nx, PLINT ny, PLFLT xmin, PLFLT xmax,
                  PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
                  PLFLT valuemin, PLFLT valuemax,
                  PLTRANSFORM_callback pltr, PLPointer pltr_data );

// Set up a new line style

    void styl( PLINT nms, const PLINT *mark, const PLINT *space );

// Sets the edges of the viewport to the specified absolute coordinates

    void svpa( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );

// Set x axis labeling parameters

    void sxax( PLINT digmax, PLINT digits );

// Set inferior X window

    void sxwin( PLINT window_id );

// Set y axis labeling parameters

    void syax( PLINT digmax, PLINT digits );

// Plots array y against x for n points using Hershey symbol "code"

    void sym( PLINT n, const PLFLT *x, const PLFLT *y, PLINT code );

// Set z axis labeling parameters

    void szax( PLINT digmax, PLINT digits );

// Switches to text screen.

    void text( void );

// Set the format for date / time labels

    void timefmt( const char *fmt );

// Sets the edges of the viewport with the given aspect ratio, leaving
// room for labels.

    void vasp( PLFLT aspect );

// Creates the largest viewport of the specified aspect ratio that fits
// within the specified normalized subpage coordinates.

    void vpas( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT aspect );

// Creates a viewport with the specified normalized subpage coordinates.

    void vpor( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );

// Defines a "standard" viewport with seven character heights for
// the left margin and four character heights everywhere else.

    void vsta( void );

// Set up a window for three-dimensional plotting.

    void w3d( PLFLT basex, PLFLT basey, PLFLT height, PLFLT xmin0,
              PLFLT xmax0, PLFLT ymin0, PLFLT ymax0, PLFLT zmin0,
              PLFLT zmax0, PLFLT alt, PLFLT az );

// Set pen width.

    void width( PLFLT width );

// Set up world coordinates of the viewport boundaries (2d plots).

    void wind( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );

// Set xor mode; mode = 1-enter, 0-leave, status = 0 if not interactive device
    void xormod( bool mode, bool *status );

// Random number generator based on Mersenne Twister.
// Functions to set seed and obtain real random numbers in the range [0,1].
//
//    void seed( unsigned int s );
//
//    PLFLT randd( void );


    // The rest for use from C only

// Returns a list of file-oriented device names and their menu strings

    void gFileDevs( const char ***p_menustr, const char ***p_devname,
                    int *p_ndev );

// Set the function pointer for the keyboard event handler

    void sKeyEH( void ( *KeyEH )( PLGraphicsIn *, void *, int * ),
                 void *KeyEH_data );

// Sets an optional user bop handler

    void sbopH( void ( *handler )( void *, int * ), void *handlier_data );

// Sets an optional user eop handler

    void seopH( void ( *handler )( void *, int * ), void *handlier_data );

// Set the variables to be used for storing error info

    void sError( PLINT *errcode, char *errmsg );

// Sets an optional user exit handler.

    void sexit( int ( *handler )( const char * ) );

    // Transformation routines

// Identity transformation.

    //static void tr0( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );

// Does linear interpolation from singly dimensioned coord arrays.

    //static void tr1( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );

// Does linear interpolation from doubly dimensioned coord arrays
// (column dominant, as per normal C 2d arrays).

    //static void tr2( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );

// Just like pltr2() but uses pointer arithmetic to get coordinates from
// 2d grid tables.

    //static void tr2p( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );

// We obviously won't be using this object from Fortran...

// Identity transformation for plots from Fortran.

//     void tr0f( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, void *pltr_data );

// Does linear interpolation from doubly dimensioned coord arrays
// (row dominant, i.e. Fortran ordering).

//     void tr2f( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, void *pltr_data );

// Example linear transformation function for contour plotter.
// This is not actually part of the core library any more
    //void  xform( PLFLT x, PLFLT y, PLFLT * tx, PLFLT * ty );

    // Function evaluators

// Does a lookup from a 2d function array.  Array is of type (PLFLT **),
// and is column dominant (normal C ordering).

    PLFLT f2eval2( PLINT ix, PLINT iy, PLPointer plf2eval_data );

// Does a lookup from a 2d function array.  Array is of type (PLFLT *),
// and is column dominant (normal C ordering).

    PLFLT f2eval( PLINT ix, PLINT iy, PLPointer plf2eval_data );

// Does a lookup from a 2d function array.  Array is of type (PLFLT *),
// and is row dominant (Fortran ordering).

    PLFLT f2evalr( PLINT ix, PLINT iy, PLPointer plf2eval_data );

    // Command line parsing utilities

// Clear internal option table info structure.

    void ClearOpts( void );

// Reset internal option table info structure.

    void ResetOpts( void );

// Merge user option table into internal info structure.

    PLINT MergeOpts( PLOptionTable *options, const char *name, const char **notes );

// Set the strings used in usage and syntax messages.

    void SetUsage( char *program_string, char *usage_string );

// Process input strings, treating them as an option and argument pair.

    PLINT setopt( const char *opt, const char *optarg );

// This version is for backward compatibility only - don't use in new code

    int SetOpt( const char *opt, const char *optarg );

// Print usage & syntax message.

    void OptUsage( void );

    // Miscellaneous

// Set the output file pointer

    void gfile( FILE **p_file );

// Get the output file pointer

    void sfile( FILE *file );

// Get the escape character for text strings.

    void gesc( char *p_esc );

// Front-end to driver escape function.

    void cmd( PLINT op, void *ptr );

// Return full pathname for given file if executable

    PLINT  FindName( char *p );

// Looks for the specified executable file according to usual search path.

    char *FindCommand( char *fn );

// Gets search name for file by concatenating the dir, subdir, and file
// name, allocating memory as needed.

    void GetName( char *dir, char *subdir, char *filename, char **filespec );

// Prompts human to input an integer in response to given message.

    PLINT GetInt( char *s );

// Prompts human to input a float in response to given message.

    PLFLT GetFlt( char *s );

// Determine the Iliffe column vector of pointers to PLFLT row
// vectors corresponding to a 2D matrix of PLFLT's that is statically
// allocated.

    void Static2dGrid( PLFLT_NC_MATRIX zIliffe, PLFLT_VECTOR zStatic, PLINT nx, PLINT ny );

// Allocate a block of memory for use as a 2-d grid of PLFLT's organized
// as an Iliffe column vector of pointers to PLFLT row vectors.

    void Alloc2dGrid( PLFLT_NC_MATRIX *f, PLINT nx, PLINT ny );

// Frees a block of memory allocated with plAlloc2dGrid().

    void Free2dGrid( PLFLT **f, PLINT nx, PLINT ny );

// Find the maximum and minimum of a 2d matrix allocated with plAllc2dGrid().
    void MinMax2dGrid( PLFLT_MATRIX f, PLINT nx, PLINT ny, PLFLT *fmax, PLFLT *fmin );

// Functions for converting between HLS and RGB color space

    void hlsrgb( PLFLT h, PLFLT l, PLFLT s,
                 PLFLT *p_r, PLFLT *p_g, PLFLT *p_b );

    void rgbhls( PLFLT r, PLFLT g, PLFLT b,
                 PLFLT *p_h, PLFLT *p_l, PLFLT *p_s );

// Wait for graphics input event and translate to world coordinates

    PLINT GetCursor( PLGraphicsIn *plg );

#ifdef PL_DEPRECATED
// Deprecated versions of methods which use PLINT instead of bool
    void svect( const PLFLT *arrow_x, const PLFLT *arrow_y, PLINT npts, PLINT fill );
    void cpstrm( plstream &pls, PLINT flags );
    void plot3d( const PLFLT *x, const PLFLT *y, const PLFLT * const *z,
                 PLINT nx, PLINT ny, PLINT opt, PLINT side );
    void poly3( PLINT n, const PLFLT *x, const PLFLT *y, const PLFLT *z, const PLINT *draw, PLINT ifcc );
    void scmap1l( PLINT itype, PLINT npts, const PLFLT *intensity,
                  const PLFLT *coord1, const PLFLT *coord2, const PLFLT *coord3, const PLINT *alt_hue_path );

    void shade( const PLFLT * const *a, PLINT nx, PLINT ny,
                PLDEFINED_callback defined,
                PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
                PLFLT shade_min, PLFLT shade_max,
                PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
                PLINT min_color, PLFLT min_width,
                PLINT max_color, PLFLT max_width,
                PLFILL_callback fill, PLINT rectangular,
                PLTRANSFORM_callback pltr, PLPointer pltr_data );

    void shades( const PLFLT * const *a, PLINT nx, PLINT ny, PLDEFINED_callback defined,
                 PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
                 const PLFLT * clevel, PLINT nlevel, PLFLT fill_width,
                 PLINT cont_color, PLFLT cont_width,
                 PLFILL_callback fill, PLINT rectangular,
                 PLTRANSFORM_callback pltr, PLPointer pltr_data );

    void shade( Contourable_Data& d, PLFLT xmin, PLFLT xmax,
                PLFLT ymin, PLFLT ymax, PLFLT shade_min, PLFLT shade_max,
                PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
                PLINT min_color, PLFLT min_width,
                PLINT max_color, PLFLT max_width,
                PLINT rectangular,
                Coord_Xformer *pcxf );

    void fshade( PLFLT ( *f2eval )( PLINT, PLINT, PLPointer ),
                 PLPointer f2eval_data,
                 PLFLT ( *c2eval )( PLINT, PLINT, PLPointer ),
                 PLPointer c2eval_data,
                 PLINT nx, PLINT ny,
                 PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
                 PLFLT shade_min, PLFLT shade_max,
                 PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
                 PLINT min_color, PLFLT min_width,
                 PLINT max_color, PLFLT max_width,
                 PLFILL_callback fill, PLINT rectangular,
                 PLTRANSFORM_callback pltr, PLPointer pltr_data );

    void spause( PLINT pause );

    void stripc( PLINT *id, const char *xspec, const char *yspec,
                 PLFLT xmin, PLFLT xmax, PLFLT xjump, PLFLT ymin, PLFLT ymax,
                 PLFLT xlpos, PLFLT ylpos,
                 PLINT y_ascl, PLINT acc,
                 PLINT colbox, PLINT collab,
                 const PLINT colline[], const PLINT styline[], const char *legline[],
                 const char *labx, const char *laby, const char *labtop );

    void xormod( PLINT mode, PLINT *status );
#endif //PL_DEPRECATED
};



#endif                          // __plstream_h__

//--------------------------------------------------------------------------
//                              end of plstream.h
//--------------------------------------------------------------------------
