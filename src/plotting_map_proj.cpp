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
#include "plotting.hpp"

namespace lib {

  using namespace std;

  static bool isRot;
  static bool noInv;
  static DDouble sRot,cRot;

BaseGDL* map_proj_forward_fun( EnvT* e ) {
#ifdef USE_LIBPROJ4
  BaseGDL* p0;
  BaseGDL* p1;

  DDoubleGDL *lon;
  DDoubleGDL *lat;
  Guard<DDoubleGDL> lonGuard, latGuard;

  DLongGDL* gons=NULL;
  DLongGDL* lines=NULL;
  DLongGDL* connectivity=NULL;
  DDoubleGDL *res=NULL;
  SizeT nEl;

  SizeT nParam = e->NParam( );
  if ( nParam < 1 || nParam > 2 )
    e->Throw( "Incorrect number of arguments." );

  //RADIANS
  static int radianIx = e->KeywordIx( "RADIANS" );
  bool radians = e->KeywordSet( radianIx );

  // Get MATRIX 
  bool externalMap;
  DStructGDL* map = GetMapAsMapStructureKeyword(e, externalMap);
  ref = map_init( map );
  if ( ref == NULL ) {
    e->Throw( "Projection initialization failed." );
  }

  // keywords "POLYGONS", "POLYLINES" and "FILL"
  static int gonsIx = e->KeywordIx( "POLYGONS" );
  bool doGons = e->KeywordPresent( gonsIx );
  static int linesIx = e->KeywordIx( "POLYLINES" );
  bool doLines = e->KeywordPresent( linesIx );
  static int fillIx = e->KeywordIx( "FILL" );
  bool doFill = e->KeywordSet( fillIx );
  
  //keyword CONNECTIVITY
  static int connIx = e->KeywordIx( "CONNECTIVITY" );
  bool doConn = e->KeywordPresent( connIx );
  if ( doConn ) connectivity=e->GetKWAs<DLongGDL>(connIx);

  //with connectivity, polygons or polylines, and 1 argument, dimension MUST be [2,*]
  bool fussy=(doConn || doGons || doLines);
  
  //Get arguments
  if ( nParam == 1 ) { //lat is not present...
    p0 = e->GetParDefined( 0 );
    DDoubleGDL* ll = static_cast<DDoubleGDL*> (p0->Convert2( GDL_DOUBLE, BaseGDL::COPY ));
    if (fussy) {
      if (p0->Rank() != 2) e->Throw( "(X,Y) array must be (2,N).");
      if (p0->Dim(0) != 2) e->Throw( "(X,Y) array must be (2,N).");
    }
    nEl = p0->N_Elements( ) / 2; //as simple as that

    lon = new DDoubleGDL( dimension( nEl ), BaseGDL::NOZERO );    lonGuard.Reset( lon );
    lat = new DDoubleGDL( dimension( nEl ), BaseGDL::NOZERO );    latGuard.Reset( lat );

#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for ( OMPInt i = 0; i < nEl; ++i ) {
        (*lon)[i] = (*ll)[2 * i] * ((radians) ? 1.0 : DEG_TO_RAD);
        (*lat)[i] = (*ll)[2 * i + 1] * ((radians) ? 1.0 : DEG_TO_RAD);
      }
    }
  } else { //nParam==2
    p0 = e->GetParDefined( 0 );
    nEl = p0->N_Elements( );
    p1 = e->GetParDefined( 1 );
    if ( p1->N_Elements( ) != nEl ) e->Throw( "X & Y arrays must have same number of points." );
    //allocate arrays
    DDoubleGDL *tmplon = static_cast<DDoubleGDL*> (p0->Convert2( GDL_DOUBLE, BaseGDL::COPY ));
    DDoubleGDL *tmplat = static_cast<DDoubleGDL*> (p1->Convert2( GDL_DOUBLE, BaseGDL::COPY ));
    lon = new DDoubleGDL( dimension( nEl ), BaseGDL::NOZERO );    lonGuard.Reset( lon );
    lat = new DDoubleGDL( dimension( nEl ), BaseGDL::NOZERO );    latGuard.Reset( lat );

#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for ( OMPInt i = 0; i < nEl; ++i ) {
        (*lon)[i] = (*tmplon)[i] * ((radians)? 1: DEG_TO_RAD);
        (*lat)[i] = (*tmplat)[i] * ((radians)? 1: DEG_TO_RAD);
      }
    }
  }

  if ( doGons || doLines ) {
    res=gdlProjForward(ref, map, lon,lat, connectivity, doConn, gons, doGons, lines, doLines, doFill);
    if ( doGons ) e->SetKW( gonsIx, gons ); else e->SetKW( linesIx, lines ); 
  } else {  //do it oursef
    LPTYPE idata;
    XYTYPE odata;
    DLong dims[2];
    dims[0] = 2;
    dims[1] = nEl;
    dimension dim( dims, 2 );
    res = new DDoubleGDL( dim, BaseGDL::NOZERO );
#ifdef PROJ_IS_THREADSAFE
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
#endif
      for ( OMPInt i = 0; i < nEl; ++i ) {
        idata.u = (*lon)[i];
        idata.v = (*lat)[i];
        odata = PJ_FWD( idata, ref );
        (*res)[2 * i] = odata.u;
        (*res)[2 * i + 1] = odata.v;
      }
    }
#ifdef PROJ_IS_THREADSAFE
  }
#endif
  return res;
#else
  e->Throw( "GDL was compiled without support for map projections" );
  return NULL;
#endif
}

  BaseGDL* map_proj_inverse_fun(EnvT* e) {
#ifdef USE_LIBPROJ4
    // xy -> lonlat
    SizeT nParam = e->NParam();
    if (nParam < 1 || nParam > 2)
      e->Throw("Incorrect number of arguments.");

    static int radianIx = e->KeywordIx("RADIANS");
    bool radians=e->KeywordSet(radianIx);
    
  // Get MATRIX 
  bool externalMap;
  DStructGDL* map = GetMapAsMapStructureKeyword(e, externalMap);
  ref = map_init( map );
  if ( ref == NULL ) {
    e->Throw( "Projection initialization failed." );
  }

//protect against projections that have no inverse in proj.4 (and inverse in libproj) (silly, is'nt it?) (I guess I'll copy all
//this code one day and make our own certified version!
    if (noInv )  e->Throw("The proj4 library version you use unfortunately defines no inverse for this projection!");
  
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
        (*res)[2 * i] = odata.u * ((radians)?1.0:RAD_TO_DEG);
        (*res)[2 * i + 1] = odata.v * ((radians)?1.0:RAD_TO_DEG);
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

#ifdef PROJ_IS_THREADSAFE
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for 
#endif
        for (OMPInt i = 0; i < nEl; ++i) {
        idata.u = (*x)[i];
        idata.v = (*y)[i];
        odata = PJ_INV(idata, ref);
        (*res)[2 * i] = odata.u * ((radians)?1.0:RAD_TO_DEG);
        (*res)[2 * i + 1] = odata.v * ((radians)?1.0:RAD_TO_DEG);
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
  PROJTYPE map_init(DStructGDL * map) {
    //enum all the projections

    enum {
      Invalid = 0,
      Stereographic,
      Orthographic,
      LambertConic,
      LambertAzimuthal,
      Gnomonic,
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
    SPHE,
    CLON,
    CLAT,
    SPR1,
    SPR2,
    HGHT,
    TILT,
    CAZM,
    SMAJ,
    SMIN,
    SFAC,
    ZONE,
    FE,
    FN,
    SLAT,
    SPAR,
    LON1,
    LON2,
    AZML,
    SATN,
    PATH,
    SHPM,
    SHPN,
    GSSM
  } projElementIndex;
  int isAngle[]={-100,0,1,1,1,1,0,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0 };
  string projElement[] = { //same order that projElementIndex, please!
    "Null", " +R=",    " +lon_0=",    " +lat_0=",    " +lat_1=",    " +lat_2=",
    " +h=",    " +tilt=",    " +alpha=",    " +a=",    " +b=",    " +k0=",    " +zone=",    " +x_0=",
    " +y_0=",    " +lat_ts=",    " +lat_0=",    " +lon_1=",    " +lon_2=",    " +lonc=",
    " +lsat=",    " +path=",    " +m=",    " +n=", " +n=4 +m="};
  string obliqueProjElement[] = { //same order that projElementIndex, please!
    "Null", " +R=",    " +o_lon_p=",    " +o_lat_p=",    " +lat_1=",    " +lat_2=",
    " +h=",    " +tilt=",    " +alpha=",    " +a=",    " +b=",    " +k0=",    " +zone=",    " +x_0=",
    " +y_0=",    " +lat_ts=",    " +lat_0=",    " +lon_1=",    " +lon_2=",    " +lonc=",
    " +lsat=",    " +path=",    " +m=",    " +n=", " +n=4 +m="};  
  typedef struct {
    int projindex;
    int projnum;
    int projtype;
    string projname;
    string proj4name;
    int numopts;
    int theCode[16];
    int theVarCode[16];
  } projCoding;

  static projCoding projectionOptions[]={
    // index, proj. Number as in doc, Name, Proj.4 litle name, number of projElements to read, 
    // Values as index in projElements (options for proj.4), idem for variant.
    {0  ,   0 ,0,"Invalid Projection", "none", 0 , {0},{0}},
    {1  ,   1 ,0,"Stereographic" , "stere" , 6 , {SPHE , 0 , 0 , 0 , CLON , CLAT , 0},{0}} , 
    {2  ,   2 ,0,"Orthographic" , "ortho" , 6 , {SPHE , 0 , 0 , 0 , CLON , CLAT , 0},{0}} , 
    {3  ,   3 ,1,"LambertConic" , "lcc" , 6 , {SPHE , 0 , SPR1 , SPR2 , CLON , CLAT , 0},{0}} , 
    {4  ,   4 ,0,"LambertAzimuthal" , "laea" , 6 , {SPHE , 0 , 0 , 0 , CLON , CLAT , 0},{0}} , 
    {5  ,   5 ,0,"Gnomonic" , "gnom" , 6 , {SPHE , 0 , 0 , 0 , CLON , CLAT , 0},{0}} , 
    {6  ,   6 ,0,"AzimuthalEquidistant" , "aeqd" , 6 , {SPHE , 0 , 0 , 0 , CLON , CLAT , 0},{0}} , 
    {7  ,   7 ,0,"Satellite" , "tpers" , 6 , {SPHE , 0 , HGHT , TILT , CLON , CLAT , 0},{0}} , 
    {8  ,   8 ,2,"Cylindrical" , "eqc" , 6 , {SPHE , 0 , 0 , CAZM , CLON , CLAT , 0},{0}} , 
    {9  ,   9 ,2,"Mercator" , "merc" , 6 , {SPHE , 0 , 0 , CAZM , CLON , CLAT , 0},{0}} , 
    {10 ,  10 ,4,"Mollweide" , "moll" , 6 , {SPHE , 0 , 0 , CAZM , CLON , CLAT , 0},{0}} , 
    {11 ,  11 ,4,"Sinusoidal" , "sinu" , 6 , {SPHE , 0 , 0 , CAZM , CLON , CLAT , 0},{0}} , 
    {12 ,  12 ,3,"Aitoff" , "aitoff" , 6 , {SPHE , 0 , 0 , 0 , CLON , CLAT , 0},{0}} , 
    {13 ,  13 ,3,"HammerAitoff" , "hammer" , 6 , {SPHE , 0 , 0 , 0 , CLON , CLAT , 0},{0}} , 
    {14 ,  14 ,1,"AlbersEqualAreaConic" , "aea" , 6 , {SPHE , 0 , SPR1 , SPR2 , CLON , CLAT , 0},{0}} , 
    {15 ,  15 ,2,"TransverseMercator" , "tmerc" , 6 , {SMAJ , SMIN , SFAC , 0 , CLON , CLAT , 0},{0}} , 
    {16 ,  16 ,2,"MillerCylindrical" , "mill" , 6 , {SPHE , 0 , 0 , CAZM , CLON , CLAT , 0},{0}} , 
    {17 ,  17 ,4,"Robinson" , "robin" , 6 , {SPHE , 0 , 0 , CAZM , CLON , CLAT , 0},{0}} , 
    {18 ,  18 ,1,"LambertEllipsoidConic" , "lcc" , 6 , {SMAJ , SMIN , SPR1 , SPR2 , CLON , CLAT , 0},{0}} , 
    {19 ,  19 ,4,"GoodesHomolosine" , 
#ifdef USE_LIBPROJ4_NEW
    "igh" ,
#else
    "goode" ,
#endif
    5 , {SPHE , 0 , 0 , 0 , CLON , 0 , 0},{0}} , 
    {20 , 100 ,5,"Geographic" , "lonlat" , 0 , {SMAJ , SMIN , 0 , 0 , 0 , 0 , 0 , 0},{0}} , 
    {21 , 101 ,5,"GCTP_UTM" , "utm" , 3 , {CLON , CLAT , ZONE , 0 , 0 , 0 , 0 , 0},{0}} ,   //may use "+south"
    {22 , 102 ,2,"GCTP_StatePlane" , "utm" , 3 , {SMAJ , SMIN , ZONE , 0 , 0 , 0 , 0 , 0},{0}} ,  //may use "+south"
    {23 , 103 ,1,"GCTP_AlbersEqualArea" , "aea" , 8 , {SMAJ , SMIN , SPR1 , SPR2 , CLON , CLAT , FE , FN},{0}} , 
    {24 , 104 ,1,"GCTP_LambertConformalConic" , "lcc" , 8 , {SMAJ , SMIN , SPR1 , SPR2 , CLON , CLAT , FE , FN},{0}} , 
    {25 , 105 ,2,"GCTP_Mercator" , "merc" , 8 , {SMAJ , SMIN , 0 , 0 , CLON , SLAT , FE , FN},{0}} , 
    {26 , 106 ,0,"GCTP_PolarStereographic" , "ups" , 9 , {SMAJ , SMIN , 0 , 0 , CLON , SLAT , FE , FN , CLAT},{0}} ,  //may use "+south"
    {27 , 107 ,1,"GCTP_Polyconic" , "poly" , 8 , {SMAJ , SMIN , 0 , 0 , CLON , CLAT , FE , FN},{0}} , 
    {28 , 108 ,1,"GCTP_EquidistantConic" , "eqdc" , 8 , {SMAJ , SMIN , SPAR , 0 , CLON , CLAT , FE , FN , 0}, {SMAJ , SMIN , SPR1 , SPR2 , CLON , CLAT , FE , FN , 0}} , 
    {29 , 109 ,2,"GCTP_TransverseMercator" , "tmerc" , 8 , {SMAJ , SMIN , SFAC , 0 , CLON , CLAT , FE , FN},{0}} , 
    {30 , 110 ,0,"GCTP_Stereographic" , "stere" , 8 , {SPHE , 0 , 0 , 0 , CLON , CLAT , FE , FN},{0}} , 
    {31 , 111 ,0,"GCTP_LambertAzimutha" , "laea" , 8 , {SMAJ , SMIN , 0 , 0 , CLON , CLAT , FE , FN},{0}} , 
    {32 , 112 ,0,"GCTP_Azimuthal" , "aeqd" , 8 , {SPHE , 0 , 0 , 0 , CLON , CLAT , FE , FN},{0}} , 
    {33 , 113 ,0,"GCTP_Gnomonic" , "gnom" , 8 , {SPHE , 0 , 0 , 0 , CLON , CLAT , FE , FN},{0}} , 
    {34 , 114 ,0,"GCTP_Orthographic" , "ortho" , 8 , {SPHE , 0 , 0 , 0 , CLON , CLAT , FE , FN},{0}} , 
    {35 , 115 ,0,"GCTP_NearSidePerspective" , "nsper" , 8 , {SPHE , 0 , HGHT , 0 , CLON , CLAT , FE , FN},{0}} , 
    {36 , 116 ,4,"GCTP_Sinusoidal" , "sinu" , 8 , {SPHE , 0 , 0 , 0 , CLON , 0 , FE , FN},{0}} , 
    {37 , 117 ,2,"GCTP_Equirectangular" , "eqc" , 8 , {SPHE , 0 , 0 , 0 , CLON , SLAT , FE , FN},{0}} , 
    {38 , 118 ,2,"GCTP_MillerCylindrical" , "mill" , 8 , {SPHE , 0 , 0 , 0 , CLON , 0 , FE , FN},{0}} , 
    {39 , 119 ,3,"GCTP_VanderGrinten" , "vandg" , 8 , {SPHE , 0 , 0 , 0 , CLON , CLAT , FE , FN},{0}} , 
    {40 , 120 ,2,"GCTP_HotineObliqueMercator" , "omerc" , 12 , {SMAJ , SMIN , SFAC , 0 , 0 , CLAT , FE , FN , LON1 , SPR1 , LON2 , SPR2 , 0},{SMAJ , SMIN , SFAC , CAZM , AZML , CLAT , FE , FN , 0 , 0 , 0 , 0 , 0}} , 
    {41 , 121 ,4,"GCTP_Robinson" , "robin" , 8 , {SPHE , 0 , 0 , 0 , CLON , 0 , FE , FN},{0}} , 
    {42 , 122 ,2,"GCTP_SpaceObliqueMercator" , "lsat" , 12 , {SMAJ , SMIN , 0 , 0 , 0 , 0 , FE , FN , 0 , 0 , 0 , 0 , 0},{SMAJ , SMIN , SATN , PATH , 0 , 0 , FE , FN , 0 , 0 , 0 , 0 , 0}} , 
    {43 , 123 ,0,"GCTP_AlaskaConformal" , "alsk" , 8 , {SMAJ , SMIN , 0 , 0 , 0 , 0 , FE , FN},{0}} , 
    {44 , 124 ,4,"GCTP_InterruptedGoode" , "igh" , 8 , {SPHE , 0 , 0 , 0 , 0 , 0 , 0 , 0},{0}} , 
    {45 , 125 ,4,"GCTP_Mollweide" , "moll" , 8 , {SPHE , 0 , 0 , 0 , CLON , 0 , FE , FN},{0}} , 
    {46 , 126 ,4,"GCTP_InterruptedMollweide" , "moll" , 8 , {SPHE , 0 , 0 , 0 , 0 , 0 , 0 , 0},{0}} , 
    {47 , 127 ,4,"GCTP_Hammer" , "hammer" , 8 , {SPHE , 0 , 0 , 0 , CLON , 0 , FE , FN},{0}} , 
    {48 , 128 ,4,"GCTP_WagnerIV" , "wag4" , 8 , {SPHE , 0 , 0 , 0 , CLON , 0 , FE , FN},{0}} , 
    {49 , 129 ,3,"GCTP_WagnerVII" , "wag7" , 8 , {SPHE , 0 , 0 , 0 , CLON , 0 , FE , FN},{0}} , 
    {50 , 130 ,3,"GCTP_OblatedEqualArea" , "oea" , 9 , {SPHE , 0 , SHPM , SHPN , CLON , CLAT , FE , FN , 0},{0}} , 
    {51 , 131 ,9,"GCTP_IntegerizedSinusoidal" , "yet_unknown" , 11 , {SPHE , 0 , 0 , 0 , CLON , 0 , FE , FN , GSSM , 0 , 0},{0}} , 
    {52 , 132 ,2,"GCTP_CylindricalEqualArea" , "cea" , 8 , {SMAJ , SMIN , SPAR , 0 , CLON , 0 , FE , FN},{0}}
  };

  bool variant=false;

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
    DDouble map_p0lon = (*static_cast<DDoubleGDL*> (map->GetTag(p0lonTag, 0)))[0];
    DDouble map_p0lat = (*static_cast<DDoubleGDL*> (map->GetTag(p0latTag, 0)))[0];
    DDouble map_a = (*static_cast<DDoubleGDL*> (map->GetTag(aTag, 0)))[0];
    DDouble map_e2 = (*static_cast<DDoubleGDL*> (map->GetTag(e2Tag, 0)))[0];
    DDoubleGDL* map_p = (static_cast<DDoubleGDL*> (map->GetTag(pTag, 0)));
    DDouble map_lat1 = (*static_cast<DDoubleGDL*> (map->GetTag(pTag, 0)))[3];
    DDouble map_lat2 = (*static_cast<DDoubleGDL*> (map->GetTag(pTag, 0)))[4];
    DString projName = (*static_cast<DStringGDL*> (map->GetTag(projNameTag, 0)))[0];
    
    static char *parms[32];//parameters for LIBPROj.4, old style
    DLong nparms = 0;
      
    //GCTP support
    if (map_projection == 20) {
      map_projection = (*static_cast<DLongGDL*> (map->GetTag(simpleTag, 0)))[0] + GoodesHomolosine + 1;
      if ((*static_cast<DDoubleGDL*> (map->GetTag(pTag, 0)))[projectionOptions[map_projection].numopts] == 1) variant=true; //seen tag B variant
    }
    
    
    if (map_projection <1) return NULL;

//protect against projections that have no inverse in proj.4 (and inverse in libproj) (silly, is'nt it?) (I guess I'll copy all
//this code one day and make our own certified version!
    noInv=FALSE;
    if (map_projection == 12 || map_projection == 13 || map_projection == 47 || map_projection == 49) {
      noInv=TRUE;
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
    char azi[64];

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
    static DDoubleGDL* last_p = new DDoubleGDL(16, BaseGDL::INDGEN);
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
    for (SizeT i = 0; i < 16; ++i) if ((*static_cast<DDoubleGDL*> (last_p))[i] != (*static_cast<DDoubleGDL*> (map_p))[i]) redo = true;

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
      for (SizeT i = 0; i < 16; ++i) (*static_cast<DDoubleGDL*> (last_p))[i] = (*static_cast<DDoubleGDL*> (map_p))[i];


      if (map_rot != 0.0) 
      {
        isRot = true;
        sRot=sin(map_rot*DEG_TO_RAD);
        cRot=cos(map_rot*DEG_TO_RAD);
      } else isRot=false;

      //Trick for using ALL the libproj.4 projections.

      if (map_projection == 999) { //our special code
#ifdef USE_LIBPROJ4_NEW
      prev_ref = pj_init_plus(projName.c_str());
      if (!prev_ref) {Warning("LIBPROJ.4 returned error message: "); Warning(pj_strerrno(pj_errno));}
#else
      prev_ref = proj_initstr((char*)projName.c_str());
      if (!prev_ref) {Warning("LIBPROJ.4 returned error message: "); Warning(proj_strerrno(proj_errno));}
#endif
      return prev_ref;
      }

      if ( projectionOptions[map_projection].projtype > 1  && abs(map_p0lat)>1 ) trans=true;
      DDouble val;
      //GCTP Projections only:
      std::string projCommand;
      projCommand+=(trans?"+proj=ob_tran +o_proj=":" +proj=")+projectionOptions[map_projection].proj4name;
      for (int i=0; i<projectionOptions[map_projection].numopts; i++) {
        if (variant) {
          if (projectionOptions[map_projection].theVarCode[i] != 0) {
            val=(*static_cast<DDoubleGDL*> (map_p))[i];
            if (isAngle[projectionOptions[map_projection].theVarCode[i]] == 1) val *= ((map_projection > GoodesHomolosine)?1.0E-6:RAD_TO_DEG);
            projCommand+=trans?obliqueProjElement[projectionOptions[map_projection].theVarCode[i]]:projElement[projectionOptions[map_projection].theVarCode[i]];
            projCommand+=i2s(val);
          }
        } else {
          if (projectionOptions[map_projection].theCode[i] != 0) {
            val=(*static_cast<DDoubleGDL*> (map_p))[i];
            if (
            ( (map_projection==GCTP_UTM || map_projection==GCTP_StatePlane) && i==2) ||
            (map_projection==GCTP_PolarStereographic && i==8)
            ) {
              if (val < 0) { //negative Zone is South!
                val *= -1.0;
                projCommand+=" +south";
              }
            }
            if (isAngle[projectionOptions[map_projection].theCode[i]] == 1) val *= ((map_projection > GoodesHomolosine)?1.0E-6:RAD_TO_DEG);
            projCommand+=trans?obliqueProjElement[projectionOptions[map_projection].theCode[i]]:projElement[projectionOptions[map_projection].theCode[i]];
            projCommand+=i2s(val);
          }
        }
      }

      if (map_projection > GoodesHomolosine) { //use projComand automatically defined above
//      cout<<projCommand<<endl;
#ifdef USE_LIBPROJ4_NEW
      prev_ref = pj_init_plus(projCommand.c_str());
      if (!prev_ref) {Warning("LIBPROJ.4 returned error message: "); Warning(pj_strerrno(pj_errno));}
#else
      prev_ref = proj_initstr((char*)projCommand.c_str());
      if (!prev_ref) {Warning("LIBPROJ.4 returned error message: "); Warning(proj_strerrno(proj_errno));}
#endif
      return prev_ref;
      }
      
      //only classical projections here [1..19]
      if (trans){ //use rotation variant, a killer with some projections
//        if (map_p0lon==180.0) map_p0lon=179.999992;
        if ( map_p0lat > 89.999 ) map_p0lat = 89.999; //take some precautions as PROJ.4 is not proetected!!! 
        if ( map_p0lat < -89.999 ) map_p0lat = -89.999;
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

      parms[nparms++] = &a[0];
      if (map_e2 != 0.0) parms[nparms++] = &e2[0];

      if (trans
      && map_projection != Satellite
      && map_projection != GCTP_WagnerVII //no invert in old and new libproj4.
#ifdef USE_LIBPROJ4_NEW
      && map_projection != Mercator  //crashes with map_grid!!!
      && map_projection != TransverseMercator //idem!!
      && map_projection != Orthographic //idem!!
      && map_projection != Aitoff //idem!!
#endif
      ) {
        strcpy(ob_proj, "proj=ob_tran");
        parms[nparms++] = &ob_proj[0];
        sprintf(proj, "o_proj=%s", projectionOptions[map_projection].proj4name.c_str());
        parms[nparms++] = &proj[0];
        parms[nparms++] = &ob_lon[0];
        parms[nparms++] = &ob_lat[0];
        parms[nparms++] = &p0lon[0];
        parms[nparms++] = &p0lat[0];
      } else {
        sprintf(proj, "proj=%s", projectionOptions[map_projection].proj4name.c_str());
        parms[nparms++] = &proj[0];
        parms[nparms++] = &p0lon[0];
        parms[nparms++] = &p0lat[0];
      }

      switch (map_projection) {
        case Mercator:
            sprintf(lat_ts,"lat_ts=%lf",map_lat1 * RAD_TO_DEG);
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
          sprintf(h, "h=%lf", ((*static_cast<DDoubleGDL*> (map_p))[0]));
          parms[nparms++] = &h[0];
          sprintf(tilt, "tilt=%lf", RAD_TO_DEG * atan2((*static_cast<DDoubleGDL*> (map_p))[2], (*static_cast<DDoubleGDL*> (map_p))[3]));
          parms[nparms++] = &tilt[0];
//          sprintf(azi, "azi=%lf", map_rot);
//          parms[nparms++] = &azi[0]; 
          break;
      }
//
//      fprintf(stderr, "nparms=%d:", nparms);
//      for (SizeT i = 0; i < nparms; ++i) fprintf(stderr, "+%s ", parms[i]);
//      fprintf(stderr, "\n");
#ifdef USE_LIBPROJ4_NEW
      prev_ref = PJ_INIT(nparms, parms);
      if (!prev_ref) {Warning("LIBPROJ.4 returned error message: "); Warning(pj_strerrno(pj_errno));}
#else
      prev_ref = PJ_INIT(nparms, parms);
      if (!prev_ref) {Warning("LIBPROJ.4 returned error message: "); Warning(proj_strerrno(proj_errno));}
#endif      
    }
    return prev_ref;
  }

#ifdef USE_LIBPROJ4_NEW

PROJDATA protect_proj_fwd( PROJDATA idata, PROJTYPE proj ) {
  XYTYPE odata;
  DDouble u,v;
  if ( finite( (idata.u)*(idata.v) ) ) {
    odata = LIB_PJ_FWD( idata, proj );
    if (finite(odata.u) && odata.u != HUGE_VAL){
      if(isRot) {
        u=odata.u;
        v=odata.v;
        odata.u=u*cRot+v*sRot;
        odata.v=-u*sRot+v*cRot;
      }
      return odata;
    }
  }
  return badProj;
}

PROJDATA protect_proj_inv( PROJDATA idata, PROJTYPE proj ) {
  if (noInv )  return badProj ;
//  throw GDLException("The proj4 library version you use unfortunately defines no inverse for this projection!");
  LPTYPE odata;
  DDouble u,v;
  if ( finite( (idata.u)*(idata.v) ) ) {
    odata = LIB_PJ_INV( idata, proj );
    if (finite(odata.u) && odata.u != HUGE_VAL) {
      if(isRot) {
        u=odata.u;
        v=odata.v;
        odata.u=u*cRot-v*sRot;
        odata.v=u*sRot+v*cRot;
      }
      return odata;
    }
  }
  return badProj;
}
#else
PROJDATA protect_proj_fwd (PROJDATA idata, PROJTYPE proj){
  XYTYPE odata;
  DDouble u,v;
  if ( finite( (idata.u)*(idata.v) ) ) {
    odata = LIB_PJ_FWD( idata, proj );
    if (finite(odata.u) && odata.u != HUGE_VAL){
      if(isRot) {
        u=odata.u;
        v=odata.v;
        odata.u=u*cRot+v*sRot;
        odata.v=-u*sRot+v*cRot;
      }
      return odata;
    }
  }
  return badProj;
}
PROJDATA protect_proj_inv (PROJDATA idata, PROJTYPE proj){
  XYTYPE odata;
  DDouble u,v;
  if ( finite( (idata.u)*(idata.v) ) ) {
    odata = LIB_PJ_INV( idata, proj );
    if (finite(odata.u) && odata.u != HUGE_VAL) {
      if(isRot) {
        u=odata.u;
        v=odata.v;
        odata.u=u*cRot-v*sRot;
        odata.v=u*sRot+v*cRot;
      }
      return odata;
    }
  }
  return badProj;
}
#endif

//dummy functions for compatibility support of GCTP projections. Should define map_pipeline values. 

  void map_proj_gctp_forinit(EnvT* e) {
  };

  void map_proj_gctp_revinit(EnvT* e) {
  };
#endif

} // namespace

