/* $ANTLR 2.7.7 (2006-11-01): "gdlc.g" -> "GDLLexer.cpp"$ */

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
#include <fstream>
#include <iostream>
#include <string>

static void printLineErrorHelper(std::string filename, int line, int col) {
  if (filename.size() > 0) {
	std::ifstream ifs;
	ifs.open(filename, std::ifstream::in);
	int linenum = 0;
	std::string str;
	while (std::getline(ifs, str)) {
	  linenum++;
	  if (linenum == line) {
		std::cerr << std::endl << str << std::endl; //skip one line, print line
		break;
	  }
	}
	ifs.close();
  } else {
	for (auto i = 0; i < SysVar::Prompt().size(); ++i) std::cerr << ' ';
  }
  for (auto i = 0; i < col; ++i) std::cerr << ' ';
  std::cerr << '^';
  std::cerr << '\n';
  std::cerr << "% Syntax error.\n";
  if ( filename.size() > 0)   std::cerr <<"  At: "<<filename<<", Line "<<line<<std::endl;
  return;
}

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
	literals["endcase"] = 101;
	literals["case"] = 95;
	literals["while"] = 91;
	literals["repeat"] = 81;
	literals["ne"] = 120;
	literals["endif"] = 105;
	literals["end"] = 100;
	literals["le"] = 117;
	literals["then"] = 127;
	literals["endswitch"] = 107;
	literals["until"] = 128;
	literals["and"] = 93;
	literals["endrep"] = 106;
	literals["not"] = 121;
	literals["foreach"] = 33;
	literals["mod"] = 119;
	literals["forward_function"] = 110;
	literals["do"] = 98;
	literals["function"] = 111;
	literals["endfor"] = 103;
	literals["gt"] = 114;
	literals["compile_opt"] = 97;
	literals["inherits"] = 116;
	literals["of"] = 122;
	literals["or"] = 124;
	literals["if"] = 115;
	literals["pro"] = 125;
	literals["xor"] = 129;
	literals["ge"] = 112;
	literals["goto"] = 113;
	literals["for"] = 31;
	literals["eq"] = 109;
	literals["on_ioerror"] = 123;
	literals["endelse"] = 102;
	literals["begin"] = 94;
	literals["else"] = 99;
	literals["lt"] = 118;
	literals["endwhile"] = 108;
	literals["switch"] = 126;
	literals["common"] = 96;
	literals["endforeach"] = 104;
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
				if ((LA(1) == 0x61 /* 'a' */ ) && (LA(2) == 0x6e /* 'n' */ ) && (LA(3) == 0x64 /* 'd' */ ) && (LA(4) == 0x3d /* '=' */ )) {
					mAND_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x6d /* 'm' */ ) && (LA(2) == 0x6f /* 'o' */ ) && (LA(3) == 0x64 /* 'd' */ ) && (LA(4) == 0x3d /* '=' */ )) {
					mMOD_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x78 /* 'x' */ ) && (LA(2) == 0x6f /* 'o' */ ) && (LA(3) == 0x72 /* 'r' */ ) && (LA(4) == 0x3d /* '=' */ )) {
					mXOR_OP_EQ(true);
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
				else if ((LA(1) == 0x6e /* 'n' */ ) && (LA(2) == 0x65 /* 'e' */ ) && (LA(3) == 0x3d /* '=' */ )) {
					mNE_OP_EQ(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x6f /* 'o' */ ) && (LA(2) == 0x72 /* 'r' */ ) && (LA(3) == 0x3d /* '=' */ )) {
					mOR_OP_EQ(true);
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
				else if ((_tokenSet_1.member(LA(1))) && (true) && (true) && (true)) {
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
			goto _loop309;
		}
		
	}
	_loop309:;
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
		size_t pos = name.find_first_of(';', 0);   
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
	bool synPredMatched361 = false;
	if (((LA(1) == 0xd /* '\r' */ ) && (LA(2) == 0xa /* '\n' */ ) && (true) && (true))) {
		int _m361 = mark();
		synPredMatched361 = true;
		inputState->guessing++;
		try {
			{
			match("\r\n");
			}
		}
		catch (antlr::RecognitionException& pe) {
			synPredMatched361 = false;
		}
		rewind(_m361);
		inputState->guessing--;
	}
	if ( synPredMatched361 ) {
		match("\r\n");
	}
	else if ((LA(1) == 0xa /* '\n' */ )) {
		match('\n' /* charlit */ );
	}
	else if ((LA(1) == 0xd /* '\r' */ ) && (true) && (true) && (true)) {
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
		int _cnt379=0;
		for (;;) {
			if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
				mD(false);
			}
			else {
				if ( _cnt379>=1 ) { goto _loop379; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			
			_cnt379++;
		}
		_loop379:;
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
		int _cnt386=0;
		for (;;) {
			if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
				mD(false);
			}
			else {
				if ( _cnt386>=1 ) { goto _loop386; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			
			_cnt386++;
		}
		_loop386:;
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

void GDLLexer::mCONSTANT_CMPLX_I(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_CMPLX_I;
	std::string::size_type _saveIndex;
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void GDLLexer::mCONSTANT_CMPLXDBL_I(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CONSTANT_CMPLXDBL_I;
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
	
	bool synPredMatched461 = false;
	if (((LA(1) == 0x27 /* '\'' */ ) && (_tokenSet_4.member(LA(2))) && (_tokenSet_5.member(LA(3))) && (_tokenSet_6.member(LA(4))))) {
		int _m461 = mark();
		synPredMatched461 = true;
		inputState->guessing++;
		try {
			{
			match('\'' /* charlit */ );
			{ // ( ... )+
			int _cnt459=0;
			for (;;) {
				if ((_tokenSet_4.member(LA(1)))) {
					mH(false);
				}
				else {
					if ( _cnt459>=1 ) { goto _loop459; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				
				_cnt459++;
			}
			_loop459:;
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
			synPredMatched461 = false;
		}
		rewind(_m461);
		inputState->guessing--;
	}
	if ( synPredMatched461 ) {
		{
		_saveIndex = text.length();
		match('\'' /* charlit */ );
		text.erase(_saveIndex);
		{ // ( ... )+
		int _cnt464=0;
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				mH(false);
			}
			else {
				if ( _cnt464>=1 ) { goto _loop464; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			
			_cnt464++;
		}
		_loop464:;
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
		bool synPredMatched470 = false;
		if (((LA(1) == 0x27 /* '\'' */ ) && ((LA(2) >= 0x30 /* '0' */  && LA(2) <= 0x37 /* '7' */ )) && (_tokenSet_7.member(LA(3))) && (_tokenSet_8.member(LA(4))))) {
			int _m470 = mark();
			synPredMatched470 = true;
			inputState->guessing++;
			try {
				{
				match('\'' /* charlit */ );
				{ // ( ... )+
				int _cnt468=0;
				for (;;) {
					if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ ))) {
						mO(false);
					}
					else {
						if ( _cnt468>=1 ) { goto _loop468; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					
					_cnt468++;
				}
				_loop468:;
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
				synPredMatched470 = false;
			}
			rewind(_m470);
			inputState->guessing--;
		}
		if ( synPredMatched470 ) {
			{
			_saveIndex = text.length();
			match('\'' /* charlit */ );
			text.erase(_saveIndex);
			{ // ( ... )+
			int _cnt473=0;
			for (;;) {
				if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ ))) {
					mO(false);
				}
				else {
					if ( _cnt473>=1 ) { goto _loop473; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
				}
				
				_cnt473++;
			}
			_loop473:;
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
			bool synPredMatched479 = false;
			if (((LA(1) == 0x27 /* '\'' */ ) && (LA(2) == 0x30 /* '0' */  || LA(2) == 0x31 /* '1' */ ) && (LA(3) == 0x27 /* '\'' */  || LA(3) == 0x30 /* '0' */  || LA(3) == 0x31 /* '1' */ ) && (_tokenSet_9.member(LA(4))))) {
				int _m479 = mark();
				synPredMatched479 = true;
				inputState->guessing++;
				try {
					{
					match('\'' /* charlit */ );
					{ // ( ... )+
					int _cnt477=0;
					for (;;) {
						if ((LA(1) == 0x30 /* '0' */  || LA(1) == 0x31 /* '1' */ )) {
							mB(false);
						}
						else {
							if ( _cnt477>=1 ) { goto _loop477; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt477++;
					}
					_loop477:;
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
					synPredMatched479 = false;
				}
				rewind(_m479);
				inputState->guessing--;
			}
			if ( synPredMatched479 ) {
				{
				_saveIndex = text.length();
				match('\'' /* charlit */ );
				text.erase(_saveIndex);
				{ // ( ... )+
				int _cnt482=0;
				for (;;) {
					if ((LA(1) == 0x30 /* '0' */  || LA(1) == 0x31 /* '1' */ )) {
						mB(false);
					}
					else {
						if ( _cnt482>=1 ) { goto _loop482; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
					}
					
					_cnt482++;
				}
				_loop482:;
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
				bool synPredMatched443 = false;
				if (((LA(1) == 0x30 /* '0' */ ) && (LA(2) == 0x62 /* 'b' */ ) && (LA(3) == 0x30 /* '0' */  || LA(3) == 0x31 /* '1' */ ) && (true))) {
					int _m443 = mark();
					synPredMatched443 = true;
					inputState->guessing++;
					try {
						{
						match("0b");
						{ // ( ... )+
						int _cnt441=0;
						for (;;) {
							if ((LA(1) == 0x30 /* '0' */  || LA(1) == 0x31 /* '1' */ )) {
								mB(false);
							}
							else {
								if ( _cnt441>=1 ) { goto _loop441; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt441++;
						}
						_loop441:;
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
						default:
							if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (LA(3) == 0x6c /* 'l' */ )) {
								match("ull");
							}
							else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x73 /* 's' */ )) {
								match("us");
							}
							else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x62 /* 'b' */ )) {
								match("ub");
							}
							else if ((LA(1) == 0x6c /* 'l' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
								match("ll");
							}
							else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (true)) {
								match("ul");
							}
							else if ((LA(1) == 0x6c /* 'l' */ ) && (true)) {
								match('l' /* charlit */ );
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
						synPredMatched443 = false;
					}
					rewind(_m443);
					inputState->guessing--;
				}
				if ( synPredMatched443 ) {
					{
					_saveIndex = text.length();
					match("0b");
					text.erase(_saveIndex);
					{ // ( ... )+
					int _cnt446=0;
					for (;;) {
						if ((LA(1) == 0x30 /* '0' */  || LA(1) == 0x31 /* '1' */ )) {
							mB(false);
						}
						else {
							if ( _cnt446>=1 ) { goto _loop446; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
						}
						
						_cnt446++;
					}
					_loop446:;
					}  // ( ... )+
					if ( inputState->guessing==0 ) {
						_ttype=CONSTANT_BIN_I;
					}
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
						match("b");
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
						}
					}
					}
					}
				}
				else {
					bool synPredMatched425 = false;
					if (((LA(1) == 0x30 /* '0' */ ) && (LA(2) == 0x78 /* 'x' */ ))) {
						int _m425 = mark();
						synPredMatched425 = true;
						inputState->guessing++;
						try {
							{
							match("0x");
							{ // ( ... )+
							int _cnt423=0;
							for (;;) {
								if ((_tokenSet_4.member(LA(1)))) {
									mH(false);
								}
								else {
									if ( _cnt423>=1 ) { goto _loop423; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
								}
								
								_cnt423++;
							}
							_loop423:;
							}  // ( ... )+
							{
							switch ( LA(1)) {
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
							case 0x75 /* 'u' */ :
							{
								match('u' /* charlit */ );
								break;
							}
							default:
								{
								}
							}
							}
							}
						}
						catch (antlr::RecognitionException& pe) {
							synPredMatched425 = false;
						}
						rewind(_m425);
						inputState->guessing--;
					}
					if ( synPredMatched425 ) {
						{
						_saveIndex = text.length();
						match("0x");
						text.erase(_saveIndex);
						{ // ( ... )+
						int _cnt428=0;
						for (;;) {
							if ((_tokenSet_4.member(LA(1)))) {
								mH(false);
							}
							else {
								if ( _cnt428>=1 ) { goto _loop428; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
							}
							
							_cnt428++;
						}
						_loop428:;
						}  // ( ... )+
						if ( inputState->guessing==0 ) {
							_ttype=CONSTANT_HEX_I;
						}
						{
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
						else if ((LA(1) == 0x73 /* 's' */ )) {
							_saveIndex = text.length();
							match('s' /* charlit */ );
							text.erase(_saveIndex);
							if ( inputState->guessing==0 ) {
								_ttype=CONSTANT_HEX_INT;
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
						}
						
						}
						}
					}
					else {
						bool synPredMatched434 = false;
						if (((LA(1) == 0x30 /* '0' */ ) && (LA(2) == 0x6f /* 'o' */ ))) {
							int _m434 = mark();
							synPredMatched434 = true;
							inputState->guessing++;
							try {
								{
								match("0o");
								{ // ( ... )+
								int _cnt432=0;
								for (;;) {
									if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ ))) {
										mO(false);
									}
									else {
										if ( _cnt432>=1 ) { goto _loop432; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
									}
									
									_cnt432++;
								}
								_loop432:;
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
								default:
									if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (LA(3) == 0x6c /* 'l' */ )) {
										match("ull");
									}
									else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x73 /* 's' */ )) {
										match("us");
									}
									else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x62 /* 'b' */ )) {
										match("ub");
									}
									else if ((LA(1) == 0x6c /* 'l' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
										match("ll");
									}
									else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (true)) {
										match("ul");
									}
									else if ((LA(1) == 0x6c /* 'l' */ ) && (true)) {
										match('l' /* charlit */ );
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
								synPredMatched434 = false;
							}
							rewind(_m434);
							inputState->guessing--;
						}
						if ( synPredMatched434 ) {
							{
							_saveIndex = text.length();
							match("0o");
							text.erase(_saveIndex);
							{ // ( ... )+
							int _cnt437=0;
							for (;;) {
								if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ ))) {
									mO(false);
								}
								else {
									if ( _cnt437>=1 ) { goto _loop437; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
								}
								
								_cnt437++;
							}
							_loop437:;
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
								match("b");
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
							bool synPredMatched452 = false;
							if (((LA(1) == 0x22 /* '\"' */ ) && ((LA(2) >= 0x30 /* '0' */  && LA(2) <= 0x37 /* '7' */ )) && (true) && (true))) {
								int _m452 = mark();
								synPredMatched452 = true;
								inputState->guessing++;
								try {
									{
									match('\"' /* charlit */ );
									{ // ( ... )+
									int _cnt450=0;
									for (;;) {
										if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ ))) {
											mO(false);
										}
										else {
											if ( _cnt450>=1 ) { goto _loop450; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
										}
										
										_cnt450++;
									}
									_loop450:;
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
									case 0x22 /* '\"' */ :
									{
										match('\"' /* charlit */ );
										break;
									}
									default:
										if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (LA(3) == 0x6c /* 'l' */ )) {
											match("ull");
										}
										else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x73 /* 's' */ )) {
											match("us");
										}
										else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x62 /* 'b' */ )) {
											match("ub");
										}
										else if ((LA(1) == 0x6c /* 'l' */ ) && (LA(2) == 0x6c /* 'l' */ )) {
											match("ll");
										}
										else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x6c /* 'l' */ ) && (true)) {
											match("ul");
										}
										else if ((LA(1) == 0x6c /* 'l' */ ) && (true)) {
											match('l' /* charlit */ );
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
									synPredMatched452 = false;
								}
								rewind(_m452);
								inputState->guessing--;
							}
							if ( synPredMatched452 ) {
								{
								_saveIndex = text.length();
								match('\"' /* charlit */ );
								text.erase(_saveIndex);
								{ // ( ... )+
								int _cnt455=0;
								for (;;) {
									if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ ))) {
										mO(false);
									}
									else {
										if ( _cnt455>=1 ) { goto _loop455; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
									}
									
									_cnt455++;
								}
								_loop455:;
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
								case 0x22 /* '\"' */ :
								{
									_saveIndex = text.length();
									match("\"");
									text.erase(_saveIndex);
									if ( inputState->guessing==0 ) {
										_ttype=STRING_LITERAL;
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
								bool synPredMatched503 = false;
								if (((_tokenSet_10.member(LA(1))) && (_tokenSet_11.member(LA(2))) && (true) && (true))) {
									int _m503 = mark();
									synPredMatched503 = true;
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
											int _cnt495=0;
											for (;;) {
												if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
													mD(false);
												}
												else {
													if ( _cnt495>=1 ) { goto _loop495; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
												}
												
												_cnt495++;
											}
											_loop495:;
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
														goto _loop498;
													}
													
												}
												_loop498:;
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
											int _cnt501=0;
											for (;;) {
												if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
													mD(false);
												}
												else {
													if ( _cnt501>=1 ) { goto _loop501; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
												}
												
												_cnt501++;
											}
											_loop501:;
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
										synPredMatched503 = false;
									}
									rewind(_m503);
									inputState->guessing--;
								}
								if ( synPredMatched503 ) {
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
										int _cnt507=0;
										for (;;) {
											if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
												mD(false);
											}
											else {
												if ( _cnt507>=1 ) { goto _loop507; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
											}
											
											_cnt507++;
										}
										_loop507:;
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
													goto _loop510;
												}
												
											}
											_loop510:;
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
										int _cnt513=0;
										for (;;) {
											if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
												mD(false);
											}
											else {
												if ( _cnt513>=1 ) { goto _loop513; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
											}
											
											_cnt513++;
										}
										_loop513:;
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
									{
									if ((LA(1) == 0x69 /* 'i' */  || LA(1) == 0x6a /* 'j' */ )) {
										{
										switch ( LA(1)) {
										case 0x69 /* 'i' */ :
										{
											match('i' /* charlit */ );
											break;
										}
										case 0x6a /* 'j' */ :
										{
											match('j' /* charlit */ );
											break;
										}
										default:
										{
											throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
										}
										}
										}
										if ( inputState->guessing==0 ) {
											_ttype=CONSTANT_CMPLXDBL_I;
										}
									}
									else {
										if ( inputState->guessing==0 ) {
											_ttype=CONSTANT_DOUBLE;
										}
									}
									
									}
								}
								else {
									bool synPredMatched528 = false;
									if (((_tokenSet_10.member(LA(1))) && (_tokenSet_12.member(LA(2))) && (true) && (true))) {
										int _m528 = mark();
										synPredMatched528 = true;
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
												int _cnt520=0;
												for (;;) {
													if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
														mD(false);
													}
													else {
														if ( _cnt520>=1 ) { goto _loop520; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
													}
													
													_cnt520++;
												}
												_loop520:;
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
															goto _loop523;
														}
														
													}
													_loop523:;
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
												int _cnt526=0;
												for (;;) {
													if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
														mD(false);
													}
													else {
														if ( _cnt526>=1 ) { goto _loop526; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
													}
													
													_cnt526++;
												}
												_loop526:;
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
											synPredMatched528 = false;
										}
										rewind(_m528);
										inputState->guessing--;
									}
									if ( synPredMatched528 ) {
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
											int _cnt532=0;
											for (;;) {
												if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
													mD(false);
												}
												else {
													if ( _cnt532>=1 ) { goto _loop532; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
												}
												
												_cnt532++;
											}
											_loop532:;
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
														goto _loop535;
													}
													
												}
												_loop535:;
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
											int _cnt538=0;
											for (;;) {
												if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
													mD(false);
												}
												else {
													if ( _cnt538>=1 ) { goto _loop538; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
												}
												
												_cnt538++;
											}
											_loop538:;
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
										{
										if ((LA(1) == 0x69 /* 'i' */  || LA(1) == 0x6a /* 'j' */ )) {
											{
											switch ( LA(1)) {
											case 0x69 /* 'i' */ :
											{
												match('i' /* charlit */ );
												break;
											}
											case 0x6a /* 'j' */ :
											{
												match('j' /* charlit */ );
												break;
											}
											default:
											{
												throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
											}
											}
											}
											if ( inputState->guessing==0 ) {
												_ttype=CONSTANT_CMPLX_I;
											}
										}
										else {
											if ( inputState->guessing==0 ) {
												_ttype=CONSTANT_FLOAT;
											}
										}
										
										}
									}
									else if ((LA(1) == 0x22 /* '\"' */ ) && (true) && (true) && (true)) {
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
											else if ((_tokenSet_13.member(LA(1)))) {
												{
												match(_tokenSet_13);
												}
											}
											else {
												goto _loop486;
											}
											
										}
										_loop486:;
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
									else if ((LA(1) == 0x27 /* '\'' */ ) && (true) && (true) && (true)) {
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
											else if ((_tokenSet_14.member(LA(1)))) {
												{
												match(_tokenSet_14);
												}
											}
											else {
												goto _loop490;
											}
											
										}
										_loop490:;
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
									else if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ )) && (true) && (true) && (true)) {
										{ // ( ... )+
										int _cnt543=0;
										for (;;) {
											if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
												mD(false);
											}
											else {
												if ( _cnt543>=1 ) { goto _loop543; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
											}
											
											_cnt543++;
										}
										_loop543:;
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
										case 0x69 /* 'i' */ :
										case 0x6a /* 'j' */ :
										{
											{
											switch ( LA(1)) {
											case 0x69 /* 'i' */ :
											{
												_saveIndex = text.length();
												match('i' /* charlit */ );
												text.erase(_saveIndex);
												break;
											}
											case 0x6a /* 'j' */ :
											{
												_saveIndex = text.length();
												match('j' /* charlit */ );
												text.erase(_saveIndex);
												break;
											}
											default:
											{
												throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
											}
											}
											}
											if ( inputState->guessing==0 ) {
												_ttype=CONSTANT_CMPLX_I;
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
											else if ((LA(1) == 0x75 /* 'u' */ ) && (LA(2) == 0x73 /* 's' */ )) {
												_saveIndex = text.length();
												match("us");
												text.erase(_saveIndex);
												if ( inputState->guessing==0 ) {
													_ttype=CONSTANT_UINT;
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
												if ( inputState->guessing==0 ) {
													_ttype=CONSTANT_UI;
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
	}}}}}}}}
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
		if ((_tokenSet_2.member(LA(1))) && (true) && (true) && (true)) {
			{
			match(_tokenSet_2);
			}
		}
		else {
			goto _loop549;
		}
		
	}
	_loop549:;
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
			goto _loop553;
		}
		}
	}
	_loop553:;
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
	int _cnt557=0;
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
			if ( _cnt557>=1 ) { goto _loop557; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		}
		_cnt557++;
	}
	_loop557:;
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
	int _cnt561=0;
	for (;;) {
		if ((LA(1) == 0x9 /* '\t' */  || LA(1) == 0xc /* '\14' */  || LA(1) == 0x20 /* ' ' */ )) {
			mW(false);
		}
		else {
			if ( _cnt561>=1 ) { goto _loop561; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt561++;
	}
	_loop561:;
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
			goto _loop564;
		}
		}
	}
	_loop564:;
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
			goto _loop568;
		}
		
	}
	_loop568:;
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
// 0xe6 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef 
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
const unsigned long GDLLexer::_tokenSet_6_data_[] = { 0UL, 67043456UL, 0UL, 16777342UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// \' 0 1 2 3 4 5 6 7 8 9 a b c d e f x 
const antlr::BitSet GDLLexer::_tokenSet_6(_tokenSet_6_data_,10);
const unsigned long GDLLexer::_tokenSet_7_data_[] = { 0UL, 16711808UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// \' 0 1 2 3 4 5 6 7 
const antlr::BitSet GDLLexer::_tokenSet_7(_tokenSet_7_data_,10);
const unsigned long GDLLexer::_tokenSet_8_data_[] = { 0UL, 16711808UL, 0UL, 32768UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// \' 0 1 2 3 4 5 6 7 o 
const antlr::BitSet GDLLexer::_tokenSet_8(_tokenSet_8_data_,10);
const unsigned long GDLLexer::_tokenSet_9_data_[] = { 0UL, 196736UL, 0UL, 4UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// \' 0 1 b 
const antlr::BitSet GDLLexer::_tokenSet_9(_tokenSet_9_data_,10);
const unsigned long GDLLexer::_tokenSet_10_data_[] = { 0UL, 67059712UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 
const antlr::BitSet GDLLexer::_tokenSet_10(_tokenSet_10_data_,10);
const unsigned long GDLLexer::_tokenSet_11_data_[] = { 0UL, 67059712UL, 0UL, 16UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 d 
const antlr::BitSet GDLLexer::_tokenSet_11(_tokenSet_11_data_,10);
const unsigned long GDLLexer::_tokenSet_12_data_[] = { 0UL, 67059712UL, 0UL, 32UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// . 0 1 2 3 4 5 6 7 8 9 e 
const antlr::BitSet GDLLexer::_tokenSet_12(_tokenSet_12_data_,10);
const unsigned long GDLLexer::_tokenSet_13_data_[] = { 4294958072UL, 4294967291UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
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
// 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef 
const antlr::BitSet GDLLexer::_tokenSet_13(_tokenSet_13_data_,16);
const unsigned long GDLLexer::_tokenSet_14_data_[] = { 4294958072UL, 4294967167UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
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
// 0xe7 0xe8 0xe9 0xea 0xeb 0xec 0xed 0xee 0xef 
const antlr::BitSet GDLLexer::_tokenSet_14(_tokenSet_14_data_,16);

