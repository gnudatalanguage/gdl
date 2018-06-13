/* $ANTLR 2.7.7 (2006-11-01): "gdlc.i.g" -> "GDLInterpreter.cpp"$ */

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
	
	//_t->setLine(0);
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
		
		// track actual line number
		callStack.back()->SetLineNumber( last->getLine());
		
		retCode = last->Run(); // Run() sets _retTree
		
		}
		while( 
		_retTree != NULL && 
		retCode == RC_OK && 
		!(sigControlC && interruptEnable) && 
		(debugMode == DEBUG_CLEAR));
		
		// commented out, because we are only at the last statement
		// if( _retTree != NULL) 
		//     last = _retTree;
		
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
		case FOREACH_INDEX:
		{
			ProgNodeP tmp14_AST_in = _t;
			match(antlr::RefAST(_t),FOREACH_INDEX);
			_t = _t->getNextSibling();
			break;
		}
		case FOREACH_INDEX_LOOP:
		{
			ProgNodeP tmp15_AST_in = _t;
			match(antlr::RefAST(_t),FOREACH_INDEX_LOOP);
			_t = _t->getNextSibling();
			break;
		}
		case FOR_STEP:
		{
			ProgNodeP tmp16_AST_in = _t;
			match(antlr::RefAST(_t),FOR_STEP);
			_t = _t->getNextSibling();
			break;
		}
		case FOR_STEP_LOOP:
		{
			ProgNodeP tmp17_AST_in = _t;
			match(antlr::RefAST(_t),FOR_STEP_LOOP);
			_t = _t->getNextSibling();
			break;
		}
		case REPEAT:
		{
			ProgNodeP tmp18_AST_in = _t;
			match(antlr::RefAST(_t),REPEAT);
			_t = _t->getNextSibling();
			break;
		}
		case REPEAT_LOOP:
		{
			ProgNodeP tmp19_AST_in = _t;
			match(antlr::RefAST(_t),REPEAT_LOOP);
			_t = _t->getNextSibling();
			break;
		}
		case WHILE:
		{
			ProgNodeP tmp20_AST_in = _t;
			match(antlr::RefAST(_t),WHILE);
			_t = _t->getNextSibling();
			break;
		}
		case IF:
		{
			ProgNodeP tmp21_AST_in = _t;
			match(antlr::RefAST(_t),IF);
			_t = _t->getNextSibling();
			break;
		}
		case IF_ELSE:
		{
			ProgNodeP tmp22_AST_in = _t;
			match(antlr::RefAST(_t),IF_ELSE);
			_t = _t->getNextSibling();
			break;
		}
		case CASE:
		{
			ProgNodeP tmp23_AST_in = _t;
			match(antlr::RefAST(_t),CASE);
			_t = _t->getNextSibling();
			break;
		}
		case SWITCH:
		{
			ProgNodeP tmp24_AST_in = _t;
			match(antlr::RefAST(_t),SWITCH);
			_t = _t->getNextSibling();
			break;
		}
		case BLOCK:
		{
			ProgNodeP tmp25_AST_in = _t;
			match(antlr::RefAST(_t),BLOCK);
			_t = _t->getNextSibling();
			break;
		}
		case LABEL:
		{
			ProgNodeP tmp26_AST_in = _t;
			match(antlr::RefAST(_t),LABEL);
			_t = _t->getNextSibling();
			break;
		}
		case ON_IOERROR_NULL:
		{
			ProgNodeP tmp27_AST_in = _t;
			match(antlr::RefAST(_t),ON_IOERROR_NULL);
			_t = _t->getNextSibling();
			break;
		}
		case ON_IOERROR:
		{
			ProgNodeP tmp28_AST_in = _t;
			match(antlr::RefAST(_t),ON_IOERROR);
			_t = _t->getNextSibling();
			break;
		}
		case BREAK:
		{
			ProgNodeP tmp29_AST_in = _t;
			match(antlr::RefAST(_t),BREAK);
			_t = _t->getNextSibling();
			break;
		}
		case CONTINUE:
		{
			ProgNodeP tmp30_AST_in = _t;
			match(antlr::RefAST(_t),CONTINUE);
			_t = _t->getNextSibling();
			break;
		}
		case GOTO:
		{
			ProgNodeP tmp31_AST_in = _t;
			match(antlr::RefAST(_t),GOTO);
			_t = _t->getNextSibling();
			break;
		}
		case RETF:
		{
			ProgNodeP tmp32_AST_in = _t;
			match(antlr::RefAST(_t),RETF);
			_t = _t->getNextSibling();
			break;
		}
		case RETP:
		{
			ProgNodeP tmp33_AST_in = _t;
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
		
		retCode = NewInterpreterInstance( last->getLine());//-1);
		}
		else if( debugMode != DEBUG_CLEAR)
		{
		if( debugMode == DEBUG_STOP)
		{
		DebugMsg( last, "Stop encountered: ");
		if( !interruptEnable)
		debugMode = DEBUG_PROCESS_STOP;
		}
		
		if( debugMode == DEBUG_STEP)
		{
		if( stepCount == 1)
		{
		stepCount = 0;
		DebugMsg( last, "Stepped to: ");
		
		debugMode = DEBUG_CLEAR;
		
		retCode = NewInterpreterInstance( last->getLine());//-1);
		}
		else
		{
		--stepCount;
		#ifdef GDL_DEBUG
		std::cout << "stepCount-- = " << stepCount << std::endl;
		#endif
		}
		}
		// else if( debugMode == DEBUG_SKIP)
		//     {
		//         if( last != NULL)
		//             {
		//                 last = last->getNextSibling();
		//                 DebugMsg( last, "Skipped to: ");
		//             }
		//         else
		//             DebugMsg( last, "Cannot SKIP fro here");
		
		//         debugMode = DEBUG_CLEAR;
		//         retCode = RC_OK;
		//     }
		else if( interruptEnable)
		{
		if( debugMode == DEBUG_PROCESS_STOP)
		{
		DebugMsg( last, "Stepped to: ");
		}
		
		debugMode = DEBUG_CLEAR;
		
		retCode = NewInterpreterInstance( last->getLine());//-1);
		}   
		else
		{
		retCode = RC_ABORT;
		}
		}
		return retCode;
		
	}
	catch ( GDLException& e) {
		
		// reset _retTree to last statement
		// (might otherwise be inside an expression in which case 
		// .CONTINUE does not work)
		_retTree = last; 
		
		if( last->IsWrappedNode())
		throw e; // WRAPPED_... nodes should not stop inside
		
		// set !ERROR_STATE sys var 
		DStructDesc* errorStateDesc = SysVar::Error_State()->Desc();   //MUST NOT BE STATIC, due to .reset 
		static unsigned nameTag = errorStateDesc->TagIndex( "NAME");
		static unsigned codeTag = errorStateDesc->TagIndex( "CODE");
		static unsigned msgTag = errorStateDesc->TagIndex( "MSG");
		
		if( e.IsIOException())
		{
		assert( dynamic_cast< GDLIOException*>( &e) != NULL);
		// set the jump target - also logs the jump
		ProgNodeP onIOErr = 
		static_cast<EnvUDT*>(callStack.back())->GetIOError();
		if( onIOErr != NULL)
		{
		DStructGDL* errorState = SysVar::Error_State();
		(*static_cast<DStringGDL*>( errorState->GetTag( nameTag)))[0] = 
		"IDL_M_FAILURE";
		(*static_cast<DLongGDL*>( errorState->GetTag( codeTag)))[0] = 
		e.ErrorCode();
		SysVar::SetErrError( e.ErrorCode());
		(*static_cast<DStringGDL*>( errorState->GetTag( msgTag)))[0] = 
		e.getMessage();
		SysVar::SetErr_String( e.getMessage());
		
		_retTree = onIOErr;
		return RC_OK;
		}
		}
		
		// handle CATCH
		ProgNodeP catchNode = callStack.back()->GetCatchNode();
		if( catchNode != NULL)
		{
		DStructGDL* errorState = SysVar::Error_State();
		(*static_cast<DStringGDL*>( errorState->GetTag( nameTag)))[0] = 
		"IDL_M_FAILURE";
		(*static_cast<DLongGDL*>( errorState->GetTag( codeTag)))[0] = 
		e.ErrorCode();
		SysVar::SetErrError( e.ErrorCode());
		(*static_cast<DStringGDL*>( errorState->GetTag( msgTag)))[0] = 
		e.getMessage();
		SysVar::SetErr_String( e.getMessage());
		
		BaseGDL** catchVar = callStack.back()->GetCatchVar();
		GDLDelete(*catchVar);
		*catchVar = new DLongGDL( e.ErrorCode());
		_retTree = catchNode;
		return RC_OK;
		}
		
		EnvUDT* targetEnv = e.GetTargetEnv();
		if( targetEnv == NULL)
		{
		// initial exception, set target env
		
		// set !ERROR_STATE here
		DStructGDL* errorState = SysVar::Error_State();
		(*static_cast<DStringGDL*>( errorState->GetTag( nameTag)))[0] = 
		"IDL_M_FAILURE";
		(*static_cast<DLongGDL*>( errorState->GetTag( codeTag)))[0] = 
		e.ErrorCode();
		SysVar::SetErrError( e.ErrorCode());
		(*static_cast<DStringGDL*>( errorState->GetTag( msgTag)))[0] = 
		e.getMessage();
		SysVar::SetErr_String( e.getMessage());
		
		// look if ON_ERROR is set somewhere
		// for( EnvStackT::reverse_iterator i = callStack.rbegin();
		//     i != callStack.rend(); ++i)
		for( long ix = callStack.size() - 1; ix>=0; --ix)
		{
		EnvUDT** i = &callStack[ ix];
		DLong oE = -1;
		EnvUDT* envUD = dynamic_cast<EnvUDT*>(*i);
		if( envUD != NULL)
		oE = envUD->GetOnError();
		
		if( oE != -1) 
		{ // oE was set
		
		// 0 -> stop here
		if( oE == 0) 
		targetEnv = static_cast<EnvUDT*>(callStack.back()); 
		// 1 -> $MAIN$
		else if( oE == 1) 
		{
		EnvUDT* cS_begin = 
		static_cast<EnvUDT*>(callStack[0]);
		// static_cast<EnvUDT*>(*callStack.begin());
		targetEnv = cS_begin;  
		}
		// 2 -> caller of routine which called ON_ERROR
		else if( oE == 2)
		{
		// set to caller, handle nested
		while( ix > 0 && static_cast<EnvUDT*>(callStack[--ix])->GetOnError() == 2)
		; // just set ix
		
		EnvUDT* iUDT = static_cast<EnvUDT*>(callStack[ix]);
		targetEnv = iUDT;
		
		
		// while( static_cast<EnvUDT*>(*(++i))->GetOnError() == 2 
		//        && i != callStack.rend());
		// if( i == callStack.rend())
		// {
		//     EnvUDT* cS_begin = 
		//     static_cast<EnvUDT*>(*callStack.begin());
		//     targetEnv = cS_begin;
		// }
		// else
		// {
		//     EnvUDT* iUDT = static_cast<EnvUDT*>(*i);
		//     targetEnv = iUDT;
		// }
		}   
		// 3 -> routine which called ON_ERROR
		else if( oE == 3)
		{
		EnvUDT* iUDT = static_cast<EnvUDT*>(callStack[ix]);
		// EnvUDT* iUDT = static_cast<EnvUDT*>(*i);
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
		if( e.getLine() == 0 && last != NULL)
		e.SetLine( last->getLine());
		
		// tell where we are
		ReportError(e, "Execution halted at:", targetEnv == NULL); 
		
		retCode = NewInterpreterInstance(e.getLine());//-1);
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
	assert(returnValue == NULL);
	RetCode retCode;
	
		for (; _t != NULL;) {
	
				retCode=statement(_t);
	_t = _retTree;
				
	// 			if( retCode == RC_RETURN) 
				if( retCode >= RC_RETURN) 
				{
	res=returnValue;
	returnValue=NULL;
	// already done in RETFNode::Run() :
	// if( returnValueL != NULL)
	//     {
	//         callStack.back()->SetPtrToReturnValue( returnValueL);
	//         returnValueL = NULL;
	//     }
				break;
				}					
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
		}
		else {
			goto _loop7;
		}
		
	}
	_loop7:;
	} // ( ... )*
	_retTree = _t;
	return res;
}

 BaseGDL**  GDLInterpreter::call_lfun(ProgNodeP _t) {
	 BaseGDL** res;
	ProgNodeP call_lfun_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res = NULL;
	assert(returnValueL == NULL);
	RetCode retCode;
	
		ProgNodeP in = _t;
	
		for (; _t != NULL;) 
	{
	retCode=statement(_t);
	_t = _retTree;
				
	if( retCode >= RC_RETURN) 
				{
	res=returnValueL;
	returnValueL=NULL;
	break;
	}
	}
		
		// res must be defined here
		if( res == NULL)
	throw GDLException( in, "Function "+callStack.back()->GetProName()+
	" must return a global left-value in this context.",
	false,false);
		_retTree = _t;
		return res;
	
	
	{ // ( ... )*
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			retCode=statement(_t);
			_t = _retTree;
		}
		else {
			goto _loop10;
		}
		
	}
	_loop10:;
	} // ( ... )*
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
		}
		else {
			goto _loop13;
		}
		
	}
	_loop13:;
	} // ( ... )*
	_retTree = _t;
}

BaseGDL**  GDLInterpreter::l_deref(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_deref_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		ProgNodeP retTree = _t->getNextSibling();
	
	BaseGDL* e1;
	ProgNodeP evalExpr = _t->getFirstChild();
	if( NonCopyNode( evalExpr->getType()))
	{
	e1 = evalExpr->EvalNC();
	}
	else
	{
	BaseGDL** ref = evalExpr->EvalRefCheck(e1);
	if( ref == NULL)
	{
	// use new env if set (during parameter parsing)
	EnvBaseT* actEnv = DInterpreter::CallStackBack()->GetNewEnv();
	if( actEnv == NULL) actEnv = DInterpreter::CallStackBack();
	assert( actEnv != NULL);
	// this is crucial, a guard does not work here as a temporary
	// ptr will be cleaned up at return from this function
	actEnv->DeleteAtExit( e1);
	}
	else
	e1 = *ref;
	}
	
	if( e1 == NULL || e1->Type() != GDL_PTR)
	throw GDLException( evalExpr, "Pointer type required in this context: "+Name(e1),true,false);
	
	DPtrGDL* ptr=static_cast<DPtrGDL*>(e1);
	
	DPtr sc; 
	if( !ptr->Scalar(sc))
	throw GDLException( _t, "Expression must be a scalar in this context: "+Name(e1),true,false);
	if( sc == 0)
	throw GDLException( _t, "Unable to dereference NULL pointer: "+Name(e1),true,false);
	
	try
	{
	res = &GetHeap(sc);
	}
	catch( HeapException)
	{
	throw GDLException( _t, "Invalid pointer: "+Name(e1),true,false);
	}
	
		_retTree = retTree;
		return res;
	
	
	ProgNodeP tmp34_AST_in = _t;
	match(antlr::RefAST(_t),DEREF);
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_decinc_indexable_expr(ProgNodeP _t,
	 BaseGDL*& res
) {
	BaseGDL** e;
	ProgNodeP l_decinc_indexable_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	e = _t->LEval();
	res = *e;
	if( res == NULL)
	throw GDLException( _t, "Variable is undefined: "+Name(e),true,false);
	return e;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case FCALL:
	case FCALL_LIB:
	case MFCALL:
	case MFCALL_PARENT:
	{
		e=l_function_call_internal(_t);
		_t = _retTree;
		break;
	}
	case DEREF:
	{
		e=l_deref(_t);
		_t = _retTree;
		break;
	}
	case VAR:
	case VARPTR:
	{
		e=l_defined_simple_var(_t);
		_t = _retTree;
		break;
	}
	case SYSVAR:
	{
		e=l_sys_var(_t);
		_t = _retTree;
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return e;
}

 BaseGDL**  GDLInterpreter::l_function_call_internal(ProgNodeP _t) {
	 BaseGDL** res;
	ProgNodeP l_function_call_internal_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res = _t->LEval();
	_retTree = _t->getNextSibling();
	return res;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case FCALL_LIB:
	{
		ProgNodeP tmp35_AST_in = _t;
		match(antlr::RefAST(_t),FCALL_LIB);
		_t = _t->getNextSibling();
		break;
	}
	case MFCALL:
	{
		ProgNodeP tmp36_AST_in = _t;
		match(antlr::RefAST(_t),MFCALL);
		_t = _t->getNextSibling();
		break;
	}
	case MFCALL_PARENT:
	{
		ProgNodeP tmp37_AST_in = _t;
		match(antlr::RefAST(_t),MFCALL_PARENT);
		_t = _t->getNextSibling();
		break;
	}
	case FCALL:
	{
		ProgNodeP tmp38_AST_in = _t;
		match(antlr::RefAST(_t),FCALL);
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

BaseGDL**  GDLInterpreter::l_defined_simple_var(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_defined_simple_var_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		assert( _t != NULL);
	res = _t->LEval();
	_retTree = _t->getNextSibling();
		if( *res == NULL)
	{
	if( _t->getType() == VAR)
	throw GDLException( _t, "Variable is undefined: "+
	callStack.back()->GetString(_t->varIx),true,false);
	else
	throw GDLException( _t, "Common block variable is undefined: "+
	callStack.back()->GetString( *res),true,false);
	}
	return res;
	
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	{
		ProgNodeP tmp39_AST_in = _t;
		match(antlr::RefAST(_t),VAR);
		_t = _t->getNextSibling();
		break;
	}
	case VARPTR:
	{
		ProgNodeP tmp40_AST_in = _t;
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
	
		ProgNodeP sysVar = _t;
		//match(antlr::RefAST(_t),SYSVAR);
		res=sysVar->LEval();
		_retTree = sysVar->getNextSibling();
		return res;
	
	
	
	ProgNodeP tmp41_AST_in = _t;
	match(antlr::RefAST(_t),SYSVAR);
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_decinc_array_expr(ProgNodeP _t,
	int dec_inc, BaseGDL*& res
) {
	BaseGDL** e;
	ProgNodeP l_decinc_array_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		ArrayIndexListT* aL;
		ArrayIndexListGuard guard;
		
		if( _t->getType() == ARRAYEXPR)
		{
			ProgNodeP arrExNP = _t;
			// match(antlr::RefAST(_t),ARRAYEXPR);
	
			_t = _t->getFirstChild();
	
			BaseGDL** tmp =_t->LEval();// l_decinc_indexable_expr(_t, res);
	res = *tmp;
	if( res == NULL)
	throw GDLException( _t, "Variable is undefined: " + Name(tmp),
	true,false);
			_t = _t->getNextSibling();
	
			aL=arrayindex_list(_t,!res->IsAssoc());
	
			_retTree = arrExNP->getNextSibling();
			
			guard.reset( aL); 
			aL->SetVariable( res);
			
			if( dec_inc == DECSTATEMENT) 
			{
	res->DecAt( aL); 
	return NULL;
			}
			if( dec_inc == INCSTATEMENT)
			{
	res->IncAt( aL);
	return NULL;
			}
			
			if( dec_inc == DEC || dec_inc == DEC_REF_CHECK) 
			res->DecAt( aL); 
			else if( dec_inc == INC || dec_inc == INC_REF_CHECK) 
			res->IncAt( aL);
			
			BaseGDL* resBefore = res;
			res = resBefore->Index( aL);
			
			if( dec_inc == POSTDEC) resBefore->DecAt( aL);
			else if( dec_inc == POSTINC) resBefore->IncAt( aL);
			
	return NULL;
		}
	else
	{
			e =_t->LEval();// l_decinc_indexable_expr(_t, res);
	res = *e;
	if( res == NULL)
	throw GDLException( _t, "Variable is undefined: " + Name(e),
	true,false);
			_retTree = _t->getNextSibling();
			// e=l_decinc_indexable_expr(_t, res);
			// _t = _retTree;
			
			if( dec_inc == DECSTATEMENT || dec_inc == DEC_REF_CHECK) 
			{
	res->Dec(); 
	return e;
			}
			if( dec_inc == INCSTATEMENT || dec_inc == INC_REF_CHECK)
			{
	res->Inc();
	return e;
			}
			
			if( dec_inc == DEC) res->Dec();
			else if( dec_inc == INC) res->Inc();
	
			BaseGDL* resBefore = res;
			res = resBefore->Dup();
			
			if( dec_inc == POSTDEC) resBefore->Dec();
			else if( dec_inc == POSTINC) resBefore->Inc();
			
	return e;
		}
	assert(false);
		return e;
	
	// not used _______________________________________
	// ------------------------------------------------
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	{
		ProgNodeP tmp42_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getNextSibling();
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
		e=l_decinc_indexable_expr(_t, res);
		_t = _retTree;
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
	return e;
}

BaseGDL*  GDLInterpreter::l_decinc_dot_expr(ProgNodeP _t,
	int dec_inc
) {
	BaseGDL* res;
	ProgNodeP l_decinc_dot_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		ProgNodeP dot = _t;
		//match(antlr::RefAST(_t),DOT);
	
		_t = _t->getFirstChild();
		
		SizeT nDot=dot->nDot;
		Guard<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
		
		l_dot_array_expr(_t, aD.Get());
		_t = _retTree;
	
	while( _t != NULL)
	{
	tag_array_expr(_t, aD.Get());
				_t = _retTree;
			}
	
		_retTree = dot->getNextSibling();
		
		if( dec_inc == DECSTATEMENT) 
	{
	aD.Get()->Dec(); 
	return NULL;
	}   
		if( dec_inc == INCSTATEMENT)
	{
	aD.Get()->Inc();
	return NULL; 
	}
		
		if( dec_inc == DEC || dec_inc == DEC_REF_CHECK) 
	aD.Get()->Dec(); //*** aD->Assign( dec_inc);
		else if( dec_inc == INC || dec_inc == INC_REF_CHECK) 
	aD.Get()->Inc();
		
		res=aD.Get()->ADResolve();
		
		if( dec_inc == POSTDEC) aD.Get()->Dec();
		else if( dec_inc == POSTINC) aD.Get()->Inc();
	
		return res;
	
	
	ProgNodeP tmp43_AST_in = _t;
	match(antlr::RefAST(_t),DOT);
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_decinc_expr(ProgNodeP _t,
	int dec_inc, BaseGDL*& res
) {
	BaseGDL** refRet;
	ProgNodeP l_decinc_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP mp2 = ProgNodeP(antlr::nullAST);
	
	refRet = NULL;
	
	BaseGDL*       e1;
	ProgNodeP startNode = _t;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case QUESTION:
	{
		ProgNodeP __t24 = _t;
		ProgNodeP tmp44_AST_in = _t;
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getFirstChild();
		e1=expr(_t);
		_t = _retTree;
		
		Guard<BaseGDL> e1_guard(e1);
		
		if( e1->True())
		{
		refRet=l_decinc_expr(_t, dec_inc, res);
		}
		else
		{
		_t=_t->GetNextSibling(); // jump over 1st expression
		refRet=l_decinc_expr(_t, dec_inc, res);
		}
		
		_t = __t24;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN:
	{
		ProgNodeP __t25 = _t;
		ProgNodeP tmp45_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN);
		_t = _t->getFirstChild();
		
		Guard<BaseGDL> r_guard;
		
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
		case ARRAYDEF_GENERALIZED_INDGEN:
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
			r_guard.Init( e1);
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call_internal(_t);
			_t = _retTree;
			
			// if( !callStack.back()->Contains( e1)) 
			if( callStack.back()->GetPtrToReturnValueNull() == NULL) 
			r_guard.Init( e1); // guard if no global data
			
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
		
		tmp=l_expr_internal(_t, e1);
		_t = _retTree;
		
		_t = l;
		
		refRet=l_decinc_expr(_t,dec_inc, res);
		_t = _retTree;
		_t = __t25;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_ARRAYEXPR_MFCALL:
	{
		ProgNodeP __t27 = _t;
		ProgNodeP tmp46_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_ARRAYEXPR_MFCALL);
		_t = _t->getFirstChild();
		
		Guard<BaseGDL> r_guard;
		
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
		case ARRAYDEF_GENERALIZED_INDGEN:
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
			r_guard.Init( e1);
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call_internal(_t);
			_t = _retTree;
			
			// if( !callStack.back()->Contains( e1)) 
			if( callStack.back()->GetPtrToReturnValueNull() == NULL) 
			r_guard.Init( e1); // guard if no global data
			
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
		
		if( r_guard.Get() == e1)
		*tmp = r_guard.release();
		else          
		*tmp = e1->Dup();
		}
		
		refRet=l_decinc_expr( l, dec_inc, res);
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
		
		refRet=l_decinc_expr( l, dec_inc, res);
		}
		
		_t = __t27;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_REPLACE:
	{
		ProgNodeP __t29 = _t;
		ProgNodeP tmp47_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
		_t = _t->getFirstChild();
		
		Guard<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ASSIGN_ARRAYEXPR_MFCALL:
		case ARRAYDEF:
		case ARRAYDEF_GENERALIZED_INDGEN:
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
			
			r_guard.Init( e1);
			
			break;
		}
		case FCALL_LIB:
		{
			e1=lib_function_call_internal(_t);
			_t = _retTree;
			
			// if( !callStack.back()->Contains( e1)) 
			if( callStack.back()->GetPtrToReturnValueNull() == NULL) 
			r_guard.Init( e1);
			
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
			tmp=l_function_call_internal(_t);
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
		
		if( r_guard.Get() == e1)
		*tmp = r_guard.release();
		else  
		*tmp = e1->Dup();
		}
		
		
		_t = l;
		
		refRet=l_decinc_expr(_t,dec_inc, res);
		_t = _retTree;
		_t = __t29;
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
		refRet=l_decinc_array_expr(_t,dec_inc, res);
		_t = _retTree;
		break;
	}
	case ARRAYEXPR_MFCALL:
	{
		ProgNodeP __t32 = _t;
		ProgNodeP tmp48_AST_in = _t;
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
		
		Guard<BaseGDL> self_guard(self);
		
		EnvUDT* newEnv;
		
		DObjGDL* selfObj = NULL;
		if( self->Type() == GDL_OBJ)
		selfObj = static_cast<DObjGDL*>( self);
		try {
		newEnv=new EnvUDT( selfObj, mp2, "", EnvUDT::LFUNCTION);
		self_guard.release();
		}
		catch( GDLException& ex)
		{
		_t = mark;
		
		res=l_decinc_dot_expr(_t, dec_inc);
		
		_retTree = startNode->getNextSibling();
		return refRet;
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
		
		if( dec_inc == DECSTATEMENT || dec_inc == DEC_REF_CHECK) 
		{
		e->Dec(); 
		_retTree = startNode->getNextSibling();
		return ee;
		}
		if( dec_inc == INCSTATEMENT || dec_inc == INC_REF_CHECK)
		{
		e->Inc();
		_retTree = startNode->getNextSibling();
		return ee;
		}
		
		if( dec_inc == DEC) e->Dec();
		else if( dec_inc == INC) e->Inc();
		//          
		res = e->Dup();
		
		if( dec_inc == POSTDEC) e->Dec();
		else if( dec_inc == POSTINC) e->Inc();
		
		_retTree = startNode->getNextSibling();
		return ee;
		
		_t = __t32;
		_t = _t->getNextSibling();
		break;
	}
	case DOT:
	{
		res=l_decinc_dot_expr(_t,dec_inc);
		_t = _retTree;
		break;
	}
	case ARRAYDEF:
	case ARRAYDEF_GENERALIZED_INDGEN:
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
		ProgNodeP tmp49_AST_in = _t;
		match(antlr::RefAST(_t),CONSTANT);
		_t = _t->getNextSibling();
		
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
	return refRet;
}

BaseGDL*  GDLInterpreter::expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		assert( _t != NULL);
	
	res = _t->Eval();
	_retTree = _t->getNextSibling();
	return res; //tmp_expr(_t);
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	case ASSIGN_REPLACE:
	case ASSIGN_ARRAYEXPR_MFCALL:
	case ARRAYDEF:
	case ARRAYDEF_GENERALIZED_INDGEN:
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
		res=lib_function_call_internal(_t);
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

BaseGDL*  GDLInterpreter::indexable_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP indexable_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res = _t->EvalNC();
	_retTree = _t->getNextSibling();
	return res;
	
	BaseGDL** e2;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	case VARPTR:
	{
		e2=l_defined_simple_var(_t);
		_t = _retTree;
		break;
	}
	case SYSVAR:
	{
		ProgNodeP tmp50_AST_in = _t;
		match(antlr::RefAST(_t),SYSVAR);
		_t = _t->getNextSibling();
		break;
	}
	case CONSTANT:
	{
		ProgNodeP tmp51_AST_in = _t;
		match(antlr::RefAST(_t),CONSTANT);
		_t = _t->getNextSibling();
		break;
	}
	case DEREF:
	{
		e2=l_deref(_t);
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

BaseGDL*  GDLInterpreter::indexable_tmp_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP indexable_tmp_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res = _t->Eval(); //lib_function_call_retnew_internal(_t);
		_retTree = _t->getNextSibling();
	return res;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case QUESTION:
	{
		{
		ProgNodeP tmp52_AST_in = _t;
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getNextSibling();
		}
		break;
	}
	case ARRAYEXPR:
	{
		{
		ProgNodeP tmp53_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getNextSibling();
		}
		break;
	}
	case DOT:
	{
		ProgNodeP tmp54_AST_in = _t;
		match(antlr::RefAST(_t),DOT);
		_t = _t->getNextSibling();
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
		res=unused_function_call(_t);
		_t = _retTree;
		break;
	}
	case ARRAYDEF:
	case ARRAYDEF_GENERALIZED_INDGEN:
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
		res=lib_function_call_retnew_internal(_t);
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

 BaseGDL*  GDLInterpreter::lib_function_call_internal(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP lib_function_call_internal_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	assert( _t->getType() == FCALL_LIB);
		BaseGDL** retValPtr;
		res = static_cast<FCALL_LIBNode*>(_t)->EvalFCALL_LIB(retValPtr); 
		_retTree = _t->getNextSibling();
	
	callStack.back()->SetPtrToReturnValue( retValPtr); 
	return res;
	
	
	ProgNodeP tmp55_AST_in = _t;
	match(antlr::RefAST(_t),FCALL_LIB);
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_expr_internal(ProgNodeP _t,
	BaseGDL* right
) {
	BaseGDL** res;
	ProgNodeP l_expr_internal_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res = _t->LExpr( right);
	_retTree = _t->getNextSibling();
	return res;
	
	BaseGDL* e1; 
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case QUESTION:
	{
		ProgNodeP tmp56_AST_in = _t;
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR:
	{
		res=unused_l_array_expr(_t, right);
		_t = _retTree;
		break;
	}
	case SYSVAR:
	{
		res=l_sys_var(_t);
		_t = _retTree;
		break;
	}
	case FCALL:
	case FCALL_LIB:
	case MFCALL:
	case MFCALL_PARENT:
	{
		res=l_function_call_internal(_t);
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
	case ARRAYEXPR_MFCALL:
	{
		res=l_arrayexpr_mfcall(_t, right);
		_t = _retTree;
		break;
	}
	case DOT:
	{
		ProgNodeP tmp57_AST_in = _t;
		match(antlr::RefAST(_t),DOT);
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN:
	{
		ProgNodeP tmp58_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN);
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_ARRAYEXPR_MFCALL:
	{
		ProgNodeP tmp59_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_ARRAYEXPR_MFCALL);
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_REPLACE:
	{
		ProgNodeP tmp60_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYDEF:
	case ARRAYDEF_GENERALIZED_INDGEN:
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
		break;
	}
	case CONSTANT:
	{
		ProgNodeP tmp61_AST_in = _t;
		match(antlr::RefAST(_t),CONSTANT);
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

BaseGDL*  GDLInterpreter::tmp_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP tmp_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		res = _t->Eval();
		_retTree = _t->getNextSibling();
	return res;
	
	BaseGDL** e2;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case DEREF:
	{
		e2=l_deref(_t);
		_t = _retTree;
		break;
	}
	case QUESTION:
	{
		{
		ProgNodeP tmp62_AST_in = _t;
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getNextSibling();
		}
		break;
	}
	case ARRAYEXPR:
	{
		{
		ProgNodeP tmp63_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getNextSibling();
		}
		break;
	}
	case DOT:
	{
		ProgNodeP tmp64_AST_in = _t;
		match(antlr::RefAST(_t),DOT);
		_t = _t->getNextSibling();
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
		res=unused_function_call(_t);
		_t = _retTree;
		break;
	}
	case ARRAYDEF:
	case ARRAYDEF_GENERALIZED_INDGEN:
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
		ProgNodeP tmp65_AST_in = _t;
		match(antlr::RefAST(_t),CONSTANT);
		_t = _t->getNextSibling();
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
		res=lib_function_call_retnew_internal(_t);
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

BaseGDL**  GDLInterpreter::l_simple_var(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_simple_var_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		assert( _t != NULL);
	res = _t->LEval();    
	_retTree = _t->getNextSibling();
	return res;
	
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	{
		ProgNodeP tmp66_AST_in = _t;
		match(antlr::RefAST(_t),VAR);
		_t = _t->getNextSibling();
		break;
	}
	case VARPTR:
	{
		ProgNodeP tmp67_AST_in = _t;
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
	
	// as actEnv is not on the stack guard it here
	Guard<EnvBaseT> guard(actEnv); 
	
	EnvBaseT* callerEnv = callStack.back();
	EnvBaseT* oldNewEnv = callerEnv->GetNewEnv();
	
	callerEnv->SetNewEnv( actEnv);
	
	try{
	
	_retTree = _t;
	if( _retTree != NULL)
	{
	int nPar = _retTree->GetNParam();
	int nSub = actEnv->GetPro()->NPar();
	// // variable number of parameters
	// if( nSub == -1)
	//     {
	//         // _retTree != NULL, save one check
	//         static_cast<ParameterNode*>(_retTree)->Parameter( actEnv);
	//         while(_retTree != NULL) 
	//             static_cast<ParameterNode*>(_retTree)->Parameter( actEnv);
	//     }
	// // fixed number of parameters
	if( nSub != -1 && nPar > nSub) // check here
	{
	throw GDLException( _t, actEnv->GetProName() +
	": Incorrect number of arguments.",
	false, false);
	}
	else
	{
	// _retTree != NULL, save one check
	static_cast<ParameterNode*>(_retTree)->Parameter( actEnv);
	// Parameter does no checking
	while(_retTree != NULL) 
	static_cast<ParameterNode*>(_retTree)->Parameter( actEnv);
	}    
	actEnv->ResolveExtra(); // expand _EXTRA        
	}
	} 
	catch( GDLException& e)
	{
	callerEnv->SetNewEnv( oldNewEnv);
	// update line number, currently set to caller->CallingNode()
	// because actEnv is not on the stack yet, 
	// report caller->Pro()'s name is ok, because we are not inside
	// the call yet
	e.SetErrorNodeP( actEnv->CallingNode());
	throw e;
	}
	callerEnv->SetNewEnv( oldNewEnv);
	
		guard.release();
		
	return;
	
	
	ProgNodeP tmp68_AST_in = _t;
	match(antlr::RefAST(_t),KEYDEF_REF);
	_t = _t->getNextSibling();
	ProgNodeP tmp69_AST_in = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	_retTree = _t;
}

BaseGDL*  GDLInterpreter::r_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP r_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res=_t->Eval();
		_retTree = _t->getNextSibling();
		return res;
	
	BaseGDL** refRet; // not used
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case EXPR:
	{
		ProgNodeP tmp70_AST_in = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYDEF:
	{
		ProgNodeP tmp71_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYDEF);
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYDEF_GENERALIZED_INDGEN:
	{
		ProgNodeP tmp72_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYDEF_GENERALIZED_INDGEN);
		_t = _t->getNextSibling();
		break;
	}
	case STRUC:
	{
		ProgNodeP tmp73_AST_in = _t;
		match(antlr::RefAST(_t),STRUC);
		_t = _t->getNextSibling();
		break;
	}
	case NSTRUC:
	{
		ProgNodeP tmp74_AST_in = _t;
		match(antlr::RefAST(_t),NSTRUC);
		_t = _t->getNextSibling();
		break;
	}
	case NSTRUC_REF:
	{
		ProgNodeP tmp75_AST_in = _t;
		match(antlr::RefAST(_t),NSTRUC_REF);
		_t = _t->getNextSibling();
		break;
	}
	case DEC:
	{
		ProgNodeP __t43 = _t;
		ProgNodeP tmp76_AST_in = _t;
		match(antlr::RefAST(_t),DEC);
		_t = _t->getFirstChild();
		refRet=l_decinc_expr(_t, DEC, res);
		_t = _retTree;
		_t = __t43;
		_t = _t->getNextSibling();
		break;
	}
	case INC:
	{
		ProgNodeP __t44 = _t;
		ProgNodeP tmp77_AST_in = _t;
		match(antlr::RefAST(_t),INC);
		_t = _t->getFirstChild();
		refRet=l_decinc_expr(_t, INC, res);
		_t = _retTree;
		_t = __t44;
		_t = _t->getNextSibling();
		break;
	}
	case POSTDEC:
	{
		ProgNodeP __t45 = _t;
		ProgNodeP tmp78_AST_in = _t;
		match(antlr::RefAST(_t),POSTDEC);
		_t = _t->getFirstChild();
		refRet=l_decinc_expr(_t, POSTDEC, res);
		_t = _retTree;
		_t = __t45;
		_t = _t->getNextSibling();
		break;
	}
	case POSTINC:
	{
		ProgNodeP __t46 = _t;
		ProgNodeP tmp79_AST_in = _t;
		match(antlr::RefAST(_t),POSTINC);
		_t = _t->getFirstChild();
		refRet=l_decinc_expr(_t, POSTINC, res);
		_t = _retTree;
		_t = __t46;
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

BaseGDL**  GDLInterpreter::l_indexable_expr(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_indexable_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res = _t->LEval();
	if( *res == NULL) 
	{
	// check not needed for SYSVAR 
	assert( _t->getType() != SYSVAR);
	if( _t->getType() == VARPTR)
	throw GDLException( _t, "Common block variable is undefined: "+
	callStack.back()->GetString( *res),true,false);
	if( _t->getType() == VAR)
	throw GDLException( _t, "Variable is undefined: "+
	callStack.back()->GetString(_t->varIx),true,false);
	throw GDLException( _t, "Heap variable is undefined: "+Name(res),true,false);
	}
	_retTree = _t->getNextSibling();
		return res;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case EXPR:
	{
		ProgNodeP tmp80_AST_in = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getNextSibling();
		break;
	}
	case FCALL:
	case FCALL_LIB:
	case MFCALL:
	case MFCALL_PARENT:
	{
		res=l_function_call_internal(_t);
		_t = _retTree;
		break;
	}
	case ARRAYEXPR_MFCALL:
	{
		res=l_arrayexpr_mfcall_as_mfcall(_t);
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

 BaseGDL**  GDLInterpreter::l_arrayexpr_mfcall_as_mfcall(ProgNodeP _t) {
	 BaseGDL** res;
	ProgNodeP l_arrayexpr_mfcall_as_mfcall_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP mp2 = ProgNodeP(antlr::nullAST);
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	BaseGDL *self;
	EnvUDT*   newEnv;
	
	
	ProgNodeP __t79 = _t;
	ProgNodeP tmp81_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYEXPR_MFCALL);
	_t = _t->getFirstChild();
	
	_t = _t->getNextSibling(); // skip DOT
	
	self=expr(_t);
	_t = _retTree;
	mp2 = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	
	Guard<BaseGDL> self_guard(self);
	
	newEnv=new EnvUDT( self, mp2, "", EnvUDT::LFUNCTION);
	
	self_guard.release();
	
	parameter_def(_t, newEnv);
	_t = _retTree;
	_t = __t79;
	_t = _t->getNextSibling();
	
	// push environment onto call stack
	callStack.push_back(newEnv);
	
	// make the call
	res=call_lfun(static_cast<DSubUD*>(
	newEnv->GetPro())->GetTree());
	
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::unused_l_array_expr(ProgNodeP _t,
	BaseGDL* right
) {
	BaseGDL** res;
	ProgNodeP unused_l_array_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	ArrayIndexListT* aL;
	
	
	ProgNodeP __t35 = _t;
	ProgNodeP tmp82_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYEXPR);
	_t = _t->getFirstChild();
	res=l_indexable_expr(_t);
	_t = _retTree;
	aL=arrayindex_list(_t, false);
	_t = _retTree;
	_t = __t35;
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

ArrayIndexListT*  GDLInterpreter::arrayindex_list(ProgNodeP _t,
	 bool noAssoc
) {
	ArrayIndexListT* aL;
	ProgNodeP arrayindex_list_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	// IxExprListT      cleanupList; // for cleanup
	IxExprListT      ixExprList;
	SizeT nExpr;
	BaseGDL* s;
		
	//	ProgNodeP retTree = _t->getNextSibling();
		ProgNodeP ax = _t;
	// 	match(antlr::RefAST(_t),ARRAYIX);
		_t = _t->getFirstChild();
		
	if( noAssoc)
	aL = ax->arrIxListNoAssoc;
	else
	aL = ax->arrIxList; 
		assert( aL != NULL);
	
		nExpr = aL->NParam();
		if( nExpr == 0)
		{
	aL->Init();
	_retTree = ax->getNextSibling();//retTree;
	return aL;
		}
		
	IxExprListT* cleanupList = aL->GetCleanupIx(); // for cleanup
		while( true) {
	
	assert( _t != NULL);
	if( NonCopyNode( _t->getType()))
	{
	s = _t->EvalNC(); 
	}
	else
	{
	BaseGDL** ref =_t->EvalRefCheck( s); 
	if( ref == NULL)
	cleanupList->push_back( s);
	else
	s = *ref;
	}
				
	assert( s != NULL);
	ixExprList.push_back( s);
	if( ixExprList.size() == nExpr)
	break; // allows some manual tuning
	
	_t = _t->getNextSibling();
		}
	
		aL->Init( ixExprList);//, &cleanupList);
		
		_retTree = ax->getNextSibling();//retTree;
		return aL;
	
	
	ProgNodeP tmp83_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYIX);
	_t = _t->getNextSibling();
	_retTree = _t;
	return aL;
}

void GDLInterpreter::l_dot_array_expr(ProgNodeP _t,
	DotAccessDescT* aD
) {
	ProgNodeP l_dot_array_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	ArrayIndexListT* aL;
	BaseGDL**        rP;
		
		if( _t->getType() == ARRAYEXPR)
		{
			rP=l_indexable_expr(_t->getFirstChild());
	
			aL=arrayindex_list(_retTree,!(*rP)->IsAssoc());
	
			_retTree = _t->getNextSibling();
	
	SetRootL( _t, aD, *rP, aL); 
		}
	else
		// case ARRAYEXPR_MFCALL:
		// case DEREF:
		// case EXPR:
		// case FCALL:
		// case FCALL_LIB:
		// case MFCALL:
		// case MFCALL_PARENT:
		// case SYSVAR:
		// case VAR:
		// case VARPTR:
		{
			rP=l_indexable_expr(_t);
	
	SetRootL( _t, aD, *rP, NULL); 
		}
	return;
	//	_retTree = _t;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	{
		ProgNodeP __t37 = _t;
		ProgNodeP tmp84_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		rP=l_indexable_expr(_t);
		_t = _retTree;
		aL=arrayindex_list(_t, false);
		_t = _retTree;
		_t = __t37;
		_t = _t->getNextSibling();
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
		break;
	}
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
	_retTree = _t;
}

BaseGDL**  GDLInterpreter::l_arrayexpr_mfcall(ProgNodeP _t,
	BaseGDL* right
) {
	BaseGDL** res;
	ProgNodeP l_arrayexpr_mfcall_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP mp2 = ProgNodeP(antlr::nullAST);
	ProgNodeP dot = ProgNodeP(antlr::nullAST);
	
	// better than Guard: Guard wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	BaseGDL *self;
	EnvUDT*  newEnv;
	ProgNodeP startNode = _t;
	
	
	ProgNodeP __t69 = _t;
	ProgNodeP tmp85_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYEXPR_MFCALL);
	_t = _t->getFirstChild();
	
	ProgNodeP mark = _t;
	_t = _t->getNextSibling(); // skip DOT
	
	self=expr(_t);
	_t = _retTree;
	mp2 = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	
	Guard<BaseGDL> self_guard(self);
	
	try {
	newEnv=new EnvUDT( self, mp2, "", EnvUDT::LFUNCTION);
	self_guard.release();
	}
	catch( GDLException& ex)
	{
	goto tryARRAYEXPR;
	}
	
	parameter_def(_t, newEnv);
	_t = _retTree;
	_t = __t69;
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
	
	ProgNodeP __t70 = _t;
	dot = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),DOT);
	_t = _t->getFirstChild();
	
	SizeT nDot=dot->nDot;
	Guard<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
	
	l_dot_array_expr(_t, aD.Get());
	_t = _retTree;
	{ // ( ... )+
	int _cnt72=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR || _t->getType() == IDENTIFIER)) {
			tag_array_expr(_t, aD.Get());
			_t = _retTree;
		}
		else {
			if ( _cnt72>=1 ) { goto _loop72; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt72++;
	}
	_loop72:;
	}  // ( ... )+
	_t = __t70;
	_t = _t->getNextSibling();
	
	if( right == NULL)
	throw GDLException( _t, 
	"Struct expression not allowed in this context.",
	true,false);
	
	aD.Get()->ADAssign( right);
	
	res=NULL;
	
	_retTree = startNode->getNextSibling();
	return res;
	
	_retTree = _t;
	return res;
}

void GDLInterpreter::tag_expr(ProgNodeP _t,
	DotAccessDescT* aD
) {
	ProgNodeP tag_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	BaseGDL* e;
		
		if( _t->getType() == EXPR)
		{
			ProgNodeP tIn = _t;
			_t = _t->getFirstChild();
			e=expr(_t);
			
			Guard<BaseGDL> e_guard(e);
			
			SizeT tagIx;
			int ret=e->Scalar2Index(tagIx);
			if( ret < 1) // this is a return code, not the index
	throw GDLException( tIn, "Expression must be a scalar"
	" >= 0 in this context: "+Name(e),true,false);
			aD->ADAdd( tagIx);
	
			_retTree = tIn->getNextSibling();
		}
	else
		// case IDENTIFIER:
		{
	assert( _t->getType() == IDENTIFIER);
			std::string tagName=_t->getText();
	
			aD->ADAdd( tagName);
	
			_retTree = _t->getNextSibling();		
		}
	return;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case EXPR:
	{
		ProgNodeP tmp86_AST_in = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getNextSibling();
		break;
	}
	case IDENTIFIER:
	{
		ProgNodeP tmp87_AST_in = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
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

void GDLInterpreter::tag_array_expr(ProgNodeP _t,
	DotAccessDescT* aD
) {
	ProgNodeP tag_array_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	ArrayIndexListT* aL;
		
		if( _t->getType() == ARRAYEXPR)
		{
			ProgNodeP tIn = _t;
			_t = _t->getFirstChild();
			tag_expr(_t, aD);
			_t = _retTree;
	// noAssoc==true, as assoc vars cannot be struct members
			aL=arrayindex_list(_t,true); 
			//_t = _retTree;
			aD->ADAddIx(aL);
			_retTree = tIn->getNextSibling();
		}
	else
		// case EXPR:
		// case IDENTIFIER:
		{
			tag_expr(_t, aD);
			//_t = _retTree;
			aD->ADAddIx(NULL);
		}
		//_retTree = _t;
	return;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	{
		ProgNodeP __t49 = _t;
		ProgNodeP tmp88_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		tag_expr(_t, aD);
		_t = _retTree;
		aL=arrayindex_list(_t, true);
		_t = _retTree;
		_t = __t49;
		_t = _t->getNextSibling();
		break;
	}
	case EXPR:
	case IDENTIFIER:
	{
		tag_expr(_t, aD);
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

BaseGDL*  GDLInterpreter::r_dot_indexable_expr(ProgNodeP _t,
	DotAccessDescT* aD
) {
	BaseGDL* res;
	ProgNodeP r_dot_indexable_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res=NULL;
		switch ( _t->getType()) {
		case EXPR:
		{
			ProgNodeP tIn = _t;
			_t = _t->getFirstChild();
			//res=expr(_t);
	res = _t->Eval();
			aD->SetOwner( true);
			_retTree = tIn->getNextSibling();
			break;
		}
		case VAR:
		case VARPTR:
		{
			BaseGDL** v=l_defined_simple_var(_t);
			//_t = _retTree;
			res = *v;
			break;
		}
		case SYSVAR:
		{
	res = _t->EvalNC();
	_retTree = _t->getNextSibling();
			//res=sys_var_nocopy(_t);
			//_t = _retTree;
			break;
		}
		}
		//_retTree = _t;
		return res;
	
	BaseGDL** e;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case EXPR:
	{
		ProgNodeP __t51 = _t;
		ProgNodeP tmp89_AST_in = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getFirstChild();
		res=expr(_t);
		_t = _retTree;
		aD->SetOwner( true);
		_t = __t51;
		_t = _t->getNextSibling();
		break;
	}
	case VAR:
	case VARPTR:
	{
		e=l_defined_simple_var(_t);
		_t = _retTree;
		break;
	}
	case SYSVAR:
	{
		ProgNodeP tmp90_AST_in = _t;
		match(antlr::RefAST(_t),SYSVAR);
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

void GDLInterpreter::r_dot_array_expr(ProgNodeP _t,
	DotAccessDescT* aD
) {
	ProgNodeP r_dot_array_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	BaseGDL*         r;
	ArrayIndexListT* aL;
		
		if( _t->getType() == ARRAYEXPR)
		{
			//match(antlr::RefAST(_t),ARRAYEXPR);
			r=r_dot_indexable_expr(_t->getFirstChild(), aD);
	
			aL=arrayindex_list(_retTree,!r->IsAssoc());
			_retTree = _t->getNextSibling();
			
			// check here for object and get struct
			SetRootR( _t, aD, r, aL); 
		}
	else
		{
			r=r_dot_indexable_expr(_t, aD);
			//_t = _retTree;
			
			// check here for object and get struct
			SetRootR( _t, aD, r, NULL); 
		}
	return;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ARRAYEXPR:
	{
		ProgNodeP __t53 = _t;
		ProgNodeP tmp91_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		r=r_dot_indexable_expr(_t, aD);
		_t = _retTree;
		aL=arrayindex_list(_t, false);
		_t = _retTree;
		_t = __t53;
		_t = _t->getNextSibling();
		break;
	}
	case EXPR:
	case SYSVAR:
	case VAR:
	case VARPTR:
	{
		r=r_dot_indexable_expr(_t, aD);
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

BaseGDL*  GDLInterpreter::assign_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP assign_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res = _t->Eval();
		_retTree = _t->getNextSibling();
	return res;
	
	BaseGDL** l;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	{
		ProgNodeP tmp92_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN);
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_ARRAYEXPR_MFCALL:
	{
		ProgNodeP tmp93_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_ARRAYEXPR_MFCALL);
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_REPLACE:
	{
		ProgNodeP tmp94_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
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

 BaseGDL*  GDLInterpreter::unused_function_call(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP unused_function_call_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	res=NULL;
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case MFCALL:
	{
		ProgNodeP tmp95_AST_in = _t;
		match(antlr::RefAST(_t),MFCALL);
		_t = _t->getNextSibling();
		break;
	}
	case MFCALL_PARENT:
	{
		ProgNodeP tmp96_AST_in = _t;
		match(antlr::RefAST(_t),MFCALL_PARENT);
		_t = _t->getNextSibling();
		break;
	}
	case FCALL:
	{
		ProgNodeP tmp97_AST_in = _t;
		match(antlr::RefAST(_t),FCALL);
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR_MFCALL:
	{
		ProgNodeP tmp98_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR_MFCALL);
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

 BaseGDL*  GDLInterpreter::lib_function_call_retnew_internal(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP lib_function_call_retnew_internal_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res = _t->Eval();
		_retTree = _t->getNextSibling();
		return res; //_t->cData->Dup(); 
	
	
	ProgNodeP tmp99_AST_in = _t;
	match(antlr::RefAST(_t),FCALL_LIB_RETNEW);
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::simple_var(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP simple_var_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
		assert( _t != NULL);
	BaseGDL* vData = _t->EvalNC();
		if( vData == NULL)
	{
	if( _t->getType() == VAR)
	throw GDLException( _t, "Variable is undefined: "+_t->getText(),true,false);
	else // VARPTR
	throw GDLException( _t, "Common block variable is undefined.",true,false);
	}
		_retTree = _t->getNextSibling();
		return vData->Dup();
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	{
		ProgNodeP tmp100_AST_in = _t;
		match(antlr::RefAST(_t),VAR);
		_t = _t->getNextSibling();
		break;
	}
	case VARPTR:
	{
		ProgNodeP tmp101_AST_in = _t;
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

BaseGDL*  GDLInterpreter::sys_var(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP sys_var_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res = _t->Eval();
		_retTree = _t->getNextSibling();
		return res; // no ->Dup()
	
	
	ProgNodeP tmp102_AST_in = _t;
	match(antlr::RefAST(_t),SYSVAR);
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::l_arrayexpr_mfcall_as_arrayexpr(ProgNodeP _t,
	BaseGDL* right
) {
	BaseGDL** res;
	ProgNodeP l_arrayexpr_mfcall_as_arrayexpr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP dot = ProgNodeP(antlr::nullAST);
	
	ProgNodeP __t74 = _t;
	ProgNodeP tmp103_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYEXPR_MFCALL);
	_t = _t->getFirstChild();
	ProgNodeP __t75 = _t;
	dot = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),DOT);
	_t = _t->getFirstChild();
	
	SizeT nDot=dot->nDot;
	Guard<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
	
	l_dot_array_expr(_t, aD.Get());
	_t = _retTree;
	{ // ( ... )+
	int _cnt77=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR || _t->getType() == IDENTIFIER)) {
			tag_array_expr(_t, aD.Get());
			_t = _retTree;
		}
		else {
			if ( _cnt77>=1 ) { goto _loop77; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt77++;
	}
	_loop77:;
	}  // ( ... )+
	_t = __t75;
	_t = _t->getNextSibling();
	_t = __t74;
	_t = _t->getNextSibling();
	
	if( right == NULL)
	throw GDLException( _t, 
	"Struct expression not allowed in this context.",
	true,false);
	
	aD.Get()->ADAssign( right);
	
	res=NULL;
	
	_retTree = _t;
	return res;
}

void GDLInterpreter::parameter_def_n_elements(ProgNodeP _t,
	EnvBaseT* actEnv
) {
	ProgNodeP parameter_def_n_elements_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	Guard<EnvBaseT> guard(actEnv); 
	_retTree = _t;
	//     bool interruptEnableIn = interruptEnable;
	if( _retTree != NULL)
	{
	int nPar = _retTree->GetNParam();
	//int nSub = actEnv->GetPro()->NPar();
	assert(  actEnv->GetPro()->NPar() == 1); // N_ELEMENTS
	// fixed number of parameters
	if( nPar > 1)//nSub) // check here
	{
	throw GDLException( _t, actEnv->GetProName() +
	": Incorrect number of arguments.",
	false, false);
	}
	
	if( _retTree->getType() == REF ||
	_retTree->getType() == REF_EXPR ||
	_retTree->getType() == REF_CHECK ||
	_retTree->getType() == PARAEXPR)
	{
	try
	{
	//                     interruptEnable = false;
	static_cast<ParameterNode*>(_retTree)->Parameter( actEnv);
	//                     interruptEnable = interruptEnableIn;
	} 
	catch( GDLException& e)
	{
	// an error occured -> parameter is undefined 
	//                         interruptEnable = interruptEnableIn;
	if( actEnv->NParam() == 0) // not set yet
	{
	BaseGDL* nullP = NULL;
	actEnv->SetNextPar( nullP);
	}
	}
	}
	else // used for error handling: keywords are checked only here in Parameter()
	{
	try
	{
	// as N_ELEMENTS has no keywords this should throw always
	static_cast<ParameterNode*>(_retTree)->Parameter( actEnv);
	assert( 0);
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
	}
	// actEnv->Extra(); // expand _EXTRA
	} // if( _retTree != NULL)
	
		guard.release();
		
	return;
	
	
	ProgNodeP tmp104_AST_in = _t;
	match(antlr::RefAST(_t),KEYDEF_REF_EXPR);
	_t = _t->getNextSibling();
	ProgNodeP tmp105_AST_in = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	_retTree = _t;
}

void GDLInterpreter::parameter_def_nocheck(ProgNodeP _t,
	EnvBaseT* actEnv
) {
	ProgNodeP parameter_def_nocheck_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	Guard<EnvBaseT> guard(actEnv); 
	
	EnvBaseT* callerEnv = callStack.back();
	EnvBaseT* oldNewEnv = callerEnv->GetNewEnv();
	
	callerEnv->SetNewEnv( actEnv);
	
	try{
	
	if( _t != NULL)
	{
	_retTree = _t;
	// _retTree != NULL, save one check // 'if' is needed already for Extra()
	static_cast<ParameterNode*>(_retTree)->Parameter( actEnv);
	// Parameter does no checking
	while(_retTree != NULL) 
	static_cast<ParameterNode*>(_retTree)->Parameter( actEnv);
	
	actEnv->ResolveExtra(); // expand _EXTRA        
	}
	} 
	catch( GDLException& e)
	{
	callerEnv->SetNewEnv( oldNewEnv);
	// update line number, currently set to caller->CallingNode()
	// because actEnv is not on the stack yet, 
	// report caller->Pro()'s name is ok, because we are not inside
	// the call yet
	e.SetErrorNodeP( actEnv->CallingNode());
	throw e;
	}
	callerEnv->SetNewEnv( oldNewEnv);
	
		guard.release();
		
	return;
	
	
	ProgNodeP tmp106_AST_in = _t;
	match(antlr::RefAST(_t),KEYDEF_REF);
	_t = _t->getNextSibling();
	ProgNodeP tmp107_AST_in = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	_retTree = _t;
}

void GDLInterpreter::arrayindex_list_overload(ProgNodeP _t,
	IxExprListT& indexList
) {
	ProgNodeP arrayindex_list_overload_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	ArrayIndexListT* aL;
	// IxExprListT      cleanupList; // for cleanup
	IxExprListT      ixExprList;
	SizeT nExpr;
	BaseGDL* s;
		
	//	ProgNodeP retTree = _t->getNextSibling();
		ProgNodeP ax = _t;
	// 	match(antlr::RefAST(_t),ARRAYIX);
		_t = _t->getFirstChild();
		
		aL = ax->arrIxListNoAssoc;
		assert( aL != NULL);
	
		nExpr = aL->NParam();
		if( nExpr == 0)
		{
	aL->InitAsOverloadIndex( ixExprList, /* NULL,*/ indexList);
	_retTree = ax->getNextSibling();//retTree;
	return;
		}
		
	IxExprListT* cleanupList = aL->GetCleanupIx();
		
		while( true) {
	assert( _t != NULL);
	if( NonCopyNode( _t->getType()))
	{
	s= _t->EvalNCNull(); // in this case (overload) NULL is ok
	//_t = _retTree;
	}
	else
	{
	BaseGDL** ref=_t->EvalRefCheck( s); //indexable_tmp_expr(_t);
	//_t = _retTree;
	if( ref == NULL)
			  cleanupList->push_back( s);
			else
			  s = *ref;
	}
				
	ixExprList.push_back( s);
	if( ixExprList.size() == nExpr)
	break; // allows some manual tuning
	
	_t = _t->getNextSibling();
		}
	
	aL->InitAsOverloadIndex( ixExprList, /*&cleanupList,*/ indexList);
		
		_retTree = ax->getNextSibling();//retTree;
		return;
	
	
	
	ProgNodeP tmp108_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYIX);
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

const unsigned long GDLInterpreter::_tokenSet_0_data_[] = { 2170552736UL, 2151161983UL, 2285240323UL, 1208614912UL, 384UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// ASSIGN ASSIGN_REPLACE ASSIGN_ARRAYEXPR_MFCALL BLOCK BREAK CONTINUE "for" 
// FOR_STEP "foreach" FOREACH_INDEX FOR_LOOP FOR_STEP_LOOP FOREACH_LOOP 
// FOREACH_INDEX_LOOP IF_ELSE LABEL MPCALL MPCALL_PARENT ON_IOERROR_NULL 
// PCALL PCALL_LIB "repeat" REPEAT_LOOP RETF RETP "while" "case" "goto" 
// "if" "on_ioerror" "switch" DEC INC 
const antlr::BitSet GDLInterpreter::_tokenSet_0(_tokenSet_0_data_,12);


