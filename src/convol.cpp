/***************************************************************************
                          convol.cpp  -  convol function
                             -------------------
    begin                : Sep 19 2004
    copyright            : (C) 2004 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// optimized version

// to be included from datatypes.cpp (twice)
#ifdef INCLUDE_CONVOL_CPP

#ifndef CONVOL_COMMON__
#define CONVOL_COMMON__

// common used functions

#endif //#define CONVOL_COMMON__

#ifdef CONVOL_BYTE__
template<>
BaseGDL* Data_<SpDByte>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, 
				 bool center, int edgeMode)
{
  Data_<SpDInt>* kernel = static_cast<Data_<SpDInt>*>( kIn);
  DInt scale = (*static_cast<Data_<SpDInt>*>( scaleIn))[0];
  // the result to be returned
  Data_* res = New( dim, BaseGDL::ZERO);
  DInt* ker = static_cast<DInt*>( kernel->DataAddr());
#else
template<class Sp>
BaseGDL* Data_<Sp>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, 
			    bool center, int edgeMode)
{
  Data_* kernel = static_cast<Data_*>( kIn);
  Ty scale = static_cast<Data_*>( scaleIn)->dd[0];
  // the result to be returned
  Data_* res = New( this->dim, BaseGDL::ZERO);
  Ty* ker = &kernel->dd[0];
#endif
  if( scale == this->zero) scale = 1;

  SizeT nA = N_Elements();
  SizeT nK = kernel->N_Elements();

  // general case (look at kernel rank == 1 later)
  SizeT nDim = this->Rank(); // number of dimension to run over

  SizeT kStride[MAXRANK+1];
  kernel->Dim().Stride( kStride, nDim);

  // setup kIxArr[ nDim * nK] the offset array
  // this handles center
  long* kIxArr = new long[ nDim * nK];
  ArrayGuard<long> kIxArrGuard( kIxArr); // guard it
  for( SizeT k=0; k<nK; ++k)
    {
      kIxArr[ k * nDim + 0] = -(k % kernel->Dim( 0));
      if( center) kIxArr[ k * nDim + 0] = -(kIxArr[ k * nDim + 0] + 
					    kernel->Dim( 0) / 2);
      for( SizeT kSp=1; kSp<nDim; ++kSp)
	{
	  SizeT kDim = kernel->Dim( kSp);
	  if( kDim == 0) kDim = 1;
	  kIxArr[ k * nDim + kSp] = -((k / kStride[kSp]) % kDim);
	  if( center) kIxArr[ k * nDim + kSp] = -(kIxArr[ k * nDim + kSp] + 
						  kDim / 2);
	}
    }

  SizeT  aStride[ MAXRANK + 1];
  this->dim.Stride( aStride, nDim);

  long  aInitIx[ MAXRANK+1];
  for( SizeT aSp=0; aSp<=nDim; ++aSp) aInitIx[ aSp] = 0;

  bool  regArr[ MAXRANK];

  long  aBeg[ MAXRANK];
  long  aEnd[ MAXRANK];
  for( SizeT aSp=0; aSp<nDim; ++aSp) 
    {
      SizeT kDim = kernel->Dim( aSp);
      if( kDim == 0) kDim = 1;
      aBeg[ aSp] = (center) ? kDim/2 : kDim-1; // >= 
      regArr[ aSp] = !aBeg[ aSp];
      aEnd[ aSp] = (center) ? this->dim[aSp]-(kDim-1)/2 : this->dim[aSp]; // <
    }

  Ty* ddP = &dd[0];

  // some loop constants
  SizeT dim0  = this->dim[0];
  SizeT aBeg0 = aBeg[0];
  SizeT aEnd0 = aEnd[0];
  SizeT dim0_1     = dim0 - 1;
  SizeT dim0_aEnd0 = dim0 - aEnd[0];
  SizeT kDim0      = kernel->Dim( 0);
  SizeT	kDim0_nDim = kDim0 * nDim;

#define INCLUDE_CONVOL_INC_CPP
  if( edgeMode == 0)
    {
#include "convol_inc0.cpp"
    }
  else if( edgeMode == 1)
    {
#include "convol_inc1.cpp"
    }
  else if( edgeMode == 2)
    {
#include "convol_inc2.cpp"
    }
#undef INCLUDE_CONVOL_INC_CPP

  return res;
}

#endif // #ifdef INCLUDE_CONVOL_CPP

