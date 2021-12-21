/***************************************************************************
                          basic_op_div.cpp  -  GDL div (/) operators
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

//#include "datatypes.hpp" // for friend declaration
#include "nullgdl.hpp"
#include "dinterpreter.hpp"

// needed with gcc-3.3.2
#include <cassert>

#include "sigfpehandler.hpp"

// Div
// division: left=left/right

template<class Sp>
Data_<Sp>* Data_<Sp>::Div(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);
  ULong nEl = N_Elements();
  assert(nEl);
  SizeT i = 0;

  if (sigsetjmp(sigFPEJmpBuf, 1) == 0) {
    // TODO: Check if we can use OpenMP here (is longjmp allowed?)
    //             if yes: need to run the full loop after the longjmp
    for (/*SizeT i=0*/; i < nEl; ++i)
      (*this)[i] /= (*right)[i];
    return this;
  } else {

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt ix = i; ix < nEl; ++ix) if ((*right)[ix] != this->zero) (*this)[ix] /= (*right)[ix];
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt ix = i; ix < nEl; ++ix) if ((*right)[ix] != this->zero) (*this)[ix] /= (*right)[ix];
    }
    return this;
  }
}
// inverse division: left=right/left

template<class Sp>
Data_<Sp>* Data_<Sp>::DivInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);
  ULong nEl = N_Elements();
  assert(nEl);

  SizeT i = 0;

  if (sigsetjmp(sigFPEJmpBuf, 1) == 0) {
    for (/*SizeT i=0*/; i < nEl; ++i)    (*this)[i] = (*right)[i] / (*this)[i];
    return this;
  } else {

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt ix = i; ix < nEl; ++ix)
        if ((*this)[ix] != this->zero)
          (*this)[ix] = (*right)[ix] / (*this)[ix];
        else
          (*this)[ix] = (*right)[ix];
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt ix = i; ix < nEl; ++ix)
        if ((*this)[ix] != this->zero)
          (*this)[ix] = (*right)[ix] / (*this)[ix];
        else
          (*this)[ix] = (*right)[ix];
    }
    return this;
  }
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::Div(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDString>* Data_<SpDString>::DivInv(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::Div(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivInv(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::Div(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::DivInv(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<class Sp>
Data_<Sp>* Data_<Sp>::DivS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];

  // remember: this is a template (must work for several types)
  // due to error handling the actual devision by 0
  // has to be done
  // but if not 0, we save the expensive error handling
  if (s != this->zero) {
    for (SizeT i = 0; i < nEl; ++i) {
      (*this)[i] /= s;
    }
    return this;
  }
  if (sigsetjmp(sigFPEJmpBuf, 1) == 0) {
    for (SizeT i = 0; i < nEl; ++i) {
      (*this)[i] /= s;
    }
    return this;
  }
  return this;
}

// inverse division: left=right/left

template<class Sp>
Data_<Sp>* Data_<Sp>::DivInvS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);

  // remember: this is a template (must work for several types)
  // due to error handling the actual devision by 0
  // has to be done
  // but if not 0, we save the expensive error handling
  if (nEl == 1 && (*this)[0] != this->zero) {
    (*this)[0] = (*right)[0] / (*this)[0];
    return this;
  }

  Ty s = (*right)[0];
  SizeT i = 0;
  if (sigsetjmp(sigFPEJmpBuf, 1) == 0) {
    for (/*SizeT i=0*/; i < nEl; ++i) (*this)[i] = s / (*this)[i];
    return this;
  } else {
    for (SizeT ix = i; ix < nEl; ++ix) if ((*this)[ix] != this->zero) (*this)[ix] = s / (*this)[ix];
      else (*this)[ix] = s;
    return this;
  }
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::DivS(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDString>* Data_<SpDString>::DivInvS(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivS(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivInvS(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::DivS(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::DivInvS(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

#include "instantiate_templates.hpp"
