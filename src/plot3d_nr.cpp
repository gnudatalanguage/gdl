/* $Id: plot3d_nr.cpp,v 1.7 2010-11-17 07:24:55 slayoo Exp $

	3d plot routines.

   Copyright (C) 2004  Alan W. Irwin
   Copyright (C) 2004  Joao Cardoso
   Copyright (C) 2004  Andrew Ross

   This file is part of PLplot.

   PLplot is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   PLplot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with PLplot; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <plplot/plplotP.h>

namespace lib {

  /* Internal constants */

#define  BINC      50		/* Block size for memory allocation */

  static PLINT pl3mode = 0;	/* 0 3d solid; 1 mesh plot */
  static PLINT pl3upv = 1;	/* 1 update view; 0 no update */
  
  static PLINT zbflg = 0, zbcol, zbwidth;
  static PLFLT zbtck;

  static PLINT *oldhiview = NULL;
  static PLINT *oldloview = NULL;
  static PLINT *newhiview = NULL;
  static PLINT *newloview = NULL;
  static PLINT *utmp = NULL;
  static PLINT *vtmp = NULL;
  static PLFLT *ctmp = NULL;

  static PLINT mhi, xxhi, newhisize;
  static PLINT mlo, xxlo, newlosize;

  /* Light source for shading */
  static PLFLT xlight, ylight, zlight;
  static PLINT falsecolor=0;
  static PLFLT fc_minz, fc_maxz;

  /* Prototypes for static functions */

  static void plgrid3	(PLFLT);
  static void plnxtv (PLINT *, PLINT *, PLFLT*, PLINT, PLINT);
  static void plside3	(PLFLT *, PLFLT *, PLFLT **, PLINT, PLINT, PLINT);
  static void plt3zz	(PLINT, PLINT, PLINT, PLINT,
			 PLINT, PLINT *, PLFLT *, PLFLT *, PLFLT **,
			 PLINT, PLINT, PLINT *, PLINT *, PLFLT*);
  static void plnxtvhi (PLINT *, PLINT *, PLFLT*, PLINT, PLINT);
  static void plnxtvlo (PLINT *, PLINT *, PLFLT*, PLINT, PLINT);
  static void plnxtvhi_draw(PLINT *u, PLINT *v, PLFLT* c, PLINT n);

  static void savehipoint	(PLINT, PLINT);
  static void savelopoint	(PLINT, PLINT);
  static void swaphiview	(void);
  static void swaploview	(void);
  static void myexit	(char *);
  static void myabort	(char *);
  static void freework	(void);
  static int  plabv	(PLINT, PLINT, PLINT, PLINT, PLINT, PLINT);
  static void pl3cut	(PLINT, PLINT, PLINT, PLINT, PLINT,
			 PLINT, PLINT, PLINT, PLINT *, PLINT *);
  static void plP_draw3d(PLINT x, PLINT y, PLFLT *c, PLINT j, PLINT move);

  static void plt3zz_nr (PLINT, PLINT, PLINT, PLINT,
			 PLINT, PLINT *, PLFLT *, PLFLT *, PLFLT **,
			 PLINT, PLINT, PLINT *, PLINT *, PLFLT*);

  void mesh_nr(PLFLT *, PLFLT *, PLFLT **, PLINT, PLINT, PLINT);

  void c_plot3dc_nr(PLFLT *, PLFLT *, PLFLT **,
		    PLINT, PLINT, PLINT,
		    PLFLT *, PLINT);

  void c_plot3dcl_nr(PLFLT *, PLFLT *, PLFLT **,
		     PLINT, PLINT, PLINT, 
		     PLFLT *, PLINT,
		     PLINT, PLINT, PLINT *, PLINT *);

/*--------------------------------------------------------------------------*\
 * void plside3()
 *
 * This routine draws sides around the front of the 3d plot so that
 * it does not appear to float.
\*--------------------------------------------------------------------------*/

static void
plside3(PLFLT *x, PLFLT *y, PLFLT **z, PLINT nx, PLINT ny, PLINT opt)
{
    PLINT i;
    PLFLT cxx, cxy, cyx, cyy, cyz;
    PLFLT xmin, ymin, zmin, xmax, ymax, zmax, zscale;
    PLFLT tx, ty, ux, uy;

    plP_gw3wc(&cxx, &cxy, &cyx, &cyy, &cyz);
    plP_gdom(&xmin, &xmax, &ymin, &ymax);
    plP_grange(&zscale, &zmin, &zmax);

/* Get x, y coordinates of legs and plot */

    if (cxx >= 0.0 && cxy <= 0.0) {
	if (opt != 1) {
	    for (i = 0; i < nx; i++) {
		tx = plP_w3wcx(x[i], y[0], zmin);
		ty = plP_w3wcy(x[i], y[0], zmin);
		ux = plP_w3wcx(x[i], y[0], z[i][0]);
		uy = plP_w3wcy(x[i], y[0], z[i][0]);
		pljoin(tx, ty, ux, uy);
	    }
	}
	if (opt != 2) {
	    for (i = 0; i < ny; i++) {
		tx = plP_w3wcx(x[0], y[i], zmin);
		ty = plP_w3wcy(x[0], y[i], zmin);
		ux = plP_w3wcx(x[0], y[i], z[0][i]);
		uy = plP_w3wcy(x[0], y[i], z[0][i]);
		pljoin(tx, ty, ux, uy);
	    }
	}
    }
    else if (cxx <= 0.0 && cxy <= 0.0) {
	if (opt != 1) {
	    for (i = 0; i < nx; i++) {
		tx = plP_w3wcx(x[i], y[ny - 1], zmin);
		ty = plP_w3wcy(x[i], y[ny - 1], zmin);
		ux = plP_w3wcx(x[i], y[ny - 1], z[i][ny - 1]);
		uy = plP_w3wcy(x[i], y[ny - 1], z[i][ny - 1]);
		pljoin(tx, ty, ux, uy);
	    }
	}
	if (opt != 2) {
	    for (i = 0; i < ny; i++) {
		tx = plP_w3wcx(x[0], y[i], zmin);
		ty = plP_w3wcy(x[0], y[i], zmin);
		ux = plP_w3wcx(x[0], y[i], z[0][i]);
		uy = plP_w3wcy(x[0], y[i], z[0][i]);
		pljoin(tx, ty, ux, uy);
	    }
	}
    }
    else if (cxx <= 0.0 && cxy >= 0.0) {
	if (opt != 1) {
	    for (i = 0; i < nx; i++) {
		tx = plP_w3wcx(x[i], y[ny - 1], zmin);
		ty = plP_w3wcy(x[i], y[ny - 1], zmin);
		ux = plP_w3wcx(x[i], y[ny - 1], z[i][ny - 1]);
		uy = plP_w3wcy(x[i], y[ny - 1], z[i][ny - 1]);
		pljoin(tx, ty, ux, uy);
	    }
	}
	if (opt != 2) {
	    for (i = 0; i < ny; i++) {
		tx = plP_w3wcx(x[nx - 1], y[i], zmin);
		ty = plP_w3wcy(x[nx - 1], y[i], zmin);
		ux = plP_w3wcx(x[nx - 1], y[i], z[nx - 1][i]);
		uy = plP_w3wcy(x[nx - 1], y[i], z[nx - 1][i]);
		pljoin(tx, ty, ux, uy);
	    }
	}
    }
    else if (cxx >= 0.0 && cxy >= 0.0) {
	if (opt != 1) {
	    for (i = 0; i < nx; i++) {
		tx = plP_w3wcx(x[i], y[0], zmin);
		ty = plP_w3wcy(x[i], y[0], zmin);
		ux = plP_w3wcx(x[i], y[0], z[i][0]);
		uy = plP_w3wcy(x[i], y[0], z[i][0]);
		pljoin(tx, ty, ux, uy);
	    }
	}
	if (opt != 2) {
	    for (i = 0; i < ny; i++) {
		tx = plP_w3wcx(x[nx - 1], y[i], zmin);
		ty = plP_w3wcy(x[nx - 1], y[i], zmin);
		ux = plP_w3wcx(x[nx - 1], y[i], z[nx - 1][i]);
		uy = plP_w3wcy(x[nx - 1], y[i], z[nx - 1][i]);
		pljoin(tx, ty, ux, uy);
	    }
	}
    }
}

/*--------------------------------------------------------------------------*\
 * void plgrid3()
 *
 * This routine draws a grid around the back side of the 3d plot with
 * hidden line removal.
\*--------------------------------------------------------------------------*/

static void
plgrid3(PLFLT tick)
{
    PLFLT xmin, ymin, zmin, xmax, ymax, zmax, zscale;
    PLFLT cxx, cxy, cyx, cyy, cyz, zmin_in, zmax_in;
    PLINT u[3], v[3];
    PLINT nsub = 0;
    PLFLT tp;

    plP_gw3wc(&cxx, &cxy, &cyx, &cyy, &cyz);
    plP_gdom(&xmin, &xmax, &ymin, &ymax);
    plP_grange(&zscale, &zmin_in, &zmax_in);
    zmin = (zmax_in > zmin_in) ? zmin_in: zmax_in;
    zmax = (zmax_in > zmin_in) ? zmax_in: zmin_in;

#ifdef HAVE_OLDPLPLOT
    pldtik(zmin, zmax, &tick, &nsub);
#else
    pldtik(zmin, zmax, &tick, &nsub, FALSE);
#endif
    tp = tick * floor(zmin / tick) + tick;
    pl3upv = 0;

    if (cxx >= 0.0 && cxy <= 0.0) {
	while (tp <= zmax) {
	    u[0] = plP_wcpcx(plP_w3wcx(xmin, ymax, tp));
	    v[0] = plP_wcpcy(plP_w3wcy(xmin, ymax, tp));
	    u[1] = plP_wcpcx(plP_w3wcx(xmax, ymax, tp));
	    v[1] = plP_wcpcy(plP_w3wcy(xmax, ymax, tp));
	    u[2] = plP_wcpcx(plP_w3wcx(xmax, ymin, tp));
	    v[2] = plP_wcpcy(plP_w3wcy(xmax, ymin, tp));
	    plnxtv(u, v, 0, 3, 0);

	    tp += tick;
	}
	u[0] = plP_wcpcx(plP_w3wcx(xmax, ymax, zmin));
	v[0] = plP_wcpcy(plP_w3wcy(xmax, ymax, zmin));
	u[1] = plP_wcpcx(plP_w3wcx(xmax, ymax, zmax));
	v[1] = plP_wcpcy(plP_w3wcy(xmax, ymax, zmax));
	plnxtv(u, v, 0, 2, 0);
    }
    else if (cxx <= 0.0 && cxy <= 0.0) {
	while (tp <= zmax) {
	    u[0] = plP_wcpcx(plP_w3wcx(xmax, ymax, tp));
	    v[0] = plP_wcpcy(plP_w3wcy(xmax, ymax, tp));
	    u[1] = plP_wcpcx(plP_w3wcx(xmax, ymin, tp));
	    v[1] = plP_wcpcy(plP_w3wcy(xmax, ymin, tp));
	    u[2] = plP_wcpcx(plP_w3wcx(xmin, ymin, tp));
	    v[2] = plP_wcpcy(plP_w3wcy(xmin, ymin, tp));
	    plnxtv(u, v, 0,3, 0);

	    tp += tick;
	}
	u[0] = plP_wcpcx(plP_w3wcx(xmax, ymin, zmin));
	v[0] = plP_wcpcy(plP_w3wcy(xmax, ymin, zmin));
	u[1] = plP_wcpcx(plP_w3wcx(xmax, ymin, zmax));
	v[1] = plP_wcpcy(plP_w3wcy(xmax, ymin, zmax));
	plnxtv(u, v, 0,2, 0);
    }
    else if (cxx <= 0.0 && cxy >= 0.0) {
	while (tp <= zmax) {
	    u[0] = plP_wcpcx(plP_w3wcx(xmax, ymin, tp));
	    v[0] = plP_wcpcy(plP_w3wcy(xmax, ymin, tp));
	    u[1] = plP_wcpcx(plP_w3wcx(xmin, ymin, tp));
	    v[1] = plP_wcpcy(plP_w3wcy(xmin, ymin, tp));
	    u[2] = plP_wcpcx(plP_w3wcx(xmin, ymax, tp));
	    v[2] = plP_wcpcy(plP_w3wcy(xmin, ymax, tp));
	    plnxtv(u, v, 0,3, 0);

	    tp += tick;
	}
	u[0] = plP_wcpcx(plP_w3wcx(xmin, ymin, zmin));
	v[0] = plP_wcpcy(plP_w3wcy(xmin, ymin, zmin));
	u[1] = plP_wcpcx(plP_w3wcx(xmin, ymin, zmax));
	v[1] = plP_wcpcy(plP_w3wcy(xmin, ymin, zmax));
	plnxtv(u, v, 0,2, 0);
    }
    else if (cxx >= 0.0 && cxy >= 0.0) {
	while (tp <= zmax) {
	    u[0] = plP_wcpcx(plP_w3wcx(xmin, ymin, tp));
	    v[0] = plP_wcpcy(plP_w3wcy(xmin, ymin, tp));
	    u[1] = plP_wcpcx(plP_w3wcx(xmin, ymax, tp));
	    v[1] = plP_wcpcy(plP_w3wcy(xmin, ymax, tp));
	    u[2] = plP_wcpcx(plP_w3wcx(xmax, ymax, tp));
	    v[2] = plP_wcpcy(plP_w3wcy(xmax, ymax, tp));
	    plnxtv(u, v, 0,3, 0);

	    tp += tick;
	}
	u[0] = plP_wcpcx(plP_w3wcx(xmin, ymax, zmin));
	v[0] = plP_wcpcy(plP_w3wcy(xmin, ymax, zmin));
	u[1] = plP_wcpcx(plP_w3wcx(xmin, ymax, zmax));
	v[1] = plP_wcpcy(plP_w3wcy(xmin, ymax, zmax));
	plnxtv(u, v, 0,2, 0);
    }
    pl3upv = 1;
}

/*--------------------------------------------------------------------------*\
 * void plnxtv()
 *
 * Draw the next view of a 3-d plot. The physical coordinates of the
 * points for the next view are placed in the n points of arrays u and
 * v. The silhouette found so far is stored in the heap as a set of m peak
 * points.
 *
 * These routines dynamically allocate memory for hidden line removal.
 * Memory is allocated in blocks of 2*BINC*sizeof(PLINT) bytes.  Large
 * values of BINC give better performance but also allocate more memory
 * than is needed. If your 3D plots are very "spiky" or you are working
 * with very large matrices then you will probably want to increase BINC.
\*--------------------------------------------------------------------------*/

static void
plnxtv(PLINT *u, PLINT *v, PLFLT* c, PLINT n, PLINT init)
{
    plnxtvhi(u, v, c, n, init);

    if (pl3mode)
	plnxtvlo(u, v, c, n, init);
}

/*--------------------------------------------------------------------------*\
 * void plnxtvhi()
 *
 * Draw the top side of the 3-d plot.
\*--------------------------------------------------------------------------*/

static void
plnxtvhi(PLINT *u, PLINT *v, PLFLT* c, PLINT n, PLINT init)
{
  /*
   * For the initial set of points, just display them and store them as the
   * peak points.
   */
  if (init == 1) {
    int i;
    oldhiview = (PLINT *) malloc((size_t) (2 * n * sizeof(PLINT)));
    if ( ! oldhiview)
      myexit((char*)"plnxtvhi: Out of memory.");

    oldhiview[0] = u[0];
    oldhiview[1] = v[0];
    plP_draw3d(u[0], v[0], c, 0, 1);
    for (i = 1; i < n; i++) {
      oldhiview[2 * i] = u[i];
      oldhiview[2 * i + 1] = v[i];
      plP_draw3d(u[i], v[i], c, i, 0);
    }
    mhi = n;
    return;
  }

  /*
   * Otherwise, we need to consider hidden-line removal problem. We scan
   * over the points in both the old (i.e. oldhiview[]) and new (i.e. u[]
   * and v[]) arrays in order of increasing x coordinate.  At each stage, we
   * find the line segment in the other array (if one exists) that straddles
   * the x coordinate of the point. We have to determine if the point lies
   * above or below the line segment, and to check if the below/above status
   * has changed since the last point.
   *
   * If pl3upv = 0 we do not update the view, this is useful for drawing
   * lines on the graph after we are done plotting points.  Hidden line
   * removal is still done, but the view is not updated.
   */
  xxhi = 0;
  if (pl3upv != 0) {
    newhisize = 2 * (mhi + BINC);
    if (newhiview != NULL) {
      newhiview =
	(PLINT *) realloc((void *) newhiview,
			  (size_t) (newhisize * sizeof(PLINT)));
    }
    else {
      newhiview =
	(PLINT *) malloc((size_t) (newhisize * sizeof(PLINT)));
    }
    if ( ! newhiview)
      myexit((char*)"plnxtvhi: Out of memory.");
  }

  /* Do the draw or shading with hidden line removal */

  plnxtvhi_draw(u, v, c, n);

  /* Set oldhiview */

  swaphiview();
}

/*--------------------------------------------------------------------------*\
 * void plnxtvhi_draw()
 *
 * Draw the top side of the 3-d plot.
\*--------------------------------------------------------------------------*/

static void
plnxtvhi_draw(PLINT *u, PLINT *v, PLFLT* c, PLINT n)
{
    PLINT i = 0, j = 0, first = 1;
    PLINT sx1 = 0, sx2 = 0, sy1 = 0, sy2 = 0;
    PLINT su1, su2, sv1, sv2;
    PLINT cx, cy, px, py;
    PLINT seg, ptold, lstold = 0, pthi, pnewhi = 0, newhi, change, ochange = 0;

/*
 * (oldhiview[2*i], oldhiview[2*i]) is the i'th point in the old array
 * (u[j], v[j]) is the j'th point in the new array
 */

/*
 * First attempt at 3d shading.  It works ok for simple plots, but
 * will just not draw faces, or draw them overlapping for very
 * jagged plots
 */

    while (i < mhi || j < n) {

    /*
     * The coordinates of the point under consideration are (px,py).  The
     * line segment joins (sx1,sy1) to (sx2,sy2).  "ptold" is true if the
     * point lies in the old array. We set it by comparing the x coordinates
     * of the i'th old point and the j'th new point, being careful if we
     * have fallen past the edges. Having found the point, load up the point
     * and segment coordinates appropriately.
     */

    ptold = (j >= n || (i < mhi && oldhiview[2 * i] < u[j]));
    if (ptold) {
      px = oldhiview[2 * i];
      py = oldhiview[2 * i + 1];
      seg = j > 0 && j < n;
      if (seg) {
	sx1 = u[j - 1];
	sy1 = v[j - 1];
	sx2 = u[j];
	sy2 = v[j];
      }
    } else {
      px = u[j];
      py = v[j];
      seg = i > 0 && i < mhi;
      if (seg) {
	sx1 = oldhiview[2 * (i - 1)];
	sy1 = oldhiview[2 * (i - 1) + 1];
	sx2 = oldhiview[2 * i];
	sy2 = oldhiview[2 * i + 1];
      }
    }

    /*
     * Now determine if the point is higher than the segment, using the
     * logical function "above". We also need to know if it is the old view
     * or the new view that is higher. "newhi" is set true if the new view
     * is higher than the old.
     */
    if (seg)
      pthi = plabv(px, py, sx1, sy1, sx2, sy2);
    else
      pthi = 1;

    newhi = (ptold && !pthi) || (!ptold && pthi);
    /*
     * The last point and this point lie on different sides of
     * the current silouette
     */
    change = (newhi && !pnewhi) || (!newhi && pnewhi);

    /*
     * There is a new intersection point to put in the peak array if the
     * state of "newhi" changes.
     */
    if (first) {
      plP_draw3d(px, py, c, j, 1);
      first = 0;
      lstold = ptold;
      savehipoint(px, py);
      pthi = 0;
      ochange = 0;
    } else if (change) {

      /*
       * Take care of special cases at end of arrays.  If pl3upv is 0 the
       * endpoints are not connected to the old view.
       */
      if (pl3upv == 0 && ((!ptold && j == 0) || (ptold && i == 0))) {
	plP_draw3d(px, py, c, j, 1);
	lstold = ptold;
	pthi = 0;
	ochange = 0;
      } else if (pl3upv == 0 &&
		 (( ! ptold && i >= mhi) || (ptold && j >= n))) {
	plP_draw3d(px, py, c, j, 1);
	lstold = ptold;
	pthi = 0;
	ochange = 0;
      } else {

	/*
	 * If pl3upv is not 0 then we do want to connect the current line
	 * with the previous view at the endpoints.  Also find intersection
	 * point with old view.
	 */
	if (i == 0) {
	  sx1 = oldhiview[0];
	  sy1 = -1;
	  sx2 = oldhiview[0];
	  sy2 = oldhiview[1];
	} else if (i >= mhi) {
	  sx1 = oldhiview[2 * (mhi - 1)];
	  sy1 = oldhiview[2 * (mhi - 1) + 1];
	  sx2 = oldhiview[2 * (mhi - 1)];
	  sy2 = -1;
	} else {
	  sx1 = oldhiview[2 * (i - 1)];
	  sy1 = oldhiview[2 * (i - 1) + 1];
	  sx2 = oldhiview[2 * i];
	  sy2 = oldhiview[2 * i + 1];
	}

	if (j == 0) {
	  su1 = u[0];
	  sv1 = -1;
	  su2 = u[0];
	  sv2 = v[0];
	} else if (j >= n) {
	  su1 = u[n - 1];
	  sv1 = v[n - 1];
	  su2 = u[n - 1];
	  sv2 = -1;
	} else {
	  su1 = u[j - 1];
	  sv1 = v[j - 1];
	  su2 = u[j];
	  sv2 = v[j];
	}

	/* Determine the intersection */

	pl3cut(sx1, sy1, sx2, sy2, su1, sv1, su2, sv2, &cx, &cy);
	if (cx == px && cy == py) {
	  if (lstold && !ochange)
	    plP_draw3d(px, py, c, j, 1);
	  else
	    plP_draw3d(px, py, c, j, 0);

	  savehipoint(px, py);
	  lstold = 1;
	  pthi = 0;
	} else {
	  if (lstold && !ochange)
	    plP_draw3d(cx, cy, c, j, 1);
	  else
	    plP_draw3d(cx, cy, c, j, 0);

	  lstold = 1;
	  savehipoint(cx, cy);
	}
	ochange = 1;
      }
    }

    /* If point is high then draw plot to point and update view. */

    if (pthi) {
      if (lstold && ptold)
	plP_draw3d(px, py, c, j, 1);
      else
	plP_draw3d(px, py, c, j, 0);

      savehipoint(px, py);
      lstold = ptold;
      ochange = 0;
    }
    pnewhi = newhi;

    if (ptold)
      i++;
    else
      j++;
  }
}

/*--------------------------------------------------------------------------*\
 * void  plP_draw3d()
 *
 * Does a simple move or line draw.
\*--------------------------------------------------------------------------*/

static void
plP_draw3d(PLINT x, PLINT y, PLFLT *c, PLINT j, PLINT move)
{
    if (move)
      plP_movphy(x, y);
    else {
      if (c != NULL)
	plcol1(c[j-1]);
      plP_draphy(x, y);
    }
}

/*--------------------------------------------------------------------------*\
 * void plnxtvlo()
 *
 * Draw the bottom side of the 3-d plot.
\*--------------------------------------------------------------------------*/

static void
plnxtvlo(PLINT *u, PLINT *v, PLFLT*c, PLINT n, PLINT init)
{
  PLINT i, j, first;
  PLINT sx1 = 0, sx2 = 0, sy1 = 0, sy2 = 0;
  PLINT su1, su2, sv1, sv2;
  PLINT cx, cy, px, py;
  PLINT seg, ptold, lstold = 0, ptlo, pnewlo, newlo, change, ochange = 0;

  first = 1;
  pnewlo = 0;

  /*
   * For the initial set of points, just display them and store them as the
   * peak points.
   */
  if (init == 1) {

    oldloview = (PLINT *) malloc((size_t) (2 * n * sizeof(PLINT)));
    if ( ! oldloview)
      myexit((char*)"\nplnxtvlo: Out of memory.");

    plP_draw3d(u[0], v[0], c, 0, 1);
    oldloview[0] = u[0];
    oldloview[1] = v[0];
    for (i = 1; i < n; i++) {
      plP_draw3d(u[i], v[i], c, i, 0);
      oldloview[2 * i] = u[i];
      oldloview[2 * i + 1] = v[i];
    }
    mlo = n;
    return;
  }

  /*
   * Otherwise, we need to consider hidden-line removal problem. We scan
   * over the points in both the old (i.e. oldloview[]) and new (i.e. u[]
   * and v[]) arrays in order of increasing x coordinate.  At each stage, we
   * find the line segment in the other array (if one exists) that straddles
   * the x coordinate of the point. We have to determine if the point lies
   * above or below the line segment, and to check if the below/above status
   * has changed since the last point.
   *
   * If pl3upv = 0 we do not update the view, this is useful for drawing
   * lines on the graph after we are done plotting points.  Hidden line
   * removal is still done, but the view is not updated.
   */
  xxlo = 0;
  i = 0;
  j = 0;
  if (pl3upv != 0) {
    newlosize = 2 * (mlo + BINC);
    if (newloview != NULL) {
      newloview =
	(PLINT *) realloc((void *) newloview,
			  (size_t) (newlosize * sizeof(PLINT)));
    }
    else {
      newloview =
	(PLINT *) malloc((size_t) (newlosize * sizeof(PLINT)));
    }
    if ( ! newloview)
      myexit((char*)"plnxtvlo: Out of memory.");
  }

  /*
   * (oldloview[2*i], oldloview[2*i]) is the i'th point in the old array
   * (u[j], v[j]) is the j'th point in the new array.
   */
  while (i < mlo || j < n) {

    /*
     * The coordinates of the point under consideration are (px,py).  The
     * line segment joins (sx1,sy1) to (sx2,sy2).  "ptold" is true if the
     * point lies in the old array. We set it by comparing the x coordinates
     * of the i'th old point and the j'th new point, being careful if we
     * have fallen past the edges. Having found the point, load up the point
     * and segment coordinates appropriately.
     */
    ptold = (j >= n || (i < mlo && oldloview[2 * i] < u[j]));
    if (ptold) {
      px = oldloview[2 * i];
      py = oldloview[2 * i + 1];
      seg = j > 0 && j < n;
      if (seg) {
	sx1 = u[j - 1];
	sy1 = v[j - 1];
	sx2 = u[j];
	sy2 = v[j];
      }
    }
    else {
      px = u[j];
      py = v[j];
      seg = i > 0 && i < mlo;
      if (seg) {
	sx1 = oldloview[2 * (i - 1)];
	sy1 = oldloview[2 * (i - 1) + 1];
	sx2 = oldloview[2 * i];
	sy2 = oldloview[2 * i + 1];
      }
    }

    /*
     * Now determine if the point is lower than the segment, using the
     * logical function "above". We also need to know if it is the old view
     * or the new view that is lower. "newlo" is set true if the new view is
     * lower than the old.
     */
    if (seg)
      ptlo = !plabv(px, py, sx1, sy1, sx2, sy2);
    else
      ptlo = 1;

    newlo = (ptold && !ptlo) || (!ptold && ptlo);
    change = (newlo && !pnewlo) || (!newlo && pnewlo);

    /*
     * There is a new intersection point to put in the peak array if the
     * state of "newlo" changes.
     */
    if (first) {
      plP_draw3d(px, py, c, j, 1);
      first = 0;
      lstold = ptold;
      savelopoint(px, py);
      ptlo = 0;
      ochange = 0;
    }
    else if (change) {

      /*
       * Take care of special cases at end of arrays.  If pl3upv is 0 the
       * endpoints are not connected to the old view.
       */
      if (pl3upv == 0 && ((!ptold && j == 0) || (ptold && i == 0))) {
	plP_draw3d(px, py, c, j, 1);
	lstold = ptold;
	ptlo = 0;
	ochange = 0;
      }
      else if (pl3upv == 0 &&
	       (( ! ptold && i >= mlo) || (ptold && j >= n))) {
	plP_draw3d(px, py, c, j, 1);
	lstold = ptold;
	ptlo = 0;
	ochange = 0;
      }

      /*
       * If pl3upv is not 0 then we do want to connect the current line
       * with the previous view at the endpoints.  Also find intersection
       * point with old view.
       */
      else {
	if (i == 0) {
	  sx1 = oldloview[0];
	  sy1 = 100000;
	  sx2 = oldloview[0];
	  sy2 = oldloview[1];
	}
	else if (i >= mlo) {
	  sx1 = oldloview[2 * (mlo - 1)];
	  sy1 = oldloview[2 * (mlo - 1) + 1];
	  sx2 = oldloview[2 * (mlo - 1)];
	  sy2 = 100000;
	}
	else {
	  sx1 = oldloview[2 * (i - 1)];
	  sy1 = oldloview[2 * (i - 1) + 1];
	  sx2 = oldloview[2 * i];
	  sy2 = oldloview[2 * i + 1];
	}

	if (j == 0) {
	  su1 = u[0];
	  sv1 = 100000;
	  su2 = u[0];
	  sv2 = v[0];
	}
	else if (j >= n) {
	  su1 = u[n - 1];
	  sv1 = v[n - 1];
	  su2 = u[n];
	  sv2 = 100000;
	}
	else {
	  su1 = u[j - 1];
	  sv1 = v[j - 1];
	  su2 = u[j];
	  sv2 = v[j];
	}

	/* Determine the intersection */

	pl3cut(sx1, sy1, sx2, sy2, su1, sv1, su2, sv2, &cx, &cy);
	if (cx == px && cy == py) {
	  if (lstold && !ochange)
	    plP_draw3d(px, py, c, j, 1);
	  else
	    plP_draw3d(px, py, c, j, 0);

	  savelopoint(px, py);
	  lstold = 1;
	  ptlo = 0;
	}
	else {
	  if (lstold && !ochange)
	    plP_draw3d(cx, cy, c, j, 1);
	  else
	    plP_draw3d(cx, cy, c, j, 0);

	  lstold = 1;
	  savelopoint(cx, cy);
	}
	ochange = 1;
      }
    }

    /* If point is low then draw plot to point and update view. */

    if (ptlo) {
      if (lstold && ptold)
	plP_draw3d(px, py, c, j, 1);
      else
	plP_draw3d(px, py, c, j, 0);

      savelopoint(px, py);
      lstold = ptold;
      ochange = 0;
    }

    pnewlo = newlo;

    if (ptold)
      i = i + 1;
    else
      j = j + 1;
  }

  /* Set oldloview */

  swaploview();
}

/*--------------------------------------------------------------------------*\
 * savehipoint
 * savelopoint
 *
 * Add a point to the list of currently visible peaks/valleys, when
 * drawing the top/bottom surface, respectively.
\*--------------------------------------------------------------------------*/

static void
savehipoint(PLINT px, PLINT py)
{
    if (pl3upv == 0)
	return;

    if (xxhi >= newhisize) {	/* allocate additional space */
	newhisize += 2 * BINC;
	newhiview = (PLINT *) realloc((void *) newhiview,
				      (size_t) (newhisize * sizeof(PLINT)));
	if ( ! newhiview)
	    myexit((char*)"savehipoint: Out of memory.");
    }

    newhiview[xxhi] = px;
    xxhi++;
    newhiview[xxhi] = py;
    xxhi++;
}

static void
savelopoint(PLINT px, PLINT py)
{
    if (pl3upv == 0)
	return;

    if (xxlo >= newlosize) {	/* allocate additional space */
	newlosize += 2 * BINC;
	newloview = (PLINT *) realloc((void *) newloview,
				      (size_t) (newlosize * sizeof(PLINT)));
	if ( ! newloview)
	    myexit((char*)"savelopoint: Out of memory.");
    }

    newloview[xxlo] = px;
    xxlo++;
    newloview[xxlo] = py;
    xxlo++;
}

/*--------------------------------------------------------------------------*\
 * swaphiview
 * swaploview
 *
 * Swaps the top/bottom views.  Need to do a real swap so that the
 * memory cleanup routine really frees everything (and only once).
\*--------------------------------------------------------------------------*/

static void
swaphiview(void)
{
    PLINT *tmp;

    if (pl3upv != 0) {
	mhi = xxhi / 2;
	tmp = oldhiview;
	oldhiview = newhiview;
	newhiview = tmp;
    }
}

static void
swaploview(void)
{
    PLINT *tmp;

    if (pl3upv != 0) {
	mlo = xxlo / 2;
	tmp = oldloview;
	oldloview = newloview;
	newloview = tmp;
    }
}

/*--------------------------------------------------------------------------*\
 * freework
 *
 * Frees memory associated with work arrays
\*--------------------------------------------------------------------------*/

static void
freework(void)
{
    free_mem(oldhiview);
    free_mem(oldloview);
    free_mem(newhiview);
    free_mem(newloview);
    free_mem(vtmp);
    free_mem(utmp);
    free_mem(ctmp);
}

/*--------------------------------------------------------------------------*\
 * myexit
 *
 * Calls plexit, cleaning up first.
\*--------------------------------------------------------------------------*/

static void
myexit(char *msg)
{
    freework();
    plexit(msg);
}

/*--------------------------------------------------------------------------*\
 * myabort
 *
 * Calls plabort, cleaning up first.
 * Caller should return to the user program.
\*--------------------------------------------------------------------------*/

static void
myabort(char *msg)
{
    freework();
    plabort(msg);
}

/*--------------------------------------------------------------------------*\
 * int plabv()
 *
 * Determines if point (px,py) lies above the line joining (sx1,sy1) to
 * (sx2,sy2). It only works correctly if sx1 <= px <= sx2.
\*--------------------------------------------------------------------------*/

static int
plabv(PLINT px, PLINT py, PLINT sx1, PLINT sy1, PLINT sx2, PLINT sy2)
{
    int above;

    if (py >= sy1 && py >= sy2)
	above = 1;
    else if (py < sy1 && py < sy2)
	above = 0;
    else if ((double) (sx2 - sx1) * (py - sy1) >=
	     (double) (px - sx1) * (sy2 - sy1))
	above = 1;
    else
	above = 0;

    return above;
}

/*--------------------------------------------------------------------------*\
 * void pl3cut()
 *
 * Determines the point of intersection (cx,cy) between the line joining
 * (sx1,sy1) to (sx2,sy2) and the line joining (su1,sv1) to (su2,sv2).
\*--------------------------------------------------------------------------*/

static void
pl3cut(PLINT sx1, PLINT sy1, PLINT sx2, PLINT sy2,
       PLINT su1, PLINT sv1, PLINT su2, PLINT sv2, PLINT *cx, PLINT *cy)
{
    PLINT x21, y21, u21, v21, yv1, xu1, a, b;
    double fa, fb;

    x21 = sx2 - sx1;
    y21 = sy2 - sy1;
    u21 = su2 - su1;
    v21 = sv2 - sv1;
    yv1 = sy1 - sv1;
    xu1 = sx1 - su1;

    a = x21 * v21 - y21 * u21;
    fa = (double) a;
    if (a == 0) {
	if (sx2 < su2) {
	    *cx = sx2;
	    *cy = sy2;
	}
	else {
	    *cx = su2;
	    *cy = sv2;
	}
	return;
    }
    else {
	b = yv1 * u21 - xu1 * v21;
	fb = (double) b;
	*cx = (PLINT) (sx1 + (fb * x21) / fa + .5);
	*cy = (PLINT) (sy1 + (fb * y21) / fa + .5);
    }
}


void
mesh_nr(PLFLT *x, PLFLT *y, PLFLT **z, PLINT nx, PLINT ny, PLINT opt)
{
  c_plot3dc_nr(x, y, z, nx, ny, opt | MESH, NULL, 0);
}


/*--------------------------------------------------------------------------*\
 * void plot3dc_nr(x, y, z, nx, ny, opt, clevel, nlevel)
 *
 * Plots a 3-d representation of the function z[x][y]. The x values
 * are stored as x[0..nx-1], the y values as y[0..ny-1], and the z
 * values are in the 2-d array z[][]. The integer "opt" specifies:
 * see plot3dcl() below
\*--------------------------------------------------------------------------*/

void
c_plot3dc_nr(PLFLT *x, PLFLT *y, PLFLT **z,
	     PLINT nx, PLINT ny, PLINT opt,
	     PLFLT *clevel, PLINT nlevel)
{
  c_plot3dcl_nr( x, y, z, nx, ny, opt, clevel, nlevel, 0, 0, NULL, NULL);
}


/*--------------------------------------------------------------------------*\
 * void plot3dcl_nr(x, y, z, nx, ny, opt, clevel, nlevel,
 *       ixstart, ixn, indexymin, indexymax)
 *
 * Plots a 3-d representation of the function z[x][y]. The x values
 * are stored as x[0..nx-1], the y values as y[0..ny-1], and the z
 * values are in the 2-d array z[][]. The integer "opt" specifies:
 *
 *  DRAW_LINEX :  Draw lines parallel to x-axis
 *  DRAW_LINEY :  Draw lines parallel to y-axis
 *  DRAW_LINEXY:  Draw lines parallel to both axes
 *  MAG_COLOR:    Magnitude coloring of wire frame
 *  BASE_CONT:    Draw contour at bottom xy plane
 *  TOP_CONT:     Draw contour at top xy plane (not yet)
 *  DRAW_SIDES:   Draw sides around the plot
 *  MESH:       Draw the "under" side of the plot
 *
 * or any bitwise combination, e.g. "MAG_COLOR | DRAW_LINEX"
 * indexymin and indexymax are arrays which specify the y index limits
 * (following the convention that the upper range limit is one more than
 * actual index limit) for an x index range of ixstart, ixn.
\*--------------------------------------------------------------------------*/

void
c_plot3dcl_nr(PLFLT *x, PLFLT *y, PLFLT **z,
	      PLINT nx, PLINT ny, PLINT opt,
	      PLFLT *clevel, PLINT nlevel,
	      PLINT ixstart, PLINT ixn, PLINT *indexymin, PLINT *indexymax)
{
    PLFLT cxx, cxy, cyx, cyy, cyz;
    PLINT init, i, ix, iy, color, width;
    PLFLT xmin, xmax, ymin, ymax, zmin, zmax, zscale;
    PLINT clipped = 0, base_cont = 0, side = 0;

    pl3mode = 0;

    if (plsc->level < 3) {
	myabort((char*)"plot3dcl: Please set up window first");
	return;
    }

    if (opt < 1) {
	myabort((char*)"plot3dcl: Bad option");
	return;
    }

    if (nx <= 0 || ny <= 0) {
	myabort((char*)"plot3dcl: Bad array dimensions.");
	return;
    }

    plP_gdom(&xmin, &xmax, &ymin, &ymax);
    plP_grange(&zscale, &zmin, &zmax);
    if(zmin > zmax) {
      PLFLT t = zmin;
      zmin = zmax;
      zmax = t;
    }

    if (opt & MESH)
      pl3mode = 1;

    if (opt & DRAW_SIDES)
      side = 1;

    if ((opt & BASE_CONT) || (opt & TOP_CONT) || (opt && MAG_COLOR)) {
     /*
      * Don't use the data z value to scale the color, use the z axis
      * values set by plw3d()
      *
      * plMinMax2dGrid(z, nx, ny, &fc_maxz, &fc_minz);
      */

     fc_minz = plsc->ranmi;
     fc_maxz = plsc->ranma;

     if (fc_maxz == fc_minz) {
       plwarn("plot3dcl: Maximum and minimum Z values are equal! \"fixing\"...");
       fc_maxz = fc_minz + 1e-6;
     }
    }

    if (opt & BASE_CONT) {     /* If enabled, draw the contour at the base.  */
      if (clevel != NULL && nlevel != 0) {
	base_cont = 1;
	/* even if MESH is not set, "set it",
	   as the base contour can only be done in this case */
	pl3mode = 1;
      }
    }

    if (opt & MAG_COLOR) {     /* If enabled, use magnitude colored wireframe  */
      //      ctmp = (PLFLT *) malloc((size_t) (2 * MAX(nx, ny) * sizeof(PLFLT)));
    } else
      ctmp = NULL;
    
    /* next logic only knows opt = 1 | 2 | 3, make sure that it only gets that */
    opt &= DRAW_LINEXY;

    /* Allocate work arrays */

    utmp = (PLINT *) malloc((size_t) (2 * MAX(nx, ny) * sizeof(PLINT)));
    vtmp = (PLINT *) malloc((size_t) (2 * MAX(nx, ny) * sizeof(PLINT)));

    if ( ! utmp || ! vtmp)
	myexit((char*)"plot3dcl: Out of memory.");

    plP_gw3wc(&cxx, &cxy, &cyx, &cyy, &cyz);
    init = 1;
    /* Call 3d line plotter.  Each viewing quadrant
       (perpendicular to x-y plane) must be handled separately. */

    if (cxx >= 0.0 && cxy <= 0.0) {
      if (opt == DRAW_LINEY)
	plt3zz_nr(1, ny, 1, -1, -opt, &init, x, y, z, nx, ny, utmp, vtmp,ctmp);
      else {
	    for (iy = 2; iy <= ny; iy++)
		plt3zz_nr(1, iy, 1, -1, -opt, &init, x, y, z, nx, ny, utmp, vtmp,ctmp);
	}
	if (opt == DRAW_LINEX)
	    plt3zz_nr(1, ny, 1, -1, opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	else {
	    for (ix = 1; ix <= nx - 1; ix++)
		plt3zz_nr(ix, ny, 1, -1, opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	}
    }

    else if (cxx <= 0.0 && cxy <= 0.0) {
        if (opt == DRAW_LINEX)
	    plt3zz_nr(nx, ny, -1, -1, opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	else {
	    for (ix = 2; ix <= nx; ix++)
		plt3zz_nr(ix, ny, -1, -1, opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	}
	if (opt == DRAW_LINEY)
	    plt3zz_nr(nx, ny, -1, -1, -opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	else {
	    for (iy = ny; iy >= 2; iy--)
	      plt3zz_nr(nx, iy, -1, -1, -opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	}
    }

    else if (cxx <= 0.0 && cxy >= 0.0) {
	if (opt == DRAW_LINEY)
	    plt3zz_nr(nx, 1, -1, 1, -opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	else {
	    for (iy = ny - 1; iy >= 1; iy--)
		plt3zz_nr(nx, iy, -1, 1, -opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	}
	if (opt == DRAW_LINEX)
	    plt3zz_nr(nx, 1, -1, 1, opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	else {
	    for (ix = nx; ix >= 2; ix--)
		plt3zz_nr(ix, 1, -1, 1, opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	}
    }

    else if (cxx >= 0.0 && cxy >= 0.0) {
	if (opt == DRAW_LINEX)
	    plt3zz_nr(1, 1, 1, 1, opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	else {
	    for (ix = nx - 1; ix >= 1; ix--)
		plt3zz_nr(ix, 1, 1, 1, opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	}
	if (opt == DRAW_LINEY)
	    plt3zz_nr(1, 1, 1, 1, -opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	else {
	    for (iy = 1; iy <= ny - 1; iy++)
		plt3zz_nr(1, iy, 1, 1, -opt, &init, x, y, z, nx, ny, utmp, vtmp, ctmp);
	}
    }

    /* Finish up by drawing sides, background grid (both are optional) */

    if (side)
	plside3(x, y, z, nx, ny, opt);

    if (zbflg) {
	color = plsc->icol0;
        width = plsc->width;
	plwid(zbwidth);
	plcol0(zbcol);
	plgrid3(zbtck);
	plwid(width);
	plcol0(color);
    }

    freework();
}


/*--------------------------------------------------------------------------*\
 * void plt3zz_nr()
 *
 * Draws the next zig-zag line for a 3-d plot.  The data is stored in array
 * z[][] as a function of x[] and y[], and is plotted out starting at index
 * (x0,y0).
 *
 * Depending on the state of "flag", the sequence of data points sent to
 * plnxtv is altered so as to allow cross-hatch plotting, or plotting
 * parallel to either the x-axis or the y-axis.
\*--------------------------------------------------------------------------*/

static void
plt3zz_nr(PLINT x0, PLINT y0, PLINT dx, PLINT dy, PLINT flag, PLINT *init,
       PLFLT *x, PLFLT *y, PLFLT **z, PLINT nx, PLINT ny,
       PLINT *u, PLINT *v, PLFLT* c)
{
    PLINT n = 0;
    PLFLT x2d, y2d;

    while (1 <= x0 && x0 <= nx && 1 <= y0 && y0 <= ny) {

      if (ny == 1) {
	x2d = plP_w3wcx(x[x0 - 1], y[x0 - 1], z[x0 - 1][y0 - 1]);
	y2d = plP_w3wcy(x[x0 - 1], y[x0 - 1], z[x0 - 1][y0 - 1]);
      }

      if (nx == 1) {
	x2d = plP_w3wcx(x[y0 - 1], y[y0 - 1], z[x0 - 1][y0 - 1]);
	y2d = plP_w3wcy(x[y0 - 1], y[y0 - 1], z[x0 - 1][y0 - 1]);
      }

	u[n] = plP_wcpcx(x2d);
	v[n] = plP_wcpcy(y2d);
	if (c != NULL)
	  c[n] = (z[x0 - 1][y0 - 1] - fc_minz)/(fc_maxz-fc_minz);

	switch (flag) {
	case -3:
	    y0 += dy;
	    flag = -flag;
	    break;
	case -2:
	    y0 += dy;
	    break;
	case -1:
	    y0 += dy;
	    flag = -flag;
	    break;
	case 1:
	    x0 += dx;
	    break;
	case 2:
	    x0 += dx;
	    flag = -flag;
	    break;
	case 3:
	    x0 += dx;
	    flag = -flag;
	    break;
	}
	n++;
    }

    if (flag == 1 || flag == -2) {
	if (flag == 1) {
	    x0 -= dx;
	    y0 += dy;
	}
	else if (flag == -2) {
	    y0 -= dy;
	    x0 += dx;
	}
	if (1 <= x0 && x0 <= nx && 1 <= y0 && y0 <= ny) {
	  if (ny == 1) {
	    x2d = plP_w3wcx(x[x0 - 1], y[x0 - 1], z[x0 - 1][y0 - 1]);
	    y2d = plP_w3wcy(x[x0 - 1], y[x0 - 1], z[x0 - 1][y0 - 1]);
	  }

	  if (nx == 1) {
	    x2d = plP_w3wcx(x[y0 - 1], y[y0 - 1], z[x0 - 1][y0 - 1]);
	    y2d = plP_w3wcy(x[y0 - 1], y[y0 - 1], z[x0 - 1][y0 - 1]);
	  }

	    u[n] = plP_wcpcx(x2d);
	    v[n] = plP_wcpcy(y2d);
	    if (c != NULL)
	      c[n] = (z[x0 - 1][y0 - 1] - fc_minz)/(fc_maxz-fc_minz);
	    n++;
	}
    }

/* All the setup is done.  Time to do the work. */

    plnxtv(u, v, c, n, *init);
    *init = 0;
}
}

