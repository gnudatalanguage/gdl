/* $ANTLR 2.7.4: "format.g" -> "FMTLexer.cpp"$ */
#include "FMTLexer.hpp"
#include <antlr/CharBuffer.hpp>
#include <antlr/TokenStreamException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>
#include <antlr/CharStreamException.hpp>
#include <antlr/CharStreamIOException.hpp>
#include <antlr/NoViableAltForCharException.hpp>

FMTLexer::FMTLexer(std::istream& in)
	: antlr::CharScanner(new antlr::CharBuffer(in),true)
{
	initLiterals();
}

FMTLexer::FMTLexer(antlr::InputBuffer& ib)
	: antlr::CharScanner(ib,true)
{
	initLiterals();
}

FMTLexer::FMTLexer(const antlr::LexerSharedInputState& state)
	: antlr::CharScanner(state,true)
{
	initLiterals();
}

void FMTLexer::initLiterals()
{
	literals["cyi"] = 62;
	literals["csi"] = 64;
	literals["cmi"] = 63;
	literals["tl"] = 32;
	literals["tr"] = 33;
	literals["cdi"] = 61;
	literals["cmoi"] = 60;
	literals["csf"] = 65;
}

antlr::RefToken FMTLexer::nextToken()
{
	antlr::RefToken theRetToken;
	for (;;) {
		antlr::RefToken theRetToken;
		int _ttype = antlr::Token::INVALID_TYPE;
		resetText();
		try {   // for lexical and char stream error handling
			switch ( LA(1)) {
			case 0x22 /* '"' */ :
			case 0x27 /* '\'' */ :
			{
				mSTRING(true);
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
			case 0x2f /* '/' */ :
			{
				mSLASH(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2c /* ',' */ :
			{
				mCOMMA(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x41 /* 'A' */ :
			case 0x61 /* 'a' */ :
			{
				mA(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x3a /* ':' */ :
			{
				mTERM(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x24 /* '$' */ :
			{
				mNONL(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x46 /* 'F' */ :
			case 0x66 /* 'f' */ :
			{
				mF(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x44 /* 'D' */ :
			case 0x64 /* 'd' */ :
			{
				mD(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x45 /* 'E' */ :
			case 0x65 /* 'e' */ :
			{
				mE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x47 /* 'G' */ :
			case 0x67 /* 'g' */ :
			{
				mG(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x49 /* 'I' */ :
			case 0x69 /* 'i' */ :
			{
				mI(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x4f /* 'O' */ :
			case 0x6f /* 'o' */ :
			{
				mO(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x5a /* 'Z' */ :
			{
				mZ(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x7a /* 'z' */ :
			{
				mZZ(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x51 /* 'Q' */ :
			case 0x71 /* 'q' */ :
			{
				mQ(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x48 /* 'H' */ :
			case 0x68 /* 'h' */ :
			{
				mH(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x54 /* 'T' */ :
			case 0x74 /* 't' */ :
			{
				mT(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x4c /* 'L' */ :
			case 0x6c /* 'l' */ :
			{
				mL(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x52 /* 'R' */ :
			case 0x72 /* 'r' */ :
			{
				mR(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x58 /* 'X' */ :
			case 0x78 /* 'x' */ :
			{
				mX(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x2e /* '.' */ :
			{
				mDOT(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x9 /* '\t' */ :
			case 0x20 /* ' ' */ :
			{
				mWHITESPACE(true);
				theRetToken=_returnToken;
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
				mNUMBER(true);
				theRetToken=_returnToken;
				break;
			}
			default:
				if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (LA(2) == 0x4d /* 'M' */ ) && (LA(3) == 0x4f /* 'O' */ )) {
					mCMOA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (LA(2) == 0x4d /* 'M' */ ) && (LA(3) == 0x6f /* 'o' */ )) {
					mCMoA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (LA(2) == 0x44 /* 'D' */ ) && (LA(3) == 0x57 /* 'W' */ )) {
					mCDWA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (LA(2) == 0x44 /* 'D' */ ) && (LA(3) == 0x77 /* 'w' */ )) {
					mCDwA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (LA(2) == 0x41 /* 'A' */ ) && (LA(3) == 0x50 /* 'P' */ )) {
					mCAPA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (LA(2) == 0x41 /* 'A' */ ) && (LA(3) == 0x70 /* 'p' */ )) {
					mCApA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x25 /* '%' */ ) && (LA(2) == 0x22 /* '"' */  || LA(2) == 0x27 /* '\'' */ )) {
					mCSTRING(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (LA(2) == 0x6d /* 'm' */ )) {
					mCmoA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (LA(2) == 0x68 /* 'h' */ )) {
					mCHI(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (LA(2) == 0x48 /* 'H' */ )) {
					mChI(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (LA(2) == 0x64 /* 'd' */ )) {
					mCdwA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (LA(2) == 0x61 /* 'a' */ )) {
					mCapA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */  || LA(1) == 0x63 /* 'c' */ ) && (true)) {
					mC(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x25 /* '%' */ ) && (true)) {
					mPERCENT(true);
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
			_ttype = testLiteralsTable(_ttype);
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

void FMTLexer::mSTRING(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = STRING;
	int _saveIndex;
	
	switch ( LA(1)) {
	case 0x22 /* '"' */ :
	{
		_saveIndex = text.length();
		match('"');
		text.erase(_saveIndex);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == 0x22 /* '"' */ ) && (LA(2) == 0x22 /* '"' */ )) {
				match('"');
				_saveIndex = text.length();
				match('"');
				text.erase(_saveIndex);
			}
			else if ((_tokenSet_0.member(LA(1)))) {
				{
				match(_tokenSet_0);
				}
			}
			else {
				goto _loop44;
			}
			
		}
		_loop44:;
		} // ( ... )*
		_saveIndex = text.length();
		match('"');
		text.erase(_saveIndex);
		break;
	}
	case 0x27 /* '\'' */ :
	{
		_saveIndex = text.length();
		match('\'');
		text.erase(_saveIndex);
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == 0x27 /* '\'' */ ) && (LA(2) == 0x27 /* '\'' */ )) {
				match('\'');
				_saveIndex = text.length();
				match('\'');
				text.erase(_saveIndex);
			}
			else if ((_tokenSet_1.member(LA(1)))) {
				{
				match(_tokenSet_1);
				}
			}
			else {
				goto _loop47;
			}
			
		}
		_loop47:;
		} // ( ... )*
		_saveIndex = text.length();
		match('\'');
		text.erase(_saveIndex);
		break;
	}
	default:
	{
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCSTRING(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CSTRING;
	int _saveIndex;
	
	if ((LA(1) == 0x25 /* '%' */ ) && (LA(2) == 0x22 /* '"' */ )) {
		_saveIndex = text.length();
		match('%');
		text.erase(_saveIndex);
		_saveIndex = text.length();
		match('"');
		text.erase(_saveIndex);
		cLexer->DoubleQuotes( true); selector->push( cLexer); selector->retry();
	}
	else if ((LA(1) == 0x25 /* '%' */ ) && (LA(2) == 0x27 /* '\'' */ )) {
		_saveIndex = text.length();
		match('%');
		text.erase(_saveIndex);
		_saveIndex = text.length();
		match('\'');
		text.erase(_saveIndex);
		cLexer->DoubleQuotes( false); selector->push( cLexer); selector->retry();
	}
	else {
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mLBRACE(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = LBRACE;
	int _saveIndex;
	
	match('(');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mRBRACE(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = RBRACE;
	int _saveIndex;
	
	match(')');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mSLASH(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = SLASH;
	int _saveIndex;
	
	match('/');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCOMMA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = COMMA;
	int _saveIndex;
	
	match(',');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = A;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x41 /* 'A' */ :
	{
		match('A');
		break;
	}
	case 0x61 /* 'a' */ :
	{
		match('a');
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

void FMTLexer::mTERM(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = TERM;
	int _saveIndex;
	
	match(':');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mNONL(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = NONL;
	int _saveIndex;
	
	match('$');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mF(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = F;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x66 /* 'f' */ :
	{
		match('f');
		break;
	}
	case 0x46 /* 'F' */ :
	{
		match('F');
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

void FMTLexer::mD(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = D;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x64 /* 'd' */ :
	{
		match('d');
		break;
	}
	case 0x44 /* 'D' */ :
	{
		match('D');
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

void FMTLexer::mE(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = E;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x65 /* 'e' */ :
	{
		match('e');
		break;
	}
	case 0x45 /* 'E' */ :
	{
		match('E');
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

void FMTLexer::mG(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = G;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x67 /* 'g' */ :
	{
		match('g');
		break;
	}
	case 0x47 /* 'G' */ :
	{
		match('G');
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

void FMTLexer::mI(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = I;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x69 /* 'i' */ :
	{
		match('i');
		break;
	}
	case 0x49 /* 'I' */ :
	{
		match('I');
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

void FMTLexer::mO(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = O;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x6f /* 'o' */ :
	{
		match('o');
		break;
	}
	case 0x4f /* 'O' */ :
	{
		match('O');
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

void FMTLexer::mZ(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = Z;
	int _saveIndex;
	
	{
	match('Z');
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mZZ(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = ZZ;
	int _saveIndex;
	
	{
	match('z');
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = Q;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x71 /* 'q' */ :
	{
		match('q');
		break;
	}
	case 0x51 /* 'Q' */ :
	{
		match('Q');
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

void FMTLexer::mH(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = H;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x68 /* 'h' */ :
	{
		match('h');
		break;
	}
	case 0x48 /* 'H' */ :
	{
		match('H');
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

void FMTLexer::mT(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = T;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x74 /* 't' */ :
	{
		match('t');
		break;
	}
	case 0x54 /* 'T' */ :
	{
		match('T');
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

void FMTLexer::mL(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = L;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x6c /* 'l' */ :
	{
		match('l');
		break;
	}
	case 0x4c /* 'L' */ :
	{
		match('L');
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

void FMTLexer::mR(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = R;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x72 /* 'r' */ :
	{
		match('r');
		break;
	}
	case 0x52 /* 'R' */ :
	{
		match('R');
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

void FMTLexer::mX(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = X;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x78 /* 'x' */ :
	{
		match('x');
		break;
	}
	case 0x58 /* 'X' */ :
	{
		match('X');
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

void FMTLexer::mC(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = C;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x63 /* 'c' */ :
	{
		match('c');
		break;
	}
	case 0x43 /* 'C' */ :
	{
		match('C');
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

void FMTLexer::mCMOA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CMOA;
	int _saveIndex;
	
	{
	mC(false);
	match('M');
	match('O');
	mA(false);
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCMoA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CMoA;
	int _saveIndex;
	
	{
	mC(false);
	match('M');
	match('o');
	mA(false);
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCmoA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CmoA;
	int _saveIndex;
	
	{
	mC(false);
	match('m');
	match('o');
	mA(false);
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCHI(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CHI;
	int _saveIndex;
	
	{
	mC(false);
	match('h');
	mI(false);
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mChI(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = ChI;
	int _saveIndex;
	
	{
	mC(false);
	match('H');
	mI(false);
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCDWA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CDWA;
	int _saveIndex;
	
	{
	mC(false);
	match('D');
	match('W');
	mA(false);
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCDwA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CDwA;
	int _saveIndex;
	
	{
	mC(false);
	match('D');
	match('w');
	mA(false);
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCdwA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CdwA;
	int _saveIndex;
	
	{
	mC(false);
	match('d');
	match('w');
	mA(false);
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCAPA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CAPA;
	int _saveIndex;
	
	{
	mC(false);
	match('A');
	match('P');
	mA(false);
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCApA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CApA;
	int _saveIndex;
	
	{
	mC(false);
	match('A');
	match('p');
	mA(false);
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCapA(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CapA;
	int _saveIndex;
	
	{
	mC(false);
	match('a');
	match('p');
	mA(false);
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mPERCENT(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = PERCENT;
	int _saveIndex;
	
	match('%');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mDOT(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = DOT;
	int _saveIndex;
	
	match('.');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mW(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = W;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x20 /* ' ' */ :
	{
		match(' ');
		break;
	}
	case 0x9 /* '\t' */ :
	{
		match('\t');
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

void FMTLexer::mWHITESPACE(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = WHITESPACE;
	int _saveIndex;
	
	{ // ( ... )+
	int _cnt115=0;
	for (;;) {
		if ((LA(1) == 0x9 /* '\t' */  || LA(1) == 0x20 /* ' ' */ )) {
			mW(false);
		}
		else {
			if ( _cnt115>=1 ) { goto _loop115; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt115++;
	}
	_loop115:;
	}  // ( ... )+
	_ttype=antlr::Token::SKIP;
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mDIGITS(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = DIGITS;
	int _saveIndex;
	
	{ // ( ... )+
	int _cnt118=0;
	for (;;) {
		if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
			matchRange('0','9');
		}
		else {
			if ( _cnt118>=1 ) { goto _loop118; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt118++;
	}
	_loop118:;
	}  // ( ... )+
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCHAR(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CHAR;
	int _saveIndex;
	
	{
	matchRange('\3',static_cast<unsigned char>(255));
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mNUMBER(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = NUMBER;
	int _saveIndex;
	antlr::RefToken num;
	
	SizeT n;
	SizeT i = 0; 
	
	
	mDIGITS(true);
	num=_returnToken;
	{
	if ((LA(1) == 0x48 /* 'H' */ )) {
		
		_ttype = STRING; 
		std::istringstream s(num->getText());
		s >> n;
		
		_saveIndex = text.length();
		match('H');
		text.erase(_saveIndex);
		{ // ( ... )+
		int _cnt124=0;
		for (;;) {
			// init action gets executed even in guessing mode
			if( i == n )
			break;
			i++; // count chars here so that guessing mode works
			
			if (((LA(1) >= 0x3 /* '\3' */  && LA(1) <= 0xff))) {
				mCHAR(false);
			}
			else {
				if ( _cnt124>=1 ) { goto _loop124; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			
			_cnt124++;
		}
		_loop124:;
		}  // ( ... )+
	}
	else {
	}
	
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}


const unsigned long FMTLexer::_tokenSet_0_data_[] = { 4294967288UL, 4294967291UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! # $ 
// % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F 
// G H I J K 
const antlr::BitSet FMTLexer::_tokenSet_0(_tokenSet_0_data_,16);
const unsigned long FMTLexer::_tokenSet_1_data_[] = { 4294967288UL, 4294967167UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! " # 
// $ % & ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F 
// G H I J K 
const antlr::BitSet FMTLexer::_tokenSet_1(_tokenSet_1_data_,16);

