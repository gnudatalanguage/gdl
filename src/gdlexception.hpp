/***************************************************************************
                              gdlexception.hpp
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

#ifndef GDLEXCEPTION_HPP_
#define GDLEXCEPTION_HPP_

#include <string>
#include <iostream>

#include "prognode.hpp"

#include "antlr/ANTLRException.hpp"

//using namespace std;

class EnvUDT;

class GDLException: public antlr::ANTLRException
{
  static DInterpreter* interpreter;

  std::string msg;

  RefDNode  errorNode;
  ProgNodeP errorNodeP;
  DLong     errorCode;
  SizeT line;
  SizeT col;
  bool prefix;
protected:
  bool ioException;
 
private:  
  EnvUDT* targetEnv; // where to stop (depending on ON_ERROR)

public:
  static DInterpreter* Interpreter() { return interpreter;}
  static void SetInterpreter( DInterpreter* i) { interpreter = i;}

  static std::string Name( BaseGDL* b);

  void SetErrorNodeP( ProgNodeP p) { errorNodeP = p;}

  GDLException(): ANTLRException(), 
    errorNode(static_cast<RefDNode>(antlr::nullAST)),
		  errorNodeP( NULL),
		  errorCode(-1),
		  line( 0), col( 0), prefix( true),
		  ioException( false),
		  targetEnv( NULL)
  {}
  GDLException( DLong eC): ANTLRException(), 
    errorNode(static_cast<RefDNode>(antlr::nullAST)),
		  errorNodeP( NULL),
		  errorCode(eC),
		  line( 0), col( 0), prefix( true),
		  ioException( false),
		  targetEnv( NULL)
  {}
  GDLException(const std::string& s, bool pre = true, bool decorate=true);
  GDLException(const RefDNode eN, const std::string& s);
  GDLException(const ProgNodeP eN, const std::string& s, bool decorate=true, bool overWriteNode=true);
  GDLException(SizeT l, SizeT c, const std::string& s);

  GDLException(DLong eC, const std::string& s, bool pre = true, bool decorate=true);
  GDLException(DLong eC, const RefDNode eN, const std::string& s);
  GDLException(DLong eC, const ProgNodeP eN, const std::string& s, bool decorate=true, bool overWriteNode=true);
  GDLException(DLong eC, SizeT l, SizeT c, const std::string& s);

  ~GDLException() throw() {}

  DLong ErrorCode() const { return errorCode;}
  
  std::string toString() const
  {
	  return msg;
  }

  SizeT getLine() const 
  { 
    if( line != 0) 
      return line;
    if( errorNodeP != NULL)
      return errorNodeP->getLine();
    if( errorNode != static_cast<RefDNode>(antlr::nullAST))
      return errorNode->getLine();
    return 0;
  }

  void SetLine( SizeT l) { line = l;}

  SizeT getColumn() const 
  { 
    //    if( errorNode != static_cast<RefDNode>(antlr::nullAST))
    //      return errorNode->getColumn();
    return col;
  }

  bool Prefix() const 
  { 
    return prefix;
  }

  void SetTargetEnv( EnvUDT* tEnv)
  {
    targetEnv = tEnv;
  }

  EnvUDT* GetTargetEnv()
  {
    return targetEnv;
  }

  bool IsIOException() const { return ioException;}
};

// for ON_IOERROR
class GDLIOException: public GDLException
{
public:
  GDLIOException(): 
    GDLException()
  { ioException = true;}

  GDLIOException(const std::string& s, bool pre = true):
    GDLException( s, pre)
  { ioException = true;}
    
  GDLIOException(const ProgNodeP eN, const std::string& s):
    GDLException( eN, s)
  { ioException = true;}

  GDLIOException(DLong eC): 
    GDLException(eC)
  { ioException = true;}

  GDLIOException(DLong eC,const std::string& s, bool pre = true):
    GDLException( eC, s, pre)
  { ioException = true;}
    
  GDLIOException(DLong eC,const ProgNodeP eN, const std::string& s):
    GDLException( eC, eN, s)
  { ioException = true;}
};

// warnings ignore !QUIET
void Warning(const std::string& s);

// messages honor !QUIET
void Message(const std::string& s);

void ThrowGDLException( const std::string& str);

void WarnAboutObsoleteRoutine(const std::string& name);
void WarnAboutObsoleteRoutine(const RefDNode eN, const std::string& name);

#endif

