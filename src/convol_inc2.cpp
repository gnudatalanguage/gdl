/***************************************************************************
                          convol_inc2.cpp  -  convol function EDGE_TRUNCATE
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

// to be included from convol.cpp

#ifdef INCLUDE_CONVOL_INC_CPP

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
      // 0-dim beginning
      for( long aInitIx0 = 0; aInitIx0 < aBeg0; ++aInitIx0, ++a)
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

      // 0-dim regular 
      if( center)
	for( long aInitIx0 = aBeg0; aInitIx0 < aEnd0; ++aInitIx0, ++a)
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
      else
	for( long aInitIx0 = aBeg0; aInitIx0 < aEnd0; ++aInitIx0, ++a)
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

      // 0-dim end
      for( long aInitIx0 = aEnd0; aInitIx0 < dim0; ++aInitIx0, ++a)
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

    } // if( regular) // in dim 1-n
  else
    { 
      // 0-dim beginning
      for( long aInitIx0 = 0; aInitIx0 < aBeg0; ++aInitIx0, ++a)
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
      if( center)
	for( long aInitIx0 = aBeg0; aInitIx0 < aEnd0; ++aInitIx0, ++a)
	  {
#ifdef CONVOL_BYTE__
	    DInt res_a = 0;
#else
	    Ty& res_a = (*res)[ a];
#endif
	    long* kIx = kIxArr;
	    for( SizeT k=0; k<nK; k+=kDim0)
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

		for( SizeT k0=0; k0<kDim0; ++k0)
		  res_a += ddP[ aLonIx+k0] * ker[ k+k0]; 
		//		      res_a += ddP[ aLonIx] * ker[ k]; 
		  
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
      else
	for( long aInitIx0 = aBeg0; aInitIx0 < aEnd0; ++aInitIx0, ++a)
	  {
#ifdef CONVOL_BYTE__
	    DInt res_a = 0;
#else
	    Ty& res_a = (*res)[ a];
#endif
	    long* kIx = kIxArr;
	    for( SizeT k=0; k<nK; k+=kDim0)
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

		for( SizeT k0=0; k0<kDim0; ++k0)
		  res_a += ddP[ aLonIx-k0] * ker[ k+k0]; 
		//		      res_a += ddP[ aLonIx] * ker[ k]; 
		  
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

      // 0-dim end
      for( long aInitIx0 = aEnd0; aInitIx0 < dim0; ++aInitIx0, ++a)
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
    } // if( regular) else
} // for(...)

#endif
