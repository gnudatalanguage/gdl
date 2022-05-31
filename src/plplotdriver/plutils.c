#define STRING_LEN         40
#define FORMAT_LEN         10
#define TEMP_LEN           30
#define N_EDGE_SEGMENTS    50

#define    gdl_plmtex                   gdl_c_plmtex

static PLFLT plplot_xlog[8] ={
  0.301030, 0.477121, 0.602060, 0.698970,
  0.778151, 0.845098, 0.903090, 0.954243
};

static PLINT xline[PL_MAXPOLY], yline[PL_MAXPOLY];

static void
gdl_plP_movphy(PLINT x, PLINT y)
{
  PLFLT xt, yt;
  TRANSFORM(x, y, &xt, &yt);
  plsc->currx = xt;
  plsc->curry = yt;
}

static void
gdl_plP_draphy(PLINT x, PLINT y)
{
  PLFLT xt, yt;
  TRANSFORM(x, y, &xt, &yt);
  plP_draphy(xt, yt);
}
//--------------------------------------------------------------------------
// void pllclp()
//
// Draws a polyline within the clip limits.
// Merely a front-end to plP_pllclp().
//--------------------------------------------------------------------------

static void
gdl_pllclp(PLINT *x, PLINT *y, PLINT npts)
{
  plP_pllclp(x, y, npts, plsc->clpxmi, plsc->clpxma,
      plsc->clpymi, plsc->clpyma, genlin);
}

//--------------------------------------------------------------------------
// void gdl_plP_pllclp()
//
// Draws a polyline within the clip limits.
//
// (AM)
// Wanted to change the type of xclp, yclp to avoid overflows!
// But that changes the type for the drawing routines too!
//--------------------------------------------------------------------------

void
gdl_plP_pllclp(PLINT *x, PLINT *y, PLINT npts,
    PLINT xmin, PLINT xmax, PLINT ymin, PLINT ymax,
    void ( *draw)(short *, short *, PLINT))
{
  PLINT x1, x2, y1, y2;
  PLINT i, iclp = 0;

  short _xclp[PL_MAXPOLY], _yclp[PL_MAXPOLY];
  short *xclp = NULL, *yclp = NULL;
  int drawable;

  if (npts < PL_MAXPOLY) {
    xclp = _xclp;
    yclp = _yclp;
  } else {
    if (((xclp = (short *) malloc((size_t) npts * sizeof ( short))) == NULL) ||
        ((yclp = (short *) malloc((size_t) npts * sizeof ( short))) == NULL)) {
      plexit("gdl_plP_pllclp: Insufficient memory");
    }
  }

  for (i = 0; i < npts - 1; i++) {
    x1 = x[i];
    x2 = x[i + 1];
    y1 = y[i];
    y2 = y[i + 1];

    drawable = (INSIDE(x1, y1) && INSIDE(x2, y2));
    if (!drawable)
      drawable = !plP_clipline(&x1, &y1, &x2, &y2,
        xmin, xmax, ymin, ymax);

    if (drawable) {
      // First point of polyline.

      if (iclp == 0) {
        xclp[iclp] = (short) x1;
        yclp[iclp] = (short) y1;
        iclp++;
        xclp[iclp] = (short) x2;
        yclp[iclp] = (short) y2;
      }        // Not first point.  Check if first point of this segment matches up to
        // previous point, and if so, add it to the current polyline buffer.

      else if (x1 == xclp[iclp] && y1 == yclp[iclp]) {
        iclp++;
        xclp[iclp] = (short) x2;
        yclp[iclp] = (short) y2;
      }        // Otherwise it's time to start a new polyline

      else {
        if (iclp + 1 >= 2)
          (*draw)(xclp, yclp, iclp + 1);
        iclp = 0;
        xclp[iclp] = (short) x1;
        yclp[iclp] = (short) y1;
        iclp++;
        xclp[iclp] = (short) x2;
        yclp[iclp] = (short) y2;
      }
    }
  }

  // Handle remaining polyline

  if (iclp + 1 >= 2)
    (*draw)(xclp, yclp, iclp + 1);

  plsc->currx = x[npts - 1];
  plsc->curry = y[npts - 1];

  if (xclp != _xclp) {
    free(xclp);
    free(yclp);
  }
}

static void
gdl_plP_draphy_poly(PLINT *x, PLINT *y, PLINT n)
{
  PLINT i, j, ib, ilim;
  PLFLT xt, yt;

  for (ib = 0; ib < n; ib += PL_MAXPOLY - 1) {
    ilim = MIN(PL_MAXPOLY, n - ib);

    for (i = 0; i < ilim; i++) {
      j = ib + i;
      TRANSFORM(x[j], y[j], &xt, &yt);
      xline[i] = xt;
      yline[i] = yt;
    }
    gdl_pllclp(xline, yline, ilim);
  }
}

static void
gdl_plxtik(PLINT x, PLINT y, PLINT below, PLINT above)
{
  gdl_plP_movphy(x, y - below);
  gdl_plP_draphy(x, y + above);
}

static void
gdl_plytik(PLINT x, PLINT y, PLINT left, PLINT right)
{
  gdl_plP_movphy(x - left, y);
  gdl_plP_draphy(x + right, y);
}
static PLUNICODE unicode_buffer_static[1024];

void
gdl_plP_text(PLINT base, PLFLT just, PLFLT *xform, PLINT x, PLINT y,
    PLINT refx, PLINT refy, PLCHAR_VECTOR string)
{
  size_t len;

  // First, check if the caller passed an empty string.  If it is,
  // then we can return now
  if (string == NULL)
    return;

  if (plsc->dev_text) // Does the device render it's own text ?
  {
    EscText args;

    args.text_type = PL_STRING_TEXT;
    args.base = base;
    args.just = just;
    args.xform = xform;
    args.x = x;
    args.y = y;
    args.refx = refx;
    args.refy = refy;

    // Always store the string passed by the caller, even for unicode
    // enabled drivers.  The plmeta driver will use this field to store
    // the string data in the metafile.
    args.string = string;

    // Does the device also understand unicode?
    if (plsc->dev_unicode) {
      if (plsc->alt_unicode) {
        // We are using the alternate unicode processing
        alternate_unicode_processing(string, &args);

        // All text processing is done, so we can exit
        return;
      } else {
        // Setup storage for the unicode array and
        // process the string to generate the unicode
        // representation of it.
        args.unicode_array = unicode_buffer_static;
        encode_unicode(string, &args);

        len = (size_t) args.unicode_array_len;
      }
    } else {
      //  We are using the char array, NULL out the unicode part
      args.unicode_array = NULL;
      args.unicode_array_len = 0;

      len = strlen(string);
    }

    // If the string is not empty, ask the driver to display it
    if (len > 0)
      plP_esc(PLESC_HAS_TEXT, &args);

#ifndef DEBUG_TEXT
  } else {
#endif
    gdl_plstr(base, xform, refx, refy, string);
  }
}
//--------------------------------------------------------------------------
// void plstr()
//
// Prints out a "string" at reference position with physical coordinates
// (refx,refy). The coordinates of the vectors defining the string are
// passed through the linear mapping defined by the 2 x 2 matrix xform()
// before being plotted.  The reference position is at the left-hand edge of
// the string. If base = 1, it is aligned with the baseline of the string.
// If base = 0, it is aligned with the center of the character box.
//
// Note, all calculations are done in terms of millimetres. These are scaled
// as necessary before plotting the string on the page.
//--------------------------------------------------------------------------

void
gdl_plstr(PLINT base, PLFLT *xform, PLINT refx, PLINT refy, PLCHAR_VECTOR string)
{
  short int *symbol;
  signed char *vxygrid = 0;

  PLINT ch, i, length, level = 0, style, oline = 0, uline = 0;
  PLFLT width = 0., xorg = 0., yorg = 0., def, ht, dscale, scale;
  PLFLT old_sscale, sscale, old_soffset, soffset;

  plgchr(&def, &ht);
  dscale = 0.05 * ht;
  scale = dscale;

  // Line style must be continuous

  style = plsc->nms;
  plsc->nms = 0;

  pldeco(&symbol, &length, string);

  for (i = 0; i < length; i++) {
    ch = symbol[i];
    if (ch == -1) // superscript
    {
      plP_script_scale(TRUE, &level,
          &old_sscale, &sscale, &old_soffset, &soffset);
      yorg = 16.0 * dscale * soffset;
      scale = dscale * sscale;
    } else if (ch == -2) // subscript
    {
      plP_script_scale(FALSE, &level,
          &old_sscale, &sscale, &old_soffset, &soffset);
      yorg = -16.0 * dscale * soffset;
      scale = dscale * sscale;
    } else if (ch == -3) // back-char
      xorg -= width * scale;
    else if (ch == -4) // toogle overline
      oline = !oline;
    else if (ch == -5) // toogle underline
      uline = !uline;
    else {
      if (plcvec(ch, &vxygrid))
        gdl_plchar(vxygrid, xform, base, oline, uline, refx, refy, scale,
          plsc->xpmm, plsc->ypmm, &xorg, &yorg, &width);
    }
  }
  plsc->nms = style;
}

//--------------------------------------------------------------------------
// plchar()
//
// Plots out a given stroke font character.
//--------------------------------------------------------------------------

static void
gdl_plchar(signed char *vxygrid, PLFLT *xform, PLINT base, PLINT oline, PLINT uline,
    PLINT refx, PLINT refy, PLFLT scale, PLFLT xpmm, PLFLT ypmm,
    PLFLT *p_xorg, PLFLT *p_yorg, PLFLT *p_width)
{
  PLINT xbase, ybase, ydisp, lx, ly, cx, cy;
  PLINT k, penup;
  PLFLT x, y;
  PLINT llx[STLEN], lly[STLEN], l = 0;

  xbase = vxygrid[2];
  *p_width = vxygrid[3] - xbase;
  if (base == 0) {
    ybase = 0;
    ydisp = vxygrid[0];
  } else {
    ybase = vxygrid[0];
    ydisp = 0;
  }
  k = 4;
  penup = 1;

  for (;;) {
    cx = vxygrid[k++];
    cy = vxygrid[k++];
    if (cx == 64 && cy == 64) {
      if (l) {
        gdl_plP_draphy_poly(llx, lly, l);
        l = 0;
      }
      break;
    }
    if (cx == 64 && cy == 0) {
      if (l) {
        gdl_plP_draphy_poly(llx, lly, l);
        l = 0;
      }
      penup = 1;
    } else {
      x = *p_xorg + (cx - xbase) * scale;
      y = *p_yorg + (cy - ybase) * scale;
      lx = refx + ROUND(xpmm * (xform[0] * x + xform[1] * y));
      ly = refy + ROUND(ypmm * (xform[2] * x + xform[3] * y));
      if (penup == 1) {
        if (l) {
          gdl_plP_draphy_poly(llx, lly, l);
          l = 0;
        }
        llx[l] = lx;
        lly[l++] = ly; // store 1st point !
        plP_movphy(lx, ly);
        penup = 0;
      } else {
        llx[l] = lx;
        lly[l++] = ly;
      }
    }
  }

  if (oline) {
    x = *p_xorg;
    y = *p_yorg + (30 + ydisp) * scale;
    lx = refx + ROUND(xpmm * (xform[0] * x + xform[1] * y));
    ly = refy + ROUND(ypmm * (xform[2] * x + xform[3] * y));
    plP_movphy(lx, ly);
    x = *p_xorg + *p_width * scale;
    lx = refx + ROUND(xpmm * (xform[0] * x + xform[1] * y));
    ly = refy + ROUND(ypmm * (xform[2] * x + xform[3] * y));
    plP_draphy(lx, ly);
  }
  if (uline) {
    x = *p_xorg;
    y = *p_yorg + (-5 + ydisp) * scale;
    lx = refx + ROUND(xpmm * (xform[0] * x + xform[1] * y));
    ly = refy + ROUND(ypmm * (xform[2] * x + xform[3] * y));
    plP_movphy(lx, ly);
    x = *p_xorg + *p_width * scale;
    lx = refx + ROUND(xpmm * (xform[0] * x + xform[1] * y));
    ly = refy + ROUND(ypmm * (xform[2] * x + xform[3] * y));
    plP_draphy(lx, ly);
  }
  *p_xorg = *p_xorg + *p_width * scale;
}

//--------------------------------------------------------------------------
// PLFLT plstrl()
//
// Computes the length of a string in mm, including escape sequences.
//--------------------------------------------------------------------------

PLFLT
gdl_plstrl(PLCHAR_VECTOR string)
{
  short int *symbol;
  signed char *vxygrid = 0;
  PLINT ch, i, length, level = 0;
  PLFLT width = 0., xorg = 0., dscale, scale, def, ht;

  // If the driver will compute string lengths for us then we ask
  // it do so by setting get_string_length flag. When this is set
  // the driver will set the string_length variable instead of
  // actually rendering the string.
  // Note we must make sure that this text command does not end up
  // in the buffer.
  //
  // TODO:
  //   Is plmtex the best string diplay routine to use?
  //   Will this work for buffered plots?

  if (plsc->has_string_length) {
    PLINT plbuf_write = plsc->plbuf_write;
    plsc->plbuf_write = FALSE;
    plsc->get_string_length = 1;
    c_plmtex("t", 0.0, 0.0, 0.0, string);
    plsc->get_string_length = 0;
    plsc->plbuf_write = plbuf_write;
    return (PLFLT) plsc->string_length;
  }


  plgchr(&def, &ht);
  dscale = 0.05 * ht;
  scale = dscale;
  pldeco(&symbol, &length, string);

  for (i = 0; i < length; i++) {
    ch = symbol[i];
    if (ch == -1) {
      level++;
      scale = dscale * pow(0.75, (double) ABS(level));
    } else if (ch == -2) {
      level--;
      scale = dscale * pow(0.75, (double) ABS(level));
    } else if (ch == -3)
      xorg -= width * scale;
    else if (ch == -4 || ch == -5)
      ;
    else {
      if (plcvec(ch, &vxygrid)) {
        width = vxygrid[3] - vxygrid[2];
        xorg += width * scale;
      }
    }
  }
  return (PLFLT) xorg;
}

//--------------------------------------------------------------------------
// void plmtex()
//
// Prints out "text" at specified position relative to viewport
// (may be inside or outside)
//
// side	String which is one of the following:
//     B or b  :  Bottom of viewport
//     T or t  :  Top of viewport
//     BV or bv : Bottom of viewport, vertical text
//     TV or tv : Top of viewport, vertical text
//     L or l  :  Left of viewport
//     R or r  :  Right of viewport
//     LV or lv : Left of viewport, vertical text
//     RV or rv : Right of viewport, vertical text
//
// disp Displacement from specified edge of viewport, measured outwards from
//	the viewport in units of the current character height. The
//	centerlines of the characters are aligned with the specified
//	position.
//
// pos	Position of the reference point of the string relative to the
//	viewport edge, ranging from 0.0 (left-hand edge) to 1.0 (right-hand
//	edge)
//
// just	Justification of string relative to reference point
//	just = 0.0 => left hand edge of string is at reference
//	just = 1.0 => right hand edge of string is at reference
//	just = 0.5 => center of string is at reference
//--------------------------------------------------------------------------

void
gdl_c_plmtex(PLCHAR_VECTOR side, PLFLT disp, PLFLT pos, PLFLT just,
    PLCHAR_VECTOR text)
{
  PLINT clpxmi, clpxma, clpymi, clpyma;
  PLINT vert, refx, refy, x, y;
  PLFLT xdv, ydv, xmm, ymm, refxmm, refymm, shift, xform[4];
  PLFLT chrdef, chrht;
  PLFLT dispx, dispy;

  if (plsc->level < 2) {
    plabort("plmtex: Please set up viewport first");
    return;
  }

  // Open clip limits to subpage limits

  plP_gclp(&clpxmi, &clpxma, &clpymi, &clpyma); // get and store current clip limits
  plP_sclp(plsc->sppxmi, plsc->sppxma, plsc->sppymi, plsc->sppyma);

  if (plP_stindex(side, "BV") != -1 || plP_stindex(side, "bv") != -1) {
    vert = 1;
    xdv = plsc->vpdxmi + (plsc->vpdxma - plsc->vpdxmi) * pos;
    ydv = plsc->vpdymi;
    dispx = 0;
    dispy = -disp;
  } else if (plP_stindex(side, "TV") != -1 || plP_stindex(side, "tv") != -1) {
    vert = 1;
    xdv = plsc->vpdxmi + (plsc->vpdxma - plsc->vpdxmi) * pos;
    ydv = plsc->vpdyma;
    dispx = 0;
    dispy = disp;
  } else if (plP_stsearch(side, 'b')) {
    vert = 0;
    xdv = plsc->vpdxmi + (plsc->vpdxma - plsc->vpdxmi) * pos;
    ydv = plsc->vpdymi;
    dispx = 0;
    dispy = -disp;
  } else if (plP_stsearch(side, 't')) {
    vert = 0;
    xdv = plsc->vpdxmi + (plsc->vpdxma - plsc->vpdxmi) * pos;
    ydv = plsc->vpdyma;
    dispx = 0;
    dispy = disp;
  } else if (plP_stindex(side, "LV") != -1 || plP_stindex(side, "lv") != -1) {
    vert = 0;
    xdv = plsc->vpdxmi;
    ydv = plsc->vpdymi + (plsc->vpdyma - plsc->vpdymi) * pos;
    dispx = -disp;
    dispy = 0;
  } else if (plP_stindex(side, "RV") != -1 || plP_stindex(side, "rv") != -1) {
    vert = 0;
    xdv = plsc->vpdxma;
    ydv = plsc->vpdymi + (plsc->vpdyma - plsc->vpdymi) * pos;
    dispx = disp;
    dispy = 0;
  } else if (plP_stsearch(side, 'l')) {
    vert = 1;
    xdv = plsc->vpdxmi;
    ydv = plsc->vpdymi + (plsc->vpdyma - plsc->vpdymi) * pos;
    dispx = -disp;
    dispy = 0;
  } else if (plP_stsearch(side, 'r')) {
    vert = 1;
    xdv = plsc->vpdxma;
    ydv = plsc->vpdymi + (plsc->vpdyma - plsc->vpdymi) * pos;
    dispx = disp;
    dispy = 0;
  } else {
    plP_sclp(clpxmi, clpxma, clpymi, clpyma); // restore initial clip limits
    return;
  }

  // Transformation matrix

  if (vert != 0) {
    xform[0] = 0.0;
    xform[1] = -1.0;
    xform[2] = 1.0;
    xform[3] = 0.0;
  } else {
    xform[0] = 1.0;
    xform[1] = 0.0;
    xform[2] = 0.0;
    xform[3] = 1.0;
  }

  // Convert to physical units (mm) and compute shifts

  plgchr(&chrdef, &chrht);
  shift = (just == 0.0) ? 0.0 : gdl_plstrl(text) * just;

  xmm = plP_dcmmx(xdv) + dispx * chrht;
  ymm = plP_dcmmy(ydv) + dispy * chrht;
  refxmm = xmm - shift * xform[0];
  refymm = ymm - shift * xform[2];

  // Convert to device units (pixels) and call text plotter

  x = plP_mmpcx(xmm);
  y = plP_mmpcy(ymm);
  refx = plP_mmpcx(refxmm);
  refy = plP_mmpcy(refymm);

  plP_text(0, just, xform, x, y, refx, refy, text);
  plP_sclp(clpxmi, clpxma, clpymi, clpyma); // restore clip limits
}

static void
grid_box(PLCHAR_VECTOR xopt, PLFLT xtick1, PLINT nxsub1,
    PLCHAR_VECTOR yopt, PLFLT ytick1, PLINT nysub1)
{
  PLINT lgx, lhx, llx, ldx;
  PLINT lgy, lhy, lly, ldy;
  PLFLT vpwxmi, vpwxma, vpwymi, vpwyma;
  PLFLT vpwxmin, vpwxmax, vpwymin, vpwymax;
  PLFLT tn, temp, tcrit, tspace = 0.1;
  PLFLT tstart, factor;
  PLINT i;

  // Set plot options from input

  lgx = plP_stsearch(xopt, 'g');
  lhx = plP_stsearch(xopt, 'h');
  llx = plP_stsearch(xopt, 'l');
  ldx = plP_stsearch(xopt, 'd');

  lgy = plP_stsearch(yopt, 'g');
  lhy = plP_stsearch(yopt, 'h');
  lly = plP_stsearch(yopt, 'l');
  ldy = plP_stsearch(yopt, 'd');

  plP_xgvpw(&vpwxmin, &vpwxmax, &vpwymin, &vpwymax);
  // n.b. large change; vpwxmi always numerically less than vpwxma, and
  // similarly for vpwymi
  vpwxmi = (vpwxmax > vpwxmin) ? vpwxmin : vpwxmax;
  vpwxma = (vpwxmax > vpwxmin) ? vpwxmax : vpwxmin;
  vpwymi = (vpwymax > vpwymin) ? vpwymin : vpwymax;
  vpwyma = (vpwymax > vpwymin) ? vpwymax : vpwymin;

  // Draw grid in x direction.

  if (lgx) {
    if (ldx) {
      pldtfac(vpwxmi, vpwxma, &factor, &tstart);
      tn = xtick1 * (floor((vpwxmi - tstart) / xtick1)) + tstart;
    } else {
      tn = xtick1 * floor(vpwxmi / xtick1);
    }
    for (; tn <= vpwxma; tn += xtick1) {
      if (lhx) {
        if (llx) {
          PLFLT otemp = tn;
          for (i = 0; i <= 7; i++) {
            temp = tn + plplot_xlog[i];
            tcrit = (temp - otemp) * tspace;
            otemp = temp;
            if (BETW(temp, vpwxmi + tcrit, vpwxma - tcrit))
              pljoin(temp, vpwymi, temp, vpwyma);
          }
        } else {
          for (i = 1; i <= nxsub1 - 1; i++) {
            temp = tn + i * xtick1 / nxsub1;
            tcrit = xtick1 / nxsub1 * tspace;
            if (BETW(temp, vpwxmi + tcrit, vpwxma - tcrit))
              pljoin(temp, vpwymi, temp, vpwyma);
          }
        }
      }
      tcrit = xtick1 * tspace;
      if (BETW(tn, vpwxmi + tcrit, vpwxma - tcrit))
        pljoin(tn, vpwymi, tn, vpwyma);
    }
  }

  // Draw grid in y direction

  if (lgy) {
    if (ldy) {
      pldtfac(vpwymi, vpwyma, &factor, &tstart);
      tn = ytick1 * (floor((vpwymi - tstart) / ytick1)) + tstart;
    } else {
      tn = ytick1 * floor(vpwymi / ytick1);
    }
    for (; tn <= vpwyma; tn += ytick1) {
      if (lhy) {
        if (lly) {
          PLFLT otemp = tn;
          for (i = 0; i <= 7; i++) {
            temp = tn + plplot_xlog[i];
            tcrit = (temp - otemp) * tspace;
            otemp = temp;
            if (BETW(temp, vpwymi + tcrit, vpwyma - tcrit))
              pljoin(vpwxmi, temp, vpwxma, temp);
          }
        } else {
          for (i = 1; i <= nysub1 - 1; i++) {
            temp = tn + i * ytick1 / nysub1;
            tcrit = ytick1 / nysub1 * tspace;
            if (BETW(temp, vpwymi + tcrit, vpwyma - tcrit))
              pljoin(vpwxmi, temp, vpwxma, temp);
          }
        }
      }
      tcrit = ytick1 * tspace;
      if (BETW(tn, vpwymi + tcrit, vpwyma - tcrit))
        pljoin(vpwxmi, tn, vpwxma, tn);
    }
  }
}

static PLCHAR_VECTOR
plgesc_string(void)
{
  static PLCHAR_VECTOR esc_strings = {"!\0#\0$\0%\0&\0*\0@\0^\0~\0"};
  int d;
  // Follow plgesc logic here which is to set the default escape
  // if plsc->esc is in its initial state.
  if (plsc->esc == '\0')
    plsc->esc = '#';

  switch (plsc->esc) {
  case '!':
    d = 0;
    break;
  case '#':
    d = 1;
    break;
  case '$':
    d = 2;
    break;
  case '%':
    d = 3;
    break;
  case '&':
    d = 4;
    break;
  case '*':
    d = 5;
    break;
  case '@':
    d = 6;
    break;
  case '^':
    d = 7;
    break;
  case '~':
    d = 8;
    break;
  default:
    plwarn("plgesc_string: Invalid escape character, assuming '#' instead");
    d = 1;
    break;
  }
  return &(esc_strings[d * 2]);
}
//--------------------------------------------------------------------------
//
// Default labeling functions for PLplot
//
// These are the functions which are used internally by PLplot under various
// conditions.
//
// They have been separated out for use in other PLplot functions and
// potential exposure in the PLplot API.
//
//--------------------------------------------------------------------------

void plP_default_label_log(PLINT PL_UNUSED(axis), PLFLT value, char *string, PLINT len, void * PL_UNUSED(data))
{
  PLCHAR_VECTOR esc_string = plgesc_string();
  // Exponential, i.e. 10^-1, 10^0, 10^1, etc
  snprintf(string, (size_t) len, "10%su%d", esc_string, (int) ROUND(value));
}

void plP_default_label_log_fixed(PLINT PL_UNUSED(axis), PLFLT value, char *string, PLINT len, void * PL_UNUSED(data))
{
  // Fixed point, i.e. .1, 1, 10, etc

  int exponent = ROUND(value);

  value = pow(10.0, exponent);
  if (exponent < 0) {
    char form[FORMAT_LEN];
    snprintf(form, FORMAT_LEN, "%%.%df", ABS(exponent));
    snprintf(string, (size_t) len, form, value);
  } else {
    snprintf(string, (size_t) len, "%d", (int) value);
  }
}

void plP_default_label(PLINT PL_UNUSED(axis), PLFLT value, char *string, PLINT len, void *data)
{
  PLINT scale, prec;
  PLINT setpre, precis;
  char form[FORMAT_LEN], temp[TEMP_LEN];
  double scale2;

  scale = ((PLINT *) data)[0];
  prec = ((PLINT *) data)[1];

  plP_gprec(&setpre, &precis);

  if (setpre)
    prec = precis;

  if (scale)
    value /= pow(10., (double) scale);

  // This is necessary to prevent labels like "-0.0" on some systems

  scale2 = pow(10., prec);
  value = floor((value * scale2) + .5) / scale2;

  snprintf(form, FORMAT_LEN, "%%.%df", (int) prec);
  snprintf(temp, TEMP_LEN, form, value);
  strncpy(string, temp, (size_t) (len - 1));
  string[len - 1] = '\0';
}

//--------------------------------------------------------------------------
// void plform()
//
// Formats a PLFLT value in one of the following formats.
//
// If ll (logarithmic), then:
//
//    -	If lf (fixed), then used fixed point notation, i.e. .1, 1, 10, etc,
//	with unnecessary trailing .'s or 0's removed.
//
//    -	If !lf (default), then use exponential notation, i.e. 10^-1, etc.
//
// If !ll (linear), then:
//
//    - If scale == 0, use fixed point format with "prec" places after the
//	decimal point.
//
//    -	If scale == 1, use scientific notation with one place before the
//	decimal point and "prec" places after.  In this case, the value
//	must be divided by 10^scale.
//
// The axis argument is included to support PLplot's custom axis labeling.  It
// is passed on to the custom labeling function if it exists.  Otherwise, it
// is ignored.
//--------------------------------------------------------------------------

static void
plform(PLINT axis, PLFLT value, PLINT scale, PLINT prec, char *string, PLINT len, PLBOOL ll, PLBOOL lf, PLBOOL lo)
{
  // Check to see if a custom labeling function is defined.  If not,
  // use default.
  if (lo && plsc->label_func) {
    (*plsc->label_func)(axis, value, string, len, plsc->label_data);
  } else {
    if (lo) {
      plwarn("Custom axis labels requested without a labeling function \
                    - using default.");
    }
    if (ll) {
      // Logarithmic

      if (lf) {
        // Fixed point, i.e. .1, 1, 10, etc
        plP_default_label_log_fixed(axis, value, string, len, NULL);
      } else {
        // Exponential, i.e. 10^-1, 10^0, 10^1, etc
        plP_default_label_log(axis, value, string, len, NULL);
      }
    } else {
      // Linear
      PLINT scale_prec[2] = {scale, prec};
      plP_default_label(axis, value, string, len, (void *) scale_prec);
    }
  }
}

//--------------------------------------------------------------------------
// void label_box()
//
// Writes numeric labels on side(s) of box.
//--------------------------------------------------------------------------

static void
label_box(PLCHAR_VECTOR xopt, PLFLT xtick1, PLCHAR_VECTOR yopt, PLFLT ytick1)
{
  static char string[STRING_LEN];
  PLBOOL ldx, lfx, lix, llx, lmx, lnx, ltx, lox, lxx;
  PLBOOL ldy, lfy, liy, lly, lmy, lny, lty, lvy, loy, lxy;
  PLFLT vpwxmi, vpwxma, vpwymi, vpwyma;
  PLFLT vpwxmin, vpwxmax, vpwymin, vpwymax;
  PLFLT tn, tp, offset;
  PLFLT factor, tstart;
  PLCHAR_VECTOR timefmt = NULL;
  PLFLT default_mm, char_height_mm, height_mm;
  PLFLT string_length_mm = 0.0, pos_mm = 0.0;

  // Assume label data is for placement of exponents if no custom
  // label function is provided.
  PLBOOL custom_exponent_placement = !plsc->label_func && plsc->label_data;

  // pos, height, and just are unnecessarily set to quiet
  // -O3 -Wuninitialized warnings that are obvious false alarms from
  // the clarity of the code associated with the true or false
  // result for custom_exponent_placement.
  PLFLT pos = 0.0, height = 0.0, just = 0.0;
  PLCHAR_VECTOR esc_string = plgesc_string();

  plgchr(&default_mm, &char_height_mm);

  // Set plot options from input

  ldx = plP_stsearch(xopt, 'd');
  lfx = plP_stsearch(xopt, 'f');
  lix = plP_stsearch(xopt, 'i');
  llx = plP_stsearch(xopt, 'l');
  lmx = plP_stsearch(xopt, 'm');
  lnx = plP_stsearch(xopt, 'n');
  ltx = plP_stsearch(xopt, 't');
  lox = plP_stsearch(xopt, 'o');
  lxx = plP_stsearch(xopt, 'x');

  ldy = plP_stsearch(yopt, 'd');
  lfy = plP_stsearch(yopt, 'f');
  liy = plP_stsearch(yopt, 'i');
  lly = plP_stsearch(yopt, 'l');
  lmy = plP_stsearch(yopt, 'm');
  lny = plP_stsearch(yopt, 'n');
  lty = plP_stsearch(yopt, 't');
  lvy = plP_stsearch(yopt, 'v');
  loy = plP_stsearch(yopt, 'o');
  lxy = plP_stsearch(yopt, 'x');

  plP_xgvpw(&vpwxmin, &vpwxmax, &vpwymin, &vpwymax);
  // vpwxmi always numerically less than vpwxma, and
  // similarly for vpwymi
  vpwxmi = (vpwxmax > vpwxmin) ? vpwxmin : vpwxmax;
  vpwxma = (vpwxmax > vpwxmin) ? vpwxmax : vpwxmin;
  vpwymi = (vpwymax > vpwymin) ? vpwymin : vpwymax;
  vpwyma = (vpwymax > vpwymin) ? vpwymax : vpwymin;

  // Write label(s) for horizontal axes.
  if ((lmx || lnx) && (ltx || lxx)) {
    PLINT xmode, xprec, xdigmax, xdigits, xscale;

    plgxax(&xdigmax, &xdigits);
    pldprec(vpwxmi, vpwxma, xtick1, lfx, &xmode, &xprec, xdigmax, &xscale);
    timefmt = plP_gtimefmt();

    if (ldx) {
      pldtfac(vpwxmi, vpwxma, &factor, &tstart);
      tp = xtick1 * (1. + floor((vpwxmi - tstart) / xtick1)) + tstart;
    } else {
      tp = xtick1 * (1. + floor(vpwxmi / xtick1));
    }
    height = lix ? 1.75 : 1.5;
    if (plsc->if_boxbb) {
      // For horizontal axes, height of zero corresponds to
      // character centred on edge so should add 0.5 to height
      // to obtain bounding box edge in direction away from
      // edge.  However, experimentally found 0.7 gave a better
      // looking result.
      height_mm = (height + 0.7) * char_height_mm;
      if (lnx)
        plsc->boxbb_ymin = MIN(plsc->boxbb_ymin, plsc->vppymi /
          plsc->ypmm - height_mm);
      if (lmx)
        plsc->boxbb_ymax = MAX(plsc->boxbb_ymax, plsc->vppyma /
          plsc->ypmm + height_mm);
    }

    for (tn = tp; BETW(tn, vpwxmi, vpwxma); tn += xtick1) {
      if (ldx) {
        //        strfqsas(string, STRING_LEN, timefmt, (double) tn, plsc->qsasconfig);
      } else {
        plform(PL_X_AXIS, tn, xscale, xprec, string, STRING_LEN, llx, lfx, lox);
      }
      pos = (vpwxmax > vpwxmin) ?
          (tn - vpwxmi) / (vpwxma - vpwxmi) :
          (vpwxma - tn) / (vpwxma - vpwxmi);
      if (plsc->if_boxbb) {
        string_length_mm = gdl_plstrl(string);
        pos_mm = (plsc->vppxmi + pos *
            (plsc->vppxma - plsc->vppxmi)) /
            plsc->xpmm;
      }
      if (lnx) {
        // Bottom axis.
        if (plsc->if_boxbb) {
          plsc->boxbb_xmin = MIN(plsc->boxbb_xmin,
              pos_mm - 0.5 * string_length_mm);
          plsc->boxbb_xmax = MAX(plsc->boxbb_xmax,
              pos_mm + 0.5 * string_length_mm);
        } else {
          gdl_pmltex("b", height, pos, 0.5, string);
        }
      }
      if (lmx) {
        // Top axis.
        if (plsc->if_boxbb) {
          plsc->boxbb_xmin = MIN(plsc->boxbb_xmin,
              pos_mm - 0.5 * string_length_mm);
          plsc->boxbb_xmax = MAX(plsc->boxbb_xmax,
              pos_mm + 0.5 * string_length_mm);
        } else {
          gdl_pmltex("t", height, pos, 0.5, string);
        }
      }
    }
    xdigits = 2;
    plsxax(xdigmax, xdigits);

    // Write separate exponential label if mode = 1.

    if (!llx && !ldx && !lox && xmode) {
      if (custom_exponent_placement) {
        height = ((PLLabelDefaults *) plsc->label_data)->exp_label_disp;
        pos = ((PLLabelDefaults *) plsc->label_data)->exp_label_pos;
        just = ((PLLabelDefaults *) plsc->label_data)->exp_label_just;
      } else {
        height = 3.2;
        pos = 1.0;
        just = 0.5;
      }
      snprintf(string, STRING_LEN, "(x10%su%d%sd)", esc_string, (int) xscale, esc_string);
      if (lnx) {
        // Bottom axis exponent.
        if (plsc->if_boxbb) {
          // For horizontal axes, height of zero corresponds
          // to character centred on edge so should add 0.5
          // to height to obtain bounding box edge in
          // direction away from edge if no exponent.  Add
          // an additional offset to make exponent fit.
          height_mm = (height + 0.9) * char_height_mm;
          plsc->boxbb_ymin = MIN(plsc->boxbb_ymin, plsc->vppymi /
              plsc->ypmm - height_mm);
          string_length_mm = gdl_plstrl(string);
          pos_mm = (plsc->vppxmi + pos *
              (plsc->vppxma - plsc->vppxmi)) /
              plsc->xpmm;
          plsc->boxbb_xmin = MIN(plsc->boxbb_xmin,
              pos_mm - 0.5 * string_length_mm);
          plsc->boxbb_xmax = MAX(plsc->boxbb_xmax,
              pos_mm + 0.5 * string_length_mm);
        } else {
          gdl_pmltex("b", height, pos, just, string);
        }
      }
      if (lmx) {
        // Top axis exponent.
        if (plsc->if_boxbb) {
          // For horizontal axes, height of zero corresponds
          // to character centred on edge so should add 0.5
          // to height to obtain bounding box edge in
          // direction away from edge if no exponent.  Add
          // an additional offset to make exponent fit.
          height_mm = (height + 1.4) * char_height_mm;
          plsc->boxbb_ymax = MAX(plsc->boxbb_ymax, plsc->vppyma /
              plsc->ypmm + height_mm);
          string_length_mm = gdl_plstrl(string);
          pos_mm = (plsc->vppxmi + pos *
              (plsc->vppxma - plsc->vppxmi)) /
              plsc->xpmm;
          plsc->boxbb_xmin = MIN(plsc->boxbb_xmin,
              pos_mm - 0.5 * string_length_mm);
          plsc->boxbb_xmax = MAX(plsc->boxbb_xmax,
              pos_mm + 0.5 * string_length_mm);
        } else {
          gdl_pmltex("t", height, pos, just, string);
        }
      }
    }
  }

  // Write label(s) for vertical axes.

  if ((lmy || lny) && (lty || lxy)) {
    PLINT ymode, yprec, ydigmax, ydigits, yscale;

    plgyax(&ydigmax, &ydigits);
    pldprec(vpwymi, vpwyma, ytick1, lfy, &ymode, &yprec, ydigmax, &yscale);

    ydigits = 0;
    if (ldy) {
      pldtfac(vpwymi, vpwyma, &factor, &tstart);
      tp = ytick1 * (1. + floor((vpwymi - tstart) / ytick1)) + tstart;
    } else {
      tp = ytick1 * (1. + floor(vpwymi / ytick1));
    }
    for (tn = tp; BETW(tn, vpwymi, vpwyma); tn += ytick1) {
      if (ldy) {
        //        strfqsas(string, STRING_LEN, timefmt, (double) tn, plsc->qsasconfig);
      } else {
        plform(PL_Y_AXIS, tn, yscale, yprec, string, STRING_LEN, lly, lfy, loy);
      }
      pos = (vpwymax > vpwymin) ?
          (tn - vpwymi) / (vpwyma - vpwymi) :
          (vpwyma - tn) / (vpwyma - vpwymi);
      if (lny) {
        if (lvy) {
          // Left axis with text written perpendicular to edge.
          height = liy ? 1.0 : 0.5;
          if (plsc->if_boxbb) {
            // For vertical axes with text written
            // perpendicular to edge, height of zero
            // corresponds character centred on edge so
            // should add 0.5 to height to obtain bounding
            // box edge in direction away from edge, and
            // that value apparently works.
            height_mm = (height + 0.0) * char_height_mm;
            string_length_mm = gdl_plstrl(string);
            plsc->boxbb_xmin = MIN(plsc->boxbb_xmin, plsc->vppxmi /
                plsc->xpmm - height_mm - string_length_mm);
            pos_mm = (plsc->vppymi + pos *
                (plsc->vppyma - plsc->vppymi)) /
                plsc->ypmm;
            // Expected offset is 0.5, but adjust to improve
            // look of result.
            plsc->boxbb_ymin = MIN(plsc->boxbb_ymin,
                pos_mm - 0.6 * char_height_mm);
            plsc->boxbb_ymax = MAX(plsc->boxbb_ymax,
                pos_mm + 0.7 * char_height_mm);
          } else {
            gdl_pmltex("lv", height, pos, 1.0, string);
          }
        } else {
          // Left axis with text written parallel to edge.
          height = liy ? 1.75 : 1.5;
          if (plsc->if_boxbb) {
            // For vertical axes with text written
            // parallel to edge, height of zero
            // corresponds to character centred on edge so
            // should add 0.5 to height to obtain bounding
            // box edge in direction away from edge,
            // However, experimentally found 0.8 gave a
            // better looking result.
            height_mm = (height + 0.8) * char_height_mm;
            plsc->boxbb_xmin = MIN(plsc->boxbb_xmin, plsc->vppxmi /
                plsc->xpmm - height_mm);
            pos_mm = (plsc->vppymi + pos *
                (plsc->vppyma - plsc->vppymi)) /
                plsc->ypmm;
            string_length_mm = gdl_plstrl(string);
            plsc->boxbb_ymin = MIN(plsc->boxbb_ymin,
                pos_mm - 0.5 * string_length_mm);
            plsc->boxbb_ymax = MAX(plsc->boxbb_ymax,
                pos_mm + 0.5 * string_length_mm);
          } else {
            gdl_pmltex("l", height, pos, 0.5, string);
          }
        }
      }
      if (lmy) {
        if (lvy) {
          // Right axis with text written perpendicular to edge.
          height = liy ? 1.0 : 0.5;
          if (plsc->if_boxbb) {
            // For vertical axes with text written
            // perpendicular to edge, height of zero
            // corresponds character centred on edge so
            // should add 0.5 to height to obtain bounding
            // box edge in direction away from edge, and
            // that value apparently works.
            height_mm = (height + 0.0) * char_height_mm;
            string_length_mm = gdl_plstrl(string);
            plsc->boxbb_xmax = MAX(plsc->boxbb_xmax, plsc->vppxma /
                plsc->xpmm + height_mm + string_length_mm);
            pos_mm = (plsc->vppymi + pos *
                (plsc->vppyma - plsc->vppymi)) /
                plsc->ypmm;
            // Expected offset is 0.5, but adjust to improve
            // look of result.
            plsc->boxbb_ymin = MIN(plsc->boxbb_ymin,
                pos_mm - 0.6 * char_height_mm);
            plsc->boxbb_ymax = MAX(plsc->boxbb_ymax,
                pos_mm + 0.7 * char_height_mm);
          } else {
            gdl_pmltex("rv", height, pos, 0.0, string);
          }
        } else {
          // Right axis with text written parallel to edge.
          height = liy ? 1.75 : 1.5;
          if (plsc->if_boxbb) {
            // For vertical axes with text written
            // parallel to edge, height of zero
            // corresponds to character centred on edge so
            // should add 0.5 to height to obtain bounding
            // box edge in direction away from edge,
            // However, experimentally found 0.8 gave a
            // better looking result.
            height_mm = (height + 0.8) * char_height_mm;
            plsc->boxbb_xmax = MAX(plsc->boxbb_xmax, plsc->vppxma /
                plsc->xpmm + height_mm);
            pos_mm = (plsc->vppymi + pos *
                (plsc->vppyma - plsc->vppymi)) /
                plsc->ypmm;
            string_length_mm = gdl_plstrl(string);
            plsc->boxbb_ymin = MIN(plsc->boxbb_ymin,
                pos_mm - 0.5 * string_length_mm);
            plsc->boxbb_ymax = MAX(plsc->boxbb_ymax,
                pos_mm + 0.5 * string_length_mm);
          } else {
            gdl_pmltex("r", height, pos, 0.5, string);
          }
        }
      }
      ydigits = MAX(ydigits, (PLINT) strlen(string));
    }
    if (!lvy)
      ydigits = 2;

    plsyax(ydigmax, ydigits);

    // Write separate exponential label if mode = 1.

    if (!lly && !ldy && !loy && ymode) {
      snprintf(string, STRING_LEN, "(x10%su%d%sd)", esc_string, (int) yscale, esc_string);
      if (custom_exponent_placement) {
        height = ((PLLabelDefaults *) plsc->label_data)->exp_label_disp;
        pos = ((PLLabelDefaults *) plsc->label_data)->exp_label_pos;
        just = ((PLLabelDefaults *) plsc->label_data)->exp_label_just;
      }
      if (lvy) {
        offset = 0.1; // more space to clear labels in "v" mode
      } else {
        offset = 0.02;
      }
      // Left axis exponent
      if (lny) {
        if (!custom_exponent_placement) {
          height = 3.2;
          pos = 1.0 + offset;
          just = 0.5;
        }
        if (plsc->if_boxbb) {
          // For horizontal axes, height of zero corresponds
          // to character centred on edge so should add 0.5
          // to height to obtain bounding box edge in
          // direction away from edge if no exponent.  Add
          // an additional offset to make exponent fit.
          height_mm = (height + 1.4) * char_height_mm;
          plsc->boxbb_ymax = MAX(plsc->boxbb_ymax, plsc->vppyma /
              plsc->ypmm + height_mm);
          string_length_mm = gdl_plstrl(string);
          pos_mm = (plsc->vppxmi + pos *
              (plsc->vppxma - plsc->vppxmi)) /
              plsc->xpmm;
          plsc->boxbb_xmin = MIN(plsc->boxbb_xmin,
              pos_mm - string_length_mm);
        } else {
          if (lvy) {
            gdl_pmltex("lv", height, pos, just, string);
          } else {
            gdl_pmltex("l", height, pos, just, string);
          }
        }
      }
      // Right axis exponent.
      if (lmy) {
        if (!custom_exponent_placement) {
          height = 3.4; // Extra space for superscript
          pos = 1.0 + offset;
          just = 0.5;
        }
        if (plsc->if_boxbb) {
          // For horizontal axes, height of zero corresponds
          // to character centred on edge so should add 0.5
          // to height to obtain bounding box edge in
          // direction away from edge if no exponent.  Add
          // an additional offset to make exponent fit.
          height_mm = (height + 1.4) * char_height_mm;
          plsc->boxbb_ymax = MAX(plsc->boxbb_ymax, plsc->vppyma /
              plsc->ypmm + height_mm);
          string_length_mm = gdl_plstrl(string);
          pos_mm = (plsc->vppxmi + pos *
              (plsc->vppxma - plsc->vppxmi)) /
              plsc->xpmm;
          plsc->boxbb_xmax = MAX(plsc->boxbb_xmin,
              pos_mm + string_length_mm);
        } else {
          if (lvy) {
            gdl_pmltex("rv", height, pos, just, string);
          } else {
            gdl_pmltex("r", height, pos, just, string);
          }
        }
      }
    }
  }
}

void
GDLGStream::gdl_c_plaxes(PLFLT x0, PLFLT y0,
    PLCHAR_VECTOR xopt, PLFLT xtick, PLINT nxsub,
    PLCHAR_VECTOR yopt, PLFLT ytick, PLINT nysub)
{
  PLBOOL lax, lbx, lcx, ldx, lgx, lix, llx, lsx, ltx, lux, lwx, lxx;
  PLBOOL lay, lby, lcy, ldy, lgy, liy, lly, lsy, lty, luy, lwy, lxy;
  PLINT xmajor, xminor, ymajor, yminor;
  PLINT i, i1x, i2x, i3x, i4x, i1y, i2y, i3y, i4y;
  PLINT nxsub1, nysub1;
  PLINT lxmin, lxmax, lymin, lymax;
  PLINT pxmin, pxmax, pymin, pymax;
  PLINT vppxmi, vppxma, vppymi, vppyma;
  PLFLT xtick1, ytick1, vpwxmi, vpwxma, vpwymi, vpwyma;
  PLFLT vpwxmin, vpwxmax, vpwymin, vpwymax;
  PLFLT xp0, yp0, tn, tp, temp;
  PLFLT factor, tstart;

  if (plsc->level < 3) {
    plabort("plbox: Please set up window first");
    return;
  }

  // Open the clip limits to the subpage limits

  plP_gclp(&lxmin, &lxmax, &lymin, &lymax);
  plP_gphy(&pxmin, &pxmax, &pymin, &pymax);
  plP_sclp(pxmin, pxmax, pymin, pymax);

  vppxmi = plsc->vppxmi;
  vppxma = plsc->vppxma;
  vppymi = plsc->vppymi;
  vppyma = plsc->vppyma;

  if (plsc->if_boxbb) {
    // Bounding-box limits for the box in mm before corrections
    // for decorations are applied.
    plsc->boxbb_xmin = plsc->vppxmi / plsc->xpmm;
    plsc->boxbb_xmax = plsc->vppxma / plsc->xpmm;
    plsc->boxbb_ymin = plsc->vppymi / plsc->ypmm;
    plsc->boxbb_ymax = plsc->vppyma / plsc->ypmm;
  }

  // Set plot options from input

  lax = plP_stsearch(xopt, 'a');
  lbx = plP_stsearch(xopt, 'b');
  lcx = plP_stsearch(xopt, 'c');
  ldx = plP_stsearch(xopt, 'd');
  lgx = plP_stsearch(xopt, 'g');
  lix = plP_stsearch(xopt, 'i');
  llx = plP_stsearch(xopt, 'l');
  lsx = plP_stsearch(xopt, 's');
  ltx = plP_stsearch(xopt, 't');
  lux = plP_stsearch(xopt, 'u');
  lwx = plP_stsearch(xopt, 'w');
  lxx = plP_stsearch(xopt, 'x');

  lay = plP_stsearch(yopt, 'a');
  lby = plP_stsearch(yopt, 'b');
  lcy = plP_stsearch(yopt, 'c');
  ldy = plP_stsearch(yopt, 'd');
  lgy = plP_stsearch(yopt, 'g');
  liy = plP_stsearch(yopt, 'i');
  lly = plP_stsearch(yopt, 'l');
  lsy = plP_stsearch(yopt, 's');
  lty = plP_stsearch(yopt, 't');
  luy = plP_stsearch(yopt, 'u');
  lwy = plP_stsearch(yopt, 'w');
  lxy = plP_stsearch(yopt, 'x');

  // Tick and subtick sizes in device coords

  xmajor = MAX(ROUND(plsc->majht * plsc->ypmm), 1);
  ymajor = MAX(ROUND(plsc->majht * plsc->xpmm), 1);
  xminor = MAX(ROUND(plsc->minht * plsc->ypmm), 1);
  yminor = MAX(ROUND(plsc->minht * plsc->xpmm), 1);

  nxsub1 = nxsub;
  nysub1 = nysub;
  xtick1 = llx ? 1.0 : xtick;
  ytick1 = lly ? 1.0 : ytick;

  plP_xgvpw(&vpwxmin, &vpwxmax, &vpwymin, &vpwymax);
  // vpwxmi always numerically less than vpwxma, and
  // similarly for vpwymi
  vpwxmi = (vpwxmax > vpwxmin) ? vpwxmin : vpwxmax;
  vpwxma = (vpwxmax > vpwxmin) ? vpwxmax : vpwxmin;
  vpwymi = (vpwymax > vpwymin) ? vpwymin : vpwymax;
  vpwyma = (vpwymax > vpwymin) ? vpwymax : vpwymin;

  // Plot axes only if they are inside viewport.
  lax = lax && vpwymi < y0 && y0 < vpwyma;
  lay = lay && vpwxmi < x0 && x0 < vpwxma;

  // Calculate tick spacing

  if (ltx || lgx || lxx)
    pldtik(vpwxmi, vpwxma, &xtick1, &nxsub1, ldx);

  if (lty || lgy || lxy)
    pldtik(vpwymi, vpwyma, &ytick1, &nysub1, ldy);
  // n.b. large change; xtick1, nxsub1, ytick1, nysub1 always positive.

  // Set up tick variables

  if (lix) {
    i1x = xminor;
    i2x = 0;
    i3x = xmajor;
    i4x = 0;
  } else {
    i1x = 0;
    i2x = xminor;
    i3x = 0;
    i4x = xmajor;
  }

  if (liy) {
    i1y = yminor;
    i2y = 0;
    i3y = ymajor;
    i4y = 0;
  } else {
    i1y = 0;
    i2y = yminor;
    i3y = 0;
    i4y = ymajor;
  }

  if (plsc->if_boxbb) {
    // Carefully follow logic below (and above) for the case where
    // an inverted major tick mark is written (in the X direction
    // for a Y axis and vice versa).  Ignore minor tick marks
    // which are assumed to be smaller.  Ignore axes and grids
    // which are all contained within the viewport.
    if (lix && (lbx || lux) && (ltx && !lxx))
      plsc->boxbb_ymin -= xmajor / plsc->ypmm;
    if (liy && (lcy || lwy) && (lty && !lxy))
      plsc->boxbb_xmax += ymajor / plsc->xpmm;
    if (lix && (lcx || lwx) && (ltx && !lxx))
      plsc->boxbb_ymax += xmajor / plsc->ypmm;
    if (liy && (lby || luy) && (lty && !lxy))
      plsc->boxbb_xmin -= ymajor / plsc->xpmm;
  } else {
    // Draw the bottom frame of the box

    if (lbx || lux) {
      if (!lux) {
        gdl_plP_movphy(vppxmi, vppymi);
        gdl_plP_draphy(vppxma, vppymi);
      }
      if (ltx && !lxx) {
        if (ldx) {
          pldtfac(vpwxmi, vpwxma, &factor, &tstart);
          tp = xtick1 * (floor((vpwxmi - tstart) / xtick1)) + tstart;
        } else
          tp = xtick1 * floor(vpwxmi / xtick1);
        for (;;) {
          tn = tp + xtick1;
          if (lsx) {
            if (llx) {
              for (i = 0; i <= 7; i++) {
                temp = tp + plplot_xlog[i];
                if (BETW(temp, vpwxmi, vpwxma))
                  gdl_plxtik(plP_wcpcx(temp), vppymi, i1x, i2x);
              }
            } else {
              for (i = 1; i <= nxsub1 - 1; i++) {
                temp = tp + i * xtick1 / nxsub1;
                if (BETW(temp, vpwxmi, vpwxma))
                  gdl_plxtik(plP_wcpcx(temp), vppymi, i1x, i2x);
              }
            }
          }
          if (!BETW(tn, vpwxmi, vpwxma))
            break;
          gdl_plxtik(plP_wcpcx(tn), vppymi, i3x, i4x);
          tp = tn;
        }
      }
    }

    // Draw the right-hand frame of box

    if (lcy || lwy) {
      if (!lwy) {
        gdl_plP_movphy(vppxma, vppymi);
        gdl_plP_draphy(vppxma, vppyma);
      }
      if (lty && !lxy) {
        if (ldy) {
          pldtfac(vpwymi, vpwyma, &factor, &tstart);
          tp = ytick1 * (floor((vpwymi - tstart) / ytick1)) + tstart;
        } else
          tp = ytick1 * floor(vpwymi / ytick1);
        for (;;) {
          tn = tp + ytick1;
          if (lsy) {
            if (lly) {
              for (i = 0; i <= 7; i++) {
                temp = tp + plplot_xlog[i];
                if (BETW(temp, vpwymi, vpwyma))
                  gdl_plytik(vppxma, plP_wcpcy(temp), i2y, i1y);
              }
            } else {
              for (i = 1; i <= nysub1 - 1; i++) {
                temp = tp + i * ytick1 / nysub1;
                if (BETW(temp, vpwymi, vpwyma))
                  gdl_plytik(vppxma, plP_wcpcy(temp), i2y, i1y);
              }
            }
          }
          if (!BETW(tn, vpwymi, vpwyma))
            break;
          gdl_plytik(vppxma, plP_wcpcy(tn), i4y, i3y);
          tp = tn;
        }
      }
    }

    // Draw the top frame of the box

    if (lcx || lwx) {
      if (!lwx) {
        gdl_plP_movphy(vppxma, vppyma);
        gdl_plP_draphy(vppxmi, vppyma);
      }
      if (ltx && !lxx) {
        if (ldx) {
          pldtfac(vpwxmi, vpwxma, &factor, &tstart);
          tp = xtick1 * (floor((vpwxma - tstart) / xtick1) + 1) + tstart;
        } else
          tp = xtick1 * (floor(vpwxma / xtick1) + 1);
        for (;;) {
          tn = tp - xtick1;
          if (lsx) {
            if (llx) {
              for (i = 7; i >= 0; i--) {
                temp = tn + plplot_xlog[i];
                if (BETW(temp, vpwxmi, vpwxma))
                  gdl_plxtik(plP_wcpcx(temp), vppyma, i2x, i1x);
              }
            } else {
              for (i = nxsub1 - 1; i >= 1; i--) {
                temp = tn + i * xtick1 / nxsub1;
                if (BETW(temp, vpwxmi, vpwxma))
                  gdl_plxtik(plP_wcpcx(temp), vppyma, i2x, i1x);
              }
            }
          }
          if (!BETW(tn, vpwxmi, vpwxma))
            break;
          gdl_plxtik(plP_wcpcx(tn), vppyma, i4x, i3x);
          tp = tn;
        }
      }
    }

    // Draw the left-hand frame of box

    if (lby || luy) {
      if (!luy) {
        gdl_plP_movphy(vppxmi, vppyma);
        gdl_plP_draphy(vppxmi, vppymi);
      }
      if (lty && !lxy) {
        if (ldy) {
          pldtfac(vpwymi, vpwyma, &factor, &tstart);
          tp = ytick1 * (floor((vpwyma - tstart) / ytick1) + 1) + tstart;
        } else
          tp = ytick1 * (floor(vpwyma / ytick1) + 1);
        for (;;) {
          tn = tp - ytick1;
          if (lsy) {
            if (lly) {
              for (i = 7; i >= 0; i--) {
                temp = tn + plplot_xlog[i];
                if (BETW(temp, vpwymi, vpwyma))
                  gdl_plytik(vppxmi, plP_wcpcy(temp), i1y, i2y);
              }
            } else {
              for (i = nysub1 - 1; i >= 1; i--) {
                temp = tn + i * ytick1 / nysub1;
                if (BETW(temp, vpwymi, vpwyma))
                  gdl_plytik(vppxmi, plP_wcpcy(temp), i1y, i2y);
              }
            }
          }
          if (!BETW(tn, vpwymi, vpwyma))
            break;
          gdl_plytik(vppxmi, plP_wcpcy(tn), i3y, i4y);
          tp = tn;
        }
      }
    }

    // Draw the horizontal axis.
    if (lax) {
      // Convert world coordinates to physical
      yp0 = plP_wcpcy(y0);
      gdl_plP_movphy(vppxmi, (PLINT) yp0);
      gdl_plP_draphy(vppxma, (PLINT) yp0);
      if (ltx && !lxx) {
        tp = xtick1 * floor(vpwxmi / xtick1);
        for (;;) {
          tn = tp + xtick1;
          if (lsx) {
            if (llx) {
              for (i = 0; i <= 7; i++) {
                temp = tp + plplot_xlog[i];
                if (BETW(temp, vpwxmi, vpwxma))
                  gdl_plxtik(plP_wcpcx(temp), (PLINT) yp0, xminor, xminor);
              }
            } else {
              for (i = 1; i <= nxsub1 - 1; i++) {
                temp = tp + i * xtick1 / nxsub1;
                if (BETW(temp, vpwxmi, vpwxma))
                  gdl_plxtik(plP_wcpcx(temp), (PLINT) yp0, xminor, xminor);
              }
            }
          }
          if (!BETW(tn, vpwxmi, vpwxma))
            break;
          gdl_plxtik(plP_wcpcx(tn), (PLINT) yp0, xmajor, xmajor);
          tp = tn;
        }
      }
    }

    // Draw the vertical axis.
    if (lay) {
      // Convert world coordinates to physical
      xp0 = plP_wcpcx(x0);
      gdl_plP_movphy((PLINT) xp0, vppymi);
      gdl_plP_draphy((PLINT) xp0, vppyma);
      if (lty && !lxy) {
        tp = ytick1 * floor(vpwymi / ytick1);
        for (;;) {
          tn = tp + ytick1;
          if (lsy) {
            if (lly) {
              for (i = 0; i <= 7; i++) {
                temp = tp + plplot_xlog[i];
                if (BETW(temp, vpwymi, vpwyma))
                  gdl_plytik((PLINT) xp0, plP_wcpcy(temp), yminor, yminor);
              }
            } else {
              for (i = 1; i <= nysub1 - 1; i++) {
                temp = tp + i * ytick1 / nysub1;
                if (BETW(temp, vpwymi, vpwyma))
                  gdl_plytik((PLINT) xp0, plP_wcpcy(temp), yminor, yminor);
              }
            }
          }
          if (!BETW(tn, vpwymi, vpwyma))
            break;
          gdl_plytik((PLINT) xp0, plP_wcpcy(tn), ymajor, ymajor);
          tp = tn;
        }
      }
    }

    // Draw grids.
    grid_box(xopt, xtick1, nxsub1, yopt, ytick1, nysub1);
  }

  // Write labels.
  label_box(xopt, xtick1, yopt, ytick1);

  // Restore the clip limits to viewport edge

  plP_sclp(lxmin, lxmax, lymin, lymax);
}
