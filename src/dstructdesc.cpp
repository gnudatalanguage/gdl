/***************************************************************************
                          dstructdesc.cpp  -  GDL struct descriptor
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

#include "includefirst.hpp"

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
  for( SizeT i=0; i < nTags; ++i) 
    {
      delete tags[ i];
    }
}

// DUStructDesc::~DUStructDesc()
// {}

DStructDesc::~DStructDesc()
{
  assert( !isUnnamed || (operatorList == NULL));
  if( !isUnnamed) // only named structs have members and overloaded operators 
  // (usually they are never deleted only with .RESET_SESSION and .FULL_RESET_SESSION dot commands)
  {
    delete operatorList;
    for(FunListT::iterator i = this->fun.begin(); i != this->fun.end(); ++i) 
      { delete *i;}
    for(ProListT::iterator i = this->pro.begin(); i != this->pro.end(); ++i) 
      { delete *i;}
  }
}

DStructDesc* FindInStructList(StructListT v, const string& s)
{
  StructListT::iterator f=find_if(v.begin(),v.end(),DStruct_eq(s));
  if( f == v.end()) return NULL;
  return *f;
}

  bool DStructBase::ContainsStringPtrObject()
  {
   for( SizeT t=0; t<tags.size(); ++t)
	{
		if( tags[t]->Type() == GDL_STRING) return true;
		if( tags[t]->Type() == GDL_PTR) return true;
		if( tags[t]->Type() == GDL_OBJ) return true;
		if( tags[t]->Type() == GDL_STRUCT)
		{
			if( static_cast<DStructGDL*>( tags[t])->Desc()->ContainsStringPtrObject()) return true;
		} 
        }
   return false;
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
  OperatorList* parentOperatorList = p->GetOperatorList(); 
  if( parentOperatorList != NULL)
  {
    assert( this->operatorList == NULL); // GDL_OBJECT can only be inherited once
    operatorList = new OperatorList(*parentOperatorList);
  }
}

// copy appropiate member subroutines from fun and pro lists
void DStructDesc::SetupOperators()
{
  assert( this->operatorList != NULL);
  for( FunListT::iterator f = this->fun.begin(); f != this->fun.end(); ++f)
  {
    int ix = OverloadOperatorIndexFun( (*f)->Name());
    if( ix != -1)
      operatorList->SetOperator(ix,*f);      
  }
  for( ProListT::iterator p = this->pro.begin(); p != this->pro.end(); ++p)
  {
    int ix = OverloadOperatorIndexPro( (*p)->Name());
    if( ix != -1)
      operatorList->SetOperator(ix,*p);      
  }
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

      // tag type is converted for convertable types
      if( (!ConvertableType( tags[i]->Type()) || 
	   !ConvertableType( d->tags[i]->Type())) && 
	  tags[i]->Type() != d->tags[i]->Type())
	{
	  throw GDLException( "STRUCT: "+name+": "+TagName(i)+
			      " tag type differs in redefinition.");
	}
      if( tags[i]->Type() == GDL_STRUCT)
	{
	  SpDStruct* castLeft= 
	    static_cast<SpDStruct*>(tags[i]);
	  SpDStruct* castRight= 
	    static_cast<SpDStruct*>(d->tags[i]);
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
      if( left.tags[i]->Type() == GDL_STRUCT)
	{
	  SpDStruct* castLeft= 
	    static_cast<SpDStruct*>(left.tags[i]);
	  SpDStruct* castRight= 
	    static_cast<SpDStruct*>(right.tags[i]);
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
      bool found=GDLInterpreter::SearchCompilePro( name+"__"+pName, true); // true -> search for procedure
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
      bool found=GDLInterpreter::SearchCompilePro( name+"__"+pName, false); // false -> search for function
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

