/***************************************************************************
                          basic_fun_jmg.cpp  -  basic GDL library function
                             -------------------
    begin                : 2004
    copyright            : (C) 2004 by Joel Gales
                         : (C) 2010 by Christoph Fuchs (CALL_EXTERNAL)
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

#include "envt.hpp"
#include "dinterpreter.hpp"
#include "basic_pro_jmg.hpp"

#define MAXNDLL 40

//#define GDL_DEBUG
#undef GDL_DEBUG

#ifdef _MSC_VER
#include "gtdhelper.hpp"
#endif

namespace lib {

  using namespace std;

  void (*dynPro[MAXNDLL/2])( EnvT* e);
  BaseGDL*(*dynFun[MAXNDLL/2])( EnvT* e);


  void point_lun( EnvT* e) 
  { 
    e->NParam( 1);//, "POINT_LUN");

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    if( lun == 0 || abs(lun) > maxLun)
      throw GDLException( e->CallingNode(), 
  			  "POINT_LUN:  File unit is not within allowed range.");

    GDLStream& actUnit = fileUnits[ abs(lun)-1];

    if( !actUnit.IsOpen()) 
      throw GDLException( e->CallingNode(), 
  			  "POINT_LUN:  File unit is not open: " +i2s(abs(lun)));

    if (lun < 0) {
      BaseGDL** retPos = &e->GetPar( 1);

      GDLDelete(*retPos);
      *retPos = new DLongGDL( actUnit.Tell());
      return;

    } else {
      DLong64 pos;
      e->AssureLongScalarPar( 1, pos);
      actUnit.Seek( pos);
    }
  }

   
  void linkimage( EnvT* e) 
  {

#ifdef _MSC_VER
    HMODULE module[MAXNDLL];
#else
    const char *error;
    void *module[MAXNDLL];
#endif
    static int count_pro=0;
    static int count_fun=0;
    int count;

    if (count_pro == MAXNDLL/2) {
      printf("Maximum number of dynamic procedures reached: %d\n", 
	     MAXNDLL/2);
      return;
    }

    if (count_fun == MAXNDLL/2) {
      printf("Maximum number of dynamic functions reached: %d\n", 
	     MAXNDLL/2);
      return;
    }

    count = count_pro + count_fun;

    SizeT nParam=e->NParam();

    DString funcName;
    e->AssureScalarPar<DStringGDL>( 0, funcName);
    DString upCasefuncName = StrUpCase( funcName);

    DString shrdimgName;
    e->AssureScalarPar<DStringGDL>( 1, shrdimgName);

    DLong funcType;
    e->AssureLongScalarPar( 2, funcType);

    DString entryName;
    e->AssureScalarPar<DStringGDL>( 3, entryName);

    /* Load dynamically loaded library */
#ifdef _MSC_VER
    #ifdef _UNICODE
    TCHAR u_shrdimgName[255];
    MultiByteToWideChar(CP_ACP, 0, shrdimgName.c_str(), shrdimgName.length(), u_shrdimgName, 255);
    module[count] = LoadLibrary(u_shrdimgName);
    #else
    module[count] = LoadLibrary(shrdimgName.c_str());
    #endif

    if (!module[count]) {
      fprintf(stderr, "Couldn't open %s\n", 
	      shrdimgName.c_str());
      return;
    }
#else
    module[count] = dlopen(shrdimgName.c_str(), RTLD_LAZY);

    if (!module[count]) {
      fprintf(stderr, "Couldn't open %s: %s\n", 
	      shrdimgName.c_str(), dlerror());
      return;
    }

    /* Get symbol */
    dlerror();
#endif

#ifdef _MSC_VER
    if (funcType == 0) {
      (void* &) dynPro[count_pro] = 
	(void *) GetProcAddress(module[count], entryName.c_str());
    } else if (funcType == 1) {
      (BaseGDL* &) dynFun[count_fun] = 
	(BaseGDL*) GetProcAddress(module[count], entryName.c_str());
    } else {
      printf("Improper function type: %d\n", funcType);
      FreeLibrary(module[count]);
      return;
    }
#else
    if (funcType == 0) {
      (void* &) dynPro[count_pro] = 
	(void *) dlsym(module[count], entryName.c_str());
    } else if (funcType == 1) {
      (BaseGDL* &) dynFun[count_fun] = 
	(BaseGDL*) dlsym(module[count], entryName.c_str());
    } else {
      printf("Improper function type: %d\n", funcType);
      dlclose(module[count]);
      return;
    }

    if ((error = dlerror())) {
      fprintf(stderr, "Couldn't find %s: %s\n", entryName.c_str(), error);
      return;
    }
#endif
    if (funcType == 0){
      new DLibPro(lib::dynPro[count_pro], upCasefuncName.c_str(), 16);
      count_pro++;
    } else if (funcType == 1) {
      new DLibFun(lib::dynFun[count_fun], upCasefuncName.c_str(), 16);
      count_fun++;
    }
  }

  void wait( EnvT* e) 
  { 
    e->NParam( 1);//, "WAIT");

    DDouble waittime;
    e->AssureDoubleScalarPar( 0, waittime);

    if( waittime < 0)
      throw GDLException( e->CallingNode(), 
  			  "WAIT:  Argument must be non-negative"
			  +e->GetParString( 0));
    int old_version=0;

    if (waittime <= 0.005) old_version=1;

    // AC 2010-09-16
    // this version is OK and very accurate for small durations
    // but used 100% of one CPU :((
    if (old_version == 1) {

      struct timeval tval;
      struct timezone tzone;
      
      // derivated from the current version of SYSTIME()
      
      gettimeofday(&tval,&tzone);
      double t_start = tval.tv_sec+tval.tv_usec/1e+6; // time in UTC seconds
      double t_current=0.0;
      
      double diff=0.0;
      while (diff < waittime ) {      
	gettimeofday(&tval,&tzone);
	t_current= tval.tv_sec+tval.tv_usec/1e+6;
	diff=t_current - t_start;
      }
    }

    // AC 2010-09-16 this version should used much less CPU !
    if (old_version == 0) {
      //cout << floor(waittime) << " " <<  waittime-floor(waittime) << endl;
#ifdef _MSC_VER
      Sleep(floor(waittime*1e3));
#else
      struct timespec tv;
      tv.tv_sec = floor(waittime);
      tv.tv_nsec = (waittime-floor(waittime))*1e9;
      int retval;
      retval=nanosleep(&tv,NULL);
#endif
    }
  }

//   void kwtest( EnvT* e)
//   {
//     StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());
// 
//     // here first parameter is the function name
//     // set callF to the function you want to call
//     int nParam=e->NParam();
//     if( nParam == 0)
//      e->Throw( "No function specified.");
//     DString callF;
//     e->AssureScalarPar<DStringGDL>( 0, callF);
//  
//     // this is a function name -> convert to UPPERCASE
//     callF = StrUpCase( callF);
// 
// 	SizeT funIx = GDLInterpreter::GetFunIx( callF);
// 	
// 	EnvUDT* newEnv= new EnvUDT( e->CallingNode(), funList[ funIx], (BaseGDL**)NULL);
// 
// 	// add parameter
// 	SizeT widgetID = 999;
//       
//     newEnv->SetNextPar( new DLongGDL(widgetID)); // pass as local
// 
// 	e->Interpreter()->CallStack().push_back( newEnv);
// 	
// 	// make the call
// 	BaseGDL* res = e->Interpreter()->
// 	  call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
// 
// 	// set the keyword to the function's return value
//     static int testIx = e->KeywordIx( "TEST");
//     e->SetKW( testIx, res);
//   }

  // CALL_EXTERNAL by Christoph Fuchs
  //AC #ifdef USE_EIGEN
  //AC SizeT defaultAlign = 16;
  //AC #else  
  typedef struct {
    char      c;
    long long l;
  } testAlign;
  SizeT defaultAlign = (SizeT)( sizeof(testAlign)-sizeof(long long) );
  //AC #endif
  
  BaseGDL* call_external( EnvT* e)
  {
    DString image, entry;
    static std::string s;
    SizeT myAlign      = defaultAlign;
    DType myReturnType = GDL_UNDEF;

    SizeT nParam=e->NParam(2);

    // Interpret keywords

    // Return-Type

    static int kwIxReturnType = e->KeywordIx("RETURN_TYPE");
    DIntGDL* KWReturnType     = e->IfDefGetKWAs<DIntGDL>(kwIxReturnType);
    if (KWReturnType != NULL && (*KWReturnType)[0] != 0) {
	myReturnType = (DType) (*KWReturnType)[0];
    }

    string TypeKW[16] = {
	"", "B_VALUE", "I_VALUE", "L_VALUE", "F_VALUE", "D_VALUE",
	"", "S_VALUE", "", "", "", "",
	"UI_VALUE", "UL_VALUE", "L64_VALUE", "UL64_VALUE"
    };

    for (int i=0; i<16; i++) {
	if (strcmp(TypeKW[i].c_str(), "") == 0) {continue;}
	int kwIxType = e->KeywordIx(TypeKW[i]);
	if (e->KeywordSet(kwIxType)) {
	    if (myReturnType != GDL_UNDEF) {
		e->Throw("Multiple requests of return type not allowed: "
			 + TypeKW[i] );
	    }
	    myReturnType = (DType)i;
	}
    }

    if (myReturnType == GDL_UNDEF) {
	myReturnType = GDL_LONG;
    }

    // STRUCT_ALIGN_BYTES

    static int kwIxStructAlign = e->KeywordIx("STRUCT_ALIGN_BYTES");
    DIntGDL* KWStructAlign     = e->IfDefGetKWAs<DIntGDL>(kwIxStructAlign);
    if (KWStructAlign != NULL && (*KWStructAlign)[0] != 0) {
	myAlign = (SizeT) (*KWStructAlign)[0];
    }

    // UNLOAD

    static int kwIxUnload  = e->KeywordIx("UNLOAD");
    bool flagUnload = e->KeywordSet(kwIxUnload);

    // parameters by value/reference/GDL reference

    static int kwIxAllValue  = e->KeywordIx("ALL_VALUE");
    bool flagAllValue = e->KeywordSet(kwIxAllValue);

    static int kwIxAllGdl  = e->KeywordIx("ALL_GDL");
    bool flagAllGdl = e->KeywordSet(kwIxAllGdl);
    if (flagAllValue && flagAllGdl) {
	e->Throw("Conflicting keywords ALL_VALUE and ALL_GDL");
    }

//     short* byValue = (short*) malloc( (nParam-2) * sizeof(short) );
//     if (byValue == NULL) {
// 	e->Throw("Internal error allocating memory for byValue");
//     }
    vector<short> byValue(nParam-2,0);

    for (SizeT i=0; i<nParam-2;i++) {
	byValue[i] = flagAllValue ? 1 : flagAllGdl ? -1 : 0;
    }

    static int kwIxValue = e->KeywordIx("VALUE");
    DIntGDL* KWValue     = e->IfDefGetKWAs<DIntGDL>(kwIxValue);
    if (KWValue != NULL) {
	if (flagAllValue || flagAllGdl) {
	    e->Throw("Only one of VALUE,  ALL_VALUE, or ALL_GDL allowed");
	}
	if (KWValue->N_Elements() < nParam - 2) {
	    e->Throw("VALUE must have an entry for every parameter");
	}
	for (SizeT i=0; i<nParam-2;i++) {
	    byValue[i] = (short)(*KWValue)[i];
	}
    }

    e->AssureStringScalarPar( (SizeT)0, image);
    e->AssureStringScalarPar( (SizeT)1, entry);

    int argc      = nParam-2;
    // must be void** for dl... stuff
    void **argv   = (void**)malloc((nParam-2) * sizeof(void*) );
    if (argv == NULL) {
	e->Throw("Internal error allocating memory for argv");
    }
    GDLGuard<void*,void,void> argvGuard(argv, free);
    
    // Fill argv with the parameters

    for(SizeT i =2; i < nParam; i++){
	BaseGDL* par = e->GetParDefined(i);
	DType    pType  = par->Type();

	if (byValue[i-2] > 0 ) {	// By Value
	    if (! par->StrictScalar() ) {
		e->Throw("Must be a scalar here: "
			 + e->GetParString(i)
		);
	    }

	    if (NumericType(pType)) {
		if (par->Sizeof() > sizeof(void*)) {
		    e->Throw("Parameter is larger than pointer: "
			     + e->GetParString(i)
		    );
		}
		memcpy(&argv[i-2], (void*) par->DataAddr(), par->Sizeof());
	    }
	    else if (pType == GDL_STRING) {
		argv[i-2] = (void*) (*(DStringGDL*)(par))[0].c_str();
	    }
	    else {
		e->Throw("Unsupported type for call by value: "
			 + e->GetParString(i)
		);
	    }
	}
	else if (byValue[i-2] < 0) {		// by GDL pointer
	    argv[i-2] = (void*) par;
	}
	else {					// By reference (default)
	    if (NumericType(pType) || pType == GDL_PTR || pType == GDL_OBJ ) {
		argv[i-2] = (void*) par->DataAddr();
	    }
	    else if (pType == GDL_STRING) {
		argv[i-2] = (void*) ce_StringGDLtoIDL(e, par);
	    }
	    else if (pType == GDL_STRUCT) {
		argv[i-2] = ce_StructGDLtoIDL(e, par, NULL, myAlign);
	    }
	    else {
		e->Throw("Objects and Pointers not yet supported");
	    }
	}
    }

    // Load shared object, call function

#ifdef _MSC_VER
    HMODULE handle =  LoadLibrary((LPCSTR)image.c_str());
#else
    // you MUST keep the double "||" to have CALL_EXTERNAL working
    void* handle =  dlopen(image.c_str(),  RTLD_NOW || RTLD_GLOBAL);
#endif
    if (handle == NULL) {
#ifndef _MSC_VER
	cout << dlerror() << endl;
#endif
	e->Throw("Error opening shared object: " + image);
    }

#ifdef _MSC_VER
    void* func = GetProcAddress(handle,entry.c_str());
#else
    void* func = dlsym(handle,entry.c_str());
#endif
    if (func == NULL) {
	e->Throw("Entry not found: " + entry);
    }

    union {
	DByte		d_byte;
	DInt		d_int;
	DLong		d_long;
	DFloat		d_float;
	DDouble		d_double;
	DUInt		d_uint;
	DULong		d_ulong;
	DLong64		d_long64;
	DULong64	d_ulong64;
	char*		d_string;
    } ret;

    switch (myReturnType) {
	case GDL_BYTE:    ret.d_byte    = ((DByte(*)   (int, void**))func)(argc, argv);
		      break;
	case GDL_INT:     ret.d_int     = ((DInt(*)    (int, void**))func)(argc, argv);
		      break;
	case GDL_LONG:    ret.d_long    = ((DLong(*)   (int, void**))func)(argc, argv);
		      break;
	case GDL_FLOAT:   ret.d_float   = ((DFloat(*)  (int, void**))func)(argc, argv);
		      break;
	case GDL_DOUBLE:  ret.d_double  = ((DDouble(*) (int, void**))func)(argc, argv);
		      break;
	case GDL_UINT:    ret.d_uint    = ((DUInt(*)   (int, void**))func)(argc, argv);
		      break;
	case GDL_ULONG:   ret.d_ulong   = ((DULong(*)  (int, void**))func)(argc, argv);
		      break;
	case GDL_LONG64:  ret.d_long64  = ((DLong64(*) (int, void**))func)(argc, argv);
		      break;
	case GDL_ULONG64: ret.d_ulong64 = ((DULong64(*)(int, void**))func)(argc, argv);
		      break;
	case GDL_STRING:  ret.d_string  = ((char*(*)   (int, void**))func)(argc, argv); 
		      break;
	default:      e->Throw("Return type not supported: " + i2s(myReturnType) );
		      break;
    }

    if (flagUnload) {
#ifdef _MSC_VER
	while (! FreeLibrary(handle) ) {}
#else
	while (! dlclose(handle) ) {}
#endif
    }
// necessary since struct is freed below, i do not see how??? (FIXME)
    if (myReturnType == GDL_STRING) {s=ret.d_string;}
    // Copy strings and structures back to GDL, free memory

    for (SizeT i = nParam - 1; i >= 2; i--) {
	if (byValue[i-2] != 0) {continue;}
      BaseGDL* par = e->GetParDefined(i);
      SizeT pType = par->Type();
      if (pType == GDL_STRING) {
        ce_StringIDLtoGDL((EXTERN_STRING*) argv[i - 2], par, 1);
      }
      else if (pType == GDL_STRUCT) {
        ce_StructIDLtoGDL(e, argv[i - 2], par, 1, myAlign);
      }
    }

    // now guarded. s. a.
    //free(argv);

    // Return the return value

    switch (myReturnType) {
        case GDL_BYTE:      return new DByteGDL(ret.d_byte);
			break;
        case GDL_INT:       return new DIntGDL(ret.d_int);
			break;
        case GDL_LONG:      return new DLongGDL(ret.d_long);
			break;
        case GDL_FLOAT:     return new DFloatGDL(ret.d_float);
			break;
        case GDL_DOUBLE:    return new DDoubleGDL(ret.d_double);
			break;
        case GDL_UINT:      return new DUIntGDL(ret.d_uint);
			break;
        case GDL_ULONG:     return new DULongGDL(ret.d_ulong);
			break;
        case GDL_LONG64:    return new DLong64GDL(ret.d_long64);
			break;
        case GDL_ULONG64:   return new DULong64GDL(ret.d_ulong64);
			break;
        case GDL_STRING:    return new DStringGDL(s);
			break;
    }
    assert( false);
  }


    // ce_StringGDLtoIDL: Convert a GDL string (array) to the IDL string structure
    //                   used for call_external
    //                   Allocates memory, which should be freed later with
    //                   ce_StringIDLtoGDL

    EXTERN_STRING* ce_StringGDLtoIDL(EnvT* e, const BaseGDL* par) 
    {
	SizeT NEl = par->N_Elements();
	EXTERN_STRING* extstring = (EXTERN_STRING*) malloc(NEl * sizeof(EXTERN_STRING));
	if (extstring == NULL) {
	    e->Throw("Internal error allocating memory for extstring");
	}
	for (SizeT iEl=0; iEl < NEl; iEl++) {
	    // DString     parstring = (*static_cast<DStringGDL*>(par))[iEl];
	    DString   parstring = (*(DStringGDL*)(par))[iEl];
	    extstring[iEl].slen = parstring.length();
	    extstring[iEl].s    = (char*) malloc( (extstring[iEl].slen+1) * sizeof(char) );
	    if (extstring[iEl].s == NULL) {
		e->Throw("Internal error allocating memory for extstring[iEl].s");
	    }
	    strcpy(extstring[iEl].s, parstring.c_str());
	}
	return extstring;
    }

    // ce_StringIDLtoGDL: copy any changes made in the IDL string stucture
    //                   back to an GDL string 
    //                   possibly free any memory allocated by ce_StringGDLtoIDL

    void ce_StringIDLtoGDL( EXTERN_STRING* extstring, BaseGDL* par, int freeMemory)
    {
	SizeT NEl = par->N_Elements();
	for (SizeT iEl=0; iEl < NEl; iEl++) {
	    DString parstring = (*static_cast<DStringGDL*>(par))[iEl];
	    if (strcmp( extstring[iEl].s, parstring.c_str()) ) {
		((*static_cast<DStringGDL*>(par))[iEl]).assign(extstring[iEl].s);
	    }
	    if (freeMemory) {free(extstring[iEl].s); }
	}
	if (freeMemory) {free(extstring); }
    }
    

    // ce_StructGDLtoIDL: Convert a GDL structure (array) to the IDL (C) structure
    //                   used for call_external
    //                   Allocates memory, which should be freed later with
    //                   ce_StructIDLtoGDL

    void* ce_StructGDLtoIDL( EnvT* e, const BaseGDL* par, SizeT* length, SizeT myAlign) 
    {
	SizeT totalSize = ce_LengthOfIDLStruct( e, par, myAlign);
	if (length != NULL) {*length = totalSize;}

	SizeT nEl       = par->N_Elements();
	void* IDLStruct = malloc(nEl * totalSize);
	if (IDLStruct == NULL) {
	    e->Throw("Internal error allocating memory for IDLStruct");
	}

	DStructGDL* s = (DStructGDL*) par;
	SizeT nTags   = s->Desc()->NTags();

	for (SizeT iEl=0; iEl < nEl; iEl++) {
	    char* p = (char*) IDLStruct + iEl*totalSize;
	    for (SizeT iTag=0; iTag < nTags; iTag++) {
		BaseGDL* member = s->GetTag(iTag, iEl);
		DType    pType  = member->Type();
		SizeT length;
		SizeT sizeOf;
		void* source;
		int   doFree = 0;
		if (NumericType(pType) || pType == GDL_PTR || pType == GDL_OBJ) {
		    source = (void*) member->DataAddr();
		    length = member->NBytes();
		    sizeOf = member->Sizeof();
		}
		else if (pType == GDL_STRING) {
		    source = (void*) ce_StringGDLtoIDL(e, member);
		    length = member->N_Elements() * sizeof(EXTERN_STRING);
		    sizeOf = 8;
		    doFree = 1;
		}
		else if (pType == GDL_STRUCT) {
		    source = ce_StructGDLtoIDL( e, member, &length, myAlign );
		    length *= member->N_Elements();
		    sizeOf = 8;
		}
		else {
		    e->Throw("Unsupported type in structure: " + i2s(pType) );
		}
		SizeT align = sizeOf < myAlign ? sizeOf : myAlign;
		if ((SizeT)p % align) {
		    SizeT space = align - (SizeT)p % align;
		    p += space;
		}
		memcpy(p, source, length);
		p+=length;
		if (doFree) {free(source);}
		    
	    }
	}

	return IDLStruct;
    }

    // ce_StructIDLtoGDL: copy any changes made in the IDL stucture
    //                   back to an GDL structure
    //                   possibly free any memory allocated bi ce_StructGDLtoIDL

    void ce_StructIDLtoGDL( EnvT* e,  void* IDLStruct, BaseGDL* par, int freeMemory, SizeT myAlign)
    {
	SizeT nEl     = par->N_Elements();
	DStructGDL* s = (DStructGDL*) par;
	SizeT nTags   = s->Desc()->NTags();

	char* p = (char*) IDLStruct;
	for (SizeT iEl=0; iEl < nEl; iEl++) {
	    for (SizeT iTag=0; iTag < nTags; iTag++) {
		BaseGDL* member = s->GetTag(iTag, iEl);
		DType    pType  = member->Type();
		SizeT length;
		SizeT sizeOf;
		void* dest;
		if (NumericType(pType)) {
		    sizeOf = member->Sizeof();
		}
		else {
		    sizeOf = 8;
		}
		SizeT align = sizeOf < myAlign ? sizeOf : myAlign;
		if ((SizeT)p % align) {
		    SizeT space = align - (SizeT)p % align;
		    p += space;
		}

		if (NumericType(pType) || pType == GDL_PTR || pType == GDL_OBJ) {
		    length = member->NBytes();
		    dest   = (void*) member->DataAddr();
		    memcpy(dest, p, length);
		}
		else if (pType == GDL_STRING) {
		    ce_StringIDLtoGDL((EXTERN_STRING*) p, member, 0);
		    length = member->N_Elements() * sizeof(EXTERN_STRING);
		}
		else if (pType == GDL_STRUCT) {
		    ce_StructIDLtoGDL( e, (void*)p, member, 0, myAlign );
		    length = member->N_Elements() * ce_LengthOfIDLStruct(e, member, myAlign);
		}
		else {
		    e->Throw("Unsupported type");
		}

		p += length;
	    }
	    if ((SizeT)p%myAlign) {
		p+= (myAlign - (SizeT)p%myAlign);
	    }
	}

	if (freeMemory) {
	    free(IDLStruct);
	}
    }

    SizeT ce_LengthOfIDLStruct( EnvT* e, const BaseGDL* par, SizeT myAlign) 
    {
	DStructGDL* s = (DStructGDL*) par;
	SizeT nTags   = s->Desc()->NTags();

	SizeT totalSize = 0;
	SizeT sizeOf;
	for (SizeT iTag=0; iTag < nTags; iTag++) {
	    BaseGDL* member = s->GetTag(iTag);
	    DType    pType  = member->Type();
	    // there is probably no point transfering PTR and OBJ
	    // but if it fits why restrict it? They should never be used though
	    if (NumericType(pType) || pType == GDL_PTR || pType == GDL_OBJ) {
		totalSize += member->NBytes();
		sizeOf    =  member->Sizeof();
	    }
	    else if (pType == GDL_STRING) {
		totalSize += member->N_Elements() * sizeof(EXTERN_STRING);
		sizeOf    =  8;
	    }
	    else if (pType == GDL_STRUCT) {
		totalSize += member->N_Elements() * ce_LengthOfIDLStruct( e, member, myAlign);
		sizeOf    =  8;
	    }
	    else {
		e->Throw("Unsupported type");
	    }

	    SizeT align = sizeOf < myAlign ? sizeOf : myAlign;
	    if (totalSize%align) {
		totalSize+= (align - totalSize%align);
	    }
	}
	if (totalSize%myAlign) {
	    totalSize+= (myAlign - totalSize%myAlign);
	}
	return totalSize;
    }

} // namespace
