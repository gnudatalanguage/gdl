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
#define IDL_TYP_COMPLEXDBL IDL_TYP_DCOMPLEX

// list of memory (strings...) to be released when IDL_KWFree() is called
static std::vector<char*> export_dealloc;
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

inline void ZeroVPTR(IDL_VPTR p) {
	memset((void*)p,0,sizeof(p));
}

inline IDL_VPTR NewTMPVPTR(UCHAR flag) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	IDL_VPTR ret=new IDL_VARIABLE();
	ret->type=IDL_TYP_UNDEF;
	ret->flags=IDL_V_TEMP|flag;
	if ( flag & IDL_V_CONST) {
		ret->flags |= IDL_V_CONST;
	}
	if ( (flag & IDL_V_ARR) || (flag & IDL_V_STRUCT) ) {
		IDL_ARRAY *a=new IDL_ARRAY();
		ret->flags |= IDL_V_DYNAMIC;
		a->flags=IDL_A_NO_GUARD;
		ret->value.arr=a;
	}
	return ret;
}
inline IDL_VPTR NewTMPVPTRARRAY() {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	IDL_VPTR ret=new IDL_VARIABLE();
	ret->type=IDL_TYP_UNDEF;
	ret->flags=IDL_V_TEMP | IDL_V_ARR | IDL_V_DYNAMIC;
	IDL_ARRAY *a=new IDL_ARRAY();
	a->flags=IDL_A_NO_GUARD;
	ret->value.arr=a;
	return ret;
}
inline IDL_VPTR NewTMPVPTRSTRUCT() {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	IDL_VPTR ret=new IDL_VARIABLE();
	ret->type=IDL_TYP_STRUCT;
	ret->flags=IDL_V_TEMP| IDL_V_DYNAMIC | IDL_V_ARR;
	return ret;
}
inline IDL_VPTR NewTMPVPTRSTRING() {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	IDL_VPTR ret=new IDL_VARIABLE();
	ret->type=IDL_TYP_STRING;
	ret->flags=IDL_V_TEMP | IDL_V_DYNAMIC;
	return ret;
}

	inline IDL_VPTR NewTMPVPTR() {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	IDL_VPTR ret=new IDL_VARIABLE();
	ret->flags=IDL_V_TEMP;
	ret->type=IDL_TYP_UNDEF;
	return ret;
}	
	inline IDL_VPTR NewSTATICVPTR() {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	IDL_VPTR ret=new IDL_VARIABLE();
	ret->type=IDL_TYP_UNDEF;
	return ret;
}
	IDL_VPTR GDL_ToVPTR(BaseGDL* var, bool tempo=false) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
    IDL_VPTR v;
    if (tempo) v=NewTMPVPTR(); else v=NewSTATICVPTR(); 
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
      if (tempo) v->flags |= IDL_V_CONST;
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
        case GDL_PTR:
        {
          v->type = IDL_TYP_PTR;
		  v->value.ptrint=(*static_cast<DPtrGDL*>(var))[0];
          break;
        }
		  default: throw GDLException("GDL_ToVPTR: unsupported case.");
      }
    } else {
      v->flags |= IDL_V_ARR;
      v->flags |= IDL_V_DYNAMIC;
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
			export_dealloc.push_back((char*)allstringdescr);
			void* stringdescPtrs=malloc(nEl*sizeof(IDL_STRING*));
			export_dealloc.push_back((char*)stringdescPtrs);
			arraydescr->data = (UCHAR*) (stringdescPtrs);
			IDL_STRING** p=(IDL_STRING**)stringdescPtrs;
			for (SizeT i=0; i< nEl; ++i) p[i]=(IDL_STRING*)((SizeT)allstringdescr+(i*sizeof(IDL_STRING)));
			DStringGDL* gdlstr=(DStringGDL*)var;
			for (auto i=0; i< nEl; ++i) {
				p[i]->slen = ((*gdlstr)[i]).size();
				p[i]->s = (char*) malloc(p[i]->slen + 1);
				export_dealloc.push_back(p[i]->s);
				strncpy(p[i]->s, (*gdlstr)[i].c_str(), p[i]->slen + 1);
			}
          break;
        }
        default: throw GDLException("GDL_ToVPTR: unsupported case.");

      }
    }
    return v;
  } 
  
#define DOCASE(type, gdltype, element)\
 case type: return new gdltype(v->value.element); break;

#define DOCASE_ARRAY(type, gdltype)\
 case type: var = new gdltype(dim, BaseGDL::NOALLOC); break;
	
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
  
BaseGDL* VPTR_ToGDL(IDL_VPTR v) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	if (v == NULL) {
		throw GDLException("Invalid IDL_VPTR used.");
	}
	if (v->flags & IDL_V_NULL) {
		return NullGDL::GetSingleInstance();
	}
	if ((v->flags & IDL_V_ARR) == IDL_V_ARR) {
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
//				DOCASE_ARRAY(IDL_TYP_STRING, DStringGDL);
			case IDL_TYP_STRUCT: {
				throw GDLException("VPTR_ToGDL: Write STRUCT case!.");
			break;
			}
			default: throw GDLException("VPTR_ToGDL: bad array case.");
		}
//		if ((SizeT) (arraydescr->data) % (16 * sizeof(size_t))) std::cerr << "unaligned\n";
		var->SetBuffer(arraydescr->data);
		var->SetBufferSize(dim.NDimElements());
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
				DOCASE(IDL_TYP_STRING, DStringGDL, str.s);
			case IDL_TYP_COMPLEX:
				return new DComplexGDL(std::complex<float>(v->value.cmp.r, v->value.cmp.i));
				break;
			case IDL_TYP_DCOMPLEX:
				return new DComplexDblGDL(std::complex<double>(v->value.dcmp.r, v->value.dcmp.i));
				break;
			default: throw GDLException("ReturnIDL_VPTR_AsGDL: bad array case.");
		}
	}
	throw;
}
#undef DOCASE
#undef DOCASE_ARRAY

#define C_ (char*)
char *IDL_OutputFormat[IDL_NUM_TYPES]={C_ "<Undefined>",C_ "%4d",C_ "%8d",C_ "%12d",C_ "%#13.6g",C_ "%#16.8g",
C_ "(%#13.6g,%#13.6g)",C_ "%s",C_ "",C_ "(%#16.8g,%#16.8g)",C_ "%12lu",C_ "%12lu",C_ "%8u",C_ "%12u",C_ "%22ld"};
char *IDL_CDECL IDL_OutputFormatFunc(int type){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		if (type > IDL_MAX_TYPE) throw GDLException("type must be > 0 and < 15");
		return IDL_OutputFormat[type];
}
int IDL_OutputFormatLen[IDL_NUM_TYPES]={11,4,8,12,13,16,29,0,0,35,12,12,8,12,22};
int IDL_CDECL IDL_OutputFormatLenFunc(int type){TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		if (type > IDL_MAX_TYPE) throw GDLException("type must be > 0 and < 15");
		return IDL_OutputFormatLen[type];
}
char *IDL_OutputFormatFull[IDL_NUM_TYPES]={C_"<Undefined>",C_"%4d",C_"%8d",C_"%12d",C_"%#16.8g",C_"%#25.17g",
C_"(%#16.8g,%#16.8g)",C_"%s",C_"",C_"(%#25.17g,%#25.17g)",C_"%12lu",C_"%12lu",C_"%8u",C_"%12u",C_"%22ld"};
int IDL_OutputFormatFullLen[IDL_NUM_TYPES]={11,4,8,12,16,25,35,0,0,53,12,12,8,12,22};

char *IDL_OutputFormatNatural[IDL_NUM_TYPES]={C_"<Undefined>",C_"%d",C_"%d",C_"%d",C_"%g",C_"%g",C_"(%g,%g)",C_"%s",C_"",C_"(%g,%g)",C_"%u",C_"%u",C_"%u",C_"%u",C_"%ld"};

	IDL_LONG IDL_TypeSize[IDL_NUM_TYPES]={0, 1, sizeof (IDL_INT), sizeof (IDL_LONG), sizeof (float), sizeof (double),
		sizeof (IDL_COMPLEX), sizeof (IDL_STRING), sizeof (IDL_SREF), sizeof (IDL_DCOMPLEX), sizeof (IDL_MEMINT), sizeof (IDL_MEMINT),
		sizeof ( IDL_UINT), sizeof (IDL_ULONG), sizeof (IDL_LONG64), sizeof (IDL_ULONG64)};
	
	inline int IDL_TypeSizeFunc(int type){ TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		if (type > IDL_MAX_TYPE) throw GDLException("type must be > 0 and < 15");
		return IDL_TypeSize[type];}

char *IDL_TypeName[]={C_"UNDEFINED",C_"BYTE     ",C_"INT      ",C_"LONG     ",C_"FLOAT    ",C_"DOUBLE   ",C_"COMPLEX  ",C_"STRING   ",C_"STRUCT   ",C_"DCOMPLEX ",C_"POINTER  ",C_"OBJREF   ",C_"UINT     ",C_"ULONG    ",C_"LONG64   "};
char *IDL_CDECL IDL_TypeNameFunc(int type){ TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
		if (type > IDL_MAX_TYPE) throw GDLException("type must be > 0 and < 15");
		return IDL_TypeName[type];}

void GDL_Print(std::string &printPro, int nPar, IDL_VPTR *argv, DLong unit=0, bool useUnit=false) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		SizeT printIx = LibProIx(printPro);
      EnvT* env = new EnvT(NULL, libProList[printIx]);
      Guard<EnvT> guard(env);
   	// add parameters
       if (useUnit) env->SetNextPar( new DLongGDL(unit));
		for (auto i=0; i< nPar; ++i) {
          env->SetNextPar(VPTR_ToGDL(argv[i]));
		}
	  static_cast<DLibPro*>(env->GetPro())->Pro()( env);
	}

IDL_VPTR IDL_CDECL IDL_ImportArray(int n_dim, IDL_MEMINT dim[], int type, UCHAR *data, IDL_ARRAY_FREE_CB free_cb,  IDL_StructDefPtr s){
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	IDL_VPTR v = NewTMPVPTRARRAY();
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
	v->value.arr->data = data;
	return v;
}
IDL_VPTR IDL_CDECL IDL_ImportNamedArray(char *name, int n_dim, IDL_MEMINT dim[],  int type, UCHAR *data,  IDL_ARRAY_FREE_CB free_cb, IDL_StructDefPtr s){
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	IDL_VPTR v = NewTMPVPTRARRAY();
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
	v->value.arr->data = data;
	BaseGDL* gdlvar = VPTR_ToGDL(v);
	restoreNormalVariable(std::string(name), gdlvar);
	return v;
}
 
#define DOCASE(type, gdltype)\
 case type: return new gdltype(dimension(nelts), mode); break;

  BaseGDL* CreateNewGDLVector(int type, SizeT nelts, BaseGDL::InitType mode) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
			throw GDLException("CreateNewGDLVector failure.");
	}
	throw;
}
#undef DOCASE

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
			throw GDLException("CreateNewGDLArray failure.");
	}
	throw;
}
#undef DOCASE
  // for use inside source of LINKIMAGE and other DLMs
extern "C" {
	//all the IDL_** modules below should be listed in dynlist.txt ---now done using an asterisk syntax 
char *IDL_CDECL IDL_VarName(IDL_VPTR v){
		char* s=(char*) calloc(1,128); 
		strncat(s,"<",2);
		strncat(s,IDL_TypeNameFunc(v->type),9);
		if (v->flags & IDL_V_ARR) {
			strncat(s,"Array[",7);
			int i;
			for (i=0; i< v->value.arr->n_dim-1; ++i) {
				int l=strlen(s);
				sprintf (&s[l], "%d,",v->value.arr->dim[i]);
			}
			int l = strlen(s);
			sprintf(&s[l], "%d", v->value.arr->dim[i]);
			strncat(s,"]>",3);
		}
		return s;
	}

void IDL_VarEnsureSimple(IDL_VPTR v) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
if (v->flags == 0) return;
    static char* message= (char*)"Expression must be a scalar in this context: ";
	if ( !( v->flags & IDL_TYP_B_SIMPLE) || ( v->flags & IDL_V_ARR) ) IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, message, IDL_VarName(v), NULL);
}
	
void IDL_KWFree(void){
		for (std::vector<char*>::iterator it = export_dealloc.begin(); it != export_dealloc.end(); ++it) free(*it);
		export_dealloc.clear();
	}; 

void IDL_CDECL IDL_Print(int argc, IDL_VPTR *argv, char *argk){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		//argk is to be set to NULL by users according to the doc.
		std::string s("PRINT");
		GDL_Print(s, argc, argv);
	}

void IDL_CDECL IDL_PrintF(int argc, IDL_VPTR *argv, char *argk){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		std::string s("PRINTF");
		GDL_Print(s, argc-1, &(argv[1]),argv[0]->value.l, true);
	}

void IDL_CDECL IDL_StrStore(IDL_STRING *s, const char *fs){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
	s->slen=strlen(fs);
	s->stype=0;
	s->s=(char*)malloc(s->slen);
	strncpy(s->s,fs,s->slen);
}
// str below is a supposed to be a copy of the string descriptor(s).
// to properluy duplicate, oen has to ceate copies of the string(s) and update the descripto(s)
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
		IDL_VPTR ss = NewTMPVPTRSTRING(); 
		ss->value.str.slen = strlen(s);
		ss->value.str.stype = 0;
		ss->value.str.s=(char*)s;
		return ss;
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "IDL_StoreScalar: unexpected type "+i2s(type));
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "IDL_StoreScalar: unexpected type "+i2s(type));
		}
	}
#undef DOCASE
#undef DOCASE_CMP
//scalar only

void IDL_CDECL IDL_VarCopy(IDL_REGISTER IDL_VPTR src, IDL_REGISTER
        IDL_VPTR dst){
	if (dst->value.arr != NULL) {
		free(dst->value.arr);
	}
	dst->flags = src->flags;
	dst->type = src->type;
	if (src->flags & IDL_V_TEMP) {
		dst->value = src->value;
	    free(src);
	} else {
		dst->value = src->value;
		if (src->flags & IDL_V_STRUCT) { //must copy
		  throw GDLException("IDL_VarCopy not yet ready for structure, FIXME.");
		} else if (src->flags & IDL_V_ARR) { //must copy
				IDL_ARRAY *a=new IDL_ARRAY();
	            a->flags=IDL_A_NO_GUARD;
	            dst->value.arr=a;
				dst->value.arr->n_dim = src->value.arr->n_dim;
				dst->value.arr->n_elts = src->value.arr->n_elts;
				for (auto i = 0; i < src->value.arr->n_dim; ++i) dst->value.arr->dim[i] = src->value.arr->dim[i];
				IDL_LONG64 sz = IDL_TypeSizeFunc(src->type);
				dst->value.arr->elt_len = sz;
				SizeT l = sz;
				for (auto i = 0; i < src->value.arr->n_dim; ++i) l *= src->value.arr->dim[i];
				dst->value.arr->arr_len = l;
				void * addr = malloc(l);
				dst->value.arr->data = (UCHAR*) addr;
				memcpy(dst->value.arr->data,src->value.arr->data,l);
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "IDL_DoubleScalar: unexpected type "+i2s(v->type));
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "IDL_ULongScalar: unexpected type "+i2s(v->type));
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "IDL_LongScalar: unexpected type "+i2s(v->type));
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "IDL_Long64Scalar: unexpected type "+i2s(v->type));
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "IDL_ULong64Scalar: unexpected type "+i2s(v->type));
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "IDL_ULong64Scalar: unexpected type "+i2s(v->type));
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "IDL_ULong64Scalar: unexpected type "+i2s(v->type));
		}
		throw;
	}
#undef DOCASE
#undef DOCASE_CMP
void IDL_CDECL IDL_VarGetData(IDL_VPTR v, IDL_MEMINT *n, char **pd,
        int ensure_simple){
	if (ensure_simple) IDL_ENSURE_SIMPLE(v);
	if (v->flags & IDL_V_ARR) {
		*n=v->value.arr->n_elts;
		*pd=(char*) v->value.arr->data;
	} else {
		*n=1;
		*pd=(char*) &(v->value);
	}
	
}
char* IDL_CDECL IDL_MakeTempVector(int type, IDL_MEMINT dim, int  init, IDL_VPTR *var){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		IDL_VPTR v=NewTMPVPTRARRAY();
		*var=v;
		v->type=type;
		v->value.arr->dim[0]=dim;
		v->value.arr->n_elts=dim;
		v->value.arr->n_dim=1;
		IDL_LONG64 sz=IDL_TypeSizeFunc(type);
		v->value.arr->elt_len = sz;
		if ( init == IDL_ARR_INI_INDEX) {
			BaseGDL* temp=CreateNewGDLVector(type, dim, BaseGDL::INDGEN);
			v->value.arr->data = (UCHAR*) temp->DataAddr();
			v->value.arr->arr_len=temp->NBytes();
			return (char*) temp->DataAddr();
		}
		SizeT l=dim*sz;
		void * addr=malloc(l);
		v->value.arr->arr_len=l;
		v->value.arr->data = (UCHAR*) addr;
		if (init == IDL_ARR_INI_ZERO) memset((void*)addr, 0, l); 
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
		if ( init == IDL_ARR_INI_INDEX) {
			SizeT dim64[n_dim];
			for (auto i=0; i<n_dim; ++i) dim64[i]=dim[i]; 
			BaseGDL* temp=CreateNewGDLArray(type, n_dim, dim64, BaseGDL::INDGEN);
			v->value.arr->data = (UCHAR*) temp->DataAddr();
			return (char*) temp->DataAddr();
		}
		l=sz; for (auto i=0; i<n_dim; ++i) l*=dim[i];
		void * addr=malloc(l);
		v->value.arr->arr_len=l;
		v->value.arr->data = (UCHAR*) addr;
		if (init == IDL_ARR_INI_ZERO)  memset((void*)addr, 0, l);
		return (char*) addr;	
}
char *IDL_CDECL IDL_VarMakeTempFromTemplate(IDL_VPTR template_var, int type, IDL_StructDefPtr sdef,  IDL_VPTR *result_addr, int zero){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
if (sdef != NULL) throw GDLException("IDL_VarMakeTempFromTemplate not yet ready for structure, FIXME.");
		IDL_VPTR t=template_var;
		if (t->flags & IDL_V_ARR) {
			IDL_VPTR v = NewTMPVPTRARRAY();
			*result_addr = v;
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
			IDL_VPTR v = NewTMPVPTR(t->flags);
			*result_addr = v;
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
IDL_VPTR IDL_CDECL IDL_GettmpLong64(IDL_LONG64 value);
IDL_VPTR IDL_CDECL IDL_GettmpULong64(IDL_ULONG64 value);
IDL_VPTR IDL_GettmpFILEINT(IDL_FILEINT value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_FILEINT, fileint);
	}
IDL_VPTR IDL_GettmpMEMINT(IDL_MEMINT value){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
      DOIT(IDL_TYP_MEMINT, memint);
	}
#undef DOIT

#define DEFOUT(idl_dst_type)\
    if (argc != 1) IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP,"IDL_CvtXXX: fixme.");\
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
		PREPARE_ARRAY(char);		
	    switch (argv[0]->type) {
		DOCASE_ARRAY(IDL_TYP_BYTE, char);
		DOCASE_ARRAY(IDL_TYP_INT, short);
		DOCASE_ARRAY(IDL_TYP_LONG, long );
		DOCASE_ARRAY(IDL_TYP_FLOAT, float); 
		DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
		DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float );
		DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double );
		DOCASE_ARRAY(IDL_TYP_UINT, uint );
		DOCASE_ARRAY(IDL_TYP_ULONG, ulong );
		DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64 );
		DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP,"unexpected type");
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
		default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP,"unexpected type");
		}
	}
	return ret;
}

IDL_VPTR IDL_CvtBytscl(int argc, IDL_VPTR argv[], char *argk) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(IDL_TYP_BYTE);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(char);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, char);
					DOCASE_ARRAY(IDL_TYP_INT, short);
					DOCASE_ARRAY(IDL_TYP_LONG, long);
					DOCASE_ARRAY(IDL_TYP_FLOAT, long);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, uint);
					DOCASE_ARRAY(IDL_TYP_ULONG, ulong);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
			}
		}
		return ret;
}
//
IDL_VPTR IDL_CvtFix(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		DEFOUT(IDL_TYP_INT);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(short);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, char);
					DOCASE_ARRAY(IDL_TYP_INT, short);
					DOCASE_ARRAY(IDL_TYP_LONG, long);
					DOCASE_ARRAY(IDL_TYP_FLOAT, long);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, uint);
					DOCASE_ARRAY(IDL_TYP_ULONG, ulong);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
			}
		}
		return ret;

}
//
IDL_VPTR IDL_CvtUInt(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(IDL_TYP_UINT);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(uint);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, char);
					DOCASE_ARRAY(IDL_TYP_INT, short);
					DOCASE_ARRAY(IDL_TYP_LONG, long);
					DOCASE_ARRAY(IDL_TYP_FLOAT, long);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, uint);
					DOCASE_ARRAY(IDL_TYP_ULONG, ulong);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
		}
	}
		return ret;
	}
//
IDL_VPTR IDL_CvtLng(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(IDL_TYP_LONG);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(long);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, char);
					DOCASE_ARRAY(IDL_TYP_INT, short);
					DOCASE_ARRAY(IDL_TYP_LONG, long);
					DOCASE_ARRAY(IDL_TYP_FLOAT, long);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, uint);
					DOCASE_ARRAY(IDL_TYP_ULONG, ulong);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
		}
	}
		return ret;

	}
//
IDL_VPTR IDL_CvtULng(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(IDL_TYP_ULONG);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(ulong);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, char);
					DOCASE_ARRAY(IDL_TYP_INT, short);
					DOCASE_ARRAY(IDL_TYP_LONG, long);
					DOCASE_ARRAY(IDL_TYP_FLOAT, long);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, uint);
					DOCASE_ARRAY(IDL_TYP_ULONG, ulong);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
					DOCASE_ARRAY(IDL_TYP_BYTE, char);
					DOCASE_ARRAY(IDL_TYP_INT, short);
					DOCASE_ARRAY(IDL_TYP_LONG, long);
					DOCASE_ARRAY(IDL_TYP_FLOAT, long);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, uint);
					DOCASE_ARRAY(IDL_TYP_ULONG, ulong);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
					DOCASE_ARRAY(IDL_TYP_BYTE, char);
					DOCASE_ARRAY(IDL_TYP_INT, short);
					DOCASE_ARRAY(IDL_TYP_LONG, long);
					DOCASE_ARRAY(IDL_TYP_FLOAT, long);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, uint);
					DOCASE_ARRAY(IDL_TYP_ULONG, ulong);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
		}
	}
		return ret;

	}
//
IDL_VPTR IDL_CvtFlt(int argc, IDL_VPTR argv[]) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
        DEFOUT(IDL_TYP_FLOAT);
		if (argv[0]->flags & IDL_V_ARR) {
			PREPARE_ARRAY(float);
			switch (argv[0]->type) {
					DOCASE_ARRAY(IDL_TYP_BYTE, char);
					DOCASE_ARRAY(IDL_TYP_INT, short);
					DOCASE_ARRAY(IDL_TYP_LONG, long);
					DOCASE_ARRAY(IDL_TYP_FLOAT, long);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, uint);
					DOCASE_ARRAY(IDL_TYP_ULONG, ulong);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
			default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
					DOCASE_ARRAY(IDL_TYP_BYTE, char);
					DOCASE_ARRAY(IDL_TYP_INT, short);
					DOCASE_ARRAY(IDL_TYP_LONG, long);
					DOCASE_ARRAY(IDL_TYP_FLOAT, long);
					DOCASE_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, float);
					DOCASE_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, double);
					DOCASE_ARRAY(IDL_TYP_UINT, uint);
					DOCASE_ARRAY(IDL_TYP_ULONG, ulong);
					DOCASE_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
					DOCASE_TO_CMP_ARRAY(IDL_TYP_BYTE, char);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_INT, short);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_LONG, long);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_FLOAT, long);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, IDL_COMPLEX);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, IDL_DCOMPLEX);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_UINT, uint);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_ULONG, ulong);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
					DOCASE_TO_CMP_ARRAY(IDL_TYP_BYTE, char);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_INT, short);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_LONG, long);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_FLOAT, long);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_DOUBLE, double);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(IDL_TYP_COMPLEX, IDL_COMPLEX);
					DOCASE_TO_CMP_ARRAY_FROM_CMP(IDL_TYP_DCOMPLEX, IDL_DCOMPLEX);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_UINT, uint);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_ULONG, ulong);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_LONG64, IDL_LONG64);
					DOCASE_TO_CMP_ARRAY(IDL_TYP_ULONG64, IDL_ULONG64);
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
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
				default: IDL_Message(IDL_M_NAMED_GENERIC, IDL_MSG_LONGJMP, "unexpected type");
			}
		}
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

IDL_VPTR IDL_CvtString(int argc, IDL_VPTR argv[], char *argk=NULL){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
		if (argv[0]->type == IDL_TYP_STRING) return argv[0];
		BaseGDL* temp=(VPTR_ToGDL(argv[0]))->Convert2(GDL_STRING, BaseGDL::CONVERT);
		Guard <BaseGDL> g(temp);
		return GDL_ToVPTR(temp);
	}

char* GDLWriteVarAtAddr(BaseGDL* var, std::string name, UCHAR type, size_t address, bool isoutput, bool isarray){
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
					export_dealloc.push_back((char*)allstringdescr);
					void* stringdescPtrs=malloc(nEl*sizeof(IDL_STRING*));
					export_dealloc.push_back((char*)stringdescPtrs);
					IDL_STRING** p=(IDL_STRING**)stringdescPtrs;
					for (SizeT i=0; i< nEl; ++i) p[i]=(IDL_STRING*)((SizeT)allstringdescr+(i*sizeof(IDL_STRING)));
					for (auto i=0; i< nEl; ++i) {
						p[i]->slen = ((*res)[i]).size();
						p[i]->s = (char*) malloc(p[i]->slen + 1);
						export_dealloc.push_back(p[i]->s);
						strncpy(p[i]->s, (*res)[i].c_str(), p[i]->slen + 1);
					}
					return (char*)p;
				//write an array of string descriptors at address; return vector of addresses to be destroyed when IDL_KWFree will be called
				} else {
					IDL_STRING *s=(IDL_STRING*) (address); //string descr is at address
					s->slen = res->NBytes();
					s->s = (char*) malloc(s->slen + 1);
					export_dealloc.push_back(s->s);
					strncpy(s->s, (*res)[0].c_str(), s->slen + 1);
					return s->s;
				}
				break;
			}
			case IDL_TYP_UNDEF:
			{
				if (!isoutput) throw GDLException("GDLWriteVarAtAddr: variable " + name + " is not writeable.");
				break;
			}
//			case IDL_TYP_PTR:
//				throw GDLException("Unable to convert variable to type pointer.");
//				break;
//			case IDL_TYP_OBJREF:
//				throw GDLException("Unable to convert variable to type object.");
//				break;
			default: throw GDLException("GDLWriteVarAtAddr: unsupported case.");
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
			if (isoutput && passed.readonly) throw GDLException("Keyword " + std::string(requested.keyword) + " must be a named variable.");
			BaseGDL* var = passed.varptr;
			//if requested var is NULL here, it is an undefined var, which MAY be returned as good value.
			if (var == NULL) throw GDLException("GDLExportKeyword: variable " + std::string(requested.keyword) + " is not defined.");
			if (iszero) { //zero before write.
				GDLZeroAtAddr(global_address);
			}
			if (var != NULL) {
				if (!isarray && (var->N_Elements() > 1)) throw GDLException("Expression must be a scalar or 1 element array in this context: " + std::string(passed.name) + ".");
				if (inputByReference) {//address (relative) of a IDL_VPTR with input only value
					IDL_VPTR temp = GDL_ToVPTR(var);
					memcpy((void*) (global_address), (void*) (&temp), sizeof (IDL_VPTR)); //pass by address of a IDL_VAR
				} else if (isoutput) { //address (relative) of a IDL_VPTR where input/output value is written (if existing) and will be returned 
					toBeReturned = GDL_ToVPTR(var);
					memcpy((void*) (global_address), (void*) (&toBeReturned), sizeof (IDL_VPTR)); //pass address of a IDL_VAR that will contain the result.
				} else if (isarray) {
					size_t array_desc_address = (size_t) (requested.value);
					IDL_KW_ARR_DESC_R* arr_desc = (IDL_KW_ARR_DESC_R*) (array_desc_address);
					//check limits
					if ((var->N_Elements() > (*arr_desc).nmax) || (var->N_Elements() < (*arr_desc).nmin))
						throw GDLException(
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
						if (requested.type != IDL_TYP_LONG) throw GDLException("Invalid use of IDL_KW_VALUE on non-integer keyword.");
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

		bool iszero = ((requested.flags & IDL_KW_ZERO) == IDL_KW_ZERO); //zero field if requested
		bool inputByReference = ((requested.flags & IDL_KW_VIN) == IDL_KW_VIN); // input, but passed by reference
		bool isoutput = (!inputByReference && (requested.flags & IDL_KW_OUT) == IDL_KW_OUT);
		bool isarray = (!inputByReference && (requested.flags & IDL_KW_ARRAY) == IDL_KW_ARRAY); //var must be an array, field is a IDL_KW_ARR_DESC_R*
		bool byMask = ((requested.flags & IDL_KW_VALUE) == IDL_KW_VALUE);
		if (requested.specified != NULL) { 
			memcpy((void*) (address), &ok, sizeof (int)); //requested is in offset
		}
		if (requested.value != NULL) { // need to pass either an address of a IDL_VPTR or fill in static elements of the structure exchanged with routine
			if (isoutput && passed.readonly) throw GDLException("Keyword " + std::string(requested.keyword) + " must be a named variable.");
			BaseGDL* var = passed.varptr;
			//if requested var is NULL here, it is an undefined var, which MAY be returned as good value.
			if (var == NULL) throw GDLException("GDLExportKeyword: variable " + std::string(requested.keyword) + " is not defined.");
			if (iszero) GDLZeroAtAddr((size_t)address);
			if (var != NULL) {
				if (!isarray && (var->N_Elements() > 1)) throw GDLException("Expression must be a scalar or 1 element array in this context: " + std::string(passed.name) + ".");
				if (inputByReference) {//address (relative) of a IDL_VPTR with input only value
					IDL_VPTR temp = GDL_ToVPTR(var);
					memcpy((void*) (address), (void*) (&temp), sizeof (IDL_VPTR)); //pass by address of a IDL_VAR
				} else if (isoutput) { //address (relative) of a IDL_VPTR where input/output value is written (if existing) and will be returned 
					toBeReturned = GDL_ToVPTR(var);
					memcpy((void*) (address), (void*) (&toBeReturned), sizeof (IDL_VPTR)); //pass address of a IDL_VAR that will contain the result.
				} else if (isarray) {
					size_t array_desc_address = (size_t) (requested.value);
					IDL_KW_ARR_DESC* arr_desc = (IDL_KW_ARR_DESC*) (array_desc_address);
					//check limits
					if ((var->N_Elements() > (*arr_desc).nmax) || (var->N_Elements() < (*arr_desc).nmin))
						throw GDLException(
							"Keyword array parameter " + std::string(requested.keyword) + " must have from " + i2s(arr_desc->nmin) + " to " + i2s(arr_desc->nmax) + " elements.");
					//and these are offsets!
					IDL_MEMINT passedArraySize = var->N_Elements();
					memcpy((void*) address, (void*) &passedArraySize, sizeof (IDL_MEMINT)); //number of passed elements
					size_t data_address = (size_t) ((*arr_desc).data); //address where to pass elements
					GDLWriteVarAtAddr(var, std::string(requested.keyword), requested.type, data_address, isoutput, true);
				} else {
					//here IDL_KW_VALUE may appear
					if (byMask) {
						if (requested.type != IDL_TYP_LONG) throw GDLException("Invalid use of IDL_KW_VALUE on non-integer keyword.");
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
				     IDL_VPTR *plain_args, int mask) {
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
		std::map<const char*, int> requested;
		std::map<const char*, int>::iterator it;
		while ((kw = kw_requested[ikw].keyword) != NULL) {
			int code = -1;
			if (kw_requested[ikw].mask & mask == 0) code = -2;
			requested.insert(std::pair<const char*, int>(kw, code));
			ikw++;
		}
		for (auto ipassed = 0; ipassed < npassed; ++ipassed) {
			const char* s = argk[ipassed].name;
			int l = strlen(s);
			bool found = false;
			bool ignored = false;
			for (it = requested.begin(); it != requested.end(); ++it) {
				const char* expected_kw = it->first;
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
					const char* expected_kw = it->first;
					if (strncmp(expected_kw, s, l) == 0) DInterpreter::CallStackBack()->Throw("Ambiguous keyword abbreviation: " + std::string(s));
				}
			}
		}
		//populate all passed addresses
		for (it = requested.begin(); it != requested.end(); ++it) {
			int ipassed = it->second;
			if (ipassed == -1) GdlExportAbsentKeywordInOldApi(kw_requested[irequested], kw_requested[irequested].value);
			else if (ipassed >= 0) {
				IDL_VPTR ret = GdlExportPresentKeywordInOldApi(kw_requested[irequested], argk[ipassed], kw_requested[irequested].value);
				if (ret != NULL) {
					argk[ipassed].out = ret; //pass vptr back
				}
			}
			irequested++;
		}
		return 0;
}
	int IDL_CDECL IDL_KWProcessByOffset(int argc, IDL_VPTR *argv, char
			*argk_passed, IDL_KW_PAR *kw_requested, IDL_VPTR *plain_args, int mask, void *kw_result) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
			static const int NoClean=0;
					// this is always true with GDL:
		for (auto i=0; i< argc; ++i) plain_args[i]=argv[i];
		//argk is a pointer to a  GDL_PASS_KEYWORDS_LIST struct
		GDL_PASS_KEYWORDS_LIST* container = (GDL_PASS_KEYWORDS_LIST*) argk_passed;
		int npassed=0;
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
		std::map<const char*, int> requested;
		std::map<const char*, int>::iterator it;
		while ((kw = kw_requested[ikw].keyword) != NULL) {
			int code = -1;
			if (kw_requested[ikw].mask & mask == 0) code = -2;
			requested.insert(std::pair<const char*, int>(kw, code));
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
				const char* expected_kw = it->first;
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
					const char* expected_kw = it->first;
					if (strncmp(expected_kw, s, l) == 0) DInterpreter::CallStackBack()->Throw("Ambiguous keyword abbreviation: " + std::string(s));
				}
			}
		}
//		std::cerr << std::endl;
		//populate kw_result
		//first set IDL_KW_RESULT_FIRST_FIELD to zero (no need to clean)
		memcpy(kw_result,&NoClean,sizeof(int));
		for (it = requested.begin(); it != requested.end(); ++it) {
			int ipassed = it->second;
//			std::cerr << "\"" << it->first << "\"," << ipassed<< std::endl;
			if (ipassed == -1) GdlExportAbsentKeyword(kw_requested[irequested], kw_result);
			else if (ipassed >= 0) {
				IDL_VPTR ret=GdlExportPresentKeyword(kw_requested[irequested], argk[ipassed], kw_result);
				if (ret != NULL) {
					argk[ipassed].out=ret; //pass vptr back
				}
			}
			irequested++;
		}
		return argc;
	}

	// not so simple
//	int IDL_CDECL IDL_KWProcessByAddr(int argc, IDL_VPTR *argv, char
//			*argk_passed, IDL_KW_PAR *kw_requested, IDL_VPTR *plain_args, int mask, void *kw_result) {
//	}
void *IDL_CDECL IDL_MemAlloc(IDL_MEMINT n, const char *err_str, int
			msg_action){return malloc(n);}
void *IDL_CDECL IDL_MemRealloc(void *ptr, IDL_MEMINT n, const char
 *err_str, int action){return realloc(ptr,n);}
void IDL_CDECL IDL_MemFree(IDL_REGISTER void *m, const char
 *err_str, int msg_action){free(m);}
void *IDL_CDECL IDL_MemAllocPerm(IDL_MEMINT n, const char *err_str,
        int action){throw GDLException("MemAllocPerm is not currently supported.");}
char *IDL_CDECL IDL_GetScratch(IDL_REGISTER IDL_VPTR *p,
        IDL_REGISTER IDL_MEMINT n_elts,  IDL_REGISTER IDL_MEMINT elt_size){ char* allocated=(char*)malloc (n_elts*elt_size);
export_dealloc.push_back(allocated);
return allocated;}
void IDL_CDECL IDL_KWCleanup(int fcn){IDL_KWFree();}
char *IDL_CDECL IDL_GetScratchOnThreshold(IDL_REGISTER char
        *auto_buf, IDL_REGISTER IDL_MEMINT auto_elts,  IDL_REGISTER IDL_MEMINT
        n_elts,  IDL_REGISTER IDL_MEMINT elt_size,  IDL_VPTR *tempvar){
	throw GDLException("IDL_GetScratchOnThreshold is not implemented, FIXME.");
}
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
			while (s=va_arg(args,char*)) {
				if (s != NULL) {
					finalMessage += std::string(s);
				} else va_end(args);
			}
			va_end(args);
		} else throw GDLException("Invalid Error Code given to IDL_Message() by user-written routine.");
		if (action == IDL_MSG_LONGJMP || IDL_MSG_RET) throw GDLException(finalMessage);
		if (action == IDL_MSG_IO_LONGJMP) throw GDLIOException(finalMessage);
		if (action == IDL_MSG_EXIT) {
			Warning(finalMessage);
			throw GDLException("IDL_MSG_EXIT forbidden for user-written routines.");
		}
		if (action == IDL_MSG_INFO) Warning(finalMessage);
	}
	
void IDL_CDECL IDL_MessageFromBlock(IDL_MSG_BLOCK block, int code, int action,...){if (action!=IDL_MSG_INFO) throw GDLException("exception caused by non-GDL (dlm) function call.");}//do nothing.
void IDL_CDECL IDL_MessageSyscode(int code, IDL_MSG_SYSCODE_T syscode_type, int syscode, int action, ...){if (action!=IDL_MSG_INFO) throw GDLException("exception caused by non-GDL (dlm) function call.");}//do nothing.
void IDL_CDECL IDL_MessageSyscodeFromBlock(IDL_MSG_BLOCK block, int code, IDL_MSG_SYSCODE_T syscode_type,  int syscode, int action, ...){if (action!=IDL_MSG_INFO) throw GDLException("exception caused by non-GDL (dlm) function call.");}
void IDL_CDECL IDL_MessageVarError(int code, IDL_VPTR var, int action){if (action!=IDL_MSG_INFO) throw GDLException("exception caused by non-GDL (dlm) function call.");}
void IDL_CDECL IDL_MessageVarErrorFromBlock(IDL_MSG_BLOCK block, int code, IDL_VPTR var, int action){if (action!=IDL_MSG_INFO) throw GDLException("exception caused by non-GDL (dlm) function call.");}
void IDL_CDECL IDL_MessageResetSysvErrorState(void) {
		try {
			std::string command = ("message,/reset");
			DInterpreter::CallStackBack()->Interpreter()->ExecuteStringLine(command);
		} catch (...) {
		}
}
void IDL_CDECL IDL_MessageSJE(void *value){} //do nothing (?)
void *IDL_CDECL IDL_MessageGJE(void){return NULL;}//do nothing (?)
void IDL_CDECL IDL_MessageVE_UNDEFVAR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO) throw GDLException("Variable is Undefined.");}
void IDL_CDECL IDL_MessageVE_NOTARRAY(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Expression must be an array in this context.");}
void IDL_CDECL IDL_MessageVE_NOTSCALAR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Expression must be a scalar in this context.");}
void IDL_CDECL IDL_MessageVE_NOEXPR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Expression must be a named variable in this context");}
void IDL_CDECL IDL_MessageVE_NOCONST(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Constant not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOFILE(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("File expression not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOCOMPLEX(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Variable is not of complex type/");}
void IDL_CDECL IDL_MessageVE_NOSTRING(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("String expression not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOSTRUCT(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Struct expression not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_REQSTR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("String expression required in this context.");}
void IDL_CDECL IDL_MessageVE_NOSCALAR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Scalar variable not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NULLSTR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Null string not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOPTR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Pointer expression not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOOBJREF(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Object reference expression not allowed in this context.");}
void IDL_CDECL IDL_MessageVE_NOMEMINT64(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("This routine is 32-bit limited and cannot handle this many elements.");}
void IDL_CDECL IDL_MessageVE_STRUC_REQ(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Expression must be a structure in this context.");}
void IDL_CDECL IDL_MessageVE_REQPTR(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Pointer type required in this context.");}
void IDL_CDECL IDL_MessageVE_REQOBJREF(IDL_VPTR var, int action){if (action!=IDL_MSG_INFO)throw GDLException("Object reference required in this context");}
void IDL_CDECL IDL_Message_BADARRDNUM(int action){if (action!=IDL_MSG_INFO)throw GDLException("Arrays are allowed 1 - 8 dimensions");}
void IDL_CDECL IDL_Deltmp(IDL_REGISTER IDL_VPTR p){TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) free(p);}
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
	if (argc != 1) throw GDLException("FIXME IDL_BasicTypeConversion!");
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
		default: throw GDLException("Wrong type in IDL_BasicTypeConversion!");
	}
	return NULL;
}
int IDL_AddSystemRoutine(IDL_SYSFUN_DEF *defs, int is_function, int cnt){return 1;} //using DLM insure this is OK. I think.
int IDL_CDECL IDL_BailOut(int stop){return sigControlC;} //use of stop not supported.
void IDL_CDECL IDL_ExitRegister(IDL_EXIT_HANDLER_FUNC proc){}
void IDL_CDECL IDL_ExitUnregister(IDL_EXIT_HANDLER_FUNC proc){}

IDL_STRUCTURE* IDL_CDECL IDL_MakeStruct(char *name,  IDL_STRUCT_TAG_DEF *tags){
	IDL_STRUCTURE *newIdlStructure= new IDL_STRUCTURE();
	IDL_SREF *str=new IDL_SREF();
	IDL_ARRAY *arr=new IDL_ARRAY();
	str->arr=arr;
	return newIdlStructure;
}
}
#endif
