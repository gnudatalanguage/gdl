/* $ANTLR 2.7.7 (20130428): "gdlc.g" -> "GDLLexer.cpp"$ */

#include "includefirst.hpp"

#include "GDLLexer.hpp"
#include <antlr/CharBuffer.hpp>
#include <antlr/TokenStreamException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>
#include <antlr/CharStreamException.hpp>
#include <antlr/CharStreamIOException.hpp>
#include <antlr/NoViableAltForCharException.hpp>


#include <errno.h>

#include <cstdlib>

GDLLexer::GDLLexer(std::istream& in)
	: antlr::CharScanner(new antlr::CharBuffer(in),false)
{
	initLiterals();
}

GDLLexer::GDLLexer(antlr::InputBuffer& ib)
	: antlr::CharScanner(ib,false)
{
	initLiterals();
}

GDLLexer::GDLLexer(const antlr::LexerSharedInputState& state)
	: antlr::CharScanner(state,false)
{
	initLiterals();
}

void GDLLexer::initLiterals()
{
	literals["endcase"] = 100;
	literals["case"] = 94;
	literals["while"] = 90;
	literals["repeat"] = 80;
	literals["ne"] = 119;
	literals["endif"] = 104;
	literals["end"] = 99;
	literals["le"] = 116;
	literals["then"] = 126;
	literals["endswitch"] = 106;
	literals["until"] = 127;
	literals["and"] = 92;
	literals["endrep"] = 105;
	literals["not"] = 120;
	literals["foreach"] = 32;
	literals["mod"] = 118;
	literals["forward_function"] = 109;
	literals["do"] = 97;
	literals["function"] = 110;
	literals["endfor"] = 102;
	literals["gt"] = 113;
	literals["compile_opt"] = 96;
	literals["inherits"] = 115;
	literals["of"] = 121;
	literals["or"] = 123;
	literals["if"] = 114;
	literals["pro"] = 124;
	literals["xor"] = 128;
	literals["ge"] = 111;
	literals["goto"] = 112;
	literals["for"] = 30;
	literals["eq"] = 108;
	literals["on_ioerror"] = 122;
	literals["endelse"] = 101;
	literals["begin"] = 93;
	literals["else"] = 98;
	literals["lt"] = 117;
	literals["endwhile"] = 107;
	literals["switch"] = 125;
	literals["common"] = 95;
	literals["endforeach"] = 103;
}

antlr::RefToken GDLLexer::nextToken()
{
	antlr::RefToken theRetToken;
	for (;;) {
		antlr::RefToken theRetToken;
		int _ttype = antlr::Token::INVALID_TYPE;
		resetText();
		try {   // for lexical and char stream error handling
			switch ( LA(1)) {
			case 0x40 /* '@' */ :
			{
				mINCLUDE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2c /* ',' */ :
			{
				mCOMMA(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3d /* '=' */ :
			{
				mEQUAL(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x7b /* '{' */ :
			{
				mLCURLY(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x7d /* '}' */ :
			{
				mRCURLY(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5b /* '[' */ :
			{
				mLSQUARE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5d /* ']' */ :
			{
				mRSQUARE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x28 /* '(' */ :
			{
				mLBRACE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x29 /* ')' */ :
			{
				mRBRACE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3f /* '?' */ :
			{
				mQUESTION(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x7c /* '|' */ :
			{
				mLOG_OR(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x7e /* '~' */ :
			{
				mLOG_NEG(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x22 /* '\"' */ :
			case 0x27 /* '\'' */ :
			case 0x2e /* '.' */ :
			case 0x30 /* '0' */ :
			case 0x31 /* '1' */ :
			case 0x32 /* '2' */ :
			case 0x33 /* '3' */ :
			case 0x34 /* '4' */ :
			case 0x35 /* '5' */ :
			case 0x36 /* '6' */ :
			case 0x37 /* '7' */ :
			case 0x38 /* '8' */ :
			case 0x39 /* '9' */ :
			{
				mCONSTANT_OR_STRING_LITERAL(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3b /* ';' */ :
			{
				mCOMMENT(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x9 /* '\t' */ :
			case 0xc /* '\14' */ :
			case 0x20 /* ' ' */ :
			{
				mWHITESPACE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x24 /* '$' */ :
			{
				mCONT_STATEMENT(true);
				theRetToken=_returnToken;
				break;
			}
			case 0xa /* '\n' */ :
			case 0xd /* '\r' */ :
			{
				mEND_OF_LINE(true);
				theRetToken=_returnToken;
				break;
			}
			default:
				if (((LA(1) == 0x61 /* 'a' */ ) && (LA(2) == 0x6e /* 'n' */ ) && (LA(3) == 0x64 /* 'd' */ ))&&( LA(4) == '=')) {
					mAND_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x65 /* 'e' */ ) && (LA(2) == 0x71 /* 'q' */ ) && (LA(3) == 0x3d /* '=' */ )) {
					mEQ_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x67 /* 'g' */ ) && (LA(2) == 0x65 /* 'e' */ ) && (LA(3) == 0x3d /* '=' */ )) {
					mGE_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x67 /* 'g' */ ) && (LA(2) == 0x74 /* 't' */ ) && (LA(3) == 0x3d /* '=' */ )) {
					mGT_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x6c /* 'l' */ ) && (LA(2) == 0x65 /* 'e' */ ) && (LA(3) == 0x3d /* '=' */ )) {
					mLE_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x6c /* 'l' */ ) && (LA(2) == 0x74 /* 't' */ ) && (LA(3) == 0x3d /* '=' */ )) {
					mLT_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x23 /* '#' */ ) && (LA(2) == 0x23 /* '#' */ ) && (LA(3) == 0x3d /* '=' */ )) {
					mMATRIX_OP2_EQ(true);
					theRetToken=_returnToken;
				}
				else if (((LA(1) == 0x6d /* 'm' */ ) && (LA(2) == 0x6f /* 'o' */ ) && (LA(3) == 0x64 /* 'd' */ ))&&( LA(4) == '=')) {
					mMOD_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x6e /* 'n' */ ) && (LA(2) == 0x65 /* 'e' */ ) && (LA(3) == 0x3d /* '=' */ )) {
					mNE_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x6f /* 'o' */ ) && (LA(2) == 0x72 /* 'r' */ ) && (LA(3) == 0x3d /* '=' */ )) {
					mOR_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if (((LA(1) == 0x78 /* 'x' */ ) && (LA(2) == 0x6f /* 'o' */ ) && (LA(3) == 0x72 /* 'r' */ ))&&( LA(4) == '=')) {
					mXOR_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2a /* '*' */ ) && (LA(2) == 0x3d /* '=' */ )) {
					mASTERIX_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x3e /* '>' */ ) && (LA(2) == 0x3d /* '=' */ )) {
					mGTMARK_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x3c /* '<' */ ) && (LA(2) == 0x3d /* '=' */ )) {
					mLTMARK_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x23 /* '#' */ ) && (LA(2) == 0x3d /* '=' */ )) {
					mMATRIX_OP1_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2d /* '-' */ ) && (LA(2) == 0x3d /* '=' */ )) {
					mMINUS_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2b /* '+' */ ) && (LA(2) == 0x3d /* '=' */ )) {
					mPLUS_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x5e /* '^' */ ) && (LA(2) == 0x3d /* '=' */ )) {
					mPOW_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2f /* '/' */ ) && (LA(2) == 0x3d /* '=' */ )) {
					mSLASH_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x23 /* '#' */ ) && (LA(2) == 0x23 /* '#' */ ) && (true)) {
					mMATRIX_OP2(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x3a /* ':' */ ) && (LA(2) == 0x3a /* ':' */ )) {
					mMETHOD(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2d /* '-' */ ) && (LA(2) == 0x3e /* '>' */ )) {
					mMEMBER(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2b /* '+' */ ) && (LA(2) == 0x2b /* '+' */ )) {
					mINC(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2d /* '-' */ ) && (LA(2) == 0x2d /* '-' */ )) {
					mDEC(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x26 /* '&' */ ) && (LA(2) == 0x26 /* '&' */ )) {
					mLOG_AND(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x21 /* '!' */ ) && (_tokenSet_0.member(LA(2)))) {
					mSYSVARNAME(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x23 /* '#' */ ) && (true)) {
					mMATRIX_OP1(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x3a /* ':' */ ) && (true)) {
					mCOLON(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x21 /* '!' */ ) && (true)) {
					mEXCLAMATION(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x5e /* '^' */ ) && (true)) {
					mPOW(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2a /* '*' */ ) && (true)) {
					mASTERIX(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2f /* '/' */ ) && (true)) {
					mSLASH(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2d /* '-' */ ) && (true)) {
					mMINUS(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2b /* '+' */ ) && (true)) {
					mPLUS(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x3e /* '>' */ ) && (true)) {
					mGTMARK(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x3c /* '<' */ ) && (true)) {
					mLTMARK(true);
					theRetToken=_returnToken;
				}
				else if ((_tokenSet_1.member(LA(1))) && (true) && (true)) {
					mIDENTIFIER(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x26 /* '&' */ ) && (true)) {
					mEND_MARKER(true);
					theRetToken=_returnToken;
				}
			else {
				if (LA(1)==EOF_CHAR)
				{
					uponEOF();
					_returnToken = makeToken(antlr::Token::EOF_TYPE);
				}
				else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			}
			if ( !_returnToken )
				goto tryAgain; // found SKIP token

			_ttype = _returnToken->getType();
			_returnToken->setType(_ttype);
			return _returnToken;
		}
		catch (antlr::RecognitionException& e) {
				throw antlr::TokenStreamRecognitionException(e);
		}
		catch (antlr::CharStreamIOException& csie) {
			throw antlr::TokenStreamIOException(csie.io);
		}
		catch (antlr::CharStreamException& cse) {
			throw antlr::TokenStreamException(cse.getMessage());
		}
tryAgain:;
	}
}

void GDLLexer::mSTRING(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = STRING;
	std::string::size_type _saveIndex;
	
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_2.member(LA(1)))) {
			{
			match(_tokenSet_2);
			}
		}
		else {
			goto _loop267;
		}
		
	}
	_loop267:;
	} // ( ... )*
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mINCLUDE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = INCLUDE;
	std::string::size_type _saveIndex;
	antlr::RefToken f;
	
	_saveIndex = text.length();
	match('@' /* charlit */ );
	text.erase(_saveIndex);
	_saveIndex = text.length();
	mSTRING(true);
	text.erase(_saveIndex);
	f=_returnToken;
	if ( inputState->guessing==0 ) {
		
				ANTLR_USING_NAMESPACE(std)
				// create lexer to handle include
				std::string name = f->getText();
		
		// find comments on the same line
		long pos = name.find_first_of(';', 0);   
		if( pos != std::string::npos) // remove them  
		name = name.substr(0, pos);
		
			  	StrTrim(name);
		
			  	std::string appName=name;
			  	AppendIfNeeded(appName,".pro");
		
		errno = 0; // zero it to detect errors
		
		bool found = CompleteFileName( appName);
		if( found) 
		name = appName;
		else
		found = CompleteFileName( name);
		
		if( !found)
		{
		if( errno == EMFILE)
		throw GDLException( "Too many open files "
		"(recursive use of '@'?): " + name);
		else 
		throw GDLException( "File not found: " + name);
		}
		
		std::ifstream* input = new std::ifstream(name.c_str());
				if (!*input) 
					{
				  	delete input;
		throw GDLException( "Error opening file. File: " + name);
				  	cerr << SysVar::MsgPrefix() << "Error opening file. File: " << name << endl;
					}
		
			  	if( *input) 
			  		{
					new GDLLexer(*input,name,this);
					selector->retry(); // throws TokenStreamRetryException
					}
				
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mAND_OP_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = AND_OP_EQ;
	std::string::size_type _saveIndex;
	
	if (!( LA(4) == '='))
		throw antlr::SemanticException(" LA(4) == \'=\'");
	match("and=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mASTERIX_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = ASTERIX_EQ;
	std::string::size_type _saveIndex;
	
	match("*=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mEQ_OP_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = EQ_OP_EQ;
	std::string::size_type _saveIndex;
	
	match("eq=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mGE_OP_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = GE_OP_EQ;
	std::string::size_type _saveIndex;
	
	match("ge=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mGTMARK_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = GTMARK_EQ;
	std::string::size_type _saveIndex;
	
	match(">=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mGT_OP_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = GT_OP_EQ;
	std::string::size_type _saveIndex;
	
	match("gt=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mLE_OP_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LE_OP_EQ;
	std::string::size_type _saveIndex;
	
	match("le=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mLTMARK_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LTMARK_EQ;
	std::string::size_type _saveIndex;
	
	match("<=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mLT_OP_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LT_OP_EQ;
	std::string::size_type _saveIndex;
	
	match("lt=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mMATRIX_OP1_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = MATRIX_OP1_EQ;
	std::string::size_type _saveIndex;
	
	match("#=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mMATRIX_OP2_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = MATRIX_OP2_EQ;
	std::string::size_type _saveIndex;
	
	match("##=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mMINUS_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = MINUS_EQ;
	std::string::size_type _saveIndex;
	
	match("-=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mMOD_OP_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = MOD_OP_EQ;
	std::string::size_type _saveIndex;
	
	if (!( LA(4) == '='))
		throw antlr::SemanticException(" LA(4) == \'=\'");
	match("mod=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mNE_OP_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = NE_OP_EQ;
	std::string::size_type _saveIndex;
	
	match("ne=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mOR_OP_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = OR_OP_EQ;
	std::string::size_type _saveIndex;
	
	match("or=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mPLUS_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = PLUS_EQ;
	std::string::size_type _saveIndex;
	
	match("+=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mPOW_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = POW_EQ;
	std::string::size_type _saveIndex;
	
	match("^=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mSLASH_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = SLASH_EQ;
	std::string::size_type _saveIndex;
	
	match("/=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mXOR_OP_EQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = XOR_OP_EQ;
	std::string::size_type _saveIndex;
	
	if (!( LA(4) == '='))
		throw antlr::SemanticException(" LA(4) == \'=\'");
	match("xor=");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mMATRIX_OP1(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = MATRIX_OP1;
	std::string::size_type _saveIndex;
	
	match('#' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mMATRIX_OP2(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = MATRIX_OP2;
	std::string::size_type _saveIndex;
	
	match("##");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mMETHOD(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = METHOD;
	std::string::size_type _saveIndex;
	
	match("::");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mMEMBER(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = MEMBER;
	std::string::size_type _saveIndex;
	
	match("->");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCOMMA(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = COMMA;
	std::string::size_type _saveIndex;
	
	match(',' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCOLON(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = COLON;
	std::string::size_type _saveIndex;
	
	match(':' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mEQUAL(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = EQUAL;
	std::string::size_type _saveIndex;
	
	match('=' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mLCURLY(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LCURLY;
	std::string::size_type _saveIndex;
	
	match('{' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mRCURLY(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = RCURLY;
	std::string::size_type _saveIndex;
	
	match('}' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mLSQUARE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LSQUARE;
	std::string::size_type _saveIndex;
	
	match('[' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mRSQUARE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = RSQUARE;
	std::string::size_type _saveIndex;
	
	match(']' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mLBRACE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LBRACE;
	std::string::size_type _saveIndex;
	
	match('(' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mRBRACE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = RBRACE;
	std::string::size_type _saveIndex;
	
	match(')' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mQUESTION(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = QUESTION;
	std::string::size_type _saveIndex;
	
	match('?' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mEXCLAMATION(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = EXCLAMATION;
	std::string::size_type _saveIndex;
	
	match('!' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mPOW(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = POW;
	std::string::size_type _saveIndex;
	
	match('^' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mASTERIX(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = ASTERIX;
	std::string::size_type _saveIndex;
	
	match('*' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mSLASH(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = SLASH;
	std::string::size_type _saveIndex;
	
	match('/' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mMINUS(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = MINUS;
	std::string::size_type _saveIndex;
	
	match('-' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mPLUS(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = PLUS;
	std::string::size_type _saveIndex;
	
	match('+' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mINC(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = INC;
	std::string::size_type _saveIndex;
	
	match("++");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mDEC(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = DEC;
	std::string::size_type _saveIndex;
	
	match("--");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mGTMARK(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = GTMARK;
	std::string::size_type _saveIndex;
	
	match('>' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mLTMARK(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LTMARK;
	std::string::size_type _saveIndex;
	
	match('<' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mLOG_AND(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LOG_AND;
	std::string::size_type _saveIndex;
	
	match("&&");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mLOG_OR(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LOG_OR;
	std::string::size_type _saveIndex;
	
	match("||");
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mLOG_NEG(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = LOG_NEG;
	std::string::size_type _saveIndex;
	
	match('~' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mEND_U(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = END_U;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mEOL(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = EOL;
	std::string::size_type _saveIndex;
	
	{
	bool synPredMatched319 = false;
	if (((LA(1) == 0xd /* '\r' */ ) && (LA(2) == 0xa /* '\n' */ ) && (true))) {
		int _m319 = mark();
		synPredMatched319 = true;
		inputState->guessing++;
		try {
			{
			match("\r\n");
			}
		}
		catch (antlr::RecognitionException& pe) {
			synPredMatched319 = false;
		}
		rewind(_m319);
		inputState->guessing--;
	}
	if ( synPredMatched319 ) {
		match("\r\n");
	}
	else if ((LA(1) == 0xa /* '\n' */ )) {
		match('\n' /* charlit */ );
	}
	else if ((LA(1) == 0xd /* '\r' */ ) && (true) && (true)) {
		match('\r' /* charlit */ );
	}
	else {
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	
	}
	if ( inputState->guessing==0 ) {
		newline();
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mW(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = W;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x20 /* ' ' */ :
	{
		match(' ' /* charlit */ );
		break;
	}
	case 0x9 /* '\t' */ :
	{
		match('\t' /* charlit */ );
		break;
	}
	case 0xc /* '\14' */ :
	{
		match('\14' /* charlit */ );
		break;
	}
	default:
	{
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mD(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = D;
	std::string::size_type _saveIndex;
	
	{
	matchRange('0','9');
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mL(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = L;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x61 /* 'a' */ :
	case 0x62 /* 'b' */ :
	case 0x63 /* 'c' */ :
	case 0x64 /* 'd' */ :
	case 0x65 /* 'e' */ :
	case 0x66 /* 'f' */ :
	case 0x67 /* 'g' */ :
	case 0x68 /* 'h' */ :
	case 0x69 /* 'i' */ :
	case 0x6a /* 'j' */ :
	case 0x6b /* 'k' */ :
	case 0x6c /* 'l' */ :
	case 0x6d /* 'm' */ :
	case 0x6e /* 'n' */ :
	case 0x6f /* 'o' */ :
	case 0x70 /* 'p' */ :
	case 0x71 /* 'q' */ :
	case 0x72 /* 'r' */ :
	case 0x73 /* 's' */ :
	case 0x74 /* 't' */ :
	case 0x75 /* 'u' */ :
	case 0x76 /* 'v' */ :
	case 0x77 /* 'w' */ :
	case 0x78 /* 'x' */ :
	case 0x79 /* 'y' */ :
	case 0x7a /* 'z' */ :
	{
		matchRange('a','z');
		break;
	}
	case 0x5f /* '_' */ :
	{
		match('_' /* charlit */ );
		break;
	}
	default:
	{
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mH(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = H;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x61 /* 'a' */ :
	case 0x62 /* 'b' */ :
	case 0x63 /* 'c' */ :
	case 0x64 /* 'd' */ :
	case 0x65 /* 'e' */ :
	case 0x66 /* 'f' */ :
	{
		matchRange('a','f');
		break;
	}
	case 0x30 /* '0' */ :
	case 0x31 /* '1' */ :
	case 0x32 /* '2' */ :
	case 0x33 /* '3' */ :
	case 0x34 /* '4' */ :
	case 0x35 /* '5' */ :
	case 0x36 /* '6' */ :
	case 0x37 /* '7' */ :
	case 0x38 /* '8' */ :
	case 0x39 /* '9' */ :
	{
		matchRange('0','9');
		break;
	}
	default:
	{
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mO(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = O;
	std::string::size_type _saveIndex;
	
	{
	matchRange('0','7');
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mB(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = B;
	std::string::size_type _saveIndex;
	
	{
	matchRange('0','1');
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mEXP(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = EXP;
	std::string::size_type _saveIndex;
	
	{
	match('e' /* charlit */ );
	{
	if ((_tokenSet_3.member(LA(1)))) {
		{
		switch ( LA(1)) {
		case 0x2b /* '+' */ :
		{
			match('+' /* charlit */ );
			break;
		}
		case 0x2d /* '-' */ :
		{
			match('-' /* charlit */ );
			break;
		}
		case 0x30 /* '0' */ :
		case 0x31 /* '1' */ :
		case 0x32 /* '2' */ :
		case 0x33 /* '3' */ :
		case 0x34 /* '4' */ :
		case 0x35 /* '5' */ :
		case 0x36 /* '6' */ :
		case 0x37 /* '7' */ :
		case 0x38 /* '8' */ :
		case 0x39 /* '9' */ :
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
		}
		{ // ( ... )+
		int _cnt337=0;
		for (;;) {
			if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
				mD(false);
			}
			else {
				if ( _cnt337>=1 ) { goto _loop337; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			
			_cnt337++;
		}
		_loop337:;
		}  // ( ... )+
	}
	else {
	}
	
	}
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mDBL_E(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = DBL_E;
	std::string::size_type _saveIndex;
	
	match('d' /* charlit */ );
	if ( inputState->guessing==0 ) {
		{ text.erase(_begin); text +=  "E"; };
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mDBL(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = DBL;
	std::string::size_type _saveIndex;
	
	{
	mDBL_E(false);
	{
	if ((_tokenSet_3.member(LA(1)))) {
		{
		switch ( LA(1)) {
		case 0x2b /* '+' */ :
		{
			match('+' /* charlit */ );
			break;
		}
		case 0x2d /* '-' */ :
		{
			match('-' /* charlit */ );
			break;
		}
		case 0x30 /* '0' */ :
		case 0x31 /* '1' */ :
		case 0x32 /* '2' */ :
		case 0x33 /* '3' */ :
		case 0x34 /* '4' */ :
		case 0x35 /* '5' */ :
		case 0x36 /* '6' */ :
		case 0x37 /* '7' */ :
		case 0x38 /* '8' */ :
		case 0x39 /* '9' */ :
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
		}
		{ // ( ... )+
		int _cnt344=0;
		for (;;) {
			if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
				mD(false);
			}
			else {
				if ( _cnt344>=1 ) { goto _loop344; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			
			_cnt344++;
		}
		_loop344:;
		}  // ( ... )+
	}
	else {
	}
	
	}
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_HEX_BYTE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_HEX_BYTE;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_HEX_LONG(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_HEX_LONG;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_HEX_LONG64(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_HEX_LONG64;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_HEX_I(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_HEX_I;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_HEX_INT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_HEX_INT;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_HEX_ULONG(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_HEX_ULONG;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_HEX_ULONG64(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_HEX_ULONG64;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_HEX_UI(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_HEX_UI;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_HEX_UINT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_HEX_UINT;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_BYTE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_BYTE;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_LONG(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_LONG;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_LONG64(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_LONG64;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_I(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_I;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_INT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_INT;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_ULONG(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_ULONG;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_ULONG64(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_ULONG64;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_UI(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_UI;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_UINT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_UINT;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_OCT_BYTE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_OCT_BYTE;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_OCT_LONG(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_OCT_LONG;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_OCT_LONG64(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_OCT_LONG64;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_OCT_I(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_OCT_I;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_OCT_INT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_OCT_INT;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_OCT_ULONG(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_OCT_ULONG;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_OCT_ULONG64(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_OCT_ULONG64;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_OCT_UI(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_OCT_UI;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_OCT_UINT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_OCT_UINT;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_FLOAT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_FLOAT;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_DOUBLE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_DOUBLE;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mSTRING_LITERAL(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = STRING_LITERAL;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mDOT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = DOT;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_OR_STRING_LITERAL(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_OR_STRING_LITERAL;
	std::string::size_type _saveIndex;
	
	bool synPredMatched390 = false;
	if (((LA(1) == 0x27 /* '\'' */ ) && (_tokenSet_4.member(LA(2))) && (_tokenSet_5.member(LA(3))))) {
		int _m390 = mark();
		synPredMatched390 = true;
		inputState->guessing++;
		try {
			{
			match('\'' /* charlit */ );
			{ // ( ... )+
			int _cnt388=0;
			for (;;) {
				if ((_tokenSet_4.member(LA(1)))) {
					mH(false);
				}
				else {
					if ( _cnt388>=1 ) { goto _loop388; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				
				_cnt388++;
			}
			_loop388:;
			}  // ( ... )+
			match('\'' /* charlit */ );
			{
			if ((LA(1) == 0x78 /* 'x' */ ) && (LA(2) == 0x75 /* 'u' */ ) && (LA(3) == 0x73 /* 's' */ )) {
				match("xus");
			}
			else if ((LA(1) == 0x78 /* 'x' */ ) && (LA(2) == 0x75 /* 'u' */ ) && (LA(3) == 0x62 /* 'b' */ )) {
				match("xub");
			}
			else if ((LA(1) == 0x78 /* 'x' */ ) && (LA(2) == 0x75 /* 'u' */ ) && (LA(3) == 0x6c /* 'l' */ )) {
				match("xul");
			}
			else if ((LA(1) == 0x78 /* 'x' */ ) && (LA(2) == 0x73 /* 's' */ )) {
				match("xs");
			}
			else if ((LA(1) == 0x78 /* 'x' */ ) && (LA(2) == 0x62 /* 'b' */ )) {
				match("xb");
			}
			else if ((LA(1) == 0x78 /* 'x' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
				match("xl");
			}
			else if ((LA(1) == 0x78 /* 'x' */ ) && (LA(2) == 0x75 /* 'u' */ ) && (true)) {
				match("xu");
			}
			else if ((LA(1) == 0x78 /* 'x' */ ) && (true)) {
				match('x' /* charlit */ );
			}
			else {
				throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
			}
			
			}
			}
		}
		catch (antlr::RecognitionException& pe) {
			synPredMatched390 = false;
		}
		rewind(_m390);
		inputState->guessing--;
	}
	if ( synPredMatched390 ) {
		{
		_saveIndex = text.length();
		match('\'' /* charlit */ );
		text.erase(_saveIndex);
		{ // ( ... )+
		int _cnt393=0;
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				mH(false);
			}
			else {
				if ( _cnt393>=1 ) { goto _loop393; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			
			_cnt393++;
		}
		_loop393:;
		}  // ( ... )+
		_saveIndex = text.length();
		match('\'' /* charlit */ );
		text.erase(_saveIndex);
		_saveIndex = text.length();
		match('x' /* charlit */ );
		text.erase(_saveIndex);
		{
		switch ( LA(1)) {
		case 0x73 /* 's' */ :
		{
			_saveIndex = text.length();
			match('s' /* charlit */ );
			text.erase(_saveIndex);
			if ( inputState->guessing==0 ) {
				_ttype=CONSTANT_HEX_INT;
			}
			break;
		}
		case 0x62 /* 'b' */ :
		{
			_saveIndex = text.length();
			match('b' /* charlit */ );
			text.erase(_saveIndex);
			if ( inputState->guessing==0 ) {
				_ttype=CONSTANT_HEX_BYTE;
			}
			break;
		}
		default:
			if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (LA(3) == 0x6c /* 'l' */ )) {
				_saveIndex = text.length();
				match("ull");
				text.erase(_saveIndex);
				if ( inputState->guessing==0 ) {
					_ttype=CONSTANT_HEX_ULONG64;
				}
			}
			else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x73 /* 's' */ )) {
				_saveIndex = text.length();
				match("us");
				text.erase(_saveIndex);
				if ( inputState->guessing==0 ) {
					_ttype=CONSTANT_HEX_UINT;
				}
			}
			else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x62 /* 'b' */ )) {
				_saveIndex = text.length();
				match("ub");
				text.erase(_saveIndex);
				if ( inputState->guessing==0 ) {
					_ttype=CONSTANT_HEX_BYTE;
				}
			}
			else if ((LA(1) == 0x6c /* 'l' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
				_saveIndex = text.length();
				match("ll");
				text.erase(_saveIndex);
				if ( inputState->guessing==0 ) {
					_ttype=CONSTANT_HEX_LONG64;
				}
			}
			else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (true)) {
				_saveIndex = text.length();
				match("ul");
				text.erase(_saveIndex);
				if ( inputState->guessing==0 ) {
					_ttype=CONSTANT_HEX_ULONG;
				}
			}
			else if ((LA(1) == 0x75 /* 'u' */ ) && (true)) {
				_saveIndex = text.length();
				match('u' /* charlit */ );
				text.erase(_saveIndex);
				if ( inputState->guessing==0 ) {
					_ttype=CONSTANT_HEX_UI;
				}
			}
			else if ((LA(1) == 0x6c /* 'l' */ ) && (true)) {
				_saveIndex = text.length();
				match('l' /* charlit */ );
				text.erase(_saveIndex);
				if ( inputState->guessing==0 ) {
					_ttype=CONSTANT_HEX_LONG;
				}
			}
			else {
				if ( inputState->guessing==0 ) {
					_ttype=CONSTANT_HEX_I;
				}
			}
		}
		}
		}
	}
	else {
		bool synPredMatched399 = false;
		if (((LA(1) == 0x27 /* '\'' */ ) && ((LA(2) >= 0x30 /* '0' */  && LA(2) <= 0x37 /* '7' */ )) && (_tokenSet_6.member(LA(3))))) {
			int _m399 = mark();
			synPredMatched399 = true;
			inputState->guessing++;
			try {
				{
				match('\'' /* charlit */ );
				{ // ( ... )+
				int _cnt397=0;
				for (;;) {
					if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ ))) {
						mO(false);
					}
					else {
						if ( _cnt397>=1 ) { goto _loop397; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					
					_cnt397++;
				}
				_loop397:;
				}  // ( ... )+
				match('\'' /* charlit */ );
				{
				if ((LA(1) == 0x6f /* 'o' */ ) && (LA(2) == 0x75 /* 'u' */ ) && (LA(3) == 0x6c /* 'l' */ )) {
					match("oul");
				}
				else if ((LA(1) == 0x6f /* 'o' */ ) && (LA(2) == 0x73 /* 's' */ )) {
					match("os");
				}
				else if ((LA(1) == 0x6f /* 'o' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
					match("ol");
				}
				else if ((LA(1) == 0x6f /* 'o' */ ) && (LA(2) == 0x75 /* 'u' */ ) && (true)) {
					match("ou");
				}
				else if ((LA(1) == 0x6f /* 'o' */ ) && (true)) {
					match('o' /* charlit */ );
				}
				else {
					throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
				}
				
				}
				}
			}
			catch (antlr::RecognitionException& pe) {
				synPredMatched399 = false;
			}
			rewind(_m399);
			inputState->guessing--;
		}
		if ( synPredMatched399 ) {
			{
			_saveIndex = text.length();
			match('\'' /* charlit */ );
			text.erase(_saveIndex);
			{ // ( ... )+
			int _cnt402=0;
			for (;;) {
				if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ ))) {
					mO(false);
				}
				else {
					if ( _cnt402>=1 ) { goto _loop402; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				
				_cnt402++;
			}
			_loop402:;
			}  // ( ... )+
			_saveIndex = text.length();
			match('\'' /* charlit */ );
			text.erase(_saveIndex);
			_saveIndex = text.length();
			match('o' /* charlit */ );
			text.erase(_saveIndex);
			{
			switch ( LA(1)) {
			case 0x73 /* 's' */ :
			{
				_saveIndex = text.length();
				match('s' /* charlit */ );
				text.erase(_saveIndex);
				if ( inputState->guessing==0 ) {
					_ttype=CONSTANT_OCT_INT;
				}
				break;
			}
			case 0x62 /* 'b' */ :
			{
				_saveIndex = text.length();
				match('b' /* charlit */ );
				text.erase(_saveIndex);
				if ( inputState->guessing==0 ) {
					_ttype=CONSTANT_OCT_BYTE;
				}
				break;
			}
			default:
				if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (LA(3) == 0x6c /* 'l' */ )) {
					_saveIndex = text.length();
					match("ull");
					text.erase(_saveIndex);
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_OCT_ULONG64;
					}
				}
				else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x73 /* 's' */ )) {
					_saveIndex = text.length();
					match("us");
					text.erase(_saveIndex);
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_OCT_UINT;
					}
				}
				else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x62 /* 'b' */ )) {
					_saveIndex = text.length();
					match("ub");
					text.erase(_saveIndex);
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_OCT_BYTE;
					}
				}
				else if ((LA(1) == 0x6c /* 'l' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
					_saveIndex = text.length();
					match("ll");
					text.erase(_saveIndex);
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_OCT_LONG64;
					}
				}
				else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (true)) {
					_saveIndex = text.length();
					match("ul");
					text.erase(_saveIndex);
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_OCT_ULONG;
					}
				}
				else if ((LA(1) == 0x75 /* 'u' */ ) && (true)) {
					_saveIndex = text.length();
					match('u' /* charlit */ );
					text.erase(_saveIndex);
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_OCT_UI;
					}
				}
				else if ((LA(1) == 0x6c /* 'l' */ ) && (true)) {
					_saveIndex = text.length();
					match('l' /* charlit */ );
					text.erase(_saveIndex);
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_OCT_LONG;
					}
				}
				else {
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_OCT_I;
					}
				}
			}
			}
			}
		}
		else {
			bool synPredMatched408 = false;
			if (((LA(1) == 0x27 /* '\'' */ ) && (LA(2) == 0x30 /* '0' */  || LA(2) == 0x31 /* '1' */ ) && (LA(3) == 0x27 /* '\'' */  || LA(3) == 0x30 /* '0' */  || LA(3) == 0x31 /* '1' */ ))) {
				int _m408 = mark();
				synPredMatched408 = true;
				inputState->guessing++;
				try {
					{
					match('\'' /* charlit */ );
					{ // ( ... )+
					int _cnt406=0;
					for (;;) {
						if ((LA(1) == 0x30 /* '0' */  || LA(1) == 0x31 /* '1' */ )) {
							mB(false);
						}
						else {
							if ( _cnt406>=1 ) { goto _loop406; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt406++;
					}
					_loop406:;
					}  // ( ... )+
					match('\'' /* charlit */ );
					{
					if ((LA(1) == 0x62 /* 'b' */ ) && (LA(2) == 0x75 /* 'u' */ ) && (LA(3) == 0x6c /* 'l' */ )) {
						match("bul");
					}
					else if ((LA(1) == 0x62 /* 'b' */ ) && (LA(2) == 0x73 /* 's' */ )) {
						match("bs");
					}
					else if ((LA(1) == 0x62 /* 'b' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
						match("bl");
					}
					else if ((LA(1) == 0x62 /* 'b' */ ) && (LA(2) == 0x75 /* 'u' */ ) && (true)) {
						match("bu");
					}
					else if ((LA(1) == 0x62 /* 'b' */ ) && (true)) {
						match('b' /* charlit */ );
					}
					else {
						throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
					}
					
					}
					}
				}
				catch (antlr::RecognitionException& pe) {
					synPredMatched408 = false;
				}
				rewind(_m408);
				inputState->guessing--;
			}
			if ( synPredMatched408 ) {
				{
				_saveIndex = text.length();
				match('\'' /* charlit */ );
				text.erase(_saveIndex);
				{ // ( ... )+
				int _cnt411=0;
				for (;;) {
					if ((LA(1) == 0x30 /* '0' */  || LA(1) == 0x31 /* '1' */ )) {
						mB(false);
					}
					else {
						if ( _cnt411>=1 ) { goto _loop411; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					
					_cnt411++;
				}
				_loop411:;
				}  // ( ... )+
				_saveIndex = text.length();
				match('\'' /* charlit */ );
				text.erase(_saveIndex);
				_saveIndex = text.length();
				match('b' /* charlit */ );
				text.erase(_saveIndex);
				{
				switch ( LA(1)) {
				case 0x73 /* 's' */ :
				{
					_saveIndex = text.length();
					match('s' /* charlit */ );
					text.erase(_saveIndex);
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_BIN_INT;
					}
					break;
				}
				case 0x62 /* 'b' */ :
				{
					_saveIndex = text.length();
					match('b' /* charlit */ );
					text.erase(_saveIndex);
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_BIN_BYTE;
					}
					break;
				}
				default:
					if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (LA(3) == 0x6c /* 'l' */ )) {
						_saveIndex = text.length();
						match("ull");
						text.erase(_saveIndex);
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_BIN_ULONG64;
						}
					}
					else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x73 /* 's' */ )) {
						_saveIndex = text.length();
						match("us");
						text.erase(_saveIndex);
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_BIN_UINT;
						}
					}
					else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x62 /* 'b' */ )) {
						_saveIndex = text.length();
						match("ub");
						text.erase(_saveIndex);
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_BIN_BYTE;
						}
					}
					else if ((LA(1) == 0x6c /* 'l' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
						_saveIndex = text.length();
						match("ll");
						text.erase(_saveIndex);
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_BIN_LONG64;
						}
					}
					else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (true)) {
						_saveIndex = text.length();
						match("ul");
						text.erase(_saveIndex);
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_BIN_ULONG;
						}
					}
					else if ((LA(1) == 0x75 /* 'u' */ ) && (true)) {
						_saveIndex = text.length();
						match('u' /* charlit */ );
						text.erase(_saveIndex);
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_BIN_UI;
						}
					}
					else if ((LA(1) == 0x6c /* 'l' */ ) && (true)) {
						_saveIndex = text.length();
						match('l' /* charlit */ );
						text.erase(_saveIndex);
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_BIN_LONG;
						}
					}
					else {
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_BIN_I;
						}
					}
				}
				}
				}
			}
			else {
				bool synPredMatched381 = false;
				if (((LA(1) == 0x22 /* '\"' */ ) && ((LA(2) >= 0x30 /* '0' */  && LA(2) <= 0x37 /* '7' */ )) && (true))) {
					int _m381 = mark();
					synPredMatched381 = true;
					inputState->guessing++;
					try {
						{
						match('\"' /* charlit */ );
						{ // ( ... )+
						int _cnt379=0;
						for (;;) {
							if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ ))) {
								mO(false);
							}
							else {
								if ( _cnt379>=1 ) { goto _loop379; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt379++;
						}
						_loop379:;
						}  // ( ... )+
						{
						switch ( LA(1)) {
						case 0x62 /* 'b' */ :
						{
							match('b' /* charlit */ );
							break;
						}
						case 0x73 /* 's' */ :
						{
							match('s' /* charlit */ );
							break;
						}
						case 0x6c /* 'l' */ :
						{
							match('l' /* charlit */ );
							break;
						}
						default:
							if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x73 /* 's' */ )) {
								match("us");
							}
							else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x62 /* 'b' */ )) {
								match("ub");
							}
							else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
								match("ul");
							}
							else if ((LA(1) == 0x75 /* 'u' */ ) && (true)) {
								match('u' /* charlit */ );
							}
							else {
							}
						}
						}
						}
					}
					catch (antlr::RecognitionException& pe) {
						synPredMatched381 = false;
					}
					rewind(_m381);
					inputState->guessing--;
				}
				if ( synPredMatched381 ) {
					{
					_saveIndex = text.length();
					match('\"' /* charlit */ );
					text.erase(_saveIndex);
					{ // ( ... )+
					int _cnt384=0;
					for (;;) {
						if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ ))) {
							mO(false);
						}
						else {
							if ( _cnt384>=1 ) { goto _loop384; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt384++;
					}
					_loop384:;
					}  // ( ... )+
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_OCT_I;
					}
					{
					switch ( LA(1)) {
					case 0x73 /* 's' */ :
					{
						_saveIndex = text.length();
						match('s' /* charlit */ );
						text.erase(_saveIndex);
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_OCT_INT;
						}
						break;
					}
					case 0x62 /* 'b' */ :
					{
						_saveIndex = text.length();
						match('b' /* charlit */ );
						text.erase(_saveIndex);
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_OCT_BYTE;
						}
						break;
					}
					default:
						if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (LA(3) == 0x6c /* 'l' */ )) {
							_saveIndex = text.length();
							match("ull");
							text.erase(_saveIndex);
							if ( inputState->guessing==0 ) {
								_ttype=CONSTANT_OCT_ULONG64;
							}
						}
						else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x73 /* 's' */ )) {
							_saveIndex = text.length();
							match("us");
							text.erase(_saveIndex);
							if ( inputState->guessing==0 ) {
								_ttype=CONSTANT_OCT_UINT;
							}
						}
						else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x62 /* 'b' */ )) {
							_saveIndex = text.length();
							match("ub");
							text.erase(_saveIndex);
							if ( inputState->guessing==0 ) {
								_ttype=CONSTANT_OCT_BYTE;
							}
						}
						else if ((LA(1) == 0x6c /* 'l' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
							_saveIndex = text.length();
							match("ll");
							text.erase(_saveIndex);
							if ( inputState->guessing==0 ) {
								_ttype=CONSTANT_OCT_LONG64;
							}
						}
						else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (true)) {
							_saveIndex = text.length();
							match("ul");
							text.erase(_saveIndex);
							if ( inputState->guessing==0 ) {
								_ttype=CONSTANT_OCT_ULONG;
							}
						}
						else if ((LA(1) == 0x75 /* 'u' */ ) && (true)) {
							_saveIndex = text.length();
							match('u' /* charlit */ );
							text.erase(_saveIndex);
							if ( inputState->guessing==0 ) {
								_ttype=CONSTANT_OCT_UI;
							}
						}
						else if ((LA(1) == 0x6c /* 'l' */ ) && (true)) {
							_saveIndex = text.length();
							match('l' /* charlit */ );
							text.erase(_saveIndex);
							if ( inputState->guessing==0 ) {
								_ttype=CONSTANT_OCT_LONG;
							}
						}
						else {
						}
					}
					}
					}
				}
				else {
					bool synPredMatched432 = false;
					if (((_tokenSet_7.member(LA(1))) && (_tokenSet_8.member(LA(2))) && (true))) {
						int _m432 = mark();
						synPredMatched432 = true;
						inputState->guessing++;
						try {
							{
							switch ( LA(1)) {
							case 0x30 /* '0' */ :
							case 0x31 /* '1' */ :
							case 0x32 /* '2' */ :
							case 0x33 /* '3' */ :
							case 0x34 /* '4' */ :
							case 0x35 /* '5' */ :
							case 0x36 /* '6' */ :
							case 0x37 /* '7' */ :
							case 0x38 /* '8' */ :
							case 0x39 /* '9' */ :
							{
								{
								{ // ( ... )+
								int _cnt424=0;
								for (;;) {
									if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
										mD(false);
									}
									else {
										if ( _cnt424>=1 ) { goto _loop424; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
									}
									
									_cnt424++;
								}
								_loop424:;
								}  // ( ... )+
								{
								switch ( LA(1)) {
								case 0x64 /* 'd' */ :
								{
									mDBL(false);
									break;
								}
								case 0x2e /* '.' */ :
								{
									match('.' /* charlit */ );
									{ // ( ... )*
									for (;;) {
										if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
											mD(false);
										}
										else {
											goto _loop427;
										}
										
									}
									_loop427:;
									} // ( ... )*
									{
									mDBL(false);
									}
									break;
								}
								default:
								{
									throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
								}
								}
								}
								}
								break;
							}
							case 0x2e /* '.' */ :
							{
								match('.' /* charlit */ );
								{ // ( ... )+
								int _cnt430=0;
								for (;;) {
									if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
										mD(false);
									}
									else {
										if ( _cnt430>=1 ) { goto _loop430; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
									}
									
									_cnt430++;
								}
								_loop430:;
								}  // ( ... )+
								{
								mDBL(false);
								}
								break;
							}
							default:
							{
								throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
							}
							}
							}
						}
						catch (antlr::RecognitionException& pe) {
							synPredMatched432 = false;
						}
						rewind(_m432);
						inputState->guessing--;
					}
					if ( synPredMatched432 ) {
						{
						switch ( LA(1)) {
						case 0x30 /* '0' */ :
						case 0x31 /* '1' */ :
						case 0x32 /* '2' */ :
						case 0x33 /* '3' */ :
						case 0x34 /* '4' */ :
						case 0x35 /* '5' */ :
						case 0x36 /* '6' */ :
						case 0x37 /* '7' */ :
						case 0x38 /* '8' */ :
						case 0x39 /* '9' */ :
						{
							{
							{ // ( ... )+
							int _cnt436=0;
							for (;;) {
								if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
									mD(false);
								}
								else {
									if ( _cnt436>=1 ) { goto _loop436; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
								}
								
								_cnt436++;
							}
							_loop436:;
							}  // ( ... )+
							{
							switch ( LA(1)) {
							case 0x64 /* 'd' */ :
							{
								mDBL(false);
								break;
							}
							case 0x2e /* '.' */ :
							{
								match('.' /* charlit */ );
								{ // ( ... )*
								for (;;) {
									if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
										mD(false);
									}
									else {
										goto _loop439;
									}
									
								}
								_loop439:;
								} // ( ... )*
								{
								mDBL(false);
								}
								break;
							}
							default:
							{
								throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
							}
							}
							}
							}
							break;
						}
						case 0x2e /* '.' */ :
						{
							match('.' /* charlit */ );
							{ // ( ... )+
							int _cnt442=0;
							for (;;) {
								if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
									mD(false);
								}
								else {
									if ( _cnt442>=1 ) { goto _loop442; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
								}
								
								_cnt442++;
							}
							_loop442:;
							}  // ( ... )+
							{
							mDBL(false);
							}
							break;
						}
						default:
						{
							throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
						}
						}
						}
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_DOUBLE;
						}
					}
					else {
						bool synPredMatched455 = false;
						if (((_tokenSet_7.member(LA(1))) && (_tokenSet_9.member(LA(2))) && (true))) {
							int _m455 = mark();
							synPredMatched455 = true;
							inputState->guessing++;
							try {
								{
								switch ( LA(1)) {
								case 0x30 /* '0' */ :
								case 0x31 /* '1' */ :
								case 0x32 /* '2' */ :
								case 0x33 /* '3' */ :
								case 0x34 /* '4' */ :
								case 0x35 /* '5' */ :
								case 0x36 /* '6' */ :
								case 0x37 /* '7' */ :
								case 0x38 /* '8' */ :
								case 0x39 /* '9' */ :
								{
									{
									{ // ( ... )+
									int _cnt447=0;
									for (;;) {
										if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
											mD(false);
										}
										else {
											if ( _cnt447>=1 ) { goto _loop447; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
										}
										
										_cnt447++;
									}
									_loop447:;
									}  // ( ... )+
									{
									switch ( LA(1)) {
									case 0x65 /* 'e' */ :
									{
										mEXP(false);
										break;
									}
									case 0x2e /* '.' */ :
									{
										match('.' /* charlit */ );
										{ // ( ... )*
										for (;;) {
											if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
												mD(false);
											}
											else {
												goto _loop450;
											}
											
										}
										_loop450:;
										} // ( ... )*
										{
										if ((LA(1) == 0x65 /* 'e' */ )) {
											mEXP(false);
										}
										else {
										}
										
										}
										break;
									}
									default:
									{
										throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
									}
									}
									}
									}
									break;
								}
								case 0x2e /* '.' */ :
								{
									match('.' /* charlit */ );
									{ // ( ... )+
									int _cnt453=0;
									for (;;) {
										if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
											mD(false);
										}
										else {
											if ( _cnt453>=1 ) { goto _loop453; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
										}
										
										_cnt453++;
									}
									_loop453:;
									}  // ( ... )+
									{
									if ((LA(1) == 0x65 /* 'e' */ )) {
										mEXP(false);
									}
									else {
									}
									
									}
									break;
								}
								default:
								{
									throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
								}
								}
								}
							}
							catch (antlr::RecognitionException& pe) {
								synPredMatched455 = false;
							}
							rewind(_m455);
							inputState->guessing--;
						}
						if ( synPredMatched455 ) {
							{
							switch ( LA(1)) {
							case 0x30 /* '0' */ :
							case 0x31 /* '1' */ :
							case 0x32 /* '2' */ :
							case 0x33 /* '3' */ :
							case 0x34 /* '4' */ :
							case 0x35 /* '5' */ :
							case 0x36 /* '6' */ :
							case 0x37 /* '7' */ :
							case 0x38 /* '8' */ :
							case 0x39 /* '9' */ :
							{
								{
								{ // ( ... )+
								int _cnt459=0;
								for (;;) {
									if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
										mD(false);
									}
									else {
										if ( _cnt459>=1 ) { goto _loop459; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
									}
									
									_cnt459++;
								}
								_loop459:;
								}  // ( ... )+
								{
								switch ( LA(1)) {
								case 0x65 /* 'e' */ :
								{
									mEXP(false);
									break;
								}
								case 0x2e /* '.' */ :
								{
									match('.' /* charlit */ );
									{ // ( ... )*
									for (;;) {
										if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
											mD(false);
										}
										else {
											goto _loop462;
										}
										
									}
									_loop462:;
									} // ( ... )*
									{
									if ((LA(1) == 0x65 /* 'e' */ )) {
										mEXP(false);
									}
									else {
									}
									
									}
									break;
								}
								default:
								{
									throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
								}
								}
								}
								}
								break;
							}
							case 0x2e /* '.' */ :
							{
								match('.' /* charlit */ );
								{ // ( ... )+
								int _cnt465=0;
								for (;;) {
									if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
										mD(false);
									}
									else {
										if ( _cnt465>=1 ) { goto _loop465; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
									}
									
									_cnt465++;
								}
								_loop465:;
								}  // ( ... )+
								{
								if ((LA(1) == 0x65 /* 'e' */ )) {
									mEXP(false);
								}
								else {
								}
								
								}
								break;
							}
							default:
							{
								throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
							}
							}
							}
							if ( inputState->guessing==0 ) {
								_ttype=CONSTANT_FLOAT;
							}
						}
						else if ((LA(1) == 0x22 /* '\"' */ ) && (true) && (true)) {
							_saveIndex = text.length();
							match('\"' /* charlit */ );
							text.erase(_saveIndex);
							{ // ( ... )*
							for (;;) {
								if ((LA(1) == 0x22 /* '\"' */ ) && (LA(2) == 0x22 /* '\"' */ )) {
									match('\"' /* charlit */ );
									_saveIndex = text.length();
									match('\"' /* charlit */ );
									text.erase(_saveIndex);
								}
								else if ((_tokenSet_10.member(LA(1)))) {
									{
									match(_tokenSet_10);
									}
								}
								else {
									goto _loop415;
								}
								
							}
							_loop415:;
							} // ( ... )*
							{
							if ((LA(1) == 0x22 /* '\"' */ )) {
								_saveIndex = text.length();
								match('\"' /* charlit */ );
								text.erase(_saveIndex);
							}
							else {
							}
							
							}
							if ( inputState->guessing==0 ) {
								_ttype=STRING_LITERAL;
							}
						}
						else if ((LA(1) == 0x27 /* '\'' */ ) && (true) && (true)) {
							_saveIndex = text.length();
							match('\'' /* charlit */ );
							text.erase(_saveIndex);
							{ // ( ... )*
							for (;;) {
								if ((LA(1) == 0x27 /* '\'' */ ) && (LA(2) == 0x27 /* '\'' */ )) {
									match('\'' /* charlit */ );
									_saveIndex = text.length();
									match('\'' /* charlit */ );
									text.erase(_saveIndex);
								}
								else if ((_tokenSet_11.member(LA(1)))) {
									{
									match(_tokenSet_11);
									}
								}
								else {
									goto _loop419;
								}
								
							}
							_loop419:;
							} // ( ... )*
							{
							if ((LA(1) == 0x27 /* '\'' */ )) {
								_saveIndex = text.length();
								match('\'' /* charlit */ );
								text.erase(_saveIndex);
							}
							else {
							}
							
							}
							if ( inputState->guessing==0 ) {
								_ttype=STRING_LITERAL;
							}
						}
						else if ((LA(1) == 0x2e /* '.' */ ) && (true)) {
							match('.' /* charlit */ );
							if ( inputState->guessing==0 ) {
								_ttype=DOT;
							}
						}
						else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (true) && (true)) {
							{ // ( ... )+
							int _cnt468=0;
							for (;;) {
								if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
									mD(false);
								}
								else {
									if ( _cnt468>=1 ) { goto _loop468; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
								}
								
								_cnt468++;
							}
							_loop468:;
							}  // ( ... )+
							if ( inputState->guessing==0 ) {
								_ttype=CONSTANT_I;
							}
							{
							switch ( LA(1)) {
							case 0x73 /* 's' */ :
							{
								_saveIndex = text.length();
								match('s' /* charlit */ );
								text.erase(_saveIndex);
								if ( inputState->guessing==0 ) {
									_ttype=CONSTANT_INT;
								}
								break;
							}
							case 0x62 /* 'b' */ :
							{
								_saveIndex = text.length();
								match('b' /* charlit */ );
								text.erase(_saveIndex);
								if ( inputState->guessing==0 ) {
									_ttype=CONSTANT_BYTE;
								}
								break;
							}
							default:
								if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (LA(3) == 0x6c /* 'l' */ )) {
									_saveIndex = text.length();
									match("ull");
									text.erase(_saveIndex);
									if ( inputState->guessing==0 ) {
										_ttype=CONSTANT_ULONG64;
									}
								}
								else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x62 /* 'b' */ )) {
									_saveIndex = text.length();
									match("ub");
									text.erase(_saveIndex);
									if ( inputState->guessing==0 ) {
										_ttype=CONSTANT_BYTE;
									}
								}
								else if ((LA(1) == 0x6c /* 'l' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
									_saveIndex = text.length();
									match("ll");
									text.erase(_saveIndex);
									if ( inputState->guessing==0 ) {
										_ttype=CONSTANT_LONG64;
									}
								}
								else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (true)) {
									_saveIndex = text.length();
									match("ul");
									text.erase(_saveIndex);
									if ( inputState->guessing==0 ) {
										_ttype=CONSTANT_ULONG;
									}
								}
								else if ((LA(1) == 0x75 /* 'u' */ ) && (true)) {
									_saveIndex = text.length();
									match('u' /* charlit */ );
									text.erase(_saveIndex);
									{
									if ((LA(1) == 0x73 /* 's' */ )) {
										_saveIndex = text.length();
										match('s' /* charlit */ );
										text.erase(_saveIndex);
									}
									else {
									}
									
									}
									if ( inputState->guessing==0 ) {
										_ttype=CONSTANT_UINT;
									}
								}
								else if ((LA(1) == 0x6c /* 'l' */ ) && (true)) {
									_saveIndex = text.length();
									match('l' /* charlit */ );
									text.erase(_saveIndex);
									if ( inputState->guessing==0 ) {
										_ttype=CONSTANT_LONG;
									}
								}
								else {
								}
							}
							}
						}
	else {
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}}}}}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCOMMENT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = COMMENT;
	std::string::size_type _saveIndex;
	
	match(';' /* charlit */ );
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_2.member(LA(1))) && (true) && (true)) {
			{
			match(_tokenSet_2);
			}
		}
		else {
			goto _loop474;
		}
		
	}
	_loop474:;
	} // ( ... )*
	if ( inputState->guessing==0 ) {
		_ttype=antlr::Token::SKIP;
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mIDENTIFIER(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = IDENTIFIER;
	std::string::size_type _saveIndex;
	
	{
	mL(false);
	}
	{ // ( ... )*
	for (;;) {
		switch ( LA(1)) {
		case 0x5f /* '_' */ :
		case 0x61 /* 'a' */ :
		case 0x62 /* 'b' */ :
		case 0x63 /* 'c' */ :
		case 0x64 /* 'd' */ :
		case 0x65 /* 'e' */ :
		case 0x66 /* 'f' */ :
		case 0x67 /* 'g' */ :
		case 0x68 /* 'h' */ :
		case 0x69 /* 'i' */ :
		case 0x6a /* 'j' */ :
		case 0x6b /* 'k' */ :
		case 0x6c /* 'l' */ :
		case 0x6d /* 'm' */ :
		case 0x6e /* 'n' */ :
		case 0x6f /* 'o' */ :
		case 0x70 /* 'p' */ :
		case 0x71 /* 'q' */ :
		case 0x72 /* 'r' */ :
		case 0x73 /* 's' */ :
		case 0x74 /* 't' */ :
		case 0x75 /* 'u' */ :
		case 0x76 /* 'v' */ :
		case 0x77 /* 'w' */ :
		case 0x78 /* 'x' */ :
		case 0x79 /* 'y' */ :
		case 0x7a /* 'z' */ :
		{
			mL(false);
			break;
		}
		case 0x30 /* '0' */ :
		case 0x31 /* '1' */ :
		case 0x32 /* '2' */ :
		case 0x33 /* '3' */ :
		case 0x34 /* '4' */ :
		case 0x35 /* '5' */ :
		case 0x36 /* '6' */ :
		case 0x37 /* '7' */ :
		case 0x38 /* '8' */ :
		case 0x39 /* '9' */ :
		{
			mD(false);
			break;
		}
		case 0x24 /* '$' */ :
		{
			match('$' /* charlit */ );
			break;
		}
		default:
		{
			goto _loop478;
		}
		}
	}
	_loop478:;
	} // ( ... )*
	if ( inputState->guessing==0 ) {
		
			  std::string s=StrUpCase( text.substr(_begin,text.length()-_begin));
			  { text.erase(_begin); text +=  s; }; 
			
	}
	_ttype = testLiteralsTable(_ttype);
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mSYSVARNAME(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = SYSVARNAME;
	std::string::size_type _saveIndex;
	
	{
	match('!' /* charlit */ );
	}
	{ // ( ... )+
	int _cnt482=0;
	for (;;) {
		switch ( LA(1)) {
		case 0x5f /* '_' */ :
		case 0x61 /* 'a' */ :
		case 0x62 /* 'b' */ :
		case 0x63 /* 'c' */ :
		case 0x64 /* 'd' */ :
		case 0x65 /* 'e' */ :
		case 0x66 /* 'f' */ :
		case 0x67 /* 'g' */ :
		case 0x68 /* 'h' */ :
		case 0x69 /* 'i' */ :
		case 0x6a /* 'j' */ :
		case 0x6b /* 'k' */ :
		case 0x6c /* 'l' */ :
		case 0x6d /* 'm' */ :
		case 0x6e /* 'n' */ :
		case 0x6f /* 'o' */ :
		case 0x70 /* 'p' */ :
		case 0x71 /* 'q' */ :
		case 0x72 /* 'r' */ :
		case 0x73 /* 's' */ :
		case 0x74 /* 't' */ :
		case 0x75 /* 'u' */ :
		case 0x76 /* 'v' */ :
		case 0x77 /* 'w' */ :
		case 0x78 /* 'x' */ :
		case 0x79 /* 'y' */ :
		case 0x7a /* 'z' */ :
		{
			mL(false);
			break;
		}
		case 0x30 /* '0' */ :
		case 0x31 /* '1' */ :
		case 0x32 /* '2' */ :
		case 0x33 /* '3' */ :
		case 0x34 /* '4' */ :
		case 0x35 /* '5' */ :
		case 0x36 /* '6' */ :
		case 0x37 /* '7' */ :
		case 0x38 /* '8' */ :
		case 0x39 /* '9' */ :
		{
			mD(false);
			break;
		}
		case 0x24 /* '$' */ :
		{
			match('$' /* charlit */ );
			break;
		}
		default:
		{
			if ( _cnt482>=1 ) { goto _loop482; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		}
		_cnt482++;
	}
	_loop482:;
	}  // ( ... )+
	if ( inputState->guessing==0 ) {
		
			  std::string s=StrUpCase( text.substr(_begin,text.length()-_begin));
			  { text.erase(_begin); text +=  s; }; 
			
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mEND_MARKER(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = END_MARKER;
	std::string::size_type _saveIndex;
	
	match('&' /* charlit */ );
	if ( inputState->guessing==0 ) {
		_ttype=END_U;
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mWHITESPACE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = WHITESPACE;
	std::string::size_type _saveIndex;
	
	{ // ( ... )+
	int _cnt486=0;
	for (;;) {
		if ((LA(1) == 0x9 /* '\t' */  || LA(1) == 0xc /* '\14' */  || LA(1) == 0x20 /* ' ' */ )) {
			mW(false);
		}
		else {
			if ( _cnt486>=1 ) { goto _loop486; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt486++;
	}
	_loop486:;
	}  // ( ... )+
	if ( inputState->guessing==0 ) {
		_ttype=antlr::Token::SKIP;
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mSKIP_LINES(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = SKIP_LINES;
	std::string::size_type _saveIndex;
	
	{ // ( ... )*
	for (;;) {
		switch ( LA(1)) {
		case 0x3b /* ';' */ :
		{
			mCOMMENT(false);
			break;
		}
		case 0x9 /* '\t' */ :
		case 0xc /* '\14' */ :
		case 0x20 /* ' ' */ :
		{
			mW(false);
			break;
		}
		case 0xa /* '\n' */ :
		case 0xd /* '\r' */ :
		{
			mEOL(false);
			break;
		}
		default:
		{
			goto _loop489;
		}
		}
	}
	_loop489:;
	} // ( ... )*
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONT_STATEMENT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONT_STATEMENT;
	std::string::size_type _saveIndex;
	
	match('$' /* charlit */ );
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_2.member(LA(1)))) {
			{
			match(_tokenSet_2);
			}
		}
		else {
			goto _loop493;
		}
		
	}
	_loop493:;
	} // ( ... )*
	mEOL(false);
	mSKIP_LINES(false);
	if ( inputState->guessing==0 ) {
		
		++lineContinuation;
		_ttype=antlr::Token::SKIP; 
		
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mEND_OF_LINE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = END_OF_LINE;
	std::string::size_type _saveIndex;
	
	mEOL(false);
	mSKIP_LINES(false);
	if ( inputState->guessing==0 ) {
		_ttype=END_U;
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mMAX_TOKEN_NUMBER(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = MAX_TOKEN_NUMBER;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}


const unsigned long GDLLexer::_tokenSet_0_data_[] = { 0UL, 67043344UL, 2147483648UL, 134217726UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// $ 0 1 2 3 4 5 6 7 8 9 _ a b c d e f g h i j k l m n o p q r s t u v 
// w x y z 
const antlr::BitSet GDLLexer::_tokenSet_0(_tokenSet_0_data_,10);
const unsigned long GDLLexer::_tokenSet_1_data_[] = { 0UL, 0UL, 2147483648UL, 134217726UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// _ a b c d e f g h i j k l m n o p q r s t u v w x y z 
const antlr::BitSet GDLLexer::_tokenSet_1(_tokenSet_1_data_,10);
const unsigned long GDLLexer::_tokenSet_2_data_[] = { 4294958072UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xc 0xe 0xf 0x10 0x11 0x12 0x13 0x14 
// 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # $ % 
// & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G 
// H I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g h 
// i j k l m n o p q r s t u v w x y z { | } ~ 0x7f 0x80 0x81 0x82 0x83 
// 0x84 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 0x90 0x91 
// 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f 
// 0xa0 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 
// 0xae 0xaf 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 
// 0xbc 0xbd 0xbe 0xbf 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 
// 0xca 0xcb 0xcc 0xcd 0xce 0xcf 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 
// 0xd8 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 
// 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 
const antlr::BitSet GDLLexer::_tokenSet_2(_tokenSet_2_data_,16);
const unsigned long GDLLexer::_tokenSet_3_data_[] = { 0UL, 67053568UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// + - 0 1 2 3 4 5 6 7 8 9 
const antlr::BitSet GDLLexer::_tokenSet_3(_tokenSet_3_data_,10);
const unsigned long GDLLexer::_tokenSet_4_data_[] = { 0UL, 67043328UL, 0UL, 126UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0 1 2 3 4 5 6 7 8 9 a b c d e f 
const antlr::BitSet GDLLexer::_tokenSet_4(_tokenSet_4_data_,10);
const unsigned long GDLLexer::_tokenSet_5_data_[] = { 0UL, 67043456UL, 0UL, 126UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// \' 0 1 2 3 4 5 6 7 8 9 a b c d e f 
const antlr::BitSet GDLLexer::_tokenSet_5(_tokenSet_5_data_,10);
const unsigned long GDLLexer::_tokenSet_6_data_[] = { 0UL, 16711808UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// \' 0 1 2 3 4 5 6 7 
const antlr::BitSet GDLLexer::_tokenSet_6(_tokenSet_6_data_,10);
const unsigned long GDLLexer::_tokenSet_7_data_[] = { 0UL, 67059712UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 
const antlr::BitSet GDLLexer::_tokenSet_7(_tokenSet_7_data_,10);
const unsigned long GDLLexer::_tokenSet_8_data_[] = { 0UL, 67059712UL, 0UL, 16UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 d 
const antlr::BitSet GDLLexer::_tokenSet_8(_tokenSet_8_data_,10);
const unsigned long GDLLexer::_tokenSet_9_data_[] = { 0UL, 67059712UL, 0UL, 32UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 e 
const antlr::BitSet GDLLexer::_tokenSet_9(_tokenSet_9_data_,10);
const unsigned long GDLLexer::_tokenSet_10_data_[] = { 4294958072UL, 4294967291UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xc 0xe 0xf 0x10 0x11 0x12 0x13 0x14 
// 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! # $ % & \' 
// ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H I 
// J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g h i j 
// k l m n o p q r s t u v w x y z { | } ~ 0x7f 0x80 0x81 0x82 0x83 0x84 
// 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 0x90 0x91 0x92 
// 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f 0xa0 
// 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 
// 0xaf 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 0xbc 
// 0xbd 0xbe 0xbf 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 0xca 
// 0xcb 0xcc 0xcd 0xce 0xcf 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 0xd8 
// 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 
// 0xe7 0xe8 0xe9 0xea 0xeb 0xec 
const antlr::BitSet GDLLexer::_tokenSet_10(_tokenSet_10_data_,16);
const unsigned long GDLLexer::_tokenSet_11_data_[] = { 4294958072UL, 4294967167UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xb 0xc 0xe 0xf 0x10 0x11 0x12 0x13 0x14 
// 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # $ % 
// & ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H 
// I J K L M N O P Q R S T U V W X Y Z [ 0x5c ] ^ _ ` a b c d e f g h i 
// j k l m n o p q r s t u v w x y z { | } ~ 0x7f 0x80 0x81 0x82 0x83 0x84 
// 0x85 0x86 0x87 0x88 0x89 0x8a 0x8b 0x8c 0x8d 0x8e 0x8f 0x90 0x91 0x92 
// 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9a 0x9b 0x9c 0x9d 0x9e 0x9f 0xa0 
// 0xa1 0xa2 0xa3 0xa4 0xa5 0xa6 0xa7 0xa8 0xa9 0xaa 0xab 0xac 0xad 0xae 
// 0xaf 0xb0 0xb1 0xb2 0xb3 0xb4 0xb5 0xb6 0xb7 0xb8 0xb9 0xba 0xbb 0xbc 
// 0xbd 0xbe 0xbf 0xc0 0xc1 0xc2 0xc3 0xc4 0xc5 0xc6 0xc7 0xc8 0xc9 0xca 
// 0xcb 0xcc 0xcd 0xce 0xcf 0xd0 0xd1 0xd2 0xd3 0xd4 0xd5 0xd6 0xd7 0xd8 
// 0xd9 0xda 0xdb 0xdc 0xdd 0xde 0xdf 0xe0 0xe1 0xe2 0xe3 0xe4 0xe5 0xe6 
// 0xe7 0xe8 0xe9 0xea 0xeb 0xec 
const antlr::BitSet GDLLexer::_tokenSet_11(_tokenSet_11_data_,16);

