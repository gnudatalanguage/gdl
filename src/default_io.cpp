/* *************************************************************************
                          default_io.cpp  -  input/output, no FORMAT keyword
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// to be included from datatypes.cpp
#ifdef INCLUDE_DEFAULT_IO_CPP

//#include "datatypes.hpp"
//#include "dstructgdl.hpp"
//#include "arrayindex.hpp"
//#include "assocdata.hpp"
//#include "io.hpp"
#include "ofmt.hpp"
#include "gdljournal.hpp"

using namespace std;

// helper function - reads one line, does error checking
const string ReadLine(istream& is)
{
  if( is.eof())
    throw GDLException( "End of file encountered. "+
			StreamInfo( &is));

  string retStr;
  getline( is, retStr);
  
  if ( (is.rdstate() & ifstream::failbit ) != 0 )
    {
      if ( (is.rdstate() & ifstream::eofbit ) != 0 )
	throw GDLException( "End of file encountered. "+
			    StreamInfo( &is));
      
      if ( (is.rdstate() & ifstream::badbit ) != 0 )
	throw GDLException( "Error reading line. "+
			    StreamInfo( &is));
      
      is.clear();
      return "";
    }

  if( !is.good())
    throw GDLException( "Error reading line. "+StreamInfo( &is));
  
  //  cout << "Read line: " << retStr << endl;

  return retStr;
}

inline void InsNL(ostream& o, SizeT* actPosPtr)
{
  o << '\n';
  if( actPosPtr != NULL) *actPosPtr = 0;
} 

// class CheckNL (ofmt.hpp)
ostream& operator<<(ostream& os, const CheckNL& c)
{
  if( c.actPosPtr == NULL) return os;
  if( (*c.actPosPtr + c.nextW) > c.width) 
    {
      os << '\n';
      *c.actPosPtr = 0;
    }

  // check if output is to journal file
  if( *c.actPosPtr == 0)
    {
      GDLStream* s = lib::get_journal();
      if( s != NULL && s->OStream() == os) os << lib::JOURNALCOMMENT;
    }

  *c.actPosPtr += c.nextW;
  return os;
} 

// *** the default input functions
template< class Sp>
istream& Data_<Sp>::FromStream( istream& i) 
{ i >> *this; return i;}

// integer types
template<class Sp> 
istream& operator>>(istream& i, Data_<Sp>& data_) 
{
  long int nTrans =  data_.dd.size();
  SizeT assignIx = 0;

  while( nTrans > 0)
    {
      const string& actLine = ReadLine( i);
      SizeT actPos = 0;
      SizeT strLen = actLine.length();

      while( actPos < strLen && nTrans > 0)
	{
	  // split string
	  SizeT first = actLine.find_first_not_of(" \t",actPos);
	  if( first >= strLen)
	    break;

	  SizeT last  = actLine.find_first_of(" \t",first);

	  if( last >= strLen) last = strLen;

	  actPos = last;

	  string segment = actLine.substr( first, last-first);
	
	  // convert segment and assign
	  const char* cStart=segment.c_str();
	  char* cEnd;
	  data_.dd[ assignIx] = strtol( cStart, &cEnd, 10);
	  if( cEnd == cStart)
	    {
	      data_.dd[ assignIx]= -1;
	      Warning("Input conversion error.");
	    }
	  
	  assignIx++;
	  nTrans--;
	}
    }

  return i;
}

// float
template<> 
istream& operator>>(istream& i, Data_<SpDFloat>& data_) 
{
  long int nTrans =  data_.dd.size();
  SizeT assignIx = 0;

  while( nTrans > 0)
    {
      const string& actLine = ReadLine( i);
      SizeT actPos = 0;
      SizeT strLen = actLine.length();

      while( actPos < strLen && nTrans > 0)
	{
	  // split string
	  SizeT first = actLine.find_first_not_of(" \t",actPos);
	  if( first >= strLen)
	    break;

	  SizeT last  = actLine.find_first_of(" \t",first);

	  if( last >= strLen) last = strLen;

	  actPos = last;

	  string segment = actLine.substr( first, last-first);
	
	  // convert segment and assign
	  const char* cStart=segment.c_str();
	  char* cEnd;
	  data_.dd[ assignIx] = strtod( cStart, &cEnd);
	  if( cEnd == cStart)
	    {
	      data_.dd[ assignIx]= -1;
	      Warning("Input conversion error.");
	    }
	  
	  assignIx++;
	  nTrans--;
	}
    }

  return i;
}
// double
template<> 
istream& operator>>(istream& i, Data_<SpDDouble>& data_) 
{
  long int nTrans =  data_.dd.size();
  SizeT assignIx = 0;

  while( nTrans > 0)
    {
      const string& actLine = ReadLine( i);
      SizeT actPos = 0;
      SizeT strLen = actLine.length();

      while( actPos < strLen && nTrans > 0)
	{
	  // split string
	  SizeT first = actLine.find_first_not_of(" \t",actPos);
	  if( first >= strLen)
	    break;

	  SizeT last  = actLine.find_first_of(" \t",first);

	  if( last >= strLen) last = strLen;

	  actPos = last;

	  string segment = actLine.substr( first, last-first);
	
	  // convert segment and assign
	  const char* cStart=segment.c_str();
	  char* cEnd;
	  data_.dd[ assignIx] = strtod( cStart, &cEnd);
	  if( cEnd == cStart)
	    {
	      data_.dd[ assignIx]= -1;
	      Warning("Input conversion error.");
	    }
	  
	  assignIx++;
	  nTrans--;
	}
    }

  return i;
}
// complex
template<> 
istream& operator>>(istream& i, Data_<SpDComplex>& data_) 
{
  long int nTrans =  data_.dd.size();
  SizeT assignIx = 0;

  while( nTrans > 0)
    {
      const string& actLine = ReadLine( i);
      SizeT actPos = 0;
      SizeT strLen = actLine.length();

      while( actPos < strLen && nTrans > 0)
	{
	  // split string
	  SizeT first = actLine.find_first_not_of(" \t",actPos);
	  if( first >= strLen)
	    break;

	  if( actLine[ first] == '(')
	    {
	      first++;

	      SizeT mid  = actLine.find_first_of(" \t,",first);
	      if( mid >= strLen) mid = strLen;
	      
	      string seg1 = actLine.substr( first, mid-first);

	      mid++;
	      SizeT next = actLine.find_first_not_of(" \t",mid);
	      if( next >= strLen) next = strLen;

	      SizeT last = actLine.find_first_of(")",next);
	      if( last >= strLen) last = strLen;

	      actPos = last;

	      if( last <= next)
		{
		  data_.dd[ assignIx]= DComplex(0.0,0.0);
		  Warning("Imaginary part of complex missing.");
		} 
	      else
		{

		  string seg2 = actLine.substr( next, last-next);

		  char* cEnd1, *cEnd2;
		  const char* c1=seg1.c_str();
		  double re = strtod( c1, &cEnd1);
		  const char* c2=seg2.c_str();
		  double im = strtod( c2, &cEnd2);
		  if( cEnd1 == c1 || cEnd2 == c2)
		    {
		      data_.dd[ assignIx]= DComplex(0.0,0.0);
		      Warning("Input conversion error.");
		    }
		  else
		    {
		      data_.dd[ assignIx] = DComplex( re, im);
		    }
		}
	    }
	  else
	    { // real part only read, all values are set to this
	      // the file pointer hangs (ie. a following
	      // float reads the same value again)

	      SizeT last  = actLine.find_first_of(" \t",first);

	      if( last >= strLen) last = strLen;

	      actPos = last;

	      string segment = actLine.substr( first, last-first);
	
	      // convert segment and assign
	      const char* cStart=segment.c_str();
	      char* cEnd;
	      double val = strtod( cStart, &cEnd);
	      if( cEnd == cStart)
		{
		  data_.dd[ assignIx]= DComplex(0.0,0.0);
		  Warning("Input conversion error.");
		}
	      
	      for( long int c=assignIx; c<nTrans; c++)
		data_.dd[ c] = DComplex(val,0.0);
	      
	      // i.seekg( pos); // rewind stream

	      return i;
	    }
	  
	  assignIx++;
	  nTrans--;
	}
    }

  return i;
}
// double complex
template<> 
istream& operator>>(istream& i, Data_<SpDComplexDbl>& data_) 
{
  long int nTrans =  data_.dd.size();
  SizeT assignIx = 0;

  while( nTrans > 0)
    {
      const string& actLine = ReadLine( i);
      SizeT actPos = 0;
      SizeT strLen = actLine.length();

      while( actPos < strLen && nTrans > 0)
	{
	  // split string
	  SizeT first = actLine.find_first_not_of(" \t",actPos);
	  if( first >= strLen)
	    break;

	  if( actLine[ first] == '(')
	    {
	      first++;

	      SizeT mid  = actLine.find_first_of(" \t,",first);
	      if( mid >= strLen) mid = strLen;
	      
	      string seg1 = actLine.substr( first, mid-first);

	      mid++;
	      SizeT next = actLine.find_first_not_of(" \t",mid);
	      if( next >= strLen) next = strLen;

	      SizeT last = actLine.find_first_of(")",next);
	      if( last >= strLen) last = strLen;

	      actPos = last;

	      if( last <= next)
		{
		  data_.dd[ assignIx]= DComplexDbl(0.0,0.0);
		  Warning("Imaginary part of complex missing.");
		} 
	      else
		{

		  string seg2 = actLine.substr( next, last-next);

		  char* cEnd1, *cEnd2;
		  const char* c1=seg1.c_str();
		  double re = strtod( c1, &cEnd1);
		  const char* c2=seg2.c_str();
		  double im = strtod( c2, &cEnd2);
		  if( cEnd1 == c1 || cEnd2 == c2)
		    {
		      data_.dd[ assignIx]= DComplexDbl(0.0,0.0);
		      Warning("Input conversion error.");
		    }
		  else
		    {
		      data_.dd[ assignIx] = DComplexDbl( re, im);
		    }
		}
	    }
	  else
	    { // real part only read, all values are set to this
	      // the file pointer hangs (ie. a following
	      // float reads the same value again)

	      SizeT last  = actLine.find_first_of(" \t",first);

	      if( last >= strLen) last = strLen;

	      actPos = last;

	      string segment = actLine.substr( first, last-first);
	
	      // convert segment and assign
	      const char* cStart=segment.c_str();
	      char* cEnd;
	      double val = strtod( cStart, &cEnd);
	      if( cEnd == cStart)
		{
		  data_.dd[ assignIx]= DComplexDbl(0.0,0.0);
		  Warning("Input conversion error.");
		}
	      
	      for( long int c=assignIx; c<nTrans; c++)
		data_.dd[ c] = DComplexDbl(val,0.0);
	      
	      // i.seekg( pos); // rewind stream

	      return i;
	    }
	  
	  assignIx++;
	  nTrans--;
	}
    }

  return i;
}

// string
template<> 
istream& operator>>(istream& is, Data_<SpDString>& data_) 
{
  stringstream ioss;

  SizeT nEl = data_.dd.size();
  for( SizeT c=0; c < nEl; c++)
    {
      is.get( *ioss.rdbuf());
      if ( (is.rdstate() & ifstream::failbit ) != 0 )
	{
	  if ( (is.rdstate() & ifstream::eofbit ) != 0 )
	    throw GDLException( "End of file encountered. "+
				StreamInfo( &is));
      
	  if ( (is.rdstate() & ifstream::badbit ) != 0 )
	    throw GDLException( "Error reading STRING. "+
				StreamInfo( &is));
      
	  is.clear();
	  is.get();   // remove delimiter
	  data_.dd[ c] = "";

	  continue;
	}

      if( !is.good())
	throw GDLException( "Error reading STRING. "+StreamInfo( &is));
  
      if( !is.eof()) is.get(); // remove delimiter

      data_.dd[ c] = ioss.str();
    }
  return is;
}

istream& operator>>(istream& i, DStructGDL& data_)
{
  SizeT nTags = data_.NTags();
  SizeT nEl   = data_.N_Elements();

  for( SizeT e=0; e<nEl; ++e)
    {
      for( SizeT tIx=0; tIx<nTags; ++tIx)
	{
	  BaseGDL* actEl = data_.Get( tIx, e);
	  if( actEl == NULL)
	    throw 
	      GDLException("Internal error: Input of UNDEF struct element.");
	  actEl->FromStream( i);
	}
    }
  return i;
}

// end input *********************************************************

// the default output functions
template<class Sp> 
ostream& Data_<Sp>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 8) << setw(8) << dd[0];
      return o;
    }

  SizeT nLoop=nElem / this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    o << CheckNL( w, actPosPtr, 8) << setw(8) << dd[eIx++];
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 8) << setw(8) << dd[eIx++];
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}

// Long
template<> 
ostream& Data_<SpDLong>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 12) << setw(12) << dd[0];
      return o;
    }

  SizeT nLoop=nElem / this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    o << CheckNL( w, actPosPtr, 12) << setw(12) << dd[eIx++];
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 12) << setw(12) << dd[eIx++];
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}

// ULong
template<> 
ostream& Data_<SpDULong>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 12) << setw(12) << dd[0];
      return o;
    }

  SizeT nLoop=nElem/this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    o << CheckNL( w, actPosPtr, 12) << setw(12) << dd[eIx++];
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 12) << setw(12) << dd[eIx++];
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}

// Long64
template<> 
ostream& Data_<SpDLong64>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 22) << setw(22) << dd[0];
      return o;
    }

  SizeT nLoop=nElem/this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    o << CheckNL( w, actPosPtr, 22) << setw(22) << dd[eIx++];
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 22) << setw(22) << dd[eIx++];
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}

// ULong64
template<> 
ostream& Data_<SpDULong64>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 22) << setw(22) << dd[0];
      return o;
    }

  SizeT nLoop=nElem/this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    o << CheckNL( w, actPosPtr, 22) << setw(22) << dd[eIx++];
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 22) << setw(22) << dd[eIx++];
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}

// ptr
template<> 
ostream& Data_<SpDPtr>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 15) << "<PtrHeapVar" << dd[0] << ">";
      return o;
    }

  SizeT nLoop=nElem/this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    o << CheckNL( w, actPosPtr, 15) << "<PtrHeapVar" << dd[eIx++] << ">";
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 15) << "<PtrHeapVar" << dd[eIx++] << ">";
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}
// obj
template<> 
ostream& Data_<SpDObj>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 15) << "<ObjHeapVar" << dd[0] << ">";
      return o;
    }

  SizeT nLoop=nElem/this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    o << CheckNL( w, actPosPtr, 15) << "<ObjHeapVar" << dd[eIx++] << ">";
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 15) << "<ObjHeapVar" << dd[eIx++] << ">";
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}
// float
template<> 
ostream& Data_<SpDFloat>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  const int prec = 6;
  const int width = 13;

  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, width); 
      OutAuto( o, dd[0], width, prec);
      return o;
    }

  SizeT nLoop=nElem/this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    {
	      o << CheckNL( w, actPosPtr, width);
	      OutAuto( o, dd[eIx++], width, prec);
	    }
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	{
	  o << CheckNL( w, actPosPtr, width); 
	  OutAuto( o, dd[eIx++], width, prec);
	}
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}
// double
template<> 
ostream& Data_<SpDDouble>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  const int prec = 8;
  const int width = 16;

  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, width); 
      OutAuto( o, dd[0], width, prec);
      return o;
    }

  SizeT nLoop=nElem/this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    {
	      o << CheckNL( w, actPosPtr, width); 
	      OutAuto( o, dd[eIx++], width, prec);
	    }
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	{
	  o << CheckNL( w, actPosPtr, width); 
	  OutAuto( o, dd[eIx++], width, prec);
	}
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}

template<> 
ostream& Data_<SpDComplex>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  const int prec = 6;
  const int width = 13;

  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplex>( dd[0], width, prec);
      return o;
    }

  SizeT nLoop=nElem/this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplex>( dd[eIx++], width, prec);
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplex>( dd[eIx++], width, prec);
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}
template<> 
ostream& Data_<SpDComplexDbl>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  const int prec = 8;
  const int width = 16;

  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplexDbl>( dd[0], width, prec);
      return o;
    }

  SizeT nLoop=nElem/this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplexDbl>( dd[eIx++], width, prec);
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplexDbl>( dd[eIx++], width, prec);
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}

// byte (c++ does output as characters)
template<> 
ostream& Data_<SpDByte>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  const int width = 4;

  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, width) << setw(width) << static_cast<int>(dd[0]);
      return o;
    }

  SizeT nLoop=nElem/this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=0; i1<d1; i1++)
	{
	  for( SizeT i0=0; i0<d0; i0++)
	    o << CheckNL( w, actPosPtr, width) << setw(width) << static_cast<int>(dd[eIx++]);
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, width) << setw(width) << static_cast<int>(dd[eIx++]);
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}

// strings
template<> 
ostream& Data_<SpDString>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  SizeT length;
  if( this->dim.Rank() == 0)
    {
      length = dd[0].length();
      o << CheckNL( w, actPosPtr, length) << dd[0];
      return o;
    }

  SizeT nLoop=nElem/this->dim.Stride(2);
  SizeT eIx=0; // linear counter
  SizeT d0=this->Dim(0); 
  SizeT d1=this->Dim(1);

  // d0 cannot be 0
  if( d1 == 0) d1 = 1;

  for( SizeT l=1; l<nLoop; l++)
    {
      for( SizeT i1=1; i1<d1; i1++)
	{
	  for( SizeT i0=1; i0<d0; i0++)
	    {
	      length = dd[eIx].length() + 1;
	      if( length > 1)
		o << CheckNL( w, actPosPtr, length) << dd[eIx++] << " ";
	      else eIx++;
	    }
	  length = dd[eIx].length();
	  if( length > 0)
	    o << CheckNL( w, actPosPtr, length) << dd[eIx++]; 
	  else eIx++;
	  InsNL( o, actPosPtr);
	}

      for( SizeT i0=1; i0<d0; i0++)
	{
	  length = dd[eIx].length() + 1;
	  if( length > 1)
	    o << CheckNL( w, actPosPtr, length) << dd[eIx++] << " ";
	  else eIx++;
	}
      length = dd[eIx].length();
      if( length > 0)
	o << CheckNL( w, actPosPtr, length) << dd[eIx++]; 
      else eIx++;
      InsNL( o, actPosPtr);
      
      InsNL( o, actPosPtr);
    }

  for( SizeT i1=1; i1<d1; i1++)
    {
      for( SizeT i0=1; i0<d0; i0++)
	{
	  length = dd[eIx].length() + 1;
	  if( length > 1)
	    o << CheckNL( w, actPosPtr, length) << dd[eIx++] << " ";
	  else eIx++;
	}
      length = dd[eIx].length();
      if( length > 0)
	o << CheckNL( w, actPosPtr, length) << dd[eIx++]; 
      else eIx++;
      InsNL( o, actPosPtr);
    }
  
  for( SizeT i0=1; i0<d0; i0++)
    {
      length = dd[eIx].length() + 1;
      if( length > 1)
	o << CheckNL( w, actPosPtr, length) << dd[eIx++] << " ";
      else eIx++;
    }
  length = dd[eIx].length();
  if( length > 0)
    o << CheckNL( w, actPosPtr, length) << dd[eIx++];
  else eIx++;
  InsNL( o, actPosPtr);
  
  return o;
}

ostream& DStructGDL::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  SizeT nTags = NTags();
  SizeT nEl   = N_Elements();
  
  for( SizeT e=0; e<nEl; ++e)
    {
      o << CheckNL( w, actPosPtr, 1) << "{";
      for( SizeT tIx=0; tIx<nTags-1; ++tIx)
	{
	  BaseGDL* actEl = Get( tIx, e);
	  if( actEl == NULL)
	    throw 
	      GDLException("Internal error: Output of UNDEF struct element.");
	  actEl->ToStream( o, w, actPosPtr);
	  if( actEl->Dim().Rank() != 0) InsNL( o, actPosPtr);
	}

      BaseGDL* actEl = Get( nTags-1, e);
      if( actEl == NULL)
	throw 
	  GDLException("Internal error: Output of UNDEF struct element.");
      actEl->ToStream( o, w, actPosPtr);

      o << CheckNL( w, actPosPtr, 1) << "}";
    }
  return o;
}


// unformatted ***************************************** 
template<class Sp>
ostream& Data_<Sp>::Write( ostream& os, bool swapEndian)
{
  SizeT count = dd.size();

  if( swapEndian && (sizeof(Ty) != 1))
    {
      char* cData = reinterpret_cast<char*>(&dd[0]);
      SizeT cCount = count * sizeof(Ty);

      char swap[ sizeof(Ty)];
      for( SizeT i=0; i<cCount; i += sizeof(Ty))
	{
	  SizeT src = i+sizeof(Ty)-1;

	  for( SizeT dst=0; dst<sizeof(Ty); dst++)
	    swap[dst] = cData[ src--];
	  
	  os.write(swap,sizeof(Ty));
	}
    }
  else
    {
      os.write( reinterpret_cast<char*>(&dd[0]),
		count * sizeof(Ty));
    }
  
  if( os.eof())
    {
      os.clear();
    }

  if( !os.good())
    {
//       if( os.rdstate() & istream::eofbit) cout << "eof." << endl;
//       if( os.rdstate() & istream::badbit) cout << "bad." << endl;
//       if( os.rdstate() & istream::failbit) cout << "fail." << endl;
      throw GDLException("Error writing data.");
    }
  return os;
}

template<class Sp>
istream& Data_<Sp>::Read( istream& os, bool swapEndian)
{
  SizeT count = dd.size();
  
  if( swapEndian && (sizeof(Ty) != 1))
    {
      char* cData = reinterpret_cast<char*>(&dd[0]);
      SizeT cCount = count * sizeof(Ty);

      char swap[ sizeof(Ty)];
      for( SizeT i=0; i<cCount; i += sizeof(Ty))
	{
	  os.read(swap,sizeof(Ty));

	  SizeT src = i+sizeof(Ty)-1;

	  for( SizeT dst=0; dst<sizeof(Ty); dst++)
	    cData[ src--] = swap[dst];
	}
    }
  else
    {
      os.read( reinterpret_cast<char*>(&dd[0]),
		count * sizeof(Ty));
    }
  
  if( os.eof())
    {
      os.clear();
    }

  if( !os.good())
    {
      throw GDLException("Error reading data.");
    }

  return os;
}

template<>
ostream& Data_<SpDString>::Write( ostream& os, bool swapEndian)
{
  SizeT count = dd.size();
  
  for( SizeT i=0; i<count; i++)
    {
      os.write( dd[i].c_str(), dd[i].size());
    }
  
  if( os.eof()) 
    {
      os.clear();
    }

  if( !os.good())
    {
      throw GDLException("Error writing data.");
    }  

  return os;
}

template<>
istream& Data_<SpDString>::Read( istream& os, bool swapEndian)
{
  SizeT count = dd.size();
  
  SizeT maxLen = 1024;
  vector<char> buf( maxLen);

  for( SizeT i=0; i<count; i++)
    {
      SizeT nChar = dd[i].size();

      if( nChar > 0)
	{
	  if( nChar > maxLen)
	    {
	      maxLen = nChar;
	      buf.resize( maxLen);
	    }
	  
	  os.read(&buf[0],nChar);
	  
	  dd[i].assign(&buf[0],nChar);
	}
    }

  if( os.eof())
    {
      os.clear();
    }

  if( !os.good())
    {
      throw GDLException("Error reading data.");
    }
  
  return os;
}

template<class Sp>
BaseGDL* Data_<Sp>::AssocVar( int lun, SizeT offset)
{
  return new Assoc_<Data_>( lun, this, offset);
}

BaseGDL* DStructGDL::AssocVar( int lun, SizeT offset)
{
  return new Assoc_<DStructGDL >( lun, this, offset);
}

//#include "instantiate_templates.hpp"

#endif
