#ifndef INC_GDLInterpreter_hpp_
#define INC_GDLInterpreter_hpp_

#include <antlr/config.hpp>
#include "GDLInterpreterTokenTypes.hpp"
/* $ANTLR 2.7.4: "gdlc.i.g" -> "GDLInterpreter.hpp"$ */
#include <antlr/TreeParser.hpp>


#include <map>
#include <iomanip>
//#include <exception>

#include "datatypes.hpp"
#include "objects.hpp"
#include "dpro.hpp"
#include "dnode.hpp"
#include "accessdesc.hpp"
#include "initsysvar.hpp"
#include "gdljournal.hpp"

class CUSTOM_API GDLInterpreter : public antlr::TreeParser, public GDLInterpreterTokenTypes
{

public: 
    enum RetCode {
        RC_OK=0,
        RC_BREAK,
        RC_CONTINUE,
        RC_RETURN,
        RC_ABORT
    };  

    // code in: dinterpreter.cpp
    static bool SearchCompilePro(const std::string& pro);
    static int GetFunIx( const std::string& subName);
    static int GetProIx( const std::string& subName);
    DStructGDL* ObjectStruct( BaseGDL* self, RefDNode mp);
    DStructGDL* ObjectStructCheckAccess( BaseGDL* self, RefDNode mp);

private: 
    // code in: dinterpreter.cpp
    static void SetFunIx( RefDNode& f); // triggers read/compile
    static void SetProIx( RefDNode& f); // triggers read/compile
    static void AdjustTypes( BaseGDL*&, BaseGDL*&);


protected:
    std::istringstream executeLine; // actual interactive executed line

    class RetAllException {};
    
    // code in: dinterpreter.cpp
//    static bool CompleteFileName(std::string& fn); -> str.cpp
    static bool CompileFile(const std::string& f, const std::string& untilPro=""); 
    BaseGDL*  returnValue;  // holding the return value for functions
    BaseGDL** returnValueL; // holding the return value for l_functions

    // override method to gain speed (no checking needed in interpreter)
	void match(antlr::RefAST t, int ttype) {}

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
    DStructDesc* GetStruct(const std::string& name, const RefDNode cN); 

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
    static void FreeHeap( DPtrGDL* p)
    {
        SizeT nEl=p->N_Elements();
        for( SizeT ix=0; ix < nEl; ix++)
        {
            DPtr id= (*p)[ix];
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
    static void ReportError( GDLException& e)
    {
        DString msgPrefix = SysVar::MsgPrefix();

        std::cout << std::flush;
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

        std::cerr << msgPrefix << "Execution halted at:  " << 
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
        
        DumpStack();
    }
    
    static void DumpStack()
    {
        DString msgPrefix = SysVar::MsgPrefix();

        EnvStackT::reverse_iterator upEnv = callStack.rbegin();
        EnvStackT::reverse_iterator env = upEnv++;
        for(; 
            upEnv != callStack.rend();
            ++upEnv, ++env)
        {
            std::cerr << msgPrefix << "                      ";
            std::cerr << std::left << std::setw(16) << (*upEnv)->GetProName();

            std::string file = (*upEnv)->GetFilename();
            if( file != "")
            {
                RefDNode cNode= (*env)->CallingNode();
                if( cNode != static_cast<RefDNode>(antlr::nullAST))
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

    static void DebugMsg( RefDNode _t, const std::string& msg)
    {    
        DString msgPrefix = SysVar::MsgPrefix();

        std::cout << std::flush;
        std::cerr << msgPrefix << msg
        << std::left << std::setw(16) << callStack.back()->GetProName();
        std::string file=callStack.back()->GetFilename();
        if( file != "")
        {
            RefDNode eNode = _t;
            if( eNode != static_cast<RefDNode>(antlr::nullAST))
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

    static void RetAll()
    {
        throw RetAllException();
    }

    static EnvStackT& CallStack() { return callStack;} // the callstack
    
    std::string GetClearActualLine()
    {
        std::string ret = executeLine.str();
        executeLine.str("");
        return ret;
    }

    ~GDLInterpreter()
    {
    }
public:
	GDLInterpreter();
	static void initializeASTFactory( antlr::ASTFactory& factory );
	int getNumTokens() const
	{
		return GDLInterpreter::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return GDLInterpreter::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return GDLInterpreter::tokenNames;
	}
	public:  GDLInterpreter::RetCode  interactive(RefDNode _t);
	public:  GDLInterpreter::RetCode  statement_list(RefDNode _t);
	public: void execute(RefDNode _t);
	public:  BaseGDL*  call_fun(RefDNode _t);
	public:  GDLInterpreter::RetCode  statement(RefDNode _t);
	public:  BaseGDL**  call_lfun(RefDNode _t);
	public: void call_pro(RefDNode _t);
	public: void assignment(RefDNode _t);
	public: void procedure_call(RefDNode _t);
	public: void lib_procedure_call(RefDNode _t);
	public: void decinc_statement(RefDNode _t);
	public:  GDLInterpreter::RetCode  for_statement(RefDNode _t);
	public:  GDLInterpreter::RetCode  repeat_statement(RefDNode _t);
	public:  GDLInterpreter::RetCode  while_statement(RefDNode _t);
	public:  GDLInterpreter::RetCode  if_statement(RefDNode _t);
	public:  GDLInterpreter::RetCode  if_else_statement(RefDNode _t);
	public:  GDLInterpreter::RetCode  case_statement(RefDNode _t);
	public:  GDLInterpreter::RetCode  switch_statement(RefDNode _t);
	public:  GDLInterpreter::RetCode  block(RefDNode _t);
	public:  GDLInterpreter::RetCode  jump_statement(RefDNode _t);
	public: BaseGDL*  expr(RefDNode _t);
	public: BaseGDL**  l_simple_var(RefDNode _t);
	public: BaseGDL**  l_ret_expr(RefDNode _t);
	public: void parameter_def(RefDNode _t,
		EnvT* actEnv
	);
	public: BaseGDL**  l_expr(RefDNode _t,
		BaseGDL* right
	);
	public: BaseGDL*  l_decinc_expr(RefDNode _t,
		int dec_inc
	);
	public: BaseGDL**  l_deref(RefDNode _t);
	public:  BaseGDL**  l_function_call(RefDNode _t);
	public: BaseGDL*  r_expr(RefDNode _t);
	public: BaseGDL*  l_decinc_indexable_expr(RefDNode _t,
		int dec_inc
	);
	public: BaseGDL**  l_defined_simple_var(RefDNode _t);
	public: BaseGDL**  l_sys_var(RefDNode _t);
	public: BaseGDL*  l_decinc_array_expr(RefDNode _t,
		int dec_inc
	);
	public: ArrayIndexListT*  arrayindex_list(RefDNode _t);
	public: BaseGDL*  l_decinc_dot_expr(RefDNode _t,
		int dec_inc
	);
	public: void l_dot_array_expr(RefDNode _t,
		DotAccessDescT* aD
	);
	public: void tag_array_expr(RefDNode _t,
		DotAccessDescT* aD
	);
	public: BaseGDL**  l_indexoverwriteable_expr(RefDNode _t);
	public: BaseGDL**  l_indexable_expr(RefDNode _t);
	public: BaseGDL**  l_array_expr(RefDNode _t,
		BaseGDL* right
	);
	public: BaseGDL**  l_dot_expr(RefDNode _t,
		BaseGDL* right
	);
	public: BaseGDL*  constant(RefDNode _t);
	public: BaseGDL*  array_def(RefDNode _t);
	public:  BaseGDL*  struct_def(RefDNode _t);
	public: BaseGDL*  indexable_expr(RefDNode _t);
	public: BaseGDL*  simple_var(RefDNode _t);
	public: BaseGDL*  sys_var(RefDNode _t);
	public: BaseGDL*  array_expr(RefDNode _t);
	public: void tag_expr(RefDNode _t,
		DotAccessDescT* aD
	);
	public: BaseGDL*  r_dot_indexable_expr(RefDNode _t,
		DotAccessDescT* aD
	);
	public: BaseGDL*  sys_var_nocopy(RefDNode _t);
	public: void r_dot_array_expr(RefDNode _t,
		DotAccessDescT* aD
	);
	public: BaseGDL*  dot_expr(RefDNode _t);
	public: BaseGDL*  assign_expr(RefDNode _t);
	public:  BaseGDL*  function_call(RefDNode _t);
	public:  BaseGDL*  lib_function_call(RefDNode _t);
	public:  BaseGDL**  l_lib_function_call(RefDNode _t);
	public:  BaseGDL**  ref_parameter(RefDNode _t);
	public:  ArrayIndexT  arrayindex_all(RefDNode _t);
	public:  ArrayIndexT  arrayindex_range(RefDNode _t);
	public:  ArrayIndexT  arrayindex_end(RefDNode _t);
	public:  ArrayIndexT  arrayindex(RefDNode _t);
	public:  BaseGDL*  named_struct_def(RefDNode _t);
	public:  BaseGDL*  unnamed_struct_def(RefDNode _t);
public:
	antlr::RefAST getAST()
	{
		return antlr::RefAST(returnAST);
	}
	
protected:
	RefDNode returnAST;
	RefDNode _retTree;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 191;
#else
	enum {
		NUM_TOKENS = 191
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const antlr::BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const antlr::BitSet _tokenSet_2;
};

#endif /*INC_GDLInterpreter_hpp_*/
