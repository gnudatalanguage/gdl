/***************************************************************************
                          saverestore.cpp  -  GDL library procedure
                             -------------------
    begin                : Dec 10 2017
    copyright            : (C) 2017 by Gilles Duvert

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
#include "datatypes.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"

using namespace std;

const string rectypes[] = {"START_MARKER", //0
  "COMMON_VARIABLE", //1
  "VARIABLE", //2 
  "SYSTEM_VARIABLE", //3
  "", "", "END_MARKER", //6
  "", "", "", "TIMESTAMP", //10
  "", "COMPILED", //12
  "IDENTIFICATION", //13
  "VERSION", //14
  "HEAP_HEADER", //15
  "HEAP_DATA", //16
  "PROMOTE64", //17
  "", "NOTICE", //19
  "DESCRIPTION", //20
  "UNKNOWN", "UNKNOWN", "UNKNOWN"};
const string codes[] = {"Undefined", "BYTE", "INT", "LONG", "FLOAT", "DOUBLE", "COMPLEX", "String", "Structure", "DCOMPLEX",
  "Heap Pointer", "Object Reference", "UINT", "ULONG", "LONG64", "ULONG64"};

namespace lib {

  using namespace std;

  static std::map<long, DPtr> heapIndexMap;
  static char* saveFileAuthor;
  static char* saveFileDatestring;
  static char* saveFileUser;
  static char* saveFileHost;

#include <rpc/xdr.h>

  int xdr_convert(XDR *xdrs, DInt *buf) {
    return (xdr_short(xdrs, buf));
  }

  int xdr_convert(XDR *xdrs, DUInt *buf) {
    return (xdr_u_short(xdrs, buf));
  }

  int xdr_convert(XDR *xdrs, DLong *buf) {
#if defined(__APPLE__) && defined(__LP64__)
    /* xdr_long actually takes an int on 64bit darwin */
    return (xdr_long(xdrs, buf));
#else
    return (xdr_int(xdrs, buf));
#endif
  }

  int xdr_convert(XDR *xdrs, DULong *buf) {
#if defined(__APPLE__) && defined(__LP64__)
    /* xdr_u_long actually takes an unsigned int on 64bit darwin */
    return (xdr_u_long(xdrs, buf));
#else
    return (xdr_u_int(xdrs, buf));
#endif
  }

  bool_t xdr_convert(XDR *xdrs, DLong64 *buf) {
    return (xdr_longlong_t(xdrs, (quad_t *) (buf)));
  }

  bool_t xdr_convert(XDR *xdrs, DULong64 *buf) {
    return (xdr_u_longlong_t(xdrs, (u_quad_t *) (buf)));
  }

  int xdr_convert(XDR *xdrs, DFloat *buf) {
    return (xdr_float(xdrs, buf));
  }

  int xdr_convert(XDR *xdrs, DDouble *buf) {
    return (xdr_double(xdrs, buf));
  }

  //this is the routined used by IDL as per the documentation.

  bool_t xdr_complex(XDR *xdrs, DComplex *p) {
    return (xdr_float(xdrs, reinterpret_cast<float *> (p)) && xdr_float(xdrs, reinterpret_cast<float *> (p) + 1));
  }
  //this is the routined used by IDL as per the documentation.

  bool_t xdr_dcomplex(XDR *xdrs, DComplexDbl *p) {
    return (xdr_double(xdrs, reinterpret_cast<double *> (p)) && xdr_double(xdrs, reinterpret_cast<double *> (p) + 1));
  }

  void getTimeUserHost(XDR *xdrs) {
    int UnknownLong;
    for (int i = 0; i < 256; ++i) if (!xdr_convert(xdrs, &UnknownLong)) break;
    {
      free(saveFileDatestring);
      saveFileDatestring = 0;
      if (!xdr_string(xdrs, &saveFileDatestring, 2048)) cerr << "read error" << endl;
      //      else        fprintf(stderr, "date: \"%s\"\n", saveFileDatestring);
    }
    {
      free(saveFileUser);
      saveFileUser = 0;
      if (!xdr_string(xdrs, &saveFileUser, 2048)) cerr << "read error" << endl;
      //      else        fprintf(stderr, "user: \"%s\"\n", saveFileUser);
    }
    {
      free(saveFileHost);
      saveFileHost = 0;
      if (!xdr_string(xdrs, &saveFileHost, 2048)) cerr << "read error" << endl;
      //      else        fprintf(stderr, "host: \"%s\"\n", saveFileHost);
    }
  }

  char* getDescription(XDR *xdrs) {
    int length = 0;
    if (!xdr_int(xdrs, &length)) cerr << "error reading description string length" << endl;
    if (length > 0)
    {
      char* chars = 0;
      if (!xdr_string(xdrs, &chars, length)) cerr << "error getting string" << endl;
      return chars;
    } else return NULL;
  }

  int getVersion(XDR* xdrs) {
    DLong format;
    if (!xdr_convert(xdrs, &format)) return 0;
    //    cerr << "Format: " << format << endl;
    char* arch = 0;
    if (!xdr_string(xdrs, &arch, 2048)) return 0;
    //    cerr << arch << endl;
    char* os = 0;
    if (!xdr_string(xdrs, &os, 2048)) return 0;
    //    cerr << os << endl;
    char* release = 0;
    if (!xdr_string(xdrs, &release, 2048)) return 0;
    //    cerr << release << endl;
    return 1;
  }

  int getAuthor(XDR *xdrs) {
    free(saveFileAuthor);
    saveFileAuthor = 0;
    if (!xdr_string(xdrs, &saveFileAuthor, 2048)) return 0;
    //    cerr << author << endl;
    char* title = 0;
    if (!xdr_string(xdrs, &title, 2048)) return 0;
    //    cerr << title << endl;
    char* otherinfo = 0;
    if (!xdr_string(xdrs, &otherinfo, 2048)) return 0;
    //    cerr << otherinfo << endl;
    return 1;
  }

  int getNotice(XDR* xdrs) {
    char* notice = 0;
    if (!xdr_string(xdrs, &notice, 20480)) return 0;
    //    cerr << notice << endl;
    return 1;
  }

  dimension* getArrDesc(XDR* xdrs) {
    int arrstart;
    int UnknownLong;
    if (!xdr_convert(xdrs, &arrstart)) return NULL;
    if (arrstart != 8)
    {
      cerr << "array is not a array! abort." << endl;
      return 0;
    }
    if (!xdr_convert(xdrs, &UnknownLong)) return NULL;
    ;
    int nbytes;
    if (!xdr_convert(xdrs, &nbytes)) return NULL;
    ;
    int nEl;
    if (!xdr_convert(xdrs, &nEl)) return NULL;
    ;
    int nDims;
    if (!xdr_convert(xdrs, &nDims)) return NULL;
    ;
    if (!xdr_convert(xdrs, &UnknownLong)) return NULL;
    ;
    int nmax;
    if (!xdr_convert(xdrs, &nmax)) return NULL;
    ;
    //    cerr << "nbytes:" << nbytes << " ,nEl:" << nEl << ", nDims:" << nDims<<" ";
    int* dims = 0;
    uint sizep = 8;
    if (!xdr_array(xdrs, (char**) &dims, &sizep, 8, sizeof (int), (xdrproc_t) xdr_int)) return NULL;
    ;
    SizeT k = dims[0];
    dimension* theDim = new dimension(k);
    for (int i = 1; i < sizep; ++i)
    {
      k = dims[i];
      *theDim << k;
    }
    theDim->Purge();
    //    cerr<<*theDim<<endl;
    return theDim;
  }

  int defineCommonBlock(EnvT* e, XDR* xdrs, bool verbose) {
    int ncommonvars;
    if (!xdr_convert(xdrs, &ncommonvars)) return 0;
    char* commonname = 0;
    if (!xdr_string(xdrs, &commonname, 2048)) return 0;
    char* varnames[ncommonvars];
    for (int i = 0; i < ncommonvars; ++i) varnames[i] = 0;
    for (int i = 0; i < ncommonvars; ++i) if (!xdr_string(xdrs, &varnames[i], 2048)) return 0;

    //    cerr << "Common " << commonname << " has:" << endl;
    //    for (int i = 0; i < ncommonvars; ++i) cerr << varnames[i] << ",";
    //    cerr << endl;
    //populate common block

    //behaviour is: if one of these variables already exist, the whole common is not defined, and
    //the common variables become just normal variables:

    EnvStackT& callStack = e->Interpreter()->CallStack();
    DLong curlevnum = callStack.size();
    DSubUD* pro = static_cast<DSubUD*> (callStack[curlevnum - 1]->GetPro());

    for (int i = 0; i < ncommonvars; ++i)
    {
      std::string varName = varnames[i];
      if (pro->FindVar(varName) >= 0)
      {
        Message(varName + " is already defined with a conflicting definition.");
        return 1; //this should not produce an error.
      }
    }

    std::string commonName = commonname;

    if (pro->Common(commonName) == NULL)
    { //does not exist: create
      DCommon* newCommon = new DCommon(commonName);
      pro->AddCommon(newCommon);
      if (verbose) Message("Restored common block: " + commonName);
    }

    DCommonBase* currentcommon = pro->Common(std::string(commonname));
    for (int i = 0; i < ncommonvars; ++i)
    {
      std::string varName = varnames[i];
      currentcommon->AddVar(std::string(varnames[i]));
    }

    return 1;
  }

  DStructGDL* getDStruct(EnvT* e, XDR* xdrs, dimension* inputdims = NULL) {
    int structstart;
    if (!xdr_convert(xdrs, &structstart)) return NULL;
    if (structstart != 9)
    {
      cerr << "structure is not a structure! abort." << endl;
      return NULL;
    }
    char* structname = 0;
    if (!xdr_string(xdrs, &structname, 2048)) return NULL;
    int structure_def_flags;
    if (!xdr_convert(xdrs, &structure_def_flags)) return NULL;
    bool ispredef = false;
    if (structure_def_flags & 0x1)
    {
      ispredef = true;
//      cerr << ", PREDEF ";
    }
    bool inherits = false;
    if (structure_def_flags & 0x2)
    {
      inherits = true;
//      cerr << ", INHERITS ";
    }
    bool is_super = false;
    if (structure_def_flags & 0x4)
    {
      is_super = true;
//      cerr << ", IS_SUPER ";
    }

//    if (ispredef || inherits || is_super) cerr << "Structure name:\"" << structname << "\"" << endl;

    //    if (structure_def_flags & 0x8)
    //    {
    //      cerr << ", unknown 0x08 flag for structure_def_flags";
    //    }
    //    cerr << endl;
    int ntags;
    if (!xdr_convert(xdrs, &ntags)) return NULL;
    int struct_nbytes;
    if (!xdr_convert(xdrs, &struct_nbytes)) return NULL;
    //    cerr << "ntags=" << ntags << ",nbytes=" << struct_nbytes << endl;
    //if predef == 1 this ends the Struct_desc, meaning that the definition of such a
    //structure has already been presented and we should reuse it.
    // otherwise, we define the structure using the following entries:
    if (ispredef)
    {
      std::string name = std::string(structname);
      if (name != "$truct") // named struct
        name = StrUpCase(name);
      DStructDesc* desc =
        e->Interpreter()->GetStruct(name, e->CallingNode()); //will throw if does not exist.

      return new DStructGDL(desc, *inputdims);
    } else
    {
      //TAG_DESC repated ntags times:
      int tag_typecode[ntags];
      char* tag_name[ntags];
      int tag_flag[ntags];
      int tag_offset[ntags];
      for (int i = 0; i < ntags; ++i) tag_name[i] = 0;
      for (int i = 0; i < ntags; ++i)
      {
        //TAG_DESC:
        if (!xdr_convert(xdrs, &tag_offset[i])) break;
        if (!xdr_convert(xdrs, &tag_typecode[i])) break;
        if (!xdr_convert(xdrs, &tag_flag[i])) break;
      }
      for (int i = 0; i < ntags; ++i)
      {
        //TAGNAMES x NTAGS:
        if (!xdr_string(xdrs, &tag_name[i], 2048)) break;
      }

      int narrays = 0;
      int nstructs = 0;
      for (int i = 0; i < ntags; ++i) if (tag_flag[i] & 0x20) nstructs++;
      for (int i = 0; i < ntags; ++i) if (tag_flag[i] & 0x04) narrays++;
      dimension * tagdimensions[narrays + 1]; //Always >0: FIXME IF MEMORY LEAK!
      //      cerr<<"reading "<<nstructs<<" structs and "<<narrays<<" arrays."<<endl;
      //if there are any tag flags indicating the tag is an array, read the ARRDESC
      for (int i = 0; i < narrays; ++i)
      {
        tagdimensions[i] = getArrDesc(xdrs);
        if (tagdimensions[i] == NULL) return NULL;
      }

      DStructGDL * substruct[nstructs + 1]; //Always >0: FIXME IF MEMORY LEAK!
      std::string stru_name = std::string(structname);
      DStructDesc* stru_desc = new DStructDesc((stru_name.length() == 0) ? "$truct" : stru_name);
      DStructGDL* res_stru = new DStructGDL(stru_desc);

      //if there are any tag flags indicating the tag is a STRUCTURE, read the STRUCTDESC
      for (int i = 0; i < nstructs; ++i)
      {
        substruct[i] = getDStruct(e, xdrs);
        if (substruct[i] == NULL) return NULL;
      }
      //summary & tag population:
      for (int i = 0, j = 0, k = 0; i < ntags; ++i)
      {
        //reserve a DStruct
        DStructGDL* parStruct = NULL;
        //reserved a dimension
        dimension pardim = dimension();
        //        cerr << "Tag " << tag_name[i] << " is " << codes[tag_typecode[i]];
        if (tag_flag[i] & 0x20)
        {
          //          cerr << " Structure";
          // attribute parstruct and push struct index:
          parStruct = substruct[k++];
        }
        //        if (tag_flag[i] & 0x10) cerr << " Pointer to array";
        if (tag_flag[i] & 0x04)
        {
          // modify pardim and push index;
          pardim = *(tagdimensions[j++]); //memory leak?
          //          cerr << " " << pardim << endl;
        }

        switch (tag_typecode[i]) {
          case 1: //	Byte
          {
            SpDByte entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case 2: //	16-bit Integer 
          {
            SpDInt entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case 3: //	32-bit Long Integer 
          {
            SpDLong entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case 4: //	32-bit Floating Point Number 
          {
            SpDFloat entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case 5: //	64-bit Floating Point Number 
          {
            SpDDouble entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case 6: //	Complex Floating Point Number (32-bits each) 
          {
            SpDComplex entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;
          case 7: //	String
          {
            SpDString entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;
          case 8: //	Structure (never a scalar)
          {
            stru_desc->AddTag(tag_name[i], parStruct);
          }
            break;

          case 9: //	Complex Floating Point Number (64-bits each) 
          {
            SpDComplexDbl entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case 10: //	Heap Pointer 
          {
            DPtrGDL entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;
          case 11: //	Object Reference (not supported by CMSVLIB)
          {
            DObjGDL entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;
          case 12: //	16-bit Unsigned Integer 
          {
            SpDUInt entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case 13: //	32-bit Unsigned Integer 
          {
            SpDULong entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case 14: //	64-bit Integer 
          {
            SpDLong64 entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case 15: //	64-bit Unsigned Integer 
          {
            SpDULong64 entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          default: //	0 ? Undefined (not allowed) 
            //            cerr << "Should not happen" << endl;
            break;
        }



        //        cerr << endl;
      }

      //Then there should be CLASSNAME if INHERITS or IS_SUPER 
      if (inherits || is_super)
      {
        char* classname = 0;
        if (!xdr_string(xdrs, &classname, 2048)) return NULL;
        cerr << "CLASSNAME: \"" << classname << "\"" << endl;
        //NSUPCLASSES:
        int nsupclasses = 0;
        if (!xdr_convert(xdrs, &nsupclasses)) return NULL;
        cerr << "NSUPCLASSES=" << nsupclasses << endl;
        if (nsupclasses > 0)
        {
          for (int i = 0; i < nsupclasses; ++i)
          {
            char* supclassname = 0;
            if (!xdr_string(xdrs, &supclassname, 2048)) return NULL;
            cerr << "SUPCLASSNAME " << i << ": " << supclassname << endl;
          }
          for (int i = 0; i < nsupclasses; ++i)
          {
            cerr << "OBJECT Definition not handled, fixme!" << endl;
            DStructGDL* toto = getDStruct(e, xdrs);
            if (toto == NULL) return NULL; else GDLDelete(toto);
          }
        }
      }
      return new DStructGDL(stru_desc, *inputdims);
    }
  }

  BaseGDL* getVariable(EnvT* e, XDR* xdrs, int &isSysVar) {
    bool isStructure = false;
    bool isArray = false;
    // start of TYPEDESC
    // common for VARIABLE, SYSTEM_VARIABLE and HEAP_DATA:
    // 1) TYPECODE
    int typecode;
    if (!xdr_convert(xdrs, &typecode)) return NULL;
    // 2) VARFLAGS
    int varflags;
    if (!xdr_convert(xdrs, &varflags)) return NULL;

    if (varflags & 0x02) //defines a system variable.
    {
      isSysVar |= 0x02;
//            cerr << " system " << endl;
    }
    if (varflags & 0x01)
    {
      isSysVar |= 0x01;
//            cerr << " readonly " << endl;
    }


    if (varflags & 0x20)
    {
      isStructure = true; //may also be an array!
    }
    else if (varflags & 0x04)
    {
      isArray = true;
    }
    //This is not signaled in C. Marqwardt doc: a system variable has two supplemental int32 (0x04 and 0x02) here, that we skip.
    if (isSysVar & 0x02)
    {
      int dummy;
      if (!xdr_convert(xdrs, &dummy)) return NULL;
      if (!xdr_convert(xdrs, &dummy)) return NULL;
    }
    //we gonnna create a BaseGDL:

    BaseGDL* var;
    dimension* dims;
    // if ARRAY or STRUCTURE, Read ARRAY_DESC that follows:
    if (isStructure)
    {//if This was a Structure, it has an ARRAY_DESC plus a STRUCT_DESC 
      dims = getArrDesc(xdrs);
      if (dims == NULL) return NULL;
      var = getDStruct(e, xdrs, dims);
    } else
    {
      if (isArray)
      { //and NOT a structure...
        dims = getArrDesc(xdrs);
        if (dims == NULL) return NULL;
      } else
      { //normal plain variable
        dims = new dimension();
      }
      switch (typecode) {
        case 1: //	Byte
          var = new DByteGDL(*dims);
          break;
        case 2: //	16-bit Integer 
          var = new DIntGDL(*dims);
          break;
        case 3: //	32-bit Long Integer 
          var = new DLongGDL(*dims);
          break;
        case 4: //	32-bit Floating Point Number 
          var = new DFloatGDL(*dims);
          break;
        case 5: //	64-bit Floating Point Number 
          var = new DDoubleGDL(*dims);
          break;
        case 6: //	Complex Floating Point Number (32-bits each) 
          var = new DComplexGDL(*dims);
          break;
        case 7: //	String
          var = new DStringGDL(*dims);
          break;
        case 8: //	Structure (never a scalar)
          cerr << "Should not happen: struct" << endl;
          break;
        case 9: //	Complex Floating Point Number (64-bits each) 
          var = new DComplexDblGDL(*dims);
          break;
        case 10: //	Heap Pointer 
          var = new DPtrGDL(*dims);
          break;
        case 11: //	Object Reference (not supported by CMSVLIB) 
          cerr << "Should not happen: object." << endl;
          return NULL;
          break;
        case 12: //	16-bit Unsigned Integer 
          var = new DUIntGDL(*dims);
          break;
        case 13: //	32-bit Unsigned Integer 
          var = new DULongGDL(*dims);
          break;
        case 14: //	64-bit Integer 
          var = new DLong64GDL(*dims);
          break;
        case 15: //	64-bit Unsigned Integer 
          var = new DULong64GDL(*dims);
          break;
        default: //	0 ? Undefined (not allowed) 
          //          cerr <<"Should not happen"<<endl;
          break;
      }
    }

    return var;
  }

  void fillVariableData(XDR* xdrs, BaseGDL* var) {
    u_int nEl = var->N_Elements();
    switch (var->Type()) {

      case GDL_BYTE:
      {
        char* addr = (char*) var->DataAddr();
        if (!xdr_bytes(xdrs, &addr, &nEl, nEl)) cerr << "error GDL_BYTE" << endl;
      }
        break;
      case GDL_INT:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DInt), (xdrproc_t) xdr_int)) cerr << "error GDL_INT" << endl;
      }
        break;
      case GDL_UINT:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DUInt), (xdrproc_t) xdr_u_int)) cerr << "error GDL_UINT" << endl;
      }
        break;
      case GDL_LONG:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DLong), (xdrproc_t) xdr_long)) cerr << "error GDL_LONG" << endl;
      }
        break;
      case GDL_ULONG:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DULong), (xdrproc_t) xdr_u_long)) cerr << "error GDL_ULONG" << endl;
      }
        break;
      case GDL_LONG64:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DLong64), (xdrproc_t) xdr_u_longlong_t)) cerr << "error GDL_LONG64" << endl;
      }
        break;
      case GDL_ULONG64:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DULong64), (xdrproc_t) xdr_u_longlong_t)) cerr << "error GDL_ULONG64" << endl;
      }
        break;
      case GDL_FLOAT:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DFloat), (xdrproc_t) xdr_float)) cerr << "error GDL_FLOAT" << endl;
      }
        break;
      case GDL_DOUBLE:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DDouble), (xdrproc_t) xdr_double)) cerr << "error GDL_DOUBLE" << endl;
      }
        break;
      case GDL_COMPLEX:
      {
        u_int nEl2 = nEl * 2;
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl2, sizeof (DFloat), (xdrproc_t) xdr_float)) cerr << "error GDL_COMPLEX" << endl;
      }
        break;
      case GDL_COMPLEXDBL:
      {
        u_int nEl2 = nEl * 2;
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl2, sizeof (DDouble), (xdrproc_t) xdr_double)) cerr << "error GDL_COMPLEXDBL" << endl;
      }
        break;
      case GDL_STRING:
      {
        for (SizeT i = 0; i < nEl; ++i)
        {
          int length;
          if (!xdr_int(xdrs, &length)) cerr << "error reading string length" << endl;
          if (length > 0)
          {
            char* chars = 0;
            if (!xdr_string(xdrs, &chars, length)) cerr << "error getting string" << endl;
            (*static_cast<DStringGDL*> (var))[i].assign(chars);
          }
        }
      }
        break;
      case GDL_STRUCT:
      {
        DStructGDL* str = static_cast<DStructGDL*> (var);
        SizeT nTags = str->Desc()->NTags();
        for (SizeT ix = 0; ix < nEl; ++ix) for (SizeT t = 0; t < nTags; ++t) fillVariableData(xdrs, str->GetTag(t, ix));
        break;
      }
      case GDL_PTR:
      {
        int heapNumber[nEl];
        DPtrGDL* ptr = static_cast<DPtrGDL*> (var);
        for (SizeT ix = 0; ix < nEl; ++ix) xdr_int(xdrs, &(heapNumber[ix]));
        for (SizeT ix = 0; ix < nEl; ++ix)
        {
          DPtr heapptr = heapIndexMap.find(heapNumber[ix])->second;
          (*ptr)[ix] = heapptr;
        }
        break;
      }
      case GDL_OBJ:
        break;
      default: assert(false);
    }
  }

  void restoreNormalVariable(EnvT* e, std::string varName, BaseGDL* ret) {
    //write variable back
    EnvStackT& callStack = e->Interpreter()->CallStack();
    DLong curlevnum = callStack.size();
    DSubUD* pro = static_cast<DSubUD*> (callStack[curlevnum - 1]->GetPro());
    SizeT nVar = pro->Size(); // # var in GDL for desired level 
    int nKey = pro->NKey();
    //    cout << "nKey:" << nKey << endl;
    //    cout << "nVar:" << nVar << endl;
    //    cout << pro->Name() << endl;
    int xI = pro->FindVar(varName);
    //    cout << "varName: " << varName << " xI: " << xI << endl;
    SizeT s;
    if (xI != -1)
    {
      s = xI;
      //      cout << "Found Already existing Var \""<< varName <<" s=" << s << endl;
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
        //        cout << "AddVar u: " << u << endl;
        //        cout << "AddEnv s: " << s << endl;
        ((EnvT*) (callStack[curlevnum - 1]))->GetPar(s - nKey) = ret;
      }
    }
  }

  void restoreSystemVariable(EnvT* e, std::string sysVarNameFull, BaseGDL* ret, bool rdOnly = false) {
    //more or less a copy of "DEFSYSV" code...
    if (sysVarNameFull.length() < 2 || sysVarNameFull[0] != '!')
    {
      Warning("Not restoring illegal system variable name: " + sysVarNameFull + ".");
      GDLDelete(ret);
      return;
    }
    // strip "!", uppercase
    DString sysVarName = StrUpCase(sysVarNameFull.substr(1));

    DVar* sysVar = FindInVarList(sysVarList, sysVarName);

    if (sysVar == NULL)
    {
      // define new
      DVar *newSysVar = new DVar(sysVarName, ret);
      sysVarList.push_back(newSysVar);

      // rdOnly is only set at the first definition
      if (rdOnly) sysVarRdOnlyList.push_back(newSysVar);
      return;
    }

    // else: re-set
    // make sure type and size are kept
    BaseGDL* oldVar = sysVar->Data();
    if (oldVar->Type() != ret->Type() || oldVar->N_Elements() != ret->N_Elements())
    {
      Message("Conflicting definition for " + sysVarNameFull + ".");
      GDLDelete(ret);
      return;
    }

    // if struct -> assure equal descriptors
    if (oldVar->Type() == GDL_STRUCT)
    {
      DStructGDL *oldStruct = static_cast<DStructGDL*> (oldVar);
      // types are same -> static cast
      DStructGDL *newStruct = static_cast<DStructGDL*> (ret);

      // note that IDL handles different structs more relaxed
      // ie. just the structure pattern is compared.
      if (*oldStruct->Desc() != *newStruct->Desc())
      {
        Warning("Conflicting definition for " + sysVarNameFull + ".");
        GDLDelete(ret);
        return;
      }

      DVar* sysVarRdOnly = FindInVarList(sysVarRdOnlyList, sysVarName);
      if (sysVarRdOnly != NULL)
      {
        // rdOnly set and is already rdOnly: do nothing
        if (rdOnly) return;

        // else complain
        Warning("Attempt to write to a readonly variable: " + sysVarNameFull + ".");
      } else
      {
        // not read only
        GDLDelete(oldVar);
        sysVar->Data() = ret;
      }
    }
  }
  
  //clever but very VERY VERY dirty trick not gentle with memory and full of leaks:
  XDR* compress_trick(FILE* fid, XDR* xdrsmem, char* expanded, DULong64 nextptr, DULong64 currentptr) {
    if (expanded) free(expanded);
    uLong compsz = nextptr - currentptr;
    char* expandable = (char*) malloc(compsz);
    size_t retval=fread(expandable, 1, compsz, fid);
    int iloop = 1;
    uLong uncompsz;
    //of course one should never do like that. One should do as in gzstream.hpp...
    while (1)
    {
      uncompsz = 10 * iloop*compsz; //a default starting value...
      expanded = (char*) malloc(uncompsz); 
      int retval=uncompress((Bytef *) expanded, &uncompsz, (Bytef *) expandable, compsz);
      if ( retval == Z_OK) break; //ok length was sufficient
      free(expanded);
      if ( retval != Z_BUF_ERROR) throw GDLException("fatal error when uncompressing data.");
      iloop++;
    }
    free(expandable);
    xdrmem_create(xdrsmem, expanded, uncompsz, XDR_DECODE);
    return xdrsmem;
  }
  
  // new fast restore.

  void gdl_restore(EnvT* e) {

    static int VERBOSE = e->KeywordIx("VERBOSE");
    static int FILENAME = e->KeywordIx("FILENAME");
    static int DESCRIPTION = e->KeywordIx("DESCRIPTION");

    bool verbose = e->KeywordSet(VERBOSE);
    bool needsDescription = e->KeywordPresent(DESCRIPTION);

    //empty heap map by security.
    heapIndexMap.clear();

    std::vector<Guard<BaseGDL>* > guardVector;
    //    std::vector<BaseGDL*> myObj;
    std::vector<std::pair<std::string, BaseGDL*> > variableVector;
    std::vector<std::pair<std::string, BaseGDL*> > systemVariableVector;
    std::vector<std::pair<std::string, BaseGDL*> > systemReadonlyVariableVector; //for readonly variables

    DString name;
    // IDL allows here also arrays of length 1
    SizeT nparam = e->NParam();
    if (nparam > 0)
    {
      e->AssureScalarPar<DStringGDL>(0, name);
    } else if (e->KeywordPresent(FILENAME))
    {
      e->AssureScalarKW<DStringGDL>(FILENAME, name);
    } else name = "idlsave.dat";

    FILE *fid;
    fid = fopen(name.c_str(), "r");
    if (fid == NULL) e->Throw("Error opening file. Unit: XXXX, File: " + name + ".");

    XDR* xdrsmem=new XDR;
    XDR* xdrs;
    XDR* xdrsfile=new XDR;
    xdrstdio_create(xdrsfile, fid, XDR_DECODE);
    xdrs=xdrsfile;
    char* expanded=NULL;
    
    SizeT returned;
    char signature[4];
    returned = fread(signature, 4, 1, fid);
    if (signature[0] != 'S' || signature[1] != 'R')
    {
      fclose(fid);  delete xdrsmem; delete xdrsfile;
      e->Throw("Not a valid save file: " + name + ".");
    }
//    if (signature[3]==0x06) cerr<<"probably compressed"<<endl;
#define LONG sizeof(DLong) //sizeof(DInt)
#define ULONG LONG 


    bool isHdr64 = false;
    int isSysVar = 0x00;
    bool isArray = false;
    bool isStructure = false;
    bool isCompress = false;
    //will start at TMESTAMP
    DULong64 currentptr = 0;
    DULong64 nextptr = LONG;
    DULong ptrs0, ptrs1;
    DLong rectype;
    DLong UnknownLong;
    bool SomethingFussyHappened = true;

    //pass twice. First to define heap variables only (and ancillary data).

    while (1)
    {
      xdrs=xdrsfile; //back to file if we were smarting the xdr to read a char* due to compression.
      if (fseek(fid, nextptr, SEEK_SET)) break;
      if (!xdr_convert(xdrs, &rectype)) break;

//      if (isCompress) cerr << "\nOffset " << nextptr << ": record type " << rectypes[rectype] << endl;

      if (rectype == 6)
      {
        SomethingFussyHappened = false;
        break;
      }
      if (isHdr64)
      {
        if (!xdr_convert(xdrs, &nextptr)) break;
        if (!xdr_convert(xdrs, &UnknownLong)) break;
        if (!xdr_convert(xdrs, &UnknownLong)) break;
      } else
      {
        if (!xdr_convert(xdrs, &ptrs0)) break;
        if (!xdr_convert(xdrs, &ptrs1)) break;
        if (!xdr_convert(xdrs, &UnknownLong)) break;
        nextptr = ptrs0;
        if (ptrs1 > 0)
        {
          DULong64 tmp = ptrs1;
          nextptr &= (tmp << 32);
        }
      }

      //dispatch accordingly:

      isSysVar = 0x00;
      isArray = false;
      isStructure = false;
      currentptr = ftell(fid);

      switch ((int) rectype) {
        case 10: //timestamp
          if (nextptr < 1024)
          {
//            cerr << "version offset < 1024... probably a compressed file" << endl;
//            Message("sorry, can''t deal with this yet.  if possible, save without the \"COMPRESS\" flag.");
            isCompress=true;
          }
          if (isCompress) xdrs=compress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
          getTimeUserHost(xdrs);
          if (verbose)
          {
            if (isCompress) Message("Portable (XDR) compressed SAVE/RESTORE file."); else Message("Portable (XDR) SAVE/RESTORE file.");
            Message("Save file written by " + std::string(saveFileUser) + "@" + std::string(saveFileHost) + ", " + std::string(saveFileDatestring));
          }
          break;
          //        case 14:
          //          if (!getVersion(xdrs))
          //          {
          //            cerr << "error in VERSION" << endl;
          //            break;
          //          }
          //          break;
        case 17: //PROMOTE64
          isHdr64 = true;
          break;
        case 13: //IDENTIFICATION
          if (verbose)
          {
            if (isCompress) xdrs=compress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
            if (!getAuthor(xdrs))
            {
              cerr << "error in AUTHOR" << endl;
              break;
            }
          }
          break;
          //        case 19: //NOTICE
          //          if (!getNotice(xdrs))
          //          {
          //            cerr << "error in NOTICE" << endl;
          //            break;
          //          }
          //          break;
        case 20: //description
          if (verbose || needsDescription) {
            if (isCompress) xdrs=compress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
            std::string descr(getDescription(xdrs));
            if (verbose) Message("Description: "+descr);
            if (needsDescription) e->SetKW(DESCRIPTION, new DStringGDL(descr));
          }
          break;
        case 1: //COMMONBLOCK
          if (isCompress) xdrs=compress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
          if (!defineCommonBlock(e, xdrs, verbose))
          {
            cerr << "error in COMMONBLOCK" << endl;
            break;
          }
          break;
        case 15: //HEAP_HEADER. IS BEFORE ANY REFERENCE TO HEAP.
          if (isCompress) xdrs=compress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
        {
          u_int elementcount;
          if (!xdr_convert(xdrs, &elementcount)) break;
          int indices[elementcount];
          if (!xdr_vector(xdrs, (char*) indices, elementcount, sizeof (int), (xdrproc_t) xdr_int)) break;
          //          cerr << "Heap indexes, " << elementcount << " elements: ";
          //          for (int i = 0; i < elementcount; ++i) cerr << indices[i] << ",";
          //          cerr << endl;

          break;
        }
        case 16: //HEAP_DATA
          if (isCompress) xdrs=compress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
        {
          int heap_index = 0;
          if (!xdr_convert(xdrs, &heap_index)) break;
          int heap_unknown = 0;
          if (!xdr_convert(xdrs, &heap_unknown)) break; // start of TYPEDESC
          BaseGDL* ret = getVariable(e, xdrs, isSysVar);
          if (ret == NULL) {
            fclose(fid);  delete xdrsmem; delete xdrsfile;
            e->Throw("error reading heap variable data.");
          }
          // should be at varstat=7 to read data
          int varstart = 0;
          if (!xdr_convert(xdrs, &varstart)) break;
          if (varstart != 7)
          {
            fclose(fid);  delete xdrsmem; delete xdrsfile;
            e->Throw("Lost track in HEAP_DATA definition at offset " + i2s(nextptr));
            
          }

          fillVariableData(xdrs, ret);
          Guard<BaseGDL>* guard = new Guard<BaseGDL>;
          guard->Reset(ret);
          guardVector.push_back(guard);
          //allocate corresponding heap entries and store in saveFileHeapMap:
          DPtr ptr = e->NewHeap(1, ret);
          heapIndexMap.insert(std::pair<long, DPtr>(heap_index, ptr));
          //          //insert in heap index map at good place
          //          heapIndexMap.insert(heapIndexMap.find(heap_index),std::pair<long,DPtr>(heap_index,ptr));
        }
          break;
        default:
          break;
      }
    }

    if (SomethingFussyHappened) {
      fclose(fid);  delete xdrsmem; delete xdrsfile;
      e->Throw("Error Reading File: " + name + ".");
    }

    //from then, saveFileHeapMap.second contains heap DPtr.

    //Then on second pass, define normal variables that may be pointers to heap.
    rewind(fid);
    currentptr = 0;
    nextptr = LONG;
    SomethingFussyHappened = true;

    while (1)
    {

      xdrs=xdrsfile; //back to file if we were smarting the xdr to read a char* due to compression.
      if (fseek(fid, nextptr, SEEK_SET)) break;
      if (!xdr_convert(xdrs, &rectype)) break;

//      if (isCompress) cerr << "\nOffset " << nextptr << ": record type " << rectypes[rectype] << endl;

      if (rectype == 6)
      {
        SomethingFussyHappened = false;
        break;
      }
      if (isHdr64)
      {
        if (!xdr_convert(xdrs, &nextptr)) break;
        if (!xdr_convert(xdrs, &UnknownLong)) break;
        if (!xdr_convert(xdrs, &UnknownLong)) break;
      } else
      {
        if (!xdr_convert(xdrs, &ptrs0)) break;
        if (!xdr_convert(xdrs, &ptrs1)) break;
        if (!xdr_convert(xdrs, &UnknownLong)) break;
        nextptr = ptrs0;
        if (ptrs1 > 0)
        {
          DULong64 tmp = ptrs1;
          nextptr &= (tmp << 32);
        }
      }

      //dispatch accordingly:

      isSysVar = 0x00;
      isArray = false;
      isStructure = false;
      currentptr = ftell(fid);

      switch ((int) rectype) {
        case 3: //SYSTEM VARIABLE
          isSysVar = 0x02; //see? no break. defines a read-write system variable (default)
        case 2: //VARIABLE
          if (isCompress) xdrs=compress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
        {
          char* varname = 0;
          if (!xdr_string(xdrs, &varname, 2048)) break;
          string varName(varname);
          BaseGDL* ret = getVariable(e, xdrs, isSysVar);
          if (ret == NULL) {
            fclose(fid);  delete xdrsmem; delete xdrsfile;
            e->Throw("error reading variable data.");
          }

          // should be at varstat=7 to read data
          int varstart = 0;
          if (!xdr_convert(xdrs, &varstart)) break;
          if (varstart != 7)
          {
            fclose(fid);  delete xdrsmem; delete xdrsfile;
            e->Throw("Lost track in VARIABLE definition at offset " + i2s(nextptr));
          }

          fillVariableData(xdrs, ret);

          if (isSysVar & 0x01) systemReadonlyVariableVector.push_back(make_pair(varName, ret));
          else if (isSysVar & 0x02) systemVariableVector.push_back(make_pair(varName, ret));
          else variableVector.push_back(make_pair(varName, ret));
          Guard<BaseGDL>* guard = new Guard<BaseGDL>;
          guard->Reset(ret);
          guardVector.push_back(guard);
        }
          break;
        default:
          break;
      }
    }
    fclose(fid);
    delete xdrsmem;
    delete xdrsfile;

    //if problem, guards should deleted the allocated BaseGDLs.
    if (SomethingFussyHappened) e->Throw("Error Reading File: " + name + ".");
    //here everything was ok



    while (!systemVariableVector.empty())
    {
      restoreSystemVariable(e, systemVariableVector.back().first, (systemVariableVector.back()).second);
      if (verbose) Message("Restored system variable: " + (systemVariableVector.back()).first);
      systemVariableVector.pop_back();
    }
    while (!systemReadonlyVariableVector.empty())
    {
      restoreSystemVariable(e, systemReadonlyVariableVector.back().first, (systemReadonlyVariableVector.back()).second, true);
      if (verbose) Message("Restored system variable: " + (systemReadonlyVariableVector.back()).first);
      systemReadonlyVariableVector.pop_back();
    }
    while (!variableVector.empty())
    {
      restoreNormalVariable(e, variableVector.back().first, (variableVector.back()).second);
      if (verbose) Message("Restored variable: " + (variableVector.back()).first);
      variableVector.pop_back();
    }

    //everything ok, remove guards and exit
    while (!guardVector.empty())
    {
      guardVector.back()->Release();
      guardVector.pop_back();
    }

  }

}

