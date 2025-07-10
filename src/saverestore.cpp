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

#include "dinterpreter.hpp"
#include <queue>

#include "print_tree.hpp"

#include <antlr/ASTRefCount.hpp>
#include <antlr/AST.hpp>
#include "dnode.hpp"

//Useful for debugging...
#define DEBUG_SAVERESTORE 0

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
  "UNKNOWN", "UNKNOWN", "UNKNOWN", //23
  "GDL Private"};

const int sizeOfType[] = {0, 1, 2, 4, 4, 8, 8, 0, 0, 16,0,0,2,4,8,8};

namespace lib {

  using namespace std;
enum {
  START_MARKER=0, // Start of SAVE file
  COMMONBLOCK=1, // Block contains a common block definition
  VARIABLE=2, // Block contains variable data
  SYSTEM_VARIABLE=3, // Block contains system variable data
  END_MARKER=6, // End of SAVE file for IDL
  VARSTART=7, //start of variable data
  ARRAYSTART=8,
  STRUCTSTART=9,
  TIMESTAMP=10, //Block contains time stamp information
  COMPILED=12, // Block contains idl semi-compiled? procedure or function
  IDENTIFICATION=13, // Block contains author information
  VERSION_MARKER=14, // Block contains IDL version information
  HEAP_HEADER=15, // Block contains heap index information
  HEAP_DATA=16, // Block contains heap data
  PROMOTE64=17, // Flags start of 64-bit record file offsets
  ARRAYSTART64=18,
  NOTICE=19, // Disclaimer notice
  DESCRIPTION_MARKER=20, //description ?
  GDL_COMPILED = 1789, // revolutionary. Block contains gdl semi-compiled procedure or function
  GDL_END_MARKER = 1790 // Real End of SAVE file for GDL
} Markers;

//should be increased each time the COMPILER CODE (gdlc.g, etc) is modified and the Tokens are changed.
#define GDL_SAVE_ROUTINES_VERSION 3

  typedef std::map<DPtr, SizeT> heapT;
  static heapT heapIndexMapSave;  //list of [ heap pointer, heap index ] used when saving.
  static std::map<long, std::pair<BaseGDL*,DPtr>> heapIndexMapRestore; //list of [heap index , [variable,heap pointer]] used when reading.
  static std::vector<std::string> predeflist;
  static char* saveFileAuthor;
  static char* saveFileDatestring;
  static char* saveFileUser;
  static char* saveFileHost;
  
  static char* notice;
  static int32_t format;
  static  char* arch = 0;
  static  char* os = 0;
  static  char* release = 0;

  static bool safetyTested=false;
  static bool isSafe=false;
  
  static bool save_compress=false;
  static FILE* save_fid=NULL;


  // AC 2017-12-13 : missing in  <rpc/xdr.h> for OSX
  // Following https://www.gnu.org/software/gnulib/manual/html_node/xdr_005fint16_005ft.html
  // it may be needed for others OS : Cygwin, Mingw (seems to be OK for *BSD)

  // AC 2023-12-27
  // xdr_uint16_t are defined in old Linux systems (but no xdr_u_int16_t)
  // xdr_uint16_t and xdr_u_int16_t in new (eg u2204) Linux systems
  // xdr_u_int16_t are defined in OSX since 2017 (but no xdr_uint16_t )
#ifdef __APPLE__
#define xdr_uint16_t xdr_u_int16_t
#define xdr_uint32_t xdr_u_int32_t
#define xdr_uint64_t xdr_u_int64_t
#endif
  
//windows is the only one to use portablexdr at the moment.  
#ifdef  _WIN32
#   define u_int64_t uint64_t
#define xdr_int8_t xdr_char
#define xdr_u_int8_t xdr_u_char
#define xdr_uint8_t xdr_short
#define xdr_int16_t xdr_short
#define xdr_u_int16_t xdr_u_short
#define xdr_uint16_t xdr_u_short
#define xdr_int32_t xdr_long
#define xdr_u_int32_t xdr_u_long
#define xdr_uint32_t xdr_u_long
#endif
#include <rpc/xdr.h>
  
  static u_int64_t ENDOFLIST =   0xFFFFFFFFFFFFFFFF;
//  void writeCData(XDR *xdrs, BaseGDL* var);
//  BaseGDL* readCData(EnvT* e, XDR *xdrs);

//  write a copy of the SemiCompiledCode stored along with any new compiled routine.

  void wr_writeNode(sccstruct savenode, XDR* xdrs) {
	//write node index:
	xdr_u_int(xdrs, & savenode.node);
	//write right index:
	xdr_u_int(xdrs, & savenode.right);
	//write down index:
	xdr_u_int(xdrs, & savenode.down);
	//other infos
    xdr_u_int(xdrs, &savenode.nodeType); //ttype
	u_int l = (savenode.Text).length();
	xdr_u_int(xdrs, &l);
	char *t = (char*) (savenode.Text).c_str();
	xdr_string(xdrs, &t, l); //TEXT
	xdr_u_int(xdrs, & savenode.ligne); //LineNUM
	xdr_u_int(xdrs, & savenode.flags);
	int varType = 0;
	if (savenode.var!=NULL) {
	  varType = savenode.var->Type(); //will be 0 otherwise
	}
	xdr_int32_t(xdrs, &varType);
	if (varType > 0) {
	  switch (varType) {
	  case GDL_FLOAT:
	  {
		DFloat f = (*static_cast<DFloatGDL*> (savenode.var))[0];
		xdr_float(xdrs, &f);
		break;
	  }
	  case GDL_DOUBLE:
	  {
		DDouble d = (*static_cast<DDoubleGDL*> (savenode.var))[0];
		xdr_double(xdrs, & d);
		break;
	  }
		//		case GDL_COMPLEX: //should not happen
		//		case GDL_COMPLEXDBL:
	  case GDL_ULONG64:
	  {
		u_int64_t ul6 = (*static_cast<DULong64GDL*> (savenode.var))[0];
		xdr_uint64_t(xdrs, & ul6);
		break;
	  }
	  case GDL_LONG64:
	  {
		int64_t l6 = (*static_cast<DLong64GDL*> (savenode.var))[0];
		xdr_int64_t(xdrs, & l6);
		break;
	  }
	  case GDL_BYTE:
	  {
		DByte b = (*static_cast<DByteGDL*> (savenode.var))[0];
		xdr_u_char(xdrs, & b);
		break;
	  }
	  case GDL_INT:
	  {
		DInt i = (*static_cast<DIntGDL*> (savenode.var))[0];
		xdr_short(xdrs, & i);
		break;
	  }
	  case GDL_UINT:
	  {
		DUInt ui = (*static_cast<DUIntGDL*> (savenode.var))[0];
		xdr_u_short(xdrs, & ui);
		break;
	  }
	  case GDL_LONG:
	  {
		int32_t l = (*static_cast<DLongGDL*> (savenode.var))[0];
		xdr_int32_t(xdrs, & l);
		break;
	  }
	  case GDL_ULONG:
	  {
		u_int32_t ul = (*static_cast<DULongGDL*> (savenode.var))[0];
		xdr_uint32_t(xdrs, & ul);
		break;
	  }
	  case GDL_STRING:
	  {
		DString s = (*static_cast<DStringGDL*> (savenode.var))[0];
		u_int l = s.length();
		char* c = (char*) s.c_str();
		xdr_string(xdrs, &c, l);
		break;
	  }
	  case GDL_UNDEF:
		break;
	  default: std::cerr << "unsupported constant var in procedure code :" << varType;
	  }
	}
  } 



  typedef struct _SAVENODE_STRUCT_ {
	u_int nodeType = 0;
	u_int ligne = 0;
	u_int flags = 0;
	u_int64_t node = 0;
	u_int64_t right = 0;
	u_int64_t down = 0;
	BaseGDL* var = NULL;
	std::string Text;
  } savenodestruct;

  bool rd_readNode(EnvT*e, XDR* xdrs, std::vector<savenodestruct> &nodes) {
	//read node marker. if END,return
	savenodestruct savenode;
	// address is not that simple to retrieve. The address is what ' std::cout<<this " prints, and it is NOT easy to get it right. 'This is the way'.
	//read node address:
	u_int node=0;
	xdr_u_int(xdrs, & node);
	if (node == 0xFFFFFFFF) return false;
	savenode.node=node;
	//read right address:
	xdr_u_int(xdrs, & node);
	savenode.right=node;
	//read down address:
	xdr_u_int(xdrs, & node);
	savenode.down=node;
	xdr_u_int(xdrs, &savenode.nodeType);
	//read text length
	u_int l = 0;
	xdr_u_int(xdrs, &l);
	//read text
	char *text = NULL;
	xdr_string(xdrs, &text, l); //TEXT
	savenode.Text = std::string(text);
	xdr_u_int(xdrs, &savenode.ligne); //LineNUM
	// the 'flags' are an union of ints, with different meanings depending on the actual node
	xdr_u_int(xdrs, &savenode.flags);
	u_int varType = 0;
	xdr_u_int(xdrs, &varType); //get VarType
	savenode.var=NULL;
	if (varType > 0) {
	  switch (varType) {
	  case GDL_FLOAT:
	  {
		DFloat f = 0;
		xdr_float(xdrs, &f);
		savenode.var = new DFloatGDL(f);
		break;
	  }
	  case GDL_DOUBLE:
	  {
		DDouble d = 0;
		xdr_double(xdrs, & d);
		savenode.var = new DDoubleGDL(d);
		break;
	  }
		//		case GDL_COMPLEX:
		//		case GDL_COMPLEXDBL:
	  case GDL_ULONG64:
	  {
		u_int64_t ul6 = 0;
		xdr_uint64_t(xdrs, & ul6);
		savenode.var = new DULong64GDL(ul6);
		break;
	  }
	  case GDL_LONG64:
	  {
		int64_t l6 = 0;
		xdr_int64_t(xdrs, & l6);
		savenode.var = new DLong64GDL(l6);
		break;
	  }
	  case GDL_BYTE:
	  {
		DByte b = 0;
		xdr_u_char(xdrs, & b);
		savenode.var = new DByteGDL(b);
		break;
	  }
	  case GDL_INT:
	  {
		DInt i = 0;
		xdr_short(xdrs, & i);
		savenode.var = new DIntGDL(i);
		break;
	  }
	  case GDL_UINT:
	  {
		DUInt ui = 0;
		xdr_u_short(xdrs, & ui);
		savenode.var = new DUIntGDL(ui);
		break;
	  }
	  case GDL_LONG:
	  {
		int32_t l = 0;
		xdr_int32_t(xdrs, & l);
		savenode.var = new DLongGDL(l);
		break;
	  }
	  case GDL_ULONG:
	  {
		u_int32_t ul = 0;
		xdr_uint32_t(xdrs, & ul);
		savenode.var = new DULongGDL(ul);
		break;
	  }
	  case GDL_STRING:
	  {
		char* c = 0;
		xdr_string(xdrs, &c, 4096);
		savenode.var = new DStringGDL(std::string(c));
		break;
	  }
	  default:
		break;
	  }
	}
//	  std::cerr<<"["<<savenode.nodeType<< ":";
//	  if (savenode.var!=NULL) std::cerr<<savenode.var->NBytes();
//	  std::cerr<<"]"<<savenode.Text<<"("<<savenode.ligne<<")"<<std::hex<<savenode.node<<"->"<<savenode.right<<" !"<<savenode.down<<std::dec<<std::endl;
	nodes.push_back(savenode);
	return true;

  } 

  void rd_tree(EnvT* e, XDR* xdrs) {
	std::vector<savenodestruct> nodes;
	bool ok;
	do {
	  ok = rd_readNode(e, xdrs, nodes);
	}	while (ok) ;

	//make RefDNode vector
	std::vector<RefDNode> vrefdnodes;
	for (auto i = 0; i < nodes.size(); ++i) {
	  RefDNode rdnode = RefDNode(new DNode);
	  rdnode.get()->initialize(nodes[i].nodeType, nodes[i].Text);
	  rdnode.get()->ResetCData(nodes[i].var);
	  rdnode.get()->SetLine(nodes[i].ligne);
	  rdnode.get()->SetCompileOpt(nodes[i].flags);
	  vrefdnodes.push_back(rdnode);
	}

	//update dnodes links:
	for (auto i = 0; i < nodes.size(); ++i) {
	  //The following warnings should not happen.
	  if (nodes[i].down > nodes.size() + 1) {
		std::cerr << "bad down link @" << nodes[i].ligne << " in " << nodes[i].Text << " for " << nodes[3].Text << std::endl;
		nodes[i].down = 0;
	  }
	  if (nodes[i].right > nodes.size() + 1) {
		std::cerr << "bad right link @" << nodes[i].ligne << " in " << nodes[i].Text << " for " << nodes[3].Text << std::endl;
		nodes[i].right = 0;
	  }
	  if (nodes[i].down != 0) vrefdnodes[i].get()->setFirstChild(vrefdnodes[nodes[i].down - 1].get());
	  if (nodes[i].right != 0) vrefdnodes[i].get()->setNextSibling(vrefdnodes[nodes[i].right - 1].get());
	}
	DNode* root = vrefdnodes[0].get();
	GDLInterpreter::CompileSaveFile(root);
  } 
  
long int xdr_get_gdl_pos(XDR *x){
  long int where=ftell(save_fid);
//  std::cerr<<"getpos: "<<where<<std::endl;
  return where;
}
bool_t xdr_set_gdl_pos(XDR *x, long int y){
  int ret=fseek(save_fid,y,SEEK_SET);
//  std::cerr<<"setpos returned "<<ret<<std::endl;
  return ret;
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
    int32_t UnknownLong;
    for (int i = 0; i < 256; ++i) if (!xdr_int32_t(xdrs, &UnknownLong)) break;
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
  
  //we write only "normal" (i.e. no PROMOTE64) headers
  inline u_int64_t writeNewRecordHeader(XDR *xdrs, int code){
    int32_t rectype=code;    
    xdr_int32_t(xdrs, &rectype); //-16
    u_int32_t ptrs0=0;
    u_int32_t ptrs1=0;
    xdr_uint32_t(xdrs, &ptrs0); //-12 //void, to be updated
    xdr_uint32_t(xdrs, &ptrs1); //-8
    int32_t UnknownLong=0;
    xdr_int32_t(xdrs, &UnknownLong);
    return xdr_get_gdl_pos(xdrs); //end of header
  }

  union U
  {
	u_int32_t trentedeux[2];
	u_int64_t soixantequatre;
  };

  inline u_int64_t updateNewRecordHeader(XDR *xdrs, u_int64_t cur) {
    u_int64_t next = xdr_get_gdl_pos(xdrs);
    //dirty trick for compression: write uncompressed, rewind, read what was just written, compress, write over, reset positions.
    if (save_compress)
    {
      u_int64_t uLength = next - cur;
      uLong cLength = compressBound(uLength);
      char* uncompressed = (char*) calloc(uLength+1,1);
      xdr_set_gdl_pos(xdrs, cur);
      size_t retval = fread(uncompressed, 1, uLength, save_fid);
      if (retval!=uLength) cerr<<"(compress) read error:"<<retval<<"eof:"<<feof(save_fid)<<", error:"<<ferror(save_fid)<<endl;
      char* compressed = (char*) calloc(cLength + 1,1);
      // Deflate
      compress2((Bytef *) compressed, &cLength, (Bytef *) uncompressed, uLength, Z_BEST_SPEED);
      //cLength is the good length now.
      xdr_set_gdl_pos(xdrs, cur);
      xdr_opaque(xdrs,compressed,cLength);
      next = cur+cLength;
      xdr_set_gdl_pos(xdrs, next);
      //if (next!=(cur+cLength)) cerr<<"problem:"<<cur+cLength<<":"<<next<<"\n";
    }
    xdr_set_gdl_pos(xdrs, cur-12); //ptrs0
	//we write a sort of BigEndian format
	  U u;
	  u.soixantequatre=next;
	  if (BigEndian()) { //to be tested on BigEndian machines: may be some other logic at work?.
		xdr_uint32_t(xdrs, &(u.trentedeux[1])); // least significative bits at end ( < 4 GB)
		xdr_uint32_t(xdrs, &(u.trentedeux[0])); // for adresses > 4GB
	  } else {
		xdr_uint32_t(xdrs, &(u.trentedeux[0])); //e.g. intel, little_endian 1st
		xdr_uint32_t(xdrs, &(u.trentedeux[1])); // this would be for adresses > 4GB
	  }
    xdr_set_gdl_pos(xdrs, next);
    return next;
  }  
  
  u_int64_t writeTimeUserHost(XDR *xdrs, char* FileDatestring, char* FileUser, char* FileHost) {
    u_int64_t cur=writeNewRecordHeader(xdrs, TIMESTAMP);
    int32_t UnknownLong=0;
    for (int i = 0; i < 256; ++i) if (!xdr_int32_t(xdrs, &UnknownLong)) cerr << "write error" << endl;
    if (!xdr_string(xdrs, &FileDatestring, strlen(FileDatestring))) cerr << "write error" << endl;
    if (!xdr_string(xdrs, &FileUser, strlen(FileUser))) cerr << "write error" << endl;
    if (!xdr_string(xdrs, &FileHost, strlen(FileHost))) cerr << "write error" << endl;
    u_int64_t next=updateNewRecordHeader(xdrs, cur);
    return next;
  }
  
  u_int64_t writeEnd(XDR *xdrs) {
    u_int64_t cur=writeNewRecordHeader(xdrs, END_MARKER);
    return cur;
  }
  
  u_int64_t writeGDLEnd(XDR *xdrs) {
    u_int64_t cur=writeNewRecordHeader(xdrs, GDL_END_MARKER);
    return cur;
  }  //user-defined Sub

  u_int64_t writeDSubUD(XDR *xdrs, DSubUD* p, bool isPro) {
	u_int64_t cur = writeNewRecordHeader(xdrs, GDL_COMPILED);
	//write file name
	std::string name = p->ObjectName();
	u_int len = name.size();
	//PRO/FUN name 
	const char* cname = name.c_str();
	xdr_string(xdrs, (char**) &cname, len);
	// FUN (0) or PRO (1)
	int32_t proorfun=isPro;
	xdr_int32_t(xdrs, & proorfun);
	//the SemiCompiledCode
	SCCStructV* topP=p->GetSCC();
	SCCStructV top=*topP;
    for (SCCStructV::iterator i = top.begin() ; i != top.end(); ++i) {
	  wr_writeNode((*i), xdrs);
	}
	//finish by ENDOFLIST address
	xdr_uint64_t(xdrs, &ENDOFLIST); //ENDOFLIST
	return updateNewRecordHeader(xdrs, cur);
  }

  //user-defined Sub

  int getDSubUD(EnvT*e, XDR *xdrs, bool verbose, bool skipIfExist) {
	char* dsubUD_name = 0;
	//get pro name
	if (!xdr_string(xdrs, &dsubUD_name, 4096)) return 0;
	std::string name(dsubUD_name);
//	std::cerr<<"getting "<<name<<std::endl;
	// PRO or FUN?
	int32_t isPro=0;
	xdr_int32_t(xdrs, &isPro);
	//check if present
	bool already_present=false;
	DSubUD * present=NULL;
	if (isPro == 1) {
	  for (ProListT::iterator i = proList.begin(); i != proList.end(); ++i) {
		if ((*i)->ObjectName() == name) {
		  already_present = true;
		  present = (*i);
		  break;
		}
	  }
	  //if existing and skipIfExist set, just ignore:
	  if (skipIfExist && already_present) return 1;
	  //check if active
	  EnvStackT& cS = GDLInterpreter::CallStack();
	  SizeT stSz = cS.size();
	  for (SizeT i = 1; i < stSz; ++i) // i=1: skip $MAIN$
	  {
		if (cS[ i]->GetPro() == present) {
		  Warning("Procedure " + name + " can't be restored while active.");
		  return 1;
		}
	  }
	  rd_tree(e, xdrs);

	  if (verbose) Message("RESTORE: Restored procedure: " + name +".");
	  return 1;
	} else if (isPro == 0) {
	  for (FunListT::iterator i = funList.begin(); i != funList.end(); ++i) {
		if ((*i)->ObjectName() == name) {
		  already_present = true;
		  present = (*i);
		  break;
		}
	  }
	  //if existing and skipIfExist set, just ignore:
	  if (skipIfExist && already_present)  return 1;
	  //check if active
	  EnvStackT& cS = GDLInterpreter::CallStack();
	  SizeT stSz = cS.size();
	  for (SizeT i = 1; i < stSz; ++i) // i=1: skip $MAIN$
	  {
		if (cS[ i]->GetPro() == present)
		{
		  Warning("Function "+name+" can't be restored while active.");
		  return 1;
		}
	  }
	  rd_tree(e,xdrs);

	if (verbose) Message("RESTORE: Restored function: " + name+".");
	  return 1;
	} else Message("Error in SAVE file, please report.");
	return 0;
  }
  
  int getVersion(XDR* xdrs) {
    if (!xdr_int32_t(xdrs, &format)) return 0;
    //    cerr << "Format: " << format << endl;
    free(arch); arch = 0;
    if (!xdr_string(xdrs, &arch, 2048)) return 0;
    //    cerr << arch << endl;
    free(os); os = 0;
    if (!xdr_string(xdrs, &os, 2048)) return 0;
    //    cerr << os << endl;
    free(release); release = 0;
    if (!xdr_string(xdrs, &release, 2048)) return 0;
    //    cerr << release << endl;
    return 1;
  }
  
  u_int64_t writeVersion(XDR* xdrs, int32_t *format, char* arch, char* os , char* release) {
    u_int64_t cur=writeNewRecordHeader(xdrs, VERSION_MARKER);
    xdr_int32_t(xdrs, format);
    xdr_string(xdrs, &arch, strlen(arch));
    xdr_string(xdrs, &os, strlen(os));
    xdr_string(xdrs, &release, strlen(release));
    u_int64_t next=updateNewRecordHeader(xdrs, cur);
    return next;
  }
  
  int getNotice(XDR* xdrs) {
    free(notice); notice = 0;
    if (!xdr_string(xdrs, &notice, 20480)) return 0;
//    cerr << notice << endl;
    return 1;
  }
  
  u_int64_t writeNotice(XDR* xdrs, char* notice) {
    u_int64_t cur=writeNewRecordHeader(xdrs, NOTICE);
    xdr_string(xdrs, &notice, strlen(notice));
    u_int64_t next=updateNewRecordHeader(xdrs, cur);
    return next;
  }

  char* getDescription(XDR *xdrs) {
    int32_t length = 0;
    if (!xdr_int32_t(xdrs, &length)) cerr << "error reading description string length" << endl;
    if (length > 0)
    {
      char* chars = 0;
      if (!xdr_string(xdrs, &chars, length)) cerr << "error getting string" << endl;
      return chars;
    } else return NULL;
  }

  u_int64_t writeDescription(XDR *xdrs, char* descr) {
    u_int64_t cur=writeNewRecordHeader(xdrs, DESCRIPTION_MARKER);
    int32_t length = strlen(descr);
    if (!xdr_int32_t(xdrs, &length)) cerr << "error writing description string length" << endl;
    if (!xdr_string(xdrs, &descr, length)) cerr << "error writing string" << endl;
    u_int64_t next=updateNewRecordHeader(xdrs, cur);
    return next;
   }

  int getIdentification(XDR *xdrs) {
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
  
  u_int64_t writeIdentification(XDR *xdrs, char *saveFileAuthor, char* title, char* otherinfo ) {
    u_int64_t cur=writeNewRecordHeader(xdrs, IDENTIFICATION);
    xdr_string(xdrs, &saveFileAuthor, strlen(saveFileAuthor));
    xdr_string(xdrs, &title, strlen(title) );
    xdr_string(xdrs, &otherinfo, strlen(otherinfo) );
    u_int64_t next=updateNewRecordHeader(xdrs, cur);
    return next;
  }
  
  dimension* getArrDesc64(XDR* xdrs) {
    if (DEBUG_SAVERESTORE) cerr << "get 64-Bits Array descriptor ," ;
    int64_t UnknownLong;
    if (!xdr_int64_t(xdrs, &UnknownLong)) return NULL;
    int64_t nbytes;
    if (!xdr_int64_t(xdrs, &nbytes)) return NULL;
    int64_t nEl;
    if (!xdr_int64_t(xdrs, &nEl)) return NULL;
    int32_t nDims;
    if (!xdr_int32_t(xdrs, &nDims)) return NULL; //on 32 bits
    if (!xdr_int64_t(xdrs, &UnknownLong)) return NULL; //ignored as we ignore the 2x32 bits integers in the other version.
    if (DEBUG_SAVERESTORE) cerr << "nbytes:" << nbytes << " ,nEl:" << nEl << ", nDims:" << nDims<<" ";
    int64_t dims[8];
    if (!xdr_vector(xdrs, (char*) dims, 8, sizeof (int64_t), (xdrproc_t) xdr_int64_t)) return NULL;
    SizeT k = dims[0];
    dimension* theDim = new dimension(k);
    for (int i = 1; i < 8; ++i)
    {
      k = dims[i];
      *theDim << k;
    }
    theDim->Purge();
    if (DEBUG_SAVERESTORE) cerr<<*theDim<<endl;
    return theDim;
  }

  dimension* getArrDesc(XDR* xdrs) {
    if (DEBUG_SAVERESTORE) cerr << "get standard 32-Bits Array descriptor ," ;
    int32_t arrstart;
    int32_t UnknownLong;
    if (!xdr_int32_t(xdrs, &arrstart)) return NULL;
    if (arrstart != ARRAYSTART && arrstart != ARRAYSTART64) //'10'o and '22'o
    {
      cerr << "array is not a array! abort." << endl;
      return 0;
    }
    if (arrstart == ARRAYSTART64) return getArrDesc64(xdrs); //as the rest is specially coded on 8 bytes.
    
    if (!xdr_int32_t(xdrs, &UnknownLong)) return NULL;
    int32_t nbytes;
    if (!xdr_int32_t(xdrs, &nbytes)) return NULL;
    int32_t nEl;
    if (!xdr_int32_t(xdrs, &nEl)) return NULL;
    int32_t nDims;
    if (!xdr_int32_t(xdrs, &nDims)) return NULL;
    if (!xdr_int32_t(xdrs, &UnknownLong)) return NULL;
    if (!xdr_int32_t(xdrs, &UnknownLong)) return NULL;
    int32_t nmax;
    if (!xdr_int32_t(xdrs, &nmax)) return NULL;
    if (DEBUG_SAVERESTORE) cerr << "nbytes:" << nbytes << " ,nEl:" << nEl << ", nDims:" << nDims<<" ";
    int32_t dims[nmax];
    if (!xdr_vector(xdrs, (char*) dims, nmax, sizeof (int32_t), (xdrproc_t) xdr_int32_t)) return NULL;
    SizeT k = dims[0];
    dimension* theDim = new dimension(k);
    for (int i = 1; i < nmax; ++i)
    {
      k = dims[i];
      *theDim << k;
    }
    theDim->Purge();
    if (DEBUG_SAVERESTORE)    cerr<<*theDim<<endl;
    return theDim;
  }

  void writeArrDesc64(XDR* xdrs, BaseGDL* var) {
    int32_t arrstart=ARRAYSTART64;
    xdr_int32_t(xdrs, &arrstart);
    //very important:
    int64_t typeLength=sizeOfType[var->Type()];
    if (var->Type()==GDL_STRING) typeLength=(var->NBytes()/var->N_Elements())-1;
    if (typeLength==0) typeLength=(var->NBytes());
    xdr_int64_t(xdrs, &typeLength);
    int64_t nbytes=var->NBytes();
    xdr_int64_t(xdrs, &nbytes);
    int64_t nEl=var->N_Elements();
    xdr_int64_t(xdrs, &nEl);
    int32_t nDims=var->Rank();
    if (nDims==0 && var->Type()==GDL_STRUCT ) nDims=1;
    xdr_int32_t(xdrs, &nDims);
    int32_t UnknownLong=0;
    xdr_int32_t(xdrs, &UnknownLong);
    xdr_int32_t(xdrs, &UnknownLong);
    int64_t nmax=8;
    int64_t dims[nmax];
    for (int i=0; i < nmax; ++i) dims[i]=1; //yes.
    // not written xdr_int32_t(xdrs, &nmax);
    for (int i=0; i < nDims; ++i) if (var->Dim(i) > 0) dims[i]=var->Dim(i);
    if (DEBUG_SAVERESTORE) std::cerr<<var->Dim()<<std::endl;
    xdr_vector(xdrs, (char*) dims, nmax, sizeof (int64_t), (xdrproc_t) xdr_int64_t);
  }

  void writeArrDesc32(XDR* xdrs, BaseGDL* var) {
    int32_t arrstart=ARRAYSTART;
    xdr_int32_t(xdrs, &arrstart);
    //very important:
    int32_t typeLength=sizeOfType[var->Type()];
    if (var->Type()==GDL_STRING) typeLength=(var->NBytes()/var->N_Elements())-1;
    if (typeLength==0) typeLength=(var->NBytes());
    xdr_int32_t(xdrs, &typeLength);
    int32_t nbytes=var->NBytes();
    xdr_int32_t(xdrs, &nbytes);
    int32_t nEl=var->N_Elements();
    xdr_int32_t(xdrs, &nEl);
    int32_t nDims=var->Rank();
    if (nDims==0 && var->Type()==GDL_STRUCT ) nDims=1;
    xdr_int32_t(xdrs, &nDims);
    int32_t UnknownLong=0;
    xdr_int32_t(xdrs, &UnknownLong);
    xdr_int32_t(xdrs, &UnknownLong);
    int32_t nmax=8;
    int32_t dims[nmax];
    for (int i=0; i < nmax; ++i) dims[i]=1; //yes.
    xdr_int32_t(xdrs, &nmax);
    for (int i=0; i < nDims; ++i) if (var->Dim(i) > 0) dims[i]=var->Dim(i);
    if (DEBUG_SAVERESTORE) std::cerr<<var->Dim()<<std::endl;
    xdr_vector(xdrs, (char*) dims, nmax, sizeof (int32_t), (xdrproc_t) xdr_int32_t);
  }
  
  void writeArrDesc(XDR* xdrs, BaseGDL* var) {
    //very important check total size and switch if size is >2GO. 
    //Warning do not use a SizeT as typelength as this does not like negative values returned by for 0 length strings
    DLong64 typeLength=sizeOfType[var->Type()];
    if (var->Type()==GDL_STRING) typeLength=(var->NBytes()/var->N_Elements()); 
    SizeT nEl=var->N_Elements();
    if (nEl*typeLength > 2000000000ULL) writeArrDesc64(xdrs,var); else writeArrDesc32(xdrs,var);
  }
  
  int defineCommonBlock(EnvT* e, XDR* xdrs, int verboselevel) {
    int32_t ncommonvars;
    if (!xdr_int32_t(xdrs, &ncommonvars)) return 0;
    char* commonname = 0;
    if (!xdr_string(xdrs, &commonname, 2048)) return 0;
    char* varnames[ncommonvars];
    for (int i = 0; i < ncommonvars; ++i) varnames[i] = 0;
    for (int i = 0; i < ncommonvars; ++i) if (!xdr_string(xdrs, &varnames[i], 2048)) return 0;

    if (verboselevel>1){
      cerr << "Common " << commonname << " consists of the following variables:" << endl;
      for (int i = 0; i < ncommonvars; ++i) cerr << varnames[i] << ",";
      cerr << endl;
    }
    //populate common block

    //behaviour is: if one of these variables already exist, the whole common is not defined, and
    //the common variables become just normal variables:

    EnvStackT& callStack = e->Interpreter()->CallStack();
    int32_t curlevnum = callStack.size();
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
      if (verboselevel>0) Message("Restored common block: " + commonName);
    }
    commonName.clear();
    DCommonBase* currentcommon = pro->Common(std::string(commonname));
    for (int i = 0; i < ncommonvars; ++i)
    {
      std::string varName = varnames[i];
      currentcommon->AddVar(varName);
      varName.clear();
    }

    return 1;
  }
  
  DStructGDL* getDStruct(EnvT* e, XDR* xdrs, dimension* inputdims, bool &isObjStruct) {
    isObjStruct=false;
    int32_t structstart;
    if (!xdr_int32_t(xdrs, &structstart)) return NULL;
    if (structstart != STRUCTSTART)
    {
      cerr << "structure is not a structure! abort." << endl;
      return NULL;
    }
    char* structname = 0;
    if (!xdr_string(xdrs, &structname, 2048)) return NULL;
    int32_t structure_def_flags;
    if (!xdr_int32_t(xdrs, &structure_def_flags)) return NULL;
    bool ispredef = false; //ispredef means that this struct has been already defined inside the save file,
    // but its definition may clash with an existing definition already made in GDL previous to the RESTORE command.
    if (structure_def_flags & 0x01)
    {
      ispredef = true;
    }
    bool unknown_flag = false;
    bool is_super = false;
    bool inherits = false;
    if (structure_def_flags & 0x08)
    {
      unknown_flag = true;
    }
    if (structure_def_flags & 0x02)
    {
      inherits = true;
    }
    if (structure_def_flags & 0x04)
    {
      is_super = true;
    }
    
    if (is_super || inherits) isObjStruct=true;
    if (DEBUG_SAVERESTORE) {
     if (isObjStruct) {
       cerr<<std::hex<<structure_def_flags<<std::dec<<std::endl;
       cerr << "Object name:\"" << structname << "\"";
       if (inherits) cerr << ", inherits";
       if (is_super) cerr << ", is a Superclass";
     } else {
       cerr << "Structure name:\"" << structname << "\"";
     }
     if (unknown_flag) cerr << ", is tagged by unknown flag 0x08";
     if (ispredef) cerr << ", is already defined";
     cerr<<endl;
    }

    int32_t ntags;
    if (!xdr_int32_t(xdrs, &ntags)) return NULL;
    int32_t struct_nbytes;
    if (!xdr_int32_t(xdrs, &struct_nbytes)) return NULL;
    //if predef == 1 this ends the Struct_desc, meaning that the definition of such a
    //structure has already been presented and we should reuse it.
    // otherwise, we define the structure using the following entries:
    if (ispredef) {
      std::string name = std::string(structname);
      //beautify
      assert(name != "$truct") ; // named struct
        name = StrUpCase(name);
      if( name == "IDL_OBJECT") name = GDL_OBJECT_NAME; // replacement also done in GDLParser
      if( name == "IDL_CONTAINER") name = GDL_CONTAINER_NAME; // replacement also done in GDLParser
      DStructDesc* desc = e->Interpreter()->GetStruct(name, e->CallingNode()); //will throw if does not exist.
      return new DStructGDL(desc, *inputdims);

    } else {

      //definition of a new structure or class. Structure, if named, may be different from already existing (prior to RESTORE): clash!
      //this is why the "RELAXED" option of RESTORE exists.
      // Besides, if we create an Object, (a named structure) it is necessary to check wether it can be defined using XXXX__define.pro
      // and if this definition is in accordance with the structure defined here, i.e., in the save file.
      //TAG_DESC repated ntags times:
      int32_t tag_typecode[ntags];
      char* tag_name[ntags];
      int32_t tag_flag[ntags];
      int32_t tag_offset[ntags];
      for (int i = 0; i < ntags; ++i) tag_name[i] = 0;
      for (int i = 0; i < ntags; ++i)
      {
        //TAG_DESC:
        if (!xdr_int32_t(xdrs, &tag_offset[i])) break;
        if (!xdr_int32_t(xdrs, &tag_typecode[i])) break;
        if (!xdr_int32_t(xdrs, &tag_flag[i])) break;
      }
      for (int i = 0; i < ntags; ++i)
      {
        //TAGNAMES x NTAGS:
        if (!xdr_string(xdrs, &tag_name[i], 2048)) break;
      }

      int32_t narrays = 0;
      int32_t nstructs = 0;
      for (int i = 0; i < ntags; ++i) if (tag_flag[i] & 0x20) nstructs++;
      for (int i = 0; i < ntags; ++i) if (tag_flag[i] & 0x04) narrays++;
      dimension * tagdimensions[narrays + 1]; //Always >0: FIXME IF MEMORY LEAK!
      //if there are any tag flags indicating the tag is an array, read the ARRDESC
      for (int i = 0; i < narrays; ++i)
      {
        tagdimensions[i] = getArrDesc(xdrs);
        if (tagdimensions[i] == NULL) return NULL;
      }

      std::string stru_name = std::string(structname);
      if( stru_name == "IDL_OBJECT") stru_name = GDL_OBJECT_NAME; // replacement also done in GDLParser
      if( stru_name == "IDL_CONTAINER") stru_name = GDL_CONTAINER_NAME; // replacement also done in GDLParser
      DStructDesc* stru_desc=NULL;
      DStructDesc* ref_desc=NULL;
      bool checkStruct=false;
      //take care of named structures. Here is also where one should try to create an object, since, if it exists, its structure must be
      //compared with the current one.
      if (stru_name.length() > 0 && stru_name[0] != '$')
      {
        if (isObjStruct) { //create a dummy object of this name, remove it.
          DString objName=StrUpCase(stru_name);
          DStructDesc* objDesc;
          try {
            objDesc = e->Interpreter()->GetStruct(objName, e->CallingNode());
            DStructGDL* objStruct = new DStructGDL(objDesc, dimension(1));
            DObj objID = e->NewObjHeap(1, objStruct); // owns objStruct
            DObjGDL* newObj = new DObjGDL(objID); // the object
            //calling the INIT function seems to intrusive.
//            try {
//              // call INIT function
//              DFun* objINIT = objDesc->GetFun("INIT");
//              if (objINIT != NULL) {
//                StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());
//
//                // morph to obj environment and push it onto the stack again
//                e->PushNewEnvUD(objINIT, 1, &newObj);
//
//                BaseGDL* res = e->Interpreter()->call_fun(objINIT->GetTree());
//                GDLDelete(res);
//              }
//            } catch (...) {
//            }
            e->FreeObjHeap(objID); // delete objStruct
            GDLDelete(newObj);
          } catch (...) {
            if (DEBUG_SAVERESTORE) std::cerr << stru_name<<": is NOT a known Object."<<endl;
          }
        }
        stru_desc = FindInStructList(structList, stru_name);

        if (stru_desc == NULL)
        {
          stru_desc = new DStructDesc(stru_name);
          structList.push_back(stru_desc);
        } else {
          if (DEBUG_SAVERESTORE) cerr<<stru_name<<": is a known Object."<<endl;
          checkStruct=true;
          ref_desc=stru_desc;
          stru_desc=new DStructDesc("$truct"); //make it anonymous, test if equality at end!
        }
      } else stru_desc=new DStructDesc("$truct");
      //summary & tag population:
      for (int i = 0, j = 0, k = 0; i < ntags; ++i)
      {
        //reserved a dimension
        dimension pardim = dimension();

        if (tag_flag[i] & 0x04)
        {
          // modify pardim and push index;
          pardim = *(tagdimensions[j++]); //memory leak?
        }

        switch (tag_typecode[i]) {
          case GDL_BYTE: //	Byte
          {
            SpDByte entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case GDL_INT: //	16-bit Integer 
          {
            SpDInt entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case GDL_LONG: //	32-bit Long Integer 
          {
            SpDLong entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case GDL_FLOAT: //	32-bit Floating Point Number 
          {
            SpDFloat entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case GDL_DOUBLE: //	64-bit Floating Point Number 
          {
            SpDDouble entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case GDL_COMPLEX: //	Complex Floating Point Number (32-bits each) 
          {
            SpDComplex entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;
          case GDL_STRING: //	String
          {
            SpDString entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;
          case GDL_STRUCT: //	Structure (never a scalar)
          {
            bool dummy;
            DStructGDL* parStruct = getDStruct(e, xdrs, &pardim, dummy);
            if (parStruct == NULL) return NULL;
            stru_desc->AddTag(tag_name[i], parStruct);
          }
            break;

          case GDL_COMPLEXDBL: //	Complex Floating Point Number (64-bits each) 
          {
            SpDComplexDbl entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case GDL_PTR: //	Heap Pointer 
          {
            DPtrGDL entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;
          case GDL_OBJ: //	Object Reference (not supported by CMSVLIB)
          {
            DObjGDL entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;
          case GDL_UINT: //	16-bit Unsigned Integer 
          {
            SpDUInt entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case GDL_ULONG: //	32-bit Unsigned Integer 
          {
            SpDULong entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case GDL_LONG64: //	64-bit Integer 
          {
            SpDLong64 entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          case GDL_ULONG64: //	64-bit Unsigned Integer 
          {
            SpDULong64 entry(pardim);
            stru_desc->AddTag(tag_name[i], &entry);
          }
            break;

          default: //	0 ? Undefined (not allowed) 
            if (DEBUG_SAVERESTORE) cerr << "Should not happen: undefined typecode tag in getDStruct()." << endl;
            break;
        }



      }

      //Then there should be CLASSNAME if INHERITS or IS_SUPER 
      if (isObjStruct || is_super)
      {
        char* classname = 0;
        if (!xdr_string(xdrs, &classname, 2048)) return NULL;
        if (DEBUG_SAVERESTORE) cerr << "CLASSNAME: \"" << classname << "\"" << endl;
        //NSUPCLASSES:
        int32_t nsupclasses = 0;
        if (!xdr_int32_t(xdrs, &nsupclasses)) return NULL;
        if (DEBUG_SAVERESTORE)  cerr << "NSUPCLASSES=" << nsupclasses << endl;
        if (nsupclasses > 0)
        {
          for (int i = 0; i < nsupclasses; ++i)
          {
            char* supclassname = 0;
            if (!xdr_string(xdrs, &supclassname, 2048)) return NULL;
            if (DEBUG_SAVERESTORE) cerr << "SUPCLASSNAME " << i << ": " << supclassname << endl;
          }
          for (int i = 0; i < nsupclasses; ++i)
          {
            //define all parent classes in objheap.
            DStructGDL* superclass = NULL;
            bool dummy=false;
            superclass = getDStruct(e, xdrs, new dimension(1), dummy); // will define the class as an object.
            if (superclass) stru_desc->AddParentListOnly(superclass->Desc());
//            if (isObjStruct)  {
//             DPtr ptr= e->NewObjHeap(1, static_cast<DStructGDL*>(superclass));
//            }
          }
        }
      }
      if (checkStruct)
      {
        try
        {
          ref_desc->AssureIdentical(stru_desc);
          stru_desc=ref_desc; //OK, switch back.
        }        catch (GDLException& ex)
        {
          e->Throw("Structure not restored due to conflict with existing definition: "+stru_name);
          return NULL;
        }
      }
      return new DStructGDL(stru_desc, *inputdims);
    }
  }
  
  void writeStructDesc(XDR* xdrs, DStructGDL* var, bool isObject=false, bool is_super=false) {
    DStructDesc* str=var->Desc();
    int32_t structstart=STRUCTSTART;
    xdr_int32_t(xdrs, &structstart);
    const char* structname = str->Name().c_str();
    if (DEBUG_SAVERESTORE && isObject) cerr << "Writing Object \""<< str->Name() <<"\" @"<< var << endl;
    if (DEBUG_SAVERESTORE && !isObject) cerr << "Writing Structure \""<< str->Name() <<"\" @"<< var << endl;
    //predef: struct name is known
    bool ispredef = (str->Name() != "$truct"); // named struct
    //void name if anonymous struct!
    if (!ispredef) {
      std::string nullstr=""; char* voidchar=(char*)nullstr.c_str();
      xdr_string(xdrs,(char**)&voidchar, 0);
      }
    else xdr_string(xdrs, (char**) &structname, str->Name().size());
    //now, did we already define this named structure in the file?
    //if no, add name to list of defined (for further use) and remove is_predef (to write it).
    if (ispredef) {
      bool found=false;
      for (SizeT i=0; i<predeflist.size(); ++i)
      {
        if (predeflist[i]==str->Name()) {found=true; break;}
      }
      if (!found) {
        predeflist.push_back(str->Name());
        ispredef=false;
      }
    }
    //flags.

    int32_t structure_def_flags=0x28; //0;
    if (ispredef) structure_def_flags |= 0x01;
    if (isObject) structure_def_flags |= 0x08; //it is a CLASS
    if (isObject) structure_def_flags |= 0x02;
    if (is_super) structure_def_flags |= 0x04;
    xdr_int32_t(xdrs, &structure_def_flags);
    int32_t ntags=str->NTags();
    xdr_int32_t(xdrs, &ntags);
    int32_t struct_nbytes=0; //(ispredef)?str->NBytes():0;
    xdr_int32_t(xdrs, &struct_nbytes);
    //if predef == 1  this ends the Struct_desc, meaning that we have already presented this structure.
    if (ispredef) return;
    //TAG_DESC repated ntags times:
    int32_t tag_typecode[ntags];
    char* tag_name[ntags];
    int32_t tag_flag[ntags];
    int32_t tag_offset[ntags];
    int32_t byteoff=0;
    for (int i = 0; i < ntags; ++i) tag_name[i] = (char*) str->TagName(i).c_str();
    for (SizeT i = 0; i < ntags; ++i) { tag_offset[i] = byteoff; byteoff+=var->GetTag(i)->NBytes();} //probably OK
    for (int i = 0; i < ntags; ++i)
    {
      tag_flag[i]=0;
      if (var->GetTag(i,0)->Rank()> 0) tag_flag[i]|=0x04;
      switch (var->GetTag(i,0)->Type()) {
        case GDL_STRUCT: 
          tag_flag[i] |=0x20;
        default: 
          tag_typecode[i] = var->GetTag(i,0)->Type();
      }
    }
    for (int i = 0; i < ntags; ++i)
    {
      //TAG_DESC:
      xdr_int32_t(xdrs, &tag_offset[i]);
      xdr_int32_t(xdrs, &tag_typecode[i]);
      xdr_int32_t(xdrs, &tag_flag[i]);
    }
    for (int i = 0; i < ntags; ++i)
    {
      //TAGNAMES x NTAGS:
      xdr_string(xdrs, &tag_name[i], strlen(tag_name[i]));
    }
    //ARRDESC x NARRAYS:
    for (int i = 0; i < ntags; ++i)
    {
      if (tag_flag[i] & 0x04) writeArrDesc(xdrs, var->GetTag(i));
    }
    //STRUCTDESC x NARRAYS:
    for (int i = 0; i < ntags; ++i)
    {
      if (tag_flag[i] & 0x20) writeStructDesc(xdrs, static_cast<DStructGDL*>(var->GetTag(i)));
    }
    //TBD: CLASSES
    if (isObject) {
      xdr_string(xdrs, (char**) &structname, str->Name().size()); //CLASSNAME
      if (DEBUG_SAVERESTORE) std::cerr << "Object CLASS is \""<< str->Name() << std::endl;
      std::vector< std::string> pNames;
      str->GetParentNames(pNames);
//TBD: get super classes and write structs accordingly. 
      int32_t nsupclasses=pNames.size(); //do they exist???
      for (int i=0 ; i< nsupclasses;++i) {
          DStructGDL* parent = new DStructGDL( pNames[i]);
          if (parent == NULL) nsupclasses--;
          GDLDelete(parent);
        }      
      xdr_int32_t(xdrs, &nsupclasses);
      if (nsupclasses > 0) {
        DStructGDL* parents[nsupclasses];
        // AC not OK on OSX12: std::string pnames[nsupclasses];
	string *pnames = new string[nsupclasses];
        int k=0;
        for (int i=0 ; i< pNames.size();++i) {
          DStructGDL* parent = new DStructGDL( pNames[i]);
          if (parent != NULL) {
            pnames[k]=pNames[i];
            parents[k]=parent;
            if (DEBUG_SAVERESTORE) std::cerr << "     SUPCLASS:  \""<< pNames[k] << std::endl;
            k++;
          }
        }
        for (int i=0 ; i< nsupclasses;++i) {char* str=(char*)(pnames[i].c_str()); xdr_string(xdrs, &str,pnames[i].size());}
        for (int i=0 ; i< nsupclasses;++i) {
            DStructGDL* parent = parents[i];
            Guard<DStructGDL> parent_guard(parent);
            writeStructDesc(xdrs, parent, true, true);
          }
	delete [] pnames;       
	}
    }
  }
  
  
  BaseGDL* getVariable(EnvT* e, XDR* xdrs, int &isSysVar, bool &isObjStruct) {
    bool isStructure = false;
    bool isArray = false;
    // start of TYPEDESC
    // common for VARIABLE, SYSTEM_VARIABLE and HEAP_DATA:
    // 1) TYPECODE
    int32_t typecode;
    if (!xdr_int32_t(xdrs, &typecode)) return NULL;
    // 2) VARFLAGS
    int32_t varflags;
    if (!xdr_int32_t(xdrs, &varflags)) return NULL;
	//This is not signaled in C. Marqwardt doc: a system variable (isSysVar=0x02 when function is called) has two supplemental int32, that we skip.
	if (isSysVar & 0x02) {
	  int32_t dummy;
	  if (!xdr_int32_t(xdrs, &dummy)) return NULL;
	  if (!xdr_int32_t(xdrs, &dummy)) return NULL;
	}
	
    if (varflags & 0x40) return NullGDL::GetSingleInstance(); //special !NULL variable, no variable content follows.

    if (varflags & 0x02) //defines a system variable.
    {
      isSysVar |= 0x02;
//           cerr << " system " << endl;
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

    //we gonnna create a BaseGDL:

    BaseGDL* var;
    dimension* dims;
    // if ARRAY or STRUCTURE, Read ARRAY_DESC that follows:
    if (isStructure)
    {//if This was a Structure, it has an ARRAY_DESC plus a STRUCT_DESC 
      dims = getArrDesc(xdrs);
      if (dims == NULL) return NULL;
      var = getDStruct(e, xdrs, dims, isObjStruct);
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
        case GDL_BYTE: //	Byte
          var = new DByteGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_INT: //	16-bit Integer 
          var = new DIntGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_LONG: //	32-bit Long Integer 
          var = new DLongGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_FLOAT: //	32-bit Floating Point Number 
          var = new DFloatGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_DOUBLE: //	64-bit Floating Point Number 
          var = new DDoubleGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_COMPLEX: //	Complex Floating Point Number (32-bits each) 
          var = new DComplexGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_STRING: //	String
          var = new DStringGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_STRUCT: //	Structure (never a scalar)
          cerr << "Should not happen: struct" << endl;
          break;
        case GDL_COMPLEXDBL: //	Complex Floating Point Number (64-bits each) 
          var = new DComplexDblGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_PTR: //	Heap Pointer 
          var = new DPtrGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_OBJ: //	Object Reference (not supported by CMSVLIB) 
          var = new DObjGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_UINT: //	16-bit Unsigned Integer 
          var = new DUIntGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_ULONG: //	32-bit Unsigned Integer 
          var = new DULongGDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_LONG64: //	64-bit Integer 
          var = new DLong64GDL(*dims,BaseGDL::NOZERO);
          break;
        case GDL_ULONG64: //	64-bit Unsigned Integer 
          var = new DULong64GDL(*dims,BaseGDL::NOZERO);
          break;
        default: //	0 ? Undefined (not allowed) 
          var = NullGDL::GetSingleInstance(); //          cerr <<"Should not happen"<<endl;
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
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DInt), (xdrproc_t) xdr_int16_t)) cerr << "error GDL_INT" << endl;
      }
        break;
      case GDL_UINT:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DUInt), (xdrproc_t) xdr_uint16_t)) cerr << "error GDL_UINT" << endl;
      }
        break;
      case GDL_LONG:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (int32_t), (xdrproc_t) xdr_int32_t)) cerr << "error GDL_LONG" << endl;
      }
        break;
      case GDL_ULONG:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DULong), (xdrproc_t) xdr_uint32_t)) cerr << "error GDL_ULONG" << endl;
      }
        break;
      case GDL_LONG64:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DLong64), (xdrproc_t) xdr_int64_t)) cerr << "error GDL_LONG64" << endl;
      }
        break;
      case GDL_ULONG64:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DULong64), (xdrproc_t) xdr_uint64_t)) cerr << "error GDL_ULONG64" << endl;
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
          int32_t length;
          if (!xdr_int32_t(xdrs, &length)) cerr << "error reading string length" << endl;
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
        int32_t heapNumber[nEl];
        DPtrGDL* ptr = static_cast<DPtrGDL*> (var);
        for (SizeT ix = 0; ix < nEl; ++ix) xdr_int32_t(xdrs, &(heapNumber[ix]));
        for (SizeT ix = 0; ix < nEl; ++ix)
        {
          DPtr heapptr = heapIndexMapRestore.find(heapNumber[ix])->second.second;
           (*ptr)[ix] = heapptr;
           GDLInterpreter::IncRef(heapptr);
          if (DEBUG_SAVERESTORE) std::cerr<<"PTR at #"<<heapNumber[ix]<<" restored at "<<heapptr<<std::endl;
        }
        break;
      }
      case GDL_OBJ:
      {
        int32_t heapNumber[nEl];
        DObjGDL* ptr = static_cast<DObjGDL*> (var);
        for (SizeT ix = 0; ix < nEl; ++ix) xdr_int32_t(xdrs, &(heapNumber[ix]));
        for (SizeT ix = 0; ix < nEl; ++ix)
        {
          DObj heapptr = heapIndexMapRestore.find(heapNumber[ix])->second.second;
            (*ptr)[ix] = heapptr;
           GDLInterpreter::IncRefObj(heapptr);
          if (DEBUG_SAVERESTORE) std::cerr<<"OBJ at #"<<heapNumber[ix]<<" restored at "<<heapptr<<std::endl;
        }
        break;
      }
      default: assert(false);
    }
  }
  
  void writeVariableData(XDR* xdrs, BaseGDL* var) {
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
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DInt), (xdrproc_t) xdr_int16_t)) cerr << "error GDL_INT" << endl;
      }
        break;
      case GDL_UINT:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DUInt), (xdrproc_t) xdr_uint16_t)) cerr << "error GDL_UINT" << endl;
      }
        break;
      case GDL_LONG:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (int32_t), (xdrproc_t) xdr_int32_t)) cerr << "error GDL_LONG" << endl;
      }
        break;
      case GDL_ULONG:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DULong), (xdrproc_t) xdr_uint32_t)) cerr << "error GDL_ULONG" << endl;
      }
        break;
      case GDL_LONG64:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DLong64), (xdrproc_t) xdr_int64_t)) cerr << "error GDL_LONG64" << endl;
      }
        break;
      case GDL_ULONG64:
      {
        if (!xdr_vector(xdrs, (char*) var->DataAddr(), nEl, sizeof (DULong64), (xdrproc_t) xdr_uint64_t)) cerr << "error GDL_ULONG64" << endl;
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
          const char *chars=(*static_cast<DStringGDL*> (var))[i].c_str();
          int32_t length=(*static_cast<DStringGDL*> (var))[i].length();
          if (!xdr_int32_t(xdrs, &length)) cerr << "error writing string length" << endl;
          if (length > 0) if (!xdr_string(xdrs, (char**)&chars, length)) cerr << "error writing string" << endl;
        }
      }
        break;
      case GDL_STRUCT:
      {
        DStructGDL* str = static_cast<DStructGDL*> (var);
        SizeT nTags = str->Desc()->NTags();
        for (SizeT ix = 0; ix < nEl; ++ix) for (SizeT t = 0; t < nTags; ++t) writeVariableData(xdrs, str->GetTag(t, ix));
        break;
      }
      case GDL_PTR:  //we must translate PTRs as they may have been scrambled in the writing of the heap indexes.
      {
        u_int32_t heapNumber[nEl];
        for (SizeT i = 0; i < nEl; ++i)
        {
          heapNumber[i]=0;
          DPtr ptr = (*static_cast<DPtrGDL*>(var))[i];
          heapT::iterator it=heapIndexMapSave.find(ptr);
          if (it!=heapIndexMapSave.end()) heapNumber[i]=(*it).second;
        }
        if (!xdr_vector(xdrs, (char*) &heapNumber, nEl, sizeof (int32_t), (xdrproc_t) xdr_uint32_t)) cerr << "error PTR" << endl;
        break;
      }
      case GDL_OBJ:
      {
        u_int32_t heapNumber[nEl];
        for (SizeT i = 0; i < nEl; ++i)
        {
          heapNumber[i]=0;
          DObj ptr = (*static_cast<DObjGDL*>(var))[i];
          heapT::iterator it=heapIndexMapSave.find(ptr);
          if (it!=heapIndexMapSave.end()) heapNumber[i]=(*it).second; 
        }
        if (!xdr_vector(xdrs, (char*) &heapNumber, nEl, sizeof (int32_t), (xdrproc_t) xdr_uint32_t)) cerr << "error OBJ" << endl;
        break;
      }
      default: assert(false);
    }
  }

  void writeVariableHeader(XDR* xdrs, BaseGDL* var, bool isSysVar=false, bool readonly=false, bool isObject=false) {
    if (DEBUG_SAVERESTORE)  std::cerr<<"Writing normal Variable Header, isObject="<<isObject<<std::endl;
    int32_t varflags=0;
    bool isStructure = (var->Type()==GDL_STRUCT);
    bool isArray = (var->Rank() > 0);
    // start of TYPEDESC
    // common for VARIABLE, SYSTEM_VARIABLE and HEAP_DATA:
    // 1) TYPECODE
    int32_t typecode=0;
    // special case for !NULL
    bool nullsize=false;
    if ( var->N_Elements() == 0 ) nullsize=true;
    typecode=var->Type(); //these are the same.
    xdr_int32_t(xdrs, &typecode);
    // 2) VARFLAGS
    if (isSysVar) varflags |= 0x02;
    if (readonly) varflags |= 0x01;
    if (isObject) varflags |= 0x34; if (isStructure) varflags |= 0x24; else if (isArray) varflags |= 0x04;   
    if (nullsize) varflags=0x40; 

    //This is not signaled in C. Marqwardt doc: a system variable has two supplemental int32 as such:
    if (isSysVar) {
	  if (typecode == 8) {
		int32_t system = 54;
		int32_t writeable = 52;
		int32_t notwriteable = 53;
		if (readonly) xdr_int32_t(xdrs, &notwriteable);
		else xdr_int32_t(xdrs, &writeable);
		xdr_int32_t(xdrs, &typecode);
		xdr_int32_t(xdrs, &system); //system
	  } else {
		int32_t noll = 0x0;
		int32_t array = 0x04;
		if (isArray) xdr_int32_t(xdrs, &array);
		else xdr_int32_t(xdrs, &noll);
		xdr_int32_t(xdrs, &typecode);
		xdr_int32_t(xdrs, &varflags);
	  }
	  if (nullsize) return;
	} else {
	  xdr_int32_t(xdrs, &varflags);
	  if (nullsize) return;
	}

    // if ARRAY or STRUCTURE, write ARRAY_DESC that follows:
    if (isStructure||isArray) writeArrDesc(xdrs, var);
    if (isStructure) writeStructDesc(xdrs, static_cast<DStructGDL*>(var), isObject);
  }

  u_int64_t writeNormalVariable(XDR *xdrs, std::string varName, BaseGDL* var, int varflags=0x0) {
    bool isSysVar=false;
    bool readonly=false;
    if (varflags & 0x02) //defines a system variable.
    {
      isSysVar = true;
    }
    if (varflags & 0x01)
    {
      readonly = true;
    }
    const char* varname=varName.c_str();
    u_int64_t cur=writeNewRecordHeader(xdrs, isSysVar?SYSTEM_VARIABLE:VARIABLE);
    xdr_string(xdrs, (char**)&varname, 2048); 
    if (var==NULL) return updateNewRecordHeader(xdrs, cur); //unexistent var
    if (DEBUG_SAVERESTORE)  std::cerr<<"Writing normal Variable "<<varName<<std::endl;
    writeVariableHeader(xdrs, var, isSysVar, readonly); 
    // !NULL variable stops here since no data
    if (var->N_Elements() == 0) return updateNewRecordHeader(xdrs, cur);
    // varstat=7 to read data
    int32_t varstart = VARSTART;
    xdr_int32_t(xdrs, &varstart);
    writeVariableData(xdrs, var);
    return updateNewRecordHeader(xdrs, cur);
  }
  
// //if we want to read a CData variable 
//    void writeCData(XDR *xdrs, BaseGDL* var) {
//    if (var==NULL) { //write 0 if NULL
//	  int32_t null = 0;
//	  xdr_int32_t(xdrs, &null);
//	  return;
//	} else { //write 1 if exist
//	  int32_t exist = 1;
//	  xdr_int32_t(xdrs, &exist);
//	}
//    writeVariableHeader(xdrs, var, false, false); 
//    // !NULL variable stops here since no data
//    if (var->N_Elements() == 0) return;
//    // varstat=7 to read data
//    int32_t varstart = VARSTART;
//    xdr_int32_t(xdrs, &varstart);
//    writeVariableData(xdrs, var);
//    return;
//  }

// //if we want to save a CData variable	
//   BaseGDL* readCData(EnvT* e, XDR *xdrs) {
//	 int32_t code=0;
//	 xdr_int32_t(xdrs, &code);
//	 if (code==0) return NULL;
//	 else if (code!=1) {
//	   std::cerr<<"error in readCData"<<std::endl;
//	   return NULL;
//	}
//	int isSysVar = 0;
//	bool isObjStruct = false;
//	BaseGDL* ret = getVariable(e, xdrs, isSysVar, isObjStruct);
//	if (ret == NULL)
//	{
//	  Message("Unable to restore CData.");
//	  return NULL;
//	}
//	if (isObjStruct) std::cerr<<"Problem: found an Object in normal variable processing -- should not happen.\n";
//	// should be at varstat=VARSTAT to read data
//	int32_t varstart = 0;
//	if (!xdr_int32_t(xdrs, &varstart)) std::cerr<<"problem in readCData."<<std::endl;
//	if (varstart != VARSTART)
//	{
//	  e->Throw("Lost track in VARIABLE definition .");
//	}
//    fillVariableData(xdrs, ret);
//
//    return ret;
//  }
	
	u_int64_t writeHeapVariable(EnvT* e, XDR *xdrs, DPtr ptr, bool isObject=false) {
    //what is passed is the list of existent heap positions occupied.
    //we  write only the ones that are actuall in , depending, the 
    heapT::iterator itheap;
    bool unknown=false;
      itheap=heapIndexMapSave.find(ptr);
      if ( itheap==heapIndexMapSave.end() ) unknown=true;
    if (unknown) {
      if (DEBUG_SAVERESTORE) std::cerr<<"ignoring unused heap_index "<<ptr<<std::endl;
      return xdr_get_gdl_pos(xdrs); //do nothing.
    }
    
    bool isSysVar=false;
    bool readonly=false;
    u_int64_t cur=writeNewRecordHeader(xdrs, HEAP_DATA); //HEAP_DATA
    int32_t heap_index=ptr;
    if (DEBUG_SAVERESTORE) {
      if (isObject) {
        std::cerr << "write OBJ at heap_index " << heap_index << std::endl;
      } else {
        std::cerr << "write PTR at heap_index " << heap_index << std::endl;
      }
    }
    //rest is normally the same as for any other variable
    xdr_int32_t(xdrs, &heap_index);
    int32_t heap_type = 0x02;
    if (isObject) heap_type = 0x04;
    xdr_int32_t(xdrs, &heap_type); 

    // start of TYPEDESC. Structures may be objects, in which case ptr.second is negative.
    BaseGDL* var;
    try{
    if (isObject) var=e->GetObjHeap(ptr); else var=e->GetHeap(ptr); //TRICK!
    } catch( GDLInterpreter::HeapException& hEx)
    {
      e->Throw("ID <"+i2s(ptr)+"> not found.");      
    }
    if (var==NULL) return updateNewRecordHeader(xdrs, cur); //unexistent var
    if (DEBUG_SAVERESTORE) std::cerr<<var->TypeStr()<<" @ "<<std::hex<<var<<std::dec<<std::endl;
    writeVariableHeader(xdrs, var, isSysVar, readonly, isObject );
    // !NULL variable stops here since no data
    if (var->N_Elements() == 0) return updateNewRecordHeader(xdrs, cur);
    // varstat=7 to read data
    int32_t varstart = VARSTART;
    xdr_int32_t(xdrs, &varstart);
    writeVariableData(xdrs, var);
    return updateNewRecordHeader(xdrs, cur);
  }   
  void restoreNormalVariable(EnvT* e, std::string varName, BaseGDL* ret) {
    //write variable back
    EnvStackT& callStack = e->Interpreter()->CallStack();
    DLong curlevnum = callStack.size();
    DSubUD* pro = static_cast<DSubUD*> (callStack[curlevnum - 1]->GetPro());
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
      //the existing var is deleted (including heap if it pointed to heap values), and restored anew.
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

  XDR* uncompress_trick(FILE* fid, XDR* xdrsmem, char* &expanded, DULong64 nextptr, DULong64 currentptr) {
    if (expanded!=NULL) free(expanded);
    uLong compsz = nextptr - currentptr;
    char* expandable = (char*) malloc(compsz);
    size_t retval = fread(expandable, 1, compsz, fid);
    int iloop = 1;
    uLong uncompsz;
    //of course one should never do like that. One should do as in gzstream.hpp...
    while (1)
    {
      uncompsz = 10 * iloop*compsz; //a default starting value...
      expanded = (char*) malloc(uncompsz);
      int retval = uncompress((Bytef *) expanded, &uncompsz, (Bytef *) expandable, compsz);
      if (retval == Z_OK) break; //ok length was sufficient
      free(expanded);
      if (retval != Z_BUF_ERROR) throw GDLException("fatal error when uncompressing data.");
      iloop++;
    }
    free(expandable);
    xdrmem_create(xdrsmem, NULL, 0, XDR_FREE);
    xdrmem_create(xdrsmem, expanded, uncompsz, XDR_DECODE);
    return xdrsmem;
  }
  
  bool testSafety() {
    if (sizeof(int8_t) != sizeof(DByte)) return false;
    if (sizeof(int16_t) != sizeof(DInt)) return false;
    if (sizeof(int32_t) != sizeof(DLong)) return false;
    if (sizeof(int64_t) != sizeof(DLong64)) return false;
    if (sizeof(u_int16_t) != sizeof(DUInt)) return false;
    if (sizeof(u_int32_t) != sizeof(DULong)) return false;
    if (sizeof(u_int64_t) != sizeof(DULong64)) return false;    
    if (sizeof(double) != sizeof(DDouble)) return false;    
    if (sizeof(float) != sizeof(DFloat)) return false;    
    return true;
  }

  // new fast restore.

  void gdl_restore(EnvT* e) {

    // xdr() is used through all the following. I program here xdr to use the 8, 16, 32 and 64 bits length
    // types of GDL/IDL (BYTE,INT,LONG,LONG64). However, a risk exist in some architectures that the real length
     // of DInt, DLong etc is not really the expected one (16 and 32 respectively). So the following is a test on these
    // lengths. If the test fails, 1) GDL is false for this architecture and that needs to be reported and
    // 2) gdl_restore cannot work.

    if (safetyTested == false)
    {
      isSafe = testSafety();
      if (!isSafe) e->Throw("Severe: internal representation of integers in this version of GDL is wrong, please report. Aborting unsafe use of RESTORE.");
      safetyTested = true;
    }
    //if testSafety is correct, DLong and int32_t , DInt and int16_t etc have the same meaning.

    static int FILENAME = e->KeywordIx("FILENAME");

    static int VERBOSE = e->KeywordIx("VERBOSE");
    bool verbose = e->KeywordSet(VERBOSE);
    DLong verboselevel=(verbose?1:0);
    if (verbose) e->AssureLongScalarKW(VERBOSE,verboselevel);

    static int DESCRIPTION = e->KeywordIx("DESCRIPTION");
    bool hasDescription = e->KeywordPresent(DESCRIPTION);
    // AC 20200323 : we may have this keyword set but no value in the file : should return "" 
    if (hasDescription)	e->SetKW(DESCRIPTION, new DStringGDL(""));
	
	static int SKIP_EXISTING = e->KeywordIx("SKIP_EXISTING");
	bool doSkipExisting=e->KeywordSet(SKIP_EXISTING);
	
    //empty heap map by security.
    heapIndexMapRestore.clear();

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

    WordExp(name);

    bool isCompress = false;

    FILE *fid;
    fid = fopen(name.c_str(), "rb");
    if (fid == NULL) e->Throw("Error opening file. Unit: XXXX, File: " + name + ".");

    XDR* xdrsmem = new XDR;
    XDR* xdrs;
    XDR* xdrsfile = new XDR;
    xdrstdio_create(xdrsfile, fid, XDR_DECODE);
    xdrs = xdrsfile;
    char* expanded = NULL;

    SizeT returned;
    char signature[4];
    returned = fread(signature, 4, 1, fid);
    if (signature[0] != 'S' || signature[1] != 'R')
    {
      fclose(fid);
      delete xdrsmem;
      delete xdrsfile;
      e->Throw("Not a valid save file: " + name + ".");
    }
    if (signature[3]==0x06) isCompress = true; //cerr<<"probably compressed"<<endl;
#define LONG sizeof(int32_t) //sizeof(DInt)
#define ULONG LONG 


    bool isHdr64 = false;
    int isSysVar = 0x00;
    bool isArray = false;
    bool isStructure = false;
    //will start at TMESTAMP
    u_int64_t currentptr = 0;
    u_int64_t nextptr = LONG;
    u_int32_t ptr_low, ptr_high;
    int32_t rectype;
    int32_t UnknownLong;
    bool SomethingFussyHappened = true;

    //pass twice. First to define heap variables only (and ancillary data).

    while (1)
    {
      xdrs = xdrsfile; //back to file if we were smarting the xdr to read a char* due to compression.
      if (fseek(fid, nextptr, SEEK_SET)) break;
      if (!xdr_int32_t(xdrs, &rectype)) break;

      if (DEBUG_SAVERESTORE) cerr << "Offset " << nextptr << ": record type " << rectypes[min(23,rectype)] << endl;

      if (rectype == END_MARKER)
      {
        SomethingFussyHappened = false;
        break;
      }
      if (isHdr64)
      {
        u_int64_t my_ulong64;
        if (!xdr_uint64_t(xdrs, &my_ulong64)) break;
        nextptr = my_ulong64; //HDR64 is followed by 2 longs.
        if (!xdr_int32_t(xdrs, &UnknownLong)) break;
        if (!xdr_int32_t(xdrs, &UnknownLong)) break;
      } else //the 2 pointers may point together to a l64 address, bug #1545
      { //we read a sort of BigEndian format
		if (!xdr_uint32_t(xdrs, &ptr_low)) break;
		if (!xdr_uint32_t(xdrs, &ptr_high)) break;
		nextptr = ptr_low;
		u_int64_t tmp = ptr_high;
        nextptr |= (tmp << 32);
        if (!xdr_int32_t(xdrs, &UnknownLong)) break; //only 1 long following in non-HDR64 format
        if (nextptr <=LONG) e->Throw("error in pointers, please report.");
      }

      //dispatch accordingly:

      isSysVar = 0x00;
      isArray = false;
      isStructure = false;
      currentptr = ftell(fid);

      switch ((int) rectype) {
        case TIMESTAMP: 
          if (nextptr < 1024)
          {
            //            cerr << "version offset < 1024... probably a compressed file" << endl;
            //            Message("sorry, can''t deal with this yet.  if possible, save without the \"COMPRESS\" flag.");
            isCompress = true;
          }
          if (isCompress) xdrs = uncompress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
          getTimeUserHost(xdrs);
          if (verbose)
          {
            if (isCompress) Message("Portable (XDR) compressed SAVE/RESTORE file.");
            else Message("Portable (XDR) SAVE/RESTORE file.");
            Message("Save file written by " + std::string(saveFileUser) + "@" + std::string(saveFileHost) + ", " + std::string(saveFileDatestring));
          }
          break;
        case VERSION_MARKER:
          if (isCompress) xdrs = uncompress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
          if (!getVersion(xdrs))
          {
            cerr << "error in VERSION" << endl;
            break;
          }
          break;
      case PROMOTE64:
          isHdr64 = true;
		  Message("Using unsupported PROMOTE64 pointers, expect problems.");
          break;
      case IDENTIFICATION:
          if (verbose)
          {
            if (isCompress) xdrs = uncompress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
            if (!getIdentification(xdrs))
            {
              cerr << "error in AUTHOR" << endl;
              break;
            }
          }
          break;
      case NOTICE:
          if (isCompress) xdrs = uncompress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
          if (!getNotice(xdrs))
          {
            cerr << "error in NOTICE" << endl;
            break;
          }
          break;
        case DESCRIPTION_MARKER:
          if (verbose || hasDescription)
          {
            if (isCompress) xdrs = uncompress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
            std::string descr(getDescription(xdrs));
            if (verbose) Message("Description: " + descr);
	    // AC 20200323 no "else" possible here, see above
            if (hasDescription) e->SetKW(DESCRIPTION, new DStringGDL(descr));
          }
          break;
      case COMMONBLOCK:
          if (isCompress) xdrs = uncompress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
          if (!defineCommonBlock(e, xdrs, verboselevel))
          {
            cerr << "error in COMMONBLOCK" << endl;
            break;
          }
          break;
      case HEAP_HEADER: // IS IN PREAMBLE since version 5. BEFORE ANY REFERENCE TO HEAP.
          if (isCompress) xdrs = uncompress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
        {
          int32_t elementcount;
          if (!xdr_int32_t(xdrs, &elementcount)) break;
          int32_t indices[elementcount];
          if (!xdr_vector(xdrs, (char*) indices, elementcount, sizeof (int32_t), (xdrproc_t) xdr_int32_t)) break;
          if (DEBUG_SAVERESTORE)
          {
            cerr << "Heap indexes, " << elementcount << " elements: ";
            for (int i = 0; i < elementcount; ++i) cerr << indices[i] << ",";
            cerr << endl;
          }

          break;
        }
        case HEAP_DATA: //define all HEAP_DATA variable but do not fill them yet
          if (isCompress) xdrs = uncompress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
        {
          int32_t heap_index = 0;
          if (!xdr_int32_t(xdrs, &heap_index)) break;
          int32_t heap_unknown = 0;
          if (!xdr_int32_t(xdrs, &heap_unknown)) break; // start of TYPEDESC
          bool isObjStruct=false;
          BaseGDL* ret = getVariable(e, xdrs, isSysVar, isObjStruct);
          if (ret == NULL)
          {
            fclose(fid);
            delete xdrsmem;
            delete xdrsfile;
            e->Throw("error reading heap variable definition.");
          }

         //allocate corresponding heap entries and store gdl variable and heap entry in heapIndexMapRestore:
          //if ret is a struct defining an object, use ObjHeap.
          DPtr ptr;
          if (isObjStruct) ptr = e->NewObjHeap(1, static_cast<DStructGDL*> (ret)); else ptr = e->NewHeap(1, ret);
          heapIndexMapRestore.insert(std::pair<long, std::pair<BaseGDL*,DPtr>>(heap_index, std::make_pair(ret,ptr)));

          if (ret == NullGDL::GetSingleInstance()) break; //no data follows

          // should be at varstat=VARSTAT to read data
          int32_t varstart = 0;
          if (!xdr_int32_t(xdrs, &varstart)) break;
          if (varstart != VARSTART)
          {
            fclose(fid);
            delete xdrsmem;
            delete xdrsfile;
            e->Throw("Lost track in HEAP_DATA definition at offset " + i2s(nextptr));

          }
        }
          break;
        default:
          break;
      }
    }

    if (SomethingFussyHappened)
    {
      fclose(fid);
      delete xdrsmem;
      delete xdrsfile;
      e->Throw("Error Reading File: " + name + ".");
    }

    //from then, saveFileHeapMap.second contains heap DPtr.

    //Then on second pass, define normal variables that may be pointers to heap.
    rewind(fid);
    if (DEBUG_SAVERESTORE) cerr << "Second Pass"<<endl;
    currentptr = 0;
    nextptr = LONG;
    SomethingFussyHappened = true;

    while (1)
    {

      xdrs = xdrsfile; //back to file if we were smarting the xdr to read a char* due to compression.
      if (fseek(fid, nextptr, SEEK_SET)) break;
      if (!xdr_int32_t(xdrs, &rectype)) break;

      if (DEBUG_SAVERESTORE) cerr << "Offset " << nextptr << ": record type " << rectypes[min(23,rectype)] << endl;

      if (rectype == END_MARKER)
      {
        SomethingFussyHappened = false;
		//test if something behind END_MARKER
		nextptr+=16; //jump to end of normal file, may be EOF
		xdrs = xdrsfile; //back to file if we were smarting the xdr to read a char* due to compression.
		if (fseek(fid, nextptr, SEEK_SET)) break;
		int32_t save_routine_version=0;
		if (!xdr_int32_t(xdrs, &save_routine_version)) break;
		if (save_routine_version != GDL_SAVE_ROUTINES_VERSION) {
		  Message ("This SAVE file ( version "+i2s(save_routine_version)+") is not compatible with Current version ("+i2s(GDL_SAVE_ROUTINES_VERSION)+"), saved routines cannot be restored.");
		  goto endoffile;
		}
		nextptr+=4;
		
		while (1) {
		  xdrs = xdrsfile; //back to file if we were smarting the xdr to read a char* due to compression.
		  if (fseek(fid, nextptr, SEEK_SET)) break;
		  if (!xdr_int32_t(xdrs, &rectype)) break;

		  if (DEBUG_SAVERESTORE) cerr << "Offset " << nextptr << ": record type " << rectypes[min(24,rectype)] << endl;
		  if (isHdr64) {
			u_int64_t my_ulong64;
			if (!xdr_uint64_t(xdrs, &my_ulong64)) break;
			nextptr = my_ulong64;
			if (!xdr_int32_t(xdrs, &UnknownLong)) break;
			if (!xdr_int32_t(xdrs, &UnknownLong)) break;
		  } else {//we read a sort of BigEndian format
			if (!xdr_uint32_t(xdrs, &ptr_low)) break;
			if (!xdr_uint32_t(xdrs, &ptr_high)) break;
			nextptr = ptr_low;
			u_int64_t tmp = ptr_high;
			nextptr |= (tmp << 32);
			if (!xdr_int32_t(xdrs, &UnknownLong)) break;
		  }

		  //dispatch accordingly:

		  isSysVar = 0x00;
		  isArray = false;
		  isStructure = false;
		  currentptr = ftell(fid);

		  switch ((int) rectype) {
		  case GDL_END_MARKER:
			goto endoffile;
		  case GDL_COMPILED:
			if (isCompress) xdrs = uncompress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
			if (!getDSubUD(e, xdrs, verbose, doSkipExisting)) {
			  cerr << "error in COMPILED" << endl;
        break;
      }
			break;
		  default:
			goto endoffile;
			break;
		  }
		}
	  }
      if (isHdr64)
      {
        u_int64_t my_ulong64;
        if (!xdr_uint64_t(xdrs, &my_ulong64)) break;
        nextptr = my_ulong64;
        if (!xdr_int32_t(xdrs, &UnknownLong)) break;
        if (!xdr_int32_t(xdrs, &UnknownLong)) break;
      } else {//we read a sort of BigEndian format
		if (!xdr_uint32_t(xdrs, &ptr_low)) break;
		if (!xdr_uint32_t(xdrs, &ptr_high)) break;
		nextptr = ptr_low;
		u_int64_t tmp = ptr_high;
        nextptr |= (tmp << 32);
        if (!xdr_int32_t(xdrs, &UnknownLong)) break;
      }

      //dispatch accordingly:

      isSysVar = 0x00;
      isArray = false;
      isStructure = false;
      currentptr = ftell(fid);

      switch ((int) rectype) {
      case SYSTEM_VARIABLE:
        if (DEBUG_SAVERESTORE) cerr<<"SYSTEM ";
          isSysVar = 0x02; //see? no break. defines a read-write system variable (default)
      case VARIABLE:
        if (DEBUG_SAVERESTORE) cerr<<"Variable ";
          if (isCompress) xdrs = uncompress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
        {
          char* varname = 0;
          if (!xdr_string(xdrs, &varname, 2048)) break;
          string varName(varname);
          if (DEBUG_SAVERESTORE) cerr<<varname<<endl;
          bool isObjStruct=false;
          BaseGDL* ret = getVariable(e, xdrs, isSysVar, isObjStruct);
          if (ret == NULL)
          {
            Message("Unable to restore " + varName +".");
            break;
          }
          if (isObjStruct) std::cerr<<"Problem: found an Object in normal variable processing -- should not happen.\n";
          // should be at varstat=VARSTAT to read data
          int32_t varstart = 0;
          if (!xdr_int32_t(xdrs, &varstart)) break;
          if (varstart != VARSTART)
          {
            fclose(fid);
            delete xdrsmem;
            delete xdrsfile;
            e->Throw("Lost track in VARIABLE definition at offset " + i2s(nextptr));
          }

          fillVariableData(xdrs, ret);
          if (isSysVar & 0x03) { //readonly system var
			std::string name=varName.substr(1);
		    if (FindInVarList(sysVarRdOnlyList, name) != NULL) { //exists as readonly
			  if (FindInVarList(sysVarNoSaveList, name) != NULL) { //is a system-defined readonly, not updateable.
				Message("Read only system defined system variable not restored: " + varName);
			  } else {
			    Message("Attempt to write to a readonly variable: " + varName);
			  }
			} else systemReadonlyVariableVector.push_back(make_pair(varName, ret));
		  }
          else if (isSysVar & 0x02) systemVariableVector.push_back(make_pair(varName, ret));
          else variableVector.push_back(make_pair(varName, ret));
          Guard<BaseGDL>* guard = new Guard<BaseGDL>;
          guard->Reset(ret);
          guardVector.push_back(guard);
        }
          break;
      case HEAP_DATA: //use previous variable, now that the list of heap pointers is complete.
          if (isCompress) xdrs = uncompress_trick(fid, xdrsmem, expanded, nextptr, currentptr);
        {
          int32_t heap_index = 0;
          if (!xdr_int32_t(xdrs, &heap_index)) break;
          int32_t heap_unknown = 0;
          if (!xdr_int32_t(xdrs, &heap_unknown)) break; // start of TYPEDESC
          bool isObjStruct=false;
          BaseGDL* dummy = getVariable(e, xdrs, isSysVar, isObjStruct); //obliged to read all that infortunately.
          // we are at varstat=VARSTAT since this has already been seen above
          int32_t varstart = 0;
          if (!xdr_int32_t(xdrs, &varstart)) break; 
          GDLDelete(dummy); //get rid of variable that may have wrong pointers and restore the good one:
          if (DEBUG_SAVERESTORE) cerr<<"Restoring Heap Data initially at #"<<heap_index<<"...";
          if (heapIndexMapRestore.find(heap_index) == heapIndexMapRestore.end()) {
            e->Throw("Lost track in HEAP VARIABLE definition at offset " + i2s(nextptr));
          } else {
            BaseGDL* ret = heapIndexMapRestore.find(heap_index)->second.first;
            if (ret == NullGDL::GetSingleInstance()) break; //no data follows as this is a !NULL
            fillVariableData(xdrs, ret);
            if (DEBUG_SAVERESTORE) std::cerr<<"at offset #"<<heapIndexMapRestore.find(heap_index)->second.second<<std::endl;
          }


        }
          break;
      default:
          break;
      }
    }
endoffile:    
    if (expanded!=NULL) free(expanded);
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
      if (verbose) Message("Restored variable: " + (variableVector.back()).first+".");
      variableVector.pop_back();
    }

    //heap variables have one more reference that necessary (1 at creation of the heap variable + 1 each time somthing points to it).
    //decrease each of them by 1:
    std::map<long, std::pair<BaseGDL*,DPtr>>::iterator imap;
    for (imap=heapIndexMapRestore.begin(); imap!=heapIndexMapRestore.end(); ++imap) {
      DPtr ptr=(*imap).second.second;
      GDLInterpreter::DecRef(ptr);
      GDLInterpreter::DecRefObj(ptr); //if not PTR then try Obj --- will always decrement the good one now that ptr is unique between Obj and Ptr.
    }
    //everything ok, remove guards and exit
    while (!guardVector.empty())
    {
      guardVector.back()->Release();
      guardVector.pop_back();
    }

  }
// This adds in heaplist map:: all the OBJ or normal HEAP adresses of variables that are part of any named variable that are to be SAVEd.
// GDL has 2 heaplists, one for OBJ and the other for normal pointers, but I've insured that the index in these maps is common, so
// it is just as if there was only one heap list, as in IDL.
  void addToHeapList(EnvT* e, BaseGDL* var)
  {
    if (var->Type() == GDL_PTR) {
      for (SizeT ielem = 0; ielem < var->N_Elements(); ++ielem) {
        DPtr subptr = (*static_cast<DPtrGDL*> (var))[ielem];
        if (subptr > 0) {
          heapT::iterator itheap=heapIndexMapSave.find(subptr);
          if ( itheap==heapIndexMapSave.end() ) {
            try {
              BaseGDL* v = e->GetHeap(subptr);  //this pointed-to heap value MAY NOT EXIST. It may have been destroyed.
            heapIndexMapSave.insert(std::pair<DPtr, SizeT>(subptr, subptr));
              if (DEBUG_SAVERESTORE)   std::cerr<<"Adding Heaplist PTR: HeapIndex="<<subptr<<", id="<<std::hex<<v<<std::dec<<std::endl;
            if (v && v!=NullGDL::GetSingleInstance()) addToHeapList(e, v);
            } catch( GDLInterpreter::HeapException& hEx)
            {
              if (DEBUG_SAVERESTORE) std::cerr<<"PTR ID<"<<subptr<<"> not found."<<std::endl;      
            }
          }
        }
      }
    } else if (var->Type() == GDL_OBJ) {
      for (SizeT ielem = 0; ielem < var->N_Elements(); ++ielem) {
        DObj subptr = (*static_cast<DObjGDL*> (var))[ielem];
        if (subptr > 0) {
          heapT::iterator itheap=heapIndexMapSave.find(subptr);
          if ( itheap==heapIndexMapSave.end() ) {
            try {
              BaseGDL* v = e->GetObjHeap(subptr); //remember subptr IS uniquely increasing between OBJ and PTR Heaps indexes, but v is to be found in OBJ.
              heapIndexMapSave.insert(std::pair<DPtr, SizeT>(subptr, subptr));
              if (DEBUG_SAVERESTORE)   std::cerr<<"Adding Heaplist OBJ: HeapIndex="<<subptr<<", id="<<std::hex<<v<<std::dec<<std::endl;
            if (v && v!=NullGDL::GetSingleInstance() ) addToHeapList(e, v);
            } catch( GDLInterpreter::HeapException& hEx)
            {
              if (DEBUG_SAVERESTORE) std::cerr<<"OBJ ID <"<<subptr<<"> not found."<<std::endl;      
            }
          }
        }
      }
    } else if (var->Type() == GDL_STRUCT) {
      DStructGDL* str = static_cast<DStructGDL*> (var);
      for (SizeT ielem = 0; ielem < var->N_Elements(); ++ielem) {
        for (int itag = 0; itag < str->NTags(); ++itag) {
          BaseGDL* v = str->GetTag(itag, ielem);
          if (v && v!=NullGDL::GetSingleInstance() ) addToHeapList(e, v);
        }
      }
    }
    return;
  }

  u_int64_t writeHeapList(XDR* xdrs) {
// writing heap list for IDL compatiblilty implies to "mimic" the single heap list of IDL.
//We write the PTRs first, then the OBJs after. OBJ ptrs will thus start at the last value held by PTRs plus one.
    int32_t elementcount = heapIndexMapSave.size();
    if (elementcount < 1) return xdr_get_gdl_pos(xdrs);
    u_int64_t cur = writeNewRecordHeader(xdrs, HEAP_HEADER); //HEAP_HEADER
    xdr_int32_t(xdrs, &elementcount);
    int32_t indices[elementcount];
    SizeT i = 0;
    heapT::iterator it;
    for (it = heapIndexMapSave.begin(); it != heapIndexMapSave.end(); ++it) indices[i++] = (*it).second;
    
    xdr_vector(xdrs, (char*) indices, elementcount, sizeof (int32_t), (xdrproc_t) xdr_int32_t);
    if (DEBUG_SAVERESTORE) {
      cerr << "Heap indexes, " << elementcount << " elements: ";
      for (int i = 0; i < elementcount; ++i) cerr << indices[i] << ",";
      cerr << endl;
    }
    u_int64_t next = updateNewRecordHeader(xdrs, cur);
    return next;
  }

  u_int64_t writeCommonList(EnvT*e, XDR* xdrs, std::string commonname) {
   if (DEBUG_SAVERESTORE) std::cerr<<"Writing Common "<<commonname<<std::endl;
    EnvStackT& callStack = e->Interpreter()->CallStack();
    int32_t curlevnum = callStack.size();
    DSubUD* pro = static_cast<DSubUD*> (callStack[curlevnum - 1]->GetPro());
    DCommon* c=pro->Common(commonname);
    int32_t ncommonvars = c->NVar();
    if (ncommonvars < 1) return xdr_get_gdl_pos(xdrs);
    u_int64_t cur = writeNewRecordHeader(xdrs, COMMONBLOCK); //COMMON
    xdr_int32_t(xdrs, &ncommonvars);
    char* name = (char*)commonname.c_str();
    u_int len=c->Name().size();
    xdr_string(xdrs, &name, len);
    char* varnames[ncommonvars];
    u_int lens[ncommonvars];
    for (int i = 0; i < ncommonvars; ++i) {
      varnames[i] = (char*)c->VarName(i).c_str();
     if (DEBUG_SAVERESTORE)     std::cerr<<"        Name   "<<c->VarName(i)<<std::endl;
     }
    for (int i = 0; i < ncommonvars; ++i) lens[i] = c->VarName(i).size();
    for (int i = 0; i < ncommonvars; ++i) xdr_string(xdrs, &varnames[i], lens[i]);    
    u_int64_t next = updateNewRecordHeader(xdrs, cur);
    if (DEBUG_SAVERESTORE) std::cerr<<std::endl;
    return next;
  } 
  bool myfunctionToSortStringsInPair (std::pair<std::string, BaseGDL*> i, std::pair<std::string, BaseGDL*> j) { return (i.first>j.first); }      
  // new fast save.
  void gdl_save(EnvT* e) {

    // xdr() is used through all the following. I program here xdr to use the 8, 16, 32 and 64 bits length
    // types of GDL/IDL (BYTE,INT,LONG,LONG64). However, a risk exist in some architectures that the real length
    // of DInt, DLong etc is not really the expected one (16 and 32 respectively). So the following is a test on these
    // lengths. If the test fails, 1) GDL is false for this architecture and that needs to be reported and
    // 2) gdl_restore cannot work.

    if (safetyTested == false)
    {
      isSafe = testSafety();
      if (!isSafe) e->Throw("Severe: internal representation of integers in this version of GDL is wrong, please report. Aborting unsafe use of RESTORE.");
      safetyTested = true;
    }
    //if testSafety is correct, DLong and int32_t , DInt and int16_t etc have the same meaning.
  
    //empty maps etc by security.
    heapIndexMapSave.clear();
    predeflist.clear();
    
    static int VERBOSE = e->KeywordIx("VERBOSE");
    bool verbose = e->KeywordSet(VERBOSE);
    DLong verboselevel=(verbose?1:0);
    if (verbose) e->AssureLongScalarKW(VERBOSE,verboselevel);

	//variables or routines, not both. Note this is not a GDL requirement, merely for compatibility.
	bool wantsToSaveVariables=false;
    static int VARIABLES = e->KeywordIx("VARIABLES");
    bool doVars=e->KeywordSet(VARIABLES);
	if (doVars) wantsToSaveVariables=true;
    static int ALL = e->KeywordIx("ALL");
    bool allVars=e->KeywordSet(ALL);
	if (allVars) wantsToSaveVariables=true;
    static int SYSTEM_VARIABLES = e->KeywordIx("SYSTEM_VARIABLES");
    bool doSys=e->KeywordSet(SYSTEM_VARIABLES);
	if (doSys) wantsToSaveVariables=true;
    static int COMM = e->KeywordIx("COMM");
    bool doComm=e->KeywordSet(COMM);
	if (doComm) wantsToSaveVariables=true;
    static int ROUTINES=e->KeywordIx("ROUTINES");
    bool doRoutines =e->KeywordSet(ROUTINES);
	if (doRoutines && wantsToSaveVariables) e->Throw("Conflicting keywords.");
    if (allVars) {
      doSys=true;
      doComm=true;
      doVars=true;
    }

	static int COMPRESS = e->KeywordIx("COMPRESS");
	save_compress = (e->KeywordSet(COMPRESS));

	static int FILENAME = e->KeywordIx("FILENAME");

    static int DESCRIPTION = e->KeywordIx("DESCRIPTION");
    bool needsDescription = e->KeywordPresent(DESCRIPTION);

	static int IGNORE_NOSAVE = e->KeywordIx("IGNORE_NOSAVE");
	bool ignore_nosave=e->KeywordPresent(IGNORE_NOSAVE);
    
    DStringGDL* description=NULL;
    if (needsDescription) description=e->GetKWAs<DStringGDL>(DESCRIPTION);

    std::vector<std::pair<std::string, BaseGDL*> > variableVector;
    std::vector<std::pair<std::string, BaseGDL*> > systemVariableVector;
//    std::vector<std::pair<std::string, BaseGDL*> > systemReadonlyVariableVector; //for readonly variables //not used
    set<string> commonList;
// versioning for safety of future changes in parser (for saved routine code)	
	int32_t save_routine_version = GDL_SAVE_ROUTINES_VERSION;
//Variables
    std::queue<std::pair<std::string, BaseGDL*> >varNameList;

    long nparam=e->NParam();
//Saving routines excludes saving anything else (same as IDL in fact)
	if (doRoutines) {
	  doVars=false;
	  doSys=false;
	  doComm=false;
	} else {
      if (!doComm && !doSys) doVars=(doVars||(nparam==0));
	}

    if (doSys)
    {
      SizeT nVar = sysVarList.size();
      for (SizeT v = 0; v < nVar; ++v)
      {
        DVar* var = sysVarList[v];
        DString sysVarName = var->Name();
        if (FindInVarList(sysVarNoSaveList, sysVarName) != NULL) continue; //only those need to be absent from a SVAE file. User-defined readonly sysVars CAN be written to a SAVE file.
        systemVariableVector.push_back(make_pair("!" + sysVarName, var->Data()));
      }
      std::sort (systemVariableVector.begin(), systemVariableVector.end(),  myfunctionToSortStringsInPair);
    }

    if (doVars) 
    {
      //will list (all) variables, incl. common defined, at desired level.
      EnvStackT& callStack = e->Interpreter()->CallStack();
      DLong curlevnum = callStack.size();
      DSubUD* pro = static_cast<DSubUD*> (callStack[curlevnum-1]->GetPro());

      SizeT nVar = pro->Size(); // # var in GDL for desired level 
      SizeT nComm = pro->CommonsSize(); // # has commons?
      SizeT nTotVar = nVar + nComm; //All the variables availables at that lev.

      if (nTotVar > 0)
      {
        if (nComm > 0)
        {
          DStringGDL* list = static_cast<DStringGDL*> (pro->GetCommonVarNameList());
          for (SizeT i = 0; i < list->N_Elements(); ++i) {
            BaseGDL** var =  pro->GetCommonVarPtr((*list)[i]);
            if (*var != NULL) varNameList.push(make_pair((*list)[i],*var));
          }
        }
        if (nVar > 0 )
        {
          for (SizeT i = 0; i < nVar; ++i) {
            BaseGDL* var = ((EnvT*) (callStack[curlevnum - 1]))->GetTheKW(i); //NULL variables must be saved too
            if (var != NULL) varNameList.push(make_pair(pro->GetVarName(i),var));
          }
        }
      }
    }

    if (doComm)
    {
      //will list (all) variables, incl. common defined, at desired level.
      EnvStackT& callStack = e->Interpreter()->CallStack();
      DLong curlevnum = callStack.size();
      DSubUD* pro = static_cast<DSubUD*> (callStack[curlevnum - 1]->GetPro());

      SizeT nComm = pro->CommonsSize(); // # has commons?
      if (nComm > 0)
      {
        DStringGDL* list = static_cast<DStringGDL*> (pro->GetCommonVarNameList());
        for (SizeT i = 0; i < list->N_Elements(); ++i)
        {
          DCommonBase* common = pro->FindCommon((*list)[i]);
          commonList.insert(common->Name());
        }
      }
	}

	if (!doRoutines) {
	  for (int i = 0; i < nparam; ++i) {
		BaseGDL* var = e->GetPar(i);
		if (var == NULL) {
		  Message("Undefined item not saved: " + e->GetParString(i));
		} else //var exists, but may have been already done by doVars.
		{
		  if (!doVars) varNameList.push(make_pair(e->GetParString(i), var));
		}
	  }

	  while (!varNameList.empty()) {
		std::string varName = varNameList.front().first;
		BaseGDL* var = varNameList.front().second;
		varNameList.pop();
		if (var->N_Elements() == 0) Message("Undefined item not saved: " + varName + ".");
		else {
		  //sytem variables are saved with /SYSTEM. This is a special case, test: try "SAVE,!P" with IDL:
		  //<Expression> xxx generates an error.
		  if (varName.substr(0, 1) == "<") e->Throw("Expression must be named variable in this context:" + varName);
		  else {
			//examine variable. Cases: in common, normal. remove common name if necessary.
			std::size_t pos = varName.find("(", 0);
			if (pos != std::string::npos) varName = varName.substr(0, pos - 1); //one Blank.
			variableVector.push_back(make_pair(varName, var));
		  }
		}
	  }

	  //Now, do we have heap variables in the variableVector (pointers)? If yes, we add these BaseGDL* to the heaplist unique list.
	  //we then write the heap variables first.
	  //then, anytime a pointer is found in any normal variable, instead of writing the data we will
	  //just write the index of the pointed-to address in the heaplist.
	  std::vector<std::pair<std::string, BaseGDL*> >::iterator itvar;
	  for (itvar = variableVector.begin(); itvar != variableVector.end(); ++itvar) {
		addToHeapList(e, itvar->second);
	  }
	  //NOTE: the following will not presently keep the information of a sysvar and readonly sysvar in heap. Which
	  //is probably overkill?
	  for (itvar = systemVariableVector.begin(); itvar != systemVariableVector.end(); ++itvar) {
		addToHeapList(e, itvar->second);
	  }
	  //    for (itvar=systemReadonlyVariableVector.begin(); itvar!=systemReadonlyVariableVector.end(); ++itvar) {
	  //      addToHeapList(e, itvar->second);
	  //    }    
	}
	
    DString name;
    if (e->KeywordPresent(FILENAME))
    {
      e->AssureScalarKW<DStringGDL>(FILENAME, name);
    } else name = "idlsave.dat";

    WordExp(name);

    save_fid = fopen(name.c_str(), "wb+");
    if (save_fid == NULL) e->Throw("Error opening file. Unit: XXXX, File: " + name + ".");

    XDR* xdrs = new XDR;
    xdrstdio_create(xdrs, save_fid, XDR_ENCODE);

    SizeT returned;
    char signature[4]={'S','R',0x00,0x04};
    if (save_compress) signature[3]=0x06;
    returned = fwrite(signature, 4, 1, save_fid);

#define LONG sizeof(int32_t) //sizeof(DInt)
#define ULONG LONG 


    //will start at TIMESTAMP if not /ROUTINE
    u_int64_t nextptr = LONG;
    u_int64_t oldptr = 0;

    fseek(save_fid, nextptr, SEEK_SET);

if (!doRoutines){    
    const int    MAX_DATE_STRING_LENGTH = 80;
    time_t t=time(0);
    struct tm * tstruct;
    tstruct=localtime(&t);
    char *saveFileDatestring=new char[MAX_DATE_STRING_LENGTH];
    const char *dateformat="%a %h %d %T %Y";// day,month,day number,time,year
    SizeT res=strftime(saveFileDatestring,MAX_DATE_STRING_LENGTH,dateformat,tstruct);
    std::string saveFileUser = GetEnvString( "USER");
    std::string saveFileHost = ""; //GetEnvString( "HOST");
//    if (saveFileHost == "") saveFileHost = GetEnvString( "HOSTNAME");
//    if (saveFileHost == "") {
#define GDL_HOST_NAME_MAX 255
      char gethost[GDL_HOST_NAME_MAX];
      size_t lgethost=GDL_HOST_NAME_MAX;
      // don't know if this primitive is available on Mac OS X
      int success = gethostname(gethost, lgethost);
      if( success == 0) saveFileHost=string(gethost);
//    }
    //TIMESTAMP
    nextptr=writeTimeUserHost(xdrs, saveFileDatestring, (char*)saveFileUser.c_str(), (char*)saveFileHost.c_str());
    int32_t format=9; //IDL v. 6.1 ++
    DStructGDL* version = SysVar::Version();
    static unsigned osTag = version->Desc()->TagIndex( "OS");
    static unsigned archTag = version->Desc()->TagIndex( "ARCH");
    static unsigned releaseTag = version->Desc()->TagIndex( "RELEASE");
    DString os = (*static_cast<DStringGDL*>( version->GetTag( osTag, 0)))[0];
    DString arch = (*static_cast<DStringGDL*>( version->GetTag( archTag, 0)))[0];
    DString release = (*static_cast<DStringGDL*>( version->GetTag( releaseTag, 0)))[0];
    //VERSION
    nextptr=writeVersion(xdrs, &format, (char*)arch.c_str(), (char*) os.c_str() , (char*) release.c_str());
    //HEAPLIST
    if (heapIndexMapSave.size() > 0) nextptr=writeHeapList(xdrs);
    // promote64: NO!
//    //notice:
//    std::string notice="Made by GDL, a free software program that you can redistribute and/or modify"
//                       " under the terms of the GPL, use at your own risks.";
//    nextptr=writeNotice(xdrs, (char*)notice.c_str());
    if (description!=NULL)  nextptr=writeDescription(xdrs,(char*)((*description)[0].c_str()));
    
    //COMMON
    std::set<std::string>::iterator itcommon;
    for (itcommon=commonList.begin(); itcommon!=commonList.end(); ++itcommon) {      
      nextptr=writeCommonList(e, xdrs, *itcommon);
      if (verboselevel>0) Message("Saved common block: " + *itcommon);
    }
    //HEAP Variables: all terminal variables pointed by, OBJs or PTRs
    oldptr=nextptr; //will test if number goes down: would be an error until the 64 bit xdr write is not fixed
    
    if (heapIndexMapSave.size() > 0) { //there is some heap...
      DPtrGDL* heapPtrList=e->Interpreter( )->GetAllHeap( );
      for (SizeT i=0; i<heapPtrList->N_Elements(); ++i) { nextptr=writeHeapVariable(e, xdrs, (*heapPtrList)[i], false); if (nextptr < oldptr) goto fail;oldptr=nextptr;}
      GDLDelete(heapPtrList);
      DObjGDL* heapObjPtrList=e->Interpreter( )->GetAllObjHeap( );
      for (SizeT i=0; i<heapObjPtrList->N_Elements(); ++i) {nextptr=writeHeapVariable(e, xdrs, (*heapObjPtrList)[i], true); if (nextptr < oldptr) goto fail;oldptr=nextptr;}
      GDLDelete(heapObjPtrList);
    }

//
//    while (!systemReadonlyVariableVector.empty())
//    {
//      nextptr = writeNormalVariable(xdrs, systemReadonlyVariableVector.back().first, (systemReadonlyVariableVector.back()).second, 0x3);
//      if (verboselevel > 0) Message("Saved variable: " + (systemReadonlyVariableVector.back()).first + ".");
//      systemReadonlyVariableVector.pop_back();
//    }
    while (!systemVariableVector.empty())
    {
      nextptr = writeNormalVariable(xdrs, systemVariableVector.back().first, (systemVariableVector.back()).second, 0x2); if (nextptr < oldptr) goto fail;oldptr=nextptr;
      if (verboselevel > 0) Message("SAVE: Saved system variable: " + (systemVariableVector.back()).first + ".");
      systemVariableVector.pop_back();
    }
    
    while (!variableVector.empty())
    { 
      nextptr=writeNormalVariable(xdrs, variableVector.back().first, (variableVector.back()).second); if (nextptr < oldptr) goto fail;oldptr=nextptr;
      if (verboselevel>0) Message("SAVE: Saved variable: " + (variableVector.back()).first+".");
      variableVector.pop_back();
    }
}
    nextptr=writeEnd(xdrs);
	//write Our save routine library version just after regular end. It will not be seen by IDL.

	xdr_int32_t(xdrs, &save_routine_version);
	
	if (doRoutines) {
	  if (nparam > 0) { //will not check nosave as the names are explicit
		for (int i = 0; i < nparam; ++i) {
		  DString name;
		  bool notFound=true;
		  e->AssureStringScalarPar(i,name);
		  name = StrUpCase(name);
		  for (ProListT::iterator i = proList.begin(); i != proList.end(); ++i) {
			if ((*i)->ObjectName() == name) {
			  DPro * p = (*i);
			  if (p->GetSCC() != NULL) {
				nextptr = writeDSubUD(xdrs, p, true);
				notFound = false;
			  }
			  break;
			}
		  }
		  //May also exist as a FUN, see e.g. TIC & TOC
		  for (FunListT::iterator i = funList.begin(); i != funList.end(); ++i) {
			if ((*i)->ObjectName() == name) {
			  DFun * f = (*i);
			  if (f->GetSCC() != NULL) {
				nextptr = writeDSubUD(xdrs, f, false);
				notFound = false;
			  }
			  break;
			}
		  }
		  if (notFound) Message("Undefined item not saved: "+name);
		}
	  } else { //wil not save NoSave pro/funs unless IGNORE_NOSAVE is set
		for (ProListT::iterator i = proList.begin(); i != proList.end(); ++i) {
		  DPro * p = (*i);
		  //		std::cerr<<"PRO: "<<p->ObjectName()<<std::endl;
		  if (ignore_nosave || !(p->isNoSave())) nextptr = writeDSubUD(xdrs, p, true);
		}
		for (FunListT::iterator i = funList.begin(); i != funList.end(); ++i) {
		  DFun * f = (*i);
		  //		std::cerr<<"FUN: "<<f->ObjectName()<<std::endl;
		  if (ignore_nosave || !(f->isNoSave())) nextptr = writeDSubUD(xdrs, f, false);
		}
		for (SizeT i = 0; i < structList.size(); ++i) {
		  DStructDesc* s=structList[i];
		  for (auto j = 0; j < s->ProList().size(); ++j) {
			DPro * p = (s->ProList())[j];
			if (p->GetSCC() != NULL) if (ignore_nosave || !(p->isNoSave())) nextptr = writeDSubUD(xdrs, p, true);
		  }
		  for (auto j= 0 ; j < s->FunList().size(); ++j) {
			DFun * f = (s->FunList())[j];
  		    if (f->GetSCC() != NULL) if (ignore_nosave || !(f->isNoSave())) nextptr = writeDSubUD(xdrs, f, false);
		  }
		}

	  }
	}
	
    nextptr=writeGDLEnd(xdrs);
    xdr_destroy(xdrs);
    fclose(save_fid);
    return;
    fail:
    xdr_destroy(xdrs);
    fclose(save_fid);
    e->Throw("GDL cannot yet write such large SAVE files. Try with less values. File "+name+" is invalid, remove it.");  
  }

//  void gdl_savetest(EnvT* e) {
//	long nparam = e->NParam();
//	if (nparam > 0) { //will not check nosave as the names are explicit
//	  for (int i = 0; i < nparam; ++i) {
//		DString name;
//		bool notFound = true;
//		e->AssureStringScalarPar(i, name);
//		name = StrUpCase(name);
//		for (ProListT::iterator i = proList.begin(); i != proList.end(); ++i) {
//		  if ((*i)->ObjectName() == name) {
//			std::cout << "-----------------------------------" << (*i)->ObjectName() << "----------------------------------------" << std::endl;
//			antlr::print_tree pt;
//			pt.pr_tree(static_cast<ProgNodeP> ((*i)->GetTree()));
//			break;
//		  }
//		}
//		//May also exist as a FUN, see e.g. TIC & TOC
//		for (FunListT::iterator i = funList.begin(); i != funList.end(); ++i) {
//		  if ((*i)->ObjectName() == name) {
//			std::cout << "-----------------------------------" << (*i)->ObjectName() << "----------------------------------------" << std::endl;
//			antlr::print_tree pt;
//			pt.pr_tree(static_cast<ProgNodeP> ((*i)->GetTree()));
//			break;
//		  }
//		}
//	  }
//	} else {
//	  for (ProListT::iterator i = proList.begin(); i != proList.end(); ++i) {
//		std::cout << "-----------------------------------" << (*i)->ObjectName() << "----------------------------------------" << std::endl;
//		antlr::print_tree pt;
//		pt.pr_tree(static_cast<ProgNodeP> ((*i)->GetTree()));
//	  }
//	  for (FunListT::iterator i = funList.begin(); i != funList.end(); ++i) {
//		std::cout << "-----------------------------------" << (*i)->ObjectName() << "----------------------------------------" << std::endl;
//		antlr::print_tree pt;
//		pt.pr_tree(static_cast<ProgNodeP> ((*i)->GetTree()));
//	  }
//	}
//  }
}

