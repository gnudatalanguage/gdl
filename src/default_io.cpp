/**************************************************************************
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
//#include "arrayindex.hpp".
//#include "assocdata.hpp"
//#include "io.hpp"
#include "ofmt.hpp"
#include "gdljournal.hpp"
#include "list.hpp"
#include "hash.hpp"

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
  if( (*c.actPosPtr + c.nextW) > c.width && *c.actPosPtr > 0) 
    {
      os << '\n';
      *c.actPosPtr = 0;
    }

  // check if output is to journal file
  if( *c.actPosPtr == 0)
    {
      GDLStream* s = lib::get_journal();
      if( s != NULL && s->OStream().rdbuf() == os.rdbuf()) os << lib::JOURNALCOMMENT;
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
      data_[ assignIx] = strtol( cStart, &cEnd, 10);
      if( cEnd == cStart)
	{
	  data_[ assignIx]= -1;
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
      data_[ assignIx] = StrToD( cStart, &cEnd);
      if( cEnd == cStart)
	{
	  data_[ assignIx]= -1;
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
      data_[ assignIx] = StrToD( cStart, &cEnd);
      if( cEnd == cStart)
	{
	  data_[ assignIx]= -1;
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
	      data_[ assignIx]= DComplex(0.0,0.0);
	      Warning("Imaginary part of complex missing.");
	    } 
	  else
	    {
	      
	      string seg2 = actLine.substr( next, last-next);
	      
	      char* cEnd1, *cEnd2;
	      const char* c1=seg1.c_str();
	      double re = StrToD( c1, &cEnd1);
	      const char* c2=seg2.c_str();
	      double im = StrToD( c2, &cEnd2);
	      if( cEnd1 == c1 || cEnd2 == c2)
		{
		  data_[ assignIx]= DComplex(0.0,0.0);
		  Warning("Input conversion error.");
		}
	      else
		{
		  data_[ assignIx] = DComplex( re, im);
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
	  double val = StrToD( cStart, &cEnd);
	  if( cEnd == cStart)
	    {
	      data_[ assignIx]= DComplex(0.0,0.0);
	      Warning("Input conversion error.");
	    }
	  
	  for( long int c=assignIx; c<nTrans; c++)
	    data_[ c] = DComplex(val,0.0);
	  
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
	      data_[ assignIx]= DComplexDbl(0.0,0.0);
	      Warning("Imaginary part of complex missing.");
	    } 
	  else
	    {
	      
	      string seg2 = actLine.substr( next, last-next);
	      
	      char* cEnd1, *cEnd2;
	      const char* c1=seg1.c_str();
	      double re = StrToD( c1, &cEnd1);
	      const char* c2=seg2.c_str();
	      double im = StrToD( c2, &cEnd2);
	      if( cEnd1 == c1 || cEnd2 == c2)
		{
		  data_[ assignIx]= DComplexDbl(0.0,0.0);
		  Warning("Input conversion error.");
		}
	      else
		{
		  data_[ assignIx] = DComplexDbl( re, im);
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
	  double val = StrToD( cStart, &cEnd);
	  if( cEnd == cStart)
	    {
	      data_[ assignIx]= DComplexDbl(0.0,0.0);
	      Warning("Input conversion error.");
	    }
	  
	  for( long int c=assignIx; c<nTrans; c++)
	    data_[ c] = DComplexDbl(val,0.0);
	  
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

    char delim = '\n';
    streampos startPos = is.tellg();
    bool checkForCROnly = true;
    goto start;

rerunCR:
    delim = '\r';
    is.seekg( startPos);
    ioss.str("");

start:
    for( SizeT c=0; c < nEl; c++)
    {
//      is.get( *ioss.rdbuf());
        is.get( *ioss.rdbuf(), delim);

        // error handling
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
            data_[ c] = "";

            continue;
        }

        if( !is.good() && !is.eof())
            throw GDLIOException( "Error reading STRING. "+StreamInfo( &is));

        if( !is.eof()) is.get(); // remove delimiter

        const string& str = ioss.str();

        if( checkForCROnly)
        {
            // do only once
            checkForCROnly = false;

            SizeT posCR = str.find( '\r');
            if( posCR != string::npos && posCR != str.length()-1)
            {
                goto rerunCR;
            }
        }

        // handle \r\n (\n not read)
        if( delim == '\n' && str.length() > 0 && str[ str.length()-1] == '\r')
        {
            data_[ c] = str.substr(0,str.length()-1);
        }
        else
        {
            data_[ c] = str;
        }

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
	  BaseGDL* actEl = data_.GetTag( tIx, e);
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

  o << right;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 8) << setw(8) << (*this)[0];
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
	    o << CheckNL( w, actPosPtr, 8) << setw(8) << (*this)[eIx++];
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 8) << setw(8) << (*this)[eIx++];
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

  o << right;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 12) << setw(12) << (*this)[0];
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
	    o << CheckNL( w, actPosPtr, 12) << setw(12) << (*this)[eIx++];
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 12) << setw(12) << (*this)[eIx++];
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

  o << right;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 12) << setw(12) << (*this)[0];
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
	    o << CheckNL( w, actPosPtr, 12) << setw(12) << (*this)[eIx++];
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 12) << setw(12) << (*this)[eIx++];
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

  o << right;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 22) << setw(22) << (*this)[0];
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
	    o << CheckNL( w, actPosPtr, 22) << setw(22) << (*this)[eIx++];
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 22) << setw(22) << (*this)[eIx++];
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

  o << right;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 22) << setw(22) << (*this)[0];
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
	    o << CheckNL( w, actPosPtr, 22) << setw(22) << (*this)[eIx++];
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 22) << setw(22) << (*this)[eIx++];
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}

// ptr
void HeapVarString(ostream& o, DPtr ptr)
{
  if( ptr != 0)
  {
    o << "<PtrHeapVar" << ptr << ">";
  }
  else
    o << "<NullPointer>";
}
template<> 
ostream& Data_<SpDPtr>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  o << left;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 15);
      HeapVarString( o, (*this)[0]);
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
	    o << CheckNL( w, actPosPtr, 15);
	    HeapVarString( o, (*this)[eIx++]);
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
	o << CheckNL( w, actPosPtr, 15);
	HeapVarString( o, (*this)[eIx++]);
      }
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}
// obj
void ObjHeapVarString(ostream& o, DObj obj)
{
  if( obj != 0)
  {
    DStructGDL* oStructGDL= GDLInterpreter::GetObjHeapNoThrow( obj);
    if( oStructGDL != NULL)
    {
      o << "<ObjHeapVar" << obj << "("<< oStructGDL->Desc()->Name() <<")>";      
    }
    else    
      o << "<ObjHeapVar" << obj << "(*INVALID*)>";
  }
  else
    o << "<NullObject>";
}
template<> 
ostream& Data_<SpDObj>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  static bool recursive = false;
  if( this->StrictScalar() && !recursive)
  {
    DObj s = dd[0]; // is StrictScalar()
    if( s != 0)  // no overloads for null object
    {
      DStructGDL* oStructGDL= GDLInterpreter::GetObjHeapNoThrow( s);
      if( oStructGDL != NULL) // if object not valid -> default behaviour
      {  
	DStructDesc* desc = oStructGDL->Desc();

	if( desc->IsParent("LIST"))
	{
	  recursive = true;
	  try{
	    LIST__ToStream(oStructGDL,o,w,actPosPtr);
	    recursive = false;
	  } catch( ...)
	  {
	    recursive = false;
	    throw;
	  }
	  
	  return o;
	}
	if( desc->IsParent("HASH"))
	{
	  recursive = true;
	  try{
	    HASH__ToStream(oStructGDL,o,w,actPosPtr);
	    recursive = false;
	  } catch( ...)
	  {
	    recursive = false;
	    throw;
	  }
	  
	  return o;
	}
      }
    }
  }

  SizeT nElem=this->Size();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  o << left;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 15);
      ObjHeapVarString( o, (*this)[0]);
//       o << CheckNL( w, actPosPtr, 15) << "<ObjHeapVar" << (*this)[0] << ">";
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
	    o << CheckNL( w, actPosPtr, 15);
	    ObjHeapVarString( o, (*this)[eIx++]);
// 	    o << CheckNL( w, actPosPtr, 15) << "<ObjHeapVar" << (*this)[eIx++] << ">";
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
	o << CheckNL( w, actPosPtr, 15);
	ObjHeapVarString( o, (*this)[eIx++]);
// 	o << CheckNL( w, actPosPtr, 15) << "<ObjHeapVar" << (*this)[eIx++] << ">";
      }
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

  o << right;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, width); 
      OutAuto( o, (*this)[0], width, prec, 0);
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
	      OutAuto( o, (*this)[eIx++], width, prec, 0);
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
	  OutAuto( o, (*this)[eIx++], width, prec, 0);
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

  o << right;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, width); 
      OutAuto( o, (*this)[0], width, prec, 0);
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
	      OutAuto( o, (*this)[eIx++], width, prec, 0);
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
	  OutAuto( o, (*this)[eIx++], width, prec, 0);
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
  const char fill = ' ';

  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  o << right;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplex>( (*this)[0], width, prec, fill);
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
	    o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplex>( (*this)[eIx++], width, prec, fill);
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplex>( (*this)[eIx++], width, prec, fill);
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
  const char fill = ' ';

  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  o << right;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplexDbl>( (*this)[0], width, prec, fill);
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
	    o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplexDbl>( (*this)[eIx++], width, prec, fill);
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, 2*width+3) << AsComplex< DComplexDbl>( (*this)[eIx++], width, prec, fill);
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

  o << right;

  if( this->dim.Rank() == 0)
    {
      o << CheckNL( w, actPosPtr, width) << setw(width) << static_cast<int>((*this)[0]);
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
	    o << CheckNL( w, actPosPtr, width) << setw(width) << static_cast<int>((*this)[eIx++]);
	  InsNL( o, actPosPtr);
	}
      InsNL( o, actPosPtr);
    }

  // last block (no '\n' at the end)
  for( SizeT i1=0; i1<d1; i1++)
    {
      for( SizeT i0=0; i0<d0; i0++)
	o << CheckNL( w, actPosPtr, width) << setw(width) << static_cast<int>((*this)[eIx++]);
      //      if( (i1+1) < d1) InsNL( o, actPosPtr);
      InsNL( o, actPosPtr);
    }
  return o;
}

// strings
template<> 
ostream& Data_<SpDString>::ToStream(ostream& o, SizeT w, SizeT* actPosPtr) 
{
  bool someCharacterSeen=false;
  SizeT nElem=N_Elements();
  if( nElem == 0)
    throw GDLException("Variable is undefined.");

  o << left;

  SizeT length;
  if( this->dim.Rank() == 0)
    {
      length = (*this)[0].length();
      o << CheckNL( w, actPosPtr, length) << (*this)[0];
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
	      length = (*this)[eIx].length() + 1;
	      if( length > 1)
		o << CheckNL( w, actPosPtr, length) << (*this)[eIx++] << " ";
	      else eIx++;
	    }
	  length = (*this)[eIx].length();
	  if( length > 0)
	    o << CheckNL( w, actPosPtr, length) << (*this)[eIx++]; 
	  else eIx++;
	  InsNL( o, actPosPtr);
	}

      for( SizeT i0=1; i0<d0; i0++)
	{
	  length = (*this)[eIx].length() + 1;
	  if( length > 1)
	    o << CheckNL( w, actPosPtr, length) << (*this)[eIx++] << " ";
	  else eIx++;
	}
      length = (*this)[eIx].length();
      if( length > 0)
		o << CheckNL( w, actPosPtr, length) << (*this)[eIx++]; 
      else eIx++;
      InsNL( o, actPosPtr);
      
      InsNL( o, actPosPtr);
    }

  for ( SizeT i1 = 1; i1 < d1; i1++ ) {
    someCharacterSeen=false;
    for ( SizeT i0 = 1; i0 < d0; i0++ ) {
	  length = (*this)[eIx].length() + 1;
      if( length > 1)  someCharacterSeen=true;
      // for array output a space should be inserted e.g. a=strarr(9)&a[8]=':'&a[0]='>'&aa=[[a],[a]]&print,aa
      // actually, blanks are inserted only between the first non-null character and the last non-null character. 
      //see a=strarr(9)&a[6]=':'&a[1]='>'&aa=[[a],[a]]&print,aa
      if (someCharacterSeen) o << CheckNL( w, actPosPtr, length ) << (*this)[eIx++] << " ";
      else eIx++;
	}
      length = (*this)[eIx].length();
      if( length > 0)
		o << CheckNL( w, actPosPtr, length) << (*this)[eIx++]; 
      else eIx++;
      InsNL( o, actPosPtr);
    }
  someCharacterSeen=false;
  for ( SizeT i0 = 1; i0 < d0; i0++ ) {
      length = (*this)[eIx].length() + 1;
    // for array output a space should be inserted e.g. a=strarr(9)&a[8]=':'&a[0]='>'&aa=[[a],[a]]&print,aa
    // actually, blanks are inserted only between the first non-null character and the last non-null character. 
    //see a=strarr(9)&a[6]=':'&a[1]='>'&aa=[[a],[a]]&print,aa
    if( length > 1)  someCharacterSeen=true;
    if (someCharacterSeen) o << CheckNL( w, actPosPtr, length ) << (*this)[eIx++] << " ";
    else eIx++;
    }
  length = (*this)[eIx].length();
  if( length > 0)
    o << CheckNL( w, actPosPtr, length) << (*this)[eIx++];
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
	  BaseGDL* actEl = GetTag( tIx, e);

	  assert( actEl != NULL);
// 	  if( actEl == NULL)
// 	    throw 
// 	      GDLException("Internal error: Output of UNDEF struct element.");
	  if( actEl->Type() == GDL_STRING)
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

      BaseGDL* actEl = GetTag( nTags-1, e);
      assert( actEl != NULL);
//    if( actEl == NULL)
//      throw 
//        GDLException("Internal error: Output of UNDEF struct element.");
      if( actEl->Type() == GDL_STRING)
	o << CheckNL( w, actPosPtr, 1) << " ";
      
      actEl->ToStream( o, w, actPosPtr);

      o << CheckNL( w, actPosPtr, 1) << "}";
    }
  return o;
}

ostream& DStructGDL::ToStreamRaw( ostream& o) {
  // avoid checking actPosPtr
  SizeT dummyPos = 0;

  SizeT nTags = NTags( );
  SizeT nEl = N_Elements( );

  bool arrOut = false; // remember if an array was already put out

  for ( SizeT e = 0; e < nEl; ++e ) {
    for ( SizeT tIx = 0; tIx < nTags - 1; ++tIx ) {
      BaseGDL* actEl = GetTag( tIx, e );
      assert( actEl != NULL );
      bool isArr = (actEl->Dim( ).Rank( ) != 0);
      actEl->ToStream( o, 0, &dummyPos );
      if ( isArr ) arrOut = true;
    }

    BaseGDL* actEl = GetTag( nTags - 1, e );
    assert( actEl != NULL );
    actEl->ToStream( o, 0, &dummyPos );
  }
  return o;
}
//this is the routined used by IDL as per the documentation.
bool_t xdr_complex( XDR *xdrs, DComplex *p)  
{ 
  return(xdr_float(xdrs, reinterpret_cast<float *>(p)) && xdr_float(xdrs, reinterpret_cast<float *>(p) + 1));
}  
//this is the routined used by IDL as per the documentation.
bool_t xdr_dcomplex(XDR *xdrs, DComplexDbl *p)  
{  
  return(xdr_double(xdrs, reinterpret_cast<double *>(p)) && xdr_double(xdrs, reinterpret_cast<double *>(p) + 1));
}

//Alternate version, having problems on windows (??).
////this is the routined used by IDL as per the documentation.
//bool_t xdr_complex( XDR *xdrs, DComplex *p)  
//{ 
////Don't you EVER change this code. Refer to gnudatalanguage-devel@lists.sourceforge.net for suggestions/blames.
//  DFloat preal = p->real();
//  DFloat pimag = p->imag();
////this convoluted code should prevent compilation problem with MINGW's GCC
//  bool ret1=xdr_float(xdrs, &preal);
//  bool ret2=xdr_float(xdrs, &pimag);
//  p->real()=preal;
//  p->imag()=pimag;
//  return( (ret1) && (ret2));
//}  
////this is the routined used by IDL as per the documentation.
//bool_t xdr_dcomplex(XDR *xdrs, DComplexDbl *p)  
//{  
////Don't you EVER change this code. Refer to gnudatalanguage-devel@lists.sourceforge.net for suggestions/blames.
//  DDouble preal = p->real();
//  DDouble pimag = p->imag();
////this convoluted code should prevent compilation problem with MINGW's GCC
//  bool ret1=xdr_double(xdrs, &preal);
//  bool ret2=xdr_double(xdrs, &pimag);
//  p->real()=preal;
//  p->imag()=pimag;
//  return( (ret1) && (ret2));
//} 
//

//this is the routined used by IDL as per the documentation.
//It happens to write twice the number of chars in the file, not very clever.
bool_t xdr_counted_string(XDR *xdrs, char **p)  
{  
  int input = (xdrs->x_op == XDR_DECODE);  
  short length;  
  
  /* If writing, obtain the length */  
  if (!input) length = strlen(*p);  
  
  /* Transfer the string length */  
  if (!xdr_short(xdrs, (short *) &length)) return(FALSE);  
  /* If reading, obtain room for the string */  
  if (input)  
  {  
      *p = (char*) malloc((unsigned) (length + 1));  
         (*p)[length] = '\0'; /* Null termination */  
  }  
  /* If the string length is nonzero, transfer it */  
  return(length ? xdr_string(xdrs, p, length) : TRUE);  
}

int xdr_convert(XDR *xdrs, DByte *buf)
{
  assert(false); return 0;
}

int xdr_convert(XDR *xdrs, DInt *buf)
{
  return (xdr_short(xdrs, buf));
}

int xdr_convert(XDR *xdrs, DUInt *buf)
{
  return (xdr_u_short(xdrs, buf));
}

int xdr_convert(XDR *xdrs, DLong *buf)
{
#if defined(__APPLE__) && defined(__LP64__)
  /* xdr_long actually takes an int on 64bit darwin */
  return (xdr_long(xdrs, buf));
#else
   return (xdr_int(xdrs, buf));
#endif
}

int xdr_convert(XDR *xdrs, DULong *buf)
{
#if defined(__APPLE__) && defined(__LP64__)
  /* xdr_u_long actually takes an unsigned int on 64bit darwin */
  return (xdr_u_long(xdrs, buf));
#else
  return (xdr_u_int(xdrs, buf));
#endif
}

int xdr_convert(XDR *xdrs, DLong64 *buf)
{
  return (xdr_longlong_t(xdrs, (quad_t *)(buf)));
}

int xdr_convert(XDR *xdrs, DULong64 *buf)
{
  return (xdr_u_longlong_t(xdrs, (u_quad_t *) (buf)));
}

int xdr_convert(XDR *xdrs, DFloat *buf)
{
  return (xdr_float(xdrs, buf));
}

int xdr_convert(XDR *xdrs, DDouble *buf)
{
  return (xdr_double(xdrs, buf));
}

int xdr_convert(XDR *xdrs, DComplex *buf)
{
  return ( xdr_complex(xdrs, buf)) ;
}

int xdr_convert(XDR *xdrs, DComplexDbl *buf)
{
  return ( xdr_dcomplex(xdrs, buf)) ;
}



// unformatted ***************************************** 
template<class Sp>
ostream& Data_<Sp>::Write( ostream& os, bool swapEndian, bool compress, XDR *xdrs ) {
  if ( os.eof( ) ) os.clear( );

  SizeT count = dd.size( );

  if ( swapEndian && (sizeof (Ty) != 1) ) {
    char* cData = reinterpret_cast<char*> (&(*this)[0]);
    SizeT cCount = count * sizeof (Ty);
    if ( Data_<Sp>::IS_COMPLEX ) {
      char *swapBuf = (char*) malloc( sizeof (char) * sizeof (Ty) / 2 );
      for ( SizeT i = 0; i < cCount; i += sizeof (Ty) / 2 ) {
        SizeT src = i + sizeof (Ty) / 2 - 1;
        for ( SizeT dst = 0; dst<sizeof (Ty) / 2; dst++ ) swapBuf[dst]=cData[ src--];
        os.write( swapBuf, sizeof (Ty) / 2 );
      }
      free( swapBuf );
    } else {
      char swapBuf[ sizeof (Ty)];
      for ( SizeT i = 0; i < cCount; i += sizeof (Ty) ) {
        SizeT src = i + sizeof (Ty) - 1;
        for ( SizeT dst = 0; dst<sizeof (Ty); dst++ ) swapBuf[dst] = cData[ src--];
        os.write( swapBuf, sizeof (Ty) );
      }
    }
  } else if ( xdrs != NULL ) {
    long fac = 1;
    if ( sizeof (Ty) == 2 ) fac = 2;
    SizeT bufsize = sizeof (Ty)*fac;
    char *buf = (char *) calloc( bufsize, sizeof (char) );
    for ( SizeT i = 0; i < count; i++ ) {
      xdrmem_create( xdrs, buf, bufsize, XDR_ENCODE );
      if ( !xdr_convert( xdrs, (&(*this)[i]) ) ) cerr << "Error in XDR write" << endl;
      xdr_destroy( xdrs );
      os.write( buf, bufsize );
    }
    free( buf );
  } else if (compress)
  {
    (static_cast<ogzstream&>(os)).write(reinterpret_cast<char*> (&(*this)[0]), count * sizeof (Ty));
    if (!(static_cast<ogzstream&> (os)).good())
    {
      throw GDLIOException("Error writing data.");
    }
  } else {
    os.write( reinterpret_cast<char*> (&(*this)[0]), count * sizeof (Ty) );
  }

  if ( !os.good( ) ) {
    throw GDLIOException( "Error writing data." );
  }
  return os;
}

template<>
ostream& Data_<SpDByte>::Write( ostream& os, bool swapEndian,
bool compress, XDR *xdrs ) {
  if ( os.eof( ) ) os.clear( );

  SizeT count = dd.size( );

  if ( xdrs != NULL ) {
    int bufsize = 4 + 4 * ((count - 1) / 4 + 1);
    char *buf = (char *) calloc( bufsize, sizeof (char) );

    // XDR adds an addition string length
    xdrmem_create( xdrs, &buf[0], 4, XDR_ENCODE );
    short int length = count;
    if ( !xdr_short( xdrs, (short int *) &length ) ) cerr << "Error in XDR write" << endl;
    xdr_destroy( xdrs );
    //do it ourselves
    for ( SizeT i = 0; i < count; i++ ) buf[i + 4] = (*this)[i];
    os.write( buf, bufsize );
    free( buf );
  } else if (compress)
  {
    (static_cast<ogzstream&>(os)).write( reinterpret_cast<char*> (&(*this)[0]), count );
    if (!(static_cast<ogzstream&> (os)).good())
    {
      throw GDLIOException("Error writing data.");
    }
  } else {
    os.write( reinterpret_cast<char*> (&(*this)[0]), count );
  }

  if ( !os.good( ) ) {
    throw GDLIOException( "Error writing data." );
  }

  return os;
}

template<>
ostream& Data_<SpDString>::Write( ostream& os, bool swapEndian,
bool compress, XDR *xdrs ) {
  if ( os.eof( ) ) os.clear( );

  SizeT count = dd.size( );

  for ( SizeT i = 0; i < count; i++ ) {
    if ( xdrs != NULL ) {
      int bufsize = 8 + 4 * (((*this)[i].size( ) - 1) / 4 + 1);
      char *buf = (char *) malloc( bufsize * sizeof (char) );
      xdrmem_create( xdrs, &buf[0], bufsize, XDR_ENCODE );
      char* bufptr = (char *) (*this)[i].c_str( );
      if ( !xdr_counted_string( xdrs, &bufptr ) ) cerr << "Error in XDR write" << endl;
      xdr_destroy( xdrs );
      os.write( buf, bufsize );
      free( buf );
    } else if (compress)
    {
      (static_cast<ogzstream&> (os)).write((*this)[i].c_str( ), (*this)[i].size( ));
      if (!(static_cast<ogzstream&>(os)).good())
      {
        throw GDLIOException("Error writing data.");
      }
    } else {
      os.write( (*this)[i].c_str( ), (*this)[i].size( ) );
    }
  }

  if ( !os.good( ) ) {
    throw GDLIOException( "Error writing data." );
  }

  return os;
}

template<class Sp>
istream& Data_<Sp>::Read( istream& os, bool swapEndian,
bool compress, XDR *xdrs ) {
  if ( os.eof( ) )
    throw GDLIOException( "End of file encountered." );

  SizeT count = dd.size( );

  if ( swapEndian && (sizeof (Ty) != 1) ) {
    char* cData = reinterpret_cast<char*> (&(*this)[0]);
    SizeT cCount = count * sizeof (Ty);

    if ( Data_<Sp>::IS_COMPLEX ) {
      char *swapBuf = (char*) malloc( sizeof (char) * sizeof (Ty) / 2 );
      for ( SizeT i = 0; i < cCount; i += sizeof (Ty) / 2 ) {
        os.read( swapBuf, sizeof (Ty) / 2 );
        SizeT src = i + sizeof (Ty) / 2 - 1;
        for ( SizeT dst = 0; dst<sizeof (Ty) / 2; dst++ ) cData[ src--] = swapBuf[dst];
      }
      free( swapBuf );
    } else {
      char swapBuf[ sizeof (Ty)];
      for ( SizeT i = 0; i < cCount; i += sizeof (Ty) ) {
        os.read( swapBuf, sizeof (Ty) );
        SizeT src = i + sizeof (Ty) - 1;
        for ( SizeT dst = 0; dst<sizeof (Ty); dst++ ) cData[ src--] = swapBuf[dst];
      }
    }
  } else if ( xdrs != NULL ) {
    long fac = 1;
    if ( sizeof (Ty) == 2 ) fac = 2;
    SizeT bufsize = sizeof (Ty)*fac;
    char *buf = (char *) calloc( bufsize, sizeof (char) );
    for ( SizeT i = 0; i < count; i++ ) {
      xdrmem_create( xdrs, buf, bufsize, XDR_DECODE );
      os.read( buf, bufsize );
      if ( !xdr_convert( xdrs, (&(*this)[i]) ) ) cerr << "Error in XDR read" << endl;
      xdr_destroy( xdrs );
    }
    free( buf );
  } else if ( compress )
    /* GD: minimum (?) hack since we want to keep trace of the position in gzipped stream.*/
  {
    int typesize=sizeof(Ty);
    SizeT totCount = count * typesize;
    char curVal[typesize];    
    for (SizeT i = 0; i < count; ++i) 
    { 
      for (int k=0; k<typesize; ++k) os.get(curVal[k]);
      char* cData = reinterpret_cast<char*>(&(*this)[i]);
      for (int k=0; k<typesize; ++k) cData[k]=curVal[k];
    }
    (static_cast<igzstream&> (os)).rdbuf()->incrementPosition(totCount); //ugly patch to maintain position        
//was:    (static_cast<igzstream&>(os)).read( reinterpret_cast<char*> (&(*this)[0]), count * sizeof (Ty) );
  } else {
    os.read( reinterpret_cast<char*> (&(*this)[0]), count * sizeof (Ty) );
  }

  if ( os.eof( ) )
    throw GDLIOException( "End of file encountered." );

  if ( !os.good( ) ) {
    throw GDLIOException( "Error reading data." );
  }

  return os;
}

template<>
istream& Data_<SpDByte>::Read( istream& os, bool swapEndian, bool compress, XDR *xdrs ) {
  if ( os.eof( ) )
    throw GDLIOException( "End of file encountered." );

  SizeT count = dd.size( );

  if ( xdrs != NULL ) {
    unsigned int nChar = this->N_Elements( );

    //read byte length in file
    char* buf = (char *) malloc( 4 );
    os.read( buf, 4 );
    xdrmem_create( xdrs, &buf[0], 4, XDR_DECODE );
    short int length = 0;
    if ( !xdr_short( xdrs, &length ) ) throw GDLIOException( "Problem reading XDR file." );
    xdr_destroy( xdrs );
    free( buf );
    if ( length <= 0 ) return os;

    int bufsize = 4 * ((length - 1) / 4 + 1);
    buf = (char *) calloc( length, sizeof (char) );
    os.read( &buf[0], bufsize );
    if ( !os.good( ) ) throw GDLIOException( "Problem reading XDR file." ); //else we are correctly aligned for next read!
    //do it by ourselves, faster and surer!
    if ( bufsize < nChar ) nChar = bufsize; //truncate eventually
    for ( SizeT i = 0; i < nChar; i++ ) ( *this )[i] = buf[i];
    free( buf );
  } else if ( compress ) {
    /* GD: minimum (?) hack since we want to keep trace of the position in gzipped stream.*/
    char* cData = reinterpret_cast<char*> (&(*this)[0]);
    for (SizeT i = 0; i < count; ++i) (static_cast<igzstream&> (os)).get(cData[ i]);
    (static_cast<igzstream&> (os)).rdbuf()->incrementPosition(count); //ugly patch to maintain position
//    (static_cast<igzstream&>(os)).read( reinterpret_cast<char*> (&(*this)[0]), count );
  } else {
    os.read( reinterpret_cast<char*> (&(*this)[0]), count );
  }

  if ( os.eof( ) )
    throw GDLIOException( "End of file encountered." );

  if ( !os.good( ) ) {
    throw GDLIOException( "Error reading data." );
  }
  return os;
}

template<>
istream& Data_<SpDString>::Read( istream& os, bool swapEndian,
bool compress, XDR *xdrs ) {
  if ( os.eof( ) )
    throw GDLIOException( "End of file encountered." );

  SizeT count = dd.size( );

  for ( SizeT i = 0; i < count; i++ ) {
    SizeT nChar = (*this)[i].size( );

    if ( xdrs != NULL ) {
      //read counted string length in file
      char* buf = (char *) malloc( 4 );
      os.read( buf, 4 );
      xdrmem_create( xdrs, &buf[0], 4, XDR_DECODE );
      short int length = 0;
      if ( !xdr_short( xdrs, &length ) ) throw GDLIOException( "Problem reading XDR file." );
      xdr_destroy( xdrs );
      free( buf );
      if ( length <= 0 ) {
        (*this)[i].clear();
      } else {
        int bufsize = 4 + 4 * ((length - 1) / 4 + 1) ;
        buf = (char *) calloc( length, sizeof (char) );
        os.read( &buf[0], bufsize );
        if ( !os.good( ) ) throw GDLIOException( "Problem reading XDR file." ); //else we are correctly aligned for next read!
        (*this)[i].assign( &buf[4], length );
      }
    } else {

      SizeT maxLen = 1024;
      vector<char> vbuf( maxLen );

      if ( nChar > 0 ) {
        if ( nChar > maxLen ) {
          maxLen = nChar;
          vbuf.resize( maxLen );
        }
        if ( compress ) {
        /* GD: minimum (?) hack since we want to keep trace of the position in gzipped stream.*/
          char c;
          vbuf.clear( );
          for ( SizeT i = 0; i < nChar; i++ ) {
            (static_cast<igzstream&>(os)).get( c ); //which does nothing more than os.get(c)...
            vbuf.push_back( c );
          }
          (static_cast<igzstream&>(os)).rdbuf()->incrementPosition(nChar);  //ugly patch to maintain position        
        } else {
          os.read( &vbuf[0], nChar );
        }
        (*this)[i].assign( &vbuf[0], nChar );
      }
    }
  }

  if ( os.eof( ) )
    throw GDLIOException( "End of file encountered." );

  if ( !os.good( ) ) {
    throw GDLIOException( "Error reading data." );
  }

  return os;
}

ostream& DStructGDL::Write( ostream& os, bool swapEndian,
bool compress, XDR *xdrs ) {
  SizeT nEl = N_Elements( );
  SizeT nTags = NTags( );
  for ( SizeT i = 0; i < nEl; ++i )
    for ( SizeT t = 0; t < nTags; ++t )
      GetTag( t, i )->Write( os, swapEndian, compress, xdrs );
  return os;
}

istream& DStructGDL::Read( istream& os, bool swapEndian,
bool compress, XDR *xdrs ) {
  SizeT nEl = N_Elements( );
  SizeT nTags = NTags( );
  for ( SizeT i = 0; i < nEl; ++i )
    for ( SizeT t = 0; t < nTags; ++t )
      GetTag( t, i )->Read( os, swapEndian, compress, xdrs );
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
