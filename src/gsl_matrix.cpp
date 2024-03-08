/***************************************************************************
                          gsl_matrix.cpp  -  GSL GDL library function
                             -------------------
    begin                : Dec 9 2011
    copyright            : (C) 2011 by Alain Coulais
    email                : alaingdl@users.sourceforge.net
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

#include <map>
#include <cmath>

#include "envt.hpp"
#include "basic_fun.hpp"
#include "gsl_fun.hpp"
#include "dinterpreter.hpp"

#include <gsl/gsl_sys.h>
#include <gsl/gsl_linalg.h>

// constant
#include <gsl/gsl_math.h>

//#define LOG10E 0.434294

namespace lib {

  using namespace std;

  const int szdbl=sizeof(DDouble);
  const int szflt=sizeof(DFloat);
  const int szlng=sizeof(DLong);
  const int szlng64=sizeof(DLong64);

  void ludc_pro( EnvT* e)
  {
    //  cout << szdbl << " " <<szflt << " " << szlng << " " szlng64 << endl;

    SizeT nParam=e->NParam(1);

    BaseGDL* p0 = e->GetParDefined( 0);
    
    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( "Variable is undefined: " + e->GetParString(0));
  
    if (p0->Rank() > 2)
      e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    
    if (p0->Rank() > 1) {
      if (p0->Dim(0) != p0->Dim(1))
        e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    }

    // status 
    // check here, if not done, res would be pending in case of SetPar() throws
    // SetPar() only throws in AssureGlobalPar()
    if (nParam == 2) e->AssureGlobalPar( 1);

    // only one element matrix

    if (p0->Type() == GDL_COMPLEXDBL || p0->Type() == GDL_COMPLEX){
      e->Throw( "Input type cannot be COMPLEX, please use LA_LUDC (not ready)");
    }
    
    //DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
    DDoubleGDL *p0D = e->GetParAs<DDoubleGDL>(0);

    gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(0));
    GDLGuard<gsl_matrix> g1(mat,gsl_matrix_free);
   
    memcpy(mat->data, &(*p0D)[0], nEl*szdbl);

    gsl_permutation * p = gsl_permutation_alloc (p0->Dim(0));
    GDLGuard<gsl_permutation> g2( p, gsl_permutation_free);
    int s;
    gsl_linalg_LU_decomp (mat, p, &s);

    int debug=0;
    if (debug) {
      cout << "permutation order: " << s << endl;
      cout << "permutation vector:"<< endl;
      gsl_permutation_fprintf (stdout, p, " %u");
      cout << endl;
    }
    
    // copying over p0 the updated matrix	
    SizeT dims[2] = {p0->Dim(0), p0->Dim(0)};
    dimension dim0(dims, (SizeT) 2);

    BaseGDL** p0Do = &e->GetPar( 0);
    GDLDelete((*p0Do));
    *p0Do = new DDoubleGDL(dim0, BaseGDL::NOZERO);  
    memcpy(&(*(DDoubleGDL*) *p0Do)[0], mat->data,
	   p0->Dim(0)*p0->Dim(0)*szdbl);

    int double_flag=0;
    if (p0->Type() == GDL_DOUBLE) double_flag=1;
    static int doubleIx=e->KeywordIx("DOUBLE");
    if (e->KeywordSet(doubleIx)) double_flag=1;
    static int INTERCHANGES=e->KeywordIx("INTERCHANGES");
    if (e->KeywordPresent(  INTERCHANGES)) {
	  e->AssureGlobalKW(INTERCHANGES);
	  e->SetKW(INTERCHANGES,new DLongGDL(s));
    }
 
    // this code will always return GDL_DOUBLE because I don't know how to do :(
    // AC 13-Feb-2012 : this is not working and I don't know how to do :(
    // if (double_flag == 0)
    // { p0->Convert2(GDL_FLOAT, BaseGDL::CONVERT); }

    // copying over p1 the permutation vector
    SizeT n = p0->Dim(0);
    dimension dim1(&n, (SizeT) 1);
    BaseGDL** p1D = &e->GetPar( 1);
    GDLDelete((*p1D));
    if (sizeof(size_t) == szlng) {
      *p1D = new DLongGDL(dim1, BaseGDL::NOZERO);
      memcpy(&(*(DLongGDL*) *p1D)[0], p->data, 
	p0->Dim(0)*szlng);
    } else {
      *p1D = new DLong64GDL(dim1, BaseGDL::NOZERO);
      memcpy(&(*(DLong64GDL*) *p1D)[0], p->data, 
	p0->Dim(0)*szlng64);
    }
    
//     gsl_matrix_free(mat);
//     gsl_permutation_free(p);
  
  }
  
  BaseGDL* lusol_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
//    int s;
    
    //     if( nParam == 0)
    //       e->Throw( "Incorrect number of arguments.");

    // managing first input: Square Matrix

    BaseGDL* p0 = e->GetParDefined( 0);
    
    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( "Variable is undefined: " + e->GetParString(0));
  
    if (p0->Rank() > 2)
      e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    
    if (p0->Rank() > 1) {
      if (p0->Dim(0) != p0->Dim(1))
        e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    }

    // status 
    // check here, if not done, res would be pending in case of SetPar() throws
    // SetPar() only throws in AssureGlobalPar()
    if (nParam == 2) e->AssureGlobalPar( 1);


    // managing seconf input: Vector
    BaseGDL* p1 = e->GetParDefined(1);
    
    SizeT nEl1 = p1->N_Elements();
    if( nEl1 == 0)
      e->Throw( "Variable is undefined: " + e->GetParString(1));
    
    if (p1->Rank() > 2)
      e->Throw( "Input must be a Vector:" + e->GetParString(1));
    

    // managing third input: Vector
    BaseGDL* p2 = e->GetParDefined(2);
    
    SizeT nEl2 = p2->N_Elements();
    if( nEl2 == 0)
      e->Throw( "Variable is undefined: " + e->GetParString(2));
    
    if (p2->Rank() > 2)
      e->Throw( "Input must be a Vector:" + e->GetParString(2));
    
    if (p0->Type() == GDL_COMPLEXDBL || p0->Type() == GDL_COMPLEX){
      e->Throw( "Input type cannot be COMPLEX, please use LA_LUDC (not ready)");
    }
  
    DDoubleGDL *p0D = e->GetParAs<DDoubleGDL>(0);
    gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(0));
    GDLGuard<gsl_matrix> g1(mat,gsl_matrix_free);
    memcpy(mat->data, &(*p0D)[0], nEl*szdbl);

    gsl_permutation *p = gsl_permutation_alloc (nEl1);
    GDLGuard<gsl_permutation> g2(p,gsl_permutation_free);
    if (sizeof(size_t) == szlng) {
      DLongGDL* p1L =e->GetParAs<DLongGDL>(1);
      memcpy(p->data, &(*p1L)[0], nEl1*szlng);
    } else {
      DLong64GDL* p1L =e->GetParAs<DLong64GDL>(1);
      memcpy(p->data, &(*p1L)[0], nEl1*szlng64);
    }
      
    DDoubleGDL *p2D = e->GetParAs<DDoubleGDL>(2);
    gsl_vector *b = gsl_vector_alloc(nEl2);
    GDLGuard<gsl_vector> g3(b,gsl_vector_free); // b was NOT freed before   
    memcpy(b->data, &(*p2D)[0], nEl1*szdbl);
    
    gsl_vector *x = gsl_vector_alloc(nEl2);
    GDLGuard<gsl_vector> g4(x,gsl_vector_free);    
      
    // computation by GSL
    gsl_linalg_LU_solve (mat, p, b, x);
    
    int debug=0;
    if (debug) {
//      cout << "permutation order: " << s << endl;
      cout << "permutation vector:";
      gsl_permutation_fprintf (stdout, p, " %u");
      cout << endl;
      cout << "input vector:";
      gsl_vector_fprintf (stdout, b, " %g");
      cout << endl;
      cout << "result vector:";
      gsl_vector_fprintf (stdout, x, " %g");
      cout << endl;
    }
      
    DDoubleGDL* res = new DDoubleGDL( p2->Dim(), BaseGDL::NOZERO);
    memcpy(&(*res)[0], x->data, nEl1*szdbl);
	
//     gsl_matrix_free(mat);Parameter
//     gsl_vector_free(x);
//     gsl_permutation_free(p);
//     b ???    

    int double_flag=0;
    if (p0->Type() == GDL_DOUBLE || p2->Type() == GDL_DOUBLE) double_flag=1;
    static int doubleIx=e->KeywordIx("DOUBLE");
    if (e->KeywordSet(doubleIx)) double_flag=1;

    if (double_flag)
      {	return res; }
    else
      { return res->Convert2(GDL_FLOAT, BaseGDL::CONVERT); }
  }

  BaseGDL* determ_fun( EnvT* e) {
    
    // managing first input: Square Matrix
    
    BaseGDL* p0 = e->GetParDefined( 0);
    
    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( "Variable is undefined: " + e->GetParString(0));
  
    if (p0->Rank() > 2)
      e->Throw( "Input must be a square matrix:" + e->GetParString(0));

    if (p0->Rank() > 1) {
      if (p0->Dim(0) != p0->Dim(1))
        e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    }
    
    if (p0->Type() == GDL_COMPLEXDBL || p0->Type() == GDL_COMPLEX){
      e->Throw( "Input type cannot be COMPLEX, please use LA_DETERM (not ready)");
    }
    
    DDoubleGDL *p0D = e->GetParAs<DDoubleGDL>(0);
    gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(0));
    GDLGuard<gsl_matrix> g1(mat,gsl_matrix_free);
    memcpy(mat->data, &(*p0D)[0], nEl*szdbl);
      
    gsl_permutation *p = gsl_permutation_alloc(p0->Dim(0));
    GDLGuard<gsl_permutation> g2(p,gsl_permutation_free);

    int sign;
    double determ=0.0;
   
    // computation by GSL
    gsl_linalg_LU_decomp (mat, p, &sign);
    determ=gsl_linalg_LU_det (mat, sign);

    int debug=0;
    if (debug) {
      cout << "Determ : " << determ << endl;
    }

//     gsl_matrix_free(mat);
//     gsl_permutation_free(p);

    DDoubleGDL* res = new DDoubleGDL(1, BaseGDL::NOZERO);
    (*res)[0]=determ;

    int double_flag=0;
    if (p0->Type() == GDL_DOUBLE) double_flag=1;
    static int doubleIx=e->KeywordIx("DOUBLE");
    if (e->KeywordSet(doubleIx)) double_flag=1;

    if (double_flag)
      {	return res; }
    else
      { return res->Convert2(GDL_FLOAT, BaseGDL::CONVERT); }
  }
  
  int TDMAsolver8(SizeT M, double a[], double b[], double c[], double d[], double x[])
  {
    /* Tri Diagonal Matrix Algorithm(a.k.a Thomas algorithm) solver
     TDMA solver, a b c d can be NumPy array type or Python list type.
     refer to http://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
     */
    if (b[0] == 0) return 1;
    double w;
    DLong i; //not SizeT as unsigned loops fail miserably with decrementing (--i)

    for (i = 1; i < M; ++i) {
      if (b[i-1] == 0) return 1;
      w = a[i] / b[i - 1];
      b[i] -= w * c[i - 1];
      d[i] -= w * d[i - 1];
    }
    x[M - 1] = d[M - 1] / b[M - 1];

    for (i = M - 2; i >= 0; --i) {
      x[i] = (d[i] - c[i] * x[i + 1]) / b[i];
    }
    return 0;
  }

  int TDMAsolver4(SizeT M, double a[], double b[], double c[], double d[], float x[])
  {
    /* Tri Diagonal Matrix Algorithm(a.k.a Thomas algorithm) solver
     TDMA solver, a b c d can be NumPy array type or Python list type.
     refer to http://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
     */
    double w;
    DLong i; //not SizeT as unsigned loops fail miserably with decrementing (--i)

    for (i = 1; i < M; ++i) {
      if (b[i-1] == 0) return 1;
      w = a[i] / b[i - 1];
      b[i] -= w * c[i - 1];
      d[i] -= w * d[i - 1];
    }
    x[M - 1] = d[M - 1] / b[M - 1];

    for (i = M - 2; i >= 0; --i) {
      x[i] = (d[i] - c[i] * x[i + 1]) / b[i];
    }
    return 0;
  }
  BaseGDL* trisol_fun( EnvT* e) {
    
    // managing first input: Square Matrix
    
    BaseGDL* p0 = e->GetParDefined(0); // sub-diag elements
    BaseGDL* p1 = e->GetParDefined(1); // diagonal elements
    BaseGDL* p2 = e->GetParDefined(2); // sup-diag elements
    BaseGDL* p3 = e->GetParDefined(3); // right-hand side vector

    SizeT nEl0 = p0->N_Elements();
    if( nEl0 == 0) e->Throw( "Variable is undefined: " + e->GetParString(0));
    SizeT nEl1 = p1->N_Elements();
    if( nEl1 == 0) e->Throw( "Variable is undefined: " + e->GetParString(1));
    SizeT nEl2 = p2->N_Elements();
    if( nEl2 == 0) e->Throw( "Variable is undefined: " + e->GetParString(2));
    SizeT nEl3 = p3->N_Elements();
    if( nEl3 == 0) e->Throw( "Variable is undefined: " + e->GetParString(3));

    //    cout << nEl0 << " " << nEl1 << " " << nEl2 << " " << nEl3 << " " << endl;

    SizeT nEl = nEl0;
    if (nEl1 != nEl) e->Throw( "Argument: " + e->GetParString(1)+" does not have correct size");
    if (nEl2 != nEl) e->Throw( "Argument: " + e->GetParString(2)+" does not have correct size");
    if (nEl3 != nEl) e->Throw( "Argument: " + e->GetParString(3)+" does not have correct size");
    
    int complex_flag=0;
    if (p0->Type() == GDL_COMPLEXDBL || p0->Type() == GDL_COMPLEX) complex_flag=1;
    if (p1->Type() == GDL_COMPLEXDBL || p1->Type() == GDL_COMPLEX) complex_flag=1;
    if (p2->Type() == GDL_COMPLEXDBL || p2->Type() == GDL_COMPLEX) complex_flag=1;
    if (p3->Type() == GDL_COMPLEXDBL || p3->Type() == GDL_COMPLEX) complex_flag=1;
    if (complex_flag) {
      e->Throw( "Input type cannot be COMPLEX, please use LA_TRISOL (not ready)");
    }

    // computations are done in Double type, conversion at the end
    
    DDoubleGDL *p0D = e->GetParAs<DDoubleGDL>(0); //A
    DDoubleGDL *p1D= e->GetParAs<DDoubleGDL>(1)->Dup();//B modified in call to TDMASolver
    Guard<BaseGDL> g1(p1D);
    DDoubleGDL *p2D= e->GetParAs<DDoubleGDL>(2);//C
    DDoubleGDL *p3D= e->GetParAs<DDoubleGDL>(3)->Dup();//D idem
    Guard<BaseGDL> g3(p3D);
    DLong double_flag=0;
    static int doubleIx=e->KeywordIx("DOUBLE");
    e->AssureLongScalarKWIfPresent(doubleIx,double_flag);
    if (double_flag) {
    
    DDoubleGDL* res = new DDoubleGDL(nEl, BaseGDL::NOZERO);
      int err=TDMAsolver8(nEl,(DDouble*)p0D->DataAddr(),(DDouble*)p1D->DataAddr(),(DDouble*)p2D->DataAddr(),(DDouble*)p3D->DataAddr(), (DDouble*)res->DataAddr());
      if (err > 0) {
        GDLDelete(res);
        e->Throw("TRISOL: Error "+i2s(err)+" in tridag");
      }
      return res;
    } else {  
      DFloatGDL* res = new DFloatGDL(nEl, BaseGDL::NOZERO);
      int err=TDMAsolver4(nEl,(DDouble*)p0D->DataAddr(),(DDouble*)p1D->DataAddr(),(DDouble*)p2D->DataAddr(),(DDouble*)p3D->DataAddr(), (DFloat*)res->DataAddr());
      if (err > 0) {
        GDLDelete(res);
        e->Throw("TRISOL: Error "+i2s(err)+" in tridag");
      }
      return res;
    }
  }
}

