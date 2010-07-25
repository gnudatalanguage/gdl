#ifndef INC_FMTOut_hpp_
#define INC_FMTOut_hpp_

#include <antlr/config.hpp>
#include "FMTOutTokenTypes.hpp"
/* $ANTLR 2.7.7 (20091222): "format.out.g" -> "FMTOut.hpp"$ */
#include <antlr/TreeParser.hpp>



#include <fstream>
#include <cassert>

//***test
#include "print_tree.hpp"

#include "envt.hpp"

#include "fmtnode.hpp"
//#include "getfmtast.hpp"    // for FMTNodeFactory;    

#include "gdljournal.hpp"

class CUSTOM_API FMTOut : public antlr::TreeParser, public FMTOutTokenTypes
{

public:
    FMTOut( RefFMTNode fmt, std::ostream* os_, EnvT* e_, int parOffset)
    : antlr::TreeParser(), os(os_), e( e_), nextParIx( parOffset),
	valIx(0), termFlag(false), nonlFlag(false), nElements(0)
    {
        std::ostringstream* osLocal;
        std::auto_ptr<std::ostream> osLocalGuard;
        if( *os_ == std::cout)
            {
                osLocal = new std::ostringstream();
                osLocalGuard.reset( osLocal);
                os = osLocal;
            }
        else
            {
                os = os_;
            }

        nParam = e->NParam();

        NextPar();
    
        GDLStream* j = lib::get_journal();

        if( j != NULL && j->OStream() == (*os)) 
            (*os) << lib::JOURNALCOMMENT;

        format( fmt);
        
        SizeT nextParIxComp = nextParIx;
        SizeT valIxComp = valIx;

        // format reversion
        while( actPar != NULL)
        {
           (*os) << '\n';
            
            if( j != NULL && j->OStream() == (*os)) 
                (*os) << lib::JOURNALCOMMENT;

            format_reversion( reversionAnker);            
 
           if( (nextParIx == nextParIxComp) && (valIx == valIxComp))   
                throw GDLException("Infinite format loop detected.");
         }
        
        os->seekp( 0, std::ios_base::end);

        if( !nonlFlag)
            {
                (*os) << '\n';
            }
        (*os) << std::flush;

        if( *os_ == std::cout)
            {
                os = os_;
                (*os) << osLocal->str();
                (*os) << std::flush;
            }
    }
    
private:
    void NextPar()
    {
        valIx = 0;
        if( nextParIx < nParam)
        {
            actPar = e->GetPar( nextParIx);
            if( actPar != NULL)
            nElements = actPar->ToTransfer();
            else
            nElements = 0;
        } 
        else 
        {
            actPar = NULL;
            nElements = 0;
        }
        nextParIx++;
    }

    void NextVal( SizeT n=1)
    {
        valIx += n;
        if( valIx >= nElements)
            NextPar();
    }
    
    std::ostream* os;
    EnvT*    e;
    SizeT   nextParIx;
    SizeT   valIx;

    bool termFlag;
    bool nonlFlag;

    SizeT   nParam;
    BaseGDL* actPar;
    SizeT nElements;

    RefFMTNode reversionAnker;
public:
	FMTOut();
	static void initializeASTFactory( antlr::ASTFactory& factory );
	int getNumTokens() const
	{
		return FMTOut::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return FMTOut::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return FMTOut::tokenNames;
	}
	public: void format(RefFMTNode _t);
	public: void q(RefFMTNode _t);
	public: void f(RefFMTNode _t);
	public: void format_reversion(RefFMTNode _t);
	public: void f_csubcode(RefFMTNode _t);
	public: void x(RefFMTNode _t);
	public: void csubcode(RefFMTNode _t);
public:
	antlr::RefAST getAST()
	{
		return antlr::RefAST(returnAST);
	}
	
protected:
	RefFMTNode returnAST;
	RefFMTNode _retTree;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 78;
#else
	enum {
		NUM_TOKENS = 78
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const antlr::BitSet _tokenSet_1;
};

#endif /*INC_FMTOut_hpp_*/
