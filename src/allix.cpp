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
	throw GDLException(NULL,"Array used to subscript array "
			   "contains out of range subscript (at index: "+i2s(i)+").",true,false);
return index;
}

SizeT AllIxIndicesStrictT::InitSeqAccess()
{
assert( upperSet);
seqIx = 0;
SizeT index = ref->GetAsIndexStrict( 0);
if( index > upper)
	throw GDLException(NULL,"Array used to subscript array "
			   "contains out of range subscript (at index: "+i2s(index)+").",true,false);
return index;
}

SizeT AllIxIndicesStrictT::SeqAccess() 
{
assert( upperSet);
SizeT index = ref->GetAsIndexStrict( ++seqIx);
if( index > upper)
	throw GDLException(NULL,"Array used to subscript array "
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
	seqIx = 0;
	return (*this)[0];
}
SizeT AllIxNewMultiT::SeqAccess()
{
	return (*this)[++seqIx];
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
	seqIx = 0;
	return (*this)[0];
}
SizeT AllIxNewMulti2DT::SeqAccess()
{
	return (*this)[++seqIx];
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
// 			resIndex += (((i / stride[l]) %  nIterLimit[l]) * ixListStride[l] + s[l]) * varStride[l];
	}
	return resIndex;
  }
SizeT AllIxNewMultiNoneIndexedT::InitSeqAccess()
{
	seqIx = 0;
	return (*this)[0];
}
SizeT AllIxNewMultiNoneIndexedT::SeqAccess()
{
	return (*this)[++seqIx];
}


// acRank == 2
SizeT AllIxNewMultiNoneIndexed2DT::operator[]( SizeT i) const
  {
    assert( i < nIx);
    // otherwise AllIxNewMultiOneVariableIndex...T in MakeArrayIndex
	assert( nIterLimit[0] > 1 && nIterLimit[1] > 1);
	
    SizeT resIndex = add;
// 	if( nIterLimit[0] > 1)
		resIndex += (i % nIterLimit[0]) * ixListStride[0];
// 	if( nIterLimit[1] > 1)
		resIndex += ((i / stride[1]) %  nIterLimit[1]) * ixListStride[1];
// 			resIndex += (((i / stride[l]) %  nIterLimit[l]) * ixListStride[l] + s[l]) * varStride[l];
	return resIndex;
  }
SizeT AllIxNewMultiNoneIndexed2DT::InitSeqAccess()
{
	seqIx = 0;
	return (*this)[0];
}
SizeT AllIxNewMultiNoneIndexed2DT::SeqAccess()
{
	return (*this)[++seqIx];
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
