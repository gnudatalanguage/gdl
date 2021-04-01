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
    void* (*alloc)(size_t size);
    int (*init)(void*, const double xa[], const double ta[], size_t xsize);
    int (*eval)(const void*, const double xa[], const double ta[], size_t xsize, double x, gsl_interp_accel*, double* t);
    void (*free)(void*);
  } gdl_interpol_type;
 
  typedef struct {
    const gdl_interpol_type* type;
    double xmin;
    double xmax;
    size_t xsize;
    void* state;
  } gdl_interpol; 
  
size_t gdl_interpol_type_min_size(const gdl_interpol_type* T) {
    return T->min_size;
}

size_t gdl_interpol_min_size(const gdl_interpol* interp) {
  return interp->type->min_size;
}

const char* gdl_interpol_name(const gdl_interpol* interp) {
  return interp->type->name;
}

GSL_VAR const gdl_interpol_type* gdl_interpol_linear;
GSL_VAR const gdl_interpol_type* gdl_interpol_quadratic;
//GSL_VAR const gdl_interpol_type* gdl_interpol_lsquadratic;
//GSL_VAR const gdl_interpol_type* gdl_interpol_spline;

double gdl_interpol_eval(const gdl_interpol* interp, const double xarr[], const double tarr[], const double x, gsl_interp_accel* xa);

gdl_interpol* gdl_interpol_alloc(const gdl_interpol_type* T, size_t xsize) {
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

  int gdl_interpol_init(gdl_interpol* interp, const double xarr[], const double tarr[], size_t xsize) {
    size_t i;
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
    return status;
  }
  void gdl_interpol_free(gdl_interpol* interp) {
    if (!interp) {
      return;
    }
    if (interp->type->free) {
      interp->type->free(interp->state);
    }
    free(interp);
  }
  
  double gdl_interpol_eval(const gdl_interpol* interp, const double xarr[], const double tarr[], const double x, gsl_interp_accel* xa) {
    double xx, t;
    xx = x;
    int status;
    status = interp->type->eval(interp->state, xarr, tarr, interp->xsize, xx, xa, &t);
    if ((status) != GSL_SUCCESS) GSL_ERROR_VAL("interpolation error", (status), GSL_NAN);

    return t;
  }
  static int linear_init(void* state, const double xa[], const double ta[], size_t xsize) {
    return GSL_SUCCESS;
  }
  static int quadratic_init(void* state, const double xa[], const double ta[], size_t xsize) {
    return GSL_SUCCESS;
  }  
  static int linear_eval(const void* state, const double xarr[], const double tarr[], size_t xsize, double x, gsl_interp_accel* xa, double* t) {
    double xmin, xmax;
    double dx;
    double u;
    size_t xi, xp;
    double kern[2];
    xi = gsl_interp_accel_find(xa, xarr, xsize, x);//xa must be ALWAYS defined for GDL
    xp = (xi + 1 < xsize) ? xi + 1 : xi;

    kern[0] = tarr[xi];
    kern[1] = tarr[xp];

    xmin = xarr[xi];
    xmax = xarr[xp];
    dx = xmax - xmin;
    u = (dx > 0.0) ? (x - xmin) / dx : 0.0;

    *t = linConv(u, kern[0], kern[1]);

    return GSL_SUCCESS;
  }
    static int quadratic_eval(const void* state, const double xarr[], const double tarr[], size_t xsize, double x, gsl_interp_accel* xa, double* t) {
    double x0, x1, x2;
    double y0, y1, y2;
    size_t xi, xp, xm;
    xi = gsl_interp_accel_find(xa, xarr, xsize, x);//xa must be ALWAYS defined for GDL
    xp = (xi + 1 < xsize) ? xi + 1 : xi;
    xm = (xi > 0) ? xi - 1 : xi;

    y0 = tarr[xm];
    y1 = tarr[xi];
    y2 = tarr[xp];

    x0 = xarr[xm];
    x1 = xarr[xi];
    x2 = xarr[xp];
    *t = (y0 * (x-x1) * (x-x2) / ((x0-x1) * (x0-x2))) + (y1 * (x-x0) * (x-x2) / ((x1-x0) * (x1-x2)))+ (y2 * (x-x0) * (x-x1) / ((x2-x0) * (x2-x1)));

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
  
namespace lib {
  
  BaseGDL* interpol_fun(EnvT* e){
    SizeT nParam = e->NParam();
    if (nParam < 2 || nParam > 3) e->Throw("Incorrect number of arguments.");

    const gdl_interpol_type* interpol=gdl_interpol_linear;
    // options
    static int LSQUADRATIC = e->KeywordIx("LSQUADRATIC");
//    if (e->KeywordSet(LSQUADRATIC)) interpol=gdl_interpol_lsquadratic;
    static int QUADRATIC = e->KeywordIx("QUADRATIC");
    if (e->KeywordSet(QUADRATIC)) interpol=gdl_interpol_quadratic;
    static int SPLINE = e->KeywordIx("SPLINE");
//    if (e->KeywordSet(SPLINE)) interpol=gdl_interpol_spline;
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
//
//
//      //think about boundaries...
//      DLong minEl, maxEl;
//      X->MinMax( &minEl, &maxEl, NULL, NULL, true);
//      DDouble minX=(*X)[minEl];
//      DDouble maxX=(*X)[maxEl];
//      //trouble if values are asked at position that X does not sample: gsl will hang on an error. IDL is way cooler.
//      Xout->MinMax( &minEl, &maxEl, NULL, NULL, true);
//      DDouble minXout=(*Xout)[minEl];
//      DDouble maxXout=(*Xout)[maxEl];
//      bool good=((( minXout >= minX) && (minXout <= maxX)) && ((maxXout >= minX) && (maxXout <= maxX)));
//      if (good) {
//        for (SizeT i = 0; i < n2; ++i) {
//          (*res)[i] = gsl_interp_eval(myinterp, (const double*) X->DataAddr(), (const double*) V->DataAddr(), (*Xout)[i], acc);
//        }
//      } else {
//        for (SizeT i = 0; i < n2; ++i) {
//          DDouble pos = (*Xout)[i];
//          if ((pos >= minX) && (pos <= maxX)) { //still good
//            (*res)[i] = gsl_interp_eval(myinterp, (const double*) X->DataAddr(), (const double*) V->DataAddr(), pos, acc);
//          } else {
//            if (interpol == gsl_interp_linear) {
//                double  xb0 = (*X)[0];
//                double  yb0 = (*V)[0];
//                double ybinc = (*V)[1]-yb0;
//                double dbx = (*X)[1] - xb0;
//                double  xe0 = (*X)[nV-1];
//                double  ye0 = (*V)[nV-1];
//                double yeinc = (*V)[nV-2]-ye0;
//                double dex = (*X)[nV-2] - xe0;
//                if (pos < minX) {
//                  (*res)[i]=yb0 + (pos - xb0) / dbx * ybinc;
//              } else {
//                  (*res)[i]=ye0 + (pos - xe0) / dex * yeinc;
//              }
//            }
//          }
//        }
//      }

  }

//    BaseGDL* interpol_fun(EnvT* e){
//    SizeT nParam = e->NParam();
//    if (nParam < 2 || nParam > 3) e->Throw("Incorrect number of arguments.");
//
//    const gdl_interp_type* interp=gdl_interp_linear;
//    // options
////    static int LSQUADRATIC = e->KeywordIx("LSQUADRATIC");
////    if (e->KeywordSet(LSQUADRATIC)) interp=gsl_interp_polynomial;
////    static int QUADRATIC = e->KeywordIx("QUADRATIC");
////    if (e->KeywordSet(QUADRATIC)) interp=gsl_interp_polynomial;
////    static int SPLINE = e->KeywordIx("SPLINE");
////    if (e->KeywordSet(SPLINE)) interp=gsl_interp_cspline;
//    static int NANIx = e->KeywordIx("NAN");
//    bool noNan=e->KeywordSet(NANIx);
//    unsigned int nmin=dll_interp_type_min_size(interp);
//    
//    //dimensions
//    BaseGDL* p0 = e->GetParDefined(0);
//    DType type=p0->Type();
//    SizeT nx=p0->N_Elements();
//    if (nx <nmin) e->Throw("V has too few elements for this kind of interpolation.");
//    DDoubleGDL* V=e->GetParAs<DDoubleGDL>(0);
//      if (nParam==2) {
////      BaseGDL* p1 = e->GetParDefined(1);
////      if (p1->N_Elements() >1) e->Throw("N must be one positive integer");
////      DLongGDL* n1gdl=e->GetParAs<DLongGDL>(1);
////      DLong n=(*n1gdl)[0];
////      if (n < 1) e->Throw("N must be one positive integer");
////
////      DDoubleGDL* X=new DDoubleGDL(dimension(nx),BaseGDL::INDGEN); Guard<BaseGDL> guardX;guardX.Reset(X);
////      //alloc interpolant object
////      gsl_interp * myinterp=gsl_interp_alloc (interp,nx);
////      //init source arrays 
////      int status=gsl_interp_init (myinterp, (const double*)X->DataAddr(), (const double*)V->DataAddr() , nx);
////      //acceleration
////      gsl_interp_accel * acc=gsl_interp_accel_alloc ();
////      //allocate result
////      DDoubleGDL* res=new DDoubleGDL(n,BaseGDL::NOZERO);
////      for (SizeT i=0; i< n; ++i) {
////        double x = i*(nx-1)/(n-1);
////       (*res)[i]=gsl_interp_eval(myinterp, (const double*)X->DataAddr() , (const double*) V->DataAddr(), x, acc);
////      }
////      gsl_interp_accel_free(acc);
////      gsl_interp_free(myinterp);
////      if (p0->Type() == GDL_DOUBLE) return res;
////      else {
////        BaseGDL* resf=res->Convert2(GDL_FLOAT, BaseGDL::COPY);
////        GDLDelete(res);
////        return resf;
////      }
//    } else {
//      BaseGDL* p1 = e->GetParDefined(1);
//      if (p1->N_Elements() != nx) e->Throw("V and X arrays must have same # of elements");
//      DDoubleGDL* X=e->GetParAs<DDoubleGDL>(1);
//      BaseGDL* p2 = e->GetParDefined(2);
//      SizeT n2=p2->N_Elements();
//      DType t = (DTypeOrder[ p0->Type()] > DTypeOrder[ p1->Type()]) ? p0->Type() : p1->Type();
//      t=(DTypeOrder[t] > DTypeOrder[ p2->Type()]) ? t : p2->Type();
//      DDoubleGDL* Xout=e->GetParAs<DDoubleGDL>(2);
//     //alloc interpolant object
//      gsl_interp * myinterp=gdl_interp_alloc (interp,nx);
//      //init source arrays 
//      int status=gdl_interp_init (myinterp, (const double*)X->DataAddr(), (const double*)V->DataAddr() , nx);
//      //acceleration
//      gdl_interp_accel * acc=gsl_interp_accel_alloc ();
//      //allocate result
//      DDoubleGDL* res=new DDoubleGDL(n2,BaseGDL::NOZERO);
//      //think about boundaries...
//      DLong minEl, maxEl;
//      X->MinMax( &minEl, &maxEl, NULL, NULL, true);
//      DDouble minX=(*X)[minEl];
//      DDouble maxX=(*X)[maxEl];
//      //trouble if values are asked at position that X does not sample: gsl will hang on an error. IDL is way cooler.
//      Xout->MinMax( &minEl, &maxEl, NULL, NULL, true);
//      DDouble minXout=(*Xout)[minEl];
//      DDouble maxXout=(*Xout)[maxEl];
////      bool good=((( minXout >= minX) && (minXout <= maxX)) && ((maxXout >= minX) && (maxXout <= maxX)));
////      if (good) {
//////          res=interpolate_1dim(e,gdl_interp1d_linear,V,X,false,0.0,0.0);
////
////        for (SizeT i = 0; i < n2; ++i) {
////          (*res)[i] = gsl_interp_eval(myinterp, (const double*) X->DataAddr(), (const double*) V->DataAddr(), (*Xout)[i], acc);
////        }
////      } else {
//        for (SizeT i = 0; i < n2; ++i) {
//          DDouble pos = (*Xout)[i];
//          if ((pos >= minX) && (pos <= maxX)) { //still good
//            (*res)[i] = gsl_interp_eval(myinterp, (const double*) X->DataAddr(), (const double*) V->DataAddr(), pos, acc);
//          } else {
//            if (interp == gsl_interp_linear) {
//                double  xb0 = (*X)[0];
//                double  yb0 = (*V)[0];
//                double ybinc = (*V)[1]-yb0;
//                double dbx = (*X)[1] - xb0;
//                double  xe0 = (*X)[nx-1];
//                double  ye0 = (*V)[nx-1];
//                double yeinc = (*V)[nx-2]-ye0;
//                double dex = (*X)[nx-2] - xe0;
//                if (pos < minX) {
//                  (*res)[i]=yb0 + (pos - xb0) / dbx * ybinc;
//              } else {
//                  (*res)[i]=ye0 + (pos - xe0) / dex * yeinc;
//              }
//            }
//          }
////        }
//      }
//      gsl_interp_accel_free(acc);
//      gsl_interp_free(myinterp);
//      if (t == GDL_DOUBLE) return res;
//      else {
//        BaseGDL* resf=res->Convert2(t, BaseGDL::COPY);
//        GDLDelete(res);
//        return resf;
//      }
//    }
//    e->Throw("Impossible.");
//    return NULL;
//  }

}
