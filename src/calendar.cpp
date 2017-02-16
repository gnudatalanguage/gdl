/***************************************************************************
                          calendar.cpp  -  basic calendar transformations
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by G. Duvert
    email                : gilles.duvert@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* The intent of this file is to unify all calendar-related pieces of code used in various
 * places of GDL. */

#include "includefirst.hpp"
#include "calendar.hpp"
#include "gdlexception.hpp"
using namespace std;

  double Gregorian2Julian(struct tm *ts)
  {
    DDouble jd;
    DLong day=ts->tm_mday;
    DLong mon=ts->tm_mon+1;
    DLong year=ts->tm_year+1900;
    DLong hour=ts->tm_hour;
    DLong min=ts->tm_min;
    DDouble sec=ts->tm_sec;
    if (!dateToJD(jd, day, mon, year, hour, min, sec))
        throw GDLException("Invalid Julian date input.");
    return jd;
  }
  bool dateToJD(DDouble &jd, DLong day, DLong month, DLong year, DLong hour, DLong minute, DDouble second) {
    if (year < -4716 || year > 5000000 || year == 0) return false;

    // the following tests seem to be NOT active in IDL. We choose to mimic it.
    //    if (month < 1 || month > 12) return false;
    //    if (day < 0 || day > 31) return false;
    //these one too...
    // if (hour < 0 || hour > 24) return false;
    // if (minute < 0 || minute > 60) return false;
    // if (second < 0 || second > 60) return false;

    DDouble a, y, b;
    DLong m;
    y = (year > 0) ? year : year + 1; //we use here a calendar with no year 0 (not astronomical)
    m = month;
    b = 0.0;
    if (month <= 2) {
      y = y - 1.0;
      m = m + 12;
    }
    if (y >= 0) {
      if (year > 1582 || (year == 1582 && (month > 10 ||
        (month == 10 && day > 14)))) {
        a = floor(y / 100.0);
        b = 2.0 - a + floor(a / 4.0);
      } else if (year == 1582 && month == 10 && day >= 5 && day <= 14) {
        jd = 2299161; //date does not move
        return true;
      }
    }
    jd = floor(365.25 * y) + floor(30.6001 * (m + 1)) + day + (hour * 1.0) / 24.0 + (minute * 1.0) / 1440.0 +
      (second * 1.0) / 86400.0 + 1720994.50 + b;
    return true;
  }
// C code ****************************************************
    bool j2ymdhms(DDouble jd, DLong &iMonth, DLong &iDay , DLong &iYear ,
                  DLong &iHour , DLong &iMinute, DDouble &Second, DLong &dow, DLong &icap)
    {
    DDouble JD,Z,F;
    DLong A,B,C,D,E;
    JD = jd + 0.5;
    Z = floor(JD);
    if (Z < -1095 || Z > 1827933925 ) return FALSE;
    F = JD - Z;
    // note that IDL dow is false before Sun dec 31 12:00:00 -4714, (type: a=[-2,-1]& PRINT, FORMAT='(C())',a)
    // ...and ... we are not!
    if ((DLong)Z > 0) dow = ((DLong)Z) % 7; else dow = ((DLong)Z+1099) % 7; //just translate axis...
    
    if (Z < 2299161) A = (DLong)Z;
    else {
      DDouble a;
      a = (DLong) ((Z - 1867216.25) / 36524.25);
      A = (DLong) (Z + 1 + a - (DLong)(a / 4));
    }

    B = A + 1524;
    C = (DLong) ((B - 122.1) / 365.25);
    D = (DLong) (365.25 * C);
    E = (DLong) ((B - D) / 30.6001);

    // month
    iMonth = E < 14 ? E - 1 : E - 13;
    iMonth--; //to get a zero-based index;  
    // iday
    iDay=B - D - (DLong)(30.6001 * E);

    // year
//    iYear = iMonth > 2 ? C - 4716 : C - 4715;
    iYear = iMonth > 1 ? C - 4716 : C - 4715; //with a zero-based index
    if (iYear < 1) iYear--; //No Year Zero 
    // hours
    iHour = (DLong) (F * 24);
    { //this prevents interpreting 04:00:00 as 03:59:60 !
      //this kind of rounding up is explained in IDL doc.
      DDouble FF=F+6E-10;
      DLong test= (DLong) (FF * 24);
      if (test > iHour) {iHour=test;F=FF;}
    }
    
    icap = (iHour > 11);

    F -= (DDouble)iHour / 24;
    // minutes
    iMinute = (DLong) (F * 1440);
    { //this prevents interpreting 04:00:00 as 03:59:60 !
      //this kind of rounding up is explained in IDL doc.
      DDouble FF=F+6E-10;
      DLong test= (DLong) (FF * 1440);
      if (test > iMinute) {iMinute=test;F=FF;}
    }
    F -= (DDouble)iMinute / (DDouble)1440;
    // seconds
    Second = F * 86400; 
    return TRUE;
  }

  
