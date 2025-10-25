/***************************************************************************
                       plotting.cpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002-2011 by Marc Schellens et al.
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"
#include "dinterpreter.hpp"
#include "plotting.hpp"
#include "math_utl.hpp"

#define LABELOFFSET 0.003
#define LABELSPACING 0.4

// sometime after 10/14 the tr0,tr1 moved from plstream:: to plcallback::

#define PLCALLBACK plcallback

namespace lib {

  using namespace std;

  // shared parameter
  bool xLog;
  bool yLog;
  bool zLog;

  void myrecordingfunction(PLFLT x, PLFLT y, PLFLT *xt, PLFLT *yt, PLPointer data) {
    *xt = x;
    *yt = y;
    fprintf(stderr, "x=%f,y=%f\n", x, y);
  }
  //kept out of object vaguely because plplot is in C, but should not be the case.

  PLINT doIt(PLFLT x, PLFLT y) {
    if (xLog && x <= 0) return 0;
    if (yLog && y <= 0) return 0;
    return 1;
  }
  
  class contour_call : public plotting_routine_call {
    DDoubleGDL *zVal, *yVal, *xVal;
    Guard<BaseGDL> xval_guard, yval_guard, zval_guard, p0_guard;
    DDoubleGDL *yValIrregularCase, *xValIrregularCase;
    SizeT xEl, yEl, zEl, ixEl, iyEl;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd;
    DDouble minVal, maxVal;
    bool isLog=false;
    bool overplot=false, nodata=false;
    DLongGDL *colors, *labels, *style;
    DFloatGDL* thick;
    Guard<BaseGDL> colors_guard, thick_guard, labels_guard, style_guard;
    DFloatGDL *spacing, *orientation;
    Guard<BaseGDL> spacing_guard, orientation_guard;
    bool doT3d = false;
    bool flat3d = false;
    bool irregular;
    bool iso;
    DDouble zValue;
    bool hasMinVal, hasMaxVal;
    bool recordPath;

    //PATH_XY etc: use actStream->stransform with a crafted recording function per level [lev-maxmax].
    //disentangle positive and negative contours with their rotation signature.
  private:

    bool handle_args(EnvT* e) {
      //T3D ?
      static int t3dIx = e->KeywordIx("T3D");
      doT3d = (e->BooleanKeywordSet(t3dIx) || T3Denabled()); //should check !P.T is OK 

      zValue = std::numeric_limits<DDouble>::quiet_NaN(); //NAN = no zValue?
      static int zvIx = e->KeywordIx("ZVALUE");
      if (doT3d && e->KeywordPresent(zvIx)) {
        e->AssureDoubleScalarKWIfPresent(zvIx, zValue);
        zValue = min(zValue, ZVALUEMAX); //to avoid problems with plplot
        zValue = max(zValue, 0.0);
        flat3d = true;
      }

      static int irregIx = e->KeywordIx("IRREGULAR");
      irregular = e->KeywordSet(irregIx);

      // in all cases, we have to exit here
      if (nParam() == 2 || nParam() > 3) {
        e->Throw("Incorrect number of arguments.");
      }
      if (irregular && nParam()==2) e->Throw("Incorrect number of arguments.");
      if (nParam() > 0) {
        // By testing here using EquivalentRank() we avoid computing zval if there was a problem.
        // AC 2018/04/24
        // a sub-array like: a=RANDOMU(seed, 3,4,5) & (this procedure name), a[1,*,*]
        // should be OK ...
        if (irregular) {} else if ((e->GetNumericArrayParDefined(0))->EquivalentRank() != 2) e->Throw("Array must have 2 dimensions: " + e->GetParString(0));
      }

      if (nParam() == 1) {
        BaseGDL* p0 = e->GetNumericArrayParDefined(0)->Transpose(NULL);
        p0_guard.Init(p0); // delete upon exit

        zVal = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
        zval_guard.Init(zVal); // delete upon exit

        xEl = zVal->Dim(1);
        yEl = zVal->Dim(0);

        xVal = new DDoubleGDL(dimension(xEl), BaseGDL::INDGEN);
        xval_guard.Init(xVal); // delete upon exit
        yVal = new DDoubleGDL(dimension(yEl), BaseGDL::INDGEN);
        yval_guard.Init(yVal); // delete upon exit
      }

      if (nParam() == 3) {
       //ZVal will be treated as 1 dim array and X and Y must have the same number of elements.
        BaseGDL* p0 = e->GetNumericArrayParDefined(0)->Transpose(NULL);
        p0_guard.Init(p0); // delete upon exit

        zVal = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
        zval_guard.Init(zVal); // delete upon exit
        if (irregular) {
 
          xValIrregularCase = e->GetWriteableParAs< DDoubleGDL>(1);
          yValIrregularCase = e->GetWriteableParAs< DDoubleGDL>(2);

          if (xValIrregularCase->N_Elements() != zVal->N_Elements())
            e->Throw("X, Y, or Z array dimensions are incompatible.");
          if (yValIrregularCase->N_Elements() != zVal->N_Elements())
            e->Throw("X, Y, or Z array dimensions are incompatible.");
          xEl = xValIrregularCase->N_Elements();
          yEl = yValIrregularCase->N_Elements(); //all points inside
          xVal = xValIrregularCase;
          yVal = yValIrregularCase; //for the time being, will be update later
        } else {
          xVal = e->GetWriteableParAs< DDoubleGDL>(1);
          yVal = e->GetWriteableParAs< DDoubleGDL>(2);
          //filter out incompatible ranks >2 or ==0
          if (xVal->Rank() > 2)
            e->Throw("X, Y, or Z array dimensions are incompatible.");
          if (yVal->Rank() > 2)
            e->Throw("X, Y, or Z array dimensions are incompatible.");
          if (xVal->Rank() == 0 || yVal->Rank() == 0)
            e->Throw("X, Y, or Z array dimensions are incompatible.");
          //filter out incompatible 1D dimensions
          if (xVal->Rank() == 1) {
            xEl = xVal->Dim(0);
            if (xEl != zVal->Dim(1))
              e->Throw("X, Y, or Z array dimensions are incompatible.");
          }
          if (yVal->Rank() == 1) {
            yEl = yVal->Dim(0);
            if (yEl != zVal->Dim(0))
              e->Throw("X, Y, or Z array dimensions are incompatible.");
          }
          //filter out incompatible 2D dimensions
          if (xVal->Rank() == 2) {
            xEl = xVal->Dim(0);
            if ((xVal->Dim(0) != zVal->Dim(1))&&(xVal->Dim(1) != zVal->Dim(0)))
              e->Throw("X, Y, or Z array dimensions are incompatible.");
          }
          if (yVal->Rank() == 2) {
            yEl = yVal->Dim(1);
            if ((yVal->Dim(0) != zVal->Dim(1))&&(yVal->Dim(1) != zVal->Dim(0)))
              e->Throw("X, Y, or Z array dimensions are incompatible.");
          }
          // But if X is 2D and Y is 1D (or reciprocally), we need to promote the 1D to 2D since this is supported by IDL
          if (xVal->Rank() == 1 && yVal->Rank() == 2) {
            DDoubleGDL* xValExpanded = new DDoubleGDL(zVal->Dim(), BaseGDL::NOZERO);
            SizeT k = 0;
            for (SizeT j = 0; j < zVal->Dim(1); ++j) for (SizeT i = 0; i < zVal->Dim(0); ++i) (*xValExpanded)[k++] = (*xVal)[i];
            xval_guard.Init(xValExpanded); // delete upon exit
            xVal = xValExpanded;
          } else if (xVal->Rank() == 2 && yVal->Rank() == 1) {
            DDoubleGDL* yValExpanded = new DDoubleGDL(zVal->Dim(), BaseGDL::NOZERO);
            SizeT k = 0;
            for (SizeT j = 0; j < zVal->Dim(1); ++j) for (SizeT i = 0; i < zVal->Dim(0); ++i) (*yValExpanded)[k++] = (*yVal)[j];
            xval_guard.Init(yValExpanded); // delete upon exit
            yVal = yValExpanded;
          }
//          //if there is a projection, we can use the irregular case, see:
//          bool mapSet = false;
//          get_mapset(mapSet);
//          if (mapSet) {
//            //if x and y are monodim, create a 2d mesh
//            if (xVal->Rank() == 1) { // so Y is 1 also
//              xValIrregularCase=new DDoubleGDL(xEl*yEl, BaseGDL::NOZERO);
//              yValIrregularCase=new DDoubleGDL(xEl*yEl, BaseGDL::NOZERO);
//              SizeT k=0;
//              for (SizeT j = 0; j < yEl; ++j) for (SizeT i = 0; i < xEl; ++i) 
//              { 
//                (*xValIrregularCase)[k] = (*xVal)[i];
//                (*yValIrregularCase)[k++] = (*yVal)[j];
//              }
//              irregular = true;
//            }
//            //else just say X and Y are the irregularcase
//            else {
//              xValIrregularCase=xVal;
//              yValIrregularCase=yVal;
//              irregular = true;
//            }
//          }        

        }
      }

      xLog = false;
      yLog = false;
      zLog = false;

      // handle Log options passing via Keywords
      // note: undocumented keywords [xyz]type still exist and
      // have priority on [xyz]log !
      static int xLogIx = e->KeywordIx("XLOG");
      static int yLogIx = e->KeywordIx("YLOG");
      static int zLogIx = e->KeywordIx("ZLOG");
      if (e->KeywordPresent(xLogIx)) xLog = e->KeywordSet(xLogIx);
      if (e->KeywordPresent(yLogIx)) yLog = e->KeywordSet(yLogIx);
      if (e->KeywordPresent(zLogIx)) zLog = e->KeywordSet(zLogIx);

      // note: undocumented keywords [xyz]type still exist and
      // have priority on [xyz]log ! In fact, it is the modulo (1, 3, 5 ... --> /log)   
      static int xTypeIx = e->KeywordIx("XTYPE");
      static int yTypeIx = e->KeywordIx("YTYPE");
      // ztype does not exist in IDL
      static int xType, yType;
      if (e->KeywordPresent(xTypeIx)) {
        e->AssureLongScalarKWIfPresent(xTypeIx, xType);
        if ((xType % 2) == 1) xLog = true;
        else xLog = false;
      }
      if (e->KeywordPresent(yTypeIx)) {
        e->AssureLongScalarKWIfPresent(yTypeIx, yType);
        if ((yType % 2) == 1) yLog = true;
        else yLog = false;
      }

      isLog = false;
      if (xLog || yLog) isLog = true;

      GetMinMaxVal(xVal, &xStart, &xEnd);
      GetMinMaxVal(yVal, &yStart, &yEnd);
      //XRANGE and YRANGE overrides all that, but  Start/End should be recomputed accordingly
      DDouble xAxisStart, xAxisEnd, yAxisStart, yAxisEnd;
      bool setx = gdlGetDesiredAxisRange(e, XAXIS, xAxisStart, xAxisEnd);
      bool sety = gdlGetDesiredAxisRange(e, YAXIS, yAxisStart, yAxisEnd);
      if (setx && sety) {
        xStart = xAxisStart;
        xEnd = xAxisEnd;
        yStart = yAxisStart;
        yEnd = yAxisEnd;
      } else if (sety) {
        yStart = yAxisStart;
        yEnd = yAxisEnd;
      } else if (setx) {
        xStart = xAxisStart;
        xEnd = xAxisEnd;
        //must compute min-max for other axis!
        {
          gdlDoRangeExtrema(xVal, yVal, yStart, yEnd, xStart, xEnd, false);
        }
      }
      // z range
      minVal = maxVal = 0;
      GetMinMaxVal(zVal, &minVal, &maxVal);
      zStart = minVal;
      zEnd = maxVal;
      bool setz = gdlGetDesiredAxisRange(e, ZAXIS, zStart, zEnd);

      //minVal and maxVal are values to make the contours
      static int MIN_VALUE = e->KeywordIx("MIN_VALUE");
      static int MAX_VALUE = e->KeywordIx("MAX_VALUE");
      hasMinVal = e->KeywordPresent(MIN_VALUE);
      hasMaxVal = e->KeywordPresent(MAX_VALUE);
      e->AssureDoubleScalarKWIfPresent(MIN_VALUE, minVal);
      e->AssureDoubleScalarKWIfPresent(MAX_VALUE, maxVal);

      return false; //do not abort
    }

  private:

    bool prepareDrawArea(EnvT* e, GDLGStream* actStream) {
      //OVERPLOT: get stored range values instead to use them!
      static int overplotKW = e->KeywordIx("OVERPLOT");
      overplot = e->KeywordSet(overplotKW);
      if (overplot) {
        //get DATA limits (not necessary CRANGE, see AXIS / SAVE behaviour!)
        GetCurrentUserLimits(xStart, xEnd, yStart, yEnd);
      } else {

        //ISOTROPIC
        static int ISOTROPIC = e->KeywordIx("ISOTROPIC");
        iso = e->BooleanKeywordSet(ISOTROPIC);
        // background BEFORE next plot since it is the only place plplot may redraw the background...
        gdlSetGraphicsBackgroundColorFromKw(e, actStream);
        //start a plot
        gdlNextPlotHandlingNoEraseOption(e, actStream); //NOERASE

        //Box adjustement:
        gdlAdjustAxisRange(e, XAXIS, xStart, xEnd, xLog);
        gdlAdjustAxisRange(e, YAXIS, yStart, yEnd, yLog);
        gdlAdjustAxisRange(e, ZAXIS, zStart, zEnd, zLog);


        if (xLog && xStart <= 0.0) Warning("CONTOUR: Infinite x plot range.");
        if (yLog && yStart <= 0.0) Warning("CONTOUR: Infinite y plot range.");
        if (zLog && zStart <= 0.0) Warning("CONTOUR: Infinite z plot range.");

        // viewport and world coordinates
        // set the PLOT charsize before setting viewport (margin depend on charsize)
        gdlSetPlotCharsize(e, actStream);
        zValue = gdlSetViewPortAndWorldCoordinates(e, actStream, xStart, xEnd, xLog, yStart, yEnd, yLog, zStart, zEnd, zLog, zValue, iso);

        if (doT3d) gdlStartT3DMatrixDriverTransform(actStream, zValue); //call for driver to perform special transform for all further drawing

        //current pen color...
        gdlSetGraphicsForegroundColorFromKw(e, actStream);
        gdlBox(e, actStream, xStart, xEnd, xLog, yStart, yEnd, yLog);

        if (doT3d) gdlStop3DDriverTransform(actStream);
      }

      if (gdlSwitchToClippedNormalizedCoordinates(e, actStream)) return true; //normal clip meaning

      return false;
    }

  private:

// FOR AN UNKNOWN REASON OPTIMZING THIS TOO MUCH PREVENTS Basic_Contour_Plot of test_coyote to pass!!! FIXME!
#pragma GCC push_options
#pragma GCC optimize ("O0")
    void applyGraphics(EnvT* e, GDLGStream* actStream) {
      static int nodataIx = e->KeywordIx("NODATA");
      if (e->KeywordSet(nodataIx)) return; //will perform post_call

      // we need to define the NaN value
      DStructGDL *Values = SysVar::Values(); //MUST NOT BE STATIC, due to .reset 
      static DDouble d_nan = (*static_cast<DDoubleGDL*> (Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0];
      static DDouble minmin = std::numeric_limits<PLFLT>::min();
      static DDouble maxmax = std::numeric_limits<PLFLT>::max();

      //We could RECORD PATH this way. Not developed since PATH_INFO seems not to be used
      static int pathinfoIx = e->KeywordIx("PATH_INFO");
      static int pathxyIx = e->KeywordIx("PATH_XY");
      recordPath = (e->KeywordSet(pathinfoIx) || e->KeywordSet(pathxyIx));
      if (recordPath) {
        //recordPath--> use actStream->stransform(myrecordingfunction, &data);
        //       actStream->stransform(myrecordingfunction, NULL);
        Warning("PATH_INFO, PATH_XY not yet supported, (FIXME)");
        recordPath = false;
      }

      // labeling
      // initiated by /FOLLOW.
      // C_ANNOTATION=vector of strings:  strings replace the default numerical values. Implies FOLLOW. Impossible with PLPLOT!
      // C_CHARSIZE. IDL doc false: c_charsize independent from /CHARSIZE. Charsize should be 3/4 size of AXIS LABELS,
      // but in fact c_charsize is independent from all [XYZ]charsize setups. Implies FOLLOW.
      // C_CHARTHICK Implies FOLLOW.
      // C_COLORS=vector, (eventually converted to integer), give color index. repated if less than contours.
      // C_LABELS=vector of 0 and 1 (float, double, int) . Implies FOLLOW.
      // C_LINESTYLE =vector of linestyles. Defaults to !P.LINESTYLE
      // C_ORIENTATION = vector of angles of lines to  FILL (needs FILL KW) .
      // C_SPACING= vector of spacing in CENTIMETRES of lines to  FILL (needs FILL KW) .
      // if C_SPACING and C_ORIENTATION absent, FILL will do a solid fill .
      // C_THICK=vector of thickness. repated if less than contours. defaults to !P.THICK or THICK

      static int FOLLOW = e->KeywordIx("FOLLOW");
      static int C_ANNOTATION = e->KeywordIx("C_ANNOTATION");
      static int C_CHARSIZE = e->KeywordIx("C_CHARSIZE");
      static int C_CHARTHICK = e->KeywordIx("C_CHARTHICK");
      static int C_LABELS = e->KeywordIx("C_LABELS");
      bool dolabels = false;
      static int FILL = e->KeywordIx("FILL");
      static int CELL_FILL = e->KeywordIx("CELL_FILL");
      static int C_SPACING = e->KeywordIx("C_SPACING");
      bool dospacing = false;
      static int C_ORIENTATION = e->KeywordIx("C_ORIENTATION");
      bool doori = false;
      bool label = (e->KeywordSet(FOLLOW) || e->KeywordSet(C_CHARSIZE) || e->KeywordSet(C_CHARTHICK) || e->KeywordSet(C_LABELS) || e->KeywordSet(C_ANNOTATION));
      bool fill = (e->KeywordSet(FILL) || e->KeywordSet(CELL_FILL) || e->KeywordSet(C_SPACING) || e->KeywordSet(C_ORIENTATION));
      if (fill) label = false; //mutually exclusive
      if (recordPath) {
        fill = true;
      }

      // managing the levels list OR the nlevels value
      // LEVELS=vector_of_values_in_increasing_order
      // NLEVELS=[1..60]
      DDouble cmax, cmin;
      cmin = zStart;
      cmax = zEnd;
      PLINT nlevel=0;
      PLFLT *clevel=NULL;
      ArrayGuard<PLFLT> clevel_guard;
      static int levelsix = e->KeywordIx("LEVELS");
      BaseGDL* b_levels = e->GetKW(levelsix);
      if (b_levels != NULL) {
        DDoubleGDL* d_levels = e->GetKWAs<DDoubleGDL>(levelsix);
        nlevel = d_levels-> N_Elements();
        clevel = (PLFLT *) &(*d_levels)[0];
        // are the levels ordered ?
        for (SizeT i = 1; i < nlevel; i++) {
          if (clevel[i] <= clevel[i - 1])
            e->Throw("Contour levels must be in increasing order.");
        }
      } else {
        PLFLT zintv;
        static int NLEVELS = e->KeywordIx("NLEVELS");
        if (e->KeywordSet(NLEVELS)) {
          DLong l_nlevel = nlevel; // GCC 3.4.4 needs that
          e->AssureLongScalarKWIfPresent(NLEVELS, l_nlevel);
          nlevel = l_nlevel;
          if (nlevel < 0) nlevel = 2; //as IDL
          if (nlevel == 0) nlevel = 3; //idem
          zintv = (cmax - cmin) / (nlevel + 1);
        } else {
          zintv = AutoTickIntv(cmax - cmin, true); //zintv never null.
          nlevel = (PLINT) floor((cmax - cmin) / zintv);
          if (nlevel <= 0) nodata = true; else nodata=false; //yes, happens if image has constant value...
        }
        if (fill) {
          nlevel = nlevel + 1;
        }
        clevel = new PLFLT[nlevel];
        clevel_guard.Reset(clevel);
        //IDL does this:

        if (zLog) for (SizeT i = 1; i <= nlevel; i++) clevel[i - 1] = pow(10.0, (zintv * i + cmin));
        else for (SizeT i = 1; i <= nlevel; i++) clevel[i - 1] = zintv * i + cmin;
      }

      // set label params always since they are not forgotten by plplot.'label' tells if they should be used.
      // IDL default: 3/4 of the axis charsize (CHARSIZE keyword or !P.CHARSIZE)
      // PlPlot default: .3
      // should be: DFloat label_size=.75*actStream->charScale(); however IDL doc false (?).
      DFloat label_size = 0.9; //IDL behaviour, IDL doc is false, label of contours is not 3/4 of !P.CHARSIZE or CHARSIZE
      if (e->KeywordSet(C_CHARSIZE)) e->AssureFloatScalarKWIfPresent(C_CHARSIZE, label_size);
      // set up after:      actStream->setcontlabelparam ( LABELOFFSET, (PLFLT) label_size, LABELSPACING, (label)?1:0 );
      DFloat label_thick = 1;
      if (e->KeywordSet(C_CHARTHICK)) e->AssureFloatScalarKWIfPresent(C_CHARTHICK, label_thick);
      actStream->setcontlabelformat(4, 3);

      // PLOT ONLY IF NODATA=0
      if (!nodata) {

        //here every postion XYZ must be in NORMalized coordinates to follow convention above.
        //use of intermediate map for correct handling of blanking values and nans. We take advantage of the fact that
        //this program makes either filled regions with plshades() [but plshades hates Nans!] or contours with plcont,
        //which needs Nans to avoid blanked regions. The idea is to mark unwanted regions with Nans for plcont, and
        //with a blanking value (minmin) for plshade. Eventually one could use a zdefined() function testing on top of it.
        PLFLT ** map;

        if (irregular) {
          //x-y ranges:
          DDouble xmin, xmax, ymin, ymax;
          long xsize, ysize;
          actStream->GetGeometry(xsize, ysize);

          //if projection active, convert pairs
          SelfProjectXY(xValIrregularCase, yValIrregularCase);
          SizeT nEl=xValIrregularCase->N_Elements();
          GetMinMaxVal(xValIrregularCase, &xmin, &xmax);
          GetMinMaxVal(yValIrregularCase, &ymin, &ymax);
          if (!isfinite(xmin)||!isfinite(xmax)||!isfinite(ymin)||!isfinite(ymax)) return; //possible leak
          //filter out z values whre x or y is nan
          SizeT finalSize=0;
          for (SizeT i = 0; i < nEl; ++i) if (isfinite((*xValIrregularCase)[i]) && isfinite((*yValIrregularCase)[i])) finalSize++;
          if (finalSize != nEl) {
            DDoubleGDL* newxValIrregularCase= new DDoubleGDL(finalSize,BaseGDL::NOZERO);
            DDoubleGDL* newyValIrregularCase= new DDoubleGDL(finalSize,BaseGDL::NOZERO);
            DDoubleGDL* newzVal= new DDoubleGDL(finalSize,BaseGDL::NOZERO);
            SizeT k=0;
            for (SizeT i = 0; i < nEl; ++i) if (isfinite((*xValIrregularCase)[i]) && isfinite((*yValIrregularCase)[i])) {
              (*newxValIrregularCase)[k]=(*xValIrregularCase)[i];
              (*newyValIrregularCase)[k]=(*yValIrregularCase)[i];
              (*newzVal)[k]=(*zVal)[i];
              k++;
            }
            //TODO: insure we recover the loss memory of this:
            xValIrregularCase=newxValIrregularCase;
            yValIrregularCase=newyValIrregularCase;
            zVal=newzVal;
            nEl=finalSize;
          }
          // find a good compromise for default size of gridded map...
          ixEl = max(51.0, 2 * sqrt((double) nEl) + 1); //preferably odd
          iyEl = max(51.0, 2 * sqrt((double) nEl) + 1);
          ixEl = ixEl < xsize ? ixEl : xsize; //no more than pixels on screen!
          iyEl = iyEl < ysize ? iyEl : ysize;
          xVal = new DDoubleGDL(dimension(ixEl), BaseGDL::NOZERO);
          yVal = new DDoubleGDL(dimension(iyEl), BaseGDL::NOZERO);
          for (SizeT i = 0; i < ixEl; ++i) (*xVal)[i] = xmin + i * (xmax - xmin) / ixEl;
          for (SizeT i = 0; i < iyEl; ++i) (*yVal)[i] = ymin + i * (ymax - ymin) / iyEl;
          actStream->Alloc2dGrid(&map, ixEl, iyEl);
          int griddataIx = e->Interpreter()->GetFunIx("GRIDDATA");
          if (griddataIx < 0) e->Throw("internal error in CONTOUR, please report");
          // GDL magick
          EnvUDT* newEnv = new EnvUDT(NULL, funList[griddataIx], NULL);
          // add parameters
          DLongGDL* theDimension=new DLongGDL(dimension(2)); (* theDimension)[0]=ixEl; (* theDimension)[1]=iyEl;
          newEnv->SetNextPar(xValIrregularCase);
          newEnv->SetNextPar(yValIrregularCase);
          newEnv->SetNextPar(zVal);
          newEnv->SetKeyword("DIMENSION",theDimension);
          bool mapSet = false;
          get_mapset(mapSet);
          if (mapSet) newEnv->SetKeyword("SPHERE",new DLongGDL(1));
          if (mapSet) newEnv->SetKeyword("DEGREES",new DLongGDL(1));
          BaseGDL::interpreter->CallStack().push_back(newEnv);
          // make the call
          DDoubleGDL* res = (DDoubleGDL*)BaseGDL::interpreter->call_fun(funList[ griddataIx]->GetTree());
          BaseGDL::interpreter->CallStack().pop_back();
          for (SizeT j = 0, k = 0; j < iyEl; j++) {
            for (SizeT i = 0; i < ixEl; i++) {
              PLFLT v = (*res)[k++];
              if (!isfinite(v)) v = (fill) ? minmin : d_nan; //note: nan regions could eventually be filled.
              if (hasMinVal && v < minVal) v = (fill) ? minmin : d_nan;
              if (hasMaxVal && v > maxVal) v = (fill) ? maxmax : d_nan;
              map[i][j] = v;
            }
          }
          xEl = ixEl;
          yEl = iyEl;
        } else {
          actStream->Alloc2dGrid(&map, xEl, yEl);
          for (SizeT i = 0, k = 0; i < xEl; i++) {
            for (SizeT j = 0; j < yEl; j++) {
              PLFLT v = (*zVal)[k++];
              if (!isfinite(v)) v = (fill) ? minmin : d_nan; //note: nan regions could eventually be filled.
              if (hasMinVal && v < minVal) v = (fill) ? minmin : d_nan;
              if (hasMaxVal && v > maxVal) v = (fill) ? maxmax : d_nan;
              map[i][j] = v;
            }
          }
        }

        //Good place for conversion to normed values
        COORDSYS coordinateSystem = DATA;
        SelfConvertToNormXY(xVal, xLog, yVal, yLog, coordinateSystem); //DATA

        if (flat3d) {
          gdlStartT3DMatrixDriverTransform(actStream, zValue); //call for driver to perform special transform for all further drawing. No further 3D case as everyting is handled by the driver.
          doT3d = false;
        } // else we have to do it ourselves for each contour

        // provision for 2 types of grids.
        PLcGrid cgrid1; // X and Y independent deformation
        PLFLT* xg1;
        PLFLT* yg1;
        PLcGrid2 cgrid2; // Dependent X Y (e.g., rotation)
        bool tidyGrid1WorldData = false;
        bool tidyGrid2WorldData = false;
        bool oneDim = true;
        bool rank1 = (xVal->Rank() == 1 && yVal->Rank() == 1);
        // the Grids:
        // 1 DIM X & Y
        if (rank1) {
          oneDim = true;
          xg1 = new PLFLT[xEl];
          yg1 = new PLFLT[yEl];
          cgrid1.xg = xg1;
          cgrid1.yg = yg1;
          cgrid1.nx = xEl;
          cgrid1.ny = yEl;
          for (SizeT i = 0; i < xEl; i++) cgrid1.xg[i] = (*xVal)[i];
          for (SizeT i = 0; i < yEl; i++) cgrid1.yg[i] = (*yVal)[i];
          tidyGrid1WorldData = true;
        } else //rank 2
        {
          oneDim = false;
          actStream->Alloc2dGrid(&cgrid2.xg, xEl, yEl);
          actStream->Alloc2dGrid(&cgrid2.yg, xEl, yEl);
          tidyGrid2WorldData = true;
          cgrid2.nx = xEl;
          cgrid2.ny = yEl;
          for (SizeT i = 0; i < xEl; i++) {
            for (SizeT j = 0; j < yEl; j++) {
              cgrid2.xg[i][j] = (*xVal)[j * (xEl) + i];
              cgrid2.yg[i][j] = (*yVal)[j * (xEl) + i];
            }
          }
        }

        //Colors.
        DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
        static int c_colorsIx = e->KeywordIx("C_COLORS");
        bool docolors = false;
        // Get decomposed value for colors
        DLong decomposed = GraphicsDevice::GetDevice()->GetDecomposed();
        if (e->GetKW(c_colorsIx) != NULL) {
          colors = e->GetKWAs<DLongGDL>(c_colorsIx);
          docolors = true;
        } else {
          // Get COLOR from PLOT system variable
          colors = new DLongGDL(1, BaseGDL::NOZERO);
          colors_guard.Init(colors); // delete upon exit
          colors = static_cast<DLongGDL*> (pStruct->GetTag(pStruct->Desc()->TagIndex("COLOR"), 0));
          docolors = false;
        }
        //treat decomposed problem here (simpler: colors for table 0 will already be truncated to 0xFF)
        if (!decomposed) for (SizeT i = 0; i < colors->N_Elements(); ++i) {
            (*colors)[i] &= 0xFF;
          }

        static int c_linestyleIx = e->KeywordIx("C_LINESTYLE");
        bool dostyle = false;
        static int c_thickIx = e->KeywordIx("C_THICK");
        bool dothick = false;

        if (e->GetKW(c_thickIx) != NULL) {
          thick = e->GetKWAs<DFloatGDL>(c_thickIx);
          dothick = true;
        } else {
          // Get THICK from PLOT system variable
          thick = new DFloatGDL(1, BaseGDL::NOZERO);
          thick_guard.Init(thick); // delete upon exit
          (*thick) = gdlGetPenThickness(e, actStream);
          dothick = false;
        }
        if (e->GetKW(C_LABELS) != NULL) {
          labels = e->GetKWAs<DLongGDL>(C_LABELS);
          dolabels = true;
        } else //every other level
        {
          labels = new DLongGDL(dimension(nlevel), BaseGDL::ZERO);
          labels_guard.Init(labels);
          (*labels)[0]=1;
          for (SizeT i = 1; i < nlevel; ++i) (*labels)[i] = (i - 1) % 2; //by default every other contour is labeled
          if (label) dolabels = true; //yes!
        }
        if (e->GetKW(c_linestyleIx) != NULL) {
          style = e->GetKWAs<DLongGDL>(c_linestyleIx);
          dostyle = true;
        }
        if (e->GetKW(C_ORIENTATION) != NULL) {
          orientation = e->GetKWAs<DFloatGDL>(C_ORIENTATION);
          doori = true;
        } else {
          orientation = new DFloatGDL(dimension(1), BaseGDL::ZERO);
          orientation_guard.Init(orientation);
          (*orientation)[0] = 0;
        }
        if (e->GetKW(C_SPACING) != NULL) {
          spacing = e->GetKWAs<DFloatGDL>(C_SPACING);
          dospacing = true;
        } else {
          spacing = new DFloatGDL(dimension(1), BaseGDL::ZERO);
          spacing_guard.Init(spacing);
          (*spacing)[0] = 0.25;
        }
        bool hachures = (dospacing || doori);

        //provides some defaults:
        if (!docolors) gdlSetGraphicsForegroundColorFromKw(e, actStream);
        if (!dothick) gdlSetPenThickness(e, actStream);
        gdlSetPlotCharsize(e, actStream);
        actStream->psty(0); //solid fill by default!

        PLFLT value; //used for all filled contours: they use colortable 1; value is between 0 and 1.
        if (docolors) actStream->SetColorMap1Table(nlevel, colors, decomposed); //load colormap1 with given colors (decomposed or not))
        else actStream->SetColorMap1DefaultColors(nlevel, decomposed);

        if (fill) {
          if (hachures) {
            PLINT ori;
            PLINT spa;

            actStream->psty(1);
            // C_ORIENTATION = vector of angles of lines to  FILL (needs FILL KW) .
            // C_SPACING= vector of spacing in CENTIMETRES of lines to  FILL (needs FILL KW) .
            // if C_SPACING and C_ORIENTATION absent, FILL will do a solid fill .
            for (SizeT i = 0; i < nlevel - 1; ++i) {
              if (doT3d) gdlStartT3DMatrixDriverTransform(actStream, (clevel[i] - cmin) / (cmax - cmin));
              ori = floor(10.0 * (*orientation)[i % orientation->N_Elements()]);
              spa = floor(10000 * (*spacing)[i % spacing->N_Elements()]);
              value = static_cast<PLFLT> (i) / nlevel;
              if (dothick) actStream->Thick((*thick)[i % thick->N_Elements()]);
              if (dostyle) gdlLineStyle(actStream, (*style)[i % style->N_Elements()]); //not working; plplot bug see https://sourceforge.net/p/plplot/bugs/111
              //the background
              actStream->psty(0);
              gdlSetGraphicsForegroundColorFromBackgroundKw(e,actStream); //patterns written on background, not transparents!
              actStream->shade(map, xEl, yEl, isLog ? doIt : NULL, xStart, xEnd, yStart, yEnd,
                clevel[i], maxmax,
                1,  0, //color + background
                static_cast<PLFLT> ((*thick)[i % thick->N_Elements()]),
                0, 0, 0, 0,
                (PLCALLBACK::fill), (oneDim),
                (oneDim) ? (PLCALLBACK::tr1) : (PLCALLBACK::tr2), (oneDim) ? (void *) &cgrid1 : (void *) &cgrid2);
              //the pattern, now
              gdlSetGraphicsForegroundColorFromKw ( e, actStream );
              actStream->pat(1, &ori, &spa);
              actStream->shade(map, xEl, yEl, isLog ? doIt : NULL, xStart, xEnd, yStart, yEnd,
                clevel[i], maxmax,
                (docolors) ? 1 : 0, (docolors) ? value : 255,
                static_cast<PLFLT> ((*thick)[i % thick->N_Elements()]),
                0, 0, 0, 0,
                (PLCALLBACK::fill), (oneDim),
                (oneDim) ? (PLCALLBACK::tr1) : (PLCALLBACK::tr2), (oneDim) ? (void *) &cgrid1 : (void *) &cgrid2);
              actStream->psty(0);
            }
            //            if (docolors) gdlSetGraphicsForegroundColorFromKw( e, actStream );
            //            if (dothick) gdlSetPenThickness(e, actStream);
            //            if (dostyle) gdlLineStyle(actStream, 0);
          }//end FILL with equispaced lines
          else if (doT3d) { //contours will be filled with solid color and displaced in Z according to their value
            for (SizeT i = 0; i < nlevel; ++i) {
              gdlStartT3DMatrixDriverTransform(actStream, (clevel[i] - cmin) / (cmax - cmin));

              value = static_cast<PLFLT> (i) / nlevel;
              actStream->shade(map, xEl, yEl, isLog ? doIt : NULL,
                xStart, xEnd, yStart, yEnd,
                clevel[i], maxmax,
                1, value,
                0, 0, 0, 0, 0,
                PLCALLBACK::fill, (oneDim), //Onedim is accelerator since rectangles are kept rectangles see plplot doc
                (oneDim) ? (PLCALLBACK::tr1) : (PLCALLBACK::tr2), (oneDim) ? (void *) &cgrid1 : (void *) &cgrid2);
            }
          } else { //every other case of fill 
            // note that plshades is not protected against 1 level (color formula is
            // "shade_color = color_min + i / (PLFLT) ( nlevel - 2 ) * color_range;"
            // meaning that nlevel=xx must be xx>=2 for plshades to work!)
            if (nlevel > 2 && !(docolors)) { //acceleration with shades when no c_colors are given. use continuous table1, decomposed or not.
              //acceleration is most sensible when a (x,y) transform (rotate, stretch) is in place since plplot does not recompute the map.
              actStream->shades(map, xEl, yEl, isLog ? doIt : NULL, xStart, xEnd, yStart, yEnd,
                clevel, nlevel, 1, 0, 0, PLCALLBACK::fill, (oneDim),
                (oneDim) ? (PLCALLBACK::tr1) : (PLCALLBACK::tr2),
                (oneDim) ? (void *) &cgrid1 : (void *) &cgrid2);
            } else { //fill with colors defined with c_colors or n<=2
              for (SizeT i = 0; i < nlevel; ++i) {
                value = static_cast<PLFLT> (i) / nlevel;
                actStream->shade(map, xEl, yEl, isLog ? doIt : NULL,
                  xStart, xEnd, yStart, yEnd,
                  clevel[i], maxmax,
                  1, value,
                  0, 0, 0, 0, 0,
                  PLCALLBACK::fill, (oneDim), //Onedim is accelerator since rectangles are kept rectangles see plplot doc
                  (oneDim) ? (PLCALLBACK::tr1) : (PLCALLBACK::tr2), (oneDim) ? (void *) &cgrid1 : (void *) &cgrid2);
              }
            }
          }
        } else { //no fill = contours . use normal pen procedures.
          if (!docolors) gdlSetGraphicsForegroundColorFromKw(e, actStream);
          DFloat referencePenThickness;
          if (!dothick) {
            gdlSetPenThickness(e, actStream);
            referencePenThickness = gdlGetPenThickness(e, actStream);
          }
          gdlSetPlotCharsize(e, actStream);
          for (SizeT i = 0; i < nlevel; ++i) {
            if (doT3d) gdlStartT3DMatrixDriverTransform(actStream, (clevel[i] - cmin) / (cmax - cmin));
            if (docolors) actStream->Color((*colors)[i % colors->N_Elements()], decomposed);
            if (dothick) {
              actStream->Thick((*thick)[i % thick->N_Elements()]);
              referencePenThickness = (*thick)[i % thick->N_Elements()];
            }
            if (dostyle) gdlLineStyle(actStream, (*style)[i % style->N_Elements()]);
            if (dolabels && i < labels->N_Elements()) {
              if (label_thick < referencePenThickness) { //one pass with (current) thick without labels, over with (smaller) label+contour.
                //else (lables thicker than contours) impossible with plplot...
                actStream->setcontlabelparam(LABELOFFSET, (PLFLT) label_size, LABELSPACING, 0);
                actStream->cont(map, xEl, yEl, 1, xEl, 1, yEl, &(clevel[i]), 1,
                  (oneDim) ? (PLCALLBACK::tr1) : (PLCALLBACK::tr2), (oneDim) ? (void *) &cgrid1 : (void *) &cgrid2); //thick contours, no label
                actStream->Thick(label_thick);
              }
              actStream->setcontlabelparam(LABELOFFSET, (PLFLT) label_size, LABELSPACING * sqrt(label_size), (PLINT) (*labels)[i]);
              actStream->cont(map, xEl, yEl, 1, xEl, 1, yEl, &(clevel[i]), 1,
                (oneDim) ? (PLCALLBACK::tr1) : (PLCALLBACK::tr2), (oneDim) ? (void *) &cgrid1 : (void *) &cgrid2);
              if (!dothick) gdlSetPenThickness(e, actStream);
            } else {
              actStream->setcontlabelparam(0,0,0,0);
              actStream->cont(map, xEl, yEl, 1, xEl, 1, yEl, &(clevel[i]), 1,
                (oneDim) ? (PLCALLBACK::tr1) : (PLCALLBACK::tr2), (oneDim) ? (void *) &cgrid1 : (void *) &cgrid2);
            }
          }
          //          if (docolors) gdlSetGraphicsForegroundColorFromKw ( e, actStream );
          //          if (dothick) gdlSetPenThickness(e, actStream);
          //          if (dostyle) gdlLineStyle(actStream, 0);
        }
        if (tidyGrid2WorldData) {
          actStream->Free2dGrid(cgrid2.xg, xEl, yEl);
          actStream->Free2dGrid(cgrid2.yg, xEl, yEl);
        }
        if (tidyGrid1WorldData) {
          delete[] xg1;
          delete[] yg1;
        }

//        if (do_free_grid) 
        actStream->Free2dGrid(map, xEl, yEl);
      }

      //restore color for boxes
      gdlSetGraphicsForegroundColorFromKw(e, actStream); //COLOR
      //reset driver to 2D plotting routines in all cases
      gdlStop3DDriverTransform(actStream);
    }
#pragma GCC pop_options

  private:

    virtual void post_call(EnvT*, GDLGStream * actStream) {
      if (recordPath) actStream->stransform(NULL, NULL);
      actStream->setcontlabelparam(0,0,0,0);
      actStream->lsty(1); //reset linestyle
      actStream->sizeChar(1.0);
    }

  }; // contour_call class

  void contour(EnvT* e) {
    contour_call contour;
    contour.call(e, 1);
  }

} // namespace
#undef PLCALLBACK
