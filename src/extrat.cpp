/***************************************************************************
                          extrat.cpp  -  (_REF)_EXTRA keyword handling
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

#include "objects.hpp"
#include "extrat.hpp"
#include "envt.hpp"

using namespace std;

void ExtraT::Resolve()
{
  // if the subroutine has _REF_EXTRA, explicit keywords override
  // if the subroutine has _EXTRA, _EXTRA keywords override

  // 1. extract own keywords from _EXTRA data (override explicit ones)
  // put all others to extra data
  BaseGDL* thisExtra= (env != NULL)? *env : loc;

  DSub* pro=thisEnv->pro;

  DSub::ExtraType extraType= pro->Extra();

  DStructGDL* extraStruct= dynamic_cast<DStructGDL*>(thisExtra);
  if( extraStruct != NULL)
    {
      DStructDesc* desc=extraStruct->Desc();

      SizeT nTag=desc->NTags();
      for( SizeT t=0; t<nTag; t++)
	{
	  const string& tName=desc->TagName( t);
	  
	  // search keyword
	  IDList::iterator f=find_if(pro->key.begin(),
				     pro->key.end(),
				     String_abbref_eq( tName));
	  if( f != pro->key.end())
	    { // found, _EXTRA always overrides explicit keywords
	      SizeT varIx=distance(pro->key.begin(),f);
	      
	      thisEnv->env.Reset( varIx, extraStruct->Get( t)); // local
	    }
	  else // not found -> add tag to extra data
	    { 
	      if( extraType != DSub::NONE)
		{
		  listName.push_back( tName);
		  listEnv.push_back( extraStruct->Get( t)); // always local
		}
	      else if( strict)
		{ // pro has no (_REF)_EXTRA) and _STRICT_EXTRA -> error
		  throw GDLException( "Keyword "+tName+
				      " not allowed in call to: "+
				      pro->ObjectName());
		}
	    }
	}
    }
  else
    {
      DStringGDL* extraString= dynamic_cast<DStringGDL*>(thisExtra);
      if( extraString != NULL)
	{
	  EnvT* caller=thisEnv->Caller();

	  // STRING only works, if the *caller* has _REF_EXTRA
	  if( caller->pro->Extra() == DSub::REFEXTRA)
	    {
	      // caller's extra member holds the actual data
	      ExtraT& cExtra=caller->extra;

	      SizeT nStr=extraString->N_Elements();
	      for( SizeT t=0; t<nStr; t++)
		{
		  const string& kName= StrUpCase( (*extraString)[t]);
		  
		  // get data from caller
		  int  dataIx=cExtra.Find( kName);
		  if( dataIx != -1) 
		    { // found

		      // search keyword
		      IDList::iterator f=find_if(pro->key.begin(),
						 pro->key.end(),
						 String_abbref_eq( kName));
		      if( f != pro->key.end())
			{ // found, _EXTRA always overrides
			  SizeT varIx=distance(pro->key.begin(),f);
			  
			  // global, caller is owner
			  thisEnv->env.Reset( varIx, &cExtra.listEnv[dataIx]);
			}
		      else // not found -> add to extra data
			{ 
			  if( extraType != DSub::NONE)
			    {
			      listName.push_back( kName);
			      // global
			      listEnv.push_back( &cExtra.listEnv[dataIx]);
			    }
			  else if( strict)
			    { // pro has no (_REF)_EXTRA) and _STRICT_EXTRA -> 
			      // error
			      throw GDLException( "Keyword "+kName+
						  " not allowed in call to: "+
						  pro->ObjectName());
			    }
			}
		    } // dataIx != -1
		} // for
	    } // caller->pro->Extra() == DSub::REFEXTRA)
      	} // extraString != NULL
    }
  // all keywords are now overridden in the actual environment
  // listName/listEnv holds all _EXTRA data, which is not used by this 
  // subroutine

  // 2. if pro has (_REF)_EXTRA:
  // combine additional keywords and the (remaining) _EXTRA data to pro's 
  // (_REF)_EXTRA value
  if( extraType == DSub::REFEXTRA)
    { // make string array
      SizeT nEl = listName.size();
      dimension dim( &nEl, 1);
      DStringGDL* extraString = new DStringGDL( dim);
      for( SizeT i=0; i<nEl; i++)
	(*extraString)[i] = listName[i];

      thisEnv->env.Reset( static_cast<SizeT>(pro->extraIx), 
		      static_cast<BaseGDL*>(extraString));
    }
  else if( extraType == DSub::EXTRA)
    { // make structure
      SizeT nEl = listName.size();
      if( nEl > 0)
	{

	  DStructDesc* extraStructDesc = new DStructDesc( "$truct");
	  DStructGDL*  extraStruct = new DStructGDL( extraStructDesc);

	  // from back -> _EXTRA overrides additional keyword
	  for( int i=nEl-1; i>=0; --i)
	    {
	      if( extraStructDesc->TagIndex( listName[i]) == -1)
		{
		  extraStructDesc->AddTag( listName[i], listEnv[i]); 
		  extraStruct->AddTagGrab( listEnv.Grab(i));
		}
	    }
 
	  // look if equal is already there (very possible eg. _EXTRA in loops)
// 	  DStructDesc* oStructDesc=extraStructDesc->FindEqual( structList);
// 	  if( oStructDesc != NULL)
// 	    {
// 	      extraStruct->SetDesc(oStructDesc);
// 	      delete extraStructDesc; 
// 	    }
// 	  else
// 	    {
// 	      // insert into struct list // NOT ANYMORE!!!
// 	      structList.push_back( extraStructDesc);
// 	    }

	  //	  structList.push_back( extraStructDesc);
	  thisEnv->env.Reset( static_cast<SizeT>(pro->extraIx), 
			      static_cast<BaseGDL*>(extraStruct));
	}
    }
}
