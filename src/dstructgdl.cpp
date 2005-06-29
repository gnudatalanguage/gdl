/***************************************************************************
                          dstructgdl.cpp  -  GDL struct datatype
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

#include "includefirst.hpp"

#include "arrayindex.hpp"
#include "dstructgdl.hpp"
#include "accessdesc.hpp"
#include "objects.hpp"

using namespace std;

DStructGDL::~DStructGDL() 
{  
  SizeT nD=dd.size();
  for( SizeT i=0; i < nD; ++i) delete dd[i];
}

// new scalar, creating new descriptor
// intended for internal (C++) use to ease struct definition
DStructGDL::DStructGDL( const string& name_): SpDStruct( NULL), dd()
{ 
  if( name_[0] == '$') // unnamed struct 
    {
      cerr << "DStructGDL::DStructGDL( const string&) used"
	" for unnamed struct" << endl;
      throw exception(); 
      //      desc = new DStructDesc( name_);
      //      return;
    }
  
  desc = FindInStructList( structList, name_);
                
  if( desc == NULL)
    {
      desc = new DStructDesc( name_);
      structList.push_back( desc);
    }
  else
    { 
      SizeT nTags=NTags();

      dd.resize( nTags);
      
      for( SizeT i=0; i < nTags; i++)
	{
	  dd[i]=(*desc)[i]->GetInstance();
	}
    }
}
 
  // c-i 
DStructGDL::DStructGDL(const DStructGDL& d_): 
  SpDStruct(d_.desc, d_.dim), 
  dd(d_.dd.size()) 
{
  MakeOwnDesc();

  SizeT nEl=dd.size();
  for( SizeT i=0; i<nEl; i++)
    {
      dd[i]=d_.dd[i]->Dup();
    }
}

// // assignment. 
// DStructGDL& DStructGDL::operator=(const DStructGDL& right)
// {
//   //  std::cout << "DStructGDL& operator=(const DStructGDL& right): " << desc <<" "<< right.desc << std::endl;
  
//   if( &right == this) return *this; // self assignment
//   dim=right.dim;

// //   if( right.desc->IsUnnamed())
// //     SetDesc( new DStructDesc( right.desc));

//   // delete actual data
//   SizeT nD=dd.size();
//   for( SizeT i=0; i < nD; i++) delete dd[i];

//   // copy new data
//   SizeT nEl=right.N_Elements();
//   dd.resize(nEl);
//   for( SizeT i=0; i<nEl; i++)
//     {
//       dd[i]=right.dd[i]->Dup();
//     }
  
//   return *this;
// }


DStructGDL* DStructGDL::CShift( DLong d)
{
  DStructGDL* sh = new DStructGDL( Desc(), dim, BaseGDL::NOZERO);

  SizeT sz = dd.size();
  d *= NTags();
  if( d >= 0)
    for( SizeT i=0; i<sz; ++i) sh->dd[(i + d) % sz] = dd[i]->Dup();
  else
    for( SizeT i=0; i<sz; ++i) sh->dd[i] = dd[(i - d) % sz]->Dup();

  return sh;
}

DStructGDL* DStructGDL::CShift( DLong s[MAXRANK])
{
  DStructGDL* sh = new DStructGDL( Desc(), dim, BaseGDL::NOZERO);

  SizeT nDim = Rank();
  SizeT nEl = N_Elements();

  SizeT nTags = NTags();

  SizeT  stride[ MAXRANK + 1];
  dim.Stride( stride, nDim);

  long  srcIx[ MAXRANK+1];
  long  dstIx[ MAXRANK+1];
  SizeT dim_stride[ MAXRANK];
  for( SizeT aSp=0; aSp<nDim; ++aSp) 
    {
      srcIx[ aSp] = 0;
      if( s[ aSp] >= 0)
	dstIx[ aSp] = s[ aSp] % dim[ aSp];
      else
	dstIx[ aSp] = -(-s[aSp] % dim[ aSp]);
      if( dstIx[ aSp] < 0) dstIx[ aSp] += dim[ aSp];

      dim_stride[ aSp] = dim[ aSp] * stride[ aSp];
    }
  srcIx[ nDim] = dstIx[ nDim] = 0;

  SizeT dstLonIx = dstIx[ 0];
  for( SizeT rSp=1; rSp<nDim; ++rSp)
    dstLonIx += dstIx[ rSp] * stride[ rSp];

  BaseGDL** ddP = &dd[0];
  BaseGDL** shP = &sh->dd[0];
  
  for( SizeT a=0; a<nEl; ++srcIx[0],++dstIx[0], ++dstLonIx, ++a)
    {
      for( SizeT aSp=0; aSp<nDim;)
	{
	  if( dstIx[ aSp] >= dim[ aSp]) 
	    {
	      // dstIx[ aSp] -= dim[ aSp];
	      dstIx[ aSp] = 0;
	      dstLonIx -= dim_stride[ aSp];
	    }
	  if( srcIx[ aSp] < dim[ aSp]) break;

	  srcIx[ aSp] = 0;
	  ++srcIx[ ++aSp];
	  ++dstIx[ aSp];
	  dstLonIx += stride[ aSp];
	}

      for( SizeT t=0; t<nTags; ++t)
	shP[ dstLonIx * nTags + t] = ddP[ a * nTags + t]->Dup();
    }
  
  return sh;
}

// assigns srcIn to this at ixList, if ixList is NULL does linear copy
// assumes: ixList has this already set as variable
// used by DotAccessDescT::DoAssign
//         GDLInterpreter::l_array_expr
void DStructGDL::AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList, 
			SizeT offset)
{
  DStructGDL* src=static_cast<DStructGDL*>(srcIn);

  // check struct compatibility
  if( src->Desc() != this->Desc() && (*src->Desc()) != (*this->Desc()))
    throw 
      GDLException( "Conflicting data structures.");

  SizeT nTags=NTags();
  
  bool isScalar= src->N_Elements() == 1;
  if( isScalar) 
    { // src is scalar
      if( ixList == NULL)
	{
	  SizeT nCp=N_Elements();
	  
	  for( SizeT c=0; c<nCp; c++)
	    {
	      SizeT cTag=c*nTags;
	      for( SizeT tagIx=0; tagIx<nTags; tagIx++)
		{
		  delete dd[cTag+tagIx];
		  dd[ cTag+tagIx]=src->dd[tagIx]->Dup();
		}
	    }
	}
      else
	{
	  SizeT nCp=ixList->N_Elements();

	  AllIxT* allIx = ixList->BuildIx();
	  for( SizeT c=0; c<nCp; c++)
	    {
	      SizeT cTag=(*allIx)[ c]*nTags;
	      for( SizeT tagIx=0; tagIx<nTags; tagIx++)
		{
		  delete dd[cTag+tagIx];
		  dd[ cTag+tagIx]=src->dd[tagIx]->Dup();
		}
	    }
	}
    }
  else
    {
      SizeT srcElem=src->N_Elements();

      if( ixList == NULL)
	{
	  SizeT nCp=N_Elements();
	
	  // if (non-indexed) src is smaller -> just copy its number of elements
	  if( nCp > (srcElem-offset))
	    if( offset == 0)
	      nCp=srcElem;
	    else
	      throw GDLException("Source expr contains not enough elements.");

	  for( SizeT c=0; c<nCp; c++)
	    {
	      SizeT cTag= c*nTags;
	      SizeT srcTag= (c+offset)*nTags;
	      for( SizeT tagIx=0; tagIx<nTags; tagIx++)
		{
		  delete dd[cTag+tagIx];
		  dd[ cTag+tagIx]=src->dd[srcTag+tagIx]->Dup();
		}
	    }
 	}
      else
	{
	  SizeT nCp=ixList->N_Elements();
	
	  if( nCp == 1)
	    {
	      //	      InsAt( src, ixList->GetDim());
	      InsAt( src, ixList);
	    }
	  else
	    {
	      if( (srcElem-offset) < nCp)
		throw GDLException("Array subscript must have"
				   " same size as source expression.");

	      AllIxT* allIx = ixList->BuildIx();
	      for( SizeT c=0; c<nCp; c++)
		{
		  SizeT cTag= (*allIx)[ c]*nTags;
		  SizeT srcTag= (c+offset)*nTags;
		  for( SizeT tagIx=0; tagIx<nTags; tagIx++)
		    {
		      delete dd[cTag+tagIx];
		      dd[ cTag+tagIx]=src->dd[srcTag+tagIx]->Dup();
		    }
		}
	    }
 	}
    }
}

// used by AccessDescT for resolving, no checking is done
// inserts srcIn[ ixList] at offset
// used by DotAccessDescT::DoResolve
// there d is initially empty -> no deleting of old data in InsertAt
void DStructGDL::InsertAt( SizeT offset, BaseGDL* srcIn, 
			   ArrayIndexListT* ixList)
{
  DStructGDL* src=static_cast<DStructGDL* >(srcIn);

  SizeT nTags=NTags();
  
  if( ixList == NULL)
    {
      SizeT nCp=src->N_Elements();
      
      for( SizeT c=0; c<nCp; c++)
	{
	  SizeT cTag=(c+offset)*nTags;
	  SizeT srcTag=c*nTags;
	  for( SizeT tagIx=0; tagIx<nTags; tagIx++)
	    dd[ cTag+tagIx]=src->dd[srcTag+tagIx]->Dup();
	}
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; c++)
	{
	  SizeT cTag=(c+offset)*nTags;
	  SizeT srcTag=(*allIx)[ c]*nTags;
	  for( SizeT tagIx=0; tagIx<nTags; tagIx++)
	    dd[ cTag+tagIx]=src->dd[srcTag+tagIx]->Dup();
	}
    }
}


// used for array concatenation
DStructGDL* DStructGDL::CatArray( ExprListT& exprList,
				  const SizeT catRankIx, 
				  const SizeT rank)
{
  //  breakpoint();
  SizeT rankIx = RankIx( rank);
  SizeT maxIx = (catRankIx > rankIx)? catRankIx : rankIx;

  dimension     catArrDim(dim); // list contains at least one element

  catArrDim.MakeRank( maxIx+1);
  catArrDim.Set(catRankIx,0);     // clear rank which is added up

  SizeT dimSum=0;
  ExprListIterT i=exprList.begin();
  for(; i != exprList.end(); i++)
    {
      for( SizeT dIx=0; dIx<=maxIx; dIx++)
	{
	  if( dIx != catRankIx) 
	    {
	      if( catArrDim[dIx] == (*i)->Dim(dIx)) continue;
	      if( (catArrDim[dIx] > 1) || ((*i)->Dim(dIx) > 1)) 
                throw  GDLException("Unable to concatenate variables "
                                    "because the dimensions do not agree.");
	    }
	  else
	    {
	      SizeT add=(*i)->Dim(dIx);
	      dimSum+=(add)?add:1;
	    }
	}
    }
  
  catArrDim.Set(catRankIx,dimSum);
  
  // the concatenated array
  DStructGDL* catArr=New( catArrDim, BaseGDL::NOZERO);
  
  SizeT at=0;
  for( i=exprList.begin(); i != exprList.end(); i++)
    {
      // for STRUCT all types are compatible structs already (array_def in gdlc.i.g)
      //      if( (*i)->Type() != t) (*i)=(*i)->Convert2( t);
      catArr->CatInsert(static_cast<DStructGDL*>( (*i)),
			catRankIx,at); // advances 'at'
    }
  
  return catArr;
}

// returns (*this)[ ixList]
DStructGDL* DStructGDL::Index( ArrayIndexListT* ixList)
{
  //  ixList->SetVariable( this);
  
  DStructGDL* res=New( ixList->GetDim(), BaseGDL::NOZERO);
  
  SizeT nTags=NTags();
  
  SizeT nCp=ixList->N_Elements();
  AllIxT* allIx = ixList->BuildIx();
  for( SizeT c=0; c<nCp; c++)
    {
      SizeT cTag=c*nTags;
      SizeT srcTag=(*allIx)[ c]*nTags;
      for( SizeT tagIx=0; tagIx<nTags; tagIx++)
	res->dd[cTag+tagIx] = dd[ srcTag+tagIx]->Dup();
    }
  
  return res;
}

void DStructGDL::AddTag(BaseGDL* data)
{
  dd.resize(dd.size()+1, data->Dup());
}
void DStructGDL::AddTagGrab(BaseGDL* data)
{
  dd.resize(dd.size()+1, data);
}
void DStructGDL::NewTag(const string& tName, BaseGDL* data)
{
  Desc()->AddTag( tName, data); // makes a copy of data
  AddTagGrab( data);
}

void DStructGDL::AddParent( DStructDesc* p)
{
  SizeT oldSize=dd.size();
  SizeT nTags=p->NTags();
  dd.resize( oldSize+nTags);
  for( SizeT t=0; t < nTags; t++)
    {
    dd[oldSize+t]= (*p)[t]->GetInstance();
    }
}

// inserts srcIn at ixDim
// respects the exact structure
// used by Assign -> old data must be freed
void DStructGDL::InsAt( DStructGDL* srcIn, ArrayIndexListT* ixList)
{
  SizeT nDim; // max. number of dimensions to copy
  // ATTENTION: dimension is used as an index here
  dimension ixDim = ixList->GetDimIx0( nDim);
  nDim--;

  //const DStructGDL* srcArr=static_cast<const DStructGDL*>(srcIn->Convert2( t));
  dimension srcDim=srcIn->Dim();
    
  //  SizeT nDim   =RankIx(ixDim.Rank());
  SizeT srcNDim=RankIx(srcDim.Rank()); // number of source dimensions
  if( srcNDim < nDim) nDim=srcNDim;
    
  // check limits (up to Rank to consider)
  for( SizeT dIx=0; dIx <= nDim; dIx++)
    // check if in bounds of a
    if( (ixDim[dIx]+srcDim[dIx]) > dim[dIx])
      throw GDLException("Out of range subscript encountered.");

  SizeT len=srcDim[0]; // length of one segment to copy (one line of srcIn)
  
  SizeT nCp=srcIn->Stride(nDim+1)/len; // number of OVERALL copy actions

  // as lines are copied, we need the stride from 2nd dim on
  SizeT retStride[MAXRANK];
  for( SizeT a=0; a <= nDim; a++) retStride[a]=srcDim.Stride(a+1)/len;
    
  // a magic number, to reset destStart for this dimension
  SizeT resetStep[MAXRANK];
  for( SizeT a=1; a <= nDim; a++) 
    resetStep[a]=(retStride[a]-1)/retStride[a-1]*dim.Stride(a);
	
  SizeT destStart=dim.LongIndex(ixDim); // starting pos

  SizeT nTags=NTags();
    
  SizeT srcIx=0; // this one simply runs from 0 to N_Elements(srcIn)
  for( SizeT c=1; c<=nCp; c++) // linearized verison of nested loops
    {
      // copy one segment
      SizeT destEnd=destStart+len;
      for( SizeT destIx=destStart; destIx< destEnd; destIx++)
	{
	  SizeT destIxTag = destIx*nTags;
	  SizeT srcIxTag  = srcIx*nTags;
	  for( SizeT tagIx=0; tagIx<nTags; tagIx++)
	    {
	      delete dd[destIxTag+tagIx];
	      dd[destIxTag+tagIx] = srcIn->dd[ srcIxTag+tagIx]->Dup();
	    }
	  srcIx++;
	}

      // update destStart for all dimensions
      for( SizeT a=1; a<=nDim; a++)
	{
	  if( c % retStride[a])
	    {
	      // advance to next
	      destStart += dim.Stride(a);
	      break;
	    }
	  else
	    {
	      // reset
	      destStart -= resetStep[a];
	    }
	}
    }
}
  
// used for concatenation, called from CatArray
// assumes that everything is checked (see CatInfo)
void DStructGDL::CatInsert( const DStructGDL* srcArr, const SizeT atDim, SizeT& at)
{
  // length of one segment to copy
  SizeT len=srcArr->dim.Stride(atDim+1); // src array

  // number of copy actions
  SizeT nCp=srcArr->N_Elements()/len;

  // initial offset
  SizeT destStart= dim.Stride(atDim) * at; // dest array
  SizeT destEnd  = destStart + len;

  // number of elements to skip
  SizeT gap=dim.Stride(atDim+1);    // dest array

  SizeT nTags=NTags();

  SizeT srcIx=0;
  for( SizeT c=0; c<nCp; c++)
    {
      // copy one segment
      for( SizeT destIx=destStart; destIx< destEnd; destIx++)
	{
	  SizeT destIxTag = destIx*nTags;
	  SizeT srcIxTag  = srcIx*nTags;
	  for( SizeT tagIx=0; tagIx<nTags; tagIx++)
	    {
	      dd[destIxTag+tagIx] = srcArr->dd[ srcIxTag+tagIx]->Dup();
	    }
	  srcIx++;
	}	  

      // set new destination pointer
      destStart += gap;
      destEnd   += gap;
    }
      
  SizeT add=srcArr->dim[atDim]; // update 'at'
  at += (add > 1)? add : 1;
}

BaseGDL* DStructGDL::Get( SizeT tag)
{
  DotAccessDescT aD( 2); // like a.b (=2 levels)
  aD.Root( this);  // set root, implicit no index
  aD.Add( tag);    // tag to extract
  aD.AddIx( NULL); // no index -> ALL
  
  return aD.Resolve();
}

ostream& DStructGDL::Write( ostream& os, bool swapEndian)
{
  SizeT count = dd.size();
  for( SizeT i=0; i<count; i++)
    dd[i]->Write( os, swapEndian);
  return os;
}

istream& DStructGDL::Read( istream& os, bool swapEndian)
{
  SizeT count = dd.size();
  for( SizeT i=0; i<count; i++)
    dd[i]->Read( os, swapEndian);
  return os;
}
