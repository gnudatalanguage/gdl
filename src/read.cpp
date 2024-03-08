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

#include "envt.hpp"
#include "getfmtast.hpp"
#include "basic_pro.hpp"
#include "objects.hpp"
#include "FMTIn.hpp"
#include "dinterpreter.hpp"
#include "gdlfpexceptions.hpp"

//#include "format.g"

#if defined(_WIN32) && !defined(__CYGWIN__)
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

	// removing "$" in input format : not used
	std::size_t found =fmtString.find("$");
	if (found!=std::string::npos)
	  fmtString.erase(found,1); 

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
#if defined(HAVE_LIBREADLINE)
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
	      {
		if( prompt != NULL)
		  {
		    prompt->ToStream( oss);
		    actualPrompt = oss.str();
#if defined(HAVE_LIBREADLINE)
		    cout << flush;
#else
		    cout << oss.str() << flush;
#endif
		  }
		else
		  {
		    actualPrompt = ": ";
#if defined(HAVE_LIBREADLINE)
		    cout << flush;
#else
		    cout << ": " << flush;
#endif
		  }
	      }
		
#if defined(HAVE_LIBREADLINE)
	    if( is == &cin  && isatty(0))
	      {
		string line;
		string strTrimLine;

		int edit_input = SysVar::Edit_Input();// && isatty(0);

		do {
		  char *cline;

		  lineEdit = true;
			//report last math exceptions
			GDLCheckFPExceptionsAtLineLevel();
		  if( edit_input != 0)
		    cline = readline(actualPrompt.c_str());
		  else
		    cline = e->Interpreter()->NoReadline(actualPrompt.c_str());

		  lineEdit = false;

		  if( !cline)
		    {
		      // 						if (isatty(0))
		      cout << '\n';
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
// 		posBeforeLast = is->tellg();

		parIn->FromStream( *is);

		lastParIsString = parIn->Type() == GDL_STRING;
	      }
	  }
      }
      // read until end of line
      if( !lastParIsString && !is->eof()) // && is->peek() != '\n' && is->peek() != '\r')
      {
	streampos actPos = is->tellg();

// 	is->seekg( posBeforeLast);
	assert( actPos > 0);
	
	streamoff one = 1;
	is->seekg( actPos-one);
	
	DStringGDL gdlString("");
	gdlString.FromStream( *is);
      }
}

void reads( EnvT* e ) {
  SizeT nParam = e->NParam( );
  if ( nParam < 1 ) throw GDLException( e->CallingNode( ), "READS: Incorrect number of arguments." );

  BaseGDL* p = e->GetPar( 0 );
  if ( p == NULL ) throw GDLException( e->CallingNode( ), "Parameter undefined: " + e->GetParString( 0 ) );

  if ( nParam == 1 ) return;

  //  Guard<DStringGDL> guard;
  stringstream is;
  bool hasFormat=(e->GetKW( 0 ) != NULL);
  DStringGDL* iStr;
  
  //not optimal if already string, but simplifies code: iStr can be safely deleted.
  iStr = static_cast<DStringGDL*> (p->Convert2( GDL_STRING, BaseGDL::COPY ));
  SizeT nStr = iStr->N_Elements( );
  for ( SizeT i = 0; i < nStr; i++ )  is << (*iStr)[ i] << '\n';
  delete iStr;

  // FORMAT keyword
  if ( hasFormat ) {
    DString fmtString;
    e->AssureScalarKW<DStringGDL>(0, fmtString);

    // removing "$" in input format : not used
    std::size_t found = fmtString.find( "$" );
    if ( found != std::string::npos )
      fmtString.erase( found, 1 );

    RefFMTNode fmtAST = GetFMTAST( fmtString );

    FMTIn Formatter( fmtAST, &is, e, 1, NULL );
  } else { // default-format input
    BaseGDL* parIn;
    for ( SizeT i = 1; i < nParam; i++ ) {
      BaseGDL** par = &e->GetPar( i );
      if ( (*par) != NULL ) {
        if ( e->GlobalPar( i ) ) { // defined global
          parIn = *par;
        } else { // defined local
          throw GDLException( e->CallingNode( ),
          "Expression must be named variable "
          "in this context: " + e->GetParString( i ) );
        }
      } else { // undefined
        if ( e->LocalPar( i ) )
          throw GDLException( e->CallingNode( ),
			      "Internal error: Input: UNDEF is local." );
	
        (*par) = new DFloatGDL( 0.0 );
        parIn = *par;
      }

      bool debug=false;
      if (debug) cout << "the raw full input :" << is.str() << endl;

      // for Complex, compting cases is complex since (12) eq (12,12) eq 12 ... count 1
      int flag_cplx=0;
      if (((*par)->Type() == GDL_COMPLEX) || ((*par)->Type() == GDL_COMPLEXDBL)) flag_cplx=1;
      //      int open_brace=0;
      //int loop=0;

      if ((*par)->Type()!=GDL_STRING) { //special treatment for decoding commas
        stringstream temp;
        char c;
	int loop=0;
	int open_brace=0;
	//repeat as many elements necessary, but no more!
        //for (SizeT ielem=0; ielem < (*par)->N_Elements(); ++ielem ) {
	if (debug) cout << "nb elems : " << (*par)->N_Elements() << endl;

        for (int ielem=0; ielem < (*par)->N_Elements(); ++ielem ) {
  
	  loop++;
	  while ( is.get( c ) ) { //remove starting blanks, commas, tabs, newlines
	    if ( c == '(') open_brace++;
	    if ( c == ')') open_brace--;
            if ( c != ',' && c != ' ' && c != '\t' && c != '\n') {
              temp.put( c );
              break;
            }
          }
	  if (debug) cout << "after first while : " << temp.str() << endl;

          while ( is.get( c ) ) { //copy until new separator appears.
            if ( c != ',' && c != ' ' && c != '\t' && c != '\n' ) {
 	    if ( c == '(') open_brace++;
	    if ( c == ')') open_brace--;
              temp.put( c );
            } else {
              is.unget( );
              break;
            }
          }
	  if (debug) cout << "after second while : " << temp.str() << endl;
	  if (flag_cplx && (open_brace > 0)) ielem--;
	  if (debug) cout << "ielem : " << ielem << endl;
 	  
	  //	  if ((ielem > 10) || (ielem < -10)) break;

	  temp.put( ' ' ); //put a spearator between values

	  // this is a security if the input is really badly formatted 
	  if (loop > 5*(*par)->N_Elements()) break;

        } // for loop

	// the way to output the content of "temp" :
	if (debug) cout << "what is transmitted to processing : " << temp.str() << endl;
	//	cout << "what remaind to be processed : " << is.str() << endl;

        parIn->FromStream( temp );
      } else { //so much simpler
        parIn->FromStream( is );
      }
    }
  }
}

} // namespace
