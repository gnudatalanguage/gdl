/***************************************************************************
                          dstructdesc.cpp  -  GDL struct descriptor
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

#include <algorithm>

#include "datatypes.hpp"
#include "dstructdesc.hpp"
#include "GDLInterpreter.hpp"
#include "dstructgdl.hpp"

using namespace std;

// store vtable in this obj file
DStructBase::~DStructBase()
{
  SizeT nTags = NTags();
  for( SizeT i=0; i < nTags; ++i) delete tags[i];
}

// DUStructDesc::~DUStructDesc()
// {}

DStructDesc::~DStructDesc()
{}

DStructDesc* FindInStructList(StructListT v, const string& s)
{
  StructListT::iterator f=find_if(v.begin(),v.end(),DStruct_eq(s));
  if( f == v.end()) return NULL;
  return *f;
}

void DUStructDesc::AddTag( const string& tagName, const BaseGDL* data)
{
  for( SizeT i=0; i < tNames.size(); i++)
    if( tNames[i] == tagName)
      throw GDLException(tagName+" is already defined "
			 "with a conflicting definition");
  
  tNames.push_back(tagName);
  Add( data->GetTag());
}

void DStructDesc::AddParent( DStructDesc* p)
{
  SizeT nTags=p->NTags();
  for( SizeT t=0; t < nTags; t++)
    AddTag( p->TagName(t), (*p)[t]);
  parent.push_back(p);
}

// more sophisticated error messages than operator==
void DStructDesc::AssureIdentical( DStructDesc* d)
{
  // name is the same
  if( NTags() != d->NTags())
    {
      throw GDLException( "STRUCT: "+name+": redefinition with different "
			  "number of tags.");
    }
  if( parent.size() != d->parent.size())
    {
      throw GDLException( "STRUCT: "+name+": redefinition with different "
			  "number of parents.");
    }
  // compare all tag names
  // compare the tags (type and dim)
  for( SizeT i=0; i < NTags(); i++)
    {
      if( TagName(i) != d->TagName(i))
	{
	  throw GDLException( "STRUCT: "+name+": "+TagName(i)+
			      " tag name differs in redefinition.");
	}
      if( tags[i]->Dim() != d->tags[i]->Dim())
	{
	  throw GDLException( "STRUCT: "+name+": "+TagName(i)+
			      " tag dimension "
			      "differs in redefinition.");
	} 
      if( tags[i]->Type() != d->tags[i]->Type())
	{
	  throw GDLException( "STRUCT: "+name+": "+TagName(i)+
			      " tag type differs in redefinition.");
	}
      if( tags[i]->Type() == STRUCT)
	{
	  SpDStruct* castLeft= 
	    dynamic_cast<SpDStruct*>(tags[i]);
	  SpDStruct* castRight= 
	    dynamic_cast<SpDStruct*>(d->tags[i]);
	  DStructDesc* leftD=castLeft->Desc();
	  DStructDesc* rightD=castRight->Desc();
	  
	  if( !(*leftD == *rightD)) 
	    {
	      throw GDLException( "STRUCT: "+name+": "+TagName(i)+
				  " tag struct differs in"
				  " redefinition.");
	    }
	}
    }
  // compare all parents
  for( SizeT i=0; i < parent.size(); i++)
    {
      if( parent[i] != d->parent[i])
	{
	  throw GDLException( "STRUCT: "+name+": "+parent[i]->Name()+
			      " parent class differs in redefinition.");
	}
    }
}

// two structs are equal if the contain the same datatypes in the
// same order (although names might differ)
bool operator==(const DStructDesc& left,
		const DStructDesc& right)
{
  // name is the same
  if( left.NTags() != right.NTags()) return false;
  // struct layout can be same with different inheritance
  //  if( left.parent.size() != right.parent.size()) return false;
  // compare all tag names
  // compare the tags (type and dim)
  for( SizeT i=0; i < left.NTags(); i++)
    {
      //      if( left.TagName(i) != right.TagName(i)) return false;
      if( left.tags[i]->Dim() != right.tags[i]->Dim()) return false;
      if( left.tags[i]->Type() != right.tags[i]->Type()) return false;
      if( left.tags[i]->Type() == STRUCT)
	{
	  SpDStruct* castLeft= 
	    dynamic_cast<SpDStruct*>(left.tags[i]);
	  SpDStruct* castRight= 
	    dynamic_cast<SpDStruct*>(right.tags[i]);
	  DStructDesc* leftD=castLeft->Desc();
	  DStructDesc* rightD=castRight->Desc();
	  
	  // recursive call of operator ==
	  if( (leftD != rightD) && !(*leftD == *rightD)) 
	    return false;
	}
    }
  // compare all parents
  //  for( SizeT i=0; i < left.parent.size(); i++)
  //    {
  //      if( left.parent[i] != right.parent[i]) return false;
  //    }
  return true;
}

bool operator!=(const DStructDesc& left,
		const DStructDesc& right)
{
  return !(left == right);
}

// DStructDesc* DStructDesc::FindEqual( const StructListT& sL)
// {
//   for( SizeT i=0; i<sL.size(); i++)
//     {
//       if( *this == *sL[i]) return sL[i];
//     }
//   return NULL;
// }

DPro* DStructDesc::GetPro( const string& pName)
{
  DPro* p;

  p=FindInProList( pName);
  if( p != NULL) return p;
      
  int fInIDList=FindInIDList( noDirectMembers, pName);
  if( fInIDList == -1)
    {
      bool found=GDLInterpreter::SearchCompilePro( name+"__"+pName);
      if( found)
	{
	  p=FindInProList( pName);
	  if( p != NULL) return p;
	}
      else
	noDirectMembers.push_back(pName);
    }  
  
  SizeT nParents=parent.size();
  for( SizeT i=0; i<nParents; i++)
    {
      p=parent[i]->GetPro( pName);
      if( p != NULL) return p;
    }
  return NULL;
}

DFun* DStructDesc::GetFun( const string& pName)
{
  DFun* p;

  p=FindInFunList( pName);
  if( p != NULL) return p;

  int fInIDList=FindInIDList( noDirectMembers, pName);
  if( fInIDList == -1)
    {
      bool found=GDLInterpreter::SearchCompilePro( name+"__"+pName);
      if( found)
	{
	  p=FindInFunList( pName);
	  if( p != NULL) return p;
	}
      else
	noDirectMembers.push_back(pName);
    }  
  
  SizeT nParents=parent.size();
  for( SizeT i=0; i<nParents; i++)
    {
      p=parent[i]->GetFun( pName);
      if( p != NULL) return p;
    }
  return NULL;
}

DPro* DStructDesc::GetPro( const string& pName, const string& parentName)
{
  if( !IsParent( parentName))
    throw GDLException( parentName+" is not a parent of "+name);
  
  DStructDesc* d=FindInStructList( structList, parentName);
  if( d == NULL)
    throw GDLException("Internal error: Struct "+parentName+" not found.");
  
  return d->GetPro( pName);
}

DFun* DStructDesc::GetFun( const string& pName, const string& parentName)
{
  if( !IsParent( parentName))
    throw GDLException( parentName+" is not a parent of "+name);
  
  DStructDesc* d=FindInStructList( structList, parentName);
  if( d == NULL)
    throw GDLException("Internal error: Struct "+parentName+" not found.");
  
  return d->GetFun( pName);
}

