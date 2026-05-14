#ifndef export_is_defined
#define export_is_defined

#if defined(TRACE_ROUTINE)
#undef TRACE_ROUTINE
#endif

//#define GDL_DEBUG
//#undef GDL_DEBUG

//#define TRACE_OPCALLS
//#define TRACE_OPCALLS_LITE
//#define BACKTRACE_OPCALLS

#if defined(TRACE_OPCALLS)
#define TRACE_ROUTINE(func,file,line) std::cout << func << "\t" << file << "\t" << line << std::endl;
#elif defined(TRACE_OPCALLS_LITE)
#define TRACE_ROUTINE(func,file,line) std::cout << func << ",";
#elif defined(BACKTRACE_OPCALLS)
#if !defined(_WIN32) || defined(__CYGWIN__)
#define TRACE_ROUTINE(func,file,line) { std::cout << func << "\t" << file << "\t" << line << std::endl;\
	int nptrs = backtrace(bt_buffer, BT_BUF_SIZE);\
	fprintf(stderr, "\tbacktrace:\n");\
	backtrace_symbols_fd(bt_buffer, nptrs, STDERR_FILENO);\
	}
#endif
#else
#define TRACE_ROUTINE(func,file,line)
#endif

#include "gdl_export.h"
#include "datatypes.hpp"
#include "GDLInterpreter.hpp"
#include "terminfo.hpp"
#include <setjmp.h>
#if !defined(_WIN32) || defined(__CYGWIN__)
#include <execinfo.h>
#endif
#define BT_BUF_SIZE 100
static void* bt_buffer[BT_BUF_SIZE];

#define GDL_TYP_COMPLEXDBL GDL_TYP_DCOMPLEX
#define JUMP_THROW 1
#define JUMP_RETURN 2

static std::map<const char*,void*> SysFunDefinitions; 
static std::map<const char*,void*> SysProDefinitions;
typedef struct {
  EXPORT_VPTR v;
  BaseGDL** par;
  bool global;
  SizeT scope_level;
  bool create;
} varInfo;
typedef struct {
	void* symbol;
	std::string name;
	bool haskeywords;
} callableInfo;
static std::vector<varInfo> PassedVariables;
static std::map<EXPORT_HVID, EXPORT_HEAP_VPTR> MimickedHeap;
static std::map<void*, std::string> PassedVariablesNames; 
static std::vector<callableInfo> CollectedDLMSymbols; //vector of possible function addresses and dDLM names 
static std::vector<EXPORT_VPTR> GlobalVPTRList;
static std::vector<EXPORT_VPTR> GlobalVPTRKwList;

// list of memory (strings...) to be released when GDL_FreeResources() is called.
// If each call is ended by freeing the resources, this list does not need to be private to each CallDllFunc/CallDllpro I guess.
static std::vector<void*> FreeAtEnd;
static jmp_buf callerEnv;

#define ISSCALAR(me) ((me->flags & GDL_V_ARR)==0)
#define ISARRAY(me) (me->flags & GDL_V_ARR)
#define ISTEMP(me) (me->flags & (GDL_V_TEMP|GDL_V_CONST))
inline void* MyMallocDestroyedOnExit(size_t size) {
	//TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	void* ret =  gdlAlignedMalloc(size);
	memset(ret, 0, size);
	FreeAtEnd.push_back(ret);
	return ret;
}

inline void* MyCallocDestroyedOnExit(size_t nmemb, size_t size) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
			void* ret = gdlAlignedMalloc(nmemb*size);
	memset(ret,0,nmemb*size);
	FreeAtEnd.push_back(ret);
	return ret;
}
inline void MyFree(void* p) {//TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	gdlAlignedFree(p);
}
void FreeIntermediateMemory(){	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
for (auto it = FreeAtEnd.begin(); it != FreeAtEnd.end(); ++it) {
#ifdef GDL_DEBUG
	std::cerr<<std::hex<<"freeing #"<<(*it)<<std::endl;
#endif  
	MyFree(*it);
}
	FreeAtEnd.clear();
}

typedef struct {
  const char* name;
  BaseGDL* gdlVarPtr; // pointer to some externally produced var if out=true
  EXPORT_VPTR convertedVPTR;
  bool out;
  UCHAR type;
  bool global; // associated variable
  std::string varname; 
} GDL_KEYWORDS_LIST;

typedef struct {
  int npassed;
  GDL_KEYWORDS_LIST* passed;
} GDL_PASS_KEYWORDS_LIST;

extern "C" {  
  DLL_PUBLIC char * GDL_CDECL IDL_VarName(EXPORT_VPTR v);

	//--------------temporary utilities------------------
void GdlExportPrintVar(EXPORT_VPTR v);
	void GdlExportPrintArrayDescr(EXPORT_ARRAY *arr) {
		printf("Array[");
		int i;
		for (i = 0; i < arr->n_dim - 1; ++i) {
			printf("%ld,", arr->dim[i]);
		}
		printf("%ld ]", arr->dim[i]);
	}

void GdlExportPrintHash(EXPORT_IDENT *h){ while (h) {
		printf ("-> {hash: 0x%x , name=%s len:%d} ", h->hash, h->name, h->len); h=h->hash;
	}
}
void GdlExportPrintStruct(EXPORT_SREF s){
  GdlExportPrintArrayDescr(s.arr);
  if (s.sdef->id != NULL) printf("ident: %s ,", s.sdef->id->name);
  printf("ntags %d\n", s.sdef->ntags);
  for (int i=0; i< s.sdef->ntags; ++i) { 
	  printf ("TAG [%d] name: %s at offset: %d, len:%d, start hash chain = 0x%x ",i,s.sdef->tags[i].id->name,s.sdef->tags[i].offset,s.sdef->tags[i].id->len,s.sdef->tags[i].id->hash); GdlExportPrintHash(s.sdef->tags[i].id->hash);
	  printf("\n var: \n");
	  GdlExportPrintVar( &(s.sdef->tags[i].var) ); printf("\n");}
}
	// print a full var

	void GdlExportPrintVar(EXPORT_VPTR v) {
		if (v->flags & GDL_V_STRUCT) {
			printf("STRUCTURE:\n");
			GdlExportPrintStruct(v->value.s);
		} else if (ISARRAY(v)) GdlExportPrintArrayDescr(v->value.arr);
		printf("\t%s\n", IDL_VarName(v));
	}
//--------------------------------------------
static const EXPORT_ALLTYPES IDL_zero={0};

void GDL_WillThrowAfterCleaning(const char* f, const std::string &s);
void GDL_WillReturnAfterCleaning(const std::string &s);

static DStructGDL* GetOBJ(BaseGDL* Objptr, EXPORT_LONG *rcount) {
  if (Objptr == 0 || Objptr->Type() != GDL_OBJ)
    GDL_WillReturnAfterCleaning("Objptr not of type OBJECT. Please report.");
  if (!Objptr->Scalar())
    GDL_WillReturnAfterCleaning("Objptr must be a scalar. Please report.");
  DObjGDL* Object = static_cast<DObjGDL*> (Objptr);
  DObj ID = (*Object)[0];
  try {
	*rcount=BaseGDL::interpreter->RefCountHeapObj(ID);
    return BaseGDL::interpreter->GetObjHeap(ID);
  } catch (GDLInterpreter::HeapException& hEx) {
    GDL_WillReturnAfterCleaning("Object ID <" + i2s(ID) + "> not found.");
  }

  assert(false);
  return NULL;
}

inline void checkOK(EXPORT_VPTR v) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (v == NULL ) GDL_WillReturnAfterCleaning("Internal error: null VPTR encountered in checkOK().");
	if ((v->type < 0) || (v->type > GDL_MAX_TYPE)) GDL_WillReturnAfterCleaning("Internal error: Bad variable type encountered in checkOK().");
    if ((ISARRAY(v)) && v->value.arr->n_dim > GDL_MAX_ARRAY_DIM ) GDL_WillReturnAfterCleaning("Arrays are allowed 1 - 8 dimensions.");
}
inline void checkStorable(EXPORT_VPTR v) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
    if (v->flags & (GDL_V_TEMP|GDL_V_CONST)) GDL_WillReturnAfterCleaning("Attempt to store into an expression: "+std::string(IDL_VarName(v)));
}
DLL_PUBLIC void  GDL_CDECL IDL_MessageSJE(void *value){} //do nothing (?)
DLL_PUBLIC void * GDL_CDECL IDL_MessageGJE(void){return NULL;}//do nothing (?)
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_UNDEFVAR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO) GDL_WillReturnAfterCleaning("Variable is Undefined.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOTARRAY(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Expression must be an array in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOTSCALAR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Expression must be a scalar in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOEXPR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Expression must be a named variable in this context");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOCONST(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Constant not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOFILE(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("File expression not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOCOMPLEX(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Variable is not of complex type/");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOSTRING(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("String expression not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOSTRUCT(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Struct expression not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_REQSTR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("String expression required in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOSCALAR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Scalar variable not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NULLSTR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Null string not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOPTR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Pointer expression not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOOBJREF(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Object reference expression not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOMEMINT64(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("This routine is 32-bit limited and cannot handle this many elements.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_STRUC_REQ(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Expression must be a structure in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_REQPTR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Pointer type required in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_REQOBJREF(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Object reference required in this context");}
DLL_PUBLIC void  GDL_CDECL IDL_Message_BADARRDNUM(int action){if (action!=EXPORT_MSG_INFO)GDL_WillReturnAfterCleaning("Arrays are allowed 1 - 8 dimensions");}
DLL_PUBLIC EXPORT_MEMINT  GDL_CDECL IDL_SysRtnNumEnabled(int is_function, int enabled){return 1;} //why not - this is a stub
DLL_PUBLIC void  GDL_CDECL IDL_SysRtnGetEnabledNames(int is_function, EXPORT_STRING *str, int enabled){str->slen=0;};
DLL_PUBLIC void  GDL_CDECL IDL_SysRtnEnable(int is_function, EXPORT_STRING *names, EXPORT_MEMINT n, int option,  EXPORT_SYSRTN_GENERIC disfcn){}// do nothing
DLL_PUBLIC EXPORT_SYSRTN_GENERIC  GDL_CDECL IDL_SysRtnGetRealPtr(int is_function, char *name){return NULL;}
DLL_PUBLIC char * GDL_CDECL IDL_SysRtnGetCurrentName(void){return NULL;};
DLL_PUBLIC int  GDL_CDECL IDL_LMGRLicenseInfo(int iFlags){return 1;}
DLL_PUBLIC int  GDL_CDECL IDL_LMGRSetLicenseInfo(int iFlags){return 1;}
DLL_PUBLIC int  GDL_CDECL IDL_LMGRLicenseCheckoutUnique(char *szFeature, char *szVersion){return 1;}
DLL_PUBLIC int  GDL_CDECL IDL_LMGRLicenseCheckout(char *szFeature, char *szVersion){return 1;}
DLL_PUBLIC void  GDL_CDECL IDL_Win32MessageLoop(int fFlush){}
DLL_PUBLIC int  GDL_CDECL IDL_Win32Init(EXPORT_INIT_DATA_OPTIONS_T iOpts, void  *hinstExe, void *hwndExe, void *hAccel){return 1;}
DLL_PUBLIC void  GDL_CDECL IDL_WinPostInit(void){}
DLL_PUBLIC void  GDL_CDECL IDL_WinCleanup(void){}

EXPORT_STRUCT_TAG_DEF* GDL_Make_EXPORT_STRUCT_TAG_DEF(DStructGDL* gdlstruct);
EXPORT_StructDefPtr  GDL_CDECL IDL_MakeStruct(char *name, EXPORT_STRUCT_TAG_DEF *tags);
EXPORT_VPTR  GDL_CDECL IDL_ImportArray(int n_dim, EXPORT_MEMINT dim[], int type, UCHAR *data, EXPORT_ARRAY_FREE_CB free_cb,  EXPORT_StructDefPtr s);



DLL_PUBLIC void  GDL_CDECL IDL_Deltmp(GDL_REGISTER EXPORT_VPTR v) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (v) {
		if (ISTEMP(v)) {
			if (v->flags & GDL_V_DYNAMIC) {
				if (v->value.arr != NULL) {
					if (v->value.arr->free_cb != NULL && v->value.arr->data) v->value.arr->free_cb(v->value.arr->data); //else free(v->value.arr->data);//No as long as we are not sure this has not been allocated by us (and released by GDL)
					MyFree(v->value.arr);
					v->value.arr = NULL;
					if (v->type == GDL_TYP_STRUCT) {
						MyFree(v->value.s.sdef);
						v->value.s.sdef = NULL;
					}
				} else if (v->type == GDL_TYP_STRING) {
					if (v->value.str.slen != 0 && v->value.str.stype == 1) {
						//free(v->value.str.s);//No as long as we are not sure this has not been allocated by us (and released by GDL)
						v->value.str.s = NULL;
						v->value.str.slen = 0;
						v->value.str.stype = 0;
					}
				}
			}
			v->flags=0;
			v->type=0;
		}
	}
	//TEMPORARY EXPORT_VARIABLE itself will be destroyed at exit from function/procedure by GDL_FreeResources()
}

//Used by GDL_FreeResources(), like IDL_Deltmp, but deletes the EXPORT_VARIABLE itself
DLL_PUBLIC void GDL_CDECL GDL_DeleteDescriptors(GDL_REGISTER EXPORT_VPTR v) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		if (v && (v->flags & GDL_V_DYNAMIC) ) {
			if (ISARRAY(v) ) {
				if (v->value.arr->free_cb != NULL && v->value.arr->data) v->value.arr->free_cb(v->value.arr->data);
			}
		}
}

DLL_PUBLIC void  GDL_CDECL IDL_Freetmp(GDL_REGISTER EXPORT_VPTR v) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	IDL_Deltmp(v);
}
void  GDL_CDECL IDL_Print(int argc, EXPORT_VPTR *argv, char *argk);
} //extern "C"
void GDL_FreeResources() {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		
	bool message = true;
    for (auto it = GlobalVPTRList.begin(); it != GlobalVPTRList.end(); ++it) {
		EXPORT_VPTR v=(*it);
    	printf(" GlobalVPTRList: %s\n",IDL_VarName(v));
		IDL_Print(1,&v,NULL);
	}
//		if ((*it) == NULL) continue;
//		//if ((*it)->flags & GDL_V_TEMP) continue;
//		if ((*it)->flags & GDL_V_DYNAMIC) {
//			if (message) { message=false;
//				Message("Temporary variables are still checked out - cleaning up...");
//			}
//			char* varname=IDL_VarName((*it));
//			if (strlen(varname) > 0) fprintf(stderr,"%s\n",varname);
//		}
//		GDL_DeleteDescriptors(*it);
//	}
    FreeIntermediateMemory(); //removes all memory allocated in routine called
	GlobalVPTRList.clear();
}

void GDL_WillThrowAfterCleaning(const char *f, const std::string &s) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	std::cerr << " Unexpected error happened at \"" << f << "\", message is: \"" << s << "\"." << std::endl;
#if !defined(_WIN32) || defined(__CYGWIN__)
	//backtrace is not present under windows. If you need a backtrace, please provide patch for windows.
	int nptrs = backtrace(bt_buffer, BT_BUF_SIZE);
	fprintf(stderr, "backtrace:\n");
	backtrace_symbols_fd(bt_buffer, nptrs, STDERR_FILENO);
#endif
	GDL_FreeResources();
	longjmp(callerEnv, JUMP_THROW);
}

void GDL_WillReturnAfterCleaning(const std::string &s) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	Message(s);
	GDL_FreeResources();
	longjmp(callerEnv, JUMP_RETURN);
}

inline EXPORT_ARRAY* NewExportArray() {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) EXPORT_ARRAY* ret=new EXPORT_ARRAY(); memset(ret,0,sizeof(EXPORT_ARRAY));return ret;}

inline EXPORT_VPTR NewVPTR(UCHAR flag=0, EXPORT_StructDefPtr structdefptr=NULL) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	EXPORT_VPTR ret = new EXPORT_VARIABLE();
    //zero everything
    memset(ret,0,sizeof(EXPORT_VARIABLE));
	ret->flags =flag;
	if (flag & GDL_V_STRUCT  ) {
		ret->type = GDL_TYP_STRUCT;
		ret->flags |= GDL_V_DYNAMIC;
		ret->flags |= GDL_V_ARR;
		ret->value.arr= NewExportArray();
		ret->value.arr->flags = GDL_A_NO_GUARD;
		ret->value.s.sdef = structdefptr;
	} else if (flag & GDL_V_ARR) {
		ret->value.arr = NewExportArray();
		ret->flags |= GDL_V_DYNAMIC;
		ret->flags |= GDL_V_ARR;
		ret->value.arr->flags = GDL_A_NO_GUARD;
	}
	return ret;
}
//Alias expressing the fact that these are not TEMPO VPTRS.
inline EXPORT_VPTR NewNAMEDVPTR(UCHAR flag=0, EXPORT_StructDefPtr structdefptr=NULL){ 	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	EXPORT_VPTR v=NewVPTR(flag,structdefptr);
	return v;
}
inline EXPORT_VPTR NewTMPVPTR(UCHAR flag=0, EXPORT_StructDefPtr structdefptr=NULL) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	return NewVPTR(flag|GDL_V_TEMP,structdefptr);
}
inline EXPORT_VPTR NewTMPVPTRARRAY() {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	return NewTMPVPTR(GDL_V_ARR|GDL_V_DYNAMIC);
}
inline EXPORT_VPTR NewTMPVPTRARRAYWithCB(EXPORT_ARRAY_FREE_CB free_cb) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	EXPORT_VPTR v=NewTMPVPTR(GDL_V_ARR|GDL_V_DYNAMIC);
	v->value.arr->free_cb=free_cb;
	return v;
}
inline EXPORT_VPTR NewTMPVPTRSTRUCT(EXPORT_StructDefPtr structdefptr=NULL) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	return NewTMPVPTR(GDL_V_STRUCT,structdefptr);
}
inline EXPORT_VPTR NewTMPVPTRSTRUCTWithCB(EXPORT_StructDefPtr structdefptr=NULL, EXPORT_ARRAY_FREE_CB free_cb=NULL) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	EXPORT_VPTR v=NewTMPVPTR(GDL_V_STRUCT,structdefptr);
	v->value.s.arr->free_cb=free_cb;
	return v;
}
inline EXPORT_VPTR NewTMPVPTRFromGDL(bool kw=false, bool global=false) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
EXPORT_VPTR ret;
if (global)  ret=NewNAMEDVPTR();  else ret=NewTMPVPTR();
if (kw && global) GlobalVPTRKwList.push_back(ret); else if (global) GlobalVPTRList.push_back(ret);
return ret;
}	

void CopyStdStringToExportString(SizeT & offset,  BaseGDL* var);
void DumpElement(SizeT &offset, int i, int n, BaseGDL* v, EXPORT_StructDefPtr s);
SizeT GdlStructDump(DStructGDL* gdlStruct, SizeT &offset, EXPORT_StructDefPtr s) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	SizeT nTags = gdlStruct->NTags();
	SizeT nEl = gdlStruct->N_Elements();
	SizeT ret=0;
	if (offset == 0) {
		SizeT toAlloc=nEl*s->length;
		offset = (SizeT) MyCallocDestroyedOnExit(1,toAlloc);
		ret=offset; //memorize initial offset as it must be returned 
	}
	for (SizeT e = 0; e < nEl; ++e) {
		SizeT startOffset=offset; 
		for (SizeT tIx = 0; tIx < nTags; ++tIx) {
			BaseGDL* actEl = gdlStruct->GetTag(tIx, e);
			DumpElement(startOffset, tIx, nTags, actEl, s);
		}
		offset=startOffset+s->length;
	}
	return ret;
}

void CopyStdStringToExportString(SizeT & offset, BaseGDL* var){	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
			const DStringGDL* gdlstr=static_cast<DStringGDL*>(var);
			SizeT nEl=gdlstr->N_Elements();
			void* stringdescPtrs=MyMallocDestroyedOnExit(nEl*sizeof(EXPORT_STRING*));
			EXPORT_STRING** p=(EXPORT_STRING**)stringdescPtrs;
			for (SizeT i=0; i< nEl; ++i) p[i]=(EXPORT_STRING*)(offset+(i*sizeof(EXPORT_STRING)));
			for (auto i=0; i< nEl; ++i) {
				p[i]->slen = ((*gdlstr)[i]).size();
				p[i]->stype=1;
				if( p[i]->slen > 0) {
					p[i]->s = (char*) MyMallocDestroyedOnExit(p[i]->slen + 1);
				    strncpy(p[i]->s, (*gdlstr)[i].c_str(), p[i]->slen + 1);
				} else p[i]->s=0;
			}
//			offset+=nEl*sizeof(EXPORT_STRING);
}

void DumpElement(SizeT &offset, int iTag, int nTags, BaseGDL* gdlVar, EXPORT_StructDefPtr s ) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
        SizeT where=offset+s->tags[iTag].offset;
		switch (gdlVar->Type()) {
		case GDL_STRUCT:
			GdlStructDump(static_cast<DStructGDL*>(gdlVar), where, s->tags[iTag].var.value.s.sdef);
			break;
		case GDL_STRING:
			CopyStdStringToExportString(where,  gdlVar);
			break;
		default:
			memcpy((void*) where, gdlVar->DataAddr(), gdlVar->NBytes());
	}
}

EXPORT_VPTR GDL_ToVPTR(BaseGDL* var, bool global = false, bool is_kw = false, EXPORT_ARRAY* newArrayDescr = NULL) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	EXPORT_VPTR v;
	if (var == NULL) {
		v = NewTMPVPTRFromGDL(is_kw, global);
		v->type = GDL_TYP_UNDEF;
		return v;
	}
	if (var == NullGDL::GetSingleInstance()) {
		v = NewTMPVPTRFromGDL(is_kw, global);
		v->type = GDL_TYP_LONG; // any type, as the NULL flags is checked.
		v->flags |= GDL_V_NULL;
		return v;
	}

	if (var->Type() == GDL_OBJ) {
		EXPORT_HEAP_VPTR h = (EXPORT_HEAP_VPTR) MyMallocDestroyedOnExit(sizeof (EXPORT_HEAP_VARIABLE));
		EXPORT_LONG refcount;
		DStructGDL* objstruct = GetOBJ(var, &refcount);
		EXPORT_STRUCT_TAG_DEF* exportStructTagDef = GDL_Make_EXPORT_STRUCT_TAG_DEF(objstruct);
		EXPORT_StructDefPtr s;
		if (objstruct->Desc()->IsUnnamed()) {
			s = IDL_MakeStruct(NULL, exportStructTagDef);
		} else {
			s = IDL_MakeStruct((char*) (objstruct->Desc()->Name().c_str()), exportStructTagDef);
		}
		SizeT nelts = objstruct->N_Elements();
		SizeT arr = 0;
		SizeT ret = GdlStructDump(objstruct, arr, s);
		h->hash = NULL;
		h->flags = 0;
		h->refcount = refcount;
		h->hash_id = MimickedHeap.size();
		h->var = *IDL_ImportArray(1, &nelts, GDL_TYP_STRUCT, (UCHAR*) ret, 0, s);
		MimickedHeap[h->hash_id] = h;
		EXPORT_VPTR v = NewTMPVPTRFromGDL(is_kw, global);
		v->type = GDL_TYP_OBJREF;
		v->value.hvid = h->hash_id;
		return v;
	}

	if (var->Type() == GDL_PTR) {
		DPtrGDL* varPtr = static_cast<DPtrGDL*> (var);
		DPtr actPtrID = (*varPtr)[0];
		if (!DInterpreter::PtrValid(actPtrID)) GDL_WillThrowAfterCleaning(__func__, "GDL_ToVPTR: Wrong PTR.");
		EXPORT_HEAP_VPTR h = (EXPORT_HEAP_VPTR) MyMallocDestroyedOnExit(sizeof (EXPORT_HEAP_VARIABLE));
		h->hash = 0;
		h->flags = 0;
		h->hash_id = MimickedHeap.size();
		h->refcount = BaseGDL::interpreter->RefCountHeap(actPtrID);
		h->var = *GDL_ToVPTR(DInterpreter::GetHeap(actPtrID));
		MimickedHeap[h->hash_id] = h;
		EXPORT_VPTR v = NewTMPVPTRFromGDL(is_kw, global);
		v->type = GDL_TYP_PTR;
		v->value.hvid = h->hash_id;
		return v;
	}
	if (var->Type() == GDL_STRUCT) {
		DStructGDL *gdlstruct = static_cast<DStructGDL*> (var);
		EXPORT_STRUCT_TAG_DEF* exportStructTagDef = GDL_Make_EXPORT_STRUCT_TAG_DEF(gdlstruct);
		EXPORT_StructDefPtr s;
		if (gdlstruct->Desc()->IsUnnamed()) {
			s = IDL_MakeStruct(NULL, exportStructTagDef);
		} else {
			s = IDL_MakeStruct((char*) (gdlstruct->Desc()->Name().c_str()), exportStructTagDef);
		}
		SizeT nelts = (newArrayDescr == NULL) ? gdlstruct->N_Elements() : newArrayDescr->n_elts;
		SizeT arr = 0;
		SizeT ret = GdlStructDump(gdlstruct, arr, s);
		v = IDL_ImportArray(1, &nelts, GDL_TYP_STRUCT, (UCHAR*) ret, 0, s);
		return v;
	}
	v = NewTMPVPTRFromGDL(is_kw, global);
	if (var->Rank() == 0) {
		switch (var->Type()) {
			case GDL_UNDEF:
				v->type = GDL_TYP_UNDEF;
				v->flags |= GDL_V_NULL;
				break;
			case GDL_BYTE:
				v->type = GDL_TYP_BYTE;
				v->value.c = (*static_cast<DByteGDL*> (var))[0];
				break;
			case GDL_INT:
				v->type = GDL_TYP_INT;
				v->value.i = (*static_cast<DIntGDL*> (var))[0];
				break;
			case GDL_LONG:
				v->type = GDL_TYP_LONG;
				v->value.l = (*static_cast<DLongGDL*> (var))[0];
				break;
			case GDL_LONG64:
				v->type = GDL_TYP_LONG64;
				v->value.l64 = (*static_cast<DLong64GDL*> (var))[0];
				break;
			case GDL_UINT:
				v->type = GDL_TYP_UINT;
				v->value.ui = (*static_cast<DUIntGDL*> (var))[0];
				break;
			case GDL_ULONG:
				v->type = GDL_TYP_ULONG;
				v->value.ul = (*static_cast<DULongGDL*> (var))[0];
				break;
			case GDL_ULONG64:
				v->type = GDL_TYP_ULONG64;
				v->value.ul64 = (*static_cast<DULong64GDL*> (var))[0];
				break;
			case GDL_FLOAT:
				v->type = GDL_TYP_FLOAT;
				v->value.f = (*static_cast<DFloatGDL*> (var))[0];
				break;
			case GDL_DOUBLE:
				v->type = GDL_TYP_DOUBLE;
				v->value.d = (*static_cast<DDoubleGDL*> (var))[0];
				break;
			case GDL_COMPLEX:
			{
				v->type = GDL_TYP_COMPLEX;
				DComplex c = (*static_cast<DComplexGDL*> (var))[0];
				v->value.cmp.r = c.real();
				v->value.cmp.i = c.imag();
				break;
			}
			case GDL_COMPLEXDBL:
			{
				v->type = GDL_TYP_DCOMPLEX;
				DComplexDbl c = (*static_cast<DComplexDblGDL*> (var))[0];
				v->value.dcmp.r = c.real();
				v->value.dcmp.i = c.imag();
				break;
			}
			case GDL_STRING:
			{
				v->type = GDL_TYP_STRING;
				DString s = (*static_cast<DStringGDL*> (var))[0];
				v->value.str.slen = s.size();
				if (v->value.str.slen > 0) {
					v->value.str.s = (char*) MyMallocDestroyedOnExit(v->value.str.slen + 1);
					strncpy(v->value.str.s, s.c_str(), v->value.str.slen + 1);
				}
				break;
			}
			case GDL_PTR:
			{
				v->type = GDL_TYP_PTR;
				v->value.ptrint = (*static_cast<DPtrGDL*> (var))[0];
				break;
			}
			default: GDL_WillThrowAfterCleaning(__func__, "GDL_ToVPTR: unsupported case.");
		}
	} else {
		v->flags |= (GDL_V_ARR | GDL_V_DYNAMIC);
		EXPORT_ARRAY* arraydescr = NewExportArray();
		v->value.arr = arraydescr;
		if (newArrayDescr == NULL) {
			arraydescr->arr_len = var->NBytes();
			//	  v->flags2 = GDL_V_SHAREDDATA; //data in GDL_VPTR is the same as the corresponding GDL variable
			for (int i = 0; i < var->Rank(); ++i) arraydescr->dim[i] = var->Dim(i);
			arraydescr->n_dim = var->Rank();
			arraydescr->n_elts = var->N_Elements();
			arraydescr->offset = 0;
		} else {
			memcpy(arraydescr, newArrayDescr, sizeof (EXPORT_ARRAY));
		}
		switch (var->Type()) {
			case GDL_UNDEF:
				v->type = GDL_TYP_UNDEF;
				arraydescr->elt_len = 0;
				break;
			case GDL_BYTE:
				v->type = GDL_TYP_BYTE;
				arraydescr->elt_len = 1;
				break;
			case GDL_INT:
				v->type = GDL_TYP_INT;
				arraydescr->elt_len = 2;
				break;
			case GDL_LONG:
				v->type = GDL_TYP_LONG;
				arraydescr->elt_len = 4;
				break;
			case GDL_LONG64:
				v->type = GDL_TYP_LONG64;
				arraydescr->elt_len = 8;
				break;
			case GDL_UINT:
				v->type = GDL_TYP_UINT;
				arraydescr->elt_len = 2;
				break;
			case GDL_ULONG:
				v->type = GDL_TYP_ULONG;
				arraydescr->elt_len = 4;
				break;
			case GDL_ULONG64:
				v->type = GDL_TYP_ULONG64;
				arraydescr->elt_len = 8;
				break;
			case GDL_FLOAT:
				v->type = GDL_TYP_FLOAT;
				arraydescr->elt_len = 4;
				break;
			case GDL_DOUBLE:
				v->type = GDL_TYP_DOUBLE;
				arraydescr->elt_len = 8;
				break;
			case GDL_COMPLEX:
			{
				v->type = GDL_TYP_COMPLEX;
				arraydescr->elt_len = 8;
				break;
			}
			case GDL_COMPLEXDBL:
			{
				v->type = GDL_TYP_DCOMPLEX;
				arraydescr->elt_len = 16;
				break;
			}
			case GDL_STRING:
			{
				v->type = GDL_TYP_STRING;
				SizeT nEl = arraydescr->n_elts;
				arraydescr->elt_len = sizeof (EXPORT_STRING);
				arraydescr->arr_len = arraydescr->n_elts * arraydescr->elt_len;
				void* allstringdescr = MyMallocDestroyedOnExit(arraydescr->arr_len);
				memset(allstringdescr, 0, arraydescr->arr_len);
				arraydescr->data = (UCHAR*) (allstringdescr);
				void* stringdescPtrs = MyMallocDestroyedOnExit(arraydescr->n_elts * sizeof (EXPORT_STRING*));
				//pointers to EXPORT_STRINGS
				EXPORT_STRING** p = (EXPORT_STRING**) stringdescPtrs;
				for (SizeT i = 0; i < arraydescr->n_elts; ++i) p[i] = (EXPORT_STRING*) ((SizeT) allstringdescr + (i * sizeof (EXPORT_STRING)));
				DStringGDL* gdlstr = (DStringGDL*) var;
				for (auto i = 0; i < arraydescr->n_elts; ++i) {
					p[i]->slen = ((*gdlstr)[i]).size();
					if (p[i]->slen > 0) {
						p[i]->s = (char*) MyMallocDestroyedOnExit(p[i]->slen + 1);
						strncpy(p[i]->s, (*gdlstr)[i].c_str(), p[i]->slen + 1);
					}
				}
				return v;
				break;
			}
			default: GDL_WillThrowAfterCleaning(__func__, "GDL_ToVPTR: unsupported case.");
		}
		arraydescr->arr_len = arraydescr->n_elts * arraydescr->elt_len;
		arraydescr->data = (UCHAR*) MyMallocDestroyedOnExit(arraydescr->arr_len);
		memcpy(arraydescr->data, var->DataAddr(), arraydescr->arr_len);
	}
	return v;
} 
	
// from RESTORE	
void restoreNormalVariable(std::string varName, BaseGDL* ret) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
    //write variable back
    EnvStackT& callStack = DInterpreter::CallStack();
    DLong curlevnum = callStack.size();
    DSubUD* pro = static_cast<DSubUD*> (callStack[curlevnum - 1]->GetPro());
    int nKey = pro->NKey();
    int xI = pro->FindVar(varName);
    SizeT s;
    if (xI != -1)
    {
      s = xI;
     GDLDelete( ((EnvT*) (callStack[curlevnum - 1]))->GetPar(s - nKey));
      ((EnvT*) (callStack[curlevnum - 1]))->GetPar(s - nKey) = ret;

    } else
    {
      BaseGDL** varPtr = pro->GetCommonVarPtr(varName);
      if (varPtr)
      {
        pro->ReplaceExistingCommonVar(varName, ret);
      } else
      {
        SizeT u = pro->AddVar(varName);
        s = callStack[curlevnum - 1]->AddEnv();
        ((EnvT*) (callStack[curlevnum - 1]))->GetPar(s - nKey) = ret;
      }
    }
  }
  
DStringGDL* GDL_GetString(EXPORT_VPTR v, EXPORT_ARRAY* newArrayDescr=NULL) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	checkOK(v);
 		  dimension *dim;
		  EXPORT_STRING* ss;
		if (ISARRAY(v)) {
			EXPORT_ARRAY* arraydescr = (newArrayDescr)?newArrayDescr:v->value.arr;
			if (arraydescr) {
				SizeT rank = arraydescr->n_dim;
				SizeT arraydim[rank];
				for (int i = 0; i < rank; ++i) arraydim[i] = arraydescr->dim[i];
				dim = new dimension(arraydim, rank);
				ss=(EXPORT_STRING*)(v->value.arr->data);
			} else {
				dim = new dimension();
				ss = &(v->value.str);
			}
	    } else {
			dim = new  dimension();
			ss=&(v->value.str);
		}
		  DStringGDL* gdls=new DStringGDL(*dim,BaseGDL::NOZERO);
//strings copy data
			for (auto i=0; i< dim->NDimElements(); ++i) {
				if ( ss[i].slen > 0) 
					(*gdls)[i]=std::string(ss[i].s,ss[i].slen);
			}
			return gdls;
}
extern "C" {
#define C_ (char*)
char *IDL_TypeName[] = {C_ "UNDEFINED",C_ "BYTE     ",C_ "INT      ",C_ "LONG     ",C_ "FLOAT    ",C_ "DOUBLE   ",C_ "COMPLEX  ",C_ "STRING   ",C_ "STRUCT   ",C_ "DCOMPLEX ",C_ "POINTER  ",C_ "OBJREF   ",C_ "UINT     ",C_ "ULONG    ",C_ "LONG64   ",C_ "ULONG64  "};
char *IDL_OutputFormat[EXPORT_NUM_TYPES] = {C_"<Undefined>", C_"%4d", C_"%8d", C_"%12d", C_"%#13.6g", C_"%#16.8g", C_"(%#13.6g,%#13.6g)", C_"%s", C_"", C_"(%#16.8g,%#16.8g)", C_"%12lu", C_"%12lu", C_"%8u", C_"%12u", C_"%22ld", C_"%22lu"};
int IDL_OutputFormatLen[EXPORT_NUM_TYPES] = {11, 4, 8, 12, 13, 16, 29, 0, 0, 35, 12, 12, 8, 12, 22, 22};
char *IDL_OutputFormatFull[EXPORT_NUM_TYPES] = {C_"<Undefined>", C_"%4d", C_"%8d", C_"%12d", C_"%#16.8g", C_"%#25.17g", C_"(%#16.8g,%#16.8g)", C_"%s", C_"", C_"(%#25.17g,%#25.17g)", C_"%12lu", C_"%12lu", C_"%8u", C_"%12u", C_"%22ld", C_"%22lu" };
int IDL_OutputFormatFullLen[EXPORT_NUM_TYPES] = {11, 4, 8, 12, 16, 25, 35, 0, 0, 53, 12, 12, 8, 12, 22, 22};
char *EXPORT_InputFormat[EXPORT_NUM_TYPES] = {C_"", C_"%i", C_"%i", C_"%i", C_"%g", C_"%g",  C_"(%g,%g)", C_"%s", C_"", C_"(%g,%g)", C_"%d", C_"%d", C_"%d", C_"%d", C_"%ld", C_"%lu"};
char *IDL_OutputFormatNatural[EXPORT_NUM_TYPES] = {C_"<Undefined>", C_"%d", C_"%d", C_"%d", C_"%g", C_"%g", C_"(%g,%g)", C_"%s", C_"", C_"(%g,%g)", C_"%u", C_"%u", C_"%u", C_"%u", C_"%ld", C_"%lu", };

DLL_PUBLIC char * GDL_CDECL IDL_OutputFormatFunc(int type) {
	if (type > EXPORT_MAX_TYPE) GDL_WillThrowAfterCleaning(__func__ , "type must be > 0 and < 16");
	return IDL_OutputFormat[type];
}
int IDL_OutputFormatLenFunc(int type) {
	if (type > EXPORT_MAX_TYPE) GDL_WillThrowAfterCleaning(__func__ , "type must be > 0 and < 16");
	return IDL_OutputFormatLen[type];
}

// I define IDL_TypeSize[EXPORT_TYP_UNDEF] to SizeT as it is used in GDLZeroAtAddr() used by GdlExportPresentKeyword() etc for READWRITE and usually is of pointer size.
// If this causes problems, then we'll have to think twice.
EXPORT_LONG IDL_TypeSize[EXPORT_NUM_TYPES] = {sizeof (EXPORT_MEMINT), 1, sizeof (EXPORT_INT), sizeof (EXPORT_LONG), sizeof (float), sizeof (double),
	sizeof (EXPORT_COMPLEX), sizeof (EXPORT_STRING), sizeof (EXPORT_SREF), sizeof (EXPORT_DCOMPLEX), sizeof (EXPORT_MEMINT), sizeof (EXPORT_MEMINT),
	sizeof ( EXPORT_UINT), sizeof (EXPORT_ULONG), sizeof (EXPORT_LONG64), sizeof (EXPORT_ULONG64)};

EXPORT_LONG GDL_TypeAlignment[EXPORT_NUM_TYPES] = {0, 1, sizeof (EXPORT_INT), sizeof (EXPORT_LONG), sizeof (float), sizeof (double),
	sizeof (float), sizeof (EXPORT_MEMINT), sizeof (EXPORT_MEMINT), sizeof (double), sizeof (EXPORT_MEMINT), sizeof (EXPORT_MEMINT),
	sizeof ( EXPORT_UINT), sizeof (EXPORT_ULONG), sizeof (EXPORT_LONG64), sizeof (EXPORT_ULONG64)};

DLL_PUBLIC int GDL_CDECL IDL_TypeSizeFunc(int type) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (type > EXPORT_MAX_TYPE) GDL_WillThrowAfterCleaning(__func__ , "type must be > 0 and < 16");
#if defined(TRACE_OPCALLS)
if (type==GDL_TYP_UNDEF) std::cerr<<"Warning, type UNDEF used in IDL_TypeSizeFunc()"<<std::endl;
#endif
	return IDL_TypeSize[type];
}


DLL_PUBLIC char * GDL_CDECL IDL_TypeNameFunc(int type) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (type > EXPORT_MAX_TYPE) GDL_WillThrowAfterCleaning(__func__ , "type must be > 0 and < 16");
	return IDL_TypeName[type];
}
#undef C_
}
void StructFillVariableData(EXPORT_MEMINT baseData, EXPORT_VPTR v, int t, BaseGDL* var) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	checkOK(v);
	SizeT nEl = var->N_Elements();
	EXPORT_MEMINT off = v->value.s.sdef->tags[t].offset; 
	EXPORT_MEMINT dataset = baseData + off;
	if (v->value.s.sdef->tags[t].var.flags & GDL_V_STRUCT) {
		DStructGDL* myStruct = static_cast<DStructGDL*> (var);
		SizeT nTags = myStruct->Desc()->NTags();
		for (SizeT ix = 0; ix < nEl; ++ix) for (SizeT tt = 0; tt < nTags; ++tt) StructFillVariableData(dataset, &(v->value.s.sdef->tags[t].var), tt, myStruct->GetTag(tt, ix));
	} else if (v->value.s.sdef->tags[t].var.flags & GDL_V_ARR) {
		if (v->value.s.sdef->tags[t].var.type == GDL_TYP_STRING) {
            EXPORT_STRING* ss = (EXPORT_STRING*) dataset;
			for (auto i = 0; i < nEl; ++i) {
				if (ss[i].slen > 0) (*static_cast<DStringGDL*> (var))[i] = std::string(ss[i].s, ss[i].slen);
			}
		} else memcpy(var->DataAddr(), (void*) dataset, v->value.s.sdef->tags[t].var.value.arr->arr_len);
	} else if (v->value.s.sdef->tags[t].var.type == GDL_TYP_STRING) {
		EXPORT_STRING* ss = (EXPORT_STRING*) dataset;
		if (ss->slen > 0) (*static_cast<DStringGDL*> (var))[0] = std::string(ss->s, ss->slen);
	} else memcpy(var->DataAddr(), (void*) dataset, IDL_TypeSize[v->value.s.sdef->tags[t].var.type]);
}
  
#define DOCASE(type, gdltype, tagname, pardim)\
 case type: { gdltype entry(pardim); stru_desc->AddTag(std::string(tagname), &entry);} break;

DStructGDL* GDL_MakeGDLStruct(EXPORT_VPTR v, dimension &inputdim);
DStructDesc * GDL_GetStructDesc(EXPORT_VPTR v, dimension &inputdim) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	checkOK(v);
	std::string struname("$truct");
	if (v->value.s.sdef->id != NULL && v->value.s.sdef->id->name != NULL) struname=std::string(v->value.s.sdef->id->name,v->value.s.sdef->id->len); 
	DStructDesc * stru_desc = new DStructDesc(struname);
	//summary & tag population:
	for (int i = 0; i < v->value.s.sdef->ntags; ++i) {
		dimension *dim;
		if (v->value.s.sdef->tags[i].var.flags & GDL_V_ARR) {
			EXPORT_ARRAY* arraydescr = v->value.s.sdef->tags[i].var.value.arr;
			if (arraydescr) {
				SizeT rank = arraydescr->n_dim;
				SizeT arraydim[rank];
				for (int j = 0; j < rank; ++j) arraydim[j] = arraydescr->dim[j];
				dim = new dimension(arraydim, rank);
			} else dim = new dimension();
		} else dim = new dimension();
		switch (v->value.s.sdef->tags[i].var.type) {
				DOCASE(GDL_TYP_BYTE, SpDByte, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_INT, SpDInt, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_LONG, SpDLong, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_FLOAT, SpDFloat, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_DOUBLE, SpDDouble, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_COMPLEX, SpDComplex, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_DCOMPLEX, SpDComplexDbl, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_STRING, SpDString, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_PTR, DPtrGDL, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_OBJREF, DObjGDL, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_UINT, SpDUInt, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_ULONG, SpDULong, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_ULONG64, SpDULong64, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(GDL_TYP_LONG64, SpDLong64, v->value.s.sdef->tags[i].id->name, *dim)

			case GDL_TYP_STRUCT:
			{
				DStructDesc * desc =  GDL_GetStructDesc(&(v->value.s.sdef->tags[i].var), *dim);
				if (desc == NULL) GDL_WillThrowAfterCleaning(__func__ , "GDL_GetStructDesc: NULL substructure descriptor, abort.");
				stru_desc->AddTag(std::string(v->value.s.sdef->tags[i].id->name), new DStructGDL(desc, *dim));
				break;
			}

			default:
				GDL_WillThrowAfterCleaning(__func__ , "GDL_GetStructDesc: unsupported case.");
				break;
		}
	}

	return stru_desc;
}
#undef DOCASE

DStructGDL* GDL_MakeGDLStruct(EXPORT_VPTR v, dimension &inputdim) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	DStructDesc * stru_desc = GDL_GetStructDesc(v, inputdim);
	DStructGDL* var = new DStructGDL(stru_desc, inputdim);
	u_int nEl = var->N_Elements();
	SizeT nTags = var->Desc()->NTags();
	EXPORT_MEMINT running_offset= (EXPORT_MEMINT) v->value.s.arr->data;
	if (running_offset == 0) GDL_WillThrowAfterCleaning(__func__ , "GDL_MakeGDLStruct: null pointer.");
	EXPORT_MEMINT offset=0;
	int k=0;
	for (SizeT ix = 0; ix < nEl; ++ix) {
		for (SizeT t = 0; t < nTags; ++t) {
			EXPORT_MEMINT doffset = offset;
			StructFillVariableData(running_offset,v, t, var->GetTag(t, ix));
			EXPORT_MEMINT incr = running_offset - doffset;
			doffset = running_offset;
		}
		running_offset +=  v->value.s.sdef->length;

	}
	return var;

}

#define DOCASE(type, gdltype, element)\
 case type: return new gdltype(v->value.element);

#define DOCASE_ARRAY(type, gdltype)\
 case type: var = (IsADefinedVar)? new gdltype(dim, BaseGDL::NOALLOC):new gdltype(dim, BaseGDL::NOZERO); break;

  
BaseGDL* VPTR_ToGDL(EXPORT_VPTR v, bool protectSourceVPTR=false, EXPORT_ARRAY* newArrayDescr=NULL) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	checkOK(v);
	if (v->type == GDL_TYP_UNDEF) {
		return NullGDL::GetSingleInstance();
	}
	if (v->flags & GDL_V_NULL) {
		return NullGDL::GetSingleInstance();
	}
	// string and structs special call as they must change the data flow (due to STRINGS!)
	if (v->type == GDL_TYP_STRING) return GDL_GetString(v, newArrayDescr);
	else if (v->flags & GDL_V_STRUCT) {
		if (protectSourceVPTR) v->flags &= ~GDL_V_TEMP; //will no destroy data
		EXPORT_ARRAY* arraydescr = (newArrayDescr)?newArrayDescr:v->value.arr;
		SizeT rank = arraydescr->n_dim;
		SizeT arraydim[rank];
		for (int i = 0; i < rank; ++i) arraydim[i] = arraydescr->dim[i];
		dimension dim(arraydim, rank);
		return GDL_MakeGDLStruct(v, dim);
	} else if (ISARRAY(v)) {
		EXPORT_ARRAY* arraydescr = (newArrayDescr)?newArrayDescr:v->value.arr;
		//DEFINED vars data will not be destroyed by GDL when the function returns.
		//Additionnally I check the data is aligned but this should be the case unless the user
		//has overwritten the pointer.
        //bool IsADefinedVar=( isTemp(v) &&  (SizeT) (arraydescr->data) %16 == 0);
		bool IsADefinedVar=false; //WELL THIS IS NOT OK due to e->SetPar() in returning. Better to get some memory leak for the moment. was: ( isTemp(v) &&  (SizeT) (arraydescr->data) %16 == 0);
		if (protectSourceVPTR) v->flags &= ~GDL_V_TEMP; //will no destroy data
		SizeT rank = arraydescr->n_dim;
		SizeT arraydim[rank];
		for (int i = 0; i < rank; ++i) arraydim[i] = arraydescr->dim[i];
		dimension dim(arraydim, rank);
		BaseGDL* var;
		switch (v->type) {
			    case GDL_TYP_UNDEF:
				return NullGDL::GetSingleInstance();
				break;
				DOCASE_ARRAY(GDL_TYP_BYTE, DByteGDL);
				DOCASE_ARRAY(GDL_TYP_INT, DIntGDL);
				DOCASE_ARRAY(GDL_TYP_LONG, DLongGDL);
				DOCASE_ARRAY(GDL_TYP_FLOAT, DFloatGDL);
				DOCASE_ARRAY(GDL_TYP_DOUBLE, DDoubleGDL);
				DOCASE_ARRAY(GDL_TYP_COMPLEX, DComplexGDL);
				DOCASE_ARRAY(GDL_TYP_DCOMPLEX, DComplexDblGDL);
				DOCASE_ARRAY(GDL_TYP_UINT, DUIntGDL);
				DOCASE_ARRAY(GDL_TYP_ULONG, DULongGDL);
				DOCASE_ARRAY(GDL_TYP_LONG64, DLong64GDL);
				DOCASE_ARRAY(GDL_TYP_ULONG64, DULong64GDL);
			default: GDL_WillReturnAfterCleaning("VPTR_ToGDL: bad array case.");
		}
//	  if ((SizeT) (arraydescr->data) %16 == 0) std::cerr << "ALIGNED!!!\n"; else std::cerr << "unaligned: "<<(SizeT) (arraydescr->data)<<std::endl; 
//		if (IsADefinedVar) { //just plug in the address, saves copies and loss memory
//			var->SetCallbackFunction(v->value.arr->free_cb);
//			var->SetBuffer(arraydescr->data);
//			var->SetBufferSize(v->value.arr->n_elts); //dim.NDimElements());
//			var->SetDim(dim);
//			//if arraydescr->data is in GlobalVPTRList or GlobalVPTRKwList, remove it from these vectors, otherwise the data will be freed when the function/procedure exits.
//			bool notFound=true;
//			for (auto it = GlobalVPTRList.begin(); it != GlobalVPTRList.end(); ++it) if ((*it)==v) {
//				GlobalVPTRList.erase(it); notFound=false; break;
//			}
//			if (notFound) {
//				for (auto it = GlobalVPTRKwList.begin(); it != GlobalVPTRKwList.end(); ++it) if ((*it)==v) {
//					GlobalVPTRKwList.erase(it); break;
//				}
//			}
//		} else { // data must be copied
		  if (arraydescr->data) memcpy(var->DataAddr(),arraydescr->data, arraydescr->arr_len);
//		}
		return var;
	} else {
		switch (v->type) {
				case GDL_TYP_UNDEF:
				return NullGDL::GetSingleInstance(); break;
				DOCASE(GDL_TYP_BYTE, DByteGDL, c);
				DOCASE(GDL_TYP_INT, DIntGDL, i);
				DOCASE(GDL_TYP_LONG, DLongGDL, l);
				DOCASE(GDL_TYP_FLOAT, DFloatGDL, f);
				DOCASE(GDL_TYP_DOUBLE, DDoubleGDL, d);
				DOCASE(GDL_TYP_UINT, DUIntGDL, ui);
				DOCASE(GDL_TYP_ULONG, DULongGDL, ul);
				DOCASE(GDL_TYP_LONG64, DLong64GDL, l64);
				DOCASE(GDL_TYP_ULONG64, DULong64GDL, ul64);
			case GDL_TYP_COMPLEX:
				return new DComplexGDL(std::complex<float>(v->value.cmp.r, v->value.cmp.i));
			case GDL_TYP_DCOMPLEX:
				return new DComplexDblGDL(std::complex<double>(v->value.dcmp.r, v->value.dcmp.i));
			default: GDL_WillReturnAfterCleaning("ReturnEXPORT_VPTR_AsGDL: bad array case.");
		}
	}
	return NULL;
}
#undef DOCASE
#undef DOCASE_ARRAY

extern "C" {
void GDL_CDECL GDL_ImportArrayInExistingVPTR(EXPORT_VPTR v,int n_dim, EXPORT_MEMINT dim[], int type, UCHAR *data, EXPORT_ARRAY_FREE_CB free_cb,  EXPORT_StructDefPtr s){	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (type == GDL_TYP_STRUCT){
	 if (v==NULL) v= NewTMPVPTRSTRUCTWithCB(s,free_cb); else if (v->value.s.arr ==NULL) { //create it
				v->type = GDL_TYP_STRUCT;
				v->flags |= GDL_V_DYNAMIC;
				v->flags |= GDL_V_ARR;
				v->value.s.arr = NewExportArray();
				v->value.s.arr->flags = GDL_A_NO_GUARD;
				v->value.s.sdef = s;
	 }
		v->type = type;
		SizeT l = 1;
		for (auto i = 0; i < n_dim; ++i) {
			l *= dim[i];
			v->value.s.arr->dim[i] = dim[i];
		}
		v->value.s.arr->n_elts = l;
		v->value.s.arr->n_dim = n_dim;
		EXPORT_LONG64 sz = s->length;
		v->value.s.arr->elt_len = sz;
		v->value.s.arr->arr_len = sz*l;
		v->value.s.arr->data = data;
	} else {	
		if (v==NULL) EXPORT_VPTR v= NewTMPVPTRARRAYWithCB(free_cb); else if (v->value.arr ==NULL) { //create it
				v->value.arr = NewExportArray();
				v->flags |= GDL_V_DYNAMIC;
				v->flags |= GDL_V_ARR;
				v->value.arr->flags = GDL_A_NO_GUARD;
	 }
		v->type = type;
		SizeT l = 1;
		for (auto i = 0; i < n_dim; ++i) {
			l *= dim[i];
			v->value.arr->dim[i] = dim[i];
		}
		v->value.arr->n_elts = l;
		v->value.arr->n_dim = n_dim;
		EXPORT_LONG64 sz = IDL_TypeSizeFunc(type);
		v->value.arr->elt_len = sz;
		v->value.arr->arr_len = sz*l;
		v->value.arr->data = data;
	}
}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_ImportArray(int n_dim, EXPORT_MEMINT dim[], int type, UCHAR *data, EXPORT_ARRAY_FREE_CB free_cb,  EXPORT_StructDefPtr s){	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	EXPORT_VPTR v;
    if (type == GDL_TYP_STRUCT){
	 if (s==NULL) GDL_WillReturnAfterCleaning("IDL_ImportArray() defines a struct without passing a valid EXPORT_StructDefPtr");
     v= NewTMPVPTRSTRUCTWithCB(s,free_cb);
	} else {
	 v= NewTMPVPTRARRAYWithCB(free_cb);
	}
    GDL_ImportArrayInExistingVPTR(v, n_dim, dim, type, data,free_cb,s);
    return v;
}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_ImportNamedArray(char *name, int n_dim, EXPORT_MEMINT dim[],  int type, UCHAR *data,  EXPORT_ARRAY_FREE_CB free_cb, EXPORT_StructDefPtr s){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	EXPORT_VPTR v = IDL_ImportArray(n_dim, dim,  type, data, free_cb, s);		
	BaseGDL* gdlvar = VPTR_ToGDL(v, true); //protect data as this is passed to *MAIN* GDL
	restoreNormalVariable(std::string(name), gdlvar);
	return v;
}

}

#define DOCASE(type, gdltype)\
 case type: return new gdltype(d, mode);

  BaseGDL* CreateNewGDLArray(int type, SizeT n_dim, SizeT dim[], BaseGDL::InitType mode) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	  dimension d(dim, n_dim);
	switch (type) {
			DOCASE(GDL_TYP_BYTE, DByteGDL);
			DOCASE(GDL_TYP_INT, DIntGDL);
			DOCASE(GDL_TYP_LONG, DLongGDL);
			DOCASE(GDL_TYP_FLOAT,DFloatGDL);
			DOCASE(GDL_TYP_DOUBLE, DDoubleGDL);
			DOCASE(GDL_TYP_COMPLEX, DComplexGDL);
			DOCASE(GDL_TYP_DCOMPLEX, DComplexDblGDL);
			DOCASE(GDL_TYP_UINT, DUIntGDL);
			DOCASE(GDL_TYP_ULONG, DULongGDL);
			DOCASE(GDL_TYP_LONG64, DLong64GDL);
			DOCASE(GDL_TYP_ULONG64, DULong64GDL);
			DOCASE(GDL_TYP_STRING, DStringGDL);
		default:
			GDL_WillReturnAfterCleaning("CreateNewGDLArray failure.");
	}
	return NULL;
}
#undef DOCASE


		  extern "C" {
  EXPORT_VPTR findGDLVar(std::string varName, bool acceptNew, bool doMain=false) ;
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_FindNamedVariable(char *name, int ienter){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	std::string s(name);
	return findGDLVar(s, (ienter>0), false );
}
DLL_PUBLIC void  GDL_CDECL IDL_Print(int argc, EXPORT_VPTR *argv, char *argk);


#define DOCASE(ty, what)\
 case ty: {snprintf (&infoline[l], IDL_OutputFormatLen[v->type]+1, IDL_OutputFormat[v->type],v->value.what);break;}
#define DOCASE_STR(ty)\
 case ty: {snprintf (&infoline[l], v->value.str.slen+3 , "'%s'" ,v->value.str.s);break;}
#define DOCASE_CMP(ty, what)\
 case ty: {snprintf (&infoline[l], IDL_OutputFormatLen[v->type]+1, IDL_OutputFormat[v->type],v->value.what.r,v->value.what.i);  break;}

DLL_PUBLIC char * GDL_CDECL IDL_VarName(EXPORT_VPTR v){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
        checkOK(v);
#ifdef GDL_DEBUG
fprintf(stderr, "IDL_VARNAME() called on: "); if (v && v->type != GDL_TYP_UNDEF) IDL_Print(1, &v, NULL); else std::cerr<<"<Undefined>";
#endif		
        auto it=PassedVariablesNames.find((void*)v);
		if (it != PassedVariablesNames.end())	{
#ifdef GDL_DEBUG
			std::cerr<<" (was : "<<it->second<<")"<<std::endl;
#endif
			return (char*)(it->second).c_str();
        }
		char* infoline=(char*) MyCallocDestroyedOnExit(1,128);
		if (v->type == GDL_TYP_UNDEF) {strncat(infoline,"<UNDEFINED> ",13); return infoline;}
        strncat(infoline,"<",2);
		strncat(infoline,IDL_TypeNameFunc(v->type),9);
		if (( ISARRAY(v)) && (v->value.arr != NULL)) {
			strncat(infoline,"Array[",7);
			int i;
			for (i=0; i< v->value.arr->n_dim-1; ++i) {
				int l=strlen(infoline);
				snprintf (&infoline[l], 127-l, "%llu,",v->value.arr->dim[i]);
			}
			int l = strlen(infoline);
			snprintf(&infoline[l], 127-l, "%llu", v->value.arr->dim[i]);
			strncat(infoline,"]",2);
		} else {
			strncat(infoline,"(",2);
			int l = strlen(infoline);
			switch(v->type){
				DOCASE(GDL_TYP_BYTE, c);
				DOCASE(GDL_TYP_INT, i);
				DOCASE(GDL_TYP_LONG, l);
				DOCASE(GDL_TYP_FLOAT, f);
				DOCASE(GDL_TYP_DOUBLE, d);
				DOCASE_CMP(GDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(GDL_TYP_DCOMPLEX, dcmp);
				DOCASE(GDL_TYP_UINT, ui);
				DOCASE(GDL_TYP_ULONG, ul);
				DOCASE(GDL_TYP_LONG64, l64);
				DOCASE(GDL_TYP_ULONG64, ul64);
				DOCASE_STR(GDL_TYP_STRING);
				DOCASE(GDL_TYP_STRUCT, s.sdef->id->name);
				DOCASE(GDL_TYP_PTR, hvid);
				DOCASE(GDL_TYP_OBJREF, hvid);
			default: GDL_WillReturnAfterCleaning("IDL_VarName: unexpected type "+i2s(v->type));
			}
			strncat(infoline,")",2);
		}
        strncat(infoline,">",2);
#ifdef GDL_DEBUG
		std::cerr<<" (was : "<<infoline<<")"<<std::endl;
#endif
		return infoline;
	}
#undef DOCASE
#undef DOCASE_CMP
#undef DOCASE_STR

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GetVarAddr1(char *name, int ienter) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		std::string s(name);
		return findGDLVar(s, (ienter > 0), true);
}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GetVarAddr(char *name){	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	return IDL_GetVarAddr1(name, 0);
}

DLL_PUBLIC void  GDL_CDECL IDL_VarEnsureSimple(EXPORT_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	checkOK(v);
    static char* message= (char*)"Expression must not be a file variable, a structure variable, a pointer heap variable, or an object reference heap variable in this context: ";
	if ( v->flags & (GDL_V_STRUCT|GDL_V_FILE) ) GDL_WillReturnAfterCleaning(message+std::string(IDL_VarName(v)));
	else if ( v->type == GDL_TYP_PTR ) GDL_WillReturnAfterCleaning(message+std::string(IDL_VarName(v)));
	else if ( v->type == GDL_TYP_OBJREF ) GDL_WillReturnAfterCleaning(message+std::string(IDL_VarName(v)));
}

DLL_PUBLIC int  GDL_CDECL IDL_StructNumTags(EXPORT_StructDefPtr sdef){	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return sdef->ntags;}
DLL_PUBLIC EXPORT_MEMINT  GDL_CDECL IDL_StructTagInfoByIndex(EXPORT_StructDefPtr   sdef, int index, int msg_action, EXPORT_VPTR *var);

#define DOCASE(ty, what)\
 case ty: fprintf (stdout,IDL_OutputFormat[v->type],v->value.what);break;
#define DOCASE_ARRAY(ty, c_ty)\
 case ty: {\
 const c_ty *val=(c_ty *) ((use_address)? use_address: v->value.arr->data);\
 int w=0;\
 int l=IDL_OutputFormatLen[v->type];\
 if (val != NULL) {\
 for (int i=0; i< v->value.arr->n_elts; ++i) {\
 fprintf (stdout,IDL_OutputFormat[v->type],val[i]);\
 w+=l;\
 if (w>=ncols-l) {fprintf (stdout,"\n");w=0;}\
 }\
}\
 fprintf (stdout,"\n");\
 }\
 break;
DLL_PUBLIC  EXPORT_HEAP_VPTR GDL_CDECL IDL_HeapVarHashFind(EXPORT_HVID hash_id);
DLL_PUBLIC void GDL_CDECL GDL_Print(int argc, EXPORT_VPTR *argv, char *argk, bool print_to_file, UCHAR* use_address=NULL) {
		//argk is to be set to NULL by users according to the doc.
		int start = 0;
		int ncols = TermWidth();
		if (print_to_file) {
			EXPORT_LONG lun = argv[0]->value.l;
			start = 1;
			if (lun < -2 || lun > maxLun) GDL_WillReturnAfterCleaning("File unit is not within allowed range: " + i2s(lun) + ".");
			if (lun == 0) GDL_WillReturnAfterCleaning("Cannot print to standard input.");
			if (lun > 0) { //need a special treatment, as our luns are not at all file descriptors...
				int proIx = LibProIx("PRINTF");
				EnvT* newEnv = new EnvT(DInterpreter::CallStackBack()->CallingNode(), libProList[proIx]);
				Guard<EnvT> guard(newEnv);
				// add parameters
				newEnv->SetNextPar(new DLongGDL(lun));
				int nParam = argc;
				for (SizeT iarg = 1; iarg < nParam; ++iarg) {
					EXPORT_VPTR v = argv[iarg];
					checkOK(v);
					const BaseGDL* par = VPTR_ToGDL(v);
					newEnv->SetNextPar(par->Dup());
				}
				static_cast<DLibPro*> (newEnv->GetPro())->Pro()(newEnv);
				return; //job done.
			}
		}
		for (int iarg = start; iarg < argc; ++iarg) {
			EXPORT_VPTR v = argv[iarg];
			checkOK(v);
			if (ISARRAY(v)) {
				switch (v->type) {
						DOCASE_ARRAY(GDL_TYP_UNDEF, UCHAR);
						DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
						DOCASE_ARRAY(GDL_TYP_INT, EXPORT_INT);
						DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
						DOCASE_ARRAY(GDL_TYP_FLOAT, float);
						DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
						DOCASE_ARRAY(GDL_TYP_COMPLEX, EXPORT_COMPLEX);
						DOCASE_ARRAY(GDL_TYP_DCOMPLEX, EXPORT_DCOMPLEX);
						DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
						DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
						DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
						DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
					case GDL_TYP_STRING:
					{
						int w = 0;
						int nEl = v->value.arr->n_elts;
						void* stringdescPtrs = MyMallocDestroyedOnExit(nEl * sizeof (EXPORT_STRING*));
						EXPORT_STRING** p = (EXPORT_STRING**) stringdescPtrs;
						for (SizeT i = 0; i < nEl; ++i) p[i] = (EXPORT_STRING*) ((SizeT) (use_address?use_address:v->value.arr->data) + (i * sizeof (EXPORT_STRING)));
						for (auto i = 0; i < nEl; ++i) if (p[i])  { //protect against fake arr->data in struct descrs.
							w += p[i]->slen;
							if (p[i]->slen > 0) {
								fprintf(stdout, "%s", p[i]->s);
							}
							if (w >= ncols) {
								fprintf(stdout, "\n");
								w = 0;
							}
						}
					}
						break;
					case GDL_TYP_STRUCT:
					{   fprintf(stdout, "{");
						int ntags=IDL_StructNumTags(v->value.s.sdef);
						UCHAR* start=v->value.s.arr->data;
						if (start == NULL) break;
						for (auto i=0; i< ntags; ++i) {
							EXPORT_VPTR var;
							EXPORT_MEMINT offset=IDL_StructTagInfoByIndex(v->value.s.sdef,i, EXPORT_MSG_INFO, &var);
							
							GDL_Print(1, &var, argk, print_to_file, (UCHAR*) (start+offset) );
						}
						fprintf(stdout, "}");
					}
					break;
				default: GDL_WillReturnAfterCleaning("Unable to convert variable to type "+std::string(IDL_TypeName[v->type])+".");
				}
			} else {
				switch (v->type) {
						DOCASE(GDL_TYP_UNDEF, c);
						DOCASE(GDL_TYP_BYTE, c);
						DOCASE(GDL_TYP_INT, i);
						DOCASE(GDL_TYP_LONG, l);
						DOCASE(GDL_TYP_FLOAT, f);
						DOCASE(GDL_TYP_DOUBLE, d);
						DOCASE(GDL_TYP_COMPLEX, cmp);
						DOCASE(GDL_TYP_DCOMPLEX, dcmp);
						DOCASE(GDL_TYP_UINT, ui);
						DOCASE(GDL_TYP_ULONG, ul);
						DOCASE(GDL_TYP_LONG64, l64);
						DOCASE(GDL_TYP_ULONG64, ul64);
						DOCASE(GDL_TYP_STRING, str.s);
						case GDL_TYP_PTR:
							fprintf(stdout, "<PtrHeapVar%d>", v->value.hvid);
							break;
					case GDL_TYP_OBJREF:
							fprintf(stdout, "<ObjHeapVar%d(%s)>", v->value.hvid, IDL_HeapVarHashFind(v->value.hvid)->var.value.s.sdef->id->name);
						break;
			default: GDL_WillReturnAfterCleaning("Unable to convert variable to type "+std::string(IDL_TypeName[v->type])+".");
				}
			}
			fprintf(stdout, "\n");
		}
	}
#undef DOCASE
#undef DOCASE_ARRAY

DLL_PUBLIC void  GDL_CDECL IDL_Print(int argc, EXPORT_VPTR *argv, char *argk) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
GDL_Print(argc, argv, argk, false);
}

DLL_PUBLIC void  GDL_CDECL IDL_PrintF(int argc, EXPORT_VPTR *argv, char *argk) {		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		GDL_Print(argc, argv, argk, true);
	}

DLL_PUBLIC void  GDL_CDECL IDL_StrStore(EXPORT_STRING *s, const char *fs){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	s->slen=strlen(fs);
	s->stype=1;
	if (s->slen > 0) {
		s->s=(char*)MyMallocDestroyedOnExit(s->slen+1);
	    strncpy(s->s,fs,s->slen+1);
	}
}
DLL_PUBLIC char* GDL_CDECL IDL_VarGetString(EXPORT_VPTR s) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) 
	checkOK(s);
	if (s->type != GDL_TYP_STRING) GDL_WillReturnAfterCleaning("String expression required in this context: "+std::string(IDL_VarName(s))+".");
	GDL_ENSURE_SIMPLE(s);
	if (s->value.str.slen==0) {
		return (char*) MyCallocDestroyedOnExit(1,1);
	} else {
		return s->value.str.s;
	}
	}
#define DOCASE_STRING(type, what)\
				case type: { \
				int slen = IDL_OutputFormatLenFunc(type);\
		        char* fmt=IDL_OutputFormatFunc(type);\
		        s->s = (char*)MyMallocDestroyedOnExit(slen+1);\
                snprintf(s->s,slen+1,fmt,v->value.what);\
				s->slen=slen;}\
				break;

DLL_PUBLIC EXPORT_STRING *GDL_CDECL IDL_VarGet1EltStringDesc(EXPORT_VPTR v, EXPORT_VPTR *tc_v, int like_print){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) 
	// do not grok what tc_v and like_print do really, the following works however:
	checkOK(v);
	if (!tc_v) {
		if (v->type != GDL_TYP_STRING) GDL_WillReturnAfterCleaning("String expression required in this context:"+std::string(IDL_VarName(v)));
	} // else need just tc_v to be not null, go figure.
	if (ISARRAY(v)) GDL_WillReturnAfterCleaning("Expression must be a scalar or 1 element array in this context:"+std::string(IDL_VarName(v)));
	EXPORT_STRING* s=(EXPORT_STRING*)MyMallocDestroyedOnExit(sizeof(EXPORT_STRING));
	if (tc_v) {
		switch (v->type) {
				DOCASE_STRING(GDL_TYP_UNDEF, c);
				DOCASE_STRING(GDL_TYP_BYTE, c);
				DOCASE_STRING(GDL_TYP_INT, i);
				DOCASE_STRING(GDL_TYP_LONG, l);
				DOCASE_STRING(GDL_TYP_FLOAT, f);
				DOCASE_STRING(GDL_TYP_DOUBLE, d);
				DOCASE_STRING(GDL_TYP_COMPLEX, cmp);
				DOCASE_STRING(GDL_TYP_DCOMPLEX, dcmp);
				DOCASE_STRING(GDL_TYP_UINT, ui);
				DOCASE_STRING(GDL_TYP_ULONG, ul);
				DOCASE_STRING(GDL_TYP_LONG64, l64);
				DOCASE_STRING(GDL_TYP_ULONG64, ul64);
				DOCASE_STRING(GDL_TYP_STRING, str);
			default: GDL_WillReturnAfterCleaning("Unable to convert variable to type "+std::string(IDL_TypeName[v->type])+".");
		}
	} else {
		s->slen = v->value.str.slen;
		if (s->slen > 0) {
			s->s = (char*) MyMallocDestroyedOnExit(s->slen + 1);
			strncpy(s->s, v->value.str.s, s->slen);
		}
	}
	return s;
}
#undef DOCASE_STRING

// str below is a supposed to be a copy of the string descriptor(s).
// to properly duplicate, one has to create copies of the string(s) and update the descriptor(s)
DLL_PUBLIC void  GDL_CDECL IDL_StrDup(GDL_REGISTER EXPORT_STRING *str, GDL_REGISTER EXPORT_MEMINT n){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	for (auto i=0; i< n; ++i) {
		if (str[i].slen > 0) {
			char* news=(char*)MyMallocDestroyedOnExit(str[i].slen+1);
			strncpy(news,str[i].s,str[i].slen+1);
			str[i].s=news;
			str[i].stype=1;
		}
	}
}

DLL_PUBLIC void  GDL_CDECL IDL_StrDelete(EXPORT_STRING *str, EXPORT_MEMINT n) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	for (auto i=0; i< n; ++i) {
		if (str[i].slen > 0 && str[i].stype==1) {
			MyFree(str[i].s); //? No as long as we are not sure this has not been allocated by us (and released by GDL)
			str[i].s=NULL;
		}
		str[i].slen=0;
		str[i].stype=0;
	}
}

// n: The number of characters the string must be able to contain, not including the terminating NULL character.
DLL_PUBLIC void  GDL_CDECL IDL_StrEnsureLength(EXPORT_STRING *s, int n) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
        if (n<0) GDL_WillReturnAfterCleaning("IDL_StrEnsureLength() passed a negative string length!");
		if (s->slen < n) {
			IDL_StrDelete(s, 1); //takes into account slen==0
			s->slen=n;
			if (n > 0) s->s = (char*) MyMallocDestroyedOnExit(n+1); //will contain garbage as described in documentation.
		}
	}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_StrToSTRING(const char *s) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		EXPORT_VPTR ret = NewTMPVPTR();
		ret->type = GDL_TYP_STRING;
		ret->flags = GDL_V_TEMP;
		size_t l=strlen(s);
		ret->value.str.slen = l;
		ret->value.str.stype = 1;
		if (l >0) {
			ret->value.str.s=(char*) MyMallocDestroyedOnExit(l+1);
		    strncpy(ret->value.str.s,s,l+1);
		}
		return ret;
	}

#define DOCASE(type, what)\
 case type: {dest->value.what=value->what; break;}
#define DOCASE_CMP(type, what)\
 case type: {dest->value.what.r=value->what.r; dest->value.what.i=value->what.i;  break;}
DLL_PUBLIC void  GDL_CDECL IDL_StoreScalar(EXPORT_VPTR dest, int type,	EXPORT_ALLTYPES * value) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		checkOK(dest);
		// dest cannot be TEMP:
        if (ISTEMP(dest)) GDL_WillReturnAfterCleaning("DLM library internal error : cannot use IDL_StoreScalar on a temporary VPTR.");
	// "normally" data and arr descr proper should be deallocated at exit from pro/fun.
	// just remove the GDL_V_ARR and GDL_V_DYNAMIC etc
        dest->flags=0; //not temporary, not array.
		dest->type=type;
		switch (type) {
				DOCASE(GDL_TYP_BYTE, c);
				DOCASE(GDL_TYP_INT, i);
				DOCASE(GDL_TYP_LONG, l);
				DOCASE(GDL_TYP_FLOAT, f);
				DOCASE(GDL_TYP_DOUBLE, d);
				DOCASE_CMP(GDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(GDL_TYP_DCOMPLEX, dcmp);
				DOCASE(GDL_TYP_UINT, ui);
				DOCASE(GDL_TYP_ULONG, ul);
				DOCASE(GDL_TYP_LONG64, l64);
				DOCASE(GDL_TYP_ULONG64, ul64);
				DOCASE(GDL_TYP_STRING, str);
			default: GDL_WillReturnAfterCleaning("Internal error: Bad type code in IDL_StoreScalar.");
		}
	}
#undef DOCASE
#undef DOCASE_CMP

#define DOCASE(type, what)\
 case type: {dest->value.what=0; break;}
#define DOCASESTR(type, what)\
 case type: {dest->value.what={0,0,0}; break;}
#define DOCASE_CMP(type, what)\
 case type: {dest->value.what.r=0; dest->value.what.i=0;  break;}
DLL_PUBLIC void  GDL_CDECL IDL_StoreScalarZero(EXPORT_VPTR dest, int type) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
//	GDL_ENSURE_SIMPLE(dest); //NO! : storescalar can overwrite a struct for example.
checkOK(dest);
		// dest cannot be TEMP:
        if (ISTEMP(dest)) GDL_WillReturnAfterCleaning("DLM library internal error : cannot use IDL_StoreScalar on a temporary VPTR.");
	// "normally" data and arr descr proper should be deallocated at exit from pro/fun.
	// just remove the GDL_V_ARR and GDL_V_DYNAMIC etc
        dest->flags=0; //not temporary, not array.
		dest->type=type;
		switch (type) {
				DOCASE(GDL_TYP_BYTE, c);
				DOCASE(GDL_TYP_INT, i);
				DOCASE(GDL_TYP_LONG, l);
				DOCASE(GDL_TYP_FLOAT, f);
				DOCASE(GDL_TYP_DOUBLE, d);
				DOCASE_CMP(GDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(GDL_TYP_DCOMPLEX, dcmp);
				DOCASE(GDL_TYP_UINT, ui);
				DOCASE(GDL_TYP_ULONG, ul);
				DOCASE(GDL_TYP_LONG64, l64);
				DOCASE(GDL_TYP_ULONG64, ul64);
				DOCASESTR(GDL_TYP_STRING, str);
			default: GDL_WillReturnAfterCleaning("Internal error: Bad type code in IDL_StoreScalar.");
		}
	}
#undef DOCASE
#undef DOCASE_CMP

DLL_PUBLIC void  GDL_CDECL IDL_VarCopy(GDL_REGISTER EXPORT_VPTR src, GDL_REGISTER EXPORT_VPTR dst) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	checkOK(src);
//	checkStorable(dst); //NO
	// if the destination variable already has a dynamic part, this dynamic part is released.
	if (dst->flags & GDL_V_DYNAMIC) {
		if (dst->value.arr != NULL) MyFree(dst->value.arr->data); // ?? No as long as we are not sure this has not been allocated by us (and released by GDL)
		// WHAT OF STRUCT ??
		dst->value.arr = NULL;
	}
	// The destination becomes a copy of the source.
	if (src->flags & GDL_V_TEMP) { //if src is temporary the dynamic part of the source is given to the destination
		memcpy(dst,src,sizeof(EXPORT_VARIABLE));
		//memset(src,0,sizeof(EXPORT_VARIABLE));//and the source variable itself is returned to the pool of free temporary variables (not in GDL yet)
//		free(src);
//		src=0;
		return;
	} 
	if (src->flags & GDL_V_STRUCT) { //must copy
		GDL_ImportArrayInExistingVPTR(dst, src->value.s.arr->n_dim, src->value.s.arr->dim, src->type , src->value.s.arr->data,src->value.s.arr->free_cb,src->value.s.sdef);
	} else if (ISARRAY(src)) { //must copy
		GDL_ImportArrayInExistingVPTR(dst, src->value.arr->n_dim, src->value.arr->dim, src->type, src->value.arr->data,src->value.arr->free_cb,NULL);
		} else {
			dst->type = src->type;
			dst->flags = src->flags;
			dst->flags2 = src->flags2;
			dst->value = src->value; //this copies pointers.
		}
}

#define DOCASE(rettype, type, what)\
 case type: {return (rettype) (v->value.what);}
#define DOCASE_CMP(rettype,type, what)\
 case type: {return (rettype) (v->value.what.r);}
DLL_PUBLIC double  GDL_CDECL IDL_DoubleScalar(GDL_REGISTER EXPORT_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		GDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(double, GDL_TYP_BYTE, c);
				DOCASE(double, GDL_TYP_INT, i);
				DOCASE(double, GDL_TYP_LONG, l);
				DOCASE(double, GDL_TYP_FLOAT, f);
				DOCASE(double, GDL_TYP_DOUBLE, d);
				DOCASE_CMP(double, GDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(double, GDL_TYP_DCOMPLEX, dcmp);
				DOCASE(double, GDL_TYP_UINT, ui);
				DOCASE(double, GDL_TYP_ULONG, ul);
				DOCASE(double, GDL_TYP_LONG64, l64);
				DOCASE(double, GDL_TYP_ULONG64, ul64);
			default: GDL_WillReturnAfterCleaning("IDL_DoubleScalar: unexpected type "+i2s(v->type));
		}
		return 0;
	}
DLL_PUBLIC EXPORT_ULONG  GDL_CDECL IDL_ULongScalar(GDL_REGISTER EXPORT_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		GDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(EXPORT_ULONG, GDL_TYP_BYTE, c);
				DOCASE(EXPORT_ULONG, GDL_TYP_INT, i);
				DOCASE(EXPORT_ULONG, GDL_TYP_LONG, l);
				DOCASE(EXPORT_ULONG, GDL_TYP_FLOAT, f);
				DOCASE(EXPORT_ULONG, GDL_TYP_DOUBLE, d);
				DOCASE_CMP(EXPORT_ULONG, GDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(EXPORT_ULONG, GDL_TYP_DCOMPLEX, dcmp);
				DOCASE(EXPORT_ULONG, GDL_TYP_UINT, ui);
				DOCASE(EXPORT_ULONG, GDL_TYP_ULONG, ul);
				DOCASE(EXPORT_ULONG, GDL_TYP_LONG64, l64);
				DOCASE(EXPORT_ULONG, GDL_TYP_ULONG64, ul64);
			default: GDL_WillReturnAfterCleaning("IDL_ULongScalar: unexpected type "+i2s(v->type));
		}
		return 0;
	}
DLL_PUBLIC EXPORT_LONG  GDL_CDECL IDL_LongScalar(GDL_REGISTER EXPORT_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		GDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(EXPORT_LONG, GDL_TYP_BYTE, c);
				DOCASE(EXPORT_LONG, GDL_TYP_INT, i);
				DOCASE(EXPORT_LONG, GDL_TYP_LONG, l);
				DOCASE(EXPORT_LONG, GDL_TYP_FLOAT, f);
				DOCASE(EXPORT_LONG, GDL_TYP_DOUBLE, d);
				DOCASE_CMP(EXPORT_LONG, GDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(EXPORT_LONG, GDL_TYP_DCOMPLEX, dcmp);
				DOCASE(EXPORT_LONG, GDL_TYP_UINT, ui);
				DOCASE(EXPORT_LONG, GDL_TYP_ULONG, ul);
				DOCASE(EXPORT_LONG, GDL_TYP_LONG64, l64);
				DOCASE(EXPORT_LONG, GDL_TYP_ULONG64, ul64);
			default: return (EXPORT_LONG) 0;
		}
		return 0;
	}
DLL_PUBLIC EXPORT_LONG64  GDL_CDECL IDL_Long64Scalar(GDL_REGISTER EXPORT_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		GDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(EXPORT_LONG64, GDL_TYP_BYTE, c);
				DOCASE(EXPORT_LONG64, GDL_TYP_INT, i);
				DOCASE(EXPORT_LONG64, GDL_TYP_LONG, l);
				DOCASE(EXPORT_LONG64, GDL_TYP_FLOAT, f);
				DOCASE(EXPORT_LONG64, GDL_TYP_DOUBLE, d);
				DOCASE_CMP(EXPORT_LONG64, GDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(EXPORT_LONG64, GDL_TYP_DCOMPLEX, dcmp);
				DOCASE(EXPORT_LONG64, GDL_TYP_UINT, ui);
				DOCASE(EXPORT_LONG64, GDL_TYP_ULONG, ul);
				DOCASE(EXPORT_LONG64, GDL_TYP_LONG64, l64);
				DOCASE(EXPORT_LONG64, GDL_TYP_ULONG64, ul64);
			default: GDL_WillReturnAfterCleaning("IDL_ULong64Scalar: unexpected type "+i2s(v->type));
		}
		return 0;
	}
DLL_PUBLIC EXPORT_ULONG64  GDL_CDECL IDL_ULong64Scalar(GDL_REGISTER EXPORT_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		GDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(EXPORT_ULONG64, GDL_TYP_BYTE, c);
				DOCASE(EXPORT_ULONG64, GDL_TYP_INT, i);
				DOCASE(EXPORT_ULONG64, GDL_TYP_LONG, l);
				DOCASE(EXPORT_ULONG64, GDL_TYP_FLOAT, f);
				DOCASE(EXPORT_ULONG64, GDL_TYP_DOUBLE, d);
				DOCASE_CMP(EXPORT_ULONG64, GDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(EXPORT_ULONG64, GDL_TYP_DCOMPLEX, dcmp);
				DOCASE(EXPORT_ULONG64, GDL_TYP_UINT, ui);
				DOCASE(EXPORT_ULONG64, GDL_TYP_ULONG, ul);
				DOCASE(EXPORT_ULONG64, GDL_TYP_LONG64, l64);
				DOCASE(EXPORT_ULONG64, GDL_TYP_ULONG64, ul64);
			default: GDL_WillReturnAfterCleaning("IDL_ULong64Scalar: unexpected type "+i2s(v->type));
		}
		return 0;
	}
DLL_PUBLIC EXPORT_MEMINT  GDL_CDECL IDL_MEMINTScalar(GDL_REGISTER EXPORT_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		GDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(EXPORT_MEMINT, GDL_TYP_BYTE, c);
				DOCASE(EXPORT_MEMINT, GDL_TYP_INT, i);
				DOCASE(EXPORT_MEMINT, GDL_TYP_LONG, l);
				DOCASE(EXPORT_MEMINT, GDL_TYP_FLOAT, f);
				DOCASE(EXPORT_MEMINT, GDL_TYP_DOUBLE, d);
				DOCASE_CMP(EXPORT_MEMINT, GDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(EXPORT_MEMINT, GDL_TYP_DCOMPLEX, dcmp);
				DOCASE(EXPORT_MEMINT, GDL_TYP_UINT, ui);
				DOCASE(EXPORT_MEMINT, GDL_TYP_ULONG, ul);
				DOCASE(EXPORT_MEMINT, GDL_TYP_LONG64, l64);
				DOCASE(EXPORT_MEMINT, GDL_TYP_ULONG64, ul64);
			default: GDL_WillReturnAfterCleaning("IDL_MEMINTScalar: unexpected type "+i2s(v->type));
		}
		return 0;
	}
DLL_PUBLIC EXPORT_FILEINT  GDL_CDECL IDL_FILEINTScalar(GDL_REGISTER EXPORT_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		GDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(EXPORT_FILEINT, GDL_TYP_BYTE, c);
				DOCASE(EXPORT_FILEINT, GDL_TYP_INT, i);
				DOCASE(EXPORT_FILEINT, GDL_TYP_LONG, l);
				DOCASE(EXPORT_FILEINT, GDL_TYP_FLOAT, f);
				DOCASE(EXPORT_FILEINT, GDL_TYP_DOUBLE, d);
				DOCASE_CMP(EXPORT_FILEINT, GDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(EXPORT_FILEINT, GDL_TYP_DCOMPLEX, dcmp);
				DOCASE(EXPORT_FILEINT, GDL_TYP_UINT, ui);
				DOCASE(EXPORT_FILEINT, GDL_TYP_ULONG, ul);
				DOCASE(EXPORT_FILEINT, GDL_TYP_LONG64, l64);
				DOCASE(EXPORT_FILEINT, GDL_TYP_ULONG64, ul64);
			default: GDL_WillReturnAfterCleaning("IDL_FILEINTScalar: unexpected type "+i2s(v->type));
		}
		return 0;
	}
#undef DOCASE
#undef DOCASE_CMP


#define DOCASE_ARRAY(type, realtype)\
 case type: {realtype *z=(realtype*) arr; for (auto i=0; i< nelts; ++i) z[i]=i ;} break;
#define DOCASE_ARRAY_CPLX(type, realtype)\
 case type: {realtype *z=(realtype*) arr; for (auto i=0; i< nelts; ++i) {z[i].r=i; z[i].i=0;}} break;
void gdlInitVector(void* arr, int type, size_t nelts) {			TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
			switch (type) {
						DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
						DOCASE_ARRAY(GDL_TYP_INT, EXPORT_INT);
						DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
						DOCASE_ARRAY(GDL_TYP_FLOAT, float);
						DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
						DOCASE_ARRAY_CPLX(GDL_TYP_COMPLEX, EXPORT_COMPLEX);
						DOCASE_ARRAY_CPLX(GDL_TYP_DCOMPLEX, EXPORT_DCOMPLEX);
						DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
						DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
						DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
						DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
					default: GDL_WillReturnAfterCleaning("gdlInitVector: unexpected type " + i2s(type));
			}
		}
#undef DOCASE_ARRAY
#undef DOCASE_ARRAY_CPLX

DLL_PUBLIC char * GDL_CDECL IDL_MakeTempVector(int type, EXPORT_MEMINT dim, int  init, EXPORT_VPTR *var){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
EXPORT_VPTR v=NewTMPVPTRARRAY();
*var=v;
v->type=type;
v->value.arr->dim[0]=dim;
v->value.arr->n_elts=dim;
v->value.arr->n_dim=1;
EXPORT_LONG64 sz=IDL_TypeSizeFunc(type);
v->value.arr->elt_len = sz;
SizeT l=dim*sz;
void * addr=gdlAlignedMalloc(l); //aka GDL_ARR_INI_NOP
v->value.arr->arr_len=l;
v->value.arr->data = (UCHAR*) addr;
if (init == GDL_ARR_INI_ZERO) memset((void*)addr, 0, l);
else if (init == GDL_ARR_INI_INDEX) {
	if (type == GDL_TYP_STRING) {
		EXPORT_LONG64 nelts=v->value.arr->n_elts;
		static int slen = IDL_OutputFormatLenFunc(GDL_TYP_LONG);
		// allocate size:
		EXPORT_STRING *thestrings = (EXPORT_STRING *) v->value.arr->data;
		char* fmt=IDL_OutputFormatFunc(GDL_TYP_LONG);
		for (auto i = 0; i < nelts; ++i) {
			thestrings[i].slen = slen;
			thestrings[i].s = (char*)MyMallocDestroyedOnExit(slen+1); //dumb to allocate small chunks but necessary as we cannot know what will free each element
			snprintf(thestrings[i].s,slen+1, fmt, i);
		}
	} else gdlInitVector(addr, type, l);
}
return (char*) addr;
}

DLL_PUBLIC char * GDL_CDECL IDL_MakeTempStructVector(EXPORT_StructDefPtr sdef, EXPORT_MEMINT dim, EXPORT_VPTR *var, int zero) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
EXPORT_VPTR v = NewTMPVPTRSTRUCT(sdef);
if (var) *var = v;
v->value.s.arr=NewExportArray();
v->type=GDL_TYP_STRUCT; //already done, just for clarity.
v->value.arr->dim[0]=dim;
v->value.arr->n_elts=dim;
v->value.arr->n_dim=1;
EXPORT_LONG64 sz=v->value.s.sdef->length;
v->value.arr->elt_len = sz;
SizeT l=dim*sz;
void * addr=gdlAlignedMalloc(l);
if (zero) memset(addr, 0, l);
v->value.arr->arr_len=l;
v->value.arr->data = (UCHAR*) addr;
return (char*) addr;
}

DLL_PUBLIC char * GDL_CDECL IDL_MakeTempArray(int type, int n_dim, EXPORT_MEMINT  dim[], int init, EXPORT_VPTR *var){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
EXPORT_VPTR v=NewTMPVPTRARRAY();
if (var) *var=v;
v->type=type;
SizeT l=1;
v->value.arr->dim[0] = 1;
if (dim) for (auto i=0; i< n_dim; ++i) {
	l*=dim[i];
	v->value.arr->dim[i] = dim[i];
}
v->value.arr->n_elts=l;
v->value.arr->n_dim = n_dim;
EXPORT_LONG64 sz=IDL_TypeSizeFunc(type);
v->value.arr->elt_len = sz;
l=sz; if (dim) for (auto i=0; i<n_dim; ++i) l*=dim[i];
void * addr=gdlAlignedMalloc(l); //aka GDL_ARR_INI_NOP
v->value.arr->arr_len=l;
v->value.arr->data = (UCHAR*) addr;
if (init == GDL_ARR_INI_ZERO)  memset((void*)addr, 0, l);
else if (init == GDL_ARR_INI_INDEX) {
	if (type == GDL_TYP_STRING) {
		EXPORT_LONG64 nelts=v->value.arr->n_elts;
		static int slen = IDL_OutputFormatLenFunc(GDL_TYP_LONG);
		// allocate size:
		EXPORT_STRING *thestrings = (EXPORT_STRING *) v->value.arr->data;
		char* fmt=IDL_OutputFormatFunc(GDL_TYP_LONG);
		for (auto i = 0; i < nelts; ++i) {
			thestrings[i].slen = slen;
			thestrings[i].s = (char*)MyMallocDestroyedOnExit(slen+1); //dumb to allocate small chunks but necessary as we cannot know what will free each element
			snprintf(thestrings[i].s, slen+1, fmt, i);
		}
	} else gdlInitVector(addr, type, l);
}
return (char*) addr;	
}

DLL_PUBLIC char * GDL_CDECL IDL_MakeTempStruct(EXPORT_StructDefPtr sdef, int  n_dim, EXPORT_MEMINT *dim, EXPORT_VPTR *var, int zero){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	if (sdef == NULL) GDL_WillReturnAfterCleaning("EXPORT_VarMakeTempFromTemplate() defines a struct without passing a valid EXPORT_StructDefPtr");
	EXPORT_VPTR v = NewTMPVPTRSTRUCT(sdef);
if (var) *var=v;
SizeT l=1;
v->value.arr->dim[0] = 1;
if (dim) for (auto i=0; i< n_dim; ++i) {
	l*=dim[i];
	v->value.arr->dim[i] = dim[i];
}
v->value.arr->n_elts=l;
v->value.arr->n_dim = n_dim;
EXPORT_LONG64 sz=v->value.s.sdef->length;
v->value.arr->elt_len = sz;
l=sz; if (dim) for (auto i=0; i<n_dim; ++i) l*=dim[i];
void * addr=gdlAlignedMalloc(l); //aka GDL_ARR_INI_NOP
v->value.arr->arr_len=l;
v->value.arr->data = (UCHAR*) addr;
if (zero)  memset((void*)addr, 0, l);
return (char*) addr;
}


DLL_PUBLIC char * GDL_CDECL IDL_VarMakeTempFromTemplate(EXPORT_VPTR template_var, int type, EXPORT_StructDefPtr sdef,  EXPORT_VPTR *result_addr, int zero){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
EXPORT_VPTR t = template_var;
checkOK(t);
EXPORT_VPTR v;
if (t->flags & GDL_V_STRUCT) {
	if (sdef == NULL) GDL_WillReturnAfterCleaning("IDL_VarMakeTempFromTemplate() defines a struct without passing a valid IDL_StructDefPtr");
	v = NewTMPVPTRSTRUCT(sdef);
} else if (ISARRAY(t)) {
	v = NewTMPVPTRARRAY();
} else {
	v = NewTMPVPTR(t->flags);
}
*result_addr = v;
if (ISARRAY(t)) {
	v->type = type;
	v->flags = t->flags;
	v->value.arr->n_dim = t->value.arr->n_dim;
	v->value.arr->n_elts=t->value.arr->n_elts;
	for (auto i=0; i< t->value.arr->n_dim; ++i) v->value.arr->dim[i] = t->value.arr->dim[i];
	EXPORT_LONG64 sz=IDL_TypeSizeFunc(type);
	v->value.arr->elt_len = sz;
	SizeT l=sz; for (auto i=0; i<t->value.arr->n_dim; ++i) l*=t->value.arr->dim[i];
	v->value.arr->arr_len = l;
	void * addr=gdlAlignedMalloc(l);
	v->value.arr->data = (UCHAR*) addr;
//	if (zero) 
		memset((void*)addr, 0, l);
	return (char*) addr;	
} else {
	v->type = type;
//	if (zero) memset(&(v->value),0,sizeof(EXPORT_ALLTYPES)); //already zeroed
	return (char*) 	&(v->value);
}
return NULL;
}	

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_Gettmp(void){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	return NewTMPVPTR();
	}

#define DOIT(aaa, entry)\
	EXPORT_VPTR ret=NewTMPVPTR();\
	ret->type=aaa;\
	ret->value.entry=value;\
	return ret;
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpByte(UCHAR value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
     DOIT(GDL_TYP_BYTE, c);
}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpInt(EXPORT_INT value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_INT, i);
	}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpUInt(EXPORT_UINT value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_UINT, ui);
	}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpLong(EXPORT_LONG value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_LONG, l);
	}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpFloat(float value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_FLOAT, f);
	}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpDouble(double value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_DOUBLE, d);
	}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpPtr(EXPORT_HVID value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_PTR, hvid);
	}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpObjRef(EXPORT_HVID value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_OBJREF, hvid);
	}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpULong(EXPORT_ULONG value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_ULONG, ul);
	}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpLong64(EXPORT_LONG64 value){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_LONG64, l64);
}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpULong64(EXPORT_ULONG64 value){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_ULONG64, ul64);
}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpFILEINT(EXPORT_FILEINT value) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_FILEINT, fileint);
	}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GettmpMEMINT(EXPORT_MEMINT value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(GDL_TYP_MEMINT, memint);
	}
#undef DOIT

#define OFFSET_HELPER(ty, c_ty)\
 case ty: {\
 const c_ty *val=(c_ty *) v->value.arr->data;\
 for (int k=0; k< addim; ++k) destArray->dim[destArray->n_dim++]=val[k];\
 destArray->elt_len= sizeof (c_ty);\
 break;\
 }
#define OFFSET_HELPER_CPX(ty, c_ty)\
 case ty: {\
 const c_ty *val=(c_ty *) v->value.arr->data;\
 for (int k=0; k< addim; ++k) destArray->dim[destArray->n_dim++]=val[k].r;\
 destArray->elt_len= sizeof (c_ty);\
 break;\
 }

	void extract_offset_and_dims(int argc, EXPORT_VPTR* argv, EXPORT_VPTR ret, EXPORT_MEMINT *off, EXPORT_ARRAY *destArray) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		if (argc == 1) return;
		//more than 1 argument:
		if (ISARRAY(argv[1])) GDL_WillReturnAfterCleaning("Expression must be a scalar or 1 element array in this context: " + std::string(IDL_VarName(argv[1])) + ".");
		if (argv[0]->type == GDL_TYP_STRING) GDL_WillReturnAfterCleaning("String expression not allowed in this context: " + std::string(IDL_VarName(argv[0])) + ".");
		SizeT ncharToTransferMax = (ISARRAY(argv[0])) ? argv[0]->value.arr->arr_len : IDL_TypeSizeFunc(argv[0]->type);
		*off = argv[1]->value.memint;
		ncharToTransferMax -= *off;
		// void dest
		memset((void*) destArray, 0, sizeof (EXPORT_ARRAY));
		bool noarray = false;
		bool noscalar = false;
		for (int i = 2; i < argc; ++i) {
			EXPORT_VPTR v = argv[i];
			checkOK(v);
			if (ISARRAY(v)) {
				noscalar = true;
				if (noarray) GDL_WillReturnAfterCleaning("Expression must be a scalar or 1 element array in this context: " + std::string(IDL_VarName(argv[i])) + ".");
				//if there is another argument;, this one SHOULD have been a scalar
				int addim = v->value.arr->n_elts; //this is an array whose elements are the successive dimensions, such as [2,3,1,4]; dimension 1 is kept.
				if (destArray->n_dim + addim > GDL_MAX_ARRAY_DIM) GDL_WillReturnAfterCleaning("Arrays are allowed 1 - 8 dimensions.");
				switch (v->type) {
						OFFSET_HELPER(GDL_TYP_BYTE, UCHAR);
						OFFSET_HELPER(GDL_TYP_INT, EXPORT_INT);
						OFFSET_HELPER(GDL_TYP_LONG, EXPORT_LONG);
						OFFSET_HELPER(GDL_TYP_FLOAT, float);
						OFFSET_HELPER(GDL_TYP_DOUBLE, double);
						OFFSET_HELPER_CPX(GDL_TYP_COMPLEX, EXPORT_COMPLEX);
						OFFSET_HELPER_CPX(GDL_TYP_DCOMPLEX, EXPORT_DCOMPLEX);
						OFFSET_HELPER(GDL_TYP_UINT, EXPORT_UINT);
						OFFSET_HELPER(GDL_TYP_ULONG, EXPORT_ULONG);
						OFFSET_HELPER(GDL_TYP_LONG64, EXPORT_LONG64);
						OFFSET_HELPER(GDL_TYP_ULONG64, EXPORT_ULONG64);
					case GDL_TYP_STRING:
					{
						EXPORT_STRING *s = (EXPORT_STRING *) v->value.arr->data;
						for (int k = 0; k < addim; ++k) {
							char buf[s[k].slen + 1];
							strncpy(buf, s[k].s, s[k].slen);
							buf[s[k].slen] = 0;
							long long z = strtoll(buf, NULL, 10);
							if (z == LLONG_MIN || z == LLONG_MAX) Warning("Type conversion error: Unable to convert given STRING to Long64.");
							destArray->dim[destArray->n_dim++] = z;
						}
						break;
					}
					default: GDL_WillReturnAfterCleaning("Unsupported type for argument.");
				}
			} else {
				noarray = true;
				if (noscalar) GDL_WillReturnAfterCleaning("Expression must be a scalar or 1 element array in this context: " + std::string(IDL_VarName(argv[i - 1])) + ".");
				if (destArray->n_dim + 1 > GDL_MAX_ARRAY_DIM) GDL_WillReturnAfterCleaning("Arrays are allowed 1 - 8 dimensions.");
				destArray->dim[destArray->n_dim++] = v->value.memint; //? 
			}
			for (int k = 0; k < destArray->n_dim; ++k) if (destArray->dim[k] <= 0) GDL_WillReturnAfterCleaning("Array dimensions must be greater than 0.");
		}
		// check total length
		SizeT l = IDL_TypeSizeFunc(ret->type);
		if (destArray->n_dim > 0) for (auto i = 0; i < destArray->n_dim; ++i) l *= destArray->dim[i];
		if (l > ncharToTransferMax) GDL_WillReturnAfterCleaning("Specified offset to array is out of range: (max " + i2s(ncharToTransferMax) + ")" + std::string(IDL_VarName(argv[0])) + ".");
		destArray->n_elts = 1;
		for (auto i = 0; i < destArray->n_dim; ++i) destArray->n_elts *= destArray->dim[i];
	}

#define TREAT_MULTIPLE_ARGS() \
EXPORT_MEMINT myOff=0;\
EXPORT_ARRAY myExportArray;\
int ndim;\
EXPORT_ARRAY_DIM dim;\
extract_offset_and_dims(argc, argv, ret, &myOff, &myExportArray);

	//printf(" offset=%d , ndim=%d, dims=[%d",myOff,myExportArray.n_dim, myExportArray.dim[0]); for (int i=1; i< myExportArray.n_dim; ++i) printf(", %d",myExportArray.dim[i]); printf("]\n");
	
#define DEFOUT(idl_dst_type)\
	if (argc ==1 && argv[0]->type == idl_dst_type) return argv[0];\
	EXPORT_VPTR ret = NewTMPVPTR(argv[0]->flags);\
	ret->type = idl_dst_type;\
	EXPORT_ALLTYPES value = argv[0]->value;
	
#define PREPARE_ARRAY(dst_type)\
	EXPORT_ARRAY *dstArrayDescr=NewExportArray();\
    ret->flags |= GDL_V_ARR;\
	ret->value.arr=dstArrayDescr;\
    EXPORT_ARRAY* srcArrayDescr=argv[0]->value.arr;\
	if (argc != 1) memcpy(dstArrayDescr, &myExportArray, sizeof (EXPORT_ARRAY)); else memcpy(dstArrayDescr, srcArrayDescr, sizeof (EXPORT_ARRAY));\
	dstArrayDescr->elt_len = sizeof (dst_type);\
	dstArrayDescr->arr_len = dstArrayDescr->elt_len*dstArrayDescr->n_elts;\
	dst_type *dstval = (dst_type*) gdlAlignedMalloc(dstArrayDescr->arr_len);\
	ret->value.arr->data = (UCHAR*) dstval;

#define DOCASE_ARRAY(idl_src_type, src_type)\
 			case idl_src_type: {\
				if (argc==1) {\
				const src_type *srcval=(src_type *)(srcArrayDescr->data+myOff);\
				for (auto i=0; i< dstArrayDescr->n_elts; ++i) dstval[i]=srcval[i];\
				} else {\
					memcpy(ret->value.arr->data,srcArrayDescr->data+myOff,ret->value.arr->arr_len);\
					}\
				break; }


#define DOCASE_ARRAY_FROM_STRING(idl_src_type, desired_type, src_type)\
 			case idl_src_type: {\
				const src_type *s=(src_type *)(srcArrayDescr->data+myOff);\
				for (auto i=0; i< dstArrayDescr->n_elts; ++i) {\
							char buf[s[i].slen + 1];\
							strncpy(buf, s[i].s, s[i].slen);\
							buf[s[i].slen] = 0;\
							sscanf(buf,EXPORT_InputFormat[desired_type],&(dstval[i]));\
						}\
				break; }


#define DOCASE_TO_CMP_CONVERT_2ARGS(idl_src_type, src_type, my_type, __extension)\
	case idl_src_type: {\
	EXPORT_VPTR z;\
	z=IDL_CvtDbl(1,&(argv[1])); /*will be the inaginary part*/\
	EXPORT_ARRAY *dstArrayDescr = NewExportArray();\
	ret->flags |= GDL_V_ARR;\
	ret->value.arr = dstArrayDescr;\
	if (ISSCALAR(z)) {\
		if (ISARRAY(argv[0]) ) memcpy(dstArrayDescr, argv[0]->value.arr, sizeof (EXPORT_ARRAY));\
		else memcpy(dstArrayDescr, argv[1]->value.arr, sizeof (EXPORT_ARRAY));\
		dstArrayDescr->elt_len = sizeof (my_type);\
		dstArrayDescr->arr_len = dstArrayDescr->elt_len * dstArrayDescr->n_elts;\
		my_type *dstval = (my_type*) gdlAlignedMalloc(dstArrayDescr->arr_len);\
		ret->value.arr->data = (UCHAR*) dstval;\
		const src_type *srcval; int k=0;\
		if (ISARRAY(argv[0]) ) {k=1; srcval= (src_type *) (argv[0]->value.arr->data);}\
		else srcval= (src_type *) &(argv[0]->value.memint);\
		for (auto i = 0, l = 0; i < dstArrayDescr->n_elts; ++i) {\
			dstval[i].r = srcval[l]__extension; l+=k;\
			dstval[i].i = z->value.d;\
		}\
		} else {\
		if (ISARRAY(argv[0]) && argv[0]->value.arr->n_elts <  argv[1]->value.arr->n_elts) memcpy(dstArrayDescr, argv[0]->value.arr, sizeof (EXPORT_ARRAY));\
		else memcpy(dstArrayDescr, argv[1]->value.arr, sizeof (EXPORT_ARRAY));\
		dstArrayDescr->elt_len = sizeof (my_type);\
		dstArrayDescr->arr_len = dstArrayDescr->elt_len * dstArrayDescr->n_elts;\
		my_type *dstval = (my_type*) gdlAlignedMalloc(dstArrayDescr->arr_len);\
		ret->value.arr->data = (UCHAR*) dstval;\
		const src_type *srcvalr; int k=0;\
		if (ISARRAY(argv[0]) ) {k=1; srcvalr= (src_type *) (argv[0]->value.arr->data);}\
		else srcvalr = (src_type *) &(argv[0]->value.memint);\
		const double *srcvali = (double *) (z->value.arr->data);\
		for (auto i = 0, l = 0; i < dstArrayDescr->n_elts; ++i) {\
			dstval[i].r = srcvalr[l]__extension; l+=k;\
			dstval[i].i = srcvali[i];\
		}\
	}\
	}\
	break;
	
#define DOCASE_ARRAY_NO_CONVERT(idl_src_type, src_type, my_type)\
 			case idl_src_type: {\
				const my_type *srcval=(my_type *)(srcArrayDescr->data+myOff);\
				memcpy(dstval,srcval,dstArrayDescr->n_elts*sizeof(my_type)); \
				break; }
#define DOCASE_ARRAY_FROM_CMP_NO_CONVERT(idl_src_type, src_type, my_type)\
 			case idl_src_type: {\
				const src_type *srcval=(src_type *)(srcArrayDescr->data+myOff);\
				for (auto i=0; i< dstArrayDescr->n_elts; ++i) {dstval[i]=srcval[i].r;}\
				break; }
#define DOCASE_TO_CMP_ARRAY_NO_CONVERT(idl_src_type, src_type, my_type)\
 			case idl_src_type: {\
				const my_type *srcval=(my_type *)(srcArrayDescr->data+myOff);\
				memcpy(dstval,srcval,dstArrayDescr->n_elts*sizeof(my_type)); \
				break; }
#define DOCASE_TO_CMP_ARRAY_FROM_CMP(idl_src_type, src_type, my_type)\
 			case idl_src_type: {\
				const src_type *srcval=(src_type *)(srcArrayDescr->data+myOff);\
				for (auto i=0; i< dstArrayDescr->n_elts; ++i) {dstval[i].r=srcval[i].r;dstval[i].i=srcval[i].i;}\
				break; }
#define DOCASE(type, field1, field2)\
 case type: ret->value.field1=value.field2 ; break;

#define DOCASE_FROM_STRING(type, field1, field2)\
				case type:  {int nread=sscanf(argv[0]->value.str.s,EXPORT_InputFormat[field1],&(ret->value.field2));\
				if (nread < 0) GDL_WillReturnAfterCleaning("Error converting input to "+std::string(IDL_TypeName[field1]));} break;

#define DOCASE_FROM_CMP(type, field1, field2)\
 case type: ret->value.field1=value.field2.r ; break;
#define DOCASE_ARRAY_FROM_CMP_NOT_TESTED(idl_src_type, src_type)\
 			case idl_src_type: {\
				const src_type *srcval=(src_type *)(srcArrayDescr->data+myOff);\
				for (auto i=0; i< dstArrayDescr->n_elts; ++i) dstval[i]=srcval[2*i];\
				break; }
#define DOCASE_TO_CMP_SINGLE(type, field1, field2)\
 case type: ret->value.field1.r=value.field2 ; break;
#define DOCASE_TO_CMP_FROM_CMP_SINGLE(type, field1, field2)\
 case type: ret->value.field1.r=value.field2.r ; ret->value.field1.i=value.field2.i ;break;
#define DOCASE_TO_CMP_ARRAY(idl_src_type, src_type, my_type)\
 			case idl_src_type: {\
				const src_type *srcval=(src_type *)(srcArrayDescr->data);\
				for (auto i=0; i< srcArrayDescr->n_elts; ++i) dstval[i].r=srcval[i];\
				break; }
#define DOCASE_TO_CMP_FROM_CMP_VARIANT(type, field1, field2)\
 case type:  { EXPORT_VPTR z;\
	z=IDL_CvtDbl(1,&(argv[1])); /*will be the inaginary part*/\
    ret->value.field1.r=value.field2 ; ret->value.field1.i=z->value.d ;\
	} break;

#define TARGET_GDL_TYPE GDL_TYP_BYTE
#define TARGET_EXPORT_TYPE EXPORT_BYTE
	DLL_PUBLIC EXPORT_VPTR GDL_CDECL GDL_Other_CvtByte(int argc, EXPORT_VPTR argv[]) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
#include "snippets/export_cvt_function.incc"
	}
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtByte(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
    if (argc !=1 || (argv[0]->type != GDL_TYP_STRING) ) return GDL_Other_CvtByte(argc, argv); // sole case not same asIDL_BasicTypeConversion()
	// only left: argc=1 and type is gdl_typ_string
	EXPORT_VPTR ret = NewTMPVPTR(argv[0]->flags);
	ret->type = GDL_TYP_BYTE;
	EXPORT_ARRAY *dstArrayDescr = NewExportArray();
	ret->flags |= GDL_V_ARR;
	ret->value.arr = dstArrayDescr;
	// if argv[0] is a simple string it has no arr
    if (ISSCALAR(argv[0])) {
		dstArrayDescr->elt_len = 1;
		dstArrayDescr->data_guard = 0;
		dstArrayDescr->flags=0;
		dstArrayDescr->n_dim = 1;
		int sz=argv[0]->value.str.slen;
		dstArrayDescr->dim[0] =  sz;
		dstArrayDescr->arr_len = sz;
		dstArrayDescr->n_elts=sz;
		UCHAR *dstval = (UCHAR*) gdlAlignedMalloc(sz);
		ret->value.arr->data = (UCHAR*) dstval;
		memcpy(dstval, argv[0]->value.str.s,sz);
		return ret;
	}
	// else: argv[0] is a string array
	EXPORT_ARRAY* srcArrayDescr = argv[0]->value.arr;
	//copy common values
	memcpy(dstArrayDescr, srcArrayDescr, sizeof (EXPORT_ARRAY));
	const EXPORT_STRING *s = (EXPORT_STRING *) (srcArrayDescr->data);
	// find max size for all strings in array
	int sz = 0;	for (auto i = 0; i < srcArrayDescr->n_elts; ++i) sz = (sz < s[i].slen) ? s[i].slen : sz;
	//update related descrs
	dstArrayDescr->elt_len = 1;
	dstArrayDescr->data_guard = 0; //?????????????? TBC
	dstArrayDescr->n_elts = sz*srcArrayDescr->n_elts;
	dstArrayDescr->flags= 0;
	dstArrayDescr->n_dim = srcArrayDescr->n_dim + 1;
	if (dstArrayDescr->n_dim > GDL_MAX_ARRAY_DIM) GDL_WillReturnAfterCleaning("Arrays are allowed 1 - 8 dimensions.");
	for (auto i = dstArrayDescr->n_dim - 1; i == 1; --i) dstArrayDescr->dim[i] = dstArrayDescr->dim[i - 1];
	dstArrayDescr->dim[0] = sz;
	dstArrayDescr->arr_len = srcArrayDescr->n_elts * sz;
	UCHAR *dstval = (UCHAR*) gdlAlignedMalloc(dstArrayDescr->arr_len);
	memset(dstval, 0, dstArrayDescr->arr_len);
	ret->value.arr->data = (UCHAR*) dstval;
	for (auto i = 0; i < srcArrayDescr->n_elts; ++i) { //for each source string
		for (auto j = 0; j < s[i].slen; ++j) dstval[i * sz + j] = s[i].s[j]; //copy all available string bytes at the correct location
	}
	return ret;
	}

#define DOCASE_CVTBYTSCL_ARRAY(idl_src_type, src_type)\
 			case idl_src_type: {\
				const src_type *srcval=(src_type *)(srcArrayDescr->data+myOff);\
				src_type min=srcval[0];\
				src_type max=min;\
				for (auto i=1; i< dstArrayDescr->n_elts; ++i) { min=std::min(min,srcval[i]); max=std::max(max,srcval[i]); }\
				src_type top=255; src_type range=max-min;\
				for (auto i=0; i< dstArrayDescr->n_elts; ++i) dstval[i]=(top+0.999)*(srcval[i]-min)/range;\
				break; }
#define DOCASE_CVTBYTSCL_ARRAY_FROM_CMP(idl_src_type, src_type)\
 			case idl_src_type: {\
				const src_type *srcval=(src_type *)(srcArrayDescr->data+myOff);\
				src_type min=srcval[0];\
				src_type max=min;\
				for (auto i=1; i< dstArrayDescr->n_elts; ++i) { min=std::min(min,srcval[2*i]); max=std::max(max,srcval[2*i]); }\
				src_type top=255; src_type range=max-min;\
				for (auto i=0; i< dstArrayDescr->n_elts; ++i) dstval[i]=(top+0.999)*(srcval[2*i]-min)/range;\
				break; }

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtBytscl(int argc, EXPORT_VPTR argv[], char *argk) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		if (argv[0]->type == GDL_TYP_STRING) GDL_WillReturnAfterCleaning("String expression not allowed in this context: " + std::string(IDL_VarName(argv[0])) + ".");
		 DEFOUT(GDL_TYP_BYTE);
		TREAT_MULTIPLE_ARGS();
		if (ISARRAY(argv[0])) {
			PREPARE_ARRAY(UCHAR);
			switch (argv[0]->type) {
					DOCASE_CVTBYTSCL_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_CVTBYTSCL_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_CVTBYTSCL_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_CVTBYTSCL_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_CVTBYTSCL_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_CVTBYTSCL_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, float);
					DOCASE_CVTBYTSCL_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, double);
					DOCASE_CVTBYTSCL_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_CVTBYTSCL_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_CVTBYTSCL_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_CVTBYTSCL_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
					DOCASE_ARRAY_FROM_STRING(GDL_TYP_STRING, GDL_TYP_BYTE, EXPORT_STRING);
				default: GDL_WillThrowAfterCleaning(__func__, "Internal error: Unknown combination in convert()..");
			}
		} else {
			//do nothing, CvtBytscl on 1 element gives 0
		}
		return ret;
}

#undef DOCASE_CVTBYTSCL_ARRAY_FROM_CMP
#undef DOCASE_CVTBYTSCL_ARRAY

#define TARGET_GDL_TYPE GDL_TYP_INT
#define TARGET_EXPORT_TYPE EXPORT_INT

	DLL_PUBLIC EXPORT_VPTR GDL_CDECL IDL_CvtFix(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
#include "snippets/export_cvt_function.incc"
	}
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE

#define TARGET_GDL_TYPE GDL_TYP_UINT
#define TARGET_EXPORT_TYPE EXPORT_UINT

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtUInt(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/export_cvt_function.incc"
	}
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE

#define TARGET_GDL_TYPE GDL_TYP_LONG
#define TARGET_EXPORT_TYPE EXPORT_LONG

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtLng(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/export_cvt_function.incc"
	}
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE

#define TARGET_GDL_TYPE GDL_TYP_ULONG
#define TARGET_EXPORT_TYPE EXPORT_ULONG

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtULng(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/export_cvt_function.incc"
	}
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE

#define TARGET_GDL_TYPE GDL_TYP_LONG64
#define TARGET_EXPORT_TYPE EXPORT_LONG64

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtLng64(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/export_cvt_function.incc"
	}
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE

#define TARGET_GDL_TYPE GDL_TYP_ULONG64
#define TARGET_EXPORT_TYPE EXPORT_ULONG64

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtULng64(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/export_cvt_function.incc"
	}
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtMEMINT(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) 
	EXPORT_VPTR v;
	static const bool arch64= (sizeof(EXPORT_VPTR) == 64);
	if (arch64) v=IDL_CvtULng64(argc, argv); else v=IDL_CvtULng(argc, argv);
	v->type=GDL_TYP_MEMINT;
	return v;
}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtFILEINT(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) 
	EXPORT_VPTR v;
	static const bool arch64= (sizeof(EXPORT_VPTR) == 64);
	if (arch64) v=IDL_CvtULng64(argc, argv); else v=IDL_CvtULng(argc, argv);
	v->type=GDL_TYP_FILEINT;
	return v;
}

#define TARGET_GDL_TYPE GDL_TYP_FLOAT
#define TARGET_EXPORT_TYPE float

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtFlt(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/export_cvt_function.incc"
	}
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE

#define TARGET_GDL_TYPE GDL_TYP_DOUBLE
#define TARGET_EXPORT_TYPE double

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtDbl(int argc, EXPORT_VPTR argv[]){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/export_cvt_function.incc"
	}
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE


//IDL_CvtComplex does not behave as others above
// If argc==1 argv[1] is a vector, it is the vector of the imaginary part.
#define TARGET_GDL_TYPE GDL_TYP_DCOMPLEX
#define TARGET_EXPORT_TYPE EXPORT_DCOMPLEX
	DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtDComplex(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		bool normal_mode=true;
#include "snippets/export_cvt_function_complex.incc"
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE
	}
//IDL_CvtComplex does not behave as others above
// If argc==1 argv[1] is a vector, it is the vector of the imaginary part.
#define TARGET_GDL_TYPE GDL_TYP_DCOMPLEX
#define TARGET_EXPORT_TYPE EXPORT_DCOMPLEX
	EXPORT_VPTR  GDL_CDECL GDL_CvtDComplex2(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		bool normal_mode=false;
#include "snippets/export_cvt_function_complex.incc"
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE
	}
	
//IDL_CvtComplex does not behave as others above
// If argc==1 argv[1] is a vector, it is the vector of the imaginary part.
#define TARGET_GDL_TYPE GDL_TYP_COMPLEX
#define TARGET_EXPORT_TYPE EXPORT_COMPLEX
	DLL_PUBLIC EXPORT_VPTR GDL_CDECL IDL_CvtComplex(int argc, EXPORT_VPTR argv[], char *argk) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		if ((size_t) argk == 2) return GDL_CvtDComplex2(argc, argv); //DComplex called from  BasicTypeConversion
		bool normal_mode=true;
		if (argk !=0) normal_mode=false;
#include "snippets/export_cvt_function_complex.incc"
#undef TARGET_GDL_TYPE
#undef TARGET_EXPORT_TYPE
	}

#undef DOCASE
#undef DOCASE_ARRAY
#undef DOCASE_FROM_CMP
#undef DOCASE_TO_CMP_SINGLE
#undef DOCASE_TO_CMP_ARRAY_NO_CONVERT
#undef DOCASE_TO_CMP_ARRAY_FROM_CMP
#undef DOCASE_TO_CMP_FROM_CMP_SINGLE
#undef DOCASE_FROM_STRING

#define DOCASE_STRING(type, field2)\
				case type: { \
		        EXPORT_LONG64 nelts=1;\
				int slen = IDL_OutputFormatLenFunc(type);\
		        char* fmt=IDL_OutputFormatFunc(type);\
		        char* allstrings = (char*)MyMallocDestroyedOnExit(nelts * (slen+1));\
				ret->value.str.s=allstrings;\
				ret->value.str.slen=slen;\
				snprintf(ret->value.str.s,slen+1, fmt, value.field2);}\
				break;

#define DOCASE_STRING_ARRAY(idl_src_type, src_type)\
				case idl_src_type: { \
                const src_type *srcval=(src_type *)(srcArrayDescr->data);\
		        EXPORT_LONG64 nelts=dstArrayDescr->n_elts;\
				int slen = IDL_OutputFormatLenFunc(idl_src_type);\
		        char* fmt=IDL_OutputFormatFunc(idl_src_type);\
				void* stringdescPtrs = MyMallocDestroyedOnExit(nelts * sizeof (EXPORT_STRING*));\
				EXPORT_STRING** p = (EXPORT_STRING**) stringdescPtrs;\
				for (SizeT i = 0; i < nelts; ++i) p[i] = (EXPORT_STRING*) ((SizeT) dstArrayDescr->data + (i * sizeof (EXPORT_STRING)));\
		        for (auto i = 0; i < nelts; ++i) {\
					p[i]->s = (char*) MyMallocDestroyedOnExit(slen + 1);\
					p[i]->slen = slen;\
					snprintf(p[i]->s,slen+1, fmt, srcval[i]);}\
				}\
				break;

   EXPORT_VPTR GDL_CDECL GDL_Other_CvtString(int argc, EXPORT_VPTR argv[], char *argk = NULL) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		DEFOUT(GDL_TYP_STRING);
		TREAT_MULTIPLE_ARGS();
		if (ISARRAY(argv[0])) {
			EXPORT_ARRAY *dstArrayDescr = NewExportArray();
			ret->flags |= GDL_V_ARR;
			ret->value.arr = dstArrayDescr;
			EXPORT_ARRAY* srcArrayDescr = argv[0]->value.arr;
			memcpy(dstArrayDescr, srcArrayDescr, sizeof (EXPORT_ARRAY));
			dstArrayDescr->elt_len = sizeof (EXPORT_STRING);
			dstArrayDescr->arr_len = dstArrayDescr->elt_len * dstArrayDescr->n_elts;
			EXPORT_STRING *stringdescPtrs = (EXPORT_STRING*) gdlAlignedMalloc(dstArrayDescr->arr_len);
			dstArrayDescr->data = (UCHAR*) stringdescPtrs;
			switch (argv[0]->type) {
					DOCASE_STRING_ARRAY(GDL_TYP_UNDEF, UCHAR);
					DOCASE_STRING_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_STRING_ARRAY(GDL_TYP_INT, EXPORT_INT);
					DOCASE_STRING_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_STRING_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_STRING_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_STRING_ARRAY(GDL_TYP_COMPLEX, EXPORT_COMPLEX);
					DOCASE_STRING_ARRAY(GDL_TYP_DCOMPLEX, EXPORT_DCOMPLEX);
					DOCASE_STRING_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_STRING_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_STRING_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_STRING_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning(__func__, "Internal error: Unknown combination in convert()..");
			}
		} else {
			switch (argv[0]->type) {
					DOCASE_STRING(GDL_TYP_UNDEF, c);
					DOCASE_STRING(GDL_TYP_BYTE, c);
					DOCASE_STRING(GDL_TYP_INT, i);
					DOCASE_STRING(GDL_TYP_LONG, l);
					DOCASE_STRING(GDL_TYP_FLOAT, f);
					DOCASE_STRING(GDL_TYP_DOUBLE, d);
					DOCASE_STRING(GDL_TYP_COMPLEX, cmp);
					DOCASE_STRING(GDL_TYP_DCOMPLEX, dcmp);
					DOCASE_STRING(GDL_TYP_UINT, ui);
					DOCASE_STRING(GDL_TYP_ULONG, ul);
					DOCASE_STRING(GDL_TYP_LONG64, l64);
					DOCASE_STRING(GDL_TYP_ULONG64, ul64);
				default: GDL_WillThrowAfterCleaning(__func__, "Internal error: Unknown combination in convert()..");
			}
		}
		return ret;

	}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtString(int argc, EXPORT_VPTR argv[], char *argk=NULL){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
    if (argc !=1 || (argv[0]->type != GDL_TYP_BYTE) ) return GDL_Other_CvtString(argc, argv); 
	// only left: argc=1 and type is gdl_typ_byte. We expect strings as results.
		EXPORT_VPTR ret = NewTMPVPTR();
		ret->type = GDL_TYP_STRING;

		
		// if argv[0] is a 1D byte array or a single byte ret will be a single string
		if (ISSCALAR(argv[0])) {
			char *dstval = (char*) MyMallocDestroyedOnExit(1);
			ret->value.str.s=dstval;
			ret->value.str.slen=1;
			dstval[0]=argv[0]->value.c;
			return ret;
		}
		if (argv[0]->value.arr->n_dim == 1) {
			char *dstval = (char*) MyMallocDestroyedOnExit(argv[0]->value.arr->n_elts);
			ret->value.str.s=dstval;
			ret->value.str.slen=argv[0]->value.arr->n_elts;
			memcpy(dstval,argv[0]->value.arr->data,ret->value.str.slen);
			return ret;
		}
		// else result will have ndim-1
		EXPORT_ARRAY *dstArrayDescr = NewExportArray();
		ret->flags |= GDL_V_ARR;
		ret->value.arr = dstArrayDescr;
	// else: argv[0] is a string array
		EXPORT_ARRAY* srcArrayDescr = argv[0]->value.arr;
		//copy common values
		int sz = srcArrayDescr->dim[0];
		//update related descrs
		dstArrayDescr->elt_len = sizeof(EXPORT_STRING);
		dstArrayDescr->data_guard = 0; //?????????????? TBC
		dstArrayDescr->n_elts = srcArrayDescr->n_elts/sz;
		dstArrayDescr->flags = 0;
		dstArrayDescr->n_dim = srcArrayDescr->n_dim - 1;
		memset(dstArrayDescr->dim,0,GDL_MAX_ARRAY_DIM);
		for (auto i = 1; i<srcArrayDescr->n_dim; ++i) dstArrayDescr->dim[i-1] = srcArrayDescr->dim[i];
		dstArrayDescr->arr_len = dstArrayDescr->elt_len*dstArrayDescr->n_elts;
		char *dstval = (char*) gdlAlignedMalloc(dstArrayDescr->arr_len);
		dstArrayDescr->data=(UCHAR*)dstval;
		// create pointers 
		EXPORT_STRING* p=(EXPORT_STRING*)dstval;
		for (auto i = 0, k = 0; i < dstArrayDescr->n_elts; ++i, k+=sz) { //for each dest string
			p[i].s=(char*) MyMallocDestroyedOnExit(sz);
			p[i].slen=sz;
			memcpy(p[i].s,(srcArrayDescr->data+k),sz);
		}
		return ret;

}
#undef DOCASE_STRING
#undef DOCASE_STRING_ARRAY
#undef PREPARE_ARRAY
#undef DEFOUT
#undef TREAT_MULTIPLE_ARGS
#undef DOCASE_ARRAY_FROM_STRING

char* GDLConvertToAndWriteVarAtAddr(BaseGDL* var, std::string name, UCHAR type, size_t address, bool isoutput, bool isarray){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		switch (type) {
			case GDL_TYP_BYTE:
			{
				DByteGDL* res = static_cast<DByteGDL*> (var->Convert2(GDL_BYTE));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (UCHAR));
				break;
			}
			case GDL_TYP_INT:
			{
				DIntGDL* res = static_cast<DIntGDL*> (var->Convert2(GDL_INT));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (EXPORT_INT));
				break;
			}
			case GDL_TYP_LONG:
			{
				DLongGDL* res = static_cast<DLongGDL*> (var->Convert2(GDL_LONG));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (EXPORT_LONG));
				break;
			}
			case GDL_TYP_FLOAT:
			{
				DFloatGDL* res = static_cast<DFloatGDL*> (var->Convert2(GDL_FLOAT));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (float));
				break;
			}
			case GDL_TYP_DOUBLE:
			{
				DDoubleGDL* res = static_cast<DDoubleGDL*> (var->Convert2(GDL_DOUBLE));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (double));
				break;
			}
			case GDL_TYP_COMPLEX:
			{
				DComplexGDL* res = static_cast<DComplexGDL*> (var->Convert2(GDL_COMPLEX));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (EXPORT_COMPLEX));
				break;
			}
			case GDL_TYP_DCOMPLEX:
			{
				DComplexDblGDL* res = static_cast<DComplexDblGDL*> (var->Convert2(GDL_COMPLEXDBL));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (EXPORT_DCOMPLEX));
				break;
			}
			case GDL_TYP_UINT:
			{
				DUIntGDL* res = static_cast<DUIntGDL*> (var->Convert2(GDL_UINT));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (EXPORT_UINT));
				break;
			}
			case GDL_TYP_ULONG:
			{
				DULongGDL* res = static_cast<DULongGDL*> (var->Convert2(GDL_ULONG));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (EXPORT_ULONG));
				break;
			}
			case GDL_TYP_ULONG64:
			{
				DULong64GDL* res = static_cast<DULong64GDL*> (var->Convert2(GDL_ULONG64));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (EXPORT_ULONG64));
				break;
			}
			case GDL_TYP_LONG64:
			{
				DLong64GDL* res = static_cast<DLong64GDL*> (var->Convert2(GDL_LONG64));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (EXPORT_LONG64));
				break;
			}
			case GDL_TYP_STRING:
			{
				DStringGDL* res = static_cast<DStringGDL*> (var->Convert2(GDL_STRING));
				if (isarray) {
					SizeT nEl=var->N_Elements();
					void* allstringdescr=MyMallocDestroyedOnExit(nEl*sizeof(EXPORT_STRING));
					memset(allstringdescr,0,nEl*sizeof(EXPORT_STRING));
					void* stringdescPtrs=MyMallocDestroyedOnExit(nEl*sizeof(EXPORT_STRING*));
					EXPORT_STRING** p=(EXPORT_STRING**)stringdescPtrs;
					for (SizeT i=0; i< nEl; ++i) p[i]=(EXPORT_STRING*)((SizeT)allstringdescr+(i*sizeof(EXPORT_STRING)));
					for (auto i=0; i< nEl; ++i) {
						p[i]->slen = ((*res)[i]).size();
						if (p[i]->slen > 0) {
							p[i]->s = (char*) MyMallocDestroyedOnExit(p[i]->slen + 1);
						    strncpy(p[i]->s, (*res)[i].c_str(), p[i]->slen + 1);
						}
					}
					return (char*)allstringdescr;
				//write an array of string descriptors at address; return vector of addresses to be destroyed when GDL_KWFree will be called
				} else {
					EXPORT_STRING *s=(EXPORT_STRING*) (address); //string descr is at address
					s->slen = res->NBytes();
					if (s->slen > 0) {
						s->s = (char*) MyMallocDestroyedOnExit(s->slen + 1);
					    strncpy(s->s, (*res)[0].c_str(), s->slen + 1);
					} else s->s=NULL;
					return (char*)s;
				}
			}
			case GDL_TYP_UNDEF:
			{
				if (!isoutput) GDL_WillReturnAfterCleaning("GDLWriteVarAtAddr: variable " + name + " is not writeable.");
				break;
			}
//			case GDL_TYP_PTR:
//				GDL_WillReturnAfterCleaning("Unable to convert variable to type pointer.");
//				break;
//			case GDL_TYP_OBJREF:
//				GDL_WillReturnAfterCleaning("Unable to convert variable to type object.");
//				break;
			default: GDL_WillReturnAfterCleaning("GDLWriteVarAtAddr: unsupported case.");
		}
		return NULL;
	}
	
void GDLZeroAtAddr(size_t address, int type ){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
int l=IDL_TypeSizeFunc(type);
memset((void*) (address), 0, l);
}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_BasicTypeConversion(int argc, EXPORT_VPTR argv[], GDL_REGISTER int type);

//works for OLD an NEW API, by checking if kw_result is NULL.
EXPORT_VPTR GdlExportPresentKeyword(GDL_KW_PAR requested, GDL_KEYWORDS_LIST passed, void* kw_result) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
static const int ok = 1;
static const int cleanMem = 1;
EXPORT_VPTR toBeReturned = NULL;
bool global=passed.global;
bool iszero = ((requested.flags & GDL_KW_ZERO) == GDL_KW_ZERO); //zero field if requested
// Indicates parameter is an OUTPUT parameter passed by reference.
// Expressions are excluded.  The address of the EXPORT_VARIABLE is stored in
// the value field.  Otherwise, no checking is performed.  Special hint:
// to find out if a GDL_KW_OUT parameter is specified, use 0 for the type,
// and GDL_KW_OUT | GDL_KW_ZERO for the flags.  The value field will either
// contain NULL or the pointer to the variable. */
bool inputByReference = ((requested.flags & GDL_KW_VIN) == GDL_KW_VIN); // input, but passed by reference
bool isoutput = ((requested.flags & GDL_KW_OUT) == GDL_KW_OUT);
bool isarray = ((requested.flags & GDL_KW_ARRAY) == GDL_KW_ARRAY); //var must be an array, field is a GDL_KW_ARR_DESC_R*
bool byMask = ((requested.flags & GDL_KW_VALUE) == GDL_KW_VALUE);
// tag 'out' those that will get a return value
if (requested.specified != NULL) { // need write 0 or 1 in a special int in KW structure
	size_t global_address = (size_t) (kw_result)+(size_t) (requested.specified);
	memcpy((void*) (global_address), &ok, sizeof (int)); //requested is in offset
}
if (requested.value != NULL) { // need to pass either an address of a EXPORT_VPTR or fill in static elements of the structure exchanged with routine
	size_t global_address = (size_t) (kw_result)+(size_t) (requested.value);
	if (isoutput && !global) GDL_WillReturnAfterCleaning("Keyword " + std::string(requested.keyword) + " must be a named variable.");
	BaseGDL* var = passed.gdlVarPtr;
	//if requested var is NULL here, it is an undefined var, which MAY be returned as good value.
	if (var == NULL && !isoutput) GDL_WillReturnAfterCleaning("GDLExportKeyword: variable " + std::string(requested.keyword) + " is not defined.");
	if (iszero) {
		if (isoutput) {
			memset((void*) (global_address), 0, sizeof(EXPORT_VPTR*)); /* Special hint:
   to find out if a IDL_KW_OUT parameter is specified, use 0 for the type,
   and IDL_KW_OUT | IDL_KW_ZERO for the flags.  The value field will either
   contain NULL or the pointer to the variable. */
		} else {
			GDLZeroAtAddr(global_address, requested.type); //will complain if type is 0
		}
	}
	if (var != NULL) {
		if (inputByReference) {//address (relative) of a EXPORT_VPTR with input only value
			memcpy((void*) (global_address), (void*) (&passed.convertedVPTR), sizeof (EXPORT_VPTR)); //pass by address of a EXPORT_VAR
			return NULL;
		} else if (isoutput) { //address (relative) of a EXPORT_VPTR where input/output value is written (if existing) and will be returned 
			toBeReturned = passed.convertedVPTR; //to be returned
			memcpy((void*) (global_address), (void*) (&toBeReturned), sizeof (EXPORT_VPTR)); //pass address of a EXPORT_VAR that will contain the result.
			return toBeReturned;
		} else if (isarray) {
			size_t array_desc_address = (size_t) (requested.value);
			GDL_KW_ARR_DESC_R* arr_desc = (GDL_KW_ARR_DESC_R*) (array_desc_address);
			//check limits
			if ((var->N_Elements() > (*arr_desc).nmax) || (var->N_Elements() < (*arr_desc).nmin))
				GDL_WillReturnAfterCleaning(
					"Keyword array parameter " + std::string(requested.keyword) + " must have from " + i2s(arr_desc->nmin) + " to " + i2s(arr_desc->nmax) + " elements.");
			//and these are offsets if kw_result is not NULL (new API, by offset in the kw_result structure)
			int *passedArraySize;
			if (kw_result) passedArraySize=(int *) ( (size_t) (kw_result)+(size_t) ((*arr_desc).n_offset)); 
			else passedArraySize=(int *) &((*arr_desc).n_offset); //n_offset may also be the odl api "n"
			*passedArraySize = var->N_Elements();
			size_t data_address = (size_t) (kw_result)+(size_t) ((*arr_desc).data); //address where to pass elements
			if (GDLConvertToAndWriteVarAtAddr(var, std::string(requested.keyword), requested.type, data_address, isoutput, true) != NULL) {
				if (kw_result) memcpy(kw_result, &cleanMem, sizeof (int)); //make GDL_KW_FREE in called program call GDL_KWFree();
			}
			return NULL;
		} else {
			//here GDL_KW_VALUE may appear
			if (byMask) {
				// Be sure that the type field contains TYP_LONG.
				if (requested.type != GDL_TYP_LONG) GDL_WillReturnAfterCleaning("Invalid use of GDL_KW_VALUE on non-integer keyword.");
				// and its value is non-zero ...
				// must convert to long...
				if (var->LogTrue()) {
					long mask = GDL_KW_VALUE_MASK & requested.flags;
					long *val = (long*) global_address;
					*val |= mask;
				}
			} else {
				if (GDLConvertToAndWriteVarAtAddr(var, std::string(requested.keyword), requested.type, global_address, isoutput, false) != NULL) {
					if (kw_result) memcpy(kw_result, &cleanMem, sizeof (int)); //make GDL_KW_FREE in called program call GDL_KWFree();
				}
			}
			return NULL;
		}
	} else if (isoutput) {
				toBeReturned = NewNAMEDVPTR(); //create a global EXPORT_VPTR that will return a real variable
				memcpy((void*) global_address, (void*) (&toBeReturned), sizeof (EXPORT_VPTR)); //pass address of a EXPORT_VAR that will contain the result.
                return toBeReturned;
	}
}
return NULL;
}

void GdlExportAbsentKeyword(GDL_KW_PAR requested, void* kw_result) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		static const int ok = 1;
		static const int nok = 0;
		
		bool iszero = ((requested.flags & GDL_KW_ZERO) == GDL_KW_ZERO); //zero field if requested
		bool byAddress = ((requested.flags & GDL_KW_VIN) == GDL_KW_VIN);// input, but passed by address
		bool isarray = (!byAddress && ( requested.flags & GDL_KW_ARRAY) == GDL_KW_ARRAY ); //var will be an arry
//		bool isoutput = ((requested.flags & GDL_KW_OUT) == GDL_KW_OUT); // output, hence passed by address
//		bool byMask = ((requested.flags & GDL_KW_VALUE) == GDL_KW_VALUE);
		// tag 'out' those that will get a return value
		if (requested.specified != NULL) { // need write 0 or 1 in a special int in KW structure
			size_t address = (size_t) (kw_result)+(size_t) (requested.specified);
			memcpy((void*) (address), &nok, sizeof (int)); //requested is in offset
		}
		if (requested.value != NULL) { // need to pass either an address of a EXPORT_VPTR or fill in static elements of the structure exchanged with routine
			// GDL_KW_VIN types (GDL_KW_OUT | GDL_KW_ARRAY) have an absolute address.
			//if requested var is not present, this has to be returned
			if (iszero) { //zero before write.
				size_t address;
				if (isarray) {
					address = (size_t) (requested.value);
					memset((void*) address,0,sizeof(size_t));
				} else {
					address = (size_t) (kw_result)+(size_t) (requested.value);
					GDLZeroAtAddr(address, requested.type);
				}
			}
		}
	}
void dumpkw(GDL_KW_PAR kw){
	std::cerr<<"name: "<<kw.keyword<<std::endl;
	std::cerr<<" flags: "<<std::endl;
	if (kw.flags & GDL_KW_ARRAY) std::cerr<<"ARRAY ,";"ARRAY ,";
	if (kw.flags & GDL_KW_OUT) std::cerr<<"OUT ,";
	if (kw.flags & GDL_KW_VIN) std::cerr<<"VIN,";
	if (kw.flags & GDL_KW_ZERO) std::cerr<<"ZERO ,";
	if (kw.flags & GDL_KW_VALUE) std::cerr<<"VALUE ,";
	if (kw.flags & GDL_KW_VALUE_MASK) {std::cerr<<"VALUE_MASK (";long mask = GDL_KW_VALUE_MASK & kw.flags; fprintf(stderr," %d [ ox%x ] )",mask,mask);}
	std::cerr<<"\n mask: "<<kw.mask<<std::endl;
	std::cerr<<" specified: 0x"<<std::hex<<kw.specified<<std::endl;
	std::cerr<<" type: "<<(int) kw.type<<std::endl;
	std::cerr<<" value: 0x"<<std::hex<<(size_t)kw.value<<std::endl;
}
#define KEYWORD_ACCEPT -1
#define KEYWORD_IGNORE -2
DLL_PUBLIC int  GDL_CDECL IDL_KWGetParams(int argc, EXPORT_VPTR *argv, char *argk_passed, GDL_KW_PAR *kw_requested, EXPORT_VPTR *plain_args, int mask) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		int hasplain=(plain_args != NULL);
        
//		if (hasplain) std::cerr << "IDL_KWGetParams: HAS PLAIN ARGS" << std::endl;

		GDL_PASS_KEYWORDS_LIST* container = (GDL_PASS_KEYWORDS_LIST*) argk_passed;
		int npassed = 0;
		int nplain=argc;
		GDL_KEYWORDS_LIST* argk;
		if (container) {
			npassed = container->npassed;
			argk = container->passed;
			nplain=argc-npassed;
		}
		//build a vector of desired keywords 
		int ikw = 0;
		int irequested = 0;
		const char * kw;
		bool do_fast_scan = false; // not yet used, but retrieved.
		if (kw_requested == NULL) {
			if (hasplain) { for (auto i = 0; i < argc; ++i) plain_args[i] = argv[i];}  return nplain;
		}
		kw = kw_requested[0].keyword;
		if (kw == NULL) {
			if (hasplain) { for (auto i = 0; i < argc; ++i) plain_args[i] = argv[i];} return nplain;
		}
		if (kw[0] == 0) {
			do_fast_scan = true;
			ikw++; // start at 1 next item
			irequested++; //start at 1 too below
		}
		if (hasplain) { for (auto i = 0; i < argc-npassed; ++i) plain_args[i] = argv[i]; }
		//get the list of expected KWs. Initial value of <int> in map will be: ACCEPT (take into account) IGNORE:ignored
		std::map<int, int> requested;
		std::map<int, int>::iterator it;
		while ((kw = kw_requested[ikw].keyword) != NULL) {
#ifdef GDL_DEBUG
			std::cerr << "\nrequested kw: " << kw << std::endl;
			dumpkw(kw_requested[ikw]);
#endif
			int code = KEYWORD_ACCEPT;
			if ((kw_requested[ikw].mask & mask) == 0) code = KEYWORD_IGNORE;
			requested.insert(std::pair<int, int>(ikw, code));
			ikw++;
		}
		for (auto ipassed = 0; ipassed < npassed; ++ipassed) {
			const char* s = argk[ipassed].name;
			int l = strlen(s);
			bool found = false;
			bool ignored = false;
			for (it = requested.begin(); it != requested.end(); ++it) {
				const char* expected_kw = kw_requested[it->first].keyword;
				if (strncmp(expected_kw, s, l) == 0) { //found
					found = true;
//					std::cerr<<"found kw: "<<expected_kw<<std::endl;
					if (it->second == KEYWORD_IGNORE) { //ignored
						ignored = true;
						break;
					}
					it->second = ipassed;
					break;
				}
			}
			if (!found && !ignored) {
				GDL_WillReturnAfterCleaning("Invalid keyword " + std::string(s));
			} else if (found && ignored) {
				GDL_WillReturnAfterCleaning("Keyword " + std::string(s) + " not allowed in call.");
			} else {
				for (++it; it != requested.end(); ++it) { //search for ambiguous KW
					const char* expected_kw =  kw_requested[it->first].keyword;
					if (strncmp(expected_kw, s, l) == 0) GDL_WillReturnAfterCleaning("Ambiguous keyword abbreviation: " + std::string(s));
				}
			}
		}
		//populate all passed addresses
		for (it = requested.begin(); it != requested.end(); ++it) {
			int ipassed = it->second;
			if (ipassed == KEYWORD_ACCEPT) GdlExportAbsentKeyword(kw_requested[it->first], NULL);
			else if (ipassed >= 0) {
			argk[ipassed].out = false;
			EXPORT_VPTR ret = GdlExportPresentKeyword(kw_requested[it->first], argk[it->second], NULL);
				if (ret != NULL) {
					argk[ipassed].out = true;
					argk[ipassed].convertedVPTR = ret; //pass vptr back
					PassedVariablesNames[ret] = argk[ipassed].varname; //memorize GDL varname
				}
			}
			irequested++;
		}
		return nplain;
	}

DLL_PUBLIC int  GDL_CDECL IDL_KWProcessByOffset(int argc, EXPORT_VPTR *argv, char *argk_passed, GDL_KW_PAR *kw_requested, EXPORT_VPTR *plain_args, int mask, void *kw_result) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
				static const int NoClean = 0;
		int hasplain=(plain_args != NULL);
        
//		if (hasplain) std::cerr << "IDL_KWGetParams: HAS PLAIN ARGS" << std::endl;

		//argk is a pointer to a  GDL_PASS_KEYWORDS_LIST struct
		GDL_PASS_KEYWORDS_LIST* container = (GDL_PASS_KEYWORDS_LIST*) argk_passed;
		int npassed = 0;
		int nplain=argc;
		GDL_KEYWORDS_LIST* argk;
		if (container) {
			npassed = container->npassed;
			argk = container->passed;
			nplain=argc-npassed;
		}
		//build a vector of desired keywords 
		int ikw = 0;
		int irequested = 0;
		const char * kw;
		bool do_fast_scan = false; // not yet used, but retrieved.
		if (kw_requested == NULL) {
			if (hasplain) { for (auto i = 0; i < argc; ++i) plain_args[i] = argv[i];}  return nplain;
		}
		kw = kw_requested[0].keyword;
		if (kw == NULL) {
			if (hasplain) { for (auto i = 0; i < argc; ++i) plain_args[i] = argv[i];} return nplain;
		}
		if (kw[0] == 0) {
			do_fast_scan = true;
			ikw++; // start at 1 next item
			irequested++; //start at 1 too below
		}
		if (hasplain) { for (auto i = 0; i < argc-npassed; ++i) plain_args[i] = argv[i]; }
		//get the list of expected KWs. Initial value of <int> in map will be: ACCEPT (take into account) IGNORE:ignored
		std::map<int, int> requested;
		std::map<int, int>::iterator it;
		while ((kw = kw_requested[ikw].keyword) != NULL) {
#ifdef GDL_DEBUG
			std::cerr << "\nrequested kw: " << kw << std::endl;
			dumpkw(kw_requested[ikw]);
#endif
			int code = KEYWORD_ACCEPT;
			if ((kw_requested[ikw].mask & mask) == 0) code = KEYWORD_IGNORE;
			requested.insert(std::pair<int, int>(ikw, code));
			ikw++;
		}
		for (auto ipassed = 0; ipassed < npassed; ++ipassed) {
			const char* s = argk[ipassed].name;
			int l = strlen(s);
			bool found = false;
			bool ignored = false;
			for (it = requested.begin(); it != requested.end(); ++it) {
				const char* expected_kw = kw_requested[it->first].keyword;
				if (strncmp(expected_kw, s, l) == 0) { //found
					found = true;
					if (it->second == KEYWORD_IGNORE) { //ignored
						ignored = true;
						break;
					}
					it->second = ipassed;
					break;
				}
			}
			if (!found && !ignored) {
				GDL_WillReturnAfterCleaning("Invalid keyword " + std::string(s));
			} else if (found && ignored) {
				GDL_WillReturnAfterCleaning("Keyword " + std::string(s) + " not allowed in call.");
			} else {
				for (++it; it != requested.end(); ++it) { //search for ambiguous KW
					const char* expected_kw =  kw_requested[it->first].keyword;
					if (strncmp(expected_kw, s, l) == 0) GDL_WillReturnAfterCleaning("Ambiguous keyword abbreviation: " + std::string(s));
				}
			}
		}
		//populate kw_result
		//first set GDL_KW_RESULT_FIRST_FIELD to zero (no need to clean)
		memcpy(kw_result, &NoClean, sizeof (int));
		//rewind: 
		for (it = requested.begin(); it != requested.end(); ++it) {
			int ipassed = it->second;
			argk[ipassed].out = false;
			if (ipassed == KEYWORD_ACCEPT) GdlExportAbsentKeyword(kw_requested[it->first], kw_result);
			else if (ipassed >= 0) {
				EXPORT_VPTR ret=GdlExportPresentKeyword(kw_requested[it->first], argk[it->second], kw_result);
			    if (ret != NULL) {
					argk[ipassed].out=true;
					argk[ipassed].convertedVPTR=ret; //pass vptr back
					PassedVariablesNames[ret]=argk[ipassed].varname; //memorize GDL varname
				}
			}
		}
		return nplain;
	}

#undef KEYWORD_ACCEPT
#undef KEYWORD_IGNORE

DLL_PUBLIC int GDL_CDECL IDL_KWProcessByAddr(int argc, EXPORT_VPTR *argv, char *argk_passed,  GDL_KW_PAR *kw_requested, EXPORT_VPTR *plain_args, int mask, int *free_required) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) 
	*free_required=0;
	return IDL_KWGetParams(argc, argv, argk_passed, kw_requested, plain_args, mask);
}

DLL_PUBLIC void  GDL_CDECL IDL_KWFree(void) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	for (auto it = GlobalVPTRKwList.begin(); it != GlobalVPTRKwList.end(); ++it) IDL_Deltmp(*it);
	GlobalVPTRKwList.clear();
};
DLL_PUBLIC void  GDL_CDECL IDL_KWFreeAll(void) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	for (auto it = GlobalVPTRKwList.begin(); it != GlobalVPTRKwList.end(); ++it) IDL_Deltmp(*it);
	GlobalVPTRKwList.clear();
};

	DLL_PUBLIC void * GDL_CDECL IDL_MemAlloc(EXPORT_MEMINT n, const char *err_str, int msg_action) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return gdlAlignedMalloc(n);
	}

	DLL_PUBLIC void * GDL_CDECL IDL_MemRealloc(void *ptr, EXPORT_MEMINT n, const char *err_str, int action) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return realloc(ptr, n); //could be NOT ALIGNED!
	}

	DLL_PUBLIC void GDL_CDECL IDL_MemFree(GDL_REGISTER void *m, const char *err_str, int msg_action) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) gdlAlignedFree(m);
	}

	DLL_PUBLIC void * GDL_CDECL IDL_MemAllocPerm(EXPORT_MEMINT n, const char *err_str, int action) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return gdlAlignedMalloc(n);
	}

	DLL_PUBLIC char * GDL_CDECL IDL_GetScratch(GDL_REGISTER EXPORT_VPTR *v, GDL_REGISTER EXPORT_MEMINT n_elts, GDL_REGISTER EXPORT_MEMINT elt_size) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		*v=NewTMPVPTRARRAY();
		(*v)->type=1;
		(*v)->value.arr->data=(UCHAR*) gdlAlignedMalloc(n_elts * elt_size);
		(*v)->value.arr->arr_len=n_elts * elt_size;
		(*v)->value.arr->dim[0]=n_elts;
		(*v)->value.arr->elt_len=elt_size;
		(*v)->value.arr->n_dim=0;
		(*v)->value.arr->n_elts=n_elts;
		return (char*) (*v)->value.arr->data;
	}

	DLL_PUBLIC void GDL_CDECL GDL_KWCleanup(int fcn) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) IDL_KWFree();
	}


EXPORT_TERMINFO IDL_TermInfo={
#ifdef GDL_OS_HAS_TTYS
  (char*)"Xterm" ,          /* Name of terminal type */
  1    ,             /* True if stdin is a terminal */
#endif
  24,128};

DLL_PUBLIC char * GDL_CDECL IDL_FileTermName(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) 
#ifdef GDL_OS_HAS_TTYS
	return IDL_TermInfo.name;
#else
	return (char*)"<Anonymous>";
#endif
}

DLL_PUBLIC int  GDL_CDECL IDL_FileTermIsTty(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) 
#ifdef GDL_OS_HAS_TTYS
	return IDL_TermInfo.is_tty;
#else
	return 0;
#endif
}

DLL_PUBLIC int  GDL_CDECL IDL_FileTermLines(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return IDL_TermInfo.lines;}

DLL_PUBLIC int  GDL_CDECL IDL_FileTermColumns(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return IDL_TermInfo.columns;}

EXPORT_SYS_VERSION IDL_SysvVersion={{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL},0,0};

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvVersionArch(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return &(IDL_SysvVersion.arch);}

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvVersionOS(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return &(IDL_SysvVersion.os);};

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvVersionOSFamily(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return &(IDL_SysvVersion.os_family);};

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvVersionRelease(void){return &(IDL_SysvVersion.release);};

char *IDL_ProgramName=(char*)"gdl";

DLL_PUBLIC char * GDL_CDECL IDL_ProgramNameFunc(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return (char*)"gdl";}

char *IDL_ProgramNameLC=(char*)"gdl";

DLL_PUBLIC char * GDL_CDECL IDL_ProgramNameLCFunc(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return (char*)"gdl";}

EXPORT_STRING IDL_SysvDir={0,0,NULL};

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvDirFunc(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return &IDL_SysvDir;}

EXPORT_LONG IDL_SysvErrCode=0;

DLL_PUBLIC EXPORT_LONG  GDL_CDECL IDL_SysvErrCodeValue(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return 0;};

EXPORT_SYS_ERROR_STATE IDL_SysvErrorState={{0,0,NULL},{0,0,NULL},0,{0,0},{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL}};

DLL_PUBLIC EXPORT_SYS_ERROR_STATE * GDL_CDECL IDL_SysvErrorStateAddr(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return &IDL_SysvErrorState;}

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvErrStringFunc(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return &(IDL_SysvErrorState.msg);}

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvSyserrStringFunc(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return &(IDL_SysvErrorState.sys_msg);}

DLL_PUBLIC EXPORT_LONG  GDL_CDECL IDL_SysvErrorCodeValue(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return IDL_SysvErrorState.code;}

DLL_PUBLIC EXPORT_LONG * GDL_CDECL IDL_SysvSyserrorCodesAddr(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return &(IDL_SysvErrorState.code);}

EXPORT_LONG IDL_SysvOrder=0;

DLL_PUBLIC EXPORT_LONG  GDL_CDECL IDL_SysvOrderValue(void){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return 0;}

DLL_PUBLIC float  GDL_CDECL IDL_SysvValuesGetFloat(int type){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return 0;}

DLL_PUBLIC int  GDL_CDECL IDL_MessageNameToCode(EXPORT_MSG_BLOCK block, const char *name){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return 3333;} //a random (!) value
DLL_PUBLIC EXPORT_MSG_BLOCK  GDL_CDECL IDL_MessageDefineBlock(char *block_name, int n, EXPORT_MSG_DEF *defs){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)  return MyMallocDestroyedOnExit(1);} //do nothing but returns an "address"
DLL_PUBLIC void  GDL_CDECL IDL_MessageErrno(int code, ...){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) } //do nothing. obsoleted.
DLL_PUBLIC void  GDL_CDECL IDL_MessageErrnoFromBlock(EXPORT_MSG_BLOCK block, int code, ...){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) } //do nothing. obsoleted.
DLL_PUBLIC void  GDL_CDECL IDL_Message(int code, int action, ...) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		std::string finalMessage = "";
		char* s;
		if (code == EXPORT_M_GENERIC || code == EXPORT_M_NAMED_GENERIC) {
			va_list args;
			va_start(args, action);
			s=va_arg(args,char*);
			if (s != NULL) {
				finalMessage += std::string(s);
			}
			va_end(args);
		} else GDL_WillReturnAfterCleaning("Invalid Error Code given to IDL_Message() by user-written routine.");
		if (action == EXPORT_MSG_INFO || action == EXPORT_MSG_RET) {Warning(finalMessage); return;}
		if (action == EXPORT_MSG_LONGJMP || action == EXPORT_MSG_IO_LONGJMP) GDL_WillReturnAfterCleaning(finalMessage);//exit directly back to the interpreter
		if (action == EXPORT_MSG_EXIT) {
			Warning(finalMessage);
			GDL_WillReturnAfterCleaning("IDL_MSG_EXIT forbidden for user-written routines.");
		}
	}
//JUST IGNORE BLOCK for the moment
DLL_PUBLIC void  GDL_CDECL IDL_MessageFromBlock(EXPORT_MSG_BLOCK block, int code, int action,...){
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		std::string finalMessage = "";
		char* s;
			va_list args;
			va_start(args, action);
			s=va_arg(args,char*);
			if ((size_t) s < 32768) { //not a string, more probably an integer code
				int i=(size_t) s;
				finalMessage += " Error code "+i2s(i);
			} else {
				finalMessage += std::string(s);
			}
			va_end(args);
		if (action == EXPORT_MSG_INFO || action == EXPORT_MSG_RET)  {Warning(finalMessage); return;}
		if (action == EXPORT_MSG_LONGJMP ||action == EXPORT_MSG_IO_LONGJMP) GDL_WillReturnAfterCleaning(finalMessage);
		if (action == EXPORT_MSG_EXIT) {
			Warning(finalMessage);
			GDL_WillReturnAfterCleaning("IDL_MSG_EXIT forbidden for user-written routines.");
		}
}
DLL_PUBLIC void  GDL_CDECL IDL_MessageSyscode(int code, EXPORT_MSG_SYSCODE_T syscode_type, int syscode, int action, ...){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) }//{if (action!=EXPORT_MSG_INFO) GDL_WillReturnAfterCleaning("exception caused by non-GDL (dlm) function call.");}//do nothing.
DLL_PUBLIC void  GDL_CDECL IDL_MessageSyscodeFromBlock(EXPORT_MSG_BLOCK block, int code, EXPORT_MSG_SYSCODE_T syscode_type,  int syscode, int action, ...){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) }//{if (action!=EXPORT_MSG_INFO) GDL_WillReturnAfterCleaning("exception caused by non-GDL (dlm) function call.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVarError(int code, EXPORT_VPTR var, int action){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) }//{if (action!=EXPORT_MSG_INFO) GDL_WillReturnAfterCleaning("exception caused by non-GDL (dlm) function call.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVarErrorFromBlock(EXPORT_MSG_BLOCK block, int code, EXPORT_VPTR var, int action){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) }//{if (action!=EXPORT_MSG_INFO) GDL_WillReturnAfterCleaning("exception caused by non-GDL (dlm) function call.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageResetSysvErrorState(void) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) 
		try {
			std::string command = ("message,/reset");
			DInterpreter::CallStackBack()->Interpreter()->ExecuteStringLine(command);
		} catch (...) {
		}
}

#include <stdio.h>
#include <stdarg.h>
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_BasicTypeConversion(int argc, EXPORT_VPTR argv[], GDL_REGISTER int type){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	switch(type) {
		case GDL_TYP_BYTE:     return GDL_Other_CvtByte(argc, argv); //not CvtByte, behaviour is a bit different, see documentation.
		case GDL_TYP_INT:     return IDL_CvtFix(argc, argv);
		case GDL_TYP_LONG:    return IDL_CvtLng(argc, argv);
		case GDL_TYP_FLOAT:    return IDL_CvtFlt(argc, argv);
		case GDL_TYP_DOUBLE:    return IDL_CvtDbl(argc, argv);
		case GDL_TYP_COMPLEX:    return IDL_CvtComplex(argc, argv, (char*)1); // IDL_CvtComplex will handle this
		case GDL_TYP_DCOMPLEX:    return IDL_CvtComplex(argc, argv, (char*)2);// called as  IDL_CvtComplex but will transfer to IDL_CvtDComplex
		case GDL_TYP_UINT:    return IDL_CvtUInt(argc, argv);
		case GDL_TYP_ULONG:    return IDL_CvtULng(argc, argv);
		case GDL_TYP_LONG64:    return IDL_CvtLng64(argc, argv);
		case GDL_TYP_ULONG64:    return IDL_CvtULng64(argc, argv);
		case GDL_TYP_STRING:    return GDL_Other_CvtString(argc, argv, NULL); 
		default: GDL_WillThrowAfterCleaning(__func__, "Internal error: Unknown combination in convert()..");
	}
	return NULL;
	}

	DLL_PUBLIC void GDL_CDECL IDL_VarGetData(EXPORT_VPTR v, EXPORT_MEMINT *n, char **pd, int ensure_simple) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		checkOK(v);
		if (ensure_simple) GDL_ENSURE_SIMPLE(v);
		else GDL_EXCLUDE_FILE(v);
		if ((ISARRAY(v)) && (v->value.arr != NULL)) {
			*n = v->value.arr->n_elts;
			*pd = (char*) v->value.arr->data;
		} else {
			*n = 1;
			*pd = (char*) &(v->value);
		}

	}

DLL_PUBLIC int  GDL_CDECL IDL_AddSystemRoutine(EXPORT_SYSFUN_DEF *defs, int is_function, int cnt){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return 1;} //using DLM insure this is OK. I think.
DLL_PUBLIC int  GDL_CDECL IDL_BailOut(int stop){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return sigControlC;} //use of stop not supported.
DLL_PUBLIC void  GDL_CDECL IDL_ExitRegister(EXPORT_EXIT_HANDLER_FUNC proc){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) }
DLL_PUBLIC void  GDL_CDECL IDL_ExitUnregister(EXPORT_EXIT_HANDLER_FUNC proc){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) }
DLL_PUBLIC int  GDL_CDECL IDL_GetExitStatus(){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return 0;} //do nothing
DLL_PUBLIC int  GDL_CDECL IDL_Cleanup(int just_cleanup){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return 1;} //do nothing
DLL_PUBLIC int  GDL_CDECL IDL_Initialize(EXPORT_INIT_DATA *init_data){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return 1;} //do nothing
DLL_PUBLIC int  GDL_CDECL IDL_Init(EXPORT_INIT_DATA_OPTIONS_T options, int *argc, char *argv[]){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return 1;} //do nothing
DLL_PUBLIC int  GDL_CDECL IDL_Main(EXPORT_INIT_DATA_OPTIONS_T options, int argc, char *argv[]){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) return 1;} //do nothing
DLL_PUBLIC int  GDL_CDECL IDL_ExecuteStr(char *cmd) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) 
  std::string command(cmd);
 //always between try{} catch{} when calling ExecuteStringLine!
  try {
  DInterpreter::CallStackBack()->Interpreter()->ExecuteStringLine(command);
  } catch (...) {std::cerr<<"Problem executing command: "<<command<<" ."<<std::endl; return 0;}
  return 1;
 }
DLL_PUBLIC int  GDL_CDECL IDL_Execute(int argc, char *argv[]){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) 
	std::string command(argv[0]);
	for (auto i=1;i< argc; ++i) {command.append(" & "); command.append(argv[i]);}
  try {
  DInterpreter::CallStackBack()->Interpreter()->ExecuteStringLine(command);
  } catch (...) {std::cerr<<"Problem executing command: "<<std::string(argv[0])<<"(...)"<<std::endl; return 0;}
  return 1;
}
DLL_PUBLIC int  GDL_CDECL IDL_RuntimeExec(char *file){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) Warning("IDL_RuntimeExec function not allowed in GDL.");return 0;}
DLL_PUBLIC void  GDL_CDECL IDL_Runtime(EXPORT_INIT_DATA_OPTIONS_T options, int *argc, char *argv[], char *file){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__) GDL_WillReturnAfterCleaning("IDL_Runtime function not allowed in GDL.");}

#define DOCASE_TAG(type, gdltype, tagname, pardim)\
 case type: { gdltype entry(pardim); stru_desc->AddTag(std::string(tagname), &entry);} break;

	DStructDesc* GDL_GetStructDesc(std::string name, EXPORT_STRUCT_TAG_DEF *tags) {
		int ntags = 0;
		if (tags) {
			while (tags[ntags++].name != NULL) {};
		ntags--;
		}
		DStructDesc* stru_desc = new DStructDesc(name);
		for (int itag = 0; itag < ntags; ++itag) {
			EXPORT_STRUCT_TAG_DEF def = tags[itag];
			dimension *dim;
			if (def.dims == NULL) {
				dim = new dimension();
			} else {
				EXPORT_LONG64 ndim = def.dims[0];
				dim = new dimension(&(def.dims[1]), ndim);
			}
			if (def.type == NULL) { /*If this field is NULL, it indicates that we should search for a structure of the given name and fill in the pointer to its structure definition. */
				if (def.name == NULL) GDL_WillReturnAfterCleaning("IDL_MakeStruct(): no name for inherited structure!");
				std::string passed_name = std::string(def.name);
				assert(passed_name != "$truct"); // named struct
				passed_name = StrUpCase(passed_name);
				if (passed_name == "IDL_OBJECT") passed_name = GDL_OBJECT_NAME; // replacement also done in GDLParser
				if (passed_name == "IDL_CONTAINER") passed_name = GDL_CONTAINER_NAME; // replacement also done in GDLParser
				DStructDesc* desc = DInterpreter::CallStackBack()->Interpreter()->GetStruct(passed_name, DInterpreter::CallStackBack()->CallingNode()); //will throw if does not exist.
				stru_desc->AddTag(std::string(def.name), new DStructGDL(desc, *dim));
				continue;
			}
			/* This may be either a pointer to another structure definition, or a simple IDL type code (GDL_TYP_*) cast to void (e.g. (void *) GDL_TYP_BYTE)*/
			if ((size_t) def.type > EXPORT_MAX_TYPE) { /* a structure ptr */
//				 EXPORT_StructDefPtr s= (EXPORT_StructDefPtr) def.type;
//				 stru_desc->AddTag(std::string(def.name), GDL_GetStructDesc(NULL, s.));
			} else {
				switch ((SizeT) def.type) {
						DOCASE_TAG(GDL_TYP_BYTE, SpDByte, def.name, *dim)
						DOCASE_TAG(GDL_TYP_INT, SpDInt, def.name, *dim)
						DOCASE_TAG(GDL_TYP_LONG, SpDLong, def.name, *dim)
						DOCASE_TAG(GDL_TYP_FLOAT, SpDFloat, def.name, *dim)
						DOCASE_TAG(GDL_TYP_DOUBLE, SpDDouble, def.name, *dim)
						DOCASE_TAG(GDL_TYP_COMPLEX, SpDComplex, def.name, *dim)
						DOCASE_TAG(GDL_TYP_DCOMPLEX, SpDComplexDbl, def.name, *dim)
						DOCASE_TAG(GDL_TYP_STRING, SpDString, def.name, *dim)
						DOCASE_TAG(GDL_TYP_PTR, DPtrGDL, def.name, *dim)
						DOCASE_TAG(GDL_TYP_OBJREF, DObjGDL, def.name, *dim)
						DOCASE_TAG(GDL_TYP_UINT, SpDUInt, def.name, *dim)
						DOCASE_TAG(GDL_TYP_ULONG, SpDULong, def.name, *dim)
						DOCASE_TAG(GDL_TYP_ULONG64, SpDULong64, def.name, *dim)
						DOCASE_TAG(GDL_TYP_LONG64, SpDLong64, def.name, *dim)
					default: GDL_WillReturnAfterCleaning("GDL_GetStructDesc(EXPORT_STRUCT_TAG_DEF): bad case.");
				}
			}
		}
		return stru_desc;
	}
#undef DOCASE_TAG

#define ADJUST_ELEMENT_OFFSET(x) {EXPORT_MEMINT l=x;\
EXPORT_MEMINT excess=newStruct->length % l;\
if (excess != 0) {\
EXPORT_MEMINT pad=l-excess;\
newStruct->length+=pad;\
}}
// NB: we use RCOUNT to store the padding of each struct (probably not clever enough to do without that). Should not be aproblem as GDL does not use it, and users probably as well.
DLL_PUBLIC EXPORT_StructDefPtr  GDL_CDECL IDL_MakeStruct(char *name, EXPORT_STRUCT_TAG_DEF *tags) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
				int ntags = 0;
if (tags) {
		//count tags (?)
	while (tags[ntags++].name != NULL) {
	};
	ntags--;
}
if (ntags == 0) GDL_WillReturnAfterCleaning("IDL_MakeStruct(): structure creation needs tags!");
	// create structure with extended size after (see https://stackoverflow.com/questions/6390331/why-use-array-size-1-instead-of-pointer)
	EXPORT_STRUCTURE *newStruct = (EXPORT_STRUCTURE *) MyCallocDestroyedOnExit(1, sizeof (EXPORT_STRUCTURE) + ntags * sizeof (EXPORT_TAGDEF));
	if (name) {
		EXPORT_IDENT *iid = (EXPORT_IDENT*) MyCallocDestroyedOnExit(1, sizeof (EXPORT_IDENT));
		newStruct->id = iid;
		iid->hash = NULL;
		iid->name = name;
		iid->len = strlen(name);
	} else newStruct->id = NULL;
	newStruct->flags = 0;
	newStruct->ntags = ntags;
	newStruct->length = 0; //start at 0
	newStruct->data_length = 0; 
	newStruct->contains_string = false;
	struct _idlgdl_ident *memhash = NULL;
	// compute alignment, this is the difference between length and data_length
	//"on modern machines each data item must usually be ``self-aligned'', 
	// beginning on an address that is a multiple of its type size. 
	// Thus, 32-bit types must begin on a 32-bit boundary, 16-bit types on a 16-bit boundary, 
	// 8-bit types may begin anywhere, struct/array/union types have the alignment of their most restrictive member. 
	int finalPad=0;
	for (int itag = 0; itag < ntags; ++itag) {
		EXPORT_STRUCT_TAG_DEF def = tags[itag];
		EXPORT_IDENT *tagid = (EXPORT_IDENT*) MyCallocDestroyedOnExit(1, sizeof (EXPORT_IDENT));
		newStruct->tags[itag].id = tagid;
		tagid->name = def.name;
		tagid->len = strlen(def.name);
		tagid->hash = memhash;
//		memhash = tagid; //chain
		void* thetypePtr = def.type;
		if (thetypePtr == NULL) {    /*If this field is NULL, it indicates that we should search for a structure of the given name and fill in the pointer to its structure definition. */
		  if (def.name == NULL) GDL_WillReturnAfterCleaning("IDL_MakeStruct(): no name for inherited structure!");
		  std::string passed_name = std::string(def.name);
		  assert(passed_name != "$truct") ; // named struct
		  passed_name = StrUpCase(passed_name);
		  if( passed_name == "IDL_OBJECT") passed_name = GDL_OBJECT_NAME; // replacement also done in GDLParser
		  if( passed_name == "IDL_CONTAINER") passed_name = GDL_CONTAINER_NAME; // replacement also done in GDLParser
		  DStructDesc* desc = DInterpreter::CallStackBack()->Interpreter()->GetStruct(passed_name, DInterpreter::CallStackBack()->CallingNode()); //will throw if does not exist.
	      DStructGDL* tmpstruct=new DStructGDL(desc,dimension());
		  Guard<DStructGDL> g(tmpstruct);
		  thetypePtr=(void*) GDL_Make_EXPORT_STRUCT_TAG_DEF(tmpstruct);
		  //structure found, pursue
		} 
		/* This may be either a pointer to another structure definition, or a simple IDL type code (GDL_TYP_*) cast to void (e.g. (void *) GDL_TYP_BYTE)*/
		if ((size_t) thetypePtr > EXPORT_MAX_TYPE) { /* a structure ptr */
			newStruct->tags[itag].var.type = GDL_TYP_STRUCT;
			newStruct->tags[itag].var.flags |= (GDL_V_STRUCT|GDL_V_ARR);
			newStruct->tags[itag].var.value.s.sdef=(EXPORT_StructDefPtr)thetypePtr;
			EXPORT_LONG64 strulen=((EXPORT_StructDefPtr) (thetypePtr))->length;
			EXPORT_LONG64 strudata_len=((EXPORT_StructDefPtr) (thetypePtr))->data_length;
			int alignment=((EXPORT_StructDefPtr) (thetypePtr))->rcount;
			if (finalPad < alignment) finalPad=alignment;
			ADJUST_ELEMENT_OFFSET(alignment) //add to current struct length the necessary pad if needed 
			newStruct->tags[itag].offset = newStruct->length; //current struct length at this point of creation: start of tag
			if (def.dims == NULL) {
				newStruct->length += strulen;
				newStruct->data_length += strudata_len;
			} else {
				newStruct->tags[itag].var.value.s.arr = NewExportArray();
				EXPORT_LONG64 ndim = def.dims[0];
				newStruct->tags[itag].var.value.s.arr->n_dim = ndim;
				size_t l = 1;
				for (auto i = 1; i < ndim + 1; ++i) {
					newStruct->tags[itag].var.value.s.arr->dim[i - 1] = def.dims[i];
					if (def.dims[i] != 0) l *= def.dims[i]; 
				}
				newStruct->tags[itag].var.value.s.arr->arr_len = l * strulen; 
				newStruct->tags[itag].var.value.s.arr->elt_len = strulen;
				newStruct->tags[itag].var.value.s.arr->n_elts = l;
				newStruct->data_length += l * strudata_len;
				newStruct->length += l * strulen;
			}
		} else {
			size_t thetype = (size_t) thetypePtr;
			int realType = thetype;
			int alignment=GDL_TypeAlignment[realType];
			if (finalPad < alignment) finalPad=alignment;
			ADJUST_ELEMENT_OFFSET(alignment) //add to current struct length the necessary pad if needed 
			newStruct->tags[itag].offset = newStruct->length; //current struct length at this point of creation: start of tag
			if (realType == GDL_TYP_STRING || realType == GDL_TYP_PTR) newStruct->contains_string = true;
			newStruct->tags[itag].var.type = realType;
			if (def.dims == NULL) {
				newStruct->data_length += IDL_TypeSize[realType];
				newStruct->length += IDL_TypeSize[realType]; 
			} else {
				newStruct->tags[itag].var.flags |= GDL_V_ARR;
				newStruct->tags[itag].var.value.arr = NewExportArray();
				EXPORT_LONG64 ndim = def.dims[0];
				newStruct->tags[itag].var.value.arr->n_dim = ndim;
				size_t l = 1;
				for (auto i = 1; i < ndim + 1; ++i) {
					newStruct->tags[itag].var.value.arr->dim[i - 1] = def.dims[i];
					if (def.dims[i] != 0) l *= def.dims[i];
				}
				newStruct->tags[itag].var.value.arr->arr_len = l * IDL_TypeSize[realType];
				newStruct->tags[itag].var.value.arr->elt_len = IDL_TypeSize[realType];
				newStruct->tags[itag].var.value.arr->n_elts = l;
				newStruct->data_length += newStruct->tags[itag].var.value.arr->arr_len;
				newStruct->length += newStruct->tags[itag].var.value.arr->arr_len;
			}
		}
	}
	// add pad (finalPad) to struct size if necessary
    if (finalPad > 0) ADJUST_ELEMENT_OFFSET(finalPad) //add to current struct length the necessary pad if needed 

	newStruct->rcount = finalPad;
	newStruct->object = NULL;
	if (name) { //define named struct at GDL level. probably easier to do otherwise without creating an IDL, then a GDL, struct, but this limits the 'new' code at a minimum.
		std::string sname(name);
		sname= StrUpCase(sname);
		structList.push_back(GDL_GetStructDesc(sname,tags));
	}
	return newStruct;
	}
#undef ADJUST_ELEMENT_OFFSET
DLL_PUBLIC EXPORT_MEMINT  GDL_CDECL IDL_StructTagInfoByName(EXPORT_StructDefPtr sdef, char *name, int msg_action, EXPORT_VPTR *var) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		int l = strlen(name);
		for (auto i = 0; i < sdef->ntags; ++i) {
			if (sdef->tags[i].id->len == l && strncmp(name, sdef->tags[i].id->name, l)) {
			if (var) *var=&(sdef->tags[i].var);
			}
			return sdef->tags[i].offset;
		}
		char* mess=(char*)calloc(256,1);
		strncat(mess,"Tag name ",10);strncat(mess,name,200);strncat(mess," is undefined for structure ",29);
		if (sdef->id!=NULL && sdef->id->name !=NULL) strcat(mess,sdef->id->name); else strcat(mess,"<Anonymous>");
		IDL_Message(EXPORT_M_GENERIC, msg_action, mess);
		MyFree(mess);
		return 0;
	}

DLL_PUBLIC EXPORT_MEMINT  GDL_CDECL IDL_StructTagInfoByIndex(EXPORT_StructDefPtr   sdef, int index, int msg_action, EXPORT_VPTR *var){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		if (sdef->ntags > index) {
			 if (var) *var=&(sdef->tags[index].var);
			return sdef->tags[index].offset;
		}
		char* mess=(char*)calloc(256,1);
		strncat(mess,"Tag number ",12);snprintf(mess,64,"%d",index); strncat(mess," is undefined for structure ",29);
		if (sdef->id!=NULL && sdef->id->name !=NULL) strcat(mess,sdef->id->name); else strcat(mess,"<Anonymous>");
		IDL_Message(EXPORT_M_GENERIC, msg_action, mess);
		MyFree(mess);
		return 0;
	}

DLL_PUBLIC char * GDL_CDECL IDL_StructTagNameByIndex(EXPORT_StructDefPtr sdef, int index, int msg_action, char **struct_name){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		if (sdef->ntags > index) {
			if (struct_name != NULL) {if  (sdef->id!=NULL && sdef->id->name !=NULL) *struct_name=sdef->id->name; else *struct_name=(char*)"<Anonymous>";}
			return sdef->tags[index].id->name;
		}
		char* mess=(char*)calloc(256,1);
		strncat(mess,"Tag number ",12);snprintf(mess,64,"%d",index); strncat(mess," is undefined for structure ",29);
		if (sdef->id!=NULL && sdef->id->name !=NULL) strcat(mess,sdef->id->name); else strcat(mess,"<Anonymous>");
		IDL_Message(EXPORT_M_GENERIC, msg_action, mess);
		MyFree(mess);
		return NULL;
}

EXPORT_STRUCT_TAG_DEF* GDL_Make_EXPORT_STRUCT_TAG_DEF(DStructGDL* gdlstruct){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	DStructDesc* desc=gdlstruct->Desc();
	SizeT nTags=desc->NTags();
	SizeT l=sizeof(EXPORT_STRUCT_TAG_DEF);
	EXPORT_STRUCT_TAG_DEF* tagarray=(EXPORT_STRUCT_TAG_DEF*)MyCallocDestroyedOnExit((nTags+1),l);
	EXPORT_STRUCT_TAG_DEF** ret=(EXPORT_STRUCT_TAG_DEF**)MyMallocDestroyedOnExit((nTags+1)*sizeof(char*));
	for (SizeT i=0; i< nTags+1; ++i) ret[i]=(EXPORT_STRUCT_TAG_DEF*)((SizeT)tagarray+i*l);
	for (int i=0; i< nTags+1; ++i) {
		EXPORT_STRUCT_TAG_DEF* sample=ret[i];
		if (i == nTags) {
			sample->name=NULL;
			break;
		}
		BaseGDL* entry=gdlstruct->GetTag(i);
		int rank=entry->Rank();		
		EXPORT_MEMINT* dims=(EXPORT_MEMINT*)MyMallocDestroyedOnExit((rank+1)*sizeof(EXPORT_MEMINT*));
		dims[0]=rank; for (auto n=0; n<rank; ++n) dims[n+1]=entry->Dim(n);
		sample->dims=dims;
		sample->name=(char*)(desc->TagName(i).c_str());
		sample->type=(void*)(entry->Type()); 
		sample->flags=0;
		if ((SizeT)(sample->type) == GDL_TYP_STRUCT) {
		 if (!desc->IsUnnamed()) {
			 sample->type=NULL; //will look for a named struct
		 } else { //reentrant call for making a substructure tag def
			 DStructGDL* substruct=static_cast<DStructGDL*>(entry);
			 EXPORT_STRUCT_TAG_DEF* tags=GDL_Make_EXPORT_STRUCT_TAG_DEF(substruct);
			 sample->type=(void*)IDL_MakeStruct(NULL, tags);
		 }
		 if (desc->GetNumberOfParents() > 0 ) sample->flags=1;
		}
	}
	return tagarray;
}



#include <signal.h>

#if defined(SIGWINDOW) && !defined(SIGWINCH)
#define SIGWINCH SIGWINDOW
#endif

typedef struct {
#ifdef linux
  unsigned long set[_SIGSET_NWORDS];
#else
  double set[4];
#endif
} EXPORT_SignalSet_t;
typedef void (* EXPORT_SignalHandler_t)(int signo);

//void GDL_CDECL IDL_SignalSetInit(EXPORT_SignalSet_t *set, int signo);
//void GDL_CDECL IDL_SignalSetAdd(EXPORT_SignalSet_t *set, int signo);
//void GDL_CDECL IDL_SignalSetDel(EXPORT_SignalSet_t *set, int signo);
//int GDL_CDECL IDL_SignalSetIsMember(EXPORT_SignalSet_t *set, int signo);
//void GDL_CDECL IDL_SignalMaskGet(EXPORT_SignalSet_t *set);
//void GDL_CDECL IDL_SignalMaskSet(EXPORT_SignalSet_t *set, EXPORT_SignalSet_t *oset);
//void GDL_CDECL IDL_SignalMaskBlock(EXPORT_SignalSet_t *set,	EXPORT_SignalSet_t *oset);
//void GDL_CDECL IDL_SignalBlock(int signo, EXPORT_SignalSet_t *oset);
//void GDL_CDECL IDL_SignalSuspend(EXPORT_SignalSet_t *set);
//should mimic what IDL does
#if defined (_WIN32) && !defined(__CYGWIN__)
int GDL_CDECL IDL_SignalRegister(int signo, EXPORT_SignalHandler_t func, int msg_action){
	Warning("Signal "+i2s(signo)+" not allowed. Use alternative API.");
		return 0;
	}

	int GDL_CDECL IDL_SignalUnegister(int signo, EXPORT_SignalHandler_t func, int msg_action) {
		Warning("Signal " + i2s(signo) + " not allowed. Use alternative API.");
		return 0;
	}
#else
int GDL_CDECL IDL_SignalRegister(int signo, EXPORT_SignalHandler_t func, int msg_action){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	if (signo == SIGFPE || signo == SIGALRM || signo == SIGCHLD  ) {
		Warning("Signal "+i2s(signo)+" not allowed. Use alternative API.");
		return 0;
	}
	sig_t ret=signal(signo,func);
	if (ret==SIG_ERR) {
			Warning("Attempt to set signal failed:"+std::string(strerror(errno)));
			return 0;
	}
	return 1;
}
//not exactly what IDL does
int GDL_CDECL IDL_SignalUnregister(int signo, EXPORT_SignalHandler_t func, int msg_action){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	if (signo == SIGFPE || signo == SIGALRM  || signo == SIGCHLD ) {
		Warning("Signal "+i2s(signo)+" not allowed. Use alternative API.");
		return 0;
	}
	sig_t ret=signal(signo,SIG_DFL);
	return 1;
	}

	DLL_PUBLIC int GDL_CDECL IDL_SysRtnAdd(EXPORT_SYSFUN_DEF2 *defs, int is_function, int cnt) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		if (is_function) {
			for (auto i = 0; i < cnt; ++i) {
				const char* name = (const char*) defs[i].name;
				void* addr = (void*) (defs[i].funct_addr.fun);
//				printf("0x%x %s %u %u %u %u\n",addr, name, defs[i].arg_min, defs[i].arg_max, defs[i].flags, defs[i].extra);
//				if (SysFunDefinitions.count(name)) {
//					std::cerr << name << ": " << SysFunDefinitions[name] << std::endl;
//					return 1;
//				} //already done
				SysFunDefinitions[name] = addr;
				if (defs[i].flags & EXPORT_SYSFUN_DEF_F_OBSOLETE) Warning("Obsolete function "+std::string(defs[i].name)+" found in loaded DLM.");
			}
		} else {
			for (auto i = 0; i < cnt; ++i) {
				const char* name = (const char*) defs[i].name;
				void* addr = (void*) (defs[i].funct_addr.fun);
//				printf("0x%x %s %u %u %u %u\n", addr, name, defs[i].arg_min, defs[i].arg_max, defs[i].flags, defs[i].extra);
//				if (SysProDefinitions.count(name)) {
//					std::cerr << name << ": " << SysProDefinitions[name] << std::endl;
//					return 1;
//				}//already done
				SysProDefinitions[name] = addr;
				if (defs[i].flags & EXPORT_SYSFUN_DEF_F_OBSOLETE) Warning("Obsolete function " + std::string(defs[i].name) + " found in loaded DLM.");
			}
		}
		return 1;
	}
	
DLL_PUBLIC EXPORT_VPTR GDL_CDECL IDL_VarTypeConvert(EXPORT_VPTR v, GDL_REGISTER int type) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		return IDL_BasicTypeConversion(1, &v, type);
	}
	
extern void GDL_CDECL IDL_KWCleanup(int fcn){		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
} //DO EXACTLY NOTHING!
DLL_PUBLIC EXPORT_VPTR GDL_CDECL IDL_conj(EXPORT_VPTR v){		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)

	checkOK(v);
	EXPORT_VPTR temp=IDL_BasicTypeConversion(1, &v, GDL_TYP_COMPLEX);
	if (ISARRAY(temp)) {
		EXPORT_COMPLEX* c=(EXPORT_COMPLEX*)temp->value.arr->data;
		for (int i=0; i< temp->value.arr->n_elts; ++i){ c[i].i *= -1;}
	} else { temp->value.cmp.i *=-1;}
	return temp;
	}

	DLL_PUBLIC EXPORT_VPTR GDL_CDECL IDL_transpose(int argc, EXPORT_VPTR *argv) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)

		GDL_ENSURE_ARRAY(argv[0]);
	    BaseGDL* tmp = VPTR_ToGDL(argv[0]);
		if (argc > 1) {
			checkOK(argv[1]);
			GDL_ENSURE_ARRAY(argv[1]);
			BaseGDL* thePerm = VPTR_ToGDL(argv[1])->Convert2(GDL_UINT);
				DUInt perm[GDL_MAX_ARRAY_DIM] = {0};
				for (int i = 0; i < argv[1]->value.arr->n_elts; ++i) {
					perm[i] = (*static_cast<DUIntGDL*> (thePerm))[i];
					if (perm[i] < 0 || perm[i] > argv[0]->value.arr->n_dim - 1) GDL_WillReturnAfterCleaning("Value of " + std::string(IDL_VarName(argv[1])) + " is out of allowed range.");
				}
				return GDL_ToVPTR(tmp->Transpose(perm,argv[1]->value.arr->n_elts));
		}
		return GDL_ToVPTR(tmp->Transpose(NULL));
	}

#endif

  EXPORT_VPTR findGDLVar(std::string varName, bool acceptNew, bool doMain) {		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)

	EnvStackT& callStack = DInterpreter::CallStack();
    DLong curlevnum = callStack.size();
    // normal name
    varName = StrUpCase(varName);
	SizeT desiredLevel=(doMain)?0:curlevnum-1; //either current level or level zero
		DSubUD* pro = static_cast<DSubUD*> (callStack[desiredLevel]->GetPro());
		SizeT nVar = pro->Size(); // # var in GDL for desired level
		int nKey = pro->NKey();

		int xI = pro->FindVar(varName);
		if (xI != -1) { //exists
			BaseGDL*& par = callStack[desiredLevel]->GetTheKW(xI);
			EXPORT_VPTR v = GDL_ToVPTR(par, true);
			varInfo info={v,&par,true,desiredLevel,false}; //exists at desiredlevel
			PassedVariables.push_back(info);
			return v;
		} else if (acceptNew) {
			SizeT u = pro->AddVar(varName);
			SizeT s = callStack[desiredLevel]->AddEnv();
			BaseGDL*& par = ((EnvT*) (callStack[desiredLevel]))->GetPar(s - nKey);
			EXPORT_VPTR v = GDL_ToVPTR(par, true);
			varInfo info={v,&par,true,desiredLevel,true};
			PassedVariables.push_back(info);
			return v;
		} 
    return NULL; // compiler shut-up
  }
  DLL_PUBLIC void GDL_CDECL IDL_MakeStructInternal(char* name,char,EXPORT_STRUCT_TAG_DEF* tags){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
 #ifdef GDL_DEBUG
  std::cerr<<"IDL_MakeStructInternal, name= "<<name<<std::endl;
 #endif
   IDL_MakeStruct(name,tags);
  }
#ifndef _WIN32
#include <paths.h>
#endif
  DLL_PUBLIC char* GDL_CDECL IDL_FilePathFromRoot(int flags, char *pathbuf, char
        *root, char *file, char *ext,  int nsubdir, char **subdir){		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		static const char* const sep=(char*)lib::PathSeparator().c_str();
		static const char* dot=(char*)".";
		pathbuf[0]=0;
		strcat(pathbuf, root); 
		strcat (pathbuf, sep);
		for (int i=0; i< nsubdir; ++i) {
			strcat(pathbuf, subdir[i]);
		    strcat (pathbuf, sep);
		}
		strcat (pathbuf, file);
		if (flags == 0) strcat (pathbuf, dot); //???
		strcat (pathbuf, ext);
		return pathbuf;
  }

  DLL_PUBLIC void GDL_CDECL IDL_FilePathGetTmpDir(char *path){		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
          char* where=getenv("GDL_TMPDIR");
          if (where != NULL) {
			  strcpy(path,where);
			  return;
		  }
          where=getenv("IDL_TMPDIR");
          if (where != NULL) {
			  strcpy(path,where);
			  return;
		  }
#ifdef _WIN32
            WCHAR tmpBuf[MAX_PATH];
            GetTempPathW(MAX_PATH, tmpBuf);
            WideCharToMultiByte(CP_ACP, 0, tmpBuf, MAX_PATH, path, MAX_PATH, NULL, NULL);
#else
            strcpy(path, (char*) _PATH_TMP);
#endif
	}
typedef struct {
  EXPORT_MEMINT x;
  char *name;
} EXPORT_TEST;
//undocumented:

	DLL_PUBLIC void GDL_CDECL IDL_ObjInsertDef(EXPORT_TEST *s) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
//#ifdef GDL_DEBUG
		std::cerr<<"IDL_ObjInsertDef, name= "<<s->name<<std::endl;
//#endif
	}
	
	DLL_PUBLIC EXPORT_VPTR GDL_CDECL IDL_ObjReference(EXPORT_VPTR v) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		EXPORT_HVID hash_id=v->value.hvid;
		return &(IDL_HeapVarHashFind(hash_id)->var);
	}
DLL_PUBLIC  EXPORT_HEAP_VPTR GDL_CDECL IDL_HeapVarHashFind(EXPORT_HVID hash_id){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	EXPORT_HEAP_VPTR pointed;
try {pointed=MimickedHeap.at(hash_id);} catch (const std::out_of_range& oor) {GDL_WillThrowAfterCleaning(__func__,"Invalid Heap.");}
	return pointed;
}
DLL_PUBLIC UCHAR* IDL_ObjGetInstanceData(EXPORT_VPTR v){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		return v->value.s.arr->data;}

#include "export_notsupported.hpp"
}
#endif
