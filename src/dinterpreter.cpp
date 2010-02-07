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
#include <unistd.h> // isatty

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
// #define GDL_DEBUG
//#undef GDL_DEBUG

#ifdef GDL_DEBUG
#include "print_tree.hpp"
#endif

using namespace std;
using namespace antlr;

// instantiation of static data
GDLInterpreter::HeapT     GDLInterpreter::heap; 
GDLInterpreter::ObjHeapT  GDLInterpreter::objHeap; 
SizeT                     GDLInterpreter::objHeapIx;
SizeT                     GDLInterpreter::heapIx;
EnvStackT                 GDLInterpreter::callStack; 

ProgNode GDLInterpreter::NULLProgNode;
ProgNodeP GDLInterpreter::NULLProgNodeP = &GDLInterpreter::NULLProgNode;

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

void SetActualCompileOpt( unsigned int cOpt)
{
if( BaseGDL::interpreter!=NULL && BaseGDL::interpreter->CallStack().size()>0) 
	BaseGDL::interpreter->CallStack().back()->SetCompileOpt( cOpt);
}

// used in the statement function.
// runs a new instance of the interpreter if not
// at main level
GDLInterpreter::RetCode GDLInterpreter::NewInterpreterInstance( SizeT lineOffset)
{
  if( callStack.size() <= 1) return RC_ABORT; // stay in main loop 
  
  assert( dynamic_cast<DInterpreter*>( this) != NULL);
  return static_cast<DInterpreter*>( this)->InnerInterpreterLoop(lineOffset);
}

DStructGDL* GDLInterpreter::ObjectStruct( BaseGDL* self, ProgNodeP mp)
{
  DObjGDL* obj=dynamic_cast<DObjGDL*>(self);
  if( obj == NULL) 
    throw GDLException( mp, "Object reference type"
			" required in this context: "+Name(self));
  
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
  catch ( HeapException)
    {
      throw GDLException( mp, "Object not valid: "+Name(self));
    }
  
  return oStructGDL;
}

DStructGDL* GDLInterpreter::ObjectStructCheckAccess( BaseGDL* self, ProgNodeP mp)
{
  DStructGDL* oStruct = ObjectStruct( self, mp);
  
  // check accessibility
  DStructDesc* desc = oStruct->Desc();
  if( !desc->IsParent( callStack.back()->GetPro()->Object()))
    {
      throw GDLException( mp, "Object of type "+desc->Name()+
			  " is not accessible within "+
			  callStack.back()->GetProName() + ": "+Name(self));
    }
  
  return oStruct;
}

// searches and compiles procedure 'pro'
bool GDLInterpreter::SearchCompilePro(const string& pro)
{
  static StrArr openFiles;

  string proFile=StrLowCase(pro)+".pro";

  bool found=CompleteFileName(proFile);
  if( !found) return false;
  
  // file already opened?
  for( StrArr::iterator i=openFiles.begin(); i != openFiles.end(); i++)
    {
      if( proFile == *i) return false;
    }

  StackSizeGuard<StrArr> guard( openFiles);

  // append file to list
  openFiles.push_back(proFile);

  return CompileFile( proFile, pro); // this might trigger recursion
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

  // read/compile of IDENTIFIER__define.pro
  string proName=name+"__DEFINE";
  
  /*bool found=*/ SearchCompilePro(proName);

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
      throw GDLException(cN, "Structure type not defined: "+name);
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
      /*bool found=*/ SearchCompilePro(subName);
            
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
      /*bool found=*/ SearchCompilePro(subName);
            
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
      /*bool found=*/ SearchCompilePro(subName);
	  
      proIx=ProIx(subName);
      if( proIx == -1)
	{
	  throw GDLException(f,"Procedure not found: "+subName,true,false);
	}
    }
  return proIx;
}
int GDLInterpreter::GetProIx( const string& subName)
{
  int proIx=ProIx(subName);
  if( proIx == -1)
    {
      // trigger reading/compiling of source file
      /*bool found=*/ SearchCompilePro(subName);
	  
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
  if( aTy > 100 || bTy > 100)
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
bool GDLInterpreter::CompileFile(const string& f, const string& untilPro) 
{
  ifstream in(f.c_str());
  if( !in) return false; // maybe throw exception here
  
  RefDNode theAST;
  try {  
    GDLLexer   lexer(in, f, GDLParser::NONE, untilPro);
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
  catch( GDLException e)
    {
      ReportCompileError( e, f);
      return false;
    }
  catch( ANTLRException e)
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
  catch( GDLException e)
    {
      ReportCompileError( e, f);
      if( treeParser.ActiveProCompiled()) RetAll();
      return false;
    }
  catch( ANTLRException e)
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

DInterpreter::CommandCode DInterpreter::CmdCompile( const string& command)
{
  string cmdstr = command;
  int sppos = cmdstr.find(" ",0);
  if (sppos == string::npos) 
    {
      cout << "Interactive COMPILE not implemented yet." << endl;
      return CC_OK;
    }
      
  bool retAll = false; // Remember if Retall is needed

  // Parse each file name
  int pos = sppos + 1;
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
	      catch( RetAllException)
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
  int sppos = cmdstr.find(" ",0);
  if (sppos == string::npos) 
    {
      cout << "Interactive RUN not implemented yet." << endl;
      return CC_OK;
    }
      
  bool retAll = false; // Remember if Retall is needed

  // Parse each file name
  int pos = sppos + 1;
  while (pos < command.length()) 
    {
      sppos = command.find(" ",pos);
      if (sppos == string::npos) sppos = command.length();

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
	      catch( RetAllException)
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
  //  return CC_OK;
}


// execute GDL command (.run, .step, ...)
DInterpreter::CommandCode DInterpreter::ExecuteCommand(const string& command)
{
  string cmdstr = command;
  int sppos = cmdstr.find(" ",0);
  if (sppos != string::npos) {
    cmdstr = cmdstr.substr(0, sppos);
  }
    
  //  cout << "Execute command: " << command << endl;

  String_abbref_eq cmd( StrUpCase( cmdstr));

  if( cmd( "COMPILE"))
    {
      return CmdCompile( command);
    }
  if( cmd( "CONTINUE"))
    {
      return CC_CONTINUE;
    }
  if( cmd( "EDIT"))
    {
      cout << "Can't edit file without running GDLDE." << endl;
      return CC_OK;
    }
  if( cmd( "FULL_RESET_SESSION"))
    {
      cout << "FULL_RESET_SESSION not implemented yet." << endl;
      return CC_OK;
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
      cout << "RESET_SESSION not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "RNEW"))
    {
      cout << "RNEW not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "SIZE"))
    {
      cout << "SIZE not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "SKIP"))
    {
      cout << "SKIP not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "STEP"))
    {
      cout << "STEP not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "STEPOVER"))
    {
      cout << "STEPOVER not implemented yet." << endl;
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
  if(commandLine == "") 
  { 
    commandLine = GetEnvString("SHELL");
    if (commandLine == "")
    {
      cout << "Error managing child process. Environment variable SHELL not set." << endl;
      return;
    }
  }

  int ignored = system( commandLine.c_str());
}



string GetLine( istream* in)
{
  string line = "";
  while( line == "" && in->good()) 
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

  //  cout << "ExecuteLine: " << line << endl;

  string firstChar = line.substr(0,1);

  // command
  if( firstChar == ".") 
    {
      return ExecuteCommand( line.substr(1));
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
    auto_ptr<GDLLexer> lexer;

    // LineContinuation LC
    // conactenate the strings and insert \n
    // the resulting string can be fed to the lexer
   
    // print if expr parse ok 
    int lCNum = 0;
    for(;;) 
      {
	lexer.reset( new GDLLexer(executeLine, "", callStack.back()->CompileOpt()));
	try {
	  // works, but ugly -> depends from parser detecting an error
	  // (which it always will due to missing END_U token in case of LC)
 	  //lexer->Parser().SetCompileOpt(callStack.back()->CompileOpt());
 	  lexer->Parser().interactive();
	  break; // no error -> everything ok
	}
	catch( GDLException& e)
	  {
	    int lCNew = lexer->LineContinuation();
	    if( lCNew == lCNum)
// 	      throw; // no LC -> real error
	{
 		try {
// 			executeLine.clear(); // clear EOF (for executeLine)
// 			lexer.reset( new GDLLexer(executeLine, "", callStack.back()->CompileOpt()));
// 			lexer->Parser().expr();
	
			executeLine.clear(); // clear EOF (for executeLine)
			executeLine.str( "print," + executeLine.str()); // append new line
			
			lexer.reset( new GDLLexer(executeLine, "", callStack.back()->CompileOpt()));
			lexer->Parser().interactive();
			
			break; // no error -> everything ok
		}
		catch( GDLException& e2)
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
    theAST = lexer->Parser().getAST();

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
    AddLineOffset( lineOffset, theAST);

#ifdef GDL_DEBUG
  antlr::print_tree pt;
  cout << "Parser output:" << endl;
  pt.pr_tree(static_cast<antlr::RefAST>(theAST));
  cout << "ExecuteLine: Parser end." << endl;
#endif

  ProgNodeP progAST = NULL;;

  RefDNode trAST;
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

      progAST = ProgNode::NewProgNode( trAST);
    }
  catch( GDLException& e)
    {
      ReportCompileError( e);
      return CC_OK;
    }
  catch( ANTLRException& e)
    {
      cerr << "Compiler exception: " <<  e.getMessage() << endl;
      return CC_OK;
    }
  auto_ptr< ProgNode> progAST_guard( progAST);

  try
    {
      
#ifdef GDL_DEBUG
  cout << "Converted tree (ProgNode):" << endl;
  pt.pr_tree( progAST);
  cout << "end." << endl;
#endif

      GDLInterpreter::RetCode retCode = interactive( progAST);
      
      // write to journal file
      string actualLine = GetClearActualLine();
      if( actualLine != "") lib::write_journal( actualLine); 
  
      if( retCode == RC_RETURN) return CC_RETURN;
      return CC_OK;
    }
  catch( GDLException& e)
    {
      cerr << "Unhandled GDL exception: " <<  e.toString() << endl;;
      return CC_OK;
    }
  catch( ANTLRException& e)
    {
      cerr << "Interpreter exception: " <<  e.getMessage() << endl;
      return CC_OK;
    }

  return CC_OK;
}

// if readline is not available or !EDIT_INPUT set to zero
char* DInterpreter::NoReadline( const string& prompt)
{
  if (isatty(0)) cout << prompt << flush;

  ostringstream ostr;
  char ch;
  if( feof(stdin)) return NULL;
  for(;;)
    {
      GDLEventHandler(); 

      ch = getchar();

      if( ch == '\n') break;
      if( feof(stdin)) return NULL;
      ostr << ch;
    }
  ostr << ends;
  string str = ostr.str();

  char *result = (char*) malloc((str.length()+1) * sizeof(char));

  strcpy(result,str.c_str()); // copies including terminating '\0'
  return result;
}

bool  lineEdit = false;

void ControlCHandler(int)
{
  cout << SysVar::MsgPrefix() << "Interrupt encountered." << endl;
  if( lineEdit) cout << SysVar::Prompt() /*.c_str()*/ << flush;
  sigControlC = true;
}

string DInterpreter::GetLine()
{
  clog << flush; cout << flush;

#ifdef HAVE_LIBREADLINE
  int edit_input = SysVar::Edit_Input() && isatty(0);
#endif

  string line;
  do {

    char *cline;

    lineEdit = true;

#ifdef HAVE_LIBREADLINE
    
    if( edit_input != 0)
      cline = readline(const_cast<char*>(SysVar::Prompt().c_str()));
    else
      cline = NoReadline(SysVar::Prompt().c_str());
#else
    
    cline = NoReadline(SysVar::Prompt().c_str());
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
    
    // make a string
    line = cline;
    free(cline);        // done here for compatibility with readline
  
    StrTrim(line);
  } while( line == "");
  
#ifdef HAVE_LIBREADLINE
  // SA: commented out to comply with IDL behaviour- allowing to 
  //     set the history-file length only in the startup file
  //if( edit_input > 20)
  //  {
  //    stifle_history( edit_input);
  //  }

  // we would not like to add the current command if is "EXIT" !!
  if ( StrUpCase(line) != "EXIT") {
    // const_cast to make it work with older readline versions
    add_history(const_cast<char*>(line.c_str())); 
  }
#endif
  
  return line;
}

// reads user input and executes it
// inner loop (called via Control-C, STOP, error)
GDLInterpreter::RetCode DInterpreter::InnerInterpreterLoop(SizeT lineOffset)
{

  bool runCmd = false;
  for (;;) {
    feclearexcept(FE_ALL_EXCEPT);

//     try
//       {
	DInterpreter::CommandCode ret=ExecuteLine(NULL, lineOffset);
	if( ret == CC_RETURN) return RC_RETURN;
	if( ret == CC_CONTINUE) return RC_OK; 
//       }
//     catch( RetAllException&)
//       {
//  	throw;
//       }
    //     catch( exception& e)
    //       {
    // 	cerr << "InnerInterpreterLoop: Exception: " << e.what() << endl;
    //       }
    //     catch (...)
    //       {	
    // 	cerr << "InnerInterpreterLoop: Unhandled Error." << endl;
    //       }
  }
}

// used by pyhton module
bool DInterpreter::RunBatch( istream* in)
{
  ValueGuard<bool> guard( interruptEnable);
  interruptEnable = false;

  while( in->good())
    {
      feclearexcept(FE_ALL_EXCEPT);
      
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
      feclearexcept(FE_ALL_EXCEPT);

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
GDLInterpreter::RetCode DInterpreter::InterpreterLoop( const string& startup, 
  vector<string>& batch_files, const std::string& statement)
{
  // process startup file
  if( startup != "")
    {
      ifstream in(startup.c_str());

      if( in.fail())
	Warning( "Error opening startup file: "+startup);

      ValueGuard<bool> guard( interruptEnable);
      interruptEnable = false;

      bool runCmd = false;
      try
	{
	  while( in.good())
	    {
	      feclearexcept(FE_ALL_EXCEPT);

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
			  Warning( "Prematurely closing batch file: "+startup);
			  break;
			}
		    }
		}
	      catch( RetAllException& retAllEx)
		{
		  runCmd = (retAllEx.Code() == RetAllException::RUN);
		  if( !runCmd) throw;
		}
	      catch( exception& e)
		{
		  cerr << startup << ": Exception: " << e.what() << endl;
		}
	      catch (...)
		{	
		  cerr << startup << ": Unhandled Error." << endl;
		}
	    } // while
	}
      catch( RetAllException& retAllEx)
	{
	}
    } // if( startup...

#ifdef USE_MPI
  int myrank = 0;
  int tag = 0;
  char mpi_procedure[256];
  MPI_Status status;
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank);
  int size; 
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (size > 1) {
    MPI_Recv(mpi_procedure, 256, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);

    istringstream istr(StrUpCase(mpi_procedure) + "\n");
    DInterpreter::CommandCode ret=ExecuteLine( &istr);

    MPI_Finalize();
    exit( EXIT_SUCCESS);
  }
#endif

  if (statement.length() > 0) 
  {
    // execute single statement and exit (a new-line is added to statement in gdl.cpp)
    // (e.g. $ gdl -e "print, 'hello world'")
    istringstream iss(statement, ios_base::out);
    ExecuteLine(&iss);
    return RC_OK;
  }
  else
  {
    // execute batch files (e.g. $ gdl script.pro)
    // before entering the interactive mode
    for (vector<string>::iterator it = batch_files.begin(); it < batch_files.end(); it++) 
      ExecuteFile(*it);
    batch_files.clear(); // not needed anymore...
  }

#ifdef HAVE_LIBREADLINE
  // initialize readline (own version - not pythons one)
  // in includefirst.hpp readline is disabled for python_module
  // http://www.delorie.com/gnu/docs/readline/rlman.html
  char rlName[] = "GDL";
  rl_readline_name = rlName;
  rl_event_hook = GDLEventHandler;
  {
    int edit_input = SysVar::Edit_Input();
    stifle_history(edit_input == 1 || edit_input < 0 ? 20 : edit_input);
  }
  
  // Eventually read back the ".gdl" path in user $HOME
  // we do not make one commun function with the save side
  // because on the save side we may need to create the .gdl/ PATH ...
  int result, debug=0;
  char *homeDir = getenv( "HOME");
  if (homeDir != NULL)
  {
    string pathToGDL_history;
    pathToGDL_history=homeDir;
    AppendIfNeeded(pathToGDL_history, "/");
    pathToGDL_history=pathToGDL_history+".gdl";
    string history_filename;
    AppendIfNeeded(pathToGDL_history, "/");
    history_filename=pathToGDL_history+"history";
    if (debug) cout << "History file name: " <<history_filename << endl;

    result=read_history(history_filename.c_str());
    if (debug) 
    { 
      if (result == 0) cout<<"Successfull reading of ~/.gdl/history"<<endl;
      else cout<<"Fail to read back ~/.gdl/history"<<endl;
    }
  }
#endif


  bool runCmd = false; // should tree from $MAIN$ be executed?
  bool continueCmd = false; // .CONTINUE command given already?

  // go into main loop
  for (;;) {
    feclearexcept(FE_ALL_EXCEPT);

    try
      {
	if( runCmd)
	  {
	    runCmd = false;
	    continueCmd = false;
	    RunDelTree();
	  }
	else
	  {
	    DInterpreter::CommandCode ret=ExecuteLine();

	    if( ret == CC_CONTINUE)
	      {
		if( static_cast<DSubUD*>
		    (callStack.back()->GetPro())->GetTree() != NULL)
		  {
		    if( continueCmd) 
		      runCmd = true;
		    else
		      {
			cout << SysVar::MsgPrefix() << 
			  "Starting at: $MAIN$" << endl;
			continueCmd = true;
		      }
		  }
		else
		  cout << SysVar::MsgPrefix() << 
		    "Cannot continue from this point." << endl;
	      }
	  }
      }
    catch( RetAllException& retAllEx)
      {
	runCmd = (retAllEx.Code() == RetAllException::RUN);
      }
    catch( exception& e)
      {
	cerr << "InterpreterLoop: Exception: " << e.what() << endl;
      }
    catch (...)
      {	
	cerr << "InterpreterLoop: Unhandled Error." << endl;
      }
  }
}

