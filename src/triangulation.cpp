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
    bool reorderFvalue=(e->KeywordPresent(fvalueIx));
    if (!isSphere) {
      doDegree=false;
      reorderFvalue=false;
    }
    bool wantsDupes=false;
    static int dupesIx=e->KeywordIx( "REPEATS");
    if( e->KeywordPresent( dupesIx)) wantsDupes=true;
    
    bool wantsConnectivity=false;
    static int connIx=e->KeywordIx( "CONNECTIVITY");
    if( e->KeywordPresent( connIx)) wantsConnectivity=true;
    
    //check since we return values in them 
    if (isSphere)  e->AssureGlobalKW(sphereIx); 
    if (reorderFvalue)  e->AssureGlobalKW(fvalueIx);
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
    if (reorderFvalue) {
      fvalue=e->GetKWAs<DDoubleGDL>(fvalueIx);
      if (fvalue->Rank() == 0) e->Throw("Expression must be an array in this context: " +  e->GetString( fvalueIx)+".");
      if (fvalue->N_Elements() != npts) e->Throw("X & Y arrays must have same number of points."); //yes yes.
    }
    //compute default tol
    DLong maxEl, minEl;
    DDouble maxVal;
    xVal->MinMax(&minEl,&maxEl,NULL,NULL,false);
    maxVal=(*xVal)[maxEl];
    yVal->MinMax(&minEl,&maxEl,NULL,NULL,false);
    maxVal=abs(max(maxVal,(*yVal)[maxEl])); //maximum ABSOLUTE VALUE
    
    //default value for dtol
    DDouble dtol = isDouble ? 1e-12 : 1e-6;
    if (maxVal>0.0) dtol*=maxVal; //we may have maxval=0 exactly...
    DDouble naturalTol=dtol; //an internal 'standard' tolerance that can be used in cas of identical points...
    //use passed value if any
    DDouble tol=dtol;
    static int tolIx = e->KeywordIx("TOLERANCE");
    if (e->KeywordPresent(tolIx)) {
      e->AssureDoubleScalarKW(tolIx, tol);
      if (tol > 0.0) dtol = tol;
    }
    
    //Renka's algorithm imposes the 3 first points to be uncolinear. We have to deal with this.
    DDouble* xx=&(*xVal)[0];
    DDouble* yy=&(*yVal)[0];    

    bool exchangeBack=false;
    bool colinear = false;
    DLong Offset = 0;

    // Insure first triangle is not colinear. Skip as many colinear points as necessary to start with something not colinear by inverting the 3rd point
    // with the first point making a non-colinear triangle.
    // I write this because TRIPACK has no notion of 'tol', and IDL has a very strange notion of 'tol': it will easily complain that points are colinear
    // when they are obviously not, when tol is relatively high. I suspect a bug. Anyway, the following is an attempt to define a colinarity within 'tol'.
    // given at least 2 points, find a regression line, do the next point lie > tol from it, and the next, etc?
    colinear=true;
    DDouble sx=xx[0];
    DDouble sx2=xx[0]*xx[0];
    DDouble sy=yy[0];
    DDouble sxy=xx[0]*yy[0];
    DDouble a,b;
    Offset=1;
    while (colinear && Offset < (npts-1) ) {
      DLong n=Offset+1;
    //compute best line 
      sx += xx[Offset];
      sx2 += (xx[Offset] * xx[Offset]);
      sy += yy[Offset];
      sxy += xx[Offset] * yy[Offset];
      // linear coeff of y=a*x+b
      b = (n * sxy - (sx * sy)) / (n * sx2 - (sx * sx) );
      a = (sy - b * sx) / n;
      // distance of next point wrt previous regression line: (a+b*x-y)/sqrt(1+b^2)
      Offset++;
      DDouble dist=(a+b*xx[Offset]-yy[Offset])/sqrt(1+(b*b));
//      std::cerr<<a<<","<<b<<":"<<abs(dist)<<":"<<dtol<<std::endl;
      if (abs(dist) > dtol ) colinear = false;
    }
//    std::cerr<<Offset<<std::endl;
    if (Offset > 2 ) exchangeBack=true;
    
    if (colinear) e->Throw("Points are co-linear, no solution.");
    //if exchange 2 and Offset:
    if (exchangeBack) {
      DDouble tmp = xx[2];
      xx[2] = xx[Offset];
      xx[Offset] = tmp;
      tmp = yy[2];
      yy[2] = yy[Offset];
      yy[Offset] = tmp;
    }

    //for duplicates
    std::vector<std::pair<DLong,DLong>> dupes;
    //TRIPACK has its own way to say 2 points are identical. 

    
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
      DLong* near__=(DLong*)malloc(npts*sizeof(DLong)); //"near" is reserved in Windows C.
      DLong* next=(DLong*)malloc(npts*sizeof(DLong));
      DDouble* dist=(DDouble*)malloc(npts*sizeof(DDouble));
      DLong ier=0;
      DLong lnew=0;
      DLong l_npts=npts;
      DLong* originalIndex=(DLong*)malloc(npts*sizeof(DLong));
      for (DLong i = 0; i < npts; ++i) originalIndex[i]=i;
      if (exchangeBack) originalIndex[2]=Offset;
      if (exchangeBack) originalIndex[Offset]=2;

      DLong ret1=tripack::trmesh_(&l_npts, xx , yy, list, lptr, lend, &lnew, near__, next, dist, &ier);
      // At this point, only positive ier are expected (duplicates).
      if (ier!=0) {
        if (ier > 0) { //duplicate nodes exist. We have to remove all of them and keep track of their index
            //we will work on a copy. To hell with speed, this is not optimized.
          DDouble* l_xx=(DDouble*)malloc(l_npts*sizeof(DDouble));
          DDouble* l_yy=(DDouble*)malloc(l_npts*sizeof(DDouble));
          memcpy(l_xx,&(*xVal)[0],l_npts*sizeof(DDouble));
          memcpy(l_yy,&(*yVal)[0],l_npts*sizeof(DDouble));
          // if ndupes > 0 we have to take into account that several returned indexes in the list have been changed wrt the original because of the list shortening
          // by elimination of the duplicates. So we have to create a table of correspondence.
          while (ier > 0) { 
            ier--;
            DLong m = 0;
            for (DLong i = 0; i < lnew -1 ; ++i) if (list[i] > m) m = list[i]; //simple way to compute where we are
            dupes.push_back(make_pair(ier, originalIndex[m])); // value at index 'm' is identical to the one at 'ier'
            //remove node "m" and add +1 to originalIndex starting at 'm'
            for (DLong i=m; i< l_npts-1; ++i) {l_xx[i]=l_xx[i+1]; l_yy[i]=l_yy[i+1];originalIndex[i]=originalIndex[i+1];}
            l_npts--;
            ret1 = tripack::trmesh_(&l_npts, l_xx, l_yy, list, lptr, lend, &lnew, near__, next, dist, &ier);
            if (ier < 0)  e->Throw("Internal Error, please report."); //Should not happen
            }
          free (l_xx);
          free (l_yy);
        } 
        else if (ier == -2)  e->Throw("Points are co-linear, no solution."); //Should not happen
        else if (ier == -1)  e->Throw("Not enough valid and unique points specified."); //ier==-1 impossible (npoints < 3)
        //ier==-4 internal error should be reported! -- never happened in 40 years
          else if (ier == -4) e->Throw("Congratulations, you found the impossible: a set of points that triggers an internal error in TRMESH, please report!");
          else e->Throw("Congratulations, you found an impossible error code of the TRIPACK package.");
      }
      DLong ndupes=dupes.size();
      free(near__);
      free(next);
      free(dist); 
      
      //Dupes indexing is the original one.
      if (wantsDupes) {
        if (dupes.size() ==0) {
          DLongGDL* nothing=new DLongGDL(dimension(2),BaseGDL::ZERO); 
          nothing->Dec();
          e->SetKW(dupesIx, nothing);
        }
        else {
          DLongGDL* returned_dupes = new DLongGDL(dimension(2,dupes.size()), BaseGDL::NOZERO);
          for (DLong i = 0; i < dupes.size(); ++i) {
            (*returned_dupes)[2*i+0]=dupes[i].first;
            (*returned_dupes)[2*i+1]=dupes[i].second;
          } 
          e->SetKW(dupesIx, returned_dupes);
        }
      }
      
      //All indexes in the returned arrays must be replaced by those of originalIndex.

      if (wantsTriangles) {
        //convert to triangle list
        DLong noconstraints = 0;
        DLong dummylistofconstraints = 0;
        DLong nrow = 6; //no arcs.
        DLong* ltri = (DLong*) malloc((12 * l_npts) * sizeof (DLong));
        DLong lct = 0;
        DLong ntriangles = 0;
        DLong ret2 = tripack::trlist_(&noconstraints, &dummylistofconstraints, &l_npts, list, lptr, lend, &nrow, &ntriangles, ltri, &lct, &ier);
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
        for (DLong j = 0; j < ntriangles; ++j)
        {
          for (int i = 0; i < 3; ++i)
          {
            (*returned_triangles)[3 * j + i] = originalIndex[ltri[6 * j + i] - 1]; //our nubering starts a 0.
          }
        }
        free(ltri);
        //pass back to GDL env:
        e->SetPar(2, returned_triangles);
      }
      if (wantsEdge) {
        DLong* nodes=(DLong*)malloc(l_npts*sizeof(DLong));
        DLong nb=0;
        DLong na=0;
        DLong nt=0;
        DLong ret3=tripack::bnodes_(&l_npts, list, lptr, lend, nodes, &nb, &na, &nt);
        DLongGDL* returned_edges = new DLongGDL(nb, BaseGDL::NOZERO);
        for (DLong j = 0; j < nb; ++j) (*returned_edges)[j]=originalIndex[nodes[j]-1];
        free(nodes);
        e->SetPar(3, returned_edges);
      }
      if (wantsConnectivity) {
        //remove 1 to get C array indexes.
        for (DLong i = 0; i < lnew-1; ++i) lptr[i]--; 
        for (DLong i = 0; i < l_npts; ++i) lend[i]--;
        //in connectivity we MUST have all the points, even the duplicated ones. But connectivity of duplicated points will break IDL.
        // we could avoid this easily as I believe this is an IDL bug. We just have to reproduce the connectivity of the first encounter
        // of the duplicated point.
        DLong* array=(DLong*)malloc((npts*npts+npts+1)*sizeof(DLong)); // size > max possible connectivity 
        DLong runningindex = npts+1; // and not l_npts: we report for all points 
        DLong startindex=0;
        array[startindex++]= npts+1;
        DLong* effective_index=(DLong*)malloc(npts*sizeof(DLong));//this is the list of npts vertexes for which we want the neighbour. It must be
        // the  list of l_npts (returned) indexes, with indexes of the first instance of duplicated points for the duplicated points.
        if (ndupes == 0) for (DLong i=0; i< npts; ++i) effective_index[i]=i; //easy
        else { //piecewise index construction, each time dupes.second is encountered, insert dupes.first instead of increasing index:
          DLong i=0; DLong k=0; //i:index from 0 to l_npts-1., k running index of 0 to npts-1
          for (DLong idup=0; idup< ndupes; ++idup) {
            DLong encounter=dupes[idup].second;
            while (k<encounter) effective_index[k++]=i++;
            effective_index[k++]=dupes[idup].first;
          }
          while (k<npts) effective_index[k++]=i++;
        }
        for (DLong k = 0; k < npts; ++k) { 
          DLong i=effective_index[k]; 
          //is it an exterior point? Yes if the termination of the connectivity list is exterior.
          DLong lpl=lend[i];
          if (list[lpl]<0) array[runningindex++]=originalIndex[i]; //exterior - we write it
          //write all points until nfin=lend[i] is found again using lptr connectivity pointers:
          DLong lp=lpl; //see nbcnt_()
          do {
            lp=lptr[lp];
            array[runningindex]=(list[lp]>0)?list[lp]-1:(-list[lp])-1;
            array[runningindex]=originalIndex[array[runningindex]];
            runningindex++;
          } while (lp!=lpl);
          array[startindex++]=runningindex;
        }
        DLongGDL* connections = new DLongGDL(runningindex, BaseGDL::NOZERO);
        for (DLong i = 0; i < runningindex; ++i) (*connections)[i]=array[i];
        e->SetKW(connIx,connections);
        free(array);
      }
      //cleanup
      free(list);
      free(lptr);
      free(lend);
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

  template< typename T1, typename T2>
  void gdlGrid2DData(DLong nx, DDouble* x, DLong ny, DDouble* y, DLong ntri, DLongGDL* tri, 
      DDoubleGDL* xVal, DDoubleGDL* yVal, T1* zVal, bool domaxvalue, 
      bool dominvalue, T2 maxVal, T2 minVal, T2 missVal, T1* res, bool input) {
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
              if ((dominvalue && dres < minVal) || (domaxvalue && dres > maxVal))
              {
                if (!input) (*res)[iy * nx + ix] = missVal;
              } else
              {
                (*res)[iy * nx + ix] = dres;
              }
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
//version for Complex Values.
  template<>
  void gdlGrid2DData(DLong nx, DDouble* x, DLong ny, DDouble* y, DLong ntri, 
    DLongGDL* tri, DDoubleGDL* xVal, DDoubleGDL* yVal, 
    DComplexDblGDL* zVal, bool domaxvalue, bool dominvalue, DComplexDbl maxVal,
    DComplexDbl minVal, DComplexDbl missVal, DComplexDblGDL* res, bool input) {
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
              if ((dominvalue && dres < minVal.real()) || (domaxvalue && dres > maxVal.real()))
              {
                if (!input)
                {
                  (*res)[iy * nx + ix].real(missVal.real());
                } else
                {
                  ((*res)[iy * nx + ix]).real(dres);
                }
              }
              dres = Ai + Bi * x[ix] + Ci * y[iy];
              if ((dominvalue && dres < minVal.imag()) || (domaxvalue && dres > maxVal.imag()))
              {
                if (!input)
                {
                  (*res)[iy * nx + ix].imag(missVal.imag());
                } else
                {
                  ((*res)[iy * nx + ix]).imag(dres);
                }
              }
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
              (*res)[iy * nx + ix].real(Ar + Br * x[ix] + Cr * y[iy]);
              (*res)[iy * nx + ix].imag(Ai + Bi * x[ix] + Ci * y[iy]);
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
      if (e->KeywordPresent(missvalueIx)) missValG=e->GetKWAs<DComplexDblGDL>(missvalueIx);
      DComplexDblGDL* zVal = static_cast<DComplexDblGDL*>(p2->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
      DComplexDblGDL* res;
      if (inputArrayPresent) res=static_cast<DComplexDblGDL*>(inputKwData->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
      else res= new DComplexDblGDL(dim, BaseGDL::ZERO);

      DComplexDbl minVal=std::complex<double>(0,0);
      if (minValG!=NULL) minVal=std::complex<double>((*minValG)[0].real(),(*minValG)[0].imag());
      DComplexDbl maxVal=std::complex<double>(0,0);
      if (maxValG!=NULL) maxVal=std::complex<double>((*maxValG)[0].real(),(*maxValG)[0].imag());
      DComplexDbl missVal=std::complex<double>(0,0);
      if (missValG!=NULL) missVal=std::complex<double>((*missValG)[0].real(),(*missValG)[0].imag());
      gdlGrid2DData< DComplexDblGDL, DComplexDbl>(nx, x, ny, y, ntri, tri, xVal, yVal, zVal, domaxvalue, dominvalue, maxVal, minVal, missVal, res, inputArrayPresent);
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
      DDoubleGDL* res;
      if (inputArrayPresent) res = res=static_cast<DDoubleGDL*>(inputKwData->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      else res =new DDoubleGDL(dim, BaseGDL::ZERO);
      gdlGrid2DData< DDoubleGDL, DDouble>(nx, x, ny, y, ntri, tri, xVal, yVal, zVal, domaxvalue, dominvalue, maxVal, minVal, missVal, res, inputArrayPresent);
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

