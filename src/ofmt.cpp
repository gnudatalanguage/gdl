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

template<typename T> void OutHex(ostream* os,int w, char f, T x)
{
  ostringstream local_os;
  local_os << uppercase << hex << x;
  if (local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
}
template<typename T> void OutHexl(ostream* os,int w, char f, T x)
{
  ostringstream local_os;
  local_os << nouppercase << hex << x;
  if (local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
}
template<typename T> void OutOct(ostream* os,int w, char f, T x)
{
  ostringstream local_os;
  local_os << uppercase << oct << x;
  if (local_os.str().size() > w) (*os) << allstars.substr(0,w).c_str(); else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
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
       BaseGDL::IOMode oMode) 
{
  DDoubleGDL* cVal = static_cast<DDoubleGDL*>
    ( this->Convert2( GDL_DOUBLE, BaseGDL::COPY));
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
  ossF << noshowpoint << setprecision(0);
  if (f == '+') 
    ossF << "+";
  ossF << dd;
  int ddLen = ossF.str().size();

  if (w == 0) w = ddLen; // I0 -> auto width
  if (d > 0 && dd < 0) ++d; // minus sign
  if (f == '0' && d == -1) d = w; // zero padding

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
      if (nZero > 0 && dd < 0) // preventing "00-1.00" (instead of -001.00)
      {
        skip = 1;
        (*os) << "-";
      }
      for( SizeT i=0; i<nZero; ++i)
	(*os) << "0";
    }
  (*os) << ossF.str().substr(skip);
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
  if( w==0) { //still more special cases
     if ( oMode == OCT) w=3;
     if ( oMode == HEX || oMode == HEXL ) w=2; 
  }
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
  if( w==0) { //still more special cases
     if ( oMode == OCT) w=6;
     if ( oMode == HEX || oMode == HEXL ) w=4; 
  }
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
  if( w==0) { //still more special cases
     if ( oMode == OCT) w=6;
     if ( oMode == HEX || oMode == HEXL ) w=4; 
  }
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
  if( w==0) { //still more special cases
     if ( oMode == OCT) w=11;
     if ( oMode == HEX || oMode == HEXL ) w=8; 
  }
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
  if( w==0) { //still more special cases
     if ( oMode == OCT) w=11;
     if ( oMode == HEX || oMode == HEXL ) w=8; 
  }
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
  if( w==0) { //still more special cases
     if ( oMode == OCT) w=22;
     if ( oMode == HEX || oMode == HEXL ) w=16; 
  }
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
  if( w==0) { //still more special cases
     if ( oMode == OCT) w=22;
     if ( oMode == HEX || oMode == HEXL ) w=16; 
  }
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
  if( w==0) { //still more special cases
     if ( oMode == OCT) w=22;
     if ( oMode == HEX || oMode == HEXL ) w=16; 
  }
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
      else if( oMode == DEC) ZeroPad( os, w, d, f, (*this)[ firstEl++].imag());
      else {
        if ( oMode == OCT) local_os  << oct << static_cast<long>((*this)[ firstEl++].imag());
        else if ( oMode == HEX) local_os << uppercase << hex  << static_cast<long>((*this)[ firstEl++].imag());
        else local_os << hex  << static_cast<long>((*this)[ firstEl++].imag());
        if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os)<< setw(w) << setfill(f) <<local_os.str().c_str();
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
      ZeroPad( os, w, d, f, (*this)[ i].real( ) );
      ZeroPad( os, w, d, f, (*this)[ i].imag( ) );
    } else if ( oMode == OCT )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os  << oct << static_cast<long>((*this)[ i].real());
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os  << oct << static_cast<long>((*this)[ i].imag());
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
    } else if ( oMode == HEX )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os << uppercase << hex << static_cast<long> ((*this)[ i].real( ));
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os  << uppercase << hex << static_cast<long> ((*this)[ i].imag( ));
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
    } else
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os << nouppercase << hex << static_cast<long> ((*this)[ i].real( ));
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os  << nouppercase << hex << static_cast<long> ((*this)[ i].imag( ));
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();;
    }
  
  if( tCount & 0x01)
  {
      if ( oMode == BIN)	(*os) << setw(w) << setfill(f) << binstr(static_cast<long>((*this)[ endEl].real()), w);
      else if( oMode == DEC) ZeroPad( os, w, d, f, (*this)[ endEl].real());
      else {
        if ( oMode == OCT) local_os  << oct << static_cast<long>((*this)[ endEl].real());
        else if ( oMode == HEX) local_os << uppercase << hex  << static_cast<long>((*this)[ endEl].real());
        else local_os << hex << static_cast<long>((*this)[ endEl].real());
        if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f )<<local_os.str().c_str();
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
  if( w==0) { //still more special cases
     if ( oMode == OCT) w=22;
     if ( oMode == HEX || oMode == HEXL ) w=16; 
  }
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
      else if( oMode == DEC) ZeroPad( os, w, d, f, (*this)[ firstEl++].imag());
      else {
        if ( oMode == OCT) local_os  << oct << static_cast<long int>((*this)[ firstEl++].imag());
        else if ( oMode == HEX) local_os << uppercase << hex  << static_cast<long int>((*this)[ firstEl++].imag());
        else local_os << hex  << static_cast<long int>((*this)[ firstEl++].imag());
        if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os)<< setw(w) << setfill(f) <<local_os.str().c_str();
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
      ZeroPad( os, w, d, f, (*this)[ i].real( ) );
      ZeroPad( os, w, d, f, (*this)[ i].imag( ) );
    } else if ( oMode == OCT )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os  << oct << static_cast<long int>((*this)[ i].real());
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os  << oct << static_cast<long int>((*this)[ i].imag());
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
    } else if ( oMode == HEX )
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os << uppercase << hex << static_cast<long int> ((*this)[ i].real( ));
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os  << uppercase << hex << static_cast<long int> ((*this)[ i].imag( ));
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
    } else
    for ( SizeT i = firstEl; i < endEl; ++i ) {
      local_os << nouppercase << hex << static_cast<long int> ((*this)[ i].real( ));
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();
      local_os  << nouppercase << hex << static_cast<long int> ((*this)[ i].imag( ));
      if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f ) <<local_os.str().c_str();;
    }
  
  if( tCount & 0x01)
  {
      if ( oMode == BIN)	(*os) << setw(w) << setfill(f) << binstr(static_cast<long int>((*this)[ endEl].real()), w);
      else if( oMode == DEC) ZeroPad( os, w, d, f, (*this)[ endEl].real());
      else {
        if ( oMode == OCT) local_os  << oct << static_cast<long int>((*this)[ endEl].real());
        else if ( oMode == HEX) local_os << uppercase << hex  << static_cast<long int>((*this)[ endEl].real());
        else local_os << hex << static_cast<long int>((*this)[ endEl].real());
        if (local_os.str().size() > w) {(*os) << allstars.substr(0,w).c_str(); local_os.seekp(0);} else (*os) << setw( w ) << setfill( f )<<local_os.str().c_str();
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

// C code ****************************************************
    void j2ymdhms(DDouble jd, DLong &iMonth, DLong &iDay , DLong &iYear ,
                  DLong &iHour , DLong &iMinute, DDouble &Second)
    {
    DDouble JD,Z,F,a;
    DLong A,B,C,D,E;
    JD = jd + 0.5;
    Z = floor(JD);
    F = JD - Z;

    if (Z < 2299161) A = (DLong)Z;
    else {
      a = (DLong) ((Z - 1867216.25) / 36524.25);
      A = (DLong) (Z + 1 + a - (DLong)(a / 4));
    }

    B = A + 1524;
    C = (DLong) ((B - 122.1) / 365.25);
    D = (DLong) (365.25 * C);
    E = (DLong) ((B - D) / 30.6001);

    // month
    iMonth = E < 14 ? E - 1 : E - 13;
    // iday
    iDay=B - D - (DLong)(30.6001 * E);

    // year
    iYear = iMonth > 2 ? C - 4716 : C - 4715;
    // hours
    iHour = (DLong) (F * 24);
    F -= (DDouble)iHour / 24;
    // minutes
    iMinute = (DLong) (F * 1440);
    F -= (DDouble)iMinute / 1440;
    // seconds
    Second = F * 86400;
  }
   static struct 
   {
     DLong iMonth;
     DLong iDay;
     DLong iYear;
     DLong iHour;
     DLong iMinute;
     DLong dow;
     DLong icap;
     DDouble Second;
   } mytime;
   
// other
 
 template<class Sp> SizeT Data_<Sp>::
 OFmtCal( ostream* os, SizeT offs, SizeT r, int w, 
			int d, char f, BaseGDL::Cal_IOMode cMode)
 {
   DDoubleGDL* cVal = static_cast<DDoubleGDL*>
   ( this->Convert2( GDL_DOUBLE, BaseGDL::COPY));
   SizeT retVal = cVal->OFmtCal( os, offs, r, w, d, f, cMode);
   delete cVal;
   return retVal;
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
 //double
 template<> SizeT Data_<SpDDouble>::
 OFmtCal( ostream* os, SizeT offs, SizeT r, int w, 
			int d, char f, BaseGDL::Cal_IOMode cMode)
 {
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
   
   DLong iMonth, iDay , iYear , iHour , iMinute, dow, icap;
   DDouble Second;
   SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;
 
  
  for( SizeT i=offs; i<endEl; ++i)
  {
    j2ymdhms((*this)[ i], iMonth, iDay, iYear, iHour, iMinute, Second);
    // DayOfWeek
    dow=((DLong)((*this)[i]))%7;
    //capa:
    icap=(iHour>11);
    if( cMode == DEFAULT) 
    {
      fprintf(stderr,"cdef\n");
    }
    else if( cMode == CMOA)
    {
      outA(os, theMONTH[iMonth], w);//std::cout << theMONTH[iMonth];
    }
    else if( cMode == CMoA) 
    {
      outA(os, theMonth[iMonth], w);
    }
    else if ( cMode == CmoA)
    {
      outA(os, themonth[iMonth], w);
    }
    else if ( cMode == CDWA) 
    {
      outA(os, theDAY[dow], w);
    }
    else if ( cMode == CDwA) 
    {
      outA(os,theDay[dow], w);
    }
    else if ( cMode == CdwA) 
    {
      outA(os, theday[dow], w);
    }
    else if( cMode == CapA) 
    {
      outA(os, capa[icap], w);
    }
    else if( cMode == CApA) 
    {
      outA(os, cApa[icap], w);
    }
    else if( cMode == CAPA) 
    {
      outA(os, cAPa[icap], w);
    }
    //integer
    else if ( cMode == CMOI) 
    {
      if (w==-1) w=2; 
      ZeroPad( os, w, d, f, iMonth);
    }
    else if ( cMode == CYI) 
    {
      if (w==-1) w=4; 
      ZeroPad( os, w, d, f, iYear);
    }
    else if ( cMode == CHI) 
    {
      if (w==-1) w=2; 
      ZeroPad( os, w, d, f, iHour);
    }
    else if ( cMode == ChI) 
    {
      if (w==-1) w=2; 
      ZeroPad( os, w, d, f, iHour%12);
    }
    else if ( cMode == CDI) 
    {
      if (w==-1) w=2; 
      ZeroPad( os, w, d, f, iDay);
    }
    else if ( cMode == CMI) 
    {
      if (w==-1) w=2; 
      ZeroPad( os, w, d, f, iMinute);
    }
    else if ( cMode == CSI) 
    {
      if (w==-1) {w=2; d=0;}; 
      ZeroPad( os, w, d, f, (DLong)Second);
    }
    //Float
    else if ( cMode == CSF) 
    {
      if (w==-1) {w=5; d=4;} 
//      SetField( w, d, 6,  16, 25);
      OutAuto( *os, Second, w, d, f);
    }
  }
  return tCount;
 }

//#include "instantiate_templates.hpp"

#endif
