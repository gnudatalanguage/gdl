/***************************************************************************
                          read.cpp  -  GDL READ(F/S) functions
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

#include <iostream>

#include "datatypes.hpp"
#include "envt.hpp"
#include "getfmtast.hpp"
#include "basic_pro.hpp"
#include "objects.hpp"
#include "FMTIn.hpp"

namespace lib {
  
  using namespace std;

  void readf( EnvT* e)
  {
    SizeT nParam=e->NParam();
    if( nParam < 1)
      throw GDLException( e->CallingNode(),
			  "READF: Incorrect number of arguments.");

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    istream* is;

    bool stdLun = check_lun( e, lun);
    if( stdLun)
      {
	if( lun != 0)
	  throw GDLException( e->CallingNode(),
			      "READF: Cannot read from stdout and stderr."
			      " Unit: "+i2s( lun));
	is = &cin;
      }
    else
      {
	is = &fileUnits[ lun-1].IStream();
      }

    read_is( is, e, 1);
  }

  void read( EnvT* e)
  {
    read_is( &cin, e, 0);
  }

  void read_is( istream* is, EnvT* e, int parOffset)
  {
    // PROMPT keyword
    BaseGDL* prompt = e->GetKW( 4);
    if( prompt != NULL && !prompt->Scalar())
      throw GDLException( e->CallingNode(),
			  "PROMPT keyword expression must be a scalar in this context.");
  
    // FORMAT keyword
    if( e->GetKW( 0) != NULL)
      {
	DString fmtString;
	e->AssureScalarKW<DStringGDL>( 0, fmtString);

	RefFMTNode fmtAST = GetFMTAST( fmtString);

	FMTIn Formatter( fmtAST, is, e, parOffset, prompt);
      }
    else // default-format input
      {
	bool noPrompt = true;

	int nParam = e->NParam();
	if( nParam == parOffset) return; 
      
	BaseGDL* parIn;
	for( SizeT i=parOffset; i<nParam; i++)
	  {
            BaseGDL** par = &e->GetPar( i);
            if( (*par) != NULL)
	      {
                if( e->GlobalPar( i))
		  { // defined global
                    parIn = *par;
		  }
                else
		  { // defined local
                    if( prompt != NULL)
		      { // prompt keyword there -> error
                        throw GDLException( e->CallingNode(),
					    "Expression must be named variable "
					    "in this context: "+e->GetParString( i));
		      }
                    else
		      { // prompt not there -> put out or ignore
                        if( is == &cin) 
			  {
			    (*par)->ToStream( cout);
			    cout << flush;
			    noPrompt = false;
			  }
			continue;
		      }
		  }
	      }
            else
	      { // undefined
                if( e->LocalPar( i))
		  throw GDLException( e->CallingNode(),
				      "Internal error: Input: UNDEF is local.");

                (*par) = new DFloatGDL( 0.0);
                parIn = *par;
	      }

	    if( is == &cin && noPrompt)
	      if( prompt != NULL) 
		{
		  prompt->ToStream( cout);
		  cout << flush;
		}
	      else 
		{
		  cout << ": " << flush;
		}
	    parIn->FromStream( *is);
	  }
      }
  }


  void reads( EnvT* e)
  {
    SizeT nParam=e->NParam();
    if( nParam < 1)
      throw GDLException( e->CallingNode(),
			  "READS: Incorrect number of arguments.");
  
    BaseGDL* p = e->GetPar( 0);
    if( p == NULL)
      throw GDLException( e->CallingNode(), "Parameter undefined: "+
			  e->GetParString(0));
  
    //  auto_ptr<DStringGDL> guard;
    stringstream is;

    DStringGDL* iStr = dynamic_cast<DStringGDL*>(p);
    if( iStr == NULL)
      {
	iStr = static_cast<DStringGDL*>(p->Convert2( STRING, BaseGDL::COPY));

	SizeT nStr = iStr->N_Elements();
	for( SizeT i = 0; i < nStr; i++)
	  is << (*iStr)[ i] << '\n';

	delete iStr;
      }
    else
      {
	SizeT nStr = iStr->N_Elements();
	for( SizeT i = 0; i < nStr; i++)
	  is << (*iStr)[ i] << '\n';
      }

    // FORMAT keyword
    if( e->GetKW( 0) != NULL)
      {
	DString fmtString;
	e->AssureScalarKW<DStringGDL>( 0, fmtString);

	RefFMTNode fmtAST = GetFMTAST( fmtString);

	FMTIn Formatter( fmtAST, &is, e, 1, NULL);
      }
    else // default-format input
      {
	if( nParam == 1) return; 
      
	BaseGDL* parIn;
	for( SizeT i=1; i<nParam; i++)
	  {
            BaseGDL** par = &e->GetPar( i);
            if( (*par) != NULL)
	      {
                if( e->GlobalPar( i))
		  { // defined global
                    parIn = *par;
		  }
                else
		  { // defined local
		    throw GDLException( e->CallingNode(),
					"Expression must be named variable "
					"in this context: "+e->GetParString( i));
		  }
	      }
            else
	      { // undefined
                if( e->LocalPar( i))
		  throw GDLException( e->CallingNode(),
				      "Internal error: Input: UNDEF is local.");
		
                (*par) = new DFloatGDL( 0.0);
                parIn = *par;
	      }

	    parIn->FromStream( is);
	  }
      }
  }

} // namespace
