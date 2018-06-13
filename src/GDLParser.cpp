/* $ANTLR 2.7.7 (2006-11-01): "gdlc.g" -> "GDLParser.cpp"$ */

#include "includefirst.hpp"

#include "GDLParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>

#include <errno.h>

#include <cstdlib>

GDLParser::GDLParser(antlr::TokenBuffer& tokenBuf, int k)
: antlr::LLkParser(tokenBuf,k)
{
}

GDLParser::GDLParser(antlr::TokenBuffer& tokenBuf)
: antlr::LLkParser(tokenBuf,2)
{
}

GDLParser::GDLParser(antlr::TokenStream& lexer, int k)
: antlr::LLkParser(lexer,k)
{
}

GDLParser::GDLParser(antlr::TokenStream& lexer)
: antlr::LLkParser(lexer,2)
{
}

GDLParser::GDLParser(const antlr::ParserSharedInputState& state)
: antlr::LLkParser(state,2)
{
}

void GDLParser::identifier() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode identifier_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  a = antlr::nullToken;
	RefDNode a_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  b = antlr::nullToken;
	RefDNode b_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c = antlr::nullToken;
	RefDNode c_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  co = antlr::nullToken;
	RefDNode co_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  com = antlr::nullToken;
	RefDNode com_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  d = antlr::nullToken;
	RefDNode d_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  e = antlr::nullToken;
	RefDNode e_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  en = antlr::nullToken;
	RefDNode en_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  end = antlr::nullToken;
	RefDNode end_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  ende = antlr::nullToken;
	RefDNode ende_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  endf = antlr::nullToken;
	RefDNode endf_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  endfe = antlr::nullToken;
	RefDNode endfe_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  endi = antlr::nullToken;
	RefDNode endi_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  endr = antlr::nullToken;
	RefDNode endr_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  ends = antlr::nullToken;
	RefDNode ends_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  endw = antlr::nullToken;
	RefDNode endw_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  eq = antlr::nullToken;
	RefDNode eq_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  f = antlr::nullToken;
	RefDNode f_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  fe = antlr::nullToken;
	RefDNode fe_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  fo = antlr::nullToken;
	RefDNode fo_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  fu = antlr::nullToken;
	RefDNode fu_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  g = antlr::nullToken;
	RefDNode g_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  go = antlr::nullToken;
	RefDNode go_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  gt = antlr::nullToken;
	RefDNode gt_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  i = antlr::nullToken;
	RefDNode i_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  in = antlr::nullToken;
	RefDNode in_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  l = antlr::nullToken;
	RefDNode l_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  lt = antlr::nullToken;
	RefDNode lt_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  m = antlr::nullToken;
	RefDNode m_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  n = antlr::nullToken;
	RefDNode n_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  no = antlr::nullToken;
	RefDNode no_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  o = antlr::nullToken;
	RefDNode o_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  on = antlr::nullToken;
	RefDNode on_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  o_ = antlr::nullToken;
	RefDNode o__AST = RefDNode(antlr::nullAST);
	antlr::RefToken  p = antlr::nullToken;
	RefDNode p_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  r = antlr::nullToken;
	RefDNode r_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  s = antlr::nullToken;
	RefDNode s_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  t = antlr::nullToken;
	RefDNode t_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  u = antlr::nullToken;
	RefDNode u_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  w = antlr::nullToken;
	RefDNode w_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  x = antlr::nullToken;
	RefDNode x_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case IDENTIFIER:
	{
		RefDNode tmp1_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp1_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp1_AST));
		}
		match(IDENTIFIER);
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case AND_OP:
	{
		a = LT(1);
		if ( inputState->guessing == 0 ) {
			a_AST = astFactory->create(a);
			astFactory->addASTChild(currentAST, antlr::RefAST(a_AST));
		}
		match(AND_OP);
		if ( inputState->guessing==0 ) {
			a_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case BEGIN:
	{
		b = LT(1);
		if ( inputState->guessing == 0 ) {
			b_AST = astFactory->create(b);
			astFactory->addASTChild(currentAST, antlr::RefAST(b_AST));
		}
		match(BEGIN);
		if ( inputState->guessing==0 ) {
			b_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case CASE:
	{
		c = LT(1);
		if ( inputState->guessing == 0 ) {
			c_AST = astFactory->create(c);
			astFactory->addASTChild(currentAST, antlr::RefAST(c_AST));
		}
		match(CASE);
		if ( inputState->guessing==0 ) {
			c_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case COMMON:
	{
		co = LT(1);
		if ( inputState->guessing == 0 ) {
			co_AST = astFactory->create(co);
			astFactory->addASTChild(currentAST, antlr::RefAST(co_AST));
		}
		match(COMMON);
		if ( inputState->guessing==0 ) {
			co_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case COMPILE_OPT:
	{
		com = LT(1);
		if ( inputState->guessing == 0 ) {
			com_AST = astFactory->create(com);
			astFactory->addASTChild(currentAST, antlr::RefAST(com_AST));
		}
		match(COMPILE_OPT);
		if ( inputState->guessing==0 ) {
			com_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case DO:
	{
		d = LT(1);
		if ( inputState->guessing == 0 ) {
			d_AST = astFactory->create(d);
			astFactory->addASTChild(currentAST, antlr::RefAST(d_AST));
		}
		match(DO);
		if ( inputState->guessing==0 ) {
			d_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case ELSE:
	{
		e = LT(1);
		if ( inputState->guessing == 0 ) {
			e_AST = astFactory->create(e);
			astFactory->addASTChild(currentAST, antlr::RefAST(e_AST));
		}
		match(ELSE);
		if ( inputState->guessing==0 ) {
			e_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case END:
	{
		en = LT(1);
		if ( inputState->guessing == 0 ) {
			en_AST = astFactory->create(en);
			astFactory->addASTChild(currentAST, antlr::RefAST(en_AST));
		}
		match(END);
		if ( inputState->guessing==0 ) {
			en_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case ENDCASE:
	{
		end = LT(1);
		if ( inputState->guessing == 0 ) {
			end_AST = astFactory->create(end);
			astFactory->addASTChild(currentAST, antlr::RefAST(end_AST));
		}
		match(ENDCASE);
		if ( inputState->guessing==0 ) {
			end_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case ENDELSE:
	{
		ende = LT(1);
		if ( inputState->guessing == 0 ) {
			ende_AST = astFactory->create(ende);
			astFactory->addASTChild(currentAST, antlr::RefAST(ende_AST));
		}
		match(ENDELSE);
		if ( inputState->guessing==0 ) {
			ende_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case ENDFOR:
	{
		endf = LT(1);
		if ( inputState->guessing == 0 ) {
			endf_AST = astFactory->create(endf);
			astFactory->addASTChild(currentAST, antlr::RefAST(endf_AST));
		}
		match(ENDFOR);
		if ( inputState->guessing==0 ) {
			endf_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case ENDFOREACH:
	{
		endfe = LT(1);
		if ( inputState->guessing == 0 ) {
			endfe_AST = astFactory->create(endfe);
			astFactory->addASTChild(currentAST, antlr::RefAST(endfe_AST));
		}
		match(ENDFOREACH);
		if ( inputState->guessing==0 ) {
			endf_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case ENDIF:
	{
		endi = LT(1);
		if ( inputState->guessing == 0 ) {
			endi_AST = astFactory->create(endi);
			astFactory->addASTChild(currentAST, antlr::RefAST(endi_AST));
		}
		match(ENDIF);
		if ( inputState->guessing==0 ) {
			endi_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case ENDREP:
	{
		endr = LT(1);
		if ( inputState->guessing == 0 ) {
			endr_AST = astFactory->create(endr);
			astFactory->addASTChild(currentAST, antlr::RefAST(endr_AST));
		}
		match(ENDREP);
		if ( inputState->guessing==0 ) {
			endr_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case ENDSWITCH:
	{
		ends = LT(1);
		if ( inputState->guessing == 0 ) {
			ends_AST = astFactory->create(ends);
			astFactory->addASTChild(currentAST, antlr::RefAST(ends_AST));
		}
		match(ENDSWITCH);
		if ( inputState->guessing==0 ) {
			ends_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case ENDWHILE:
	{
		endw = LT(1);
		if ( inputState->guessing == 0 ) {
			endw_AST = astFactory->create(endw);
			astFactory->addASTChild(currentAST, antlr::RefAST(endw_AST));
		}
		match(ENDWHILE);
		if ( inputState->guessing==0 ) {
			endw_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case EQ_OP:
	{
		eq = LT(1);
		if ( inputState->guessing == 0 ) {
			eq_AST = astFactory->create(eq);
			astFactory->addASTChild(currentAST, antlr::RefAST(eq_AST));
		}
		match(EQ_OP);
		if ( inputState->guessing==0 ) {
			eq_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case FOR:
	{
		f = LT(1);
		if ( inputState->guessing == 0 ) {
			f_AST = astFactory->create(f);
			astFactory->addASTChild(currentAST, antlr::RefAST(f_AST));
		}
		match(FOR);
		if ( inputState->guessing==0 ) {
			f_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case FOREACH:
	{
		fe = LT(1);
		if ( inputState->guessing == 0 ) {
			fe_AST = astFactory->create(fe);
			astFactory->addASTChild(currentAST, antlr::RefAST(fe_AST));
		}
		match(FOREACH);
		if ( inputState->guessing==0 ) {
			f_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case FORWARD:
	{
		fo = LT(1);
		if ( inputState->guessing == 0 ) {
			fo_AST = astFactory->create(fo);
			astFactory->addASTChild(currentAST, antlr::RefAST(fo_AST));
		}
		match(FORWARD);
		if ( inputState->guessing==0 ) {
			fo_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case FUNCTION:
	{
		fu = LT(1);
		if ( inputState->guessing == 0 ) {
			fu_AST = astFactory->create(fu);
			astFactory->addASTChild(currentAST, antlr::RefAST(fu_AST));
		}
		match(FUNCTION);
		if ( inputState->guessing==0 ) {
			fu_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case GE_OP:
	{
		g = LT(1);
		if ( inputState->guessing == 0 ) {
			g_AST = astFactory->create(g);
			astFactory->addASTChild(currentAST, antlr::RefAST(g_AST));
		}
		match(GE_OP);
		if ( inputState->guessing==0 ) {
			g_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case GOTO:
	{
		go = LT(1);
		if ( inputState->guessing == 0 ) {
			go_AST = astFactory->create(go);
			astFactory->addASTChild(currentAST, antlr::RefAST(go_AST));
		}
		match(GOTO);
		if ( inputState->guessing==0 ) {
			go_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case GT_OP:
	{
		gt = LT(1);
		if ( inputState->guessing == 0 ) {
			gt_AST = astFactory->create(gt);
			astFactory->addASTChild(currentAST, antlr::RefAST(gt_AST));
		}
		match(GT_OP);
		if ( inputState->guessing==0 ) {
			gt_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case IF:
	{
		i = LT(1);
		if ( inputState->guessing == 0 ) {
			i_AST = astFactory->create(i);
			astFactory->addASTChild(currentAST, antlr::RefAST(i_AST));
		}
		match(IF);
		if ( inputState->guessing==0 ) {
			i_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case INHERITS:
	{
		in = LT(1);
		if ( inputState->guessing == 0 ) {
			in_AST = astFactory->create(in);
			astFactory->addASTChild(currentAST, antlr::RefAST(in_AST));
		}
		match(INHERITS);
		if ( inputState->guessing==0 ) {
			in_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case LE_OP:
	{
		l = LT(1);
		if ( inputState->guessing == 0 ) {
			l_AST = astFactory->create(l);
			astFactory->addASTChild(currentAST, antlr::RefAST(l_AST));
		}
		match(LE_OP);
		if ( inputState->guessing==0 ) {
			l_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case LT_OP:
	{
		lt = LT(1);
		if ( inputState->guessing == 0 ) {
			lt_AST = astFactory->create(lt);
			astFactory->addASTChild(currentAST, antlr::RefAST(lt_AST));
		}
		match(LT_OP);
		if ( inputState->guessing==0 ) {
			lt_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case MOD_OP:
	{
		m = LT(1);
		if ( inputState->guessing == 0 ) {
			m_AST = astFactory->create(m);
			astFactory->addASTChild(currentAST, antlr::RefAST(m_AST));
		}
		match(MOD_OP);
		if ( inputState->guessing==0 ) {
			m_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case NE_OP:
	{
		n = LT(1);
		if ( inputState->guessing == 0 ) {
			n_AST = astFactory->create(n);
			astFactory->addASTChild(currentAST, antlr::RefAST(n_AST));
		}
		match(NE_OP);
		if ( inputState->guessing==0 ) {
			n_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case NOT_OP:
	{
		no = LT(1);
		if ( inputState->guessing == 0 ) {
			no_AST = astFactory->create(no);
			astFactory->addASTChild(currentAST, antlr::RefAST(no_AST));
		}
		match(NOT_OP);
		if ( inputState->guessing==0 ) {
			no_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case OF:
	{
		o = LT(1);
		if ( inputState->guessing == 0 ) {
			o_AST = astFactory->create(o);
			astFactory->addASTChild(currentAST, antlr::RefAST(o_AST));
		}
		match(OF);
		if ( inputState->guessing==0 ) {
			o_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case ON_IOERROR:
	{
		on = LT(1);
		if ( inputState->guessing == 0 ) {
			on_AST = astFactory->create(on);
			astFactory->addASTChild(currentAST, antlr::RefAST(on_AST));
		}
		match(ON_IOERROR);
		if ( inputState->guessing==0 ) {
			on_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case OR_OP:
	{
		o_ = LT(1);
		if ( inputState->guessing == 0 ) {
			o__AST = astFactory->create(o_);
			astFactory->addASTChild(currentAST, antlr::RefAST(o__AST));
		}
		match(OR_OP);
		if ( inputState->guessing==0 ) {
			o__AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case PRO:
	{
		p = LT(1);
		if ( inputState->guessing == 0 ) {
			p_AST = astFactory->create(p);
			astFactory->addASTChild(currentAST, antlr::RefAST(p_AST));
		}
		match(PRO);
		if ( inputState->guessing==0 ) {
			p_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case REPEAT:
	{
		r = LT(1);
		if ( inputState->guessing == 0 ) {
			r_AST = astFactory->create(r);
			astFactory->addASTChild(currentAST, antlr::RefAST(r_AST));
		}
		match(REPEAT);
		if ( inputState->guessing==0 ) {
			r_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case SWITCH:
	{
		s = LT(1);
		if ( inputState->guessing == 0 ) {
			s_AST = astFactory->create(s);
			astFactory->addASTChild(currentAST, antlr::RefAST(s_AST));
		}
		match(SWITCH);
		if ( inputState->guessing==0 ) {
			s_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case THEN:
	{
		t = LT(1);
		if ( inputState->guessing == 0 ) {
			t_AST = astFactory->create(t);
			astFactory->addASTChild(currentAST, antlr::RefAST(t_AST));
		}
		match(THEN);
		if ( inputState->guessing==0 ) {
			t_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case UNTIL:
	{
		u = LT(1);
		if ( inputState->guessing == 0 ) {
			u_AST = astFactory->create(u);
			astFactory->addASTChild(currentAST, antlr::RefAST(u_AST));
		}
		match(UNTIL);
		if ( inputState->guessing==0 ) {
			u_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case WHILE:
	{
		w = LT(1);
		if ( inputState->guessing == 0 ) {
			w_AST = astFactory->create(w);
			astFactory->addASTChild(currentAST, antlr::RefAST(w_AST));
		}
		match(WHILE);
		if ( inputState->guessing==0 ) {
			w_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	case XOR_OP:
	{
		x = LT(1);
		if ( inputState->guessing == 0 ) {
			x_AST = astFactory->create(x);
			astFactory->addASTChild(currentAST, antlr::RefAST(x_AST));
		}
		match(XOR_OP);
		if ( inputState->guessing==0 ) {
			x_AST->setType( IDENTIFIER);
		}
		identifier_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = identifier_AST;
}

void GDLParser::translation_unit() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode translation_unit_AST = RefDNode(antlr::nullAST);
	
	subReached=false;
	compileOpt=NONE; // reset compileOpt    
	
	
	try {      // for error handling
		{ // ( ... )*
		for (;;) {
			switch ( LA(1)) {
			case PRO:
			{
				procedure_def();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				if ( inputState->guessing==0 ) {
					
					compileOpt=NONE; // reset compileOpt    
					if( subReached) goto bailOut;
					
				}
				break;
			}
			case FUNCTION:
			{
				function_def();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				if ( inputState->guessing==0 ) {
					
					compileOpt=NONE; // reset compileOpt    
					if( subReached) goto bailOut;
					
				}
				break;
			}
			default:
				if ((LA(1) == END_U) && (_tokenSet_0.member(LA(2)))) {
					end_unit();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
				}
				else if ((LA(1) == FORWARD) && (LA(2) == IDENTIFIER)) {
					forward_function();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					end_unit();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
				}
				else if ((LA(1) == COMMON) && (LA(2) == IDENTIFIER)) {
					common_block();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
				}
			else {
				goto _loop4;
			}
			}
		}
		_loop4:;
		} // ( ... )*
		{
		switch ( LA(1)) {
		case FOR:
		case FOREACH:
		case REPEAT:
		case WHILE:
		case IDENTIFIER:
		case BEGIN:
		case CASE:
		case COMMON:
		case COMPILE_OPT:
		case FORWARD:
		case GOTO:
		case IF:
		case INHERITS:
		case ON_IOERROR:
		case SWITCH:
		case END_U:
		case DEC:
		case INC:
		case LBRACE:
		case SYSVARNAME:
		case ASTERIX:
		{
			statement_list();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			match(END);
			{
			switch ( LA(1)) {
			case END_U:
			{
				end_unit();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				break;
			}
			case antlr::Token::EOF_TYPE:
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
		case antlr::Token::EOF_TYPE:
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
		match(antlr::Token::EOF_TYPE);
		}
		if ( inputState->guessing==0 ) {
			bailOut:;
		}
		translation_unit_AST = RefDNode(currentAST.root);
	}
	catch ( GDLException& e) {
		if (inputState->guessing==0) {
			
			throw;
			
		} else {
			throw;
		}
	}
	catch ( antlr::NoViableAltException& e) {
		if (inputState->guessing==0) {
			
			// PARSER SYNTAX ERROR
			throw GDLException( e.getLine(), e.getColumn(), "Parser syntax error: "+e.getMessage());
			
		} else {
			throw;
		}
	}
	catch ( antlr::NoViableAltForCharException& e) {
		if (inputState->guessing==0) {
			
			// LEXER SYNTAX ERROR
			throw GDLException( e.getLine(), e.getColumn(), "Lexer syntax error: "+e.getMessage());
			
		} else {
			throw;
		}
	}
	catch ( antlr::RecognitionException& e) {
		if (inputState->guessing==0) {
			
			// SYNTAX ERROR
			throw GDLException( e.getLine(), e.getColumn(), "Lexer/Parser syntax error: "+e.getMessage());
			
		} else {
			throw;
		}
	}
	catch ( antlr::TokenStreamIOException& e) {
		if (inputState->guessing==0) {
			
			// IO ERROR
			throw GDLException( returnAST, "Input/Output error: "+e.getMessage());
			
		} else {
			throw;
		}
	}
	catch ( antlr::TokenStreamException& e) {
		if (inputState->guessing==0) {
			
			throw GDLException( returnAST, "Token stream error: "+e.getMessage());
			
		} else {
			throw;
		}
	}
	returnAST = translation_unit_AST;
}

void GDLParser::end_unit() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode end_unit_AST = RefDNode(antlr::nullAST);
	
	{ // ( ... )+
	int _cnt35=0;
	for (;;) {
		if ((LA(1) == END_U) && (_tokenSet_1.member(LA(2)))) {
			RefDNode tmp4_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp4_AST = astFactory->create(LT(1));
			}
			match(END_U);
		}
		else {
			if ( _cnt35>=1 ) { goto _loop35; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt35++;
	}
	_loop35:;
	}  // ( ... )+
	returnAST = end_unit_AST;
}

void GDLParser::forward_function() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode forward_function_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp5_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp5_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp5_AST));
	}
	match(FORWARD);
	identifier_list();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	forward_function_AST = RefDNode(currentAST.root);
	returnAST = forward_function_AST;
}

void GDLParser::procedure_def() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode procedure_def_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  p = antlr::nullToken;
	RefDNode p_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  n = antlr::nullToken;
	RefDNode n_AST = RefDNode(antlr::nullAST);
	
	std::string name;
	
	
	p = LT(1);
	if ( inputState->guessing == 0 ) {
		p_AST = astFactory->create(p);
		astFactory->makeASTRoot(currentAST, antlr::RefAST(p_AST));
	}
	match(PRO);
	{
	if ((LA(1) == IDENTIFIER) && (LA(2) == COMMA || LA(2) == END_U)) {
		n = LT(1);
		if ( inputState->guessing == 0 ) {
			n_AST = astFactory->create(n);
			astFactory->addASTChild(currentAST, antlr::RefAST(n_AST));
		}
		match(IDENTIFIER);
		if ( inputState->guessing==0 ) {
			name=n->getText();
		}
	}
	else if ((LA(1) == IDENTIFIER) && (LA(2) == METHOD)) {
		name=object_name();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	}
	{
	switch ( LA(1)) {
	case COMMA:
	{
		match(COMMA);
		parameter_declaration();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case END_U:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	end_unit();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{
	switch ( LA(1)) {
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case BEGIN:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case FORWARD:
	case GOTO:
	case IF:
	case INHERITS:
	case ON_IOERROR:
	case SWITCH:
	case END_U:
	case DEC:
	case INC:
	case LBRACE:
	case SYSVARNAME:
	case ASTERIX:
	{
		statement_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case END:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(END);
	if ( inputState->guessing==0 ) {
		
		if( subName == name && searchForPro == true) subReached=true;
		p_AST->SetCompileOpt( compileOpt);
		
	}
	procedure_def_AST = RefDNode(currentAST.root);
	returnAST = procedure_def_AST;
}

void GDLParser::function_def() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode function_def_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  f = antlr::nullToken;
	RefDNode f_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  n = antlr::nullToken;
	RefDNode n_AST = RefDNode(antlr::nullAST);
	
	std::string name;
	
	
	f = LT(1);
	if ( inputState->guessing == 0 ) {
		f_AST = astFactory->create(f);
		astFactory->makeASTRoot(currentAST, antlr::RefAST(f_AST));
	}
	match(FUNCTION);
	{
	if ((LA(1) == IDENTIFIER) && (LA(2) == COMMA || LA(2) == END_U)) {
		n = LT(1);
		if ( inputState->guessing == 0 ) {
			n_AST = astFactory->create(n);
			astFactory->addASTChild(currentAST, antlr::RefAST(n_AST));
		}
		match(IDENTIFIER);
		if ( inputState->guessing==0 ) {
			name=n->getText();
		}
	}
	else if ((LA(1) == IDENTIFIER) && (LA(2) == METHOD)) {
		name=object_name();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	}
	{
	switch ( LA(1)) {
	case COMMA:
	{
		match(COMMA);
		parameter_declaration();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case END_U:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	end_unit();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{
	switch ( LA(1)) {
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case BEGIN:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case FORWARD:
	case GOTO:
	case IF:
	case INHERITS:
	case ON_IOERROR:
	case SWITCH:
	case END_U:
	case DEC:
	case INC:
	case LBRACE:
	case SYSVARNAME:
	case ASTERIX:
	{
		statement_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case END:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(END);
	if ( inputState->guessing==0 ) {
		
		if( subName == name && searchForPro == false) subReached=true;
		f_AST->SetCompileOpt( compileOpt);
		
	}
	function_def_AST = RefDNode(currentAST.root);
	returnAST = function_def_AST;
}

void GDLParser::common_block() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode common_block_AST = RefDNode(antlr::nullAST);
	
	match(COMMON);
	RefDNode tmp11_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp11_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp11_AST));
	}
	match(IDENTIFIER);
	{
	switch ( LA(1)) {
	case antlr::Token::EOF_TYPE:
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case BEGIN:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case ELSE:
	case FORWARD:
	case FUNCTION:
	case GOTO:
	case IF:
	case INHERITS:
	case ON_IOERROR:
	case PRO:
	case SWITCH:
	case UNTIL:
	case END_U:
	case DEC:
	case INC:
	case LBRACE:
	case SYSVARNAME:
	case ASTERIX:
	{
		if ( inputState->guessing==0 ) {
			common_block_AST = RefDNode(currentAST.root);
			common_block_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(COMMONDECL,"commondecl")))->add(antlr::RefAST(common_block_AST))));
			currentAST.root = common_block_AST;
			if ( common_block_AST!=RefDNode(antlr::nullAST) &&
				common_block_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = common_block_AST->getFirstChild();
			else
				currentAST.child = common_block_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case COMMA:
	{
		match(COMMA);
		identifier_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			common_block_AST = RefDNode(currentAST.root);
			common_block_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(COMMONDEF,"commondef")))->add(antlr::RefAST(common_block_AST))));
			currentAST.root = common_block_AST;
			if ( common_block_AST!=RefDNode(antlr::nullAST) &&
				common_block_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = common_block_AST->getFirstChild();
			else
				currentAST.child = common_block_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	common_block_AST = RefDNode(currentAST.root);
	returnAST = common_block_AST;
}

void GDLParser::statement_list() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode statement_list_AST = RefDNode(antlr::nullAST);
	
	{ // ( ... )+
	int _cnt73=0;
	for (;;) {
		if ((LA(1) == END_U)) {
			end_unit();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else if ((_tokenSet_2.member(LA(1))) && (_tokenSet_3.member(LA(2)))) {
			compound_statement();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			end_unit();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else if ((LA(1) == IDENTIFIER) && (LA(2) == COLON)) {
			label_statement();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			end_unit();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			if ( _cnt73>=1 ) { goto _loop73; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt73++;
	}
	_loop73:;
	}  // ( ... )+
	statement_list_AST = RefDNode(currentAST.root);
	returnAST = statement_list_AST;
}

void GDLParser::interactive_compile() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode interactive_compile_AST = RefDNode(antlr::nullAST);
	
	{
	switch ( LA(1)) {
	case FUNCTION:
	{
		RefDNode tmp13_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp13_AST = astFactory->create(LT(1));
		}
		match(FUNCTION);
		break;
	}
	case PRO:
	{
		RefDNode tmp14_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp14_AST = astFactory->create(LT(1));
		}
		match(PRO);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	RefDNode tmp15_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp15_AST = astFactory->create(LT(1));
	}
	match(IDENTIFIER);
	if ( inputState->guessing==0 ) {
		
		throw GDLException( "Programs can't be compiled from "
		"single statement mode.");
		
	}
	{
	switch ( LA(1)) {
	case METHOD:
	{
		RefDNode tmp16_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp16_AST = astFactory->create(LT(1));
		}
		match(METHOD);
		RefDNode tmp17_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp17_AST = astFactory->create(LT(1));
		}
		match(IDENTIFIER);
		break;
	}
	case COMMA:
	case END_U:
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
	case COMMA:
	{
		RefDNode tmp18_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp18_AST = astFactory->create(LT(1));
		}
		match(COMMA);
		parameter_declaration();
		break;
	}
	case END_U:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	end_unit();
	returnAST = interactive_compile_AST;
}

void GDLParser::parameter_declaration() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode parameter_declaration_AST = RefDNode(antlr::nullAST);
	
	{
	if ((LA(1) == IDENTIFIER) && (LA(2) == COMMA || LA(2) == END_U)) {
		RefDNode tmp19_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp19_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp19_AST));
		}
		match(IDENTIFIER);
	}
	else if ((LA(1) == IDENTIFIER) && (LA(2) == EQUAL)) {
		keyword_declaration();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			{
			if ((LA(1) == IDENTIFIER) && (LA(2) == COMMA || LA(2) == END_U)) {
				RefDNode tmp21_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp21_AST = astFactory->create(LT(1));
					astFactory->addASTChild(currentAST, antlr::RefAST(tmp21_AST));
				}
				match(IDENTIFIER);
			}
			else if ((LA(1) == IDENTIFIER) && (LA(2) == EQUAL)) {
				keyword_declaration();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
			}
			else {
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			
			}
		}
		else {
			goto _loop41;
		}
		
	}
	_loop41:;
	} // ( ... )*
	if ( inputState->guessing==0 ) {
		parameter_declaration_AST = RefDNode(currentAST.root);
		parameter_declaration_AST = 
					RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(PARADECL,"paradecl")))->add(antlr::RefAST(parameter_declaration_AST))));
		currentAST.root = parameter_declaration_AST;
		if ( parameter_declaration_AST!=RefDNode(antlr::nullAST) &&
			parameter_declaration_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = parameter_declaration_AST->getFirstChild();
		else
			currentAST.child = parameter_declaration_AST;
		currentAST.advanceChildToEnd();
	}
	parameter_declaration_AST = RefDNode(currentAST.root);
	returnAST = parameter_declaration_AST;
}

void GDLParser::interactive() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode interactive_AST = RefDNode(antlr::nullAST);
	
	try {      // for error handling
		{ // ( ... )+
		int _cnt15=0;
		for (;;) {
			switch ( LA(1)) {
			case END_U:
			{
				end_unit();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				{
				switch ( LA(1)) {
				case END:
				case ENDCASE:
				case ENDELSE:
				case ENDFOR:
				case ENDFOREACH:
				case ENDIF:
				case ENDREP:
				case ENDSWITCH:
				case ENDWHILE:
				{
					end_mark();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					break;
				}
				case antlr::Token::EOF_TYPE:
				case FOR:
				case FOREACH:
				case REPEAT:
				case WHILE:
				case IDENTIFIER:
				case BEGIN:
				case CASE:
				case COMMON:
				case COMPILE_OPT:
				case FORWARD:
				case FUNCTION:
				case GOTO:
				case IF:
				case INHERITS:
				case ON_IOERROR:
				case PRO:
				case SWITCH:
				case END_U:
				case DEC:
				case INC:
				case LBRACE:
				case SYSVARNAME:
				case ASTERIX:
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
			case FOR:
			case FOREACH:
			case REPEAT:
			case WHILE:
			case IDENTIFIER:
			case BEGIN:
			case CASE:
			case COMMON:
			case COMPILE_OPT:
			case FORWARD:
			case GOTO:
			case IF:
			case INHERITS:
			case ON_IOERROR:
			case SWITCH:
			case DEC:
			case INC:
			case LBRACE:
			case SYSVARNAME:
			case ASTERIX:
			{
				interactive_statement();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				break;
			}
			case FUNCTION:
			case PRO:
			{
				interactive_compile();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				break;
			}
			default:
			{
				if ( _cnt15>=1 ) { goto _loop15; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
			}
			}
			_cnt15++;
		}
		_loop15:;
		}  // ( ... )+
		interactive_AST = RefDNode(currentAST.root);
	}
	catch ( GDLException& e) {
		if (inputState->guessing==0) {
			
			throw;
			
		} else {
			throw;
		}
	}
	catch ( antlr::NoViableAltException& e) {
		if (inputState->guessing==0) {
			
			// PARSER SYNTAX ERROR
			throw GDLException( e.getLine(), e.getColumn(), "Parser syntax error: "+
			e.getMessage());
			
		} else {
			throw;
		}
	}
	catch ( antlr::NoViableAltForCharException& e) {
		if (inputState->guessing==0) {
			
			// LEXER SYNTAX ERROR
			throw GDLException( e.getLine(), e.getColumn(), "Lexer syntax error: "+
			e.getMessage());
			
		} else {
			throw;
		}
	}
	catch ( antlr::RecognitionException& e) {
		if (inputState->guessing==0) {
			
			// SYNTAX ERROR
			throw GDLException( e.getLine(), e.getColumn(), 
			"Lexer/Parser syntax error: "+e.getMessage());
			
		} else {
			throw;
		}
	}
	catch ( antlr::TokenStreamIOException& e) {
		if (inputState->guessing==0) {
			
			// IO ERROR
			throw GDLException( returnAST, "Input/Output error: "+e.getMessage());
			
		} else {
			throw;
		}
	}
	catch ( antlr::TokenStreamException& e) {
		if (inputState->guessing==0) {
			
			throw GDLException( returnAST, "Token stream error: "+e.getMessage());
			
		} else {
			throw;
		}
	}
	returnAST = interactive_AST;
}

void GDLParser::end_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode end_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case END:
	{
		RefDNode tmp22_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp22_AST = astFactory->create(LT(1));
		}
		match(END);
		break;
	}
	case ENDIF:
	{
		RefDNode tmp23_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp23_AST = astFactory->create(LT(1));
		}
		match(ENDIF);
		break;
	}
	case ENDELSE:
	{
		RefDNode tmp24_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp24_AST = astFactory->create(LT(1));
		}
		match(ENDELSE);
		break;
	}
	case ENDCASE:
	{
		RefDNode tmp25_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp25_AST = astFactory->create(LT(1));
		}
		match(ENDCASE);
		break;
	}
	case ENDSWITCH:
	{
		RefDNode tmp26_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp26_AST = astFactory->create(LT(1));
		}
		match(ENDSWITCH);
		break;
	}
	case ENDFOR:
	{
		RefDNode tmp27_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp27_AST = astFactory->create(LT(1));
		}
		match(ENDFOR);
		break;
	}
	case ENDFOREACH:
	{
		RefDNode tmp28_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp28_AST = astFactory->create(LT(1));
		}
		match(ENDFOREACH);
		break;
	}
	case ENDWHILE:
	{
		RefDNode tmp29_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp29_AST = astFactory->create(LT(1));
		}
		match(ENDWHILE);
		break;
	}
	case ENDREP:
	{
		RefDNode tmp30_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp30_AST = astFactory->create(LT(1));
		}
		match(ENDREP);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = end_mark_AST;
}

void GDLParser::interactive_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode interactive_statement_AST = RefDNode(antlr::nullAST);
	
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == BEGIN)) {
			match(BEGIN);
		}
		else if ((LA(1) == IDENTIFIER) && (LA(2) == COLON)) {
			match(IDENTIFIER);
			match(COLON);
		}
		else {
			goto _loop18;
		}
		
	}
	_loop18:;
	} // ( ... )*
	statement();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	end_unit();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	interactive_statement_AST = RefDNode(currentAST.root);
	returnAST = interactive_statement_AST;
}

void GDLParser::statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode statement_AST = RefDNode(antlr::nullAST);
	RefDNode d1_AST = RefDNode(antlr::nullAST);
	RefDNode d2_AST = RefDNode(antlr::nullAST);
	RefDNode d3_AST = RefDNode(antlr::nullAST);
	
	bool parent=false;
	
	
	switch ( LA(1)) {
	case DEC:
	case INC:
	{
		{
		switch ( LA(1)) {
		case DEC:
		{
			RefDNode tmp34_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp34_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp34_AST));
			}
			match(DEC);
			break;
		}
		case INC:
		{
			RefDNode tmp35_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp35_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp35_AST));
			}
			match(INC);
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case FOR:
	{
		for_statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case FOREACH:
	{
		foreach_statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case REPEAT:
	{
		repeat_statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case WHILE:
	{
		while_statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case GOTO:
	case ON_IOERROR:
	{
		jump_statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case IF:
	{
		if_statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case CASE:
	{
		case_statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case SWITCH:
	{
		switch_statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case FORWARD:
	{
		forward_function();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case COMMON:
	{
		common_block();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case COMPILE_OPT:
	{
		compile_opt();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	default:
		bool synPredMatched83 = false;
		if (((LA(1) == LBRACE) && (_tokenSet_4.member(LA(2))))) {
			int _m83 = mark();
			synPredMatched83 = true;
			inputState->guessing++;
			try {
				{
				assign_expr();
				}
			}
			catch (antlr::RecognitionException& pe) {
				synPredMatched83 = false;
			}
			rewind(_m83);
			inputState->guessing--;
		}
		if ( synPredMatched83 ) {
			assign_expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			{
			switch ( LA(1)) {
			case DEC:
			{
				RefDNode tmp36_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp36_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp36_AST));
				}
				match(DEC);
				break;
			}
			case INC:
			{
				RefDNode tmp37_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp37_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp37_AST));
				}
				match(INC);
				break;
			}
			case ELSE:
			case UNTIL:
			case END_U:
			{
				break;
			}
			default:
			{
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			}
			}
			statement_AST = RefDNode(currentAST.root);
		}
		else {
			bool synPredMatched86 = false;
			if (((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2))))) {
				int _m86 = mark();
				synPredMatched86 = true;
				inputState->guessing++;
				try {
					{
					deref_dot_expr_keeplast();
					match(IDENTIFIER);
					match(COMMA);
					}
				}
				catch (antlr::RecognitionException& pe) {
					synPredMatched86 = false;
				}
				rewind(_m86);
				inputState->guessing--;
			}
			if ( synPredMatched86 ) {
				deref_dot_expr_keeplast();
				if (inputState->guessing==0) {
					d1_AST = returnAST;
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				formal_procedure_call();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				if ( inputState->guessing==0 ) {
					statement_AST = RefDNode(currentAST.root);
					
					statement_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MPCALL,"mpcall")))->add(antlr::RefAST(statement_AST))));
					statement_AST->SetLine( d1_AST->getLine());
					
					
					currentAST.root = statement_AST;
					if ( statement_AST!=RefDNode(antlr::nullAST) &&
						statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
						  currentAST.child = statement_AST->getFirstChild();
					else
						currentAST.child = statement_AST;
					currentAST.advanceChildToEnd();
				}
				statement_AST = RefDNode(currentAST.root);
			}
			else {
				bool synPredMatched88 = false;
				if (((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2))))) {
					int _m88 = mark();
					synPredMatched88 = true;
					inputState->guessing++;
					try {
						{
						deref_dot_expr_keeplast();
						baseclass_method();
						}
					}
					catch (antlr::RecognitionException& pe) {
						synPredMatched88 = false;
					}
					rewind(_m88);
					inputState->guessing--;
				}
				if ( synPredMatched88 ) {
					deref_dot_expr_keeplast();
					if (inputState->guessing==0) {
						d2_AST = returnAST;
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					baseclass_method();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					formal_procedure_call();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					if ( inputState->guessing==0 ) {
						statement_AST = RefDNode(currentAST.root);
						
						statement_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MPCALL_PARENT,"mpcall::")))->add(antlr::RefAST(statement_AST))));
						statement_AST->SetLine( d2_AST->getLine());
						
						currentAST.root = statement_AST;
						if ( statement_AST!=RefDNode(antlr::nullAST) &&
							statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
							  currentAST.child = statement_AST->getFirstChild();
						else
							currentAST.child = statement_AST;
						currentAST.advanceChildToEnd();
					}
					statement_AST = RefDNode(currentAST.root);
				}
				else {
					bool synPredMatched91 = false;
					if (((_tokenSet_4.member(LA(1))) && (_tokenSet_6.member(LA(2))))) {
						int _m91 = mark();
						synPredMatched91 = true;
						inputState->guessing++;
						try {
							{
							deref_expr();
							{
							switch ( LA(1)) {
							case EQUAL:
							{
								match(EQUAL);
								break;
							}
							case AND_OP_EQ:
							{
								match(AND_OP_EQ);
								break;
							}
							case ASTERIX_EQ:
							{
								match(ASTERIX_EQ);
								break;
							}
							case EQ_OP_EQ:
							{
								match(EQ_OP_EQ);
								break;
							}
							case GE_OP_EQ:
							{
								match(GE_OP_EQ);
								break;
							}
							case GTMARK_EQ:
							{
								match(GTMARK_EQ);
								break;
							}
							case GT_OP_EQ:
							{
								match(GT_OP_EQ);
								break;
							}
							case LE_OP_EQ:
							{
								match(LE_OP_EQ);
								break;
							}
							case LTMARK_EQ:
							{
								match(LTMARK_EQ);
								break;
							}
							case LT_OP_EQ:
							{
								match(LT_OP_EQ);
								break;
							}
							case MATRIX_OP1_EQ:
							{
								match(MATRIX_OP1_EQ);
								break;
							}
							case MATRIX_OP2_EQ:
							{
								match(MATRIX_OP2_EQ);
								break;
							}
							case MINUS_EQ:
							{
								match(MINUS_EQ);
								break;
							}
							case MOD_OP_EQ:
							{
								match(MOD_OP_EQ);
								break;
							}
							case NE_OP_EQ:
							{
								match(NE_OP_EQ);
								break;
							}
							case OR_OP_EQ:
							{
								match(OR_OP_EQ);
								break;
							}
							case PLUS_EQ:
							{
								match(PLUS_EQ);
								break;
							}
							case POW_EQ:
							{
								match(POW_EQ);
								break;
							}
							case SLASH_EQ:
							{
								match(SLASH_EQ);
								break;
							}
							case XOR_OP_EQ:
							{
								match(XOR_OP_EQ);
								break;
							}
							case DEC:
							{
								match(DEC);
								break;
							}
							case INC:
							{
								match(INC);
								break;
							}
							case MEMBER:
							{
								match(MEMBER);
								break;
							}
							default:
							{
								throw antlr::NoViableAltException(LT(1), getFilename());
							}
							}
							}
							}
						}
						catch (antlr::RecognitionException& pe) {
							synPredMatched91 = false;
						}
						rewind(_m91);
						inputState->guessing--;
					}
					if ( synPredMatched91 ) {
						deref_expr();
						if (inputState->guessing==0) {
							astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
						}
						{
						switch ( LA(1)) {
						case EQUAL:
						{
							match(EQUAL);
							expr();
							if (inputState->guessing==0) {
								astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
							}
							if ( inputState->guessing==0 ) {
								statement_AST = RefDNode(currentAST.root);
								statement_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(statement_AST))));
								currentAST.root = statement_AST;
								if ( statement_AST!=RefDNode(antlr::nullAST) &&
									statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
									  currentAST.child = statement_AST->getFirstChild();
								else
									currentAST.child = statement_AST;
								currentAST.advanceChildToEnd();
							}
							break;
						}
						case AND_OP_EQ:
						case ASTERIX_EQ:
						case EQ_OP_EQ:
						case GE_OP_EQ:
						case GTMARK_EQ:
						case GT_OP_EQ:
						case LE_OP_EQ:
						case LTMARK_EQ:
						case LT_OP_EQ:
						case MATRIX_OP1_EQ:
						case MATRIX_OP2_EQ:
						case MINUS_EQ:
						case MOD_OP_EQ:
						case NE_OP_EQ:
						case OR_OP_EQ:
						case PLUS_EQ:
						case POW_EQ:
						case SLASH_EQ:
						case XOR_OP_EQ:
						{
							{
							switch ( LA(1)) {
							case AND_OP_EQ:
							{
								RefDNode tmp39_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp39_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp39_AST));
								}
								match(AND_OP_EQ);
								break;
							}
							case ASTERIX_EQ:
							{
								RefDNode tmp40_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp40_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp40_AST));
								}
								match(ASTERIX_EQ);
								break;
							}
							case EQ_OP_EQ:
							{
								RefDNode tmp41_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp41_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp41_AST));
								}
								match(EQ_OP_EQ);
								break;
							}
							case GE_OP_EQ:
							{
								RefDNode tmp42_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp42_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp42_AST));
								}
								match(GE_OP_EQ);
								break;
							}
							case GTMARK_EQ:
							{
								RefDNode tmp43_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp43_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp43_AST));
								}
								match(GTMARK_EQ);
								break;
							}
							case GT_OP_EQ:
							{
								RefDNode tmp44_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp44_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp44_AST));
								}
								match(GT_OP_EQ);
								break;
							}
							case LE_OP_EQ:
							{
								RefDNode tmp45_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp45_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp45_AST));
								}
								match(LE_OP_EQ);
								break;
							}
							case LTMARK_EQ:
							{
								RefDNode tmp46_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp46_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp46_AST));
								}
								match(LTMARK_EQ);
								break;
							}
							case LT_OP_EQ:
							{
								RefDNode tmp47_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp47_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp47_AST));
								}
								match(LT_OP_EQ);
								break;
							}
							case MATRIX_OP1_EQ:
							{
								RefDNode tmp48_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp48_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp48_AST));
								}
								match(MATRIX_OP1_EQ);
								break;
							}
							case MATRIX_OP2_EQ:
							{
								RefDNode tmp49_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp49_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp49_AST));
								}
								match(MATRIX_OP2_EQ);
								break;
							}
							case MINUS_EQ:
							{
								RefDNode tmp50_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp50_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp50_AST));
								}
								match(MINUS_EQ);
								break;
							}
							case MOD_OP_EQ:
							{
								RefDNode tmp51_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp51_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp51_AST));
								}
								match(MOD_OP_EQ);
								break;
							}
							case NE_OP_EQ:
							{
								RefDNode tmp52_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp52_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp52_AST));
								}
								match(NE_OP_EQ);
								break;
							}
							case OR_OP_EQ:
							{
								RefDNode tmp53_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp53_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp53_AST));
								}
								match(OR_OP_EQ);
								break;
							}
							case PLUS_EQ:
							{
								RefDNode tmp54_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp54_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp54_AST));
								}
								match(PLUS_EQ);
								break;
							}
							case POW_EQ:
							{
								RefDNode tmp55_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp55_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp55_AST));
								}
								match(POW_EQ);
								break;
							}
							case SLASH_EQ:
							{
								RefDNode tmp56_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp56_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp56_AST));
								}
								match(SLASH_EQ);
								break;
							}
							case XOR_OP_EQ:
							{
								RefDNode tmp57_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp57_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp57_AST));
								}
								match(XOR_OP_EQ);
								break;
							}
							default:
							{
								throw antlr::NoViableAltException(LT(1), getFilename());
							}
							}
							}
							expr();
							if (inputState->guessing==0) {
								astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
							}
							break;
						}
						case DEC:
						case INC:
						{
							{
							switch ( LA(1)) {
							case DEC:
							{
								RefDNode tmp58_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp58_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp58_AST));
								}
								match(DEC);
								break;
							}
							case INC:
							{
								RefDNode tmp59_AST = RefDNode(antlr::nullAST);
								if ( inputState->guessing == 0 ) {
									tmp59_AST = astFactory->create(LT(1));
									astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp59_AST));
								}
								match(INC);
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
						case MEMBER:
						{
							match(MEMBER);
							{
							if ((LA(1) == IDENTIFIER) && (LA(2) == METHOD)) {
								baseclass_method();
								if (inputState->guessing==0) {
									astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
								}
								if ( inputState->guessing==0 ) {
									parent=true;
								}
							}
							else if ((LA(1) == IDENTIFIER) && (_tokenSet_7.member(LA(2)))) {
							}
							else {
								throw antlr::NoViableAltException(LT(1), getFilename());
							}
							
							}
							formal_procedure_call();
							if (inputState->guessing==0) {
								astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
							}
							if ( inputState->guessing==0 ) {
								statement_AST = RefDNode(currentAST.root);
								
								if( parent)
								statement_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MPCALL_PARENT,"mpcall::")))->add(antlr::RefAST(statement_AST))));
								else
								statement_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MPCALL,"mpcall")))->add(antlr::RefAST(statement_AST))));
								
								currentAST.root = statement_AST;
								if ( statement_AST!=RefDNode(antlr::nullAST) &&
									statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
									  currentAST.child = statement_AST->getFirstChild();
								else
									currentAST.child = statement_AST;
								currentAST.advanceChildToEnd();
							}
							break;
						}
						default:
						{
							throw antlr::NoViableAltException(LT(1), getFilename());
						}
						}
						}
						statement_AST = RefDNode(currentAST.root);
					}
					else if ((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2)))) {
						deref_dot_expr_keeplast();
						if (inputState->guessing==0) {
							d3_AST = returnAST;
							astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
						}
						formal_procedure_call();
						if (inputState->guessing==0) {
							astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
						}
						if ( inputState->guessing==0 ) {
							statement_AST = RefDNode(currentAST.root);
							
							statement_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MPCALL,"mpcall")))->add(antlr::RefAST(statement_AST))));
							statement_AST->SetLine( d3_AST->getLine());
							
							currentAST.root = statement_AST;
							if ( statement_AST!=RefDNode(antlr::nullAST) &&
								statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
								  currentAST.child = statement_AST->getFirstChild();
							else
								currentAST.child = statement_AST;
							currentAST.advanceChildToEnd();
						}
						statement_AST = RefDNode(currentAST.root);
					}
					else if ((LA(1) == IDENTIFIER) && (_tokenSet_7.member(LA(2)))) {
						procedure_call();
						if (inputState->guessing==0) {
							astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
						}
						statement_AST = RefDNode(currentAST.root);
					}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}}}}
	returnAST = statement_AST;
}

void GDLParser::switch_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode switch_statement_AST = RefDNode(antlr::nullAST);
	
	int numBranch=0;
	
	
	RefDNode tmp61_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp61_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp61_AST));
	}
	match(SWITCH);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	match(OF);
	{
	switch ( LA(1)) {
	case END_U:
	{
		end_unit();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case IDENTIFIER:
	case ELSE:
	case END:
	case ENDSWITCH:
	case INHERITS:
	case NOT_OP:
	case DEC:
	case INC:
	case LBRACE:
	case LSQUARE:
	case SYSVARNAME:
	case LCURLY:
	case CONSTANT_HEX_BYTE:
	case CONSTANT_HEX_LONG:
	case CONSTANT_HEX_LONG64:
	case CONSTANT_HEX_INT:
	case CONSTANT_HEX_I:
	case CONSTANT_HEX_ULONG:
	case CONSTANT_HEX_ULONG64:
	case CONSTANT_HEX_UI:
	case CONSTANT_HEX_UINT:
	case CONSTANT_BYTE:
	case CONSTANT_LONG:
	case CONSTANT_LONG64:
	case CONSTANT_INT:
	case CONSTANT_I:
	case CONSTANT_ULONG:
	case CONSTANT_ULONG64:
	case CONSTANT_UI:
	case CONSTANT_UINT:
	case CONSTANT_OCT_BYTE:
	case CONSTANT_OCT_LONG:
	case CONSTANT_OCT_LONG64:
	case CONSTANT_OCT_INT:
	case CONSTANT_OCT_I:
	case CONSTANT_OCT_ULONG:
	case CONSTANT_OCT_ULONG64:
	case CONSTANT_OCT_UI:
	case CONSTANT_OCT_UINT:
	case CONSTANT_FLOAT:
	case CONSTANT_DOUBLE:
	case CONSTANT_BIN_BYTE:
	case CONSTANT_BIN_LONG:
	case CONSTANT_BIN_LONG64:
	case CONSTANT_BIN_INT:
	case CONSTANT_BIN_I:
	case CONSTANT_BIN_ULONG:
	case CONSTANT_BIN_ULONG64:
	case CONSTANT_BIN_UI:
	case CONSTANT_BIN_UINT:
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case LOG_NEG:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_8.member(LA(1)))) {
			switch_body();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			if ( inputState->guessing==0 ) {
				
				numBranch++;
				
			}
		}
		else {
			goto _loop22;
		}
		
	}
	_loop22:;
	} // ( ... )*
	endswitch_mark();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	if ( inputState->guessing==0 ) {
		
		tmp61_AST->SetNumBranch(numBranch);
		
	}
	switch_statement_AST = RefDNode(currentAST.root);
	returnAST = switch_statement_AST;
}

void GDLParser::expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode expr_AST = RefDNode(antlr::nullAST);
	
	logical_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{
	switch ( LA(1)) {
	case QUESTION:
	{
		RefDNode tmp63_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp63_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp63_AST));
		}
		match(QUESTION);
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		match(COLON);
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case antlr::Token::EOF_TYPE:
	case DO:
	case ELSE:
	case OF:
	case THEN:
	case UNTIL:
	case COMMA:
	case COLON:
	case END_U:
	case RBRACE:
	case RSQUARE:
	case RCURLY:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	expr_AST = RefDNode(currentAST.root);
	returnAST = expr_AST;
}

void GDLParser::switch_body() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode switch_body_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case IDENTIFIER:
	case INHERITS:
	case NOT_OP:
	case DEC:
	case INC:
	case LBRACE:
	case LSQUARE:
	case SYSVARNAME:
	case LCURLY:
	case CONSTANT_HEX_BYTE:
	case CONSTANT_HEX_LONG:
	case CONSTANT_HEX_LONG64:
	case CONSTANT_HEX_INT:
	case CONSTANT_HEX_I:
	case CONSTANT_HEX_ULONG:
	case CONSTANT_HEX_ULONG64:
	case CONSTANT_HEX_UI:
	case CONSTANT_HEX_UINT:
	case CONSTANT_BYTE:
	case CONSTANT_LONG:
	case CONSTANT_LONG64:
	case CONSTANT_INT:
	case CONSTANT_I:
	case CONSTANT_ULONG:
	case CONSTANT_ULONG64:
	case CONSTANT_UI:
	case CONSTANT_UINT:
	case CONSTANT_OCT_BYTE:
	case CONSTANT_OCT_LONG:
	case CONSTANT_OCT_LONG64:
	case CONSTANT_OCT_INT:
	case CONSTANT_OCT_I:
	case CONSTANT_OCT_ULONG:
	case CONSTANT_OCT_ULONG64:
	case CONSTANT_OCT_UI:
	case CONSTANT_OCT_UINT:
	case CONSTANT_FLOAT:
	case CONSTANT_DOUBLE:
	case CONSTANT_BIN_BYTE:
	case CONSTANT_BIN_LONG:
	case CONSTANT_BIN_LONG64:
	case CONSTANT_BIN_INT:
	case CONSTANT_BIN_I:
	case CONSTANT_BIN_ULONG:
	case CONSTANT_BIN_ULONG64:
	case CONSTANT_BIN_UI:
	case CONSTANT_BIN_UINT:
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case LOG_NEG:
	{
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		match(COLON);
		{
		switch ( LA(1)) {
		case FOR:
		case FOREACH:
		case REPEAT:
		case WHILE:
		case IDENTIFIER:
		case CASE:
		case COMMON:
		case COMPILE_OPT:
		case FORWARD:
		case GOTO:
		case IF:
		case INHERITS:
		case ON_IOERROR:
		case SWITCH:
		case DEC:
		case INC:
		case LBRACE:
		case SYSVARNAME:
		case ASTERIX:
		{
			statement();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			break;
		}
		case BEGIN:
		{
			match(BEGIN);
			statement_list();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			endswitch_mark();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			break;
		}
		case END_U:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		end_unit();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			switch_body_AST = RefDNode(currentAST.root);
			switch_body_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(switch_body_AST))));
			currentAST.root = switch_body_AST;
			if ( switch_body_AST!=RefDNode(antlr::nullAST) &&
				switch_body_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = switch_body_AST->getFirstChild();
			else
				currentAST.child = switch_body_AST;
			currentAST.advanceChildToEnd();
		}
		switch_body_AST = RefDNode(currentAST.root);
		break;
	}
	case ELSE:
	{
		match(ELSE);
		match(COLON);
		{
		switch ( LA(1)) {
		case FOR:
		case FOREACH:
		case REPEAT:
		case WHILE:
		case IDENTIFIER:
		case CASE:
		case COMMON:
		case COMPILE_OPT:
		case FORWARD:
		case GOTO:
		case IF:
		case INHERITS:
		case ON_IOERROR:
		case SWITCH:
		case DEC:
		case INC:
		case LBRACE:
		case SYSVARNAME:
		case ASTERIX:
		{
			statement();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			break;
		}
		case BEGIN:
		{
			match(BEGIN);
			statement_list();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			endswitchelse_mark();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			break;
		}
		case END_U:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		end_unit();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			switch_body_AST = RefDNode(currentAST.root);
			switch_body_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(ELSEBLK,"elseblk")))->add(antlr::RefAST(switch_body_AST))));
			currentAST.root = switch_body_AST;
			if ( switch_body_AST!=RefDNode(antlr::nullAST) &&
				switch_body_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = switch_body_AST->getFirstChild();
			else
				currentAST.child = switch_body_AST;
			currentAST.advanceChildToEnd();
		}
		switch_body_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = switch_body_AST;
}

void GDLParser::endswitch_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode endswitch_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case ENDSWITCH:
	{
		RefDNode tmp70_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp70_AST = astFactory->create(LT(1));
		}
		match(ENDSWITCH);
		break;
	}
	case END:
	{
		RefDNode tmp71_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp71_AST = astFactory->create(LT(1));
		}
		match(END);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = endswitch_mark_AST;
}

void GDLParser::endswitchelse_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode endswitchelse_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case END:
	case ENDSWITCH:
	{
		endswitch_mark();
		break;
	}
	case ENDELSE:
	{
		RefDNode tmp72_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp72_AST = astFactory->create(LT(1));
		}
		match(ENDELSE);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = endswitchelse_mark_AST;
}

void GDLParser::case_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode case_statement_AST = RefDNode(antlr::nullAST);
	
	int numBranch=0;
	
	
	RefDNode tmp73_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp73_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp73_AST));
	}
	match(CASE);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	match(OF);
	{
	switch ( LA(1)) {
	case END_U:
	{
		end_unit();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case IDENTIFIER:
	case ELSE:
	case END:
	case ENDCASE:
	case INHERITS:
	case NOT_OP:
	case DEC:
	case INC:
	case LBRACE:
	case LSQUARE:
	case SYSVARNAME:
	case LCURLY:
	case CONSTANT_HEX_BYTE:
	case CONSTANT_HEX_LONG:
	case CONSTANT_HEX_LONG64:
	case CONSTANT_HEX_INT:
	case CONSTANT_HEX_I:
	case CONSTANT_HEX_ULONG:
	case CONSTANT_HEX_ULONG64:
	case CONSTANT_HEX_UI:
	case CONSTANT_HEX_UINT:
	case CONSTANT_BYTE:
	case CONSTANT_LONG:
	case CONSTANT_LONG64:
	case CONSTANT_INT:
	case CONSTANT_I:
	case CONSTANT_ULONG:
	case CONSTANT_ULONG64:
	case CONSTANT_UI:
	case CONSTANT_UINT:
	case CONSTANT_OCT_BYTE:
	case CONSTANT_OCT_LONG:
	case CONSTANT_OCT_LONG64:
	case CONSTANT_OCT_INT:
	case CONSTANT_OCT_I:
	case CONSTANT_OCT_ULONG:
	case CONSTANT_OCT_ULONG64:
	case CONSTANT_OCT_UI:
	case CONSTANT_OCT_UINT:
	case CONSTANT_FLOAT:
	case CONSTANT_DOUBLE:
	case CONSTANT_BIN_BYTE:
	case CONSTANT_BIN_LONG:
	case CONSTANT_BIN_LONG64:
	case CONSTANT_BIN_INT:
	case CONSTANT_BIN_I:
	case CONSTANT_BIN_ULONG:
	case CONSTANT_BIN_ULONG64:
	case CONSTANT_BIN_UI:
	case CONSTANT_BIN_UINT:
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case LOG_NEG:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_8.member(LA(1)))) {
			case_body();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			if ( inputState->guessing==0 ) {
				
				numBranch++;
				
			}
		}
		else {
			goto _loop29;
		}
		
	}
	_loop29:;
	} // ( ... )*
	endcase_mark();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	if ( inputState->guessing==0 ) {
		
		tmp73_AST->SetNumBranch(numBranch);
		
	}
	case_statement_AST = RefDNode(currentAST.root);
	returnAST = case_statement_AST;
}

void GDLParser::case_body() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode case_body_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case IDENTIFIER:
	case INHERITS:
	case NOT_OP:
	case DEC:
	case INC:
	case LBRACE:
	case LSQUARE:
	case SYSVARNAME:
	case LCURLY:
	case CONSTANT_HEX_BYTE:
	case CONSTANT_HEX_LONG:
	case CONSTANT_HEX_LONG64:
	case CONSTANT_HEX_INT:
	case CONSTANT_HEX_I:
	case CONSTANT_HEX_ULONG:
	case CONSTANT_HEX_ULONG64:
	case CONSTANT_HEX_UI:
	case CONSTANT_HEX_UINT:
	case CONSTANT_BYTE:
	case CONSTANT_LONG:
	case CONSTANT_LONG64:
	case CONSTANT_INT:
	case CONSTANT_I:
	case CONSTANT_ULONG:
	case CONSTANT_ULONG64:
	case CONSTANT_UI:
	case CONSTANT_UINT:
	case CONSTANT_OCT_BYTE:
	case CONSTANT_OCT_LONG:
	case CONSTANT_OCT_LONG64:
	case CONSTANT_OCT_INT:
	case CONSTANT_OCT_I:
	case CONSTANT_OCT_ULONG:
	case CONSTANT_OCT_ULONG64:
	case CONSTANT_OCT_UI:
	case CONSTANT_OCT_UINT:
	case CONSTANT_FLOAT:
	case CONSTANT_DOUBLE:
	case CONSTANT_BIN_BYTE:
	case CONSTANT_BIN_LONG:
	case CONSTANT_BIN_LONG64:
	case CONSTANT_BIN_INT:
	case CONSTANT_BIN_I:
	case CONSTANT_BIN_ULONG:
	case CONSTANT_BIN_ULONG64:
	case CONSTANT_BIN_UI:
	case CONSTANT_BIN_UINT:
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case LOG_NEG:
	{
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		match(COLON);
		{
		switch ( LA(1)) {
		case FOR:
		case FOREACH:
		case REPEAT:
		case WHILE:
		case IDENTIFIER:
		case CASE:
		case COMMON:
		case COMPILE_OPT:
		case FORWARD:
		case GOTO:
		case IF:
		case INHERITS:
		case ON_IOERROR:
		case SWITCH:
		case DEC:
		case INC:
		case LBRACE:
		case SYSVARNAME:
		case ASTERIX:
		{
			statement();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			break;
		}
		case BEGIN:
		{
			match(BEGIN);
			statement_list();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			endcase_mark();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			break;
		}
		case END_U:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		end_unit();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			case_body_AST = RefDNode(currentAST.root);
			case_body_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(case_body_AST))));
			currentAST.root = case_body_AST;
			if ( case_body_AST!=RefDNode(antlr::nullAST) &&
				case_body_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = case_body_AST->getFirstChild();
			else
				currentAST.child = case_body_AST;
			currentAST.advanceChildToEnd();
		}
		case_body_AST = RefDNode(currentAST.root);
		break;
	}
	case ELSE:
	{
		match(ELSE);
		match(COLON);
		{
		switch ( LA(1)) {
		case FOR:
		case FOREACH:
		case REPEAT:
		case WHILE:
		case IDENTIFIER:
		case CASE:
		case COMMON:
		case COMPILE_OPT:
		case FORWARD:
		case GOTO:
		case IF:
		case INHERITS:
		case ON_IOERROR:
		case SWITCH:
		case DEC:
		case INC:
		case LBRACE:
		case SYSVARNAME:
		case ASTERIX:
		{
			statement();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			break;
		}
		case BEGIN:
		{
			match(BEGIN);
			statement_list();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			endcaseelse_mark();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			break;
		}
		case END_U:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		end_unit();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			case_body_AST = RefDNode(currentAST.root);
			case_body_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(ELSEBLK,"elseblk")))->add(antlr::RefAST(case_body_AST))));
			currentAST.root = case_body_AST;
			if ( case_body_AST!=RefDNode(antlr::nullAST) &&
				case_body_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = case_body_AST->getFirstChild();
			else
				currentAST.child = case_body_AST;
			currentAST.advanceChildToEnd();
		}
		case_body_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = case_body_AST;
}

void GDLParser::endcase_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode endcase_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case ENDCASE:
	{
		RefDNode tmp80_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp80_AST = astFactory->create(LT(1));
		}
		match(ENDCASE);
		break;
	}
	case END:
	{
		RefDNode tmp81_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp81_AST = astFactory->create(LT(1));
		}
		match(END);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = endcase_mark_AST;
}

void GDLParser::endcaseelse_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode endcaseelse_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case END:
	case ENDCASE:
	{
		endcase_mark();
		break;
	}
	case ENDELSE:
	{
		RefDNode tmp82_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp82_AST = astFactory->create(LT(1));
		}
		match(ENDELSE);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = endcaseelse_mark_AST;
}

void GDLParser::identifier_list() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode identifier_list_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp83_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp83_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp83_AST));
	}
	match(IDENTIFIER);
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			RefDNode tmp85_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp85_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, antlr::RefAST(tmp85_AST));
			}
			match(IDENTIFIER);
		}
		else {
			goto _loop59;
		}
		
	}
	_loop59:;
	} // ( ... )*
	identifier_list_AST = RefDNode(currentAST.root);
	returnAST = identifier_list_AST;
}

void GDLParser::keyword_declaration() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode keyword_declaration_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp86_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp86_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp86_AST));
	}
	match(IDENTIFIER);
	match(EQUAL);
	RefDNode tmp88_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp88_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp88_AST));
	}
	match(IDENTIFIER);
	if ( inputState->guessing==0 ) {
		keyword_declaration_AST = RefDNode(currentAST.root);
		keyword_declaration_AST =
					RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(KEYDECL,"keydecl")))->add(antlr::RefAST(keyword_declaration_AST))));
		currentAST.root = keyword_declaration_AST;
		if ( keyword_declaration_AST!=RefDNode(antlr::nullAST) &&
			keyword_declaration_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = keyword_declaration_AST->getFirstChild();
		else
			currentAST.child = keyword_declaration_AST;
		currentAST.advanceChildToEnd();
	}
	keyword_declaration_AST = RefDNode(currentAST.root);
	returnAST = keyword_declaration_AST;
}

std::string  GDLParser::object_name() {
	std::string name;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode object_name_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  i1 = antlr::nullToken;
	RefDNode i1_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  m = antlr::nullToken;
	RefDNode m_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  i2 = antlr::nullToken;
	RefDNode i2_AST = RefDNode(antlr::nullAST);
	
	i1 = LT(1);
	if ( inputState->guessing == 0 ) {
		i1_AST = astFactory->create(i1);
	}
	match(IDENTIFIER);
	m = LT(1);
	if ( inputState->guessing == 0 ) {
		m_AST = astFactory->create(m);
	}
	match(METHOD);
	i2 = LT(1);
	if ( inputState->guessing == 0 ) {
		i2_AST = astFactory->create(i2);
	}
	match(IDENTIFIER);
	if ( inputState->guessing==0 ) {
		object_name_AST = RefDNode(currentAST.root);
		
		// here we translate IDL_OBECT to GDL_OBJECT for source code compatibility
		{
		if( i1_AST->getText() == "IDL_OBJECT")
				i1_AST->setText(GDL_OBJECT_NAME);
		else if( i1_AST->getText() == "IDL_CONTAINER")
				i1_AST->setText(GDL_CONTAINER_NAME);
		}
		
		object_name_AST = RefDNode(astFactory->make((new antlr::ASTArray(4))->add(antlr::RefAST(NULL))->add(antlr::RefAST(i2_AST))->add(antlr::RefAST(m_AST))->add(antlr::RefAST(i1_AST)))); // NULL -> no root
		name= std::string( i1->getText()+"__"+i2->getText());
		
		currentAST.root = object_name_AST;
		if ( object_name_AST!=RefDNode(antlr::nullAST) &&
			object_name_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = object_name_AST->getFirstChild();
		else
			currentAST.child = object_name_AST;
		currentAST.advanceChildToEnd();
	}
	returnAST = object_name_AST;
	return name;
}

void GDLParser::compile_opt() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode compile_opt_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  i = antlr::nullToken;
	RefDNode i_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  ii = antlr::nullToken;
	RefDNode ii_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp89_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp89_AST = astFactory->create(LT(1));
	}
	match(COMPILE_OPT);
	i = LT(1);
	if ( inputState->guessing == 0 ) {
		i_AST = astFactory->create(i);
	}
	match(IDENTIFIER);
	if ( inputState->guessing==0 ) {
		
		AddCompileOpt( i->getText());
		
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			RefDNode tmp90_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp90_AST = astFactory->create(LT(1));
			}
			match(COMMA);
			ii = LT(1);
			if ( inputState->guessing == 0 ) {
				ii_AST = astFactory->create(ii);
			}
			match(IDENTIFIER);
			if ( inputState->guessing==0 ) {
				
				AddCompileOpt( ii->getText());
				
			}
		}
		else {
			goto _loop54;
		}
		
	}
	_loop54:;
	} // ( ... )*
	returnAST = compile_opt_AST;
}

void GDLParser::endforeach_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode endforeach_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case ENDFOREACH:
	{
		RefDNode tmp91_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp91_AST = astFactory->create(LT(1));
		}
		match(ENDFOREACH);
		break;
	}
	case END:
	{
		RefDNode tmp92_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp92_AST = astFactory->create(LT(1));
		}
		match(END);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = endforeach_mark_AST;
}

void GDLParser::endfor_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode endfor_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case ENDFOR:
	{
		RefDNode tmp93_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp93_AST = astFactory->create(LT(1));
		}
		match(ENDFOR);
		break;
	}
	case END:
	{
		RefDNode tmp94_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp94_AST = astFactory->create(LT(1));
		}
		match(END);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = endfor_mark_AST;
}

void GDLParser::endrep_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode endrep_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case ENDREP:
	{
		RefDNode tmp95_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp95_AST = astFactory->create(LT(1));
		}
		match(ENDREP);
		break;
	}
	case END:
	{
		RefDNode tmp96_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp96_AST = astFactory->create(LT(1));
		}
		match(END);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = endrep_mark_AST;
}

void GDLParser::endwhile_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode endwhile_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case ENDWHILE:
	{
		RefDNode tmp97_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp97_AST = astFactory->create(LT(1));
		}
		match(ENDWHILE);
		break;
	}
	case END:
	{
		RefDNode tmp98_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp98_AST = astFactory->create(LT(1));
		}
		match(END);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = endwhile_mark_AST;
}

void GDLParser::endif_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode endif_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case ENDIF:
	{
		RefDNode tmp99_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp99_AST = astFactory->create(LT(1));
		}
		match(ENDIF);
		break;
	}
	case END:
	{
		RefDNode tmp100_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp100_AST = astFactory->create(LT(1));
		}
		match(END);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = endif_mark_AST;
}

void GDLParser::endelse_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode endelse_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case ENDELSE:
	{
		RefDNode tmp101_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp101_AST = astFactory->create(LT(1));
		}
		match(ENDELSE);
		break;
	}
	case END:
	{
		RefDNode tmp102_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp102_AST = astFactory->create(LT(1));
		}
		match(END);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = endelse_mark_AST;
}

void GDLParser::compound_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode compound_statement_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case FORWARD:
	case GOTO:
	case IF:
	case INHERITS:
	case ON_IOERROR:
	case SWITCH:
	case DEC:
	case INC:
	case LBRACE:
	case SYSVARNAME:
	case ASTERIX:
	{
		statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		compound_statement_AST = RefDNode(currentAST.root);
		break;
	}
	case BEGIN:
	{
		match(BEGIN);
		statement_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		end_mark();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			compound_statement_AST = RefDNode(currentAST.root);
			compound_statement_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(compound_statement_AST))));
			currentAST.root = compound_statement_AST;
			if ( compound_statement_AST!=RefDNode(antlr::nullAST) &&
				compound_statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = compound_statement_AST->getFirstChild();
			else
				currentAST.child = compound_statement_AST;
			currentAST.advanceChildToEnd();
		}
		compound_statement_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = compound_statement_AST;
}

void GDLParser::label_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode label_statement_AST = RefDNode(antlr::nullAST);
	
	{ // ( ... )+
	int _cnt77=0;
	for (;;) {
		if ((LA(1) == IDENTIFIER) && (LA(2) == COLON)) {
			label();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			if ( _cnt77>=1 ) { goto _loop77; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt77++;
	}
	_loop77:;
	}  // ( ... )+
	{
	switch ( LA(1)) {
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case BEGIN:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case FORWARD:
	case GOTO:
	case IF:
	case INHERITS:
	case ON_IOERROR:
	case SWITCH:
	case DEC:
	case INC:
	case LBRACE:
	case SYSVARNAME:
	case ASTERIX:
	{
		compound_statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case END_U:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	label_statement_AST = RefDNode(currentAST.root);
	returnAST = label_statement_AST;
}

void GDLParser::label() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode label_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp104_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp104_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp104_AST));
	}
	match(IDENTIFIER);
	RefDNode tmp105_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp105_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp105_AST));
	}
	match(COLON);
	label_AST = RefDNode(currentAST.root);
	returnAST = label_AST;
}

void GDLParser::baseclass_method() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode baseclass_method_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  s = antlr::nullToken;
	RefDNode s_AST = RefDNode(antlr::nullAST);
	
	s = LT(1);
	if ( inputState->guessing == 0 ) {
		s_AST = astFactory->create(s);
		astFactory->addASTChild(currentAST, antlr::RefAST(s_AST));
	}
	match(IDENTIFIER);
	match(METHOD);
	if ( inputState->guessing==0 ) {
		
		if( s_AST->getText() == "IDL_OBJECT")
		s_AST->setText(GDL_OBJECT_NAME);
		else if( s_AST->getText() == "IDL_CONTAINER")
		s_AST->setText(GDL_CONTAINER_NAME);
		
	}
	baseclass_method_AST = RefDNode(currentAST.root);
	returnAST = baseclass_method_AST;
}

void GDLParser::assign_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode assign_expr_AST = RefDNode(antlr::nullAST);
	
	match(LBRACE);
	deref_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	match(EQUAL);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	match(RBRACE);
	if ( inputState->guessing==0 ) {
		assign_expr_AST = RefDNode(currentAST.root);
		assign_expr_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(assign_expr_AST))));
		currentAST.root = assign_expr_AST;
		if ( assign_expr_AST!=RefDNode(antlr::nullAST) &&
			assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = assign_expr_AST->getFirstChild();
		else
			currentAST.child = assign_expr_AST;
		currentAST.advanceChildToEnd();
	}
	assign_expr_AST = RefDNode(currentAST.root);
	returnAST = assign_expr_AST;
}

void GDLParser::deref_dot_expr_keeplast() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode deref_dot_expr_keeplast_AST = RefDNode(antlr::nullAST);
	RefDNode a1_AST = RefDNode(antlr::nullAST);
	
	RefDNode dot;
	int nDot;
	
	
	switch ( LA(1)) {
	case IDENTIFIER:
	case INHERITS:
	case LBRACE:
	case SYSVARNAME:
	{
		array_expr_1st();
		if (inputState->guessing==0) {
			a1_AST = returnAST;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{
		nDot=tag_access_keeplast();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			deref_dot_expr_keeplast_AST = RefDNode(currentAST.root);
			
			if( --nDot > 0)
			{
			dot=astFactory->create(DOT,".");
			dot->SetNDot( nDot);    
			dot->SetLine( a1_AST->getLine());
			deref_dot_expr_keeplast_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(dot))->add(antlr::RefAST(deref_dot_expr_keeplast_AST))));
			}
			
			currentAST.root = deref_dot_expr_keeplast_AST;
			if ( deref_dot_expr_keeplast_AST!=RefDNode(antlr::nullAST) &&
				deref_dot_expr_keeplast_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = deref_dot_expr_keeplast_AST->getFirstChild();
			else
				currentAST.child = deref_dot_expr_keeplast_AST;
			currentAST.advanceChildToEnd();
		}
		}
		deref_dot_expr_keeplast_AST = RefDNode(currentAST.root);
		break;
	}
	case ASTERIX:
	{
		match(ASTERIX);
		deref_dot_expr_keeplast();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			deref_dot_expr_keeplast_AST = RefDNode(currentAST.root);
			deref_dot_expr_keeplast_AST = 
						RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(DEREF,"deref")))->add(antlr::RefAST(deref_dot_expr_keeplast_AST))));
			currentAST.root = deref_dot_expr_keeplast_AST;
			if ( deref_dot_expr_keeplast_AST!=RefDNode(antlr::nullAST) &&
				deref_dot_expr_keeplast_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = deref_dot_expr_keeplast_AST->getFirstChild();
			else
				currentAST.child = deref_dot_expr_keeplast_AST;
			currentAST.advanceChildToEnd();
		}
		deref_dot_expr_keeplast_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = deref_dot_expr_keeplast_AST;
}

void GDLParser::formal_procedure_call() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode formal_procedure_call_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp111_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp111_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp111_AST));
	}
	match(IDENTIFIER);
	{
	switch ( LA(1)) {
	case COMMA:
	{
		match(COMMA);
		parameter_def_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case ELSE:
	case UNTIL:
	case END_U:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	formal_procedure_call_AST = RefDNode(currentAST.root);
	returnAST = formal_procedure_call_AST;
}

void GDLParser::deref_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode deref_expr_AST = RefDNode(antlr::nullAST);
	RefDNode a1_AST = RefDNode(antlr::nullAST);
	
	RefDNode dot;
	SizeT nDot;
	
	
	switch ( LA(1)) {
	case IDENTIFIER:
	case INHERITS:
	case LBRACE:
	case SYSVARNAME:
	{
		array_expr_1st();
		if (inputState->guessing==0) {
			a1_AST = returnAST;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{
		switch ( LA(1)) {
		case DOT:
		{
			nDot=tag_access();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			if ( inputState->guessing==0 ) {
				deref_expr_AST = RefDNode(currentAST.root);
				
				
				dot=astFactory->create(DOT,".");
				dot->SetNDot( nDot);    
				dot->SetLine( a1_AST->getLine());
				
				deref_expr_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(dot))->add(antlr::RefAST(deref_expr_AST))));
				
				currentAST.root = deref_expr_AST;
				if ( deref_expr_AST!=RefDNode(antlr::nullAST) &&
					deref_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
					  currentAST.child = deref_expr_AST->getFirstChild();
				else
					currentAST.child = deref_expr_AST;
				currentAST.advanceChildToEnd();
			}
			break;
		}
		case antlr::Token::EOF_TYPE:
		case AND_OP:
		case DO:
		case ELSE:
		case EQ_OP:
		case GE_OP:
		case GT_OP:
		case LE_OP:
		case LT_OP:
		case MOD_OP:
		case NE_OP:
		case OF:
		case OR_OP:
		case THEN:
		case UNTIL:
		case XOR_OP:
		case COMMA:
		case COLON:
		case END_U:
		case EQUAL:
		case DEC:
		case INC:
		case AND_OP_EQ:
		case ASTERIX_EQ:
		case EQ_OP_EQ:
		case GE_OP_EQ:
		case GTMARK_EQ:
		case GT_OP_EQ:
		case LE_OP_EQ:
		case LTMARK_EQ:
		case LT_OP_EQ:
		case MATRIX_OP1_EQ:
		case MATRIX_OP2_EQ:
		case MINUS_EQ:
		case MOD_OP_EQ:
		case NE_OP_EQ:
		case OR_OP_EQ:
		case PLUS_EQ:
		case POW_EQ:
		case SLASH_EQ:
		case XOR_OP_EQ:
		case MEMBER:
		case RBRACE:
		case SLASH:
		case RSQUARE:
		case RCURLY:
		case ASTERIX:
		case POW:
		case MATRIX_OP1:
		case MATRIX_OP2:
		case PLUS:
		case MINUS:
		case LTMARK:
		case GTMARK:
		case LOG_AND:
		case LOG_OR:
		case QUESTION:
		{
			if ( inputState->guessing==0 ) {
				deref_expr_AST = RefDNode(currentAST.root);
				deref_expr_AST = a1_AST;
				currentAST.root = deref_expr_AST;
				if ( deref_expr_AST!=RefDNode(antlr::nullAST) &&
					deref_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
					  currentAST.child = deref_expr_AST->getFirstChild();
				else
					currentAST.child = deref_expr_AST;
				currentAST.advanceChildToEnd();
			}
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		deref_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case ASTERIX:
	{
		match(ASTERIX);
		deref_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			deref_expr_AST = RefDNode(currentAST.root);
			deref_expr_AST = 
						RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(DEREF,"deref")))->add(antlr::RefAST(deref_expr_AST))));
			currentAST.root = deref_expr_AST;
			if ( deref_expr_AST!=RefDNode(antlr::nullAST) &&
				deref_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = deref_expr_AST->getFirstChild();
			else
				currentAST.child = deref_expr_AST;
			currentAST.advanceChildToEnd();
		}
		deref_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = deref_expr_AST;
}

void GDLParser::procedure_call() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode procedure_call_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  id = antlr::nullToken;
	RefDNode id_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode pa_AST = RefDNode(antlr::nullAST);
	
	id = LT(1);
	if ( inputState->guessing == 0 ) {
		id_AST = astFactory->create(id);
	}
	match(IDENTIFIER);
	{
	if (((_tokenSet_7.member(LA(1))) && (_tokenSet_1.member(LA(2))))&&(id->getText() == "RETURN")) {
		{
		switch ( LA(1)) {
		case COMMA:
		{
			match(COMMA);
			expr();
			if (inputState->guessing==0) {
				e_AST = returnAST;
			}
			break;
		}
		case ELSE:
		case UNTIL:
		case END_U:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
			procedure_call_AST = RefDNode(currentAST.root);
			
			id_AST->setType(RETURN); // text is already "return"
			procedure_call_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(id_AST))->add(antlr::RefAST(e_AST)))); // make root
			
			currentAST.root = procedure_call_AST;
			if ( procedure_call_AST!=RefDNode(antlr::nullAST) &&
				procedure_call_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = procedure_call_AST->getFirstChild();
			else
				currentAST.child = procedure_call_AST;
			currentAST.advanceChildToEnd();
		}
	}
	else if (((LA(1) == ELSE || LA(1) == UNTIL || LA(1) == END_U) && (_tokenSet_1.member(LA(2))))&&(id->getText() == "BREAK")) {
		if ( inputState->guessing==0 ) {
			procedure_call_AST = RefDNode(currentAST.root);
			
			id_AST->setType(BREAK); // text is already "break"
			procedure_call_AST = id_AST;
			
			currentAST.root = procedure_call_AST;
			if ( procedure_call_AST!=RefDNode(antlr::nullAST) &&
				procedure_call_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = procedure_call_AST->getFirstChild();
			else
				currentAST.child = procedure_call_AST;
			currentAST.advanceChildToEnd();
		}
	}
	else if (((LA(1) == ELSE || LA(1) == UNTIL || LA(1) == END_U) && (_tokenSet_1.member(LA(2))))&&(id->getText() == "CONTINUE")) {
		if ( inputState->guessing==0 ) {
			procedure_call_AST = RefDNode(currentAST.root);
			
			id_AST->setType(CONTINUE); // text is already "continue"
			procedure_call_AST = id_AST;
			
			currentAST.root = procedure_call_AST;
			if ( procedure_call_AST!=RefDNode(antlr::nullAST) &&
				procedure_call_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = procedure_call_AST->getFirstChild();
			else
				currentAST.child = procedure_call_AST;
			currentAST.advanceChildToEnd();
		}
	}
	else if ((_tokenSet_7.member(LA(1))) && (_tokenSet_9.member(LA(2)))) {
		{
		switch ( LA(1)) {
		case COMMA:
		{
			match(COMMA);
			parameter_def_list();
			if (inputState->guessing==0) {
				pa_AST = returnAST;
			}
			break;
		}
		case ELSE:
		case UNTIL:
		case END_U:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		if ( inputState->guessing==0 ) {
			procedure_call_AST = RefDNode(currentAST.root);
			
			procedure_call_AST = RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(PCALL,"pcall")))->add(antlr::RefAST(id_AST))->add(antlr::RefAST(pa_AST))));
			procedure_call_AST->SetLine(id->getLine());
			
			currentAST.root = procedure_call_AST;
			if ( procedure_call_AST!=RefDNode(antlr::nullAST) &&
				procedure_call_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = procedure_call_AST->getFirstChild();
			else
				currentAST.child = procedure_call_AST;
			currentAST.advanceChildToEnd();
		}
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	}
	returnAST = procedure_call_AST;
}

void GDLParser::for_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode for_statement_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp116_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp116_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp116_AST));
	}
	match(FOR);
	RefDNode tmp117_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp117_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp117_AST));
	}
	match(IDENTIFIER);
	match(EQUAL);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	match(COMMA);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{
	switch ( LA(1)) {
	case COMMA:
	{
		match(COMMA);
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case DO:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(DO);
	for_block();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	for_statement_AST = RefDNode(currentAST.root);
	returnAST = for_statement_AST;
}

void GDLParser::foreach_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode foreach_statement_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp122_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp122_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp122_AST));
	}
	match(FOREACH);
	RefDNode tmp123_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp123_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp123_AST));
	}
	match(IDENTIFIER);
	match(COMMA);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{
	switch ( LA(1)) {
	case COMMA:
	{
		match(COMMA);
		RefDNode tmp126_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp126_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp126_AST));
		}
		match(IDENTIFIER);
		break;
	}
	case DO:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(DO);
	foreach_block();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	foreach_statement_AST = RefDNode(currentAST.root);
	returnAST = foreach_statement_AST;
}

void GDLParser::repeat_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode repeat_statement_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp128_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp128_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp128_AST));
	}
	match(REPEAT);
	repeat_block();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	match(UNTIL);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	repeat_statement_AST = RefDNode(currentAST.root);
	returnAST = repeat_statement_AST;
}

void GDLParser::while_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode while_statement_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp130_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp130_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp130_AST));
	}
	match(WHILE);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	match(DO);
	while_block();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	while_statement_AST = RefDNode(currentAST.root);
	returnAST = while_statement_AST;
}

void GDLParser::jump_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode jump_statement_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case GOTO:
	{
		RefDNode tmp132_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp132_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp132_AST));
		}
		match(GOTO);
		match(COMMA);
		RefDNode tmp134_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp134_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp134_AST));
		}
		match(IDENTIFIER);
		jump_statement_AST = RefDNode(currentAST.root);
		break;
	}
	case ON_IOERROR:
	{
		RefDNode tmp135_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp135_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp135_AST));
		}
		match(ON_IOERROR);
		match(COMMA);
		RefDNode tmp137_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp137_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp137_AST));
		}
		match(IDENTIFIER);
		jump_statement_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = jump_statement_AST;
}

void GDLParser::if_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode if_statement_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp138_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp138_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp138_AST));
	}
	match(IF);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	match(THEN);
	if_block();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{
	if ((LA(1) == ELSE) && (_tokenSet_2.member(LA(2)))) {
		match(ELSE);
		else_block();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
	}
	else if ((LA(1) == ELSE || LA(1) == UNTIL || LA(1) == END_U) && (_tokenSet_1.member(LA(2)))) {
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	}
	if_statement_AST = RefDNode(currentAST.root);
	returnAST = if_statement_AST;
}

void GDLParser::repeat_block() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode repeat_block_AST = RefDNode(antlr::nullAST);
	RefDNode st_AST = RefDNode(antlr::nullAST);
	RefDNode stl_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case FORWARD:
	case GOTO:
	case IF:
	case INHERITS:
	case ON_IOERROR:
	case SWITCH:
	case DEC:
	case INC:
	case LBRACE:
	case SYSVARNAME:
	case ASTERIX:
	{
		statement();
		if (inputState->guessing==0) {
			st_AST = returnAST;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			repeat_block_AST = RefDNode(currentAST.root);
			repeat_block_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(st_AST))));
			currentAST.root = repeat_block_AST;
			if ( repeat_block_AST!=RefDNode(antlr::nullAST) &&
				repeat_block_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = repeat_block_AST->getFirstChild();
			else
				currentAST.child = repeat_block_AST;
			currentAST.advanceChildToEnd();
		}
		repeat_block_AST = RefDNode(currentAST.root);
		break;
	}
	case BEGIN:
	{
		match(BEGIN);
		statement_list();
		if (inputState->guessing==0) {
			stl_AST = returnAST;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		endrep_mark();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			repeat_block_AST = RefDNode(currentAST.root);
			repeat_block_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(stl_AST))));
			currentAST.root = repeat_block_AST;
			if ( repeat_block_AST!=RefDNode(antlr::nullAST) &&
				repeat_block_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = repeat_block_AST->getFirstChild();
			else
				currentAST.child = repeat_block_AST;
			currentAST.advanceChildToEnd();
		}
		repeat_block_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = repeat_block_AST;
}

void GDLParser::while_block() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode while_block_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case FORWARD:
	case GOTO:
	case IF:
	case INHERITS:
	case ON_IOERROR:
	case SWITCH:
	case DEC:
	case INC:
	case LBRACE:
	case SYSVARNAME:
	case ASTERIX:
	{
		statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		while_block_AST = RefDNode(currentAST.root);
		break;
	}
	case BEGIN:
	{
		match(BEGIN);
		statement_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		endwhile_mark();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			while_block_AST = RefDNode(currentAST.root);
			while_block_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(while_block_AST))));
			currentAST.root = while_block_AST;
			if ( while_block_AST!=RefDNode(antlr::nullAST) &&
				while_block_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = while_block_AST->getFirstChild();
			else
				currentAST.child = while_block_AST;
			currentAST.advanceChildToEnd();
		}
		while_block_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = while_block_AST;
}

void GDLParser::for_block() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode for_block_AST = RefDNode(antlr::nullAST);
	RefDNode st_AST = RefDNode(antlr::nullAST);
	RefDNode stl_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case FORWARD:
	case GOTO:
	case IF:
	case INHERITS:
	case ON_IOERROR:
	case SWITCH:
	case DEC:
	case INC:
	case LBRACE:
	case SYSVARNAME:
	case ASTERIX:
	{
		statement();
		if (inputState->guessing==0) {
			st_AST = returnAST;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			for_block_AST = RefDNode(currentAST.root);
			for_block_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(st_AST))));
			currentAST.root = for_block_AST;
			if ( for_block_AST!=RefDNode(antlr::nullAST) &&
				for_block_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = for_block_AST->getFirstChild();
			else
				currentAST.child = for_block_AST;
			currentAST.advanceChildToEnd();
		}
		for_block_AST = RefDNode(currentAST.root);
		break;
	}
	case BEGIN:
	{
		match(BEGIN);
		statement_list();
		if (inputState->guessing==0) {
			stl_AST = returnAST;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		endfor_mark();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			for_block_AST = RefDNode(currentAST.root);
			for_block_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(stl_AST))));
			currentAST.root = for_block_AST;
			if ( for_block_AST!=RefDNode(antlr::nullAST) &&
				for_block_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = for_block_AST->getFirstChild();
			else
				currentAST.child = for_block_AST;
			currentAST.advanceChildToEnd();
		}
		for_block_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = for_block_AST;
}

void GDLParser::foreach_block() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode foreach_block_AST = RefDNode(antlr::nullAST);
	RefDNode st_AST = RefDNode(antlr::nullAST);
	RefDNode stl_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case FORWARD:
	case GOTO:
	case IF:
	case INHERITS:
	case ON_IOERROR:
	case SWITCH:
	case DEC:
	case INC:
	case LBRACE:
	case SYSVARNAME:
	case ASTERIX:
	{
		statement();
		if (inputState->guessing==0) {
			st_AST = returnAST;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			foreach_block_AST = RefDNode(currentAST.root);
			foreach_block_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(st_AST))));
			currentAST.root = foreach_block_AST;
			if ( foreach_block_AST!=RefDNode(antlr::nullAST) &&
				foreach_block_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = foreach_block_AST->getFirstChild();
			else
				currentAST.child = foreach_block_AST;
			currentAST.advanceChildToEnd();
		}
		foreach_block_AST = RefDNode(currentAST.root);
		break;
	}
	case BEGIN:
	{
		match(BEGIN);
		statement_list();
		if (inputState->guessing==0) {
			stl_AST = returnAST;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		endforeach_mark();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			foreach_block_AST = RefDNode(currentAST.root);
			foreach_block_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(stl_AST))));
			currentAST.root = foreach_block_AST;
			if ( foreach_block_AST!=RefDNode(antlr::nullAST) &&
				foreach_block_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = foreach_block_AST->getFirstChild();
			else
				currentAST.child = foreach_block_AST;
			currentAST.advanceChildToEnd();
		}
		foreach_block_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = foreach_block_AST;
}

void GDLParser::if_block() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode if_block_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case FORWARD:
	case GOTO:
	case IF:
	case INHERITS:
	case ON_IOERROR:
	case SWITCH:
	case DEC:
	case INC:
	case LBRACE:
	case SYSVARNAME:
	case ASTERIX:
	{
		statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if_block_AST = RefDNode(currentAST.root);
		break;
	}
	case BEGIN:
	{
		match(BEGIN);
		statement_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		endif_mark();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			if_block_AST = RefDNode(currentAST.root);
			if_block_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(if_block_AST))));
			currentAST.root = if_block_AST;
			if ( if_block_AST!=RefDNode(antlr::nullAST) &&
				if_block_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = if_block_AST->getFirstChild();
			else
				currentAST.child = if_block_AST;
			currentAST.advanceChildToEnd();
		}
		if_block_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = if_block_AST;
}

void GDLParser::else_block() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode else_block_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case FORWARD:
	case GOTO:
	case IF:
	case INHERITS:
	case ON_IOERROR:
	case SWITCH:
	case DEC:
	case INC:
	case LBRACE:
	case SYSVARNAME:
	case ASTERIX:
	{
		statement();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		else_block_AST = RefDNode(currentAST.root);
		break;
	}
	case BEGIN:
	{
		match(BEGIN);
		statement_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		endelse_mark();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			else_block_AST = RefDNode(currentAST.root);
			else_block_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))->add(antlr::RefAST(else_block_AST))));
			currentAST.root = else_block_AST;
			if ( else_block_AST!=RefDNode(antlr::nullAST) &&
				else_block_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = else_block_AST->getFirstChild();
			else
				currentAST.child = else_block_AST;
			currentAST.advanceChildToEnd();
		}
		else_block_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = else_block_AST;
}

void GDLParser::parameter_def_list() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode parameter_def_list_AST = RefDNode(antlr::nullAST);
	
	parameter_def();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			parameter_def();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop123;
		}
		
	}
	_loop123:;
	} // ( ... )*
	parameter_def_list_AST = RefDNode(currentAST.root);
	returnAST = parameter_def_list_AST;
}

void GDLParser::formal_function_call() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode formal_function_call_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp148_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp148_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp148_AST));
	}
	match(IDENTIFIER);
	match(LBRACE);
	{
	switch ( LA(1)) {
	case FOR:
	case FOREACH:
	case REPEAT:
	case WHILE:
	case IDENTIFIER:
	case AND_OP:
	case BEGIN:
	case CASE:
	case COMMON:
	case COMPILE_OPT:
	case DO:
	case ELSE:
	case END:
	case ENDCASE:
	case ENDELSE:
	case ENDFOR:
	case ENDFOREACH:
	case ENDIF:
	case ENDREP:
	case ENDSWITCH:
	case ENDWHILE:
	case EQ_OP:
	case FORWARD:
	case FUNCTION:
	case GE_OP:
	case GOTO:
	case GT_OP:
	case IF:
	case INHERITS:
	case LE_OP:
	case LT_OP:
	case MOD_OP:
	case NE_OP:
	case NOT_OP:
	case OF:
	case ON_IOERROR:
	case OR_OP:
	case PRO:
	case SWITCH:
	case THEN:
	case UNTIL:
	case XOR_OP:
	case DEC:
	case INC:
	case LBRACE:
	case SLASH:
	case LSQUARE:
	case SYSVARNAME:
	case LCURLY:
	case CONSTANT_HEX_BYTE:
	case CONSTANT_HEX_LONG:
	case CONSTANT_HEX_LONG64:
	case CONSTANT_HEX_INT:
	case CONSTANT_HEX_I:
	case CONSTANT_HEX_ULONG:
	case CONSTANT_HEX_ULONG64:
	case CONSTANT_HEX_UI:
	case CONSTANT_HEX_UINT:
	case CONSTANT_BYTE:
	case CONSTANT_LONG:
	case CONSTANT_LONG64:
	case CONSTANT_INT:
	case CONSTANT_I:
	case CONSTANT_ULONG:
	case CONSTANT_ULONG64:
	case CONSTANT_UI:
	case CONSTANT_UINT:
	case CONSTANT_OCT_BYTE:
	case CONSTANT_OCT_LONG:
	case CONSTANT_OCT_LONG64:
	case CONSTANT_OCT_INT:
	case CONSTANT_OCT_I:
	case CONSTANT_OCT_ULONG:
	case CONSTANT_OCT_ULONG64:
	case CONSTANT_OCT_UI:
	case CONSTANT_OCT_UINT:
	case CONSTANT_FLOAT:
	case CONSTANT_DOUBLE:
	case CONSTANT_BIN_BYTE:
	case CONSTANT_BIN_LONG:
	case CONSTANT_BIN_LONG64:
	case CONSTANT_BIN_INT:
	case CONSTANT_BIN_I:
	case CONSTANT_BIN_ULONG:
	case CONSTANT_BIN_ULONG64:
	case CONSTANT_BIN_UI:
	case CONSTANT_BIN_UINT:
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case LOG_NEG:
	{
		parameter_def_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
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
	match(RBRACE);
	formal_function_call_AST = RefDNode(currentAST.root);
	returnAST = formal_function_call_AST;
}

void GDLParser::parameter_def() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode parameter_def_AST = RefDNode(antlr::nullAST);
	RefDNode id_AST = RefDNode(antlr::nullAST);
	
	if ((_tokenSet_10.member(LA(1))) && (LA(2) == EQUAL)) {
		identifier();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		match(EQUAL);
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			parameter_def_AST = RefDNode(currentAST.root);
			parameter_def_AST =
						RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(KEYDEF,"!=!")))->add(antlr::RefAST(parameter_def_AST))));
			currentAST.root = parameter_def_AST;
			if ( parameter_def_AST!=RefDNode(antlr::nullAST) &&
				parameter_def_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = parameter_def_AST->getFirstChild();
			else
				currentAST.child = parameter_def_AST;
			currentAST.advanceChildToEnd();
		}
		parameter_def_AST = RefDNode(currentAST.root);
	}
	else if ((_tokenSet_11.member(LA(1))) && (_tokenSet_12.member(LA(2)))) {
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		parameter_def_AST = RefDNode(currentAST.root);
	}
	else if ((LA(1) == SLASH)) {
		match(SLASH);
		identifier();
		if (inputState->guessing==0) {
			id_AST = returnAST;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			parameter_def_AST = RefDNode(currentAST.root);
			
			RefDNode c=static_cast<RefDNode>( astFactory->create(CONSTANT,"1"));
			c->Text2Int(10);
			c->SetLine( id_AST->getLine());
			parameter_def_AST = RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(KEYDEF,"!=!")))->add(antlr::RefAST(id_AST))->add(antlr::RefAST(c))));
			
			currentAST.root = parameter_def_AST;
			if ( parameter_def_AST!=RefDNode(antlr::nullAST) &&
				parameter_def_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = parameter_def_AST->getFirstChild();
			else
				currentAST.child = parameter_def_AST;
			currentAST.advanceChildToEnd();
		}
		parameter_def_AST = RefDNode(currentAST.root);
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	returnAST = parameter_def_AST;
}

void GDLParser::array_def() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode array_def_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode ee_AST = RefDNode(antlr::nullAST);
	RefDNode eee_AST = RefDNode(antlr::nullAST);
	
	bool constant = true;
	int flexible_array_def_count=1;
	
	
	match(LSQUARE);
	expr();
	if (inputState->guessing==0) {
		e_AST = returnAST;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	if ( inputState->guessing==0 ) {
		if( !ConstantExprNode( e_AST->getType())) constant = false;
	}
	{
	switch ( LA(1)) {
	case COMMA:
	case RSQUARE:
	{
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				expr();
				if (inputState->guessing==0) {
					ee_AST = returnAST;
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				if ( inputState->guessing==0 ) {
					if( !ConstantExprNode( ee_AST->getType())) constant = false;
				}
			}
			else {
				goto _loop127;
			}
			
		}
		_loop127:;
		} // ( ... )*
		match(RSQUARE);
		if ( inputState->guessing==0 ) {
			array_def_AST = RefDNode(currentAST.root);
			
			if( constant)
			array_def_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(ARRAYDEF_CONST,"array_def_const")))->add(antlr::RefAST(array_def_AST))));
			else
			array_def_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(ARRAYDEF,"array_def")))->add(antlr::RefAST(array_def_AST))));
			
			currentAST.root = array_def_AST;
			if ( array_def_AST!=RefDNode(antlr::nullAST) &&
				array_def_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = array_def_AST->getFirstChild();
			else
				currentAST.child = array_def_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case COLON:
	{
		{ // ( ... )+
		int _cnt129=0;
		for (;;) {
			if ((LA(1) == COLON)) {
				match(COLON);
				expr();
				if (inputState->guessing==0) {
					eee_AST = returnAST;
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				if ( inputState->guessing==0 ) {
					flexible_array_def_count++;
				}
			}
			else {
				if ( _cnt129>=1 ) { goto _loop129; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
			}
			
			_cnt129++;
		}
		_loop129:;
		}  // ( ... )+
		match(RSQUARE);
		if ( inputState->guessing==0 ) {
			array_def_AST = RefDNode(currentAST.root);
			
			if (flexible_array_def_count!=3) throw GDLException( "Illegal array creation syntax.");
			array_def_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(ARRAYDEF_GENERALIZED_INDGEN,"array_def_generalized_indgen")))->add(antlr::RefAST(array_def_AST))));
			
			currentAST.root = array_def_AST;
			if ( array_def_AST!=RefDNode(antlr::nullAST) &&
				array_def_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = array_def_AST->getFirstChild();
			else
				currentAST.child = array_def_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	array_def_AST = RefDNode(currentAST.root);
	returnAST = array_def_AST;
}

void GDLParser::struct_identifier() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode struct_identifier_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  s = antlr::nullToken;
	RefDNode s_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  e = antlr::nullToken;
	RefDNode e_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  i = antlr::nullToken;
	RefDNode i_AST = RefDNode(antlr::nullAST);
	
	{
	switch ( LA(1)) {
	case IDENTIFIER:
	{
		RefDNode tmp158_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp158_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp158_AST));
		}
		match(IDENTIFIER);
		break;
	}
	case SYSVARNAME:
	{
		s = LT(1);
		if ( inputState->guessing == 0 ) {
			s_AST = astFactory->create(s);
			astFactory->addASTChild(currentAST, antlr::RefAST(s_AST));
		}
		match(SYSVARNAME);
		if ( inputState->guessing==0 ) {
			s_AST->setType( IDENTIFIER);
		}
		break;
	}
	case EXCLAMATION:
	{
		e = LT(1);
		if ( inputState->guessing == 0 ) {
			e_AST = astFactory->create(e);
			astFactory->addASTChild(currentAST, antlr::RefAST(e_AST));
		}
		match(EXCLAMATION);
		if ( inputState->guessing==0 ) {
			e_AST->setType( IDENTIFIER);
		}
		break;
	}
	case INHERITS:
	{
		i = LT(1);
		if ( inputState->guessing == 0 ) {
			i_AST = astFactory->create(i);
			astFactory->addASTChild(currentAST, antlr::RefAST(i_AST));
		}
		match(INHERITS);
		if ( inputState->guessing==0 ) {
			i_AST->setType( IDENTIFIER);
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	struct_identifier_AST = RefDNode(currentAST.root);
	returnAST = struct_identifier_AST;
}

void GDLParser::struct_name() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode struct_name_AST = RefDNode(antlr::nullAST);
	RefDNode s_AST = RefDNode(antlr::nullAST);
	
	struct_identifier();
	if (inputState->guessing==0) {
		s_AST = returnAST;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	if ( inputState->guessing==0 ) {
		
		if( s_AST->getText() == "IDL_OBJECT")
		s_AST->setText(GDL_OBJECT_NAME);
		else if( s_AST->getText() == "IDL_CONTAINER")
		s_AST->setText(GDL_CONTAINER_NAME);
		
	}
	struct_name_AST = RefDNode(currentAST.root);
	returnAST = struct_name_AST;
}

void GDLParser::struct_def() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode struct_def_AST = RefDNode(antlr::nullAST);
	
	match(LCURLY);
	{
	if ((_tokenSet_13.member(LA(1))) && (LA(2) == COMMA || LA(2) == RCURLY)) {
		struct_name();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{
		switch ( LA(1)) {
		case COMMA:
		{
			match(COMMA);
			named_tag_def_list();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			break;
		}
		case RCURLY:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		match(RCURLY);
		if ( inputState->guessing==0 ) {
			struct_def_AST = RefDNode(currentAST.root);
			struct_def_AST = 
							RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(NSTRUC_REF,"nstruct_ref")))->add(antlr::RefAST(struct_def_AST))));
			currentAST.root = struct_def_AST;
			if ( struct_def_AST!=RefDNode(antlr::nullAST) &&
				struct_def_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = struct_def_AST->getFirstChild();
			else
				currentAST.child = struct_def_AST;
			currentAST.advanceChildToEnd();
		}
	}
	else if ((_tokenSet_13.member(LA(1))) && (LA(2) == COLON)) {
		tag_def_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		match(RCURLY);
		if ( inputState->guessing==0 ) {
			struct_def_AST = RefDNode(currentAST.root);
			struct_def_AST = 
							RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(STRUC,"struct")))->add(antlr::RefAST(struct_def_AST))));
			currentAST.root = struct_def_AST;
			if ( struct_def_AST!=RefDNode(antlr::nullAST) &&
				struct_def_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = struct_def_AST->getFirstChild();
			else
				currentAST.child = struct_def_AST;
			currentAST.advanceChildToEnd();
		}
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	}
	struct_def_AST = RefDNode(currentAST.root);
	returnAST = struct_def_AST;
}

void GDLParser::named_tag_def_list() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode named_tag_def_list_AST = RefDNode(antlr::nullAST);
	
	named_tag_def_entry();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			named_tag_def_entry();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop150;
		}
		
	}
	_loop150:;
	} // ( ... )*
	named_tag_def_list_AST = RefDNode(currentAST.root);
	returnAST = named_tag_def_list_AST;
}

void GDLParser::tag_def_list() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_def_list_AST = RefDNode(antlr::nullAST);
	
	tag_def();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			tag_def();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop139;
		}
		
	}
	_loop139:;
	} // ( ... )*
	tag_def_list_AST = RefDNode(currentAST.root);
	returnAST = tag_def_list_AST;
}

void GDLParser::tag_def() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_def_AST = RefDNode(antlr::nullAST);
	
	struct_identifier();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	match(COLON);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	tag_def_AST = RefDNode(currentAST.root);
	returnAST = tag_def_AST;
}

void GDLParser::ntag_def() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode ntag_def_AST = RefDNode(antlr::nullAST);
	
	if ((_tokenSet_13.member(LA(1))) && (LA(2) == COLON)) {
		tag_def();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		ntag_def_AST = RefDNode(currentAST.root);
	}
	else if ((_tokenSet_11.member(LA(1))) && (_tokenSet_14.member(LA(2)))) {
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		ntag_def_AST = RefDNode(currentAST.root);
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	returnAST = ntag_def_AST;
}

void GDLParser::ntag_defs() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode ntag_defs_AST = RefDNode(antlr::nullAST);
	
	ntag_def();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			ntag_def();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop143;
		}
		
	}
	_loop143:;
	} // ( ... )*
	ntag_defs_AST = RefDNode(currentAST.root);
	returnAST = ntag_defs_AST;
}

void GDLParser::named_tag_def_entry() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode named_tag_def_entry_AST = RefDNode(antlr::nullAST);
	
	{
	bool synPredMatched147 = false;
	if (((LA(1) == INHERITS) && (_tokenSet_13.member(LA(2))))) {
		int _m147 = mark();
		synPredMatched147 = true;
		inputState->guessing++;
		try {
			{
			match(INHERITS);
			}
		}
		catch (antlr::RecognitionException& pe) {
			synPredMatched147 = false;
		}
		rewind(_m147);
		inputState->guessing--;
	}
	if ( synPredMatched147 ) {
		RefDNode tmp167_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp167_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp167_AST));
		}
		match(INHERITS);
		struct_name();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
	}
	else if ((_tokenSet_15.member(LA(1))) && (_tokenSet_16.member(LA(2)))) {
		ntag_def();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	}
	named_tag_def_entry_AST = RefDNode(currentAST.root);
	returnAST = named_tag_def_entry_AST;
}

void GDLParser::numeric_constant() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode numeric_constant_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c1 = antlr::nullToken;
	RefDNode c1_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c2 = antlr::nullToken;
	RefDNode c2_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c3 = antlr::nullToken;
	RefDNode c3_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c4 = antlr::nullToken;
	RefDNode c4_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c44 = antlr::nullToken;
	RefDNode c44_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c5 = antlr::nullToken;
	RefDNode c5_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c6 = antlr::nullToken;
	RefDNode c6_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c77 = antlr::nullToken;
	RefDNode c77_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c7 = antlr::nullToken;
	RefDNode c7_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c8 = antlr::nullToken;
	RefDNode c8_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c9 = antlr::nullToken;
	RefDNode c9_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c10 = antlr::nullToken;
	RefDNode c10_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c11 = antlr::nullToken;
	RefDNode c11_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c111 = antlr::nullToken;
	RefDNode c111_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c12 = antlr::nullToken;
	RefDNode c12_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c13 = antlr::nullToken;
	RefDNode c13_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c144 = antlr::nullToken;
	RefDNode c144_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c14 = antlr::nullToken;
	RefDNode c14_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c15 = antlr::nullToken;
	RefDNode c15_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c16 = antlr::nullToken;
	RefDNode c16_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c17 = antlr::nullToken;
	RefDNode c17_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c18 = antlr::nullToken;
	RefDNode c18_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c188 = antlr::nullToken;
	RefDNode c188_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c19 = antlr::nullToken;
	RefDNode c19_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c20 = antlr::nullToken;
	RefDNode c20_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c211 = antlr::nullToken;
	RefDNode c211_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c21 = antlr::nullToken;
	RefDNode c21_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c22 = antlr::nullToken;
	RefDNode c22_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c23 = antlr::nullToken;
	RefDNode c23_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c24 = antlr::nullToken;
	RefDNode c24_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c25 = antlr::nullToken;
	RefDNode c25_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c26 = antlr::nullToken;
	RefDNode c26_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c27 = antlr::nullToken;
	RefDNode c27_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c277 = antlr::nullToken;
	RefDNode c277_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c28 = antlr::nullToken;
	RefDNode c28_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c29 = antlr::nullToken;
	RefDNode c29_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c300 = antlr::nullToken;
	RefDNode c300_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  c30 = antlr::nullToken;
	RefDNode c30_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case CONSTANT_HEX_BYTE:
	{
		c1 = LT(1);
		if ( inputState->guessing == 0 ) {
			c1_AST = astFactory->create(c1);
		}
		match(CONSTANT_HEX_BYTE);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c1->getText());
					  numeric_constant_AST->Text2Byte(16);	
					  numeric_constant_AST->SetLine( c1->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_HEX_LONG:
	{
		c2 = LT(1);
		if ( inputState->guessing == 0 ) {
			c2_AST = astFactory->create(c2);
		}
		match(CONSTANT_HEX_LONG);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c2->getText());
					  numeric_constant_AST->Text2Long(16);	
					  numeric_constant_AST->SetLine( c2->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_HEX_LONG64:
	{
		c3 = LT(1);
		if ( inputState->guessing == 0 ) {
			c3_AST = astFactory->create(c3);
		}
		match(CONSTANT_HEX_LONG64);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c3->getText());
					  numeric_constant_AST->Text2Long64(16);	
					  numeric_constant_AST->SetLine( c3->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_HEX_INT:
	{
		c4 = LT(1);
		if ( inputState->guessing == 0 ) {
			c4_AST = astFactory->create(c4);
		}
		match(CONSTANT_HEX_INT);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c4->getText());
					  numeric_constant_AST->Text2Int(16);	
					  numeric_constant_AST->SetLine( c4->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_HEX_I:
	{
		c44 = LT(1);
		if ( inputState->guessing == 0 ) {
			c44_AST = astFactory->create(c44);
		}
		match(CONSTANT_HEX_I);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c44->getText());
			if( compileOpt & DEFINT32)
			numeric_constant_AST->Text2Long(16,true);	
			else
			numeric_constant_AST->Text2Int(16,true);	
					  numeric_constant_AST->SetLine( c44->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_HEX_ULONG:
	{
		c5 = LT(1);
		if ( inputState->guessing == 0 ) {
			c5_AST = astFactory->create(c5);
		}
		match(CONSTANT_HEX_ULONG);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c5->getText());
					  numeric_constant_AST->Text2ULong(16);	
					  numeric_constant_AST->SetLine( c5->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_HEX_ULONG64:
	{
		c6 = LT(1);
		if ( inputState->guessing == 0 ) {
			c6_AST = astFactory->create(c6);
		}
		match(CONSTANT_HEX_ULONG64);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c6->getText());
					  numeric_constant_AST->Text2ULong64(16);	
					  numeric_constant_AST->SetLine( c6->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_HEX_UI:
	{
		c77 = LT(1);
		if ( inputState->guessing == 0 ) {
			c77_AST = astFactory->create(c77);
		}
		match(CONSTANT_HEX_UI);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c77->getText());
			if( compileOpt & DEFINT32)
			numeric_constant_AST->Text2ULong(16,true);	
			else
			numeric_constant_AST->Text2UInt(16,true);	
					  numeric_constant_AST->SetLine( c77->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_HEX_UINT:
	{
		c7 = LT(1);
		if ( inputState->guessing == 0 ) {
			c7_AST = astFactory->create(c7);
		}
		match(CONSTANT_HEX_UINT);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c7->getText());
					  numeric_constant_AST->Text2UInt(16);	
					  numeric_constant_AST->SetLine( c7->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_BYTE:
	{
		c8 = LT(1);
		if ( inputState->guessing == 0 ) {
			c8_AST = astFactory->create(c8);
		}
		match(CONSTANT_BYTE);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c8->getText());
					  numeric_constant_AST->Text2Byte(10);	
					  numeric_constant_AST->SetLine( c8->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_LONG:
	{
		c9 = LT(1);
		if ( inputState->guessing == 0 ) {
			c9_AST = astFactory->create(c9);
		}
		match(CONSTANT_LONG);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c9->getText());
					  numeric_constant_AST->Text2Long(10);	
					  numeric_constant_AST->SetLine( c9->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_LONG64:
	{
		c10 = LT(1);
		if ( inputState->guessing == 0 ) {
			c10_AST = astFactory->create(c10);
		}
		match(CONSTANT_LONG64);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c10->getText());
					  numeric_constant_AST->Text2Long64(10);	
					  numeric_constant_AST->SetLine( c10->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_INT:
	{
		c11 = LT(1);
		if ( inputState->guessing == 0 ) {
			c11_AST = astFactory->create(c11);
		}
		match(CONSTANT_INT);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c11->getText());
					  numeric_constant_AST->Text2Int(10);	
					  numeric_constant_AST->SetLine( c11->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_I:
	{
		c111 = LT(1);
		if ( inputState->guessing == 0 ) {
			c111_AST = astFactory->create(c111);
		}
		match(CONSTANT_I);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c111->getText());
			if( compileOpt & DEFINT32)
			numeric_constant_AST->Text2Long(10,true);	
			else
			numeric_constant_AST->Text2Int(10,true);	
					  numeric_constant_AST->SetLine( c111->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_ULONG:
	{
		c12 = LT(1);
		if ( inputState->guessing == 0 ) {
			c12_AST = astFactory->create(c12);
		}
		match(CONSTANT_ULONG);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c12->getText());
					  numeric_constant_AST->Text2ULong(10);	
					  numeric_constant_AST->SetLine( c12->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_ULONG64:
	{
		c13 = LT(1);
		if ( inputState->guessing == 0 ) {
			c13_AST = astFactory->create(c13);
		}
		match(CONSTANT_ULONG64);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c13->getText());
					  numeric_constant_AST->Text2ULong64(10);	
					  numeric_constant_AST->SetLine( c13->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_UI:
	{
		c144 = LT(1);
		if ( inputState->guessing == 0 ) {
			c144_AST = astFactory->create(c144);
		}
		match(CONSTANT_UI);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c144->getText());
			if( compileOpt & DEFINT32)
			numeric_constant_AST->Text2ULong(10,true);	
			else
			numeric_constant_AST->Text2UInt(10,true);	
					  numeric_constant_AST->SetLine( c144->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_UINT:
	{
		c14 = LT(1);
		if ( inputState->guessing == 0 ) {
			c14_AST = astFactory->create(c14);
		}
		match(CONSTANT_UINT);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c14->getText());
					  numeric_constant_AST->Text2UInt(10);	
					  numeric_constant_AST->SetLine( c14->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_OCT_BYTE:
	{
		c15 = LT(1);
		if ( inputState->guessing == 0 ) {
			c15_AST = astFactory->create(c15);
		}
		match(CONSTANT_OCT_BYTE);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c15->getText());
					  numeric_constant_AST->Text2Byte(8);	
					  numeric_constant_AST->SetLine( c15->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_OCT_LONG:
	{
		c16 = LT(1);
		if ( inputState->guessing == 0 ) {
			c16_AST = astFactory->create(c16);
		}
		match(CONSTANT_OCT_LONG);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c16->getText());
					  numeric_constant_AST->Text2Long(8);	
					  numeric_constant_AST->SetLine( c16->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_OCT_LONG64:
	{
		c17 = LT(1);
		if ( inputState->guessing == 0 ) {
			c17_AST = astFactory->create(c17);
		}
		match(CONSTANT_OCT_LONG64);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c17->getText());
					  numeric_constant_AST->Text2Long64(8);	
					  numeric_constant_AST->SetLine( c17->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_OCT_INT:
	{
		c18 = LT(1);
		if ( inputState->guessing == 0 ) {
			c18_AST = astFactory->create(c18);
		}
		match(CONSTANT_OCT_INT);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c18->getText());
					  numeric_constant_AST->Text2Int(8);	
					  numeric_constant_AST->SetLine( c18->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_OCT_I:
	{
		c188 = LT(1);
		if ( inputState->guessing == 0 ) {
			c188_AST = astFactory->create(c188);
		}
		match(CONSTANT_OCT_I);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c188->getText());
			if( compileOpt & DEFINT32)
			numeric_constant_AST->Text2Long(8,true);	
			else
			numeric_constant_AST->Text2Int(8,true);	
					  numeric_constant_AST->SetLine( c188->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_OCT_ULONG:
	{
		c19 = LT(1);
		if ( inputState->guessing == 0 ) {
			c19_AST = astFactory->create(c19);
		}
		match(CONSTANT_OCT_ULONG);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c19->getText());
					  numeric_constant_AST->Text2ULong(8);	
					  numeric_constant_AST->SetLine( c19->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_OCT_ULONG64:
	{
		c20 = LT(1);
		if ( inputState->guessing == 0 ) {
			c20_AST = astFactory->create(c20);
		}
		match(CONSTANT_OCT_ULONG64);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c20->getText());
					  numeric_constant_AST->Text2ULong64(8);	
					  numeric_constant_AST->SetLine( c20->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_OCT_UI:
	{
		c211 = LT(1);
		if ( inputState->guessing == 0 ) {
			c211_AST = astFactory->create(c211);
		}
		match(CONSTANT_OCT_UI);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c211->getText());
			if( compileOpt & DEFINT32)
			numeric_constant_AST->Text2ULong(8,true);	
			else
			numeric_constant_AST->Text2UInt(8,true);	
					  numeric_constant_AST->SetLine( c211->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_OCT_UINT:
	{
		c21 = LT(1);
		if ( inputState->guessing == 0 ) {
			c21_AST = astFactory->create(c21);
		}
		match(CONSTANT_OCT_UINT);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c21->getText());
					  numeric_constant_AST->Text2UInt(8);	
					  numeric_constant_AST->SetLine( c21->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_FLOAT:
	{
		c22 = LT(1);
		if ( inputState->guessing == 0 ) {
			c22_AST = astFactory->create(c22);
		}
		match(CONSTANT_FLOAT);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c22->getText());
					  numeric_constant_AST->Text2Float();	
					  numeric_constant_AST->SetLine( c22->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_DOUBLE:
	{
		c23 = LT(1);
		if ( inputState->guessing == 0 ) {
			c23_AST = astFactory->create(c23);
		}
		match(CONSTANT_DOUBLE);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c23->getText());
					  numeric_constant_AST->Text2Double();	
					  numeric_constant_AST->SetLine( c23->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_BIN_BYTE:
	{
		c24 = LT(1);
		if ( inputState->guessing == 0 ) {
			c24_AST = astFactory->create(c24);
		}
		match(CONSTANT_BIN_BYTE);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c24->getText());
					  numeric_constant_AST->Text2Byte(2);	
					  numeric_constant_AST->SetLine( c24->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_BIN_LONG:
	{
		c25 = LT(1);
		if ( inputState->guessing == 0 ) {
			c25_AST = astFactory->create(c25);
		}
		match(CONSTANT_BIN_LONG);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c25->getText());
					  numeric_constant_AST->Text2Long(2);	
					  numeric_constant_AST->SetLine( c25->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_BIN_LONG64:
	{
		c26 = LT(1);
		if ( inputState->guessing == 0 ) {
			c26_AST = astFactory->create(c26);
		}
		match(CONSTANT_BIN_LONG64);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c26->getText());
					  numeric_constant_AST->Text2Long64(2);	
					  numeric_constant_AST->SetLine( c26->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_BIN_INT:
	{
		c27 = LT(1);
		if ( inputState->guessing == 0 ) {
			c27_AST = astFactory->create(c27);
		}
		match(CONSTANT_BIN_INT);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c27->getText());
					  numeric_constant_AST->Text2Int(2);	
					  numeric_constant_AST->SetLine( c27->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_BIN_I:
	{
		c277 = LT(1);
		if ( inputState->guessing == 0 ) {
			c277_AST = astFactory->create(c277);
		}
		match(CONSTANT_BIN_I);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c277->getText());
			if( compileOpt & DEFINT32)
			numeric_constant_AST->Text2Long(2,true);	
			else
			numeric_constant_AST->Text2Int(2,true);	
					  numeric_constant_AST->SetLine( c277->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_BIN_ULONG:
	{
		c28 = LT(1);
		if ( inputState->guessing == 0 ) {
			c28_AST = astFactory->create(c28);
		}
		match(CONSTANT_BIN_ULONG);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c28->getText());
					  numeric_constant_AST->Text2ULong(2);	
					  numeric_constant_AST->SetLine( c28->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_BIN_ULONG64:
	{
		c29 = LT(1);
		if ( inputState->guessing == 0 ) {
			c29_AST = astFactory->create(c29);
		}
		match(CONSTANT_BIN_ULONG64);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c29->getText());
					  numeric_constant_AST->Text2ULong64(2);	
					  numeric_constant_AST->SetLine( c29->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_BIN_UI:
	{
		c300 = LT(1);
		if ( inputState->guessing == 0 ) {
			c300_AST = astFactory->create(c300);
		}
		match(CONSTANT_BIN_UI);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c300->getText());
			if( compileOpt & DEFINT32)
			numeric_constant_AST->Text2ULong(2,true);	
			else
			numeric_constant_AST->Text2UInt(2,true);	
					  numeric_constant_AST->SetLine( c300->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_BIN_UINT:
	{
		c30 = LT(1);
		if ( inputState->guessing == 0 ) {
			c30_AST = astFactory->create(c30);
		}
		match(CONSTANT_BIN_UINT);
		if ( inputState->guessing==0 ) {
			numeric_constant_AST = RefDNode(currentAST.root);
			numeric_constant_AST=astFactory->create(CONSTANT,c30->getText());
					  numeric_constant_AST->Text2UInt(2);	
					  numeric_constant_AST->SetLine( c30->getLine());	
					
			currentAST.root = numeric_constant_AST;
			if ( numeric_constant_AST!=RefDNode(antlr::nullAST) &&
				numeric_constant_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = numeric_constant_AST->getFirstChild();
			else
				currentAST.child = numeric_constant_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = numeric_constant_AST;
}

void GDLParser::arrayindex_list() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode arrayindex_list_AST = RefDNode(antlr::nullAST);
			
	int rank = 1;
	
	
	if ((LA(1) == LSQUARE)) {
		match(LSQUARE);
		arrayindex();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{ // ( ... )*
		for (;;) {
			if (((LA(1) == COMMA))&&(++rank <= MAXRANK)) {
				match(COMMA);
				arrayindex();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
			}
			else {
				goto _loop154;
			}
			
		}
		_loop154:;
		} // ( ... )*
		match(RSQUARE);
		arrayindex_list_AST = RefDNode(currentAST.root);
	}
	else if (((LA(1) == LBRACE))&&( IsRelaxed())) {
		match(LBRACE);
		arrayindex();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{ // ( ... )*
		for (;;) {
			if (((LA(1) == COMMA))&&(++rank <= MAXRANK)) {
				match(COMMA);
				arrayindex();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
			}
			else {
				goto _loop156;
			}
			
		}
		_loop156:;
		} // ( ... )*
		match(RBRACE);
		arrayindex_list_AST = RefDNode(currentAST.root);
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	returnAST = arrayindex_list_AST;
}

void GDLParser::arrayindex() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode arrayindex_AST = RefDNode(antlr::nullAST);
	
	{
	bool synPredMatched162 = false;
	if (((LA(1) == ASTERIX) && (LA(2) == COMMA || LA(2) == RBRACE || LA(2) == RSQUARE))) {
		int _m162 = mark();
		synPredMatched162 = true;
		inputState->guessing++;
		try {
			{
			match(ASTERIX);
			{
			switch ( LA(1)) {
			case COMMA:
			{
				match(COMMA);
				break;
			}
			case RSQUARE:
			{
				match(RSQUARE);
				break;
			}
			default:
				if (((LA(1) == RBRACE))&&( IsRelaxed())) {
					match(RBRACE);
				}
			else {
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			}
			}
			}
		}
		catch (antlr::RecognitionException& pe) {
			synPredMatched162 = false;
		}
		rewind(_m162);
		inputState->guessing--;
	}
	if ( synPredMatched162 ) {
		all_elements();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
	}
	else if ((_tokenSet_11.member(LA(1))) && (_tokenSet_17.member(LA(2)))) {
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{
		switch ( LA(1)) {
		case COLON:
		{
			match(COLON);
			{
			bool synPredMatched167 = false;
			if (((LA(1) == ASTERIX) && (_tokenSet_18.member(LA(2))))) {
				int _m167 = mark();
				synPredMatched167 = true;
				inputState->guessing++;
				try {
					{
					match(ASTERIX);
					{
					switch ( LA(1)) {
					case COMMA:
					{
						match(COMMA);
						break;
					}
					case RSQUARE:
					{
						match(RSQUARE);
						break;
					}
					case COLON:
					{
						match(COLON);
						break;
					}
					default:
						if (((LA(1) == RBRACE))&&( IsRelaxed())) {
							match(RBRACE);
						}
					else {
						throw antlr::NoViableAltException(LT(1), getFilename());
					}
					}
					}
					}
				}
				catch (antlr::RecognitionException& pe) {
					synPredMatched167 = false;
				}
				rewind(_m167);
				inputState->guessing--;
			}
			if ( synPredMatched167 ) {
				all_elements();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
			}
			else if ((_tokenSet_11.member(LA(1))) && (_tokenSet_17.member(LA(2)))) {
				expr();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
			}
			else {
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			
			}
			{
			switch ( LA(1)) {
			case COLON:
			{
				match(COLON);
				{
				bool synPredMatched172 = false;
				if (((LA(1) == ASTERIX) && (LA(2) == COMMA || LA(2) == RBRACE || LA(2) == RSQUARE))) {
					int _m172 = mark();
					synPredMatched172 = true;
					inputState->guessing++;
					try {
						{
						match(ASTERIX);
						{
						switch ( LA(1)) {
						case COMMA:
						{
							match(COMMA);
							break;
						}
						case RSQUARE:
						{
							match(RSQUARE);
							break;
						}
						default:
							if (((LA(1) == RBRACE))&&( IsRelaxed())) {
								match(RBRACE);
							}
						else {
							throw antlr::NoViableAltException(LT(1), getFilename());
						}
						}
						}
						}
					}
					catch (antlr::RecognitionException& pe) {
						synPredMatched172 = false;
					}
					rewind(_m172);
					inputState->guessing--;
				}
				if ( synPredMatched172 ) {
					match(ASTERIX);
					if ( inputState->guessing==0 ) {
						
						throw 
						GDLException( "n:n:* subscript form not allowed.");
						
					}
				}
				else if ((_tokenSet_11.member(LA(1))) && (_tokenSet_19.member(LA(2)))) {
					expr();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
				}
				else {
					throw antlr::NoViableAltException(LT(1), getFilename());
				}
				
				}
				break;
			}
			case COMMA:
			case RBRACE:
			case RSQUARE:
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
		case COMMA:
		case RBRACE:
		case RSQUARE:
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
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	}
	if ( inputState->guessing==0 ) {
		arrayindex_AST = RefDNode(currentAST.root);
		arrayindex_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(ARRAYIX,"arrayix")))->add(antlr::RefAST(arrayindex_AST))));
		currentAST.root = arrayindex_AST;
		if ( arrayindex_AST!=RefDNode(antlr::nullAST) &&
			arrayindex_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = arrayindex_AST->getFirstChild();
		else
			currentAST.child = arrayindex_AST;
		currentAST.advanceChildToEnd();
	}
	arrayindex_AST = RefDNode(currentAST.root);
	returnAST = arrayindex_AST;
}

void GDLParser::all_elements() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode all_elements_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp177_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp177_AST = astFactory->create(LT(1));
	}
	match(ASTERIX);
	if ( inputState->guessing==0 ) {
		all_elements_AST = RefDNode(currentAST.root);
		all_elements_AST = RefDNode(astFactory->make((new antlr::ASTArray(1))->add(antlr::RefAST(astFactory->create(ALL,"*")))));
		currentAST.root = all_elements_AST;
		if ( all_elements_AST!=RefDNode(antlr::nullAST) &&
			all_elements_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = all_elements_AST->getFirstChild();
		else
			currentAST.child = all_elements_AST;
		currentAST.advanceChildToEnd();
	}
	returnAST = all_elements_AST;
}

void GDLParser::sysvar() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode sysvar_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp178_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp178_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp178_AST));
	}
	match(SYSVARNAME);
	if ( inputState->guessing==0 ) {
		sysvar_AST = RefDNode(currentAST.root);
		sysvar_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(SYSVAR,"SYSVAR")))->add(antlr::RefAST(sysvar_AST))));
		currentAST.root = sysvar_AST;
		if ( sysvar_AST!=RefDNode(antlr::nullAST) &&
			sysvar_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = sysvar_AST->getFirstChild();
		else
			currentAST.child = sysvar_AST;
		currentAST.advanceChildToEnd();
	}
	sysvar_AST = RefDNode(currentAST.root);
	returnAST = sysvar_AST;
}

void GDLParser::var() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode var_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  id = antlr::nullToken;
	RefDNode id_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  ih = antlr::nullToken;
	RefDNode ih_AST = RefDNode(antlr::nullAST);
	
	{
	switch ( LA(1)) {
	case IDENTIFIER:
	{
		id = LT(1);
		if ( inputState->guessing == 0 ) {
			id_AST = astFactory->create(id);
		}
		match(IDENTIFIER);
		if ( inputState->guessing==0 ) {
			var_AST = RefDNode(currentAST.root);
			
			var_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(VAR,"VAR")))->add(antlr::RefAST(id_AST))));
			
			currentAST.root = var_AST;
			if ( var_AST!=RefDNode(antlr::nullAST) &&
				var_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = var_AST->getFirstChild();
			else
				currentAST.child = var_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case INHERITS:
	{
		ih = LT(1);
		if ( inputState->guessing == 0 ) {
			ih_AST = astFactory->create(ih);
		}
		match(INHERITS);
		if ( inputState->guessing==0 ) {
			var_AST = RefDNode(currentAST.root);
			
			ih_AST->setType( IDENTIFIER);
			var_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(VAR,"VAR")))->add(antlr::RefAST(ih_AST))));
			
			currentAST.root = var_AST;
			if ( var_AST!=RefDNode(antlr::nullAST) &&
				var_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = var_AST->getFirstChild();
			else
				currentAST.child = var_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	returnAST = var_AST;
}

void GDLParser::brace_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode brace_expr_AST = RefDNode(antlr::nullAST);
	
	match(LBRACE);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	match(RBRACE);
	if ( inputState->guessing==0 ) {
		brace_expr_AST = RefDNode(currentAST.root);
		brace_expr_AST = 
					RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(EXPR,"expr")))->add(antlr::RefAST(brace_expr_AST))));
		currentAST.root = brace_expr_AST;
		if ( brace_expr_AST!=RefDNode(antlr::nullAST) &&
			brace_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = brace_expr_AST->getFirstChild();
		else
			currentAST.child = brace_expr_AST;
		currentAST.advanceChildToEnd();
	}
	brace_expr_AST = RefDNode(currentAST.root);
	returnAST = brace_expr_AST;
}

void GDLParser::array_expr_1st_sub() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode array_expr_1st_sub_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case IDENTIFIER:
	case INHERITS:
	{
		var();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		array_expr_1st_sub_AST = RefDNode(currentAST.root);
		break;
	}
	case SYSVARNAME:
	{
		sysvar();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		array_expr_1st_sub_AST = RefDNode(currentAST.root);
		break;
	}
	case LBRACE:
	{
		brace_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		array_expr_1st_sub_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = array_expr_1st_sub_AST;
}

void GDLParser::array_expr_1st() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode array_expr_1st_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode al_AST = RefDNode(antlr::nullAST);
	
	array_expr_1st_sub();
	if (inputState->guessing==0) {
		e_AST = returnAST;
	}
	{
	switch ( LA(1)) {
	case LBRACE:
	case LSQUARE:
	{
		arrayindex_list();
		if (inputState->guessing==0) {
			al_AST = returnAST;
		}
		if ( inputState->guessing==0 ) {
			array_expr_1st_AST = RefDNode(currentAST.root);
			array_expr_1st_AST = 
							RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ARRAYEXPR,"arrayexpr")))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(al_AST))));
			currentAST.root = array_expr_1st_AST;
			if ( array_expr_1st_AST!=RefDNode(antlr::nullAST) &&
				array_expr_1st_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = array_expr_1st_AST->getFirstChild();
			else
				currentAST.child = array_expr_1st_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case antlr::Token::EOF_TYPE:
	case AND_OP:
	case DO:
	case ELSE:
	case EQ_OP:
	case GE_OP:
	case GT_OP:
	case LE_OP:
	case LT_OP:
	case MOD_OP:
	case NE_OP:
	case OF:
	case OR_OP:
	case THEN:
	case UNTIL:
	case XOR_OP:
	case COMMA:
	case COLON:
	case END_U:
	case EQUAL:
	case DEC:
	case INC:
	case AND_OP_EQ:
	case ASTERIX_EQ:
	case EQ_OP_EQ:
	case GE_OP_EQ:
	case GTMARK_EQ:
	case GT_OP_EQ:
	case LE_OP_EQ:
	case LTMARK_EQ:
	case LT_OP_EQ:
	case MATRIX_OP1_EQ:
	case MATRIX_OP2_EQ:
	case MINUS_EQ:
	case MOD_OP_EQ:
	case NE_OP_EQ:
	case OR_OP_EQ:
	case PLUS_EQ:
	case POW_EQ:
	case SLASH_EQ:
	case XOR_OP_EQ:
	case MEMBER:
	case RBRACE:
	case SLASH:
	case RSQUARE:
	case RCURLY:
	case ASTERIX:
	case DOT:
	case POW:
	case MATRIX_OP1:
	case MATRIX_OP2:
	case PLUS:
	case MINUS:
	case LTMARK:
	case GTMARK:
	case LOG_AND:
	case LOG_OR:
	case QUESTION:
	{
		if ( inputState->guessing==0 ) {
			array_expr_1st_AST = RefDNode(currentAST.root);
			array_expr_1st_AST = e_AST;
			currentAST.root = array_expr_1st_AST;
			if ( array_expr_1st_AST!=RefDNode(antlr::nullAST) &&
				array_expr_1st_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = array_expr_1st_AST->getFirstChild();
			else
				currentAST.child = array_expr_1st_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	returnAST = array_expr_1st_AST;
}

void GDLParser::array_expr_nth_sub() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode array_expr_nth_sub_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case IDENTIFIER:
	{
		RefDNode tmp181_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp181_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp181_AST));
		}
		match(IDENTIFIER);
		array_expr_nth_sub_AST = RefDNode(currentAST.root);
		break;
	}
	case LBRACE:
	{
		brace_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		array_expr_nth_sub_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = array_expr_nth_sub_AST;
}

void GDLParser::array_expr_nth() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode array_expr_nth_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode al_AST = RefDNode(antlr::nullAST);
	
	array_expr_nth_sub();
	if (inputState->guessing==0) {
		e_AST = returnAST;
	}
	{
	switch ( LA(1)) {
	case LBRACE:
	case LSQUARE:
	{
		arrayindex_list();
		if (inputState->guessing==0) {
			al_AST = returnAST;
		}
		if ( inputState->guessing==0 ) {
			array_expr_nth_AST = RefDNode(currentAST.root);
			array_expr_nth_AST = 
			RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ARRAYEXPR,"arrayexpr")))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(al_AST))));
			currentAST.root = array_expr_nth_AST;
			if ( array_expr_nth_AST!=RefDNode(antlr::nullAST) &&
				array_expr_nth_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = array_expr_nth_AST->getFirstChild();
			else
				currentAST.child = array_expr_nth_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case antlr::Token::EOF_TYPE:
	{
		if ( inputState->guessing==0 ) {
			array_expr_nth_AST = RefDNode(currentAST.root);
			array_expr_nth_AST = e_AST;
			currentAST.root = array_expr_nth_AST;
			if ( array_expr_nth_AST!=RefDNode(antlr::nullAST) &&
				array_expr_nth_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = array_expr_nth_AST->getFirstChild();
			else
				currentAST.child = array_expr_nth_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	returnAST = array_expr_nth_AST;
}

void GDLParser::tag_array_expr_nth_sub() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_array_expr_nth_sub_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  s = antlr::nullToken;
	RefDNode s_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  e = antlr::nullToken;
	RefDNode e_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case IDENTIFIER:
	{
		RefDNode tmp182_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp182_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp182_AST));
		}
		match(IDENTIFIER);
		tag_array_expr_nth_sub_AST = RefDNode(currentAST.root);
		break;
	}
	case SYSVARNAME:
	{
		s = LT(1);
		if ( inputState->guessing == 0 ) {
			s_AST = astFactory->create(s);
			astFactory->addASTChild(currentAST, antlr::RefAST(s_AST));
		}
		match(SYSVARNAME);
		if ( inputState->guessing==0 ) {
			s_AST->setType( IDENTIFIER); /* #s->setText( "!" + #s->getText()); */
		}
		tag_array_expr_nth_sub_AST = RefDNode(currentAST.root);
		break;
	}
	case EXCLAMATION:
	{
		e = LT(1);
		if ( inputState->guessing == 0 ) {
			e_AST = astFactory->create(e);
			astFactory->addASTChild(currentAST, antlr::RefAST(e_AST));
		}
		match(EXCLAMATION);
		if ( inputState->guessing==0 ) {
			e_AST->setType( IDENTIFIER);
		}
		tag_array_expr_nth_sub_AST = RefDNode(currentAST.root);
		break;
	}
	case LBRACE:
	{
		brace_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		tag_array_expr_nth_sub_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = tag_array_expr_nth_sub_AST;
}

void GDLParser::tag_array_expr_nth() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_array_expr_nth_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode al_AST = RefDNode(antlr::nullAST);
	
	tag_array_expr_nth_sub();
	if (inputState->guessing==0) {
		e_AST = returnAST;
	}
	{
	switch ( LA(1)) {
	case LBRACE:
	case LSQUARE:
	{
		arrayindex_list();
		if (inputState->guessing==0) {
			al_AST = returnAST;
		}
		if ( inputState->guessing==0 ) {
			tag_array_expr_nth_AST = RefDNode(currentAST.root);
			tag_array_expr_nth_AST = 
			RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ARRAYEXPR,"arrayexpr")))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(al_AST))));
			currentAST.root = tag_array_expr_nth_AST;
			if ( tag_array_expr_nth_AST!=RefDNode(antlr::nullAST) &&
				tag_array_expr_nth_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = tag_array_expr_nth_AST->getFirstChild();
			else
				currentAST.child = tag_array_expr_nth_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case antlr::Token::EOF_TYPE:
	case AND_OP:
	case DO:
	case ELSE:
	case EQ_OP:
	case GE_OP:
	case GT_OP:
	case LE_OP:
	case LT_OP:
	case MOD_OP:
	case NE_OP:
	case OF:
	case OR_OP:
	case THEN:
	case UNTIL:
	case XOR_OP:
	case COMMA:
	case COLON:
	case END_U:
	case EQUAL:
	case DEC:
	case INC:
	case AND_OP_EQ:
	case ASTERIX_EQ:
	case EQ_OP_EQ:
	case GE_OP_EQ:
	case GTMARK_EQ:
	case GT_OP_EQ:
	case LE_OP_EQ:
	case LTMARK_EQ:
	case LT_OP_EQ:
	case MATRIX_OP1_EQ:
	case MATRIX_OP2_EQ:
	case MINUS_EQ:
	case MOD_OP_EQ:
	case NE_OP_EQ:
	case OR_OP_EQ:
	case PLUS_EQ:
	case POW_EQ:
	case SLASH_EQ:
	case XOR_OP_EQ:
	case MEMBER:
	case RBRACE:
	case SLASH:
	case RSQUARE:
	case RCURLY:
	case ASTERIX:
	case DOT:
	case POW:
	case MATRIX_OP1:
	case MATRIX_OP2:
	case PLUS:
	case MINUS:
	case LTMARK:
	case GTMARK:
	case LOG_AND:
	case LOG_OR:
	case QUESTION:
	{
		if ( inputState->guessing==0 ) {
			tag_array_expr_nth_AST = RefDNode(currentAST.root);
			tag_array_expr_nth_AST = e_AST;
			currentAST.root = tag_array_expr_nth_AST;
			if ( tag_array_expr_nth_AST!=RefDNode(antlr::nullAST) &&
				tag_array_expr_nth_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = tag_array_expr_nth_AST->getFirstChild();
			else
				currentAST.child = tag_array_expr_nth_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	returnAST = tag_array_expr_nth_AST;
}

int  GDLParser::tag_access_keeplast() {
	int nDot;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_access_keeplast_AST = RefDNode(antlr::nullAST);
	
	int t;
	bool parent = false;
	nDot=1;
	
	
	match(DOT);
	{
	bool synPredMatched189 = false;
	if (((_tokenSet_20.member(LA(1))) && (_tokenSet_5.member(LA(2))))) {
		int _m189 = mark();
		synPredMatched189 = true;
		inputState->guessing++;
		try {
			{
			tag_array_expr_nth();
			match(DOT);
			}
		}
		catch (antlr::RecognitionException& pe) {
			synPredMatched189 = false;
		}
		rewind(_m189);
		inputState->guessing--;
	}
	if ( synPredMatched189 ) {
		{
		tag_array_expr_nth();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		t=tag_access_keeplast();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			nDot += t;
		}
		}
	}
	else if ((LA(1) == IDENTIFIER) && (_tokenSet_21.member(LA(2)))) {
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	}
	tag_access_keeplast_AST = RefDNode(currentAST.root);
	returnAST = tag_access_keeplast_AST;
	return nDot;
}

SizeT  GDLParser::tag_access() {
	SizeT nDot;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_access_AST = RefDNode(antlr::nullAST);
	
	nDot=0;
	
	
	{ // ( ... )+
	int _cnt195=0;
	for (;;) {
		if ((LA(1) == DOT)) {
			match(DOT);
			if ( inputState->guessing==0 ) {
				++nDot;
			}
			tag_array_expr_nth();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			if ( _cnt195>=1 ) { goto _loop195; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt195++;
	}
	_loop195:;
	}  // ( ... )+
	tag_access_AST = RefDNode(currentAST.root);
	returnAST = tag_access_AST;
	return nDot;
}

void GDLParser::deref_dot_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode deref_dot_expr_AST = RefDNode(antlr::nullAST);
	RefDNode a1_AST = RefDNode(antlr::nullAST);
	
	RefDNode dot;
	SizeT nDot;
	
	
	switch ( LA(1)) {
	case IDENTIFIER:
	case INHERITS:
	case LBRACE:
	case SYSVARNAME:
	{
		array_expr_1st();
		if (inputState->guessing==0) {
			a1_AST = returnAST;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{
		nDot=tag_access();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			deref_dot_expr_AST = RefDNode(currentAST.root);
			
			
			dot=astFactory->create(DOT,".");
			dot->SetNDot( nDot);    
			dot->SetLine( a1_AST->getLine());
			
			deref_dot_expr_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(dot))->add(antlr::RefAST(deref_dot_expr_AST))));
			
			currentAST.root = deref_dot_expr_AST;
			if ( deref_dot_expr_AST!=RefDNode(antlr::nullAST) &&
				deref_dot_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = deref_dot_expr_AST->getFirstChild();
			else
				currentAST.child = deref_dot_expr_AST;
			currentAST.advanceChildToEnd();
		}
		}
		deref_dot_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case ASTERIX:
	{
		match(ASTERIX);
		deref_dot_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			deref_dot_expr_AST = RefDNode(currentAST.root);
			deref_dot_expr_AST = 
						RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(DEREF,"deref")))->add(antlr::RefAST(deref_dot_expr_AST))));
			currentAST.root = deref_dot_expr_AST;
			if ( deref_dot_expr_AST!=RefDNode(antlr::nullAST) &&
				deref_dot_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = deref_dot_expr_AST->getFirstChild();
			else
				currentAST.child = deref_dot_expr_AST;
			currentAST.advanceChildToEnd();
		}
		deref_dot_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = deref_dot_expr_AST;
}

bool  GDLParser::member_function_call() {
	bool parent;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode member_function_call_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  s = antlr::nullToken;
	RefDNode s_AST = RefDNode(antlr::nullAST);
	
	if ( inputState->guessing==0 ) {
		parent = false;
	}
	match(MEMBER);
	{
	if ((LA(1) == IDENTIFIER) && (LA(2) == METHOD)) {
		s = LT(1);
		if ( inputState->guessing == 0 ) {
			s_AST = astFactory->create(s);
			astFactory->addASTChild(currentAST, antlr::RefAST(s_AST));
		}
		match(IDENTIFIER);
		match(METHOD);
		if ( inputState->guessing==0 ) {
			
			// here we translate IDL_OBECT to GDL_OBJECT for source code compatibility
			{
			if( s_AST->getText() == "IDL_OBJECT")
			s_AST->setText(GDL_OBJECT_NAME);
			else if( s_AST->getText() == "IDL_CONTAINER")
			s_AST->setText(GDL_CONTAINER_NAME);
			}
			parent = true;
			
		}
	}
	else if ((LA(1) == IDENTIFIER) && (LA(2) == LBRACE)) {
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	}
	formal_function_call();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	member_function_call_AST = RefDNode(currentAST.root);
	returnAST = member_function_call_AST;
	return parent;
}

void GDLParser::member_function_call_dot() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode member_function_call_dot_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  s = antlr::nullToken;
	RefDNode s_AST = RefDNode(antlr::nullAST);
	
	match(DOT);
	{
	s = LT(1);
	if ( inputState->guessing == 0 ) {
		s_AST = astFactory->create(s);
		astFactory->addASTChild(currentAST, antlr::RefAST(s_AST));
	}
	match(IDENTIFIER);
	match(METHOD);
	if ( inputState->guessing==0 ) {
		
		if( s_AST->getText() == "IDL_OBJECT")
		s_AST->setText(GDL_OBJECT_NAME);
		else if( s_AST->getText() == "IDL_CONTAINER")
		s_AST->setText(GDL_CONTAINER_NAME);
		
	}
	}
	formal_function_call();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	member_function_call_dot_AST = RefDNode(currentAST.root);
	returnAST = member_function_call_dot_AST;
}

void GDLParser::arrayexpr_mfcall() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode arrayexpr_mfcall_AST = RefDNode(antlr::nullAST);
	RefDNode a1_AST = RefDNode(antlr::nullAST);
	RefDNode t1_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  id = antlr::nullToken;
	RefDNode id_AST = RefDNode(antlr::nullAST);
	RefDNode al_AST = RefDNode(antlr::nullAST);
	RefDNode deref_arrayexpr_mfcall_AST = RefDNode(antlr::nullAST);
	
	RefDNode dot;
	RefDNode tag;
	int nDot;
	
	
	switch ( LA(1)) {
	case IDENTIFIER:
	case INHERITS:
	case LBRACE:
	case SYSVARNAME:
	{
		array_expr_1st();
		if (inputState->guessing==0) {
			a1_AST = returnAST;
		}
		{
		nDot=tag_access_keeplast();
		if (inputState->guessing==0) {
			t1_AST = returnAST;
		}
		if ( inputState->guessing==0 ) {
			
			if( --nDot > 0)
			{
			dot=astFactory->create(DOT,"DOT_A_MF");
			dot->SetNDot( nDot);    
			dot->SetLine( a1_AST->getLine());
			tag = RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(dot))->add(antlr::RefAST(a1_AST))->add(antlr::RefAST(t1_AST))));
			}
			
		}
		}
		id = LT(1);
		if ( inputState->guessing == 0 ) {
			id_AST = astFactory->create(id);
		}
		match(IDENTIFIER);
		arrayindex_list();
		if (inputState->guessing==0) {
			al_AST = returnAST;
		}
		if ( inputState->guessing==0 ) {
			arrayexpr_mfcall_AST = RefDNode(currentAST.root);
			
			if( nDot > 0)
			arrayexpr_mfcall_AST = RefDNode(astFactory->make((new antlr::ASTArray(4))->add(antlr::RefAST(astFactory->create(ARRAYEXPR_MFCALL,"arrayexpr_mfcall")))->add(antlr::RefAST(tag))->add(antlr::RefAST(id_AST))->add(antlr::RefAST(al_AST))));
			else
			arrayexpr_mfcall_AST = RefDNode(astFactory->make((new antlr::ASTArray(4))->add(antlr::RefAST(astFactory->create(ARRAYEXPR_MFCALL,"arrayexpr_mfcall")))->add(antlr::RefAST(a1_AST))->add(antlr::RefAST(id_AST))->add(antlr::RefAST(al_AST))));
			
			currentAST.root = arrayexpr_mfcall_AST;
			if ( arrayexpr_mfcall_AST!=RefDNode(antlr::nullAST) &&
				arrayexpr_mfcall_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = arrayexpr_mfcall_AST->getFirstChild();
			else
				currentAST.child = arrayexpr_mfcall_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case ASTERIX:
	{
		RefDNode tmp190_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp190_AST = astFactory->create(LT(1));
		}
		match(ASTERIX);
		arrayexpr_mfcall();
		if (inputState->guessing==0) {
			deref_arrayexpr_mfcall_AST = returnAST;
		}
		if ( inputState->guessing==0 ) {
			arrayexpr_mfcall_AST = RefDNode(currentAST.root);
			arrayexpr_mfcall_AST = 
						RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(DEREF,"deref")))->add(antlr::RefAST(deref_arrayexpr_mfcall_AST))));
			currentAST.root = arrayexpr_mfcall_AST;
			if ( arrayexpr_mfcall_AST!=RefDNode(antlr::nullAST) &&
				arrayexpr_mfcall_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = arrayexpr_mfcall_AST->getFirstChild();
			else
				currentAST.child = arrayexpr_mfcall_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = arrayexpr_mfcall_AST;
}

void GDLParser::primary_expr_tail() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode primary_expr_tail_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  ls = antlr::nullToken;
	RefDNode ls_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  lc = antlr::nullToken;
	RefDNode lc_AST = RefDNode(antlr::nullAST);
	
	bool parent;
	
	
	bool synPredMatched211 = false;
	if (((_tokenSet_4.member(LA(1))) && (_tokenSet_22.member(LA(2))))) {
		int _m211 = mark();
		synPredMatched211 = true;
		inputState->guessing++;
		try {
			{
			deref_dot_expr();
			}
		}
		catch (antlr::RecognitionException& pe) {
			synPredMatched211 = false;
		}
		rewind(_m211);
		inputState->guessing--;
	}
	if ( synPredMatched211 ) {
		deref_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{
		switch ( LA(1)) {
		case MEMBER:
		{
			parent=member_function_call();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			if ( inputState->guessing==0 ) {
				primary_expr_tail_AST = RefDNode(currentAST.root);
				
				if( parent)
				{
				primary_expr_tail_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MFCALL_PARENT,"mfcall::")))->add(antlr::RefAST(primary_expr_tail_AST))));
				}
				else
				{
				primary_expr_tail_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MFCALL,"mfcall")))->add(antlr::RefAST(primary_expr_tail_AST))));
				}
				
				currentAST.root = primary_expr_tail_AST;
				if ( primary_expr_tail_AST!=RefDNode(antlr::nullAST) &&
					primary_expr_tail_AST->getFirstChild() != RefDNode(antlr::nullAST) )
					  currentAST.child = primary_expr_tail_AST->getFirstChild();
				else
					currentAST.child = primary_expr_tail_AST;
				currentAST.advanceChildToEnd();
			}
			break;
		}
		case antlr::Token::EOF_TYPE:
		case AND_OP:
		case DO:
		case ELSE:
		case EQ_OP:
		case GE_OP:
		case GT_OP:
		case LE_OP:
		case LT_OP:
		case MOD_OP:
		case NE_OP:
		case OF:
		case OR_OP:
		case THEN:
		case UNTIL:
		case XOR_OP:
		case COMMA:
		case COLON:
		case END_U:
		case DEC:
		case INC:
		case RBRACE:
		case SLASH:
		case RSQUARE:
		case RCURLY:
		case ASTERIX:
		case POW:
		case MATRIX_OP1:
		case MATRIX_OP2:
		case PLUS:
		case MINUS:
		case LTMARK:
		case GTMARK:
		case LOG_AND:
		case LOG_OR:
		case QUESTION:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		primary_expr_tail_AST = RefDNode(currentAST.root);
	}
	else {
		bool synPredMatched216 = false;
		if (((LA(1) == IDENTIFIER || LA(1) == INHERITS) && (LA(2) == LBRACE || LA(2) == LSQUARE))) {
			int _m216 = mark();
			synPredMatched216 = true;
			inputState->guessing++;
			try {
				{
				match(IDENTIFIER);
				match(LBRACE);
				expr();
				{ // ( ... )*
				for (;;) {
					if ((LA(1) == COMMA)) {
						match(COMMA);
						expr();
					}
					else {
						goto _loop215;
					}
					
				}
				_loop215:;
				} // ( ... )*
				match(RBRACE);
				}
			}
			catch (antlr::RecognitionException& pe) {
				synPredMatched216 = false;
			}
			rewind(_m216);
			inputState->guessing--;
		}
		if ( synPredMatched216 ) {
			{
			if (((LA(1) == IDENTIFIER) && (LA(2) == LBRACE))&&( IsFun(LT(1)))) {
				formal_function_call();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				if ( inputState->guessing==0 ) {
					primary_expr_tail_AST = RefDNode(currentAST.root);
					
					primary_expr_tail_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(FCALL,"fcall")))->add(antlr::RefAST(primary_expr_tail_AST))));
					
					currentAST.root = primary_expr_tail_AST;
					if ( primary_expr_tail_AST!=RefDNode(antlr::nullAST) &&
						primary_expr_tail_AST->getFirstChild() != RefDNode(antlr::nullAST) )
						  currentAST.child = primary_expr_tail_AST->getFirstChild();
					else
						currentAST.child = primary_expr_tail_AST;
					currentAST.advanceChildToEnd();
				}
			}
			else {
				bool synPredMatched219 = false;
				if (((LA(1) == IDENTIFIER || LA(1) == INHERITS) && (LA(2) == LBRACE || LA(2) == LSQUARE))) {
					int _m219 = mark();
					synPredMatched219 = true;
					inputState->guessing++;
					try {
						{
						var();
						arrayindex_list();
						}
					}
					catch (antlr::RecognitionException& pe) {
						synPredMatched219 = false;
					}
					rewind(_m219);
					inputState->guessing--;
				}
				if ( synPredMatched219 ) {
					var();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					arrayindex_list();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					if ( inputState->guessing==0 ) {
						primary_expr_tail_AST = RefDNode(currentAST.root);
						
						primary_expr_tail_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(ARRAYEXPR_FCALL,"arrayexpr_fcall")))->add(antlr::RefAST(primary_expr_tail_AST))));
						
						currentAST.root = primary_expr_tail_AST;
						if ( primary_expr_tail_AST!=RefDNode(antlr::nullAST) &&
							primary_expr_tail_AST->getFirstChild() != RefDNode(antlr::nullAST) )
							  currentAST.child = primary_expr_tail_AST->getFirstChild();
						else
							currentAST.child = primary_expr_tail_AST;
						currentAST.advanceChildToEnd();
					}
				}
				else if ((LA(1) == IDENTIFIER) && (LA(2) == LBRACE)) {
					formal_function_call();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					if ( inputState->guessing==0 ) {
						primary_expr_tail_AST = RefDNode(currentAST.root);
						
						primary_expr_tail_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(FCALL,"fcall")))->add(antlr::RefAST(primary_expr_tail_AST))));
						
						currentAST.root = primary_expr_tail_AST;
						if ( primary_expr_tail_AST!=RefDNode(antlr::nullAST) &&
							primary_expr_tail_AST->getFirstChild() != RefDNode(antlr::nullAST) )
							  currentAST.child = primary_expr_tail_AST->getFirstChild();
						else
							currentAST.child = primary_expr_tail_AST;
						currentAST.advanceChildToEnd();
					}
				}
			else {
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			}
			}
			primary_expr_tail_AST = RefDNode(currentAST.root);
		}
		else {
			bool synPredMatched221 = false;
			if (((LA(1) == IDENTIFIER) && (LA(2) == LBRACE))) {
				int _m221 = mark();
				synPredMatched221 = true;
				inputState->guessing++;
				try {
					{
					formal_function_call();
					}
				}
				catch (antlr::RecognitionException& pe) {
					synPredMatched221 = false;
				}
				rewind(_m221);
				inputState->guessing--;
			}
			if ( synPredMatched221 ) {
				formal_function_call();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				if ( inputState->guessing==0 ) {
					primary_expr_tail_AST = RefDNode(currentAST.root);
					primary_expr_tail_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(FCALL,"fcall")))->add(antlr::RefAST(primary_expr_tail_AST))));
					currentAST.root = primary_expr_tail_AST;
					if ( primary_expr_tail_AST!=RefDNode(antlr::nullAST) &&
						primary_expr_tail_AST->getFirstChild() != RefDNode(antlr::nullAST) )
						  currentAST.child = primary_expr_tail_AST->getFirstChild();
					else
						currentAST.child = primary_expr_tail_AST;
					currentAST.advanceChildToEnd();
				}
				primary_expr_tail_AST = RefDNode(currentAST.root);
			}
			else {
				bool synPredMatched223 = false;
				if (((_tokenSet_4.member(LA(1))) && (_tokenSet_22.member(LA(2))))) {
					int _m223 = mark();
					synPredMatched223 = true;
					inputState->guessing++;
					try {
						{
						deref_expr();
						}
					}
					catch (antlr::RecognitionException& pe) {
						synPredMatched223 = false;
					}
					rewind(_m223);
					inputState->guessing--;
				}
				if ( synPredMatched223 ) {
					deref_expr();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					{
					switch ( LA(1)) {
					case MEMBER:
					{
						parent=member_function_call();
						if (inputState->guessing==0) {
							astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
						}
						if ( inputState->guessing==0 ) {
							primary_expr_tail_AST = RefDNode(currentAST.root);
							
							if( parent)
							{
							primary_expr_tail_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MFCALL_PARENT,"mfcall::")))->add(antlr::RefAST(primary_expr_tail_AST))));
							}
							else
							{
							primary_expr_tail_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MFCALL,"mfcall")))->add(antlr::RefAST(primary_expr_tail_AST))));
							}
							
							currentAST.root = primary_expr_tail_AST;
							if ( primary_expr_tail_AST!=RefDNode(antlr::nullAST) &&
								primary_expr_tail_AST->getFirstChild() != RefDNode(antlr::nullAST) )
								  currentAST.child = primary_expr_tail_AST->getFirstChild();
							else
								currentAST.child = primary_expr_tail_AST;
							currentAST.advanceChildToEnd();
						}
						break;
					}
					case antlr::Token::EOF_TYPE:
					case AND_OP:
					case DO:
					case ELSE:
					case EQ_OP:
					case GE_OP:
					case GT_OP:
					case LE_OP:
					case LT_OP:
					case MOD_OP:
					case NE_OP:
					case OF:
					case OR_OP:
					case THEN:
					case UNTIL:
					case XOR_OP:
					case COMMA:
					case COLON:
					case END_U:
					case DEC:
					case INC:
					case RBRACE:
					case SLASH:
					case RSQUARE:
					case RCURLY:
					case ASTERIX:
					case POW:
					case MATRIX_OP1:
					case MATRIX_OP2:
					case PLUS:
					case MINUS:
					case LTMARK:
					case GTMARK:
					case LOG_AND:
					case LOG_OR:
					case QUESTION:
					{
						break;
					}
					default:
					{
						throw antlr::NoViableAltException(LT(1), getFilename());
					}
					}
					}
					primary_expr_tail_AST = RefDNode(currentAST.root);
				}
				else if ((LA(1) == LBRACE) && (_tokenSet_4.member(LA(2)))) {
					assign_expr();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					primary_expr_tail_AST = RefDNode(currentAST.root);
				}
				else if ((LA(1) == LSQUARE) && (_tokenSet_11.member(LA(2)))) {
					array_def();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					primary_expr_tail_AST = RefDNode(currentAST.root);
				}
				else if ((LA(1) == LCURLY) && (_tokenSet_13.member(LA(2)))) {
					struct_def();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					primary_expr_tail_AST = RefDNode(currentAST.root);
				}
				else if ((LA(1) == LSQUARE) && (LA(2) == RSQUARE)) {
					ls = LT(1);
					ls_AST = astFactory->create(ls);
					match(LSQUARE);
					RefDNode tmp191_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp191_AST = astFactory->create(LT(1));
					}
					match(RSQUARE);
					if ( inputState->guessing==0 ) {
						primary_expr_tail_AST = RefDNode(currentAST.root);
						primary_expr_tail_AST=astFactory->create(GDLNULL,"GDLNULL[]");
						primary_expr_tail_AST->SetLine( ls_AST->getLine());
								
						currentAST.root = primary_expr_tail_AST;
						if ( primary_expr_tail_AST!=RefDNode(antlr::nullAST) &&
							primary_expr_tail_AST->getFirstChild() != RefDNode(antlr::nullAST) )
							  currentAST.child = primary_expr_tail_AST->getFirstChild();
						else
							currentAST.child = primary_expr_tail_AST;
						currentAST.advanceChildToEnd();
					}
				}
				else if ((LA(1) == LCURLY) && (LA(2) == RCURLY)) {
					lc = LT(1);
					lc_AST = astFactory->create(lc);
					match(LCURLY);
					RefDNode tmp192_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp192_AST = astFactory->create(LT(1));
					}
					match(RCURLY);
					if ( inputState->guessing==0 ) {
						primary_expr_tail_AST = RefDNode(currentAST.root);
						primary_expr_tail_AST=astFactory->create(GDLNULL,"GDLNULL{}");
						primary_expr_tail_AST->SetLine( lc_AST->getLine());
								
						currentAST.root = primary_expr_tail_AST;
						if ( primary_expr_tail_AST!=RefDNode(antlr::nullAST) &&
							primary_expr_tail_AST->getFirstChild() != RefDNode(antlr::nullAST) )
							  currentAST.child = primary_expr_tail_AST->getFirstChild();
						else
							currentAST.child = primary_expr_tail_AST;
						currentAST.advanceChildToEnd();
					}
				}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}}}
	returnAST = primary_expr_tail_AST;
}

void GDLParser::primary_expr_deref() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode primary_expr_deref_AST = RefDNode(antlr::nullAST);
	
	// the following needs to be updated if the symbols are rearranged (e. g. a symbol is inserted)
	// (it is taken from GDLParser.cpp: const antlr::BitSet GDLParser::_tokenSet_XX)
	const unsigned long _tokenSet_4_data_[] = { 0UL, 0UL, 268435456UL, 1048576UL, 536870912UL, 4UL, 4096UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
	// IDENTIFIER "inherits" LBRACE SYSVARNAME ASTERIX 
	const antlr::BitSet _tokenSet_4(_tokenSet_4_data_,16);
	const unsigned long _tokenSet_5_data_[] = { 0UL, 0UL, 268435456UL, 34603008UL, 536871296UL, 4294967253UL, 5013503UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
	// IDENTIFIER "inherits" "not" DEC INC LBRACE LSQUARE SYSVARNAME LCURLY 
	// CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT 
	// CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI 
	// CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT 
	// CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT 
	// CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT 
	// CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI 
	// CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG 
	// CONSTANT_BIN_LONG64 CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG 
	// CONSTANT_BIN_ULONG64 CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX DOT STRING_LITERAL 
	// PLUS MINUS LOG_NEG 
	const antlr::BitSet _tokenSet_5(_tokenSet_5_data_,16);
	const unsigned long _tokenSet_23_data_[] = { 0UL, 0UL, 268435456UL, 1048576UL, 536870912UL, 21UL, 4096UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
	// IDENTIFIER "inherits" LBRACE LSQUARE SYSVARNAME LCURLY ASTERIX 
	const antlr::BitSet _tokenSet_23(_tokenSet_23_data_,16);
	const unsigned long _tokenSet_24_data_[] = { 2UL, 0UL, 805306368UL, 2549424140UL, 4026532283UL, 4294967295UL, 67108863UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
	// EOF IDENTIFIER "and" "do" "else" "eq" "ge" "gt" "inherits" "le" "lt" 
	// "mod" "ne" "not" "of" "or" "then" "until" "xor" COMMA COLON END_U DEC 
	// INC MEMBER LBRACE RBRACE SLASH LSQUARE RSQUARE SYSVARNAME EXCLAMATION 
	// LCURLY RCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 
	// CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 
	// CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 
	// CONSTANT_INT CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI 
	// CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 
	// CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 
	// CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE 
	// CONSTANT_BIN_LONG CONSTANT_BIN_LONG64 CONSTANT_BIN_INT CONSTANT_BIN_I 
	// CONSTANT_BIN_ULONG CONSTANT_BIN_ULONG64 CONSTANT_BIN_UI CONSTANT_BIN_UINT 
	// ASTERIX DOT STRING_LITERAL POW MATRIX_OP1 MATRIX_OP2 PLUS MINUS LTMARK 
	// GTMARK LOG_NEG LOG_AND LOG_OR QUESTION 
	const antlr::BitSet _tokenSet_24(_tokenSet_24_data_,16);
	
	bool parent;
	
	bool skip;
	int markIn = mark();
	
		inputState->guessing++;
	
	bool tailLa1La2 = (_tokenSet_23.member(LA(1))) && (_tokenSet_24.member(LA(2)));
	bool derefLa1La2 = (_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2))); 
	
	if ( derefLa1La2) 
	{
	skip = false;
	try {
	deref_dot_expr_keeplast();
	}
	catch (antlr::RecognitionException& pe) {
	skip = true;
	}
	}
	else
	skip = true;
	
	if( skip && tailLa1La2)
	{
	rewind( markIn);
	inputState->guessing--;
	
	primary_expr_tail();
	if (inputState->guessing==0) {
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	primary_expr_deref_AST = RefDNode(currentAST.root);
	returnAST = primary_expr_deref_AST;
	return;
	}
	
	bool arrayexpr_mfcallParse = false;
	bool function_callParse = false;
	
	int mark2nd = mark();
	
	bool baseclass_methodParse = true;
	try {
	{
	baseclass_method();
	}
	}
	catch (antlr::RecognitionException& pe) {
	baseclass_methodParse = false;
	}
	
	rewind( mark2nd);
	
	if( !baseclass_methodParse)
	{
	int mark3rd = mark();
	
	arrayexpr_mfcallParse = true;
	try {
	{
				match(IDENTIFIER);
				match(LBRACE);
				expr();
				{ // ( ... )*
				for (;;) {
					if ((LA(1) == COMMA)) {
						match(COMMA);
						expr();
					}
					else {
	break;
					}
	}
				} // ( ... )*
				match(RBRACE);
	}
	}
	catch (antlr::RecognitionException& pe) {
	arrayexpr_mfcallParse = false;
	}
	
	rewind( mark3rd);
	
	if( !arrayexpr_mfcallParse)
	{
	function_callParse = true;
	try {
	{
	formal_function_call();
	}
	}
	catch (antlr::RecognitionException& pe) {
	function_callParse = false;
	}
	
	if( !function_callParse && tailLa1La2)
	{
	rewind( markIn);
	inputState->guessing--;
	
	primary_expr_tail();
	if (inputState->guessing==0) {
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	primary_expr_deref_AST = RefDNode(currentAST.root);
	returnAST = primary_expr_deref_AST;
	return; 
	}
	}
	}
	
	rewind( markIn);
	inputState->guessing--;
	
	
	if (((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2))))&&(baseclass_methodParse)) {
		deref_dot_expr_keeplast();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		baseclass_method();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		formal_function_call();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			primary_expr_deref_AST = RefDNode(currentAST.root);
			
			primary_expr_deref_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MFCALL_PARENT,"mfcall::")))->add(antlr::RefAST(primary_expr_deref_AST))));
			
			currentAST.root = primary_expr_deref_AST;
			if ( primary_expr_deref_AST!=RefDNode(antlr::nullAST) &&
				primary_expr_deref_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = primary_expr_deref_AST->getFirstChild();
			else
				currentAST.child = primary_expr_deref_AST;
			currentAST.advanceChildToEnd();
		}
		primary_expr_deref_AST = RefDNode(currentAST.root);
	}
	else if (((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2))))&&(arrayexpr_mfcallParse)) {
		arrayexpr_mfcall();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		primary_expr_deref_AST = RefDNode(currentAST.root);
	}
	else if (((_tokenSet_4.member(LA(1))) && (_tokenSet_5.member(LA(2))))&&(function_callParse)) {
		deref_dot_expr_keeplast();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		formal_function_call();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			primary_expr_deref_AST = RefDNode(currentAST.root);
			primary_expr_deref_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MFCALL,"mfcall")))->add(antlr::RefAST(primary_expr_deref_AST))));
			currentAST.root = primary_expr_deref_AST;
			if ( primary_expr_deref_AST!=RefDNode(antlr::nullAST) &&
				primary_expr_deref_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = primary_expr_deref_AST->getFirstChild();
			else
				currentAST.child = primary_expr_deref_AST;
			currentAST.advanceChildToEnd();
		}
		primary_expr_deref_AST = RefDNode(currentAST.root);
	}
	else if ((_tokenSet_23.member(LA(1))) && (_tokenSet_24.member(LA(2)))) {
		primary_expr_tail();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		primary_expr_deref_AST = RefDNode(currentAST.root);
	}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	
	returnAST = primary_expr_deref_AST;
}

void GDLParser::primary_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode primary_expr_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  sl = antlr::nullToken;
	RefDNode sl_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case STRING_LITERAL:
	{
		sl = LT(1);
		if ( inputState->guessing == 0 ) {
			sl_AST = astFactory->create(sl);
		}
		match(STRING_LITERAL);
		if ( inputState->guessing==0 ) {
			primary_expr_AST = RefDNode(currentAST.root);
			primary_expr_AST=astFactory->create(CONSTANT,sl->getText());
			primary_expr_AST->Text2String();	
			primary_expr_AST->SetLine( sl_AST->getLine());
					
			currentAST.root = primary_expr_AST;
			if ( primary_expr_AST!=RefDNode(antlr::nullAST) &&
				primary_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = primary_expr_AST->getFirstChild();
			else
				currentAST.child = primary_expr_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case CONSTANT_HEX_BYTE:
	case CONSTANT_HEX_LONG:
	case CONSTANT_HEX_LONG64:
	case CONSTANT_HEX_INT:
	case CONSTANT_HEX_I:
	case CONSTANT_HEX_ULONG:
	case CONSTANT_HEX_ULONG64:
	case CONSTANT_HEX_UI:
	case CONSTANT_HEX_UINT:
	case CONSTANT_BYTE:
	case CONSTANT_LONG:
	case CONSTANT_LONG64:
	case CONSTANT_INT:
	case CONSTANT_I:
	case CONSTANT_ULONG:
	case CONSTANT_ULONG64:
	case CONSTANT_UI:
	case CONSTANT_UINT:
	case CONSTANT_OCT_BYTE:
	case CONSTANT_OCT_LONG:
	case CONSTANT_OCT_LONG64:
	case CONSTANT_OCT_INT:
	case CONSTANT_OCT_I:
	case CONSTANT_OCT_ULONG:
	case CONSTANT_OCT_ULONG64:
	case CONSTANT_OCT_UI:
	case CONSTANT_OCT_UINT:
	case CONSTANT_FLOAT:
	case CONSTANT_DOUBLE:
	case CONSTANT_BIN_BYTE:
	case CONSTANT_BIN_LONG:
	case CONSTANT_BIN_LONG64:
	case CONSTANT_BIN_INT:
	case CONSTANT_BIN_I:
	case CONSTANT_BIN_ULONG:
	case CONSTANT_BIN_ULONG64:
	case CONSTANT_BIN_UI:
	case CONSTANT_BIN_UINT:
	{
		numeric_constant();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case IDENTIFIER:
	case INHERITS:
	case LBRACE:
	case LSQUARE:
	case SYSVARNAME:
	case LCURLY:
	case ASTERIX:
	{
		primary_expr_deref();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = primary_expr_AST;
}

void GDLParser::decinc_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode decinc_expr_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  i = antlr::nullToken;
	RefDNode i_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  d = antlr::nullToken;
	RefDNode d_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case IDENTIFIER:
	case INHERITS:
	case LBRACE:
	case LSQUARE:
	case SYSVARNAME:
	case LCURLY:
	case CONSTANT_HEX_BYTE:
	case CONSTANT_HEX_LONG:
	case CONSTANT_HEX_LONG64:
	case CONSTANT_HEX_INT:
	case CONSTANT_HEX_I:
	case CONSTANT_HEX_ULONG:
	case CONSTANT_HEX_ULONG64:
	case CONSTANT_HEX_UI:
	case CONSTANT_HEX_UINT:
	case CONSTANT_BYTE:
	case CONSTANT_LONG:
	case CONSTANT_LONG64:
	case CONSTANT_INT:
	case CONSTANT_I:
	case CONSTANT_ULONG:
	case CONSTANT_ULONG64:
	case CONSTANT_UI:
	case CONSTANT_UINT:
	case CONSTANT_OCT_BYTE:
	case CONSTANT_OCT_LONG:
	case CONSTANT_OCT_LONG64:
	case CONSTANT_OCT_INT:
	case CONSTANT_OCT_I:
	case CONSTANT_OCT_ULONG:
	case CONSTANT_OCT_ULONG64:
	case CONSTANT_OCT_UI:
	case CONSTANT_OCT_UINT:
	case CONSTANT_FLOAT:
	case CONSTANT_DOUBLE:
	case CONSTANT_BIN_BYTE:
	case CONSTANT_BIN_LONG:
	case CONSTANT_BIN_LONG64:
	case CONSTANT_BIN_INT:
	case CONSTANT_BIN_I:
	case CONSTANT_BIN_ULONG:
	case CONSTANT_BIN_ULONG64:
	case CONSTANT_BIN_UI:
	case CONSTANT_BIN_UINT:
	case ASTERIX:
	case STRING_LITERAL:
	{
		primary_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{
		switch ( LA(1)) {
		case INC:
		{
			i = LT(1);
			if ( inputState->guessing == 0 ) {
				i_AST = astFactory->create(i);
				astFactory->makeASTRoot(currentAST, antlr::RefAST(i_AST));
			}
			match(INC);
			if ( inputState->guessing==0 ) {
				i_AST->setType( POSTINC); i_AST->setText( "_++");
			}
			break;
		}
		case DEC:
		{
			d = LT(1);
			if ( inputState->guessing == 0 ) {
				d_AST = astFactory->create(d);
				astFactory->makeASTRoot(currentAST, antlr::RefAST(d_AST));
			}
			match(DEC);
			if ( inputState->guessing==0 ) {
				d_AST->setType( POSTDEC); d_AST->setText( "_--");
			}
			break;
		}
		case antlr::Token::EOF_TYPE:
		case AND_OP:
		case DO:
		case ELSE:
		case EQ_OP:
		case GE_OP:
		case GT_OP:
		case LE_OP:
		case LT_OP:
		case MOD_OP:
		case NE_OP:
		case OF:
		case OR_OP:
		case THEN:
		case UNTIL:
		case XOR_OP:
		case COMMA:
		case COLON:
		case END_U:
		case RBRACE:
		case SLASH:
		case RSQUARE:
		case RCURLY:
		case ASTERIX:
		case POW:
		case MATRIX_OP1:
		case MATRIX_OP2:
		case PLUS:
		case MINUS:
		case LTMARK:
		case GTMARK:
		case LOG_AND:
		case LOG_OR:
		case QUESTION:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(LT(1), getFilename());
		}
		}
		}
		decinc_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case INC:
	{
		RefDNode tmp193_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp193_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp193_AST));
		}
		match(INC);
		primary_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		decinc_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case DEC:
	{
		RefDNode tmp194_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp194_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp194_AST));
		}
		match(DEC);
		primary_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		decinc_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = decinc_expr_AST;
}

void GDLParser::exponential_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode exponential_expr_AST = RefDNode(antlr::nullAST);
	
	decinc_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == POW)) {
			RefDNode tmp195_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp195_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp195_AST));
			}
			match(POW);
			decinc_expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop231;
		}
		
	}
	_loop231:;
	} // ( ... )*
	exponential_expr_AST = RefDNode(currentAST.root);
	returnAST = exponential_expr_AST;
}

void GDLParser::multiplicative_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode multiplicative_expr_AST = RefDNode(antlr::nullAST);
	
	exponential_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_25.member(LA(1)))) {
			{
			switch ( LA(1)) {
			case ASTERIX:
			{
				RefDNode tmp196_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp196_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp196_AST));
				}
				match(ASTERIX);
				break;
			}
			case MATRIX_OP1:
			{
				RefDNode tmp197_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp197_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp197_AST));
				}
				match(MATRIX_OP1);
				break;
			}
			case MATRIX_OP2:
			{
				RefDNode tmp198_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp198_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp198_AST));
				}
				match(MATRIX_OP2);
				break;
			}
			case SLASH:
			{
				RefDNode tmp199_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp199_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp199_AST));
				}
				match(SLASH);
				break;
			}
			case MOD_OP:
			{
				RefDNode tmp200_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp200_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp200_AST));
				}
				match(MOD_OP);
				break;
			}
			default:
			{
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			}
			}
			exponential_expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop235;
		}
		
	}
	_loop235:;
	} // ( ... )*
	multiplicative_expr_AST = RefDNode(currentAST.root);
	returnAST = multiplicative_expr_AST;
}

void GDLParser::signed_multiplicative_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode signed_multiplicative_expr_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  m = antlr::nullToken;
	RefDNode m_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case PLUS:
	{
		match(PLUS);
		multiplicative_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		signed_multiplicative_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case MINUS:
	{
		m = LT(1);
		if ( inputState->guessing == 0 ) {
			m_AST = astFactory->create(m);
			astFactory->makeASTRoot(currentAST, antlr::RefAST(m_AST));
		}
		match(MINUS);
		multiplicative_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		if ( inputState->guessing==0 ) {
			
			m_AST->setType( UMINUS); 
			m_AST->setText( "u-"); 
			
		}
		signed_multiplicative_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case IDENTIFIER:
	case INHERITS:
	case DEC:
	case INC:
	case LBRACE:
	case LSQUARE:
	case SYSVARNAME:
	case LCURLY:
	case CONSTANT_HEX_BYTE:
	case CONSTANT_HEX_LONG:
	case CONSTANT_HEX_LONG64:
	case CONSTANT_HEX_INT:
	case CONSTANT_HEX_I:
	case CONSTANT_HEX_ULONG:
	case CONSTANT_HEX_ULONG64:
	case CONSTANT_HEX_UI:
	case CONSTANT_HEX_UINT:
	case CONSTANT_BYTE:
	case CONSTANT_LONG:
	case CONSTANT_LONG64:
	case CONSTANT_INT:
	case CONSTANT_I:
	case CONSTANT_ULONG:
	case CONSTANT_ULONG64:
	case CONSTANT_UI:
	case CONSTANT_UINT:
	case CONSTANT_OCT_BYTE:
	case CONSTANT_OCT_LONG:
	case CONSTANT_OCT_LONG64:
	case CONSTANT_OCT_INT:
	case CONSTANT_OCT_I:
	case CONSTANT_OCT_ULONG:
	case CONSTANT_OCT_ULONG64:
	case CONSTANT_OCT_UI:
	case CONSTANT_OCT_UINT:
	case CONSTANT_FLOAT:
	case CONSTANT_DOUBLE:
	case CONSTANT_BIN_BYTE:
	case CONSTANT_BIN_LONG:
	case CONSTANT_BIN_LONG64:
	case CONSTANT_BIN_INT:
	case CONSTANT_BIN_I:
	case CONSTANT_BIN_ULONG:
	case CONSTANT_BIN_ULONG64:
	case CONSTANT_BIN_UI:
	case CONSTANT_BIN_UINT:
	case ASTERIX:
	case STRING_LITERAL:
	{
		multiplicative_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		signed_multiplicative_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = signed_multiplicative_expr_AST;
}

void GDLParser::additive_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode additive_expr_AST = RefDNode(antlr::nullAST);
	
	{
	switch ( LA(1)) {
	case IDENTIFIER:
	case INHERITS:
	case DEC:
	case INC:
	case LBRACE:
	case LSQUARE:
	case SYSVARNAME:
	case LCURLY:
	case CONSTANT_HEX_BYTE:
	case CONSTANT_HEX_LONG:
	case CONSTANT_HEX_LONG64:
	case CONSTANT_HEX_INT:
	case CONSTANT_HEX_I:
	case CONSTANT_HEX_ULONG:
	case CONSTANT_HEX_ULONG64:
	case CONSTANT_HEX_UI:
	case CONSTANT_HEX_UINT:
	case CONSTANT_BYTE:
	case CONSTANT_LONG:
	case CONSTANT_LONG64:
	case CONSTANT_INT:
	case CONSTANT_I:
	case CONSTANT_ULONG:
	case CONSTANT_ULONG64:
	case CONSTANT_UI:
	case CONSTANT_UINT:
	case CONSTANT_OCT_BYTE:
	case CONSTANT_OCT_LONG:
	case CONSTANT_OCT_LONG64:
	case CONSTANT_OCT_INT:
	case CONSTANT_OCT_I:
	case CONSTANT_OCT_ULONG:
	case CONSTANT_OCT_ULONG64:
	case CONSTANT_OCT_UI:
	case CONSTANT_OCT_UINT:
	case CONSTANT_FLOAT:
	case CONSTANT_DOUBLE:
	case CONSTANT_BIN_BYTE:
	case CONSTANT_BIN_LONG:
	case CONSTANT_BIN_LONG64:
	case CONSTANT_BIN_INT:
	case CONSTANT_BIN_I:
	case CONSTANT_BIN_ULONG:
	case CONSTANT_BIN_ULONG64:
	case CONSTANT_BIN_UI:
	case CONSTANT_BIN_UINT:
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	{
		signed_multiplicative_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case NOT_OP:
	case LOG_NEG:
	{
		neg_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{ // ( ... )*
	for (;;) {
		if (((LA(1) >= PLUS && LA(1) <= GTMARK))) {
			{
			switch ( LA(1)) {
			case PLUS:
			{
				RefDNode tmp202_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp202_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp202_AST));
				}
				match(PLUS);
				break;
			}
			case MINUS:
			{
				RefDNode tmp203_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp203_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp203_AST));
				}
				match(MINUS);
				break;
			}
			case LTMARK:
			{
				RefDNode tmp204_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp204_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp204_AST));
				}
				match(LTMARK);
				break;
			}
			case GTMARK:
			{
				RefDNode tmp205_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp205_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp205_AST));
				}
				match(GTMARK);
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
			case IDENTIFIER:
			case INHERITS:
			case DEC:
			case INC:
			case LBRACE:
			case LSQUARE:
			case SYSVARNAME:
			case LCURLY:
			case CONSTANT_HEX_BYTE:
			case CONSTANT_HEX_LONG:
			case CONSTANT_HEX_LONG64:
			case CONSTANT_HEX_INT:
			case CONSTANT_HEX_I:
			case CONSTANT_HEX_ULONG:
			case CONSTANT_HEX_ULONG64:
			case CONSTANT_HEX_UI:
			case CONSTANT_HEX_UINT:
			case CONSTANT_BYTE:
			case CONSTANT_LONG:
			case CONSTANT_LONG64:
			case CONSTANT_INT:
			case CONSTANT_I:
			case CONSTANT_ULONG:
			case CONSTANT_ULONG64:
			case CONSTANT_UI:
			case CONSTANT_UINT:
			case CONSTANT_OCT_BYTE:
			case CONSTANT_OCT_LONG:
			case CONSTANT_OCT_LONG64:
			case CONSTANT_OCT_INT:
			case CONSTANT_OCT_I:
			case CONSTANT_OCT_ULONG:
			case CONSTANT_OCT_ULONG64:
			case CONSTANT_OCT_UI:
			case CONSTANT_OCT_UINT:
			case CONSTANT_FLOAT:
			case CONSTANT_DOUBLE:
			case CONSTANT_BIN_BYTE:
			case CONSTANT_BIN_LONG:
			case CONSTANT_BIN_LONG64:
			case CONSTANT_BIN_INT:
			case CONSTANT_BIN_I:
			case CONSTANT_BIN_ULONG:
			case CONSTANT_BIN_ULONG64:
			case CONSTANT_BIN_UI:
			case CONSTANT_BIN_UINT:
			case ASTERIX:
			case STRING_LITERAL:
			{
				multiplicative_expr();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				break;
			}
			case NOT_OP:
			case LOG_NEG:
			{
				neg_expr();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				break;
			}
			default:
			{
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
		else {
			goto _loop242;
		}
		
	}
	_loop242:;
	} // ( ... )*
	additive_expr_AST = RefDNode(currentAST.root);
	returnAST = additive_expr_AST;
}

void GDLParser::neg_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode neg_expr_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case NOT_OP:
	{
		RefDNode tmp206_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp206_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp206_AST));
		}
		match(NOT_OP);
		multiplicative_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		neg_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case LOG_NEG:
	{
		RefDNode tmp207_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp207_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp207_AST));
		}
		match(LOG_NEG);
		multiplicative_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		neg_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = neg_expr_AST;
}

void GDLParser::relational_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode relational_expr_AST = RefDNode(antlr::nullAST);
	
	additive_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_26.member(LA(1)))) {
			{
			switch ( LA(1)) {
			case EQ_OP:
			{
				RefDNode tmp208_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp208_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp208_AST));
				}
				match(EQ_OP);
				break;
			}
			case NE_OP:
			{
				RefDNode tmp209_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp209_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp209_AST));
				}
				match(NE_OP);
				break;
			}
			case LE_OP:
			{
				RefDNode tmp210_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp210_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp210_AST));
				}
				match(LE_OP);
				break;
			}
			case LT_OP:
			{
				RefDNode tmp211_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp211_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp211_AST));
				}
				match(LT_OP);
				break;
			}
			case GE_OP:
			{
				RefDNode tmp212_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp212_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp212_AST));
				}
				match(GE_OP);
				break;
			}
			case GT_OP:
			{
				RefDNode tmp213_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp213_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp213_AST));
				}
				match(GT_OP);
				break;
			}
			default:
			{
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			}
			}
			additive_expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop247;
		}
		
	}
	_loop247:;
	} // ( ... )*
	relational_expr_AST = RefDNode(currentAST.root);
	returnAST = relational_expr_AST;
}

void GDLParser::boolean_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode boolean_expr_AST = RefDNode(antlr::nullAST);
	
	relational_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == AND_OP || LA(1) == OR_OP || LA(1) == XOR_OP)) {
			{
			switch ( LA(1)) {
			case AND_OP:
			{
				RefDNode tmp214_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp214_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp214_AST));
				}
				match(AND_OP);
				break;
			}
			case OR_OP:
			{
				RefDNode tmp215_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp215_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp215_AST));
				}
				match(OR_OP);
				break;
			}
			case XOR_OP:
			{
				RefDNode tmp216_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp216_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp216_AST));
				}
				match(XOR_OP);
				break;
			}
			default:
			{
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			}
			}
			relational_expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop251;
		}
		
	}
	_loop251:;
	} // ( ... )*
	boolean_expr_AST = RefDNode(currentAST.root);
	returnAST = boolean_expr_AST;
}

void GDLParser::logical_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode logical_expr_AST = RefDNode(antlr::nullAST);
	
	boolean_expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == LOG_AND || LA(1) == LOG_OR)) {
			{
			switch ( LA(1)) {
			case LOG_AND:
			{
				RefDNode tmp217_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp217_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp217_AST));
				}
				match(LOG_AND);
				break;
			}
			case LOG_OR:
			{
				RefDNode tmp218_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp218_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp218_AST));
				}
				match(LOG_OR);
				break;
			}
			default:
			{
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			}
			}
			boolean_expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop255;
		}
		
	}
	_loop255:;
	} // ( ... )*
	logical_expr_AST = RefDNode(currentAST.root);
	returnAST = logical_expr_AST;
}

void GDLParser::initializeASTFactory( antlr::ASTFactory& factory )
{
	factory.setMaxNodeType(237);
}
const char* GDLParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"ALL",
	"ASSIGN",
	"ASSIGN_INPLACE",
	"ASSIGN_REPLACE",
	"ASSIGN_ARRAYEXPR_MFCALL",
	"ARRAYDEF",
	"ARRAYDEF_CONST",
	"ARRAYDEF_GENERALIZED_INDGEN",
	"ARRAYIX",
	"ARRAYIX_ALL",
	"ARRAYIX_ORANGE",
	"ARRAYIX_RANGE",
	"ARRAYIX_ORANGE_S",
	"ARRAYIX_RANGE_S",
	"ARRAYEXPR",
	"ARRAYEXPR_FCALL",
	"ARRAYEXPR_MFCALL",
	"BLOCK",
	"BREAK",
	"CSBLOCK",
	"CONTINUE",
	"COMMONDECL",
	"COMMONDEF",
	"CONSTANT",
	"DEREF",
	"ELSEBLK",
	"EXPR",
	"\"for\"",
	"FOR_STEP",
	"\"foreach\"",
	"FOREACH_INDEX",
	"FOR_LOOP",
	"FOR_STEP_LOOP",
	"FOREACH_LOOP",
	"FOREACH_INDEX_LOOP",
	"FCALL",
	"FCALL_LIB",
	"FCALL_LIB_DIRECT",
	"FCALL_LIB_N_ELEMENTS",
	"FCALL_LIB_RETNEW",
	"GDLNULL",
	"IF_ELSE",
	"KEYDECL",
	"KEYDEF",
	"KEYDEF_REF",
	"KEYDEF_REF_CHECK",
	"KEYDEF_REF_EXPR",
	"LABEL",
	"MPCALL",
	"MPCALL_PARENT",
	"MFCALL",
	"MFCALL_LIB",
	"MFCALL_LIB_RETNEW",
	"MFCALL_PARENT",
	"MFCALL_PARENT_LIB",
	"MFCALL_PARENT_LIB_RETNEW",
	"NOP",
	"NSTRUC",
	"NSTRUC_REF",
	"ON_IOERROR_NULL",
	"PCALL",
	"PCALL_LIB",
	"PARADECL",
	"PARAEXPR",
	"PARAEXPR_VN",
	"DEC_REF_CHECK",
	"INC_REF_CHECK",
	"POSTDEC",
	"POSTINC",
	"DECSTATEMENT",
	"INCSTATEMENT",
	"REF",
	"REF_VN",
	"REF_CHECK",
	"REF_CHECK_VN",
	"REF_EXPR",
	"REF_EXPR_VN",
	"\"repeat\"",
	"REPEAT_LOOP",
	"RETURN",
	"RETF",
	"RETP",
	"STRUC",
	"SYSVAR",
	"UMINUS",
	"VAR",
	"VARPTR",
	"\"while\"",
	"IDENTIFIER",
	"\"and\"",
	"\"begin\"",
	"\"case\"",
	"\"common\"",
	"\"compile_opt\"",
	"\"do\"",
	"\"else\"",
	"\"end\"",
	"\"endcase\"",
	"\"endelse\"",
	"\"endfor\"",
	"\"endforeach\"",
	"\"endif\"",
	"\"endrep\"",
	"\"endswitch\"",
	"\"endwhile\"",
	"\"eq\"",
	"\"forward_function\"",
	"\"function\"",
	"\"ge\"",
	"\"goto\"",
	"\"gt\"",
	"\"if\"",
	"\"inherits\"",
	"\"le\"",
	"\"lt\"",
	"\"mod\"",
	"\"ne\"",
	"\"not\"",
	"\"of\"",
	"\"on_ioerror\"",
	"\"or\"",
	"\"pro\"",
	"\"switch\"",
	"\"then\"",
	"\"until\"",
	"\"xor\"",
	"METHOD",
	"COMMA",
	"COLON",
	"END_U",
	"EQUAL",
	"DEC",
	"INC",
	"AND_OP_EQ",
	"ASTERIX_EQ",
	"EQ_OP_EQ",
	"GE_OP_EQ",
	"GTMARK_EQ",
	"GT_OP_EQ",
	"LE_OP_EQ",
	"LTMARK_EQ",
	"LT_OP_EQ",
	"MATRIX_OP1_EQ",
	"MATRIX_OP2_EQ",
	"MINUS_EQ",
	"MOD_OP_EQ",
	"NE_OP_EQ",
	"OR_OP_EQ",
	"PLUS_EQ",
	"POW_EQ",
	"SLASH_EQ",
	"XOR_OP_EQ",
	"MEMBER",
	"LBRACE",
	"RBRACE",
	"SLASH",
	"LSQUARE",
	"RSQUARE",
	"SYSVARNAME",
	"EXCLAMATION",
	"LCURLY",
	"RCURLY",
	"CONSTANT_HEX_BYTE",
	"CONSTANT_HEX_LONG",
	"CONSTANT_HEX_LONG64",
	"CONSTANT_HEX_INT",
	"CONSTANT_HEX_I",
	"CONSTANT_HEX_ULONG",
	"CONSTANT_HEX_ULONG64",
	"CONSTANT_HEX_UI",
	"CONSTANT_HEX_UINT",
	"CONSTANT_BYTE",
	"CONSTANT_LONG",
	"CONSTANT_LONG64",
	"CONSTANT_INT",
	"CONSTANT_I",
	"CONSTANT_ULONG",
	"CONSTANT_ULONG64",
	"CONSTANT_UI",
	"CONSTANT_UINT",
	"CONSTANT_OCT_BYTE",
	"CONSTANT_OCT_LONG",
	"CONSTANT_OCT_LONG64",
	"CONSTANT_OCT_INT",
	"CONSTANT_OCT_I",
	"CONSTANT_OCT_ULONG",
	"CONSTANT_OCT_ULONG64",
	"CONSTANT_OCT_UI",
	"CONSTANT_OCT_UINT",
	"CONSTANT_FLOAT",
	"CONSTANT_DOUBLE",
	"CONSTANT_BIN_BYTE",
	"CONSTANT_BIN_LONG",
	"CONSTANT_BIN_LONG64",
	"CONSTANT_BIN_INT",
	"CONSTANT_BIN_I",
	"CONSTANT_BIN_ULONG",
	"CONSTANT_BIN_ULONG64",
	"CONSTANT_BIN_UI",
	"CONSTANT_BIN_UINT",
	"ASTERIX",
	"DOT",
	"STRING_LITERAL",
	"POW",
	"MATRIX_OP1",
	"MATRIX_OP2",
	"PLUS",
	"MINUS",
	"LTMARK",
	"GTMARK",
	"LOG_NEG",
	"LOG_AND",
	"LOG_OR",
	"QUESTION",
	"STRING",
	"INCLUDE",
	"EOL",
	"W",
	"D",
	"L",
	"H",
	"O",
	"B",
	"EXP",
	"DBL_E",
	"DBL",
	"CONSTANT_OR_STRING_LITERAL",
	"COMMENT",
	"END_MARKER",
	"WHITESPACE",
	"SKIP_LINES",
	"CONT_STATEMENT",
	"END_OF_LINE",
	"MAX_TOKEN_NUMBER",
	0
};

const unsigned long GDLParser::_tokenSet_0_data_[] = { 2147483650UL, 2UL, 3624009728UL, 1746583555UL, 536871328UL, 4UL, 4096UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF "for" "foreach" "repeat" "while" IDENTIFIER "begin" "case" "common" 
// "compile_opt" "forward_function" "function" "goto" "if" "inherits" "on_ioerror" 
// "pro" "switch" END_U DEC INC LBRACE SYSVARNAME ASTERIX 
const antlr::BitSet GDLParser::_tokenSet_0(_tokenSet_0_data_,16);
const unsigned long GDLParser::_tokenSet_1_data_[] = { 2147483650UL, 2UL, 3624009728UL, 1780146171UL, 536871328UL, 4294967253UL, 5005311UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF "for" "foreach" "repeat" "while" IDENTIFIER "begin" "case" "common" 
// "compile_opt" "else" "end" "endcase" "endelse" "endfor" "endforeach" 
// "endif" "endrep" "endswitch" "endwhile" "forward_function" "function" 
// "goto" "if" "inherits" "not" "on_ioerror" "pro" "switch" END_U DEC INC 
// LBRACE LSQUARE SYSVARNAME LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG 
// CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG 
// CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE 
// CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I CONSTANT_ULONG 
// CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG 
// CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG 
// CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT 
// CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG CONSTANT_BIN_LONG64 
// CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG CONSTANT_BIN_ULONG64 
// CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX STRING_LITERAL PLUS MINUS 
// LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_1(_tokenSet_1_data_,16);
const unsigned long GDLParser::_tokenSet_2_data_[] = { 2147483648UL, 2UL, 3624009728UL, 1209679875UL, 536871296UL, 4UL, 4096UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "for" "foreach" "repeat" "while" IDENTIFIER "begin" "case" "common" 
// "compile_opt" "forward_function" "goto" "if" "inherits" "on_ioerror" 
// "switch" DEC INC LBRACE SYSVARNAME ASTERIX 
const antlr::BitSet GDLParser::_tokenSet_2(_tokenSet_2_data_,16);
const unsigned long GDLParser::_tokenSet_3_data_[] = { 2147483648UL, 2UL, 3624009728UL, 1243234307UL, 1073741800UL, 4294967253UL, 5013503UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "for" "foreach" "repeat" "while" IDENTIFIER "begin" "case" "common" 
// "compile_opt" "forward_function" "goto" "if" "inherits" "not" "on_ioerror" 
// "switch" COMMA END_U EQUAL DEC INC AND_OP_EQ ASTERIX_EQ EQ_OP_EQ GE_OP_EQ 
// GTMARK_EQ GT_OP_EQ LE_OP_EQ LTMARK_EQ LT_OP_EQ MATRIX_OP1_EQ MATRIX_OP2_EQ 
// MINUS_EQ MOD_OP_EQ NE_OP_EQ OR_OP_EQ PLUS_EQ POW_EQ SLASH_EQ XOR_OP_EQ 
// MEMBER LBRACE LSQUARE SYSVARNAME LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG 
// CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG 
// CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE 
// CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I CONSTANT_ULONG 
// CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG 
// CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG 
// CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT 
// CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG CONSTANT_BIN_LONG64 
// CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG CONSTANT_BIN_ULONG64 
// CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX DOT STRING_LITERAL PLUS MINUS 
// LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_3(_tokenSet_3_data_,16);
const unsigned long GDLParser::_tokenSet_4_data_[] = { 0UL, 0UL, 268435456UL, 1048576UL, 536870912UL, 4UL, 4096UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "inherits" LBRACE SYSVARNAME ASTERIX 
const antlr::BitSet GDLParser::_tokenSet_4(_tokenSet_4_data_,16);
const unsigned long GDLParser::_tokenSet_5_data_[] = { 0UL, 0UL, 268435456UL, 34603008UL, 536871296UL, 4294967253UL, 5013503UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "inherits" "not" DEC INC LBRACE LSQUARE SYSVARNAME LCURLY 
// CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT 
// CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI 
// CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT 
// CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT 
// CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT 
// CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI 
// CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG 
// CONSTANT_BIN_LONG64 CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG 
// CONSTANT_BIN_ULONG64 CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX DOT STRING_LITERAL 
// PLUS MINUS LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_5(_tokenSet_5_data_,16);
const unsigned long GDLParser::_tokenSet_6_data_[] = { 0UL, 0UL, 268435456UL, 34603008UL, 1073741760UL, 4294967253UL, 5013503UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "inherits" "not" EQUAL DEC INC AND_OP_EQ ASTERIX_EQ EQ_OP_EQ 
// GE_OP_EQ GTMARK_EQ GT_OP_EQ LE_OP_EQ LTMARK_EQ LT_OP_EQ MATRIX_OP1_EQ 
// MATRIX_OP2_EQ MINUS_EQ MOD_OP_EQ NE_OP_EQ OR_OP_EQ PLUS_EQ POW_EQ SLASH_EQ 
// XOR_OP_EQ MEMBER LBRACE LSQUARE SYSVARNAME LCURLY CONSTANT_HEX_BYTE 
// CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I 
// CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT 
// CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I 
// CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE 
// CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I 
// CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT 
// CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG CONSTANT_BIN_LONG64 
// CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG CONSTANT_BIN_ULONG64 
// CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX DOT STRING_LITERAL PLUS MINUS 
// LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_6(_tokenSet_6_data_,16);
const unsigned long GDLParser::_tokenSet_7_data_[] = { 0UL, 0UL, 0UL, 8UL, 41UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "else" "until" COMMA END_U 
const antlr::BitSet GDLParser::_tokenSet_7(_tokenSet_7_data_,12);
const unsigned long GDLParser::_tokenSet_8_data_[] = { 0UL, 0UL, 268435456UL, 34603016UL, 536871296UL, 4294967253UL, 5005311UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "else" "inherits" "not" DEC INC LBRACE LSQUARE SYSVARNAME 
// LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT 
// CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI 
// CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT 
// CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT 
// CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT 
// CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI 
// CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG 
// CONSTANT_BIN_LONG64 CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG 
// CONSTANT_BIN_ULONG64 CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX STRING_LITERAL 
// PLUS MINUS LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_8(_tokenSet_8_data_,16);
const unsigned long GDLParser::_tokenSet_9_data_[] = { 2147483650UL, 2UL, 4160880640UL, 4294967295UL, 2684354979UL, 4294967253UL, 5005311UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF "for" "foreach" "repeat" "while" IDENTIFIER "and" "begin" "case" 
// "common" "compile_opt" "do" "else" "end" "endcase" "endelse" "endfor" 
// "endforeach" "endif" "endrep" "endswitch" "endwhile" "eq" "forward_function" 
// "function" "ge" "goto" "gt" "if" "inherits" "le" "lt" "mod" "ne" "not" 
// "of" "on_ioerror" "or" "pro" "switch" "then" "until" "xor" END_U DEC 
// INC LBRACE SLASH LSQUARE SYSVARNAME LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG 
// CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG 
// CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE 
// CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I CONSTANT_ULONG 
// CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG 
// CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG 
// CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT 
// CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG CONSTANT_BIN_LONG64 
// CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG CONSTANT_BIN_ULONG64 
// CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX STRING_LITERAL PLUS MINUS 
// LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_9(_tokenSet_9_data_,16);
const unsigned long GDLParser::_tokenSet_10_data_[] = { 2147483648UL, 2UL, 4160880640UL, 4294967295UL, 3UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "for" "foreach" "repeat" "while" IDENTIFIER "and" "begin" "case" "common" 
// "compile_opt" "do" "else" "end" "endcase" "endelse" "endfor" "endforeach" 
// "endif" "endrep" "endswitch" "endwhile" "eq" "forward_function" "function" 
// "ge" "goto" "gt" "if" "inherits" "le" "lt" "mod" "ne" "not" "of" "on_ioerror" 
// "or" "pro" "switch" "then" "until" "xor" 
const antlr::BitSet GDLParser::_tokenSet_10(_tokenSet_10_data_,12);
const unsigned long GDLParser::_tokenSet_11_data_[] = { 0UL, 0UL, 268435456UL, 34603008UL, 536871296UL, 4294967253UL, 5005311UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "inherits" "not" DEC INC LBRACE LSQUARE SYSVARNAME LCURLY 
// CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT 
// CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI 
// CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT 
// CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT 
// CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT 
// CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI 
// CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG 
// CONSTANT_BIN_LONG64 CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG 
// CONSTANT_BIN_ULONG64 CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX STRING_LITERAL 
// PLUS MINUS LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_11(_tokenSet_11_data_,16);
const unsigned long GDLParser::_tokenSet_12_data_[] = { 0UL, 0UL, 805306368UL, 334831624UL, 4026532267UL, 4294967295UL, 67108863UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "and" "else" "eq" "ge" "gt" "inherits" "le" "lt" "mod" "ne" 
// "not" "or" "until" "xor" COMMA END_U DEC INC MEMBER LBRACE RBRACE SLASH 
// LSQUARE RSQUARE SYSVARNAME EXCLAMATION LCURLY RCURLY CONSTANT_HEX_BYTE 
// CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I 
// CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT 
// CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I 
// CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE 
// CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I 
// CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT 
// CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG CONSTANT_BIN_LONG64 
// CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG CONSTANT_BIN_ULONG64 
// CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX DOT STRING_LITERAL POW MATRIX_OP1 
// MATRIX_OP2 PLUS MINUS LTMARK GTMARK LOG_NEG LOG_AND LOG_OR QUESTION 
const antlr::BitSet GDLParser::_tokenSet_12(_tokenSet_12_data_,16);
const unsigned long GDLParser::_tokenSet_13_data_[] = { 0UL, 0UL, 268435456UL, 1048576UL, 0UL, 12UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "inherits" SYSVARNAME EXCLAMATION 
const antlr::BitSet GDLParser::_tokenSet_13(_tokenSet_13_data_,12);
const unsigned long GDLParser::_tokenSet_14_data_[] = { 2UL, 0UL, 805306368UL, 334831616UL, 2952790410UL, 4294967295UL, 67108863UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF IDENTIFIER "and" "eq" "ge" "gt" "inherits" "le" "lt" "mod" "ne" 
// "not" "or" "xor" COMMA DEC INC MEMBER LBRACE SLASH LSQUARE RSQUARE SYSVARNAME 
// EXCLAMATION LCURLY RCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 
// CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 
// CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 
// CONSTANT_INT CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI 
// CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 
// CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 
// CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE 
// CONSTANT_BIN_LONG CONSTANT_BIN_LONG64 CONSTANT_BIN_INT CONSTANT_BIN_I 
// CONSTANT_BIN_ULONG CONSTANT_BIN_ULONG64 CONSTANT_BIN_UI CONSTANT_BIN_UINT 
// ASTERIX DOT STRING_LITERAL POW MATRIX_OP1 MATRIX_OP2 PLUS MINUS LTMARK 
// GTMARK LOG_NEG LOG_AND LOG_OR QUESTION 
const antlr::BitSet GDLParser::_tokenSet_14(_tokenSet_14_data_,16);
const unsigned long GDLParser::_tokenSet_15_data_[] = { 0UL, 0UL, 268435456UL, 34603008UL, 536871296UL, 4294967261UL, 5005311UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "inherits" "not" DEC INC LBRACE LSQUARE SYSVARNAME EXCLAMATION 
// LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT 
// CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI 
// CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT 
// CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT 
// CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT 
// CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI 
// CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG 
// CONSTANT_BIN_LONG64 CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG 
// CONSTANT_BIN_ULONG64 CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX STRING_LITERAL 
// PLUS MINUS LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_15(_tokenSet_15_data_,16);
const unsigned long GDLParser::_tokenSet_16_data_[] = { 0UL, 0UL, 805306368UL, 334831616UL, 2952790426UL, 4294967295UL, 67108863UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "and" "eq" "ge" "gt" "inherits" "le" "lt" "mod" "ne" "not" 
// "or" "xor" COMMA COLON DEC INC MEMBER LBRACE SLASH LSQUARE RSQUARE SYSVARNAME 
// EXCLAMATION LCURLY RCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 
// CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 
// CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 
// CONSTANT_INT CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI 
// CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 
// CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 
// CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE 
// CONSTANT_BIN_LONG CONSTANT_BIN_LONG64 CONSTANT_BIN_INT CONSTANT_BIN_I 
// CONSTANT_BIN_ULONG CONSTANT_BIN_ULONG64 CONSTANT_BIN_UI CONSTANT_BIN_UINT 
// ASTERIX DOT STRING_LITERAL POW MATRIX_OP1 MATRIX_OP2 PLUS MINUS LTMARK 
// GTMARK LOG_NEG LOG_AND LOG_OR QUESTION 
const antlr::BitSet GDLParser::_tokenSet_16(_tokenSet_16_data_,16);
const unsigned long GDLParser::_tokenSet_17_data_[] = { 0UL, 0UL, 805306368UL, 334831616UL, 4026532250UL, 4294967295UL, 67108863UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "and" "eq" "ge" "gt" "inherits" "le" "lt" "mod" "ne" "not" 
// "or" "xor" COMMA COLON DEC INC MEMBER LBRACE RBRACE SLASH LSQUARE RSQUARE 
// SYSVARNAME EXCLAMATION LCURLY RCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG 
// CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG 
// CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE 
// CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I CONSTANT_ULONG 
// CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG 
// CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG 
// CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT 
// CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG CONSTANT_BIN_LONG64 
// CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG CONSTANT_BIN_ULONG64 
// CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX DOT STRING_LITERAL POW MATRIX_OP1 
// MATRIX_OP2 PLUS MINUS LTMARK GTMARK LOG_NEG LOG_AND LOG_OR QUESTION 
const antlr::BitSet GDLParser::_tokenSet_17(_tokenSet_17_data_,16);
const unsigned long GDLParser::_tokenSet_18_data_[] = { 0UL, 0UL, 0UL, 0UL, 1073741848UL, 2UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// COMMA COLON RBRACE RSQUARE 
const antlr::BitSet GDLParser::_tokenSet_18(_tokenSet_18_data_,12);
const unsigned long GDLParser::_tokenSet_19_data_[] = { 0UL, 0UL, 805306368UL, 334831616UL, 4026532234UL, 4294967295UL, 67108863UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "and" "eq" "ge" "gt" "inherits" "le" "lt" "mod" "ne" "not" 
// "or" "xor" COMMA DEC INC MEMBER LBRACE RBRACE SLASH LSQUARE RSQUARE 
// SYSVARNAME EXCLAMATION LCURLY RCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG 
// CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG 
// CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE 
// CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I CONSTANT_ULONG 
// CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG 
// CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG 
// CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT 
// CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG CONSTANT_BIN_LONG64 
// CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG CONSTANT_BIN_ULONG64 
// CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX DOT STRING_LITERAL POW MATRIX_OP1 
// MATRIX_OP2 PLUS MINUS LTMARK GTMARK LOG_NEG LOG_AND LOG_OR QUESTION 
const antlr::BitSet GDLParser::_tokenSet_19(_tokenSet_19_data_,16);
const unsigned long GDLParser::_tokenSet_20_data_[] = { 0UL, 0UL, 268435456UL, 0UL, 536870912UL, 12UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER LBRACE SYSVARNAME EXCLAMATION 
const antlr::BitSet GDLParser::_tokenSet_20(_tokenSet_20_data_,12);
const unsigned long GDLParser::_tokenSet_21_data_[] = { 0UL, 0UL, 0UL, 8UL, 536870957UL, 1UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "else" "until" METHOD COMMA END_U LBRACE LSQUARE 
const antlr::BitSet GDLParser::_tokenSet_21(_tokenSet_21_data_,12);
const unsigned long GDLParser::_tokenSet_22_data_[] = { 2UL, 0UL, 805306368UL, 2549424140UL, 4026532283UL, 4294967287UL, 67108863UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF IDENTIFIER "and" "do" "else" "eq" "ge" "gt" "inherits" "le" "lt" 
// "mod" "ne" "not" "of" "or" "then" "until" "xor" COMMA COLON END_U DEC 
// INC MEMBER LBRACE RBRACE SLASH LSQUARE RSQUARE SYSVARNAME LCURLY RCURLY 
// CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT 
// CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI 
// CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT 
// CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT 
// CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT 
// CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI 
// CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE CONSTANT_BIN_LONG 
// CONSTANT_BIN_LONG64 CONSTANT_BIN_INT CONSTANT_BIN_I CONSTANT_BIN_ULONG 
// CONSTANT_BIN_ULONG64 CONSTANT_BIN_UI CONSTANT_BIN_UINT ASTERIX DOT STRING_LITERAL 
// POW MATRIX_OP1 MATRIX_OP2 PLUS MINUS LTMARK GTMARK LOG_NEG LOG_AND LOG_OR 
// QUESTION 
const antlr::BitSet GDLParser::_tokenSet_22(_tokenSet_22_data_,16);
const unsigned long GDLParser::_tokenSet_23_data_[] = { 0UL, 0UL, 268435456UL, 1048576UL, 536870912UL, 21UL, 4096UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "inherits" LBRACE LSQUARE SYSVARNAME LCURLY ASTERIX 
const antlr::BitSet GDLParser::_tokenSet_23(_tokenSet_23_data_,16);
const unsigned long GDLParser::_tokenSet_24_data_[] = { 2UL, 0UL, 805306368UL, 2549424140UL, 4026532283UL, 4294967295UL, 67108863UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF IDENTIFIER "and" "do" "else" "eq" "ge" "gt" "inherits" "le" "lt" 
// "mod" "ne" "not" "of" "or" "then" "until" "xor" COMMA COLON END_U DEC 
// INC MEMBER LBRACE RBRACE SLASH LSQUARE RSQUARE SYSVARNAME EXCLAMATION 
// LCURLY RCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 
// CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 
// CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 
// CONSTANT_INT CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI 
// CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 
// CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 
// CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE CONSTANT_BIN_BYTE 
// CONSTANT_BIN_LONG CONSTANT_BIN_LONG64 CONSTANT_BIN_INT CONSTANT_BIN_I 
// CONSTANT_BIN_ULONG CONSTANT_BIN_ULONG64 CONSTANT_BIN_UI CONSTANT_BIN_UINT 
// ASTERIX DOT STRING_LITERAL POW MATRIX_OP1 MATRIX_OP2 PLUS MINUS LTMARK 
// GTMARK LOG_NEG LOG_AND LOG_OR QUESTION 
const antlr::BitSet GDLParser::_tokenSet_24(_tokenSet_24_data_,16);
const unsigned long GDLParser::_tokenSet_25_data_[] = { 0UL, 0UL, 0UL, 8388608UL, 2147483648UL, 0UL, 200704UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// "mod" SLASH ASTERIX MATRIX_OP1 MATRIX_OP2 
const antlr::BitSet GDLParser::_tokenSet_25(_tokenSet_25_data_,16);
const unsigned long GDLParser::_tokenSet_26_data_[] = { 0UL, 0UL, 0UL, 23404544UL, 0UL, 0UL, 0UL, 0UL };
// "eq" "ge" "gt" "le" "lt" "ne" 
const antlr::BitSet GDLParser::_tokenSet_26(_tokenSet_26_data_,8);


