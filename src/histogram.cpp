/***************************************************************************
                          histogram.cpp  -  all things histogram
                             -------------------
    begin                : Jan 20 2004
    copyright            : (C) 2004 by Joel Gales, 2022 by Jingwei Wan, G. Duvert
    email                : gilles dot duvert at free dot fr
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

#include "dinterpreter.hpp"

// AC 22/05/11 (needed in Debian 10 & U 18.04)
using namespace std;

template<typename TGDL, typename T>
void update_histo_limits (TGDL* array, T& endVal, DLong nbins, T bsize, T startVal) {

  if (FloatType(array->Type()) ) {
    DDouble endValDble = nbins * (DDouble) bsize + (DDouble) startVal;
    if (endValDble > std::numeric_limits<T>::max()) endVal = std::numeric_limits<T>::max();
    else endVal = endValDble;
  } else {
    DLong64 endVal64 = nbins * (DLong64) bsize + (DLong64) startVal;
    if (endVal64 > std::numeric_limits<T>::max()) endVal = std::numeric_limits<T>::max();
    else endVal = endVal64;
  }
}

template<typename TGDL, typename T>
static BaseGDL* do_histogram_fun(EnvT* e, BaseGDL* p0) {

  static int binsizeIx = e->KeywordIx("BINSIZE");
  T bsize = 1;
  bool hasBinsize = false;
  if (e->KeywordPresentAndDefined(binsizeIx)) {
    hasBinsize = true;
    TGDL* temp = e->GetKWAs<TGDL>(binsizeIx);
    bsize = (*temp)[0];
    if (bsize <= 0)
      e->Throw("Illegal BINSIZE.");
  }

  T maxGivenVal = 0;
  bool hasMaxVal = false;
  static int maxIx = e->KeywordIx("MAX");
  if (e->KeywordPresentAndDefined(maxIx)) {
    hasMaxVal = true;
    TGDL* maxKW = e->GetKWAs<TGDL>(maxIx);
    maxGivenVal = (*maxKW)[0];
  }
  T minGivenVal = 0;
  bool hasMinVal = false;
  static int minIx = e->KeywordIx("MIN");
  if (e->KeywordPresentAndDefined(minIx)) {
    hasMinVal = true;
    TGDL* minKW = e->GetKWAs<TGDL>(minIx);
    minGivenVal = (*minKW)[0];
  }

  static int nbinsIx = e->KeywordIx("NBINS");
  BaseGDL* nbinsKW = e->GetKW(nbinsIx);
  DLong nbins = 0;
  if (nbinsKW != NULL) {
    e->AssureLongScalarKW(nbinsIx, nbins);
    if (nbins < 0) e->Throw("Illegal NBINS (<0).");
  }
  // It is an error to specify NBINS with both BINSIZE and MAX.
  if (hasBinsize && nbins > 0 && hasMaxVal) e->Throw("Conflicting keywords.");

  TGDL *array = static_cast<TGDL*> (p0);
  SizeT nEl = p0->N_Elements();
  // get min max
  // use MinMax here when NAN will be supported

  T minArrayVal, maxArrayVal;
  //Only for floats:
  if (FloatType(p0->Type())) {
    static int nanIx = e->KeywordIx("NAN");
    // minArrayVal/maxArrayVal
    if (e->KeywordSet(nanIx)) {
      DLong minEl, maxEl;
      array->MinMax(&minEl, &maxEl, NULL, NULL, true);
      minArrayVal = (*array)[minEl];
      maxArrayVal = (*array)[maxEl];
    } else {
      minArrayVal = (*array)[0];
      maxArrayVal = minArrayVal;
      for (SizeT i = 1; i < nEl; ++i) {
        T val = (*array)[i];
        if (isinf(val)) e->Throw("Illegal binsize or max/min.");
        if (val < minArrayVal)
          minArrayVal = val;
        else if (val > maxArrayVal)
          maxArrayVal = val;
      }
    }
  } else {
    DLong minEl, maxEl;
    array->MinMax(&minEl, &maxEl, NULL, NULL, true);
    minArrayVal = (*array)[minEl];
    maxArrayVal = (*array)[maxEl];
  }

  int debug = 0;
  if (debug) std::cout << "min/max : " << minArrayVal << " " << maxArrayVal << std::endl;


  T startVal = minGivenVal;
  // startVal 
  if (!hasMinVal) {
    if (p0->Type() == GDL_BYTE)
      startVal = 0;
    else
      startVal = minArrayVal;
  }
  //endVal is first evaluated thus
  T endVal = maxGivenVal;
  if (!hasMaxVal) {
    if (p0->Type() == GDL_BYTE)
      endVal = 255;
    else
      endVal = maxArrayVal;
  }

  bool maxIsIncluded = true;
  //but if NBINS is specified, the value for endVal will be adjusted to NBINS*BINSIZE + MIN
  if (nbins > 0) { //nbins set
    if (nbins > 1 && (hasBinsize || hasMaxVal)) maxIsIncluded = false;
    if (!hasBinsize) {
      //If you define NBINS without specifying MIN, MAX, and BINSIZE, the values are distributed among the (NBINS-1) with the last data value being placed in the last bin.
      if (nbins == 1) bsize = (endVal - startVal);
      else bsize = (endVal - startVal) / (nbins - 1);
    }
    //there is a subtlety with Bytes and other integers as the arithmetic overflows. Best to use a tailored template 
    update_histo_limits<TGDL, T>(array, endVal, nbins, bsize, startVal);

  } else { //nbins not already set
    double nbins0 = (endVal - startVal) / bsize;
    if (ceil(nbins0) <= nbins0) {
      nbins = static_cast<DLong> (ceil(nbins0) + 1);
    } else {
      nbins = static_cast<DLong> (floor(nbins0) + 1);
    }
  }


  debug = 0;
  //    if (debug) 

  // AC 2022/03/26 detected by "test_2876372"
  if (bsize <= 0 || startVal > endVal || !isfinite(startVal) || !isfinite(endVal))
    e->Throw("Illegal binsize or max/min.");

  // INPUT keyword
  static int inputIx = e->KeywordIx("INPUT");
  DLongGDL* input = e->IfDefGetKWAs<DLongGDL>(inputIx);
  if (input != NULL) {
    if (!e->GlobalKW(inputIx))
      e->Throw("Expression must be named variable in this context: " +
      e->GetString(inputIx));
    if (input->N_Elements() > nbins)
      nbins = input->N_Elements();
  }
  // REVERSE_INDICES
  static int reverse_indicesIx = e->KeywordIx("REVERSE_INDICES");
  if (e->KeywordPresent(reverse_indicesIx)) {
    if (input != NULL) e->Throw("Conflicting keywords.");
  }
  //    //starting from here, if endVal is min of endVal,maxVal
  //    endVal = endVal > maxArrayVal ? maxArrayVal : endVal;
  //    std::cout << "minArrayVal=" << +minArrayVal << ", startVal=" << +startVal << ", maxArrayVal=" << +maxArrayVal << ", endVal=" << +endVal << ", bsize=" << +bsize << ", nbins=" << nbins << std::endl;

  dimension dim(nbins);
  DLongGDL* res = new DLongGDL(dim, BaseGDL::ZERO);
  if (e->KeywordPresent(reverse_indicesIx)) {
    DULong k = 0;
    for (SizeT j = 0; j < nEl; ++j) {
      if ((*array)[j] < startVal) continue;
      if ((*array)[j] < endVal) {
        DLong64 r = floorl((((*array)[j] - startVal) / bsize));
        //          if (r < nbins) {
        ++k;
        (*res)[r]++;
        //          } else std::cerr << r << ":" << (*array)[j] << std::endl;
      } else if (maxIsIncluded && (*array)[j] == endVal) {
        DLong64 r = nbins - 1;
        ++k;
        (*res)[r]++;
      }
    }

    DULong nri = nbins + k + 1;
    DLongGDL* indices = new DLongGDL(dimension(nri), BaseGDL::NOZERO);
    //we know the histogram so we put at each bin indice the offset where to write the reverse_index. 
    k = nbins + 1;
    for (SizeT i = 0; i < nbins; ++i) {
      (*indices)[i] = k;
      k += (*res)[i];
    }
    (*indices)[nbins] = nri ;
    // When writing a reverse_index, one reads this offset,
    // write the reverse index at the offset, and increments offset by 1.
    for (SizeT j = 0; j < nEl; ++j) {
      if ((*array)[j] < startVal) continue;
      if ((*array)[j] < endVal) {
        DLong64 r = floorl((((*array)[j] - startVal) / bsize));
        //          if (r < nbins) {
        SizeT pos = (*indices)[r];
        (*indices)[pos] = j;
        (*indices)[r]++;
        //          } else std::cerr << r << ":" << (*array)[j] << std::endl;
      } else if (maxIsIncluded && (*array)[j] == endVal) {
        DLong64 r = nbins - 1;
        SizeT pos = (*indices)[r];
        (*indices)[pos] = j;
        (*indices)[r]++;
      }
    }
    //At the end, the nbins+1 'start-end' indices are reset to the initial value.
    k = nbins + 1;
    for (SizeT i = 0; i < nbins; ++i) {
      (*indices)[i] = k;
      k += (*res)[i];
    }

    e->SetKW(reverse_indicesIx, indices);
  } else {
    for (SizeT j = 0; j < nEl; ++j) {
      if ((*array)[j] < startVal) continue;
      if ((*array)[j] < endVal) {
        DLong64 r = floorl(((*array)[j] - startVal) / bsize);
        (*res)[r]++;
      } else if (maxIsIncluded && (*array)[j] == endVal) {
        (*res)[nbins - 1]++;
      }
    }
  }

  // Add input to output if present
  if (input != NULL) {
    for (SizeT i = 0; i < input->N_Elements(); ++i) (*res)[i] += (*input)[i];
  }

  // OMAX
  static int omaxIx = e->KeywordIx("OMAX");
  if (e->KeywordPresent(omaxIx)) {
    e->SetKW(omaxIx, new TGDL(endVal));
  }
  // OMIN
  static int ominIx = e->KeywordIx("OMIN");
  if (e->KeywordPresent(ominIx)) {
    e->SetKW(ominIx, new TGDL(startVal));
  }


  static int locationsIx = e->KeywordIx("LOCATIONS");
  if (e->WriteableKeywordPresent(locationsIx)) {
    dimension dim(nbins);
    TGDL *locationsKW = new TGDL(dim, BaseGDL::NOZERO);
    for (SizeT i = 0; i < nbins; ++i) {
      (*locationsKW)[i] = startVal + bsize * i;
    }
    e->SetKW(locationsIx, locationsKW);
  }

  return (res);
}

namespace lib {

  BaseGDL* histogram_fun(EnvT* e) {
    SizeT nParam = e->NParam(1);

    BaseGDL* p0 = e->GetNumericParDefined(0);

    if (p0->Rank() == 0)
      e->Throw("Expression must be an array in this context: " + e->GetParString(0));

    SizeT nEl = p0->N_Elements();

    if (p0->Type() == GDL_COMPLEX || p0->Type() == GDL_COMPLEXDBL)
      e->Throw("Complex expression not allowed in this context: "
      + e->GetParString(0));
    else if (p0->Type() == GDL_STRING) {
      e->Throw("String expression not allowed in this context: " + e->GetParString(0) + ".");
    }
    BaseGDL* res;
    switch (p0->Type()) {
    case GDL_BYTE:
      res = do_histogram_fun<DByteGDL, DByte>(e, p0);
      break;
    case GDL_INT:
      res = do_histogram_fun<DIntGDL, DInt>(e, p0);
      break;
    case GDL_LONG:
      res = do_histogram_fun<DLongGDL, DLong>(e, p0);
      break;
    case GDL_FLOAT:
      res = do_histogram_fun<DFloatGDL, DFloat>(e, p0);
      break;
    case GDL_DOUBLE:
      res = do_histogram_fun<DDoubleGDL, DDouble>(e, p0);
      break;
    case GDL_UINT:
      res = do_histogram_fun<DUIntGDL, DUInt>(e, p0);
      break;
    case GDL_ULONG:
      res = do_histogram_fun<DULongGDL, DULong>(e, p0);
      break;
    case GDL_LONG64:
      res = do_histogram_fun<DLong64GDL, DLong64>(e, p0);
      break;
    case GDL_ULONG64:
      res = do_histogram_fun<DULong64GDL, DULong64>(e, p0);
      break;
    default:
      e->Throw("expression not allowed in this context: " + e->GetParString(0) + ".");
    }

    return res;
  }

}//namespace
