/***************************************************************************
                          allix.hpp  -  indexer
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

#ifndef ALLIX_HPP_
#define ALLIX_HPP_

#include "typedefs.hpp"

class AllIxBaseT
{
public:
  virtual ~AllIxBaseT(); // code in arrayindex.cpp

  virtual AllIxBaseT* Clone() = 0;

  virtual SizeT operator[]( SizeT i) const = 0;
  virtual SizeT size() const = 0;
  // 	virtual SizeT max() const = 0;
  
  virtual SizeT InitSeqAccess() = 0;
  virtual SizeT SeqAccess() =0;

};
class AllIxT: public AllIxBaseT
{
protected:
  SizeT ix;

public:
  AllIxT() {}  	

  AllIxT( SizeT i): ix( i)
  {
  }
	
  ~AllIxT() {}

  AllIxT* Clone()
  {
    return new AllIxT( ix);
  }

  SizeT operator[]( SizeT i) const
  {
    assert( i == 0);
    return ix;
  }

  SizeT size() const { return 1;}

  SizeT InitSeqAccess() { return ix;}
  SizeT SeqAccess() { assert(false); return 0;}

//   SizeT max() const { return ix;}

  void Set( SizeT i) { ix = i;}
};
// class AllIxMultiT: public AllIxBaseT
// {
// private:
//   SizeT* ixArr;
//   SizeT  sz;
// 
// public:
//   AllIxMultiT( SizeT s): sz( s)
//   {
//     assert( s > 0);
//     ixArr = new SizeT[ s];
//   }
//   ~AllIxMultiT() { delete[] ixArr;}
// 
//   AllIxMultiT* Clone()
//   {
//     AllIxMultiT* clone = new AllIxMultiT( sz);
//     for( SizeT i=0; i<sz; ++i)
//       clone[ i] = ixArr[ i];
//     return clone;
//   }
// 
//   SizeT operator[]( SizeT i) const
//   {
//     assert( i < sz);
//     return ixArr[ i];
//   }
// 
//   SizeT size() const { return sz;}
//   SizeT max() const
//   {
//     SizeT m = ixArr[0];
//     for( SizeT i=1; i<sz; ++i)
//       if( ixArr[ i] > m)
// 	m = ixArr[ i] ;
//     return m;
//   }
// 
//   SizeT GetIx( SizeT i) const
//   {
//     assert( i < sz);
//     return ixArr[ i];
//   }
//   void SetIx( SizeT i, SizeT value)
//   {
//     assert( i < sz);
//     ixArr[ i] = value;
//   }
//   void SetIxTo0( SizeT i)
//   {
//     assert( i < sz);
//     ixArr[ i] = 0;
//   }
//   void AddToIx( SizeT i, SizeT value)
//   {
//     assert( i < sz);
//     ixArr[ i] += value;
//   }
// };

class AllIxRange0T: public AllIxBaseT
{
private:
  SizeT  sz;
  SizeT seqIx;

public:
  AllIxRange0T( SizeT sz_): sz( sz_)
  {
  }
  ~AllIxRange0T() {}
	
  AllIxRange0T* Clone()
  {
    AllIxRange0T* clone = new AllIxRange0T( sz);
    return clone;
  }
	
  SizeT operator[]( SizeT i) const
  {
    assert( i < sz);
    return i;
  }
  
  SizeT InitSeqAccess() { seqIx = 0; return 0;}
  SizeT SeqAccess() { assert( (seqIx+1) < sz); return ++seqIx;}

  SizeT size() const { return sz;}
  SizeT max() const
  {
    return sz - 1;
  }
};
class AllIxRangeT: public AllIxBaseT
{
private:
  SizeT  sz;
  SizeT  st;
  SizeT seqIx;
		
public:
  AllIxRangeT( SizeT sz_, SizeT st_): sz( sz_), st( st_)
  {
  }
  ~AllIxRangeT() {}
	
  AllIxRangeT* Clone()
  {
    AllIxRangeT* clone = new AllIxRangeT( sz, st);
    return clone;
  }
	
  SizeT operator[]( SizeT i) const
  {
    assert( i < sz);
    return i + st;
  }
  SizeT InitSeqAccess() { seqIx = st; return st;}
  SizeT SeqAccess() { assert( (seqIx-st+1) < sz); return ++seqIx;}

  SizeT size() const { return sz;}
  SizeT max() const
  {
    return sz - 1 + st;
  }
};
class AllIxRangeStrideT: public AllIxBaseT
{
private:
  SizeT  sz;
  SizeT  st;
  SizeT  stride;
  SizeT seqIx;
		
public:
  AllIxRangeStrideT( SizeT sz_, SizeT st_, SizeT stride_): sz( sz_), st( st_), stride( stride_)
  {
  }
  ~AllIxRangeStrideT() {}
	
  AllIxRangeStrideT* Clone()
  {
    AllIxRangeStrideT* clone = new AllIxRangeStrideT( sz, st, stride);
    return clone;
  }
	
  SizeT operator[]( SizeT i) const
  {
    assert( i < sz);
    return i * stride + st;
  }
  SizeT InitSeqAccess() { seqIx = st; return st;}
  SizeT SeqAccess() { assert( ((seqIx+stride-st)/stride) < sz); seqIx += stride; return seqIx;}

  SizeT size() const { return sz;}
  SizeT max() const
  {
    return (sz - 1) * stride + st;
  }
};
class AllIxRange0StrideT: public AllIxBaseT
{
private:
  SizeT  sz;
  SizeT  stride;
  SizeT seqIx;
		
public:
  AllIxRange0StrideT( SizeT sz_, SizeT stride_): sz( sz_), stride( stride_)
  {
  }
  ~AllIxRange0StrideT() {}
	
  AllIxRange0StrideT* Clone()
  {
    AllIxRange0StrideT* clone = new AllIxRange0StrideT( sz, stride);
    return clone;
  }
	
  SizeT operator[]( SizeT i) const
  {
    assert( i < sz);
    return i * stride;
  }
  SizeT InitSeqAccess() { seqIx = 0; return 0;}
  SizeT SeqAccess() { assert( ((seqIx+stride)/stride) < sz); seqIx += stride; return seqIx;}

  SizeT size() const { return sz;}
  SizeT max() const
  {
    return (sz - 1) * stride;
  }
};
class BaseGDL;
class AllIxIndicesT: public AllIxBaseT
{
protected:
  BaseGDL* ref;
  SizeT         upper;
  SizeT         seqIx;
#ifndef NDEBUG
  bool upperSet;
#endif  

public:
  AllIxIndicesT( BaseGDL* r): ref( r), upper( 0)
  {
#ifndef NDEBUG
  upperSet = false;
#endif  
  }
  ~AllIxIndicesT() {}

  AllIxIndicesT* Clone()
  {
    AllIxIndicesT* clone = new AllIxIndicesT( ref);
    return clone;
  }
  virtual AllIxIndicesT* CloneAt( char* buf)
  {
    AllIxIndicesT* clone = new (buf) AllIxIndicesT( ref);
    return clone;
  }

  SizeT operator[]( SizeT i) const; // code in arrayindex.cpp

  SizeT InitSeqAccess();
  SizeT SeqAccess(); // code in arrayindex.cpp
  
  SizeT size() const;
  void SetUpper( SizeT u)
  {
  upper = u;
#ifndef NDEBUG
  upperSet = true;
#endif  
  }
};
class AllIxIndicesStrictT: public AllIxIndicesT
{
public:
  AllIxIndicesStrictT( BaseGDL* r): AllIxIndicesT( r)
  {
  }
	 
  AllIxIndicesStrictT* Clone()
  {
    AllIxIndicesStrictT* clone = new AllIxIndicesStrictT( ref);
    return clone;
  }
  AllIxIndicesStrictT* CloneAt( char* buf)
  {
    AllIxIndicesStrictT* clone = new (buf) AllIxIndicesStrictT( ref);
    return clone;
  }

  SizeT operator[]( SizeT i) const; // code in arrayindex.cpp
  SizeT InitSeqAccess();
  SizeT SeqAccess(); // code in arrayindex.cpp
};



class AllIxAllIndexedT: public AllIxBaseT
{
private:
  ArrayIndexVectorT* ixList;
  const SizeT* varStride;
  SizeT acRank;
  SizeT nIx;
  SizeT         seqIx;
	
public:
  AllIxAllIndexedT( ArrayIndexVectorT* ixList_, SizeT acRank_, SizeT nIx_, const SizeT* varStride_)
    : ixList( ixList_)
    , acRank( acRank_)
    , nIx( nIx_)
    , varStride( varStride_)
  {}
  ~AllIxAllIndexedT() {}

  AllIxAllIndexedT* Clone()
  {
    AllIxAllIndexedT* clone = new AllIxAllIndexedT( ixList, acRank, nIx, varStride);
    return clone;
  }

  SizeT operator[]( SizeT i) const;
  SizeT InitSeqAccess();
  SizeT SeqAccess();
	
  SizeT size() const { return nIx;}	
};



class AllIxNewMultiT: public AllIxBaseT
{
private:
  ArrayIndexVectorT* ixList;
  SizeT ixListStride[MAXRANK];
  const SizeT* varStride;
  SizeT* nIterLimit;
  SizeT* stride;
  SizeT acRank;
  SizeT nIx;
  SizeT seqIx;
  SizeT add;

public:
  AllIxNewMultiT( ArrayIndexVectorT* ixList_, SizeT acRank_, SizeT nIx_, const SizeT* varStride_, SizeT* nIterLimit_, SizeT* stride_)
    : ixList( ixList_)
    , acRank( acRank_)
    , nIx( nIx_)
    , varStride( varStride_)
    , nIterLimit( nIterLimit_)
    , stride( stride_)
  {
    add = 0;
	assert( varStride[0] == 1);
	for( SizeT i=0; i<acRank;++i)
		{
		if( !(*ixList)[i]->Indexed())
			{
				ixListStride[i] = (*ixList)[i]->GetStride() * varStride[i];
				assert( ixListStride[i] >= 1);
				add += (*ixList)[i]->GetS() * varStride[i];
			}
		}
  }
  ~AllIxNewMultiT() {}

  AllIxNewMultiT* Clone()
  {
    AllIxNewMultiT* clone = new AllIxNewMultiT( *this);
    return clone;
  }

  SizeT operator[]( SizeT i) const;
  SizeT InitSeqAccess();
  SizeT SeqAccess(); 
	
  SizeT size() const { return nIx;}	
};



// acRank == 2
class AllIxNewMulti2DT: public AllIxBaseT
{
private:
  ArrayIndexVectorT* ixList;
  SizeT ixListStride[2];
  const SizeT* varStride;
  SizeT* nIterLimit;
  SizeT* stride;
  SizeT nIx;
  SizeT seqIx;
//   SizeT s[MAXRANK];
  SizeT add;

public:
  AllIxNewMulti2DT( ArrayIndexVectorT* ixList_, SizeT nIx_, const SizeT* varStride_, SizeT* nIterLimit_, SizeT* stride_)
    : ixList( ixList_)
    , nIx( nIx_)
    , varStride( varStride_)
    , nIterLimit( nIterLimit_)
    , stride( stride_)
  {
    add = 0;
	if( !(*ixList)[0]->Indexed())
	{
		assert( varStride[0] == 1);
		ixListStride[0] = (*ixList)[0]->GetStride();
		assert( ixListStride[0] >= 1);
		add += (*ixList)[0]->GetS();
	}
	if( !(*ixList)[1]->Indexed())
		{
			ixListStride[1] = (*ixList)[1]->GetStride() * varStride[1];
			assert( ixListStride[1] >= 1);
			add += (*ixList)[1]->GetS() * varStride[1];
		}
  }
  ~AllIxNewMulti2DT() {}

  AllIxNewMulti2DT* Clone()
  {
    AllIxNewMulti2DT* clone = new AllIxNewMulti2DT( *this);
    return clone;
  }

  SizeT operator[]( SizeT i) const;
  SizeT InitSeqAccess();
  SizeT SeqAccess(); 
	
  SizeT size() const { return nIx;}	
};




class AllIxNewMultiNoneIndexedT: public AllIxBaseT
{
private:
  ArrayIndexVectorT* ixList;
  SizeT ixListStride[MAXRANK];
  const SizeT* varStride;
  SizeT* nIterLimit;
  SizeT* stride;
  SizeT acRank;
  SizeT nIx;
  SizeT seqIx;
  SizeT add;
  SizeT seqIter;
  SizeT correctionIncrease;
  SizeT nextCorrection;

//   SizeT seqIxDebug;

public:
  AllIxNewMultiNoneIndexedT( ArrayIndexVectorT* ixList_, SizeT acRank_, SizeT nIx_, const SizeT* varStride_, SizeT* nIterLimit_, SizeT* stride_)
    : ixList( ixList_)
    , acRank( acRank_)
    , nIx( nIx_)
    , varStride( varStride_)
    , nIterLimit( nIterLimit_)
    , stride( stride_)
  {
	assert( varStride[0] == 1);
    add = 0;
	for( SizeT i=0; i<acRank;++i)
		{
		ixListStride[i] = (*ixList)[i]->GetStride() * varStride[i];
		assert( ixListStride[i] >= 1);
		add += (*ixList)[i]->GetS() * varStride[i];
		}
  }
  ~AllIxNewMultiNoneIndexedT() {}

  AllIxNewMultiNoneIndexedT* Clone()
  {
    AllIxNewMultiNoneIndexedT* clone = new AllIxNewMultiNoneIndexedT( *this);
    return clone;
  }

  SizeT operator[]( SizeT i) const;
  SizeT InitSeqAccess();
  SizeT SeqAccess();
	
  SizeT size() const { return nIx;}	
};
		

// acRank == 2
class AllIxNewMultiNoneIndexed2DT: public AllIxBaseT
{
private:
  ArrayIndexVectorT* ixList;
  SizeT ixListStride[2];
  const SizeT* varStride;
  SizeT* nIterLimit;
  SizeT* stride;
  SizeT nIx;
  SizeT seqIx;
  SizeT add;
  SizeT nextCorrection;
  SizeT correctionIncrease;
  
public:
  AllIxNewMultiNoneIndexed2DT( ArrayIndexVectorT* ixList_, SizeT nIx_, const SizeT* varStride_, SizeT* nIterLimit_, SizeT* stride_)
    : ixList( ixList_)
    , nIx( nIx_)
    , varStride( varStride_)
    , nIterLimit( nIterLimit_)
    , stride( stride_)
  {
		assert( varStride[0] == 1);
		ixListStride[0] = (*ixList)[0]->GetStride();
		ixListStride[1] = (*ixList)[1]->GetStride() * varStride[1];
		add = (*ixList)[0]->GetS() + (*ixList)[1]->GetS() * varStride[1];
  }
  ~AllIxNewMultiNoneIndexed2DT() {}

  AllIxNewMultiNoneIndexed2DT* Clone()
  {
    AllIxNewMultiNoneIndexed2DT* clone = new AllIxNewMultiNoneIndexed2DT( *this);
    return clone;
  }

  SizeT operator[]( SizeT i) const;
  SizeT InitSeqAccess();
  SizeT SeqAccess();
	
  SizeT size() const { return nIx;}	
};
		

		
class AllIxNewMultiOneVariableIndexNoIndexT: public AllIxBaseT
{
private:
  ArrayIndexVectorT* ixList;
  SizeT ixListStride;
  const SizeT* varStride;
  SizeT* nIterLimit;
  SizeT* stride;
  SizeT acRank;
  SizeT nIx;
  SizeT seqIx;
  SizeT add;
  RankT variableIndex;
	
public:
  AllIxNewMultiOneVariableIndexNoIndexT( RankT gt1Rank, SizeT add_,
								  ArrayIndexVectorT* ixList_, SizeT acRank_, SizeT nIx_, const SizeT* varStride_, SizeT* nIterLimit_, SizeT* stride_)
	: ixList( ixList_)
    , acRank( acRank_)
    , nIx( nIx_)
    , varStride( varStride_)
    , nIterLimit( nIterLimit_)
    , stride( stride_)
	, add( add_)
	, variableIndex( gt1Rank)
	{
	assert( varStride[0] == 1);
	ixListStride = (*ixList)[variableIndex]->GetStride() * varStride[variableIndex];
	assert( ixListStride >= 1);
	}
  ~AllIxNewMultiOneVariableIndexNoIndexT() {}

  AllIxNewMultiOneVariableIndexNoIndexT* Clone()
  {
    AllIxNewMultiOneVariableIndexNoIndexT* clone = new AllIxNewMultiOneVariableIndexNoIndexT( *this);
    return clone;
  }

  SizeT operator[]( SizeT i) const;
  SizeT InitSeqAccess();
  SizeT SeqAccess();
	
  SizeT size() const { return nIx;}	
};



class AllIxNewMultiOneVariableIndexIndexedT: public AllIxBaseT
{
private:
  ArrayIndexVectorT* ixList;
  SizeT ixListStride;
  const SizeT* varStride;
  SizeT* nIterLimit;
  SizeT* stride;
  SizeT acRank;
  SizeT nIx;
  SizeT seqIx;
  SizeT add;
  RankT variableIndex;
  ArrayIndexT* arrayIndexIndexed;
		  
public:
  AllIxNewMultiOneVariableIndexIndexedT( RankT gt1Rank, SizeT add_,
								  ArrayIndexVectorT* ixList_, SizeT acRank_, SizeT nIx_, const SizeT* varStride_, SizeT* nIterLimit_, SizeT* stride_)
	: ixList( ixList_)
    , acRank( acRank_)
    , nIx( nIx_)
    , varStride( varStride_)
    , nIterLimit( nIterLimit_)
    , stride( stride_)
	, add( add_)
	, variableIndex( gt1Rank)
	{
		arrayIndexIndexed = (*ixList)[variableIndex];
		ixListStride = varStride[variableIndex];
		assert( ixListStride >= 1);
	}
  ~AllIxNewMultiOneVariableIndexIndexedT() {}

  AllIxNewMultiOneVariableIndexIndexedT* Clone()
  {
    AllIxNewMultiOneVariableIndexIndexedT* clone = new AllIxNewMultiOneVariableIndexIndexedT( *this);
    return clone;
  }

  SizeT operator[]( SizeT i) const;
  SizeT InitSeqAccess();
  SizeT SeqAccess();
	
  SizeT size() const { return nIx;}	
};



namespace AllIxMaxSizeCalculation
{
  static const int a = sizeof( AllIxRangeT);
  static const int b = sizeof( AllIxRange0T);
  static const int c = sizeof( AllIxRangeStrideT);
  static const int d = sizeof( AllIxRange0StrideT);
  static const int e = sizeof( AllIxT);
  static const int f = sizeof( AllIxNewMultiOneVariableIndexIndexedT);
  static const int g = sizeof( AllIxNewMultiOneVariableIndexNoIndexT);
  static const int h = sizeof( AllIxNewMulti2DT);
  static const int i = sizeof( AllIxNewMultiT);
  static const int j = sizeof( AllIxNewMultiNoneIndexedT);
  static const int k = sizeof( AllIxNewMultiNoneIndexed2DT);
  static const int ab = a > b ? a : b;
  static const int abc = ab > c ? ab : c;
  static const int abcd = abc > d ? abc : d;
  static const int abcde = abcd > e ? abcd : e;
  static const int abcdef = abcde > f ? abcde : f;
  static const int abcdefg = abcdef > g ? abcdef : g;
  static const int abcdefgh = abcdefg > h ? abcdefg : h;
  static const int abcdefghi = abcdefgh > i ? abcdefgh : i;
  static const int abcdefghij = abcdefghi > j ? abcdefghi : j;
  static const int abcdefghijk = abcdefghij > k ? abcdefghij : k;
  static const int Max = abcdefghijk;
}

static const int AllIxMaxSize = AllIxMaxSizeCalculation::Max;

#endif
