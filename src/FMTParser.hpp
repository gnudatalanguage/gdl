#ifndef INC_FMTParser_hpp_
#define INC_FMTParser_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (2006-11-01): "format.g" -> "FMTParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "FMTTokenTypes.hpp"
#include <antlr/LLkParser.hpp>



#include <fstream>
#include <sstream>

#include "fmtnode.hpp"

#include "CFMTLexer.hpp"

#include <antlr/TokenStreamSelector.hpp>

//using namespace antlr;

class CUSTOM_API FMTParser : public antlr::LLkParser, public FMTTokenTypes
{

// class extensions
public:
	void initializeASTFactory( antlr::ASTFactory& factory );
protected:
	FMTParser(antlr::TokenBuffer& tokenBuf, int k);
public:
	FMTParser(antlr::TokenBuffer& tokenBuf);
protected:
	FMTParser(antlr::TokenStream& lexer, int k);
public:
	FMTParser(antlr::TokenStream& lexer);
	FMTParser(const antlr::ParserSharedInputState& state);
	int getNumTokens() const
	{
		return FMTParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return FMTParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return FMTParser::tokenNames;
	}
	public: void format(
		 int repeat
	);
	public: void qfq();
	public: void q();
	public: void f();
	public: void f_csubcode();
	public: void cstring();
	public:  int  nn();
	public: void cformat();
	public: void cnnf(
		int *infos
	);
	public:  int  cnn();
	public: void rep_fmt(
		 int repeat
	);
	public: int  nnf(
		 RefFMTNode fNode
	);
	public: void w_d(
		 RefFMTNode fNode
	);
	public: void w_d_e(
		 RefFMTNode fNode
	);
	public: void calendar_string();
	public: void calendar_code();
public:
	antlr::RefAST getAST()
	{
		return antlr::RefAST(returnAST);
	}
	
protected:
	RefFMTNode returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 86;
#else
	enum {
		NUM_TOKENS = 86
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const antlr::BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const antlr::BitSet _tokenSet_2;
};

#endif /*INC_FMTParser_hpp_*/
