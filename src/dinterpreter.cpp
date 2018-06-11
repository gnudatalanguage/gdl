/***************************************************************************
                       dinterpreter.cpp  -  main class which controls it all
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
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

#include <iostream>
#ifdef _MSC_VER
#include <io.h> // isatty, windows
#else
#include <unistd.h> // isatty, usleep
#endif

//#include <wordexp.h>

#include "dnodefactory.hpp"
#include "str.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"
#include "gdljournal.hpp"
#include "gdleventhandler.hpp"

#ifdef USE_MPI
#include "mpi.h"
#endif

#include <cassert>

// print out AST tree
//#define GDL_DEBUG
//#undef GDL_DEBUG

#ifdef GDL_DEBUG
#include "print_tree.hpp"
#endif

#if (__cplusplus >= 201103L || _MSC_VER >= 1800) && !defined(__MINGW32__)
#   include <thread> // C++11
#   define HAVE_CXX11THREAD
#else
#   include <pthread.h>
#endif

using namespace std;
using namespace antlr;

string inputstr;
bool historyIntialized = false;

// instantiation of static data
GDLInterpreter::HeapT     GDLInterpreter::heap; 
GDLInterpreter::ObjHeapT  GDLInterpreter::objHeap; 
SizeT                     GDLInterpreter::objHeapIx;
SizeT                     GDLInterpreter::heapIx;
EnvStackT             GDLInterpreter::callStack;
DLong                   GDLInterpreter::stepCount;

ProgNode GDLInterpreter::NULLProgNode;
ProgNodeP GDLInterpreter::NULLProgNodeP = &GDLInterpreter::NULLProgNode;

void LibInit(); // defined in libinit.cpp

DInterpreter::DInterpreter(): GDLInterpreter()
{
//  DataStackT::Init();

  //    heap.push_back(NULL); // init heap index 0 (used as NULL ptr)
  //    objHeap.push_back(NULL); // init heap index 0 (used as NULL ptr)
  interruptEnable = true;
  objHeapIx=1; // map version (0 is NULL ptr)
  heapIx=1;    // map version (0 is NULL ptr)
  returnValue  = NULL;
  returnValueL = NULL;

	stepCount = 0;
  
    // setup main level environment
  DPro* mainPro=new DPro();        // $MAIN$  NOT inserted into proList
  EnvUDT* mainEnv=new EnvUDT(NULL, mainPro);
  callStack.push_back(mainEnv);   // push main environment (necessary)
    
  assert( ProgNode::interpreter == NULL);
  GDLException::SetInterpreter( this);
  ProgNode::interpreter = this; // interface to expr( ProgNodeP)
  EnvT::interpreter = this; 
  BaseGDL::interpreter = this;

  //  tmpList.reserve(100);
}

// void SetActualCompileOpt( unsigned int cOpt)
// {
// if( BaseGDL::interpreter!=NULL && BaseGDL::interpreter->CallStack().size()>0) 
// 	BaseGDL::interpreter->CallStack().back()->SetCompileOpt( cOpt);
// }

// used in the statement function.
// runs a new instance of the interpreter if not
// at main level
RetCode GDLInterpreter::NewInterpreterInstance( SizeT lineOffset)
{
  if( callStack.size() <= 1 && lineOffset == 0) return RC_ABORT; // stay in main loop 
  
  assert( dynamic_cast<DInterpreter*>( this) != NULL);
  return static_cast<DInterpreter*>( this)->InnerInterpreterLoop(lineOffset);
}

DStructGDL* GDLInterpreter::ObjectStruct( DObjGDL* self, ProgNodeP mp)
{
//   DType selfType = self->Type();
//   if( selfType != GDL_OBJ) 
//     throw GDLException( mp, "Object reference type"
// 			" required in this context: "+Name(self));

  DObjGDL* obj=self;//static_cast<DObjGDL*>(self);

  SizeT o;
  if( !obj->Scalar( o))
    throw GDLException( mp, "Object reference"
			" must be scalar in this context: "+Name(self));
  
  if( o == 0)
    throw GDLException( mp, "Unable to invoke method"
			" on NULL object reference: "+Name(self));
  
  DStructGDL* oStructGDL;
  try {
    oStructGDL= GetObjHeap( o);
  }
  catch ( HeapException&)
    {
      throw GDLException( mp, "Object not valid: "+Name(self));
    }
  
  return oStructGDL;
}

void GDLInterpreter::SetRootL( ProgNodeP tt, DotAccessDescT* aD, BaseGDL* r, ArrayIndexListT* aL) 
{ 
  if( r->Type() == GDL_STRUCT)
  {
      if( r->IsAssoc())
	  {
	      ArrayIndexListGuard guard( aL);
	      throw GDLException( tt, "File expression not allowed in this context: "+
					Name(r),true,false);
	  }
      DStructGDL* structR=static_cast<DStructGDL*>(r);
      aD->ADRoot(structR, aL); 
  }
  else
  {
      if( r->Type() != GDL_OBJ)
	  {
	      throw GDLException( tt, "Expression must be a STRUCT in this context: "+
					Name(r),true,false);
	  }

      ArrayIndexListGuard guard( aL);

      DStructGDL* oStruct = ObjectStruct( static_cast<DObjGDL*>(r), tt);
      DStructDesc* desc = oStruct->Desc();

      bool isObj = callStack.back()->IsObject(); // called from member subroutine?

      if( desc->IsParent( GDL_OBJECT_NAME))
	  {
	    SizeT sss = 0;
	    SizeT ooo = 0;
	    if( isObj)
	    {
	      static_cast<DObjGDL*>(r)->Scalar( ooo); // checked in ObjectStruct

	      BaseGDL* self = callStack.back()->GetKW(callStack.back()->GetPro()->NKey()); // SELF

	      assert( dynamic_cast<DObjGDL*>(self) != NULL);

	      if( !static_cast<DObjGDL*>(self)->Scalar( sss))
		  throw GDLException( tt, "Internal error: SELF Object reference"
				    " must be scalar in this context: "+Name(self));

	      assert( sss != 0);
	    }

	    if( !isObj || (sss != ooo))
	    {
	      // call SetProperty
	      throw GDLException( tt, "Calling SetProperty not yet implemented: "+Name(r));
	      //return;
	    }
	  }

      if( isObj) // member access to object?
	  {
	      if( !desc->IsParent( callStack.back()->GetPro()->Object()))
		  {
		      throw GDLException( tt, "Object of type "+desc->Name()+
					  " is not accessible within "+
					  callStack.back()->GetProName() + 
					  ": "+Name(r));
		  }
	      // DStructGDL* oStruct = 
	      //        ObjectStructCheckAccess( static_cast<DObjGDL*>(r), tt);

	      // oStruct cannot be "Assoc_"
	      aD->ADRoot( oStruct, guard.release()); 
	  }
      else
	  {
	      throw GDLException( tt, "Expression must be a"
				  " STRUCT in this context: "+Name(r),
				  true,false);
	  }
  }
}

void GDLInterpreter::SetRootR( ProgNodeP tt, DotAccessDescT* aD, BaseGDL* r, ArrayIndexListT* aL) 
{ 
// check here for object and get struct
if( r->Type() == GDL_STRUCT)
  {
      if( r->IsAssoc())
	  {
	      ArrayIndexListGuard guard( aL);
	      throw GDLException( tt, "File expression not allowed in this context: "+
				       Name(r),true,false);
	  }
      DStructGDL* structR=static_cast<DStructGDL*>(r);
      aD->ADRoot( structR, aL); 
  }
else
  {
      ArrayIndexListGuard guard( aL);

      if( r->Type() != GDL_OBJ)
	  {
	      throw GDLException( tt, "Expression must be a"
				  " STRUCT in this context: "+Name(r),
				  true,false);
	  }

      DStructGDL* oStruct = ObjectStruct( static_cast<DObjGDL*>(r), tt);
      DStructDesc* desc = oStruct->Desc();

      bool isObj = callStack.back()->IsObject();

      if( desc->IsParent( GDL_OBJECT_NAME))
	  {
	    SizeT sss = 0;
	    SizeT ooo = 0;
	    if( isObj)
	    {
	      static_cast<DObjGDL*>(r)->Scalar( ooo); // checked in ObjectStruct

	      BaseGDL* self = callStack.back()->GetKW(callStack.back()->GetPro()->NKey()); // SELF

	      assert( dynamic_cast<DObjGDL*>(self) != NULL);

	      if( !static_cast<DObjGDL*>(self)->Scalar( sss))
		  throw GDLException( tt, "Internal error: SELF Object reference"
				    " must be scalar in this context: "+Name(self));

	      assert( sss != 0);
	    }

	    if( !isObj || (sss != ooo))
	    {
	      // call GetProperty
	      throw GDLException( tt, "Calling GetProperty not yet implemented: "+Name(r));

	      //aD->ADRootGetProperty( oStruct, guard.release()); 
	      return;
	    }
	  }

      if( isObj)
	  {
	      if( !desc->IsParent( callStack.back()->GetPro()->Object()))
		  {
		      throw GDLException( tt, "Object of type "+desc->Name()+
					  " is not accessible within "+
					  callStack.back()->GetProName() + 
					  ": "+Name(r));
		  }
	      // DStructGDL* oStruct = 
	      //     ObjectStructCheckAccess( static_cast<DObjGDL*>(r), tt);

	      if( aD->IsOwner()) delete r; 
	      aD->SetOwner( false); // object struct, not owned
	      
	      aD->ADRoot( oStruct, guard.release()); 
	  }
      else
	  {
	      throw GDLException( tt, "Expression must be a"
				  " STRUCT in this context: "+Name(r),true,false);
	  }
  }
}

// DStructDesc* GDLInterpreter::GDLObjectDesc( DStructGDL* oStruct, ProgNodeP mp)
// {
//   //DStructGDL* oStruct = ObjectStruct( self, mp);
//   
//   // check accessibility
//   DStructDesc* desc = oStruct->Desc();
//   if( !desc->IsParent( GDL_OBJECT_NAME))
//     {
//       return NULL;
//     }
//   
//   return desc;
// }
// 
// void GDLInterpreter::ObjectStructCheckAccess( DStructGDL* oStruct, ProgNodeP mp)
// {
//   //DStructGDL* oStruct = ObjectStruct( self, mp);
//   
//   // check accessibility
//   DStructDesc* desc = oStruct->Desc();
//   if( !desc->IsParent( callStack.back()->GetPro()->Object()))
//     {
//       throw GDLException( mp, "Object of type "+desc->Name()+
// 			  " is not accessible within "+
// 			  callStack.back()->GetProName() + ": "+Name(self));
//     }
//   
//   //return oStruct;
// }

// searches and compiles procedure (searchForPro == true) or function (searchForPro == false)  'pro'
bool GDLInterpreter::SearchCompilePro(const string& pro, bool searchForPro) 
{
  static StrArr openFiles;

  string proFile=StrLowCase(pro)+".pro";
  //AppendIfNeeded( proFile, ".pro");

  bool found=CompleteFileName(proFile);
  if( !found) return false;
  
  // file already opened?
  for( StrArr::iterator i=openFiles.begin(); i != openFiles.end(); ++i)
    {
      if( proFile == *i) return false;
    }

  StackSizeGuard<StrArr> guard( openFiles);

  // append file to list
  openFiles.push_back(proFile);

  return CompileFile( proFile, pro, searchForPro); // this might trigger recursion
}

// returns the struct descriptor with name 'name'
// read/compiles 'name'__define.pro if necessary
// cN is the calling node, passed for (runtime) debug information
DStructDesc* GDLInterpreter::GetStruct(const string& name, ProgNodeP cN)
{                   
  // find struct 'id'
  DStructDesc* dStruct=FindInStructList( structList, name);

  // member function/pro declaration inserts an empty DStructDesc
  if( dStruct != NULL && dStruct->NTags() > 0) return dStruct;

  static StrArr getStructList;
  
  // read/compile of IDENTIFIER__define.pro
  string proName=name+"__DEFINE";

  for( StrArr::iterator i=getStructList.begin(); i != getStructList.end(); ++i)
    {
      if( proName == *i) 
		throw GDLException(cN, "Structure type not defined (recursive call): "+name,true,false);
    }

  StackSizeGuard<StrArr> guardStructList( getStructList);

  // append file to list
  getStructList.push_back(proName);

//   if( Called( proName))
//     {
//       throw GDLException(cN, "Structure type not defined (recursive call): "+name);
//     }
  
  /*bool found=*/ SearchCompilePro(proName, true);

  // if an exception occurs in SearchCompilePro, the struct is not compiled
            
  int proIx=ProIx(proName);
  if( proIx == -1)
    {
	throw GDLException(cN, "Procedure not found: "+proName, true, false);
    }
  
  // 'guard' call stack
  StackGuard<EnvStackT> guard(callStack);

  // interpret it
  EnvUDT* newEnv=new EnvUDT( cN, proList[proIx]);

  // push id.pro onto call stack
  callStack.push_back(newEnv);
  
  // make the call
  call_pro(static_cast<DSubUD*>(callStack.back()->GetPro())->GetTree());
  
  dStruct=FindInStructList( structList, name);
  if( dStruct == NULL)
    {
      throw GDLException(cN, "Structure type not defined: "+name,true,false);
    }
     
  return dStruct;
}

void GDLInterpreter::SetFunIx( ProgNodeP f)
{
  if( f->funIx == -1)
    f->funIx=GetFunIx(f);
}

int GDLInterpreter::GetFunIx( ProgNodeP f)
{
  string subName = f->getText();
  int funIx=FunIx(subName);
  if( funIx == -1)
    {
      // trigger reading/compiling of source file
      /*bool found=*/ SearchCompilePro(subName, false);
            
      funIx=FunIx(subName);
      if( funIx == -1)
	{
	  throw GDLException(f, "Function not found: "+subName, true, false);
	}
    }
  return funIx;
}
int GDLInterpreter::GetFunIx( const string& subName)
{
  int funIx=FunIx(subName);
  if( funIx == -1)
    {
      // trigger reading/compiling of source file
      /*bool found=*/ SearchCompilePro(subName, false);
            
      funIx=FunIx(subName);
      if( funIx == -1)
	{
	  throw GDLException("Function not found: "+subName);
	}
    }
  return funIx;
}

void GDLInterpreter::SetProIx( ProgNodeP f)
{
  if( f->proIx == -1)
    f->proIx=GetProIx(f);//->getText());
}

int GDLInterpreter::GetProIx( ProgNodeP f)
{
  string subName = f->getText();
  int proIx=ProIx(subName);
  if( proIx == -1)
    {
      // trigger reading/compiling of source file
      /*bool found=*/ SearchCompilePro(subName, true);
	  
      proIx=ProIx(subName);
#ifndef AUTO_PRINT_EXPR
      if( proIx == -1)
	{
	  throw GDLException(f,"Procedure not found: "+subName,true,false);
	}
#endif
    }
  return proIx;
}
int GDLInterpreter::GetProIx( const string& subName)
{
  int proIx=ProIx(subName);
  if( proIx == -1)
    {
      // trigger reading/compiling of source file
      /*bool found=*/ SearchCompilePro(subName, true);
	  
      proIx=ProIx(subName);
      if( proIx == -1)
	{
	  throw GDLException("Procedure not found: "+subName);
	}
    }
  return proIx;
}

// converts inferior type to superior type
void GDLInterpreter::AdjustTypes(BaseGDL* &a, BaseGDL* &b)
{
  DType aTy=a->Type();
  DType bTy=b->Type();
  if( aTy == bTy) return;
  if( DTypeOrder[aTy] > 100 || DTypeOrder[bTy] > 100)
    {
      //exception
      throw GDLException( "Expressions of this type cannot be converted.");
    }
  if( DTypeOrder[aTy] > DTypeOrder[bTy])
    {
      // convert b to a
      b=b->Convert2( aTy);
    }
  else
    {
      // convert a to b
      a=a->Convert2( bTy);
    }
}

void GDLInterpreter::ReportCompileError( GDLException& e, const string& file)
{
  cout << flush;
  cerr << SysVar::MsgPrefix() << e.toString() << endl;
  if( file != "")
    {
      cerr << "  At: " << file;
      SizeT line = e.getLine();
      if( line != 0)
	{       
	  cerr  << ", Line " << line;
	  SizeT col = e.getColumn();
	  if( col != 0)
	    cerr << "  Column " << e.getColumn();
	}
      cerr << endl;
    }
}

// compiles file, returns success
// if untilPro is set to "" the whole file is compiled
// procedure (searchForPro == true (default)) or function (searchForPro == false)
bool GDLInterpreter::CompileFile(const string& f, const string& untilPro, bool searchForPro) 
{
  ifstream in(f.c_str());
  if( !in) return false; // maybe throw exception here
  
  RefDNode theAST;
  try {  
    GDLLexer   lexer(in, f, GDLParser::NONE, untilPro, searchForPro);
    GDLParser& parser=lexer.Parser();
    
    // parsing
    parser.translation_unit();
    
    theAST=parser.getAST();
    
    if( !theAST)
      {
	cout << "No parser output generated." << endl;
	return false;
      }	
  }
  catch( GDLException& e)
    {
      ReportCompileError( e, f);
      return false;
    }
  catch( ANTLRException& e)
    {
      cerr << "Lexer/Parser exception: " <<  e.getMessage() << endl;
      return false;
    }

#ifdef GDL_DEBUG
  cout << "Parser output:" << endl;
  antlr::print_tree pt;
  pt.pr_tree(static_cast<antlr::RefAST>(theAST));
  cout << "CompileFile: Parser end." << endl;
#endif
  
#ifdef GDL_DEBUG
  RefDNode trAST;
#endif

  GDLTreeParser treeParser( f, untilPro);
  try
    {
      treeParser.translation_unit(theAST);

// #ifdef GDL_DEBUG
// nothing is returned (pro/funList are changed)
//       trAST=treeParser.getAST();
// #endif

      if( treeParser.ActiveProCompiled()) RetAll();
    }
  catch( GDLException& e)
    {
      ReportCompileError( e, f);
      if( treeParser.ActiveProCompiled()) RetAll();
      return false;
    }
  catch( ANTLRException& e)
    {
      cerr << "Compiler exception: " <<  e.getMessage() << endl;
      if( treeParser.ActiveProCompiled()) RetAll();
      return false;
    }
// #ifdef GDL_DEBUG
//   cout << "Tree parser output:" << endl;
//   pt.pr_tree(static_cast<antlr::RefAST>(trAST));
//   cout << "ExecuteLine: Tree parser end." << endl;
// #endif
/*#ifdef GDL_DEBUG
      cout << "Tree parser output:" << endl;
      antlr::print_tree ptTP;
      ptTP.pr_tree(static_cast<antlr::RefAST>(trAST));
      cout << "CompileFile: Tree parser end." << endl;
#endif*/
  
  return true;
}      

void AppendExtension( string& argstr)
{
  SizeT slPos = argstr.find_last_of( '/');
  SizeT dotPos = argstr.find_last_of( '.');

  if( dotPos == string::npos || (slPos != string::npos && slPos > dotPos))
    //  if( argstr.length() <= 4 || argstr.find( '.', 1) == string::npos) 
    //	      StrLowCase( argstr.substr(argstr.length()-4,4)) != ".pro")
    {
      argstr += ".pro";
    }
}

DInterpreter::CommandCode DInterpreter::CmdReset()
{
  RetAll( RetAllException::RESET);
  return CC_OK;
}
DInterpreter::CommandCode DInterpreter::CmdFullReset()
{
  RetAll( RetAllException::FULL_RESET);
  return CC_OK;
}

DInterpreter::CommandCode DInterpreter::CmdCompile( const string& command)
{
  string cmdstr = command;
  size_t sppos = cmdstr.find(" ",0);
  if (sppos == string::npos) 
    {
      cout << "Interactive COMPILE not implemented yet." << endl;
      return CC_OK;
    }
      
  bool retAll = false; // Remember if Retall is needed

  // Parse each file name
  size_t pos = sppos + 1;
  while (pos < command.length()) 
    {
      sppos = command.find(" ",pos);
      if (sppos == string::npos) sppos = command.length();

      // Found a file
      if ((sppos - pos) > 0) 
	{
	  string argstr  = command.substr(pos, sppos-pos);
	  string origstr = argstr;

	  // try first with extension
	  AppendExtension( argstr);
	  bool found = CompleteFileName( argstr);

	  // 2nd try without extension
	  if( !found)
	    {
	      argstr = origstr;
	      found = CompleteFileName( argstr);
	    }

	  if (found) 
	    {
	      try {
		// default is more verbose
		CompileFile( argstr); //, origstr); 
	      }
	      catch( RetAllException&)
		{
		  // delay the RetAllException until finished
		  retAll = true;
		}
	    } 
	  else 
	    {
	      Message( "Error opening file. File: "+origstr+".");
	      return CC_OK;
	    }
	}
      pos = sppos + 1;
    }
  if( retAll) RetAll();

  return CC_OK;
}

DInterpreter::CommandCode DInterpreter::CmdRun( const string& command)
{
  string cmdstr = command;
  size_t sppos = cmdstr.find(" ",0);
  if (sppos == string::npos) 
    {
      cout << "Interactive RUN not implemented yet." << endl;
      return CC_OK;
    }
      
  bool retAll = false; // Remember if Retall is needed

  // Parse each file name
  size_t pos = sppos + 1;
  while (pos < command.length()) 
    {
      sppos = command.find(" ",pos);
      size_t spposComma = command.find(",",pos);
      if (sppos == string::npos && spposComma == string::npos) 
	sppos = command.length();
      else if (sppos == string::npos)
	sppos = spposComma;
	

      // Found a file
      if ((sppos - pos) > 0) 
	{
	  string argstr  = command.substr(pos, sppos-pos);
	  string origstr = argstr;

	  // try 1st with extension
	  AppendExtension( argstr);
	  bool found = CompleteFileName(argstr);

	  // 2nd try without extension
	  if( !found)
	    {
	      argstr = origstr;
	      found = CompleteFileName( argstr);
	    }

	  if (found) 
	    {
	      try {
		// default is more verbose
		CompileFile( argstr); //, origstr); 
	      }
	      catch( RetAllException&)
		{
		  // delay the RetAllException until finished
		  retAll = true;
		}
	    } 
	  else 
	    {
	      Message( "Error opening file. File: "+origstr+".");
	      return CC_OK;
	    }
	}
      pos = sppos + 1;
    }
  if( retAll) 
    Warning( "Compiled a main program while inside a procedure. "
	     "Returning.");

  // actual run is perfomed in InterpreterLoop()
  RetAll( RetAllException::RUN); // throws (always)
  return CC_OK; //avoid warnings
}

// execute GDL command (.run, .step, ...)
DInterpreter::CommandCode DInterpreter::ExecuteCommand(const string& command)
{
  string cmdstr = command;
  string args;
  size_t sppos = cmdstr.find(" ",0);
  if (sppos != string::npos) {
    args = cmdstr.substr(sppos+1);
    cmdstr = cmdstr.substr(0, sppos);
  }
    
  //   cout << "Execute command: " << command << endl;

  String_abbref_eq cmd( StrUpCase( cmdstr));

  // AC: Continue before Compile to have ".c" giving ".continue"
  if( cmd( "CONTINUE"))
    {
      return CC_CONTINUE;
    }
  if( cmd( "COMPILE"))
    {
      return CmdCompile( command);
    }
  if( cmd( "EDIT"))
    {
      cout << "Can't edit file without running GDLDE." << endl;
      return CC_OK;
    }
  if( cmd( "FULL_RESET_SESSION"))
    {
      return CmdFullReset();
    }
  if( cmd( "GO"))
    {
      cout << "GO not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "OUT"))
    {
      cout << "OUT not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "RUN"))
    {
      return CmdRun( command);
    }
  if( cmd( "RETURN"))
    {
      cout << "RETURN not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "RESET_SESSION"))
    {
      return CmdReset();
    }
  if( cmd( "RNEW"))
    {
	    EnvUDT* mainEnv = 
		   static_cast<EnvUDT*>(GDLInterpreter::callStack[0]);
			  SizeT nEnv = mainEnv->EnvSize();

		dynamic_cast<DSubUD*>(mainEnv->GetPro())->Reset();
		if(!mainEnv->Removeall()) 
			cout << " Danger ! Danger! Unexpected result. Please exit asap & report" <<endl;

      return CmdRun( command);
    }
  // GD:Here to have ".s" giving ".step"
  if( cmd( "STEP"))
    {
      DLong sCount;
      if( args == "")
      {
	  sCount = 1;
      }
      else
      {
	  const char* cStart=args.c_str();
	  char* cEnd;
	  sCount = strtol(cStart,&cEnd,10);
	  if( cEnd == cStart)
	  {
	    cout << "Type conversion error: Unable to convert given STRING: '"+args+"' to LONG." << endl;
	    return CC_OK;
	  }
	}
	stepCount = sCount;
	debugMode = DEBUG_STEP;
	return CC_STEP;
    }

  if( cmd( "SKIP"))
    {
      DLong sCount;
      if( args == "")
      {
	  sCount = 1;
      }
      else
      {
	const char* cStart=args.c_str();
	char* cEnd;
	sCount = strtol(cStart,&cEnd,10);
	if( cEnd == cStart)
	{
	  cout << "Type conversion error: Unable to convert given STRING: '"+args+"' to LONG." << endl;
	  return CC_OK;
	}
      }
      stepCount = sCount;
      return CC_SKIP;
    }
  if( cmd( "STEPOVER"))
    {
      cout << "STEPOVER not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "SIZE"))
    {
      cout << "SIZE not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "TRACE"))
    {
      cout << "TRACE not implemented yet." << endl;
      return CC_OK;
    }
  cout << SysVar::MsgPrefix() << 
    "Unknown command: "<< command << endl;
  return CC_OK; // get rid of warning
}

// execute OS shell command (interactive shell if command == "")
// by Peter Messmer
void DInterpreter::ExecuteShellCommand(const string& command)
{
  string commandLine = command;
  if(commandLine == "") {
     char* shellEnv = getenv("SHELL");
	 if (shellEnv == NULL) shellEnv = getenv("COMSPEC");
	 if (shellEnv == NULL) {
        cout << "Error managing child process. " <<
		" Environment variable SHELL or COMSPEC not set." << endl;
      return;
    }
	 commandLine = shellEnv;
  }

  int ignored = system( commandLine.c_str());
}



string GetLine( istream* in)
{
  string line = "";
  while( in->good() && 
    (  line == "" 
    || line[0] == ';' )) // skip also comment lines (bug #663) 
    {
      getline( *in, line);
      StrTrim(line);
    }
  return line;
}

void AddLineOffset( SizeT lineOffset, RefDNode astR)
{
astR->SetLine( astR->getLine() + lineOffset);
if( astR->getFirstChild() != NULL) AddLineOffset( lineOffset, (RefDNode)astR->getFirstChild() );
if( astR->getNextSibling() != NULL) AddLineOffset( lineOffset, (RefDNode)astR->getNextSibling() );
}

// execute one line of code (commands and statements)
DInterpreter::CommandCode DInterpreter::ExecuteLine( istream* in, SizeT lineOffset)
{
  string line = (in != NULL) ? ::GetLine(in) : GetLine();

  // cout << "ExecuteLine: " << line << endl;

  string firstChar = line.substr(0,1);

  // command
  if( firstChar == ".") 
    {
      return ExecuteCommand( line.substr(1));
    }

  //  online help (if possible, start a browser)
  if( firstChar == "?") 
    {
      // later, we will have to check whether we have X11/Display or not
      // on some computing nodes on supercomputers, this is de-activated.
      if (line.substr(1).length() > 0) {
	line=line.substr(1);
	StrTrim(line);
	line="online_help, '"+line+"'"; //'
      } else {
	line="online_help";
      }
    }
  
  // shell command
  if( firstChar == "#") 
    {
      if (line.substr(1).length() > 0) {
	line=line.substr(1);
	StrTrim(line);
	line=StrUpCase(line);
	//cout << "yes ! >>"<<StrUpCase(line)<<"<<" << endl;
	SizeT nProFun;
	int nbFound=0;
	// looking in internal procedures
	nProFun=libProList.size();
	for( SizeT i = 0; i<nProFun; ++i)
	  {
	    if (line.compare(libProList[ i]->Name()) == 0) {
	      cout << "Internal PROCEDURE : " << libProList[ i]->ToString() << endl;
	      nbFound++;
	      break;
	    }
	  }
	// looking in internal functions
	nProFun = libFunList.size();
	for( SizeT i = 0; i<nProFun; ++i)
	  {
	    if (line.compare(libFunList[ i]->Name()) == 0) {
	      cout << "Internal FUNCTION : " << libFunList[ i]->ToString() << endl;
	      nbFound++;
	      break;
	    }
	  }
	// looking in compiled functions
	nProFun = funList.size();
	for( SizeT i = 0; i<nProFun; ++i)
	  {
	    if (line.compare(funList[ i]->Name()) == 0) {
	      cout << "Compiled FUNCTION : " << funList[ i]->ToString() << endl;
	      nbFound++;
	      break;
	    }
	  }
	// looking in compiled procedures
	nProFun = proList.size();
	for( SizeT i = 0; i<nProFun; ++i)
	  {
	    if (line.compare(proList[ i]->Name()) == 0) {
	      cout << "Compiled PROCEDURE : " << proList[ i]->ToString() << endl;
	      nbFound++;
	      break;
	    }
	  }
	if (nbFound == 0) {
	  cout << "No Procedure/Function, internal or compiled, with name : "<< line << endl;
	}
      } else {
	cout << "Please provide a pro/fun name !" << endl;
      }
      return CC_OK;
    }

  // shell command
  if( firstChar == "$") 
    {
      ExecuteShellCommand( line.substr(1));
      return CC_OK;
    }

  // include (only when at $MAIN$)
  // during compilation this is handled by the interpreter
  if( firstChar == "@" && callStack.size() <= 1) 
    {
      string fileRaw = line.substr(1);
      StrTrim( fileRaw);

      string file = fileRaw;
      AppendExtension( file);
      
      bool found = CompleteFileName( file);
      if( !found)
	{
	  file = fileRaw;
	  CompleteFileName( file);
	}

      ExecuteFile( file);
      return CC_OK;
    }

  // statement -> execute it
  executeLine.clear(); // clear EOF (for executeLine)
  executeLine.str( line + "\n"); // append new line

  RefDNode theAST;
  try { 
    Guard<GDLLexer> lexer;

    // LineContinuation LC
    // conactenate the strings and insert \n
    // the resulting string can be fed to the lexer
   
    // print if expr parse ok 
    int lCNum = 0;
    for(;;) 
      {
	lexer.Reset( new GDLLexer(executeLine, "", callStack.back()->CompileOpt()));
	try {
	  // works, but ugly -> depends from parser detecting an error
	  // (which it always will due to missing END_U token in case of LC)
 	  //lexer->Parser().SetCompileOpt(callStack.back()->CompileOpt());
 	  lexer.Get()->Parser().interactive();
	  break; // no error -> everything ok
	}
	catch( GDLException& e)
	  {
	    int lCNew = lexer.Get()->LineContinuation();
	    if( lCNew == lCNum)
// 	      throw; // no LC -> real error
	{
#ifdef 	AUTO_PRINT_EXPR
#ifndef GDL_DEBUG 		
 		try {
// 			executeLine.clear(); // clear EOF (for executeLine)
// 			lexer.reset( new GDLLexer(executeLine, "", callStack.back()->CompileOpt()));
// 			lexer->Parser().expr();
	
			executeLine.clear(); // clear EOF (for executeLine)
			executeLine.str( "print,/implied_print," + executeLine.str()); // append new line
			
			lexer.reset( new GDLLexer(executeLine, "", callStack.back()->CompileOpt()));
			lexer->Parser().interactive();
			
			break; // no error -> everything ok
		}
		catch( GDLException& e2)
#endif
#endif
		{
			throw e;
		}
	}

	    lCNum = lCNew; // save number to see if next line also has LC
	  }



	// line continuation -> get next line
	if( in != NULL && !in->good())
	  throw GDLException( "End of file encountered during line continuation.");
	
	string cLine = (in != NULL) ? ::GetLine(in) : GetLine();

	executeLine.clear(); // clear EOF (for executeLine)
	executeLine.str( executeLine.str() + cLine + "\n"); // append new line
      } 
    
    //    lexer->Parser().interactive();
    theAST = lexer.Get()->Parser().getAST();

  }
  catch( GDLException& e)
    {
      ReportCompileError( e);
      return CC_OK;
    }
  catch( ANTLRException& e)
    {
      cerr << "Lexer/Parser exception: " <<  e.getMessage() << endl;
      return CC_OK;
    }

  if( theAST == NULL) return CC_OK;

    // consider line offset
    if( lineOffset > 0)
		AddLineOffset( lineOffset, theAST);

#ifdef GDL_DEBUG
  antlr::print_tree pt;
  cout << "Parser output:" << endl;
  pt.pr_tree(static_cast<antlr::RefAST>(theAST));
  cout << "ExecuteLine: Parser end." << endl;
#endif

  ProgNodeP progAST = NULL;

  RefDNode trAST;
	
  assert( dynamic_cast<EnvUDT*>(callStack.back()) != NULL);
  EnvUDT* env = static_cast<EnvUDT*>(callStack.back());
  int nForLoopsIn = env->NForLoops();
  try
    {
      GDLTreeParser treeParser( callStack.back());
	  
      treeParser.interactive(theAST);

      trAST=treeParser.getAST();

  if( trAST == NULL)
    {
      // normal condition for cmd line procedure calls
      return CC_OK;
    }	

#ifdef GDL_DEBUG
  cout << "Tree parser output (RefDNode):" << endl;
  pt.pr_tree(static_cast<antlr::RefAST>(trAST));
  cout << "ExecuteLine: Tree parser end." << endl;
#endif

	// **************************************
	// this is the call of the ProgNode factory
	// **************************************
    progAST = ProgNode::NewProgNode( trAST);

	assert( dynamic_cast<EnvUDT*>(callStack.back()) != NULL);
    EnvUDT* env = static_cast<EnvUDT*>(callStack.back());
    int nForLoops = ProgNode::NumberForLoops( progAST, nForLoopsIn);
	env->ResizeForLoops( nForLoops);
    }
  catch( GDLException& e)
    {
	  env->ResizeForLoops( nForLoopsIn);
      
      ReportCompileError( e);
      return CC_OK;
    }
  catch( ANTLRException& e)
    {
	  env->ResizeForLoops( nForLoopsIn);
      
      cerr << "Compiler exception: " <<  e.getMessage() << endl;
      return CC_OK;
    }
  Guard< ProgNode> progAST_guard( progAST);

  try
    {
      
#ifdef GDL_DEBUG
  cout << "Converted tree (ProgNode):" << endl;
  pt.pr_tree( progAST);
  cout << "end." << endl;
#endif

      RetCode retCode = interactive( progAST);
      
	  env->ResizeForLoops( nForLoopsIn);
      
      // write to journal file
      string actualLine = GetClearActualLine();
      if( actualLine != "") lib::write_journal( actualLine); 
  
      if( retCode == RC_RETURN) return CC_RETURN;
      return CC_OK;
    }
  catch( GDLException& e)
    {
	  env->ResizeForLoops( nForLoopsIn);
      
      cerr << "Unhandled GDL exception: " <<  e.toString() << endl;
      return CC_OK;
    }
  catch( ANTLRException& e)
    {
	  env->ResizeForLoops( nForLoopsIn);
      
      cerr << "Interpreter exception: " <<  e.getMessage() << endl;
      return CC_OK;
    }

  return CC_OK;
}

#ifdef HAVE_CXX11THREAD
void inputThread() {
#else
void *inputThread(void*) {
#endif
    while (1) {
      // patch by Ole, 2017-01-06
      //char ch = getchar(); if (ch==EOF) return NULL;
      char ch = getchar();
      if (ch==EOF) {
#ifdef HAVE_CXX11THREAD
	return;
#else
	return NULL;
#endif
      }        
      inputstr += ch;
      if (ch == '\n')
	break;
    }
#ifndef HAVE_CXX11THREAD
    return NULL;
#endif
}

// if readline is not available or !EDIT_INPUT set to zero
char* DInterpreter::NoReadline( const string& prompt)
{
  if (isatty(0)) cout << prompt << flush;
  if( feof(stdin)) return NULL;

#ifdef HAVE_CXX11THREAD
  thread th(inputThread);
#else
  pthread_t th;
  pthread_create(&th, NULL, inputThread, NULL);
#endif

  for (;;)
    {
        GDLEventHandler();
        if (inputstr.size() && inputstr[inputstr.size() - 1] == '\n') break;
        if (feof(stdin)) return NULL;
#ifdef WIN32
        Sleep(10);
#else
        usleep(10);
#endif
    }
  inputstr = inputstr.substr(0, inputstr.size() - 1); // removes '\n'
  //if (inputstr[inputstr.size() - 1] == '\r')
  //    inputstr = inputstr.substr(0, inputstr.size() - 1); // removes '\r' too, if exists
  char *result = (char*)malloc((inputstr.length() + 1) * sizeof(char));
  strcpy(result, inputstr.c_str()); // copies including terminating '\0'
  inputstr.clear();

#ifdef HAVE_CXX11THREAD
  th.join();
#else
  pthread_join(th, NULL);
#endif

  return result;
}

bool  lineEdit = false;
string actualPrompt;

void ControlCHandler(int)
{
  cout << SysVar::MsgPrefix() << "Interrupt encountered." << endl;
  if( lineEdit) cout << actualPrompt /*SysVar::Prompt()*/ /*.c_str()*/ << flush;
  sigControlC = true;
  signal(SIGINT,ControlCHandler);
}

string DInterpreter::GetLine()
{
  clog << flush; cout << flush;

#if defined(HAVE_LIBREADLINE) || defined(HAVE_LIBEDITLINE)
  int edit_input = SysVar::Edit_Input() && isatty(0);
#endif

  string line;
  do {

    char *cline;

	actualPrompt = SysVar::Prompt();

    lineEdit = true;

#if defined(HAVE_LIBREADLINE) || defined(HAVE_LIBEDITLINE)
    
    if( edit_input != 0)
      cline = readline(const_cast<char*>(actualPrompt.c_str()));
    else
      cline = NoReadline(actualPrompt);
#else
    
    cline = NoReadline(actualPrompt);
#endif
    
    lineEdit = false;
    sigControlC = false; // reset all control-c which occured during line editing
    
    if( !cline) 
      {
	if (isatty(0)) cout << endl;
	// instead or going out (EXITing) immediately, we go to
	// the "exitgdl" in order to save the history
	// exit( EXIT_SUCCESS); //break; // readline encountered eof
	line="EXIT";
	StrTrim(line);
	break;
      }
    else
    // make a string
    line = cline;
    
#ifndef _MSC_VER
    free(cline);        // done here for compatibility with readline
#endif
  
    StrTrim(line);
  } while( line == "" 
	|| line[0] == ';'); // skip also comment lines (bug #663)
  
#if defined(HAVE_LIBREADLINE) || defined(HAVE_LIBEDITLINE)
  // SA: commented out to comply with IDL behaviour- allowing to 
  //     set the history-file length only in the startup file
  //if( edit_input > 20)
  //  {
  //    stifle_history( edit_input);
  //  }

static string lastAdded;
  
  // we would not like to add the current command if is "EXIT" !!
  if ( StrUpCase(line) != "EXIT" && line != lastAdded) {
    // const_cast to make it work with older readline versions
    add_history(const_cast<char*>(line.c_str()));

    lastAdded = line;
  }

/*
cout << "################ " << history_length << endl;
for( int h=0;h<history_length; ++h)
{
HIST_ENTRY *lH = history_get (h);
if( lH != NULL)
	cout << h << ": " << string(lH->line) << endl;
else
	cout << h << ": NULL" << endl;
}
*/
#endif
  
  return line;
}

// reads user input and executes it
// inner loop (called via Control-C, STOP, error)
RetCode DInterpreter::InnerInterpreterLoop(SizeT lineOffset)
{
  ProgNodeP retTreeSave = _retTree;
  for (;;) {
#if defined (_MSC_VER) && _MSC_VER < 1800
	_clearfp();
#else
    feclearexcept(FE_ALL_EXCEPT);
#endif

    DInterpreter::CommandCode ret=ExecuteLine(NULL, lineOffset);

    _retTree = retTreeSave; // on return, _retTree should be kept

    if( ret == CC_SKIP)
    {
      for( int s=0; s<stepCount; ++s)
      {
	if( _retTree == NULL)
	  break;
	
	_retTree = _retTree->getNextSibling();
      }
//       cout << ".SKIP " << stepCount << "   " << _retTree << endl;

      stepCount = 0;
      retTreeSave = _retTree;
      // we stay at the command line here
      if( _retTree == NULL)
	Message( "Can't continue from this point.");
      else
	DebugMsg( _retTree, "Skipped to: ");
    }
    else if( ret == CC_RETURN) return RC_RETURN;
    else if( ret == CC_CONTINUE) return RC_OK; 
    else if( ret == CC_STEP) return RC_OK;
  }
}

// used by pyhton module
bool DInterpreter::RunBatch( istream* in)
{
  ValueGuard<bool> guard( interruptEnable);
  interruptEnable = false;

  while( in->good())
    {
#if defined (_MSC_VER) && _MSC_VER < 1800
	  _clearfp();
#else
      feclearexcept(FE_ALL_EXCEPT);
#endif
      
      try
	{
	  DInterpreter::CommandCode ret=ExecuteLine( in);
	      
	  if( debugMode != DEBUG_CLEAR)
	    {
	      debugMode = DEBUG_CLEAR;
	      return false;
	    }
	}
      catch( RetAllException& retAllEx)
	{
	}
      catch( exception& e)
	{
	  cerr << "Batch" << ": Exception: " << e.what() << endl;
	}
      catch (...)
	{	
	  cerr << "Batch" << ": Unhandled Error." << endl;
	}
    } // while

  return true;
}

// used for @file
// Note: As long as we are in batch mode we are at $MAIN$
void DInterpreter::ExecuteFile( const string& file)
{
  ifstream in(file.c_str());
  
  if( in.fail())
    Warning( "Error opening file: "+file);

  //  ValueGuard<bool> guard( interruptEnable);
  //  interruptEnable = false;

  bool runCmd = false;
  while( in.good())
    {
#if defined (_MSC_VER) && _MSC_VER < 1800
	  _clearfp();
#else
      feclearexcept(FE_ALL_EXCEPT);
#endif 

      try
 	{
	  if( runCmd)
	    {
	      runCmd = false;
	      RunDelTree();
	    }
	  else
	    {		  
	      DInterpreter::CommandCode ret=ExecuteLine( &in);
	      
	      if( debugMode != DEBUG_CLEAR)
		{
		  debugMode = DEBUG_CLEAR;
		  // Warning( "Prematurely closing batch file: "+startup);
		  break;
		}
	    }
	}
      catch( RetAllException& retAllEx)
	{
	  runCmd = (retAllEx.Code() == RetAllException::RUN);
	  if( !runCmd) throw;
	}
      //       catch( exception& e)
      // 	{
      // 	  cerr << file << ": Exception: " << e.what() << endl;
      // 	}
      //       catch (...)
      // 	{	
      // 	  cerr << file << ": Unhandled Error." << endl;
      // 	}
    } // while
}

// this must be run only from $MAIN$
void DInterpreter::RunDelTree()
{
  if( static_cast<DSubUD*>
      (callStack.back()->GetPro())->GetTree() != NULL)
    {
      try
	{
	  call_pro(static_cast<DSubUD*>
		   (callStack.back()->GetPro())->GetTree());

	  static_cast<DSubUD*>
	    (callStack.back()->GetPro())->DelTree();
	}
      catch( RetAllException&)
	{
	  static_cast<DSubUD*>
	    (callStack.back()->GetPro())->DelTree();
	  throw;
	}
    }
}


// reads user input and executes it
// the main loop

RetCode DInterpreter::InterpreterLoop(const string& startup,
  vector<string>& batch_files, const std::string& statement) {
  // process startup file
  if (startup != "") {
    ifstream in(startup.c_str());

    if (in.fail())
      Warning("Error opening startup file: " + startup);

    ValueGuard<bool> guard(interruptEnable);
    interruptEnable = false;

    bool runCmd = false;
    try {
      while (in.good()) {
#if defined (_MSC_VER) && _MSC_VER < 1800
        _clearfp();
#else
        feclearexcept(FE_ALL_EXCEPT);
#endif

        try {
          if (runCmd) {
            runCmd = false;
            RunDelTree();
          } else {
            DInterpreter::CommandCode ret = ExecuteLine(&in);

            if (debugMode != DEBUG_CLEAR) {
              debugMode = DEBUG_CLEAR;
              Warning("Prematurely closing batch file: " + startup);
              break;
            }
          }
        } catch (RetAllException& retAllEx) {
          runCmd = (retAllEx.Code() == RetAllException::RUN);
          if (!runCmd) throw;
        } catch (exception& e) {
          cerr << startup << ": Exception: " << e.what() << endl;
        } catch (...) {
          cerr << startup << ": Unhandled Error." << endl;
        }
      } // while
    } catch (RetAllException& retAllEx) {
    }
  } // if( startup...

#ifdef USE_MPI
  int myrank = 0;
  int tag = 0;
  char mpi_procedure[256];
  MPI_Status status;
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (size > 1) {
    MPI_Recv(mpi_procedure, 256, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);

    istringstream istr(StrUpCase(mpi_procedure) + "\n");
    DInterpreter::CommandCode ret = ExecuteLine(&istr);

    MPI_Finalize();
    exit(EXIT_SUCCESS);
  }
#endif

  if (statement.length() > 0) {
    // execute single statement and exit (a new-line is added to statement in gdl.cpp)
    // (e.g. $ gdl -e "print, 'hello world'")
    istringstream iss(statement, ios_base::out);
    try {
      ExecuteLine(&iss);
    } catch (RetAllException& retAllEx) {
    }
    return RC_OK;
  } else {
    // execute batch files (e.g. $ gdl script.pro)
    // before entering the interactive mode
    for (vector<string>::iterator it = batch_files.begin(); it < batch_files.end(); ++it)
      ExecuteFile(*it);
    batch_files.clear(); // not needed anymore...
  }

#if defined(HAVE_LIBREADLINE) || defined(HAVE_LIBEDITLINE)

  // initialize readline (own version - not pythons one)
  // in includefirst.hpp readline is disabled for python_module
  // http://www.delorie.com/gnu/docs/readline/rlman.html
  char rlName[] = "GDL";
  rl_readline_name = rlName;
  //Our handler takes too long
  //when editing the command line with ARROW keys. (bug 562). (used also in gdl.cpp)
  //but... without it we have no graphics event handler! FIXME!!!
  rl_event_hook = GDLEventHandler;
  {
    int edit_input = SysVar::Edit_Input();
    stifle_history(edit_input == 1 || edit_input < 0 ? 200 : edit_input);
  }

  // Eventually read back the ".gdl" path in user $HOME
  // we do not make one commun function with the save side
  // because on the save side we may need to create the .gdl/ PATH ...
  int result, debug = 0;
#ifdef _WIN32
  char *homeDir = getenv("HOMEPATH");
#else
  char *homeDir = getenv("HOME");
#endif
  if (homeDir != NULL) {
    string pathToGDL_history;
    pathToGDL_history = homeDir;
    AppendIfNeeded(pathToGDL_history, "/");
    pathToGDL_history = pathToGDL_history + ".gdl";
    string history_filename;
    AppendIfNeeded(pathToGDL_history, "/");
    history_filename = pathToGDL_history + "history";
    if (debug) cout << "History file name: " << history_filename << endl;

    result = read_history(history_filename.c_str());
    if (debug) {
      if (result == 0) cout << "Successfull reading of ~/.gdl/history" << endl;
      else cout << "Fail to read back ~/.gdl/history" << endl;
    }
  }

  historyIntialized = true;

#endif


  bool runCmd = false; // should tree from $MAIN$ be executed?
  bool continueCmd = false; // .CONTINUE command given already?

  // go into main loop
  for (;;) {
#if defined (_MSC_VER) && _MSC_VER < 1800
    _clearfp();
#else
    feclearexcept(FE_ALL_EXCEPT);
#endif

    try {
      if (runCmd) {
        runCmd = false;
        continueCmd = false;
        RunDelTree();
      } else {
        DInterpreter::CommandCode ret = ExecuteLine();

        // stop steppig when at main level
        stepCount = 0;
        debugMode = DEBUG_CLEAR;

        if (ret == CC_SKIP) {
          Message("Can't continue from this point.");
        }
        else if (ret == CC_CONTINUE) {
          if (static_cast<DSubUD*>
            (callStack.back()->GetPro())->GetTree() != NULL) {
            if (continueCmd)
              runCmd = true;
            else {
              cout << SysVar::MsgPrefix() <<
                "Starting at: $MAIN$" << endl;
              continueCmd = true;
            }
          } else
            cout << SysVar::MsgPrefix() <<
            "Cannot continue from this point." << endl;
        }
      }
    }    catch (RetAllException& retAllEx) {
      runCmd = (retAllEx.Code() == RetAllException::RUN);
      bool resetCmd = (retAllEx.Code() == RetAllException::RESET);
      bool fullResetCmd = (retAllEx.Code() == RetAllException::FULL_RESET);
      if (resetCmd || fullResetCmd) {
        // remove $MAIN$
        delete callStack.back();
        callStack.pop_back();
        assert(callStack.empty());

        ResetObjects();
        ResetHeap();
        if (fullResetCmd) {
          PurgeContainer(libFunList);
          PurgeContainer(libProList);
        }
        // initially done in InitGDL()
        // initializations
        InitObjects();
        // init library functions
        if (fullResetCmd) {
          LibInit();
        }

        // initially done in constructor: setup main level environment
        DPro* mainPro = new DPro(); // $MAIN$  NOT inserted into proList
        EnvUDT* mainEnv = new EnvUDT(NULL, mainPro);
        callStack.push_back(mainEnv); // push main environment (necessary)

        // re-process startup file
        if (startup != "") {
          ifstream in(startup.c_str());

          if (in.fail())
            Warning("Error opening startup file: " + startup);

          ValueGuard<bool> guard(interruptEnable);
          interruptEnable = false;

          bool runCmd = false;
          try {
            while (in.good()) {
#if defined (_MSC_VER) && _MSC_VER < 1800
              _clearfp();
#else
              feclearexcept(FE_ALL_EXCEPT);
#endif

              try {
                if (runCmd) {
                  runCmd = false;
                  RunDelTree();
                } else {
                  DInterpreter::CommandCode ret = ExecuteLine(&in);

                  if (debugMode != DEBUG_CLEAR) {
                    debugMode = DEBUG_CLEAR;
                    Warning("Prematurely closing batch file: " + startup);
                    break;
                  }
                }
              } catch (RetAllException& retAllEx) {
                runCmd = (retAllEx.Code() == RetAllException::RUN);
                if (!runCmd) throw;
              } catch (exception& e) {
                cerr << startup << ": Exception: " << e.what() << endl;
              } catch (...) {
                cerr << startup << ": Unhandled Error." << endl;
              }
            } // while
          } catch (RetAllException& retAllEx) {
          }
        } // if( startup...
      }
    }    catch (exception& e) {
      cerr << "InterpreterLoop: Exception: " << e.what() << endl;
    }    catch (...) {
      cerr << "InterpreterLoop: Unhandled Error." << endl;
    }
  }
}

