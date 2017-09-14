/***************************************************************************
                          triangulation.cpp  -  GDL library function
                             -------------------
    begin                : Aug 30 2017
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

#include <list>

#include "includefirst.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"

using namespace std;

//if we ever want to use the TRIANGLE library, even if it fails on gridded data? This code is a starting point.
//#define USE_TRIANGLE 1
#define USE_TRIPACK 1

#ifdef USE_TRIANGLE

#define REAL double //as in triangle.c by default!
#include "triangle.c"

static void triangulateio_init( struct triangulateio* io )
{
    io->pointlist                  = NULL;
    io->pointattributelist         = NULL;
    io->pointmarkerlist            = NULL;
    io->numberofpoints             = 0;
    io->numberofpointattributes    = 0;
    io->trianglelist               = NULL;
    io->triangleattributelist      = NULL;
    io->trianglearealist           = NULL;
    io->neighborlist               = NULL;
    io->numberoftriangles          = 0;
    io->numberofcorners            = 0;
    io->numberoftriangleattributes = 0;
    io->segmentlist                = 0;
    io->segmentmarkerlist          = NULL;
    io->numberofsegments           = 0;
    io->holelist        = NULL;
    io->numberofholes   = 0;
    io->regionlist      = NULL;
    io->numberofregions = 0;
    io->edgelist        = NULL;
    io->edgemarkerlist  = NULL;
    io->normlist        = NULL;
    io->numberofedges   = 0;
}

static void triangulateio_destroy( struct triangulateio* io )
{
    if ( io->pointlist != NULL )
        free( io->pointlist );
    if ( io->pointattributelist != NULL )
        free( io->pointattributelist );
    if ( io->pointmarkerlist != NULL )
        free( io->pointmarkerlist );
    if ( io->trianglelist != NULL )
        free( io->trianglelist );
    if ( io->triangleattributelist != NULL )
        free( io->triangleattributelist );
    if ( io->trianglearealist != NULL )
        free( io->trianglearealist );
    if ( io->neighborlist != NULL )
        free( io->neighborlist );
    if ( io->segmentlist != NULL )
        free( io->segmentlist );
    if ( io->segmentmarkerlist != NULL )
        free( io->segmentmarkerlist );
    if ( io->holelist != NULL )
        free( io->holelist );
    if ( io->regionlist != NULL )
        free( io->regionlist );
    if ( io->edgelist != NULL )
        free( io->edgelist );
    if ( io->edgemarkerlist != NULL )
        free( io->edgemarkerlist );
    if ( io->normlist != NULL )
        free( io->normlist );
}
#endif 
#if USE_TRIPACK
//default is use tripack and stripack
#include "tripack.c"
#include "stripack.c"
#endif

namespace lib {

  using namespace std;

  void GDL_Triangulate(EnvT* e) {

    DDoubleGDL *xVal, *yVal;
    DLong npts;
    SizeT nParam = e->NParam();
    if (nParam < 2) e->Throw("Incorrect number of arguments."); //actually IDL permits to *not* have the 3rd argument.
    bool wantsTriangles=(nParam > 2);
    bool wantsEdge=(nParam == 4);
    if (wantsTriangles)  e->AssureGlobalPar(2); //since we return values in it?  
    if (wantsEdge)  e->AssureGlobalPar(3); //since we return values in it?  

//    static int tolIx=e->KeywordIx( "TOLERANCE"); //we do not use tolerance, we just insure that the first 3 points are not colinear by moving
    //the first one by a small amount, the default tolerance of IDL, fixed at 1e-12*maxval (see IDL doc).
    static int sphereIx=e->KeywordIx( "SPHERE");
    static int degreeIx=e->KeywordIx( "DEGREES");
    static int fvalueIx=e->KeywordIx( "FVALUE");
    bool isSphere=(e->KeywordPresent(sphereIx));
    bool doDegree=(e->KeywordPresent(degreeIx));
    bool reorderFvalue=(e->KeywordPresent(fvalueIx));
    if (!isSphere) {
      doDegree=false;
      reorderFvalue=false;
    }
    
    if (isSphere)  e->AssureGlobalKW(sphereIx); //since we return values in it 
    if (reorderFvalue)  e->AssureGlobalKW(fvalueIx); //since we return values in it 
    
    bool doConnectivity=false;
    static int connIx=e->KeywordIx( "CONNECTIVITY");
    if( e->KeywordPresent( connIx)) doConnectivity=true;
    
    xVal = e->GetParAs< DDoubleGDL > (0);
    if (xVal->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(0));
    npts = xVal->N_Elements();
    yVal = e->GetParAs< DDoubleGDL > (1);
    if (yVal->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(1));
    if (yVal->N_Elements() != npts) e->Throw("X & Y arrays must have same number of points.");
    if (reorderFvalue) {
      BaseGDL* f=e->GetKW(fvalueIx);
      if (f->N_Elements() != npts) e->Throw("X & Y arrays must have same number of points."); //yes yes.
    }
    if (npts<3)  e->Throw("Not enough valid and unique points specified.");
    //compute default tol
    DLong maxEl, minEl;
    DDouble maxVal;
    xVal->MinMax(&minEl,&maxEl,NULL,NULL,false);
    maxVal=(*xVal)[maxEl];
    yVal->MinMax(&minEl,&maxEl,NULL,NULL,false);
    maxVal=max(maxVal,(*yVal)[maxEl]);
    DDouble tol=1e-12*maxVal;;
// TOLERANCE KW not useful.
//    if (e->KeywordPresent(tolIx)) e->AssureDoubleScalarKW(tolIx,tol); if (tol<=0.0) tol=1e-12*maxVal;

#ifdef USE_TRIANGLE
    struct triangulateio in, out;
    triangulateio_init(&in);
    triangulateio_init(&out);
    in.numberofpoints = npts;
    in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof (REAL));
    for (SizeT i = 0, k = 0; i < npts; ++i)
    {
      in.pointlist[k++] = (*yVal)[i]; //inverted x and y to insure ccw order in the following code.
      in.pointlist[k++] = (*xVal)[i];
    }
    char tristring[32] = "ceznCQ";

    triangulate(tristring, &in, &out, NULL);
    if (out.numberoftriangles < 1) e->Throw("Triangulation failed.");

    SizeT d[2];
    d[1] = out.numberoftriangles;
    d[0] = out.numberofcorners;
    DLongGDL* returned_triangles = new DLongGDL(dimension(d, 2), BaseGDL::NOZERO);

    for (SizeT itri = 0, k = 0; itri < out.numberoftriangles; ++itri)
    {
      (*returned_triangles)[k] = out.trianglelist[k++];
      (*returned_triangles)[k] = out.trianglelist[k++];
      (*returned_triangles)[k] = out.trianglelist[k++];
    }
    //pass back to GDL env:
    e->SetPar(2, returned_triangles);
    
    if (wantsEdge) {
      //create a list of triangles having two external points
      std::list<DLong> trianglelist;
      for (SizeT jtri = 0; jtri < out.numberoftriangles; ++jtri) {
        int ngood=0;
        for (int ipos=0; ipos< 3; ++ipos) {
            DLong corner=out.trianglelist[jtri*3 +ipos];
            if (out.pointmarkerlist[corner]>0) ngood++; //good one
        }
        if (ngood > 1) trianglelist.push_back(jtri);
      }
      //start with any external point
      DLong next=-1;
      DLong curr=-1;
      for (SizeT i = 0; i < out.numberofpoints; ++i) {
        if (out.pointmarkerlist[i]>0) {
          curr=i;
          next=curr;
          break;
        }
      }
      std::list<DLong> vertexlist;
      //find sucessively in triangles the "next" of next
      do
      { //find triangle containing next and another external point.
        for (std::list<DLong>::iterator it = trianglelist.begin(); it != trianglelist.end(); it++)
        {
          bool found=false;
          int pos=-1;
          for (int ipos=0; ipos< 3; ++ipos) {
            DLong corner=out.trianglelist[(*it)*3 +ipos];
            if (corner == curr) {found=true;} //good one
            if (corner != curr && out.pointmarkerlist[corner]>0) pos=ipos;
          }
          if (found && pos>-1) {
            vertexlist.push_front(curr);
            next=out.trianglelist[(*it)*3 + pos];
            break;
          }
        }
        //remove all triangles containing the old point curr:
        for (std::list<DLong>::iterator it = trianglelist.begin(); it != trianglelist.end(); it++)
        {
          bool erase=false;
          for (int ipos=0; ipos< 3; ++ipos) {
            DLong corner=out.trianglelist[(*it)*3 +ipos];
            if (corner == curr) erase=true;
          }
          if (erase) it=trianglelist.erase(it);
        }
        curr=next;
      } while (trianglelist.size());
      vertexlist.push_front(next); //last of its kind
      //if we started on the "wrong" side, this array may still be in cw order. check and transpose eventually.
      bool cw=false;
      DDouble x[3];
      DDouble y[3];
      DDouble rot=0;
      std::list<DLong>::iterator it = vertexlist.begin();
      do {
        for (int k=0; (k<3 && it!=vertexlist.end()) ; ++k) {
          x[k]=out.pointlist[2*(*it)];
          y[k]=out.pointlist[2*(*it)+1];
          it++;
        }
        rot=(((x[2]-x[0])*(y[1]-y[0]))-((x[1]-x[0])*(y[2]-y[0])));
      } while (rot==0.0);
      if (rot<0) { //we are in the wrong order, note it
        cw=true;
      }
      
      DLongGDL* returned_edges = new DLongGDL(vertexlist.size(), BaseGDL::NOZERO);
      if (cw) {DLong k=vertexlist.size(); for (std::list<DLong>::iterator it = vertexlist.begin(); it != vertexlist.end(); it++) (*returned_edges)[--k]=(*it);}
      else {DLong k=0; for (std::list<DLong>::iterator it = vertexlist.begin(); it != vertexlist.end(); it++) (*returned_edges)[k++]=(*it);}
      e->SetPar(3, returned_edges);
      vertexlist.clear();
    }
    if (doConnectivity) {
      DLong hugearray[npts*npts];
      DLong startindex=0; //runinng index of the npts+1 values for the intervals
      DLong hugearrayindex=npts+1; //running index in hugearray: starts;
      for (SizeT ipoint = 0; ipoint < out.numberofpoints;  ++ipoint) {
        std::list<DLong> vertexlist;
        std::list<DLong> trianglelist;
        DLong nconnects=0;
        bool externalpoint = ( out.pointmarkerlist[ipoint]>0 ); //special procedure in this case

        //find all triangles containing ipoint
        for (SizeT jtri = 0; jtri < out.numberoftriangles; ++jtri)
        {
          for (int ipos=0; ipos< 3; ++ipos) if (out.trianglelist[jtri*3+ipos]==ipoint) {trianglelist.push_back(jtri); break;}
        }

// for tests
//        cerr <<trianglelist.size()<<endl;
//        for (std::list<DLong>::iterator it = trianglelist.begin(); it != trianglelist.end(); it++) {for (SizeT ncor=0; ncor< 3; ++ncor) cerr<<out.trianglelist[(*it)*3 +ncor]<<","; cerr<<endl;} 

        DLong element=ipoint;
        DLong next;
        // take first triangle, get the 2 other points, they start the ordered list of neighbours. In the case ipoint
        // is on a boundary (hull) we must start with the most external triangle, not the first
        std::list<DLong>::iterator firsttriangle=trianglelist.begin();
        DLong jtriangle=(*firsttriangle); //by default
        if ( externalpoint ) { //find triangle j where there is an other external point
          for (std::list<DLong>::iterator it = trianglelist.begin(); it != trianglelist.end(); it++) {
            bool fnd=false;
            for (SizeT ncor=0; ncor< 3; ++ncor) {
              DLong corner=out.trianglelist[(*it)*3 +ncor];
              if (corner!=ipoint && out.pointmarkerlist[corner]>0) {
                firsttriangle=it;
                jtriangle=(*firsttriangle);
                fnd=true;
                break;
              }
            }
            if (fnd) break;
          }
        }
        int centerpos;
        for (int ipos=0; ipos< 3; ++ipos) if (out.trianglelist[jtriangle*3 +ipos] == ipoint) {centerpos=ipos; break;}
        switch (centerpos)
        {
          case 0:
            element = out.trianglelist[jtriangle * 3 + 1];
            if (!externalpoint || out.pointmarkerlist[element]>0) //we should use the other point as next and write this one which is on the border.
            {
              vertexlist.push_front(element); //triangle.c writes triangles clockwise. IDL writes them ccwise.
              next = out.trianglelist[jtriangle * 3 + 2]; // next triangle to find contains [ipoint,next]. next will be added to list and the 3rd element will be next, etc.
            }
            else //external point which is not the next-in-line in triangle
            {
              vertexlist.push_front(out.trianglelist[jtriangle * 3 + 2]); //triangle.c writes triangles clockwise. IDL writes them ccwise.
              next = element; // next triangle to find contains [ipoint,next]. next will be added to list and the 3rd element will be next, etc.
            }
            break;
          case 1:
            element = out.trianglelist[jtriangle * 3 + 2];
            if (!externalpoint || out.pointmarkerlist[element]>0) 
            {
              vertexlist.push_front(element);
              next = out.trianglelist[jtriangle * 3 ];
            } 
            else
            {
              vertexlist.push_front(out.trianglelist[jtriangle * 3 ]);
              next = element;
            }
            break;
          case 2:
            element = out.trianglelist[jtriangle * 3];
            if (!externalpoint || out.pointmarkerlist[element]>0) 
            {
              vertexlist.push_front(element);
              next = out.trianglelist[jtriangle * 3 + 1];
            }
            else
            {
              vertexlist.push_front(out.trianglelist[jtriangle * 3 + 1]);
              next = element;
            }
            break;
        }
        //remove this triangle;
        trianglelist.erase(firsttriangle);  
        do
        { //find triangle containing next.

          for (std::list<DLong>::iterator it = trianglelist.begin(); it != trianglelist.end(); it++)
          {
            bool found=false;
            int pos1,pos2;
            for (int ipos=0; ipos< 3; ++ipos) {
              if (out.trianglelist[(*it)*3 +ipos] == next) {pos2=ipos; found=true;} //good one
              if (out.trianglelist[(*it)*3 +ipos] == ipoint) pos1=ipos;
            }
            if (found) {
              vertexlist.push_front(next);
              int tot=pos1+pos2;
              switch (tot) {
                case 1:  next=out.trianglelist[(*it)*3 + 2]; break;
                case 2:  next=out.trianglelist[(*it)*3 + 1]; break;
                case 3:  next=out.trianglelist[(*it)*3 + 0]; break;
              }
              trianglelist.erase(it);
              break;
            }
          }
        } while (trianglelist.size());

        //close last triangle if we are on a border, and we must:
        bool cw=false;
        if ( externalpoint ) {
          assert( out.pointlist[next] > 0 );
          vertexlist.push_front(next);
        //start with ipoint if ipoint is on the border. This is the sign in IDL that the point lies on the border.
          vertexlist.push_front(ipoint);
        //if we started on the "wrong" side, this array may still be in cw order. check and transpose eventually.
          DDouble x[3];
          DDouble y[3];
          DDouble rot=0;
          std::list<DLong>::iterator it = vertexlist.begin();
          do {
            for (int k=0; (k<3 && it!=vertexlist.end()) ; ++k) {
              x[k]=out.pointlist[2*(*it)];
              y[k]=out.pointlist[2*(*it)+1];
              it++;
            }
            rot=(((x[2]-x[0])*(y[1]-y[0]))-((x[1]-x[0])*(y[2]-y[0])));
          } while (rot==0.0);
          if (rot<0) { //we are in the wrong order, note it
            cw=true;
          }
//          cerr<<"point "<<ipoint<<" rot= "<<rot<<" cw="<<cw<<endl;
        }


// for tests
//        cerr<<"point "<<ipoint<<": "; for (std::list<DLong>::iterator it = vertexlist.begin(); it != vertexlist.end(); it++) cerr<<(*it)<<","; cerr<<endl;
                
        //write list values in hugearray
        nconnects=vertexlist.size();
        DLong currentnodesstartindex=hugearrayindex;
        hugearray[startindex++]=hugearrayindex;
        for (std::list<DLong>::iterator it = vertexlist.begin(); it != vertexlist.end(); it++) hugearray[hugearrayindex++]=*it;
        hugearray[startindex]=hugearrayindex;
        //if cw, rewrite it in reverse order (simpler than changing lines above. First element should not be changed (it is ipoint) according to IDL's unwritten rules:
        if (cw) {
          DLong reverse_index=hugearrayindex;
          std::list<DLong>::iterator it = vertexlist.begin(); it++;
          for (; it != vertexlist.end(); it++) hugearray[--reverse_index]=(*it);
        }
        vertexlist.clear();
        trianglelist.clear();
      }
      DLongGDL* connections = new DLongGDL(hugearrayindex, BaseGDL::NOZERO);
      for (SizeT i = 0; i < hugearrayindex; ++i) (*connections)[i]=hugearray[i];
      e->SetKW(connIx,connections);
    }

    triangulateio_destroy(&out);
    triangulateio_destroy(&in);
#endif

#ifdef USE_TRIPACK
    //Insure first triangle is not colinear. If it is, move first point infinitesimally. Revert to initial value at end of triangulation...
    bool revertToInitialPoint=false;
    DDouble x[3];
    DDouble y[3];
    DDouble rot=0;
    SizeT k=0;
    for (k=0; k<3 ; ++k) {x[k]=(*xVal)[k];y[k]=(*yVal)[k];} 
    rot=(((x[2]-x[0])*(y[1]-y[0]))-((x[1]-x[0])*(y[2]-y[0])));
    if (abs(rot)<tol) {
      revertToInitialPoint=true;
      (*xVal)[0]+=tol;
      (*yVal)[0]-=tol;
    }
    
    //TODO remove duplicate points and store their indices in repeats if optioN is present.

    if (isSphere) {
      DDoubleGDL *sc_xVal=xVal;
      DDoubleGDL *sc_yVal=yVal;
      Guard<BaseGDL>xguard;
      Guard<BaseGDL>yguard;
      if (doDegree) {
        static DDouble DToR=double(3.1415926535897932384626433832795)/180.0;
        sc_xVal=new DDoubleGDL(npts,BaseGDL::NOZERO);
        for (SizeT i=0; i<npts; ++i) (*sc_xVal)[i]=(*xVal)[i]*DToR;
        xguard.reset(sc_xVal);
        sc_yVal=new DDoubleGDL(npts,BaseGDL::NOZERO);
        for (SizeT i=0; i<npts; ++i) (*sc_yVal)[i]=(*yVal)[i]*DToR;
        yguard.reset(sc_yVal);
      }
      DDoubleGDL* x=new DDoubleGDL(npts,BaseGDL::NOZERO);
      DDoubleGDL* y=new DDoubleGDL(npts,BaseGDL::NOZERO);
      DDoubleGDL* z=new DDoubleGDL(npts,BaseGDL::NOZERO);
      DLong ret0=stripack::trans_(&npts, (DDouble*)sc_xVal->DataAddr(), (DDouble*)sc_yVal->DataAddr(),(DDouble*)x->DataAddr(),(DDouble*)y->DataAddr(),(DDouble*)z->DataAddr()); 
      SizeT listsize=6*npts-12;
      DLongGDL* list=new DLongGDL(listsize,BaseGDL::NOZERO);
      DLongGDL* lptr=new DLongGDL(listsize,BaseGDL::NOZERO);
      DLongGDL* lend=new DLongGDL(npts,BaseGDL::NOZERO);
      DLong* near=(DLong*)malloc(npts*sizeof(DLong));
      DLong* next=(DLong*)malloc(npts*sizeof(DLong));
      DDouble* dist=(DDouble*)malloc(npts*sizeof(DDouble));
      DLong ier=0;
      DLong lnew=0;
      DLong ret1=stripack::sph_trmesh_(&npts,(DDouble*)x->DataAddr(),(DDouble*)y->DataAddr(),(DDouble*)z->DataAddr(), (DLong*)list->DataAddr(), (DLong*)lptr->DataAddr(), (DLong*)lend->DataAddr(), &lnew, near, next, dist, &ier);
      free(dist);
      free(next);
      free(near);
      if (ier !=0) {
        GDLDelete(list);
        GDLDelete(lptr);
        GDLDelete(lend);
        GDLDelete(x);
        GDLDelete(y);
        GDLDelete(z);
        if (ier == -2) e->Throw("The first 3 nodes lie on the same great circle.");
        else e->Throw("Duplicate nodes encountered.");
      }
      //Structure returned in SPHERE is not the same as IDL (although it must be possible to write
      //something identical, but for what use? The structure will be passed to the TRIGRID function
      //that uses SRFPACK, which in turn uses this structure, containing x,y,z,list,lptr,lend
        // creating the output anonymous structure
        DStructDesc* sphere_desc = new DStructDesc("$truct");
        SpDDouble aDbleArr(dimension((SizeT)npts));
        SpDLong aLongArr1(dimension((SizeT)listsize));
        SpDLong aLongArr2(dimension((SizeT)npts));
        sphere_desc->AddTag("X", &aDbleArr);
        sphere_desc->AddTag("Y", &aDbleArr);
        sphere_desc->AddTag("Z", &aDbleArr);
        sphere_desc->AddTag("LIST", &aLongArr1);
        sphere_desc->AddTag("LPTR", &aLongArr1);
        sphere_desc->AddTag("LEND", &aLongArr2);
        DStructGDL* sphere = new DStructGDL(sphere_desc, dimension());

      sphere->InitTag("X", *x); 
      sphere->InitTag("Y", *y); 
      sphere->InitTag("Z", *z); 
      sphere->InitTag("LIST", *list); 
      sphere->InitTag("LPTR", *lptr); 
      sphere->InitTag("LEND", *lend); 
      e->SetKW(sphereIx,sphere);
    
      if (wantsTriangles) {
        //convert to triangle list
        DLong nrow = 6; //no arcs.
        DLong* ltri = (DLong*) malloc((12 * npts) * sizeof (DLong));
        DLong ntriangles = 0;
        DLong ret2=stripack::sph_trlist_(&npts, (DLong*)list->DataAddr(), (DLong*)lptr->DataAddr(), (DLong*)lend->DataAddr(), &nrow, &ntriangles, ltri, &ier);
        if (ier != 0)
        {
          free(ltri);
          e->Throw("Unexpected Error in STRIPACK, SPH_TRLIST routine. Please report.");
        }
        SizeT d[2];
        d[1] = ntriangles;
        d[0] = 3;
        DLongGDL* returned_triangles = new DLongGDL(dimension(d, 2), BaseGDL::NOZERO);
        for (SizeT j = 0; j < ntriangles; ++j)
        {
          for (int i = 0; i < 3; ++i)
          {
            (*returned_triangles)[3 * j + i] = (ltri[6 * j + i] - 1); //nubering starts a 0.
          }
        }
        free(ltri);
        //pass back to GDL env:
        e->SetPar(2, returned_triangles);
      }      
      if (wantsEdge) {
        DLong* nodes=(DLong*)malloc(npts*sizeof(DLong));
        DLong nb=0;
        DLong na=0;
        DLong nt=0;
        DLong ret3=stripack::sph_bnodes_(&npts, (DLong*)list->DataAddr(), (DLong*)lptr->DataAddr(), (DLong*)lend->DataAddr(), nodes, &nb, &na, &nt);
        if (nb>0){
          DLongGDL* returned_edges = new DLongGDL(nb, BaseGDL::NOZERO);
          for (SizeT j = 0; j < nb; ++j) (*returned_edges)[j]=nodes[j]-1;
          e->SetPar(3, returned_edges);
        } else e->SetPar(3, new DLongGDL(-1));
        free(nodes);
      }
      if (reorderFvalue) {
// do nothing until TRIGRID has been rewritten!        
//        //get fvalue
//        DDoubleGDL* f=e->GetKWAs<DDoubleGDL>(fvalueIx);
//        //create a dummy array of same size
//        DDoubleGDL* ret=new DDoubleGDL(npts,BaseGDL::NOZERO);
//        //do not forget remove 1 to get C array indexes.
//        e->SetKW(fvalueIx,ret);
      }
       //no connectivity in SPHERE mode
      //no more cleanup, x,y,z,and list,lptr,lend are in the returned structure!
    } else {
      SizeT listsize=6*npts-12;
      DLong* list=(DLong*)malloc(listsize*sizeof(DLong));
      DLong* lptr=(DLong*)malloc(listsize*sizeof(DLong));
      DLong* lend=(DLong*)malloc(npts*sizeof(DLong));
      DLong* near=(DLong*)malloc(npts*sizeof(DLong));
      DLong* next=(DLong*)malloc(npts*sizeof(DLong));
      DDouble* dist=(DDouble*)malloc(npts*sizeof(DDouble));
      DLong ier=0;
      DLong lnew=0;

      DLong ret1=tripack::trmesh_(&npts, (DDouble*)xVal->DataAddr() , (DDouble*)yVal->DataAddr(), list, lptr, lend, &lnew, near, next, dist, &ier);
      free(near);
      free(next);
      free(dist);
      if (ier !=0) {
        free(list);
        free(lptr);
        free(lend);
        if (ier == -2) e->Throw("The first 3 nodes are collinear.");
        else e->Throw("Duplicate nodes encountered.");
      }
      if (wantsTriangles) {
        //convert to triangle list
        DLong nocontraints = 0;
        DLong dummylistofcontraints = 0;
        DLong nrow = 6; //no arcs.
        DLong* ltri = (DLong*) malloc((12 * npts) * sizeof (DLong));
        DLong lct = 0;
        DLong ntriangles = 0;
        DLong ret2 = tripack::trlist_(&nocontraints, &dummylistofcontraints, &npts, list, lptr, lend, &nrow, &ntriangles, ltri, &lct, &ier);
        if (ier != 0)
        {
          free(ltri);
          free(list);
          free(lptr);
          free(lend);
          e->Throw("Unexpected Error in TRIPACK, TRLIST routine. Please report.");
        }
        SizeT d[2];
        d[1] = ntriangles;
        d[0] = 3;
        DLongGDL* returned_triangles = new DLongGDL(dimension(d, 2), BaseGDL::NOZERO);
        for (SizeT j = 0; j < ntriangles; ++j)
        {
          for (int i = 0; i < 3; ++i)
          {
            (*returned_triangles)[3 * j + i] = (ltri[6 * j + i] - 1); //nubering starts a 0.
          }
        }
        free(ltri);
        //pass back to GDL env:
        e->SetPar(2, returned_triangles);
      }
      if (wantsEdge) {
        DLong* nodes=(DLong*)malloc(npts*sizeof(DLong));
        DLong nb=0;
        DLong na=0;
        DLong nt=0;
        DLong ret3=tripack::bnodes_(&npts, list, lptr, lend, nodes, &nb, &na, &nt);
        DLongGDL* returned_edges = new DLongGDL(nb, BaseGDL::NOZERO);
        for (SizeT j = 0; j < nb; ++j) (*returned_edges)[j]=nodes[j]-1;
        free(nodes);
        e->SetPar(3, returned_edges);
      }
      if (doConnectivity) {
        //remove 1 to get C array indexes.
        for (SizeT i = 0; i < lnew-1; ++i) lptr[i]--; 
        for (SizeT i = 0; i < npts; ++i) lend[i]--;
        DLong array[2*(lnew-1)];
        SizeT runningindex=npts+1;
        SizeT startindex=0;
        array[startindex++]=npts+1;
        for (SizeT i = 0; i < npts; ++i) {
          //is it an exterior point? Yes if the termination of the connectivity list is exterior.
          DLong lpl=lend[i];
          if (list[lpl]<0) array[runningindex++]=i; //exterior - we write it
          //write all points until nfin=lend[i] is found again using lptr connectivity pointers:
          DLong lp=lpl; //see nbcnt_()
          do {
            lp=lptr[lp];
            array[runningindex++]=(list[lp]>0)?list[lp]-1:(-list[lp])-1;
          } while (lp!=lpl);
          array[startindex++]=runningindex;
        }
        DLongGDL* connections = new DLongGDL(runningindex, BaseGDL::NOZERO);
        for (SizeT i = 0; i < runningindex; ++i) (*connections)[i]=array[i];
        e->SetKW(connIx,connections);
      }
      //cleanup
      free(list);
      free(lptr);
      free(lend);
    }
    if (revertToInitialPoint) {
      (*xVal)[0]-=tol;
      (*yVal)[0]+=tol;
    }
#endif
  }

  BaseGDL* trigrid_fun(EnvT* e) {
    //   Compute plane parameters A,B,C given 3 points on plane.
    //
    //   z = A + Bx + Cy
    //
    //       (^x21^z10 - ^x10^z21)
    //   C = --------------------- 
    //       (^x21^y10 - ^x10^y21)
    //
    //       (^z10 - C*^y10)
    //   B = ---------------
    //            ^x10
    //
    //   A = z - Bx -Cy
    //
    //   where ^x21 = x2 - x1, etc.

    SizeT nParam = e->NParam();
    if (nParam < 4)
      e->Throw("Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined(0);
    BaseGDL* p1 = e->GetParDefined(1);
    BaseGDL* p2 = e->GetParDefined(2);
    BaseGDL* p3 = e->GetParDefined(3);

    if (p0->N_Elements() != p1->N_Elements() ||
      p0->N_Elements() != p2->N_Elements() ||
      p1->N_Elements() != p2->N_Elements())
      e->Throw("X, Y, or Z array dimensions are incompatible.");

    if (p3->Rank() == 0)
      e->Throw("Expression must be an array "
      "in this context: " + e->GetParString(0));
    if (p3->N_Elements() % 3 != 0)
      e->Throw("Array of triangles incorrectly dimensioned.");
    DLong n_tri = p3->N_Elements() / 3;

    if (p0->Rank() == 0)
      e->Throw("Expression must be an array "
      "in this context: " + e->GetParString(0));
    if (p0->N_Elements() < 3)
      e->Throw("Value of Bounds is out of allowed range.");

    if (p1->Rank() == 0)
      e->Throw("Expression must be an array "
      "in this context: " + e->GetParString(1));

    if (p2->Rank() == 0)
      e->Throw("Expression must be an array "
      "in this context: " + e->GetParString(2));

    if (p2->Rank() < 1 || p2->Rank() > 2)
      e->Throw("Array must have 1 or 2 dimensions: "
      + e->GetParString(0));

    DDoubleGDL* GS = NULL;
    DDoubleGDL* limits = NULL;
    if (nParam > 4)
    {
      BaseGDL* p4 = e->GetParDefined(4);
      if (p4->Rank() == 0)
        e->Throw("Expression must be an array "
        "in this context: " + e->GetParString(4));
      if (p4->N_Elements() != 2)
        e->Throw("Array must have 2 elements: "
        + e->GetParString(4));
      GS = static_cast<DDoubleGDL*>
        (p4->Convert2(GDL_DOUBLE, BaseGDL::COPY));

      if (nParam == 6)
      {
        BaseGDL* p5 = e->GetParDefined(5);
        if (p5->Rank() == 0)
          e->Throw("Expression must be an array "
          "in this context: " + e->GetParString(4));
        if (p5->N_Elements() != 4)
          e->Throw("Array must have 4 elements: "
          + e->GetParString(5));
        limits = static_cast<DDoubleGDL*>
          (p5->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      }
    }

    DLong n_segx = 50;
    DLong n_segy = 50;

    DDouble diff[3][2];
    DDouble edge[3][3];

    DDouble diff_pl[3][3];
    DDouble edge_pl[3][3];
    DDouble uv_tri[3][3];
    DDouble uv_gridpt[3];

    DDouble delx10;
    DDouble delx21;
    DDouble dely10;
    DDouble dely21;
    DDouble delz10;
    DDouble delz21;

    DDouble A;
    DDouble B;
    DDouble C;

    bool vertx = false;
    bool verty = false;

    DDouble lon;
    DDouble lat;

    DLong minxEl;
    DLong maxxEl;
    DLong minyEl;
    DLong maxyEl;

    // Get NX, NY values if present
    static int nxix = e->KeywordIx("NX");
    if (e->KeywordSet(nxix))
    {
      e->AssureLongScalarKW(nxix, n_segx);
    }
    static int nyix = e->KeywordIx("NY");
    if (e->KeywordSet(nyix))
    {
      e->AssureLongScalarKW(nyix, n_segy);
    }

    DDoubleGDL* x_tri = static_cast<DDoubleGDL*>
      (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
    DDoubleGDL* y_tri = static_cast<DDoubleGDL*>
      (p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));
    DDoubleGDL* z = static_cast<DDoubleGDL*>
      (p2->Convert2(GDL_DOUBLE, BaseGDL::COPY));
    DLongGDL* triangles = static_cast<DLongGDL*>
      (p3->Convert2(GDL_LONG, BaseGDL::COPY));


    //    bool sphere=false;
    //int sphereix = e->KeywordIx( "SPHERE");
    //if(e->KeywordSet(sphereix)) sphere = true;

    DDouble xvsx[2];
    DDouble yvsy[2];
    bool map = false;

//#ifdef USE_LIBPROJ4
//    // Map Stuff (xtype = 3)
//
//    // Stuff needed for MAP keyword processing
//    static int mapix = e->KeywordIx("MAP"); //trigrid_fun is standalone.
//    BaseGDL* Map = e->GetKW(mapix);
//    if (Map != NULL)
//    {
//      if (Map->N_Elements() != 4)
//        e->Throw("Keyword array parameter MAP"
//        "must have 4 elements.");
//      map = true;
//      Guard<DDoubleGDL> guard;
//      DDoubleGDL* mapD = static_cast<DDoubleGDL*>
//        (Map->Convert2(GDL_DOUBLE, BaseGDL::COPY));
//      guard.Reset(mapD);
//      xvsx[0] = (*mapD)[0];
//      xvsx[1] = (*mapD)[1];
//      yvsy[0] = (*mapD)[2];
//      yvsy[1] = (*mapD)[3];
//    }
//
//    LPTYPE idata;
//    XYTYPE odata;
//
//    if (map)
//    {
//      ref = map_init();
//      if (ref == NULL)
//      {
//        e->Throw("Projection initialization failed.");
//      }
//
//      // Convert lon/lat to x/y device coord
//      for (SizeT i = 0; i < x_tri->N_Elements(); ++i)
//      {
//        idata.u = (*x_tri)[i] * DEG_TO_RAD;
//        idata.v = (*y_tri)[i] * DEG_TO_RAD;
//        odata = PJ_FWD(idata, ref);
//        (*x_tri)[i] = odata.u * xvsx[1] + xvsx[0];
//        (*y_tri)[i] = odata.v * yvsy[1] + yvsy[0];
//      }
//    }
//#endif

    // Determine grid range
    x_tri->MinMax(&minxEl, &maxxEl, NULL, NULL, true);
    y_tri->MinMax(&minyEl, &maxyEl, NULL, NULL, true);

    DDouble x0;
    DDouble y0;
    DDouble x_span = (*x_tri)[maxxEl] - (*x_tri)[minxEl];
    DDouble y_span = (*y_tri)[maxyEl] - (*y_tri)[minyEl];
    if (limits != NULL)
    {
      x_span = (*limits)[2] - (*limits)[0];
      y_span = (*limits)[3] - (*limits)[1];
      x0 = (*limits)[0];
      y0 = (*limits)[1];
    } else
    {
      x0 = (*x_tri)[minxEl];
      y0 = (*y_tri)[minyEl];
    }

    // Determine grid spacing
    DDouble x_spacing = x_span / n_segx;
    DDouble y_spacing = y_span / n_segy;
    if (GS != NULL && !e->KeywordSet(nxix))
    {
      x_spacing = (*GS)[0];
      n_segx = (DLong) (x_span / x_spacing);
    }
    if (GS != NULL && !e->KeywordSet(nyix))
    {
      y_spacing = (*GS)[1];
      n_segy = (DLong) (y_span / y_spacing);
    }
    if (e->KeywordSet(nxix) && n_segx == 1) n_segx = 0;
    if (e->KeywordSet(nyix) && n_segy == 1) n_segy = 0;

    // Setup return array
    DLong dims[2];
    dims[0] = n_segx + 1;
    dims[1] = n_segy + 1;
    dimension dim((DLong *) dims, 2);
    DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::ZERO);

    bool *found = new bool [(n_segx + 1)*(n_segy + 1)];
    for (SizeT i = 0; i < (n_segx + 1)*(n_segy + 1); ++i) found[i] = false;

    // *** LOOP THROUGH TRIANGLES *** //

    // Loop through all triangles
    for (SizeT i = 0; i < n_tri; ++i)
    {

      DLong tri0 = (*triangles)[3 * i + 0];
      DLong tri1 = (*triangles)[3 * i + 1];
      DLong tri2 = (*triangles)[3 * i + 2];

      /*
      // Convert lon/lat to x/y device coord
      if ( map) {
    idata.lam = (*x_tri)[tri0] * DEG_TO_RAD;
    idata.phi = (*y_tri)[tri0] * DEG_TO_RAD;
    odata = pj_fwd(idata, ref);
    (*x_tri)[tri0] = odata.x *  xvsx[1] + xvsx[0];
    (*y_tri)[tri0] = odata.y *  yvsy[1] + yvsy[0];

    idata.lam = (*x_tri)[tri1] * DEG_TO_RAD;
    idata.phi = (*y_tri)[tri1] * DEG_TO_RAD;
    odata = pj_fwd(idata, ref);
    (*x_tri)[tri1] = odata.x *  xvsx[1] + xvsx[0];
    (*y_tri)[tri1] = odata.y *  yvsy[1] + yvsy[0];

    idata.lam = (*x_tri)[tri2] * DEG_TO_RAD;
    idata.phi = (*y_tri)[tri2] * DEG_TO_RAD;
    odata = pj_fwd(idata, ref);
    (*x_tri)[tri2] = odata.x *  xvsx[1] + xvsx[0];
    (*y_tri)[tri2] = odata.y *  yvsy[1] + yvsy[0];
      }
       */
      // *** PLANE INTERPOLATION *** //

      delx10 = (*x_tri)[tri1] - (*x_tri)[tri0];
      delx21 = (*x_tri)[tri2] - (*x_tri)[tri1];

      dely10 = (*y_tri)[tri1] - (*y_tri)[tri0];
      dely21 = (*y_tri)[tri2] - (*y_tri)[tri1];

      delz10 = (*z)[tri1] - (*z)[tri0];
      delz21 = (*z)[tri2] - (*z)[tri1];

      // Compute grid array
      for (SizeT j = 0; j < 3; ++j)
      {
        DLong itri = (*triangles)[3 * i + j];
        DLong ktri = (*triangles)[3 * i + ((j + 1) % 3)];
        edge[j][0] = (*x_tri)[ktri] - (*x_tri)[itri];
        edge[j][1] = (*y_tri)[ktri] - (*y_tri)[itri];
      }

      C = (delx21 * delz10 - delx10 * delz21) /
        (delx21 * dely10 - delx10 * dely21);
      B = (delz10 - C * dely10) / delx10;
      A = (*z)[tri0] - B * (*x_tri)[tri0] - C * (*y_tri)[tri0];


      // *** LOOP THROUGH GRID POINTS *** //

      // Loop through all y-grid values
      for (SizeT iy = 0; iy < n_segy + 1; ++iy)
      {
        DDouble devy = y0 + iy * y_spacing;

        // Loop through all x-grid values
        for (SizeT ix = 0; ix < n_segx + 1; ++ix)
        {

          if (found[iy * (n_segx + 1) + ix]) continue;

          bool inside = true;
          DDouble devx = x0 + ix * x_spacing;

          // *** PLANE INTERPOLATION *** //

          // Compute diff array for xy-values
          for (SizeT j = 0; j < 3; ++j)
          {
            DLong itri = (*triangles)[3 * i + j];
            diff[j][0] = (*x_tri)[itri] - devx;
            diff[j][1] = (*y_tri)[itri] - devy;
          }


          // Determine if inside triangle
          for (SizeT ivert = 0; ivert < 3; ++ivert)
          {
            DLong kvert = (ivert + 1) % 3;

            DDouble crs1;
            DDouble crs2;
            crs1 = diff[ivert][0] * edge[ivert][1] - diff[ivert][1] * edge[ivert][0];
            crs2 = edge[kvert][0] * edge[ivert][1] - edge[kvert][1] * edge[ivert][0];
            if (crs1 * crs2 > 0)
            {
              inside = false;
              break;
            }
          }

          if (inside == true)
          {
            found[iy * (n_segx + 1) + ix] = true;
            (*res)[iy * (n_segx + 1) + ix] = A + B * devx + C*devy;
            if (map)
            {
              //	      cout << setiosflags(ios::fixed);
              //cout << setw(2);
              //cout << setprecision(2);
              //cout << left << "lon: "   << setw(10) << right << lon;
              //cout << left << "  lat: " << setw(10) << right << lat; 
              //cout << " in triangle: " << i << endl;
            }
          }

        } // ix loop
      } // iy loop
    } // i (triangle) loop

    for (SizeT i = 0; i < (n_segx + 1)*(n_segy + 1); ++i)
    {
      if (found[i] == false && map)
      {
        //	cout << i/(n_segy+1) << " ";
        //cout << i - (i/(n_segy+1))*(n_segy+1);
        //cout << " NOT in triangles." << endl;
        //(*res)[i] = 0;
      }
    }

    delete[] found;
    return res;
  }

  void grid_input(EnvT* e) {
    e->Throw("Writing in progress.");
  }

// see http://www.geom.umn.edu/software/qhull/. Used also with plplot.
#ifdef PL_HAVE_QHULL
  void qhull ( EnvT* e)
  {
    e->Throw("Please Write this function in GDL.");
  }


  BaseGDL* qgrid3_fun ( EnvT* e)
  {
    e->Throw("Please Write this function in GDL.");
  }
#endif  
  }

