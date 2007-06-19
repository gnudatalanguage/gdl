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

 GDLInterpreter::RetCode  GDLInterpreter::interactive(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP interactive_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	retCode=statement_list(_t);
	_t = _retTree;
	_retTree = _t;
	return retCode;
}

 GDLInterpreter::RetCode  GDLInterpreter::statement_list(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP statement_list_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	{ // ( ... )+
	int _cnt14=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			retCode=statement(_t);
			_t = _retTree;
			
			if( retCode != RC_OK) break; // break out if non-regular
			
		}
		else {
			if ( _cnt14>=1 ) { goto _loop14; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt14++;
	}
	_loop14:;
	}  // ( ... )+
	_retTree = _t;
	return retCode;
}

 GDLInterpreter::RetCode  GDLInterpreter::execute(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP execute_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	//    GDLInterpreter::RetCode retCode;
	ValueGuard<bool> guard( interruptEnable);
	interruptEnable = false;
	
	
	retCode=statement_list(_t);
	_t = _retTree;
	_retTree = _t;
	return retCode;
}

 BaseGDL*  GDLInterpreter::call_fun(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP call_fun_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	
	res = NULL;
	returnValue = NULL;
	GDLInterpreter::RetCode retCode;
	
	
	{ // ( ... )*
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			retCode=statement(_t);
			_t = _retTree;
			
			if( retCode == RC_RETURN) 
			{
			res=returnValue;
			returnValue=NULL;
			
			break;
			}
			
		}
		else {
			goto _loop5;
		}
		
	}
	_loop5:;
	} // ( ... )*
	
	// default return value if none was set
	if( res == NULL) res = new DIntGDL( 0); 
	
	_retTree = _t;
	return res;
}

 GDLInterpreter::RetCode  GDLInterpreter::statement(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP o = ProgNodeP(antlr::nullAST);
	
	retCode = RC_OK;
	ProgNodeP actPos = _t;
	
	
	try {      // for error handling
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		{
			assignment(_t);
			_t = _retTree;
			break;
		}
		case MPCALL:
		case MPCALL_PARENT:
		case PCALL:
		case PCALL_LIB:
		{
			procedure_call(_t);
			_t = _retTree;
			break;
		}
		case DEC:
		case INC:
		{
			decinc_statement(_t);
			_t = _retTree;
			break;
		}
		case FOR_STEP:
		case FOR:
		{
			retCode=for_statement(_t);
			_t = _retTree;
			break;
		}
		case REPEAT:
		{
			retCode=repeat_statement(_t);
			_t = _retTree;
			break;
		}
		case WHILE:
		{
			retCode=while_statement(_t);
			_t = _retTree;
			break;
		}
		case IF:
		{
			retCode=if_statement(_t);
			_t = _retTree;
			break;
		}
		case IF_ELSE:
		{
			retCode=if_else_statement(_t);
			_t = _retTree;
			break;
		}
		case CASE:
		{
			retCode=case_statement(_t);
			_t = _retTree;
			break;
		}
		case SWITCH:
		{
			retCode=switch_statement(_t);
			_t = _retTree;
			break;
		}
		case BLOCK:
		{
			retCode=block(_t);
			_t = _retTree;
			break;
		}
		case BREAK:
		case CONTINUE:
		case RETF:
		case RETP:
		case GOTO:
		{
			retCode=jump_statement(_t);
			_t = _retTree;
			break;
		}
		case LABEL:
		{
			ProgNodeP tmp1_AST_in = _t;
			match(antlr::RefAST(_t),LABEL);
			_t = _t->getNextSibling();
			break;
		}
		case ON_IOERROR_NULL:
		{
			ProgNodeP tmp2_AST_in = _t;
			match(antlr::RefAST(_t),ON_IOERROR_NULL);
			_t = _t->getNextSibling();
			
			static_cast<EnvUDT*>(callStack.back())->SetIOError( -1);
			
			break;
		}
		case ON_IOERROR:
		{
			o = _t;
			match(antlr::RefAST(_t),ON_IOERROR);
			_t = _t->getNextSibling();
			
			static_cast<EnvUDT*>(callStack.back())->
			SetIOError( o->targetIx);
			
			break;
		}
		default:
		{
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		}
		}
		
		// possible optimization: make sigControlC a debugMode 
		if( interruptEnable && sigControlC)
		{
		DebugMsg( actPos, "Interrupted at: "); 
		
		sigControlC = false;
		
		retCode = NewInterpreterInstance();
		}
		else if( debugMode != DEBUG_CLEAR)
		{
		if( debugMode == DEBUG_STOP)
		{
		DebugMsg( actPos, "Stop encoutered: ");
		if( !interruptEnable)
		debugMode = DEBUG_PROCESS_STOP;
		}
		
		if( interruptEnable)
		{
		if( debugMode == DEBUG_PROCESS_STOP)
		{
		DebugMsg( actPos, "Stepped to: ");
		}
		
		debugMode = DEBUG_CLEAR;
		
		retCode = NewInterpreterInstance();
		}   
		else
		{
		retCode = RC_ABORT;
		}
		}
		
	}
	catch ( GDLException& e) {
		
		if( dynamic_cast< GDLIOException*>( &e) != NULL)
		{
		// set the jump target - also logs the jump
		ProgNodeP onIOErr = static_cast<EnvUDT*>(callStack.back())->GetIOError();
		if( onIOErr != NULL)
		{
		_t = onIOErr;
		retCode=RC_OK;		
		
		_retTree = _t;
		return retCode;
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
		++i; // set to caller
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
		
		// remeber where to stop
		e.SetTargetEnv( targetEnv);
		
		// State where error occured
		if( e.getLine() == 0 && _t != NULL)
		e.SetLine( _t->getLine());
		
		ReportError(e, "Error occurred at:");
		
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
		if( e.getLine() == 0 && _t != NULL)
		{
		e.SetLine( _t->getLine());
		}
		
		if( interruptEnable)
		{
		// tell where we are
		ReportError(e, "Execution halted at:", targetEnv == NULL); 
		
		retCode = NewInterpreterInstance();
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
		
	}
	_retTree = _t;
	return retCode;
}

 BaseGDL**  GDLInterpreter::call_lfun(ProgNodeP _t) {
	 BaseGDL** res;
	ProgNodeP call_lfun_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res = NULL;
	returnValueL = NULL;
	GDLInterpreter::RetCode retCode;
	
	
	{ // ( ... )*
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_0.member(_t->getType()))) {
			retCode=statement(_t);
			_t = _retTree;
			
			if( retCode == RC_RETURN) 
			{
			res=returnValueL;
			returnValueL=NULL;
			break;
			}
			
		}
		else {
			goto _loop8;
		}
		
	}
	_loop8:;
	} // ( ... )*
	
	// default return value if none was set
	if( res == NULL)
	throw GDLException( call_lfun_AST_in, "Function "+
	callStack.back()->GetProName()+
	" must return a left-value in this context.");
	
	_retTree = _t;
	return res;
}

void GDLInterpreter::call_pro(ProgNodeP _t) {
	ProgNodeP call_pro_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	GDLInterpreter::RetCode retCode;
	
	
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
			goto _loop11;
		}
		
	}
	_loop11:;
	} // ( ... )*
	_retTree = _t;
}

void GDLInterpreter::assignment(ProgNodeP _t) {
	ProgNodeP assignment_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	BaseGDL*  r;
	BaseGDL** l;
	auto_ptr<BaseGDL> r_guard;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	{
		ProgNodeP __t46 = _t;
		ProgNodeP tmp3_AST_in = _t;
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
			r=indexable_expr(_t);
			_t = _retTree;
			break;
		}
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ARRAYDEF:
		case ARRAYEXPR:
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
			r=check_expr(_t);
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
		l=l_expr(_t, r);
		_t = _retTree;
		_t = __t46;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_REPLACE:
	{
		ProgNodeP __t48 = _t;
		ProgNodeP tmp4_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
		_t = _t->getFirstChild();
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ARRAYDEF:
		case ARRAYEXPR:
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
			r=tmp_expr(_t);
			_t = _retTree;
			
			r_guard.reset( r);
			
			break;
		}
		case FCALL_LIB:
		{
			r=check_expr(_t);
			_t = _retTree;
			
			
			if( !callStack.back()->Contains( r)) 
			r_guard.reset( r);
			
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
		
		if( r != (*l))
		{
		delete *l;
		
		if( r_guard.get() == r)
		*l = r_guard.release();
		else  
		*l = r->Dup();
		}
		
		_t = __t48;
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

void GDLInterpreter::procedure_call(ProgNodeP _t) {
	ProgNodeP procedure_call_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP pl = ProgNodeP(antlr::nullAST);
	ProgNodeP mp = ProgNodeP(antlr::nullAST);
	ProgNodeP parent = ProgNodeP(antlr::nullAST);
	ProgNodeP pp = ProgNodeP(antlr::nullAST);
	ProgNodeP p = ProgNodeP(antlr::nullAST);
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	BaseGDL *self;
	EnvUDT*   newEnv;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case PCALL_LIB:
	{
		ProgNodeP __t39 = _t;
		ProgNodeP tmp5_AST_in = _t;
		match(antlr::RefAST(_t),PCALL_LIB);
		_t = _t->getFirstChild();
		pl = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		EnvT* newEnv=new EnvT( pl, pl->libPro);//libProList[pl->proIx]);
		
		parameter_def(_t, newEnv);
		_t = _retTree;
		
		// push environment onto call stack
		callStack.push_back(newEnv);
		
		// make the call
		static_cast<DLibPro*>(newEnv->GetPro())->Pro()(newEnv);
		
		_t = __t39;
		_t = _t->getNextSibling();
		break;
	}
	case MPCALL:
	case MPCALL_PARENT:
	case PCALL:
	{
		{
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case MPCALL:
		{
			ProgNodeP __t42 = _t;
			ProgNodeP tmp6_AST_in = _t;
			match(antlr::RefAST(_t),MPCALL);
			_t = _t->getFirstChild();
			self=expr(_t);
			_t = _retTree;
			mp = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			
			auto_ptr<BaseGDL> self_guard(self);
			
			newEnv=new EnvUDT( mp, self);
			
			self_guard.release();
			
			parameter_def(_t, newEnv);
			_t = _retTree;
			_t = __t42;
			_t = _t->getNextSibling();
			break;
		}
		case MPCALL_PARENT:
		{
			ProgNodeP __t43 = _t;
			ProgNodeP tmp7_AST_in = _t;
			match(antlr::RefAST(_t),MPCALL_PARENT);
			_t = _t->getFirstChild();
			self=expr(_t);
			_t = _retTree;
			parent = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			pp = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			
			auto_ptr<BaseGDL> self_guard(self);
			
			newEnv = new EnvUDT( pp, self, parent->getText());
			
			self_guard.release();
			
			parameter_def(_t, newEnv);
			_t = _retTree;
			_t = __t43;
			_t = _t->getNextSibling();
			break;
		}
		case PCALL:
		{
			ProgNodeP __t44 = _t;
			ProgNodeP tmp8_AST_in = _t;
			match(antlr::RefAST(_t),PCALL);
			_t = _t->getFirstChild();
			p = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			
			SetProIx( p);
			
			newEnv = new EnvUDT( p, proList[p->proIx]);
			
			parameter_def(_t, newEnv);
			_t = _retTree;
			_t = __t44;
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
		call_pro(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
		
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

void GDLInterpreter::decinc_statement(ProgNodeP _t) {
	ProgNodeP decinc_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	BaseGDL* l;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case DEC:
	{
		ProgNodeP __t52 = _t;
		ProgNodeP tmp9_AST_in = _t;
		match(antlr::RefAST(_t),DEC);
		_t = _t->getFirstChild();
		l=l_decinc_expr(_t, DECSTATEMENT);
		_t = _retTree;
		_t = __t52;
		_t = _t->getNextSibling();
		break;
	}
	case INC:
	{
		ProgNodeP __t53 = _t;
		ProgNodeP tmp10_AST_in = _t;
		match(antlr::RefAST(_t),INC);
		_t = _t->getFirstChild();
		l=l_decinc_expr(_t, INCSTATEMENT);
		_t = _retTree;
		_t = __t53;
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

 GDLInterpreter::RetCode  GDLInterpreter::for_statement(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP for_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP f = ProgNodeP(antlr::nullAST);
	ProgNodeP fs = ProgNodeP(antlr::nullAST);
	
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
		ProgNodeP __t29 = _t;
		f = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),FOR);
		_t = _t->getFirstChild();
		
		ProgNodeP sv = _t;
		
		v=l_simple_var(_t);
		_t = _retTree;
		s=expr(_t);
		_t = _retTree;
		e=expr(_t);
		_t = _retTree;
		
		auto_ptr<BaseGDL> s_guard(s);
		auto_ptr<BaseGDL> e_guard(e);
		
		EnvUDT* callStack_back = 
		static_cast<EnvUDT*>(callStack.back());
		SizeT nJump = callStack_back->NJump();
		
		s->ForCheck( &e);
		e_guard.release();
		e_guard.reset(e);
		
		ProgNodeP b=_t; //->getFirstChild();
		
		// ASSIGNMENT used here also
		delete (*v);
		
		// problem:
		// EXECUTE may call DataListT.loc.resize(), as v points to the
		// old sequence v might be invalidated -> segfault
		// note that the value (*v) is preserved by resize()
		s_guard.release(); // s hold in *v after this
		for((*v)=s; (*v)->ForCondUp( e); 
		v=l_simple_var( sv), (*v)->ForAdd()) 
		{
		//                    retCode=block(b);
		if( b != NULL)
		{
		retCode=statement_list(b);
		
		if( retCode != RC_OK) // optimization
		{
		if( retCode == RC_CONTINUE) continue;  
		if( retCode == RC_BREAK) 
		{
		retCode = RC_OK;
		break;        
		}
		if( retCode >= RC_RETURN) break;
		}
		
		if( (callStack_back->NJump() != nJump) &&
		!f->LabelInRange( callStack_back->LastJump()))
		{
		// a jump (goto) occured out of this loop
		return retCode;
		}
		}
		}
		//                retCode=RC_OK; // clear RC_BREAK/RC_CONTINUE retCode
		
		_t = __t29;
		_t = _t->getNextSibling();
		break;
	}
	case FOR_STEP:
	{
		ProgNodeP __t30 = _t;
		fs = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),FOR_STEP);
		_t = _t->getFirstChild();
		
		ProgNodeP sv = _t;
		
		v=l_simple_var(_t);
		_t = _retTree;
		s=expr(_t);
		_t = _retTree;
		e=expr(_t);
		_t = _retTree;
		st=expr(_t);
		_t = _retTree;
		
		auto_ptr<BaseGDL> s_guard(s);
		auto_ptr<BaseGDL> e_guard(e);
		auto_ptr<BaseGDL> st_guard(st);
		
		SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
		
		s->ForCheck( &e, &st);
		e_guard.release();
		e_guard.reset(e);
		st_guard.release();
		st_guard.reset(st);
		
		ProgNodeP bs=_t;
		
		// ASSIGNMENT used here also
		delete (*v);
		
		if( st->Sgn() == -1) 
		{
		s_guard.release();
		for((*v)=s; (*v)->ForCondDown( e); 
		v=l_simple_var( sv), (*v)->ForAdd(st))
		{
		if( bs != NULL)
		{
		retCode=statement_list(bs);
		
		if( retCode == RC_CONTINUE) continue;  
		if( retCode == RC_BREAK) 
		{
		retCode = RC_OK;
		break;        
		}
		if( retCode >= RC_RETURN) break;
		
		if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
		!fs->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
		{
		// a jump (goto) occured out of this loop
		return retCode;
		}
		}
		}
		} 
		else
		{
		s_guard.release();
		for((*v)=s; (*v)->ForCondUp( e);
		v=l_simple_var( sv), (*v)->ForAdd(st))
		{
		if( bs != NULL)
		{
		retCode=statement_list(bs);
		
		if( retCode == RC_CONTINUE) continue;  
		if( retCode == RC_BREAK) 
		{
		retCode = RC_OK;
		break;        
		}
		if( retCode >= RC_RETURN) break;
		
		if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
		!fs->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
		{
		// a jump (goto) occured out of this loop
		return retCode;
		}
		}
		}
		}
		
		_t = __t30;
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

 GDLInterpreter::RetCode  GDLInterpreter::repeat_statement(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP repeat_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP r = ProgNodeP(antlr::nullAST);
	
	ProgNodeP __t25 = _t;
	r = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),REPEAT);
	_t = _t->getFirstChild();
	
	SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	// remember block and expr nodes
	ProgNodeP e =_t;
	ProgNodeP bb  = e->GetNextSibling();
	
	//                 ProgNodeP bb =_t;
	//                 ProgNodeP e  = bb->GetNextSibling();
	//                 bb = bb->GetFirstChild();
	
	auto_ptr<BaseGDL> eVal;
	do {
	if( bb != NULL)
	{
	retCode=statement_list(bb);
	
	if( retCode == RC_CONTINUE) continue;  
	if( retCode == RC_BREAK) 
	{
	retCode = RC_OK;
	break;        
	}
	if( retCode >= RC_RETURN) break;
	// if( retCode == RC_BREAK) break;        
	// if( retCode >= RC_RETURN) return retCode;
	
	if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	!r->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	{
	// a jump (goto) occured out of this loop
	return retCode;
	}
	}
	eVal.reset( expr(e));
	} while( eVal.get()->False());
	
	// retCode=RC_OK; // clear RC_BREAK/RC_CONTINUE retCode
	
	_t = __t25;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 GDLInterpreter::RetCode  GDLInterpreter::while_statement(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP while_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP w = ProgNodeP(antlr::nullAST);
	
	retCode = RC_OK;
	
	
	ProgNodeP __t27 = _t;
	w = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),WHILE);
	_t = _t->getFirstChild();
	
	SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	ProgNodeP s = _t; //->GetFirstChild();  // statement
	ProgNodeP e =  s->GetNextSibling(); // expr
	
	auto_ptr< BaseGDL> eVal( expr( e));
	while( eVal.get()->True()) {
	retCode=statement(s);
	
	if( retCode == RC_CONTINUE) continue;  
	if( retCode == RC_BREAK) 
	{
	retCode = RC_OK;
	break;        
	}
	if( retCode >= RC_RETURN) break;
	
	if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	!w->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	{
	// a jump (goto) occured out of this loop
	return retCode;
	}
	
	eVal.reset( expr( e));
	} 
	
	// retCode=RC_OK; // clear RC_BREAK/RC_CONTINUE retCode
	
	_t = __t27;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 GDLInterpreter::RetCode  GDLInterpreter::if_statement(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP if_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP i = ProgNodeP(antlr::nullAST);
	
	BaseGDL* e;
	retCode = RC_OK; // not set if not executed
	
	
	ProgNodeP __t32 = _t;
	i = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),IF);
	_t = _t->getFirstChild();
	e=expr(_t);
	_t = _retTree;
	
	auto_ptr<BaseGDL> e_guard(e);
	
	SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	if( e->True())
	{
	retCode=statement(_t);
	//                    if( retCode != RC_OK) return retCode;
	
	if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	!i->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	{
	// a jump (goto) occured out of this loop
	return retCode;
	}
	}
	
	_t = __t32;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 GDLInterpreter::RetCode  GDLInterpreter::if_else_statement(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP if_else_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP i = ProgNodeP(antlr::nullAST);
	
	BaseGDL* e;
	
	
	ProgNodeP __t34 = _t;
	i = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),IF_ELSE);
	_t = _t->getFirstChild();
	e=expr(_t);
	_t = _retTree;
	
	auto_ptr<BaseGDL> e_guard(e);
	
	SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	if( e->True())
	{
	retCode=statement(_t);
	//                    if( retCode != RC_OK) return retCode;
	
	if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	!i->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	{
	// a jump (goto) occured out of this loop
	return retCode;
	}
	}
	else
	{
	_t=_t->GetNextSibling(); // jump over 1st statement
	retCode=statement(_t);
	//                    if( retCode != RC_OK) return retCode;
	
	if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	!i->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	{
	// a jump (goto) occured out of this loop
	return retCode;
	}
	}
	
	_t = __t34;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 GDLInterpreter::RetCode  GDLInterpreter::case_statement(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP case_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP c = ProgNodeP(antlr::nullAST);
	
	BaseGDL* e;
	retCode = RC_OK; // not set if no branch is executed
	
	
	ProgNodeP __t23 = _t;
	c = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),CASE);
	_t = _t->getFirstChild();
	e=expr(_t);
	_t = _retTree;
	
	auto_ptr<BaseGDL> e_guard(e);
	
	SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	if( !e->Scalar())
	throw GDLException( _t, "Expression must be a"
	" scalar in this context: "+Name(e));
	
	ProgNodeP b=_t; // remeber block begin
	
	for( int i=0; i<c->numBranch; i++)
	{
	if( b->getType() == ELSEBLK)
	{
	ProgNodeP sL = b->GetFirstChild(); // statement_list
	
	if(sL != NULL )
	{
	// statement there
	retCode=statement_list(sL);
	//if( retCode == RC_BREAK) break; // break anyway
	//                            if( retCode >= RC_RETURN) return retCode; 
	if( retCode >= RC_RETURN) break;
	
	if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	!c->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	{
	// a jump (goto) occured out of this loop
	return retCode;
	}
	
	}
	retCode = RC_OK;
	break;
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
	if(bb != NULL)
	{
	// statement there
	retCode=statement_list(bb);
	//if( retCode == RC_BREAK) break; // break anyway
	//                                if( retCode >= RC_RETURN) return retCode;
	if( retCode >= RC_RETURN) break;
	
	if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	!c->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	{
	// a jump (goto) occured out of this loop
	return retCode;
	}
	
	}
	retCode = RC_OK;
	break;
	}
	
	}
	b=b->GetNextSibling(); // next block
	}
	// finish or break
	//                retCode=RC_OK; // clear RC_BREAK retCode
	
	_t = __t23;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 GDLInterpreter::RetCode  GDLInterpreter::switch_statement(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP switch_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP s = ProgNodeP(antlr::nullAST);
	
	BaseGDL* e;
	retCode = RC_OK; // not set if no branch is executed
	
	
	ProgNodeP __t21 = _t;
	s = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),SWITCH);
	_t = _t->getFirstChild();
	e=expr(_t);
	_t = _retTree;
	
	auto_ptr<BaseGDL> e_guard(e);
	
	SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
	
	ProgNodeP b=_t; // remeber block begin (block)
	
	bool hook=false; // switch executes everything after 1st match
	for( int i=0; i<s->numBranch; i++)
	{
	if( b->getType() == ELSEBLK)
	{
	hook=true;
	
	ProgNodeP sL = b->GetFirstChild(); // statement_list
	
	if( sL != NULL )
	{
	// statement there
	retCode=statement_list( sL);
	if( retCode == RC_BREAK) 
	{
	retCode = RC_OK;    
	break;          // break
	}
	if( retCode >= RC_RETURN) break; // goto
	
	if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	!s->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	{
	// a jump (goto) occured out of this loop
	return retCode;
	}
	
	}
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
	
	if(bb != NULL && hook)
	{
	// statement there
	retCode=statement_list(bb);
	if( retCode == RC_BREAK) 
	{
	retCode = RC_OK;    
	break;          // break
	}
	if( retCode >= RC_RETURN) break; // goto
	
	if( (static_cast<EnvUDT*>(callStack.back())->NJump() != nJump) &&
	!s->LabelInRange( static_cast<EnvUDT*>(callStack.back())->LastJump()))
	{
	// a jump (goto) occured out of this loop
	return retCode;
	}
	}
	
	}
	b=b->GetNextSibling(); // next block
	}
	// finish or break
	//                retCode=RC_OK; // clear RC_BREAK retCode
	
	_t = __t21;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 GDLInterpreter::RetCode  GDLInterpreter::block(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP block_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	retCode = RC_OK;
	
	
	ProgNodeP __t18 = _t;
	ProgNodeP tmp11_AST_in = _t;
	match(antlr::RefAST(_t),BLOCK);
	_t = _t->getFirstChild();
	{
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	case ASSIGN_REPLACE:
	case BLOCK:
	case BREAK:
	case CONTINUE:
	case FOR_STEP:
	case IF_ELSE:
	case LABEL:
	case MPCALL:
	case MPCALL_PARENT:
	case ON_IOERROR_NULL:
	case PCALL:
	case PCALL_LIB:
	case RETF:
	case RETP:
	case SWITCH:
	case CASE:
	case DEC:
	case INC:
	case REPEAT:
	case WHILE:
	case FOR:
	case GOTO:
	case ON_IOERROR:
	case IF:
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
	_t = __t18;
	_t = _t->getNextSibling();
	_retTree = _t;
	return retCode;
}

 GDLInterpreter::RetCode  GDLInterpreter::jump_statement(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	ProgNodeP jump_statement_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP g = ProgNodeP(antlr::nullAST);
	
	BaseGDL*  e;
	BaseGDL** eL;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case GOTO:
	{
		g = _t;
		match(antlr::RefAST(_t),GOTO);
		_t = _t->getNextSibling();
		
		// note that this version jumps 'dumb'
		// jumping into loops is legal, even then looping is not done
		
		// set the jump target - also logs the jump
		_t = static_cast<EnvUDT*>(callStack.back())->GotoTarget( g->targetIx);
		_t = _t->GetNextSibling();
		retCode=RC_OK;
		
		break;
	}
	case RETF:
	{
		ProgNodeP __t36 = _t;
		ProgNodeP tmp12_AST_in = _t;
		match(antlr::RefAST(_t),RETF);
		_t = _t->getFirstChild();
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if (((_tokenSet_1.member(_t->getType())))&&( !static_cast<EnvUDT*>(callStack.back())->LFun())) {
			e=expr(_t);
			_t = _retTree;
			
			delete returnValue;
			returnValue=e;
			retCode=RC_RETURN;
			callStack.back()->RemoveLoc( e); // steal e from local list
			
		}
		else if ((_tokenSet_2.member(_t->getType()))) {
			eL=l_ret_expr(_t);
			_t = _retTree;
			
			// returnValueL is otherwise owned
			returnValueL=eL;
			retCode=RC_RETURN;
			
		}
		else {
			throw antlr::NoViableAltException(antlr::RefAST(_t));
		}
		
		}
		_t = __t36;
		_t = _t->getNextSibling();
		break;
	}
	case RETP:
	{
		ProgNodeP tmp13_AST_in = _t;
		match(antlr::RefAST(_t),RETP);
		_t = _t->getNextSibling();
		
		retCode=RC_RETURN;
		
		break;
	}
	case BREAK:
	{
		ProgNodeP tmp14_AST_in = _t;
		match(antlr::RefAST(_t),BREAK);
		_t = _t->getNextSibling();
		
		retCode=RC_BREAK;
		
		break;
	}
	case CONTINUE:
	{
		ProgNodeP tmp15_AST_in = _t;
		match(antlr::RefAST(_t),CONTINUE);
		_t = _t->getNextSibling();
		
		retCode=RC_CONTINUE;
		
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

BaseGDL*  GDLInterpreter::expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	case ASSIGN_REPLACE:
	case ARRAYDEF:
	case ARRAYEXPR:
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
		res=check_expr(_t);
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

BaseGDL**  GDLInterpreter::l_simple_var(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_simple_var_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP var = ProgNodeP(antlr::nullAST);
	ProgNodeP varPtr = ProgNodeP(antlr::nullAST);
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	{
		var = _t;
		match(antlr::RefAST(_t),VAR);
		_t = _t->getNextSibling();
		
		
		res=&callStack.back()->GetKW(var->varIx); 
		
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
	default:
	{
		throw antlr::NoViableAltException(antlr::RefAST(_t));
	}
	}
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
		ProgNodeP __t57 = _t;
		ProgNodeP tmp16_AST_in = _t;
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
		
		_t = __t57;
		_t = _t->getNextSibling();
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
		"from left-function.");
		
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
		"Attempt to return a non-global variable from left-function.");
		
		res=&callStack.back()->GetKW(var->varIx); 
		
		break;
	}
	case ASSIGN:
	{
		ProgNodeP __t58 = _t;
		ProgNodeP tmp17_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ARRAYDEF:
		case ARRAYEXPR:
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
			e1=check_expr(_t);
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
		
		_t = __t58;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_REPLACE:
	{
		ProgNodeP __t60 = _t;
		ProgNodeP tmp18_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ARRAYDEF:
		case ARRAYEXPR:
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
			e1=check_expr(_t);
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
		
		_t = __t60;
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR:
	{
		ProgNodeP __t62 = _t;
		ProgNodeP tmp19_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		
		throw GDLException( _t, 
		"Indexed expression not allowed as left-function"
		" return value.");
		
		_t = __t62;
		_t = _t->getNextSibling();
		break;
	}
	case DOT:
	{
		ProgNodeP __t63 = _t;
		ProgNodeP tmp20_AST_in = _t;
		match(antlr::RefAST(_t),DOT);
		_t = _t->getFirstChild();
		
		throw GDLException( _t, 
		"Struct expression not allowed as left-function"
		" return value.");
		
		_t = __t63;
		_t = _t->getNextSibling();
		break;
	}
	case SYSVAR:
	{
		ProgNodeP tmp21_AST_in = _t;
		match(antlr::RefAST(_t),SYSVAR);
		_t = _t->getNextSibling();
		
		throw GDLException( _t, 
		"System variable not allowed as left-function"
		" return value.");
		
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
		"Expression not allowed as left-function return value.");
		
		break;
	}
	case CONSTANT:
	{
		e1=constant_nocopy(_t);
		_t = _retTree;
		
		throw GDLException( _t, 
		"Constant not allowed as left-function return value.");
		
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
	ProgNodeP knameR = ProgNodeP(antlr::nullAST);
	ProgNodeP knameE = ProgNodeP(antlr::nullAST);
	ProgNodeP kname = ProgNodeP(antlr::nullAST);
	ProgNodeP knameCk = ProgNodeP(antlr::nullAST);
	
	auto_ptr<EnvBaseT> guard(actEnv); 
	BaseGDL*  kval;
	BaseGDL*  pval;
	BaseGDL** kvalRef;
	BaseGDL** pvalRef;
	
	
	{ // ( ... )*
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case KEYDEF_REF:
		{
			ProgNodeP __t160 = _t;
			ProgNodeP tmp22_AST_in = _t;
			match(antlr::RefAST(_t),KEYDEF_REF);
			_t = _t->getFirstChild();
			knameR = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			kvalRef=ref_parameter(_t);
			_t = _retTree;
			// pass reference
			actEnv->SetKeyword( knameR->getText(), kvalRef); 
			
			_t = __t160;
			_t = _t->getNextSibling();
			break;
		}
		case KEYDEF_REF_EXPR:
		{
			ProgNodeP __t161 = _t;
			ProgNodeP tmp23_AST_in = _t;
			match(antlr::RefAST(_t),KEYDEF_REF_EXPR);
			_t = _t->getFirstChild();
			knameE = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			kval=expr(_t);
			_t = _retTree;
			kvalRef=ref_parameter(_t);
			_t = _retTree;
			// pass reference
			delete kval;
			actEnv->SetKeyword( knameE->getText(), kvalRef); 
			
			_t = __t161;
			_t = _t->getNextSibling();
			break;
		}
		case KEYDEF:
		{
			ProgNodeP __t162 = _t;
			ProgNodeP tmp24_AST_in = _t;
			match(antlr::RefAST(_t),KEYDEF);
			_t = _t->getFirstChild();
			kname = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			kval=expr(_t);
			_t = _retTree;
			// pass value
			actEnv->SetKeyword( kname->getText(), kval);
			
			_t = __t162;
			_t = _t->getNextSibling();
			break;
		}
		case REF:
		{
			ProgNodeP __t163 = _t;
			ProgNodeP tmp25_AST_in = _t;
			match(antlr::RefAST(_t),REF);
			_t = _t->getFirstChild();
			pvalRef=ref_parameter(_t);
			_t = _retTree;
			// pass reference
			actEnv->SetNextPar(pvalRef); 
			
			_t = __t163;
			_t = _t->getNextSibling();
			break;
		}
		case REF_EXPR:
		{
			ProgNodeP __t164 = _t;
			ProgNodeP tmp26_AST_in = _t;
			match(antlr::RefAST(_t),REF_EXPR);
			_t = _t->getFirstChild();
			pval=expr(_t);
			_t = _retTree;
			pvalRef=ref_parameter(_t);
			_t = _retTree;
			// pass reference
			delete pval;
			actEnv->SetNextPar(pvalRef); 
			
			_t = __t164;
			_t = _t->getNextSibling();
			break;
		}
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ARRAYDEF:
		case ARRAYEXPR:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case FCALL_LIB:
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
			pval=expr(_t);
			_t = _retTree;
			// pass value
			actEnv->SetNextPar(pval); 
			
			break;
		}
		case KEYDEF_REF_CHECK:
		{
			ProgNodeP __t165 = _t;
			ProgNodeP tmp27_AST_in = _t;
			match(antlr::RefAST(_t),KEYDEF_REF_CHECK);
			_t = _t->getFirstChild();
			knameCk = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			kval=check_expr(_t);
			_t = _retTree;
			
			kvalRef = callStack.back()->GetPtrTo( kval);
			if( kvalRef != NULL)
			{   // pass reference
			actEnv->SetKeyword(knameCk->getText(), kvalRef); 
			}
			else 
			{   // pass value
			actEnv->SetKeyword(knameCk->getText(), kval); 
			}
			
			_t = __t165;
			_t = _t->getNextSibling();
			break;
		}
		case REF_CHECK:
		{
			ProgNodeP __t166 = _t;
			ProgNodeP tmp28_AST_in = _t;
			match(antlr::RefAST(_t),REF_CHECK);
			_t = _t->getFirstChild();
			pval=check_expr(_t);
			_t = _retTree;
			
			pvalRef = callStack.back()->GetPtrTo( pval);
			if( pvalRef != NULL)
			{   // pass reference
			actEnv->SetNextPar( pvalRef); 
			}
			else 
			{   // pass value
			actEnv->SetNextPar( pval); 
			}
			
			_t = __t166;
			_t = _t->getNextSibling();
			break;
		}
		default:
		{
			goto _loop167;
		}
		}
	}
	_loop167:;
	} // ( ... )*
	
	actEnv->Extra(); // expand _EXTRA
	guard.release();
	
	_retTree = _t;
}

BaseGDL*  GDLInterpreter::indexable_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP indexable_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
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
		throw GDLException( _t, "Variable is undefined: "+Name(e2));
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
	
	BaseGDL*  e1;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case QUESTION:
	{
		ProgNodeP __t123 = _t;
		q = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getFirstChild();
		res = q->Eval();
		_t = __t123;
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR:
	{
		res=array_expr(_t);
		_t = _retTree;
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
	{
		res=assign_expr(_t);
		_t = _retTree;
		break;
	}
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

BaseGDL*  GDLInterpreter::check_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP check_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	res=lib_function_call(_t);
	_t = _retTree;
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
		ProgNodeP __t85 = _t;
		ProgNodeP tmp29_AST_in = _t;
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
		
		_t = __t85;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN:
	{
		ProgNodeP __t86 = _t;
		ProgNodeP tmp30_AST_in = _t;
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
		case ARRAYDEF:
		case ARRAYEXPR:
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
			e1=check_expr(_t);
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
		_t = __t86;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_REPLACE:
	{
		ProgNodeP __t88 = _t;
		ProgNodeP tmp31_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
		_t = _t->getFirstChild();
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ARRAYDEF:
		case ARRAYEXPR:
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
			e1=check_expr(_t);
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
		
		_t = __t88;
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
		throw GDLException( _t, 
		"System variable not allowed in this context.");
		
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
		right->TypeStr()+" "+right->Dim().ToString()+">,!"+ 
		sysVar->getText());
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
	case DOT:
	{
		ProgNodeP __t92 = _t;
		dot = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),DOT);
		_t = _t->getFirstChild();
		
		SizeT nDot=dot->nDot;
		auto_ptr<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
		
		l_dot_array_expr(_t, aD.get());
		_t = _retTree;
		{ // ( ... )+
		int _cnt94=0;
		for (;;) {
			if (_t == ProgNodeP(antlr::nullAST) )
				_t = ASTNULL;
			if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR || _t->getType() == IDENTIFIER)) {
				tag_array_expr(_t, aD.get());
				_t = _retTree;
			}
			else {
				if ( _cnt94>=1 ) { goto _loop94; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
			}
			
			_cnt94++;
		}
		_loop94:;
		}  // ( ... )+
		_t = __t92;
		_t = _t->getNextSibling();
		
		if( right == NULL)
		throw GDLException( _t, "Struct expression not allowed in this context.");
		
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
		"Expression not allowed as l-value.");
		
		break;
	}
	case CONSTANT:
	{
		e1=constant_nocopy(_t);
		_t = _retTree;
		
		throw GDLException( _t, 
		"Constant not allowed as l-value.");
		
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
		throw GDLException( _t, "Variable is undefined: "+Name(e2));
		
		res = (*e2)->Dup();
		
		break;
	}
	case QUESTION:
	{
		ProgNodeP __t128 = _t;
		q = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
		match(antlr::RefAST(_t),QUESTION);
		_t = _t->getFirstChild();
		res = q->Eval();
		_t = __t128;
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYEXPR:
	{
		res=array_expr(_t);
		_t = _retTree;
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
	{
		res=assign_expr(_t);
		_t = _retTree;
		break;
	}
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
		ProgNodeP __t151 = _t;
		ProgNodeP tmp32_AST_in = _t;
		match(antlr::RefAST(_t),FCALL_LIB);
		_t = _t->getFirstChild();
		fl = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
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
		
		_t = __t151;
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
			ProgNodeP __t154 = _t;
			ProgNodeP tmp33_AST_in = _t;
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
			_t = __t154;
			_t = _t->getNextSibling();
			break;
		}
		case MFCALL_PARENT:
		{
			ProgNodeP __t155 = _t;
			ProgNodeP tmp34_AST_in = _t;
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
			_t = __t155;
			_t = _t->getNextSibling();
			break;
		}
		case FCALL:
		{
			ProgNodeP __t156 = _t;
			ProgNodeP tmp35_AST_in = _t;
			match(antlr::RefAST(_t),FCALL);
			_t = _t->getFirstChild();
			f = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			
			SetFunIx( f);
			
			newEnv=new EnvUDT( f, funList[f->funIx], true);
			
			parameter_def(_t, newEnv);
			_t = _retTree;
			_t = __t156;
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

BaseGDL**  GDLInterpreter::l_deref(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_deref_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	BaseGDL*       e1;
	
	
	ProgNodeP __t55 = _t;
	ProgNodeP tmp36_AST_in = _t;
	match(antlr::RefAST(_t),DEREF);
	_t = _t->getFirstChild();
	e1=expr(_t);
	_t = _retTree;
	
	auto_ptr<BaseGDL> e1_guard(e1);
	
	DPtrGDL* ptr=dynamic_cast<DPtrGDL*>(e1);
	if( ptr == NULL)
	throw GDLException( _t, "Pointer type required"
	" in this context: "+Name(e1));
	DPtr sc; 
	if( !ptr->Scalar(sc))
	throw GDLException( _t, "Expression must be a "
	"scalar in this context: "+Name(e1));
	if( sc == 0)
	throw GDLException( _t, "Unable to dereference"
	" NULL pointer: "+Name(e1));
	
	try{
	res = &GetHeap(sc);
	}
	catch( HeapException)
	{
	throw GDLException( _t, "Invalid pointer: "+Name(e1));
	}
	
	_t = __t55;
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::l_decinc_expr(ProgNodeP _t,
	int dec_inc
) {
	BaseGDL* res;
	ProgNodeP l_decinc_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	BaseGDL*       e1;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case QUESTION:
	{
		ProgNodeP __t72 = _t;
		ProgNodeP tmp37_AST_in = _t;
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
		
		_t = __t72;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN:
	{
		ProgNodeP __t73 = _t;
		ProgNodeP tmp38_AST_in = _t;
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
		case ARRAYDEF:
		case ARRAYEXPR:
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
			e1=check_expr(_t);
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
		_t = __t73;
		_t = _t->getNextSibling();
		break;
	}
	case ASSIGN_REPLACE:
	{
		ProgNodeP __t75 = _t;
		ProgNodeP tmp39_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ARRAYDEF:
		case ARRAYEXPR:
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
			e1=check_expr(_t);
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
		_t = __t75;
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
		"Expression not allowed with decrement/increment operator.");
		
		break;
	}
	case CONSTANT:
	{
		e1=constant_nocopy(_t);
		_t = _retTree;
		
		throw GDLException( _t, 
		"Constant not allowed with decrement/increment operator.");
		
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

BaseGDL*  GDLInterpreter::r_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP r_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP e = ProgNodeP(antlr::nullAST);
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case EXPR:
	{
		e = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getNextSibling();
		res = e->Eval();
		break;
	}
	case DEC:
	{
		ProgNodeP __t99 = _t;
		ProgNodeP tmp40_AST_in = _t;
		match(antlr::RefAST(_t),DEC);
		_t = _t->getFirstChild();
		res=l_decinc_expr(_t, DEC);
		_t = _retTree;
		_t = __t99;
		_t = _t->getNextSibling();
		break;
	}
	case INC:
	{
		ProgNodeP __t100 = _t;
		ProgNodeP tmp41_AST_in = _t;
		match(antlr::RefAST(_t),INC);
		_t = _t->getFirstChild();
		res=l_decinc_expr(_t, INC);
		_t = _retTree;
		_t = __t100;
		_t = _t->getNextSibling();
		break;
	}
	case POSTDEC:
	{
		ProgNodeP __t101 = _t;
		ProgNodeP tmp42_AST_in = _t;
		match(antlr::RefAST(_t),POSTDEC);
		_t = _t->getFirstChild();
		res=l_decinc_expr(_t, POSTDEC);
		_t = _retTree;
		_t = __t101;
		_t = _t->getNextSibling();
		break;
	}
	case POSTINC:
	{
		ProgNodeP __t102 = _t;
		ProgNodeP tmp43_AST_in = _t;
		match(antlr::RefAST(_t),POSTINC);
		_t = _t->getFirstChild();
		res=l_decinc_expr(_t, POSTINC);
		_t = _retTree;
		_t = __t102;
		_t = _t->getNextSibling();
		break;
	}
	case ARRAYDEF:
	{
		res=array_def(_t);
		_t = _retTree;
		break;
	}
	case NSTRUC:
	case NSTRUC_REF:
	case STRUC:
	{
		res=struct_def(_t);
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

BaseGDL*  GDLInterpreter::constant_nocopy(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP constant_nocopy_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP c = ProgNodeP(antlr::nullAST);
	
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
		throw GDLException( _t, "Variable is undefined: "+Name(e));
		
		break;
	}
	case DEREF:
	{
		e=l_deref(_t);
		_t = _retTree;
		
		res = *e;
		if( res == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(e));
		
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
	ProgNodeP var = ProgNodeP(antlr::nullAST);
	ProgNodeP varPtr = ProgNodeP(antlr::nullAST);
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case VAR:
	{
		var = _t;
		match(antlr::RefAST(_t),VAR);
		_t = _t->getNextSibling();
		
		res=&callStack.back()->GetKW(var->varIx); 
		if( *res == NULL)
		throw GDLException( _t, "Variable is undefined: "+
		callStack.back()->GetString(var->varIx));
		
		break;
	}
	case VARPTR:
	{
		varPtr = _t;
		match(antlr::RefAST(_t),VARPTR);
		_t = _t->getNextSibling();
		
		res=&varPtr->var->Data(); // returns BaseGDL* of var (DVar*) 
		if( *res == NULL)
		throw GDLException( _t, "Variable is undefined: "+
		callStack.back()->GetString( *res));
		
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
	sysVar->getText());
	
	// note: this works, because system variables are never 
	//       passed by reference
	SizeT rdOnlySize = sysVarRdOnlyList.size();
	for( SizeT i=0; i<rdOnlySize; ++i)
	if( sysVarRdOnlyList[ i] == sysVar->var)
	throw GDLException( _t, 
	"Attempt to write to a readonly variable: !"+
	sysVar->getText());
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
		ProgNodeP __t66 = _t;
		ProgNodeP tmp44_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		e=l_decinc_indexable_expr(_t, dec_inc);
		_t = _retTree;
		aL=arrayindex_list(_t);
		_t = _retTree;
		_t = __t66;
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
	
	
	ProgNodeP __t169 = _t;
	ax = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),ARRAYIX);
	_t = _t->getFirstChild();
	
	aL = ax->arrIxList;
	assert( aL != NULL);
	
	nExpr = aL->NParam();
	if( nExpr == 0)
	{
	aL->Init();
	goto empty;
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
				s=check_expr(_t);
				_t = _retTree;
				
				if( !callStack.back()->Contains( s)) 
				exprList.push_back( s);
				
				break;
			}
			case ASSIGN:
			case ASSIGN_REPLACE:
			case ARRAYDEF:
			case ARRAYEXPR:
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
			goto _loop172;
		}
		
	}
	_loop172:;
	} // ( ... )*
	_t = __t169;
	_t = _t->getNextSibling();
	
	aL->Init( ixExprList);
	empty:
	
	_retTree = _t;
	return aL;
}

BaseGDL*  GDLInterpreter::l_decinc_dot_expr(ProgNodeP _t,
	int dec_inc
) {
	BaseGDL* res;
	ProgNodeP l_decinc_dot_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP dot = ProgNodeP(antlr::nullAST);
	
	ProgNodeP __t68 = _t;
	dot = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),DOT);
	_t = _t->getFirstChild();
	
	SizeT nDot=dot->nDot;
	auto_ptr<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
	
	l_dot_array_expr(_t, aD.get());
	_t = _retTree;
	{ // ( ... )+
	int _cnt70=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR || _t->getType() == IDENTIFIER)) {
			tag_array_expr(_t, aD.get());
			_t = _retTree;
		}
		else {
			if ( _cnt70>=1 ) { goto _loop70; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt70++;
	}
	_loop70:;
	}  // ( ... )+
	_t = __t68;
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
		ProgNodeP __t83 = _t;
		ProgNodeP tmp45_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		rP=l_indexable_expr(_t);
		_t = _retTree;
		aL=arrayindex_list(_t);
		_t = _retTree;
		guard.reset(aL);
		_t = __t83;
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
		" STRUCT in this context: "+Name(*rP));
		}
		}
		else 
		{
		if( (*rP)->IsAssoc())
		throw GDLException( _t, "File expression not allowed "
		"in this context: "+Name(*rP));
		
		aD->Root( structR, guard.release() /* aL */); 
		}
		
		break;
	}
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
		" STRUCT in this context: "+Name(*rP));
		}
		}
		else
		{
		if( (*rP)->IsAssoc())
		{
		throw GDLException( _t, "File expression not allowed "
		"in this context: "+Name(*rP));
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
		ProgNodeP __t113 = _t;
		ProgNodeP tmp46_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		tag_expr(_t, aD);
		_t = _retTree;
		aL=arrayindex_list(_t);
		_t = _retTree;
		aD->AddIx(aL);
		_t = __t113;
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

BaseGDL**  GDLInterpreter::l_indexable_expr(ProgNodeP _t) {
	BaseGDL** res;
	ProgNodeP l_indexable_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case EXPR:
	{
		ProgNodeP __t79 = _t;
		ProgNodeP tmp47_AST_in = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getFirstChild();
		res=l_expr(_t, NULL);
		_t = _retTree;
		_t = __t79;
		_t = _t->getNextSibling();
		
		if( *res == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(res));
		
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
		throw GDLException( _t, "Variable is undefined: "+Name(res));
		
		break;
	}
	case DEREF:
	{
		res=l_deref(_t);
		_t = _retTree;
		
		if( *res == NULL)
		throw GDLException( _t, "Variable is undefined: "+Name(res));
		
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
	
	
	ProgNodeP __t81 = _t;
	ProgNodeP tmp48_AST_in = _t;
	match(antlr::RefAST(_t),ARRAYEXPR);
	_t = _t->getFirstChild();
	res=l_indexable_expr(_t);
	_t = _retTree;
	aL=arrayindex_list(_t);
	_t = _retTree;
	guard.reset(aL);
	_t = __t81;
	_t = _t->getNextSibling();
	
	if( right == NULL)
	throw GDLException( _t, 
	"Indexed expression not allowed in this context.");
	
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

BaseGDL*  GDLInterpreter::array_def(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP array_def_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP a = ProgNodeP(antlr::nullAST);
	
	DType  cType=UNDEF; // conversion type
	SizeT maxRank=0;
	BaseGDL* e;
	ExprListT          exprList;
	BaseGDL*           cTypeData;
	
	
	ProgNodeP __t174 = _t;
	a = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),ARRAYDEF);
	_t = _t->getFirstChild();
	{ // ( ... )+
	int _cnt176=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_tokenSet_1.member(_t->getType()))) {
			e=expr(_t);
			_t = _retTree;
			
			// add first (this way it will get cleaned up anyway)
			exprList.push_back(e);
			
			DType ty=e->Type();
			if( ty == UNDEF)
			{
			throw GDLException( _t, "Variable is undefined: "+
			Name(e));
			}
			if( cType == UNDEF) 
			{
			cType=ty;
			cTypeData=e;
			}
			else 
			{ 
			if( cType != ty) 
			{
			if( DTypeOrder[ty] > 100) // struct, ptr, object
			{
			throw 
			GDLException( _t, e->TypeStr()+
			" is not allowed in this context.");
			}
			
			// update order if larger type (or types are equal)
			if( DTypeOrder[ty] >= DTypeOrder[cType]) 
			{
			cType=ty;
			cTypeData=e;
			}
			}
			if( ty == STRUCT)
			{
			// check for struct compatibility
			DStructDesc* newS=
			static_cast<DStructGDL*>(e)->Desc();
			DStructDesc* oldS=
			static_cast<DStructGDL*>(cTypeData)->Desc();
			
			// *** here (*newS) != (*oldS) must be set when
			// unnamed structs not in struct list anymore
			// WRONG! This speeds up things for named structs
			// unnamed structs all have their own desc
			// and thus the next is always true for them
			if( newS != oldS)
			{
			//                                 if( (*newS) == (*oldS))
			//                                 {
			// Not needed, CatArray puts the right descriptor
			//                                     // different structs with same layout
			//                                     // replace desc with first one
			//                                     if( oldS->IsUnnamed())
			//                                         oldS = new DStructDesc( oldS);
			
			//                                     static_cast<DStructGDL*>(e)->SetDesc( oldS);
			//                                 }
			//                                 else
			
			if( (*newS) != (*oldS))
			throw GDLException( _t, 
			"Conflicting data structures: "+
			Name(cTypeData)+", "+Name(e));
			}
			}
			}
			
			// memorize maximum Rank
			SizeT rank=e->Rank();
			if( rank > maxRank) maxRank=rank;
			
		}
		else {
			if ( _cnt176>=1 ) { goto _loop176; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt176++;
	}
	_loop176:;
	}  // ( ... )+
	_t = __t174;
	_t = _t->getNextSibling();
	
	res=cTypeData->CatArray(exprList,a->arrayDepth,maxRank);
	
	_retTree = _t;
	return res;
}

 BaseGDL*  GDLInterpreter::struct_def(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP struct_def_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP idRef = ProgNodeP(antlr::nullAST);
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case NSTRUC:
	{
		res=named_struct_def(_t);
		_t = _retTree;
		break;
	}
	case STRUC:
	{
		res=unnamed_struct_def(_t);
		_t = _retTree;
		break;
	}
	case NSTRUC_REF:
	{
		ProgNodeP __t186 = _t;
		ProgNodeP tmp49_AST_in = _t;
		match(antlr::RefAST(_t),NSTRUC_REF);
		_t = _t->getFirstChild();
		idRef = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		// find struct 'id'
		// returns it or throws an exception
		DStructDesc* dStruct=GetStruct( idRef->getText(), _t);
		
		res=new DStructGDL( dStruct, dimension(1));
		
		_t = __t186;
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
	
	
	ProgNodeP __t104 = _t;
	ProgNodeP tmp50_AST_in = _t;
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
	case ARRAYDEF:
	case ARRAYEXPR:
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
		r=check_expr(_t);
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
	ProgNodeP __t106 = _t;
	ax = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),ARRAYIX);
	_t = _t->getFirstChild();
	
	aL = ax->arrIxList;
	assert( aL != NULL);
	
	guard.reset(aL);
	
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
				s=check_expr(_t);
				_t = _retTree;
				
				if( !callStack.back()->Contains( s)) 
				exprList.push_back( s);
				
				break;
			}
			case ASSIGN:
			case ASSIGN_REPLACE:
			case ARRAYDEF:
			case ARRAYEXPR:
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
			goto _loop109;
		}
		
	}
	_loop109:;
	} // ( ... )*
	_t = __t106;
	_t = _t->getNextSibling();
	
	empty:
	res = aL->Index( r, ixExprList);
	//                 aL->Init( ixExprList);
	//                 aL->SetVariable( r);
	//                 res=r->Index( aL);
	//                ClearTmpList();
	
	_t = __t104;
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
		ProgNodeP __t111 = _t;
		ProgNodeP tmp51_AST_in = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getFirstChild();
		e=expr(_t);
		_t = _retTree;
		
		auto_ptr<BaseGDL> e_guard(e);
		
		SizeT tagIx;
		int ret=e->Scalar2index(tagIx);
		if( ret < 1)
		throw GDLException( _t, "Expression must be a scalar"
		" >= 0 in this context: "+Name(e));
		
		aD->Add( tagIx);
		
		_t = __t111;
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
		ProgNodeP __t115 = _t;
		ProgNodeP tmp52_AST_in = _t;
		match(antlr::RefAST(_t),EXPR);
		_t = _t->getFirstChild();
		res=expr(_t);
		_t = _retTree;
		aD->SetOwner( true);
		_t = __t115;
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
	
	sysVar = _t;
	match(antlr::RefAST(_t),SYSVAR);
	_t = _t->getNextSibling();
	
	if( sysVar->var == NULL) 
	{
	sysVar->var=FindInVarList(sysVarList,sysVar->getText());
	if( sysVar->var == NULL)		    
	throw GDLException( _t, "Not a legal system variable: !"+
	sysVar->getText());
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
		ProgNodeP __t117 = _t;
		ProgNodeP tmp53_AST_in = _t;
		match(antlr::RefAST(_t),ARRAYEXPR);
		_t = _t->getFirstChild();
		r=r_dot_indexable_expr(_t, aD);
		_t = _retTree;
		aL=arrayindex_list(_t);
		_t = _retTree;
		guard.reset(aL);
		_t = __t117;
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
		" STRUCT in this context: "+Name(r));
		}
		}
		else
		{
		if( r->IsAssoc())
		throw GDLException( _t, "File expression not allowed "
		"in this context: "+Name(r));
		
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
		" STRUCT in this context: "+Name(r));
		}
		}
		else
		{
		if( r->IsAssoc())
		{
		throw GDLException( _t, "File expression not allowed "
		"in this context: "+Name(r));
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
	
	ProgNodeP __t119 = _t;
	dot = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),DOT);
	_t = _t->getFirstChild();
	
	SizeT nDot=dot->nDot;
	auto_ptr<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
	
	r_dot_array_expr(_t, aD.get());
	_t = _retTree;
	{ // ( ... )+
	int _cnt121=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR || _t->getType() == IDENTIFIER)) {
			tag_array_expr(_t, aD.get());
			_t = _retTree;
		}
		else {
			if ( _cnt121>=1 ) { goto _loop121; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt121++;
	}
	_loop121:;
	}  // ( ... )+
	_t = __t119;
	_t = _t->getNextSibling();
	res= aD->Resolve();
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::assign_expr(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP assign_expr_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
	BaseGDL** l;
	BaseGDL*  r;
	
	
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case ASSIGN:
	{
		ProgNodeP __t130 = _t;
		ProgNodeP tmp54_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ARRAYDEF:
		case ARRAYEXPR:
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
			res=check_expr(_t);
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
		ProgNodeP tmp55_AST_in = _t;
		match(antlr::RefAST(_t),ASSIGN_REPLACE);
		_t = _t->getFirstChild();
		
		auto_ptr<BaseGDL> r_guard;
		
		{
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ARRAYDEF:
		case ARRAYEXPR:
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
			res=check_expr(_t);
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
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	BaseGDL *self;
	EnvUDT*   newEnv;
	
	
	{
	{
	if (_t == ProgNodeP(antlr::nullAST) )
		_t = ASTNULL;
	switch ( _t->getType()) {
	case MFCALL:
	{
		ProgNodeP __t147 = _t;
		ProgNodeP tmp56_AST_in = _t;
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
		_t = __t147;
		_t = _t->getNextSibling();
		break;
	}
	case MFCALL_PARENT:
	{
		ProgNodeP __t148 = _t;
		ProgNodeP tmp57_AST_in = _t;
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
		_t = __t148;
		_t = _t->getNextSibling();
		break;
	}
	case FCALL:
	{
		ProgNodeP __t149 = _t;
		ProgNodeP tmp58_AST_in = _t;
		match(antlr::RefAST(_t),FCALL);
		_t = _t->getFirstChild();
		f = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		SetFunIx( f);
		
		newEnv=new EnvUDT( f, funList[f->funIx]);
		
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
	
	}
	_retTree = _t;
	return res;
}

 BaseGDL*  GDLInterpreter::lib_function_call_retnew(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP lib_function_call_retnew_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP fl = ProgNodeP(antlr::nullAST);
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	
	
	ProgNodeP __t143 = _t;
	ProgNodeP tmp59_AST_in = _t;
	match(antlr::RefAST(_t),FCALL_LIB_RETNEW);
	_t = _t->getFirstChild();
	fl = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	
	EnvT* newEnv=new EnvT( fl, fl->libFun);//libFunList[fl->funIx]);
	
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

 BaseGDL*  GDLInterpreter::lib_function_call(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP lib_function_call_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP fl = ProgNodeP(antlr::nullAST);
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	
	
	ProgNodeP __t141 = _t;
	ProgNodeP tmp60_AST_in = _t;
	match(antlr::RefAST(_t),FCALL_LIB);
	_t = _t->getFirstChild();
	fl = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	
	EnvT* newEnv=new EnvT( fl, fl->libFun);//libFunList[fl->funIx]);
	
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

BaseGDL*  GDLInterpreter::constant(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP constant_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP c = ProgNodeP(antlr::nullAST);
	
	c = _t;
	match(antlr::RefAST(_t),CONSTANT);
	_t = _t->getNextSibling();
	
	res=c->cData->Dup(); 
	
	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::simple_var(ProgNodeP _t) {
	BaseGDL* res;
	ProgNodeP simple_var_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP var = ProgNodeP(antlr::nullAST);
	ProgNodeP varPtr = ProgNodeP(antlr::nullAST);
	
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
		throw GDLException( _t, "Variable is undefined: "+var->getText());
		
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
		throw GDLException( _t, "Common block variable is undefined.");
		
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
	
	BaseGDL* sv;
	
	
	sv=sys_var_nocopy(_t);
	_t = _retTree;
	
	res=sv->Dup();
	
	_retTree = _t;
	return res;
}

 BaseGDL**  GDLInterpreter::ref_parameter(ProgNodeP _t) {
	 BaseGDL** ret;
	ProgNodeP ref_parameter_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	
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

 BaseGDL*  GDLInterpreter::named_struct_def(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP named_struct_def_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP n = ProgNodeP(antlr::nullAST);
	ProgNodeP id = ProgNodeP(antlr::nullAST);
	ProgNodeP i = ProgNodeP(antlr::nullAST);
	ProgNodeP ii = ProgNodeP(antlr::nullAST);
	
	DStructDesc*          nStructDesc;
	auto_ptr<DStructDesc> nStructDescGuard;
	BaseGDL* e;
	BaseGDL* ee;
	
	
	ProgNodeP __t178 = _t;
	n = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	match(antlr::RefAST(_t),NSTRUC);
	_t = _t->getFirstChild();
	id = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	
	// definedStruct: no tags present
	if( n->definedStruct == 1) GetStruct( id->getText(), _t);
	
	// find struct 'id' (for compatibility check)
	DStructDesc* oStructDesc=
	FindInStructList( structList, id->getText());
	
	if( oStructDesc == NULL || oStructDesc->NTags() > 0)
	{
	// not defined at all yet (-> define now)
	// or completely defined  (-> define now and check equality)
	nStructDesc= new DStructDesc( id->getText());
	
	// guard it
	nStructDescGuard.reset( nStructDesc); 
	} 
	else
	{   // NTags() == 0
	// not completely defined (only name in list)
	nStructDesc= oStructDesc;
	}
	
	// the instance variable
	DStructGDL* instance= new DStructGDL( nStructDesc,
	dimension(1)); 
	auto_ptr<DStructGDL> instance_guard(instance);
	
	{ // ( ... )+
	int _cnt180=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case ASSIGN:
		case ASSIGN_REPLACE:
		case ARRAYDEF:
		case ARRAYEXPR:
		case CONSTANT:
		case DEREF:
		case EXPR:
		case FCALL:
		case FCALL_LIB:
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
			ee=expr(_t);
			_t = _retTree;
			
			// also adds to descriptor, grabs
			instance->NewTag( 
			oStructDesc->TagName( nStructDesc->NTags()),
			ee);
			
			break;
		}
		case IDENTIFIER:
		{
			i = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			e=expr(_t);
			_t = _retTree;
			
			// also adds to descriptor, grabs
			instance->NewTag( i->getText(), e); 
			
			break;
		}
		case INHERITS:
		{
			ProgNodeP tmp61_AST_in = _t;
			match(antlr::RefAST(_t),INHERITS);
			_t = _t->getNextSibling();
			ii = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			
			DStructDesc* inherit=GetStruct( ii->getText(), _t);
			
			nStructDesc->AddParent( inherit);
			instance->AddParent( inherit);
			
			break;
		}
		default:
		{
			if ( _cnt180>=1 ) { goto _loop180; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		}
		_cnt180++;
	}
	_loop180:;
	}  // ( ... )+
	
	// inherit refers to nStructDesc, in case of error both have to
	// be freed here
	if( oStructDesc != NULL)
	{
	if( oStructDesc != nStructDesc)
	{
	oStructDesc->AssureIdentical(nStructDesc);
	instance->DStructGDL::SetDesc(oStructDesc);
	//delete nStructDesc; // auto_ptr
	}
	}
	else
	{
	// release from guard (if not NULL)
	nStructDescGuard.release();
	
	// insert into struct list 
	structList.push_back(nStructDesc);
	}
	
	instance_guard.release();
	res=instance;
	
	_t = __t178;
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
}

 BaseGDL*  GDLInterpreter::unnamed_struct_def(ProgNodeP _t) {
	 BaseGDL* res;
	ProgNodeP unnamed_struct_def_AST_in = (_t == ProgNodeP(ASTNULL)) ? ProgNodeP(antlr::nullAST) : _t;
	ProgNodeP si = ProgNodeP(antlr::nullAST);
	
	// don't forget the struct in extrat.cpp if you change something here
	// "$" as first char in the name is necessary 
	// as it defines unnnamed structs (see dstructdesc.hpp)
	DStructDesc*   nStructDesc = new DStructDesc( "$truct");
	
	// instance takes care of nStructDesc since it is unnamed
	DStructGDL* instance = new DStructGDL( nStructDesc, dimension(1));
	auto_ptr<DStructGDL> instance_guard(instance);
	
	BaseGDL* e;
	
	
	ProgNodeP __t182 = _t;
	ProgNodeP tmp62_AST_in = _t;
	match(antlr::RefAST(_t),STRUC);
	_t = _t->getFirstChild();
	{ // ( ... )+
	int _cnt184=0;
	for (;;) {
		if (_t == ProgNodeP(antlr::nullAST) )
			_t = ASTNULL;
		if ((_t->getType() == IDENTIFIER)) {
			si = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			e=expr(_t);
			_t = _retTree;
			
			// also adds to descriptor, grabs
			instance->NewTag( si->getText(), e); 
			
		}
		else {
			if ( _cnt184>=1 ) { goto _loop184; } else {throw antlr::NoViableAltException(antlr::RefAST(_t));}
		}
		
		_cnt184++;
	}
	_loop184:;
	}  // ( ... )+
	
	//                 DStructDesc* oStructDesc=nStructDesc->FindEqual( structList);
	//                 if( oStructDesc != NULL)
	//                 {
	//                     instance->SetDesc(oStructDesc);
	//                     //delete nStructDesc; // auto_ptr
	//                 }
	//                 else
	//                 {
	//                     // insert into struct list
	//                     structList.push_back( nStructDesc.release());
	//                 }
	
	instance_guard.release();
	res=instance;
	
	_t = __t182;
	_t = _t->getNextSibling();
	_retTree = _t;
	return res;
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
	"ARRAYDEF",
	"ARRAYIX",
	"ARRAYIX_ALL",
	"ARRAYIX_ORANGE",
	"ARRAYIX_RANGE",
	"ARRAYIX_ORANGE_S",
	"ARRAYIX_RANGE_S",
	"ARRAYEXPR",
	"ARRAYEXPR_FN",
	"BLOCK",
	"BREAK",
	"CONTINUE",
	"COMMONDECL",
	"COMMONDEF",
	"CONSTANT",
	"DEREF",
	"ELSEBLK",
	"EXPR",
	"FOR_STEP",
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
	"POSTDEC",
	"POSTINC",
	"DECSTATEMENT",
	"INCSTATEMENT",
	"REF",
	"REF_CHECK",
	"REF_EXPR",
	"RETURN",
	"RETF",
	"RETP",
	"STRUC",
	"SYSVAR",
	"UMINUS",
	"VAR",
	"VARPTR",
	"\"end\"",
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
	"\"inherits\"",
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
	"LOG_NEG",
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

const unsigned long GDLInterpreter::_tokenSet_0_data_[] = { 1141768352UL, 805765232UL, 402671616UL, 32112640UL, 0UL, 0UL, 0UL, 0UL };
// ASSIGN ASSIGN_REPLACE BLOCK BREAK CONTINUE FOR_STEP IF_ELSE LABEL MPCALL 
// MPCALL_PARENT ON_IOERROR_NULL PCALL PCALL_LIB RETF RETP "switch" "case" 
// DEC INC "repeat" "while" "for" "goto" "on_ioerror" "if" 
const antlr::BitSet GDLInterpreter::_tokenSet_0(_tokenSet_0_data_,8);
const unsigned long GDLInterpreter::_tokenSet_1_data_[] = { 985694624UL, 3224421504UL, 402653190UL, 0UL, 0UL, 33554436UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// ASSIGN ASSIGN_REPLACE ARRAYDEF ARRAYEXPR CONSTANT DEREF EXPR FCALL FCALL_LIB 
// FCALL_LIB_RETNEW MFCALL MFCALL_PARENT NSTRUC NSTRUC_REF POSTDEC POSTINC 
// STRUC SYSVAR VAR VARPTR DEC INC DOT QUESTION 
const antlr::BitSet GDLInterpreter::_tokenSet_1(_tokenSet_1_data_,12);
const unsigned long GDLInterpreter::_tokenSet_2_data_[] = { 448823712UL, 3224421504UL, 402653190UL, 0UL, 0UL, 33554436UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL };
// ASSIGN ASSIGN_REPLACE ARRAYDEF ARRAYEXPR CONSTANT DEREF EXPR FCALL FCALL_LIB 
// MFCALL MFCALL_PARENT NSTRUC NSTRUC_REF POSTDEC POSTINC STRUC SYSVAR 
// VAR VARPTR DEC INC DOT QUESTION 
const antlr::BitSet GDLInterpreter::_tokenSet_2(_tokenSet_2_data_,12);


