/***************************************************************************
                          dpro.cpp  -  GDL procedures/functions
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

// DPro/DFun classes

#include "dpro.hpp"
#include "dvar.hpp"
#include "str.hpp"
#include "objects.hpp"

using namespace std;

// vtable
DSub::~DSub() {}


// DLib ******************************************************
DLib::DLib( const string& n, const string& o, const int nPar_, 
      const string keyNames[]): DSub(n,o)
{
  // find out number of keywords and set them
  SizeT nKey_=0;
  if( keyNames != NULL)
    {
    while( keyNames[nKey_] != "") nKey_++;
    }

  key.resize(nKey_);
  for( SizeT k=0; k<nKey_; k++) key[k]=keyNames[k];

  if( nKey_ >= 1)
    if( keyNames[0] == "_EXTRA")
      {
	extra = EXTRA;
	extraIx = 0;
      }
    else if( keyNames[0] == "_REF_EXTRA")
      {
	extra = REFEXTRA;
	extraIx = 0;
      }

  nPar=nPar_;
}

const string DLibPro::ToString()
{
  string s;
  if( object != "") s += object+"::";
  s += name;
  if( nPar == -1)
    s += ",[inf. Args]";
  else if( nPar == 1)
    s += ",["+i2s( nPar)+" Arg]";
  else if( nPar > 1)
    s += ",["+i2s( nPar)+" Args]";
  for( SizeT i=0; i<key.size(); ++i)
    s += ","+key[ i];//+"=val";
  return s;
}

const string DLibFun::ToString()
{
  string s = "res=";
  if( object != "") s += object+"::";
  s += name+"(";
  if( nPar == -1)
    s += "[inf. Args]";
  else if( nPar == 1)
    s += "["+i2s( nPar)+" Arg]";
  else if( nPar > 1)
    s += "["+i2s( nPar)+" Args]";
  if( key.size() > 0)
    {
      if( nPar != 0) s += ",";
      for( SizeT i=0; i<key.size(); ++i)
	{
	  s += key[ i];//+"=val";
	    if( i+1 != key.size()) s += ",";
	}
    }
  s += ")";
  return s;
}

DLibPro::DLibPro( LibPro p, const string& n, const string& o, const int nPar_, 
	 const string keyNames[], const string warnKeyNames[])
  : DLib(n,o,nPar_,keyNames), pro(p)
{
  libProList.push_back(this);
}
DLibPro::DLibPro( LibPro p, const string& n, const int nPar_, 
	 const string keyNames[], const string warnKeyNames[])
  : DLib(n,"",nPar_,keyNames), pro(p)
{
  libProList.push_back(this);
}

DLibFun::DLibFun( LibFun f, const string& n, const string& o, const int nPar_, 
	 const string keyNames[], const string warnKeyNames[])
  : DLib(n,o,nPar_,keyNames), fun(f)
{
  libFunList.push_back(this);
}

DLibFun::DLibFun( LibFun f, const string& n, const int nPar_, 
	 const string keyNames[], const string warnKeyNames[])
  : DLib(n,"",nPar_,keyNames), fun(f)
{
  libFunList.push_back(this);
}



// DSubUD ****************************************************
DSubUD::~DSubUD()
{
  // delete only common references (common blocks only if owner)
  CommonBaseListT::iterator it;
  for( it=common.begin(); it !=common.end(); ++it)
    {
      DCommonRef* cRef=dynamic_cast<DCommonRef*>(*it);
      delete cRef; // also ok if cRef is NULL
    }
}

DSubUD::DSubUD(const string& n,const string& o,const string& f) : 
  DSub(n,o), file(f),
  tree( static_cast<RefDNode>(antlr::nullAST)),
  labelList()
{
  if( o != "")
    AddPar( "SELF");
}

void DSubUD::AddPar(const string& p)
{
  var.push_back(p);
  nPar++;
}

unsigned DSubUD::AddVar(const string& v)
{
  var.push_back(v);
  return var.size()-1;
}

void DSubUD::AddKey(const string& k, const string& v)
{
  if( k == "_REF_EXTRA")
    {
      if( extra == EXTRA)
	throw GDLException("Routines cannot be declared with both"
			   " _EXTRA and _REF_EXTRA.");
      extra=REFEXTRA;
      extraIx=0;
    }
  else if( k == "_EXTRA")
    {
      if( extra == REFEXTRA)
	throw GDLException("Routines cannot be declared with both"
			   " _EXTRA and _REF_EXTRA.");
      extra=EXTRA;
      extraIx=0;
    }
  else
    {
      if(extraIx != -1) extraIx++;
    }
  key.push_front(k);
  var.push_front(v);
}


