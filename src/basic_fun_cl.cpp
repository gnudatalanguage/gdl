/***************************************************************************

                          basic_fun.cpp  -  basic GDL library function

                             -------------------

    begin                : March 14 2004

    copyright            : (C) 2004 by Christopher Lee

    email                : leec_gdl@publius.co.uk

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
// default: assume we have netCDF
#define USE_NETCDF 1
// default: assume we have ImageMagick
#define USE_MAGICK 1
#endif

#include "includefirst.hpp"

#include <string>
#include <fstream>
#include <memory>

#include <stdio.h>
#include <time.h>
#include <cmath>
#include <regex>

#include <gsl/gsl_sf.h>

#include "initsysvar.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
#include "basic_fun_cl.hpp"

#define MAX_DATE_STRING_LENGTH 80

#ifdef _MSC_VER
#include "gtdhelper.hpp"
#else
#include <sys/time.h>
#endif

namespace lib {

  using namespace std;
  using namespace antlr;

  void timestamptovalues(EnvT* e) {

    //    try {

      DStringGDL* timestamps;

      timestamps = e->GetParAs<DStringGDL>(0);

      SizeT time_size = timestamps->N_Elements();

      long dim = timestamps->Dim(0);

      long valueYEAR = e->KeywordIx("YEAR");
      bool hasYear = e->KeywordPresent(valueYEAR);

      long valueMONTH = e->KeywordIx("MONTH");
      bool hasMonth = e->KeywordPresent(valueMONTH);

      long valueDAY = e->KeywordIx("DAY");
      bool hasDay = e->KeywordPresent(valueDAY);

      long valueHOUR = e->KeywordIx("HOUR");
      bool hasHour = e->KeywordPresent(valueHOUR);

      long valueMINUTE = e->KeywordIx("MINUTE");
      bool hasMinute = e->KeywordPresent(valueMINUTE);

      long valueSECOND = e->KeywordIx("SECOND");
      bool hasSecond = e->KeywordPresent(valueSECOND);

      long valueOFFSET = e->KeywordIx("OFFSET");
      bool hasOffset = e->KeywordPresent(valueOFFSET);

      if (dim >= 1) {

        for(int i = 0 ; i<time_size ; i++) {
	  if ((*timestamps)[i].length() < 10) e->Throw("Time string is too short.");
	}

        unsigned long year[time_size];
        DLongGDL* year_gdl = new DLongGDL(*(new dimension(time_size)));

        unsigned long month[time_size];
        DLongGDL* month_gdl = new DLongGDL(*(new dimension(time_size)));

        unsigned long day[time_size];
        DLongGDL* day_gdl = new DLongGDL(*(new dimension(time_size)));

        unsigned long hour[time_size];
        DLongGDL* hour_gdl = new DLongGDL(*(new dimension(time_size)));

        unsigned long minute[time_size];
        DLongGDL* minute_gdl = new DLongGDL(*(new dimension(time_size)));

        double second[time_size];
        DDoubleGDL* second_gdl = new DDoubleGDL(*(new dimension(time_size)));

        double offset[time_size];
        DDoubleGDL* offset_gdl = new DDoubleGDL(*(new dimension(time_size)));

        for(int i = 0 ; i<time_size ; i++) {
          string timestamp = (*timestamps)[i];

          if (hasYear) {
            year[i] = stoi(timestamp.substr(0,4));
            (*year_gdl)[i] = year[i];
          }

          if (hasMonth) {
            month[i] = stoi(timestamp.substr(5,2));
            (*month_gdl)[i] = month[i];
          }

          if (hasDay) {
            day[i] = stoi(timestamp.substr(8,2));
            (*day_gdl)[i] = day[i];
          }

          if (hasHour) {
            if (timestamp.length() > 10) {
              hour[i] = stoi(timestamp.substr(11,2));
            } else {
              hour[i] = 0;
            }
            (*hour_gdl)[i] = hour[i];
          }

          if (hasMinute) {
            if (timestamp.length() > 10) {
              minute[i] = stoi(timestamp.substr(14,2));
            } else {
              minute[i] = 0;
            }
            (*minute_gdl)[i] = minute[i];
          }

          if (hasSecond) {
            if (timestamp.length() > 10) {
              second[i] = stoi(timestamp.substr(17,2));
              if (timestamp.substr(19,1) == ".") {
                if (timestamp.substr(timestamp.length()-1,1) == "Z") {
                  second[i] += stod(timestamp.substr(20, timestamp.length()-21))/pow(10,(timestamp.length()-21));
                } else {
                  second[i] += stod(timestamp.substr(20, timestamp.length()-26))/pow(10,(timestamp.length()-26));
                }
              }
            } else {
              second[i] = 0;
            }
            (*second_gdl)[i] = second[i];
          }

          if (hasOffset) {
            if (timestamp.length() > 10) {
              string offset_string;
              if (timestamp.substr(timestamp.length()-1,1) == "Z") {
                offset_string = "+00:00";
              } else {
                offset_string = timestamp.substr(timestamp.length()-6,7);
              }
              offset[i] = stod(offset_string.substr(0,3)) + stod(offset_string.substr(4,2))/60;
            } else {
            offset[i] = 0;
            }
            (*offset_gdl)[i] = offset[i];
          }
        }

        if(hasYear) {
          e->SetKW(valueYEAR, year_gdl);
        }

        if(hasMonth) {
          e->SetKW(valueMONTH, month_gdl);
        }

        if(hasDay) {
          e->SetKW(valueDAY, day_gdl);
        }

        if(hasHour) {
          e->SetKW(valueHOUR, hour_gdl);
        }

        if(hasMinute) {
          e->SetKW(valueMINUTE, minute_gdl);
        }

        if(hasSecond) {
          e->SetKW(valueSECOND, second_gdl);
        }

        if(hasOffset) {
          e->SetKW(valueOFFSET, offset_gdl);
        }
      } else {

        string timestamp = (*e->GetParAs<DStringGDL>(0))[0];
	if (timestamp.length() < 10) e->Throw("Time string is too short.");

        unsigned long year, month, day, hour, minute;
        double second, offset;

        if (hasYear) {
          year = stoi(timestamp.substr(0,4));
          DLongGDL* year_gdl = new DLongGDL(year);
          e->SetKW(valueYEAR, year_gdl);
        }

        if (hasMonth) {
          month = stoi(timestamp.substr(5,2));
          DLongGDL* month_gdl = new DLongGDL(month);
          e->SetKW(valueMONTH, month_gdl);
        }

        if (hasDay) {
          day = stoi(timestamp.substr(8,2));
          DLongGDL* day_gdl = new DLongGDL(day);
          e->SetKW(valueDAY, day_gdl);
        }

        if (hasHour) {
          if (timestamp.length() > 10) {
            hour = stoi(timestamp.substr(11,2));
          } else {
            hour = 0;
          }
          DLongGDL* hour_gdl = new DLongGDL(hour);
          e->SetKW(valueHOUR, hour_gdl);
        }

        if (hasMinute) {
          if (timestamp.length() > 10) {
            minute = stoi(timestamp.substr(14,2));
          } else {
            minute = 0;
          }
          DLongGDL* minute_gdl = new DLongGDL(minute);
          e->SetKW(valueMINUTE, minute_gdl);
        }

        if (hasSecond) {
          if (timestamp.length() > 10) {
            second = stoi(timestamp.substr(17,2));
            if (timestamp.substr(19,1) == ".") {
              if (timestamp.substr(timestamp.length()-1,1) == "Z") {
                second += stod(timestamp.substr(20, timestamp.length()-21))/pow(10,(timestamp.length()-21));
              } else {
                second += stod(timestamp.substr(20, timestamp.length()-26))/pow(10,(timestamp.length()-26));
              }
            }
          } else {
            second = 0;
          }
          DDoubleGDL* second_gdl = new DDoubleGDL(second);
          e->SetKW(valueSECOND, second_gdl);
        }

        if (hasOffset) {
          if (timestamp.length() > 10) {
            string offset_string;
            if (timestamp.substr(timestamp.length()-1,1) == "Z") {
              offset_string = "+00:00";
            } else {
              offset_string = timestamp.substr(timestamp.length()-6,7);
            }
            offset = stod(offset_string.substr(0,3)) + stod(offset_string.substr(4,2))/60;
          } else {
          offset = 0;
          }
          DDoubleGDL* offset_gdl = new DDoubleGDL(offset);
          e->SetKW(valueOFFSET, offset_gdl);
        }
      }
      //} catch(...) {
     // e->Throw("Input is invalid.");
    //}
  }

  // Timestamp implemented by Eloi R. de Linage in June of 2021
  BaseGDL* timestamp(EnvT* e)
  {
    static int zeroIx=e->KeywordIx("ZERO");
    bool isZero=e->KeywordSet(zeroIx);
    //return timestamp will all values set to 0
    if(isZero) return new DStringGDL("0000-00-00T00:00:00Z");

    static int utcIx=e->KeywordIx("UTC");
    bool isUTC=e->BooleanKeywordAbsentOrSet(utcIx);

    struct timeval tv;

    /* Get the current time at high precision; could also use clock_gettime() for
     * even higher precision times if we want it. */
    gettimeofday(&tv, NULL);

    // convert to time to 'struct tm' for use with strftime
    time_t time_sec=tv.tv_sec;
    struct tm *tm_local=localtime(&time_sec);
    struct tm *tm_utc=gmtime(&time_sec);

    if(isUTC){
      tm_local=tm_utc;
    }

    string values_str[]={"YEAR","MONTH","DAY","HOUR","MINUTE","SECOND","OFFSET"};
    int values_min[7]={1000,1,1,0,0,0,-14};
    int values_max[7]={9999,12,31,23,59,59,12};

    vector<vector<float>> values_vec(7);
    values_vec.reserve(7);

    bool isParScalar[7];
    bool isValue[7];

    bool isAnyValue=false;
    bool noOffset=true;

    DStringGDL* res = new DStringGDL("");

    int minListNelem = -1;

    for(int i=0; i < 7; ++i){
      int valueIx = e->KeywordIx(values_str[i]);
      isValue[i]=e->KeywordSet(valueIx);

      if(isValue[i]){
        if(i==6){
          noOffset=false;
        } else {
          isAnyValue=true;
        }

        DFloatGDL * par = e->GetKWAs<DFloatGDL>(valueIx);
        isParScalar[i] = par->Rank() == 0;

        if(!isParScalar[i])
        {
          if(minListNelem == -1) minListNelem = par->N_Elements();
          if(par->N_Elements() <= minListNelem) res = new DStringGDL(par->Dim());
        }

        for(int j=0; j < par->N_Elements(); ++j)
        {
          values_vec[i].push_back((*par)[j]);

          if(values_vec[i][j] < values_min[i] || values_vec[i][j] > values_max[i]){
            //the value is out of its range, throw error
            stringstream ss_min, ss_max;
            ss_min << values_min[i];
            ss_max << values_max[i];
            e->Throw(values_str[i]+" must contain numbers between "+ss_min.str()+" and "+ss_max.str()+".");
          }
        }
      } else {
        values_vec[i].push_back(0);
        isParScalar[i] = true;
      }
    }

    for(int i=0; i<res->N_Elements(); ++i)
    {
      string ts; //result string

      if(isAnyValue){
        //feed input values in time struct tm_local (default 0 if not set)

        if(isValue[0]) tm_local->tm_year = values_vec[0][(!isParScalar[0]) * i]-1900;
        if(isValue[1]) tm_local->tm_mon = values_vec[1][(!isParScalar[1]) * i]-1;
        if(isValue[2]) tm_local->tm_mday = values_vec[2][(!isParScalar[2]) * i];
        tm_local->tm_hour = values_vec[3][(!isParScalar[3]) * i];
        tm_local->tm_min = values_vec[4][(!isParScalar[4]) * i];
        tm_local->tm_sec = values_vec[5][(!isParScalar[5]) * i];

        char timestamp[] = "YYYY-MM-ddTHH:mm:ss";
        //format the time
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", tm_local);
        ts=timestamp;
      } else {
        char timestamp[] = "YYYY-MM-ddTHH:mm:ss.000";
        //format the time
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S.000", tm_local);
        //puttinf in milliseconds
        char milli[50]; //size of 50 to avoid buffer overload (not optimal)
        sprintf(milli, "%03ld", (long) tv.tv_usec/1000);
        sprintf(timestamp + 20, "%.3s", milli);
        ts=timestamp;
      }

      float diff = 0;
      if(noOffset){
        //compute difference in hours of local time to UTC time
        diff = (float) tm_local->tm_hour-tm_utc->tm_hour+(tm_local->tm_min-tm_utc->tm_min)/60.0;
      } else {
        //if hour offset is specified
        diff = values_vec[6][(!isParScalar[6]) * i];
      }
      long int h_diff, m_diff;
      h_diff=(long int) diff;
      m_diff=round(abs(60*(diff-h_diff)));

      if(isUTC){
          ts+="Z";
      } else if(h_diff > 0) {
          char diff[50];  //size of 50 to avoid buffer overload (not optimal)
          sprintf(diff,"%s%02ld:%02ld", "+", h_diff, m_diff);
          ts += diff;
      } else if(h_diff < 0) {
          char diff[50];  //size of 50 to avoid buffer overload (not optimal)
          sprintf(diff,"%s%02ld:%02ld", "-", -h_diff, m_diff);
          ts += diff;
      } else {
          ts+="Z";
      }

      (*res)[i] = ts;
    }

    return res;
  }

  BaseGDL* systime(EnvT* e)
  {
    struct timeval tval;
    struct timezone tzone;
    
    /*get the time before doing anything else,
      this hopefully gives a more meaningful "time"
      than if the t=time(0) call came after an
      arbitary number of conditional statements.*/
    gettimeofday(&tval,&tzone);
    double tt = tval.tv_sec+tval.tv_usec/1e+6; // time in UTC seconds

    SizeT nParam=e->NParam(0); //,"SYSTIME");
    bool ret_seconds=false;

    Guard<BaseGDL> v_guard;
    Guard<BaseGDL> v1_guard;
    DLong v=0;
    if (nParam == 1) {
      //1 parameter,
      //      1->current UTC time seconds
      //      default
      //DLong v=0;
      e->AssureLongScalarPar(0,v);

      //    DIntGDL* v = static_cast<DIntGDL*>(e->GetParDefined(0)->Convert2(GDL_INT,BaseGDL::COPY));
      //v_guard.Reset( v); //  e->Guard(v);
      //if ( (*v)[0] == 1) //->EqualNoDelete( static_cDIntGDL(1)))

      // AC 15/05/14 : in fact, the range goes in ]-1,1[
      if ((v <= -1) || (v >= 1)) ret_seconds=true;

    }

    static double elapsedIx=e->KeywordIx("ELAPSED");
    //bool iselapsed=e->KeywordSet("ELAPSED");
    bool iselapsed=e->KeywordPresentAndDefined(elapsedIx);
    static int julianIx=e->KeywordIx("JULIAN");
    bool isjulian=e->KeywordSet(julianIx);
    static int secondsIx=e->KeywordIx("SECONDS");
    bool isseconds=e->KeywordSet(secondsIx);
    static int utcIx=e->KeywordIx("UTC");
    bool isutc=e->KeywordSet(utcIx);
    
    
    if (nParam == 2) {
      if (isjulian) e->Throw("Conflicting keywords.");//I don't think it's necessory.
      //2 parameters
      //if the first param is 0, return the date of the second arg
      //if the first param is 1, return the 'double' of the second arg
      DLong v1=0;
      e->AssureLongScalarPar(0,v1);
      //     DIntGDL* v1 = static_cast<DIntGDL*>(e->GetParDefined(0)->Convert2(GDL_INT,BaseGDL::COPY));

      if (iselapsed==0){//if is elapsed, v2 is ignored.
      DDouble v2=0.0;
      e->AssureDoubleScalarPar(1,v2);
      //v_guard.Reset( v1); //  e->Guard(v1);
      //DDoubleGDL* v2 = static_cast<DDoubleGDL*>(e->GetParDefined(1)->Convert2(GDL_DOUBLE,BaseGDL::COPY));
      if (v1 == 0) { //v1->EqualNoDelete( DIntGDL(0))) { //0, read the second argument as time_t;
          tval.tv_sec = static_cast<long int>(v2);
          tval.tv_usec = static_cast<long int>((v2-tval.tv_sec)*1e+6);
        //delete v2; // we delete v2 here as it is not guarded. Avoids a "new" in the following "else"
      } else { //1
        return new DDoubleGDL(v2);
      }
      }
      else{
        if ((v1 <= -1) || (v1 >= 1)) ret_seconds=true;
      }
      
    }
    
    //
    if (iselapsed) {
      DDouble elapsed;
      e->AssureDoubleScalarKW(elapsedIx, elapsed);
      tt = elapsed;
      // In case the elapsed = NaN or Inf;
      int inputnan = isnan(elapsed);
      int inputinf = isinf(elapsed);
      if (inputnan == 1|| inputinf == 1){
        if (isjulian==1){
          return new DDoubleGDL(static_cast<double>(tt));
          }else{
            if ((ret_seconds|| isseconds )){
              return new DDoubleGDL(static_cast<double>(tt));
              } 
            else { elapsed=0; tt=elapsed;isutc=true;}
            }
        }
      tval.tv_sec = static_cast<long int>(elapsed);
      tval.tv_usec = static_cast<long int>((elapsed-tval.tv_sec)*1e+6);
    }

    //return the variable in seconds, either JULIAN, JULIAN+UTC,
    //or no other keywords
    struct tm *tstruct;
    //return a string of the time, either UTC or local (default)

    time_t ttime = tval.tv_sec;
    
    if(isutc){
      tstruct= gmtime(&ttime);
      }
      else{
      tstruct= localtime(&ttime);
      }
    //Convert the time to JULIAN or NOT
    if(isjulian){
        DDouble t_nsec = (tt-tval.tv_sec)*1e+9;
        return new DDoubleGDL(Gregorian2Julian(tstruct,t_nsec));}
    else if( ret_seconds || isseconds )
        return new DDoubleGDL(static_cast<double>(tt));
    else{
       char st[MAX_DATE_STRING_LENGTH];
//        char *st=new char[MAX_DATE_STRING_LENGTH];
//        ArrayGuard<char> stGuard( st);
       const char *format="%a %h %e %T %Y";//my IDL date format.
       DStringGDL *S;
    
#ifdef _WIN32
  char *at = asctime(tstruct); // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/asctime-wasctime
  at[strlen(at) - 1] = 0;      // On Windows, asctime appends a newline at the end of string.
	S = new DStringGDL(at);
#else
       SizeT res=strftime(st,MAX_DATE_STRING_LENGTH,format,tstruct);

       if(res != 0)
         S=new DStringGDL(st);
       else
         S=new DStringGDL("");
#endif

       return S;
      }
  }

  BaseGDL* legendre(EnvT* e)
  {
    SizeT nParam=e->NParam(2); //, "LEGENDRE");

    Guard<BaseGDL> x_guard;
    Guard<BaseGDL> l_guard;
    Guard<BaseGDL> m_guard;

    Guard<BaseGDL> guard;
    int count;

    BaseGDL* xvals,* lvals,* mvals;

    //    xvals= e->GetParDefined(0); //,"LEGENDRE");
    xvals= e->GetNumericParDefined(0);

    SizeT nEx, nEl, nEm, nmin;
    //    nEl=0;
    //nEm=0;

    nEx=xvals->N_Elements();
    if(nEx == 0) e->Throw( "Variable is undefined: "+e->GetParString(0));

    //    lvals=e->GetParDefined(1); //,"LEGENDRE");
    lvals= e->GetNumericParDefined(1);

    nEl=lvals->N_Elements();
    if(nEl == 0) e->Throw("Variable is undefined: "+e->GetParString(1));

    if(nParam > 2)
      {
	mvals= e->GetNumericParDefined(2);
	//mvals=e->GetParDefined(2); //,"LEGENDRE");
	nEm=mvals->N_Elements();
      } else {
	mvals=new DIntGDL(0);
	nEm=1;
	guard.Reset(mvals);
      }

    if(nEm == 0) e->Throw("Variable is undefined: "+e->GetParString(2));

    nmin=nEx;
    if(nEl < nmin && nEl > 1) 	nmin=nEl;
    if(nEm < nmin && nEm > 1) 	nmin=nEm;

    if(xvals->Type() == GDL_COMPLEX ||
       xvals->Type() == GDL_COMPLEXDBL) {
      e->Throw("Complex Legendre not implemented: ");
    }        else      {
      //byte, int, long float, double, uint, ulong, int64, uint64 (AC and string too ;)

      DDoubleGDL* res;
      DDoubleGDL* x_cast;
      DIntGDL* l_cast,*m_cast;

      if(xvals->Type() == GDL_DOUBLE)
	x_cast=  static_cast<DDoubleGDL*>(xvals);
      else
	{
	x_cast=  static_cast<DDoubleGDL*>(xvals->Convert2(GDL_DOUBLE,BaseGDL::COPY));
	x_guard.Reset(x_cast);//e->Guard( x_cast);
	}

      // lvals check
      if(lvals->Type() == GDL_COMPLEX ||
	 lvals->Type() == GDL_COMPLEXDBL)
	e->Throw(
		 "Complex Legendre not implemented: ");
      else if(lvals->Type() == GDL_INT)
	l_cast=static_cast<DIntGDL*>(lvals);
      else
	{
	  l_cast=static_cast<DIntGDL*>(lvals->Convert2(GDL_INT,BaseGDL::COPY));
	  l_guard.Reset(l_cast);//e->Guard( l_cast);
	}

      //mval check
      if(mvals->Type() == GDL_COMPLEX ||
	 mvals->Type() == GDL_COMPLEXDBL)
	e->Throw(
		 "Complex Legendre not implemented: ");
      else if(mvals->Type() == GDL_INT)
	m_cast=static_cast<DIntGDL*>(mvals);
      else
	{
	  m_cast=static_cast<DIntGDL*>(mvals->Convert2(GDL_INT,BaseGDL::COPY));
	  //e->Guard( m_cast);
	  m_guard.Reset(m_cast);
	}

      //x,m,l are converted to the correct format (double, int, int) here


      //make the result array have the same size as the smallest x,m,l array
      if(nmin == nEx) res=new DDoubleGDL(xvals->Dim(),BaseGDL::NOZERO);
      else if(nmin == nEl) res=new DDoubleGDL(lvals->Dim(),BaseGDL::NOZERO);
      else if(nmin == nEm) res=new DDoubleGDL(mvals->Dim(),BaseGDL::NOZERO);

      for (count=0;count<nmin;count++)
	{
	  DDouble xNow = (*x_cast)[nmin > nEx?0:count];
	  DInt lNow =    (*l_cast)[nmin > nEl?0:count];
	  DInt mNow =    (*m_cast)[nmin > nEm?0:count];

	  if( xNow < -1.0 || xNow > 1.0)
	    e->Throw( "Argument X must be in the range [-1.0, 1.0]");
	  if( lNow < 0)
	    e->Throw( "Argument L must be greater than or equal to zero.");
	  if( mNow < -lNow || mNow > lNow)
	    e->Throw( "Argument M must be in the range [-L, L].");

	  if( mNow >= 0)
	    (*res)[count]=
	      gsl_sf_legendre_Plm( lNow, mNow, xNow);
	  else
	    {
	      mNow = -mNow;

	      int addIx  = lNow+mNow;
	      DDouble mul = 1.0;
	      DDouble dD  = static_cast<DDouble>( lNow-mNow+1);
	      for( int d=lNow-mNow+1; d<=addIx; ++d)
		{
		  mul *= dD;
		  dD  += 1.0;
		}

	      DDouble Pm = gsl_sf_legendre_Plm( lNow, mNow, xNow);
	      if( mNow % 2 == 1) Pm = -Pm;

	      (*res)[count] = Pm / mul;
	    }
	}

      //convert things back
      static int doubleIx=e->KeywordIx("DOUBLE");
      if(xvals->Type() != GDL_DOUBLE && !e->KeywordSet(doubleIx))
	{
	  return res->Convert2(GDL_FLOAT,BaseGDL::CONVERT);
	}
      else
	{
	  return res;
	}
    }
    return new DByteGDL(0);
  }

  // Gamma, LnGamma, IGamma and Beta are now in math_fun_gm.cpp
  // I rewrite them because they had many bugs (gregory.marchal_at_obspm.fr)

  BaseGDL* gsl_exp(EnvT* e)
  {
    Guard<BaseGDL> cdr_guard;
    Guard<BaseGDL> cd_guard;
    Guard<BaseGDL> d_guard;
    Guard<BaseGDL> fr_guard;

    SizeT nParam = e->NParam(1);
    BaseGDL* v=e->GetNumericParDefined(0);

    size_t nEl = v->N_Elements();
    size_t i;

      //      DDoubleGDL* d;
    DDoubleGDL* dr = new DDoubleGDL(v->Dim(), BaseGDL::NOZERO);
      //      e->Guard( dr);

    if(v->Type() == GDL_COMPLEX) {
      DComplexDblGDL* cd=
	static_cast<DComplexDblGDL*>(v->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
      cd_guard.Reset(cd);//e->Guard( cd);

	DComplexDblGDL* cdr =
	  new DComplexDblGDL(v->Dim(), BaseGDL::NOZERO);
	cdr_guard.Reset(cdr);//e->Guard( cdr);

	if(nEl == 1)
	  (*cdr)[0]=
	   DComplex((gsl_sf_exp((*cd)[0].real())*cos((*cd)[0].imag())),
		    (gsl_sf_exp((*cd)[0].real())*sin((*cd)[0].imag())));
	else
	  for(i=0;i<nEl;++i)
	    (*cdr)[i]=
	      DComplex((gsl_sf_exp((*cd)[i].real())*cos((*cd)[i].imag())),
		       (gsl_sf_exp((*cd)[i].real())*sin((*cd)[i].imag())));

	return static_cast<DComplexGDL*>(cdr->Convert2(GDL_COMPLEX,BaseGDL::COPY));

      } else if(v->Type() == GDL_COMPLEXDBL) {
	DComplexDblGDL* cd=
	  static_cast<DComplexDblGDL*>(v->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
	cd_guard.Reset(cd);//e->Guard( cd);

	DComplexDblGDL* cdr =
	  new DComplexDblGDL(v->Dim(), BaseGDL::NOZERO);

	if(nEl == 1)
	  (*cdr)[0]=
	   DComplex((gsl_sf_exp((*cd)[0].real())*cos((*cd)[0].imag())),
		    (gsl_sf_exp((*cd)[0].real())*sin((*cd)[0].imag())));
	else
	  for(i=0;i<nEl;i++)
	    (*cdr)[i]=
	      DComplex((gsl_sf_exp((*cd)[i].real())*cos((*cd)[i].imag())),
		       (gsl_sf_exp((*cd)[i].real())*sin((*cd)[i].imag())));

	return cdr;

      } else if(v->Type() == GDL_DOUBLE) {

	DDoubleGDL* d=static_cast<DDoubleGDL*>(v->Convert2(GDL_DOUBLE,
							   BaseGDL::COPY));
	d_guard.Reset(d);//e->Guard( d);
	if(nEl == 1)
	  (*dr)[0]=gsl_sf_exp((*d)[0]);
	else
	  for (i=0;i<nEl;++i) (*dr)[i]=gsl_sf_exp((*d)[i]);

	return dr;
    } else { // all remainding types converted to Float ...
      /*if(v->Type() == GDL_FLOAT ||
		v->Type() == GDL_INT ||
		v->Type() == GDL_LONG) {
      */
	DFloatGDL *fr=new DFloatGDL(v->Dim(), BaseGDL::NOZERO);
	fr_guard.Reset(fr);//e->Guard( fr);

	DDoubleGDL* d=static_cast<DDoubleGDL*>(v->Convert2(GDL_DOUBLE,
							   BaseGDL::COPY));
	d_guard.Reset(d);//e->Guard( d);

	if(nEl == 1)
	  (*dr)[0]=gsl_sf_exp((*d)[0]);
	else
	  for (i=0;i<nEl;++i) (*dr)[i]=gsl_sf_exp((*d)[i]);

	return static_cast<DFloatGDL*>(dr->Convert2(GDL_FLOAT,BaseGDL::COPY));
      }


    assert(false);
    return NULL;
  }
} // namespace
