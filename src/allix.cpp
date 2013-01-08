/***************************************************************************
                          allix.cpp  -  indexer definitions
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2011 by Marc Schellens
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
#include "includefirst.hpp"

#include "arrayindex.hpp"
#include "allix.hpp"
 
// older versions of gcc put the vtable into this file (where destructor is defined)
AllIxBaseT::~AllIxBaseT() {}

SizeT AllIxIndicesT::operator[]( SizeT i) const
{
assert( upperSet);
SizeT index = ref->GetAsIndex( i);
if( index > upper)
	return upper;
return index;
}

SizeT AllIxIndicesT::InitSeqAccess()
{
assert( upperSet);
seqIx = 0;
SizeT index = ref->GetAsIndex( 0);
if( index > upper)
	return upper;
return index;
}

SizeT AllIxIndicesT::SeqAccess() 
{
assert( upperSet);
SizeT index = ref->GetAsIndex( ++seqIx);
if( index > upper)
	return upper;
return index;
}

SizeT AllIxIndicesT::size() const
{
return ref->N_Elements();
}

SizeT AllIxIndicesStrictT::operator[]( SizeT i) const
{
assert( upperSet);
SizeT index = ref->GetAsIndexStrict( i);
if( index > upper)
	throw GDLException(-1,NULL,"Array used to subscript array "
			   "contains out of range subscript (at index: "+i2s(i)+").",true,false);
return index;
}

SizeT AllIxIndicesStrictT::InitSeqAccess()
{
assert( upperSet);
seqIx = 0;
SizeT index = ref->GetAsIndexStrict( 0);
if( index > upper)
	throw GDLException(-1,NULL,"Array used to subscript array "
			   "contains out of range subscript (at index: "+i2s(index)+").",true,false);
return index;
}

SizeT AllIxIndicesStrictT::SeqAccess() 
{
assert( upperSet);
SizeT index = ref->GetAsIndexStrict( ++seqIx);
if( index > upper)
	throw GDLException(-1,NULL,"Array used to subscript array "
			   "contains out of range subscript (at index: "+i2s(index)+").",true,false);
return index;
}

SizeT AllIxAllIndexedT::operator[]( SizeT i) const
  {
    assert( i < nIx);
	
    assert( dynamic_cast<ArrayIndexIndexed*>( (*ixList)[0]) != NULL);
    SizeT resIndex = static_cast< ArrayIndexIndexed*>( (*ixList)[0])->GetIx( i);
		
    for( SizeT l=1; l < acRank; ++l)
      {
		assert( dynamic_cast<ArrayIndexIndexed*>( (*ixList)[l]) != NULL);
		resIndex += static_cast< ArrayIndexIndexed*>( (*ixList)[l])->GetIx( i) * varStride[l];
      }
    return resIndex;
  }
SizeT AllIxAllIndexedT::InitSeqAccess()
  {
    seqIx = 0;
    
    assert( dynamic_cast<ArrayIndexIndexed*>( (*ixList)[0]) != NULL);
    SizeT resIndex = static_cast< ArrayIndexIndexed*>( (*ixList)[0])->GetIx( 0 /*seqIx*/);
		
    for( SizeT l=1; l < acRank; ++l)
      {
		assert( dynamic_cast<ArrayIndexIndexed*>( (*ixList)[l]) != NULL);
		resIndex += static_cast< ArrayIndexIndexed*>( (*ixList)[l])->GetIx( 0 /*seqIx*/) * varStride[l];
      }
    return resIndex;
  }
SizeT AllIxAllIndexedT::SeqAccess()
  {
    ++seqIx;
    assert( seqIx < nIx);
	
    assert( dynamic_cast<ArrayIndexIndexed*>( (*ixList)[0]) != NULL);    
    SizeT resIndex = static_cast< ArrayIndexIndexed*>( (*ixList)[0])->GetIx( seqIx);	
    for( SizeT l=1; l < acRank; ++l)
      {
		assert( dynamic_cast<ArrayIndexIndexed*>( (*ixList)[l]) != NULL);
		resIndex += static_cast< ArrayIndexIndexed*>( (*ixList)[l])->GetIx( seqIx) * varStride[l];
      }
    return resIndex;
  }

	
  
// Note: mixed indices can expand an array
SizeT AllIxNewMultiT::operator[]( SizeT i) const
  {
    assert( i < nIx);
    
    SizeT resIndex = add;
    if( (*ixList)[0]->Indexed())
      {
		resIndex += static_cast< ArrayIndexIndexed*>((*ixList)[0])->GetIx( i %  nIterLimit[0]);
      }
    else
      {
// 		SizeT s = ixList->FrontGetS(); //ixList[0]->GetS();
		if( nIterLimit[0] > 1)
			resIndex += (i % nIterLimit[0]) * ixListStride[0]; // + s[0];
      }

    for( SizeT l=1; l < acRank; ++l)
    {
		if( (*ixList)[l]->Indexed())
		{
			resIndex += static_cast< ArrayIndexIndexed*>( (*ixList)[l])->GetIx( (i / stride[l]) %  nIterLimit[l]) * varStride[l];
		}
		else
		{
// 			SizeT s = (*ixList)[l]->GetS();
			if( nIterLimit[l] > 1)
				resIndex += ((i / stride[l]) %  nIterLimit[l]) * ixListStride[l]; //  + s[l] * varStride[l];
// 			resIndex += (((i / stride[l]) %  nIterLimit[l]) * ixListStride[l] + s[l]) * varStride[l];
		}
	}
	return resIndex;
  }
SizeT AllIxNewMultiT::InitSeqAccess()
{
// 	seqIx = 0;
// 	return (*this)[0];
	seqIter = 0;
	seqIter0 = 0;
	ix2 = add;
	for( SizeT l=1; l < acRank; ++l)
	{
		if( (*ixList)[l]->Indexed())
		{
			ix2 += static_cast< ArrayIndexIndexed*>( (*ixList)[l])->GetIx( 0) * varStride[l];
		}
	}

	seqIx = ix2;
	if( (*ixList)[0]->Indexed())
	{
		seqIx += static_cast< ArrayIndexIndexed*>((*ixList)[0])->GetIx( 0);
	}

	assert( seqIx == (*this)[seqIter+seqIter0]);
	return seqIx; //(*this)[0];
}
SizeT AllIxNewMultiT::SeqAccess()
{
// 	return (*this)[++seqIx];
	++seqIter0;
	if( seqIter0 >= stride[1])
	{
		seqIter += stride[1];
		seqIter0 = 0;
		ix2 = add;
		for( SizeT l=1; l < acRank; ++l)
			{
				if( (*ixList)[l]->Indexed())
				{
					ix2 += static_cast< ArrayIndexIndexed*>( (*ixList)[l])->GetIx( (seqIter / stride[l]) %  nIterLimit[l]) * varStride[l];
				}
				else
				{
					if( nIterLimit[l] > 1)
						ix2 += ((seqIter / stride[l]) %  nIterLimit[l]) * ixListStride[l];
				}
			}
		seqIx = ix2;
		if( (*ixList)[0]->Indexed())
		{
			seqIx += static_cast< ArrayIndexIndexed*>((*ixList)[0])->GetIx( seqIter0);
		}
		assert( seqIx == (*this)[seqIter+seqIter0]);
		return seqIx;
	}
	seqIx = ix2;
	if( (*ixList)[0]->Indexed())
	{
		seqIx += static_cast< ArrayIndexIndexed*>((*ixList)[0])->GetIx( seqIter0);
	}
	else
	{
		if( nIterLimit[0] > 1)
			seqIx += seqIter0 * ixListStride[0]; // + s[0];
	}
	assert( seqIx == (*this)[seqIter+seqIter0]);
	return seqIx;
}






SizeT AllIxNewMulti2DT::operator[]( SizeT i) const
  {
	assert( i < nIx);

	SizeT resIndex = add;
	if( (*ixList)[0]->Indexed())
	{
		resIndex += static_cast< ArrayIndexIndexed*>((*ixList)[0])->GetIx( i %  nIterLimit[0]);
	}
	else
	{
		if( nIterLimit[0] > 1)
			resIndex += (i % nIterLimit[0]) * ixListStride[0]; // + s[0];
	}

	if( (*ixList)[1]->Indexed())
	{
		return resIndex + static_cast< ArrayIndexIndexed*>( (*ixList)[1])->GetIx( (i / stride[1]) %  nIterLimit[1]) * varStride[1];
	}
	else
	{
		if( nIterLimit[1] > 1)
			return resIndex + ((i / stride[1]) %  nIterLimit[1]) * ixListStride[1];
		else
			return resIndex;
	}
	return resIndex;
  }
SizeT AllIxNewMulti2DT::InitSeqAccess()
{
// 	seqIx = 0;
// 	return (*this)[0];

	seqIter = 0;
	seqIter0 = 0;
	ix2 = add;
	if( (*ixList)[1]->Indexed())
	{
		ix2 += static_cast< ArrayIndexIndexed*>( (*ixList)[1])->GetIx( 0) * varStride[1];
	}

	seqIx = ix2;
	if( (*ixList)[0]->Indexed())
	{
		seqIx += static_cast< ArrayIndexIndexed*>((*ixList)[0])->GetIx( 0);
	}

	assert( seqIx == (*this)[seqIter+seqIter0]);
	return seqIx; //(*this)[0];
}
SizeT AllIxNewMulti2DT::SeqAccess()
{
// 	return (*this)[++seqIx];

	++seqIter0;
	if( seqIter0 >= stride[1])
	{
		seqIter += stride[1];
		seqIter0 = 0;
		ix2 = add;
		if( (*ixList)[1]->Indexed())
		{
			ix2 += static_cast< ArrayIndexIndexed*>( (*ixList)[1])->GetIx( (seqIter / stride[1]) %  nIterLimit[1]) * varStride[1];
		}
		else
		{
			if( nIterLimit[1] > 1)
				ix2 += ((seqIter / stride[1]) %  nIterLimit[1]) * ixListStride[1];
		}
		seqIx = ix2;
		if( (*ixList)[0]->Indexed())
		{
			seqIx += static_cast< ArrayIndexIndexed*>((*ixList)[0])->GetIx( seqIter0);
		}
		assert( seqIx == (*this)[seqIter+seqIter0]);
		return seqIx;
	}
	
	seqIx = ix2;
	if( (*ixList)[0]->Indexed())
	{
		seqIx += static_cast< ArrayIndexIndexed*>((*ixList)[0])->GetIx( seqIter0);
	}
	else
	{
		if( nIterLimit[0] > 1)
			seqIx += seqIter0 * ixListStride[0]; // + s[0];
	}
	assert( seqIx == (*this)[seqIter+seqIter0]);
	return seqIx;
}










SizeT AllIxNewMultiNoneIndexedT::operator[]( SizeT i) const
  {
    assert( i < nIx);
    
    SizeT resIndex = add;
    if( nIterLimit[0] > 1)
	resIndex += (i % nIterLimit[0]) * ixListStride[0];

    for( SizeT l=1; l < acRank; ++l)
    {
	if( nIterLimit[l] > 1)
	  resIndex += ((i / stride[l]) %  nIterLimit[l]) * ixListStride[l];
// resIndex += (((i / stride[l]) %  nIterLimit[l]) * ixListStride[l] + s[l]) * varStride[l];
    }
    return resIndex;
  }
SizeT AllIxNewMultiNoneIndexedT::InitSeqAccess()
{
//  	seqIxDebug = 0;
// 	return (*this)[0];
	seqIx = add;
	seqIter = 0;
	correctionIncrease = stride[1] * ixListStride[0];
	nextCorrection = seqIx + correctionIncrease; // stride[1] == nIterLimit[0]
	return seqIx; //(*this)[0];
}
SizeT AllIxNewMultiNoneIndexedT::SeqAccess() // 1st dim linearized
{
//   return (*this)[++seqIx];
  seqIx += ixListStride[0];
  if( seqIx >= nextCorrection)
    {
      seqIter += stride[1];
      seqIx = add;
      for( SizeT l=1; l < acRank; ++l)
      {
	  if( nIterLimit[l] > 1)
	    seqIx += ((seqIter / stride[l]) %  nIterLimit[l]) * ixListStride[l];
      }
      nextCorrection = seqIx + correctionIncrease; // stride[1] == nIterLimit[0]
    }
  return seqIx; // fast path
}


// acRank == 2
SizeT AllIxNewMultiNoneIndexed2DT::operator[]( SizeT i) const
  {
    // stride[1] == nIterLimit[0] (see SetVariable(...))
    assert( i < nIx);

    // otherwise AllIxNewMultiOneVariableIndex...T in MakeArrayIndex
    assert( nIterLimit[0] > 1 && nIterLimit[1] > 1);
	
    SizeT resIndex = add;
// if( nIterLimit[0] > 1)
    resIndex += (i % nIterLimit[0]) * ixListStride[0];
// if( nIterLimit[1] > 1)
    // 2D: nIx == nIterLimit[0] * nIterLimit[1] && stride[1] == nIterLimit[0] 
    // -> nIterLimit[1] > i/stride[1] -> no %
    resIndex += (i / stride[1]) * ixListStride[1];
// resIndex += ((i / stride[1]) %  nIterLimit[1]) * ixListStride[1];
// resIndex += (((i / stride[l]) %  nIterLimit[l]) * ixListStride[l] + s[l]) * varStride[l];
    return resIndex;
  }
SizeT AllIxNewMultiNoneIndexed2DT::InitSeqAccess()
{
    seqIx = add;
    correctionIncrease = stride[1] * ixListStride[0];
    nextCorrection = seqIx + correctionIncrease; // stride[1] == nIterLimit[0]
    return seqIx; //(*this)[0];
}
SizeT AllIxNewMultiNoneIndexed2DT::SeqAccess() // linearized
{
//     return (*this)[++seqIx];
    seqIx += ixListStride[0];
    if( seqIx >= nextCorrection)
    {
      // increase 2nd dim
      seqIx += ixListStride[1];
      nextCorrection = seqIx; // stride[1] == nIterLimit[0]
      // correct modulo
      seqIx -= correctionIncrease;
      // set new limit
      // nextCorrection = seqIx + correctionIncrease; // stride[1] == nIterLimit[0]
    }
    return seqIx; // fast path
}



SizeT AllIxNewMultiOneVariableIndexNoIndexT::operator[]( SizeT i) const
  {
	assert( i < nIx);
	return add + i * ixListStride; //  + s[l] * varStride[l];
//      resIndex += (((i / stride[l]) %  nIterLimit[l]) * ixListStride[l] + s[l]) * varStride[l];
	}
SizeT AllIxNewMultiOneVariableIndexNoIndexT::InitSeqAccess()
{
	seqIx = add;
	return add;
}
SizeT AllIxNewMultiOneVariableIndexNoIndexT::SeqAccess()
{
	seqIx += ixListStride;
	assert( (seqIx - add) / ixListStride < nIx);
	return seqIx;
}



SizeT AllIxNewMultiOneVariableIndexIndexedT::operator[]( SizeT i) const
  {
    assert( i < nIx);
    return add + static_cast< ArrayIndexIndexed*>( arrayIndexIndexed)->GetIx( i) * ixListStride; //varStride[l];
  }
SizeT AllIxNewMultiOneVariableIndexIndexedT::InitSeqAccess()
{
    seqIx = 0;
    return add + static_cast< ArrayIndexIndexed*>( arrayIndexIndexed)->GetIx( 0) * ixListStride; //varStride[l];
}
SizeT AllIxNewMultiOneVariableIndexIndexedT::SeqAccess()
{
    assert( (seqIx+1) < nIx);
    return add + static_cast< ArrayIndexIndexed*>( arrayIndexIndexed)->GetIx( ++seqIx) * ixListStride; //varStride[l];
}
