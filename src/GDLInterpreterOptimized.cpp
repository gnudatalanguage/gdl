#define NOTEST
#ifdef NOTEST

/***************************************************************************
       GDLInterpreterOptimized.cpp  -  Optimized functions for GDLInterpreter
                             -------------------
    begin                : May 03 2008
    copyright            : (C) 2008 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"

#include "GDLInterpreter.hpp"
#include <antlr/Token.hpp>
#include <antlr/AST.hpp>

    // gets inserted after the antlr generated includes in the cpp file
#include "dinterpreter.hpp"

#include <cassert>

// tweaking ANTLR
#define ASTNULL          NULLProgNodeP
#define ProgNodeP( xxx ) NULL             /* ProgNodeP(antlr::nullAST) */
#define RefAST( xxx)     ConvertAST( xxx) /* antlr::RefAST( Ref type)  */
#define match( a, b)     // remove from source for now

using namespace std;

BaseGDL*  GDLInterpreter::constant(ProgNodeP _t) {

	BaseGDL* res = _t->cData->Dup(); 
	
	_retTree = _t->getNextSibling();
	return res;
}

GDLInterpreter::RetCode  GDLInterpreter::interactive(ProgNodeP _t) { // called from several places
	return statement_list(_t);
}

GDLInterpreter::RetCode  GDLInterpreter::statement_list(ProgNodeP _t) {
	 GDLInterpreter::RetCode retCode;
	for (; _t != NULL;) {

		retCode=statement(_t);
		_t = _retTree;
			
		if( retCode != RC_OK) break; // break out if non-regular
	}
	_retTree = _t;
	return retCode;
}

GDLInterpreter::RetCode  GDLInterpreter::execute(ProgNodeP _t) {

	ValueGuard<bool> guard( interruptEnable);
	interruptEnable = false;
	
	return statement_list(_t);
}

BaseGDL* GDLInterpreter::call_fun(ProgNodeP _t) { // called from pythongdl.cpp
	 BaseGDL* res = NULL;
	returnValue = NULL;
	GDLInterpreter::RetCode retCode;
	
	for (; _t != NULL;) {

			retCode=statement(_t);
			_t = _retTree;
			
			if( retCode == RC_RETURN) 
			{
			res=returnValue;
			returnValue=NULL;
			
			break;
			}
					
	}
	
	// default return value if none was set
	if( res == NULL) res = new DIntGDL( 0); 
	
	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::call_lfun(ProgNodeP _t) {
	 BaseGDL** res = NULL;
	returnValueL = NULL;
	GDLInterpreter::RetCode retCode;
	
	ProgNodeP in = _t;

	for (; _t != NULL;) {
			retCode=statement(_t);
			_t = _retTree;
			
			if( retCode == RC_RETURN) 
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
	" must return a left-value in this context.");
	
	_retTree = _t;
	return res;
}

void GDLInterpreter::call_pro(ProgNodeP _t) { 
// called from RunDelTree() (InterpreterLoop()), pythongdl.cpp
	
	GDLInterpreter::RetCode retCode;
	
	for (; _t != NULL;) {
			retCode=statement(_t);
			_t = _retTree;
			
			// added RC_ABORT here
			if( retCode >= RC_RETURN) break;
	}
	_retTree = _t;
}

GDLInterpreter::RetCode GDLInterpreter::block(ProgNodeP _t) {
    GDLInterpreter::RetCode retCode;
	
	retCode = RC_OK;
	
	ProgNodeP rTree = _t->getNextSibling();
	match(antlr::RefAST(_t),BLOCK);
	_t = _t->getFirstChild();
	if (_t != NULL)
		{
		retCode=statement_list(_t);
		}
	_retTree = rTree;
	return retCode;
}

BaseGDL* GDLInterpreter::expr(ProgNodeP _t) {
	
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

}

BaseGDL**  GDLInterpreter::l_simple_var(ProgNodeP _t) {
	BaseGDL** res;
	
	assert( _t != NULL);

	if( _t->getType() == VAR)
	{
		ProgNodeP var = _t;
		match(antlr::RefAST(_t),VAR);
		_t = _t->getNextSibling();
		
		
		res=&callStack.back()->GetKW(var->varIx); 
		
	}
	else
	{
		ProgNodeP varPtr = _t;
		match(antlr::RefAST(_t),VARPTR);
		_t = _t->getNextSibling();
		
		res=&varPtr->var->Data(); // returns BaseGDL* of var (DVar*) 
		
	}

	_retTree = _t;
	return res;
}

BaseGDL*  GDLInterpreter::sys_var(ProgNodeP _t) { // *

	BaseGDL* sv = sys_var_nocopy(_t);
	//_t = _retTree;
	
	BaseGDL* res=sv->Dup();
	
	//_retTree = _t;
	return res;
}

BaseGDL* GDLInterpreter::sys_var_nocopy(ProgNodeP _t) {

	ProgNodeP& sysVar = _t;
	
	match(antlr::RefAST(_t),SYSVAR);
	
	if( sysVar->var == NULL) 
	{
	sysVar->var=FindInVarList(sysVarList,sysVar->getText());
	if( sysVar->var == NULL)		    
	throw GDLException( _t, "Not a legal system variable: !"+
	sysVar->getText());
	}
	
	if( sysVar->getText() == "STIME") SysVar::UpdateSTime();
	
	// note: system variables are always defined
	
	_retTree = _t->getNextSibling();;
	return sysVar->var->Data(); // no ->Dup()
}

BaseGDL* GDLInterpreter::dot_expr(ProgNodeP _t) {

	ProgNodeP rTree = _t->getNextSibling();
	ProgNodeP dot = _t;
	match(antlr::RefAST(_t),DOT);
	_t = _t->getFirstChild();
	
	SizeT nDot=dot->nDot;
	auto_ptr<DotAccessDescT> aD( new DotAccessDescT(nDot+1));
	
	r_dot_array_expr(_t, aD.get());
	_t = _retTree;
	for (; _t != NULL;) {
			tag_array_expr(_t, aD.get());
			_t = _retTree;
	}
	BaseGDL* res= aD->Resolve();
	_retTree = rTree;
	return res;
}

BaseGDL* GDLInterpreter::simple_var(ProgNodeP _t) { // *
	BaseGDL* res;

	assert( _t != NULL);

	if( _t->getType() == VAR)
	{
		ProgNodeP var = _t;
		match(antlr::RefAST(_t),VAR);
		_t = _t->getNextSibling();
		
		BaseGDL* vData=callStack.back()->GetKW( var->varIx);
		
		if( vData == NULL)
		throw GDLException( _t, "Variable is undefined: "+var->getText());
		
		res=vData->Dup();
		
	}
	else // VARPTR
	{
		ProgNodeP varPtr = _t;
		match(antlr::RefAST(_t),VARPTR);
		_t = _t->getNextSibling();
		
		BaseGDL* vData=varPtr->var->Data();
		
		if( vData == NULL)
		throw GDLException( _t, "Common block variable is undefined.");
		
		res=vData->Dup();
		
	}

	_retTree = _t;
	return res;
}

BaseGDL**  GDLInterpreter::ref_parameter(ProgNodeP _t) {

	 BaseGDL** ret;
	
	if (_t != NULL)
	{
		if ( _t->getType() == DEREF) {
			ret=l_deref(_t);
			_t = _retTree;
		}
		else	
		//case VAR:
		//case VARPTR:
		{
			ret=l_simple_var(_t);
			_t = _retTree;
		}
	}
 	_retTree = _t;
	return ret;
}

BaseGDL* GDLInterpreter::unnamed_struct_def(ProgNodeP _t) {

	BaseGDL* res;

	// don't forget the struct in extrat.cpp if you change something here
	// "$" as first char in the name is necessary 
	// as it defines unnnamed structs (see dstructdesc.hpp)
	DStructDesc*   nStructDesc = new DStructDesc( "$truct");
	
	// instance takes care of nStructDesc since it is unnamed
	//     DStructGDL* instance = new DStructGDL( nStructDesc, dimension(1));
	DStructGDL* instance = new DStructGDL( nStructDesc);
	auto_ptr<DStructGDL> instance_guard(instance);
	
	ProgNodeP rTree = _t->getNextSibling();
	match(antlr::RefAST(_t),STRUC);
	_t = _t->getFirstChild();
	for (; _t != NULL;) {
			ProgNodeP si = _t;
			match(antlr::RefAST(_t),IDENTIFIER);
			_t = _t->getNextSibling();
			BaseGDL* e=expr(_t);
			_t = _retTree;
			
			// also adds to descriptor, grabs
			instance->NewTag( si->getText(), e); 
	}

	instance_guard.release();
	res=instance;
	
	_retTree = rTree;
	return res;
}

BaseGDL* GDLInterpreter::lib_function_call_retnew(ProgNodeP _t) { // *
	
	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	
	ProgNodeP rTree = _t->getNextSibling();

	match(antlr::RefAST(_t),FCALL_LIB_RETNEW);
	_t = _t->getFirstChild();
	ProgNodeP fl = _t;
	match(antlr::RefAST(_t),IDENTIFIER);
	_t = _t->getNextSibling();
	
	EnvT* newEnv=new EnvT( fl, fl->libFun);//libFunList[fl->funIx]);
	
	parameter_def(_t, newEnv);
	
	// push id.pro onto call stack
	callStack.push_back(newEnv);
	// make the call
	BaseGDL* res=static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv);
	//*** MUST always return a defined expression
	
	_retTree = rTree;
	return res;
}

BaseGDL*  GDLInterpreter::lib_function_call(ProgNodeP _t) {

	// better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
	StackGuard<EnvStackT> guard(callStack);
	
	ProgNodeP rTree = _t->getNextSibling();
	match(antlr::RefAST(_t),FCALL_LIB);

	_t = _t->getFirstChild();
	match(antlr::RefAST(_t),IDENTIFIER);

	ProgNodeP& fl = _t;
	EnvT* newEnv=new EnvT( fl, fl->libFun);//libFunList[fl->funIx]);
	
	parameter_def(_t->getNextSibling(), newEnv);
	
	// push id.pro onto call stack
	callStack.push_back(newEnv);
	// make the call
	BaseGDL* res=static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv);
	//*** MUST always return a defined expression
	
	_retTree = rTree;
	return res;
}

BaseGDL* GDLInterpreter::constant_nocopy(ProgNodeP _t) { // *

	BaseGDL* res = _t->cData; // no ->Dup(); 
		
	_retTree = _t->getNextSibling();
	return res;
}

BaseGDL* GDLInterpreter::struct_def(ProgNodeP _t) {
	BaseGDL* res;

	assert( _t != NULL);
	
	switch ( _t->getType()) {
	case NSTRUC:
	{
		return named_struct_def(_t);
	}
	case STRUC:
	{
		return unnamed_struct_def(_t);
	}
	case NSTRUC_REF:
	{
		ProgNodeP rTree = _t->getNextSibling();
		match(antlr::RefAST(_t),NSTRUC_REF);
		_t = _t->getFirstChild();
		ProgNodeP idRef = _t;
		match(antlr::RefAST(_t),IDENTIFIER);
		_t = _t->getNextSibling();
		
		// find struct 'id'
		// returns it or throws an exception
		DStructDesc* dStruct=GetStruct( idRef->getText(), _t);
		
		res=new DStructGDL( dStruct, dimension(1));
		
		_t = rTree;
		break;
	}
	}
	_retTree = _t;
	return res;
}
#endif
