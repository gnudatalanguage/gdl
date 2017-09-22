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
#include "ssrfpack.c"
#endif

namespace lib {

  using namespace std;

  void GDL_Triangulate(EnvT* e) {

    DDoubleGDL *xVal, *yVal, *fvalue;
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
      fvalue=e->GetKWAs<DDoubleGDL>(fvalueIx);
      if (fvalue->Rank() == 0) e->Throw("Expression must be an array in this context: " +  e->GetString( fvalueIx)+".");
      if (fvalue->N_Elements() != npts) e->Throw("X & Y arrays must have same number of points."); //yes yes.
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
      //Stripack is lat, lon and IDL lon,lat
      DLong ret0=stripack::trans_(&npts, (DDouble*)sc_yVal->DataAddr(), (DDouble*)sc_xVal->DataAddr(),(DDouble*)x->DataAddr(),(DDouble*)y->DataAddr(),(DDouble*)z->DataAddr()); 
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
        //No need to reorder whatever.
//        //create a dummy array of same size
//        DDoubleGDL* ret=new DDoubleGDL(npts,BaseGDL::NOZERO);
//        //do not forget remove 1 to get C array indexes
//        DLong index_lend, index_list; //can be negative.
//        (*ret)[0]=(*fvalue)[(*list)[0]-1];
//        for (SizeT i=0; i<npts-1; ++i) {
//          index_lend=(*lend)[i]; //as it starts at 1, is the next point, not the end of current point neighbours.
//          index_list=(*list)[index_lend]; //can be negative
//          if (index_list < 0) index_list=-index_list-1; else index_list--; //remove negative values and set as C index.
//          (*ret)[i+1]=(*fvalue)[index_list]; //next point
//        }
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

  
  BaseGDL* trigrid_fun_spherical(EnvT* e) {
    //NOT USED in this case: EXTRAPOLATE, MAX_VALUE, MIN_VALUE, QUINTIC, XOUT, YOUT
    static DDouble DToR=double(3.1415926535897932384626433832795)/180.0;
    SizeT nParam = e->NParam();
    if (nParam < 3) e->Throw("Incorrect number of arguments.");
    //OK, trigrid does not care if more than 3 args in sphere mode. Limit at 6 is done at interpreter level.

    // Get NX, NY values if present
    DLong nx = 51;
    DLong ny = 51;

    static int degreeIx=e->KeywordIx( "DEGREES");
    bool doDegree=(e->KeywordPresent(degreeIx));

    // in difference with IDL we accept xout and yout for spherical data.
    static int xoutIx=e->KeywordIx( "XOUT");
    bool doXout=(e->KeywordSet(xoutIx)); //and not "present"
    static int youtIx=e->KeywordIx( "YOUT");
    bool doYout=(e->KeywordSet(youtIx));
    if ((doXout && !doYout) || (doYout && !doXout)) e->Throw("Incorrect number of arguments.");
    static int xgridIx=e->KeywordIx( "XGRID");
    bool doXgrid=(e->KeywordPresent(xgridIx));
    static int ygridIx=e->KeywordIx( "YGRID");
    bool doYgrid=(e->KeywordPresent(ygridIx));

    static int nxIx = e->KeywordIx("NX");
    static int nyIx = e->KeywordIx("NY");
    bool canUseLimitsx=true;
    bool canUseLimitsy=true;
    if (e->KeywordSet(nxIx)) { canUseLimitsx=false; e->AssureLongScalarKW(nxIx, nx);}
    if (e->KeywordSet(nyIx)) { canUseLimitsy=false; e->AssureLongScalarKW(nyIx, ny);}
    if (nx < 1 || ny < 1) e->Throw("Array dimensions must be greater than 0.");
    // we will further on define GS, whose value may overwrite the nx, ny.
    
    BaseGDL* p0 = e->GetParDefined(0); //F
    if (p0->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(0));
    DDoubleGDL* fval=static_cast<DDoubleGDL*>(p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
    BaseGDL* p1 = e->GetParDefined(1);
    if (p1->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(1));
    if (p1->N_Elements() != 2) e->Throw("Array must have 2 elements: " + e->GetParString(1));
    DDoubleGDL* GS = static_cast<DDoubleGDL*>(p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));

    BaseGDL* p2 = e->GetParDefined(2);
    if (p2->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(2));
    if (p2->N_Elements() != 4) e->Throw("Array must have 4 elements: " + e->GetParString(2));
    DDoubleGDL* limits = static_cast<DDoubleGDL*> (p2->Convert2(GDL_DOUBLE, BaseGDL::COPY));      

    //Sphere is present since we are called. Test it.
    DStructGDL* sphere=NULL;
    static int sphereIx = e->KeywordIx( "SPHERE");
    BaseGDL* test=e->GetKW(sphereIx);
    
    int xTag,yTag,zTag,listTag,lptrTag,lendTag;
    
    if (test->Type() == GDL_STRUCT)
    {
      sphere = static_cast<DStructGDL*> (test);

      xTag = sphere->Desc()->TagIndex("X");
      yTag = sphere->Desc()->TagIndex("Y");
      zTag = sphere->Desc()->TagIndex("Z");
      listTag = sphere->Desc()->TagIndex("LIST");
      lptrTag = sphere->Desc()->TagIndex("LPTR");
      lendTag = sphere->Desc()->TagIndex("LEND");
      if (xTag < 0 || yTag < 0 || zTag < 0 || listTag < 0 ||lptrTag < 0 || lendTag < 0) e->Throw("Invalid structure for the SPHERE keyword.");
    } else {
      e->Throw("SPHERE keyword must be a structure.");
    }

    DDoubleGDL* xVal = static_cast<DDoubleGDL*>(sphere->GetTag(xTag,0));
    DDoubleGDL* yVal = static_cast<DDoubleGDL*>(sphere->GetTag(yTag,0));
    DDoubleGDL* zVal = static_cast<DDoubleGDL*>(sphere->GetTag(zTag,0));
    if (xVal->N_Elements() != yVal->N_Elements())e->Throw("Invalid structure for the SPHERE keyword.");
    if (xVal->N_Elements() != zVal->N_Elements())e->Throw("Invalid structure for the SPHERE keyword.");
    DLongGDL* list = static_cast<DLongGDL*>(sphere->GetTag(listTag,0));
    DLongGDL* lptr = static_cast<DLongGDL*>(sphere->GetTag(lptrTag,0));
    DLongGDL* lend = static_cast<DLongGDL*>(sphere->GetTag(lendTag,0));
    
    //get npts
    DLong npts=fval->N_Elements();
    if (xVal->N_Elements()<npts) npts=xVal->N_Elements();
    
//    //get min max X Y.
//    DLong minxEl,maxxEl,minyEl,maxyEl;
//    xVal->MinMax(&minxEl, &maxxEl, NULL, NULL, true);
//    yVal->MinMax(&minyEl, &maxyEl, NULL, NULL, true);
    // Determine grid range
    DDouble xref=0.0, yref=0.0;
    DDouble xval, xinc;
    DDouble yval, yinc;
    DDouble xrange; // = (*xVal)[maxxEl] - (*xVal)[minxEl];
    DDouble yrange; // = (*yVal)[maxyEl] - (*yVal)[minyEl];
    //compute World positions of each pixels.
    if ((*limits)[0]==(*limits)[2] || (*limits)[0]==(*limits)[2]) e->Throw("Inconsistent coordinate bounds.");
    xval = (*limits)[0];
    if (doDegree) {
      xval*=DToR;
      xrange = (*limits)[2]*DToR - xval;
    } else xrange = (*limits)[2] - xval;
    yval = (*limits)[1];
    if (doDegree) {
      yval*=DToR;
      yrange = (*limits)[3]*DToR - yval;
    } else yrange = (*limits)[3] - yval;
    // Determine grid spacing
    xinc = xrange / (nx-1);
    yinc = yrange / (ny-1);
    if (canUseLimitsx && (*GS)[0] > 0.0)
    {
      xinc = (*GS)[0];
      if (doDegree) xinc*=DToR;
      nx = (xrange / xinc) +1;
    }
    if (canUseLimitsy && (*GS)[1] > 0.0)
    {
      yinc = (*GS)[1];
      if (doDegree) yinc*=DToR;
      ny = (yrange / yinc) +1;
    }
    DDouble *plon, *plat;
    bool cleanplon=false;
    bool cleanplat=false;

    if (doXout) {
      DDoubleGDL* xOut=e->GetKWAs<DDoubleGDL>(xoutIx);
      nx=xOut->N_Elements();
      if (nx < 1) e->Throw("XOUT must have >1 elements.");
      plon=(DDouble*)xOut->DataAddr();
      if (doDegree) for (int i=0; i<nx; ++i) plon[i]*=DToR;
    } else {
      plon = (DDouble*)malloc(nx*sizeof(DDouble));
      for (int i=0; i<nx; ++i) plon[i]=(i-xref)*xinc+xval;
      cleanplon=true;
    }

    if (doYout) {
      DDoubleGDL* yOut=e->GetKWAs<DDoubleGDL>(youtIx);
      ny=yOut->N_Elements();
      if (ny < 1) e->Throw("YOUT must have >1 elements.");
      plat=(DDouble*)yOut->DataAddr();
      if (doDegree) for (int j=0; j<ny; ++j) plat[j]*=DToR;
    } else {
      plat = (DDouble*)malloc(ny*sizeof(DDouble));
      for (int j=0; j<ny; ++j) plat[j]=(j-yref)*yinc+yval;
      cleanplat=true;
    }

    // Setup return array
    DLong dims[2];
    dims[0] = ny;
    dims[1] = nx;
    dimension dim((DLong *) dims, 2);
    DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::ZERO);
    DLong ier=0;

    //must be done here, unif_() below messes with plat and plon...
    if (doXgrid) {
      DDoubleGDL *xgrid=new DDoubleGDL(nx, BaseGDL::NOZERO);
      for (int i=0; i<nx; ++i) (*xgrid)[i]=plon[i];
      if (doDegree) for (int i=0; i<nx; ++i) (*xgrid)[i]/=DToR;
      e->SetKW(xgridIx, xgrid);
    }
    if (doYgrid) {
      DDoubleGDL *ygrid=new DDoubleGDL(ny, BaseGDL::NOZERO);
      for (int i=0; i<ny; ++i) (*ygrid)[i]=plat[i];
      if (doDegree) for (int i=0; i<ny; ++i) (*ygrid)[i]/=DToR;
      e->SetKW(ygridIx, ygrid);
    }
    
    DLong iflgs=0;
    DLong iflgg=0;
    DDouble grad=0.0;
    DDouble sigma=1.;
    DLong ret2=ssrfpack::unif_ (&npts,(DDouble*)xVal->DataAddr(), (DDouble*)yVal->DataAddr(),
              (DDouble*)zVal->DataAddr(), (DDouble*)fval->DataAddr(),(DLong*)list->DataAddr(), (DLong*)lptr->DataAddr(),
              (DLong*)lend->DataAddr(), &iflgs, &sigma, &ny, &ny, &nx, plat, plon, &iflgg, 
	&grad, (DDouble*)res->DataAddr() , &ier);
    if (cleanplon) free(plon);
    if (cleanplat) free(plat);
    if (ret2 != 0) {
      if (ret2==-1) e->Throw("Error in SSRFPACK unif_() function: \"N, NI, NJ, or IFLGG is outside its valid range\".");
      if (ret2==-2) e->Throw("Error in SSRFPACK unif_() function: Nodes are colinear");
      if (ret2==-3) e->Throw("Error in SSRFPACK unif_() function: extrapolation failed due to the uniform grid extending too far beyond the triangulation boundary.");
    }

    return res->Transpose(NULL);
  }

  template< typename T1, typename T2>
  void gdlGrid2DData(DLong nx, DDouble* x, DLong ny, DDouble* y, DLong ntri, DLongGDL* tri, DDoubleGDL* xVal, DDoubleGDL* yVal, T1* zVal, bool domaxvalue, bool dominvalue, T2 maxVal, T2 minVal, T2 missVal, T1* res) {



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

    bool *found = new bool [nx * ny];
    for (SizeT i = 0; i < nx * ny; ++i) found[i] = false;

    // *** LOOP THROUGH TRIANGLES *** //
    DDouble diff[3][2];
    DDouble edge[3][3];

    DDouble delx10;
    DDouble delx21;
    DDouble dely10;
    DDouble dely21;
    DDouble delz10;
    DDouble delz21;
    DDouble A, B, C;

    // Loop through all triangles
    for (SizeT i = 0; i < ntri; ++i)
    {

      DLong tri0 = (*tri)[3 * i + 0];
      DLong tri1 = (*tri)[3 * i + 1];
      DLong tri2 = (*tri)[3 * i + 2];

      delx10 = (*xVal)[tri1] - (*xVal)[tri0];
      delx21 = (*xVal)[tri2] - (*xVal)[tri1];

      dely10 = (*yVal)[tri1] - (*yVal)[tri0];
      dely21 = (*yVal)[tri2] - (*yVal)[tri1];

      delz10 = (*zVal)[tri1] - (*zVal)[tri0];
      delz21 = (*zVal)[tri2] - (*zVal)[tri1];

      // Compute grid array
      for (SizeT j = 0; j < 3; ++j)
      {
        DLong itri = (*tri)[3 * i + j];
        DLong ktri = (*tri)[3 * i + ((j + 1) % 3)];
        edge[j][0] = (*xVal)[ktri] - (*xVal)[itri];
        edge[j][1] = (*yVal)[ktri] - (*yVal)[itri];
      }

      C = (delx21 * delz10 - delx10 * delz21) /
        (delx21 * dely10 - delx10 * dely21);
      B = (delz10 - C * dely10) / delx10;
      A = (*zVal)[tri0] - B * (*xVal)[tri0] - C * (*yVal)[tri0];


      // *** LOOP THROUGH GRID POINTS *** //
      if (domaxvalue || dominvalue)
      {
        // Loop through all y-grid values
        for (SizeT iy = 0; iy < ny; ++iy)
        {
          // Loop through all x-grid values
          for (SizeT ix = 0; ix < nx; ++ix)
          {
            if (found[iy * nx + ix]) continue;

            bool inside = true;

            // *** PLANE INTERPOLATION *** //

            // Compute diff array for xy-values
            for (SizeT j = 0; j < 3; ++j)
            {
              DLong itri = (*tri)[3 * i + j];
              diff[j][0] = (*xVal)[itri] - x[ix];
              diff[j][1] = (*yVal)[itri] - y[iy];
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
              found[iy * nx + ix] = true;
              DDouble dres = A + B * x[ix] + C * y[iy];
              if (dominvalue && dres < minVal) dres = missVal;
              if (domaxvalue && dres > maxVal) dres = missVal;
              (*res)[iy * nx + ix] = dres;
            }

          } // ix loop
        } // iy loop
      }//if minmax
      else
      {
        // Loop through all y-grid values
        for (SizeT iy = 0; iy < ny; ++iy)
        {
          // Loop through all x-grid values
          for (SizeT ix = 0; ix < nx; ++ix)
          {
            if (found[iy * nx + ix]) continue;

            bool inside = true;

            // *** PLANE INTERPOLATION *** //

            // Compute diff array for xy-values
            for (SizeT j = 0; j < 3; ++j)
            {
              DLong itri = (*tri)[3 * i + j];
              diff[j][0] = (*xVal)[itri] - x[ix];
              diff[j][1] = (*yVal)[itri] - y[iy];
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
              found[iy * nx + ix] = true;
              (*res)[iy * nx + ix] = A + B * x[ix] + C * y[iy];
            }

          } // ix loop
        }
      }
    } // i (triangle) loop

    delete[] found;

  }
  template<>
  void gdlGrid2DData(DLong nx, DDouble* x, DLong ny, DDouble* y, DLong ntri, DLongGDL* tri, DDoubleGDL* xVal, DDoubleGDL* yVal, 
    DComplexDblGDL* zVal, bool domaxvalue, bool dominvalue, DComplexDbl maxVal, DComplexDbl minVal, DComplexDbl missVal, DComplexDblGDL* res) {



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

    bool *found = new bool [nx * ny];
    for (SizeT i = 0; i < nx * ny; ++i) found[i] = false;

    // *** LOOP THROUGH TRIANGLES *** //
    DDouble diff[3][2];
    DDouble edge[3][3];

    DDouble delx10;
    DDouble delx21;
    DDouble dely10;
    DDouble dely21;
    DDouble delz10r;
    DDouble delz10i;
    DDouble delz21r;
    DDouble delz21i;
    DDouble Ar, Br, Cr, Ai, Bi, Ci;

    // Loop through all triangles
    for (SizeT i = 0; i < ntri; ++i)
    {

      DLong tri0 = (*tri)[3 * i + 0];
      DLong tri1 = (*tri)[3 * i + 1];
      DLong tri2 = (*tri)[3 * i + 2];

      delx10 = (*xVal)[tri1] - (*xVal)[tri0];
      delx21 = (*xVal)[tri2] - (*xVal)[tri1];

      dely10 = (*yVal)[tri1] - (*yVal)[tri0];
      dely21 = (*yVal)[tri2] - (*yVal)[tri1];

      delz10r = (*zVal)[tri1].real() - (*zVal)[tri0].real();
      delz10i = (*zVal)[tri1].imag() - (*zVal)[tri0].imag();
      delz21r = (*zVal)[tri2].real() - (*zVal)[tri1].real();
      delz21i = (*zVal)[tri2].imag() - (*zVal)[tri1].imag();

      // Compute grid array
      for (SizeT j = 0; j < 3; ++j)
      {
        DLong itri = (*tri)[3 * i + j];
        DLong ktri = (*tri)[3 * i + ((j + 1) % 3)];
        edge[j][0] = (*xVal)[ktri] - (*xVal)[itri];
        edge[j][1] = (*yVal)[ktri] - (*yVal)[itri];
      }

      Cr = (delx21 * delz10r - delx10 * delz21r) /  (delx21 * dely10 - delx10 * dely21);
      Br = (delz10r - Cr * dely10) / delx10;
      Ar = (*zVal)[tri0].real() - Br * (*xVal)[tri0] - Cr * (*yVal)[tri0];
      Ci = (delx21 * delz10r - delx10 * delz21r) /  (delx21 * dely10 - delx10 * dely21);
      Bi = (delz10i - Ci * dely10) / delx10;
      Ai = (*zVal)[tri0].imag() - Bi * (*xVal)[tri0] - Ci * (*yVal)[tri0];


      // *** LOOP THROUGH GRID POINTS *** //
      if (domaxvalue || dominvalue)
      {
        // Loop through all y-grid values
        for (SizeT iy = 0; iy < ny; ++iy)
        {
          // Loop through all x-grid values
          for (SizeT ix = 0; ix < nx; ++ix)
          {
            if (found[iy * nx + ix]) continue;

            bool inside = true;

            // *** PLANE INTERPOLATION *** //

            // Compute diff array for xy-values
            for (SizeT j = 0; j < 3; ++j)
            {
              DLong itri = (*tri)[3 * i + j];
              diff[j][0] = (*xVal)[itri] - x[ix];
              diff[j][1] = (*yVal)[itri] - y[iy];
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
              found[iy * nx + ix] = true;
              DDouble dres = Ar + Br * x[ix] + Cr * y[iy];
              if (dominvalue && dres < minVal.real()) dres = missVal.real();
              if (domaxvalue && dres > maxVal.real()) dres = missVal.real();
              (*res)[iy * nx + ix].real() = dres;

              dres = Ai + Bi * x[ix] + Ci * y[iy];
              if (dominvalue && dres < minVal.imag()) dres = missVal.imag();
              if (domaxvalue && dres > maxVal.imag()) dres = missVal.imag();
              (*res)[iy * nx + ix].imag() = dres;
            }

          } // ix loop
        } // iy loop
      }//if minmax
      else
      {
        // Loop through all y-grid values
        for (SizeT iy = 0; iy < ny; ++iy)
        {
          // Loop through all x-grid values
          for (SizeT ix = 0; ix < nx; ++ix)
          {
            if (found[iy * nx + ix]) continue;

            bool inside = true;

            // *** PLANE INTERPOLATION *** //

            // Compute diff array for xy-values
            for (SizeT j = 0; j < 3; ++j)
            {
              DLong itri = (*tri)[3 * i + j];
              diff[j][0] = (*xVal)[itri] - x[ix];
              diff[j][1] = (*yVal)[itri] - y[iy];
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
              found[iy * nx + ix] = true;
              (*res)[iy * nx + ix].real() = Ar + Br * x[ix] + Cr * y[iy];
              (*res)[iy * nx + ix].imag() = Ai + Bi * x[ix] + Ci * y[iy];;
            }

          } // ix loop
        }
      }
    } // i (triangle) loop

    delete[] found;

  }

  BaseGDL* trigrid_fun_plane(EnvT* e) {
    SizeT nParam = e->NParam();
    if (nParam < 4) e->Throw("Incorrect number of arguments.");
    //OK, trigrid does not care if more than 3 args in sphere mode. Limit at 6 is done at interpreter level.


    BaseGDL* p0 = e->GetParDefined(0); //x
    BaseGDL* p1 = e->GetParDefined(1); //y
    BaseGDL* p2 = e->GetParDefined(2); //z

    DType type=GDL_FLOAT;
    bool isComplex=(p2->Type()==GDL_COMPLEX || p2->Type()==GDL_COMPLEXDBL);
    if (p2->Type()==GDL_DOUBLE || p2->Type()==GDL_COMPLEXDBL) type=GDL_DOUBLE;

    BaseGDL* p3 = e->GetParDefined(3); //triangles

    if (p0->N_Elements() != p1->N_Elements() ||
      p0->N_Elements() != p2->N_Elements() ||
      p1->N_Elements() != p2->N_Elements())
      e->Throw("X, Y, or Z array dimensions are incompatible.");

    if (p0->N_Elements() < 3) e->Throw("Value of Bounds is out of allowed range.");

    if (p3->Rank() == 0)            e->Throw("Expression must be an array in this context: " + e->GetParString(0));
    if (p3->N_Elements() % 3 != 0)  e->Throw("Array of triangles incorrectly dimensioned.");

    DLong ntri = p3->N_Elements() / 3;

    if (p0->Rank() == 0)      e->Throw("Expression must be an array in this context: " + e->GetParString(0));
    if (p1->Rank() == 0)      e->Throw("Expression must be an array in this context: " + e->GetParString(1));
    if (p2->Rank() == 0)      e->Throw("Expression must be an array in this context: " + e->GetParString(2));

    static int xoutIx=e->KeywordIx( "XOUT");
    bool doXout=(e->KeywordSet(xoutIx)); 
    static int youtIx=e->KeywordIx( "YOUT");
    bool doYout=(e->KeywordSet(youtIx));
    if ((doXout && !doYout) || (doYout && !doXout)) e->Throw("Incorrect number of arguments.");
    static int xgridIx=e->KeywordIx( "XGRID");
    bool doXgrid=(e->KeywordPresent(xgridIx));
    static int ygridIx=e->KeywordIx( "YGRID");
    bool doYgrid=(e->KeywordPresent(ygridIx));

    // Get NX, NY values if present
    DLong nx = 51;
    DLong ny = 51;
    static int nxIx = e->KeywordIx("NX");
    static int nyIx = e->KeywordIx("NY");
    bool canUseLimitsx=true;
    bool canUseLimitsy=true;
    if (e->KeywordSet(nxIx)) { canUseLimitsx=false; e->AssureLongScalarKW(nxIx, nx);}
    if (e->KeywordSet(nyIx)) { canUseLimitsy=false; e->AssureLongScalarKW(nyIx, ny);}
    if (nx < 1 || ny < 1) e->Throw("Array dimensions must be greater than 0.");
    // we will further on define GS, whose value may overwrite the nx, ny.
 
    DDoubleGDL* GS = NULL;
    DDoubleGDL* limits = NULL;
    if (nParam > 4)
    {
      BaseGDL* p4 = e->GetParDefined(4);
      if (p4->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(4));
      if (p4->N_Elements() != 2) e->Throw("Array must have 2 elements: " + e->GetParString(4));
      GS = static_cast<DDoubleGDL*>(p4->Convert2(GDL_DOUBLE, BaseGDL::COPY));
    }
    if (nParam == 6)
    {
      BaseGDL* p5 = e->GetParDefined(5);
      if (p5->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(5));
      if (p5->N_Elements() != 4) e->Throw("Array must have 4 elements: " + e->GetParString(5));
      limits = static_cast<DDoubleGDL*> (p5->Convert2(GDL_DOUBLE, BaseGDL::COPY));      
    }
    DLong minxEl;
    DLong maxxEl;
    DLong minyEl;
    DLong maxyEl;
    DDoubleGDL* xVal = static_cast<DDoubleGDL*>(p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
    DDoubleGDL* yVal = static_cast<DDoubleGDL*>(p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));
    DLongGDL* tri = static_cast<DLongGDL*>(p3->Convert2(GDL_LONG, BaseGDL::COPY));
    //get min max X Y.
    xVal->MinMax(&minxEl, &maxxEl, NULL, NULL, true);
    yVal->MinMax(&minyEl, &maxyEl, NULL, NULL, true);
    // Determine grid range
    DLong xref=0, yref=0;
    DDouble xval = (*xVal)[minxEl];
    DDouble yval = (*yVal)[minyEl];
    DDouble xrange = (*xVal)[maxxEl] - (*xVal)[minxEl];
    DDouble yrange = (*yVal)[maxyEl] - (*yVal)[minyEl];
    //compute World positions of each pixels.
    if (limits != NULL)
    {
      if (canUseLimitsx) {
        xval = (*limits)[0];
        xrange = (*limits)[2] - xval;
      }
      if (canUseLimitsy) {
        yval = (*limits)[1];
        yrange = (*limits)[3] - yval;
      }
    }
    // Determine grid spacing
    DDouble xinc = xrange / (nx-1);
    DDouble yinc = yrange / (ny-1);
    if (GS != NULL && canUseLimitsx)
    {
      xinc = (*GS)[0];
      nx = (DLong) (xrange / xinc) +1;
    }
    if (GS != NULL && canUseLimitsy)
    {
      yinc = (*GS)[1];
      ny = (DLong) (yrange / yinc) +1;
    }

    DDouble *x, *y;
    bool cleanx=false;
    bool cleany=false;

    if (doXout) {
      DDoubleGDL* xOut=e->GetKWAs<DDoubleGDL>(xoutIx);
      nx=xOut->N_Elements();
      if (nx < 1) e->Throw("XOUT must have >1 elements.");
      x=(DDouble*)xOut->DataAddr();
    } else {
      x = (DDouble*)malloc(nx*sizeof(DDouble));
      for (int i=0; i<nx; ++i) x[i]=(i-xref)*xinc+xval;
      cleanx=true;
    }

    if (doYout) {
      DDoubleGDL* yOut=e->GetKWAs<DDoubleGDL>(youtIx);
      ny=yOut->N_Elements();
      if (ny < 1) e->Throw("YOUT must have >1 elements.");
      y=(DDouble*)yOut->DataAddr();
    } else {
      y = (DDouble*)malloc(ny*sizeof(DDouble));
      for (int j=0; j<ny; ++j) y[j]=(j-yref)*yinc+yval;
      cleany=true;
    }

    //must be done here
    if (doXgrid) {
      DDoubleGDL *xgrid=new DDoubleGDL(nx, BaseGDL::NOZERO);
      for (int i=0; i<nx; ++i) (*xgrid)[i]=x[i];
      e->SetKW(xgridIx, xgrid);
    }
    if (doYgrid) {
      DDoubleGDL *ygrid=new DDoubleGDL(ny, BaseGDL::NOZERO);
      for (int i=0; i<ny; ++i) (*ygrid)[i]=y[i];
      e->SetKW(ygridIx, ygrid);
    }


    // Setup return array
    DLong dims[2];
    dims[0] = nx;
    dims[1] = ny;
    dimension dim((DLong *) dims, 2);

    if (isComplex) {
      DComplexDblGDL* minValG=NULL;
      static int minvalueIx=e->KeywordIx( "MIN_VALUE");
      bool dominvalue=(e->KeywordPresent(minvalueIx));
      if (dominvalue) minValG=e->GetKWAs<DComplexDblGDL>(minvalueIx);
      DComplexDblGDL* maxValG=NULL;
      static int maxvalueIx=e->KeywordIx( "MAX_VALUE");
      bool domaxvalue=(e->KeywordPresent(maxvalueIx));
      if (domaxvalue) maxValG=e->GetKWAs<DComplexDblGDL>(maxvalueIx);
      DComplexDblGDL* missValG=NULL;
      static int missvalueIx=e->KeywordIx( "MISSING");
      if (e->KeywordPresent(missvalueIx)) missValG=e->GetKWAs<DComplexDblGDL>(missvalueIx);
      DComplexDblGDL* zVal = static_cast<DComplexDblGDL*>(p2->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
      DComplexDblGDL* res = new DComplexDblGDL(dim, BaseGDL::ZERO);
      DComplexDbl minVal=std::complex<double>(0,0);
      if (minValG!=NULL) minVal=std::complex<double>((*minValG)[0].real(),(*minValG)[0].imag());
      DComplexDbl maxVal=std::complex<double>(0,0);
      if (maxValG!=NULL) maxVal=std::complex<double>((*maxValG)[0].real(),(*maxValG)[0].imag());
      DComplexDbl missVal=std::complex<double>(0,0);
      if (missValG!=NULL) missVal=std::complex<double>((*missValG)[0].real(),(*missValG)[0].imag());
      gdlGrid2DData< DComplexDblGDL, DComplexDbl>(nx, x, ny, y, ntri, tri, xVal, yVal, zVal, domaxvalue, dominvalue, maxVal, minVal, missVal, res);
      if (type==GDL_FLOAT) return res->Convert2(GDL_COMPLEX,BaseGDL::CONVERT); else return res;
    }else{
      DDouble minVal=0;
      static int minvalueIx=e->KeywordIx( "MIN_VALUE");
      bool dominvalue=(e->KeywordPresent(minvalueIx));
      if (dominvalue) e->AssureDoubleScalarKW(minvalueIx, minVal);
      DDouble maxVal=0;
      static int maxvalueIx=e->KeywordIx( "MAX_VALUE");
      bool domaxvalue=(e->KeywordPresent(maxvalueIx));
      if (domaxvalue) e->AssureDoubleScalarKW(maxvalueIx, maxVal);
      DDouble missVal=0.0;
      static int missvalueIx=e->KeywordIx( "MISSING");
      e->AssureDoubleScalarKWIfPresent(missvalueIx, missVal);
      DDoubleGDL* zVal = static_cast<DDoubleGDL*>(p2->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::ZERO);    
      gdlGrid2DData< DDoubleGDL, DDouble>(nx, x, ny, y, ntri, tri, xVal, yVal, zVal, domaxvalue, dominvalue, maxVal, minVal, missVal, res);
      if (type==GDL_FLOAT) return res->Convert2(GDL_FLOAT,BaseGDL::CONVERT); else return res;
    }
  }
  

  BaseGDL* trigrid_fun(EnvT* e) {
    //just send to normal or spherical based on presence of SPHERE argument
    static int sphereIx = e->KeywordIx( "SPHERE");
    if(e->KeywordPresent(sphereIx)) return trigrid_fun_spherical(e); else return trigrid_fun_plane(e);
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

