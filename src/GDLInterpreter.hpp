#ifndef INC_GDLInterpreter_hpp_
#define INC_GDLInterpreter_hpp_

#include <antlr/config.hpp>
#include "GDLInterpreterTokenTypes.hpp"
/* $ANTLR 2.7.7 (2006-11-01): "gdlc.i.g" -> "GDLInterpreter.hpp"$ */
#include <antlr/TreeParser.hpp>


    // antlr header

    // make sure it gets included before the 'tweak'
#include "GDLParser.hpp" 
#include "GDLTreeParser.hpp" 

#include <map>
#include <iomanip>
//#include <exception>

#include "datatypes.hpp"
#include "objects.hpp"
#include "dpro.hpp"
#include "accessdesc.hpp"
#include "initsysvar.hpp"
#include "gdljournal.hpp"

//class ProgNode;
//typedef ProgNode* ProgNodeP;

// tweaking ANTLR
#define RefAST( xxx)     ConvertAST( xxx) /* antlr::RefAST( Ref type)  */

// print out AST tree
//#define GDL_DEBUG
//#undef GDL_DEBUG
//#define GDL_DEBUG_HEAP
bool IsEnabledGC(); // defined in GDLInterpreter.hpp with EnableGC(bool);
void EnableGC(bool);

class CUSTOM_API GDLInterpreter : public antlr::TreeParser, public GDLInterpreterTokenTypes
{

private:
    // ASTNULL replacement
    static ProgNode  NULLProgNode;
    static ProgNodeP NULLProgNodeP;

    friend class BaseGDL;
    friend class ProgNode;
    friend class ARRAYDEFNode;
    friend class ARRAYDEF_GENERALIZED_INDGENNode;
    friend class STRUCNode;
    friend class NSTRUCNode;
    friend class NSTRUC_REFNode;
    friend class ASSIGNNode;
    friend class ASSIGN_ARRAYEXPR_MFCALLNode;
    friend class ASSIGN_REPLACENode;
    friend class PCALL_LIBNode;//: public CommandNode
    friend class MPCALLNode;//: public CommandNode
    friend class MPCALL_PARENTNode;//: public CommandNode
    friend class PCALLNode;//: public CommandNode
    friend class RETFNode;
    friend class RETPNode;
    friend class FORNode;
    friend class FOR_LOOPNode;
    friend class FOREACHNode;
    friend class FOREACH_LOOPNode;
    friend class FOREACH_INDEXNode;
    friend class FOREACH_INDEX_LOOPNode;
    friend class FOR_STEPNode;
    friend class FOR_STEP_LOOPNode;
    friend class KEYDEFNode;
    friend class KEYDEF_REFNode;
    friend class KEYDEF_REF_CHECKNode;
    friend class KEYDEF_REF_EXPRNode;
    friend class REFNode;
    friend class REF_CHECKNode;
    friend class REF_EXPRNode;
    friend class ParameterNode;
    friend class REFVNNode;
    friend class REF_CHECKVNNode;
    friend class REF_EXPRVNNode;
    friend class ParameterVNNode;
    friend class WRAPPED_FUNNode;
    friend class WRAPPED_PRONode;

public: 

//     RetCode returnCode;    
    ProgNodeP GetNULLProgNodeP() const { return NULLProgNodeP;}


    void SetRetTree( ProgNodeP rT)
    {
        this->_retTree = rT;
    }
    ProgNodeP GetRetTree() const
    {
        return this->_retTree;
    }
//     void SetReturnCode( RetCode rC)
//     {
//         this->returnCode = rC;
//     }
    
//     enum RetCode {
//         RC_OK=0,
//         RC_BREAK,
//         RC_CONTINUE,
//         RC_RETURN, 
//         RC_ABORT, // checked as retCode >= RC_RETURN
//     };  

    // code in: dinterpreter.cpp
    // procedure (searchForPro == true) or function (searchForPro == false)
    static bool SearchCompilePro(const std::string& pro, bool searchForPro); 
    static int GetFunIx( ProgNodeP);
    static int GetFunIx( const std::string& subName);
    static int GetProIx( ProgNodeP);//const std::string& subName);
    static int GetProIx( const std::string& subName);
    DStructGDL* ObjectStruct( DObjGDL* self, ProgNodeP mp);
    void SetRootR( ProgNodeP tt, DotAccessDescT* aD, BaseGDL* r, ArrayIndexListT* aL);
    void SetRootL( ProgNodeP tt, DotAccessDescT* aD, BaseGDL* r, ArrayIndexListT* aL);
    // DStructGDL* ObjectStructCheckAccess( DObjGDL* self, ProgNodeP mp);
    // DStructDesc* GDLObjectDesc( DObjGDL* self, ProgNodeP mp);

    // code in: dinterpreter.cpp
    static void SetFunIx( ProgNodeP f); // triggers read/compile


private: 

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
            NONE=0 // normal RETALL
            ,RUN   // RETALL from .RUN command
            ,RESET // RETALL from .RESET command
            ,FULL_RESET // RETALL from .FULL_RESET command
        };  

        private:
        ExCode code;

        public:
        RetAllException( ExCode code_=NONE): code( code_) {}

        ExCode Code() { return code;}
    };
    
    // code in: dinterpreter.cpp
//    static bool CompleteFileName(std::string& fn); -> str.cpp

    BaseGDL*  returnValue;  // holding the return value for functions
    BaseGDL** returnValueL; // holding the return value for l_functions

    bool interruptEnable;

public:

    bool InterruptEnable() const { return interruptEnable;}
    // procedure (searchForPro == true) or function (searchForPro == false)
    static bool CompileFile(const std::string& f, 
                            const std::string& untilPro="",
                            bool searchForPro=true); 

    typedef RefHeap<BaseGDL> RefBaseGDL;
    typedef RefHeap<DStructGDL> RefDStructGDL;

    typedef std::map<SizeT, RefBaseGDL> HeapT;
    typedef std::map<SizeT, RefDStructGDL> ObjHeapT;

protected:
//     typedef std::map<SizeT, BaseGDL*> HeapT;
//     typedef std::map<SizeT, DStructGDL*> ObjHeapT;

    // the following must be all static because several interpreter might be active
    // the heap for all dynamic variables
    // ease the handling, no memory leaks, gc possible
    static HeapT     heap; 
    static ObjHeapT  objHeap; 

    // index for newly allocated heap variables
    static SizeT objHeapIx;
    static SizeT heapIx;

    static EnvStackT  callStack; 

    static DLong stepCount;


// smuggle optimizations in
//#include "GDLInterpreterOptimized.inc"


public:
    // triggers read/compile/interpret
    DStructDesc* GetStruct(const std::string& name, const ProgNodeP cN); 

//     bool Called( std::string proName)
//     {
//         for( EnvStackT::reverse_iterator env = callStack.rbegin();
//             env != callStack.rend();
//             ++env)
//             {
//                 //std::cout <<  (*env)->GetPro()->ObjectFileName() << std::endl;
//                 if( proName == (*env)->GetPro()->ObjectFileName()) return true;
//             }
//         return false;
//     }

//   static bool IsEnabledGC() { return enable_GC;}
	static bool IsEnabledGC() { return true;}

    // the New... functions 'own' their BaseGDL*
    SizeT NewObjHeap( SizeT n=1, DStructGDL* var=NULL)
    {
        SizeT tmpIx=objHeapIx;
        for( SizeT i=0; i<n; i++)
        objHeap.insert( objHeap.end(),
            std::pair<SizeT, RefDStructGDL>( objHeapIx++, (DStructGDL*)var));
        return tmpIx;
    }
    SizeT NewHeap( SizeT n=1, BaseGDL* var=NULL)
    {
        SizeT tmpIx=heapIx;
        for( SizeT i=0; i<n; i++)
        heap.insert( heap.end(),
            std::pair<SizeT, RefBaseGDL>( heapIx++, var));
        return tmpIx;
    }
    static void FreeObjHeapDirect( DObj id, ObjHeapT::iterator it)
    {
        BaseGDL* del = (*it).second.get();
        objHeap.erase( id); 
        delete del;
        // delete (*it).second.get();
        // objHeap.erase( id);
    }
    static void FreeObjHeap( DObj id)
    {
        if( id != 0)
        {       
            ObjHeapT::iterator it=objHeap.find( id);
            if  ( it != objHeap.end()) 
            { 
                FreeObjHeapDirect( id, it);
                // delete (*it).second.get();
                // objHeap.erase( id);
            }
        }
    }
    static void FreeHeapDirect( DPtr id, HeapT::iterator it)
    {
        BaseGDL* del = (*it).second.get();
        heap.erase( id); 
        delete del;
        // delete (*it).second.get();
        // // useless because of next: (*it).second.get() = NULL;
        // heap.erase( id); 
    }
    static void FreeHeap( DPtr id)
    {
        if( id != 0)
            {
                HeapT::iterator it=heap.find( id);
                if( it != heap.end()) 
                    { 
                        FreeHeapDirect( id, it);
                        // delete (*it).second.get();
                        // heap.erase( id); 
                    }
            }
    }
    static void HeapErase( DPtr id) // for LIST
    {
        if( id != 0)
            {
              heap.erase( id); 
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

   static DByteGDL* IsEnabledGC( DPtrGDL* p)
   {
		SizeT nEl=p->N_Elements();
		if( nEl == 0) return new DByteGDL( 0);
		DByteGDL* ret = new DByteGDL( p->Dim());
		Guard<DByteGDL> guard(ret);
		for( SizeT ix=0; ix < nEl; ix++)
		{
			DPtr id= (*p)[ix];
		   if( id != 0)
			   {
			   HeapT::iterator it=heap.find( id);
			   if( it != heap.end() and (*it).second.IsEnabledGC())
					(*ret)[ix] = 1;
			   }
		}
		return guard.release();
   }
   static DByteGDL* IsEnabledGCObj( DObjGDL* p)
   {
        SizeT nEl=p->N_Elements();
        if( nEl == 0) return new DByteGDL( 0);
        DByteGDL* ret = new DByteGDL( p->Dim());
        Guard<DByteGDL> guard(ret);
        for( SizeT ix=0; ix < nEl; ix++)
   {
            DObj id= (*p)[ix];
		   if( id != 0)
			   {
			   ObjHeapT::iterator it=objHeap.find( id);
			   if( it != objHeap.end() and (*it).second.IsEnabledGC())
					(*ret)[ix] = 1;
			   }
        }
        return guard.release();
   }

   static void EnableGC( DPtr id, bool set=true)
   {
       if( id != 0)
           {
		   HeapT::iterator it=heap.find( id);
		   if( it != heap.end()) (*it).second.EnableGC(set);
           }
   }
   static void EnableGC( DPtrGDL* p, bool set=true)
   {
        SizeT nEl=p->N_Elements();
        for( SizeT ix=0; ix < nEl; ix++)
        {
            DPtr id= (*p)[ix];
            EnableGC( id, set);
       }
   }
   static void EnableAllGC() {
        SizeT nEl = heap.size();
        for( HeapT::iterator it=heap.begin(); it != heap.end(); ++it)
			it->second.EnableGC(true);
        nEl = objHeap.size();
        for( ObjHeapT::iterator it=objHeap.begin(); it != objHeap.end(); ++it)
			it->second.EnableGC(true);
   }

   static void EnableGCObj( DObj id, bool set=true)
   {
       if( id != 0)
           {
		   ObjHeapT::iterator it=objHeap.find( id);
		   if( it != objHeap.end()) (*it).second.EnableGC(set);
           }
   }
   static void EnableGCObj( DObjGDL* p, bool set=true)
   {
        SizeT nEl=p->N_Elements();
        for( SizeT ix=0; ix < nEl; ix++)
        {
            DObj id= (*p)[ix];
            EnableGCObj( id, set);
       }
   }

   static void DecRef( DPtr id)
   {
       if( id != 0 and IsEnabledGC())
           {
#ifdef GDL_DEBUG_HEAP
               std::cout << "-- <PtrHeapVar" << id << ">" << std::endl; 
#endif
               HeapT::iterator it=heap.find( id);
               if( it != heap.end()) 
                   { 
                       if( (*it).second.Dec() and (*it).second.IsEnabledGC() )
                           {
#ifdef GDL_DEBUG_HEAP
                               std::cout << "Out of scope (garbage collected): <PtrHeapVar" << id 
                                         << ">"
                                         << " at: " << callStack.back()->GetProName()
                                         << "  line: " << callStack.back()->GetLineNumber()
                                         << std::endl; 
#endif
                               FreeHeapDirect( id, it);
                           }
#ifdef GDL_DEBUG_HEAP
                       else
                           std::cout << "<PtrHeapVar" << id << "> = " << (*it).second.Count() << std::endl; 
#endif
                   }
           }
   }
   static void DecRef( DPtrGDL* p)
    {
        SizeT nEl=p->N_Elements();
        for( SizeT ix=0; ix < nEl; ix++)
        {
            DPtr id= (*p)[ix];
            DecRef( id);
       }
    }
   static void DecRefObj( DObj id)
    {
        if( id != 0 and IsEnabledGC())
            {
#ifdef GDL_DEBUG_HEAP
std::cout << "-- <ObjHeapVar" << id << ">" << std::endl; 
#endif
                ObjHeapT::iterator it=objHeap.find( id);
                if( it != objHeap.end()) 
                    { 
                       if( (*it).second.Dec() and (*it).second.IsEnabledGC() )
                           {
#ifdef GDL_DEBUG_HEAP
                               std::cout << "Out of scope (garbage collected): <ObjHeapVar" << id 
                                         << ">"
                                         << " at: " << callStack.back()->GetProName()
                                         << "  line: " << callStack.back()->GetLineNumber()
                                         << std::endl; 
#endif
                               callStack.back()->ObjCleanup( id);
//                             FreeObjHeapDirect( id, it);
                           }
#ifdef GDL_DEBUG_HEAP
                        else
std::cout << "<ObjHeapVar" << id << "> = " << (*it).second.Count() << std::endl; 
#endif
                        
                     }
            }
    }
   static void DecRefObj( DObjGDL* p)
    {
        SizeT nEl=p->Size();//N_Elements();
        for( SizeT ix=0; ix < nEl; ix++)
        {
            DObj id= (*p)[ix];
            DecRefObj( id);
       }
    }
   static void IncRef( DPtr id)
    {
        if( id != 0 and IsEnabledGC())
            {
#ifdef GDL_DEBUG_HEAP
std::cout << "++ <PtrHeapVar" << id << ">" << std::endl; 
#endif
                HeapT::iterator it=heap.find( id);
                if( it != heap.end()) 
                    { 
                        (*it).second.Inc(); 
#ifdef GDL_DEBUG_HEAP
std::cout << "<PtrHeapVar" << id << "> = " << (*it).second.Count() << std::endl; 
#endif
                    }
            }
    }
   static void AddRef( DPtr id, SizeT add)
    {
        if( id != 0)
            {
#ifdef GDL_DEBUG_HEAP
std::cout << add << " + <PtrHeapVar" << id << ">" << std::endl; 
#endif
                HeapT::iterator it=heap.find( id);
                if( it != heap.end()) 
                    { 
                        (*it).second.Add(add); 
                    }
            }
    }
   static SizeT RefCountHeap( DPtr id)
    {
		SizeT result = 0;
        if( id != 0)
            {
                HeapT::iterator it=heap.find( id);
                if( it != heap.end()) result = (*it).second.Count();
			}
		return result;
	   }
   static SizeT RefCountHeapObj( DObj id)
    {
		SizeT result = 0;
        if( id != 0)
            {
                ObjHeapT::iterator it=objHeap.find( id);
                if( it != objHeap.end()) result = (*it).second.Count();
			}
		return result;
	   }
   static void IncRef( DPtrGDL* p)
    {
        SizeT nEl=p->N_Elements();
        for( SizeT ix=0; ix < nEl; ix++)
        {
            DPtr id= (*p)[ix];
            IncRef( id);
       }
    }
   static void IncRefObj( DObj id)
    {
        if( id != 0 and IsEnabledGC())
            {
#ifdef GDL_DEBUG_HEAP
std::cout << "++ <ObjHeapVar" << id << ">" << std::endl; 
#endif
                ObjHeapT::iterator it=objHeap.find( id);
                if( it != objHeap.end()) 
                    { 
                        (*it).second.Inc(); 
                    }
            }
    }
   static void AddRefObj( DObj id, SizeT add)
    {
        if( id != 0)
            {
#ifdef GDL_DEBUG_HEAP
std::cout << add << " + <ObjHeapVar" << id << ">" << std::endl; 
#endif
                ObjHeapT::iterator it=objHeap.find( id);
                if( it != objHeap.end()) 
                    { 
                        (*it).second.Add(add); 
                    }
            }
    }
   static void IncRefObj( DObjGDL* p)
    {
        SizeT nEl=p->Size();//N_Elements();
        for( SizeT ix=0; ix < nEl; ix++)
        {
            DObj id= (*p)[ix];
            IncRefObj( id);
       }
    }

    class HeapException {};

    static BaseGDL*& GetHeap( DPtr ID)
    {
        HeapT::iterator it=heap.find( ID);
        if( it == heap.end()) 
            throw HeapException();
        return it->second.get();
    }
    static BaseGDL* GetHeapNoThrow( DPtr ID)
    {
        HeapT::iterator it=heap.find( ID);
        if( it == heap.end())  return NULL;
        return it->second.get();
    }
    static DStructGDL*& GetObjHeap( DObj ID)
    {
        ObjHeapT::iterator it=objHeap.find( ID);
        if( it == objHeap.end()) 
            throw HeapException();
        return it->second.get();
    }

    // for overload functions
    static DSubUD* GetObjHeapOperator( DObj ID, int opIx)
    {
        if( ID == 0) return NULL;
        ObjHeapT::iterator it=objHeap.find( ID);
        if( it == objHeap.end()) return NULL;
        return it->second.get()->Desc()->GetOperator( opIx);
    }
    static DStructGDL* GetObjHeapNoThrow( DObj ID)
    {
        ObjHeapT::iterator it=objHeap.find( ID);
        if( it == objHeap.end()) return NULL;
        return it->second.get();
    }
//     static DStructGDL*& GetObjHeap( DObj ID, ObjHeapT::iterator& it)
//     {
// //         ObjHeapT::iterator it=objHeap.find( ID);
//         it=objHeap.find( ID);
//         if( it == objHeap.end()) throw HeapException();
//         return it->second.get();
//     }

    static bool PtrValid( DPtr ID)
    {
        HeapT::iterator it=heap.find( ID);
        return  (it != heap.end());
    }

    static SizeT HeapSize()
    {
        return heap.size();
    }

    static DPtr FindInHeap( BaseGDL** p)
    {
        for( HeapT::iterator it=heap.begin(); it != heap.end(); ++it)
        {
            if( &it->second.get() == p)
                return it->first;
        }
        return 0;
    }
    // static BaseGDL** GetPtrToHeap( BaseGDL* p)
    // {
    //     for( HeapT::iterator it=heap.begin(); it != heap.end(); ++it)
    //     {
    //         if( it->second.get() == p)
    //             return &it->second.get();
    //     }
    //     return NULL;
    // }
    static DPtrGDL* GetAllHeap()
    {
        SizeT nEl = heap.size();
        if( nEl == 0) return new DPtrGDL( 0);
        DPtrGDL* ret = new DPtrGDL( dimension( &nEl, 1), BaseGDL::NOZERO);
        SizeT i=0;
        for( HeapT::iterator it=heap.begin(); it != heap.end(); ++it)
        {
            IncRef( it->first);
            (*ret)[ i++] = it->first;
        }
        return ret;
    }

    // no ref counting here
    static std::vector<DPtr>* GetAllHeapSTL()
    {
        SizeT nEl = heap.size();
        if( nEl == 0) return new std::vector<DPtr>();
        std::vector<DPtr>* ret = new std::vector<DPtr>( nEl);
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

//     static DObj FindInObjHeap( BaseGDL** p)
//     {
//         for( ObjHeapT::iterator it=objHeap.begin(); it != objHeap.end(); ++it)
//         {
//             if( &it->second == reinterpret_cast<DStructGDL**>(p))
//                 return it->first;
//         }
//         return 0;
//     }
    static DObjGDL* GetAllObjHeap()
    {
        SizeT nEl = objHeap.size();
        if( nEl == 0) return new DObjGDL( 0);
        DObjGDL* ret = new DObjGDL( dimension( &nEl, 1), BaseGDL::NOZERO);
        SizeT i=0;
        for( ObjHeapT::iterator it=objHeap.begin(); it != objHeap.end(); ++it)
        {
            IncRefObj( it->first);
            (*ret)[ i++] = it->first;
        }
        return ret;
    }

    // no ref counting here
    static std::vector<DObj>* GetAllObjHeapSTL()
    {
        SizeT nEl = objHeap.size();
        if( nEl == 0) return new std::vector<DObj>();
        std::vector<DObj>* ret = new std::vector<DObj>( nEl);
        SizeT i=0;
        for( ObjHeapT::iterator it=objHeap.begin(); it != objHeap.end(); ++it)
        {
            (*ret)[ i++] = it->first;
        }
        return ret;
    }


    static void ResetHeap() // purges both heaps
    {
        for( HeapT::iterator it=heap.begin(); it != heap.end(); ++it)
        {
           delete (*it).second.get();
           heap.erase( it->first); 
        }
        for( ObjHeapT::iterator it=objHeap.begin(); it != objHeap.end(); ++it)
        {
            delete (*it).second.get();
            objHeap.erase( it->first); 
        }
// The counters are reset for easier human readability.
       heapIx = 1;
       objHeapIx = 1;
    }

    // name of data
    static const std::string Name( BaseGDL* p) // const
    {
        return callStack.back()->GetString( p);
    }

    static const std::string Name( BaseGDL** p) // const
    {
        assert( *p == NULL);
        DPtr h = FindInHeap( p);
        if( h != 0) return std::string("<PtrHeapVar")+i2s(h)+">";
//         DObj o = FindInObjHeap( p);
//         if( o != 0) return std::string("<ObjHeapVar")+i2s(o)+">";
        return "<(ptr to undefined expression not found on the heap)>";
    }




    // compiler (lexer, parser, treeparser) def in dinterpreter.cpp
    static void ReportCompileError( GDLException& e, const std::string& file = "");

    // interpreter
    static void ReportError( GDLException& e, const std::string &emsg, 
                             bool dumpStack=true)
    {
        DString msgPrefix = SysVar::MsgPrefix();

        std::cout << std::flush;
        if( dumpStack)
            {
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

        // EnvStackT::reverse_iterator upEnv = callStack.rbegin();
        // //EnvStackT::reverse_iterator env = upEnv++;
        // upEnv++;
        // for(; 
        //     upEnv != callStack.rend();
        //     ++upEnv /*,++env*/)
        
        long actIx = callStack.size() - 2;
        for( ; actIx >= 0; --actIx)
        {
            EnvStackT::pointer_type upEnv = callStack[ actIx]; 

            std::cerr << msgPrefix << std::right << std::setw( w) << "";
            std::cerr << std::left << std::setw(16) << upEnv->GetProName();

            std::string file = upEnv->GetFilename();
            if( file != "")
            {              
//                 ProgNodeP cNode= (*env)->CallingNode();
//                 if( cNode != NULL)
//                 {       
//                     std::cerr << std::right << std::setw(6) << cNode->getLine();
//                 }
//                 else
//                 {
//                     std::cerr << std::right << std::setw(6) << "";
//                 }                
//                 ProgNodeP cNode= (*env)->CallingNode();
//                 if( cNode != NULL && cNode->getLine() != 0)
//                 {       
//                     (*upEnv)->SetLineNumber( cNode->getLine());
//                 }

                int lineNumber = upEnv->GetLineNumber();
                if( lineNumber != 0)
                {       
                    std::cerr << std::right << std::setw(6) << lineNumber;
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
//    static EnvBaseT*  CallStackBack() { return callStack.back();} 
    static EnvUDT*  CallStackBack() { return callStack.back();} 
    
    std::string GetClearActualLine()
    {
        std::string ret = executeLine.str();
        executeLine.str("");
        return ret;
    }

    RetCode NewInterpreterInstance(SizeT lineOffset); // code in dinterpreter.cpp

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
	public:  RetCode  interactive(ProgNodeP _t);
	public:  RetCode  statement(ProgNodeP _t);
	public:  RetCode  execute(ProgNodeP _t);
	public:  RetCode  statement_list(ProgNodeP _t);
	public:  BaseGDL*  call_fun(ProgNodeP _t);
	public:  BaseGDL**  call_lfun(ProgNodeP _t);
	public: void call_pro(ProgNodeP _t);
	public: BaseGDL**  l_deref(ProgNodeP _t);
	public: BaseGDL**  l_decinc_indexable_expr(ProgNodeP _t,
		 BaseGDL*& res
	);
	public:  BaseGDL**  l_function_call_internal(ProgNodeP _t);
	public: BaseGDL**  l_defined_simple_var(ProgNodeP _t);
	public: BaseGDL**  l_sys_var(ProgNodeP _t);
	public: BaseGDL**  l_decinc_array_expr(ProgNodeP _t,
		int dec_inc, BaseGDL*& res
	);
	public: BaseGDL*  l_decinc_dot_expr(ProgNodeP _t,
		int dec_inc
	);
	public: BaseGDL**  l_decinc_expr(ProgNodeP _t,
		int dec_inc, BaseGDL*& res
	);
	public: BaseGDL*  expr(ProgNodeP _t);
	public: BaseGDL*  indexable_expr(ProgNodeP _t);
	public: BaseGDL*  indexable_tmp_expr(ProgNodeP _t);
	public:  BaseGDL*  lib_function_call_internal(ProgNodeP _t);
	public: BaseGDL**  l_expr_internal(ProgNodeP _t,
		BaseGDL* right
	);
	public: BaseGDL*  tmp_expr(ProgNodeP _t);
	public: BaseGDL**  l_simple_var(ProgNodeP _t);
	public: void parameter_def(ProgNodeP _t,
		EnvBaseT* actEnv
	);
	public: BaseGDL*  r_expr(ProgNodeP _t);
	public: BaseGDL**  l_indexable_expr(ProgNodeP _t);
	public:  BaseGDL**  l_arrayexpr_mfcall_as_mfcall(ProgNodeP _t);
	public: BaseGDL**  unused_l_array_expr(ProgNodeP _t,
		BaseGDL* right
	);
	public: ArrayIndexListT*  arrayindex_list(ProgNodeP _t,
		 bool noAssoc
	);
	public: void l_dot_array_expr(ProgNodeP _t,
		DotAccessDescT* aD
	);
	public: BaseGDL**  l_arrayexpr_mfcall(ProgNodeP _t,
		BaseGDL* right
	);
	public: void tag_expr(ProgNodeP _t,
		DotAccessDescT* aD
	);
	public: void tag_array_expr(ProgNodeP _t,
		DotAccessDescT* aD
	);
	public: BaseGDL*  r_dot_indexable_expr(ProgNodeP _t,
		DotAccessDescT* aD
	);
	public: void r_dot_array_expr(ProgNodeP _t,
		DotAccessDescT* aD
	);
	public: BaseGDL*  assign_expr(ProgNodeP _t);
	public:  BaseGDL*  unused_function_call(ProgNodeP _t);
	public:  BaseGDL*  lib_function_call_retnew_internal(ProgNodeP _t);
	public: BaseGDL*  simple_var(ProgNodeP _t);
	public: BaseGDL*  sys_var(ProgNodeP _t);
	public: BaseGDL**  l_arrayexpr_mfcall_as_arrayexpr(ProgNodeP _t,
		BaseGDL* right
	);
	public: void parameter_def_n_elements(ProgNodeP _t,
		EnvBaseT* actEnv
	);
	public: void parameter_def_nocheck(ProgNodeP _t,
		EnvBaseT* actEnv
	);
	public: void arrayindex_list_overload(ProgNodeP _t,
		IxExprListT& indexList
	);
public:
	antlr::RefAST getAST()
	{
		return antlr::RefAST(returnAST);
	}
	
protected:
	ProgNodeP returnAST;
	ProgNodeP _retTree;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 238;
#else
	enum {
		NUM_TOKENS = 238
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
};

#endif /*INC_GDLInterpreter_hpp_*/
