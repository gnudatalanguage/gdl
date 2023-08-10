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
static GDL_3DTRANSFORMDEVICE PlotDevice3D;
#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#define TONORMCOORDX( in, out, log) out = (log) ? sx[0] + sx[1] * log10(in) : sx[0] + sx[1] * in;
//#define TODATACOORDX( in, out, log) out = (log) ? pow(10.0, (in -sx[0])/sx[1]) : (in -sx[0])/sx[1];
#define TONORMCOORDY( in, out, log) out = (log) ? sy[0] + sy[1] * log10(in) : sy[0] + sy[1] * in;
//#define TODATACOORDY( in, out, log) out = (log) ? pow(10.0, (in -sy[0])/sy[1]) : (in -sy[0])/sy[1];
#define TONORMCOORDZ( in, out, log) out = (log) ? sz[0] + sz[1] * log10(in) : sz[0] + sz[1] * in;
//#define TODATACOORDZ( in, out, log) out = (log) ? pow(10.0, (in -sz[0])/sz[1]) : (in -sz[0])/sz[1];
// protect against plplot limited to 16 bit values. Also (#1441) should not change +Inf and -Inf to
// permit correct functioning of draw_polyline
#define  PROTECTXY if (isfinite(x)) { if(x>32760.) x=32760; else if(x< -32760.) x=-32760; } if (isfinite(y)) { if(y>32760.) y=32760; else if( y< -32760.) y=-32760;}

namespace lib
{

  using namespace std;
//  using std::isinf;
#ifdef _MSC_VER
#define finite _finite
#else
  using std::isnan;
#endif
  
//static values
    static int* current_buffer_size=NULL;
    static PLFLT* x_buff;
    static PLFLT* y_buff;

  static DLong savedStyle=0;
  static DDouble savedPointX=std::numeric_limits<double>::quiet_NaN();
  static DDouble savedPointY=std::numeric_limits<double>::quiet_NaN();
  static DFloat sym1x[6]={1, -1, 0, 0, 0, 0}; // +
  static DFloat sym1y[6]={0, 0, 0, -1, 1, 0}; // +
  static DFloat sym2x[12]= {1, -1, 0, 0, 0, 0,1,-1,0,1,-1, 0}; //*
  static DFloat sym2y[12]= {0, 0, 0, -1, 1,0,1,-1,0,-1,1, 0}; // *
  static DFloat sym3x[2]={0,0}; // dot. On PostScript device, x1=x1 and y2=1 creates a round dot.
  static DFloat sym3y[2]={0,0}; // .
  static DFloat sym4x[5]={ 0, 1, 0, -1, 0 }; //diamond.
  static DFloat sym4y[5]={ 1, 0, -1, 0, 1 }; //diamond.
  static DFloat sym5x[4]={ -1, 0, 1, -1 }; // triangle up.
  static DFloat sym5y[4]={ -1, 1, -1, -1 }; // triangle up.
  static DFloat sym6x[5]={ -1, 1, 1, -1, -1 }; //square
  static DFloat sym6y[5]={ 1, 1, -1, -1, 1 }; //square
  static DFloat sym7x[6]= {1,-1,0,1,-1,0}; //x
  static DFloat sym7y[6]= {1,-1,0,-1,1,0}; //x
  DLong syml[7]={6,12,2,5,4,5,6};
  static DFloat sym3xalt[6]={-0.2,-0.2,0.2,0.2,-0.2,0}; // big dot.
  static DFloat sym3yalt[6]={-0.2,0.2,0.2,-0.2,-0.2,0}; // .
  DLong syml_alt=6;

  struct LOCALUSYM {
    DLong nusym;
    DInt fill;
    DFloat usymx[49];
    DFloat usymy[49];
    bool hasColor;
    DLong color;
    bool hasThick;
    DFloat thick;
  };
  static LOCALUSYM localusym;
  
  static std::vector<PLFLT>xtickget;
  static std::vector<PLFLT>ytickget;
  static std::vector<PLFLT>ztickget;

  void gdlDoRangeExtrema(DDoubleGDL *xVal, DDoubleGDL *yVal, DDouble &min, DDouble &max, DDouble xmin, DDouble xmax, bool doMinMax, DDouble minVal, DDouble maxVal)
  {
    DDouble valx, valy;
    SizeT i,k;
    DLong n=xVal->N_Elements();
    if(n!=yVal->N_Elements()) return;
    for (i=0,k=0 ; i<n ; ++i)
    {
       //look only in range x=[xmin,xmax]
       valx=(*xVal)[i];
       if ( (valx<xmin || valx>xmax || isnan(valx))) continue;
       else {
       //min and max of y if not NaN and in range [minVal, maxVal] if doMinMax=yes (min_value, max_value keywords)
       valy=(*yVal)[i];
       if ((doMinMax && (valy<minVal || valy>maxVal )) || isnan(valy)) continue;
       else {if(k==0) {min=valy; max=valy;} else {min=MIN(min,valy); max=MAX(max,valy);}}
       }
       k++;
    }
  }

  void GetMinMaxVal(DDoubleGDL* val, double* minVal, double* maxVal)
  {
#define UNDEF_RANGE_VALUE 1E-12
    //prudency
    *minVal = UNDEF_RANGE_VALUE;
    *maxVal=*minVal+1.0;
    //the real values.
    DLong minE, maxE;
    const bool omitNaN=true;
    val->MinMax(&minE, &maxE, NULL, NULL, omitNaN);
    if ( minVal!=NULL ) {
       *minVal=(*val)[ minE];
       if (isnan(*minVal)) *minVal = UNDEF_RANGE_VALUE;
    }
    if ( maxVal!=NULL ) {
      *maxVal=(*val)[ maxE];
       if (isnan(*maxVal)) *maxVal = 1.0;
    }
    if ((*maxVal)==(*minVal)) *maxVal=*minVal+1.0;
#undef UNDEF_RANGE_VALUE
  }
  
  PLFLT AutoTick(DDouble x)
  {
    if ( x==0.0 ) return 1.0;

    DLong n=static_cast<DLong>(floor(log10(x/3.5)));
    DDouble y=(x/(3.5*pow(10., static_cast<double>(n))));
    DLong m=0;

    if ( y>= 4.51754) m=5;
    else if ( y>=2.0203057 )  m=2;
    else if ( y>=1 )  m=1;

    PLFLT intv=(PLFLT)(m*pow(10., static_cast<double>(n)));
    return intv;
  }
  
  PLFLT AutoLogTick(DDouble min, DDouble max)
  {
    DDouble x=abs(log10(max)-log10(min));
    if (!isfinite(x)) return 0; //trouble ahead...
    if ( x==0.0 ) return 1.0;
    if (x <= 6) return 0;
    if (x <= 7.2) return 1;
    if (x <= 15) return 2;
    if (x <= 35) return 5;
    return 10;
  }
  // the algo below is OK
  PLFLT AutoIntv(DDouble x)
  {
    if ( x==0.0 )
    {
      //      cout << "zero"<<endl;
      return 1.0;
    }

    DLong n=static_cast<DLong>(floor(log10(x/2.82)));
    DDouble y=(x/(2.82*pow(10., static_cast<double>(n))));
    DLong m=0;
    if ( y>=1 && y<2 )  m=1;
    else if ( y>=2 && y<4.47 )  m=2;
    else m=5;


    PLFLT intv=(PLFLT)(m*pow(10., static_cast<double>(n)));
//      cout << "AutoIntv :" << intv << " : "<< x << " " << y << endl;
    return intv;
  }



 #define EXTENDED_DEFAULT_LOGRANGE 12
  //protect from (inverted, strange) axis log values: infinities & nans restrict range to 12 powers of ten
  void gdlHandleUnwantedLogAxisValue(DDouble &min, DDouble &max, bool log)
  {
    if (!log) return;

    bool invert=false;
    DDouble val_min, val_max;

    if(max-min >= 0)
    {
      val_min=min;
      val_max=max;
      invert=false;
    } else {
      val_min=max;
      val_max=min;
      invert=true;
    }

    if ( val_min<=0. ) //problems will happen here
    {
      Warning("Warning: Infinite plot range.");
      if ( val_max<=0. )
      {
        val_min=-EXTENDED_DEFAULT_LOGRANGE;
        val_max=0.0;
      }
      else
      {
        val_min=log10(val_max)-EXTENDED_DEFAULT_LOGRANGE;
        val_max=log10(val_max);
      }
    }
    else
    {
      val_min=log10(val_min);
      val_max=log10(val_max);
    }
    if (invert)
    {
      min=pow(10.,val_max);
      max=pow(10.,val_min);
    } else {
      min=pow(10.,val_min);
      max=pow(10.,val_max);
    }

  }
#undef EXTENDED_DEFAULT_LOGRANGE

  DDouble adjustForTickInterval(const DDouble interval, DDouble &min, DDouble &max) {
    // due to  float to double conversion, computation of integer intervals are prone to failure. We use FLOAT solution
    DFloat fmin=min;
    DFloat fmax=max;
    DFloat finterval=interval;
//    DLong64 n = min / interval;
    DLong64 fn = fmin / finterval;
//    if (n < fn) n=fn; //happens 
    if (fn*finterval > fmin) fn--;
    min=fn*finterval;
//    n = max / interval;
    fn = fmax / finterval;
//    if (n > fn) n=fn; //happens 
    if ( fn*finterval < fmax) fn++;
    max = fn*finterval;
    return max-min; //return range
  }

  // given a juldate, return the juldate of the first 'code' immediately before (or after). Not tested.
  PLFLT gdlReturnTickJulday(DDouble val, int code, bool up) {
    static int monthSize[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    DLong Day, Year, Hour, Minute, Month;
    DLong idow, icap;
    DDouble Seconde;
    j2ymdhms(val, Month, Day, Year, Hour, Minute, Seconde, idow, icap);
    Month++; //j2ymdhms gives back Month number in the [0-11] range for indexing month name tables. pity.
    switch (code) {
    case 1:
      if (up) Year++;
      dateToJD(val, 1, 1, Year, 0, 0, 0.0);
      break;
    case 2:
      if (up) {
        Month++;
        if (Month > 12) {
          Month -= 12;
          Year += 1;
        }
      }
      dateToJD(val, 1, Month, Year, 0, 0, 0.0);
      break;
    case 3:
      if (up) {
        Day++;
        if (Day > monthSize[Month]) {
          Day -= monthSize[Month];
          Month += 1;
        }
        if (Month > 12) {
          Month -= 12;
          Year += 1;
        }
      }
      dateToJD(val, Day, Month, Year, 0, 0, 0.0);
      break;
    case 4:
      if (up) {
        Hour++;
        if (Hour > 23) {
          Hour -= 24;
          Day += 1;
        }
        if (Day > monthSize[Month]) {
          Day -= monthSize[Month];
          Month += 1;
        }
        if (Month > 12) {
          Month -= 12;
          Year += 1;
        }
      }
      dateToJD(val, Day, Month, Year, Hour, 0, 0.0);
      break;
    case 5:
      if (up) {
        Minute++;
        if (Minute > 59) {
          Minute -= 60;
          Hour += 1;
        }
        if (Hour > 23) {
          Hour -= 24;
          Day += 1;
        }
        if (Day > monthSize[Month]) {
          Day -= monthSize[Month];
          Month += 1;
        }
        if (Month > 12) {
          Month -= 12;
          Year += 1;
        }
      }
      dateToJD(val, Day, Month, Year, Hour, Minute, 0.0);
      break;
    case 6:
      if (up) {
        Seconde++;
        if (Seconde > 59) {
          Seconde -= 60;
          Minute += 1;
        }
        if (Minute > 59) {
          Minute -= 60;
          Hour += 1;
        }
        if (Hour > 23) {
          Hour -= 24;
          Day += 1;
        }
        if (Day > monthSize[Month]) {
          Day -= monthSize[Month];
          Month += 1;
        }
        if (Month > 12) {
          Month -= 12;
          Year += 1;
        }
      }
      dateToJD(val, Day, Month, Year, Hour, Minute, Seconde);
      break;
    default:
      assert(false);
      break;
    }
    return val;
  }
  
  void gdlAdjustAxisRange(EnvT* e, int axisId, DDouble &start, DDouble &end, bool &log) {

    bool hastickunits=gdlHasTickUnits(e, axisId);
    if (hastickunits && log) {
      Message("PLOT: LOG setting ignored for Date/Time TICKUNITS.");
      log = false;
    }
    int code = gdlGetCalendarCode(e, axisId);
    
    // [XY]STYLE
    DLong myStyle = 0;

    gdlGetDesiredAxisStyle(e, axisId, myStyle);

    bool exact = ((myStyle & 1) == 1);
    bool extended = ((myStyle & 2) == 2);

    DDouble min = start;
    DDouble max = end;


    if (log) {
      gdlHandleUnwantedLogAxisValue(min, max, log);
      min = log10(min);
      max = log10(max);
    }

    bool invert = false;

    //range useful for estimate
    DDouble range = max - min;

    // correct special case "all values are equal"
    if ((ABS(range) <= std::numeric_limits<DDouble>::min())) {
      if (exact) { //IDL does this
        DLong Ticks;
        gdlGetDesiredAxisTicks(e, axisId, Ticks);
        if (Ticks > 0) e->Throw("Data range for axis has zero length.");
      }

      max=min+1;
      range=1;
    }

    if (range >= 0) {
      invert = false;
    } else {
      range = -range;
      DDouble temp = min;
      min = max;
      max = temp;
      invert = true;
    }

    DDouble TickInterval = 0;
    if (!log) gdlGetDesiredAxisTickInterval(e, axisId, TickInterval); //tickinterval ignored when LOG
    bool doTickInt=(TickInterval > 0);
    if (doTickInt) range=adjustForTickInterval(TickInterval, min, max);

    if (exact) { //exit soon...
      if (extended) { //... after 'extended' range correction
        range = max - min; //does not hurt to recompute
        DDouble val = 0.025 * range;
        min -= val;
        max += val;
        if (doTickInt) range=adjustForTickInterval(TickInterval, min, max);
      }

      //check if tickinterval would make more than 59 ticks (IDL apparent limit). In which case, IDL plots only the first 59 intervals:
      if (doTickInt) if (range / TickInterval > 59) max = min + 59.0 * TickInterval;

      //give back non-log values
      if (log) {
        min = pow(10, min);
        max = pow(10, max);
      }

      if (invert) {
        start = max;
        end = min;
      } else {
        start = min;
        end = max;
      }

      return;
    }

    // general case (only negative OR negative and positive)
    //correct this for calendar values (round to nearest year, month, etc)
    if (code > 0) {
      if (code == 7) {
        if (range >= 366) code = 1;
        else if (range >= 32) code = 2;
        else if (range >= 1.1) code = 3;
        else if (range * 24 >= 1.1) code = 4;
        else if (range * 24 * 60 >= 1.1) code = 5;
        else code = 6;
      }
      min=gdlReturnTickJulday(min, code, false);
      max=gdlReturnTickJulday(max, code, true);
 //code below factorized by gdlReturnTickJulday() above - to be deleted.     
//      static int monthSize[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//      DLong Day1, Day2, Year1, Year2, Hour1, Hour2, Minute1, Minute2, MonthNum1, MonthNum2;
//      DLong idow1, icap1, idow2, icap2;
//      DDouble Seconde1, Seconde2;
//      j2ymdhms(min, MonthNum1, Day1, Year1, Hour1, Minute1, Seconde1, idow1, icap1);
//      MonthNum1++; //j2ymdhms gives back Month number in the [0-11] range for indexing month name tables. pity.
//      j2ymdhms(max, MonthNum2, Day2, Year2, Hour2, Minute2, Seconde2, idow2, icap2);
//      MonthNum2++;
//      switch (code) {
//      case 1:
//        //           day mon year h m s.s
//        dateToJD(min, 1, 1, Year1, 0, 0, 0.0);
//        dateToJD(max, 1, 1, Year2 + 1, 0, 0, 0.0);
//        break;
//      case 2:
//        dateToJD(min, 1, MonthNum1, Year1, 0, 0, 0.0);
//        MonthNum2++;
//        if (MonthNum2 > 12) {
//          MonthNum2 -= 12;
//          Year2 += 1;
//        }
//        dateToJD(max, 1, MonthNum2, Year2, 0, 0, 0.0);
//        break;
//      case 3:
//        dateToJD(min, Day1, MonthNum1, Year1, 0, 0, 0.0);
//        Day2++;
//        if (Day2 > monthSize[MonthNum2]) {
//          Day2 -= monthSize[MonthNum2];
//          MonthNum2 += 1;
//        }
//        if (MonthNum2 > 12) {
//          MonthNum2 -= 12;
//          Year2 += 1;
//        }
//        dateToJD(max, Day2, MonthNum2, Year2, 0, 0, 0.0);
//        break;
//      case 4:
//        dateToJD(min, Day1, MonthNum1, Year1, Hour1, 0, 0.0);
//        Hour2++;
//        if (Hour2 > 23) {
//          Hour2 -= 24;
//          Day2 += 1;
//        }
//        if (Day2 > monthSize[MonthNum2]) {
//          Day2 -= monthSize[MonthNum2];
//          MonthNum2 += 1;
//        }
//        if (MonthNum2 > 12) {
//          MonthNum2 -= 12;
//          Year2 += 1;
//        }
//        dateToJD(max, Day2, MonthNum2, Year2, Hour2, 0, 0.0);
//        break;
//      case 5:
//        dateToJD(min, Day1, MonthNum1, Year1, Hour1, Minute1, 0.0);
//        Minute2++;
//        if (Minute2 > 59) {
//          Minute2 -= 60;
//          Hour2 += 1;
//        }
//        if (Hour2 > 23) {
//          Hour2 -= 24;
//          Day2 += 1;
//        }
//        if (Day2 > monthSize[MonthNum2]) {
//          Day2 -= monthSize[MonthNum2];
//          MonthNum2 += 1;
//        }
//        if (MonthNum2 > 12) {
//          MonthNum2 -= 12;
//          Year2 += 1;
//        }
//        dateToJD(max, Day2, MonthNum2, Year2, Hour2, Minute2, 0.0);
//        break;
//      case 6:
//        dateToJD(min, Day1, MonthNum1, Year1, Hour1, Minute1, Seconde1);
//        Seconde2++;
//        if (Seconde2 > 59) {
//          Seconde2 -= 60;
//          Minute2 += 1;
//        }
//        if (Minute2 > 59) {
//          Minute2 -= 60;
//          Hour2 += 1;
//        }
//        if (Hour2 > 23) {
//          Hour2 -= 24;
//          Day2 += 1;
//        }
//        if (Day2 > monthSize[MonthNum2]) {
//          Day2 -= monthSize[MonthNum2];
//          MonthNum2 += 1;
//        }
//        if (MonthNum2 > 12) {
//          MonthNum2 -= 12;
//          Year2 += 1;
//        }
//        dateToJD(max, Day2, MonthNum2, Year2, Hour2, Minute2, Seconde2);
//        break;
//      default:
//        break;
//      }
    } else if (!doTickInt) {
      if (log) {
        int imin = floor(min);
        int imax = ceil(max);
        min = imin;
        max = imax;
      } else {
        const double leak_factor = 1.25e-6;
        PLFLT intv = AutoIntv(range);
        //diminish max a little to avoid a jump of 'intv' when max value is practically indifferentiable from a 'intv' mark:
        if (max >0) {
          max *= (1 - leak_factor); 
          max =  ceil(max/ intv) * intv;
        } else {
          max *= (1 + leak_factor); 
          max =  ceil(max/ intv) * intv;
      }
        //same for min, in the other direction
        if (min > 0) {
          min *= (1 + leak_factor);
          min = floor(min / intv) * intv;
        } else {
          min *= (1 - leak_factor);
          min = floor(min / intv) * intv;
       }
        //min = floor((min * max_allowed_leak_factor) / intv) * intv;
      }
    }

    if (doTickInt) range=adjustForTickInterval(TickInterval, min, max);

    if (extended) {
      range = max - min;
      DDouble val = 0.025 * range;
      min -= val;
      max += val;
      if (doTickInt) range=adjustForTickInterval(TickInterval, min, max);
    }

    //check if tickinterval would make more than 59 ticks (IDL apparent limit). In which case, IDL plots only the first 59 intervals:
    if (doTickInt) if (range / TickInterval > 59) max = min + 59.0 * TickInterval;

    //give back non-log values
    if (log) {
      min = pow(10, min);
      max = pow(10, max);
    }

    if (invert) {
      start = max;
      end = min;
    } else {
      start = min;
      end = max;
    }
  }

    void plotting_routine_call::restoreDrawArea(GDLGStream *a)
    {
      //retrieve and reset plplot to the last setup for vpor() and wind() made by position-scaling commands like PLOT or CONTOUR
      DDouble *sx, *sy;
      DDouble wx[2], wy[2];
      GetSFromPlotStructs(&sx, &sy, NULL);
      GetWFromPlotStructs(wx, wy, NULL);
      a->vpor(wx[0], wx[1], wy[0], wy[1]);
      PLFLT wx0=(wx[0]-sx[0])/sx[1];
      PLFLT wx1=(wx[1]-sx[0])/sx[1];
      PLFLT wy0=(wy[0]-sy[0])/sy[1];
      PLFLT wy1=(wy[1]-sy[0])/sy[1];
      a->wind(wx0, wx1, wy0, wy1);
    }
    void plotting_routine_call::call(EnvT* e, SizeT n_params_required) {
      // when !d.name == Null  we do nothing !
      DString name = (*static_cast<DStringGDL*> (SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("NAME"), 0)))[0];
      if (name == "NULL") return;

      _nParam = e->NParam(n_params_required);

      abort = handle_args(e);
      if (abort) return;

      GDLGStream* actStream = GraphicsDevice::GetDevice()->GetStream();
      if (actStream == NULL) e->Throw("Unable to create window.");

      //ALL THE DoubleBuffering and Flush() code below introduces terrible slowness in remote X displays, as well as a lot of time lost
      //for displays on the same server. They are completely removed now.      
      //      //double buffering kills the logic and operation of XOR modes. Use HasSafeDoubleBuffering() that tests this feature.)
      //      isDB = actStream->HasSafeDoubleBuffering();
      //      if (isDB) actStream->SetDoubleBuffering();

      if (name == "X" || name == "MAC" || name == "WIN") actStream->updatePageInfo(); //since window size can change

      restoreDrawArea(actStream);

      abort = prepareDrawArea(e, actStream);
      if (abort) { 
        actStream->Update();
        return;
      }

      applyGraphics(e, actStream);

      restoreDrawArea(actStream);

      post_call(e, actStream);
      // IDEM: SLOW
      //      if (isDB) actStream->eop(); else actStream->flush();
      //      if (isDB) actStream->UnSetDoubleBuffering();

      //this is absolutely necessary for widgets as for windows. However the virtual Update function
      //i.e., calling  plstream::cmd(PLESC_EXPOSE, NULL) is very slow.
      // See how to overload it by a faster function such as in GDLXStream::Update() . 
      actStream->Update();
    }
    
  void GetSFromPlotStructs(DDouble **sx, DDouble **sy, DDouble **sz)
  {
    DStructGDL* xStruct=SysVar::X();   //MUST NOT BE STATIC, due to .reset 
    DStructGDL* yStruct=SysVar::Y();   //MUST NOT BE STATIC, due to .reset 
    DStructGDL* zStruct=SysVar::Z();   //MUST NOT BE STATIC, due to .reset 
    unsigned sxTag=xStruct->Desc()->TagIndex("S");
    unsigned syTag=yStruct->Desc()->TagIndex("S");
    unsigned szTag=zStruct->Desc()->TagIndex("S");
    if (sx != NULL) *sx= &(*static_cast<DDoubleGDL*>(xStruct->GetTag(sxTag, 0)))[0];
    if (sy != NULL) *sy= &(*static_cast<DDoubleGDL*>(yStruct->GetTag(syTag, 0)))[0];
    if (sz != NULL) *sz= &(*static_cast<DDoubleGDL*>(zStruct->GetTag(szTag, 0)))[0];
  }
  
  void GetWFromPlotStructs(DDouble *wx, DDouble *wy, DDouble *wz )
  {
    DStructGDL* xStruct=SysVar::X();   //MUST NOT BE STATIC, due to .reset 
    DStructGDL* yStruct=SysVar::Y();   //MUST NOT BE STATIC, due to .reset 
    DStructGDL* zStruct=SysVar::Z();   //MUST NOT BE STATIC, due to .reset 
    unsigned xwindowTag=xStruct->Desc()->TagIndex("WINDOW");
    unsigned ywindowTag=yStruct->Desc()->TagIndex("WINDOW");
    unsigned zwindowTag=zStruct->Desc()->TagIndex("WINDOW");
    if (wx != NULL) {
      wx[0]= (*static_cast<DFloatGDL*>(xStruct->GetTag(xwindowTag, 0)))[0];
      wx[1]= (*static_cast<DFloatGDL*>(xStruct->GetTag(xwindowTag, 0)))[1];
    }
    if (wy != NULL) {
      wy[0]= (*static_cast<DFloatGDL*>(yStruct->GetTag(ywindowTag, 0)))[0];
      wy[1]= (*static_cast<DFloatGDL*>(yStruct->GetTag(ywindowTag, 0)))[1];
    }
    if (wz != NULL) {
      wz[0]= (*static_cast<DFloatGDL*>(zStruct->GetTag(zwindowTag, 0)))[0];
      wz[1]= (*static_cast<DFloatGDL*>(zStruct->GetTag(zwindowTag, 0)))[1];
    }
  }

  void GetUsym(DLong **n, DInt **do_fill, DFloat **x, DFloat **y, bool **do_color, DLong **usymColor , bool **do_thick, DFloat **usymThick)
  {
    *n= &(localusym.nusym);
    *do_fill= &(localusym.fill);
    *do_color= &(localusym.hasColor);
    *do_thick= &(localusym.hasThick);
    *usymColor=&(localusym.color);
    *usymThick=&(localusym.thick);
    *x=localusym.usymx;
    *y=localusym.usymy;
  }

  void SetUsym(DLong n, DInt do_fill, DFloat *x, DFloat *y, bool usersymhascolor, DLong usymColor , bool usersymhasthick, DFloat usymThick )
  {
    localusym.nusym=n;
    localusym.fill=do_fill;
    for ( int i=0; i<n; i++ )
    {
      localusym.usymx[i]=x[i];
      localusym.usymy[i]=y[i];
    }
    localusym.hasColor=usersymhascolor;
    localusym.hasThick=usersymhasthick;
    localusym.color=usymColor;
    localusym.thick=usymThick;
  }
  
  //RETURN RAW VALUES, not deLog-ified
  void gdlGetCurrentAxisRawRangeValues(int axisId, DDouble &Start, DDouble &End) {
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) Struct = SysVar::X();
    if (axisId == YAXIS) Struct = SysVar::Y();
    if (axisId == ZAXIS) Struct = SysVar::Z();
    Start = 0;
    End = 0;
    if (Struct != NULL) {
      static unsigned crangeTag = Struct->Desc()->TagIndex("CRANGE");
      Start = (*static_cast<DDoubleGDL*> (Struct->GetTag(crangeTag, 0)))[0];
      End = (*static_cast<DDoubleGDL*> (Struct->GetTag(crangeTag, 0)))[1];
    }
  }
  
  //This is the good way to get world start end end values.
  void GetCurrentUserLimits( DDouble &xStart, DDouble &xEnd, DDouble &yStart, DDouble &yEnd, DDouble &zStart, DDouble & zEnd)
  {
    gdlGetCurrentAxisRawRangeValues(XAXIS, xStart, xEnd);
    gdlGetCurrentAxisRawRangeValues(YAXIS, yStart, yEnd);
    gdlGetCurrentAxisRawRangeValues(ZAXIS, zStart, zEnd);
  }
  //This is the good way to get world start end end values.
  void GetCurrentUserLimits( DDouble &xStart, DDouble &xEnd, DDouble &yStart, DDouble &yEnd)
  {
    gdlGetCurrentAxisRawRangeValues(XAXIS, xStart, xEnd);
    gdlGetCurrentAxisRawRangeValues(YAXIS, yStart, yEnd);
  }  
  
  void ac_histo(GDLGStream *a, int i_buff, PLFLT *x_buff, PLFLT *y_buff)
  {
    PLFLT x, x1, y, y1, val;
    for ( int jj=1; jj<i_buff; ++jj )
    {
      x1=x_buff[jj-1];
      x=x_buff[jj];
      y1=y_buff[jj-1];
      y=y_buff[jj];
      val=(x1+x)/2.0;
      a->join(x1, y1, val, y1);
      a->join(val, y1, val, y);
      a->join(val, y, x, y);
    }
  }


  void saveLastPoint(DDouble wx, DDouble wy) 
  {
    //we are in Normed coordinates
    savedPointX=wx;
    savedPointY=wy;
  }

  void getLastPoint(GDLGStream *a, DDouble& wx, DDouble& wy)
  {
    wx=savedPointX;
    wy=savedPointY;    //we are in Normed coordinates
  }
  //LINESTYLE
  void gdlLineStyle(GDLGStream *a, DLong style)
  {
      //set saved Satle to nex style:
      savedStyle=style;
      static PLINT mark1[]={200};
      static PLINT space1[]={750};
      static PLINT mark2[]={1500};
      static PLINT space2[]={1500};
      static PLINT mark3[]={1500, 100};
      static PLINT space3[]={1000, 1000};
      static PLINT mark4[]={1500, 100, 100, 100};
      static PLINT space4[]={1000, 1000, 1000, 1000};
      static PLINT mark5[]={3000};
      static PLINT space5[]={1500};          // see plplot-5.5.3/examples/c++/x09.cc
      switch(style)
      {
        case 0:
          a->styl(0, mark1, space1);
          return;
         case 1:
          a->styl(1, mark1, space1);
          return;
        case 2:
          a->styl(1, mark2, space2);
          return;
        case 3:
          a->styl(2, mark3, space3);
          return;
        case 4:
          a->styl(4, mark4, space4);
          return;
        case 5:
          a->styl(1, mark5, space5);
          return;
        default:
          a->styl(0, NULL, NULL);
          return;
      }
  }
  DLong gdlGetCurrentStyle(){
    return savedStyle;
  }

///
/// Draws a line along xVal, yVal
/// @param general environnement pointer 
/// @param graphic stream 
/// @param xVal pointer on DDoubleGDL x values
/// @param yVal pointer on DDoubleGDL y values
/// @param minVal DDouble min value to plot.
/// @param maxVal DDouble max value to plot.
/// @param doMinMax bool do we use minval & maxval above?
/// @param xLog bool scale is log in x
/// @param yLog bool scale is log in y
/// @param psym DLong plotting symbol code
/// @param append bool values must be drawn starting from last plotted value 
/// @param color DLongGDL* pointer to color list (NULL if no use)
///

#define GDL_MAX_PLOT_BUFFER_SIZE 128*128
  
  void draw_polyline(GDLGStream *a, DDoubleGDL *xVal, DDoubleGDL *yVal, DLong psym, bool append, DLongGDL *colorgdl)
  {
//        std::cerr<<"draw_polyline()"<<std::endl;

    bool docolor=(colorgdl != NULL);
 
    // Get decomposed value for colors
    DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
    //if docolor, do we really have more than one color?
    if (docolor) if (colorgdl->N_Elements() == 1) { //do the job once and forget it after.
      a->Color ( ( *colorgdl )[0], decomposed);
      docolor=false;
      
    }    
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"draw_polyline()\n");
    SizeT plotIndex=0;
    bool line=false;
    DLong local_psym=0;

    if ( psym<0 )
    {
      line=true;
      local_psym= -psym;
    }
    else if ( psym==0 )
    {
      line=true;
      local_psym=psym;
    }
    else
    {
      local_psym=psym;
    }

    //usersym and other syms as well!
    DFloat *userSymX, *userSymY;
    DLong *userSymArrayDim;
    //accelerate: define a localUsymArray where the computations that were previously in the loop are already done
    DFloat *localUserSymX=NULL;
    DFloat *localUserSymY=NULL;
    
    bool useLocalPsymAccelerator=false;
    
    //initialize symbol vertex list
    static PLFLT xSym[49];
    static PLFLT ySym[49];
    DInt *do_fill;
    bool *usersymhascolor;
    bool *usersymhasthick;
    DLong *usymColor;
    DFloat *usymThick;
    static DInt nofill=0;
    if ( local_psym==8 )
    {
      GetUsym(&userSymArrayDim, &do_fill, &userSymX, &userSymY, &usersymhascolor, &usymColor , &usersymhasthick, &usymThick );
      if ( *userSymArrayDim==0 )
      {
        ThrowGDLException("No user symbol defined.");
      }
      useLocalPsymAccelerator=true;
    }
    else if ( (local_psym>0&&local_psym<8))
    {
      do_fill=&nofill;
      userSymArrayDim=&(syml[local_psym-1]);
      useLocalPsymAccelerator=true;
      switch(local_psym)
      {
        case 1:
          userSymX=sym1x;
          userSymY=sym1y;
          break;
        case 2:
          userSymX=sym2x;
          userSymY=sym2y;
          break;
        case 3:
    
          //if device does not (bug?) draw single points (vector of 2 same coordinates as plplot does normally) then use special point psym=3.
          if (GraphicsDevice::GetDevice()->DoesNotDrawSinglePoints()) {
            userSymX = sym3xalt;
            userSymY = sym3yalt;
            userSymArrayDim=&syml_alt;
          } else {
            userSymX=sym3x;
            userSymY=sym3y;
          }
          break;
        case 4:
          userSymX=sym4x;
          userSymY=sym4y;
          break;
        case 5:
          userSymX=sym5x;
          userSymY=sym5y;
          break;
        case 6:
          userSymX=sym6x;
          userSymY=sym6y;
          break;
        case 7:
          userSymX=sym7x;
          userSymY=sym7y;
          break;
     }
    }

    if (useLocalPsymAccelerator) { //since userSymArrayDim is not defined
      localUserSymX = (DFloat*) malloc(*userSymArrayDim * sizeof (DFloat));
      localUserSymY = (DFloat*) malloc(*userSymArrayDim * sizeof (DFloat));
      if (local_psym == 3 && GraphicsDevice::GetDevice()->DoesNotDrawSinglePoints()) {
        for (int kk = 0; kk < *userSymArrayDim; kk++) {
          localUserSymX[kk] = userSymX[kk] * a->getPsymConvX()  / a->getSymbolSize();
          localUserSymY[kk] = userSymY[kk] * a->getPsymConvY()  / a->getSymbolSize();
        }
      } else {
        for (int kk = 0; kk < *userSymArrayDim; kk++) {
          localUserSymX[kk] = userSymX[kk] * a->getPsymConvX();
          localUserSymY[kk] = userSymY[kk] * a->getPsymConvY();
        }
      }
    }

    DLong minEl=(xVal->N_Elements()<yVal->N_Elements())?
    xVal->N_Elements():yVal->N_Elements();
    // if scalar x
    if ( xVal->N_Elements()==1&&xVal->Rank()==0 )
      minEl=yVal->N_Elements();
    // if scalar y
    if ( yVal->N_Elements()==1&&yVal->Rank()==0 )
      minEl=xVal->N_Elements();

    // is one of the 2 "arrays" a singleton or not ?

    PLFLT y=0;
    int flag_y_const=0;
    PLFLT y_ref=static_cast<PLFLT>((*yVal)[0]);
    if ( yVal->N_Elements()==1&&yVal->Rank()==0 ) flag_y_const=1;

    PLFLT x=0;
    int flag_x_const=0;
    PLFLT x_ref=static_cast<PLFLT>((*xVal)[0]);
    if ( xVal->N_Elements()==1&&xVal->Rank()==0 ) flag_x_const=1;

    // AC 070601 we use a buffer to use the fast ->line method
    // instead of the slow ->join one.
    // 2 tricks:
    // trick 1/ size of buffer is limited to 1e4 (compromize syze/speed) in order to be able to manage very
    //    large amount of data whitout duplicating all the arrays
    // trick 2/ when we have a NaN or and Inf, we realize the plot, then reset.

    int GDL_POLYLINE_BUFSIZE=64; // WE HAVE TO CHECK WHERE WE LOOSE TIME -- LONG BUFSIZE ONLY SLOW THE PROCESS.
    if (current_buffer_size==NULL) {
      current_buffer_size = (int*) malloc(sizeof(*current_buffer_size));
      *current_buffer_size=GDL_POLYLINE_BUFSIZE;
      x_buff= (PLFLT*) malloc( *current_buffer_size*sizeof(PLFLT));
      y_buff= (PLFLT*) malloc( *current_buffer_size*sizeof(PLFLT));
    }
    bool isBad=false;

    if (append) { //check save value is not bad
      getLastPoint(a, x, y);
      isBad = (!isfinite(x) || !isfinite(y) );
      if (isBad) append=false;
    }
    //compute needed buffersize
    if ((GDL_POLYLINE_BUFSIZE < GDL_MAX_PLOT_BUFFER_SIZE) && (minEl > GDL_POLYLINE_BUFSIZE)) {
      GDL_POLYLINE_BUFSIZE = append ? minEl + 1 : minEl;
      GDL_POLYLINE_BUFSIZE = MIN (GDL_POLYLINE_BUFSIZE, GDL_MAX_PLOT_BUFFER_SIZE);
      // current buffer is too small, get a larger one, but try to use a max buffersize of GDL_MAX_PLOT_BUFFER_SIZE
      if (*current_buffer_size < GDL_POLYLINE_BUFSIZE) {
          *current_buffer_size = GDL_POLYLINE_BUFSIZE;
//          std::cerr<<"allocating more:"<<*current_buffer_size<<std::endl;
          x_buff = (PLFLT*) realloc(x_buff, *current_buffer_size * sizeof (PLFLT));
          y_buff = (PLFLT*) realloc(y_buff, *current_buffer_size * sizeof (PLFLT));
          GDL_POLYLINE_BUFSIZE = *current_buffer_size;
      }
    }
    int i_buff=0;

    if (append) //start with the old point
    {
      getLastPoint(a, x, y);
      PROTECTXY //patch against PLPLOT unfortunate behaviour #1415
      x_buff[i_buff]=x;
      y_buff[i_buff]=y;
      i_buff++;
    }

    // switches for fastness
    bool hasNormalPsym=(local_psym > 0 && local_psym < 8);
    bool hasCustomPsym=(local_psym==8);
    bool hasHisto=(local_psym==10);
    SizeT maxAllowedColorsForSpeed=minEl;
    //wxWidgets driver by plplot somehow makes an N^2 call to change colors --- impossibly slow. Check if the driver is wxWidgets. 
    // TO BE REMOVED WHEN BUG DISAPPEARS
    if (a->IsWxStream()) maxAllowedColorsForSpeed=512;
    if (!docolor) { 
      for (SizeT i = 0; i < minEl; ++i) {
        isBad = (!isfinite(x) || !isfinite(y) ); //nb: isfinite(x)   returns a nonzero value if (fpclassify(x) != FP_NAN && fpclassify(x) != FP_INFINITE)
        if (isBad && i_buff > 0) {
          if (line) a->line(i_buff, x_buff, y_buff);
          if (hasNormalPsym) {
            for (int j = 0; j < i_buff; ++j) {
              for (int kk = 0; kk < *userSymArrayDim; kk++) {
                xSym[kk] = x_buff[j] + localUserSymX[kk];
                ySym[kk] = y_buff[j] + localUserSymY[kk];
              }
              a->line(*userSymArrayDim, xSym, ySym);
            }
          } else if (hasCustomPsym) {
            DLong oldStyl = gdlGetCurrentStyle();
            a->styl(0, NULL, NULL); //symbols drawn in continuous lines
            if (*usersymhascolor) {
              a->Color(*usymColor, decomposed);
            }
            if (*usersymhasthick) {
              a->Thick(*usymThick);
            }
            for (int j = 0; j < i_buff; ++j) {
              for (int kk = 0; kk < *userSymArrayDim; kk++) {
                xSym[kk] = x_buff[j] + localUserSymX[kk];
                ySym[kk] = y_buff[j] + localUserSymY[kk];
              }
              if (*do_fill == 1) {
                a->fill(*userSymArrayDim, xSym, ySym);
              } else {
                a->line(*userSymArrayDim, xSym, ySym);
              }
            }
            gdlLineStyle(a, oldStyl);
          } else if (hasHisto) {
            ac_histo(a, i_buff, x_buff, y_buff);
          }
          i_buff = 0;
        }

        x = static_cast<PLFLT> ((*xVal)[i]);
        y = static_cast<PLFLT> ((*yVal)[i]);
        PROTECTXY //patch against PLPLOT unfortunate behaviour #1415
        x_buff[i_buff] = x;
        y_buff[i_buff] = y;
        i_buff++;


        if ((i_buff == GDL_POLYLINE_BUFSIZE) || (i == minEl - 1)) {
//          cout << "GDL_POLYLINE_BUFSIZE:" << GDL_POLYLINE_BUFSIZE << " i: " << i << " i_buff" << i_buff << " minEl: " << minEl << endl;
          if (line) a->line(i_buff, x_buff, y_buff);
          if (hasNormalPsym) {
            for (int j = 0; j < i_buff; ++j) {
              for (int kk = 0; kk < *userSymArrayDim; kk++) {
                xSym[kk] = x_buff[j] + localUserSymX[kk];
                ySym[kk] = y_buff[j] + localUserSymY[kk];
              }
              a->line(*userSymArrayDim, xSym, ySym);
            }
          } else if (hasCustomPsym) {
            DLong oldStyl = gdlGetCurrentStyle();
            a->styl(0, NULL, NULL); //symbols drawn in continuous lines
            if (*usersymhascolor) {
              a->Color(*usymColor, decomposed);
            }
            if (*usersymhasthick) {
              a->Thick(*usymThick);
            }
            for (int j = 0; j < i_buff; ++j) {
              for (int kk = 0; kk < *userSymArrayDim; kk++) {
                xSym[kk] = x_buff[j] + localUserSymX[kk];
                ySym[kk] = y_buff[j] + localUserSymY[kk];
              }
              if (*do_fill == 1) {
                a->fill(*userSymArrayDim, xSym, ySym);
              } else {
                a->line(*userSymArrayDim, xSym, ySym);
              }
            }
            gdlLineStyle(a, oldStyl);
          } else if (hasHisto) {
            ac_histo(a, i_buff, x_buff, y_buff);
          }

          // we must recopy the last point since the line must continue (tested via small buffer ...)
          x_buff[0] = x_buff[i_buff - 1];
          y_buff[0] = y_buff[i_buff - 1];
          i_buff = 1;
        }
      }
    } else { //docolor mode
      DLong* color=static_cast<DLong*>(colorgdl->DataAddr());
      for (SizeT i = 0; i < minEl; ++i) {
        isBad = (!isfinite(x) || !isfinite(y)); //nb: isfinite(x)   returns a nonzero value if (fpclassify(x) != FP_NAN && fpclassify(x) != FP_INFINITE)
        if (isBad && i_buff > 0) {
          //tricky as we need to plot line and/or eventual symbol the same color
          DLong oldStyl = gdlGetCurrentStyle();
          if (hasCustomPsym) {
            a->styl(0, NULL, NULL); //symbols drawn in continuous lines
            if (*usersymhasthick) {
              a->Thick(*usymThick);
            }
          }
          for (SizeT j = 0; j < i_buff - 1; ++j) {
            DULong col=color[plotIndex++%maxAllowedColorsForSpeed];
            a->Color(col, decomposed); //set color
            if (line) a->line(2, &(x_buff[j]), &(y_buff[j]));
            if (hasNormalPsym) {
              for (int j = 0; j < i_buff; ++j) {
                for (int kk = 0; kk < *userSymArrayDim; kk++) {
                  xSym[kk] = x_buff[j] + localUserSymX[kk];
                  ySym[kk] = y_buff[j] + localUserSymY[kk];
                }
                a->line(*userSymArrayDim, xSym, ySym);
              }
            } else if (hasCustomPsym) {
              for (int kk = 0; kk < *userSymArrayDim; kk++) {
                xSym[kk] = x_buff[j] + localUserSymX[kk];
                ySym[kk] = y_buff[j] + localUserSymY[kk];
              }
              if (*do_fill == 1) {
                a->fill(*userSymArrayDim, xSym, ySym);
              } else {
                a->line(*userSymArrayDim, xSym, ySym);
              }
              gdlLineStyle(a, oldStyl);
            }
          }
          i_buff = 0;
        }

        x = static_cast<PLFLT> ((*xVal)[i]);
        y = static_cast<PLFLT> ((*yVal)[i]);
        PROTECTXY //patch against PLPLOT unfortunate behaviour #1415
        x_buff[i_buff] = x;
        y_buff[i_buff] = y;
        i_buff++;


        if ((i_buff == GDL_POLYLINE_BUFSIZE) || (i == minEl - 1)) {
          //tricky as we need to plot line and/or eventual symbol the same color
          DLong oldStyl = gdlGetCurrentStyle();
          if (hasCustomPsym) {
            a->styl(0, NULL, NULL); //symbols drawn in continuous lines
            if (*usersymhasthick) {
              a->Thick(*usymThick);
            }
          }
          for (SizeT j = 0; j < i_buff - 1; ++j) {
            DULong col=color[plotIndex++%maxAllowedColorsForSpeed];
            a->Color(col, decomposed); //set color
            if (line) a->line(2, &(x_buff[j]), &(y_buff[j]));
            if (hasNormalPsym) {
              for (int j = 0; j < i_buff; ++j) {
                for (int kk = 0; kk < *userSymArrayDim; kk++) {
                  xSym[kk] = x_buff[j] + localUserSymX[kk];
                  ySym[kk] = y_buff[j] + localUserSymY[kk];
                }
                a->line(*userSymArrayDim, xSym, ySym);
              }
            } else if (hasCustomPsym) {
              for (int kk = 0; kk < *userSymArrayDim; kk++) {
                xSym[kk] = x_buff[j] + localUserSymX[kk];
                ySym[kk] = y_buff[j] + localUserSymY[kk];
              }
              if (*do_fill == 1) {
                a->fill(*userSymArrayDim, xSym, ySym);
              } else {
                a->line(*userSymArrayDim, xSym, ySym);
              }
              gdlLineStyle(a, oldStyl);
            }
          }

          // we must recopy the last point since the line must continue (tested via small buffer ...)
          x_buff[0] = x_buff[i_buff - 1];
          y_buff[0] = y_buff[i_buff - 1];
          i_buff = 1;
        }
      }
    }
    

    if (useLocalPsymAccelerator) {
      free(localUserSymX);
      free(localUserSymY);
    }
//    delete[] x_buff;
//    delete[] y_buff;
    //save last point
    saveLastPoint(x, y);
  }

 
  //BACKGROUND COLOR


  
  //Very special usage only in plotting surface
  void gdlSetGraphicsPenColorToBackground(GDLGStream *a)
  {
    a->plstream::col0( 0);
  }

  //COLOR

  // helper for NOERASE

  //PSYM

  //SYMSIZE

  //CHARSIZE

  //THICK
  
  //crange to struct

  void gdlStoreAxisCRANGE(int axisId, DDouble Start, DDouble End, bool log)
  {
    DStructGDL* Struct=NULL;
    if ( axisId==XAXIS ) Struct=SysVar::X();
    if ( axisId==YAXIS ) Struct=SysVar::Y();
    if ( axisId==ZAXIS ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      int debug=0;
      if ( debug ) cout<<"Set     :"<<Start<<" "<<End<<endl;

      unsigned crangeTag=Struct->Desc()->TagIndex("CRANGE");
      if ( log )
      {
        (*static_cast<DDoubleGDL*>(Struct->GetTag(crangeTag, 0)))[0]=log10(Start);
        (*static_cast<DDoubleGDL*>(Struct->GetTag(crangeTag, 0)))[1]=log10(End);
        if ( debug ) cout<<"set log"<<Start<<" "<<End<<endl;
      }
      else
      {
        (*static_cast<DDoubleGDL*>(Struct->GetTag(crangeTag, 0)))[0]=Start;
        (*static_cast<DDoubleGDL*>(Struct->GetTag(crangeTag, 0)))[1]=End;
      }
    }
  }

//CRANGE from struct

  void gdlGetCurrentAxisWindow(int axisId, DDouble &wStart, DDouble &wEnd)
  {
    DStructGDL* Struct=NULL;
    if ( axisId==XAXIS ) Struct=SysVar::X();
    if ( axisId==YAXIS ) Struct=SysVar::Y();
    if ( axisId==ZAXIS ) Struct=SysVar::Z();
    wStart=0;
    wEnd=0;
    if ( Struct!=NULL )
    {
      static unsigned windowTag=Struct->Desc()->TagIndex("WINDOW");
      wStart=(*static_cast<DFloatGDL*>(Struct->GetTag(windowTag, 0)))[0];
      wEnd=(*static_cast<DFloatGDL*>(Struct->GetTag(windowTag, 0)))[1];
    }
  }
  //converts x and y but leaves code and log unchanged.

  void ConvertToNormXY(SizeT n, DDouble *x, bool const xLog, DDouble *y, bool const yLog, COORDSYS const code) {
    //  std::cerr<<"ConvertToNormXY(DDouble)"<<std::endl;
    if (code == DATA) {
      DDouble *sx, *sy;
      GetSFromPlotStructs(&sx, &sy);
      for (auto i = 0; i < n; ++i) TONORMCOORDX(x[i], x[i], xLog);
      for (auto i = 0; i < n; ++i) TONORMCOORDY(y[i], y[i], yLog);
    } else if (code == DEVICE) {
      int xSize, ySize;
      //give default values
      DStructGDL* dStruct = SysVar::D();
      unsigned xsizeTag = dStruct->Desc()->TagIndex("X_SIZE");
      unsigned ysizeTag = dStruct->Desc()->TagIndex("Y_SIZE");
      xSize = (*static_cast<DLongGDL*> (dStruct->GetTag(xsizeTag, 0)))[0];
      ySize = (*static_cast<DLongGDL*> (dStruct->GetTag(ysizeTag, 0)))[0];
      for (auto i = 0; i < n; ++i) x[i] /= xSize;
      for (auto i = 0; i < n; ++i) y[i] /= ySize;
    }
  }
  void ConvertToNormZ(SizeT n, DDouble *z, bool const zLog, COORDSYS const code) {
    //  std::cerr<<"ConvertToNormZ(DDouble)"<<std::endl;
    if (code == DATA) {
      DDouble *sz;
      GetSFromPlotStructs(NULL, NULL, &sz);
      for (auto i = 0; i < n; ++i) TONORMCOORDZ(z[i], z[i], zLog);
    } else if (code == DEVICE) {
      for (auto i = 0; i < n; ++i) z[i] = 0;
    }
  }
  void gdlStoreSC() {
    //save corresponding SCxx values useful for oldies compatibility (to be checked as some changes have been done):
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset
    static unsigned positionTag = pStruct->Desc()->TagIndex("POSITION");
    DFloat* position = &(*static_cast<DFloatGDL*> (pStruct->GetTag(positionTag, 0)))[0];
    DStructGDL* dStruct = SysVar::D(); //MUST NOT BE STATIC, due to .reset
    static unsigned dxvsizeTag = dStruct->Desc()->TagIndex("X_VSIZE");
    static unsigned dyvsizeTag = dStruct->Desc()->TagIndex("Y_VSIZE");

    DLong x_vsize = (*static_cast<DLongGDL*> (dStruct->GetTag(dxvsizeTag, 0)))[0];
    DLong y_vsize = (*static_cast<DLongGDL*> (dStruct->GetTag(dyvsizeTag, 0)))[0];
    DFloat* sc = SysVar::GetSC();
    DStructGDL* xStruct = SysVar::X(); 
    static unsigned xwindowTag = xStruct->Desc()->TagIndex("WINDOW");
    DStructGDL* yStruct = SysVar::Y(); 
    static unsigned ywindowTag = yStruct->Desc()->TagIndex("WINDOW");
    DFloat* xwindow=&(*static_cast<DFloatGDL*> (xStruct->GetTag(xwindowTag, 0)))[0];
    DFloat* ywindow=&(*static_cast<DFloatGDL*> (yStruct->GetTag(ywindowTag, 0)))[0];
    sc[0] = (position[2] != 0) ? position[0] * x_vsize : xwindow[0]*x_vsize;
    sc[1] = (position[2] != 0) ? position[2] * x_vsize : xwindow[1]*x_vsize;
    sc[2] = (position[2] != 0) ? position[1] * y_vsize : ywindow[0]*y_vsize;
    sc[3] = (position[2] != 0) ? position[3] * y_vsize : ywindow[1]*y_vsize;

  }
  //Get [XYZ].WINDOW
  DFloat* gdlGetRegion() {
    static const SizeT REGIONTAG=12;
    static DFloat position[6];
    position[0]=(*static_cast<DFloatGDL*>(SysVar::X()->GetTag(REGIONTAG, 0)))[0];
    position[1]=(*static_cast<DFloatGDL*>(SysVar::X()->GetTag(REGIONTAG, 0)))[1];
    position[2]=(*static_cast<DFloatGDL*>(SysVar::Y()->GetTag(REGIONTAG, 0)))[0];
    position[3]=(*static_cast<DFloatGDL*>(SysVar::Y()->GetTag(REGIONTAG, 0)))[1];
    position[4]=(*static_cast<DFloatGDL*>(SysVar::Z()->GetTag(REGIONTAG, 0)))[0];
    position[5]=(*static_cast<DFloatGDL*>(SysVar::Z()->GetTag(REGIONTAG, 0)))[1];
    return position;
}
  //Stores [XYZ].WINDOW, .REGION and .S
  void gdlStoreXAxisParameters(GDLGStream* actStream, DDouble Start, DDouble End, bool log)
  {
    // !X etc parameters relative to the VIEWPORT:
    //easier to retrieve here the values sent to vpor() in the calling function instead of
    //calling a special function like gdlStoreRegion(), gdlStoreWindow() each time a memory of vpor() is needed.
    //Will thus need to be amended when/if we get rid of plplot.
    PLFLT p_xmin, p_xmax, p_ymin, p_ymax, norm_min, norm_max;
    actStream->getCurrentNormBox(p_xmin, p_xmax, p_ymin, p_ymax); //viewport normalized coords
    DStructGDL* Struct=SysVar::X(); 
    norm_min=p_xmin; 
    norm_max=p_xmax; 
    //WINDOW
    static unsigned windowTag=Struct->Desc()->TagIndex("WINDOW");
    (*static_cast<DFloatGDL*>(Struct->GetTag(windowTag, 0)))[0]=norm_min;
    (*static_cast<DFloatGDL*>(Struct->GetTag(windowTag, 0)))[1]=norm_max;
    // Loginess
    static unsigned typeTag = Struct->Desc()->TagIndex("TYPE");
    (*static_cast<DLongGDL*> (Struct->GetTag(typeTag, 0)))[0] = log;
    //CRANGE
    unsigned crangeTag = Struct->Desc()->TagIndex("CRANGE");
    //here, as this is called from a box-setting (or axis-setting) function, Start and End are already in LOG if case be.
    (*static_cast<DDoubleGDL*> (Struct->GetTag(crangeTag, 0)))[0] = Start;
    (*static_cast<DDoubleGDL*> (Struct->GetTag(crangeTag, 0)))[1] = End;
    //X.S
    DDouble range=End-Start;
    if (range!=0) {
    static unsigned sTag=Struct->Desc()->TagIndex("S");
    (*static_cast<DDoubleGDL*>(Struct->GetTag(sTag, 0)))[0]=(norm_min*End-norm_max*Start)/range;
    (*static_cast<DDoubleGDL*>(Struct->GetTag(sTag, 0)))[1]=(norm_max-norm_min)/range;
    }
    //OLDIES COMPATIBILITY (?)
    gdlStoreSC();
  }
    void gdlStoreYAxisParameters(GDLGStream* actStream, DDouble Start, DDouble End, bool log)
{
    // !Y etc parameters relative to the VIEWPORT:
    //easier to retrieve here the values sent to vpor() in the calling function instead of
    //calling a special function like gdlStoreRegion(), gdlStoreWindow() each time a memory of vpor() is needed.
    //Will thus need to be amended when/if we get rid of plplot.
    PLFLT p_xmin, p_xmax, p_ymin, p_ymax, norm_min, norm_max;
    actStream->getCurrentNormBox(p_xmin, p_xmax, p_ymin, p_ymax); //viewport normalized coords
    DStructGDL* Struct = SysVar::Y();
    norm_min = p_ymin;
    norm_max = p_ymax;
    //WINDOW
    static unsigned windowTag = Struct->Desc()->TagIndex("WINDOW");
    (*static_cast<DFloatGDL*> (Struct->GetTag(windowTag, 0)))[0] = norm_min;
    (*static_cast<DFloatGDL*> (Struct->GetTag(windowTag, 0)))[1] = norm_max;
    // Loginess
    static unsigned typeTag = Struct->Desc()->TagIndex("TYPE");
    (*static_cast<DLongGDL*> (Struct->GetTag(typeTag, 0)))[0] = log;
    //CRANGE
    unsigned crangeTag = Struct->Desc()->TagIndex("CRANGE");
    //here, as this is called from a box-setting (or axis-setting) function, Start and End are already in LOG if case be.
    (*static_cast<DDoubleGDL*> (Struct->GetTag(crangeTag, 0)))[0] = Start;
    (*static_cast<DDoubleGDL*> (Struct->GetTag(crangeTag, 0)))[1] = End;
    //Y.S
    DDouble range = End - Start;
    if (range == 0) range = 1;
    static unsigned sTag = Struct->Desc()->TagIndex("S");
    (*static_cast<DDoubleGDL*> (Struct->GetTag(sTag, 0)))[0] = (norm_min * End - norm_max * Start) / range;
    (*static_cast<DDoubleGDL*> (Struct->GetTag(sTag, 0)))[1] = (norm_max - norm_min) / range;
    //OLDIES COMPATIBILITY (?)
    gdlStoreSC();
  }
  
  //Z is a bit different. this is for Z= or Z param.  
  void gdlStoreZAxisParameters(GDLGStream* actStream, DDouble Start, DDouble End, bool log, DDouble zNormMin, DDouble zNormMax)
{
    DStructGDL* Struct=SysVar::Z(); 
    // Loginess
    static unsigned typeTag = Struct->Desc()->TagIndex("TYPE");
    (*static_cast<DLongGDL*> (Struct->GetTag(typeTag, 0)))[0] = log;
    //CRANGE
    unsigned crangeTag = Struct->Desc()->TagIndex("CRANGE");
    //here, as this is called from a box-setting (or axis-setting) function, Start and End are already in LOG if case be.
    (*static_cast<DDoubleGDL*> (Struct->GetTag(crangeTag, 0)))[0] = Start;
    (*static_cast<DDoubleGDL*> (Struct->GetTag(crangeTag, 0)))[1] = End;
    //WINDOW
    static unsigned windowTag = Struct->Desc()->TagIndex("WINDOW");
    (*static_cast<DFloatGDL*> (Struct->GetTag(windowTag, 0)))[0] = zNormMin;
    (*static_cast<DFloatGDL*> (Struct->GetTag(windowTag, 0)))[1] = zNormMax;
    //Z.S
    DDouble range = End - Start;
    if (range != 0) {
      static unsigned sTag = Struct->Desc()->TagIndex("S");
      (*static_cast<DDoubleGDL*> (Struct->GetTag(sTag, 0)))[0] = (zNormMin * End - zNormMax * Start) / range;
      (*static_cast<DDoubleGDL*> (Struct->GetTag(sTag, 0)))[1] = (zNormMax - zNormMin) / range;
    }
}

//  void gdlGetCLIPXY(DLong &x0, DLong &y0, DLong &x1, DLong &y1){
//    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
//    static unsigned clipTag=pStruct->Desc()->TagIndex("CLIP");
//    x0=(*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[0];
//    y0=(*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[1];
//    x1=(*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[2];
//    y1=(*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[3];
//  }
  void gdlStoreCLIP()
  {
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    DLong xsize=(*static_cast<DLongGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("X_SIZE"), 0)))[0];
    DLong ysize=(*static_cast<DLongGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("Y_SIZE"), 0)))[0];
    //WINDOW
    DFloat x0=(*static_cast<DFloatGDL*>(SysVar::X()->GetTag(SysVar::X()->Desc()->TagIndex("WINDOW"), 0)))[0];
    DFloat x1=(*static_cast<DFloatGDL*>(SysVar::X()->GetTag(SysVar::X()->Desc()->TagIndex("WINDOW"), 0)))[1];
    DFloat y0=(*static_cast<DFloatGDL*>(SysVar::Y()->GetTag(SysVar::Y()->Desc()->TagIndex("WINDOW"), 0)))[0];
    DFloat y1=(*static_cast<DFloatGDL*>(SysVar::Y()->GetTag(SysVar::Y()->Desc()->TagIndex("WINDOW"), 0)))[1];
    static unsigned clipTag=pStruct->Desc()->TagIndex("CLIP");
    (*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[0]=x0*xsize;
    (*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[1]=y0*ysize;
    (*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[2]=x1*xsize;
    (*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[3]=y1*ysize;
    (*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[4]=0;
    (*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[5]=1000; //for the time being
  }

  void gdlGetAxisType(int axisId, bool &log)
  {
    DStructGDL* Struct;
    if ( axisId==XAXIS ) Struct=SysVar::X();
    if ( axisId==YAXIS ) Struct=SysVar::Y();
    if ( axisId==ZAXIS ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      static unsigned typeTag=Struct->Desc()->TagIndex("TYPE");
      if ( (*static_cast<DLongGDL*>(Struct->GetTag(typeTag, 0)))[0]==1 )
        log=true;
      else
        log=false;
    }
  }

  void get_mapset(bool &mapset)
  {
    DStructGDL* Struct=SysVar::X();   //MUST NOT BE STATIC, due to .reset 
    if ( Struct!=NULL )
    {
      static unsigned typeTag=Struct->Desc()->TagIndex("TYPE");

      if ( (*static_cast<DLongGDL*>(Struct->GetTag(typeTag, 0)))[0]==3 )
        mapset=true;
      else
        mapset=false;
    }
  }

  void set_mapset(bool mapset)
  {
    DStructGDL* Struct=SysVar::X();   //MUST NOT BE STATIC, due to .reset 
    if ( Struct!=NULL )
    {
      static unsigned typeTag=Struct->Desc()->TagIndex("TYPE");
      (*static_cast<DLongGDL*>(Struct->GetTag(typeTag, 0)))[0]=(mapset)?3:0;
    }
  }


  DDoubleGDL* getLabelingValues(int axisId) {
    DDoubleGDL* res = NULL;
    int nEl;
    switch (axisId) {
    case XAXIS:
      nEl = xtickget.size();
      if (nEl > 0) {
        res = new DDoubleGDL(nEl, BaseGDL::NOZERO);
        for (auto i = 0; i < nEl; ++i) (*res)[i] = xtickget[i];
      }
      xtickget.clear();
      break;
    case YAXIS:
      nEl = ytickget.size();
      if (nEl > 0) {
        res = new DDoubleGDL(nEl, BaseGDL::NOZERO);
        for (auto i = 0; i < nEl; ++i) (*res)[i] = ytickget[i];
      }
      ytickget.clear();
      break;
    case ZAXIS:
      nEl = ztickget.size();
      if (nEl > 0) {
        res = new DDoubleGDL(nEl, BaseGDL::NOZERO);
        for (auto i = 0; i < nEl; ++i) (*res)[i] = ztickget[i];
      }
      ztickget.clear();
      break;
    }
    return res;
  }
  void resetTickGet(int axisId){
    switch(axisId){
    case XAXIS:
      xtickget.clear();
      break;
    case YAXIS:
      ytickget.clear();
      break;
    case ZAXIS:
      ztickget.clear();
      break;
    }
  }
  void addToTickGet(int axisId, PLFLT value){
    switch(axisId){
    case PL_X_AXIS:
      xtickget.push_back(value);
      break;
    case PL_Y_AXIS:
      ytickget.push_back(value);
      break;
    case PL_Z_AXIS:
      ztickget.push_back(value);
      break;
    }
  }
  void defineLabeling(GDLGStream *a, int axisId, void(*func)(PLINT axis, PLFLT value, char *label, PLINT length, PLPointer data), PLPointer data) {
    resetTickGet(axisId);
    a->slabelfunc(func, data);
  }
  void resetLabeling(GDLGStream *a, int axisId) {
    a->slabelfunc(NULL, NULL);
  }
  void doOurOwnFormat(PLINT axisNotUsed, PLFLT value, char *label, PLINT length, PLPointer data)
  {
    struct GDL_TICKDATA *ptr = (GDL_TICKDATA* )data;
    //was:
//    static string normalfmt[7]={"%1.0fx10#u%d#d","%2.1fx10#u%d#d","%3.2fx10#u%d#d","%4.2fx10#u%d#d","%5.4fx10#u%d#d","%6.5fx10#u%d#d","%7.6fx10#u%d#d"};
//    static string specialfmt="10#u%d#d";
//    static string specialfmtlog="10#u%s#d";
    
    //we need !3x!X to insure the x sign is always written in single roman.
    static string normalfmt[7]={"%1.0f!3x!X10!E%d!N","%2.1f!3x!X10!E%d!N","%3.2f!3x!X10!E%d!N","%4.2f!3x!X10!E%d!N","%5.4f!3x!X10!E%d!N","%6.5f!3x!X10!E%d!N","%7.6f!3x!X10!E%d!N"};
    static string specialfmt="10!E%d!N";
    static string specialfmtlog="10!E%s!N";
    PLFLT z;
    int ns;
    char *i;
    int sgn=(value<0)?-1:1;
    //special cases, since plplot gives approximate zero values, not strict zeros.
    if (!(ptr->isLog) && (sgn*value<ptr->axisrange*1e-6)) 
    {
      snprintf(label, length, "0");
      return;
    }
    //in log, plplot gives correctly rounded "integer" values but 10^0 needs a bit of help.
    if ((ptr->isLog) && (sgn*value<1e-6)) //i.e. 0 
    {
      snprintf(label, length, "10!E0!N"); 
      return;
    }
    
    int e=floor(log10(value*sgn));
    char *test=(char*)calloc(2*length, sizeof(char)); //be safe
    if (!isfinite(log10(value*sgn))||(e<4 && e>-4)) 
    {
      snprintf(test, length, "%f",value);
      ns=strlen(test);
      i=strrchr (test,'0');
      while (i==(test+ns-1)) //remove trailing zeros...
      {
          *i='\0';
        i=strrchr(test,'0');
        ns--;
      }
      i=strrchr(test,'.'); //remove trailing '.'
      if (i==(test+ns-1)) {*i='\0'; ns--;}
      if (ptr->isLog) {
        snprintf( label, length, specialfmtlog.c_str(),test);
      }
      else
      {
        strcpy(label, test);
      }
    }
    else
    {
      z=value*sgn/pow(10.,e);
      snprintf(test,20,"%7.6f",z);
      ns=strlen(test);
      i=strrchr(test,'0');
      while (i==(test+ns-1))
      {
          *i='\0';
        i=strrchr(test,'0');
        ns--;
      }
      ns-=2;ns=(ns>6)?6:ns;
//      if (floor(sgn*z)==1 && ns==0) { // removed see #1582 and, besides, intent was obscure ? 
//        snprintf( label, length, specialfmt.c_str(),e);
//      } else {
        snprintf( label, length, normalfmt[ns].c_str(),sgn*z,e);
//      }
    }
    free(test);
  }

  void doPossibleCalendarFormat(PLINT axisNotUsed, PLFLT value, char *label, PLINT length, DString &what, PLPointer data) {
    struct GDL_TICKDATA *ptr = (GDL_TICKDATA* )data;
    static string theMonth[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    PLINT Month, Day, Year, Hour, Minute, dow, cap;
    PLFLT Second;
    j2ymdhms(value, Month, Day, Year, Hour, Minute, Second, dow, cap);
    int convcode = 0;
    if (what.length() < 1) convcode = 7;
    else if (what.substr(0, 7) == "NUMERIC") convcode = 7;
    else if (what.substr(0, 4) == "YEAR") convcode = 1;
    else if (what.substr(0, 5) == "MONTH") convcode = 2;
    else if (what.substr(0, 3) == "DAY") convcode = 3;
    else if (what.substr(0, 4) == "HOUR") convcode = 4;
    else if (what.substr(0, 6) == "MINUTE") convcode = 5;
    else if (what.substr(0, 6) == "SECOND") convcode = 6;
    else if (what.substr(0, 4) == "TIME") {
      if (ptr->axisrange >= 366) convcode = 1;
      else if (ptr->axisrange >= 32) convcode = 2;
      else if (ptr->axisrange >= 1.1) convcode = 3;
      else if (ptr->axisrange * 24 >= 1.1) convcode = 4;
      else if (ptr->axisrange * 24 * 60 >= 1.1) convcode = 5;
      else convcode = 6;
    } else convcode = 7;
    switch (convcode) {
    case 7:
      doOurOwnFormat(axisNotUsed, value, label, length, data);
      break;
    case 1:
      snprintf(label, length, "%d", Year);
      break;
    case 2:
      snprintf(label, length, "%s", theMonth[Month].c_str());
      break;
    case 3:
      snprintf(label, length, "%d", Day);
      break;
    case 4:
      snprintf(label, length, "%02d", Hour);
      break;
    case 5:
      snprintf(label, length, "%02d", Minute);
      break;
    case 6:
      snprintf(label, length, "%05.2f", Second);
      break;

    }
  }
  
  void doFormatAxisValue(DDouble value, string &label)
  {
    static string normalfmt[7]={"%1.0fx10^%d","%2.1fx10^%d","%3.2fx10^%d","%4.2fx10^%d","%5.4fx10^%d","%6.5fx10^%d","%7.6fx10^%d"};
    static string specialfmt="10^%d";
    static const int length=20;
    PLFLT z;
    int ns;
    char *i;
    int sgn=(value<0)?-1:1;
    //special cases, since plplot gives approximate zero values, not strict zeros.
    if (sgn*value< std::numeric_limits<DDouble>::min()) 
    {
      label="0"; 
      return;
    }
    
    int e=floor(log10(value*sgn));
    char *test=(char*)calloc(2*length, sizeof(char)); //be safe
    if (!isfinite(e)||(e<4 && e>-4)) 
    {
      snprintf(test, length, "%f",value);
      ns=strlen(test);
      i=strrchr (test,'0');
      while (i==(test+ns-1)) //remove trailing zeros...
      {
          *i='\0';
        i=strrchr(test,'0');
        ns--;
      }
      i=strrchr(test,'.'); //remove trailing '.'
      if (i==(test+ns-1)) {*i='\0'; ns--;}
    }
    else
    {
      z=value*sgn/pow(10.,e);
      snprintf(test,20,"%7.6f",z);
      ns=strlen(test);
      i=strrchr(test,'0');
      while (i==(test+ns-1))
      {
          *i='\0';
        i=strrchr(test,'0');
        ns--;
      }
      ns-=2;ns=(ns>6)?6:ns;
	if (floor(sgn*z)==1 && ns==0)
	  snprintf( test, length, specialfmt.c_str(),e);
	else
	  snprintf( test, length, normalfmt[ns].c_str(),sgn*z,e);
    }
    label=test;
    free(test);
  }
  
  BaseGDL* format_axis_values(EnvT *e){
    DDoubleGDL* p0D = e->GetParAs<DDoubleGDL>( 0);
    DStringGDL* res = new DStringGDL( p0D->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0D->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
	  {
	    doFormatAxisValue((*p0D)[i], (*res)[i]);
	  }
  return res;
  }

  void gdlAxisTickFunc(PLINT axis, PLFLT value, char *label, PLINT length, PLPointer data) {
    SizeT internalIndex = 0;
    addToTickGet(axis, value);
    struct GDL_TICKDATA *ptr = (GDL_TICKDATA*) data;
    if (ptr->nTickName > 0) {
      if (ptr->tickNameCounter > ptr->nTickName - 1) {
        doOurOwnFormat(axis, value, label, length, data);
      } else {
        snprintf(label, length, "%s", ((*ptr->TickName)[ptr->tickNameCounter]).c_str());
      }
      ptr->tickNameCounter++;
    } else if (ptr->nTickFormat > 0) {//will be formatted -- not interpreting the tickunits
      DDouble v = value;
      if (ptr->isLog) v = pow(10., v);
      if (((*ptr->TickFormat)[0]).substr(0, 1) == "(") { //internal format, call internal func "STRING"
        EnvT *e = ptr->e;
        static int stringIx = LibFunIx("STRING");
        assert(stringIx >= 0);
        EnvT* newEnv = new EnvT(e, libFunList[stringIx], NULL);
        Guard<EnvT> guard(newEnv);
        // add parameters
        newEnv->SetNextPar(new DDoubleGDL(v));
        newEnv->SetNextPar(new DStringGDL(((*ptr->TickFormat)[0]).c_str()));
        // make the call
        BaseGDL* res = static_cast<DLibFun*> (newEnv->GetPro())->Fun()(newEnv);
        strncpy(label, (*static_cast<DStringGDL*> (res))[0].c_str(), length);
      } else // external function: if tickunits not specified, pass Axis (int), Index(int),Value(Double)
        //    else pass also Level(int)
        // Thanks to Marc for code snippet!
        // NOTE: this encompasses the 'LABEL_DATE' format, an existing procedure in the IDL library.
      {
        EnvT *e = ptr->e;
        DString callF = (*ptr->TickFormat)[0];
        // this is a function name -> convert to UPPERCASE
        callF = StrUpCase(callF);
        //  Search in user proc and function
        SizeT funIx = GDLInterpreter::GetFunIx(callF);

        EnvUDT* newEnv = new EnvUDT(e->CallingNode(), funList[ funIx], (DObjGDL**) NULL);
        Guard< EnvUDT> guard(newEnv);
        // add parameters
        newEnv->SetNextPar(new DLongGDL(axis - 1)); //axis in PLPLOT starts at 1, it starts at 0 in IDL
        newEnv->SetNextPar(new DLongGDL(internalIndex)); //index
        newEnv->SetNextPar(new DDoubleGDL(v)); //value
        // guard *before* pushing new env
        StackGuard<EnvStackT> guard1(e->Interpreter()->CallStack());
        e->Interpreter()->CallStack().push_back(newEnv);
        guard.release();

        BaseGDL* retValGDL = e->Interpreter()->call_fun(static_cast<DSubUD*> (newEnv->GetPro())->GetTree());
        // we are the owner of the returned value
        Guard<BaseGDL> retGuard(retValGDL);
        strncpy(label, (*static_cast<DStringGDL*> (retValGDL))[0].c_str(), length);
        internalIndex++;
      }
    } else {
      doOurOwnFormat(axis, value, label, length, data);
    }
    //translate format codes (as in mtex).
    double nchars;
    std::string out = ptr->a->TranslateFormatCodes(label, &nchars);
    ptr->nchars = max(ptr->nchars, nchars);
    strcpy(label, out.c_str());
  }

  void gdlMultiAxisTickFunc(PLINT axis, PLFLT value, char *label, PLINT length, PLPointer multiaxisdata) {
    addToTickGet(axis, value);
    PLINT axisNotUsed = 0; //to indicate that effectively the axis number is not (yet?) used in some calls
    static SizeT internalIndex = 0;
    static DLong lastMultiAxisLevel = 0;
    struct GDL_TICKDATA *ptr = (GDL_TICKDATA*) multiaxisdata;
    if (ptr->reset) {
      internalIndex = 0; //reset index each time a new axis command is issued.
      ptr->reset = false;
    }
    if (ptr->counter != lastMultiAxisLevel) {
      lastMultiAxisLevel = ptr->counter;
      internalIndex = 0; //reset index each time sub-axis changes
    }
    if (ptr->what == GDL_TICKUNITS) {
    do_tickunits:
      if (ptr->counter > ptr->nTickUnits - 1) {
        doOurOwnFormat(axisNotUsed, value, label, length, multiaxisdata);
      } else {
        DString what = StrUpCase((*ptr->TickUnits)[ptr->counter]);
        doPossibleCalendarFormat( axisNotUsed, value, label, length, what, multiaxisdata);
      }
    } else if (ptr->what == GDL_TICKUNITS_AND_FORMAT) {
      if (ptr->counter > ptr->nTickFormat - 1) {
        goto do_tickunits; //format does not apply but tickunits still apply
      } else { //will be formatted -- not interpreting the tickunits
        DDouble v = value;
        if (ptr->isLog) v = pow(10., v);
        if (((*ptr->TickFormat)[ptr->counter]).substr(0, 1) == "(") { //internal format, call internal func "STRING"
          EnvT *e = ptr->e;
          static int stringIx = LibFunIx("STRING");
          assert(stringIx >= 0);
          EnvT* newEnv = new EnvT(e, libFunList[stringIx], NULL);
          Guard<EnvT> guard(newEnv);
          // add parameters
          newEnv->SetNextPar(new DDoubleGDL(v));
          newEnv->SetNextPar(new DStringGDL(((*ptr->TickFormat)[ptr->counter]).c_str()));
          // make the call
          BaseGDL* res = static_cast<DLibFun*> (newEnv->GetPro())->Fun()(newEnv);
          strncpy(label, (*static_cast<DStringGDL*> (res))[0].c_str(), 1000);
        } else // external function: if tickunits not specified, pass Axis (int), Index(int),Value(Double)
          //    else pass also Level(int)
          // Thanks to Marc for code snippet!
          // NOTE: this encompasses the 'LABEL_DATE' format, an existing procedure in the IDL library.
        {
          EnvT *e = ptr->e;
          DString callF = (*ptr->TickFormat)[ptr->counter];
          // this is a function name -> convert to UPPERCASE
          callF = StrUpCase(callF);
          //  Search in user proc and function
          SizeT funIx = GDLInterpreter::GetFunIx(callF);

          EnvUDT* newEnv = new EnvUDT(e->CallingNode(), funList[ funIx], (DObjGDL**) NULL);
          Guard< EnvUDT> guard(newEnv);
          // add parameters
          newEnv->SetNextPar(new DLongGDL(axis - 1)); //axis in PLPLOT starts at 1, it starts at 0 in IDL
          newEnv->SetNextPar(new DLongGDL(internalIndex)); //index
          newEnv->SetNextPar(new DDoubleGDL(v)); //value
          newEnv->SetNextPar(new DLongGDL(ptr->counter)); //level
          // guard *before* pushing new env
          StackGuard<EnvStackT> guard1(e->Interpreter()->CallStack());
          e->Interpreter()->CallStack().push_back(newEnv);
          guard.release();

          BaseGDL* retValGDL = e->Interpreter()->call_fun(static_cast<DSubUD*> (newEnv->GetPro())->GetTree());
          // we are the owner of the returned value
          Guard<BaseGDL> retGuard(retValGDL);
          strncpy(label, (*static_cast<DStringGDL*> (retValGDL))[0].c_str(), 1000);
        }
      }
    }
    //translate format codes (as in mtex).
    double nchars;
    std::string out = ptr->a->TranslateFormatCodes(label, &nchars);
    ptr->nchars = max(ptr->nchars, nchars);
    strcpy(label, out.c_str());
    internalIndex++;
  }

  
  //this will take into account !Z.REGION to interpret zValue as a percentage between Z.REGION[0] and !Z.REGION[1]
  void gdlStartT3DMatrixDriverTransform( GDLGStream *a, DDouble zValue){
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset
    static unsigned tTag = pStruct->Desc()->TagIndex("T");
    for (int i = 0; i < 16; ++i) PlotDevice3D.T[i] = (*static_cast<DDoubleGDL*> (pStruct->GetTag(tTag, 0)))[i];
    PLFLT sxmin,symin,sxmax,symax,szmin,szmax;
    a->getSubpageRegion(sxmin,symin,sxmax,symax,&szmin,&szmax);
    zValue = (std::isfinite(zValue))?zValue:0; //necessary as NaN means something.
    PlotDevice3D.zValue = szmin+zValue*(szmax-szmin);
    a->cmd(PLESC_3D, &PlotDevice3D); //copy to driver's
  }
  void gdlStartSpecial3DDriverTransform( GDLGStream *a, GDL_3DTRANSFORMDEVICE &UseThis3DMatrix){
    for (int i = 0; i < 16; ++i) PlotDevice3D.T[i] = UseThis3DMatrix.T[i];
    PlotDevice3D.zValue = UseThis3DMatrix.zValue;
    a->cmd(PLESC_3D, &PlotDevice3D);//copy to driver's
  }
  void gdlExchange3DDriverTransform( GDLGStream *a){
    yzaxisExch(PlotDevice3D.T); //for zAxis ONLY
    a->cmd(PLESC_3D, &PlotDevice3D);//copy to driver's
  }
  void gdlFlipYPlotDirection( GDLGStream *a){
    for (int i = 0; i < 16; ++i) PlotDevice3D.T[i] = 0;
    PlotDevice3D.T[0] = 1;
    PlotDevice3D.T[5] = -1;
    PlotDevice3D.T[7] = 1; //displace in Y so that it appears in 3d viewport.
    PlotDevice3D.T[10] = 1;
    PlotDevice3D.T[15] = 1;
    a->cmd(PLESC_3D, &PlotDevice3D);//copy to driver's
  }
  void gdlSetZto3DDriverTransform( GDLGStream *a, DDouble zValue){
    PlotDevice3D.zValue=zValue; //for zAxis ONLY
    a->cmd(PLESC_3D, &PlotDevice3D);//copy to driver's
  }
  void gdlShiftYaxisUsing3DDriverTransform( GDLGStream *a, DDouble yval, bool invert){
    for (int i = 0; i < 16; ++i) PlotDevice3D.T[i] = 0;
    PlotDevice3D.T[0] = 1;
    PlotDevice3D.T[5] = invert?-1:1; //see gdlFlipYPlotDirection() above 
    PlotDevice3D.T[10] = 1;
    PlotDevice3D.T[15] = 1;
    PlotDevice3D.zValue=0;
    PlotDevice3D.T[7]=yval;
    a->cmd(PLESC_3D, &PlotDevice3D);//copy to driver's
  }
  void gdlStop3DDriverTransform( GDLGStream *a){ //stop and RESET to unity/
    for (int i = 0; i < 16; ++i) PlotDevice3D.T[i] = 0;PlotDevice3D.T[0] = 1;PlotDevice3D.T[5] = 1;PlotDevice3D.T[10] = 1;PlotDevice3D.T[15] = 1;
    PlotDevice3D.zValue=0;
    a->cmd(PLESC_2D, NULL);
  }
  //Convert 3D to 2D using a passed PlotDevice3D matrix
  void Matrix3DTransformXYZval(DDouble x, DDouble y, DDouble z, DDouble *xt, DDouble *yt, DDouble* t) {
    *xt = x * t[0] + y * t[1] + z * t[2] + t[3];
    *yt = x * t[4] + y * t[5] + z * t[6] + t[7];
  }
 
  bool T3Denabled()
  {
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    DLong ok4t3d=(*static_cast<DLongGDL*>(pStruct->GetTag(pStruct->Desc()->TagIndex("T3D"), 0)))[0];
    if (ok4t3d==0) return false; else return true;
  }

  void yzaxisExch(DDouble* me) {
    DDouble tmp=me[2];
    me[2]=me[1];
    me[1]=tmp;
    tmp=me[6];
    me[6]=me[5];
    me[5]=tmp;
    tmp=me[10];
    me[10]=me[9];
    me[9]=tmp;
    tmp=me[14];
    me[14]=me[13];
    me[13]=tmp;
  }
  void yaxisFlip(DDouble* me) {for (auto i=0; i<2; ++i) me[i]*=-1;}

  void usersym(EnvT *e)
  {
    DFloatGDL *xyVal, *xVal, *yVal;
    Guard<BaseGDL> p0_guard;
    DLong n;
    DInt do_fill;
    bool do_color;
    bool do_thick;
    DFloat thethick;
    DLong thecolor;
    DFloat *x, *y;
    SizeT nParam=e->NParam(1);
    if ( nParam==1 )
    {
      BaseGDL* p0=e->GetNumericArrayParDefined(0)->Transpose(NULL); //hence [49,2]

      xyVal=static_cast<DFloatGDL*>
      (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      p0_guard.Reset(p0); // delete upon exit

      if ( xyVal->Rank()!=2||xyVal->Dim(1)!=2 )
        e->Throw(e->GetParString(0)+" must be a 2-dim array of type [2,N] in this context.");

      if ( xyVal->Dim(0)>49 )
      {
        e->Throw("Max array size for USERSYM is 49");
      }
      n=xyVal->Dim(0);
      // array is in the good order for direct C assignement
      x=&(*xyVal)[0];
      y=&(*xyVal)[n];
    }
    else
    {
      xVal=e->GetParAs< DFloatGDL>(0);
      if ( xVal->Rank()!=1 )
        e->Throw(e->GetParString(0)+" must be a 1D array in this context: ");

      yVal=e->GetParAs< DFloatGDL>(1);
      if ( yVal->Rank()!=1 )
        e->Throw("Expression must be a 1D array in this context: "+e->GetParString(1));

      if ( xVal->Dim(0)!=yVal->Dim(0) )
      {
        e->Throw("Arrays must have same size ");
      }

      if ( xVal->Dim(0)>49 )
      {
        e->Throw("Max array size for USERSYM is 49");
      }
      n=xVal->Dim(0);
      x=&(*xVal)[0];
      y=&(*yVal)[0];
    }
    do_fill=0;
    int FILLIx = e->KeywordIx("FILL");
    if ( e->KeywordSet(FILLIx) )
    {
      do_fill=1;
    }
    //IDL does not complain if color is undefined.
    do_color=false;
    thecolor=0;
    int COLORIx = e->KeywordIx("COLOR");
    if ( e->KeywordPresent(COLORIx))
    {
      if (e->IfDefGetKWAs<DLongGDL>( COLORIx )) {
        e->AssureLongScalarKW(COLORIx, thecolor);
        do_color=true;
      }
    }
    //IDL does not complain if thick is undefined.
    do_thick=false;
    thethick=0;
    int THICKIx = e->KeywordIx("THICK");
    if ( e->KeywordPresent(THICKIx))
    {
      if (e->IfDefGetKWAs<DFloatGDL>( THICKIx )) {
        e->AssureFloatScalarKW(THICKIx, thethick);
        do_thick=true;
      }
    }    
    SetUsym(n, do_fill, x, y, do_color, thecolor, do_thick, thethick);
  }

  void SelfProjectXY(DDoubleGDL *x, DDoubleGDL *y) {
    //Take care of projections: better to duplicate the code 
    //projections: X & Y to be converted to u,v BEFORE plotting in NORM coordinates
    assert (x->N_Elements()==y->N_Elements());
    bool mapSet = false;
    get_mapset(mapSet);
    SizeT nEl=x->N_Elements();
    
    if (mapSet) {
#ifdef USE_LIBPROJ
      ref = map_init();
      if (ref == NULL) BaseGDL::interpreter->CallStack().back()->Throw("Projection initialization failed.");
      LPTYPE idata;
      XYTYPE odata;
      for (OMPInt i = 0; i < nEl; ++i) {
#if LIBPROJ_MAJOR_VERSION >= 5
        idata.lam = (*x)[i] * DEG_TO_RAD;
        idata.phi = (*y)[i] * DEG_TO_RAD;
        odata = protect_proj_fwd_lp(idata, ref);
        (*x)[i] = odata.x;
        (*y)[i] = odata.y;
#else
        idata.u = (*x)[i] * DEG_TO_RAD;
        idata.v = (*y)[i] * DEG_TO_RAD;
        odata = PJ_FWD(idata, ref);
        (*x)[i] = odata.u;
        (*y)[i] = odata.v;
#endif
      }
#endif
    }
  }
  void SelfProjectXY(SizeT nEl, DDouble *x, DDouble *y, COORDSYS const coordinateSystem) {
    //Take care of projections: better to duplicate the code 
    //projections: X & Y to be converted to u,v BEFORE plotting in NORM coordinates
    bool mapSet = false;
    get_mapset(mapSet);
    mapSet = (mapSet && coordinateSystem == DATA);

    if (mapSet) {
#ifdef USE_LIBPROJ
      ref = map_init();
      if (ref == NULL) BaseGDL::interpreter->CallStack().back()->Throw("Projection initialization failed.");
      LPTYPE idata;
      XYTYPE odata;
      for (OMPInt i = 0; i < nEl; ++i) {
#if LIBPROJ_MAJOR_VERSION >= 5
        idata.lam = x[i] * DEG_TO_RAD;
        idata.phi = y[i] * DEG_TO_RAD;
        odata = protect_proj_fwd_lp(idata, ref);
        x[i] = odata.x;
        y[i] = odata.y;
#else
        idata.u = x[i] * DEG_TO_RAD;
        idata.v = y[i] * DEG_TO_RAD;
        odata = PJ_FWD(idata, ref);
        x[i] = odata.u;
        y[i] = odata.v;
#endif
      }
#endif
    }
  }
 
  //passes the return of GDLgrGetProjectPolygon() to the 3D Matrix, to be called when real 3D is used in conjunction with a projection.
  void SelfPDotTTransformProjectedPolygonTable(DDoubleGDL *lonlat){
//  std::cerr<<"SelfPDotTTransformProjectedPolygonTable()"<<std::endl;
    //retrieve !P.T 
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset
    static unsigned tTag = pStruct->Desc()->TagIndex("T");
    DDouble* t= static_cast<DDouble*>(pStruct->GetTag(tTag, 0)->DataAddr());
    SizeT n = lonlat->Dim(0);
    assert(lonlat->Dim(1)==3);
    DDouble  *sx, *sy , *sz;
    GetSFromPlotStructs(&sx, &sy, &sz);
    DDouble* x=&((*lonlat)[0]); //static_cast<DDouble*>(lonlat->DataAddr());
    DDouble* y=&((*lonlat)[n]); //static_cast<DDouble*>(lonlat->DataAddr())+n*sizeof(DDouble);
    DDouble* z=&((*lonlat)[2*n]); //static_cast<DDouble*>(lonlat->DataAddr())+2*n*sizeof(DDouble);
    DDouble a,b,c,w;
    for (SizeT i=0; i< n; ++i) {
      x[i] = sx[0] + sx[1] * x[i]; //normalize here
      y[i] = sy[0] + sy[1] * y[i]; //normalize here
      z[i] = sz[0] + sz[1] * z[i]; //normalize here
      a = x[i] * t[0] + y[i] * t[1] + z[i] * t[2] + t[3]; 
      b = x[i] * t[4] + y[i] * t[5] + z[i] * t[6] + t[7]; 
      c = x[i] * t[8] + y[i] * t[9] + z[i] * t[10] + t[11]; 
      w = x[i] * t[12] + y[i] * t[13] + z[i] * t[14] + t[15];

      x[i] = a / w; 
      y[i] = b / w; 
//      z[i] = c / w;
    }
  }
  
void SelfNormLonLat(DDoubleGDL *lonlat) {
//    std::cerr << "SelfNormLonLat()" << std::endl;
    DDouble *sx, *sy, *sz;
    GetSFromPlotStructs(&sx, &sy, &sz);
    assert(lonlat->Dim(1) == 3);
    SizeT Dim1 = lonlat->Dim(0);
    for (auto i = 0; i < Dim1; ++i) (*lonlat)[i] = sx[0] + sx[1] * (*lonlat)[i];
    for (auto i = Dim1; i < 2 * Dim1; ++i) (*lonlat)[i] = sy[0] + sy[1] * (*lonlat)[i];
    for (auto i = 2 * Dim1; i < 3 * Dim1; ++i) (*lonlat)[i] = sz[0] + sz[1] * (*lonlat)[i];
  }
#ifdef USE_LIBPROJ

//ALL-IN-ONE: if justProject is true, will just convert lons and lats to projected coordinates. Z is unchanged, returned value is NULL.
// if justProject is false (default), will output a [N,2] or [N,3] (if z is not NULL) 'polygon' list  
 DDoubleGDL* GDLgrGetProjectPolygon(GDLGStream * a, PROJTYPE ref, DStructGDL* map, DDoubleGDL *lons, DDoubleGDL *lats, DDoubleGDL *zVal, bool isRadians, bool const doFill, bool const doLines, DLongGDL *&conn) {
//    std::cerr << "GDLgrGetProjectPolygon()" << std::endl;

    DStructGDL* localMap = map;
    if (localMap == NULL) localMap = SysVar::Map();
    DLongGDL *gons, *lines;
    if (!isRadians) {
      SizeT nin = lons->N_Elements();
      if ((GDL_NTHREADS = parallelize(nin, TP_MEMORY_ACCESS)) == 1) {
        for (OMPInt in = 0; in < nin; in++) { //pass in radians for gdlProjForward
          (*lons)[in] *= DEG_TO_RAD;
          (*lats)[in] *= DEG_TO_RAD;
        }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt in = 0; in < nin; in++) { //pass in radians for gdlProjForward
          (*lons)[in] *= DEG_TO_RAD;
          (*lats)[in] *= DEG_TO_RAD;
        }
      }
    }
    
    bool doConn = (conn != NULL);
    DDoubleGDL *res = gdlProjForward(ref, localMap, lons, lats, zVal, conn, doConn, gons, doFill, lines, doLines, false, true); //transposed=true for speed  and gives a [N,3] table
    //res may be DDoubleGDL(-1) 
    SizeT nout = res->Dim(0);
    if (nout < 1) {
      GDLDelete(res);
      return NULL;
    } //projection clipped totally these values.
    assert(res->Dim(1) == 3);
    if (doFill) conn = gons;
    else conn = lines; //return appropriate connectivity list
    return res;
  }
  
 //for projected lines and fills, does not handle PSYMs
  void GDLgrPlotProjectedPolygon(GDLGStream * a, DDoubleGDL *lonlat, bool const doFill, DLongGDL *conn) {
    //convert to normed values
//    std::cerr<<"GDLgrPlotProjectedPolygon()"<<std::endl;
    if (doFill && (conn->N_Elements()<3) ) return; //protection
    else if (conn->N_Elements()<2) return; //protection
    SizeT nout=lonlat->Dim(0);
    int minpoly;
    if (doFill) {
      minpoly = 3;
    } else {
      minpoly = 2;
    }
    SizeT index = 0;
    SizeT size;
    SizeT start;
    while (index < conn->N_Elements()) {
      size = (*conn)[index];
      if (size == 0) break; //cannot be negative!
      start = (*conn)[index + 1];
      if (size >= minpoly) {
        if (doFill) {
          a->fill(size, (PLFLT*) &((*lonlat)[start]), (PLFLT*) &((*lonlat)[start + nout]));
        } else {
          a->line(size, (PLFLT*) &((*lonlat)[start]), (PLFLT*) &((*lonlat)[start + nout]));
        }
      }
      index += (size + 1);
    }
  }
#endif
  
  
    void gdlSetGraphicsBackgroundColorFromKw(EnvT *e, GDLGStream *a, bool kw) {
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
    DLong background =
      (*static_cast<DLongGDL*>
      (pStruct->GetTag(pStruct->Desc()->TagIndex("BACKGROUND"), 0)))[0];
    if (kw) {
      int BACKGROUNDIx = e->KeywordIx("BACKGROUND");
      e->AssureLongScalarKWIfPresent(BACKGROUNDIx, background);
    }
    DLong decomposed = GraphicsDevice::GetDevice()->GetDecomposed();
    a->Background(background, decomposed);
  }

  void gdlSetGraphicsForegroundColorFromBackgroundKw(EnvT *e, GDLGStream *a, bool kw) {
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
    DLong background =
      (*static_cast<DLongGDL*>
      (pStruct->GetTag(pStruct->Desc()->TagIndex("BACKGROUND"), 0)))[0];
    if (kw) {
      int BACKGROUNDIx = e->KeywordIx("BACKGROUND");
      e->AssureLongScalarKWIfPresent(BACKGROUNDIx, background);
    }
    DLong decomposed = GraphicsDevice::GetDevice()->GetDecomposed();
    a->Color(background, decomposed);
  }

  void gdlSetGraphicsForegroundColorFromKw(EnvT *e, GDLGStream *a, string OtherColorKw) {
    // Get COLOR from PLOT system variable
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
    DLong color =
      (*static_cast<DLongGDL*>
      (pStruct->GetTag(pStruct->Desc()->TagIndex("COLOR"), 0)))[0];

    DLongGDL *colorVect;
    int colorIx = e->KeywordIx("COLOR");
    int realcolorIx = colorIx;
    //eventually do not get color from standard "COLOR" keyword but from another...
    if (OtherColorKw != "") realcolorIx = e->KeywordIx(OtherColorKw);
    if (e->GetDefinedKW(realcolorIx) != NULL) {
      colorVect = e->GetKWAs<DLongGDL>(realcolorIx); //color can be vectorial, but...
      color = (*colorVect)[0]; //this function only sets color to 1st arg in list!
    }
    // Get decomposed value for colors
    DLong decomposed = GraphicsDevice::GetDevice()->GetDecomposed();
    a->Color(color, decomposed);
  }

  void gdlGetPsym(EnvT *e, DLong &psym) {
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
    psym = (*static_cast<DLongGDL*>
      (pStruct->GetTag(pStruct->Desc()->TagIndex("PSYM"), 0)))[0];
    int PSYMIx = e->KeywordIx("PSYM");
    e->AssureLongScalarKWIfPresent(PSYMIx, psym);
    if (psym > 10 || psym < -8 || psym == 9)
      e->Throw(
      "PSYM (plotting symbol) out of range.");
  }

  void gdlSetSymsize(EnvT *e, GDLGStream *a) {
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
    DFloat symsize = (*static_cast<DFloatGDL*>
      (pStruct->GetTag(pStruct->Desc()->TagIndex("SYMSIZE"), 0)))[0];
    //NOTE THAT AS OF IDL 8.2 !P.SYMSIZE, HOWEVER EXISTING, IS NOT TAKEN INTO ACCOUNT. We however do not want
    //to reproduce this feature.
    int SYMSIZEIx = e->KeywordIx("SYMSIZE");
    e->AssureFloatScalarKWIfPresent(SYMSIZEIx, symsize);
    if (symsize <= 0.0) symsize = 1.0;
    a->setSymbolSize(symsize);
  }
  //  void GetUserSymSize(EnvT *e, GDLGStream *a, DDouble& UsymConvX, DDouble& UsymConvY)
  //  {
  //    //get symsize
  //    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
  //    DFloat symsize=(*static_cast<DFloatGDL*>
  //                    (pStruct->GetTag(pStruct->Desc()->TagIndex("SYMSIZE"), 0)))[0];
  //    int SYMSIZEIx = e->KeywordIx("SYMSIZE");
  //    e->AssureFloatScalarKWIfPresent(SYMSIZEIx, symsize);
  //    if ( symsize<=0.0 ) symsize=1.0;
  //    
  //    UsymConvX=(0.5*symsize*(a->wCharLength()/a->charScale())); //be dependent only on symsize!
  //    UsymConvY=(0.5*symsize*(a->wCharHeight()/a->charScale()));
  //    PLFLT wun, wdeux, wtrois, wquatre; //take care of axes world orientation!
  //    a->pageWorldCoordinates(wun, wdeux, wtrois, wquatre);
  //    if ((wdeux-wun)<0) UsymConvX*=-1.0;
  //    if ((wquatre-wtrois)<0) UsymConvY*=-1.0;
  //    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GetUserSymSize(%f,%f), charlen=%f, charheight=%f, charscale=%f\n",
  //				    UsymConvX, UsymConvY,a->wCharLength(),a->wCharHeight(),a->charScale());
  //  }

  void gdlSetPlotCharsize(EnvT *e, GDLGStream *a, bool accept_sizeKw) {
    PLFLT charsize;
    DDouble pmultiscale = 1.0;
    // get !P preference or !FANCY ... they should agree as charsize = 0.2*FANCY+0.8 
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset
    DFloat* charsizePos = &((*static_cast<DFloatGDL*> (pStruct->GetTag(pStruct->Desc()->TagIndex("CHARSIZE"), 0)))[0]);
    charsize = charsizePos[0];
    //    //if charsize==0 see if !FANCY is set to something above 1 or below 1
    //    DIntGDL* fancy= SysVar::GetFancy();
    //    if ((*fancy)[0] > -4) { //negative values are a mess
    //     PLFLT fancySize = 0.2 * (*fancy)[0] + 0.8;
    //     if (fancySize != charsize) { //make them agree
    //      charsize = fancySize;
    //      charsizePos[0] = charsize;
    //     }
    //    }
    //overload with command preference. Charsize may be a vector now in some gdl commands, take care of it:
    if (accept_sizeKw) //XYOUTS specials!
    {
      int SIZEIx = e->KeywordIx("SIZE"); //define here only (else trig an assert() )
      DFloat fcharsize;
      fcharsize = charsize;
      e->AssureFloatScalarKWIfPresent(SIZEIx, fcharsize);
      charsize = fcharsize;
    }
    int charsizeIx = e->KeywordIx("CHARSIZE");
    if (e->GetDefinedKW(charsizeIx) != NULL) {
      DFloatGDL* charsizeVect = e->GetKWAs<DFloatGDL>(charsizeIx);
      charsize = (*charsizeVect)[0];
    }
    if (charsize <= 0.0) charsize = 1.0;
    // adjust if MULTI:
    DLongGDL* pMulti = SysVar::GetPMulti();
    if ((*pMulti)[1] > 2 || (*pMulti)[2] > 2) pmultiscale = 0.5;
    a->sizeChar(charsize * pmultiscale);
  }

  void gdlSetPlotCharthick(EnvT *e, GDLGStream *a) {
    // get !P preference
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
    DFloat charthick = (*static_cast<DFloatGDL*>
      (pStruct->GetTag
      (pStruct->Desc()->TagIndex("CHARTHICK"), 0)))[0];
    int charthickIx = e->KeywordIx("CHARTHICK"); //Charthick values may be vector in GDL, not in IDL!
    if (e->GetDefinedKW(charthickIx) != NULL) {
      DFloatGDL* charthickVect = e->GetKWAs<DFloatGDL>(charthickIx);
      charthick = (*charthickVect)[0];
    }
    if (charthick <= 0.0) charthick = 1.0;
    a->Thick(charthick);
  }

  PLFLT gdlComputeTickInterval(EnvT *e, int axisId, DDouble &min, DDouble &max, bool log, int level) {
    DLong nticks = 0;

    int XTICKSIx = e->KeywordIx("XTICKS");
    int YTICKSIx = e->KeywordIx("YTICKS");
    int ZTICKSIx = e->KeywordIx("ZTICKS");
    int choosenIx = XTICKSIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTICKSIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTICKSIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTICKSIx;
    }

    if (Struct != NULL) {
      unsigned tickTag = Struct->Desc()->TagIndex("TICKS");
      nticks = (*static_cast<DLongGDL*> (Struct->GetTag(tickTag, 0)))[0];
    }
    e->AssureLongScalarKWIfPresent(choosenIx, nticks);

    PLFLT intv;
    static DDouble multiplier[]={0,365.25,30,1,1./24.,1./(24*60.),1./(24.*60*60)};
    int code = gdlGetCalendarCode(e, axisId, level);
    switch (code) {
    case 0:
    case 7:
      if (nticks == 0) {
        intv = (log) ? AutoLogTick(min, max) : AutoTick(max - min);
      } else {
        intv = (log) ? log10(max - min) / nticks : (max - min) / nticks;
      }
      return intv;
    default:
      intv = multiplier[code];
      if (nticks == 0) {
        DDouble nintv=(max - min)/intv;
        PLFLT test=AutoTick(nintv);
        if (test > 1) return intv*test; else {
          if (nintv<1) return (max-min); //at least one tick
          return intv;
        }
      } else {
        intv = (log) ? log10(max - min) / nticks : (max - min) / nticks;
      }
      return intv;
    }
  }

  void gdlGetDesiredAxisCharsize(EnvT* e, int axisId, DFloat &charsize) {
    //default:
    charsize = 1.0;
    // get !P preference. Even if [xyz]charsize is absent, presence of charsize or !P.charsize must be taken into account.
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
    charsize = (*static_cast<DFloatGDL*>
      (pStruct->GetTag
      (pStruct->Desc()->TagIndex("CHARSIZE"), 0)))[0];
    int CharsizeIx = e->KeywordIx("CHARSIZE");
    //cerr<<" CHARSIZE: "<< CharsizeIx<<" ("<< &CharsizeIx<<")"<<endl;
    e->AssureFloatScalarKWIfPresent(CharsizeIx, charsize); // option charsize overloads P.CHARSIZE
    if (charsize == 0) charsize = 1.0;
    // Axis Preference. Is a Multiplier!
    int XCharsizeIx = e->KeywordIx("XCHARSIZE");
    int YCharsizeIx = e->KeywordIx("YCHARSIZE");
    int ZCharsizeIx = e->KeywordIx("ZCHARSIZE");
    int choosenIx = XCharsizeIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XCharsizeIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YCharsizeIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZCharsizeIx;
    }

    if (Struct != NULL) {
      unsigned charsizeTag = Struct->Desc()->TagIndex("CHARSIZE"); //[XYZ].CHARSIZE
      DFloat axisCharsizeMultiplier = (*static_cast<DFloatGDL*> (Struct->GetTag(charsizeTag, 0)))[0];
      e->AssureFloatScalarKWIfPresent(choosenIx, axisCharsizeMultiplier); //option [XYZ]CHARSIZE overloads ![XYZ].CHARSIZE
      if (axisCharsizeMultiplier > 0.0) charsize *= axisCharsizeMultiplier; //IDL Behaviour...
    }
  }

  void gdlSetAxisCharsize(EnvT *e, GDLGStream *a, int axisId) {

    DFloat charsize = 0.0;
    DDouble pmultiscale = 1.0;
    gdlGetDesiredAxisCharsize(e, axisId, charsize);
    // adjust if MULTI:
    DLongGDL* pMulti = SysVar::GetPMulti();
    if ((*pMulti)[1] > 2 || (*pMulti)[2] > 2) pmultiscale = 0.5; //IDL behaviour
    // scale default value (which depends on number of subpages)
    // a->schr(0.0, charsize*pmultiscale);
    a->sizeChar(charsize * pmultiscale);
  }

  void gdlGetDesiredAxisGridStyle(EnvT* e, int axisId, DLong &axisGridstyle) {
    axisGridstyle = 0;
    DStructGDL* Struct = NULL;
    int XGRIDSTYLEIx = e->KeywordIx("XGRIDSTYLE");
    int YGRIDSTYLEIx = e->KeywordIx("YGRIDSTYLE");
    int ZGRIDSTYLEIx = e->KeywordIx("ZGRIDSTYLE");
    int choosenIx = XGRIDSTYLEIx;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XGRIDSTYLEIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YGRIDSTYLEIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZGRIDSTYLEIx;
    }

    if (Struct != NULL) {
      unsigned gridstyleTag = Struct->Desc()->TagIndex("GRIDSTYLE");
      axisGridstyle = (*static_cast<DLongGDL*> (Struct->GetTag(gridstyleTag, 0)))[0];
      e->AssureLongScalarKWIfPresent(choosenIx, axisGridstyle);
    }
  }

  void gdlGetDesiredAxisMargin(EnvT *e, int axisId, DFloat &start, DFloat &end) {
    int XMARGINIx = e->KeywordIx("XMARGIN");
    int YMARGINIx = e->KeywordIx("YMARGIN");
    int ZMARGINIx = e->KeywordIx("ZMARGIN");
    int choosenIx = XMARGINIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XMARGINIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YMARGINIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZMARGINIx;
    }

    if (Struct != NULL) {
      unsigned marginTag = Struct->Desc()->TagIndex("MARGIN");
      start = (*static_cast<DFloatGDL*> (Struct->GetTag(marginTag, 0)))[0];
      end = (*static_cast<DFloatGDL*> (Struct->GetTag(marginTag, 0)))[1];
    }

    BaseGDL* Margin = e->GetKW(choosenIx);
    if (Margin != NULL) {
      if (Margin->N_Elements() > 2)
        e->Throw("Keyword array parameter " + axisName[axisId] + "MARGIN must have from 1 to 2 elements.");
      Guard<DFloatGDL> guard;
      DFloatGDL* MarginF = static_cast<DFloatGDL*>
        (Margin->Convert2(GDL_FLOAT, BaseGDL::COPY));
      guard.Reset(MarginF);
      start = (*MarginF)[0];
      if (MarginF->N_Elements() > 1)
        end = (*MarginF)[1];
    }
  }

  void gdlGetDesiredAxisMinor(EnvT* e, int axisId, DLong &axisMinor) {
    axisMinor = 0;
    int XMINORIx = e->KeywordIx("XMINOR");
    int YMINORIx = e->KeywordIx("YMINOR");
    int ZMINORIx = e->KeywordIx("ZMINOR");
    int choosenIx = XMINORIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XMINORIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YMINORIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZMINORIx;
    }
    if (Struct != NULL) {
      unsigned AxisMinorTag = Struct->Desc()->TagIndex("MINOR");
      axisMinor = (*static_cast<DLongGDL*> (Struct->GetTag(AxisMinorTag, 0)))[0];
    }
    e->AssureLongScalarKWIfPresent(choosenIx, axisMinor);
  }

  bool gdlGetDesiredAxisRange(EnvT *e, int axisId, DDouble &start, DDouble &end) {
    bool set = false;
    int XRANGEIx = e->KeywordIx("XRANGE");
    int YRANGEIx = e->KeywordIx("YRANGE");
    int ZRANGEIx = e->KeywordIx("ZRANGE");
    int choosenIx = XRANGEIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XRANGEIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YRANGEIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZRANGEIx;
    }
    if (Struct != NULL) {
      DDouble test1, test2;
      unsigned rangeTag = Struct->Desc()->TagIndex("RANGE");
      test1 = (*static_cast<DDoubleGDL*> (Struct->GetTag(rangeTag, 0)))[0];
      test2 = (*static_cast<DDoubleGDL*> (Struct->GetTag(rangeTag, 0)))[1];
      if (!((test1 - test2) == 0.0)) {
        start = test1;
        end = test2;
        set = true;
      }
    }
    BaseGDL* Range = e->GetDefinedKW(choosenIx);
    if (Range != NULL) {
      if (Range->N_Elements() != 2)
        e->Throw("Keyword array parameter " + axisName[axisId] + "RANGE must have 2 elements.");
      Guard<DDoubleGDL> guard;
      DDoubleGDL* RangeF = static_cast<DDoubleGDL*> (Range->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      guard.Reset(RangeF);
      if (!(((*RangeF)[0]-(*RangeF)[1]) == 0.0)) {
        start = (*RangeF)[0];
        end = (*RangeF)[1];
        set = true;
      }
    }
    return set;
  }

  void gdlGetDesiredAxisStyle(EnvT *e, int axisId, DLong &style) {
    int XSTYLEIx = e->KeywordIx("XSTYLE");
    int YSTYLEIx = e->KeywordIx("YSTYLE");
    int ZSTYLEIx = e->KeywordIx("ZSTYLE");
    int choosenIx = XSTYLEIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XSTYLEIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YSTYLEIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZSTYLEIx;
    }

    if (Struct != NULL) {
      int styleTag = Struct->Desc()->TagIndex("STYLE");
      style = (*static_cast<DLongGDL*> (Struct->GetTag(styleTag, 0)))[0];
    }

    e->AssureLongScalarKWIfPresent(choosenIx, style);
  }

  void gdlGetDesiredAxisThick(EnvT *e, int axisId, DFloat &thick) {
    thick = 1.0;
    int XTHICKIx = e->KeywordIx("XTHICK");
    int YTHICKIx = e->KeywordIx("YTHICK");
    int ZTHICKIx = e->KeywordIx("ZTHICK");
    int choosenIx = XTHICKIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTHICKIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTHICKIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTHICKIx;
    }

    if (Struct != NULL) {
      //not static!
      int thickTag = Struct->Desc()->TagIndex("THICK");
      thick = (*static_cast<DFloatGDL*> (Struct->GetTag(thickTag, 0)))[0];
    }
    e->AssureFloatScalarKWIfPresent(choosenIx, thick);
    if (thick <= 0.0) thick = 1.0;
  }

  void gdlGetDesiredAxisTickget(EnvT *e, int axisId, DDoubleGDL *Axistickget) {
    //TODO!
  }

  void gdlGetDesiredAxisTickFormat(EnvT* e, int axisId, DStringGDL* &axisTickformatVect) {
    int XTICKFORMATIx = e->KeywordIx("XTICKFORMAT");
    int YTICKFORMATIx = e->KeywordIx("YTICKFORMAT");
    int ZTICKFORMATIx = e->KeywordIx("ZTICKFORMAT");
    int choosenIx = XTICKFORMATIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTICKFORMATIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTICKFORMATIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTICKFORMATIx;
    }

    if (Struct != NULL) {
      unsigned AxisTickformatTag = Struct->Desc()->TagIndex("TICKFORMAT");
      axisTickformatVect = static_cast<DStringGDL*> (Struct->GetTag(AxisTickformatTag, 0));
    }
    if (e->GetDefinedKW(choosenIx) != NULL) {
      axisTickformatVect = e->GetKWAs<DStringGDL>(choosenIx);
    }
  }

  void gdlGetDesiredAxisTickInterval(EnvT* e, int axisId, DDouble &axisTickinterval) {
    axisTickinterval = 0;
    int XTICKINTERVALIx = e->KeywordIx("XTICKINTERVAL");
    int YTICKINTERVALIx = e->KeywordIx("YTICKINTERVAL");
    int ZTICKINTERVALIx = e->KeywordIx("ZTICKINTERVAL");
    int choosenIx = XTICKINTERVALIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTICKINTERVALIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTICKINTERVALIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTICKINTERVALIx;
    }

    if (Struct != NULL) {
      axisTickinterval = (*static_cast<DDoubleGDL*>
        (Struct->GetTag
        (Struct->Desc()->TagIndex("TICKINTERVAL"), 0)))[0];
    }
    e->AssureDoubleScalarKWIfPresent(choosenIx, axisTickinterval);
    if (axisTickinterval < 0) axisTickinterval = 0;
  }

  void gdlGetDesiredAxisTickLayout(EnvT* e, int axisId, DLong &axisTicklayout) {
    axisTicklayout = 0;
    int XTICKLAYOUTIx = e->KeywordIx("XTICKLAYOUT");
    int YTICKLAYOUTIx = e->KeywordIx("YTICKLAYOUT");
    int ZTICKLAYOUTIx = e->KeywordIx("ZTICKLAYOUT");
    int choosenIx = XTICKLAYOUTIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTICKLAYOUTIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTICKLAYOUTIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTICKLAYOUTIx;
    }
    if (Struct != NULL) {
      axisTicklayout = (*static_cast<DLongGDL*>
        (Struct->GetTag
        (Struct->Desc()->TagIndex("TICKLAYOUT"), 0)))[0];
    }
    e->AssureLongScalarKWIfPresent(choosenIx, axisTicklayout);
  }

  void gdlGetDesiredAxisTickLen(EnvT* e, int axisId, DFloat &ticklen) {
    // order: !P.TICKLEN, TICKLEN, !X.TICKLEN, /XTICKLEN
    // get !P preference
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
    ticklen = (*static_cast<DFloatGDL*>
      (pStruct->GetTag
      (pStruct->Desc()->TagIndex("TICKLEN"), 0)))[0]; //!P.TICKLEN, always exist, may be 0
    int TICKLENIx = e->KeywordIx("TICKLEN");
    e->AssureFloatScalarKWIfPresent(TICKLENIx, ticklen); //overwritten by TICKLEN option

    int XTICKLENIx = e->KeywordIx("XTICKLEN");
    int YTICKLENIx = e->KeywordIx("YTICKLEN");
    int ZTICKLENIx = e->KeywordIx("ZTICKLEN");
    int choosenIx = XTICKLENIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTICKLENIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTICKLENIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTICKLENIx;
    }
    if (Struct != NULL) {
      unsigned ticklenTag = Struct->Desc()->TagIndex("TICKLEN");
      DFloat axisTicklen = (*static_cast<DFloatGDL*> (Struct->GetTag(ticklenTag, 0)))[0]; //![XYZ].TICKLEN (exist)
      e->AssureFloatScalarKWIfPresent(choosenIx, axisTicklen); //overriden by kw
      if (axisTicklen != 0.0) ticklen = axisTicklen;
    }
  }

  void gdlGetDesiredAxisTickName(EnvT* e, GDLGStream* a, int axisId, DStringGDL* &axisTicknameVect) {

    int XTICKNAMEIx = e->KeywordIx("XTICKNAME");
    int YTICKNAMEIx = e->KeywordIx("YTICKNAME");
    int ZTICKNAMEIx = e->KeywordIx("ZTICKNAME");
    int choosenIx = XTICKNAMEIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTICKNAMEIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTICKNAMEIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTICKNAMEIx;
    }
    if (Struct != NULL) {
      unsigned AxisTicknameTag = Struct->Desc()->TagIndex("TICKNAME");
      axisTicknameVect = static_cast<DStringGDL*> (Struct->GetTag(AxisTicknameTag, 0));
    }
    if (e->GetDefinedKW(choosenIx) != NULL) {
      axisTicknameVect = e->GetKWAs<DStringGDL>(choosenIx);
      //translate format codes here:
      //      for (SizeT iname=0; iname < axisTicknameVect->N_Elements(); ++iname) {
      //        std::string out = std::string("");
      //        a->TranslateFormatCodes(((*axisTicknameVect)[iname]).c_str(),out);
      ////TBD: not finished, see cases not treated in TransmateFormatCodes (gdlgstream.cpp)
      //        (*axisTicknameVect)[iname]=out;
      //      }
    }

  }

  void gdlGetDesiredAxisTicks(EnvT* e, int axisId, DLong &axisTicks) {
    axisTicks = 0;

    int XTICKSIx = e->KeywordIx("XTICKS");
    int YTICKSIx = e->KeywordIx("YTICKS");
    int ZTICKSIx = e->KeywordIx("ZTICKS");
    int choosenIx = XTICKSIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTICKSIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTICKSIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTICKSIx;
    }

    if (Struct != NULL) {
      axisTicks = (*static_cast<DLongGDL*>
        (Struct->GetTag
        (Struct->Desc()->TagIndex("TICKS"), 0)))[0];
    }
    e->AssureLongScalarKWIfPresent(choosenIx, axisTicks);
    if (axisTicks > 59) e->Throw("Value of number of ticks is out of allowed range.");
  }


  //if axis tick units is specified, first tickunit determines how the automatic limits are computed.
  // for example, if tickunits=['year','day'] the limits will be on a round nuber of years.
  // This is conveyed by the code

  int gdlGetCalendarCode(EnvT* e, int axisId, int level) {
    int XTICKUNITSIx = e->KeywordIx("XTICKUNITS");
    int YTICKUNITSIx = e->KeywordIx("YTICKUNITS");
    int ZTICKUNITSIx = e->KeywordIx("ZTICKUNITS");
    int choosenIx = XTICKUNITSIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTICKUNITSIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTICKUNITSIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTICKUNITSIx;
    }
    DStringGDL* axisTickunitsVect = NULL;
    if (Struct != NULL) {
      unsigned AxisTickunitsTag = Struct->Desc()->TagIndex("TICKUNITS");
      axisTickunitsVect = static_cast<DStringGDL*> (Struct->GetTag(AxisTickunitsTag, 0));
    }
    if (e->GetDefinedKW(choosenIx) != NULL) {
      axisTickunitsVect = e->GetKWAs<DStringGDL>(choosenIx);
    }
    int code = 0;
    DString what = StrUpCase((*axisTickunitsVect)[level]);
    if (what.substr(0, 4) == "YEAR") code = 1;
    else if (what.substr(0, 5) == "MONTH") code = 2;
    else if (what.substr(0, 3) == "DAY") code = 3;
    else if (what.substr(0, 7) == "NUMERIC") code = 3;
    else if (what.substr(0, 4) == "HOUR") code = 4;
    else if (what.substr(0, 6) == "MINUTE") code = 5;
    else if (what.substr(0, 6) == "SECOND") code = 6;
    else if (what.substr(0, 4) == "TIME") code = 7;
    return code;
  }

  void gdlGetDesiredAxisTickUnits(EnvT* e, int axisId, DStringGDL* &axisTickunitsVect) {
    int XTICKUNITSIx = e->KeywordIx("XTICKUNITS");
    int YTICKUNITSIx = e->KeywordIx("YTICKUNITS");
    int ZTICKUNITSIx = e->KeywordIx("ZTICKUNITS");
    int choosenIx = XTICKUNITSIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTICKUNITSIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTICKUNITSIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTICKUNITSIx;
    }
    if (Struct != NULL) {
      unsigned AxisTickunitsTag = Struct->Desc()->TagIndex("TICKUNITS");
      axisTickunitsVect = static_cast<DStringGDL*> (Struct->GetTag(AxisTickunitsTag, 0));
    }
    if (e->GetDefinedKW(choosenIx) != NULL) {
      axisTickunitsVect = e->GetKWAs<DStringGDL>(choosenIx);
    }
  }

  bool gdlHasTickUnits(EnvT* e, int axisId) {
    bool has = false;
    int XTICKUNITSIx = e->KeywordIx("XTICKUNITS");
    int YTICKUNITSIx = e->KeywordIx("YTICKUNITS");
    int ZTICKUNITSIx = e->KeywordIx("ZTICKUNITS");
    int choosenIx = XTICKUNITSIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTICKUNITSIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTICKUNITSIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTICKUNITSIx;
    }
    if (Struct != NULL) {
      unsigned AxisTickunitsTag = Struct->Desc()->TagIndex("TICKUNITS");
      DStringGDL* axisTickunitsVect = static_cast<DStringGDL*> (Struct->GetTag(AxisTickunitsTag, 0));
      for (auto i = 0; i < axisTickunitsVect->N_Elements(); ++i) {
        if ((*axisTickunitsVect)[i].size() > 0) {
          has = true;
          break;
        }
      }
    }
    if (e->GetDefinedKW(choosenIx) != NULL) {
      DStringGDL* axisTickunitsVect = e->GetKWAs<DStringGDL>(choosenIx);
      for (auto i = 0; i < axisTickunitsVect->N_Elements(); ++i) {
        if ((*axisTickunitsVect)[i].size() > 0) {
          has = true;
          break;
        }
      }
    }
    return has;
  }

  bool gdlGetDesiredAxisTickv(EnvT* e, int axisId, DDoubleGDL* axisTickvVect) {
    bool exist = false;
    int XTICKVIx = e->KeywordIx("XTICKV");
    int YTICKVIx = e->KeywordIx("YTICKV");
    int ZTICKVIx = e->KeywordIx("ZTICKV");
    int choosenIx = XTICKVIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTICKVIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTICKVIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTICKVIx;
    }
    if (Struct != NULL) {
      unsigned AxisTickvTag = Struct->Desc()->TagIndex("TICKV");
      axisTickvVect = static_cast<DDoubleGDL*> (Struct->GetTag(AxisTickvTag, 0));
      exist = true;
    }
    if (e->GetDefinedKW(choosenIx) != NULL) {
      axisTickvVect = e->GetKWAs<DDoubleGDL>(choosenIx);
      exist = true;
    }
    if (exist) { //there is something
      exist = false; //but it could be all Zeros...
      for (auto i = 0; i < axisTickvVect->N_Elements(); ++i) if ((*axisTickvVect)[i] != 0) {
          exist = true;
          break;
        }
    }
    return exist;
  }

  //if [X|Y|Z]TICK_GET was given for axis, write the values.

  void gdlWriteDesiredAxisTickGet(EnvT* e, int axisId, bool isLog) {
    int XTICKGIx = e->KeywordIx("XTICK_GET");
    int YTICKGIx = e->KeywordIx("YTICK_GET");
    int ZTICKGIx = e->KeywordIx("ZTICK_GET");
    int choosenIx = XTICKGIx;
    if (axisId == XAXIS) {
      choosenIx = XTICKGIx;
    }
    if (axisId == YAXIS) {
      choosenIx = YTICKGIx;
    }
    if (axisId == ZAXIS) {
      choosenIx = ZTICKGIx;
    }
    if (e->WriteableKeywordPresent(choosenIx)) {
      DDoubleGDL* val = getLabelingValues(axisId);
      if (val == NULL) e->Throw("Internal GDL error for [X|Y|Z]TICK_GET, please report");
      if (isLog) for (auto i = 0; i < val->N_Elements(); ++i) (*val)[i] = pow(10.0, (*val)[i]);
      e->SetKW(choosenIx, val);
    }
  }

  void gdlGetDesiredAxisTitle(EnvT *e, int axisId, DString &title) {
    int XTITLEIx = e->KeywordIx("XTITLE");
    int YTITLEIx = e->KeywordIx("YTITLE");
    int ZTITLEIx = e->KeywordIx("ZTITLE");
    int choosenIx = XTITLEIx;
    DStructGDL* Struct = NULL;
    if (axisId == XAXIS) {
      Struct = SysVar::X();
      choosenIx = XTITLEIx;
    }
    if (axisId == YAXIS) {
      Struct = SysVar::Y();
      choosenIx = YTITLEIx;
    }
    if (axisId == ZAXIS) {
      Struct = SysVar::Z();
      choosenIx = ZTITLEIx;
    }

    if (Struct != NULL) {
      unsigned titleTag = Struct->Desc()->TagIndex("TITLE");
      title =
        (*static_cast<DStringGDL*> (Struct->GetTag(titleTag, 0)))[0];
    }

    e->AssureStringScalarKWIfPresent(choosenIx, title);
  }

  void gdlSetLineStyle(EnvT *e, GDLGStream *a) {
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
    DLong linestyle =
      (*static_cast<DLongGDL*>
      (pStruct->GetTag(pStruct->Desc()->TagIndex("LINESTYLE"), 0)))[0];

    // if the LINESTYLE keyword is present, the value will be change
    DLong linestyleNew = -1111;
    int linestyleIx = e->KeywordIx("LINESTYLE");

    if (e->KeywordSet(linestyleIx)) e->AssureLongScalarKWIfPresent(linestyleIx, linestyleNew);

    bool debug = false;
    if (debug) {
      cout << "temp_linestyle " << linestyleNew << endl;
      cout << "     linestyle " << linestyle << endl;
    }
    if (linestyleNew != -1111) {
      linestyle = linestyleNew;
    }//+1;
    if (linestyle < 0) {
      linestyle = 0;
    }
    if (linestyle > 5) {
      linestyle = 5;
    }
    gdlLineStyle(a, linestyle);
  }

  DFloat gdlGetPenThickness(EnvT *e, GDLGStream *a) {
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
    DFloat thick = (*static_cast<DFloatGDL*>
      (pStruct->GetTag(pStruct->Desc()->TagIndex("THICK"), 0)))[0];

    int THICKIx = e->KeywordIx("THICK");
    e->AssureFloatScalarKWIfPresent(THICKIx, thick);
    if (thick <= 0.0) thick = 1.0;
    return thick;
  }

  void gdlSetPenThickness(EnvT *e, GDLGStream *a) {
    a->Thick(gdlGetPenThickness(e, a));
  }

  void gdlWriteTitleAndSubtitle(EnvT* e, GDLGStream *a) {
    unsigned titleTag = SysVar::P()->Desc()->TagIndex("TITLE");
    unsigned subTitleTag = SysVar::P()->Desc()->TagIndex("SUBTITLE");
    DString title = (*static_cast<DStringGDL*> (SysVar::P()->GetTag(titleTag, 0)))[0];
    DString subTitle = (*static_cast<DStringGDL*> (SysVar::P()->GetTag(subTitleTag, 0)))[0];

    int TITLEIx = e->KeywordIx("TITLE");
    int SUBTITLEIx = e->KeywordIx("SUBTITLE");
    e->AssureStringScalarKWIfPresent(TITLEIx, title);
    e->AssureStringScalarKWIfPresent(SUBTITLEIx, subTitle);
    if (title.empty() && subTitle.empty()) return;

    gdlSetPlotCharsize(e, a);
    if (!title.empty()) {
      e->AssureStringScalarKWIfPresent(TITLEIx, title);
      gdlSetPlotCharthick(e, a);
      a->sizeChar(1.25 * a->charScale());
      a->mtex("t", 1.5, 0.5, 0.5, title.c_str()); //position is in units of current char height. baseline at half-height
      a->sizeChar(a->charScale() / 1.25);
    }
    if (!subTitle.empty()) {
      e->AssureStringScalarKWIfPresent(SUBTITLEIx, subTitle);
      //      DFloat step=a->mmLineSpacing()/a->mmCharHeight();
      //      a->mtex("b", 5*step, 0.5, 0.5, subTitle.c_str());
      a->mtex("b", 5, 0.5, 0.5, subTitle.c_str()); //position is in units of current char height. baseline at half-height
    }
  }
  //call this function if Y data is strictly >0.
  //set yStart to 0 only if gdlYaxisNoZero is false.

  bool gdlYaxisNoZero(EnvT* e) {
    //no explict range given?
    DDouble test1, test2;
    unsigned rangeTag = SysVar::Y()->Desc()->TagIndex("RANGE");
    test1 = (*static_cast<DDoubleGDL*> (SysVar::Y()->GetTag(rangeTag, 0)))[0];
    test2 = (*static_cast<DDoubleGDL*> (SysVar::Y()->GetTag(rangeTag, 0)))[1];
    if (!(test1 == 0.0 && test2 == 0.0)) return true;
    int YRANGEIx = e->KeywordIx("YRANGE");

    if (e->KeywordPresent(YRANGEIx)) return true;
    //Style contains 1?
    DLong ystyle;
    gdlGetDesiredAxisStyle(e, YAXIS, ystyle);
    if (ystyle & 1) return true;

    DLong nozero = 0;
    if (ystyle & 16) nozero = 1;
    int YNOZEROIx = e->KeywordIx("YNOZERO");
    if (e->KeywordSet(YNOZEROIx)) nozero = 1;
    return (nozero == 1);
  }


  //advance to next plot unless the noerase flag is set

  void gdlNextPlotHandlingNoEraseOption(EnvT *e, GDLGStream *a, bool noe) {
    bool noErase = false;
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 

    if (!noe) {
      DLong LnoErase = (*static_cast<DLongGDL*>
        (pStruct->
        GetTag(pStruct->Desc()->TagIndex("NOERASE"), 0)))[0];
      noErase = (LnoErase == 1);
      int NOERASEIx = e->KeywordIx("NOERASE");

      if (e->KeywordSet(NOERASEIx)) {
        noErase = true;
      }
    } else {
      noErase = true;
    }

    a->NextPlot(!noErase);
    // all but the first element of !P.MULTI are ignored if POSITION kw or !P.POSITION or !P.REGION is specified
    // TODO: !P.REGION!

    DFloatGDL* pos = NULL;
    DFloatGDL* pos1 = NULL;
    DFloatGDL* pos2 = NULL;

    bool hasregion = false;
    bool hasposition = false;
    // system variable !P.REGION first 
    pos1 = static_cast<DFloatGDL*> (pStruct-> GetTag(pStruct->Desc()->TagIndex("REGION"), 0));
    if ((*pos1)[0] != (*pos1)[2]) hasregion = true;

    pos2 = static_cast<DFloatGDL*> (pStruct-> GetTag(pStruct->Desc()->TagIndex("POSITION"), 0));
    if ((*pos2)[0] != (*pos2)[2]) hasposition = true;

    // keyword
    int positionIx = e->KeywordIx("POSITION");
    if (e->GetDefinedKW(positionIx) != NULL) {
      pos = e->GetKWAs<DFloatGDL>(positionIx);
    }
    if (pos != NULL || hasposition || hasregion) a->NoSub();
  }

  //handling of Z bounds is not complete IMHO.

  inline void CheckMargin(GDLGStream* actStream,
    DFloat xMarginL,
    DFloat xMarginR,
    DFloat yMarginB,
    DFloat yMarginT,
    PLFLT& xMR,
    PLFLT& xML,
    PLFLT& yMB,
    PLFLT& yMT) {
    PLFLT sclx = actStream->dCharLength() / actStream->xSubPageSize(); //current char length/subpage size
    xML = xMarginL*sclx; //margin as percentage of subpage
    xMR = xMarginR*sclx;
    PLFLT scly = actStream->dLineSpacing() / actStream->ySubPageSize(); //current char length/subpage size
    yMB = (yMarginB) * scly;
    yMT = (yMarginT) * scly; //to allow subscripts and superscripts (as in IDL)

    if (xML + xMR >= 1.0) {
      PLFLT xMMult = xML + xMR;
      xML /= xMMult * 1.5;
      xMR /= xMMult * 1.5;
    }
    if (yMB + yMT >= 1.0) {
      PLFLT yMMult = yMB + yMT;
      yMB /= yMMult * 1.5;
      yMT /= yMMult * 1.5;
    }
  }

  DDouble gdlSetViewPortAndWorldCoordinates(EnvT* e,
    GDLGStream* actStream,
    DDouble x0,
    DDouble x1,
    bool xLog,
    DDouble y0,
    DDouble y1,
    bool yLog,
    DDouble z0,
    DDouble z1,
    bool zLog,
    DDouble zValue_input, //input
    bool iso) {

    COORDSYS coordinateSystem = DATA;
    DDouble xStart, yStart, xEnd, yEnd, zStart, zEnd;

    //work on local values, taking account loginess
    xStart = x0;
    xEnd = x1;
    yStart = y0;
    yEnd = y1;
    zStart = z0;
    zEnd = z1;

    if (xLog) {
      xStart = log10(xStart);
      xEnd = log10(xEnd);
    }
    if (yLog) {
      yStart = log10(yStart);
      yEnd = log10(yEnd);
    }
    if (zLog) {
      zStart = log10(zStart);
      zEnd = log10(zEnd);
    }

    // MARGIN
    DFloat xMarginL, xMarginR, yMarginB, yMarginT, zMarginB, zMarginT; //, zMarginB, zMarginT;
    gdlGetDesiredAxisMargin(e, XAXIS, xMarginL, xMarginR);
    gdlGetDesiredAxisMargin(e, YAXIS, yMarginB, yMarginT);
    gdlGetDesiredAxisMargin(e, ZAXIS, zMarginB, zMarginT);

    PLFLT sxmin, symin, sxmax, symax, szmin, szmax;
    actStream->getSubpageRegion(sxmin, symin, sxmax, symax, &szmin, &szmax);
    //Special for Z: for Z.S, Z.WINDOW and Z.REGION, in case of POSITION having 6 elements

    DDouble zposStart, zposEnd;
    if (std::isfinite(zValue_input)) {
      zposStart = zValue_input;
      zposEnd = ZVALUEMAX;
    } else {
      zposStart = szmin;
      zposEnd = szmax;
    }

    PLFLT xMR, xML, yMB, yMT, zMB, zMT;
    CheckMargin(actStream,
      xMarginL,
      xMarginR,
      yMarginB,
      yMarginT,
      xMR, xML, yMB, yMT);

    // viewport - POSITION overrides
    static PLFLT aspect = 0.0;

    static PLFLT P_position_normed[4] = {0, 0, 0, 0};
    static PLFLT P_region_normed[4] = {0, 0, 0, 0};
    static PLFLT position[4];
    // Set to default values:

    //compute position removing margins
    position[0] = sxmin + xMarginL * actStream->nCharLength();
    position[1] = symin + yMarginB * actStream->nLineSpacing();
    position[2] = sxmax - xMarginR * actStream->nCharLength();
    position[3] = symax - yMarginT * actStream->nLineSpacing();
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset 
    // Get !P.position values. !P.REGION is superseded by !P.POSITION
    if (pStruct != NULL) {
      unsigned regionTag = pStruct->Desc()->TagIndex("REGION");
      for (SizeT i = 0; i < 4; ++i) P_region_normed[i] = (PLFLT) (*static_cast<DFloatGDL*> (pStruct->GetTag(regionTag, 0)))[i];
      unsigned positionTag = pStruct->Desc()->TagIndex("POSITION");
      for (SizeT i = 0; i < 4; ++i) P_position_normed[i] = (PLFLT) (*static_cast<DFloatGDL*> (pStruct->GetTag(positionTag, 0)))[i];
    }
    if (P_region_normed[0] != P_region_normed[2]) //exist, so it is a first approx to position: 
    {
      //compute position removing margins
      position[0] = P_region_normed[0] + xMarginL * actStream->nCharLength();
      position[1] = P_region_normed[1] + yMarginB * actStream->nLineSpacing();
      position[2] = P_region_normed[2] - xMarginR * actStream->nCharLength();
      position[3] = P_region_normed[3] - yMarginT * actStream->nLineSpacing();
    }
    if (P_position_normed[0] != P_position_normed[2]) //exist, so it is a second approx to position, this one dos not include margins:
    {
      position[0] = P_position_normed[0];
      position[1] = P_position_normed[1];
      position[2] = P_position_normed[2];
      position[3] = P_position_normed[3];
    }
    //compatibility: Position NEVER outside [0,1]:
    position[0] = max(0.0, position[0]);
    position[1] = max(0.0, position[1]);
    position[2] = min(1.0, position[2]);
    position[3] = min(1.0, position[3]);

    //check presence of DATA,DEVICE and NORMAL options
    int DATAIx = e->KeywordIx("DATA");
    int DEVICEIx = e->KeywordIx("DEVICE");
    int NORMALIx = e->KeywordIx("NORMAL");

    if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
    if (e->KeywordSet(DEVICEIx)) coordinateSystem = DEVICE;
    if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;

    // read boxPosition if needed
    int positionIx = e->KeywordIx("POSITION");
    DFloatGDL* boxPosition = e->IfDefGetKWAs<DFloatGDL>(positionIx);
    if (boxPosition != NULL) {
      for (SizeT i = 0; i < 4 && i < boxPosition->N_Elements(); ++i) position[i] = (*boxPosition)[i];
      if (boxPosition->N_Elements() > 4) {
        zposStart = fmin((*boxPosition)[4], ZVALUEMAX);
        zposStart = fmax(zposStart, 0);
        if (boxPosition->N_Elements() > 5) {
          zposEnd = fmin((*boxPosition)[5], ZVALUEMAX);
          zposEnd = fmax(zposEnd, 0);
        }
      }
    }
    if (boxPosition != NULL) { //use passed values
      if (coordinateSystem == DEVICE) {
        // modify position to NORMAL if DEVICE is present
        actStream->DeviceToNormedDevice(position[0], position[1], position[0], position[1]);
        actStream->DeviceToNormedDevice(position[2], position[3], position[2], position[3]);
      }
      //compatibility again: Position NEVER outside [0,1]:
      position[0] = max(0.0, position[0]);
      position[1] = max(0.0, position[1]);
      position[2] = min(1.0, position[2]);
      position[3] = min(1.0, position[3]);
    }

    aspect = 0.0; // vpas with aspect=0.0 equals vpor.
    if (iso) aspect = abs((yEnd - yStart) / (xEnd - xStart)); //log-log or lin-log
    bool ret;
    if (iso) ret = actStream->isovpor(position[0], position[2], position[1], position[3], aspect);
    else ret = actStream->vpor(position[0], position[2], position[1], position[3]);
    if (ret) e->Throw("Data coordinate system not established.");

    actStream->wind(xStart, xEnd, yStart, yEnd);

    //set ![XYZ].CRANGE ![XYZ].type ![XYZ].WINDOW and ![XYZ].S
    gdlStoreXAxisParameters(actStream, xStart, xEnd, xLog); //already in log here if relevant!
    gdlStoreYAxisParameters(actStream, yStart, yEnd, yLog);
    gdlStoreZAxisParameters(actStream, zStart, zEnd, zLog, zposStart, zposEnd);
    //set P.CLIP (done by PLOT, CONTOUR, SHADE_SURF, and SURFACE)
    gdlStoreCLIP();
    return zposStart;
  }


  //gdlAxis will write an axis from Start to End, following all modifier switch, in the good place of the current VPOR, independent of the current WIN,
  // as it is temporarily superseded by setting a new a->win().
  //this makes GDLAXIS independent of WIN, and help the whole code to be dependent only on VPOR which is the sole useful plplot command to really use.
  //ZAXIS will always be an YAXIS plotted with a special YZEXCH T3D matrix. So no special handling of ZAXIS here.

  void gdlAxis(EnvT *e, GDLGStream *a, int axisId, DDouble Start, DDouble End, bool Log, DLong modifierCode) {
    if (Start == End) return;
    if (Log && (Start <= 0 || End <= 0)) return; //important protection 
    DLong AxisStyle;
    gdlGetDesiredAxisStyle(e, axisId, AxisStyle);
    if ((AxisStyle & 4) == 4) return; //if we do not write the axis...

    // we WILL plot something, so set temporarlily WIN accordingly
    PLFLT owxmin, owxmax, owymin, owymax;
    a->plstream::gvpw(owxmin, owxmax, owymin, owymax);
    if (axisId == XAXIS) {
      if (Log) a->wind(log10(Start), log10(End), owymin, owymax);
      else a->wind(Start, End, owymin, owymax);
    } else {
      if (Log) a->wind(owxmin, owxmax, log10(Start), log10(End));
      else a->wind(owxmin, owxmax, Start, End);
    }

    //special values
    DLong GridStyle;
    gdlGetDesiredAxisGridStyle(e, axisId, GridStyle);
    DLong Minor;
    gdlGetDesiredAxisMinor(e, axisId, Minor);
    DFloat Thick;
    gdlGetDesiredAxisThick(e, axisId, Thick);
    DStringGDL* TickFormat;
    gdlGetDesiredAxisTickFormat(e, axisId, TickFormat);
    DDouble TickInterval;
    gdlGetDesiredAxisTickInterval(e, axisId, TickInterval);
    DLong TickLayout;
    gdlGetDesiredAxisTickLayout(e, axisId, TickLayout);
    DFloat TickLen = 0.02;
    gdlGetDesiredAxisTickLen(e, axisId, TickLen);
    DStringGDL* TickName;
    gdlGetDesiredAxisTickName(e, a, axisId, TickName);
    DDoubleGDL *Tickv = NULL;
    bool hasTickv = gdlGetDesiredAxisTickv(e, axisId, Tickv);
    DLong Ticks;
    gdlGetDesiredAxisTicks(e, axisId, Ticks);
    if (Ticks < 1) hasTickv = false;
    DStringGDL* TickUnits;
    gdlGetDesiredAxisTickUnits(e, axisId, TickUnits);
    DString Title;
    gdlGetDesiredAxisTitle(e, axisId, Title);
    bool hasTitle = (Title.size() > 0);

    bool hasTickUnitDefined = (TickUnits->NBytes() > 0);
    int tickUnitArraySize = (hasTickUnitDefined) ? TickUnits->N_Elements() : 0;
    // tickunits stops when a null string is in the list. Check if such:
    if (hasTickUnitDefined) {
      int naxes=0;
      for (auto i=0; i< tickUnitArraySize; ++i) {
        if ( (*TickUnits)[i].size() < 1 ) break;
        naxes++;
      }
      tickUnitArraySize=naxes;
      if (tickUnitArraySize ==0) hasTickUnitDefined=false;
    }
    
    

    //For labels we need ticklen in current character size, for ticks we need it in mm
    DFloat ticklen_in_mm = TickLen;
    bool inverted_ticks=false;
    if (TickLen < 0) {
      ticklen_in_mm *= -1;
      inverted_ticks=true;
      TickLen=-TickLen;
    }
    //ticklen in a percentage of box x or y size, to be expressed in mm 
    if (axisId == XAXIS) ticklen_in_mm = a->mmyPageSize()*(a->boxnYSize()) * ticklen_in_mm;
    else ticklen_in_mm = a->mmxPageSize()*(a->boxnXSize()) * ticklen_in_mm;
    //    if (ticklen_in_mm > 100.) ticklen_in_mm = 0; //PATCH to avoid PS and MEM device problem. Check why gspa() returns silly values. TBC 
    DFloat ticklen_as_norm = (axisId == XAXIS) ? a->mm2ndy(ticklen_in_mm) : a->mm2ndx(ticklen_in_mm); //in normed coord
    //eventually, each succesive X or Y axis is separated from previous by interligne + ticklen in adequate units. 
    DFloat interligne_as_char;
    DFloat interligne_as_norm;
    DFloat typical_char_size_mm = (axisId == XAXIS) ? a->mmCharHeight() : a->mmCharLength();
    interligne_as_char = (axisId == XAXIS) ? a->mmLineSpacing() / typical_char_size_mm : a->mmCharLength() / typical_char_size_mm; //in normed coord
//    interligne_as_norm = (axisId == XAXIS) ? a->nLineSpacing() : a->nCharLength(); //in normed coord
    interligne_as_norm = (axisId == XAXIS) ? a->nLineSpacing() : a->nCharLength(); //in normed coord
    DFloat displacement = (axisId == XAXIS) ? 2*interligne_as_norm: 3.5*interligne_as_norm;

    double nchars[100]; //max number of chars written in label of axis. //100 should be OK otherwise, paf!
    std::string Opt;
    if (TickInterval == 0) {
      if (Ticks <= 0) TickInterval = gdlComputeTickInterval(e, axisId, Start, End, Log);
      else if (Ticks > 1) TickInterval = (End - Start) / Ticks;
      else TickInterval = (End - Start);
    } else { //check that tickinterval does not make more than 59 ticks:
      if (abs((End - Start) / TickInterval) > 59) TickInterval = (End - Start) / 59;
    }
    if (Minor == 0) {// if tickinterval is 0.1,1,10,100..., IDL wants to see all 10 tickmarks.
      DDouble test = log10(abs(TickInterval));
      if ((test - floor(test)) < std::numeric_limits<DDouble>::epsilon()) Minor = 10;
    }
    
    //first write labels only:
    gdlSetAxisCharsize(e, a, axisId);
    gdlSetPlotCharthick(e, a);

    //define all tick and drawing axes related options
    std::string tickOpt;
    std::string additionalAxesTickOpt;
    std::string tickOpt2=TICKS BOTTOM TICKINVERT;
    std::string tickOpt3=BOTTOM;
    //define tick-related (ticklayout) options
    //ticks or grid eventually with style and length:
    if (TickLen < 1e-6) tickOpt = ""; else tickOpt = TICKS; //remove ticks if ticklen=0
    if (inverted_ticks)  tickOpt += TICKINVERT;
    additionalAxesTickOpt = tickOpt; //layout==2 has ticks

    switch (modifierCode) {
    case 2:
      tickOpt += TOP;
      additionalAxesTickOpt += TOP;
      break;
    case 1:
      tickOpt += BOTTOM;
      additionalAxesTickOpt += BOTTOM;
      break;
    case 0:
      if ((AxisStyle & 8) == 8) {
        tickOpt += BOTTOM;
        additionalAxesTickOpt += BOTTOM;
      }
      else {
        tickOpt += BOTTOM TOP;
        additionalAxesTickOpt += BOTTOM; //no top!
      }
    }
    //gridstyle applies here:
    gdlLineStyle(a, GridStyle);
    // ticklayout2 has no log and no subticks
    if (Log) {
      if (TickInterval < 1) { //if log and tickinterval was >1 then we pass in 'linear, no subticks' mode (see issue #1112)
        if (hasTickv) tickOpt += LOG;
        else tickOpt += SUBTICKS LOG;
        Minor = 0;
      } else if (TickInterval < 2) {
        Minor = 1;
      } else if (TickInterval < 2.1) {
        if (!hasTickv) tickOpt += SUBTICKS;
        Minor = 2;
      } else if (TickInterval < 5.1) {
        if (!hasTickv) tickOpt += SUBTICKS;
        Minor = 5;
      } else {
        if (!hasTickv) tickOpt += SUBTICKS;
        Minor = 10;
      }
    } else if (!hasTickv) tickOpt += SUBTICKS;
    // special case
    if (TickLayout == 1) tickOpt="";
    
    Opt =  NOTICKS BOTTOM_NOLINE TOP_NOLINE;
    if (axisId == YAXIS && TickLayout != 2) {
      Opt += YLABEL_HORIZONTAL;
    }
    // the x option is in plplot 5.9.8 but not before. It permits
    // to avoid writing tick marks here (they will be written after)
    // I hope old plplots were clever enough to ignore 'x'
    // if they did not understand 'x'
    if (Log) {
      if (TickInterval <= 1) Opt += LOG;
      //if log and tickinterval was >1 then we pass in 'linear, no subticks' mode (see issue #1112)
    }

    //the reference norm box
    PLFLT refboxxmin, refboxxmax, refboxymin, refboxymax, boxxmin, boxxmax, boxymin, boxymax, xboxxmin, xboxxmax, xboxymin, xboxymax;
    a->plstream::gvpd(refboxxmin, refboxxmax, refboxymin, refboxymax);
    a->plstream::gvpd(boxxmin, boxxmax, boxymin, boxymax);
    a->plstream::gvpw(xboxxmin, xboxxmax, xboxymin, xboxymax);
    // Add the tick displacement if negative tick length, so as to write the labels UNDER the tick (a plplot flaw IMHO)
    if (inverted_ticks) { //the LABELS should be displaced
      displacement += ticklen_as_norm;
      if (axisId == XAXIS) {
        boxymin -= ticklen_as_norm;
        boxymax += ticklen_as_norm;
      } else {
        boxxmin -= ticklen_as_norm;
        boxxmax += ticklen_as_norm;
      }
      a->plstream::vpor(boxxmin, boxxmax, boxymin, boxymax);
      a->plstream::wind(xboxxmin, xboxxmax, xboxymin, xboxymax);
    }

//define minimal TICKDATA structure
      
      static GDL_TICKDATA tickdata;
      //reset all values
      tickdata.a = a;
      tickdata.e = e;
      tickdata.isLog = Log;
      tickdata.axisrange = abs(End - Start);
      tickdata.nchars = 0;
      tickdata.TickFormat = NULL;
      tickdata.nTickFormat = 0;
      tickdata.TickName = NULL;
      tickdata.nTickName = 0;
      tickdata.tickNameCounter = 0;
      //set modifiers
      if (TickFormat->NBytes() > 0) {
        tickdata.TickFormat = TickFormat;
        tickdata.nTickFormat = TickFormat->N_Elements();
      } else if (TickName->NBytes() > 0) {
        tickdata.TickName = TickName;
        tickdata.nTickName = TickName->N_Elements();
      }

      tickdata.what = GDL_NONE;
      tickdata.reset = true;      
      tickdata.counter = 0;

      //Write labels first , using charthick
    if (hasTickUnitDefined) // /TICKUNITS=[several types of axes written below each other]
    {
      tickdata.what = GDL_TICKUNITS;
      tickdata.TickUnits = TickUnits;
      tickdata.nTickUnits = tickUnitArraySize;
      if (TickFormat->NBytes() > 0) tickdata.what = GDL_TICKUNITS_AND_FORMAT;

      defineLabeling(a, axisId, gdlMultiAxisTickFunc, &tickdata);
      Opt += LABELFUNC;
      if (modifierCode == 2) {
        Opt += NUMERIC_UNCONVENTIONAL;
      } else {
        Opt += NUMERIC;
      } //m: write numerical/right above, n: below/left (normal)
      if (!inverted_ticks && TickLayout != 2) displacement+=ticklen_as_norm; //every next axis will be separated by this
      DFloat y_displacement=0;
      for (SizeT i = 0; i < tickdata.nTickUnits; ++i) //loop on TICKUNITS axis
      {
        TickInterval = gdlComputeTickInterval(e, axisId, Start, End, Log, i);
        tickdata.nchars = 0; //set nchars to 0, at the end nchars will be the maximum size.
        if (axisId == XAXIS) {
          a->plstream::vpor(boxxmin, boxxmax, boxymin - i*displacement, boxymax);
          a->plstream::wind(xboxxmin, xboxxmax, xboxymin, xboxymax);
          a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0); //to avoid plplot crashes: do not use tickinterval. or recompute it correctly (no too small!)
        } else {
          a->plstream::vpor(boxxmin - (i*displacement+y_displacement), boxxmax, boxymin, boxymax);
          a->plstream::wind(xboxxmin, xboxxmax, xboxymin, xboxymax);
          a->box("", 0.0, 0.0, Opt.c_str(), TickInterval, Minor); //to avoid plplot crashes: do not use tickinterval. or recompute it correctly (no too small!)
          nchars[i] = tickdata.nchars;
          if (TickLayout != 2) y_displacement+=(nchars[i]*a->nCharLength());
        }
        tickdata.counter++;
      }
      resetLabeling(a, axisId);
    }
    else //normal simple axis
    {
      defineLabeling(a, axisId, gdlAxisTickFunc, &tickdata);
      Opt += LABELFUNC;
      if (modifierCode == 2) Opt += NUMERIC_UNCONVENTIONAL;
      else Opt += NUMERIC;
      if (axisId == XAXIS) a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0);
      else a->box("", 0.0, 0.0, Opt.c_str(), TickInterval, Minor);
      resetLabeling(a, axisId);
    }

    // insure no axes written with ticklayout=1
    if (TickLayout == 1) tickOpt="";
    if (TickLayout == 1) additionalAxesTickOpt="";

    //replay above, with ticks only
    //     write box with ticks, in proper XTHICK, in the reference position
    a->plstream::vpor(refboxxmin, refboxxmax, refboxymin, refboxymax); //reset to initial box
    a->plstream::gvpd(boxxmin, boxxmax, boxymin, boxymax);
    a->plstream::wind(xboxxmin, xboxxmax, xboxymin, xboxymax);
    //      thick for box and ticks.
    a->Thick(Thick);
    a->smaj(ticklen_in_mm, 1.0);
    a->smin(ticklen_in_mm / 2.0, 1.0);
    if (hasTickUnitDefined) //  replay tickunits
    {
      DFloat y_displacement=0;
      for (auto i = 0; i < tickdata.nTickUnits; ++i) //loop on TICKUNITS axis
      {
        TickInterval = gdlComputeTickInterval(e, axisId, Start, End, Log, i);

        tickdata.nchars = 0; //set nchars to 0, at the end nchars will be the maximum size.
        if (axisId == XAXIS) {
          if (i == 1) {
            tickOpt = (TickLayout == 2) ? tickOpt2 : additionalAxesTickOpt;
            if (TickLayout == 2) {
              a->smaj(a->nd2my(displacement), 1.0);
            }
          }          
          a->plstream::vpor(boxxmin, boxxmax, boxymin - i*displacement, boxymax);
          a->plstream::wind(xboxxmin, xboxxmax, xboxymin, xboxymax);
          a->box(tickOpt.c_str(), TickInterval, Minor, "", 0.0, 0); //to avoid plplot crashes: do not use tickinterval. or recompute it correctly (no too small!)
        } else {
          if (i == 1) {
            tickOpt = (TickLayout == 2) ? tickOpt2 : additionalAxesTickOpt;
            if (TickLayout == 2) {
              a->smaj(a->nd2mx(displacement), 1.0);
            }
          }
          a->plstream::vpor(boxxmin - (i*displacement+y_displacement), boxxmax, boxymin, boxymax);
          a->plstream::wind(xboxxmin, xboxxmax, xboxymin, xboxymax);
          a->box("", 0.0, 0.0, tickOpt.c_str(), TickInterval, Minor); //to avoid plplot crashes: do not use tickinterval. or recompute it correctly (no too small!)
          if (TickLayout != 2) y_displacement+=(nchars[i]*a->nCharLength());
        }
        if (TickLayout == 2 && i==0) {
          if (axisId == XAXIS) {
            a->smaj(a->nd2my(displacement), 1.0);
            a->box(tickOpt2.c_str(), TickInterval, Minor, "", 0.0, 0);
          } else {
            a->smaj(a->nd2mx(displacement), 1.0);
            a->box("", 0.0, 0, tickOpt2.c_str(), TickInterval, Minor);
          }
        }
        tickdata.counter++;
      }
    } else {
      //replay normal
      if (axisId == XAXIS) a->box(tickOpt.c_str(), TickInterval, Minor, "", 0.0, 0);
      else a->box("", 0.0, 0, tickOpt.c_str(), TickInterval, Minor);
    }

    a->plstream::gvpd(boxxmin, boxxmax, boxymin, boxymax);
    if (axisId == XAXIS) { //add last displacement
      a->plstream::vpor(boxxmin, boxxmax, boxymin - displacement, boxymax);
      a->plstream::wind(xboxxmin, xboxxmax, xboxymin, xboxymax);
    } else {
      a->plstream::vpor(boxxmin - displacement, boxxmax, boxymin, boxymax);
      a->plstream::wind(xboxxmin, xboxxmax, xboxymin, xboxymax);
    }
  
    if (TickLayout == 2) {
      if (axisId == XAXIS) {
        a->box(tickOpt3.c_str(), TickInterval, Minor, "", 0.0, 0);
      } else {
        a->box("", 0.0, 0, tickOpt3.c_str(), TickInterval, Minor);
      }
    }

    // Write title (position depends on above values)
    if (hasTitle) {
      gdlSetPlotCharthick(e, a);
      PLFLT title_position = 2;
      if (modifierCode == 0 || modifierCode == 1) {
        if (axisId == XAXIS) a->mtex("b", title_position, 0.5, 0.5, Title.c_str());
        else a->mtex("l", title_position, 0.5, 0.5, Title.c_str());
      } else if (modifierCode == 2) {
        if (axisId == XAXIS) a->mtex("t", title_position, 0.5, 0.5, Title.c_str());
        else a->mtex("r", title_position, 0.5, 0.5, Title.c_str());
      }
    }

      //reset gridstyle
    gdlLineStyle(a, 0);
    gdlWriteDesiredAxisTickGet(e, axisId, Log);
    //reset charsize & thick
    a->Thick(1.0);
    a->sizeChar(1.0);
    a->vpor(refboxxmin, refboxxmax, refboxymin, refboxymax);
    a->wind(owxmin, owxmax, owymin, owymax); //restore old values 
  }

  void gdlBox(EnvT *e, GDLGStream *a, DDouble xStart, DDouble xEnd, bool xLog, DDouble yStart, DDouble yEnd, bool yLog) {
    gdlWriteTitleAndSubtitle(e, a);
    gdlAxis(e, a, XAXIS, xStart, xEnd, xLog);
    gdlAxis(e, a, YAXIS, yStart, yEnd, yLog);
  }

  void gdlBox3(EnvT *e, GDLGStream *a, DDouble xStart, DDouble xEnd, bool xLog, DDouble yStart, DDouble yEnd, bool yLog, DDouble zStart, DDouble zEnd, bool zLog, DDouble zValue) {
    gdlWriteTitleAndSubtitle(e, a);
    gdlAxis(e, a, XAXIS, xStart, xEnd, xLog, 1); //only Bottom
    gdlAxis(e, a, YAXIS, yStart, yEnd, yLog, 1); //only left

    PLFLT xnormmin, xnormmax, ynormmin, ynormmax, znormmin = 0, znormmax = 1;
    a->getCurrentNormBox(xnormmin, xnormmax, ynormmin, ynormmax);
    gdlGetCurrentAxisWindow(ZAXIS, znormmin, znormmax);

    //almost cut and paste of plotting_axis section on Z axis:
    PLFLT yPos = ynormmax;
    PLFLT xPos = xnormmin;
    PLFLT viewportXSize = xnormmax - xPos;
    if (viewportXSize < 0.001) viewportXSize = 0.01;
    a->vpor(xPos, xPos + viewportXSize, znormmin, znormmax);
    //special transform to use 'y' axis code, but with 'z' values and yz exch.
    gdlSetZto3DDriverTransform(a, yPos);
    gdlExchange3DDriverTransform(a);
    gdlAxis(e, a, ZAXIS, zStart, zEnd, zLog, 1);

    //restore (normally not useful?)
    gdlExchange3DDriverTransform(a);
    gdlSetZto3DDriverTransform(a, zValue);
    a->vpor(xnormmin, xnormmax, ynormmin, ynormmax);
    a->wind(xStart, xEnd, xLog, yStart, yEnd, yLog); //Y
  }

  //restore current clipbox, make another or remove it at all.

  bool gdlSwitchToClippedNormalizedCoordinates(EnvT *e, GDLGStream *actStream, bool invertedClipMeaning, bool commandHasCoordSys) {

    COORDSYS coordinateSystem = DATA;
    //check presence of DATA,DEVICE and NORMAL options only of command accept them (otherwise assert triggered if in debug mode)
    if (commandHasCoordSys) {
      int DATAIx = e->KeywordIx("DATA");
      int DEVICEIx = e->KeywordIx("DEVICE");
      int NORMALIx = e->KeywordIx("NORMAL");
      coordinateSystem = DATA;
      //check presence of DATA,DEVICE and NORMAL options
      if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
      if (e->KeywordSet(DEVICEIx)) coordinateSystem = DEVICE;
      if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;
    }

    //CLIPPING (or not) is just defining the adequate viewport and world coordinates, all of them normalized since this is what plplot will get in the end.
    //CLIPPING is in NORMAL case triggered by the existence of CLIP keyword, and is not set by GDL if 3D is in use, as IDL does weird things and we cannot clip correctly
    // with the current version of plplot.
    bool doClip, noclip;
    int NOCLIPIx = e->KeywordIx("NOCLIP");
    if (invertedClipMeaning) {
      noclip = e->BooleanKeywordAbsentOrSet(NOCLIPIx);
    } else {
      noclip = e->BooleanKeywordSet(NOCLIPIx);
    }
    doClip = (!noclip);

    if (doClip) { //we use current norm box, then clipping if smaller:
      PLFLT xnormmin, xnormmax, ynormmin, ynormmax;
      actStream->getCurrentNormBox(xnormmin, xnormmax, ynormmin, ynormmax);
      int CLIP = e->KeywordIx("CLIP"); //this one may be in other coordinates
      DDoubleGDL* clipBoxGDL = e->IfDefGetKWAs<DDoubleGDL>(CLIP);
      if (clipBoxGDL != NULL) {
        //if clipBoxGDL is fully qualified, both its coordsys and values replace clipBoxInMemory
        if (clipBoxGDL->N_Elements() >= 4) {
          for (auto i = 0; i < MIN(clipBoxGDL->N_Elements(), 4); ++i) clipBoxInMemory[i] = (*clipBoxGDL)[i];
          coordinateSystemInMemory = coordinateSystem;
        } else {
          //passed clipbox not fully qualified:
          //if same coordsys, replace as many as possible
          if (coordinateSystem == coordinateSystemInMemory) {
            for (auto i = 0; i < MIN(clipBoxGDL->N_Elements(), 4); ++i) clipBoxInMemory[i] = (*clipBoxGDL)[i];
          } else { //not possible
            return true; //abort
          }
        }
        //test validity of clipbox, we are with the same coordinate system:
        if (clipBoxInMemory[0] >= clipBoxInMemory[2] || clipBoxInMemory[1] >= clipBoxInMemory[3]) return true; //abort
        //clipBox is defined accordingly to /NORM /DEVICE /DATA:
        //convert clipBox to normalized coordinates, avoiding overwriting clipBoxInMemory of course:
        PLFLT clipBox[4];
        for (auto i = 0; i < 4; ++i) clipBox[i] = clipBoxInMemory[i];
        ConvertToNormXY(1, &clipBox[0], false, &clipBox[1], false, coordinateSystem);
        ConvertToNormXY(1, &clipBox[2], false, &clipBox[3], false, coordinateSystem);
        xnormmin = MAX(xnormmin, clipBox[0]);
        xnormmax = MIN(xnormmax, clipBox[2]);
        ynormmin = MAX(ynormmin, clipBox[1]);
        ynormmax = MIN(ynormmax, clipBox[3]);
      }
      bool ret = actStream->vpor(xnormmin, xnormmax, ynormmin, ynormmax);
      if (ret) e->Throw("Data coordinate system not established.");
      actStream->wind(xnormmin, xnormmax, ynormmin, ynormmax); //transformed (plotted) coords will be in NORM. Conversion will be made on the data values.
    } else {
      //make the whole area writeable, but keep world to norm correspondance
      PLFLT xnormmin = 0;
      PLFLT xnormmax = 1;
      PLFLT ynormmin = 0;
      PLFLT ynormmax = 1;
      DDouble *sx, *sy;
      GetSFromPlotStructs(&sx, &sy);
      PLFLT wx1 = (xnormmin - sx[0]) / sx[1];
      PLFLT wx2 = (xnormmax - sx[0]) / sx[1];
      PLFLT wy1 = (ynormmin - sy[0]) / sy[1];
      PLFLT wy2 = (ynormmax - sy[0]) / sy[1];
      bool ret = actStream->vpor(xnormmin, xnormmax, ynormmin, ynormmax);
      if (ret) e->Throw("Data coordinate system not established.");
      actStream->wind(xnormmin, xnormmax, ynormmin, ynormmax);
    }
    return false;
  }

  //just test if clip values (!P.CLIP , CLIP=) are OK (accounting for all NOCLIP etc possibilities!)

  bool gdlTestClipValidity(EnvT *e, GDLGStream *actStream, bool invertedClipMeaning, bool commandHasCoordSys) {
    COORDSYS coordinateSystem = DATA;
    //check presence of DATA,DEVICE and NORMAL options only of command accept them (otherwise assert triggered if in debug mode)
    if (commandHasCoordSys) {
      int DATAIx = e->KeywordIx("DATA");
      int DEVICEIx = e->KeywordIx("DEVICE");
      int NORMALIx = e->KeywordIx("NORMAL");
      coordinateSystem = DATA;
      //check presence of DATA,DEVICE and NORMAL options
      if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
      if (e->KeywordSet(DEVICEIx)) coordinateSystem = DEVICE;
      if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;
    }

    //CLIPPING (or not) is just defining the adequate viewport and world coordinates, all of them normalized since this is what plplot will get in the end.
    //CLIPPING is in NORMAL case triggered by the existence of CLIP keyword, and is not set by GDL if 3D is in use, as IDL does weird things and we cannot clip correctly
    // with the current version of plplot.
    bool doClip, noclip;
    int NOCLIPIx = e->KeywordIx("NOCLIP");
    if (invertedClipMeaning) {
      noclip = e->BooleanKeywordAbsentOrSet(NOCLIPIx);
    } else {
      noclip = e->BooleanKeywordSet(NOCLIPIx);
    }
    doClip = (!noclip);

    if (doClip) { //we use current norm box, then clipping if smaller:
      PLFLT xnormmin, xnormmax, ynormmin, ynormmax;
      actStream->getCurrentNormBox(xnormmin, xnormmax, ynormmin, ynormmax);
      int CLIP = e->KeywordIx("CLIP"); //this one may be in other coordinates
      DDoubleGDL* clipBoxGDL = e->IfDefGetKWAs<DDoubleGDL>(CLIP);
      if (clipBoxGDL != NULL) {
        //if clipBoxGDL is fully qualified, both its coordsys and values replace clipBoxInMemory
        if (clipBoxGDL->N_Elements() >= 4) {
          for (auto i = 0; i < MIN(clipBoxGDL->N_Elements(), 4); ++i) clipBoxInMemory[i] = (*clipBoxGDL)[i];
          coordinateSystemInMemory = coordinateSystem;
        } else {
          //passed clipbox not fully qualified:
          //if same coordsys, replace as many as possible
          if (coordinateSystem == coordinateSystemInMemory) {
            for (auto i = 0; i < MIN(clipBoxGDL->N_Elements(), 4); ++i) clipBoxInMemory[i] = (*clipBoxGDL)[i];
          } else { //not possible
            return true; //abort
          }
        }
        //test validity of clipbox, we are with the same coordinate system:
        if (clipBoxInMemory[0] >= clipBoxInMemory[2] || clipBoxInMemory[1] >= clipBoxInMemory[3]) return true; //abort
        //clipBox is defined accordingly to /NORM /DEVICE /DATA:
        //convert clipBox to normalized coordinates, avoiding overwriting clipBoxInMemory of course:
        PLFLT clipBox[4];
        for (auto i = 0; i < 4; ++i) clipBox[i] = clipBoxInMemory[i];
        ConvertToNormXY(1, &clipBox[0], false, &clipBox[1], false, coordinateSystem);
        ConvertToNormXY(1, &clipBox[2], false, &clipBox[3], false, coordinateSystem);
        xnormmin = MAX(xnormmin, clipBox[0]);
        xnormmax = MIN(xnormmax, clipBox[2]);
        ynormmin = MAX(ynormmin, clipBox[1]);
        ynormmax = MIN(ynormmax, clipBox[3]);
      }
      if (xnormmin >= xnormmax || ynormmin >= ynormmax) return true;
    }
    return false; //should be OK
  }
} // namespace
