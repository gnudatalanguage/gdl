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

#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include "shapefil.h"

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
  
namespace lib {

  BaseGDL* GDLffShape___Init(EnvUDT* e)
  {
    SizeT nParam = e->NParam(1); //for (int i=0; i< nParam; ++i) std::cerr<<e->GetParString(i)<<std::endl;

    if (nParam > 1) {
      DStructGDL* self = GetOBJ(e->GetParDefined(0), e);
      BaseGDL* filename = e->GetParDefined(1);
      if (filename != NULL && filename->Type() == GDL_STRING) { //filename
        SHPHandle shph;
        const char* access = "rb";
        DString f = (*static_cast<DStringGDL*> (filename))[0];
        shph = SHPOpen(f.c_str(), access);
        DBFHandle dbfh;
        dbfh = DBFOpen(f.c_str(), access);

        if (shph==NULL || dbfh==NULL) { //which is NOT allowed!!!
          return new DLongGDL(0); //undefines object! marvelous!
        }

        int nShapeType, nEntities;
        double adfMinBound[4], adfMaxBound[4];
        SHPGetInfo(shph, &nEntities, &nShapeType, adfMinBound, adfMaxBound);
        
        self->InitTag("FILENAME", *filename);
        self->InitTag("SHAPEHANDLE", DLong64GDL((DLong64) shph));
        self->InitTag("ISOPEN", DIntGDL(1));
        self->InitTag("DBFHANDLE", DLong64GDL((DLong64) dbfh));
        self->InitTag("SHPTYPE", DIntGDL(nShapeType));
      }
    }
    return new DLongGDL(1);
  }

  BaseGDL* GDLffShape___Open(EnvUDT* e)
  {
    SizeT nParam = e->NParam(1);
    if (nParam < 2) e->Throw("No filename given.");
    DStructGDL* self = GetOBJ(e->GetParDefined(0), e);
    BaseGDL* isopen = self->GetTag(self->Desc()->TagIndex("ISOPEN"));
    if ((*static_cast<DIntGDL*> (isopen))[0] == 1) e->Throw("Shapefile already open.");
    BaseGDL* filename = e->GetParDefined(1);
    if (filename != NULL && filename->Type() == GDL_STRING) { //filename
        SHPHandle shph;
        const char* access = "rb";
        DString f = (*static_cast<DStringGDL*> (filename))[0];
        shph = SHPOpen(f.c_str(), access);
        DBFHandle dbfh;
        dbfh = DBFOpen(f.c_str(), access);

        if (shph==NULL || dbfh==NULL) { //which is NOT allowed!!!
          return new DLongGDL(0); //undefines object! marvelous!
        }

        int nShapeType, nEntities;
        double adfMinBound[4], adfMaxBound[4];
        SHPGetInfo(shph, &nEntities, &nShapeType, adfMinBound, adfMaxBound);
        
        self->InitTag("FILENAME", *filename);
        self->InitTag("SHAPEHANDLE", DLong64GDL((DLong64) shph));
        self->InitTag("ISOPEN", DIntGDL(1));
        self->InitTag("DBFHANDLE", DLong64GDL((DLong64) dbfh));
        self->InitTag("SHPTYPE", DIntGDL(nShapeType));
    }
    return new DLongGDL(1);
  }

  void GDLffShape___GetProperty(EnvUDT* e)
  {

    static int ATTRIBUTE_INFO = e->GetKeywordIx("ATTRIBUTE_INFO"); //DBF
    static int ATTRIBUTE_NAMES = e->GetKeywordIx("ATTRIBUTE_NAMES"); //DBF
    static int ENTITY_TYPE = e->GetKeywordIx("ENTITY_TYPE"); //SHP
    static int FILENAME = e->GetKeywordIx("FILENAME"); //simple
    static int IS_OPEN = e->GetKeywordIx("IS_OPEN"); //simple
    static int N_ATTRIBUTES = e->GetKeywordIx("N_ATTRIBUTES"); //DBF
    static int N_ENTITIES = e->GetKeywordIx("N_ENTITIES"); //SHP
    static int N_RECORDS = e->GetKeywordIx("N_RECORDS"); //DBF but also SHP
    SHPHandle shph;
    DBFHandle dbfh;
    int fieldCount=0;
    int recordCount=0;
    
    int nShapeType = -1;
    int nEntities = 0;
    double adfMinBound[4], adfMaxBound[4];
    SizeT nParam = e->NParam(1);

    DStructGDL* self = GetOBJ(e->GetParDefined(0), e);
    BaseGDL* open = self->GetTag(self->Desc()->TagIndex("ISOPEN"));
    bool isopen = ((*static_cast<DIntGDL*> (open))[0] == 1);
    if (e->KeywordPresent(IS_OPEN)) {
      e->SetKW(IS_OPEN, new DIntGDL(isopen));
    }
    if (e->KeywordPresent(FILENAME)) {
      e->SetKW(FILENAME, (self->GetTag(self->Desc()->TagIndex("FILENAME")))->Dup() );
    }
    bool has_shph = false;
    bool has_dbfh = false;
    if (isopen) {
      BaseGDL* handleGDL = self->GetTag(self->Desc()->TagIndex("SHAPEHANDLE"));
      shph = (SHPHandle) ((*static_cast<DLong64GDL*> (handleGDL))[0]);
      has_shph = (shph != NULL);
      BaseGDL* dbfhGDL = self->GetTag(self->Desc()->TagIndex("DBFHANDLE"));
      dbfh = (DBFHandle) ((*static_cast<DLong64GDL*> (dbfhGDL))[0]);
      has_dbfh = (dbfh != NULL);
    }
    if (has_shph) { //read all related info and fill corresp kw:
      SHPGetInfo(shph, &nEntities, &nShapeType, adfMinBound, adfMaxBound);
    }
    if (has_dbfh) {
      recordCount=DBFGetRecordCount( dbfh );
      fieldCount=DBFGetFieldCount( dbfh );
    }

    if (e->KeywordPresent(N_ENTITIES)) {
      e->SetKW(N_ENTITIES, new DLongGDL(nEntities));
    }
    if (e->KeywordPresent(N_RECORDS)) { //should really be the number of records in the dBASE table (.dbf) component of the Shapefile. see below.
      e->SetKW(N_RECORDS, new DIntGDL(nEntities));
    }
    if (e->KeywordPresent(ENTITY_TYPE)) {
      e->SetKW(ENTITY_TYPE, new DIntGDL(nShapeType));
    }
    if (e->KeywordPresent(N_RECORDS)) { //now really the number of records in the dBASE table (.dbf) component of the Shapefile.
      e->SetKW(N_RECORDS, new DIntGDL(recordCount));
    }
    if (e->KeywordPresent(N_ATTRIBUTES)) { //now really the number of records in the dBASE table (.dbf) component of the Shapefile.
      e->SetKW(N_ATTRIBUTES, new DIntGDL(fieldCount));
    }
    if (e->KeywordPresent(ATTRIBUTE_NAMES)) {
      if (has_dbfh) {
        //Attribute_names
        DStringGDL* attr_names=new DStringGDL(fieldCount);
        char name[12];
        for (int i=0; i< fieldCount; ++i) {
          DBFGetFieldInfo( dbfh, i, name,NULL,NULL);
          (*attr_names)[i].append(name);
        }
        e->SetKW(ATTRIBUTE_NAMES, attr_names);
      } else e->Throw("No attributes exist for this shapefile.");
    }
    if (e->KeywordPresent(ATTRIBUTE_INFO)) {
      if (has_dbfh) {
      DStructDesc* desc=DStructGDL( "IDL_SHAPE_ATTRIBUTE").Desc();
      DStructGDL*  info = new DStructGDL( desc, dimension(fieldCount));
      long returned_type[8]={7,2,5,1,0,0,0,0}; //three last dummy values to avoid troubles and be warned by users if API changes. 
        DBFFieldType ret;
        char name[12];
        int width;
        int ndec;
        for (int i=0; i< fieldCount; ++i) {
          ret = DBFGetFieldInfo( dbfh, i, name,&width,&ndec);
          (*static_cast<DStringGDL*>(info->GetTag(info->Desc()->TagIndex("NAME"),i)))[0].append(name);
          (*static_cast<DLongGDL*>(info->GetTag(info->Desc()->TagIndex("WIDTH"),i)))[0]=width;
          (*static_cast<DLongGDL*>(info->GetTag(info->Desc()->TagIndex("PRECISION"),i)))[0]=ndec;
          (*static_cast<DLongGDL*>(info->GetTag(info->Desc()->TagIndex("TYPE"),i)))[0]=returned_type[ret];
        }
        e->SetKW(ATTRIBUTE_INFO, info);
      } else e->Throw("No attributes exist for this shapefile.");
    }
  }

  BaseGDL* GDLffShape___GetEntity(EnvUDT* e)
  {
    static int ATTRIBUTES = e->GetKeywordIx("ATTRIBUTES"); //DBF
    static int ALL = e->GetKeywordIx("ALL"); //DBF
    SHPHandle shph;
    DBFHandle dbfh;
    int fieldCount = 0;
    int *attribute_type=NULL;

    int nShapeType = 0;
    int nEntities = 1;
    int entity = 0;
    int offset = 0;
    double adfMinBound[4], adfMaxBound[4];
    SizeT nParam = e->NParam(1);

    DStructGDL* self = GetOBJ(e->GetParDefined(0), e);
    BaseGDL* open = self->GetTag(self->Desc()->TagIndex("ISOPEN"));
    if ((*static_cast<DIntGDL*> (open))[0] != 1) e->Throw("A shapefile is not currently open.");
    //if it is open, then shph and dbfh ARE defined.
    bool doAll = (e->KeywordSet(ALL));
    bool doAttr = (e->KeywordSet(ATTRIBUTES));
    if (!doAll && nParam > 1) {
      BaseGDL* p1 = (e->GetParDefined(1))->Convert2(GDL_INT, BaseGDL::CONVERT);
      if (p1 != NULL) entity = (*static_cast<DIntGDL*> (p1))[0];
    }
    bool has_shph = false;
    bool has_dbfh = false;
    BaseGDL* handleGDL = self->GetTag(self->Desc()->TagIndex("SHAPEHANDLE"));
    shph = (SHPHandle) ((*static_cast<DLong64GDL*> (handleGDL))[0]);
    has_shph = (shph != NULL);
    if (!has_shph) e->Throw(".sph file absent (?).");

    BaseGDL* dbfhGDL = self->GetTag(self->Desc()->TagIndex("DBFHANDLE"));
    dbfh = (DBFHandle) ((*static_cast<DLong64GDL*> (dbfhGDL))[0]);
    has_dbfh = (dbfh != NULL);
    if (!has_dbfh) e->Throw(".dbh file absent (?).");

    SHPGetInfo(shph, &nEntities, &nShapeType, adfMinBound, adfMaxBound);
    fieldCount = DBFGetFieldCount(dbfh);
    if (fieldCount<1) e->Throw("empty .dbh file.");
    long returned_type[8] = {7, 2, 5, 1, 0, 0, 0, 0}; //three last dummy values to avoid troubles and be warned by users if API changes. 
    attribute_type=(int*)malloc(fieldCount*sizeof(int));
    DBFFieldType ret;
    char name[12];
    int width;
    int ndec;
    for (int i = 0; i < fieldCount; ++i) {
      ret = DBFGetFieldInfo(dbfh, i, name, &width, &ndec);
      attribute_type[i]=returned_type[ret]; //will serve as case switch for reading attributes below.
    }
    //define an ATTR desc here if /ATTR:
    DStructDesc* attr_desc;
    if (doAttr) {
      DString s = "ATTRIBUTE_";
      attr_desc = new DStructDesc("$truct");
      SpDLong aLong;
      SpDString aString;
      SpDByte aByte;
      SpDDouble aDouble;
      for (int j = 0; j < fieldCount; ++j) {
        DString title = s + i2s(j);
        switch (attribute_type[j]) {
        case 7:
          attr_desc->AddTag(title, &aString);
          break;
        case 2:
          attr_desc->AddTag(title, &aLong);
          break;
        case 5:
          attr_desc->AddTag(title, &aDouble);
          break;
        case 1:
          attr_desc->AddTag(title, &aByte);
          break;
        default:
          attr_desc->AddTag(title, &aLong);
        }
      }
    }
    
    if (doAll) {
      entity = 0;
    }//start=0; end:nEntities-1 
    else {
      if (entity >= nEntities) e->Throw("Index value out of range.");
      nEntities = 1;
      offset = entity;
    }

    DStructDesc* desc = DStructGDL("IDL_SHAPE_ENTITY").Desc();
    DStructGDL* entities = new DStructGDL(desc, dimension(nEntities));
    for (int i = 0; i < nEntities; ++i) {
      SHPObject *ret = SHPReadObject(shph, i + offset);
      DLong n = ret->nVertices;
      if (n<1) continue;
      (*static_cast<DLongGDL*> (entities->GetTag(entities->Desc()->TagIndex("SHAPE_TYPE"), i)))[0] = ret->nSHPType;
      (*static_cast<DLongGDL*> (entities->GetTag(entities->Desc()->TagIndex("ISHAPE"), i)))[0] = ret->nShapeId;
      DDouble* bounds = &(*static_cast<DDoubleGDL*> (entities->GetTag(entities->Desc()->TagIndex("BOUNDS"), i)))[0]; //value: just fill
      bounds[0] = ret->dfXMin;
      bounds[1] = ret->dfYMin;
      bounds[2] = ret->dfZMin;
      bounds[3] = ret->dfMMin;
      bounds[4] = ret->dfXMax;
      bounds[5] = ret->dfYMax;
      bounds[6] = ret->dfZMax;
      bounds[7] = ret->dfMMax;


      (*static_cast<DLongGDL*> (entities->GetTag(entities->Desc()->TagIndex("N_VERTICES"), i)))[0] = n;
      //pointer stuff
      int dim2 = 3;
      bool doMeasure = false;
      bool doPartsType = false;
      switch (ret->nSHPType) {
      case SHPT_POINT:
      case SHPT_ARC:
      case SHPT_POLYGON:
      case SHPT_MULTIPOINT:
        dim2 = 2;
        break;
      case SHPT_POINTM:
      case SHPT_ARCM:
      case SHPT_POLYGONM:
        dim2 = 2;
        doMeasure = true;
        break;
      case SHPT_MULTIPOINTM:
        dim2 = 2;
        doMeasure = false; //see IDL doc. Values should be in Bounds already (?) --- FIXME! ?
      case SHPT_MULTIPATCH:
        dim2 = 3;
        doMeasure = true;
        doPartsType = true;
      }
      SizeT dims[2];
      dims[0] = n;
      dims[1] = dim2;
      dimension dim(dims, 2);
      DDoubleGDL* vertices = new DDoubleGDL(dim);
      memcpy(&((*vertices)[0]), ret->padfX, n * sizeof (DDouble));
      memcpy(&((*vertices)[n]), ret->padfY, n * sizeof (DDouble));
      if (dim2 > 2) memcpy(&((*vertices)[2 * n]), ret->padfZ, n * sizeof (DDouble));
      DPtrGDL* ptr = static_cast<DPtrGDL*> (entities->GetTag(entities->Desc()->TagIndex("VERTICES"), i));
      DPtr heapID = e->NewHeap(1, vertices->Transpose(0));
      (*ptr)[0] = heapID;
      GDLDelete(vertices);
      if (doMeasure) {
        DDoubleGDL* measure = new DDoubleGDL(n);
        memcpy(&((*measure)[0]), ret->padfM, n * sizeof (DDouble));
        DPtr p = e->NewHeap(1, measure);
        (*static_cast<DPtrGDL*> (entities->GetTag(entities->Desc()->TagIndex("MEASURE"), i)))[0] = p;
      }
      int nParts = ret->nParts;
      (*static_cast<DLongGDL*> (entities->GetTag(entities->Desc()->TagIndex("N_PARTS"), i)))[0] = nParts;
      if (nParts > 0) {
        DLongGDL* parts = new DLongGDL(dimension(nParts));
        for (int j = 0; j < nParts; ++j) (*parts)[j] = ret->panPartStart[j];
        DPtr p = e->NewHeap(1, parts);
        (*static_cast<DPtrGDL*> (entities->GetTag(entities->Desc()->TagIndex("PARTS"), i)))[0] = p;
        if (doPartsType) {
          DLongGDL* partstype = new DLongGDL(dimension(nParts));
          for (int j = 0; j < nParts; ++j) (*partstype)[j] = ret->panPartType[j];
          DPtr p = e->NewHeap(1, partstype);
          (*static_cast<DPtrGDL*> (entities->GetTag(entities->Desc()->TagIndex("PART_TYPES"), i)))[0] = p;
        }
      }
      //destroy object
      SHPDestroyObject(ret);
      if (doAttr) {
 
        DStructGDL* attrs = new DStructGDL(attr_desc,dimension());
        for (int j = 0; j <  fieldCount; ++j) {
          switch (attribute_type[j]) {
          case 7:
            (*static_cast<DStringGDL*>(attrs->GetTag(j)))[0]=strdup(DBFReadStringAttribute(dbfh,i,j)); //strdup as DBFReadStringAttribute is only valid untill the next DBF function call
            break;
         case 2:
            (*static_cast<DLongGDL*>(attrs->GetTag(j)))[0]=DBFReadIntegerAttribute(dbfh,i,j);
            break;
          case 5:
            (*static_cast<DDoubleGDL*>(attrs->GetTag(j)))[0]=DBFReadDoubleAttribute(dbfh,i,j);
            break;
          case 1:
            (*static_cast<DByteGDL*>(attrs->GetTag(j)))[0]=DBFReadIntegerAttribute(dbfh,i,j);
            break;
          }
        }
        DPtr p = e->NewHeap(1, attrs->Dup()); //Dup() seems really really needed here!!!
        (*static_cast<DPtrGDL*> (entities->GetTag(entities->Desc()->TagIndex("ATTRIBUTES"), i)))[0] = p;
      }

    }
    if (has_dbfh) free(attribute_type);
    return entities;
  }

    BaseGDL * GDLffShape___GetAttributes(EnvUDT * e)
    {
      std::cerr << "TODO GetAttributes" << std::endl;
      return new DLongGDL(33);
    }

    void GDLffShape___AddAttribute(EnvUDT * e)
    {
      std::cerr << "TODO AddAttribute" << std::endl;
    }

    void GDLffShape___Cleanup(EnvUDT * e)
    {
      std::cerr << "TODO Cleanup" << std::endl;
    }

    void GDLffShape___Close(EnvUDT * e)
    {
      std::cerr << "TODO Close" << std::endl;
    }

    void GDLffShape___DestroyEntity(EnvUDT * e)
    {
      std::cerr << "TODO DestroyEntity" << std::endl;
    }

    void GDLffShape___PutEntity(EnvUDT * e)
    {
      std::cerr << "TODO PutEntity" << std::endl;
    }

    void GDLffShape___SetAttributes(EnvUDT * e)
    {
      std::cerr << "TODO SetAttributes" << std::endl;
    }

  }
