/* $ANTLR 2.7.4: "gdlc.g" -> "GDLParser.cpp"$ */
#include "GDLParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
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

void GDLParser::translation_unit() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode translation_unit_AST = RefDNode(antlr::nullAST);
	
	subReached=false;
	compileOpt=NONE; // reset compileOpt    
	
	
	try {      // for error handling
		{ // ( ... )+
		int _cnt3=0;
		for (;;) {
			switch ( LA(1)) {
			case END_U:
			{
				end_unit();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				break;
			}
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
			case FORWARD:
			{
				forward_function();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				end_unit();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				break;
			}
			default:
			{
				if ( _cnt3>=1 ) { goto _loop3; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
			}
			}
			_cnt3++;
		}
		_loop3:;
		}  // ( ... )+
		{
		RefDNode tmp1_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp1_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp1_AST));
		}
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
	int _cnt32=0;
	for (;;) {
		if ((LA(1) == END_U) && (_tokenSet_0.member(LA(2)))) {
			RefDNode tmp2_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp2_AST = astFactory->create(LT(1));
			}
			match(END_U);
		}
		else {
			if ( _cnt32>=1 ) { goto _loop32; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt32++;
	}
	_loop32:;
	}  // ( ... )+
	returnAST = end_unit_AST;
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
	case IDENTIFIER:
	case BEGIN:
	case SWITCH:
	case CASE:
	case END_U:
	case FORWARD:
	case COMPILE_OPT:
	case COMMON:
	case DEC:
	case INC:
	case BREAK:
	case CONTINUE:
	case REPEAT:
	case WHILE:
	case FOR:
	case GOTO:
	case ON_IOERROR:
	case IF:
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
		
		if( subName == name) subReached=true;
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
	case IDENTIFIER:
	case BEGIN:
	case SWITCH:
	case CASE:
	case END_U:
	case FORWARD:
	case COMPILE_OPT:
	case COMMON:
	case DEC:
	case INC:
	case BREAK:
	case CONTINUE:
	case REPEAT:
	case WHILE:
	case FOR:
	case GOTO:
	case ON_IOERROR:
	case IF:
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
		
		if( subName == name) subReached=true;
		f_AST->SetCompileOpt( compileOpt);
		
	}
	function_def_AST = RefDNode(currentAST.root);
	returnAST = function_def_AST;
}

void GDLParser::forward_function() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode forward_function_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp7_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp7_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp7_AST));
	}
	match(FORWARD);
	identifier_list();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	forward_function_AST = RefDNode(currentAST.root);
	returnAST = forward_function_AST;
}

void GDLParser::interactive_compile() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode interactive_compile_AST = RefDNode(antlr::nullAST);
	
	{
	switch ( LA(1)) {
	case FUNCTION:
	{
		RefDNode tmp8_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp8_AST = astFactory->create(LT(1));
		}
		match(FUNCTION);
		break;
	}
	case PRO:
	{
		RefDNode tmp9_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp9_AST = astFactory->create(LT(1));
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
	RefDNode tmp10_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp10_AST = astFactory->create(LT(1));
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
		RefDNode tmp11_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp11_AST = astFactory->create(LT(1));
		}
		match(METHOD);
		RefDNode tmp12_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp12_AST = astFactory->create(LT(1));
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
		RefDNode tmp13_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp13_AST = astFactory->create(LT(1));
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
		RefDNode tmp14_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp14_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp14_AST));
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
				RefDNode tmp16_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp16_AST = astFactory->create(LT(1));
					astFactory->addASTChild(currentAST, antlr::RefAST(tmp16_AST));
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
			goto _loop38;
		}
		
	}
	_loop38:;
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
		int _cnt12=0;
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
				case ENDIF:
				case ENDELSE:
				case ENDCASE:
				case ENDSWITCH:
				case ENDFOR:
				case ENDWHILE:
				case ENDREP:
				{
					end_mark();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					break;
				}
				case antlr::Token::EOF_TYPE:
				case FUNCTION:
				case PRO:
				case IDENTIFIER:
				case BEGIN:
				case SWITCH:
				case CASE:
				case END_U:
				case FORWARD:
				case COMPILE_OPT:
				case COMMON:
				case DEC:
				case INC:
				case BREAK:
				case CONTINUE:
				case REPEAT:
				case WHILE:
				case FOR:
				case GOTO:
				case ON_IOERROR:
				case IF:
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
			case IDENTIFIER:
			case BEGIN:
			case SWITCH:
			case CASE:
			case FORWARD:
			case COMPILE_OPT:
			case COMMON:
			case DEC:
			case INC:
			case BREAK:
			case CONTINUE:
			case REPEAT:
			case WHILE:
			case FOR:
			case GOTO:
			case ON_IOERROR:
			case IF:
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
				if ( _cnt12>=1 ) { goto _loop12; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
			}
			}
			_cnt12++;
		}
		_loop12:;
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
		RefDNode tmp17_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp17_AST = astFactory->create(LT(1));
		}
		match(END);
		break;
	}
	case ENDIF:
	{
		RefDNode tmp18_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp18_AST = astFactory->create(LT(1));
		}
		match(ENDIF);
		break;
	}
	case ENDELSE:
	{
		RefDNode tmp19_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp19_AST = astFactory->create(LT(1));
		}
		match(ENDELSE);
		break;
	}
	case ENDCASE:
	{
		RefDNode tmp20_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp20_AST = astFactory->create(LT(1));
		}
		match(ENDCASE);
		break;
	}
	case ENDSWITCH:
	{
		RefDNode tmp21_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp21_AST = astFactory->create(LT(1));
		}
		match(ENDSWITCH);
		break;
	}
	case ENDFOR:
	{
		RefDNode tmp22_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp22_AST = astFactory->create(LT(1));
		}
		match(ENDFOR);
		break;
	}
	case ENDWHILE:
	{
		RefDNode tmp23_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp23_AST = astFactory->create(LT(1));
		}
		match(ENDWHILE);
		break;
	}
	case ENDREP:
	{
		RefDNode tmp24_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp24_AST = astFactory->create(LT(1));
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
			goto _loop15;
		}
		
	}
	_loop15:;
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
	
	bool parent=false;
	
	
	switch ( LA(1)) {
	case DEC:
	case INC:
	{
		{
		switch ( LA(1)) {
		case DEC:
		{
			RefDNode tmp28_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp28_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp28_AST));
			}
			match(DEC);
			break;
		}
		case INC:
		{
			RefDNode tmp29_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp29_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp29_AST));
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
	case BREAK:
	{
		RefDNode tmp30_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp30_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp30_AST));
		}
		match(BREAK);
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	case CONTINUE:
	{
		RefDNode tmp31_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp31_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp31_AST));
		}
		match(CONTINUE);
		statement_AST = RefDNode(currentAST.root);
		break;
	}
	default:
		bool synPredMatched79 = false;
		if (((LA(1) == LBRACE) && (_tokenSet_1.member(LA(2))))) {
			int _m79 = mark();
			synPredMatched79 = true;
			inputState->guessing++;
			try {
				{
				assign_expr();
				}
			}
			catch (antlr::RecognitionException& pe) {
				synPredMatched79 = false;
			}
			rewind(_m79);
			inputState->guessing--;
		}
		if ( synPredMatched79 ) {
			assign_expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			{
			switch ( LA(1)) {
			case DEC:
			{
				RefDNode tmp32_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp32_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp32_AST));
				}
				match(DEC);
				break;
			}
			case INC:
			{
				RefDNode tmp33_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp33_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp33_AST));
				}
				match(INC);
				break;
			}
			case ELSE:
			case END_U:
			case UNTIL:
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
		else if ((_tokenSet_1.member(LA(1))) && (_tokenSet_2.member(LA(2)))) {
			{
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
					RefDNode tmp35_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp35_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp35_AST));
					}
					match(AND_OP_EQ);
					break;
				}
				case ASTERIX_EQ:
				{
					RefDNode tmp36_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp36_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp36_AST));
					}
					match(ASTERIX_EQ);
					break;
				}
				case EQ_OP_EQ:
				{
					RefDNode tmp37_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp37_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp37_AST));
					}
					match(EQ_OP_EQ);
					break;
				}
				case GE_OP_EQ:
				{
					RefDNode tmp38_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp38_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp38_AST));
					}
					match(GE_OP_EQ);
					break;
				}
				case GTMARK_EQ:
				{
					RefDNode tmp39_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp39_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp39_AST));
					}
					match(GTMARK_EQ);
					break;
				}
				case GT_OP_EQ:
				{
					RefDNode tmp40_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp40_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp40_AST));
					}
					match(GT_OP_EQ);
					break;
				}
				case LE_OP_EQ:
				{
					RefDNode tmp41_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp41_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp41_AST));
					}
					match(LE_OP_EQ);
					break;
				}
				case LTMARK_EQ:
				{
					RefDNode tmp42_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp42_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp42_AST));
					}
					match(LTMARK_EQ);
					break;
				}
				case LT_OP_EQ:
				{
					RefDNode tmp43_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp43_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp43_AST));
					}
					match(LT_OP_EQ);
					break;
				}
				case MATRIX_OP1_EQ:
				{
					RefDNode tmp44_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp44_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp44_AST));
					}
					match(MATRIX_OP1_EQ);
					break;
				}
				case MATRIX_OP2_EQ:
				{
					RefDNode tmp45_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp45_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp45_AST));
					}
					match(MATRIX_OP2_EQ);
					break;
				}
				case MINUS_EQ:
				{
					RefDNode tmp46_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp46_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp46_AST));
					}
					match(MINUS_EQ);
					break;
				}
				case MOD_OP_EQ:
				{
					RefDNode tmp47_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp47_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp47_AST));
					}
					match(MOD_OP_EQ);
					break;
				}
				case NE_OP_EQ:
				{
					RefDNode tmp48_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp48_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp48_AST));
					}
					match(NE_OP_EQ);
					break;
				}
				case OR_OP_EQ:
				{
					RefDNode tmp49_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp49_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp49_AST));
					}
					match(OR_OP_EQ);
					break;
				}
				case PLUS_EQ:
				{
					RefDNode tmp50_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp50_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp50_AST));
					}
					match(PLUS_EQ);
					break;
				}
				case POW_EQ:
				{
					RefDNode tmp51_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp51_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp51_AST));
					}
					match(POW_EQ);
					break;
				}
				case SLASH_EQ:
				{
					RefDNode tmp52_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp52_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp52_AST));
					}
					match(SLASH_EQ);
					break;
				}
				case XOR_OP_EQ:
				{
					RefDNode tmp53_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp53_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp53_AST));
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
					RefDNode tmp54_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp54_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp54_AST));
					}
					match(DEC);
					break;
				}
				case INC:
				{
					RefDNode tmp55_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp55_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp55_AST));
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
				else if ((LA(1) == IDENTIFIER) && (_tokenSet_3.member(LA(2)))) {
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
			}
			statement_AST = RefDNode(currentAST.root);
		}
		else if ((LA(1) == IDENTIFIER) && (_tokenSet_3.member(LA(2)))) {
			procedure_call();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			statement_AST = RefDNode(currentAST.root);
		}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = statement_AST;
}

void GDLParser::switch_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode switch_statement_AST = RefDNode(antlr::nullAST);
	
	int numBranch=0;
	
	
	RefDNode tmp57_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp57_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp57_AST));
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
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case NOT_OP:
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
		if ((_tokenSet_4.member(LA(1)))) {
			switch_body();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			if ( inputState->guessing==0 ) {
				
				numBranch++;
				
			}
		}
		else {
			goto _loop19;
		}
		
	}
	_loop19:;
	} // ( ... )*
	endswitch_mark();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	if ( inputState->guessing==0 ) {
		
		tmp57_AST->SetNumBranch(numBranch);
		
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
		RefDNode tmp59_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp59_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp59_AST));
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
	case COMMA:
	case COLON:
	case OF:
	case ELSE:
	case END_U:
	case UNTIL:
	case DO:
	case THEN:
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
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case NOT_OP:
	case LOG_NEG:
	{
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		match(COLON);
		{
		switch ( LA(1)) {
		case IDENTIFIER:
		case SWITCH:
		case CASE:
		case FORWARD:
		case COMPILE_OPT:
		case COMMON:
		case DEC:
		case INC:
		case BREAK:
		case CONTINUE:
		case REPEAT:
		case WHILE:
		case FOR:
		case GOTO:
		case ON_IOERROR:
		case IF:
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
		case IDENTIFIER:
		case SWITCH:
		case CASE:
		case FORWARD:
		case COMPILE_OPT:
		case COMMON:
		case DEC:
		case INC:
		case BREAK:
		case CONTINUE:
		case REPEAT:
		case WHILE:
		case FOR:
		case GOTO:
		case ON_IOERROR:
		case IF:
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
		RefDNode tmp66_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp66_AST = astFactory->create(LT(1));
		}
		match(ENDSWITCH);
		break;
	}
	case END:
	{
		RefDNode tmp67_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp67_AST = astFactory->create(LT(1));
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

void GDLParser::statement_list() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode statement_list_AST = RefDNode(antlr::nullAST);
	
	{ // ( ... )+
	int _cnt69=0;
	for (;;) {
		if ((LA(1) == END_U)) {
			end_unit();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else if ((_tokenSet_5.member(LA(1))) && (_tokenSet_6.member(LA(2)))) {
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
			if ( _cnt69>=1 ) { goto _loop69; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt69++;
	}
	_loop69:;
	}  // ( ... )+
	statement_list_AST = RefDNode(currentAST.root);
	returnAST = statement_list_AST;
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
		RefDNode tmp68_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp68_AST = astFactory->create(LT(1));
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
	
	
	RefDNode tmp69_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp69_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp69_AST));
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
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case NOT_OP:
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
		if ((_tokenSet_4.member(LA(1)))) {
			case_body();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
			if ( inputState->guessing==0 ) {
				
				numBranch++;
				
			}
		}
		else {
			goto _loop26;
		}
		
	}
	_loop26:;
	} // ( ... )*
	endcase_mark();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	if ( inputState->guessing==0 ) {
		
		tmp69_AST->SetNumBranch(numBranch);
		
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
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case NOT_OP:
	case LOG_NEG:
	{
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		match(COLON);
		{
		switch ( LA(1)) {
		case IDENTIFIER:
		case SWITCH:
		case CASE:
		case FORWARD:
		case COMPILE_OPT:
		case COMMON:
		case DEC:
		case INC:
		case BREAK:
		case CONTINUE:
		case REPEAT:
		case WHILE:
		case FOR:
		case GOTO:
		case ON_IOERROR:
		case IF:
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
		case IDENTIFIER:
		case SWITCH:
		case CASE:
		case FORWARD:
		case COMPILE_OPT:
		case COMMON:
		case DEC:
		case INC:
		case BREAK:
		case CONTINUE:
		case REPEAT:
		case WHILE:
		case FOR:
		case GOTO:
		case ON_IOERROR:
		case IF:
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
		RefDNode tmp76_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp76_AST = astFactory->create(LT(1));
		}
		match(ENDCASE);
		break;
	}
	case END:
	{
		RefDNode tmp77_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp77_AST = astFactory->create(LT(1));
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
		RefDNode tmp78_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp78_AST = astFactory->create(LT(1));
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
	
	RefDNode tmp79_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp79_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp79_AST));
	}
	match(IDENTIFIER);
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			RefDNode tmp81_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp81_AST = astFactory->create(LT(1));
				astFactory->addASTChild(currentAST, antlr::RefAST(tmp81_AST));
			}
			match(IDENTIFIER);
		}
		else {
			goto _loop56;
		}
		
	}
	_loop56:;
	} // ( ... )*
	identifier_list_AST = RefDNode(currentAST.root);
	returnAST = identifier_list_AST;
}

void GDLParser::keyword_declaration() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode keyword_declaration_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp82_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp82_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp82_AST));
	}
	match(IDENTIFIER);
	match(EQUAL);
	RefDNode tmp84_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp84_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp84_AST));
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
	
	RefDNode tmp85_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp85_AST = astFactory->create(LT(1));
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
			RefDNode tmp86_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp86_AST = astFactory->create(LT(1));
			}
			match(COMMA);
			ii = LT(1);
			if ( inputState->guessing == 0 ) {
				ii_AST = astFactory->create(ii);
			}
			match(IDENTIFIER);
			if ( inputState->guessing==0 ) {
				
				AddCompileOpt( i->getText());
				
			}
		}
		else {
			goto _loop51;
		}
		
	}
	_loop51:;
	} // ( ... )*
	returnAST = compile_opt_AST;
}

void GDLParser::common_block() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode common_block_AST = RefDNode(antlr::nullAST);
	
	match(COMMON);
	RefDNode tmp88_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp88_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp88_AST));
	}
	match(IDENTIFIER);
	{
	switch ( LA(1)) {
	case ELSE:
	case END_U:
	case UNTIL:
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

void GDLParser::endfor_mark() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode endfor_mark_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case ENDFOR:
	{
		RefDNode tmp90_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp90_AST = astFactory->create(LT(1));
		}
		match(ENDFOR);
		break;
	}
	case END:
	{
		RefDNode tmp91_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp91_AST = astFactory->create(LT(1));
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
		RefDNode tmp92_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp92_AST = astFactory->create(LT(1));
		}
		match(ENDREP);
		break;
	}
	case END:
	{
		RefDNode tmp93_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp93_AST = astFactory->create(LT(1));
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
		RefDNode tmp94_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp94_AST = astFactory->create(LT(1));
		}
		match(ENDWHILE);
		break;
	}
	case END:
	{
		RefDNode tmp95_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp95_AST = astFactory->create(LT(1));
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
		RefDNode tmp96_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp96_AST = astFactory->create(LT(1));
		}
		match(ENDIF);
		break;
	}
	case END:
	{
		RefDNode tmp97_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp97_AST = astFactory->create(LT(1));
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
		RefDNode tmp98_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp98_AST = astFactory->create(LT(1));
		}
		match(ENDELSE);
		break;
	}
	case END:
	{
		RefDNode tmp99_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp99_AST = astFactory->create(LT(1));
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
	case IDENTIFIER:
	case SWITCH:
	case CASE:
	case FORWARD:
	case COMPILE_OPT:
	case COMMON:
	case DEC:
	case INC:
	case BREAK:
	case CONTINUE:
	case REPEAT:
	case WHILE:
	case FOR:
	case GOTO:
	case ON_IOERROR:
	case IF:
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
	int _cnt73=0;
	for (;;) {
		if ((LA(1) == IDENTIFIER) && (LA(2) == COLON)) {
			label();
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
	{
	switch ( LA(1)) {
	case IDENTIFIER:
	case BEGIN:
	case SWITCH:
	case CASE:
	case FORWARD:
	case COMPILE_OPT:
	case COMMON:
	case DEC:
	case INC:
	case BREAK:
	case CONTINUE:
	case REPEAT:
	case WHILE:
	case FOR:
	case GOTO:
	case ON_IOERROR:
	case IF:
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
	
	RefDNode tmp101_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp101_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp101_AST));
	}
	match(IDENTIFIER);
	RefDNode tmp102_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp102_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp102_AST));
	}
	match(COLON);
	label_AST = RefDNode(currentAST.root);
	returnAST = label_AST;
}

void GDLParser::baseclass_method() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode baseclass_method_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp103_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp103_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp103_AST));
	}
	match(IDENTIFIER);
	match(METHOD);
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

void GDLParser::deref_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode deref_expr_AST = RefDNode(antlr::nullAST);
	RefDNode a1_AST = RefDNode(antlr::nullAST);
	
	RefDNode dot;
	SizeT nDot;
	
	
	switch ( LA(1)) {
	case IDENTIFIER:
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
		case COMMA:
		case COLON:
		case OF:
		case ELSE:
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
		case UNTIL:
		case DO:
		case THEN:
		case RBRACE:
		case SLASH:
		case RSQUARE:
		case RCURLY:
		case ASTERIX:
		case POW:
		case MATRIX_OP1:
		case MATRIX_OP2:
		case MOD_OP:
		case PLUS:
		case MINUS:
		case LTMARK:
		case GTMARK:
		case EQ_OP:
		case NE_OP:
		case LE_OP:
		case LT_OP:
		case GE_OP:
		case GT_OP:
		case AND_OP:
		case OR_OP:
		case XOR_OP:
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

void GDLParser::formal_procedure_call() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode formal_procedure_call_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp109_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp109_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp109_AST));
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
	case END_U:
	case UNTIL:
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
	if (((_tokenSet_3.member(LA(1))) && (_tokenSet_0.member(LA(2))))&&(id->getText() == "RETURN")) {
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
		case END_U:
		case UNTIL:
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
	else if ((_tokenSet_3.member(LA(1))) && (_tokenSet_7.member(LA(2)))) {
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
		case END_U:
		case UNTIL:
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
	
	RefDNode tmp113_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp113_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp113_AST));
	}
	match(FOR);
	RefDNode tmp114_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp114_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp114_AST));
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

void GDLParser::repeat_statement() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode repeat_statement_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp119_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp119_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp119_AST));
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
	
	RefDNode tmp121_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp121_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp121_AST));
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
		RefDNode tmp123_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp123_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp123_AST));
		}
		match(GOTO);
		match(COMMA);
		RefDNode tmp125_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp125_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp125_AST));
		}
		match(IDENTIFIER);
		jump_statement_AST = RefDNode(currentAST.root);
		break;
	}
	case ON_IOERROR:
	{
		RefDNode tmp126_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp126_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp126_AST));
		}
		match(ON_IOERROR);
		match(COMMA);
		RefDNode tmp128_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp128_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp128_AST));
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
	
	RefDNode tmp129_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp129_AST = astFactory->create(LT(1));
		astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp129_AST));
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
	if ((LA(1) == ELSE) && (_tokenSet_5.member(LA(2)))) {
		match(ELSE);
		else_block();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
	}
	else if ((LA(1) == ELSE || LA(1) == END_U || LA(1) == UNTIL) && (_tokenSet_0.member(LA(2)))) {
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
	case IDENTIFIER:
	case SWITCH:
	case CASE:
	case FORWARD:
	case COMPILE_OPT:
	case COMMON:
	case DEC:
	case INC:
	case BREAK:
	case CONTINUE:
	case REPEAT:
	case WHILE:
	case FOR:
	case GOTO:
	case ON_IOERROR:
	case IF:
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
	case IDENTIFIER:
	case SWITCH:
	case CASE:
	case FORWARD:
	case COMPILE_OPT:
	case COMMON:
	case DEC:
	case INC:
	case BREAK:
	case CONTINUE:
	case REPEAT:
	case WHILE:
	case FOR:
	case GOTO:
	case ON_IOERROR:
	case IF:
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
	case IDENTIFIER:
	case SWITCH:
	case CASE:
	case FORWARD:
	case COMPILE_OPT:
	case COMMON:
	case DEC:
	case INC:
	case BREAK:
	case CONTINUE:
	case REPEAT:
	case WHILE:
	case FOR:
	case GOTO:
	case ON_IOERROR:
	case IF:
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

void GDLParser::if_block() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode if_block_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case IDENTIFIER:
	case SWITCH:
	case CASE:
	case FORWARD:
	case COMPILE_OPT:
	case COMMON:
	case DEC:
	case INC:
	case BREAK:
	case CONTINUE:
	case REPEAT:
	case WHILE:
	case FOR:
	case GOTO:
	case ON_IOERROR:
	case IF:
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
	case IDENTIFIER:
	case SWITCH:
	case CASE:
	case FORWARD:
	case COMPILE_OPT:
	case COMMON:
	case DEC:
	case INC:
	case BREAK:
	case CONTINUE:
	case REPEAT:
	case WHILE:
	case FOR:
	case GOTO:
	case ON_IOERROR:
	case IF:
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
			goto _loop110;
		}
		
	}
	_loop110:;
	} // ( ... )*
	parameter_def_list_AST = RefDNode(currentAST.root);
	returnAST = parameter_def_list_AST;
}

void GDLParser::formal_function_call() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode formal_function_call_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp138_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp138_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp138_AST));
	}
	match(IDENTIFIER);
	match(LBRACE);
	{
	switch ( LA(1)) {
	case IDENTIFIER:
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
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case NOT_OP:
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
	antlr::RefToken  id = antlr::nullToken;
	RefDNode id_AST = RefDNode(antlr::nullAST);
	
	if ((LA(1) == IDENTIFIER) && (LA(2) == EQUAL)) {
		RefDNode tmp141_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp141_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp141_AST));
		}
		match(IDENTIFIER);
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
	else if ((_tokenSet_8.member(LA(1))) && (_tokenSet_9.member(LA(2)))) {
		expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		parameter_def_AST = RefDNode(currentAST.root);
	}
	else if ((LA(1) == SLASH)) {
		match(SLASH);
		id = LT(1);
		if ( inputState->guessing == 0 ) {
			id_AST = astFactory->create(id);
			astFactory->addASTChild(currentAST, antlr::RefAST(id_AST));
		}
		match(IDENTIFIER);
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
	
	match(LSQUARE);
	expr();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA)) {
			match(COMMA);
			expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop113;
		}
		
	}
	_loop113:;
	} // ( ... )*
	match(RSQUARE);
	if ( inputState->guessing==0 ) {
		array_def_AST = RefDNode(currentAST.root);
		array_def_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(ARRAYDEF,"array_def")))->add(antlr::RefAST(array_def_AST))));
		
		currentAST.root = array_def_AST;
		if ( array_def_AST!=RefDNode(antlr::nullAST) &&
			array_def_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = array_def_AST->getFirstChild();
		else
			currentAST.child = array_def_AST;
		currentAST.advanceChildToEnd();
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
	
	{
	switch ( LA(1)) {
	case IDENTIFIER:
	{
		RefDNode tmp147_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp147_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp147_AST));
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
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	}
	struct_identifier_AST = RefDNode(currentAST.root);
	returnAST = struct_identifier_AST;
}

void GDLParser::struct_def() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode struct_def_AST = RefDNode(antlr::nullAST);
	
	match(LCURLY);
	{
	if ((LA(1) == IDENTIFIER || LA(1) == SYSVARNAME || LA(1) == EXCLAMATION) && (LA(2) == COMMA || LA(2) == RCURLY)) {
		struct_identifier();
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
	else if ((LA(1) == IDENTIFIER || LA(1) == SYSVARNAME || LA(1) == EXCLAMATION) && (LA(2) == COLON)) {
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
	
	{
	switch ( LA(1)) {
	case IDENTIFIER:
	case DEC:
	case INC:
	case LBRACE:
	case LSQUARE:
	case SYSVARNAME:
	case EXCLAMATION:
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
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case NOT_OP:
	case LOG_NEG:
	{
		ntag_def_list();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		break;
	}
	case INHERITS:
	{
		RefDNode tmp152_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp152_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp152_AST));
		}
		match(INHERITS);
		RefDNode tmp153_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp153_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp153_AST));
		}
		match(IDENTIFIER);
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
		if ((LA(1) == COMMA)) {
			match(COMMA);
			{
			switch ( LA(1)) {
			case IDENTIFIER:
			case DEC:
			case INC:
			case LBRACE:
			case LSQUARE:
			case SYSVARNAME:
			case EXCLAMATION:
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
			case ASTERIX:
			case STRING_LITERAL:
			case PLUS:
			case MINUS:
			case NOT_OP:
			case LOG_NEG:
			{
				ntag_def_list();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				break;
			}
			case INHERITS:
			{
				RefDNode tmp155_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp155_AST = astFactory->create(LT(1));
					astFactory->addASTChild(currentAST, antlr::RefAST(tmp155_AST));
				}
				match(INHERITS);
				RefDNode tmp156_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp156_AST = astFactory->create(LT(1));
					astFactory->addASTChild(currentAST, antlr::RefAST(tmp156_AST));
				}
				match(IDENTIFIER);
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
			goto _loop131;
		}
		
	}
	_loop131:;
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
			goto _loop122;
		}
		
	}
	_loop122:;
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
	
	if ((LA(1) == IDENTIFIER || LA(1) == SYSVARNAME || LA(1) == EXCLAMATION) && (LA(2) == COLON)) {
		tag_def();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		ntag_def_AST = RefDNode(currentAST.root);
	}
	else if ((_tokenSet_8.member(LA(1))) && (_tokenSet_10.member(LA(2)))) {
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

void GDLParser::ntag_def_list() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode ntag_def_list_AST = RefDNode(antlr::nullAST);
	
	ntag_def();
	if (inputState->guessing==0) {
		astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
	}
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == COMMA) && (_tokenSet_11.member(LA(2)))) {
			match(COMMA);
			ntag_def();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop126;
		}
		
	}
	_loop126:;
	} // ( ... )*
	ntag_def_list_AST = RefDNode(currentAST.root);
	returnAST = ntag_def_list_AST;
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
	
	switch ( LA(1)) {
	case LSQUARE:
	{
		match(LSQUARE);
		arrayindex();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				arrayindex();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
			}
			else {
				goto _loop135;
			}
			
		}
		_loop135:;
		} // ( ... )*
		match(RSQUARE);
		arrayindex_list_AST = RefDNode(currentAST.root);
		break;
	}
	case LBRACE:
	{
		match(LBRACE);
		arrayindex();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == COMMA)) {
				match(COMMA);
				arrayindex();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
			}
			else {
				goto _loop137;
			}
			
		}
		_loop137:;
		} // ( ... )*
		match(RBRACE);
		arrayindex_list_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = arrayindex_list_AST;
}

void GDLParser::arrayindex() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode arrayindex_AST = RefDNode(antlr::nullAST);
	
	{
	bool synPredMatched143 = false;
	if (((LA(1) == ASTERIX) && (LA(2) == COMMA || LA(2) == RBRACE || LA(2) == RSQUARE))) {
		int _m143 = mark();
		synPredMatched143 = true;
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
			case RBRACE:
			{
				match(RBRACE);
				break;
			}
			case RSQUARE:
			{
				match(RSQUARE);
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
			synPredMatched143 = false;
		}
		rewind(_m143);
		inputState->guessing--;
	}
	if ( synPredMatched143 ) {
		all();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
	}
	else if ((_tokenSet_8.member(LA(1))) && (_tokenSet_12.member(LA(2)))) {
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
			bool synPredMatched148 = false;
			if (((LA(1) == ASTERIX) && (LA(2) == COMMA || LA(2) == RBRACE || LA(2) == RSQUARE))) {
				int _m148 = mark();
				synPredMatched148 = true;
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
					case RBRACE:
					{
						match(RBRACE);
						break;
					}
					case RSQUARE:
					{
						match(RSQUARE);
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
					synPredMatched148 = false;
				}
				rewind(_m148);
				inputState->guessing--;
			}
			if ( synPredMatched148 ) {
				all();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
			}
			else if ((_tokenSet_8.member(LA(1))) && (_tokenSet_13.member(LA(2)))) {
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

void GDLParser::all() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode all_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp167_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp167_AST = astFactory->create(LT(1));
	}
	match(ASTERIX);
	if ( inputState->guessing==0 ) {
		all_AST = RefDNode(currentAST.root);
		all_AST = RefDNode(astFactory->make((new antlr::ASTArray(1))->add(antlr::RefAST(astFactory->create(ALL,"*")))));
		currentAST.root = all_AST;
		if ( all_AST!=RefDNode(antlr::nullAST) &&
			all_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = all_AST->getFirstChild();
		else
			currentAST.child = all_AST;
		currentAST.advanceChildToEnd();
	}
	returnAST = all_AST;
}

void GDLParser::sysvar() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode sysvar_AST = RefDNode(antlr::nullAST);
	
	RefDNode tmp168_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp168_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp168_AST));
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
	
	RefDNode tmp169_AST = RefDNode(antlr::nullAST);
	if ( inputState->guessing == 0 ) {
		tmp169_AST = astFactory->create(LT(1));
		astFactory->addASTChild(currentAST, antlr::RefAST(tmp169_AST));
	}
	match(IDENTIFIER);
	if ( inputState->guessing==0 ) {
		var_AST = RefDNode(currentAST.root);
		var_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(VAR,"VAR")))->add(antlr::RefAST(var_AST))));
		currentAST.root = var_AST;
		if ( var_AST!=RefDNode(antlr::nullAST) &&
			var_AST->getFirstChild() != RefDNode(antlr::nullAST) )
			  currentAST.child = var_AST->getFirstChild();
		else
			currentAST.child = var_AST;
		currentAST.advanceChildToEnd();
	}
	var_AST = RefDNode(currentAST.root);
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
							RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ARRAYEXPR,"arrayexpr")))->add(antlr::RefAST(al_AST))->add(antlr::RefAST(e_AST))));
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
	case COMMA:
	case COLON:
	case OF:
	case ELSE:
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
	case UNTIL:
	case DO:
	case THEN:
	case RBRACE:
	case SLASH:
	case RSQUARE:
	case RCURLY:
	case ASTERIX:
	case DOT:
	case POW:
	case MATRIX_OP1:
	case MATRIX_OP2:
	case MOD_OP:
	case PLUS:
	case MINUS:
	case LTMARK:
	case GTMARK:
	case EQ_OP:
	case NE_OP:
	case LE_OP:
	case LT_OP:
	case GE_OP:
	case GT_OP:
	case AND_OP:
	case OR_OP:
	case XOR_OP:
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
		RefDNode tmp172_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp172_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp172_AST));
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
			RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ARRAYEXPR,"arrayexpr")))->add(antlr::RefAST(al_AST))->add(antlr::RefAST(e_AST))));
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
	case COMMA:
	case COLON:
	case OF:
	case ELSE:
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
	case UNTIL:
	case DO:
	case THEN:
	case RBRACE:
	case SLASH:
	case RSQUARE:
	case RCURLY:
	case ASTERIX:
	case DOT:
	case POW:
	case MATRIX_OP1:
	case MATRIX_OP2:
	case MOD_OP:
	case PLUS:
	case MINUS:
	case LTMARK:
	case GTMARK:
	case EQ_OP:
	case NE_OP:
	case LE_OP:
	case LT_OP:
	case GE_OP:
	case GT_OP:
	case AND_OP:
	case OR_OP:
	case XOR_OP:
	case LOG_AND:
	case LOG_OR:
	case QUESTION:
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

SizeT  GDLParser::tag_access() {
	SizeT nDot;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode tag_access_AST = RefDNode(antlr::nullAST);
	
	nDot=0;
	
	
	{ // ( ... )+
	int _cnt160=0;
	for (;;) {
		if ((LA(1) == DOT)) {
			match(DOT);
			if ( inputState->guessing==0 ) {
				nDot++;
			}
			array_expr_nth();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			if ( _cnt160>=1 ) { goto _loop160; } else {throw antlr::NoViableAltException(LT(1), getFilename());}
		}
		
		_cnt160++;
	}
	_loop160:;
	}  // ( ... )+
	tag_access_AST = RefDNode(currentAST.root);
	returnAST = tag_access_AST;
	return nDot;
}

void GDLParser::array_expr_fn() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode array_expr_fn_AST = RefDNode(antlr::nullAST);
	RefDNode v_AST = RefDNode(antlr::nullAST);
	RefDNode al_AST = RefDNode(antlr::nullAST);
	
	RefDNode dot, t;
	SizeT nDot;
	
	
	var();
	if (inputState->guessing==0) {
		v_AST = returnAST;
	}
	arrayindex_list();
	if (inputState->guessing==0) {
		al_AST = returnAST;
	}
	{
	switch ( LA(1)) {
	case DOT:
	{
		nDot=tag_access();
		if ( inputState->guessing==0 ) {
			array_expr_fn_AST = RefDNode(currentAST.root);
			// -> do so 
			t= RefDNode(returnAST);    
			
			dot=astFactory->create(DOT,".");
			dot->SetNDot( nDot);    
			dot->SetLine( al_AST->getLine());
			
			array_expr_fn_AST = 
				  		RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(dot))->add(antlr::RefAST(RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ARRAYEXPR,"arrayexpr")))->add(antlr::RefAST(al_AST))->add(antlr::RefAST(v_AST))))))->add(antlr::RefAST(t))));
			currentAST.root = array_expr_fn_AST;
			if ( array_expr_fn_AST!=RefDNode(antlr::nullAST) &&
				array_expr_fn_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = array_expr_fn_AST->getFirstChild();
			else
				currentAST.child = array_expr_fn_AST;
			currentAST.advanceChildToEnd();
		}
		break;
	}
	case COMMA:
	case COLON:
	case OF:
	case ELSE:
	case END_U:
	case DEC:
	case INC:
	case MEMBER:
	case UNTIL:
	case DO:
	case THEN:
	case RBRACE:
	case SLASH:
	case RSQUARE:
	case RCURLY:
	case ASTERIX:
	case POW:
	case MATRIX_OP1:
	case MATRIX_OP2:
	case MOD_OP:
	case PLUS:
	case MINUS:
	case LTMARK:
	case GTMARK:
	case EQ_OP:
	case NE_OP:
	case LE_OP:
	case LT_OP:
	case GE_OP:
	case GT_OP:
	case AND_OP:
	case OR_OP:
	case XOR_OP:
	case LOG_AND:
	case LOG_OR:
	case QUESTION:
	{
		if ( inputState->guessing==0 ) {
			array_expr_fn_AST = RefDNode(currentAST.root);
			array_expr_fn_AST = 
				  		RefDNode(astFactory->make((new antlr::ASTArray(3))->add(antlr::RefAST(astFactory->create(ARRAYEXPR_FN,"arrayexpr_fn")))->add(antlr::RefAST(v_AST))->add(antlr::RefAST(al_AST))));
			currentAST.root = array_expr_fn_AST;
			if ( array_expr_fn_AST!=RefDNode(antlr::nullAST) &&
				array_expr_fn_AST->getFirstChild() != RefDNode(antlr::nullAST) )
				  currentAST.child = array_expr_fn_AST->getFirstChild();
			else
				currentAST.child = array_expr_fn_AST;
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
	returnAST = array_expr_fn_AST;
}

bool  GDLParser::member_function_call() {
	bool parent;
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode member_function_call_AST = RefDNode(antlr::nullAST);
	
	if ( inputState->guessing==0 ) {
		parent = false;
	}
	match(MEMBER);
	{
	if ((LA(1) == IDENTIFIER) && (LA(2) == METHOD)) {
		RefDNode tmp175_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp175_AST = astFactory->create(LT(1));
			astFactory->addASTChild(currentAST, antlr::RefAST(tmp175_AST));
		}
		match(IDENTIFIER);
		match(METHOD);
		if ( inputState->guessing==0 ) {
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

void GDLParser::primary_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode primary_expr_AST = RefDNode(antlr::nullAST);
	antlr::RefToken  sl = antlr::nullToken;
	RefDNode sl_AST = RefDNode(antlr::nullAST);
	
	bool parent;
	
	
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
	{
		numeric_constant();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case LSQUARE:
	{
		array_def();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case LCURLY:
	{
		struct_def();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		primary_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
		bool synPredMatched172 = false;
		if (((LA(1) == IDENTIFIER) && (LA(2) == LBRACE || LA(2) == LSQUARE))) {
			int _m172 = mark();
			synPredMatched172 = true;
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
						goto _loop171;
					}
					
				}
				_loop171:;
				} // ( ... )*
				match(RBRACE);
				}
			}
			catch (antlr::RecognitionException& pe) {
				synPredMatched172 = false;
			}
			rewind(_m172);
			inputState->guessing--;
		}
		if ( synPredMatched172 ) {
			{
			if (((LA(1) == IDENTIFIER) && (LA(2) == LBRACE))&&( IsFun(LT(1)))) {
				formal_function_call();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				if ( inputState->guessing==0 ) {
					primary_expr_AST = RefDNode(currentAST.root);
					primary_expr_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(FCALL,"fcall")))->add(antlr::RefAST(primary_expr_AST))));
					currentAST.root = primary_expr_AST;
					if ( primary_expr_AST!=RefDNode(antlr::nullAST) &&
						primary_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
						  currentAST.child = primary_expr_AST->getFirstChild();
					else
						currentAST.child = primary_expr_AST;
					currentAST.advanceChildToEnd();
				}
			}
			else if ((LA(1) == IDENTIFIER) && (LA(2) == LBRACE || LA(2) == LSQUARE)) {
				array_expr_fn();
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
						primary_expr_AST = RefDNode(currentAST.root);
						
						if( parent)
						{
						primary_expr_AST = 
						RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MFCALL_PARENT,"mfcall::")))->add(antlr::RefAST(primary_expr_AST))));
						}
						else
						{
						primary_expr_AST = 
						RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MFCALL,"mfcall")))->add(antlr::RefAST(primary_expr_AST))));
						}
						
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
				case COMMA:
				case COLON:
				case OF:
				case ELSE:
				case END_U:
				case DEC:
				case INC:
				case UNTIL:
				case DO:
				case THEN:
				case RBRACE:
				case SLASH:
				case RSQUARE:
				case RCURLY:
				case ASTERIX:
				case POW:
				case MATRIX_OP1:
				case MATRIX_OP2:
				case MOD_OP:
				case PLUS:
				case MINUS:
				case LTMARK:
				case GTMARK:
				case EQ_OP:
				case NE_OP:
				case LE_OP:
				case LT_OP:
				case GE_OP:
				case GT_OP:
				case AND_OP:
				case OR_OP:
				case XOR_OP:
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
			}
			else {
				throw antlr::NoViableAltException(LT(1), getFilename());
			}
			
			}
			primary_expr_AST = RefDNode(currentAST.root);
		}
		else {
			bool synPredMatched176 = false;
			if (((LA(1) == IDENTIFIER) && (LA(2) == LBRACE))) {
				int _m176 = mark();
				synPredMatched176 = true;
				inputState->guessing++;
				try {
					{
					formal_function_call();
					}
				}
				catch (antlr::RecognitionException& pe) {
					synPredMatched176 = false;
				}
				rewind(_m176);
				inputState->guessing--;
			}
			if ( synPredMatched176 ) {
				formal_function_call();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
				if ( inputState->guessing==0 ) {
					primary_expr_AST = RefDNode(currentAST.root);
					primary_expr_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(FCALL,"fcall")))->add(antlr::RefAST(primary_expr_AST))));
					currentAST.root = primary_expr_AST;
					if ( primary_expr_AST!=RefDNode(antlr::nullAST) &&
						primary_expr_AST->getFirstChild() != RefDNode(antlr::nullAST) )
						  currentAST.child = primary_expr_AST->getFirstChild();
					else
						currentAST.child = primary_expr_AST;
					currentAST.advanceChildToEnd();
				}
				primary_expr_AST = RefDNode(currentAST.root);
			}
			else {
				bool synPredMatched178 = false;
				if (((_tokenSet_1.member(LA(1))) && (_tokenSet_14.member(LA(2))))) {
					int _m178 = mark();
					synPredMatched178 = true;
					inputState->guessing++;
					try {
						{
						deref_expr();
						}
					}
					catch (antlr::RecognitionException& pe) {
						synPredMatched178 = false;
					}
					rewind(_m178);
					inputState->guessing--;
				}
				if ( synPredMatched178 ) {
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
							primary_expr_AST = RefDNode(currentAST.root);
							
							if( parent)
							{
							primary_expr_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MFCALL_PARENT,"mfcall::")))->add(antlr::RefAST(primary_expr_AST))));
							}
							else
							{
							primary_expr_AST = RefDNode(astFactory->make((new antlr::ASTArray(2))->add(antlr::RefAST(astFactory->create(MFCALL,"mfcall")))->add(antlr::RefAST(primary_expr_AST))));
							}
							
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
					case COMMA:
					case COLON:
					case OF:
					case ELSE:
					case END_U:
					case DEC:
					case INC:
					case UNTIL:
					case DO:
					case THEN:
					case RBRACE:
					case SLASH:
					case RSQUARE:
					case RCURLY:
					case ASTERIX:
					case POW:
					case MATRIX_OP1:
					case MATRIX_OP2:
					case MOD_OP:
					case PLUS:
					case MINUS:
					case LTMARK:
					case GTMARK:
					case EQ_OP:
					case NE_OP:
					case LE_OP:
					case LT_OP:
					case GE_OP:
					case GT_OP:
					case AND_OP:
					case OR_OP:
					case XOR_OP:
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
					primary_expr_AST = RefDNode(currentAST.root);
				}
				else if ((LA(1) == LBRACE) && (_tokenSet_1.member(LA(2)))) {
					assign_expr();
					if (inputState->guessing==0) {
						astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
					}
					primary_expr_AST = RefDNode(currentAST.root);
				}
	else {
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}}}
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
				i_AST->setType( POSTINC);
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
				d_AST->setType( POSTDEC);
			}
			break;
		}
		case COMMA:
		case COLON:
		case OF:
		case ELSE:
		case END_U:
		case UNTIL:
		case DO:
		case THEN:
		case RBRACE:
		case SLASH:
		case RSQUARE:
		case RCURLY:
		case ASTERIX:
		case POW:
		case MATRIX_OP1:
		case MATRIX_OP2:
		case MOD_OP:
		case PLUS:
		case MINUS:
		case LTMARK:
		case GTMARK:
		case EQ_OP:
		case NE_OP:
		case LE_OP:
		case LT_OP:
		case GE_OP:
		case GT_OP:
		case AND_OP:
		case OR_OP:
		case XOR_OP:
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
		RefDNode tmp177_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp177_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp177_AST));
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
		RefDNode tmp178_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp178_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp178_AST));
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
			RefDNode tmp179_AST = RefDNode(antlr::nullAST);
			if ( inputState->guessing == 0 ) {
				tmp179_AST = astFactory->create(LT(1));
				astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp179_AST));
			}
			match(POW);
			decinc_expr();
			if (inputState->guessing==0) {
				astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
			}
		}
		else {
			goto _loop184;
		}
		
	}
	_loop184:;
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
		if ((_tokenSet_15.member(LA(1)))) {
			{
			switch ( LA(1)) {
			case ASTERIX:
			{
				RefDNode tmp180_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp180_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp180_AST));
				}
				match(ASTERIX);
				break;
			}
			case MATRIX_OP1:
			{
				RefDNode tmp181_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp181_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp181_AST));
				}
				match(MATRIX_OP1);
				break;
			}
			case MATRIX_OP2:
			{
				RefDNode tmp182_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp182_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp182_AST));
				}
				match(MATRIX_OP2);
				break;
			}
			case SLASH:
			{
				RefDNode tmp183_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp183_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp183_AST));
				}
				match(SLASH);
				break;
			}
			case MOD_OP:
			{
				RefDNode tmp184_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp184_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp184_AST));
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
			goto _loop188;
		}
		
	}
	_loop188:;
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
	
	switch ( LA(1)) {
	case IDENTIFIER:
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
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	{
		signed_multiplicative_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		{ // ( ... )*
		for (;;) {
			if (((LA(1) >= PLUS && LA(1) <= GTMARK))) {
				{
				switch ( LA(1)) {
				case PLUS:
				{
					RefDNode tmp186_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp186_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp186_AST));
					}
					match(PLUS);
					break;
				}
				case MINUS:
				{
					RefDNode tmp187_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp187_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp187_AST));
					}
					match(MINUS);
					break;
				}
				case LTMARK:
				{
					RefDNode tmp188_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp188_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp188_AST));
					}
					match(LTMARK);
					break;
				}
				case GTMARK:
				{
					RefDNode tmp189_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp189_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp189_AST));
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
				multiplicative_expr();
				if (inputState->guessing==0) {
					astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
				}
			}
			else {
				goto _loop193;
			}
			
		}
		_loop193:;
		} // ( ... )*
		additive_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case NOT_OP:
	{
		RefDNode tmp190_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp190_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp190_AST));
		}
		match(NOT_OP);
		additive_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		additive_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = additive_expr_AST;
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
		if (((LA(1) >= EQ_OP && LA(1) <= GT_OP))) {
			{
			switch ( LA(1)) {
			case EQ_OP:
			{
				RefDNode tmp191_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp191_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp191_AST));
				}
				match(EQ_OP);
				break;
			}
			case NE_OP:
			{
				RefDNode tmp192_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp192_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp192_AST));
				}
				match(NE_OP);
				break;
			}
			case LE_OP:
			{
				RefDNode tmp193_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp193_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp193_AST));
				}
				match(LE_OP);
				break;
			}
			case LT_OP:
			{
				RefDNode tmp194_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp194_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp194_AST));
				}
				match(LT_OP);
				break;
			}
			case GE_OP:
			{
				RefDNode tmp195_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp195_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp195_AST));
				}
				match(GE_OP);
				break;
			}
			case GT_OP:
			{
				RefDNode tmp196_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp196_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp196_AST));
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
			goto _loop197;
		}
		
	}
	_loop197:;
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
		if (((LA(1) >= AND_OP && LA(1) <= XOR_OP))) {
			{
			switch ( LA(1)) {
			case AND_OP:
			{
				RefDNode tmp197_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp197_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp197_AST));
				}
				match(AND_OP);
				break;
			}
			case OR_OP:
			{
				RefDNode tmp198_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp198_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp198_AST));
				}
				match(OR_OP);
				break;
			}
			case XOR_OP:
			{
				RefDNode tmp199_AST = RefDNode(antlr::nullAST);
				if ( inputState->guessing == 0 ) {
					tmp199_AST = astFactory->create(LT(1));
					astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp199_AST));
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
			goto _loop201;
		}
		
	}
	_loop201:;
	} // ( ... )*
	boolean_expr_AST = RefDNode(currentAST.root);
	returnAST = boolean_expr_AST;
}

void GDLParser::logical_expr() {
	returnAST = RefDNode(antlr::nullAST);
	antlr::ASTPair currentAST;
	RefDNode logical_expr_AST = RefDNode(antlr::nullAST);
	
	switch ( LA(1)) {
	case IDENTIFIER:
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
	case ASTERIX:
	case STRING_LITERAL:
	case PLUS:
	case MINUS:
	case NOT_OP:
	{
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
					RefDNode tmp200_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp200_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp200_AST));
					}
					match(LOG_AND);
					break;
				}
				case LOG_OR:
				{
					RefDNode tmp201_AST = RefDNode(antlr::nullAST);
					if ( inputState->guessing == 0 ) {
						tmp201_AST = astFactory->create(LT(1));
						astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp201_AST));
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
				goto _loop205;
			}
			
		}
		_loop205:;
		} // ( ... )*
		logical_expr_AST = RefDNode(currentAST.root);
		break;
	}
	case LOG_NEG:
	{
		RefDNode tmp202_AST = RefDNode(antlr::nullAST);
		if ( inputState->guessing == 0 ) {
			tmp202_AST = astFactory->create(LT(1));
			astFactory->makeASTRoot(currentAST, antlr::RefAST(tmp202_AST));
		}
		match(LOG_NEG);
		logical_expr();
		if (inputState->guessing==0) {
			astFactory->addASTChild(currentAST, antlr::RefAST(returnAST));
		}
		logical_expr_AST = RefDNode(currentAST.root);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(LT(1), getFilename());
	}
	}
	returnAST = logical_expr_AST;
}

void GDLParser::initializeASTFactory( antlr::ASTFactory& factory )
{
	factory.setMaxNodeType(192);
}
const char* GDLParser::tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"ALL",
	"ASSIGN",
	"ARRAYDEF",
	"ARRAYIX",
	"ARRAYIX_ALL",
	"ARRAYIX_ORANGE",
	"ARRAYIX_RANGE",
	"ARRAYEXPR",
	"ARRAYEXPR_FN",
	"BLOCK",
	"COMMONDECL",
	"COMMONDEF",
	"CONSTANT",
	"DEREF",
	"ELSEBLK",
	"EXPR",
	"FOR_STEP",
	"FCALL",
	"FCALL_LIB",
	"IF_ELSE",
	"KEYDECL",
	"KEYDEF",
	"KEYDEF_REF",
	"LABEL",
	"MPCALL",
	"MPCALL_PARENT",
	"MFCALL",
	"MFCALL_LIB",
	"MFCALL_PARENT",
	"MFCALL_PARENT_LIB",
	"NOP",
	"NSTRUC",
	"NSTRUC_REF",
	"ON_IOERROR_NULL",
	"PCALL",
	"PCALL_LIB",
	"PARADECL",
	"POSTDEC",
	"POSTINC",
	"DECSTATEMENT",
	"INCSTATEMENT",
	"REF",
	"RETURN",
	"RETF",
	"RETP",
	"STRUC",
	"SYSVAR",
	"UMINUS",
	"VAR",
	"VARPTR",
	"\"function\"",
	"\"pro\"",
	"IDENTIFIER",
	"METHOD",
	"COMMA",
	"\"begin\"",
	"COLON",
	"\"switch\"",
	"\"of\"",
	"\"else\"",
	"\"case\"",
	"END_U",
	"\"forward_function\"",
	"EQUAL",
	"\"end\"",
	"\"compile_opt\"",
	"\"common\"",
	"\"endif\"",
	"\"endelse\"",
	"\"endcase\"",
	"\"endswitch\"",
	"\"endfor\"",
	"\"endwhile\"",
	"\"endrep\"",
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
	"\"break\"",
	"\"continue\"",
	"\"repeat\"",
	"\"until\"",
	"\"while\"",
	"\"do\"",
	"\"for\"",
	"\"goto\"",
	"\"on_ioerror\"",
	"\"if\"",
	"\"then\"",
	"LBRACE",
	"RBRACE",
	"SLASH",
	"LSQUARE",
	"RSQUARE",
	"SYSVARNAME",
	"EXCLAMATION",
	"LCURLY",
	"RCURLY",
	"\"inherits\"",
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
	"ASTERIX",
	"DOT",
	"STRING_LITERAL",
	"POW",
	"MATRIX_OP1",
	"MATRIX_OP2",
	"\"mod\"",
	"PLUS",
	"MINUS",
	"LTMARK",
	"GTMARK",
	"\"not\"",
	"\"eq\"",
	"\"ne\"",
	"\"le\"",
	"\"lt\"",
	"\"ge\"",
	"\"gt\"",
	"\"and\"",
	"\"or\"",
	"\"xor\"",
	"LOG_AND",
	"LOG_OR",
	"LOG_NEG",
	"QUESTION",
	"STRING",
	"INCLUDE",
	"EOL",
	"W",
	"D",
	"L",
	"H",
	"O",
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
	0
};

const unsigned long GDLParser::_tokenSet_0_data_[] = { 2UL, 2847932416UL, 65527UL, 4266966384UL, 1635778559UL, 8194UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF "function" "pro" IDENTIFIER "begin" "switch" "else" "case" END_U 
// "forward_function" "end" "compile_opt" "common" "endif" "endelse" "endcase" 
// "endswitch" "endfor" "endwhile" "endrep" DEC INC "break" "continue" 
// "repeat" "while" "for" "goto" "on_ioerror" "if" LBRACE LSQUARE SYSVARNAME 
// LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT 
// CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI 
// CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT 
// CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT 
// CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT 
// CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI 
// CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE ASTERIX STRING_LITERAL 
// PLUS MINUS "not" LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_0(_tokenSet_0_data_,12);
const unsigned long GDLParser::_tokenSet_1_data_[] = { 0UL, 16777216UL, 0UL, 1081344UL, 4194304UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER LBRACE SYSVARNAME ASTERIX 
const antlr::BitSet GDLParser::_tokenSet_1(_tokenSet_1_data_,12);
const unsigned long GDLParser::_tokenSet_2_data_[] = { 0UL, 16777216UL, 4294950920UL, 4266950671UL, 1644167167UL, 8194UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER EQUAL DEC INC AND_OP_EQ ASTERIX_EQ EQ_OP_EQ GE_OP_EQ GTMARK_EQ 
// GT_OP_EQ LE_OP_EQ LTMARK_EQ LT_OP_EQ MATRIX_OP1_EQ MATRIX_OP2_EQ MINUS_EQ 
// MOD_OP_EQ NE_OP_EQ OR_OP_EQ PLUS_EQ POW_EQ SLASH_EQ XOR_OP_EQ MEMBER 
// LBRACE LSQUARE SYSVARNAME LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG 
// CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG 
// CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE 
// CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I CONSTANT_ULONG 
// CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG 
// CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG 
// CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT 
// CONSTANT_DOUBLE ASTERIX DOT STRING_LITERAL PLUS MINUS "not" LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_2(_tokenSet_2_data_,12);
const unsigned long GDLParser::_tokenSet_3_data_[] = { 0UL, 2214592512UL, 2UL, 128UL, 0UL, 0UL, 0UL, 0UL };
// COMMA "else" END_U "until" 
const antlr::BitSet GDLParser::_tokenSet_3(_tokenSet_3_data_,8);
const unsigned long GDLParser::_tokenSet_4_data_[] = { 0UL, 2164260864UL, 49152UL, 4266950656UL, 1635778559UL, 8194UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "else" DEC INC LBRACE LSQUARE SYSVARNAME LCURLY CONSTANT_HEX_BYTE 
// CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I 
// CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT 
// CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I 
// CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE 
// CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I 
// CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT 
// CONSTANT_FLOAT CONSTANT_DOUBLE ASTERIX STRING_LITERAL PLUS MINUS "not" 
// LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_4(_tokenSet_4_data_,12);
const unsigned long GDLParser::_tokenSet_5_data_[] = { 0UL, 687865856UL, 49253UL, 1097072UL, 4194304UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER "begin" "switch" "case" "forward_function" "compile_opt" 
// "common" DEC INC "break" "continue" "repeat" "while" "for" "goto" "on_ioerror" 
// "if" LBRACE SYSVARNAME ASTERIX 
const antlr::BitSet GDLParser::_tokenSet_5(_tokenSet_5_data_,12);
const unsigned long GDLParser::_tokenSet_6_data_[] = { 0UL, 754974720UL, 4294951023UL, 4266966399UL, 1644167167UL, 8194UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER COMMA "begin" "switch" "case" END_U "forward_function" EQUAL 
// "compile_opt" "common" DEC INC AND_OP_EQ ASTERIX_EQ EQ_OP_EQ GE_OP_EQ 
// GTMARK_EQ GT_OP_EQ LE_OP_EQ LTMARK_EQ LT_OP_EQ MATRIX_OP1_EQ MATRIX_OP2_EQ 
// MINUS_EQ MOD_OP_EQ NE_OP_EQ OR_OP_EQ PLUS_EQ POW_EQ SLASH_EQ XOR_OP_EQ 
// MEMBER "break" "continue" "repeat" "while" "for" "goto" "on_ioerror" 
// "if" LBRACE LSQUARE SYSVARNAME LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG 
// CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG 
// CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE 
// CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I CONSTANT_ULONG 
// CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG 
// CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG 
// CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT 
// CONSTANT_DOUBLE ASTERIX DOT STRING_LITERAL PLUS MINUS "not" LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_6(_tokenSet_6_data_,12);
const unsigned long GDLParser::_tokenSet_7_data_[] = { 2UL, 2847932416UL, 65527UL, 4267097456UL, 1635778559UL, 8194UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// EOF "function" "pro" IDENTIFIER "begin" "switch" "else" "case" END_U 
// "forward_function" "end" "compile_opt" "common" "endif" "endelse" "endcase" 
// "endswitch" "endfor" "endwhile" "endrep" DEC INC "break" "continue" 
// "repeat" "while" "for" "goto" "on_ioerror" "if" LBRACE SLASH LSQUARE 
// SYSVARNAME LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 
// CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 
// CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 
// CONSTANT_INT CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI 
// CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 
// CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 
// CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE ASTERIX 
// STRING_LITERAL PLUS MINUS "not" LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_7(_tokenSet_7_data_,12);
const unsigned long GDLParser::_tokenSet_8_data_[] = { 0UL, 16777216UL, 49152UL, 4266950656UL, 1635778559UL, 8194UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER DEC INC LBRACE LSQUARE SYSVARNAME LCURLY CONSTANT_HEX_BYTE 
// CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I 
// CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT 
// CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I 
// CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE 
// CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I 
// CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT 
// CONSTANT_FLOAT CONSTANT_DOUBLE ASTERIX STRING_LITERAL PLUS MINUS "not" 
// LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_8(_tokenSet_8_data_,12);
const unsigned long GDLParser::_tokenSet_9_data_[] = { 0UL, 2231369728UL, 49154UL, 4269244552UL, 4294967295UL, 32767UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER COMMA "else" END_U DEC INC MEMBER "until" LBRACE RBRACE SLASH 
// LSQUARE SYSVARNAME EXCLAMATION LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG 
// CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG 
// CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE 
// CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I CONSTANT_ULONG 
// CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG 
// CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG 
// CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT 
// CONSTANT_DOUBLE ASTERIX DOT STRING_LITERAL POW MATRIX_OP1 MATRIX_OP2 
// "mod" PLUS MINUS LTMARK GTMARK "not" "eq" "ne" "le" "lt" "ge" "gt" "and" 
// "or" "xor" LOG_AND LOG_OR LOG_NEG QUESTION 
const antlr::BitSet GDLParser::_tokenSet_9(_tokenSet_9_data_,12);
const unsigned long GDLParser::_tokenSet_10_data_[] = { 0UL, 83886080UL, 49152UL, 4277567496UL, 4294967295UL, 32767UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER COMMA DEC INC MEMBER LBRACE SLASH LSQUARE SYSVARNAME EXCLAMATION 
// LCURLY RCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 
// CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 
// CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 
// CONSTANT_INT CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI 
// CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 
// CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 
// CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE ASTERIX 
// DOT STRING_LITERAL POW MATRIX_OP1 MATRIX_OP2 "mod" PLUS MINUS LTMARK 
// GTMARK "not" "eq" "ne" "le" "lt" "ge" "gt" "and" "or" "xor" LOG_AND 
// LOG_OR LOG_NEG QUESTION 
const antlr::BitSet GDLParser::_tokenSet_10(_tokenSet_10_data_,12);
const unsigned long GDLParser::_tokenSet_11_data_[] = { 0UL, 16777216UL, 49152UL, 4269047808UL, 1635778559UL, 8194UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER DEC INC LBRACE LSQUARE SYSVARNAME EXCLAMATION LCURLY CONSTANT_HEX_BYTE 
// CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT CONSTANT_HEX_I 
// CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI CONSTANT_HEX_UINT 
// CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT CONSTANT_I 
// CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT CONSTANT_OCT_BYTE 
// CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT CONSTANT_OCT_I 
// CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI CONSTANT_OCT_UINT 
// CONSTANT_FLOAT CONSTANT_DOUBLE ASTERIX STRING_LITERAL PLUS MINUS "not" 
// LOG_NEG 
const antlr::BitSet GDLParser::_tokenSet_11(_tokenSet_11_data_,12);
const unsigned long GDLParser::_tokenSet_12_data_[] = { 0UL, 352321536UL, 49152UL, 4269768712UL, 4294967295UL, 32767UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER COMMA COLON DEC INC MEMBER LBRACE RBRACE SLASH LSQUARE RSQUARE 
// SYSVARNAME EXCLAMATION LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 
// CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 
// CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 
// CONSTANT_INT CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI 
// CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 
// CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 
// CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE ASTERIX 
// DOT STRING_LITERAL POW MATRIX_OP1 MATRIX_OP2 "mod" PLUS MINUS LTMARK 
// GTMARK "not" "eq" "ne" "le" "lt" "ge" "gt" "and" "or" "xor" LOG_AND 
// LOG_OR LOG_NEG QUESTION 
const antlr::BitSet GDLParser::_tokenSet_12(_tokenSet_12_data_,12);
const unsigned long GDLParser::_tokenSet_13_data_[] = { 0UL, 83886080UL, 49152UL, 4269768712UL, 4294967295UL, 32767UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER COMMA DEC INC MEMBER LBRACE RBRACE SLASH LSQUARE RSQUARE 
// SYSVARNAME EXCLAMATION LCURLY CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 
// CONSTANT_HEX_INT CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 
// CONSTANT_HEX_UI CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 
// CONSTANT_INT CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI 
// CONSTANT_UINT CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 
// CONSTANT_OCT_INT CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 
// CONSTANT_OCT_UI CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE ASTERIX 
// DOT STRING_LITERAL POW MATRIX_OP1 MATRIX_OP2 "mod" PLUS MINUS LTMARK 
// GTMARK "not" "eq" "ne" "le" "lt" "ge" "gt" "and" "or" "xor" LOG_AND 
// LOG_OR LOG_NEG QUESTION 
const antlr::BitSet GDLParser::_tokenSet_13(_tokenSet_13_data_,12);
const unsigned long GDLParser::_tokenSet_14_data_[] = { 0UL, 3573547008UL, 49154UL, 4276077192UL, 4294967295UL, 32767UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// IDENTIFIER COMMA COLON "of" "else" END_U DEC INC MEMBER "until" "do" 
// "then" LBRACE RBRACE SLASH LSQUARE RSQUARE SYSVARNAME LCURLY RCURLY 
// CONSTANT_HEX_BYTE CONSTANT_HEX_LONG CONSTANT_HEX_LONG64 CONSTANT_HEX_INT 
// CONSTANT_HEX_I CONSTANT_HEX_ULONG CONSTANT_HEX_ULONG64 CONSTANT_HEX_UI 
// CONSTANT_HEX_UINT CONSTANT_BYTE CONSTANT_LONG CONSTANT_LONG64 CONSTANT_INT 
// CONSTANT_I CONSTANT_ULONG CONSTANT_ULONG64 CONSTANT_UI CONSTANT_UINT 
// CONSTANT_OCT_BYTE CONSTANT_OCT_LONG CONSTANT_OCT_LONG64 CONSTANT_OCT_INT 
// CONSTANT_OCT_I CONSTANT_OCT_ULONG CONSTANT_OCT_ULONG64 CONSTANT_OCT_UI 
// CONSTANT_OCT_UINT CONSTANT_FLOAT CONSTANT_DOUBLE ASTERIX DOT STRING_LITERAL 
// POW MATRIX_OP1 MATRIX_OP2 "mod" PLUS MINUS LTMARK GTMARK "not" "eq" 
// "ne" "le" "lt" "ge" "gt" "and" "or" "xor" LOG_AND LOG_OR LOG_NEG QUESTION 
const antlr::BitSet GDLParser::_tokenSet_14(_tokenSet_14_data_,12);
const unsigned long GDLParser::_tokenSet_15_data_[] = { 0UL, 0UL, 0UL, 131072UL, 473956352UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// SLASH ASTERIX MATRIX_OP1 MATRIX_OP2 "mod" 
const antlr::BitSet GDLParser::_tokenSet_15(_tokenSet_15_data_,12);


