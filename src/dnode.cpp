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

#include "antlr/TreeParser.hpp"
#include "antlr/Token.hpp"

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
  var(cp.var), arrIxList(NULL), 
  libFun( cp.libFun),
  libPro( cp.libPro),
  labelStart( cp.labelStart), labelEnd( cp.labelEnd)
{
  if( cp.cData != NULL) cData = cp.cData->Dup();
  if( cp.arrIxList != NULL)
    arrIxList = cp.arrIxList->Clone();
//     arrIxList = new ArrayIndexListT( *cp.arrIxList);

  initInt = cp.initInt;
}

ArrayIndexListT* DNode::CloneArrIxList() 
  { 
    if( arrIxList == NULL) 
      return NULL;
    return arrIxList->Clone();
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
	delete cData;
      }
    if( (getType() == GDLTokenTypes::ARRAYIX))
      {
	delete arrIxList;
      }
  }

void DNode::Text2Byte(int base)
{
  // cout << "byte" << endl;
  DByte val;
  Text2Number( val, base);
  cData=new DByteGDL(val);
}

// promote: make Long (Long64) if number is to large
void DNode::Text2Int(int base, bool promote)
{
  static const DLong64 maxDInt=
    static_cast<DLong64>(numeric_limits<DInt>::max());
  static const DLong64 maxDLong=
    static_cast<DLong64>(numeric_limits<DLong>::max());
  
  if( promote)
    {
      DLong64 ll;
      Text2Number( ll, base);
      
      if( ll <= maxDInt)
	{
	  DInt val = static_cast<DInt>(ll);
	  cData=new DIntGDL( val);
	}
      else if( ll <= maxDLong)
	{
	  DLong val = static_cast<DLong>(ll);
	  cData=new DLongGDL( val);
	}
      else
	{
	  cData=new DLong64GDL( ll);
	}
    }
  else
    {
      DInt val;
      Text2Number( val, base);
      cData=new DIntGDL(val);
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
      Text2Number( val, base);
      cData=new DUIntGDL(val);
    }
}

void DNode::Text2Long(int base, bool promote)
{
  static const DLong64 maxDInt=
    static_cast<DLong64>(numeric_limits<DInt>::max());
  static const DLong64 maxDLong=
    static_cast<DLong64>(numeric_limits<DLong>::max());
  
  if( promote)
    {
      DLong64 ll;
      Text2Number( ll, base);
      
      if( ll <= maxDLong)
	{
	  DLong val = static_cast<DLong>(ll);
	  cData=new DLongGDL( val);
	}
      else
	{
	  cData=new DLong64GDL( ll);
	}
      return;
    }
  
  if( base == 16)
    {
      if( text.size() > sizeof( DLong)*2) 
	throw GDLException( "Long hexadecimal constant can only have "+
			    i2s(sizeof( DLong)*2)+" digits.");

      DLong val;
      Text2Number( val, base);
      cData=new DLongGDL(val);
      return;
    }

  DLong64 val;
  bool noOverFlow = Text2Number( val, base);

  if( !noOverFlow || val > std::numeric_limits< DLong>::max())
    throw GDLException( "Long constant must be smaller than or equal to "+
			i2s(std::numeric_limits< DLong>::max()));

  cData=new DLongGDL(val);
}

void DNode::Text2ULong(int base, bool promote) 
{
  static const DULong64 maxDUInt=
    static_cast<DULong64>(numeric_limits<DUInt>::max());
  static const DULong64 maxDULong=
    static_cast<DULong64>(numeric_limits<DULong>::max());

  if( promote)
    {
      DULong64 ll;
      Text2Number( ll, base);
      
      if( ll <= maxDULong)
	{
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
	throw GDLException( "ULong hexadecimal constant can only have "+
			    i2s(sizeof( DLong)*2)+" digits.");

      DULong val;
      Text2Number( val, base);
      cData=new DULongGDL(val);
      return;
    }

  DULong64 val;
  bool noOverFlow = Text2Number( val, base);

  if( !noOverFlow || val > std::numeric_limits< DULong>::max())
    throw GDLException( "ULong constant must be smaller than or equal to "+
			i2s(std::numeric_limits< DULong>::max()));

  cData=new DULongGDL(val);
}

void DNode::Text2Long64(int base)
{
  DLong64 val;
  bool noOverFlow = Text2Number( val, base);
  if( noOverFlow)
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
  DFloat val=strtod(cStr,NULL);
  cData=new DFloatGDL(val);
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
  funIx = ix;
  if (ix != -1 && funList[ix]->isObsolete()) 
    WarnAboutObsoleteRoutine(this, funList[ix]->Name());
}

void DNode::SetProIx(const int ix) {
  proIx = ix;
  if (ix != -1 && proList[ix]->isObsolete()) 
    WarnAboutObsoleteRoutine(this, proList[ix]->Name());
}

