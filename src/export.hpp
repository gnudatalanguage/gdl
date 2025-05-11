
#include "gdl_export.h"
#include "datatypes.hpp"

IDL_VPTR GDL_ToVPTR(BaseGDL* var);

typedef struct {
  const char* name;
  BaseGDL* varptr; // pointer to some externally produced var if out=true
  bool out=false;
  UCHAR type;
} GDL_KEYWORDS_LIST;

typedef struct {
  int npassed;
  GDL_KEYWORDS_LIST* passed;
} GDL_PASS_KEYWORDS_LIST;

// for use inside source of LINKIMAGE and other DLMs
extern "C" {
	//all the IDL_** modules below should be listed in dynlist.txt ---now done using an asterisk syntax 
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

	void GDlWriteVarAtAddr(BaseGDL* var, std::string name, UCHAR type, size_t address, bool isoutput){
		switch (type) {
			case IDL_TYP_BYTE:
			{
				DByteGDL* res = static_cast<DByteGDL*> (var->Convert2(GDL_BYTE));
				memcpy((void*) (address), res->DataAddr(), sizeof (UCHAR));
				break;
			}
			case IDL_TYP_INT:
			{
				DIntGDL* res = static_cast<DIntGDL*> (var->Convert2(GDL_INT));
				memcpy((void*) (address), res->DataAddr(), sizeof (IDL_INT));
				break;
			}
			case IDL_TYP_LONG:
			{
				DLongGDL* res = static_cast<DLongGDL*> (var->Convert2(GDL_LONG));
				memcpy((void*) (address), res->DataAddr(), sizeof (IDL_LONG));
				break;
			}
			case IDL_TYP_FLOAT:
			{
				DFloatGDL* res = static_cast<DFloatGDL*> (var->Convert2(GDL_FLOAT));
				memcpy((void*) (address), res->DataAddr(), sizeof (float));
				break;
			}
			case IDL_TYP_DOUBLE:
			{
				DDoubleGDL* res = static_cast<DDoubleGDL*> (var->Convert2(GDL_DOUBLE));
				memcpy((void*) (address), res->DataAddr(), sizeof (double));
				break;
			}
			case IDL_TYP_COMPLEX:
			{
				DComplexGDL* res = static_cast<DComplexGDL*> (var->Convert2(GDL_COMPLEX));
				memcpy((void*) (address), res->DataAddr(), sizeof (IDL_COMPLEX));
				break;
			}
			case IDL_TYP_DCOMPLEX:
			{
				DComplexDblGDL* res = static_cast<DComplexDblGDL*> (var->Convert2(GDL_COMPLEXDBL));
				memcpy((void*) (address), res->DataAddr(), sizeof (IDL_DCOMPLEX));
				break;
			}
			case IDL_TYP_PTR:
			{
				DPtrGDL* res = static_cast<DPtrGDL*> (var->Convert2(GDL_PTR));
				memcpy((void*) (address), res->DataAddr(), sizeof (IDL_PTRINT));
				break;
			}
				//      case IDL_TYP_OBJREF:
				//        break; }
			case IDL_TYP_UINT:
			{
				DUIntGDL* res = static_cast<DUIntGDL*> (var->Convert2(GDL_UINT));
				memcpy((void*) (address), res->DataAddr(), sizeof (IDL_UINT));
				break;
			}
			case IDL_TYP_ULONG:
			{
				DULongGDL* res = static_cast<DULongGDL*> (var->Convert2(GDL_ULONG));
				memcpy((void*) (address), res->DataAddr(), sizeof (IDL_ULONG));
				break;
			}
			case IDL_TYP_ULONG64:
			{
				DULong64GDL* res = static_cast<DULong64GDL*> (var->Convert2(GDL_ULONG64));
				memcpy((void*) (address), res->DataAddr(), sizeof (IDL_ULONG64));
				break;
			}
			case IDL_TYP_STRING:
			{
				DStringGDL* res = static_cast<DStringGDL*> (var->Convert2(GDL_STRING));
				IDL_STRING s;
				s.slen = res->NBytes();
				s.s = (char*) malloc(s.slen + 1);
				strncpy(s.s, (*res)[0].c_str(), s.slen + 1);
				memcpy((void*) (address), &s, sizeof (IDL_STRING));
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

	void GdlExportPresentKeyword(IDL_KW_PAR requested, GDL_KEYWORDS_LIST passed, void* kw_result) {
		static const int ok = 1;
		static const int nok = 0;
		
		bool iszero = ((requested.flags & IDL_KW_ZERO) == IDL_KW_ZERO); //zero field if requested
		bool isarray = (( requested.flags & IDL_KW_ARRAY) == IDL_KW_ARRAY); //var will be an arry
		bool isoutput = ((requested.flags & IDL_KW_OUT) == IDL_KW_OUT); // output, hence passed by address
		bool byAddress = ((requested.flags & IDL_KW_VIN) == IDL_KW_VIN);// input, but passed by address
		byAddress = (byAddress || isarray); 
		// tag 'out' those that will get a return value
		if (requested.specified != NULL) { // need write 0 or 1 in a special int in KW structure
			std::cerr << requested.keyword << ", specified: as " << (size_t) (requested.specified) << std::endl;
			size_t address = (size_t) (kw_result)+(size_t) (requested.specified);
			memcpy((void*) (address), &ok , sizeof (int)); //requested is in offset
		}
		if (requested.value != NULL) { // need to pass either an address of a IDL_VPTR or fill in static elements of the structure exchanged with routine
			std::cerr << requested.keyword << ", value to report at " << (size_t) (requested.value) << std::endl;
			// IDL_KW_VIN types (IDL_KW_OUT | IDL_KW_ARRAY) have an absolute address.
			size_t address = byAddress ? (size_t) (requested.value) : (size_t) (kw_result)+(size_t) (requested.value);
			BaseGDL* var = passed.varptr;
			//if requested var is NULL here, it is an undefined var, which MAY be returned as good value.
			if (var == NULL && !isoutput) throw GDLException("GDLExportKeyword: variable " + std::string(requested.keyword) + " is not defined.");
			if (iszero) {
				if (byAddress) {
					memcpy((void*) (address), NULL, sizeof (NULL)); //pass by address of a IDL_VAR
				} else {
					DFloatGDL* zero=new DFloatGDL(0);
					GDlWriteVarAtAddr(zero, std::string(requested.keyword), requested.type, address, isoutput);
				}
			}
			if (var != NULL) {
				if (byAddress) {
					IDL_VPTR temp=GDL_ToVPTR(var);
					memcpy((void*) (address), (void*)(temp), sizeof (IDL_VPTR)); //pass by address of a IDL_VAR
				} else {
					GDlWriteVarAtAddr(var, std::string(requested.keyword), requested.type, address, isoutput);
				}
			}
			passed.out = isoutput;
			if (isoutput) {
				passed.varptr = (BaseGDL*) address; //pass back address of output variable as false ptr to the real output content
				passed.type = requested.type;
			}
		}
	}
	void GdlExportAbsentKeyword(IDL_KW_PAR requested, void* kw_result) {
		static const int ok = 1;
		static const int nok = 0;
		
		bool iszero = ((requested.flags & IDL_KW_ZERO) == IDL_KW_ZERO); //zero field if requested
		bool isarray = (( requested.flags & IDL_KW_ARRAY) == IDL_KW_ARRAY); //var will be an arry
		bool isoutput = ((requested.flags & IDL_KW_OUT) == IDL_KW_OUT); // output, hence passed by address
		bool byAddress = ((requested.flags & IDL_KW_VIN) == IDL_KW_VIN);// input, but passed by address
		byAddress = (byAddress || isarray); 
		// tag 'out' those that will get a return value
		if (requested.specified != NULL) { // need write 0 or 1 in a special int in KW structure
			std::cerr << requested.keyword << ", specified: as " << std::hex << (size_t) (requested.specified) <<std::dec<< "... not present" << std::endl;
			size_t address = (size_t) (kw_result)+(size_t) (requested.specified);
			memcpy((void*) (address), &nok, sizeof (int)); //requested is in offset
		}
		if (requested.value != NULL) { // need to pass either an address of a IDL_VPTR or fill in static elements of the structure exchanged with routine
			std::cerr << requested.keyword << ", value: at " << std::hex << (size_t) (requested.value) <<std::dec<< "... not present" << std::endl;
			// IDL_KW_VIN types (IDL_KW_OUT | IDL_KW_ARRAY) have an absolute address.
			size_t address = byAddress ? (size_t) (requested.value) : (size_t) (kw_result)+(size_t) (requested.value);
			//if requested var is not present, this has to be returned
			if (iszero) {
				if (byAddress) {
					memcpy((void*) (address), NULL, sizeof (NULL)); //pass by address of a IDL_VAR
				} else {
					DFloatGDL* zero = new DFloatGDL(0);
					GDlWriteVarAtAddr(zero, std::string(requested.keyword), requested.type, address, isoutput);
				}
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
		std::cerr << "Expected: ";
		for (it = requested.begin(); it != requested.end(); ++it) {
			std::cerr << "\"" << it->first << "\",";
		}
		std::cerr << std::endl;
		// scan argk for wrong keywords
		std::cerr << "Passed: \"";
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
			std::cerr << s << "\", \"";
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
		std::cerr << std::endl;
		//populate kw_result
		for (it = requested.begin(); it != requested.end(); ++it) {
			int ipassed = it->second;
			std::cerr << "\"" << it->first << "\"," << ipassed<< std::endl;
			if (ipassed == -1) GdlExportAbsentKeyword(kw_requested[irequested], kw_result);
			else if (ipassed >= 0) GdlExportPresentKeyword(kw_requested[irequested], argk[ipassed], kw_result); //case -2: do nothing: ignored
			irequested++;
		}
		return argc;
	}

	// not so simple
//	int IDL_CDECL IDL_KWProcessByAddr(int argc, IDL_VPTR *argv, char
//			*argk_passed, IDL_KW_PAR *kw_requested, IDL_VPTR *plain_args, int mask, void *kw_result) {
//	}
		void IDL_CDECL IDL_StoreScalar(IDL_VPTR dest, int type,
				IDL_ALLTYPES * value) {
		}
	}

