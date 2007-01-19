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

namespace lib {

  using namespace std;

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

    int sockNum = fileUnits[ lun-1].SockNum();

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
	
	if (sockNum == -1) 
	  os = &fileUnits[ lun-1].OStream();
	else
	  os = &oss;

	width = fileUnits[ lun-1].Width();
      }
    
    print_os( os, e, 1, width);

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
	write_journal_comment( e, 1, width);
      }
  }
  
  void print( EnvT* e)
  {
    SizeT width = TermWidth();
    print_os( &cout, e, 0, width);

    GDLInterpreter* ip = e->Interpreter();
    write_journal( ip->GetClearActualLine());
    write_journal_comment( e, 0, width);
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

} // namespace


