/***************************************************************************
                          basic_op.cpp  -  GDL operators
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

#include "dinterpreter.hpp"

// needed with gcc-3.3.2
#include <cassert>

#include "gdlfpexceptions.hpp"
#include "gdl_util.hpp" //for gdl::powI

using namespace std;

#if defined(USE_EIGEN)
using namespace Eigen;
#endif

// Not operation
// for integers
// ex: b=(not a)
template<class Sp>
Data_<Sp>* Data_<Sp>::NotOp() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  ULong nEl = N_Elements();
  assert(nEl != 0);

  if (nEl == 1) {
    (*this)[0] = ~(*this)[0];
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = ~(*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = ~(*this)[i];
  }
  return this;
}

// others

template<>
Data_<SpDFloat>* Data_<SpDFloat>::NotOp() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  ULong nEl = N_Elements();
  assert(nEl != 0);
  if (nEl == 1) {
    (*this)[0] = ((*this)[0] == 0.0f) ? 1.0f : 0.0f;
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = ((*this)[i] == 0.0f) ? 1.0f : 0.0f;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = ((*this)[i] == 0.0f) ? 1.0f : 0.0f;
  }
  return this;
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::NotOp() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  ULong nEl = N_Elements();
  assert(nEl != 0);
  if (nEl == 1) {
    (*this)[0] = ((*this)[0] == 0.0) ? 1.0 : 0.0;
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = ((*this)[i] == 0.0) ? 1.0 : 0.0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = ((*this)[i] == 0.0) ? 1.0 : 0.0;
  }
  return this;
}

template<>
Data_<SpDString>* Data_<SpDString>::NotOp() { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::NotOp() { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::NotOp() { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::NotOp() { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::NotOp() { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

// UMinus unary minus
// for numbers

template<class Sp>
BaseGDL* Data_<Sp>::UMinus() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  ULong nEl = N_Elements();
  assert(nEl != 0);
  if (nEl == 1) {
    (*this)[0] = -(*this)[0];
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = -(*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = -(*this)[i];
  }
  return this;
}

template<>
BaseGDL* Data_<SpDString>::UMinus() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  ULong nEl = N_Elements();
  assert(nEl != 0);
  Data_<SpDFloat>* newThis = static_cast<Data_<SpDFloat>*> (this->Convert2(GDL_FLOAT));
  //  this is deleted by convert2!!!
  return static_cast<BaseGDL*> (newThis->UMinus());
}

template<>
BaseGDL* Data_<SpDPtr>::UMinus() { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
BaseGDL* Data_<SpDObj>::UMinus() { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

// logical negation
// integers, also ptr and object

template<class Sp>
Data_<SpDByte>* Data_<Sp>::LogNeg() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = dd.size();
  assert(nEl);
  DByteGDL* res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);

  if (nEl == 1) {
    (*res)[0] = ((*this)[0] == 0) ? 1 : 0;
    return res;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == 0) ? 1 : 0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == 0) ? 1 : 0;
  }
  return res;
}

template<>
Data_<SpDByte>* Data_<SpDObj>::LogNeg() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if (this->Scalar()) {
    DSubUD* isTrueOverload = static_cast<DSubUD*> (GDLInterpreter::GetObjHeapOperator(dd[0], OOIsTrue));
    if (isTrueOverload != NULL) {
      if (this->LogTrue())
        return new Data_<SpDByte>(0);
      else
        return new Data_<SpDByte>(1);
    }
  }

  SizeT nEl = dd.size();
  assert(nEl);
  DByteGDL* res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);

  if (nEl == 1) {
    (*res)[0] = ((*this)[0] == 0) ? 1 : 0;
    return res;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == 0) ? 1 : 0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == 0) ? 1 : 0;
  }
  return res;
}

template<>
Data_<SpDByte>* Data_<SpDFloat>::LogNeg() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = dd.size();
  assert(nEl);

  DByteGDL* res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
  if (nEl == 1) {
    (*res)[0] = ((*this)[0] == 0.0f) ? 1 : 0;
    return res;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == 0.0f) ? 1 : 0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == 0.0f) ? 1 : 0;
  }
  return res;
}

template<>
Data_<SpDByte>* Data_<SpDDouble>::LogNeg() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = dd.size();
  assert(nEl);

  DByteGDL* res = new Data_<SpDByte>(dim, BaseGDL::NOZERO);
  if (nEl == 1) {
    (*res)[0] = ((*this)[0] == 0.0) ? 1 : 0;
    return res;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == 0.0) ? 1 : 0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == 0.0) ? 1 : 0;
  }
  return res;
}

template<>
Data_<SpDByte>* Data_<SpDString>::LogNeg() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = dd.size();
  assert(nEl);

  DByteGDL* res = new Data_<SpDByte>(dim, BaseGDL::NOZERO);
  if (nEl == 1) {
    (*res)[0] = ((*this)[0] == "") ? 1 : 0;
    return res;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == "") ? 1 : 0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == "") ? 1 : 0;
  }
  return res;
}

template<>
Data_<SpDByte>* Data_<SpDComplex>::LogNeg() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = dd.size();
  assert(nEl);

  DByteGDL* res = new Data_<SpDByte>(dim, BaseGDL::NOZERO);
  if (nEl == 1) {
    (*res)[0] = ((*this)[0].real() == 0.0 && (*this)[0].imag() == 0.0) ? 1 : 0;
    return res;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i].real() == 0.0 && (*this)[i].imag() == 0.0) ? 1 : 0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i].real() == 0.0 && (*this)[i].imag() == 0.0) ? 1 : 0;
  }
  return res;
}

template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::LogNeg() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = dd.size();
  assert(nEl);

  DByteGDL* res = new Data_<SpDByte>(dim, BaseGDL::NOZERO);
  if (nEl == 1) {
    (*res)[0] = ((*this)[0].real() == 0.0 && (*this)[0].imag() == 0.0) ? 1 : 0;
    return res;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i].real() == 0.0 && (*this)[i].imag() == 0.0) ? 1 : 0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i].real() == 0.0 && (*this)[i].imag() == 0.0) ? 1 : 0;
  }
  return res;
}

// increment decrement operators
// integers

template<class Sp>
void Data_<Sp>::Dec() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  ULong nEl = N_Elements();
  assert(nEl != 0);

  if (nEl == 1) {
    (*this)[0]--;
    return;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i]--;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i]--;
  }
}

template<class Sp>
void Data_<Sp>::Inc() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  ULong nEl = N_Elements();
  assert(nEl != 0);

  if (nEl == 1) {
    (*this)[0]++;
    return;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i]++;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i]++;
  }
}
// float

template<>
void Data_<SpDFloat>::Dec() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)

  ULong nEl = N_Elements();
  assert(nEl != 0);

  if (nEl == 1) {
    (*this)[0] -= 1.0;
    return;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] -= 1.0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] -= 1.0;
  }
}

template<>
void Data_<SpDFloat>::Inc() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)

  ULong nEl = N_Elements();
  assert(nEl != 0);

  if (nEl == 1) {
    (*this)[0] += 1.0;
    return;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] += 1.0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] += 1.0;
  }
}
// double

template<>
void Data_<SpDDouble>::Dec() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)

  ULong nEl = N_Elements();
  assert(nEl != 0);

  if (nEl == 1) {
    (*this)[0] -= 1.0;
    return;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] -= 1.0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] -= 1.0;
  }
}

template<>
void Data_<SpDDouble>::Inc() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)

  ULong nEl = N_Elements();
  assert(nEl != 0);

  if (nEl == 1) {
    (*this)[0] += 1.0;
    return;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] += 1.0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] += 1.0;
  }
}
// complex

template<>
void Data_<SpDComplex>::Dec() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)

  ULong nEl = N_Elements();
  assert(nEl != 0);

  if (nEl == 1) {
    (*this)[0] -= 1.0;
    return;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] -= 1.0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] -= 1.0;
  }
}

template<>
void Data_<SpDComplex>::Inc() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)

  ULong nEl = N_Elements();
  assert(nEl != 0);

  if (nEl == 1) {
    (*this)[0] += 1.0;
    return;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] += 1.0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] += 1.0;
  }
}

template<>
void Data_<SpDComplexDbl>::Dec() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)

  ULong nEl = N_Elements();
  assert(nEl != 0);

  if (nEl == 1) {
    (*this)[0] -= 1.0;
    return;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] -= 1.0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] -= 1.0;
  }
}

template<>
void Data_<SpDComplexDbl>::Inc() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)

  ULong nEl = N_Elements();
  assert(nEl != 0);

  if (nEl == 1) {
    (*this)[0] += 1.0;
    return;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] += 1.0;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] += 1.0;
  }
}
// forbidden types

template<>
void Data_<SpDString>::Dec() { 
  throw GDLException("String expression not allowed in this context.", true, false);
}

template<>
void Data_<SpDPtr>::Dec() { 
  throw GDLException("Pointer expression not allowed in this context.", true, false);
}

template<>
void Data_<SpDObj>::Dec() { 
  throw GDLException("Object expression not allowed in this context.", true, false);
}

template<>
void Data_<SpDString>::Inc() { 
  throw GDLException("String expression not allowed in this context.", true, false);
}

template<>
void Data_<SpDPtr>::Inc() { 
  throw GDLException("Pointer expression not allowed in this context.", true, false);
}

template<>
void Data_<SpDObj>::Inc() { 
  throw GDLException("Object expression not allowed in this context.", true, false);
}


// binary operators

// 1. operators that always return a new result
// EqOp
// returns *this eq *r
// ex: b=(a eq 0)
template<class Sp>
BaseGDL* Data_<Sp>::EqOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);

  Data_<SpDByte>* res;

  Ty s;
  if (right->StrictScalar(s)) {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (nEl == 1) {
      (*res)[0] = (s == (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == s);
    }
  } else if (StrictScalar(s)) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] == s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] == s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] == s);
    }
  } else if (rEl < nEl) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] == (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] == (*this)[i]);
    }
  } else // ( rEl >= nEl)
  {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] == (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] == (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] == (*this)[i]);
    }
  }
  return res;
}
// must handle overloads

template<>
BaseGDL* Data_<SpDObj>::EqOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if (Scalar()) {
    DSubUD* EQOverload = static_cast<DSubUD*> (GDLInterpreter::GetObjHeapOperator((*this)[0], OOEQ));
    if (EQOverload == NULL) {
      //scalar object 'this' can be equal to NullGDL::GetSingleInstance() only of it is undefined. If not scalar, not equal, if defined, not equal.
      if (r == NullGDL::GetSingleInstance()) {
        DObj pVal;
        if (this->Scalar(pVal)) {
          if (pVal == 0) return new Data_<SpDByte>(1);
          return new DByteGDL(!interpreter->ObjValid(pVal));
        }
        Data_<SpDByte>* res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
        (*res)[0] = (0 == (*this)[0]);
        return res;
      }
    } else //( EQOverload != NULL)
    {
      ProgNodeP callingNode = interpreter->GetRetTree();
      // hidden SELF is counted as well
      int nParSub = EQOverload->NPar();
      assert(nParSub >= 1); // SELF
      if (nParSub < 3) // (SELF), LEFT, RIGHT
      {
        throw GDLException(callingNode, EQOverload->ObjectName() +
          ": Incorrect number of arguments.",
          false, false);
      }
      EnvUDT* newEnv;
      DObjGDL* self;
      Guard<BaseGDL> selfGuard;
      // Dup() here is not optimal
      // avoid at least for internal overload routines (which do/must not change SELF or r)
      bool internalDSubUD = EQOverload->GetTree()->IsWrappedNode();
      if (internalDSubUD) {
        self = this;
        newEnv = new EnvUDT(callingNode, EQOverload, &self);
        newEnv->SetNextParUnchecked((BaseGDL**) & self); // LEFT  parameter
        newEnv->SetNextParUnchecked(&r); // RVALUE  parameter, as reference to prevent cleanup in newEnv
      } else {
        self = this->Dup();
        selfGuard.Init(self);
        newEnv = new EnvUDT(callingNode, EQOverload, &self);
        newEnv->SetNextParUnchecked(this->Dup()); // LEFT  parameter, as value
        newEnv->SetNextParUnchecked(r->Dup()); // RIGHT parameter, as value
      }


      // better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
      StackGuard<EnvStackT> guard(interpreter->CallStack());

      interpreter->CallStack().push_back(newEnv);

      // make the call
      BaseGDL* res = interpreter->call_fun(static_cast<DSubUD*> (newEnv->GetPro())->GetTree());

      if (!internalDSubUD && self != selfGuard.Get()) {
        // always put out warning first, in case of a later crash
        Warning("WARNING: " + EQOverload->ObjectName() +
          ": Assignment to SELF detected (GDL session still ok).");
        // assignment to SELF -> self was deleted and points to new variable
        // which it owns
        selfGuard.Release();
        if (static_cast<BaseGDL*> (self) != NullGDL::GetSingleInstance())
          selfGuard.Reset(self);
      }

      return res;
    }
  } // if( StrictScalar())

  // handle type conversion first
  // here r can be of any GDL type (due to operator overloading)
  if (r->Type() != GDL_OBJ) {
    if (r == NullGDL::GetSingleInstance()) // "this" is not scalar here -> return always false
    {
      Data_<SpDByte>* res = new Data_<SpDByte>(0);
      return res;
    }
    throw GDLException("Unable to convert variable to type object reference.", true, false);
  }

  // same code as for other types from here
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);

  Data_<SpDByte>* res;

  Ty s;
  if (right->StrictScalar(s)) {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (nEl == 1) {
      (*res)[0] = (s == (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] == s);
    }
  } else if (StrictScalar(s)) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] == s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] == s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] == s);
    }
  } else if (rEl < nEl) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] == (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] == (*this)[i]);
    }
  } else // ( rEl >= nEl)
  {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] == (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] == (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] == (*this)[i]);
    }
  }
  return res;
}

// NeOp
// returns *this ne *r, //C deletes itself and right
// ex b=(a ne 0)
template<class Sp>
BaseGDL* Data_<Sp>::NeOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);

  Data_<SpDByte>* res;

  Ty s;
  if (right->StrictScalar(s)) {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (nEl == 1) {
      (*res)[0] = (s != (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] != s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] != s);
    }
  } else if (StrictScalar(s)) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] != s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != s);
    }
  } else if (rEl < nEl) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != (*this)[i]);
    }
  } else // ( rEl >= nEl)
  {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] != (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] != (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] != (*this)[i]);
    }
  }
  return res;
}

// must handle overloads

template<>
BaseGDL* Data_<SpDObj>::NeOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if (Scalar()) {
    DSubUD* NEOverload =
      static_cast<DSubUD*> (GDLInterpreter::GetObjHeapOperator((*this)[0], OONE));
    if (NEOverload == NULL) {
      if (r == NullGDL::GetSingleInstance()) {
        Data_<SpDByte>* res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
        (*res)[0] = (0 != (*this)[0]);
        return res;
      }
    } else //      if( NEOverload != NULL)
    {
      ProgNodeP callingNode = interpreter->GetRetTree();
      // hidden SELF is counted as well
      int nParSub = NEOverload->NPar();
      assert(nParSub >= 1); // SELF
      if (nParSub < 3) // (SELF), LEFT, RIGHT
      {
        throw GDLException(callingNode, NEOverload->ObjectName() +
          ": Incorrect number of arguments.",
          false, false);
      }
      EnvUDT* newEnv;
      DObjGDL* self;
      Guard<BaseGDL> selfGuard;
      // Dup() here is not optimal
      // avoid at least for internal overload routines (which do/must not change SELF or r)
      bool internalDSubUD = NEOverload->GetTree()->IsWrappedNode();
      if (internalDSubUD) {
        self = this;
        newEnv = new EnvUDT(callingNode, NEOverload, &self);
        newEnv->SetNextParUnchecked((BaseGDL**) & self); // LEFT  parameter
        newEnv->SetNextParUnchecked(&r); // RVALUE  parameter, as reference to prevent cleanup in newEnv
      } else {
        self = this->Dup();
        selfGuard.Init(self);
        newEnv = new EnvUDT(callingNode, NEOverload, &self);
        newEnv->SetNextParUnchecked(this->Dup()); // LEFT  parameter, as value
        newEnv->SetNextParUnchecked(r->Dup()); // RIGHT parameter, as value
      }


      // better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
      StackGuard<EnvStackT> guard(interpreter->CallStack());

      interpreter->CallStack().push_back(newEnv);

      // make the call
      BaseGDL* res = interpreter->call_fun(static_cast<DSubUD*> (newEnv->GetPro())->GetTree());

      if (!internalDSubUD && self != selfGuard.Get()) {
        // always put out warning first, in case of a later crash
        Warning("WARNING: " + NEOverload->ObjectName() +
          ": Assignment to SELF detected (GDL session still ok).");
        // assignment to SELF -> self was deleted and points to new variable
        // which it owns
        selfGuard.Release();
        if (static_cast<BaseGDL*> (self) != NullGDL::GetSingleInstance())
          selfGuard.Reset(self);
      }

      return res;
    }
  } // if( StrictScalar())

  // handle type conversion first
  // here r can be of any GDL type (due to operator overloading)
  if (r->Type() != GDL_OBJ) {
    if (r == NullGDL::GetSingleInstance()) // "this" is not scalar here -> return always true
    {
      Data_<SpDByte>* res = new Data_<SpDByte>(1);
      return res;
    }

    throw GDLException("Unable to convert variable to type object reference.", true, false);
  }

  // same code as for other types from here
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);

  Data_<SpDByte>* res;

  Ty s;
  if (right->StrictScalar(s)) {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (nEl == 1) {
      (*res)[0] = (s != (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] != s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] != s);
    }
  } else if (StrictScalar(s)) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] != s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != s);
    }
  } else if (rEl < nEl) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != (*this)[i]);
    }
  } else // ( rEl >= nEl)
  {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] != (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] != (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] != (*this)[i]);
    }
  }
  return res;
}

// LeOp
// returns *this le *r, //C deletes itself and right
//ex:b=(a le 0)
template<class Sp>
BaseGDL* Data_<Sp>::LeOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);

  Data_<SpDByte>* res;

  Ty s;
  if (right->StrictScalar(s)) {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (nEl == 1) {
      (*res)[0] = ((*this)[0] <= s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] <= s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] <= s);
    }
  } else if (StrictScalar(s)) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] >= s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] >= s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] >= s);
    }
  } else if (rEl < nEl) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] >= (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] >= (*this)[i]);
    }
  } else // ( rEl >= nEl)
  {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] >= (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] >= (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] >= (*this)[i]);
    }
  }
  return res;
}
// invalid types

template<>
BaseGDL* Data_<SpDPtr>::LeOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDObj>::LeOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDComplex>::LeOp(BaseGDL* r) {  TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_LeOpCplx.incpp"
}

template<>
BaseGDL* Data_<SpDComplexDbl>::LeOp(BaseGDL* r) {  TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_LeOpCplx.incpp"
}

// LtOp
// returns *this lt *r, //C deletes itself and right
// ex: b=(a lt 0)
template<class Sp>
BaseGDL* Data_<Sp>::LtOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);

  Data_<SpDByte>* res;

  Ty s;
  if (right->StrictScalar(s)) {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (nEl == 1) {
      (*res)[0] = ((*this)[0] < s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] < s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] < s);
    }
  } else if (StrictScalar(s)) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] > s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] > s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] > s);
    }
  } else if (rEl < nEl) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] > (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] > (*this)[i]);
    }
  } else // ( rEl >= nEl)
  {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] > (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] > (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] > (*this)[i]);
    }
  }
  return res;
}
// invalid types

template<>
BaseGDL* Data_<SpDPtr>::LtOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDObj>::LtOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDComplex>::LtOp(BaseGDL* r) {  TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_LtOpCplx.incpp"
}

template<>
BaseGDL* Data_<SpDComplexDbl>::LtOp(BaseGDL* r) {  TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_LtOpCplx.incpp"
}

// GeOp
// returns *this ge *r, //C deletes itself and right
// ex: b=(a ge 0)
template<class Sp>
BaseGDL* Data_<Sp>::GeOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);

  Data_<SpDByte>* res;

  Ty s;
  if (right->StrictScalar(s)) {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (nEl == 1) {
      (*res)[0] = ((*this)[0] >= s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] >= s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] >= s);
    }
  } else if (StrictScalar(s)) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] <= s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] <= s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] <= s);
    }
  } else if (rEl < nEl) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] <= (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] <= (*this)[i]);
    }
  } else // ( rEl >= nEl)
  {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] <= (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] <= (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] <= (*this)[i]);
    }
  }
  return res;
}
// invalid types

template<>
BaseGDL* Data_<SpDPtr>::GeOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDObj>::GeOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDComplex>::GeOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_GeOpCplx.incpp"
}

template<>
BaseGDL* Data_<SpDComplexDbl>::GeOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_GeOpCplx.incpp"
}

// GtOp
// returns *this gt *r, //C deletes itself and right
// ex: b=(a gt 0)
template<class Sp>
BaseGDL* Data_<Sp>::GtOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);

  Data_<SpDByte>* res;

  Ty s;
  if (right->StrictScalar(s)) {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (nEl == 1) {
      (*res)[0] = ((*this)[0] > s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] > s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*this)[i] > s);
    }
  } else if (StrictScalar(s)) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] < s);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] < s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] < s);
    }
  } else if (rEl < nEl) {
    res = new Data_<SpDByte>(right->dim, BaseGDL::NOZERO);

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] < (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] < (*this)[i]);
    }
  } else // ( rEl >= nEl)
  {
    res = new Data_<SpDByte>(this->dim, BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] < (*this)[0]);
      return res;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] < (*this)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] < (*this)[i]);
    }
  }
  return res;
}
// invalid types

template<>
BaseGDL* Data_<SpDPtr>::GtOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDObj>::GtOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDComplex>::GtOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_GtOpCplx.incpp"
}

template<>
BaseGDL* Data_<SpDComplexDbl>::GtOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_GtOpCplx.incpp"
}

//#undef USE_EIGEN
// MatrixOp
// returns *this # *r, //C does not delete itself and right

template<class Sp>
Data_<Sp>* Data_<Sp>::MatrixOp(BaseGDL* r, bool atranspose, bool btranspose) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  bool at = atranspose;
  bool bt = btranspose;

  Data_* par1 = static_cast<Data_*> (r);

  long NbCol0, NbRow0, NbCol1, NbRow1; //, NbCol2, NbRow2;
  SizeT rank0 = this->Rank();
  if (rank0 > 2)
    throw GDLException("Array must have 1 or 2 dimensions", true, false);
  SizeT rank1 = par1->Rank();
  if (rank1 > 2)
    throw GDLException("Array must have 1 or 2 dimensions", true, false);

  NbCol0 = this->Dim(0);
  if (NbCol0 == 0) NbCol0 = 1;
  NbRow0 = (rank0 == 2) ? this->Dim(1) : 1;

  NbCol1 = par1->Dim(0);
  if (NbCol1 == 0) NbCol1 = 1;
  NbRow1 = (rank1 == 2) ? par1->Dim(1) : 1;
  // NbCol0, NbRow0, NbCol1, NbRow1 are properly set now

  // vector cases (possible degeneration)
  if (rank0 <= 1 || rank1 <= 1) {
    if (rank0 <= 1 && rank1 <= 1) {
      // [NbCol0,1]#[NbCol1,1] -> just transpose b (if a is not transposed)
      if (!at) // && !bt
        bt = true;
    } else if (rank0 <= 1) // rank1 == 2
    {
      // [NbCol0,1]#[NbCol1,NbRow1]
      if (!at && ((!bt && NbCol1 != 1) || (bt && NbRow1 != 1)))
        at = true;
    } else // if( rank1 <= 1) // rank0 == 2
    {
      // [NbCol0,NbRow0]#[NbCol1,1]
      if (!bt && ((!at && NbRow0 == 1) || (at && NbCol0 == 1)))
        bt = true;
    }
  }

#ifdef USE_EIGEN

  Map < Matrix<Ty, -1, -1 >, Aligned > m0(&(*this)[0], NbCol0, NbRow0);
  Map < Matrix<Ty, -1, -1 >, Aligned > m1(&(*par1)[0], NbCol1, NbRow1);

  if (at && bt) {
    if (/*(at &&  bt) &&*/ (NbCol0 != NbRow1)) {
      throw GDLException("Operands of matrix multiply have incompatible dimensions.atbt", true, false);
    }
    long& NbCol2 = NbRow0;
    long& NbRow2 = NbCol1;
    dimension dim(NbCol2, NbRow2);

    Data_* res = new Data_(dim, BaseGDL::NOZERO);
    // no guarding necessary: eigen only throws on memory allocation

    Map < Matrix<Ty, -1, -1 >, Aligned > m2(&(*res)[0], NbCol2, NbRow2);
    m2.noalias() = m0.transpose() * m1.transpose();
    return res;
  } else if (bt) {
    if (/*(!at &&  bt) &&*/ (NbRow0 != NbRow1)) {
      throw GDLException("Operands of matrix multiply have incompatible dimensions.bt", true, false);
    }
    long& NbCol2 = NbCol0;
    long& NbRow2 = NbCol1;
    dimension dim(NbCol2, NbRow2);

    Data_* res = new Data_(dim, BaseGDL::NOZERO);
    Map < Matrix<Ty, -1, -1 >, Aligned > m2(&(*res)[0], NbCol2, NbRow2);
    m2.noalias() = m0 * m1.transpose();
    return res;
  } else if (at) {
    if (/*(at && !bt) &&*/ (NbCol0 != NbCol1)) {
      throw GDLException("Operands of matrix multiply have incompatible dimensions.at", true, false);
    }
    long& NbCol2 = NbRow0;
    long& NbRow2 = NbRow1;
    dimension dim(NbCol2, NbRow2);

    Data_* res = new Data_(dim, BaseGDL::NOZERO);
    Map < Matrix<Ty, -1, -1 >, Aligned > m2(&(*res)[0], NbCol2, NbRow2);
    m2.noalias() = m0.transpose() * m1;
    return res;
  } else {
    if (/*(!at && !bt) &&*/ (NbRow0 != NbCol1)) {
      throw GDLException("Operands of matrix multiply have incompatible dimensions._", true, false);
    }
    long& NbCol2 = NbCol0;
    long& NbRow2 = NbRow1;
    dimension dim(NbCol2, NbRow2);

    Data_* res = new Data_(dim, BaseGDL::NOZERO);
    Map < Matrix<Ty, -1, -1 >, Aligned > m2(&(*res)[0], NbCol2, NbRow2);
    m2.noalias() = m0*m1;
    return res;
  }

#else

  Data_* right = static_cast<Data_*> (r);
  Data_* res;
  // right op 1st
  SizeT nRow, nRowEl;
  if (bt) {
    nRow = NbCol1;
    nRowEl = NbRow1;
  } else {
    nRow = NbRow1;
    nRowEl = NbCol1;
  }

  SizeT nCol, nColEl;
  if (at) {
    nCol = NbRow0;
    nColEl = NbCol0;
  } else {
    nCol = NbCol0;
    nColEl = NbRow0;
  }

  if (nColEl != nRowEl)
    throw GDLException("Operands of matrix multiply have"
    " incompatible dimensions.", true, false);

  if (nRow > 1)
    res = New(dimension(nCol, nRow), BaseGDL::NOZERO);
  else
    res = New(dimension(nCol), BaseGDL::NOZERO);
  SizeT rIxEnd = nRow * nColEl;
  SizeT nOp = rIxEnd * nCol;

#ifdef USE_STRASSEN_MATRIXMULTIPLICATION
  if (!bt && !at && strassen) {
    SizeT maxDim;
    if (nCol >= nColEl && nCol >= nRow)
      maxDim = nCol;
    else if (nColEl >= nRow)
      maxDim = nColEl;
    else
      maxDim = nRow;

    SizeT sOp = maxDim * maxDim * maxDim;

    {
      SizeT mSz = 2;
      while (mSz < maxDim) mSz <<= 1;

      SM1<Ty>(mSz, nCol, nColEl, nRow,
        static_cast<Ty*> (right->DataAddr()),
        static_cast<Ty*> (this->DataAddr()),
        static_cast<Ty*> (res->DataAddr()));

      return res;
    }
  }
#endif

  if (!at) // normal
  {
    if (!bt) // normal
    {

      if ((GDL_NTHREADS=parallelize( nOp))==1) {
        for (OMPInt colA = 0; colA < nCol; ++colA) // res dim 0
          for (OMPInt rIx = 0, rowBnCol = 0; rIx < rIxEnd;
            rIx += nColEl, rowBnCol += nCol) // res dim 1
          {
            Ty& resEl = (*res)[ rowBnCol + colA];
            resEl = 0; //(*this)[ colA] * (*right)[ rIx]; // initialization
            for (OMPInt i = 0; i < nColEl; ++i)
              resEl += (*this)[ i * nCol + colA] * (*right)[ rIx + i];
          }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt colA = 0; colA < nCol; ++colA) // res dim 0
          for (OMPInt rIx = 0, rowBnCol = 0; rIx < rIxEnd;
            rIx += nColEl, rowBnCol += nCol) // res dim 1
          {
            Ty& resEl = (*res)[ rowBnCol + colA];
            resEl = 0; //(*this)[ colA] * (*right)[ rIx]; // initialization
            for (OMPInt i = 0; i < nColEl; ++i)
              resEl += (*this)[ i * nCol + colA] * (*right)[ rIx + i];
          }
      }
    } else // transpose r
    {

      if ((GDL_NTHREADS=parallelize( nOp))==1) {
        for (OMPInt colA = 0; colA < nCol; ++colA) // res dim 0
          for (OMPInt rIx = 0, rowBnCol = 0; rIx < rIxEnd;
            rIx += nColEl, rowBnCol += nCol) // res dim 1
          {
            Ty& resEl = (*res)[ rowBnCol + colA];
            resEl = 0; //(*this)[ colA] * (*right)[ rIx]; // initialization
            for (OMPInt i = 0; i < nColEl; ++i)
              resEl += (*this)[ i * nCol + colA] * (*right)[ rIx + i];
          }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt colA = 0; colA < nCol; ++colA) // res dim 0
          for (OMPInt rIx = 0, rowBnCol = 0; rIx < nRow; ++rIx, rowBnCol += nCol) // res dim 1
          {
            Ty& resEl = (*res)[ rowBnCol + colA];
            resEl = 0; //(*this)[ colA] * (*right)[ rIx]; // initialization
            for (OMPInt i = 0; i < nColEl; ++i)
              resEl += (*this)[ i * nCol + colA] * (*right)[ rIx + i * nRow];
          }
      }
    }
  } else // atranspose
  {
    if (!bt) // normal
    {

      if ((GDL_NTHREADS=parallelize( nOp))==1) {
        for (OMPInt colA = 0; colA < nCol; ++colA) // res dim 0
          for (OMPInt rIx = 0, rowBnCol = 0; rIx < rIxEnd;
            rIx += nColEl, ++rowBnCol) // res dim 1
          {
            Ty& resEl = (*res)[ rowBnCol * nCol + colA];
            resEl = 0; //(*this)[ colA] * (*right)[ rIx]; // initialization
            for (OMPInt i = 0; i < nColEl; ++i)
              resEl += (*this)[ i + colA * nColEl] * (*right)[ rIx + i];
          }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt colA = 0; colA < nCol; ++colA) // res dim 0
          for (OMPInt rIx = 0, rowBnCol = 0; rIx < rIxEnd;
            rIx += nColEl, ++rowBnCol) // res dim 1
          {
            Ty& resEl = (*res)[ rowBnCol * nCol + colA];
            resEl = 0; //(*this)[ colA] * (*right)[ rIx]; // initialization
            for (OMPInt i = 0; i < nColEl; ++i)
              resEl += (*this)[ i + colA * nColEl] * (*right)[ rIx + i];
          }
      }
    } else // transpose r
    {

      if ((GDL_NTHREADS=parallelize( nOp))==1) {
        for (OMPInt colA = 0; colA < nCol; ++colA) // res dim 0
          for (OMPInt rIx = 0; rIx < nRow; ++rIx) // res dim 1
          {
            Ty& resEl = (*res)[ rIx * nCol + colA];
            resEl = 0; //(*this)[ colA] * (*right)[ rIx]; // initialization
            for (OMPInt i = 0; i < nColEl; ++i)
              resEl += (*this)[ i + colA * nColEl] * (*right)[ rIx + i * nRow];
          }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt colA = 0; colA < nCol; ++colA) // res dim 0
          for (OMPInt rIx = 0; rIx < nRow; ++rIx) // res dim 1
          {
            Ty& resEl = (*res)[ rIx * nCol + colA];
            resEl = 0; //(*this)[ colA] * (*right)[ rIx]; // initialization
            for (OMPInt i = 0; i < nColEl; ++i)
              resEl += (*this)[ i + colA * nColEl] * (*right)[ rIx + i * nRow];
          }
      }
    }
  }

  return res;

#endif // #elseif USE_EIGEN
}





// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::MatrixOp(BaseGDL* r, bool atranspose, bool btranspose) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::MatrixOp(BaseGDL* r, bool atranspose, bool btranspose) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::MatrixOp(BaseGDL* r, bool atranspose, bool btranspose) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}


// 2. operators which operate on 'this'
// AndOp
// Ands right to itself, //C deletes right
// right must always have more or same number of elements
// for integers

template<class Sp>
Data_<Sp>* Data_<Sp>::AndOp(BaseGDL* r)
// GDL_DEFINE_INTEGER_FUNCTION( Data_<Sp>*) AndOp( BaseGDL* r)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    (*this)[0] = (*this)[0] & (*right)[0]; // & Ty(1);
    return this;
  }
  // note: we can't use valarray operation here as right->dd
  // might be larger than this->dd

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = (*this)[i] & (*right)[i]; // & Ty(1);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = (*this)[i] & (*right)[i]; // & Ty(1);
  }
  return this;
}
// different for floats

template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpInv(BaseGDL* right) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return AndOp(right);
}
// for floats

template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_AndOpCplx.incpp"
}


template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    if ((*this)[0] != zero) (*this)[0] = (*right)[0];
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*this)[i] = (*right)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*this)[i] = (*right)[i];
  }
  return this;
}

// for doubles

template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_AndOpCplx.incpp"
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    if ((*this)[0] != zero) (*this)[0] = (*right)[0];
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*this)[i] = (*right)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*this)[i] = (*right)[i];
  }
  return this;
}

// invalid types
// GDL_DEFINE_COMPLEX_FUNCTION( Data_<Sp>*) AndOp( BaseGDL* r)
// {
//   throw GDLException("Cannot apply operation to datatype "+Sp::str+".",true,false);
//   return this;
// }

template<>
Data_<SpDComplex>* Data_<SpDComplex>::AndOp(BaseGDL* r) { 
#include "snippets/basic_op_AndOpCplx.incpp"
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOp(BaseGDL* r) { 
#include "snippets/basic_op_AndOpCplx.incpp"
}

// GDL_DEFINE_OTHER_FUNCTION( Data_<Sp>*) AndOp( BaseGDL* r)
// {
//   throw GDLException("Cannot apply operation to datatype "+Sp::str+".",true,false);
//   return this;
// }

template<>
Data_<SpDString>* Data_<SpDString>::AndOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

// template<>
// Data_<SpDString>* Data_<SpDString>::AndOpInv( BaseGDL* r)
// { 
//  throw GDLException("Cannot apply operation to datatype STRING.",true,false);
//  return this;
// }

template<>
Data_<SpDPtr>* Data_<SpDPtr>::AndOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

// template<>
// Data_<SpDPtr>* Data_<SpDPtr>::AndOpInv( BaseGDL* r)
// { 
//  throw GDLException("Cannot apply operation to datatype PTR.",true,false);
//  return this;
// }

template<>
Data_<SpDObj>* Data_<SpDObj>::AndOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

// template<>
// Data_<SpDObj>* Data_<SpDObj>::AndOpInv( BaseGDL* r)
// { 
//  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);
//  return this;
// }

template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);

  Ty s = (*right)[0];

  if (nEl == 1) {
    (*this)[0] &= s;
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] &= s;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) shared(s)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] &= s;
  }
  return this;
}
// different for floats

template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpInvS(BaseGDL* right) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return AndOpS(right);
}
// for floats

template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  // right->Scalar(s);
  if (s == zero) {
    {
      for (SizeT i = 0; i < nEl; ++i) (*this)[i] = zero;
    }
  }
  return this;
}

template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInvS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();

  assert(nEl);
  Ty s = (*right)[0];
  if (s == zero) {
    {
      for (SizeT i = 0; i < nEl; ++i)
        (*this)[i] = zero;
    }
  } else {
    if (nEl == 1) {
      if ((*this)[0] != zero) (*this)[0] = s;
      return this;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*this)[i] = s;
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*this)[i] = s;
    }
  }
  return this;
}

// for doubles

template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();

  assert(nEl);
  Ty s = (*right)[0];
  if (s == zero)
  {
    for (SizeT i = 0; i < nEl; ++i)
      (*this)[i] = zero;
  }
  return this;
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInvS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  if (s == zero) {
    {
      for (SizeT i = 0; i < nEl; ++i)
        (*this)[i] = zero;
    }
  } else {
    if (nEl == 1) {
      if ((*this)[0] != zero) (*this)[0] = s;
      return this;
    }

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*this)[i] = s;
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*this)[i] = s;
    }
  }
  return this;
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::AndOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::AndOpS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_AndOpSCplx.incpp"
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOpS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_AndOpSCplx.incpp"
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::AndOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::AndOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}


// OrOp
// Ors right to itself, //C deletes right
// right must always have more or same number of elements
// for integers

template<class Sp>
Data_<Sp>* Data_<Sp>::OrOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    (*this)[0] = (*this)[0] | (*right)[0]; // | Ty(1);
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = (*this)[i] | (*right)[i]; // | Ty(1);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = (*this)[i] | (*right)[i]; // | Ty(1);
  }
  return this;
}
// different for floats

template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpInv(BaseGDL* right) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return OrOp(right);
}
// for floats

template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    if ((*this)[0] == zero) (*this)[0] = (*right)[0];
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] == zero) (*this)[i] = (*right)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] == zero) (*this)[i] = (*right)[i];
  }
  return this;
}

template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    if ((*right)[0] != zero) (*this)[0] = (*right)[0];
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*right)[i] != zero) (*this)[i] = (*right)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*right)[i] != zero) (*this)[i] = (*right)[i];
  }
  return this;
}
// for doubles

template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    if ((*this)[0] == zero) (*this)[0] = (*right)[0];
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] == zero) (*this)[i] = (*right)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] == zero) (*this)[i] = (*right)[i];
  }
  return this;
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    if ((*right)[0] != zero) (*this)[0] = (*right)[0];
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*right)[i] != zero) (*this)[i] = (*right)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*right)[i] != zero) (*this)[i] = (*right)[i];
  }
  return this;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::OrOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::OrOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpCplx.incpp"
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOp(BaseGDL* r) {  TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpCplx.incpp"
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::OrOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::OrOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}
// OrOp
// Ors right to itself, //C deletes right
// right must always have more or same number of elements
// for integers

template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  if (nEl == 1) {
    (*this)[0] = (*this)[0] | s;
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = (*this)[i] | s;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = (*this)[i] | s;
  }
  return this;
}
// different for floats

template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpInvS(BaseGDL* right) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return OrOpS(right);
}

template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpS(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpSCplx.incpp"
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpS(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpSCplx.incpp"
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::OrOpS(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpSCplx.incpp"
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOpS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpSCplx.incpp"
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::OrOpS(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::OrOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::OrOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

// XorOp
// Xors right to itself, //C deletes right
// right must always have more or same number of elements
// for integers

template<class Sp>
Data_<Sp>* Data_<Sp>::XorOp(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    (*this)[0] ^= (*right)[0];
    return this;
  }
  Ty s = (*right)[0];
  if (right->StrictScalar(s)) {
    if (s != Sp::zero) {

      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*this)[i] ^= s;
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*this)[i] ^= s;
      }
    }
  } else {

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] ^= (*right)[i];
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*this)[i] ^= (*right)[i];
    }
  }
  return this;
}
// invalid types

template<>
Data_<SpDFloat>* Data_<SpDFloat>::XorOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype FLOAT.", true, false);
  return this;
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::XorOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype DOUBLE.", true, false);
  return this;
}

template<>
Data_<SpDString>* Data_<SpDString>::XorOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::XorOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::XorOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::XorOp(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<class Sp>
Data_<Sp>* Data_<Sp>::XorOpS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    (*this)[0] ^= /*(*this)[0] ^*/ (*right)[0];
    return this;
  }
  Ty s = (*right)[0];

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] ^= s;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] ^= s;
  }
  return this;
}
// different for floats
// for floats

template<>
Data_<SpDFloat>* Data_<SpDFloat>::XorOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype FLOAT.", true, false);
  return this;
}
// for doubles

template<>
Data_<SpDDouble>* Data_<SpDDouble>::XorOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype DOUBLE.", true, false);
  return this;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::XorOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::XorOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::XorOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::XorOpS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

// LtMark
// LtMarks right to itself, //C deletes right
// right must always have more or same number of elements

template<class Sp>
Data_<Sp>* Data_<Sp>::LtMark(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    if ((*this)[0] > (*right)[0]) (*this)[0] = (*right)[0];
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] > (*right)[i]) (*this)[i] = (*right)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] > (*right)[i]) (*this)[i] = (*right)[i];
  }
  return this;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::LtMark(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::LtMark(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::LtMark(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::LtMark(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::LtMark(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<class Sp>
Data_<Sp>* Data_<Sp>::LtMarkS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    if ((*this)[0] > (*right)[0]) (*this)[0] = (*right)[0];
    return this;
  }
  Ty s = (*right)[0];

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] > s) (*this)[i] = s;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] > s) (*this)[i] = s;
  }
  return this;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::LtMarkS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::LtMarkS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::LtMarkS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::LtMarkS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::LtMarkS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}
// GtMark
// GtMarks right to itself, //C deletes right
// right must always have more or same number of elements

template<class Sp>
Data_<Sp>* Data_<Sp>::GtMark(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    if ((*this)[0] < (*right)[0]) (*this)[0] = (*right)[0];
    return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] < (*right)[i]) (*this)[i] = (*right)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] < (*right)[i]) (*this)[i] = (*right)[i];
  }
  return this;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::GtMark(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::GtMark(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::GtMark(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::GtMark(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::GtMark(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<class Sp>
Data_<Sp>* Data_<Sp>::GtMarkS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    if ((*this)[0] < (*right)[0]) (*this)[0] = (*right)[0];
    return this;
  }

  Ty s = (*right)[0];

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] < s) (*this)[i] = s;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] < s) (*this)[i] = s;
  }
  return this;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::GtMarkS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::GtMarkS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::GtMarkS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::GtMarkS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::GtMarkS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}


// Mod

inline DFloat Modulo(const DFloat& l, const DFloat& r) {
  return fmodf(l, r); //fmodf returns a float
}
// in basic_op.cpp
// double modulo division: left=left % right

inline DDouble DModulo(const DDouble& l, const DDouble& r) {
  return fmod(l, r); //fmod returns a double
}

// modulo division: left=left % right
// right is of size > 1 (otherwise ModS would have been called)
// catching FPE exceptions for FP, avoiding segfault on integer zero divide for integers

template<class Sp> //avoiding segfault on integer zero divide for integers, FP treatment specialized below
Data_<Sp>* Data_<Sp>::Mod(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  if (nEl == 1) { //accelerator for single value on LEFT 
    if ((*right)[0] != this->zero) (*this)[0] %= (*right)[0]; else (*this)[0] = this->zero;
	return this;
  }
  // Multiple LEFT values, parallel section slowed by inner 'if' clause.
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
      for (OMPInt ix = 0; ix < nEl; ++ix) if ((*right)[ix] != this->zero) (*this)[ix] %= (*right)[ix]; else (*this)[ix] = this->zero;
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt ix = 0; ix < nEl; ++ix) if ((*right)[ix] != this->zero) (*this)[ix] %= (*right)[ix]; else (*this)[ix] = this->zero;
  }

  return this;
}
// float modulo division: left=left % right


template<> // catching FPE exceptions for FP
Data_<SpDFloat>* Data_<SpDFloat>::Mod(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
	(*this)[0] = Modulo((*this)[0], (*right)[0]);
	GDLStopRegisteringFPExceptions();
	return this;
  }

  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = Modulo((*this)[i], (*right)[i]);
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = Modulo((*this)[i], (*right)[i]);
  }

  GDLStopRegisteringFPExceptions();

  return this;
}



template<> // catching FPE exceptions for FP
Data_<SpDDouble>* Data_<SpDDouble>::Mod(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
	(*this)[0] = DModulo((*this)[0], (*right)[0]);
	GDLStopRegisteringFPExceptions();
	return this;
  }

  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = DModulo((*this)[i], (*right)[i]);
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = DModulo((*this)[i], (*right)[i]);
  }

  GDLStopRegisteringFPExceptions();

  return this;
}
  // -------------------------------------------------------------------------------------------------------
  // ModInv
  // inverse modulo division: left=right % left
  // right is of size > 1 (otherwise ModInvS would have been called)
  // catching FPE exceptions for FP, avoiding segfault on integer zero divide for integers

  template<class Sp > //avoiding segfault on integer zero divide for integers, FP treatment specialzed below
  Data_<Sp>* Data_<Sp>::ModInv(BaseGDL * r) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	Data_* right = static_cast<Data_*> (r);

	ULong nEl = N_Elements();
	assert(nEl);
	if (nEl == 1) { //accelerator for LEFT singleton
	  if ((*this)[0] != this->zero) (*this)[0] = (*right)[0] % (*this)[0];  else (*this)[0] = this->zero;
	  return this;
	}
	// Multiple LEFT values, parallel section slowed by inner 'if' clause.
	if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	  for (OMPInt ix = 0; ix < nEl; ++ix) if ((*this)[ix] != this->zero) (*this)[ix] = (*right)[ix] % (*this)[ix]; else (*this)[ix] = this->zero;
	} else {
	  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
		for (OMPInt ix = 0; ix < nEl; ++ix) if ((*this)[ix] != this->zero) (*this)[ix] = (*right)[ix] % (*this)[ix]; else (*this)[ix] = this->zero;
	}

	return this;
  }
  
  // float  inverse modulo division: left=right % left

template<> // catching FPE exceptions
Data_<SpDFloat>* Data_<SpDFloat>::ModInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
	(*this)[0] = Modulo((*right)[0], (*this)[0]);
    GDLStopRegisteringFPExceptions();
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = Modulo((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = Modulo((*right)[i], (*this)[i]);
  }

  GDLStopRegisteringFPExceptions();
  
  return this;
}

// double inverse modulo division: left=right % left

template<>// catching FPE exceptions
Data_<SpDDouble>* Data_<SpDDouble>::ModInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
	(*this)[0] = DModulo((*right)[0], (*this)[0]);
    GDLStopRegisteringFPExceptions();
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = DModulo((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = DModulo((*right)[i], (*this)[i]);
  }

  GDLStopRegisteringFPExceptions();
  
  return this;
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::Mod(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDString>* Data_<SpDString>::ModInv(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::Mod(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::Mod(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModInv(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModInv(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::Mod(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModInv(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::Mod(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::ModInv(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

//----------------------------------------------------------------------------------------------------
// ModS
// modulo division: left=left % right
// right is of size 1 (otherwise Mod would have been called)
// catching FPE exceptions for FP, avoiding segfault on integer zero divide for integers
template<class Sp> //avoiding segfault on integer zero divide for integers, FP treatment specialized below
Data_<Sp>* Data_<Sp>::ModS(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);

  Ty s = (*right)[0];

  if (s == this->zero) {
	for (SizeT i = 0; i < nEl; ++i) (*this)[i] = 0;
	//add an FP div by zero to counters
	GDLRegisterADivByZeroException();
	return this;
  }
  //s is not 0 here 
  if (nEl == 1) {
	(*this)[0] %= s;
	return this;
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt ix = 0; ix < nEl; ++ix) (*this)[ix] %= s;
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt ix = 0; ix < nEl; ++ix) (*this)[ix] %= s;
  }
  return this;
}

template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModS(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  GDLStartRegisteringFPExceptions();
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*this)[0] = Modulo((*this)[0], s);
	GDLStopRegisteringFPExceptions();
	return this;
  }

  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = Modulo((*this)[i], s);
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = Modulo((*this)[i], s);
  }

  GDLStopRegisteringFPExceptions();

  return this;
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModS(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  GDLStartRegisteringFPExceptions();
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*this)[0] = DModulo((*this)[0], s);
	GDLStopRegisteringFPExceptions();
	return this;
  }

  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = DModulo((*this)[i], s);
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = DModulo((*this)[i], s);
  }

  GDLStopRegisteringFPExceptions();

  return this;
}
// ModInvS
// inverse modulo division: left=right % left
// right is of size == 1 (otherwise ModInv would have been called)
// catching FPE exceptions for FP, avoiding segfault on integer zero divide for integers; but report an FP exception to the GDL exception reporting system

template<class Sp >//avoiding segfault on integer zero divide for integers, FP treatment specialized below
  Data_<Sp>* Data_<Sp>::ModInvS(BaseGDL * r) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	Data_* right = static_cast<Data_*> (r);

	ULong nEl = N_Elements();
	assert(nEl); 

	Ty s = (*right)[0];
  if (nEl == 1) {
	if ((*this)[0] != this->zero) (*this)[0] = s % (*this)[0];
	else GDLRegisterADivByZeroException(); //add an FP div by zero to counters
	return this;
  }
	
	if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
      for (OMPInt ix = 0; ix < nEl; ++ix) if ((*this)[ix] != this->zero) (*this)[ix] = s % (*this)[ix]; else GDLRegisterADivByZeroException(); //add an FP div by zero to counters
	} else {
	  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt ix = 0; ix < nEl; ++ix) if ((*this)[ix] != this->zero) (*this)[ix] = s % (*this)[ix]; else GDLRegisterADivByZeroException(); //add an FP div by zero to counters
  }
  
	return this;
  }

// float  inverse modulo division: left=right % left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModInvS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  GDLStartRegisteringFPExceptions();
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*this)[0] = Modulo(s, (*this)[0]);
    GDLStopRegisteringFPExceptions();
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = Modulo(s, (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = Modulo(s, (*this)[i]);
  }

  GDLStopRegisteringFPExceptions();
  
  return this;
}

// double inverse modulo division: left=right % left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModInvS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  GDLStartRegisteringFPExceptions();
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*this)[0] = DModulo(s, (*this)[0]);
    GDLStopRegisteringFPExceptions();
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = DModulo(s, (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = DModulo(s, (*this)[i]);
  }

  GDLStopRegisteringFPExceptions();
  
  return this;
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::ModS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDString>* Data_<SpDString>::ModInvS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModInvS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModInvS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModInvS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::ModS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::ModInvS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<class Sp>
Data_<Sp>* Data_<Sp>::Pow(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
	(*this)[0] = pow((*this)[0], (*right)[0]);
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], (*right)[i]); // valarray
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], (*right)[i]); // valarray
  }
  return this;
}
// inverse power of value: left=right ^ left

template<class Sp>
Data_<Sp>* Data_<Sp>::PowInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
	(*this)[0] = pow((*right)[0], (*this)[0]);
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*right)[i], (*this)[i]);
  }
  return this;
}
// floats power of value: left=left ^ right

template<>
Data_<SpDFloat>* Data_<SpDFloat>::Pow(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);
  if (nEl == 1) {
	(*this)[0] = pow((*this)[0], (*right)[0]);
	return this;
  }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], (*right)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], (*right)[i]);
    }
  return this;
}

// anygdl (except complex) power to a GDL_LONG value left=left ^ right
template<class Sp>
Data_<Sp>* Data_<Sp>::PowInt(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  DLongGDL* right = static_cast<DLongGDL*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);
  if (r->StrictScalar()) {
    DLong r0 = (*right)[0];

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = gdl::powI((*this)[i], r0);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = gdl::powI((*this)[i], r0);
    }
    return this;
  }
  if (StrictScalar()) {
    Data_* res = new Data_(right->Dim(), BaseGDL::NOZERO);
    Ty s0 = (*this)[ 0];
	
    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = gdl::powI(s0, (*right)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = gdl::powI(s0, (*right)[ i]);
    }
    return res;
  }
  if (nEl <= rEl) {

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = gdl::powI((*this)[i], (*right)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = gdl::powI((*this)[i], (*right)[i]);
    }
    return this;
  } else {
    Data_* res = new Data_(right->Dim(), BaseGDL::NOZERO);

    if ((GDL_NTHREADS=parallelize( rEl))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = gdl::powI((*this)[i], (*right)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = gdl::powI((*this)[i], (*right)[i]);
    }
    return res;
  }
}
template<>
Data_<SpDString>* Data_<SpDString>::PowInt(BaseGDL* r) {
  assert(0);
  throw GDLException("Internal error: Data_::PowInt called.", true, false);
  return NULL;
}

template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);
  if (nEl == 1) {
	(*this)[0] = pow((*right)[0], (*this)[0]);
	return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*right)[i], (*this)[i]);
  }
  return this;
}
// doubles power of value: left=left ^ right

template<>
Data_<SpDDouble>* Data_<SpDDouble>::Pow(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);
  if (nEl == 1) {
	(*this)[0] = pow((*this)[0], (*right)[0]);
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], (*right)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], (*right)[i]);
  }
  return this;
}
// doubles inverse power of value: left=right ^ left

template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);
  if (nEl == 1) {
	(*this)[0] = pow((*right)[0], (*this)[0]);
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*right)[i], (*this)[i]);
  }
  return this;
}
// complex power of value: left=left ^ right
// complex is special here

template<>
Data_<SpDComplex>* Data_<SpDComplex>::Pow(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();

  assert(nEl > 0);
  assert(r->N_Elements() > 0);

  if (r->Type() == GDL_FLOAT) {
    Data_<SpDFloat>* right = static_cast<Data_<SpDFloat>*> (r);

    DFloat s;
    // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
    // (concerning when a new variable is created vs. using this)
    // (must also be consistent with ComplexDbl)
    if (right->StrictScalar(s)) {

      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      }
      return this;
    } else {
      SizeT rEl = right->N_Elements();
      if (nEl < rEl) {
        DComplex s;
        if (StrictScalar(s)) {
          DComplexGDL* res = new DComplexGDL(right->Dim(),
            BaseGDL::NOZERO);

          if ((GDL_NTHREADS=parallelize( rEl))==1) {
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          }
          return res;
		}
		
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return this;
      } else {
        DComplexGDL* res = new DComplexGDL(right->Dim(),
          BaseGDL::NOZERO);

        if ((GDL_NTHREADS=parallelize( rEl))==1) {
          for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return res;
      }
    }
  }
  if (r->Type() == GDL_LONG) {
    Data_<SpDLong>* right = static_cast<Data_<SpDLong>*> (r);

    DLong s;
    // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
    // (concerning when a new variable is created vs. using this)
    // (must also be consistent with ComplexDbl)
    if (right->StrictScalar(s)) {

      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      }
      return this;
    } else {
      SizeT rEl = right->N_Elements();
      if (nEl < rEl) {
        DComplex s;
        if (StrictScalar(s)) {
          DComplexGDL* res = new DComplexGDL(right->Dim(),
            BaseGDL::NOZERO);

          if ((GDL_NTHREADS=parallelize( rEl))==1) {
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          }
          return res;
		}

        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return this;
      } else {
        DComplexGDL* res = new DComplexGDL(right->Dim(),
          BaseGDL::NOZERO);

        if ((GDL_NTHREADS=parallelize( rEl))==1) {
          for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return res;
      }
    }
  }

  Data_* right = static_cast<Data_*> (r);

#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for (SizeT i = 0; i < nEl; ++i)
    (*this)[ i] = pow((*this)[ i], (*right)[ i]);
#else
	if (nEl == 1) {
	(*this)[0] = pow((*this)[0], (*right)[0]);
	return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], (*right)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], (*right)[i]);
  }
#endif
  return this;
}
// complex inverse power of value: left=right ^ left

template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);
  if (nEl == 1) {
	(*this)[0] = pow((*right)[0], (*this)[0]);
	return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*right)[i], (*this)[i]);
  }
  return this;
}
// double complex power of value: left=left ^ right

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::Pow(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();

  assert(nEl > 0);

  if (r->Type() == GDL_DOUBLE) {
    Data_<SpDDouble>* right = static_cast<Data_<SpDDouble>*> (r);

    assert(right->N_Elements() > 0);

    DDouble s;

    // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
    // (concerning when a new variable is created vs. using this)
    if (right->StrictScalar(s)) {

      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      }
      return this;
    } else {
      SizeT rEl = right->N_Elements();
      if (nEl < rEl) {
        DComplexDbl s;
        if (StrictScalar(s)) {
          DComplexDblGDL* res = new DComplexDblGDL(right->Dim(),
            BaseGDL::NOZERO);

          if ((GDL_NTHREADS=parallelize( rEl))==1) {
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          }
          return res;
        }

        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return this;
      } else {
        DComplexDblGDL* res = new DComplexDblGDL(right->Dim(),
          BaseGDL::NOZERO);

        if ((GDL_NTHREADS=parallelize( rEl))==1) {
          for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return res;
      }
    }
  }
  if (r->Type() == GDL_LONG) {
    Data_<SpDLong>* right = static_cast<Data_<SpDLong>*> (r);

    assert(right->N_Elements() > 0);

    DLong s;

    // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
    // (concerning when a new variable is created vs. using this)
    if (right->StrictScalar(s)) {

      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      }
      return this;
    } else {
      SizeT rEl = right->N_Elements();
      if (nEl < rEl) {
        DComplexDbl s;
        if (StrictScalar(s)) {
          DComplexDblGDL* res = new DComplexDblGDL(right->Dim(),
            BaseGDL::NOZERO);

          if ((GDL_NTHREADS=parallelize( rEl))==1) {
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          }
          return res;
        }

        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return this;
      } else {
        DComplexDblGDL* res = new DComplexDblGDL(right->Dim(),
          BaseGDL::NOZERO);

        if ((GDL_NTHREADS=parallelize( rEl))==1) {
          for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return res;
      }
    }
  }

  Data_* right = static_cast<Data_*> (r);

#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for (SizeT i = 0; i < nEl; ++i)
    (*this)[ i] = pow((*this)[ i], (*right)[ i]);
#else
  if (nEl == 1) {
	(*this)[0] = pow((*this)[0], (*right)[0]);
	return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], (*right)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], (*right)[i]);
  }
#endif
  return this;
}
// double complex inverse power of value: left=right ^ left

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowInv(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for (SizeT i = 0; i < nEl; ++i)
    (*this)[ i] = pow((*right)[ i], (*this)[i]);
#else
  if (nEl == 1) {
	(*this)[0] = pow((*right)[0], (*this)[0]);
	return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*right)[i], (*this)[i]);
  }
#endif
  return this;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::Pow(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDString>* Data_<SpDString>::PowInv(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::Pow(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowInv(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::Pow(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::PowInv(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<class Sp>
Data_<Sp>* Data_<Sp>::PowS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*this)[0] = pow((*this)[0], s);
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], s);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow((*this)[i], s);
  }
  return this;
}
// inverse power of value: left=right ^ left

template<class Sp>
Data_<Sp>* Data_<Sp>::PowInvS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*this)[0] = pow(s, (*this)[0]);
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow(s, (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[i] = pow(s, (*this)[i]);
  }
  return this;
}
// floats power of value: left=left ^ right

template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];

  if (nEl == 1) {
	dd[ 0] = pow(dd[ 0], s); // valarray
	return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) dd[ i] = pow(dd[ i], s); // valarray
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) dd[ i] = pow(dd[ i], s); // valarray
  }
  return this;
}
// floats inverse power of value: left=right ^ left

template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowInvS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  if (nEl == 1) {
	dd[ 0] = pow(s, dd[ 0]); // valarray
	return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) dd[ i] = pow(s, dd[ i]); // valarray
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) dd[ i] = pow(s, dd[ i]); // valarray
  }
  return this;
}
// doubles power of value: left=left ^ right

template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  
  if (nEl == 1) {
	dd[ 0] = pow(dd[ 0],s); // valarray
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) dd[ i] = pow(dd[ i], s); // valarray
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) dd[ i] = pow(dd[ i], s); // valarray
  }
  return this;
}
// doubles inverse power of value: left=right ^ left

template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowInvS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  if (nEl == 1) {
	dd[ 0] = pow(s, dd[ 0]); // valarray
	return this;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) dd[ i] = pow(s, dd[ i]); // valarray
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) dd[ i] = pow(s, dd[ i]); // valarray
  }
  return this;
}
// complex power of value: left=left ^ right
// complex is special here

template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();

  assert(nEl > 0);
  assert(r->N_Elements() > 0);

  if (r->Type() == GDL_FLOAT) {
    Data_<SpDFloat>* right = static_cast<Data_<SpDFloat>*> (r);

    DFloat s;
    // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
    // (concerning when a new variable is created vs. using this)
    // (must also be consistent with ComplexDbl)
    if (right->StrictScalar(s)) {

      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      }
      return this;
    } else {
      SizeT rEl = right->N_Elements();
      if (nEl < rEl) {
        DComplex s;
        if (StrictScalar(s)) {
          DComplexGDL* res = new DComplexGDL(right->Dim(),
            BaseGDL::NOZERO);

          if ((GDL_NTHREADS=parallelize( rEl))==1) {
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          }
          return res;
        }

        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return this;
      } else {
        DComplexGDL* res = new DComplexGDL(right->Dim(),
          BaseGDL::NOZERO);

        if ((GDL_NTHREADS=parallelize( rEl))==1) {
          for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return res;
      }
    }
  }
  if (r->Type() == GDL_LONG) {
    Data_<SpDLong>* right = static_cast<Data_<SpDLong>*> (r);

    DLong s;
    // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
    // (concerning when a new variable is created vs. using this)
    // (must also be consistent with ComplexDbl)
    if (right->StrictScalar(s)) {

      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      }
      return this;
    } else {
      SizeT rEl = right->N_Elements();
      if (nEl < rEl) {
        DComplex s;
        if (StrictScalar(s)) {
          DComplexGDL* res = new DComplexGDL(right->Dim(),
            BaseGDL::NOZERO);

          if ((GDL_NTHREADS=parallelize( rEl))==1) {
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          }
          return res;
        }

        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return this;
      } else {
        DComplexGDL* res = new DComplexGDL(right->Dim(),
          BaseGDL::NOZERO);

        if ((GDL_NTHREADS=parallelize( rEl))==1) {
          for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return res;
      }
    }
  }

  Data_* right = static_cast<Data_*> (r);

  Ty s = (*right)[0];
  if (nEl == 1) {
	(*this)[ 0] = pow((*this)[ 0], s);
	return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
  }
  return this;
}
// complex inverse power of value: left=right ^ left

template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowInvS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*this)[ 0] = pow(s, (*this)[ 0]);
	return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow(s, (*this)[ i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow(s, (*this)[ i]);
  }
  return this;
}
// double complex power of value: left=left ^ right

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();

  assert(nEl > 0);

  if (r->Type() == GDL_DOUBLE) {
    Data_<SpDDouble>* right = static_cast<Data_<SpDDouble>*> (r);

    assert(right->N_Elements() > 0);

    DDouble s;

    // note: changes here have to be reflected in POWNCNode::Eval() (prognodeexpr.cpp)
    // (concerning when a new variable is created vs. using this)
    if (right->StrictScalar(s)) {

      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      }
      return this;
    } else {
      SizeT rEl = right->N_Elements();
      if (nEl < rEl) {
        DComplexDbl s;
        if (StrictScalar(s)) {
          DComplexDblGDL* res = new DComplexDblGDL(right->Dim(),
            BaseGDL::NOZERO);

          if ((GDL_NTHREADS=parallelize( rEl))==1) {
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          }
          return res;
        }

        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return this;
      } else {
        DComplexDblGDL* res = new DComplexDblGDL(right->Dim(),
          BaseGDL::NOZERO);

        if ((GDL_NTHREADS=parallelize( rEl))==1) {
          for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return res;
      }
    }
  }
  if (r->Type() == GDL_LONG) {
    Data_<SpDLong>* right = static_cast<Data_<SpDLong>*> (r);

    assert(right->N_Elements() > 0);

    DLong s;

    // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
    // (concerning when a new variable is created vs. using this)
    if (right->StrictScalar(s)) {

      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
      }
      return this;
    } else {
      SizeT rEl = right->N_Elements();
      if (nEl < rEl) {
        DComplexDbl s;
        if (StrictScalar(s)) {
          DComplexDblGDL* res = new DComplexDblGDL(right->Dim(),
            BaseGDL::NOZERO);

          if ((GDL_NTHREADS=parallelize( rEl))==1) {
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow(s, (*right)[ i]);
          }
          return res;
        }

        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return this;
      } else {
        DComplexDblGDL* res = new DComplexDblGDL(right->Dim(),
          BaseGDL::NOZERO);

        if ((GDL_NTHREADS=parallelize( rEl))==1) {
          for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return res;
      }
    }
  }

  Data_* right = static_cast<Data_*> (r);

  Ty s = (*right)[0];
  if (nEl == 1) {
	(*this)[ 0] = pow((*this)[ 0],s);
	return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow((*this)[ i], s);
  }
  return this;
}

// double complex inverse power of value: left=right ^ left

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowInvS(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*this)[ 0] = pow(s, (*this)[ 0]);
	return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow(s, (*this)[ i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*this)[ i] = pow(s, (*this)[ i]);
  }
  return this;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::PowS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDString>* Data_<SpDString>::PowInvS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowInvS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::PowS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::PowInvS(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return this;
}


#include "instantiate_templates.hpp"
