/***************************************************************************
                          math_fun_jmg.cpp  -  math GDL library function (JMG)
                             -------------------
    begin                : March 03 2004
    copyright            : (C) 2002 by Joel Gales
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

#include <iostream>
#include <complex>
#include <cmath>

#include "datatypes.hpp"
#include "envt.hpp"
#include "math_utl.hpp"
#include "math_fun_jmg.hpp"

//#define GDL_DEBUG
#undef GDL_DEBUG

using namespace std;

namespace lib {

//   template< typename T>
//   BaseGDL* transpose_fun_template( BaseGDL* p0,  
// 				   SizeT Rank, dimension dim, SizeT dims[],
// 				   DType type, long perm[])
//   {
//     SizeT sz = sizeof( typename T::Ty);
//     T* p0C = static_cast<T*>( p0);
//     T* res = new T( dim, BaseGDL::NOZERO);
//     memcpy(&(*res)[0], &(*p0C)[0], p0C->N_Elements()*sz);
//     transpose_perm((char *) &(*res)[0], Rank, dims, sz, p0->Type(), perm);
//     return res;
//   }

//   BaseGDL* transpose_fun_jmg( EnvT* e)
//   {
//     SizeT nParam=e->NParam( 1); //, "TRANSPOSE");
//     SizeT dims[MAXRANK];
//     SizeT new_dims[MAXRANK];
//     SizeT j;

//     BaseGDL* p0 = e->GetParDefined( 0); //, "TRANSPOSE");

//     SizeT nEl = p0->N_Elements();
//     if( nEl == 0)
//       throw GDLException( e->CallingNode(), 
// 			  "TRANSPOSE: Variable is undefined: "+
// 			  e->GetParString(0));
    
//     SizeT Rank = p0->Rank();
//     if( Rank == 0)
//       throw GDLException( e->CallingNode(), 
// 			  "TRANSPOSE: Expression must be an array "
// 			  "in this context: "+
// 			  e->GetParString(0));

//     long *perm = NULL;
//     if( nParam == 2) 
//       {
// 	BaseGDL* p1 = e->GetParDefined( 1); //, "TRANSPOSE");
// 	if (p1->N_Elements() != Rank)
// 	  throw GDLException( e->CallingNode(), 
// 			      "TRANSPOSE: Incorrect number of elements "
// 			      "in permutation");

// 	perm = new long[Rank];
// 	DLongGDL* p1L = static_cast<DLongGDL*>
// 	  (p1->Convert2( LONG, BaseGDL::COPY));
// 	for( SizeT i=0; i<Rank; ++i) perm[i] = (*p1L)[ i];

// 	for( SizeT i=0; i<Rank; ++i) {
// 	  for( j=0; j<Rank; ++j) 
// 	    if ((SizeT) perm[j] == i) break;
// 	  if (j == Rank)
// 	    throw GDLException( e->CallingNode(), 
// 				"TRANSPOSE: Incorrect permutation vector");
// 	}
//       }

//     for( SizeT i=0; i<Rank; ++i) dims[i] = p0->Dim(i);
//     if (perm == NULL)
//       for( SizeT i=0; i<Rank; ++i) new_dims[i] = dims[Rank-i-1];
//     else
//       for( SizeT i=0; i<Rank; ++i) new_dims[i] = dims[perm[i]];

//     dimension dim((SizeT *) &new_dims, (SizeT) Rank);


//     if (p0->Type() == COMPLEXDBL) {
//       return transpose_fun_template< DComplexDblGDL>(p0, 
// 						     Rank, dim, dims, 
// 						     p0->Type(), perm);
      
//     } else if (p0->Type() == COMPLEX) {
//       return transpose_fun_template< DComplexGDL>(p0, 
// 						  Rank, dim, dims, 
// 						  p0->Type(), perm);

//     } else if (p0->Type() == DOUBLE) {
//       return transpose_fun_template< DDoubleGDL>(p0, 
// 						 Rank, dim, dims, 
// 						 p0->Type(), perm);

//     } else if (p0->Type() == FLOAT) {
//       return transpose_fun_template< DFloatGDL>(p0, 
// 						Rank, dim, dims, 
// 						p0->Type(), perm);

//     } else if (p0->Type() == LONG) {
//       return transpose_fun_template< DLongGDL>(p0, 
// 					       Rank, dim, dims, 
// 					       p0->Type(), perm);

//     } else if (p0->Type() == ULONG) {
//       return transpose_fun_template< DULongGDL>(p0, 
// 						Rank, dim, dims, 
// 						p0->Type(), perm);

//     } else if (p0->Type() == INT) {
//       return transpose_fun_template< DIntGDL>(p0, 
// 					      Rank, dim, dims, 
// 					      p0->Type(), perm);

//     } else if (p0->Type() == UINT) {
//       return transpose_fun_template< DUIntGDL>(p0, 
// 					       Rank, dim, dims, 
// 					       p0->Type(), perm);

//     } else if (p0->Type() == BYTE) {
//       return transpose_fun_template< DByteGDL>(p0, 
// 					       Rank, dim, dims, 
// 					       p0->Type(), perm);

//     } else if (p0->Type() == STRING) {
//       return transpose_fun_template< DStringGDL>(p0, 
// 						 Rank, dim, dims, 
// 						 p0->Type(), perm);
      
//     }
//     cout << "TRANSPOSE: Type " << p0->TypeStr() << " not handled." << endl;
//     exit( EXIT_FAILURE);
    
//     return p0->Dup(); // (library-) functions always return by value
//   }


  BaseGDL* machar_fun( EnvT* e)
  {
    long int ibeta, it, irnd, ngrd, machep, negep, iexp, minexp, maxexp;
    float eps, epsneg, xmin, xmax;

    // not necessary (maximum number of paramters is checked by interpreter
//     SizeT nParam=e->NParam();
//     if( nParam != 0) {
//       throw GDLException( e->CallingNode(), 
// 			  "MACHAR: Incorrect number of arguments");
//     }
    DStructGDL*  machar = new DStructGDL( "MACHAR");

    machar_s(&ibeta, &it, &irnd, &ngrd, &machep, &negep, &iexp, &minexp, &maxexp, 
	     &eps, &epsneg, &xmin, &xmax );

    machar->InitTag("IBETA", DLongGDL(ibeta));
    machar->InitTag("IT", DLongGDL(it));
    machar->InitTag("IRND", DLongGDL(irnd));
    machar->InitTag("NGRD", DLongGDL(ngrd));
    machar->InitTag("MACHEP", DLongGDL(machep));
    machar->InitTag("NEGEP", DLongGDL(negep));
    machar->InitTag("IEXP", DLongGDL(iexp));
    machar->InitTag("MINEXP", DLongGDL(minexp));
    machar->InitTag("MAXEXP", DLongGDL(maxexp));
    machar->InitTag("EPS", DFloatGDL(eps));
    machar->InitTag("EPSNEG", DFloatGDL(epsneg));
    machar->InitTag("XMIN", DFloatGDL(xmin));
    machar->InitTag("XMAX", DFloatGDL(xmax));

    return machar;
  }

} // namespace








