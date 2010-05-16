/* $ANTLR 2.7.6 (20071205): "gdlc.i.g" -> "GDLInterpreter.cpp"$ */

    // gets inserted before the antlr generated includes in the cpp file
#include "includefirst.hpp"

#include "GDLInterpreter.hpp"
#include <antlr/Token.hpp>
#include <antlr/AST.hpp>
#include <antlr/NoViableAltException.hpp>
#include <antlr/MismatchedTokenException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/BitSet.hpp>

    // gets inserted after the antlr generated includes in the cpp file
#include "dinterpreter.hpp"
#include "prognodeexpr.hpp"

#include <cassert>

// tweaking ANTLR
#define ASTNULL          NULLProgNodeP
#define ProgNodeP( xxx ) NULL             /* ProgNodeP(antlr::nullAST) */
#define RefAST( xxx)     ConvertAST( xxx) /* antlr::RefAST( Ref type)  */
#define match( a, b)     /* remove from source */

using namespace std;

GDLInterpreter::GDLInterpreter()
	: antlr::TreeParser() {
}

 RetCode  GDLInterpreter::interactive(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP interactive_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		for (; _t != NULL;) {
	
	_t->setLine(0);
			retCode=statement(_t);
			_t = _retTree;
				
			if( retCode != RC_OK) break; // break out if non-regular
		}
		_retTree = _t;
		return retCode;
	
	
	{ // ( ... )+
	int _cnt3=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			retCode=statement(_t);
			_t = _retTree;
			
			if( retCode != RC_OK) break; // break out if non-regular
			
		}
		else {
			if ( _cnt3>=1 ) { goto _loop3; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt3++;
	}
	_loop3:;
	}  // ( ... )+
	_retTree = _t;
	return retCode;
}

 RetCode  GDLInterpreter::statement(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	//    ProgNodeP& actPos = statement_AST_in;
	assert( _t != NULL);
	ProgNodeP last;
	_retTree = _t;
	//  if( callStack.back()->GetLineNumber() == 0) 
	//  if( _t->getLine() != 0) 
	//      callStack.back()->SetLineNumber( _t->getLine());
	
	
	try {      // for error handling
		
		do {
		//                 if( _t->getLine() != 0) 
		//                     callStack.back()->SetLineNumber( _t->getLine());
		
		last = _retTree;
		
		retCode = last->Run(); // Run() sets _retTree
		
		}
		while( 
		_retTree != NULL && 
		retCode == RC_OK && 
		!(sigControlC && interruptEnable) && 
		(debugMode == DEBUG_CLEAR));
		
		if( _retTree != NULL) 
		last = _retTree;
		
		goto afterStatement;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		{
			ProgNodeP tmp1_AST_in = _t;
			match(antlr::RefAST(_t),ASSIGN);
			_t = _t->getNextSibling();
			break;
		}
		case ASSIGN_ARRAYEXPR_MFCALL:
		{
			ProgNodeP tmp2_AST_in = _t;
			match(antlr::RefAST(_t),ASSIGN_ARRAYEXPR_MFCALL);
			_t = _t->getNextSibling();
			break;
		}
		case ASSIGN_REPLACE:
		{
			ProgNodeP tmp3_AST_in = _t;
			match(antlr::RefAST(_t),ASSIGN_REPLACE);
			_t = _t->getNextSibling();
			break;
		}
		case PCALL_LIB:
		{
			ProgNodeP tmp4_AST_in = _t;
			match(antlr::RefAST(_t),PCALL_LIB);
			_t = _t->getNextSibling();
			break;
		}
		case MPCALL:
		{
			ProgNodeP tmp5_AST_in = _t;
			match(antlr::RefAST(_t),MPCALL);
			_t = _t->getNextSibling();
			break;
		}
		case MPCALL_PARENT:
		{
			ProgNodeP tmp6_AST_in = _t;
			match(antlr::RefAST(_t),MPCALL_PARENT);
			_t = _t->getNextSibling();
			break;
		}
		case PCALL:
		{
			ProgNodeP tmp7_AST_in = _t;
			match(antlr::RefAST(_t),PCALL);
			_t = _t->getNextSibling();
			break;
		}
		case DEC:
		{
			ProgNodeP tmp8_AST_in = _t;
			match(antlr::RefAST(_t),DEC);
			_t = _t->getNextSibling();
			break;
		}
		case INC:
		{
			ProgNodeP tmp9_AST_in = _t;
			match(antlr::RefAST(_t),INC);
			_t = _t->getNextSibling();
			break;
		}
		case FOR:
		{
			ProgNodeP tmp10_AST_in = _t;
			match(antlr::RefAST(_t),FOR);
			_t = _t->getNextSibling();
			break;
		}
		case FOR_LOOP:
		{
			ProgNodeP tmp11_AST_in = _t;
			match(antlr::RefAST(_t),FOR_LOOP);
			_t = _t->getNextSibling();
			break;
		}
		case FOREACH:
		{
			ProgNodeP tmp12_AST_in = _t;
			match(antlr::RefAST(_t),FOREACH);
			_t = _t->getNextSibling();
			break;
		}
		case FOREACH_LOOP:
		{
			ProgNodeP tmp13_AST_in = _t;
			match(antlr::RefAST(_t),FOREACH_LOOP);
			_t = _t->getNextSibling();
			break;
		}
		case FOR_STEP:
		{
			ProgNodeP tmp14_AST_in = _t;
			match(antlr::RefAST(_t),FOR_STEP);
			_t = _t->getNextSibling();
			break;
		}
		case FOR_STEP_LOOP:
		{
			ProgNodeP tmp15_AST_in = _t;
			match(antlr::RefAST(_t),FOR_STEP_LOOP);
			_t = _t->getNextSibling();
			break;
		}
		case REPEAT:
		{
			ProgNodeP tmp16_AST_in = _t;
			match(antlr::RefAST(_t),REPEAT);
			_t = _t->getNextSibling();
			break;
		}
		case REPEAT_LOOP:
		{
			ProgNodeP tmp17_AST_in = _t;
			match(antlr::RefAST(_t),REPEAT_LOOP);
			_t = _t->getNextSibling();
			break;
		}
		case WHILE:
		{
			ProgNodeP tmp18_AST_in = _t;
			match(antlr::RefAST(_t),WHILE);
			_t = _t->getNextSibling();
			break;
		}
		case IF:
		{
			ProgNodeP tmp19_AST_in = _t;
			match(antlr::RefAST(_t),IF);
			_t = _t->getNextSibling();
			break;
		}
		case IF_ELSE:
		{
			ProgNodeP tmp20_AST_in = _t;
			match(antlr::RefAST(_t),IF_ELSE);
			_t = _t->getNextSibling();
			break;
		}
		case CASE:
		{
			ProgNodeP tmp21_AST_in = _t;
			match(antlr::RefAST(_t),CASE);
			_t = _t->getNextSibling();
			break;
		}
		case SWITCH:
		{
			ProgNodeP tmp22_AST_in = _t;
			match(antlr::RefAST(_t),SWITCH);
			_t = _t->getNextSibling();
			break;
		}
		case BLOCK:
		{
			ProgNodeP tmp23_AST_in = _t;
			match(antlr::RefAST(_t),BLOCK);
			_t = _t->getNextSibling();
			break;
		}
		case LABEL:
		{
			ProgNodeP tmp24_AST_in = _t;
			match(antlr::RefAST(_t),LABEL);
			_t = _t->getNextSibling();
			break;
		}
		case ON_IOERROR_NULL:
		{
			ProgNodeP tmp25_AST_in = _t;
			match(antlr::RefAST(_t),ON_IOERROR_NULL);
			_t = _t->getNextSibling();
			break;
		}
		case ON_IOERROR:
		{
			ProgNodeP tmp26_AST_in = _t;
			match(antlr::RefAST(_t),ON_IOERROR);
			_t = _t->getNextSibling();
			break;
		}
		case BREAK:
		{
			ProgNodeP tmp27_AST_in = _t;
			match(antlr::RefAST(_t),BREAK);
			_t = _t->getNextSibling();
			break;
		}
		case CONTINUE:
		{
			ProgNodeP tmp28_AST_in = _t;
			match(antlr::RefAST(_t),CONTINUE);
			_t = _t->getNextSibling();
			break;
		}
		case GOTO:
		{
			ProgNodeP tmp29_AST_in = _t;
			match(antlr::RefAST(_t),GOTO);
			_t = _t->getNextSibling();
			break;
		}
		case RETF:
		{
			ProgNodeP tmp30_AST_in = _t;
			match(antlr::RefAST(_t),RETF);
			_t = _t->getNextSibling();
			break;
		}
		case RETP:
		{
			ProgNodeP tmp31_AST_in = _t;
			match(antlr::RefAST(_t),RETP);
			_t = _t->getNextSibling();
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		afterStatement:;
		
		// possible optimization: make sigControlC a debugMode 
		if( interruptEnable && sigControlC)
		{
		DebugMsg( last, "Interrupted at: "); 
		
		sigControlC = false;
		
		retCode = NewInterpreterInstance( last->getLine()-1);
		}
		else if( debugMode != DEBUG_CLEAR)
		{
		if( debugMode == DEBUG_STOP)
		{
		DebugMsg( last, "Stop encoutered: ");
		if( !interruptEnable)
		debugMode = DEBUG_PROCESS_STOP;
		}
		
		if( interruptEnable)
		{
		if( debugMode == DEBUG_PROCESS_STOP)
		{
		DebugMsg( last, "Stepped to: ");
		}
		
		debugMode = DEBUG_CLEAR;
		
		retCode = NewInterpreterInstance( last->getLine()-1);
		}   
		else
		{
		retCode = RC_ABORT;
		}
		}
		return retCode;
		
	}
	catch ( GDLException& e) {
		
		if( dynamic_cast< GDLIOException*>( &e) != NULL)
		{
		// set the jump target - also logs the jump
		ProgNodeP onIOErr = 
		static_cast<EnvUDT*>(callStack.back())->GetIOError();
		if( onIOErr != NULL)
		{
		SysVar::SetErr_String( e.getMessage());
		
		_retTree = onIOErr;
		return RC_OK;
		}
		}
		
		EnvUDT* targetEnv = e.GetTargetEnv();
		if( targetEnv == NULL)
		{
		// initial exception, set target env
		// look if ON_ERROR is set somewhere
		for( EnvStackT::reverse_iterator i = callStack.rbegin();
		i != callStack.rend(); ++i)
		{
		DLong oE = static_cast<EnvUDT*>(*i)->GetOnError();
		
		if( oE != -1) 
		{ // oE was set
		
		// 0 -> stop here
		if( oE == 0) 
		targetEnv = static_cast<EnvUDT*>(callStack.back()); 
		// 1 -> $MAIN$
		else if( oE == 1) 
		{
		EnvUDT* cS_begin = 
		static_cast<EnvUDT*>(*callStack.begin());
		targetEnv = cS_begin;  
		}
		// 2 -> caller of routine which called ON_ERROR
		else if( oE == 2)
		{
		// set to caller, handle nested
		while( static_cast<EnvUDT*>(*(++i))->GetOnError() == 2 
		&& i != callStack.rend());
		
		if( i == callStack.rend())
		{
		EnvUDT* cS_begin = 
		static_cast<EnvUDT*>(*callStack.begin());
		targetEnv = cS_begin;
		}
		else
		{
		EnvUDT* iUDT = static_cast<EnvUDT*>(*i);
		targetEnv = iUDT;
		}
		}   
		// 3 -> routine which called ON_ERROR
		else if( oE == 3)
		{
		EnvUDT* iUDT = static_cast<EnvUDT*>(*i);
		targetEnv = iUDT;
		}
		
		
		// State where error occured
		//                     if( e.getLine() == 0 && _t != NULL)
		//                         e.SetLine( _t->getLine());
		//                     if( e.getLine() == 0 && _retTree != NULL)
		//                         e.SetLine( _retTree->getLine());
		if( e.getLine() == 0 && last != NULL)
		e.SetLine( last->getLine());
		
		if( interruptEnable)
		ReportError(e, "Error occurred at:");
		
		// remeber where to stop
		e.SetTargetEnv( targetEnv);
		
		if( targetEnv->GetLineNumber() != 0)
		e.SetLine( targetEnv->GetLineNumber());                    
		
		//                     ProgNodeP errorNodeP = targetEnv->CallingNode();
		//                     e.SetErrorNodeP( errorNodeP);
		
		// break on first occurence of set oE
		break;
		}
		}
		}
		
		if( targetEnv != NULL && targetEnv != callStack.back())
		{
		throw e; // rethrow
		}
		lib::write_journal( GetClearActualLine());
		
		// many low level routines don't have errorNode info
		// set line number here in this case
		//         if( e.getLine() == 0 && _t != NULL)
		//             e.SetLine( _t->getLine());
		//         if( e.getLine() == 0 && _retTree != NULL)
		//             e.SetLine( _retTree->getLine());
		//        if( e.getLine() == 0 && actPos != NULL)
		//            e.SetLine( actPos->getLine());
		
		if( interruptEnable)
		{
		// tell where we are
		ReportError(e, "Execution halted at:", targetEnv == NULL); 
		
		retCode = NewInterpreterInstance(e.getLine()-1);
		}    
		else
		{
		
		DString msgPrefix = SysVar::MsgPrefix();
		if( e.Prefix())
		{
		std::cerr << msgPrefix << e.toString() << std::endl;
		lib::write_journal_comment(msgPrefix+e.toString());
		}
		else
		{
		std::cerr << e.toString() << std::endl;
		lib::write_journal_comment(e.toString());
		}
		
		retCode = RC_ABORT;
		}
		
		return retCode;
		
	}
	_retTree = _t;
	return retCode;
}

 RetCode  GDLInterpreter::execute(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP execute_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	//    RetCode retCode;
	ValueGuard<bool> guard( interruptEnable);
	interruptEnable = false;
	
		return statement_list(_t);
	
	
	retCode=statement_list(_t);
	_t = _retTree;
	_retTree = _t;
	return retCode;
}

 RetCode  GDLInterpreter::statement_list(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP statement_list_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		for (; _t != NULL;) {
	
			retCode=statement(_t);
			_t = _retTree;
				
			if( retCode != RC_OK) break; // break out if non-regular
		}
		_retTree = _t;
		return retCode;
	
	
	{ // ( ... )+
	int _cnt16=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			retCode=statement(_t);
			_t = _retTree;
			
			if( retCode != RC_OK) break; // break out if non-regular
			
		}
		else {
			if ( _cnt16>=1 ) { goto _loop16; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt16++;
	}
	_loop16:;
	}  // ( ... )+
	_retTree = _t;
	return retCode;
}

 BaseGDL*  GDLInterpreter::call_fun(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP call_fun_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	
	res = NULL;
	returnValue = NULL;
	RetCode retCode;
	
		for (; _t != NULL;) {
	
				retCode=statement(_t);
				
	// 			if( retCode == RC_RETURN) 
				if( retCode >= RC_RETURN) 
				{
				res=returnValue;
				returnValue=NULL;
				
				break;
				}					
	
			_t = _retTree;
		}
		
		// default return value if none was set
		if( res == NULL) res = new DIntGDL( 0); 
		
		_retTree = _t;
		return res;
	
	
	{ // ( ... )*
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			retCode=statement(_t);
			_t = _retTree;
			
			//                if( retCode == RC_RETURN) 
			if( retCode >= RC_RETURN) 
			{
			res=returnValue;
			returnValue=NULL;
			
			break;
			}
			
		}
		else {
			goto _loop7;
		}
		
	}
	_loop7:;
	} // ( ... )*
	
	// default return value if none was set
	if( res == NULL) res = new DIntGDL( 0); 
	
	_retTree = _t;
	return res;
}

 BaseGDL**  GDLInterpreter::call_lfun(ProgNodeP _t) {
	 BaseGDL** res;
	ProgNodeP call_lfun_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res = NULL;
	returnValueL = NULL;
	RetCode retCode;
	
		ProgNodeP in = _t;
	
		for (; _t != NULL;) {
				retCode=statement(_t);
				_t = _retTree;
				
	//			if( retCode == RC_RETURN) 
				if( retCode >= RC_RETURN) 
				{
				res=returnValueL;
				returnValueL=NULL;
				break;
				}
				
		}
		
		// default return value if none was set
		if( res == NULL)
		throw GDLException( in, "Function "+
		callStack.back()->GetProName()+
		" must return a left-value in this context.",false,false);
		
		_retTree = _t;
		return res;
	
	
	{ // ( ... )*
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			retCode=statement(_t);
			_t = _retTree;
			
			//                if( retCode == RC_RETURN) 
			if( retCode >= RC_RETURN) 
			{
			res=returnValueL;
			returnValueL=NULL;
			break;
			}
			
		}
		else {
			goto _loop10;
		}
		
	}
	_loop10:;
	} // ( ... )*
	
	// default return value if none was set
	if( res == NULL)
	throw GDLException( call_lfun_AST_in, "Function "+
	callStack.back()->GetProName()+
	" must return a left-value in this context.",false,false);
	
	_retTree = _t;
	return res;
}

void GDLInterpreter::call_pro(ProgNodeP _t) {
	ProgNodeP call_pro_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	RetCode retCode;
	
		for (; _t != NULL;) {
				retCode=statement(_t);
				_t = _retTree;
				
				// added RC_ABORT here
				if( retCode >= RC_RETURN) break;
		}
		_retTree = _t;
	return;
	
	
	{ // ( ... )*
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			retCode=statement(_t);
			_t = _retTree;
			
			// added RC_ABORT here
			if( retCode >= RC_RETURN) break;
			
		}
		else {
			goto _loop13;
		}
		
	}
	_loop13:;
	} // ( ... )*
	_retTree = _t;
}

 RetCode  GDLInterpreter::block(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP block_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		match(antlr::RefAST(_t),BLOCK);
		_retTree = _t->getFirstChild();
	return RC_OK;
	//     retCode = RC_OK;
	
	// 	ProgNodeP block = _t;
	// 	match(antlr::RefAST(_t),BLOCK);
	// 	_t = _t->getFirstChild();
	// 	if (_t != NULL)
	// 		{
	
	//             SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	//             retCode=statement_list(_t);
	
	//             if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	//                 !block->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	//                 {
	//                     // a jump (goto) occured out of this block
	//                     return retCode;
	//                 }
	// 		}
	// 	_retTree = block->getNextSibling();
	// 	return retCode;
	
	
	ProgNodeP __t20 = _t;
	ProgNodeP tmp32_AST_in = _t;
	match(antlr::RefAST(_t),BLOCK);
	_t = _t->getFirstChild();
	{
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	case ASSIGN_REPLACE:
	case ASSIGN_ARRAYEXPR_MFCALL:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case FOR:
	case FOR_STEP:
	case FOREACH:
	case FOR_LOOP:
	case FOR_STEP_LOOP:
	case FOREACH_LOOP:
	case IF_ELSE:
	case LABEL:
	case MPCALL:
	case MPCALL_PARENT:
	case ON_IOERROR_NULL:
	case PCALL:
	case PCALL_LIB:
	case REPEAT:
	case REPEAT_LOOP:
	case RETF:
	case RETP:
	case WHILE:
	case CASE:
	case GOTO:
	case IF:
	case ON_IOERROR:
	case SWITCH:
	case DEC:
	case INC:
	{
		retCode=statement_list(_t);
		_t = _retTree;
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
	_t = __t20;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 RetCode  GDLInterpreter::switch_statement(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP switch_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP s = ProgNodeP(antlr::nullAST);
	
	BaseGDL* e;
	retCode = RC_OK; // not set if no branch is executed
	
	
	ProgNodeP __t23 = _t;
	s = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),SWITCH);
	_t = _t->getFirstChild();
	e=expr(_t);
	_t = _retTree;
	
	auto_ptr<BaseGDL> e_guard(e);
	
	//                SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	ProgNodeP b=_t; // remeber block begin (block)
	
	bool hook=false; // switch executes everything after 1st match
	for( int i=0; i<s->numBranch; i++)
	{
	if( b->getType() == ELSEBLK)
	{
	hook=true;
	
	ProgNodeP sL = b->GetFirstChild(); // statement_list
	
	if(sL != NULL )
	{
	_retTree = sL;
	return RC_OK;
	}
	
	//                             // statement there
	//                             retCode=statement_list( sL);
	//                             if( retCode == RC_BREAK) 
	//                             {
	//                                 retCode = RC_OK;    
	//                                 break;          // break
	//                             }
	//                             if( retCode >= RC_RETURN) break; // goto
	
	//                             if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	//                                 !s->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	//                             {
	//                                 // a jump (goto) occured out of this loop
	//                                 return retCode;
	//                             }
	
	//                         }
	}
	else
	{
	ProgNodeP ex = b->GetFirstChild();  // EXPR
	ProgNodeP bb = ex->GetNextSibling(); // statement_list
	
	if( !hook)
	{
	//                            RefDNode ee_ = _t->GetFirstChild(); // expr
	
	BaseGDL* ee=expr(ex);
	// auto_ptr<BaseGDL> ee_guard(ee);
	
	hook=e->Equal(ee); // Equal deletes ee
	}
	
	if( hook)
	{
	// statement there
	if(bb != NULL )
	{
	_retTree = bb;
	return RC_OK;
	}
	
	//                             _retTree = // find first non empty
	//                             return RC_OK;
	//                                 }
	
	//                             retCode=statement_list(bb);
	//                             if( retCode == RC_BREAK) 
	//                             {
	//                                 retCode = RC_OK;    
	//                                 break;          // break
	//                             }
	//                             if( retCode >= RC_RETURN) break; // goto
	
	//                             if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	//                                 !s->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	//                             {
	//                                 // a jump (goto) occured out of this loop
	//                                 return retCode;
	//                             }
	}
	
	}
	b=b->GetNextSibling(); // next block
	}
	_retTree = s->GetNextSibling();
	return RC_OK;
	// finish or break
	//                retCode=RC_OK; // clear RC_BREAK retCode
	
	_t = __t23;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

BaseGDL*  GDLInterpreter::expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		
		assert( _t != NULL);
	
		if ( _t->getType() == FCALL_LIB) {
	BaseGDL* res=lib_function_call(_t);
			
			if( callStack.back()->Contains( res)) 
			res = res->Dup();
	
	return res;    		
		}
		else
		{
			return tmp_expr(_t);
		}
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	case ASSIGN_REPLACE:
	case ASSIGN_ARRAYEXPR_MFCALL:
	case ARRAYDEF:
	case ARRAYEXPR:
	case ARRAYEXPR_MFCALL:
	case CONSTANT:
	case DEREF:
	case EXPR:
	case FCALL:
	case FCALL_LIB_RETNEW:
	case MFCALL:
	case MFCALL_PARENT:
	case NSTRUC:
	case NSTRUC_REF:
	case POSTDEC:
	case POSTINC:
	case STRUC:
	case SYSVAR:
	case VAR:
	case VARPTR:
	case DEC:
	case INC:
	case DOT:
	case QUESTION:
	{
		res=tmp_expr(_t);
		_t = _retTree;
		break;
	}
	case FCALL_LIB:
	{
		res=lib_function_call(_t);
		_t = _retTree;
		
		if( callStack.back()->Contains( res)) 
		res = res->Dup();
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

 RetCode  GDLInterpreter::case_statement(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP case_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP c = ProgNodeP(antlr::nullAST);
	
	BaseGDL* e;
	retCode = RC_OK; // not set if no branch is executed
	
	
	ProgNodeP __t25 = _t;
	c = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),CASE);
	_t = _t->getFirstChild();
	e=expr(_t);
	_t = _retTree;
	
	auto_ptr<BaseGDL> e_guard(e);
	
	//                SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	if( !e->Scalar())
	throw GDLException( _t, "Expression must be a"
	" scalar in this context: "+Name(e),true,false);
	
	ProgNodeP b=_t; // remeber block begin
	
	for( int i=0; i<c->numBranch; ++i)
	{
	if( b->getType() == ELSEBLK)
	{
	ProgNodeP sL = b->GetFirstChild(); // statement_list
	
	if(sL != NULL )
	{
	_retTree = sL;
	return RC_OK;
	}
	else
	{
	_retTree = c->GetNextSibling();
	return RC_OK;
	}
	
	//                             // statement there
	//                             retCode=statement_list(sL);
	//                             //if( retCode == RC_BREAK) break; // break anyway
	// //                            if( retCode >= RC_RETURN) return retCode; 
	//                             if( retCode >= RC_RETURN) break;
	
	//                             if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	//                                 !c->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	//                             {
	//                                 // a jump (goto) occured out of this loop
	//                                 return retCode;
	//                             }
	
	//                         }
	//                         retCode = RC_OK;
	//                         break;
	}
	else
	{
	ProgNodeP ex = b->GetFirstChild();  // EXPR
	ProgNodeP bb = ex->GetNextSibling(); // statement_list
	
	BaseGDL* ee=expr(ex);
	// auto_ptr<BaseGDL> ee_guard(ee);
	
	bool equalexpr=e->Equal(ee); // Equal deletes ee
	
	if( equalexpr)
	{
	if(bb != NULL )
	{
	_retTree = bb;
	return RC_OK;
	}
	else
	{
	_retTree = c->GetNextSibling();
	return RC_OK;
	}
	//                             if(bb != NULL)
	//                             {
	//                                 // statement there
	//                                 retCode=statement_list(bb);
	//                                 //if( retCode == RC_BREAK) break; // break anyway
	// //                                if( retCode >= RC_RETURN) return retCode;
	//                                 if( retCode >= RC_RETURN) break;
	
	//                                 if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	//                                     !c->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	//                                 {
	//                                     // a jump (goto) occured out of this loop
	//                                     return retCode;
	//                                 }
	
	//                             }
	//                             retCode = RC_OK;
	//                             break;
	}
	
	}
	b=b->GetNextSibling(); // next block
	} // for
	// finish or break
	//                retCode=RC_OK; // clear RC_BREAK retCode
	throw GDLException( c, "CASE statement found no match.",true,false);
	
	_t = __t25;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 RetCode  GDLInterpreter::repeat_statement(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP repeat_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP r = ProgNodeP(antlr::nullAST);
	
	ProgNodeP __t27 = _t;
	r = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),REPEAT);
	_t = _t->getFirstChild();
	
	// _t is REPEAT_LOOP, GetFirstChild() is expr, GetNextSibling is first loop statement
	_retTree = _t->GetFirstChild()->GetNextSibling();     // statement
	return RC_OK;
	
	_t = __t27;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 RetCode  GDLInterpreter::repeat_loop_statement(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP repeat_loop_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP r = ProgNodeP(antlr::nullAST);
	
	ProgNodeP __t29 = _t;
	r = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),REPEAT_LOOP);
	_t = _t->getFirstChild();
	
	auto_ptr<BaseGDL> eVal( expr(_t));
	if( eVal.get()->False())
	{
	_retTree = _t->GetNextSibling();     // 1st loop statement
	if( _retTree == NULL)
	throw GDLException(r,
	"Empty REPEAT loop entered (infinite loop).",
	true,false);
	return RC_OK;
	}
	else
	{
	_retTree = r->GetNextSibling();     // statement
	return RC_OK;
	}
	
	//                 retCode=RC_OK; // clear RC_BREAK/RC_CONTINUE retCode
	//                 SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	//                 // remember block and expr nodes
	//                 ProgNodeP e =_t;
	//                 ProgNodeP bb  = e->GetNextSibling();
	
	// //                 ProgNodeP bb =_t;
	// //                 ProgNodeP e  = bb->GetNextSibling();
	// //                 bb = bb->GetFirstChild();
	
	//                 auto_ptr<BaseGDL> eVal;
	//                 do {
	//                     if( bb != NULL)
	//                     {
	//                     retCode=statement_list(bb);
	
	//                     if( retCode == RC_CONTINUE)
	//                                 {
	//                                 retCode = RC_OK;
	//                                 continue;  
	//                                 }
	//                     if( retCode == RC_BREAK) 
	//                     {
	//                         retCode = RC_OK;
	//                         break;        
	//                     }
	//                     if( retCode >= RC_RETURN) break;
	//                     // if( retCode == RC_BREAK) break;        
	//                     // if( retCode >= RC_RETURN) return retCode;
	
	//                     if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	//                         !r->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	//                     {
	//                         // a jump (goto) occured out of this loop
	//                         return retCode;
	//                     }
	//                     }
	//                     eVal.reset( expr(e));
	//                 } while( eVal.get()->False());
	
	//                 // retCode=RC_OK; // clear RC_BREAK/RC_CONTINUE retCode
	
	_t = __t29;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 RetCode  GDLInterpreter::while_statement(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP while_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP w = ProgNodeP(antlr::nullAST);
	
	retCode = RC_OK;
	
	
	ProgNodeP __t31 = _t;
	w = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),WHILE);
	_t = _t->getFirstChild();
	
	//                 SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	//                 ProgNodeP e = _t; //->GetFirstChild();  // expr
	
	auto_ptr<BaseGDL> eVal( expr( _t));
	if( eVal.get()->True()) 
	{
	_retTree = _t->GetNextSibling();     // 1st loop statement
	if( _retTree == NULL)
	throw GDLException(w,
	"Empty WHILE loop entered (infinite loop).",
	true,false);
	return RC_OK;
	}
	else
	{
	_retTree = w->GetNextSibling();     // statement
	return RC_OK;
	}
	
	//                 auto_ptr< BaseGDL> eVal( expr( e));
	//                 while( eVal.get()->True()) {
	//                     retCode=statement_list(s);
	
	//                     if( retCode == RC_CONTINUE) 
	//                                 {
	//                                 retCode = RC_OK;
	//                                 continue;  
	//                                 }
	//                     if( retCode == RC_BREAK) 
	//                     {
	//                         retCode = RC_OK;
	//                         break;        
	//                     }
	//                     if( retCode >= RC_RETURN) break;
	
	//                     if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	//                         !w->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	//                     {
	//                         // a jump (goto) occured out of this loop
	//                         return retCode;
	//                     }
	
	//                     eVal.reset( expr( e));
	//                 } 
	
	// retCode=RC_OK; // clear RC_BREAK/RC_CONTINUE retCode
	
	_t = __t31;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 RetCode  GDLInterpreter::for_statement(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP for_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP f = ProgNodeP(antlr::nullAST);
	ProgNodeP fl = ProgNodeP(antlr::nullAST);
	ProgNodeP fs = ProgNodeP(antlr::nullAST);
	ProgNodeP fsl = ProgNodeP(antlr::nullAST);
	
	BaseGDL** v;
	BaseGDL* s;
	BaseGDL* e;
	BaseGDL* st;
	retCode = RC_OK;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case FOR:
	{
		ProgNodeP __t33 = _t;
		f = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),FOR);
		_t = _t->getFirstChild();
		
		_t = f->GetNextSibling()->GetFirstChild();
		
		EnvUDT* callStack_back = 
		static_cast<EnvUDT*>(callStack.back());
		
		ForLoopInfoT& loopInfo = 
		callStack_back->GetForLoopInfo( f->forLoopIx);
		
		v=l_simple_var(_t);
		_t = _retTree;
		
		s=expr(_t);
		auto_ptr<BaseGDL> s_guard(s);
		_t = _retTree;
		
		delete loopInfo.endLoopVar;
		loopInfo.endLoopVar=expr(_t);
		
		ProgNodeP b = _retTree;
				
		s->ForCheck( &loopInfo.endLoopVar);
		
		// ASSIGNMENT used here also
		delete (*v);
		(*v)= s_guard.release(); // s held in *v after this
		
		if( (*v)->ForCondUp( loopInfo.endLoopVar))
		{
		_retTree = b;
		return RC_OK;
		}
		else
		{
		// skip if initial test fails
		_retTree = f->GetNextSibling()->GetNextSibling();
		return RC_OK;
		}
		
		_t = __t33;
		_t = _t->getNextSibling();
		break;
	}
	case FOR_LOOP:
	{
		ProgNodeP __t34 = _t;
		fl = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),FOR_LOOP);
		_t = _t->getFirstChild();
		
		EnvUDT* callStack_back = 
		static_cast<EnvUDT*>(callStack.back());
		
		ForLoopInfoT& loopInfo = 
		callStack_back->GetForLoopInfo( fl->forLoopIx);
		
		if( loopInfo.endLoopVar == NULL)
		{
		// non-initialized loop (GOTO)
		_retTree = fl->GetNextSibling();
		return RC_OK;
		}
		
		// // problem:
		// // EXECUTE may call DataListT.loc.resize(), as v points to the
		// // old sequence v might be invalidated -> segfault
		// // note that the value (*v) is preserved by resize()
		
		v=l_simple_var(_t);
		_t = _retTree;
		
		(*v)->ForAdd();
		if( (*v)->ForCondUp( loopInfo.endLoopVar))
		{
		_retTree = _t->GetNextSibling()->GetNextSibling();
		return RC_OK;
		}
		else
		{
		delete loopInfo.endLoopVar;
		loopInfo.endLoopVar = NULL;
		_retTree = fl->GetNextSibling();
		return RC_OK;
		}
		
		
		_t = __t34;
		_t = _t->getNextSibling();
		break;
	}
	case FOR_STEP:
	{
		ProgNodeP __t35 = _t;
		fs = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),FOR_STEP);
		_t = _t->getFirstChild();
		
		_t = fs->GetNextSibling()->GetFirstChild();
		
		EnvUDT* callStack_back = 
		static_cast<EnvUDT*>(callStack.back());
		
		ForLoopInfoT& loopInfo = 
		callStack_back->GetForLoopInfo( fs->forLoopIx);
		
		v=l_simple_var(_t);
		_t = _retTree;
		
		s=expr(_t);
		auto_ptr<BaseGDL> s_guard(s);
		_t = _retTree;
		
		delete loopInfo.endLoopVar;
		loopInfo.endLoopVar=expr(_t);
		_t = _retTree;
		
		delete loopInfo.loopStepVar;
		loopInfo.loopStepVar=expr(_t);
		
		ProgNodeP b = _retTree;
				
		s->ForCheck( &loopInfo.endLoopVar, &loopInfo.loopStepVar);
		
		// ASSIGNMENT used here also
		delete (*v);
		(*v)= s_guard.release(); // s held in *v after this
		
		if( loopInfo.loopStepVar->Sgn() == -1) 
		{
		if( (*v)->ForCondDown( loopInfo.endLoopVar))
		{
		_retTree = b;
		return RC_OK;
		}
		}
		else
		{
		if( (*v)->ForCondUp( loopInfo.endLoopVar))
		{
		_retTree = b;
		return RC_OK;
		}
		}
		
		// skip if initial test fails
		_retTree = f->GetNextSibling()->GetNextSibling();
		return RC_OK;
		
		_t = __t35;
		_t = _t->getNextSibling();
		break;
	}
	case FOR_STEP_LOOP:
	{
		ProgNodeP __t36 = _t;
		fsl = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),FOR_STEP_LOOP);
		_t = _t->getFirstChild();
		
		EnvUDT* callStack_back = 
		static_cast<EnvUDT*>(callStack.back());
		
		ForLoopInfoT& loopInfo = 
		callStack_back->GetForLoopInfo( fsl->forLoopIx);
		
		if( loopInfo.endLoopVar == NULL)
		{
		// non-initialized loop (GOTO)
		_retTree = fsl->GetNextSibling();
		return RC_OK;
		}
		
		// // problem:
		// // EXECUTE may call DataListT.loc.resize(), as v points to the
		// // old sequence v might be invalidated -> segfault
		// // note that the value (*v) is preserved by resize()
		
		v=l_simple_var(_t);
		_t = _retTree;
		
		ProgNodeP b = _t->GetNextSibling()->GetNextSibling()->GetNextSibling();
		
		(*v)->ForAdd(loopInfo.loopStepVar);
		if( loopInfo.loopStepVar->Sgn() == -1) 
		{
		if( (*v)->ForCondDown( loopInfo.endLoopVar))
		{
		_retTree = b;
		return RC_OK;
		}
		}
		else
		{
		if( (*v)->ForCondUp( loopInfo.endLoopVar))
		{
		_retTree = b;
		return RC_OK;
		}
		}
		
		delete loopInfo.endLoopVar;
		loopInfo.endLoopVar = NULL;
		delete loopInfo.loopStepVar;
		loopInfo.loopStepVar = NULL;
		_retTree = fsl->GetNextSibling();
		return RC_OK;
		
		
		_t = __t36;
		_t = _t->getNextSibling();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return retCode;
}

 RetCode  GDLInterpreter::foreach_statement(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP foreach_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP f = ProgNodeP(antlr::nullAST);
	ProgNodeP fl = ProgNodeP(antlr::nullAST);
	
	BaseGDL** v;
	BaseGDL* s;
	retCode = RC_OK;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case FOREACH:
	{
		ProgNodeP __t38 = _t;
		f = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),FOREACH);
		_t = _t->getFirstChild();
		
		_t = f->GetNextSibling()->GetFirstChild();
		
		EnvUDT* callStack_back = 
		static_cast<EnvUDT*>(callStack.back());
		
		ForLoopInfoT& loopInfo = 
		callStack_back->GetForLoopInfo( f->forLoopIx);
		
		v=l_simple_var(_t);
		_t = _retTree;
		
		delete loopInfo.endLoopVar;
		loopInfo.endLoopVar=expr(_t);
		
		ProgNodeP b = _retTree;
				
		loopInfo.foreachIx = 0;
		
		// currently there are no empty arrays
		//SizeT nEl = loopInfo.endLoopVar->N_Elements();
		
		// ASSIGNMENT used here also
		delete (*v);
		(*v) = loopInfo.endLoopVar->NewIx( 0);
		
		_retTree = b;
		return RC_OK;
		
		_t = __t38;
		_t = _t->getNextSibling();
		break;
	}
	case FOREACH_LOOP:
	{
		ProgNodeP __t39 = _t;
		fl = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),FOREACH_LOOP);
		_t = _t->getFirstChild();
		
		EnvUDT* callStack_back = 
		static_cast<EnvUDT*>(callStack.back());
		
		ForLoopInfoT& loopInfo = 
		callStack_back->GetForLoopInfo( fl->forLoopIx);
		
		if( loopInfo.endLoopVar == NULL)
		{
		// non-initialized loop (GOTO)
		_retTree = fl->GetNextSibling();
		return RC_OK;
		}
		
		v=l_simple_var(_t);
		_t = _retTree;
		
		// skip expr
		_t = _t->getNextSibling();
		
		++loopInfo.foreachIx;
		
		SizeT nEl = loopInfo.endLoopVar->N_Elements();
		
		if( loopInfo.foreachIx < nEl)
		{
		// ASSIGNMENT used here also
		delete (*v);
		(*v) = loopInfo.endLoopVar->NewIx( loopInfo.foreachIx);
		
		_retTree = _t;
		return RC_OK;
		}
		
		delete loopInfo.endLoopVar;
		loopInfo.endLoopVar = NULL;
		loopInfo.foreachIx = -1;
		_retTree = fl->GetNextSibling();
		return RC_OK;
		
		_t = __t39;
		_t = _t->getNextSibling();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return retCode;
}

 RetCode  GDLInterpreter::if_statement(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP if_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP i = ProgNodeP(antlr::nullAST);
	
	BaseGDL* e;
	//    retCode = RC_OK; // not set if not executed
	
	
	ProgNodeP __t41 = _t;
	i = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),IF);
	_t = _t->getFirstChild();
	e=expr(_t);
	_t = _retTree;
	
	auto_ptr<BaseGDL> e_guard(e);
	
	//                SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	if( e->True())
	{
	_retTree = _t;
	return RC_OK;
	}
	
	_retTree = i->GetNextSibling();
	return RC_OK;
	
	//                     retCode=statement(_t);
	// //                     if( retCode != RC_OK) return retCode;
	
	//                         if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	//                             !i->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	//                         {
	//                             // a jump (goto) occured out of this loop
	//                             return retCode;
	//                         }
	
	_t = __t41;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 RetCode  GDLInterpreter::if_else_statement(ProgNodeP _t) {
	 RetCode retCode;
	ProgNodeP if_else_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP i = ProgNodeP(antlr::nullAST);
	
	BaseGDL* e;
	retCode = RC_OK; // not set if not executed
	
	
	ProgNodeP __t43 = _t;
	i = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),IF_ELSE);
	_t = _t->getFirstChild();
	e=expr(_t);
	_t = _retTree;
	
	auto_ptr<BaseGDL> e_guard(e);
	
	//                SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	if( e->True())
	{
	_retTree = _t->GetFirstChild();
	return RC_OK;
	}
	
	_retTree = _t->GetNextSibling();
	return RC_OK;
	//             { 
	//                 auto_ptr<BaseGDL> e_guard(e);
	
	//                 SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	//                 if( e->True())
	//                 {
	//                     retCode=statement(_t);
	// // //                    if( retCode != RC_OK) return retCode;
	
	//                     if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	//                         !i->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	//                     {
	//                         // a jump (goto) occured out of this loop
	//                         return retCode;
	//                     }
	//                 }
	//                 else
	//                 {
	//                     _t=_t->GetNextSibling(); // jump over 1st statement
	//                     retCode=statement(_t);
	// // //                   if( retCode != RC_OK) return retCode;
	
	//                     if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	//                         !i->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	//                     {
	//                         // a jump (goto) occured out of this loop
	//                         return retCode;
	//                     }
	//                 }
	
	_t = __t43;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

BaseGDL**  GDLInterpreter::l_deref(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_deref_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		ProgNodeP retTree = _t->getNextSibling();
	
	auto_ptr<BaseGDL> e1_guard;
	BaseGDL* e1;
	ProgNodeP evalExpr = _t->getFirstChild();
	if( NonCopyNode( evalExpr->getType()))
	{
	e1 = evalExpr->EvalNC();
	}
	else if( evalExpr->getType() ==  GDLTokenTypes::FCALL_LIB)
	{
			e1=lib_function_call(evalExpr);
	
			if( e1 == NULL) // ROUTINE_NAMES
				throw GDLException( evalExpr, "Undefined return value", true, false);
			
			if( !callStack.back()->Contains( e1)) 
				e1_guard.reset( e1);
	}
	else
	{
	e1 = evalExpr->Eval();
	e1_guard.reset(e1);
	}
	
	if( e1 == NULL || e1->Type() != PTR)
	throw GDLException( evalExpr, "Pointer type required"
				" in this context: "+Name(e1),true,false);
	
	DPtrGDL* ptr=static_cast<DPtrGDL*>(e1);
	
	//     _t = _t->getFirstChild();
	
	//     BaseGDL* e1;
	
	// 	auto_ptr<BaseGDL> e1_guard;
	
	//     if( _t->getType() ==  GDLTokenTypes::FCALL_LIB)
	//       {
	// 		e1=lib_function_call(_t);
	
	// 		if( e1 == NULL) // ROUTINE_NAMES
	// 			throw GDLException( _t, "Undefined return value", true, false);
			
	// 		if( !ProgNode::interpreter->callStack.back()->Contains( e1)) 
	// 			e1_guard.reset( e1);
	//       }
	//     else
	//       {
	// 			e1=tmp_expr(_t);
	// 			e1_guard.reset( e1);
	//       }
	
	// 	DPtrGDL* ptr=dynamic_cast<DPtrGDL*>(e1);
	// 	if( ptr == NULL)
	// 	throw GDLException( _t, "Pointer type required"
	// 	" in this context: "+Name(e1),true,false);
		DPtr sc; 
		if( !ptr->Scalar(sc))
		throw GDLException( _t, "Expression must be a "
		"scalar in this context: "+Name(e1),true,false);
		if( sc == 0)
		throw GDLException( _t, "Unable to dereference"
		" NULL pointer: "+Name(e1),true,false);
		
		try{
	res = &GetHeap(sc);
		}
		catch( HeapException)
		{
	throw GDLException( _t, "Invalid pointer: "+Name(e1),true,false);
		}
		
		_retTree = retTree;
		return res;
	
	
	
	ProgNodeP __t45 = _t;
	ProgNodeP tmp33_AST_in = _t;
	match(antlr::RefAST(_t),DEREF);
	_t = _t->getFirstChild();
	e1=expr(_t);
	_t = _retTree;
	_t = __t45;
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_ret_expr(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_ret_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP varPtr = ProgNodeP(antlr::nullAST);
	ProgNodeP var = ProgNodeP(antlr::nullAST);
	
	BaseGDL*       e1;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case DEREF:
	{
		res=l_deref(_t);
		_t = _retTree;
		break;
	}
	case QUESTION:
	{
		ProgNodeP __t47 = _t;
		ProgNodeP tmp34_AST_in = _t;
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getFirstChild();
		e1=expr(_t);
		_t = _retTree;
		
		auto_ptr<BaseGDL> e1_guard(e1);
		if( e1->True())
		{
		res=l_ret_expr(_t);
		}
		else
		{
		_t=_t->GetNextSibling(); // jump over 1st expression
		res=l_ret_expr(_t);
		}
		
		_t = __t47;
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR_MFCALL:
	{
		res=l_arrayexpr_mfcall_as_mfcall(_t);
		_t = _retTree;
		break;
	}
	case FCALL:
	case FCALL_LIB:
	case MFCALL:
	case MFCALL_PARENT:
	{
		res=l_function_call(_t);
		_t = _retTree;
		// here a local to the actual environment could be returned
		if( callStack.back()->IsLocalKW( res))
		throw GDLException( _t, 
		"Attempt to return indirectly a local variable "
		"from left-function.",true,false);
		
		break;
	}
	case VARPTR:
	{
		varPtr = _t;
		match(antlr::RefAST(_t),VARPTR);
		_t = _t->getNextSibling();
		
		res=&varPtr->var->Data(); // returns BaseGDL* of var (DVar*) 
		
		break;
	}
	case VAR:
	{
		var = _t;
		match(antlr::RefAST(_t),VAR);
		_t = _t->getNextSibling();
		// check if variable is non-local 
		// (because it will be invalid after return otherwise)
		if( !callStack.back()->GlobalKW(var->varIx))
		throw GDLException( _t, 
		"Attempt to return a non-global variable from left-function.",true,false);
		
		res=&callStack.back()->GetKW(var->varIx); 
		
		break;
	}
	case ASSIGN:
	{
		ProgNodeP __t48 = _t;
		ProgNodeP tmp35_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case VAR:
		case VARPTR:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			e1=tmp_expr(_t);
			_t = _retTree;
			
			r_guard.reset( e1);
			
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( e1)) 
			r_guard.reset( e1);
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		res=l_ret_expr(_t);
		_t = _retTree;
		
		if( e1 != (*res))
		{
		delete *res;
		*res = e1;
		}
		r_guard.release();
		
		_t = __t48;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_ARRAYEXPR_MFCALL:
	{
		ProgNodeP __t50 = _t;
		ProgNodeP tmp36_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_ARRAYEXPR_MFCALL);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case VAR:
		case VARPTR:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			e1=tmp_expr(_t);
			_t = _retTree;
			
			r_guard.reset( e1);
			
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( e1)) 
			r_guard.reset( e1);
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		res=l_arrayexpr_mfcall_as_mfcall(_t);
		_t = _retTree;
		
		if( e1 != (*res))
		{
		delete *res;
		*res = e1;
		}
		r_guard.release();
		
		_t = __t50;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_REPLACE:
	{
		ProgNodeP __t52 = _t;
		ProgNodeP tmp37_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case VAR:
		case VARPTR:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			e1=tmp_expr(_t);
			_t = _retTree;
			
			r_guard.reset( e1);
			
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( e1)) 
			r_guard.reset( e1);
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		res=l_ret_expr(_t);
		_t = _retTree;
		
		if( e1 != (*res))
		{
		delete *res;
		*res = e1;
		}
		r_guard.release();
		
		_t = __t52;
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR:
	{
		ProgNodeP __t54 = _t;
		ProgNodeP tmp38_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		
		throw GDLException( _t, 
		"Indexed expression not allowed as left-function"
		" return value.",true,false);
		
		_t = __t54;
		_t = _t->getNextSibling();
		break;
	}
	case DOT:
	{
		ProgNodeP __t55 = _t;
		ProgNodeP tmp39_AST_in = _t;
		match(antlr::RefAST(_t),DOT);
		_t = _t->getFirstChild();
		
		throw GDLException( _t, 
		"Struct expression not allowed as left-function"
		" return value.",true,false);
		
		_t = __t55;
		_t = _t->getNextSibling();
		break;
	}
	case SYSVAR:
	{
		ProgNodeP tmp40_AST_in = _t;
		match(antlr::RefAST(_t),SYSVAR);
		_t = _t->getNextSibling();
		
		throw GDLException( _t, 
		"System variable not allowed as left-function"
		" return value.",true,false);
		
		break;
	}
	case ARRAYDEF:
	case EXPR:
	case NSTRUC:
	case NSTRUC_REF:
	case POSTDEC:
	case POSTINC:
	case STRUC:
	case DEC:
	case INC:
	{
		e1=r_expr(_t);
		_t = _retTree;
		
		delete e1;
		throw GDLException( _t, 
		"Expression not allowed as left-function return value.",true,false);
		
		break;
	}
	case CONSTANT:
	{
		e1=constant_nocopy(_t);
		_t = _retTree;
		
		throw GDLException( _t, 
		"Constant not allowed as left-function return value.",true,false);
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

 BaseGDL**  GDLInterpreter::l_arrayexpr_mfcall_as_mfcall(ProgNodeP _t) {
	 BaseGDL** res;
	ProgNodeP l_arrayexpr_mfcall_as_mfcall_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP mp2 = ProgNodeP(antlr::nullAST);
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	BaseGDL *self;
	EnvUDT*   newEnv;
	
	
	ProgNodeP __t161 = _t;
	ProgNodeP tmp41_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYEXPR_MFCALL);
	_t = _t->getFirstChild();
	
	_t = _t->getNextSibling(); // skip DOT
	
	self=expr(_t);
	_t = _retTree;
	mp2 = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	
	auto_ptr<BaseGDL> self_guard(self);
	
	newEnv=new EnvUDT( self, mp2, "", true);
	
	self_guard.release();
	
	parameter_def(_t, newEnv);
	_t = _retTree;
	_t = __t161;
	_t = _t->getNextSibling();
	
	// push environment onto call stack
	callStack.push_back(newEnv);
	
	// make the call
	res=call_lfun(static_cast<DSubUD*>(
	newEnv->GetPro())->GetTree());
	
	_retTree = _t;
	return res;
}

 BaseGDL**  GDLInterpreter::l_function_call(ProgNodeP _t) {
	 BaseGDL** res;
	ProgNodeP l_function_call_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP fl = ProgNodeP(antlr::nullAST);
	ProgNodeP mp = ProgNodeP(antlr::nullAST);
	ProgNodeP parent = ProgNodeP(antlr::nullAST);
	ProgNodeP p = ProgNodeP(antlr::nullAST);
	ProgNodeP f = ProgNodeP(antlr::nullAST);
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	BaseGDL *self;
	BaseGDL *libRes;
	EnvUDT*   newEnv;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case FCALL_LIB:
	{
		ProgNodeP __t163 = _t;
		fl = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),FCALL_LIB);
		_t = _t->getFirstChild();
		
		EnvT* newEnv=new EnvT( fl, fl->libFun);//libFunList[fl->funIx]);
		
		parameter_def(_t, newEnv);
		_t = _retTree;
		
		EnvT* callerEnv = static_cast<EnvT*>(callStack.back());
		// push id.pro onto call stack
		callStack.push_back(newEnv);
		// make the call
		BaseGDL* libRes = 
		static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv);
		
		res = callerEnv->GetPtrTo( libRes);
		if( res == NULL)
		throw GDLException( _t, "Library function must return a "
		"l-value in this context: "+fl->getText());
		
		_t = __t163;
		_t = _t->getNextSibling();
		break;
	}
	case FCALL:
	case MFCALL:
	case MFCALL_PARENT:
	{
		{
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case MFCALL:
		{
			ProgNodeP __t166 = _t;
			ProgNodeP tmp42_AST_in = _t;
			match(antlr::RefAST(_t),MFCALL);
			_t = _t->getFirstChild();
			self=expr(_t);
			_t = _retTree;
			mp = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			
			auto_ptr<BaseGDL> self_guard(self);
			
			newEnv=new EnvUDT( self, mp, "", true);
			
			self_guard.release();
			
			parameter_def(_t, newEnv);
			_t = _retTree;
			_t = __t166;
			_t = _t->getNextSibling();
			break;
		}
		case MFCALL_PARENT:
		{
			ProgNodeP __t167 = _t;
			ProgNodeP tmp43_AST_in = _t;
			match(antlr::RefAST(_t),MFCALL_PARENT);
			_t = _t->getFirstChild();
			self=expr(_t);
			_t = _retTree;
			parent = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			p = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			
			auto_ptr<BaseGDL> self_guard(self);
			
			newEnv=new EnvUDT( self, p,
			parent->getText(), true);
			
			self_guard.release();
			
			parameter_def(_t, newEnv);
			_t = _retTree;
			_t = __t167;
			_t = _t->getNextSibling();
			break;
		}
		case FCALL:
		{
			ProgNodeP __t168 = _t;
			f = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
			match(antlr::RefAST(_t),FCALL);
			_t = _t->getFirstChild();
			
			SetFunIx( f);
			
			newEnv=new EnvUDT( f, funList[f->funIx], true);
			
			parameter_def(_t, newEnv);
			_t = _retTree;
			_t = __t168;
			_t = _t->getNextSibling();
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		// push environment onto call stack
		callStack.push_back(newEnv);
		
		// make the call
		res=call_lfun(static_cast<DSubUD*>(
		newEnv->GetPro())->GetTree());
		
		}
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::tmp_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP tmp_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP q = ProgNodeP(antlr::nullAST);
	ProgNodeP a = ProgNodeP(antlr::nullAST);
	
	BaseGDL*  e1;
	BaseGDL** e2;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case DEREF:
	{
		e2=l_deref(_t);
		_t = _retTree;
		
		if( *e2 == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(e2),true,false);
		
		res = (*e2)->Dup();
		
		break;
	}
	case QUESTION:
	{
		ProgNodeP __t125 = _t;
		q = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getFirstChild();
		res = q->Eval();
		_t = __t125;
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR:
	{
		{
		a = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getNextSibling();
		res = a->Eval();
		}
		break;
	}
	case DOT:
	{
		res=dot_expr(_t);
		_t = _retTree;
		break;
	}
	case ASSIGN:
	case ASSIGN_REPLACE:
	case ASSIGN_ARRAYEXPR_MFCALL:
	{
		res=assign_expr(_t);
		_t = _retTree;
		break;
	}
	case ARRAYEXPR_MFCALL:
	case FCALL:
	case MFCALL:
	case MFCALL_PARENT:
	{
		res=function_call(_t);
		_t = _retTree;
		break;
	}
	case ARRAYDEF:
	case EXPR:
	case NSTRUC:
	case NSTRUC_REF:
	case POSTDEC:
	case POSTINC:
	case STRUC:
	case DEC:
	case INC:
	{
		res=r_expr(_t);
		_t = _retTree;
		break;
	}
	case CONSTANT:
	{
		res=constant(_t);
		_t = _retTree;
		break;
	}
	case VAR:
	case VARPTR:
	{
		res=simple_var(_t);
		_t = _retTree;
		break;
	}
	case SYSVAR:
	{
		res=sys_var(_t);
		_t = _retTree;
		break;
	}
	case FCALL_LIB_RETNEW:
	{
		res=lib_function_call_retnew(_t);
		_t = _retTree;
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

 BaseGDL*  GDLInterpreter::lib_function_call(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP lib_function_call_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP fll = ProgNodeP(antlr::nullAST);
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
		
		ProgNodeP rTree = _t->getNextSibling();
	// 	match(antlr::RefAST(_t),FCALL_LIB);
	
	// 	match(antlr::RefAST(_t),IDENTIFIER);
	
		ProgNodeP& fl = _t;
		EnvT* newEnv=new EnvT( fl, fl->libFun);//libFunList[fl->funIx]);
		
	parameter_def(_t->getFirstChild(), newEnv);
	
		// push id.pro onto call stack
		callStack.push_back(newEnv);
		// make the call
		//BaseGDL* 
	res=static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv);
		// *** MUST always return a defined expression
	//    if( res == NULL)
	//       throw GDLException( _t, "");
	
		_retTree = rTree;
		return res;
	
	
	ProgNodeP __t141 = _t;
	fll = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),FCALL_LIB);
	_t = _t->getFirstChild();
	
	//EnvT* 
	newEnv=new EnvT( fll, fll->libFun);//libFunList[fl->funIx]);
	
	parameter_def(_t, newEnv);
	_t = _retTree;
	
	// push id.pro onto call stack
	callStack.push_back(newEnv);
	// make the call
	res=static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv);
	//*** MUST always return a defined expression
	
	_t = __t141;
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::r_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP r_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		switch ( _t->getType()) {
		case EXPR:
		case ARRAYDEF:
		case STRUC:
		case NSTRUC:
		case NSTRUC_REF:
		{
			res = _t->Eval(); 
			break;
		}
		case DEC:
		{
			res=l_decinc_expr( _t->getFirstChild(), DEC);
			break;
		}
		case INC:
		{
			res=l_decinc_expr( _t->getFirstChild(), INC);
			break;
		}
		case POSTDEC:
		{
			res=l_decinc_expr( _t->getFirstChild(), POSTDEC);
			break;
		}
	//	case POSTINC:
		default:
		{
			res=l_decinc_expr( _t->getFirstChild(), POSTINC);
			break;
		}
	// 	default:
	// 	{
	// 		throw antlr::NoViableAltException(antlr::RefAST(_t));
	// 	}
		}
		_retTree = _t->getNextSibling();
		return res;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case EXPR:
	{
		ProgNodeP tmp44_AST_in = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYDEF:
	{
		ProgNodeP tmp45_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYDEF);
		_t = _t->getNextSibling();
		break;
	}
	case STRUC:
	{
		ProgNodeP tmp46_AST_in = _t;
		match(antlr::RefAST(_t),STRUC);
		_t = _t->getNextSibling();
		break;
	}
	case NSTRUC:
	{
		ProgNodeP tmp47_AST_in = _t;
		match(antlr::RefAST(_t),NSTRUC);
		_t = _t->getNextSibling();
		break;
	}
	case NSTRUC_REF:
	{
		ProgNodeP tmp48_AST_in = _t;
		match(antlr::RefAST(_t),NSTRUC_REF);
		_t = _t->getNextSibling();
		break;
	}
	case DEC:
	{
		ProgNodeP __t96 = _t;
		ProgNodeP tmp49_AST_in = _t;
		match(antlr::RefAST(_t),DEC);
		_t = _t->getFirstChild();
		res=l_decinc_expr(_t, DEC);
		_t = _retTree;
		_t = __t96;
		_t = _t->getNextSibling();
		break;
	}
	case INC:
	{
		ProgNodeP __t97 = _t;
		ProgNodeP tmp50_AST_in = _t;
		match(antlr::RefAST(_t),INC);
		_t = _t->getFirstChild();
		res=l_decinc_expr(_t, INC);
		_t = _retTree;
		_t = __t97;
		_t = _t->getNextSibling();
		break;
	}
	case POSTDEC:
	{
		ProgNodeP __t98 = _t;
		ProgNodeP tmp51_AST_in = _t;
		match(antlr::RefAST(_t),POSTDEC);
		_t = _t->getFirstChild();
		res=l_decinc_expr(_t, POSTDEC);
		_t = _retTree;
		_t = __t98;
		_t = _t->getNextSibling();
		break;
	}
	case POSTINC:
	{
		ProgNodeP __t99 = _t;
		ProgNodeP tmp52_AST_in = _t;
		match(antlr::RefAST(_t),POSTINC);
		_t = _t->getFirstChild();
		res=l_decinc_expr(_t, POSTINC);
		_t = _retTree;
		_t = __t99;
		_t = _t->getNextSibling();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::constant_nocopy(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP constant_nocopy_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP c = ProgNodeP(antlr::nullAST);
	
		//BaseGDL* 
		_retTree = _t->getNextSibling();
		return _t->cData; // no ->Dup(); 
	
	
	c = _t;
	match(antlr::RefAST(_t),CONSTANT);
	_t = _t->getNextSibling();
	
	res=c->cData; // no ->Dup(); 
	
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::l_decinc_indexable_expr(ProgNodeP _t,
	int dec_inc
) {
	BaseGDL* res;
	ProgNodeP l_decinc_indexable_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	BaseGDL** e;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case FCALL:
	case FCALL_LIB:
	case MFCALL:
	case MFCALL_PARENT:
	{
		e=l_function_call(_t);
		_t = _retTree;
		
		res = *e;
		if( res == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(e),true,false);
		
		break;
	}
	case DEREF:
	{
		e=l_deref(_t);
		_t = _retTree;
		
		res = *e;
		if( res == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(e),true,false);
		
		break;
	}
	case VAR:
	case VARPTR:
	{
		e=l_defined_simple_var(_t);
		_t = _retTree;
		res = *e;
		break;
	}
	case SYSVAR:
	{
		e=l_sys_var(_t);
		_t = _retTree;
		res = *e;
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_defined_simple_var(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_defined_simple_var_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		if( _t->getType() == VAR)
		{
	// 		match(antlr::RefAST(_t),VAR);
			res=&callStack.back()->GetKW(_t->varIx); 
			if( *res == NULL)
			throw GDLException( _t, "Variable is undefined: "+
			callStack.back()->GetString(_t->varIx),true,false);
			
		}
		else
		{
	// 		match(antlr::RefAST(_t),VARPTR);
			res=&_t->var->Data(); // returns BaseGDL* of var (DVar*) 
			if( *res == NULL)
			throw GDLException( _t, "Variable is undefined: "+
			callStack.back()->GetString( *res),true,false);
			
		}
		_retTree = _t->getNextSibling();
		return res;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	{
		ProgNodeP tmp53_AST_in = _t;
		match(antlr::RefAST(_t),VAR);
		_t = _t->getNextSibling();
		break;
	}
	case VARPTR:
	{
		ProgNodeP tmp54_AST_in = _t;
		match(antlr::RefAST(_t),VARPTR);
		_t = _t->getNextSibling();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_sys_var(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_sys_var_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP sysVar = ProgNodeP(antlr::nullAST);
	
	sysVar = _t;
	match(antlr::RefAST(_t),SYSVAR);
	_t = _t->getNextSibling();
	
	if( sysVar->var == NULL) 
	{
	sysVar->var=FindInVarList(sysVarList,sysVar->getText());
	if( sysVar->var == NULL)		    
	throw GDLException( _t, "Not a legal system variable: !"+
	sysVar->getText(),true,false);
	
	// note: this works, because system variables are never 
	//       passed by reference
	SizeT rdOnlySize = sysVarRdOnlyList.size();
	for( SizeT i=0; i<rdOnlySize; ++i)
	if( sysVarRdOnlyList[ i] == sysVar->var)
	throw GDLException( _t, 
	"Attempt to write to a readonly variable: !"+
	sysVar->getText(),true,false);
	}
	// system variables are always defined
	res=&sysVar->var->Data();
	
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::l_decinc_array_expr(ProgNodeP _t,
	int dec_inc
) {
	BaseGDL* res;
	ProgNodeP l_decinc_array_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	ArrayIndexListT* aL;
	BaseGDL*         e;
	ArrayIndexListGuard guard;
	
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	{
		ProgNodeP __t58 = _t;
		ProgNodeP tmp55_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		e=l_decinc_indexable_expr(_t, dec_inc);
		_t = _retTree;
		aL=arrayindex_list(_t);
		_t = _retTree;
		_t = __t58;
		_t = _t->getNextSibling();
		
		guard.reset( aL); 
		aL->SetVariable( e);
		
		if( dec_inc == DECSTATEMENT) 
		{
		e->DecAt( aL); 
		res = NULL;
		break;
		}
		if( dec_inc == INCSTATEMENT)
		{
		e->IncAt( aL);
		res = NULL;
		break;
		}
		
		if( dec_inc == DEC) e->DecAt( aL); 
		else if( dec_inc == INC) e->IncAt( aL);
		//
		res=e->Index( aL);
		
		if( dec_inc == POSTDEC) e->DecAt( aL);
		else if( dec_inc == POSTINC) e->IncAt( aL);
		
		break;
	}
	case DEREF:
	case FCALL:
	case FCALL_LIB:
	case MFCALL:
	case MFCALL_PARENT:
	case SYSVAR:
	case VAR:
	case VARPTR:
	{
		e=l_decinc_indexable_expr(_t, dec_inc);
		_t = _retTree;
		
		if( dec_inc == DECSTATEMENT) 
		{
		e->Dec(); 
		res = NULL;
		break;
		}
		if( dec_inc == INCSTATEMENT)
		{
		e->Inc();
		res = NULL;
		break;
		}
		
		if( dec_inc == DEC) e->Dec();
		else if( dec_inc == INC) e->Inc();
		//          
		res = e->Dup();
		
		if( dec_inc == POSTDEC) e->Dec();
		else if( dec_inc == POSTINC) e->Inc();
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

ArrayIndexListT*  GDLInterpreter::arrayindex_list(ProgNodeP _t) {
	ArrayIndexListT* aL;
	ProgNodeP arrayindex_list_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP ax = ProgNodeP(antlr::nullAST);
	
	ExprListT        exprList; // for cleanup
	IxExprListT      ixExprList;
	SizeT nExpr;
	BaseGDL* s;
	
		
		ProgNodeP retTree = _t->getNextSibling();
		ax = _t;
	// 	match(antlr::RefAST(_t),ARRAYIX);
		_t = _t->getFirstChild();
		
		aL = ax->arrIxList;
		assert( aL != NULL);
		
		nExpr = aL->NParam();
		if( nExpr == 0)
		{
		aL->Init();
		_retTree = retTree;
		return aL;
		}
		
		while( _t != NULL) {
	
				switch ( _t->getType()) {
				case CONSTANT:
				case DEREF:
				case SYSVAR:
				case VAR:
				case VARPTR:
				{
					s=indexable_expr(_t);
	//				_t = _retTree;
					break;
				}
				case FCALL_LIB:
				{
					s=lib_function_call(_t);
	//				_t = _retTree;
					
					if( !callStack.back()->Contains( s)) 
					exprList.push_back( s);
					
					break;
				}
				default:
				{
					s=indexable_tmp_expr(_t);
	//				_t = _retTree;
					exprList.push_back( s);
					break;
				}
				} // switch
				
				
				ixExprList.push_back( s);
				if( ixExprList.size() == nExpr)
	break; // allows some manual tuning
	
	_t = _t->getNextSibling();
		}
	
		aL->Init( ixExprList);
		
		_retTree = retTree;
		return aL;
	
	
	ProgNodeP __t176 = _t;
	ax = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),ARRAYIX);
	_t = _t->getFirstChild();
	{ // ( ... )*
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_1.member(_t->getType()))) {
			{
			if (_t == ProgNodeP(antlr::nullAST) )
				_t = ASTNULL;
			switch ( _t->getType()) {
			case CONSTANT:
			case DEREF:
			case SYSVAR:
			case VAR:
			case VARPTR:
			{
				s=indexable_expr(_t);
				_t = _retTree;
				break;
			}
			case FCALL_LIB:
			{
				s=lib_function_call(_t);
				_t = _retTree;
				
				if( !callStack.back()->Contains( s)) 
				exprList.push_back( s);
				
				break;
			}
			case ASSIGN:
			case ASSIGN_REPLACE:
			case ASSIGN_ARRAYEXPR_MFCALL:
			case ARRAYDEF:
			case ARRAYEXPR:
			case ARRAYEXPR_MFCALL:
			case EXPR:
			case FCALL:
			case FCALL_LIB_RETNEW:
			case MFCALL:
			case MFCALL_PARENT:
			case NSTRUC:
			case NSTRUC_REF:
			case POSTDEC:
			case POSTINC:
			case STRUC:
			case DEC:
			case INC:
			case DOT:
			case QUESTION:
			{
				s=indexable_tmp_expr(_t);
				_t = _retTree;
				exprList.push_back( s);
				break;
			}
			default:
			{
				throw antlr::NoViableAltException(antlr::RefAST(_t));
			}
			}
			}
		}
		else {
			goto _loop179;
		}
		
	}
	_loop179:;
	} // ( ... )*
	_t = __t176;
	_t = _t->getNextSibling();
	_retTree = _t;
	return aL;
}

BaseGDL*  GDLInterpreter::l_decinc_dot_expr(ProgNodeP _t,
	int dec_inc
) {
	BaseGDL* res;
	ProgNodeP l_decinc_dot_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP dot = ProgNodeP(antlr::nullAST);
	
	ProgNodeP __t60 = _t;
	dot = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),DOT);
	_t = _t->getFirstChild();
	
	SizeT nDot=dot->nDot;
	auto_ptr<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
	
	l_dot_array_expr(_t, aD.get());
	_t = _retTree;
	{ // ( ... )+
	int _cnt62=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR || _t->getType() == IDENTIFIER)) {
			tag_array_expr(_t, aD.get());
			_t = _retTree;
		}
		else {
			if ( _cnt62>=1 ) { goto _loop62; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt62++;
	}
	_loop62:;
	}  // ( ... )+
	_t = __t60;
	_t = _t->getNextSibling();
	
	if( dec_inc == DECSTATEMENT) 
	{
	aD->Dec(); 
	res = NULL;
	}
	else if( dec_inc == INCSTATEMENT)
	{
	aD->Inc();
	res = NULL;
	}
	else
	{
	if( dec_inc == DEC) aD->Dec(); //*** aD->Assign( dec_inc);
	else if( dec_inc == INC) aD->Inc();
	//                
	res=aD->Resolve();
	
	if( dec_inc == POSTDEC) aD->Dec();
	else if( dec_inc == POSTINC) aD->Inc();
	}
	
	_retTree = _t;
	return res;
}

void GDLInterpreter::l_dot_array_expr(ProgNodeP _t,
	DotAccessDescT* aD
) {
	ProgNodeP l_dot_array_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	ArrayIndexListT* aL;
	BaseGDL**        rP;
	DStructGDL*      structR;
	ArrayIndexListGuard guard;
	bool isObj = callStack.back()->IsObject();
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	{
		ProgNodeP __t78 = _t;
		ProgNodeP tmp56_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		rP=l_indexable_expr(_t);
		_t = _retTree;
		aL=arrayindex_list(_t);
		_t = _retTree;
		guard.reset(aL);
		_t = __t78;
		_t = _t->getNextSibling();
		
		// check here for object and get struct
		structR=dynamic_cast<DStructGDL*>(*rP);
		if( structR == NULL)
		{
		if( isObj)
		{
		DStructGDL* oStruct = ObjectStructCheckAccess( *rP, _t);
		
		// oStruct cannot be "Assoc_"
		aD->Root( oStruct, guard.release()); 
		}
		else
		{
		throw GDLException( _t, "Expression must be a"
		" STRUCT in this context: "+Name(*rP),true,false);
		}
		}
		else 
		{
		if( (*rP)->IsAssoc())
		throw GDLException( _t, "File expression not allowed "
		"in this context: "+Name(*rP),true,false);
		
		aD->Root( structR, guard.release() /* aL */); 
		}
		
		break;
	}
	case ARRAYEXPR_MFCALL:
	case DEREF:
	case EXPR:
	case FCALL:
	case FCALL_LIB:
	case MFCALL:
	case MFCALL_PARENT:
	case SYSVAR:
	case VAR:
	case VARPTR:
	{
		rP=l_indexable_expr(_t);
		_t = _retTree;
		
		// check here for object and get struct
		structR = dynamic_cast<DStructGDL*>(*rP);
		if( structR == NULL)
		{
		if( isObj) // member access to object?
		{
		DStructGDL* oStruct = ObjectStructCheckAccess( *rP, _t);
		
		// oStruct cannot be "Assoc_"
		aD->Root( oStruct); 
		}
		else
		{
		throw GDLException( _t, "Expression must be a"
		" STRUCT in this context: "+Name(*rP),true,false);
		}
		}
		else
		{
		if( (*rP)->IsAssoc())
		{
		throw GDLException( _t, "File expression not allowed "
		"in this context: "+Name(*rP),true,false);
		}
		
		aD->Root(structR); 
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

void GDLInterpreter::tag_array_expr(ProgNodeP _t,
	DotAccessDescT* aD
) {
	ProgNodeP tag_array_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	ArrayIndexListT* aL;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	{
		ProgNodeP __t110 = _t;
		ProgNodeP tmp57_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		tag_expr(_t, aD);
		_t = _retTree;
		aL=arrayindex_list(_t);
		_t = _retTree;
		aD->AddIx(aL);
		_t = __t110;
		_t = _t->getNextSibling();
		break;
	}
	case EXPR:
	case IDENTIFIER:
	{
		tag_expr(_t, aD);
		_t = _retTree;
		aD->AddIx(NULL);
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
}

BaseGDL*  GDLInterpreter::l_decinc_expr(ProgNodeP _t,
	int dec_inc
) {
	BaseGDL* res;
	ProgNodeP l_decinc_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP mp2 = ProgNodeP(antlr::nullAST);
	
	BaseGDL*       e1;
	ProgNodeP startNode = _t;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case QUESTION:
	{
		ProgNodeP __t64 = _t;
		ProgNodeP tmp58_AST_in = _t;
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getFirstChild();
		e1=expr(_t);
		_t = _retTree;
		
		auto_ptr<BaseGDL> e1_guard(e1);
		
		if( e1->True())
		{
		res=l_decinc_expr(_t, dec_inc);
		}
		else
		{
		_t=_t->GetNextSibling(); // jump over 1st expression
		res=l_decinc_expr(_t, dec_inc);
		}
		
		_t = __t64;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN:
	{
		ProgNodeP __t65 = _t;
		ProgNodeP tmp59_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case CONSTANT:
		case DEREF:
		case SYSVAR:
		case VAR:
		case VARPTR:
		{
			e1=indexable_expr(_t);
			_t = _retTree;
			break;
		}
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			e1=indexable_tmp_expr(_t);
			_t = _retTree;
			r_guard.reset( e1);
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( e1)) 
			r_guard.reset( e1); // guard if no global data
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		ProgNodeP l = _t;
		
		BaseGDL** tmp;
		
		tmp=l_expr(_t, e1);
		_t = _retTree;
		
		_t = l;
		
		res=l_decinc_expr(_t, dec_inc);
		_t = _retTree;
		_t = __t65;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_ARRAYEXPR_MFCALL:
	{
		ProgNodeP __t67 = _t;
		ProgNodeP tmp60_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_ARRAYEXPR_MFCALL);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case CONSTANT:
		case DEREF:
		case SYSVAR:
		case VAR:
		case VARPTR:
		{
			e1=indexable_expr(_t);
			_t = _retTree;
			break;
		}
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			e1=indexable_tmp_expr(_t);
			_t = _retTree;
			r_guard.reset( e1);
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( e1)) 
			r_guard.reset( e1); // guard if no global data
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		ProgNodeP l = _t;
		
		BaseGDL** tmp;
		
		// try MFCALL
		try
		{
		
		tmp=l_arrayexpr_mfcall_as_mfcall(l);
		
		if( e1 != (*tmp))
		{
		delete *tmp;
		
		if( r_guard.get() == e1)
		*tmp = r_guard.release();
		else          
		*tmp = e1->Dup();
		}
		
		res=l_decinc_expr( l, dec_inc);
		}
		catch( GDLException& ex)
		{
		// try ARRAYEXPR
		try
			                {
		tmp=l_arrayexpr_mfcall_as_arrayexpr(l, e1);
			                }
		catch( GDLException& ex2)
		{
		throw GDLException(ex.toString() + " or "+ex2.toString());
		}
		
		res=l_decinc_expr( l, dec_inc);
		}
		
		_t = __t67;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_REPLACE:
	{
		ProgNodeP __t69 = _t;
		ProgNodeP tmp61_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case VAR:
		case VARPTR:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			e1=tmp_expr(_t);
			_t = _retTree;
			
			r_guard.reset( e1);
			
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( e1)) 
			r_guard.reset( e1);
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		ProgNodeP l = _t;
		
		BaseGDL** tmp;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case FCALL:
		case FCALL_LIB:
		case MFCALL:
		case MFCALL_PARENT:
		{
			tmp=l_function_call(_t);
			_t = _retTree;
			break;
		}
		case DEREF:
		{
			tmp=l_deref(_t);
			_t = _retTree;
			break;
		}
		case VAR:
		case VARPTR:
		{
			tmp=l_simple_var(_t);
			_t = _retTree;
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		if( e1 != (*tmp))
		{
		delete *tmp;
		
		if( r_guard.get() == e1)
		*tmp = r_guard.release();
		else  
		*tmp = e1->Dup();
		}
		
		
		_t = l;
		
		res=l_decinc_expr(_t, dec_inc);
		_t = _retTree;
		_t = __t69;
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR:
	case DEREF:
	case FCALL:
	case FCALL_LIB:
	case MFCALL:
	case MFCALL_PARENT:
	case SYSVAR:
	case VAR:
	case VARPTR:
	{
		res=l_decinc_array_expr(_t, dec_inc);
		_t = _retTree;
		break;
	}
	case ARRAYEXPR_MFCALL:
	{
		ProgNodeP __t72 = _t;
		ProgNodeP tmp62_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR_MFCALL);
		_t = _t->getFirstChild();
		
		ProgNodeP mark = _t;
		_t = _t->getNextSibling(); // step over DOT
		
		BaseGDL* self;
		
		self=expr(_t);
		_t = _retTree;
		mp2 = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		auto_ptr<BaseGDL> self_guard(self);
		
		EnvUDT* newEnv;
		
		try {
		newEnv=new EnvUDT( self, mp2, "", true);
		self_guard.release();
		}
		catch( GDLException& ex)
		{
		_t = mark;
		
		res=l_decinc_dot_expr(_t, dec_inc);
		
		_retTree = startNode->getNextSibling();
		return res;
		}   
		
		parameter_def(_t, newEnv);
		_t = _retTree;
		
		// push environment onto call stack
		callStack.push_back(newEnv);
		
		// make the call
		BaseGDL** ee=call_lfun(static_cast<DSubUD*>(
		newEnv->GetPro())->GetTree());
		
		BaseGDL* e = *ee;
		if( e == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(ee),true,false);
		
		if( dec_inc == DECSTATEMENT) 
		{
		e->Dec(); 
		res = NULL;
		_retTree = startNode->getNextSibling();
		return res;
		}
		if( dec_inc == INCSTATEMENT)
		{
		e->Inc();
		res = NULL;
		_retTree = startNode->getNextSibling();
		return res;
		}
		
		if( dec_inc == DEC) e->Dec();
		else if( dec_inc == INC) e->Inc();
		//          
		res = e->Dup();
		
		if( dec_inc == POSTDEC) e->Dec();
		else if( dec_inc == POSTINC) e->Inc();
		
		_retTree = startNode->getNextSibling();
		return res;
		
		_t = __t72;
		_t = _t->getNextSibling();
		break;
	}
	case DOT:
	{
		res=l_decinc_dot_expr(_t, dec_inc);
		_t = _retTree;
		break;
	}
	case ARRAYDEF:
	case EXPR:
	case NSTRUC:
	case NSTRUC_REF:
	case POSTDEC:
	case POSTINC:
	case STRUC:
	case DEC:
	case INC:
	{
		e1=r_expr(_t);
		_t = _retTree;
		
		delete e1;
		throw GDLException( _t, 
		"Expression not allowed with decrement/increment operator.",true,false);
		
		break;
	}
	case CONSTANT:
	{
		e1=constant_nocopy(_t);
		_t = _retTree;
		
		throw GDLException( _t, 
		"Constant not allowed with decrement/increment operator.",true,false);
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::indexable_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP indexable_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		_retTree = _t->getNextSibling();
	return _t->EvalNC();
	
	BaseGDL** e2;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	case VARPTR:
	{
		e2=l_defined_simple_var(_t);
		_t = _retTree;
		
		res = *e2;
		
		break;
	}
	case SYSVAR:
	{
		res=sys_var_nocopy(_t);
		_t = _retTree;
		break;
	}
	case CONSTANT:
	{
		res=constant_nocopy(_t);
		_t = _retTree;
		break;
	}
	case DEREF:
	{
		e2=l_deref(_t);
		_t = _retTree;
		
		if( *e2 == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(e2),true,false);
		res = *e2;
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::indexable_tmp_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP indexable_tmp_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP q = ProgNodeP(antlr::nullAST);
	ProgNodeP a = ProgNodeP(antlr::nullAST);
	
	BaseGDL*  e1;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case QUESTION:
	{
		ProgNodeP __t120 = _t;
		q = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getFirstChild();
		res = q->Eval();
		_t = __t120;
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR:
	{
		{
		a = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getNextSibling();
		res = a->Eval();
		}
		break;
	}
	case DOT:
	{
		res=dot_expr(_t);
		_t = _retTree;
		break;
	}
	case ASSIGN:
	case ASSIGN_REPLACE:
	case ASSIGN_ARRAYEXPR_MFCALL:
	{
		res=assign_expr(_t);
		_t = _retTree;
		break;
	}
	case ARRAYEXPR_MFCALL:
	case FCALL:
	case MFCALL:
	case MFCALL_PARENT:
	{
		res=function_call(_t);
		_t = _retTree;
		break;
	}
	case ARRAYDEF:
	case EXPR:
	case NSTRUC:
	case NSTRUC_REF:
	case POSTDEC:
	case POSTINC:
	case STRUC:
	case DEC:
	case INC:
	{
		res=r_expr(_t);
		_t = _retTree;
		break;
	}
	case FCALL_LIB_RETNEW:
	{
		res=lib_function_call_retnew(_t);
		_t = _retTree;
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_expr(ProgNodeP _t,
	BaseGDL* right
) {
	BaseGDL** res;
	ProgNodeP l_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP dot = ProgNodeP(antlr::nullAST);
	
	BaseGDL*       e1;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case QUESTION:
	{
		ProgNodeP __t80 = _t;
		ProgNodeP tmp63_AST_in = _t;
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getFirstChild();
		e1=expr(_t);
		_t = _retTree;
		
		auto_ptr<BaseGDL> e1_guard(e1);
		
		if( e1->True())
		{
		res=l_expr(_t, right);
		}
		else
		{
		_t=_t->GetNextSibling(); // jump over 1st expression
		res=l_expr(_t, right);
		}
		
		_t = __t80;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN:
	{
		ProgNodeP __t81 = _t;
		ProgNodeP tmp64_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN);
		_t = _t->getFirstChild();
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case CONSTANT:
		case DEREF:
		case SYSVAR:
		case VAR:
		case VARPTR:
		{
			e1=indexable_expr(_t);
			_t = _retTree;
			break;
		}
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			e1=indexable_tmp_expr(_t);
			_t = _retTree;
			delete e1;
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( e1)) 
			delete e1; // guard if no global data
			//                        r_guard.reset( e1); // guard if no global data
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		res=l_expr(_t, right);
		_t = _retTree;
		_t = __t81;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_ARRAYEXPR_MFCALL:
	{
		ProgNodeP __t83 = _t;
		ProgNodeP tmp65_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_ARRAYEXPR_MFCALL);
		_t = _t->getFirstChild();
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case CONSTANT:
		case DEREF:
		case SYSVAR:
		case VAR:
		case VARPTR:
		{
			e1=indexable_expr(_t);
			_t = _retTree;
			break;
		}
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			e1=indexable_tmp_expr(_t);
			_t = _retTree;
			delete e1;
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( e1)) 
			delete e1; 
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		ProgNodeP l = _t;
		
		// try MFCALL
		try
		{
		
		res=l_arrayexpr_mfcall_as_mfcall( l);
		
		if( right != (*res))
		{
		delete *res;
		*res = right->Dup();
		}
		}
		catch( GDLException& ex)
		{
		// try ARRAYEXPR
		try
			                {
		res=l_arrayexpr_mfcall_as_arrayexpr(l, right);
			                }
		catch( GDLException& ex2)
		{
		throw GDLException(ex.toString() + " or "+ex2.toString());
		}
		}
		
		_t = __t83;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_REPLACE:
	{
		ProgNodeP __t85 = _t;
		ProgNodeP tmp66_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
		_t = _t->getFirstChild();
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case VAR:
		case VARPTR:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			e1=tmp_expr(_t);
			_t = _retTree;
			
			delete e1;
			//                    r_guard.reset( e1);
			
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( e1)) 
			delete e1;
			//                        r_guard.reset( e1);
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case FCALL:
		case FCALL_LIB:
		case MFCALL:
		case MFCALL_PARENT:
		{
			res=l_function_call(_t);
			_t = _retTree;
			break;
		}
		case DEREF:
		{
			res=l_deref(_t);
			_t = _retTree;
			break;
		}
		case VAR:
		case VARPTR:
		{
			res=l_simple_var(_t);
			_t = _retTree;
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		if( right != (*res))
		//            if( e1 != (*res))
		{
		delete *res;
		//
		//                if( r_guard.get() == e1)
		//                  *res = r_guard.release();
		//                else  
		*res = right->Dup();
		}
		
		_t = __t85;
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR:
	{
		res=l_array_expr(_t, right);
		_t = _retTree;
		break;
	}
	case SYSVAR:
	{
		ProgNodeP sysVar = _t;
		res=l_sys_var(_t);
		_t = _retTree;
		
		if( right == NULL)
		throw GDLException( sysVar, 
		"System variable not allowed in this context.",true,false);
		
		auto_ptr<BaseGDL> conv_guard; //( rConv);
		BaseGDL* rConv = right;
		if( !(*res)->EqType( right))
		{
		rConv = right->Convert2( (*res)->Type(), 
		BaseGDL::COPY);
		conv_guard.reset( rConv);
		}
		
		if( right->N_Elements() != 1 && 
		((*res)->N_Elements() != right->N_Elements()))
		{
		throw GDLException( _t, "Conflicting data structures: <"+
		right->TypeStr()+" "+right->Dim().ToString()+">, !"+ 
		sysVar->getText(),true,false);
		}
		
		(*res)->AssignAt( rConv); // linear copy
		
		break;
	}
	case DEREF:
	case FCALL:
	case FCALL_LIB:
	case MFCALL:
	case MFCALL_PARENT:
	case VAR:
	case VARPTR:
	{
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case FCALL:
		case FCALL_LIB:
		case MFCALL:
		case MFCALL_PARENT:
		{
			res=l_function_call(_t);
			_t = _retTree;
			break;
		}
		case DEREF:
		{
			res=l_deref(_t);
			_t = _retTree;
			break;
		}
		case VAR:
		case VARPTR:
		{
			res=l_simple_var(_t);
			_t = _retTree;
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		if( right != NULL && right != (*res))
		{
		delete *res;
		*res = right->Dup();
		}
		
		break;
	}
	case ARRAYEXPR_MFCALL:
	{
		res=l_arrayexpr_mfcall(_t, right);
		_t = _retTree;
		break;
	}
	case DOT:
	{
		ProgNodeP __t89 = _t;
		dot = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),DOT);
		_t = _t->getFirstChild();
		
		SizeT nDot=dot->nDot;
		auto_ptr<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
		
		l_dot_array_expr(_t, aD.get());
		_t = _retTree;
		{ // ( ... )+
		int _cnt91=0;
		for (;;) {
			if (_t == ProgNodeP(antlr::nullAST) )
				_t = ASTNULL;
			if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR || _t->getType() == IDENTIFIER)) {
				tag_array_expr(_t, aD.get());
				_t = _retTree;
			}
			else {
				if ( _cnt91>=1 ) { goto _loop91; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
			}
			
			_cnt91++;
		}
		_loop91:;
		}  // ( ... )+
		_t = __t89;
		_t = _t->getNextSibling();
		
		if( right == NULL)
		throw GDLException( _t, "Struct expression not allowed in this context.",true,false);
		
		aD->Assign( right);
		
		res=NULL;
		
		break;
	}
	case ARRAYDEF:
	case EXPR:
	case NSTRUC:
	case NSTRUC_REF:
	case POSTDEC:
	case POSTINC:
	case STRUC:
	case DEC:
	case INC:
	{
		e1=r_expr(_t);
		_t = _retTree;
		
		delete e1;
		throw GDLException( _t, 
		"Expression not allowed as l-value.",true,false);
		
		break;
	}
	case CONSTANT:
	{
		e1=constant_nocopy(_t);
		_t = _retTree;
		
		throw GDLException( _t, 
		"Constant not allowed as l-value.",true,false);
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_simple_var(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_simple_var_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		assert( _t != NULL);
	
		_retTree = _t->getNextSibling();
	
		if( _t->getType() == VAR)
		{
			return &callStack.back()->GetKW(_t->varIx); 
	//		ProgNodeP var = _t;
	// 		match(antlr::RefAST(_t),VAR);
		}
		else
		{
			return &_t->var->Data(); // returns BaseGDL* of var (DVar*) 
	//		ProgNodeP varPtr = _t;
	// 		match(antlr::RefAST(_t),VARPTR);
		}
		return res;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	{
		ProgNodeP tmp67_AST_in = _t;
		match(antlr::RefAST(_t),VAR);
		_t = _t->getNextSibling();
		break;
	}
	case VARPTR:
	{
		ProgNodeP tmp68_AST_in = _t;
		match(antlr::RefAST(_t),VARPTR);
		_t = _t->getNextSibling();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

void GDLInterpreter::parameter_def(ProgNodeP _t,
	EnvBaseT* actEnv
) {
	ProgNodeP parameter_def_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	auto_ptr<EnvBaseT> guard(actEnv); 
	try{
	
	_retTree = _t;
	while(_retTree != NULL) {
	static_cast<ParameterNode*>(_retTree)->Parameter( actEnv);
	}    
	
	actEnv->Extra(); // expand _EXTRA
	
	} 
	catch( GDLException& e)
	{
	// update line number, currently set to caller->CallingNode()
	// because actEnv is not on the stack yet, 
	// report caller->Pro()'s name is ok, because we are not inside
	// the call yet
	e.SetErrorNodeP( actEnv->CallingNode());
	throw e;
	}
	
		guard.release();
		
	return;
	
	
	{
	ProgNodeP __t174 = _t;
	ProgNodeP tmp69_AST_in = _t;
	match(antlr::RefAST(_t),KEYDEF_REF);
	_t = _t->getFirstChild();
	ProgNodeP tmp70_AST_in = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	_t = __t174;
	_t = _t->getNextSibling();
	}
	_retTree = _t;
}

BaseGDL**  GDLInterpreter::l_indexable_expr(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_indexable_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case EXPR:
	{
		ProgNodeP __t74 = _t;
		ProgNodeP tmp71_AST_in = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getFirstChild();
		res=l_expr(_t, NULL);
		_t = _retTree;
		_t = __t74;
		_t = _t->getNextSibling();
		
		if( *res == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(res),true,false);
		
		break;
	}
	case FCALL:
	case FCALL_LIB:
	case MFCALL:
	case MFCALL_PARENT:
	{
		res=l_function_call(_t);
		_t = _retTree;
		
		if( *res == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(res),true,false);
		
		break;
	}
	case ARRAYEXPR_MFCALL:
	{
		res=l_arrayexpr_mfcall_as_mfcall(_t);
		_t = _retTree;
		
		if( *res == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(res),true,false);
		
		break;
	}
	case DEREF:
	{
		res=l_deref(_t);
		_t = _retTree;
		
		if( *res == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(res),true,false);
		
		break;
	}
	case VAR:
	case VARPTR:
	{
		res=l_defined_simple_var(_t);
		_t = _retTree;
		break;
	}
	case SYSVAR:
	{
		res=l_sys_var(_t);
		_t = _retTree;
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_array_expr(ProgNodeP _t,
	BaseGDL* right
) {
	BaseGDL** res;
	ProgNodeP l_array_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	ArrayIndexListT* aL;
	ArrayIndexListGuard guard;
	
	
	ProgNodeP __t76 = _t;
	ProgNodeP tmp72_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYEXPR);
	_t = _t->getFirstChild();
	res=l_indexable_expr(_t);
	_t = _retTree;
	aL=arrayindex_list(_t);
	_t = _retTree;
	guard.reset(aL);
	_t = __t76;
	_t = _t->getNextSibling();
	
	if( right == NULL)
	throw GDLException( _t, 
	"Indexed expression not allowed in this context.",true,false);
	
	aL->AssignAt( *res, right);
	
	//             aL->SetVariable( *res);
	//             if( (*res)->EqType( right))
	//             {
	//                 (*res)->AssignAt( right, aL); // assigns inplace
	//             }
	//             else
	//             {
	//                 BaseGDL* rConv = right->Convert2( (*res)->Type(), BaseGDL::COPY);
	//                 auto_ptr<BaseGDL> conv_guard( rConv);
	//                 (*res)->AssignAt( rConv, aL); // assigns inplace
	//             }
	
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_arrayexpr_mfcall(ProgNodeP _t,
	BaseGDL* right
) {
	BaseGDL** res;
	ProgNodeP l_arrayexpr_mfcall_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP mp2 = ProgNodeP(antlr::nullAST);
	ProgNodeP dot = ProgNodeP(antlr::nullAST);
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	BaseGDL *self;
	EnvUDT*  newEnv;
	ProgNodeP startNode = _t;
	
	
	ProgNodeP __t151 = _t;
	ProgNodeP tmp73_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYEXPR_MFCALL);
	_t = _t->getFirstChild();
	
	ProgNodeP mark = _t;
	_t = _t->getNextSibling(); // skip DOT
	
	self=expr(_t);
	_t = _retTree;
	mp2 = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	
	auto_ptr<BaseGDL> self_guard(self);
	
	try {
	newEnv=new EnvUDT( self, mp2, "", true);
	self_guard.release();
	}
	catch( GDLException& ex)
	{
	goto tryARRAYEXPR;
	}
	
	parameter_def(_t, newEnv);
	_t = _retTree;
	_t = __t151;
	_t = _t->getNextSibling();
	
	// push environment onto call stack
	callStack.push_back(newEnv);
	
	// make the call
	res=call_lfun(static_cast<DSubUD*>(
	newEnv->GetPro())->GetTree());
	
	_retTree = startNode->getNextSibling();
	return res;
	
	tryARRAYEXPR:;
	_t = mark;
	
	ProgNodeP __t152 = _t;
	dot = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),DOT);
	_t = _t->getFirstChild();
	
	SizeT nDot=dot->nDot;
	auto_ptr<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
	
	l_dot_array_expr(_t, aD.get());
	_t = _retTree;
	{ // ( ... )+
	int _cnt154=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR || _t->getType() == IDENTIFIER)) {
			tag_array_expr(_t, aD.get());
			_t = _retTree;
		}
		else {
			if ( _cnt154>=1 ) { goto _loop154; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt154++;
	}
	_loop154:;
	}  // ( ... )+
	_t = __t152;
	_t = _t->getNextSibling();
	
	if( right == NULL)
	throw GDLException( _t, 
	"Struct expression not allowed in this context.",
	true,false);
	
	aD->Assign( right);
	
	res=NULL;
	
	_retTree = startNode->getNextSibling();
	return res;
	
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::array_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP array_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP ax = ProgNodeP(antlr::nullAST);
	
	ArrayIndexListT* aL;
	BaseGDL* r;
	ArrayIndexListGuard guard;
	auto_ptr<BaseGDL> r_guard;
	
	ExprListT        exprList; // for cleanup
	IxExprListT      ixExprList;
	SizeT nExpr;
	BaseGDL* s;
	
		ProgNodeP retTree = _t->getNextSibling();
	//	match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		
		switch ( _t->getType()) {
		case VAR:
		case CONSTANT:
		case DEREF:
		case SYSVAR:
		case VARPTR:
		{
	r=_t->EvalNC();
			//r=indexable_expr(_t);
			_t = _t->getNextSibling(); //_retTree;
			break;
		}
		case FCALL_LIB:
		{
			r=lib_function_call(_t);
			_t = _t->getNextSibling();
			
			if( !callStack.back()->Contains( r)) 
	r_guard.reset( r); // guard if no global data
			
			break;
		}
	// 	case ASSIGN:
	// 	case ASSIGN_REPLACE:
	// 	case ASSIGN_ARRAYEXPR_MFCALL:
	// 	case ARRAYDEF:
	// 	case ARRAYEXPR:
	// 	case ARRAYEXPR_MFCALL:
	// 	case EXPR:
	// 	case FCALL:
	// 	case FCALL_LIB_RETNEW:
	// 	case MFCALL:
	// 	case MFCALL_PARENT:
	// 	case NSTRUC:
	// 	case NSTRUC_REF:
	// 	case POSTDEC:
	// 	case POSTINC:
	// 	case STRUC:
	// 	case DEC:
	// 	case INC:
	// 	case DOT:
	// 	case QUESTION:
	default:
		{
			r=indexable_tmp_expr(_t);
			_t = _retTree;
			r_guard.reset( r);
			break;
		}
		}
		
	
		aL = _t->arrIxList;
		assert( aL != NULL);
		guard.reset(aL);
		
	//    ax = _t
	//	match(antlr::RefAST(_t),ARRAYIX);
		_t = _t->getFirstChild();
		
		nExpr = aL->NParam();
		
		if( nExpr == 0)
		{
	goto empty;
		}
		//                 if( nExpr > 1)
		//                 {
		//                     ixExprList.reserve( nExpr);
		//                     exprList.reserve( nExpr);
		//                 }
		//                if( nExpr == 0) goto empty;
		
		for (;;) {
	//		if ((_tokenSet_1.member(_t->getType()))) {
				switch ( _t->getType()) {
				case VAR:
				case CONSTANT:
				case DEREF:
				case SYSVAR:
				case VARPTR:
				{
					s=_t->EvalNC();//indexable_expr(_t);
					_t = _t->getNextSibling();//_retTree;
					break;
				}
				case FCALL_LIB:
				{
					s=lib_function_call(_t);
					_t = _retTree;
					
					if( !callStack.back()->Contains( s)) 
					exprList.push_back( s);
					
					break;
				}
	// 			case ASSIGN:
	// 			case ASSIGN_REPLACE:
	// 			case ASSIGN_ARRAYEXPR_MFCALL:
	// 			case ARRAYDEF:
	// 			case ARRAYEXPR:
	// 			case ARRAYEXPR_MFCALL:
	// 			case EXPR:
	// 			case FCALL:
	// 			case FCALL_LIB_RETNEW:
	// 			case MFCALL:
	// 			case MFCALL_PARENT:
	// 			case NSTRUC:
	// 			case NSTRUC_REF:
	// 			case POSTDEC:
	// 			case POSTINC:
	// 			case STRUC:
	// 			case DEC:
	// 			case INC:
	// 			case DOT:
	// 			case QUESTION:
	default:
				{
					s=indexable_tmp_expr(_t);
					_t = _retTree;
					exprList.push_back( s);
					break;
				}
				} // switch
				
				ixExprList.push_back( s);
				if( ixExprList.size() == nExpr)
	break; // finish
				
			} // for
	// 		else {
	// 			assert( 0);//goto _loop106;
	// 		}
			
		empty:
		//_retTree = ax;
		res = aL->Index( r, ixExprList);
		//                 aL->Init( ixExprList);
		//                 aL->SetVariable( r);
		//                 res=r->Index( aL);
		//                ClearTmpList();
	
	_retTree = retTree;
		return res;
	
	
	ProgNodeP __t101 = _t;
	ProgNodeP tmp74_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYEXPR);
	_t = _t->getFirstChild();
	{
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case CONSTANT:
	case DEREF:
	case SYSVAR:
	case VAR:
	case VARPTR:
	{
		r=indexable_expr(_t);
		_t = _retTree;
		break;
	}
	case ASSIGN:
	case ASSIGN_REPLACE:
	case ASSIGN_ARRAYEXPR_MFCALL:
	case ARRAYDEF:
	case ARRAYEXPR:
	case ARRAYEXPR_MFCALL:
	case EXPR:
	case FCALL:
	case FCALL_LIB_RETNEW:
	case MFCALL:
	case MFCALL_PARENT:
	case NSTRUC:
	case NSTRUC_REF:
	case POSTDEC:
	case POSTINC:
	case STRUC:
	case DEC:
	case INC:
	case DOT:
	case QUESTION:
	{
		r=indexable_tmp_expr(_t);
		_t = _retTree;
		r_guard.reset( r);
		break;
	}
	case FCALL_LIB:
	{
		r=lib_function_call(_t);
		_t = _retTree;
		
		if( !callStack.back()->Contains( r)) 
		r_guard.reset( r); // guard if no global data
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	ProgNodeP __t103 = _t;
	ax = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),ARRAYIX);
	_t = _t->getFirstChild();
	
	aL = ax->arrIxList;
	assert( aL != NULL);
	
	guard.reset(aL);
	
	nExpr = aL->NParam();
	
	if( nExpr == 0)
	{
	goto empty2;
	}
	//                 if( nExpr > 1)
	//                 {
	//                     ixExprList.reserve( nExpr);
	//                     exprList.reserve( nExpr);
	//                 }
	//                if( nExpr == 0) goto empty;
	
	{ // ( ... )*
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_1.member(_t->getType()))) {
			{
			if (_t == ProgNodeP(antlr::nullAST) )
				_t = ASTNULL;
			switch ( _t->getType()) {
			case CONSTANT:
			case DEREF:
			case SYSVAR:
			case VAR:
			case VARPTR:
			{
				s=indexable_expr(_t);
				_t = _retTree;
				break;
			}
			case FCALL_LIB:
			{
				s=lib_function_call(_t);
				_t = _retTree;
				
				if( !callStack.back()->Contains( s)) 
				exprList.push_back( s);
				
				break;
			}
			case ASSIGN:
			case ASSIGN_REPLACE:
			case ASSIGN_ARRAYEXPR_MFCALL:
			case ARRAYDEF:
			case ARRAYEXPR:
			case ARRAYEXPR_MFCALL:
			case EXPR:
			case FCALL:
			case FCALL_LIB_RETNEW:
			case MFCALL:
			case MFCALL_PARENT:
			case NSTRUC:
			case NSTRUC_REF:
			case POSTDEC:
			case POSTINC:
			case STRUC:
			case DEC:
			case INC:
			case DOT:
			case QUESTION:
			{
				s=indexable_tmp_expr(_t);
				_t = _retTree;
				exprList.push_back( s);
				break;
			}
			default:
			{
				throw antlr::NoViableAltException(antlr::RefAST(_t));
			}
			}
			}
			
			ixExprList.push_back( s);
			if( ixExprList.size() == nExpr)
			break; // allows some manual tuning
			
		}
		else {
			goto _loop106;
		}
		
	}
	_loop106:;
	} // ( ... )*
	_t = __t103;
	_t = _t->getNextSibling();
	
	empty2:
	//_retTree = ax;
	res = aL->Index( r, ixExprList);
	//                 aL->Init( ixExprList);
	//                 aL->SetVariable( r);
	//                 res=r->Index( aL);
	//                ClearTmpList();
	
	_t = __t101;
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

void GDLInterpreter::tag_expr(ProgNodeP _t,
	DotAccessDescT* aD
) {
	ProgNodeP tag_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP i = ProgNodeP(antlr::nullAST);
	
	BaseGDL* e;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case EXPR:
	{
		ProgNodeP __t108 = _t;
		ProgNodeP tmp75_AST_in = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getFirstChild();
		e=expr(_t);
		_t = _retTree;
		
		auto_ptr<BaseGDL> e_guard(e);
		
		SizeT tagIx;
		int ret=e->Scalar2index(tagIx);
		if( ret < 1)
		throw GDLException( _t, "Expression must be a scalar"
		" >= 0 in this context: "+Name(e),true,false);
		
		aD->Add( tagIx);
		
		_t = __t108;
		_t = _t->getNextSibling();
		break;
	}
	case IDENTIFIER:
	{
		i = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		std::string tagName=i->getText();
		aD->Add( tagName);
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
}

BaseGDL*  GDLInterpreter::r_dot_indexable_expr(ProgNodeP _t,
	DotAccessDescT* aD
) {
	BaseGDL* res;
	ProgNodeP r_dot_indexable_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	BaseGDL** e;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case EXPR:
	{
		ProgNodeP __t112 = _t;
		ProgNodeP tmp76_AST_in = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getFirstChild();
		res=expr(_t);
		_t = _retTree;
		aD->SetOwner( true);
		_t = __t112;
		_t = _t->getNextSibling();
		break;
	}
	case VAR:
	case VARPTR:
	{
		e=l_defined_simple_var(_t);
		_t = _retTree;
		res = *e;
		break;
	}
	case SYSVAR:
	{
		res=sys_var_nocopy(_t);
		_t = _retTree;
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::sys_var_nocopy(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP sys_var_nocopy_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP sysVar = ProgNodeP(antlr::nullAST);
	
		ProgNodeP& sysVarRef = _t;
		
	// 	match(antlr::RefAST(_t),SYSVAR);
		
		if( sysVarRef->var == NULL) 
		{
		sysVarRef->var=FindInVarList(sysVarList,sysVarRef->getText());
		if( sysVarRef->var == NULL)		    
		throw GDLException( _t, "Not a legal system variable: !"+
		sysVarRef->getText(),true,false);
		}
		
		if( sysVarRef->getText() == "STIME") SysVar::UpdateSTime();
		
		// note: system variables are always defined
		
		_retTree = _t->getNextSibling();;
		return sysVarRef->var->Data(); // no ->Dup()
	
	
	sysVar = _t;
	match(antlr::RefAST(_t),SYSVAR);
	_t = _t->getNextSibling();
	
	if( sysVar->var == NULL) 
	{
	sysVar->var=FindInVarList(sysVarList,sysVar->getText());
	if( sysVar->var == NULL)		    
	throw GDLException( _t, "Not a legal system variable: !"+
	sysVar->getText(),true,false);
	}
	
	if( sysVar->getText() == "STIME") SysVar::UpdateSTime();
	
	// system variables are always defined
	res=sysVar->var->Data(); // no ->Dup()
	
	_retTree = _t;
	return res;
}

void GDLInterpreter::r_dot_array_expr(ProgNodeP _t,
	DotAccessDescT* aD
) {
	ProgNodeP r_dot_array_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	ArrayIndexListT* aL;
	BaseGDL*         r;
	DStructGDL*      structR;
	ArrayIndexListGuard guard;
	bool isObj = callStack.back()->IsObject();
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	{
		ProgNodeP __t114 = _t;
		ProgNodeP tmp77_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		r=r_dot_indexable_expr(_t, aD);
		_t = _retTree;
		aL=arrayindex_list(_t);
		_t = _retTree;
		guard.reset(aL);
		_t = __t114;
		_t = _t->getNextSibling();
		
		// check here for object and get struct
		structR=dynamic_cast<DStructGDL*>(r);
		if( structR == NULL)
		{
		if( isObj)
		{
		DStructGDL* oStruct = ObjectStructCheckAccess( r, _t);
		
		//                    DStructGDL* obj = oStruct->Index( aL);
		
		if( aD->IsOwner()) delete r; 
		aD->SetOwner( false); // object struct, not owned
		
		aD->Root( oStruct, guard.release()); 
		//                    aD->Root( obj); 
		
		//                     BaseGDL* obj = r->Index( aL);
		//                     auto_ptr<BaseGDL> objGuard( obj); // new object -> guard
		
		//                     DStructGDL* oStruct = ObjectStructCheckAccess( obj, _t);
		
		//                     // oStruct cannot be "Assoc_"
		//                     if( aD->IsOwner()) delete r; 
		//                     aD->SetOwner( false); // object structs are never owned
		//                     aD->Root( oStruct); 
		}
		else
		{
		throw GDLException( _t, "Expression must be a"
		" STRUCT in this context: "+Name(r),true,false);
		}
		}
		else
		{
		if( r->IsAssoc())
		throw GDLException( _t, "File expression not allowed "
		"in this context: "+Name(r),true,false);
		
		aD->Root( structR, guard.release()); 
		}
		
		break;
	}
	case EXPR:
	case SYSVAR:
	case VAR:
	case VARPTR:
	{
		r=r_dot_indexable_expr(_t, aD);
		_t = _retTree;
		
		// check here for object and get struct
		structR = dynamic_cast<DStructGDL*>(r);
		if( structR == NULL)
		{
		if( isObj) // memeber access to object?
		{
		DStructGDL* oStruct = ObjectStructCheckAccess( r, _t);
		
		// oStruct cannot be "Assoc_"
		if( aD->IsOwner()) delete r;
		aD->SetOwner( false); // object structs are never owned
		aD->Root( oStruct); 
		}
		else
		{
		throw GDLException( _t, "Expression must be a"
		" STRUCT in this context: "+Name(r),true,false);
		}
		}
		else
		{
		if( r->IsAssoc())
		{
		throw GDLException( _t, "File expression not allowed "
		"in this context: "+Name(r),true,false);
		}
		
		aD->Root(structR); 
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

BaseGDL*  GDLInterpreter::dot_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP dot_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP dot = ProgNodeP(antlr::nullAST);
	
	
		ProgNodeP rTree = _t->getNextSibling();
		//ProgNodeP 
	dot = _t;
	// 	match(antlr::RefAST(_t),DOT);
		_t = _t->getFirstChild();
		
		SizeT nDot=dot->nDot;
		auto_ptr<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
		
		r_dot_array_expr(_t, aD.get());
		_t = _retTree;
		for (; _t != NULL;) {
				tag_array_expr(_t, aD.get());
				_t = _retTree;
		}
		res= aD->Resolve();
		_retTree = rTree;
		return res;
	
	
	ProgNodeP __t116 = _t;
	dot = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),DOT);
	_t = _t->getFirstChild();
	r_dot_array_expr(_t, aD.get());
	_t = _retTree;
	{ // ( ... )+
	int _cnt118=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR || _t->getType() == IDENTIFIER)) {
			tag_array_expr(_t, aD.get());
			_t = _retTree;
		}
		else {
			if ( _cnt118>=1 ) { goto _loop118; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt118++;
	}
	_loop118:;
	}  // ( ... )+
	_t = __t116;
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::assign_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP assign_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	BaseGDL** l;
	BaseGDL*  r;
	
		ProgNodeP startNode = _t;
		if( _t->getType() == ASSIGN) 
		{
	// 		match(antlr::RefAST(_t),ASSIGN);
			_t = _t->getFirstChild();
			
			auto_ptr<BaseGDL> r_guard;
			
			if( _t->getType() == FCALL_LIB)
			{
				res=lib_function_call(_t);
				_t = _retTree;
				
				if( !callStack.back()->Contains( res)) 
				r_guard.reset( res);
				
			}
	else
			{
				res=tmp_expr(_t);
				_t = _retTree;
				
				r_guard.reset( res);
				
			}
			
			l=l_expr(_t, res);
			_t = _retTree;
			
			if( r_guard.get() == res) // owner
			r_guard.release();
			else
			res = res->Dup();
			
		}
	else if( _t->getType() == ASSIGN_ARRAYEXPR_MFCALL) 
	{
	
			_t = _t->getFirstChild();
			
			auto_ptr<BaseGDL> r_guard;
			
			if( _t->getType() == FCALL_LIB)
			{
				res=lib_function_call(_t);
				_t = _retTree;
				
				if( !callStack.back()->Contains( res)) 
				r_guard.reset( res);
				
			}
	else
			{
				res=tmp_expr(_t);
				_t = _retTree;
				
				r_guard.reset( res);
				
			}
	
	ProgNodeP mark = _t;
	
	// try MFCALL
	try
	{
	l=l_arrayexpr_mfcall_as_mfcall( mark);
	
	if( res != (*l))
	{
	delete *l;
	*l = res->Dup();     
			
	if( r_guard.get() == res) // owner
	{
	r_guard.release(); 
	}
	else
	res = res->Dup();
	}
	}
	catch( GDLException& ex)
	{
	// try ARRAYEXPR
	try
		                {
	l=l_arrayexpr_mfcall_as_arrayexpr(mark, res);
	
	if( r_guard.get() == res) // owner
	r_guard.release();
	else
	res = res->Dup();
		                }
	catch( GDLException& ex2)
	{
	throw GDLException(ex.toString() + " or "+ex2.toString());
	}
	}
	
	}
	else
		{
	// 		match(antlr::RefAST(_t),ASSIGN_REPLACE);
			_t = _t->getFirstChild();
			
			auto_ptr<BaseGDL> r_guard;
			
			if( _t->getType() == FCALL_LIB)
			{
	
				res=lib_function_call(_t);
				_t = _retTree;
				
				if( !callStack.back()->Contains( res)) 
				r_guard.reset( res);
				
			}
	else
			{
				res=tmp_expr(_t);
				_t = _retTree;
				
				r_guard.reset( res);
				
			}
	
			
			switch ( _t->getType()) {
			case DEREF:
			{
				l=l_deref(_t);
				_t = _retTree;
				break;
			}
			case VAR:
			case VARPTR:
			{
				l=l_simple_var(_t);
				_t = _retTree;
				break;
			}
			default:
			{
				l=l_function_call(_t);
				_t = _retTree;
				break;
			}
			}
			
			if( res != (*l))
			{
			delete *l;
			*l = res->Dup();     
			
			if( r_guard.get() == res) // owner
			{
			r_guard.release(); 
			}
			else
			res = res->Dup();
			}
	}
	
		_retTree = startNode->getNextSibling();
		return res;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	{
		ProgNodeP __t128 = _t;
		ProgNodeP tmp78_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case VAR:
		case VARPTR:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			res=tmp_expr(_t);
			_t = _retTree;
			
			r_guard.reset( res);
			
			break;
		}
		case FCALL_LIB:
		{
			res=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( res)) 
			r_guard.reset( res);
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		l=l_expr(_t, res);
		_t = _retTree;
		
		if( r_guard.get() == res) // owner
		r_guard.release();
		else
		res = res->Dup();
		
		_t = __t128;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_ARRAYEXPR_MFCALL:
	{
		ProgNodeP __t130 = _t;
		ProgNodeP tmp79_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_ARRAYEXPR_MFCALL);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case VAR:
		case VARPTR:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			res=tmp_expr(_t);
			_t = _retTree;
			
			r_guard.reset( res);
			
			break;
		}
		case FCALL_LIB:
		{
			res=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( res)) 
			r_guard.reset( res);
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		l=l_expr(_t, res);
		_t = _retTree;
		
		if( r_guard.get() == res) // owner
		r_guard.release();
		else
		res = res->Dup();
		
		_t = __t130;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_REPLACE:
	{
		ProgNodeP __t132 = _t;
		ProgNodeP tmp80_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYEXPR:
		case ARRAYEXPR_MFCALL:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case FCALL_LIB_RETNEW:
		case MFCALL:
		case MFCALL_PARENT:
		case NSTRUC:
		case NSTRUC_REF:
		case POSTDEC:
		case POSTINC:
		case STRUC:
		case SYSVAR:
		case VAR:
		case VARPTR:
		case DEC:
		case INC:
		case DOT:
		case QUESTION:
		{
			res=tmp_expr(_t);
			_t = _retTree;
			
			r_guard.reset( res);
			
			break;
		}
		case FCALL_LIB:
		{
			res=lib_function_call(_t);
			_t = _retTree;
			
			if( !callStack.back()->Contains( res)) 
			r_guard.reset( res);
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case FCALL:
		case FCALL_LIB:
		case MFCALL:
		case MFCALL_PARENT:
		{
			l=l_function_call(_t);
			_t = _retTree;
			break;
		}
		case DEREF:
		{
			l=l_deref(_t);
			_t = _retTree;
			break;
		}
		case VAR:
		case VARPTR:
		{
			l=l_simple_var(_t);
			_t = _retTree;
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		if( res != (*l))
		{
		delete *l;
		*l = res->Dup();     
		
		if( r_guard.get() == res) // owner
		{
		r_guard.release(); 
		}
		else
		res = res->Dup();
		}
		
		_t = __t132;
		_t = _t->getNextSibling();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

 BaseGDL*  GDLInterpreter::function_call(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP function_call_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP mp = ProgNodeP(antlr::nullAST);
	ProgNodeP parent = ProgNodeP(antlr::nullAST);
	ProgNodeP p = ProgNodeP(antlr::nullAST);
	ProgNodeP f = ProgNodeP(antlr::nullAST);
	ProgNodeP mp2 = ProgNodeP(antlr::nullAST);
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	BaseGDL *self;
	EnvUDT*  newEnv;
	ProgNodeP startNode = _t;
	ProgNodeP mark;
	
	
	{
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case MFCALL:
	{
		ProgNodeP __t146 = _t;
		ProgNodeP tmp81_AST_in = _t;
		match(antlr::RefAST(_t),MFCALL);
		_t = _t->getFirstChild();
		self=expr(_t);
		_t = _retTree;
		mp = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		auto_ptr<BaseGDL> self_guard(self);
		
		newEnv=new EnvUDT( self, mp);
		
		self_guard.release();
		
		parameter_def(_t, newEnv);
		_t = _retTree;
		_t = __t146;
		_t = _t->getNextSibling();
		break;
	}
	case MFCALL_PARENT:
	{
		ProgNodeP __t147 = _t;
		ProgNodeP tmp82_AST_in = _t;
		match(antlr::RefAST(_t),MFCALL_PARENT);
		_t = _t->getFirstChild();
		self=expr(_t);
		_t = _retTree;
		parent = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		p = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		auto_ptr<BaseGDL> self_guard(self);
		
		newEnv=new EnvUDT( self, p,
		parent->getText());
		
		self_guard.release();
		
		parameter_def(_t, newEnv);
		_t = _retTree;
		_t = __t147;
		_t = _t->getNextSibling();
		break;
	}
	case FCALL:
	{
		ProgNodeP __t148 = _t;
		f = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),FCALL);
		_t = _t->getFirstChild();
		
		SetFunIx( f);
		
		newEnv=new EnvUDT( f, funList[f->funIx]);
		
		parameter_def(_t, newEnv);
		_t = _retTree;
		_t = __t148;
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR_MFCALL:
	{
		ProgNodeP __t149 = _t;
		ProgNodeP tmp83_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR_MFCALL);
		_t = _t->getFirstChild();
		
		mark = _t;
		_t = _t->getNextSibling(); // skip DOT
		
		self=expr(_t);
		_t = _retTree;
		mp2 = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		auto_ptr<BaseGDL> self_guard(self);
		
		try {
		newEnv=new EnvUDT( self, mp2);
		self_guard.release();
		}
		catch( GDLException& ex)
		{
		goto tryARRAYEXPR;
		}
		
		parameter_def(_t, newEnv);
		_t = _retTree;
		_t = __t149;
		_t = _t->getNextSibling();
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	}
	
	// push environment onto call stack
	callStack.push_back(newEnv);
	
	// make the call
	res=call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
	
	_retTree = startNode->getNextSibling();
	return res;
	
	tryARRAYEXPR:;
	//_t = mark;
	
	ProgNodeP dot = mark;
	// 	match(antlr::RefAST(_t),DOT);
	_t = mark->getFirstChild();
		
	SizeT nDot=dot->nDot;
	auto_ptr<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
		
	r_dot_array_expr(_t, aD.get());
	_t = _retTree;
	for (; _t != NULL;) {
	tag_array_expr(_t, aD.get());
	_t = _retTree;
	}
	res= aD->Resolve();
	
	_retTree = startNode->getNextSibling();
	return res;
	
	_retTree = _t;
	return res;
}

 BaseGDL*  GDLInterpreter::lib_function_call_retnew(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP lib_function_call_retnew_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP fll = ProgNodeP(antlr::nullAST);
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	
		ProgNodeP rTree = _t->getNextSibling();
	
	// 	match(antlr::RefAST(_t),FCALL_LIB_RETNEW);
	//	_t = _t->getFirstChild();
	// 	match(antlr::RefAST(_t),IDENTIFIER);
		EnvT* newEnv=new EnvT( _t, _t->libFun);//libFunList[fl->funIx]);
	
	// 	_t =_t->getFirstChild();
		
	// 	EnvT* newEnv=new EnvT( fl, fl->libFun);//libFunList[fl->funIx]);
	static int n_elementsIx = LibFunIx("N_ELEMENTS");
	static DLibFun* n_elementsFun = libFunList[n_elementsIx];
	
	if( _t->libFun == n_elementsFun)
	{
	parameter_def_n_elements(_t->getFirstChild(), newEnv);
	}
	else
	{
	parameter_def(_t->getFirstChild(), newEnv);
	}
	
	
	//	parameter_def(_t->getFirstChild(), newEnv);
		
		// push id.pro onto call stack
		callStack.push_back(newEnv);
		// make the call
		//BaseGDL* 
	res=static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv);
		//*** MUST always return a defined expression
		
		_retTree = rTree;
		return res;
	
	
	ProgNodeP __t143 = _t;
	fll = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),FCALL_LIB_RETNEW);
	_t = _t->getFirstChild();
	
	//EnvT* 
	newEnv=new EnvT( fll, fll->libFun);//libFunList[fl->funIx]);
	
	parameter_def(_t, newEnv);
	_t = _retTree;
	
	// push id.pro onto call stack
	callStack.push_back(newEnv);
	// make the call
	res=static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv);
	//*** MUST always return a defined expression
	
	_t = __t143;
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::constant(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP constant_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		_retTree = _t->getNextSibling();
		return _t->cData->Dup(); 
	
	
	ProgNodeP tmp84_AST_in = _t;
	match(antlr::RefAST(_t),CONSTANT);
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::simple_var(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP simple_var_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP var = ProgNodeP(antlr::nullAST);
	ProgNodeP varPtr = ProgNodeP(antlr::nullAST);
	
		assert( _t != NULL);
	
		if( _t->getType() == VAR)
		{
			ProgNodeP var = _t;
			match(antlr::RefAST(_t),VAR);
			_t = _t->getNextSibling();
			
			BaseGDL* vData=callStack.back()->GetKW( var->varIx);
			
			if( vData == NULL)
			throw GDLException( _t, "Variable is undefined: "+var->getText(),true,false);
			
			res=vData->Dup();
			
		}
		else // VARPTR
		{
			ProgNodeP varPtr = _t;
			match(antlr::RefAST(_t),VARPTR);
			_t = _t->getNextSibling();
			
			BaseGDL* vData=varPtr->var->Data();
			
			if( vData == NULL)
			throw GDLException( _t, "Common block variable is undefined.",true,false);
			
			res=vData->Dup();
			
		}
	
		_retTree = _t;
		return res;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	{
		var = _t;
		match(antlr::RefAST(_t),VAR);
		_t = _t->getNextSibling();
		
		BaseGDL* vData=callStack.back()->GetKW( var->varIx);
		
		if( vData == NULL)
		throw GDLException( _t, "Variable is undefined: "+var->getText(),true,false);
		
		res=vData->Dup();
		
		break;
	}
	case VARPTR:
	{
		varPtr = _t;
		match(antlr::RefAST(_t),VARPTR);
		_t = _t->getNextSibling();
		
		BaseGDL* vData=varPtr->var->Data();
		
		if( vData == NULL)
		throw GDLException( _t, "Common block variable is undefined.",true,false);
		
		res=vData->Dup();
		
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::sys_var(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP sys_var_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		BaseGDL* sv = sys_var_nocopy(_t);
		//_t = _retTree;
		
		//BaseGDL* 
	res=sv->Dup();
		
		//_retTree = _t;
		return res;
	
	
	sv=sys_var_nocopy(_t);
	_t = _retTree;
	
	res=sv->Dup();
	
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_arrayexpr_mfcall_as_arrayexpr(ProgNodeP _t,
	BaseGDL* right
) {
	BaseGDL** res;
	ProgNodeP l_arrayexpr_mfcall_as_arrayexpr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP dot = ProgNodeP(antlr::nullAST);
	
	ProgNodeP __t156 = _t;
	ProgNodeP tmp85_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYEXPR_MFCALL);
	_t = _t->getFirstChild();
	ProgNodeP __t157 = _t;
	dot = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),DOT);
	_t = _t->getFirstChild();
	
	SizeT nDot=dot->nDot;
	auto_ptr<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
	
	l_dot_array_expr(_t, aD.get());
	_t = _retTree;
	{ // ( ... )+
	int _cnt159=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR || _t->getType() == IDENTIFIER)) {
			tag_array_expr(_t, aD.get());
			_t = _retTree;
		}
		else {
			if ( _cnt159>=1 ) { goto _loop159; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt159++;
	}
	_loop159:;
	}  // ( ... )+
	_t = __t157;
	_t = _t->getNextSibling();
	_t = __t156;
	_t = _t->getNextSibling();
	
	if( right == NULL)
	throw GDLException( _t, 
	"Struct expression not allowed in this context.",
	true,false);
	
	aD->Assign( right);
	
	res=NULL;
	
	_retTree = _t;
	return res;
}

 BaseGDL**  GDLInterpreter::ref_parameter(ProgNodeP _t) {
	 BaseGDL** ret;
	ProgNodeP ref_parameter_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		assert(_t != NULL);
	
			if ( _t->getType() == DEREF) {
				//ret=
	return l_deref(_t);
	// 			_t = _retTree;
			}
			else	
			//case VAR:
			//case VARPTR:
			{
				//ret=
	return l_simple_var(_t);
	// 			_t = _retTree;
			}
	
	//  	_retTree = _t;
	// 	return ret;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	case VARPTR:
	{
		ret=l_simple_var(_t);
		_t = _retTree;
		break;
	}
	case DEREF:
	{
		ret=l_deref(_t);
		_t = _retTree;
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return ret;
}

void GDLInterpreter::parameter_def_n_elements(ProgNodeP _t,
	EnvBaseT* actEnv
) {
	ProgNodeP parameter_def_n_elements_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	auto_ptr<EnvBaseT> guard(actEnv); 
	_retTree = _t;
	//     bool interruptEnableIn = interruptEnable;
	if( _retTree != NULL)
	{
	if( _retTree->getType() == REF ||
	_retTree->getType() == REF_EXPR ||
	_retTree->getType() == REF_CHECK ||
	_retTree->getType() == PARAEXPR)
	{
	try{
	//                     interruptEnable = false;
	static_cast<ParameterNode*>(_retTree)->Parameter( actEnv);
	//                     interruptEnable = interruptEnableIn;
	} 
	catch( GDLException& e)
	{
	//                         interruptEnable = interruptEnableIn;
	if( actEnv->NParam() == 0) 
	{
	BaseGDL* nP = NULL;
	actEnv->SetNextPar( nP);
	}
	}
	}
	}
	try{
	while(_retTree != NULL) {
	static_cast<ParameterNode*>(_retTree)->Parameter( actEnv);
	}    
	}
	catch( GDLException& e)
	{
	// update line number, currently set to caller->CallingNode()
	// because actEnv is not on the stack yet, 
	// report caller->Pro()'s name is ok, because we are not inside
	// the call yet
	e.SetErrorNodeP( actEnv->CallingNode());
	throw e;
	}
	
	actEnv->Extra(); // expand _EXTRA
	
		guard.release();
		
	return;
	
	
	ProgNodeP __t171 = _t;
	ProgNodeP tmp86_AST_in = _t;
	match(antlr::RefAST(_t),KEYDEF_REF_EXPR);
	_t = _t->getFirstChild();
	ProgNodeP tmp87_AST_in = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	_t = __t171;
	_t = _t->getNextSibling();
	_retTree = _t;
}

void GDLInterpreter::initializeASTFactory( antlr::ASTFactory& )
{
}
const char* GDLInterpreter::tokenNames[] = {
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
	"ARRAYIX",
	"ARRAYIX_ALL",
	"ARRAYIX_ORANGE",
	"ARRAYIX_RANGE",
	"ARRAYIX_ORANGE_S",
	"ARRAYIX_RANGE_S",
	"ARRAYEXPR",
	"ARRAYEXPR_FN",
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
	"FOR_LOOP",
	"FOR_STEP_LOOP",
	"FOREACH_LOOP",
	"FCALL",
	"FCALL_LIB",
	"FCALL_LIB_RETNEW",
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
	"POSTDEC",
	"POSTINC",
	"DECSTATEMENT",
	"INCSTATEMENT",
	"REF",
	"REF_CHECK",
	"REF_EXPR",
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
	0
};

const unsigned long GDLInterpreter::_tokenSet_0_data_[] = { 3232760224UL, 234938511UL, 557920UL, 402948256UL, 0UL, 0UL, 0UL, 0UL };
// ASSIGN ASSIGN_REPLACE ASSIGN_ARRAYEXPR_MFCALL BLOCK BREAK CONTINUE "for" 
// FOR_STEP "foreach" FOR_LOOP FOR_STEP_LOOP FOREACH_LOOP IF_ELSE LABEL 
// MPCALL MPCALL_PARENT ON_IOERROR_NULL PCALL PCALL_LIB "repeat" REPEAT_LOOP 
// RETF RETP "while" "case" "goto" "if" "on_ioerror" "switch" DEC INC 
const antlr::BitSet GDLInterpreter::_tokenSet_0(_tokenSet_0_data_,8);
const unsigned long GDLInterpreter::_tokenSet_1_data_[] = { 738853792UL, 3246981232UL, 27648UL, 402653184UL, 0UL, 0UL, 8194UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// ASSIGN ASSIGN_REPLACE ASSIGN_ARRAYEXPR_MFCALL ARRAYDEF ARRAYEXPR ARRAYEXPR_MFCALL 
// CONSTANT DEREF EXPR FCALL FCALL_LIB FCALL_LIB_RETNEW MFCALL MFCALL_PARENT 
// NSTRUC NSTRUC_REF POSTDEC POSTINC STRUC SYSVAR VAR VARPTR DEC INC DOT 
// QUESTION 
const antlr::BitSet GDLInterpreter::_tokenSet_1(_tokenSet_1_data_,16);


