/***************************************************************************
                          |FILENAME|  -  description
                             -------------------
    begin                : |DATE|
    copyright            : (C) |YEAR| by |AUTHOR|
    email                : |EMAIL|
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

static DStructGDL* GetOBJ( BaseGDL* Objptr, EnvUDT* e)
  {
    if( Objptr == 0 || Objptr->Type() != GDL_OBJ)
      ThrowFromInternalUDSub( e, "Objptr not of type OBJECT. Please report.");
    if( !Objptr->Scalar())
      ThrowFromInternalUDSub( e, "Objptr must be a scalar. Please report.");
    DObjGDL* Object = static_cast<DObjGDL*>( Objptr);
    DObj ID = (*Object)[0];
    try {
      return BaseGDL::interpreter->GetObjHeap( ID);
    }
    catch( GDLInterpreter::HeapException& hEx)
    {
      ThrowFromInternalUDSub( e, "Object ID <"+i2s(ID)+"> not found.");      
    }

    assert(false);
    return NULL;
  }
#include <stdio.h>
#include <expat.h>

#ifdef XML_LARGE_SIZE
# if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#  define XML_FMT_INT_MOD "I64"
# else
#  define XML_FMT_INT_MOD "ll"
# endif
#else
# define XML_FMT_INT_MOD "l"
#endif

#ifdef XML_UNICODE_WCHAR_T
# include <wchar.h>
# define XML_FMT_STR "ls"
#else
# define XML_FMT_STR "s"
#endif

typedef struct gdlSaxXmlLocator {
  EnvUDT* e;
  int depth;
//  DByte namespace_prefixes;
//  DLong parser_location[2];
//  DString* public_idP;
//  DString* parser_uriP;
// DInt schema_checkingP;
// DInt validation_modeP;
  DString* fileP;
} AppData;
#define gdlXMLSAXSTARTPROCEDURE(_METHOD)  \
  gdlSaxXmlLocator *s = (gdlSaxXmlLocator*) data; \
  EnvUDT* e = s->e; \
  DObjGDL* myObj = static_cast<DObjGDL*> (e->GetParDefined(0)); \
  DString meth=_METHOD;\
  DPro* method = GetOBJ(myObj, e)->Desc()->GetPro(meth);\
  if (method == NULL) e->Throw("Method not found: "+meth); \
  StackGuard<EnvStackT> guard(e->Interpreter()->CallStack()); \
  e->PushNewEmptyEnvUD(method, (DObjGDL**) & myObj); \
  EnvT* curenv = (EnvT*) (e->Interpreter()->CallStackBack());

static void XMLCALL eldecl (void *data,
                            const XML_Char *name,
                            XML_Content *model){
  gdlXMLSAXSTARTPROCEDURE("ELEMENTDECL");
  int par = 1;
  DStringGDL* ret;

  if (name) ret=new DStringGDL(std::string(name)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;

  if (model->name) ret=new DStringGDL(std::string(model->name)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;

  curenv->Interpreter()->call_pro(method->GetTree());
}
static void XMLCALL attdecl (void *data,
                                    const XML_Char  *elname,
                                    const XML_Char  *attname,
                                    const XML_Char  *att_type,
                                    const XML_Char  *dflt,
                                    int              isrequired){
  gdlXMLSAXSTARTPROCEDURE("ATTRIBUTEDECL");
  int par = 1;
  DStringGDL* ret;

  if (elname) ret=new DStringGDL(std::string(elname)); else  ret=new DStringGDL(""); //eNAME
  if (method->NPar() > par) curenv->GetPar(par++) = ret;

  if (attname) ret=new DStringGDL(std::string(attname)); else  ret=new DStringGDL(""); //aNAME
  if (method->NPar() > par) curenv->GetPar(par++) = ret;

  if (att_type) ret=new DStringGDL(std::string(att_type)); else  ret=new DStringGDL(""); //TYPE
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
// The "isrequired" parameter will be true and the default
//   value will be NULL in the case of "#REQUIRED". If "isrequired" is
//   true and default is non-NULL, then this is a "#FIXED" default.
  if (isrequired) {
    if (!dflt) ret=new DStringGDL("#REQUIRED"); else ret=new DStringGDL("#FIXED"); //MODE
  } else ret=new DStringGDL("#IMPLIED"); //? sure ?
  if (method->NPar() > par) curenv->GetPar(par++) = ret;

  if (dflt) ret=new DStringGDL(std::string(dflt)); else  ret=new DStringGDL(""); //VALUE
  if (method->NPar() > par) curenv->GetPar(par++) = ret;

  curenv->Interpreter()->call_pro(method->GetTree());
}

static void XMLCALL entdecl (void *data,
                              const XML_Char *entityName,
                              int is_parameter_entity,
                              const XML_Char *value,
                              int value_length,
                              const XML_Char *base,
                              const XML_Char *systemId,
                              const XML_Char *publicId,
                              const XML_Char *notationName){
  gdlXMLSAXSTARTPROCEDURE("STARTENTITY");
  int par = 1;
  DStringGDL* ret;

  if (entityName) ret=new DStringGDL(std::string(entityName)); else  ret=new DStringGDL(""); //NAME
  if (method->NPar() > par) curenv->GetPar(par++) = ret;

  curenv->Interpreter()->call_pro(method->GetTree());
}


static void XMLCALL
startElement(void *data, const XML_Char *name, const XML_Char **atts)
{
  gdlXMLSAXSTARTPROCEDURE("STARTELEMENT");

  s->depth += 1;

  int natts = 0;
  for (int i = 0; atts[i]; i += 2) natts++;
  //Pro->npar defined in object may be < number given in doc and in overload.cpp
  //Note: there should be a simpler way to change the values of the call variables...
  int par = 1;
  if (method->NPar() > par) curenv->GetPar(par++) = new DStringGDL(""); //URI
  if (method->NPar() > par) curenv->GetPar(par++) = new DStringGDL(""); // LOCAL, empty if no URI
  if (method->NPar() > par) curenv->GetPar(par++) = new DStringGDL(name);
  if (method->NPar() > par & natts > 0) {
    DStringGDL* ret = new DStringGDL(dimension(natts));
    int j = 0;
    for (int i = 0; atts[i]; i += 2) (*ret)[j] = atts[i];
    curenv->GetPar(par++) = ret;
  }
  if (method->NPar() > par & natts > 0) {
    DStringGDL* ret = new DStringGDL(dimension(natts));
    int j = 0;
    for (int i = 0; atts[i]; i += 2) (*ret)[j] = atts[i + 1];
    curenv->GetPar(par++) = ret;
  }
  curenv->Interpreter()->call_pro(method->GetTree());
}

static void XMLCALL
endElement(void *data, const XML_Char *name)
{
  gdlXMLSAXSTARTPROCEDURE("ENDELEMENT");

  s->depth -= 1;
  int par = 1;
  if (method->NPar() > par) curenv->GetPar(par++) = new DStringGDL(""); //URI
  if (method->NPar() > par) curenv->GetPar(par++) = new DStringGDL(""); // LOCAL, empty if no URI
  if (method->NPar() > par) curenv->GetPar(par++) = new DStringGDL(name);
  curenv->Interpreter()->call_pro(method->GetTree());
}

 static void XMLCALL
 characters(void *data, const XML_Char *chars, int len){
  gdlXMLSAXSTARTPROCEDURE("CHARACTERS");

  int par = 1;
  DStringGDL* ret=new DStringGDL(std::string(chars,len));
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  curenv->Interpreter()->call_pro(method->GetTree());
 }
 
static void XMLCALL handle_comment(void *data, const XML_Char *comment) {
 gdlXMLSAXSTARTPROCEDURE("COMMENT");

  int par = 1;
  DStringGDL* ret=new DStringGDL(std::string(comment));
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  curenv->Interpreter()->call_pro(method->GetTree());
}

static void XMLCALL startPrefixMapping(void *data,
      const XML_Char *prefix,
      const XML_Char *uri) {
 gdlXMLSAXSTARTPROCEDURE("STARTPREFIXMAPPING");

  int par = 1;
  DStringGDL* ret;
  if (prefix) ret=new DStringGDL(std::string(prefix)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  if (uri) ret=new DStringGDL(std::string(uri)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  curenv->Interpreter()->call_pro(method->GetTree());
}

static void XMLCALL endPrefixMapping(void *data, const XML_Char *prefix) {
 gdlXMLSAXSTARTPROCEDURE("ENDPREFIXMAPPING");

  int par = 1;
  DStringGDL* ret;
  if (prefix) ret=new DStringGDL(std::string(prefix)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  curenv->Interpreter()->call_pro(method->GetTree());
} 

static void XMLCALL  startcdatasectionhandler(void *data){
 gdlXMLSAXSTARTPROCEDURE("STARTCDATA"); 
 curenv->Interpreter()->call_pro(method->GetTree());
}
static void XMLCALL endcdatasectionhandler(void *data){
 gdlXMLSAXSTARTPROCEDURE("ENDCDATA"); 
 curenv->Interpreter()->call_pro(method->GetTree());
}
static void XMLCALL notationdeclhandler(void *data, const XML_Char *notationName,
                           const XML_Char *base,
                           const XML_Char *systemId,
                           const XML_Char *publicId){
  gdlXMLSAXSTARTPROCEDURE("NOTATIONDECL"); 
  int par = 1;
  DStringGDL* ret;
  if (notationName) ret=new DStringGDL(std::string(notationName)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  if (publicId) ret=new DStringGDL(std::string(publicId)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  if (systemId) ret=new DStringGDL(std::string(systemId)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  curenv->Interpreter()->call_pro(method->GetTree());
}

//pretty unuseful with EXPAT as there is no endDTD handler...
static int  XMLCALL  dtdhandler(XML_Parser parser,
                                const XML_Char *context,
                                const XML_Char *base,
                                const XML_Char *systemId,
                                const XML_Char *publicId){
  gdlSaxXmlLocator *s = (gdlSaxXmlLocator *) XML_GetUserData(parser);
  EnvUDT* e = s->e;
  DObjGDL* myObj = static_cast<DObjGDL*> (e->GetParDefined(0)); 
  DString meth="STARTDTD";
  DPro* method = GetOBJ(myObj, e)->Desc()->GetPro(meth);\
  if (method == NULL) e->Throw("Method not found: "+meth); \
  StackGuard<EnvStackT> guard(e->Interpreter()->CallStack()); \
  e->PushNewEmptyEnvUD(method, (DObjGDL**) & myObj); \
  EnvT* curenv = (EnvT*) (e->Interpreter()->CallStackBack());

  int par = 1;
  DStringGDL* ret;
  if (context) ret=new DStringGDL(std::string(context)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  if (systemId) ret=new DStringGDL(std::string(systemId)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  if (publicId) ret=new DStringGDL(std::string(publicId)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  e->Interpreter()->call_pro(method->GetTree());
  return XML_STATUS_OK; //probably too optimistic!
}

static void XMLCALL unparsedentityhandler(void *data, const XML_Char *entityName,
                           const XML_Char *base,
                           const XML_Char *systemId,
                           const XML_Char *publicId,
                           const XML_Char *notationName){
  gdlXMLSAXSTARTPROCEDURE("SKIPPEDENTITY"); 
  int par = 1;
  DStringGDL* ret;
  if (entityName) ret=new DStringGDL(std::string(entityName)); else  ret=new DStringGDL("");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  e->Interpreter()->call_pro(method->GetTree());
}

static void handleStartDocument(EnvUDT* e) {
  DObjGDL* myObj = static_cast<DObjGDL*> (e->GetParDefined(0)); 
  DString meth="STARTDOCUMENT";
  DPro* method = GetOBJ(myObj, e)->Desc()->GetPro(meth);
  if (method == NULL) return; 
  e->Interpreter()->call_pro(method->GetTree());
}
static void handleEndDocument(EnvUDT* e) {
  DObjGDL* myObj = static_cast<DObjGDL*> (e->GetParDefined(0)); 
  DString meth="ENDDOCUMENT";
  DPro* method = GetOBJ(myObj, e)->Desc()->GetPro(meth);
  if (method == NULL) return; 
  e->Interpreter()->call_pro(method->GetTree());
}
static void handleFatalError(EnvUDT* e, XML_Parser parser) {
  DObjGDL* myObj = static_cast<DObjGDL*> (e->GetParDefined(0)); 
  DString meth="FATALERROR";
  DPro* method = GetOBJ(myObj, e)->Desc()->GetPro(meth);
  if (method == NULL) return; 
  EnvT* curenv = (EnvT*) (e->Interpreter()->CallStackBack());
  int par = 1;
  DStringGDL* ret;
  ret=new DStringGDL("UnknownSystemID");
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  ret=new DStringGDL(i2s(XML_GetCurrentLineNumber(parser)));
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  ret=new DStringGDL(i2s(XML_GetCurrentColumnNumber(parser)));
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
  ret=new DStringGDL(XML_ErrorString(XML_GetErrorCode(parser)));
  if (method->NPar() > par) curenv->GetPar(par++) = ret;
        
        
 e->Interpreter()->call_pro(method->GetTree());
}
gdlSaxXmlLocator *
createGdlSaxXmlLocator(EnvUDT* e) {
  gdlSaxXmlLocator *ret;
  ret = (gdlSaxXmlLocator *) malloc(sizeof(gdlSaxXmlLocator));
  if (ret == NULL) Message( "internal error, please report.");
  /* Initialize */
  ret->e=e;
  ret->depth=0;
  ret->fileP=NULL;
  return ret;
}  /* End */


namespace lib {
  //needs to return 1, as it is called, as IDLffXMLSAX is a Base class object not a gdl_object.
  BaseGDL* GDLffXmlSax___Init(EnvUDT* e)
  {
    return new DLongGDL(1);
  }

  void GDLffXmlSax___Cleanup(EnvUDT * e)
  {
//    std::cerr << "TODO Cleanup" << std::endl;
  }

  void GDLffXmlSax__ParseFile(EnvUDT* e)
  {
    DObjGDL* myObj = static_cast<DObjGDL*> (e->GetParDefined(0));
    if (myObj==NULL) e->Throw("No Object");
    DStructGDL* objStruct = GetOBJ(myObj,e);

    //
    //if we are the IDLffXMLSAX container object, return immediately, as it is not supposed to do anything by itself.
    //it must be overloaded: saves time and the planet.
    //   
    if (objStruct->Desc()->Name() == "IDLFFXMLSAX") return;
    //    

    BaseGDL* filename = e->GetParDefined(1);
    FILE *f;
    DString file;
    
    if (filename != NULL && filename->Type() == GDL_STRING) { //filename
      file = (*static_cast<DStringGDL*> (filename))[0];
      f = fopen(file.c_str(), "rb");
      if (f == NULL) e->Throw("file " + file + " does not exist.");
    } else {e->Throw("No xml file provided.");}
    
    gdlSaxXmlLocator *data = createGdlSaxXmlLocator(e);
    data->fileP=new std::string(file.c_str());
    XML_Parser parser = XML_ParserCreateNS(NULL, '|');
    if (!parser) e->Throw("Couldn't allocate memory for parser\n");
    objStruct->InitTag("_XML_PARSER", DLong64GDL((DLong64) parser));

    char buf[256];

    XML_SetParamEntityParsing(parser, XML_PARAM_ENTITY_PARSING_ALWAYS);

    XML_SetUserData(parser, data);

    //can we call only the handlers overloaded in the user-written object?
    XML_SetAttlistDeclHandler(parser,attdecl);
    XML_SetElementDeclHandler(parser,eldecl);
    XML_SetEntityDeclHandler(parser,entdecl);   
    XML_SetNamespaceDeclHandler(parser, startPrefixMapping, endPrefixMapping);
    XML_SetCharacterDataHandler(parser, characters);
    XML_SetElementHandler(parser, startElement, endElement);
    XML_SetCommentHandler(parser, handle_comment);
    XML_SetCdataSectionHandler(parser, startcdatasectionhandler, endcdatasectionhandler);
    XML_SetExternalEntityRefHandler(parser, dtdhandler);
    XML_SetNotationDeclHandler(parser, notationdeclhandler);
    XML_SetUnparsedEntityDeclHandler( parser, unparsedentityhandler);
    
    // this is used for stop_parsing
    DIntGDL* halt_processing = static_cast<DIntGDL*> (objStruct->GetTag(objStruct->Desc()->TagIndex("HALT_PROCESSING")));

    int done;
    handleStartDocument(e);
    do {
      // stop parsing sent
      if ((*halt_processing)[0] == 1) {
        handleEndDocument(e);
        XML_ParserFree(parser);
        return;
      }

      size_t len = fread(buf, 1, sizeof (buf), f);
      done = len < sizeof (buf);
      if (XML_Parse(parser, buf, (int) len, done) == XML_STATUS_ERROR) {
        handleFatalError(e,parser); //only fatal errors at the moment.
      }

    } while (!done);
    handleEndDocument(e);
    XML_ParserFree(parser);
  }

  void GDLffXmlSax__Characters(EnvUDT* e)
  {
  }

  void GDLffXmlSax__AttributeDecl(EnvUDT* e)
  {
  }

  void GDLffXmlSax__Cleanup(EnvUDT* e)
  {
  }

  void GDLffXmlSax__Comment(EnvUDT* e)
  {
  }

  void GDLffXmlSax__ElementDecl(EnvUDT* e)
  {
  }

  void GDLffXmlSax__EndCDATA(EnvUDT* e)
  {
  }

  void GDLffXmlSax__EndDocument(EnvUDT* e)
  {
  }

  void GDLffXmlSax__EndDTD(EnvUDT* e)
  {
  }

  void GDLffXmlSax__EndElement(EnvUDT* e)
  {
  }

  void GDLffXmlSax__EndEntity(EnvUDT* e)
  {
    std::cerr << "GDLffXmlSax__EndEntity" << " unavailable with EXPAT. FIXME." << std::endl;
  }

  void GDLffXmlSax__EndPrefixMapping(EnvUDT* e)
  {
  }

  void GDLffXmlSax__Error(EnvUDT* e)
  {
  }

  void GDLffXmlSax__ExternalEntityDecl(EnvUDT* e)
  {
    std::cerr << "GDLffXmlSax__ExternalEntityDecl" << " unavailable with EXPAT. FIXME." << std::endl;
  }

  void GDLffXmlSax__FatalError(EnvUDT* e)
  {
    DObjGDL* myObj = static_cast<DObjGDL*> (e->GetParDefined(0));
    DStructGDL* objStruct = GetOBJ(myObj, e);
    DLong64GDL* gdlparser = static_cast<DLong64GDL*> (objStruct->GetTag(objStruct->Desc()->TagIndex("_XML_PARSER")));
    XML_Parser parser = (XML_Parser) ((*gdlparser)[0]);
    if (parser) {
      gdlSaxXmlLocator *data = (gdlSaxXmlLocator *) XML_GetUserData(parser);

      e->Throw("Parser SAX fatal error: File: " + *(data->fileP) + ", line: " +
            i2s(XML_GetCurrentLineNumber(parser)) + ", column: " +
            i2s(XML_GetCurrentColumnNumber(parser)) +
            " :: " + XML_ErrorString(XML_GetErrorCode(parser)));
    }
  }

  void GDLffXmlSax__GetProperty(EnvUDT* e)
  {
    DObjGDL* myObj = static_cast<DObjGDL*> (e->GetParDefined(0));
    DStructGDL* objStruct = GetOBJ(myObj, e);
    DLong64GDL* gdlparser = static_cast<DLong64GDL*> (objStruct->GetTag(objStruct->Desc()->TagIndex("_XML_PARSER")));

    static int FILENAME = e->GetKeywordIx("FILENAME");
    static int NAMESPACE_PREFIXES = e->GetKeywordIx("NAMESPACE_PREFIXES");
    static int PARSER_LOCATION = e->GetKeywordIx("PARSER_LOCATION");
    static int PARSER_PUBLICID = e->GetKeywordIx("PARSER_PUBLICID");
    static int PARSER_URI = e->GetKeywordIx("PARSER_URI");
    static int SCHEMA_CHECKING = e->GetKeywordIx("SCHEMA_CHECKING");
    static int VALIDATION_MODE = e->GetKeywordIx("VALIDATION_MODE");

    DIntGDL* val = new DIntGDL(dimension(2));
    DStringGDL* s;
    DIntGDL* zero=new DIntGDL(0);

    XML_Parser parser = (XML_Parser) ((*gdlparser)[0]);
    if (parser) {
      gdlSaxXmlLocator *data = (gdlSaxXmlLocator *) XML_GetUserData(parser);
      s=new DStringGDL(*(data->fileP));
      (*val)[0] = XML_GetCurrentLineNumber(parser);
      (*val)[1] = XML_GetCurrentColumnNumber(parser);
    } else s=new DStringGDL(""); 

    if (e->KeywordPresent(FILENAME)) e->SetKW(FILENAME, s);
    if (e->KeywordPresent(NAMESPACE_PREFIXES)) e->SetKW(NAMESPACE_PREFIXES, zero);
    if (e->KeywordPresent(PARSER_LOCATION)) e->SetKW(PARSER_LOCATION, val);
    if (e->KeywordPresent(PARSER_PUBLICID)) e->SetKW(PARSER_PUBLICID, s);
    if (e->KeywordPresent(PARSER_URI)) e->SetKW(PARSER_URI, s);
    if (e->KeywordPresent(SCHEMA_CHECKING)) e->SetKW(SCHEMA_CHECKING, zero);
    if (e->KeywordPresent(VALIDATION_MODE)) e->SetKW(VALIDATION_MODE, zero);

  }

  void GDLffXmlSax__IgnorableWhitespace(EnvUDT* e)
  {
    std::cerr << "GDLffXmlSax__IgnorableWhitespace"<<" unavailable with EXPAT. FIXME."<<std::endl;
  }

  void GDLffXmlSax__InternalEntityDecl(EnvUDT* e)
  {
    std::cerr << "GDLffXmlSax__InternalEntityDecl" << " unavailable with EXPAT. FIXME." << std::endl;
  }

  void GDLffXmlSax__NotationDecl(EnvUDT* e)
  {
  }

  void GDLffXmlSax__ProcessingInstruction(EnvUDT* e)
  {
    std::cerr << "GDLffXmlSax__ProcessingInstruction" << " unavailable with EXPAT. FIXME." << std::endl;
  }

  void GDLffXmlSax__SetProperty(EnvUDT* e)
  {
    DObjGDL* myObj = static_cast<DObjGDL*> (e->GetParDefined(0));
    DStructGDL* objStruct = GetOBJ(myObj, e);
    DLong64GDL* gdlparser = static_cast<DLong64GDL*> (objStruct->GetTag(objStruct->Desc()->TagIndex("_XML_PARSER")));

    static int NAMESPACE_PREFIXES = e->GetKeywordIx("NAMESPACE_PREFIXES");
    static int SCHEMA_CHECKING = e->GetKeywordIx("SCHEMA_CHECKING");
    static int VALIDATION_MODE = e->GetKeywordIx("VALIDATION_MODE");

    BaseGDL* dummy;
    if (e->KeywordPresent(NAMESPACE_PREFIXES)) dummy=e->GetKW(NAMESPACE_PREFIXES);
    if (e->KeywordPresent(SCHEMA_CHECKING)) e->GetKW(SCHEMA_CHECKING);
    if (e->KeywordPresent(VALIDATION_MODE)) e->GetKW(VALIDATION_MODE);
    XML_Parser parser = (XML_Parser) ((*gdlparser)[0]);
    if (parser) { //do something useful!
    } 
  }

  void GDLffXmlSax__SkippedEntity(EnvUDT* e)
  {
  }

  void GDLffXmlSax__StartCDATA(EnvUDT* e)
  {
  }

  void GDLffXmlSax__StartDocument(EnvUDT* e)
  {
  }

  void GDLffXmlSax__StartDTD(EnvUDT* e)
  {
  }

  void GDLffXmlSax__StartElement(EnvUDT* e)
  {
  }

  void GDLffXmlSax__StartEntity(EnvUDT* e)
  {
  }

  void GDLffXmlSax__StartPrefixmapping(EnvUDT* e)
  {
  }

  void GDLffXmlSax__StopParsing(EnvUDT* e)
  {
  //  std::cerr << "GDLffXmlSax__StopParsing" << " called." << std::endl;
    DStructGDL* objStruct=GetOBJ(e->GetParDefined(0), e);
    DIntGDL* halt_processing=static_cast<DIntGDL*>(objStruct->GetTag(objStruct->Desc()->TagIndex("HALT_PROCESSING")));
    (*halt_processing)[0]=1;
  }

  void GDLffXmlSax__UnparsedEntityDecl(EnvUDT* e)
  {
  }

  void GDLffXmlSax__Warning(EnvUDT* e)
  {
    std::cerr << "GDLffXmlSax__Warning" << " called." << std::endl;
  }

}
