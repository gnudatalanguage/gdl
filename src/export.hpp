
#include "gdl_export.h"
#include "datatypes.hpp"
#include "GDLInterpreter.hpp"

IDL_VPTR GDL_ToVPTR(BaseGDL* var);

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

  IDL_VPTR GDL_ToVPTR(BaseGDL* var) {
    IDL_VARIABLE *v = new IDL_VARIABLE();
    if (var == NULL) {
      v->type=IDL_TYP_UNDEF;
      return v;
    }
    if (var == NullGDL::GetSingleInstance()) {
      v->type=IDL_TYP_LONG; // any type, as the NULL flags is checked.
      v->flags=IDL_V_NULL;
      return v;
    }
    if (var->N_Elements() == 1) {
      v->flags = IDL_V_CONST;
      v->flags2 = 0;
      switch (var->Type()) {
        case GDL_UNDEF:
          v->type = IDL_TYP_UNDEF;
          v->flags = IDL_V_NULL;
          v->value.dcmp.r = 0;
          v->value.dcmp.i = 0;
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
          v->flags = IDL_V_DYNAMIC;
          DString s = (*static_cast<DStringGDL*> (var))[0];
          IDL_STRING idlstr = {(int) s.size(), 0, (char*) s.c_str()};
          v->value.str = idlstr;
          break;
        }
        default: std::cerr << "GDL_ToVPTR: bad case of "<<var->TypeStr()<<std::endl;
      }
    } else {
      v->flags = IDL_V_ARR;
      v->flags2 = 0;
      IDL_ARRAY arraydescr;
      arraydescr.arr_len = var->NBytes();
      arraydescr.data = (UCHAR*) (var->DataAddr());
      for (int i = 0; i < var->Rank(); ++i) arraydescr.dim[i] = var->Dim(i);
      arraydescr.n_dim = var->Rank();
      arraydescr.n_elts = var->N_Elements();
      arraydescr.offset = 0;
      v->value.arr = &arraydescr;
      switch (var->Type()) {
        case GDL_UNDEF:
          v->type = IDL_TYP_UNDEF;
          arraydescr.elt_len = 0;
          break;
        case GDL_BYTE:
          v->type = IDL_TYP_BYTE;
          arraydescr.elt_len = 1;
          break;
        case GDL_INT:
          v->type = IDL_TYP_INT;
          arraydescr.elt_len = 2;
          break;

        case GDL_LONG:
          v->type = IDL_TYP_LONG;
          arraydescr.elt_len = 4;
          break;

        case GDL_LONG64:
          v->type = IDL_TYP_LONG64;
          arraydescr.elt_len = 8;
          break;

        case GDL_UINT:
          v->type = IDL_TYP_UINT;
          arraydescr.elt_len = 2;
          break;

        case GDL_ULONG:
          v->type = IDL_TYP_ULONG;
          arraydescr.elt_len = 4;
          break;

        case GDL_ULONG64:
          v->type = IDL_TYP_ULONG64;
          arraydescr.elt_len = 8;
          break;

        case GDL_FLOAT:
          v->type = IDL_TYP_FLOAT;
          arraydescr.elt_len = 4;
          break;
        case GDL_DOUBLE:
          v->type = IDL_TYP_DOUBLE;
          arraydescr.elt_len = 8;
          break;
        case GDL_COMPLEX:
        {
          v->type = IDL_TYP_COMPLEX;
          arraydescr.elt_len = 8;
          break;
        }
        case GDL_COMPLEXDBL:
        {
          v->type = IDL_TYP_DCOMPLEX;
          arraydescr.elt_len = 16;
          break;
        }
        case GDL_STRING:
        {
          v->type = IDL_TYP_STRING;
          arraydescr.elt_len = 1;
          break;
        }
        default: std::cerr << "GDL_ToVPTR: bad case of "<<var->TypeStr()<<std::endl;
      }
    }
    return v;
  }  
  
  BaseGDL* VPTR_ToGDL(IDL_VPTR v) {
    if (v == NULL) {
      throw GDLException("Invalid IDL_VPTR used.");
    }
    if ( (v->flags & IDL_V_NULL) == IDL_V_NULL) {
      return NullGDL::GetSingleInstance();
    }
    if ((v->flags && IDL_V_ARR) == IDL_V_ARR) {
      throw GDLException("Array not supported.");
    } else {
      switch (v->type) {
      case IDL_TYP_BYTE:
        return new DByteGDL(v->value.c);
        break;
      case IDL_TYP_INT:
        return new DIntGDL(v->value.i);
        break;
      case IDL_TYP_LONG:
        return new DLongGDL(v->value.l);
        break;
      case IDL_TYP_FLOAT:
        return new DFloatGDL(v->value.f);
        break;
      case IDL_TYP_DOUBLE:
        return new DDoubleGDL(v->value.d);
        break;
      case IDL_TYP_COMPLEX:
        return new DComplexGDL(std::complex<float>(v->value.cmp.r,v->value.cmp.i));
        break;
      case IDL_TYP_DCOMPLEX:
        return new DComplexDblGDL(std::complex<double>(v->value.dcmp.r,v->value.dcmp.i));
        break;
      case IDL_TYP_PTR:
        return new DPtrGDL(v->value.ptrint);
        break;
//      case IDL_TYP_OBJREF:
//        return new DObjGDL(v->value.???);
//        break;
      case IDL_TYP_UINT:
        return new DUIntGDL(v->value.ui);
        break;
      case IDL_TYP_ULONG:
        return new DULongGDL(v->value.ul);
        break;
      case IDL_TYP_ULONG64:
        return new DByteGDL(v->value.ul64);
        break;
       case IDL_TYP_STRING:
        return new DStringGDL(v->value.str.s);
        break;
      default: std::cerr << "ReturnIDL_VPTR_AsGDL: bad case " << v->type << "\n";
    }
    }
    throw;
  }
  
	void GDL_Print(std::string &printPro, int nPar, IDL_VPTR *argv, DLong unit=0, bool useUnit=false) {
		SizeT printIx = LibProIx(printPro);
      // 	e->PushNewEnv( libProList[ proIx], 1);
//       make the call
      EnvT* env = new EnvT(NULL, libProList[printIx]);
//      EnvT* newEnv = DInterpreter::CallStackBack()->NewEnv(libProList[proIx], 1);
      Guard<EnvT> guard(env);
   	// add parameters
       if (useUnit) env->SetNextPar( new DLongGDL(unit));
		for (auto i=0; i< nPar; ++i) {
          env->SetNextPar(VPTR_ToGDL(argv[i]));
		}
	  static_cast<DLibPro*>(env->GetPro())->Pro()( env);
	}

  // for use inside source of LINKIMAGE and other DLMs
extern "C" {
	//all the IDL_** modules below should be listed in dynlist.txt ---now done using an asterisk syntax 
void IDL_KWFree(void){}; //do nothing... yet

void IDL_CDECL IDL_Print(int argc, IDL_VPTR *argv, char *argk){
		//argk is to be set to NULL by users according to the doc.
		std::string s("PRINT");
		GDL_Print(s, argc, argv);
	};
	void IDL_CDECL IDL_PrintF(int argc, IDL_VPTR *argv, char *argk){
		std::string s("PRINTF");
		GDL_Print(s, argc-1, &(argv[1]),argv[0]->value.l, true);
	}

	
	void IDL_CDECL IDL_StrStore(IDL_STRING *s, const char *fs){
	s->slen=strlen(fs);
	s->stype=0;
	strncpy(s->s,fs,s->slen);
}
// str below is a supposed to be a copy of the string descriptor(s).
// to properluy duplicate, oen has to ceate copies of the string(s) and update the descripto(s)
void IDL_CDECL IDL_StrDup(IDL_REGISTER IDL_STRING *str,
        IDL_REGISTER IDL_MEMINT n){
	for (auto i=0; i< n; ++i) {
		if (str[i].slen > 0) {
			char* news=(char*)malloc(str[i].slen+1);
			strncpy(news,str[i].s,str[i].slen+1);
		}
	}
}

void IDL_CDECL IDL_StrDelete(IDL_STRING *str, IDL_MEMINT n)
{
	for (auto i=0; i< n; ++i) if (str[i].slen > 0) free(str[i].s);
}

void IDL_CDECL IDL_StrEnsureLength(IDL_STRING *s, int n){
  if (s->slen < n) {
  IDL_StrDelete(s, 1);
  s->s=(char*)calloc(n,1);
}
	}

	void IDL_CDECL IDL_StoreScalar(IDL_VPTR dest, int type,	IDL_ALLTYPES * value) {
		dest->type=type;
		switch (type) {
			case IDL_TYP_BYTE:
			{
				dest->value.c=value->c;
				break;
			}
			case IDL_TYP_INT:
			{
				dest->value.i=value->i;
				break;
			}
			case IDL_TYP_LONG:
			{
				dest->value.i=value->i;
				break;
			}
			case IDL_TYP_FLOAT:
			{
				dest->value.f=value->f;
				break;
			}
			case IDL_TYP_DOUBLE:
			{
				dest->value.d=value->d;
				break;
			}
			case IDL_TYP_COMPLEX:
			{
				dest->value.cmp=value->cmp;
				break;
			}
			case IDL_TYP_DCOMPLEX:
			{
				dest->value.dcmp=value->dcmp;
				break;
			}
			case IDL_TYP_PTR:
			{
				dest->value.ptrint=value->ptrint;
				break;
			}
		    case IDL_TYP_OBJREF:
			{
				dest->value.ptrint=value->ptrint;
				break;
		    }
			case IDL_TYP_UINT:
			{
				dest->value.ui=value->ui;
				break;
			}
			case IDL_TYP_ULONG:
			{
				dest->value.ul=value->ul;
				break;
			}
			case IDL_TYP_ULONG64:
			{
				dest->value.ul64=value->ul64;
				break;
			}
			case IDL_TYP_STRING:
			{
				dest->value.str=value->str;
				break;
			}
			case IDL_TYP_STRUCT:
			{
				dest->value.s=value->s;
				break;
			}
			case IDL_TYP_UNDEF:
			default: throw GDLException("IDL_StoreScalar: unsupported case.");
		}
	}

IDL_VPTR IDL_CDECL IDL_StrToSTRING(const char *s) {
		IDL_STRING idlstr;
		idlstr.slen = strlen(s);
		idlstr.stype = 0;
		idlstr.s = (char*) malloc(idlstr.slen + 1);
		strncpy(idlstr.s, s, idlstr.slen+1);
		IDL_VARIABLE *ss = (IDL_VARIABLE *) malloc(sizeof (IDL_VARIABLE));
		ss->type = IDL_TYP_STRING;
		ss->flags = IDL_V_DYNAMIC;
		ss->flags2 = 0;
		ss->value.str = idlstr;
		return ss;
	}

	double IDL_CDECL IDL_DoubleScalar(IDL_REGISTER IDL_VPTR v) {
		switch (v->type) {
			case 1: return (double) (v->value.c);
				break;
			case 2: return (double) (v->value.i);
				break;
			case 3: return (double) (v->value.l);
				break;
			case 4: return (double) (v->value.f);
				break;
			case 5: return v->value.d;
				break;
			case IDL_TYP_UINT: return (double) (v->value.ui);
				break;
			case IDL_TYP_ULONG: return (double) (v->value.ul);
				break;
			case IDL_TYP_LONG64: return (double) (v->value.l64);
				break;
			case IDL_TYP_ULONG64: return (double) (v->value.ul64);
				break;
			default:
				return 0;
				break;
		}
	}

	void GDLWriteVarAtAddr(BaseGDL* var, std::string name, UCHAR type, size_t address, bool isoutput){
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
			case IDL_TYP_PTR:
			{
				DPtrGDL* res = static_cast<DPtrGDL*> (var->Convert2(GDL_PTR));
				memcpy((void*) (address), res->DataAddr(), res->N_Elements() * sizeof (IDL_PTRINT));
				break;
			}
				//      case IDL_TYP_OBJREF:
				//        break; }
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
				IDL_STRING s;
				s.slen = res->NBytes();
				s.s = (char*) malloc(s.slen + 1);
				strncpy(s.s, (*res)[0].c_str(), s.slen + 1);
				memcpy((void*) (address), &s, res->N_Elements() * sizeof (IDL_STRING));
				break;
			}
			case IDL_TYP_UNDEF:
			{
				if (!isoutput) throw GDLException("GDLExportKeyword: variable " + name + " is not writeable.");
				break;
			}
			default: throw GDLException("GDLExportKeyword: unsupported case.");
		}
	}
	
	void GDLZeroAtAddr(size_t address){
				memset((void*) (address), 0, sizeof(NULL));
			}

	IDL_VPTR GdlExportPresentKeyword(IDL_KW_PAR requested, GDL_KEYWORDS_LIST passed, void* kw_result) {
		static const int ok = 1;
		static const int nok = 0;
	    IDL_VPTR toBeReturned=NULL;
		
		bool iszero = ((requested.flags & IDL_KW_ZERO) == IDL_KW_ZERO); //zero field if requested
		bool isarray = (( requested.flags & IDL_KW_ARRAY) == IDL_KW_ARRAY); //var will be an array. Array has an absolute address
		// Indicates parameter is an OUTPUT parameter passed by reference.
		// Expressions are excluded.  The address of the IDL_VARIABLE is stored in
		// the value field.  Otherwise, no checking is performed.  Special hint:
		// to find out if a IDL_KW_OUT parameter is specified, use 0 for the type,
		// and IDL_KW_OUT | IDL_KW_ZERO for the flags.  The value field will either
		// contain NULL or the pointer to the variable. */
		bool isoutput = ((requested.flags & IDL_KW_OUT) == IDL_KW_OUT);
		bool byAddress = ((requested.flags & IDL_KW_VIN) == IDL_KW_VIN);// IDL_KW_VIN types (IDL_KW_OUT | IDL_KW_ARRAY) have an absolute address.
		bool byMask = ((requested.flags & IDL_KW_VALUE) == IDL_KW_VALUE);
		// tag 'out' those that will get a return value
		if (requested.specified != NULL) { // need write 0 or 1 in a special int in KW structure
//			std::cerr << requested.keyword << ", specified: at 0x " << (size_t) (requested.specified) << std::dec << std::endl;
			size_t address = (size_t) (kw_result)+(size_t) (requested.specified);
			memcpy((void*) (address), &ok , sizeof (int)); //requested is in offset
		}
		if (requested.value != NULL) { // need to pass either an address of a IDL_VPTR or fill in static elements of the structure exchanged with routine
//			std::cerr << requested.keyword << ", value to report at 0x" << std::hex<< (size_t) (requested.value) << std::dec <<std::endl;
			if (isoutput && passed.readonly) throw GDLException("Keyword " + std::string(requested.keyword)+ " must be a named variable.");
			BaseGDL* var = passed.varptr;
			//if requested var is NULL here, it is an undefined var, which MAY be returned as good value.
			if (var == NULL) throw GDLException("GDLExportKeyword: variable " + std::string(requested.keyword) + " is not defined.");
			if (iszero) { //zero before write.
				size_t address;
				if (byAddress || isarray) {
					address = (size_t) (requested.value);
				} else {
					address = (size_t) (kw_result)+(size_t) (requested.value);
				}
				GDLZeroAtAddr(address);
			}
			if (var != NULL) {
				if (byAddress) { //can be isoutput too?
					toBeReturned = GDL_ToVPTR(var);
					size_t address = (size_t) (requested.value);
					memcpy((void*) (address), (void*) (toBeReturned), sizeof (IDL_VPTR)); //pass by address of a IDL_VAR
				} else if (isoutput) { //address (relative) of a IDL_VPTR where input/output value is written (if existing) and will be returned 
					toBeReturned = GDL_ToVPTR(var);
					size_t address = (size_t) (kw_result)+(size_t) (requested.value);
					memcpy((void*) (address), (void*) (&toBeReturned), sizeof (IDL_VPTR)); //pass address of a IDL_VAR that will contain the result.
				} else if (isarray) {
					size_t address = (size_t) (requested.value);
					IDL_KW_ARR_DESC_R* arr_desc = (IDL_KW_ARR_DESC_R*) (address);
					//check limits
					if ((var->N_Elements() > (*arr_desc).nmax) || (var->N_Elements() < (*arr_desc).nmin))
						throw GDLException(
							"Keyword array parameter " + std::string(requested.keyword) + " must have from " + i2s(arr_desc->nmin) + " to " + i2s(arr_desc->nmax) + " elements.");
					//and these are offsets!
					address = (size_t) (kw_result)+(size_t) ((*arr_desc).n_offset);
					IDL_MEMINT passedArraySize = var->N_Elements();
					memcpy((void*) address, (void*) &passedArraySize, sizeof (IDL_MEMINT)); //number of passed elements
					address = (size_t) (kw_result)+(size_t) ((*arr_desc).data); //address where to pass elements
					GDLWriteVarAtAddr(var, std::string(requested.keyword), requested.type, address, isoutput);
				} else {
					size_t address = (size_t) (kw_result)+(size_t) (requested.value);
					//here IDL_KW_VALUE may appear
					if (byMask) {
						if (requested.type != IDL_TYP_LONG) throw GDLException("Invalid use of IDL_KW_VALUE on non-integer keyword.");
						long mask = IDL_KW_VALUE_MASK & requested.flags;
						long *val = (long*) address;
						*val |= mask;
					} else {
						GDLWriteVarAtAddr(var, std::string(requested.keyword), requested.type, address, isoutput);
					}
				}
			}
		}
		return toBeReturned;
	}

	void GdlExportAbsentKeyword(IDL_KW_PAR requested, void* kw_result) {
		static const int ok = 1;
		static const int nok = 0;
		
		bool iszero = ((requested.flags & IDL_KW_ZERO) == IDL_KW_ZERO); //zero field if requested
		bool isarray = (( requested.flags & IDL_KW_ARRAY) == IDL_KW_ARRAY); //var will be an arry
		bool isoutput = ((requested.flags & IDL_KW_OUT) == IDL_KW_OUT); // output, hence passed by address
		bool byAddress = ((requested.flags & IDL_KW_VIN) == IDL_KW_VIN);// input, but passed by address
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
				if (byAddress || isarray) {
					address = (size_t) (requested.value);
				} else {
					address = (size_t) (kw_result)+(size_t) (requested.value);
				}
				GDLZeroAtAddr(address);
			}
		}
	}

	int IDL_CDECL IDL_KWProcessByOffset(int argc, IDL_VPTR *argv, char
			*argk_passed, IDL_KW_PAR *kw_requested, IDL_VPTR *plain_args, int mask, void *kw_result) {
		// this is always true with GDL:	
		plain_args = argv;
		bool someArgsPassed = true;
		if (argk_passed == NULL) { //no keywords passed, however need to process some of IDL_KW_PAR requests.
			someArgsPassed = false;
		}
		//argk is a pointer to a  GDL_PASS_KEYWORDS_LIST struct
		GDL_PASS_KEYWORDS_LIST* container = (GDL_PASS_KEYWORDS_LIST*) argk_passed;
		int npassed = container->npassed;
		GDL_KEYWORDS_LIST* argk = container->passed;
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
		if (someArgsPassed) {
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
		}
//		std::cerr << std::endl;
		//populate kw_result
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
	}

