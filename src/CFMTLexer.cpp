/* $ANTLR 2.7.4: "cformat.g" -> "CFMTLexer.cpp"$ */
#include "CFMTLexer.hpp"
#include <antlr/CharBuffer.hpp>
#include <antlr/TokenStreamException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/TokenStreamRecognitionException.hpp>
#include <antlr/CharStreamException.hpp>
#include <antlr/CharStreamIOException.hpp>
#include <antlr/NoViableAltForCharException.hpp>

CFMTLexer::CFMTLexer(std::istream& in)
	: antlr::CharScanner(new antlr::CharBuffer(in),true)
{
	initLiterals();
}

CFMTLexer::CFMTLexer(antlr::InputBuffer& ib)
	: antlr::CharScanner(ib,true)
{
	initLiterals();
}

CFMTLexer::CFMTLexer(const antlr::LexerSharedInputState& state)
	: antlr::CharScanner(state,true)
{
	initLiterals();
}

void CFMTLexer::initLiterals()
{
}

antlr::RefToken CFMTLexer::nextToken()
{
	antlr::RefToken theRetToken;
	for (;;) {
		antlr::RefToken theRetToken;
		int _ttype = antlr::Token::INVALID_TYPE;
		resetText();
		try {   // for lexical and char stream error handling
			if (((LA(1) >= 0x3 /* '\3' */  && LA(1) <= 0xff))) {
				mALL(true);
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

void CFMTLexer::mALL(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = ALL;
	int _saveIndex;
	
	if (((_tokenSet_0.member(LA(1))) && (true))&&( format)) {
		{
		switch ( LA(1)) {
		case 0x44 /* 'D' */ :
		case 0x45 /* 'E' */ :
		case 0x46 /* 'F' */ :
		case 0x47 /* 'G' */ :
		case 0x49 /* 'I' */ :
		case 0x4f /* 'O' */ :
		case 0x53 /* 'S' */ :
		case 0x58 /* 'X' */ :
		case 0x5a /* 'Z' */ :
		case 0x64 /* 'd' */ :
		case 0x65 /* 'e' */ :
		case 0x66 /* 'f' */ :
		case 0x67 /* 'g' */ :
		case 0x69 /* 'i' */ :
		case 0x6f /* 'o' */ :
		case 0x73 /* 's' */ :
		case 0x78 /* 'x' */ :
		case 0x7a /* 'z' */ :
		{
			{
			{
			switch ( LA(1)) {
			case 0x44 /* 'D' */ :
			case 0x64 /* 'd' */ :
			{
				mCD(false);
				_ttype = CD;
				break;
			}
			case 0x45 /* 'E' */ :
			case 0x65 /* 'e' */ :
			{
				mCE(false);
				_ttype = CE;
				break;
			}
			case 0x49 /* 'I' */ :
			case 0x69 /* 'i' */ :
			{
				mCI(false);
				_ttype = CI;
				break;
			}
			case 0x46 /* 'F' */ :
			case 0x66 /* 'f' */ :
			{
				mCF(false);
				_ttype = CF;
				break;
			}
			case 0x47 /* 'G' */ :
			case 0x67 /* 'g' */ :
			{
				mCG(false);
				_ttype = CG;
				break;
			}
			case 0x4f /* 'O' */ :
			case 0x6f /* 'o' */ :
			{
				mCO(false);
				_ttype = CO;
				break;
			}
			case 0x53 /* 'S' */ :
			case 0x73 /* 's' */ :
			{
				mCS(false);
				_ttype = CS;
				break;
			}
			case 0x58 /* 'X' */ :
			case 0x78 /* 'x' */ :
			{
				mCX(false);
				_ttype = CX;
				break;
			}
			case 0x5a /* 'Z' */ :
			case 0x7a /* 'z' */ :
			{
				mCZ(false);
				_ttype = CZ;
				break;
			}
			default:
			{
				throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
			}
			}
			}
			format = false;
			}
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
			mCNUMBER(false);
			_ttype = CNUMBER;
			break;
		}
		case 0x2e /* '.' */ :
		{
			mCDOT(false);
			_ttype = CDOT;
			break;
		}
		case 0x9 /* '\t' */ :
		case 0x20 /* ' ' */ :
		{
			mCWS(false);
			_ttype=antlr::Token::SKIP;
			break;
		}
		default:
		{
			throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
		}
	}
	else if (((LA(1) >= 0x3 /* '\3' */  && LA(1) <= 0xff)) && (true)) {
		mCSTR(false);
		_ttype = CSTR;
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

void CFMTLexer::mCD(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CD;
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

void CFMTLexer::mCE(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CE;
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

void CFMTLexer::mCI(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CI;
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

void CFMTLexer::mCF(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CF;
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

void CFMTLexer::mCG(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CG;
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

void CFMTLexer::mCO(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CO;
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

void CFMTLexer::mCS(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CS;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x73 /* 's' */ :
	{
		match('s');
		break;
	}
	case 0x53 /* 'S' */ :
	{
		match('S');
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

void CFMTLexer::mCX(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CX;
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

void CFMTLexer::mCZ(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CZ;
	int _saveIndex;
	
	{
	switch ( LA(1)) {
	case 0x7a /* 'z' */ :
	{
		match('z');
		break;
	}
	case 0x5a /* 'Z' */ :
	{
		match('Z');
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

void CFMTLexer::mCNUMBER(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CNUMBER;
	int _saveIndex;
	
	mDIGITS(false);
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CFMTLexer::mCDOT(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CDOT;
	int _saveIndex;
	
	match('.');
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CFMTLexer::mCWS(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CWS;
	int _saveIndex;
	
	{ // ( ... )+
	int _cnt63=0;
	for (;;) {
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
			if ( _cnt63>=1 ) { goto _loop63; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		}
		_cnt63++;
	}
	_loop63:;
	}  // ( ... )+
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CFMTLexer::mCSTR(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CSTR;
	int _saveIndex;
	
	if ((((LA(1) >= 0x3 /* '\3' */  && LA(1) <= 0xff)) && (true))&&( doubleQuotes)) {
		{
		mCSTR1(false);
		{
		switch ( LA(1)) {
		case 0x22 /* '"' */ :
		{
			_saveIndex = text.length();
			match('"');
			text.erase(_saveIndex);
			selector->pop();
			break;
		}
		case 0x25 /* '%' */ :
		{
			_saveIndex = text.length();
			match('%');
			text.erase(_saveIndex);
			format = true;
			break;
		}
		default:
		{
			throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
		}
		}
	}
	else if (((LA(1) >= 0x3 /* '\3' */  && LA(1) <= 0xff)) && (true)) {
		{
		mCSTR2(false);
		{
		switch ( LA(1)) {
		case 0x27 /* '\'' */ :
		{
			_saveIndex = text.length();
			match('\'');
			text.erase(_saveIndex);
			selector->pop();
			break;
		}
		case 0x25 /* '%' */ :
		{
			_saveIndex = text.length();
			match('%');
			text.erase(_saveIndex);
			format = true;
			break;
		}
		default:
		{
			throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
		}
		}
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

void CFMTLexer::mCSTR1(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CSTR1;
	int _saveIndex;
	
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == 0x25 /* '%' */ ) && (LA(2) == 0x25 /* '%' */ )) {
			_saveIndex = text.length();
			match('%');
			text.erase(_saveIndex);
			match('%');
		}
		else if ((LA(1) == 0x5c /* '\\' */ )) {
			mESC(false);
		}
		else if ((_tokenSet_1.member(LA(1)))) {
			{
			match(_tokenSet_1);
			}
		}
		else {
			goto _loop13;
		}
		
	}
	_loop13:;
	} // ( ... )*
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CFMTLexer::mCSTR2(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = CSTR2;
	int _saveIndex;
	
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == 0x25 /* '%' */ ) && (LA(2) == 0x25 /* '%' */ )) {
			_saveIndex = text.length();
			match('%');
			text.erase(_saveIndex);
			match('%');
		}
		else if ((LA(1) == 0x5c /* '\\' */ )) {
			mESC(false);
		}
		else if ((_tokenSet_2.member(LA(1)))) {
			{
			match(_tokenSet_2);
			}
		}
		else {
			goto _loop17;
		}
		
	}
	_loop17:;
	} // ( ... )*
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CFMTLexer::mESC(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = ESC;
	int _saveIndex;
	
	_saveIndex = text.length();
	match('\\');
	text.erase(_saveIndex);
	{
	switch ( LA(1)) {
	case 0x41 /* 'A' */ :
	case 0x61 /* 'a' */ :
	{
		{
		switch ( LA(1)) {
		case 0x61 /* 'a' */ :
		{
			match('a');
			break;
		}
		case 0x41 /* 'A' */ :
		{
			match('A');
			break;
		}
		default:
		{
			throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
		}
		{ text.erase(_begin); text +=  "\7"; };
		break;
	}
	case 0x42 /* 'B' */ :
	case 0x62 /* 'b' */ :
	{
		{
		switch ( LA(1)) {
		case 0x62 /* 'b' */ :
		{
			match('b');
			break;
		}
		case 0x42 /* 'B' */ :
		{
			match('B');
			break;
		}
		default:
		{
			throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
		}
		{ text.erase(_begin); text +=  "\b"; };
		break;
	}
	case 0x46 /* 'F' */ :
	case 0x66 /* 'f' */ :
	{
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
		{ text.erase(_begin); text +=  "\f"; };
		break;
	}
	case 0x4e /* 'N' */ :
	case 0x6e /* 'n' */ :
	{
		{
		switch ( LA(1)) {
		case 0x6e /* 'n' */ :
		{
			match('n');
			break;
		}
		case 0x4e /* 'N' */ :
		{
			match('N');
			break;
		}
		default:
		{
			throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
		}
		{ text.erase(_begin); text +=  "\n"; };
		break;
	}
	case 0x52 /* 'R' */ :
	case 0x72 /* 'r' */ :
	{
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
		{ text.erase(_begin); text +=  "\r"; };
		break;
	}
	case 0x54 /* 'T' */ :
	case 0x74 /* 't' */ :
	{
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
		{ text.erase(_begin); text +=  "\t"; };
		break;
	}
	case 0x56 /* 'V' */ :
	case 0x76 /* 'v' */ :
	{
		{
		switch ( LA(1)) {
		case 0x76 /* 'v' */ :
		{
			match('v');
			break;
		}
		case 0x56 /* 'V' */ :
		{
			match('V');
			break;
		}
		default:
		{
			throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		}
		}
		{ text.erase(_begin); text +=  "\13"; };
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
	{
		mOCTESC(false);
		break;
	}
	case 0x58 /* 'X' */ :
	case 0x78 /* 'x' */ :
	{
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
		mHEXESC(false);
		break;
	}
	default:
		if ((_tokenSet_3.member(LA(1)))) {
			{
			match(_tokenSet_3);
			}
		}
	else {
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

void CFMTLexer::mOCTESC(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = OCTESC;
	int _saveIndex;
	
	mODIGIT(false);
	{
	if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ )) && ((LA(2) >= 0x3 /* '\3' */  && LA(2) <= 0xff))) {
		mODIGIT(false);
		{
		if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x37 /* '7' */ )) && ((LA(2) >= 0x3 /* '\3' */  && LA(2) <= 0xff))) {
			mODIGIT(false);
		}
		else if (((LA(1) >= 0x3 /* '\3' */  && LA(1) <= 0xff)) && (true)) {
		}
		else {
			throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
		}
		
		}
	}
	else if (((LA(1) >= 0x3 /* '\3' */  && LA(1) <= 0xff)) && (true)) {
	}
	else {
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	
	}
	
	std::string s = text.substr(_begin,text.length()-_begin);
	char c = static_cast<char>(strtoul(s.c_str(),NULL,8));
	{ text.erase(_begin); text +=  c; };
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CFMTLexer::mHEXESC(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = HEXESC;
	int _saveIndex;
	
	mHDIGIT(false);
	{
	if ((_tokenSet_4.member(LA(1))) && ((LA(2) >= 0x3 /* '\3' */  && LA(2) <= 0xff))) {
		mHDIGIT(false);
	}
	else if (((LA(1) >= 0x3 /* '\3' */  && LA(1) <= 0xff)) && (true)) {
	}
	else {
		throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());
	}
	
	}
	
	std::string s = text.substr(_begin,text.length()-_begin);
	char c = static_cast<char>(strtoul(s.c_str(),NULL,16));
	{ text.erase(_begin); text +=  c; };
	
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}

void CFMTLexer::mODIGIT(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = ODIGIT;
	int _saveIndex;
	
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

void CFMTLexer::mHDIGIT(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = HDIGIT;
	int _saveIndex;
	
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
		matchRange('0','9');
		break;
	}
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
	case 0x41 /* 'A' */ :
	case 0x42 /* 'B' */ :
	case 0x43 /* 'C' */ :
	case 0x44 /* 'D' */ :
	case 0x45 /* 'E' */ :
	case 0x46 /* 'F' */ :
	{
		matchRange('A','F');
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

void CFMTLexer::mDIGITS(bool _createToken) {
	int _ttype; antlr::RefToken _token; int _begin=text.length();
	_ttype = DIGITS;
	int _saveIndex;
	
	{ // ( ... )+
	int _cnt59=0;
	for (;;) {
		if (((LA(1) >= 0x30 /* '0' */  && LA(1) <= 0x39 /* '9' */ ))) {
			matchRange('0','9');
		}
		else {
			if ( _cnt59>=1 ) { goto _loop59; } else {throw antlr::NoViableAltForCharException(LA(1), getFilename(), getLine(), getColumn());}
		}
		
		_cnt59++;
	}
	_loop59:;
	}  // ( ... )+
	if ( _createToken && _token==antlr::nullToken && _ttype!=antlr::Token::SKIP ) {
	   _token = makeToken(_ttype);
	   _token->setText(text.substr(_begin, text.length()-_begin));
	}
	_returnToken = _token;
	_saveIndex=0;
}


const unsigned long CFMTLexer::_tokenSet_0_data_[] = { 512UL, 67059713UL, 84443888UL, 84443888UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x9 
const antlr::BitSet CFMTLexer::_tokenSet_0(_tokenSet_0_data_,10);
const unsigned long CFMTLexer::_tokenSet_1_data_[] = { 4294967288UL, 4294967259UL, 4026531839UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 
const antlr::BitSet CFMTLexer::_tokenSet_1(_tokenSet_1_data_,16);
const unsigned long CFMTLexer::_tokenSet_2_data_[] = { 4294967288UL, 4294967135UL, 4026531839UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 
const antlr::BitSet CFMTLexer::_tokenSet_2(_tokenSet_2_data_,16);
const unsigned long CFMTLexer::_tokenSet_3_data_[] = { 4294967288UL, 4278255615UL, 4272668601UL, 4272668601UL, 4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf 0x10 0x11 0x12 0x13 
// 0x14 0x15 0x16 0x17 0x18 0x19 
const antlr::BitSet CFMTLexer::_tokenSet_3(_tokenSet_3_data_,16);
const unsigned long CFMTLexer::_tokenSet_4_data_[] = { 0UL, 67043328UL, 126UL, 126UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
const antlr::BitSet CFMTLexer::_tokenSet_4(_tokenSet_4_data_,10);

