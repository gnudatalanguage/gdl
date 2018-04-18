/* $ANTLR 2.7.7 (2006-11-01): "format.g" -> "FMTLexer.cpp"$ */

#include "includefirst.hpp"

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
	literals["tl"] = 35;
	literals["tr"] = 36;
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
			case 0x22 /* '\"' */ :
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
			{
				mE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x65 /* 'e' */ :
			{
				mSE(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x47 /* 'G' */ :
			{
				mG(true);
				theRetToken=_returnToken;
				break;
			}
			case 0x67 /* 'g' */ :
			{
				mSG(true);
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
			case 0x42 /* 'B' */ :
			case 0x62 /* 'b' */ :
			{
				mB(true);
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
			default:
				if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x4d /* 'M' */ ) && (LA(3) == 0x4f /* 'O' */ ) && (LA(4) == 0x41 /* 'A' */ )) {
					mCMOA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x4d /* 'M' */ ) && (LA(3) == 0x4f /* 'O' */ ) && (LA(4) == 0x49 /* 'I' */ )) {
					mCMOI(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x4d /* 'M' */ ) && (LA(3) == 0x6f /* 'o' */ )) {
					mCMoA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x44 /* 'D' */ ) && (LA(3) == 0x49 /* 'I' */ )) {
					mCDI(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x4d /* 'M' */ ) && (LA(3) == 0x49 /* 'I' */ )) {
					mCMI(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x53 /* 'S' */ ) && (LA(3) == 0x49 /* 'I' */ )) {
					mCSI(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x53 /* 'S' */ ) && (LA(3) == 0x46 /* 'F' */ )) {
					mCSF(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x44 /* 'D' */ ) && (LA(3) == 0x57 /* 'W' */ )) {
					mCDWA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x44 /* 'D' */ ) && (LA(3) == 0x77 /* 'w' */ )) {
					mCDwA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x41 /* 'A' */ ) && (LA(3) == 0x50 /* 'P' */ )) {
					mCAPA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x41 /* 'A' */ ) && (LA(3) == 0x70 /* 'p' */ )) {
					mCApA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x25 /* '%' */ ) && (LA(2) == 0x22 /* '\"' */  || LA(2) == 0x27 /* '\'' */ )) {
					mCSTRING(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x6d /* 'm' */ )) {
					mCmoA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x59 /* 'Y' */ )) {
					mCYI(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x48 /* 'H' */ )) {
					mCHI(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x68 /* 'h' */ )) {
					mChI(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x64 /* 'd' */ )) {
					mCdwA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x43 /* 'C' */ ) && (LA(2) == 0x61 /* 'a' */ )) {
					mCapA(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2b /* '+' */ ) && (LA(2) == 0x2d /* '-' */ )) {
					mPM(true);
					theRetToken=_returnToken;
				}
				else if ((LA(1) == 0x2d /* '-' */ ) && (LA(2) == 0x2b /* '+' */ )) {
					mMP(true);
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
				else if ((_tokenSet_0.member(LA(1))) && (true)) {
					mNUMBER(true);
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = STRING;
	std::string::size_type _saveIndex;
	
	switch ( LA(1)) {
	case 0x22 /* '\"' */ :
	{
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
			else if ((_tokenSet_1.member(LA(1)))) {
				{
				match(_tokenSet_1);
				}
			}
			else {
				goto _loop54;
			}
			
		}
		_loop54:;
		} // ( ... )*
		_saveIndex = text.length();
		match('\"' /* charlit */ );
		text.erase(_saveIndex);
		break;
	}
	case 0x27 /* '\'' */ :
	{
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
			else if ((_tokenSet_2.member(LA(1)))) {
				{
				match(_tokenSet_2);
				}
			}
			else {
				goto _loop57;
			}
			
		}
		_loop57:;
		} // ( ... )*
		_saveIndex = text.length();
		match('\'' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CSTRING;
	std::string::size_type _saveIndex;
	
	if ((LA(1) == 0x25 /* '%' */ ) && (LA(2) == 0x22 /* '\"' */ )) {
		_saveIndex = text.length();
		match('%' /* charlit */ );
		text.erase(_saveIndex);
		_saveIndex = text.length();
		match('\"' /* charlit */ );
		text.erase(_saveIndex);
		cLexer->DoubleQuotes( true); selector->push( cLexer); selector->retry();
	}
	else if ((LA(1) == 0x25 /* '%' */ ) && (LA(2) == 0x27 /* '\'' */ )) {
		_saveIndex = text.length();
		match('%' /* charlit */ );
		text.erase(_saveIndex);
		_saveIndex = text.length();
		match('\'' /* charlit */ );
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

void FMTLexer::mRBRACE(bool _createToken) {
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

void FMTLexer::mSLASH(bool _createToken) {
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

void FMTLexer::mCOMMA(bool _createToken) {
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

void FMTLexer::mA(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = A;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x41 /* 'A' */ :
	{
		match('A' /* charlit */ );
		break;
	}
	case 0x61 /* 'a' */ :
	{
		match('a' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = TERM;
	std::string::size_type _saveIndex;
	
	match(':' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mNONL(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = NONL;
	std::string::size_type _saveIndex;
	
	match('$' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mF(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = F;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x66 /* 'f' */ :
	{
		match('f' /* charlit */ );
		break;
	}
	case 0x46 /* 'F' */ :
	{
		match('F' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = D;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x64 /* 'd' */ :
	{
		match('d' /* charlit */ );
		break;
	}
	case 0x44 /* 'D' */ :
	{
		match('D' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = E;
	std::string::size_type _saveIndex;
	
	{
	match('E' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mSE(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = SE;
	std::string::size_type _saveIndex;
	
	{
	match('e' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mG(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = G;
	std::string::size_type _saveIndex;
	
	{
	match('G' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mSG(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = SG;
	std::string::size_type _saveIndex;
	
	{
	match('g' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mI(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = I;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x69 /* 'i' */ :
	{
		match('i' /* charlit */ );
		break;
	}
	case 0x49 /* 'I' */ :
	{
		match('I' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = O;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x6f /* 'o' */ :
	{
		match('o' /* charlit */ );
		break;
	}
	case 0x4f /* 'O' */ :
	{
		match('O' /* charlit */ );
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

void FMTLexer::mB(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = B;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x62 /* 'b' */ :
	{
		match('b' /* charlit */ );
		break;
	}
	case 0x42 /* 'B' */ :
	{
		match('B' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = Z;
	std::string::size_type _saveIndex;
	
	{
	match('Z' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mZZ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = ZZ;
	std::string::size_type _saveIndex;
	
	{
	match('z' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mQ(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = Q;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x71 /* 'q' */ :
	{
		match('q' /* charlit */ );
		break;
	}
	case 0x51 /* 'Q' */ :
	{
		match('Q' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = H;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x68 /* 'h' */ :
	{
		match('h' /* charlit */ );
		break;
	}
	case 0x48 /* 'H' */ :
	{
		match('H' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = T;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x74 /* 't' */ :
	{
		match('t' /* charlit */ );
		break;
	}
	case 0x54 /* 'T' */ :
	{
		match('T' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = L;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x6c /* 'l' */ :
	{
		match('l' /* charlit */ );
		break;
	}
	case 0x4c /* 'L' */ :
	{
		match('L' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = R;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x72 /* 'r' */ :
	{
		match('r' /* charlit */ );
		break;
	}
	case 0x52 /* 'R' */ :
	{
		match('R' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = X;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x78 /* 'x' */ :
	{
		match('x' /* charlit */ );
		break;
	}
	case 0x58 /* 'X' */ :
	{
		match('X' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = C;
	std::string::size_type _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x63 /* 'c' */ :
	{
		match('c' /* charlit */ );
		break;
	}
	case 0x43 /* 'C' */ :
	{
		match('C' /* charlit */ );
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CMOA;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('M' /* charlit */ );
	match('O' /* charlit */ );
	match('A' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCMoA(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CMoA;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('M' /* charlit */ );
	match('o' /* charlit */ );
	match('A' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCmoA(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CmoA;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('m' /* charlit */ );
	match('o' /* charlit */ );
	match('A' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCMOI(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CMOI;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('M' /* charlit */ );
	match('O' /* charlit */ );
	match('I' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCDI(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CDI;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('D' /* charlit */ );
	match('I' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCMI(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CMI;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('M' /* charlit */ );
	match('I' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCYI(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CYI;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('Y' /* charlit */ );
	match('I' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCSI(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CSI;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('S' /* charlit */ );
	match('I' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCSF(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CSF;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('S' /* charlit */ );
	match('F' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCHI(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CHI;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('H' /* charlit */ );
	match('I' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mChI(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = ChI;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('h' /* charlit */ );
	match('I' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCDWA(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CDWA;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('D' /* charlit */ );
	match('W' /* charlit */ );
	match('A' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCDwA(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CDwA;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('D' /* charlit */ );
	match('w' /* charlit */ );
	match('A' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCdwA(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CdwA;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('d' /* charlit */ );
	match('w' /* charlit */ );
	match('A' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCAPA(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CAPA;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('A' /* charlit */ );
	match('P' /* charlit */ );
	match('A' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCApA(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CApA;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('A' /* charlit */ );
	match('p' /* charlit */ );
	match('A' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCapA(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CapA;
	std::string::size_type _saveIndex;
	
	{
	match('C' /* charlit */ );
	match('a' /* charlit */ );
	match('p' /* charlit */ );
	match('A' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mPERCENT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = PERCENT;
	std::string::size_type _saveIndex;
	
	match('%' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mDOT(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = DOT;
	std::string::size_type _saveIndex;
	
	match('.' /* charlit */ );
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mPM(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = PM;
	std::string::size_type _saveIndex;
	
	{
	match('+' /* charlit */ );
	match('-' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mMP(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = MP;
	std::string::size_type _saveIndex;
	
	{
	match('-' /* charlit */ );
	match('+' /* charlit */ );
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mW(bool _createToken) {
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = WHITESPACE;
	std::string::size_type _saveIndex;
	
	{ // ( ... )+
	int _cnt147=0;
	for (;;) {
		if ((LA(1) == 0x9 /* '\t' */  || LA(1) == 0x20 /* ' ' */ )) {
			mW(false);
		}
		else {
			if ( _cnt147>=1 ) { goto _loop147; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt147++;
	}
	_loop147:;
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
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = DIGITS;
	std::string::size_type _saveIndex;
	
	{ // ( ... )+
	int _cnt150=0;
	for (;;) {
		if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
			matchRange('0','9');
		}
		else {
			if ( _cnt150>=1 ) { goto _loop150; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt150++;
	}
	_loop150:;
	}  // ( ... )+
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mCHAR(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = CHAR;
	std::string::size_type _saveIndex;
	
	{
	matchRange('\3',static_cast<unsigned char>('\377'));
	}
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void FMTLexer::mNUMBER(bool _createToken) {
	int _ttype; antlr::RefToken _token; std::string::size_type _begin = text.length();
	_ttype = NUMBER;
	std::string::size_type _saveIndex;
	antlr::RefToken num;
	
	SizeT n;
	SizeT i = 0;
	bool uPlus = false;
	bool uMinus = false;
	
	
	{
	switch ( LA(1)) {
	case 0x2d /* '-' */ :
	{
		match('-' /* charlit */ );
		
		uMinus = true;
		
		break;
	}
	case 0x2b /* '+' */ :
	{
		match('+' /* charlit */ );
		
		uPlus = true;
		
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
	mDIGITS(true);
	num=_returnToken;
	
	if( uMinus) num->setText( "-" + num->getText());
	if( uPlus) num->setText( "+" + num->getText());
	
	{
	if ((LA(1) == 0x48 /* 'H' */ )) {
		
		_ttype = STRING; 
		std::istringstream s(num->getText());
		s >> n;
		{ text.erase(_begin); text += ""; }; // clear string (remove number)
		
		_saveIndex = text.length();
		match('H' /* charlit */ );
		text.erase(_saveIndex);
		{ // ( ... )+
		int _cnt157=0;
		for (;;) {
			// init action gets executed even in guessing mode
			if( i == n )
			break;
			i++; // count chars here so that guessing mode works
			
			if (((LA(1) >= 0x3 /* '\3' */  && LA(1) <= 0xff))) {
				mCHAR(false);
			}
			else {
				if ( _cnt157>=1 ) { goto _loop157; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
			}
			
			_cnt157++;
		}
		_loop157:;
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


const unsigned long FMTLexer::_tokenSet_0_data_[] = { 0UL, 67053568UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// + - 0 1 2 3 4 5 6 7 8 9 
const antlr::BitSet FMTLexer::_tokenSet_0(_tokenSet_0_data_,10);
const unsigned long FMTLexer::_tokenSet_1_data_[] = { 4294967288UL, 4294967291UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! # $ 
// % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F 
// G H I J K L M N O P Q R S 
const antlr::BitSet FMTLexer::_tokenSet_1(_tokenSet_1_data_,16);
const unsigned long FMTLexer::_tokenSet_2_data_[] = { 4294967288UL, 4294967167UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f   ! \" # 
// $ % & ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F 
// G H I J K L M N O P Q R S 
const antlr::BitSet FMTLexer::_tokenSet_2(_tokenSet_2_data_,16);

