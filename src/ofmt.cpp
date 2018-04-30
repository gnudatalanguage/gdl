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

static const std::string allstars="****************************************************************************************************************************";
template< typename Ty>
std::string binstr( const Ty v, int w, int d, int code)
{ 
  bool dofill=( ( (code & fmtPAD) > 0 ) );
  SizeT bitsetsize=sizeof(Ty)*8;
  if ( v==Ty(0.0) ) {
   if ( w<=0 ) return "0";
   if ( (code & fmtALIGN_LEFT) > 0 ) {
    return "0";
   } else {
    if (d>0) {
     if (d<=w) {
      std::string z(d,'0');
      std::string s(w-d,' ');
      s+=z;
      return s;
     } else {
      std::string s(w+10,' '); //overfill to get ***
      return s;
     } 
    } else {
     std::string s(w-1,dofill?'0':' ');
     s+='0';
     return s;
    }
   }
  }
  
  if (w==0) w=bitsetsize;

  SizeT first=0;
    if (bitsetsize == 8) {
    std::bitset<8> me(v);
    for (SizeT i=0; i<8; ++i) if (me.test(7-i)) {first=i; break;}
    if (8-first > w) return allstars.substr(0,w); else
    return me.to_string<char,char_traits<char>,allocator<char> >().substr(first);
  } else if (bitsetsize == 16) {
    std::bitset<16> me(v);
    for (SizeT i=0; i<16; ++i) if (me.test(15-i)) {first=i; break;}
    if (16-first > w) return allstars.substr(0,w); else
    return me.to_string<char,char_traits<char>,allocator<char> >().substr(first);
  } else  if (bitsetsize == 32) {
    std::bitset<32> me(v);
    for (SizeT i=0; i<32; ++i) if (me.test(31-i)) {first=i; break;}
    if (32-first > w) return allstars.substr(0,w); else
    return me.to_string<char,char_traits<char>,allocator<char> >().substr(first);
  } else {
    std::bitset<64> me(v);
    for (SizeT i=0; i<64; ++i) if (me.test(63-i)) {first=i; break;}
    if (64-first > w) return allstars.substr(0,w); else
    return me.to_string<char,char_traits<char>,allocator<char> >().substr(first);
 }
}

#include "ofmt.hpp"

template <typename T>
void OutInteger(std::ostream& os, const T &val, const int w, const int d, int code, const BaseGDL::IOMode oMode) {
 std::ostringstream oss;
  if (d > 0) {
   std::ostringstream ossI;
   if (code & fmtSHOWPOS) ossI << std::showpos;
   if (oMode == BaseGDL::DEC) ossI << val;
   else if (oMode == BaseGDL::OCT) ossI << std::oct << val;
   else if (oMode == BaseGDL::BIN) ossI << binstr(val, w, d, code);
   else if (oMode == BaseGDL::HEX) ossI << std::uppercase << std::hex << val;
   else ossI << std::nouppercase << std::hex << val; // HEXL
   //force PADDING
   code |= fmtPAD;
   OutAdjustFill(oss, ossI.str(), d, code);
   //remove PAD for next treatment ---> must be blanks
   code &= (~fmtPAD);
 } else {
  if (code & fmtSHOWPOS) oss << std::showpos;
  if (d > 0) oss << std::setw(d) << std::setfill('0');
  if (oMode == BaseGDL::DEC) oss << val;
  else if (oMode == BaseGDL::OCT) oss << std::oct << val;
  else if (oMode == BaseGDL::BIN) oss << binstr(val, w, d, code);
  else if (oMode == BaseGDL::HEX) oss << std::uppercase << std::hex << val;
  else oss << std::nouppercase << std::hex << val; // HEXL
 }
 if (w == 0)
  os << oss.str();
 else if (oss.tellp() > w)
  OutStars(os, w);
 else if (code & fmtALIGN_LEFT) {
  os << std::left;
  os << std::setw(w);
  os << oss.str();
  os << std::right;
 } else
  OutFixFill(os, oss.str(), w, code);
}


template <>
void OutFixed<DComplex>(ostream& os, const DComplex &val, const int w, const int d, const int code)
{
  OutFixed(os, val.real(), w, d, code);
  OutFixed(os, val.imag(), w, d, code);
}

template <>
void OutFixed<DComplexDbl>( ostream& os, const DComplexDbl &val, const int w, const int d, const int code)
{
  OutFixed(os, val.real(), w, d, code);
  OutFixed(os, val.imag(), w, d, code);
}

template <>
void OutScientific<DComplex>( ostream& os, const DComplex &val, const int w, const int d, const int code)
{
  OutScientific( os, val.real(), w, d, code);
  OutScientific( os, val.imag(), w, d, code);
}

template <>
void OutScientific<DComplexDbl>( ostream& os, const DComplexDbl &val, const int w, const int d, const int code)
{
  OutScientific( os, val.real(), w, d, code);
  OutScientific( os, val.imag(), w, d, code);
}

template <>
void OutAuto<DComplex>( ostream& os, const DComplex &val, const int w, const int d, const int code)
{
  OutAuto( os, val.real(), w, d, code);
  OutAuto( os, val.imag(), w, d, code);
}

template <>
void OutAuto<DComplexDbl>( ostream& os, const DComplexDbl &val, const int w, const int d, const int code)
{
  OutAuto( os, val.real(), w, d, code);
  OutAuto( os, val.imag(), w, d, code);
}

void SetDefaultFieldLengths( int& w, int& d, const SizeT defPrec, const SizeT maxPrec, const SizeT wDef)
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
  else if( d < 0) //should never happen now.
    d = maxPrec;
}

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
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, const int code, const BaseGDL::IOMode oMode) 
{
  DDoubleGDL* cVal = static_cast<DDoubleGDL*>
    ( this->Convert2( GDL_DOUBLE, BaseGDL::COPY));
  SizeT retVal = cVal->OFmtF( os, offs, r, w, d, code, oMode);
  delete cVal;
  return retVal;
}
// double
template<> SizeT Data_<SpDDouble>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, const int code, const BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;
 
  SetDefaultFieldLengths( w, d, 6,  16, 25);

  if( oMode == AUTO) // G
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutAuto( *os, (*this)[ i], w, d, code);
    }
  else if( oMode == FIXED) // F, D
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutFixed(*os, (*this)[ i], w, d, code);
    }
  else if ( oMode == SCIENTIFIC) // E 
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutScientific( *os, (*this)[ i], w, d, code);
    }
  
  return tCount;
}
// float (same code as double)
template<> SizeT Data_<SpDFloat>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, const int code, const BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  SetDefaultFieldLengths( w, d, 6, 7, 15);

  if( oMode == AUTO) // G
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutAuto( *os, (*this)[ i], w, d, code);
    }
  else if( oMode == FIXED) // F, D
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutFixed(*os, (*this)[ i], w, d, code);
    }
  else if ( oMode == SCIENTIFIC) // E 
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutScientific( *os, (*this)[ i], w, d, code);
    }

  return tCount;
}
// complex
template<> SizeT Data_<SpDComplex>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, const int code, const BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;

  SizeT firstEl = offs / 2;

  SetDefaultFieldLengths( w, d, 6, 7, 15);

  if( oMode == AUTO)
    {
      if( offs & 0x01)
	{
	  OutAuto( *os, (*this)[ firstEl++].imag(), w, d, code);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;
      
      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutAuto( *os, (*this)[ i], w, d, code);
	}
  
      if( tCount & 0x01)
	{
	  OutAuto( *os, (*this)[ endEl].real(), w, d, code);
	}
    }
  else if( oMode == FIXED)
    {
      if( offs & 0x01)
	{
	  OutFixed(*os, (*this)[ firstEl++].imag(), w, d, code);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutFixed(*os, (*this)[ i], w, d, code);
	}
  
      if( tCount & 0x01)
	{
	  OutFixed(*os, (*this)[ endEl].real(), w, d, code);
	}
    }
  else if ( oMode == SCIENTIFIC)
    {
      if( offs & 0x01)
	{
	  OutScientific( *os, (*this)[ firstEl++].imag(), w, d, code);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutScientific( *os, (*this)[ i], w, d, code);
	}
  
      if( tCount & 0x01)
	{
	  OutScientific( *os, (*this)[ endEl].real(), w, d, code);
	}
    }
  
  return tCountOut;
}
// same code a float
template<> SizeT Data_<SpDComplexDbl>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, const int code, const BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;

  SizeT firstEl = offs / 2;

  SetDefaultFieldLengths( w, d, 6, 16, 25);

  if( oMode == AUTO)
    {
      if( offs & 0x01)
	{
	  OutAuto( *os, (*this)[ firstEl++].imag(), w, d, code);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;
      
      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutAuto( *os, (*this)[ i], w, d, code);
	}
  
      if( tCount & 0x01)
	{
	  OutAuto( *os, (*this)[ endEl].real(), w, d, code);
	}
    }
  else if( oMode == FIXED)
    {
      if( offs & 0x01)
	{
	  OutFixed(*os, (*this)[ firstEl++].imag(), w, d, code);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutFixed(*os, (*this)[ i], w, d, code);
	}
  
      if( tCount & 0x01)
	{
	  OutFixed(*os, (*this)[ endEl].real(), w, d, code);
	}
    }
  else if ( oMode == SCIENTIFIC)
    {
      if( offs & 0x01)
	{
	  OutScientific( *os, (*this)[ firstEl++].imag(), w, d, code);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutScientific( *os, (*this)[ i], w, d, code);
	}
  
      if( tCount & 0x01)
	{
	  OutScientific( *os, (*this)[ endEl].real(), w, d, code);
	}
    }
  
  return tCountOut;
}
// struct
SizeT DStructGDL::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, const int code, BaseGDL::IOMode oMode) 
{
  SizeT firstOut, firstOffs, tCount, tCountOut;
  OFmtAll( offs, r, firstOut, firstOffs, tCount, tCountOut);

  SizeT trans = (*this)[ firstOut]->OFmtF( os, firstOffs, tCount, w, d, code, oMode);
  if( trans >= tCount) return tCountOut;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstOut+1); i < ddSize; ++i)
    {
      trans = (*this)[ i]->OFmtF( os, 0, tCount, w, d, code, oMode);
      if( trans >= tCount) return tCountOut;
      tCount -= trans;
    }

  return tCountOut;
}

// I code ****************************************************
// other

//                         undf byte int lint real dbl cplx str strct dcplx ptr obj uint ulon int64 uint64
const int iFmtWidth[] =    { -1,  7,  7,  12,  12,  12,  12, 12,   -1,   12, -1, -1,   7,   12,  22,   22}; 
const int iFmtWidthBIN[] = { -1,  8, 16,  32,  32,  32,  32, 32,   -1,   64, -1, -1,  16,   32,  64,   64};
			      // GDL_STRUCT-GDL_ULONG64

template<class Sp> SizeT Data_<Sp>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, int code, 
       BaseGDL::IOMode oMode) {
    if ( this->Sizeof()==2 ) {
      DIntGDL* cVal = static_cast<DIntGDL*>
      (this->Convert2( GDL_INT, BaseGDL::COPY ));
      if ( w < 0 ) w = (oMode == BaseGDL::BIN ? iFmtWidthBIN[ this->t] : iFmtWidth[ this->t]);
      SizeT retVal = cVal->OFmtI( os, offs, r, w, d, code, oMode);
      delete cVal;
      return retVal;
//FIXME THIS MAY DEPEND ON THE MACHINE NATURAL SIZE. ON 64 BITS it is promoted to 64 bits.
//    } else if ( this->Sizeof()==4 ) {
//      DLongGDL* cVal = static_cast<DLongGDL*>
//      (this->Convert2( GDL_LONG, BaseGDL::COPY )); 
//      if ( w < 0 ) w = (oMode == BaseGDL::BIN ? iFmtWidthBIN[ this->t] : iFmtWidth[ this->t]);
//      SizeT retVal = cVal->OFmtI( os, offs, r, w, d, code, oMode);
//      delete cVal;
//      return retVal;
    } else {
      DLong64GDL* cVal = static_cast<DLong64GDL*>
      (this->Convert2( GDL_LONG64, BaseGDL::COPY ));
      if ( w < 0 ) w = (oMode == BaseGDL::BIN ? iFmtWidthBIN[ this->t] : iFmtWidth[ this->t]);
      SizeT retVal = cVal->OFmtI( os, offs, r, w, d, code, oMode);
      delete cVal;
      return retVal;
    }
}
template<> SizeT Data_<SpDByte>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, int code,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 8 : 7);
  SizeT nTrans = ToTransfer();
  DIntGDL* cVal = static_cast<DIntGDL*> (this->Convert2( GDL_INT, BaseGDL::COPY )); //necessary for non-b formats.

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  for( SizeT i=offs; i<endEl; ++i)  OutInteger( *os, (*cVal)[ i], w, d, code, oMode);
  return tCount;
}

//GDL_UINT
template<> SizeT Data_<SpDUInt>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, int code,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 16 : 7);
  SizeT nTrans = ToTransfer();
  DLongGDL* cVal = static_cast<DLongGDL*> (this->Convert2( GDL_LONG, BaseGDL::COPY )); //necessary as IDL affixes the '+' when format="+".
                                                                                       //meaning it does not pass an unsigned int!

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  for( SizeT i=offs; i<endEl; ++i)  OutInteger( *os, (*cVal)[ i], w, d, code, oMode);
  return tCount;
}
//GDL_INT
template<> SizeT Data_<SpDInt>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, int code,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 16 : 7);
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  for( SizeT i=offs; i<endEl; ++i)  OutInteger( *os, (*this)[ i], w, d, code, oMode);
  return tCount;
}

// GDL_LONG
template<> SizeT Data_<SpDLong>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, int code,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 32 : 12);
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;
  
  for( SizeT i=offs; i<endEl; ++i)  OutInteger( *os, (*this)[ i], w, d, code, oMode);
  return tCount;
}
// GDL_ULONG
template<> SizeT Data_<SpDULong>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, int code,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 32 : 12);
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  for( SizeT i=offs; i<endEl; ++i)  OutInteger( *os, (*this)[ i], w, d, code, oMode);
  return tCount;
}
// GDL_LONG64
template<> SizeT Data_<SpDLong64>::
OFmtI(ostream* os, SizeT offs, SizeT r, int w, int d, int code,
    BaseGDL::IOMode oMode)
{
  if (w < 0) w = (oMode == BIN ? 64 : 22);
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if (r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  for (SizeT i = offs; i < endEl; ++i) OutInteger(*os, (*this)[ i], w, d, code, oMode);
  return tCount;
}
// GDL_ULONG64
template<> SizeT Data_<SpDULong64>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, int code,
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = (oMode == BIN ? 64 : 22);
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  for( SizeT i=offs; i<endEl; ++i)  OutInteger( *os, (*this)[ i], w, d, code, oMode);
  return tCount;
}

template<> SizeT Data_<SpDComplex>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, int code,
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
      OutInteger( *os, static_cast<DLong64>((*this)[ firstEl++].imag()), w, d, code, oMode);
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

    for ( SizeT i = firstEl; i < endEl; ++i ) {
       OutInteger( *os, static_cast<DLong64>((*this)[ i].real()), w, d, code, oMode);
       OutInteger( *os, static_cast<DLong64>((*this)[ i].imag()), w, d, code, oMode);
    } 
  
  if( tCount & 0x01)
  {
      OutInteger( *os, static_cast<DLong64>((*this)[ endEl++].real()), w, d, code, oMode);
  }
  return tCountOut;
}
template<> SizeT Data_<SpDComplexDbl>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, int code,
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
      OutInteger( *os, static_cast<DLong64>((*this)[ firstEl++].imag()), w, d, code, oMode);
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

    for ( SizeT i = firstEl; i < endEl; ++i ) {
       OutInteger( *os, static_cast<DLong64>((*this)[ i].real()), w, d, code, oMode);
       OutInteger( *os, static_cast<DLong64>((*this)[ i].imag()), w, d, code, oMode);
    } 
  
  if( tCount & 0x01)
  {
      OutInteger( *os, static_cast<DLong64>((*this)[ endEl++].real()), w, d, code, oMode);
  }
  return tCountOut;
}

SizeT DStructGDL::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, int code,
       BaseGDL::IOMode oMode) 
{
  SizeT firstOut, firstOffs, tCount, tCountOut;
  OFmtAll( offs, r, firstOut, firstOffs, tCount, tCountOut);

  SizeT trans = (*this)[ firstOut]->OFmtI( os, firstOffs, tCount, w, d, code, oMode);
  if( trans >= tCount) return tCountOut;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstOut+1); i < ddSize; ++i)
    {
      trans = (*this)[ i]->OFmtI( os, 0, tCount, w, d, code, oMode);
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
OFmtCal( ostream* os, SizeT offs, SizeT r, int w, int d, char *f, int code, BaseGDL::Cal_IOMode cMode) 
{
  SizeT firstOut, firstOffs, tCount, tCountOut;
  OFmtAll( offs, r, firstOut, firstOffs, tCount, tCountOut);

  SizeT trans = (*this)[ firstOut]->OFmtCal( os, firstOffs, tCount, w, d, f, code, cMode);
  if( trans >= tCount) return tCountOut;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstOut+1); i < ddSize; ++i)
    {
      trans = (*this)[ i]->OFmtCal( os, 0, tCount, w, d, f, code, cMode);
      if( trans >= tCount) return tCountOut;
      tCount -= trans;
     }

  return tCountOut;
}

 template<class Sp> SizeT Data_<Sp>::
 OFmtCal( ostream* os, SizeT offs, SizeT repeat, int w, int d, char *fill, int code, BaseGDL::Cal_IOMode cMode)
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
      OutInteger( *(local_os[i]), iDay[i], 2, 2, code, BaseGDL::DEC);
      (*local_os[i]) << " ";
      OutInteger( *(local_os[i]), iHour[i], 2, 2, code, BaseGDL::DEC);
      (*local_os[i]) << ":";
      OutInteger( *(local_os[i]), iMinute[i], 2, 2, code, BaseGDL::DEC);
      (*local_os[i]) << ":";
      OutInteger( *(local_os[i]), (DLong) (Second[i]) , 2, 2, code, BaseGDL::DEC);
      std::stringbuf buffer; // empty buffer
      std::ostream os (&buffer); // associate stream buffer to stream
      os.width(6);
      os << iYear[i];
      outA( local_os[i], buffer.str().substr(buffer.str().size()-6,6), 6 ); //selects the 6 last digits of year.
      }
      break;
    case BaseGDL::STRING:
      for (SizeT i=0; i<r; i++){
      (*local_os[i]) << *fill;
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
      OutInteger( *(local_os[i]), iMonth[i]+1, w, d, code, BaseGDL::DEC);
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
      OutInteger( *(local_os[i]), iHour[i]%12, w, d, code, BaseGDL::DEC);
      }
      break;
    case BaseGDL::CHI:
      if ( w == -1 ) w = 2;
      for (SizeT i=0; i<r; i++){
      OutInteger( *(local_os[i]), iHour[i], w, d, code, BaseGDL::DEC);
      }
      break;
    case BaseGDL::CDI:
      if ( w == -1 ) w = 2;
      for (SizeT i=0; i<r; i++){
      OutInteger( *(local_os[i]), iDay[i], w, d, code, BaseGDL::DEC);
      }
      break;
    case BaseGDL::CMI:
      if ( w == -1 ) w = 2;
      for (SizeT i=0; i<r; i++){
      OutInteger( *(local_os[i]), iMinute[i], w, d, code, BaseGDL::DEC);
      }
      break;
    case BaseGDL::CSI:
      if ( w == -1 ) {
        w = 2;
        d = 0;
      }
      for (SizeT i=0; i<r; i++){
      OutInteger( *(local_os[i]), (DLong) (Second[i]), w, d, code, BaseGDL::DEC);
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
      OutFixed(*local_os[i], Second[i], w, d, code);
      }
      break;
  }
  return tCount;
 }

//#include "instantiate_templates.hpp"

#endif
