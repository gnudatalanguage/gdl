#ifndef __INTERP_MULTID_H__
#define __INTERP_MULTID_H__

#include <gsl/gsl_interp.h>
#include <gsl/gsl_errno.h>

#ifdef __cplusplus
extern "C" {
#endif

  enum missing_mode {
    missing_NONE = 0,
    missing_NEAREST,
    missing_GIVEN
  };
 /* Gamma value for cubic interpolation and means to change it*/
  static double gdl_cubic_gamma=-1.0;
  void gdl_update_cubic_interpolation_coeff(double gammaValue)
  {
    gdl_cubic_gamma=-1;
    if (gammaValue <=0 && gammaValue >= -1.0) gdl_cubic_gamma=gammaValue;
  }

  /* convolutions available */
  double linConv(double d, double x0, double x1) {
    return (1. - d)*x0 + d*x1;
  }
//idem cubConvParam but with param=-1.0 - replaced by cubConv Below
//  double cubConv(double d, double x0, double x1, double x2, double x3) {
//    return (-x0 + x1 - x2 + x3)*pow(d, 3) +(2 * x0 - 2 * x1 + x2 - x3) * pow(d, 2) + (-x0 + x2) * d + x1;
//  }

  double cubConv(double d, double x0, double x1, double x2, double x3) {
    double g=gdl_cubic_gamma;
    return ((g + 2) * pow(d, 3) - (g + 3) * pow(d, 2) + 1)*x1 +
        ((g + 2) * pow((1 - d), 3) - (g + 3) * pow((1 - d), 2) + 1) * x2 +
        (g * pow((1 + d), 3) -5 * g * pow((1 + d), 2) + 8 * g * (1 + d) - 4 * g) * x0 +
        (g * pow((2 - d), 3) -5 * g * pow((2 - d), 2) + 8 * g * (2 - d) - 4 * g) * x3;
  }

  /* 1D */

  typedef struct {
    const char* name;
    unsigned int min_size;
    void* (*alloc)(size_t size);
    int (*init)(void*, const double xa[], const double ta[], size_t xsize);
    int (*eval)(const void*, const double xa[], const double ta[], size_t xsize, double x, gsl_interp_accel*, double* t);
    void (*free)(void*);
  } gdl_interp1d_type;

  typedef struct {
    const gdl_interp1d_type* type;
    double xmin;
    double xmax;
    size_t xsize;
    missing_mode mode;
    double missing;
    void* state;
  } gdl_interp1d;

  size_t gdl_interp1d_type_min_size(const gdl_interp1d_type* T) {
    return T->min_size;
  }

  size_t gdl_interp1d_min_size(const gdl_interp1d* interp) {
    return interp->type->min_size;
  }

  const char* gdl_interp1d_name(const gdl_interp1d* interp) {
    return interp->type->name;
  }

  GSL_VAR const gdl_interp1d_type* gdl_interp1d_linear;
  GSL_VAR const gdl_interp1d_type* gdl_interp1d_nearest;
  GSL_VAR const gdl_interp1d_type* gdl_interp1d_cubic;

  double gdl_interp1d_eval(const gdl_interp1d* interp, const double xarr[], const double tarr[], const double x, gsl_interp_accel* xa);

  gdl_interp1d* gdl_interp1d_alloc(const gdl_interp1d_type* T, size_t xsize) {
    gdl_interp1d* interp;
    //    protection removed since interpolators are protected against index overflow. Mimics IDL.
    //    if (xsize < T->min_size) {
    //        GSL_ERROR_NULL("insufficient number of points for interpolation type", GSL_EINVAL);
    //    }
    interp = (gdl_interp1d*) malloc(sizeof (gdl_interp1d));
    if (interp == NULL) {
      GSL_ERROR_NULL("failed to allocate space for gdl_interp1d struct", GSL_ENOMEM);
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
      GSL_ERROR_NULL("failed to allocate space for gdl_interp1d state", GSL_ENOMEM);
    }
    return interp;
  }

  int gdl_interp1d_init(gdl_interp1d* interp, const double xarr[], const double tarr[], size_t xsize, missing_mode mode, double missing, double gammaValue) {
    size_t i;
    if (xsize != interp->xsize) {
      GSL_ERROR("data must match size of interpolation object", GSL_EINVAL);
    }
    for (i = 1; i < xsize; i++) {
      if (xarr[i - 1] >= xarr[i]) {
        GSL_ERROR("x values must be strictly increasing", GSL_EINVAL);
      }
    }
    interp->xmin = xarr[0];
    interp->xmax = xarr[xsize - 1];
    interp->mode = mode;
    interp->missing = missing;
    gdl_update_cubic_interpolation_coeff(gammaValue);
    {
      int status = interp->type->init(interp->state, xarr, tarr, xsize);
      return status;
    }
  }

  void gdl_interp1d_free(gdl_interp1d* interp) {
    if (!interp) {
      return;
    }
    if (interp->type->free) {
      interp->type->free(interp->state);
    }
    free(interp);
  }

  double gdl_interp1d_eval(const gdl_interp1d* interp, const double xarr[], const double tarr[], const double x, gsl_interp_accel* xa) {
    double xx, t;
    xx = x;
    int status;
    switch (interp->mode) {
      case missing_NONE:
        if (xx < interp->xmin || xx >= interp->xmax) {
          GSL_ERROR_VAL("interpolation error", GSL_EDOM, GSL_NAN);
        }
        break;
      case missing_NEAREST:
        if (xx < interp->xmin) xx = interp->xmin;
        if (xx >= interp->xmax) xx = interp->xmax;
        break;
      case missing_GIVEN:
        if ((xx < interp->xmin) || (xx >= interp->xmax)) {
          return interp->missing;
        }
        break;
    }
    status = interp->type->eval(interp->state, xarr, tarr, interp->xsize, xx, xa, &t);
    if ((status) != GSL_SUCCESS) GSL_ERROR_VAL("interpolation error", (status), GSL_NAN);

    return t;
  }

  static int linear_init(void* state, const double xa[], const double ta[], size_t xsize) {
    return GSL_SUCCESS;
  }

  static int nearest_init(void* state, const double xa[], const double ta[], size_t xsize) {
    return GSL_SUCCESS;
  }

  static int cubic_init(void* state, const double xa[], const double ta[], size_t xsize) {
  return GSL_SUCCESS;
  }

  static int cubic_eval(const void* state, const double xarr[], const double tarr[], size_t xsize, double x, gsl_interp_accel* xa, double* t) {
    double xmin, xmax;
    double dx;
    double u;
    size_t xm, xi, xp, xp2;
    double kern[4];

    if (xa != NULL) {
      xi = gsl_interp_accel_find(xa, xarr, xsize, x);
    } else {
      xi = gsl_interp_bsearch(xarr, x, 0, xsize - 1);
    }
    xm = (xi > 0) ? xi - 1 : xi;
    xp = (xi + 1 < xsize) ? xi + 1 : xi;
    xp2 = (xp + 1 < xsize) ? xp + 1 : xp;

    kern[0] = tarr[xm];
    kern[1] = tarr[xi];
    kern[2] = tarr[xp];
    kern[3] = tarr[xp2];

    xmin = xarr[xi];
    xmax = xarr[xp];
    dx = xmax - xmin;
    u = (dx > 0.0) ? (x - xmin) / dx : 0.0;
    *t = cubConv(u, kern[0], kern[1], kern[2], kern[3]);

    return GSL_SUCCESS;
  }

  static int linear_eval(const void* state, const double xarr[], const double tarr[], size_t xsize, double x, gsl_interp_accel* xa, double* t) {
    double xmin, xmax;
    double dx;
    double u;
    size_t xi, xp;
    double kern[2];
    if (xa != NULL) {
      xi = gsl_interp_accel_find(xa, xarr, xsize, x);
    } else {
      xi = gsl_interp_bsearch(xarr, x, 0, xsize - 1);
    }
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

  static int nearest_eval(const void* state, const double xarr[], const double tarr[], size_t xsize, double x, gsl_interp_accel* xa, double* t) {
    size_t xmin;
    xmin = (x < 0) ? 0 : x;
    xmin = (xmin > xsize - 1) ? xsize - 1 : xmin;
    *t = tarr[xmin];
    return GSL_SUCCESS;
  }

  static const gdl_interp1d_type cubic_type = {
    "cubic",
    4,
    NULL,
    &cubic_init,
    &cubic_eval
  };

  static const gdl_interp1d_type linear_type = {
    "linear",
    2,
    NULL,
    &linear_init,
    &linear_eval
  };

  static const gdl_interp1d_type nearest_type = {
    "nearest",
    1,
    NULL,
    &nearest_init,
    &nearest_eval
  };

  const gdl_interp1d_type* gdl_interp1d_linear = &linear_type;
  const gdl_interp1d_type* gdl_interp1d_nearest = &nearest_type;
  const gdl_interp1d_type* gdl_interp1d_cubic = &cubic_type;

  /* 2D */

  /* column-major index (FORTRAN, IDL) */
#define INDEX_2D(xi, yi, xsize, ysize) (yi) * (xsize) + (xi)
  /* row-major index (C, C++) */

  /* #define INDEX_2D(xi, yi, xsize, ysize) (xi) * (ysize) + (yi)  */

  typedef struct {
    const char* name;
    unsigned int min_size;
    void* (*alloc)(size_t size);
    int (*init)(void*, const double xa[], const double ya[], const double ta[], size_t xsize, size_t ysize);
    int (*eval)(const void*, const double xa[], const double ya[], const double ta[], size_t xsize, size_t ysize, double x, double y, gsl_interp_accel*, gsl_interp_accel*, double* z);
    void (*free)(void*);
  } gdl_interp2d_type;

  typedef struct {
    const gdl_interp2d_type* type;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    size_t xsize;
    size_t ysize;
    missing_mode mode;
    double missing;
    void* state;
  } gdl_interp2d;

  size_t gdl_interp2d_type_min_size(const gdl_interp2d_type* T) {
    return T->min_size;
  }

  size_t gdl_interp2d_min_size(const gdl_interp2d* interp) {
    return interp->type->min_size;
  }

  const char* gdl_interp2d_name(const gdl_interp2d* interp) {
    return interp->type->name;
  }
  GSL_VAR const gdl_interp2d_type* gdl_interp2d_bilinear;
  GSL_VAR const gdl_interp2d_type* gdl_interp2d_binearest;
  GSL_VAR const gdl_interp2d_type* gdl_interp2d_bicubic;

  double gdl_interp2d_eval(const gdl_interp2d* interp, const double xarr[], const double yarr[], const double tarr[], const double x, const double y, gsl_interp_accel* xa, gsl_interp_accel* ya);

  gdl_interp2d* gdl_interp2d_alloc(const gdl_interp2d_type* T, size_t xsize, size_t ysize) {
    gdl_interp2d* interp;
    //    protection removed since interpolators are protected against index overflow. Mimics IDL.
    //    if (xsize < T->min_size || ysize < T->min_size) {
    //        GSL_ERROR_NULL("insufficient number of points for interpolation type", GSL_EINVAL);
    //    }
    interp = (gdl_interp2d*) malloc(sizeof (gdl_interp2d));
    if (interp == NULL) {
      GSL_ERROR_NULL("failed to allocate space for gdl_interp2d struct", GSL_ENOMEM);
    }
    interp->type = T;
    interp->xsize = xsize;
    interp->ysize = ysize;
    if (interp->type->alloc == NULL) {
      interp->state = NULL;
      return interp;
    }
    interp->state = interp->type->alloc(xsize);
    if (interp->state == NULL) {
      free(interp);
      GSL_ERROR_NULL("failed to allocate space for gdl_interp2d state", GSL_ENOMEM);
    }
    return interp;
  }

  int gdl_interp2d_init(gdl_interp2d* interp, const double xarr[], const double yarr[], const double tarr[], size_t xsize, size_t ysize, missing_mode mode, double missing, double gammaValue) {
    size_t i;
    if (xsize != interp->xsize || ysize != interp->ysize) {
      GSL_ERROR("data must match size of interpolation object", GSL_EINVAL);
    }
    for (i = 1; i < xsize; i++) {
      if (xarr[i - 1] >= xarr[i]) {
        GSL_ERROR("x values must be strictly increasing", GSL_EINVAL);
      }
    }
    for (i = 1; i < ysize; i++) {
      if (yarr[i - 1] >= yarr[i]) {
        GSL_ERROR("y values must be strictly increasing", GSL_EINVAL);
      }
    }
    interp->xmin = xarr[0];
    interp->xmax = xarr[xsize - 1];
    interp->ymin = yarr[0];
    interp->ymax = yarr[ysize - 1];
    interp->mode = mode;
    interp->missing = missing;
    gdl_update_cubic_interpolation_coeff(gammaValue);
   {
      int status = interp->type->init(interp->state, xarr, yarr, tarr, xsize, ysize);
      return status;
    }
  }

  void gdl_interp2d_free(gdl_interp2d* interp) {
    if (!interp) {
      return;
    }
    if (interp->type->free) {
      interp->type->free(interp->state);
    }
    free(interp);
  }

  double gdl_interp2d_eval(const gdl_interp2d* interp, const double xarr[], const double yarr[], const double tarr[], const double x, const double y, gsl_interp_accel* xa, gsl_interp_accel* ya) {
    double xx, yy, t;
    xx = x;
    yy = y;
    int status;
    switch (interp->mode) {
      case missing_NONE:
        if (xx < interp->xmin || xx > interp->xmax) {
          GSL_ERROR_VAL("interpolation error", GSL_EDOM, GSL_NAN);
        }
        if (yy < interp->ymin || yy > interp->ymax) {
          GSL_ERROR_VAL("interpolation error", GSL_EDOM, GSL_NAN);
        }
        break;
      case missing_NEAREST:
        if (xx < interp->xmin) xx = interp->xmin;
        if (xx > interp->xmax) xx = interp->xmax;
        if (yy < interp->ymin) yy = interp->ymin;
        if (yy > interp->ymax) yy = interp->ymax;
        break;
      case missing_GIVEN:
        if ((xx < interp->xmin) || (xx > interp->xmax) || (yy < interp->ymin) || (yy > interp->ymax)) {
          return interp->missing;
        }
    }
    status = interp->type->eval(interp->state, xarr, yarr, tarr, interp->xsize, interp->ysize, xx, yy, xa, ya, &t);
    if ((status) != GSL_SUCCESS) GSL_ERROR_VAL("interpolation error", (status), GSL_NAN);

    return t;
  }

  static int bilinear_init(void* state, const double xa[], const double ya[], const double ta[], size_t xsize, size_t ysize) {
    return GSL_SUCCESS;
  }

  static int binearest_init(void* state, const double xa[], const double ya[], const double ta[], size_t xsize, size_t ysize) {
    return GSL_SUCCESS;
  }

  static int bicubic_init(void* state, const double xa[], const double ya[], const double ta[], size_t xsize, size_t ysize) {
    return GSL_SUCCESS;
  }

  static int bicubic_eval(const void* state, const double xarr[], const double yarr[], const double tarr[], size_t xsize, size_t ysize, double x, double y, gsl_interp_accel* xa, gsl_interp_accel* ya, double* t) {
    double xmin, xmax, ymin, ymax;
    double dx, dy;
    double v, u;
    size_t xm, ym, xi, yi, xp, yp, xp2, yp2;
    double kern[4][4];
    double t1, t2, t3, t4;

    if (xa != NULL) {
      xi = gsl_interp_accel_find(xa, xarr, xsize, x);
    } else {
      xi = gsl_interp_bsearch(xarr, x, 0, xsize - 1);
    }
    xm = (xi > 0) ? xi - 1 : xi;
    xp = (xi + 1 < xsize) ? xi + 1 : xi;
    xp2 = (xp + 1 < xsize) ? xp + 1 : xp;
    if (ya != NULL) {
      yi = gsl_interp_accel_find(ya, yarr, ysize, y);
    } else {
      yi = gsl_interp_bsearch(yarr, y, 0, ysize - 1);
    }
    ym = (yi > 0) ? yi - 1 : yi;
    yp = (yi + 1 < ysize) ? yi + 1 : yi;
    yp2 = (yp + 1 < ysize) ? yp + 1 : yp;

    kern[0][0] = tarr[INDEX_2D(xm, ym, xsize, ysize)];
    kern[0][1] = tarr[INDEX_2D(xm, yi, xsize, ysize)];
    kern[0][2] = tarr[INDEX_2D(xm, yp, xsize, ysize)];
    kern[0][3] = tarr[INDEX_2D(xm, yp2, xsize, ysize)];

    kern[1][0] = tarr[INDEX_2D(xi, ym, xsize, ysize)];
    kern[1][1] = tarr[INDEX_2D(xi, yi, xsize, ysize)];
    kern[1][2] = tarr[INDEX_2D(xi, yp, xsize, ysize)];
    kern[1][3] = tarr[INDEX_2D(xi, yp2, xsize, ysize)];

    kern[2][0] = tarr[INDEX_2D(xp, ym, xsize, ysize)];
    kern[2][1] = tarr[INDEX_2D(xp, yi, xsize, ysize)];
    kern[2][2] = tarr[INDEX_2D(xp, yp, xsize, ysize)];
    kern[2][3] = tarr[INDEX_2D(xp, yp2, xsize, ysize)];

    kern[3][0] = tarr[INDEX_2D(xp2, ym, xsize, ysize)];
    kern[3][1] = tarr[INDEX_2D(xp2, yi, xsize, ysize)];
    kern[3][2] = tarr[INDEX_2D(xp2, yp, xsize, ysize)];
    kern[3][3] = tarr[INDEX_2D(xp2, yp2, xsize, ysize)];

    xmin = xarr[xi];
    xmax = xarr[xp];
    ymin = yarr[yi];
    ymax = yarr[yp];
    dx = xmax - xmin;
    dy = ymax - ymin;
    u = (dx > 0.0) ? (x - xmin) / dx : 0.0;
    v = (dy > 0.0) ? (y - ymin) / dy : 0.0;
    t1 = cubConv(u, kern[0][0], kern[1][0], kern[2][0], kern[3][0]);
    t2 = cubConv(u, kern[0][1], kern[1][1], kern[2][1], kern[3][1]);
    t3 = cubConv(u, kern[0][2], kern[1][2], kern[2][2], kern[3][2]);
    t4 = cubConv(u, kern[0][3], kern[1][3], kern[2][3], kern[3][3]);

    *t = cubConv(v, t1, t2, t3, t4);

    return GSL_SUCCESS;
  }

  static int bilinear_eval(const void* state, const double xarr[], const double yarr[], const double tarr[], size_t xsize, size_t ysize, double x, double y, gsl_interp_accel* xa, gsl_interp_accel* ya, double* t) {
    double xmin, xmax, ymin, ymax;
    double dx, dy;
    double v, u;
    size_t xi, yi, xp, yp;
    double kern[2][2];
    double t1, t2;

    if (xa != NULL) {
      xi = gsl_interp_accel_find(xa, xarr, xsize, x);
    } else {
      xi = gsl_interp_bsearch(xarr, x, 0, xsize - 1);
    }
    xp = (xi + 1 < xsize) ? xi + 1 : xi;

    if (ya != NULL) {
      yi = gsl_interp_accel_find(ya, yarr, ysize, y);
    } else {
      yi = gsl_interp_bsearch(yarr, y, 0, ysize - 1);
    }
    yp = (yi + 1 < ysize) ? yi + 1 : yi;

    kern[0][0] = tarr[INDEX_2D(xi, yi, xsize, ysize)];
    kern[0][1] = tarr[INDEX_2D(xi, yp, xsize, ysize)];

    kern[1][0] = tarr[INDEX_2D(xp, yi, xsize, ysize)];
    kern[1][1] = tarr[INDEX_2D(xp, yp, xsize, ysize)];

    xmin = xarr[xi];
    xmax = xarr[xp];
    ymin = yarr[yi];
    ymax = yarr[yp];
    dx = xmax - xmin;
    dy = ymax - ymin;
    u = (dx > 0.0) ? (x - xmin) / dx : 0.0;
    v = (dy > 0.0) ? (y - ymin) / dy : 0.0;
    t1 = linConv(u, kern[0][0], kern[1][0]);
    t2 = linConv(u, kern[0][1], kern[1][1]);

    *t = linConv(v, t1, t2);

    return GSL_SUCCESS;
  }

  static int binearest_eval(const void* state, const double xarr[], const double yarr[], const double tarr[], size_t xsize, size_t ysize, double x, double y, gsl_interp_accel* xa, gsl_interp_accel* ya, double* t) {
    size_t xmin, ymin;
    xmin = (x < 0) ? 0 : x;
    xmin = (xmin > xsize - 1) ? xsize - 1 : xmin;
    ymin = (y < 0) ? 0 : y;
    ymin = (ymin > ysize - 1) ? ysize - 1 : ymin;
    *t = tarr[INDEX_2D(xmin, ymin, xsize, ysize)];
    return GSL_SUCCESS;
  }

  static const gdl_interp2d_type bicubic_type = {
    "cubic",
    4,
    NULL,
    &bicubic_init,
    &bicubic_eval
  };

  static const gdl_interp2d_type bilinear_type = {
    "linear",
    2,
    NULL,
    &bilinear_init,
    &bilinear_eval
  };

  static const gdl_interp2d_type binearest_type = {
    "nearest",
    1,
    NULL,
    &binearest_init,
    &binearest_eval
  };

  const gdl_interp2d_type* gdl_interp2d_bilinear = &bilinear_type;
  const gdl_interp2d_type* gdl_interp2d_binearest = &binearest_type;
  const gdl_interp2d_type* gdl_interp2d_bicubic = &bicubic_type;

  /* 3D */

  /*column-major index (idl, fortran). Beware to keep all the parenthesis!*/
#define INDEX_3D(xi, yi, zi, xsize, ysize, zsize) ((zi) * (ysize) + (yi)) * (xsize) + (xi)
  /* row-major index (C, C++) */

  /* #define INDEX_3D(xi, yi, zi, xsize, ysize, zsize) ((xi) * (ysize) + (yi) ) * (zsize) + (zi) */

  typedef struct {
    const char* name;
    unsigned int min_size;
    void* (*alloc)(size_t size);
    int (*init)(void*, const double xa[], const double ya[], const double za[], const double ta[], size_t xsize, size_t ysize, size_t zsize);
    int (*eval)(const void*, const double xa[], const double ya[], const double za[], const double ta[], size_t xsize, size_t ysize, size_t zsize, double x, double y, double z, gsl_interp_accel*, gsl_interp_accel*, gsl_interp_accel*, double* t);
    void (*free)(void*);
  } gdl_interp3d_type;

  typedef struct {
    const gdl_interp3d_type* type;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
    size_t xsize;
    size_t ysize;
    size_t zsize;
    missing_mode mode;
    double missing;
    void* state;
  } gdl_interp3d;

  size_t gdl_interp3d_type_min_size(const gdl_interp3d_type* T) {
    return T->min_size;
  }

  size_t gdl_interp3d_min_size(const gdl_interp3d* interp) {
    return interp->type->min_size;
  }

  const char* gdl_interp3d_name(const gdl_interp3d* interp) {
    return interp->type->name;
  }

  GSL_VAR const gdl_interp3d_type* gdl_interp3d_trilinear;

  double gdl_interp3d_eval(const gdl_interp3d* interp, const double xarr[], const double yarr[], const double zarr[], const double tarr[], const double x, const double y, const double z, gsl_interp_accel* xa, gsl_interp_accel* ya, gsl_interp_accel* za);

  gdl_interp3d* gdl_interp3d_alloc(const gdl_interp3d_type* T, size_t xsize, size_t ysize, size_t zsize) {
    gdl_interp3d* interp;
    //    if (xsize < T->min_size || ysize < T->min_size || zsize < T->min_size) {
    //        GSL_ERROR_NULL("insufficient number of points for interpolation type", GSL_EINVAL);
    //    }
    interp = (gdl_interp3d*) malloc(sizeof (gdl_interp3d));
    if (interp == NULL) {
      GSL_ERROR_NULL("failed to allocate space for gdl_interp3d struct", GSL_ENOMEM);
    }
    interp->type = T;
    interp->xsize = xsize;
    interp->ysize = ysize;
    interp->zsize = zsize;
    if (interp->type->alloc == NULL) {
      interp->state = NULL;
      return interp;
    }
    interp->state = interp->type->alloc(xsize);
    if (interp->state == NULL) {
      free(interp);
      GSL_ERROR_NULL("failed to allocate space for gdl_interp3d state", GSL_ENOMEM);
    }
    return interp;
  }

  int gdl_interp3d_init(gdl_interp3d* interp, const double xarr[], const double yarr[], const double zarr[], const double tarr[], size_t xsize, size_t ysize, size_t zsize, missing_mode mode, double missing) {
    size_t i;
    if (xsize != interp->xsize || ysize != interp->ysize || zsize != interp->zsize) {
      GSL_ERROR("data must match size of interpolation object", GSL_EINVAL);
    }
    for (i = 1; i < xsize; i++) {
      if (xarr[i - 1] >= xarr[i]) {
        GSL_ERROR("x values must be strictly increasing", GSL_EINVAL);
      }
    }
    for (i = 1; i < ysize; i++) {
      if (yarr[i - 1] >= yarr[i]) {
        GSL_ERROR("y values must be strictly increasing", GSL_EINVAL);
      }
    }
    for (i = 1; i < zsize; i++) {
      if (zarr[i - 1] >= zarr[i]) {
        GSL_ERROR("z values must be strictly increasing", GSL_EINVAL);
      }
    }
    interp->xmin = xarr[0 ];
    interp->xmax = xarr[xsize - 1];
    interp->ymin = yarr[0];
    interp->ymax = yarr[ysize - 1];
    interp->zmin = zarr[0];
    interp->zmax = zarr[zsize - 1];
    interp->mode = mode;
    interp->missing = missing;
    {
      int status = interp->type->init(interp->state, xarr, yarr, zarr, tarr, xsize, ysize, zsize);
      return status;
    }
  }

  void gdl_interp3d_free(gdl_interp3d* interp) {
    if (!interp) {
      return;
    }
    if (interp->type->free) {
      interp->type->free(interp->state);
    }
    free(interp);
  }

  double gdl_interp3d_eval(const gdl_interp3d* interp, const double xarr[], const double yarr[], const double zarr[], const double tarr[], const double x, const double y, const double z, gsl_interp_accel* xa, gsl_interp_accel* ya, gsl_interp_accel* za) {
    double xx, yy, zz, t;
    xx = x;
    yy = y;
    zz = z;
    int status;
    switch (interp->mode) {
      case missing_NONE:
        if (xx < interp->xmin || xx > interp->xmax) {
          GSL_ERROR_VAL("interpolation error", GSL_EDOM, GSL_NAN);
        }
        if (yy < interp->ymin || yy > interp->ymax) {
          GSL_ERROR_VAL("interpolation error", GSL_EDOM, GSL_NAN);
        }
        if (zz < interp->zmin || zz > interp->zmax) {
          GSL_ERROR_VAL("interpolation error", GSL_EDOM, GSL_NAN);
        }
        break;
      case missing_NEAREST:
        if (xx < interp->xmin) xx = interp->xmin;
        if (xx > interp->xmax) xx = interp->xmax;
        if (yy < interp->ymin) yy = interp->ymin;
        if (yy > interp->ymax) yy = interp->ymax;
        if (zz < interp->zmin) zz = interp->zmin;
        if (zz > interp->zmax) zz = interp->zmax;
        break;
      case missing_GIVEN:
        if ((xx < interp->xmin) || (xx > interp->xmax) || (yy < interp->ymin) || (yy > interp->ymax) || (zz < interp->zmin) || (zz > interp->zmax)) {
          t = interp->missing;
          return t;
        }
    }
    status = interp->type->eval(interp->state, xarr, yarr, zarr, tarr, interp->xsize,
        interp->ysize, interp->zsize, xx, yy, zz, xa, ya, za, &t);
    if ((status) != GSL_SUCCESS) GSL_ERROR_VAL("interpolation error", (status), GSL_NAN);
    return t;
  }

  static int trilinear_init(void* state, const double xa[], const double ya[], const double za[], const double ta[], size_t xsize, size_t ysize, size_t zsize) {
    return GSL_SUCCESS;
  }

  static int trilinear_eval(const void* state, const double xarr[], const double yarr[], const double zarr[], const double tarr[], size_t xsize, size_t ysize, size_t zsize, double x, double y, double z, gsl_interp_accel* xa, gsl_interp_accel* ya, gsl_interp_accel* za, double* t) {
    double xmin, xmax, ymin, ymax, zmin, zmax;
    double dx, dy, dz;
    double u, v, w;
    size_t xi, xp, yi, yp, zi, zp;
    double kern[2][2];
    double t1, t2, t12, t21;
    if (xa != NULL) {
      xi = gsl_interp_accel_find(xa, xarr, xsize, x);
    } else {
      xi = gsl_interp_bsearch(xarr, x, 0, xsize - 1);
    }
    xp = (xi + 1 < xsize) ? xi + 1 : xi;

    if (ya != NULL) {
      yi = gsl_interp_accel_find(ya, yarr, ysize, y);
    } else {
      yi = gsl_interp_bsearch(yarr, y, 0, ysize - 1);
    }
    yp = (yi + 1 < ysize) ? yi + 1 : yi;
    if (za != NULL) {
      zi = gsl_interp_accel_find(za, zarr, zsize, z);
    } else {
      zi = gsl_interp_bsearch(zarr, z, 0, zsize - 1);
    }
    zp = (zi + 1 < zsize) ? zi + 1 : zi;

    xmin = xarr[xi];
    xmax = xarr[xp];
    ymin = yarr[yi];
    ymax = yarr[yp];
    zmin = zarr[zi];
    zmax = zarr[zp];
    dx = xmax - xmin;
    dy = ymax - ymin;
    dz = zmax - zmin;
    u = (dx > 0.0) ? (x - xmin) / dx : 0.0;
    v = (dy > 0.0) ? (y - ymin) / dy : 0.0;
    w = (dz > 0.0) ? (z - zmin) / dz : 0.0;

    kern[0][0] = tarr[INDEX_3D(xi, yi, zi, xsize, ysize, zsize)];
    kern[1][0] = tarr[INDEX_3D(xp, yi, zi, xsize, ysize, zsize)];
    kern[0][1] = tarr[INDEX_3D(xi, yp, zi, xsize, ysize, zsize)];
    kern[1][1] = tarr[INDEX_3D(xp, yp, zi, xsize, ysize, zsize)];

    t1 = linConv(u, kern[0][0], kern[1][0]);
    t2 = linConv(u, kern[0][1], kern[1][1]);
    t12 = linConv(v, t1, t2);

    kern[0][0] = tarr[INDEX_3D(xi, yi, zp, xsize, ysize, zsize)];
    kern[1][0] = tarr[INDEX_3D(xp, yi, zp, xsize, ysize, zsize)];
    kern[0][1] = tarr[INDEX_3D(xi, yp, zp, xsize, ysize, zsize)];
    kern[1][1] = tarr[INDEX_3D(xp, yp, zp, xsize, ysize, zsize)];

    t1 = linConv(u, kern[0][0], kern[1][0]);
    t2 = linConv(u, kern[0][1], kern[1][1]);
    t21 = linConv(v, t1, t2);

    *t = linConv(w, t12, t21);

    return GSL_SUCCESS;
  }

  static const gdl_interp3d_type trilinear_type = {
    "linear",
    2,
    NULL,
    &trilinear_init,
    &trilinear_eval
  };

  const gdl_interp3d_type* gdl_interp3d_trilinear = &trilinear_type;


#ifdef __cplusplus
}
#endif

#endif
