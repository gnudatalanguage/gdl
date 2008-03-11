/* $ANTLR 2.7.6 (20070910): "format.g" -> "FMTParser.cpp"$ */

#include "includefirst.hpp"

#include "FMTParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
FMTParser::FMTParser(antlr::TokenBuffer& tokenBuf, int k)
: antlr::LLkParser(tokenBuf,k)
{
}

FMTParser::FMTParser(antlr::TokenBuffer& tokenBuf)
: antlr::LLkParser(tokenBuf,1)
{
}

FMTParser::FMTParser(antlr::TokenStream& lexer, int k)
: antlr::LLkParser(lexer,k)
{
}

FMTParser::FMTParser(antlr::TokenStream& lexer)
: antlr::LLkParser(lexer,1)
{
}

FMTParser::FMTParser(const antlr::ParserSharedInputState& state)
: antlr::LLkParser(state,1)
{
}

void FMTParser::format(
	 int repeat
) {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode format_AST = RefFMTNode(antlr::nullAST);
	
	match(LBRACE);
	qfq();
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			qfq();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		else {
			goto _loop3;
		}
		
	}
	_loop3:;
	} // ( ... )*
	match(RBRACE);
	format_AST = RefFMTNode(currentAST.root);
	
	format_AST = RefFMTNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(FORMAT,"FORMAT")))->add(antlr::RefAST(format_AST))));
	format_AST->setRep( repeat);
	
	currentAST.root = format_AST;
	if ( format_AST!=RefFMTNode(antlr::nullAST) &&
		format_AST->getFirstChild() != RefFMTNode(antlr::nullAST) )
		  currentAST.child = format_AST->getFirstChild();
	else
		currentAST.child = format_AST;
	currentAST.advanceChildToEnd();
	format_AST = RefFMTNode(currentAST.root);
	returnAST = format_AST;
}

void FMTParser::qfq() {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode qfq_AST = RefFMTNode(antlr::nullAST);
	
	q();
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	{
	switch ( LA(1)) {
	case CSTR:
	case CD:
	case CE:
	case CI:
	case CF:
	case CG:
	case CO:
	case CS:
	case CX:
	case CZ:
	case CNUMBER:
	case LBRACE:
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
	case D:
	case E:
	case G:
	case I:
	case O:
	case Z:
	case ZZ:
	case C:
	case NUMBER:
	{
		f();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		q();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		break;
	}
	case COMMA:
	case RBRACE:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	qfq_AST = RefFMTNode(currentAST.root);
	returnAST = qfq_AST;
}

void FMTParser::q() {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode q_AST = RefFMTNode(antlr::nullAST);
	
	int n1 = 0;
	
	
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == SLASH)) {
			RefFMTNode tmp4_AST = RefFMTNode(antlr::nullAST);
			tmp4_AST = astFactory->create(LT(1));
			match(SLASH);
			n1++;
		}
		else {
			goto _loop8;
		}
		
	}
	_loop8:;
	} // ( ... )*
	q_AST = RefFMTNode(currentAST.root);
	
	if( n1 > 0) 
	{
	q_AST = RefFMTNode(astFactory->make((new antlr::ASTArray(1))->add(antlr::RefAST(astFactory->create(SLASH,"/")))));
	q_AST->setRep( n1);
	}           
	
	currentAST.root = q_AST;
	if ( q_AST!=RefFMTNode(antlr::nullAST) &&
		q_AST->getFirstChild() != RefFMTNode(antlr::nullAST) )
		  currentAST.child = q_AST->getFirstChild();
	else
		currentAST.child = q_AST;
	currentAST.advanceChildToEnd();
	returnAST = q_AST;
}

void FMTParser::f() {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode f_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  t = antlr::nullToken;
	RefFMTNode t_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  x = antlr::nullToken;
	RefFMTNode x_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  xx = antlr::nullToken;
	RefFMTNode xx_AST = RefFMTNode(antlr::nullAST);
	
	int n1;
	
	
	switch ( LA(1)) {
	case TERM:
	{
		RefFMTNode tmp5_AST = RefFMTNode(antlr::nullAST);
		tmp5_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp5_AST));
		match(TERM);
		f_AST = RefFMTNode(currentAST.root);
		break;
	}
	case NONL:
	{
		RefFMTNode tmp6_AST = RefFMTNode(antlr::nullAST);
		tmp6_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp6_AST));
		match(NONL);
		f_AST = RefFMTNode(currentAST.root);
		break;
	}
	case Q:
	{
		RefFMTNode tmp7_AST = RefFMTNode(antlr::nullAST);
		tmp7_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp7_AST));
		match(Q);
		f_AST = RefFMTNode(currentAST.root);
		break;
	}
	case T:
	{
		t = LT(1);
		t_AST = astFactory->create(t);
		astFactory->addASTChild(currentAST, antlr::RefAST(t_AST));
		match(T);
		n1=nn();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		t_AST->setW( n1);
		f_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CSTR:
	case CD:
	case CE:
	case CI:
	case CF:
	case CG:
	case CO:
	case CS:
	case CX:
	case CZ:
	case CNUMBER:
	case STRING:
	case TL:
	case TR:
	{
		f_csubcode();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		f_AST = RefFMTNode(currentAST.root);
		break;
	}
	case LBRACE:
	case A:
	case F:
	case D:
	case E:
	case G:
	case I:
	case O:
	case Z:
	case ZZ:
	case C:
	{
		rep_fmt( 1);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		f_AST = RefFMTNode(currentAST.root);
		break;
	}
	case NUMBER:
	{
		n1=nn();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		{
		switch ( LA(1)) {
		case LBRACE:
		case A:
		case F:
		case D:
		case E:
		case G:
		case I:
		case O:
		case Z:
		case ZZ:
		case C:
		{
			rep_fmt( n1);
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			break;
		}
		case X:
		{
			x = LT(1);
			x_AST = astFactory->create(x);
			astFactory->addASTChild(currentAST, antlr::RefAST(x_AST));
			match(X);
			x_AST->setW( n1);
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		f_AST = RefFMTNode(currentAST.root);
		break;
	}
	case X:
	{
		xx = LT(1);
		xx_AST = astFactory->create(xx);
		astFactory->addASTChild(currentAST, antlr::RefAST(xx_AST));
		match(X);
		xx_AST->setW( 1);
		f_AST = RefFMTNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = f_AST;
}

void FMTParser::f_csubcode() {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode f_csubcode_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  tl = antlr::nullToken;
	RefFMTNode tl_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  tr = antlr::nullToken;
	RefFMTNode tr_AST = RefFMTNode(antlr::nullAST);
	
	int n1;
	
	
	switch ( LA(1)) {
	case STRING:
	{
		RefFMTNode tmp8_AST = RefFMTNode(antlr::nullAST);
		tmp8_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp8_AST));
		match(STRING);
		f_csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CSTR:
	case CD:
	case CE:
	case CI:
	case CF:
	case CG:
	case CO:
	case CS:
	case CX:
	case CZ:
	case CNUMBER:
	{
		cstring();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		f_csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case TL:
	{
		tl = LT(1);
		tl_AST = astFactory->create(tl);
		astFactory->addASTChild(currentAST, antlr::RefAST(tl_AST));
		match(TL);
		n1=nn();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		tl_AST->setW( n1);
		f_csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case TR:
	{
		tr = LT(1);
		tr_AST = astFactory->create(tr);
		astFactory->addASTChild(currentAST, antlr::RefAST(tr_AST));
		match(TR);
		n1=nn();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		tr_AST->setW( n1);
		f_csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = f_csubcode_AST;
}

void FMTParser::cstring() {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode cstring_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  s = antlr::nullToken;
	RefFMTNode s_AST = RefFMTNode(antlr::nullAST);
	
	{ // ( ... )+
	int _cnt12=0;
	for (;;) {
		switch ( LA(1)) {
		case CSTR:
		{
			s = LT(1);
			s_AST = astFactory->create(s);
			astFactory->addASTChild(currentAST, antlr::RefAST(s_AST));
			match(CSTR);
			s_AST->setType( STRING);
			break;
		}
		case CD:
		case CE:
		case CI:
		case CF:
		case CG:
		case CO:
		case CS:
		case CX:
		case CZ:
		case CNUMBER:
		{
			cformat();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			break;
		}
		default:
		{
			if ( _cnt12>=1 ) { goto _loop12; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		}
		_cnt12++;
	}
	_loop12:;
	}  // ( ... )+
	cstring_AST = RefFMTNode(currentAST.root);
	returnAST = cstring_AST;
}

 int  FMTParser::nn() {
	 int n;
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode nn_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  num = antlr::nullToken;
	RefFMTNode num_AST = RefFMTNode(antlr::nullAST);
	
	num = LT(1);
	num_AST = astFactory->create(num);
	match(NUMBER);
	
	std::istringstream s(num_AST->getText());
	s >> n;
	
	returnAST = nn_AST;
	return n;
}

void FMTParser::cformat() {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode cformat_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c = antlr::nullToken;
	RefFMTNode c_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  e = antlr::nullToken;
	RefFMTNode e_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  i = antlr::nullToken;
	RefFMTNode i_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  ff = antlr::nullToken;
	RefFMTNode ff_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  g = antlr::nullToken;
	RefFMTNode g_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  o = antlr::nullToken;
	RefFMTNode o_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  x = antlr::nullToken;
	RefFMTNode x_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  z = antlr::nullToken;
	RefFMTNode z_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  s = antlr::nullToken;
	RefFMTNode s_AST = RefFMTNode(antlr::nullAST);
	
	int w = -1;
	int d = -1;
	
	
	{
	switch ( LA(1)) {
	case CNUMBER:
	{
		w=cnn();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		{
		switch ( LA(1)) {
		case CDOT:
		{
			match(CDOT);
			d=cnn();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			break;
		}
		case CD:
		case CE:
		case CI:
		case CF:
		case CG:
		case CO:
		case CS:
		case CX:
		case CZ:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		break;
	}
	case CD:
	case CE:
	case CI:
	case CF:
	case CG:
	case CO:
	case CS:
	case CX:
	case CZ:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{
	switch ( LA(1)) {
	case CD:
	{
		c = LT(1);
		c_AST = astFactory->create(c);
		astFactory->addASTChild(currentAST, antlr::RefAST(c_AST));
		match(CD);
		c_AST->setW( w); c_AST->setD( d); c_AST->setType( I);
		break;
	}
	case CE:
	{
		e = LT(1);
		e_AST = astFactory->create(e);
		astFactory->addASTChild(currentAST, antlr::RefAST(e_AST));
		match(CE);
		e_AST->setW( w); e_AST->setD( d); e_AST->setType( E);
		break;
	}
	case CI:
	{
		i = LT(1);
		i_AST = astFactory->create(i);
		astFactory->addASTChild(currentAST, antlr::RefAST(i_AST));
		match(CI);
		i_AST->setW( w); i_AST->setD( d); i_AST->setType( I);
		break;
	}
	case CF:
	{
		ff = LT(1);
		ff_AST = astFactory->create(ff);
		astFactory->addASTChild(currentAST, antlr::RefAST(ff_AST));
		match(CF);
		ff_AST->setW( w); ff_AST->setD( d); ff_AST->setType( F);
		break;
	}
	case CG:
	{
		g = LT(1);
		g_AST = astFactory->create(g);
		astFactory->addASTChild(currentAST, antlr::RefAST(g_AST));
		match(CG);
		g_AST->setW( w); g_AST->setD( d); g_AST->setType( G);
		break;
	}
	case CO:
	{
		o = LT(1);
		o_AST = astFactory->create(o);
		astFactory->addASTChild(currentAST, antlr::RefAST(o_AST));
		match(CO);
		o_AST->setW( w); o_AST->setD( d); o_AST->setType( O);
		break;
	}
	case CX:
	{
		x = LT(1);
		x_AST = astFactory->create(x);
		astFactory->addASTChild(currentAST, antlr::RefAST(x_AST));
		match(CX);
		x_AST->setW( w); x_AST->setD( d); x_AST->setType( Z);
		break;
	}
	case CZ:
	{
		z = LT(1);
		z_AST = astFactory->create(z);
		astFactory->addASTChild(currentAST, antlr::RefAST(z_AST));
		match(CZ);
		z_AST->setW( w); z_AST->setD( d); z_AST->setType( Z);
		break;
	}
	case CS:
	{
		s = LT(1);
		s_AST = astFactory->create(s);
		astFactory->addASTChild(currentAST, antlr::RefAST(s_AST));
		match(CS);
		s_AST->setW( w); s_AST->setType( A);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	cformat_AST = RefFMTNode(currentAST.root);
	returnAST = cformat_AST;
}

 int  FMTParser::cnn() {
	 int n;
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode cnn_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  num = antlr::nullToken;
	RefFMTNode num_AST = RefFMTNode(antlr::nullAST);
	
	num = LT(1);
	num_AST = astFactory->create(num);
	match(CNUMBER);
	
	std::istringstream s(num_AST->getText());
	s >> n;
	
	returnAST = cnn_AST;
	return n;
}

void FMTParser::rep_fmt(
	 int repeat
) {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode rep_fmt_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  a = antlr::nullToken;
	RefFMTNode a_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  ff = antlr::nullToken;
	RefFMTNode ff_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  d = antlr::nullToken;
	RefFMTNode d_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  e = antlr::nullToken;
	RefFMTNode e_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  g = antlr::nullToken;
	RefFMTNode g_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  i = antlr::nullToken;
	RefFMTNode i_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  o = antlr::nullToken;
	RefFMTNode o_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  z = antlr::nullToken;
	RefFMTNode z_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  zz = antlr::nullToken;
	RefFMTNode zz_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c = antlr::nullToken;
	RefFMTNode c_AST = RefFMTNode(antlr::nullAST);
	
	int n1;
	
	
	switch ( LA(1)) {
	case LBRACE:
	{
		format( repeat);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		rep_fmt_AST = RefFMTNode(currentAST.root);
		break;
	}
	case A:
	{
		a = LT(1);
		a_AST = astFactory->create(a);
		astFactory->addASTChild(currentAST, antlr::RefAST(a_AST));
		match(A);
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			n1=nn();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			a_AST->setW( n1);
			break;
		}
		case COMMA:
		case RBRACE:
		case SLASH:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		a_AST->setRep( repeat);
		rep_fmt_AST = RefFMTNode(currentAST.root);
		break;
	}
	case F:
	{
		ff = LT(1);
		ff_AST = astFactory->create(ff);
		astFactory->addASTChild(currentAST, antlr::RefAST(ff_AST));
		match(F);
		w_d( ff_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		ff_AST->setRep( repeat);
		rep_fmt_AST = RefFMTNode(currentAST.root);
		break;
	}
	case D:
	{
		d = LT(1);
		d_AST = astFactory->create(d);
		astFactory->addASTChild(currentAST, antlr::RefAST(d_AST));
		match(D);
		w_d( d_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		d_AST->setRep( repeat); d_AST->setText("f"); d_AST->setType(F);
		rep_fmt_AST = RefFMTNode(currentAST.root);
		break;
	}
	case E:
	{
		e = LT(1);
		e_AST = astFactory->create(e);
		astFactory->addASTChild(currentAST, antlr::RefAST(e_AST));
		match(E);
		w_d_e( e_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		e_AST->setRep( repeat);
		rep_fmt_AST = RefFMTNode(currentAST.root);
		break;
	}
	case G:
	{
		g = LT(1);
		g_AST = astFactory->create(g);
		astFactory->addASTChild(currentAST, antlr::RefAST(g_AST));
		match(G);
		w_d_e( g_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		g_AST->setRep( repeat);
		rep_fmt_AST = RefFMTNode(currentAST.root);
		break;
	}
	case I:
	{
		i = LT(1);
		i_AST = astFactory->create(i);
		astFactory->addASTChild(currentAST, antlr::RefAST(i_AST));
		match(I);
		w_d( i_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		i_AST->setRep( repeat);
		rep_fmt_AST = RefFMTNode(currentAST.root);
		break;
	}
	case O:
	{
		o = LT(1);
		o_AST = astFactory->create(o);
		astFactory->addASTChild(currentAST, antlr::RefAST(o_AST));
		match(O);
		w_d( o_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		o_AST->setRep( repeat);
		rep_fmt_AST = RefFMTNode(currentAST.root);
		break;
	}
	case Z:
	{
		z = LT(1);
		z_AST = astFactory->create(z);
		astFactory->addASTChild(currentAST, antlr::RefAST(z_AST));
		match(Z);
		w_d( z_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		z_AST->setRep( repeat);
		rep_fmt_AST = RefFMTNode(currentAST.root);
		break;
	}
	case ZZ:
	{
		zz = LT(1);
		zz_AST = astFactory->create(zz);
		astFactory->addASTChild(currentAST, antlr::RefAST(zz_AST));
		match(ZZ);
		w_d( zz_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		zz_AST->setRep( repeat);
		rep_fmt_AST = RefFMTNode(currentAST.root);
		break;
	}
	case C:
	{
		c = LT(1);
		c_AST = astFactory->create(c);
		astFactory->makeASTRoot(currentAST, antlr::RefAST(c_AST));
		match(C);
		match(RBRACE);
		csub();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		match(LBRACE);
		c_AST->setRep( repeat);
		rep_fmt_AST = RefFMTNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = rep_fmt_AST;
}

void FMTParser::w_d(
	 RefFMTNode fNode
) {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode w_d_AST = RefFMTNode(antlr::nullAST);
	
	int n1, n2;
	fNode->setW( -1);
	fNode->setD( -1);
	
	
	{
	switch ( LA(1)) {
	case NUMBER:
	{
		n1=nn();
		fNode->setW( n1);
		{
		switch ( LA(1)) {
		case DOT:
		{
			RefFMTNode tmp12_AST = RefFMTNode(antlr::nullAST);
			tmp12_AST = astFactory->create(LT(1));
			match(DOT);
			n2=nn();
			fNode->setD( n2);
			break;
		}
		case LBRACE:
		case COMMA:
		case RBRACE:
		case SLASH:
		case E:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		break;
	}
	case LBRACE:
	case COMMA:
	case RBRACE:
	case SLASH:
	case E:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	returnAST = w_d_AST;
}

void FMTParser::w_d_e(
	 RefFMTNode fNode
) {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode w_d_e_AST = RefFMTNode(antlr::nullAST);
	
	{
	if ((_tokenSet_0.member(LA(1)))) {
		w_d( fNode);
		{
		switch ( LA(1)) {
		case E:
		{
			RefFMTNode tmp13_AST = RefFMTNode(antlr::nullAST);
			tmp13_AST = astFactory->create(LT(1));
			match(E);
			RefFMTNode tmp14_AST = RefFMTNode(antlr::nullAST);
			tmp14_AST = astFactory->create(LT(1));
			match(NUMBER);
			break;
		}
		case COMMA:
		case RBRACE:
		case SLASH:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	else if (((LA(1) >= COMMA && LA(1) <= SLASH))) {
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	}
	returnAST = w_d_e_AST;
}

void FMTParser::csub() {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode csub_AST = RefFMTNode(antlr::nullAST);
	
	csubcode();
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			csubcode();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		else {
			goto _loop24;
		}
		
	}
	_loop24:;
	} // ( ... )*
	csub_AST = RefFMTNode(currentAST.root);
	returnAST = csub_AST;
}

void FMTParser::csubcode() {
	returnAST = RefFMTNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefFMTNode csubcode_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c1 = antlr::nullToken;
	RefFMTNode c1_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c2 = antlr::nullToken;
	RefFMTNode c2_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c3 = antlr::nullToken;
	RefFMTNode c3_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c4 = antlr::nullToken;
	RefFMTNode c4_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c5 = antlr::nullToken;
	RefFMTNode c5_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c6 = antlr::nullToken;
	RefFMTNode c6_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c7 = antlr::nullToken;
	RefFMTNode c7_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c8 = antlr::nullToken;
	RefFMTNode c8_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c9 = antlr::nullToken;
	RefFMTNode c9_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c10 = antlr::nullToken;
	RefFMTNode c10_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c11 = antlr::nullToken;
	RefFMTNode c11_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c12 = antlr::nullToken;
	RefFMTNode c12_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c13 = antlr::nullToken;
	RefFMTNode c13_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c14 = antlr::nullToken;
	RefFMTNode c14_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c15 = antlr::nullToken;
	RefFMTNode c15_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c16 = antlr::nullToken;
	RefFMTNode c16_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  c17 = antlr::nullToken;
	RefFMTNode c17_AST = RefFMTNode(antlr::nullAST);
	antlr::RefToken  x = antlr::nullToken;
	RefFMTNode x_AST = RefFMTNode(antlr::nullAST);
	
	int n1;
	
	
	switch ( LA(1)) {
	case CMOA:
	{
		c1 = LT(1);
		c1_AST = astFactory->create(c1);
		astFactory->addASTChild(currentAST, antlr::RefAST(c1_AST));
		match(CMOA);
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			n1=nn();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			c1_AST->setW( n1);
			break;
		}
		case LBRACE:
		case COMMA:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CMoA:
	{
		c2 = LT(1);
		c2_AST = astFactory->create(c2);
		astFactory->addASTChild(currentAST, antlr::RefAST(c2_AST));
		match(CMoA);
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			n1=nn();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			c2_AST->setW( n1);
			break;
		}
		case LBRACE:
		case COMMA:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CmoA:
	{
		c3 = LT(1);
		c3_AST = astFactory->create(c3);
		astFactory->addASTChild(currentAST, antlr::RefAST(c3_AST));
		match(CmoA);
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			n1=nn();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			c3_AST->setW( n1);
			break;
		}
		case LBRACE:
		case COMMA:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CHI:
	{
		c4 = LT(1);
		c4_AST = astFactory->create(c4);
		astFactory->addASTChild(currentAST, antlr::RefAST(c4_AST));
		match(CHI);
		w_d( c4_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case ChI:
	{
		c5 = LT(1);
		c5_AST = astFactory->create(c5);
		astFactory->addASTChild(currentAST, antlr::RefAST(c5_AST));
		match(ChI);
		w_d( c5_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CDWA:
	{
		c6 = LT(1);
		c6_AST = astFactory->create(c6);
		astFactory->addASTChild(currentAST, antlr::RefAST(c6_AST));
		match(CDWA);
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			n1=nn();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			c6_AST->setW( n1);
			break;
		}
		case LBRACE:
		case COMMA:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CDwA:
	{
		c7 = LT(1);
		c7_AST = astFactory->create(c7);
		astFactory->addASTChild(currentAST, antlr::RefAST(c7_AST));
		match(CDwA);
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			n1=nn();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			c7_AST->setW( n1);
			break;
		}
		case LBRACE:
		case COMMA:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CdwA:
	{
		c8 = LT(1);
		c8_AST = astFactory->create(c8);
		astFactory->addASTChild(currentAST, antlr::RefAST(c8_AST));
		match(CdwA);
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			n1=nn();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			c8_AST->setW( n1);
			break;
		}
		case LBRACE:
		case COMMA:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CAPA:
	{
		c9 = LT(1);
		c9_AST = astFactory->create(c9);
		astFactory->addASTChild(currentAST, antlr::RefAST(c9_AST));
		match(CAPA);
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			n1=nn();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			c9_AST->setW( n1);
			break;
		}
		case LBRACE:
		case COMMA:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CApA:
	{
		c10 = LT(1);
		c10_AST = astFactory->create(c10);
		astFactory->addASTChild(currentAST, antlr::RefAST(c10_AST));
		match(CApA);
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			n1=nn();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			c10_AST->setW( n1);
			break;
		}
		case LBRACE:
		case COMMA:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CapA:
	{
		c11 = LT(1);
		c11_AST = astFactory->create(c11);
		astFactory->addASTChild(currentAST, antlr::RefAST(c11_AST));
		match(CapA);
		{
		switch ( LA(1)) {
		case NUMBER:
		{
			n1=nn();
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			c11_AST->setW( n1);
			break;
		}
		case LBRACE:
		case COMMA:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CMOI:
	{
		c12 = LT(1);
		c12_AST = astFactory->create(c12);
		astFactory->addASTChild(currentAST, antlr::RefAST(c12_AST));
		match(CMOI);
		w_d( c12_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CDI:
	{
		c13 = LT(1);
		c13_AST = astFactory->create(c13);
		astFactory->addASTChild(currentAST, antlr::RefAST(c13_AST));
		match(CDI);
		w_d( c13_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CYI:
	{
		c14 = LT(1);
		c14_AST = astFactory->create(c14);
		astFactory->addASTChild(currentAST, antlr::RefAST(c14_AST));
		match(CYI);
		w_d( c14_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CMI:
	{
		c15 = LT(1);
		c15_AST = astFactory->create(c15);
		astFactory->addASTChild(currentAST, antlr::RefAST(c15_AST));
		match(CMI);
		w_d( c15_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CSI:
	{
		c16 = LT(1);
		c16_AST = astFactory->create(c16);
		astFactory->addASTChild(currentAST, antlr::RefAST(c16_AST));
		match(CSI);
		w_d( c16_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CSF:
	{
		c17 = LT(1);
		c17_AST = astFactory->create(c17);
		astFactory->addASTChild(currentAST, antlr::RefAST(c17_AST));
		match(CSF);
		w_d( c17_AST);
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case NUMBER:
	{
		n1=nn();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		x = LT(1);
		x_AST = astFactory->create(x);
		astFactory->addASTChild(currentAST, antlr::RefAST(x_AST));
		match(X);
		x_AST->setW( n1);
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	case CSTR:
	case CD:
	case CE:
	case CI:
	case CF:
	case CG:
	case CO:
	case CS:
	case CX:
	case CZ:
	case CNUMBER:
	case STRING:
	case TL:
	case TR:
	{
		f_csubcode();
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		csubcode_AST = RefFMTNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = csubcode_AST;
}

void FMTParser::initializeASTFactory( antlr::ASTFactory& factory )
{
	factory.setMaxNodeType(75);
}
const char* FMTParser::tokenNames[] = {
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

const unsigned long FMTParser::_tokenSet_0_data_[] = { 1879048192UL, 1024UL, 4UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// COMMA RBRACE SLASH E NUMBER 
const antlr::BitSet FMTParser::_tokenSet_0(_tokenSet_0_data_,8);


