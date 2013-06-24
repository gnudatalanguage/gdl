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


#include "includefirst.hpp"

#include <string>
#include <iostream>

#include "datatypes.hpp"
#include "envt.hpp"
#include "getfmtast.hpp"
#include "basic_pro.hpp"
#include "objects.hpp"
#include "FMTIn.hpp"
#include "dinterpreter.hpp"

//#include "format.g"

#ifdef _MSC_VER
#include <io.h>
#define isatty _isatty
#endif

namespace lib {
  
  using namespace std;

  void readf( EnvT* e)
  {
    SizeT nParam=e->NParam();
    if( nParam < 1)
      e->Throw( "Incorrect number of arguments.");

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    istream* is;

    bool stdLun = check_lun( e, lun);
    if( stdLun)
      {
	if( lun != 0)
	  e->Throw( "Cannot read from stdout and stderr."
		    " Unit: "+i2s( lun));
	is = &cin;
      }
    else
      {
	if( fileUnits[ lun-1].F77())
	  e->Throw( "Formatted IO not allowed with F77_UNFORMATTED "
		    "files. Unit: "+i2s( lun));

	int sockNum = fileUnits[ lun-1].SockNum();
	//cout << "sockNum: " << sockNum << endl;

       	if (sockNum == -1) {
	  // *** File Read *** //
	  if( fileUnits[ lun-1].Compress())
	    is = &fileUnits[ lun-1].IgzStream();
	  else
	    is = &fileUnits[ lun-1].IStream();

	} else {
	  //  *** Socket Read *** //
	  string *recvBuf = &fileUnits[ lun-1].RecvBuf();

	  // Setup recv buffer & string
	  const int MAXRECV = 2048*8;
	  char buf[MAXRECV+1];

	  // Read socket until finished & store in recv string
	  int totalread = 0;
	  while (1) {
	    memset(buf, 0, MAXRECV+1);
	    int status = recv(sockNum, buf, MAXRECV, 0);
	    //	    cout << "Bytes received: " << status << endl;
	    if (status == 0) break;

	    recvBuf->append(buf, status);

	    //	    for( SizeT i=0; i<status; i++) 
	    // recvBuf->push_back(buf[i]);

	    totalread += status;
	    //cout << "recvBuf size: " << recvBuf->size() << endl;
	    //cout << "Total bytes read: " << totalread << endl << endl;
	  }
	  //  if (totalread > 0) cout << "Total bytes read: " << totalread << endl;

	  // Get istringstream, write recv string, & assign to istream
	  istringstream *iss = &fileUnits[ lun-1].ISocketStream();
	  iss->str(*recvBuf);
	  is = iss;
	}
      }

    read_is( is, e, 1);

    // If socket strip off leading line
    if (lun > 0 && fileUnits[ lun-1].SockNum() != -1) {
      string *recvBuf = &fileUnits[ lun-1].RecvBuf();
      int pos = is->tellg();
      recvBuf->erase(0, pos);

      //      int pos = recvBuf->find("\n", 0);
      //recvBuf->erase(0, pos+1);
    }
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

    bool lastParIsString = true; // default set to 'true' so that nothing is done 
    streampos posBeforeLast;
    
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
      
	ostringstream oss;
      
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
			    (*par)->ToStream( oss);
			    actualPrompt = oss.str();
#ifdef HAVE_LIBREADLINE
			    cout << flush;
#else
			    cout << oss.str() << flush;
#endif
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
		  prompt->ToStream( oss);
		  actualPrompt = oss.str();
#ifdef HAVE_LIBREADLINE
		  cout << flush;
#else
		  cout << oss.str() << flush;
#endif
		}
	      else
		{
		  actualPrompt = ": ";
#ifdef HAVE_LIBREADLINE
		  cout << flush;
#else
		  cout << ": " << flush;
#endif
		}
		
#ifdef HAVE_LIBREADLINE
	    if( is == &cin  && isatty(0))
	      {
		string line;
		string strTrimLine;

		int edit_input = SysVar::Edit_Input();// && isatty(0);

		do {
		  char *cline;

		  lineEdit = true;

		  if( edit_input != 0)
		    cline = readline(actualPrompt.c_str());
		  else
		    cline = e->Interpreter()->NoReadline(actualPrompt.c_str());

		  lineEdit = false;

		  if( !cline)
		    {
		      // 						if (isatty(0))
		      cout << endl;
		      e->Throw("Error encountered reading from: Unit: 0, <stdin> (redirected).");
		    }
		  else
		    // make a string
		    line = cline;

		  free(cline);        // done here for compatibility with readline

		  strTrimLine = line;
		  StrTrim(strTrimLine); 
		} while( strTrimLine == "" && parIn->Type() != GDL_STRING);

		istringstream iss( line + "\n");

		parIn->FromStream( iss);

		if( sigControlC)
		  return;
	      }
	    else
#endif
	      {
		posBeforeLast = is->tellg();

		parIn->FromStream( *is);

		lastParIsString = parIn->Type() == GDL_STRING;
	      }
	  }
      }
      if( !lastParIsString && !is->eof()) // && is->peek() != '\n' && is->peek() != '\r')
      {
	is->seekg( posBeforeLast);
	
	DStringGDL gdlString("");
	gdlString.FromStream( *is);
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
  
    //  Guard<DStringGDL> guard;
    stringstream is;

    if( p->Type() != GDL_STRING)
      {
	DStringGDL* iStr = static_cast<DStringGDL*>(p->Convert2( GDL_STRING, BaseGDL::COPY));

	SizeT nStr = iStr->N_Elements();
	for( SizeT i = 0; i < nStr; i++)
	  is << (*iStr)[ i] << '\n';

	delete iStr;
      }
    else
      {
	DStringGDL* iStr = static_cast<DStringGDL*>(p);
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
