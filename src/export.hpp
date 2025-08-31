#ifndef export_is_defined
#define export_is_defined

#if defined(TRACE_ROUTINE)
#undef TRACE_ROUTINE
#endif

//#define TRACE_OPCALLS

#if defined(TRACE_OPCALLS)
#define TRACE_ROUTINE(func,file,line) std::cout << func << "\t" << file << "\t" << line << std::endl;
#else
#define TRACE_ROUTINE(func,file,line)
#endif

#include "gdl_export.h"
#include "datatypes.hpp"
#include "GDLInterpreter.hpp"
#include "terminfo.hpp"

#define GDL_TYP_COMPLEXDBL GDL_TYP_DCOMPLEX

static std::map<const char*,void*> SysFunDefinitions; 
static std::map<const char*,void*> SysProDefinitions; 

// list of memory (strings...) to be released when GDL_FreeResources() is called.
// If each call is ended by freeing the resources, this list does not need to be private to each CallDllFunc/CallDllpro I guess.
static std::vector<void*> FreeAtEnd;
void FreeIntermediateMemory(){	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
for (std::vector<void*>::iterator it = FreeAtEnd.begin(); it != FreeAtEnd.end(); ++it) free(*it);
	FreeAtEnd.clear();
}
void* MyMalloc(size_t size){	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	void* ret=malloc(size);
	FreeAtEnd.push_back(ret);
	return ret;
}
static std::vector<EXPORT_VPTR> FreeList;
static std::vector<EXPORT_VPTR> FreeKwList;
static std::vector<std::pair<EXPORT_VPTR,std::string> > ExportedNamesList;
typedef struct {
  const char* name;
  BaseGDL* varptr; // pointer to some externally produced var if out=true
  EXPORT_VPTR out;
  UCHAR type;
  UCHAR readonly; // no associated variable
} GDL_KEYWORDS_LIST;

typedef struct {
  int npassed;
  GDL_KEYWORDS_LIST* passed;
} GDL_PASS_KEYWORDS_LIST;

extern "C" {

EXPORT_STRUCT_TAG_DEF* GDL_Make_EXPORT_STRUCT_TAG_DEF(DStructGDL* gdlstruct);
EXPORT_StructDefPtr  GDL_CDECL IDL_MakeStruct(char *name, EXPORT_STRUCT_TAG_DEF *tags);
EXPORT_VPTR  GDL_CDECL IDL_ImportArray(int n_dim, EXPORT_MEMINT dim[], int type, UCHAR *data, EXPORT_ARRAY_FREE_CB free_cb,  EXPORT_StructDefPtr s);

DLL_PUBLIC char * GDL_CDECL IDL_VarName(EXPORT_VPTR v);

DLL_PUBLIC void  GDL_CDECL IDL_Deltmp(GDL_REGISTER EXPORT_VPTR v) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (v->flags & GDL_V_TEMP) {
		if (v->flags & GDL_V_DYNAMIC) {
			if (v->value.arr != NULL) {
				if (v->value.arr->free_cb != NULL) v->value.arr->free_cb(v->value.arr->data); else free(v->value.arr->data);
				free(v->value.arr);
				v->value.arr=NULL;
				if (v->type == GDL_TYP_STRUCT) {free(v->value.s.sdef);v->value.s.sdef=NULL;}
			} else if (v->type == GDL_TYP_STRING) {
				if (v->value.str.slen != 0 && v->value.str.stype==1) {free(v->value.str.s);v->value.str.s=NULL;v->value.str.slen=0;v->value.str.stype=0;}
			}
		}
	}
	//TEMPORARY EXPORT_VARIABLE itself will be destroyed at exit from function/procedure by GDL_FreeResources()
}

//Used by GDL_FreeResources(), like IDL_Deltmp, but deletes the EXPORT_VARIABLE itself
DLL_PUBLIC void GDL_CDECL IDL_DeleteDescriptors(GDL_REGISTER EXPORT_VPTR v) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		if (v->flags & GDL_V_DYNAMIC) {
			if (v->flags & GDL_V_ARR ) {
				if (v->value.arr->free_cb != NULL) v->value.arr->free_cb(v->value.arr->data);
			} //else if (v->type == GDL_TYP_STRING) if (v->value.str.slen != 0) free(v->value.str.s);
		}
}

DLL_PUBLIC void  GDL_CDECL IDL_Freetmp(GDL_REGISTER EXPORT_VPTR v) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	IDL_Deltmp(v);
}
}

void GDL_FreeResources() {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	bool message = true;
    FreeIntermediateMemory();
    for (std::vector<EXPORT_VPTR>::iterator it = FreeList.begin(); it != FreeList.end(); ++it) {
		if ((*it)->flags & GDL_V_TEMP != 0) continue;
		if ((*it)->flags & GDL_V_DYNAMIC) {
			if (message) { message=false;
				Message("Temporary variables are still checked out - cleaning up...");
			}
			char* varname=IDL_VarName((*it));
			if (strlen(varname) > 0) fprintf(stderr,"%s\n",varname);
			IDL_DeleteDescriptors(*it);
//			IDL_Deltmp(*it);
		}
	}
	FreeList.clear();
}

void GDL_WillThrowAfterCleaning(const std::string &s) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	GDL_FreeResources();
   DInterpreter::CallStackBack()->Throw(s);
}

inline void checkOK(EXPORT_VPTR v) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (v->type == GDL_TYP_UNDEF) GDL_WillThrowAfterCleaning("Variable is undefined: <UNDEFINED>.");
}

inline EXPORT_VPTR NewTMPVPTR(UCHAR flag=0, EXPORT_StructDefPtr structdefptr=NULL) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	EXPORT_VPTR ret = new EXPORT_VARIABLE();
	ret->type = GDL_TYP_UNDEF;
	ret->flags2 = 0;
	memset(&(ret->value),0,sizeof(EXPORT_ALLTYPES));
	ret->flags = GDL_V_TEMP | flag;
	if (flag & GDL_V_STRUCT  ) {
		ret->type = GDL_TYP_STRUCT;
		ret->flags |= GDL_V_DYNAMIC;
		ret->flags |= GDL_V_ARR;
		ret->value.arr= new EXPORT_ARRAY();
		ret->value.arr->flags = GDL_A_NO_GUARD;
		ret->value.s.sdef = structdefptr;
	} else if (flag & GDL_V_ARR) {
		ret->value.arr = new EXPORT_ARRAY();
		ret->flags |= GDL_V_DYNAMIC;
		ret->flags |= GDL_V_ARR;
		ret->value.arr->flags = GDL_A_NO_GUARD;
	}
	return ret;
}
inline EXPORT_VPTR NewTMPVPTRARRAY() {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	return NewTMPVPTR(GDL_V_ARR);
}
inline EXPORT_VPTR NewTMPVPTRARRAYWithCB(EXPORT_ARRAY_FREE_CB free_cb) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	EXPORT_VPTR v=NewTMPVPTR(GDL_V_ARR);
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
inline EXPORT_VPTR NewTMPVPTRFromGDL(bool kw=false, bool tempo=true) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
EXPORT_VPTR ret=NewTMPVPTR(); if(kw) FreeKwList.push_back(ret); else if (tempo) FreeList.push_back(ret);
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
		offset = (SizeT) calloc(1,toAlloc);
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
			void* stringdescPtrs=malloc(nEl*sizeof(EXPORT_STRING*));
			EXPORT_STRING** p=(EXPORT_STRING**)stringdescPtrs;
			for (SizeT i=0; i< nEl; ++i) p[i]=(EXPORT_STRING*)(offset+(i*sizeof(EXPORT_STRING)));
			for (auto i=0; i< nEl; ++i) {
				p[i]->slen = ((*gdlstr)[i]).size();
				p[i]->stype=1;
				if( p[i]->slen > 0) {
					p[i]->s = (char*) malloc(p[i]->slen + 1);
				    strncpy(p[i]->s, (*gdlstr)[i].c_str(), p[i]->slen + 1);
				} else p[i]->s=0;
			}
			free(stringdescPtrs);
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

EXPORT_VPTR GDL_ToVPTR(BaseGDL* var, bool tempo=false, bool is_kw=false) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
    EXPORT_VPTR v;
    if (var == NULL) {
      v=NewTMPVPTRFromGDL(is_kw, tempo);
      v->type=GDL_TYP_UNDEF;
      return v;
    }
    if (var == NullGDL::GetSingleInstance()) {
      v=NewTMPVPTRFromGDL(is_kw, tempo);
      v->type=GDL_TYP_LONG; // any type, as the NULL flags is checked.
      v->flags|=GDL_V_NULL;
      return v;
    }
	if (var->Type() == GDL_STRUCT) {
	  DStructGDL *gdlstruct=static_cast<DStructGDL*>(var);
	  EXPORT_STRUCT_TAG_DEF* exportStructTagDef=GDL_Make_EXPORT_STRUCT_TAG_DEF(gdlstruct);
	  EXPORT_StructDefPtr s;
	  if (gdlstruct->Desc()->IsUnnamed()) {
		  s=IDL_MakeStruct(NULL, exportStructTagDef);
	  } else {
		  s=IDL_MakeStruct((char*)(gdlstruct->Desc()->Name().c_str()), exportStructTagDef);
	  }
	  SizeT nelts=gdlstruct->N_Elements();
	  SizeT arr=0;
	  SizeT ret=GdlStructDump(gdlstruct, arr, s);
	  v = IDL_ImportArray(1, &nelts, GDL_TYP_STRUCT, (UCHAR*)ret , 0, s);
	  return v;
    }
    v=NewTMPVPTRFromGDL(is_kw, tempo);
    if (var->N_Elements() == 1) {
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
		  v->value.str.slen=s.size();
		  if (v->value.str.slen > 0) {
			  v->value.str.s=(char*) malloc(v->value.str.slen+1);
		      strncpy(v->value.str.s,s.c_str(),v->value.str.slen+1);
		  }
          break;
        }
		case GDL_PTR:
        {
          v->type = GDL_TYP_PTR;
		  v->value.ptrint=(*static_cast<DPtrGDL*>(var))[0];
          break;
        }
		  default: GDL_WillThrowAfterCleaning("GDL_ToVPTR: unsupported case.");
      }
    } else {
      v->flags |= (GDL_V_ARR | GDL_V_DYNAMIC);
      EXPORT_ARRAY* arraydescr=new EXPORT_ARRAY();
      arraydescr->arr_len = var->NBytes();
      arraydescr->data = (UCHAR*) (var->DataAddr());
      for (int i = 0; i < var->Rank(); ++i) arraydescr->dim[i] = var->Dim(i);
      arraydescr->n_dim = var->Rank();
      arraydescr->n_elts = var->N_Elements();
      arraydescr->offset = 0;
      v->value.arr = arraydescr;
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
			SizeT nEl=arraydescr->n_elts;
			void* allstringdescr=malloc(nEl*sizeof(EXPORT_STRING));
			memset(allstringdescr,0,nEl*sizeof(EXPORT_STRING));
			arraydescr->data = (UCHAR*) (allstringdescr);
			void* stringdescPtrs=malloc(nEl*sizeof(EXPORT_STRING*));
			EXPORT_STRING** p=(EXPORT_STRING**)stringdescPtrs;
			for (SizeT i=0; i< nEl; ++i) p[i]=(EXPORT_STRING*)((SizeT)allstringdescr+(i*sizeof(EXPORT_STRING)));
			DStringGDL* gdlstr=(DStringGDL*)var;
			for (auto i=0; i< nEl; ++i) {
				p[i]->slen = ((*gdlstr)[i]).size();
				if (p[i]->slen > 0) {
					p[i]->s = (char*) malloc(p[i]->slen + 1);
				    strncpy(p[i]->s, (*gdlstr)[i].c_str(), p[i]->slen + 1);
				}
			}
			free(stringdescPtrs);
          break;
        }
        default: GDL_WillThrowAfterCleaning("GDL_ToVPTR: unsupported case.");

      }
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
  
DStringGDL* GDL_GetString(EXPORT_VPTR v) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
 		  dimension *dim;
		  EXPORT_STRING* ss;
		if (v->flags & GDL_V_ARR) {
			EXPORT_ARRAY* arraydescr = v->value.arr;
			if (arraydescr) {
				SizeT rank = arraydescr->n_dim;
				SizeT arraydim[rank];
				for (int i = 0; i < rank; ++i) arraydim[i] = arraydescr->dim[i];
				dim = new dimension(arraydim, rank);
				ss=(EXPORT_STRING*)(v->value.arr->data);
			} else {
				dim = new dimension(1);
				ss = &(v->value.str);
			}
	    } else {
			dim = new  dimension(1);
			ss=&(v->value.str);
		}
		  DStringGDL* gdls=new DStringGDL(*dim,BaseGDL::NOZERO);
//strings copy data
			for (auto i=0; i< dim->NDimElements(); ++i) if ( ss[i].slen > 0) (*gdls)[i]=std::string(ss[i].s,ss[i].slen);
//thus, delete v if temp, it deletes the original data, we're left with a copy			
			IDL_Deltmp(v);
			return gdls;
}
extern "C" {
#define C_ (char*)
char *IDL_OutputFormat[EXPORT_NUM_TYPES] = {C_ "<Undefined>", C_ "%4d", C_ "%8d", C_ "%12d", C_ "%#13.6g", C_ "%#16.8g",
	C_ "(%#13.6g,%#13.6g)", C_ "%s", C_ "", C_ "(%#16.8g,%#16.8g)", C_ "%12lu", C_ "%12lu", C_ "%8u", C_ "%12u", C_ "%22ld"};

DLL_PUBLIC char * GDL_CDECL IDL_OutputFormatFunc(int type) {
	if (type > EXPORT_MAX_TYPE) GDL_WillThrowAfterCleaning("type must be > 0 and < 15");
	return IDL_OutputFormat[type];
}
int IDL_OutputFormatLen[EXPORT_NUM_TYPES] = {11, 4, 8, 12, 13, 16, 29, 0, 0, 35, 12, 12, 8, 12, 22};
int IDL_OutputFormatLenFunc(int type) {
	if (type > EXPORT_MAX_TYPE) GDL_WillThrowAfterCleaning("type must be > 0 and < 15");
	return IDL_OutputFormatLen[type];
}
char *EXPORT_OutputFormatFull[EXPORT_NUM_TYPES] = {C_"<Undefined>", C_"%4d", C_"%8d", C_"%12d", C_"%#16.8g", C_"%#25.17g",
	C_"(%#16.8g,%#16.8g)", C_"%s", C_"", C_"(%#25.17g,%#25.17g)", C_"%12lu", C_"%12lu", C_"%8u", C_"%12u", C_"%22ld"};
int IDL_OutputFormatFullLen[EXPORT_NUM_TYPES] = {11, 4, 8, 12, 16, 25, 35, 0, 0, 53, 12, 12, 8, 12, 22};

char *EXPORT_OutputFormatNatural[EXPORT_NUM_TYPES] = {C_"<Undefined>", C_"%d", C_"%d", C_"%d", C_"%g", C_"%g", C_"(%g,%g)", C_"%s", C_"", C_"(%g,%g)", C_"%u", C_"%u", C_"%u", C_"%u", C_"%ld"};

// I define IDL_TypeSize[EXPORT_TYP_UNDEF] to SizeT as it is used in GDLZeroAtAddr() used by GdlExportPresentKeyword() etc for READWRITE and usually is of pointer size.
// If this causes problems, then we'll have to think twice.
EXPORT_LONG IDL_TypeSize[EXPORT_NUM_TYPES] = {sizeof (EXPORT_MEMINT), 1, sizeof (EXPORT_INT), sizeof (EXPORT_LONG), sizeof (float), sizeof (double),
	sizeof (EXPORT_COMPLEX), sizeof (EXPORT_STRING), sizeof (EXPORT_SREF), sizeof (EXPORT_DCOMPLEX), sizeof (EXPORT_MEMINT), sizeof (EXPORT_MEMINT),
	sizeof ( EXPORT_UINT), sizeof (EXPORT_ULONG), sizeof (EXPORT_LONG64), sizeof (EXPORT_ULONG64)};

EXPORT_LONG GDL_TypeAlignment[EXPORT_NUM_TYPES] = {0, 1, sizeof (EXPORT_INT), sizeof (EXPORT_LONG), sizeof (float), sizeof (double),
	sizeof (float), sizeof (EXPORT_MEMINT), sizeof (EXPORT_MEMINT), sizeof (double), sizeof (EXPORT_MEMINT), sizeof (EXPORT_MEMINT),
	sizeof ( EXPORT_UINT), sizeof (EXPORT_ULONG), sizeof (EXPORT_LONG64), sizeof (EXPORT_ULONG64)};

DLL_PUBLIC int GDL_CDECL IDL_TypeSizeFunc(int type) {
	if (type > EXPORT_MAX_TYPE) GDL_WillThrowAfterCleaning("type must be > 0 and < 15");
#if defined(TRACE_OPCALLS)
if (type==GDL_TYP_UNDEF) std::cerr<<"Warning, type UNDEF used"<<std::endl;
#endif
	return IDL_TypeSize[type];
}

char *IDL_TypeName[] = {C_"UNDEFINED", C_"BYTE     ", C_"INT      ", C_"LONG     ", C_"FLOAT    ", C_"DOUBLE   ", C_"COMPLEX  ", C_"STRING   ", C_"STRUCT   ", C_"DCOMPLEX ", C_"POINTER  ", C_"OBJREF   ", C_"UINT     ", C_"ULONG    ", C_"LONG64   "};

DLL_PUBLIC char * GDL_CDECL IDL_TypeNameFunc(int type) {
	if (type >= EXPORT_MAX_TYPE) GDL_WillThrowAfterCleaning("type must be > 0 and < 15");
	return IDL_TypeName[type];
}
#undef C_
}
void StructFillVariableData(EXPORT_MEMINT baseData, EXPORT_VPTR v, int t, BaseGDL* var) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
			} else dim = new dimension(1);
		} else dim = new dimension(1);
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
				if (desc == NULL) GDL_WillThrowAfterCleaning("GDL_GetStructDesc: NULL substructure descriptor, abort.");
				DStructGDL entry( desc, *dim, BaseGDL::NOALLOC);
				stru_desc->AddTag(std::string(v->value.s.sdef->tags[i].id->name), &entry);
				break;
			}

			default:
				GDL_WillThrowAfterCleaning("GDL_GetStructDesc: unsupported case.");
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
 case type: var = new gdltype(dim, BaseGDL::NOALLOC); break;

  
BaseGDL* VPTR_ToGDL(EXPORT_VPTR v, bool protect=false) {	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (v == NULL) {
		GDL_WillThrowAfterCleaning("Invalid EXPORT_VPTR used.");
	}
	if (v->type == GDL_TYP_UNDEF) {
		return NullGDL::GetSingleInstance();
	}
	if (v->flags & GDL_V_NULL) {
		return NullGDL::GetSingleInstance();
	}
	// string and structs special call as they must change the data flow (due to STRINGS!)
	if (v->type == GDL_TYP_STRING) return GDL_GetString(v);
	else if (v->flags & GDL_V_STRUCT) {
		if (protect) v->flags &= ~GDL_V_TEMP; //will no destroy data
		EXPORT_ARRAY* arraydescr = v->value.arr;
		SizeT rank = arraydescr->n_dim;
		SizeT arraydim[rank];
		for (int i = 0; i < rank; ++i) arraydim[i] = arraydescr->dim[i];
		dimension dim(arraydim, rank);
		return GDL_MakeGDLStruct(v, dim);
	} else if (v->flags & GDL_V_ARR) {
		if (protect) v->flags &= ~GDL_V_TEMP; //will no destroy data
		EXPORT_ARRAY* arraydescr = v->value.arr;
		SizeT rank = arraydescr->n_dim;
		SizeT arraydim[rank];
		for (int i = 0; i < rank; ++i) arraydim[i] = arraydescr->dim[i];
		dimension dim(arraydim, rank);
		BaseGDL* var;
		switch (v->type) {
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
			default: GDL_WillThrowAfterCleaning("VPTR_ToGDL: bad array case.");
		}
//		if ((SizeT) (arraydescr->data) % (16 * sizeof(size_t))) std::cerr << "unaligned\n";
		var->SetCallbackFunction(v->value.arr->free_cb);
		var->SetBuffer(arraydescr->data);
		var->SetBufferSize(v->value.arr->n_elts); //dim.NDimElements());
		var->SetDim(dim);
		return var;
	} else {
		switch (v->type) {
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
			default: GDL_WillThrowAfterCleaning("ReturnEXPORT_VPTR_AsGDL: bad array case.");
		}
	}
	return NULL;
}
#undef DOCASE
#undef DOCASE_ARRAY

extern "C" {

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_ImportArray(int n_dim, EXPORT_MEMINT dim[], int type, UCHAR *data, EXPORT_ARRAY_FREE_CB free_cb,  EXPORT_StructDefPtr s){	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)

	if (type == GDL_TYP_STRUCT){
	 if (s==NULL) GDL_WillThrowAfterCleaning("EXPORT_ImportArray() defines a struct without passing a valid EXPORT_StructDefPtr");
	 EXPORT_VPTR v= NewTMPVPTRSTRUCTWithCB(s,free_cb);
		v->type = type;
		SizeT l = 1;
		for (auto i = 0; i < n_dim; ++i) {
			l *= dim[i];
			v->value.arr->dim[i] = dim[i];
		}
		v->value.s.arr->n_elts = l;
		v->value.s.arr->n_dim = n_dim;
		EXPORT_LONG64 sz = s->length;
		v->value.s.arr->elt_len = sz;
		v->value.s.arr->arr_len = sz*l;
		v->value.s.arr->data = data;
		return v;
	} else {	
		EXPORT_VPTR v= NewTMPVPTRARRAYWithCB(free_cb);
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
		return v;
	}
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
			GDL_WillThrowAfterCleaning("CreateNewGDLArray failure.");
	}
	return NULL;
}
#undef DOCASE
  
extern "C" {
#define DOCASE(ty, what)\
 case ty: {snprintf (&infoline[l], IDL_OutputFormatLen[v->type]+1, IDL_OutputFormat[v->type],v->value.what);break;}
#define DOCASE_CMP(ty, what)\
 case ty: {snprintf (&infoline[l], IDL_OutputFormatLen[v->type]+1, IDL_OutputFormat[v->type],v->value.what.r,v->value.what.i);  break;}
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_FindNamedVariable(char *name, int ienter){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	std::string s(name);
	// should use std::find_if
    for (std::vector<std::pair <EXPORT_VPTR, std::string>>::iterator it = ExportedNamesList.begin(); it != ExportedNamesList.end(); ++it) {
				if (it->second == s) {
					return it->first;
				}
	}
	return NULL;
}
DLL_PUBLIC char * GDL_CDECL IDL_VarName(EXPORT_VPTR v){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		char* infoline=(char*) calloc(1,128);

		if (v->type == GDL_TYP_UNDEF) {strncat(infoline,"<UNDEFINED> ",13); return infoline;}

        if ((v->flags & GDL_V_TEMP)==0) {
			for (std::vector<std::pair <EXPORT_VPTR, std::string>>::iterator it = ExportedNamesList.begin(); it != ExportedNamesList.end(); ++it) {
				if (it->first == v) {
					strncat(infoline,it->second.c_str(),it->second.size());
					break;
				}
				strncat(infoline,"<No Name>",10);
			}
			return infoline;
		}
        strncat(infoline,"<Expression> ",14);
		strncat(infoline,IDL_TypeNameFunc(v->type),9);
		if (( v->flags & GDL_V_ARR) && (v->value.arr != NULL)) {
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
				DOCASE(GDL_TYP_STRING, str.s);
				DOCASE(GDL_TYP_STRUCT, s.sdef->id->name);
			default: Warning/*GDL_WillThrowAfterCleaning*/("IDL_VarName: unexpected type "+i2s(v->type));
			}
			strncat(infoline,")",2);
		}
		return infoline;
	}
#undef DOCASE
#undef DOCASE_CMP

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GetVarAddr1(char *name, int enter){
	GDL_WillThrowAfterCleaning("IDL_GetVarAddr is not currently programmed -- as it would never be the address of a real GDL variable. Use parameters in call to get a copy of GDL variables.");
	return NULL;
}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_GetVarAddr(char *name){
	return IDL_GetVarAddr1(name, 0);
}

DLL_PUBLIC void  GDL_CDECL IDL_VarEnsureSimple(EXPORT_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
if (v->flags == 0) return;
    static char* message= (char*)"Expression must be a scalar in this context: ";
	if ( !( v->flags & GDL_TYP_B_SIMPLE) || ( v->flags & GDL_V_ARR) ) GDL_WillThrowAfterCleaning(message+std::string(IDL_VarName(v)));
}
DLL_PUBLIC EXPORT_VPTR GDL_CDECL IDL_VarTypeConvert(EXPORT_VPTR v, GDL_REGISTER int type){
	v->type=type;
	return v;
}
#define DOCASE(ty, what)\
 case ty: fprintf (stdout,IDL_OutputFormat[v->type],v->value.what);break;
#define DOCASE_ARRAY(ty, c_ty)\
 case ty: {\
 const c_ty *val=(c_ty *) v->value.arr->data;\
 int i=0;\
 int w=0;\
 int l=IDL_OutputFormatLen[v->type];\
 for (; i< v->value.arr->n_elts; ++i) {\
 w+=l;\
 fprintf (stdout,IDL_OutputFormat[v->type],val[i]);\
 if (w>=ncols) {fprintf (stdout,"\n");w=0;} }\
 fprintf (stdout,"\n");}\
 break;
DLL_PUBLIC void GDL_CDECL GDL_Print(int argc, EXPORT_VPTR *argv, char *argk, bool print_to_file) {
//argk is to be set to NULL by users according to the doc.
int start = 0;
int ncols = TermWidth();
if (print_to_file) {
	EXPORT_LONG lun = argv[0]->value.l;
	start = 1;
	if (lun < -2 || lun > maxLun) GDL_WillThrowAfterCleaning("File unit is not within allowed range: " + i2s(lun) + ".");
	if (lun == 0) GDL_WillThrowAfterCleaning("Cannot print to standard input.");
	if (lun > 0) { //need a special treatment, as our luns are not at all file descriptors...
		int proIx = LibProIx("PRINTF");
		  EnvT* newEnv = new EnvT(DInterpreter::CallStackBack()->CallingNode(),libProList[proIx]);
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
	if (v->flags & GDL_V_ARR) {
		switch (v->type) {
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
				DOCASE_ARRAY(GDL_TYP_STRING, EXPORT_STRING);
			default: GDL_WillThrowAfterCleaning("IDL_Print: unexpected type " + i2s(v->type));
		}
	} else {
		switch (v->type) {
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
				DOCASE(GDL_TYP_STRING, str);
			default: GDL_WillThrowAfterCleaning("IDL_Print: unexpected type " + i2s(v->type));
		}
	}
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
		s->s=(char*)malloc(s->slen+1);
	    strncpy(s->s,fs,s->slen+1);
	}
}
DLL_PUBLIC char* GDL_CDECL IDL_VarGetString(EXPORT_VPTR s) {
	if (s->type != GDL_TYP_STRING) GDL_WillThrowAfterCleaning("IDL_VarGetString: variable is not a string.");
	GDL_ENSURE_SIMPLE(s);
	if (s->value.str.slen==0) {
		return (char*) calloc(1,1);
	} else {
		return s->value.str.s;
	}
}
DLL_PUBLIC EXPORT_STRING *GDL_CDECL IDL_VarGet1EltStringDesc(EXPORT_VPTR v, EXPORT_VPTR *tc_v, int like_print){
	GDL_WillThrowAfterCleaning("IDL_VarGet1EltStringDesc() used but not yet programmed, please report.");
	return NULL;
}
// str below is a supposed to be a copy of the string descriptor(s).
// to properly duplicate, one has to create copies of the string(s) and update the descriptor(s)
DLL_PUBLIC void  GDL_CDECL IDL_StrDup(GDL_REGISTER EXPORT_STRING *str, GDL_REGISTER EXPORT_MEMINT n){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	for (auto i=0; i< n; ++i) {
		if (str[i].slen > 0) {
			char* news=(char*)malloc(str[i].slen+1);
			strncpy(news,str[i].s,str[i].slen+1);
			str[i].s=news;
			str[i].stype=1;
		}
	}
}

DLL_PUBLIC void  GDL_CDECL IDL_StrDelete(EXPORT_STRING *str, EXPORT_MEMINT n) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	for (auto i=0; i< n; ++i) {
		if (str[i].slen > 0 && str[i].stype==1) {free(str[i].s);str[i].s=NULL;}
		str[i].slen=0;
		str[i].stype=0;
	}
}

// n: The number of characters the string must be able to contain, not including the terminating NULL character.
DLL_PUBLIC void  GDL_CDECL IDL_StrEnsureLength(EXPORT_STRING *s, int n) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
        if (n<0) GDL_WillThrowAfterCleaning("IDL_StrEnsureLength() passed a negative string length!");
		if (s->slen < n) {
			IDL_StrDelete(s, 1); //takes into account slen==0
			s->slen=n;
			if (n > 0) s->s = (char*) malloc(n+1); //will contain garbage as described in documentation.
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
			ret->value.str.s=(char*) malloc(l+1);
		    strncpy(ret->value.str.s,s,l+1);
		}
		return ret;
	}

#define DOCASE(type, what)\
 case type: {dest->value.what=value->what; break;}
#define DOCASE_CMP(type, what)\
 case type: {dest->value.what.r=value->what.r; dest->value.what.i=value->what.i;  break;}
DLL_PUBLIC void  GDL_CDECL IDL_StoreScalar(EXPORT_VPTR dest, int type,	EXPORT_ALLTYPES * value) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	GDL_ENSURE_SIMPLE(dest);
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
			default: GDL_WillThrowAfterCleaning("IDL_StoreScalar: unexpected type "+i2s(type));
		}
	}
#undef DOCASE
#undef DOCASE_CMP

#define DOCASE(type, what)\
 case type: {dest->value.what=0; break;}
#define DOCASE_CMP(type, what)\
 case type: {dest->value.what.r=0; dest->value.what.i=0;  break;}
DLL_PUBLIC void  GDL_CDECL IDL_StoreScalarZero(EXPORT_VPTR dest, int type) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	GDL_ENSURE_SIMPLE(dest);
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
			default: GDL_WillThrowAfterCleaning("IDL_StoreScalar: unexpected type "+i2s(type));
		}
	}
#undef DOCASE
#undef DOCASE_CMP

DLL_PUBLIC void  GDL_CDECL IDL_VarCopy(GDL_REGISTER EXPORT_VPTR src, GDL_REGISTER EXPORT_VPTR dst) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	if (dst->value.arr != NULL) {
		free(dst->value.arr->data);
		dst->value.arr = NULL;
	}
	dst->type = src->type;
	dst->value = src->value; //copy pointers.
	if (src->flags & GDL_V_STRUCT) { //must copy
		GDL_WillThrowAfterCleaning("IDL_VarCopy not yet ready for structure, FIXME.");
	} else if (src->flags & GDL_V_ARR) { //must copy
		EXPORT_ARRAY *a = new EXPORT_ARRAY();
		a->flags = GDL_A_NO_GUARD;
		dst->value.arr = a;
		dst->value.arr->n_dim = src->value.arr->n_dim;
		dst->value.arr->n_elts = src->value.arr->n_elts;
		for (auto i = 0; i < src->value.arr->n_dim; ++i) dst->value.arr->dim[i] = src->value.arr->dim[i];
		dst->value.arr->elt_len = IDL_TypeSizeFunc(src->type);
		dst->value.arr->arr_len = src->value.arr->n_elts * dst->value.arr->elt_len;
		if (src->flags & GDL_V_TEMP) {
			dst->value.arr->data = src->value.arr->data; //do not copy if src is temp, just pass.
		} else {
			void * addr = malloc(dst->value.arr->arr_len);
			dst->value.arr->data = (UCHAR*) addr;
			memcpy(dst->value.arr->data, src->value.arr->data, dst->value.arr->arr_len);
		}
		if (src->flags & GDL_V_TEMP) {
			src->flags = GDL_V_TEMP; //only that.
			src->type = GDL_TYP_UNDEF;
		}
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
			default: GDL_WillThrowAfterCleaning("IDL_DoubleScalar: unexpected type "+i2s(v->type));
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
			default: GDL_WillThrowAfterCleaning("IDL_ULongScalar: unexpected type "+i2s(v->type));
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
			default: return (EXPORT_LONG64) 0;
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
			default: GDL_WillThrowAfterCleaning("IDL_ULong64Scalar: unexpected type "+i2s(v->type));
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
			default: GDL_WillThrowAfterCleaning("IDL_ULong64Scalar: unexpected type "+i2s(v->type));
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
			default: GDL_WillThrowAfterCleaning("IDL_ULong64Scalar: unexpected type "+i2s(v->type));
		}
		return 0;
	}
#undef DOCASE
#undef DOCASE_CMP

DLL_PUBLIC void  GDL_CDECL IDL_VarGetData(EXPORT_VPTR v, EXPORT_MEMINT *n, char **pd,  int ensure_simple){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (ensure_simple) GDL_ENSURE_SIMPLE(v);
	if ((v->flags & GDL_V_ARR) && ( v->value.arr != NULL) ) {
		*n=v->value.arr->n_elts;
		*pd=(char*) v->value.arr->data;
	} else {
		*n=1;
		*pd=(char*) &(v->value);
	}
	
}

#define DOCASE_ARRAY(type, realtype)\
 case type: {realtype *z=(realtype*) arr; for (auto i=0; i< nelts; ++i) z[i]=i ;} break;
#define DOCASE_ARRAY_CPLX(type, realtype)\
 case type: {realtype *z=(realtype*) arr; for (auto i=0; i< nelts; ++i) z[i].r=i ;} break;
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
					default: GDL_WillThrowAfterCleaning("IDL_MakeTempVector: unexpected type " + i2s(type));
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
void * addr=malloc(l); //aka GDL_ARR_INI_NOP
v->value.arr->arr_len=l;
v->value.arr->data = (UCHAR*) addr;
if (init == GDL_ARR_INI_ZERO) memset((void*)addr, 0, l);
else if (init == GDL_ARR_INI_INDEX) {
	if (type == GDL_TYP_STRING) {
		EXPORT_LONG64 nelts=v->value.arr->n_elts;
		static int slen = IDL_OutputFormatLenFunc(GDL_TYP_LONG);
		// allocate size:
		char* allstrings = (char*)malloc(nelts * (slen+1));
		EXPORT_STRING *thestrings = (EXPORT_STRING *) v->value.arr->data;
		char* fmt=IDL_OutputFormatFunc(GDL_TYP_LONG);
		for (auto i = 0; i < nelts; ++i) {
			thestrings[i].slen = slen;
			thestrings[i].s = &(allstrings[i * (slen+1)]);
			snprintf(thestrings[i].s,slen+1, fmt, i);
		}
	} else gdlInitVector(addr, type, l);
}
return (char*) addr;
}

DLL_PUBLIC char * GDL_CDECL IDL_MakeTempStructVector(EXPORT_StructDefPtr sdef, EXPORT_MEMINT dim, EXPORT_VPTR *var, int zero) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
EXPORT_VPTR v = NewTMPVPTRSTRUCT(sdef);
*var = v;
v->value.s.arr=new EXPORT_ARRAY();
v->type=GDL_TYP_STRUCT; //already done, just for clarity.
v->value.arr->dim[0]=dim;
v->value.arr->n_elts=dim;
v->value.arr->n_dim=1;
EXPORT_LONG64 sz=v->value.s.sdef->length;
v->value.arr->elt_len = sz;
SizeT l=dim*sz;
void * addr=malloc(l);
if (zero) memset(addr, 0, l);
v->value.arr->arr_len=l;
v->value.arr->data = (UCHAR*) addr;
return (char*) addr;
}

DLL_PUBLIC char * GDL_CDECL IDL_MakeTempArray(int type, int n_dim, EXPORT_MEMINT  dim[], int init, EXPORT_VPTR *var){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
EXPORT_VPTR v=NewTMPVPTRARRAY();
*var=v;
v->type=type;
SizeT l=1;
for (auto i=0; i< n_dim; ++i) {
	l*=dim[i];
	v->value.arr->dim[i] = dim[i];
}
v->value.arr->n_elts=l;
v->value.arr->n_dim = n_dim;
EXPORT_LONG64 sz=IDL_TypeSizeFunc(type);
v->value.arr->elt_len = sz;
l=sz; for (auto i=0; i<n_dim; ++i) l*=dim[i];
void * addr=malloc(l); //aka GDL_ARR_INI_NOP
v->value.arr->arr_len=l;
v->value.arr->data = (UCHAR*) addr;
if (init == GDL_ARR_INI_ZERO)  memset((void*)addr, 0, l);
else if (init == GDL_ARR_INI_INDEX) {
	if (type == GDL_TYP_STRING) {
		EXPORT_LONG64 nelts=v->value.arr->n_elts;
		static int slen = IDL_OutputFormatLenFunc(GDL_TYP_LONG);
		// allocate size:
		char* allstrings = (char*)malloc(nelts * (slen+1));
		EXPORT_STRING *thestrings = (EXPORT_STRING *) v->value.arr->data;
		char* fmt=IDL_OutputFormatFunc(GDL_TYP_LONG);
		for (auto i = 0; i < nelts; ++i) {
			thestrings[i].slen = slen;
			thestrings[i].s = &(allstrings[i * (slen+1)]);
			snprintf(thestrings[i].s, slen+1, fmt, i);
		}
	} else gdlInitVector(addr, type, l);
}
return (char*) addr;	
}

DLL_PUBLIC char * GDL_CDECL IDL_MakeTempStruct(EXPORT_StructDefPtr sdef, int  n_dim, EXPORT_MEMINT *dim, EXPORT_VPTR *var, int zero){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	if (sdef == NULL) GDL_WillThrowAfterCleaning("EXPORT_VarMakeTempFromTemplate() defines a struct without passing a valid EXPORT_StructDefPtr");
	EXPORT_VPTR v = NewTMPVPTRSTRUCT(sdef);
	if (zero) memset(&(v->value),0,sizeof(EXPORT_ALLTYPES));
	return (char*) 	&(v->value);
}


DLL_PUBLIC char * GDL_CDECL IDL_VarMakeTempFromTemplate(EXPORT_VPTR template_var, int type, EXPORT_StructDefPtr sdef,  EXPORT_VPTR *result_addr, int zero){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
EXPORT_VPTR t = template_var;
EXPORT_VPTR v;
if (t->flags & GDL_V_STRUCT) {
	if (sdef == NULL) GDL_WillThrowAfterCleaning("IDL_VarMakeTempFromTemplate() defines a struct without passing a valid IDL_StructDefPtr");
	v = NewTMPVPTRSTRUCT(sdef);
} else if (t->flags & GDL_V_ARR) {
	v = NewTMPVPTRARRAY();
} else {
	v = NewTMPVPTR(t->flags);
}
*result_addr = v;
if (t->flags & GDL_V_ARR) {
	v->type = type;
	v->flags = t->flags;
	v->value.arr->n_dim = t->value.arr->n_dim;
	v->value.arr->n_elts=t->value.arr->n_elts;
	for (auto i=0; i< t->value.arr->n_dim; ++i) v->value.arr->dim[i] = t->value.arr->dim[i];
	EXPORT_LONG64 sz=IDL_TypeSizeFunc(type);
	v->value.arr->elt_len = sz;
	SizeT l=sz; for (auto i=0; i<t->value.arr->n_dim; ++i) l*=t->value.arr->dim[i];
	v->value.arr->arr_len = l;
	void * addr=malloc(l);
	v->value.arr->data = (UCHAR*) addr;
	if (zero) memset((void*)addr, 0, l);
	return (char*) addr;	
} else {
	v->type = type;
	if (zero) memset(&(v->value),0,sizeof(EXPORT_ALLTYPES));
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

#define DEFOUT(idl_dst_type)\
    if (argc != 1) GDL_WillThrowAfterCleaning("IDL_CvtXXX: multiple argvuments not supported, fixme.");\
	if (argv[0]->type == idl_dst_type) return argv[0];\
	EXPORT_VPTR ret = NewTMPVPTR(argv[0]->flags);\
	ret->type = idl_dst_type;\
	EXPORT_ALLTYPES value = argv[0]->value;
#define PREPARE_ARRAY(dst_type)\
	EXPORT_ARRAY *dstarr=new EXPORT_ARRAY();\
    ret->flags |= GDL_V_ARR;\
	ret->value.arr=dstarr;\
    EXPORT_ARRAY* scrArrayDescr=argv[0]->value.arr;\
	memcpy(dstarr, scrArrayDescr, sizeof (EXPORT_ARRAY));\
	dstarr->elt_len = sizeof (dst_type);\
	dstarr->arr_len = dstarr->elt_len*dstarr->n_elts;\
	dst_type *retval = (dst_type*) malloc(dstarr->arr_len);\
	ret->value.arr->data = (UCHAR*) retval;
#define DOCASE_ARRAY(idl_src_type, src_type)\
 			case idl_src_type: {\
				const src_type *srcval=(src_type *)(scrArrayDescr->data);\
				for (auto i=0; i< scrArrayDescr->n_elts; ++i) retval[i]=srcval[i];\
				break; }
#define DOCASE_TO_CMP_ARRAY(idl_src_type, src_type)\
 			case idl_src_type: {\
				const src_type *srcval=(src_type *)(scrArrayDescr->data);\
				for (auto i=0; i< scrArrayDescr->n_elts; ++i) retval[i].r=srcval[i];\
				break; }
#define DOCASE_TO_CMP_ARRAY_FROM_CMP(idl_src_type, src_type)\
 			case idl_src_type: {\
				const src_type *srcval=(src_type *)(scrArrayDescr->data);\
				for (auto i=0; i< scrArrayDescr->n_elts; ++i) {retval[i].r=srcval[i].r;retval[i].i=srcval[i].i;}\
				break; }
#define DOCASE(type, field1, field2)\
 case type: ret->value.field1=value.field2 ; break;
#define DOCASE_FROM_CMP(type, field1, field2)\
 case type: ret->value.field1=value.field2.r ; break;
#define DOCASE_ARRAY_FROM_CMP(idl_src_type, src_type)\
 			case idl_src_type: {\
				const src_type *srcval=(src_type *)(scrArrayDescr->data);\
				for (auto i=0; i< scrArrayDescr->n_elts; ++i) retval[i]=srcval[2*i];\
				break; }
#define DOCASE_TO_CMP(type, field1, field2)\
 case type: ret->value.field1.r=value.field2 ; break;
#define DOCASE_TO_CMP_FROM_CMP(type, field1, field2)\
 case type: ret->value.field1.r=value.field2.r ; ret->value.field1.i=value.field2.i ;break;
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtByte(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(GDL_TYP_BYTE);
		if (argv[0]->flags & GDL_V_ARR) {
		PREPARE_ARRAY(UCHAR);		
	    switch (argv[0]->type) {
		DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
		DOCASE_ARRAY(GDL_TYP_INT, EXPORT_INT);
		DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG );
		DOCASE_ARRAY(GDL_TYP_FLOAT, float); 
		DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
		DOCASE_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, float );
		DOCASE_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, double );
		DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT );
		DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG );
		DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64 );
		DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
			default: GDL_WillThrowAfterCleaning("IDL_CvtByte: unexpected type");
		}
	} else {
		switch (argv[0]->type) {
		DOCASE(GDL_TYP_BYTE, c, c);
		DOCASE(GDL_TYP_INT, c, i);
		DOCASE(GDL_TYP_LONG, c, l);
		DOCASE(GDL_TYP_FLOAT, c, f);
		DOCASE(GDL_TYP_DOUBLE, c, d);
		DOCASE_FROM_CMP(GDL_TYP_COMPLEX, c, cmp);
		DOCASE_FROM_CMP(GDL_TYP_DCOMPLEX, c, dcmp);
		DOCASE(GDL_TYP_UINT, c, ui);
		DOCASE(GDL_TYP_ULONG, c, ul);
		DOCASE(GDL_TYP_LONG64, c, l64);
		DOCASE(GDL_TYP_ULONG64, c, ul64);
			default: GDL_WillThrowAfterCleaning("IDL_CvtByte: unexpected type");
		}
	}
	return ret;
}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtBytscl(int argc, EXPORT_VPTR argv[], char *argk) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(GDL_TYP_BYTE);
		if (argv[0]->flags & GDL_V_ARR) {
			PREPARE_ARRAY(UCHAR);
			switch (argv[0]->type) {
					DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
			switch (argv[0]->type) {
					DOCASE(GDL_TYP_BYTE, c, c);
					DOCASE(GDL_TYP_INT, c, i);
					DOCASE(GDL_TYP_LONG, c, l);
					DOCASE(GDL_TYP_FLOAT, c, f);
					DOCASE(GDL_TYP_DOUBLE, c, d);
					DOCASE_FROM_CMP(GDL_TYP_COMPLEX, c, cmp);
					DOCASE_FROM_CMP(GDL_TYP_DCOMPLEX, c, dcmp);
					DOCASE(GDL_TYP_UINT, c, ui);
					DOCASE(GDL_TYP_ULONG, c, ul);
					DOCASE(GDL_TYP_LONG64, c, l64);
					DOCASE(GDL_TYP_ULONG64, c, ul64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		}
		return ret;
}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtFix(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(GDL_TYP_INT);
		if (argv[0]->flags & GDL_V_ARR) {
			PREPARE_ARRAY(EXPORT_INT);
			switch (argv[0]->type) {
					DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
			switch (argv[0]->type) {
					DOCASE(GDL_TYP_BYTE, i, c);
					DOCASE(GDL_TYP_INT, i, i);
					DOCASE(GDL_TYP_LONG, i, l);
					DOCASE(GDL_TYP_FLOAT, i, f);
					DOCASE(GDL_TYP_DOUBLE, i, d);
					DOCASE_FROM_CMP(GDL_TYP_COMPLEX, i, cmp);
					DOCASE_FROM_CMP(GDL_TYP_DCOMPLEX, i, dcmp);
					DOCASE(GDL_TYP_UINT, i, ui);
					DOCASE(GDL_TYP_ULONG, i, ul);
					DOCASE(GDL_TYP_LONG64, i, l64);
					DOCASE(GDL_TYP_ULONG64, i, ul64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		}
		return ret;

}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtUInt(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(GDL_TYP_UINT);
		if (argv[0]->flags & GDL_V_ARR) {
			PREPARE_ARRAY(EXPORT_UINT);
			switch (argv[0]->type) {
					DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(GDL_TYP_BYTE, ui, c);
				DOCASE(GDL_TYP_INT, ui, i);
				DOCASE(GDL_TYP_LONG, ui, l);
				DOCASE(GDL_TYP_FLOAT, ui, f);
				DOCASE(GDL_TYP_DOUBLE, ui, d);
				DOCASE_FROM_CMP(GDL_TYP_COMPLEX, ui, cmp);
				DOCASE_FROM_CMP(GDL_TYP_DCOMPLEX, ui, dcmp);
				DOCASE(GDL_TYP_UINT, ui, ui);
				DOCASE(GDL_TYP_ULONG, ui, ul);
				DOCASE(GDL_TYP_LONG64, ui, l64);
				DOCASE(GDL_TYP_ULONG64, ui, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;
	}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtLng(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(GDL_TYP_LONG);
		if (argv[0]->flags & GDL_V_ARR) {
			PREPARE_ARRAY(EXPORT_LONG);
			switch (argv[0]->type) {
					DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(GDL_TYP_BYTE, l, c);
				DOCASE(GDL_TYP_INT, l, i);
				DOCASE(GDL_TYP_LONG, l, l);
				DOCASE(GDL_TYP_FLOAT, l, f);
				DOCASE(GDL_TYP_DOUBLE, l, d);
				DOCASE_FROM_CMP(GDL_TYP_COMPLEX, l, cmp);
				DOCASE_FROM_CMP(GDL_TYP_DCOMPLEX, l, dcmp);
				DOCASE(GDL_TYP_UINT, l, ui);
				DOCASE(GDL_TYP_ULONG, l, ul);
				DOCASE(GDL_TYP_LONG64, l, l64);
				DOCASE(GDL_TYP_ULONG64, l, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;

	}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtULng(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(GDL_TYP_ULONG);
		if (argv[0]->flags & GDL_V_ARR) {
			PREPARE_ARRAY(EXPORT_ULONG);
			switch (argv[0]->type) {
					DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(GDL_TYP_BYTE, ul, c);
				DOCASE(GDL_TYP_INT, ul, i);
				DOCASE(GDL_TYP_LONG, ul, l);
				DOCASE(GDL_TYP_FLOAT, ul, f);
				DOCASE(GDL_TYP_DOUBLE, ul, d);
				DOCASE_FROM_CMP(GDL_TYP_COMPLEX, ul, cmp);
				DOCASE_FROM_CMP(GDL_TYP_DCOMPLEX, ul, dcmp);
				DOCASE(GDL_TYP_UINT, ul, ui);
				DOCASE(GDL_TYP_ULONG, ul, ul);
				DOCASE(GDL_TYP_LONG64, ul, l64);
				DOCASE(GDL_TYP_ULONG64, ul, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;

	}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtLng64(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(GDL_TYP_LONG64);
		if (argv[0]->flags & GDL_V_ARR) {
			PREPARE_ARRAY(EXPORT_LONG64);
			switch (argv[0]->type) {
					DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(GDL_TYP_BYTE, l64, c);
				DOCASE(GDL_TYP_INT, l64, i);
				DOCASE(GDL_TYP_LONG, l64, l);
				DOCASE(GDL_TYP_FLOAT, l64, f);
				DOCASE(GDL_TYP_DOUBLE, l64, d);
				DOCASE_FROM_CMP(GDL_TYP_COMPLEX, l64, cmp);
				DOCASE_FROM_CMP(GDL_TYP_DCOMPLEX, l64, dcmp);
				DOCASE(GDL_TYP_UINT, l64, ui);
				DOCASE(GDL_TYP_ULONG, l64, ul);
				DOCASE(GDL_TYP_LONG64, l64, l64);
				DOCASE(GDL_TYP_ULONG64, l64, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;

	}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtULng64(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(GDL_TYP_ULONG64);
		if (argv[0]->flags & GDL_V_ARR) {
			PREPARE_ARRAY(EXPORT_ULONG64);
			switch (argv[0]->type) {
					DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(GDL_TYP_BYTE, ul64, c);
				DOCASE(GDL_TYP_INT, ul64, i);
				DOCASE(GDL_TYP_LONG, ul64, l);
				DOCASE(GDL_TYP_FLOAT, ul64, f);
				DOCASE(GDL_TYP_DOUBLE, ul64, d);
				DOCASE_FROM_CMP(GDL_TYP_COMPLEX, ul64, cmp);
				DOCASE_FROM_CMP(GDL_TYP_DCOMPLEX, ul64, dcmp);
				DOCASE(GDL_TYP_UINT, ul64, ui);
				DOCASE(GDL_TYP_ULONG, ul64, ul);
				DOCASE(GDL_TYP_LONG64, ul64, l64);
				DOCASE(GDL_TYP_ULONG64, ul64, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;

	}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtMEMINT(int argc, EXPORT_VPTR argv[]) {
	EXPORT_VPTR v=IDL_CvtULng64(argc, argv);
	v->type=GDL_TYP_MEMINT;
	return v;
}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtFILEINT(int argc, EXPORT_VPTR argv[]) {
	EXPORT_VPTR v=IDL_CvtULng64(argc, argv);
	v->type=GDL_TYP_FILEINT;
	return v;
}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtFlt(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(GDL_TYP_FLOAT);
		if (argv[0]->flags & GDL_V_ARR) {
			PREPARE_ARRAY(float);
			switch (argv[0]->type) {
					DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(GDL_TYP_BYTE, f, c);
				DOCASE(GDL_TYP_INT, f, i);
				DOCASE(GDL_TYP_LONG, f, l);
				DOCASE(GDL_TYP_FLOAT, f, f);
				DOCASE(GDL_TYP_DOUBLE, f, d);
				DOCASE_FROM_CMP(GDL_TYP_COMPLEX, f, cmp);
				DOCASE_FROM_CMP(GDL_TYP_DCOMPLEX, f, dcmp);
				DOCASE(GDL_TYP_UINT, f, ui);
				DOCASE(GDL_TYP_ULONG, f, ul);
				DOCASE(GDL_TYP_LONG64, f, l64);
				DOCASE(GDL_TYP_ULONG64, f, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;
	}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtDbl(int argc, EXPORT_VPTR argv[]){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(GDL_TYP_DOUBLE);
		if (argv[0]->flags & GDL_V_ARR) {
			PREPARE_ARRAY(double);
			switch (argv[0]->type) {
					DOCASE_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
			switch (argv[0]->type) {
					DOCASE(GDL_TYP_BYTE, d, c);
					DOCASE(GDL_TYP_INT, d, i);
					DOCASE(GDL_TYP_LONG, d, l);
					DOCASE(GDL_TYP_FLOAT, d, f);
					DOCASE(GDL_TYP_DOUBLE, d, d);
					DOCASE_FROM_CMP(GDL_TYP_COMPLEX, d, cmp);
					DOCASE_FROM_CMP(GDL_TYP_DCOMPLEX, d, dcmp);
					DOCASE(GDL_TYP_UINT, d, ui);
					DOCASE(GDL_TYP_ULONG, d, ul);
					DOCASE(GDL_TYP_LONG64, d, l64);
					DOCASE(GDL_TYP_ULONG64, d, ul64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		}
		return ret;
	}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtComplex(int argc, EXPORT_VPTR argv[], char *argk=NULL) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(GDL_TYP_COMPLEX);
		if (argv[0]->flags & GDL_V_ARR) {
			PREPARE_ARRAY(EXPORT_COMPLEX);
			switch (argv[0]->type) {
					DOCASE_TO_CMP_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, EXPORT_COMPLEX);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, EXPORT_DCOMPLEX);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
			switch (argv[0]->type) {
					DOCASE_TO_CMP(GDL_TYP_BYTE, dcmp, c);
					DOCASE_TO_CMP(GDL_TYP_INT, dcmp, i);
					DOCASE_TO_CMP(GDL_TYP_LONG, dcmp, l);
					DOCASE_TO_CMP(GDL_TYP_FLOAT, dcmp, f);
					DOCASE_TO_CMP(GDL_TYP_DOUBLE, dcmp, d);
					DOCASE_TO_CMP_FROM_CMP(GDL_TYP_COMPLEX, dcmp, cmp);
					DOCASE_TO_CMP_FROM_CMP(GDL_TYP_DCOMPLEX, dcmp, dcmp);
					DOCASE_TO_CMP(GDL_TYP_UINT, dcmp, ui);
					DOCASE_TO_CMP(GDL_TYP_ULONG, dcmp, ul);
					DOCASE_TO_CMP(GDL_TYP_LONG64, dcmp, l64);
					DOCASE_TO_CMP(GDL_TYP_ULONG64, dcmp, ul64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		}
		return ret;
	}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtDComplex(int argc, EXPORT_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(GDL_TYP_DCOMPLEX);
		if (argv[0]->flags & GDL_V_ARR) {
			PREPARE_ARRAY(EXPORT_DCOMPLEX);
			switch (argv[0]->type) {
					DOCASE_TO_CMP_ARRAY(GDL_TYP_BYTE, UCHAR);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_INT,  EXPORT_INT);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_FLOAT, float);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_DOUBLE, double);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, EXPORT_COMPLEX);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, EXPORT_DCOMPLEX);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
					DOCASE_TO_CMP_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
			switch (argv[0]->type) {
					DOCASE_TO_CMP(GDL_TYP_BYTE, cmp, c);
					DOCASE_TO_CMP(GDL_TYP_INT, cmp, i);
					DOCASE_TO_CMP(GDL_TYP_LONG, cmp, l);
					DOCASE_TO_CMP(GDL_TYP_FLOAT, cmp, f);
					DOCASE_TO_CMP(GDL_TYP_DOUBLE, cmp, d);
					DOCASE_TO_CMP_FROM_CMP(GDL_TYP_COMPLEX, cmp, cmp);
					DOCASE_TO_CMP_FROM_CMP(GDL_TYP_DCOMPLEX, cmp, dcmp);
					DOCASE_TO_CMP(GDL_TYP_UINT, cmp, ui);
					DOCASE_TO_CMP(GDL_TYP_ULONG, cmp, ul);
					DOCASE_TO_CMP(GDL_TYP_LONG64, cmp, l64);
					DOCASE_TO_CMP(GDL_TYP_ULONG64, cmp, ul64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		}
		return ret;
			}

DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_CvtString(int argc, EXPORT_VPTR argv[], char *argk=NULL){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
DEFOUT(GDL_TYP_STRING);
GDL_WillThrowAfterCleaning("IDL_CvtString not supported, fixme");
//		if (argv[0]->flags & GDL_V_ARR) {
//			PREPARE_ARRAY(EXPORT_DCOMPLEX);
//			switch (argv[0]->type) {
//					DOCASE_TO_CMP_ARRAY(GDL_TYP_BYTE, UCHAR);
//					DOCASE_TO_CMP_ARRAY(GDL_TYP_INT,  EXPORT_INT);
//					DOCASE_TO_CMP_ARRAY(GDL_TYP_LONG, EXPORT_LONG);
//					DOCASE_TO_CMP_ARRAY(GDL_TYP_FLOAT, float);
//					DOCASE_TO_CMP_ARRAY(GDL_TYP_DOUBLE, double);
//					DOCASE_TO_CMP_ARRAY_FROM_CMP(GDL_TYP_COMPLEX, EXPORT_COMPLEX);
//					DOCASE_TO_CMP_ARRAY_FROM_CMP(GDL_TYP_DCOMPLEX, EXPORT_DCOMPLEX);
//					DOCASE_TO_CMP_ARRAY(GDL_TYP_UINT, EXPORT_UINT);
//					DOCASE_TO_CMP_ARRAY(GDL_TYP_ULONG, EXPORT_ULONG);
//					DOCASE_TO_CMP_ARRAY(GDL_TYP_LONG64, EXPORT_LONG64);
//					DOCASE_TO_CMP_ARRAY(GDL_TYP_ULONG64, EXPORT_ULONG64);
//				default: GDL_WillThrowAfterCleaning("unexpected type");
//			}
//		} else {
//			switch (argv[0]->type) {
//					DOCASE_TO_CMP(GDL_TYP_BYTE, cmp, c);
//					DOCASE_TO_CMP(GDL_TYP_INT, cmp, i);
//					DOCASE_TO_CMP(GDL_TYP_LONG, cmp, l);
//					DOCASE_TO_CMP(GDL_TYP_FLOAT, cmp, f);
//					DOCASE_TO_CMP(GDL_TYP_DOUBLE, cmp, d);
//					DOCASE_TO_CMP_FROM_CMP(GDL_TYP_COMPLEX, cmp, cmp);
//					DOCASE_TO_CMP_FROM_CMP(GDL_TYP_DCOMPLEX, cmp, dcmp);
//					DOCASE_TO_CMP(GDL_TYP_UINT, cmp, ui);
//					DOCASE_TO_CMP(GDL_TYP_ULONG, cmp, ul);
//					DOCASE_TO_CMP(GDL_TYP_LONG64, cmp, l64);
//					DOCASE_TO_CMP(GDL_TYP_ULONG64, cmp, ul64);
//				default: GDL_WillThrowAfterCleaning("unexpected type");
//			}
//		}
		return ret;

}
#undef DEFOUT
#undef PREPARE_ARRAY
#undef DOCASE
#undef DOCASE_ARRAY
#undef DOCASE_FROM_CMP
#undef DOCASE_TO_CMP
#undef DOCASE_TO_CMP_TO_CMP
#undef DOCASE_TO_CMP_ARRAY
#undef DOCASE_TO_CMP_ARRAY_FROM_CMP
#undef DOCASE_TO_CMP_FROM_CMP

char* GDLWriteVarAtAddr(BaseGDL* var, std::string name, UCHAR type, size_t address, bool isoutput, bool isarray){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
			case GDL_TYP_STRING:
			{
				DStringGDL* res = static_cast<DStringGDL*> (var->Convert2(GDL_STRING));
				if (isarray) {
					SizeT nEl=var->N_Elements();
					void* allstringdescr=malloc(nEl*sizeof(EXPORT_STRING));
					memset(allstringdescr,0,nEl*sizeof(EXPORT_STRING));
					void* stringdescPtrs=malloc(nEl*sizeof(EXPORT_STRING*));
					EXPORT_STRING** p=(EXPORT_STRING**)stringdescPtrs;
					for (SizeT i=0; i< nEl; ++i) p[i]=(EXPORT_STRING*)((SizeT)allstringdescr+(i*sizeof(EXPORT_STRING)));
					for (auto i=0; i< nEl; ++i) {
						p[i]->slen = ((*res)[i]).size();
						if (p[i]->slen > 0) {
							p[i]->s = (char*) malloc(p[i]->slen + 1);
						    strncpy(p[i]->s, (*res)[i].c_str(), p[i]->slen + 1);
						}
					}
					free(stringdescPtrs);
					return (char*)allstringdescr;
				//write an array of string descriptors at address; return vector of addresses to be destroyed when GDL_KWFree will be called
				} else {
					EXPORT_STRING *s=(EXPORT_STRING*) (address); //string descr is at address
					s->slen = res->NBytes();
					if (s->slen > 0) {
						s->s = (char*) malloc(s->slen + 1);
					    strncpy(s->s, (*res)[0].c_str(), s->slen + 1);
					} else s->s=NULL;
					return (char*)s;
				}
			}
			case GDL_TYP_UNDEF:
			{
				if (!isoutput) GDL_WillThrowAfterCleaning("GDLWriteVarAtAddr: variable " + name + " is not writeable.");
				break;
			}
//			case GDL_TYP_PTR:
//				GDL_WillThrowAfterCleaning("Unable to convert variable to type pointer.");
//				break;
//			case GDL_TYP_OBJREF:
//				GDL_WillThrowAfterCleaning("Unable to convert variable to type object.");
//				break;
			default: GDL_WillThrowAfterCleaning("GDLWriteVarAtAddr: unsupported case.");
		}
		return NULL;
	}
	
void GDLZeroAtAddr(size_t address, int type ){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
int l=IDL_TypeSizeFunc(type);
memset((void*) (address), 0, l);
}

EXPORT_VPTR GdlExportPresentKeyword(GDL_KW_PAR requested, GDL_KEYWORDS_LIST passed, void* kw_result) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
static const int ok = 1;
static const int cleanMem = 1;
EXPORT_VPTR toBeReturned = NULL;
bool tempo=(passed.readonly==0);
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
	if (isoutput && passed.readonly) GDL_WillThrowAfterCleaning("Keyword " + std::string(requested.keyword) + " must be a named variable.");
	BaseGDL* var = passed.varptr;
	//if requested var is NULL here, it is an undefined var, which MAY be returned as good value.
	if (var == NULL && !isoutput) GDL_WillThrowAfterCleaning("GDLExportKeyword: variable " + std::string(requested.keyword) + " is not defined.");
	if (iszero) { //zero before write.
		
		GDLZeroAtAddr(global_address, requested.type);
	}
	if (var != NULL) {
		if (!isarray && (var->N_Elements() > 1)) GDL_WillThrowAfterCleaning("Expression must be a scalar or 1 element array in this context: " + std::string(passed.name) + ".");
		if (inputByReference) {//address (relative) of a EXPORT_VPTR with input only value
			EXPORT_VPTR temp = GDL_ToVPTR(var,tempo, true);
			memcpy((void*) (global_address), (void*) (&temp), sizeof (EXPORT_VPTR)); //pass by address of a EXPORT_VAR
		} else if (isoutput) { //address (relative) of a EXPORT_VPTR where input/output value is written (if existing) and will be returned 
			toBeReturned = GDL_ToVPTR(var,false, true); //to be returned
			memcpy((void*) (global_address), (void*) (&toBeReturned), sizeof (EXPORT_VPTR)); //pass address of a EXPORT_VAR that will contain the result.
		} else if (isarray) {
			size_t array_desc_address = (size_t) (requested.value);
			GDL_KW_ARR_DESC_R* arr_desc = (GDL_KW_ARR_DESC_R*) (array_desc_address);
			//check limits
			if ((var->N_Elements() > (*arr_desc).nmax) || (var->N_Elements() < (*arr_desc).nmin))
				GDL_WillThrowAfterCleaning(
					"Keyword array parameter " + std::string(requested.keyword) + " must have from " + i2s(arr_desc->nmin) + " to " + i2s(arr_desc->nmax) + " elements.");
			//and these are offsets!
			size_t copy_address_offset = (size_t) (kw_result)+(size_t) ((*arr_desc).n_offset);
			EXPORT_MEMINT passedArraySize = var->N_Elements();
			memcpy((void*) copy_address_offset, (void*) &passedArraySize, sizeof (EXPORT_MEMINT)); //number of passed elements
			size_t data_address = (size_t) (kw_result)+(size_t) ((*arr_desc).data); //address where to pass elements
			if (GDLWriteVarAtAddr(var, std::string(requested.keyword), requested.type, data_address, isoutput, true) != NULL) {
				memcpy(kw_result, &cleanMem, sizeof (int)); //make GDL_KW_FREE in called program call GDL_KWFree();
			}
		} else {
			//here GDL_KW_VALUE may appear
			if (byMask) {
				if (requested.type != GDL_TYP_LONG) GDL_WillThrowAfterCleaning("Invalid use of GDL_KW_VALUE on non-integer keyword.");
				long mask = GDL_KW_VALUE_MASK & requested.flags;
				long *val = (long*) global_address;
				*val |= mask;
			} else {
				if (GDLWriteVarAtAddr(var, std::string(requested.keyword), requested.type, global_address, isoutput, false) != NULL) {
					memcpy(kw_result, &cleanMem, sizeof (int)); //make GDL_KW_FREE in called program call GDL_KWFree();
				}
			}
		}
	} else if (isoutput) {
				toBeReturned = IDL_Gettmp();
				memcpy((void*) (global_address), (void*) (&toBeReturned), sizeof (EXPORT_VPTR)); //pass address of a EXPORT_VAR that will contain the result.
	}
}
return toBeReturned;
}

EXPORT_VPTR GdlExportPresentKeywordInOldApi(GDL_KW_PAR requested, GDL_KEYWORDS_LIST passed, void* address) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
static const int ok = 1;
EXPORT_VPTR toBeReturned = NULL;

bool tempo=(passed.readonly==0);
bool iszero = ((requested.flags & GDL_KW_ZERO) == GDL_KW_ZERO); //zero field if requested
bool inputByReference = ((requested.flags & GDL_KW_VIN) == GDL_KW_VIN); // input, but passed by reference
bool isoutput = ((requested.flags & GDL_KW_OUT) == GDL_KW_OUT);
bool isarray = ((requested.flags & GDL_KW_ARRAY) == GDL_KW_ARRAY); //var must be an array, field is a GDL_KW_ARR_DESC_R*
bool byMask = ((requested.flags & GDL_KW_VALUE) == GDL_KW_VALUE);
if (requested.specified != NULL) { 
	memcpy((void*) (address), &ok, sizeof (int)); //requested is in offset
}
if (requested.value != NULL) { // need to pass either an address of a EXPORT_VPTR or fill in static elements of the structure exchanged with routine
	if (isoutput && passed.readonly) GDL_WillThrowAfterCleaning("Keyword " + std::string(requested.keyword) + " must be a named variable.");
	BaseGDL* var = passed.varptr;
	//if requested var is NULL here, it is an undefined var, which MAY be returned as good value.
	if (var == NULL && !isoutput) GDL_WillThrowAfterCleaning("GDLExportKeyword: variable " + std::string(requested.keyword) + " is not defined.");
	if (iszero) GDLZeroAtAddr((size_t)address, requested.type);
	if (var != NULL) {
		if (!isarray && (var->N_Elements() > 1)) GDL_WillThrowAfterCleaning("Expression must be a scalar or 1 element array in this context: " + std::string(passed.name) + ".");
		if (inputByReference) {//address (relative) of a EXPORT_VPTR with input only value
			EXPORT_VPTR temp = GDL_ToVPTR(var,tempo,true);
			memcpy((void*) (address), (void*) (&temp), sizeof (EXPORT_VPTR)); //pass by address of a EXPORT_VAR
		} else if (isoutput) { //address (relative) of a EXPORT_VPTR where input/output value is written (if existing) and will be returned 
			toBeReturned = GDL_ToVPTR(var,false,true);
			memcpy((void*) (address), (void*) (&toBeReturned), sizeof (EXPORT_VPTR)); //pass address of a EXPORT_VAR that will contain the result.
		} else if (isarray) {
			size_t array_desc_address = (size_t) (requested.value);
			GDL_KW_ARR_DESC* arr_desc = (GDL_KW_ARR_DESC*) (array_desc_address);
			//check limits
			if ((var->N_Elements() > (*arr_desc).nmax) || (var->N_Elements() < (*arr_desc).nmin))
				GDL_WillThrowAfterCleaning(
					"Keyword array parameter " + std::string(requested.keyword) + " must have from " + i2s(arr_desc->nmin) + " to " + i2s(arr_desc->nmax) + " elements.");
			//and these are offsets!
			EXPORT_MEMINT passedArraySize = var->N_Elements();
			memcpy((void*) address, (void*) &passedArraySize, sizeof (EXPORT_MEMINT)); //number of passed elements
			size_t data_address = (size_t) ((*arr_desc).data); //address where to pass elements
			GDLWriteVarAtAddr(var, std::string(requested.keyword), requested.type, data_address, isoutput, true);
		} else {
			//here GDL_KW_VALUE may appear
			if (byMask) {
				if (requested.type != GDL_TYP_LONG) GDL_WillThrowAfterCleaning("Invalid use of GDL_KW_VALUE on non-integer keyword.");
				long mask = GDL_KW_VALUE_MASK & requested.flags;
				long *val = (long*) address;
				*val |= mask;
			} else {
				GDLWriteVarAtAddr(var, std::string(requested.keyword), requested.type, (size_t) address, isoutput, false);
			}
		}
	} else if (isoutput) {
				toBeReturned = IDL_Gettmp();
				memcpy((void*) (address), (void*) (&toBeReturned), sizeof (EXPORT_VPTR)); //pass address of a EXPORT_VAR that will contain the result.
	}
}
return toBeReturned;
}

void GdlExportAbsentKeywordInOldApi(GDL_KW_PAR requested, void* address) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
				static const int ok = 1;
		static const int nok = 0;

		bool iszero = ((requested.flags & GDL_KW_ZERO) == GDL_KW_ZERO); //zero field if requested
		bool byAddress = ((requested.flags & GDL_KW_VIN) == GDL_KW_VIN); // input, but passed by address
//		bool isarray = (!byAddress && (requested.flags & GDL_KW_ARRAY) == GDL_KW_ARRAY); //var will be an arry
//		bool isoutput = ((requested.flags & GDL_KW_OUT) == GDL_KW_OUT); // output, hence passed by address
//		bool byMask = ((requested.flags & GDL_KW_VALUE) == GDL_KW_VALUE);
		// tag 'out' those that will get a return value
		if (requested.specified != NULL) { // need write 0 or 1 in a special int in KW structure
			memcpy((void*) (address), &nok, sizeof (int)); //requested is in offset
		}
		if (requested.value != NULL) { 
			//if requested var is not present, this has to be returned
			if (iszero) GDLZeroAtAddr((size_t)address, requested.type);
		}
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
				} else {
					address = (size_t) (kw_result)+(size_t) (requested.value);
				}
				GDLZeroAtAddr(address, requested.type);
			}
		}
	}

DLL_PUBLIC int  GDL_CDECL IDL_KWGetParams(int argc, EXPORT_VPTR *argv, char *argk_passed, GDL_KW_PAR *kw_requested, EXPORT_VPTR *plain_args, int mask) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		for (auto i = 0; i < argc; ++i) plain_args[i] = argv[i];
		//argk is a pointer to a  GDL_PASS_KEYWORDS_LIST struct
		GDL_PASS_KEYWORDS_LIST* container = (GDL_PASS_KEYWORDS_LIST*) argk_passed;
		int npassed = 0;
		GDL_KEYWORDS_LIST* argk;
		if (container) {
			npassed = container->npassed;
			argk = container->passed;
		}
		//build a vector of desired keywords 
		int ikw = 0;
		int irequested = 0;
		const char * kw;
		bool do_fast_scan = false; // not yet used, but retrieved.
		if (kw_requested == NULL) {
			return argc;
		}
		kw = kw_requested[0].keyword;
		if (kw == NULL) {
			return argc;
		}
		if (kw[0] == 0) {
			do_fast_scan = true;
			ikw++; // start at 1 next item
			irequested++; //start at 1 too below
		}
		//get the list of expected KWs. Initial value of <int> in map will be: -1 (take into account) -2:ignored
		std::map<int, int> requested;
		std::map<int, int>::iterator it;
		while ((kw = kw_requested[ikw].keyword) != NULL) {
			int code = -1;
			if ((kw_requested[ikw].mask & mask) == 0) code = -2;
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
					if (it->second == -2) { //ignored
						ignored = true;
						break;
					}
					it->second = ipassed;
					found = true;
					break;
				}
			}
			if (!found && !ignored) {
				DInterpreter::CallStackBack()->Throw("Invalid keyword " + std::string(s));
			} else {
				for (++it; it != requested.end(); ++it) { //search for ambiguous KW
					const char* expected_kw =  kw_requested[it->first].keyword;
					if (strncmp(expected_kw, s, l) == 0) DInterpreter::CallStackBack()->Throw("Ambiguous keyword abbreviation: " + std::string(s));
				}
			}
		}
		//populate all passed addresses
		for (it = requested.begin(); it != requested.end(); ++it) {
			int ipassed = it->second;
			if (ipassed == -1) GdlExportAbsentKeywordInOldApi(kw_requested[it->first], kw_requested[it->first].value);
			else if (ipassed >= 0) {
				EXPORT_VPTR ret = GdlExportPresentKeywordInOldApi(kw_requested[it->first], argk[it->second], kw_requested[it->first].value);
				if (ret != NULL) {
					argk[ipassed].out = ret; //pass vptr back
				}
			}
			irequested++;
		}
		return 0;
	}

DLL_PUBLIC int  GDL_CDECL IDL_KWProcessByOffset(int argc, EXPORT_VPTR *argv, char *argk_passed, GDL_KW_PAR *kw_requested, EXPORT_VPTR *plain_args, int mask, void *kw_result) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
				static const int NoClean = 0;
		// this is always true with GDL:
		if (plain_args) for (auto i = 0; i < argc; ++i) plain_args[i] = argv[i];
		//argk is a pointer to a  GDL_PASS_KEYWORDS_LIST struct
		GDL_PASS_KEYWORDS_LIST* container = (GDL_PASS_KEYWORDS_LIST*) argk_passed;
		int npassed = 0;
		GDL_KEYWORDS_LIST* argk;
		if (container) {
			npassed = container->npassed;
			argk = container->passed;
		}
		//build a vector of desired keywords 
		int ikw = 0;
		int irequested = 0;
		const char * kw;
		bool do_fast_scan = false; // not yet used, but retrieved.
		if (kw_requested == NULL) {
			return argc;
		}
		kw = kw_requested[0].keyword;
		if (kw == NULL) {
			return argc;
		}
		if (kw[0] == 0) {
			do_fast_scan = true;
			ikw++; // start at 1 next item
			irequested++; //start at 1 too below
		}
		//get the list of expected KWs. Initial value of <int> in map will be: -1 (take into account) -2:ignored
		std::map<int, int> requested;
		std::map<int, int>::iterator it;
		while ((kw = kw_requested[ikw].keyword) != NULL) {
			int code = -1;
			if ((kw_requested[ikw].mask & mask) == 0) code = -2;
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
					if (it->second == -2) { //ignored
						ignored = true;
						break;
					}
					it->second = ipassed;
					found = true;
					break;
				}
			}
			if (!found && !ignored) {
				DInterpreter::CallStackBack()->Throw("Invalid keyword " + std::string(s));
			} else {
				for (++it; it != requested.end(); ++it) { //search for ambiguous KW
					const char* expected_kw =  kw_requested[it->first].keyword;
					if (strncmp(expected_kw, s, l) == 0) DInterpreter::CallStackBack()->Throw("Ambiguous keyword abbreviation: " + std::string(s));
				}
			}
		}
		//populate kw_result
		//first set GDL_KW_RESULT_FIRST_FIELD to zero (no need to clean)
		memcpy(kw_result, &NoClean, sizeof (int));
		//rewind: 
		for (it = requested.begin(); it != requested.end(); ++it) {
			int ipassed = it->second;
			if (ipassed == -1) GdlExportAbsentKeyword(kw_requested[it->first], kw_result);
			else if (ipassed >= 0) {
				EXPORT_VPTR ret=GdlExportPresentKeyword(kw_requested[it->first], argk[it->second], kw_result);
				if (ret != NULL) {
					argk[ipassed].out=ret; //pass vptr back
				}
			}
		}
		return argc;
	}

DLL_PUBLIC int GDL_CDECL IDL_KWProcessByAddr(int argc, EXPORT_VPTR *argv, char *argk,  GDL_KW_PAR *kw_list, EXPORT_VPTR *plain_args, int mask, int *free_required){
	return argc;
}

DLL_PUBLIC void  GDL_CDECL IDL_KWFree(void) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	for (std::vector<EXPORT_VPTR>::iterator it = FreeKwList.begin(); it != FreeKwList.end(); ++it) IDL_Deltmp(*it);
	FreeKwList.clear();
};

DLL_PUBLIC void * GDL_CDECL IDL_MemAlloc(EXPORT_MEMINT n, const char *err_str, int msg_action){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) return malloc(n);}

DLL_PUBLIC void * GDL_CDECL IDL_MemRealloc(void *ptr, EXPORT_MEMINT n, const char *err_str, int action){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) return realloc(ptr,n);}

DLL_PUBLIC void  GDL_CDECL IDL_MemFree(GDL_REGISTER void *m, const char *err_str, int msg_action){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) free(m);}

DLL_PUBLIC void * GDL_CDECL IDL_MemAllocPerm(EXPORT_MEMINT n, const char *err_str,  int action){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) GDL_WillThrowAfterCleaning("MemAllocPerm is not currently supported.");return NULL;}

DLL_PUBLIC char * GDL_CDECL IDL_GetScratch(GDL_REGISTER EXPORT_VPTR *p, GDL_REGISTER EXPORT_MEMINT n_elts,  GDL_REGISTER EXPORT_MEMINT elt_size){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) return (char*)malloc (n_elts*elt_size);}

DLL_PUBLIC void  GDL_CDECL GDL_KWCleanup(int fcn){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) IDL_KWFree();}

DLL_PUBLIC char * GDL_CDECL IDL_GetScratchOnThreshold(GDL_REGISTER char  *auto_buf, GDL_REGISTER EXPORT_MEMINT auto_elts,  GDL_REGISTER EXPORT_MEMINT n_elts,  GDL_REGISTER EXPORT_MEMINT elt_size,  EXPORT_VPTR *tempvar){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
	GDL_WillThrowAfterCleaning("IDL_GetScratchOnThreshold is not implemented, FIXME.");
		return NULL;
}

EXPORT_TERMINFO IDL_TermInfo={
#ifdef GDL_OS_HAS_TTYS
  (char*)"Xterm" ,          /* Name of terminal type */
  1    ,             /* True if stdin is a terminal */
#endif
  24,128};

DLL_PUBLIC char * GDL_CDECL IDL_FileTermName(void){
#ifdef GDL_OS_HAS_TTYS
	return IDL_TermInfo.name;
#else
	return (char*)"<Anonymous>";
#endif
}

DLL_PUBLIC int  GDL_CDECL IDL_FileTermIsTty(void){
#ifdef GDL_OS_HAS_TTYS
	return IDL_TermInfo.is_tty;
#else
	return 0;
#endif
}

DLL_PUBLIC int  GDL_CDECL IDL_FileTermLines(void){return IDL_TermInfo.lines;}

DLL_PUBLIC int  GDL_CDECL IDL_FileTermColumns(void){return IDL_TermInfo.columns;}

EXPORT_SYS_VERSION IDL_SysvVersion={{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL},0,0};

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvVersionArch(void){return &(IDL_SysvVersion.arch);}

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvVersionOS(void){return &(IDL_SysvVersion.os);};

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvVersionOSFamily(void){return &(IDL_SysvVersion.os_family);};

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvVersionRelease(void){return &(IDL_SysvVersion.release);};

char *IDL_ProgramName=(char*)"gdl";

DLL_PUBLIC char * GDL_CDECL IDL_ProgramNameFunc(void){return (char*)"gdl";}

char *IDL_ProgramNameLC=(char*)"gdl";

DLL_PUBLIC char * GDL_CDECL IDL_ProgramNameLCFunc(void){return (char*)"gdl";}

EXPORT_STRING IDL_SysvDir={0,0,NULL};

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvDirFunc(void){return &IDL_SysvDir;}

EXPORT_LONG IDL_SysvErrCode=0;

DLL_PUBLIC EXPORT_LONG  GDL_CDECL IDL_SysvErrCodeValue(void){return 0;};

EXPORT_SYS_ERROR_STATE IDL_SysvErrorState={{0,0,NULL},{0,0,NULL},0,{0,0},{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL}};

DLL_PUBLIC EXPORT_SYS_ERROR_STATE * GDL_CDECL IDL_SysvErrorStateAddr(void){return &IDL_SysvErrorState;}

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvErrStringFunc(void){return &(IDL_SysvErrorState.msg);}

DLL_PUBLIC EXPORT_STRING * GDL_CDECL IDL_SysvSyserrStringFunc(void){return &(IDL_SysvErrorState.sys_msg);}

DLL_PUBLIC EXPORT_LONG  GDL_CDECL IDL_SysvErrorCodeValue(void){return IDL_SysvErrorState.code;}

DLL_PUBLIC EXPORT_LONG * GDL_CDECL IDL_SysvSyserrorCodesAddr(void){return &(IDL_SysvErrorState.code);}

EXPORT_LONG IDL_SysvOrder=0;

DLL_PUBLIC EXPORT_LONG  GDL_CDECL IDL_SysvOrderValue(void){return 0;}

DLL_PUBLIC float  GDL_CDECL IDL_SysvValuesGetFloat(int type){return 0;}

DLL_PUBLIC int  GDL_CDECL IDL_MessageNameToCode(EXPORT_MSG_BLOCK block, const char *name){return 3333;} //a random (!) value
DLL_PUBLIC EXPORT_MSG_BLOCK  GDL_CDECL IDL_MessageDefineBlock(char *block_name, int n, EXPORT_MSG_DEF *defs){ return malloc(1);} //do nothing but returns an "address"
DLL_PUBLIC void  GDL_CDECL IDL_MessageErrno(int code, ...){} //do nothing. obsoleted.
DLL_PUBLIC void  GDL_CDECL IDL_MessageErrnoFromBlock(EXPORT_MSG_BLOCK block, int code, ...){} //do nothing. obsoleted.
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
		} else GDL_WillThrowAfterCleaning("Invalid Error Code given to IDL_Message() by user-written routine.");
		if (action == EXPORT_MSG_LONGJMP || EXPORT_MSG_RET) GDL_WillThrowAfterCleaning(finalMessage);
		if (action == EXPORT_MSG_IO_LONGJMP) throw GDLIOException(finalMessage);
		if (action == EXPORT_MSG_EXIT) {
			Warning(finalMessage);
			GDL_WillThrowAfterCleaning("IDL_MSG_EXIT forbidden for user-written routines.");
		}
		if (action == EXPORT_MSG_INFO) Warning(finalMessage);
	}
DLL_PUBLIC void  GDL_CDECL IDL_MessageFromBlock(EXPORT_MSG_BLOCK block, int code, int action,...){if (action!=EXPORT_MSG_INFO) GDL_WillThrowAfterCleaning("exception caused by non-GDL (dlm) function call.");}//do nothing.
DLL_PUBLIC void  GDL_CDECL IDL_MessageSyscode(int code, EXPORT_MSG_SYSCODE_T syscode_type, int syscode, int action, ...){if (action!=EXPORT_MSG_INFO) GDL_WillThrowAfterCleaning("exception caused by non-GDL (dlm) function call.");}//do nothing.
DLL_PUBLIC void  GDL_CDECL IDL_MessageSyscodeFromBlock(EXPORT_MSG_BLOCK block, int code, EXPORT_MSG_SYSCODE_T syscode_type,  int syscode, int action, ...){if (action!=EXPORT_MSG_INFO) GDL_WillThrowAfterCleaning("exception caused by non-GDL (dlm) function call.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVarError(int code, EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO) GDL_WillThrowAfterCleaning("exception caused by non-GDL (dlm) function call.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVarErrorFromBlock(EXPORT_MSG_BLOCK block, int code, EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO) GDL_WillThrowAfterCleaning("exception caused by non-GDL (dlm) function call.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageResetSysvErrorState(void) {
		try {
			std::string command = ("message,/reset");
			DInterpreter::CallStackBack()->Interpreter()->ExecuteStringLine(command);
		} catch (...) {
		}
}
DLL_PUBLIC void  GDL_CDECL IDL_MessageSJE(void *value){} //do nothing (?)
DLL_PUBLIC void * GDL_CDECL IDL_MessageGJE(void){return NULL;}//do nothing (?)
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_UNDEFVAR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO) GDL_WillThrowAfterCleaning("Variable is Undefined.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOTARRAY(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Expression must be an array in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOTSCALAR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Expression must be a scalar in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOEXPR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Expression must be a named variable in this context");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOCONST(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Constant not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOFILE(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("File expression not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOCOMPLEX(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Variable is not of complex type/");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOSTRING(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("String expression not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOSTRUCT(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Struct expression not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_REQSTR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("String expression required in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOSCALAR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Scalar variable not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NULLSTR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Null string not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOPTR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Pointer expression not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOOBJREF(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Object reference expression not allowed in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_NOMEMINT64(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("This routine is 32-bit limited and cannot handle this many elements.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_STRUC_REQ(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Expression must be a structure in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_REQPTR(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Pointer type required in this context.");}
DLL_PUBLIC void  GDL_CDECL IDL_MessageVE_REQOBJREF(EXPORT_VPTR var, int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Object reference required in this context");}
DLL_PUBLIC void  GDL_CDECL IDL_Message_BADARRDNUM(int action){if (action!=EXPORT_MSG_INFO)GDL_WillThrowAfterCleaning("Arrays are allowed 1 - 8 dimensions");}
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

#include <stdio.h>
#include <stdarg.h>
DLL_PUBLIC EXPORT_VPTR  GDL_CDECL IDL_BasicTypeConversion(int argc, EXPORT_VPTR argv[], GDL_REGISTER int type){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	if (argc != 1) GDL_WillThrowAfterCleaning("FIXME IDL_BasicTypeConversion!");
	int i=0;
	GDL_ENSURE_SIMPLE(argv[i]);
	if (argv[i]->type == type) return argv[i];

	switch(type) {
		case GDL_TYP_BYTE:     return IDL_CvtByte(1, &argv[i]);
		case GDL_TYP_INT:     return IDL_CvtFix(1, &argv[i]);
		case GDL_TYP_LONG:    return IDL_CvtLng(1, &argv[i]);
		case GDL_TYP_FLOAT:    return IDL_CvtFlt(1, &argv[i]);
		case GDL_TYP_DOUBLE:    return IDL_CvtDbl(1, &argv[i]);
		case GDL_TYP_COMPLEX:    return IDL_CvtComplex(1, &argv[i]);
		case GDL_TYP_DCOMPLEX:    return IDL_CvtDComplex(1, &argv[i]);
		case GDL_TYP_UINT:    return IDL_CvtUInt(1, &argv[i]);
		case GDL_TYP_ULONG:    return IDL_CvtULng(1, &argv[i]);
		case GDL_TYP_LONG64:    return IDL_CvtLng64(1, &argv[i]);
		case GDL_TYP_ULONG64:    return IDL_CvtULng64(1, &argv[i]);
		case GDL_TYP_STRING:    return IDL_CvtString(1, &argv[i], NULL);
		default: GDL_WillThrowAfterCleaning("Wrong type in IDL_BasicTypeConversion!");
	}
	return NULL;
}
DLL_PUBLIC int  GDL_CDECL IDL_AddSystemRoutine(EXPORT_SYSFUN_DEF *defs, int is_function, int cnt){return 1;} //using DLM insure this is OK. I think.
DLL_PUBLIC int  GDL_CDECL IDL_BailOut(int stop){return sigControlC;} //use of stop not supported.
DLL_PUBLIC void  GDL_CDECL IDL_ExitRegister(EXPORT_EXIT_HANDLER_FUNC proc){}
DLL_PUBLIC void  GDL_CDECL IDL_ExitUnregister(EXPORT_EXIT_HANDLER_FUNC proc){}
DLL_PUBLIC int  GDL_CDECL IDL_GetExitStatus(){return 0;} //do nothing
DLL_PUBLIC int  GDL_CDECL IDL_Cleanup(int just_cleanup){return 1;} //do nothing
DLL_PUBLIC int  GDL_CDECL IDL_Initialize(EXPORT_INIT_DATA *init_data){return 1;} //do nothing
DLL_PUBLIC int  GDL_CDECL IDL_Init(EXPORT_INIT_DATA_OPTIONS_T options, int *argc, char *argv[]){return 1;} //do nothing
DLL_PUBLIC int  GDL_CDECL IDL_Main(EXPORT_INIT_DATA_OPTIONS_T options, int argc, char *argv[]){return 1;} //do nothing
DLL_PUBLIC int  GDL_CDECL IDL_ExecuteStr(char *cmd) {
  std::string command(cmd);
 //always between try{} catch{} when calling ExecuteStringLine!
  try {
  DInterpreter::CallStackBack()->Interpreter()->ExecuteStringLine(command);
  } catch (...) {std::cerr<<"Problem executing command: "<<command<<" ."<<std::endl; return 0;}
  return 1;
 }
DLL_PUBLIC int  GDL_CDECL IDL_Execute(int argc, char *argv[]){
	std::string command(argv[0]);
	for (auto i=1;i< argc; ++i) {command.append(" & "); command.append(argv[i]);}
  try {
  DInterpreter::CallStackBack()->Interpreter()->ExecuteStringLine(command);
  } catch (...) {std::cerr<<"Problem executing command: "<<std::string(argv[0])<<"(...)"<<std::endl; return 0;}
  return 1;
}
DLL_PUBLIC int  GDL_CDECL IDL_RuntimeExec(char *file){Warning("IDL_RuntimeExec function not allowed in GDL.");return 0;}
DLL_PUBLIC void  GDL_CDECL IDL_Runtime(EXPORT_INIT_DATA_OPTIONS_T options, int *argc, char *argv[], char *file){GDL_WillThrowAfterCleaning("IDL_Runtime function not allowed in GDL.");}

#define DOCASE_TAG(type, gdltype, tagname, pardim)\
 case type: { gdltype entry(pardim); stru_desc->AddTag(std::string(tagname), &entry);} break;

	DStructDesc* GDL_GetStructDesc(std::string name, EXPORT_STRUCT_TAG_DEF *tags) {
		int ntags = 0;
		while (tags[ntags++].name != NULL) {
		};
		ntags--;
		DStructDesc* stru_desc = new DStructDesc(name);
		for (int itag = 0; itag < ntags; ++itag) {
			EXPORT_STRUCT_TAG_DEF def = tags[itag];
			dimension *dim;
			if (def.dims == NULL) {
				dim = new dimension(1);
			} else {
				EXPORT_LONG64 ndim = def.dims[0];
				dim = new dimension(&(def.dims[1]), ndim);
			}
			if (def.type == NULL) { /*If this field is NULL, it indicates that we should search for a structure of the given name and fill in the pointer to its structure definition. */
				if (def.name == NULL) GDL_WillThrowAfterCleaning("IDL_MakeStruct(): no name for inherited structure!");
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
					default: GDL_WillThrowAfterCleaning("GDL_GetStructDesc(EXPORT_STRUCT_TAG_DEF): bad case.");
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
			//count tags (?)
			int ntags = 0;
	while (tags[ntags++].name != NULL) {
	};
	ntags--;
	// create structure with extended size after (see https://stackoverflow.com/questions/6390331/why-use-array-size-1-instead-of-pointer)
	EXPORT_STRUCTURE *newStruct = (EXPORT_STRUCTURE *) calloc(1, sizeof (EXPORT_STRUCTURE) + ntags * sizeof (EXPORT_TAGDEF));
	if (name) {
		EXPORT_IDENT *iid = (EXPORT_IDENT*) calloc(1, sizeof (EXPORT_IDENT));
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
		EXPORT_IDENT *tagid = (EXPORT_IDENT*) calloc(1, sizeof (EXPORT_IDENT));
		newStruct->tags[itag].id = tagid;
		tagid->name = def.name;
		tagid->len = strlen(def.name);
		tagid->hash = memhash;
		memhash = tagid; //chain
		void* thetypePtr = def.type;
		if (thetypePtr == NULL) {    /*If this field is NULL, it indicates that we should search for a structure of the given name and fill in the pointer to its structure definition. */
		  if (def.name == NULL) GDL_WillThrowAfterCleaning("IDL_MakeStruct(): no name for inherited structure!");
		  std::string passed_name = std::string(def.name);
		  assert(passed_name != "$truct") ; // named struct
		  passed_name = StrUpCase(passed_name);
		  if( passed_name == "IDL_OBJECT") passed_name = GDL_OBJECT_NAME; // replacement also done in GDLParser
		  if( passed_name == "IDL_CONTAINER") passed_name = GDL_CONTAINER_NAME; // replacement also done in GDLParser
		  DStructDesc* desc = DInterpreter::CallStackBack()->Interpreter()->GetStruct(passed_name, DInterpreter::CallStackBack()->CallingNode()); //will throw if does not exist.
	      DStructGDL* tmpstruct=new DStructGDL(desc,dimension(1));
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
				newStruct->tags[itag].var.value.s.arr = new EXPORT_ARRAY();
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
				newStruct->tags[itag].var.value.arr = new EXPORT_ARRAY();
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
    ADJUST_ELEMENT_OFFSET(finalPad) //add to current struct length the necessary pad if needed 

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
			 *var=&(sdef->tags[i].var);
			}
			return sdef->tags[i].offset;
		}
		char* mess=(char*)calloc(256,1);
		strncat(mess,"Tag name ",10);strncat(mess,name,strlen(name)+1);strncat(mess," is undefined for structure ",29);
		if (sdef->id!=NULL && sdef->id->name !=NULL) strcat(mess,sdef->id->name); else strcat(mess,"<Anonymous>");
		IDL_Message(EXPORT_M_GENERIC, msg_action, mess);
		free(mess);
		return 0;
	}

DLL_PUBLIC EXPORT_MEMINT  GDL_CDECL IDL_StructTagInfoByIndex(EXPORT_StructDefPtr   sdef, int index, int msg_action, EXPORT_VPTR *var){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		if (sdef->ntags > index) {
			 *var=&(sdef->tags[index].var);
			return sdef->tags[index].offset;
		}
		char* mess=(char*)calloc(256,1);
		strncat(mess,"Tag number ",12);snprintf(mess,64,"%d",index); strncat(mess," is undefined for structure ",29);
		if (sdef->id!=NULL && sdef->id->name !=NULL) strcat(mess,sdef->id->name); else strcat(mess,"<Anonymous>");
		IDL_Message(EXPORT_M_GENERIC, msg_action, mess);
		free(mess);
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
		free(mess);
		return NULL;
}

EXPORT_STRUCT_TAG_DEF* GDL_Make_EXPORT_STRUCT_TAG_DEF(DStructGDL* gdlstruct){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	DStructDesc* desc=gdlstruct->Desc();
	SizeT nTags=desc->NTags();
	SizeT l=sizeof(EXPORT_STRUCT_TAG_DEF);
	EXPORT_STRUCT_TAG_DEF* tagarray=(EXPORT_STRUCT_TAG_DEF*)calloc((nTags+1),l);
	EXPORT_STRUCT_TAG_DEF** ret=(EXPORT_STRUCT_TAG_DEF**)MyMalloc((nTags+1)*sizeof(char*));
	for (SizeT i=0; i< nTags+1; ++i) ret[i]=(EXPORT_STRUCT_TAG_DEF*)((SizeT)tagarray+i*l);
	for (int i=0; i< nTags+1; ++i) {
		EXPORT_STRUCT_TAG_DEF* sample=ret[i];
		if (i == nTags) {
			sample->name=NULL;
			break;
		}
		BaseGDL* entry=gdlstruct->GetTag(i);
		int rank=entry->Rank();		
		EXPORT_MEMINT* dims=(EXPORT_MEMINT*)MyMalloc((rank+1)*sizeof(EXPORT_MEMINT*));
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

DLL_PUBLIC int  GDL_CDECL IDL_StructNumTags(EXPORT_StructDefPtr sdef){return sdef->ntags;}

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
			Warning("Attempt to set signal failed.\n"+std::string(strerror(errno)));
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
		for (auto i = 0; i < cnt; ++i) {
			const char* name=(const char*) defs[i].name;
			void* addr=(void*)(defs[i].funct_addr.fun);
			if (is_function){
				if (SysFunDefinitions.count(name) > 0 ) return 1; //already done
//				printf("%u %s %u %u %u %u\n", defs[i].funct_addr, name, defs[i].arg_min, defs[i].arg_max, defs[i].flags, defs[i].extra);
				SysFunDefinitions[name]=addr;
			} else {
				if (SysProDefinitions.count(name) > 0 ) return 1;
//				printf("%u %s %u %u %u %u\n", defs[i].funct_addr, name, defs[i].arg_min, defs[i].arg_max, defs[i].flags, defs[i].extra);
				SysProDefinitions[name]=addr;
			}
		}
		return 1;
	}

#endif
}
#endif
