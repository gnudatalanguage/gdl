/* $ANTLR 2.7.4: "format.in.g" -> "FMTIn.cpp"$ */
#include "FMTIn.hpp"
#include <antlr/Token.hpp>
#include <antlr/AST.hpp>
#include <antlr/NoViableAltException.hpp>
#include <antlr/MismatchedTokenException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/BitSet.hpp>

    // gets inserted after the antlr generated includes in the cpp file

FMTIn::FMTIn()
	: antlr::TreeParser() {
}

void FMTIn::format(RefFMTNode _t) {
	RefFMTNode format_AST_in = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode fmt = RefFMTNode(antlr::nullAST);
	
	RefFMTNode __t2 = _t;
	fmt = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),FORMAT);
	_t = _t->getFirstChild();
	goto realCode;
	q(_t);
	_t = _retTree;
	{ // ( ... )+
	int _cnt4=0;
	for (;;) {
		if (_t == RefFMTNode(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			f(_t);
			_t = _retTree;
			q(_t);
			_t = _retTree;
		}
		else {
			if ( _cnt4>=1 ) { goto _loop4; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt4++;
	}
	_loop4:;
	}  // ( ... )+
	
	realCode:
	
	reversionAnker = fmt;
	
	RefFMTNode blk = _t; // q (f q)+
	
	// as later format_recursive is used, this loop only
	// loops once (ie. could be eliminated - left here in
	// case of later changes)
	for( int r = fmt->getRep(); r > 0; r--)
	{
	GetLine(); 
	
	q( blk);
	_t = _retTree;
	
	for (;;) 
	{
	if( _t == static_cast<RefFMTNode>(antlr::nullAST))
	_t = ASTNULL;
	
	switch ( _t->getType()) {
	case FORMAT:
	case STRING:
	case CSTRING:
	case TL:
	case TR:
	case TERM:
	case NONL:
	case Q: case T: case X: case A:
	case F: case D: case E: case G:
	case I: case O: case Z: case C:
	{
	f(_t);
	if( actPar == NULL && termFlag) goto endFMT;
	_t = _retTree;
	q(_t);
	_t = _retTree;
	break; // out of switch
	}
	default:
	goto endFMT;
	}
	}
	
	endFMT: // end of one repetition
	if( actPar == NULL && termFlag) break;
	}
	
	_t = __t2;
	_t = _t->getNextSibling();
	_retTree = _t;
}

void FMTIn::q(RefFMTNode _t) {
	RefFMTNode q_AST_in = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode s = RefFMTNode(antlr::nullAST);
	
	{
	if (_t == RefFMTNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case SLASH:
	{
		s = _t;
		match(antlr::RefAST(_t),SLASH);
		_t = _t->getNextSibling();
		
		for( int r=s->getRep(); r > 0; r--) GetLine();
		
		break;
	}
	case 3:
	case FORMAT:
	case STRING:
	case TL:
	case TR:
	case TERM:
	case NONL:
	case Q:
	case T:
	case X:
	case A:
	case F:
	case E:
	case G:
	case I:
	case O:
	case Z:
	case ZZ:
	case C:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	_retTree = _t;
}

void FMTIn::f(RefFMTNode _t) {
	RefFMTNode f_AST_in = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode t = RefFMTNode(antlr::nullAST);
	RefFMTNode a = RefFMTNode(antlr::nullAST);
	RefFMTNode f = RefFMTNode(antlr::nullAST);
	RefFMTNode e = RefFMTNode(antlr::nullAST);
	RefFMTNode g = RefFMTNode(antlr::nullAST);
	RefFMTNode i = RefFMTNode(antlr::nullAST);
	RefFMTNode o = RefFMTNode(antlr::nullAST);
	RefFMTNode z = RefFMTNode(antlr::nullAST);
	RefFMTNode zz = RefFMTNode(antlr::nullAST);
	RefFMTNode c = RefFMTNode(antlr::nullAST);
	
	RefFMTNode actNode;
	
	
	if (_t == RefFMTNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case TERM:
	{
		RefFMTNode tmp1_AST_in = _t;
		match(antlr::RefAST(_t),TERM);
		_t = _t->getNextSibling();
		termFlag = true;
		break;
	}
	case NONL:
	{
		RefFMTNode tmp2_AST_in = _t;
		match(antlr::RefAST(_t),NONL);
		_t = _t->getNextSibling();
		break;
	}
	case Q:
	{
		RefFMTNode tmp3_AST_in = _t;
		match(antlr::RefAST(_t),Q);
		_t = _t->getNextSibling();
		
		SizeT nLeft = ioss.rdbuf()->in_avail();
		std::istringstream iossTmp( i2s( nLeft));
		int r = 1;
		do {
		SizeT tCount = actPar->IFmtA( &iossTmp, valIx, r, 0);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case T:
	{
		t = _t;
		match(antlr::RefAST(_t),T);
		_t = _t->getNextSibling();
		
		int    tVal = t->getW();
		ioss.seekg( tVal, std::ios_base::beg);
		
		break;
	}
	case STRING:
	case TL:
	case TR:
	{
		f_csubcode(_t);
		_t = _retTree;
		break;
	}
	case X:
	{
		x(_t);
		_t = _retTree;
		break;
	}
	case FORMAT:
	{
		format_recursive(_t);
		_t = _retTree;
		break;
	}
	case A:
	{
		a = _t;
		match(antlr::RefAST(_t),A);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int r = a->getRep();
		int w = a->getW();
		do {
		SizeT tCount = actPar->IFmtA( &ioss, valIx, r, w);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case F:
	case E:
	case G:
	{
		{
		if (_t == RefFMTNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case F:
		{
			f = _t;
			match(antlr::RefAST(_t),F);
			_t = _t->getNextSibling();
			actNode = f;
			break;
		}
		case E:
		{
			e = _t;
			match(antlr::RefAST(_t),E);
			_t = _t->getNextSibling();
			actNode = e;
			break;
		}
		case G:
		{
			g = _t;
			match(antlr::RefAST(_t),G);
			_t = _t->getNextSibling();
			actNode = g;
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		if( actPar == NULL) break;
		
		int r = actNode->getRep();
		int w = actNode->getW();
		//             if( w <= 0) 
		//                 if( actPar->Type() == FLOAT) 
		//                 w = 15; // set default
		//                 else
		//                 w = 25;
		do {
		SizeT tCount = actPar->IFmtF( &ioss, valIx, r, w);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case I:
	{
		i = _t;
		match(antlr::RefAST(_t),I);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int r = i->getRep();
		int w = i->getW();
		do {
		SizeT tCount = actPar->IFmtI( &ioss, valIx, r, w,
		BaseGDL::DEC);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case O:
	{
		o = _t;
		match(antlr::RefAST(_t),O);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int r = o->getRep();
		int w = o->getW();
		do {
		SizeT tCount = actPar->IFmtI( &ioss, valIx, r, w, 
		BaseGDL::OCT);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case Z:
	case ZZ:
	{
		{
		if (_t == RefFMTNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case Z:
		{
			z = _t;
			match(antlr::RefAST(_t),Z);
			_t = _t->getNextSibling();
			actNode = z;
			break;
		}
		case ZZ:
		{
			zz = _t;
			match(antlr::RefAST(_t),ZZ);
			_t = _t->getNextSibling();
			actNode = zz;
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		if( actPar == NULL) break;
		
		int r = actNode->getRep();
		int w = actNode->getW();
		do {
		SizeT tCount = actPar->IFmtI( &ioss, valIx, r, w,
		BaseGDL::HEX);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case C:
	{
		RefFMTNode __t18 = _t;
		c = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),C);
		_t = _t->getFirstChild();
		{ // ( ... )+
		int _cnt20=0;
		for (;;) {
			if (_t == RefFMTNode(antlr::nullAST) )
				_t = ASTNULL;
			if ((_tokenSet_1.member(_t->getType()))) {
				csubcode(_t);
				_t = _retTree;
			}
			else {
				if ( _cnt20>=1 ) { goto _loop20; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
			}
			
			_cnt20++;
		}
		_loop20:;
		}  // ( ... )+
		_t = __t18;
		_t = _t->getNextSibling();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
}

void FMTIn::format_recursive(RefFMTNode _t) {
	RefFMTNode format_recursive_AST_in = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode fmt = RefFMTNode(antlr::nullAST);
	
	RefFMTNode __t6 = _t;
	fmt = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),FORMAT);
	_t = _t->getFirstChild();
	goto realCode;
	q(_t);
	_t = _retTree;
	{ // ( ... )+
	int _cnt8=0;
	for (;;) {
		if (_t == RefFMTNode(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			f(_t);
			_t = _retTree;
			q(_t);
			_t = _retTree;
		}
		else {
			if ( _cnt8>=1 ) { goto _loop8; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt8++;
	}
	_loop8:;
	}  // ( ... )+
	
	realCode:
	
	reversionAnker = fmt;
	
	RefFMTNode blk = _t; // q (f q)+
	
	for( int r = fmt->getRep(); r > 0; r--)
	{
	//    GetLine(); // the difference to format 
	
	q( blk);
	_t = _retTree;
	
	for (;;) 
	{
	if( _t == static_cast<RefFMTNode>(antlr::nullAST))
	_t = ASTNULL;
	
	switch ( _t->getType()) {
	case FORMAT:
	case STRING:
	case CSTRING:
	case TL:
	case TR:
	case TERM:
	case NONL:
	case Q: case T: case X: case A:
	case F: case D: case E: case G:
	case I: case O: case Z: case C:
	{
	f(_t);
	if( actPar == NULL && termFlag) goto endFMT;
	_t = _retTree;
	q(_t);
	_t = _retTree;
	break; // out of switch
	}
	default:
	goto endFMT;
	}
	}
	
	endFMT: // end of one repetition
	if( actPar == NULL && termFlag) break;
	}
	
	_t = __t6;
	_t = _t->getNextSibling();
	_retTree = _t;
}

void FMTIn::format_reversion(RefFMTNode _t) {
	RefFMTNode format_reversion_AST_in = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	
	format(_t);
	_t = _retTree;
	goto realCode;
	q(_t);
	_t = _retTree;
	{ // ( ... )*
	for (;;) {
		if (_t == RefFMTNode(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			f(_t);
			_t = _retTree;
			q(_t);
			_t = _retTree;
		}
		else {
			goto _loop11;
		}
		
	}
	_loop11:;
	} // ( ... )*
	
	realCode:
	
	q( _t);
	_t = _retTree;
	
	for (;;) 
	{
	if( _t == static_cast<RefFMTNode>(antlr::nullAST))
	_t = ASTNULL;
	
	switch ( _t->getType()) {
	case FORMAT:
	case STRING:
	case CSTRING:
	case TL:
	case TR:
	case TERM:
	case NONL:
	case Q: case T: case X: case A:
	case F: case D: case E: case G:
	case I: case O: case Z: case C:
	{
	f(_t);
	if( actPar == NULL) goto endFMT;
	_t = _retTree;
	q(_t);
	_t = _retTree;
	break; // out of switch
	}
	default:
	goto endFMT;
	}
	}
	endFMT:
	
	_retTree = _t;
}

void FMTIn::f_csubcode(RefFMTNode _t) {
	RefFMTNode f_csubcode_AST_in = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode s = RefFMTNode(antlr::nullAST);
	RefFMTNode tl = RefFMTNode(antlr::nullAST);
	RefFMTNode tr = RefFMTNode(antlr::nullAST);
	
	if (_t == RefFMTNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case STRING:
	{
		s = _t;
		match(antlr::RefAST(_t),STRING);
		_t = _t->getNextSibling();
		break;
	}
	case TL:
	{
		tl = _t;
		match(antlr::RefAST(_t),TL);
		_t = _t->getNextSibling();
		
		SizeT actP  = ioss.tellg(); 
		int    tlVal = tl->getW();
		if( tlVal > actP)
		ioss.seekg( 0);
		else
		ioss.seekg( actP - tlVal);
		
		break;
	}
	case TR:
	{
		tr = _t;
		match(antlr::RefAST(_t),TR);
		_t = _t->getNextSibling();
		
		int    tlVal = tl->getW();
		ioss.seekg( tlVal, std::ios_base::cur);
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
}

void FMTIn::x(RefFMTNode _t) {
	RefFMTNode x_AST_in = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode tl = RefFMTNode(antlr::nullAST);
	
	tl = _t;
	match(antlr::RefAST(_t),X);
	_t = _t->getNextSibling();
	
	if( _t != static_cast<RefFMTNode>(antlr::nullAST))
	{
	int    tlVal = tl->getW();
	ioss.seekg( tlVal, std::ios_base::cur);
	}
	
	_retTree = _t;
}

void FMTIn::csubcode(RefFMTNode _t) {
	RefFMTNode csubcode_AST_in = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode c1 = RefFMTNode(antlr::nullAST);
	RefFMTNode c2 = RefFMTNode(antlr::nullAST);
	RefFMTNode c3 = RefFMTNode(antlr::nullAST);
	RefFMTNode c4 = RefFMTNode(antlr::nullAST);
	RefFMTNode c5 = RefFMTNode(antlr::nullAST);
	RefFMTNode c6 = RefFMTNode(antlr::nullAST);
	RefFMTNode c7 = RefFMTNode(antlr::nullAST);
	RefFMTNode c8 = RefFMTNode(antlr::nullAST);
	RefFMTNode c9 = RefFMTNode(antlr::nullAST);
	RefFMTNode c10 = RefFMTNode(antlr::nullAST);
	RefFMTNode c11 = RefFMTNode(antlr::nullAST);
	RefFMTNode c12 = RefFMTNode(antlr::nullAST);
	RefFMTNode c13 = RefFMTNode(antlr::nullAST);
	RefFMTNode c14 = RefFMTNode(antlr::nullAST);
	RefFMTNode c15 = RefFMTNode(antlr::nullAST);
	RefFMTNode c16 = RefFMTNode(antlr::nullAST);
	RefFMTNode c17 = RefFMTNode(antlr::nullAST);
	
	if (_t == RefFMTNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case CMOA:
	{
		c1 = _t;
		match(antlr::RefAST(_t),CMOA);
		_t = _t->getNextSibling();
		break;
	}
	case CMoA:
	{
		c2 = _t;
		match(antlr::RefAST(_t),CMoA);
		_t = _t->getNextSibling();
		break;
	}
	case CmoA:
	{
		c3 = _t;
		match(antlr::RefAST(_t),CmoA);
		_t = _t->getNextSibling();
		break;
	}
	case CHI:
	{
		c4 = _t;
		match(antlr::RefAST(_t),CHI);
		_t = _t->getNextSibling();
		break;
	}
	case ChI:
	{
		c5 = _t;
		match(antlr::RefAST(_t),ChI);
		_t = _t->getNextSibling();
		break;
	}
	case CDWA:
	{
		c6 = _t;
		match(antlr::RefAST(_t),CDWA);
		_t = _t->getNextSibling();
		break;
	}
	case CDwA:
	{
		c7 = _t;
		match(antlr::RefAST(_t),CDwA);
		_t = _t->getNextSibling();
		break;
	}
	case CdwA:
	{
		c8 = _t;
		match(antlr::RefAST(_t),CdwA);
		_t = _t->getNextSibling();
		break;
	}
	case CAPA:
	{
		c9 = _t;
		match(antlr::RefAST(_t),CAPA);
		_t = _t->getNextSibling();
		break;
	}
	case CApA:
	{
		c10 = _t;
		match(antlr::RefAST(_t),CApA);
		_t = _t->getNextSibling();
		break;
	}
	case CapA:
	{
		c11 = _t;
		match(antlr::RefAST(_t),CapA);
		_t = _t->getNextSibling();
		break;
	}
	case CMOI:
	{
		c12 = _t;
		match(antlr::RefAST(_t),CMOI);
		_t = _t->getNextSibling();
		break;
	}
	case CDI:
	{
		c13 = _t;
		match(antlr::RefAST(_t),CDI);
		_t = _t->getNextSibling();
		break;
	}
	case CYI:
	{
		c14 = _t;
		match(antlr::RefAST(_t),CYI);
		_t = _t->getNextSibling();
		break;
	}
	case CMI:
	{
		c15 = _t;
		match(antlr::RefAST(_t),CMI);
		_t = _t->getNextSibling();
		break;
	}
	case CSI:
	{
		c16 = _t;
		match(antlr::RefAST(_t),CSI);
		_t = _t->getNextSibling();
		break;
	}
	case CSF:
	{
		c17 = _t;
		match(antlr::RefAST(_t),CSF);
		_t = _t->getNextSibling();
		break;
	}
	case X:
	{
		x(_t);
		_t = _retTree;
		break;
	}
	case STRING:
	case TL:
	case TR:
	{
		f_csubcode(_t);
		_t = _retTree;
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
}

void FMTIn::initializeASTFactory( antlr::ASTFactory& )
{
}
const char* FMTIn::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"ALL",
	"CSTR",
	"CSTR1",
	"CSTR2",
	"ESC",
	"OCTESC",
	"ODIGIT",
	"HEXESC",
	"HDIGIT",
	"CD",
	"CE",
	"CI",
	"CF",
	"CG",
	"CO",
	"CS",
	"CX",
	"CZ",
	"CDOT",
	"DIGITS",
	"CNUMBER",
	"CWS",
	"FORMAT",
	"LBRACE",
	"COMMA",
	"RBRACE",
	"SLASH",
	"STRING",
	"\"tl\"",
	"\"tr\"",
	"TERM",
	"NONL",
	"Q",
	"T",
	"X",
	"A",
	"F",
	"D",
	"E",
	"G",
	"I",
	"O",
	"Z",
	"ZZ",
	"C",
	"CMOA",
	"CMoA",
	"CmoA",
	"CHI",
	"ChI",
	"CDWA",
	"CDwA",
	"CdwA",
	"CAPA",
	"CApA",
	"CapA",
	"\"cmoi\"",
	"\"cdi\"",
	"\"cyi\"",
	"\"cmi\"",
	"\"csi\"",
	"\"csf\"",
	"NUMBER",
	"DOT",
	"CSTRING",
	"H",
	"L",
	"R",
	"PERCENT",
	"W",
	"WHITESPACE",
	"CHAR",
	0
};

const unsigned long FMTIn::_tokenSet_0_data_[] = { 2214592512UL, 130559UL, 0UL, 0UL };
// FORMAT STRING "tl" "tr" TERM NONL Q T X A F E G I O Z ZZ C 
const antlr::BitSet FMTIn::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long FMTIn::_tokenSet_1_data_[] = { 2147483648UL, 4294836291UL, 3UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// STRING "tl" "tr" X CMOA CMoA CmoA CHI ChI CDWA CDwA CdwA CAPA CApA CapA 
// "cmoi" "cdi" "cyi" "cmi" "csi" "csf" 
const antlr::BitSet FMTIn::_tokenSet_1(_tokenSet_1_data_,8);


