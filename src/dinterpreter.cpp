/***************************************************************************
                       dinterpreter.cpp  -  main class which controls it all
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>

#include "dnodefactory.hpp"
#include "str.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"
#include "gdljournal.hpp"

// print out AST tree
//#define GDL_DEBUG
#undef GDL_DEBUG

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


DStructGDL* GDLInterpreter::ObjectStruct( BaseGDL* self, RefDNode mp)
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

DStructGDL* GDLInterpreter::ObjectStructCheckAccess( BaseGDL* self, RefDNode mp)
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


bool GDLInterpreter::CompleteFileName(string& fn)
  // Tries to find file "fn" along GDLPATH.
  // If found, sets fn to the full pathname.
  // and returns true, else false
  // If fn starts with '/' or ".." or "./", just checks it is readable.
{
  // try actual directory (or given path)
  FILE *fp = fopen(fn.c_str(),"r");
  if(fp)
    {
      fclose(fp);
      return true;
    }

  if( PathGiven(fn)) return false;

  StrArr path=SysVar::GDLPath();
  if( path.size() == 0)
    {
      string act="./pro/"; // default path if no path is given
	
#ifdef GDL_DEBUG
      cout << "Looking in:" << endl;
      cout << act << endl;
#endif

      act=act+fn;
      fp = fopen(act.c_str(),"r");
      if(fp) {fclose(fp); fn=act; return true;}
    }
  else
    for(unsigned p=0; p<path.size(); p++)
      {
	string act=path[p];
	
	AppendIfNeeded(act,"/");
	
#ifdef GDL_DEBUG
	if( p == 0) cout << "Looking in:" << endl;
	cout << act << endl;
#endif

	act=act+fn;
	fp = fopen(act.c_str(),"r");
	if(fp) {fclose(fp); fn=act; return true;}
      }
  return false;
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
DStructDesc* GDLInterpreter::GetStruct(const string& name, RefDNode cN)
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
      throw GDLException(cN, "Procedure not found: "+proName);
    }
  
  // 'guard' call stack
  StackGuard<EnvStackT> guard(callStack);

  // interpret it
  EnvT* newEnv=new EnvT(this, cN, proList[proIx]);

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

void GDLInterpreter::SetFunIx( RefDNode& f)
{
  if( f->funIx == -1)
    f->funIx=GetFunIx(f->getText());
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

void GDLInterpreter::SetProIx( RefDNode& f)
{
  if( f->proIx == -1)
    f->proIx=GetProIx(f->getText());
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
    GDLLexer   lexer(in, f, untilPro);
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
  //string strlist=theAST->toStringList();
  //cout << strlist  << endl;
  cout << "Parser output:" << endl;
  antlr::print_tree pt;
  pt.pr_tree(static_cast<antlr::RefAST>(theAST));
  //  cout << endl;
  cout << "Parser end." << endl;
#endif
  
#ifdef GDL_DEBUG
  RefDNode trAST;
#endif

  GDLTreeParser treeParser( f, untilPro);
  try
    {
      treeParser.translation_unit(theAST);

#ifdef GDL_DEBUG
      trAST=treeParser.getAST();
#endif

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
  
#ifdef GDL_DEBUG
  cout << "Tree parser output:" << endl;
  pt.pr_tree(static_cast<antlr::RefAST>(trAST));
  //  cout << endl;
  cout << "Tree parser end." << endl;
#endif
  
  return true;
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

	      if( argstr.length() <= 4 ||
		  StrLowCase( argstr.substr(argstr.length()-4,4)) != ".pro")
		{
		  argstr += ".pro";
		}
	  
	      bool found = CompleteFileName(argstr);
	      if (found) 
		{
		  try {
		    CompileFile( argstr); //, origstr); // default is more verbose
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
  if( cmd( "RESET_SESSION"))
    {
      cout << "RESET_SESSION not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "RETURN"))
    {
      cout << "RETURN not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "RNEW"))
    {
      cout << "RNEW not implemented yet." << endl;
      return CC_OK;
    }
  if( cmd( "RUN"))
    {
      cout << "RUN not implemented yet." << endl;
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
  return CC_OK; // get rid of warning
}

// execute OS shell command (interactive shell if command == "") 
void DInterpreter::ExecuteShellCommand(const string& command)
{
  cout << "Not implemented yet: Execute shell command: " << command << endl;
}

string GetLine( ifstream* in)
{
  string line = "";
  while( line == "" && in->good()) 
    {
      getline( *in, line);
      StrTrim(line);
    }
  return line;
}

// execute one line of code (commands and statements)
DInterpreter::CommandCode DInterpreter::ExecuteLine( ifstream* in)
{
  string line = (in != NULL) ? ::GetLine(in) : GetLine();

  // command
  if( line.substr(0,1) == ".") 
    {
      return ExecuteCommand( line.substr(1));
    }

  // shell command
  if( line.substr(0,1) == "$") 
    {
      ExecuteShellCommand( line.substr(1));
      return CC_OK;
    }

  // statement -> execute it
  executeLine.clear(); // clear EOF (for executeLine)
  executeLine.str( line + "\n"); // append new line

  RefDNode theAST;
  try { 
    auto_ptr<GDLLexer> lexer;

    int lCNum = 0;
    for(;;) 
      {
	lexer.reset( new GDLLexer(executeLine, ""));
	try {
	  // works, but ugly -> depends from parser detecting an error
	  // (which it always will due to missing END_U token in case of LC)
	  lexer->Parser().interactive();
	  break; // no error -> everything ok
	}
	catch( GDLException e)
	  {
	    int lCNew = lexer->LineContinuation();
	    if( lCNew == lCNum)
	      throw; // no LC -> real error
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
  catch( GDLException e)
    {
      ReportCompileError( e);
      return CC_OK;
    }
  catch( ANTLRException e)
    {
      cerr << "Lexer/Parser exception: " <<  e.getMessage() << endl;
      return CC_OK;
    }

  if( theAST == NULL) return CC_OK;

#ifdef GDL_DEBUG
  antlr::print_tree pt;
  cout << "Parser output:" << endl;
  pt.pr_tree(static_cast<antlr::RefAST>(theAST));
  cout << "Parser end." << endl;
#endif

  RefDNode trAST;
  try
    {
      GDLTreeParser treeParser( callStack.back());
	  
      treeParser.interactive(theAST);

      trAST=treeParser.getAST();
    }
  catch( GDLException e)
    {
      ReportCompileError( e);
      return CC_OK;
    }
  catch( ANTLRException e)
    {
      cerr << "Compiler exception: " <<  e.getMessage() << endl;
      return CC_OK;
    }
      
  if( trAST == NULL)
    {
      // normal condition for cmd line procedure calls
      return CC_OK;
    }	

#ifdef GDL_DEBUG
  cout << "Tree parser output:" << endl;
  pt.pr_tree(static_cast<antlr::RefAST>(trAST));
  //  cout << endl;
  cout << "Tree parser end." << endl;
#endif

  try
    {
      GDLInterpreter::RetCode retCode = interactive( trAST);

      // write to journal file
      string actualLine = GetClearActualLine();
      if( actualLine != "") lib::write_journal( actualLine); 
  
      if( retCode == RC_RETURN) return CC_RETURN;
      return CC_OK;
    }
  catch( GDLException e)
    {
      cerr << "Unhandled GDL exception: " <<  e.toString() << endl;;
      return CC_OK;
    }
  catch( ANTLRException e)
    {
      cerr << "Interpreter exception: " <<  e.getMessage() << endl;
      return CC_OK;
    }

  return CC_OK;
}

// if readline is not available or !EDIT_INPUT set to zero
char* DInterpreter::NoReadline( const string& prompt)
{
  cout << prompt << flush;

  ostringstream ostr;
  char ch;
  if( feof(stdin)) return 0;
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
  int edit_input = SysVar::Edit_Input();
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
    
    if( !cline) exit( EXIT_SUCCESS); //break; // readline encountered eof
    
    // make a string
    line = cline;
    free(cline);        // done here for compatibility with readline
  
    StrTrim(line);
  } while( line == "");
  
#ifdef HAVE_LIBREADLINE
  if( edit_input > 20)
    {
      stifle_history( edit_input);
    }
  // const_cast to make it work with older readline versions
  add_history(const_cast<char*>(line.c_str())); 
#endif
  
  return line;
}

// reads user input and executes it
// the main loop
GDLInterpreter::RetCode DInterpreter::InterpreterLoop()
{
  for (;;) {
    try
      {
	DInterpreter::CommandCode ret=ExecuteLine();
	if( ret == CC_RETURN)
	  {
	    if( callStack.size() > 1) return RC_RETURN;
	  }
	if( ret == CC_CONTINUE)
	  {
	    if( callStack.size() > 1) return RC_OK; 
	    cout << SysVar::MsgPrefix() << 
	      "Cannot continue from this point." << endl;
	  }
      }
    catch( RetAllException)
      {
	if( callStack.size() > 1) throw;
      }
    catch( exception e)
      {
	cerr << "InterpreterLoop: Exception: " << e.what() << endl;
      }
    catch (...)
      {	
	cerr << "InterpreterLoop: Unhandled Error." << endl;
      }
  }
}

