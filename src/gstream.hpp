//----------------------------------*-C++-*----------------------------------//
// $Id: gstream.hpp,v 1.1.1.1 2004-12-09 15:10:15 m_schellens Exp $
// Geoffrey Furnish
// Sep 21 1994
//
// slightly modified for GDL by Marc Schellens
// Copyright (C) 2004  Andrew Ross
// Copyright (C) 2004  Alan W. Irwin
//
// This file was part of PLplot.
//
// PLplot is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Library Public License as published
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

//---------------------------------------------------------------------------//
// @> Header file plstream.
//---------------------------------------------------------------------------//

#ifndef __plstream_h__
#define __plstream_h__

// added for GDL
#include <plplot/plxwd.h>

// changed for GDL
// #include "plplot.h"
#include <plplot/plplot.h>


class PLS {
  public:
    enum stream_id { Next, Current, Specific };
};

enum PLcolor { Black=0, Red, Yellow, Green,
	       Cyan, Pink, Tan, Grey,
	       DarkRed, DeepBlue, Purple, LightCyan,
	       LightBlue, Orchid, Mauve, White };

// A class for assisting in generalizing the data prescription
// interface to the contouring routines.

class Contourable_Data {
    int _nx, _ny;
  public:
    Contourable_Data( int nx, int ny ) : _nx(nx), _ny(ny) {}
    virtual void elements( int& nx, int& ny ) const { nx = _nx; ny=_ny; }
    virtual PLFLT operator()( int i, int j ) const =0;
};

PLFLT Contourable_Data_evaluator( PLINT i, PLINT j, PLPointer p );

class Coord_Xformer {
  public:
    virtual void xform( PLFLT ox, PLFLT oy, PLFLT& nx, PLFLT& ny ) const =0;
};

void Coord_Xform_evaluator( PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer );

class Coord_2d {
  public:
    virtual PLFLT operator() ( int ix, int iy ) const =0;
    virtual void elements( int& _nx, int& _ny ) =0;
    virtual void min_max( PLFLT& _min, PLFLT& _max ) =0;
};

class cxx_pltr2 : public Coord_Xformer {
    Coord_2d& xg;
    Coord_2d& yg;
  public:
    cxx_pltr2( Coord_2d& cx, Coord_2d& cy );
    void xform( PLFLT x, PLFLT y, PLFLT& tx, PLFLT& ty ) const;
};

//===========================================================================//
// class plstream - C++ class for encapsulating PLplot streams

// Cool stuff.
//===========================================================================//

class plstream {

    int stream;

    static int next_stream;
    static int active_streams;

  private:
    // These have to be disabled till we implement reference counting.

    plstream( const plstream& );
    plstream& operator=( const plstream& );

    void set_stream() { ::c_plsstrm(stream); }

  public:
    plstream();
    plstream( plstream * pls );
    plstream( PLS::stream_id sid, int strm =0 );
    plstream( int _stream ) : stream(_stream) {}
    plstream( int nx /*=1*/, int ny /*=1*/,
	      const char *driver =NULL, const char *file =NULL );
    plstream( int nx /*=1*/, int ny /*=1*/, int r, int g, int b,
	      const char *driver =NULL, const char *file =NULL );

    ~plstream();

// Now start miroring the PLplot C API.

	/* C routines callable from stub routines come first */

// Advance to subpage "page", or to the next one if "page" = 0.

    void adv( PLINT page );

// Simple arrow plotter
    void arrows( PLFLT *u, PLFLT *v, PLFLT *x, PLFLT *y, PLINT n,
                 PLFLT scale, PLFLT dx, PLFLT dy );

// This functions similarly to plbox() except that the origin of the axes is
// placed at the user-specified point (x0, y0).

    void axes( PLFLT x0, PLFLT y0, const char *xopt, PLFLT xtick, PLINT nxsub,
	       const char *yopt, PLFLT ytick, PLINT nysub );

// Plot a histogram using x to store data values and y to store frequencies.

    void bin( PLINT nbin, PLFLT *x, PLFLT *y, PLINT center );

// Start new page.  Should only be used with pleop().

    void bop();

// This draws a box around the current viewport.

    void box( const char *xopt, PLFLT xtick, PLINT nxsub,
	      const char *yopt, PLFLT ytick, PLINT nysub );

// This is the 3-d analogue of plbox().

    void box3( const char *xopt, const char *xlabel, PLFLT xtick, PLINT nsubx,
	       const char *yopt, const char *ylabel, PLFLT ytick, PLINT nsuby,
	       const char *zopt, const char *zlabel, PLFLT ztick, PLINT nsubz );

// Set color, map 0.  Argument is integer between 0 and 15.

    void col0( PLINT icol0 );

// Set the color using a descriptive name.  Replaces plcol0().  (Except that
// col0 won't die.)

    void col( PLcolor c );

// Set color, map 1.  Argument is a float between 0. and 1.

    void col( PLFLT c );

// Draws a contour plot from data in f(nx,ny).  Is just a front-end to
// plfcont, with a particular choice for f2eval and f2eval_data.

    void cont( PLFLT **f, PLINT nx, PLINT ny, PLINT kx, PLINT lx,
	       PLINT ky, PLINT ly, PLFLT *clevel, PLINT nlevel,
	       void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
	       PLPointer pltr_data );

/* Draws a contour plot using the function evaluator f2eval and data stored
 * by way of the f2eval_data pointer.  This allows arbitrary organizations
 * of 2d array data to be used.
 */

    void fcont( PLFLT (*f2eval) (PLINT, PLINT, PLPointer),
		PLPointer f2eval_data,
		PLINT nx, PLINT ny, PLINT kx, PLINT lx,
		PLINT ky, PLINT ly, PLFLT *clevel, PLINT nlevel,
		void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
		PLPointer pltr_data );

// /* Copies state parameters from the reference stream to the current stream. */

//     void cpstrm( PLINT iplsr, PLINT flags );
     void cpstrm( plstream &pls, PLINT flags );

// Converts input values from relative device coordinates to relative plot
// coordinates.

    void did2pc( PLFLT& xmin, PLFLT& ymin, PLFLT& xmax, PLFLT& ymax );

// Converts input values from relative plot coordinates to relative device
// coordinates.

    void dip2dc( PLFLT& xmin, PLFLT& ymin, PLFLT& xmax, PLFLT& ymax );

// These shouldn't be needed, are supposed to be handled by ctor/dtor
// semantics of the plstream object.

// /* End a plotting session for all open streams. */

//     void end();

// /* End a plotting session for the current stream only. */

//     void end1();

// Simple interface for defining viewport and window.

    void env( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
	      PLINT just, PLINT axis );

// similar to env() above, but in multiplot mode does not advance
// the subpage, instead the current subpage is cleared

    void env0( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
	      PLINT just, PLINT axis );

// End current page.  Should only be used with plbop().

    void eop();

// Plot horizontal error bars (xmin(i),y(i)) to (xmax(i),y(i)).

    void errx( PLINT n, PLFLT *xmin, PLFLT *xmax, PLFLT *y );

// Plot vertical error bars (x,ymin(i)) to (x(i),ymax(i)).

    void erry( PLINT n, PLFLT *x, PLFLT *ymin, PLFLT *ymax );

// Advance to the next family file on the next new page.

    void famadv();

// Pattern fills the polygon bounded by the input points.

    void fill( PLINT n, PLFLT *x, PLFLT *y );

// Flushes the output stream.  Use sparingly, if at all.

    void flush();

// Sets the global font flag to 'ifont'.

    void font( PLINT ifont );

// Load specified font set.

    void fontld( PLINT fnt );

// Get character default height and current (scaled) height.

    void gchr( PLFLT& p_def, PLFLT& p_ht );

// Returns 8 bit RGB values for given color from color map 0.

    void gcol0( PLINT icol0, PLINT& r, PLINT& g, PLINT& b );

// Returns the background color by 8 bit RGB value.

    void gcolbg( PLINT& r, PLINT& g, PLINT& b );

// Retrieve current window into device space.

    void gdidev( PLFLT& mar, PLFLT& aspect, PLFLT& jx, PLFLT& jy );

// Get plot orientation.

    void gdiori( PLFLT& rot );

// Retrieve current window into plot space.

    void gdiplt( PLFLT& xmin, PLFLT& ymin, PLFLT& xmax, PLFLT& ymax );

// Get family file parameters.

    void gfam( PLINT& fam, PLINT& num, PLINT& bmax );

// Get the (current) output file name.  Must be preallocated to >80 bytes.

    void gfnam( char *fnam );

/* Get the (current) run level.  */

    void glevel( PLINT &p_level );

// Get output device parameters.

    void gpage( PLFLT& xp, PLFLT& yp, PLINT& xleng, PLINT& yleng,
		PLINT& xoff, PLINT& yoff );

// Switches to graphics screen.

    void gra();

// grid irregularly sampled data

    void griddata(PLFLT *x, PLFLT *y, PLFLT *z, int npts,
                  PLFLT *xg, int nptsx, PLFLT *yg,  int nptsy,
                  PLFLT **zg, int type, PLFLT data);

// Get subpage boundaries in absolute coordinates.

    void gspa( PLFLT& xmin, PLFLT& xmax, PLFLT& ymin, PLFLT& ymax );

// This shouldn't be needed in this model.

// /* Get current stream number. */

//     void gstrm( PLINT *p_strm );

// Get the current library version number.

    void gver( char *p_ver );

// Get x axis labeling parameters.

    void gxax( PLINT& digmax, PLINT& digits );

// Get y axis labeling parameters.

    void gyax( PLINT& digmax, PLINT& digits );

/* Get z axis labeling parameters */

    void gzax( PLINT *p_digmax, PLINT *p_digits );

/* Draws a histogram of n values of a variable in array data[0..n-1] */

    void hist(PLINT n, PLFLT *data, PLFLT datmin, PLFLT datmax,
	      PLINT nbin, PLINT oldwin);

/* Set current color (map 0) by hue, lightness, and saturation. */

    void hls( PLFLT h, PLFLT l, PLFLT s );

/* Initializes PLplot, using preset or default options */

    void init();

/* Draws a line segment from (x1, y1) to (x2, y2). */

    void join( PLFLT x1, PLFLT y1, PLFLT x2, PLFLT y2 );

/* Simple routine for labelling graphs. */

    void lab( const char *xlabel, const char *ylabel, const char *tlabel );

/* Sets position of the light source */

   void lightsource( PLFLT x, PLFLT y, PLFLT z );

/* Draws line segments connecting a series of points. */

    void line( PLINT n, PLFLT *x, PLFLT *y );

/* Draws a line in 3 space.  */

    void line3( PLINT n, PLFLT *x, PLFLT *y, PLFLT *z );

/* Set line style. */

    void lsty( PLINT lin );

/* plot continental outline in world coordinates */

    void map( void (*mapform)(PLINT, PLFLT *, PLFLT *), char *type,
	      PLFLT minlong, PLFLT maxlong, PLFLT minlat, PLFLT maxlat );

/* Plot the latitudes and longitudes on the background. */

    void  meridians( void (*mapform)(PLINT, PLFLT *, PLFLT *),
		     PLFLT dlong, PLFLT dlat, PLFLT minlong, PLFLT maxlong,
		     PLFLT minlat, PLFLT maxlat );

/* Plots a mesh representation of the function z[x][y]. */

    void mesh( PLFLT *x, PLFLT *y, PLFLT **z, PLINT nx, PLINT ny, PLINT opt );

/* Plots a mesh representation of the function z[x][y] with contour. */

    void meshc( PLFLT *x, PLFLT *y, PLFLT **z, PLINT nx, PLINT ny, PLINT opt,
    		    PLFLT *clevel, PLINT nlevel);

// /* Creates a new stream and makes it the default.  */

// void
// mkstrm(PLINT *p_strm);

/* Prints out "text" at specified position relative to viewport */

    void mtex( const char *side, PLFLT disp, PLFLT pos, PLFLT just,
	       const char *text );

/* Plots a 3-d representation of the function z[x][y]. */

    void plot3d( PLFLT *x, PLFLT *y, PLFLT **z,
		 PLINT nx, PLINT ny, PLINT opt, PLINT side );

/* Plots a 3-d representation of the function z[x][y] with contour. */

    void plot3dc(PLFLT *x, PLFLT *y, PLFLT **z,
                 PLINT nx, PLINT ny, PLINT opt,
                 PLFLT *clevel, PLINT nlevel);

/* Plots a 3-d shaded representation of the function z[x][y]. */

    void surf3d( PLFLT *x, PLFLT *y, PLFLT **z,
		 PLINT nx, PLINT ny, PLINT opt,
		 PLFLT *clevel, PLINT nlevel);

/* Set fill pattern directly. */

void pat( PLINT nlin, PLINT *inc, PLINT *del );

/* Plots array y against x for n points using ASCII code "code".*/

    void poin( PLINT n, PLFLT *x, PLFLT *y, PLINT code );

/* Draws a series of points in 3 space. */

    void poin3( PLINT n, PLFLT *x, PLFLT *y, PLFLT *z, PLINT code );

/* Draws a polygon in 3 space.  */

    void poly3( PLINT n, PLFLT *x, PLFLT *y, PLFLT *z, PLINT *draw, PLINT ifcc );

/* Set the floating point precision (in number of places) in numeric labels. */

    void prec( PLINT setp, PLINT prec );

/* Set fill pattern, using one of the predefined patterns.*/

    void psty( PLINT patt );

/* Prints out "text" at world cooordinate (x,y). */

    void ptex( PLFLT x, PLFLT y, PLFLT dx, PLFLT dy, PLFLT just,
	       const char *text );

/* Replays contents of plot buffer to current device/file. */

    void replot();

/* Set line color by red, green, blue from  0. to 1. */

    void rgb( PLFLT r, PLFLT g, PLFLT b );

/* Set line color by 8 bit RGB values. */

    void rgb( PLINT r, PLINT g, PLINT b );

/* Set character height. */

    void schr( PLFLT def, PLFLT scale );

/* Set number of colors in cmap 0 */

    void scmap0n( PLINT ncol0 );

/* Set number of colors in cmap 1 */

    void scmap1n( PLINT ncol1 );

/* Set color map 0 colors by 8 bit RGB values */

    void scmap0( PLINT *r, PLINT *g, PLINT *b, PLINT ncol0 );

/* Set color map 1 colors by 8 bit RGB values */

    void scmap1( PLINT *r, PLINT *g, PLINT *b, PLINT ncol1 );

/* Set color map 1 colors using a piece-wise linear relationship between */
/* intensity [0,1] (cmap 1 index) and position in HLS or RGB color space. */

    void scmap1l( PLINT itype, PLINT npts, PLFLT *intensity,
		  PLFLT *coord1, PLFLT *coord2, PLFLT *coord3, PLINT *rev );

/* Set a given color from color map 0 by 8 bit RGB value */

    void scol0( PLINT icol0, PLINT r, PLINT g, PLINT b );

/* Set the background color by 8 bit RGB value */

    void scolbg( PLINT r, PLINT g, PLINT b );

/* Used to globally turn color output on/off */

    void scolor( PLINT color );

/* Set the device (keyword) name */

    void sdev( const char *devname );

/* Get the device (keyword) name */

    void gdev( char *devname );

/* Set window into device space using margin, aspect ratio, and */
/* justification */

    void sdidev( PLFLT mar, PLFLT aspect, PLFLT jx, PLFLT jy );

/* Set up transformation from metafile coordinates. */

    void sdimap( PLINT dimxmin, PLINT dimxmax,
		 PLINT dimymin, PLINT dimymax,
		 PLFLT dimxpmm, PLFLT dimypmm);

/* Set plot orientation, specifying rotation in units of pi/2. */

    void sdiori( PLFLT rot );

/* Set window into plot space */

    void sdiplt( PLFLT xmin, PLFLT ymin, PLFLT xmax, PLFLT ymax );

/* Set window into plot space incrementally (zoom) */

    void sdiplz( PLFLT xmin, PLFLT ymin, PLFLT xmax, PLFLT ymax );

/* Set the escape character for text strings. */

    void sesc( char esc );

/* Set offset and spacing of contour labels */

    void setcontlabelparam( PLFLT offset, PLFLT size, PLFLT spacing, PLINT active);

/* Set the format of the contour labels */

    void setcontlabelformat( PLINT lexp, PLINT sigdig);

/* Set family file parameters */

    void sfam( PLINT fam, PLINT num, PLINT bmax );

/* Set the output file name. */

    void sfnam( const char *fnam );

/* Shade region. */

    void shade( PLFLT **a, PLINT nx, PLINT ny,
		PLINT (*defined) (PLFLT, PLFLT),
		PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
		PLFLT shade_min, PLFLT shade_max,
		PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
		PLINT min_color, PLINT min_width,
		PLINT max_color, PLINT max_width,
		void (*fill) (PLINT, PLFLT *, PLFLT *), PLINT rectangular,
		void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
		PLPointer pltr_data );

    void shades( PLFLT **a, PLINT nx, PLINT ny, PLINT (*defined) (PLFLT, PLFLT),
              	PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
		PLFLT *clevel, PLINT nlevel, PLINT fill_width,
                PLINT cont_color, PLINT cont_width,
                void (*fill) (PLINT, PLFLT *, PLFLT *), PLINT rectangular,
                void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
                PLPointer pltr_data);

// Would be nice to fix this even more, say by stuffing xmin, xmax,
// ymin, ymax, rectangular, and pcxf all into the contourable data
// class.  Have to think more on that.  Or maybe the coordinate info.

    void shade( Contourable_Data& d, PLFLT xmin, PLFLT xmax,
		PLFLT ymin, PLFLT ymax, PLFLT shade_min, PLFLT shade_max,
		PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
		PLINT min_color, PLINT min_width,
		PLINT max_color, PLINT max_width,
		PLINT rectangular,
		Coord_Xformer *pcxf );

    void  shade1( PLFLT *a, PLINT nx, PLINT ny,
		  PLINT (*defined) (PLFLT, PLFLT),
		  PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
		  PLFLT shade_min, PLFLT shade_max,
		  PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
		  PLINT min_color, PLINT min_width,
		  PLINT max_color, PLINT max_width,
		  void (*fill) (PLINT, PLFLT *, PLFLT *), PLINT rectangular,
		  void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
		  PLPointer pltr_data);

    void fshade( PLFLT (*f2eval) (PLINT, PLINT, PLPointer),
		 PLPointer f2eval_data,
		 PLFLT (*c2eval) (PLINT, PLINT, PLPointer),
		 PLPointer c2eval_data,
		 PLINT nx, PLINT ny,
		 PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
		 PLFLT shade_min, PLFLT shade_max,
		 PLINT sh_cmap, PLFLT sh_color, PLINT sh_width,
		 PLINT min_color, PLINT min_width,
		 PLINT max_color, PLINT max_width,
		 void (*fill) (PLINT, PLFLT *, PLFLT *), PLINT rectangular,
		 void (*pltr) (PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer),
		 PLPointer pltr_data );

/* Set up lengths of major tick marks. */

    void smaj( PLFLT def, PLFLT scale );

/* Set up lengths of minor tick marks. */

    void smin( PLFLT def, PLFLT scale );

/* Set orientation.  Must be done before calling plinit. */

    void sori( PLINT ori );

/* Set output device parameters.  Usually ignored by the driver. */

void spage( PLFLT xp, PLFLT yp, PLINT xleng, PLINT yleng,
	    PLINT xoff, PLINT yoff );

/* Set the pause (on end-of-page) status */

    void spause( PLINT pause );

/* Set stream number.  */

    void sstrm( PLINT strm );

/* Set the number of subwindows in x and y */

    void ssub( PLINT nx, PLINT ny );

/* Set symbol height. */

    void ssym( PLFLT def, PLFLT scale );

/* Initialize PLplot, passing in the windows/page settings. */

    void star( PLINT nx, PLINT ny );

/* Initialize PLplot, passing the device name and windows/page settings. */

    void start( const char *devname, PLINT nx, PLINT ny );

/* Create 1d stripchart */

    void stripc(PLINT *id, char *xspec, char *yspec,
        PLFLT xmin, PLFLT xmax, PLFLT xjump, PLFLT ymin, PLFLT ymax,
        PLFLT xlpos, PLFLT ylpos,
        PLINT y_ascl, PLINT acc,
        PLINT colbox, PLINT collab,
        PLINT colline[], PLINT styline[], char *legline[],
        char *labx, char *laby, char *labtop);

/* Add a point to a stripchart.  */

    void stripa(PLINT id, PLINT pen, PLFLT x, PLFLT y);

/* Deletes and releases memory used by a stripchart.  */

    void stripd(PLINT id);

/* plots a 2d image (or a matrix too large for plshade() ) */

    void image( PLFLT **data, PLINT nx, PLINT ny,
           PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
           PLFLT Dxmin, PLFLT Dxmax, PLFLT Dymin, PLFLT Dymax);

/* Set up a new line style */

    void styl( PLINT nms, PLINT *mark, PLINT *space );

/* Sets the edges of the viewport to the specified absolute coordinates */

    void svpa( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );

/* Set x axis labeling parameters */

    void sxax( PLINT digmax, PLINT digits );

/* Set inferior X window */

    void sxwin( PLINT window_id );

/* Set y axis labeling parameters */

    void syax( PLINT digmax, PLINT digits );

/* Plots array y against x for n points using Hershey symbol "code" */

    void sym( PLINT n, PLFLT *x, PLFLT *y, PLINT code );

/* Set z axis labeling parameters */

    void szax( PLINT digmax, PLINT digits );

/* Switches to text screen. */

    void text();

/* Sets the edges of the viewport with the given aspect ratio, leaving */
/* room for labels. */

    void vasp( PLFLT aspect );

/* Creates the largest viewport of the specified aspect ratio that fits */
/* within the specified normalized subpage coordinates. */

    void vpas( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT aspect );

/* Creates a viewport with the specified normalized subpage coordinates. */

    void vpor( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );

/* Defines a "standard" viewport with seven character heights for */
/* the left margin and four character heights everywhere else. */

    void vsta();

/* Set up a window for three-dimensional plotting. */

    void w3d( PLFLT basex, PLFLT basey, PLFLT height, PLFLT xmin0,
	      PLFLT xmax0, PLFLT ymin0, PLFLT ymax0, PLFLT zmin0,
	      PLFLT zmax0, PLFLT alt, PLFLT az);

/* Set pen width. */

    void wid( PLINT width );

/* Set up world coordinates of the viewport boundaries (2d plots). */

    void wind( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );

/* set xor mode; mode = 1-enter, 0-leave, status = 0 if not interactive device */
    void xormod(PLINT mode, PLINT *status);

	/* The rest for use from C only */

/* Returns a list of file-oriented device names and their menu strings */

    void gFileDevs( char ***p_menustr, char ***p_devname, int *p_ndev );

/* Set the function pointer for the keyboard event handler */

    void sKeyEH( void (*KeyEH) (PLGraphicsIn *, void *, int *),
		 void *KeyEH_data );

/* Sets an optional user bop handler */

    void sbopH(void (*handler) (void *, int *), void *handlier_data);

/* Sets an optional user eop handler */

    void seopH(void (*handler) (void *, int *), void *handlier_data);

/* Set the variables to be used for storing error info */

    void sError(PLINT *errcode, char *errmsg);

/* Sets an optional user exit handler. */

    void sexit( int (*handler) (char *) );

	/* Transformation routines */

/* Identity transformation. */

    void tr0( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );

/* Does linear interpolation from singly dimensioned coord arrays. */

    void tr1( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );

/* Does linear interpolation from doubly dimensioned coord arrays */
/* (column dominant, as per normal C 2d arrays). */

    void tr2( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );

/* Just like pltr2() but uses pointer arithmetic to get coordinates from */
/* 2d grid tables.  */

    void tr2p( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data );

// We obviously won't be using this object from Fortran...

// /* Identity transformation for plots from Fortran. */

//     void tr0f( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, void *pltr_data );

// /* Does linear interpolation from doubly dimensioned coord arrays */
// /* (row dominant, i.e. Fortran ordering). */

//     void tr2f( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, void *pltr_data );

/* Example linear transformation function for contour plotter. */

    void  xform( PLFLT x, PLFLT y, PLFLT * tx, PLFLT * ty );

	/* Function evaluators */

/* Does a lookup from a 2d function array.  Array is of type (PLFLT **), */
/* and is column dominant (normal C ordering). */

    PLFLT f2eval2( PLINT ix, PLINT iy, PLPointer plf2eval_data );

/* Does a lookup from a 2d function array.  Array is of type (PLFLT *), */
/* and is column dominant (normal C ordering). */

    PLFLT f2eval( PLINT ix, PLINT iy, PLPointer plf2eval_data );

/* Does a lookup from a 2d function array.  Array is of type (PLFLT *), */
/* and is row dominant (Fortran ordering). */

    PLFLT f2evalr( PLINT ix, PLINT iy, PLPointer plf2eval_data );

	/* Command line parsing utilities */

/* Clear internal option table info structure. */

    void ClearOpts();

/* Reset internal option table info structure. */

    void ResetOpts();

/* Merge user option table into internal info structure. */

    int MergeOpts( PLOptionTable *options, char *name, char **notes );

/* Set the strings used in usage and syntax messages. */

    void SetUsage( char *program_string, char *usage_string );

/* Process input strings, treating them as an option and argument pair. */

    int SetOpt( char *opt, char *optarg );

/* Process options list using current options info. */

    int ParseOpts( int *p_argc, char **argv, PLINT mode );

/* Print usage & syntax message. */

    void OptUsage();

	/* Miscellaneous */

/* Set the output file pointer */

    void gfile( FILE **p_file );

/* Get the output file pointer */

    void sfile( FILE *file );

/* Get the escape character for text strings. */

    void gesc( char *p_esc );

/* Front-end to driver escape function. */

    void cmd( PLINT op, void *ptr );

/* Return full pathname for given file if executable */

    int  FindName( char *p );

/* Looks for the specified executable file according to usual search path. */

    char *FindCommand( char *fn );

/* Gets search name for file by concatenating the dir, subdir, and file */
/* name, allocating memory as needed.  */

    void GetName( char *dir, char *subdir, char *filename, char **filespec );

/* Prompts human to input an integer in response to given message. */

    PLINT GetInt( char *s );

/* Prompts human to input a float in response to given message. */

    PLFLT GetFlt( char *s );

	/* Nice way to allocate space for a vectored 2d grid */

/* Allocates a block of memory for use as a 2-d grid of PLFLT's.  */

    void Alloc2dGrid( PLFLT ***f, PLINT nx, PLINT ny );

/* Frees a block of memory allocated with plAlloc2dGrid(). */

    void Free2dGrid( PLFLT **f, PLINT nx, PLINT ny );

/* Find the maximum and minimum of a 2d matrix allocated with plAllc2dGrid(). */
    void MinMax2dGrid(PLFLT **f, PLINT nx, PLINT ny, PLFLT *fmax, PLFLT *fmin);

/* Functions for converting between HLS and RGB color space */

    void HLS_RGB( PLFLT h, PLFLT l, PLFLT s,
		  PLFLT *p_r, PLFLT *p_g, PLFLT *p_b );

    void RGB_HLS( PLFLT r, PLFLT g, PLFLT b,
		  PLFLT *p_h, PLFLT *p_l, PLFLT *p_s );

/* Wait for graphics input event and translate to world coordinates */

    int GetCursor( PLGraphicsIn *plg );

  // added for GDL
  bool Valid();

};

#endif                          // __plstream_h__

//---------------------------------------------------------------------------//
//                              end of plstream.h
//---------------------------------------------------------------------------//
