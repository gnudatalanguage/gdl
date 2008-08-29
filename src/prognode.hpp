/***************************************************************************
                          prognode.hpp  -  the node used for the running program
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

#ifndef prognode_hpp__
#define prognode_hpp__

#include "dnode.hpp"

//#include "GDLInterpreter.hpp"

    enum RetCode {
        RC_OK=0,
        RC_BREAK,
        RC_CONTINUE,
        RC_RETURN, 
        RC_ABORT, // checked as retCode >= RC_RETURN
    };

class ProgNode;
typedef ProgNode* ProgNodeP;

// the nodes the programs are made of
class ProgNode
{
protected:
  static DInterpreter* interpreter;

private:
  int ttype;
  std::string text;

protected:
  ProgNodeP down;
  ProgNodeP right;

  static void AdjustTypes(std::auto_ptr<BaseGDL>& a, 
			  std::auto_ptr<BaseGDL>& b);

  BaseGDL*   cData;           // constant data
  DVar*      var;             // ptr to variable 

  DLibFun*   libFun;
  DLibPro*   libPro;

  union {
    int        initInt;    // for c-i not actually used
    int        numBranch;  // number of branches in switch/case statements
    int        nDot;       // nesting level for tag access
    int        arrayDepth; // dimension to cat
    int        proIx;      // Index into proList
    int        funIx;      // Index into funList
    int        varIx;      // Index into variable list
    int        targetIx;   // Index into label list
    int        structDefined; // struct contains entry with no tag name
    int        compileOpt; // for PRO and FUNCTION nodes
  };

private:
  // from DNode (see there)
  int lineNumber;
  ArrayIndexListT* arrIxList; // ptr to array index list
  int labelStart; // for loops to determine if to bail out
  int labelEnd; // for loops to determine if to bail out

public:
  ProgNode();

  ProgNode( const RefDNode& refNode);

  static ProgNodeP NewProgNode( const RefDNode& refNode);

  virtual ~ProgNode();
  
  void SetNodes( const ProgNodeP right, const ProgNodeP down);

  virtual BaseGDL* Eval();
  virtual BaseGDL* EvalNC(); // non-copy
  virtual void     Run();

  bool ConstantNode()
  {
    return this->getType() == GDLTokenTypes::CONSTANT;
  }

  ProgNodeP getFirstChild() const
  {
    return down;
  }
  ProgNodeP GetFirstChild() const
  {
    return getFirstChild();
  }
  ProgNodeP getNextSibling() const
  {
    return right;
  }
  ProgNodeP GetNextSibling() const
  {
    return getNextSibling();
  }

  ProgNodeP StealNextSibling()
  {
    ProgNodeP n = right;
    right = NULL;
    return n;
  }
  ProgNodeP StealFirstChild()
  {
    ProgNodeP n = down;
    down = NULL;
    return n;
  }
  
  int getType() { return ttype;}
  void setType( int t) { ttype=t;}
  std::string getText() { return text;}
  void setText(const std::string t) { text = t;}
  int getLine() const { return lineNumber;}
  void SetGotoIx( int ix) { targetIx=ix;}
  
  bool LabelInRange( const int lIx)
  { return (lIx >= labelStart) && (lIx < labelEnd);}
  
  friend class GDLInterpreter;
  friend class DInterpreter;

friend class NSTRUCNode;
friend class ASSIGNNode;
friend class ASSIGN_REPLACENode;
friend class PCALL_LIBNode;//: public CommandNode
friend class MPCALLNode;//: public CommandNode
friend class MPCALL_PARENTNode;//: public CommandNode
friend class PCALLNode;//: public CommandNode
};

class DefaultNode: public ProgNode
{
public:
  DefaultNode(): ProgNode()  {}  

  DefaultNode( const RefDNode& refNode): ProgNode( refNode) 
  {
    if( refNode->GetFirstChild() != RefDNode(antlr::nullAST))
      {
	down = NewProgNode( refNode->GetFirstChild());
      }
    if( refNode->GetNextSibling() != RefDNode(antlr::nullAST))
      {
	right = NewProgNode( refNode->GetNextSibling());
      }
  }
};

//#undef UNDEF
//#ifdef UNDEF

class CommandNode: public DefaultNode
{
public:
  CommandNode( const RefDNode& refNode): DefaultNode( refNode) {}
 
};

class ASSIGNNode: public CommandNode
{
public:
  ASSIGNNode( const RefDNode& refNode): CommandNode( refNode) {}
  void Run();
};
class ASSIGN_REPLACENode: public CommandNode
{
public:
  ASSIGN_REPLACENode( const RefDNode& refNode): CommandNode( refNode) {}
  void Run();
};
class PCALL_LIBNode: public CommandNode
{
public:
  PCALL_LIBNode( const RefDNode& refNode): CommandNode( refNode) {}
  void Run();
};
class MPCALLNode: public CommandNode
{
public:
  MPCALLNode( const RefDNode& refNode): CommandNode( refNode) {}
  void Run();
};
class MPCALL_PARENTNode: public CommandNode
{
public:
  MPCALL_PARENTNode( const RefDNode& refNode): CommandNode( refNode) {}
  void Run();
};
class PCALLNode: public CommandNode
{
public:
  PCALLNode( const RefDNode& refNode): CommandNode( refNode) {}
  void Run();
};
class DECNode: public CommandNode
{ public:
  DECNode( const RefDNode& refNode): CommandNode( refNode){}
  void Run();
};
class INCNode: public CommandNode
{ public:
  INCNode( const RefDNode& refNode): CommandNode( refNode){}
  void Run();
};
// class FOR_INITNode: public CommandNode
// { public:
//   FOR_INITNode( const RefDNode& refNode): CommandNode( refNode){}
//   void Run();
// };
// class FORNode: public CommandNode
// { public:
//   FORNode( const RefDNode& refNode): CommandNode( refNode){}
//   void Run();
// };
// class FOR_STEPNode: public CommandNode
// { public:
//   FOR_STEPNode( const RefDNode& refNode): CommandNode( refNode){}
//   void Run();
// };


// class ARRAYDEFNode: public CommandNode
// {
// public:
//   /*virtual*/ RetCode   Run();
// 
// };
//#endif

#endif
