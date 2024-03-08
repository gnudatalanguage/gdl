/***************************************************************************
                          gdlexception.cpp  -  exception handling
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
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

#include "gdlexception.hpp"
#include "dnode.hpp"
#include "initsysvar.hpp"
#include "gdljournal.hpp"
#include "dinterpreter.hpp"

//#define GDL_DEBUG
//#undef GDL_DEBUG

using namespace std;

DInterpreter* GDLException::interpreter = NULL;

string GDLException::Name( BaseGDL* b)
{
if(interpreter!=NULL && interpreter->CallStack().size()>0) 
	return interpreter->Name(b);
return "";
}

GDLException::GDLException(DLong eC, const string& s, bool pre, bool decorate): 
  ANTLRException(s),
  errorNode(static_cast<RefDNode>(antlr::nullAST)),
  errorNodeP( NULL),
  errorCode(eC),
  line( 0), col( 0), prefix( pre),
  arrayexprIndexeeFailed(false),
  ioException( false),
  targetEnv( NULL)
{
if(decorate && interpreter!=NULL && interpreter->CallStack().size()>0) 
{
  EnvBaseT* e = interpreter->CallStack().back();
  errorNodeP = e->CallingNode();
  msg = e->GetProName();
  if( msg != "$MAIN$") msg +=  ": "+ s; else msg = s;
}
else
{
  msg = s;
}
  // note: This is for cases, when form a destructor is thrown
  // in these cases, program aborts
#ifdef GDL_DEBUG
   cerr << s << endl;
#endif
}
GDLException::GDLException(const string& s, bool pre, bool decorate): 
  ANTLRException(s),
  errorNode(static_cast<RefDNode>(antlr::nullAST)),
  errorNodeP( NULL),
  errorCode(-1),
  line( 0), col( 0), prefix( pre),
  ioException( false),
  targetEnv( NULL)
{
if(decorate && interpreter!=NULL && interpreter->CallStack().size()>0) 
{
  EnvBaseT* e = interpreter->CallStack().back();
  errorNodeP = e->CallingNode();
  msg = e->GetProName();
  if( msg != "$MAIN$") msg +=  ": "+ s; else msg = s;
}
else
{
  msg = s;
}
  // note: This is for cases, when form a destructor is thrown
  // in these cases, program aborts
#ifdef GDL_DEBUG
   cerr << s << endl;
#endif
}

GDLException::GDLException(const RefDNode eN, const string& s): 
  ANTLRException(s), 
  errorNode(eN),
  errorNodeP( NULL),
  errorCode(-1),
  line( 0), col( 0), prefix( true),
  arrayexprIndexeeFailed(false),
  ioException( false),
  targetEnv( NULL)
{
if(interpreter!=NULL && interpreter->CallStack().size()>0) 
{
  EnvBaseT* e = interpreter->CallStack().back();
  errorNodeP = e->CallingNode();
  msg = e->GetProName();
  if( msg != "$MAIN$") msg +=  ": "+ s; else msg = s;
}
else
{
  msg = s;
}
#ifdef GDL_DEBUG
   cerr << s << endl;
#endif
}
GDLException::GDLException(DLong eC, const RefDNode eN, const string& s): 
  ANTLRException(s), 
  errorNode(eN),
  errorNodeP( NULL),
  errorCode(eC),
  line( 0), col( 0), prefix( true),
  arrayexprIndexeeFailed(false),
  ioException( false),
  targetEnv( NULL)
{
if(interpreter!=NULL && interpreter->CallStack().size()>0) 
{
  EnvBaseT* e = interpreter->CallStack().back();
  errorNodeP = e->CallingNode();
  msg = e->GetProName();
  if( msg != "$MAIN$") msg +=  ": "+ s; else msg = s;
}
else
{
  msg = s;
}
#ifdef GDL_DEBUG
   cerr << s << endl;
#endif
}

GDLException::GDLException(const ProgNodeP eN, const string& s, bool decorate, bool overWriteNode): 
  ANTLRException(s), 
  errorNode(static_cast<RefDNode>(antlr::nullAST)),
  errorNodeP( eN),
  errorCode(-1),
  line( 0), col( 0), prefix( true),
  arrayexprIndexeeFailed(false),
  ioException( false),
  targetEnv( NULL)
{
if( overWriteNode && interpreter!=NULL && interpreter->CallStack().size()>0) 
{
  EnvBaseT* e = interpreter->CallStack().back();
  errorNodeP = e->CallingNode();
}
if( decorate && interpreter!=NULL && interpreter->CallStack().size()>0)
{
  EnvBaseT* e = interpreter->CallStack().back();
  msg = e->GetProName();
  if( msg != "$MAIN$") msg +=  ": "+ s; else msg = s;
}
else
{
  msg = s;
}
#ifdef GDL_DEBUG
   cerr << s << endl;
#endif
}
GDLException::GDLException(DLong eC, const ProgNodeP eN, const string& s, bool decorate, bool overWriteNode): 
  ANTLRException(s), 
  errorNode(static_cast<RefDNode>(antlr::nullAST)),
  errorNodeP( eN),
  errorCode(eC),
  line( 0), col( 0), prefix( true),
  arrayexprIndexeeFailed(false),
  ioException( false),
  targetEnv( NULL)
{
  if( overWriteNode && interpreter!=NULL && interpreter->CallStack().size()>0) 
  {
    EnvBaseT* e = interpreter->CallStack().back();
    errorNodeP = e->CallingNode();
  }
  if( decorate && interpreter!=NULL && interpreter->CallStack().size()>0)
  {
    EnvBaseT* e = interpreter->CallStack().back();
    msg = e->GetProName();
    if( msg != "$MAIN$") msg +=  ": "+ s; else msg = s;
  }
  else
  {
    msg = s;
  }
#ifdef GDL_DEBUG
   cerr << s << endl;
#endif
}

GDLException::GDLException(SizeT l, SizeT c, const string& s): 
  ANTLRException(s),
  errorNode(static_cast<RefDNode>(antlr::nullAST)),
  errorNodeP( NULL),
  errorCode(-1),
  line( l), col( c), prefix( true),
  arrayexprIndexeeFailed(false),
  ioException( false),
  targetEnv( NULL)
{
  if(interpreter!=NULL && interpreter->CallStack().size()>0) 
  {
    EnvBaseT* e = interpreter->CallStack().back();
    errorNodeP = e->CallingNode();
    msg = e->GetProName();
    if( msg != "$MAIN$") msg +=  ": "+ s; else msg = s;
  }
  else
  {
    msg = s;
  }
#ifdef GDL_DEBUG
   cerr << s << endl;
#endif
}
GDLException::GDLException(DLong eC, SizeT l, SizeT c, const string& s): 
  ANTLRException(s),
  errorNode(static_cast<RefDNode>(antlr::nullAST)),
  errorNodeP( NULL),
  errorCode(eC),
  line( l), col( c), prefix( true),
  arrayexprIndexeeFailed(false),
  targetEnv( NULL)
{
  if(interpreter!=NULL && interpreter->CallStack().size()>0) 
  {
    EnvBaseT* e = interpreter->CallStack().back();
    errorNodeP = e->CallingNode();
    msg = e->GetProName();
    if( msg != "$MAIN$") msg +=  ": "+ s; else msg = s;
  }
  else
  {
    msg = s;
  }
#ifdef GDL_DEBUG
   cerr << s << endl;
#endif
}

void Message(const string& s) 
{
  if( SysVar::Quiet() == 0)
    {
      cerr << SysVar::MsgPrefix() << s << endl; 
      lib::write_journal_comment( SysVar::MsgPrefix() + s);
    }
} 

void Warning(const std::string& s) 
{
  cerr << SysVar::MsgPrefix() << s << endl; 
  lib::write_journal_comment( SysVar::MsgPrefix() + s);
} 

void ThrowGDLException( const std::string& str)
{
throw GDLException( str);
}

void WarnAboutObsoleteRoutine(const string& name)
{
  // no static here due to .RESET_SESSION
  DStructGDL* warnStruct = SysVar::Warn();
  // this static is ok as it will evaluate always to the same value
  static unsigned obs_routinesTag = warnStruct->Desc()->TagIndex( "OBS_ROUTINES");
  if (((static_cast<DByteGDL*>( warnStruct->GetTag(obs_routinesTag, 0)))[0]).LogTrue())
    Message("Routine compiled from an obsolete library: " + name);
  // TODO: journal / !QUIET??
}

void WarnAboutObsoleteRoutine(const RefDNode eN, const string& name)
{
// TODO: journal?
  // no static here due to .RESET_SESSION
  DStructGDL* warnStruct = SysVar::Warn();
  // this static is ok as it will evaluate always to the same value
  static unsigned obs_routinesTag = warnStruct->Desc()->TagIndex( "OBS_ROUTINES");
  if (((static_cast<DByteGDL*>( warnStruct->GetTag(obs_routinesTag, 0)))[0]).LogTrue())
  {
    GDLException* e = new GDLException(eN, 
      "Routine compiled from an obsolete library: " + name
    );
    Guard<GDLException> eGuard(e);
    GDLInterpreter::ReportCompileError(*e, "");
// TODO: file 
  }
}
