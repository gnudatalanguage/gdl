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
	SizeT tt=(*this)[firstTag]->ToTransfer();
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


//#include "instantiate_templates.hpp"

#endif
