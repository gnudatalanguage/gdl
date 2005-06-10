#ifndef INC_FMTIn_hpp_
#define INC_FMTIn_hpp_

#include <antlr/config.hpp>
#include "FMTInTokenTypes.hpp"
/* $ANTLR 2.7.4: "format.in.g" -> "FMTIn.hpp"$ */
#include <antlr/TreeParser.hpp>


#include <fstream>
#include <sstream>

#include "envt.hpp"
#include "io.hpp"             // StreamInfo()

#include "fmtnode.hpp"
//#include "getfmtast.hpp"    // for FMTNodeFactory;    

class CUSTOM_API FMTIn : public antlr::TreeParser, public FMTInTokenTypes
{

public:
    FMTIn( RefFMTNode fmt, std::istream* is_, EnvT* e_, int parOffset, 
           BaseGDL* prompt_)
    : antlr::TreeParser(), 
    noPrompt( true), 
    ioss(), 
    is(is_),  
    prompt( prompt_), e( e_), nextParIx( parOffset),
    valIx(0), termFlag(false), nElements(0)
    {
        nParam = e->NParam();

        NextPar();

        format( fmt);
        
        SizeT nextParIxComp = nextParIx;
        SizeT valIxComp = valIx;

        // format reversion
        while( actPar != NULL)
        {
            format_reversion( reversionAnker);
            
            if( (nextParIx == nextParIxComp) && (valIx == valIxComp))   
                throw GDLException("Infinite format loop detected.");
        }
    }
    
private:
    void NextPar()
    {
        valIx = 0;

        restart:
        if( nextParIx < nParam)
        {
            BaseGDL** par = &e->GetPar( nextParIx);
            if( (*par) != NULL)
            {
                if( e->GlobalPar( nextParIx))
                { // defined global
                    actPar = *par;
                    nElements = actPar->ToTransfer();
                }
                else
                { // defined local
                    if( prompt != NULL)
                    { // prompt keyword there -> error
                        throw GDLException( e->CallingNode(),
                            "Expression must be named variable "
                            "in this context: "+e->GetParString( nextParIx));
                    }
                    else
                    { // prompt not there -> put out or ignore
                        if( is == &std::cin) 
                        {
                            (*par)->ToStream( std::cout);
                            std::cout << std::flush;
                            noPrompt = false;
                        }

                        nextParIx++;
                        goto restart;
                    }
                }
            }
            else
            { // undefined
                if( e->LocalPar( nextParIx))
                throw GDLException( e->CallingNode(),
                    "Internal error: Input: UNDEF is local.");

                nElements = 1;
                (*par) = new DFloatGDL( 0.0);
                actPar = *par;
            }
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
//        std::cout << "NextVal("<<n<<")" << std::endl;

        valIx += n;
        if( valIx >= nElements)
            NextPar();

//        std::cout << "valIx:     " << valIx << std::endl;
//        std::cout << "nElements: " << nElements << std::endl;
    }
    
    void GetLine()
    {
	    if( is == &std::cin && noPrompt)
        {
            if( prompt != NULL) 
            {
                prompt->ToStream( std::cout);
                std::cout << std::flush;
            }
            else 
            {
                std::cout << ": " << std::flush;
            }
        }
        else 
        {
            if( is->eof())
            throw GDLException( e->CallingNode(), "End of file encountered. "+
                StreamInfo( is));
        }

//        std::string retStr;
//        getline( *is, retStr);
//        ioss.str( retStr);
        
        std::string initStr("");
        ioss.str( initStr);
//        ioss.seekg( 0);
//        ioss.seekp( 0);
        ioss.rdbuf()->pubseekpos(0,std::ios_base::in | std::ios_base::out);
        ioss.clear();
        is->get( *ioss.rdbuf());

        if ( (is->rdstate() & std::ifstream::failbit ) != 0 )
        {
            if ( (is->rdstate() & std::ifstream::eofbit ) != 0 )
            throw GDLException( e->CallingNode(),
                "End of file encountered. "+
			    StreamInfo( is));
      
            if ( (is->rdstate() & std::ifstream::badbit ) != 0 )
            throw GDLException( e->CallingNode(),
                "Error reading line. "+
			    StreamInfo( is));
      
            is->clear();
            is->get();   // remove delimiter
            return;     // assuming rdbuf == ""
        }

        if( !is->good())
        throw GDLException( e->CallingNode(), "Error reading data. "+
            StreamInfo( is));

        if( !is->eof()) is->get(); // remove delimiter

        //***
//        std::cout << "FMTIn::GetLine: " << ioss.str() << "." << std::endl;
//        std::cout << "tellg: " << ioss.tellg() << std::endl;
    }

    bool noPrompt;

    std::stringstream ioss;
    std::istream* is;
    BaseGDL* prompt;

    EnvT*    e;
    SizeT   nextParIx;
    SizeT   valIx;

    bool termFlag;

    SizeT   nParam;
    BaseGDL* actPar;
    SizeT nElements;

    RefFMTNode reversionAnker;
public:
	FMTIn();
	static void initializeASTFactory( antlr::ASTFactory& factory );
	int getNumTokens() const
	{
		return FMTIn::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return FMTIn::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return FMTIn::tokenNames;
	}
	public: void format(RefFMTNode _t);
	public: void q(RefFMTNode _t);
	public: void f(RefFMTNode _t);
	public: void format_recursive(RefFMTNode _t);
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
	static const int NUM_TOKENS = 76;
#else
	enum {
		NUM_TOKENS = 76
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const antlr::BitSet _tokenSet_1;
};

#endif /*INC_FMTIn_hpp_*/
