#ifndef INC_GDLLexer_hpp_
#define INC_GDLLexer_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (2006-11-01): "gdlc.g" -> "GDLLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "GDLTokenTypes.hpp"
#include <antlr/CharScanner.hpp>

#include <fstream>

#include "GDLParser.hpp"

#include "str.hpp"
#include "dnodefactory.hpp"
#include "objects.hpp"
#include "initsysvar.hpp"

#include <antlr/TokenStreamSelector.hpp>

#include <antlr/SemanticException.hpp>
#include <antlr/NoViableAltForCharException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/CharInputBuffer.hpp>

//#include "dinterpreter.hpp"

// defintion in dinterpreter.cpp
//void SetActualCompileOpt( unsigned int cOpt);

class CUSTOM_API GDLLexer : public antlr::CharScanner, public GDLTokenTypes
{

  // Stuff for include files (@filename)
  private:
#if (__cplusplus >= 201103L)
    std::unique_ptr<std::ifstream>    inputFile; // stores ifsteam* and deletes 
#else
    std::auto_ptr<std::ifstream>    inputFile; // stores ifsteam* and deletes 
#endif
                                     // it when it is deleted itself
  
    antlr::TokenStreamSelector*     selector; 
    GDLLexer*                       mainLexerPtr;
    GDLParser*                      parserPtr;

    int                             lineContinuation;

  public:
    ~GDLLexer() 
    {
        if( mainLexerPtr != this)
            selector->pop(); // return to old lexer/stream
        else
        {
            delete parserPtr;
            delete selector;
        }
    }

    // main lexer constructor
    GDLLexer( std::istream& in, const std::string &f, unsigned int compileOptIn,
        const std::string &pro="", bool searchForPro=true) 
    : antlr::CharScanner(new antlr::CharBuffer(in),false),
      lineContinuation( 0)
//    : antlr::CharScanner(in)
    {
        setCaseSensitive(false);
        initLiterals();
  
        selector=     new antlr::TokenStreamSelector();
        mainLexerPtr= this;
        parserPtr=    new GDLParser( *selector, pro, searchForPro, compileOptIn);

        parserPtr->setFilename(f);
        parserPtr->initializeASTFactory( DNodeFactory);
        parserPtr->setASTFactory( &DNodeFactory );
//        parserPtr->setASTNodeFactory( DNode::factory );
        
        selector->addInputStream(this, f);
        selector->select(f); // start with main lexer
        
        // set line number to 0 in interactive mode
        if( f == "")
            { 
                setLine(0);
            }
//        p=parserPtr;
    }

    // sublexer constructor
    GDLLexer( std::ifstream& in, const std::string& name,
        GDLLexer* parent)
    : antlr::CharScanner(new antlr::CharBuffer(in),false),
      inputFile( &in)
    //    : antlr::CharScanner(new antlr::CharInputBuffer(in))
    //    : antlr::CharScanner(new antlr::CharBuffer(in))
    {
        setCaseSensitive(false);
        initLiterals();
        
        selector=     parent->selector;
        mainLexerPtr= parent->mainLexerPtr;
        parserPtr=    parent->parserPtr;
        
//        inputFile.Reset( &in); // make sure file 
//                               // gets deleted (closed) 
// 	  				             // when lexer finish

		// make sure errors are reported in right file
		setFilename(name);
        parserPtr->setFilename(name);
        selector->push(this);
    }
 
    GDLParser& Parser()
    {
        return *parserPtr;
    }
    
    int LineContinuation()
    {
        int lC = lineContinuation;
        lineContinuation = 0;
        return lC;
    }

  void uponEOF() /*throws TokenStreamException, CharStreamException*/ 
  {
  if ( selector->getCurrentStream() != mainLexerPtr ) {
  //if( this != mainLexerPtr ) {
	  
      // make copy as we delete 'this'
      antlr::TokenStreamSelector* sel=selector; 

      // here 'this' is deleted (pops selector)
      delete sel->getCurrentStream();

      // make sure errors are reported in right file
      parserPtr->setFilename(
		static_cast<GDLLexer*>(selector->getCurrentStream())->getFilename());
            
	  // don't allow EOF until main lexer.  Force the
	  // selector to retry for another token.
	  sel->retry();
	}	
  }
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return false;
	}
public:
	GDLLexer(std::istream& in);
	GDLLexer(antlr::InputBuffer& ib);
	GDLLexer(const antlr::LexerSharedInputState& state);
	antlr::RefToken nextToken();
	protected: void mSTRING(bool _createToken);
	public: void mINCLUDE(bool _createToken);
	public: void mAND_OP_EQ(bool _createToken);
	public: void mASTERIX_EQ(bool _createToken);
	public: void mEQ_OP_EQ(bool _createToken);
	public: void mGE_OP_EQ(bool _createToken);
	public: void mGTMARK_EQ(bool _createToken);
	public: void mGT_OP_EQ(bool _createToken);
	public: void mLE_OP_EQ(bool _createToken);
	public: void mLTMARK_EQ(bool _createToken);
	public: void mLT_OP_EQ(bool _createToken);
	public: void mMATRIX_OP1_EQ(bool _createToken);
	public: void mMATRIX_OP2_EQ(bool _createToken);
	public: void mMINUS_EQ(bool _createToken);
	public: void mMOD_OP_EQ(bool _createToken);
	public: void mNE_OP_EQ(bool _createToken);
	public: void mOR_OP_EQ(bool _createToken);
	public: void mPLUS_EQ(bool _createToken);
	public: void mPOW_EQ(bool _createToken);
	public: void mSLASH_EQ(bool _createToken);
	public: void mXOR_OP_EQ(bool _createToken);
	public: void mMATRIX_OP1(bool _createToken);
	public: void mMATRIX_OP2(bool _createToken);
	public: void mMETHOD(bool _createToken);
	public: void mMEMBER(bool _createToken);
	public: void mCOMMA(bool _createToken);
	public: void mCOLON(bool _createToken);
	public: void mEQUAL(bool _createToken);
	public: void mLCURLY(bool _createToken);
	public: void mRCURLY(bool _createToken);
	public: void mLSQUARE(bool _createToken);
	public: void mRSQUARE(bool _createToken);
	public: void mLBRACE(bool _createToken);
	public: void mRBRACE(bool _createToken);
	public: void mQUESTION(bool _createToken);
	public: void mEXCLAMATION(bool _createToken);
	public: void mPOW(bool _createToken);
	public: void mASTERIX(bool _createToken);
	public: void mSLASH(bool _createToken);
	public: void mMINUS(bool _createToken);
	public: void mPLUS(bool _createToken);
	public: void mINC(bool _createToken);
	public: void mDEC(bool _createToken);
	public: void mGTMARK(bool _createToken);
	public: void mLTMARK(bool _createToken);
	public: void mLOG_AND(bool _createToken);
	public: void mLOG_OR(bool _createToken);
	public: void mLOG_NEG(bool _createToken);
	protected: void mEND_U(bool _createToken);
	protected: void mEOL(bool _createToken);
	protected: void mW(bool _createToken);
	protected: void mD(bool _createToken);
	protected: void mL(bool _createToken);
	protected: void mH(bool _createToken);
	protected: void mO(bool _createToken);
	protected: void mB(bool _createToken);
	protected: void mEXP(bool _createToken);
	protected: void mDBL_E(bool _createToken);
	protected: void mDBL(bool _createToken);
	protected: void mCONSTANT_HEX_BYTE(bool _createToken);
	protected: void mCONSTANT_HEX_LONG(bool _createToken);
	protected: void mCONSTANT_HEX_LONG64(bool _createToken);
	protected: void mCONSTANT_HEX_I(bool _createToken);
	protected: void mCONSTANT_HEX_INT(bool _createToken);
	protected: void mCONSTANT_HEX_ULONG(bool _createToken);
	protected: void mCONSTANT_HEX_ULONG64(bool _createToken);
	protected: void mCONSTANT_HEX_UI(bool _createToken);
	protected: void mCONSTANT_HEX_UINT(bool _createToken);
	protected: void mCONSTANT_BYTE(bool _createToken);
	protected: void mCONSTANT_LONG(bool _createToken);
	protected: void mCONSTANT_LONG64(bool _createToken);
	protected: void mCONSTANT_I(bool _createToken);
	protected: void mCONSTANT_INT(bool _createToken);
	protected: void mCONSTANT_ULONG(bool _createToken);
	protected: void mCONSTANT_ULONG64(bool _createToken);
	protected: void mCONSTANT_UI(bool _createToken);
	protected: void mCONSTANT_UINT(bool _createToken);
	protected: void mCONSTANT_OCT_BYTE(bool _createToken);
	protected: void mCONSTANT_OCT_LONG(bool _createToken);
	protected: void mCONSTANT_OCT_LONG64(bool _createToken);
	protected: void mCONSTANT_OCT_I(bool _createToken);
	protected: void mCONSTANT_OCT_INT(bool _createToken);
	protected: void mCONSTANT_OCT_ULONG(bool _createToken);
	protected: void mCONSTANT_OCT_ULONG64(bool _createToken);
	protected: void mCONSTANT_OCT_UI(bool _createToken);
	protected: void mCONSTANT_OCT_UINT(bool _createToken);
	protected: void mCONSTANT_FLOAT(bool _createToken);
	protected: void mCONSTANT_DOUBLE(bool _createToken);
	protected: void mSTRING_LITERAL(bool _createToken);
	protected: void mDOT(bool _createToken);
	public: void mCONSTANT_OR_STRING_LITERAL(bool _createToken);
	public: void mCOMMENT(bool _createToken);
	public: void mIDENTIFIER(bool _createToken);
	public: void mSYSVARNAME(bool _createToken);
	public: void mEND_MARKER(bool _createToken);
	public: void mWHITESPACE(bool _createToken);
	protected: void mSKIP_LINES(bool _createToken);
	public: void mCONT_STATEMENT(bool _createToken);
	public: void mEND_OF_LINE(bool _createToken);
	protected: void mMAX_TOKEN_NUMBER(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const antlr::BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const antlr::BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const antlr::BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const antlr::BitSet _tokenSet_4;
	static const unsigned long _tokenSet_5_data_[];
	static const antlr::BitSet _tokenSet_5;
	static const unsigned long _tokenSet_6_data_[];
	static const antlr::BitSet _tokenSet_6;
	static const unsigned long _tokenSet_7_data_[];
	static const antlr::BitSet _tokenSet_7;
	static const unsigned long _tokenSet_8_data_[];
	static const antlr::BitSet _tokenSet_8;
	static const unsigned long _tokenSet_9_data_[];
	static const antlr::BitSet _tokenSet_9;
	static const unsigned long _tokenSet_10_data_[];
	static const antlr::BitSet _tokenSet_10;
	static const unsigned long _tokenSet_11_data_[];
	static const antlr::BitSet _tokenSet_11;
};

#endif /*INC_GDLLexer_hpp_*/
