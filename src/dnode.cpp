/***************************************************************************
                             dnode.cpp  -  GDL's AST is made of DNodes
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

#include "includefirst.hpp"

#include <string>
#include <limits>

#include <antlr/TreeParser.hpp>
#include <antlr/Token.hpp>

#include "dnode.hpp"
#include "datatypes.hpp"
#include "arrayindexlistt.hpp"
#include "dinterpreter.hpp"

#include "objects.hpp" // SA: fun/proList for warning about obsolete-routine calls

#include <assert.h>

using namespace std;

namespace antlr {

  RefAST ConvertAST( ProgNodeP p) 
  { 
    if( p == NULL) return TreeParser::ASTNULL;

    RefDNode refNode = RefDNode( new DNode);
    refNode->setType( p->getType());
    refNode->setText( p->getText());
    refNode->SetLine( p->getLine());
    
    return static_cast<antlr::RefAST>( refNode);
  }
}

DInterpreter* ProgNode::interpreter;

DNode::DNode( const DNode& cp): 
//   keepRight( false),
  CommonAST( cp), //down(), right(), 
  lineNumber( cp.getLine()), cData(NULL), 
  var(cp.var),
  libFun( cp.libFun),
  libPro( cp.libPro),
  arrIxList(NULL),arrIxListNoAssoc(NULL), 
  labelStart( cp.labelStart), labelEnd( cp.labelEnd)
{
  if( cp.cData != NULL) cData = cp.cData->Dup();
  if( cp.arrIxList != NULL)
    arrIxList = cp.arrIxList->Clone();
  if( cp.arrIxListNoAssoc != NULL)
    arrIxListNoAssoc = cp.arrIxListNoAssoc->Clone();
//     arrIxList = new ArrayIndexListT( *cp.arrIxList);

  initInt = cp.initInt;
}

ArrayIndexListT* DNode::CloneArrIxList() 
  { 
    if( arrIxList == NULL) 
      return NULL;
    return arrIxList->Clone();
  }
ArrayIndexListT* DNode::CloneArrIxNoAssocList() 
  { 
    if( arrIxListNoAssoc == NULL) 
      return NULL;
    return arrIxListNoAssoc->Clone();
  }

void DNode::ResetCData( BaseGDL* newCData)
{ 
  //  delete cData; // as used (UMinus, ARRARYDEF_CONST) deletion is done automatically 
  cData = newCData;
}

void DNode::RemoveNextSibling()
{
  right = static_cast<BaseAST*>(static_cast<AST*>(antlr::nullAST));
}


DNode::~DNode()
  {
    // delete cData in case this node is a constant
    if( (getType() == GDLTokenTypes::CONSTANT) && cData != NULL)
      {
	GDLDelete(cData);
      }
    if( (getType() == GDLTokenTypes::ARRAYIX))
      {
	delete arrIxList;
	delete arrIxListNoAssoc;
      }
}

/**
 * @brief converts a series of (hexa)decimal values to a <T> (all integer types, all BASES).
 * the text passed is guaranteed without unwanted characters due to the action of the ANTLR PARSER.
 * for decimal values (base 10) the value is strctly positive. For other bases, it is not.
 * strings can be longer than the max size allowed for a type (e.g a='800000000000000000000000'xb must pass, but signal overflow)
 * 
 * so:
 * for long ulong and longlong we use strtol() variants that signals overflows, any base
 * for others, we have apparently to do it ourself
 * @param out the converted value, a <T>
 * @param base 16 if hexa.
 * @return 
 */
template<typename T> bool DNode::Text2Number(T& out, int base) {
  throw GDLException(this->getLine(),4, "Text2Number called on unsupported type.");
}

template<> bool DNode::Text2Number(DLong& out, int base) {
  if (base == 16 && text.size() > sizeof ( DLong)*2) throw GDLException(this->getLine(),4, "Int hexadecimal constant can only have 8 digits.");
  bool noOverflow = true;
  DLong number = 0;
  if (base != 10) { //hexa or oct decoding, can give a negative result.

    for (unsigned i = 0; i < text.size(); ++i) {
      char c = text[i];
      if (c >= '0' && c <= '9') {
        c -= '0';
      } else if (c >= 'a' && c <= 'f') {
        c -= 'a' - 10;
      } else {
        c -= 'A' - 10;
      }

      DULong64 newNumber = base * number + c;
      // no overflow possible for base hex, filtered above
      number = newNumber;
    }
  } else { //dec decoding, always positive
    for (unsigned i = 0; i < text.size(); ++i) { //base is <= 10, no hexa chars!
      char c = text[i] - '0';
      DLong64 newNumber = base * number + c;

      // check for overflow
      if (newNumber > std::numeric_limits<DLong>::max()) {
        noOverflow = false;
      }

      number = newNumber;
    }
  }
  out = number;
  return noOverflow;
}

template<> bool DNode::Text2Number(DULong& out, int base) {
  bool noOverflow = true;
  char *endptr;
  errno = 0; /* To distinguish success/failure after call */
  out = strtoul(text.c_str(), &endptr, base);
  if (errno == ERANGE) noOverflow = false;
  return noOverflow;
}
//for some reason, strtoll and strtoull do not behave correctly when decoding hexa: z='8000000000000001'xll (valid) gives an overflow.
template<> bool DNode::Text2Number(DULong64& out, int base) {
  bool noOverflow = true;
  DULong64 number = 0;
  
  if (base==10) { //value is positive
    for (unsigned i = 0; i < text.size(); ++i) { //base is <= 10, no hexa chars!
      char c = text[i] - '0';
      DULong64 newNumber = base * number + c;

      // check for overflow, this works for ULL but is not needed for other types.
      if ((newNumber-c)/base != number) {
        noOverflow = false;
        break;
      }

      number = newNumber;
    }
  } else {
    //Just insure it is not too big
    if (text.size() > sizeof (DLong64)*2) noOverflow=false;

    for (unsigned i = 0; i < text.size(); ++i) {
      char c = text[i];
      if (c >= '0' && c <= '9') {
        c -= '0';
      } else if (c >= 'a' && c <= 'f') {
        c -= 'a' - 10;
      } else {
        c -= 'A' - 10;
      }

      number = base * number + c;
    }
  }
  out = number;
  return noOverflow;
}
template<> bool DNode::Text2Number(DLong64& out, int base) {
  bool noOverflow = true;
  DLong64 number = 0;
  
  if (base==10) { //value is positive
    for (unsigned i = 0; i < text.size(); ++i) { //base is <= 10, no hexa chars!
      char c = text[i] - '0';
      DULong64 newNumber = base * number + c;

      // check for overflow, this works for ULL but is not needed for other types.
      if ((newNumber-c)/base != number) {
        noOverflow = false;
        break;
      }

      number = newNumber;
    }
  } else {
    //Just insure it is not too big
    if (text.size() > sizeof (DLong64)*2) noOverflow=false;

    for (unsigned i = 0; i < text.size(); ++i) {
      char c = text[i];
      if (c >= '0' && c <= '9') {
        c -= '0';
      } else if (c >= 'a' && c <= 'f') {
        c -= 'a' - 10;
      } else {
        c -= 'A' - 10;
      }

      number = base * number + c;
    }
  }
  out = number;
  return noOverflow;
}
//Byte is unsigned, easy
template<> bool DNode::Text2Number(DByte& out, int base) {
  bool noOverflow = true;

  DByte number = 0;

  for (unsigned i = 0; i < text.size(); ++i) {
    char c = text[i];
    if (c >= '0' && c <= '9') {
      c -= '0';
    } else if (c >= 'a' && c <= 'f') {
      c -= 'a' - 10;
    } else {
      c -= 'A' - 10;
    }

    DInt newNumber = base * number + c;

    // check for overflow
    if (newNumber > 255) {
      noOverflow = false;
    }

    number = newNumber;
  }
  out = number;

  return noOverflow;
}
//UInt is unsigned, easy
template<> bool DNode::Text2Number(DUInt& out, int base) {
  bool noOverflow = true;

  DUInt number = 0;

  for (unsigned i = 0; i < text.size(); ++i) {
    char c = text[i];
    if (c >= '0' && c <= '9') {
      c -= '0';
    } else if (c >= 'a' && c <= 'f') {
      c -= 'a' - 10;
    } else {
      c -= 'A' - 10;
    }

    DULong newNumber = base * number + c;

    // check for overflow
    if (newNumber > std::numeric_limits<DUInt>::max()) {
      noOverflow = false;
    }

    number = newNumber;
  }
  out = number;

  return noOverflow;
}
// ints are signed. Only the abs() value is passed to this function in decimal, but not in Hexa.
template<> bool DNode::Text2Number(DInt& out, int base) {
  if (base == 16 && text.size() > sizeof ( DInt)*2) throw GDLException(this->getLine(),4, "Int hexadecimal constant can only have 4 digits.");

  bool noOverflow = true;

  DInt number = 0;
  if (base != 10) { //hexa or oct decoding, can give a negative result.

    for (unsigned i = 0; i < text.size(); ++i) {
      char c = text[i];
      if (c >= '0' && c <= '9') {
        c -= '0';
      } else if (c >= 'a' && c <= 'f') {
        c -= 'a' - 10;
      } else {
        c -= 'A' - 10;
      }

      DLong newNumber = base * number + c;

      // check for overflow, as soon as the DLong is more than 2 max of Dint.  
      if (newNumber > 2 * std::numeric_limits<DInt>::max() - 1) {
        noOverflow = false;
      }

      number = newNumber;
    }
  } else { //dec decoding, always positive
    for (unsigned i = 0; i < text.size(); ++i) { //base is <= 10, no hexa chars!
      char c = text[i] - '0';
      DLong newNumber = base * number + c;

      // check for overflow
      if (newNumber > std::numeric_limits<DInt>::max()) {
        noOverflow = false;
      }

      number = newNumber;
    }
  }
  out = number;
  return noOverflow;
}

void DNode::Text2Byte(int base)
{
  // cout << "byte" << endl;
  DByte val;
  if (Text2Number( val, base)==false) throw GDLException(this->getLine(), 4, "Byte constant must be less than 256.");
  cData=new DByteGDL(val);
}

// promote: make Long (Long64) if number is to large, 1 if overflow
void DNode::Text2Int(int base, bool promote)
{
  static const DLong64 maxDInt =
    static_cast<DLong64> (numeric_limits<DInt>::max());
  static const DLong64 maxDLong =
    static_cast<DLong64> (numeric_limits<DLong>::max());

  if( promote)
    {
    DLong64 ll;
      if (Text2Number( ll, base)==false) { cData=new DLong64GDL(-1); return;}

      if( ll <= maxDInt && ll >= -maxDInt )
	{
      DInt val = static_cast<DInt> (ll);
      cData = new DIntGDL(val);
	}
      else if( ll <= maxDLong && ll >= -maxDLong)
	{
      DLong val = static_cast<DLong> (ll);
      cData = new DLongGDL(val);
	}
      else
	{
      cData = new DLong64GDL(ll);
    }
    }
  else
    {
    DInt val;
      if (Text2Number( val, base)==false) throw GDLException(this->getLine(),4,  "Integer constant must be less than 32768.");
    cData = new DIntGDL(val);
  }
}
void DNode::Text2UInt(int base, bool promote)
{
  static const DULong64 maxDUInt=
    static_cast<DULong64>(numeric_limits<DUInt>::max());
  static const DULong64 maxDULong=
    static_cast<DULong64>(numeric_limits<DULong>::max());

  if( promote)
    {
      DULong64 ll;
      Text2Number( ll, base);
      
      if( ll <= maxDUInt)
	{
	  DUInt val = static_cast<DUInt>(ll);
	  cData=new DUIntGDL( val);
	}
      else if( ll <= maxDULong)
	{
	  DULong val = static_cast<DULong>(ll);
	  cData=new DULongGDL( val);
	}
      else
	{
	  cData=new DULong64GDL( ll);
	}
    }
  else
    {
      DUInt val;
      if (Text2Number( val, base)==false) throw GDLException(this->getLine(),4,  "Unsigned integer constant must be less than 65536.");
      cData=new DUIntGDL(val);
    }
}

void DNode::Text2Long(int base, bool promote) {
  static const DLong64 maxDLong =
    static_cast<DLong64> (numeric_limits<DLong>::max());

  if (promote) {
    DLong64 ll;
    Text2Number(ll, base);

    if (ll <= maxDLong && ll>= -maxDLong ) {
      DLong val = static_cast<DLong> (ll);
      cData = new DLongGDL(val);
    } else {
      cData = new DLong64GDL(ll);
    }
    return;
  }
	
  if (base == 16) {
      if( text.size() > sizeof( DLong)*2) 
	throw GDLException(this->getLine(),4,  "Long hexadecimal constant can only have "+
			    i2s(sizeof( DLong)*2)+" digits.");

      DLong val;
      if (Text2Number( val, base)==false) throw GDLException(this->getLine(),4,  "Long integer constant must be less than 2147483648.");
      cData=new DLongGDL(val);
      return;
    }
	
  DLong64 val;
  bool noOverFlow = Text2Number( val, base);

  if( !noOverFlow || val > std::numeric_limits< DLong>::max())
    throw GDLException(this->getLine(),4,  "Long integer constant must be less than 2147483648.");

  cData=new DLongGDL(val);
}

void DNode::Text2ULong(int base, bool promote) 
{
  static const DULong64 maxDULong=
    static_cast<DULong64>(numeric_limits<DULong>::max());

  if( promote)
    {
      DULong64 ll;
      Text2Number( ll, base);
      
      if( ll <= maxDULong) {
	  DULong val = static_cast<DULong>(ll);
	  cData=new DULongGDL( val);
	}
      else
	{
	  cData=new DULong64GDL( ll);
	}
      return;
    }
  
  if( base == 16)
    {
      if( text.size() > sizeof(DULong)*2)
	throw GDLException(this->getLine(),4,  "Unsigned long hexadecimal constant can only have "+
			    i2s(sizeof( DLong)*2)+" digits.");

      DULong val;
      Text2Number( val, base);
      cData=new DULongGDL(val);
      return;
    }

  DULong64 val;
  bool noOverFlow = Text2Number( val, base);

  if( !noOverFlow || val > std::numeric_limits< DULong>::max())
    throw GDLException(this->getLine(),4,  "Unsigned long integer constant must be less than 4294967296.");

  cData=new DULongGDL(val);
}

void DNode::Text2Long64(int base)
{
  DLong64 val;
  bool noOverFlow = Text2Number( val, base);
  if( noOverFlow )
    cData=new DLong64GDL(val);
  else 
    cData=new DLong64GDL( -1);
}
void DNode::Text2ULong64(int base)
{
  DULong64 val;
  bool noOverFlow = Text2Number( val, base);
  if( noOverFlow)
    cData=new DULong64GDL(val);
  else
    cData=new DULong64GDL( std::numeric_limits< DULong64>::max());
}
void DNode::Text2Float()
{
  const char* cStr=text.c_str();
#ifdef _MSC_VER
  DFloat val=strtod(cStr,NULL);
#else
  DFloat val=strtof(cStr,NULL);
#endif
  cData=new DFloatGDL(val);
}
void DNode::Text2ComplexI()
{
  const char* cStr=text.c_str();
#ifdef _MSC_VER
  DFloat val=strtod(cStr,NULL);
#else
  DFloat val=strtof(cStr,NULL);
#endif
  cData=new DComplexGDL(std::complex<float>(0,val));
}

void DNode::Text2ComplexDblI()
{
  const char* cStr=text.c_str();
  DDouble val=strtod(cStr,NULL);
  cData=new DComplexDblGDL(std::complex<double>(0,val));
}
void DNode::Text2Double()
{
  const char*  cStr=text.c_str();
  DDouble val=strtod(cStr,NULL);
  cData=new DDoubleGDL(val);
}
void DNode::Text2String()
{
  cData=new DStringGDL(text);
}

// used by DNodeFactory
void DNode::initialize( RefDNode t )
{
  CommonAST::setType( t->getType());
  CommonAST::setText( t->getText()); 

  DNode::SetLine(t->getLine() );

  if( t->getType() == GDLTokenTypes::CONSTANT)
    {
      if( t->cData != NULL) cData=t->cData->Dup(); else cData = NULL;
    }
  else if( t->getType() == GDLTokenTypes::GOTO)
    {
      targetIx=t->targetIx;
    }
  else if( t->getType() == GDLTokenTypes::SYSVAR || 
	   t->getType() == GDLTokenTypes::VARPTR)
    {
      var=t->var;
    }
  else
    {
      initInt=t->initInt;
    }

  labelStart = t->labelStart;
  labelEnd   = t->labelEnd;
  // copy union stuff
  //initPtr=t->initPtr;
}

void DNode::SetFunIx(const int ix) {
  if (ix == -1) unknownFunList.insert(getText());
  funIx = ix;
  if (ix != -1 && funList[ix]->isObsolete()) 
    WarnAboutObsoleteRoutine(this, funList[ix]->Name());
}

void doUpCasing( std::string& s)
  {
    unsigned len=s.length();
    for(unsigned i=0;i<len;i++)
    s[i]=std::toupper(s[i]);
  }

void DNode::MemorizeUncompiledFun(std::string & name) {
  doUpCasing(name);
  unknownFunList.insert(name);
}
void DNode::SetProIx(const int ix) {
  if (ix == -1) unknownProList.insert(getText());
  proIx = ix;
  if (ix != -1 && proList[ix]->isObsolete()) WarnAboutObsoleteRoutine(this, proList[ix]->Name());
}

void DNode::MemorizeUncompiledPro(std::string & name) {
  doUpCasing(name);
  unknownProList.insert(name);
}
