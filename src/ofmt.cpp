/***************************************************************************
                          ofmt.cpp  -  formatted input/output
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
#ifdef INCLUDE_OFMT_CPP

#include <sstream>
#include <bitset> // for binary output
#define binstr(v,w) bitset<32>(v).to_string<char,char_traits<char>,allocator<char> >().substr(32-(w),w)

//#include "datatypes.hpp"
//#include "dstructgdl.hpp"
#include "ofmt.hpp"

using namespace std;

template <>
void OutFixed<DComplex>( ostream& os, DComplex val, int w, int d, char f)
{
  OutFixed( os, val.real(), w, d, f);
  OutFixed( os, val.imag(), w, d, f);
}

template <>
void OutFixed<DComplexDbl>( ostream& os, DComplexDbl val, int w, int d, char f)
{
  OutFixed( os, val.real(), w, d, f);
  OutFixed( os, val.imag(), w, d, f);
}

template <>
void OutScientific<DComplex>( ostream& os, DComplex val, int w, int d, char f)
{
  OutScientific( os, val.real(), w, d, f);
  OutScientific( os, val.imag(), w, d, f);
}

template <>
void OutScientific<DComplexDbl>( ostream& os, DComplexDbl val, int w, int d, char f)
{
  OutScientific( os, val.real(), w, d, f);
  OutScientific( os, val.imag(), w, d, f);
}

template <>
void OutAuto<DComplex>( ostream& os, DComplex val, int w, int d, char f)
{
  OutAuto( os, val.real(), w, d, f);
  OutAuto( os, val.imag(), w, d, f);
}

template <>
void OutAuto<DComplexDbl>( ostream& os, DComplexDbl val, int w, int d, char f)
{
  OutAuto( os, val.real(), w, d, f);
  OutAuto( os, val.imag(), w, d, f);
}

void SetField( int& w, int& d, SizeT defPrec, SizeT maxPrec, SizeT wDef)
{
  if( w == -1)     // (X)
    {
    d = maxPrec;
    w = wDef;
    }
  else if( w == 0) // (X0)
    {
      if( d <= 0) d = defPrec;
    }
  else if( d < 0) 
    d = maxPrec;
}

// common code for all struct output functions
void DStructGDL::OFmtAll( SizeT offs, SizeT r,
			  SizeT& firstOut, SizeT& firstOffs,
			  SizeT& tCount, SizeT& tCountOut)
{
  SizeT nTrans = ToTransfer();

  // transfer count
  tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  tCountOut = tCount;

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
	SizeT tt=(*this)[firstTag]->ToTransfer();
	nB += tt;
	if( nB > firstOffs)
	  {
	    nB -= tt;
	    break;
	  }
      }

  firstOut = firstEl * NTags() + firstTag;
  firstOffs -= nB;
}

// A code ****************************************************
// other
template<class Sp> SizeT 
Data_<Sp>::OFmtA( ostream* os, SizeT offs, SizeT r, int w) 
{
  DStringGDL* stringVal = static_cast<DStringGDL*>
    ( this->Convert2( STRING, BaseGDL::COPY));
  SizeT retVal = stringVal->OFmtA( os, offs, r, w);
  delete stringVal;
  return retVal;
}
// string
template<> SizeT Data_<SpDString>::
OFmtA( ostream* os, SizeT offs, SizeT r, int w) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;


  if( w < 0)
    {
      (*os) << left;
      for( SizeT i=offs; i<endEl; ++i)
	{
	  (*os) << setw(-w) << (*this)[ i];
	}
    }
  else if( w == 0)
    {
      (*os) << right;
      for( SizeT i=offs; i<endEl; ++i)
	(*os) << (*this)[ i];
    }
  else
    {
      (*os) << right;
      for( SizeT i=offs; i<endEl; ++i)
	(*os) << setw(w) << (*this)[ i].substr(0,w);
    }

  return tCount;
}
// complex
template<> SizeT Data_<SpDComplex>::
OFmtA( ostream* os, SizeT offs, SizeT r, int w) 
{
  const int len = 13; 	
  
  if( w < 0) w = 0;
  
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;
  
  SizeT firstEl = offs / 2;
  
  (*os) << right;

  if( offs & 0x01)
    {
      if( w <= 0)
	(*os) << i2s( (*this)[ firstEl++].imag(), len);
      else
	(*os) << setw(w) << i2s( (*this)[ firstEl++].imag(), len);
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  if( w <= 0)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << i2s( (*this)[ i].real(), len);
	(*os) << i2s( (*this)[ i].imag(), len);
      }
  else
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << setw(w) << i2s( (*this)[ i].real(), len);
	(*os) << setw(w) << i2s( (*this)[ i].imag(), len);
      }
  
  if( tCount & 0x01)
    {
      if( w <= 0)
	(*os) << i2s( (*this)[ endEl++].real(), len);
      else
	(*os) << setw(w) << i2s( (*this)[ endEl++].real(), len);
    }

  return tCountOut;
}
template<> SizeT Data_<SpDComplexDbl>::
OFmtA( ostream* os, SizeT offs, SizeT r, int w) 
{
  const int len = 16; 	
  
  if( w < 0) w = 0;
  
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;
  
  SizeT firstEl = offs / 2;
  
  (*os) << right;

  if( offs & 0x01)
    {
      if( w <= 0)
	(*os) << i2s( (*this)[ firstEl++].imag(), len);
      else
	(*os) << setw(w) << i2s( (*this)[ firstEl++].imag(), len);
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  if( w <= 0)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << i2s( (*this)[ i].real(), len);
	(*os) << i2s( (*this)[ i].imag(), len);
      }
  else
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << setw(w) << i2s( (*this)[ i].real(), len);
	(*os) << setw(w) << i2s( (*this)[ i].imag(), len);
      }
  
  if( tCount & 0x01)
    {
      if( w <= 0)
	(*os) << i2s( (*this)[ endEl++].real(), len);
      else
	(*os) << setw(w) << i2s( (*this)[ endEl++].real(), len);
    }

  return tCountOut;
}
// struct
SizeT DStructGDL::
OFmtA( ostream* os, SizeT offs, SizeT r, int w) 
{
  SizeT firstOut, firstOffs, tCount, tCountOut;
  OFmtAll( offs, r, firstOut, firstOffs, tCount, tCountOut);

  SizeT trans = (*this)[ firstOut]->OFmtA( os, firstOffs, tCount, w);
  if( trans >= tCount) return tCountOut;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstOut+1); i < ddSize; ++i)
    {
      trans = (*this)[ i]->OFmtA( os, 0, tCount, w);
      if( trans >= tCount) return tCountOut;
      tCount -= trans;
    }

  return tCountOut;
}

// F code ****************************************************
// other
template<class Sp> SizeT Data_<Sp>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  DDoubleGDL* cVal = static_cast<DDoubleGDL*>
    ( this->Convert2( DOUBLE, BaseGDL::COPY));
  SizeT retVal = cVal->OFmtF( os, offs, r, w, d, f, oMode);
  delete cVal;
  return retVal;
}
// double
template<> SizeT Data_<SpDDouble>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;
 
  SetField( w, d, 6,  16, 25);

  if( oMode == AUTO) // G
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutAuto( *os, (*this)[ i], w, d, f);
    }
  else if( oMode == FIXED) // F, D
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutFixed( *os, (*this)[ i], w, d, f);
    }
  else if ( oMode == SCIENTIFIC) // E 
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutScientific( *os, (*this)[ i], w, d, f);
    }
  
  return tCount;
}
// float (same code as double)
template<> SizeT Data_<SpDFloat>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  SetField( w, d, 6, 7, 15);

  if( oMode == AUTO) // G
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutAuto( *os, (*this)[ i], w, d, f);
    }
  else if( oMode == FIXED) // F, D
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutFixed( *os, (*this)[ i], w, d, f);
    }
  else if ( oMode == SCIENTIFIC) // E 
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutScientific( *os, (*this)[ i], w, d, f);
    }

  return tCount;
}
// complex
template<> SizeT Data_<SpDComplex>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;

  SizeT firstEl = offs / 2;

  SetField( w, d, 6, 7, 15);

  if( oMode == AUTO)
    {
      if( offs & 0x01)
	{
	  OutAuto( *os, (*this)[ firstEl++].imag(), w, d, f);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;
      
      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutAuto( *os, (*this)[ i], w, d, f);
	}
  
      if( tCount & 0x01)
	{
	  OutAuto( *os, (*this)[ endEl].real(), w, d, f);
	}
    }
  else if( oMode == FIXED)
    {
      if( offs & 0x01)
	{
	  OutFixed( *os, (*this)[ firstEl++].imag(), w, d, f);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutFixed( *os, (*this)[ i], w, d, f);
	}
  
      if( tCount & 0x01)
	{
	  OutFixed( *os, (*this)[ endEl].real(), w, d, f);
	}
    }
  else if ( oMode == SCIENTIFIC)
    {
      if( offs & 0x01)
	{
	  OutScientific( *os, (*this)[ firstEl++].imag(), w, d, f);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutScientific( *os, (*this)[ i], w, d, f);
	}
  
      if( tCount & 0x01)
	{
	  OutScientific( *os, (*this)[ endEl].real(), w, d, f);
	}
    }
  
  return tCountOut;
}
// same code a float
template<> SizeT Data_<SpDComplexDbl>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;

  SizeT firstEl = offs / 2;

  SetField( w, d, 6, 16, 25);

  if( oMode == AUTO)
    {
      if( offs & 0x01)
	{
	  OutAuto( *os, (*this)[ firstEl++].imag(), w, d, f);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;
      
      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutAuto( *os, (*this)[ i], w, d, f);
	}
  
      if( tCount & 0x01)
	{
	  OutAuto( *os, (*this)[ endEl].real(), w, d, f);
	}
    }
  else if( oMode == FIXED)
    {
      if( offs & 0x01)
	{
	  OutFixed( *os, (*this)[ firstEl++].imag(), w, d, f);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutFixed( *os, (*this)[ i], w, d, f);
	}
  
      if( tCount & 0x01)
	{
	  OutFixed( *os, (*this)[ endEl].real(), w, d, f);
	}
    }
  else if ( oMode == SCIENTIFIC)
    {
      if( offs & 0x01)
	{
	  OutScientific( *os, (*this)[ firstEl++].imag(), w, d, f);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutScientific( *os, (*this)[ i], w, d, f);
	}
  
      if( tCount & 0x01)
	{
	  OutScientific( *os, (*this)[ endEl].real(), w, d, f);
	}
    }
  
  return tCountOut;
}
// struct
SizeT DStructGDL::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  SizeT firstOut, firstOffs, tCount, tCountOut;
  OFmtAll( offs, r, firstOut, firstOffs, tCount, tCountOut);

  SizeT trans = (*this)[ firstOut]->OFmtF( os, firstOffs, tCount, w, d, f, oMode);
  if( trans >= tCount) return tCountOut;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstOut+1); i < ddSize; ++i)
    {
      trans = (*this)[ i]->OFmtF( os, 0, tCount, w, d, f, oMode);
      if( trans >= tCount) return tCountOut;
      tCount -= trans;
    }

  return tCountOut;
}

// I code ****************************************************
// other

template <typename longT> 
void ZeroPad( ostream* os, int w, int d, char f, longT dd)
{
  std::ostringstream ossF;
  ossF << noshowpoint << setprecision(0) << dd;
  int ddLen = ossF.str().size();
  if (d > 0 && dd < 0) ++d; 
  if (f == '0' && d == -1) d = w;
  if( w < ddLen || d > w) 
    {
      OutStars( *os, w);
      return;
    }
  int skip = 0;
  if( d <= ddLen)
    {
      for( SizeT i = ddLen; i<w; ++i)
	(*os) << " ";
    }
  else
    {
      for( SizeT i=0; i<(w-d); ++i)
	(*os) << " ";

      int nZero = d-ddLen;
      if (nZero > 0 && dd < 0) // preventing "00-1.00"
      {
        skip = 1;
        (*os) << "-";
      }
      for( SizeT i=0; i<nZero; ++i)
	(*os) << "0";
    }
  (*os) << ossF.str().substr(skip);
}

const int iFmtWidth[] =    { -1,  7,  7, 12, 12, 12, 12, 12,  // UNDEF-STRING
			     -1, 12, -1, -1,  7, 12, 22, 22}; // STRUCT-ULONG64
const int iFmtWidthBIN[] = { -1,  8, 16, 32, 32, 32, 32, 32,  // UNDEF-STRING
			     -1, 32, -1, -1, 16, 32, 64, 64}; // STRUCT-ULONG64

template<class Sp> SizeT Data_<Sp>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  DLongGDL* cVal = static_cast<DLongGDL*>
    ( this->Convert2( LONG, BaseGDL::COPY));
  if( w < 0) w = (oMode == BaseGDL::BIN ? iFmtWidthBIN[ this->t] : iFmtWidth[ this->t]); 
  SizeT retVal = cVal->OFmtI( os, offs, r, w, d, f, oMode);
  delete cVal;
  return retVal;
}
// LONG
template<> SizeT Data_<SpDLong>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 32 : 12);

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  if( oMode == DEC)
    for( SizeT i=offs; i<endEl; ++i)
      ZeroPad( os, w, d, f, (*this)[ i]);
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << oct << setw(w) << setfill(f) << (*this)[ i];
  else if ( oMode == BIN)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << binstr((*this)[ i], w);
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << uppercase << hex << setw(w) << setfill(f) << (*this)[ i];
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << nouppercase << hex << setw(w) << setfill(f) << (*this)[ i];

  return tCount;
}
// ULONG
template<> SizeT Data_<SpDULong>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 32 : 12);

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;
  
  if( oMode == DEC)
    for( SizeT i=offs; i<endEl; ++i)
      ZeroPad( os, w, d, f, (*this)[ i]);
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << oct << setw(w) << setfill(f) << (*this)[ i];
  else if ( oMode == BIN)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << binstr((*this)[ i], w);
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << uppercase << hex << setw(w) << setfill(f) << (*this)[ i];
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << nouppercase << hex << setfill(f) << setw(w) << (*this)[ i];

  return tCount;
}
// LONG64
template<> SizeT Data_<SpDLong64>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 64 : 22);

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  if( oMode == DEC)
    for( SizeT i=offs; i<endEl; ++i)
      ZeroPad( os, w, d, f, (*this)[ i]);
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << oct << setw(w) << setfill(f) << (*this)[ i];
  else if ( oMode == BIN)
    for( SizeT i=offs; i<endEl; ++i)
    {
      if (w > 32) (*os) << binstr((*this)[ i] >> 32, w - 32);
      (*os) << binstr((*this)[ i], w <= 32 ? w : 32);
    }
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << uppercase << hex << setw(w) << setfill(f) << (*this)[ i];
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << nouppercase << hex << setw(w) << setfill(f) << (*this)[ i];

  return tCount;
}
// ULONG64
template<> SizeT Data_<SpDULong64>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 64 : 22);

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  if( oMode == DEC)
    for( SizeT i=offs; i<endEl; ++i)
      ZeroPad( os, w, d, f, (*this)[ i]);
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << oct << setw(w) << setfill(f) << (*this)[ i];
  else if ( oMode == BIN)
    for( SizeT i=offs; i<endEl; ++i)
    {
      if (w > 32) (*os) << binstr((*this)[ i] >> 32, w - 32);
      (*os) << binstr((*this)[ i], w <= 32 ? w : 32);
    }
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << uppercase << hex << setw(w) << setfill(f) << (*this)[ i];
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << nouppercase << hex << setw(w) << setfill(f) << (*this)[ i];

  return tCount;
}
// complex
template<> SizeT Data_<SpDComplex>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 32 : 12);

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;

  SizeT firstEl = offs / 2;

  if( offs & 0x01)
    {
      if( oMode == DEC)
        ZeroPad( os, w, d, f, (*this)[ firstEl++].imag());
      else if ( oMode == OCT)
	(*os) << oct << setw(w) << setfill(f) << static_cast<long int>((*this)[ firstEl++].imag());
      else if ( oMode == BIN)
	(*os) << binstr(static_cast<long int>((*this)[ firstEl++].imag()), w);
      else if ( oMode == HEX)
	(*os) << uppercase << hex << setw(w) << setfill(f) 
	      << static_cast<long int>((*this)[ firstEl++].imag());
      else
	(*os) << hex << setw(w) << setfill(f) << static_cast<long int>((*this)[ firstEl++].imag());
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  if( oMode == DEC)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
        ZeroPad( os, w, d, f, (*this)[ i].real());
        ZeroPad( os, w, d, f, (*this)[ i].imag());
      }
  else if ( oMode == OCT)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << oct << setw(w) << setfill(f) << static_cast<long int>((*this)[ i].real());
	(*os) << oct << setw(w) << setfill(f) << static_cast<long int>((*this)[ i].imag());
      }
  else if ( oMode == BIN)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << binstr(static_cast<long int>((*this)[ i].real()), w);
	(*os) << binstr(static_cast<long int>((*this)[ i].imag()), w);
      }
  else if ( oMode == HEX)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << uppercase << hex << setw(w) << setfill(f) << static_cast<long int>((*this)[ i].real());
	(*os) << uppercase << hex << setw(w) << setfill(f) << static_cast<long int>((*this)[ i].imag());
      }
  else
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << nouppercase << hex << setw(w) << setfill(f) 
	      << static_cast<long int>((*this)[ i].real());
	(*os) << nouppercase << hex << setw(w) << setfill(f)
	      << static_cast<long int>((*this)[ i].imag());
      }
  
  if( tCount & 0x01)
    {
      if( oMode == DEC)
        ZeroPad( os, w, d, f, (*this)[ endEl].real());
      else if ( oMode == OCT)
	(*os) << oct << setw(w) << setfill(f) << static_cast<long int>((*this)[ endEl].real());
      else if ( oMode == BIN)
	(*os) << binstr(static_cast<long int>((*this)[ endEl].real()), w);
      else if ( oMode == HEX)
	(*os) << uppercase << hex << setw(w) << setfill(f)
	      << static_cast<long int>((*this)[ endEl].real());
      else
	(*os) << nouppercase << hex << setw(w) << setfill(f)
	      << static_cast<long int>((*this)[ endEl].real());
    }
  
  return tCountOut;
}
template<> SizeT Data_<SpDComplexDbl>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 32 : 12);

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;

  SizeT firstEl = offs / 2;

  if( offs & 0x01)
    {
      if( oMode == DEC)
        ZeroPad( os, w, d, f, (*this)[ firstEl++].imag());
      else if ( oMode == OCT)
	(*os) << oct << setw(w) << setfill(f) << static_cast<long int>((*this)[ firstEl++].imag());
      else if ( oMode == BIN)
	(*os) << binstr(static_cast<long int>((*this)[ firstEl++].imag()), w);
      else if ( oMode == HEX)
	(*os) << uppercase << hex << setw(w) << setfill(f) 
	      << static_cast<long int>((*this)[ firstEl++].imag());
      else
	(*os) << hex << setw(w) << setfill(f) << static_cast<long int>((*this)[ firstEl++].imag());
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  if( oMode == DEC)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
        ZeroPad( os, w, d, f, (*this)[ i].real());
        ZeroPad( os, w, d, f, (*this)[ i].imag());
      }
  else if ( oMode == OCT)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << oct << setw(w) << setfill(f) << static_cast<long int>((*this)[ i].real());
	(*os) << oct << setw(w) << setfill(f) << static_cast<long int>((*this)[ i].imag());
      }
  else if ( oMode == BIN)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << binstr(static_cast<long int>((*this)[ i].real()), w);
	(*os) << binstr(static_cast<long int>((*this)[ i].imag()), w);
      }
  else if ( oMode == HEX)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << uppercase << hex << setw(w) << setfill(f) << static_cast<long int>((*this)[ i].real());
	(*os) << uppercase << hex << setw(w) << setfill(f) << static_cast<long int>((*this)[ i].imag());
      }
  else
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << nouppercase << hex << setw(w) << setfill(f)
	      << static_cast<long int>((*this)[ i].real());
	(*os) << nouppercase << hex << setw(w) << setfill(f)
	      << static_cast<long int>((*this)[ i].imag());
      }
  
  if( tCount & 0x01)
    {
      if( oMode == DEC)
        ZeroPad( os, w, d, f, (*this)[ endEl].real());
      else if ( oMode == OCT)
	(*os) << oct << setw(w) << setfill(f) << static_cast<long int>((*this)[ endEl].real());
      else if ( oMode == BIN)
	(*os) << binstr(static_cast<long int>((*this)[ endEl].real()), w);
      else if ( oMode == HEX)
	(*os) << uppercase << hex << setw(w) << setfill(f)
	      << static_cast<long int>((*this)[ endEl].real());
      else
	(*os) << nouppercase << hex << setw(w) << setfill(f)
	      << static_cast<long int>((*this)[ endEl].real());
    }
  
  return tCountOut;
}
// struct
SizeT DStructGDL::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  SizeT firstOut, firstOffs, tCount, tCountOut;
  OFmtAll( offs, r, firstOut, firstOffs, tCount, tCountOut);

  SizeT trans = (*this)[ firstOut]->OFmtI( os, firstOffs, tCount, w, d, f, oMode);
  if( trans >= tCount) return tCountOut;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstOut+1); i < ddSize; ++i)
    {
      trans = (*this)[ i]->OFmtI( os, 0, tCount, w, d, f, oMode);
      if( trans >= tCount) return tCountOut;
      tCount -= trans;
    }
  
  return tCountOut;
}


//#include "instantiate_templates.hpp"

#endif
