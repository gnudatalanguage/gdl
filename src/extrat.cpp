/***************************************************************************
                          extrat.cpp  -  (_REF)_EXTRA keyword handling
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

#include "objects.hpp"
#include "extrat.hpp"
#include "envt.hpp"
#include "nullgdl.hpp"

using namespace std;

void ExtraT::ResolveExtra(EnvBaseT* callerIn)
{
  // if the subroutine has _REF_EXTRA, explicit keywords override
  // if the subroutine has _EXTRA, _EXTRA keywords override

  // 1. extract own keywords from _EXTRA data (override explicit ones)
  // put all others to extra data
  BaseGDL* extraVal= (envExtraVal != NULL)? *envExtraVal : locExtraVal;

  DSub* pro=thisEnv->pro;

  DSub::ExtraType extraType= pro->Extra();

//   EnvBaseT* callerDebug=thisEnv->Caller();
//   DSub::ExtraType extraTypeDebug= callerDebug->pro->Extra();

  DStructGDL* extraStruct= dynamic_cast<DStructGDL*>(extraVal);
  if( extraStruct != NULL) // _EXTRA
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
      if (f != pro->key.end())
      { // found, _EXTRA always overrides explicit keywords
        SizeT varIx = distance(pro->key.begin(), f);

        thisEnv->env.Reset(varIx, extraStruct->Get(t)); // local
      }
      else // not found -> add tag to extra data
      {
        if (extraType != DSub::NONE)
        {
          listName.push_back(tName);
          listEnv.push_back(extraStruct->Get(t)); // always local
        }
        else if (strict || callerIn != NULL) // always strict if callerIn is set
        { // pro has no (_REF)_EXTRA and _STRICT_EXTRA -> error
          // ... unless keyword is a warnkey!
          // search warn keyword
          IDList::iterator wf=find_if(pro->warnKey.begin(),
				     pro->warnKey.end(),
				     String_abbref_eq( tName));
          if (wf == pro->warnKey.end())
          {
            thisEnv->Throw("Keyword " + tName +
            " not allowed in call to: " +
            pro->ObjectName());
          }
        }
      }
    }
    }
  else // _REF_EXTRA
    {
      DStringGDL* extraString= dynamic_cast<DStringGDL*>(extraVal);
      if( extraString != NULL)
	{
	  EnvBaseT* caller;
	  if( callerIn == NULL)
	    caller = thisEnv->Caller();
	  else
	    caller = callerIn;

	  // GDL_STRING only works, if the *caller* has _REF_EXTRA
	  if( caller->pro->Extra() == DSub::REFEXTRA)
	    {
	      // caller's extra member holds the actual data
	      assert( caller->extra != NULL);

	      ExtraT& cExtra=*caller->extra;

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
			      thisEnv->Throw( "Keyword "+kName+
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
      if( nEl > 0)
	{
	  dimension dim( &nEl, 1);
	  DStringGDL* extraString = new DStringGDL( dim);
	  for( SizeT i=0; i<nEl; i++)
	    (*extraString)[i] = listName[i];

// 	  assert( thisEnv->env.Loc(static_cast<SizeT>(pro->extraIx)) == NULL /*|| thisEnv->env.Loc(static_cast<SizeT>(pro->extraIx)) == NullGDL::GetSingleInstance()*/);
// 	  assert( thisEnv->env.Env(static_cast<SizeT>(pro->extraIx)) == NULL );

	  delete thisEnv->env.Loc(static_cast<SizeT>(pro->extraIx));
	  
 	  thisEnv->env.Set( static_cast<SizeT>(pro->extraIx), 
 			      static_cast<BaseGDL*>(extraString));
// 	  thisEnv->env.Reset( static_cast<SizeT>(pro->extraIx), 
// 			      static_cast<BaseGDL*>(extraString));
	}
      else
	{
	  assert( thisEnv->env.Loc(static_cast<SizeT>(pro->extraIx)) == NULL);
	  assert( thisEnv->env.Env(static_cast<SizeT>(pro->extraIx)) == NULL);
	}
    }
  else if( extraType == DSub::EXTRA)
    { // make structure
      SizeT nEl = listName.size();
      if( nEl > 0)
	{

	  DStructDesc* extraStructDesc;
	  DStructGDL*  extraStruct = NULL;

	  // from back -> _EXTRA overrides additional keyword
	  for( int i=nEl-1; i>=0; --i)
	    {
	      if( listEnv[i] != NULL) // if undef just skip (pass by value) 
		{
		  if( extraStruct == NULL)
		    {
		      extraStructDesc = new DStructDesc( "$truct");
// 		      extraStruct = new DStructGDL( extraStructDesc, dimension(1));
		      extraStruct = new DStructGDL( extraStructDesc);

//		      extraStructDesc->AddTag( listName[i], listEnv[i]); 
// 		      extraStruct->AddTagGrab( listEnv.Grab(i));
		      extraStruct->NewTag( listName[i], listEnv.Grab(i));
		    }
		  else
		    if( extraStructDesc->TagIndex( listName[i]) == -1)
		      {
			extraStruct->NewTag( listName[i], listEnv.Grab(i)); 
//			extraStructDesc->AddTag( listName[i], listEnv[i]); 
// 			extraStruct->AddTagGrab( listEnv.Grab(i));
//			extraStruct->AddTag( listEnv[i]);
		      }
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


	  assert( thisEnv->env.Loc(static_cast<SizeT>(pro->extraIx)) == NULL);
	  assert( thisEnv->env.Env(static_cast<SizeT>(pro->extraIx)) == NULL);

 	  thisEnv->env.Set( static_cast<SizeT>(pro->extraIx), 
 			      static_cast<BaseGDL*>(extraStruct));


// 	  thisEnv->env.Reset( static_cast<SizeT>(pro->extraIx), 
// 			      static_cast<BaseGDL*>(extraStruct));
	}
    }
}
