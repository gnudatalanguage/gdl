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
//   virtual RetCode   Run(); // program nodes 

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
  
  int getType() { return ttype;}
  void setType( int t) { ttype=t;}
  std::string getText() { return text;}
  int getLine() const { return lineNumber;}
  void SetGotoIx( int ix) { targetIx=ix;}
  
  bool LabelInRange( const int lIx)
  { return (lIx >= labelStart) && (lIx < labelEnd);}
  
  friend class GDLInterpreter;
  friend class DInterpreter;

  friend class NSTRUCNode;

};

class DefaultNode: public ProgNode
{
public:
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

#undef UNDEF
#ifdef UNDEF

class CommandNode: public ProgNode
{

    // code in: dinterpreter.cpp
    static bool SearchCompilePro(const std::string& pro);
    static int GetFunIx( const std::string& subName);
    static int GetProIx( const std::string& subName);
    DStructGDL* ObjectStruct( BaseGDL* self, ProgNodeP mp);
    DStructGDL* ObjectStructCheckAccess( BaseGDL* self, ProgNodeP mp);

private: 
    // code in: dinterpreter.cpp
    static void SetFunIx( ProgNodeP f); // triggers read/compile
    static void SetProIx( ProgNodeP f); // triggers read/compile
    static void AdjustTypes( BaseGDL*&, BaseGDL*&);


protected:
    std::istringstream executeLine; // actual interactive executed line

//     std::vector<BaseGDL*> tmpList;
//     void ClearTmpList()
//     {
//         std::vector<BaseGDL*>::iterator i;
//         for(i = tmpList.begin(); i != tmpList.end(); ++i) 
//             { delete *i;}
//         tmpList.clear();
//     }

    class RetAllException 
    {
        public:
        enum ExCode {
            NONE=0, // normal RETALL
            RUN     // RETALL from .RUN command
        };  

        private:
        ExCode code;

        public:
        RetAllException( ExCode code_=NONE): code( code_) {}

        ExCode Code() { return code;}
    };
    
    // code in: dinterpreter.cpp
//    static bool CompleteFileName(std::string& fn); -> str.cpp
    static bool CompileFile(const std::string& f, const std::string& untilPro=""); 
    BaseGDL*  returnValue;  // holding the return value for functions
    BaseGDL** returnValueL; // holding the return value for l_functions

    bool interruptEnable;

    typedef std::map<SizeT, BaseGDL*> HeapT;
    typedef std::map<SizeT, DStructGDL*> ObjHeapT;

    // the following must be all static because several interpreter might be active
    // the heap for all dynamic variables
    // ease the handling, no memory leaks, gc possible
    static HeapT     heap; 
    static ObjHeapT  objHeap; 

    // index for newly allocated heap variables
    static SizeT objHeapIx;
    static SizeT heapIx;

    static EnvStackT  callStack;

public:
    // triggers read/compile/interpret
    DStructDesc* GetStruct(const std::string& name, const ProgNodeP cN); 

    // the New... functions 'own' their BaseGDL*
    SizeT NewObjHeap( SizeT n=1, DStructGDL* var=NULL)
    {
        SizeT tmpIx=objHeapIx;
        for( SizeT i=0; i<n; i++)
        objHeap.insert( objHeap.end(),
            std::pair<SizeT, DStructGDL*>( objHeapIx++, var));
        return tmpIx;
    }
    SizeT NewHeap( SizeT n=1, BaseGDL* var=NULL)
    {
        SizeT tmpIx=heapIx;
        for( SizeT i=0; i<n; i++)
        heap.insert( heap.end(),
            std::pair<SizeT, BaseGDL*>( heapIx++, var));
        return tmpIx;
    }
    static void FreeObjHeap( DObj id)
    {
        if( id != 0)
        {       
            ObjHeapT::iterator it=objHeap.find( id);
            if  ( it != objHeap.end()) 
            { 
                delete (*it).second;
                objHeap.erase( id);
            }
        }
    }
    static void FreeHeap( DPtr id)
    {
        if( id != 0)
            {
                HeapT::iterator it=heap.find( id);
                if( it != heap.end()) 
                    { 
                        delete (*it).second;
                        heap.erase( id); 
                    }
            }
    }

   static void FreeHeap( DPtrGDL* p)
    {
        SizeT nEl=p->N_Elements();
        for( SizeT ix=0; ix < nEl; ix++)
        {
            DPtr id= (*p)[ix];
            FreeHeap( id);
       }
    }

    class HeapException {};

    static BaseGDL*& GetHeap( DPtr ID)
    {
        HeapT::iterator it=heap.find( ID);
        if( it == heap.end()) throw HeapException();
        return it->second;
    }
    static DStructGDL*& GetObjHeap( DObj ID)
    {
        ObjHeapT::iterator it=objHeap.find( ID);
        if( it == objHeap.end()) throw HeapException();
        return it->second;
    }

    static bool PtrValid( DPtr ID)
    {
        HeapT::iterator it=heap.find( ID);
        return  (it != heap.end());
    }

    static SizeT HeapSize()
    {
        return heap.size();
    }

    static DPtrGDL* GetAllHeap()
    {
        SizeT nEl = heap.size();
        if( nEl == 0) return new DPtrGDL( 0);
        DPtrGDL* ret = new DPtrGDL( dimension( &nEl, 1), BaseGDL::NOZERO);
        SizeT i=0;
        for( HeapT::iterator it=heap.begin(); it != heap.end(); ++it)
        {
            (*ret)[ i++] = it->first;
        }
        return ret;
    }

    static bool ObjValid( DObj ID)
    {
        ObjHeapT::iterator it=objHeap.find( ID);
        return  (it != objHeap.end());
    }

    static SizeT ObjHeapSize()
    {
        return objHeap.size();
    }

    static DObjGDL* GetAllObjHeap()
    {
        SizeT nEl = objHeap.size();
        if( nEl == 0) return new DObjGDL( 0);
        DObjGDL* ret = new DObjGDL( dimension( &nEl, 1), BaseGDL::NOZERO);
        SizeT i=0;
        for( ObjHeapT::iterator it=objHeap.begin(); it != objHeap.end(); ++it)
        {
            (*ret)[ i++] = it->first;
        }
        return ret;
    }

    // name of data
    static const std::string Name( BaseGDL* p) // const
    {
        return callStack.back()->GetString( p);
    }

    static const std::string Name( BaseGDL** p) // const
    {
        return "<(Find name not implemented yet)>";
    }

    // compiler (lexer, parser, treeparser) def in dinterpreter.cpp
    static void ReportCompileError( GDLException& e, const std::string& file = "");

    // interpreter
    static void ReportError( GDLException& e, const std::string emsg, 
                             bool dumpStack=true)
    {
        DString msgPrefix = SysVar::MsgPrefix();

        std::cout << std::flush;
        if( dumpStack)
        if( e.Prefix())
        {
            std::cerr << msgPrefix << e.toString() << std::endl;
            lib::write_journal_comment(msgPrefix+e.toString());
        }
        else
        {
            std::cerr << e.toString() << std::endl;
            lib::write_journal_comment(e.toString());
        }

        std::cerr << msgPrefix << emsg << " " << 
        std::left << std::setw(16) << callStack.back()->GetProName();
        std::string file=callStack.back()->GetFilename();
        if( file != "")
        {
            SizeT line = e.getLine();
            if( line != 0)
            {       
                std::cerr << std::right << std::setw(6) << line;
            }
            else
            {
                std::cerr << std::right << std::setw(6) << "";
            }
            std::cerr << std::left << " " << file;
        }
        std::cerr << std::endl;
        
        if( dumpStack) DumpStack( emsg.size() + 1);
    }
    
    static void DumpStack( SizeT w)
    {
        DString msgPrefix = SysVar::MsgPrefix();

        EnvStackT::reverse_iterator upEnv = callStack.rbegin();
        EnvStackT::reverse_iterator env = upEnv++;
        for(; 
            upEnv != callStack.rend();
            ++upEnv, ++env)
        {
            std::cerr << msgPrefix << std::right << std::setw( w) << "";
            std::cerr << std::left << std::setw(16) << (*upEnv)->GetProName();

            std::string file = (*upEnv)->GetFilename();
            if( file != "")
            {
                ProgNodeP cNode= (*env)->CallingNode();
                if( cNode != NULL)
                {       
                    std::cerr << std::right << std::setw(6) << cNode->getLine();
                }
                else
                {
                    std::cerr << std::right << std::setw(6) << "";
                }
                std::cerr << std::left << " " << file;
            }
            std::cerr << std::endl;
        }
    }

    static void DebugMsg( ProgNodeP _t, const std::string& msg)
    {    
        DString msgPrefix = SysVar::MsgPrefix();

        std::cout << std::flush;
        std::cerr << msgPrefix << msg
        << std::left << std::setw(16) << callStack.back()->GetProName();
        std::string file=callStack.back()->GetFilename();
        if( file != "")
        {
            ProgNodeP eNode = _t;
            if( eNode != NULL)
            {       
                std::cerr << std::right << std::setw(6) << eNode->getLine();
            }
            else
            {
                std::cerr << std::right << std::setw(6) << "";
            }
            std::cerr << std::left << " " << file;
        }
        std::cerr << std::endl;
    }

    static void RetAll( RetAllException::ExCode c=RetAllException::NONE)    
    {
        throw RetAllException( c);
    }

    static EnvStackT& CallStack() { return callStack;} // the callstack
    static EnvBaseT*  CallStackBack() { return callStack.back();} 
};

// class ARRAYDEFNode: public CommandNode
// {
// public:
//   /*virtual*/ RetCode   Run();
// 
// };
#endif

#endif
