/***************************************************************************
                          print.cpp  -  GDL PRINT(F) functions
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

#include <iostream>

#include "datatypes.hpp"
#include "envt.hpp"
#include "getfmtast.hpp"
#include "basic_pro.hpp"
#include "objects.hpp"
#include "FMTOut.hpp"
#include "terminfo.hpp" 
#include "gdljournal.hpp"
#include "dinterpreter.hpp"

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0x2000
#endif 

namespace lib {

  using namespace std;

  void print_vmsCompat( EnvT* e, int* parOffset)
  {
    // SA: handling special VMS-compatibility syntax, e.g.: print, '$(F)', 100
    //     (if FORMAT not defined, more than 2 params, first param is scalar string
    //     and begins with "$(" then first param minus "$" is treated as FORMAT)
    if (e->GetKW(0) == NULL && e->NParam() > 1 + *parOffset)
    { 
      BaseGDL* par = e->GetParDefined(*parOffset);
      if (par->Type() == STRING && par->Scalar() && 
        (*static_cast<DStringGDL*>(par))[0].compare(0,2,"$(") == 0) 
      {
        e->SetKeyword("FORMAT", 
          new DStringGDL((*static_cast<DStringGDL*>(par))[0].c_str()+1));
        (*parOffset)++;
      }
    }
  }

  void printf( EnvT* e)
  {
    SizeT nParam=e->NParam();
    if( nParam < 1)
      e->Throw( "Incorrect number of arguments.");

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    ostream* os;
    ostringstream oss;

    bool stdLun = check_lun( e, lun);
    SizeT width;

    int sockNum = -1; 

    if( stdLun)
      {
	if( lun == 0)
	  e->Throw( "Cannot write to stdin. Unit: "+i2s( lun));
	
	os = (lun == -1)? &cout : &cerr;
	
	width = TermWidth();
      }
    else
      {
	if( fileUnits[ lun-1].F77())
	  e->Throw( "Formatted IO not allowed with F77_UNFORMATTED "
		    "files. Unit: "+i2s( lun));

	if( fileUnits[ lun-1].Xdr() != NULL)
	  e->Throw( "Formatted IO not allowed with XDR "
		    "files. Unit: "+i2s( lun));
	
	sockNum = fileUnits[ lun-1].SockNum();

	if (sockNum == -1) 
	  os = &fileUnits[ lun-1].OStream();
	else
	  os = &oss;

	width = fileUnits[ lun-1].Width();
      }
    
    int parOffset = 1;
    print_vmsCompat(e, &parOffset);
    print_os( os, e, parOffset, width);

    // Socket send
    if (sockNum != -1) {
      int status = send(sockNum, oss.rdbuf()->str().c_str(), 
			oss.rdbuf()->str().size(), MSG_NOSIGNAL);

      if (status != oss.rdbuf()->str().size())
	e->Throw( "SEND error Unit: "+i2s( lun)+":"+oss.rdbuf()->str());
    }
  
    if( stdLun)
      {
	GDLInterpreter* ip = e->Interpreter();
	write_journal( ip->GetClearActualLine());
	write_journal_comment( e, parOffset, width);
      }
  }
  
  void print( EnvT* e)
  {
    SizeT width = TermWidth();

    int parOffset = 0;
    print_vmsCompat(e, &parOffset);
    print_os( &cout, e, parOffset, width);
    GDLInterpreter* ip = e->Interpreter();
    write_journal( ip->GetClearActualLine());
    write_journal_comment( e, parOffset, width);
  }
  
  void print_os( ostream* os, EnvT* e, int parOffset, SizeT width)
  {
    // FORMAT keyword
    if( e->GetKW( 0) != NULL)
      {
	DString fmtString;
	e->AssureScalarKW<DStringGDL>( 0, fmtString);

	RefFMTNode fmtAST = GetFMTAST( fmtString);

	// formatted output ignores WIDTH
	FMTOut Formatter( fmtAST, os, e, parOffset); 
      }
    else // default-format output
      {
	int nParam = e->NParam();

	if( nParam == parOffset) 
	  {
	    (*os) << endl;
	    return;
	  }
      
	BaseGDL* par;
	SizeT actPos = 0;
	for( SizeT i=parOffset; i<nParam; i++)
	  {
	    par=e->GetParDefined( i);
	    par->ToStream( *os, width, &actPos);
	  }
        bool singleNullChar = (par->Type() == STRING && (nParam-parOffset)>1 &&
			       (*static_cast<DStringGDL*>(par))[0] == "");
	if( (par->Dim().Rank() == 0 && !singleNullChar) || 
	    par->Type() == STRUCT) (*os) << endl;
      }
  }

  // SA: we're better than IDL! - we accept more than 20 parameters ;)
  void pm(EnvT* e)
  {
    int nParam = e->NParam();
    if (nParam == 0) return;

    // GDL magick (based on the Python interface code)
    static int printIx = LibProIx("PRINT");
    EnvBaseT* env = new EnvT(NULL, libProList[printIx]);
    auto_ptr<EnvBaseT> env_guard(env);
    BaseGDL* par;
    env->SetNextPar(&par);

    // passing on the FORMAT keyword
    static int formatIx = e->KeywordIx("FORMAT");
    if (e->GetKW(formatIx) != NULL) 
    {
      if (e->GetKW(formatIx)->Rank() != 0)
        e->Throw("FORMAT keyword must be a scalar");
      env->SetKeyword("FORMAT", &e->GetKW(formatIx));
    }

    // is it needed here?
    StackSizeGuard<EnvStackT> guard( GDLInterpreter::CallStack());
    GDLInterpreter::CallStack().push_back(env);

    // printing the title if TITLE keyword present
    static int titleIx = e->KeywordIx("TITLE");
    if (e->GetKW(titleIx) != NULL)
    {
      par = e->GetKW(titleIx);
      static_cast<DLibPro*>(env->GetPro())->Pro()(static_cast<EnvT*>(env));
    }
    
    // looping over the parameters
    for (SizeT i = 0; i < nParam; ++i)
    {
      if (e->GetParDefined(i)->N_Elements() <= 1)
      {
        par = e->GetParDefined(i);
        static_cast<DLibPro*>(env->GetPro())->Pro()(static_cast<EnvT*>(env));
      } 
      else 
      {
        if (e->GetParDefined(i)->Type() == STRUCT)
          e->Throw("Transposing arrays of structures is undefined");
        par = e->GetParDefined(i)->Transpose(NULL);
        static_cast<DLibPro*>(env->GetPro())->Pro()(static_cast<EnvT*>(env));
        delete par;
      }
    }
  }

} // namespace


