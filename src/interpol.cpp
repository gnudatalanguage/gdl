/***************************************************************************
         interpolate.cpp  -  all things related to interpol command
                             -------------------
    begin                : Mar 30 2021
    copyright            : (C) 2004 by Joel Gales
                         : (C) 2018 G. Duvert 
    email                : see https://github.com/gnudatalanguage/gdl
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



//interpolate
#include <gsl/gsl_errno.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_nan.h>

/* convolutions available */
inline double linConv(double d, double x0, double x1) {
  return (1. - d)*x0 + d*x1;
}

typedef struct {
    const char* name;
    unsigned int min_size;
    void* (*alloc)(ssize_t size);
    int (*init)(void*, const double xa[], const double ta[], ssize_t xsize);
    int (*eval)(const void*, const double xa[], const double ta[], ssize_t xsize, double x, gsl_interp_accel*, ssize_t *lastval, double* lastCoefs, double *t);
    void (*free)(void*);
  } gdl_interpol_type;
 
  typedef struct {
    const gdl_interpol_type* type;
    double xmin;
    double xmax;
    ssize_t xsize;
    void* state;
    ssize_t * lastval;
    double * lastCoefs; //if last index is same, use last coefs
  } gdl_interpol; 
  
ssize_t gdl_interpol_type_min_size(const gdl_interpol_type* T) {
    return T->min_size;
}

ssize_t gdl_interpol_min_size(const gdl_interpol* interp) {
  return interp->type->min_size;
}

const char* gdl_interpol_name(const gdl_interpol* interp) {
  return interp->type->name;
}

GSL_VAR const gdl_interpol_type* gdl_interpol_linear;
GSL_VAR const gdl_interpol_type* gdl_interpol_quadratic;
GSL_VAR const gdl_interpol_type* gdl_interpol_spline;

double gdl_interpol_eval(const gdl_interpol* interp, const double xarr[], const double tarr[], const double x, gsl_interp_accel* xa);

gdl_interpol* gdl_interpol_alloc(const gdl_interpol_type* T, ssize_t xsize) {
    gdl_interpol* interp;
    interp = (gdl_interpol*) malloc(sizeof (gdl_interpol));
    if (interp == NULL) {
      GSL_ERROR_NULL("failed to allocate space for gdl_interpol struct", GSL_ENOMEM);
    }
    interp->type = T;
    interp->xsize = xsize;
    if (interp->type->alloc == NULL) {
      interp->state = NULL;
      return interp;
    }
    interp->state = interp->type->alloc(xsize);
    if (interp->state == NULL) {
      free(interp);
      GSL_ERROR_NULL("failed to allocate space for gdl_interpol state", GSL_ENOMEM);
    }
    return interp;
  }

  int gdl_interpol_init(gdl_interpol* interp, const double xarr[], const double tarr[], ssize_t xsize) {
    ssize_t i;
    if (xsize != interp->xsize) {
      GSL_ERROR("data must match size of interpolation object", GSL_EINVAL);
    }
    for (i = 1; i < xsize; i++) {
      if (xarr[i - 1] >= xarr[i]) {
//        GSL_ERROR("x values must be strictly increasing", GSL_EINVAL);
        Message ("X values are not strictly increasing, INTERPOL may give incorrect results");
        break;
      }
    }
    interp->xmin = xarr[0];
    interp->xmax = xarr[xsize - 1];
    int status = interp->type->init(interp->state, xarr, tarr, xsize);
    //allocate lastCoefs and set lastval to -1:
    interp->lastval=(ssize_t*)malloc(1*sizeof(ssize_t));
    interp->lastval[0]=-1;
    interp->lastCoefs=(double*)malloc(2*interp->type->min_size*sizeof(double));
    return status;
  }
  void gdl_interpol_free(gdl_interpol* interp) {
    if (!interp) {
      return;
    }
    if (interp->type->free) {
      interp->type->free(interp->state);
    }
    free(interp->lastval);
    free(interp->lastCoefs);
    free(interp);
  }
  
  double gdl_interpol_eval(const gdl_interpol* interp, const double xarr[], const double tarr[], const double x, gsl_interp_accel* xa) {
    double xx, t;
    xx = x;
    int status;
    status = interp->type->eval(interp->state, xarr, tarr, interp->xsize, xx, xa, interp->lastval, interp->lastCoefs, &t);
    if ((status) != GSL_SUCCESS) GSL_ERROR_VAL("interpolation error", (status), GSL_NAN);

    return t;
  }
  static int linear_init(void* state, const double xa[], const double ta[], ssize_t xsize) {
    return GSL_SUCCESS;
  }
  static int quadratic_init(void* state, const double xa[], const double ta[], ssize_t xsize) {
    return GSL_SUCCESS;
  }

  static int linear_eval(const void* state, const double xarr[], const double tarr[], ssize_t xsize, double x, gsl_interp_accel* xa, ssize_t* lastval, double* C, double *t) {
    ssize_t xi = gsl_interp_accel_find(xa, xarr, xsize, x); //xa must be ALWAYS defined for GDL
    if (xi != *lastval) {
      *lastval = xi;
      ssize_t xp = (xi + 1 < xsize) ? xi + 1 : xi;
      C[0] = tarr[xi];
      C[1] = tarr[xp];
      C[2] = xarr[xi];
      C[3] = xarr[xp]-xarr[xi];
    }
    double u = (C[3] > 0.0) ? (x - C[2]) / C[3] : 0.0;
    *t = linConv(u, C[0], C[1]);
    return GSL_SUCCESS;
  }

  static int quadratic_eval(const void* state, const double xarr[], const double tarr[], ssize_t xsize, double x, gsl_interp_accel* xa, ssize_t* lastval, double* C, double *t) {
    ssize_t xi = gsl_interp_accel_find(xa, xarr, xsize, x);//xa must be ALWAYS defined for GDL
    if (xi != *lastval) {
      *lastval = xi;
      ssize_t xp, xm;
      if ( xi+1 >= xsize ) { xp = xsize-1; xi = xsize-2; xm=xsize-3;}
      else if ( xi-1 < 0 ) { xm = 0; xi = 1; xp=2; }
      else { xm=xi-1; xp=xi+1;}
      C[3] = tarr[xm];
      C[4] = tarr[xi];
      C[5] = tarr[xp];
      C[0] = xarr[xm];
      C[1] = xarr[xi];
      C[2] = xarr[xp];
    }
    *t = (C[3] * (x-C[1]) * (x-C[2]) / ((C[0]-C[1]) * (C[0]-C[2]))) + (C[4] * (x-C[0]) * (x-C[2]) / ((C[1]-C[0]) * (C[1]-C[2])))+ (C[5] * (x-C[0]) * (x-C[1]) / ((C[2]-C[0]) * (C[2]-C[1])));
    return GSL_SUCCESS;
  }
    
  static const gdl_interpol_type linear_type = {
    "linear",
    2,
    NULL,
    &linear_init,
    &linear_eval
  };

  static const gdl_interpol_type quadratic_type = {
    "quadratic",
    3,
    NULL,
    &quadratic_init,
    &quadratic_eval
  };

 const gdl_interpol_type* gdl_interpol_linear = &linear_type; 
 const gdl_interpol_type* gdl_interpol_quadratic = &quadratic_type;
 
#if defined(USE_EIGEN)
 GSL_VAR const gdl_interpol_type* gdl_interpol_lsquadratic;
 static int lsquadratic_init(void* state, const double xa[], const double ta[], ssize_t xsize) {
    return GSL_SUCCESS;
  }

#include <Eigen/LU>
#include <Eigen/Eigenvalues>
#include <Eigen/Core>
 //no need to use another complicated general GSL solution -- just the same as IDL's procedure.
  static int lsquadratic_eval(const void* state, const double xarr[], const double tarr[], ssize_t xsize, double pos, gsl_interp_accel* xa, ssize_t* lastval, double* C, double *t) {
    ssize_t xi = gsl_interp_accel_find(xa, xarr, xsize, pos);//xa must be ALWAYS defined for GDL
    if (xi != *lastval) {
      *lastval = xi;
      ssize_t x[4];
      //make in range
      if ( xi+2 >= xsize ) {  x[0]=xsize-4;  x[1]=xsize-3;  x[2] = xsize-2; x[3] = xsize-1;}
      else if ( xi-1 < 0 ) { x[0] = 0; x[1] = 1; x[2]=2; x[3]=3;}
      else { x[0]=xi-1; x[1]= xi; x[2]=xi+1; x[3]=xi+2;}
     // least_square fit of a 2nd degree polynomial on these 4 points:
      double matrix[]={xarr[x[0]]*xarr[x[0]],xarr[x[0]],1.0,xarr[x[1]]*xarr[x[1]],xarr[x[1]],
      1.0,xarr[x[2]]*xarr[x[2]],xarr[x[2]],1.0,xarr[x[3]]*xarr[x[3]],xarr[x[3]],1.0};
      Eigen::MatrixXd A(4,3) ; //<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > A(matrix, 3,4);
      for (int i=0; i< 3; ++i) for (int j=0; j<4; ++j) A(j,i)=matrix[j*3+i];
      Eigen::Vector4d b(tarr[x[0]],tarr[x[1]],tarr[x[2]],tarr[x[3]]);
      Eigen::MatrixXd r=(A.transpose() * A).ldlt().solve(A.transpose() * b);
      C[0] = r.coeff(0);
      C[1] = r.coeff(1);
      C[2] = r.coeff(2);
    }
   *t= C[2] + C[1]* pos + C[0] * pos*pos;
   return GSL_SUCCESS;
  }
  static const gdl_interpol_type lsquadratic_type = {
    "lsquadratic",
    4,
    NULL,
    &lsquadratic_init,
    &lsquadratic_eval
  }; 
 const gdl_interpol_type* gdl_interpol_lsquadratic = &lsquadratic_type; 
#endif
 
  static int spline_init(void* state, const double xa[], const double ta[], ssize_t xsize) {
    return GSL_SUCCESS;
  }
  static int spline_eval(const void* state, const double xarr[], const double tarr[], ssize_t xsize, double pos, gsl_interp_accel* xa, ssize_t* lastval, double* C, double *t) {
    static DIntGDL One=DIntGDL(1);
    ssize_t xi = gsl_interp_accel_find(xa, xarr, xsize, pos);//xa must be ALWAYS defined for GDL
//    if (xi != *lastval) {
      DDoubleGDL* P=new DDoubleGDL(pos);
      DDoubleGDL* X;
      DDoubleGDL* Y;
      
      *lastval = xi;
      ssize_t x[4];
      //make in range
      if ( xi+2 >= xsize ) {  x[0]=xsize-4;  x[1]=xsize-3;  x[2] = xsize-2; x[3] = xsize-1;}
      else if ( xi-1 < 0 ) { x[0] = 0; x[1] = 1; x[2]=2; x[3]=3;}
      else { x[0]=xi-1; x[1]= xi; x[2]=xi+1; x[3]=xi+2;}
      C[0]=xarr[x[0]];C[1]=xarr[x[1]];C[2]=xarr[x[2]];C[3]=xarr[x[3]];
      C[4]=tarr[x[0]];C[5]=tarr[x[1]];C[6]=tarr[x[2]];C[7]=tarr[x[3]];
      X=new DDoubleGDL(dimension(4),BaseGDL::NOZERO); for(int i=0; i<4; ++i) (*X)[i]=C[i];
      Y=new DDoubleGDL(dimension(4),BaseGDL::NOZERO); for(int i=0; i<4; ++i) (*Y)[i]=C[i+4]; 

      static int splinitIx = LibFunIx( "SPL_INIT" );
      EnvT* newEnv = new EnvT(NULL, libFunList[ splinitIx]);
      newEnv->SetNextPar( X ); // pass as local
      newEnv->SetNextPar( Y ); // pass as local
      newEnv->SetKeyword("DOUBLE",&One);
      DDoubleGDL* Q = static_cast<DDoubleGDL*>(static_cast<DLibFun*>(newEnv->GetPro())->Fun()(static_cast<EnvT*>(newEnv)));

      static int splinterpIx = LibFunIx( "SPL_INTERP" );
      EnvT* newEnv1 = new EnvT(NULL, libFunList[ splinterpIx]);
      newEnv1->SetNextPar( X ); // pass as local
      newEnv1->SetNextPar( Y ); // pass as local
      newEnv1->SetNextPar( Q ); // pass as local
      newEnv1->SetNextPar( P ); // pass as local
      newEnv1->SetKeyword("DOUBLE",&One);
      DDoubleGDL* res =static_cast<DDoubleGDL*>( static_cast<DLibFun*>(newEnv1->GetPro())->Fun()(static_cast<EnvT*>(newEnv1)));
      *t=(*res)[0];
    return GSL_SUCCESS;
  }
  static const gdl_interpol_type spline_type = {
    "spline",
    4,
    NULL,
    &spline_init,
    &spline_eval
  }; 
 const gdl_interpol_type* gdl_interpol_spline = &spline_type; 
 
namespace lib {
  
  BaseGDL* interpol_fun(EnvT* e){
    SizeT nParam = e->NParam();
    if (nParam < 2 || nParam > 3) e->Throw("Incorrect number of arguments.");

    const gdl_interpol_type* interpol=gdl_interpol_linear;
    // options
    static int LSQUADRATIC = e->KeywordIx("LSQUADRATIC");
#if defined(USE_EIGEN)
    if (e->KeywordSet(LSQUADRATIC)) interpol=gdl_interpol_lsquadratic;
#else
    if (e->KeywordSet(LSQUADRATIC)) interpol=gdl_interpol_quadratic;
#endif
    static int QUADRATIC = e->KeywordIx("QUADRATIC");
    if (e->KeywordSet(QUADRATIC)) interpol=gdl_interpol_quadratic;
    static int SPLINE = e->KeywordIx("SPLINE");
    if (e->KeywordSet(SPLINE)) interpol=gdl_interpol_spline;
    static int NANIx = e->KeywordIx("NAN");
    bool noNan=e->KeywordSet(NANIx);
    unsigned int nmin=gdl_interpol_type_min_size(interpol);
    
    //dimensions
    BaseGDL* p0 = e->GetParDefined(0);
    DType type=p0->Type();
    SizeT nV=p0->N_Elements();
    if (nV <nmin) e->Throw("V has too few elements for this kind of interpolation.");
    //alloc interpolant object & guard
    gdl_interpol * myinterp=gdl_interpol_alloc (interpol,nV);
    GDLGuard<gdl_interpol> ginterpol(myinterp, gdl_interpol_free);
    //alloc accelerator & guard
    gsl_interp_accel * acc=gsl_interp_accel_alloc ();
    GDLGuard<gsl_interp_accel> g1( acc, gsl_interp_accel_free);
    
    DDoubleGDL* V=e->GetParAs<DDoubleGDL>(0);
    //X and its guard
    DDoubleGDL* X; Guard<BaseGDL> guardX;
    //Xout and its guard
    DDoubleGDL* Xout; Guard<BaseGDL> guardXout;
    // the type of output.
    DType t=type; //V's type by default.
    //size of the out value
    SizeT nout=0;
    
    if (nParam==2) {       // we make X
      BaseGDL* p1 = e->GetParDefined(1);
      if (p1->N_Elements() >1) e->Throw("N must be one positive integer");
      DLongGDL* n1gdl=e->GetParAs<DLongGDL>(1);
      nout=(*n1gdl)[0];
      if (nout < 1) e->Throw("N must be one positive integer");
      X=new DDoubleGDL(dimension(nV),BaseGDL::INDGEN);
      guardX.Reset(X);//X will be destroyed on exit
      Xout=new DDoubleGDL(dimension(nout),BaseGDL::INDGEN);
      guardXout.Reset(Xout);//Xout will be destroyed on exit
      for (SizeT i = 0; i < nout; ++i) {
        (*Xout)[i] = double(i)*(nV - 1) / (nout - 1);
      }
    } else {
      BaseGDL* p1 = e->GetParDefined(1);
      if (p1->N_Elements() != nV) e->Throw("V and X arrays must have same # of elements");
      X = e->GetParAs<DDoubleGDL>(1);
      BaseGDL* p2 = e->GetParDefined(2);
      nout = p2->N_Elements();
      DType t = (DTypeOrder[ p0->Type()] > DTypeOrder[ p1->Type()]) ? p0->Type() : p1->Type();
      t = (DTypeOrder[t] > DTypeOrder[ p2->Type()]) ? t : p2->Type();      
      Xout=e->GetParAs<DDoubleGDL>(2);
    }
    int status=gdl_interpol_init (myinterp, (const double*)X->DataAddr(), (const double*)V->DataAddr() , nV);
    //allocate result
    DDoubleGDL* res=new DDoubleGDL(nout,BaseGDL::NOZERO);
    //compute
    for (SizeT i=0; i< nout; ++i)  {
      (*res)[i]=gdl_interpol_eval(myinterp, (const double*)X->DataAddr() , (const double*) V->DataAddr(), (*Xout)[i] , acc);
    }
    
    if (p0->Type() == GDL_DOUBLE) return res;
    else {
      BaseGDL* resf=res->Convert2(GDL_FLOAT, BaseGDL::COPY);
      GDLDelete(res);
      return resf;
    }
  }
}
