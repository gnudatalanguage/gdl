/* **************************************************************************
                          basic_pro.cpp  -  basic GDL library procedures
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
#include <fstream>
#include <memory>
#include <set>

#include <sys/types.h>
#include <sys/wait.h>

#ifdef __APPLE__
# include <crt_externs.h>
# define environ (*_NSGetEnviron())
#else
#include <unistd.h>
#endif

#include "dinterpreter.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
#include "dpro.hpp"
#include "io.hpp"
#include "basic_pro.hpp"

namespace lib {
 
  using namespace std;

  // display help for one variable or one structure tag
  void help_item( BaseGDL* par, DString parString, bool doIndentation)
  {
    if( doIndentation) cout << "   ";

    // Name display
    cout.width(16);
    cout << left << parString;
    if( parString.length() >= 16)
      {
        cout << endl;
        cout.width(doIndentation? 19:16);
        cout << "";
      }

    // Type display
    if( !par)
      {
        cout << "UNDEFINED = <Undefined>" << endl;
        return;
      }
    cout.width(10);
    cout << par->TypeStr() << right;

    if( !doIndentation) cout << "= ";

    // Data display
    if( par->Type() == STRUCT)
      {
        DStructGDL* s = static_cast<DStructGDL*>( par);
        cout << "-> ";
        cout << (s->Desc()->IsUnnamed()? "<Anonymous>" : s->Desc()->Name());
      }
    else if( par->Dim( 0) == 0)
      {
        if (par->Type() == STRING)
	  {
            // trim string larger than 45 characters
            DString dataString = (*static_cast<DStringGDL*>(par))[0];
            cout << "'" << StrMid( dataString,0,45,0) << "'";
	    if( dataString.length() > 45) cout << "...";
	  }
	else
	  {
            par->ToStream( cout);
	  }
      }

    // Dimension display
    if( par->Dim( 0) != 0) cout << par->Dim();

    // End of line
    cout << endl;
  }

  void help( EnvT* e)
  {
    if( e->KeywordSet( "INFO"))
      {
	cout << "Homepage: http://gnudatalanguage.sf.net" << endl;
	cout << "HELP,/LIB for a list of all library functions/procedures." << endl;
	cout << "Additional subroutines are written in GDL language, "
	  "look for *.pro files." << endl;
	cout << endl;
      }

    if( e->KeywordSet( "LIB"))
      {
	deque<DString> subList;
	SizeT nPro = libProList.size();
	cout << "Library procedures (" << nPro <<"):" << endl;
	for( SizeT i = 0; i<nPro; ++i)
	  subList.push_back(libProList[ i]->ToString());

	sort( subList.begin(), subList.end());

	for( SizeT i = 0; i<nPro; ++i)
	  cout << subList[ i] << endl;

	subList.clear();

	SizeT nFun = libFunList.size();
	cout << "Library functions (" << nFun <<"):" << endl;
	for( SizeT i = 0; i<nFun; ++i)
	  subList.push_back(libFunList[ i]->ToString());

	sort( subList.begin(), subList.end());

	for( SizeT i = 0; i<nFun; ++i)
	  cout << subList[ i] << endl;
      }

    bool isKWSetStructures = e->KeywordSet( "STRUCTURES");
    SizeT nParam=e->NParam();
    for( SizeT i=0; i<nParam; i++)
      {
	BaseGDL*& par=e->GetPar( i);
	DString parString = e->Caller()->GetString( par);
	if( !par || !isKWSetStructures || par->Type() != STRUCT)
          {
            help_item( par, parString, false);
          }
        else
	  {
            DStructGDL* s = static_cast<DStructGDL*>( par);
            cout << "** Structure ";
            cout << (s->Desc()->IsUnnamed()? "<Anonymous>" : s->Desc()->Name());

	    SizeT nTags = s->Desc()->NTags();
            cout << ", " << nTags << " tags:" << endl;
	    for (SizeT t=0; t < nTags; ++t)
	      {    
		DString tagString = s->Desc()->TagName(t);
                help_item( s->Get(t), tagString, true);
	      }
          }
      }
  }
  
  void exitgdl( EnvT* e)
  {
    BaseGDL* status=e->GetKW( 1);
    if( status == NULL) exit( EXIT_SUCCESS);
    
    if( !status->Scalar())
      e->Throw( "Expression must be a scalar in this context: "+
		e->GetString( status));

    DLongGDL* statusL=static_cast<DLongGDL*>(status->Convert2( LONG, 
							       BaseGDL::COPY));
    
    DLong exit_status;
    statusL->Scalar( exit_status);
    exit( exit_status);
  }


  void ptr_free( EnvT* e)
  {
    SizeT nParam=e->NParam();
    for( SizeT i=0; i<nParam; i++)
      {
	DPtrGDL* par=dynamic_cast<DPtrGDL*>(e->GetPar( i));
	if( par != NULL) 
	  {
	    e->FreeHeap( par);
	  }
	else
	  e->Throw( "Pointer type required"
		    " in this context: "+e->GetParString(i));
      }
  }

  void obj_destroy( EnvT* e)
  {
    static set< DObj> inProgress;

    StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());

    int nParam=e->NParam();
    if( nParam == 0) return;
    
    BaseGDL* p= e->GetParDefined( 0);

    DObjGDL* op= dynamic_cast<DObjGDL*>(p);
    if( op == NULL)
      e->Throw( "Parameter must be an object in"
		" this context: "+
		e->GetParString(0));

    SizeT nEl=op->N_Elements();
    for( SizeT i=0; i<nEl; i++)
      {
	DObj actID=(*op)[i];
	if( actID != 0 && (inProgress.find( actID) == inProgress.end()))
	  {
	    DStructGDL* actObj;
	    try{
	      actObj=e->GetObjHeap( actID);
	    }
	    catch( GDLInterpreter::HeapException){
	      actObj=NULL;
	    }
	    
	    if( actObj != NULL)
	      {
		// call CLEANUP function
		DPro* objCLEANUP= actObj->Desc()->GetPro( "CLEANUP");

		if( objCLEANUP != NULL)
		  {
		    BaseGDL* actObjGDL = new DObjGDL( actID);
		    auto_ptr<BaseGDL> actObjGDL_guard( actObjGDL);

		    e->PushNewEnv( objCLEANUP, 1, &actObjGDL);

		    inProgress.insert( actID);
	    
		    e->Interpreter()->call_pro( objCLEANUP->GetTree());

		    inProgress.erase( actID);

		    e->FreeObjHeap( actID); // the actual freeing

		    delete e->Interpreter()->CallStack().back();
		    e->Interpreter()->CallStack().pop_back();
		  }
	      }
	  }
      }
  }
  
  void call_procedure( EnvT* e)
  {
    StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());

    int nParam=e->NParam();
    if( nParam == 0)
      e->Throw( "No procedure specified.");
    
    DString callP;
    e->AssureScalarPar<DStringGDL>( 0, callP);

    // this is a procedure name -> convert to UPPERCASE
    callP = StrUpCase( callP);

    // first search library procedures
    int proIx=LibProIx( callP);
    if( proIx != -1)
      {
	e->PushNewEnv( libProList[ proIx], 1);
	
	// make the call
	EnvT* newEnv = e->Interpreter()->CallStack().back();
	static_cast<DLibPro*>(newEnv->GetPro())->Pro()(newEnv);
      }
    else
      {
	proIx = DInterpreter::GetProIx( callP);
	
	e->PushNewEnv( proList[ proIx], 1);
	
	// make the call
	EnvT* newEnv = e->Interpreter()->CallStack().back();
	e->Interpreter()->call_pro(static_cast<DSubUD*>(newEnv->GetPro())->
				   GetTree());
      }
  }

  void call_method_procedure( EnvT* e)
  {
    StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());

    int nParam=e->NParam();
    if( nParam < 2)
      e->Throw( "Name and object reference must be specified.");
    
    DString callP;
    e->AssureScalarPar<DStringGDL>( 0, callP);

    // this is a procedure name -> convert to UPPERCASE
    callP = StrUpCase( callP);

    DStructGDL* oStruct = e->GetObjectPar( 1);
    
    DPro* method= oStruct->Desc()->GetPro( callP);

    if( method == NULL)
      e->Throw( "Method not found: "+callP);

    e->PushNewEnv( method, 2, &e->GetPar( 1));
    
    // the call
    e->Interpreter()->call_pro( method->GetTree());
  }

  void get_lun( EnvT* e)
  {
    int nParam=e->NParam( 1);
    
    // not using SetPar later gives a better error message
    e->AssureGlobalPar( 0);
    
    // here lun is the GDL lun, not the internal one
    DLong lun = GetLUN();

    if( lun == 0)
      e->Throw( "All available logical units are currently in use.");

    BaseGDL** retLun = &e->GetPar( 0);
    
    delete (*retLun); 
    //            if( *retLun != e->Caller()->Object()) delete (*retLun); 
    
    *retLun = new DLongGDL( lun);
    return;
  }

  // returns true if lun points to special unit
  // lun is GDL lun (-2..128)
  bool check_lun( EnvT* e, DLong lun)
  {
    if( lun < -2 || lun > maxLun)
      e->Throw( "File unit is not within allowed range: "+
		i2s(lun)+".");
    return (lun <= 0);
  }
  
  // TODO: handle ON_ERROR, ON_IOERROR, !ERROR_STATE.MSG
  void open_lun( EnvT* e, fstream::openmode mode)
  {
    int nParam=e->NParam( 2);
    
    if( e->KeywordSet( "GET_LUN")) get_lun( e);
    // par 0 contains now the LUN

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    bool stdLun = check_lun( e, lun);
    if( stdLun)
      e->Throw( "Unit already open. Unit: "+i2s( lun));
    
    DString name;
    // IDL allows here also arrays of length 1
    e->AssureScalarPar<DStringGDL>( 1, name); 

    // endian
    bool swapEndian=false;
    if( e->KeywordSet( "SWAP_ENDIAN"))
      swapEndian = true;
    else if( BigEndian())
      swapEndian = e->KeywordSet( "SWAP_IF_BIG_ENDIAN");
    else
      swapEndian = e->KeywordSet( "SWAP_IF_LITTLE_ENDIAN");
    
    if( e->KeywordSet( "APPEND")) mode |= fstream::app;

    bool deleteKey = e->KeywordSet( "DELETE");
    
    bool errorKeyword = e->KeywordPresent( 4);
    if( errorKeyword) e->AssureGlobalKW( 4);

    DLong width = defaultStreamWidth;
    BaseGDL* widthKeyword = e->GetKW( 13); // WIDTH
    if( widthKeyword != NULL)
      {
	e->AssureLongScalarKW( 13, width);
      }

    try{
      fileUnits[ lun-1].Open( name, mode, swapEndian, deleteKey, width);
    } 
    catch( GDLException& ex) {
      DString errorMsg = ex.toString()+" Unit: "+i2s( lun)+
	", File: "+fileUnits[ lun-1].Name();
      
      if( !errorKeyword)
	e->Throw( errorMsg);
      
      BaseGDL** err = &e->GetKW( 4);
      
      delete (*err); 
//    if( *err != e->Caller()->Object()) delete (*err); 
      
      *err = new DLongGDL( 1);
      return;
    }

    if( errorKeyword)
      {
	BaseGDL** err = &e->GetKW( 4);
      
// 	if( *err != e->Caller()->Object()) delete (*err); 
	delete (*err); 
      
	*err = new DLongGDL( 0);
      }
  }
  
  void openr( EnvT* e)
  {
    open_lun( e, fstream::in);
  }

  void openw( EnvT* e)
  {
    open_lun( e, fstream::in | fstream::out | fstream::trunc);
  }

  void openu( EnvT* e)
  {
    open_lun( e, fstream::in | fstream::out);
  }
  
  void close_free_lun( EnvT* e, bool freeLun)
  {
    DLong journalLUN = SysVar::JournalLUN();

    // within GDL, always lun+1 is used
    if( e->KeywordSet("ALL"))
      for( int p=maxUserLun; p<maxLun; ++p)
	{
	  if( (journalLUN-1) != p)
	    {
	      fileUnits[ p].Close();
	      if( freeLun) fileUnits[ p].Free();
	    }
	}
    
    if( e->KeywordSet("FILE") || e->KeywordSet("ALL"))
      for( int p=0; p<maxUserLun; ++p)
	{
	  fileUnits[ p].Close();
	  // freeing not necessary as get_lun does not use them
	  //if( freeLun) fileUnits[ p].Free();
	}
    
    int nParam=e->NParam();
    for( int p=0; p<nParam; p++)
      {
	DLong lun;
	e->AssureLongScalarPar( p, lun);
	if( lun > maxLun)
	  e->Throw( "File unit is not within allowed range: "+
		    i2s(lun)+".");
	if( lun < 1)
	  e->Throw( "File unit does not allow this operation."
		    " Unit: "+i2s(lun)+".");

	if( lun == journalLUN)
	  e->Throw(  "Reserved file cannot be closed in this manner. Unit: "+
		     i2s( lun));
	
	fileUnits[ lun-1].Close();
	if( freeLun) fileUnits[ lun-1].Free();
      }
  }

  void close_lun( EnvT* e)
  {
    close_free_lun( e, false);
  }
  
  void free_lun( EnvT* e)
  {
    close_free_lun( e, true);
  }

  void writeu( EnvT* e)
  {
    SizeT nParam=e->NParam( 1);

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    ostream* os;
    bool swapEndian = false;

    bool stdLun = check_lun( e, lun);
    if( stdLun)
      {
	if( lun == 0)
	  e->Throw( "Cannot write to stdin. Unit: "+i2s( lun));

	os = (lun == -1)? &cout : &cerr;
      }
    else
      {
	os = &fileUnits[ lun-1].OStream();
	swapEndian = fileUnits[ lun-1].SwapEndian();
      }

    for( SizeT i=1; i<nParam; i++)
      {
	BaseGDL* p = e->GetParDefined( i);
	p->Write( *os, swapEndian);
      }
  }

  void readu( EnvT* e)
  {
    SizeT nParam=e->NParam( 1);

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    istream* is;
    bool swapEndian = false;

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
	is = &fileUnits[ lun-1].IStream();
	swapEndian = fileUnits[ lun-1].SwapEndian();
      }

    for( SizeT i=1; i<nParam; i++)
      {
	BaseGDL* p = e->GetParDefined( i);
	p->Read( *is, swapEndian);
      }
  }

  void on_error( EnvT* e)
  {
    e->OnError();
  }

  void catch_pro( EnvT* e)
  {
    e->Catch();
  }

  void strput( EnvT* e)
  {
    SizeT nParam = e->NParam( 2);
    
    DStringGDL* dest = dynamic_cast<DStringGDL*>( e->GetParGlobal( 0));
    if( dest == NULL)
      e->Throw( "String expression required in this context: "+
		e->GetParString(0));
    
    DString source;
    e->AssureStringScalarPar( 1, source);
    
    DLong pos = 0;
    if (nParam == 3)
      {
	e->AssureLongScalarPar( 2, pos);
	if (pos < 0) pos = 0;
      }

    SizeT nEl = dest->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
	StrPut((*dest)[ i], source, pos);
  }

  void retall( EnvT* e)
  {
    e->Interpreter()->RetAll();
  }

  void stop( EnvT* e)
  {
    if( e->NParam() > 0) print( e);
    debugMode = DEBUG_STOP;
  }

  void defsysv( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);

    DString sysVarNameFull;
    e->AssureStringScalarPar( 0, sysVarNameFull);
    
    static int existIx = e->KeywordIx( "EXIST");
    if( e->KeywordPresent( existIx))
      {
	if( sysVarNameFull.length() < 2 || sysVarNameFull[0] != '!')
	  {
	    e->SetKW( existIx, new DLongGDL( 0));
	  }
	
	DVar* sysVar = FindInVarList( sysVarList,
				      StrUpCase( sysVarNameFull.substr(1)));
	if( sysVar == NULL)
	  e->SetKW( existIx, new DLongGDL( 0));
	else
	  e->SetKW( existIx, new DLongGDL( 1));
	return;
      }
    else if( nParam < 2)
      e->Throw( "Incorrect number of arguments.");
    
    // here: nParam >= 2
    DLong rdOnly = 0;
    if( nParam >= 3)
      e->AssureLongScalarPar( 2, rdOnly);

    if( sysVarNameFull.length() < 2 || sysVarNameFull[0] != '!')
      e->Throw( "Illegal system variable name: "+sysVarNameFull+".");
    
    // strip "!", uppercase
    DString sysVarName = StrUpCase( sysVarNameFull.substr(1)); 
    
    DVar* sysVar = FindInVarList( sysVarList, sysVarName);
    if( sysVar == NULL)
      {
	// define new
	DVar *newSysVar = new DVar( sysVarName, e->GetPar( 1)->Dup());
	sysVarList.push_back( newSysVar);

	// rdOnly is only set at the first definition
	if( rdOnly != 0)
	  sysVarRdOnlyList.push_back( newSysVar);
	return;
      }

    // re-set
    // make sure type and size are kept
    BaseGDL* oldVar = sysVar->Data();
    BaseGDL* newVar = e->GetPar( 1);
    if( oldVar->Type()       != newVar->Type() ||
	oldVar->N_Elements() != newVar->N_Elements())
      e->Throw( "Conflicting definition for "+sysVarNameFull+".");

    // if struct -> assure equal descriptors
    DStructGDL *oldStruct =  dynamic_cast<DStructGDL*>( oldVar);
    if( oldStruct != NULL)
      {
	// types are same -> static cast
	DStructGDL *newStruct =  static_cast<DStructGDL*>( newVar);

	// note that IDL handles different structs more relaxed
	// ie. just the structure pattern is compared.
	if( *oldStruct->Desc() != *newStruct->Desc())
	  e->Throw( "Conflicting definition for "+sysVarNameFull+".");
      }
	
    DVar* sysVarRdOnly = FindInVarList( sysVarRdOnlyList, sysVarName);
    if( sysVarRdOnly != NULL)
      {
	// rdOnly set and is already rdOnly: do nothing
	if( rdOnly != 0) return; 

	// else complain
	e->Throw( "Attempt to write to a readonly variable: "+
		  sysVarNameFull+".");
      }
    else
      {
	// not read only
	delete oldVar;
	sysVar->Data() = newVar->Dup();

	// only on first definition
	//	if( rdOnly != 0)
	//	  sysVarRdOnlyList.push_back( sysVar);
      }
  }

  // note: this implemetation does not honor all keywords
  void message( EnvT* e)
  {
    SizeT nParam = e->NParam();

    if( nParam == 0) return;

    static int continueIx = e->KeywordIx( "CONTINUE");
    static int infoIx = e->KeywordIx( "INFORMATIONAL");
    static int ioerrorIx = e->KeywordIx( "IOERROR");
    static int nonameIx = e->KeywordIx( "NONAME");
    static int noprefixIx = e->KeywordIx( "NOPREFIX");
    static int noprintIx = e->KeywordIx( "NOPRINT");
    static int resetIx = e->KeywordIx( "RESET");

    bool continueKW = e->KeywordSet( continueIx);
    bool info = e->KeywordSet( infoIx);
    bool ioerror = e->KeywordSet( ioerrorIx);
    bool noname = e->KeywordSet( nonameIx);
    bool noprefix = e->KeywordSet( noprefixIx);
    bool noprint = e->KeywordSet( noprintIx);
    bool reset = e->KeywordSet( resetIx);

    DString msg;
    e->AssureScalarPar<DStringGDL>( 0, msg);

    if( !noname)
      msg = e->Caller()->GetProName() + ": " + msg;

    if( !info)
      {
	DStructGDL* errorState = SysVar::Error_State();
	static unsigned msgTag = errorState->Desc()->TagIndex( "MSG");
	(*static_cast<DStringGDL*>( errorState->Get( msgTag, 0)))[0] = msg;
	
	SysVar::SetErr_String( msg);
      }
	
    if( noprint)
      msg = "";
    
    if( !continueKW && !info)
      throw GDLException( msg, !noprefix);
    
    if( !noprint && !noprefix)
      msg = SysVar::MsgPrefix() + msg;

    if( !info || (SysVar::Quiet() == 0))
      cout << msg << endl;
  }

  void byteorder( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);

    //    static int sswapIx = e->KeywordIx( "SSWAP");
    static int lswapIx = e->KeywordIx( "LSWAP");
    static int l64swapIx = e->KeywordIx( "L64SWAP");
    static int ifBigIx = e->KeywordIx( "SWAP_IF_BIG_ENDIAN");
    static int ifLittleIx = e->KeywordIx( "SWAP_IF_LITTLE_ENDIAN");
    static int ntohlIx = e->KeywordIx( "NTOHL");
    static int ntohsIx = e->KeywordIx( "NTOHS");
    static int htonlIx = e->KeywordIx( "HTONL");
    static int htonsIx = e->KeywordIx( "HTONS");

    bool lswap = e->KeywordSet( lswapIx);
    bool l64swap = e->KeywordSet( l64swapIx);
    bool ifBig = e->KeywordSet( ifBigIx);
    bool ifLittle = e->KeywordSet( ifLittleIx);

    // to-from network conversion (big-endian)
    bool ntohl = e->KeywordSet( ntohlIx);
    bool ntohs = e->KeywordSet( ntohsIx);
    bool htonl = e->KeywordSet( htonlIx);
    bool htons = e->KeywordSet( htonsIx);

    if( ifBig && !BigEndian()) return;
    if( ifLittle && BigEndian()) return;

    if( BigEndian() && (ntohl || ntohs || htonl || htons)) return;

    for( DLong p=nParam-1; p>=0; --p)
      {
	BaseGDL* par = e->GetParDefined( p);
	if( !e->GlobalPar( p))
	  e->Throw( "Expression must be named variable in this context: "+
		    e->GetParString(p));		    

	SizeT nBytes = par->NBytes();
	char* addr = static_cast<char*>(par->DataAddr());
	
	SizeT swapSz = 2; 
	if( l64swap)
	  swapSz = 8;
	else if( lswap || ntohl || htonl)
	  swapSz = 4;

	if( nBytes % swapSz != 0)
	  e->Throw( "Operand's size must be a multiple of swap "
		    "datum size: " + e->GetParString(p));		    
	    
	SizeT nSwap = nBytes / swapSz;

	for( SizeT i=0; i<nSwap; ++i)
	  {
	    for( SizeT s=0; s < (swapSz/2); ++s)
	      {
		char tmp = *(addr+i*swapSz+s);
		*(addr+i*swapSz+s) = *(addr+i*swapSz+swapSz-1-s);
		*(addr+i*swapSz+swapSz-1-s) = tmp;
	      }
	  }
      }
  }

  void setenv_pro( EnvT* e)
  {
    SizeT nParam = e->NParam();

    DStringGDL* name = e->GetParAs<DStringGDL>(0);
    SizeT nEnv = name->N_Elements();

    for(SizeT i=0; i < nEnv; ++i){
      DString strEnv = (*name)[i];
      long len = strEnv.length();
      long pos = strEnv.find_first_of("=", 0); 
      if( pos == string::npos) continue;   
      DString strArg = strEnv.substr(pos+1, len - pos - 1);
      strEnv = strEnv.substr(0, pos);
      int ret = setenv(strEnv.c_str(), strArg.c_str(), 1);
    }
  } 

  void struct_assign_pro( EnvT* e)
  {
    SizeT nParam=e->NParam( 2);
 
    DStructGDL* source = e->GetParAs<DStructGDL>(0);
    DStructGDL* dest   = e->GetParAs<DStructGDL>(1);
  
    static int nozeroIx = e->KeywordIx("NOZERO");
    bool nozero = e->KeywordSet( nozeroIx); 

    static int verboseIx = e->KeywordIx("VERBOSE");
    bool verbose = e->KeywordSet( verboseIx);

    string sourceName = (*source).Desc()->Name();

    SizeT nTags = 0;

    // array of struct
    SizeT nElements = source->N_Elements();
    SizeT nDestElements = dest->N_Elements();
    if( nElements > nDestElements)
      nElements = nDestElements;

    // zero out the destination
    if( !nozero)
       (*dest).Clear();

    nTags = (*source).Desc()->NTags();

    // copy the stuff
    for(int t=0; t < nTags; ++t)
      {    
	string sourceTagName = (*source).Desc()->TagName(t);
	int ix = (*dest).Desc()->TagIndex( sourceTagName );
	if( ix >= 0)
	  {
	    SizeT nTagElements = source->Get( t, 0)->N_Elements();
	    SizeT nTagDestElements = dest->Get( t, 0)->N_Elements();

	    if( verbose) 
	      {
		if( nTagElements > nTagDestElements)
		  Warning( "STRUCT_ASSIGN: " + sourceName + 
			   " tag " + sourceTagName + 
			   " is longer than destination. "
			   "The end will be clipped.");
		else if( nTagElements < nTagDestElements)
		  Warning( "STRUCT_ASSIGN: " + sourceName + 
			   " tag " + sourceTagName + 
			   " is shorter than destination. "
			   "The end will be zero filled.");
	      }

	    if( nTagElements > nTagDestElements)
		nTagElements = nTagDestElements;

	    for( SizeT a=0; a< nElements; ++a)
	      dest->Get( t, a)->Assign( source->Get( t, a), nTagElements);
	  }
	else 
	  if(verbose)
	    Warning( "STRUCT_ASSIGN: Destination lacks " + sourceName +
		     " tag " + sourceTagName + ". Not copied.");
      }
  }

  void spawn_pro( EnvT* e)
  {
    SizeT nParam = e->NParam();

    static int countIx = e->KeywordIx( "COUNT");
    bool countKeyword = e->KeywordPresent( countIx);
    if( countKeyword) e->AssureGlobalKW( countIx);

    static int pidIx = e->KeywordIx( "PID");
    bool pidKeyword = e->KeywordPresent( pidIx);
    if( pidKeyword) e->AssureGlobalKW( pidIx);
    
    static int exit_statusIx = e->KeywordIx( "EXIT_STATUS");
    bool exit_statusKeyword = e->KeywordPresent( exit_statusIx);
    if( exit_statusKeyword) e->AssureGlobalKW( exit_statusIx);

    static int shIx = e->KeywordIx( "SH");
    bool shKeyword = e->KeywordSet( shIx);
    
    static int noshellIx = e->KeywordIx( "NOSHELL");
    bool noshellKeyword = e->KeywordSet( noshellIx);
    
    string shellCmd;
    if( shKeyword) 
      shellCmd = "/bin/sh"; // must be there if POSIX
    else
      {
	shellCmd= getenv("SHELL");
	if(shellCmd == "")
	  e->Throw( "Error managing child process. "
		    "Environment variable SHELL not set.");
      }

    if( nParam == 0)
      { 
	system( shellCmd.c_str());
	if( countKeyword)
	  e->SetKW( countIx, new DLongGDL( 0));
	return;
      }

    DStringGDL* command = e->GetParAs<DStringGDL>( 0);
    DString cmd = (*command)[0];

    const int bufSize = 1024;
    char buf[ bufSize];

    if( nParam > 1) e->AssureGlobalPar( 1);
    if( nParam > 2) e->AssureGlobalPar( 2);

    int coutP[2];
    if( nParam > 1 && pipe(coutP)) return;	

    int cerrP[2];
    if( nParam > 2 && pipe(cerrP)) return;	
	
    pid_t pid = fork(); // *** fork
    if( pid == -1) // error in fork
      {
	close( coutP[0]); close( coutP[1]);
	if( nParam > 2) { close( cerrP[0]); close( cerrP[1]);}
	return;
      }

    if( pid == 0) // we are child
      {
	if( nParam > 1) dup2(coutP[1], 1); // cout
	if( nParam > 2) dup2(cerrP[1], 2); // cerr

	if( nParam > 1) { close( coutP[0]); close( coutP[1]);}
	if( nParam > 2) { close( cerrP[0]); close( cerrP[1]);}

	if( noshellKeyword)
	  {
	    SizeT nArg = command->N_Elements();
	    char** argv = new char*[ nArg+1];
	    argv[ nArg] = NULL;
	    for( SizeT i=0; i<nArg; ++i)
	      argv[i] = const_cast<char*>((*command)[i].c_str());
		
	    execvp( cmd.c_str(), argv);

	    delete[] argv; // only executes if exec fails
	  }
	else
	  execl( shellCmd.c_str(), shellCmd.c_str(), "-c", 
		 cmd.c_str(), (char *) NULL);

	Warning( "SPAWN: Error managing child process.");
	_exit(1); // error in exec
      }
    else // we are parent
      {
	if( pidKeyword)
	  e->SetKW( pidIx, new DLongGDL( pid));

	if( nParam > 1) close( coutP[1]);
	if( nParam > 2) close( cerrP[1]);

	FILE *coutF, *cerrF;
	if( nParam > 1) 
	  {
	    coutF = fdopen( coutP[0], "r");
	    if( coutF == NULL) close( coutP[0]);
	  }
	if( nParam > 2) 
	  {
	    cerrF = fdopen( cerrP[0], "r");
	    if( cerrF == NULL) close( cerrP[0]);
	  }

	vector<DString> outStr;
	vector<DString> errStr;
	    
	// read cout
	if( nParam > 1 && coutF != NULL)
	  {
	    while( fgets(buf, bufSize, coutF) != NULL)
	      {
		SizeT len = strlen( buf);
		if( len != 0 && buf[ len-1] == '\n') 
		  buf[ len-1] = 0;
		outStr.push_back( DString( buf));
	      }
	    fclose( coutF);
	  }

	// read cerr
	if( nParam > 2 && cerrF != NULL) 
	  {
	    while( fgets(buf, bufSize, cerrF) != NULL)
	      {
		SizeT len = strlen( buf);
		if( len != 0 && buf[ len-1] == '\n') 
		  buf[ len-1] = 0;
		errStr.push_back( DString( buf));
	      }
	    fclose( cerrF);
	  }

	// wait until child terminates
	int status;
	pid_t wpid  = wait( &status);
	
	if( exit_statusKeyword)
	  e->SetKW( exit_statusIx, new DLongGDL( status));
	    
	SizeT nLines = 0;
	if( nParam > 1)
	  {
	    DStringGDL* result;
	    nLines = outStr.size();
	    if( nLines == 0)
	      result = new DStringGDL("");
	    else 
	      {
		result = new DStringGDL( dimension( nLines), 
					 BaseGDL::NOZERO);
		for( SizeT l=0; l<nLines; ++l)
		  (*result)[ l] = outStr[ l];
	      }
	    e->SetPar( 1, result);
	  }

	if( countKeyword)
	  e->SetKW( countIx, new DLongGDL( nLines));
	    
	if( nParam > 2)
	  {
	    DStringGDL* errResult;
	    SizeT nErrLines = errStr.size();
	    if( nErrLines == 0)
	      errResult = new DStringGDL("");
	    else 
	      {
		errResult = new DStringGDL( dimension( nErrLines), 
					    BaseGDL::NOZERO);
		for( SizeT l=0; l<nErrLines; ++l)
		  (*errResult)[ l] = errStr[ l];
	      }
	    e->SetPar( 2, errResult);
	  }

	return;
      }
  }
  
} // namespace
