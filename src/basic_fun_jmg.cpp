/***************************************************************************
                          basic_fun_jmg.cpp  -  basic GDL library function
                             -------------------
    begin                : 2004
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net
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
#include <sys/stat.h>

#include "datatypes.hpp"
#include "envt.hpp"
#include "basic_fun.hpp"
#include "io.hpp"
#include "dinterpreter.hpp"
#include "objects.hpp"
#include "basic_fun_jmg.hpp"
#include "nullgdl.hpp"


//#define GDL_DEBUG
#undef GDL_DEBUG

namespace lib {

  using namespace std;
  using namespace antlr;
 
  BaseGDL* isa_fun( EnvT* e) 
  {
    if (e->NParam() == 0) e->Throw("Requires at least one argument !");

    DString type;
    BaseGDL *p0;
    BaseGDL *p1;
    DStringGDL *p1Str;
    DString p1S = "";
    int nb_kw=0;

    bool secPar = false;
    SizeT n_elem;
    SizeT rank;
    int debug=0;

    string structName;
    string objectName;

    bool ARRAY_KW_B = false;
    bool NULL_KW_B = false;
    bool NUMBER_KW_B = false;
    bool SCALAR_KW_B = false;

    bool isARRAY = false;
    bool isFILE = false;
    bool isNULL = false;
    bool isSCALAR = false;

    static int array_kw = e->KeywordIx("ARRAY");
    static int null_kw = e->KeywordIx("NULL");
    static int number_kw = e->KeywordIx("NUMBER");
    static int scalar_kw = e->KeywordIx("SCALAR");

    if (e->KeywordSet(array_kw))  { ARRAY_KW_B = true;}
    if (e->KeywordSet(null_kw))   { NULL_KW_B = true;}
    if (e->KeywordSet(number_kw)) { NUMBER_KW_B = true;}
    if (e->KeywordSet(scalar_kw)) { SCALAR_KW_B = true; }
   
    // new since 8.4

    bool isBOOLEAN = false;
    bool isINTEGER = false;
    bool isFLOAT = false;
    bool isCOMPLEX = false;
    bool isSTRING = false;

    bool BOOLEAN_KW_B = false;
    bool INTEGER_KW_B = false;
    bool FLOAT_KW_B = false;
    bool COMPLEX_KW_B = false;
    bool STRING_KW_B = false;

    static int boolean_kw = e->KeywordIx("BOOLEAN");
    static int integer_kw = e->KeywordIx("INTEGER");
    static int float_kw = e->KeywordIx("FLOAT");
    static int complex_kw = e->KeywordIx("COMPLEX");
    static int string_kw = e->KeywordIx("STRING"); 

    if (e->KeywordSet(boolean_kw)) { BOOLEAN_KW_B = true;}
    if (e->KeywordSet(integer_kw)) { INTEGER_KW_B = true;}
    if (e->KeywordSet(float_kw))   { FLOAT_KW_B = true;}
    if (e->KeywordSet(complex_kw)) { COMPLEX_KW_B = true; }
    if (e->KeywordSet(string_kw))  { STRING_KW_B = true; }

    // /FILE keyword not ready
    bool FILE_KW_B = false;
    static int file_kw = e->KeywordIx("FILE");
    if (e->KeywordSet(file_kw)) { FILE_KW_B = true;}
    if (FILE_KW_B) {
      string txt="(file keyword - ISA() not ready ! Please contribute !!";
      e->Throw(txt);
    }
	
    if (SCALAR_KW_B && ARRAY_KW_B) {
      e->Throw("Keywords ARRAY and SCALAR are mutually exclusive.");
    }

    if (NULL_KW_B) {
      string txt="Keywords NULL and ";
      if (ARRAY_KW_B) e->Throw(txt+"ARRAY are mutually exclusive.");
      if (FILE_KW_B) e->Throw(txt+"FILE are mutually exclusive.");
      if (SCALAR_KW_B) e->Throw(txt+"SCALAR are mutually exclusive.");
      if (NUMBER_KW_B) e->Throw(txt+"NUMBER are mutually exclusive.");

      if (BOOLEAN_KW_B) e->Throw(txt+"BOOLEAN are mutually exclusive.");
      if (INTEGER_KW_B) e->Throw(txt+"INTEGER are mutually exclusive.");
      if (FLOAT_KW_B) e->Throw(txt+"FLOAT are mutually exclusive.");
      if (COMPLEX_KW_B) e->Throw(txt+"COMPLEX are mutually exclusive.");
      if (STRING_KW_B) e->Throw(txt+"STRING are mutually exclusive.");
    }

    if (BOOLEAN_KW_B) {
      string txt="Keywords BOOLEAN and ";
      if (INTEGER_KW_B) e->Throw(txt+"INTEGER are mutually exclusive.");
      if (FLOAT_KW_B) e->Throw(txt+"FLOAT are mutually exclusive.");
      if (COMPLEX_KW_B) e->Throw(txt+"COMPLEX are mutually exclusive.");
      if (STRING_KW_B) e->Throw(txt+"STRING are mutually exclusive.");
    }
    if (INTEGER_KW_B) {
      string txt="Keywords INTEGER and ";
      if (FLOAT_KW_B) e->Throw(txt+"FLOAT are mutually exclusive.");
      if (COMPLEX_KW_B) e->Throw(txt+"COMPLEX are mutually exclusive.");
      if (STRING_KW_B) e->Throw(txt+"STRING are mutually exclusive.");
    }
    if (FLOAT_KW_B) {
      string txt="Keywords FLOAT and ";
      if (COMPLEX_KW_B) e->Throw(txt+"COMPLEX are mutually exclusive.");
      if (STRING_KW_B) e->Throw(txt+"STRING are mutually exclusive.");
    }
    if (COMPLEX_KW_B) {
      string txt="Keywords COMPLEX and ";
      if (STRING_KW_B) e->Throw(txt+"STRING are mutually exclusive.");
    }

    //first par.
    p0 = e->GetPar(0);

    bool isNUMBER = true;
    bool res = true;
    // (1: boolean, 2: all integer like, 3 : float, 4 : complex, 5: string)
    int sub_type=0;

    if (p0 == NULL) {
      type="UNDEFINED";
      res = false;
      isNUMBER=false;
    } else {
      n_elem = p0->N_Elements();
      rank = p0->Rank();
      if (debug) cout << "type : "<< p0->Type() << ", Rank : "<< rank << endl;
      
      switch (p0->Type())
	{
	case GDL_UNDEF: type="UNDEFINED"; isNUMBER=false; res=false; break; 
	case GDL_BYTE: type="BYTE"; sub_type=1; break;
	case GDL_INT: type="INT"; sub_type=2; break;
	case GDL_LONG: type="LONG"; sub_type=2; break;
	case GDL_FLOAT: type="FLOAT"; sub_type=3; break;
	case GDL_DOUBLE: type="DOUBLE"; sub_type=3; break;
	case GDL_COMPLEX: type="COMPLEX"; sub_type=4; break;
	case GDL_STRING: type="STRING"; sub_type=5; isNUMBER=false; break;
	case GDL_STRUCT: type="STRUCT"; isNUMBER=false; break;
	case GDL_COMPLEXDBL: type="DCOMPLEX"; sub_type=4; break;
	case GDL_PTR: type="POINTER"; isNUMBER=false; break;
	case GDL_OBJ: type="OBJREF"; isNUMBER=false; break;
	case GDL_UINT: type="UINT"; sub_type=2; break;
	case GDL_ULONG: type="ULONG"; sub_type=2; break;
	case GDL_LONG64: type="LONG64"; sub_type=2; break;
	case GDL_ULONG64: type="ULONG64"; sub_type=2; break;

	default: e->Throw("This should never happen, please report");
	}
    }

    if(type == "POINTER"){
      DPtrGDL* ptr = static_cast<DPtrGDL*>(p0);
      DPtr ptrID = (*ptr)[0];
      if(ptrID == 0) res=false; else res=true;
    }

    if(type == "STRUCT"){
      //cout << "struct" << endl;
      rank=1; // alway array following ISA() doc.
      DStructGDL* str = static_cast<DStructGDL*>(p0);	   
      if(str->Desc()->IsUnnamed()) structName="ANONYMOUS"; else structName = str->Desc()->Name();
    }

    if(type == "OBJREF"){
      rank=1; // alway array following ISA() doc.
      //cout << "OBJREF" << endl;
      DObjGDL* obj = static_cast<DObjGDL*>(p0);
      DObj objID = (*obj)[0];
      if(objID == 0) res = false; else res = true;

      DStructGDL* oStructGDL= GDLInterpreter::GetObjHeapNoThrow(objID);
      if( oStructGDL != NULL) {
	BaseGDL* objRef = DInterpreter::GetObjHeap(objID);
	DStructGDL* str = static_cast<DStructGDL*>(objRef);
	if(str->Desc()->IsUnnamed())
	  {
	    objectName="Anonymous"; 
	  }
	else {
	  objectName = str->Desc()->Name();
	}
	// cout << objectName << endl;
      }
    }

    //second par.
    p1 = e->GetPar(1);
    if (p1 != NULL){

      if (p1->Type() != GDL_STRING)
	e->Throw("String expression required in this context:"+e->GetParString(1));

      if (p1->N_Elements() > 1)
	e->Throw("Expression must be a scalar or 1 element array in this context"+e->GetParString(1));

      p1Str = static_cast<DStringGDL*>(p1->Convert2(GDL_STRING,BaseGDL::COPY));
      transform((*p1Str)[0].begin(), (*p1Str)[0].end(),(*p1Str)[0].begin(), ::toupper);
      
      if (type == (*p1Str)[0]) res = true; else res = false;

      debug=0;
      if (debug) cout << type << " " << (*p1Str)[0] << " " << res << endl;
	
      if(type == "STRUCT"){ if(structName == (*p1Str)[0]) res = true;}
      if(type == "OBJREF"){ if(objectName == (*p1Str)[0]) res = true;}
    }
	
    if(type != "UNDEFINED"){

      if (sub_type == 1) isBOOLEAN = true;
      if (sub_type == 2) isINTEGER = true;
      if (sub_type == 3) isFLOAT   = true;
      if (sub_type == 4) isCOMPLEX = true;
      if (sub_type == 5) isSTRING  = true;

      if(BOOLEAN_KW_B && res) { res = res && isBOOLEAN ;}
      if(INTEGER_KW_B && res) { res = res && isINTEGER ;}
      if(FLOAT_KW_B && res) { res = res && isFLOAT ;}
      if(COMPLEX_KW_B && res) { res = res && isCOMPLEX ;}
      if(STRING_KW_B && res) { res = res && isSTRING  ;}

      if(NULL_KW_B && res){
	res = false;
      }

      if(SCALAR_KW_B && res){
	if (rank == 0) isSCALAR=true; else isSCALAR=false;
	res = res && isSCALAR; 
      }

      if(NUMBER_KW_B && res){
	//cout<<"Number"<<endl;
	res = res && isNUMBER;
      }

      if(ARRAY_KW_B && res){
	//cout<<"Array"<<endl;
	if (rank > 0) isARRAY = true; else isARRAY = false;
	res = res && isARRAY;
      }

      if(NULL_KW_B && res){
	res = false;
      }
    } else {
      // we have two cases : undefined variable OR variable set to !null
      if (NULL_KW_B){
	if (p0 == NULL) res = false; 
	else {
	  res = true;
	  res = res && (!ARRAY_KW_B) && (!SCALAR_KW_B) && (!NUMBER_KW_B); 
	}
      } else {
	res = res && (!ARRAY_KW_B) && (!SCALAR_KW_B) && (!NUMBER_KW_B); 
      }
    }
    
    if (res) return new DByteGDL(1);
    return new DByteGDL(0);
  }

  BaseGDL* typename_fun( EnvT* e) 
  {
    DString type="";
    BaseGDL* p0 = e->GetPar(0);

    // we manage Undefined here, !null is managed below
    if (p0 == NULL) return new DStringGDL("UNDEFINED");
    
    int redo=0;

    switch (p0->Type())
      {
	// this is different that (p0 == NULL), here input is set to !null
      case GDL_UNDEF: type="UNDEFINED"; break;
      case GDL_BYTE: type="BYTE"; break;
      case GDL_INT: type="INT"; break;
      case GDL_LONG: type="LONG"; break;
      case GDL_FLOAT: type="FLOAT"; break;
      case GDL_DOUBLE: type="DOUBLE"; break;
      case GDL_COMPLEX: type="COMPLEX"; break;
      case GDL_STRING: type="STRING"; break;
      case GDL_STRUCT: redo=1; break;
      case GDL_COMPLEXDBL: type="DCOMPLEX"; break;
      case GDL_PTR: type="POINTER"; break;
      case GDL_OBJ:redo=1; break;
      case GDL_UINT: type="UINT"; break;
      case GDL_ULONG: type="ULONG"; break;
      case GDL_LONG64: type="LONG64"; break;
      case GDL_ULONG64: type="ULONG64"; break;

      default: e->Throw("This should never happen, please report");
      }

    if (redo) {
      //cout << "here we are " <<p0->Type() << endl;
      if (p0->Type() == GDL_STRUCT) {
        DStructGDL* s = static_cast<DStructGDL*> (p0);
	// AC 2018-Feb-02 : order is : Array (struct), name, anon
	bool debug=false;
	if (debug) {
	  cout << "Rank  :" << p0->Rank() << endl;
	  cout << "Dim   :" << p0->Dim() << endl;
	  cout << "Size  :" << p0->Size() << endl;
	  cout << "StrictScalar :" << p0->StrictScalar() << endl;
	}
	if (p0->Dim(0) > 1) {
	  type = "STRUCT";
	} else {
	  if (s->Desc()->IsUnnamed()) {
	    type = "ANONYMOUS";
	  } else {
	    type = s->Desc()->Name();
	  }
	}
      }

      // here we manage : {Objects, LIST, HASH}
      if (p0->Type() == GDL_OBJ) {

        // see case in "basic_pro.cpp", in help_item()
        if (!p0->StrictScalar()) {
          type = "OBJREF";
        } else {

          DObj s = (*static_cast<DObjGDL*> (p0))[0]; // is StrictScalar()
          if (s != 0) // no overloads for null object
          {
            DStructGDL* oStructGDL = GDLInterpreter::GetObjHeapNoThrow(s);
            if (oStructGDL->Desc()->IsUnnamed())
              e->Throw("We don't know how to be here (unnamed Obj/List/Hash), please provide exemple !");

            type = oStructGDL->Desc()->Name();
          } else {
            type = "UNDEFINED";
          }
        }
      }
    }
    return new DStringGDL(type);
    
  }

  BaseGDL* size_fun( EnvT* e) 
  {
    static int L64Ix = e->KeywordIx( "L64");
    static int dimIx = e->KeywordIx( "DIMENSIONS");
    static int FILE_LUNIx = e->KeywordIx( "FILE_LUN");
    static int FILE_OFFSETIx = e->KeywordIx( "FILE_OFFSET");
    static int N_DIMENSIONSIx = e->KeywordIx( "N_DIMENSIONS");
    static int N_ELEMENTSIx = e->KeywordIx( "N_ELEMENTS");
    static int STRUCTUREIx = e->KeywordIx( "STRUCTURE");
    static int SNAMEIx = e->KeywordIx( "SNAME");
    static int TNAMEIx = e->KeywordIx( "TNAME");
    static int TYPEIx = e->KeywordIx( "TYPE");

    e->NParam( 1); // might be GDL_UNDEF, but must be given

    // BaseGDL* p0 = e->GetParDefined( 0); //, "SIZE");
    BaseGDL* p0 = e->GetPar( 0); //, "SIZE");
    
    // managing exclusive keywords (all but L64)
    int nb_keywords_set=0;
    if (e->KeywordSet(dimIx)) nb_keywords_set++;
    if (e->KeywordSet(FILE_LUNIx)) nb_keywords_set++;
    if (e->KeywordSet(FILE_OFFSETIx)) nb_keywords_set++;
    if (e->KeywordSet(N_DIMENSIONSIx)) nb_keywords_set++;
    if (e->KeywordSet(N_ELEMENTSIx)) nb_keywords_set++;
    if (e->KeywordSet(STRUCTUREIx)) nb_keywords_set++;
    if (e->KeywordSet(SNAMEIx)) nb_keywords_set++;
    if (e->KeywordSet(TNAMEIx)) nb_keywords_set++;
    if (e->KeywordSet(TYPEIx)) nb_keywords_set++;

    if (nb_keywords_set > 1) e->Throw("Conflicting keywords.");

    SizeT nEl = 0;
    SizeT Rank = 0;
    SizeT LogicalRank = 0;
    SizeT vType = GDL_UNDEF;

    if (p0 != NULL) {
      nEl = p0->N_Elements();
      Rank = p0->Rank();
      LogicalRank = p0->Rank();
      vType = p0->Type();
    }

    bool forceL64=false;
    if (nEl > 2147483647UL) forceL64=true;

    bool isObjectContainer = false;
    if( vType == GDL_OBJ)
      {
	DObjGDL* p0Obj = static_cast<DObjGDL*>(p0);
	if( p0Obj->StrictScalar())
	  {
	    DStructGDL* oStructGDL= GDLInterpreter::GetObjHeapNoThrow( (*p0Obj)[0]);
	    if( oStructGDL != NULL) // if object not valid -> default behaviour
	      {  
		DStructDesc* desc = oStructGDL->Desc();

		if( desc->IsParent("LIST"))
		  {
		    isObjectContainer = true;
		  }
		if( desc->IsParent("HASH"))
		  {
		    isObjectContainer = true;
		  }
	      }
	  }
      }

    if( isObjectContainer)
      {
	LogicalRank = 1;
      }
    
    // DIMENSIONS
    if( e->KeywordSet( dimIx)) { 
      if( LogicalRank == 0) {
	if( e->KeywordSet(L64Ix) || forceL64)
	  return new DLong64GDL( 0);
	else
	  return new DLongGDL( 0);
      }
      dimension dim( LogicalRank);

      if( e->KeywordSet(L64Ix) || forceL64) { // L64
	DLong64GDL* res = new DLong64GDL( dim, BaseGDL::NOZERO);
	(*res)[0] = 0;
	for( SizeT i=0; i<Rank; ++i) (*res)[ i] = p0->Dim(i);
	if( isObjectContainer)
	  (*res)[ 0] = nEl;
	return res;
      } else {
	DLongGDL* res = new DLongGDL( dim, BaseGDL::NOZERO);
	(*res)[0] = 0;
	for( SizeT i=0; i<Rank; ++i) (*res)[ i] = p0->Dim(i);
	if( isObjectContainer)
	  (*res)[ 0] = nEl;
	return res;
      }
    }

    // FILE_LUN
    string txt="Sorry, keyword ";
    if( e->KeywordSet(FILE_LUNIx)) {    
      e->Throw(txt+"/FILE_LUN not supported yet, please contribute.");
    }
    if( e->KeywordSet(FILE_OFFSETIx))  {
      e->Throw(txt+"/FILE_OFFSET not supported yet, please contribute.");
    }

    // N_DIMENSIONS
    if( e->KeywordSet(N_DIMENSIONSIx)) {
      return new DLongGDL( LogicalRank);
    }

    //N_ELEMENTS
    if( e->KeywordSet(N_ELEMENTSIx)) { 
      if( e->KeywordSet(L64Ix) || forceL64)
	return new DLong64GDL(nEl);
      else 
	return new DLongGDL( nEl);
    }

    // STRUCTURE
    if( e->KeywordSet(STRUCTUREIx)) { 

      DStructGDL* res;

      if (e->KeywordSet(L64Ix) || forceL64 ) {
	res = new DStructGDL( "IDL_SIZE64");
      } else {
	res = new DStructGDL( "IDL_SIZE");
      }
      
      if ( p0 == NULL) {
	res->InitTag("TYPE_NAME", DStringGDL("UNDEFINED"));
	return res;
      }
      
      DString tname;
      DString sname;
      if (vType == GDL_STRUCT) {
	tname = "STRUCT";
	DStructGDL* s = static_cast<DStructGDL*>( p0);
	if (s->Desc()->IsUnnamed())
	  sname = "";
	else
	  sname = s->Desc()->Name();
      } else {
	tname = p0->TypeStr();
	sname = "";
      }

      res->InitTag("TYPE_NAME", DStringGDL(tname));
      res->InitTag("STRUCTURE_NAME", DStringGDL(sname));
      res->InitTag("TYPE", DIntGDL(vType));
      res->InitTag("FILE_LUN", DIntGDL(0));
      if (e->KeywordSet(L64Ix) || forceL64) {
	res->InitTag("FILE_OFFSET", DLong64GDL(0));
	res->InitTag("N_ELEMENTS",  DLong64GDL(nEl));
      } else {
	res->InitTag("FILE_OFFSET", DLongGDL(0));
	res->InitTag("N_ELEMENTS",  DLongGDL(nEl));
      }
      res->InitTag("N_DIMENSIONS",  DLongGDL(Rank));

      // Initialize dimension values to 0
      if (e->KeywordSet(L64Ix) || forceL64 ) {
	DLong64GDL *dims_res = new DLong64GDL(dimension(MAXRANK), BaseGDL::ZERO);
	for( SizeT i=Rank; i<MAXRANK; ++i) (*dims_res)[ i] = 0;
	for( SizeT i=0; i<Rank; ++i) (*dims_res)[ i] = p0->Dim(i);
	res->InitTag("DIMENSIONS",  *dims_res);
      } else {
	DLongGDL *dims_res = new DLongGDL(dimension(MAXRANK), BaseGDL::ZERO);	
	for( SizeT i=Rank; i<MAXRANK; ++i) (*dims_res)[ i] = 0;
	for( SizeT i=0; i<Rank; ++i) (*dims_res)[ i] = p0->Dim(i);
	res->InitTag("DIMENSIONS",  *dims_res);
      }

      return res;
      //e->Throw( "STRUCTURE not supported yet.");
    }
    
    // SNAME
    if( e->KeywordSet(SNAMEIx)) {
      DString sname="";
      if (vType == GDL_STRUCT) {
	DStructGDL* s = static_cast<DStructGDL*>( p0);
	if (!s->Desc()->IsUnnamed()) sname = s->Desc()->Name();
      }
      return new DStringGDL(sname);
    }

    // TNAME
    if( e->KeywordSet(TNAMEIx)) {
      if( p0 == NULL)
	return new DStringGDL( "UNDEFINED");
      return new DStringGDL( p0->TypeStr());
    }
    
    // TYPE
    if( e->KeywordSet(TYPEIx)) { 
      return new DLongGDL( vType );
    }

    // the general case without keyword ...

    dimension dim( 3 + LogicalRank);
    
    if( e->KeywordSet(L64Ix) || forceL64 ) {
      DLong64GDL* res = new DLong64GDL( dim, BaseGDL::NOZERO);
      (*res)[ 0] = LogicalRank;
      for( SizeT i=0; i<Rank; ++i) (*res)[ i+1] = p0->Dim(i);
      if( isObjectContainer)
	(*res)[ 0+1] = nEl;
      (*res) [ LogicalRank+1] = vType;
      (*res) [ LogicalRank+2] = nEl;
      
      return res;
    } else {
      DLongGDL* res = new DLongGDL( dim, BaseGDL::NOZERO);
      (*res)[ 0] = LogicalRank;
      for( SizeT i=0; i<Rank; ++i) (*res)[ i+1] = p0->Dim(i);
      if( isObjectContainer)
	(*res)[ 0+1] = nEl;
      (*res) [ LogicalRank+1] = vType;
      (*res) [ LogicalRank+2] = nEl;
      
      return res;
    }
    
    return new DIntGDL( 0); // default for not supported
  }

  BaseGDL* fstat_fun( EnvT* e) 
  { 
    e->NParam( 1);//, "FSTAT");

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    if( lun < -2 || lun > maxLun)
      throw GDLException( e->CallingNode(), 
			  " File unit is not within allowed range.");

    SizeT size;
    bool big = false;

    if (lun > 0)
    {
      if (fileUnits[ lun - 1].IsOpen() && !fileUnits[ lun - 1].Compress() ) //due to bug in gzTell, we DO NOT WANT to get the size here.
      {
        size = fileUnits[ lun - 1].Size();
        big = (DLong(size) != size);
      }
    }

    DStructGDL* fstat;
    if (big) fstat = new DStructGDL( "FSTAT64");
    else fstat = new DStructGDL( "FSTAT");
   
    fstat->InitTag("UNIT", DLongGDL( lun));

    if( lun == -2)
      {
	fstat->InitTag("NAME", DStringGDL( "<stderr>"));
	// fstat->InitTag("SIZE", DLongGDL( 0)); 
	fstat->InitTag("OPEN", DByteGDL( 1)); 
	fstat->InitTag("ISATTY", DByteGDL( 1)); 
	// fstat->InitTag("ISAGUI", DByteGDL( 0)); 
	fstat->InitTag("INTERACTIVE", DByteGDL( 1)); 
	// fstat->InitTag("XDR", DByteGDL( 0)); 
	// fstat->InitTag("COMPRESS", DByteGDL( 0)); 
	// fstat->InitTag("READ", DByteGDL( 0)); 
	fstat->InitTag("WRITE", DByteGDL( 1)); 
      }
    else if( lun == -1)
      {
	fstat->InitTag("NAME", DStringGDL( "<stdout>"));
	// fstat->InitTag("SIZE", DLongGDL( 0)); 
	fstat->InitTag("OPEN", DByteGDL( 1)); 
	fstat->InitTag("ISATTY", DByteGDL( 1)); 
	// fstat->InitTag("ISAGUI", DByteGDL( 0)); 
	fstat->InitTag("INTERACTIVE", DByteGDL( 1)); 
	// fstat->InitTag("XDR", DByteGDL( 0)); 
	// fstat->InitTag("COMPRESS", DByteGDL( 0)); 
	// fstat->InitTag("READ", DByteGDL( 0)); 
	fstat->InitTag("WRITE", DByteGDL( 1)); 
      }
    else if( lun == 0)
      {
	fstat->InitTag("NAME", DStringGDL( "<stdin>"));
	// fstat->InitTag("SIZE", DLongGDL( 0)); 
	fstat->InitTag("OPEN", DByteGDL( 1)); 
	fstat->InitTag("ISATTY", DByteGDL( 1)); 
	// fstat->InitTag("ISAGUI", DByteGDL( 0)); 
	fstat->InitTag("INTERACTIVE", DByteGDL( 1)); 
	// fstat->InitTag("XDR", DByteGDL( 0)); 
	// fstat->InitTag("COMPRESS", DByteGDL( 0)); 
	fstat->InitTag("READ", DByteGDL( 1)); 
	// fstat->InitTag("WRITE", DByteGDL( 0)); 
      }
    else
      { // normal file
	GDLStream& actUnit = fileUnits[ lun-1];

	if( !actUnit.IsOpen()) 
	  return fstat; // OPEN tag is init to zero (SpDByte::GetInstance())

	struct stat buffer;
	int status = stat(actUnit.Name().c_str(), &buffer);

	fstat->InitTag("NAME", DStringGDL( actUnit.Name()));
	if (big) fstat->InitTag("SIZE", DLong64GDL( buffer.st_size));//size)); 
	else fstat->InitTag("SIZE", DLongGDL( buffer.st_size));//size));
	fstat->InitTag("OPEN", DByteGDL( 1)); 
	// fstat->InitTag("ISATTY", DByteGDL( 0)); 
	// fstat->InitTag("ISAGUI", DByteGDL( 0)); 
	// fstat->InitTag("INTERACTIVE", DByteGDL( 0)); 
	// fstat->InitTag("COMPRESS", DByteGDL( 0)); 

	fstat->InitTag("COMPRESS",DByteGDL( actUnit.Compress()));

	fstat->InitTag("READ", DByteGDL( actUnit.IsReadable()?1:0)); 
	fstat->InitTag("WRITE", DByteGDL( actUnit.IsWriteable()?1:0)); 
	fstat->InitTag("ATIME", DLong64GDL( buffer.st_atime)); 
	fstat->InitTag("CTIME", DLong64GDL( buffer.st_ctime)); 
	fstat->InitTag("MTIME", DLong64GDL( buffer.st_mtime));  
	if (big) fstat->InitTag("CUR_PTR", DLong64GDL( actUnit.Tell()));
	else fstat->InitTag("CUR_PTR", DLongGDL( actUnit.Tell()));
      }

    return fstat;
  }
  
  template<typename T>
  BaseGDL* make_array_template(EnvT* e, DLongGDL* dimKey, BaseGDL* value, DDouble off, DDouble inc)
  {
    try {
      dimension dim;

      if(!dimKey) arr(e, dim);
      else dim = dimension(&(*dimKey)[0], dimKey->N_Elements());

      if(value)
        return static_cast<T*>(value)->
          New(dim, BaseGDL::INIT)->
          Convert2(T::Traits::t);

      if(e->KeywordSet("NOZERO"))
        return new T(dim, BaseGDL::NOZERO);

      if(e->KeywordSet("INDEX"))
        return new T(dim, BaseGDL::INDGEN, off, inc);

      return new T(dim);
    } catch(GDLException& ex) {
      e->Throw(ex.getMessage());
    }

    assert(false);      
    return NULL;
  }

  BaseGDL* make_array(EnvT* e) {
    DDouble off = 0, inc = 1;
    DType type = GDL_UNDEF;

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);

    DLongGDL* dimKey = NULL;
    Guard<DLongGDL> dimKey_guard;

    static int sizeix = e->KeywordIx("SIZE");
    static int dimensionix = e->KeywordIx("DIMENSION");

    BaseGDL* size = e->GetKW(sizeix);
    BaseGDL* b_dimension = e->GetKW(dimensionix);

    if(b_dimension) {
      DLongGDL* l_dimension = e->GetKWAs<DLongGDL>(dimensionix);

      for(int i = 0; i < l_dimension->N_Elements(); ++i)
        if((*l_dimension)[i] < 1)
          e->Throw("Array dimensions must be greater than 0.");

      if(e->NParam() == 0 && size == NULL) {
        dimension dim(l_dimension->N_Elements(), 1);
        dimKey = new DLongGDL(dim, BaseGDL::NOZERO);
        dimKey_guard.Reset(dimKey);

        for(int i = 0; i < l_dimension->N_Elements(); ++i)
          (*dimKey)[i] = (*l_dimension)[i];
      }
    } else if(size) {
      DLongGDL* l_size = e->GetKWAs<DLongGDL>(sizeix);

      if(l_size->N_Elements() < 4 || l_size->N_Elements() > 11)
        e->Throw("Keyword array parameter SIZE must have from 4 to 11 elements.");

      DLong expectedDim = l_size->N_Elements()-3;
      type = static_cast<DType>((*l_size)[expectedDim + 1]);

      if(e->NParam() == 0) {
        dimension dim(expectedDim, 1);
        dimKey = new DLongGDL(dim, BaseGDL::NOZERO);
        dimKey_guard.Reset(dimKey);

        for(int i = 1; i <= expectedDim; ++i) {
          if((*l_size)[i] < 1)
            e->Throw("Array dimensions must be greater than 0.");

          (*dimKey)[i - 1] = (*l_size)[i];
        }
      }
    }

    static int indexIx = e->KeywordIx("INDEX");
    static int valueIx = e->KeywordIx("VALUE");
    BaseGDL* value = e->GetKW(valueIx);

    if(value && !value->Scalar())
      e->Throw("Expression must be a scalar in this context: " + e->GetString(valueIx));

    static int typeIx = e->KeywordIx("TYPE");
    if(e->KeywordPresent(typeIx)) {
      DLong temp;
      e->AssureLongScalarKW(typeIx, temp);
      type = static_cast<DType>(temp);
    }
    else if(e->KeywordSet("BOOLEAN")) {
      if(e->KeywordSet(indexIx))
        e->Throw("Keyword INDEX is not allowed with BOOLEAN."); 

      // TODO: Add support for BOOLEAN type introduced in IDL 8.4
      e->Throw("MAKE_ARRAY of BOOLEAN types not yet implemented.");
    }
    else if(e->KeywordSet("BYTE"))      type = GDL_BYTE;
    else if(e->KeywordSet("COMPLEX"))   type = GDL_COMPLEX;
    else if(e->KeywordSet("DCOMPLEX"))  type = GDL_COMPLEXDBL;
    else if(e->KeywordSet("DOUBLE"))    type = GDL_DOUBLE;
    else if(e->KeywordSet("FLOAT"))     type = GDL_FLOAT;
    else if(e->KeywordSet("INTEGER"))   type = GDL_INT;
    else if(e->KeywordSet("L64"))       type = GDL_LONG64;
    else if(e->KeywordSet("LONG"))      type = GDL_LONG;
    else if(e->KeywordSet("OBJ"))       type = GDL_OBJ;
    else if(e->KeywordSet("PTR"))       type = GDL_PTR;
    else if(e->KeywordSet("STRING"))    type = GDL_STRING;
    else if(e->KeywordSet("UINT"))      type = GDL_UINT;
    else if(e->KeywordSet("UL64"))      type = GDL_ULONG64;
    else if(e->KeywordSet("ULONG"))     type = GDL_ULONG;
    else if(value)                      type = value->Type();

    switch(type) {
    case GDL_BYTE:          return make_array_template<DByteGDL>(e, dimKey, value, off, inc);
    case GDL_COMPLEX:       return make_array_template<DComplexGDL>(e, dimKey, value, off, inc);
    case GDL_COMPLEXDBL:    return make_array_template<DComplexDblGDL>(e, dimKey, value, off, inc);
    case GDL_DOUBLE:        return make_array_template<DDoubleGDL>(e, dimKey, value, off, inc);
    case GDL_FLOAT:         return make_array_template<DFloatGDL>(e, dimKey, value, off, inc);
    case GDL_INT:           return make_array_template<DIntGDL>(e, dimKey, value, off, inc);
    case GDL_LONG64:        return make_array_template<DLong64GDL>(e, dimKey, value, off, inc);
    case GDL_LONG:          return make_array_template<DLongGDL>(e, dimKey, value, off, inc);
    case GDL_UINT:          return make_array_template<DUIntGDL>(e, dimKey, value, off, inc);
    case GDL_ULONG64:       return make_array_template<DULong64GDL>(e, dimKey, value, off, inc);
    case GDL_ULONG:         return make_array_template<DULongGDL>(e, dimKey, value, off, inc);
    case GDL_OBJ:
      if(e->KeywordSet(indexIx)) e->Throw("Index initialization of object reference array is invalid.");
      return make_array_template<DObjGDL>(e, dimKey, value, off, inc);
    case GDL_PTR:
      if(e->KeywordSet(indexIx)) e->Throw("Index initialization of pointer array is invalid.");
      return make_array_template<DPtrGDL>(e, dimKey, value, off, inc);
    case GDL_STRING:
      if(!e->KeywordSet(indexIx)) return make_array_template<DStringGDL>(e, dimKey, value, off, inc);
      else return make_array_template<DULongGDL>(e, dimKey, value, off, inc)->Convert2(GDL_STRING);
    case GDL_STRUCT:
      e->Throw("Invalid type specified for result.");
    }

    return make_array_template<DFloatGDL>(e, dimKey, value, off, inc);
  }
  
  BaseGDL* reform( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    BaseGDL** p0P = &e->GetParDefined( 0);
    BaseGDL* p0 = *p0P;

    SizeT nEl = p0->N_Elements();
    //     SizeT Rank = p0->Rank();
    //     if( Rank == 0)
    //       e->Throw( "Parameter must be an array in this context: " 
    // 		+ e->GetParString( 0));

    //     SizeT Type = p0->Type();

    dimension dim;

    if (nParam == 1) {
      //      SizeT j=1;
      for( SizeT i=0; i<p0->Rank(); ++i) {
	//	if (p0->Dim(i) == 0) break;
	if (p0->Dim(i) > 1)
	  {
	    dim << p0->Dim( i);  
	    //	  j *= p0->Dim(i);
	    //	  cout << j << p0->Dim(i) << endl;
	    //	  dim.Set(j,p0->Dim(i));
	    //j++;
	  }
      }
      if( dim.Rank() == 0)
	dim << 1;
      //     dim.Set(0, j);
    } 
    else 
      arr( e, dim, 1);

    if (dim.NDimElements() != nEl) 
      e->Throw( "New subscripts must not change the number of elements in " 
		+ e->GetParString( 0));

    // make a copy if p0 is not global
    //      if( !e->GlobalPar( 0))
    //	p0 = p0->Dup();
    // better: steal p0
    if( !e->GlobalPar( 0))
      {
	bool success = e->StealLocalPar( 0); //*p0P = NULL;
	//*p0P = NULL; // prevent local parameter form deletion
	assert( success);
	p0->SetDim(dim);
	return p0;
      }

    static int overwriteIx = e->KeywordIx("OVERWRITE");
    if (e->KeywordSet( overwriteIx)) 
      {
	p0->SetDim(dim);
	e->SetPtrToReturnValue( p0P);
	return p0;
      }

    // global paramter - make a copy
    BaseGDL* res = p0->Dup();
    res->SetDim(dim);
    return res;
  }

  
  // note: changes here MUST be reflected in routine_names_reference() as well
  // because DLibFun of this function is used for routine_names_reference() the keyword
  // indices must match

  BaseGDL* routine_names_value(EnvT* e) {
    SizeT nParam = e->NParam();

    EnvStackT& callStack = e->Interpreter()->CallStack();
    //     DLong curlevnum = callStack.size()-1;
    // 'e' is not on the stack
    DLong curlevnum = callStack.size();
    static int sfunctionsIx=e->KeywordIx("S_FUNCTIONS");
    static int sproceduresIx=e->KeywordIx("S_PROCEDURES");
    static int levelIx=e->KeywordIx("LEVEL");
    if (e->KeywordSet(sfunctionsIx)) {
      vector<DString> subList;

      SizeT nFun = libFunList.size();
      for (SizeT i = 0; i < nFun; ++i) {
        DString s = libFunList[ i]->ToString();
        s = s.substr(4); // Remove "res="

        size_t left_paren = s.find_first_of("(");
        subList.push_back(s.substr(0, left_paren));
      }

      sort(subList.begin(), subList.end());

      DStringGDL* res = new DStringGDL(dimension(nFun), BaseGDL::NOZERO);
      for (SizeT i = 0; i < nFun; ++i) {
        (*res)[i] = subList[ i];
      }
      return res;
    }

    if (e->KeywordSet(sproceduresIx)) {
      vector<DString> subList;

      SizeT nPro = libProList.size();
      for (SizeT i = 0; i < nPro; ++i) {
        DString s = libProList[ i]->ToString();

        size_t comma_brac = s.find_first_of(",[");
        subList.push_back(s.substr(0, comma_brac));
      }

      sort(subList.begin(), subList.end());

      DStringGDL* res = new DStringGDL(dimension(nPro), BaseGDL::NOZERO);
      for (SizeT i = 0; i < nPro; ++i) {
        (*res)[i] = subList[ i];
      }
      return res;
    }

    if (e->KeywordSet(levelIx)) {
      return new DLongGDL(curlevnum);
    }

    static int variablesIx = e->KeywordIx("VARIABLES");
    static int fetchIx = e->KeywordIx("FETCH");
    static int arg_namesIx = e->KeywordIx("ARG_NAME");
    static int storeIx = e->KeywordIx("STORE");
    bool var = false, fetch = false, arg = false, store = false;

    DLongGDL* level;
    level = e->IfDefGetKWAs<DLongGDL>(variablesIx);
    if (level != NULL) {
      var = true;
    }
    else {
      level = e->IfDefGetKWAs<DLongGDL>(fetchIx);
      if (level != NULL) {
        fetch = true;
      } else {
        level = e->IfDefGetKWAs<DLongGDL>(arg_namesIx);
        if (level != NULL) {
          arg = true;
        } else {
          level = e->IfDefGetKWAs<DLongGDL>(storeIx);
          if (level != NULL) {
            store = true;
          }
        }
      }
    }

    DString varName;

    if (level != NULL) {
      DLong desiredlevnum = (*level)[0];
      if (desiredlevnum <= 0)
        desiredlevnum += curlevnum;
      if (desiredlevnum < 1)
        return new DStringGDL("");
      if (desiredlevnum > curlevnum)
        desiredlevnum = curlevnum;

      DSubUD* pro = static_cast<DSubUD*> (callStack[desiredlevnum - 1]->GetPro());
      SizeT nVar = pro->Size(); // # var in GDL for desired level 
      SizeT nComm = pro->CommonsSize(); // # has commons?
      //cerr<<"nComm= "<<nComm<<" in " <<pro->NumberOfCommons()<<" commons"<<endl;
      SizeT nTotVar = nVar + nComm; //All the variables availables at that lev.
      int nKey = pro->NKey();
      //cout << "nKey:" << nKey << endl;
      //cout << "nVar:" << nVar << endl;
      //cout << pro->Name() << endl;

      if (var) {
        if (nTotVar == 0) return new DStringGDL("");
        DStringGDL* res = new DStringGDL(dimension(nTotVar), BaseGDL::NOZERO);
        set<string> sortedList;  // "Sorted List" 
        if (nVar > 0) {
          for (SizeT i = 0; i < nVar; ++i) {
            string vname = pro->GetVarName(i);
            sortedList.insert(vname);
          }
        }
        if (nComm > 0) {
          DStringGDL* list=static_cast<DStringGDL*>(pro->GetCommonVarNameList());
          for (SizeT i = 0; i < list->N_Elements(); ++i) {
            sortedList.insert((*list)[i]);
          }
        }
        SizeT ivar=0;
        set<string>::iterator it = sortedList.begin();
	    while (it != sortedList.end()) (*res)[ivar++] = *it++;
        return res; 
      }
      else if (fetch) { // FETCH

        e->AssureScalarPar<DStringGDL>(0, varName);
        varName = StrUpCase(varName);
        int xI = pro->Find(varName);
        //cout << xI << " " << varName << " " << pro->Size() << endl;
        if (xI != -1) {
          // 	  BaseGDL* par = ((EnvT*)(callStack[desiredlevnum-1]))->GetPar( xI-nKey);

          // Keywords are already counted (in FindVar)
          // 	  BaseGDL*& par = ((EnvT*)(callStack[desiredlevnum-1]))->GetPar( xI-nKey);
          if (((EnvT*)(callStack[desiredlevnum - 1]))->NParam() < 1) return NULL; //meaning this fetch level is not initialized. Avoids throwing an #assert in debug mode
          BaseGDL*& par = ((EnvT*) (callStack[desiredlevnum - 1]))->GetKW(xI);

// not IDL behaviour                    if (par == NULL) e->Throw("Variable is undefined: " + varName);
           if (par == NULL) return NULL;
          //	  char* addr = static_cast<char*>(par->DataAddr());

          // no retnew function BUT: ret value is not from current environment
          // which is ok with the new ref return value handling introdcuced with 0.9.4
          // note that the _reference version does not need par to be defined and is hence still necessary
          e->SetPtrToReturnValue(&par); // <-  HERE IS THE DIFFERENCE
          return par; // <-  HERE IS THE DIFFERENCE 
          // return par->Dup(); // <-  HERE IS THE DIFFERENCE // no retnew function BUT: ret value is not from current environment
        } else {
          BaseGDL** par = pro->GetCommonVarPtr(varName);
// not IDL behaviour          if (par == NULL) e->Throw("Variable is undefined: " + varName);
          if (par == NULL) return NULL;
          return *par; // <-  HERE IS THE DIFFERENCE          
        }
      if (e->Interpreter()->InterruptEnable())
          Message("Variable not found: " + varName);

        return NULL;

      } else if (arg) { // ARG_NAME

        if (nParam == 0) return new DStringGDL("");

        DStringGDL* res = new DStringGDL(dimension(nParam), BaseGDL::NOZERO);

        //	cout << "nVar:" << nVar << endl;
        EnvBaseT* desiredCallStack;
        if (desiredlevnum >= callStack.size())
          desiredCallStack = e;
        else
          desiredCallStack = callStack[ desiredlevnum];

        SizeT nCall = desiredCallStack->NParam();

        //	cout << "nCall:" << nCall << "curlevnum:" << curlevnum << endl;
        // search for all given parameters of this call
        for (SizeT i = 0; i < nParam; ++i) {

          // search all parameters of target environment
          for (SizeT j = 0; j < nCall; ++j) {

            if (e->GetParString(i) == desiredCallStack->GetParString(j)) {
              //	      cout << "Calling param: " << j+1 << endl;
              BaseGDL*& p = e->GetPar(i);
              if (p == NULL) {
                (*res)[i] = "UNDEFINED";
                // 		break;
              }
              //	      cout << "p:" << p << endl;

              SizeT xI = 0;
              for (; xI < nVar; ++xI) {
                string vname = pro->GetVarName(xI);
                BaseGDL*& par = ((EnvT*) (callStack[desiredlevnum - 1]))->GetPar(xI - nKey);
                //    cout << "xI:" << xI << " " << vname.c_str() << endl;
                //    cout << "par:" << par << endl;
                if (&par == &p) {
                  (*res)[i] = vname;
                  break;
                }
              } // xI loop
              if (xI == nVar) // not found -> search common
              {
                string vname;
                bool success = pro->GetCommonVarName(p, vname);
                if (success)
                  (*res)[i] = vname;
              }
              break;
            }
          } // j loop
        } // i loop

        return res;
      } else { // STORE

        if (nParam != 2)
          throw GDLException(e->CallingNode(),
          "ROUTINE_NAMES: Incorrect number of arguments.");

        // "res" points to variables to be restored
        BaseGDL* res = e->GetParDefined(1);

        SizeT s;
        e->AssureScalarPar<DStringGDL>(0, varName);
        varName=StrUpCase(varName);
        int xI = pro->FindVar(varName);
        // cout << "varName: " << varName << " xI: " << xI << endl;
        if (xI != -1) {
          s = xI;
          // cout << "FindVar s: " << s << endl;
        } else {
          BaseGDL** varPtr = pro->GetCommonVarPtr(varName);
          // cout << "FindCommonVar s: " << varPtr << endl;
          if (varPtr) {
            if (pro->ReplaceExistingCommonVar(varName, res->Dup())) return new DIntGDL(1);
            else return new DIntGDL(0);
          } else {
            SizeT u = pro->AddVar(varName);
            s = callStack[desiredlevnum - 1]->AddEnv();
          //  cout << "AddVar u: " << u << endl;
          //  cout << "AddEnv s: " << s << endl;
          }
        }
        // 	BaseGDL*& par = ((EnvT*)(callStack[desiredlevnum-1]))->GetPar( s-nKey);

        // 	((EnvT*)(callStack[desiredlevnum-1]))->GetPar( s-nKey) = res->Dup();
        ((EnvT*) (callStack[desiredlevnum - 1]))->GetKW(s) = res->Dup();

        //	cout << "par: " << &par << endl << endl;
        // 	memcpy(&par, &res, sizeof(par)); 

        return new DIntGDL(1);
      }
    } else {
      // Get Compiled Procedures & Functions 
      DLong n = proList.size() + funList.size() + 1;

      // Add $MAIN$ to list
      vector<DString> pfList;
      pfList.push_back("$MAIN$");

      // Procedures
      for (ProListT::iterator i = proList.begin(); i != proList.end(); ++i) {
        pfList.push_back((*i)->ObjectName());
      }

      // Functions
      for (FunListT::iterator i = funList.begin(); i != funList.end(); ++i) {
        pfList.push_back((*i)->ObjectName());
      }

      // Sort
      sort(pfList.begin(), pfList.end());

      // Fill return variable
      dimension dim(&n, (size_t) 1);
      DStringGDL* res = new DStringGDL(dim, BaseGDL::NOZERO);
      for (SizeT i = 0; i < n; ++i) {
        (*res)[i] = pfList[ i];
      }
      return res;
    }
  }

  // this version does not need the return value pointing to a defined value and is hence necessary  

  BaseGDL** routine_names_reference(EnvT* e) {
    SizeT nParam = e->NParam();

    EnvStackT& callStack = e->Interpreter()->CallStack();
    DLong curlevnum = callStack.size();
    static int sfunctionsIx=e->KeywordIx("S_FUNCTIONS");
    static int sproceduresIx=e->KeywordIx("S_PROCEDURES");
    static int levelIx=e->KeywordIx("LEVEL");
    if (e->KeywordSet(sfunctionsIx)) {
      return NULL;
    }

    if (e->KeywordSet(sproceduresIx)) {
      return NULL;
    }

    if (e->KeywordSet(levelIx)) {
      return NULL;
    }

    static int variablesIx = e->KeywordIx("VARIABLES");
    static int fetchIx = e->KeywordIx("FETCH");
    static int arg_namesIx = e->KeywordIx("ARG_NAME");
    static int storeIx = e->KeywordIx("STORE");
    bool var = false, fetch = false, arg = false, store = false;
    var=e->KeywordPresent(variablesIx);
    arg=e->KeywordPresent(arg_namesIx);
    store=e->KeywordPresent(storeIx);
    fetch=e->KeywordPresent(fetchIx);
    if (fetch && nParam < 1) e->Throw("Incorrect number of arguments.");
    if ( var + fetch + store + arg > 1 ) e->Throw("Incorrect number of arguments.");
    DLongGDL* level;
    level = e->IfDefGetKWAs<DLongGDL>(variablesIx);
    if (level != NULL) {
      var = true;
    } else {
      level = e->IfDefGetKWAs<DLongGDL>(fetchIx);
      if (level != NULL) {
        fetch = true;
      } else {
        level = e->IfDefGetKWAs<DLongGDL>(arg_namesIx);
        if (level != NULL) {
          arg = true;
        } else {
          level = e->IfDefGetKWAs<DLongGDL>(storeIx);
          if (level != NULL) {
            store = true;
          }
        }
      }
    }

    DString varName;

    if (level != NULL) {
      DLong desiredlevnum = (*level)[0];
      if (desiredlevnum <= 0) desiredlevnum += curlevnum;
      if (desiredlevnum < 1) return NULL;
      if (desiredlevnum > curlevnum) desiredlevnum = curlevnum;

      DSubUD* pro = static_cast<DSubUD*> (callStack[desiredlevnum - 1]->GetPro());

      SizeT nVar = pro->Size(); // # var in GDL for desired level 
      SizeT nComm = pro->CommonsSize(); // # has commons?
      SizeT nTotVar = nVar + nComm; //All the variables availables at that lev.

      int nKey = pro->NKey();
      //cout << "nKey:" << nKey << endl;
      //cout << "nVar:" << nVar << endl;
      //cout << pro->Name() << endl;

      if (fetch) { // FETCH

        e->AssureScalarPar<DStringGDL>(0, varName);
        varName = StrUpCase(varName);
        int xI = pro->FindVar(varName);
        //	cout << xI << endl;
        if (xI != -1) {
          // 	  BaseGDL*& par = ((EnvT*)(callStack[desiredlevnum-1]))->GetPar( xI-nKey);
          BaseGDL*& par = ((EnvT*) (callStack[desiredlevnum - 1]))->GetKW(xI);
          if (par == NULL) return NULL;
          return &par; // <-  HERE IS THE DIFFERENCE
        } else {
          BaseGDL** par = pro->GetCommonVarPtr(varName);
// not IDL behaviour          if (par == NULL) e->Throw("Variable is undefined: " + varName);
          if (par == NULL)  return NULL;
          return par; // <-  HERE IS THE DIFFERENCE           
        }

        e->Throw("Variable not found: " + varName);
        return NULL;

      } else if (var) { // ARG_NAME

        return NULL;

      } else if (arg) { // ARG_NAME

        return NULL;

      }
      else { // STORE

        return NULL;
      }
    }
    else {
      // Get Compiled Procedures & Functions 
      return NULL;
    }
  }

  
  
} // namespace

