/***************************************************************************
                          convol.cpp  -  convol function
                             -------------------
    begin                : Apr 19 2004
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

// this version is simplyfied, but much shorter and easier to understand 
// than the optimized version

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
  Data_* res = New( dim, BaseGDL::NOZERO);
  DInt* ker = static_cast<DInt*>( kernel->DataAddr());
#else
template<class Sp>
BaseGDL* Data_<Sp>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, 
			    bool center, int edgeMode)
{
  Data_* kernel = static_cast<Data_*>( kIn);
  Ty scale = static_cast<Data_*>( scaleIn)->dd[0];
  // the result to be returned
  Data_* res = New( dim, BaseGDL::ZERO);
  Ty* ker = &kernel->dd[0];
#endif
  if( scale == zero) scale = 1;

  SizeT nA = N_Elements();
  SizeT nK = kernel->N_Elements();

  // general case (look at kernel rank == 1 later)
  SizeT nDim = Rank(); // number of dimension to run over

  SizeT kStride[MAXRANK+1];
  kernel->Dim().Stride( kStride, nDim);

  // setup kIxArr[ nDim * nK] the offset array
  // this handles center
  long* kIxArr = new long[ nDim * nK];
  long* kIxEnd = &kIxArr[ (nK-1) * nDim];
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
  dim.Stride( aStride, nDim);

  long  aInitIx[ MAXRANK+1];
  for( SizeT aSp=0; aSp<=nDim; ++aSp) aInitIx[ aSp] = 0;

  bool  regArr[ MAXRANK];

  long  aBeg[ MAXRANK];
  long  aEnd[ MAXRANK];
  for( SizeT aSp=0; aSp<nDim; ++aSp) 
    {
      aBeg[ aSp] = (center) ? kernel->Dim(aSp)/2 : kernel->Dim(aSp)-1; // >= 
      regArr[ aSp] = !aBeg[ aSp];
      aEnd[ aSp] = (center) ? dim[aSp]-(kernel->Dim(aSp)-1)/2 : dim[aSp]; // <
    }

  Ty* ddP = &dd[0];

  // for all result elements
  for( SizeT a=0; a<nA; ++a, ++aInitIx[0])
    {
      bool regular = true;
      for( SizeT aSp=0; aSp<nDim;)
	{
	  if( aInitIx[ aSp] < dim[ aSp])
	    {
	      regArr[ aSp] = 
		aInitIx[aSp] >= aBeg[aSp] && aInitIx[aSp] < aEnd[ aSp];

	      if( regular)
		for(; aSp<nDim; ++aSp)
		  if( !regArr[ aSp])
		    {
		      regular = false; 
			break;
		    }
	      break;
	    }

	  aInitIx[ aSp] = 0;
	  regArr[ aSp] = !aBeg[ aSp];
	  if( aBeg[ aSp]) regular = false; 
	  
	  ++aInitIx[ ++aSp];
	}

#ifdef CONVOL_BYTE__
      DInt res_a = 0;
#else
      Ty& res_a = (*res)[ a];
#endif
      if( regular)
	{
	  long* kIx = kIxArr;
	  for( SizeT k=0; k<nK; ++k)
	    {
	      SizeT aLonIx = (aInitIx[ 0] + kIx[ 0]) * aStride[ 0];
	      for( SizeT rSp=1; rSp<nDim; ++rSp)
		aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];

	      res_a += ddP[ aLonIx] * ker[ k]; 

	      kIx += nDim;
	    }

	  res_a /= scale;
	}
      else if( edgeMode == 1) //edge_wrap
	{
	  long* kIx = kIxArr;
	  for( SizeT k=0; k<nK; ++k)
	    {
	      SizeT aLonIx=0;
	      for( SizeT rSp=0; rSp<nDim; ++rSp)
		{
		  long aIx = aInitIx[ rSp] + kIx[ rSp];
		  if( aIx < 0)
		    aIx += dim[ rSp];
		  else if( aIx >= dim[ rSp])
		    aIx -= dim[ rSp];
		
		  aLonIx += aIx * aStride[ rSp];
		}

	      //	      res_a += dd[ aLonIx] * (*kernel)[ k]; 
	      res_a += ddP[ aLonIx] * ker[ k]; 

	      // advance kIx
	      kIx += nDim;
	    }

	  res_a /= scale;
	}
      else if( edgeMode == 2) //edge_truncate
	{
	  long* kIx = kIxArr;
	  for( SizeT k=0; k<nK; ++k)
	    {
	      // kIx[ nDim] -> nDim index of  k'th element

	      SizeT aLonIx=0;
	      for( SizeT rSp=0; rSp<nDim; ++rSp)
		{
		  long aIx = aInitIx[ rSp] + kIx[ rSp];
		  if( aIx < 0)
		    aIx = 0;
		  else if( aIx >= dim[ rSp])
		    aIx = dim[ rSp] - 1;
		
		  aLonIx += aIx * aStride[ rSp];
		}

	      //	      res_a += dd[ aLonIx] * (*kernel)[ k]; 
	      res_a += ddP[ aLonIx] * ker[ k]; 

	      // advance kIx
	      kIx += nDim;
	    }

	  res_a /= scale;
	}

#ifdef CONVOL_BYTE__
      if( res_a > 0) 
	if( res_a < 255)
	  (*res)[ a] = res_a;
	else
	  (*res)[ a] = 255;
      else
	(*res)[ a] = 0;
#endif
    }

  return res;
}

#endif // #ifdef INCLUDE_CONVOL_CPP

