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

#include "includefirst.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"

using namespace std;
#include "delaunator_patched.hpp"
#include "tripack.c"
#include "stripack.c"
#include "ssrfpack.c"
//#include "akima760.c"
//#include "akima761.c"

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

    static int sphereIx=e->KeywordIx( "SPHERE");
    static int degreeIx=e->KeywordIx( "DEGREES");
    static int fvalueIx=e->KeywordIx( "FVALUE");
    bool isSphere=(e->KeywordPresent(sphereIx));
    bool doDegree=(e->KeywordPresent(degreeIx));
    bool hasFvalue=(e->KeywordPresent(fvalueIx));
    if (!isSphere) {
      doDegree=false;
      hasFvalue=false;
    }
    bool wantsDupes=false;
    static int dupesIx=e->KeywordIx( "REPEATS");
    if( e->KeywordPresent( dupesIx)) wantsDupes=true;
    
    bool wantsConnectivity=false;
    static int connIx=e->KeywordIx( "CONNECTIVITY");
    if( e->KeywordPresent( connIx)) wantsConnectivity=true;
    
    //check since we return values in them 
    if (isSphere)  e->AssureGlobalKW(sphereIx); 
    if (hasFvalue)  e->AssureGlobalKW(fvalueIx);
    if (wantsDupes)  e->AssureGlobalKW(dupesIx); 
    if (wantsConnectivity)  e->AssureGlobalKW(connIx);
    
    //some things depend on X and Y being doubles of not
    bool isDouble=false;
    if ( e->GetParDefined(0)->Type()==GDL_DOUBLE &&  e->GetParDefined(1)->Type()==GDL_DOUBLE )  isDouble=true;
    xVal = e->GetParAs< DDoubleGDL > (0);
    if (xVal->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(0));
    npts = xVal->N_Elements();
    yVal = e->GetParAs< DDoubleGDL > (1);
    if (yVal->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(1));
    if (yVal->N_Elements() != npts) e->Throw("X & Y arrays must have same number of points.");
    if (hasFvalue) {
      fvalue=e->GetKWAs<DDoubleGDL>(fvalueIx);
      if (fvalue->Rank() == 0) e->Throw("Expression must be an array in this context: " +  e->GetString( fvalueIx)+".");
      if (fvalue->N_Elements() != npts) e->Throw("X & Y arrays must have same number of points."); //yes yes.
    }
   
    DLong maxEl, minEl;
    DDouble maxVal;
    xVal->MinMax(&minEl,&maxEl,NULL,NULL,false);
    //maximum ABSOLUTE value
    maxVal=abs((*xVal)[maxEl]);
    yVal->MinMax(&minEl,&maxEl,NULL,NULL,false);
    //maximum ABSOLUTE value
    maxVal=max(maxVal,abs((*yVal)[maxEl]));

    DDouble dtol = isDouble ? 1e-12 : 1e-6;
    //Tol is irrelevant in our implementation, as (s)tripack work with tol=machine precision. 
    
    DDouble* xx=&(*xVal)[0];
    DDouble* yy=&(*yVal)[0];    

    std::vector<std::pair<DLong,DLong>> dupes;

    //IN SPHERE MODE, xVal and yVal ARE RETURNED, ARE DOUBLE PRECISION and their ORDER is MODIFIED,
    //the input points are sorted using the coordinate (x or y) covering max range (y prefered). It is not the case here.
    //SPHERE does not support duplicate points yet.
    
    if (isSphere) {
      DDoubleGDL *sc_xVal=xVal;
      DDoubleGDL *sc_yVal=yVal;
      Guard<BaseGDL>xguard;
      Guard<BaseGDL>yguard;
      if (doDegree) {
        static DDouble DToR=double(3.1415926535897932384626433832795)/180.0;
        sc_xVal=new DDoubleGDL(npts,BaseGDL::NOZERO);
        for (DLong i=0; i<npts; ++i) (*sc_xVal)[i]=(*xVal)[i]*DToR;
        xguard.reset(sc_xVal);
        sc_yVal=new DDoubleGDL(npts,BaseGDL::NOZERO);
        for (DLong i=0; i<npts; ++i) (*sc_yVal)[i]=(*yVal)[i]*DToR;
        yguard.reset(sc_yVal);
      }
      DDoubleGDL* x=new DDoubleGDL(npts,BaseGDL::NOZERO);
      DDoubleGDL* y=new DDoubleGDL(npts,BaseGDL::NOZERO);
      DDoubleGDL* z=new DDoubleGDL(npts,BaseGDL::NOZERO);
      //Stripack is lat, lon and IDL lon,lat
      DLong ret0=stripack::trans_(&npts, (DDouble*)sc_yVal->DataAddr(), (DDouble*)sc_xVal->DataAddr(),(DDouble*)x->DataAddr(),(DDouble*)y->DataAddr(),(DDouble*)z->DataAddr()); 
      DLong listsize=6*npts-12;
      //we use GDL objects as they will be visible outside at GDL level.
      DLongGDL* list=new DLongGDL(listsize,BaseGDL::NOZERO);
      DLongGDL* lptr=new DLongGDL(listsize,BaseGDL::NOZERO);
      DLongGDL* lend=new DLongGDL(npts,BaseGDL::NOZERO);
      DLong* near__=(DLong*)malloc(npts*sizeof(DLong)); //initial name "near" would not work as this is reserved in Windows C. 
      DLong* next=(DLong*)malloc(npts*sizeof(DLong));
      DDouble* dist=(DDouble*)malloc(npts*sizeof(DDouble));
      DLong ier=0;
      DLong lnew=0;
      DLong ret1=stripack::sph_trmesh_(&npts,(DDouble*)x->DataAddr(),(DDouble*)y->DataAddr(),(DDouble*)z->DataAddr(), (DLong*)list->DataAddr(), (DLong*)lptr->DataAddr(), (DLong*)lend->DataAddr(), &lnew, near__, next, dist, &ier);
      free(dist);
      free(next);
      free(near__);
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
      if (hasFvalue) {
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
       //no connectivity in SPHERE mode in IDL, but yes we can!
      if (wantsConnectivity) {
        //remove 1 to get C array indexes.
        for (SizeT i = 0; i < lnew-1; ++i) (*lptr)[i]--; 
        for (SizeT i = 0; i < npts; ++i) (*lend)[i]--;
        DLong* array=(DLong*)malloc((2*(lnew-1))*sizeof(DLong)); // size > max possible connectivity 
        SizeT runningindex=npts+1;
        SizeT startindex=0;
        array[startindex++]=npts+1;
        for (SizeT i = 0; i < npts; ++i) {
          //is it an exterior point? Yes if the termination of the connectivity list is exterior.
          DLong lpl=(*lend)[i];
          if ((*list)[lpl]<0) array[runningindex++]=i; //exterior - we write it
          //write all points until nfin=lend[i] is found again using lptr connectivity pointers:
          DLong lp=lpl; //see nbcnt_()
          do {
            lp=(*lptr)[lp];
            array[runningindex++]=((*list)[lp]>0)?(*list)[lp]-1:(-(*list)[lp])-1;
          } while (lp!=lpl);
          array[startindex++]=runningindex;
        }
        DLongGDL* connections = new DLongGDL(runningindex, BaseGDL::NOZERO);
        for (SizeT i = 0; i < runningindex; ++i) (*connections)[i]=array[i];
        e->SetKW(connIx,connections);
        free(array);
      }
      //no more cleanup, x,y,z,and list,lptr,lend are in the returned structure!
    } else {
      
//      // for PLANE triangulation, everything must be scaled in order to have triangulation independent of range,
//      // as the triangulation code IS sensitive to the values of X and Y.
//      if (maxVal > 0) {
//        for (DLong i = 0; i < npts; ++i) xx[i] /= maxVal;
//        for (DLong i = 0; i < npts; ++i) yy[i] /= maxVal;
//      }

      std::vector<double> coords;
      for (DLong i = 0; i < npts; ++i) {coords.push_back(xx[i]);coords.push_back(yy[i]);}
      delaunator::Delaunator tri(coords);
      
       if (wantsTriangles) {
         DLong ntriangles=tri.triangles.size()/3;
         //convert to triangle list
        SizeT d[2];
        d[1] = ntriangles;
        d[0] = 3;
        DLongGDL* returned_triangles = new DLongGDL(dimension(d, 2), BaseGDL::NOZERO);        
        for (DLong j = 0; j < ntriangles; ++j)
        {
          for (int i = 0; i < 3; ++i)
          {
            (*returned_triangles)[3 * j + i] = tri.triangles[3*j+i]; 
          }
        }
        e->SetPar(2, returned_triangles);
       }
      
//      if (maxVal > 0) {
//        for (DLong i = 0; i < npts; ++i) xx[i] *= maxVal;
//        for (DLong i = 0; i < npts; ++i) yy[i] *= maxVal;
//      }      

// ************ TO BE REMOVED ************      
      
//      SizeT listsize=6*npts-12;
//      DLong* list=(DLong*)malloc(listsize*sizeof(DLong));
//      DLong* lptr=(DLong*)malloc(listsize*sizeof(DLong));
//      DLong* lend=(DLong*)malloc(npts*sizeof(DLong));
//      DLong* near__=(DLong*)malloc(npts*sizeof(DLong)); //"near" is reserved in Windows C.
//      DLong* next=(DLong*)malloc(npts*sizeof(DLong));
//      DDouble* dist=(DDouble*)malloc(npts*sizeof(DDouble));
//      DLong ier=0;
//      DLong lnew=0;
//      DLong l_npts=npts;
//      DLong* originalIndex=(DLong*)malloc(npts*sizeof(DLong));
//      for (DLong i = 0; i < npts; ++i) originalIndex[i]=i;
//
//      //Try first with the simple case of all points OK, if perchance the list of points is OK we gain time:
//      DLong ret1 = tripack::trmesh_(&l_npts, xx, yy, list, lptr, lend, &lnew, near__, next, dist, &ier);
//      DLong Offset=2;
//      if (ier != 0) {
//        //OK, complicated case.
//
//        //The first 3 points passed to TriPack must NOT be colinear (according to Tripack).
//        //In many cases the points are on a grid, so we need to find the first noncolinear 3 points and start the triangulation there.
//        //The N first colinear points can be added afterwards.
//        //Test colinearity:
//
//        bool colinear = true;
//        while (Offset < npts && colinear) {
//          if (!tripack::colin_(xx[Offset-2], yy[Offset-2], xx[Offset - 1], yy[Offset - 1], xx[Offset], yy[Offset])) {
//            colinear = false;
//            break;
//          }
//          Offset++;
//        }
//        if (colinear) e->Throw("Points are co-linear, no solution.");
//
//        //At this point, either there were duplicates (colinear) or colinear points.
//
//        //we will work on a copy. To hell with speed, this is not optimized.
//        DDouble* l_xx = (DDouble*) malloc(l_npts * sizeof (DDouble));
//        DDouble* l_yy = (DDouble*) malloc(l_npts * sizeof (DDouble));
//        //start with Offset and add the points before Offset at the end. 
//        memcpy(l_xx, xx, l_npts * sizeof (DDouble));
//        memcpy(l_yy, yy, l_npts * sizeof (DDouble));
//        if (Offset != 2) { // exchange Offset and position 0. It is fundamental that it is position 0 and not 1 or 2.
//          l_xx[Offset] = xx[0];
//          l_xx[0] = xx[Offset];
//          l_yy[Offset] = yy[0];
//          l_yy[0] = yy[Offset];
//          originalIndex[0] = Offset;
//          originalIndex[Offset] = 0;
//        }
//        // if ndupes > 0 we have to take into account that several returned indexes in the list have been changed wrt the original because of the list shortening
//        // by elimination of the duplicates. So we have to create a table of correspondence.
//
//        // redo triangulation, hope it works:
//        ret1 = tripack::trmesh_(&l_npts, l_xx, l_yy, list, lptr, lend, &lnew, near__, next, dist, &ier);
//        if (ier != 0) {
//          if (ier > 0) { //this can happen.
//            // and, yes, it would be more efficient to use tripack::addnod_()
//            while (ier > 0) {
//              ier--;
//              DLong m = 0;
//              for (DLong i = 0; i < lnew - 1; ++i) if (list[i] > m) m = list[i]; //simple way to compute where we are
//              dupes.push_back(make_pair(originalIndex[ier], originalIndex[m])); // value at index 'm' is identical to the one at 'ier'
//              //remove node "m" and add +1 to originalIndex starting at 'm'
//              for (DLong i = m; i < l_npts - 1; ++i) {
//                l_xx[i] = l_xx[i + 1];
//                l_yy[i] = l_yy[i + 1];
//                originalIndex[i] = originalIndex[i + 1];
//              }
//              l_npts--;
//              ret1 = tripack::trmesh_(&l_npts, l_xx, l_yy, list, lptr, lend, &lnew, near__, next, dist, &ier);
//              if (ier < 0) e->Throw("Internal Error, please report."); //Should not happen
//            }
//            free(l_xx);
//            free(l_yy);
//          }
//          //the rest SHOULD NOT HAPPEN:
//          else if (ier == -2) e->Throw("Internal Error, points are co-linear, no solution, please report."); //Should not happen
//          else if (ier == -1) e->Throw("Internal Error, not enough valid and unique points specified, please report."); //ier==-1 impossible (npoints < 3)
//            //ier==-4 internal error should be reported! -- never happened in 40 years
//          else if (ier == -4) e->Throw("Congratulations, you found the impossible: a set of points that triggers an internal error in TRMESH, please report!");
//          else e->Throw("Congratulations, you found an impossible error code of the TRIPACK package, please report.");
//          }
//      }
//      DLong ndupes=dupes.size();
//      free(near__);
//      free(next);
//      free(dist); 
//      
//      //Dupes indexing is the original one.
//      if (wantsDupes) {
//        if (dupes.size() ==0) {
//          DLongGDL* nothing=new DLongGDL(dimension(2),BaseGDL::ZERO); 
//          nothing->Dec();
//          e->SetKW(dupesIx, nothing);
//        }
//        else {
//          DLongGDL* returned_dupes = new DLongGDL(dimension(2,dupes.size()), BaseGDL::NOZERO);
//          for (DLong i = 0; i < dupes.size(); ++i) {
//            (*returned_dupes)[2*i+0]=dupes[i].first;
//            (*returned_dupes)[2*i+1]=dupes[i].second;
//          } 
//          e->SetKW(dupesIx, returned_dupes);
//        }
//      }
//      
//      //All indexes in the returned arrays must be replaced by those of originalIndex.
//
//      if (wantsTriangles) {
//        //convert to triangle list
//        DLong noconstraints = 0;
//        DLong dummylistofconstraints = 0;
//        DLong nrow = 6; //no arcs.
//        DLong* ltri = (DLong*) malloc((12 * l_npts) * sizeof (DLong));
//        DLong lct = 0;
//        DLong ntriangles = 0;
//        DLong ret2 = tripack::trlist_(&noconstraints, &dummylistofconstraints, &l_npts, list, lptr, lend, &nrow, &ntriangles, ltri, &lct, &ier);
//        if (ier != 0)
//        {
//          free(ltri);
//          free(list);
//          free(lptr);
//          free(lend);
//          e->Throw("Unexpected Error in TRIPACK, TRLIST routine. Please report.");
//        }
//        SizeT d[2];
//        d[1] = ntriangles;
//        d[0] = 3;
//        DLongGDL* returned_triangles = new DLongGDL(dimension(d, 2), BaseGDL::NOZERO);
//        for (DLong j = 0; j < ntriangles; ++j)
//        {
//          for (int i = 0; i < 3; ++i)
//          {
//            (*returned_triangles)[3 * j + i] = originalIndex[ltri[6 * j + i] - 1]; //our nubering starts a 0.
//          }
//        }
//        free(ltri);
//        //pass back to GDL env:
//        e->SetPar(2, returned_triangles);
//      }
//      
//      if (wantsEdge) {
//        DLong* nodes=(DLong*)malloc(l_npts*sizeof(DLong));
//        DLong nb=0;
//        DLong na=0;
//        DLong nt=0;
//        DLong ret3=tripack::bnodes_(&l_npts, list, lptr, lend, nodes, &nb, &na, &nt);
//        DLongGDL* returned_edges = new DLongGDL(nb, BaseGDL::NOZERO);
//        for (DLong j = 0; j < nb; ++j) (*returned_edges)[j]=originalIndex[nodes[j]-1];
//        free(nodes);
//        e->SetPar(3, returned_edges);
//      }
//      
//      if (wantsConnectivity) {
//        //remove 1 to get C array indexes.
//        for (DLong i = 0; i < lnew-1; ++i) lptr[i]--; 
//        for (DLong i = 0; i < l_npts; ++i) lend[i]--;
//        //in connectivity we MUST have all the points, even the duplicated ones. Connectivity of duplicated points is wrong with IDL (intentional?).
//        // we could avoid this easily as I believe this is an IDL bug. We just have to reproduce the connectivity of the first encounter
//        // of the duplicated point.
//        DLong* array=(DLong*)malloc((npts*npts+npts+1)*sizeof(DLong)); // size > max possible connectivity 
//        DLong runningindex = npts+1; // and not l_npts: we report for all points 
//        DLong startindex=0;
//        array[startindex++]= npts+1;
//        DLong* effective_index=(DLong*)malloc(npts*sizeof(DLong));//this is the list of npts vertexes for which we want the neighbour. It must be
//        // the  list of l_npts (returned) indexes, with indexes of the first instance of duplicated points for the duplicated points.
//        if (ndupes == 0) for (DLong i=0; i< npts; ++i) effective_index[i]=i; //easy
//        else { //piecewise index construction, each time dupes.second is encountered, insert dupes.first instead of increasing index:
//          DLong i=0; DLong k=0; //i:index from 0 to l_npts-1., k running index of 0 to npts-1
//          for (DLong idup=0; idup< ndupes; ++idup) {
//            DLong encounter=dupes[idup].second;
//            while (k<encounter) effective_index[k++]=i++;
//            effective_index[k++]=dupes[idup].first;
//          }
//          while (k<npts) effective_index[k++]=i++;
//        }
//        //nice except that we may have exchanged Offset and 0 in the first place, so effective_index should reflect this
//        if (Offset != 2) {
//          DLong tmp=effective_index[Offset];
//          effective_index[Offset] = effective_index[0];
//          effective_index[0] = tmp;
//        }
//        
//        for (DLong k = 0; k < npts; ++k) { 
//          DLong i=effective_index[k]; 
//          //is it an exterior point? Yes if the termination of the connectivity list is exterior.
//          DLong lpl=lend[i];
//          if (list[lpl]<0) array[runningindex++]=originalIndex[i]; //exterior - we write it
//          //write all points until nfin=lend[i] is found again using lptr connectivity pointers:
//          DLong lp=lpl; //see nbcnt_()
//          do {
//            lp=lptr[lp];
//            array[runningindex]=(list[lp]>0)?list[lp]-1:(-list[lp])-1;
//            array[runningindex]=originalIndex[array[runningindex]];
//            runningindex++;
//          } while (lp!=lpl);
//          array[startindex++]=runningindex;
//        }
//        DLongGDL* connections = new DLongGDL(runningindex, BaseGDL::NOZERO);
//        for (DLong i = 0; i < runningindex; ++i) (*connections)[i]=array[i];
//        e->SetKW(connIx,connections);
//        free(array);
//      }
//      //cleanup
//      free(list);
//      free(lptr);
//      free(lend);
//      if (maxVal > 0) {
//        for (DLong i = 0; i < npts; ++i) xx[i] *= maxVal;
//        for (DLong i = 0; i < npts; ++i) yy[i] *= maxVal;
//      }
    }
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
    
    // Determine grid range
    DDouble xref=0.0, yref=0.0;
    DDouble xval, xinc;
    DDouble yval, yinc;
    DDouble xrange; 
    DDouble yrange;
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

  struct triAccel {
    DDouble y23, x32, y31, x13, x3, y3; //vertex coordinates accelerators
    DDouble denom; //accelerator for barycentric coordinates
    DDouble zac, zbc, zc; //accelerators heights
    int pxmin, pxmax, pymin, pymax; //boundingBox
  };
  struct triAccelCpx {
    DDouble y23, x32, y31, x13, x3, y3; //vertex coordinates accelerators
    DDouble denom; //accelerator for barycentric coordinates
    DComplexDbl zac, zbc, zc; //accelerators heights
    int pxmin, pxmax, pymin, pymax; //boundingBox
  };  
void barycentricCoord(const triAccel &v, const DDouble xx, const DDouble yy, DDouble &val) {
    DDouble a, b, c;
    //NOTE: v.denom negative as triangles are clockwise with our triangulation.
    //already done in calling program! DDouble xx = (x - v.x3);  DDouble yy = (y - v.y3);
    a = (v.y23 * xx + v.x32 * yy); // for sign, no need to // * v.denom;
    if (a > 0) return; //since must be negative
    b = (v.y31 * xx + v.x13 * yy); // idem // * v.denom;
    if (b > 0) return; //idem
    c = (a + b) * v.denom;
    if (c > 1) return;
    val = (a * (v.zac) + b * (v.zbc)) * v.denom + v.zc;
  }
void barycentricCoordCpx(const triAccelCpx &v, const DDouble xx, const DDouble yy, DComplexDbl &val) {
    DDouble a, b, c;
    //NOTE: v.denom negative as triangles are clockwise with our triangulation.
    //already done in calling program! DDouble xx = (x - v.x3);  DDouble yy = (y - v.y3);
    a = (v.y23 * xx + v.x32 * yy); // for sign, no need to // * v.denom;
    if (a > 0) return; //since must be negative
    b = (v.y31 * xx + v.x13 * yy); // idem // * v.denom;
    if (b > 0) return; //idem
    c = (a + b) * v.denom;
    if (c > 1) return;
    DDouble valbisR = (a * (v.zac.real()) + b * (v.zbc.real())) * v.denom + v.zc.real();
    DDouble valbisI = (a * (v.zac.imag()) + b * (v.zbc.imag())) * v.denom + v.zc.imag();
    val = std::complex<double>(valbisR,valbisI);
  }
template< typename T2>
void barycentricCoordMinMax(const triAccel &v, const DDouble xx, const DDouble yy, DDouble &val, bool domaxvalue, bool dominvalue, T2 maxVal, T2 minVal ) {
    DDouble a, b, c;
    //NOTE: v.denom negative as triangles are clockwise with our triangulation.
    //already done in calling program! DDouble xx = (x - v.x3);  DDouble yy = (y - v.y3);
    a = (v.y23 * xx + v.x32 * yy); // for sign, no need to // * v.denom;
    if (a > 0) return; //since must be negative
    b = (v.y31 * xx + v.x13 * yy); // idem // * v.denom;
    if (b > 0) return; //idem
    c = (a + b) * v.denom;
    if (c > 1) return;
    DDouble valbis = (a * (v.zac) + b * (v.zbc)) * v.denom + v.zc;
    if ((dominvalue && valbis < minVal) || (domaxvalue && valbis > maxVal)) {} else val = valbis;
}
void barycentricCoordMinMaxCpx(const triAccelCpx &v, const DDouble xx, const DDouble yy, DComplexDbl &val, bool domaxvalue, bool dominvalue, DComplexDbl maxVal, DComplexDbl minVal ) {
    DDouble a, b, c;
    //NOTE: v.denom negative as triangles are clockwise with our triangulation.
    //already done in calling program! DDouble xx = (x - v.x3);  DDouble yy = (y - v.y3);
    a = (v.y23 * xx + v.x32 * yy); // for sign, no need to // * v.denom;
    if (a > 0) return; //since must be negative
    b = (v.y31 * xx + v.x13 * yy); // idem // * v.denom;
    if (b > 0) return; //idem
    c = (a + b) * v.denom;
    if (c > 1) return;
    DDouble valbisR = (a * (v.zac.real()) + b * (v.zbc.real())) * v.denom + v.zc.real();
    DDouble valbisI = (a * (v.zac.imag()) + b * (v.zbc.imag())) * v.denom + v.zc.imag();
    if ((dominvalue && valbisR < minVal.real()) || (domaxvalue && valbisR > maxVal.real())) { valbisR = val.real(); }
    if ((dominvalue && valbisI < minVal.imag()) || (domaxvalue && valbisI > maxVal.imag())) { valbisI = val.imag(); }
    val = std::complex<double>(valbisR,valbisI);
}

template< typename T1>
vector <triAccel> getTriAccelUniform(const DLong ntri, const DLongGDL* tri, const DDoubleGDL* xVal, const DDoubleGDL* yVal, const T1* zVal,
  const DLong nx, const DLong xref, const DDouble xval, const DDouble xinc, const DLong ny, const DLong yref, const DDouble yval, const DDouble yinc) {
    // Loop through all triangles and create properties
    vector <triAccel> triVect;

    for (SizeT triIndex = 0; triIndex < ntri; ++triIndex) {
      triAccel v = triAccel();
      DDouble xmin;
      DDouble xmax;
      DDouble ymin;
      DDouble ymax;
      DDouble x1, y1;
      xmax = xmin = x1 = (*xVal)[(*tri)[3 * triIndex]];
      ymax = ymin = y1 = (*yVal)[(*tri)[3 * triIndex]];
      DDouble x2 = (*xVal)[(*tri)[3 * triIndex + 1]];
      DDouble y2 = (*yVal)[(*tri)[3 * triIndex + 1]];
      v.x3 = (*xVal)[(*tri)[3 * triIndex + 2]];
      v.y3 = (*yVal)[(*tri)[3 * triIndex + 2]];
      v.zc = (*zVal)[(*tri)[3 * triIndex + 2]];
      v.zac = (*zVal)[(*tri)[3 * triIndex]] - v.zc;
      v.zbc = (*zVal)[(*tri)[3 * triIndex + 1]] - v.zc;
      v.y23 = (y2 - v.y3);
      v.x32 = (v.x3 - x2);
      v.y31 = (v.y3 - y1);
      v.x13 = (x1 - v.x3);

      v.denom = ((y2 - v.y3)*(x1 - v.x3) + (v.x3 - x2)*(y1 - v.y3));
      v.denom = 1.0 / v.denom;
      //finish boundingbox
      for (int k = 1; k < 3; ++k) {
        DDouble z = (*xVal)[(*tri)[3 * triIndex + k]];
        xmin = (z < xmin) ? z : xmin;
        xmax = (z > xmax) ? z : xmax;
        z = (*yVal)[(*tri)[3 * triIndex + k]];
        ymin = (z < ymin) ? z : ymin;
        ymax = (z > ymax) ? z : ymax;
      }
      // x=(i-xref)*xinc+xval; -> i=(x-xval)/xinc+xref
      v.pxmin = floor((xmin - xval) / xinc) + xref;
      v.pxmin = (v.pxmin < 0) ? 0 : v.pxmin;
      v.pxmax = ceil((xmax - xval) / xinc) + xref;
      v.pxmax = (v.pxmax > nx - 1) ? nx - 1 : v.pxmax;
      v.pymin = floor((ymin - yval) / yinc) + yref;
      v.pymin = (v.pymin < 0) ? 0 : v.pymin;
      v.pymax = ceil((ymax - yval) / yinc) + yref;
      v.pymax = (v.pymax > ny - 1) ? ny - 1 : v.pymax;
      triVect.push_back(v);
    }
    return triVect;
}

vector <triAccelCpx> getTriAccelUniformCpx(const DLong ntri, const DLongGDL* tri, const DDoubleGDL* xVal, const DDoubleGDL* yVal, const DComplexDblGDL* zVal,
  const DLong nx, const DLong xref, const DDouble xval, const DDouble xinc, const DLong ny, const DLong yref, const DDouble yval, const DDouble yinc) {
    // Loop through all triangles and create properties
    vector <triAccelCpx> triVect;

    for (SizeT triIndex = 0; triIndex < ntri; ++triIndex) {
      triAccelCpx v = triAccelCpx();
      DDouble xmin;
      DDouble xmax;
      DDouble ymin;
      DDouble ymax;
      DDouble x1, y1;
      xmax = xmin = x1 = (*xVal)[(*tri)[3 * triIndex]];
      ymax = ymin = y1 = (*yVal)[(*tri)[3 * triIndex]];
      DDouble x2 = (*xVal)[(*tri)[3 * triIndex + 1]];
      DDouble y2 = (*yVal)[(*tri)[3 * triIndex + 1]];
      v.x3 = (*xVal)[(*tri)[3 * triIndex + 2]];
      v.y3 = (*yVal)[(*tri)[3 * triIndex + 2]];
      v.zc = (*zVal)[(*tri)[3 * triIndex + 2]];
      v.zac =  std::complex<double>( (*zVal)[(*tri)[3 * triIndex]].real() - v.zc.real() , (*zVal)[(*tri)[3 * triIndex]].imag() - v.zc.imag() ) ;
      v.zbc =  std::complex<double>( (*zVal)[(*tri)[3 * triIndex +1]].real() - v.zc.real() , (*zVal)[(*tri)[3 * triIndex +1]].imag() - v.zc.imag() ) ;
      v.y23 = (y2 - v.y3);
      v.x32 = (v.x3 - x2);
      v.y31 = (v.y3 - y1);
      v.x13 = (x1 - v.x3);

      v.denom = ((y2 - v.y3)*(x1 - v.x3) + (v.x3 - x2)*(y1 - v.y3));
      v.denom = 1.0 / v.denom;
      //finish boundingbox
      for (int k = 1; k < 3; ++k) {
        DDouble z = (*xVal)[(*tri)[3 * triIndex + k]];
        xmin = (z < xmin) ? z : xmin;
        xmax = (z > xmax) ? z : xmax;
        z = (*yVal)[(*tri)[3 * triIndex + k]];
        ymin = (z < ymin) ? z : ymin;
        ymax = (z > ymax) ? z : ymax;
      }
      // x=(i-xref)*xinc+xval; -> i=(x-xval)/xinc+xref
      v.pxmin = floor((xmin - xval) / xinc) + xref;
      v.pxmin = (v.pxmin < 0) ? 0 : v.pxmin;
      v.pxmax = ceil((xmax - xval) / xinc) + xref;
      v.pxmax = (v.pxmax > nx - 1) ? nx - 1 : v.pxmax;
      v.pymin = floor((ymin - yval) / yinc) + yref;
      v.pymin = (v.pymin < 0) ? 0 : v.pymin;
      v.pymax = ceil((ymax - yval) / yinc) + yref;
      v.pymax = (v.pymax > ny - 1) ? ny - 1 : v.pymax;
      triVect.push_back(v);
    }
    return triVect;
}

template< typename T1>
vector <triAccel> getTriAccelRandom( const DLong ntri,  const DLongGDL* tri , const DDoubleGDL* xVal , const DDoubleGDL* yVal ,  const T1* zVal ,
  const DLong nx, const DDouble* x , const DLong ny, const DDouble* y) {
    // Loop through all triangles and create properties
    vector <triAccel> triVect;
    for (SizeT triIndex = 0; triIndex < ntri; ++triIndex) {
      triAccel v = triAccel();
      DDouble xmin;
      DDouble xmax;
      DDouble ymin;
      DDouble ymax;
      DDouble x1, y1;
      xmax = xmin = x1 = (*xVal)[(*tri)[3 * triIndex]];
      ymax = ymin = y1 = (*yVal)[(*tri)[3 * triIndex]];
      DDouble x2 = (*xVal)[(*tri)[3 * triIndex + 1]];
      DDouble y2 = (*yVal)[(*tri)[3 * triIndex + 1]];
      v.x3 = (*xVal)[(*tri)[3 * triIndex + 2]];
      v.y3 = (*yVal)[(*tri)[3 * triIndex + 2]];
      v.zc = (*zVal)[(*tri)[3 * triIndex + 2]];
      v.zac = (*zVal)[(*tri)[3 * triIndex]] - v.zc;
      v.zbc = (*zVal)[(*tri)[3 * triIndex + 1]] - v.zc;
      v.y23 = (y2 - v.y3);
      v.x32 = (v.x3 - x2);
      v.y31 = (v.y3 - y1);
      v.x13 = (x1 - v.x3);

      v.denom = ((y2 - v.y3)*(x1 - v.x3) + (v.x3 - x2)*(y1 - v.y3));
      v.denom = 1.0 / v.denom;
      //finish boundingbox
      for (int k = 1; k < 3; ++k) {
        DDouble z = (*xVal)[(*tri)[3 * triIndex + k]];
        xmin = (z < xmin) ? z : xmin;
        xmax = (z > xmax) ? z : xmax;
        z = (*yVal)[(*tri)[3 * triIndex + k]];
        ymin = (z < ymin) ? z : ymin;
        ymax = (z > ymax) ? z : ymax;
      }
      // find first and last index of x and y that will intersect this BBox
      DLong k = 0;
      while (k < (nx - 1) && x[k + 1] < xmin) {
        k++;
      }
      v.pxmin = k;
      while (k < (nx - 1) && x[k + 1] < xmax) {
        k++;
      }
      v.pxmax = k;
      k = 0;
      while (k < (ny - 1) && y[k + 1] < ymin) {
        k++;
      }
      v.pymin = k;
      while (k < (ny - 1) && y[k + 1] < ymax) {
        k++;
      }
      v.pymax = k;

      triVect.push_back(v);
      //      std::cerr<<triIndex<<":"<<v.pxmin<<","<<v.pymin<<"->"<<v.pxmax<<","<<v.pymax<<std::endl;
    }
    
    return triVect;
}

  vector <triAccelCpx> getTriAccelRandomCpx(const DLong ntri,  const DLongGDL* tri , const DDoubleGDL* xVal , const DDoubleGDL* yVal ,  const DComplexDblGDL* zVal ,
  const DLong nx, const DDouble* x , const DLong ny, const DDouble* y) {
    // Loop through all triangles and create properties
    vector <triAccelCpx> triVect;
    for (SizeT triIndex = 0; triIndex < ntri; ++triIndex) {
      triAccelCpx v = triAccelCpx();
      DDouble xmin;
      DDouble xmax;
      DDouble ymin;
      DDouble ymax;
      DDouble x1, y1;
      xmax = xmin = x1 = (*xVal)[(*tri)[3 * triIndex]];
      ymax = ymin = y1 = (*yVal)[(*tri)[3 * triIndex]];
      DDouble x2 = (*xVal)[(*tri)[3 * triIndex + 1]];
      DDouble y2 = (*yVal)[(*tri)[3 * triIndex + 1]];
      v.x3 = (*xVal)[(*tri)[3 * triIndex + 2]];
      v.y3 = (*yVal)[(*tri)[3 * triIndex + 2]];
      v.zc = (*zVal)[(*tri)[3 * triIndex + 2]];
      v.zac =  std::complex<double>( (*zVal)[(*tri)[3 * triIndex]].real() - v.zc.real() , (*zVal)[(*tri)[3 * triIndex]].imag() - v.zc.imag() ) ;
      v.zbc =  std::complex<double>( (*zVal)[(*tri)[3 * triIndex +1]].real() - v.zc.real() , (*zVal)[(*tri)[3 * triIndex +1]].imag() - v.zc.imag() ) ;
      v.y23 = (y2 - v.y3);
      v.x32 = (v.x3 - x2);
      v.y31 = (v.y3 - y1);
      v.x13 = (x1 - v.x3);

      v.denom = ((y2 - v.y3)*(x1 - v.x3) + (v.x3 - x2)*(y1 - v.y3));
      v.denom = 1.0 / v.denom;
      //finish boundingbox
      for (int k = 1; k < 3; ++k) {
        DDouble z = (*xVal)[(*tri)[3 * triIndex + k]];
        xmin = (z < xmin) ? z : xmin;
        xmax = (z > xmax) ? z : xmax;
        z = (*yVal)[(*tri)[3 * triIndex + k]];
        ymin = (z < ymin) ? z : ymin;
        ymax = (z > ymax) ? z : ymax;
      }
      // find first and last index of x and y that will intersect this BBox
      DLong k = 0;
      while (k < (nx - 1) && x[k + 1] < xmin) {
        k++;
      }
      v.pxmin = k;
      while (k < (nx - 1) && x[k + 1] < xmax) {
        k++;
      }
      v.pxmax = k;
      k = 0;
      while (k < (ny - 1) && y[k + 1] < ymin) {
        k++;
      }
      v.pymin = k;
      while (k < (ny - 1) && y[k + 1] < ymax) {
        k++;
      }
      v.pymax = k;

      triVect.push_back(v);
      //      std::cerr<<triIndex<<":"<<v.pxmin<<","<<v.pymin<<"->"<<v.pxmax<<","<<v.pymax<<std::endl;
    }

    return triVect;
  }
  
  template< typename T1, typename T2>
  void gdlGrid2DData(DLong nx, DDouble* x, DLong xref, DDouble xval, DDouble xinc, DLong ny, DDouble* y, DLong yref, DDouble yval, DDouble yinc, DLong ntri, DLongGDL* tri, 
      DDoubleGDL* xVal, DDoubleGDL* yVal, T1* zVal, bool domaxvalue, 
      bool dominvalue, T2 maxVal, T2 minVal, T1* res, bool input) {
    
    vector <triAccel> triVect = getTriAccelUniform(ntri, tri, xVal, yVal, zVal, nx, xref, xval, xinc, ny, yref, yval, yinc);

    if (dominvalue || domaxvalue) {
      for (std::vector<triAccel>::iterator triIndex = triVect.begin(); triIndex != triVect.end(); ++triIndex) {
        for (SizeT iy = (*triIndex).pymin; iy <= (*triIndex).pymax; ++iy) {
          DDouble yy = (y[iy] - (*triIndex).y3);
          DLong start = iy*ny;
          for (SizeT ix = (*triIndex).pxmin; ix <= (*triIndex).pxmax; ++ix) {
            DDouble xx = (x[ix] - (*triIndex).x3);
            barycentricCoordMinMax((*triIndex), xx, yy, (*res)[start + ix], domaxvalue, dominvalue, maxVal, minVal) ;
          } //ix loop
        } // iy
      } //tri
      return;
    } else {
      for (std::vector<triAccel>::iterator triIndex = triVect.begin(); triIndex != triVect.end(); ++triIndex) {
        for (SizeT iy = (*triIndex).pymin; iy <= (*triIndex).pymax; ++iy) {
          DDouble yy = (y[iy] - (*triIndex).y3);
          DLong start = iy*ny;
          for (SizeT ix = (*triIndex).pxmin; ix <= (*triIndex).pxmax; ++ix) {
            DDouble xx = (x[ix] - (*triIndex).x3);
            barycentricCoord((*triIndex), xx, yy, (*res)[start + ix]);
          } //ix loop
        } // iy
      } //tri
      return;
    }
  }

//version for non-uniform grids (waaaay slower)
template< typename T1, typename T2>
  void gdlGrid2DData(DLong nx, DDouble* x, DLong ny, DDouble* y, DLong ntri, DLongGDL* tri, 
      DDoubleGDL* xVal, DDoubleGDL* yVal, T1* zVal, bool domaxvalue, 
      bool dominvalue, T2 maxVal, T2 minVal, T1* res, bool input) {
    
      // Loop through all triangles and create properties
    vector <triAccel> triVect = getTriAccelRandom(ntri, tri, xVal, yVal, zVal, nx, x, ny, y);

    //loop on triangles:
    if (dominvalue || domaxvalue) {
      for (std::vector<triAccel>::iterator triIndex = triVect.begin(); triIndex != triVect.end(); ++triIndex) {
        for (SizeT iy = (*triIndex).pymin; iy <= (*triIndex).pymax; ++iy) {
          DDouble yy = (y[iy] - (*triIndex).y3);
          DLong start = iy*ny;
          for (SizeT ix = (*triIndex).pxmin; ix <= (*triIndex).pxmax; ++ix) {
            DDouble xx = (x[ix] - (*triIndex).x3);
            barycentricCoordMinMax((*triIndex), xx, yy, (*res)[start + ix], domaxvalue, dominvalue, maxVal, minVal) ;
          } //ix loop
        } // iy
      }//tri
      return;
    } else {
      //loop on triangles:
      for (std::vector<triAccel>::iterator triIndex = triVect.begin(); triIndex != triVect.end(); ++triIndex) {
        for (SizeT iy = (*triIndex).pymin; iy <= (*triIndex).pymax; ++iy) {
          DDouble yy = (y[iy] - (*triIndex).y3);
          DLong start = iy*ny;
          for (SizeT ix = (*triIndex).pxmin; ix <= (*triIndex).pxmax; ++ix) {
            DDouble xx = (x[ix] - (*triIndex).x3);
            barycentricCoord((*triIndex), xx, yy, (*res)[start + ix]);
          } //ix loop
        } // iy
      }//tri
      return;
    }
  }


//for complex values 
  void gdlGrid2DDataCpx(DLong nx, DDouble* x, DLong xref, DDouble xval, DDouble xinc, DLong ny, DDouble* y, DLong yref, DDouble yval, DDouble yinc, DLong ntri, DLongGDL* tri,
    DDoubleGDL* xVal, DDoubleGDL* yVal, DComplexDblGDL* zVal, bool domaxvalue,
    bool dominvalue, DComplexDbl maxVal, DComplexDbl minVal, DComplexDblGDL* res, bool input) {

    vector <triAccelCpx> triVect = getTriAccelUniformCpx(ntri, tri, xVal, yVal, zVal, nx, xref, xval, xinc, ny, yref, yval, yinc);

    if (dominvalue || domaxvalue) {
      for (std::vector<triAccelCpx>::iterator triIndex = triVect.begin(); triIndex != triVect.end(); ++triIndex) {
        for (SizeT iy = (*triIndex).pymin; iy <= (*triIndex).pymax; ++iy) {
          DDouble yy = (y[iy] - (*triIndex).y3);
          DLong start = iy*ny;
          for (SizeT ix = (*triIndex).pxmin; ix <= (*triIndex).pxmax; ++ix) {
            DDouble xx = (x[ix] - (*triIndex).x3);
            barycentricCoordMinMaxCpx((*triIndex), xx, yy, (*res)[start + ix], domaxvalue, dominvalue, maxVal, minVal);
          } //ix loop
        } // iy
      } //tri
      return;
    } else {
      for (std::vector<triAccelCpx>::iterator triIndex = triVect.begin(); triIndex != triVect.end(); ++triIndex) {
        for (SizeT iy = (*triIndex).pymin; iy <= (*triIndex).pymax; ++iy) {
          DDouble yy = (y[iy] - (*triIndex).y3);
          DLong start = iy*ny;
          for (SizeT ix = (*triIndex).pxmin; ix <= (*triIndex).pxmax; ++ix) {
            DDouble xx = (x[ix] - (*triIndex).x3);
            barycentricCoordCpx((*triIndex), xx, yy, (*res)[start + ix]);
          } //ix loop
        } // iy
      } //tri
      return;
    }
  }

  void gdlGrid2DDataCpx(DLong nx, DDouble* x, DLong ny, DDouble* y, DLong ntri, DLongGDL* tri,
    DDoubleGDL* xVal, DDoubleGDL* yVal, DComplexDblGDL* zVal, bool domaxvalue,
    bool dominvalue, DComplexDbl maxVal, DComplexDbl minVal, DComplexDblGDL* res, bool input) {

    // Loop through all triangles and create properties
    vector <triAccelCpx> triVect = getTriAccelRandomCpx(ntri, tri, xVal, yVal, zVal, nx, x, ny, y);

    //loop on triangles:
    if (dominvalue || domaxvalue) {
      for (std::vector<triAccelCpx>::iterator triIndex = triVect.begin(); triIndex != triVect.end(); ++triIndex) {
        for (SizeT iy = (*triIndex).pymin; iy <= (*triIndex).pymax; ++iy) {
          DDouble yy = (y[iy] - (*triIndex).y3);
          DLong start = iy*ny;
          for (SizeT ix = (*triIndex).pxmin; ix <= (*triIndex).pxmax; ++ix) {
            DDouble xx = (x[ix] - (*triIndex).x3);
            barycentricCoordMinMaxCpx((*triIndex), xx, yy, (*res)[start + ix], domaxvalue, dominvalue, maxVal, minVal);
          } //ix loop
        } // iy
      }//tri
      return;
    } else {
      //loop on triangles:
      for (std::vector<triAccelCpx>::iterator triIndex = triVect.begin(); triIndex != triVect.end(); ++triIndex) {
        for (SizeT iy = (*triIndex).pymin; iy <= (*triIndex).pymax; ++iy) {
          DDouble yy = (y[iy] - (*triIndex).y3);
          DLong start = iy*ny;
          for (SizeT ix = (*triIndex).pxmin; ix <= (*triIndex).pxmax; ++ix) {
            DDouble xx = (x[ix] - (*triIndex).x3);
            barycentricCoordCpx((*triIndex), xx, yy, (*res)[start + ix]);
          } //ix loop
        } // iy
      }//tri
      return;
    }
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
    //Double output is not only defined by z but also by x and y.
    if (p2->Type()==GDL_DOUBLE || p2->Type()==GDL_COMPLEXDBL) type=GDL_DOUBLE;
    else if (p1->Type()==GDL_DOUBLE || p1->Type()==GDL_COMPLEXDBL) type=GDL_DOUBLE;
    else if (p0->Type()==GDL_DOUBLE || p0->Type()==GDL_COMPLEXDBL) type=GDL_DOUBLE;
    
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

    static int inputIx=e->KeywordIx( "INPUT");
    bool doInput=(e->KeywordPresent(inputIx));
    
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
      nx = (DLong) ceil(xrange / xinc) +1;
    }
    if (GS != NULL && canUseLimitsy)
    {
      yinc = (*GS)[1];
      ny = (DLong) ceil(yrange / yinc) +1;
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

    // if INPUT kw, test if input is an existing variable
    BaseGDL* inputKwData=NULL;
    DDoubleGDL* inputArray=NULL;
    bool inputArrayPresent=false;
    if (doInput) {
      inputKwData=e->GetKW(inputIx);
      if (!(inputKwData==NULL)) { //does exist, check compatibility 
        if (inputKwData->Dim() != dim) e->Throw("Array has a corrupted descriptor");
        inputArrayPresent=true;
      }
    }

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
      bool doMiss=e->KeywordPresent(missvalueIx);
      if (doMiss) missValG=e->GetKWAs<DComplexDblGDL>(missvalueIx);
      DComplexDblGDL* zVal = static_cast<DComplexDblGDL*>(p2->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
      DComplexDblGDL* res;
      if (inputArrayPresent) res=static_cast<DComplexDblGDL*>(inputKwData->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
      else if (doMiss) {
        res = missValG->New(dim, BaseGDL::INIT);
      } else {
        res= new DComplexDblGDL(dim, BaseGDL::ZERO);
      }

      DComplexDbl minVal=std::complex<double>(0,0);
      if (minValG!=NULL) minVal=std::complex<double>((*minValG)[0].real(),(*minValG)[0].imag());
      DComplexDbl maxVal=std::complex<double>(0,0);
      if (maxValG!=NULL) maxVal=std::complex<double>((*maxValG)[0].real(),(*maxValG)[0].imag());
      if (doXout) gdlGrid2DDataCpx(nx, x, ny, y, ntri, tri, xVal, yVal, zVal, domaxvalue, dominvalue, maxVal, minVal, res, inputArrayPresent);
      else gdlGrid2DDataCpx(nx, x, xref,xval,xinc, ny, y, yref,yval,yinc,ntri, tri, xVal, yVal, zVal, domaxvalue, dominvalue, maxVal, minVal, res, inputArrayPresent);
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
      DDoubleGDL* missVal=NULL;
      static int missvalueIx=e->KeywordIx( "MISSING");
      bool doMiss=e->KeywordPresent(missvalueIx);
      if (doMiss) missVal=e->GetKWAs<DDoubleGDL>(missvalueIx);
      DDoubleGDL* zVal = static_cast<DDoubleGDL*>(p2->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      DDoubleGDL* res;
      if (inputArrayPresent) {
        res = static_cast<DDoubleGDL*> (inputKwData->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      } else if (doMiss) {
        res = missVal->New(dim, BaseGDL::INIT);
      } else {
        res = new DDoubleGDL(dim, BaseGDL::ZERO);
      }
      if (doXout) gdlGrid2DData< DDoubleGDL, DDouble>(nx, x, ny, y, ntri, tri, xVal, yVal, zVal, domaxvalue, dominvalue, maxVal, minVal, res, inputArrayPresent);
      else gdlGrid2DData< DDoubleGDL, DDouble>(nx, x, xref,xval,xinc, ny, y, yref,yval,yinc,ntri, tri, xVal, yVal, zVal, domaxvalue, dominvalue, maxVal, minVal, res, inputArrayPresent);
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

//to be written and do not forget to uncomment QHULL in CMakeLists and config.h.cmake  
  // see http://www.geom.umn.edu/software/qhull/. Used also with plplot.
#ifdef HAVE_QHULL
  void qhull ( EnvT* e)
  {
    e->Throw("Please Write this function in GDL.");
  }


  BaseGDL* qgrid3_fun ( EnvT* e)
  {
    e->Throw("Please Write this function in GDL.");
    return NULL;
  }
#endif  
  }

