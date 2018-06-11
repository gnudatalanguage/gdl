/* $ANTLR 2.7.7 (2006-11-01): "gdlc.tree.g" -> "GDLTreeParser.cpp"$ */

#include "includefirst.hpp"

#include "GDLTreeParser.hpp"
#include <antlr/Token.hpp>
#include <antlr/AST.hpp>
#include <antlr/NoViableAltException.hpp>
#include <antlr/MismatchedTokenException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/BitSet.hpp>

#include <memory>

#include <cstdlib>

// ****
#include "print_tree.hpp"

using namespace std;

// print out AST tree
//#define GDL_DEBUG
//#undef GDL_DEBUG


GDLTreeParser::GDLTreeParser()
	: antlr::TreeParser() {
}

void GDLTreeParser::translation_unit(RefDNode _t) {
	RefDNode translation_unit_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode translation_unit_AST = RefDNode(antlr::nullAST);
	RefDNode retAST_AST = RefDNode(antlr::nullAST);
	RefDNode retAST = RefDNode(antlr::nullAST);
	
	//    returnProgNodeP = _t;
	bool mainStarted = false;
	
	
	try {      // for error handling
		{ // ( ... )*
		for (;;) {
			if (_t == RefDNode(antlr::nullAST) )
				_t = ASTNULL;
			switch ( _t->getType()) {
			case PRO:
			{
				procedure_def(_t);
				_t = _retTree;
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				break;
			}
			case FUNCTION:
			{
				function_def(_t);
				_t = _retTree;
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				break;
			}
			default:
				if (_t == RefDNode(antlr::nullAST) )
					_t = ASTNULL;
				if ((_t->getType() == FORWARD)) {
					forward_function(_t);
					_t = _retTree;
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				else if ((_t->getType() == COMMONDECL || _t->getType() == COMMONDEF)) {
					
					EnvBaseT* envBefore = comp.GetEnv();
					if( !mainStarted)
					{
					comp.StartPro( "$MAIN$");                        
					mainStarted = true;
					}
					comp.ContinueMainPro();
					
					common_block(_t);
					_t = _retTree;
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					
					comp.EndInteractiveStatement();
					comp.SetEnv( envBefore);
					
				}
			else {
				goto _loop3;
			}
			}
		}
		_loop3:;
		} // ( ... )*
		{
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case BLOCK:
		case BREAK:
		case CONTINUE:
		case COMMONDECL:
		case COMMONDEF:
		case FOR:
		case FOREACH:
		case MPCALL:
		case MPCALL_PARENT:
		case PCALL:
		case REPEAT:
		case RETURN:
		case WHILE:
		case IDENTIFIER:
		case CASE:
		case FORWARD:
		case GOTO:
		case IF:
		case ON_IOERROR:
		case SWITCH:
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
		{
			
			if( !mainStarted)
			{
			comp.StartPro( "$MAIN$");                        
			mainStarted = true;
			}
			comp.ContinueMainPro();
			
			retAST = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
			statement_list(_t);
			_t = _retTree;
			retAST_AST = returnAST;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			
			comp.SetTree( retAST_AST);
			comp.EndPro();
			
			#ifdef GDL_DEBUG
			cout << "TreeParser output:" << endl;
			antlr::print_tree pt;
			pt.pr_tree(static_cast<antlr::RefAST>(retAST));
			cout << "CompileFile: TreeParser end." << endl;
			#endif
			
			
			break;
		}
		case 3:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		translation_unit_AST = RefDNode(currentAST.root);
	}
	catch ( GDLException& e) {
		
		throw;
		
	}
	catch ( antlr::NoViableAltException& e) {
		
		// SYNTAX ERROR
		throw GDLException( e.getLine(), e.getColumn(), "Compiler syntax error: "+e.getMessage());
		
	}
	catch ( antlr::RecognitionException& e) {
		
		// SYNTAX ERROR
		throw GDLException( e.getLine(), e.getColumn(), "General syntax error: "+e.getMessage());
		
	}
	returnAST = translation_unit_AST;
	_retTree = _t;
}

void GDLTreeParser::procedure_def(RefDNode _t) {
	RefDNode procedure_def_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode procedure_def_AST = RefDNode(antlr::nullAST);
	RefDNode p = RefDNode(antlr::nullAST);
	RefDNode p_AST = RefDNode(antlr::nullAST);
	RefDNode name = RefDNode(antlr::nullAST);
	RefDNode name_AST = RefDNode(antlr::nullAST);
	RefDNode obj = RefDNode(antlr::nullAST);
	RefDNode obj_AST = RefDNode(antlr::nullAST);
	
	RefDNode __t19 = _t;
	p = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode p_AST_in = RefDNode(antlr::nullAST);
	p_AST = astFactory->create(antlr::RefAST(p));
	antlr::ASTPair __currentAST19 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),PRO);
	_t = _t->getFirstChild();
	name = _t;
	RefDNode name_AST_in = RefDNode(antlr::nullAST);
	name_AST = astFactory->create(antlr::RefAST(name));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case METHOD:
	{
		RefDNode tmp1_AST = RefDNode(antlr::nullAST);
		RefDNode tmp1_AST_in = RefDNode(antlr::nullAST);
		tmp1_AST = astFactory->create(antlr::RefAST(_t));
		tmp1_AST_in = _t;
		match(antlr::RefAST(_t),METHOD);
		_t = _t->getNextSibling();
		obj = _t;
		RefDNode obj_AST_in = RefDNode(antlr::nullAST);
		obj_AST = astFactory->create(antlr::RefAST(obj));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		comp.StartPro(name->getText(),p_AST->GetCompileOpt(),obj->getText());
		
		break;
	}
	case 3:
	case ASSIGN:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case COMMONDECL:
	case COMMONDEF:
	case FOR:
	case FOREACH:
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	case PARADECL:
	case REPEAT:
	case RETURN:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case FORWARD:
	case GOTO:
	case IF:
	case ON_IOERROR:
	case SWITCH:
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
	{
		
		comp.StartPro(name->getText(),p_AST->GetCompileOpt());
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case PARADECL:
	{
		parameter_declaration(_t);
		_t = _retTree;
		break;
	}
	case 3:
	case ASSIGN:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case COMMONDECL:
	case COMMONDEF:
	case FOR:
	case FOREACH:
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	case REPEAT:
	case RETURN:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case FORWARD:
	case GOTO:
	case IF:
	case ON_IOERROR:
	case SWITCH:
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
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case COMMONDECL:
	case COMMONDEF:
	case FOR:
	case FOREACH:
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	case REPEAT:
	case RETURN:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case FORWARD:
	case GOTO:
	case IF:
	case ON_IOERROR:
	case SWITCH:
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
	{
		statement_list(_t);
		_t = _retTree;
		
		comp.SetTree( returnAST);
		
		break;
	}
	case 3:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	
	if( NCompileErrors() > 0)
	throw GDLException( i2s(NCompileErrors()) + " compilation error(s) in module " + name->getText() + ".");
	
	comp.EndPro();
	
	currentAST = __currentAST19;
	_t = __t19;
	_t = _t->getNextSibling();
	returnAST = procedure_def_AST;
	_retTree = _t;
}

void GDLTreeParser::function_def(RefDNode _t) {
	RefDNode function_def_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode function_def_AST = RefDNode(antlr::nullAST);
	RefDNode f = RefDNode(antlr::nullAST);
	RefDNode f_AST = RefDNode(antlr::nullAST);
	RefDNode name = RefDNode(antlr::nullAST);
	RefDNode name_AST = RefDNode(antlr::nullAST);
	RefDNode obj = RefDNode(antlr::nullAST);
	RefDNode obj_AST = RefDNode(antlr::nullAST);
	
	RefDNode __t24 = _t;
	f = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode f_AST_in = RefDNode(antlr::nullAST);
	f_AST = astFactory->create(antlr::RefAST(f));
	antlr::ASTPair __currentAST24 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),FUNCTION);
	_t = _t->getFirstChild();
	name = _t;
	RefDNode name_AST_in = RefDNode(antlr::nullAST);
	name_AST = astFactory->create(antlr::RefAST(name));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case METHOD:
	{
		RefDNode tmp2_AST = RefDNode(antlr::nullAST);
		RefDNode tmp2_AST_in = RefDNode(antlr::nullAST);
		tmp2_AST = astFactory->create(antlr::RefAST(_t));
		tmp2_AST_in = _t;
		match(antlr::RefAST(_t),METHOD);
		_t = _t->getNextSibling();
		obj = _t;
		RefDNode obj_AST_in = RefDNode(antlr::nullAST);
		obj_AST = astFactory->create(antlr::RefAST(obj));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		comp.StartFun(name->getText(),f_AST->GetCompileOpt(),obj->getText());
		
		break;
	}
	case 3:
	case ASSIGN:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case COMMONDECL:
	case COMMONDEF:
	case FOR:
	case FOREACH:
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	case PARADECL:
	case REPEAT:
	case RETURN:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case FORWARD:
	case GOTO:
	case IF:
	case ON_IOERROR:
	case SWITCH:
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
	{
		
		comp.StartFun(name->getText(),f_AST->GetCompileOpt());
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case PARADECL:
	{
		parameter_declaration(_t);
		_t = _retTree;
		break;
	}
	case 3:
	case ASSIGN:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case COMMONDECL:
	case COMMONDEF:
	case FOR:
	case FOREACH:
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	case REPEAT:
	case RETURN:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case FORWARD:
	case GOTO:
	case IF:
	case ON_IOERROR:
	case SWITCH:
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
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case COMMONDECL:
	case COMMONDEF:
	case FOR:
	case FOREACH:
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	case REPEAT:
	case RETURN:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case FORWARD:
	case GOTO:
	case IF:
	case ON_IOERROR:
	case SWITCH:
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
	{
		statement_list(_t);
		_t = _retTree;
		
		comp.SetTree( returnAST);
		
		break;
	}
	case 3:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	
	if( NCompileErrors() > 0)
	throw GDLException( i2s(NCompileErrors()) + " compilation error(s) in module " + name->getText() + ".");
	
	comp.EndFun();
	
	currentAST = __currentAST24;
	_t = __t24;
	_t = _t->getNextSibling();
	returnAST = function_def_AST;
	_retTree = _t;
}

void GDLTreeParser::forward_function(RefDNode _t) {
	RefDNode forward_function_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode forward_function_AST = RefDNode(antlr::nullAST);
	RefDNode id = RefDNode(antlr::nullAST);
	RefDNode id_AST = RefDNode(antlr::nullAST);
	
	RefDNode __t9 = _t;
	RefDNode tmp3_AST = RefDNode(antlr::nullAST);
	RefDNode tmp3_AST_in = RefDNode(antlr::nullAST);
	tmp3_AST = astFactory->create(antlr::RefAST(_t));
	tmp3_AST_in = _t;
	antlr::ASTPair __currentAST9 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),FORWARD);
	_t = _t->getFirstChild();
	{ // ( ... )+
	int _cnt11=0;
	for (;;) {
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == IDENTIFIER)) {
			id = _t;
			RefDNode id_AST_in = RefDNode(antlr::nullAST);
			id_AST = astFactory->create(antlr::RefAST(id));
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			
					  comp.ForwardFunction(id->getText());
					
		}
		else {
			if ( _cnt11>=1 ) { goto _loop11; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt11++;
	}
	_loop11:;
	}  // ( ... )+
	currentAST = __currentAST9;
	_t = __t9;
	_t = _t->getNextSibling();
	returnAST = forward_function_AST;
	_retTree = _t;
}

void GDLTreeParser::common_block(RefDNode _t) {
	RefDNode common_block_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode common_block_AST = RefDNode(antlr::nullAST);
	RefDNode id = RefDNode(antlr::nullAST);
	RefDNode id_AST = RefDNode(antlr::nullAST);
	RefDNode cv = RefDNode(antlr::nullAST);
	RefDNode cv_AST = RefDNode(antlr::nullAST);
	RefDNode id2 = RefDNode(antlr::nullAST);
	RefDNode id2_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case COMMONDEF:
	{
		RefDNode __t29 = _t;
		RefDNode tmp4_AST = RefDNode(antlr::nullAST);
		RefDNode tmp4_AST_in = RefDNode(antlr::nullAST);
		tmp4_AST = astFactory->create(antlr::RefAST(_t));
		tmp4_AST_in = _t;
		antlr::ASTPair __currentAST29 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),COMMONDEF);
		_t = _t->getFirstChild();
		id = _t;
		RefDNode id_AST_in = RefDNode(antlr::nullAST);
		id_AST = astFactory->create(antlr::RefAST(id));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
				DCommonBase* actCommon=comp.CommonDef(id->getText());
			
		{ // ( ... )+
		int _cnt31=0;
		for (;;) {
			if (_t == RefDNode(antlr::nullAST) )
				_t = ASTNULL;
			if ((_t->getType() == IDENTIFIER)) {
				cv = _t;
				RefDNode cv_AST_in = RefDNode(antlr::nullAST);
				cv_AST = astFactory->create(antlr::RefAST(cv));
				match(antlr::RefAST(_t),IDENTIFIER);
				_t = _t->getNextSibling();
				
						  comp.CommonVar(actCommon,cv->getText());
						
			}
			else {
				if ( _cnt31>=1 ) { goto _loop31; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
			}
			
			_cnt31++;
		}
		_loop31:;
		}  // ( ... )+
		currentAST = __currentAST29;
		_t = __t29;
		_t = _t->getNextSibling();
		break;
	}
	case COMMONDECL:
	{
		RefDNode __t32 = _t;
		RefDNode tmp5_AST = RefDNode(antlr::nullAST);
		RefDNode tmp5_AST_in = RefDNode(antlr::nullAST);
		tmp5_AST = astFactory->create(antlr::RefAST(_t));
		tmp5_AST_in = _t;
		antlr::ASTPair __currentAST32 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),COMMONDECL);
		_t = _t->getFirstChild();
		id2 = _t;
		RefDNode id2_AST_in = RefDNode(antlr::nullAST);
		id2_AST = astFactory->create(antlr::RefAST(id2));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
				comp.CommonDecl(id2->getText());
			
		currentAST = __currentAST32;
		_t = __t32;
		_t = _t->getNextSibling();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = common_block_AST;
	_retTree = _t;
}

void GDLTreeParser::statement_list(RefDNode _t) {
	RefDNode statement_list_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode statement_list_AST = RefDNode(antlr::nullAST);
	
	{ // ( ... )+
	int _cnt57=0;
	for (;;) {
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case BLOCK:
		case BREAK:
		case CONTINUE:
		case COMMONDECL:
		case COMMONDEF:
		case FOR:
		case FOREACH:
		case MPCALL:
		case MPCALL_PARENT:
		case PCALL:
		case REPEAT:
		case RETURN:
		case WHILE:
		case CASE:
		case FORWARD:
		case GOTO:
		case IF:
		case ON_IOERROR:
		case SWITCH:
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
		{
			statement(_t);
			_t = _retTree;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			break;
		}
		case IDENTIFIER:
		{
			label(_t);
			_t = _retTree;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			break;
		}
		default:
		{
			if ( _cnt57>=1 ) { goto _loop57; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		}
		_cnt57++;
	}
	_loop57:;
	}  // ( ... )+
	statement_list_AST = RefDNode(currentAST.root);
	returnAST = statement_list_AST;
	_retTree = _t;
}

void GDLTreeParser::interactive(RefDNode _t) {
	RefDNode interactive_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode interactive_AST = RefDNode(antlr::nullAST);
	
	try {      // for error handling
		{ // ( ... )+
		int _cnt7=0;
		for (;;) {
			if (_t == RefDNode(antlr::nullAST) )
				_t = ASTNULL;
			if ((_tokenSet_0.member(_t->getType()))) {
				statement(_t);
				_t = _retTree;
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				comp.EndInteractiveStatement();
			}
			else {
				if ( _cnt7>=1 ) { goto _loop7; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
			}
			
			_cnt7++;
		}
		_loop7:;
		}  // ( ... )+
		interactive_AST = RefDNode(currentAST.root);
	}
	catch ( GDLException& e) {
		
		throw;
		
	}
	catch ( antlr::NoViableAltException& e) {
		
		// SYNTAX ERROR
		throw GDLException( e.getLine(), e.getColumn(), "Compiler syntax error: "+e.getMessage());
		
	}
	catch ( antlr::RecognitionException& e) {
		
		// SYNTAX ERROR
		throw GDLException( e.getLine(), e.getColumn(), "General syntax error: "+e.getMessage());
		
	}
	returnAST = interactive_AST;
	_retTree = _t;
}

void GDLTreeParser::statement(RefDNode _t) {
	RefDNode statement_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode statement_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	{
		assign_expr_statement(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
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
		comp_assign_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	{
		procedure_call(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case FOR:
	{
		for_statement(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case FOREACH:
	{
		foreach_statement(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case REPEAT:
	{
		repeat_statement(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case WHILE:
	{
		while_statement(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case RETURN:
	case GOTO:
	case ON_IOERROR:
	{
		jump_statement(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case IF:
	{
		if_statement(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case CASE:
	{
		case_statement(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case SWITCH:
	{
		switch_statement(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case FORWARD:
	{
		forward_function(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case COMMONDECL:
	case COMMONDEF:
	{
		common_block(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case BLOCK:
	{
		block(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case DEC:
	{
		RefDNode __t59 = _t;
		RefDNode tmp6_AST = RefDNode(antlr::nullAST);
		RefDNode tmp6_AST_in = RefDNode(antlr::nullAST);
		tmp6_AST = astFactory->create(antlr::RefAST(_t));
		tmp6_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp6_AST));
		antlr::ASTPair __currentAST59 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),DEC);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST59;
		_t = __t59;
		_t = _t->getNextSibling();
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case INC:
	{
		RefDNode __t60 = _t;
		RefDNode tmp7_AST = RefDNode(antlr::nullAST);
		RefDNode tmp7_AST_in = RefDNode(antlr::nullAST);
		tmp7_AST = astFactory->create(antlr::RefAST(_t));
		tmp7_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp7_AST));
		antlr::ASTPair __currentAST60 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),INC);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST60;
		_t = __t60;
		_t = _t->getNextSibling();
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case BREAK:
	{
		RefDNode tmp8_AST = RefDNode(antlr::nullAST);
		RefDNode tmp8_AST_in = RefDNode(antlr::nullAST);
		tmp8_AST = astFactory->create(antlr::RefAST(_t));
		tmp8_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp8_AST));
		match(antlr::RefAST(_t),BREAK);
		_t = _t->getNextSibling();
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case CONTINUE:
	{
		RefDNode tmp9_AST = RefDNode(antlr::nullAST);
		RefDNode tmp9_AST_in = RefDNode(antlr::nullAST);
		tmp9_AST = astFactory->create(antlr::RefAST(_t));
		tmp9_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp9_AST));
		match(antlr::RefAST(_t),CONTINUE);
		_t = _t->getNextSibling();
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = statement_AST;
	_retTree = _t;
}

void GDLTreeParser::parameter_declaration(RefDNode _t) {
	RefDNode parameter_declaration_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode parameter_declaration_AST = RefDNode(antlr::nullAST);
	RefDNode id = RefDNode(antlr::nullAST);
	RefDNode id_AST = RefDNode(antlr::nullAST);
	
	RefDNode __t13 = _t;
	RefDNode tmp10_AST = RefDNode(antlr::nullAST);
	RefDNode tmp10_AST_in = RefDNode(antlr::nullAST);
	tmp10_AST = astFactory->create(antlr::RefAST(_t));
	tmp10_AST_in = _t;
	antlr::ASTPair __currentAST13 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),PARADECL);
	_t = _t->getFirstChild();
	{ // ( ... )+
	int _cnt15=0;
	for (;;) {
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case IDENTIFIER:
		{
			id = _t;
			RefDNode id_AST_in = RefDNode(antlr::nullAST);
			id_AST = astFactory->create(antlr::RefAST(id));
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			
					  comp.AddPar(id->getText());
					
			break;
		}
		case KEYDECL:
		{
			keyword_declaration(_t);
			_t = _retTree;
			break;
		}
		default:
		{
			if ( _cnt15>=1 ) { goto _loop15; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		}
		_cnt15++;
	}
	_loop15:;
	}  // ( ... )+
	currentAST = __currentAST13;
	_t = __t13;
	_t = _t->getNextSibling();
	returnAST = parameter_declaration_AST;
	_retTree = _t;
}

void GDLTreeParser::keyword_declaration(RefDNode _t) {
	RefDNode keyword_declaration_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode keyword_declaration_AST = RefDNode(antlr::nullAST);
	RefDNode key = RefDNode(antlr::nullAST);
	RefDNode key_AST = RefDNode(antlr::nullAST);
	RefDNode val = RefDNode(antlr::nullAST);
	RefDNode val_AST = RefDNode(antlr::nullAST);
	
	RefDNode __t17 = _t;
	RefDNode tmp11_AST = RefDNode(antlr::nullAST);
	RefDNode tmp11_AST_in = RefDNode(antlr::nullAST);
	tmp11_AST = astFactory->create(antlr::RefAST(_t));
	tmp11_AST_in = _t;
	antlr::ASTPair __currentAST17 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),KEYDECL);
	_t = _t->getFirstChild();
	key = _t;
	RefDNode key_AST_in = RefDNode(antlr::nullAST);
	key_AST = astFactory->create(antlr::RefAST(key));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	val = _t;
	RefDNode val_AST_in = RefDNode(antlr::nullAST);
	val_AST = astFactory->create(antlr::RefAST(val));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	currentAST = __currentAST17;
	_t = __t17;
	_t = _t->getNextSibling();
	
		  comp.AddKey(key->getText(),val->getText());
		
	returnAST = keyword_declaration_AST;
	_retTree = _t;
}

void GDLTreeParser::caseswitch_body(RefDNode _t) {
	RefDNode caseswitch_body_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode caseswitch_body_AST = RefDNode(antlr::nullAST);
	RefDNode b = RefDNode(antlr::nullAST);
	RefDNode b_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case BLOCK:
	{
		RefDNode __t34 = _t;
		b = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode b_AST_in = RefDNode(antlr::nullAST);
		b_AST = astFactory->create(antlr::RefAST(b));
		astFactory->addASTChild(currentAST, antlr::RefAST(b_AST));
		antlr::ASTPair __currentAST34 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),BLOCK);
		_t = _t->getFirstChild();
		b_AST->setType(CSBLOCK);b_AST->setText("csblock");
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		{
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case BLOCK:
		case BREAK:
		case CONTINUE:
		case COMMONDECL:
		case COMMONDEF:
		case FOR:
		case FOREACH:
		case MPCALL:
		case MPCALL_PARENT:
		case PCALL:
		case REPEAT:
		case RETURN:
		case WHILE:
		case IDENTIFIER:
		case CASE:
		case FORWARD:
		case GOTO:
		case IF:
		case ON_IOERROR:
		case SWITCH:
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
		{
			statement_list(_t);
			_t = _retTree;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			break;
		}
		case 3:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		currentAST = __currentAST34;
		_t = __t34;
		_t = _t->getNextSibling();
		caseswitch_body_AST = RefDNode(currentAST.root);
		break;
	}
	case ELSEBLK:
	{
		RefDNode __t36 = _t;
		RefDNode tmp12_AST = RefDNode(antlr::nullAST);
		RefDNode tmp12_AST_in = RefDNode(antlr::nullAST);
		tmp12_AST = astFactory->create(antlr::RefAST(_t));
		tmp12_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp12_AST));
		antlr::ASTPair __currentAST36 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),ELSEBLK);
		_t = _t->getFirstChild();
		{
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case BLOCK:
		case BREAK:
		case CONTINUE:
		case COMMONDECL:
		case COMMONDEF:
		case FOR:
		case FOREACH:
		case MPCALL:
		case MPCALL_PARENT:
		case PCALL:
		case REPEAT:
		case RETURN:
		case WHILE:
		case IDENTIFIER:
		case CASE:
		case FORWARD:
		case GOTO:
		case IF:
		case ON_IOERROR:
		case SWITCH:
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
		{
			statement_list(_t);
			_t = _retTree;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			break;
		}
		case 3:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		currentAST = __currentAST36;
		_t = __t36;
		_t = _t->getNextSibling();
		caseswitch_body_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = caseswitch_body_AST;
	_retTree = _t;
}

void GDLTreeParser::expr(RefDNode _t) {
	RefDNode expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode expr_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	case EXPR:
	case SYSVAR:
	case VAR:
	{
		array_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr_AST = RefDNode(currentAST.root);
		break;
	}
	case DOT:
	{
		RefDNode __t210 = _t;
		RefDNode tmp13_AST = RefDNode(antlr::nullAST);
		RefDNode tmp13_AST_in = RefDNode(antlr::nullAST);
		tmp13_AST = astFactory->create(antlr::RefAST(_t));
		tmp13_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp13_AST));
		antlr::ASTPair __currentAST210 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),DOT);
		_t = _t->getFirstChild();
		tag_array_expr_1st(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		{ // ( ... )+
		int _cnt212=0;
		for (;;) {
			if (_t == RefDNode(antlr::nullAST) )
				_t = ASTNULL;
			if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR || _t->getType() == IDENTIFIER)) {
				tag_array_expr(_t);
				_t = _retTree;
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			else {
				if ( _cnt212>=1 ) { goto _loop212; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
			}
			
			_cnt212++;
		}
		_loop212:;
		}  // ( ... )+
		currentAST = __currentAST210;
		_t = __t210;
		_t = _t->getNextSibling();
		expr_AST = RefDNode(currentAST.root);
		break;
	}
	case DEREF:
	{
		RefDNode __t213 = _t;
		RefDNode tmp14_AST = RefDNode(antlr::nullAST);
		RefDNode tmp14_AST_in = RefDNode(antlr::nullAST);
		tmp14_AST = astFactory->create(antlr::RefAST(_t));
		tmp14_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp14_AST));
		antlr::ASTPair __currentAST213 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),DEREF);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST213;
		_t = __t213;
		_t = _t->getNextSibling();
		expr_AST = RefDNode(currentAST.root);
		break;
	}
	case ASSIGN:
	case ARRAYDEF:
	case ARRAYDEF_CONST:
	case ARRAYDEF_GENERALIZED_INDGEN:
	case ARRAYEXPR_FCALL:
	case ARRAYEXPR_MFCALL:
	case CONSTANT:
	case FCALL:
	case GDLNULL:
	case MFCALL:
	case MFCALL_PARENT:
	case NSTRUC_REF:
	case POSTDEC:
	case POSTINC:
	case STRUC:
	case UMINUS:
	case AND_OP:
	case EQ_OP:
	case GE_OP:
	case GT_OP:
	case LE_OP:
	case LT_OP:
	case MOD_OP:
	case NE_OP:
	case NOT_OP:
	case OR_OP:
	case XOR_OP:
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
	case SLASH:
	case ASTERIX:
	case POW:
	case MATRIX_OP1:
	case MATRIX_OP2:
	case PLUS:
	case MINUS:
	case LTMARK:
	case GTMARK:
	case LOG_NEG:
	case LOG_AND:
	case LOG_OR:
	case QUESTION:
	{
		op_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = expr_AST;
	_retTree = _t;
}

void GDLTreeParser::switch_statement(RefDNode _t) {
	RefDNode switch_statement_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode switch_statement_AST = RefDNode(antlr::nullAST);
	RefDNode s = RefDNode(antlr::nullAST);
	RefDNode s_AST = RefDNode(antlr::nullAST);
	
	int labelStart = comp.NDefLabel();
	
	
	RefDNode __t39 = _t;
	s = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode s_AST_in = RefDNode(antlr::nullAST);
	s_AST = astFactory->create(antlr::RefAST(s));
	astFactory->addASTChild(currentAST, antlr::RefAST(s_AST));
	antlr::ASTPair __currentAST39 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),SWITCH);
	_t = _t->getFirstChild();
	expr(_t);
	_t = _retTree;
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	{ // ( ... )*
	for (;;) {
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == BLOCK || _t->getType() == ELSEBLK)) {
			caseswitch_body(_t);
			_t = _retTree;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		else {
			goto _loop41;
		}
		
	}
	_loop41:;
	} // ( ... )*
	currentAST = __currentAST39;
	_t = __t39;
	_t = _t->getNextSibling();
	
	s_AST->SetLabelRange( labelStart, comp.NDefLabel());
	
	switch_statement_AST = RefDNode(currentAST.root);
	returnAST = switch_statement_AST;
	_retTree = _t;
}

void GDLTreeParser::case_statement(RefDNode _t) {
	RefDNode case_statement_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode case_statement_AST = RefDNode(antlr::nullAST);
	RefDNode c = RefDNode(antlr::nullAST);
	RefDNode c_AST = RefDNode(antlr::nullAST);
	
	int labelStart = comp.NDefLabel();
	
	
	RefDNode __t43 = _t;
	c = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode c_AST_in = RefDNode(antlr::nullAST);
	c_AST = astFactory->create(antlr::RefAST(c));
	astFactory->addASTChild(currentAST, antlr::RefAST(c_AST));
	antlr::ASTPair __currentAST43 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),CASE);
	_t = _t->getFirstChild();
	expr(_t);
	_t = _retTree;
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	{ // ( ... )*
	for (;;) {
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == BLOCK || _t->getType() == ELSEBLK)) {
			caseswitch_body(_t);
			_t = _retTree;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		else {
			goto _loop45;
		}
		
	}
	_loop45:;
	} // ( ... )*
	currentAST = __currentAST43;
	_t = __t43;
	_t = _t->getNextSibling();
	
	c_AST->SetLabelRange( labelStart, comp.NDefLabel());
	
	case_statement_AST = RefDNode(currentAST.root);
	returnAST = case_statement_AST;
	_retTree = _t;
}

void GDLTreeParser::block(RefDNode _t) {
	RefDNode block_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode block_AST = RefDNode(antlr::nullAST);
	RefDNode b = RefDNode(antlr::nullAST);
	RefDNode b_AST = RefDNode(antlr::nullAST);
	
	//    int labelStart = comp.NDefLabel();
	
	
	RefDNode __t47 = _t;
	b = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode b_AST_in = RefDNode(antlr::nullAST);
	b_AST = astFactory->create(antlr::RefAST(b));
	astFactory->addASTChild(currentAST, antlr::RefAST(b_AST));
	antlr::ASTPair __currentAST47 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),BLOCK);
	_t = _t->getFirstChild();
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case COMMONDECL:
	case COMMONDEF:
	case FOR:
	case FOREACH:
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	case REPEAT:
	case RETURN:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case FORWARD:
	case GOTO:
	case IF:
	case ON_IOERROR:
	case SWITCH:
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
	{
		statement_list(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		break;
	}
	case 3:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	currentAST = __currentAST47;
	_t = __t47;
	_t = _t->getNextSibling();
	
	//            #b->SetLabelRange( labelStart, comp.NDefLabel());
	
	block_AST = RefDNode(currentAST.root);
	returnAST = block_AST;
	_retTree = _t;
}

void GDLTreeParser::unblock(RefDNode _t) {
	RefDNode unblock_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode unblock_AST = RefDNode(antlr::nullAST);
	RefDNode b = RefDNode(antlr::nullAST);
	RefDNode b_AST = RefDNode(antlr::nullAST);
	RefDNode s_AST = RefDNode(antlr::nullAST);
	RefDNode s = RefDNode(antlr::nullAST);
	
	RefDNode __t50 = _t;
	b = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode b_AST_in = RefDNode(antlr::nullAST);
	b_AST = astFactory->create(antlr::RefAST(b));
	antlr::ASTPair __currentAST50 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),BLOCK);
	_t = _t->getFirstChild();
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case COMMONDECL:
	case COMMONDEF:
	case FOR:
	case FOREACH:
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	case REPEAT:
	case RETURN:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case FORWARD:
	case GOTO:
	case IF:
	case ON_IOERROR:
	case SWITCH:
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
	{
		s = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		statement_list(_t);
		_t = _retTree;
		s_AST = returnAST;
		unblock_AST = RefDNode(currentAST.root);
		unblock_AST = s_AST;
		currentAST.root = unblock_AST;
		if ( unblock_AST!=RefDNode(antlr::nullAST) &&
			unblock_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = unblock_AST->getFirstChild();
		else
			currentAST.child = unblock_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case 3:
	{
		unblock_AST = RefDNode(currentAST.root);
		unblock_AST = b_AST;
		currentAST.root = unblock_AST;
		if ( unblock_AST!=RefDNode(antlr::nullAST) &&
			unblock_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = unblock_AST->getFirstChild();
		else
			currentAST.child = unblock_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	currentAST = __currentAST50;
	_t = __t50;
	_t = _t->getNextSibling();
	returnAST = unblock_AST;
	_retTree = _t;
}

void GDLTreeParser::unblock_empty(RefDNode _t) {
	RefDNode unblock_empty_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode unblock_empty_AST = RefDNode(antlr::nullAST);
	RefDNode b = RefDNode(antlr::nullAST);
	RefDNode b_AST = RefDNode(antlr::nullAST);
	RefDNode s_AST = RefDNode(antlr::nullAST);
	RefDNode s = RefDNode(antlr::nullAST);
	
	RefDNode __t53 = _t;
	b = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode b_AST_in = RefDNode(antlr::nullAST);
	b_AST = astFactory->create(antlr::RefAST(b));
	antlr::ASTPair __currentAST53 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),BLOCK);
	_t = _t->getFirstChild();
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case COMMONDECL:
	case COMMONDEF:
	case FOR:
	case FOREACH:
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	case REPEAT:
	case RETURN:
	case WHILE:
	case IDENTIFIER:
	case CASE:
	case FORWARD:
	case GOTO:
	case IF:
	case ON_IOERROR:
	case SWITCH:
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
	{
		s = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		statement_list(_t);
		_t = _retTree;
		s_AST = returnAST;
		unblock_empty_AST = RefDNode(currentAST.root);
		unblock_empty_AST = s_AST;
		currentAST.root = unblock_empty_AST;
		if ( unblock_empty_AST!=RefDNode(antlr::nullAST) &&
			unblock_empty_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = unblock_empty_AST->getFirstChild();
		else
			currentAST.child = unblock_empty_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case 3:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	currentAST = __currentAST53;
	_t = __t53;
	_t = _t->getNextSibling();
	returnAST = unblock_empty_AST;
	_retTree = _t;
}

void GDLTreeParser::label(RefDNode _t) {
	RefDNode label_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode label_AST = RefDNode(antlr::nullAST);
	RefDNode i = RefDNode(antlr::nullAST);
	RefDNode i_AST = RefDNode(antlr::nullAST);
	
	RefDNode __t72 = _t;
	i = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode i_AST_in = RefDNode(antlr::nullAST);
	i_AST = astFactory->create(antlr::RefAST(i));
	antlr::ASTPair __currentAST72 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getFirstChild();
	RefDNode tmp15_AST = RefDNode(antlr::nullAST);
	RefDNode tmp15_AST_in = RefDNode(antlr::nullAST);
	tmp15_AST = astFactory->create(antlr::RefAST(_t));
	tmp15_AST_in = _t;
	match(antlr::RefAST(_t),COLON);
	_t = _t->getNextSibling();
	currentAST = __currentAST72;
	_t = __t72;
	_t = _t->getNextSibling();
	label_AST = RefDNode(currentAST.root);
	
		  label_AST=astFactory->create(LABEL,i->getText());
	label_AST->SetLine( i->getLine());
		  comp.Label(label_AST); 
		
	currentAST.root = label_AST;
	if ( label_AST!=RefDNode(antlr::nullAST) &&
		label_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = label_AST->getFirstChild();
	else
		currentAST.child = label_AST;
	currentAST.advanceChildToEnd();
	returnAST = label_AST;
	_retTree = _t;
}

void GDLTreeParser::assign_expr_statement(RefDNode _t) {
	RefDNode assign_expr_statement_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode assign_expr_statement_AST = RefDNode(antlr::nullAST);
	RefDNode a = RefDNode(antlr::nullAST);
	RefDNode a_AST = RefDNode(antlr::nullAST);
	RefDNode l_AST = RefDNode(antlr::nullAST);
	RefDNode l = RefDNode(antlr::nullAST);
	RefDNode r_AST = RefDNode(antlr::nullAST);
	RefDNode r = RefDNode(antlr::nullAST);
	
	RefDNode __t125 = _t;
	a = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode a_AST_in = RefDNode(antlr::nullAST);
	a_AST = astFactory->create(antlr::RefAST(a));
	antlr::ASTPair __currentAST125 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),ASSIGN);
	_t = _t->getFirstChild();
	l = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	lassign_expr(_t);
	_t = _retTree;
	l_AST = returnAST;
	r = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	r_AST = returnAST;
	currentAST = __currentAST125;
	_t = __t125;
	_t = _t->getNextSibling();
	assign_expr_statement_AST = RefDNode(currentAST.root);
	
	if( !SelfAssignment( l_AST, r_AST))
	{
	
	AssignReplace( l_AST, a_AST);
	
	// int lT = #l->getType();
	// if( lT == FCALL || lT == MFCALL || lT == MFCALL_PARENT ||
	// lT == FCALL_LIB || lT == MFCALL_LIB || lT == MFCALL_PARENT_LIB ||
	// lT == DEREF || lT == VAR || lT == VARPTR)
	// #a->setType( ASSIGN_REPLACE);
	assign_expr_statement_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(a_AST))->add(antlr::RefAST(r_AST))->add(antlr::RefAST(l_AST))));  
	}
	else
	{
	assign_expr_statement_AST=RefDNode(astFactory->make((new antlr::ASTArray(1))->add(antlr::RefAST(astFactory->create(BLOCK,"block")))));                
	}
	
	currentAST.root = assign_expr_statement_AST;
	if ( assign_expr_statement_AST!=RefDNode(antlr::nullAST) &&
		assign_expr_statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = assign_expr_statement_AST->getFirstChild();
	else
		currentAST.child = assign_expr_statement_AST;
	currentAST.advanceChildToEnd();
	returnAST = assign_expr_statement_AST;
	_retTree = _t;
}

void GDLTreeParser::comp_assign_expr(RefDNode _t) {
	RefDNode comp_assign_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode comp_assign_expr_AST = RefDNode(antlr::nullAST);
	RefDNode a1 = RefDNode(antlr::nullAST);
	RefDNode a1_AST = RefDNode(antlr::nullAST);
	RefDNode l1_AST = RefDNode(antlr::nullAST);
	RefDNode l1 = RefDNode(antlr::nullAST);
	RefDNode r1_AST = RefDNode(antlr::nullAST);
	RefDNode r1 = RefDNode(antlr::nullAST);
	RefDNode a2 = RefDNode(antlr::nullAST);
	RefDNode a2_AST = RefDNode(antlr::nullAST);
	RefDNode l2_AST = RefDNode(antlr::nullAST);
	RefDNode l2 = RefDNode(antlr::nullAST);
	RefDNode r2_AST = RefDNode(antlr::nullAST);
	RefDNode r2 = RefDNode(antlr::nullAST);
	RefDNode a3 = RefDNode(antlr::nullAST);
	RefDNode a3_AST = RefDNode(antlr::nullAST);
	RefDNode l3_AST = RefDNode(antlr::nullAST);
	RefDNode l3 = RefDNode(antlr::nullAST);
	RefDNode r3_AST = RefDNode(antlr::nullAST);
	RefDNode r3 = RefDNode(antlr::nullAST);
	RefDNode a4 = RefDNode(antlr::nullAST);
	RefDNode a4_AST = RefDNode(antlr::nullAST);
	RefDNode l4_AST = RefDNode(antlr::nullAST);
	RefDNode l4 = RefDNode(antlr::nullAST);
	RefDNode r4_AST = RefDNode(antlr::nullAST);
	RefDNode r4 = RefDNode(antlr::nullAST);
	RefDNode a5 = RefDNode(antlr::nullAST);
	RefDNode a5_AST = RefDNode(antlr::nullAST);
	RefDNode l5_AST = RefDNode(antlr::nullAST);
	RefDNode l5 = RefDNode(antlr::nullAST);
	RefDNode r5_AST = RefDNode(antlr::nullAST);
	RefDNode r5 = RefDNode(antlr::nullAST);
	RefDNode a6 = RefDNode(antlr::nullAST);
	RefDNode a6_AST = RefDNode(antlr::nullAST);
	RefDNode l6_AST = RefDNode(antlr::nullAST);
	RefDNode l6 = RefDNode(antlr::nullAST);
	RefDNode r6_AST = RefDNode(antlr::nullAST);
	RefDNode r6 = RefDNode(antlr::nullAST);
	RefDNode a7 = RefDNode(antlr::nullAST);
	RefDNode a7_AST = RefDNode(antlr::nullAST);
	RefDNode l7_AST = RefDNode(antlr::nullAST);
	RefDNode l7 = RefDNode(antlr::nullAST);
	RefDNode r7_AST = RefDNode(antlr::nullAST);
	RefDNode r7 = RefDNode(antlr::nullAST);
	RefDNode a8 = RefDNode(antlr::nullAST);
	RefDNode a8_AST = RefDNode(antlr::nullAST);
	RefDNode l8_AST = RefDNode(antlr::nullAST);
	RefDNode l8 = RefDNode(antlr::nullAST);
	RefDNode r8_AST = RefDNode(antlr::nullAST);
	RefDNode r8 = RefDNode(antlr::nullAST);
	RefDNode a9 = RefDNode(antlr::nullAST);
	RefDNode a9_AST = RefDNode(antlr::nullAST);
	RefDNode l9_AST = RefDNode(antlr::nullAST);
	RefDNode l9 = RefDNode(antlr::nullAST);
	RefDNode r9_AST = RefDNode(antlr::nullAST);
	RefDNode r9 = RefDNode(antlr::nullAST);
	RefDNode a10 = RefDNode(antlr::nullAST);
	RefDNode a10_AST = RefDNode(antlr::nullAST);
	RefDNode l10_AST = RefDNode(antlr::nullAST);
	RefDNode l10 = RefDNode(antlr::nullAST);
	RefDNode r10_AST = RefDNode(antlr::nullAST);
	RefDNode r10 = RefDNode(antlr::nullAST);
	RefDNode a11 = RefDNode(antlr::nullAST);
	RefDNode a11_AST = RefDNode(antlr::nullAST);
	RefDNode l11_AST = RefDNode(antlr::nullAST);
	RefDNode l11 = RefDNode(antlr::nullAST);
	RefDNode r11_AST = RefDNode(antlr::nullAST);
	RefDNode r11 = RefDNode(antlr::nullAST);
	RefDNode a12 = RefDNode(antlr::nullAST);
	RefDNode a12_AST = RefDNode(antlr::nullAST);
	RefDNode l12_AST = RefDNode(antlr::nullAST);
	RefDNode l12 = RefDNode(antlr::nullAST);
	RefDNode r12_AST = RefDNode(antlr::nullAST);
	RefDNode r12 = RefDNode(antlr::nullAST);
	RefDNode a13 = RefDNode(antlr::nullAST);
	RefDNode a13_AST = RefDNode(antlr::nullAST);
	RefDNode l13_AST = RefDNode(antlr::nullAST);
	RefDNode l13 = RefDNode(antlr::nullAST);
	RefDNode r13_AST = RefDNode(antlr::nullAST);
	RefDNode r13 = RefDNode(antlr::nullAST);
	RefDNode a14 = RefDNode(antlr::nullAST);
	RefDNode a14_AST = RefDNode(antlr::nullAST);
	RefDNode l14_AST = RefDNode(antlr::nullAST);
	RefDNode l14 = RefDNode(antlr::nullAST);
	RefDNode r14_AST = RefDNode(antlr::nullAST);
	RefDNode r14 = RefDNode(antlr::nullAST);
	RefDNode a15 = RefDNode(antlr::nullAST);
	RefDNode a15_AST = RefDNode(antlr::nullAST);
	RefDNode l15_AST = RefDNode(antlr::nullAST);
	RefDNode l15 = RefDNode(antlr::nullAST);
	RefDNode r15_AST = RefDNode(antlr::nullAST);
	RefDNode r15 = RefDNode(antlr::nullAST);
	RefDNode a16 = RefDNode(antlr::nullAST);
	RefDNode a16_AST = RefDNode(antlr::nullAST);
	RefDNode l16_AST = RefDNode(antlr::nullAST);
	RefDNode l16 = RefDNode(antlr::nullAST);
	RefDNode r16_AST = RefDNode(antlr::nullAST);
	RefDNode r16 = RefDNode(antlr::nullAST);
	RefDNode a17 = RefDNode(antlr::nullAST);
	RefDNode a17_AST = RefDNode(antlr::nullAST);
	RefDNode l17_AST = RefDNode(antlr::nullAST);
	RefDNode l17 = RefDNode(antlr::nullAST);
	RefDNode r17_AST = RefDNode(antlr::nullAST);
	RefDNode r17 = RefDNode(antlr::nullAST);
	RefDNode a18 = RefDNode(antlr::nullAST);
	RefDNode a18_AST = RefDNode(antlr::nullAST);
	RefDNode l18_AST = RefDNode(antlr::nullAST);
	RefDNode l18 = RefDNode(antlr::nullAST);
	RefDNode r18_AST = RefDNode(antlr::nullAST);
	RefDNode r18 = RefDNode(antlr::nullAST);
	RefDNode a19 = RefDNode(antlr::nullAST);
	RefDNode a19_AST = RefDNode(antlr::nullAST);
	RefDNode l19_AST = RefDNode(antlr::nullAST);
	RefDNode l19 = RefDNode(antlr::nullAST);
	RefDNode r19_AST = RefDNode(antlr::nullAST);
	RefDNode r19 = RefDNode(antlr::nullAST);
	
	RefDNode l;
	
	
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case AND_OP_EQ:
	{
		RefDNode __t128 = _t;
		a1 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a1_AST_in = RefDNode(antlr::nullAST);
		a1_AST = astFactory->create(antlr::RefAST(a1));
		antlr::ASTPair __currentAST128 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),AND_OP_EQ);
		_t = _t->getFirstChild();
		l1 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l1_AST = returnAST;
		r1 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r1_AST = returnAST;
		currentAST = __currentAST128;
		_t = __t128;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l1_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(AND_OP,"and")))->add(antlr::RefAST(l1_AST))->add(antlr::RefAST(r1_AST))))))->add(antlr::RefAST(RemoveNextSibling(l1_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case ASTERIX_EQ:
	{
		RefDNode __t129 = _t;
		a2 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a2_AST_in = RefDNode(antlr::nullAST);
		a2_AST = astFactory->create(antlr::RefAST(a2));
		antlr::ASTPair __currentAST129 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),ASTERIX_EQ);
		_t = _t->getFirstChild();
		l2 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l2_AST = returnAST;
		r2 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r2_AST = returnAST;
		currentAST = __currentAST129;
		_t = __t129;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l2_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASTERIX,"*")))->add(antlr::RefAST(l2_AST))->add(antlr::RefAST(r2_AST))))))->add(antlr::RefAST(RemoveNextSibling(l2_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case EQ_OP_EQ:
	{
		RefDNode __t130 = _t;
		a3 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a3_AST_in = RefDNode(antlr::nullAST);
		a3_AST = astFactory->create(antlr::RefAST(a3));
		antlr::ASTPair __currentAST130 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),EQ_OP_EQ);
		_t = _t->getFirstChild();
		l3 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l3_AST = returnAST;
		r3 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r3_AST = returnAST;
		currentAST = __currentAST130;
		_t = __t130;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l3_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(EQ_OP,"eq")))->add(antlr::RefAST(l3_AST))->add(antlr::RefAST(r3_AST))))))->add(antlr::RefAST(RemoveNextSibling(l3_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case GE_OP_EQ:
	{
		RefDNode __t131 = _t;
		a4 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a4_AST_in = RefDNode(antlr::nullAST);
		a4_AST = astFactory->create(antlr::RefAST(a4));
		antlr::ASTPair __currentAST131 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),GE_OP_EQ);
		_t = _t->getFirstChild();
		l4 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l4_AST = returnAST;
		r4 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r4_AST = returnAST;
		currentAST = __currentAST131;
		_t = __t131;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l4_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(GE_OP,"ge")))->add(antlr::RefAST(l4_AST))->add(antlr::RefAST(r4_AST))))))->add(antlr::RefAST(RemoveNextSibling(l4_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case GTMARK_EQ:
	{
		RefDNode __t132 = _t;
		a5 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a5_AST_in = RefDNode(antlr::nullAST);
		a5_AST = astFactory->create(antlr::RefAST(a5));
		antlr::ASTPair __currentAST132 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),GTMARK_EQ);
		_t = _t->getFirstChild();
		l5 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l5_AST = returnAST;
		r5 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r5_AST = returnAST;
		currentAST = __currentAST132;
		_t = __t132;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l5_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(GTMARK,">")))->add(antlr::RefAST(l5_AST))->add(antlr::RefAST(r5_AST))))))->add(antlr::RefAST(RemoveNextSibling(l5_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case GT_OP_EQ:
	{
		RefDNode __t133 = _t;
		a6 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a6_AST_in = RefDNode(antlr::nullAST);
		a6_AST = astFactory->create(antlr::RefAST(a6));
		antlr::ASTPair __currentAST133 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),GT_OP_EQ);
		_t = _t->getFirstChild();
		l6 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l6_AST = returnAST;
		r6 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r6_AST = returnAST;
		currentAST = __currentAST133;
		_t = __t133;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l6_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(GT_OP,"gt")))->add(antlr::RefAST(l6_AST))->add(antlr::RefAST(r6_AST))))))->add(antlr::RefAST(RemoveNextSibling(l6_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case LE_OP_EQ:
	{
		RefDNode __t134 = _t;
		a7 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a7_AST_in = RefDNode(antlr::nullAST);
		a7_AST = astFactory->create(antlr::RefAST(a7));
		antlr::ASTPair __currentAST134 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),LE_OP_EQ);
		_t = _t->getFirstChild();
		l7 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l7_AST = returnAST;
		r7 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r7_AST = returnAST;
		currentAST = __currentAST134;
		_t = __t134;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l7_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(LE_OP,"le")))->add(antlr::RefAST(l7_AST))->add(antlr::RefAST(r7_AST))))))->add(antlr::RefAST(RemoveNextSibling(l7_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case LTMARK_EQ:
	{
		RefDNode __t135 = _t;
		a8 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a8_AST_in = RefDNode(antlr::nullAST);
		a8_AST = astFactory->create(antlr::RefAST(a8));
		antlr::ASTPair __currentAST135 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),LTMARK_EQ);
		_t = _t->getFirstChild();
		l8 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l8_AST = returnAST;
		r8 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r8_AST = returnAST;
		currentAST = __currentAST135;
		_t = __t135;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l8_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(LTMARK,"<")))->add(antlr::RefAST(l8_AST))->add(antlr::RefAST(r8_AST))))))->add(antlr::RefAST(RemoveNextSibling(l8_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case LT_OP_EQ:
	{
		RefDNode __t136 = _t;
		a9 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a9_AST_in = RefDNode(antlr::nullAST);
		a9_AST = astFactory->create(antlr::RefAST(a9));
		antlr::ASTPair __currentAST136 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),LT_OP_EQ);
		_t = _t->getFirstChild();
		l9 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l9_AST = returnAST;
		r9 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r9_AST = returnAST;
		currentAST = __currentAST136;
		_t = __t136;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l9_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(LT_OP,"lt")))->add(antlr::RefAST(l9_AST))->add(antlr::RefAST(r9_AST))))))->add(antlr::RefAST(RemoveNextSibling(l9_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case MATRIX_OP1_EQ:
	{
		RefDNode __t137 = _t;
		a10 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a10_AST_in = RefDNode(antlr::nullAST);
		a10_AST = astFactory->create(antlr::RefAST(a10));
		antlr::ASTPair __currentAST137 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MATRIX_OP1_EQ);
		_t = _t->getFirstChild();
		l10 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l10_AST = returnAST;
		r10 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r10_AST = returnAST;
		currentAST = __currentAST137;
		_t = __t137;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l10_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(MATRIX_OP1,"#")))->add(antlr::RefAST(l10_AST))->add(antlr::RefAST(r10_AST))))))->add(antlr::RefAST(RemoveNextSibling(l10_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case MATRIX_OP2_EQ:
	{
		RefDNode __t138 = _t;
		a11 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a11_AST_in = RefDNode(antlr::nullAST);
		a11_AST = astFactory->create(antlr::RefAST(a11));
		antlr::ASTPair __currentAST138 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MATRIX_OP2_EQ);
		_t = _t->getFirstChild();
		l11 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l11_AST = returnAST;
		r11 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r11_AST = returnAST;
		currentAST = __currentAST138;
		_t = __t138;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l11_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(MATRIX_OP2,"##")))->add(antlr::RefAST(l11_AST))->add(antlr::RefAST(r11_AST))))))->add(antlr::RefAST(RemoveNextSibling(l11_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case MINUS_EQ:
	{
		RefDNode __t139 = _t;
		a12 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a12_AST_in = RefDNode(antlr::nullAST);
		a12_AST = astFactory->create(antlr::RefAST(a12));
		antlr::ASTPair __currentAST139 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MINUS_EQ);
		_t = _t->getFirstChild();
		l12 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l12_AST = returnAST;
		r12 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r12_AST = returnAST;
		currentAST = __currentAST139;
		_t = __t139;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l12_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(MINUS,"-")))->add(antlr::RefAST(l12_AST))->add(antlr::RefAST(r12_AST))))))->add(antlr::RefAST(RemoveNextSibling(l12_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case MOD_OP_EQ:
	{
		RefDNode __t140 = _t;
		a13 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a13_AST_in = RefDNode(antlr::nullAST);
		a13_AST = astFactory->create(antlr::RefAST(a13));
		antlr::ASTPair __currentAST140 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MOD_OP_EQ);
		_t = _t->getFirstChild();
		l13 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l13_AST = returnAST;
		r13 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r13_AST = returnAST;
		currentAST = __currentAST140;
		_t = __t140;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l13_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(MOD_OP,"mod")))->add(antlr::RefAST(l13_AST))->add(antlr::RefAST(r13_AST))))))->add(antlr::RefAST(RemoveNextSibling(l13_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case NE_OP_EQ:
	{
		RefDNode __t141 = _t;
		a14 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a14_AST_in = RefDNode(antlr::nullAST);
		a14_AST = astFactory->create(antlr::RefAST(a14));
		antlr::ASTPair __currentAST141 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),NE_OP_EQ);
		_t = _t->getFirstChild();
		l14 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l14_AST = returnAST;
		r14 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r14_AST = returnAST;
		currentAST = __currentAST141;
		_t = __t141;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l14_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(NE_OP,"ne")))->add(antlr::RefAST(l14_AST))->add(antlr::RefAST(r14_AST))))))->add(antlr::RefAST(RemoveNextSibling(l14_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case OR_OP_EQ:
	{
		RefDNode __t142 = _t;
		a15 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a15_AST_in = RefDNode(antlr::nullAST);
		a15_AST = astFactory->create(antlr::RefAST(a15));
		antlr::ASTPair __currentAST142 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),OR_OP_EQ);
		_t = _t->getFirstChild();
		l15 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l15_AST = returnAST;
		r15 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r15_AST = returnAST;
		currentAST = __currentAST142;
		_t = __t142;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l15_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(OR_OP,"or")))->add(antlr::RefAST(l15_AST))->add(antlr::RefAST(r15_AST))))))->add(antlr::RefAST(RemoveNextSibling(l15_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case PLUS_EQ:
	{
		RefDNode __t143 = _t;
		a16 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a16_AST_in = RefDNode(antlr::nullAST);
		a16_AST = astFactory->create(antlr::RefAST(a16));
		antlr::ASTPair __currentAST143 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),PLUS_EQ);
		_t = _t->getFirstChild();
		l16 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l16_AST = returnAST;
		r16 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r16_AST = returnAST;
		currentAST = __currentAST143;
		_t = __t143;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l16_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(PLUS,"+")))->add(antlr::RefAST(l16_AST))->add(antlr::RefAST(r16_AST))))))->add(antlr::RefAST(RemoveNextSibling(l16_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case POW_EQ:
	{
		RefDNode __t144 = _t;
		a17 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a17_AST_in = RefDNode(antlr::nullAST);
		a17_AST = astFactory->create(antlr::RefAST(a17));
		antlr::ASTPair __currentAST144 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),POW_EQ);
		_t = _t->getFirstChild();
		l17 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l17_AST = returnAST;
		r17 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r17_AST = returnAST;
		currentAST = __currentAST144;
		_t = __t144;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l17_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(POW,"^")))->add(antlr::RefAST(l17_AST))->add(antlr::RefAST(r17_AST))))))->add(antlr::RefAST(RemoveNextSibling(l17_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case SLASH_EQ:
	{
		RefDNode __t145 = _t;
		a18 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a18_AST_in = RefDNode(antlr::nullAST);
		a18_AST = astFactory->create(antlr::RefAST(a18));
		antlr::ASTPair __currentAST145 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),SLASH_EQ);
		_t = _t->getFirstChild();
		l18 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l18_AST = returnAST;
		r18 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r18_AST = returnAST;
		currentAST = __currentAST145;
		_t = __t145;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l18_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(SLASH,"/")))->add(antlr::RefAST(l18_AST))->add(antlr::RefAST(r18_AST))))))->add(antlr::RefAST(RemoveNextSibling(l18_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case XOR_OP_EQ:
	{
		RefDNode __t146 = _t;
		a19 = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a19_AST_in = RefDNode(antlr::nullAST);
		a19_AST = astFactory->create(antlr::RefAST(a19));
		antlr::ASTPair __currentAST146 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),XOR_OP_EQ);
		_t = _t->getFirstChild();
		l19 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		lassign_expr(_t);
		_t = _retTree;
		l19_AST = returnAST;
		r19 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		r19_AST = returnAST;
		currentAST = __currentAST146;
		_t = __t146;
		_t = _t->getNextSibling();
		comp_assign_expr_AST = RefDNode(currentAST.root);
		l=l19_AST; comp_assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ASSIGN,":=")))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(XOR_OP,"xor")))->add(antlr::RefAST(l19_AST))->add(antlr::RefAST(r19_AST))))))->add(antlr::RefAST(RemoveNextSibling(l19_AST)))));
		currentAST.root = comp_assign_expr_AST;
		if ( comp_assign_expr_AST!=RefDNode(antlr::nullAST) &&
			comp_assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = comp_assign_expr_AST->getFirstChild();
		else
			currentAST.child = comp_assign_expr_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	comp_assign_expr_AST = RefDNode(currentAST.root);
	
	AssignReplace( l, comp_assign_expr_AST);
	
	returnAST = comp_assign_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::procedure_call(RefDNode _t) {
	RefDNode procedure_call_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode procedure_call_AST = RefDNode(antlr::nullAST);
	RefDNode p = RefDNode(antlr::nullAST);
	RefDNode p_AST = RefDNode(antlr::nullAST);
	RefDNode id = RefDNode(antlr::nullAST);
	RefDNode id_AST = RefDNode(antlr::nullAST);
	RefDNode para_AST = RefDNode(antlr::nullAST);
	RefDNode para = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case MPCALL:
	{
		RefDNode __t82 = _t;
		RefDNode tmp16_AST = RefDNode(antlr::nullAST);
		RefDNode tmp16_AST_in = RefDNode(antlr::nullAST);
		tmp16_AST = astFactory->create(antlr::RefAST(_t));
		tmp16_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp16_AST));
		antlr::ASTPair __currentAST82 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MPCALL);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		RefDNode tmp17_AST = RefDNode(antlr::nullAST);
		RefDNode tmp17_AST_in = RefDNode(antlr::nullAST);
		tmp17_AST = astFactory->create(antlr::RefAST(_t));
		tmp17_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp17_AST));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		parameter_def(_t, false);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST82;
		_t = __t82;
		_t = _t->getNextSibling();
		procedure_call_AST = RefDNode(currentAST.root);
		break;
	}
	case MPCALL_PARENT:
	{
		RefDNode __t83 = _t;
		RefDNode tmp18_AST = RefDNode(antlr::nullAST);
		RefDNode tmp18_AST_in = RefDNode(antlr::nullAST);
		tmp18_AST = astFactory->create(antlr::RefAST(_t));
		tmp18_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp18_AST));
		antlr::ASTPair __currentAST83 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MPCALL_PARENT);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		RefDNode tmp19_AST = RefDNode(antlr::nullAST);
		RefDNode tmp19_AST_in = RefDNode(antlr::nullAST);
		tmp19_AST = astFactory->create(antlr::RefAST(_t));
		tmp19_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp19_AST));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		RefDNode tmp20_AST = RefDNode(antlr::nullAST);
		RefDNode tmp20_AST_in = RefDNode(antlr::nullAST);
		tmp20_AST = astFactory->create(antlr::RefAST(_t));
		tmp20_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp20_AST));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		parameter_def(_t, false);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST83;
		_t = __t83;
		_t = _t->getNextSibling();
		procedure_call_AST = RefDNode(currentAST.root);
		break;
	}
	case PCALL:
	{
		RefDNode __t84 = _t;
		p = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode p_AST_in = RefDNode(antlr::nullAST);
		p_AST = astFactory->create(antlr::RefAST(p));
		astFactory->addASTChild(currentAST, antlr::RefAST(p_AST));
		antlr::ASTPair __currentAST84 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),PCALL);
		_t = _t->getFirstChild();
		id = _t;
		RefDNode id_AST_in = RefDNode(antlr::nullAST);
		id_AST = astFactory->create(antlr::RefAST(id));
		astFactory->addASTChild(currentAST, antlr::RefAST(id_AST));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		// first search library procedures
		int i=LibProIx(id_AST->getText());
		
		para = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		parameter_def(_t, i != -1 && libProList[ i]->NPar() == -1);
		_t = _retTree;
		para_AST = returnAST;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		
		if( i != -1)
		{
		int nParam = 0;
		if( para_AST != RefDNode(antlr::nullAST))
		nParam = para_AST->GetNParam();
		
		int libParam = libProList[i]->NPar();
		int libParamMin = libProList[i]->NParMin();
		if( libParam != -1 && nParam > libParam)
		throw GDLException(	p, libProList[i]->Name() + ": Too many arguments.");
		if( libParam != -1 && nParam < libParamMin)
		throw GDLException(	p, libProList[i]->Name() + ": Too few arguments.");
		
		p_AST->setType(PCALL_LIB);
		p_AST->setText("pcall_lib");
		id_AST->SetLibPro( libProList[i]);
		}
		else
		{
		// then search user defined procedures
		i=ProIx(id_AST->getText());
		id_AST->SetProIx(i);
		}
		
		currentAST = __currentAST84;
		_t = __t84;
		_t = _t->getNextSibling();
		procedure_call_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = procedure_call_AST;
	_retTree = _t;
}

void GDLTreeParser::for_statement(RefDNode _t) {
	RefDNode for_statement_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode for_statement_AST = RefDNode(antlr::nullAST);
	RefDNode f = RefDNode(antlr::nullAST);
	RefDNode f_AST = RefDNode(antlr::nullAST);
	RefDNode i = RefDNode(antlr::nullAST);
	RefDNode i_AST = RefDNode(antlr::nullAST);
	
	StackSizeGuard<IDList> guard( loopVarStack);
	int labelStart = comp.NDefLabel();
	
	
	RefDNode __t66 = _t;
	f = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode f_AST_in = RefDNode(antlr::nullAST);
	f_AST = astFactory->create(antlr::RefAST(f));
	astFactory->addASTChild(currentAST, antlr::RefAST(f_AST));
	antlr::ASTPair __currentAST66 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),FOR);
	_t = _t->getFirstChild();
	i = _t;
	RefDNode i_AST_in = RefDNode(antlr::nullAST);
	i_AST = astFactory->create(antlr::RefAST(i));
	astFactory->addASTChild(currentAST, antlr::RefAST(i_AST));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	
	i_AST->setType(VAR);
	comp.Var(i_AST);	
	
	loopVarStack.push_back(i_AST->getText());
	
	expr(_t);
	_t = _retTree;
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	expr(_t);
	_t = _retTree;
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	case ARRAYDEF:
	case ARRAYDEF_CONST:
	case ARRAYDEF_GENERALIZED_INDGEN:
	case ARRAYEXPR:
	case ARRAYEXPR_FCALL:
	case ARRAYEXPR_MFCALL:
	case CONSTANT:
	case DEREF:
	case EXPR:
	case FCALL:
	case GDLNULL:
	case MFCALL:
	case MFCALL_PARENT:
	case NSTRUC_REF:
	case POSTDEC:
	case POSTINC:
	case STRUC:
	case SYSVAR:
	case UMINUS:
	case VAR:
	case AND_OP:
	case EQ_OP:
	case GE_OP:
	case GT_OP:
	case LE_OP:
	case LT_OP:
	case MOD_OP:
	case NE_OP:
	case NOT_OP:
	case OR_OP:
	case XOR_OP:
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
	case SLASH:
	case ASTERIX:
	case DOT:
	case POW:
	case MATRIX_OP1:
	case MATRIX_OP2:
	case PLUS:
	case MINUS:
	case LTMARK:
	case GTMARK:
	case LOG_NEG:
	case LOG_AND:
	case LOG_OR:
	case QUESTION:
	{
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		
		f_AST->setType(FOR_STEP);
		f_AST->setText("for_step");
		
		break;
	}
	case BLOCK:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	unblock_empty(_t);
	_t = _retTree;
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	currentAST = __currentAST66;
	_t = __t66;
	_t = _t->getNextSibling();
	
	f_AST->SetLabelRange( labelStart, comp.NDefLabel());
	
	for_statement_AST = RefDNode(currentAST.root);
	returnAST = for_statement_AST;
	_retTree = _t;
}

void GDLTreeParser::foreach_statement(RefDNode _t) {
	RefDNode foreach_statement_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode foreach_statement_AST = RefDNode(antlr::nullAST);
	RefDNode f = RefDNode(antlr::nullAST);
	RefDNode f_AST = RefDNode(antlr::nullAST);
	RefDNode i = RefDNode(antlr::nullAST);
	RefDNode i_AST = RefDNode(antlr::nullAST);
	RefDNode l = RefDNode(antlr::nullAST);
	RefDNode l_AST = RefDNode(antlr::nullAST);
	
	StackSizeGuard<IDList> guard( loopVarStack);
	int labelStart = comp.NDefLabel();
	
	
	RefDNode __t69 = _t;
	f = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode f_AST_in = RefDNode(antlr::nullAST);
	f_AST = astFactory->create(antlr::RefAST(f));
	astFactory->addASTChild(currentAST, antlr::RefAST(f_AST));
	antlr::ASTPair __currentAST69 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),FOREACH);
	_t = _t->getFirstChild();
	i = _t;
	RefDNode i_AST_in = RefDNode(antlr::nullAST);
	i_AST = astFactory->create(antlr::RefAST(i));
	astFactory->addASTChild(currentAST, antlr::RefAST(i_AST));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	
	i_AST->setType(VAR);
	comp.Var(i_AST);	
	loopVarStack.push_back(i_AST->getText());
	
	expr(_t);
	_t = _retTree;
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case IDENTIFIER:
	{
		l = _t;
		RefDNode l_AST_in = RefDNode(antlr::nullAST);
		l_AST = astFactory->create(antlr::RefAST(l));
		astFactory->addASTChild(currentAST, antlr::RefAST(l_AST));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		l_AST->setType(VAR);
		comp.Var(l_AST);	
		loopVarStack.push_back(l_AST->getText());
		
		f_AST->setType(FOREACH_INDEX);
		f_AST->setText("foreach_index");
		
		break;
	}
	case BLOCK:
	{
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	unblock_empty(_t);
	_t = _retTree;
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	currentAST = __currentAST69;
	_t = __t69;
	_t = _t->getNextSibling();
	
	f_AST->SetLabelRange( labelStart, comp.NDefLabel());
	
	foreach_statement_AST = RefDNode(currentAST.root);
	returnAST = foreach_statement_AST;
	_retTree = _t;
}

void GDLTreeParser::repeat_statement(RefDNode _t) {
	RefDNode repeat_statement_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode repeat_statement_AST = RefDNode(antlr::nullAST);
	RefDNode r = RefDNode(antlr::nullAST);
	RefDNode r_AST = RefDNode(antlr::nullAST);
	RefDNode b_AST = RefDNode(antlr::nullAST);
	RefDNode b = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode e = RefDNode(antlr::nullAST);
	
	int labelStart = comp.NDefLabel();
	
	
	RefDNode __t62 = _t;
	r = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode r_AST_in = RefDNode(antlr::nullAST);
	r_AST = astFactory->create(antlr::RefAST(r));
	antlr::ASTPair __currentAST62 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),REPEAT);
	_t = _t->getFirstChild();
	b = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	unblock_empty(_t);
	_t = _retTree;
	b_AST = returnAST;
	e = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	e_AST = returnAST;
	currentAST = __currentAST62;
	_t = __t62;
	_t = _t->getNextSibling();
	repeat_statement_AST = RefDNode(currentAST.root);
	
	r_AST->SetLabelRange( labelStart, comp.NDefLabel());
	repeat_statement_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(r_AST))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(b_AST))));
	if( b_AST == static_cast<RefDNode>(antlr::nullAST))
	Warning( "Warning: Empty REPEAT UNTIL loop detected.");
	
	currentAST.root = repeat_statement_AST;
	if ( repeat_statement_AST!=RefDNode(antlr::nullAST) &&
		repeat_statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = repeat_statement_AST->getFirstChild();
	else
		currentAST.child = repeat_statement_AST;
	currentAST.advanceChildToEnd();
	returnAST = repeat_statement_AST;
	_retTree = _t;
}

void GDLTreeParser::while_statement(RefDNode _t) {
	RefDNode while_statement_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode while_statement_AST = RefDNode(antlr::nullAST);
	RefDNode w = RefDNode(antlr::nullAST);
	RefDNode w_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode e = RefDNode(antlr::nullAST);
	RefDNode s_AST = RefDNode(antlr::nullAST);
	RefDNode s = RefDNode(antlr::nullAST);
	
	int labelStart = comp.NDefLabel();
	
	
	RefDNode __t64 = _t;
	w = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode w_AST_in = RefDNode(antlr::nullAST);
	w_AST = astFactory->create(antlr::RefAST(w));
	antlr::ASTPair __currentAST64 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),WHILE);
	_t = _t->getFirstChild();
	e = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	e_AST = returnAST;
	s = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	statement(_t);
	_t = _retTree;
	s_AST = returnAST;
	currentAST = __currentAST64;
	_t = __t64;
	_t = _t->getNextSibling();
	while_statement_AST = RefDNode(currentAST.root);
	
	w_AST->SetLabelRange( labelStart, comp.NDefLabel());
	
	// swap e <-> s for easier access in interpreter
	while_statement_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(w_AST))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(s_AST))));
	
	currentAST.root = while_statement_AST;
	if ( while_statement_AST!=RefDNode(antlr::nullAST) &&
		while_statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = while_statement_AST->getFirstChild();
	else
		currentAST.child = while_statement_AST;
	currentAST.advanceChildToEnd();
	returnAST = while_statement_AST;
	_retTree = _t;
}

void GDLTreeParser::jump_statement(RefDNode _t) {
	RefDNode jump_statement_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode jump_statement_AST = RefDNode(antlr::nullAST);
	RefDNode g = RefDNode(antlr::nullAST);
	RefDNode g_AST = RefDNode(antlr::nullAST);
	RefDNode i1 = RefDNode(antlr::nullAST);
	RefDNode i1_AST = RefDNode(antlr::nullAST);
	RefDNode r = RefDNode(antlr::nullAST);
	RefDNode r_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode e = RefDNode(antlr::nullAST);
	RefDNode o = RefDNode(antlr::nullAST);
	RefDNode o_AST = RefDNode(antlr::nullAST);
	RefDNode i2 = RefDNode(antlr::nullAST);
	RefDNode i2_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case GOTO:
	{
		RefDNode __t74 = _t;
		g = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode g_AST_in = RefDNode(antlr::nullAST);
		g_AST = astFactory->create(antlr::RefAST(g));
		antlr::ASTPair __currentAST74 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),GOTO);
		_t = _t->getFirstChild();
		i1 = _t;
		RefDNode i1_AST_in = RefDNode(antlr::nullAST);
		i1_AST = astFactory->create(antlr::RefAST(i1));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		currentAST = __currentAST74;
		_t = __t74;
		_t = _t->getNextSibling();
		jump_statement_AST = RefDNode(currentAST.root);
		
			  jump_statement_AST=astFactory->create(GOTO,i1->getText());
		jump_statement_AST->SetLine(g->getLine()); 
		//	  #jump_statement=#[GOTO,i1->getText()]; // doesn't work
		//	  comp.Goto(#jump_statement); 
			
		currentAST.root = jump_statement_AST;
		if ( jump_statement_AST!=RefDNode(antlr::nullAST) &&
			jump_statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = jump_statement_AST->getFirstChild();
		else
			currentAST.child = jump_statement_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case RETURN:
	{
		RefDNode __t75 = _t;
		r = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode r_AST_in = RefDNode(antlr::nullAST);
		r_AST = astFactory->create(antlr::RefAST(r));
		antlr::ASTPair __currentAST75 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),RETURN);
		_t = _t->getFirstChild();
		bool exprThere=false;
		{
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ARRAYDEF:
		case ARRAYDEF_CONST:
		case ARRAYDEF_GENERALIZED_INDGEN:
		case ARRAYEXPR:
		case ARRAYEXPR_FCALL:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case GDLNULL:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case UMINUS:
		case VAR:
		case AND_OP:
		case EQ_OP:
		case GE_OP:
		case GT_OP:
		case LE_OP:
		case LT_OP:
		case MOD_OP:
		case NE_OP:
		case NOT_OP:
		case OR_OP:
		case XOR_OP:
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
		case SLASH:
		case ASTERIX:
		case DOT:
		case POW:
		case MATRIX_OP1:
		case MATRIX_OP2:
		case PLUS:
		case MINUS:
		case LTMARK:
		case GTMARK:
		case LOG_NEG:
		case LOG_AND:
		case LOG_OR:
		case QUESTION:
		{
			e = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
			expr(_t);
			_t = _retTree;
			e_AST = returnAST;
			exprThere=true;
			break;
		}
		case 3:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		currentAST = __currentAST75;
		_t = __t75;
		_t = _t->getNextSibling();
		jump_statement_AST = RefDNode(currentAST.root);
		
			  if( comp.IsFun())
			  	{
				if( !exprThere)	throw GDLException(	r, 
		"Return statement in functions "
		"must have 1 value.");
		
		//         // wrong: this is only true for l_function as return value
		//         // a ARRAYEXPR_MFCALL can only be a MFCALL here -> change tree
		//         if( #e->getType() == ARRAYEXPR_MFCALL)
		//             {
		//                 #e->setType( MFCALL);
		//                 #e->setText( "mfcall");
		//                 RefDNode mfc;
		//                 mfc = #e->getFirstChild();
		//                 mfc = mfc->getNextSibling();
		//                 #e->setFirstChild( antlr::RefAST(mfc));             
		//             }
		
				jump_statement_AST=RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(RETF,"retf")))->add(antlr::RefAST(e_AST))));
		jump_statement_AST->SetLine(r->getLine()); 
				}
			  else
			  	{
				if( exprThere) throw GDLException(	_t, 
		"Return statement in "
		"procedures cannot have values.");
				jump_statement_AST=astFactory->create(RETP,"retp"); // astFactory.create(RETP,"retp");
		jump_statement_AST->SetLine(r->getLine()); 
			  	}
			
		currentAST.root = jump_statement_AST;
		if ( jump_statement_AST!=RefDNode(antlr::nullAST) &&
			jump_statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = jump_statement_AST->getFirstChild();
		else
			currentAST.child = jump_statement_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case ON_IOERROR:
	{
		RefDNode __t77 = _t;
		o = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode o_AST_in = RefDNode(antlr::nullAST);
		o_AST = astFactory->create(antlr::RefAST(o));
		antlr::ASTPair __currentAST77 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),ON_IOERROR);
		_t = _t->getFirstChild();
		i2 = _t;
		RefDNode i2_AST_in = RefDNode(antlr::nullAST);
		i2_AST = astFactory->create(antlr::RefAST(i2));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		currentAST = __currentAST77;
		_t = __t77;
		_t = _t->getNextSibling();
		jump_statement_AST = RefDNode(currentAST.root);
		
		if( i2->getText() == "NULL")
		{
		jump_statement_AST=astFactory->create(ON_IOERROR_NULL,
		"on_ioerror_null");
		jump_statement_AST->SetLine(o->getLine()); 
		}
		else
		{
		jump_statement_AST=astFactory->create(ON_IOERROR,i2->getText());
		jump_statement_AST->SetLine(o->getLine()); 
		//	            #jump_statement=#[ON_IOERROR,i2->getText()];
		//              comp.Goto(#jump_statement); // same handling		 
		}
			
		currentAST.root = jump_statement_AST;
		if ( jump_statement_AST!=RefDNode(antlr::nullAST) &&
			jump_statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = jump_statement_AST->getFirstChild();
		else
			currentAST.child = jump_statement_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = jump_statement_AST;
	_retTree = _t;
}

void GDLTreeParser::if_statement(RefDNode _t) {
	RefDNode if_statement_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode if_statement_AST = RefDNode(antlr::nullAST);
	RefDNode i = RefDNode(antlr::nullAST);
	RefDNode i_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode e = RefDNode(antlr::nullAST);
	RefDNode s1_AST = RefDNode(antlr::nullAST);
	RefDNode s1 = RefDNode(antlr::nullAST);
	RefDNode s2_AST = RefDNode(antlr::nullAST);
	RefDNode s2 = RefDNode(antlr::nullAST);
	
	//     int labelStart = comp.NDefLabel();
	RefDNode block;
	
	
	RefDNode __t79 = _t;
	i = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode i_AST_in = RefDNode(antlr::nullAST);
	i_AST = astFactory->create(antlr::RefAST(i));
	antlr::ASTPair __currentAST79 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),IF);
	_t = _t->getFirstChild();
	e = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	e_AST = returnAST;
	s1 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	statement(_t);
	_t = _retTree;
	s1_AST = returnAST;
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case 3:
	{
		if_statement_AST = RefDNode(currentAST.root);
		
		//         #i->SetLabelRange( labelStart, comp.NDefLabel());
		if_statement_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(i_AST))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(s1_AST))));
		
		currentAST.root = if_statement_AST;
		if ( if_statement_AST!=RefDNode(antlr::nullAST) &&
			if_statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = if_statement_AST->getFirstChild();
		else
			currentAST.child = if_statement_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	case ASSIGN:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case COMMONDECL:
	case COMMONDEF:
	case FOR:
	case FOREACH:
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	case REPEAT:
	case RETURN:
	case WHILE:
	case CASE:
	case FORWARD:
	case GOTO:
	case IF:
	case ON_IOERROR:
	case SWITCH:
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
	{
		s2 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		statement(_t);
		_t = _retTree;
		s2_AST = returnAST;
		if_statement_AST = RefDNode(currentAST.root);
		
		i_AST->setText( "if_else");
		i_AST->setType( IF_ELSE);
		//         #i->SetLabelRange( labelStart, comp.NDefLabel());
		if( s1_AST->getType() != BLOCK)
		{
		block = astFactory->create(BLOCK,"block");
		block->SetLine( s1_AST->getLine());
		if_statement_AST=RefDNode(astFactory->make((new antlr::ASTArray(4))->add(antlr::RefAST(i_AST))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(block))->add(antlr::RefAST(s1_AST))))))->add(antlr::RefAST(s2_AST))));
		}
		else
		if_statement_AST=RefDNode(astFactory->make((new antlr::ASTArray(4))->add(antlr::RefAST(i_AST))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(s1_AST))->add(antlr::RefAST(s2_AST))));
		
		currentAST.root = if_statement_AST;
		if ( if_statement_AST!=RefDNode(antlr::nullAST) &&
			if_statement_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = if_statement_AST->getFirstChild();
		else
			currentAST.child = if_statement_AST;
		currentAST.advanceChildToEnd();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	currentAST = __currentAST79;
	_t = __t79;
	_t = _t->getNextSibling();
	returnAST = if_statement_AST;
	_retTree = _t;
}

void GDLTreeParser::parameter_def(RefDNode _t,
	bool varNum
) {
	RefDNode parameter_def_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode parameter_def_AST = RefDNode(antlr::nullAST);
	
	// count positional parameters
	int nKey = 0;
	int nPar = 0;
	
	
	{ // ( ... )*
	for (;;) {
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case KEYDEF:
		{
			key_parameter(_t);
			_t = _retTree;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			
			++nKey;
			
			break;
		}
		case ASSIGN:
		case ARRAYDEF:
		case ARRAYDEF_CONST:
		case ARRAYDEF_GENERALIZED_INDGEN:
		case ARRAYEXPR:
		case ARRAYEXPR_FCALL:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case GDLNULL:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case UMINUS:
		case VAR:
		case AND_OP:
		case EQ_OP:
		case GE_OP:
		case GT_OP:
		case LE_OP:
		case LT_OP:
		case MOD_OP:
		case NE_OP:
		case NOT_OP:
		case OR_OP:
		case XOR_OP:
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
		case SLASH:
		case ASTERIX:
		case DOT:
		case POW:
		case MATRIX_OP1:
		case MATRIX_OP2:
		case PLUS:
		case MINUS:
		case LTMARK:
		case GTMARK:
		case LOG_NEG:
		case LOG_AND:
		case LOG_OR:
		case QUESTION:
		{
			pos_parameter(_t, varNum);
			_t = _retTree;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			
			++nPar;
			
			break;
		}
		default:
		{
			goto _loop87;
		}
		}
	}
	_loop87:;
	} // ( ... )*
	
	if( nPar > 0 || nKey > 0)
	{
	RefDNode(currentAST.root)->SetNParam( nPar);
	}
	
	parameter_def_AST = RefDNode(currentAST.root);
	returnAST = parameter_def_AST;
	_retTree = _t;
}

void GDLTreeParser::key_parameter(RefDNode _t) {
	RefDNode key_parameter_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode key_parameter_AST = RefDNode(antlr::nullAST);
	RefDNode d = RefDNode(antlr::nullAST);
	RefDNode d_AST = RefDNode(antlr::nullAST);
	RefDNode i = RefDNode(antlr::nullAST);
	RefDNode i_AST = RefDNode(antlr::nullAST);
	RefDNode k_AST = RefDNode(antlr::nullAST);
	RefDNode k = RefDNode(antlr::nullAST);
	
	RefDNode variable;
	
	
	RefDNode __t89 = _t;
	d = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode d_AST_in = RefDNode(antlr::nullAST);
	d_AST = astFactory->create(antlr::RefAST(d));
	antlr::ASTPair __currentAST89 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),KEYDEF);
	_t = _t->getFirstChild();
	i = _t;
	RefDNode i_AST_in = RefDNode(antlr::nullAST);
	i_AST = astFactory->create(antlr::RefAST(i));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	k = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	k_AST = returnAST;
	key_parameter_AST = RefDNode(currentAST.root);
	
	variable=comp.ByReference(k_AST);
	if( variable != static_cast<RefDNode>(antlr::nullAST))
	{
	int vT = variable->getType();
	if( IsREF_CHECK(vT))
	{
	d_AST=astFactory->create(KEYDEF_REF_CHECK,"keydef_ref_check");
	key_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(d_AST))->add(antlr::RefAST(i_AST))->add(antlr::RefAST(k_AST))));
	}
	else if( variable == k_AST)
	{
	d_AST=astFactory->create(KEYDEF_REF,"keydef_ref");
	key_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(d_AST))->add(antlr::RefAST(i_AST))->add(antlr::RefAST(variable))));
	}
	else
	{
	d_AST=astFactory->create(KEYDEF_REF_EXPR,"keydef_ref_expr");
	key_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(4))->add(antlr::RefAST(d_AST))->add(antlr::RefAST(i_AST))->add(antlr::RefAST(k_AST))->add(antlr::RefAST(variable))));
	}
	}
	else 
	{
	int t = k_AST->getType();
	// Note: Right now there are no MFCALL_LIB or MFCALL_PARENT_LIB nodes
	if( IsREF_CHECK(t))
	//                            t  == FCALL_LIB 
	//                         || t == MFCALL_LIB  // || t == FCALL_LIB_N_ELEMENTS 
	//                         || t == MFCALL_PARENT_LIB  
	//                         || t == QUESTION 
	//                         || t == FCALL || t == MFCALL || t == MFCALL_PARENT
	//                         || t == ARRAYEXPR_FCALL
	//                         || t == ARRAYEXPR_MFCALL
	// //                 t == FCALL_LIB_RETNEW || t == MFCALL_LIB_RETNEW || 
	// //                 t == MFCALL_PARENT_LIB_RETNEW //||
	// //                 t == ARRARYEXPR_MFCALL_LIB // MFCALL_LIB or VAR or DEREF 
	// // )
	{
	d_AST=astFactory->create(KEYDEF_REF_CHECK,"keydef_ref_check");
	key_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(d_AST))->add(antlr::RefAST(i_AST))->add(antlr::RefAST(k_AST))));
	}
	//                     else if( t == FCALL_LIB_RETNEW || t == MFCALL_LIB_RETNEW) 
	//                     {
	// //                         #d=#[KEYDEF_REF,"keydef_ref"];
	//                         #key_parameter=#(d,i,k);
	//                     }
	else
	{
	key_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(d_AST))->add(antlr::RefAST(i_AST))->add(antlr::RefAST(k_AST))));
	}
	}
	
	currentAST.root = key_parameter_AST;
	if ( key_parameter_AST!=RefDNode(antlr::nullAST) &&
		key_parameter_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = key_parameter_AST->getFirstChild();
	else
		currentAST.child = key_parameter_AST;
	currentAST.advanceChildToEnd();
	currentAST = __currentAST89;
	_t = __t89;
	_t = _t->getNextSibling();
	returnAST = key_parameter_AST;
	_retTree = _t;
}

void GDLTreeParser::pos_parameter(RefDNode _t,
	bool varNum
) {
	RefDNode pos_parameter_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode pos_parameter_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode e = RefDNode(antlr::nullAST);
	
	RefDNode variable;
	
	
	e = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	e_AST = returnAST;
	pos_parameter_AST = RefDNode(currentAST.root);
	
	variable=comp.ByReference(e_AST);
	if( variable != static_cast<RefDNode>(antlr::nullAST))
	{
	int vT = variable->getType();
	if( IsREF_CHECK(vT))
	{
	if( varNum)
	pos_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(REF_CHECK_VN,"ref_check_vn")))->add(antlr::RefAST(e_AST))));
	else
	pos_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(REF_CHECK,"ref_check")))->add(antlr::RefAST(e_AST))));
	}
	else if( variable == e_AST)
	{
	if( varNum)
	pos_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(REF_VN,"ref_vn")))->add(antlr::RefAST(variable))));
	else
	pos_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(REF,"ref")))->add(antlr::RefAST(variable))));
	}
	else
	{
	if( varNum)
	pos_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(REF_EXPR_VN,"ref_expr_vn")))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(variable))));
	else
	pos_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(REF_EXPR,"ref_expr")))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(variable))));
	}
	}
	else 
	{
	int t = e_AST->getType();
	// Note: Right now there are no MFCALL_LIB or MFCALL_PARENT_LIB nodes
	if( IsREF_CHECK(t))
	//                 if( t == FCALL_LIB 
	//                     || t == MFCALL_LIB  //t == FCALL_LIB_N_ELEMENTS ||
	//                     || t == MFCALL_PARENT_LIB 
	//                     || t == QUESTION 
	// // TODO: These are ref check as well, but parameter nodes need to know
	//                     || t == FCALL || t == MFCALL || t == MFCALL_PARENT
	//                     || t == ARRAYEXPR_FCALL
	//                     || t == ARRAYEXPR_MFCALL
	// //                      t == FCALL_LIB_RETNEW || t == MFCALL_LIB_RETNEW || 
	// //                      t == MFCALL_PARENT_LIB_RETNEW
	// //                      t == ARRARYEXPR_MFCALL_LIB // MFCALL_LIB or VAR or DEREF 
	//                     ) 
	{
	// something like: CALLAPRO,reform(a,/OVERWRITE)
	if( varNum)
	pos_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(REF_CHECK_VN,"ref_check_vn")))->add(antlr::RefAST(e_AST))));
	else
	pos_parameter_AST=RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(REF_CHECK,"ref_check")))->add(antlr::RefAST(e_AST))));
	}
	else
	{
	if( varNum)
	pos_parameter_AST= RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(PARAEXPR_VN,"paraexpr_vn")))->add(antlr::RefAST(e_AST))));
	else
	pos_parameter_AST= RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(PARAEXPR,"paraexpr")))->add(antlr::RefAST(e_AST))));
	}
	}
	
	currentAST.root = pos_parameter_AST;
	if ( pos_parameter_AST!=RefDNode(antlr::nullAST) &&
		pos_parameter_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = pos_parameter_AST->getFirstChild();
	else
		currentAST.child = pos_parameter_AST;
	currentAST.advanceChildToEnd();
	returnAST = pos_parameter_AST;
	_retTree = _t;
}

int  GDLTreeParser::array_def(RefDNode _t) {
	int depth;
	RefDNode array_def_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode array_def_AST = RefDNode(antlr::nullAST);
	RefDNode a = RefDNode(antlr::nullAST);
	RefDNode a_AST = RefDNode(antlr::nullAST);
	RefDNode aa = RefDNode(antlr::nullAST);
	RefDNode aa_AST = RefDNode(antlr::nullAST);
	RefDNode aaa = RefDNode(antlr::nullAST);
	RefDNode aaa_AST = RefDNode(antlr::nullAST);
	
	RefDNode sPos;
	
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYDEF:
	{
		RefDNode __t92 = _t;
		a = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode a_AST_in = RefDNode(antlr::nullAST);
		a_AST = astFactory->create(antlr::RefAST(a));
		astFactory->addASTChild(currentAST, antlr::RefAST(a_AST));
		antlr::ASTPair __currentAST92 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),ARRAYDEF);
		_t = _t->getFirstChild();
		sPos=_t;
		{ // ( ... )*
		for (;;) {
			if (_t == RefDNode(antlr::nullAST) )
				_t = ASTNULL;
			if ((_tokenSet_1.member(_t->getType()))) {
				expr(_t);
				_t = _retTree;
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				/*check for constant expression*/
			}
			else {
				goto _loop94;
			}
			
		}
		_loop94:;
		} // ( ... )*
		currentAST = __currentAST92;
		_t = __t92;
		_t = _t->getNextSibling();
		
		// better check here as then CONSTANT is propagated
		depth=0;
		//             bool constant = true;
		//             bool depth0 = false;
		for( RefDNode e=sPos; 
		e != static_cast<RefDNode>(antlr::nullAST);
		e=e->getNextSibling())
		{
		//                 if( e->getType() != CONSTANT)
		//                     constant = false;
		//                 if( !depth0)
		if( (e->getType() != ARRAYDEF) && (e->getType() != ARRAYDEF_CONST) && (e->getType() != ARRAYDEF_GENERALIZED_INDGEN))
		{
		depth=0;
		break;
		//                     depth0 = true;
		}
		else
		{
		int act=array_def(e); // recursive call
		act=act+1;
		if( depth == 0)
		{
		depth=act;
		}
		else
		{
		if( depth > act) depth=act;
		}
		}   
		}
		a_AST->SetArrayDepth(depth);
		
		array_def_AST = RefDNode(currentAST.root);
		break;
	}
	case ARRAYDEF_CONST:
	{
		RefDNode __t95 = _t;
		aa = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode aa_AST_in = RefDNode(antlr::nullAST);
		aa_AST = astFactory->create(antlr::RefAST(aa));
		astFactory->addASTChild(currentAST, antlr::RefAST(aa_AST));
		antlr::ASTPair __currentAST95 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),ARRAYDEF_CONST);
		_t = _t->getFirstChild();
		sPos=_t;
		{ // ( ... )*
		for (;;) {
			if (_t == RefDNode(antlr::nullAST) )
				_t = ASTNULL;
			if ((_tokenSet_1.member(_t->getType()))) {
				expr(_t);
				_t = _retTree;
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			else {
				goto _loop97;
			}
			
		}
		_loop97:;
		} // ( ... )*
		currentAST = __currentAST95;
		_t = __t95;
		_t = _t->getNextSibling();
		
		depth=0;
		for( RefDNode e=sPos; 
		e != static_cast<RefDNode>(antlr::nullAST);
		e=e->getNextSibling())
		{
		if( (e->getType() != ARRAYDEF) && (e->getType() != ARRAYDEF_CONST) && (e->getType() != ARRAYDEF_GENERALIZED_INDGEN))
		{
		depth=0;
		break;
		}
		else
		{
		int act=array_def(e); // recursive call
		act=act+1;
		if( depth == 0)
		{
		depth=act;
		}
		else
		{
		if( depth > act) depth=act;
		}
		}   
		}
		aa_AST->SetArrayDepth(depth);
		
		array_def_AST = RefDNode(currentAST.root);
		break;
	}
	case ARRAYDEF_GENERALIZED_INDGEN:
	{
		RefDNode __t98 = _t;
		aaa = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode aaa_AST_in = RefDNode(antlr::nullAST);
		aaa_AST = astFactory->create(antlr::RefAST(aaa));
		astFactory->addASTChild(currentAST, antlr::RefAST(aaa_AST));
		antlr::ASTPair __currentAST98 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),ARRAYDEF_GENERALIZED_INDGEN);
		_t = _t->getFirstChild();
		sPos=_t;
		{ // ( ... )*
		for (;;) {
			if (_t == RefDNode(antlr::nullAST) )
				_t = ASTNULL;
			if ((_tokenSet_1.member(_t->getType()))) {
				expr(_t);
				_t = _retTree;
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			else {
				goto _loop100;
			}
			
		}
		_loop100:;
		} // ( ... )*
		currentAST = __currentAST98;
		_t = __t98;
		_t = _t->getNextSibling();
		
		depth=0;
		for( RefDNode e=sPos; 
		e != static_cast<RefDNode>(antlr::nullAST);
		e=e->getNextSibling())
		{
		if( (e->getType() != ARRAYDEF) && (e->getType() != ARRAYDEF_CONST) && (e->getType() != ARRAYDEF_GENERALIZED_INDGEN))
		{
		depth=0;
		break;
		}
		else
		{
		int act=array_def(e); // recursive call
		act=act+1;
		if( depth == 0)
		{
		depth=act; std::cerr<<act<<std::endl;
		}
		else
		{
		if( depth > act) depth=act;
		}
		}   
		}
		aaa_AST->SetArrayDepth(depth);
		
		array_def_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = array_def_AST;
	_retTree = _t;
	return depth;
}

void GDLTreeParser::struct_def(RefDNode _t) {
	RefDNode struct_def_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode struct_def_AST = RefDNode(antlr::nullAST);
	RefDNode n = RefDNode(antlr::nullAST);
	RefDNode n_AST = RefDNode(antlr::nullAST);
	
	bool noTagName = false;
	
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case NSTRUC_REF:
	{
		RefDNode __t102 = _t;
		n = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode n_AST_in = RefDNode(antlr::nullAST);
		n_AST = astFactory->create(antlr::RefAST(n));
		astFactory->addASTChild(currentAST, antlr::RefAST(n_AST));
		antlr::ASTPair __currentAST102 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),NSTRUC_REF);
		_t = _t->getFirstChild();
		RefDNode tmp21_AST = RefDNode(antlr::nullAST);
		RefDNode tmp21_AST_in = RefDNode(antlr::nullAST);
		tmp21_AST = astFactory->create(antlr::RefAST(_t));
		tmp21_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp21_AST));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		{
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ARRAYDEF:
		case ARRAYDEF_CONST:
		case ARRAYDEF_GENERALIZED_INDGEN:
		case ARRAYEXPR:
		case ARRAYEXPR_FCALL:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case GDLNULL:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case UMINUS:
		case VAR:
		case IDENTIFIER:
		case AND_OP:
		case EQ_OP:
		case GE_OP:
		case GT_OP:
		case INHERITS:
		case LE_OP:
		case LT_OP:
		case MOD_OP:
		case NE_OP:
		case NOT_OP:
		case OR_OP:
		case XOR_OP:
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
		case SLASH:
		case ASTERIX:
		case DOT:
		case POW:
		case MATRIX_OP1:
		case MATRIX_OP2:
		case PLUS:
		case MINUS:
		case LTMARK:
		case GTMARK:
		case LOG_NEG:
		case LOG_AND:
		case LOG_OR:
		case QUESTION:
		{
			{ // ( ... )+
			int _cnt105=0;
			for (;;) {
				if (_t == RefDNode(antlr::nullAST) )
					_t = ASTNULL;
				switch ( _t->getType()) {
				case ASSIGN:
				case ARRAYDEF:
				case ARRAYDEF_CONST:
				case ARRAYDEF_GENERALIZED_INDGEN:
				case ARRAYEXPR:
				case ARRAYEXPR_FCALL:
				case ARRAYEXPR_MFCALL:
				case CONSTANT:
				case DEREF:
				case EXPR:
				case FCALL:
				case GDLNULL:
				case MFCALL:
				case MFCALL_PARENT:
				case NSTRUC_REF:
				case POSTDEC:
				case POSTINC:
				case STRUC:
				case SYSVAR:
				case UMINUS:
				case VAR:
				case AND_OP:
				case EQ_OP:
				case GE_OP:
				case GT_OP:
				case LE_OP:
				case LT_OP:
				case MOD_OP:
				case NE_OP:
				case NOT_OP:
				case OR_OP:
				case XOR_OP:
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
				case SLASH:
				case ASTERIX:
				case DOT:
				case POW:
				case MATRIX_OP1:
				case MATRIX_OP2:
				case PLUS:
				case MINUS:
				case LTMARK:
				case GTMARK:
				case LOG_NEG:
				case LOG_AND:
				case LOG_OR:
				case QUESTION:
				{
					expr(_t);
					_t = _retTree;
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					noTagName = true;
					break;
				}
				case IDENTIFIER:
				{
					RefDNode tmp22_AST = RefDNode(antlr::nullAST);
					RefDNode tmp22_AST_in = RefDNode(antlr::nullAST);
					tmp22_AST = astFactory->create(antlr::RefAST(_t));
					tmp22_AST_in = _t;
					astFactory->addASTChild(currentAST, antlr::RefAST(tmp22_AST));
					match(antlr::RefAST(_t),IDENTIFIER);
					_t = _t->getNextSibling();
					expr(_t);
					_t = _retTree;
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					break;
				}
				case INHERITS:
				{
					RefDNode tmp23_AST = RefDNode(antlr::nullAST);
					RefDNode tmp23_AST_in = RefDNode(antlr::nullAST);
					tmp23_AST = astFactory->create(antlr::RefAST(_t));
					tmp23_AST_in = _t;
					astFactory->addASTChild(currentAST, antlr::RefAST(tmp23_AST));
					match(antlr::RefAST(_t),INHERITS);
					_t = _t->getNextSibling();
					RefDNode tmp24_AST = RefDNode(antlr::nullAST);
					RefDNode tmp24_AST_in = RefDNode(antlr::nullAST);
					tmp24_AST = astFactory->create(antlr::RefAST(_t));
					tmp24_AST_in = _t;
					astFactory->addASTChild(currentAST, antlr::RefAST(tmp24_AST));
					match(antlr::RefAST(_t),IDENTIFIER);
					_t = _t->getNextSibling();
					break;
				}
				default:
				{
					if ( _cnt105>=1 ) { goto _loop105; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
				}
				}
				_cnt105++;
			}
			_loop105:;
			}  // ( ... )+
			
			// set to nstruct if defined here
			n_AST->setType(NSTRUC); 
			n_AST->setText("nstruct");
			n_AST->DefinedStruct( noTagName);
			
			break;
		}
		case 3:
		{
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		currentAST = __currentAST102;
		_t = __t102;
		_t = _t->getNextSibling();
		struct_def_AST = RefDNode(currentAST.root);
		break;
	}
	case STRUC:
	{
		RefDNode __t106 = _t;
		RefDNode tmp25_AST = RefDNode(antlr::nullAST);
		RefDNode tmp25_AST_in = RefDNode(antlr::nullAST);
		tmp25_AST = astFactory->create(antlr::RefAST(_t));
		tmp25_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp25_AST));
		antlr::ASTPair __currentAST106 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),STRUC);
		_t = _t->getFirstChild();
		{ // ( ... )+
		int _cnt108=0;
		for (;;) {
			if (_t == RefDNode(antlr::nullAST) )
				_t = ASTNULL;
			if ((_t->getType() == IDENTIFIER)) {
				tag_def(_t);
				_t = _retTree;
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			else {
				if ( _cnt108>=1 ) { goto _loop108; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
			}
			
			_cnt108++;
		}
		_loop108:;
		}  // ( ... )+
		currentAST = __currentAST106;
		_t = __t106;
		_t = _t->getNextSibling();
		struct_def_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = struct_def_AST;
	_retTree = _t;
}

void GDLTreeParser::tag_def(RefDNode _t) {
	RefDNode tag_def_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_def_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp26_AST = RefDNode(antlr::nullAST);
	RefDNode tmp26_AST_in = RefDNode(antlr::nullAST);
	tmp26_AST = astFactory->create(antlr::RefAST(_t));
	tmp26_AST_in = _t;
	astFactory->addASTChild(currentAST, antlr::RefAST(tmp26_AST));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	expr(_t);
	_t = _retTree;
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	tag_def_AST = RefDNode(currentAST.root);
	returnAST = tag_def_AST;
	_retTree = _t;
}

void GDLTreeParser::arrayindex(RefDNode _t,
	ArrayIndexVectorT* ixList
) {
	RefDNode arrayindex_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode arrayindex_AST = RefDNode(antlr::nullAST);
	RefDNode e1_AST = RefDNode(antlr::nullAST);
	RefDNode e1 = RefDNode(antlr::nullAST);
	RefDNode e2_AST = RefDNode(antlr::nullAST);
	RefDNode e2 = RefDNode(antlr::nullAST);
	RefDNode e3_AST = RefDNode(antlr::nullAST);
	RefDNode e3 = RefDNode(antlr::nullAST);
	RefDNode e4_AST = RefDNode(antlr::nullAST);
	RefDNode e4 = RefDNode(antlr::nullAST);
	
	BaseGDL *c1, *c2, *c3, *c4;
	
	
	{
	RefDNode __t112 = _t;
	RefDNode tmp27_AST = RefDNode(antlr::nullAST);
	RefDNode tmp27_AST_in = RefDNode(antlr::nullAST);
	tmp27_AST = astFactory->create(antlr::RefAST(_t));
	tmp27_AST_in = _t;
	antlr::ASTPair __currentAST112 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),ARRAYIX);
	_t = _t->getFirstChild();
	{
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ALL:
	{
		RefDNode tmp28_AST = RefDNode(antlr::nullAST);
		RefDNode tmp28_AST_in = RefDNode(antlr::nullAST);
		tmp28_AST = astFactory->create(antlr::RefAST(_t));
		tmp28_AST_in = _t;
		match(antlr::RefAST(_t),ALL);
		_t = _t->getNextSibling();
		
		ixList->push_back( new ArrayIndexAll());
		
		break;
	}
	case ASSIGN:
	case ARRAYDEF:
	case ARRAYDEF_CONST:
	case ARRAYDEF_GENERALIZED_INDGEN:
	case ARRAYEXPR:
	case ARRAYEXPR_FCALL:
	case ARRAYEXPR_MFCALL:
	case CONSTANT:
	case DEREF:
	case EXPR:
	case FCALL:
	case GDLNULL:
	case MFCALL:
	case MFCALL_PARENT:
	case NSTRUC_REF:
	case POSTDEC:
	case POSTINC:
	case STRUC:
	case SYSVAR:
	case UMINUS:
	case VAR:
	case AND_OP:
	case EQ_OP:
	case GE_OP:
	case GT_OP:
	case LE_OP:
	case LT_OP:
	case MOD_OP:
	case NE_OP:
	case NOT_OP:
	case OR_OP:
	case XOR_OP:
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
	case SLASH:
	case ASTERIX:
	case DOT:
	case POW:
	case MATRIX_OP1:
	case MATRIX_OP2:
	case PLUS:
	case MINUS:
	case LTMARK:
	case GTMARK:
	case LOG_NEG:
	case LOG_AND:
	case LOG_OR:
	case QUESTION:
	{
		{
		e1 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		expr(_t);
		_t = _retTree;
		e1_AST = returnAST;
		
		// in ConstantIndex the cData of all nodes is stolen
		// (in case it succeeds)
		// therefore we build here the new constant node with
		// proper cData. See below (e2)
		c1 = comp.ConstantIndex( e1_AST);
		if( c1 != NULL)
		{    
		int e1Line = e1_AST->getLine();
		e1_AST = astFactory->create(CONSTANT,"CONST_IX");
		e1_AST->ResetCData( c1);
		e1_AST->SetLine( e1Line);
		}
		
		{
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case 3:
		{
			arrayindex_AST = RefDNode(currentAST.root);
			
			bool    constantOK = false;
			
			if( c1 != NULL)
			{    
			DType dType = c1->Type();
			int typeCheck = DTypeOrder[ dType];
			if( dType == GDL_STRING || typeCheck >= 100)
			{
			//delete c1;
			}
			else
			{
			
			try {
			// ATTENTION: These two grab c1 (all others don't)
			// a bit unclean, but for maximum efficiency
			if( c1->Rank() == 0)
			ixList->push_back( new CArrayIndexScalar( c1));
			else
			ixList->push_back( new CArrayIndexIndexed( c1));
			
			// prevent c1 from being deleted
			e1_AST->StealCData(); // ok, as #e1 is not used anymore
			
			constantOK = true;
			}
			catch( GDLException& e) {
			//delete c1; // owned by #e1
			}  
			}
			}
			
			if( !constantOK)
			{
			if( LoopVar( e1_AST))
			{
			if( e1_AST->getType() == VAR)
			ixList->push_back( new ArrayIndexScalar( e1_AST));
			else
			ixList->push_back( new ArrayIndexScalarVP( e1_AST));
			}
			else
			{
			arrayindex_AST = e1_AST;
			ixList->push_back( new ArrayIndexIndexed());
			}
			}
			
			currentAST.root = arrayindex_AST;
			if ( arrayindex_AST!=RefDNode(antlr::nullAST) &&
				arrayindex_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = arrayindex_AST->getFirstChild();
			else
				currentAST.child = arrayindex_AST;
			currentAST.advanceChildToEnd();
			break;
		}
		case ALL:
		{
			RefDNode tmp29_AST = RefDNode(antlr::nullAST);
			RefDNode tmp29_AST_in = RefDNode(antlr::nullAST);
			tmp29_AST = astFactory->create(antlr::RefAST(_t));
			tmp29_AST_in = _t;
			match(antlr::RefAST(_t),ALL);
			_t = _t->getNextSibling();
			{
			if (_t == RefDNode(antlr::nullAST) )
				_t = ASTNULL;
			switch ( _t->getType()) {
			case 3:
			{
				arrayindex_AST = RefDNode(currentAST.root);
				
				//c1 = comp.ConstantIndex( #e1); 
				if( c1 != NULL)
				{
				ixList->push_back( new CArrayIndexORange( c1));
				}
				else
				{
				arrayindex_AST = e1_AST;
				ixList->push_back( new ArrayIndexORange());
				}
				
				currentAST.root = arrayindex_AST;
				if ( arrayindex_AST!=RefDNode(antlr::nullAST) &&
					arrayindex_AST->getFirstChild() != RefDNode(antlr::nullAST) )
					  currentAST.child = arrayindex_AST->getFirstChild();
				else
					currentAST.child = arrayindex_AST;
				currentAST.advanceChildToEnd();
				break;
			}
			case ASSIGN:
			case ARRAYDEF:
			case ARRAYDEF_CONST:
			case ARRAYDEF_GENERALIZED_INDGEN:
			case ARRAYEXPR:
			case ARRAYEXPR_FCALL:
			case ARRAYEXPR_MFCALL:
			case CONSTANT:
			case DEREF:
			case EXPR:
			case FCALL:
			case GDLNULL:
			case MFCALL:
			case MFCALL_PARENT:
			case NSTRUC_REF:
			case POSTDEC:
			case POSTINC:
			case STRUC:
			case SYSVAR:
			case UMINUS:
			case VAR:
			case AND_OP:
			case EQ_OP:
			case GE_OP:
			case GT_OP:
			case LE_OP:
			case LT_OP:
			case MOD_OP:
			case NE_OP:
			case NOT_OP:
			case OR_OP:
			case XOR_OP:
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
			case SLASH:
			case ASTERIX:
			case DOT:
			case POW:
			case MATRIX_OP1:
			case MATRIX_OP2:
			case PLUS:
			case MINUS:
			case LTMARK:
			case GTMARK:
			case LOG_NEG:
			case LOG_AND:
			case LOG_OR:
			case QUESTION:
			{
				e2 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
				expr(_t);
				_t = _retTree;
				e2_AST = returnAST;
				
				// in ConstantIndex the cData of all nodes is stolen
				// (in case it succeeds)
				// therefore we build here the new constant node with
				// proper cData. This is crucial because if e. g. only e1 but not e2
				// is constant, #e1 is put to the output tree (with stolen cData) -> crash
				c2 = comp.ConstantIndex( e2_AST);
				if( c2 != NULL)
				{    
				int e2Line = e2_AST->getLine();
				e2_AST = astFactory->create(CONSTANT,"CONST_IX");
				e2_AST->ResetCData( c2);
				e2_AST->SetLine( e2Line);
				}
				
				arrayindex_AST = RefDNode(currentAST.root);
				
				//c1 = comp.ConstantIndex( #e1); 
				//c2 = comp.ConstantIndex( #e2); 
				if( c1 != NULL && c2 != NULL)
				{
				ixList->push_back( new 
				CArrayIndexORangeS( c1, c2));
				}
				else
				{
				//delete c1;
				//delete c2;
				arrayindex_AST = RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(NULL))->add(antlr::RefAST(e1_AST))->add(antlr::RefAST(e2_AST))));
				ixList->push_back( new 
				ArrayIndexORangeS());
				}
				
				currentAST.root = arrayindex_AST;
				if ( arrayindex_AST!=RefDNode(antlr::nullAST) &&
					arrayindex_AST->getFirstChild() != RefDNode(antlr::nullAST) )
					  currentAST.child = arrayindex_AST->getFirstChild();
				else
					currentAST.child = arrayindex_AST;
				currentAST.advanceChildToEnd();
				break;
			}
			default:
			{
				throw antlr::NoViableAltException(antlr::RefAST(_t));
			}
			}
			}
			break;
		}
		case ASSIGN:
		case ARRAYDEF:
		case ARRAYDEF_CONST:
		case ARRAYDEF_GENERALIZED_INDGEN:
		case ARRAYEXPR:
		case ARRAYEXPR_FCALL:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case GDLNULL:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case UMINUS:
		case VAR:
		case AND_OP:
		case EQ_OP:
		case GE_OP:
		case GT_OP:
		case LE_OP:
		case LT_OP:
		case MOD_OP:
		case NE_OP:
		case NOT_OP:
		case OR_OP:
		case XOR_OP:
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
		case SLASH:
		case ASTERIX:
		case DOT:
		case POW:
		case MATRIX_OP1:
		case MATRIX_OP2:
		case PLUS:
		case MINUS:
		case LTMARK:
		case GTMARK:
		case LOG_NEG:
		case LOG_AND:
		case LOG_OR:
		case QUESTION:
		{
			e3 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
			expr(_t);
			_t = _retTree;
			e3_AST = returnAST;
			
			// see above (#e2)
			c3 = comp.ConstantIndex( e3_AST);
			if( c3 != NULL)
			{    
			int e3Line = e3_AST->getLine();
			e3_AST = astFactory->create(CONSTANT,"CONST_IX");
			e3_AST->ResetCData( c3);
			e3_AST->SetLine( e3Line);
			}
			
			{
			if (_t == RefDNode(antlr::nullAST) )
				_t = ASTNULL;
			switch ( _t->getType()) {
			case 3:
			{
				arrayindex_AST = RefDNode(currentAST.root);
				
				//c1 = comp.ConstantIndex( #e1); 
				//c3 = comp.ConstantIndex( #e3); 
				if( c1 != NULL && c3 != NULL)
				{
				ixList->push_back( new 
				CArrayIndexRange( c1, c3));
				}
				else
				{
				//delete c1;
				//delete c3;
				arrayindex_AST = RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(NULL))->add(antlr::RefAST(e1_AST))->add(antlr::RefAST(e3_AST))));
				ixList->push_back( new ArrayIndexRange());
				}
				
				currentAST.root = arrayindex_AST;
				if ( arrayindex_AST!=RefDNode(antlr::nullAST) &&
					arrayindex_AST->getFirstChild() != RefDNode(antlr::nullAST) )
					  currentAST.child = arrayindex_AST->getFirstChild();
				else
					currentAST.child = arrayindex_AST;
				currentAST.advanceChildToEnd();
				break;
			}
			case ASSIGN:
			case ARRAYDEF:
			case ARRAYDEF_CONST:
			case ARRAYDEF_GENERALIZED_INDGEN:
			case ARRAYEXPR:
			case ARRAYEXPR_FCALL:
			case ARRAYEXPR_MFCALL:
			case CONSTANT:
			case DEREF:
			case EXPR:
			case FCALL:
			case GDLNULL:
			case MFCALL:
			case MFCALL_PARENT:
			case NSTRUC_REF:
			case POSTDEC:
			case POSTINC:
			case STRUC:
			case SYSVAR:
			case UMINUS:
			case VAR:
			case AND_OP:
			case EQ_OP:
			case GE_OP:
			case GT_OP:
			case LE_OP:
			case LT_OP:
			case MOD_OP:
			case NE_OP:
			case NOT_OP:
			case OR_OP:
			case XOR_OP:
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
			case SLASH:
			case ASTERIX:
			case DOT:
			case POW:
			case MATRIX_OP1:
			case MATRIX_OP2:
			case PLUS:
			case MINUS:
			case LTMARK:
			case GTMARK:
			case LOG_NEG:
			case LOG_AND:
			case LOG_OR:
			case QUESTION:
			{
				e4 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
				expr(_t);
				_t = _retTree;
				e4_AST = returnAST;
				
				// see above (#e2)
				c4 = comp.ConstantIndex( e4_AST);
				if( c4 != NULL)
				{    
				int e4Line = e4_AST->getLine();
				e4_AST = astFactory->create(CONSTANT,"CONST_IX");
				e4_AST->ResetCData( c4);
				e4_AST->SetLine( e4Line);
				}
				
				arrayindex_AST = RefDNode(currentAST.root);
				
				//c1 = comp.ConstantIndex( #e1); 
				//c3 = comp.ConstantIndex( #e3); 
				//c4 = comp.ConstantIndex( #e4); 
				if( c1 != NULL && c3 != NULL && c4 != NULL)
				{
				ixList->push_back( new CArrayIndexRangeS( c1, c3, c4));
				}
				else
				{
				//delete c1;
				//delete c3;
				//delete c4;
				arrayindex_AST = RefDNode(astFactory->make((new antlr::ASTArray(4))->add(antlr::RefAST(NULL))->add(antlr::RefAST(e1_AST))->add(antlr::RefAST(e3_AST))->add(antlr::RefAST(e4_AST))));
				ixList->push_back( new ArrayIndexRangeS());
				}
				
				currentAST.root = arrayindex_AST;
				if ( arrayindex_AST!=RefDNode(antlr::nullAST) &&
					arrayindex_AST->getFirstChild() != RefDNode(antlr::nullAST) )
					  currentAST.child = arrayindex_AST->getFirstChild();
				else
					currentAST.child = arrayindex_AST;
				currentAST.advanceChildToEnd();
				break;
			}
			default:
			{
				throw antlr::NoViableAltException(antlr::RefAST(_t));
			}
			}
			}
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	currentAST = __currentAST112;
	_t = __t112;
	_t = _t->getNextSibling();
	}
	returnAST = arrayindex_AST;
	_retTree = _t;
}

void GDLTreeParser::arrayindex_list(RefDNode _t) {
	RefDNode arrayindex_list_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode arrayindex_list_AST = RefDNode(antlr::nullAST);
	
	ArrayIndexVectorT* ixList = new ArrayIndexVectorT();
	PtrGuard< ArrayIndexVectorT> guard( ixList);
	
	
	{ // ( ... )+
	int _cnt120=0;
	for (;;) {
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == ARRAYIX)) {
			arrayindex(_t, ixList);
			_t = _retTree;
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		else {
			if ( _cnt120>=1 ) { goto _loop120; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt120++;
	}
	_loop120:;
	}  // ( ... )+
	arrayindex_list_AST = RefDNode(currentAST.root);
	
	arrayindex_list_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(ARRAYIX,"[...]")))->add(antlr::RefAST(arrayindex_list_AST))));
	
	//            ixList->Freeze(); // do all initial one-time settings
	//             if( ixList->NDim() == 1)
	//                 #arrayindex_list = #([ARRAYIX1,"[ix]"], arrayindex_list);
	//             else
	//                 {
	//                     #arrayindex_list = #([ARRAYIX,"[...]"], arrayindex_list);
	//                     ixList->Freeze(); // do all initial one-time settings
	//                 }
	
	guard.Release();
	ArrayIndexListT* arrayIndexNoAssoc;
	ArrayIndexListT* arrayIndex;
	MakeArrayIndex( ixList, &arrayIndex, &arrayIndexNoAssoc);
	delete ixList;
	arrayindex_list_AST->SetArrayIndexList( arrayIndex, arrayIndexNoAssoc);
	
	currentAST.root = arrayindex_list_AST;
	if ( arrayindex_list_AST!=RefDNode(antlr::nullAST) &&
		arrayindex_list_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = arrayindex_list_AST->getFirstChild();
	else
		currentAST.child = arrayindex_list_AST;
	currentAST.advanceChildToEnd();
	arrayindex_list_AST = RefDNode(currentAST.root);
	returnAST = arrayindex_list_AST;
	_retTree = _t;
}

void GDLTreeParser::lassign_expr(RefDNode _t) {
	RefDNode lassign_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode lassign_expr_AST = RefDNode(antlr::nullAST);
	RefDNode ex_AST = RefDNode(antlr::nullAST);
	RefDNode ex = RefDNode(antlr::nullAST);
	
	ex = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	ex_AST = returnAST;
	lassign_expr_AST = RefDNode(currentAST.root);
	
	// remove last pair of braces
				if( ex_AST->getType()==EXPR)
	{
	//                 int cT = #ex->getFirstChild()->getType();
	//                 if( cT != FCALL && 
	//                     cT != MFCALL && 
	//                     cT != MFCALL_PARENT &&
	//                     cT != FCALL_LIB && 
	//                     cT != MFCALL_LIB && 
	//                     cT != MFCALL_PARENT_LIB)
	ex_AST=ex_AST->getFirstChild();
	}
	
	if( ex_AST->getType()==ASSIGN)
	throw GDLException(	_t, "Assign expression is not allowed as "
	"l-expression in assignment");
	
	lassign_expr_AST= ex_AST; //#( NULL, ex);
			
	currentAST.root = lassign_expr_AST;
	if ( lassign_expr_AST!=RefDNode(antlr::nullAST) &&
		lassign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = lassign_expr_AST->getFirstChild();
	else
		currentAST.child = lassign_expr_AST;
	currentAST.advanceChildToEnd();
	returnAST = lassign_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::assign_expr(RefDNode _t) {
	RefDNode assign_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode assign_expr_AST = RefDNode(antlr::nullAST);
	RefDNode a = RefDNode(antlr::nullAST);
	RefDNode a_AST = RefDNode(antlr::nullAST);
	RefDNode l_AST = RefDNode(antlr::nullAST);
	RefDNode l = RefDNode(antlr::nullAST);
	RefDNode r_AST = RefDNode(antlr::nullAST);
	RefDNode r = RefDNode(antlr::nullAST);
	
	RefDNode __t123 = _t;
	a = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode a_AST_in = RefDNode(antlr::nullAST);
	a_AST = astFactory->create(antlr::RefAST(a));
	antlr::ASTPair __currentAST123 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),ASSIGN);
	_t = _t->getFirstChild();
	l = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	lassign_expr(_t);
	_t = _retTree;
	l_AST = returnAST;
	r = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	r_AST = returnAST;
	currentAST = __currentAST123;
	_t = __t123;
	_t = _t->getNextSibling();
	assign_expr_AST = RefDNode(currentAST.root);
	
	if( !SelfAssignment( l_AST, r_AST))
	{
	
	AssignReplace( l_AST, a_AST);
	
	// int lT = #l->getType();
	// if( lT == FCALL || lT == MFCALL || lT == MFCALL_PARENT ||
	// lT == FCALL_LIB || lT == MFCALL_LIB || lT == MFCALL_PARENT_LIB ||
	// lT == DEREF || lT == VAR || lT == VARPTR)
	// #a->setType( ASSIGN_REPLACE);
	assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(a_AST))->add(antlr::RefAST(r_AST))->add(antlr::RefAST(l_AST))));  
	}
	else
	{
	assign_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(1))->add(antlr::RefAST(l_AST))));
	}
	
	currentAST.root = assign_expr_AST;
	if ( assign_expr_AST!=RefDNode(antlr::nullAST) &&
		assign_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = assign_expr_AST->getFirstChild();
	else
		currentAST.child = assign_expr_AST;
	currentAST.advanceChildToEnd();
	returnAST = assign_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::sysvar(RefDNode _t) {
	RefDNode sysvar_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode sysvar_AST = RefDNode(antlr::nullAST);
	RefDNode s = RefDNode(antlr::nullAST);
	RefDNode s_AST = RefDNode(antlr::nullAST);
	RefDNode i = RefDNode(antlr::nullAST);
	RefDNode i_AST = RefDNode(antlr::nullAST);
	
	RefDNode __t148 = _t;
	s = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode s_AST_in = RefDNode(antlr::nullAST);
	s_AST = astFactory->create(antlr::RefAST(s));
	antlr::ASTPair __currentAST148 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),SYSVAR);
	_t = _t->getFirstChild();
	i = _t;
	RefDNode i_AST_in = RefDNode(antlr::nullAST);
	i_AST = astFactory->create(antlr::RefAST(i));
	match(antlr::RefAST(_t),SYSVARNAME);
	_t = _t->getNextSibling();
	currentAST = __currentAST148;
	_t = __t148;
	_t = _t->getNextSibling();
	sysvar_AST = RefDNode(currentAST.root);
	
	std::string sysVarName = i->getText();
	// here we create the real sysvar node      
		  sysvar_AST=astFactory->create(SYSVAR, sysVarName.substr(1));
	sysvar_AST->SetLine(s->getLine());
	//	  #sysvar=#[SYSVAR,i->getText()];
		  comp.SysVar(sysvar_AST); // sets var to NULL
		
	currentAST.root = sysvar_AST;
	if ( sysvar_AST!=RefDNode(antlr::nullAST) &&
		sysvar_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = sysvar_AST->getFirstChild();
	else
		currentAST.child = sysvar_AST;
	currentAST.advanceChildToEnd();
	returnAST = sysvar_AST;
	_retTree = _t;
}

void GDLTreeParser::var(RefDNode _t) {
	RefDNode var_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode var_AST = RefDNode(antlr::nullAST);
	RefDNode v = RefDNode(antlr::nullAST);
	RefDNode v_AST = RefDNode(antlr::nullAST);
	RefDNode i = RefDNode(antlr::nullAST);
	RefDNode i_AST = RefDNode(antlr::nullAST);
	
	RefDNode __t150 = _t;
	v = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode v_AST_in = RefDNode(antlr::nullAST);
	v_AST = astFactory->create(antlr::RefAST(v));
	antlr::ASTPair __currentAST150 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),VAR);
	_t = _t->getFirstChild();
	i = _t;
	RefDNode i_AST_in = RefDNode(antlr::nullAST);
	i_AST = astFactory->create(antlr::RefAST(i));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	currentAST = __currentAST150;
	_t = __t150;
	_t = _t->getNextSibling();
	var_AST = RefDNode(currentAST.root);
	
		  var_AST=astFactory->create(VAR,i->getText());
	var_AST->SetLine(v->getLine());
	//	  #var=#[VAR,i->getText()];
		  comp.Var(var_AST);	
		
	currentAST.root = var_AST;
	if ( var_AST!=RefDNode(antlr::nullAST) &&
		var_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = var_AST->getFirstChild();
	else
		currentAST.child = var_AST;
	currentAST.advanceChildToEnd();
	returnAST = var_AST;
	_retTree = _t;
}

void GDLTreeParser::arrayindex_list_to_parameter_list(RefDNode _t,
	bool varNum
) {
	RefDNode arrayindex_list_to_parameter_list_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode arrayindex_list_to_parameter_list_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode e = RefDNode(antlr::nullAST);
	
	int nPar = 0;
	
	
	{ // ( ... )+
	int _cnt154=0;
	for (;;) {
		if (_t == RefDNode(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == ARRAYIX)) {
			RefDNode __t153 = _t;
			RefDNode tmp30_AST = RefDNode(antlr::nullAST);
			RefDNode tmp30_AST_in = RefDNode(antlr::nullAST);
			tmp30_AST = astFactory->create(antlr::RefAST(_t));
			tmp30_AST_in = _t;
			antlr::ASTPair __currentAST153 = currentAST;
			currentAST.root = currentAST.child;
			currentAST.child = RefDNode(antlr::nullAST);
			match(antlr::RefAST(_t),ARRAYIX);
			_t = _t->getFirstChild();
			e = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
			pos_parameter(_t, varNum);
			_t = _retTree;
			e_AST = returnAST;
			currentAST = __currentAST153;
			_t = __t153;
			_t = _t->getNextSibling();
			arrayindex_list_to_parameter_list_AST = RefDNode(currentAST.root);
			
			arrayindex_list_to_parameter_list_AST=
			RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(NULL))->add(antlr::RefAST(arrayindex_list_to_parameter_list_AST))->add(antlr::RefAST(e_AST))));
			++nPar;
			
			currentAST.root = arrayindex_list_to_parameter_list_AST;
			if ( arrayindex_list_to_parameter_list_AST!=RefDNode(antlr::nullAST) &&
				arrayindex_list_to_parameter_list_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = arrayindex_list_to_parameter_list_AST->getFirstChild();
			else
				currentAST.child = arrayindex_list_to_parameter_list_AST;
			currentAST.advanceChildToEnd();
		}
		else {
			if ( _cnt154>=1 ) { goto _loop154; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt154++;
	}
	_loop154:;
	}  // ( ... )+
	
	arrayindex_list_to_parameter_list_AST->SetNParam( nPar);
	
	returnAST = arrayindex_list_to_parameter_list_AST;
	_retTree = _t;
}

void GDLTreeParser::arrayexpr_fn(RefDNode _t) {
	RefDNode arrayexpr_fn_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode arrayexpr_fn_AST = RefDNode(antlr::nullAST);
	RefDNode aIn = RefDNode(antlr::nullAST);
	RefDNode aIn_AST = RefDNode(antlr::nullAST);
	RefDNode va = RefDNode(antlr::nullAST);
	RefDNode va_AST = RefDNode(antlr::nullAST);
	RefDNode id = RefDNode(antlr::nullAST);
	RefDNode id_AST = RefDNode(antlr::nullAST);
	RefDNode el_AST = RefDNode(antlr::nullAST);
	RefDNode el = RefDNode(antlr::nullAST);
	
	std::string id_text;
	bool isVar;
	RefDNode mark, va2, vaAlt, fn, arrayindex_listAST;
	
	
	RefDNode __t156 = _t;
	aIn = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode aIn_AST_in = RefDNode(antlr::nullAST);
	aIn_AST = astFactory->create(antlr::RefAST(aIn));
	antlr::ASTPair __currentAST156 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),ARRAYEXPR_FCALL);
	_t = _t->getFirstChild();
	RefDNode __t157 = _t;
	va = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode va_AST_in = RefDNode(antlr::nullAST);
	va_AST = astFactory->create(antlr::RefAST(va));
	antlr::ASTPair __currentAST157 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),VAR);
	_t = _t->getFirstChild();
	id = _t;
	RefDNode id_AST_in = RefDNode(antlr::nullAST);
	id_AST = astFactory->create(antlr::RefAST(id));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	currentAST = __currentAST157;
	_t = __t157;
	_t = _t->getNextSibling();
	
	mark = _t;
	
	id_text = id_AST->getText(); 
	
	// IsVar is not needed, we must emit an ARRAYEXPR_FCALL even if the variable is known
	// (rule: Accessible functions always override variables
	//isVar = comp.IsVar( id_text); 
	// isVar == true -> VAR for sure 
	// (== false: maybe VAR nevertheless)
	
	int libIx = -1;    
	//                if( id_text != "LIST" && id_text != "HASH")    
	libIx = LibFunIx(id_text);
	
	
	{
	el = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	arrayindex_list_to_parameter_list(_t, libIx != -1 && libFunList[ libIx]->NPar() == -1);
	_t = _retTree;
	el_AST = returnAST;
	}
	arrayexpr_fn_AST = RefDNode(currentAST.root);
	
	// first search library functions
	if( libIx != -1)
	{
	int nParam = 0;
	if( el_AST != RefDNode(antlr::nullAST))
	nParam = el_AST->GetNParam();
	
	int libParam = libFunList[libIx]->NPar();
	int libParamMin = libFunList[libIx]->NParMin();
	if( libParam != -1 && nParam > libParam)
	throw GDLException(	aIn, libFunList[libIx]->Name() + ": Too many arguments.");
	if( libParam != -1 && nParam < libParamMin)
	throw GDLException(	aIn, libFunList[libIx]->Name() + ": Too few arguments.");
	
	id_AST->SetLibFun( libFunList[libIx]);
	if( libFunList[ libIx]->RetNew())
	{
	if( libFunList[ libIx]->Name() == "N_ELEMENTS")
	id_AST->setType( FCALL_LIB_N_ELEMENTS);
	else if( libFunList[ libIx]->DirectCall())
	id_AST->setType( FCALL_LIB_DIRECT);
	else
	id_AST->setType( FCALL_LIB_RETNEW);
	arrayexpr_fn_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(id_AST))->add(antlr::RefAST(el_AST))));
	//                              #([/*FCALL_LIB_RETNEW,"fcall_lib_retnew"],*/ id, el);
	}
	else
	{
	id_AST->setType( FCALL_LIB);
	arrayexpr_fn_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(id_AST))->add(antlr::RefAST(el_AST))));
	//                              #(/*[FCALL_LIB,"fcall_lib"],*/ id, el);
	}
	}
	// then search user defined functions
	else
	{
	
	int funIx=FunIx( id_text);
	
	// we use #id for the FCALL part
	id_AST->setType( FCALL);
	id_AST->SetFunIx( funIx);
	
	if( funIx != -1) // found -> FCALL
	{
	arrayexpr_fn_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(id_AST))->add(antlr::RefAST(el_AST)))); 
	// #(/*[FCALL,"fcall"],*/ id, el);
	}
	else // function not found -> still ambiguous
	{
	// _t = mark; // rewind to parse again 
	arrayindex_list( mark);
	//_t = _retTree;
	arrayindex_listAST = returnAST;
	
	
	va2=astFactory->create( VAR, id_text);
	// #va=#[VAR,id->getText()];
	comp.Var( va2); // we declare the variable here!
	// if IsVar() still would be used this would lead to surprising behavior: 
	// e. g.: function_call(42) & function_call(43)  
	// The first (42) would be an ARRAYEXPR_FCALL the 2nd (43) an ARRAYEXPR
	// if then at runtime function "function_call" is known,
	// it will be called only at the first appearance of the call.
	// that's why we cannot allow unambiguous VAR here 
	
	vaAlt = RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ARRAYEXPR,"arrayexpr")))->add(antlr::RefAST(va2))->add(antlr::RefAST(arrayindex_listAST))));
	fn = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(id_AST))->add(antlr::RefAST(el_AST))));    
	
	arrayexpr_fn_AST = RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(aIn_AST))->add(antlr::RefAST(vaAlt))->add(antlr::RefAST(fn)))); 
	}
	}
	
	//                 // not valid s. a. (kept for reference): unambiguous VAR
	//                 {   // variable -> arrayexpr                    
	//                     // make var
	//                     #va=astFactory->create(VAR,#id->getText());
	// //                    #va=#[VAR,id->getText()];
	//                     comp.Var(#va);
	//                     #arrayexpr_fn=
	//                     #([ARRAYEXPR,"arrayexpr"], va, al);
	//                 }
	
	currentAST.root = arrayexpr_fn_AST;
	if ( arrayexpr_fn_AST!=RefDNode(antlr::nullAST) &&
		arrayexpr_fn_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = arrayexpr_fn_AST->getFirstChild();
	else
		currentAST.child = arrayexpr_fn_AST;
	currentAST.advanceChildToEnd();
	currentAST = __currentAST156;
	_t = __t156;
	_t = _t->getNextSibling();
	returnAST = arrayexpr_fn_AST;
	_retTree = _t;
}

void GDLTreeParser::arrayexpr_mfcall(RefDNode _t) {
	RefDNode arrayexpr_mfcall_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode arrayexpr_mfcall_AST = RefDNode(antlr::nullAST);
	RefDNode a = RefDNode(antlr::nullAST);
	RefDNode a_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode e = RefDNode(antlr::nullAST);
	RefDNode i = RefDNode(antlr::nullAST);
	RefDNode i_AST = RefDNode(antlr::nullAST);
	RefDNode e2_AST = RefDNode(antlr::nullAST);
	RefDNode e2 = RefDNode(antlr::nullAST);
	RefDNode i2 = RefDNode(antlr::nullAST);
	RefDNode i2_AST = RefDNode(antlr::nullAST);
	RefDNode a2_AST = RefDNode(antlr::nullAST);
	RefDNode a2 = RefDNode(antlr::nullAST);
	
	RefDNode __t160 = _t;
	a = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode a_AST_in = RefDNode(antlr::nullAST);
	a_AST = astFactory->create(antlr::RefAST(a));
	antlr::ASTPair __currentAST160 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),ARRAYEXPR_MFCALL);
	_t = _t->getFirstChild();
	
	RefDNode mark = _t; // mark
	
	e = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	e_AST = returnAST;
	i = _t;
	RefDNode i_AST_in = RefDNode(antlr::nullAST);
	i_AST = astFactory->create(antlr::RefAST(i));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	
	bool success = true;
	RefDNode first;
		            try {
	arrayindex_list(_t);
	
	RefDNode al = returnAST;
	RefDNode ae = RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ARRAYEXPR,"arrayexpr")))->add(antlr::RefAST(i_AST))->add(antlr::RefAST(al))));
	
	if( e->getType() == DOT)
	{
	int nDot = e_AST->GetNDot();
	e_AST->SetNDot( ++nDot);
	
	e_AST->addChild( ae);
	
	first = e_AST;
	}
	else
	{
	RefDNode dot = RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(DOT,".")))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(ae))));
	dot->SetNDot( 1);
	
	first = dot;
	}
	}
	catch( GDLException& ex)
	{
	Message( "Ambiguity resolved: member function call "
	"due to invalid array index.");
	
	success = false;
	
	a_AST->setType( MFCALL);
	a_AST->setText( "mfcall");
	}
	
	_t = mark; // rewind to parse again 
	
	e2 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	e2_AST = returnAST;
	i2 = _t;
	RefDNode i2_AST_in = RefDNode(antlr::nullAST);
	i2_AST = astFactory->create(antlr::RefAST(i2));
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	a2 = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	arrayindex_list_to_parameter_list(_t, false);
	_t = _retTree;
	a2_AST = returnAST;
	arrayexpr_mfcall_AST = RefDNode(currentAST.root);
	
	if( success)
	arrayexpr_mfcall_AST = RefDNode(astFactory->make((new antlr::ASTArray(5))->add(antlr::RefAST(a_AST))->add(antlr::RefAST(first))->add(antlr::RefAST(e2_AST))->add(antlr::RefAST(i2_AST))->add(antlr::RefAST(a2_AST))));
	else
	arrayexpr_mfcall_AST = RefDNode(astFactory->make((new antlr::ASTArray(4))->add(antlr::RefAST(a_AST))->add(antlr::RefAST(e2_AST))->add(antlr::RefAST(i2_AST))->add(antlr::RefAST(a2_AST))));
	
	currentAST.root = arrayexpr_mfcall_AST;
	if ( arrayexpr_mfcall_AST!=RefDNode(antlr::nullAST) &&
		arrayexpr_mfcall_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = arrayexpr_mfcall_AST->getFirstChild();
	else
		currentAST.child = arrayexpr_mfcall_AST;
	currentAST.advanceChildToEnd();
	currentAST = __currentAST160;
	_t = __t160;
	_t = _t->getNextSibling();
	returnAST = arrayexpr_mfcall_AST;
	_retTree = _t;
}

void GDLTreeParser::primary_expr(RefDNode _t) {
	RefDNode primary_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode primary_expr_AST = RefDNode(antlr::nullAST);
	RefDNode f = RefDNode(antlr::nullAST);
	RefDNode f_AST = RefDNode(antlr::nullAST);
	RefDNode id = RefDNode(antlr::nullAST);
	RefDNode id_AST = RefDNode(antlr::nullAST);
	RefDNode p_AST = RefDNode(antlr::nullAST);
	RefDNode p = RefDNode(antlr::nullAST);
	RefDNode g = RefDNode(antlr::nullAST);
	RefDNode g_AST = RefDNode(antlr::nullAST);
	
	int dummy;
	RefDNode mark;
	
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	{
		assign_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		primary_expr_AST = RefDNode(currentAST.root);
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
		comp_assign_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case ARRAYEXPR_MFCALL:
	{
		arrayexpr_mfcall(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case MFCALL:
	{
		RefDNode __t162 = _t;
		RefDNode tmp31_AST = RefDNode(antlr::nullAST);
		RefDNode tmp31_AST_in = RefDNode(antlr::nullAST);
		tmp31_AST = astFactory->create(antlr::RefAST(_t));
		tmp31_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp31_AST));
		antlr::ASTPair __currentAST162 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MFCALL);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		RefDNode tmp32_AST = RefDNode(antlr::nullAST);
		RefDNode tmp32_AST_in = RefDNode(antlr::nullAST);
		tmp32_AST = astFactory->create(antlr::RefAST(_t));
		tmp32_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp32_AST));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		parameter_def(_t, false);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST162;
		_t = __t162;
		_t = _t->getNextSibling();
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case MFCALL_PARENT:
	{
		RefDNode __t163 = _t;
		RefDNode tmp33_AST = RefDNode(antlr::nullAST);
		RefDNode tmp33_AST_in = RefDNode(antlr::nullAST);
		tmp33_AST = astFactory->create(antlr::RefAST(_t));
		tmp33_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp33_AST));
		antlr::ASTPair __currentAST163 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MFCALL_PARENT);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		RefDNode tmp34_AST = RefDNode(antlr::nullAST);
		RefDNode tmp34_AST_in = RefDNode(antlr::nullAST);
		tmp34_AST = astFactory->create(antlr::RefAST(_t));
		tmp34_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp34_AST));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		RefDNode tmp35_AST = RefDNode(antlr::nullAST);
		RefDNode tmp35_AST_in = RefDNode(antlr::nullAST);
		tmp35_AST = astFactory->create(antlr::RefAST(_t));
		tmp35_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp35_AST));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		parameter_def(_t, false);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST163;
		_t = __t163;
		_t = _t->getNextSibling();
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case FCALL:
	{
		RefDNode __t164 = _t;
		f = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
		RefDNode f_AST_in = RefDNode(antlr::nullAST);
		f_AST = astFactory->create(antlr::RefAST(f));
		astFactory->addASTChild(currentAST, antlr::RefAST(f_AST));
		antlr::ASTPair __currentAST164 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),FCALL);
		_t = _t->getFirstChild();
		id = _t;
		RefDNode id_AST_in = RefDNode(antlr::nullAST);
		id_AST = astFactory->create(antlr::RefAST(id));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		// first search library functions
		int i=LibFunIx(id->getText());
		
		p = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
		parameter_def(_t,  i != -1 && libFunList[ i]->NPar() == -1);
		_t = _retTree;
		p_AST = returnAST;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		
		if( i != -1)
		{
		// N_ELEMENTS must handle exceptions during parameter evaluation
		//                     if( StrUpCase( #id->getText()) == "N_ELEMENTS")
		//                     {
		//                     #f->setType(FCALL_LIB_N_ELEMENTS);
		//                     #f->setText(#id->getText());
		//                     #f->SetLibFun( libFunList[i]);
		//                         //                    #id->SetFunIx(i);
		//                     }
		//                     else
		{
		int nParam = 0;
		if( p_AST != RefDNode(antlr::nullAST))
		nParam = p_AST->GetNParam();
		
		int libParam = libFunList[i]->NPar();
		int libParamMin = libFunList[i]->NParMin();
		if( libParam != -1 && nParam > libParam)
		throw GDLException(	f, libFunList[i]->Name() + ": Too many arguments.");
		if( libParam != -1 && nParam < libParamMin)
		throw GDLException(	f, libFunList[i]->Name() + ": Too few arguments.");
		if( libFunList[ i]->RetNew())
		{
		if( libFunList[ i]->Name() == "N_ELEMENTS")
		f_AST->setType( FCALL_LIB_N_ELEMENTS);
		else if( libFunList[ i]->DirectCall())
		f_AST->setType( FCALL_LIB_DIRECT);
		else
		f_AST->setType(FCALL_LIB_RETNEW);
		f_AST->setText(id_AST->getText());
		f_AST->SetLibFun( libFunList[i]);
		//                    #id->SetFunIx(i);
		}
		else
		{
		f_AST->setType(FCALL_LIB);
		f_AST->setText(id_AST->getText());
		f_AST->SetLibFun( libFunList[i]);
		//                    #id->SetFunIx(i);
		}
		}
		}
		else
		{
		// then search user defined functions
		f_AST->setType(FCALL);
		f_AST->setText(id_AST->getText());
		i=FunIx(id_AST->getText());
		f_AST->SetFunIx(i);
		}
		
		currentAST = __currentAST164;
		_t = __t164;
		_t = _t->getNextSibling();
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case ARRAYEXPR_FCALL:
	{
		arrayexpr_fn(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case CONSTANT:
	{
		RefDNode tmp36_AST = RefDNode(antlr::nullAST);
		RefDNode tmp36_AST_in = RefDNode(antlr::nullAST);
		tmp36_AST = astFactory->create(antlr::RefAST(_t));
		tmp36_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp36_AST));
		match(antlr::RefAST(_t),CONSTANT);
		_t = _t->getNextSibling();
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case ARRAYDEF:
	case ARRAYDEF_CONST:
	case ARRAYDEF_GENERALIZED_INDGEN:
	{
		dummy=array_def(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case NSTRUC_REF:
	case STRUC:
	{
		struct_def(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case GDLNULL:
	{
		g = _t;
		RefDNode g_AST_in = RefDNode(antlr::nullAST);
		g_AST = astFactory->create(antlr::RefAST(g));
		astFactory->addASTChild(currentAST, antlr::RefAST(g_AST));
		match(antlr::RefAST(_t),GDLNULL);
		_t = _t->getNextSibling();
		
		g_AST->setType(SYSVAR);
		g_AST->setText("NULL");
		comp.SysVar(g_AST); // sets var to NULL
		
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = primary_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::op_expr(RefDNode _t) {
	RefDNode op_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode op_expr_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case QUESTION:
	{
		RefDNode __t166 = _t;
		RefDNode tmp37_AST = RefDNode(antlr::nullAST);
		RefDNode tmp37_AST_in = RefDNode(antlr::nullAST);
		tmp37_AST = astFactory->create(antlr::RefAST(_t));
		tmp37_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp37_AST));
		antlr::ASTPair __currentAST166 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST166;
		_t = __t166;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case AND_OP:
	{
		RefDNode __t167 = _t;
		RefDNode tmp38_AST = RefDNode(antlr::nullAST);
		RefDNode tmp38_AST_in = RefDNode(antlr::nullAST);
		tmp38_AST = astFactory->create(antlr::RefAST(_t));
		tmp38_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp38_AST));
		antlr::ASTPair __currentAST167 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),AND_OP);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST167;
		_t = __t167;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case OR_OP:
	{
		RefDNode __t168 = _t;
		RefDNode tmp39_AST = RefDNode(antlr::nullAST);
		RefDNode tmp39_AST_in = RefDNode(antlr::nullAST);
		tmp39_AST = astFactory->create(antlr::RefAST(_t));
		tmp39_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp39_AST));
		antlr::ASTPair __currentAST168 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),OR_OP);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST168;
		_t = __t168;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case XOR_OP:
	{
		RefDNode __t169 = _t;
		RefDNode tmp40_AST = RefDNode(antlr::nullAST);
		RefDNode tmp40_AST_in = RefDNode(antlr::nullAST);
		tmp40_AST = astFactory->create(antlr::RefAST(_t));
		tmp40_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp40_AST));
		antlr::ASTPair __currentAST169 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),XOR_OP);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST169;
		_t = __t169;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case LOG_AND:
	{
		RefDNode __t170 = _t;
		RefDNode tmp41_AST = RefDNode(antlr::nullAST);
		RefDNode tmp41_AST_in = RefDNode(antlr::nullAST);
		tmp41_AST = astFactory->create(antlr::RefAST(_t));
		tmp41_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp41_AST));
		antlr::ASTPair __currentAST170 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),LOG_AND);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST170;
		_t = __t170;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case LOG_OR:
	{
		RefDNode __t171 = _t;
		RefDNode tmp42_AST = RefDNode(antlr::nullAST);
		RefDNode tmp42_AST_in = RefDNode(antlr::nullAST);
		tmp42_AST = astFactory->create(antlr::RefAST(_t));
		tmp42_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp42_AST));
		antlr::ASTPair __currentAST171 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),LOG_OR);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST171;
		_t = __t171;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case EQ_OP:
	{
		RefDNode __t172 = _t;
		RefDNode tmp43_AST = RefDNode(antlr::nullAST);
		RefDNode tmp43_AST_in = RefDNode(antlr::nullAST);
		tmp43_AST = astFactory->create(antlr::RefAST(_t));
		tmp43_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp43_AST));
		antlr::ASTPair __currentAST172 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),EQ_OP);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST172;
		_t = __t172;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case NE_OP:
	{
		RefDNode __t173 = _t;
		RefDNode tmp44_AST = RefDNode(antlr::nullAST);
		RefDNode tmp44_AST_in = RefDNode(antlr::nullAST);
		tmp44_AST = astFactory->create(antlr::RefAST(_t));
		tmp44_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp44_AST));
		antlr::ASTPair __currentAST173 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),NE_OP);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST173;
		_t = __t173;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case LE_OP:
	{
		RefDNode __t174 = _t;
		RefDNode tmp45_AST = RefDNode(antlr::nullAST);
		RefDNode tmp45_AST_in = RefDNode(antlr::nullAST);
		tmp45_AST = astFactory->create(antlr::RefAST(_t));
		tmp45_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp45_AST));
		antlr::ASTPair __currentAST174 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),LE_OP);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST174;
		_t = __t174;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case LT_OP:
	{
		RefDNode __t175 = _t;
		RefDNode tmp46_AST = RefDNode(antlr::nullAST);
		RefDNode tmp46_AST_in = RefDNode(antlr::nullAST);
		tmp46_AST = astFactory->create(antlr::RefAST(_t));
		tmp46_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp46_AST));
		antlr::ASTPair __currentAST175 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),LT_OP);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST175;
		_t = __t175;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case GE_OP:
	{
		RefDNode __t176 = _t;
		RefDNode tmp47_AST = RefDNode(antlr::nullAST);
		RefDNode tmp47_AST_in = RefDNode(antlr::nullAST);
		tmp47_AST = astFactory->create(antlr::RefAST(_t));
		tmp47_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp47_AST));
		antlr::ASTPair __currentAST176 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),GE_OP);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST176;
		_t = __t176;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case GT_OP:
	{
		RefDNode __t177 = _t;
		RefDNode tmp48_AST = RefDNode(antlr::nullAST);
		RefDNode tmp48_AST_in = RefDNode(antlr::nullAST);
		tmp48_AST = astFactory->create(antlr::RefAST(_t));
		tmp48_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp48_AST));
		antlr::ASTPair __currentAST177 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),GT_OP);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST177;
		_t = __t177;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case NOT_OP:
	{
		RefDNode __t178 = _t;
		RefDNode tmp49_AST = RefDNode(antlr::nullAST);
		RefDNode tmp49_AST_in = RefDNode(antlr::nullAST);
		tmp49_AST = astFactory->create(antlr::RefAST(_t));
		tmp49_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp49_AST));
		antlr::ASTPair __currentAST178 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),NOT_OP);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST178;
		_t = __t178;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case PLUS:
	{
		RefDNode __t179 = _t;
		RefDNode tmp50_AST = RefDNode(antlr::nullAST);
		RefDNode tmp50_AST_in = RefDNode(antlr::nullAST);
		tmp50_AST = astFactory->create(antlr::RefAST(_t));
		tmp50_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp50_AST));
		antlr::ASTPair __currentAST179 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),PLUS);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST179;
		_t = __t179;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case MINUS:
	{
		RefDNode __t180 = _t;
		RefDNode tmp51_AST = RefDNode(antlr::nullAST);
		RefDNode tmp51_AST_in = RefDNode(antlr::nullAST);
		tmp51_AST = astFactory->create(antlr::RefAST(_t));
		tmp51_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp51_AST));
		antlr::ASTPair __currentAST180 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MINUS);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST180;
		_t = __t180;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case LTMARK:
	{
		RefDNode __t181 = _t;
		RefDNode tmp52_AST = RefDNode(antlr::nullAST);
		RefDNode tmp52_AST_in = RefDNode(antlr::nullAST);
		tmp52_AST = astFactory->create(antlr::RefAST(_t));
		tmp52_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp52_AST));
		antlr::ASTPair __currentAST181 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),LTMARK);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST181;
		_t = __t181;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case GTMARK:
	{
		RefDNode __t182 = _t;
		RefDNode tmp53_AST = RefDNode(antlr::nullAST);
		RefDNode tmp53_AST_in = RefDNode(antlr::nullAST);
		tmp53_AST = astFactory->create(antlr::RefAST(_t));
		tmp53_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp53_AST));
		antlr::ASTPair __currentAST182 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),GTMARK);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST182;
		_t = __t182;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case UMINUS:
	{
		uminus(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case LOG_NEG:
	{
		RefDNode __t183 = _t;
		RefDNode tmp54_AST = RefDNode(antlr::nullAST);
		RefDNode tmp54_AST_in = RefDNode(antlr::nullAST);
		tmp54_AST = astFactory->create(antlr::RefAST(_t));
		tmp54_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp54_AST));
		antlr::ASTPair __currentAST183 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),LOG_NEG);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST183;
		_t = __t183;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case ASTERIX:
	{
		RefDNode __t184 = _t;
		RefDNode tmp55_AST = RefDNode(antlr::nullAST);
		RefDNode tmp55_AST_in = RefDNode(antlr::nullAST);
		tmp55_AST = astFactory->create(antlr::RefAST(_t));
		tmp55_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp55_AST));
		antlr::ASTPair __currentAST184 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),ASTERIX);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST184;
		_t = __t184;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case MATRIX_OP1:
	{
		RefDNode __t185 = _t;
		RefDNode tmp56_AST = RefDNode(antlr::nullAST);
		RefDNode tmp56_AST_in = RefDNode(antlr::nullAST);
		tmp56_AST = astFactory->create(antlr::RefAST(_t));
		tmp56_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp56_AST));
		antlr::ASTPair __currentAST185 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MATRIX_OP1);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST185;
		_t = __t185;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case MATRIX_OP2:
	{
		RefDNode __t186 = _t;
		RefDNode tmp57_AST = RefDNode(antlr::nullAST);
		RefDNode tmp57_AST_in = RefDNode(antlr::nullAST);
		tmp57_AST = astFactory->create(antlr::RefAST(_t));
		tmp57_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp57_AST));
		antlr::ASTPair __currentAST186 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MATRIX_OP2);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST186;
		_t = __t186;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case SLASH:
	{
		RefDNode __t187 = _t;
		RefDNode tmp58_AST = RefDNode(antlr::nullAST);
		RefDNode tmp58_AST_in = RefDNode(antlr::nullAST);
		tmp58_AST = astFactory->create(antlr::RefAST(_t));
		tmp58_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp58_AST));
		antlr::ASTPair __currentAST187 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),SLASH);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST187;
		_t = __t187;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case MOD_OP:
	{
		RefDNode __t188 = _t;
		RefDNode tmp59_AST = RefDNode(antlr::nullAST);
		RefDNode tmp59_AST_in = RefDNode(antlr::nullAST);
		tmp59_AST = astFactory->create(antlr::RefAST(_t));
		tmp59_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp59_AST));
		antlr::ASTPair __currentAST188 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),MOD_OP);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST188;
		_t = __t188;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case POW:
	{
		RefDNode __t189 = _t;
		RefDNode tmp60_AST = RefDNode(antlr::nullAST);
		RefDNode tmp60_AST_in = RefDNode(antlr::nullAST);
		tmp60_AST = astFactory->create(antlr::RefAST(_t));
		tmp60_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp60_AST));
		antlr::ASTPair __currentAST189 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),POW);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST189;
		_t = __t189;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case DEC:
	{
		RefDNode __t190 = _t;
		RefDNode tmp61_AST = RefDNode(antlr::nullAST);
		RefDNode tmp61_AST_in = RefDNode(antlr::nullAST);
		tmp61_AST = astFactory->create(antlr::RefAST(_t));
		tmp61_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp61_AST));
		antlr::ASTPair __currentAST190 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),DEC);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST190;
		_t = __t190;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case INC:
	{
		RefDNode __t191 = _t;
		RefDNode tmp62_AST = RefDNode(antlr::nullAST);
		RefDNode tmp62_AST_in = RefDNode(antlr::nullAST);
		tmp62_AST = astFactory->create(antlr::RefAST(_t));
		tmp62_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp62_AST));
		antlr::ASTPair __currentAST191 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),INC);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST191;
		_t = __t191;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case POSTDEC:
	{
		RefDNode __t192 = _t;
		RefDNode tmp63_AST = RefDNode(antlr::nullAST);
		RefDNode tmp63_AST_in = RefDNode(antlr::nullAST);
		tmp63_AST = astFactory->create(antlr::RefAST(_t));
		tmp63_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp63_AST));
		antlr::ASTPair __currentAST192 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),POSTDEC);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST192;
		_t = __t192;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case POSTINC:
	{
		RefDNode __t193 = _t;
		RefDNode tmp64_AST = RefDNode(antlr::nullAST);
		RefDNode tmp64_AST_in = RefDNode(antlr::nullAST);
		tmp64_AST = astFactory->create(antlr::RefAST(_t));
		tmp64_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp64_AST));
		antlr::ASTPair __currentAST193 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),POSTINC);
		_t = _t->getFirstChild();
		expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST193;
		_t = __t193;
		_t = _t->getNextSibling();
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case ASSIGN:
	case ARRAYDEF:
	case ARRAYDEF_CONST:
	case ARRAYDEF_GENERALIZED_INDGEN:
	case ARRAYEXPR_FCALL:
	case ARRAYEXPR_MFCALL:
	case CONSTANT:
	case FCALL:
	case GDLNULL:
	case MFCALL:
	case MFCALL_PARENT:
	case NSTRUC_REF:
	case STRUC:
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
		primary_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		op_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = op_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::uminus(RefDNode _t) {
	RefDNode uminus_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode uminus_AST = RefDNode(antlr::nullAST);
	RefDNode u = RefDNode(antlr::nullAST);
	RefDNode u_AST = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode e = RefDNode(antlr::nullAST);
	
	RefDNode __t195 = _t;
	u = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode u_AST_in = RefDNode(antlr::nullAST);
	u_AST = astFactory->create(antlr::RefAST(u));
	astFactory->addASTChild(currentAST, antlr::RefAST(u_AST));
	antlr::ASTPair __currentAST195 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),UMINUS);
	_t = _t->getFirstChild();
	e = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	e_AST = returnAST;
	astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	currentAST = __currentAST195;
	_t = __t195;
	_t = _t->getNextSibling();
	uminus_AST = RefDNode(currentAST.root);
	
	// eliminate (pre-calculate) uminus for constants
	if( e_AST->getType() == CONSTANT)
	{
	e_AST->ResetCData( e_AST->CData()->UMinus());
	e_AST->setText( "-"+e_AST->getText());
	uminus_AST = e_AST;
	}
	//             else
	//             {
	//                 #uminus = #( [UMINUS,"u-"], e);
	//             }
	
	currentAST.root = uminus_AST;
	if ( uminus_AST!=RefDNode(antlr::nullAST) &&
		uminus_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = uminus_AST->getFirstChild();
	else
		currentAST.child = uminus_AST;
	currentAST.advanceChildToEnd();
	uminus_AST = RefDNode(currentAST.root);
	returnAST = uminus_AST;
	_retTree = _t;
}

void GDLTreeParser::brace_expr(RefDNode _t) {
	RefDNode brace_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode brace_expr_AST = RefDNode(antlr::nullAST);
	RefDNode e = RefDNode(antlr::nullAST);
	RefDNode e_AST = RefDNode(antlr::nullAST);
	RefDNode ex_AST = RefDNode(antlr::nullAST);
	RefDNode ex = RefDNode(antlr::nullAST);
	
	RefDNode __t197 = _t;
	e = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	RefDNode e_AST_in = RefDNode(antlr::nullAST);
	e_AST = astFactory->create(antlr::RefAST(e));
	antlr::ASTPair __currentAST197 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),EXPR);
	_t = _t->getFirstChild();
	ex = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	ex_AST = returnAST;
	currentAST = __currentAST197;
	_t = __t197;
	_t = _t->getNextSibling();
	brace_expr_AST = RefDNode(currentAST.root);
	
	while( ex_AST->getType()==EXPR) 
	ex_AST=ex_AST->getFirstChild();
	brace_expr_AST=RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(e_AST))->add(antlr::RefAST(ex_AST))));
			
	currentAST.root = brace_expr_AST;
	if ( brace_expr_AST!=RefDNode(antlr::nullAST) &&
		brace_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = brace_expr_AST->getFirstChild();
	else
		currentAST.child = brace_expr_AST;
	currentAST.advanceChildToEnd();
	returnAST = brace_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::unbrace_expr(RefDNode _t) {
	RefDNode unbrace_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode unbrace_expr_AST = RefDNode(antlr::nullAST);
	RefDNode ex_AST = RefDNode(antlr::nullAST);
	RefDNode ex = RefDNode(antlr::nullAST);
	
	RefDNode __t199 = _t;
	RefDNode tmp65_AST = RefDNode(antlr::nullAST);
	RefDNode tmp65_AST_in = RefDNode(antlr::nullAST);
	tmp65_AST = astFactory->create(antlr::RefAST(_t));
	tmp65_AST_in = _t;
	antlr::ASTPair __currentAST199 = currentAST;
	currentAST.root = currentAST.child;
	currentAST.child = RefDNode(antlr::nullAST);
	match(antlr::RefAST(_t),EXPR);
	_t = _t->getFirstChild();
	ex = (_t == ASTNULL) ? RefDNode(antlr::nullAST) : _t;
	expr(_t);
	_t = _retTree;
	ex_AST = returnAST;
	currentAST = __currentAST199;
	_t = __t199;
	_t = _t->getNextSibling();
	unbrace_expr_AST = RefDNode(currentAST.root);
	
	while( ex_AST->getType()==EXPR) 
	ex_AST=ex_AST->getFirstChild();
	unbrace_expr_AST=ex_AST;
			
	currentAST.root = unbrace_expr_AST;
	if ( unbrace_expr_AST!=RefDNode(antlr::nullAST) &&
		unbrace_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
		  currentAST.child = unbrace_expr_AST->getFirstChild();
	else
		currentAST.child = unbrace_expr_AST;
	currentAST.advanceChildToEnd();
	returnAST = unbrace_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::indexable_expr(RefDNode _t) {
	RefDNode indexable_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode indexable_expr_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	{
		var(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		indexable_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case SYSVAR:
	{
		sysvar(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		indexable_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case EXPR:
	{
		unbrace_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		indexable_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = indexable_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::array_expr(RefDNode _t) {
	RefDNode array_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode array_expr_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	{
		RefDNode __t202 = _t;
		RefDNode tmp66_AST = RefDNode(antlr::nullAST);
		RefDNode tmp66_AST_in = RefDNode(antlr::nullAST);
		tmp66_AST = astFactory->create(antlr::RefAST(_t));
		tmp66_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp66_AST));
		antlr::ASTPair __currentAST202 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		indexable_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		arrayindex_list(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST202;
		_t = __t202;
		_t = _t->getNextSibling();
		array_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case EXPR:
	case SYSVAR:
	case VAR:
	{
		indexable_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		array_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = array_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::tag_indexable_expr(RefDNode _t) {
	RefDNode tag_indexable_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_indexable_expr_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	{
		var(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		tag_indexable_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case SYSVAR:
	{
		sysvar(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		tag_indexable_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case EXPR:
	{
		brace_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		tag_indexable_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = tag_indexable_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::tag_array_expr_1st(RefDNode _t) {
	RefDNode tag_array_expr_1st_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_array_expr_1st_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	{
		RefDNode __t205 = _t;
		RefDNode tmp67_AST = RefDNode(antlr::nullAST);
		RefDNode tmp67_AST_in = RefDNode(antlr::nullAST);
		tmp67_AST = astFactory->create(antlr::RefAST(_t));
		tmp67_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp67_AST));
		antlr::ASTPair __currentAST205 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		tag_indexable_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		arrayindex_list(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST205;
		_t = __t205;
		_t = _t->getNextSibling();
		tag_array_expr_1st_AST = RefDNode(currentAST.root);
		break;
	}
	case EXPR:
	case SYSVAR:
	case VAR:
	{
		tag_indexable_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		tag_array_expr_1st_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = tag_array_expr_1st_AST;
	_retTree = _t;
}

void GDLTreeParser::tag_expr(RefDNode _t) {
	RefDNode tag_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_expr_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case EXPR:
	{
		brace_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		tag_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case IDENTIFIER:
	{
		RefDNode tmp68_AST = RefDNode(antlr::nullAST);
		RefDNode tmp68_AST_in = RefDNode(antlr::nullAST);
		tmp68_AST = astFactory->create(antlr::RefAST(_t));
		tmp68_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp68_AST));
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		tag_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = tag_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::tag_array_expr(RefDNode _t) {
	RefDNode tag_array_expr_AST_in = (_t == RefDNode(ASTNULL)) ? RefDNode(antlr::nullAST) : _t;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_array_expr_AST = RefDNode(antlr::nullAST);
	
	if (_t == RefDNode(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	{
		RefDNode __t208 = _t;
		RefDNode tmp69_AST = RefDNode(antlr::nullAST);
		RefDNode tmp69_AST_in = RefDNode(antlr::nullAST);
		tmp69_AST = astFactory->create(antlr::RefAST(_t));
		tmp69_AST_in = _t;
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp69_AST));
		antlr::ASTPair __currentAST208 = currentAST;
		currentAST.root = currentAST.child;
		currentAST.child = RefDNode(antlr::nullAST);
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		tag_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		arrayindex_list(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		currentAST = __currentAST208;
		_t = __t208;
		_t = _t->getNextSibling();
		tag_array_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case EXPR:
	case IDENTIFIER:
	{
		tag_expr(_t);
		_t = _retTree;
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		tag_array_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	returnAST = tag_array_expr_AST;
	_retTree = _t;
}

void GDLTreeParser::initializeASTFactory( antlr::ASTFactory& factory )
{
	factory.setMaxNodeType(237);
}
const char* GDLTreeParser::tokenNames[] = {
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

const unsigned long GDLTreeParser::_tokenSet_0_data_[] = { 2271215648UL, 3145730UL, 2282356737UL, 1208631296UL, 268435328UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// ASSIGN BLOCK BREAK CONTINUE COMMONDECL COMMONDEF "for" "foreach" MPCALL 
// MPCALL_PARENT PCALL "repeat" RETURN "while" "case" "forward_function" 
// "goto" "if" "on_ioerror" "switch" DEC INC AND_OP_EQ ASTERIX_EQ EQ_OP_EQ 
// GE_OP_EQ GTMARK_EQ GT_OP_EQ LE_OP_EQ LTMARK_EQ LT_OP_EQ MATRIX_OP1_EQ 
// MATRIX_OP2_EQ MINUS_EQ MOD_OP_EQ NE_OP_EQ OR_OP_EQ PLUS_EQ POW_EQ SLASH_EQ 
// XOR_OP_EQ 
const antlr::BitSet GDLTreeParser::_tokenSet_0(_tokenSet_0_data_,12);
const unsigned long GDLTreeParser::_tokenSet_1_data_[] = { 1478233632UL, 1111494784UL, 599785856UL, 333783040UL, 2415918978UL, 0UL, 67088384UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// ASSIGN ARRAYDEF ARRAYDEF_CONST ARRAYDEF_GENERALIZED_INDGEN ARRAYEXPR 
// ARRAYEXPR_FCALL ARRAYEXPR_MFCALL CONSTANT DEREF EXPR FCALL GDLNULL MFCALL 
// MFCALL_PARENT NSTRUC_REF POSTDEC POSTINC STRUC SYSVAR UMINUS VAR "and" 
// "eq" "ge" "gt" "le" "lt" "mod" "ne" "not" "or" "xor" DEC INC AND_OP_EQ 
// ASTERIX_EQ EQ_OP_EQ GE_OP_EQ GTMARK_EQ GT_OP_EQ LE_OP_EQ LTMARK_EQ LT_OP_EQ 
// MATRIX_OP1_EQ MATRIX_OP2_EQ MINUS_EQ MOD_OP_EQ NE_OP_EQ OR_OP_EQ PLUS_EQ 
// POW_EQ SLASH_EQ XOR_OP_EQ SLASH ASTERIX DOT POW MATRIX_OP1 MATRIX_OP2 
// PLUS MINUS LTMARK GTMARK LOG_NEG LOG_AND LOG_OR QUESTION 
const antlr::BitSet GDLTreeParser::_tokenSet_1(_tokenSet_1_data_,16);


