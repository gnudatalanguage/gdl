/***************************************************************************
                          $FILENAME$  -  description
                             -------------------
    begin                : $DATE$
    copyright            : (C) $YEAR$ by $AUTHOR$
    email                : $EMAIL$
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
#include "initsysvar.hpp"  // Used to define Double Infinity and Double NaN

#include "errorfunctions.hpp"


#include "faddeeva/Faddeeva.hh"

namespace lib
{
using namespace std;

  BaseGDL* all_faddeeva_functions(EnvT* e, std::complex<double> (*funComplex)(std::complex<double>z, double relerr), double (*funDouble)(double x)  ) {
    e->NParam(1);
    BaseGDL* p0 = e->GetParDefined(0);
    SizeT nElp0 = p0->N_Elements();
    if (ComplexType(p0->Type())) {
      DComplexDblGDL* var = e->GetParAs<DComplexDblGDL>(0);
      if (p0->Type() == GDL_COMPLEX) {
        static const double lowPrecision = std::numeric_limits<DFloat>::epsilon();
        DComplexGDL* res = new DComplexGDL(p0->Dim(), BaseGDL::NOZERO);
        for (SizeT c = 0; c < nElp0; ++c) {
          (*res)[c] = (DComplex) funComplex((*var)[c], lowPrecision);
        }
        return res;
      } else {
        DComplexDblGDL* res = new DComplexDblGDL(p0->Dim(), BaseGDL::NOZERO);
        for (SizeT c = 0; c < nElp0; ++c) {
          (*res)[c] = funComplex((*var)[c], 0);
        }
        return res;
      }
    } else {
      if (funDouble==NULL) e->Throw("Faddeeva function only possible on Complex values!");
      DDoubleGDL* var = e->GetParAs<DDoubleGDL>(0);
      if (p0->Type() == GDL_DOUBLE) {
        DDoubleGDL* res = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);
        for (SizeT c = 0; c < nElp0; ++c) {
          (*res)[c] = funDouble((*var)[c]);
        }
        return res;
      } else {
        DFloatGDL* res = new DFloatGDL(p0->Dim(), BaseGDL::NOZERO);
        for (SizeT c = 0; c < nElp0; ++c) {
          (*res)[c] = (DFloat) funDouble((*var)[c]);
        }
        return res;
      }
    }
    throw;
  }
  
   
  BaseGDL* erf_fun(EnvT* e) {
    return all_faddeeva_functions(e, &(Faddeeva::erf), &(Faddeeva::erf));   
  }
    // ERRORF is the IDL old name of the ERF function. It is implemented to keep old syntax active.
  BaseGDL* errorf_fun(EnvT* e)
  {
    return erf_fun(e);
  } // errorf_fun
  BaseGDL* erfc_fun(EnvT* e)
  {
    return all_faddeeva_functions(e, &(Faddeeva::erfc), &(Faddeeva::erfc));
  }
  BaseGDL* erfcx_fun(EnvT* e)
  {
    return all_faddeeva_functions(e, &(Faddeeva::erfcx), &(Faddeeva::erfcx));
  }
  BaseGDL* erfi_fun(EnvT* e)
  {
    return all_faddeeva_functions(e, &(Faddeeva::erfi), &(Faddeeva::erfi));
  }
    BaseGDL* dawson_fun(EnvT* e)
  {
    return all_faddeeva_functions(e, &(Faddeeva::Dawson), &(Faddeeva::Dawson));
  }
    BaseGDL* faddeeva_fun(EnvT* e)
  {
    return all_faddeeva_functions(e, &(Faddeeva::w),NULL);
  }
} //namespace
