/***************************************************************************
                          gsl_fun.cpp  -  GDL GSL library function
                             -------------------
    begin                : Jan 20 2004
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net
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

//fx_root
#include <stdio.h>
#include <iostream>
#include <complex>

#include "datatypes.hpp"
#include "envt.hpp"
#include "basic_fun.hpp"
#include "gsl_fun.hpp"
#include "dinterpreter.hpp"

#ifdef _MSC_VER
#include "gtdhelper.hpp" //for gettimeofday()
#else

#include <sys/time.h>

#endif
// ms: must not be inlcuded here
//#include "libinit_ac.cpp"

#include <gsl/gsl_sys.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_real_float.h>
#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_halfcomplex_float.h>
#include <gsl/gsl_fft_complex_float.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_multimin.h>

// newton/broyden
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_vector.h>

// numerical integration (alternative to Qromb)
#include <gsl/gsl_integration.h>

// constant
#include <gsl/gsl_const_mksa.h>
#include <gsl/gsl_const_num.h>
#include <gsl/gsl_math.h>

#ifdef USE_UDUNITS
                                                                                                                        #  ifdef HAVE_UDUNITS2_UDUNITS2_H
#    include <udunits2/udunits2.h>
#  else
#    include <udunits2.h>
#  endif
#endif

// binomialcoef
#include <gsl/gsl_sf_gamma.h>

// wtn
#ifndef GSL_DISABLE_DEPRECATED
#  define GSL_DISABLE_DEPRECATED

#  include <gsl/gsl_wavelet.h>

#  undef GSL_DISABLE_DEPRECATED
#else
#  include <gsl/gsl_wavelet.h>
#endif

#include <gsl/gsl_wavelet2d.h>

// zeropoly
#include <gsl/gsl_poly.h>

// spher_harm
#include <gsl/gsl_sf_legendre.h>

#include "nullgdl.hpp"

//#include "gsl_errorhandler.hpp"


#define LOG10E 0.434294

#ifdef _MSC_VER
                                                                                                                        #define isnan _isnan
#define isfinite _finite
#endif

namespace lib {

    using namespace std;

#ifndef _MSC_VER
    using std::isnan;
#endif

    const int szdbl = sizeof(double);
    const int szflt = sizeof(float);

    class SetTemporaryGSLErrorHandlerT {
        gsl_error_handler_t *oldHandler;

    public:
        SetTemporaryGSLErrorHandlerT(gsl_error_handler_t *handler) {
            oldHandler = gsl_set_error_handler(handler);
        }

        ~SetTemporaryGSLErrorHandlerT() {
            gsl_set_error_handler(oldHandler);
        }

    };

    void GDLGenericGSLErrorHandler(const char *reason, const char *file, int line, int gsl_errno) {
        throw GDLException("GSL Error #" + i2s(gsl_errno) + ": " +
                           string(reason));// + "  file: " + file + "  line: " + i2s(line));
    }

    void SetGDLGenericGSLErrorHandler() {
        gsl_set_error_handler(GDLGenericGSLErrorHandler);
    }

    template<typename T1, typename T2>
    int cp2data2_template(BaseGDL *p0, T2 *data, SizeT nEl,
                          SizeT offset, SizeT stride_in, SizeT stride_out) {
        T1 *p0c = static_cast<T1 *>( p0);

        for (SizeT i = 0; i < nEl; ++i)
            data[2 * (i * stride_out + offset)] = (T2) (*p0c)[i * stride_in + offset];

        return 0;
    }


    template<typename T>
    int cp2data_template(BaseGDL *p0, T *data, SizeT nEl,
                         SizeT offset, SizeT stride_in, SizeT stride_out) {
        switch (p0->Type()) {
            case GDL_DOUBLE:
                cp2data2_template<DDoubleGDL, T>(p0, data, nEl, offset,
                                                 stride_in, stride_out);
                break;
            case GDL_FLOAT:
                cp2data2_template<DFloatGDL, T>(p0, data, nEl, offset,
                                                stride_in, stride_out);
                break;
            case GDL_LONG:
                cp2data2_template<DLongGDL, T>(p0, data, nEl, offset,
                                               stride_in, stride_out);
                break;
            case GDL_ULONG:
                cp2data2_template<DULongGDL, T>(p0, data, nEl, offset,
                                                stride_in, stride_out);
                break;
            case GDL_INT:
                cp2data2_template<DIntGDL, T>(p0, data, nEl, offset,
                                              stride_in, stride_out);
                break;
            case GDL_UINT:
                cp2data2_template<DUIntGDL, T>(p0, data, nEl, offset,
                                               stride_in, stride_out);
                break;
            case GDL_BYTE:
                cp2data2_template<DByteGDL, T>(p0, data, nEl, offset,
                                               stride_in, stride_out);
                break;
            default:
                std::cerr << "internal error, please report." << std::endl;
        }
        return 0;
    }


    template<typename T>
    int unpack_real_mxradix_template(T *dptr, SizeT nEl, double direct,
                                     SizeT offset, SizeT stride) {

        if (direct == -1) {
            for (SizeT i = 0; i < nEl; ++i) dptr[2 * (i * stride + offset)] /= nEl;
        }

        for (SizeT i = 1; i < nEl / 2 + (nEl % 2); ++i) {
            dptr[2 * stride * i + 1 + 2 * offset] = +dptr[2 * stride * 2 * i + 2 * offset];
            dptr[2 * stride * (nEl - i) + 1 + 2 * offset] = -dptr[2 * stride * 2 * i + 2 * offset];
        }

        for (SizeT i = 2; i < nEl / 2 + (nEl % 2); ++i) {
            dptr[2 * stride * i + 2 * offset] = +dptr[2 * stride * (2 * i - 1) + 2 * offset];
        }

        for (SizeT i = 2; i < nEl / 2 + (nEl % 2); ++i) {
            dptr[2 * stride * (nEl - i) + 2 * offset] = +dptr[2 * stride * i + 2 * offset];
        }

        dptr[1] = 0;
        if ((nEl % 2) == 0)
            dptr[stride * nEl + 2 * offset] = dptr[2 * stride * (nEl - 1) + 2 * offset];
        dptr[2 * stride * (nEl - 1) + 2 * offset] = dptr[2 * stride + 2 * offset];


        if (direct == +1) {
            for (SizeT i = 1; i < nEl; ++i)
                dptr[2 * (i * stride + offset) + 1] = -dptr[2 * (i * stride + offset) + 1];
        }
        return 0;
    }

    template<typename T, typename T1, typename T2>
    int real_fft_transform_template(BaseGDL *p0, T *dptr, SizeT nEl,
                                    double direct,
                                    SizeT offset, SizeT stride_in, SizeT stride_out,
                                    SizeT radix2,
                                    int (*complex_radix2_forward)
                                            (T[], const size_t, size_t),
                                    int (*complex_radix2_backward)
                                            (T[], const size_t, size_t),
                                    int (*real_transform)
                                            (T[], const size_t, size_t,
                                             const T1 *, T2 *),
                                    T1 *(*wavetable_alloc)(size_t),
                                    T2 *(*workspace_alloc)(size_t),
                                    void (*wavetable_free)(T1 *),
                                    void (*workspace_free)(T2 *)) {
        cp2data_template<T>(p0, dptr, nEl, offset, stride_in, stride_out);

        if (radix2) {

            if (direct == -1) {
                (*complex_radix2_forward)(&dptr[2 * offset], stride_out, nEl);
                for (SizeT i = 0; i < nEl; ++i)
                    ((std::complex <T> &) dptr[2 * (i * stride_out + offset)]) /= nEl;
            } else if (direct == +1) {
                (*complex_radix2_backward)(&dptr[2 * offset], stride_out, nEl);
            }
        } else if (!radix2) {

            T1 *wave;
            T2 *work;

            work = (*workspace_alloc)(nEl);
            GDLGuard<T2> g1(work, workspace_free);
            wave = (*wavetable_alloc)(nEl);
            GDLGuard<T1> g2(wave, wavetable_free);

            (*real_transform)(&dptr[2 * offset], 2 * stride_out, nEl, wave, work);

            unpack_real_mxradix_template<T>(dptr, nEl, direct, offset, stride_out);

            //       (*workspace_free) (work);
            //       (*wavetable_free) (wave);
        }
        return 0;
    }


    template<typename T, typename T1, typename T2>
    int complex_fft_transform_template(BaseGDL *p0, T *dptr, SizeT nEl,
                                       double direct,
                                       SizeT offset, SizeT stride, SizeT radix2,
                                       int (*complex_radix2_forward)
                                               (T[], const size_t, size_t),
                                       int (*complex_radix2_backward)
                                               (T[], const size_t, size_t),
                                       int (*complex_forward_transform)
                                               (T[], const size_t, size_t,
                                                const T1 *, T2 *),
                                       int (*complex_backward_transform)
                                               (T[], const size_t, size_t,
                                                const T1 *, T2 *),
                                       T1 *(*wavetable_alloc)(size_t),
                                       T2 *(*workspace_alloc)(size_t),
                                       void (*wavetable_free)(T1 *),
                                       void (*workspace_free)(T2 *)) {

        if (radix2) {

            if (direct == -1) {
                (*complex_radix2_forward)(&dptr[2 * offset], stride, nEl);

                for (SizeT i = 0; i < nEl; ++i)
                    ((std::complex <T> &) dptr[2 * (i * stride + offset)]) /= nEl;
            } else if (direct == +1) {
                (*complex_radix2_backward)(&dptr[2 * offset], stride, nEl);
            }
        } else if (!radix2) {

            T1 *wave;
            T2 *work;

            work = (*workspace_alloc)(nEl);
            GDLGuard<T2> g1(work, workspace_free);
            wave = (*wavetable_alloc)(nEl);
            GDLGuard<T1> g2(wave, wavetable_free);

            if (direct == -1) {
                (*complex_forward_transform)(&dptr[2 * offset], stride, nEl,
                                             wave, work);
                for (SizeT i = 0; i < nEl; ++i)
                    ((std::complex <T> &) dptr[2 * stride * i + 2 * offset]) /= nEl;

            } else if (direct == +1) {
                (*complex_backward_transform)(&dptr[2 * offset], stride, nEl,
                                              wave, work);
            }

            //       (*workspace_free) (work);
            //       (*wavetable_free) (wave);
        }
        return 0;
    }

    template<typename T>
    T *fft_template(EnvT *e, BaseGDL *p0,
                    SizeT nEl, SizeT dbl, SizeT overwrite,
                    double direct, DLong dimension) {
        SizeT offset;
        SizeT stride = 1;

        T *res;
        T *tabtemp = new T(p0->Dim());
        Guard<T> tabtempGuard(tabtemp);

        Guard<T> resGuard;
        if (overwrite == 0) {
            res = new T(p0->Dim(), BaseGDL::ZERO);
            resGuard.Reset(res);
        } else {
            res = (T *) p0;
            if (e->GlobalPar(0))
                e->SetPtrToReturnValue(&e->GetPar(0));
        }

        DComplexGDL *tabfft = new DComplexGDL(p0->Dim());
        Guard<DComplexGDL> tabfftGuard(tabfft);

        DComplexGDL *p0C = static_cast<DComplexGDL *>
        (p0->Convert2(GDL_COMPLEX, BaseGDL::COPY));
        Guard<DComplexGDL> p0CGuard(p0C);

        int dec = 0;
        int temp = 0;
        int flag = 0;
        int l = 0;

        int tab[MAXRANK];
        for (int y = 0; y < tabfft->Rank(); y++)
            tab[y] = 0;

        if (dimension >= 0) {
            // contact for this part (dim > 0) is Mathieu P. or Alain C.
            // Debut demontage tab

            l = 0;
            for (int j = 0; j < nEl / tabfft->Dim(dimension); j++) {
                dec = 0;
                flag = 0;
                for (int n = 0; n < tabfft->Rank(); n++) {
                    if (tab[n] != tabfft->Dim(n) - 1 && flag == 0 && n != dimension && l != 0) {
                        tab[n]++;
                        //cout << "tab[" << n << "] = " << tab[n] << endl;
                        flag = 1;
                    } else if (tab[n] == tabfft->Dim(n) - 1 && flag == 0 && n != dimension && l != 0)
                        tab[n] = 0;

                    temp = 1;
                    if (n != 0) {
                        for (int m = n - 1; m >= 0; m--) {
                            temp = temp * tabfft->Dim(m);
                        }
                    }
                    //cout << "temp = " << temp << endl;
                    dec = dec + tab[n] * temp;
                    //cout << "dec = " << dec << endl;
                }

                temp = 1;
                for (int y = dimension - 1; y >= 0; y--) {
                    temp = temp * tabfft->Dim(y);
                }
                for (int i = 0; i < tabfft->Dim(dimension); i++) {
                    (*tabfft)[l] = (*p0C)[dec + i * temp];
                    //cout << l << "=" << dec+i*temp << endl;
                    l++;
                }
            }

            // Fin demontage tab - Debut res

            temp = 1;
            for (int y = 0; y < tabfft->Rank(); y++) {
                if (y != dimension)
                    temp = temp * tabfft->Dim(y);
            }

            for (int i = 0; i < temp; i++) {
                offset = i * tabfft->Dim(dimension);
                fft_1d(tabfft, &(*tabtemp)[0], tabfft->Dim(dimension), offset, stride,
                       direct, dbl, 1);
            }

            // Fin res - Debut remontage

            for (int y = 0; y < tabfft->Rank(); y++)
                tab[y] = 0;

            l = 0;
            for (int j = 0; j < nEl / tabfft->Dim(dimension); j++) {
                dec = 0;
                flag = 0;
                for (int n = 0; n < tabfft->Rank(); n++) {
                    if (tab[n] != tabfft->Dim(n) - 1 && flag == 0 && n != dimension && l != 0) {
                        tab[n]++;
                        //cout << "tab[" << n << "] = " << tab[n] << endl;
                        flag = 1;
                    } else if (tab[n] == tabfft->Dim(n) - 1 && flag == 0 && n != dimension && l != 0)
                        tab[n] = 0;

                    temp = 1;
                    if (n != 0) {
                        for (int m = n - 1; m >= 0; m--) {
                            temp = temp * tabfft->Dim(m);
                        }
                    }
                    //cout << "temp = " << temp << endl;
                    dec = dec + tab[n] * temp;
                    //cout << "dec = " << dec << endl;
                }

                temp = 1;
                for (int y = dimension - 1; y >= 0; y--) {
                    temp = temp * tabfft->Dim(y);
                }
                for (int i = 0; i < tabfft->Dim(dimension); i++) {
                    (*res)[dec + i * temp] = (*tabtemp)[l];
                    //cout << l << "=" << dec+i*temp << endl;
                    l++;
                }
            }
        } else {
            dimension = 0;

            if (p0->Rank() == 1) {
                offset = 0;
                stride = 1;

                fft_1d(p0, &(*res)[0], nEl, offset, stride,
                       direct, dbl, dimension);

            } else if (p0->Rank() == 2) {
                stride = p0->Dim(0);
                for (SizeT i = 0; i < p0->Dim(0); ++i) {
                    fft_1d(p0, &(*res)[0], p0->Dim(1), i, stride,
                           direct, dbl, dimension);
                }
                for (SizeT i = 0; i < p0->Dim(1); ++i) {
                    fft_1d(res, &(*res)[0],
                           p0->Dim(0), i * p0->Dim(0), 1,
                           direct, dbl, dimension);
                }
            } else if (p0->Rank() >= 3) {
                unsigned char *used = new unsigned char[nEl];
                ArrayGuard<unsigned char> usedGuard(used);

                stride = nEl;
                for (SizeT i = p0->Rank(); i < nEl; ++i) used[i] = 0;

                for (SizeT k = p0->Rank(); k > 0; --k) {
                    for (SizeT i = 0; i < nEl; ++i) used[i] = 0;
                    stride /= p0->Dim(k - 1);

                    SizeT cnt = 1;
                    offset = 0;
                    while (cnt <= nEl / p0->Dim(k - 1)) {
                        if (used[offset] != 1) {
                            cnt++;
                            for (SizeT i = 0; i < p0->Dim(k - 1); ++i)
                                used[offset + i * stride] = 1;
                            if (k == p0->Rank())
                                fft_1d(p0, &(*res)[0], p0->Dim(k - 1), offset, stride,
                                       direct, dbl, dimension);
                            else
                                fft_1d(res, &(*res)[0], p0->Dim(k - 1), offset, stride,
                                       direct, dbl, dimension);
                        }
                        offset++;
                    }
                }
                // 	  delete used;
            }
        }

        resGuard.release();
        return res;
    }


    BaseGDL *fft_fun(EnvT *e) {
        static bool warning_done = false;
        /*
      Program Flow
      ------------
      fft_fun
      fft_template
      fft_1d
      (real/complex)_fft_transform_template
      cp2data_template (real only)
      cp2data_2_template (real only)
    */

        SizeT nParam = e->NParam(1);
        SizeT overwrite = 0, dbl = 0;
        SizeT stride;
        SizeT offset;

        double direct = -1.0;

        //BaseGDL* p0 = e->GetNumericArrayParDefined( 0);
        BaseGDL *p0 = e->GetParDefined(0);

        SizeT nEl = p0->N_Elements();
        if (nEl == 0)
            e->Throw(
                    "Variable is undefined: " + e->GetParString(0));


        if (nParam == 2) {
            BaseGDL *p1 = e->GetPar(1);
            if (p1->N_Elements() > 1)
                e->Throw(
                        "Expression must be a scalar or 1 element array: "
                        + e->GetParString(1));


            DDoubleGDL *direction =
                    static_cast<DDoubleGDL *>(p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));
            direct = GSL_SIGN((*direction)[0]);
        }


        if (e->KeywordSet(0)) dbl = 1;
        if (e->KeywordSet(1)) direct = +1.0;
        if (e->KeywordSet(2)) overwrite = 1;
        if (e->KeywordSet(4) && !warning_done) {
            warning_done = true;
            cerr << "Warning, keyword CENTER ignored, fixme!" << endl; //(recenter not handled here)
        }
        // Check for dimension keyword
        DLong dimension = 0;  // the general case

        static int DimEnvIx = e->KeywordIx("DIMENSION");
        if (e->KeywordSet(DimEnvIx)) {

            BaseGDL *DimOfDim = e->GetKW(DimEnvIx);
            if (DimOfDim->N_Elements() > 1)
                e->Throw("Expression must be a scalar or 1 element array in this context:");

            e->AssureLongScalarKW(DimEnvIx, dimension);
            if ((dimension < 0) || (dimension > p0->Rank())) {
                e->Throw("Illegal keyword value for DIMENSION.");
            }
        }
        // AC 07/09/2012: Mathieu did it like that and we checked !
        // in fact, here dimension should always be >=0
        dimension--;

        if (p0->Type() == GDL_COMPLEXDBL || p0->Type() == GDL_DOUBLE || dbl) {

            //cout << "if 1" << endl;
            if (overwrite)
                e->StealLocalPar(0); // only steals if local par
            // 		e->StealLocalParUndefGlobal(0);

            // AC 10-09-2012: temporary fix
            dbl = 1;

            return fft_template<DComplexDblGDL>(e, p0, nEl, dbl, overwrite,
                                                direct, dimension);

        } else if (p0->Type() == GDL_COMPLEX) {

            //cout << "if 2" << endl;
            DComplexGDL *res;

            if (overwrite)
                e->StealLocalPar(0); // only steals if local par
            // 		e->StealLocalParUndefGlobal(0);

            return fft_template<DComplexGDL>(e, p0, nEl, dbl, overwrite,
                                             direct, dimension);

        } else if (p0->Type() == GDL_FLOAT ||
                   p0->Type() == GDL_LONG ||
                   p0->Type() == GDL_ULONG ||
                   p0->Type() == GDL_INT ||
                   p0->Type() == GDL_UINT ||
                   p0->Type() == GDL_BYTE) {

            //cout << "if 3" << endl;
            overwrite = 0;
            return fft_template<DComplexGDL>(e, p0, nEl, dbl, overwrite,
                                             direct, dimension);

        } else {
            //cout << "else" << endl;

            overwrite = 0;
            DComplexGDL *p0C = static_cast<DComplexGDL *>
            (p0->Convert2(GDL_COMPLEX, BaseGDL::COPY));
            Guard<BaseGDL> guard_p0C(p0C);
            return fft_template<DComplexGDL>(e, p0C, nEl, dbl, overwrite,
                                             direct, dimension);

        }
    }


    int fft_1d(BaseGDL *p0, void *data, SizeT nEl, SizeT offset, SizeT stride,
               double direct, SizeT dbl, DLong dimension) {
        float f32[2];
        double f64[2];

        // Determine if radix2
        //[gsl_fun.cpp:692]: (error) Shifting signed 32-bit value by 31 bits is undefined behaviour
        //[gsl_fun.cpp:692]: (error) Signed integer overflow for expression '2<<i'.
        SizeT radix2 = 0;
        for (SizeT i = 0; i < 32; ++i) {
            if (nEl == (2 << i)) {
                radix2 = 1;
                break;
            }
        }

        // Determine input stride
        SizeT stride_in = 1;
        if (dimension > 0)
            for (SizeT i = 0; i < dimension - 1; ++i) stride_in *= p0->Dim(i);
        else
            stride_in = stride;

        if (p0->Type() == GDL_COMPLEX && dbl == 0) {
            DComplexGDL *p0C = static_cast<DComplexGDL *>( p0);
            float *dptr;
            dptr = (float *) data;

            if (dimension > 0) {
                for (SizeT i = 0; i < nEl; ++i) {
                    memcpy(&dptr[2 * (i * stride + offset)],
                           &(*p0C)[i * stride_in + offset], szflt * 2);
                }
            } else {
                // NO dimension Keyword
                if (stride == 1 && offset == 0) {
                    if ((void *) dptr != (void *) &(*p0C)[0])
                        memcpy(dptr, &(*p0C)[0], szflt * 2 * nEl);
                } else {
                    for (SizeT i = 0; i < nEl; ++i) {
                        memcpy(&dptr[2 * (i * stride + offset)],
                               &(*p0C)[i * stride + offset], szflt * 2);
                    }
                }
            }

            complex_fft_transform_template<float,
                    gsl_fft_complex_wavetable_float,
                    gsl_fft_complex_workspace_float>
                    (p0, dptr, nEl, direct, offset, stride, radix2,
                     gsl_fft_complex_float_radix2_forward,
                     gsl_fft_complex_float_radix2_backward,
                     gsl_fft_complex_float_forward,
                     gsl_fft_complex_float_backward,
                     gsl_fft_complex_wavetable_float_alloc,
                     gsl_fft_complex_workspace_float_alloc,
                     gsl_fft_complex_wavetable_float_free,
                     gsl_fft_complex_workspace_float_free);

            return 0;
        } else if (p0->Type() == GDL_COMPLEXDBL ||
                   (p0->Type() == GDL_COMPLEX && dbl)) {
            DComplexDblGDL *p0C = static_cast<DComplexDblGDL *>( p0);
            DComplexGDL *p0CF = static_cast<DComplexGDL *>( p0);

            double *dptr;
            dptr = (double *) data;

            if (p0->Type() == GDL_COMPLEXDBL) {
                for (SizeT i = 0; i < nEl; ++i) {
                    memcpy(&dptr[2 * (i * stride + offset)],
                           &(*p0C)[i * stride_in + offset], szdbl * 2);
                }
            } else if (p0->Type() == GDL_COMPLEX) {
                DComplexDbl c128;

                for (SizeT i = 0; i < nEl; ++i) {
                    c128 = (*p0CF)[i * stride_in + offset];
                    memcpy(&dptr[2 * (i * stride + offset)], &c128, 2 * szdbl);
                }
            }

            complex_fft_transform_template<double,
                    gsl_fft_complex_wavetable,
                    gsl_fft_complex_workspace>
                    (p0, dptr, nEl, direct, offset, stride, radix2,
                     gsl_fft_complex_radix2_forward,
                     gsl_fft_complex_radix2_backward,
                     gsl_fft_complex_forward,
                     gsl_fft_complex_backward,
                     gsl_fft_complex_wavetable_alloc,
                     gsl_fft_complex_workspace_alloc,
                     gsl_fft_complex_wavetable_free,
                     gsl_fft_complex_workspace_free);

            return 0;
        } else if (p0->Type() == GDL_DOUBLE || dbl) {
            double *dptr;
            dptr = (double *) data;

            real_fft_transform_template<double,
                    gsl_fft_real_wavetable,
                    gsl_fft_real_workspace>
                    (p0, dptr, nEl, direct, offset, stride_in, stride, radix2,
                     gsl_fft_complex_radix2_forward,
                     gsl_fft_complex_radix2_backward,
                     gsl_fft_real_transform,
                     gsl_fft_real_wavetable_alloc,
                     gsl_fft_real_workspace_alloc,
                     gsl_fft_real_wavetable_free,
                     gsl_fft_real_workspace_free);

            //	printf("real_fft_transform_template\n");

            return 0;
        } else if (p0->Type() == GDL_FLOAT ||
                   p0->Type() == GDL_LONG ||
                   p0->Type() == GDL_ULONG ||
                   p0->Type() == GDL_INT ||
                   p0->Type() == GDL_UINT ||
                   p0->Type() == GDL_BYTE) {
            float *dptr;
            dptr = (float *) data;

            real_fft_transform_template<float,
                    gsl_fft_real_wavetable_float,
                    gsl_fft_real_workspace_float>
                    (p0, dptr, nEl, direct, offset, stride_in, stride, radix2,
                     gsl_fft_complex_float_radix2_forward,
                     gsl_fft_complex_float_radix2_backward,
                     gsl_fft_real_float_transform,
                     gsl_fft_real_wavetable_float_alloc,
                     gsl_fft_real_workspace_float_alloc,
                     gsl_fft_real_wavetable_float_free,
                     gsl_fft_real_workspace_float_free);

            //	printf("real_fft_transform_template (float)\n");

            return 0;
        }
        assert(false);
        return 0;
    }

    /* following are modified codes taken from the GNU Scientific Library (gauss.c)
   *
   * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2006, 2007 James Theiler, Brian Gough
   * Copyright (C) 2006 Charles Karney
   *
   * This program is free software; you can redistribute it and/or modify
   * it under the terms of the GNU General Public License as published by
   * the Free Software Foundation; either version 3 of the License, or (at
   * your option) any later version.
   *
   * This program is distributed in the hope that it will be useful, but
   * WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   * General Public License for more details.
   *
   * You should have received a copy of the GNU General Public License
   * along with this program; if not, write to the Free Software
   * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
   */
    inline double high_prec_gsl_rng_uniform_pos_d(const gsl_rng *r) {
        unsigned long A, B;
        long double C;
        A = gsl_rng_uniform_pos(r) * 0xFFFFFFFFUL;
        B = gsl_rng_uniform_pos(r) * 0xFFFFFFFFUL;
        A = (A >> 5);
        B = (B >> 6);
        C = A * pow(2, 26) + B;
        return C * pow(2, -53);
    }

    float modified_gsl_ran_gaussian_f(const gsl_rng *r, const double sigma, bool reset = false) {
        //modified from GSL code to use the trick described in NumRec, that is,
        //use also the angle of the 'draw" as a no-cost random variable.
        //This trick is used by IDL.
        //The reset is used to start a new sequence (could probably be done looking at r contents)
        static int available = 0;
        if (reset) {
            available = 0;
            return std::numeric_limits<float>::quiet_NaN(); //ensure not used.
        }
        static float other;
        float x, y, r2;
        if (available == 0) {
            do {
                /* choose x,y in uniform square (-1,-1) to (+1,+1) */
                x = -1 + 2 * gsl_rng_uniform_pos(r);
                y = -1 + 2 * gsl_rng_uniform_pos(r);

                /* see if it is in the unit circle */
                r2 = x * x + y * y;
            } while (r2 > 1.0 || r2 == 0);

            /* Box-Muller transform */
            double fct = sqrt(-2.0 * log(r2) / r2);
            float current = sigma * y * fct;
            other = sigma * x * fct;
            available = 1;
            return current;
        } else {
            available = 0;
            return other;
        }
    }

    double modified_gsl_ran_gaussian_d(const gsl_rng *r, const double sigma, bool reset = false) {
        //modified from GSL code to use the trick described in NumRec, that is,
        //use also the angle of the 'draw" as a no-cost random variable.
        //This trick is used by IDL.
        //Moreover, IDL for doubles eats 2 single-precision numbers so that the result is
        // randomn_double = [(A >> 5)*226 + (B >> 6)]*2-53 where A and B are
        // 2 integer 32 bits random numbers.
        //The reset is used to start a new sequence (could probably be done looking at r contents)
        //GSL uses uniform_pos but the algo should permit x==0 or y==0 ?
        static int available = 0;
        if (reset) {
            available = 0;
            return std::numeric_limits<double>::quiet_NaN(); //ensure not used.
        }
        static double other;
        double x, y, r2;
        if (available == 0) {
            do {
                /* choose x,y in uniform square (-1,-1) to (+1,+1) */
                x = -1 + 2 * high_prec_gsl_rng_uniform_pos_d(r);
                y = -1 + 2 * high_prec_gsl_rng_uniform_pos_d(r);
                /* see if it is in the unit circle */
                r2 = x * x + y * y;
            } while (r2 > 1.0 || r2 == 0);

            /* Box-Muller transform */
            double fct = sqrt(-2.0 * log(r2) / r2);
            double current = sigma * y * fct;
            other = sigma * x * fct;
            available = 1;
            return current;
        } else {
            available = 0;
            return other;
        }
    }

    //template uses gsl, certified to give identical results to IDL8+. This is SLOW and not the default.

    template<typename T1, typename T2>
    int random_gamma(T1 *res, gsl_rng *gsl_rng_mem, dimension dim, DLong n) {
        SizeT nEl = res->N_Elements();
        for (SizeT i = 0; i < nEl; ++i)
            (*res)[i] =
                    (T2) gsl_ran_gamma_knuth(gsl_rng_mem, 1.0 * n,
                                             1.0); //differs from idl above gamma=6. ?//IDL says it's the Knuth algo used.
        return 0;
    }

    template<typename T1, typename T2>
    int random_binomial(T1 *res, gsl_rng *gsl_rng_mem, dimension dim, DDoubleGDL *binomialKey) {
        SizeT nEl = res->N_Elements();
        //Note: Binomial values are not same IDL.
        DULong n = (DULong) (*binomialKey)[0];
        DDouble p = (DDouble) (*binomialKey)[1];
        for (SizeT i = 0; i < nEl; ++i) (*res)[i] = (T2) gsl_ran_binomial_knuth(gsl_rng_mem, p, n);
        return 0;
    }

    template<typename T1, typename T2>
    int random_poisson(T1 *res, gsl_rng *gsl_rng_mem, dimension dim, DDoubleGDL *poissonKey) {
        SizeT nEl = res->N_Elements();
        //Removed old code that would return non-integer values for high mu values.
        DDouble mu = (DDouble) (*poissonKey)[0];
        for (SizeT i = 0; i < nEl; ++i) (*res)[i] = (T2) gsl_ran_poisson(gsl_rng_mem, mu);
        return 0;
    }

    template<typename T1, typename T2>
    int random_uniform(T1 *res, gsl_rng *gsl_rng_mem, dimension dim) {
        SizeT nEl = res->N_Elements();

        if (sizeof(T2) == sizeof(float)) {
            for (SizeT i = 0; i < nEl; ++i) (*res)[i] = (T2) gsl_rng_uniform(gsl_rng_mem);
            return 0;
        } else {
            //as for IDL, make a more precise random number from 2 successive ones:
            unsigned long A, B;
            long double C;
            for (SizeT i = 0; i < nEl; ++i) {
                A = gsl_rng_uniform(gsl_rng_mem) * 0xFFFFFFFFUL;
                B = gsl_rng_uniform(gsl_rng_mem) * 0xFFFFFFFFUL;
                A = (A >> 5);
                B = (B >> 6);
                C = A * pow(2, 26) + B;
                C = C * pow(2, -53);
                (*res)[i] = (T2) C; //gives the same as IDL 8
            }
            return 0;
        }
    }

    template<typename T1, typename T2>
    int random_normal(T1 *res, gsl_rng *gsl_rng_mem, dimension dim) {
        SizeT nEl = res->N_Elements();
        if (sizeof(T2) == sizeof(float)) {
            for (SizeT i = 0; i < nEl; ++i)
                (*res)[i] = (T2) modified_gsl_ran_gaussian_f(gsl_rng_mem, 1.0); //does reproduct IDL values.
            modified_gsl_ran_gaussian_f(gsl_rng_mem, 1.0,
                                        true); //reset use of internal cache in the modified_gsl_ran_gaussian function.
            return 0;
        } else {
            for (SizeT i = 0; i < nEl; ++i)
                (*res)[i] = (T2) modified_gsl_ran_gaussian_d(gsl_rng_mem, 1.0); //does reproduct IDL values.
            modified_gsl_ran_gaussian_d(gsl_rng_mem, 1.0,
                                        true); //reset use of internal cache in the modified_gsl_ran_gaussian function.
        }
        return 0;
    }

#define MERSENNE_GSL_N 624   /* Period parameters */

    typedef struct {
        unsigned long mt[MERSENNE_GSL_N];
        int mti;
    } mt_state_t;

    void set_random_state(gsl_rng *r, const unsigned long int *seed, const int pos, const int n) {
        assert(n == MERSENNE_GSL_N);
        mt_state_t *state = (mt_state_t *) (r->state);
        unsigned long *mt = state->mt;
        for (int i = 0; i < n; ++i) mt[i] = seed[i];
        state->mti = pos;
    }

    void get_random_state(EnvT *e, const gsl_rng *r, const DULong seed) {
        if (e->GlobalPar(0)) {
            int pos;
            mt_state_t *mt_state = (mt_state_t *) (r->state);
            pos = mt_state->mti;
            unsigned long int *state = mt_state->mt;
            DULongGDL *ret = new DULongGDL(dimension(MERSENNE_GSL_N + 4),
                                           BaseGDL::ZERO); //ZERO as not all elements are initialized here
            DULong *newstate = (DULong *) (ret->DataAddr());
            newstate[0] = seed;
            newstate[1] = pos;
            for (int i = 0; i < MERSENNE_GSL_N; ++i) newstate[i + 2] = state[i];
            e->SetPar(0, ret);
        }
    }

    //GSL version of random_fun. See randomgenerators.cpp

    BaseGDL *random_fun_gsl(EnvT *e) {

        //used in RANDOMU and RANDOMN, which share the SAME KEYLIST. It is safe to speed up by using static ints KeywordIx.
        //Note: LONG or ULONG are obeyed irrespectively of the presence of GAMMA etc which are ignored.
        static int LONGIx = e->KeywordIx("LONG");
        static int ULONGIx = e->KeywordIx("ULONG");
        static int GAMMAIx = e->KeywordIx("GAMMA");
        static int BINOMIALIx = e->KeywordIx("BINOMIAL");
        static int NORMALIx = e->KeywordIx("NORMAL");
        static int POISSONIx = e->KeywordIx("POISSON");
        static int UNIFORMIx = e->KeywordIx("UNIFORM");
        // testing Exclusive Keywords ...
        int exclusiveKW = e->KeywordPresent(GAMMAIx);
        exclusiveKW = exclusiveKW + e->KeywordPresent(BINOMIALIx);
        exclusiveKW = exclusiveKW + e->KeywordPresent(NORMALIx);
        exclusiveKW = exclusiveKW + e->KeywordPresent(POISSONIx);
        exclusiveKW = exclusiveKW + e->KeywordPresent(UNIFORMIx);

        if (exclusiveKW > 1) e->Throw("Conflicting keywords.");
        //idem for LONG and ULONG at the same time!
        exclusiveKW = e->KeywordPresent(LONGIx);
        exclusiveKW = exclusiveKW + e->KeywordPresent(ULONGIx);
        if (exclusiveKW > 1) e->Throw("Conflicting keywords.");

        // the generator structure
        static gsl_rng *gsl_rng_mem = gsl_rng_alloc(gsl_rng_mt19937);

        SizeT nParam = e->NParam(1);

        dimension dim;
        if (nParam > 1) arr(e, dim, 1);

        DULong seed;
        bool initialized = false;

        bool isAnull = NullGDL::IsNULLorNullGDL(e->GetPar(0));
        if (!isAnull) {
            DULongGDL *p0L = e->IfDefGetParAs<DULongGDL>(0);
            if (p0L != NULL) // some non-null value passed -> can be a seed state, 628 integers, or use first value:
            {
                // IDL does not check that the seed sequence has been changed: as long as it is a 628 element Ulong, it takes it
                // and use it as the current sequence (try with "all zeroes").
                if (p0L->N_Elements() == MERSENNE_GSL_N + 4 && p0L->Type() == GDL_ULONG) { //a (valid?) seed sequence
                    seed = (*p0L)[0];
                    int pos = (*p0L)[1];
                    int n = MERSENNE_GSL_N;
                    unsigned long int sequence[n];
                    for (int i = 0; i < n; ++i) sequence[i] = (unsigned long int) (*p0L)[i + 2];
                    set_random_state(gsl_rng_mem, sequence, pos, n); //the seed
                    initialized = true;
                } else { // not a seed sequence: take first (IDL does more than this...)
                    if (p0L->N_Elements() >= 1) {
                        seed = (*p0L)[0];
                        gsl_rng_set(gsl_rng_mem, seed);
                        initialized = true;
                    }
                }
            }
        }

        if (!initialized) {
            struct timeval tval;
            struct timezone tzone;
            gettimeofday(&tval, &tzone);
            long long int tt = tval.tv_sec * 1e6 + tval.tv_usec; // time in UTC microseconds
            seed = tt;
            gsl_rng_set(gsl_rng_mem, seed);
            initialized = true;
        }

        if (e->KeywordSet(LONGIx)) {
            DLongGDL *res = new DLongGDL(dim, BaseGDL::NOZERO);
            SizeT nEl = res->N_Elements();
            for (SizeT i = 0; i < nEl; ++i)
                (*res)[i] = (DLong) (gsl_rng_uniform(gsl_rng_mem) * 2147483646) + 1; //apparently IDL rounds up.
            get_random_state(e, gsl_rng_mem, seed);
            return res;
        }

        if (e->KeywordSet(ULONGIx)) {
            DULongGDL *res = new DULongGDL(dim, BaseGDL::NOZERO);
            SizeT nEl = res->N_Elements();
            for (SizeT i = 0; i < nEl; ++i)
                (*res)[i] = (DULong) (gsl_rng_uniform(gsl_rng_mem) * 0xFFFFFFFFUL) + 1; //apparently IDL rounds up.
            get_random_state(e, gsl_rng_mem, seed);
            return res;
        }


        if (e->KeywordPresent(GAMMAIx)) {
            DLong n = -1; //please initialize everything!
            e->AssureLongScalarKW(GAMMAIx, n);
            if (n == 0) {
                DDouble test_n;
                e->AssureDoubleScalarKW(GAMMAIx, test_n);
                if (test_n > 0.0) n = 1;
            }
            if (n <= 0) e->Throw("Value of (Int/Long) GAMMA is out of allowed range: Gamma = 1, 2, 3, ...");
            if (!e->KeywordSet(0)) { //hence:float
                if (n >= 10000000) e->Throw("Value of GAMMA is out of allowed range: Try /DOUBLE.");
            }
            if (e->KeywordSet(0)) { // GDL_DOUBLE
                DDoubleGDL *res = new DDoubleGDL(dim, BaseGDL::NOZERO);
                random_gamma<DDoubleGDL, double>(res, gsl_rng_mem, dim, n);
                get_random_state(e, gsl_rng_mem, seed);
                return res;
            } else {
                DFloatGDL *res = new DFloatGDL(dim, BaseGDL::NOZERO);
                random_gamma<DFloatGDL, float>(res, gsl_rng_mem, dim, n);
                get_random_state(e, gsl_rng_mem, seed);
                return res;
            }
        }

        DDoubleGDL *binomialKey = e->IfDefGetKWAs<DDoubleGDL>(BINOMIALIx);
        if (binomialKey != NULL) {
            SizeT nBinomialKey = binomialKey->N_Elements();
            if (nBinomialKey != 2)
                e->Throw("Keyword array parameter BINOMIAL must have 2 elements.");

            if ((*binomialKey)[0] < 1.0)
                e->Throw(" Value of BINOMIAL[0] is out of allowed range: n = 1, 2, 3, ...");

            if (((*binomialKey)[1] < 0.0) || ((*binomialKey)[1] > 1.0))
                e->Throw(" Value of BINOMIAL[1] is out of allowed range: 0.0 <= p <= 1.0");
            if (e->KeywordSet(0)) { // GDL_DOUBLE
                DDoubleGDL *res = new DDoubleGDL(dim, BaseGDL::NOZERO);
                random_binomial<DDoubleGDL, double>(res, gsl_rng_mem, dim, binomialKey);
                get_random_state(e, gsl_rng_mem, seed);
                return res;
            } else {
                DFloatGDL *res = new DFloatGDL(dim, BaseGDL::NOZERO);
                random_binomial<DFloatGDL, float>(res, gsl_rng_mem, dim, binomialKey);
                get_random_state(e, gsl_rng_mem, seed);
                return res;
            }
        }

        DDoubleGDL *poissonKey = e->IfDefGetKWAs<DDoubleGDL>(POISSONIx);
        if (poissonKey != NULL) {
            SizeT nPoissonKey = poissonKey->N_Elements();
            if (nPoissonKey != 1)
                e->Throw("Expression must be a scalar or 1 element array in this context: " + e->GetString(POISSONIx));
            if ((*poissonKey)[0] < 0.0)
                e->Throw("Value of POISSON is out of allowed range: Poisson > 0.0");

            if (e->KeywordSet("DOUBLE")) {
                DDoubleGDL *res = new DDoubleGDL(dim, BaseGDL::NOZERO);
                random_poisson<DDoubleGDL, double>(res, gsl_rng_mem, dim, poissonKey);
                get_random_state(e, gsl_rng_mem, seed);
                return res;
            } else {
                DFloatGDL *res = new DFloatGDL(dim, BaseGDL::NOZERO);
                if ((*poissonKey)[0] > 1.0e7)
                    e->Throw("Value of POISSON is out of allowed range: Try /DOUBLE.");
                random_poisson<DFloatGDL, float>(res, gsl_rng_mem, dim, poissonKey);
                get_random_state(e, gsl_rng_mem, seed);
                return res;
            }
        }

        if (e->KeywordSet(UNIFORMIx) || ((e->GetProName() == "RANDOMU") && !e->KeywordSet(NORMALIx))) {
            if (e->KeywordSet(0)) { // GDL_DOUBLE
                DDoubleGDL *res = new DDoubleGDL(dim, BaseGDL::NOZERO);
                random_uniform<DDoubleGDL, double>(res, gsl_rng_mem, dim);
                get_random_state(e, gsl_rng_mem, seed);
                return res;
            } else {
                DFloatGDL *res = new DFloatGDL(dim, BaseGDL::NOZERO);
                random_uniform<DFloatGDL, float>(res, gsl_rng_mem, dim);
                get_random_state(e, gsl_rng_mem, seed);
                return res;
            }
        }

        if (e->KeywordSet(NORMALIx) || ((e->GetProName() == "RANDOMN") && !e->KeywordSet(UNIFORMIx))) {
            if (e->KeywordSet(0)) { // GDL_DOUBLE
                DDoubleGDL *res = new DDoubleGDL(dim, BaseGDL::NOZERO);
                random_normal<DDoubleGDL, double>(res, gsl_rng_mem, dim);
                get_random_state(e, gsl_rng_mem, seed);
                return res;
            } else {
                DFloatGDL *res = new DFloatGDL(dim, BaseGDL::NOZERO);
                random_normal<DFloatGDL, float>(res, gsl_rng_mem, dim);
                get_random_state(e, gsl_rng_mem, seed);
                return res;
            }
        }
        assert(false);
        return NULL;
    }

#ifndef HAVE_NEXTTOWARD
    // SA: in C99 / C++TR1 / Boost there is the nextafter() function
    //     the code below provides an alternative if needed
    //     based on the nexttoward.c from mingw (mingw-runtime-3.8/mingwex/math)
    //     by Danny Smith <dannysmith@users.sourceforge.net>
    /*
    nexttoward.c
    Contributed by Danny Smith <dannysmith@users.sourceforge.net>
    No copyright claimed, absolutely no warranties.

    2005-05-10
  */
    double nexttoward(double x, long double y) {
        union {
            double d;
            unsigned long long ll;
        } u;

        long double xx = x;

        if (isnan(y) || isnan(x)) return x + y;

        /* nextafter (0.0, -O.0) should return -0.0.  */
        if (xx == y) return y;

        u.d = x;
        if (x == 0.0) {
            u.ll = 1;
            return y > 0.0L ? u.d : -u.d;
        }

        /* Non-extended encodings are lexicographically ordered,
       with implicit "normal" bit.  */
        if (((x > 0.0) ^ (y > xx)) == 0) u.ll++;
        else u.ll--;
        return u.d;
    }

#endif

    void la_trired_pro(EnvT *e) {
        SizeT nParam = e->NParam(3);
        float f32;
        double f64;

        BaseGDL *p0 = e->GetNumericArrayParDefined(0);

        SizeT nEl = p0->N_Elements();
        if (nEl == 0)
            e->Throw("Variable is undefined: " + e->GetParString(0));

        if (p0->Dim(0) != p0->Dim(1))
            e->Throw("Input must be a square matrix: " + e->GetParString(0));

        // AC2022 : can we write in the inputs ?!
        e->AssureGlobalPar(0);
        e->AssureGlobalPar(1);
        e->AssureGlobalPar(2);

        if (p0->Type() == GDL_COMPLEX) {
            DComplexGDL *p0C = static_cast<DComplexGDL *>( p0);

            float f32_2[2];
            double f64_2[2];

            gsl_matrix_complex *mat =
                    gsl_matrix_complex_alloc(p0->Dim(0), p0->Dim(0));
            GDLGuard<gsl_matrix_complex> g1(mat, gsl_matrix_complex_free);
            gsl_matrix_complex *Q =
                    gsl_matrix_complex_alloc(p0->Dim(0), p0->Dim(0));
            GDLGuard<gsl_matrix_complex> g2(Q, gsl_matrix_complex_free);
            gsl_vector_complex *tau = gsl_vector_complex_alloc(p0->Dim(0) - 1);
            GDLGuard<gsl_vector_complex> g3(tau, gsl_vector_complex_free);
            gsl_vector *diag = gsl_vector_alloc(p0->Dim(0));
            GDLGuard<gsl_vector> g4(diag, gsl_vector_free);
            gsl_vector *subdiag = gsl_vector_alloc(p0->Dim(0) - 1);
            GDLGuard<gsl_vector> g5(subdiag, gsl_vector_free);

            for (SizeT i = 0; i < nEl; ++i) {
                memcpy(f32_2, &(*p0C)[i], szdbl);
                f64 = (double) f32_2[0];
                memcpy(&mat->data[2 * i], &f64, szdbl);

                f64 = (double) f32_2[1];
                memcpy(&mat->data[2 * i + 1], &f64, szdbl);
            }

            gsl_linalg_hermtd_decomp(mat, tau);
            gsl_linalg_hermtd_unpack(mat, tau, Q, diag, subdiag);

            SizeT dims[2] = {p0->Dim(0), p0->Dim(0)};
            dimension dim0(dims, (SizeT) 2);
            BaseGDL **p0Co = &e->GetPar(0);
            GDLDelete((*p0Co));
            *p0Co = new DComplexGDL(dim0, BaseGDL::NOZERO);

            SizeT n = p0->Dim(0);
            dimension dim1(&n, (SizeT) 1);
            BaseGDL **p1F = &e->GetPar(1);
            GDLDelete((*p1F));
            *p1F = new DFloatGDL(dim1, BaseGDL::NOZERO);

            n--;
            dimension dim2(&n, (SizeT) 1);
            BaseGDL **p2F = &e->GetPar(2);
            GDLDelete((*p2F));
            *p2F = new DFloatGDL(dim2, BaseGDL::NOZERO);

            for (SizeT i = 0; i < p0->Dim(0) * p0->Dim(0); i++) {
                memcpy(&f64_2[0], &Q->data[2 * i], 2 * szdbl);
                f32_2[0] = (float) f64_2[0];
                f32_2[1] = (float) f64_2[1];
                memcpy(&(*(DComplexGDL *) *p0Co)[i], &f32_2, 2 * szflt);
            }

            for (SizeT i = 0; i < p0->Dim(0); i++) {
                memcpy(&f64, &diag->data[i], szdbl);
                f32 = (float) f64;
                memcpy(&(*(DFloatGDL *) *p1F)[i], &f32, szflt);
            }

            for (SizeT i = 0; i < p0->Dim(0) - 1; i++) {
                memcpy(&f64, &subdiag->data[i], szdbl);
                f32 = (float) f64;
                memcpy(&(*(DFloatGDL *) *p2F)[i], &f32, szflt);
            }

            // 	gsl_matrix_complex_free(mat);
            // 	gsl_matrix_complex_free(Q);
            // 	gsl_vector_complex_free(tau);
            // 	gsl_vector_free(diag);
            // 	gsl_vector_free(subdiag);
        } else if (p0->Type() == GDL_COMPLEXDBL) {
            DComplexDblGDL *p0C = static_cast<DComplexDblGDL *>( p0);

            gsl_matrix_complex *mat =
                    gsl_matrix_complex_alloc(p0->Dim(0), p0->Dim(0));
            GDLGuard<gsl_matrix_complex> g1(mat, gsl_matrix_complex_free);
            gsl_matrix_complex *Q =
                    gsl_matrix_complex_alloc(p0->Dim(0), p0->Dim(0));
            GDLGuard<gsl_matrix_complex> g2(Q, gsl_matrix_complex_free);
            gsl_vector_complex *tau = gsl_vector_complex_alloc(p0->Dim(0) - 1);
            GDLGuard<gsl_vector_complex> g3(tau, gsl_vector_complex_free);
            gsl_vector *diag = gsl_vector_alloc(p0->Dim(0));
            GDLGuard<gsl_vector> g4(diag, gsl_vector_free);
            gsl_vector *subdiag = gsl_vector_alloc(p0->Dim(0) - 1);
            GDLGuard<gsl_vector> g5(subdiag, gsl_vector_free);

            memcpy(mat->data, &(*p0C)[0], nEl * szdbl * 2);

            gsl_linalg_hermtd_decomp(mat, tau);
            gsl_linalg_hermtd_unpack(mat, tau, Q, diag, subdiag);

            SizeT dims[2] = {p0->Dim(0), p0->Dim(0)};
            dimension dim0(dims, (SizeT) 2);
            BaseGDL **p0Co = &e->GetPar(0);
            GDLDelete((*p0Co));
            *p0Co = new DComplexDblGDL(dim0, BaseGDL::NOZERO);

            SizeT n = p0->Dim(0);
            dimension dim1(&n, (SizeT) 1);
            BaseGDL **p1D = &e->GetPar(1);
            GDLDelete((*p1D));
            *p1D = new DDoubleGDL(dim1, BaseGDL::NOZERO);

            n--;
            dimension dim2(&n, (SizeT) 1);
            BaseGDL **p2D = &e->GetPar(2);
            GDLDelete((*p2D));
            *p2D = new DDoubleGDL(dim2, BaseGDL::NOZERO);

            memcpy(&(*(DComplexDblGDL *) *p0Co)[0], Q->data,
                   p0->Dim(0) * p0->Dim(0) * szdbl * 2);

            memcpy(&(*(DDoubleGDL *) *p1D)[0], diag->data, p0->Dim(0) * szdbl);
            memcpy(&(*(DDoubleGDL *) *p2D)[0], subdiag->data,
                   (p0->Dim(0) - 1) * szdbl);

            // 	gsl_matrix_complex_free(mat);
            // 	gsl_matrix_complex_free(Q);
            // 	gsl_vector_complex_free(tau);
            // 	gsl_vector_free(diag);
            // 	gsl_vector_free(subdiag);
        } else if (p0->Type() == GDL_DOUBLE) {
            DDoubleGDL *p0D = static_cast<DDoubleGDL *>( p0);

            gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(0));
            GDLGuard<gsl_matrix> g1(mat, gsl_matrix_free);
            gsl_matrix *Q = gsl_matrix_alloc(p0->Dim(0), p0->Dim(0));
            GDLGuard<gsl_matrix> g2(Q, gsl_matrix_free);
            gsl_vector *tau = gsl_vector_alloc(p0->Dim(0) - 1);
            GDLGuard<gsl_vector> g3(tau, gsl_vector_free);
            gsl_vector *diag = gsl_vector_alloc(p0->Dim(0));
            GDLGuard<gsl_vector> g4(diag, gsl_vector_free);
            gsl_vector *subdiag = gsl_vector_alloc(p0->Dim(0) - 1);
            GDLGuard<gsl_vector> g5(subdiag, gsl_vector_free);

            memcpy(mat->data, &(*p0D)[0], nEl * szdbl);

            gsl_linalg_symmtd_decomp(mat, tau);
            gsl_linalg_symmtd_unpack(mat, tau, Q, diag, subdiag);

            SizeT dims[2] = {p0->Dim(0), p0->Dim(0)};
            dimension dim0(dims, (SizeT) 2);
            BaseGDL **p0Do = &e->GetPar(0);
            GDLDelete((*p0Do));
            *p0Do = new DDoubleGDL(dim0, BaseGDL::NOZERO);

            SizeT n = p0->Dim(0);
            dimension dim1(&n, (SizeT) 1);
            BaseGDL **p1D = &e->GetPar(1);
            GDLDelete((*p1D));
            *p1D = new DDoubleGDL(dim1, BaseGDL::NOZERO);

            n--;
            dimension dim2(&n, (SizeT) 1);
            BaseGDL **p2D = &e->GetPar(2);
            GDLDelete((*p2D));
            *p2D = new DDoubleGDL(dim2, BaseGDL::NOZERO);

            memcpy(&(*(DDoubleGDL *) *p0Do)[0], Q->data,
                   p0->Dim(0) * p0->Dim(0) * szdbl);

            memcpy(&(*(DDoubleGDL *) *p1D)[0], diag->data, p0->Dim(0) * szdbl);
            memcpy(&(*(DDoubleGDL *) *p2D)[0], subdiag->data,
                   (p0->Dim(0) - 1) * szdbl);

            // 	gsl_matrix_free(mat);
            // 	gsl_matrix_free(Q);
            // 	gsl_vector_free(tau);
            // 	gsl_vector_free(diag);
            // 	gsl_vector_free(subdiag);
        } else if (p0->Type() == GDL_FLOAT ||
                   p0->Type() == GDL_LONG ||
                   p0->Type() == GDL_ULONG ||
                   p0->Type() == GDL_INT ||
                   p0->Type() == GDL_UINT ||
                   p0->Type() == GDL_BYTE) {
            DFloatGDL *p0F = static_cast<DFloatGDL *>( p0);
            DLongGDL *p0L = static_cast<DLongGDL *>( p0);
            DULongGDL *p0UL = static_cast<DULongGDL *>( p0);
            DIntGDL *p0I = static_cast<DIntGDL *>( p0);
            DUIntGDL *p0UI = static_cast<DUIntGDL *>( p0);
            DByteGDL *p0B = static_cast<DByteGDL *>( p0);

            gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(0));
            GDLGuard<gsl_matrix> g1(mat, gsl_matrix_free);
            gsl_matrix *Q = gsl_matrix_alloc(p0->Dim(0), p0->Dim(0));
            GDLGuard<gsl_matrix> g2(Q, gsl_matrix_free);
            gsl_vector *tau = gsl_vector_alloc(p0->Dim(0) - 1);
            GDLGuard<gsl_vector> g3(tau, gsl_vector_free);
            gsl_vector *diag = gsl_vector_alloc(p0->Dim(0));
            GDLGuard<gsl_vector> g4(diag, gsl_vector_free);
            gsl_vector *subdiag = gsl_vector_alloc(p0->Dim(0) - 1);
            GDLGuard<gsl_vector> g5(subdiag, gsl_vector_free);

            for (SizeT i = 0; i < nEl; ++i) {
                switch (p0->Type()) {
                    case GDL_FLOAT:
                        f64 = (double) (*p0F)[i];
                        break;
                    case GDL_LONG:
                        f64 = (double) (*p0L)[i];
                        break;
                    case GDL_ULONG:
                        f64 = (double) (*p0UL)[i];
                        break;
                    case GDL_INT:
                        f64 = (double) (*p0I)[i];
                        break;
                    case GDL_UINT:
                        f64 = (double) (*p0UI)[i];
                        break;
                    case GDL_BYTE:
                        f64 = (double) (*p0B)[i];
                        break;
                    default:
                        break; //pacify -Wswitch.
                }
                memcpy(&mat->data[i], &f64, szdbl);
            }

            gsl_linalg_symmtd_decomp(mat, tau);
            gsl_linalg_symmtd_unpack(mat, tau, Q, diag, subdiag);

            SizeT dims[2] = {p0->Dim(0), p0->Dim(0)};
            dimension dim0(dims, (SizeT) 2);
            BaseGDL **p0Fo = &e->GetPar(0);
            GDLDelete((*p0Fo));
            *p0Fo = new DFloatGDL(dim0, BaseGDL::NOZERO);

            SizeT n = p0->Dim(0);
            dimension dim1(&n, (SizeT) 1);
            BaseGDL **p1F = &e->GetPar(1);
            GDLDelete((*p1F));
            *p1F = new DFloatGDL(dim1, BaseGDL::NOZERO);

            n--;
            dimension dim2(&n, (SizeT) 1);
            BaseGDL **p2F = &e->GetPar(2);
            GDLDelete((*p2F));
            *p2F = new DFloatGDL(dim2, BaseGDL::NOZERO);

            for (SizeT i = 0; i < p0->Dim(0) * p0->Dim(0); i++) {
                memcpy(&f64, &Q->data[i], szdbl);
                f32 = (float) f64;
                memcpy(&(*(DFloatGDL *) *p0Fo)[i], &f32, szflt);
            }

            for (SizeT i = 0; i < p0->Dim(0); i++) {
                memcpy(&f64, &diag->data[i], szdbl);
                f32 = (float) f64;
                memcpy(&(*(DFloatGDL *) *p1F)[i], &f32, szflt);
            }

            for (SizeT i = 0; i < p0->Dim(0) - 1; i++) {
                memcpy(&f64, &subdiag->data[i], szdbl);
                f32 = (float) f64;
                memcpy(&(*(DFloatGDL *) *p2F)[i], &f32, szflt);
            }

            // 	gsl_matrix_free(mat);
            // 	gsl_matrix_free(Q);
            // 	gsl_vector_free(tau);
            // 	gsl_vector_free(diag);
            // 	gsl_vector_free(subdiag);
        } else {
            DFloatGDL *res = static_cast<DFloatGDL *>
            (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
        }
    }


    // gsl_multiroot_function-compatible function serving as a wrapper to the
    // user-defined function passed (by name) as the second arg. to NEWTON or BROYDEN
    class n_b_param {
    public:
        EnvT *envt;
        EnvUDT *nenvt;
        DDoubleGDL *arg;
        string errmsg;
    };

    int n_b_function(const gsl_vector *x, void *params, gsl_vector *f) {
        n_b_param *p = static_cast<n_b_param *> (params);
        // copying from GSL to GDL
        for (size_t i = 0; i < x->size; i++) (*(p->arg))[i] = gsl_vector_get(x, i);
        // executing GDL code
        BaseGDL *res;
        res = p->envt->Interpreter()->call_fun(
                static_cast<DSubUD *> (p->nenvt->GetPro())->GetTree()
        );
        // TODO: no guarding if res is an optimized constant
        // NO!!! the return value of call_fun() is always owned by the caller (constants are Dup()ed)
        // From 0.9.4 on, call_fun can return left and right values dependent on the call context
        // which is by default EnvUDT::RFUNCTION, hence the above is *here* correct.
        Guard<BaseGDL> res_guard(res);
        // sanity checks
        //   if (res->Rank() != 1 || res->N_Elements() != x->size)
        //AC for iCosmo
        if (res->N_Elements() != x->size) {
            p->errmsg = "user-defined function must evaluate to a vector of the size of its argument";
            return GSL_EBADFUNC;
        }
        DDoubleGDL *dres;
        try {
            // BUT: Convert2(...) with CONVERT already deletes 'res' here if the type is changed
            dres = static_cast<DDoubleGDL *> (
                    res->Convert2(GDL_DOUBLE, BaseGDL::CONVERT_THROWIOERROR)
            );
        }
        catch (GDLIOException &ex) {
            p->errmsg = "failed to convert the result of the user-defined function to double";
            return GSL_EBADFUNC;
        }
        if (res != dres) {
            // prevent 'res' from being deleted again
            res_guard.Release();
            res_guard.Init(dres);
        }
        // copying from GDL to GSL
        for (size_t i = 0; i < x->size; i++) gsl_vector_set(f, i, (*dres)[i]);
        return GSL_SUCCESS;
    }

    // a simple error handler for GSL issuing GDL warning messages
    // an initial call (with file=NULL, line=-1 and gsl_errno=-1) sets a prefix to "reason: "
    void n_b_gslerrhandler(const char *reason, const char *file, int line, int gsl_errno) {
        static string prefix;
        if (line == -1 && gsl_errno == -1 && file == NULL) prefix = string(reason) + ": ";
        else Warning(prefix + "GSL: " + reason);
    }

    // a guard object ensuring freeing of GSL-allocated memory
    class n_b_gslguard {
    private:
        //     gsl_vector* x;
        //     gsl_multiroot_fsolver* solver;
        gsl_error_handler_t *old_handler;
    public:
        //     n_b_gslguard(gsl_vector* x_, gsl_multiroot_fsolver* solver_, gsl_error_handler_t* old_handler_)
        n_b_gslguard(gsl_error_handler_t *old_handler_) {
            //       x = x_;
            //       solver = solver_;
            old_handler = old_handler_;
        }

        ~n_b_gslguard() {
            //       gsl_multiroot_fsolver_free(solver);
            //       gsl_vector_free(x);
            gsl_set_error_handler(old_handler);
        }
    };

    // SA: the library routine registered in libinit.cpp both for newton(),
    //     broyden() and used in imsl_zerosys.pro (/HYBRID keyword)
    BaseGDL *newton_broyden(EnvT *e) {
        // sanity check (for number of parameters)
        SizeT nParam = e->NParam();

        // 1-st argument : initial guess vector
        BaseGDL *p0 = e->GetParDefined(0);
        //AC for iCosmo
        //if (p0->Rank() != 1) e->Throw("the first argument is expected to be a vector");
        BaseGDL *par = p0->Convert2(GDL_DOUBLE, BaseGDL::COPY);
        Guard<BaseGDL> par_guard(par);

        // 2-nd argument : name of user function defining the system
        DString fun;
        e->AssureScalarPar<DStringGDL>(1, fun);
        fun = StrUpCase(fun);
        if (LibFunIx(fun) != -1)
            e->Throw("only user-defined functions allowed (library-routine name given)");

        // GDL magick
        StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());
        EnvUDT *newEnv = new EnvUDT(e->CallingNode(), funList[GDLInterpreter::GetFunIx(fun)], (DObjGDL **) NULL);
        newEnv->SetNextPar(&par);
        e->Interpreter()->CallStack().push_back(newEnv);

        // GSL function parameter initialization
        n_b_param param;
        param.envt = e;
        param.nenvt = newEnv;
        param.arg = static_cast<DDoubleGDL *>(par);

        // GSL function initialization
        gsl_multiroot_function F;
        F.f = &n_b_function;
        F.n = p0->N_Elements();
        F.params = &param;

        // GSL error handling
        gsl_error_handler_t *old_handler = gsl_set_error_handler(&n_b_gslerrhandler);
        // now: reinstall previous error handler (was: GSL ensuring memory de-allocation)
        n_b_gslguard gslguard = n_b_gslguard(old_handler);
        n_b_gslerrhandler(e->GetProName().c_str(), NULL, -1, -1);

        // GSL vector initialization
        gsl_vector *x = gsl_vector_alloc(F.n);
        GDLGuard<gsl_vector> g1(x, gsl_vector_free);
        for (size_t i = 0; i < F.n; i++) gsl_vector_set(x, i, (*(DDoubleGDL *) par)[i]);

        // GSL solver initialization
        gsl_multiroot_fsolver *solver;
        {
            const gsl_multiroot_fsolver_type *T;
            static int HYBRIDIx = e->KeywordIx("HYBRID"); //same place in both functions.
            if (e->KeywordSet(HYBRIDIx))
                T = gsl_multiroot_fsolver_hybrid;  //Not using static int KwIx since lists are different.
            else if (e->GetProName() == "NEWTON") T = gsl_multiroot_fsolver_dnewton; //id
            else if (e->GetProName() == "BROYDEN") T = gsl_multiroot_fsolver_broyden; //ibid
            else assert(false);
            solver = gsl_multiroot_fsolver_alloc(T, F.n);
        }
        GDLGuard<gsl_multiroot_fsolver> g2(solver, gsl_multiroot_fsolver_free);
        gsl_multiroot_fsolver_set(solver, &F, x);

        // GDL handling fine-tuning keywords
        // (intentionally not making keyword indices static here (NEWTON vs. BROYDEN))
        DLong iter_max = 200;

        static int ITMAXIx = e->KeywordIx("ITMAX"); //same place in both functions.
        e->AssureLongScalarKWIfPresent(ITMAXIx, iter_max); //ibid
        DDouble tolx = 1e-7, tolf = 1e-4;
        static int TOLXIx = e->KeywordIx("TOLX"); //same place in both functions.
        e->AssureDoubleScalarKWIfPresent(TOLXIx, tolx); //ibid
        static int TOLFIx = e->KeywordIx("TOLF"); //same place in both functions.
        e->AssureDoubleScalarKWIfPresent(TOLFIx, tolf); //ibid

        // GSL root-finding loop
        size_t iter = 0;
        int status;
        do {
            iter++;
            status = gsl_multiroot_fsolver_iterate(solver);
            if (status) break;
            { // TOLF check
                double test_tolf = 0;
                for (size_t i = 0; i < F.n; i++) test_tolf = max(test_tolf, abs(gsl_vector_get(solver->f, i)));
                if (test_tolf < tolf) break;
            }
            { // TOLX check
                double test_tolx = 0;
                for (size_t i = 0; i < F.n; i++)
                    test_tolx = max(test_tolx,
                                    abs(gsl_vector_get(solver->dx, i)) / max(abs(gsl_vector_get(solver->x, i)), 1.)
                    );
                if (test_tolx < tolx) break;
            }
            // a check from GSL doc
            // if (gsl_multiroot_test_residual(solver->f, 1e-7) != GSL_CONTINUE) break;
        } while (iter <= iter_max);

        // remembering the result
        for (size_t i = 0; i < F.n; i++) (*(DDoubleGDL *) par)[i] = gsl_vector_get(solver->x, i);

        // handling errors from GDL via GSL
        if (status == GSL_EBADFUNC) e->Throw(static_cast<n_b_param *>(F.params)->errmsg);

        // showing an error message if ITMAX reached
        if (iter > iter_max) e->Throw("maximum number of iterations reached");

        // returning the result
        static int doubleIx = e->KeywordIx("DOUBLE"); //same place in both functions.
        par_guard.release();    // reusing par for the return value
        return par->Convert2(   // converting to float if neccesarry
                e->KeywordSet(doubleIx) || p0->Type() == GDL_DOUBLE ? GDL_DOUBLE : GDL_FLOAT,
                BaseGDL::CONVERT
        );
    }

    // gsl_multiroot_function-compatible function serving as a wrapper to the
    // user-defined function passed (by name) as the second arg. to NEWTON or BROYDEN
    class param_for_minim {
    public:
        EnvT *envt;
        EnvUDT *nenvt;
        string funcname;
        DDoubleGDL *arg;
        bool failed;
        string errmsg;
    };

    double minim_function(const gsl_vector *x, void *params_minim) {
        param_for_minim *p = static_cast<param_for_minim *> (params_minim);
        p->failed = false;
        // copying from GSL to GDL
        for (size_t i = 0; i < x->size; i++) (*(p->arg))[i] = gsl_vector_get(x, i);
        // executing GDL code
        BaseGDL *res;
        res = p->envt->Interpreter()->call_fun(
                static_cast<DSubUD *> (p->nenvt->GetPro())->GetTree()
        );
        // TODO: no guarding if res is an optimized constant
        // NO!!! the return value of call_fun() is always owned by the caller (constants are Dup()ed)
        // From 0.9.4 on, call_fun can return left and right values dependent on the call context
        // which is by default EnvUDT::RFUNCTION, hence the above is *here* correct.
        Guard<BaseGDL> res_guard(res);
        if (res->N_Elements() != 1) {
            p->failed = true;
            p->errmsg = "user-defined function \"" + p->funcname + "\" must return a single non-string value";
            return std::numeric_limits<double>::quiet_NaN();
        }
        DDoubleGDL *dres;
        try {
            // BUT: Convert2(...) with CONVERT already deletes 'res' here if the type is changed
            dres = static_cast<DDoubleGDL *>(res->Convert2(GDL_DOUBLE, BaseGDL::CONVERT_THROWIOERROR));
        }
        catch (GDLIOException &ex) {
            p->failed = true;
            p->errmsg = "failed to convert the result of the user-defined function \"" + p->funcname + "\" to double";
            dres = new DDoubleGDL(std::numeric_limits<double>::quiet_NaN()); //we do not return, dres MUST exist!
        }
        if (res != dres) {
            // prevent 'res' from being deleted again
            res_guard.Release();
            res_guard.Init(dres);
        }
        // copying from GDL to GSL
        return (*dres)[0];
    }

    BaseGDL *amoeba(EnvT *e) {
        DDouble ftol = 1e-7;
        e->AssureDoubleScalarPar(0, ftol);

        BaseGDL *test;

        // name of user function defining the system
        static int fnameIx = e->KeywordIx("FUNCTION_NAME");
        DString fun;
        if (e->KeywordPresent(fnameIx)) e->AssureScalarKW<DStringGDL>(fnameIx, fun); else fun = "FUNC";
        fun = StrUpCase(fun);
        if (LibFunIx(fun) != -1)
            e->Throw("only user-defined functions allowed (library-routine name given)");

        static int P0Ix = e->KeywordIx("P0");
        bool hasp0 = false;
        if (e->KeywordPresent(P0Ix)) {
            test = e->GetKW(P0Ix);
            if (test != NULL) hasp0 = true;
        }
        static int SCALEIx = e->KeywordIx("SCALE");
        bool hasScale = false;
        if (e->KeywordPresent(SCALEIx)) {
            test = e->GetKW(SCALEIx);
            if (test != NULL) hasScale = true;
        }
        static int SIMPLEXIx = e->KeywordIx("SIMPLEX");
        bool hasSimplex = false;
        if (e->KeywordPresent(SIMPLEXIx)) {
            test = e->GetKW(SIMPLEXIx);
            if (test != NULL) hasSimplex = true;
        }

        //scale implies p0 and fails reading p0 if it is not present
        //if p0 is present, and scale is not present, use Simplex if present, silently else throw.
        bool useSimplex = false;

        BaseGDL *par0;
        BaseGDL *p0;
        Guard<BaseGDL> p0_guard;
        BaseGDL *par1;
        BaseGDL *scale;
        Guard<BaseGDL> scale_guard;
        BaseGDL *par2;
        BaseGDL *simplex;
        Guard<BaseGDL> simplex_guard;

        if (hasScale) { //suppose p0 exist, and get both Scale and p0
            if (hasp0) {
                par0 = e->GetKW(P0Ix);
                p0 = par0->Convert2(GDL_DOUBLE, BaseGDL::COPY);
                p0_guard.Reset(p0);
            } else e->Throw("Variable is undefined: P0."); //not exactly IDL. IDL throws on P0 not being defined.
            par1 = e->GetKW(SCALEIx);
            if (par1->N_Elements() != par0->N_Elements()) {
                scale = new DDoubleGDL(dimension(par0->N_Elements()), BaseGDL::NOZERO);
                scale_guard.Reset(scale);
                DDoubleGDL *tmpscale = static_cast<DDoubleGDL *>(par1->Convert2(GDL_DOUBLE, BaseGDL::COPY));
                Guard<BaseGDL> tmpscale_guard(tmpscale); //deleted immediately when loop exits.
                SizeT n = tmpscale->N_Elements();
                for (SizeT i = 0; i < scale->N_Elements(); ++i) (*(DDoubleGDL *) scale)[i] = (*tmpscale)[i % n];
            } else {
                scale = par1->Convert2(GDL_DOUBLE, BaseGDL::COPY);
                scale_guard.Reset(scale);
            }
            //Now that we have p0 and Scale, create a simplex, useful in some cases below.
            SizeT dims[2];
            dims[0] = p0->Dim(0);
            dims[1] = dims[0] + 1;
            simplex = new DDoubleGDL(dimension(dims, 2), BaseGDL::NOZERO);
            //guard this temporary simplex
            simplex_guard.Reset(simplex);
            //populate simplex as p0, p0 + [1,0,...,0] * scale, p0 + [0,1,0,...,0] * scale, ...
            for (SizeT j = 0; j < dims[0]; ++j) (*(DDoubleGDL *) simplex)[j] = (*(DDoubleGDL *) p0)[j]; //p0
            for (SizeT i = 1; i < dims[1]; ++i) {
                DDouble sc = (*(DDoubleGDL *) scale)[i - 1];
                for (SizeT j = 0; j < dims[0]; ++j) {
                    (*(DDoubleGDL *) simplex)[j + i * dims[0]] = (*(DDoubleGDL *) p0)[j] + ((j == (i - 1)) ? sc
                                                                                                           : 0.0); //warnings about operator precedence solved.
                }
            }
        } else {
            if (hasSimplex) useSimplex = true;
            else e->Throw("Either (SCALE,P0) or SIMPLEX must be initialized");
        }

        //if simplex, convert to p0 and scale
        if (useSimplex) {
            static int make_warning = true;
            if (make_warning) {
                Warning("AMOEBA% SIMPLEX values will be converted to P0 and SCALE, not used directly.");
                make_warning = false;
            }
            par2 = e->GetKW(SIMPLEXIx);
            //check dimensionality
            if (par2->Rank() != 2) e->Throw("Simplex is not a 2D array.");
            SizeT n = par2->Dim(0);
            if (par2->Dim(1) != n + 1) e->Throw("Simplex has wrong dimensions.");
            simplex = par2->Convert2(GDL_DOUBLE, BaseGDL::COPY);
            //do not guard simplex, it will be given back
            //create (temporary) p0 and scale
            p0 = new DDoubleGDL(dimension(n), BaseGDL::NOZERO);
            p0_guard.Reset(p0);
            for (SizeT j = 0; j < n; ++j) { //p0 coords is mean of coords
                DDouble mean = 0;
                for (SizeT i = 0; i < n + 1; ++i) {
                    mean += (*(DDoubleGDL *) simplex)[j + i * n];
                }
                (*(DDoubleGDL *) p0)[j] = mean / (n + 1);
            }
            scale = new DDoubleGDL(dimension(n), BaseGDL::NOZERO);
            scale_guard.Reset(scale);
            for (SizeT j = 0; j < n; ++j) {
                DDouble charScale = 0;
                for (SizeT i = 0; i < n + 1; ++i) {
                    DDouble val = (*(DDoubleGDL *) simplex)[j + i * n] - (*(DDoubleGDL *) p0)[j];
                    charScale += val * val;
                }
                (*(DDoubleGDL *) scale)[j] = sqrt(charScale / (n + 1)); //sort of characteristic nD ball radius.
            }
        }

        DLong nmax = 5000;

        static int NMAXIx = e->KeywordIx("NMAX");
        e->AssureLongScalarKWIfPresent(NMAXIx, nmax);

        static int NCALLSIx = e->KeywordIx("NCALLS");
        bool returnNCalls = e->KeywordPresent(NCALLSIx);

        static int FUNVALIx = e->KeywordIx("FUNCTION_VALUE");
        bool doFunVal = e->KeywordPresent(FUNVALIx);

        // GDL magick
        StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());
        EnvUDT *newEnv = new EnvUDT(e->CallingNode(), funList[GDLInterpreter::GetFunIx(fun)], (DObjGDL **) NULL);
        newEnv->SetNextPar(&p0);
        e->Interpreter()->CallStack().push_back(newEnv);

        // GSL function parameter initialization
        param_for_minim param_minim;
        param_minim.envt = e;
        param_minim.nenvt = newEnv;
        param_minim.funcname = fun;
        param_minim.arg = static_cast<DDoubleGDL *> (p0);

        // GSL function initialization
        gsl_multimin_function minex_func;
        minex_func.f = &minim_function;
        minex_func.n = p0->N_Elements();
        minex_func.params = &param_minim;

        // GSL error handling
        gsl_error_handler_t *old_handler = gsl_set_error_handler(&n_b_gslerrhandler);
        // now: reinstall previous error handler (was: GSL ensuring memory de-allocation)
        n_b_gslguard gslguard = n_b_gslguard(old_handler);
        n_b_gslerrhandler(e->GetProName().c_str(), NULL, -1, -1);


        // solution values at start
        gsl_vector *x = gsl_vector_alloc(minex_func.n);
        GDLGuard<gsl_vector> g1(x, gsl_vector_free);
        for (SizeT i = 0; i < minex_func.n; ++i) gsl_vector_set(x, i, (*(DDoubleGDL *) p0)[i]);

        // initial step sizes
        gsl_vector *ss = gsl_vector_alloc(minex_func.n);
        for (SizeT i = 0; i < minex_func.n; ++i) gsl_vector_set(ss, i, (*(DDoubleGDL *) scale)[i]);
        GDLGuard<gsl_vector> g2(ss, gsl_vector_free);

        const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex2;
        gsl_multimin_fminimizer *s = NULL;

        size_t iter = 0;
        int status;
        double size;

        s = gsl_multimin_fminimizer_alloc(T, minex_func.n);
        gsl_multimin_fminimizer_set(s, &minex_func, x, ss);

        do {
            iter++;
            status = gsl_multimin_fminimizer_iterate(s);

            if (status)
                break;
            size = gsl_multimin_fminimizer_size(s);
            status = gsl_multimin_test_size(size, ftol);

        } while (status == GSL_CONTINUE && iter < nmax);

        // handling errors from GDL via GSL
        if (param_minim.failed) e->Throw(param_minim.errmsg);

        if (returnNCalls) e->SetKW(NCALLSIx, new DLongGDL(iter));

        bool isDouble;
        //Caution par0 may not be defined if simplex.
        if (useSimplex) isDouble = par2->Type() == GDL_DOUBLE; else isDouble = (par0->Type() == GDL_DOUBLE);

        BaseGDL *ret;
        if (isDouble) {
            if (status == GSL_SUCCESS) {
                ret = new DDoubleGDL(dimension(minex_func.n), BaseGDL::NOZERO);
                for (SizeT i = 0; i < minex_func.n; ++i) (*(DDoubleGDL *) ret)[i] = gsl_vector_get(s->x, i);
            } else ret = new DDoubleGDL(-1);
        } else {
            if (status == GSL_SUCCESS) {
                ret = new DFloatGDL(dimension(minex_func.n), BaseGDL::NOZERO);
                for (SizeT i = 0; i < minex_func.n; ++i) (*(DFloatGDL *) ret)[i] = gsl_vector_get(s->x, i);
            } else ret = new DFloatGDL(-1);
        }

        //give back optimized simplex
        if (useSimplex) {
            SizeT n = simplex->Dim(0); //simplex always exist
            SizeT p = simplex->Dim(1);
            for (SizeT j = 0; j < n; ++j) (*(DDoubleGDL *) simplex)[j] = gsl_vector_get(s->x, j);
            for (SizeT i = 1; i < p; ++i) {
                for (SizeT j = 0; j < n; ++j) {
                    (*(DDoubleGDL *) simplex)[j + i * n] = (*(DDoubleGDL *) simplex)[j] + ((j == (i - 1)) ? size
                                                                                                          : 0.0); //warning about operator precedence solved.
                }
            }
            if (isDouble) e->SetKW(SIMPLEXIx, simplex);
            else {
                simplex_guard.Reset(simplex);
                e->SetKW(SIMPLEXIx, simplex->Convert2(GDL_FLOAT, BaseGDL::COPY));
            }
        }

        // function_value for each simplex. Warning: Destroys ss.
        if (doFunVal) {
            gsl_vector_set_all(ss, 0.0);
            BaseGDL *funval;
            SizeT n = simplex->Dim(0); //simplex always exist
            SizeT p = simplex->Dim(1);
            if (isDouble) {
                funval = new DDoubleGDL(dimension(p), BaseGDL::NOZERO);
                for (SizeT i = 0; i < p; ++i) {
                    for (SizeT j = 0; j < n; ++j) gsl_vector_set(x, j, (*(DDoubleGDL *) simplex)[j + i * n]);
                    gsl_multimin_fminimizer_set(s, &minex_func, x, ss);
                    (*(DDoubleGDL *) funval)[i] = s->fval;
                }
            } else {
                funval = new DFloatGDL(dimension(p), BaseGDL::NOZERO);
                for (SizeT i = 0; i < p; ++i) {
                    for (SizeT j = 0; j < n; ++j) gsl_vector_set(x, j, (*(DDoubleGDL *) simplex)[j + i * n]);
                    gsl_multimin_fminimizer_set(s, &minex_func, x, ss);
                    (*(DFloatGDL *) funval)[i] = s->fval;
                }
            }
            e->SetKW(FUNVALIx, funval);
        }

        gsl_multimin_fminimizer_free(s);
        return ret;
    }

    class param_for_minim_fdf {
    public:
        EnvT *envt;
        EnvUDT *nenvt;
        string funcname_f;
        string funcname_df;
        DDoubleGDL *arg;
        DIntGDL *code;
        bool failed;
        string errmsg;
    };

    double minim_function_f(const gsl_vector *x, void *params_minim) {
        param_for_minim_fdf *p = static_cast<param_for_minim_fdf *> (params_minim);
        p->failed = false;
        // copying from GSL to GDL
        for (size_t i = 0; i < x->size; i++) (*(p->arg))[i] = gsl_vector_get(x, i);
        (*(p->code))[0] = 0;
        // executing our wrapper function with code 0
        BaseGDL *res;
        res = p->envt->Interpreter()->call_fun(
                static_cast<DSubUD *> (p->nenvt->GetPro())->GetTree()
        );
        // TODO: no guarding if res is an optimized constant
        // NO!!! the return value of call_fun() is always owned by the caller (constants are Dup()ed)
        // From 0.9.4 on, call_fun can return left and right values dependent on the call context
        // which is by default EnvUDT::RFUNCTION, hence the above is *here* correct.
        Guard<BaseGDL> res_guard(res);
        if (res->N_Elements() != 1) {
            p->failed = true;
            p->errmsg = "user-defined function \"" + p->funcname_f + "\" must return a single non-string value";
            return std::numeric_limits<double>::quiet_NaN();
        }
        DDoubleGDL *dres;
        try {
            // BUT: Convert2(...) with CONVERT already deletes 'res' here if the type is changed
            dres = static_cast<DDoubleGDL *>(res->Convert2(GDL_DOUBLE, BaseGDL::CONVERT_THROWIOERROR));
        }
        catch (GDLIOException &ex) {
            p->failed = true;
            p->errmsg = "failed to convert the result of the user-defined function \"" + p->funcname_f + "\" to double";
            dres = new DDoubleGDL(std::numeric_limits<double>::quiet_NaN()); //we do not return, dres MUST exist!
        }
        if (res != dres) {
            // prevent 'res' from being deleted again
            res_guard.Release();
            res_guard.Init(dres);
        }
        // copying from GDL to GSL
        return (*dres)[0];
    }

    void minim_function_df(const gsl_vector *x, void *params_minim, gsl_vector *g) {
        param_for_minim_fdf *p = static_cast<param_for_minim_fdf *> (params_minim);
        p->failed = false;
        // copying from GSL to GDL
        for (size_t i = 0; i < x->size; i++) (*(p->arg))[i] = gsl_vector_get(x, i);
        (*(p->code))[0] = 1;
        // executing our wrapper function with code 0
        BaseGDL *res;
        res = p->envt->Interpreter()->call_fun(
                static_cast<DSubUD *> (p->nenvt->GetPro())->GetTree()
        );
        // TODO: no guarding if res is an optimized constant
        // NO!!! the return value of call_fun() is always owned by the caller (constants are Dup()ed)
        // From 0.9.4 on, call_fun can return left and right values dependent on the call context
        // which is by default EnvUDT::RFUNCTION, hence the above is *here* correct.
        Guard<BaseGDL> res_guard(res);
        if (res->N_Elements() != x->size) {
            p->failed = true;
            p->errmsg = "user-defined function \"" + p->funcname_df + "\" must return " + i2s(x->size) + " value";
        }
        DDoubleGDL *dres;
        try {
            // BUT: Convert2(...) with CONVERT already deletes 'res' here if the type is changed
            dres = static_cast<DDoubleGDL *>(res->Convert2(GDL_DOUBLE, BaseGDL::CONVERT_THROWIOERROR));
        }
        catch (GDLIOException &ex) {
            p->failed = true;
            p->errmsg =
                    "failed to convert the result of the user-defined function \"" + p->funcname_df + "\" to double";
            dres = new DDoubleGDL(dimension(x->size), BaseGDL::ZERO); //we do not return, dres MUST exist!
        }
        if (res != dres) {
            // prevent 'res' from being deleted again
            res_guard.Release();
            res_guard.Init(dres);
        }
        // copying from GDL to GSL:
        for (SizeT i = 0; i < dres->N_Elements(); ++i) gsl_vector_set(g, i, (*(DDoubleGDL *) dres)[i]);
    }

    void minim_function_fdf(const gsl_vector *x, void *params_minim, double *f, gsl_vector *g) {
        param_for_minim_fdf *p = static_cast<param_for_minim_fdf *> (params_minim);
        p->failed = false;
        // copying from GSL to GDL
        for (size_t i = 0; i < x->size; i++) (*(p->arg))[i] = gsl_vector_get(x, i);
        (*(p->code))[0] = 2;
        // executing our wrapper function with code 0
        BaseGDL *res;
        res = p->envt->Interpreter()->call_fun(
                static_cast<DSubUD *> (p->nenvt->GetPro())->GetTree()
        );
        // TODO: no guarding if res is an optimized constant
        // NO!!! the return value of call_fun() is always owned by the caller (constants are Dup()ed)
        // From 0.9.4 on, call_fun can return left and right values dependent on the call context
        // which is by default EnvUDT::RFUNCTION, hence the above is *here* correct.
        Guard<BaseGDL> res_guard(res);
        if (res->N_Elements() != x->size + 1) {
            p->failed = true;
            p->errmsg = "problem in \"" + p->funcname_f + "\" or \"" + p->funcname_df + "\".";
            return;
        }
        DDoubleGDL *dres;
        try {
            // BUT: Convert2(...) with CONVERT already deletes 'res' here if the type is changed
            dres = static_cast<DDoubleGDL *>(res->Convert2(GDL_DOUBLE, BaseGDL::CONVERT_THROWIOERROR));
        }
        catch (GDLIOException &ex) {
            p->failed = true;
            p->errmsg = "failed to convert the result of your function(s) to double";
            dres = new DDoubleGDL(dimension(x->size), BaseGDL::ZERO); //we do not return, dres MUST exist!
        }
        if (res != dres) {
            // prevent 'res' from being deleted again
            res_guard.Release();
            res_guard.Init(dres);
        }
        // copying from GDL to GSL:
        *f = (*(DDoubleGDL *) dres)[0];
        for (SizeT i = 0; i < dres->N_Elements() - 1; ++i) gsl_vector_set(g, i, (*(DDoubleGDL *) dres)[i + 1]);
    }

    void dfpmin(EnvT *e) {
        // sanity check (for number of parameters)
        SizeT nParam = e->NParam();
        if (nParam != 5) e->Throw("Incorrect number of arguments.");
        // 1-st argument : initial guess vector
        BaseGDL *p0 = e->GetParDefined(0);
        BaseGDL *xVal = p0->Convert2(GDL_DOUBLE, BaseGDL::COPY);
        // 2nd argument : gtol
        DDouble gtol = 0;
        e->AssureDoubleScalarPar(1, gtol);
        // 3rd argument : fmin. will be set on exit.

        //4th argument: function
        BaseGDL *fun = e->GetParDefined(3);
        DString sfun = StrUpCase((*(DStringGDL *) fun)[0]);
        if (LibFunIx(sfun) != -1)
            e->Throw("only user-defined functions allowed (library-routine name " + sfun + " given)");
        //5th argument: derivative function
        BaseGDL *dfun = e->GetParDefined(4);
        DString sdfun = StrUpCase((*(DStringGDL *) dfun)[0]);
        if (LibFunIx(sdfun) != -1)
            e->Throw("only user-defined functions allowed (library-routine name " + sdfun + " given)");
        //but we will use only this special crafted function that accompanies GDL:
        DStringGDL *gdlFun = new DStringGDL("GDL_MULTIMIN_FDF");

        static int DOUBLEIx = e->KeywordIx("DOUBLE");
        bool isDouble = (p0->Type() == GDL_DOUBLE);
        if (e->KeywordSet(DOUBLEIx)) isDouble = true;

        static int EPSIx = e->KeywordIx("EPS");
        DDouble eps = isDouble ? 3E-10 : 3E-8;
        if (e->KeywordPresent(EPSIx)) e->AssureDoubleScalarKW(EPSIx, eps);
        static int ITMAXIx = e->KeywordIx("ITMAX");
        DLong itmax = 200;
        if (e->KeywordPresent(ITMAXIx)) e->AssureLongScalarKW(ITMAXIx, itmax);
        static int TOLXIx = e->KeywordIx("TOLX");
        DDouble tolx = 4 * eps;
        if (e->KeywordPresent(TOLXIx)) e->AssureDoubleScalarKW(TOLXIx, tolx);
        static int STEPMAXIx = e->KeywordIx("STEPMAX");
        DDouble stepmax = 100.0;
        if (e->KeywordPresent(STEPMAXIx)) e->AssureDoubleScalarKW(STEPMAXIx, stepmax);
        BaseGDL *code = new DIntGDL(0);
        // GDL magick
        StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());
        EnvUDT *newEnvfun = new EnvUDT(e->CallingNode(), funList[GDLInterpreter::GetFunIx((*gdlFun)[0])],
                                       (DObjGDL **) NULL);
        newEnvfun->SetNextPar(&xVal); //x in function(x,'f','df',code)
        newEnvfun->SetNextPar(&fun); //f
        newEnvfun->SetNextPar(&dfun); //df
        newEnvfun->SetNextPar(&code); //code
        e->Interpreter()->CallStack().push_back(newEnvfun);

        // GSL function parameter initialization
        param_for_minim_fdf param_minim_fdf;
        param_minim_fdf.envt = e;
        param_minim_fdf.nenvt = newEnvfun;
        param_minim_fdf.funcname_f = sfun;
        param_minim_fdf.funcname_df = sdfun;
        param_minim_fdf.arg = static_cast<DDoubleGDL *> (xVal);
        param_minim_fdf.code = static_cast<DIntGDL *>(code);

        // GSL function initialization
        gsl_multimin_function_fdf minex_func_fdf;
        minex_func_fdf.f = &minim_function_f;
        minex_func_fdf.df = &minim_function_df;
        minex_func_fdf.fdf = &minim_function_fdf;
        minex_func_fdf.n = p0->N_Elements();
        minex_func_fdf.params = &param_minim_fdf;

        // GSL error handling
        gsl_error_handler_t *old_handler = gsl_set_error_handler(&n_b_gslerrhandler);
        // now: reinstall previous error handler (was: GSL ensuring memory de-allocation)
        n_b_gslguard gslguard = n_b_gslguard(old_handler);
        n_b_gslerrhandler(e->GetProName().c_str(), NULL, -1, -1);


        // solution values at start
        gsl_vector *x = gsl_vector_alloc(minex_func_fdf.n);
        GDLGuard<gsl_vector> g1(x, gsl_vector_free);
        for (SizeT i = 0; i < minex_func_fdf.n; ++i) gsl_vector_set(x, i, (*(DDoubleGDL *) xVal)[i]);


        size_t iter = 0;
        int status;

        const gsl_multimin_fdfminimizer_type *T = gsl_multimin_fdfminimizer_vector_bfgs2;
        gsl_multimin_fdfminimizer *s = gsl_multimin_fdfminimizer_alloc(T, minex_func_fdf.n);
        gsl_multimin_fdfminimizer_set(s, &minex_func_fdf, x, stepmax, gtol);
        do {
            iter++;
            status = gsl_multimin_fdfminimizer_iterate(s);

            if (status)
                break;
            status = gsl_multimin_test_gradient(s->gradient, gtol);

        } while (status == GSL_CONTINUE && iter < itmax);

        // handling errors from GDL via GSL
        if (param_minim_fdf.failed) e->Throw(param_minim_fdf.errmsg);
        for (SizeT j = 0; j < p0->N_Elements(); ++j) (*(DDoubleGDL *) xVal)[j] = gsl_vector_get(s->x, j);
        if (isDouble) {
            e->SetPar(0, xVal);
            e->SetPar(2, new DDoubleGDL(s->f));
        } else {
            Guard<BaseGDL> xval_guard(xVal);
            e->SetPar(0, xVal->Convert2(GDL_FLOAT, BaseGDL::COPY));
            e->SetPar(2, new DFloatGDL(s->f));
        }

        gsl_multimin_fdfminimizer_free(s);
    }
    // -------------------------------------------------------------------------
    // GSL don't have implementations of QROMB and QROMO but alternatives

    // AC, 10 Jan 2011, to have it for iCosmos

    class qromb_param {
    public:
        EnvT *envt;
        EnvUDT *nenvt;
        DDoubleGDL *arg;
        string errmsg;
    };

    double qromb_function(double x, void *params) {
        qromb_param *p = static_cast<qromb_param *>(params);
        (*(p->arg))[0] = x;
        BaseGDL *res;
        // marc: is this ok? call_fun will be executed within the actual (callStack.back()) environment
        res = p->envt->Interpreter()->call_fun(static_cast<DSubUD *>(p->nenvt->GetPro())->GetTree());

        // res can be of any type!
        //     return (*static_cast<DDoubleGDL*>(res))[0];
        DDoubleGDL *resDouble = static_cast<DDoubleGDL *>(res->Convert2(GDL_DOUBLE, BaseGDL::CONVERT));
        //    Guard<DDoubleGDL> guard(resDouble);
        double retRes = (*resDouble)[0];
        delete resDouble; // direct delete should be faster than setting up a guard
        return retRes;
    }

    // AC: the library routine is registered in libinit_ac.cpp
    BaseGDL *qromb_fun(EnvT *e) {
        int debug = 0;

        // sanity check (for number of parameters)
        SizeT nParam = e->NParam(3);

        // 2-nd argument : initial bound
        BaseGDL *p1 = e->GetParDefined(1);
        BaseGDL *par1 = p1->Convert2(GDL_DOUBLE, BaseGDL::COPY);
        Guard<BaseGDL> par1_guard(par1);

        // 3-th argument : final bound
        BaseGDL *p2 = e->GetParDefined(2);
        BaseGDL *par2 = p2->Convert2(GDL_DOUBLE, BaseGDL::COPY);
        Guard<BaseGDL> par2_guard(par2);

        // 1-st argument : name of user function defining the system
        DString fun;
        e->AssureScalarPar<DStringGDL>(0, fun);
        fun = StrUpCase(fun);
        if (LibFunIx(fun) != -1)
            e->Throw("only user-defined functions allowed (library-routine name given)");

        // GDL magick
        StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());
        EnvUDT *newEnv = new EnvUDT(e->CallingNode(), funList[GDLInterpreter::GetFunIx(fun)], (DObjGDL **) NULL);
        newEnv->SetNextPar(&par1);
        e->Interpreter()->CallStack().push_back(newEnv);

        // GSL function parameter initialization
        qromb_param param;
        param.envt = e;
        param.nenvt = newEnv;
        param.arg = static_cast<DDoubleGDL *>(par1);

        // GSL function initialization
        gsl_function F;
        F.function = &qromb_function;
        F.params = &param;

        double result, error;
        double first, last;

        SizeT nEl1 = par1->N_Elements();
        SizeT nEl2 = par2->N_Elements();
        SizeT nEl = nEl1;
        DDoubleGDL *res;

        if (nEl1 == 1 || nEl2 == 1) {
            if (nEl1 == 1) {
                nEl = nEl2;
                res = new DDoubleGDL(par2->Dim(), BaseGDL::NOZERO);
            }
            if (nEl2 == 1) {
                res = new DDoubleGDL(par1->Dim(), BaseGDL::NOZERO);
                nEl = nEl1;
            }
        } else {
            if (nEl1 <= nEl2) {
                res = new DDoubleGDL(par1->Dim(), BaseGDL::NOZERO);
            } else {
                res = new DDoubleGDL(par2->Dim(), BaseGDL::NOZERO);
                nEl = nEl2;
            }
        }

        // Definition of JMAX
        int pos;
        DLong wsize = static_cast<DLong>(pow(2.0, (20 - 1)));

        static int JMAXIx = e->KeywordIx("JMAX"); //same place in both functions.
        if (e->KeywordSet(JMAXIx))  //ibid
        {
            e->AssureLongScalarKWIfPresent(JMAXIx, wsize);
            wsize = static_cast<DLong>(pow(2.0, (wsize - 1)));
        }

        // eps value:
        double eps, eps_default;

        static int doubleIx = e->KeywordIx("DOUBLE"); //same place in both functions.
        bool isDouble = e->KeywordSet(doubleIx) || p1->Type() == GDL_DOUBLE || p2->Type() == GDL_DOUBLE;
        if (isDouble) { eps_default = 1.e-12; } else { eps_default = 1.e-6; }

        static int EPSIx = e->KeywordIx("EPS"); //same place in both functions
        if (e->KeywordSet(EPSIx)) {
            e->AssureDoubleScalarKWIfPresent(EPSIx, eps);
            if (eps < 0.0) {
                Message(e->GetProName() + ": EPS must be positive ! Value set to Default.");
                eps = eps_default;
            }
            if (!isfinite(eps)) {
                Message(e->GetProName() + ": EPS must be finite ! Value set to Default.");
                eps = eps_default;
            }
        } else {
            eps = eps_default;
        }

        gsl_integration_workspace *w = gsl_integration_workspace_alloc(wsize);
        GDLGuard<gsl_integration_workspace> g1(w, gsl_integration_workspace_free);

        first = (*static_cast<DDoubleGDL *>(par1))[0];
        last = (*static_cast<DDoubleGDL *>(par2))[0];

        for (SizeT i = 0; i < nEl; i++) {
            if (nEl1 > 1) { first = (*static_cast<DDoubleGDL *>(par1))[i]; }
            if (nEl2 > 1) { last = (*static_cast<DDoubleGDL *>(par2))[i]; }

            if (debug) cout << "Boundaries : " << first << " " << last << endl;

            gsl_integration_qag(&F, first, last, 0, eps, wsize, GSL_INTEG_GAUSS61, w, &result, &error);

            if (debug) cout << "Result : " << result << endl;

            (*res)[i] = result;
        }

        //     gsl_integration_workspace_free (w);

        if (isDouble) {
            return res;
        } else {
            return res->Convert2(GDL_FLOAT, BaseGDL::CONVERT);
        }
    }


    // AC: the library routine is registered in libinit_ac.cpp
    BaseGDL *qromo_fun(EnvT *e) {
        int debug = 0;

        // sanity check (for number of parameters)
        // AC 2016/10/13 : we cannot test here 2 or 3 since both are possible
        SizeT nParam = e->NParam();

        static int midexpIx = e->KeywordIx("MIDEXP");
        bool do_midexp = e->KeywordSet(midexpIx);

        if (do_midexp) {
            if (nParam < 2) e->Throw("Incorrect number of arguments.");
            if (nParam > 2) e->Throw("Too many arguments.");
        } else {
            if (nParam < 3) e->Throw("Incorrect number of arguments.");
        }

        // 2-nd argument : initial bound
        BaseGDL *p1 = NULL;
        BaseGDL *par1 = NULL;
        p1 = e->GetParDefined(1);
        par1 = p1->Convert2(GDL_DOUBLE, BaseGDL::COPY);
        Guard<BaseGDL> par1_guard(par1);

        BaseGDL *p2 = NULL;
        BaseGDL *par2 = NULL;
        if (!do_midexp) {
            // 3-th argument : final bound
            p2 = e->GetParDefined(2);
            par2 = p2->Convert2(GDL_DOUBLE, BaseGDL::COPY);
            Guard<BaseGDL> par2_guard(par2);
        }

        // do we need to compute/return in double ?
        static int doubleIx = e->KeywordIx("DOUBLE");
        bool isDouble = e->KeywordSet(doubleIx) || p1->Type() == GDL_DOUBLE;
        if (!do_midexp)
            if (p2->Type() == GDL_DOUBLE) isDouble = true;

        // 1-st argument : name of user function defining the system
        DString fun;
        e->AssureScalarPar<DStringGDL>(0, fun);
        fun = StrUpCase(fun);
        if (LibFunIx(fun) != -1)
            e->Throw("only user-defined functions allowed (library-routine name given)");

        gsl_error_handler_t *old_handler = gsl_set_error_handler(&n_b_gslerrhandler);
        n_b_gslerrhandler(e->GetProName().c_str(), NULL, -1, -1);

        // Check for impossible case
        bool flag = 0;
        for (SizeT i = 4; i <= 8; i++) {
            if (flag == 1 && e->KeywordPresent(i)) {
                e->Throw("Incorrect number of arguments.");
            } else if (flag == 0 && e->KeywordPresent(i)) flag = 1;
        }

        // GDL magick
        StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());
        EnvUDT *newEnv = new EnvUDT(e->CallingNode(), funList[GDLInterpreter::GetFunIx(fun)], (DObjGDL **) NULL);
        newEnv->SetNextPar(&par1);
        e->Interpreter()->CallStack().push_back(newEnv);

        // GSL function parameter initialization
        qromb_param param;
        param.envt = e;
        param.nenvt = newEnv;
        param.arg = static_cast<DDoubleGDL *>(par1);

        // GSL function initialization
        gsl_function F;
        F.function = &qromb_function;
        F.params = &param;

        double result, error;
        double first, last;

        SizeT nEl1 = par1->N_Elements();
        SizeT nEl2 = nEl1;
        if (!do_midexp)
            nEl2 = par2->N_Elements();
        SizeT nEl = nEl1;
        DDoubleGDL *res;
        if (!do_midexp) {
            if (nEl1 == 1 || nEl2 == 1) {
                if (nEl1 == 1) {
                    nEl = nEl2;
                    res = new DDoubleGDL(par2->Dim(), BaseGDL::NOZERO);
                }
                if (nEl2 == 1) {
                    res = new DDoubleGDL(par1->Dim(), BaseGDL::NOZERO);
                    nEl = nEl1;
                }
            } else {
                if (nEl1 <= nEl2) {
                    res = new DDoubleGDL(par1->Dim(), BaseGDL::NOZERO);
                } else {
                    res = new DDoubleGDL(par2->Dim(), BaseGDL::NOZERO);
                    nEl = nEl2;
                }
            }
        } else res = new DDoubleGDL(par1->Dim(), BaseGDL::NOZERO);

        // managing eps value:
        double eps, eps_default;
        if (isDouble) { eps_default = 1.e-12; } else { eps_default = 1.e-6; }
        int pos;

        static int epsIx = e->KeywordIx("EPS");
        if (e->KeywordSet(epsIx)) {
            e->AssureDoubleScalarKWIfPresent(epsIx, eps);
            if (eps < 0.0) {
                Message(e->GetProName() + ": EPS must be positive ! Value set to Default.");
                eps = eps_default;
            }
            if (!isfinite(eps)) {
                Message(e->GetProName() + ": EPS must be finite ! Value set to Default.");
                eps = eps_default;
            }
        } else {
            eps = eps_default;
        }

        // Definition of JMAX
        DLong wsize = static_cast<DLong>(pow(2.0, (20 - 1)));
        static int jmaxIx = e->KeywordIx("JMAX");
        if (e->KeywordSet(jmaxIx)) {
            e->AssureLongScalarKWIfPresent(jmaxIx, wsize);
            wsize = static_cast<DLong>(pow(2.0, (wsize - 1)));
        }
        gsl_integration_workspace *w = gsl_integration_workspace_alloc(wsize);
        GDLGuard<gsl_integration_workspace> g1(w, gsl_integration_workspace_free);

        first = (*static_cast<DDoubleGDL *>(par1))[0];
        if (!do_midexp) last = (*static_cast<DDoubleGDL *>(par2))[0];

        for (SizeT i = 0; i < nEl; i++) {
            if (nEl1 > 1) { first = (*static_cast<DDoubleGDL *>(par1))[i]; }
            if ((!do_midexp) && (nEl2 > 1)) { last = (*static_cast<DDoubleGDL *>(par2))[i]; }

            if (debug) cout << "Boundaries : " << first << " " << last << endl;

            static int midinfIx = e->KeywordIx("MIDINF");
            static int midpntIx = e->KeywordIx("MIDPNT");
            static int midsqlIx = e->KeywordIx("MIDSQL");
            static int midsquIx = e->KeywordIx("MIDSQU");
            static int jmaxIx = e->KeywordIx("JMAX");

            // Mimic IDL behavior for K
            static int kkkIx = e->KeywordIx("K");
            if (e->KeywordPresent(kkkIx)) {
                DLong k = 0;
                DFloat kk;
                e->AssureFloatScalarKW(kkkIx, kk);
                k = (long) floor(kk);
                if (k < 1) e->Throw("K value must be >= 1 (not used).");
            }

            // intregation on open range [first,+inf[
            if (do_midexp) {
                gsl_integration_qagiu(&F, first, 0, eps,
                                      wsize, w, &result, &error);
            } else if (e->KeywordSet(midinfIx) || e->KeywordSet(midpntIx) ||
                       e->KeywordSet(midsqlIx) || e->KeywordSet(midsquIx) ||
                       e->KeywordSet(jmaxIx) || e->KeywordSet(kkkIx)) {
                gsl_integration_qag(&F, first, last, 0, eps,
                                    wsize, GSL_INTEG_GAUSS61, w, &result, &error);
            } else {
                // AC 2012-10-10: ToDo: checks on values at the boundaries.
                // if no problem, using QAG, else QAGS
                // intregation on open range ]first,last[
                gsl_integration_qags(&F, first, last, 0, eps,
                                     wsize, w, &result, &error);
            }

            if (debug) cout << "Result : " << result << endl;

            (*res)[i] = result;
        }

        //     gsl_integration_workspace_free (w);

        if (isDouble) return res;
        else return res->Convert2(GDL_FLOAT, BaseGDL::CONVERT);

    }


    //FZ_ROOT:compute polynomial roots

    BaseGDL *fz_roots_fun(EnvT *e) {

        static int doubleIx = e->KeywordIx("DOUBLE");

        // Ascending coefficient array
        BaseGDL *p0 = e->GetNumericArrayParDefined(0);
        DDoubleGDL *coef = e->GetParAs<DDoubleGDL>(0);

        // GSL function

        if (ComplexType(p0->Type())) {
            e->Throw("Polynomials with complex coefficients not supported yet (FIXME!)");
        }

        if (coef->N_Elements() < 2) {
            e->Throw("Degree of the polynomial must be strictly greather than zero");
        }

        for (int i = 0; i < coef->N_Elements(); i++) {
            if (!isfinite((*coef)[i])) e->Throw("Not a number and infinity are not supported");
        }

        gsl_poly_complex_workspace *w = gsl_poly_complex_workspace_alloc(coef->N_Elements());
        GDLGuard<gsl_poly_complex_workspace> g1(w, gsl_poly_complex_workspace_free);

        SizeT resultSize = coef->N_Elements() - 1;
        vector<double> tmp(2 * resultSize);

        gsl_poly_complex_solve(&(*coef)[0], coef->N_Elements(), w, &(tmp[0]));

        //     gsl_poly_complex_workspace_free (w);

        int debug = 0;
        if (debug) {
            for (int i = 0; i < resultSize; i++) {
                printf("z%d = %+.18f %+.18f\n", i, tmp[2 * i], tmp[2 * i + 1]);
            }
        }
        DComplexDblGDL *result = new DComplexDblGDL(dimension(resultSize), BaseGDL::NOZERO);
        for (SizeT i = 0; i < resultSize; ++i) {
            (*result)[i] = complex<double>(tmp[2 * i], tmp[2 * i + 1]);
        }

        return result->Convert2(
                e->KeywordSet(doubleIx) || p0->Type() == GDL_DOUBLE
                ? GDL_COMPLEXDBL
                : GDL_COMPLEX,
                BaseGDL::CONVERT);
    }

    //FX_ROOT

    class fx_root_param {
    public:
        EnvT *envt;
        EnvUDT *nenvt;
        DComplexDblGDL *arg;
    };

    complex<double> fx_root_function(complex<double> x, void *params) {
        fx_root_param *p = static_cast<fx_root_param *>(params);
        (*(p->arg))[0] = x;
        BaseGDL *res;
        res = p->envt->Interpreter()->call_fun(static_cast<DSubUD *>(p->nenvt->GetPro())->GetTree());
        return (*static_cast<DComplexDblGDL *>(res))[0];
    }

    // AC 2019-Feb
    // A new version. The previous one failed on a complex example
    // (Ricati equation in Fouks Schubert equation for Si:Ga)

    BaseGDL *fx_root_fun(EnvT *e) {
        //Sanity check
        //SizeT nParam = e->NParam();
        //cout << nParam << endl;

        //1-st argument: a 3-element real or complex initial guess array
        BaseGDL *p0 = e->GetNumericArrayParDefined(0);
        DComplexDblGDL *init = e->GetParAs<DComplexDblGDL>(0);
        BaseGDL *par0 = p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY);
        Guard<BaseGDL> par0_guard(par0);

        if (init->N_Elements() != 3) {
            e->Throw("Initial guess vector must be a 3-element vector");
        }

        // 2-nd argument : function name
        DString fun;
        e->AssureScalarPar<DStringGDL>(1, fun);
        fun = StrUpCase(fun);
        //cout<<fun<<endl;
        if (LibFunIx(fun) != -1)
            e->Throw("only user-defined functions allowed (library-routine name given)");

        // GDL magick
        StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());
        EnvUDT *newEnv = new EnvUDT(e->CallingNode(),
                                    funList[GDLInterpreter::GetFunIx(fun)], (DObjGDL **) NULL);
        newEnv->SetNextPar(&par0);
        e->Interpreter()->CallStack().push_back(newEnv);

        // Function parameter initialization
        fx_root_param param;
        param.envt = e;
        param.nenvt = newEnv;
        param.arg = static_cast<DComplexDblGDL *>(par0);

        //3-rd argument : number of iteration
        DLong max_iter = 100;
        static int itmaxIx = e->KeywordIx("ITMAX");
        if (e->KeywordSet(itmaxIx)) {
            e->AssureLongScalarKWIfPresent(itmaxIx, max_iter);
        }

        //4-th argument : stopping criterion
        DLong stop = 0;
        static int stopIx = e->KeywordIx("STOP");
        if (e->KeywordSet(stopIx)) {
            e->AssureLongScalarKWIfPresent(stopIx, stop);
        }

        if (stop != 0 || stop != 1 || isfinite(stop) == 0) {
            DLong stop = 0;
        }

        //5-th argument : tolerance criterion
        DDouble tol = 0.0001;
        static int tolIx = e->KeywordIx("TOL");
        if (e->KeywordSet(tolIx)) {
            e->AssureDoubleScalarKWIfPresent(tolIx, tol);
        }
        if (isfinite(tol) == 0) {
            DDouble tol = 0.0001;
        }

        //Mueller method
        //Initialization and interpolation

        complex<double> x0((*init)[0].real(), (*init)[0].imag());
        complex<double> x1((*init)[1].real(), (*init)[1].imag());
        complex<double> x2((*init)[2].real(), (*init)[2].imag());

        //Security tests
        if ((x0.real() == x1.real() && x0.imag() == x1.imag()) ||
            (x0.real() == x2.real() && x0.imag() == x2.imag()) ||
            (x1.real() == x2.real() && x1.imag() == x2.imag())) {
            e->Throw("Initial parameters must be different");
        }

        if ((isfinite(x0.real()) == 0 || isfinite(x0.imag()) == 0) ||
            (isfinite(x1.real()) == 0 || isfinite(x1.imag()) == 0) ||
            (isfinite(x2.real()) == 0 || isfinite(x2.imag()) == 0)) {
            e->Throw("Not a number and Infinity are not supported");
        }

        complex<double> fx0, fx1, fx2;
        complex<double> a, b, c, q, q1, disc;
        complex<double> discm, discp, div, root;
        int cond = 0;
        bool debug = false;

        for (int iter = 0; iter < max_iter; iter++) {

            fx0 = fx_root_function(x0, &param);
            fx1 = fx_root_function(x1, &param);
            fx2 = fx_root_function(x2, &param);

            if (debug) {
                //cout << "x0 :" <<  x0 << x1 << x2 << endl;
                //cout <<  abs(fx0) << " "<< abs(fx1) << " "<< abs(fx2) << endl;
            }

            q = (x2 - x1) / (x1 - x0);
            q1 = q + 1.;
            a = q * fx2 - q * q1 * fx1 + q * q * fx0;
            b = (2. * q + 1.) * fx2 - q1 * q1 * fx1 + q * q * fx0;
            c = q1 * fx2;
            disc = b * b - 4. * a * c;
            discm = b - sqrt(disc);
            discp = b + sqrt(disc);
            if (abs(discp) > abs(discm)) div = discp; else div = discm;
            root = x2 - (x2 - x1) * 2. * c / div;

            if ((stop == 0) && (abs(root - x2) < tol)) {
                cond = 1;
            } else {
                if (abs(fx_root_function(root, &param)) < tol) cond = 1;
            }

            if (cond == 1) break;
            x0 = x1;
            x1 = x2;
            x2 = root;
        }

        DComplexDblGDL *res;
        res = new DComplexDblGDL(1, BaseGDL::NOZERO);
        (*res)[0] = root;

        static int DOUBLEIx = e->KeywordIx("DOUBLE");
        bool isdouble = e->KeywordSet(DOUBLEIx);

        if ((*res)[0].imag() == 0) {
            DDoubleGDL *resreal;
            resreal = new DDoubleGDL(1, BaseGDL::NOZERO);
            (*resreal)[0] = (*res)[0].real();

            if (isdouble || p0->Type() == GDL_COMPLEXDBL ||
                p0->Type() == GDL_DOUBLE) {
                return resreal->Convert2(GDL_DOUBLE, BaseGDL::CONVERT);
            } else {
                return resreal->Convert2(GDL_FLOAT, BaseGDL::CONVERT);
            }
        }

        if (isdouble || p0->Type() == GDL_COMPLEXDBL) {
            return res->Convert2(GDL_COMPLEXDBL, BaseGDL::CONVERT);
        } else {
            return res->Convert2(GDL_COMPLEX, BaseGDL::CONVERT);
        }
    }

    /*
   * SA: TODO:
   * constants: Catalan
   * units: ounce, oz, AU, mill, Fahrenheit, stoke, Abcoulomb, ATM (atm works!)
   * prefixes: u (micro)
   */
    BaseGDL *constant(EnvT *e) {
        string name;

        bool twoparams = false;
        if (e->NParam(1) == 2) twoparams = true;

        DStructGDL *Values = SysVar::Values();   //MUST NOT BE STATIC, due to .reset
        static double nan = (*static_cast<DDoubleGDL *>(Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0];
#ifdef USE_UDUNITS
        string unit;
#endif
        {
#ifdef USE_UDUNITS
            DString tmpunit;
#endif
            if (twoparams) {
#ifdef USE_UDUNITS
                                                                                                                                        e->AssureScalarPar<DStringGDL>(1, tmpunit);
	  unit.reserve(tmpunit.length());
	  for (string::iterator it = tmpunit.begin(); it < tmpunit.end(); ++it)
	    if (*it != ' ') unit.append(1, *it);
#else
                e->Throw("GDL was compiled without support for UDUNITS");
#endif
            }
            DString tmpname;
            e->AssureScalarPar<DStringGDL>(0, tmpname);
            name.reserve(tmpname.length());
            for (string::iterator it = tmpname.begin(); it < tmpname.end(); ++it)
                if (*it != ' ' && *it != '_') name.append(1, (char) std::tolower(*it));
        }

#ifdef USE_UDUNITS
                                                                                                                                ut_set_error_message_handler(ut_ignore);
    // making the unit catalogue static to gain performace in the next call
    static ut_system* unitsys = ut_read_xml(NULL);
    if (unitsys == NULL) e->Throw("UDUNITS: failed to load the default unit database");
    ut_unit* unit_from;
#endif
        DDoubleGDL *res;

        if (name.compare("amu") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_UNIFIED_ATOMIC_MASS);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "kg", UT_ASCII);
#endif
        } else if (name.compare("atm") == 0 || name.compare("standardpressure") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_STD_ATMOSPHERE);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "N/m2", UT_ASCII);
#endif
        } else if (name.compare("au") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_ASTRONOMICAL_UNIT);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "m", UT_ASCII);
#endif
        } else if (name.compare("avogadro") == 0) {
            res = new DDoubleGDL(GSL_CONST_NUM_AVOGADRO);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "1/mole", UT_ASCII);
#endif
        } else if (name.compare("boltzman") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_BOLTZMANN);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "J/K", UT_ASCII);
#endif
        } else if (name.compare("c") == 0 || name.compare("speedlight") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_SPEED_OF_LIGHT);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "m/s", UT_ASCII);
#endif
        }
            //    else if (name.compare("catalan") == 0)
            //    {
            //      res = new DDoubleGDL(); // TODO: Dirichlet Beta function!
            //#ifdef USE_UDUNITS
            //      if (twoparams) unit_from = ut_parse(unitsys, "", UT_ASCII);
            //#endif
            //    }
        else if (name.compare("e") == 0) {
            res = new DDoubleGDL(M_E);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_get_dimensionless_unit_one(unitsys);
#endif
        } else if (name.compare("electroncharge") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_ELECTRON_CHARGE);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "C", UT_ASCII);
#endif
        } else if (name.compare("electronmass") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_MASS_ELECTRON);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "kg", UT_ASCII);
#endif
        } else if (name.compare("electronvolt") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_ELECTRON_VOLT);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "J", UT_ASCII);
#endif
        } else if (name.compare("euler") == 0 || name.compare("gamma") == 0) {
            res = new DDoubleGDL(M_EULER);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_get_dimensionless_unit_one(unitsys);
#endif
        } else if (name.compare("faraday") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_FARADAY);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "C/mole", UT_ASCII);
#endif
        } else if (name.compare("finestructure") == 0) {
            res = new DDoubleGDL(GSL_CONST_NUM_FINE_STRUCTURE);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_get_dimensionless_unit_one(unitsys);
#endif
        } else if (name.compare("gas") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_MOLAR_GAS);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "J/mole/K", UT_ASCII);
#endif
        } else if (name.compare("gravity") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_GRAVITATIONAL_CONSTANT);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "N m2 kg-2", UT_ASCII);
#endif
        } else if (name.compare("hbar") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_PLANCKS_CONSTANT_HBAR);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "J s", UT_ASCII);
#endif
        } else if (name.compare("perfectgasvolume") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_STANDARD_GAS_VOLUME);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "m3/mole", UT_ASCII);
#endif
        } else if (name.compare("pi") == 0) {
            res = new DDoubleGDL(M_PI);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_get_dimensionless_unit_one(unitsys);
#endif
        } else if (name.compare("planck") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_PLANCKS_CONSTANT_H);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "J s", UT_ASCII);
#endif
        } else if (name.compare("protonmass") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_MASS_PROTON);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "kg", UT_ASCII);
#endif
        } else if (name.compare("rydberg") == 0) {
            res = new DDoubleGDL(
                    GSL_CONST_MKSA_MASS_ELECTRON * pow(GSL_CONST_MKSA_ELECTRON_CHARGE, 4) / (
                            8. * pow(GSL_CONST_MKSA_VACUUM_PERMITTIVITY, 2) *
                            pow(GSL_CONST_MKSA_PLANCKS_CONSTANT_H, 3) * GSL_CONST_MKSA_SPEED_OF_LIGHT
                    )
            );
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "m-1", UT_ASCII);
#endif
        } else if (name.compare("standardgravity") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_GRAV_ACCEL);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "m/s2", UT_ASCII);
#endif
        } else if (name.compare("stefanboltzman") == 0) {
            res = new DDoubleGDL(GSL_CONST_MKSA_STEFAN_BOLTZMANN_CONSTANT);
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "W/K4/m2", UT_ASCII);
#endif
        } else if (name.compare("watertriple") == 0) {
            res = new DDoubleGDL(273.16); // e.g. http://wtt-lite.nist.gov/cgi-bin/openindex.cgi?cid=7732185
#ifdef USE_UDUNITS
            if (twoparams) unit_from = ut_parse(unitsys, "K", UT_ASCII);
#endif
        } else {
            Warning("IMSL_CONSTANT: unknown constant");
            res = new DDoubleGDL(nan);
            twoparams = false;
        }

        // units
#ifdef USE_UDUNITS
                                                                                                                                if (twoparams)
      {
	assert(unit_from != NULL);
	ut_unit* unit_to = ut_parse(unitsys, unit.c_str(), UT_ASCII);
	if (unit_to == NULL)
	  {
	    Warning("IMSL_CONSTANT: UDUNITS: failed to parse unit");
	    (*res)[0] = nan;
	  }
	else
	  {
	    cv_converter* converter = ut_get_converter(unit_from, unit_to);
	    if (converter == NULL)
	      {
		Warning("IMSL_CONSTANT: UDUNITS: units not convertible");
		(*res)[0] = nan;
	      }
	    else
	      {
		(*res)[0] = cv_convert_double(converter, (*res)[0]);
		cv_free(converter);
	      }
	  }
	ut_free(unit_from); // leaks?
	ut_free(unit_to);   // leaks?
	//ut_free_system(unitsys); // (made static above)
      }
#endif

        static int doubleIx = e->KeywordIx("DOUBLE");
        return res->Convert2(e->KeywordSet(doubleIx) ? GDL_DOUBLE : GDL_FLOAT, BaseGDL::CONVERT);
    }

    BaseGDL *binomialcoef(EnvT *e) {
        SizeT nParam = e->NParam(2);
        if (!IntType(e->GetParDefined(0)->Type()) || !IntType(e->GetParDefined(1)->Type()))
            e->Throw("Arguments must not be floating point numbers");
        DLong n, m;
        e->AssureLongScalarPar(0, n);
        e->AssureLongScalarPar(1, m);
        if (n < 0 || m < 0 || n < m) e->Throw("Arguments must fulfil n >= m >= 0");
        BaseGDL *res = new DDoubleGDL(gsl_sf_choose(n, m));
        static int doubleIx = e->KeywordIx("DOUBLE");
        return res->Convert2(e->KeywordSet(doubleIx) ? GDL_DOUBLE : GDL_FLOAT, BaseGDL::CONVERT);
    }

    //   // SA: helper routines/classes for WTN
    //   // an auto_ptr-like class for guarding wavelets
    //   class gsl_wavelet_guard
    //   {
    //     gsl_wavelet* wavelet;
    //     public:
    //     gsl_wavelet_guard(gsl_wavelet* wavelet_) { wavelet = wavelet_; }
    //     ~gsl_wavelet_guard() { gsl_wavelet_free(wavelet); }
    //   };
    //   // as auto_ptr-like class for guarding wavelet_workspaces
    //   class gsl_wavelet_workspace_guard
    //   {
    //     gsl_wavelet_workspace* workspace;
    //     public:
    //     gsl_wavelet_workspace_guard(gsl_wavelet_workspace* workspace_) { workspace = workspace_; }
    //     ~gsl_wavelet_workspace_guard() { gsl_wavelet_workspace_free(workspace); }
    //   };
    // a simple error handler for GSL issuing GDL warning messages
    // an initial call (with file=NULL, line=-1 and gsl_errno=-1) sets a prefix to "reason: "
    // TODO: merge with the code of NEWTON/BROYDEN/IMSL_HYBRID
    void gsl_err_2_gdl_warn(const char *reason, const char *file, int line, int gsl_errno) {
        static string prefix;
        if (line == -1 && gsl_errno == -1 && file == NULL) prefix = string(reason) + ": ";
        else Warning(prefix + "GSL: " + reason);
    }

    class gsl_err_2_gdl_warn_guard {
        gsl_error_handler_t *old_handler;
    public:
        gsl_err_2_gdl_warn_guard(gsl_error_handler_t *old_handler_) { old_handler = old_handler_; }

        ~gsl_err_2_gdl_warn_guard() { gsl_set_error_handler(old_handler); }
    };

    // SA: 1. Numerical Recipes, and hence IDL as well, calculate the transform until there are
    //        two smoothing coefficients left, while GSL leaves out just one, thus:
    //        - IDL skips computations for four or less elements, while the limit of GSL is 2
    //        - plot, wtn([1,0,0,0,0,0,0,0],4,/inv) have different result in GDL and IDL
    //     2. As of version 1.13 GSL supports only two dimensions (checked at GDL level), and
    //        square matrices in 2D case (left for the GSL error handler to report)
    BaseGDL *wtn(EnvT *e) {
        SizeT nParam = e->NParam(2);
        static int doubleIx = e->KeywordIx("DOUBLE");
        static int overwriteIx = e->KeywordIx("OVERWRITE");
        static int columnIx = e->KeywordIx("COLUMN");
        static int inverseIx = e->KeywordIx("INVERSE");

        // sanity checks for the first argument - array, each dimension of length of 2^n
        BaseGDL *p0 = e->GetNumericArrayParDefined(0);
        for (SizeT dim = 0; dim < p0->Rank(); dim++)
            if ((p0->Dim(dim) & (p0->Dim(dim) - 1)) != 0)
                e->Throw("Dimensions of array must be powers of 2: " + e->GetParString(0));

        // sanity checks for the second argument
        // check for value will be done by GSL (supported numbers are 4, 6, 8, 10, 12, 14, 16, 18, 20)
        // (IDL supports 4, 12 and 20)
        DLong p1;
        e->AssureLongScalarPar(1, p1);

        // sanity checkf for GSL constraints
        if (p0->Rank() > 2) e->Throw("Only one- and two-dimensional transforms supported by GSL");

        // preparing output (GSL always uses double precision and always works in-situ)
        DType inputType = p0->Type();
        DDoubleGDL *ret;
        Guard<DDoubleGDL> ret_guard;
        if (!e->KeywordSet(overwriteIx)) {
            bool stolen = e->StealLocalPar(0);
            if (inputType == GDL_DOUBLE && stolen) {
                ret = static_cast<DDoubleGDL *>(p0);
                ret_guard.Init(ret);
            } else {
                if (stolen)
                    ret = static_cast<DDoubleGDL *>(p0->Convert2(
                            GDL_DOUBLE,
                            BaseGDL::CONVERT
                    ));
                else
                    ret = static_cast<DDoubleGDL *>(p0->Convert2(
                            GDL_DOUBLE,
                            BaseGDL::COPY
                    ));
                ret_guard.Init(ret);
            }
        } else {
            bool stolen = e->StealLocalPar(0);
            if (stolen) {
                // was local par
                ret = static_cast<DDoubleGDL *>(p0->Convert2(
                        GDL_DOUBLE,
                        BaseGDL::CONVERT
                ));
                ret_guard.Init(ret);
            } else {
                assert(e->GlobalPar(0));
                if (inputType == GDL_DOUBLE) {
                    ret = static_cast<DDoubleGDL *>(p0);
                    e->SetPtrToReturnValue(&e->GetPar(0));
                } else {
                    ret = static_cast<DDoubleGDL *>(p0->Convert2(
                            GDL_DOUBLE,
                            BaseGDL::COPY
                    ));
                    ret_guard.Init(ret);
                }
            }
        }

        // GSL error handling
        gsl_error_handler_t *old_handler = gsl_set_error_handler(&gsl_err_2_gdl_warn);
        gsl_err_2_gdl_warn_guard old_handler_guard(old_handler);
        gsl_err_2_gdl_warn(e->GetProName().c_str(), NULL, -1, -1);

        // initializing wavelet ceofficients
        gsl_wavelet *wavelet = gsl_wavelet_alloc(gsl_wavelet_daubechies, p1);
        if (wavelet == NULL) e->Throw("Failed to initialize the wavelet filter coefficients");
        GDLGuard<gsl_wavelet> wavelet_guard(wavelet, gsl_wavelet_free);
        //gsl_wavelet_guard wavelet_guard = gsl_wavelet_guard(wavelet);

        // initializing workspace (N -> N, NxN -> N, 1xN -> N)
        gsl_wavelet_workspace *workspace = gsl_wavelet_workspace_alloc(max(ret->Dim(0), ret->Dim(1)));
        if (workspace == NULL) e->Throw("Failed to allocate scratch memory");
        GDLGuard<gsl_wavelet_workspace> workspace_guard(workspace, gsl_wavelet_workspace_free);
        //gsl_wavelet_workspace_guard workspace_guard = gsl_wavelet_workspace_guard(workspace);

        // 1D (or 1xN) case
        if (ret->Rank() == 1 || (ret->Rank() == 2 && ret->Dim(0) == 1)) {
            if (GSL_SUCCESS != gsl_wavelet_transform(
                    wavelet,
                    &(*ret)[0],
                    1, // stride
                    ret->N_Elements(),
                    e->KeywordSet(inverseIx) ? gsl_wavelet_backward : gsl_wavelet_forward,
                    workspace
            ))
                e->Throw("Failed to compute the transform");

            // transposing the result if /COLUMN was set
            if (e->KeywordSet(columnIx))
                ret->SetDim(ret->Rank() == 1
                            ? dimension(1, ret->N_Elements())
                            : dimension(ret->N_Elements())
                );
        }
            // 2D case
        else {
            if (GSL_SUCCESS != gsl_wavelet2d_transform(
                    wavelet,
                    &((*ret)[0]),
                    ret->Dim(0), // physical row length
                    ret->Dim(0), // number of rows
                    ret->Dim(1), // number of columns
                    e->KeywordSet(inverseIx) ? gsl_wavelet_backward : gsl_wavelet_forward,
                    workspace
            ))
                e->Throw("Failed to compute the transform");

            // TODO: make a proper n-dimensional suuport!
            if (e->KeywordSet(columnIx)) {
                DDoubleGDL *tmp;
                tmp = ret;
                ret = static_cast<DDoubleGDL *>(ret->Transpose(NULL));
                GDLDelete(tmp);
            }
        }

        // returning
        ret_guard.release();
        return ret->Convert2(
                e->KeywordSet(doubleIx) || inputType == GDL_DOUBLE
                ? GDL_DOUBLE
                : GDL_FLOAT,
                BaseGDL::CONVERT
        );
    }


    // SA: helper class for zeropoly
    // an auto_ptr-like class for guarding the poly_complex_workspace
    //   class gsl_poly_complex_workspace_guard
    //   {
    //     gsl_poly_complex_workspace* workspace;
    //     public:
    //     gsl_poly_complex_workspace_guard(gsl_poly_complex_workspace* workspace_) { workspace = workspace_; }
    //     ~gsl_poly_complex_workspace_guard() { gsl_poly_complex_workspace_free(workspace); }
    //   };
    BaseGDL *zeropoly(EnvT *e) {
        static int doubleIx = e->KeywordIx("DOUBLE");
        static int jenkisTraubIx = e->KeywordIx("JENKINS_TRAUB");

        SizeT nParam = e->NParam(1);
        if (e->KeywordSet(jenkisTraubIx))
            e->Throw("Jenkins-Traub method not supported yet (FIXME!)");

        BaseGDL *p0 = e->GetNumericArrayParDefined(0);
        if (ComplexType(p0->Type()))
            e->Throw("Polynomials with complex coefficients not supported yet (FIXME!)");
        if (p0->Rank() != 1)
            e->Throw("The first argument must be a column vector: " + e->GetParString(0));
        DDoubleGDL *coef = e->GetParAs<DDoubleGDL>(0);

        // GSL error handling
        gsl_error_handler_t *old_handler = gsl_set_error_handler(&gsl_err_2_gdl_warn);
        gsl_err_2_gdl_warn_guard old_handler_guard(old_handler);
        gsl_err_2_gdl_warn(e->GetProName().c_str(), NULL, -1, -1);

        // initializing complex polynomial workspace
        gsl_poly_complex_workspace *w = gsl_poly_complex_workspace_alloc(coef->N_Elements());
        GDLGuard<gsl_poly_complex_workspace> g1(w, gsl_poly_complex_workspace_free);
        //     gsl_poly_complex_workspace_guard w_guard(w);

        SizeT resultSize = coef->N_Elements() - 1;
        vector<double> tmp(2 * resultSize);

        if (GSL_SUCCESS != gsl_poly_complex_solve(
                &(*coef)[0], coef->N_Elements(), w, &(tmp[0]))
                )
            e->Throw("Failed to compute the roots of the polynomial");

        DComplexDblGDL *result = new DComplexDblGDL(dimension(resultSize), BaseGDL::NOZERO);
        for (SizeT i = 0; i < resultSize; ++i)
            (*result)[i] = complex<double>(tmp[2 * i], tmp[2 * i + 1]);

        return result->Convert2(
                e->KeywordSet(doubleIx) || p0->Type() == GDL_DOUBLE
                ? GDL_COMPLEXDBL
                : GDL_COMPLEX,
                BaseGDL::CONVERT
        );
    }

    // SA: GDL implementation of LEGENDRE uses gsl_sf_legendre_Plm, while SPHER_HARM implem.
    //     below uses gsl_sf_legendre_sphPlm which is intended for use with sph. harms
    template<class T_theta, class T_phi, class T_res>
    void spher_harm_helper_helper_helper(EnvT *e, T_theta *theta, T_phi *phi, T_res *res,
                                         int l, int m, int step_theta, int step_phi, SizeT length) {
        double sign = (m < 0 && m % 2 == -1) ? -1. : 1.;
        // SA: I haven't found any L,M values which GSL would not accept...
        //gsl_sf_result sphPlm;
        for (SizeT j = 0; j < length; ++j) {
            /*
	  if (GSL_SUCCESS != gsl_sf_legendre_sphPlm_e(l, abs(m), cos(theta[j * step_theta]), &sphPlm))
          e->Throw("GSL refused to compute Legendre polynomial value for the given L,M pair");
	  res[j] = sign * sphPlm.val;
	*/
            res[j] = sign * gsl_sf_legendre_sphPlm(l, abs(m), cos(theta[j * step_theta]));
            res[j] *= T_res(std::exp(complex<T_phi>(0., m * phi[j * step_phi])));
        }
    }

    template<class T_phi, class T_res>
    void spher_harm_helper_helper(EnvT *e, BaseGDL *theta, T_phi *phi, T_res *res,
                                  int l, int m, int step_theta, int step_phi, SizeT length) {
        if (theta->Type() == GDL_DOUBLE || theta->Type() == GDL_COMPLEXDBL) {
            DDoubleGDL *theta_ = e->GetParAs<DDoubleGDL>(0);
            spher_harm_helper_helper_helper(e, &((*theta_)[0]), phi, res, l, m, step_theta, step_phi, length);
        } else {
            DFloatGDL *theta_ = e->GetParAs<DFloatGDL>(0);
            spher_harm_helper_helper_helper(e, &((*theta_)[0]), phi, res, l, m, step_theta, step_phi, length);
        }
    }

    template<class T_res>
    void spher_harm_helper(EnvT *e, BaseGDL *theta, BaseGDL *phi, T_res *res,
                           int l, int m, int step_theta, int step_phi, SizeT length) {
        if (phi->Type() == GDL_DOUBLE || phi->Type() == GDL_COMPLEXDBL) {
            DDoubleGDL *phi_ = e->GetParAs<DDoubleGDL>(1);
            spher_harm_helper_helper(e, theta, &((*phi_)[0]), res, l, m, step_theta, step_phi, length);
        } else {
            DFloatGDL *phi_ = e->GetParAs<DFloatGDL>(1);
            spher_harm_helper_helper(e, theta, &((*phi_)[0]), res, l, m, step_theta, step_phi, length);
        }
    }

    BaseGDL *spher_harm(EnvT *e) {
        // sanity checks etc
        SizeT nParam = e->NParam(4);

        BaseGDL *theta = e->GetNumericParDefined(0);
        BaseGDL *phi = e->GetNumericParDefined(1);

        int step_theta = 1, step_phi = 1;
        SizeT length = theta->N_Elements();
        if (theta->N_Elements() != phi->N_Elements()) {
            if (
                    (theta->N_Elements() > 1 && phi->Rank() != 0) ||
                    (phi->N_Elements() > 1 && theta->Rank() != 0)
                    )
                e->Throw("Theta (1st arg.) or Phi (2nd arg.) must be scalar, or have the same number of values");
            if (theta->N_Elements() > 1) step_phi = 0;
            else {
                step_theta = 0;
                length = phi->N_Elements();
            }
        }

        DLong l;
        e->AssureLongScalarPar(2, l);
        if (l < 0) e->Throw("L (3rd arg.) must be greater than or equal to zero");

        DLong m;
        e->AssureLongScalarPar(3, m);
        if (abs(m) > l) e->Throw("M (4th arg.) must be in the range [-L, L]");

        // allocating (and guarding) memory
        BaseGDL *res;
        bool dbl = e->KeywordSet(0) || theta->Type() == GDL_DOUBLE || phi->Type() == GDL_DOUBLE;
        {
            dimension dim = dimension(length);
            if (phi->Rank() == 0 && theta->Rank() == 0) dim.Remove(0);
            if (dbl) res = new DComplexDblGDL(dim, BaseGDL::NOZERO);
            else res = new DComplexGDL(dim, BaseGDL::NOZERO);
        }
        Guard<BaseGDL> res_guard(res);

        // computing the result
        if (dbl)
            spher_harm_helper(e, theta, phi, &((*static_cast<DComplexDblGDL *>(res))[0]), l, m, step_theta, step_phi,
                              length);
        else
            spher_harm_helper(e, theta, phi, &((*static_cast<DComplexGDL *>(res))[0]), l, m, step_theta, step_phi,
                              length);

        // returning
        res_guard.release();
        return res;
    }

#include <iostream>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_vector.h>
#include <vector>
#include <cassert>
#include <functional>

    struct data {
        double *x;
        double *y;
        size_t n;
    };

    // jacobian analytics & geodesic acceleration functions from the doc, disabled atm
    /*int func_fvv(const gsl_vector *x, const gsl_vector *v, void *params, gsl_vector *fvv) {
        struct data *data_struct = (struct data *) params;
        double a = gsl_vector_get(x, 0);
        double b = gsl_vector_get(x, 1);
        double c = gsl_vector_get(x, 2);
        double va = gsl_vector_get(v, 0);
        double vb = gsl_vector_get(v, 1);
        double vc = gsl_vector_get(v, 2);
        size_t i;

        for (i = 0; i < data_struct->n; ++i) {
            double ti = data_struct->x[i;
            double zi = (ti - b) / c;
            double ei = exp(-0.5 * zi * zi);
            double Dab = -zi * ei / c;
            double Dac = -zi * zi * ei / c;
            double Dbb = a * ei / (c * c) * (1.0 - zi * zi);
            double Dbc = a * zi * ei / (c * c) * (2.0 - zi * zi);
            double Dcc = a * zi * zi * ei / (c * c) * (3.0 - zi * zi);
            double sum;

            sum = 2.0 * va * vb * Dab +
                  2.0 * va * vc * Dac +
                  vb * vb * Dbb +
                  2.0 * vb * vc * Dbc +
                  vc * vc * Dcc;

            gsl_vector_set(fvv, i, sum);
        }

        return GSL_SUCCESS;
    }

    int
    func_df(const gsl_vector *x, void *params, gsl_matrix *J) {
        struct data *data_struct = (struct data *) params;
        double a = gsl_vector_get(x, 0);
        double b = gsl_vector_get(x, 1);
        double c = gsl_vector_get(x, 2);
        size_t i;

        for (i = 0; i < data_struct->n; ++i) {
            double ti = data_struct->x[i;
            double zi = (ti - b) / c;
            double ei = exp(-0.5 * zi * zi);

            gsl_matrix_set(J, i, 0, -ei);
            gsl_matrix_set(J, i, 1, -(a / c) * ei * zi);
            gsl_matrix_set(J, i, 2, -(a / c) * ei * zi * zi);
        }

        return GSL_SUCCESS;
    }*/

    void solve_system(gsl_vector *x, gsl_multifit_nlinear_fdf *fdf, gsl_multifit_nlinear_parameters *params,
                      double *chisq2) {


     
        // chisq2 is here in order to have access to its value in the function gaussfit2
        const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
        const size_t max_iter = 200;
        const double xtol = 1.0e-8;
        const double gtol = 1.0e-8;
        const double ftol = 1.0e-8;
        const size_t n = fdf->n;
        const size_t p = fdf->p;
        gsl_multifit_nlinear_workspace *work = gsl_multifit_nlinear_alloc(T, params, n, p);
        gsl_vector *f = gsl_multifit_nlinear_residual(work);
        gsl_vector *y = gsl_multifit_nlinear_position(work);
        int info;
        double chisq0, chisq, rcond;

        /* initialize solver */
        gsl_multifit_nlinear_init(x, fdf, work);

        /* store initial cost */
        gsl_blas_ddot(f, f, &chisq0);

        /* iterate until convergence */
        gsl_multifit_nlinear_driver(max_iter, xtol, gtol, ftol, NULL, NULL, &info, work);

        /* store final cost */
        gsl_blas_ddot(f, f, &chisq);

        *chisq2 = chisq / (n - p);

        /* store cond(J(x)) */
        gsl_multifit_nlinear_rcond(&rcond, work);

        gsl_vector_memcpy(x, y);

        //fprintf(stderr, "NITER         = %zu\n", gsl_multifit_nlinear_niter(work));

        gsl_multifit_nlinear_free(work);
    }

    int gaussian_vec3(const gsl_vector *x, void *params, gsl_vector *gauss) {
        struct data *data_struct = (struct data *) params;
        double z, w;
        double a0 = gsl_vector_get(x, 0);
        double a1 = gsl_vector_get(x, 1);
        double a2 = gsl_vector_get(x, 2);
        for (size_t i = 0; i < data_struct->n; i++) {
            z = (data_struct->x[i] - a1) / a2;
            w = a0 * exp(-0.5 * z * z);
            gsl_vector_set(gauss, i, data_struct->y[i] - w);
        }
        return GSL_SUCCESS;
    }

    int gaussian_vec4(const gsl_vector *x, void *params, gsl_vector *gauss) {
        struct data *data_struct = (struct data *) params;
        double z, w;
        double a0 = gsl_vector_get(x, 0);
        double a1 = gsl_vector_get(x, 1);
        double a2 = gsl_vector_get(x, 2);
        double a3 = gsl_vector_get(x, 3);
        for (size_t i = 0; i < data_struct->n; i++) {
            z = (data_struct->x[i] - a1) / a2;
            w = a0 * exp(-0.5 * z * z) + a3;
            gsl_vector_set(gauss, i, data_struct->y[i] - w);
        }
        return GSL_SUCCESS;
    }

    int gaussian_vec5(const gsl_vector *x, void *params, gsl_vector *gauss) {
        struct data *data_struct = (struct data *) params;
        double z, w;
        double a0 = gsl_vector_get(x, 0);
        double a1 = gsl_vector_get(x, 1);
        double a2 = gsl_vector_get(x, 2);
        double a3 = gsl_vector_get(x, 3);
        double a4 = gsl_vector_get(x, 4);
        for (size_t i = 0; i < data_struct->n; i++) {
            z = (data_struct->x[i] - a1) / a2;
            w = a0 * exp(-0.5 * z * z) + a3 + a4 * data_struct->x[i];
            gsl_vector_set(gauss, i, data_struct->y[i] - w);
        }
        return GSL_SUCCESS;
    }

    int gaussian_vec6(const gsl_vector *x, void *params, gsl_vector *gauss) {
        struct data *data_struct = (struct data *) params;
        double z, w;
        double a0 = gsl_vector_get(x, 0);
        double a1 = gsl_vector_get(x, 1);
        double a2 = gsl_vector_get(x, 2);
        double a3 = gsl_vector_get(x, 3);
        double a4 = gsl_vector_get(x, 4);
        double a5 = gsl_vector_get(x, 5);
        for (size_t i = 0; i < data_struct->n; i++) {
            z = (data_struct->x[i] - a1) / a2;
            w = a0 * exp(-0.5 * z * z) + a3 + a4 * data_struct->x[i] + a5 * pow(data_struct->x[i], 2.0);
            gsl_vector_set(gauss, i, data_struct->y[i] - w);
        }
        return GSL_SUCCESS;
    }

    vector<double> calcul_estimates(DDoubleGDL *param_x, DDoubleGDL *param_y, int realNterms) {
        int n = param_x->N_Elements();

        double *yd = (double *) malloc(n * sizeof(double));
        memcpy(yd, &(*param_y)[0], n * sizeof(double));
        double *xd = (double *) malloc(n * sizeof(double));
        memcpy(xd, &(*param_x)[0], n * sizeof(double));
        double y4,y5,y6=0;
      
        
       if (realNterms>5){

            for (int i=0;i<n;i++){
                y6=y6+yd[i];
               
            } 
            
            y6=(y6/(pow(n,2.0)*(n/10)))*20;

           
            y6=round(y6*100)/100;
         cout << "y6 " << y6 << endl;
            if (y6>5){y6=0;}
            for (int i=0;i<n;i++){
                yd[i]=yd[i]-y6*pow(xd[i],2.0);
            }   
            
         }

        
       if (realNterms>4){
           
            for (int i=n-(0.1*n);i<n;i++){
                y5=y5+yd[i];
            } 
            
            y5=y5/(0.1*n);
            y5=y5/(-xd[0]);

             cout << "y5 " << y5 << endl;
            double min = std::numeric_limits<double>::min();
            if (y5<min){y5=0;}
           
            for (int i=0;i<n;i++){
                yd[i]=yd[i]-(xd[i]*round(y5*10)/10);
            }    
            
         }

        
        if (realNterms>3){
            
            for (int i=0;i<(0.1*n);i++){
                y4=y4+yd[i];
            } 
            
            y4=y4/(0.1*n);
                     
            for (int i=0;i<n;i++){
                yd[i]=yd[i]-round(y4*10)/10;
            }             
         }
     

        

        double ymaxi = max_element(yd, yd + n)[0];
        double *aymaxi = find(yd, yd + n, ymaxi);
        int iymaxi = distance(yd, aymaxi);
        double xmaxi = xd[iymaxi];

        double ymini = min_element(yd, yd + n)[0];
        double *aymini = find(yd, yd + n, ymini);
        int iymini = distance(yd, aymini);
        double xmini = xd[iymini];

        int i0 = 0;
        if (abs(ymaxi) > abs(ymini)) i0 = iymaxi;
        else i0 = iymini;

        while (i0 < 1) i0++;
        while (i0 > n - 2) i0--;

        double dy = yd[i0];
        double del = dy / exp(1.);
        int i = 0;

        while (((i0 + i + 1) < n) && ((i0 - i) > 0) && (abs(yd[i0 + i]) > abs(del)) &&
               (abs(yd[i0 - i]) > abs(del))) { i++; }
        
        vector<double> a(6);
        a[0] = yd[i0];
        a[1] = xd[i0];
        a[2] = abs(xd[i0] - xd[i0 + i]);
        if (realNterms > 3) a[3] = y4;
        if (realNterms > 4) a[4] = y5;
        if (realNterms > 5) a[5] = y6;

       
        return a;

    }

    BaseGDL *gaussfit2(EnvT *e) {
        int realNterms = 6;

        static int ntermsIx = e->KeywordIx("NTERMS");
        bool nterms = e->KeywordSet(ntermsIx);
        static int estimatesIx = e->KeywordIx("ESTIMATES");
        bool estimates = e->KeywordSet(estimatesIx);
        static int measureErrorsIx = e->KeywordIx("MEASURE_ERRORS");
        bool measureErrors = e->KeywordSet(measureErrorsIx);

        static int chisqIx = e->KeywordIx("CHISQ");
        bool chisq = e->KeywordPresent(chisqIx);
        static int sigmaIx = e->KeywordIx("SIGMA");
        bool sigma = e->KeywordPresent(sigmaIx);
        static int yerrorIx = e->KeywordIx("YERROR");
        bool yerror = e->KeywordPresent(yerrorIx);

        if (nterms) {
            realNterms = (e->GetKWAs<DIntGDL>(ntermsIx))->GetAsIndex(0);
            if (realNterms < 3 || realNterms > 6) {
                e->Throw("NTERMS ranges from 3 to 6");
            }
        }

        DDoubleGDL *param_x = e->GetParAs<DDoubleGDL>(0);
        DDoubleGDL *param_y = e->GetParAs<DDoubleGDL>(1);

        const size_t n = param_x->N_Elements();
        const size_t p = realNterms; // gsl wants a const
        gsl_vector *x = gsl_vector_alloc(p);

        DDoubleGDL *estimatesInput;
        if (estimates) estimatesInput = e->GetKWAs<DDoubleGDL>(estimatesIx); // not necessary but easier to read

        if (nterms && estimates) {
            if (realNterms != estimatesInput->N_Elements()) {
                e->Throw("Inconsistent NTERMS and ESTIMATES elements");
            }
            for (int i = 0; i < estimatesInput->N_Elements(); i++) {
                gsl_vector_set(x, i, (*estimatesInput)[i]);
            }
            if (gsl_vector_get(x, 2) == 0) {
                // "compute a best guess for the starting value for this term"
                gsl_vector_set(x, 2, calcul_estimates(param_x, param_y, realNterms)[2]);
            }
        } else if (!nterms && estimates) {
            if (estimatesInput->N_Elements() != 6) {
                e->Throw("ESTIMATES should have 6 elements");
            }
            for (int i = 0; i < 6; i++) {
                gsl_vector_set(x, i, (*estimatesInput)[i]);
            }
            if (gsl_vector_get(x, 2) == 0) {
                // "compute a best guess for the starting value for this term"
                gsl_vector_set(x, 2, calcul_estimates(param_x, param_y, realNterms)[2]);
            }
        } else if (!estimates) {
            vector <double> a = calcul_estimates(param_x, param_y, realNterms);
            
                for (int i = 0; i < p; i++) {
                    gsl_vector_set(x, i, a[i]);
                }
            
        }

        gsl_multifit_nlinear_fdf fdf;
        gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();

        struct data fit_data;
        fit_data.n = n;
        fit_data.x = (double *) malloc(n * sizeof(double));
        fit_data.y = (double *) malloc(n * sizeof(double));

        memcpy(fit_data.x, &(*param_x)[0], n * sizeof(double));
        memcpy(fit_data.y, &(*param_y)[0], n * sizeof(double));

        /* define function to be minimized */
        fdf.df = nullptr;
        fdf.fvv = nullptr;
        fdf.n = n;
        fdf.p = p;
        fdf.params = &fit_data;

        bool isReturnDouble = false;
        BaseGDL *param_x_type = e->GetPar(0);
        BaseGDL *param_y_type = e->GetPar(1);

        if (param_x_type->Type() == GDL_DOUBLE || param_y_type->Type() == GDL_DOUBLE) isReturnDouble = true;
        if (p == 3) fdf.f = gaussian_vec3;
        else if (p == 4) fdf.f = gaussian_vec4;
        else if (p == 5) fdf.f = gaussian_vec5;
        else fdf.f = gaussian_vec6;

        // uncomment and change lm to another algorithm if needed (see https://www.gnu.org/software/gsl/doc/html/nls.html#c.gsl_multifit_nlinear_trs for all algorithms)
        //fdf_params.trs = gsl_multifit_nlinear_trs_lm;

        // get chisq in case we want it later
        double chisqGSL = 0;
        solve_system(x, &fdf, &fdf_params, &chisqGSL);


        DDoubleGDL *res = new DDoubleGDL(fit_data.n, BaseGDL::NOZERO);

        // compute new y values with the new coeffs
        if (p == 3) {
            double z, w;
            double a0 = gsl_vector_get(x, 0);
            double a1 = gsl_vector_get(x, 1);
            double a2 = gsl_vector_get(x, 2);
            for (size_t i = 0; i < fit_data.n; i++) {
                z = (fit_data.x[i] - a1) / a2;
                w = a0 * exp(-0.5 * z * z);
                (*res)[i] = w;
            }
        } else if (p == 4) {
            double z, w;
            double a0 = gsl_vector_get(x, 0);
            double a1 = gsl_vector_get(x, 1);
            double a2 = gsl_vector_get(x, 2);
            double a3 = gsl_vector_get(x, 3);
            for (size_t i = 0; i < fit_data.n; i++) {
                z = (fit_data.x[i] - a1) / a2;
                w = a0 * exp(-0.5 * z * z) + a3;
                (*res)[i] = w;
            }
        } else if (p == 5) {
            double z, w;
            double a0 = gsl_vector_get(x, 0);
            double a1 = gsl_vector_get(x, 1);
            double a2 = gsl_vector_get(x, 2);
            double a3 = gsl_vector_get(x, 3);
            double a4 = gsl_vector_get(x, 4);
            for (size_t i = 0; i < fit_data.n; i++) {
                z = (fit_data.x[i] - a1) / a2;
                w = a0 * exp(-0.5 * z * z) + a3 + a4 * fit_data.x[i];
                (*res)[i] = w;
            }
        } else {
            double z, w;
            double a0 = gsl_vector_get(x, 0);
            double a1 = gsl_vector_get(x, 1);
            double a2 = gsl_vector_get(x, 2);
            double a3 = gsl_vector_get(x, 3);
            double a4 = gsl_vector_get(x, 4);
            double a5 = gsl_vector_get(x, 5);
            for (size_t i = 0; i < fit_data.n; i++) {
                z = (fit_data.x[i] - a1) / a2;
                w = a0 * exp(-0.5 * z * z) + a3 + a4 * fit_data.x[i] + a5 * pow(fit_data.x[i], 2.0);
                (*res)[i] = w;
            }
        }

        if (e->NParam() == 3) { // black magic to create/update a variable in gdl, if optional param a is specified
            BaseGDL **coefs = &e->GetPar(2);
            GDLDelete((*coefs));
            if (isReturnDouble) {
                *coefs = new DDoubleGDL(realNterms, BaseGDL::NOZERO);
                for (int i = 0; i < realNterms; i++) { // not optimal, don't know how to copy the whole vector at once
                    double temp = gsl_vector_get(x, i);
                    if(i==2){temp=abs(temp);}
                    memcpy(&(*(DDoubleGDL *) *coefs)[i], &temp, sizeof(double));
                }
            } else {
                *coefs = new DFloatGDL(realNterms, BaseGDL::NOZERO);
                // memcpy(&(*(DFloatGDL *) *coefs)[0], &(gsl_vector_get(x, 0)), sizeof(float)*x->size);
                for (int i = 0; i < realNterms; i++) { // not optimal, don't know how to copy the whole vector at once
                    float temp = (float) gsl_vector_get(x, i);
                    if(i==2){temp=abs(temp);}
                    memcpy(&(*(DFloatGDL *) *coefs)[i], &temp, sizeof(float));
                }
            }
        }

        if (yerror) { // black magic to create/update a variable in gdl, if optional keyword yerror is specified
            BaseGDL **valueYerror = &e->GetTheKW(yerrorIx);
            GDLDelete((*valueYerror));
            *valueYerror = new DDoubleGDL(1, BaseGDL::NOZERO);
            double numerator = 0;
            for (size_t i = 0; i < param_y->N_Elements(); i++) {
                numerator = numerator + pow(((*param_y)[i] - (*res)[i]), (double) 2.0);
            }
            double standarderr = sqrt(numerator / (param_y->N_Elements() - realNterms));
            memcpy(&(*(DDoubleGDL *) *valueYerror)[0], &standarderr, sizeof(standarderr));
        }

        if (chisq) { // black magic to create/update a variable in gdl, if optional keyword chisq is specified
            // formula = gsl's chisq / (n - p) ///// h=(y-yfit)^2/(n-nterms)
            BaseGDL **valueChisq = &e->GetTheKW(chisqIx);
            GDLDelete((*valueChisq));
            *valueChisq = new DDoubleGDL(1, BaseGDL::NOZERO);
            memcpy(&(*(DDoubleGDL *) *valueChisq)[0], &chisqGSL, sizeof(chisqGSL));
        }

        ///TODO
        if (sigma) { // black magic to create/update a variable in gdl, if optional keyword sigma is specified
            cout << "sigma is not yet developed" << endl; 
            /*if (!measureErrors) {
                // multiply sigma by sqrt(chisq/(n*m))
            }
            BaseGDL **resSigma = &e->GetTheKW(sigmaIx);
            GDLDelete((*resSigma));
            *resSigma = new DFloatGDL(1, BaseGDL::NOZERO);
            memcpy(&(*(DFloatGDL *) *resSigma)[0], &vraieVal2, sizeof(vraieVal2)); // remplacer par valeur sigma
        */}

        if (measureErrors) { // black magic to create/update a variable in gdl, if optional keyword sigma is specified
            cout << "measureErrors  is not yet developed" << endl; 
        }

        return res;
    }
} // namespace
