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

#include "datatypes.hpp"
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

  const int szdbl=sizeof(double);
  const int szflt=sizeof(float);
  const int szlng=sizeof(long);

  void ludc_pro( EnvT* e)
  {

    //  cout << szdbl << " " <<szflt << " " << szlng << endl;

    SizeT nParam=e->NParam(1);
    int s;
    
    //     if( nParam == 0)
    //       e->Throw( "Incorrect number of arguments.");

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


    if (p0->Type() == COMPLEXDBL || p0->Type() == COMPLEX){
      e->Throw( "Input type cannot be COMPLEX, please use LA_LUDC (not ready)");
    }
    
    if ( p0->Type() != DOUBLE) {
      e->Throw("Sorry, Input type can be only DOUBLE now (please contribute)");
    } else {
      DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
      
      gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(0));
      
      memcpy(mat->data, &(*p0D)[0], nEl*szdbl);
      
      gsl_permutation * p = gsl_permutation_alloc (p0->Dim(0));
      int s;
      gsl_linalg_LU_decomp (mat, p, &s);
      //	gsl_linalg_LU_solve (mat, p, &b.vector, x);

      int debug=0;
      if (debug) {
	cout << "permutation order: " << s << endl;
	cout << "permutation vector:"<< endl;
	gsl_permutation_fprintf (stdout, p, " %u");
	cout << endl;
      }

      // copying over p0 the updated matrix	
      DLong dims[2] = {p0->Dim(0), p0->Dim(0)};
      dimension dim0(dims, (SizeT) 2);
      BaseGDL** p0Do = &e->GetPar( 0);
      delete (*p0Do);
      *p0Do = new DDoubleGDL(dim0, BaseGDL::NOZERO);
	
      memcpy(&(*(DDoubleGDL*) *p0Do)[0], mat->data, 
	     p0->Dim(0)*p0->Dim(0)*szdbl);

      // copying over p1 the permutation vector
      DLong n = p0->Dim(0);
      dimension dim1(&n, (SizeT) 1);
      BaseGDL** p1D = &e->GetPar( 1);
      delete (*p1D);
      *p1D = new DLongGDL(dim1, BaseGDL::NOZERO);
      memcpy(&(*(DLongGDL*) *p1D)[0], p->data, 
	     p0->Dim(0)*szlng);

      gsl_matrix_free(mat);
      gsl_permutation_free(p);
    }

  }
  
  BaseGDL* lusol_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    int s;
    
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
    
    
    if (p0->Type() == COMPLEXDBL || p0->Type() == COMPLEX){
      e->Throw( "Input type cannot be COMPLEX, please use LA_LUDC (not ready)");
    }
    
    DDoubleGDL* res ;
    res = new DDoubleGDL(p2->Dim(0));
    
    if( p0->Type() != DOUBLE) {
      e->Throw("Sorry, Input type can be only DOUBLE now (please contribute)");
    } else {
	
      DDoubleGDL* p0D = static_cast<DDoubleGDL*>(p0);
      gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(0));      
      memcpy(mat->data, &(*p0D)[0], nEl*szdbl);
      
      DLongGDL* p1L = static_cast<DLongGDL*>( p1);
      gsl_permutation *p = gsl_permutation_alloc (nEl1);
      memcpy(p->data, &(*p1L)[0], nEl1*szlng);
      
      DDoubleGDL* p2D = static_cast<DDoubleGDL*>( p2);
      gsl_vector *b = gsl_vector_alloc(nEl2);
      memcpy(b->data, &(*p2D)[0], nEl1*szdbl);

      gsl_vector *x = gsl_vector_alloc(nEl2);

      // computation by GSL
      gsl_linalg_LU_solve (mat, p, b, x);

      int debug=0;
      if (debug) {
	cout << "permutation order: " << s << endl;
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
	
      gsl_matrix_free(mat);
      gsl_vector_free(x);
      gsl_permutation_free(p);
      return res;

    }
    
    return res;
    
  }

  
  BaseGDL* determ_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

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
    //    if (nParam == 2) e->AssureGlobalPar( 1);
    
    if (p0->Type() == COMPLEXDBL || p0->Type() == COMPLEX){
      e->Throw( "Input type cannot be COMPLEX, please use LA_DETERM (not ready)");
    }
    
    DDoubleGDL res=0.0 ;
    
    if( p0->Type() != DOUBLE){      
      e->Throw("Sorry, Input type can be only DOUBLE now (please contribute)");
    } else{
      
      DDoubleGDL* p0D = static_cast<DDoubleGDL*>(p0);
      gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(0));
      memcpy(mat->data, &(*p0D)[0], nEl*szdbl);
      
      gsl_permutation *p = gsl_permutation_alloc(p0->Dim(0));

      int sign;
      double determ=0.0;
   
      // computation by GSL
      gsl_linalg_LU_decomp (mat, p, &sign);
      determ=gsl_linalg_LU_det (mat, sign);

      int debug=0;
      if (debug) {
	cout << "Determ : " << determ << endl;
      }

      gsl_matrix_free(mat);
      gsl_permutation_free(p);

      DDoubleGDL* res = new DDoubleGDL(1, BaseGDL::NOZERO);
      (*res)[0]=determ;

      return res;
    
    }
    //    return res;
  }

  BaseGDL* trisol_fun( EnvT* e){
    //    SizeT nParam=e->NParam(1);
    
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
    if (p0->Type() == COMPLEXDBL || p0->Type() == COMPLEX) complex_flag=1;
    if (p1->Type() == COMPLEXDBL || p1->Type() == COMPLEX) complex_flag=1;
    if (p2->Type() == COMPLEXDBL || p2->Type() == COMPLEX) complex_flag=1;
    if (p3->Type() == COMPLEXDBL || p3->Type() == COMPLEX) complex_flag=1;
    if (complex_flag) {
      e->Throw( "Input type cannot be COMPLEX, please use LA_TRISOL (not ready)");
    }

    // computations are done in Double type, conversion at the end
    
    DDoubleGDL *p0D = e->GetParAs<DDoubleGDL>(0);
    gsl_vector *subd = gsl_vector_alloc(nEl-1);
    memcpy(subd->data, &(*p0D)[1], (nEl-1)*szdbl);
    
    DDoubleGDL *p1D= e->GetParAs<DDoubleGDL>(1);// = static_cast<DDoubleGDL*>(p1);
    gsl_vector *diag = gsl_vector_alloc(nEl);
    memcpy(diag->data, &(*p1D)[0], nEl*szdbl);
    
    DDoubleGDL *p2D= e->GetParAs<DDoubleGDL>(2); // = static_cast<DDoubleGDL*>(p2);
    gsl_vector *supd = gsl_vector_alloc(nEl-1);
    memcpy(supd->data, &(*p2D)[0], (nEl-1)*szdbl);
    
    DDoubleGDL *p3D= e->GetParAs<DDoubleGDL>(3);// = static_cast<DDoubleGDL*>(p3);
    gsl_vector *rhs = gsl_vector_alloc(nEl);
    memcpy(rhs->data, &(*p3D)[0], nEl*szdbl);
    
    gsl_vector *x = gsl_vector_alloc(nEl);
    
    // computation by GSL  
    int error_code=-1;
    error_code=gsl_linalg_solve_tridiag (diag, supd, subd, rhs, x);

    if (error_code > 0)
      Message( e->GetProName() + ": GSL did return an error. Is realy the matrix Pos. Define ?");
  
    int debug=0;
    if (debug) {
      gsl_vector_fprintf (stdout, diag, "diag: %g");
      gsl_vector_fprintf (stdout, subd, "subd: %g");
      gsl_vector_fprintf (stdout, supd, "supd: %g");
      gsl_vector_fprintf (stdout, rhs, "rhs: %g");
      gsl_vector_fprintf (stdout, x, "res: %g");
    }
    
    gsl_vector_free(diag);
    gsl_vector_free(subd);
    gsl_vector_free(supd);
    gsl_vector_free(rhs);
    
    int double_flag=0;
    if (p0->Type() == DOUBLE || p1->Type() == DOUBLE) double_flag=1;
    if (p2->Type() == DOUBLE || p2->Type() == DOUBLE) double_flag=1;
    if (e->KeywordSet("DOUBLE")) double_flag=1;
    
    DDoubleGDL* res = new DDoubleGDL(nEl, BaseGDL::NOZERO);
    memcpy(&(*res)[0], x->data, nEl*szdbl);
    
    if (double_flag)
      {	return res; }
    else
      { return res->Convert2(FLOAT, BaseGDL::CONVERT); }
  }
}

