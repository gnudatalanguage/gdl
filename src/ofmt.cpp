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

static const std::string allstars="****************************************************************************************************************************";
template< typename Ty>
std::string binstr( Ty v, int w)
{
  SizeT bitsetsize=sizeof(Ty)*8;
    if (w==0) w=bitsetsize;
  SizeT first=0;
    if (bitsetsize == 8) {
    std::bitset<8>* me=new bitset<8>(v);
    for (SizeT i=0; i<8; ++i) if (me->test(7-i)) {first=i; break;}
    if (8-first > w) return allstars.substr(0,w); else
    return me->to_string<char,char_traits<char>,allocator<char> >().substr(first);
  } else if (bitsetsize == 16) {
    std::bitset<16>* me=new bitset<16>(v);
    for (SizeT i=0; i<16; ++i) if (me->test(15-i)) {first=i; break;}
    if (16-first > w) return allstars.substr(0,w); else
    return me->to_string<char,char_traits<char>,allocator<char> >().substr(first);
  } else  if (bitsetsize == 32) {
    std::bitset<32>* me=new bitset<32>(v);
    for (SizeT i=0; i<32; ++i) if (me->test(31-i)) {first=i; break;}
    if (32-first > w) return allstars.substr(0,w); else
    return me->to_string<char,char_traits<char>,allocator<char> >().substr(first);
  } else {
    std::bitset<64>* me=new bitset<64>(v);
    for (SizeT i=0; i<64; ++i) if (me->test(63-i)) {first=i; break;}
    if (64-first > w) return allstars.substr(0,w); else
    return me->to_string<char,char_traits<char>,allocator<char> >().substr(first);
 }
}

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
void OutScientific<DComplex>( ostream& os, DComplex val, int w, int d, char f, bool upper)
{
  OutScientific( os, val.real(), w, d, f, upper);
  OutScientific( os, val.imag(), w, d, f, upper);
}

template <>
void OutScientific<DComplexDbl>( ostream& os, DComplexDbl val, int w, int d, char f, bool upper)
{
  OutScientific( os, val.real(), w, d, f, upper);
  OutScientific( os, val.imag(), w, d, f, upper);
}

template <>
void OutAuto<DComplex>( ostream& os, DComplex val, int w, int d, char f, bool upper)
{
  OutAuto( os, val.real(), w, d, f, upper);
  OutAuto( os, val.imag(), w, d, f, upper);
}

template <>
void OutAuto<DComplexDbl>( ostream& os, DComplexDbl val, int w, int d, char f, bool upper)
{
  OutAuto( os, val.real(), w, d, f, upper);
  OutAuto( os, val.imag(), w, d, f, upper);
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

template<typename T> void OutHex(ostream* os,int w, char f, T x)
{
  ostringstream local_os;
  local_os << uppercase << hex << x;
  if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
}
template<typename T> void OutHexl(ostream* os,int w, char f, T x)
{
  ostringstream local_os;
  local_os << nouppercase << hex << x;
  if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
}
template<typename T> void OutOct(ostream* os,int w, char f, T x)
{
  ostringstream local_os;
  local_os << uppercase << oct << x;
  if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
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
    ( this->Convert2( GDL_STRING, BaseGDL::COPY_BYTE_AS_INT));
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
       BaseGDL::IOMode oMode, bool upper) 
{
  DDoubleGDL* cVal = static_cast<DDoubleGDL*>
    ( this->Convert2( GDL_DOUBLE, BaseGDL::COPY));
  SizeT retVal = cVal->OFmtF( os, offs, r, w, d, f, oMode, upper);
  delete cVal;
  return retVal;
}
// double
template<> SizeT Data_<SpDDouble>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode, bool upper) 
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
	OutAuto( *os, (*this)[ i], w, d, f, upper);
    }
  else if( oMode == FIXED) // F, D
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutFixed( *os, (*this)[ i], w, d, f);
    }
  else if ( oMode == SCIENTIFIC) // E 
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutScientific( *os, (*this)[ i], w, d, f, upper);
    }
  
  return tCount;
}
// float (same code as double)
template<> SizeT Data_<SpDFloat>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode, bool upper) 
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
	OutAuto( *os, (*this)[ i], w, d, f, upper);
    }
  else if( oMode == FIXED) // F, D
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutFixed( *os, (*this)[ i], w, d, f);
    }
  else if ( oMode == SCIENTIFIC) // E 
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutScientific( *os, (*this)[ i], w, d, f, upper);
    }

  return tCount;
}
// complex
template<> SizeT Data_<SpDComplex>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode, bool upper) 
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
	  OutAuto( *os, (*this)[ firstEl++].imag(), w, d, f, upper);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;
      
      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutAuto( *os, (*this)[ i], w, d, f, upper);
	}
  
      if( tCount & 0x01)
	{
	  OutAuto( *os, (*this)[ endEl].real(), w, d, f, upper);
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
	  OutScientific( *os, (*this)[ firstEl++].imag(), w, d, f, upper);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutScientific( *os, (*this)[ i], w, d, f, upper);
	}
  
      if( tCount & 0x01)
	{
	  OutScientific( *os, (*this)[ endEl].real(), w, d, f, upper);
	}
    }
  
  return tCountOut;
}
// same code a float
template<> SizeT Data_<SpDComplexDbl>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode, bool upper) 
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
	  OutAuto( *os, (*this)[ firstEl++].imag(), w, d, f, upper);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;
      
      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutAuto( *os, (*this)[ i], w, d, f, upper);
	}
  
      if( tCount & 0x01)
	{
	  OutAuto( *os, (*this)[ endEl].real(), w, d, f, upper);
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
	  OutScientific( *os, (*this)[ firstEl++].imag(), w, d, f, upper);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutScientific( *os, (*this)[ i], w, d, f, upper);
	}
  
      if( tCount & 0x01)
	{
	  OutScientific( *os, (*this)[ endEl].real(), w, d, f, upper);
	}
    }
  
  return tCountOut;
}
// struct
SizeT DStructGDL::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode, bool upper) 
{
  SizeT firstOut, firstOffs, tCount, tCountOut;
  OFmtAll( offs, r, firstOut, firstOffs, tCount, tCountOut);

  SizeT trans = (*this)[ firstOut]->OFmtF( os, firstOffs, tCount, w, d, f, oMode, upper);
  if( trans >= tCount) return tCountOut;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstOut+1); i < ddSize; ++i)
    {
      trans = (*this)[ i]->OFmtF( os, 0, tCount, w, d, f, oMode, upper);
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
  ossF << noshowpoint << noshowpos << setprecision(0);
  if (f == '+' || f == '@') ossF << showpos ;
  ossF << dd;
  int ddLen = ossF.str().size();
//w undefined --> d undefined --> default length = size of ossF.
  if (w == 0) w = ddLen; // I0 -> auto width
//d defined : if sign exist, augment d by 1.
  if ( d > 0 && ( dd < 0 || (f == '+' || f == '@')))  ++d; // minus or forced plus sign
//if zero padding and d undef, d=w
  if ( (f == '0'|| f == '@') && d == -1) d = w; // zero padding
//print stars if width cannot be respected.
  if( w < ddLen || d > w) 
    {
      OutStars( *os, w);
      return;
    }
//erase ossF, we'll need it again.
  ossF.str("");
// set width
  ossF.width(d);
//do padding if requested by f or by d > ddLen
  if ( (f == '0'|| f == '@') || d > ddLen ) {
    char prev = ossF.fill('0');
    ossF << std::internal << dd;
    ossF.fill(prev);
  } else ossF << dd;
//set total width: w
  streamsize oldw = os->width(w);
//write to os
  (*os) << ossF.str();
//reset os width
  os->width(oldw);
}
//                         undf byte int lint real dbl cplx str strct dcplx ptr obj uint ulon int64 uint64
const int iFmtWidth[] =    { -1,  7,  7,  12,  12,  12,  12, 12,   -1,   12, -1, -1,   7,   12,  22,   22}; 
const int iFmtWidthBIN[] = { -1,  8, 16,  32,  32,  32,  32, 32,   -1,   64, -1, -1,  16,   32,  64,   64};
			      // GDL_STRUCT-GDL_ULONG64

template<class Sp> SizeT Data_<Sp>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) {
    if ( this->Sizeof()==2 ) {
      DIntGDL* cVal = static_cast<DIntGDL*>
      (this->Convert2( GDL_INT, BaseGDL::COPY ));
      if ( w < 0 ) w = (oMode == BaseGDL::BIN ? iFmtWidthBIN[ this->t] : iFmtWidth[ this->t]);
      SizeT retVal = cVal->OFmtI( os, offs, r, w, d, f, oMode);
      delete cVal;
      return retVal;
    } else if ( this->Sizeof()==4 ) {
      DLongGDL* cVal = static_cast<DLongGDL*>
      (this->Convert2( GDL_LONG, BaseGDL::COPY )); 
      if ( w < 0 ) w = (oMode == BaseGDL::BIN ? iFmtWidthBIN[ this->t] : iFmtWidth[ this->t]);
      SizeT retVal = cVal->OFmtI( os, offs, r, w, d, f, oMode);
      delete cVal;
      return retVal;
    } else {
      DLong64GDL* cVal = static_cast<DLong64GDL*>
      (this->Convert2( GDL_LONG64, BaseGDL::COPY ));
      if ( w < 0 ) w = (oMode == BaseGDL::BIN ? iFmtWidthBIN[ this->t] : iFmtWidth[ this->t]);
      SizeT retVal = cVal->OFmtI( os, offs, r, w, d, f, oMode);
      delete cVal;
      return retVal;
    }
}
template<> SizeT Data_<SpDByte>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 8 : 7);
  SizeT nTrans = ToTransfer();
  DIntGDL* cVal = static_cast<DIntGDL*> (this->Convert2( GDL_INT, BaseGDL::COPY )); //necessary for non-b formats.

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  if( oMode == DEC)
    for( SizeT i=offs; i<endEl; ++i) ZeroPad( os, w, d, f, (*cVal)[ i]);
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i) OutOct(os, w, f, (*cVal)[ i]);
  else if ( oMode == BIN)
    for( SizeT i=offs; i<endEl; ++i) (*os) << setw(w) << setfill(f) << binstr((*this)[ i], w);
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i) OutHex(os, w, f, (*cVal)[ i]);
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i) OutHexl(os, w, f, (*cVal)[ i]);

  return tCount;
}

//GDL_UINT
template<> SizeT Data_<SpDUInt>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 16 : 7);
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  if( oMode == DEC)  for( SizeT i=offs; i<endEl; ++i)
      ZeroPad( os, w, d, f, (*this)[ i]);
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i) OutOct(os, w, f, (*this)[ i]);
  else if ( oMode == BIN)
    for( SizeT i=offs; i<endEl; ++i) (*os) << setw(w) << setfill(f) << binstr((*this)[ i], w);
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i) OutHex(os, w, f, (*this)[ i]);
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i) OutHexl(os, w, f, (*this)[ i]);

  return tCount;
}
//GDL_INT
template<> SizeT Data_<SpDInt>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, char f,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 16 : 7);
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  if( oMode == DEC)  for( SizeT i=offs; i<endEl; ++i)
      ZeroPad( os, w, d, f, (*this)[ i]);
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i) OutOct(os, w, f, (*this)[ i]);
  else if ( oMode == BIN)
    for( SizeT i=offs; i<endEl; ++i) (*os) << setw(w) << setfill(f) << binstr((*this)[ i], w);
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i) OutHex(os, w, f, (*this)[ i]);
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i) OutHexl(os, w, f, (*this)[ i]);

  return tCount;
}

// GDL_LONG
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
  
  if( oMode == DEC)  for( SizeT i=offs; i<endEl; ++i)
      ZeroPad( os, w, d, f, (*this)[ i]);
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i) OutOct(os, w, f, (*this)[ i]);
  else if ( oMode == BIN)
    for( SizeT i=offs; i<endEl; ++i) (*os) << setw(w) << setfill(f) << binstr((*this)[ i], w);
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i) OutHex(os, w, f, (*this)[ i]);
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i) OutHexl(os, w, f, (*this)[ i]);
  
  return tCount;
}
// GDL_ULONG
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
  
  if( oMode == DEC)  for( SizeT i=offs; i<endEl; ++i)
      ZeroPad( os, w, d, f, (*this)[ i]);
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i) OutOct(os, w, f, (*this)[ i]);
  else if ( oMode == BIN)
    for( SizeT i=offs; i<endEl; ++i) (*os) << setw(w) << setfill(f) << binstr((*this)[ i], w);
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i) OutHex(os, w, f, (*this)[ i]);
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i) OutHexl(os, w, f, (*this)[ i]);

  return tCount;
}
// GDL_LONG64
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

  if( oMode == DEC)  for( SizeT i=offs; i<endEl; ++i)
      ZeroPad( os, w, d, f, (*this)[ i]);
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i) OutOct(os, w, f, (*this)[ i]);
  else if ( oMode == BIN)
    for( SizeT i=offs; i<endEl; ++i) (*os) << setw(w) << setfill(f) << binstr((*this)[ i], w);
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i) OutHex(os, w, f, (*this)[ i]);
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i) OutHexl(os, w, f, (*this)[ i]);

  return tCount;
}
// GDL_ULONG64
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

  if( oMode == DEC)  for( SizeT i=offs; i<endEl; ++i)
      ZeroPad( os, w, d, f, (*this)[ i]);
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i) OutOct(os, w, f, (*this)[ i]);
  else if ( oMode == BIN)
    for( SizeT i=offs; i<endEl; ++i) (*os) << setw(w) << setfill(f) << binstr((*this)[ i], w);
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i) OutHex(os, w, f, (*this)[ i]);
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i) OutHexl(os, w, f, (*this)[ i]);

  return tCount;
}

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
  ostringstream local_os;
  if( offs & 0x01)
    {
      if ( oMode == BIN)	(*os) << setw(w) << setfill(f) << binstr(static_cast<long>((*this)[ firstEl++].imag()), w);
      else if( oMode == DEC) ZeroPad( os, w, d, f, static_cast<long> ((*this)[ firstEl++].imag()) );
      else {
        if ( oMode == OCT) local_os  << oct << static_cast<long>((*this)[ firstEl++].imag());
        else if ( oMode == HEX) local_os << uppercase << hex  << static_cast<long>((*this)[ firstEl++].imag());
        else local_os << hex  << static_cast<long>((*this)[ firstEl++].imag());
        if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os)<< setw(w) << setfill(f) <<local_os.str().c_str();
        local_os.seekp(0);
      }
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  if ( oMode == BIN )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      (*os) << setw( w ) << setfill( f ) << binstr( static_cast<long> ((*this)[ i].real( )), w );
      (*os) << setw( w ) << setfill( f ) << binstr( static_cast<long> ((*this)[ i].imag( )), w );
    } else if ( oMode == DEC )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      ZeroPad( os, w, d, f, static_cast<long> ((*this)[ i].real( )) );
      ZeroPad( os, w, d, f, static_cast<long> ((*this)[ i].imag( )) );
    } else if ( oMode == OCT )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os  << oct << static_cast<long>((*this)[ i].real());
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os.seekp(0);
      local_os  << oct << static_cast<long>((*this)[ i].imag());
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os.seekp(0);
    } else if ( oMode == HEX )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os << uppercase << hex << static_cast<long> ((*this)[ i].real( ));
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str();  else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os.seekp(0);
      local_os  << uppercase << hex << static_cast<long> ((*this)[ i].imag( ));
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os.seekp(0);
    } else
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os << nouppercase << hex << static_cast<long> ((*this)[ i].real( ));
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os.seekp(0);
      local_os  << nouppercase << hex << static_cast<long> ((*this)[ i].imag( ));
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();;
      local_os.seekp(0);
    }
  
  if( tCount & 0x01)
  {
      if ( oMode == BIN)	(*os) << setw(w) << setfill(f) << binstr(static_cast<long>((*this)[ endEl].real()), w);
      else if( oMode == DEC) ZeroPad( os, w, d, f, static_cast<long> ((*this)[ endEl].real()));
      else {
        if ( oMode == OCT) local_os  << oct << static_cast<long>((*this)[ endEl].real());
        else if ( oMode == HEX) local_os << uppercase << hex  << static_cast<long>((*this)[ endEl].real());
        else local_os << hex << static_cast<long>((*this)[ endEl].real());
        if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f )<<local_os.str().c_str();
        local_os.seekp(0); 
      }
  }
  return tCountOut;
}
// dbl complex
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
  ostringstream local_os;
  if( offs & 0x01)
    {
      if ( oMode == BIN)	(*os) << setw(w) << setfill(f) << binstr(static_cast<long int>((*this)[ firstEl++].imag()), w);
      else if( oMode == DEC) ZeroPad( os, w, d, f, static_cast<long int>((*this)[ firstEl++].imag()));
      else {
        if ( oMode == OCT) local_os  << oct << static_cast<long int>((*this)[ firstEl++].imag());
        else if ( oMode == HEX) local_os << uppercase << hex  << static_cast<long int>((*this)[ firstEl++].imag());
        else local_os << hex  << static_cast<long int>((*this)[ firstEl++].imag());
        if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os)<< setw(w) << setfill(f) <<local_os.str().c_str();
        local_os.seekp(0);
      }
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  if ( oMode == BIN )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      (*os) << setw( w ) << setfill( f ) << binstr( static_cast<long int> ((*this)[ i].real( )), w );
      (*os) << setw( w ) << setfill( f ) << binstr( static_cast<long int> ((*this)[ i].imag( )), w );
    } else if ( oMode == DEC )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      ZeroPad( os, w, d, f, static_cast<long int>((*this)[ i].real( )) );
      ZeroPad( os, w, d, f, static_cast<long int>((*this)[ i].imag( )) );
    } else if ( oMode == OCT )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os  << oct << static_cast<long int>((*this)[ i].real());
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os.seekp(0); 
      local_os  << oct << static_cast<long int>((*this)[ i].imag());
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os.seekp(0); 
    } else if ( oMode == HEX )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os << uppercase << hex << static_cast<long int> ((*this)[ i].real( ));
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os.seekp(0); 
      local_os  << uppercase << hex << static_cast<long int> ((*this)[ i].imag( ));
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os.seekp(0); 
    } else
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os << nouppercase << hex << static_cast<long int> ((*this)[ i].real( ));
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os.seekp(0); 
      local_os  << nouppercase << hex << static_cast<long int> ((*this)[ i].imag( ));
      if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();;
      local_os.seekp(0);
    }
  
  if( tCount & 0x01)
  {
      if ( oMode == BIN)	(*os) << setw(w) << setfill(f) << binstr(static_cast<long int>((*this)[ endEl].real()), w);
      else if( oMode == DEC) ZeroPad( os, w, d, f, static_cast<long int>((*this)[ endEl].real()) );
      else {
        if ( oMode == OCT) local_os  << oct << static_cast<long int>((*this)[ endEl].real());
        else if ( oMode == HEX) local_os << uppercase << hex  << static_cast<long int>((*this)[ endEl].real());
        else local_os << hex << static_cast<long int>((*this)[ endEl].real());
        if (w && local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f )<<local_os.str().c_str();
        local_os.seekp(0);
      }
  }
  return tCountOut;
}

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

void outA( ostream* os, string s, int w) 
{
  if (w==-1) w=3;
  if (w < 0) {
    (*os) << left;
    (*os) << setw(-w) << s;
  }
  else if (w == 0) {
    (*os) << right;
    (*os) << s;
  }
  else {
    (*os) << right;
    (*os) << setw(w) << s.substr(0, w);
  }
}
// struct
SizeT DStructGDL::
OFmtCal( ostream* os, SizeT offs, SizeT r, int w, int d, char *f,  BaseGDL::Cal_IOMode cMode) 
{
  SizeT firstOut, firstOffs, tCount, tCountOut;
  OFmtAll( offs, r, firstOut, firstOffs, tCount, tCountOut);

  SizeT trans = (*this)[ firstOut]->OFmtCal( os, firstOffs, tCount, w, d, f, cMode);
  if( trans >= tCount) return tCountOut;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstOut+1); i < ddSize; ++i)
    {
      trans = (*this)[ i]->OFmtCal( os, 0, tCount, w, d, f, cMode);
      if( trans >= tCount) return tCountOut;
      tCount -= trans;
     }

  return tCountOut;
}

 template<class Sp> SizeT Data_<Sp>::
 OFmtCal( ostream* os, SizeT offs, SizeT repeat, int w, int d, char *f, BaseGDL::Cal_IOMode cMode)
 {
  static DLong *iMonth, *iDay, *iYear, *iHour, *iMinute, *dow, *icap;
  static DDouble *Second;
  static ostringstream **local_os;
  bool cmplx=FALSE;
  SizeT nTrans = ToTransfer();
  // transfer count
  SizeT tCount = nTrans - offs;  
  SizeT r=tCount;
  if ( Data_<Sp>::IS_COMPLEX ) { cmplx=TRUE;} //tCount in this case is twice the size of the complex array 

  switch ( cMode ) {
    case BaseGDL::WRITE:
        for (SizeT i=0, j=0; j<r; j++){
          if (i >= repeat) {i=0; (*os)<<endl;}
          (*os)<<(local_os[j]->str()).c_str();
          i++;
          delete local_os[j];
        }
        delete local_os;
        delete iMonth;
        delete iDay ;
        delete iYear;
        delete iHour;
        delete iMinute;
        delete dow;
        delete icap;
        delete Second;
      break;
    case BaseGDL::COMPUTE:
      iMonth=(DLong*)calloc(r,sizeof(DLong));
      iDay=(DLong*)calloc(r,sizeof(DLong));
      iYear=(DLong*)calloc(r,sizeof(DLong));
      iHour=(DLong*)calloc(r,sizeof(DLong));
      iMinute=(DLong*)calloc(r,sizeof(DLong));
      dow=(DLong*)calloc(r,sizeof(DLong));
      icap=(DLong*)calloc(r,sizeof(DLong));
      Second=(DDouble*)calloc(r,sizeof(DDouble));
      local_os=(ostringstream**)calloc(r,sizeof(ostringstream*));
      if ( cmplx ) {
        DComplexDblGDL* cVal = static_cast<DComplexDblGDL*> (this->Convert2( GDL_COMPLEXDBL, BaseGDL::COPY ));
        for( SizeT i=0, j=0; j<(r/2); ++j)
        {
          local_os[i]=new ostringstream();
          if (!j2ymdhms( (*cVal)[offs +j].real(), iMonth[i], iDay[i], iYear[i], iHour[i], iMinute[i], Second[i], dow[i], icap[i] )) throw GDLException("Value of Julian date is out of allowed range.");
          i++;
          local_os[i]=new ostringstream();
          if (!j2ymdhms( (*cVal)[offs+j].imag(), iMonth[i], iDay[i], iYear[i], iHour[i], iMinute[i], Second[i], dow[i], icap[i] )) throw GDLException("Value of Julian date is out of allowed range.");
          i++;
        }
        delete cVal;
      } else {
        for ( SizeT i = 0; i < r; i++ ) {
          local_os[i]=new ostringstream();
          DDoubleGDL* cVal = static_cast<DDoubleGDL*> (this->Convert2( GDL_DOUBLE, BaseGDL::COPY ));
          if (!j2ymdhms( (*cVal)[offs + i], iMonth[i], iDay[i], iYear[i], iHour[i], iMinute[i], Second[i], dow[i], icap[i] )) throw GDLException("Value of Julian date is out of allowed range.");
          delete cVal;
//          cerr<<"Dow="<<dow[i]<<" iDay="<<iDay[i]<<" iMonth="<<iMonth[i]<<" iYear="<<iYear[i]<<" iHour="<<iHour[i]<<" iMinute="<<iMinute[i]<<" Second="<<Second[i]<<" icap="<<icap[i]<<endl;
        }
      }
      break;
    case BaseGDL::DEFAULT:
      for (SizeT i=0; i<r; i++){
      outA( local_os[i], theDay[dow[i]], 3 );
      (*local_os[i]) << " ";
      outA( local_os[i], theMonth[iMonth[i]], 3 );
      (*local_os[i]) << " ";
      ZeroPad( local_os[i], 2, 2, '0', iDay[i] );
      (*local_os[i]) << " ";
      ZeroPad( local_os[i], 2, 2, '0', iHour[i] );
      (*local_os[i]) << ":";
      ZeroPad( local_os[i], 2, 2, '0', iMinute[i] );
      (*local_os[i]) << ":";
      ZeroPad( local_os[i], 2, 2, '0', (DLong) (Second[i] + 0.5) );
      ZeroPad( local_os[i], 5, -1, ' ', iYear[i] );
      }
      break;
    case BaseGDL::STRING:
      for (SizeT i=0; i<r; i++){
      (*local_os[i]) << f;
      }
      break;
    case BaseGDL::CMOA:
      for (SizeT i=0; i<r; i++){
      outA( local_os[i], theMONTH[iMonth[i]], w );
      }
      break;
    case BaseGDL::CMoA:
      for (SizeT i=0; i<r; i++){
      outA( local_os[i], theMonth[iMonth[i]], w );
      }
      break;
    case BaseGDL::CmoA:
      for (SizeT i=0; i<r; i++){
      outA( local_os[i], themonth[iMonth[i]], w );
      }
      break;
    case BaseGDL::CDWA:
      for (SizeT i=0; i<r; i++){
      outA( local_os[i], theDAY[dow[i]], w );
      }
      break;
    case BaseGDL::CDwA:
      for (SizeT i=0; i<r; i++){
      outA( local_os[i], theDay[dow[i]], w );
      }
      break;
    case BaseGDL::CdwA:
      for (SizeT i=0; i<r; i++){
      outA( local_os[i], theday[dow[i]], w );
      }
      break;
    case BaseGDL::CapA:
      if ( w == -1 ) w = 2;
      for (SizeT i=0; i<r; i++){
      outA( local_os[i], capa[icap[i]], w );
      }
      break;
    case BaseGDL::CApA:
      if ( w == -1 ) w = 2;
      for (SizeT i=0; i<r; i++){
      outA( local_os[i], cApa[icap[i]], w );
      }
      break;
    case BaseGDL::CAPA:
      if ( w == -1 ) w = 2;
      for (SizeT i=0; i<r; i++){
      outA( local_os[i], cAPa[icap[i]], w );
      }
      break;
      //integer
    case BaseGDL::CMOI:
      if ( w == -1 ) w = 2;
      for (SizeT i=0; i<r; i++){
      ZeroPad( local_os[i], w, d, *f, iMonth[i]+1 );
      }
      break;
    case BaseGDL::CYI:
      if ( w == -1 ) w = 4;
      for (SizeT i=0; i<r; i++){ //convert to string before outing only the w last characters as this is what IDL does.
        std::stringbuf buffer; // empty buffer
        std::ostream os (&buffer); // associate stream buffer to stream
        os.width(w);
        os << iYear[i];
        outA( local_os[i], buffer.str().substr(buffer.str().size()-w,w), w ); //CYI2.2 selects the two last digits of year.
      }
      break;
    case BaseGDL::ChI:
      if ( w == -1 ) w = 2;
      for (SizeT i=0; i<r; i++){
      ZeroPad( local_os[i], w, d, *f, iHour[i]%12);
      }
      break;
    case BaseGDL::CHI:
      if ( w == -1 ) w = 2;
      for (SizeT i=0; i<r; i++){
      ZeroPad( local_os[i], w, d, *f, iHour[i] );
      }
      break;
    case BaseGDL::CDI:
      if ( w == -1 ) w = 2;
      for (SizeT i=0; i<r; i++){
      ZeroPad( local_os[i], w, d, *f, iDay[i] );
      }
      break;
    case BaseGDL::CMI:
      if ( w == -1 ) w = 2;
      for (SizeT i=0; i<r; i++){
      ZeroPad( local_os[i], w, d, *f, iMinute[i] );
      }
      break;
    case BaseGDL::CSI:
      if ( w == -1 ) {
        w = 2;
        d = 0;
      }
      for (SizeT i=0; i<r; i++){
      ZeroPad( local_os[i], w, d, *f, (DLong) (Second[i]) );
      }
      break;
      //Float
    case BaseGDL::CSF:
      if ( w == -1 ) {
        w = 5;
        d = 2;
      }
      //      SetField( w, d, 6,  16, 25);
      for (SizeT i=0; i<r; i++){
        //this may print Second as 60.xxx but IDL DOES THE SAME!
      OutFixed( *local_os[i], Second[i], w, d, *f );
      }
      break;
  }
  return tCount;
 }

//#include "instantiate_templates.hpp"

#endif
