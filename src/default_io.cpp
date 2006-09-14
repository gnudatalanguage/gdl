/* *************************************************************************
                          default_io.cpp  -  input/output, no FORMAT keyword
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
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


void SkipWS( istream& is)
{
  if( is.eof())
    throw GDLIOException( "End of file encountered. "+
			StreamInfo( &is));
  char c;
  do {
    c = is.get();

    if ( (is.rdstate() & ifstream::failbit ) != 0 )
      {
	if ( (is.rdstate() & ifstream::eofbit ) != 0 )
	  throw GDLIOException( "End of file encountered. "+
			      StreamInfo( &is));

	if ( (is.rdstate() & ifstream::badbit ) != 0 )
	  throw GDLIOException( "Error reading stream. "+
			      StreamInfo( &is));
	
	is.clear();
	return ;
      }
  } while( c == ' ' || c == '\t' || c == '\n');

  is.unget();
}

// helper function - reads one line, does error checking
const string ReadElement(istream& is)
{
  SkipWS( is);

  string buf;
  char c;
  for(;;)
    {
      c = is.get();
      //    int cc = c;
      //    cout << "ReadEl: " << cc << " " << c << ":" << endl;
      
      if ( (is.rdstate() & ifstream::failbit ) != 0 )
	{
	  if ( (is.rdstate() & ifstream::badbit ) != 0 )
	    throw GDLIOException( "Error reading line. "+
				StreamInfo( &is));
	  
	  is.clear();
	  return buf;
	}

      if( c == '\n') 
	return buf;
      
      if( c == ' ' || c == '\t')
	{
	  is.unget();
	  return buf;
	}

      buf.push_back( c);
    }

  if( !is.good())
    throw GDLIOException( "Error reading stream. "+StreamInfo( &is));

  return buf;

//   // old version (read full line which is then split - does not work with
//   // different types on the same line)
//   if( is.eof())
//     throw GDLIOException( "End of file encountered. "+
// 			StreamInfo( &is));
//   string retStr;
//   getline( is, retStr);
  
//   if ( (is.rdstate() & ifstream::failbit ) != 0 )
//     {
//       if ( (is.rdstate() & ifstream::eofbit ) != 0 )
// 	throw GDLIOException( "End of file encountered. "+
// 			    StreamInfo( &is));
      
//       if ( (is.rdstate() & ifstream::badbit ) != 0 )
// 	throw GDLIOException( "Error reading line. "+
// 			    StreamInfo( &is));
      
//       is.clear();
//       return "";
//     }

//   if( !is.good())
//     throw GDLIOException( "Error reading line. "+StreamInfo( &is));
  
//   cout << "Read line: " << retStr << endl;

//   return retStr;
}

// no skip of WS
const string ReadComplexElement(istream& is)
{
  SkipWS( is);
  
  string buf;
  char c = is.get();
  if ( (is.rdstate() & ifstream::failbit ) != 0 )
    {
      if ( (is.rdstate() & ifstream::eofbit ) != 0 )
	throw GDLIOException( "End of file encountered. "+
			    StreamInfo( &is));
      if ( (is.rdstate() & ifstream::badbit ) != 0 )
	throw GDLIOException( "Error reading stream. "+
			    StreamInfo( &is));
      
      is.clear();
      return buf;
    }
  
  bool brace = (c == '(');

  if( !brace)
    {
      is.unget();
      return ReadElement( is);
    }

  buf.push_back( c);
  for(;;)
    {
      c = is.get();
      
      if ( (is.rdstate() & ifstream::failbit ) != 0 )
	{
	  if ( (is.rdstate() & ifstream::badbit ) != 0 )
	    throw GDLIOException( "Error reading line. "+
				StreamInfo( &is));
	  
	  is.clear();
	  return buf;
	}

      if( c == '\n') 
	return buf;

      buf.push_back( c);

      if( c == ')') 
	return buf;
    }
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
      const string segment = ReadElement( i);

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
      const string segment = ReadElement( i);
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
      const string segment = ReadElement( i);
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
      const string& actLine = ReadComplexElement( i);
      SizeT strLen = actLine.length();

      if( actLine[ 0] == '(')
	{
	  SizeT mid  = actLine.find_first_of(" \t,",1);
	  if( mid >= strLen) mid = strLen;
	      
	  string seg1 = actLine.substr( 1, mid-1);

	  mid++;
	  SizeT next = actLine.find_first_not_of(" \t",mid);
	  if( next >= strLen) next = strLen;

	  SizeT last = actLine.find_first_of(")",next);
	  if( last >= strLen) last = strLen;

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
	  
	  // convert segment and assign
	  const char* cStart=actLine.c_str();
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
      const string& actLine = ReadComplexElement( i);
      SizeT strLen = actLine.length();

      if( actLine[ 0] == '(')
	{
	  SizeT mid  = actLine.find_first_of(" \t,",1);
	  if( mid >= strLen) mid = strLen;
	      
	  string seg1 = actLine.substr( 1, mid-1);

	  mid++;
	  SizeT next = actLine.find_first_not_of(" \t",mid);
	  if( next >= strLen) next = strLen;

	  SizeT last = actLine.find_first_of(")",next);
	  if( last >= strLen) last = strLen;

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
	  
	  // convert segment and assign
	  const char* cStart=actLine.c_str();
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
	    throw GDLIOException( "End of file encountered. "+
				StreamInfo( &is));
      
	  if ( (is.rdstate() & ifstream::badbit ) != 0 )
	    throw GDLIOException( "Error reading STRING. "+
				StreamInfo( &is));
      
	  is.clear();
	  is.get();   // remove delimiter
	  data_.dd[ c] = "";

	  continue;
	}

      if( !is.good())
	throw GDLIOException( "Error reading STRING. "+StreamInfo( &is));
  
      if( !is.eof()) is.get(); // remove delimiter

      const string& str = ioss.str();
      if( str.length() > 0 && str[ str.length()-1] == '\r')
	data_.dd[ c] = str.substr(0,str.length()-1);
      else
	data_.dd[ c] = str;
      ioss.str("");
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
  // avoid checking actPosPtr
  SizeT dummyPos = 0;
  if( actPosPtr == NULL) actPosPtr = &dummyPos;

  SizeT nTags = NTags();
  SizeT nEl   = N_Elements();
  
  bool arrOut = false; // remember if an array was already put out

  for( SizeT e=0; e<nEl; ++e)
    {
      o << CheckNL( w, actPosPtr, 2) << "{";
      for( SizeT tIx=0; tIx<nTags-1; ++tIx)
	{
	  BaseGDL* actEl = Get( tIx, e);

	  assert( actEl != NULL);
// 	  if( actEl == NULL)
// 	    throw 
// 	      GDLException("Internal error: Output of UNDEF struct element.");
	  if( actEl->Type() == STRING)
	    o << CheckNL( w, actPosPtr, 1) << " ";
	    
	  bool isArr = (actEl->Dim().Rank() != 0);

	  if( isArr && arrOut && *actPosPtr != 0)
	    InsNL( o, actPosPtr);

	  actEl->ToStream( o, w, actPosPtr);
	  
	  if( isArr)
	    {
	      arrOut = true;
	      if( *actPosPtr != 0)
		InsNL( o, actPosPtr);
	    }
	}

      BaseGDL* actEl = Get( nTags-1, e);
      assert( actEl != NULL);
//    if( actEl == NULL)
//      throw 
//        GDLException("Internal error: Output of UNDEF struct element.");
      if( actEl->Type() == STRING)
	o << CheckNL( w, actPosPtr, 1) << " ";
      
      actEl->ToStream( o, w, actPosPtr);

      o << CheckNL( w, actPosPtr, 1) << "}";
    }
  return o;
}


int xdr_convert(XDR *xdrs, DByte *buf)
{
  return (xdr_u_char(xdrs, (unsigned char *) buf));
}

int xdr_convert(XDR *xdrs, DInt *buf)
{
  return (xdr_short(xdrs, (short int *) buf));
}

int xdr_convert(XDR *xdrs, DUInt *buf)
{
  return (xdr_u_short(xdrs, (unsigned short int *) buf));
}

int xdr_convert(XDR *xdrs, DLong *buf)
{
  return (xdr_long(xdrs, (long *) buf));
}

int xdr_convert(XDR *xdrs, DULong *buf)
{
  return (xdr_u_long(xdrs, (unsigned long *) buf));
}

int xdr_convert(XDR *xdrs, DLong64 *buf)
{
  //  return (xdr_longlong(xdrs, (long long *) buf));
}

int xdr_convert(XDR *xdrs, DULong64 *buf)
{
  //  return (xdr_u_longlong(xdrs, (unsigned long long *) buf));
}

int xdr_convert(XDR *xdrs, DFloat *buf)
{
  return (xdr_float(xdrs, (float *) buf));
}

int xdr_convert(XDR *xdrs, DDouble *buf)
{
  return (xdr_double(xdrs, (double *) buf));
}

int xdr_convert(XDR *xdrs, DComplex *buf)
{
  return (0);
}

int xdr_convert(XDR *xdrs, DComplexDbl *buf)
{
  return (0);
}



// unformatted ***************************************** 
template<class Sp>
ostream& Data_<Sp>::Write( ostream& os, bool swapEndian, XDR *xdrs)
{
  if( os.eof()) os.clear();

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
  else if (xdrs != NULL)
    {
      char* cData = reinterpret_cast<char*>(&dd[0]);
      SizeT cCount = count * sizeof(Ty);
      long fac = 1;
      if (sizeof(Ty) == 2) fac = 2;

      char buf[ cCount*fac];
      memset(buf, 0, cCount*fac);

      xdrmem_create(xdrs, buf, sizeof(buf), XDR_ENCODE);

      for( SizeT i=0; i<count; i++)
	memcpy(&buf[i*sizeof(Ty)*fac], &cData[i*sizeof(Ty)], sizeof(Ty));

      for( SizeT i=0; i<count; i++) {
	xdr_convert(xdrs, (Ty *) &buf[i*sizeof(Ty)*fac]); 
      }
 
      os.write(buf,cCount*fac);

      xdr_destroy(xdrs);
    }
  else
    {
      os.write( reinterpret_cast<char*>(&dd[0]),
		count * sizeof(Ty));
    }
  
//   if( os.eof())
//     {
//       os.clear();
//     }

  if( !os.good())
    {
//       if( os.rdstate() & istream::eofbit) cout << "eof." << endl;
//       if( os.rdstate() & istream::badbit) cout << "bad." << endl;
//       if( os.rdstate() & istream::failbit) cout << "fail." << endl;
      throw GDLIOException("Error writing data.");
    }
  return os;
}

template<class Sp>
istream& Data_<Sp>::Read( istream& os, bool swapEndian, XDR *xdrs)
{
  if( os.eof())
    throw GDLIOException("End of file encountered.");

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
  else if (xdrs != NULL)
    {
      char* cData = reinterpret_cast<char*>(&dd[0]);
      SizeT cCount = count * sizeof(Ty);
      long fac = 1;
      if (sizeof(Ty) == 2) fac = 2;

      char buf[ cCount*fac];
      memset(buf, 0, cCount*fac);

      xdrmem_create(xdrs, buf, sizeof(buf), XDR_DECODE);

      os.read(buf,cCount*fac);

      for( SizeT i=0; i<count; i++)
	xdr_convert(xdrs, (Ty *) &buf[i*sizeof(Ty)*fac]); 

      for( SizeT i=0; i<count; i++)
	memcpy(&cData[i*sizeof(Ty)], &buf[i*sizeof(Ty)*fac], sizeof(Ty));

      xdr_destroy(xdrs);
    }
  else
    {
      os.read( reinterpret_cast<char*>(&dd[0]),
		count * sizeof(Ty));
    }
  
  if( os.eof())
    throw GDLIOException("End of file encountered.");

//   if( os.eof())
//     {
//       os.clear();
//     }

  if( !os.good())
    {
      throw GDLIOException("Error reading data.");
    }

  return os;
}

template<>
ostream& Data_<SpDString>::Write( ostream& os, bool swapEndian, XDR *xdrs)
{
  if( os.eof()) os.clear();

  SizeT count = dd.size();
  
  for( SizeT i=0; i<count; i++)
    {
      if (xdrs != NULL)
	{
	  int bufsize = 8 + 4 * ((dd[i].size() - 1) / 4 + 1);
	  char buf[ bufsize];

	  // IDL adds an addition string length
	  xdrmem_create(xdrs, &buf[0], 4, XDR_ENCODE);
	  short int length = dd[i].size();
	  xdr_short(xdrs, (short int *) &length);
	  xdr_destroy(xdrs);

	  xdrmem_create(xdrs, &buf[4], bufsize-4, XDR_ENCODE);
	  char* bufptr = (char *) dd[i].c_str();
	  xdr_string(xdrs, &bufptr, dd[i].size());
	  xdr_destroy(xdrs);
	  os.write( buf, bufsize);
	}
      else
	{
	  os.write( dd[i].c_str(), dd[i].size());
	}
    }
  
//   if( os.eof()) 
//     {
//       os.clear();
//     }

  if( !os.good())
    {
      throw GDLIOException("Error writing data.");
    }  

  return os;
}

template<>
istream& Data_<SpDString>::Read( istream& os, bool swapEndian, XDR *xdrs)
{
  if( os.eof())
    throw GDLIOException("End of file encountered.");

  SizeT count = dd.size();
  
  SizeT maxLen = 1024;
  vector<char> buf( maxLen);

  int jump = 0;
  for( SizeT i=0; i<count; i++)
    {
      SizeT nChar = dd[i].size();

      if (xdrs != NULL)
	{
	  os.seekg (jump, ios::cur);

	  os.read( (char *) &nChar, 4);
	  xdrmem_create(xdrs, (char *) &nChar, 4, XDR_DECODE);
	  xdr_long(xdrs, (long *) &nChar);
	  xdr_destroy(xdrs);

	  os.seekg (4, ios::cur);

	  jump = nChar % 4;
	}

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

//   if( os.eof())
//     {
//       os.clear();
//     }
  if( os.eof())
    throw GDLIOException("End of file encountered.");

  if( !os.good())
    {
      throw GDLIOException("Error reading data.");
    }
  
  return os;
}

ostream& DStructGDL::Write( ostream& os, bool swapEndian, XDR *xdrs)
{
  SizeT count = dd.size();
  for( SizeT i=0; i<count; i++)
    dd[i]->Write( os, swapEndian, xdrs);
  return os;
}

istream& DStructGDL::Read( istream& os, bool swapEndian, XDR *xdrs)
{
  SizeT count = dd.size();
  for( SizeT i=0; i<count; i++)
    dd[i]->Read( os, swapEndian, xdrs);
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
