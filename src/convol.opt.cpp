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
      // cout << "aEnd[" << aSp << "]=" << aEnd[ aSp] << endl;
    }

  Ty* ddP = &dd[0];

  // some loop constants
  SizeT dim0 = dim[0];
  SizeT dim0_1 = dim0 - 1;
  SizeT dim0_aEnd0 = dim0 - aEnd[0];
  SizeT kDim0 = kernel->Dim( 0);
  SizeT	kDim0_nDim = kDim0 * nDim;

  // for all result elements
  for( SizeT a=0; a<nA; ++aInitIx[1])
    {
      bool regular = true;
      for( SizeT aSp=1; aSp<nDim;)
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

      if( regular)
	{
	  if( edgeMode == 1) //edge_wrap
	    {
	      // 0-dim beginning
	      for( long aInitIx0 = 0; aInitIx0 < aBeg[0]; ++aInitIx0, ++a)
		{
#ifdef CONVOL_BYTE__
		  DInt res_a = 0;
#else
		  Ty& res_a = (*res)[ a];
#endif
		  long m_aInitIx0 = -aInitIx0;
		  long* kIx = kIxArr;
		  for( SizeT k=0; k<nK; ++k)
		    {
		      SizeT aLonIx;
		      if( kIx[0] < m_aInitIx0)
			aLonIx = aInitIx0 + kIx[0] + dim0;
		      else
			aLonIx = aInitIx0 + kIx[0];

		      for( SizeT rSp=1; rSp<nDim; ++rSp)
			aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];

		      res_a += ddP[ aLonIx] * ker[ k]; 

		      kIx += nDim;
		    }

		  res_a /= scale;

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
	    }
	  else if( edgeMode == 2) //edge_truncate
	    {
	      // 0-dim beginning
	      for( long aInitIx0 = 0; aInitIx0 < aBeg[0]; ++aInitIx0, ++a)
		{
#ifdef CONVOL_BYTE__
		  DInt res_a = 0;
#else
		  Ty& res_a = (*res)[ a];
#endif
		  long m_aInitIx0 = -aInitIx0;
		  long* kIx = kIxArr;
		  for( SizeT k=0; k<nK; ++k)
		    {
		      SizeT aLonIx;
		      if( kIx[0] < m_aInitIx0)
			aLonIx = 0;
		      else
			aLonIx = aInitIx0 + kIx[0];

		      for( SizeT rSp=1; rSp<nDim; ++rSp)
			aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];

		      res_a += ddP[ aLonIx] * ker[ k]; 
		  
		      kIx += nDim;
		    }

		  res_a /= scale;

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
	    }
	  else { a += aBeg[0];} // update a

//   	  // 0-dim regular 
// 	  for( long aInitIx0 = aBeg[0]; aInitIx0 < aEnd[0]; ++aInitIx0, ++a)
// 	    {
// #ifdef CONVOL_BYTE__
// 	      DInt res_a = 0;
// #else
// 	      Ty& res_a = (*res)[ a];
// #endif
// 	      long* kIx = kIxArr;
// 	      for( SizeT k=0; k<nK; ++k)
// 		{
// 		  SizeT aLonIx = aInitIx0 + kIx[0];
// 		  for( SizeT rSp=1; rSp<nDim; ++rSp)
// 		    aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];

// 		  res_a += ddP[ aLonIx] * ker[ k]; 
		  
// 		  kIx += nDim;
// 		}

// 	      res_a /= scale;

// #ifdef CONVOL_BYTE__
// 	      if( res_a > 0) 
// 		if( res_a < 255)
// 		  (*res)[ a] = res_a;
// 		else
// 		  (*res)[ a] = 255;
// 	      else
// 		(*res)[ a] = 0;
// #endif
// 	    }


  	  // 0-dim regular 
	  if( center)
	    {
	  for( long aInitIx0 = aBeg[0]; aInitIx0 < aEnd[0]; ++aInitIx0, ++a)
	    {
#ifdef CONVOL_BYTE__
	      DInt res_a = 0;
#else
	      Ty& res_a = (*res)[ a];
#endif

	      long* kIx = kIxArr;
	      for( SizeT k=0; k<nK; k+=kDim0)
		{
		  SizeT aLonIx = aInitIx0 + kIx[0];
		  for( SizeT rSp=1; rSp<nDim; ++rSp)
		    aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];

		  for( SizeT k0=0; k0<kDim0; ++k0)
		    res_a += ddP[ aLonIx+k0] * ker[ k+k0]; 

		  kIx += kDim0_nDim;
		}

	      res_a /= scale;

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
	    }
	  else
	    {
	  for( long aInitIx0 = aBeg[0]; aInitIx0 < aEnd[0]; ++aInitIx0, ++a)
	    {
#ifdef CONVOL_BYTE__
	      DInt res_a = 0;
#else
	      Ty& res_a = (*res)[ a];
#endif

	      long* kIx = kIxArr;
	      for( SizeT k=0; k<nK; k+=kDim0)
		{
		  SizeT aLonIx = aInitIx0 + kIx[0];
		  for( SizeT rSp=1; rSp<nDim; ++rSp)
		    aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];

		  for( SizeT k0=0; k0<kDim0; ++k0)
		    res_a += ddP[ aLonIx-k0] * ker[ k+k0]; 

		  kIx += kDim0_nDim;
		}

	      res_a /= scale;

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
	    }



	  if( edgeMode == 1) //edge_wrap
	    {
	      // 0-dim end
	      for( long aInitIx0 = aEnd[0]; aInitIx0 < dim0; ++aInitIx0, ++a)
		{
#ifdef CONVOL_BYTE__
		  DInt res_a = 0;
#else
		  Ty& res_a = (*res)[ a];
#endif
		  long* kIx = kIxArr;
		  for( SizeT k=0; k<nK; ++k)
		    {
		      SizeT aLonIx = aInitIx0 + kIx[0];
		      if( aLonIx >= dim0) aLonIx -= dim0;
		      for( SizeT rSp=1; rSp<nDim; ++rSp)
			aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];

		      res_a += ddP[ aLonIx] * ker[ k]; 
		  
		      kIx += nDim;
		    }

		  res_a /= scale;

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

	    }
	  else if( edgeMode == 2) //edge_truncate
	    {
	      // 0-dim end
	      for( long aInitIx0 = aEnd[0]; aInitIx0 < dim0; ++aInitIx0, ++a)
		{
#ifdef CONVOL_BYTE__
		  DInt res_a = 0;
#else
		  Ty& res_a = (*res)[ a];
#endif
		  long* kIx = kIxArr;
		  for( SizeT k=0; k<nK; ++k)
		    {
		      SizeT aLonIx = aInitIx0 + kIx[0];
		      if( aLonIx >= dim0) aLonIx = dim0_1;
		      for( SizeT rSp=1; rSp<nDim; ++rSp)
			aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];

		      res_a += ddP[ aLonIx] * ker[ k]; 
		  
		      kIx += nDim;
		    }

		  res_a /= scale;

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
	    }
	  else { a += dim0_aEnd0;} // update a
	} // if( regular) // (dim 1-n)
      else
	{ // necessary because of update of 'a'

	  // non-regular in dim 1-n
	  if( edgeMode == 1) //edge_wrap
	    {
	      // 0-dim beginning
	      for( long aInitIx0 = 0; aInitIx0 < aBeg[0]; ++aInitIx0, ++a)
		{
#ifdef CONVOL_BYTE__
		  DInt res_a = 0;
#else
		  Ty& res_a = (*res)[ a];
#endif
		  long m_aInitIx0 = -aInitIx0;
		  long* kIx = kIxArr;
		  for( SizeT k=0; k<nK; ++k)
		    {
		      SizeT aLonIx;
		      if( kIx[0] < m_aInitIx0)
			aLonIx = aInitIx0 + kIx[0] + dim0;
		      else
			aLonIx = aInitIx0 + kIx[0];

		      for( SizeT rSp=1; rSp<nDim; ++rSp)
			{
			  long aIx = aInitIx[ rSp] + kIx[ rSp];
			  if( aIx < 0)
			    aIx += dim[ rSp];
			  else if( aIx >= dim[ rSp])
			    aIx -= dim[ rSp];
			  
			  aLonIx += aIx * aStride[ rSp];
			}

		      res_a += ddP[ aLonIx] * ker[ k]; 
		  
		      kIx += nDim;
		    }

		  res_a /= scale;

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
	      // 0-dim regular 
	      for( long aInitIx0 = aBeg[0]; aInitIx0 < aEnd[0]; ++aInitIx0, ++a)
		{
#ifdef CONVOL_BYTE__
		  DInt res_a = 0;
#else
		  Ty& res_a = (*res)[ a];
#endif
		  long* kIx = kIxArr;
		  for( SizeT k=0; k<nK; ++k)
		    {
		      SizeT aLonIx = (aInitIx0 + kIx[0]);
		      for( SizeT rSp=1; rSp<nDim; ++rSp)
			{
			  long aIx = aInitIx[ rSp] + kIx[ rSp];
			  if( aIx < 0)
			    aIx += dim[ rSp];
			  else if( aIx >= dim[ rSp])
			    aIx -= dim[ rSp];
			  
			  aLonIx += aIx * aStride[ rSp];
			}

		      res_a += ddP[ aLonIx] * ker[ k]; 
		  
		      kIx += nDim;
		    }

		  res_a /= scale;

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
	      // 0-dim end
	      for( long aInitIx0 = aEnd[0]; aInitIx0 < dim0; ++aInitIx0, ++a)
		{
#ifdef CONVOL_BYTE__
		  DInt res_a = 0;
#else
		  Ty& res_a = (*res)[ a];
#endif
		  long* kIx = kIxArr;
		  for( SizeT k=0; k<nK; ++k)
		    {
		      SizeT aLonIx = aInitIx0 + kIx[0];
		      if( aLonIx >= dim0) aLonIx -= dim0;
		      for( SizeT rSp=1; rSp<nDim; ++rSp)
			{
			  long aIx = aInitIx[ rSp] + kIx[ rSp];
			  if( aIx < 0)
			    aIx += dim[ rSp];
			  else if( aIx >= dim[ rSp])
			    aIx -= dim[ rSp];
		
			  aLonIx += aIx * aStride[ rSp];
			}

		      res_a += ddP[ aLonIx] * ker[ k]; 
		  
		      kIx += nDim;
		    }

		  res_a /= scale;

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

	    }
	  else if( edgeMode == 2) //edge_truncate
	    {
	      // 0-dim beginning
	      for( long aInitIx0 = 0; aInitIx0 < aBeg[0]; ++aInitIx0, ++a)
		{
#ifdef CONVOL_BYTE__
		  DInt res_a = 0;
#else
		  Ty& res_a = (*res)[ a];
#endif
		  long m_aInitIx0 = -aInitIx0;
		  long* kIx = kIxArr;
		  for( SizeT k=0; k<nK; ++k)
		    {
		      SizeT aLonIx;
		      if( kIx[0] < m_aInitIx0)
			aLonIx = 0;
		      else
			aLonIx = aInitIx0 + kIx[0];
		      for( SizeT rSp=1; rSp<nDim; ++rSp)
			{
			  long aIx = aInitIx[ rSp] + kIx[ rSp];
			  if( aIx < 0)
			    aIx = 0;
			  else if( aIx >= dim[ rSp])
			    aIx = dim[ rSp] - 1;
		
			  aLonIx += aIx * aStride[ rSp];
			}

		      res_a += ddP[ aLonIx] * ker[ k]; 
		  
		      kIx += nDim;
		    }

		  res_a /= scale;

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
	      // 0-dim regular 
	      for( long aInitIx0 = aBeg[0]; aInitIx0 < aEnd[0]; ++aInitIx0, ++a)
		{
#ifdef CONVOL_BYTE__
		  DInt res_a = 0;
#else
		  Ty& res_a = (*res)[ a];
#endif
		  long* kIx = kIxArr;
		  for( SizeT k=0; k<nK; ++k)
		    {
		      SizeT aLonIx = (aInitIx0 + kIx[0]);
		      for( SizeT rSp=1; rSp<nDim; ++rSp)
			{
			  long aIx = aInitIx[ rSp] + kIx[ rSp];
			  if( aIx < 0)
			    aIx = 0;
			  else if( aIx >= dim[ rSp])
			    aIx = dim[ rSp] - 1;
		
			  aLonIx += aIx * aStride[ rSp];
			}

		      res_a += ddP[ aLonIx] * ker[ k]; 
		  
		      kIx += nDim;
		    }

		  res_a /= scale;

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
	      // 0-dim end
	      for( long aInitIx0 = aEnd[0]; aInitIx0 < dim0; ++aInitIx0, ++a)
		{
#ifdef CONVOL_BYTE__
		  DInt res_a = 0;
#else
		  Ty& res_a = (*res)[ a];
#endif
		  long* kIx = kIxArr;
		  for( SizeT k=0; k<nK; ++k)
		    {
		      SizeT aLonIx = aInitIx0 + kIx[0];
		      if( aLonIx >= dim0)
			aLonIx = dim0_1;

		      for( SizeT rSp=1; rSp<nDim; ++rSp)
			{
			  long aIx = aInitIx[ rSp] + kIx[ rSp];
			  if( aIx < 0)
			    aIx = 0;
			  else if( aIx >= dim[ rSp])
			    aIx = dim[ rSp] - 1;
		
			  aLonIx += aIx * aStride[ rSp];
			}

		      res_a += ddP[ aLonIx] * ker[ k]; 
		  
		      kIx += nDim;
		    }

		  res_a /= scale;

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
	    }
	  else { a += dim0;} // update a

	} // if( regular) else
    } // for(...)

  return res;
}

#endif // #ifdef INCLUDE_CONVOL_CPP

