/***************************************************************************
                          ifmt.cpp  -  formatted input
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
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

// to be included from datatypes.cpp
#ifdef INCLUDE_IFMT_CPP

#include <iostream>
#include <sstream>

//#include "datatypes.hpp"
//#include "dstructgdl.hpp"
//#include "io.hpp"
#include "real2int.hpp"

using namespace std;

// on input all format codes are the similar
// only the default width is important
// (dec, hex, oct are also supported)

void ReadNext( istream& is, string& buf)
{
  bool trail = false;
  char c;
  for(;;)
    {
      c = is.get();

      //      int cc = c;
      //      cout << "c: " << cc << " ("<<c<<")"<<endl;
      
      if( is.eof())  return;
      if( c == '\n') return;
      if( trail && (c == ' ' || c == '\t'))
	{
	  is.unget();
	  return;
	}

      buf.push_back( c);
      trail = true;
    }
}

// read double
inline double ReadD( istream* is, int w)
{
  if( w > 0)
    {
      char *buf = new char[ w+1];
      ArrayGuard<char> guard( buf);
      is->get( buf, w+1); 
      return Str2D( buf);
   }
  else if( w == 0)
    {
      string buf;
      ReadNext( *is, buf);
      return Str2D( buf.c_str());
    }
  else
    {
      string buf;
      getline( *is, buf);
      return Str2D( buf.c_str());
    }
}
// read long
inline long int ReadL( istream* is, int w, int base=10)
{
  if( w > 0)
    {
      char *buf = new char[ w+1];
      ArrayGuard<char> guard( buf);
      is->get( buf, w+1); 
      return Str2L( buf, base);
   }
  else if( w == 0)
    {
      string buf;
      ReadNext( *is, buf);
      return Str2L( buf.c_str(), base);
    }
  else
    {
      string buf;
      getline( *is, buf);
      return Str2L( buf.c_str(), base);
    }
}
// read unsigned long
inline unsigned long int ReadUL( istream* is, int w, int base=10)
{
  if( w > 0)
    {
      char *buf = new char[ w+1];
      ArrayGuard<char> guard( buf);
      is->get( buf, w+1); 
      return Str2UL( buf, base);
   }
  else if( w == 0)
    {
      string buf;
      ReadNext( *is, buf);
      return Str2UL( buf.c_str(), base);
    }
  else
    {
      string buf;
      getline( *is, buf);
      return Str2UL( buf.c_str(), base);
    }
}

// A code
// string
template<> SizeT Data_<SpDString>::
IFmtA( istream* is, SizeT offs, SizeT r, int w) 
{
  if( w < 0) w = 0;

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

   if( w <= 0)
    for( SizeT i=offs; i<endEl; i++)
      getline( *is, (*this)[ i]);
  else
    {
      char *buf = new char[ ++w];
      ArrayGuard<char> guard( buf);
      for( SizeT i=offs; i<endEl; i++)
	{
	  is->get( buf, w);
	  (*this)[ i] = buf; //string( buf);
	  if( is->eof()) // ignore if length is too short
	    {
	      if( i == (endEl-1))
		is->clear();

	      assert( is->good());
	    }
	}
    }

  return tCount;
}
// int (long, ...)
template<> SizeT Data_<SpDInt>::
IFmtA( istream* is, SizeT offs, SizeT r, int w)
{
  if( w < 0) w = 0;

  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    (*this)[ i] = ReadL( is, w);
  
  return tCount;
}
template<> SizeT Data_<SpDLong>::
IFmtA( istream* is, SizeT offs, SizeT r, int w)
{
  if( w < 0) w = 0;

  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    (*this)[ i] = ReadL( is, w);
  
  return tCount;
}
template<> SizeT Data_<SpDLong64>::
IFmtA( istream* is, SizeT offs, SizeT r, int w)
{
  if( w < 0) w = 0;

  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    (*this)[ i] = ReadL( is, w);
  
  return tCount;
}
// unsigned int (unsigned long, ...)
template<class Sp> SizeT Data_<Sp>::
IFmtA( istream* is, SizeT offs, SizeT r, int w)
{
  if( w < 0) w = 0;

  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    (*this)[ i] = ReadUL( is, w);
  
  return tCount;
}

// float (double, ...)
template<> SizeT Data_<SpDFloat>::
IFmtA( istream* is, SizeT offs, SizeT r, int w)
{
  if( w < 0) w = 0;

  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    (*this)[ i] = ReadD( is, w);
  
  return tCount;
}
template<> SizeT Data_<SpDDouble>::
IFmtA( istream* is, SizeT offs, SizeT r, int w)
{
  if( w < 0) w = 0;

  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    (*this)[ i] = ReadD( is, w);
  
  return tCount;
}

// complex, complexdbl (replace float with double)
template<> SizeT Data_<SpDComplex>::
IFmtA( istream* is, SizeT offs, SizeT r, int w) 
{
  float re, im;

  if( w < 0) w = 0;

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountIn = tCount;

  SizeT firstEl = offs / 2;


  if( offs & 0x01)
    {
    im = ReadD( is, w);
    (*this)[ firstEl] = Ty( (*this)[ firstEl].real(), im);
    firstEl++;
    tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  for( SizeT i= firstEl; i<endEl; i++)
    {
    re = ReadD( is, w);
    im = ReadD( is, w);
    (*this)[ i] = Ty( re, im);
    }

  if( tCount & 0x01)
    {
    re = ReadD( is, w);
    (*this)[ endEl] = Ty( re, (*this)[ endEl].imag());
    }

  return tCountIn;
}
template<> SizeT Data_<SpDComplexDbl>::
IFmtA( istream* is, SizeT offs, SizeT r, int w) 
{
  double re, im;

  if( w < 0) w = 0;

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountIn = tCount;

  SizeT firstEl = offs / 2;


  if( offs & 0x01)
    {
    im = ReadD( is, w);
    (*this)[ firstEl] = Ty( (*this)[ firstEl].real(), im);
    firstEl++;
    tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  for( SizeT i= firstEl; i<endEl; i++)
    {
    re = ReadD( is, w);
    im = ReadD( is, w);
    (*this)[ i] = Ty( re, im);
    }

  if( tCount & 0x01)
    {
    re = ReadD( is, w);
    (*this)[ endEl] = Ty( re, (*this)[ endEl].imag());
    }

  return tCountIn;
}

// I O Z formats
// string
template<> SizeT Data_<SpDString>::
IFmtI( istream* is, SizeT offs, SizeT r, int w,
       BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    {
      (*this)[ i] = i2s(ReadL( is, w, oMode),8);
    }
  
  return tCount;
}
// int (long, ...)
template<class Sp> SizeT Data_<Sp>::
IFmtI( istream* is, SizeT offs, SizeT r, int w,
       BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    {
      (*this)[ i] = ReadL( is, w, oMode);
    }
  
  return tCount;
}
// unsigned long (unsigned int can be handled by default)
template<> SizeT Data_<SpDULong>::
IFmtI( istream* is, SizeT offs, SizeT r, int w,
       BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    {
      (*this)[ i] = ReadUL( is, w, oMode);
    }
  
  return tCount;
}
// complex, complexdbl (replace float with double)
template<> SizeT Data_<SpDComplex>::
IFmtI( istream* is, SizeT offs, SizeT r, int w, 
       BaseGDL::IOMode oMode) 
{
  float re, im;

  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;

  if( r < tCount) tCount = r;
  SizeT tCountIn = tCount;
  
  SizeT firstEl = offs / 2;

  if( offs & 0x01)
    {
      im = ReadL( is, w, oMode);
      (*this)[ firstEl] = Ty( (*this)[ firstEl].real(), im);
      firstEl++;
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  for( SizeT i= firstEl; i<endEl; i++)
    {
      re = ReadL( is, w, oMode);
      im = ReadL( is, w, oMode);
      (*this)[ i] = Ty( re, im);
    }

  if( tCount & 0x01)
    {
      re = ReadL( is, w, oMode);
      (*this)[ endEl] = Ty( re, (*this)[ endEl].imag());
    }
  
  return tCountIn;
}
template<> SizeT Data_<SpDComplexDbl>::
IFmtI( istream* is, SizeT offs, SizeT r, int w, 
       BaseGDL::IOMode oMode) 
{
  double re, im;

  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountIn = tCount;
  
  SizeT firstEl = offs / 2;

  if( offs & 0x01)
    {
      im = ReadL( is, w, oMode);
      (*this)[ firstEl] = Ty( (*this)[ firstEl].real(), im);
      firstEl++;
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  for( SizeT i= firstEl; i<endEl; i++)
    {
      re = ReadL( is, w, oMode);
      im = ReadL( is, w, oMode);
      (*this)[ i] = Ty( re, im);
    }

  if( tCount & 0x01)
    {
      re = ReadL( is, w, oMode);
      (*this)[ endEl] = Ty( re, (*this)[ endEl].imag());
    }
  
  return tCountIn;
}

// F code
// string
template<> SizeT Data_<SpDString>::
IFmtF( istream* is, SizeT offs, SizeT r, int w)
{
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    {
      (*this)[ i] = i2s(ReadD( is, w),8);
    }
  
  return tCount;
}
// int
template<class Sp> SizeT Data_<Sp>::
IFmtF( istream* is, SizeT offs, SizeT r, int w)
{
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    {
      (*this)[ i] = Real2Int<Ty,double>(ReadD( is, w));
    }
  
  return tCount;
}
// float
template<> SizeT Data_<SpDFloat>::
IFmtF( istream* is, SizeT offs, SizeT r, int w)
{
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    {
      (*this)[ i] = ReadD( is, w);
    }
  
  return tCount;
}
// double
template<> SizeT Data_<SpDDouble>::
IFmtF( istream* is, SizeT offs, SizeT r, int w)
{
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  
  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; i++)
    {
      (*this)[ i] = ReadD( is, w);
    }
  
  return tCount;
}
// complex, complexdbl (replace float with double)
template<> SizeT Data_<SpDComplex>::
IFmtF( istream* is, SizeT offs, SizeT r, int w) 
{
  float re, im;

  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountIn = tCount;
  
  SizeT firstEl = offs / 2;
  
  if( offs & 0x01)
    {
      im = ReadD( is, w);
      (*this)[ firstEl] = Ty( (*this)[ firstEl].real(), im);
      firstEl++;
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  for( SizeT i= firstEl; i<endEl; i++)
    {
      re = ReadD( is, w);
      im = ReadD( is, w);
      (*this)[ i] = Ty( re, im);
    }

  if( tCount & 0x01)
    {
      re = ReadD( is, w);
      (*this)[ endEl] = Ty( re, (*this)[ endEl].imag());
    }
  
  return tCountIn;
}
template<> SizeT Data_<SpDComplexDbl>::
IFmtF( istream* is, SizeT offs, SizeT r, int w) 
{
  double re, im;

  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountIn = tCount;
  
  SizeT firstEl = offs / 2;
  
  if( offs & 0x01)
    {
      im = ReadD( is, w);
      (*this)[ firstEl] = Ty( (*this)[ firstEl].real(), im);
      firstEl++;
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  for( SizeT i= firstEl; i<endEl; i++)
    {
      re = ReadD( is, w);
      im = ReadD( is, w);
      (*this)[ i] = Ty( re, im);
    }

  if( tCount & 0x01)
    {
      re = ReadD( is, w);
      (*this)[ endEl] = Ty( re, (*this)[ endEl].imag());
    }
  
  return tCountIn;
}

// struct ***************************************************
// struct just calls the appropriate functions on its tags
// common code for all struct output functions
void DStructGDL::IFmtAll( SizeT offs, SizeT r,
			  SizeT& firstIn, SizeT& firstOffs,
			  SizeT& tCount, SizeT& tCountIn)
{
  SizeT nTrans = ToTransfer();

  // transfer count
  tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  tCountIn = tCount;

  // find first Element
  SizeT oneElTr = nTrans / N_Elements();

  SizeT firstEl = offs / oneElTr;
  firstOffs =  offs % oneElTr;

  // find first tag
  SizeT nB = 0;
  SizeT nTags=NTags();
  SizeT firstTag = 0;
  for( firstTag=0; firstTag < nTags; firstTag++)
      {
	SizeT tt=GetTag( firstTag)->ToTransfer();
	nB += tt;
	if( nB > firstOffs)
	  {
	    nB -= tt;
	    break;
	  }
      }

  firstIn = firstEl * NTags() + firstTag;
  firstOffs -= nB;
}
SizeT DStructGDL::
IFmtA( istream* is, SizeT offs, SizeT r, int w) 
{
  SizeT firstIn, firstOffs, tCount, tCountIn;
  IFmtAll( offs, r, firstIn, firstOffs, tCount, tCountIn);

  SizeT trans = (*this)[ firstIn]->IFmtA( is, firstOffs, tCount, w);
  if( trans >= tCount) return tCountIn;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstIn+1); i < ddSize; i++)
    {
      trans = (*this)[ i]->IFmtA( is, 0, tCount, w);
      if( trans >= tCount) return tCountIn;
      tCount -= trans;
    }

  return tCountIn;
}
SizeT DStructGDL::
IFmtI( istream* is, SizeT offs, SizeT r, int w,  
       BaseGDL::IOMode oMode) 
{
  SizeT firstIn, firstOffs, tCount, tCountIn;
  IFmtAll( offs, r, firstIn, firstOffs, tCount, tCountIn);

  SizeT trans = (*this)[ firstIn]->IFmtI( is, firstOffs, tCount, w, oMode);
  if( trans >= tCount) return tCountIn;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstIn+1); i < ddSize; i++)
    {
      trans = (*this)[ i]->IFmtI( is, 0, tCount, w, oMode);
      if( trans >= tCount) return tCountIn;
      tCount -= trans;
    }
  
  return tCountIn;
}
SizeT DStructGDL::
IFmtF( istream* is, SizeT offs, SizeT r, int w) 
{
  SizeT firstIn, firstOffs, tCount, tCountIn;
  IFmtAll( offs, r, firstIn, firstOffs, tCount, tCountIn);

  SizeT trans = (*this)[ firstIn]->IFmtF( is, firstOffs, tCount, w);
  if( trans >= tCount) return tCountIn;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstIn+1); i < ddSize; i++)
    {
      trans = (*this)[ i]->IFmtF( is, 0, tCount, w);
      if( trans >= tCount) return tCountIn;
      tCount -= trans;
    }

  return tCountIn;
}
int getPosInStringArray(string *array, int nval, string what)
{
  for (int i=0; i<nval; i++){
    if (what.compare(0,what.length(),array[i],0,what.length())==0) return i+1;
  }
  return -1;
}
    bool convertDateToJD(DDouble &jd, DLong &day, DLong &month, DLong &year, DLong &hour, DLong &minute, DDouble &second)
    {
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
      if (month <= 2)
	{
	  y = y - 1.0;
	  m = m + 12;
	}
      if (y >= 0) {
	if (year > 1582 || (year == 1582 && (month > 10 ||
					     (month == 10 && day > 14)))) {
	  a = floor(y / 100.0);
	  b = 2.0 - a + floor(a / 4.0);
	}
	else if (year == 1582 && month == 10 && day >= 5 && day <= 14) {
	  jd = 2299161; //date does not move 
	  return true;
	}
      }
      jd = floor(365.25*y) + floor(30.6001*(m + 1)) + day + (hour*1.0) / 24.0 + (minute*1.0) / 1440.0 +
	(second*1.0) / 86400.0 + 1720994.50 + b;
      return true;
    }

 static string theMonth[12]={"January","February","March","April","May","June",
    "July","August","September","October","November","December"};
 static string theMONTH[12]={"JANUARY","FEBRUARY","MARCH","APRIL","MAY","JUNE",
    "JULY","AUGUST","SEPTEMBER","OCTOBER","NOVEMBER","DECEMBER"};
 static string themonth[12]={"january","february","march","april","may","june",
    "july","august","september","october","november","december"};
 static string theDAY[7]={"MONDAY","TUESDAY","WEDNESDAY","THURSDAY","FRIDAY","SATURDAY","SUNDAY"};
 static string theDay[7]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
 static string theday[7]={"monday","tuesday","wednesday","thursday","friday","saturday","sunday"};
 static string capa[2]={"am","pm"};
 static string cApa[2]={"Am","Pm"};
 static string cAPa[2]={"AM","PM"};
   
 DDouble ReadFmtCal( string what, int w, BaseGDL::Cal_IOMode cMode)
 {
  static DLong iMonth=1;
  static DLong iDay=1;
  static DLong iYear=1;
  static DLong iHour=12;
  static DLong iMinute=0;
  static DLong icap=0;
  static DDouble Second=0.0;
  static DDouble Value;
  string fmt;
  what=StrUpCase(what);
  switch ( cMode ) {
    case BaseGDL::COMPUTE:
      if (iHour < 13 && icap == 2) {//sole case icap plays a role
        iHour += 12;
      }
      convertDateToJD(Value, iDay, iMonth, iYear, iHour, iMinute, Second);
//      cerr<<"Value="<<Value<<" iDay="<<iDay<<" iMonth="<<iMonth<<" iYear="<<iYear<<" iHour"<<iHour<<" iMinute"<<iMinute<<" Second"<<Second<<endl;
      iMonth=1; iDay=1; iYear=1; iHour=12; iMinute=0; Second=0.0; icap=0; //IDL default...
      break;
    case BaseGDL::DEFAULT:
      //IDL complains when ifDow is wrong... is it really useful?
      iMonth=getPosInStringArray(theMONTH, 12, what.substr(4,3));
      if (iMonth == -1) throw GDLException("Unable to apply format code "+fmt+" to input: "+what.substr(4,3)+".");
      iDay=Str2L( what.substr(8,2).c_str(), 10);
      if (iDay < 0 || iDay > 31) throw GDLException("Unable to apply format code "+fmt+" to input: "+what.substr(8,2)+".");
      iHour=Str2L( what.substr(11,2).c_str(), 10);
      if (iHour < 0 || iHour > 23) throw GDLException("Unable to apply format code "+fmt+" to input: "+what.substr(11,2)+".");
      iMinute=Str2L( what.substr(14,2).c_str(), 10);
      if (iMinute < 0 || iMinute > 59) throw GDLException("Unable to apply format code "+fmt+" to input: "+what.substr(14,2)+".");
      Second=Str2L( what.substr(17,2).c_str(), 10);
      if (Second < 0 || Second > 59.0) throw GDLException("Unable to apply format code "+fmt+" to input: "+what.substr(17,2)+".");
      iYear=Str2L( what.substr(20,4).c_str(), 10);
      if (iYear < -4716 || iYear > 5000000 || iYear == 0) throw GDLException("Value of Julian date is out of allowed range.");
      break;
    case BaseGDL::CMOA:
      fmt="CMOA";
      iMonth=getPosInStringArray(theMONTH, 12, what);
      if (iMonth == -1) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
    case BaseGDL::CMoA:
      fmt="CMoA";
      iMonth=getPosInStringArray(theMONTH, 12, what);
      if (iMonth == -1) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
    case BaseGDL::CmoA:
      fmt="CmoA";
      iMonth=getPosInStringArray(theMONTH, 12, what);
      if (iMonth == -1) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
    case BaseGDL::CDWA:
    case BaseGDL::CDwA:
    case BaseGDL::CdwA:
      //do nothing -- day of week is not useful.
      break;
    case BaseGDL::CapA:
      fmt="CapA";
      //icap should modify date. FIXME.
      icap=getPosInStringArray(cAPa, 2, what);
      if (icap == -1) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
    case BaseGDL::CApA:
      fmt="CApA";
      //icap should modify date. FIXME.
      icap=getPosInStringArray(cAPa, 2, what);
      if (icap == -1) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
    case BaseGDL::CAPA:
      fmt="CAPA";
      //icap should modify date. FIXME.
      icap=getPosInStringArray(cAPa, 2, what);
      if (icap == -1) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
    case BaseGDL::CMOI:
      fmt="CMOI";
      iMonth=Str2L( what.c_str(), 10);
      if (iMonth < 1 || iMonth > 12) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
    case BaseGDL::CYI:
      fmt="CYI";
      iYear=Str2L( what.c_str(), 10);
      if (iYear < -4716 || iYear > 5000000 || iYear == 0) throw GDLException("Value of Julian date is out of allowed range.");
      break;
    case BaseGDL::ChI:
      fmt="ChI";
      //AM-PM hour -- to be modified by CaPA.
      iHour=Str2L( what.c_str(), 10);
      if (iHour < 0 || iHour > 12) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
    case BaseGDL::CHI:
      fmt="CHI";
      iHour=Str2L( what.c_str(), 10);
      if (iHour < 0 || iHour > 23) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
    case BaseGDL::CDI:
     fmt="CDI";
      iDay=Str2L( what.c_str(), 10);
      if (iDay < 0 || iDay > 31) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
    case BaseGDL::CMI:
     fmt="CMI";
      iMinute=Str2L( what.c_str(), 10);
      if (iMinute < 0 || iMinute > 59) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
    case BaseGDL::CSI:
      fmt="CSI";
      Second=Str2L( what.c_str(), 10);
      if (Second < 0 || Second > 59.0) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
       break;
      //Float
    case BaseGDL::CSF:
      fmt="CSF";
      Second=Str2L( what.c_str(), 10);
      if (Second < 0 || Second > 60.0) throw GDLException("Unable to apply format code "+fmt+" to input: "+what+".");
      break;
  }
  return Value;
 }

 string IFmtGetString(istream* is, int w)
 {
   string what;
  if( w > 0)
    {
      char *buf = new char[ w+1];
      ArrayGuard<char> guard( buf);
      is->get( buf, w+1);
      what.assign(buf);
   }
  else if( w == 0)
    {
      ReadNext( *is, what);
    }
  else
    {
      getline( *is, what);
    }
   return what;
 }
 
template<> SizeT Data_<SpDString>::
IFmtCal( istream* is, SizeT offs, SizeT r, int w, BaseGDL::Cal_IOMode cMode)
{
//  ***NOT COMPLETE: Repeat count will mostly *NOT* Work ! FIXME (see ofmt.cpp solution!)
  DDouble val = ReadFmtCal( IFmtGetString(is, w), w, cMode);
  std::ostringstream s;
  s << setw( 16 ) << std::scientific<< val;
  (*this)[ offs] = s.str();
  return 1;
}
template<> SizeT Data_<SpDComplex>::
IFmtCal( istream* is, SizeT offs, SizeT r, int w, BaseGDL::Cal_IOMode cMode)
{
//  ***NOT COMPLETE: Repeat count will mostly *NOT* Work ! FIXME (see ofmt.cpp solution!)
  DFloat re, im;
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountIn = tCount;
  
  SizeT firstEl = offs / 2;
  
  if( offs & 0x01)
    {
      im = ReadFmtCal( IFmtGetString(is, w), w, cMode);
      (*this)[ firstEl] = Ty( (*this)[ firstEl].real(), im);
      firstEl++;
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  for( SizeT i= firstEl; i<endEl; i++)
    {
      re = ReadFmtCal( IFmtGetString(is, w), w, cMode);
      im = ReadFmtCal( IFmtGetString(is, w), w, cMode);
      (*this)[ i] = Ty( re, im);
    }

  if( tCount & 0x01)
    {
      re = ReadFmtCal( IFmtGetString(is, w), w, cMode);
      (*this)[ endEl] = Ty( re, (*this)[ endEl].imag());
    }
  
  return tCountIn;
}

template<> SizeT Data_<SpDComplexDbl>::
IFmtCal( istream* is, SizeT offs, SizeT r, int w, BaseGDL::Cal_IOMode cMode)
{
//  ***NOT COMPLETE: Repeat count will mostly *NOT* Work ! FIXME (see ofmt.cpp solution!)
  DDouble re, im;
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountIn = tCount;
  
  SizeT firstEl = offs / 2;
  
  if( offs & 0x01)
    {
      im = ReadFmtCal( IFmtGetString(is, w), w, cMode);
      (*this)[ firstEl] = Ty( (*this)[ firstEl].real(), im);
      firstEl++;
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  for( SizeT i= firstEl; i<endEl; i++)
    {
      re = ReadFmtCal( IFmtGetString(is, w), w, cMode);
      im = ReadFmtCal( IFmtGetString(is, w), w, cMode);
      (*this)[ i] = Ty( re, im);
    }

  if( tCount & 0x01)
    {
      re = ReadFmtCal( IFmtGetString(is, w), w, cMode);
      (*this)[ endEl] = Ty( re, (*this)[ endEl].imag());
    }
  
  return tCountIn;
}
template<class Sp> SizeT Data_<Sp>::
IFmtCal( istream* is, SizeT offs, SizeT r, int w, BaseGDL::Cal_IOMode cMode)
{
//  ***NOT COMPLETE: Repeat count will mostly *NOT* Work ! FIXME (see ofmt.cpp solution!)

  (*this)[ offs] = ReadFmtCal( IFmtGetString(is, w), w, cMode);
  return 1;
}
SizeT DStructGDL::
IFmtCal( istream* is, SizeT offs, SizeT r, int w, BaseGDL::Cal_IOMode cMode) 
{
//  ***NOT COMPLETE: Repeat count will mostly *NOT* Work ! FIXME (see ofmt.cpp solution!)
  SizeT firstIn, firstOffs, tCount, tCountIn;
  IFmtAll( offs, 1, firstIn, firstOffs, tCount, tCountIn);

  SizeT trans = (*this)[ firstIn]->IFmtCal( is, firstOffs, r, w, cMode);
  if( trans >= tCount) return tCountIn;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstIn+1); i < ddSize; i++)
    {
      trans = (*this)[ i]->IFmtCal( is, 0, r, w, cMode);
      if( trans >= tCount) return tCountIn;
      tCount -= trans;
    }
  
  return tCountIn;
}


//#include "instantiate_templates.hpp"

#endif
