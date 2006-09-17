/* $ANTLR 2.7.4: "format.out.g" -> "FMTOut.cpp"$ */

#include "includefirst.hpp"

#include "FMTOut.hpp"
#include <antlr/Token.hpp>
#include <antlr/AST.hpp>
#include <antlr/NoViableAltException.hpp>
#include <antlr/MismatchedTokenException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/BitSet.hpp>

    // gets inserted after the antlr generated includes in the cpp file

FMTOut::FMTOut()
	: antlr::TreeParser() {
}

void FMTOut::format(RefFMTNode _t) {
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
	
	for( int r = fmt->getRep(); r > 0; r--)
	{
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

void FMTOut::q(RefFMTNode _t) {
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
		
		// only one newline to journal file
		GDLStream* j = lib::get_journal();
		if( j != NULL && j->OStream() == (*os))
		(*os) << '\n' << lib::JOURNALCOMMENT;
		else
		for( int r=s->getRep(); r > 0; r--) (*os) << '\n';
		
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

void FMTOut::f(RefFMTNode _t) {
	RefFMTNode f_AST_in = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode t = RefFMTNode(antlr::nullAST);
	RefFMTNode a = RefFMTNode(antlr::nullAST);
	RefFMTNode ff = RefFMTNode(antlr::nullAST);
	RefFMTNode ee = RefFMTNode(antlr::nullAST);
	RefFMTNode g = RefFMTNode(antlr::nullAST);
	RefFMTNode i = RefFMTNode(antlr::nullAST);
	RefFMTNode o = RefFMTNode(antlr::nullAST);
	RefFMTNode z = RefFMTNode(antlr::nullAST);
	RefFMTNode zz = RefFMTNode(antlr::nullAST);
	RefFMTNode c = RefFMTNode(antlr::nullAST);
	
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
		nonlFlag = true;
		break;
	}
	case Q:
	{
		RefFMTNode tmp3_AST_in = _t;
		match(antlr::RefAST(_t),Q);
		_t = _t->getNextSibling();
		break;
	}
	case T:
	{
		t = _t;
		match(antlr::RefAST(_t),T);
		_t = _t->getNextSibling();
		
		int    tVal = t->getW();
		assert( tVal >= 1);
		os->seekp( tVal-1, std::ios_base::beg);
		
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
		format(_t);
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
		SizeT tCount = actPar->OFmtA( os, valIx, r, w);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case F:
	{
		ff = _t;
		match(antlr::RefAST(_t),F);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int r = ff->getRep();
		int w = ff->getW();
		int d = ff->getD();
		do {
		SizeT tCount = actPar->OFmtF( os, valIx, r, w, d);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case E:
	{
		ee = _t;
		match(antlr::RefAST(_t),E);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int r = ee->getRep();
		int w = ee->getW();
		int d = ee->getD();
		do {
		SizeT tCount = actPar->OFmtF( os, valIx, r, w, d, 
		BaseGDL::SCIENTIFIC);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case G:
	{
		g = _t;
		match(antlr::RefAST(_t),G);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int r = g->getRep();
		int w = g->getW();
		int d = g->getD();
		do {
		SizeT tCount = actPar->OFmtF( os, valIx, r, w, d, 
		BaseGDL::AUTO);
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
		int d = i->getD();
		do {
		SizeT tCount = actPar->OFmtI( os, valIx, r, w, d, 
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
		int d = o->getD();
		do {
		SizeT tCount = actPar->OFmtI( os, valIx, r, w, d, 
		BaseGDL::OCT);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case Z:
	{
		z = _t;
		match(antlr::RefAST(_t),Z);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int r = z->getRep();
		int w = z->getW();
		int d = z->getD();
		do {
		SizeT tCount = actPar->OFmtI( os, valIx, r, w, d, 
		BaseGDL::HEX);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case ZZ:
	{
		zz = _t;
		match(antlr::RefAST(_t),ZZ);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int r = zz->getRep();
		int w = zz->getW();
		int d = zz->getD();
		do {
		SizeT tCount = actPar->OFmtI( os, valIx, r, w, d, 
		BaseGDL::HEXL);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case C:
	{
		RefFMTNode __t12 = _t;
		c = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),C);
		_t = _t->getFirstChild();
		{ // ( ... )+
		int _cnt14=0;
		for (;;) {
			if (_t == RefFMTNode(antlr::nullAST) )
				_t = ASTNULL;
			if ((_tokenSet_1.member(_t->getType()))) {
				csubcode(_t);
				_t = _retTree;
			}
			else {
				if ( _cnt14>=1 ) { goto _loop14; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
			}
			
			_cnt14++;
		}
		_loop14:;
		}  // ( ... )+
		_t = __t12;
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

void FMTOut::format_reversion(RefFMTNode _t) {
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
			goto _loop7;
		}
		
	}
	_loop7:;
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
	endFMT: // end of one repetition
	
	_retTree = _t;
}

void FMTOut::f_csubcode(RefFMTNode _t) {
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
		(*os) << s->getText();
		break;
	}
	case TL:
	{
		tl = _t;
		match(antlr::RefAST(_t),TL);
		_t = _t->getNextSibling();
		
		SizeT actP  = os->tellp(); 
		int    tlVal = tl->getW();
		if( tlVal > actP)
		os->seekp( 0);
		else
		os->seekp( actP - tlVal);
		
		break;
	}
	case TR:
	{
		tr = _t;
		match(antlr::RefAST(_t),TR);
		_t = _t->getNextSibling();
		
		int    tlVal = tl->getW();
		for( int i=tlVal; i>0; --i)
		(*os) << " ";
		//            os->seekp( tlVal, std::ios_base::cur);
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
}

void FMTOut::x(RefFMTNode _t) {
	RefFMTNode x_AST_in = (_t == ASTNULL) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode tl = RefFMTNode(antlr::nullAST);
	
	tl = _t;
	match(antlr::RefAST(_t),X);
	_t = _t->getNextSibling();
	
	if( _t != static_cast<RefFMTNode>(antlr::nullAST))
	{
	int    tlVal = tl->getW();
	for( int i=tlVal; i>0; --i)
	(*os) << " ";
	//                os->seekp( tlVal, std::ios_base::cur);
	}
	// for( int r=x->getW(); r > 0; r--) (*os) << ' ';
	
	_retTree = _t;
}

void FMTOut::csubcode(RefFMTNode _t) {
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

void FMTOut::initializeASTFactory( antlr::ASTFactory& )
{
}
const char* FMTOut::tokenNames[] = {
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

const unsigned long FMTOut::_tokenSet_0_data_[] = { 2214592512UL, 130559UL, 0UL, 0UL };
// FORMAT STRING "tl" "tr" TERM NONL Q T X A F E G I O Z ZZ C 
const antlr::BitSet FMTOut::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long FMTOut::_tokenSet_1_data_[] = { 2147483648UL, 4294836291UL, 3UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// STRING "tl" "tr" X CMOA CMoA CmoA CHI ChI CDWA CDwA CdwA CAPA CApA CapA 
// "cmoi" "cdi" "cyi" "cmi" "csi" "csf" 
const antlr::BitSet FMTOut::_tokenSet_1(_tokenSet_1_data_,8);


