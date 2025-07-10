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

#define IDL_TYP_COMPLEXDBL IDL_TYP_DCOMPLEX

// list of memory (strings...) to be released when IDL_KWFree() is called
static std::vector<IDL_VPTR> FreeList;
static std::vector<IDL_VPTR> FreeKwList;
static std::vector<std::pair<std::string, IDL_STRUCTURE*> > ExportedGlobalNamedStructList;
static std::vector<std::pair<IDL_VPTR,std::string> > ExportedNamesList;
typedef struct {
  const char* name;
  BaseGDL* varptr; // pointer to some externally produced var if out=true
  IDL_VPTR out=NULL;
  UCHAR type;
  UCHAR readonly; // no associated variable
} GDL_KEYWORDS_LIST;

typedef struct {
  int npassed;
  GDL_KEYWORDS_LIST* passed;
} GDL_PASS_KEYWORDS_LIST;


inline void ZeroVPTR(IDL_VPTR p) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	memset((void*)p,0,sizeof(p));
}

void IDL_CDECL IDL_Deltmp(IDL_REGISTER IDL_VPTR v) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (v->flags & IDL_V_TEMP) {
		if (v->flags & IDL_V_DYNAMIC) {
			if (v->value.arr != NULL) {
				if (v->value.arr->free_cb != NULL) v->value.arr->free_cb(v->value.arr->data); else free(v->value.arr->data);
				free(v->value.arr);
				v->value.arr=NULL;
				if (v->type == IDL_TYP_STRUCT) {free(v->value.s.sdef);v->value.s.sdef=NULL;}
			} else if (v->type == IDL_TYP_STRING) {
				if (v->value.str.slen != 0) {free(v->value.str.s);v->value.str.s=NULL;v->value.str.slen=0;}
			}
		}
	}
	//TEMPORARY IDL_VARIABLE itself will be destroyed at exit from function/procedure by GDL_FreeResources()
}

//Used by GDL_FreeResources(), like IDL_Deltmp, but deletes the IDL_VARIABLE itself
void IDL_CDECL GDL_DeleteDescriptors(IDL_REGISTER IDL_VPTR v) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		if (v->flags & IDL_V_DYNAMIC) {
			if (v->flags & IDL_V_ARR ) {
				if (v->value.arr->free_cb != NULL) v->value.arr->free_cb(v->value.arr->data);
			} //else if (v->type == IDL_TYP_STRING) if (v->value.str.slen != 0) free(v->value.str.s);
		}
}

void IDL_CDECL IDL_Freetmp(IDL_REGISTER IDL_VPTR v) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	IDL_Deltmp(v);
}

void GDL_FreeResources() {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	bool message = true;
	for (std::vector<IDL_VPTR>::iterator it = FreeList.begin(); it != FreeList.end(); ++it) {
		if ((*it)->flags & IDL_V_TEMP) {
			if (message) { message=false;
				Message("Temporary variables are still checked out - cleaning up...");
			}
			fprintf(stderr,"%s\n",IDL_VarName((*it)));
			GDL_DeleteDescriptors(*it);
//			IDL_Deltmp(*it);
		}
	}
	FreeList.clear();
}

inline void GDL_WillThrowAfterCleaning(std::string s) {
	GDL_FreeResources();
	throw GDLException(s);
}

inline void checkOK(IDL_VPTR v) {
	if (v->type == IDL_TYP_UNDEF) GDL_WillThrowAfterCleaning("Variable is undefined: <UNDEFINED>.");
}

void IDL_KWFree(void) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	for (std::vector<IDL_VPTR>::iterator it = FreeKwList.begin(); it != FreeKwList.end(); ++it) IDL_Deltmp(*it);
	FreeKwList.clear();
};

inline IDL_VPTR NewTMPVPTR(UCHAR flag=0, IDL_StructDefPtr structdefptr=NULL) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	IDL_VPTR ret = new IDL_VARIABLE();
	FreeList.push_back(ret);
	ret->type = IDL_TYP_UNDEF;
	ret->flags = IDL_V_TEMP | flag;
	if (flag & IDL_V_STRUCT  ) {
		ret->type = IDL_TYP_STRUCT;
		ret->flags |= IDL_V_DYNAMIC;
		ret->flags |= IDL_V_ARR;
		ret->value.s.arr= new IDL_ARRAY();
		ret->value.s.arr->flags = IDL_A_NO_GUARD;
		ret->value.s.sdef = structdefptr;
	} else if (flag & IDL_V_ARR) {
		ret->value.arr = new IDL_ARRAY();
		ret->flags |= IDL_V_DYNAMIC;
		ret->flags |= IDL_V_ARR;
		ret->value.arr->flags = IDL_A_NO_GUARD;
	}
	return ret;
}
inline IDL_VPTR NewTMPVPTRARRAY() {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	return NewTMPVPTR(IDL_V_ARR);
}
inline IDL_VPTR NewTMPVPTRARRAYWithCB(IDL_ARRAY_FREE_CB free_cb) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	IDL_VPTR v=NewTMPVPTR(IDL_V_ARR);
	v->value.arr->free_cb=free_cb;
	return v;
}
inline IDL_VPTR NewTMPVPTRSTRUCT(IDL_StructDefPtr structdefptr=NULL) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	return NewTMPVPTR(IDL_V_STRUCT,structdefptr);
}
inline IDL_VPTR NewTMPVPTRSTRUCTWithCB(IDL_StructDefPtr structdefptr=NULL, IDL_ARRAY_FREE_CB free_cb=NULL) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	IDL_VPTR v=NewTMPVPTR(IDL_V_STRUCT,structdefptr);
	v->value.s.arr->free_cb=free_cb;
	return v;
}
	inline IDL_VPTR NewTMPVPTRFromGDL(bool kw=false) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	IDL_VPTR ret=NewTMPVPTR(); if(kw) FreeKwList.push_back(ret); else FreeList.push_back(ret);
	ret->flags=IDL_V_TEMP| IDL_V_DYNAMIC ;
	ret->type=IDL_TYP_UNDEF;
	return ret;
}	
	inline IDL_VPTR NewSTATICVPTR() {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	IDL_VPTR ret=new IDL_VARIABLE();
	ret->type=IDL_TYP_UNDEF;
	return ret;
}
	IDL_VPTR GDL_ToVPTR(BaseGDL* var, bool tempo=false, bool is_kw=false) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
    IDL_VPTR v;
    if (tempo) v=NewTMPVPTRFromGDL(is_kw); else {v=NewSTATICVPTR(); ExportedNamesList.push_back(std::pair<IDL_VPTR,std::string>(v,DInterpreter::CallStackBack()->GetString(var)));}
    if (var == NULL) {
      v->type=IDL_TYP_UNDEF;
      return v;
    }
    if (var == NullGDL::GetSingleInstance()) {
      v->type=IDL_TYP_LONG; // any type, as the NULL flags is checked.
      v->flags|=IDL_V_NULL;
      return v;
    }
    if (var->N_Elements() == 1) {
      switch (var->Type()) {
        case GDL_UNDEF:
          v->type = IDL_TYP_UNDEF;
          v->flags |= IDL_V_NULL;
          break;
        case GDL_BYTE:
          v->type = IDL_TYP_BYTE;
          v->value.c = (*static_cast<DByteGDL*> (var))[0];
          break;
        case GDL_INT:
          v->type = IDL_TYP_INT;
          v->value.i = (*static_cast<DIntGDL*> (var))[0];
          break;
        case GDL_LONG:
          v->type = IDL_TYP_LONG;
          v->value.l = (*static_cast<DLongGDL*> (var))[0];
          break;
        case GDL_LONG64:
          v->type = IDL_TYP_LONG64;
          v->value.l64 = (*static_cast<DLong64GDL*> (var))[0];
          break;
        case GDL_UINT:
          v->type = IDL_TYP_UINT;
          v->value.ui = (*static_cast<DUIntGDL*> (var))[0];
          break;
        case GDL_ULONG:
          v->type = IDL_TYP_ULONG;
          v->value.ul = (*static_cast<DULongGDL*> (var))[0];
          break;
        case GDL_ULONG64:
          v->type = IDL_TYP_ULONG64;
          v->value.ul64 = (*static_cast<DULong64GDL*> (var))[0];
          break;
        case GDL_FLOAT:
          v->type = IDL_TYP_FLOAT;
          v->value.f = (*static_cast<DFloatGDL*> (var))[0];
          break;
        case GDL_DOUBLE:
          v->type = IDL_TYP_DOUBLE;
          v->value.d = (*static_cast<DDoubleGDL*> (var))[0];
          break;
        case GDL_COMPLEX:
        {
          v->type = IDL_TYP_COMPLEX;
          DComplex c = (*static_cast<DComplexGDL*> (var))[0];
          v->value.cmp.r = c.real();
          v->value.cmp.i = c.imag();
          break;
        }
        case GDL_COMPLEXDBL:
        {
          v->type = IDL_TYP_DCOMPLEX;
          DComplexDbl c = (*static_cast<DComplexDblGDL*> (var))[0];
          v->value.dcmp.r = c.real();
          v->value.dcmp.i = c.imag();
          break;
        }
        case GDL_STRING:
        {
          v->type = IDL_TYP_STRING;
          v->flags |= IDL_V_DYNAMIC;
          DString s = (*static_cast<DStringGDL*> (var))[0];
		  v->value.str.slen=s.size();
		  v->value.str.s=(char*) s.c_str();
          break;
        }
        case GDL_STRUCT:
        {
          v->type = IDL_TYP_STRUCT;
          v->flags |= IDL_V_DYNAMIC;
          v->flags |= IDL_V_ARR;
GDL_WillThrowAfterCleaning("GDL_ToVPTR: do not support STRUCT yet.");
          break;
        }
		  case GDL_PTR:
        {
          v->type = IDL_TYP_PTR;
		  v->value.ptrint=(*static_cast<DPtrGDL*>(var))[0];
          break;
        }
		  default: GDL_WillThrowAfterCleaning("GDL_ToVPTR: unsupported case.");
      }
    } else {
      v->flags |= IDL_V_ARR | IDL_V_DYNAMIC;
      IDL_ARRAY* arraydescr=new IDL_ARRAY();
      arraydescr->arr_len = var->NBytes();
      arraydescr->data = (UCHAR*) (var->DataAddr());
      for (int i = 0; i < var->Rank(); ++i) arraydescr->dim[i] = var->Dim(i);
      arraydescr->n_dim = var->Rank();
      arraydescr->n_elts = var->N_Elements();
      arraydescr->offset = 0;
      v->value.arr = arraydescr;
      switch (var->Type()) {
        case GDL_UNDEF:
          v->type = IDL_TYP_UNDEF;
          arraydescr->elt_len = 0;
          break;
        case GDL_BYTE:
          v->type = IDL_TYP_BYTE;
          arraydescr->elt_len = 1;
          break;
        case GDL_INT:
          v->type = IDL_TYP_INT;
          arraydescr->elt_len = 2;
          break;
        case GDL_LONG:
          v->type = IDL_TYP_LONG;
          arraydescr->elt_len = 4;
          break;
        case GDL_LONG64:
          v->type = IDL_TYP_LONG64;
          arraydescr->elt_len = 8;
          break;
        case GDL_UINT:
          v->type = IDL_TYP_UINT;
          arraydescr->elt_len = 2;
          break;
        case GDL_ULONG:
          v->type = IDL_TYP_ULONG;
          arraydescr->elt_len = 4;
          break;
        case GDL_ULONG64:
          v->type = IDL_TYP_ULONG64;
          arraydescr->elt_len = 8;
          break;
        case GDL_FLOAT:
          v->type = IDL_TYP_FLOAT;
          arraydescr->elt_len = 4;
          break;
        case GDL_DOUBLE:
          v->type = IDL_TYP_DOUBLE;
          arraydescr->elt_len = 8;
          break;
        case GDL_COMPLEX:
        {
          v->type = IDL_TYP_COMPLEX;
          arraydescr->elt_len = 8;
          break;
        }
        case GDL_COMPLEXDBL:
        {
          v->type = IDL_TYP_DCOMPLEX;
          arraydescr->elt_len = 16;
          break;
        }
        case GDL_STRING:
		{ //TBC: create a serie of IDL_STRINGS, allocate all corresponding C char* strings, copy them, put all the newly allocated mem into the delete-after list.
			SizeT nEl=arraydescr->n_elts;
			void* allstringdescr=malloc(nEl*sizeof(IDL_STRING));
			memset(allstringdescr,0,nEl*sizeof(IDL_STRING));
			void* stringdescPtrs=malloc(nEl*sizeof(IDL_STRING*));
			arraydescr->data = (UCHAR*) (stringdescPtrs);
			IDL_STRING** p=(IDL_STRING**)stringdescPtrs;
			for (SizeT i=0; i< nEl; ++i) p[i]=(IDL_STRING*)((SizeT)allstringdescr+(i*sizeof(IDL_STRING)));
			DStringGDL* gdlstr=(DStringGDL*)var;
			for (auto i=0; i< nEl; ++i) {
				p[i]->slen = ((*gdlstr)[i]).size();
				p[i]->s = (char*) malloc(p[i]->slen + 1);
				strncpy(p[i]->s, (*gdlstr)[i].c_str(), p[i]->slen + 1);
			}
          break;
        }
        case GDL_STRUCT:
		{ //TBC: create a serie of IDL_SREFs, allocate all corresponding C char* strings, copy them, put all the newly allocated mem into the delete-after list.
            v->flags |= IDL_V_STRUCT;
		GDL_WillThrowAfterCleaning("GDL_ToVPTR: unsupported case: STRUCT.");
//			SizeT nEl=arraydescr->n_elts;
//			void* allstringdescr=MyMemAlloc(nEl*sizeof(IDL_STRING));
//			memset(allstringdescr,0,nEl*sizeof(IDL_STRING));
//			void* stringdescPtrs=MyMemAlloc(nEl*sizeof(IDL_STRING*));
//			arraydescr->data = (UCHAR*) (stringdescPtrs);
//			IDL_STRING** p=(IDL_STRING**)stringdescPtrs;
//			for (SizeT i=0; i< nEl; ++i) p[i]=(IDL_STRING*)((SizeT)allstringdescr+(i*sizeof(IDL_STRING)));
//			DStringGDL* gdlstr=(DStringGDL*)var;
//			for (auto i=0; i< nEl; ++i) {
//				p[i]->slen = ((*gdlstr)[i]).size();
//				p[i]->s = (char*) MyMemAlloc(p[i]->slen + 1);
//				strncpy(p[i]->s, (*gdlstr)[i].c_str(), p[i]->slen + 1);
          break;
        }
		  default: GDL_WillThrowAfterCleaning("GDL_ToVPTR: unsupported case.");

      }
    }
    return v;
  } 
  
	
// from RESTORE	
  void restoreNormalVariable(std::string varName, BaseGDL* ret) {
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
  
DStringGDL* GDL_GetString(IDL_VPTR v) {
 		  dimension *dim;
		  IDL_STRING* ss;
		if (v->flags & IDL_V_ARR) {
			IDL_ARRAY* arraydescr = v->value.arr;
			SizeT rank = arraydescr->n_dim;
			SizeT arraydim[rank];
			for (int i = 0; i < rank; ++i) arraydim[i] = arraydescr->dim[i];
			dim = new dimension(arraydim, rank);
			ss=(IDL_STRING*)(v->value.arr->data);
	    } else {
			dim = new  dimension(1);
			ss=&(v->value.str);
		}
		  DStringGDL* gdls=new DStringGDL(*dim,BaseGDL::NOZERO);
//strings copy data
			for (auto i=0; i< dim->NDimElements(); ++i) (*gdls)[i]=std::string(ss[i].s,ss[i].slen);
//thus, delete v if temp, it deletes the original data, we're left with a copy			
			IDL_Deltmp(v);
			return gdls;
}

void fillVariableData(void* baseData, IDL_VPTR v, int t, BaseGDL* var) {
	SizeT nEl = var->N_Elements();
	IDL_MEMINT off = v->value.s.sdef->tags[t].offset; 
	void* dataset = (void*) ((IDL_MEMINT) baseData + off); //fprintf(stderr,"%s at addr #%lld :\n",v->value.s.sdef->tags[t].id->name,dataset);
	if (v->value.s.sdef->tags[t].var.flags & IDL_V_STRUCT) {
		DStructGDL* myStruct = static_cast<DStructGDL*> (var);
		u_int nEl = myStruct->N_Elements();
		SizeT nTags = myStruct->Desc()->NTags();
		for (SizeT ix = 0; ix < nEl; ++ix) for (SizeT tt = 0; tt < nTags; ++tt) fillVariableData(dataset, &(v->value.s.sdef->tags[t].var), tt, myStruct->GetTag(tt, ix));
	} else if (v->value.s.sdef->tags[t].var.flags & IDL_V_ARR) {
		if (v->value.s.sdef->tags[t].var.type == IDL_TYP_STRING) {
			IDL_STRING* ss = (IDL_STRING*) (v->value.arr->data + v->value.s.sdef->tags[t].offset);
			for (auto i = 0; i < nEl; ++i) (*static_cast<DStringGDL*> (var))[i] = std::string(ss[i].s, ss[i].slen);
		} else memcpy(var->DataAddr(), dataset, v->value.s.sdef->tags[t].var.value.arr->arr_len);
	} else if (v->value.s.sdef->tags[t].var.type == IDL_TYP_STRING) {
		IDL_STRING* ss = (IDL_STRING*) (v->value.arr->data + v->value.s.sdef->tags[t].offset);
		(*static_cast<DStringGDL*> (var))[0] = std::string(ss->s, ss->slen);
	} else memcpy(var->DataAddr(), dataset, IDL_TypeSize[v->value.s.sdef->tags[t].var.type]);
}
  
#define DOCASE(type, gdltype, tagname, pardim)\
 case type: { gdltype entry(pardim); stru_desc->AddTag(std::string(tagname), &entry);} break;

DStructGDL* GDL_MakeStruct(IDL_VPTR v, dimension &inputdim);
DStructDesc * GDL_GetStructDesc(IDL_VPTR v, dimension &inputdim) {
	std::string struname("$truct");
	if (v->value.s.sdef->id != NULL && v->value.s.sdef->id->name != NULL) struname=std::string(v->value.s.sdef->id->name,v->value.s.sdef->id->len); 
	DStructDesc * stru_desc = new DStructDesc(struname);
	//summary & tag population:
	for (int i = 0; i < v->value.s.sdef->ntags; ++i) {
		dimension *dim;
		if (v->value.s.sdef->tags[i].var.flags & IDL_V_ARR) {
			IDL_ARRAY* arraydescr = v->value.s.sdef->tags[i].var.value.arr;
			SizeT rank = arraydescr->n_dim;
			SizeT arraydim[rank];
			for (int i = 0; i < rank; ++i) arraydim[i] = arraydescr->dim[i];
			dim = new dimension(arraydim, rank);
		} else dim = new dimension(1);
		switch (v->value.s.sdef->tags[i].var.type) {
				DOCASE(IDL_TYP_BYTE, SpDByte, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_INT, SpDInt, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_LONG, SpDLong, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_FLOAT, SpDFloat, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_DOUBLE, SpDDouble, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_COMPLEX, SpDComplex, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_DCOMPLEX, SpDComplexDbl, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_STRING, SpDString, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_PTR, DPtrGDL, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_OBJREF, DObjGDL, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_UINT, SpDUInt, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_ULONG, SpDULong, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_ULONG64, SpDULong64, v->value.s.sdef->tags[i].id->name, *dim)
				DOCASE(IDL_TYP_LONG64, SpDLong64, v->value.s.sdef->tags[i].id->name, *dim)

			case IDL_TYP_STRUCT:
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

DStructGDL* GDL_MakeStruct(IDL_VPTR v, dimension &inputdim) {
	DStructDesc * stru_desc = GDL_GetStructDesc(v, inputdim);
	DStructGDL* var = new DStructGDL(stru_desc, inputdim);
	u_int nEl = var->N_Elements();
	SizeT nTags = var->Desc()->NTags();
	IDL_MEMINT running_offset=0;
	for (SizeT ix = 0; ix < nEl; ++ix)
		for (SizeT t = 0; t < nTags; ++t) fillVariableData((void*)v->value.s.arr->data,v, t, var->GetTag(t, ix));

	return var;

}

#define DOCASE(type, gdltype, element)\
 case type: return new gdltype(v->value.element); break;

#define DOCASE_ARRAY(type, gdltype)\
 case type: var = new gdltype(dim, BaseGDL::NOALLOC); break;

  
BaseGDL* VPTR_ToGDL(IDL_VPTR v, bool protect=false) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (v == NULL) {
		GDL_WillThrowAfterCleaning("Invalid IDL_VPTR used.");
	}
	if (v->type == IDL_TYP_UNDEF) {
		return NullGDL::GetSingleInstance();
	}
	if (v->flags & IDL_V_NULL) {
		return NullGDL::GetSingleInstance();
	}
	if (v->flags & IDL_V_ARR) {
		if (protect) v->flags &= ~IDL_V_TEMP; //will no destroy data
		IDL_ARRAY* arraydescr = v->value.arr;
		SizeT rank = arraydescr->n_dim;
		SizeT arraydim[rank];
		for (int i = 0; i < rank; ++i) arraydim[i] = arraydescr->dim[i];
		dimension dim(arraydim, rank);
		BaseGDL* var;
		switch (v->type) {
				DOCASE_ARRAY(IDL_TYP_BYTE, DByteGDL);
				DOCASE_ARRAY(IDL_TYP_INT, DIntGDL);
				DOCASE_ARRAY(IDL_TYP_LONG, DLongGDL);
				DOCASE_ARRAY(IDL_TYP_FLOAT, DFloatGDL);
				DOCASE_ARRAY(IDL_TYP_DOUBLE, DDoubleGDL);
				DOCASE_ARRAY(IDL_TYP_COMPLEX, DComplexGDL);
				DOCASE_ARRAY(IDL_TYP_DCOMPLEX, DComplexDblGDL);
				DOCASE_ARRAY(IDL_TYP_UINT, DUIntGDL);
				DOCASE_ARRAY(IDL_TYP_ULONG, DULongGDL);
				DOCASE_ARRAY(IDL_TYP_LONG64, DLong64GDL);
				DOCASE_ARRAY(IDL_TYP_ULONG64, DULong64GDL);
			case IDL_TYP_STRING:
				return GDL_GetString(v);
			    break;
			case IDL_TYP_STRUCT: 
				return GDL_MakeStruct(v, dim);
			    break;
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
				DOCASE(IDL_TYP_BYTE, DByteGDL, c);
				DOCASE(IDL_TYP_INT, DIntGDL, i);
				DOCASE(IDL_TYP_LONG, DLongGDL, l);
				DOCASE(IDL_TYP_FLOAT, DFloatGDL, f);
				DOCASE(IDL_TYP_DOUBLE, DDoubleGDL, d);
				DOCASE(IDL_TYP_UINT, DUIntGDL, ui);
				DOCASE(IDL_TYP_ULONG, DULongGDL, ul);
				DOCASE(IDL_TYP_LONG64, DLong64GDL, l64);
				DOCASE(IDL_TYP_ULONG64, DULong64GDL, ul64);
			case IDL_TYP_STRING:
				return GDL_GetString(v);
				break;
			case IDL_TYP_COMPLEX:
				return new DComplexGDL(std::complex<float>(v->value.cmp.r, v->value.cmp.i));
				break;
			case IDL_TYP_DCOMPLEX:
				return new DComplexDblGDL(std::complex<double>(v->value.dcmp.r, v->value.dcmp.i));
				break;
			case IDL_TYP_STRUCT: {
				dimension dim(1);
				return GDL_MakeStruct(v, dim);
				break;
			}
			default: GDL_WillThrowAfterCleaning("ReturnIDL_VPTR_AsGDL: bad array case.");
		}
	}
	throw;
}
#undef DOCASE
#undef DOCASE_ARRAY

#define C_ (char*)
char *IDL_OutputFormat[IDL_NUM_TYPES]={C_ "<Undefined>",C_ "%4d",C_ "%8d",C_ "%12d",C_ "%#13.6g",C_ "%#16.8g",
C_ "(%#13.6g,%#13.6g)",C_ "%s",C_ "",C_ "(%#16.8g,%#16.8g)",C_ "%12lu",C_ "%12lu",C_ "%8u",C_ "%12u",C_ "%22ld"};
char *IDL_CDECL IDL_OutputFormatFunc(int type){
		if (type > IDL_MAX_TYPE) GDL_WillThrowAfterCleaning("type must be > 0 and < 15");
		return IDL_OutputFormat[type];
}
int IDL_OutputFormatLen[IDL_NUM_TYPES]={11,4,8,12,13,16,29,0,0,35,12,12,8,12,22};
int IDL_CDECL IDL_OutputFormatLenFunc(int type){
		if (type > IDL_MAX_TYPE) GDL_WillThrowAfterCleaning("type must be > 0 and < 15");
		return IDL_OutputFormatLen[type];
}
char *IDL_OutputFormatFull[IDL_NUM_TYPES]={C_"<Undefined>",C_"%4d",C_"%8d",C_"%12d",C_"%#16.8g",C_"%#25.17g",
C_"(%#16.8g,%#16.8g)",C_"%s",C_"",C_"(%#25.17g,%#25.17g)",C_"%12lu",C_"%12lu",C_"%8u",C_"%12u",C_"%22ld"};
int IDL_OutputFormatFullLen[IDL_NUM_TYPES]={11,4,8,12,16,25,35,0,0,53,12,12,8,12,22};

char *IDL_OutputFormatNatural[IDL_NUM_TYPES]={C_"<Undefined>",C_"%d",C_"%d",C_"%d",C_"%g",C_"%g",C_"(%g,%g)",C_"%s",C_"",C_"(%g,%g)",C_"%u",C_"%u",C_"%u",C_"%u",C_"%ld"};

	IDL_LONG IDL_TypeSize[IDL_NUM_TYPES]={0, 1, sizeof (IDL_INT), sizeof (IDL_LONG), sizeof (float), sizeof (double),
		sizeof (IDL_COMPLEX), sizeof (IDL_STRING), sizeof (IDL_SREF), sizeof (IDL_DCOMPLEX), sizeof (IDL_MEMINT), sizeof (IDL_MEMINT),
		sizeof ( IDL_UINT), sizeof (IDL_ULONG), sizeof (IDL_LONG64), sizeof (IDL_ULONG64)};
	
	IDL_LONG GDL_TypeAlignment[IDL_NUM_TYPES]={0, 1, sizeof (IDL_INT), sizeof (IDL_LONG), sizeof (float), sizeof (double),
		sizeof (float), sizeof (IDL_MEMINT), sizeof (IDL_MEMINT), sizeof (double), sizeof (IDL_MEMINT), sizeof (IDL_MEMINT),
		sizeof ( IDL_UINT), sizeof (IDL_ULONG), sizeof (IDL_LONG64), sizeof (IDL_ULONG64)};
	
	int IDL_TypeSizeFunc(int type){
		if (type > IDL_MAX_TYPE) GDL_WillThrowAfterCleaning("type must be > 0 and < 15");
		return IDL_TypeSize[type];}

char *IDL_TypeName[]={C_"UNDEFINED",C_"BYTE     ",C_"INT      ",C_"LONG     ",C_"FLOAT    ",C_"DOUBLE   ",C_"COMPLEX  ",C_"STRING   ",C_"STRUCT   ",C_"DCOMPLEX ",C_"POINTER  ",C_"OBJREF   ",C_"UINT     ",C_"ULONG    ",C_"LONG64   "};
char *IDL_CDECL IDL_TypeNameFunc(int type){
		if (type > IDL_MAX_TYPE) GDL_WillThrowAfterCleaning("type must be > 0 and < 15");
		return IDL_TypeName[type];}
#undef C_

IDL_VPTR IDL_CDECL IDL_ImportArray(int n_dim, IDL_MEMINT dim[], int type, UCHAR *data, IDL_ARRAY_FREE_CB free_cb,  IDL_StructDefPtr s){
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)

	if (type == IDL_TYP_STRUCT){
	 if (s==NULL) GDL_WillThrowAfterCleaning("IDL_ImportArray() defines a struct without passing a valid IDL_StructDefPtr");
	 IDL_VPTR v= NewTMPVPTRSTRUCTWithCB(s,free_cb);
		v->type = type;
		SizeT l = 1;
		for (auto i = 0; i < n_dim; ++i) {
			l *= dim[i];
			v->value.arr->dim[i] = dim[i];
		}
		v->value.s.arr->n_elts = l;
		v->value.s.arr->n_dim = n_dim;
		IDL_LONG64 sz = IDL_TypeSizeFunc(type);
		v->value.s.arr->elt_len = sz;
		v->value.s.arr->arr_len = sz*l;
		v->value.s.arr->data = data;
	    v->value.s.sdef=s;
		return v;
	} else {	
		IDL_VPTR v= NewTMPVPTRARRAYWithCB(free_cb);
		v->type = type;
		SizeT l = 1;
		for (auto i = 0; i < n_dim; ++i) {
			l *= dim[i];
			v->value.arr->dim[i] = dim[i];
		}
		v->value.arr->n_elts = l;
		v->value.arr->n_dim = n_dim;
		IDL_LONG64 sz = IDL_TypeSizeFunc(type);
		v->value.arr->elt_len = sz;
		v->value.arr->arr_len = sz*l;
		v->value.arr->data = data;
		return v;
	}
}

IDL_VPTR IDL_CDECL IDL_ImportNamedArray(char *name, int n_dim, IDL_MEMINT dim[],  int type, UCHAR *data,  IDL_ARRAY_FREE_CB free_cb, IDL_StructDefPtr s){
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	IDL_VPTR v=IDL_ImportArray(n_dim, dim,  type, data, free_cb, s);		
	BaseGDL* gdlvar = VPTR_ToGDL(v, true); //protect data as this is passed to *MAIN* GDL
	restoreNormalVariable(std::string(name), gdlvar);
	return v;
}
 
#define DOCASE(type, gdltype)\
 case type: return new gdltype(d, mode); break;

  BaseGDL* CreateNewGDLArray(int type, SizeT n_dim, SizeT dim[], BaseGDL::InitType mode) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	  dimension d(dim, n_dim);
	switch (type) {
			DOCASE(IDL_TYP_BYTE, DByteGDL);
			DOCASE(IDL_TYP_INT, DIntGDL);
			DOCASE(IDL_TYP_LONG, DLongGDL);
			DOCASE(IDL_TYP_FLOAT,DFloatGDL);
			DOCASE(IDL_TYP_DOUBLE, DDoubleGDL);
			DOCASE(IDL_TYP_COMPLEX, DComplexGDL);
			DOCASE(IDL_TYP_DCOMPLEX, DComplexDblGDL);
			DOCASE(IDL_TYP_UINT, DUIntGDL);
			DOCASE(IDL_TYP_ULONG, DULongGDL);
			DOCASE(IDL_TYP_LONG64, DLong64GDL);
			DOCASE(IDL_TYP_ULONG64, DULong64GDL);
			DOCASE(IDL_TYP_STRING, DStringGDL);
		default:
			GDL_WillThrowAfterCleaning("CreateNewGDLArray failure.");
	}
	throw;
}
#undef DOCASE
  // for use inside source of LINKIMAGE and other DLMs
extern "C" {
	//all the IDL_** modules below should be listed in dynlist.txt ---now done using an asterisk syntax 
	
#define DOCASE(ty, what)\
 case ty: {snprintf (&infoline[l], IDL_OutputFormatLen[v->type], IDL_OutputFormat[v->type],v->value.what);break;}
#define DOCASE_CMP(ty, what)\
 case ty: {snprintf (&infoline[l], IDL_OutputFormatLen[v->type], IDL_OutputFormat[v->type],v->value.what.r,v->value.what.i);  break;}

IDL_VPTR IDL_FindNamedVariable(char *name, int ienter){
	std::string s(name);
    for (std::vector<std::pair <IDL_VPTR, std::string>>::iterator it = ExportedNamesList.begin(); it != ExportedNamesList.end(); ++it) {
				if (it->second == s) {
					return it->first;
					break;
				}
	}
	return NULL;
}

char *IDL_CDECL IDL_VarName(IDL_VPTR v){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		char* infoline=(char*) calloc(1,128);

		if (v->type == IDL_TYP_UNDEF) {strncat(infoline,"<UNDEFINED> ",13); return infoline;}

        if ((v->flags & IDL_V_TEMP)==0) {
			for (std::vector<std::pair <IDL_VPTR, std::string>>::iterator it = ExportedNamesList.begin(); it != ExportedNamesList.end(); ++it) {
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
		if (v->flags & IDL_V_ARR) {
			strncat(infoline,"Array[",7);
			int i;
			for (i=0; i< v->value.arr->n_dim-1; ++i) {
				int l=strlen(infoline);
				snprintf (&infoline[l], 127-l, "%d,",v->value.arr->dim[i]);
			}
			int l = strlen(infoline);
			snprintf(&infoline[l], 127-l, "%d", v->value.arr->dim[i]);
			strncat(infoline,"]",2);
		} else {
			strncat(infoline,"(",2);
			int l = strlen(infoline);
			switch(v->type){
				DOCASE(IDL_TYP_BYTE, c);
				DOCASE(IDL_TYP_INT, i);
				DOCASE(IDL_TYP_LONG, l);
				DOCASE(IDL_TYP_FLOAT, f);
				DOCASE(IDL_TYP_DOUBLE, d);
				DOCASE_CMP(IDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(IDL_TYP_DCOMPLEX, dcmp);
				DOCASE(IDL_TYP_UINT, ui);
				DOCASE(IDL_TYP_ULONG, ul);
				DOCASE(IDL_TYP_LONG64, l64);
				DOCASE(IDL_TYP_ULONG64, ul64);
				DOCASE(IDL_TYP_STRING, str.s);
				DOCASE(IDL_TYP_STRUCT, s.sdef->id->name);
			default: Warning/*GDL_WillThrowAfterCleaning*/("IDL_VarName: unexpected type "+i2s(v->type));
			}
			strncat(infoline,")",2);
		}
		return infoline;
	}
#undef DOCASE
#undef DOCASE_CMP

IDL_VPTR IDL_GetVarAddr1(char *name, int enter){
	GDL_WillThrowAfterCleaning("IDL_GetVarAddr is not currently programmed -- as it would never be the address of a real GDL variable. Use parameters in call to get a copy of GDL variables.");
	return NULL;
}

IDL_VPTR IDL_GetVarAddr(char *name){
	return IDL_GetVarAddr1(name, 0);
}

void IDL_VarEnsureSimple(IDL_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
if (v->flags == 0) return;
    static char* message= (char*)"Expression must be a scalar in this context: ";
	if ( !( v->flags & IDL_TYP_B_SIMPLE) || ( v->flags & IDL_V_ARR) ) GDL_WillThrowAfterCleaning(message+std::string(IDL_VarName(v)));
}
 #define DOCASE(ty, what)\
 case ty: dprintf (fd,IDL_OutputFormat[v->type],v->value.what);break;

#define DOCASE_ARRAY(ty, c_ty)\
 case ty: {\
 c_ty *val=(c_ty *) v->value.arr->data;\
 int i=0;\
 int w=0;\
 int l=IDL_OutputFormatLen[v->type];\
 for (; i< v->value.arr->n_elts; ++i) {\
 w+=l;\
 dprintf (fd,IDL_OutputFormat[v->type],val[i]);\
 if (w>=ncols) {dprintf (fd,"\n");w=0;} }\
 dprintf (fd,"\n");}\
 break;

	void IDL_CDECL GDL_Print(int argc, IDL_VPTR *argv, char *argk, bool print_to_file) {
				//argk is to be set to NULL by users according to the doc.
		int start=0;
		int fd=0;
		if (print_to_file) {
			IDL_LONG fd=argv[0]->value.l;
			start=1;
		}
		int ncols= TermWidth();
		for (int iarg = start; iarg < argc; ++iarg) {
			IDL_VPTR v = argv[iarg];
            checkOK(v);
            if (v->flags & IDL_V_ARR) {
				switch (v->type) {
						DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
						DOCASE_ARRAY(IDL_TYP_INT, IDL_INT);
						DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG);
						DOCASE_ARRAY(IDL_TYP_FLOAT, float);
						DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
						DOCASE_ARRAY(IDL_TYP_COMPLEX, IDL_COMPLEX);
						DOCASE_ARRAY(IDL_TYP_DCOMPLEX, IDL_DCOMPLEX);
						DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT);
						DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
						DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
						DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
						DOCASE_ARRAY(IDL_TYP_STRING, IDL_STRING);
					default: GDL_WillThrowAfterCleaning("IDL_Print: unexpected type " + i2s(v->type));
				}
			} else {
				switch (v->type) {
						DOCASE(IDL_TYP_BYTE, c);
						DOCASE(IDL_TYP_INT, i);
						DOCASE(IDL_TYP_LONG, l);
						DOCASE(IDL_TYP_FLOAT, f);
						DOCASE(IDL_TYP_DOUBLE, d);
						DOCASE(IDL_TYP_COMPLEX, cmp);
						DOCASE(IDL_TYP_DCOMPLEX, dcmp);
						DOCASE(IDL_TYP_UINT, ui);
						DOCASE(IDL_TYP_ULONG, ul);
						DOCASE(IDL_TYP_LONG64, l64);
						DOCASE(IDL_TYP_ULONG64, ul64);
						DOCASE(IDL_TYP_STRING, str);
					default: GDL_WillThrowAfterCleaning("IDL_Print: unexpected type " + i2s(v->type));
				}
			}
		}
	}
#undef DOCASE
#undef DOCASE_ARRAY
	void IDL_CDECL IDL_Print(int argc, IDL_VPTR *argv, char *argk) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		GDL_Print(argc, argv, argk, false);
	}
	void IDL_CDECL IDL_PrintF(int argc, IDL_VPTR *argv, char *argk) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		GDL_Print(argc, argv, argk, true);
	}

void IDL_CDECL IDL_StrStore(IDL_STRING *s, const char *fs){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	s->slen=strlen(fs);
	s->stype=0;
	s->s=(char*)malloc(s->slen);
	strncpy(s->s,fs,s->slen);
}
// str below is a supposed to be a copy of the string descriptor(s).
// to properly duplicate, one has to create copies of the string(s) and update the descriptor(s)
void IDL_CDECL IDL_StrDup(IDL_REGISTER IDL_STRING *str, IDL_REGISTER IDL_MEMINT n){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	for (auto i=0; i< n; ++i) {
		if (str[i].slen > 0) {
			char* news=(char*)malloc(str[i].slen+1);
			strncpy(news,str[i].s,str[i].slen+1);
			str[i].s=news;
		}
	}
}

void IDL_CDECL IDL_StrDelete(IDL_STRING *str, IDL_MEMINT n) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	for (auto i=0; i< n; ++i) if (str[i].slen > 0) free(str[i].s);
	}
// n: The number of characters the string must be able to contain, not including the terminating NULL character.
	void IDL_CDECL IDL_StrEnsureLength(IDL_STRING *s, int n) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		if (s->slen < n) {
			IDL_StrDelete(s, 1);
			s->s = (char*) malloc(n+1);
			memset((void*)s->s,0,n+1);
		}
	}
IDL_VPTR IDL_CDECL IDL_StrToSTRING(const char *s) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_VPTR ret = NewTMPVPTR();
		ret->type = IDL_TYP_STRING;
		ret->flags = IDL_V_TEMP | IDL_V_DYNAMIC;
		ret->value.str.slen = strlen(s);
		ret->value.str.stype = 0;
		ret->value.str.s=(char*)s;
		return ret;
	}

#define DOCASE(type, what)\
 case type: {dest->value.what=value->what; break;}
#define DOCASE_CMP(type, what)\
 case type: {dest->value.what.r=value->what.r; dest->value.what.i=value->what.i;  break;}

void IDL_CDECL IDL_StoreScalar(IDL_VPTR dest, int type,	IDL_ALLTYPES * value) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	IDL_ENSURE_SIMPLE(dest);
		dest->type=type;
		switch (type) {
				DOCASE(IDL_TYP_BYTE, c);
				DOCASE(IDL_TYP_INT, i);
				DOCASE(IDL_TYP_LONG, l);
				DOCASE(IDL_TYP_FLOAT, f);
				DOCASE(IDL_TYP_DOUBLE, d);
				DOCASE_CMP(IDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(IDL_TYP_DCOMPLEX, dcmp);
				DOCASE(IDL_TYP_UINT, ui);
				DOCASE(IDL_TYP_ULONG, ul);
				DOCASE(IDL_TYP_LONG64, l64);
				DOCASE(IDL_TYP_ULONG64, ul64);
			default: GDL_WillThrowAfterCleaning("IDL_StoreScalar: unexpected type "+i2s(type));
		}
	}
#undef DOCASE
#undef DOCASE_CMP

#define DOCASE(type, what)\
 case type: {dest->value.what=0; break;}
#define DOCASE_CMP(type, what)\
 case type: {dest->value.what.r=0; dest->value.what.i=0;  break;}

void IDL_CDECL IDL_StoreScalarZero(IDL_VPTR dest, int type) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	IDL_ENSURE_SIMPLE(dest);
		dest->type=type;
		switch (type) {
				DOCASE(IDL_TYP_BYTE, c);
				DOCASE(IDL_TYP_INT, i);
				DOCASE(IDL_TYP_LONG, l);
				DOCASE(IDL_TYP_FLOAT, f);
				DOCASE(IDL_TYP_DOUBLE, d);
				DOCASE_CMP(IDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(IDL_TYP_DCOMPLEX, dcmp);
				DOCASE(IDL_TYP_UINT, ui);
				DOCASE(IDL_TYP_ULONG, ul);
				DOCASE(IDL_TYP_LONG64, l64);
				DOCASE(IDL_TYP_ULONG64, ul64);
			default: GDL_WillThrowAfterCleaning("IDL_StoreScalar: unexpected type "+i2s(type));
		}
	}
#undef DOCASE
#undef DOCASE_CMP
	//scalar only

	void IDL_CDECL IDL_VarCopy(IDL_REGISTER IDL_VPTR src, IDL_REGISTER
	IDL_VPTR dst) {
		if (dst->value.arr != NULL) {
			free(dst->value.arr->data);
			dst->value.arr = NULL;
		}
		dst->type = src->type;
		dst->value = src->value; //copy pointers.
		if (src->flags & IDL_V_STRUCT) { //must copy
			GDL_WillThrowAfterCleaning("IDL_VarCopy not yet ready for structure, FIXME.");
		} else if (src->flags & IDL_V_ARR) { //must copy
			IDL_ARRAY *a = new IDL_ARRAY();
			a->flags = IDL_A_NO_GUARD;
			dst->value.arr = a;
			dst->value.arr->n_dim = src->value.arr->n_dim;
			dst->value.arr->n_elts = src->value.arr->n_elts;
			for (auto i = 0; i < src->value.arr->n_dim; ++i) dst->value.arr->dim[i] = src->value.arr->dim[i];
			dst->value.arr->elt_len = IDL_TypeSizeFunc(src->type);
			dst->value.arr->arr_len = src->value.arr->n_elts * dst->value.arr->elt_len;
			if (src->flags & IDL_V_TEMP) {
				dst->value.arr->data = src->value.arr->data; //do not copy if src is temp, just pass.
			} else {
				void * addr = malloc(dst->value.arr->arr_len);
				dst->value.arr->data = (UCHAR*) addr;
				memcpy(dst->value.arr->data, src->value.arr->data, dst->value.arr->arr_len);
			}
			if (src->flags & IDL_V_TEMP) {
				src->flags = IDL_V_TEMP; //only that.
				src->type = IDL_TYP_UNDEF;
			}
		}
	}

#define DOCASE(rettype, type, what)\
 case type: {return (rettype) (v->value.what); break;}
#define DOCASE_CMP(rettype,type, what)\
 case type: {return (rettype) (v->value.what.r);  break;}

double IDL_CDECL IDL_DoubleScalar(IDL_REGISTER IDL_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(double, IDL_TYP_BYTE, c);
				DOCASE(double, IDL_TYP_INT, i);
				DOCASE(double, IDL_TYP_LONG, l);
				DOCASE(double, IDL_TYP_FLOAT, f);
				DOCASE(double, IDL_TYP_DOUBLE, d);
				DOCASE_CMP(double, IDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(double, IDL_TYP_DCOMPLEX, dcmp);
				DOCASE(double, IDL_TYP_UINT, ui);
				DOCASE(double, IDL_TYP_ULONG, ul);
				DOCASE(double, IDL_TYP_LONG64, l64);
				DOCASE(double, IDL_TYP_ULONG64, ul64);
			default: GDL_WillThrowAfterCleaning("IDL_DoubleScalar: unexpected type "+i2s(v->type));
		}
		throw;
	}

IDL_ULONG IDL_CDECL IDL_ULongScalar(IDL_REGISTER IDL_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(IDL_ULONG, IDL_TYP_BYTE, c);
				DOCASE(IDL_ULONG, IDL_TYP_INT, i);
				DOCASE(IDL_ULONG, IDL_TYP_LONG, l);
				DOCASE(IDL_ULONG, IDL_TYP_FLOAT, f);
				DOCASE(IDL_ULONG, IDL_TYP_DOUBLE, d);
				DOCASE_CMP(IDL_ULONG, IDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(IDL_ULONG, IDL_TYP_DCOMPLEX, dcmp);
				DOCASE(IDL_ULONG, IDL_TYP_UINT, ui);
				DOCASE(IDL_ULONG, IDL_TYP_ULONG, ul);
				DOCASE(IDL_ULONG, IDL_TYP_LONG64, l64);
				DOCASE(IDL_ULONG, IDL_TYP_ULONG64, ul64);
			default: GDL_WillThrowAfterCleaning("IDL_ULongScalar: unexpected type "+i2s(v->type));
		}
		throw;
	}
IDL_LONG IDL_CDECL IDL_LongScalar(IDL_REGISTER IDL_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(IDL_LONG, IDL_TYP_BYTE, c);
				DOCASE(IDL_LONG, IDL_TYP_INT, i);
				DOCASE(IDL_LONG, IDL_TYP_LONG, l);
				DOCASE(IDL_LONG, IDL_TYP_FLOAT, f);
				DOCASE(IDL_LONG, IDL_TYP_DOUBLE, d);
				DOCASE_CMP(IDL_LONG, IDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(IDL_LONG, IDL_TYP_DCOMPLEX, dcmp);
				DOCASE(IDL_LONG, IDL_TYP_UINT, ui);
				DOCASE(IDL_LONG, IDL_TYP_ULONG, ul);
				DOCASE(IDL_LONG, IDL_TYP_LONG64, l64);
				DOCASE(IDL_LONG, IDL_TYP_ULONG64, ul64);
			default: GDL_WillThrowAfterCleaning("IDL_LongScalar: unexpected type "+i2s(v->type));
		}
		throw;
	}
IDL_LONG64 IDL_CDECL IDL_Long64Scalar(IDL_REGISTER IDL_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(IDL_LONG64, IDL_TYP_BYTE, c);
				DOCASE(IDL_LONG64, IDL_TYP_INT, i);
				DOCASE(IDL_LONG64, IDL_TYP_LONG, l);
				DOCASE(IDL_LONG64, IDL_TYP_FLOAT, f);
				DOCASE(IDL_LONG64, IDL_TYP_DOUBLE, d);
				DOCASE_CMP(IDL_LONG64, IDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(IDL_LONG64, IDL_TYP_DCOMPLEX, dcmp);
				DOCASE(IDL_LONG64, IDL_TYP_UINT, ui);
				DOCASE(IDL_LONG64, IDL_TYP_ULONG, ul);
				DOCASE(IDL_LONG64, IDL_TYP_LONG64, l64);
				DOCASE(IDL_LONG64, IDL_TYP_ULONG64, ul64);
			default: GDL_WillThrowAfterCleaning("IDL_Long64Scalar: unexpected type "+i2s(v->type));
		}
		throw;
	}
IDL_ULONG64 IDL_CDECL IDL_ULong64Scalar(IDL_REGISTER IDL_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(IDL_ULONG64, IDL_TYP_BYTE, c);
				DOCASE(IDL_ULONG64, IDL_TYP_INT, i);
				DOCASE(IDL_ULONG64, IDL_TYP_LONG, l);
				DOCASE(IDL_ULONG64, IDL_TYP_FLOAT, f);
				DOCASE(IDL_ULONG64, IDL_TYP_DOUBLE, d);
				DOCASE_CMP(IDL_ULONG64, IDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(IDL_ULONG64, IDL_TYP_DCOMPLEX, dcmp);
				DOCASE(IDL_ULONG64, IDL_TYP_UINT, ui);
				DOCASE(IDL_ULONG64, IDL_TYP_ULONG, ul);
				DOCASE(IDL_ULONG64, IDL_TYP_LONG64, l64);
				DOCASE(IDL_ULONG64, IDL_TYP_ULONG64, ul64);
			default: GDL_WillThrowAfterCleaning("IDL_ULong64Scalar: unexpected type "+i2s(v->type));
		}
		throw;
	}
IDL_MEMINT IDL_CDECL IDL_MEMINTScalar(IDL_REGISTER IDL_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(IDL_MEMINT, IDL_TYP_BYTE, c);
				DOCASE(IDL_MEMINT, IDL_TYP_INT, i);
				DOCASE(IDL_MEMINT, IDL_TYP_LONG, l);
				DOCASE(IDL_MEMINT, IDL_TYP_FLOAT, f);
				DOCASE(IDL_MEMINT, IDL_TYP_DOUBLE, d);
				DOCASE_CMP(IDL_MEMINT, IDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(IDL_MEMINT, IDL_TYP_DCOMPLEX, dcmp);
				DOCASE(IDL_MEMINT, IDL_TYP_UINT, ui);
				DOCASE(IDL_MEMINT, IDL_TYP_ULONG, ul);
				DOCASE(IDL_MEMINT, IDL_TYP_LONG64, l64);
				DOCASE(IDL_MEMINT, IDL_TYP_ULONG64, ul64);
			default: GDL_WillThrowAfterCleaning("IDL_ULong64Scalar: unexpected type "+i2s(v->type));
		}
		throw;
	}
IDL_FILEINT IDL_CDECL IDL_FILEINTScalar(IDL_REGISTER IDL_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_ENSURE_SIMPLE(v);
		switch (v->type) {
				DOCASE(IDL_FILEINT, IDL_TYP_BYTE, c);
				DOCASE(IDL_FILEINT, IDL_TYP_INT, i);
				DOCASE(IDL_FILEINT, IDL_TYP_LONG, l);
				DOCASE(IDL_FILEINT, IDL_TYP_FLOAT, f);
				DOCASE(IDL_FILEINT, IDL_TYP_DOUBLE, d);
				DOCASE_CMP(IDL_FILEINT, IDL_TYP_COMPLEX, cmp);
				DOCASE_CMP(IDL_FILEINT, IDL_TYP_DCOMPLEX, dcmp);
				DOCASE(IDL_FILEINT, IDL_TYP_UINT, ui);
				DOCASE(IDL_FILEINT, IDL_TYP_ULONG, ul);
				DOCASE(IDL_FILEINT, IDL_TYP_LONG64, l64);
				DOCASE(IDL_FILEINT, IDL_TYP_ULONG64, ul64);
			default: GDL_WillThrowAfterCleaning("IDL_ULong64Scalar: unexpected type "+i2s(v->type));
		}
		throw;
	}
#undef DOCASE
#undef DOCASE_CMP
void IDL_CDECL IDL_VarGetData(IDL_VPTR v, IDL_MEMINT *n, char **pd,  int ensure_simple){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (ensure_simple) IDL_ENSURE_SIMPLE(v);
	if (v->flags & IDL_V_ARR) {
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
		void gdlInitVector(void* arr, int type, size_t nelts) {
			TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
			switch (type) {
						DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
						DOCASE_ARRAY(IDL_TYP_INT, IDL_INT);
						DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG);
						DOCASE_ARRAY(IDL_TYP_FLOAT, float);
						DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
						DOCASE_ARRAY_CPLX(IDL_TYP_COMPLEX, IDL_COMPLEX);
						DOCASE_ARRAY_CPLX(IDL_TYP_DCOMPLEX, IDL_DCOMPLEX);
						DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT);
						DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
						DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
						DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
					default: GDL_WillThrowAfterCleaning("IDL_MakeTempVector: unexpected type " + i2s(type));
			}
			throw;
		}
#undef DOCASE_ARRAY
#undef DOCASE_ARRAY_CPLX

char* IDL_CDECL IDL_MakeTempVector(int type, IDL_MEMINT dim, int  init, IDL_VPTR *var){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_VPTR v=NewTMPVPTRARRAY();
		*var=v;
		v->type=type;
		v->value.arr->dim[0]=dim;
		v->value.arr->n_elts=dim;
		v->value.arr->n_dim=1;
		IDL_LONG64 sz=IDL_TypeSizeFunc(type);
		v->value.arr->elt_len = sz;
		SizeT l=dim*sz;
		void * addr=malloc(l); //aka IDL_ARR_INI_NOP
		v->value.arr->arr_len=l;
		v->value.arr->data = (UCHAR*) addr;
		if (init == IDL_ARR_INI_ZERO) memset((void*)addr, 0, l);
		else if (init == IDL_ARR_INI_INDEX) {
			if (type == IDL_TYP_STRING) {
				IDL_LONG64 nelts=v->value.arr->n_elts;
				static int slen = IDL_OutputFormatLenFunc(IDL_TYP_LONG)+1;
				// allocate size:
				char* allstrings = (char*)malloc(nelts * slen);
				IDL_STRING *thestrings = (IDL_STRING *) v->value.arr->data;
				for (auto i = 0; i < nelts; ++i) {
					thestrings[i].slen = slen;
					thestrings[i].s = &(allstrings[i * slen]);
					snprintf(thestrings[i].s,IDL_OutputFormatLenFunc(IDL_TYP_LONG), IDL_OutputFormatFunc(IDL_TYP_LONG), i);
				}
			} else gdlInitVector(addr, type, l);
		}
		return (char*) addr;
	}

	char *IDL_CDECL IDL_MakeTempStructVector(IDL_StructDefPtr sdef, IDL_MEMINT dim, IDL_VPTR *var, int zero) {
		IDL_VPTR v = NewTMPVPTRSTRUCT(sdef);
		*var = v;
		v->value.s.arr=new IDL_ARRAY();
		v->type=IDL_TYP_STRUCT; //already done, just for clarity.
		v->value.arr->dim[0]=dim;
		v->value.arr->n_elts=dim;
		v->value.arr->n_dim=1;
		IDL_LONG64 sz=v->value.s.sdef->length;
		v->value.arr->elt_len = sz;
		SizeT l=dim*sz;
		void * addr=malloc(l);
		if (zero) memset(addr, 0, l);
		v->value.arr->arr_len=l;
		v->value.arr->data = (UCHAR*) addr;
		return (char*) addr;
	}

char *IDL_CDECL IDL_MakeTempArray(int type, int n_dim, IDL_MEMINT  dim[], int init, IDL_VPTR *var){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_VPTR v=NewTMPVPTRARRAY();
		*var=v;
		v->type=type;
		SizeT l=1;
		for (auto i=0; i< n_dim; ++i) {
			l*=dim[i];
			v->value.arr->dim[i] = dim[i];
		}
		v->value.arr->n_elts=l;
		v->value.arr->n_dim = n_dim;
		IDL_LONG64 sz=IDL_TypeSizeFunc(type);
		v->value.arr->elt_len = sz;
		l=sz; for (auto i=0; i<n_dim; ++i) l*=dim[i];
		void * addr=malloc(l); //aka IDL_ARR_INI_NOP
		v->value.arr->arr_len=l;
		v->value.arr->data = (UCHAR*) addr;
		if (init == IDL_ARR_INI_ZERO)  memset((void*)addr, 0, l);
		else if (init == IDL_ARR_INI_INDEX) {
			if (type == IDL_TYP_STRING) {
				IDL_LONG64 nelts=v->value.arr->n_elts;
				static int slen = IDL_OutputFormatLenFunc(IDL_TYP_LONG)+1;
				// allocate size:
				char* allstrings = (char*)malloc(nelts * slen);
				IDL_STRING *thestrings = (IDL_STRING *) v->value.arr->data;
				for (auto i = 0; i < nelts; ++i) {
					thestrings[i].slen = slen;
					thestrings[i].s = &(allstrings[i * slen]);
					snprintf(thestrings[i].s, IDL_OutputFormatLenFunc(IDL_TYP_LONG), IDL_OutputFormatFunc(IDL_TYP_LONG), i);
				}
			} else gdlInitVector(addr, type, l);
		}
		return (char*) addr;	
}

	char *IDL_CDECL IDL_MakeTempStruct(IDL_StructDefPtr sdef, int  n_dim, IDL_MEMINT *dim, IDL_VPTR *var, int zero){
            if (sdef == NULL) GDL_WillThrowAfterCleaning("IDL_VarMakeTempFromTemplate() defines a struct without passing a valid IDL_StructDefPtr");
			IDL_VPTR v = NewTMPVPTRSTRUCT(sdef);
			if (zero) memset(&(v->value),0,sizeof(IDL_ALLTYPES));
			return (char*) 	&(v->value);
	}
	

char *IDL_CDECL IDL_VarMakeTempFromTemplate(IDL_VPTR template_var, int type, IDL_StructDefPtr sdef,  IDL_VPTR *result_addr, int zero){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_VPTR t = template_var;
	    IDL_VPTR v;
		if (t->flags & IDL_V_STRUCT) {
            if (sdef == NULL) GDL_WillThrowAfterCleaning("IDL_VarMakeTempFromTemplate() defines a struct without passing a valid IDL_StructDefPtr");
			v = NewTMPVPTRSTRUCT(sdef);
		} else if (t->flags & IDL_V_ARR) {
			v = NewTMPVPTRARRAY();
		} else {
			v = NewTMPVPTR(t->flags);
		}
		*result_addr = v;
		if (t->flags & IDL_V_ARR) {
			v->type = type;
			v->flags = t->flags;
			v->value.arr->n_dim = t->value.arr->n_dim;
			v->value.arr->n_elts=t->value.arr->n_elts;
			for (auto i=0; i< t->value.arr->n_dim; ++i) v->value.arr->dim[i] = t->value.arr->dim[i];
			IDL_LONG64 sz=IDL_TypeSizeFunc(type);
			v->value.arr->elt_len = sz;
			SizeT l=sz; for (auto i=0; i<t->value.arr->n_dim; ++i) l*=t->value.arr->dim[i];
			v->value.arr->arr_len = l;
			void * addr=malloc(l);
			v->value.arr->data = (UCHAR*) addr;
			if (zero) memset((void*)addr, 0, l);
			return (char*) addr;	
		} else {
			v->type = type;
			if (zero) memset(&(v->value),0,sizeof(IDL_ALLTYPES));
			return (char*) 	&(v->value);
		}
		throw;
}	

IDL_VPTR IDL_Gettmp(void){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	return NewTMPVPTR();
	}
#define DOIT(aaa, entry)\
	IDL_VPTR ret=NewTMPVPTR();\
	ret->type=aaa;\
	ret->value.entry=value;\
	return ret;
IDL_VPTR IDL_CDECL IDL_GettmpByte(UCHAR value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
     DOIT(IDL_TYP_BYTE, c);
}

IDL_VPTR IDL_GettmpInt(IDL_INT value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_INT, i);
	}
IDL_VPTR IDL_GettmpUInt(IDL_UINT value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_UINT, ui);
	}
IDL_VPTR IDL_GettmpLong(IDL_LONG value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_LONG, l);
	}
IDL_VPTR IDL_CDECL IDL_GettmpFloat(float value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_FLOAT, f);
	}
IDL_VPTR IDL_CDECL IDL_GettmpDouble(double value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_DOUBLE, d);
	}
IDL_VPTR IDL_CDECL IDL_GettmpPtr(IDL_HVID value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_PTR, hvid);
	}
IDL_VPTR IDL_CDECL IDL_GettmpObjRef(IDL_HVID value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_OBJREF, hvid);
	}
IDL_VPTR IDL_GettmpULong(IDL_ULONG value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_ULONG, ul);
	}
IDL_VPTR IDL_CDECL IDL_GettmpLong64(IDL_LONG64 value){
      DOIT(IDL_TYP_LONG64, l64);
}
IDL_VPTR IDL_CDECL IDL_GettmpULong64(IDL_ULONG64 value){
      DOIT(IDL_TYP_ULONG64, ul64);
}
IDL_VPTR IDL_GettmpFILEINT(IDL_FILEINT value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_FILEINT, fileint);
	}
IDL_VPTR IDL_GettmpMEMINT(IDL_MEMINT value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_MEMINT, memint);
	}
#undef DOIT

#define DEFOUT(idl_dst_type)\
    if (argc != 1) GDL_WillThrowAfterCleaning("IDL_CvtXXX: multiple argvuments not supported, fixme.");\
	if (argv[0]->type == idl_dst_type) return argv[0];\
	IDL_VPTR ret = NewTMPVPTR(argv[0]->flags);\
	ret->type = idl_dst_type;\
	IDL_ALLTYPES value = argv[0]->value;

#define PREPARE_ARRAY(dst_type)\
	IDL_ARRAY *dstarr=new IDL_ARRAY();\
    ret->flags |= IDL_V_ARR;\
	ret->value.arr=dstarr;\
    IDL_ARRAY* scrArrayDescr=argv[0]->value.arr;\
	memcpy(dstarr, scrArrayDescr, sizeof (IDL_ARRAY));\
	dstarr->elt_len = sizeof (dst_type);\
	dstarr->arr_len = dstarr->elt_len*dstarr->n_elts;\
	dst_type *retval = (dst_type*) malloc(dstarr->arr_len);\
	ret->value.arr->data = (UCHAR*) retval;

#define DOCASE_ARRAY(idl_src_type, src_type)\
 			case idl_src_type: {\
				src_type *srcval=(src_type *)(scrArrayDescr->data);\
				for (auto i=0; i< scrArrayDescr->n_elts; ++i) retval[i]=srcval[i];\
				break; }

#define DOCASE_TO_CMP_ARRAY(idl_src_type, src_type)\
 			case idl_src_type: {\
				src_type *srcval=(src_type *)(scrArrayDescr->data);\
				for (auto i=0; i< scrArrayDescr->n_elts; ++i) retval[i].r=srcval[i];\
				break; }

#define DOCASE_TO_CMP_ARRAY_FROM_CMP(idl_src_type, src_type)\
 			case idl_src_type: {\
				src_type *srcval=(src_type *)(scrArrayDescr->data);\
				for (auto i=0; i< scrArrayDescr->n_elts; ++i) {retval[i].r=srcval[i].r;retval[i].i=srcval[i].i;}\
				break; }

#define DOCASE(type, field1, field2)\
 case type: ret->value.field1=value.field2 ; break;

#define DOCASE_FROM_CMP(type, field1, field2)\
 case type: ret->value.field1=value.field2.r ; break;

#define DOCASE_ARRAY_FROM_CMP(idl_src_type, src_type)\
 			case idl_src_type: {\
				src_type *srcval=(src_type *)(scrArrayDescr->data);\
				for (auto i=0; i< scrArrayDescr->n_elts; ++i) retval[i]=srcval[2*i];\
				break; }

#define DOCASE_TO_CMP(type, field1, field2)\
 case type: ret->value.field1.r=value.field2 ; break;

#define DOCASE_TO_CMP_FROM_CMP(type, field1, field2)\
 case type: ret->value.field1.r=value.field2.r ; ret->value.field1.i=value.field2.i ;break;

IDL_VPTR IDL_CvtByte(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(IDL_TYP_BYTE);
		if (argv[0]->flags & IDL_V_ARR) {
		PREPARE_ARRAY(UCHAR);		
	    switch (argv[0]->type) {
		DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
		DOCASE_ARRAY(IDL_TYP_INT, IDL_INT);
		DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG );
		DOCASE_ARRAY(IDL_TYP_FLOAT, float); 
		DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
		DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float );
		DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double );
		DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT );
		DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG );
		DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64 );
		DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	} else {
		switch (argv[0]->type) {
		DOCASE(IDL_TYP_BYTE, c, c);
		DOCASE(IDL_TYP_INT, c, i);
		DOCASE(IDL_TYP_LONG, c, l);
		DOCASE(IDL_TYP_FLOAT, c, f);
		DOCASE(IDL_TYP_DOUBLE, c, d);
		DOCASE_FROM_CMP(IDL_TYP_COMPLEX, c, cmp);
		DOCASE_FROM_CMP(IDL_TYP_DCOMPLEX, c, dcmp);
		DOCASE(IDL_TYP_UINT, c, ui);
		DOCASE(IDL_TYP_ULONG, c, ul);
		DOCASE(IDL_TYP_LONG64, c, l64);
		DOCASE(IDL_TYP_ULONG64, c, ul64);
		default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
	return ret;
}

IDL_VPTR IDL_CvtBytscl(int argc, IDL_VPTR argv[], char *argk) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(IDL_TYP_BYTE);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(UCHAR);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(IDL_TYP_INT,  IDL_INT);
					DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG);
					DOCASE_ARRAY(IDL_TYP_FLOAT, float);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT);
					DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
			switch (argv[0]->type) {
					DOCASE(IDL_TYP_BYTE, c, c);
					DOCASE(IDL_TYP_INT, c, i);
					DOCASE(IDL_TYP_LONG, c, l);
					DOCASE(IDL_TYP_FLOAT, c, f);
					DOCASE(IDL_TYP_DOUBLE, c, d);
					DOCASE_FROM_CMP(IDL_TYP_COMPLEX, c, cmp);
					DOCASE_FROM_CMP(IDL_TYP_DCOMPLEX, c, dcmp);
					DOCASE(IDL_TYP_UINT, c, ui);
					DOCASE(IDL_TYP_ULONG, c, ul);
					DOCASE(IDL_TYP_LONG64, c, l64);
					DOCASE(IDL_TYP_ULONG64, c, ul64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		}
		return ret;
}
//
IDL_VPTR IDL_CvtFix(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(IDL_TYP_INT);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(IDL_INT);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(IDL_TYP_INT,  IDL_INT);
					DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG);
					DOCASE_ARRAY(IDL_TYP_FLOAT, float);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT);
					DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
			switch (argv[0]->type) {
					DOCASE(IDL_TYP_BYTE, i, c);
					DOCASE(IDL_TYP_INT, i, i);
					DOCASE(IDL_TYP_LONG, i, l);
					DOCASE(IDL_TYP_FLOAT, i, f);
					DOCASE(IDL_TYP_DOUBLE, i, d);
					DOCASE_FROM_CMP(IDL_TYP_COMPLEX, i, cmp);
					DOCASE_FROM_CMP(IDL_TYP_DCOMPLEX, i, dcmp);
					DOCASE(IDL_TYP_UINT, i, ui);
					DOCASE(IDL_TYP_ULONG, i, ul);
					DOCASE(IDL_TYP_LONG64, i, l64);
					DOCASE(IDL_TYP_ULONG64, i, ul64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		}
		return ret;

}
//
IDL_VPTR IDL_CvtUInt(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(IDL_TYP_UINT);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(IDL_UINT);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(IDL_TYP_INT,  IDL_INT);
					DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG);
					DOCASE_ARRAY(IDL_TYP_FLOAT, float);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT);
					DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(IDL_TYP_BYTE, ui, c);
				DOCASE(IDL_TYP_INT, ui, i);
				DOCASE(IDL_TYP_LONG, ui, l);
				DOCASE(IDL_TYP_FLOAT, ui, f);
				DOCASE(IDL_TYP_DOUBLE, ui, d);
				DOCASE_FROM_CMP(IDL_TYP_COMPLEX, ui, cmp);
				DOCASE_FROM_CMP(IDL_TYP_DCOMPLEX, ui, dcmp);
				DOCASE(IDL_TYP_UINT, ui, ui);
				DOCASE(IDL_TYP_ULONG, ui, ul);
				DOCASE(IDL_TYP_LONG64, ui, l64);
				DOCASE(IDL_TYP_ULONG64, ui, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;
	}
//
IDL_VPTR IDL_CvtLng(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(IDL_TYP_LONG);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(IDL_LONG);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(IDL_TYP_INT,  IDL_INT);
					DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG);
					DOCASE_ARRAY(IDL_TYP_FLOAT, float);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT);
					DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(IDL_TYP_BYTE, l, c);
				DOCASE(IDL_TYP_INT, l, i);
				DOCASE(IDL_TYP_LONG, l, l);
				DOCASE(IDL_TYP_FLOAT, l, f);
				DOCASE(IDL_TYP_DOUBLE, l, d);
				DOCASE_FROM_CMP(IDL_TYP_COMPLEX, l, cmp);
				DOCASE_FROM_CMP(IDL_TYP_DCOMPLEX, l, dcmp);
				DOCASE(IDL_TYP_UINT, l, ui);
				DOCASE(IDL_TYP_ULONG, l, ul);
				DOCASE(IDL_TYP_LONG64, l, l64);
				DOCASE(IDL_TYP_ULONG64, l, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;

	}
//
IDL_VPTR IDL_CvtULng(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(IDL_TYP_ULONG);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(IDL_ULONG);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(IDL_TYP_INT,  IDL_INT);
					DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG);
					DOCASE_ARRAY(IDL_TYP_FLOAT, float);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT);
					DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(IDL_TYP_BYTE, ul, c);
				DOCASE(IDL_TYP_INT, ul, i);
				DOCASE(IDL_TYP_LONG, ul, l);
				DOCASE(IDL_TYP_FLOAT, ul, f);
				DOCASE(IDL_TYP_DOUBLE, ul, d);
				DOCASE_FROM_CMP(IDL_TYP_COMPLEX, ul, cmp);
				DOCASE_FROM_CMP(IDL_TYP_DCOMPLEX, ul, dcmp);
				DOCASE(IDL_TYP_UINT, ul, ui);
				DOCASE(IDL_TYP_ULONG, ul, ul);
				DOCASE(IDL_TYP_LONG64, ul, l64);
				DOCASE(IDL_TYP_ULONG64, ul, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;

	}
//
IDL_VPTR IDL_CvtLng64(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(IDL_TYP_LONG64);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(IDL_LONG64);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(IDL_TYP_INT,  IDL_INT);
					DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG);
					DOCASE_ARRAY(IDL_TYP_FLOAT, float);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT);
					DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(IDL_TYP_BYTE, l64, c);
				DOCASE(IDL_TYP_INT, l64, i);
				DOCASE(IDL_TYP_LONG, l64, l);
				DOCASE(IDL_TYP_FLOAT, l64, f);
				DOCASE(IDL_TYP_DOUBLE, l64, d);
				DOCASE_FROM_CMP(IDL_TYP_COMPLEX, l64, cmp);
				DOCASE_FROM_CMP(IDL_TYP_DCOMPLEX, l64, dcmp);
				DOCASE(IDL_TYP_UINT, l64, ui);
				DOCASE(IDL_TYP_ULONG, l64, ul);
				DOCASE(IDL_TYP_LONG64, l64, l64);
				DOCASE(IDL_TYP_ULONG64, l64, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;

	}
//

IDL_VPTR IDL_CvtULng64(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(IDL_TYP_ULONG64);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(IDL_ULONG64);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(IDL_TYP_INT,  IDL_INT);
					DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG);
					DOCASE_ARRAY(IDL_TYP_FLOAT, float);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT);
					DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(IDL_TYP_BYTE, ul64, c);
				DOCASE(IDL_TYP_INT, ul64, i);
				DOCASE(IDL_TYP_LONG, ul64, l);
				DOCASE(IDL_TYP_FLOAT, ul64, f);
				DOCASE(IDL_TYP_DOUBLE, ul64, d);
				DOCASE_FROM_CMP(IDL_TYP_COMPLEX, ul64, cmp);
				DOCASE_FROM_CMP(IDL_TYP_DCOMPLEX, ul64, dcmp);
				DOCASE(IDL_TYP_UINT, ul64, ui);
				DOCASE(IDL_TYP_ULONG, ul64, ul);
				DOCASE(IDL_TYP_LONG64, ul64, l64);
				DOCASE(IDL_TYP_ULONG64, ul64, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;

	}
IDL_VPTR IDL_CvtMEMINT(int argc, IDL_VPTR argv[]) {
	IDL_VPTR v=IDL_CvtULng64(argc, argv);
	v->type=IDL_TYP_MEMINT;
	return v;
}
IDL_VPTR IDL_CvtFILEINT(int argc, IDL_VPTR argv[]) {
	IDL_VPTR v=IDL_CvtULng64(argc, argv);
	v->type=IDL_TYP_FILEINT;
	return v;
}
//
IDL_VPTR IDL_CvtFlt(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(IDL_TYP_FLOAT);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(float);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(IDL_TYP_INT,  IDL_INT);
					DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG);
					DOCASE_ARRAY(IDL_TYP_FLOAT, float);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT);
					DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
		switch (argv[0]->type) {
				DOCASE(IDL_TYP_BYTE, f, c);
				DOCASE(IDL_TYP_INT, f, i);
				DOCASE(IDL_TYP_LONG, f, l);
				DOCASE(IDL_TYP_FLOAT, f, f);
				DOCASE(IDL_TYP_DOUBLE, f, d);
				DOCASE_FROM_CMP(IDL_TYP_COMPLEX, f, cmp);
				DOCASE_FROM_CMP(IDL_TYP_DCOMPLEX, f, dcmp);
				DOCASE(IDL_TYP_UINT, f, ui);
				DOCASE(IDL_TYP_ULONG, f, ul);
				DOCASE(IDL_TYP_LONG64, f, l64);
				DOCASE(IDL_TYP_ULONG64, f, ul64);
			default: GDL_WillThrowAfterCleaning("unexpected type");
		}
	}
		return ret;
	}
//
IDL_VPTR IDL_CvtDbl(int argc, IDL_VPTR argv[]){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(IDL_TYP_DOUBLE);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(double);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, UCHAR);
					DOCASE_ARRAY(IDL_TYP_INT,  IDL_INT);
					DOCASE_ARRAY(IDL_TYP_LONG, IDL_LONG);
					DOCASE_ARRAY(IDL_TYP_FLOAT, float);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, IDL_UINT);
					DOCASE_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
			switch (argv[0]->type) {
					DOCASE(IDL_TYP_BYTE, d, c);
					DOCASE(IDL_TYP_INT, d, i);
					DOCASE(IDL_TYP_LONG, d, l);
					DOCASE(IDL_TYP_FLOAT, d, f);
					DOCASE(IDL_TYP_DOUBLE, d, d);
					DOCASE_FROM_CMP(IDL_TYP_COMPLEX, d, cmp);
					DOCASE_FROM_CMP(IDL_TYP_DCOMPLEX, d, dcmp);
					DOCASE(IDL_TYP_UINT, d, ui);
					DOCASE(IDL_TYP_ULONG, d, ul);
					DOCASE(IDL_TYP_LONG64, d, l64);
					DOCASE(IDL_TYP_ULONG64, d, ul64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		}
		return ret;
	}
//
IDL_VPTR IDL_CvtComplex(int argc, IDL_VPTR argv[], char *argk=NULL) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(IDL_TYP_COMPLEX);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(IDL_COMPLEX);
			switch (argv[0]->type) {
					DOCASE_TO_CMP_ARRAY(IDL_TYP_BYTE, UCHAR);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_INT,  IDL_INT);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_LONG, IDL_LONG);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_FLOAT, float);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, IDL_COMPLEX);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, IDL_DCOMPLEX);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_UINT, IDL_UINT);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
			switch (argv[0]->type) {
					DOCASE_TO_CMP(IDL_TYP_BYTE, dcmp, c);
					DOCASE_TO_CMP(IDL_TYP_INT, dcmp, i);
					DOCASE_TO_CMP(IDL_TYP_LONG, dcmp, l);
					DOCASE_TO_CMP(IDL_TYP_FLOAT, dcmp, f);
					DOCASE_TO_CMP(IDL_TYP_DOUBLE, dcmp, d);
					DOCASE_TO_CMP_FROM_CMP(IDL_TYP_COMPLEX, dcmp, cmp);
					DOCASE_TO_CMP_FROM_CMP(IDL_TYP_DCOMPLEX, dcmp, dcmp);
					DOCASE_TO_CMP(IDL_TYP_UINT, dcmp, ui);
					DOCASE_TO_CMP(IDL_TYP_ULONG, dcmp, ul);
					DOCASE_TO_CMP(IDL_TYP_LONG64, dcmp, l64);
					DOCASE_TO_CMP(IDL_TYP_ULONG64, dcmp, ul64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		}
		return ret;
	}
//
IDL_VPTR IDL_CvtDComplex(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(IDL_TYP_DCOMPLEX);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(IDL_DCOMPLEX);
			switch (argv[0]->type) {
					DOCASE_TO_CMP_ARRAY(IDL_TYP_BYTE, UCHAR);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_INT,  IDL_INT);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_LONG, IDL_LONG);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_FLOAT, float);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, IDL_COMPLEX);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, IDL_DCOMPLEX);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_UINT, IDL_UINT);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		} else {
			switch (argv[0]->type) {
					DOCASE_TO_CMP(IDL_TYP_BYTE, cmp, c);
					DOCASE_TO_CMP(IDL_TYP_INT, cmp, i);
					DOCASE_TO_CMP(IDL_TYP_LONG, cmp, l);
					DOCASE_TO_CMP(IDL_TYP_FLOAT, cmp, f);
					DOCASE_TO_CMP(IDL_TYP_DOUBLE, cmp, d);
					DOCASE_TO_CMP_FROM_CMP(IDL_TYP_COMPLEX, cmp, cmp);
					DOCASE_TO_CMP_FROM_CMP(IDL_TYP_DCOMPLEX, cmp, dcmp);
					DOCASE_TO_CMP(IDL_TYP_UINT, cmp, ui);
					DOCASE_TO_CMP(IDL_TYP_ULONG, cmp, ul);
					DOCASE_TO_CMP(IDL_TYP_LONG64, cmp, l64);
					DOCASE_TO_CMP(IDL_TYP_ULONG64, cmp, ul64);
				default: GDL_WillThrowAfterCleaning("unexpected type");
			}
		}
		return ret;
			}

IDL_VPTR IDL_CvtString(int argc, IDL_VPTR argv[], char *argk=NULL){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
DEFOUT(IDL_TYP_STRING);
GDL_WillThrowAfterCleaning("IDL_CvtString not supported, fixme");
//		if (argv[0]->flags & IDL_V_ARR) {
//			PREPARE_ARRAY(IDL_DCOMPLEX);
//			switch (argv[0]->type) {
//					DOCASE_TO_CMP_ARRAY(IDL_TYP_BYTE, UCHAR);
//					DOCASE_TO_CMP_ARRAY(IDL_TYP_INT,  IDL_INT);
//					DOCASE_TO_CMP_ARRAY(IDL_TYP_LONG, IDL_LONG);
//					DOCASE_TO_CMP_ARRAY(IDL_TYP_FLOAT, float);
//					DOCASE_TO_CMP_ARRAY(IDL_TYP_DOUBLE, double);
//					DOCASE_TO_CMP_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, IDL_COMPLEX);
//					DOCASE_TO_CMP_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, IDL_DCOMPLEX);
//					DOCASE_TO_CMP_ARRAY(IDL_TYP_UINT, IDL_UINT);
//					DOCASE_TO_CMP_ARRAY(IDL_TYP_ULONG, IDL_ULONG);
//					DOCASE_TO_CMP_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
//					DOCASE_TO_CMP_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
//				default: GDL_WillThrowAfterCleaning("unexpected type");
//			}
//		} else {
//			switch (argv[0]->type) {
//					DOCASE_TO_CMP(IDL_TYP_BYTE, cmp, c);
//					DOCASE_TO_CMP(IDL_TYP_INT, cmp, i);
//					DOCASE_TO_CMP(IDL_TYP_LONG, cmp, l);
//					DOCASE_TO_CMP(IDL_TYP_FLOAT, cmp, f);
//					DOCASE_TO_CMP(IDL_TYP_DOUBLE, cmp, d);
//					DOCASE_TO_CMP_FROM_CMP(IDL_TYP_COMPLEX, cmp, cmp);
//					DOCASE_TO_CMP_FROM_CMP(IDL_TYP_DCOMPLEX, cmp, dcmp);
//					DOCASE_TO_CMP(IDL_TYP_UINT, cmp, ui);
//					DOCASE_TO_CMP(IDL_TYP_ULONG, cmp, ul);
//					DOCASE_TO_CMP(IDL_TYP_LONG64, cmp, l64);
//					DOCASE_TO_CMP(IDL_TYP_ULONG64, cmp, ul64);
//				default: GDL_WillThrowAfterCleaning("unexpected type");
//			}
//		}
		return ret;

}

#undef DEFOUT
#undef PREPARE_ARRAY
#undef DOCASE
#undef DOCASE_FROM_CMP
#undef DOCASE_TO_CMP
#undef DOCASE_TO_CMP_TO_CMP
#undef DOCASE_TO_CMP_ARRAY
#undef DOCASE_TO_CMP_ARRAY_FROM_CMP



char* GDLWriteVarAtAddr(BaseGDL* var, std::string name, UCHAR type, size_t address, bool isoutput, bool isarray){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		switch (type) {
			case IDL_TYP_BYTE:
			{
				DByteGDL* res = static_cast<DByteGDL*> (var->Convert2(GDL_BYTE));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (UCHAR));
				break;
			}
			case IDL_TYP_INT:
			{
				DIntGDL* res = static_cast<DIntGDL*> (var->Convert2(GDL_INT));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (IDL_INT));
				break;
			}
			case IDL_TYP_LONG:
			{
				DLongGDL* res = static_cast<DLongGDL*> (var->Convert2(GDL_LONG));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (IDL_LONG));
				break;
			}
			case IDL_TYP_FLOAT:
			{
				DFloatGDL* res = static_cast<DFloatGDL*> (var->Convert2(GDL_FLOAT));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (float));
				break;
			}
			case IDL_TYP_DOUBLE:
			{
				DDoubleGDL* res = static_cast<DDoubleGDL*> (var->Convert2(GDL_DOUBLE));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (double));
				break;
			}
			case IDL_TYP_COMPLEX:
			{
				DComplexGDL* res = static_cast<DComplexGDL*> (var->Convert2(GDL_COMPLEX));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (IDL_COMPLEX));
				break;
			}
			case IDL_TYP_DCOMPLEX:
			{
				DComplexDblGDL* res = static_cast<DComplexDblGDL*> (var->Convert2(GDL_COMPLEXDBL));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (IDL_DCOMPLEX));
				break;
			}
			case IDL_TYP_UINT:
			{
				DUIntGDL* res = static_cast<DUIntGDL*> (var->Convert2(GDL_UINT));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (IDL_UINT));
				break;
			}
			case IDL_TYP_ULONG:
			{
				DULongGDL* res = static_cast<DULongGDL*> (var->Convert2(GDL_ULONG));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (IDL_ULONG));
				break;
			}
			case IDL_TYP_ULONG64:
			{
				DULong64GDL* res = static_cast<DULong64GDL*> (var->Convert2(GDL_ULONG64));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (IDL_ULONG64));
				break;
			}
			case IDL_TYP_STRING:
			{
				DStringGDL* res = static_cast<DStringGDL*> (var->Convert2(GDL_STRING));
				if (isarray) {
					SizeT nEl=var->N_Elements();
					void* allstringdescr=malloc(nEl*sizeof(IDL_STRING));
					memset(allstringdescr,0,nEl*sizeof(IDL_STRING));
					void* stringdescPtrs=malloc(nEl*sizeof(IDL_STRING*));
					IDL_STRING** p=(IDL_STRING**)stringdescPtrs;
					for (SizeT i=0; i< nEl; ++i) p[i]=(IDL_STRING*)((SizeT)allstringdescr+(i*sizeof(IDL_STRING)));
					for (auto i=0; i< nEl; ++i) {
						p[i]->slen = ((*res)[i]).size();
						p[i]->s = (char*) malloc(p[i]->slen + 1);
						strncpy(p[i]->s, (*res)[i].c_str(), p[i]->slen + 1);
					}
					return (char*)p;
				//write an array of string descriptors at address; return vector of addresses to be destroyed when IDL_KWFree will be called
				} else {
					IDL_STRING *s=(IDL_STRING*) (address); //string descr is at address
					s->slen = res->NBytes();
					s->s = (char*) malloc(s->slen + 1);
					strncpy(s->s, (*res)[0].c_str(), s->slen + 1);
					return s->s;
				}
				break;
			}
			case IDL_TYP_UNDEF:
			{
				if (!isoutput) GDL_WillThrowAfterCleaning("GDLWriteVarAtAddr: variable " + name + " is not writeable.");
				break;
			}
//			case IDL_TYP_PTR:
//				GDL_WillThrowAfterCleaning("Unable to convert variable to type pointer.");
//				break;
//			case IDL_TYP_OBJREF:
//				GDL_WillThrowAfterCleaning("Unable to convert variable to type object.");
//				break;
			default: GDL_WillThrowAfterCleaning("GDLWriteVarAtAddr: unsupported case.");
		}
		return NULL;
	}
	
	void GDLZeroAtAddr(size_t address){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
				memset((void*) (address), 0, sizeof(NULL));
	}

	IDL_VPTR GdlExportPresentKeyword(IDL_KW_PAR requested, GDL_KEYWORDS_LIST passed, void* kw_result) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
				static const int ok = 1;
		static const int cleanMem = 1;
		static const int nok = 0;
		IDL_VPTR toBeReturned = NULL;
		bool tempo=(passed.readonly==0);
		bool iszero = ((requested.flags & IDL_KW_ZERO) == IDL_KW_ZERO); //zero field if requested
		// Indicates parameter is an OUTPUT parameter passed by reference.
		// Expressions are excluded.  The address of the IDL_VARIABLE is stored in
		// the value field.  Otherwise, no checking is performed.  Special hint:
		// to find out if a IDL_KW_OUT parameter is specified, use 0 for the type,
		// and IDL_KW_OUT | IDL_KW_ZERO for the flags.  The value field will either
		// contain NULL or the pointer to the variable. */
		bool inputByReference = ((requested.flags & IDL_KW_VIN) == IDL_KW_VIN); // input, but passed by reference
		bool isoutput = (!inputByReference && (requested.flags & IDL_KW_OUT) == IDL_KW_OUT);
		bool isarray = (!inputByReference && (requested.flags & IDL_KW_ARRAY) == IDL_KW_ARRAY); //var must be an array, field is a IDL_KW_ARR_DESC_R*
		bool byMask = ((requested.flags & IDL_KW_VALUE) == IDL_KW_VALUE);
		// tag 'out' those that will get a return value
		if (requested.specified != NULL) { // need write 0 or 1 in a special int in KW structure
			//			std::cerr << requested.keyword << ", specified: at 0x " << (size_t) (requested.specified) << std::dec << std::endl;
			size_t global_address = (size_t) (kw_result)+(size_t) (requested.specified);
			memcpy((void*) (global_address), &ok, sizeof (int)); //requested is in offset
		}
		if (requested.value != NULL) { // need to pass either an address of a IDL_VPTR or fill in static elements of the structure exchanged with routine
			size_t global_address = (size_t) (kw_result)+(size_t) (requested.value);
			//			std::cerr << requested.keyword << ", value to report at 0x" << std::hex<< (size_t) (requested.value) << std::dec <<std::endl;
			if (isoutput && passed.readonly) GDL_WillThrowAfterCleaning("Keyword " + std::string(requested.keyword) + " must be a named variable.");
			BaseGDL* var = passed.varptr;
			//if requested var is NULL here, it is an undefined var, which MAY be returned as good value.
			if (var == NULL) GDL_WillThrowAfterCleaning("GDLExportKeyword: variable " + std::string(requested.keyword) + " is not defined.");
			if (iszero) { //zero before write.
				GDLZeroAtAddr(global_address);
			}
			if (var != NULL) {
				if (!isarray && (var->N_Elements() > 1)) GDL_WillThrowAfterCleaning("Expression must be a scalar or 1 element array in this context: " + std::string(passed.name) + ".");
				if (inputByReference) {//address (relative) of a IDL_VPTR with input only value
					IDL_VPTR temp = GDL_ToVPTR(var,tempo, true);
					memcpy((void*) (global_address), (void*) (&temp), sizeof (IDL_VPTR)); //pass by address of a IDL_VAR
				} else if (isoutput) { //address (relative) of a IDL_VPTR where input/output value is written (if existing) and will be returned 
					toBeReturned = GDL_ToVPTR(var,false, true); //to be returned
					memcpy((void*) (global_address), (void*) (&toBeReturned), sizeof (IDL_VPTR)); //pass address of a IDL_VAR that will contain the result.
				} else if (isarray) {
					size_t array_desc_address = (size_t) (requested.value);
					IDL_KW_ARR_DESC_R* arr_desc = (IDL_KW_ARR_DESC_R*) (array_desc_address);
					//check limits
					if ((var->N_Elements() > (*arr_desc).nmax) || (var->N_Elements() < (*arr_desc).nmin))
						GDL_WillThrowAfterCleaning(
							"Keyword array parameter " + std::string(requested.keyword) + " must have from " + i2s(arr_desc->nmin) + " to " + i2s(arr_desc->nmax) + " elements.");
					//and these are offsets!
					size_t copy_address_offset = (size_t) (kw_result)+(size_t) ((*arr_desc).n_offset);
					IDL_MEMINT passedArraySize = var->N_Elements();
					memcpy((void*) copy_address_offset, (void*) &passedArraySize, sizeof (IDL_MEMINT)); //number of passed elements
					size_t data_address = (size_t) (kw_result)+(size_t) ((*arr_desc).data); //address where to pass elements
					if (GDLWriteVarAtAddr(var, std::string(requested.keyword), requested.type, data_address, isoutput, true) != NULL) {
						memcpy(kw_result, &cleanMem, sizeof (int)); //make IDL_KW_FREE in called program call IDL_KWFree();
					}
				} else {
					//here IDL_KW_VALUE may appear
					if (byMask) {
						if (requested.type != IDL_TYP_LONG) GDL_WillThrowAfterCleaning("Invalid use of IDL_KW_VALUE on non-integer keyword.");
						long mask = IDL_KW_VALUE_MASK & requested.flags;
						long *val = (long*) global_address;
						*val |= mask;
					} else {
						if (GDLWriteVarAtAddr(var, std::string(requested.keyword), requested.type, global_address, isoutput, false) != NULL) {
							memcpy(kw_result, &cleanMem, sizeof (int)); //make IDL_KW_FREE in called program call IDL_KWFree();
						}
					}
				}
			}
		}
		return toBeReturned;
	}

	IDL_VPTR GdlExportPresentKeywordInOldApi(IDL_KW_PAR requested, GDL_KEYWORDS_LIST passed, void* address) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		static const int ok = 1;
		static const int cleanMem = 1;
		static const int nok = 0;
		IDL_VPTR toBeReturned = NULL;

		bool tempo=(passed.readonly==0);
		bool iszero = ((requested.flags & IDL_KW_ZERO) == IDL_KW_ZERO); //zero field if requested
		bool inputByReference = ((requested.flags & IDL_KW_VIN) == IDL_KW_VIN); // input, but passed by reference
		bool isoutput = (!inputByReference && (requested.flags & IDL_KW_OUT) == IDL_KW_OUT);
		bool isarray = (!inputByReference && (requested.flags & IDL_KW_ARRAY) == IDL_KW_ARRAY); //var must be an array, field is a IDL_KW_ARR_DESC_R*
		bool byMask = ((requested.flags & IDL_KW_VALUE) == IDL_KW_VALUE);
		if (requested.specified != NULL) { 
			memcpy((void*) (address), &ok, sizeof (int)); //requested is in offset
		}
		if (requested.value != NULL) { // need to pass either an address of a IDL_VPTR or fill in static elements of the structure exchanged with routine
			if (isoutput && passed.readonly) GDL_WillThrowAfterCleaning("Keyword " + std::string(requested.keyword) + " must be a named variable.");
			BaseGDL* var = passed.varptr;
			//if requested var is NULL here, it is an undefined var, which MAY be returned as good value.
			if (var == NULL) GDL_WillThrowAfterCleaning("GDLExportKeyword: variable " + std::string(requested.keyword) + " is not defined.");
			if (iszero) GDLZeroAtAddr((size_t)address);
			if (var != NULL) {
				if (!isarray && (var->N_Elements() > 1)) GDL_WillThrowAfterCleaning("Expression must be a scalar or 1 element array in this context: " + std::string(passed.name) + ".");
				if (inputByReference) {//address (relative) of a IDL_VPTR with input only value
					IDL_VPTR temp = GDL_ToVPTR(var,tempo,true);
					memcpy((void*) (address), (void*) (&temp), sizeof (IDL_VPTR)); //pass by address of a IDL_VAR
				} else if (isoutput) { //address (relative) of a IDL_VPTR where input/output value is written (if existing) and will be returned 
					toBeReturned = GDL_ToVPTR(var,false,true);
					memcpy((void*) (address), (void*) (&toBeReturned), sizeof (IDL_VPTR)); //pass address of a IDL_VAR that will contain the result.
				} else if (isarray) {
					size_t array_desc_address = (size_t) (requested.value);
					IDL_KW_ARR_DESC* arr_desc = (IDL_KW_ARR_DESC*) (array_desc_address);
					//check limits
					if ((var->N_Elements() > (*arr_desc).nmax) || (var->N_Elements() < (*arr_desc).nmin))
						GDL_WillThrowAfterCleaning(
							"Keyword array parameter " + std::string(requested.keyword) + " must have from " + i2s(arr_desc->nmin) + " to " + i2s(arr_desc->nmax) + " elements.");
					//and these are offsets!
					IDL_MEMINT passedArraySize = var->N_Elements();
					memcpy((void*) address, (void*) &passedArraySize, sizeof (IDL_MEMINT)); //number of passed elements
					size_t data_address = (size_t) ((*arr_desc).data); //address where to pass elements
					GDLWriteVarAtAddr(var, std::string(requested.keyword), requested.type, data_address, isoutput, true);
				} else {
					//here IDL_KW_VALUE may appear
					if (byMask) {
						if (requested.type != IDL_TYP_LONG) GDL_WillThrowAfterCleaning("Invalid use of IDL_KW_VALUE on non-integer keyword.");
						long mask = IDL_KW_VALUE_MASK & requested.flags;
						long *val = (long*) address;
						*val |= mask;
					} else {
						GDLWriteVarAtAddr(var, std::string(requested.keyword), requested.type, (size_t) address, isoutput, false);
					}
				}
			}
		}
		return toBeReturned;
	}

	void GdlExportAbsentKeywordInOldApi(IDL_KW_PAR requested, void* address) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
				static const int ok = 1;
		static const int nok = 0;

		bool iszero = ((requested.flags & IDL_KW_ZERO) == IDL_KW_ZERO); //zero field if requested
		bool byAddress = ((requested.flags & IDL_KW_VIN) == IDL_KW_VIN); // input, but passed by address
		bool isarray = (!byAddress && (requested.flags & IDL_KW_ARRAY) == IDL_KW_ARRAY); //var will be an arry
		bool isoutput = ((requested.flags & IDL_KW_OUT) == IDL_KW_OUT); // output, hence passed by address
		bool byMask = ((requested.flags & IDL_KW_VALUE) == IDL_KW_VALUE);
		// tag 'out' those that will get a return value
		if (requested.specified != NULL) { // need write 0 or 1 in a special int in KW structure
			memcpy((void*) (address), &nok, sizeof (int)); //requested is in offset
		}
		if (requested.value != NULL) { 
			//if requested var is not present, this has to be returned
			if (iszero) GDLZeroAtAddr((size_t)address);
		}
	}

		void GdlExportAbsentKeyword(IDL_KW_PAR requested, void* kw_result) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		static const int ok = 1;
		static const int nok = 0;
		
		bool iszero = ((requested.flags & IDL_KW_ZERO) == IDL_KW_ZERO); //zero field if requested
		bool byAddress = ((requested.flags & IDL_KW_VIN) == IDL_KW_VIN);// input, but passed by address
		bool isarray = (!byAddress && ( requested.flags & IDL_KW_ARRAY) == IDL_KW_ARRAY ); //var will be an arry
		bool isoutput = ((requested.flags & IDL_KW_OUT) == IDL_KW_OUT); // output, hence passed by address
		bool byMask = ((requested.flags & IDL_KW_VALUE) == IDL_KW_VALUE);
		// tag 'out' those that will get a return value
		if (requested.specified != NULL) { // need write 0 or 1 in a special int in KW structure
//			std::cerr << requested.keyword << ", specified: as " << std::hex << (size_t) (requested.specified) <<std::dec<< "... not present" << std::endl;
			size_t address = (size_t) (kw_result)+(size_t) (requested.specified);
			memcpy((void*) (address), &nok, sizeof (int)); //requested is in offset
		}
		if (requested.value != NULL) { // need to pass either an address of a IDL_VPTR or fill in static elements of the structure exchanged with routine
//			std::cerr << requested.keyword << ", value: at " << std::hex << (size_t) (requested.value) <<std::dec<< "... not present" << std::endl;
			// IDL_KW_VIN types (IDL_KW_OUT | IDL_KW_ARRAY) have an absolute address.
			size_t address = byAddress ? (size_t) (requested.value) : (size_t) (kw_result)+(size_t) (requested.value);
			//if requested var is not present, this has to be returned
			if (iszero) { //zero before write.
				size_t address;
				if (isarray) {
					address = (size_t) (requested.value);
				} else {
					address = (size_t) (kw_result)+(size_t) (requested.value);
				}
				GDLZeroAtAddr(address);
			}
		}
	}
    int IDL_CDECL IDL_KWGetParams(int argc, IDL_VPTR *argv, char *argk_passed,
				     IDL_KW_PAR *kw_requested,
				     IDL_VPTR *plain_args, int mask) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
				IDL_VPTR ret = GdlExportPresentKeywordInOldApi(kw_requested[it->first], argk[it->second], kw_requested[it->first].value);
				if (ret != NULL) {
					argk[ipassed].out = ret; //pass vptr back
				}
			}
			irequested++;
		}
		return 0;
	}

	int IDL_CDECL IDL_KWProcessByOffset(int argc, IDL_VPTR *argv, char
			*argk_passed, IDL_KW_PAR *kw_requested, IDL_VPTR *plain_args, int mask, void *kw_result) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
				static const int NoClean = 0;
		// this is always true with GDL:
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
		//		std::cerr << "Expected: ";
		//		for (it = requested.begin(); it != requested.end(); ++it) {
		//			std::cerr << "\"" << it->first << "\",";
		//		}
		//		std::cerr << std::endl;
		// scan argk for wrong keywords
		//		std::cerr << "Passed: \"";
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
			//			std::cerr << s << "\", \"";
			if (!found && !ignored) {
				DInterpreter::CallStackBack()->Throw("Invalid keyword " + std::string(s));
			} else {
				for (++it; it != requested.end(); ++it) { //search for ambiguous KW
					const char* expected_kw =  kw_requested[it->first].keyword;
					if (strncmp(expected_kw, s, l) == 0) DInterpreter::CallStackBack()->Throw("Ambiguous keyword abbreviation: " + std::string(s));
				}
			}
		}
		//		std::cerr << std::endl;
		//populate kw_result
		//first set IDL_KW_RESULT_FIRST_FIELD to zero (no need to clean)
		memcpy(kw_result, &NoClean, sizeof (int));
		//rewind: 
		for (it = requested.begin(); it != requested.end(); ++it) {
			int ipassed = it->second;
//			std::cerr <<  kw_requested[it->first].keyword << "->" <<it->second << std::endl;
			if (ipassed == -1) GdlExportAbsentKeyword(kw_requested[it->first], kw_result);
			else if (ipassed >= 0) {
				IDL_VPTR ret=GdlExportPresentKeyword(kw_requested[it->first], argk[it->second], kw_result);
				if (ret != NULL) {
					argk[ipassed].out=ret; //pass vptr back
				}
			}
		}
		return argc;
	}

	// not so simple
//	int IDL_CDECL IDL_KWProcessByAddr(int argc, IDL_VPTR *argv, char
//			*argk_passed, IDL_KW_PAR *kw_requested, IDL_VPTR *plain_args, int mask, void *kw_result) {
//	}

void *IDL_CDECL IDL_MemAlloc(IDL_MEMINT n, const char *err_str, int
			msg_action){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) return malloc(n);}
void *IDL_CDECL IDL_MemRealloc(void *ptr, IDL_MEMINT n, const char
 *err_str, int action){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) return realloc(ptr,n);}
void IDL_CDECL IDL_MemFree(IDL_REGISTER void *m, const char
 *err_str, int msg_action){free(m);}
void *IDL_CDECL IDL_MemAllocPerm(IDL_MEMINT n, const char *err_str,
        int action){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) GDL_WillThrowAfterCleaning("MemAllocPerm is not currently supported.");throw;}
char *IDL_CDECL IDL_GetScratch(IDL_REGISTER IDL_VPTR *p,
        IDL_REGISTER IDL_MEMINT n_elts,  IDL_REGISTER IDL_MEMINT elt_size){ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) return (char*)malloc (n_elts*elt_size);}
void IDL_CDECL IDL_KWCleanup(int fcn){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) IDL_KWFree();}
char *IDL_CDECL IDL_GetScratchOnThreshold(IDL_REGISTER char
        *auto_buf, IDL_REGISTER IDL_MEMINT auto_elts,  IDL_REGISTER IDL_MEMINT
        n_elts,  IDL_REGISTER IDL_MEMINT elt_size,  IDL_VPTR *tempvar){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
	GDL_WillThrowAfterCleaning("IDL_GetScratchOnThreshold is not implemented, FIXME.");
		throw;
}

IDL_TERMINFO IDL_TermInfo={
#ifdef IDL_OS_HAS_TTYS
  (char*)"Xterm" ,          /* Name of terminal type */
  1    ,             /* True if stdin is a terminal */
#endif
  24,128};

char *IDL_CDECL IDL_FileTermName(void){return IDL_TermInfo.name;}
int IDL_CDECL IDL_FileTermIsTty(void){return IDL_TermInfo.is_tty;}
int IDL_CDECL IDL_FileTermLines(void){return IDL_TermInfo.lines;}
int IDL_CDECL IDL_FileTermColumns(void){return IDL_TermInfo.columns;}

IDL_SYS_VERSION IDL_SysvVersion={{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL},0,0};
IDL_STRING *IDL_CDECL IDL_SysvVersionArch(void){return &(IDL_SysvVersion.arch);}
IDL_STRING *IDL_CDECL IDL_SysvVersionOS(void){return &(IDL_SysvVersion.os);};
IDL_STRING *IDL_CDECL IDL_SysvVersionOSFamily(void){return &(IDL_SysvVersion.os_family);};
IDL_STRING *IDL_CDECL IDL_SysvVersionRelease(void){return &(IDL_SysvVersion.release);};

char *IDL_ProgramName=(char*)"gdl";
char *IDL_CDECL IDL_ProgramNameFunc(void){return (char*)"gdl";}
char *IDL_ProgramNameLC=(char*)"gdl";
char *IDL_CDECL IDL_ProgramNameLCFunc(void){return (char*)"gdl";}
IDL_STRING IDL_SysvDir={0,0,NULL};
IDL_STRING *IDL_CDECL IDL_SysvDirFunc(void){return &IDL_SysvDir;}
IDL_LONG IDL_SysvErrCode=0;
IDL_LONG IDL_CDECL IDL_SysvErrCodeValue(void){return 0;};
IDL_SYS_ERROR_STATE IDL_SysvErrorState={{0,0,NULL},{0,0,NULL},0,{0,0},{0,0,NULL},{0,0,NULL},{0,0,NULL},{0,0,NULL}};
IDL_SYS_ERROR_STATE *IDL_CDECL IDL_SysvErrorStateAddr(void){return &IDL_SysvErrorState;}
IDL_STRING *IDL_CDECL IDL_SysvErrStringFunc(void){return &(IDL_SysvErrorState.msg);}
IDL_STRING *IDL_CDECL IDL_SysvSyserrStringFunc(void){return &(IDL_SysvErrorState.sys_msg);}
IDL_LONG IDL_CDECL IDL_SysvErrorCodeValue(void){return IDL_SysvErrorState.code;}
IDL_LONG *IDL_CDECL IDL_SysvSyserrorCodesAddr(void){return &(IDL_SysvErrorState.code);}
IDL_LONG IDL_SysvOrder=0;
IDL_LONG IDL_CDECL IDL_SysvOrderValue(void){return 0;}
float IDL_CDECL IDL_SysvValuesGetFloat(int type){return 0;}
int IDL_CDECL IDL_MessageNameToCode(IDL_MSG_BLOCK block, const char *name){return 0;}
IDL_MSG_BLOCK IDL_CDECL IDL_MessageDefineBlock(char *block_name, int n, IDL_MSG_DEF *defs){return NULL;} //do nothing
void IDL_CDECL IDL_MessageErrno(int code, ...){} //do nothing. obsoleted.
void IDL_CDECL IDL_MessageErrnoFromBlock(IDL_MSG_BLOCK block, int code, ...){} //do nothing. obsoleted.

	void IDL_CDECL IDL_Message(int code, int action, ...) {
		TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		std::string finalMessage = "";
		char* s;
		if (code == IDL_M_GENERIC || code == IDL_M_NAMED_GENERIC) {
			va_list args;
			va_start(args, action);
			s=va_arg(args,char*);
			if (s != NULL) {
				finalMessage += std::string(s);
			}
			va_end(args);
		} else GDL_WillThrowAfterCleaning("Invalid Error Code given to IDL_Message() by user-written routine.");
		if (action == IDL_MSG_LONGJMP || IDL_MSG_RET) GDL_WillThrowAfterCleaning(finalMessage);
		if (action == IDL_MSG_IO_LONGJMP) throw GDLIOException(finalMessage);
		if (action == IDL_MSG_EXIT) {
			Warning(finalMessage);
			GDL_WillThrowAfterCleaning("IDL_MSG_EXIT forbidden for user-written routines.");
		}
		if (action == IDL_MSG_INFO) Warning(finalMessage);
	}
	
void IDL_CDECL IDL_MessageFromBlock(IDL_MSG_BLOCK block, int code, int action,...){if (action!=IDL_MSG_INFO) GDL_WillThrowAfterCleaning("exception caused by non-GDL (dlm) function call.");}//do nothing.
void IDL_CDECL IDL_MessageSyscode(int code, IDL_MSG_SYSCODE_T syscode_type, int syscode, int action, ...){if (action!=IDL_MSG_INFO) GDL_WillThrowAfterCleaning("exception caused by non-GDL (dlm) function call.");}//do nothing.
void IDL_CDECL IDL_MessageSyscodeFromBlock(IDL_MSG_BLOCK block, int code, IDL_MSG_SYSCODE_T syscode_type,  int syscode, int action, ...){if (action!=IDL_MSG_INFO) GDL_WillThrowAfterCleaning("exception caused by non-GDL (dlm) function call.");}
void IDL_CDECL IDL_MessageVarError(int code, IDL_VPTR var, int action){if (action!=IDL_MSG_INFO) GDL_WillThrowAfterCleaning("exception caused by non-GDL (dlm) function call.");}
void IDL_CDECL IDL_MessageVarErrorFromBlock(IDL_MSG_BLOCK block, int code, IDL_VPTR var, int action){if (action!=IDL_MSG_INFO) GDL_WillThrowAfterCleaning("exception caused by non-GDL (dlm) function call.");}
void IDL_CDECL IDL_MessageResetSysvErrorState(void) {
		try {
			std::string command = ("message,/reset");
			DInterpreter::CallStackBack()->Interpreter()->ExecuteStringLine(command);
		} catch (...) {
		}
}
void IDL_CDECL IDL_MessageSJE(void *value){} //do nothing (?)
void *IDL_CDECL IDL_MessageGJE(void){return NULL;}//do nothing (?)
void IDL_CDECL IDL_MessageVE_UNDEFVAR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO) GDL_WillThrowAfterCleaning("Variable is Undefined.");}
void IDL_CDECL IDL_MessageVE_NOTARRAY(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Expression must be an array in this context.");}
void IDL_CDECL IDL_MessageVE_NOTSCALAR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Expression must be a scalar in this context.");}
void IDL_CDECL IDL_MessageVE_NOEXPR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Expression must be a named variable in this context");}
void IDL_CDECL IDL_MessageVE_NOCONST(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Constant not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOFILE(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("File expression not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOCOMPLEX(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Variable is not of complex type/");}
void IDL_CDECL IDL_MessageVE_NOSTRING(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("String expression not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOSTRUCT(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Struct expression not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_REQSTR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("String expression required in this context.");}
void IDL_CDECL IDL_MessageVE_NOSCALAR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Scalar variable not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NULLSTR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Null string not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOPTR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Pointer expression not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOOBJREF(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Object reference expression not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOMEMINT64(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("This routine is 32-bit limited and cannot handle this many elements.");}
void IDL_CDECL IDL_MessageVE_STRUC_REQ(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Expression must be a structure in this context.");}
void IDL_CDECL IDL_MessageVE_REQPTR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Pointer type required in this context.");}
void IDL_CDECL IDL_MessageVE_REQOBJREF(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Object reference required in this context");}
void IDL_CDECL IDL_Message_BADARRDNUM(int action){if (action!=IDL_MSG_INFO)GDL_WillThrowAfterCleaning("Arrays are allowed 1 - 8 dimensions");}
int IDL_CDECL IDL_SysRtnAdd(IDL_SYSFUN_DEF2 *defs, int is_function,int cnt){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)  return 1;}
IDL_MEMINT IDL_CDECL IDL_SysRtnNumEnabled(int is_function, int enabled){return 1;} //why not - this is a stub
void IDL_CDECL IDL_SysRtnGetEnabledNames(int is_function, IDL_STRING *str, int enabled){str->slen=0;};
void IDL_CDECL IDL_SysRtnEnable(int is_function, IDL_STRING *names, IDL_MEMINT n, int option,  IDL_SYSRTN_GENERIC disfcn){}// do nothing
IDL_SYSRTN_GENERIC IDL_CDECL IDL_SysRtnGetRealPtr(int is_function, char *name){return NULL;}
char *IDL_CDECL IDL_SysRtnGetCurrentName(void){return NULL;};
int IDL_CDECL IDL_LMGRLicenseInfo(int iFlags){return 1;}
int IDL_CDECL IDL_LMGRSetLicenseInfo(int iFlags){return 1;}
int IDL_CDECL IDL_LMGRLicenseCheckoutUnique(char *szFeature, char *szVersion){return 1;}
int IDL_CDECL IDL_LMGRLicenseCheckout(char *szFeature, char *szVersion){return 1;}
int IDL_CDECL IDL_Load(void){return 1;}
void IDL_CDECL IDL_Win32MessageLoop(int fFlush){}
int IDL_CDECL IDL_Win32Init(IDL_INIT_DATA_OPTIONS_T iOpts, void  *hinstExe, void *hwndExe, void *hAccel){return 1;}
void IDL_CDECL IDL_WinPostInit(void){}
void IDL_CDECL IDL_WinCleanup(void){}
#include <stdio.h>
#include <stdarg.h>

IDL_VPTR IDL_CDECL IDL_BasicTypeConversion(int argc, IDL_VPTR argv[], IDL_REGISTER int type){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	if (argc != 1) GDL_WillThrowAfterCleaning("FIXME IDL_BasicTypeConversion!");
	int i=0;
	IDL_ENSURE_SIMPLE(argv[i]);
	if (argv[i]->type == type) return argv[i];

	switch(type) {
		case IDL_TYP_BYTE:     return IDL_CvtByte(1, &argv[i]);
		case IDL_TYP_INT:     return IDL_CvtFix(1, &argv[i]);
		case IDL_TYP_LONG:    return IDL_CvtLng(1, &argv[i]);
		case IDL_TYP_FLOAT:    return IDL_CvtFlt(1, &argv[i]);
		case IDL_TYP_DOUBLE:    return IDL_CvtDbl(1, &argv[i]);
		case IDL_TYP_COMPLEX:    return IDL_CvtComplex(1, &argv[i]);
		case IDL_TYP_DCOMPLEX:    return IDL_CvtDComplex(1, &argv[i]);
		case IDL_TYP_UINT:    return IDL_CvtUInt(1, &argv[i]);
		case IDL_TYP_ULONG:    return IDL_CvtULng(1, &argv[i]);
		case IDL_TYP_LONG64:    return IDL_CvtLng64(1, &argv[i]);
		case IDL_TYP_ULONG64:    return IDL_CvtULng64(1, &argv[i]);
		case IDL_TYP_STRING:    return IDL_CvtString(1, &argv[i], NULL);
		default: GDL_WillThrowAfterCleaning("Wrong type in IDL_BasicTypeConversion!");
	}
	return NULL;
}
int IDL_AddSystemRoutine(IDL_SYSFUN_DEF *defs, int is_function, int cnt){return 1;} //using DLM insure this is OK. I think.
int IDL_CDECL IDL_BailOut(int stop){return sigControlC;} //use of stop not supported.
void IDL_CDECL IDL_ExitRegister(IDL_EXIT_HANDLER_FUNC proc){}
void IDL_CDECL IDL_ExitUnregister(IDL_EXIT_HANDLER_FUNC proc){}
int IDL_CDECL IDL_GetExitStatus(){return 0;} //do nothing
int IDL_CDECL IDL_Cleanup(int just_cleanup){return 1;} //do nothing
int IDL_CDECL IDL_Initialize(IDL_INIT_DATA *init_data){return 1;} //do nothing
int IDL_CDECL IDL_Init(IDL_INIT_DATA_OPTIONS_T options, int *argc, char *argv[]){return 1;} //do nothing
int IDL_CDECL IDL_Main(IDL_INIT_DATA_OPTIONS_T options, int argc, char *argv[]){return 1;} //do nothing
int IDL_CDECL IDL_ExecuteStr(char *cmd) {
  std::string command(cmd);
 //always between try{} catch{} when calling ExecuteStringLine!
  try {
  DInterpreter::CallStackBack()->Interpreter()->ExecuteStringLine(command);
  } catch (...) {std::cerr<<"Problem executing command: "<<command<<" ."<<std::endl; return 0;}
  return 1;
 }
int IDL_CDECL IDL_Execute(int argc, char *argv[]){
	std::string command(argv[0]);
	for (auto i=1;i< argc; ++i) {command.append(" & "); command.append(argv[i]);}
  try {
  DInterpreter::CallStackBack()->Interpreter()->ExecuteStringLine(command);
  } catch (...) {std::cerr<<"Problem executing command: "<<std::string(argv[0])<<"(...)"<<std::endl; return 0;}
  return 1;
}
int IDL_CDECL IDL_RuntimeExec(char *file){Warning("IDL_RuntimeExec function not allowed in GDL.");return 0;}
void IDL_CDECL IDL_Runtime(IDL_INIT_DATA_OPTIONS_T options, int *argc, char *argv[], char *file){GDL_WillThrowAfterCleaning("IDL_Runtime function not allowed in GDL.");}

#define ADJUST_ELEMENT_OFFSET(x) {IDL_MEMINT l=x;\
IDL_MEMINT excess=returned_struct->tags[itag].offset % l;\
if (excess != 0) {\
IDL_MEMINT pad=l-excess;\
returned_struct->tags[itag].offset+=pad;\
}}

// At the moment the "named" structure exist only INSIDE the user program. It is not registered in $MAIN$.
// Will do this if the need appears.

	IDL_STRUCTURE* IDL_CDECL IDL_MakeStruct(char *name, IDL_STRUCT_TAG_DEF *tags) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
			//count tags (?)
			int ntags = 0;
	while (tags[ntags++].name != NULL) {
	};
	ntags--;
	// create structure with extended size after (see https://stackoverflow.com/questions/6390331/why-use-array-size-1-instead-of-pointer)
	IDL_STRUCTURE *returned_struct = (IDL_STRUCTURE *) calloc(1, sizeof (IDL_STRUCTURE) + ntags * sizeof (IDL_TAGDEF));
	if (name) {
		IDL_IDENT *iid = (IDL_IDENT*) calloc(1, sizeof (IDL_IDENT));
		ExportedGlobalNamedStructList.push_back(std::make_pair(std::string(name), returned_struct));
		returned_struct->id = iid;
		iid->hash = NULL;
		iid->name = name;
		iid->len = strlen(name);
	} else returned_struct->id = NULL;
	returned_struct->flags = 0;
	returned_struct->ntags = ntags;
	returned_struct->length = 0;
	returned_struct->data_length = 0; 
	returned_struct->contains_string = false;
	struct _idl_ident *memhash = NULL;
	// compute alignment, this is the difference between length and data_length
	//"on modern machines each data item must usually be ``self-aligned'', 
	// beginning on an address that is a multiple of its type size. 
	// Thus, 32-bit types must begin on a 32-bit boundary, 16-bit types on a 16-bit boundary, 
	// 8-bit types may begin anywhere, struct/array/union types have the alignment of their most restrictive member. 
	for (int itag = 0; itag < ntags; ++itag) {
		IDL_STRUCT_TAG_DEF def = tags[itag];
		IDL_IDENT *tagid = (IDL_IDENT*) calloc(1, sizeof (IDL_IDENT));
		returned_struct->tags[itag].id = tagid;
		tagid->name = def.name;
		tagid->len = strlen(def.name);
		tagid->hash = memhash;
		memhash = tagid; //chain
		returned_struct->tags[itag].offset = returned_struct->length; //std::cerr<<"offset for "<<itag<<": "<<returned_struct->tags[itag].offset<<std::endl;
		void* thetypePtr = def.type;
		UCHAR realType = 0;
		if (thetypePtr == NULL) {
			realType = IDL_TYP_STRUCT;
//			ADJUST_ELEMENT_OFFSET(GDL_TypeAlignment[realType])
			/*If this
		   field is NULL, it indicates that IDL
		   should search for a structure of the
		   given name and fill in the pointer to
		   its structure definition. */
			if (def.name == NULL) GDL_WillThrowAfterCleaning("IDL_MakeStruct(): no name for inherited structure!");
			bool found = false;
			for (std::vector<std::pair < std::string, IDL_STRUCTURE*>>::iterator it = ExportedGlobalNamedStructList.begin(); it != ExportedGlobalNamedStructList.end(); ++it) {
				if (it->first == std::string(def.name)) {
					found = true;
					def.type = (void*) (it->second);
					returned_struct->length += (it->second)->length;
					returned_struct->data_length += (it->second)->data_length;
					returned_struct->tags[itag].var.flags &= ~IDL_V_TEMP; //known global ??
					returned_struct->tags[itag].var.flags |= IDL_V_STRUCT;
				    returned_struct->tags[itag].var.type = realType;
					returned_struct->tags[itag].var.value.s.sdef=(it->second);
					break;
				}
			}
			if (!found) GDL_WillThrowAfterCleaning("IDL_MakeStruct(): inherited structure not found.");
		} else {
			/* This may be either a pointer to another
		   structure definition, or a simple IDL
		   type code (IDL_TYP_*) cast to void
		   (e.g. (void *) IDL_TYP_BYTE)*/
			if ((size_t) thetypePtr > IDL_MAX_TYPE) { /* a structure ptr */
				realType = IDL_TYP_STRUCT;
//				ADJUST_ELEMENT_OFFSET(GDL_TypeAlignment[realType])
				returned_struct->tags[itag].var.type = realType;
				returned_struct->tags[itag].var.flags |= IDL_V_STRUCT;
				returned_struct->tags[itag].var.value.s.sdef=(IDL_STRUCTURE*)thetypePtr;
				IDL_LONG64 strulen=((IDL_STRUCTURE*) (thetypePtr))->length;
				IDL_LONG64 strudata_len=((IDL_STRUCTURE*) (thetypePtr))->data_length;
				if (def.dims == NULL) {
					returned_struct->length += strulen;
					returned_struct->data_length += strudata_len;
				} else {
					returned_struct->tags[itag].var.flags |= IDL_V_ARR;
					returned_struct->tags[itag].var.value.arr = new IDL_ARRAY();
					IDL_LONG64 ndim = def.dims[0];
					returned_struct->tags[itag].var.value.arr->n_dim = ndim;
					size_t l = 1;
					for (auto i = 1; i < ndim + 1; ++i) {
						returned_struct->tags[itag].var.value.arr->dim[i - 1] = def.dims[i];
						if (def.dims[i] != 0) l *= def.dims[i]; 
					}
					returned_struct->tags[itag].var.value.arr->arr_len = l * strulen; 
					returned_struct->tags[itag].var.value.arr->elt_len = strulen;
					returned_struct->tags[itag].var.value.arr->n_elts = l;
					returned_struct->data_length += l * strudata_len;
					returned_struct->length += l * strulen;
				}
			} else {
				size_t thetype = (size_t) thetypePtr;
				realType = thetype;
				ADJUST_ELEMENT_OFFSET(GDL_TypeAlignment[realType])
				if (realType == IDL_TYP_STRING || realType == IDL_TYP_PTR) returned_struct->contains_string = true;
				returned_struct->tags[itag].var.type = realType;
				if (def.dims == NULL) {
					returned_struct->data_length += IDL_TypeSize[realType];
					returned_struct->length += IDL_TypeSize[realType]; //std::cerr<<"+"<<IDL_TypeSize[realType]<<std::endl;
				} else {
					returned_struct->tags[itag].var.flags |= IDL_V_ARR;
					returned_struct->tags[itag].var.value.arr = new IDL_ARRAY();
					IDL_LONG64 ndim = def.dims[0];
					returned_struct->tags[itag].var.value.arr->n_dim = ndim;
					size_t l = 1;
					for (auto i = 1; i < ndim + 1; ++i) {
						returned_struct->tags[itag].var.value.arr->dim[i - 1] = def.dims[i];
						if (def.dims[i] != 0) l *= def.dims[i]; //std::cerr<<i<<":"<<l<<","<<def.dims[i]<<std::endl;
					}
					returned_struct->tags[itag].var.value.arr->arr_len = l * IDL_TypeSize[realType]; //std::cerr<<l* IDL_TypeSize[realType]<<std::endl;
					returned_struct->tags[itag].var.value.arr->elt_len = IDL_TypeSize[realType];
					returned_struct->tags[itag].var.value.arr->n_elts = l;
					returned_struct->data_length += l * IDL_TypeSize[realType];
					returned_struct->length += l * IDL_TypeSize[realType];
				}
			}
		}
	}
	//we *could* make the difference between packed and unpacked...
	IDL_MEMINT tmp=returned_struct->length % sizeof(IDL_MEMINT);
	if (tmp != 0) { 
		tmp=returned_struct->length/sizeof(IDL_MEMINT); //alignment needed as structure data is initialzed as such.
		returned_struct->length=(tmp+1)*sizeof(IDL_MEMINT);
	}
	returned_struct->rcount = 1;
	returned_struct->object = NULL;
	return returned_struct;
	}
#undef ADJUST_ELEMENT_OFFSET

	IDL_MEMINT IDL_CDECL IDL_StructTagInfoByName(IDL_StructDefPtr sdef, char *name, int msg_action, IDL_VPTR *var) {
		int l = strlen(name);
		for (auto i = 0; i < sdef->ntags; ++i) {
			if (sdef->tags[i].id->len == l && strncmp(name, sdef->tags[i].id->name, l)) {
			 *var=&(sdef->tags[i].var);
			}
			return sdef->tags[i].offset;
		}
		char* mess=(char*)calloc(256,1);
		strncat(mess,"Tag name ",10);strncat(mess,name,strlen(name));strncat(mess," is undefined for structure ",29);
		if (sdef->id!=NULL && sdef->id->name !=NULL) strcat(mess,sdef->id->name); else strcat(mess,"<Anonymous>");
		IDL_Message(IDL_M_GENERIC, msg_action, mess);
		free(mess);
		return 0;
	}

	IDL_MEMINT IDL_CDECL IDL_StructTagInfoByIndex(IDL_StructDefPtr   sdef, int index, int msg_action, IDL_VPTR *var){
		if (sdef->ntags > index) {
			 *var=&(sdef->tags[index].var);
			return sdef->tags[index].offset;
		}
		char* mess=(char*)calloc(256,1);
		strncat(mess,"Tag number ",12);snprintf(mess,64,"%d",index); strncat(mess," is undefined for structure ",29);
		if (sdef->id!=NULL && sdef->id->name !=NULL) strcat(mess,sdef->id->name); else strcat(mess,"<Anonymous>");
		IDL_Message(IDL_M_GENERIC, msg_action, mess);
		free(mess);
		return 0;
	}
extern char *IDL_CDECL IDL_StructTagNameByIndex(IDL_StructDefPtr sdef, int index, int msg_action, char **struct_name){
		if (sdef->ntags > index) {
			if (struct_name != NULL) {if  (sdef->id!=NULL && sdef->id->name !=NULL) *struct_name=sdef->id->name; else *struct_name=(char*)"<Anonymous>";}
			return sdef->tags[index].id->name;
		}
		char* mess=(char*)calloc(256,1);
		strncat(mess,"Tag number ",12);snprintf(mess,64,"%d",index); strncat(mess," is undefined for structure ",29);
		if (sdef->id!=NULL && sdef->id->name !=NULL) strcat(mess,sdef->id->name); else strcat(mess,"<Anonymous>");
		IDL_Message(IDL_M_GENERIC, msg_action, mess);
		free(mess);
		return NULL;
}
int IDL_CDECL IDL_StructNumTags(IDL_StructDefPtr sdef){return sdef->ntags;}

};
#endif
