/***************************************************************************
                       plotting.cpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002-2011 by Marc Schellens et al.
    email                : m_schellens@users.sf.net
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
#include "projections.hpp"


namespace lib {

  using namespace std;

  static bool isRot;
  static bool noInv;
  static DDouble sRot, cRot;

  //OLD?-> Must be static and in plotting.hpp if one changes the position of "MAP_STRUCTURE".
  DStructGDL *GetMapAsMapStructureKeyword(EnvT *e, bool &externalmap)
  {
    externalmap = e->KeywordSet(0); //MAP_STRUCTURE
    DStructGDL* map = NULL;

    if (externalmap) {
      map = e->GetKWAs<DStructGDL>(0); //MAP_STRUCTURE
      if (map->Desc() != SysVar::Map()->Desc()) e->Throw("Conflicting data structures: MAP_STRUCTURE,!MAP.");
      unsigned projectionTag = map->Desc()->TagIndex("PROJECTION");
      DLong map_projection = (*static_cast<DLongGDL*> (map->GetTag(projectionTag, 0)))[0];
      if (map_projection < 1) e->Throw("Map transform not established.");
    } else {
      map = SysVar::Map();
      unsigned projectionTag = map->Desc()->TagIndex("PROJECTION");
      DLong map_projection = (*static_cast<DLongGDL*> (map->GetTag(projectionTag, 0)))[0];
      if (map_projection < 1) e->Throw("Map transform not established.");
    }
    return map;
  }
  
  BaseGDL* map_proj_forward_fun(EnvT* e)
  {
#ifdef USE_LIBPROJ4
    BaseGDL* p0;
    BaseGDL* p1;

    DDoubleGDL *lon;
    DDoubleGDL *lat;
    Guard<DDoubleGDL> lonGuard, latGuard;

    DLongGDL* gons = NULL;
    DLongGDL* lines = NULL;
    DLongGDL* connectivity = NULL;
    DDoubleGDL *res = NULL;
    SizeT nEl;

    SizeT nParam = e->NParam();
    if (nParam < 1 || nParam > 2)
      e->Throw("Incorrect number of arguments.");

    //RADIANS
    static int radianIx = e->KeywordIx("RADIANS");
    bool radians = e->KeywordSet(radianIx);

    // Get MATRIX 
    bool externalMap;
    DStructGDL* map = GetMapAsMapStructureKeyword(e, externalMap);
    ref = map_init(map);
    if (ref == NULL) {
      e->Throw("Projection initialization failed.");
    }

    // keywords "POLYGONS", "POLYLINES" and "FILL"
    static int gonsIx = e->KeywordIx("POLYGONS");
    bool doGons = e->KeywordPresent(gonsIx);
    static int linesIx = e->KeywordIx("POLYLINES");
    bool doLines = e->KeywordPresent(linesIx);
    static int fillIx = e->KeywordIx("FILL");
    bool doFill = e->KeywordSet(fillIx);

    //keyword CONNECTIVITY
    static int connIx = e->KeywordIx("CONNECTIVITY");
    bool doConn = e->KeywordPresent(connIx);
    if (doConn) connectivity = e->GetKWAs<DLongGDL>(connIx);

    //with connectivity, polygons or polylines, and 1 argument, dimension MUST be [2,*]
    bool fussy = (doConn || doGons || doLines);

    //Get arguments
    if (nParam == 1) { //lat is not present...
      p0 = e->GetParDefined(0);
      DDoubleGDL* ll = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      if (fussy) {
        if (p0->Rank() != 2) e->Throw("(X,Y) array must be (2,N).");
        if (p0->Dim(0) != 2) e->Throw("(X,Y) array must be (2,N).");
      }
      nEl = p0->N_Elements() / 2; //as simple as that

      lon = new DDoubleGDL(dimension(nEl), BaseGDL::NOZERO);
      lonGuard.Reset(lon);
      lat = new DDoubleGDL(dimension(nEl), BaseGDL::NOZERO);
      latGuard.Reset(lat);

#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
        for (OMPInt i = 0; i < nEl; ++i) {
          (*lon)[i] = (*ll)[2 * i] * ((radians) ? 1.0 : DEG_TO_RAD);
          (*lat)[i] = (*ll)[2 * i + 1] * ((radians) ? 1.0 : DEG_TO_RAD);
        }
      }
    } else { //nParam==2
      p0 = e->GetParDefined(0);
      nEl = p0->N_Elements();
      p1 = e->GetParDefined(1);
      if (p1->N_Elements() != nEl) e->Throw("X & Y arrays must have same number of points.");
      //allocate arrays
      DDoubleGDL *tmplon = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      DDoubleGDL *tmplat = static_cast<DDoubleGDL*> (p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      lon = new DDoubleGDL(dimension(nEl), BaseGDL::NOZERO);
      lonGuard.Reset(lon);
      lat = new DDoubleGDL(dimension(nEl), BaseGDL::NOZERO);
      latGuard.Reset(lat);

#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
        for (OMPInt i = 0; i < nEl; ++i) {
          (*lon)[i] = (*tmplon)[i] * ((radians) ? 1 : DEG_TO_RAD);
          (*lat)[i] = (*tmplat)[i] * ((radians) ? 1 : DEG_TO_RAD);
        }
      }
    }

    if (doGons || doLines) {
      res = gdlProjForward(ref, map, lon, lat, connectivity, doConn, gons, doGons, lines, doLines, doFill);
      if (doGons) e->SetKW(gonsIx, gons);
      else e->SetKW(linesIx, lines);
    } else { //do it oursef
      LPTYPE idata;
      XYTYPE odata;
      DLong dims[2];
      dims[0] = 2;
      dims[1] = nEl;
      dimension dim(dims, 2);
      res = new DDoubleGDL(dim, BaseGDL::NOZERO);
#ifdef PROJ_IS_THREADSAFE
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
#endif
        for (OMPInt i = 0; i < nEl; ++i) {
          idata.u = (*lon)[i];
          idata.v = (*lat)[i];
          odata = PJ_FWD(idata, ref);
          (*res)[2 * i] = odata.u;
          (*res)[2 * i + 1] = odata.v;
        }
      }
#ifdef PROJ_IS_THREADSAFE
    }
#endif
    return res;
#else
    e->Throw("GDL was compiled without support for map projections");
    return NULL;
#endif
  }

  BaseGDL* map_proj_inverse_fun(EnvT* e)
  {
#ifdef USE_LIBPROJ4
    // xy -> lonlat
    SizeT nParam = e->NParam();
    if (nParam < 1 || nParam > 2)
      e->Throw("Incorrect number of arguments.");

    static int radianIx = e->KeywordIx("RADIANS");
    bool radians = e->KeywordSet(radianIx);

    // Get MATRIX 
    bool externalMap;
    DStructGDL* map = GetMapAsMapStructureKeyword(e, externalMap);
    ref = map_init(map);
    if (ref == NULL) {
      e->Throw("Projection initialization failed.");
    }


    XYTYPE idata;
    LPTYPE odata;

    BaseGDL* p0;
    BaseGDL* p1;

    DDoubleGDL* x;
    DDoubleGDL* y;
    DDoubleGDL* xy;
    DDoubleGDL* res;
    DLong dims[2];

    if (nParam == 1) {
      p0 = e->GetParDefined(0);
      DDoubleGDL* xy = static_cast<DDoubleGDL*>
          (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));

      dims[0] = 2;
      if (p0->Rank() == 2) { //[2,dim1]->N_elements=2*Dim1
        dims[1] = p0->Dim(1);
        dimension dim((DLong *) dims, 2);
        res = new DDoubleGDL(dim, BaseGDL::NOZERO);
      } else { //[dim0] or [n,dim1,...] -> 2*N_elements idem!
        dims[1] = p0->N_Elements() / 2;
        dimension dim((DLong *) dims, 2);
        res = new DDoubleGDL(dim, BaseGDL::NOZERO);
      }
      
      //protect against projections that have no inverse in proj.4 (and inverse in libproj) (silly, is'nt it?) (I guess I'll copy all
      //this code one day and make our own certified version!
      if (noInv) {
        //return Nans --- hoping it is sufficient 
        for (OMPInt i = 0; i < p0->N_Elements() ; ++i) (*res)[i]=std::numeric_limits<double>::quiet_NaN();
        return res; //e->Throw("The proj4 library version you use unfortunately defines no inverse for this projection!");
      }
      
      SizeT nEl = p0->N_Elements() / 2;
#ifdef PROJ_IS_THREADSAFE
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
#endif
        for (OMPInt i = 0; i < nEl; ++i) {
          idata.u = (*xy)[2 * i];
          idata.v = (*xy)[2 * i + 1];
          odata = PJ_INV(idata, ref);
          (*res)[2 * i] = odata.u * ((radians) ? 1.0 : RAD_TO_DEG);
          (*res)[2 * i + 1] = odata.v * ((radians) ? 1.0 : RAD_TO_DEG);
        }
#ifdef PROJ_IS_THREADSAFE
      }
#endif

      return res;

    } else if (nParam == 2) {
      p0 = e->GetParDefined(0);
      SizeT nEl = p0->N_Elements();
      p1 = e->GetParDefined(1);
      if (p1->N_Elements() != nEl) e->Throw("X & Y arrays must have same number of points.");
      DDoubleGDL* x = static_cast<DDoubleGDL*>
          (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      DDoubleGDL* y = static_cast<DDoubleGDL*>
          (p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));

      dims[0] = 2;
      dims[1] = nEl;
      dimension dim((DLong *) dims, 2);
      res = new DDoubleGDL(dim, BaseGDL::NOZERO);
      //protect against projections that have no inverse in proj.4 (and inverse in libproj) (silly, is'nt it?) (I guess I'll copy all
      //this code one day and make our own certified version!
      if (noInv) {
        //return Nans --- hoping it is sufficient 
        for (OMPInt i = 0; i < p0->N_Elements() ; ++i) (*res)[i]=std::numeric_limits<double>::quiet_NaN();
        return res; //e->Throw("The proj4 library version you use unfortunately defines no inverse for this projection!");
      }
#ifdef PROJ_IS_THREADSAFE
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for 
#endif
        for (OMPInt i = 0; i < nEl; ++i) {
          idata.u = (*x)[i];
          idata.v = (*y)[i];
          odata = PJ_INV(idata, ref);
          (*res)[2 * i] = odata.u * ((radians) ? 1.0 : RAD_TO_DEG);
          (*res)[2 * i + 1] = odata.v * ((radians) ? 1.0 : RAD_TO_DEG);
        }
#ifdef PROJ_IS_THREADSAFE
      }
#endif

      return res;
    }
    e->Throw("More than 2 parameters not handled."); //impossible to reach!
    return NULL;
#else
    e->Throw("GDL was compiled without support for map projections");
    return NULL;
#endif
  }

#ifdef USE_LIBPROJ4
    //enum all the projections

    enum {
      Invalid = 0,
      Stereographic,
      Orthographic,
      LambertConic,
      LambertAzimuthal,
      Gnomic,
      AzimuthalEquidistant,
      Satellite,
      Cylindrical,
      Mercator,
      Mollweide,
      Sinusoidal,
      Aitoff,
      HammerAitoff,
      AlbersEqualAreaConic,
      TransverseMercator,
      MillerCylindrical,
      Robinson,
      LambertEllipsoidConic,
      GoodesHomolosine,
      Geographic,
      GCTP_UTM,
      GCTP_StatePlane,
      GCTP_AlbersEqualArea,
      GCTP_LambertConformalConic,
      GCTP_Mercator,
      GCTP_PolarStereographic,
      GCTP_Polyconic,
      GCTP_EquidistantConic,
      GCTP_TransverseMercator,
      GCTP_Stereographic,
      GCTP_LambertAzimutha,
      GCTP_Azimuthal,
      GCTP_Gnomonic,
      GCTP_Orthographic,
      GCTP_NearSidePerspective,
      GCTP_Sinusoidal,
      GCTP_Equirectangular,
      GCTP_MillerCylindrical,
      GCTP_VanderGrinten,
      GCTP_HotineObliqueMercator,
      GCTP_Robinson,
      GCTP_SpaceObliqueMercator,
      GCTP_AlaskaConformal,
      GCTP_InterruptedGoode,
      GCTP_Mollweide,
      GCTP_InterruptedMollweide,
      GCTP_Hammer,
      GCTP_WagnerIV,
      GCTP_WagnerVII,
      GCTP_OblatedEqualArea,
      GCTP_IntegerizedSinusoidal,
      GCTP_CylindricalEqualArea
    } Projection = Stereographic;

    enum { //see projElement below
      NONE = 0,
      SPHERE_RADIUS,           //+R
      CENTER_LONGITUDE,        //+lon_0
      CENTER_LATITUDE,         //+lat_0
      STANDARD_PAR1,           //+lat_1
      STANDARD_PAR2,           //+lat_2
      HEIGHT,                  //+h
      SAT_TILT,                //+tilt 
      CENTER_AZIMUTH,          //+alpha
      SEMIMAJOR_AXIS,          //+a
      SEMIMINOR_AXIS,          //+b
      MERCATOR_SCALE,          //+k0
      ZONE,                    //+zone
      FALSE_EASTING,           //+x_0
      FALSE_NORTHING,          //+y_0
      TRUE_SCALE_LATITUDE,     //+lat_ts
      STANDARD_PARALLEL,       //+lat_1
      HOM_LONGITUDE1,          //+lon_1
      HOM_LONGITUDE2,          //+lon_2
      HOM_AZIM_LONGITUDE,      //+lonc
      SOM_LANDSAT_NUMBER,      //+lsat
      SOM_LANDSAT_PATH,        //+path
      OEA_SHAPEM,              //+m
      OEA_SHAPEN,              //+n
      IS_ZONES,                // +n=4 +m=  zone num see https://modis-land.gsfc.nasa.gov/MODLAND_grid.html
      IS_JUSTIFY,              //see above
      HOM_AZIM_ANGLE,          //+alpha
      HOM_LATITUDE1,           //+lat_1
      HOM_LATITUDE2,           //+lat_2
      OEA_ANGLE,               //+theta
//      SOM_INCLINATION,         // unknown with proj4
//      SOM_LONGITUDE,           //
//      SOM_PERIOD,              //
//      SOM_RATIO,               //
//      SOM_FLAG,                //
      ROTATION                 //done elsewhere?
    } projElementIndex;
    
    static int isAngle[] = {-100, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,1,1,1};
    static string projElement[] = {//same order that projElementIndex, please!
      "Null", " +R=", " +lon_0=", " +lat_0=", " +lat_1=", " +lat_2=",
      " +h=", " +tilt=", " +alpha=", " +a=", " +b=", " +k0=", " +zone=", " +x_0=",
      " +y_0=", " +lat_ts=", " +lat_1=", " +lon_1=", " +lon_2=", " +lonc=",
      " +lsat=", " +path=", " +m=", " +n=", " +n=4 +m=", " ", " +alpha=", " +lat_1=", " +lat_2=", " +theta="
    };
    static string obliqueProjElement[] = {//same order that projElementIndex, please!
      "Null", " +R=", " +o_lon_p=", " +o_lat_p=", " +lat_1=", " +lat_2=",
      " +h=", " +tilt=", " +alpha=", " +a=", " +b=", " +k0=", " +zone=", " +x_0=",
      " +y_0=", " +lat_ts=", " +lat_1=", " +lon_1=", " +lon_2=", " +lonc=",
      " +lsat=", " +path=", " +m=", " +n=", " +n=4 +m=", " ", " +alpha=", " +lat_1=", " +lat_2=",  " +theta="
    };

    typedef struct {
      int pidx;
      int pnum;
      int ptyp;
      string pnam;
      string p4nam;
      int nopt;
      int code[16];
      int vcode[16];
    } projCoding;

    static projCoding projectionOptions[] = {
      // pidx, proj. Number as in doc, Name, Proj.4 litle name, number of projElements to read, 
      // Values as index in projElements (options for proj.4), idem for variant.
      //pidx|pnum|ptyp|pnam                          |p4nam         |nopt|                     code                           |vcode
      { 0, 0, 0, "Invalid Projection", "none", 0,
        {0},
        {0}},
      { 1, 1, 0, "Stereographic", "stere", 6,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 2, 2, 0, "Orthographic", "ortho", 6,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 3, 3, 1, "LambertConic", "lcc", 6,
        {SPHERE_RADIUS, 0, STANDARD_PAR1, STANDARD_PAR2, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 4, 4, 0, "LambertAzimuthal", "laea", 6,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 5, 5, 0, "Gnomonic", "gnom", 6,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 6, 6, 0, "AzimuthalEquidistant", "aeqd", 6,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 7, 7, 0, "Satellite", "tpers", 6,
        {SPHERE_RADIUS, 0, HEIGHT, SAT_TILT, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 8, 8, 2, "Cylindrical", "eqc", 6,
        {SPHERE_RADIUS, 0, 0, CENTER_AZIMUTH, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 9, 9, 2, "Mercator", "merc", 6,
        {SPHERE_RADIUS, 0, 0, CENTER_AZIMUTH, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 10, 10, 4, "Mollweide", "moll", 6,
        {SPHERE_RADIUS, 0, 0, CENTER_AZIMUTH, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 11, 11, 4, "Sinusoidal", "sinu", 6,
        {SPHERE_RADIUS, 0, 0, CENTER_AZIMUTH, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 12, 12, 3, "Aitoff", "aitoff", 6,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 13, 13, 3, "HammerAitoff", "hammer", 6,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 14, 14, 1, "AlbersEqualAreaConic", "aea", 6,
        {SPHERE_RADIUS, 0, STANDARD_PAR1, STANDARD_PAR2, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 15, 15, 2, "TransverseMercator", "tmerc", 6,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, MERCATOR_SCALE, 0, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 16, 16, 2, "MillerCylindrical", "mill", 6,
        {SPHERE_RADIUS, 0, 0, CENTER_AZIMUTH, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 17, 17, 4, "Robinson", "robin", 6,
        {SPHERE_RADIUS, 0, 0, CENTER_AZIMUTH, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 18, 18, 1, "LambertConicEllipsoid", "lcc", 6,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, STANDARD_PAR1, STANDARD_PAR2, CENTER_LONGITUDE, CENTER_LATITUDE, 0},
        {0}},
      { 19, 19, 4, "GoodesHomolosine",
#ifdef USE_LIBPROJ4_NEW
        "igh"
#else
        "goode"
#endif
        , 5,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, 0, 0},
        {0}},
      {20, 100, 5, "Geographic", "eqc", 1,
        {CENTER_LONGITUDE, 0, 0, 0, 0, 0, 0},
        {0}},
      {21, 101, 5, "GCTP_UTM", "utm", 6,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, ZONE, 0, CENTER_LONGITUDE, CENTER_LATITUDE, 0, 0},
        {0}}, //may use "+south"
      {22, 102, 2, "GCTP_StatePlane", "utm", 3,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, ZONE, 0, 0, 0, 0, 0},
        {0}}, //may use "+south"
      {23, 103, 1, "GCTP_AlbersEqualArea", "aea", 9,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, STANDARD_PAR1, STANDARD_PAR2, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {24, 104, 1, "GCTP_LambertConformalConic", "lcc", 9,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, STANDARD_PAR1, STANDARD_PAR2, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {25, 105, 2, "GCTP_Mercator", "merc", 9,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, 0, 0, CENTER_LONGITUDE, TRUE_SCALE_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {26, 106, 0, "GCTP_PolarStereographic", "ups", 10,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, 0, 0, CENTER_LONGITUDE, TRUE_SCALE_LATITUDE, FALSE_EASTING, FALSE_NORTHING, CENTER_LATITUDE,0},
        {0}}, //may use "+south"
      {27, 107, 1, "GCTP_Polyconic", "poly", 9,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {28, 108, 1, "GCTP_EquidistantConic", "eqdc", 9,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, STANDARD_PARALLEL, 0, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, STANDARD_PAR1, STANDARD_PAR2, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,1}},
      {29, 109, 2, "GCTP_TransverseMercator", "tmerc", 9,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, MERCATOR_SCALE, 0, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {30, 110, 0, "GCTP_Stereographic", "stere", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {31, 111, 0, "GCTP_LambertAzimutha", "laea", 9,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {32, 112, 0, "GCTP_Azimuthal", "aeqd", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {33, 113, 0, "GCTP_Gnomonic", "gnom", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {34, 114, 0, "GCTP_Orthographic", "ortho", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {35, 115, 0, "GCTP_NearSidePerspective", "nsper", 9,
        {SPHERE_RADIUS, 0, HEIGHT, 0, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {36, 116, 4, "GCTP_Sinusoidal", "sinu", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, 0, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {37, 117, 2, "GCTP_Equirectangular", "eqc", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, TRUE_SCALE_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {38, 118, 2, "GCTP_MillerCylindrical", "mill", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, 0, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {39, 119, 3, "GCTP_VanderGrinten", "vandg", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {40, 120, 1, "GCTP_HotineObliqueMercator", "omerc", 13,// Two point method; variant is Central point and azimuth method
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, MERCATOR_SCALE, 0             , 0                 , CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING, HOM_LONGITUDE1, HOM_LATITUDE1, HOM_LONGITUDE2, HOM_LATITUDE2,0},
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, MERCATOR_SCALE, HOM_AZIM_ANGLE, HOM_AZIM_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING, 0             , 0            , 0             , 0            ,1}},
      {41, 121, 4, "GCTP_Robinson", "robin", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, 0, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {42, 122, 2, "GCTP_SpaceObliqueMercator", "lsat", 13,  //only variant implemented in proj4.
        //{SEMIMAJOR_AXIS, SEMIMINOR_AXIS, 0, SOM_INCLINATION, SOM_LONGITUDE, 0, FALSE_EASTING, FALSE_NORTHING, SOM_PERIOD, SOM_RATIO, SOM_FLAG},
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, SOM_LANDSAT_NUMBER, SOM_LANDSAT_PATH, 0, 0, FALSE_EASTING, FALSE_NORTHING, 0, 0, 0, 0, 0},
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, SOM_LANDSAT_NUMBER, SOM_LANDSAT_PATH, 0, 0, FALSE_EASTING, FALSE_NORTHING, 0, 0, 0, 0, 1}},
      {43, 123, 0, "GCTP_AlaskaConformal", "alsk", 9,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, 0, 0, 0, 0, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {44, 124, 4, "GCTP_InterruptedGoode", "igh", 2,
        {SPHERE_RADIUS,0},
        {0}},
      {45, 125, 4, "GCTP_Mollweide", "moll", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, 0, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {46, 126, 4, "GCTP_InterruptedMollweide", "moll", 2,
        {SPHERE_RADIUS,0},
        {0}},
      {47, 127, 4, "GCTP_Hammer", "hammer", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, 0, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {48, 128, 4, "GCTP_WagnerIV", "wag4", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, 0, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {49, 129, 3, "GCTP_WagnerVII", "wag7", 9,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, 0, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {50, 130, 3, "GCTP_OblatedEqualArea", "oea", 9,
        {SPHERE_RADIUS, 0, OEA_SHAPEM, OEA_SHAPEN, CENTER_LONGITUDE, CENTER_LATITUDE, FALSE_EASTING, FALSE_NORTHING,0},
        {0}},
      {51, 131, 9, "GCTP_IntegerizedSinusoidal", "sinu", 12,
        {SPHERE_RADIUS, 0, 0, 0, CENTER_LONGITUDE, 0, FALSE_EASTING, FALSE_NORTHING, IS_ZONES, 0, IS_JUSTIFY,0},
        {0}},
      {52, 132, 2, "GCTP_CylindricalEqualArea", "cea", 9,
        {SEMIMAJOR_AXIS, SEMIMINOR_AXIS, STANDARD_PARALLEL, 0, CENTER_LONGITUDE, 0, FALSE_EASTING, FALSE_NORTHING,0},
        {0}}
    };
    
  PROJTYPE map_init(DStructGDL * map)
  {



    bool variant = false;

    unsigned projectionTag = map->Desc()->TagIndex("PROJECTION");
    unsigned p0lonTag = map->Desc()->TagIndex("P0LON");
    unsigned p0latTag = map->Desc()->TagIndex("P0LAT");
    unsigned aTag = map->Desc()->TagIndex("A");
    unsigned e2Tag = map->Desc()->TagIndex("E2");
    unsigned pTag = map->Desc()->TagIndex("P");
    unsigned rTag = map->Desc()->TagIndex("ROTATION");
    unsigned simpleTag = map->Desc()->TagIndex("SIMPLE");
    unsigned projNameTag = map->Desc()->TagIndex("UP_NAME");

    DDouble map_rot = (*static_cast<DDoubleGDL*> (map->GetTag(rTag, 0)))[0];

    DLong map_projection = (*static_cast<DLongGDL*> (map->GetTag(projectionTag, 0)))[0];

    if (map_projection < 1) return NULL;




    DDouble map_p0lon = (*static_cast<DDoubleGDL*> (map->GetTag(p0lonTag, 0)))[0];
    DDouble map_p0lat = (*static_cast<DDoubleGDL*> (map->GetTag(p0latTag, 0)))[0];
    DDouble map_a = (*static_cast<DDoubleGDL*> (map->GetTag(aTag, 0)))[0];
    DDouble map_e2 = (*static_cast<DDoubleGDL*> (map->GetTag(e2Tag, 0)))[0];
    DDouble *map_p = &(*static_cast<DDoubleGDL*> (map->GetTag(pTag, 0)))[0];
    DDouble map_lat1 = (*static_cast<DDoubleGDL*> (map->GetTag(pTag, 0)))[3];
    DDouble map_lat2 = (*static_cast<DDoubleGDL*> (map->GetTag(pTag, 0)))[4];
    DString projName = (*static_cast<DStringGDL*> (map->GetTag(projNameTag, 0)))[0];

    //Trick for using ALL the libproj.4 projections.
    if (map_projection == 999) { //our special code
#ifdef USE_LIBPROJ4_NEW
      prev_ref = pj_init_plus(projName.c_str());
      if (!prev_ref) {
        Warning("LIBPROJ.4 returned error message: ");
        Warning(pj_strerrno(pj_errno));
      }
#else
      prev_ref = proj_initstr((char*) projName.c_str());
      if (!prev_ref) {
        Warning("LIBPROJ.4 returned error message: ");
        Warning(proj_strerrno(proj_errno));
      }
#endif
      return prev_ref;
    }    
    
    static char *parms[32]; //parameters for LIBPROj.4, old style


    DLong noptions=0;
    //GCTP support
    if (map_projection == 20) {
      map_projection = (*static_cast<DLongGDL*> (map->GetTag(simpleTag, 0)))[0] + GoodesHomolosine + 1;
      noptions=projectionOptions[map_projection].nopt-1; //-1 as the last is not an option but variant;
      variant=(map_p[noptions]>0);
    }

    //protect against projections that have no inverse in proj.4 (and inverse in libproj) (silly, is'nt it?) (I guess I'll copy all
    //this code one day and make our own certified version!
    noInv = FALSE;
    if (map_projection == 12 || map_projection == 13 || map_projection == 47 || map_projection == 49) {
      noInv = TRUE;
    }

    char proj[64];
    char p0lon[64];
    char p0lat[64];
    char a[64];
    char e2[64];
    char lat_1[64];
    char lat_2[64];
    char lat_ts[64];
    char opt[64];
    char h[64];
    char tilt[64];
    char R[32];

    // Oblique projection parameters
    char ob_proj[64];
    char ob_lon[64];
    char ob_lat[64];
    DDouble proj_p0lon = 0.0; //Default
    DDouble proj_p0lat = 90.0; //Default

    static DLong last_proj = 0;
    static DDouble last_p0lon = -9999;
    static DDouble last_p0lat = -9999;
    static DDouble last_a = -9999;
    static DDouble last_e2 = -9999;
    static DDouble last_lat1 = -9999;
    static DDouble last_lat2 = -9999;
    static DDouble last_rot = -9999;
    static DDouble last_p[16];
    static DString last_projName = "...................";

    bool trans = false;

    bool redo = (map_projection != last_proj ||
        map_p0lon != last_p0lon ||
        map_p0lat != last_p0lat ||
        map_a != last_a ||
        map_e2 != last_e2 ||
        map_lat1 != last_lat1 ||
        map_lat2 != last_lat2 ||
        map_rot != last_rot ||
        projName != last_projName
        );
    for (SizeT i = 0; i < 16; ++i) if (last_p[i] != map_p[i]) redo = true;

    if (redo) {
      //      fprintf(stderr, "new projection number %d\n", map_projection);
      //save values here, we may change them later for local convenience
      last_proj = map_projection;
      last_p0lon = map_p0lon;
      last_p0lat = map_p0lat;
      last_a = map_a;
      last_e2 = map_e2;
      last_lat1 = map_lat1;
      last_lat2 = map_lat2;
      last_rot = map_rot;
      last_projName = projName;
      for (SizeT i = 0; i < 16; ++i) last_p[i] = map_p[i];


      if (map_rot != 0.0) {
        isRot = true;
        sRot = sin(map_rot * DEG_TO_RAD);
        cRot = cos(map_rot * DEG_TO_RAD);
      } else isRot = false;


      if (projectionOptions[map_projection].ptyp > 1 && abs(map_p0lat) > 1) trans = true;
      DDouble val;
      //GCTP Projections only:
      std::string projCommand;
      projCommand += (trans ? "+proj=ob_tran +o_proj=" : " +proj=") + projectionOptions[map_projection].p4nam;
      for (int i = 0; i < noptions; i++) {
        if (variant) {
          if (projectionOptions[map_projection].vcode[i] != 0) {
            val = map_p[i];
            if (isAngle[projectionOptions[map_projection].vcode[i]] == 1) val *= ((map_projection > GoodesHomolosine) ? 1.0E-6 : RAD_TO_DEG);
            projCommand += trans ? obliqueProjElement[projectionOptions[map_projection].vcode[i]] : projElement[projectionOptions[map_projection].vcode[i]];
            projCommand += i2s(val);
          }
        } else {
          if (projectionOptions[map_projection].code[i] != 0) {
            val = map_p[i];
            if (
                ((map_projection == GCTP_UTM || map_projection == GCTP_StatePlane) && i == 2) ||
                (map_projection == GCTP_PolarStereographic && i == 8)
                ) {
              if (val < 0) { //negative Zone is South!
                val *= -1.0;
                projCommand += " +south";
              }
            }
            if (isAngle[projectionOptions[map_projection].code[i]] == 1) val *= ((map_projection > GoodesHomolosine) ? 1.0E-6 : RAD_TO_DEG);
            projCommand += trans ? obliqueProjElement[projectionOptions[map_projection].code[i]] : projElement[projectionOptions[map_projection].code[i]];
            projCommand += i2s(val);
          }
        }
      }
      // for projections above 100 in map_proj_init, things are finished here:
      if (map_projection > GoodesHomolosine) { //use projComand automatically defined above
        //some special treatments:
      switch (map_projection) {
        case Geographic:
          projCommand +=" +R="; projCommand +=i2s(RAD_TO_DEG);
          break;
        case GCTP_StatePlane:
         ThrowGDLException("GCTP StatePlane projection unsupported (FIXME!).");
         break;
        }
      cout<<projCommand<<endl;
#ifdef USE_LIBPROJ4_NEW
        prev_ref = pj_init_plus(projCommand.c_str());
        if (!prev_ref) {
          Warning("LIBPROJ.4 returned error message: ");
          Warning(pj_strerrno(pj_errno));
        }
#else
        prev_ref = proj_initstr((char*) projCommand.c_str());
        if (!prev_ref) {
          Warning("LIBPROJ.4 returned error message: ");
          Warning(proj_strerrno(proj_errno));
        }
#endif
        return prev_ref;
      }

      //for classical projections here [1..19] we still use an older way to call pj_init (and not projcommand). This is probably bad... 
      if (trans) { //use rotation variant, a killer with some projections
        //        if (map_p0lon==180.0) map_p0lon=179.999992;
        if (map_p0lat > 89.999) map_p0lat = 89.999; //take some precautions as PROJ.4 is not proetected!!! 
        if (map_p0lat < -89.999) map_p0lat = -89.999;
        sprintf(ob_lon, "o_lon_p=%lf", proj_p0lon);
        proj_p0lat = 90.0 - map_p0lat; //ADD CENTRAL_AZIMUTH!!
        map_p0lat = 0.0;
        sprintf(ob_lat, "o_lat_p=%lf", proj_p0lat);
      }

      sprintf(p0lon, "lon_0=%lf", map_p0lon);
      sprintf(p0lat, "lat_0=%lf", map_p0lat);

      //      if (map_e2 == 0.0) {
      //        sprintf(a, "R=%lf", map_a);
      //      } else {
      sprintf(a, "a=%lf", map_a);
      sprintf(e2, "es=%lf", map_e2);
      //      }

      DLong nparms = 0;

      parms[nparms++] = &a[0];
      if (map_e2 != 0.0) parms[nparms++] = &e2[0];

      if (trans //CHECK THIS FOR GCTP projections!
          //      && map_projection != Satellite
          && map_projection != GCTP_WagnerVII //no invert in old and new libproj4.
#ifdef USE_LIBPROJ4_NEW
          //      && map_projection != Mercator  //crashes with map_grid!!!
          && map_projection != TransverseMercator //idem!!
          //      && map_projection != Orthographic //idem!!
          //      && map_projection != Aitoff //idem!!
#endif
          ) {
        strcpy(ob_proj, "proj=ob_tran");
        parms[nparms++] = &ob_proj[0];
        sprintf(proj, "o_proj=%s", projectionOptions[map_projection].p4nam.c_str());
        parms[nparms++] = &proj[0];
        parms[nparms++] = &ob_lon[0];
        parms[nparms++] = &ob_lat[0];
        parms[nparms++] = &p0lon[0];
        parms[nparms++] = &p0lat[0];
      } else {
        sprintf(proj, "proj=%s", projectionOptions[map_projection].p4nam.c_str());
        parms[nparms++] = &proj[0];
        parms[nparms++] = &p0lon[0];
        parms[nparms++] = &p0lat[0];
      }

      switch (map_projection) {
      case Mercator:
        sprintf(lat_ts, "lat_ts=%lf", map_lat1 * RAD_TO_DEG);
        parms[nparms++] = &lat_ts[0];
        break;
      case LambertConic:
      case AlbersEqualAreaConic:
      case LambertEllipsoidConic:
        if (map_lat2 == 0.0) {
          sprintf(lat_1, "lat_0=%lf", map_lat1 * RAD_TO_DEG);
          parms[nparms++] = &lat_1[0];
        } else {
          sprintf(lat_1, "lat_1=%lf", map_lat1 * RAD_TO_DEG);
          sprintf(lat_2, "lat_2=%lf", map_lat2 * RAD_TO_DEG);
          parms[nparms++] = &lat_1[0];
          parms[nparms++] = &lat_2[0];
        }
        break;
      case Satellite: //Satellite looks wrong ??? check
        sprintf(h, "h=%lf", map_p[0] - 1.0);
        parms[nparms++] = &h[0];
        sprintf(tilt, "tilt=%lf", RAD_TO_DEG * map_p[1]);
        parms[nparms++] = &tilt[0];
        sprintf(R, "R=1");
        parms[nparms++] = &R[0];
        //          sprintf(azi, "azi=%lf", map_rot);
        //          parms[nparms++] = &azi[0]; 
        break;
      }

//      fprintf(stderr, "nparms=%d:", nparms);
//      for (SizeT i = 0; i < nparms; ++i) fprintf(stderr, "+%s ", parms[i]);
//      fprintf(stderr, "\n");
#ifdef USE_LIBPROJ4_NEW
      prev_ref = PJ_INIT(nparms, parms);
      if (!prev_ref) {
        Warning("LIBPROJ.4 returned error message: ");
        Warning(pj_strerrno(pj_errno));
      }
#else
      prev_ref = PJ_INIT(nparms, parms);
      if (!prev_ref) {
        Warning("LIBPROJ.4 returned error message: ");
        Warning(proj_strerrno(proj_errno));
      }
#endif      
    }
    return prev_ref;
  }

#ifdef USE_LIBPROJ4_NEW

  PROJDATA protect_proj_fwd(PROJDATA idata, PROJTYPE proj)
  {
    XYTYPE odata;
    DDouble u, v;
    if (isfinite((idata.u)*(idata.v))) {
      odata = LIB_PJ_FWD(idata, proj);
      if (isfinite(odata.u) && odata.u != HUGE_VAL) {
        if (isRot) {
          u = odata.u;
          v = odata.v;
          odata.u = u * cRot + v*sRot;
          odata.v = -u * sRot + v*cRot;
        }
        return odata;
      }
    }
    return badProj;
  }

  PROJDATA protect_proj_inv(PROJDATA idata, PROJTYPE proj)
  {
    if (noInv) return badProj;
    //  throw GDLException("The proj4 library version you use unfortunately defines no inverse for this projection!");
    LPTYPE odata;
    DDouble u, v;
    if (isfinite((idata.u)*(idata.v))) {
      odata = LIB_PJ_INV(idata, proj);
      if (isfinite(odata.u) && odata.u != HUGE_VAL) {
        if (isRot) {
          u = odata.u;
          v = odata.v;
          odata.u = u * cRot - v*sRot;
          odata.v = u * sRot + v*cRot;
        }
        return odata;
      }
    }
    return badProj;
  }
#else

PROJDATA protect_proj_fwd(PROJDATA idata, PROJTYPE proj)
  {
    XYTYPE odata;
    DDouble u, v;
    if (isfinite((idata.u)*(idata.v))) {
      odata = LIB_PJ_FWD(idata, proj);
      if (isfinite(odata.u) && odata.u != HUGE_VAL) {
        if (isRot) {
          u = odata.u;
          v = odata.v;
          odata.u = u * cRot + v*sRot;
          odata.v = -u * sRot + v*cRot;
        }
        return odata;
      }
    }
    return badProj;
  }

  PROJDATA protect_proj_inv(PROJDATA idata, PROJTYPE proj)
  {
    if (noInv) return badProj;
    XYTYPE odata;
    DDouble u, v;
    if (isfinite((idata.u)*(idata.v))) {
      odata = LIB_PJ_INV(idata, proj);
      if (isfinite(odata.u) && odata.u != HUGE_VAL) {
        if (isRot) {
          u = odata.u;
          v = odata.v;
          odata.u = u * cRot - v*sRot;
          odata.v = u * sRot + v*cRot;
        }
        return odata;
      }
    }
    return badProj;
  }
#endif

  //dummy functions for compatibility support of GCTP projections. Should define map_pipeline values. 

  void map_proj_gctp_forinit(EnvT* e)
  {
  };

  void map_proj_gctp_revinit(EnvT* e)
  {
  };
#endif

#define GDL_PI     double(3.1415926535897932384626433832795)
#define GDL_HALFPI 0.5*GDL_PI  
  //epsilon is the size of the "trouble ahead" region around splits. Mostly due to projection numerical errors?
  //if set to a smaller value, the splits of the Goode projections are not OK (? some goode's projection computation in float instead of double?)
  static double epsilon = std::numeric_limits<float>::epsilon(); //say, 5e-7   

#define DELTA  (double)(0.5*DEG_TO_RAD) //0.5 degree for increment between stitch vertexes.

  struct Vertex {
    DDouble lon; //lon
    DDouble lat; //lat
  };

  struct Polygon {
    std::list<Vertex> VertexList;
    int type; //+1 before cut, -1 after cut
    int inside; // number of polygons inside
    int outside; // number of polygons outside
    bool valid; //to be ignored (polygon has been transferred to another polygon list
  };

  struct Point3d {
    DDouble x;
    DDouble y;
    DDouble z;
  };

  Point3d* toPoint3d(DDouble x, DDouble y, DDouble z)
  {
    Point3d* p = new Point3d;
    DDouble norm = sqrt(x * x + y * y + z * z);
    p->x = x / norm;
    p->y = y / norm;
    p->z = z / norm;
    return p;
  }

  void rotate3d(Point3d &p1, const Point3d a, DDouble theta)
  {
    DDouble st,ct;
    sincos(theta,&st,&ct);
    // quaternion-derived rotation matrix
    DDouble matrix[3][3] = {
      { a.x * a.x * (1 - ct) + ct, a.x * a.y * (1 - ct) - a.z*st, a.x * a.z * (1 - ct) + a.y * st},
      { a.y * a.x * (1 - ct) + a.z*st, a.y * a.y * (1 - ct) + ct, a.y * a.z * (1 - ct) - a.x * st},
    { a.z * a.x * (1 - ct) - a.y*st, a.z * a.y * (1 - ct) + a.x*st, a.z * a.z * (1 - ct) + ct },
     };
    // multiply matrix × vector
    DDouble vector[3]={p1.x, p1.y, p1.z};
    DDouble rotated[3] = {0, 0, 0};
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        rotated[i] += matrix[i][j] * vector[j];
      }
    }
    //normalize (useful?)
    DDouble norm=sqrt(rotated[0]*rotated[0]+rotated[1]*rotated[1]+rotated[2]*rotated[2]);

    p1.x=rotated[0]/norm;
    p1.y=rotated[1]/norm; 
    p1.z=rotated[2]/norm;
  }
    
    void printVertex(Vertex v){
      std::cerr.precision(10);
      std::cerr<<"("<<v.lon/DEG_TO_RAD<<","<<v.lat/DEG_TO_RAD<<")";
    }

  Point3d* normedCrossP(const Point3d* p1, const Point3d *p2)
  {
    Point3d* p = new Point3d;
    p->x = (p1->y * p2->z - p1->z * p2->y);
    p->y = (p1->z * p2->x - p1->x * p2->z);
    p->z = (p1->x * p2->y - p1->y * p2->x);
    DDouble norm = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
    p->x /= norm;
    p->y /= norm;
    p->z /= norm;
    return p;
  }

  inline DDouble dotP(const Point3d* p1, const Point3d *p2)
  {
    return p1->x * p2->x + p1->y * p2->y + p1->z * p2->z;
  }

  //arc distance from a (split) pole and a position

//  inline DDouble DistanceFromSplitPole(DDouble x, DDouble y, DDouble z, DDouble px, DDouble py, DDouble pz)
//  {
//    DDouble dotp = x * px + y * py + z*pz;
//    DDouble crossp = sqrt((y * pz - z * py)*(y * pz - z * py) + (z * px - x * pz)*(z * px - x * pz) + (x * py - y * px)*(x * py - y * px));
//    return atan2(crossp, dotp);
//  }
  

  //angular distance between 2 points on sphere.
  inline DDouble DistanceOnSphere(Vertex v1, Vertex v2, DDouble px, DDouble py, DDouble pz, DDouble pt=0)
  {
    DDouble clon,slon,clat,slat;

    //the 2 points are on the split
    DDouble x1, y1, z1;
    sincos(v1.lon,&slon,&clon);
    sincos(v1.lat,&slat,&clat);
    x1 = clon * clat;
    y1 = slon * clat;
    z1 = slat;
    
    DDouble x2, y2, z2;
    sincos(v2.lon,&slon,&clon);
    sincos(v2.lat,&slat,&clat);
    x2 = clon * clat;
    y2 = slon * clat;
    z2 = slat;
    
    DDouble dotp = x1 * x2 + y1 * y2 + z1 * z2;

    //cross-product vector
    DDouble xx,yy,zz;
    xx= (y1 * z2 - z1 * y2);
    yy= (z1 * x2 - x1 * z2);
    zz= (x1 * y2 - y1 * x2);
    DDouble crossp = sqrt(xx*xx+yy*yy+zz*zz);
    DDouble projpole=(xx * px + yy * py + zz * pz);
    if (projpole <0) crossp*=-1;
    return atan2(crossp, dotp);
  }
  
//    void dumpPolygonVertexes(std::list<Polygon> PolygonList)
//    {
//      int i=0;
//      for (std::list<Polygon>::iterator p = PolygonList.begin(); p != PolygonList.end(); ++p) {
//        i++;
//        if (p->VertexList.size()) {
//        std::cerr<<"Polygon "<<i<<":"<<std::endl;
//        std::cerr<<"Inside "<<p->inside<<":"<<std::endl;
//        std::cerr<<"Outside "<<p->outside<<":"<<std::endl;
//        std::cerr<<"Type "<<p->type<<":"<<std::endl;
//        std::list<Vertex>::iterator v;
//        std::cerr << "[";
//        for (v = p->VertexList.begin(); v != p->VertexList.end(); ++v) {
//          std::cerr << v->lon / DEG_TO_RAD << "d,";
//        }
//        std::cerr << "\b],[";
//        for (v = p->VertexList.begin(); v != p->VertexList.end(); ++v) {
//          std::cerr << v->lat / DEG_TO_RAD << "d,";
//        }
//        std::cerr << "\b]" << std::endl;
//        } else {
//         std::cerr<<"Polygon "<<i<<": EMPTY"<<std::endl;
//        }
//      }
//    }  




  // are we on the cut pole side or not?

  inline int getSign(DDouble indicator)
  {
    if (indicator < 0) return 1;
    else return -1;
  }

  // push values on a split plane on one or the other side by 'epsilon'.

  inline bool avoid(DDouble &x, DDouble &y, DDouble &z, DDouble u, DDouble v, DDouble w, int sign = 0)
  {
    static DDouble avoidance = epsilon;
    DDouble indicator = u * x + v * y + w * z;
    int i = 0;
    if (sign == 0) sign = getSign(indicator);

    while (abs(indicator) < avoidance) { //just displace a bit in the opposite to the current pole a,b,c
      x -= sign * avoidance*u;
      y -= sign * avoidance*v;
      z -= sign * avoidance*w;
      DDouble mag = sqrt(x * x + y * y + z * z);
      x /= mag;
      y /= mag;
      z /= mag;
      DDouble lon = atan2(y, x);
      DDouble lat = atan2(z, sqrt(x * x + y * y));
      x = cos(lon) * cos(lat);
      y = sin(lon) * cos(lat);
      z = sin(lat);
      indicator = u * x + v * y + w * z;
      i++;
    }
    if (i) return true;
    else return false;
  }

  //returns the point on the clip plane located between two vertexes.

  inline void OnSphereVectorClipPlaneIntersection(DDouble x1, DDouble y1, DDouble z1, DDouble x2, DDouble y2,
      DDouble z2, DDouble u, DDouble v, DDouble w, DDouble h,
      DDouble &x, DDouble &y, DDouble &z)
  {

    DDouble a = x2 - x1;
    DDouble b = y2 - y1;
    DDouble c = z2 - z1;
    DDouble t = u * a + v * b + w * c;
    t = (-h - u * x1 - v * y1 - w * z1) / t;
    if (!finite(t)) {
      x = x1;
      y = y1;
      z = z1;
      return;
    }
    if (t < epsilon) {
      x = x1;
      y = y1;
      z = z1;
      return;
    }
    if (t > (1.0 - epsilon)) {
      x = x2;
      y = y2;
      z = z2;
      return;
    }
    x = a * t + x1;
    y = b * t + y1;
    z = c * t + z1;
    DDouble norm = sqrt(x * x + y * y + z * z);
    x /= norm;
    y /= norm;
    z /= norm;
    avoid(x,y,z,u,v,w);
    return;
  }

  // will insure that no point falls exactly on a split, since the projection errors may send the value on either side of it 
  // if we do no avoid the splits by a (rather large) margin.

  inline bool avoidSplits(std::list<Vertex>::iterator vertex, DDouble u, DDouble v, DDouble w, int sign = 0)
  {
    DDouble x, y, z;
    DDouble clon,slon,clat,slat;
    sincos(vertex->lon,&slon,&clon);
    sincos(vertex->lat,&slat,&clat);
    x = clon * clat;
    y = slon * clat;
    z = slat;
    bool b = avoid(x, y, z, u, v, w, sign);
    if (b) {
      vertex->lon = atan2(y, x);
      vertex->lat = atan2(z, sqrt(x * x + y * y));
    }
    return b;
  }

  //find the 'sign' of the initial cut. Used to force stitch values to stay on this side, for example.

  int findSign(Polygon p, DDouble u, DDouble v, DDouble w)
  {
    static DDouble avoidance = epsilon;
    DDouble x, y, z;
    DDouble clon,slon,clat,slat;

    std::list<Vertex>::iterator vertex = p.VertexList.begin();
    sincos(vertex->lon,&slon,&clon);
    sincos(vertex->lat,&slat,&clat);
    x = clon * clat;
    y = slon * clat;
    z = slat;
    DDouble indicator = u * x + v * y + w * z; 
    ++vertex;
    while (abs(indicator) < avoidance && vertex != p.VertexList.end()) {
      sincos(vertex->lon,&slon,&clon);
      sincos(vertex->lat,&slat,&clat);
      x = clon * clat;
      y = slon * clat;
      z = slat;
      indicator = u * x + v * y + w * z;
      ++vertex;
    }
    return getSign(indicator);
  }

  //returns the point on the split plane (defined by u,v,w) located between two 3d points.

  Point3d* CutPosition(const Point3d* p1, const Point3d* p2, DDouble u, DDouble v, DDouble w)
  {
    // p1 p2 define a plane--> perpendicular vector
    Point3d* p1p2 = normedCrossP(p1, p2);
    // u,v,w define the 2nd vector
    Point3d* cutplane = toPoint3d(u, v, w);
    // intersection of the 2 planes give two opposite points on sphere. We must choose.
    Point3d* cut1 = normedCrossP(p1p2, cutplane);
    Point3d* cut2 = normedCrossP(cutplane, p1p2);
    //general case: cut must be between p1 and p2
    delete p1p2;
    delete cutplane;
    if (dotP(p1, cut1) < 0) {
      delete cut1;
      return cut2;
    } else {
      delete cut2;
      return cut1;
    }
  }

  //returns the point on the split plane located between two vertexes.
  //this is a plane-vector intersection. Problem is: we cannot afford (x1,y1,z1) or (x2,y2,z2) to be exactly on the plane.
  //in this case, the result, within the numerical error, can be on the "wrong" side. Hence the 'avoid' trick.

  inline void OnSphereVectorSplitPlaneIntersection(DDouble xs, DDouble ys, DDouble zs, DDouble xe, DDouble ye,
      DDouble ze, DDouble u, DDouble v, DDouble w,
      DDouble &xb, DDouble &yb, DDouble &zb,
      DDouble &xa, DDouble &ya, DDouble &za)
  {
    //compute exact point of crossing the plane, following a great circle (3d vectors=> we follow great circles.)
    DDouble indicator = u * xs + v * ys + w * zs;
    int sign = getSign(indicator);

    Point3d* p1 = toPoint3d(xs, ys, zs);
    Point3d* p2 = toPoint3d(xe, ye, ze);
    Point3d* cut = CutPosition(p1, p2, u, v, w);
    xb = xa = cut->x;
    yb = ya = cut->y;
    zb = za = cut->z;
    avoid(xb, yb, zb, u, v, w, sign);


    indicator = u * xe + v * ye + w * ze;
    sign = getSign(indicator);
    avoid(xa, ya, za, u, v, w, sign);
    delete p1;
    delete p2;
    delete cut;
    return;
  }

  // returns the distance between end of first and start of second
  inline DDouble proximityEvaluator(const Polygon * outside, const Polygon * inside, DDouble px, DDouble py, DDouble pz, DDouble pt=0)
  {
    Vertex endout = (outside->VertexList.back());
    Vertex startin = (inside->VertexList.front());
    DDouble ret= DistanceOnSphere(endout, startin, px, py, pz, pt);
    return ret;
  }
  
  bool IsPolygonInside(const Polygon * outside, const Polygon * inside, DDouble px, DDouble py, DDouble pz, DDouble pt=0)
  { //is second inside first?
    //second inside first means that second's start and end points are inside first's start and end points, and that they are "return" polygons.
    //I.e. when closing the "outside" polygon, on encounters first the start of the inside polygon, then its end.
    //polygons that go in the other direction are not related with the current polygon.
    //we assume that we are on the sphere and on a great circle. This translates in that the angle from "first"'s start point
    //to the 2 (start,end) points of "second" is smaller than the angle btw "first"'s start point and "first"'s end point.
    //angles here are vectorial products. We need to keep the direction of future stitch, form end of "outside" to start of "ouside".
    Vertex startout = (outside->VertexList.front());
    Vertex endout = (outside->VertexList.back());
    DDouble outRange = DistanceOnSphere(endout, startout, px, py, pz, pt);
    Vertex startin = (inside->VertexList.front());
    Vertex endin = (inside->VertexList.back());
    DDouble endout2startin = DistanceOnSphere(endout, startin, px, py, pz, pt);
    DDouble endout2endin = DistanceOnSphere(endout, endin, px, py, pz, pt);
    //we go in the direction of "outside".
    bool ret = ( (outRange > 0 && endout2startin > 0 && endout2endin > 0 ) || (outRange < 0 && endout2startin <0 && endout2endin <0 ));
    if (ret) { 
     if (outRange > 0) ret = (endout2endin < outRange && endout2startin < outRange && endout2startin < endout2endin ); 
     else              ret = (outRange < endout2endin && outRange < endout2startin && endout2endin < endout2startin );
    }
//    std::cerr<<inside<<" in "<<outside<<"? [0, "<<endout2startin<<","<<endout2endin<<","<<outRange<<"] ? "<<ret<<std::endl;
    return ret;
  }
  
  //rotates repeatedly vector end of p toward start of q...
  void StitchTwoPolygons(Polygon *p, Polygon *q, DDouble a, DDouble b, DDouble c,DDouble d=0)
  {
    //stitch end of p to start of q
    Vertex endOfP = p->VertexList.back();
    Vertex startOfQ = q->VertexList.front();
    DDouble dist = DistanceOnSphere(endOfP,startOfQ,a,b,c,d); //is an angle.
    DDouble dintervals = dist / DELTA;
    //add vertexes to end of p until start of q is reached
    if (abs(dintervals) > 1) {
      //rotate xs around vector [a,b,c] by dist/nintervals
      Point3d axis={a,b,c};
      Point3d v;
      DDouble clon,slon,clat,slat;

      sincos(endOfP.lon,&slon,&clon);
      sincos(endOfP.lat,&slat,&clat);
      v.x = clon * clat;
      v.y = slon * clat;
      v.z = slat;
      int nintervals = abs(dintervals);
      for (int k = 0; k < nintervals; k++) {
        rotate3d(v,axis,dist/nintervals);
        Vertex stitch;
        stitch.lon = atan2(v.y, v.x);
        stitch.lat = atan2(v.z, sqrt(v.x * v.x + v.y * v.y));
        p->VertexList.push_back(stitch); //add at end of p
      }
    }
    if (p == q) {
      p->VertexList.push_back(startOfQ); //close contour
    } else { //add all of q at end of p;
      p->VertexList.splice(p->VertexList.end(), q->VertexList);
    }
  }
  
  //this is for cutplanes polygons. Start and End points are on the same latitude, at different longitudes. No 3d calculation.
  void StitchTwoPolygonsOnCutPlane(Polygon *p, Polygon *q)
  {
    //stitch end of p to start of q
    Vertex endOfP = p->VertexList.back();
    Vertex startOfQ = q->VertexList.front();
    //take care of modulo
    DDouble lon1,lon2;
    lon1=startOfQ.lon; if (lon1<0) lon1+=2*GDL_PI;
    lon2=endOfP.lon;if (lon2<0) lon2+=2*GDL_PI;
    DDouble dist = lon1-lon2;
    DDouble dintervals = abs(dist / DELTA);
    //add vertexes to end of p until start of q is reached
    if (dintervals > 1) { // can be cut in 2
      int nintervals = dintervals;
      dintervals = floor(dintervals);
      DDouble dlon = dist / dintervals;
      for (int k = 0; k < nintervals; k++) {
        Vertex stitch;
        stitch.lon = endOfP.lon+k*dlon;
        stitch.lat = endOfP.lat; 
        p->VertexList.push_back(stitch); //add at end of p
      }
    }
    if (p == q) {
      p->VertexList.push_back(startOfQ); //close contour
    } else { //add all of q at end of p;
      p->VertexList.splice(p->VertexList.end(), q->VertexList);
    }
  }
  
//  bool intersectsLonLatBox(DDouble minlon, DDouble maxlon, DDouble minlat, DDouble maxlat, const DDouble* llbox){
//    return !( ( (maxlon < llbox[1]) || (minlon > llbox[3]) ) && ( ( maxlat < llbox[0] ) || ( minlat > llbox[2]) ) );
//  }
  
// a predicate implemented as a function:
bool isInvalid (const Polygon& pol) { return (!pol.valid); }

  DDoubleGDL* gdlProjForward(PROJTYPE ref, DStructGDL* map, DDoubleGDL *lonsIn, DDoubleGDL *latsIn, DLongGDL *connIn,
      bool doConn, DLongGDL *&gonsOut, bool doGons, DLongGDL *&linesOut, bool doLines, bool const doFill)
  {

    //DATA MUST BE IN RADIANS
#ifdef USE_LIBPROJ4
    LPTYPE idata;
    XYTYPE odata;
#endif

    unsigned pTag = map->Desc()->TagIndex("PIPELINE");
    DDoubleGDL* pipeline = (static_cast<DDoubleGDL*> (map->GetTag(pTag, 0))->Dup());
    Guard<BaseGDL> pipeGuard(pipeline);

    unsigned llboxTag = map->Desc()->TagIndex("LL_BOX");
    DDoubleGDL* llbox = (static_cast<DDoubleGDL*> (map->GetTag(llboxTag, 0))->Dup());
    Guard<BaseGDL> llboxGuard(llbox);
    //test if we can eliminate some polygons as they are probably (this is the use of ll_box) not going to be seen at the end.
    //this has problems as ll_box is very crude and false for some projetions (satellite)
//    bool llsubset=!((*llbox)[0] <= -90.0 && (*llbox)[2] >= 90.0 && (*llbox)[1] <= -180.0 && (*llbox)[3] >= 180.0);
    
    // convert to radians
    for (int i=0; i<4; ++i) (*llbox)[i]*=DEG_TO_RAD;

    DLong dims[2];

    enum {
      EXIT = 0,
      SPLIT,
      CLIP_PLANE,
      TRANSFORM,
      CLIP_UV
    };

    dims[0] = pipeline->Dim(0);
    dims[1] = pipeline->Dim(1);
    int line = 0;
    //if pipeline is void, a TRANSFORM will be applied anyway.This test is just for that.
    bool PerformTransform = (pipeline->Sum() == 0);
    if (PerformTransform) (*pipeline)[0] = TRANSFORM; //just change value of pipeline (which is a copy)
    bool fill = (doFill || doGons);

    int icode = (*pipeline)[dims[0] * line + 0];
    DDouble a = (*pipeline)[dims[0] * line + 1]; //plane a,b,c,d
    DDouble b = (*pipeline)[dims[0] * line + 2];
    DDouble c = (*pipeline)[dims[0] * line + 3];
    DDouble d = (*pipeline)[dims[0] * line + 4];
    DDouble px = (*pipeline)[dims[0] * line + 5]; //pole x,y,z
    DDouble py = (*pipeline)[dims[0] * line + 6];
    DDouble pz = (*pipeline)[dims[0] * line + 7];
    DDouble x, y, z, before, after, xs, ys, zs, xe, ye, ze, xcutb, ycutb, zcutb, xcuta, ycuta, zcuta; //b: before, a: after cut
    OMPInt in;
    DDoubleGDL *lons;
    DDoubleGDL *lats;
    DLongGDL *currentConn;
    bool isVisible;
    DDouble clon,slon,clat,slat;
    DDouble minlon,maxlon,minlat,maxlat;

      //interpolations for GONS on cuts is every 2.5 degrees.
    //Gons takes precedence on Lines

    SizeT nEl = lonsIn->N_Elements();
    //if connectivity does not exist, fake a simple one
    if (!doConn) {
      currentConn = new DLongGDL(dimension(nEl + 1), BaseGDL::INDGEN);
      currentConn->Dec();
      (*currentConn)[0] = nEl; //[nEl,0,1...nEl]  very important!
    } else { //just copy
      currentConn = connIn->Dup();
    }

    //copy Input
    lons = lonsIn->Dup();
    lats = latsIn->Dup();


    //convert to lists
    SizeT index;
    SizeT size;
    SizeT start;
    SizeT k;

    std::list<Polygon> PolygonList;
    Polygon currentPol;
    currentPol.inside = 0;
    currentPol.outside = 0;
    currentPol.type = 0;
    currentPol.valid = true;
    std::list<Vertex> currentVertexList;
    Vertex curr;
    Vertex currstart;

    //explore conn and construct polygon list. THE CURRENT CODE DOES NOT SOLVE NESTED POYGONS ETC. (SO CONNECTIVITY IS NOT REALLY USED)
    index = 0;
    SizeT num = 0;
    while (index < currentConn->N_Elements()) {
      size = (*currentConn)[index];
      if (size > (fill) ? 2 : 1) { //two or 3 points I hope.
        start = index + 1; //start new chunk...
        num++;
        currentVertexList.clear();
         
        k = (*currentConn)[start + 0];
        minlon=maxlon=currstart.lon = (*lons)[k];
        minlat=maxlat=currstart.lat = (*lats)[k];
        currentVertexList.push_back(currstart);
        for (in = 1; in < size; in++) {
          k = (*currentConn)[start + in]; //conn is a list of indexes...
          curr.lon = (*lons)[k];
          curr.lat = (*lats)[k];
          minlon=min(minlon,curr.lon);
          minlat=min(minlat,curr.lat);
          maxlon=max(maxlon,curr.lon);
          maxlat=max(maxlat,curr.lat);
          currentVertexList.push_back(curr);
        }
        if (fill) {
          Vertex last = currentVertexList.back();
          if (!((last.lon - currstart.lon == 0.0) && (last.lat - currstart.lat == 0.0))) {//close polygon.
            Vertex curr;
            curr.lon = currstart.lon;
            curr.lat = currstart.lat;
            currentVertexList.push_back(curr);
          }
        }
//        if (llsubset) {
//          bool keep=intersectsLonLatBox(minlon,maxlon,minlat,maxlat,&((*llbox)[0]));
//          if (keep) 
//          {
//            currentPol.VertexList = currentVertexList;
//            currentPol.type = 1; //before cut
//            PolygonList.push_back(currentPol);
//          } else {
////            cerr<<"removed "<<minlon*RAD_TO_DEG<<","<<maxlon*RAD_TO_DEG<<","<<minlat*RAD_TO_DEG<<","<<maxlat*RAD_TO_DEG<<endl;
//            currentVertexList.clear();
//          }
//        } else {
            currentPol.VertexList = currentVertexList;
            currentPol.type = 1; //before cut
            PolygonList.push_back(currentPol);
//        }
      } else break;
      index += (size + 1);
    }
    GDLDelete(lons);
    GDLDelete(lats);
    GDLDelete(currentConn);

    std::list<Polygon> newPolygonList;
    std::list<Polygon> tmpPolygonList;
    
    while (icode > 0 && line < 8) {
      switch (icode) {
      case SPLIT:
        if (PolygonList.empty()) break;
        for (std::list<Polygon>::iterator p = PolygonList.begin(); p != PolygonList.end(); ++p) {

          //cut current polygon, copy in a new polygon list the cuts
          int sign = 0;
          currentVertexList.clear();
          Vertex curr;

          std::list<Vertex>::iterator v = p->VertexList.begin();
          //peculiar case: start point is on a split. We need to find the first point in the vertex list that is not on the split and "push"
          //all the previous points towards it. This is the reason of the "sign" parameter in avoidSplits(). Sign is the sign of "before" when 
          //the point giving "before" is not on the split. If every vertexes are on the split (MAP_GRID values for example), sign is 0 and the result
          //is "somewhere" but consistent. In summary: once "sign" is defined and followed, nothing should go wrong.
          sincos(v->lon,&slon,&clon);
          sincos(v->lat,&slat,&clat);
          xs = clon * clat;
          ys = slon * clat;
          zs = slat;
          before = a * xs + b * ys + c * zs;
          if (abs(before) < epsilon) sign = findSign((*p), a, b, c);
          else sign = getSign(before);
          avoidSplits(v, a, b, c, sign);
          // xs etc may have changed due to avoidSplits(), recompute.
          sincos(v->lon,&slon,&clon);
          sincos(v->lat,&slat,&clat);
          xs = clon * clat;
          ys = slon * clat;
          zs = slat;

          before = a * xs + b * ys + c * zs;

          currentPol.type = p->type; //inherit type at start
          currentPol.valid = true;
          curr.lon = v->lon;
          curr.lat = v->lat;
          currentVertexList.push_back(curr);
          for (++v; v != p->VertexList.end(); ++v) {

            avoidSplits(v, a, b, c, sign);
            sincos(v->lon,&slon,&clon);
            sincos(v->lat,&slat,&clat);
            xe = clon * clat;
            ye = slon * clat;
            ze = slat;
            after = a * xe + b * ye + c * ze;

            if (before * after < 0) {
              //cut and start a new polygon
              //find intersection
              OnSphereVectorSplitPlaneIntersection(xs, ys, zs, xe, ye, ze, a, b, c, xcutb, ycutb, zcutb, xcuta, ycuta, zcuta);

              //double dist = DistanceFromSplitPole(xcutb, ycutb, zcutb, px, py, pz);
              //SPLIT is made to cut on the opposite side of the sphere, not on all the split plane.
              if (1) { //currentPol.haspole || dist > GDL_HALFPI - epsilon ) { //not cutting everywhere would be more complicated as it implies treating correctly the polygons around the poles.
                x = xcutb;
                y = ycutb;
                z = zcutb;
                curr.lon = atan2(y, x);
                curr.lat = atan2(z, sqrt(x * x + y * y));
                currentVertexList.push_back(curr);
                //end of current Pol. Memorize cut position of first cut for cut ordering if filling occurs:
                currentPol.VertexList = currentVertexList;
                currentVertexList.clear();
                int newtype = -1 * currentPol.type;

                tmpPolygonList.push_back(currentPol);

                //create a new polygon list
                currentPol.type = newtype; //inherit type at start
                currentPol.valid = true;
                x = xcuta;
                y = ycuta;
                z = zcuta;
                curr.lon = atan2(y, x);
                curr.lat = atan2(z, sqrt(x * x + y * y));
                currentVertexList.push_back(curr);
              }
              sign = -sign; //as we are on the other side
            }
            curr.lon = v->lon;
            curr.lat = v->lat;
            currentVertexList.push_back(curr);
            before = after;
            xs = xe;
            ys = ye;
            zs = ze;
          }
          currentPol.VertexList = currentVertexList;
          currentVertexList.clear();
          tmpPolygonList.push_back(currentPol);

          //tmpPolygonList contains the current polygon, splitted. It must be stitched if filling occurs.
          //level-0 filling consist in adding last portion at beginning of first one 

          if (fill && tmpPolygonList.size() > 2) { //2 because here there has been a cut and thus two sides.
            std::list<Polygon>::iterator beg = tmpPolygonList.begin();
            std::list<Polygon>::reverse_iterator end = tmpPolygonList.rbegin();
            beg->VertexList.splice(beg->VertexList.begin(), end->VertexList); //concatenate
            tmpPolygonList.pop_back();
          }

          if (fill && tmpPolygonList.size() > 1) {
            // produce 2 lists: before and after cut
            std::list<Polygon> beforePolygonList;
            std::list<Polygon> afterPolygonList;
            for (std::list<Polygon>::iterator p = tmpPolygonList.begin(); p != tmpPolygonList.end(); ++p) {
              if (p->type == 1) {
                beforePolygonList.push_back((*p)); //on side of first vertex.
              } else {
                afterPolygonList.push_back((*p)); //on other side.
              }
            }
            tmpPolygonList.clear();

            // for each list, repeatedly find polygons that contain others. Polygons that contain no others are closed and removed.
            // polygons that contain others: sticth the one whose start is closer to the end of the container, this decreases the complexity.
            // repeat until all polygons have been stitched.
            std::list<Polygon>* aliasList;
            std::list<Polygon>* theTwoLists[] = {&beforePolygonList, &afterPolygonList};
            int maxloop = 0;
            for (int jj = 0; jj < 2; ++jj) {
              aliasList = theTwoLists[jj];
              maxloop = 0;
              do {
                //establish the complexity of each polygon: contains or/and is contained:
done:             aliasList->remove_if(isInvalid);
                  for (std::list<Polygon>::iterator q = aliasList->begin(); q != aliasList->end(); ++q) {
                  Polygon * container = &(*q);
                  // (re)establish cur's complexity number: either the polygon does not contain others, nor it is contained, and we stitch it alone
                  // or it is contained and we pass,
                  // or it is not contained and we stitch with the first it contains, then the complexity decreases and the process continues at the
                  // next iteration
                  container->inside = 0;
                  container->outside = 0;
                  for (std::list<Polygon>::iterator p = aliasList->begin(); p != aliasList->end(); ++p) {
                    Polygon * test = &(*p);
                    if (!(test == container)) { 
                      if (IsPolygonInside(container, test,a,b,c)) container->inside += 1;
                      if (IsPolygonInside(test, container,a,b,c)) container->outside += 1;
                    }
                  }
                }
//                for (std::list<Polygon>::iterator q = aliasList->begin(); q != aliasList->end(); ++q) {
//                  Polygon * container = &(*q);
//                    std::cerr << "polygon " << container << "inside: "<<container->inside <<", outside "<<container->outside <<endl;
//                }                
                //find all non-contained non-container polygons, close and remove them
                int needsUpdate=0;  
                for (std::list<Polygon>::iterator q = aliasList->begin(); q != aliasList->end(); ++q) {
                  Polygon * container = &(*q);
                  if (container->inside == 0 && container->outside == 0) { //if the polygon is alone, stitch it and pop it
//                    cerr<<" closing on itself "<<container<<endl;
                      StitchTwoPolygons(container, container, a, b, c);
                      //add closed polygon to end of newPolygonList
                      newPolygonList.push_back(*container);
                      container->valid = false;
                      needsUpdate++;
                    }
                  }
                if (needsUpdate) goto done;
                //find all containers that are not contained, they contain at least one polygon. remove the nearest inside polygon by stitching
                for (std::list<Polygon>::iterator q = aliasList->begin(); q != aliasList->end(); ++q) {
                  Polygon * container = &(*q);
                  if (container->inside > 0 && container->outside == 0) { //not contained
//                      std::cerr<<"closing container-only polygon "<<container<<endl;
                      std::list<Polygon>::iterator toStitch = aliasList->end();
                      DDouble distref = proximityEvaluator(container,container,a,b,c);
                      DDouble dist;
                      for (std::list<Polygon>::iterator p = aliasList->begin(); p != aliasList->end(); ++p) {
                        Polygon * inside = &(*p);
                        if (!(inside == container) && IsPolygonInside(container, inside,a,b,c)) {
                          dist = proximityEvaluator(container,inside,a,b,c);
                          if (dist/distref > 0 && dist/distref < 1) {
                            distref = dist;
                            toStitch = p;
                          }
                        }
                      }
//                      cerr<<" stitching "<<container<<" to " <<&(*toStitch)<< endl;
                      StitchTwoPolygons(container, &(*toStitch), a, b, c);
                      toStitch->valid = false; 
                      goto done;
                    }
                  }
                //will break on empty list
                int erase_all = 1;
                for (std::list<Polygon>::iterator q = aliasList->begin(); q != aliasList->end(); ++q) {
                  if (q->valid) erase_all *= 0;
                }
                maxloop++;

                if (maxloop > 32) {
                  erase_all = 1;
                }
                if (erase_all == 1) aliasList->clear();
              } while (!aliasList->empty());
            }

          } else {
            //just add tmpPolygonList content to end of newPolygonList
            newPolygonList.splice(newPolygonList.end(), tmpPolygonList);
            //clear tmp (normally should be empty!)
            tmpPolygonList.clear();
          }
        }
        //end of all the input list of polygons, newPolygonList contains cut and stitched polygons:
        //exchange new & old contents and void new
        PolygonList.swap(newPolygonList);
        newPolygonList.clear();

        //Should remove empty polygons: TODO
        break;
      case CLIP_PLANE:
        if (PolygonList.empty()) break;

        //copy & cut...
        for (std::list<Polygon>::iterator p = PolygonList.begin(); p != PolygonList.end(); ++p) {
          currentVertexList.clear();
          Vertex curr;

          std::list<Vertex>::iterator v = p->VertexList.begin();
          sincos(v->lon,&slon,&clon);
          sincos(v->lat,&slat,&clat);
          xs = clon * clat;
          ys = slon * clat;
          zs = slat;
          before = a * xs + b * ys + c * zs + d;
          isVisible = (before >= 0.0);
          if (isVisible) {
            curr.lon = v->lon;
            curr.lat = v->lat;
            currentVertexList.push_back(curr);
          }
          for (++v; v != p->VertexList.end(); ++v) {
            sincos(v->lon,&slon,&clon);
            sincos(v->lat,&slat,&clat);
            xe = clon * clat;
            ye = slon * clat;
            ze = slat;
            after = a * xe + b * ye + c * ze + d;

            if (before * after < 0.0) { //cut and start a new polygon
              //find intersection for clip_plane
              OnSphereVectorClipPlaneIntersection(xs, ys, zs, xe, ye, ze, a, b, c, d, xcuta, ycuta, zcuta);
              if (isVisible) {
                x = xcuta;
                y = ycuta;
                z = zcuta;
                curr.lon = atan2(y, x);
                curr.lat = atan2(z, sqrt(x * x + y * y));
                currentVertexList.push_back(curr);
                //end of current Pol.
                currentPol.VertexList = currentVertexList;
                currentVertexList.clear();
                tmpPolygonList.push_back(currentPol);
              }
              isVisible = !isVisible;
              if (isVisible) {
                //create a new polygon list
                x = xcuta;
                y = ycuta;
                z = zcuta;
                curr.lon = atan2(y, x);
                curr.lat = atan2(z, sqrt(x * x + y * y)); //asin( z );
                currentVertexList.push_back(curr);
              }
            } else if (isVisible) {
              curr.lon = v->lon;
              curr.lat = v->lat;
              currentVertexList.push_back(curr);
            }
            before = after;
            xs = xe;
            ys = ye;
            zs = ze;
          }
          if (isVisible) {
            currentPol.VertexList = currentVertexList;
            currentVertexList.clear();
            tmpPolygonList.push_back(currentPol);
          }
          
          if (fill && tmpPolygonList.size() > 1) {
            std::list<Polygon>::iterator beg = tmpPolygonList.begin();
            std::list<Polygon>::reverse_iterator end = tmpPolygonList.rbegin();
            beg->VertexList.splice(beg->VertexList.begin(), end->VertexList); //concatenate
            tmpPolygonList.pop_back();
          }

          if (fill) {
          
          int maxloop = 0;
              do {
                //same procedure as for SPLITs except for stitch.
done2:            tmpPolygonList.remove_if(isInvalid);
                  for (std::list<Polygon>::iterator q = tmpPolygonList.begin(); q != tmpPolygonList.end(); ++q) {
                  Polygon * container = &(*q);
                  container->inside = 0;
                  container->outside = 0;
                  for (std::list<Polygon>::iterator p = tmpPolygonList.begin(); p != tmpPolygonList.end(); ++p) {
                    Polygon * test = &(*p);
                    if (!(test == container)) { 
                      if (IsPolygonInside(container, test,a,b,c)) container->inside += 1;
                      if (IsPolygonInside(test, container,a,b,c)) container->outside += 1;
                    }
                  }
                }
                int needsUpdate=0;  
                for (std::list<Polygon>::iterator q = tmpPolygonList.begin(); q != tmpPolygonList.end(); ++q) {
                  Polygon * container = &(*q);
                  if (container->inside == 0 && container->outside == 0) {
                      StitchTwoPolygons(container, container, a, b, c, d);
                      newPolygonList.push_back(*container);
                      container->valid = false;
                      needsUpdate++;
                    }
                  }
                if (needsUpdate) goto done2;
                for (std::list<Polygon>::iterator q = tmpPolygonList.begin(); q != tmpPolygonList.end(); ++q) {
                  Polygon * container = &(*q);
                  if (container->inside > 0 && container->outside == 0) { 
                      std::list<Polygon>::iterator toStitch = tmpPolygonList.end();
                      DDouble distref = proximityEvaluator(container,container,a,b,c,d);
                      DDouble dist;
                      for (std::list<Polygon>::iterator p = tmpPolygonList.begin(); p != tmpPolygonList.end(); ++p) {
                        Polygon * inside = &(*p);
                        if (!(inside == container) && IsPolygonInside(container, inside,a,b,c)) {
                          dist = proximityEvaluator(container,inside,a,b,c,d);
                          if (dist/distref > 0 && dist/distref < 1) {
                            distref = dist;
                            toStitch = p;
                          }
                        }
                      }
                      StitchTwoPolygons(container, &(*toStitch), a, b, c, d);
                      toStitch->valid = false; 
                      goto done2;
                    }
                  }
                int erase_all = 1;
                for (std::list<Polygon>::iterator q = tmpPolygonList.begin(); q != tmpPolygonList.end(); ++q) {
                  if (q->valid) erase_all *= 0;
                }
                maxloop++;

                if (maxloop > 32) {
                  erase_all = 1;
                }
                if (erase_all == 1) tmpPolygonList.clear();
              } while (!tmpPolygonList.empty());


          }else {
            //just add tmpPolygonList content to end of newPolygonList
            newPolygonList.splice(newPolygonList.end(), tmpPolygonList);
            //clear tmp (normally should be empty!)
            tmpPolygonList.clear();
          }





        }
        //exchange new & old contents and void new
        if (newPolygonList.empty()) PolygonList.clear();
        else PolygonList.swap(newPolygonList);
        newPolygonList.clear();

        break;
      case TRANSFORM:
        if (PolygonList.empty()) break;

#ifdef USE_LIBPROJ4
        for (std::list<Polygon>::iterator p = PolygonList.begin(); p != PolygonList.end(); ++p) {
          for (std::list<Vertex>::iterator v = p->VertexList.begin(); v != p->VertexList.end(); ++v) {
            idata.u = v->lon;
            idata.v = v->lat;
            odata = PJ_FWD(idata, ref);
            v->lon = odata.u;
            v->lat = odata.v;
          }
        }
#endif   //USE_LIBPROJ4 
        break;
      case CLIP_UV:
        //TO BE DONE (really useful?)
//        if (PolygonList.empty()) break;
        break;
      default:
        continue;
      }
      line++;
      icode = (*pipeline)[dims[0] * line + 0];
      a = (*pipeline)[dims[0] * line + 1]; //plane a,b,c,d
      b = (*pipeline)[dims[0] * line + 2];
      c = (*pipeline)[dims[0] * line + 3];
      d = (*pipeline)[dims[0] * line + 4];
      px = (*pipeline)[dims[0] * line + 5]; //pole x,y,z
      py = (*pipeline)[dims[0] * line + 6];
      pz = (*pipeline)[dims[0] * line + 7];
    }
    //recreate lons, lats, gons, ..
    if (PolygonList.empty()) {
      if (doGons) gonsOut = new DLongGDL(-1);
      else linesOut = new DLongGDL(-1);
      return new DDoubleGDL(-1);
    }

    //size
    SizeT nelem = 0;
    SizeT ngons = 0;

    for (std::list<Polygon>::iterator p = PolygonList.begin(); p != PolygonList.end(); ++p) {
      if (p->VertexList.size() > 0) {
        ngons++;
        ngons += p->VertexList.size();
        nelem += p->VertexList.size();
      }
    }
    lons = new DDoubleGDL(nelem, BaseGDL::NOZERO);
    lats = new DDoubleGDL(nelem, BaseGDL::NOZERO);
    currentConn = new DLongGDL(ngons, BaseGDL::NOZERO);
    SizeT i = 0;
    SizeT j = 0;
    for (std::list<Polygon>::iterator p = PolygonList.begin(); p != PolygonList.end(); ++p) {
      if (p->VertexList.size() > 0) {
        (*currentConn)[j++] = p->VertexList.size();
        for (std::list<Vertex>::iterator v = p->VertexList.begin(); v != p->VertexList.end(); ++v, i++) {
          (*lons)[i] = v->lon;
          (*lats)[i] = v->lat;
          (*currentConn)[j++] = i;
        }
        //delete vertexlist
        p->VertexList.clear();
      }
    }
    //finally, delete polygon list.
    PolygonList.clear();

    nEl = lons->N_Elements();
    DLong odims[2];
    odims[0] = 2;
    odims[1] = nEl;
    dimension dim(odims, 2);
    DDoubleGDL *res = new DDoubleGDL(dim, BaseGDL::NOZERO);
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for (OMPInt i = 0; i < nEl; ++i) {
        (*res)[2 * i] = (*lons)[i];
        (*res)[2 * i + 1] = (*lats)[i];
      }
    }
    //cleanup
    GDLDelete(lons);
    GDLDelete(lats);
    if (doGons || doLines) {
      if (doGons) gonsOut = currentConn;
      else linesOut = currentConn;
    } else GDLDelete(currentConn);

    return res;
  }




} // namespace

