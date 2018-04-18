/* $ANTLR 2.7.7 (2006-11-01): "format.out.g" -> "FMTOut.cpp"$ */

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
	RefFMTNode format_AST_in = (_t == RefFMTNode(ASTNULL)) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode fmt = RefFMTNode(antlr::nullAST);
	
	RefFMTNode __t2 = _t;
	fmt = (_t == RefFMTNode(ASTNULL)) ? RefFMTNode(antlr::nullAST) : _t;
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
	case TL:
	case TR:
	case TERM:
	case NONL:
	case Q: case T: case X: case A:
	case F: case D: case E: case SE: case G: case SG:
	case I: case O: case B: case Z: case ZZ: case C:
	{
	if( actPar == NULL && termFlag) goto endFMT;
	// no break
	}
	case STRING:
	case CSTRING:
	{
	f(_t);
	//                                if( actPar == NULL && termFlag) goto endFMT;
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
	RefFMTNode q_AST_in = (_t == RefFMTNode(ASTNULL)) ? RefFMTNode(antlr::nullAST) : _t;
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
		if( j != NULL && j->OStream().rdbuf() == os->rdbuf())
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
	case SE:
	case G:
	case SG:
	case I:
	case O:
	case B:
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
	RefFMTNode f_AST_in = (_t == RefFMTNode(ASTNULL)) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode t = RefFMTNode(antlr::nullAST);
	RefFMTNode a = RefFMTNode(antlr::nullAST);
	RefFMTNode ff = RefFMTNode(antlr::nullAST);
	RefFMTNode se = RefFMTNode(antlr::nullAST);
	RefFMTNode ee = RefFMTNode(antlr::nullAST);
	RefFMTNode sg = RefFMTNode(antlr::nullAST);
	RefFMTNode g = RefFMTNode(antlr::nullAST);
	RefFMTNode i = RefFMTNode(antlr::nullAST);
	RefFMTNode o = RefFMTNode(antlr::nullAST);
	RefFMTNode b = RefFMTNode(antlr::nullAST);
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
		char f = ff->getFill();
		do {
		SizeT tCount = actPar->OFmtF( os, valIx, r, w, d, f);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case SE:
	{
		se = _t;
		match(antlr::RefAST(_t),SE);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int r = se->getRep();
		int w = se->getW();
		int d = se->getD();
		char f = se->getFill();
		do {
		SizeT tCount = actPar->OFmtF( os, valIx, r, w, d, f,
		BaseGDL::SCIENTIFIC);
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
		char f = ee->getFill();
		do {
		SizeT tCount = actPar->OFmtF( os, valIx, r, w, d, f,
		BaseGDL::SCIENTIFIC, true); //uppercase
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case SG:
	{
		sg = _t;
		match(antlr::RefAST(_t),SG);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int r = sg->getRep();
		int w = sg->getW();
		int d = sg->getD();
		int f = sg->getFill();
		do {
		SizeT tCount = actPar->OFmtF( os, valIx, r, w, d, f,
		BaseGDL::AUTO);
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
		int f = g->getFill();
		do {
		SizeT tCount = actPar->OFmtF( os, valIx, r, w, d, f,
		BaseGDL::AUTO, true);
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
		int f = i->getFill();
		do {
		SizeT tCount = actPar->OFmtI( os, valIx, r, w, d, f,
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
		int f = o->getFill();
		do {
		SizeT tCount = actPar->OFmtI( os, valIx, r, w, d, f,
		BaseGDL::OCT);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case B:
	{
		b = _t;
		match(antlr::RefAST(_t),B);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int r = b->getRep();
		int w = b->getW();
		int d = b->getD();
		int f = b->getFill();
		do {
		SizeT tCount = actPar->OFmtI( os, valIx, r, w, d, f,
		BaseGDL::BIN);
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
		int f = z->getFill();
		do {
		SizeT tCount = actPar->OFmtI( os, valIx, r, w, d, f,
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
		int f = zz->getFill();
		do {
		SizeT tCount = actPar->OFmtI( os, valIx, r, w, d, f,
		BaseGDL::HEXL);
		r -= tCount;
		NextVal( tCount);
		if( actPar == NULL) break;
		} while( r>0);
		
		break;
	}
	case C:
	{
		{
		RefFMTNode __t13 = _t;
		c = (_t == RefFMTNode(ASTNULL)) ? RefFMTNode(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),C);
		_t = _t->getFirstChild();
		
		int r = c->getRep(); if (r<1) r=1;
		if( actPar == NULL) break;
		SizeT nTrans = actPar->ToTransfer();
		if (r > nTrans) r=nTrans;
		actPar->OFmtCal( os, valIx, r, 0, 0, NULL, BaseGDL::COMPUTE); //convert to hour, min, etc
		
		{
		if (_t == RefFMTNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case STRING:
		case X:
		case CMOA:
		case CMoA:
		case CmoA:
		case CHI:
		case ChI:
		case CDWA:
		case CDwA:
		case CdwA:
		case CAPA:
		case CApA:
		case CapA:
		case CMOI:
		case CDI:
		case CYI:
		case CMI:
		case CSI:
		case CSF:
		{
			{ // ( ... )+
			int _cnt16=0;
			for (;;) {
				if (_t == RefFMTNode(antlr::nullAST) )
					_t = ASTNULL;
				if ((_tokenSet_1.member(_t->getType()))) {
					csubcode(_t,r);
					_t = _retTree;
				}
				else {
					if ( _cnt16>=1 ) { goto _loop16; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
				}
				
				_cnt16++;
			}
			_loop16:;
			}  // ( ... )+
			break;
		}
		case 3:
		{
			
			if( actPar == NULL) break;
			actPar->OFmtCal( os, valIx, r, 0, 0, NULL, BaseGDL::DEFAULT);
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		if( actPar == NULL) break;
		SizeT tCount = actPar->OFmtCal( os, valIx, r, 0, 0, NULL, BaseGDL::WRITE); //Write the complete formatted string to os.
		NextVal( tCount);
		if( actPar == NULL) break;
		
		_t = __t13;
		_t = _t->getNextSibling();
		}
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
	RefFMTNode format_reversion_AST_in = (_t == RefFMTNode(ASTNULL)) ? RefFMTNode(antlr::nullAST) : _t;
	
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
	case F: case D: case E: case SE: case G: case SG:
	case I: case O: case B: case Z: case ZZ: case C:
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
	RefFMTNode f_csubcode_AST_in = (_t == RefFMTNode(ASTNULL)) ? RefFMTNode(antlr::nullAST) : _t;
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
	RefFMTNode x_AST_in = (_t == RefFMTNode(ASTNULL)) ? RefFMTNode(antlr::nullAST) : _t;
	RefFMTNode tl = RefFMTNode(antlr::nullAST);
	
	tl = _t;
	match(antlr::RefAST(_t),X);
	_t = _t->getNextSibling();
	
	if( _t != static_cast<RefFMTNode>(antlr::nullAST))
	{
	int    tlVal = tl->getW();
	(*os) << " "; //for format "X" (no width)
	for( int i=tlVal; i>1; --i)
	(*os) << " "; //for format "nX"
	//                os->seekp( tlVal, std::ios_base::cur);
	}
	// for( int r=x->getW(); r > 0; r--) (*os) << ' ';
	
	_retTree = _t;
}

void FMTOut::csubcode(RefFMTNode _t,
	SizeT r
) {
	RefFMTNode csubcode_AST_in = (_t == RefFMTNode(ASTNULL)) ? RefFMTNode(antlr::nullAST) : _t;
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
	RefFMTNode c18 = RefFMTNode(antlr::nullAST);
	RefFMTNode c19 = RefFMTNode(antlr::nullAST);
	
	if (_t == RefFMTNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case CMOA:
	{
		c1 = _t;
		match(antlr::RefAST(_t),CMOA);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		int w = c1->getW();
		int d = c1->getD();
		char f = c1->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CMOA);
		
		break;
	}
	case CMoA:
	{
		c2 = _t;
		match(antlr::RefAST(_t),CMoA);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c2->getW();
		int d = c2->getD();
		char f = c2->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CMoA);
		
		break;
	}
	case CmoA:
	{
		c3 = _t;
		match(antlr::RefAST(_t),CmoA);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c3->getW();
		int d = c3->getD();
		char f = c3->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CmoA);
		
		break;
	}
	case CHI:
	{
		c4 = _t;
		match(antlr::RefAST(_t),CHI);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c4->getW();
		int d = c4->getD();
		char f = c4->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CHI);
		
		break;
	}
	case ChI:
	{
		c5 = _t;
		match(antlr::RefAST(_t),ChI);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c5->getW();
		int d = c5->getD();
		char f = c5->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::ChI);
		
		break;
	}
	case CDWA:
	{
		c6 = _t;
		match(antlr::RefAST(_t),CDWA);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c6->getW();
		int d = c6->getD();
		char f = c6->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CDWA);
		
		break;
	}
	case CDwA:
	{
		c7 = _t;
		match(antlr::RefAST(_t),CDwA);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c7->getW();
		int d = c7->getD();
		char f = c7->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CDwA);
		
		break;
	}
	case CdwA:
	{
		c8 = _t;
		match(antlr::RefAST(_t),CdwA);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c8->getW();
		int d = c8->getD();
		char f = c8->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CdwA);
		
		break;
	}
	case CAPA:
	{
		c9 = _t;
		match(antlr::RefAST(_t),CAPA);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c9->getW();
		int d = c9->getD();
		char f = c9->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CAPA);
		
		break;
	}
	case CApA:
	{
		c10 = _t;
		match(antlr::RefAST(_t),CApA);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c10->getW();
		int d = c10->getD();
		char f = c10->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CApA);
		
		break;
	}
	case CapA:
	{
		c11 = _t;
		match(antlr::RefAST(_t),CapA);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c11->getW();
		int d = c11->getD();
		char f = c11->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CapA);
		
		break;
	}
	case CMOI:
	{
		c12 = _t;
		match(antlr::RefAST(_t),CMOI);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c12->getW();
		int d = c12->getD();
		char f = c12->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CMOI);
		
		break;
	}
	case CDI:
	{
		c13 = _t;
		match(antlr::RefAST(_t),CDI);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c13->getW();
		int d = c13->getD();
		char f = c13->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CDI);
		
		break;
	}
	case CYI:
	{
		c14 = _t;
		match(antlr::RefAST(_t),CYI);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c14->getW();
		int d = c14->getD();
		char f = c14->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CYI);
		
		break;
	}
	case CMI:
	{
		c15 = _t;
		match(antlr::RefAST(_t),CMI);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c15->getW();
		int d = c15->getD();
		char f = c15->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CMI);
		
		break;
	}
	case CSI:
	{
		c16 = _t;
		match(antlr::RefAST(_t),CSI);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		
		int w = c16->getW();
		int d = c16->getD();
		char f = c16->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CSI);
		
		break;
	}
	case CSF:
	{
		c17 = _t;
		match(antlr::RefAST(_t),CSF);
		_t = _t->getNextSibling();
		
		if( actPar == NULL) break;
		int w = c17->getW();
		int d = c17->getD();
		char f = c17->getFill();
		SizeT tCount = actPar->OFmtCal( os, valIx, r, w, d, &f, BaseGDL::CSF);
		
		break;
	}
	case X:
	{
		c18 = _t;
		match(antlr::RefAST(_t),X);
		_t = _t->getNextSibling();
		
				if( actPar == NULL) break;
				int    tlVal = c18->getW(); if (tlVal < 1) tlVal=1;
				std::string *s=new std::string(tlVal,' ');
				SizeT tCount = actPar->OFmtCal( os, valIx, r, 0, 0, (char*)s->c_str(), BaseGDL::STRING);
		delete s;
		
		break;
	}
	case STRING:
	{
		c19 = _t;
		match(antlr::RefAST(_t),STRING);
		_t = _t->getNextSibling();
		
				if( actPar == NULL) break;
				SizeT tCount = actPar->OFmtCal( os, valIx, r, 0, 0, (char*)c19->getText().c_str(), BaseGDL::STRING);
		
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
	"CSE",
	"CE",
	"CI",
	"CF",
	"CSG",
	"CG",
	"CO",
	"CB",
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
	"SE",
	"G",
	"SG",
	"I",
	"O",
	"B",
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
	"CMOI",
	"CDI",
	"CYI",
	"CMI",
	"CSI",
	"CSF",
	"PM",
	"MP",
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

const unsigned long FMTOut::_tokenSet_0_data_[] = { 536870912UL, 8384508UL, 0UL, 0UL };
// FORMAT STRING "tl" "tr" TERM NONL Q T X A F E SE G SG I O B Z ZZ C 
const antlr::BitSet FMTOut::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long FMTOut::_tokenSet_1_data_[] = { 0UL, 4286579204UL, 255UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// STRING X CMOA CMoA CmoA CHI ChI CDWA CDwA CdwA CAPA CApA CapA CMOI CDI 
// CYI CMI CSI CSF 
const antlr::BitSet FMTOut::_tokenSet_1(_tokenSet_1_data_,8);


