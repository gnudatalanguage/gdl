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


#include "assocdata.hpp" //for Assoc
#include "dinterpreter.hpp"

// needed with gcc-3.3.2
#include <cassert>


#include "ofmt.hpp"
#include "gdljournal.hpp"
#include "list.hpp"
#include "hash.hpp"

using namespace std;

#define GET_NEXT_CHAR     c = is.get(); \
    if ((is.rdstate() & ifstream::failbit) != 0) {\
      if ((is.rdstate() & ifstream::eofbit) != 0)  throw GDLIOException("End of file encountered. " +  StreamInfo(&is));\
      if ((is.rdstate() & ifstream::badbit) != 0)  throw GDLIOException("Error reading stream. " +     StreamInfo(&is));\
      is.clear();\
      return;\
    }

#define GET_NEXT_CHAR_RETURN_BUF_AT_LF_OR_EOF     c = is.get(); \
    if ((is.rdstate() & ifstream::failbit) != 0) {\
      if ((is.rdstate() & ifstream::badbit) != 0)  throw GDLIOException("Error reading stream. " +     StreamInfo(&is));\
      is.clear();\
      return buf;\
    }

void SkipWS( istream& is) {
  if (is.eof())
    throw GDLIOException("End of file encountered. " +  StreamInfo(&is));
  char c;
  do {
    GET_NEXT_CHAR
  } while (c == ' ' || c == '\t' || c == '\n' || c == '\r'); //special check if line is terminated with <CR><LF>:
#ifndef _WIN32
  if (c == '\r') {
    int c = is.peek();
    if (c != EOF) {
      char next = is.get();
      if (next != '\n') is.unget(); //CRLF case: gobble the \cr of crlf if not WIN32 
    }
  }
#endif
  is.unget();
}

const string ReadStringElement(istream& is) {
//  SkipWS(is); //blanks are part of strings, and returned string can be ""

  string buf;
  char c;
  for (;;) {
    GET_NEXT_CHAR_RETURN_BUF_AT_LF_OR_EOF

    if (c == '\r') {
      //test if <cr><lf> or just <cr> (old macos data)
      int p = is.peek();
      if (p != EOF) {
        char next = is.get();
        if (next != '\n') is.unget(); // test CRLF case 
      }
      return buf; // either <CR> or <CR><LF>
    }
    if (c == '\n') return buf; //simple linux <LF> ending
    buf.push_back(c); //grows buf with not line-ending chars
  }

  if (!is.good()) throw GDLIOException("Error reading stream. " + StreamInfo(&is));

  return buf;

}
const string ReadElement(istream& is) {
  SkipWS(is); //not string: skip until a non-blank, non \lf char appears. 

  string buf;
  char c;
  for (;;) {
    GET_NEXT_CHAR_RETURN_BUF_AT_LF_OR_EOF

    if (c == '\r') {
      //test if <cr><lf> or just <cr> (old macos data)
      int p = is.peek();
      if (p != EOF) {
        char next = is.get();
        if (next != '\n') is.unget(); // test CRLF case 
      }
      return buf; // either <CR> or <CR><LF>
    }
    if (c == '\n') return buf; //simple linux <LF> ending
    if( c == ' ' || c == '\t') { //Element is cut at whitespace
      is.unget(); //whitespace pertains to the next section of is (can be a string).
      return buf;
    } 
    buf.push_back(c); //grows buf with not line-ending chars
  }

  if (!is.good()) throw GDLIOException("Error reading stream. " + StreamInfo(&is));

  return buf;

}
// no skip of WS
const string ReadComplexElement(istream& is) {

  SkipWS(is);

  string buf;
  char c;
  GET_NEXT_CHAR_RETURN_BUF_AT_LF_OR_EOF

  bool brace = (c == '(');

  if (!brace) {
    is.unget();
    return ReadElement(is);
  }

  buf.push_back(c);
  for (;;) {
    GET_NEXT_CHAR_RETURN_BUF_AT_LF_OR_EOF

    if (c == '\r') {
      //test if <cr><lf> or just <cr> (old macos data)
      int p = is.peek();
      if (p != EOF) {
        char next = is.get();
        if (next != '\n') is.unget(); // test CRLF case 
      }
      return buf; // either <CR> or <CR><LF>
    }
    if (c == '\n') return buf;

    buf.push_back(c);

    if (c == ')') return buf;
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
	  ThrowGDLException("Input conversion error.");
	}
	  
      assignIx++;
      nTrans--;
    }

  return i;
}

// integer types
template<> 
istream& operator>>(istream& i, Data_<SpDString>& data_) 
{
  long int nTrans =  data_.dd.size();
  SizeT assignIx = 0;

  while( nTrans > 0)
    {
      const string segment = ReadStringElement( i);

      data_[ assignIx] = segment;
	  
      assignIx++;
      nTrans--;
    }

  return i;
}
// float : uses StrToD (character D in string)
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
	  ThrowGDLException("Input conversion error.");
	}
	  
      assignIx++;
      nTrans--;
    }

  return i;
}
// double : uses StrToD (character D in string)
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
	  ThrowGDLException("Input conversion error.");
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
      SizeT skip=0;
      if( actLine[ 0] == '(') skip=1;
		const char* cstring=actLine.c_str(); //actline is 1 or 2 numeric values, but the eventual comma ',' has disappeared.
		char* pos=(char*)cstring;
		pos+=skip; //skip eventual "("
		char* oldpos=pos;
		double re=StrToD( pos, &pos);
		if (pos==oldpos) {
		  data_[ assignIx]= DComplex(0.0,0.0);
		  ThrowGDLException("Input conversion error.");
		} 
		oldpos=pos;
		double im=StrToD( pos, &pos);
		if (pos==oldpos) {
		  data_[ assignIx]= DComplex(re,0.0); //no exception in this case see IDL: a=complex(20,10) & s="(12,)"& reads,s,a
		}
		data_[ assignIx] = DComplex( re, im);
	  
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
	SizeT skip = 0;
	if (actLine[ 0] == '(') skip = 1;
	const char* cstring = actLine.c_str(); //actline is 1 or 2 numeric values, but the eventual comma ',' has disappeared.
	char* pos = (char*) cstring;
	pos += skip; //skip eventual "("
	char* oldpos = pos;
	double re = StrToD(pos, &pos);
	if (pos == oldpos) {
	  data_[ assignIx] = DComplexDbl(0.0, 0.0);
	  ThrowGDLException("Input conversion error.");
	}
	oldpos = pos;
	double im = StrToD(pos, &pos);
	if (pos == oldpos) {
	  data_[ assignIx] = DComplexDbl(re, 0.0); //no exception in this case see IDL: a=complex(20,10) & s="(12,)"& reads,s,a
	}
	data_[ assignIx] = DComplexDbl(re, im);
      assignIx++;
      nTrans--;
    }

  return i;
}

static const std::string trimmable(" \t");
//leading blanks

  inline void trim1(std::string &s) {
    std::size_t found = s.find_first_not_of(trimmable);
    if (found != std::string::npos)
      s.erase(0, found);
    else
      s.clear();
  }
  //trailing blanks

  inline void trim0(std::string &s) {
    std::size_t found = s.find_last_not_of(trimmable);
    if (found != std::string::npos)
      s.erase(found + 1);
    else
      s.clear();
  }

  inline void trim2(std::string &s) {
    trim0(s);
    trim1(s);
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
      if (actEl->Type() == GDL_STRING) {//remove starting and ending blanks
        trim2((*(static_cast<DStringGDL*>(actEl)))[0]);
      }
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
      length = (*this)[0].length();  //app
      // this is correct but gives bug #2876161 : if (length ==0) o << '\n'; else  
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

//copy of ToStream only useable (?) as a helper for widget_table. Incidentally, all tags are scalars.
ostream& DStructGDL::ToStreamRaw( ostream& o) {
  SizeT dummyPos = 0;

  SizeT nTags = NTags( );
  SizeT nEl = N_Elements( );

  bool arrOut = false; // remember if an array was already put out

  for ( SizeT e = 0; e < nEl; ++e ) {
    for ( SizeT tIx = 0; tIx < nTags - 1; ++tIx ) {
      BaseGDL* actEl = GetTag( tIx, e );
      assert( actEl != NULL );
      if( actEl->Type() == GDL_STRING ) { 
		DString s=(*static_cast<DStringGDL*>(actEl))[0];
	 if (s.size() ==0) o << CheckNL( 0, &dummyPos, 1) << " "; //Ugly patch replacing a "" string by a whitespace - hopefully suffices to get correct tables. 
		// (ToStream would not write anything in the stream for an empty string, but FromStream would not then detect an empty string: next best option is a whitespace)
	  }
      bool isArr = (actEl->Dim( ).Rank( ) != 0);
      actEl->ToStream( o, 0, &dummyPos );
      if ( isArr ) arrOut = true;
    }

    BaseGDL* actEl = GetTag( nTags - 1, e );
    assert( actEl != NULL );
      if( actEl->Type() == GDL_STRING ) {
		DString s=(*static_cast<DStringGDL*>(actEl))[0];
	 if (s.size() ==0) o << CheckNL( 0, &dummyPos, 1) << " "; //see above
	  }
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
  return (xdr_int64_t(xdrs, (int64_t *)(buf)));
}

int xdr_convert(XDR *xdrs, DULong64 *buf)
{
  return (xdr_uint64_t(xdrs, (uint64_t *) (buf)));
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
istream& Data_<Sp>::Read( istream& is, bool swapEndian,
bool compress, XDR *xdrs ) {
  if ( is.eof( ) )
    throw GDLIOException( "End of file encountered." );

  SizeT count = dd.size( );

  if ( swapEndian && (sizeof (Ty) != 1) ) {
    char* cData = reinterpret_cast<char*> (&(*this)[0]);
    SizeT cCount = count * sizeof (Ty);

    if ( Data_<Sp>::IS_COMPLEX ) {
      char *swapBuf = (char*) malloc( sizeof (char) * sizeof (Ty) / 2 );
      for ( SizeT i = 0; i < cCount; i += sizeof (Ty) / 2 ) {
        is.read( swapBuf, sizeof (Ty) / 2 );
        SizeT src = i + sizeof (Ty) / 2 - 1;
        for ( SizeT dst = 0; dst<sizeof (Ty) / 2; dst++ ) cData[ src--] = swapBuf[dst];
      }
      free( swapBuf );
    } else {
      char swapBuf[ sizeof (Ty)];
      for ( SizeT i = 0; i < cCount; i += sizeof (Ty) ) {
        is.read( swapBuf, sizeof (Ty) );
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
      is.read( buf, bufsize );
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
      for (int k=0; k<typesize; ++k) is.get(curVal[k]);
      char* cData = reinterpret_cast<char*>(&(*this)[i]);
      for (int k=0; k<typesize; ++k) cData[k]=curVal[k];
    }
    (static_cast<igzstream&> (is)).rdbuf()->incrementPosition(totCount); //ugly patch to maintain position        
//was:    (static_cast<igzstream&>(is)).read( reinterpret_cast<char*> (&(*this)[0]), count * sizeof (Ty) );
  } else {
    is.read( reinterpret_cast<char*> (&(*this)[0]), count * sizeof (Ty) );
  }

  if ( is.eof( ) )
    throw GDLIOException( "End of file encountered." );

  if ( !is.good( ) ) {
    throw GDLIOException( "Error reading data." );
  }

  return is;
}

template<>
istream& Data_<SpDByte>::Read( istream& is, bool swapEndian, bool compress, XDR *xdrs ) {
  if ( is.eof( ) )
    throw GDLIOException( "End of file encountered." );

  SizeT count = dd.size( );

  if ( xdrs != NULL ) {
    unsigned int nChar = this->N_Elements( );

    //read byte length in file
    char* buf = (char *) malloc( 4 );
    is.read( buf, 4 );
    xdrmem_create( xdrs, &buf[0], 4, XDR_DECODE );
    short int length = 0;
    if ( !xdr_short( xdrs, &length ) ) {free( buf ); throw GDLIOException( "Problem reading XDR file." );}
    xdr_destroy( xdrs );
    free( buf );
    if ( length <= 0 ) return is;

    int bufsize = 4 * ((length - 1) / 4 + 1);
    buf = (char *) calloc( bufsize, sizeof (char) );
    is.read( &buf[0], bufsize );
    if ( !is.good( ) ) {free( buf ); throw GDLIOException( "Problem reading XDR file." );} //else we are correctly aligned for next read!
    //do it by ourselves, faster and surer!
    if ( bufsize < nChar ) nChar = bufsize; //truncate eventually
    for ( SizeT i = 0; i < nChar; i++ ) ( *this )[i] = buf[i];
    free( buf );
  } else if ( compress ) {
    /* GD: minimum (?) hack since we want to keep trace of the position in gzipped stream.*/
    char* cData = reinterpret_cast<char*> (&(*this)[0]);
    for (SizeT i = 0; i < count; ++i) (static_cast<igzstream&> (is)).get(cData[ i]);
    (static_cast<igzstream&> (is)).rdbuf()->incrementPosition(count); //ugly patch to maintain position
//    (static_cast<igzstream&>(os)).read( reinterpret_cast<char*> (&(*this)[0]), count );
  } else {
//    is.sync(); //in theory should permit to take into account an external change to the file, but this is not working.
    is.read( reinterpret_cast<char*> (&(*this)[0]), count );
  }

  if ( is.eof( ) )
    throw GDLIOException( "End of file encountered." );

  if ( !is.good( ) ) {
    throw GDLIOException( "Error reading data." );
  }
  return is;
}

template<>
istream& Data_<SpDString>::Read( istream& is, bool swapEndian,
bool compress, XDR *xdrs ) {
  if ( is.eof( ) )
    throw GDLIOException( "End of file encountered." );

  SizeT count = dd.size( );

  for ( SizeT i = 0; i < count; i++ ) {
    SizeT nChar = (*this)[i].size( );

    if ( xdrs != NULL ) {
      //read counted string length in file
      char* buf = (char *) malloc( 4 );
      is.read( buf, 4 );
      xdrmem_create( xdrs, &buf[0], 4, XDR_DECODE );
      short int length = 0;
      if ( !xdr_short( xdrs, &length ) ) throw GDLIOException( "Problem reading XDR file." );
      xdr_destroy( xdrs );
      free( buf );
      if ( length <= 0 ) {
        (*this)[i].clear();
      } else {
        int bufsize = 4 + 4 * ((length - 1) / 4 + 1) ;
        buf = (char *) calloc( bufsize, sizeof (char) );
        is.read( &buf[0], bufsize );
        if ( !is.good( ) ) throw GDLIOException( "Problem reading XDR file." ); //else we are correctly aligned for next read!
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
            (static_cast<igzstream&>(is)).get( c ); //which does nothing more than os.get(c)...
            vbuf.push_back( c );
          }
          (static_cast<igzstream&>(is)).rdbuf()->incrementPosition(nChar);  //ugly patch to maintain position        
        } else {
          is.read( &vbuf[0], nChar );
        }
        //we have read nChar but this (a std::string) may be filled with ASCII NULL chars. These MUST be removed if we want the rest of GDL behave correctly on string manipulations (strlen, where etc...)
        while (vbuf[nChar-1]==0 && nChar>1) {nChar--;}
	(*this)[i].assign( &vbuf[0], nChar );
      }
    }
  }

  if ( is.eof( ) )
    throw GDLIOException( "End of file encountered." );

  if ( !is.good( ) ) {
    throw GDLIOException( "Error reading data." );
  }

  return is;
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

istream& DStructGDL::Read( istream& is, bool swapEndian,
bool compress, XDR *xdrs ) {
  SizeT nEl = N_Elements( );
  SizeT nTags = NTags( );
  for ( SizeT i = 0; i < nEl; ++i )
    for ( SizeT t = 0; t < nTags; ++t )
      GetTag( t, i )->Read( is, swapEndian, compress, xdrs );
  return is;
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

#include "instantiate_templates.hpp"
