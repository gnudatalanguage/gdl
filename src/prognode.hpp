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

// inline bool* GetNonCopyNodeLookupArray()
// {
// static bool nonCopyNodeLookupArray[ GDLTokenTypes::MAX_TOKEN_NUMBER];
// for( int i=0; i<GDLTokenTypes::MAX_TOKEN_NUMBER; ++i)
// 	nonCopyNodeLookupArray[ i] = false;
// nonCopyNodeLookupArray[ GDLTokenTypes::VAR] = true;
// nonCopyNodeLookupArray[ GDLTokenTypes::VARPTR] = true;
// nonCopyNodeLookupArray[ GDLTokenTypes::DEREF] = true;
// nonCopyNodeLookupArray[ GDLTokenTypes::CONSTANT] = true;
// nonCopyNodeLookupArray[ GDLTokenTypes::SYSVAR] = true;
// return nonCopyNodeLookupArray;
// }
bool* GetNonCopyNodeLookupArray();

inline bool NonCopyNode( int type)
{
  static bool* nonCopyNodeLookupArray = GetNonCopyNodeLookupArray();
  return nonCopyNodeLookupArray[ type];
//   return 
//     (type == GDLTokenTypes::VAR) ||
//     (type == GDLTokenTypes::VARPTR) ||
//     (type == GDLTokenTypes::DEREF) ||
//     (type == GDLTokenTypes::CONSTANT) ||
//     (type == GDLTokenTypes::SYSVAR) ;

// are always copy nodes:
//     (type == GDLTokenTypes::ARRAYDEF)
//     (type == GDLTokenTypes::STRUC)
//     (type == GDLTokenTypes::NSTRUC)
//     (type == GDLTokenTypes::NSTRUC_REF)
}

class BreakableNode;

// the nodes the programs are made of
class ProgNode
{
protected:
  static DInterpreter* interpreter;

private:
  int ttype;
  std::string text;

protected:
  bool keepRight;
  bool keepDown;

  ProgNodeP breakTarget;
  
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
    int        nParam;     // number of parameters in this parameter list (stored in 1st par)
    int        arrayDepth; // dimension to cat
    int        proIx;      // Index into proList
    int        funIx;      // Index into funList
    int        varIx;      // Index into variable list
    int        targetIx;   // Index into label list
    int        structDefined; // struct contains entry with no tag name
    int        compileOpt; // for PRO and FUNCTION nodes
    int        forLoopIx; // acessing loop variables
  };

  void SetType( int tt, const std::string& txt) { ttype = tt; text = txt;} 

  static ProgNodeP GetNULLProgNodeP(); 

private:
  // from DNode (see there)
  int lineNumber;
  ArrayIndexListT* arrIxList; // ptr to array index list
  ArrayIndexListT* arrIxListNoAssoc; // ptr to array index list
  int labelStart; // for loops to determine if to bail out
  int labelEnd; // for loops to determine if to bail out

  // disable usage
  ProgNode( const ProgNode& p) {}

public:
  ProgNode();

  ProgNode( const RefDNode& refNode);

	// tree translation takes place here
	// see newprognode.cpp
  static ProgNodeP NewProgNode( const RefDNode& refNode);
  static int NumberForLoops( ProgNodeP tree, int offset = 0)
  {
	if( tree == NULL)
		return offset;
		
	return tree->NumberForLoops( offset);
  }

  virtual int NumberForLoops( int actNum)
  {
	if( down != NULL && !keepDown)
		{
			actNum = down->NumberForLoops( actNum);
		}
		
	if( right != NULL && !keepRight)
		{
			actNum = right->NumberForLoops( actNum);
		}
	return actNum;
 }
  
  virtual ~ProgNode();
  
  void SetRightDown( const ProgNodeP right, const ProgNodeP down);

  virtual BaseGDL** LExpr( BaseGDL* right);
//   virtual BaseGDL** LExprGrab( BaseGDL* right); // take ownership of right
  virtual BaseGDL** LEval();
  virtual BaseGDL* Eval(); // caller receives ownership
  virtual BaseGDL* EvalNC(); // non-copy used by all operators (and in other places)
  virtual RetCode    Run();

//   RetCode  (*RunP)();

  bool ConstantNode();

  ProgNodeP getFirstChild() const
  {
    return down;
  }
  ProgNodeP GetFirstChild() const
  {
    return down;//getFirstChild();
  }
  void SetFirstChild( ProgNodeP d)
  {
    down = d;
  }
  ProgNodeP getNextSibling() const
  {
    return right;
  }
  ProgNodeP GetLastSibling() const
  {
	ProgNodeP act = const_cast<ProgNodeP>(this);
	while(!act->KeepRight() && act->GetNextSibling() != NULL) act = act->GetNextSibling();
    return act;
  }
  ProgNodeP GetNextSibling() const
  {
    return right;//getNextSibling();
  }
  void SetNextSibling( ProgNodeP r)
  {
    right = r;
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

  bool KeepRight() const { return keepRight;}
  bool KeepDown() const { return keepDown;}
  
  void KeepDown( ProgNodeP d)
  {
	down = d;
	keepDown = true;
  }
  virtual void KeepRight( ProgNodeP r)
  {
	right = r;
	keepRight = true;
  }

   void SetRight( ProgNodeP r)
   {
 	right = r;
// 	keepRight = false;
   }
  
	virtual void SetAllBreak( ProgNodeP target)
	{
		if( down != NULL && !keepDown)
				{
					down->SetAllBreak( target);
				}
		
		if( right != NULL && !keepRight)
		{
			right->SetAllBreak( target);
		}
	}
	virtual void SetAllContinue( ProgNodeP target)
	{
		if( down != NULL && !keepDown)
				{
					down->SetAllContinue( target);
				}
		
		if( right != NULL && !keepRight)
		{
			right->SetAllContinue( target);
		}
	}

  int getType() { return ttype;}
  void setType( int t) { ttype=t;}
  std::string getText() { return text;}
  void setText(const std::string t) { text = t;}
  int getLine() const { return lineNumber;}
  void setLine( int l) { lineNumber = l;}
  void SetGotoIx( int ix) { targetIx=ix;}

  int GetNParam() const { return nParam;}

  ProgNodeP BreakTarget() const { return breakTarget;}
  
  bool LabelInRange( const int lIx)
  {
// 	std::cout << "LabelInRange: " << ((lIx >= labelStart) && (lIx < labelEnd)) << "     " << lIx << "   [" << labelStart << "," << labelEnd << ")" << std::endl;
	return (lIx >= labelStart) && (lIx < labelEnd);
  }
  
	friend class GDLInterpreter;
	friend class DInterpreter;

	friend class NSTRUCNode;
	friend class ASSIGNNode;
	friend class ASSIGN_REPLACENode;
	friend class PCALL_LIBNode;//: public CommandNode
	friend class MPCALLNode;//: public CommandNode
	friend class MPCALL_PARENTNode;//: public CommandNode
	friend class PCALLNode;//: public CommandNode
	friend class ARRAYEXPR_MFCALLNode;
	friend class KEYDEF_Node;
	friend class KEYDEF_REFNode;
	friend class KEYDEF_REF_CHECKNode;
	friend class KEYDEF_REF_EXPRNode;
	friend class REFNode;
	friend class REF_CHECKNode;
	friend class REF_EXPRNode;
	friend class ParameterNode;
	friend class ARRAYEXPRNode;
	friend class EXPRNode;
	friend class SYSVARNode;
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

class RETPNode: public DefaultNode
{
public:
	RetCode      Run();
	
public:
    RETPNode(): DefaultNode()  {}
	
	RETPNode( const RefDNode& refNode): DefaultNode( refNode)
	{}
};
class RETFNode: public DefaultNode
{
public:
 RetCode      Run();
	
public:
    RETFNode(): DefaultNode()  {}
	
	RETFNode( const RefDNode& refNode): DefaultNode( refNode)
	{}
};
class GOTONode: public DefaultNode
{
public:
 RetCode      Run();
	
	void SetAllBreak( ProgNodeP target)
	{
// 		breakTarget = target;
		
		if( right != NULL && !keepRight)
		{
			right->SetAllBreak( target);
		}
	}
public:
    GOTONode(): DefaultNode()  {}
	
	GOTONode( const RefDNode& refNode): DefaultNode( refNode)
	{}
};
class CONTINUENode: public DefaultNode
{
public:
 RetCode      Run();
	

	void SetAllContinue( ProgNodeP target)
	{
		assert( target != NULL);
		breakTarget = target;
		
		if( right != NULL && !keepRight)
		{
			right->SetAllContinue( target);
		}
	}
public:
    CONTINUENode(): DefaultNode()  {}
	
	CONTINUENode( const RefDNode& refNode): DefaultNode( refNode)
	{}
};
class BREAKNode: public DefaultNode
{
public:
 RetCode      Run();
	

	void SetAllBreak( ProgNodeP target)
	{
		breakTarget = target;
		
		if( right != NULL && !keepRight)
		{
			right->SetAllBreak( target);
		}
	}
public:
    BREAKNode(): DefaultNode()  {}
	
	BREAKNode( const RefDNode& refNode): DefaultNode( refNode)
	{}
};
class LABELNode: public DefaultNode
{
public:
 RetCode      Run();
	

public:
    LABELNode(): DefaultNode()  {}
	
	LABELNode( const RefDNode& refNode): DefaultNode( refNode)
	{}
};
class ON_IOERROR_NULLNode: public DefaultNode
{
public:
 RetCode      Run();
	

public:
    ON_IOERROR_NULLNode(): DefaultNode()  {}
	
	ON_IOERROR_NULLNode( const RefDNode& refNode): DefaultNode( refNode)
	{}
};
class ON_IOERRORNode: public DefaultNode
{
public:
 RetCode      Run();
	

public:
    ON_IOERRORNode(): DefaultNode()  {}
	
	ON_IOERRORNode( const RefDNode& refNode): DefaultNode( refNode)
	{}
};


class BreakableNode: public ProgNode
{
public:
	void SetAllBreak( ProgNodeP target)
	{
		// down: do NOT descent into own loop tree here
		
		if( right != NULL && !keepRight)
		{
			right->SetAllBreak( target);
		}
	}
	void SetAllContinue( ProgNodeP target)
	{
		// down: do NOT descent into own loop tree here
		
		if( right != NULL && !keepRight)
		{
			right->SetAllContinue( target);
		}
	}

public:
  BreakableNode(): ProgNode()  {}

  BreakableNode( const RefDNode& refNode): ProgNode( refNode)
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




class FOR_LOOPNode: public BreakableNode
{
  public:
 RetCode      Run();

	ProgNodeP statementList;
	
	ProgNodeP GetStatementList()
	{
		return down->GetNextSibling();//->GetNextSibling()->GetNextSibling();
	}
	
  public:
	void KeepRight( ProgNodeP r)
	{
		right = r;
		keepRight = true;
		assert( this->GetStatementList() != NULL);
// 		if( this->GetStatementList() != NULL)
			this->GetStatementList()->SetAllBreak( right);
	}
  
  int NumberForLoops( int actNum)
  {
	this->forLoopIx = actNum;
	actNum++;
	ProgNodeP statementList = this->GetStatementList();
	if( statementList != NULL && !down->KeepRight())
		{
			actNum = statementList->NumberForLoops( actNum);
		}
	if( right != NULL && !keepRight)
		{
			actNum = right->NumberForLoops( actNum);
		}
	return actNum;
 }
  
  public:
  FOR_LOOPNode( ProgNodeP r, ProgNodeP d): BreakableNode()
  {
    SetType( GDLTokenTypes::FOR_LOOP, "for_loop");
	SetRightDown( r, d);

	assert( down != NULL);
	
	statementList = this->GetStatementList();
	if( statementList != NULL)
		{
			statementList->SetAllContinue( this);
			statementList->GetLastSibling()->KeepRight( this);
			if( right != NULL) statementList->SetAllBreak( right);
		}
	else
		{
			down->KeepRight( this);
			statementList = this;
		}
  }

};


class FORNode: public BreakableNode
{
  public:
 RetCode      Run();
	
  void KeepRight( ProgNodeP r);
	
  int NumberForLoops( int actNum)
  {
	this->forLoopIx = actNum;
	
// 	assert( down == NULL);
		
	assert( right != NULL && !keepRight);
			
	actNum = right->NumberForLoops( actNum);
	
	return actNum;
 }
  
  public:
  FORNode(): BreakableNode()  {}

  FORNode( const RefDNode& refNode): BreakableNode( refNode)
	{
	ProgNodeP keep = down->GetNextSibling();
	down->SetRight( down->GetNextSibling()->GetNextSibling()->GetNextSibling());

	keep->GetNextSibling()->SetRight( NULL);
	
    FOR_LOOPNode* forLoop = new FOR_LOOPNode( right, down);
	forLoop->setLine( getLine());

	down = keep;
	
	right = forLoop;
//   		if( this->GetStatementList() != NULL && right != NULL)
// 			this->GetStatementList()->GetLastSibling()->KeepRight( right);
	}
};



class FOR_STEP_LOOPNode: public BreakableNode
{
  public:
 RetCode      Run();
	
	ProgNodeP GetStatementList()
	{
		return down->GetNextSibling();//->GetNextSibling()->GetNextSibling()->GetNextSibling();
	}
	
  public:
	void KeepRight( ProgNodeP r)
	{
		right = r;
		keepRight = true;
		if( this->GetStatementList() != NULL)
			this->GetStatementList()->SetAllBreak( right);
	}
  
  int NumberForLoops( int actNum)
  {
	this->forLoopIx = actNum;
	actNum++;
	ProgNodeP statementList = this->GetStatementList();
	if( statementList != NULL && !down->KeepRight())
		{
			actNum = statementList->NumberForLoops( actNum);
		}
	if( right != NULL && !keepRight)
		{
			actNum = right->NumberForLoops( actNum);
		}
	return actNum;
 }
  
  public:
  FOR_STEP_LOOPNode( ProgNodeP r, ProgNodeP d): BreakableNode()
  {
    SetType( GDLTokenTypes::FOR_STEP_LOOP, "for_step_loop");
	SetRightDown( r, d);

	assert( down != NULL);
	
	ProgNodeP statementList = this->GetStatementList();
	if( statementList != NULL)
		{
			statementList->SetAllContinue( this);
			statementList->GetLastSibling()->KeepRight( this);
			if( right != NULL) statementList->SetAllBreak( right);
		}
	else
		{
			down->KeepRight( this);
		}
  }

};



class FOR_STEPNode: public BreakableNode
{
  public:
 RetCode      Run();
	
	void KeepRight( ProgNodeP r);
	
  int NumberForLoops( int actNum)
  {
	this->forLoopIx = actNum;
	
	//assert( down == NULL);
		
	assert( right != NULL && !keepRight);
			
	actNum = right->NumberForLoops( actNum);
	
	return actNum;
 }
  
  public:
  FOR_STEPNode(): BreakableNode()  {}

  FOR_STEPNode( const RefDNode& refNode): BreakableNode( refNode)
	{
	ProgNodeP keep = down->GetNextSibling();
	down->SetRight( down->GetNextSibling()->GetNextSibling()->GetNextSibling()->GetNextSibling());

	keep->GetNextSibling()->GetNextSibling()->SetRight( NULL);
	
    FOR_STEP_LOOPNode* forLoop = new FOR_STEP_LOOPNode( right, down);
	forLoop->setLine( getLine());

	down = keep;
	
	right = forLoop;

//   		if( this->GetStatementList() != NULL && right != NULL)
// 			this->GetStatementList()->GetLastSibling()->KeepRight( right);
	}
};

class FOREACH_LOOPNode: public BreakableNode
{
  public:
 RetCode      Run();
	
	ProgNodeP GetStatementList()
	{
		return down->GetNextSibling();
	}
	
  public:
	void KeepRight( ProgNodeP r)
	{
		right = r;
		keepRight = true;
		if( this->GetStatementList() != NULL)
			this->GetStatementList()->SetAllBreak( right);
	}
  
  int NumberForLoops( int actNum)
  {
	this->forLoopIx = actNum;
	actNum++;
	ProgNodeP statementList = this->GetStatementList();
	if( statementList != NULL && !down->KeepRight())
		{
			actNum = statementList->NumberForLoops( actNum);
		}
	if( right != NULL && !keepRight)
		{
			actNum = right->NumberForLoops( actNum);
		}
	return actNum;
 }
  
  public:
  FOREACH_LOOPNode( ProgNodeP r, ProgNodeP d): BreakableNode()
  {
    SetType( GDLTokenTypes::FOREACH_LOOP, "foreach_loop");
	SetRightDown( r, d);

	assert( down != NULL);
	
	ProgNodeP statementList = this->GetStatementList();
	if( statementList != NULL)
		{
			statementList->SetAllContinue( this);
			statementList->GetLastSibling()->KeepRight( this);
			if( right != NULL) statementList->SetAllBreak( right);
		}
	else
		{
			down->KeepRight( this);
		}
  }

};



class FOREACHNode: public BreakableNode
{
  public:
 RetCode      Run();
	
	void KeepRight( ProgNodeP r);
	
  int NumberForLoops( int actNum)
  {
	this->forLoopIx = actNum;
	
// 	assert( down == NULL);
		
	assert( right != NULL && !keepRight);
			
	actNum = right->NumberForLoops( actNum);
	
	return actNum;
 }
  
  public:
  FOREACHNode(): BreakableNode()  {}

  FOREACHNode( const RefDNode& refNode): BreakableNode( refNode)
  {
	ProgNodeP keep = down->GetNextSibling();
	down->SetRight( down->GetNextSibling()->GetNextSibling());

	keep->SetRight( NULL);

	FOREACH_LOOPNode* forLoop = new FOREACH_LOOPNode( right, down);
	forLoop->setLine( getLine());

	down = keep;

	right = forLoop;
 }
};



class FOREACH_INDEX_LOOPNode: public BreakableNode
{
  public:
 RetCode      Run();
	
	ProgNodeP GetStatementList()
	{
		return down->GetNextSibling();
	}
	
  public:
	void KeepRight( ProgNodeP r)
	{
		right = r;
		keepRight = true;
		if( this->GetStatementList() != NULL)
			this->GetStatementList()->SetAllBreak( right);
	}
  
  int NumberForLoops( int actNum)
  {
	this->forLoopIx = actNum;
	actNum++;
	ProgNodeP statementList = this->GetStatementList();
	if( statementList != NULL && !down->KeepRight())
		{
			actNum = statementList->NumberForLoops( actNum);
		}
	if( right != NULL && !keepRight)
		{
			actNum = right->NumberForLoops( actNum);
		}
	return actNum;
 }
  
  public:
  FOREACH_INDEX_LOOPNode( ProgNodeP r, ProgNodeP d): BreakableNode()
  {
    SetType( GDLTokenTypes::FOREACH_INDEX_LOOP, "foreach_index_loop");
	SetRightDown( r, d);

	assert( down != NULL);
	
	ProgNodeP statementList = this->GetStatementList();
	if( statementList != NULL)
		{
			statementList->SetAllContinue( this);
			statementList->GetLastSibling()->KeepRight( this);
			if( right != NULL) statementList->SetAllBreak( right);
		}
	else
		{
			down->KeepRight( this);
		}
  }

};



class FOREACH_INDEXNode: public BreakableNode
{
  public:
 RetCode      Run();
	
  void KeepRight( ProgNodeP r);
	
  int NumberForLoops( int actNum)
  {
	this->forLoopIx = actNum;
	
// 	assert( down == NULL);
		
	assert( right != NULL && !keepRight);
			
	actNum = right->NumberForLoops( actNum);
	
	return actNum;
 }
  
  public:
  FOREACH_INDEXNode(): BreakableNode()  {}

  FOREACH_INDEXNode( const RefDNode& refNode): BreakableNode( refNode)
  {
    // down is variable,array,variable
	ProgNodeP keep = down->GetNextSibling(); // the array to loop over

	ProgNodeP index = down->GetNextSibling()->GetNextSibling(); // the index variable

	down->SetRight( index); // jump over array

	// cut away everything after the array from keep
	keep->SetRight( NULL);

	FOREACH_INDEX_LOOPNode* forLoop = new FOREACH_INDEX_LOOPNode( right, down);
	forLoop->setLine( getLine());

	down = keep;

	right = forLoop;
 }
};



class WHILENode: public BreakableNode
{
  public:
 RetCode      Run();
	
	ProgNodeP GetStatementList()
	{
		return down->GetNextSibling();
	}
	
	void KeepRight( ProgNodeP r)
	{
		right = r;
		keepRight = true;
		if( this->GetStatementList() != NULL)
			this->GetStatementList()->SetAllBreak( right);
	}
  
  public:
  WHILENode(): BreakableNode()  {}

  WHILENode( const RefDNode& refNode): BreakableNode( refNode)
  {
	assert( down != NULL);
  
// 	down->GetLastSibling()->KeepRight( this); // for empty body
	
	ProgNodeP statementList = this->GetStatementList();
	if( statementList != NULL)
		{
			statementList->SetAllContinue( this);
			if( right != NULL) statementList->SetAllBreak( right);
			statementList->GetLastSibling()->KeepRight( this); // for empty body
		}
  }
};



class REPEAT_LOOPNode: public BreakableNode
{
  public:
 RetCode      Run();
	
	ProgNodeP GetStatementList()
	{
		return down->GetNextSibling();
	}
	
  public:
	void KeepRight( ProgNodeP r)
	{
		right = r;
		keepRight = true;
		if( this->GetStatementList() != NULL)
			this->GetStatementList()->SetAllBreak( right);
	}
  
  public:
  REPEAT_LOOPNode( ProgNodeP r, ProgNodeP d): BreakableNode()
  {
    SetType( GDLTokenTypes::REPEAT_LOOP, "repeat_loop");
	SetRightDown( r, d);

	assert( down != NULL);
	
	ProgNodeP statementList = this->GetStatementList();
	if( statementList != NULL)
		{
			statementList->SetAllContinue( this);
			statementList->GetLastSibling()->KeepRight( this);
			if( right != NULL) statementList->SetAllBreak( right);
		}
  }

};



class REPEATNode: public BreakableNode
{
  public:
 RetCode      Run();
	
	void KeepRight( ProgNodeP r)
	{
		right = r;
		keepRight = true;
		down->KeepRight( right); // REPEAT_LOOP
	}

public:
  REPEATNode(): BreakableNode()  {}

  REPEATNode( const RefDNode& refNode): BreakableNode( refNode)
  {
    REPEAT_LOOPNode* repeatLoop = new REPEAT_LOOPNode( NULL, down);
	repeatLoop->KeepRight( right);
	repeatLoop->setLine( getLine());
	
	down = repeatLoop;
  }
};



class CASENode: public BreakableNode
{
  public:
 RetCode      Run();
  
	ProgNodeP GetStatementList()
	{
		return down->GetNextSibling();
	}
	
	void KeepRight( ProgNodeP r)
	{
		assert( down != NULL);
		right = r;
		keepRight = true;
		// down is expr
		ProgNodeP csBlock = GetStatementList();
		while( csBlock != NULL)
		{
			if( csBlock->getType() == GDLTokenTypes::ELSEBLK)
				{
					ProgNodeP statementList = csBlock->GetFirstChild();
					if( statementList != NULL)
					{
							statementList->GetLastSibling()->KeepRight( right);
					}
				}
			else
				{
					// keep expr in case of empty statement
					ProgNodeP statementList = csBlock->GetFirstChild()->GetNextSibling();
					if( statementList != NULL)
					{
							statementList->GetLastSibling()->KeepRight( right);
					}
				}
			csBlock = csBlock->GetNextSibling();
		}
		GetStatementList()->SetAllBreak( right);
	}

public:
  CASENode(): BreakableNode()  {}

  CASENode( const RefDNode& refNode): BreakableNode( refNode)
  {
    assert( down != NULL);
	
	ProgNodeP statementList = this->GetStatementList();
	statementList->SetAllBreak( right);

    // down is expr
    ProgNodeP csBlock = GetStatementList();

	while( csBlock != NULL)
	{
		if( csBlock->getType() == GDLTokenTypes::ELSEBLK)
			{
				ProgNodeP statementList = csBlock->GetFirstChild();
				if( statementList != NULL)
				{
						statementList->GetLastSibling()->KeepRight( right);
				}
			}
		else
			{
				// keep expr in case of empty statement
				ProgNodeP statementList = csBlock->GetFirstChild()->GetNextSibling();
				if( statementList != NULL)
				{
						statementList->GetLastSibling()->KeepRight( right);
				}
			}
		
// 		if( csBlock->GetNextSibling() == NULL)
// 		{
// 				csBlock->KeepRight( right);
// 				break;
// 		}
		
		csBlock = csBlock->GetNextSibling();
	}
  }
};



class SWITCHNode: public BreakableNode
{
  public:
 RetCode      Run();
  
	ProgNodeP GetStatementList()
	{
		return down->GetNextSibling();
	}

  void KeepRight( ProgNodeP r)
  {
	right = r;
	keepRight = true;
    ProgNodeP csBlock = GetStatementList();
	ProgNodeP lastStatementList = NULL;
	while( csBlock != NULL)
	{
		if( csBlock->getType() == GDLTokenTypes::ELSEBLK)
			{
				ProgNodeP statementList = csBlock->GetFirstChild();
				if( statementList != NULL)
				{
					lastStatementList = statementList;
				}
			}
		else
			{
				// keep expr in case of empty statement
				ProgNodeP statementList = csBlock->GetFirstChild()->GetNextSibling();
				if( statementList != NULL)
				{
					lastStatementList = statementList;
				}
			}
		csBlock = csBlock->GetNextSibling();
	}
	if( lastStatementList != NULL)
		lastStatementList->GetLastSibling()->KeepRight( right);
	GetStatementList()->SetAllBreak( right);
 } 
	
public:
  SWITCHNode(): BreakableNode()  {}

  SWITCHNode( const RefDNode& refNode): BreakableNode( refNode)
  {
    assert( down != NULL);

	ProgNodeP statementList = this->GetStatementList();
	statementList->SetAllBreak( right);
 
    // down is expr
    ProgNodeP csBlock = GetStatementList();

	ProgNodeP lastStatementList = NULL;

	while( csBlock != NULL)
	{
		if( csBlock->getType() == GDLTokenTypes::ELSEBLK)
			{
				ProgNodeP statementList = csBlock->GetFirstChild();
				if( statementList != NULL)
				{
					if( lastStatementList != NULL)
						lastStatementList->GetLastSibling()->KeepRight( statementList);
						
					lastStatementList = statementList;
				}
			}
		else
			{
				// keep expr in case of empty statement
				ProgNodeP statementList = csBlock->GetFirstChild()->GetNextSibling();
				if( statementList != NULL)
				{
					if( lastStatementList != NULL)
						lastStatementList->GetLastSibling()->KeepRight( statementList);
						
					lastStatementList = statementList;
				}
			}
		if( csBlock->GetNextSibling() == NULL)
		{
				if( lastStatementList != NULL)
					lastStatementList->GetLastSibling()->KeepRight( right);
				break;
		}
		csBlock = csBlock->GetNextSibling();
	}
  }

};



class BLOCKNode: public ProgNode
{
  public:
 RetCode      Run();
	
  void KeepRight( ProgNodeP r)
  {
	right = r;
	keepRight = true;
	// 	must recursively set dependents here
     if( down != NULL && !KeepDown())
		down->GetLastSibling()->KeepRight( right);
	else
		this->KeepDown( right);
 }

public:
  BLOCKNode(): ProgNode()  {}

  BLOCKNode( const RefDNode& refNode): ProgNode( refNode)
  {
    if( refNode->GetFirstChild() != RefDNode(antlr::nullAST))
      {
		down = NewProgNode( refNode->GetFirstChild());
      }
    if( refNode->GetNextSibling() != RefDNode(antlr::nullAST))
      {
		right = NewProgNode( refNode->GetNextSibling());
     
		// first statement
		if( down != NULL)
			down->GetLastSibling()->KeepRight( right);
		else
			this->KeepDown( right);
      }
  }

};



class IFNode: public ProgNode
{
  public:
 RetCode      Run();
  
  void KeepRight( ProgNodeP r)
  {
    assert( down != NULL);
	right = r;
	keepRight = true;
	down->GetLastSibling()->KeepRight( right);
  }
public:
  IFNode(): ProgNode()  {}

  IFNode( const RefDNode& refNode): ProgNode( refNode)
  {
		if( refNode->GetFirstChild() != RefDNode(antlr::nullAST))
		{
			down = NewProgNode( refNode->GetFirstChild());
		}
		if( refNode->GetNextSibling() != RefDNode(antlr::nullAST))
		{
			right = NewProgNode( refNode->GetNextSibling());
		}

		assert( down != NULL);

        // first alternative
        if( right != NULL)
        {
			ProgNodeP s1 = down->GetNextSibling(); // skip expr
			s1->GetLastSibling()->KeepRight( right);
        }
  }
};



class IF_ELSENode: public ProgNode
{
  public:
 RetCode      Run();
  
  void KeepRight( ProgNodeP r)
  {
    // 	must recursively set dependents here
    assert( down != NULL);
     
	right = r;
	keepRight = true;
        
	ProgNodeP s1 = down->GetNextSibling(); // skip expr
	if( s1->GetFirstChild() == NULL || s1->KeepDown())
			{
				s1->KeepDown( right);
			}
	else
			{
				s1->GetFirstChild()->GetLastSibling()->KeepRight( right);
			}
		
	// 2nd alternative
	ProgNodeP s2 = s1->GetNextSibling();
	s2->GetLastSibling()->KeepRight( right); 
  }

public:
  IF_ELSENode(): ProgNode()  {}

  IF_ELSENode( const RefDNode& refNode): ProgNode( refNode)
  {
// 	std::cout << "IF_ELSENode" << std::endl;
    if( refNode->GetFirstChild() != RefDNode(antlr::nullAST))
      {
		down = NewProgNode( refNode->GetFirstChild());
      }
    if( refNode->GetNextSibling() != RefDNode(antlr::nullAST))
      {
		right = NewProgNode( refNode->GetNextSibling());
      }

    assert( down != NULL);

		// IF expr s1 s2
		// first alternative
		// s1 is always a BLOCK (gdlc.tree.g, if_statement)
	// right MUST be set here even if NULL as it IS set to 2nd alternative
	ProgNodeP s1 = down->GetNextSibling(); // skip expr
	if( s1->GetFirstChild() == NULL || s1->KeepDown())
		{
			s1->KeepDown( right);
		}
	else
		{
			s1->GetFirstChild()->GetLastSibling()->KeepRight( right);
		}
		
    if( right != NULL)
    {
		// 2nd alternative
		ProgNodeP s2 = s1->GetNextSibling();

		s2->GetLastSibling()->KeepRight( right); // disconnect s2
	}
  }
};

//#undef UNDEF
//#ifdef UNDEF
class EnvBaseT;

class ParameterNode: public DefaultNode
{
public:
  ParameterNode( const RefDNode& refNode): DefaultNode( refNode) {}
  virtual void Parameter( EnvBaseT* actEnv);
//   virtual void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};

class ParameterVNNode: public ParameterNode
{
public:
  ParameterVNNode( const RefDNode& refNode): ParameterNode( refNode) {}
  void Parameter( EnvBaseT* actEnv);
//   virtual void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};

class KEYDEF_REFNode: public ParameterNode
{
public:
  KEYDEF_REFNode( const RefDNode& refNode): ParameterNode( refNode) {}
  void Parameter( EnvBaseT* actEnv);
//   void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};

class KEYDEF_REF_EXPRNode: public ParameterNode
{
public:
  KEYDEF_REF_EXPRNode( const RefDNode& refNode): ParameterNode( refNode) {}
  void Parameter( EnvBaseT* actEnv);
//   void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};

class KEYDEF_REF_CHECKNode: public ParameterNode
{
public:
  KEYDEF_REF_CHECKNode( const RefDNode& refNode): ParameterNode( refNode) {}
  void Parameter( EnvBaseT* actEnv);
//   void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};

class KEYDEFNode: public ParameterNode
{
public:
  KEYDEFNode( const RefDNode& refNode): ParameterNode( refNode) {}
  void Parameter( EnvBaseT* actEnv);
//   void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};

class REF_EXPRNode: public ParameterNode
{
public:
  REF_EXPRNode( const RefDNode& refNode): ParameterNode( refNode) {}
  void Parameter( EnvBaseT* actEnv);
//   void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};
class REF_EXPRVNNode: public ParameterNode
{
public:
  REF_EXPRVNNode( const RefDNode& refNode): ParameterNode( refNode) {}
  void Parameter( EnvBaseT* actEnv);
//   void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};

class REF_CHECKNode: public ParameterNode
{
public:
  REF_CHECKNode( const RefDNode& refNode): ParameterNode( refNode) {}
  void Parameter( EnvBaseT* actEnv);
//   void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};
class REF_CHECKVNNode: public ParameterNode
{
public:
  REF_CHECKVNNode( const RefDNode& refNode): ParameterNode( refNode) {}
  void Parameter( EnvBaseT* actEnv);
//   void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};

class REFNode: public ParameterNode
{
public:
  REFNode( const RefDNode& refNode): ParameterNode( refNode) {}
  void Parameter( EnvBaseT* actEnv);
//   void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};
class REFVNNode: public ParameterNode
{
public:
  REFVNNode( const RefDNode& refNode): ParameterNode( refNode) {}
  void Parameter( EnvBaseT* actEnv);
//   void ParameterVarNum( EnvBaseT* actEnv); // for variable number of parameters
};



class CommandNode: public DefaultNode
{
public:
  CommandNode( const RefDNode& refNode): DefaultNode( refNode) {}
};
class ASSIGNNode: public CommandNode
{
public:
  ASSIGNNode( const RefDNode& refNode): CommandNode( refNode) {}
  RetCode Run();
  BaseGDL** LExpr( BaseGDL* right);
//   BaseGDL** LExprGrab( BaseGDL* right);
  BaseGDL* Eval();
};
class ASSIGN_ARRAYEXPR_MFCALLNode: public CommandNode
{
public:
  ASSIGN_ARRAYEXPR_MFCALLNode( const RefDNode& refNode): CommandNode( refNode) {}
  RetCode Run();
  BaseGDL** LExpr( BaseGDL* right);
//   BaseGDL** LExprGrab( BaseGDL* right);
  BaseGDL* Eval();
};
class ASSIGN_REPLACENode: public CommandNode
{
public:
  ASSIGN_REPLACENode( const RefDNode& refNode): CommandNode( refNode) {}
  RetCode Run();
  BaseGDL** LExpr( BaseGDL* right);
//   BaseGDL** LExprGrab( BaseGDL* right);
  BaseGDL* Eval();
};
class PCALL_LIBNode: public CommandNode
{
public:
  PCALL_LIBNode( const RefDNode& refNode): CommandNode( refNode) {}
  RetCode Run();
};
class MPCALLNode: public CommandNode
{
public:
  MPCALLNode( const RefDNode& refNode): CommandNode( refNode) {}
  RetCode Run();
};
class MPCALL_PARENTNode: public CommandNode
{
public:
  MPCALL_PARENTNode( const RefDNode& refNode): CommandNode( refNode) {}
  RetCode Run();
};
class PCALLNode: public CommandNode
{
public:
  PCALLNode( const RefDNode& refNode): CommandNode( refNode) {}
  RetCode Run();
};
class DECNode: public CommandNode
{ public:
  DECNode( const RefDNode& refNode): CommandNode( refNode){}
  BaseGDL* Eval();
  RetCode Run();
};
class INCNode: public CommandNode
{ public:
  INCNode( const RefDNode& refNode): CommandNode( refNode){}
  BaseGDL* Eval();
  RetCode Run();
};
class POSTDECNode: public DECNode
{ public:
  POSTDECNode( const RefDNode& refNode): DECNode( refNode){}
  BaseGDL* Eval();
};
class POSTINCNode: public INCNode
{ public:
  POSTINCNode( const RefDNode& refNode): INCNode( refNode){}
  BaseGDL* Eval();
};

class ARRAYEXPRNode: public DefaultNode
{
public:
 ARRAYEXPRNode( const RefDNode& refNode): DefaultNode( refNode) {}
 BaseGDL* Eval(); // caller receives ownership
 BaseGDL** LExpr(BaseGDL* r);
 //BaseGDL** LEval(); 
};

class EXPRNode: public DefaultNode
{
public:
 EXPRNode( const RefDNode& refNode): DefaultNode( refNode) {}
 BaseGDL** LEval();
};

class DOTNode: public DefaultNode
{
public:
 DOTNode( const RefDNode& refNode): DefaultNode( refNode) {}
 BaseGDL* Eval();
 BaseGDL** LExpr( BaseGDL* right);

};

#endif
