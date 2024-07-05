/***************************************************************************
                          basic_op_new.cpp  -  GDL operators
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

#include "datatypes.hpp"

// needed with gcc-3.3.2
#include <cassert>

#include "gdl_util.hpp" //for gdl::powI
#include "gdlfpexceptions.hpp"
#include "libdivide.h" // for fast divison by integer constant
using namespace std;
static const std::complex<float> complex_float_nan (sqrt(-1),sqrt(-1));
static const std::complex<double> complex_double_nan (sqrt(-1),sqrt(-1));
// binary operators

// in basic_op.cpp:
// 1. operators that always return a new result (basic_op.cpp)
// 2. operators which operate on 'this' (basic_op.cpp)

// here:
// 3. same operators as under 2. that always return a new result

// AndOp &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// Ands right and itself into a new DataT_
// right must always have more or same number of elements
// for integers

template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  assert(right->N_Elements());

  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = (*this)[0] & (*right)[0];
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] & (*right)[i]; // & Ty(1);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] & (*right)[i]; // & Ty(1);
  }
  return res;
}
// different for floats

template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpInvNew(BaseGDL* right) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return AndOpNew(right);
}
// for floats

template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  // ULong rEl=right->N_Elements();
  ULong nEl = N_Elements();
  Data_* res = NewResult();

  // assert( rEl);
  assert(nEl);
  if (nEl == 1) {
    if ((*right)[0] == zero) (*res)[0] = zero;
    else (*res)[0] = (*this)[0];
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*right)[i] == zero) (*res)[i] = zero;
      else (*res)[i] = (*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*right)[i] == zero) (*res)[i] = zero;
      else (*res)[i] = (*this)[i];
  }
  return res;
}

//b=(a and a)
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  assert(right->N_Elements());

  Data_* res = NewResult();
  if (nEl == 1) {
    if ((*this)[0] != zero) (*res)[0] = (*right)[0];
    else (*res)[0] = zero;
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*res)[i] = (*right)[i];
      else (*res)[i] = zero;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*res)[i] = (*right)[i];
      else (*res)[i] = zero;
  }
  return res;
}
// for doubles

template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  assert(right->N_Elements());

  Data_* res = NewResult();
  if (nEl == 1) {
    if ((*right)[0] == zero) (*res)[0] = zero;
    else (*res)[0] = (*this)[0];
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*right)[i] == zero) (*res)[i] = zero;
      else (*res)[i] = (*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*right)[i] == zero) (*res)[i] = zero;
      else (*res)[i] = (*this)[i];
  }
  return res;
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  assert(right->N_Elements());

  Data_* res = NewResult();
  if (nEl == 1) {
    if ((*this)[0] != zero) (*res)[0] = (*right)[0];
    else (*res)[0] = zero;
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*res)[i] = (*right)[i];
      else (*res)[i] = zero;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*res)[i] = (*right)[i];
      else (*res)[i] = zero;
  }
  return res;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::AndOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::AndOpNew(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_AndOpNewCplx.incpp"
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOpNew(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_AndOpNewCplx.incpp"
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::AndOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::AndOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

// scalar versions

template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);

  Ty s = (*right)[0];

  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = (*this)[0] & s;
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] & s;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) shared(s)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] & s;
  }
  return res;
}
// different for floats

template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpInvSNew(BaseGDL* right) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return AndOpSNew(right);
}
// for floats

template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);
  if ((*right)[0] == zero) {
    return New(this->dim, BaseGDL::ZERO);
  }
  return this->Dup();
}

//b=(a and 1)
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  if (s == zero) {
    return New(this->dim, BaseGDL::ZERO);
  } else {
    Data_* res = NewResult();
    if (nEl == 1) {
      if ((*this)[0] != zero) (*res)[0] = s;
      else (*res)[0] = zero;
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*res)[i] = s;
        else (*res)[i] = zero;
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*res)[i] = s;
        else (*res)[i] = zero;
    }
    return res;
  }
}
// for doubles

template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);
  if ((*right)[0] == zero) {
    return New(this->dim, BaseGDL::ZERO);
  }
  return this->Dup();
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  if (s == zero) {
    return New(this->dim, BaseGDL::ZERO);
  } else {
    Data_* res = NewResult();
    if (nEl == 1) {
      if ((*this)[0] != zero) (*res)[0] = s;
      else (*res)[0] = zero;
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*res)[i] = s;
        else (*res)[i] = zero;
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*res)[i] = s;
        else (*res)[i] = zero;
    }
    return res;
  }
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::AndOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::AndOpSNew(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);
  if ((*right)[0] == zero) {
    return New(this->dim, BaseGDL::ZERO);
  }
  return this->Dup();
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOpSNew(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);
  if ((*right)[0] == zero) {
    return New(this->dim, BaseGDL::ZERO);
  }
  return this->Dup();
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::AndOpInvSNew(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_AndOpInvSNewCplx.incpp"
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOpInvSNew(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_AndOpInvSNewCplx.incpp"
}

// template<>
// Data_<SpDString>* Data_<SpDString>::AndOpInvSNew( BaseGDL* r)
// { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
//  throw GDLException("Cannot apply operation to datatype STRING.",true,false);
//  return res;
// }

template<>
Data_<SpDPtr>* Data_<SpDPtr>::AndOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::AndOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}



// OrOp ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// Ors right to itself returns new result
// right must always have more or same number of elements
// for integers

template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  // ULong rEl=right->N_Elements();
  ULong nEl = N_Elements();
  Data_* res = NewResult();
  // assert( rEl);
  assert(nEl);
  //if( !rEl || !nEl) throw GDLException("Variable is undefined.");
  if (nEl == 1) {
    (*res)[0] = (*this)[0] | (*right)[0]; // | Ty(1);
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] | (*right)[i]; // | Ty(1);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] | (*right)[i]; // | Ty(1);
  }
  //C delete right;
  return res;
}
// different for floats

template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpInvNew(BaseGDL* right) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return OrOpNew(right);
}
// for floats

template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpNew.incpp"
}

// b=(a or a)
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpInvNew.incpp"
}
// for doubles

template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpNew.incpp"
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpInvNew.incpp"
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::OrOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::OrOpNew(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpNewCplx.incpp"
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOpNew(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpNewCplx.incpp"
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::OrOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::OrOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}


template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  Data_* res = NewResult();
  assert(nEl);
  Ty s = (*right)[0];
  // right->Scalar(s);
  //s &= Ty(1);
  //  dd |= s;
  if (nEl == 1) {
	(*res)[0] = (*this)[0] | s;
	return res;
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] | s;
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] | s;
  } //C delete right;
  return res;
}

template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInvSNew(BaseGDL* r) {  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
#include "snippets/basic_op_OrOpInvSNewCplx.incpp"
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInvSNew(BaseGDL* r) {  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
#include "snippets/basic_op_OrOpInvSNewCplx.incpp"
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::OrOpInvSNew(BaseGDL* r) {  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
#include "snippets/basic_op_OrOpInvSNewCplx.incpp"
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOpInvSNew(BaseGDL* r) {  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
#include "snippets/basic_op_OrOpInvSNewCplx.incpp"
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::OrOpInvSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::OrOpInvSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::OrOpInvSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}
// for floats
//template<>
//Data_<SpDFloat>* Data_<SpDFloat>::OrOpInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
//  Data_* right = static_cast<Data_*> (r);
//
//  ULong nEl = N_Elements();
//  Data_* res = NewResult();
//  assert(nEl);
//  Ty s = (*right)[0];
//  if (s != zero) {
//    for (SizeT i = 0; i < nEl; ++i)
//      (*res)[i] = s;
//    return res;
//  } else {
//    if (nEl == 1) {
//      if ((*this)[0] != zero) (*res)[0] = s;
//      else (*res)[0] = zero;
//      return res;
//    }
//    if ((GDL_NTHREADS=parallelize( nEl))==1) {
//      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*res)[i] = s;
//        else (*res)[i] = zero;
//    } else {
//      TRACEOMP(__FILE__, __LINE__)
//#pragma omp parallel for num_threads(GDL_NTHREADS)
//        for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] != zero) (*res)[i] = s;
//        else (*res)[i] = zero;
//    }
//    return res;
//  }
//}
// for doubles
// OrOpS
// for integers

template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpSNew(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  Data_* res = NewResult();
  assert(nEl);
  Ty s = (*right)[0];
  // right->Scalar(s);
  //s &= Ty(1);
  //  dd |= s;
  if (nEl == 1) {
	(*res)[0] = (*this)[0] | s;
	return res;
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] | s;
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] | s;
  } //C delete right;
  return res;
}
// different for floats

template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpSNewCplx.incpp"
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
 #include "snippets/basic_op_OrOpSNewCplx.incpp"
}



template<>
Data_<SpDComplex>* Data_<SpDComplex>::OrOpSNew(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpSNewCplx.incpp"
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOpSNew(BaseGDL* r) {TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
#include "snippets/basic_op_OrOpSNewCplx.incpp"
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::OrOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::OrOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::OrOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}



// XorOp ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Xors right to itself, //C deletes right
// right must always have more or same number of elements
// for integers
// ex: b=(a xor c)
template<class Sp>
Data_<Sp>* Data_<Sp>::XorOpNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);

  if (nEl == 1) {
    Data_* res = NewResult();
    (*res)[0] = (*this)[0] ^ (*right)[0];
    return res;
  }

  Ty s;
  if (right->StrictScalar(s)) {
    if (s == Sp::zero)
      return this->Dup();

    Data_* res = NewResult();
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i];
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i];
    }
    return res;
  } else {
    Data_* res = NewResult();
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] ^ (*right)[i];
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] ^ (*right)[i];
    }
    return res;
  }
}
// invalid types

template<>
Data_<SpDFloat>* Data_<SpDFloat>::XorOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype FLOAT.", true, false);
  return NULL;
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::XorOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype DOUBLE.", true, false);
  return NULL;
}

template<>
Data_<SpDString>* Data_<SpDString>::XorOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::XorOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::XorOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::XorOpNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

template<class Sp>
Data_<Sp>* Data_<Sp>::XorOpSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  if (nEl == 1) {
    Data_* res = NewResult();
    (*res)[0] = (*this)[0] ^ (*right)[0];
    return res;
  }
  Ty s = (*right)[0];
  if (s == this->zero)
    return this->Dup();

  Data_* res = NewResult();
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] ^ (*right)[0];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] ^ (*right)[0];
  }
  return res;
}
// different for floats
// for floats

template<>
Data_<SpDFloat>* Data_<SpDFloat>::XorOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype FLOAT.", true, false);
  return NULL;
}
// for doubles

template<>
Data_<SpDDouble>* Data_<SpDDouble>::XorOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype DOUBLE.", true, false);
  return NULL;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::XorOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::XorOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::XorOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::XorOpSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

// Add ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Adds right to itself returns new result
// right must always have more or same number of elements

template<class Sp>
BaseGDL* Data_<Sp>::AddNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  // ULong rEl=right->N_Elements();
  ULong nEl = N_Elements();
  assert(nEl);
  assert(right->N_Elements());

  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = (*this)[0] + (*right)[0];
    return res;
  }

#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mThis(&(*this)[0], nEl);
  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRight(&(*right)[0], nEl);
  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRes(&(*res)[0], nEl);
  mRes = mThis + mRight;
  return res;
#else
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] + (*right)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] + (*right)[i];
  } //C delete right;
  return res;
#endif
}

template<class Sp>
BaseGDL* Data_<Sp>::AddInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return AddNew(r);
}

template<>
BaseGDL* Data_<SpDString>::AddInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);

  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = (*right)[0] + (*this)[0];
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*right)[i] + (*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*right)[i] + (*this)[i];
  } //C delete right;
  return res;
}

// invalid types

template<>
BaseGDL* Data_<SpDPtr>::AddNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDObj>::AddNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return Add(r);
}

template<>
BaseGDL* Data_<SpDObj>::AddInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return AddInv(r);
}

// scalar versions

template<class Sp>
BaseGDL* Data_<Sp>::AddSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);

  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = (*this)[0] + (*right)[0];
    return res;
  }
  Ty s = (*right)[0];
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mThis(&(*this)[0], nEl);
  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRes(&(*res)[0], nEl);
  mRes = mThis + s;
  return res;
#else
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] + s;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] + s;
  } //C delete right;
  return res;
#endif

}

template<class Sp>
BaseGDL* Data_<Sp>::AddInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return AddSNew(r);
}

template<>
BaseGDL* Data_<SpDString>::AddInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  Data_* res = NewResult();
  assert(nEl);
  if (nEl == 1) {
    (*res)[0] = (*right)[0] + (*this)[0];
    return res;
  }
  Ty s = (*right)[0];
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = s + (*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = s + (*this)[i];
  } //C delete right;
  return res;
}

// invalid types

template<>
BaseGDL* Data_<SpDPtr>::AddSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDObj>::AddSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return Add(r);
}

template<>
BaseGDL* Data_<SpDObj>::AddInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return AddInv(r);
}



// Sub ----------------------------------------------------------------------
// substraction: res=left-right

template<class Sp>
BaseGDL* Data_<Sp>::SubNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);

  Data_* res = NewResult();

  if (nEl == 1)// && rEl == 1)
  {
    (*res)[0] = (*this)[0] - (*right)[0];
    return res;
  }

  Ty s;
  if (right->StrictScalar(s)) {
#ifdef USE_EIGEN

    Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mThis(&(*this)[0], nEl);
    Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRes(&(*res)[0], nEl);
    mRes = mThis - s;
    return res;
#else
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] - s;
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] - s;
    }
    return res;
#endif

  } else {
#ifdef USE_EIGEN

    Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mThis(&(*this)[0], nEl);
    Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRight(&(*right)[0], nEl);
    Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRes(&(*res)[0], nEl);
    mRes = mThis - mRight;
    return res;
#else
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] - (*right)[i];
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] - (*right)[i];
    }
    return res;
#endif
  }
}
// inverse substraction: left=right-left

template<class Sp>
BaseGDL* Data_<Sp>::SubInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);
  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = (*right)[0] - (*this)[0];
    return res;
  }
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mThis(&(*this)[0], nEl);
  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRight(&(*right)[0], nEl);
  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRes(&(*res)[0], nEl);
  mRes = mRight - mThis;
  return res;
#else
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*right)[i] - (*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*right)[i] - (*this)[i];
  }
  return res;
#endif
}
// invalid types

template<>
BaseGDL* Data_<SpDString>::SubNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDString>::SubInvNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDPtr>::SubNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDPtr>::SubInvNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDObj>::SubNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return Sub(r);
}

template<>
BaseGDL* Data_<SpDObj>::SubInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return SubInv(r);
}

// scalar versions

template<class Sp>
BaseGDL* Data_<Sp>::SubSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);

  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = (*this)[0] - (*right)[0];
    return res;
  }

  Ty s = (*right)[0];
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mThis(&(*this)[0], nEl);
  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRes(&(*res)[0], nEl);
  mRes = mThis - s;
  return res;
#else
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] - s;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] - s;
  }
  return res;
#endif

}
// inverse substraction: left=right-left

template<class Sp>
BaseGDL* Data_<Sp>::SubInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);

  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = (*right)[0] - (*this)[0];
    return res;
  }

  Ty s = (*right)[0];
  // right->Scalar(s);
  //  dd = s - dd;
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mThis(&(*this)[0], nEl);
  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRes(&(*res)[0], nEl);
  mRes = s - mThis;
  return res;
#else
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = s - (*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = s - (*this)[i];
  }
  return res;
#endif

}
// invalid types

template<>
BaseGDL* Data_<SpDString>::SubSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDString>::SubInvSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDPtr>::SubSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDPtr>::SubInvSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
BaseGDL* Data_<SpDObj>::SubSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return Sub(r);
}

template<>
BaseGDL* Data_<SpDObj>::SubInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return SubInv(r);
}

// LtMark <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// LtMarks right to itself, //C deletes right
// right must always have more or same number of elements

template<class Sp>
Data_<Sp>* Data_<Sp>::LtMarkNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  Data_* res = NewResult();
  assert(nEl);
  if (nEl == 1) {
    if ((*this)[0] > (*right)[0]) (*res)[0] = (*right)[0];
    else (*res)[0] = (*this)[0];
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] > (*right)[i]) (*res)[i] = (*right)[i];
      else (*res)[i] = (*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] > (*right)[i]) (*res)[i] = (*right)[i];
      else (*res)[i] = (*this)[i];
  } //C delete right;
  return res;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::LtMarkNew(BaseGDL* r) {
#include "snippets/basic_op_LtMarkNewCplx.incpp"
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::LtMarkNew(BaseGDL* r) {
#include "snippets/basic_op_LtMarkNewCplx.incpp"
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::LtMarkNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::LtMarkNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::LtMarkNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

// scalar versions

template<class Sp>
Data_<Sp>* Data_<Sp>::LtMarkSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  if (nEl == 1) {
    if ((*this)[0] > (*right)[0]) (*res)[0] = (*right)[0];
    else (*res)[0] = (*this)[0];
    return res;
  }
  Ty s = (*right)[0];
  // right->Scalar(s);
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] > s) (*res)[i] = s;
      else (*res)[i] = (*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] > s) (*res)[i] = s;
      else (*res)[i] = (*this)[i];
  } //C delete right;
  return res;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::LtMarkSNew(BaseGDL* r) {
#include "snippets/basic_op_LtMarkSNewCplx.incpp"
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::LtMarkSNew(BaseGDL* r) {
#include "snippets/basic_op_LtMarkSNewCplx.incpp"
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::LtMarkSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}



template<>
Data_<SpDPtr>* Data_<SpDPtr>::LtMarkSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::LtMarkSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

// GtMark >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// GtMarks right to itself returns new result
// right must always have more or same number of elements

template<class Sp>
Data_<Sp>* Data_<Sp>::GtMarkNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  //  ULong rEl=right->N_Elements();
  ULong nEl = N_Elements();
  Data_* res = NewResult();
  // assert( rEl);
  assert(nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");
  if (nEl == 1) {
    if ((*this)[0] < (*right)[0]) (*res)[0] = (*right)[0];
    else (*res)[0] = (*this)[0];
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] < (*right)[i]) (*res)[i] = (*right)[i];
      else (*res)[i] = (*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] < (*right)[i]) (*res)[i] = (*right)[i];
      else (*res)[i] = (*this)[i];
  } //C delete right;
  return res;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::GtMarkNew(BaseGDL* r) {
#include "snippets/basic_op_GtMarkNewCplx.incpp"
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::GtMarkNew(BaseGDL* r) {
#include "snippets/basic_op_GtMarkNewCplx.incpp"
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::GtMarkNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::GtMarkNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::GtMarkNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

// scalar versions

template<class Sp>
Data_<Sp>* Data_<Sp>::GtMarkSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  Data_* res = NewResult();
  assert(nEl);
  if (nEl == 1) {
    if ((*this)[0] < (*right)[0]) (*res)[0] = (*right)[0];
    else (*res)[0] = (*this)[0];
    return res;
  }

  Ty s = (*right)[0];
  // right->Scalar(s);
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] < s) (*res)[i] = s;
      else (*res)[i] = (*this)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) if ((*this)[i] < s) (*res)[i] = s;
      else (*res)[i] = (*this)[i];
  };
  return res;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::GtMarkSNew(BaseGDL* r) {

#include "snippets/basic_op_GtMarkSNewCplx.incpp"
  return NULL;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::GtMarkSNew(BaseGDL* r) {
#include "snippets/basic_op_GtMarkSNewCplx.incpp"
  return NULL;
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::GtMarkSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::GtMarkSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::GtMarkSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

// Mult *********************************************************************
// Mults right to itself, //C deletes right
// right must always have more or same number of elements

template<class Sp>
Data_<Sp>* Data_<Sp>::MultNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  Data_* res = NewResult();

  //  ULong rEl=right->N_Elements();
  ULong nEl = N_Elements();
  // assert( rEl);
  assert(nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");
  if (nEl == 1) {
    (*res)[0] = (*this)[0] * (*right)[0];
    return res;
  }
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mThis(&(*this)[0], nEl);
  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRight(&(*right)[0], nEl);
  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRes(&(*res)[0], nEl);
  mRes = mThis * mRight;
  return res;
#else
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] * (*right)[i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = (*this)[i] * (*right)[i];
  } //C delete right;
  return res;
#endif

}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::MultNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::MultNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::MultNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

// scalar versions

template<class Sp>
Data_<Sp>* Data_<Sp>::MultSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);

  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = (*this)[0] * (*right)[0];
    return res;
  }
  Ty s = (*right) [0];
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mThis(&(*this)[0], nEl);
  Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, 1>, Eigen::Aligned> mRes(&(*res)[0], nEl);
  mRes = mThis * s;
  return res;
#else
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res) [i] = (*this)[i] * s;
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res) [i] = (*this)[i] * s;
  }
  return res;
#endif

}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::MultSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::MultSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::MultSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

template<class Sp> // Integer type, protect against intger division by zero. FP case explicitely served below
Data_<Sp>* Data_<Sp>::DivNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);
  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  SizeT i = 0;

  if (nEl == 1) {
	if ((*right)[0] != this->zero) (*res)[0] = (*this)[0] / (*right)[0]; else 	{ (*res)[0] = (*this)[0]; GDLRegisterADivByZeroException();}
	return res;
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
      for (OMPInt ix = i; ix < nEl; ++ix) if ((*right)[ix] != this->zero) (*res)[ix] = (*this)[ix] / (*right)[ix]; else { (*res)[ix] = (*this)[ix]; GDLRegisterADivByZeroException();}
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt ix = i; ix < nEl; ++ix) if ((*right)[ix] != this->zero) (*res)[ix] = (*this)[ix] / (*right)[ix]; else { (*res)[ix] = (*this)[ix]; GDLRegisterADivByZeroException();}
  }
  return res;
}

template<> // Float
Data_<SpDFloat>* Data_<SpDFloat>::DivNew(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);
  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  SizeT i = 0;
  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
    (*res)[0] = (*this)[0] / (*right)[0];
	GDLStopRegisteringFPExceptions();
	return res;
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt ix = i; ix < nEl; ++ix) (*res)[ix] = (*this)[ix] / (*right)[ix];
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	for (OMPInt ix = i; ix < nEl; ++ix) (*res)[ix] = (*this)[ix] / (*right)[ix];
  }

  GDLStopRegisteringFPExceptions();
  
  return res;
}

template<> // Double
Data_<SpDDouble>* Data_<SpDDouble>::DivNew(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);
  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  SizeT i = 0;
  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
    (*this)[0] /= (*right)[0];
	GDLStopRegisteringFPExceptions();
	return res;
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt ix = i; ix < nEl; ++ix) (*res)[ix] = (*this)[ix] / (*right)[ix];

  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	for (OMPInt ix = i; ix < nEl; ++ix) (*res)[ix] = (*this)[ix] / (*right)[ix];
  }

  GDLStopRegisteringFPExceptions();
  
  return res;
}


// inverse division: left=right/left

template<class Sp> // Integer type, protect against intger division by zero. FP case explicitely served below
Data_<Sp>* Data_<Sp>::DivInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);
  ULong nEl = N_Elements();
  assert(nEl); 
  Data_* res = NewResult();

  if (nEl == 1) {
	if ((*this)[0] != this->zero) (*res)[0] = (*right)[0] / (*this)[0]; else {
	  (*res)[0] = (*right)[0];
	  GDLRegisterADivByZeroException();
	}
	return res;
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
      for (OMPInt ix = 0; ix < nEl; ++ix)
        if ((*this)[ix] != this->zero)
          (*res)[ix] = (*right)[ix] / (*this)[ix];
        else
		{(*res)[ix] = (*right)[ix]; GDLRegisterADivByZeroException();}
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt ix = 0; ix < nEl; ++ix)
        if ((*this)[ix] != this->zero)
          (*res)[ix] = (*right)[ix] / (*this)[ix];
        else
 		{(*res)[ix] = (*right)[ix]; GDLRegisterADivByZeroException();}
  }
  return res;
}

template<> 
Data_<SpDFloat>* Data_<SpDFloat>::DivInvNew(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);
  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();

  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
	(*res)[0] = (*right)[0] / (*this)[0];
	GDLStopRegisteringFPExceptions();
	return res;
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt ix = 0; ix < nEl; ++ix) (*res)[ix] = (*right)[ix] / (*this)[ix];
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt ix = 0; ix < nEl; ++ix) (*res)[ix] = (*right)[ix] / (*this)[ix];
  }

  GDLStopRegisteringFPExceptions();
  
  return res;
}

template<> 
Data_<SpDDouble>* Data_<SpDDouble>::DivInvNew(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);
  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();

  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
	(*res)[0] = (*right)[0] / (*this)[0];
	GDLStopRegisteringFPExceptions();
	return res;
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt ix = 0; ix < nEl; ++ix) (*res)[ix] = (*right)[ix] / (*this)[ix];
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt ix = 0; ix < nEl; ++ix) (*res)[ix] = (*right)[ix] / (*this)[ix];
  }

  GDLStopRegisteringFPExceptions();
  
  return res;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::DivNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDString>* Data_<SpDString>::DivInvNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivInvNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::DivNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::DivInvNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

// scalar versions
template<class Sp>  //no need to differentiate Sp Types, as the FP exception is produced only by s
Data_<Sp>* Data_<Sp>::DivSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  Ty s = (*right)[0];
  Data_* res; 
  if (s == this->zero) {
	res = this->Dup(); //faster 
	GDLRegisterADivByZeroException();
	return res; 
  }
  res=NewResult(); 
  //s is not zero
  if (nEl == 1) {
	(*res)[0]=(*this)[0] / s;
	return res; //wrong copypaste was cause of #1777
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (SizeT ix = 0; ix < nEl; ++ix) (*res)[ix]=(*this)[ix] / s;
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt ix = 0; ix < nEl; ++ix) (*res)[ix]=(*this)[ix] / s;
  }
  
  return res;
}
// float version: gives Inf for division by zero
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::DivSNew(BaseGDL* r) {
#include "snippets/basic_op_DivSNew.incpp"
}
// double version: gives Inf for division by zero
template<>  //no need to differentiate Sp Types, as the FP exception is produced only by s
Data_<SpDDouble>* Data_<SpDDouble>::DivSNew(BaseGDL* r) {
#include "snippets/basic_op_DivSNew.incpp"
}
// double version: gives -NaN for division by zero to copy IDL.
template<>  //no need to differentiate Sp Types, as the FP exception is produced only by s
Data_<SpDComplex>* Data_<SpDComplex>::DivSNew(BaseGDL* r) {
#include "snippets/basic_op_DivSNewCplx.incpp"
}
// double version: gives -NaN for division by zero to copy IDL.
template<>  //no need to differentiate Sp Types, as the FP exception is produced only by s
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::DivSNew(BaseGDL* r) {
#include "snippets/basic_op_DivSNewCplxDbl.incpp"
}

//int32_t, uint32_t, int64_t, and uint64_t integer versions use libdivide in some cases.
template<>
Data_<SpDLong>* Data_<SpDLong>::DivSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  DLong s = (*right)[0];
  Data_* res; 
  if (s == 0) {
	res = this->Dup(); //faster 
	GDLRegisterADivByZeroException();
	return res; 
  }
  res=NewResult();
  //s is not zero
  if (nEl == 1) {
	(*res)[0] = (*this)[0] / s;
	return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
	for (OMPInt ix = 0; ix < nEl; ++ix)  (*res)[ix] = (*this)[ix] / s;
  } else {
	TRACEOMP(__FILE__, __LINE__)
	struct libdivide::libdivide_s32_t fast_d = libdivide::libdivide_s32_gen(s); //only when many values (>100000) is libdivide useful.
//#pragma omp parallel for num_threads(GDL_NTHREADS)
	for (OMPInt ix = 0; ix < nEl; ++ix)  (*res)[ix] = libdivide::libdivide_s32_do((*this)[ix], &fast_d );
	//	for (OMPInt ix = 0; ix < nEl; ++ix) (*res)[ix] = (*this)[ix] / s;
 }
  
 return res;
}

template<>
Data_<SpDULong>* Data_<SpDULong>::DivSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  DULong s = (*right)[0];
  Data_* res; 
  if (s == 0) {
	res = this->Dup(); //faster 
	GDLRegisterADivByZeroException();
	return res; 
  }
  res=NewResult();
  //s is not zero
  if (nEl == 1) {
	(*res)[0] = (*this)[0] / s;
	return res;
  }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
	for (OMPInt ix = 0; ix < nEl; ++ix)  (*res)[ix] = (*this)[ix] / s;
  } else {
	TRACEOMP(__FILE__, __LINE__)
	struct libdivide::libdivide_u32_t fast_d = libdivide::libdivide_u32_gen(s); //only when many values (>100000) is libdivide useful.
#pragma omp parallel for num_threads(GDL_NTHREADS)
	for (OMPInt ix = 0; ix < nEl; ++ix)  (*res)[ix] = libdivide::libdivide_u32_do((*this)[ix], &fast_d );
	//	for (OMPInt ix = 0; ix < nEl; ++ix) (*res)[ix] = (*this)[ix] / s;
 }
  
 return res;
}
template<>
Data_<SpDLong64>* Data_<SpDLong64>::DivSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  DLong64 s = (*right)[0];
  Data_* res; 
  if (s == 0) {
	res = this->Dup(); //faster 
	GDLRegisterADivByZeroException();
	return res; 
  }
  res=NewResult();
  //s is not zero
  if (nEl == 1) {
	(*res)[0] = (*this)[0] / s;
	return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
	for (OMPInt ix = 0; ix < nEl; ++ix)  (*res)[ix] = (*this)[ix] / s;
  } else {
	TRACEOMP(__FILE__, __LINE__)
	struct libdivide::libdivide_s64_t fast_d = libdivide::libdivide_s64_gen(s); //only when many values (>100000) is libdivide useful.
#pragma omp parallel for num_threads(GDL_NTHREADS)
	for (OMPInt ix = 0; ix < nEl; ++ix)  (*res)[ix] = libdivide::libdivide_s64_do((*this)[ix], &fast_d );
	//	for (OMPInt ix = 0; ix < nEl; ++ix) (*res)[ix] = (*this)[ix] / s;
 }
  
 return res;
}
template<>
Data_<SpDULong64>* Data_<SpDULong64>::DivSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  DULong64 s = (*right)[0];
  Data_* res; 
  if (s == 0) {
	res = this->Dup(); //faster 
	GDLRegisterADivByZeroException();
	return res; 
  }
  res=NewResult();
  //s is not zero
  if (nEl == 1) {
	(*res)[0] = (*this)[0] / s;
	return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
	for (OMPInt ix = 0; ix < nEl; ++ix)  (*res)[ix] = (*this)[ix] / s;
  } else {
	TRACEOMP(__FILE__, __LINE__)
	struct libdivide::libdivide_u64_t fast_d = libdivide::libdivide_u64_gen(s); //only when many values (>100000) is libdivide useful.
#pragma omp parallel for num_threads(GDL_NTHREADS)
	for (OMPInt ix = 0; ix < nEl; ++ix)  (*res)[ix] = libdivide::libdivide_u64_do((*this)[ix], &fast_d );
	//	for (OMPInt ix = 0; ix < nEl; ++ix) (*res)[ix] = (*this)[ix] / s;
 }
  
  return res;
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::DivSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::DivSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

// inverse division: left=right/left
template<class Sp>
Data_<Sp>* Data_<Sp>::DivInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  Data_* res = NewResult();
  Ty s = (*right)[0];
  SizeT i = 0;
  if (nEl == 1) {
	if ((*this)[0] != this->zero) (*res)[0] = s / (*this)[0]; else { (*res)[0] = s;	GDLRegisterADivByZeroException();}
	return res;
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
    for (SizeT ix = 0; ix < nEl; ++ix) if ((*this)[ix] != this->zero) (*res)[ix] = s / (*this)[ix]; else { (*res)[ix] = s; GDLRegisterADivByZeroException();}
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (OMPInt ix = 0; ix < nEl; ++ix) if ((*this)[ix] != this->zero) (*res)[ix] = s / (*this)[ix]; else { (*res)[ix] = s; GDLRegisterADivByZeroException();}
  }
  
  return res;
}

template<>
Data_<SpDFloat>* Data_<SpDFloat>::DivInvSNew(BaseGDL* r) {
#include "snippets/basic_op_DivInvSNew.incpp"
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::DivInvSNew(BaseGDL* r) {
#include "snippets/basic_op_DivInvSNew.incpp"
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::DivInvSNew(BaseGDL* r) {
#include "snippets/basic_op_DivInvSNew.incpp"
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::DivInvSNew(BaseGDL* r) {
#include "snippets/basic_op_DivInvSNew.incpp"
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::DivInvSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivInvSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}


template<>
Data_<SpDObj>* Data_<SpDObj>::DivInvSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}



// Mod %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// modulo division: left=left % right

// float modulo division: left=left % right
//avoid multiple definition when this file is coalesced within datatypes.cpp:
#ifndef ONE_DATAFILE
inline DFloat Modulo(const DFloat& l, const DFloat& r) {
  return fmodf(l, r); //fmodf returns a float
}
// in basic_op.cpp
// double modulo division: left=left % right

inline DDouble DModulo(const DDouble& l, const DDouble& r) {
  return fmod(l, r); //fmod returns a double
}
#endif
// modulo division: left=left % right
// ex: b=(a mod c)
// right is of size > 1 (otherwise ModS would have been called)
// catching FPE exceptions for FP, avoiding segfault on integer zero divide for integers

template<class Sp> //avoiding segfault on integer zero divide for integers, FP treatment specialized below
Data_<Sp>* Data_<Sp>::ModNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  Data_* res = NewResult();
  if (nEl == 1) { //accelerator for single value on LEFT 
    if ((*right)[0] != this->zero) (*res)[0] = (*this)[0] % (*right)[0]; else (*res)[0] = this->zero;
	return res;
  }
  // Multiple LEFT values, parallel section slowed by inner 'if' clause.
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
      for (OMPInt ix = 0; ix < nEl; ++ix) if ((*right)[ix] != this->zero) (*res)[ix] = (*this)[ix] % (*right)[ix]; else (*res)[ix] = this->zero;
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt ix = 0; ix < nEl; ++ix) if ((*right)[ix] != this->zero) (*res)[ix] = (*this)[ix] % (*right)[ix]; else (*res)[ix] = this->zero;
  }

  return res;
}


template<> // catching FPE exceptions for FP
Data_<SpDFloat>* Data_<SpDFloat>::ModNew(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
	(*res)[0] = Modulo((*this)[0], (*right)[0]);
	GDLStopRegisteringFPExceptions();
	return res;
  }

  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = Modulo((*this)[i], (*right)[i]);
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = Modulo((*this)[i], (*right)[i]);
  }

  GDLStopRegisteringFPExceptions();

  return res;
}



template<> // catching FPE exceptions for FP
Data_<SpDDouble>* Data_<SpDDouble>::ModNew(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
	(*res)[0] = DModulo((*this)[0], (*right)[0]);
	GDLStopRegisteringFPExceptions();
	return res;
  }

  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = DModulo((*this)[i], (*right)[i]);
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = DModulo((*this)[i], (*right)[i]);
  }

  GDLStopRegisteringFPExceptions();

  return res;
}
  // -------------------------------------------------------------------------------------------------------
  // ModInv
  // inverse modulo division: left=right % left
  // right is of size > 1 (otherwise ModInvS would have been called)
  // catching FPE exceptions for FP, avoiding segfault on integer zero divide for integers

  template<class Sp > //avoiding segfault on integer zero divide for integers, FP treatment specialzed below
  Data_<Sp>* Data_<Sp>::ModInvNew(BaseGDL * r) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	Data_* right = static_cast<Data_*> (r);

	ULong nEl = N_Elements();
	assert(nEl);
    Data_* res = NewResult();
	if (nEl == 1) { //accelerator for LEFT singleton
	  if ((*this)[0] != this->zero) (*res)[0] = (*right)[0] % (*this)[0];  else (*res)[0] = this->zero;
	  return res;
	}
	// Multiple LEFT values, parallel section slowed by inner 'if' clause.
	if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	  for (OMPInt ix = 0; ix < nEl; ++ix) if ((*this)[ix] != this->zero) (*res)[ix] = (*right)[ix] % (*this)[ix]; else (*res)[ix] = this->zero;
	} else {
	  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
		for (OMPInt ix = 0; ix < nEl; ++ix) if ((*this)[ix] != this->zero) (*res)[ix] = (*right)[ix] % (*this)[ix]; else (*res)[ix] = this->zero;
	}

	return res;
  }
  
  // float  inverse modulo division: left=right % left

template<> // catching FPE exceptions
Data_<SpDFloat>* Data_<SpDFloat>::ModInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  Data_* res = NewResult();
  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
	(*res)[0] = Modulo((*right)[0], (*this)[0]);
    GDLStopRegisteringFPExceptions();
	return res;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = Modulo((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = Modulo((*right)[i], (*this)[i]);
  }

  GDLStopRegisteringFPExceptions();
  
  return res;
}

// double inverse modulo division: left=right % left

template<>// catching FPE exceptions
Data_<SpDDouble>* Data_<SpDDouble>::ModInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  Data_* res = NewResult();
  GDLStartRegisteringFPExceptions();
  if (nEl == 1) {
	(*res)[0] = DModulo((*right)[0], (*this)[0]);
    GDLStopRegisteringFPExceptions();
	return res;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = DModulo((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = DModulo((*right)[i], (*this)[i]);
  }

  GDLStopRegisteringFPExceptions();
  
  return res;
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::ModNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDString>* Data_<SpDString>::ModInvNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModInvNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModInvNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModInvNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::ModNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::ModInvNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

//----------------------------------------------------------------------------------------------------
// ModS
// modulo division: left=left % right
// right is of size 1 (otherwise Mod would have been called)
// catching FPE exceptions for FP, avoiding segfault on integer zero divide for integers
template<class Sp> //avoiding segfault on integer zero divide for integers, FP treatment specialized below
Data_<Sp>* Data_<Sp>::ModSNew(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  
  Ty s = (*right)[0];

  if (s == this->zero) {
	for (SizeT i = 0; i < nEl; ++i) (*res)[i] = 0;
	//add an FP div by zero to counters
	GDLRegisterADivByZeroException();
	return res;
  }
  //s is not 0 here 
  if (nEl == 1) {
	(*res)[0] = (*this)[0] % s;
	return res;
  }
  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt ix = 0; ix < nEl; ++ix) (*res)[ix] = (*this)[ix] % s;
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt ix = 0; ix < nEl; ++ix) (*res)[ix] = (*this)[ix] % s;
  }
  return res;
}

template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModSNew(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  GDLStartRegisteringFPExceptions();
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*res)[0] = Modulo((*this)[0], s);
	GDLStopRegisteringFPExceptions();
	return res;
  }

  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = Modulo((*this)[i], s);
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = Modulo((*this)[i], s);
  }

  GDLStopRegisteringFPExceptions();

  return res;
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModSNew(BaseGDL* r) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  GDLStartRegisteringFPExceptions();
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*res)[0] = DModulo((*this)[0], s);
	GDLStopRegisteringFPExceptions();
	return res;
  }

  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = DModulo((*this)[i], s);
  } else {
	TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
	  for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = DModulo((*this)[i], s);
  }

  GDLStopRegisteringFPExceptions();

  return res;
}
// ModInvS
// inverse modulo division: left=right % left
// right is of size == 1 (otherwise ModInv would have been called)
// catching FPE exceptions for FP, avoiding segfault on integer zero divide for integers; but report an FP exception to the GDL exception reporting system

template<class Sp >//avoiding segfault on integer zero divide for integers, FP treatment specialized below
  Data_<Sp>* Data_<Sp>::ModInvSNew(BaseGDL* r) {
	TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
	Data_* right = static_cast<Data_*> (r);

	ULong nEl = N_Elements();
	assert(nEl); 
  Data_* res = NewResult();

	Ty s = (*right)[0];
  if (nEl == 1) {
	if ((*this)[0] != this->zero) (*res)[0] = s % (*this)[0]; else {
	  (*res)[0] = this->zero;
	  GDLRegisterADivByZeroException(); //add an FP div by zero to counters
	}
	return res;
  }
	
	if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
      for (OMPInt ix = 0; ix < nEl; ++ix) if ((*this)[ix] != this->zero) (*res)[ix] = s % (*this)[ix]; else {
		(*res)[ix] = this->zero;
		GDLRegisterADivByZeroException(); //add an FP div by zero to counters
	  }
	} else {
	  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt ix = 0; ix < nEl; ++ix) if ((*this)[ix] != this->zero) (*res)[ix] = s % (*this)[ix]; else {
		  (*res)[ix] = this->zero;
		  GDLRegisterADivByZeroException(); //add an FP div by zero to counters
		}
  }
  
	return res;
  }

// float  inverse modulo division: left=right % left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  Data_* res = NewResult();
  GDLStartRegisteringFPExceptions();
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*res)[0] = Modulo(s, (*this)[0]);
    GDLStopRegisteringFPExceptions();
	return res;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = Modulo(s, (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = Modulo(s, (*this)[i]);
  }

  GDLStopRegisteringFPExceptions();
  
  return res;
}

// double inverse modulo division: left=right % left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl); 
  Data_* res = NewResult();
  GDLStartRegisteringFPExceptions();
  Ty s = (*right)[0];
  if (nEl == 1) {
	(*res)[0] = DModulo(s, (*this)[0]);
    GDLStopRegisteringFPExceptions();
	return res;
  }

  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = DModulo(s, (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = DModulo(s, (*this)[i]);
  }

  GDLStopRegisteringFPExceptions();
  
  return res;
}

// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::ModSNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDString>* Data_<SpDString>::ModInvSNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModSNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModSNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModInvSNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModInvSNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype " + str + ".", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModSNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModInvSNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::ModSNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::ModInvSNew(BaseGDL* r) { 
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}


template<class Sp>
Data_<Sp>* Data_<Sp>::PowNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  Data_* res = NewResult();
  assert(nEl);
  if (nEl == 1) {
    (*res)[0] = pow((*this)[0], (*right)[0]);
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], (*right)[i]); // valarray
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], (*right)[i]); // valarray
  } //C delete right;
  return res;
}
// inverse power of value: left=right ^ left
//PowNew: operands A and B are both arrays.  n_el(A) > n_el(B), so this is B and right is A (inverted mode) 
template<class Sp>
Data_<Sp>* Data_<Sp>::PowInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = pow((*right)[0], (*this)[0]);
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*right)[i], (*this)[i]);
  }
  return res;
}

// anygdl (except complex) power to a GDL_LONG value left=left ^ right
template<class Sp>
Data_<Sp>* Data_<Sp>::PowIntNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  DLongGDL* right = static_cast<DLongGDL*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);
  if (r->StrictScalar()) {
    Data_* res = new Data_(this->Dim(), BaseGDL::NOZERO);
    DLong r0 = (*right)[0];
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = gdl::powI((*this)[i], r0);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = gdl::powI((*this)[i], r0);
    }
    return res;
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
    Data_* res = new Data_(this->Dim(), BaseGDL::NOZERO);
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = gdl::powI((*this)[i], (*right)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = gdl::powI((*this)[i], (*right)[i]);
    }
    return res;
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
Data_<SpDString>* Data_<SpDString>::PowIntNew(BaseGDL* r) {
  assert(0);
  throw GDLException("Internal error: Data_::PowIntNew called.", true, false);
  return NULL;
}

template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = pow((*this)[0], (*right)[0]);
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], (*right)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], (*right)[i]);
  }
  return res;
}
// floats inverse power of value: left=right ^ left

template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = pow((*right)[0], (*this)[0]);
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*right)[i], (*this)[i]);
  } //C delete right;
  return res;
}
// doubles power of value: left=left ^ right

template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = pow((*this)[0], (*right)[0]);
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], (*right)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], (*right)[i]);
  }
  return res;
}
// doubles inverse power of value: left=right ^ left

template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  Data_* res = NewResult();
  assert(nEl);
  if (nEl == 1) {
    (*res)[0] = pow((*right)[0], (*this)[0]);
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*right)[i], (*this)[i]);
  } //C delete right;
  return res;
}
// complex power of value: left=left ^ right
// complex is special here

template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
      DComplexGDL* res = new DComplexGDL(this->Dim(),
        BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
      } //C delete right;
      return res;
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
          } //C delete right;
          return res;
        }

        DComplexGDL* res = new DComplexGDL(this->Dim(),
          BaseGDL::NOZERO);
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } //C delete right;
        return res;
      } else {
        DComplexGDL* res = new DComplexGDL(right->Dim(),
          BaseGDL::NOZERO);
        if ((GDL_NTHREADS=parallelize( rEl))==1) {
          for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } //C delete right;
        //C delete this;
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
      DComplexGDL* res = new DComplexGDL(this->Dim(),
        BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
      } //C delete right;
      return res;
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
          } //C delete right;
          return res;
        }

        DComplexGDL* res = new DComplexGDL(this->Dim(),
          BaseGDL::NOZERO);
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } //C delete right;
        return res;
      } else {
        DComplexGDL* res = new DComplexGDL(right->Dim(),
          BaseGDL::NOZERO);
        if ((GDL_NTHREADS=parallelize( rEl))==1) {
          for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } //C delete right;
        //C delete this;
        return res;
      }
    }
  }

  Data_* right = static_cast<Data_*> (r);

  //   ULong rEl=right->N_Elements();
  //   ULong nEl=N_Elements(); Data_* res = NewResult();
  //   if( !rEl || !nEl) throw GDLException("Variable is undefined.");
  Ty s;
  if (right->StrictScalar(s)) {
    DComplexGDL* res = new DComplexGDL(this->Dim(),
      BaseGDL::NOZERO);
    //#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
    }
    return res;
  } else {
    DComplexGDL* res = new DComplexGDL(this->Dim(),
      BaseGDL::NOZERO);
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], (*right)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], (*right)[i]);
    }
    return res;
  }
}
// complex inverse power of value: left=right ^ left

template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);

  Data_* res = NewResult();
  if (nEl == 1) {
	(*res)[0] = pow((*right)[0], (*this)[0]);
	return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*right)[i], (*this)[i]);
  }
  return res;
}
// double complex power of value: left=left ^ right

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();

  assert(nEl > 0);
  assert(r->N_Elements() > 0);

  if (r->Type() == GDL_DOUBLE) {
    Data_<SpDDouble>* right = static_cast<Data_<SpDDouble>*> (r);

    DDouble s;
    // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
    // (concerning when a new variable is created vs. using this)
    // (must also be consistent with ComplexDbl)
    if (right->StrictScalar(s)) {
      DComplexDblGDL* res = new DComplexDblGDL(this->Dim(),
        BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
      }
      return res;
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

        DComplexDblGDL* res = new DComplexDblGDL(this->Dim(),
          BaseGDL::NOZERO);
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        }
        return res;
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

    DLong s;
    // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
    // (concerning when a new variable is created vs. using this)
    // (must also be consistent with ComplexDbl)
    if (right->StrictScalar(s)) {
      DComplexDblGDL* res = new DComplexDblGDL(this->Dim(),
        BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
      }
      return res;
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

        DComplexDblGDL* res = new DComplexDblGDL(this->Dim(),
          BaseGDL::NOZERO);
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } //C delete right;
        return res;
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

  Ty s;
  if (right->StrictScalar(s)) {
    DComplexDblGDL* res = new DComplexDblGDL(this->Dim(),
      BaseGDL::NOZERO);
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = pow((*this)[ i], s);
    }
    return res;
  } else {
    DComplexDblGDL* res = new DComplexDblGDL(this->Dim(),
      BaseGDL::NOZERO);
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], (*right)[i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], (*right)[i]);
    }
    return res;
  }
}
// double complex inverse power of value: left=right ^ left

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowInvNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = N_Elements();
  assert(rEl);
  assert(nEl);
  Data_* res = NewResult();
  if (nEl == 1) {
	(*res)[0] = pow((*right)[0], (*this)[0]);
	return res;
  }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*right)[i], (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*right)[i], (*this)[i]);
  }
  return res;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::PowNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDString>* Data_<SpDString>::PowInvNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowInvNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::PowNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::PowInvNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

// scalar versions
//PowInvSNew: A^B: operand B is singleton and A array 

template<class Sp>
Data_<Sp>* Data_<Sp>::PowSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  Data_* res = NewResult();
  assert(nEl);
  Ty s = (*right)[0];
  if (nEl == 1) {
    (*res)[0] = pow((*this)[0], s);
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], s);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[i], s);
  }
  //C delete right;
  return res;
}
// inverse power of value: left=right ^ left
//PowInvSNew: A^B: operand A is singleton and B array 

template<class Sp>
Data_<Sp>* Data_<Sp>::PowInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Ty s = (*right)[0];
  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = pow(s, (*this)[0]);
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow(s, (*this)[i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow(s, (*this)[i]);
  } //C delete right;
  return res;
}
// complex power of value: left=left ^ right
// complex is special here

template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
      Data_* res = NewResult();
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
      }
      return res;
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

        Data_* res = NewResult();
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], (*right)[ i]);
        } //C delete right;
        return res;
      } else {
        DComplexGDL* res = new DComplexGDL(right->Dim(),
          BaseGDL::NOZERO);
        if ((GDL_NTHREADS=parallelize( rEl))==1) {
          for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < rEl; ++i) (*res)[ i] = pow((*this)[ i], (*right)[ i]);
        } //C delete right;
        //C delete this;
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
      Data_* res = NewResult();
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
      } //C delete right;
      return res;
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
          } //C delete right;
          return res;
        }

        Data_* res = NewResult();
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], (*right)[ i]);
        } //C delete right;
        return res;
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

  // r->Type() == GDL_COMPLEX
  Data_* right = static_cast<Data_*> (r);

  Ty s = (*right)[0];
  Data_* res = NewResult();
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
  }

  return res;
}
// complex inverse power of value: left=right ^ left

template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);
  ULong nEl = N_Elements();
  assert(nEl);
  assert(right->N_Elements());
  Ty s = (*right)[0];
  Data_* res = NewResult();
  if (nEl == 1) {
    (*res)[0] = pow(s, (*this)[0]);
    return res;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow(s, (*this)[ i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow(s, (*this)[ i]);
  }
  return res;
}
// double complex power of value: left=left ^ right

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();

  assert(nEl > 0);

  if (r->Type() == GDL_DOUBLE) {
    Data_<SpDDouble>* right = static_cast<Data_<SpDDouble>*> (r);

    assert(right->N_Elements() > 0);

    DDouble s;

    // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
    // (concerning when a new variable is created vs. using this)
    if (right->StrictScalar(s)) {
      Data_* res = NewResult();
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
      }
      return res;
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

        Data_* res = NewResult();
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], (*right)[ i]);
        } //C delete right;
        return res;
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
      Data_* res = NewResult();
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
      }
      return res;
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

        Data_* res = NewResult();
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], (*right)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], (*right)[ i]);
        } //C delete right;
        return res;
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
  const Ty s = (*right)[0];
  Data_* res = NewResult();
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow((*this)[ i], s);
  }
  return res;
}
// double complex inverse power of value: left=right ^ left

template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowInvSNew(BaseGDL* r) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* right = static_cast<Data_*> (r);

  ULong nEl = N_Elements();
  assert(nEl);
  Data_* res = NewResult();
  if (nEl == 1) {
	(*res)[0] = pow((*right)[0], (*this)[0]);
	return res;
  }
  Ty s = (*right)[0];
  if ((GDL_NTHREADS=parallelize( nEl))==1) {
    for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow(s, (*this)[ i]);
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = pow(s, (*this)[ i]);
  }
  return res;
}
// invalid types

template<>
Data_<SpDString>* Data_<SpDString>::PowSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDString>* Data_<SpDString>::PowInvSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype STRING.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowInvSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype PTR.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::PowSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::PowInvSNew(BaseGDL* r) {
  throw GDLException("Cannot apply operation to datatype OBJECT.", true, false);
  return NULL;
}

#include "instantiate_templates.hpp"
