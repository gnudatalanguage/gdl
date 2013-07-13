/**************************************************************************
datatypesref.cpp  -  specializations for DPtrGDL and DObjGDL for reference counting
                             -------------------
    begin                : March 08 2010
    copyright            : (C) 2010 by Marc Schellens
    email                : m_schellens@users.sf.net
***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// to be included from datatypes.cpp
#ifdef INCLUDE_DATATYPESREF_CPP
#undef INCLUDE_DATATYPESREF_CPP

// reference counting for INIT
template<>
Data_<SpDPtr>* Data_<SpDPtr>::New( const dimension& dim_, BaseGDL::InitType noZero) const
{
  if( noZero == BaseGDL::NOZERO) return new Data_(dim_, BaseGDL::NOZERO);
  if( noZero == BaseGDL::INIT)
    {
      Data_* res =  new Data_(dim_, BaseGDL::NOZERO);
      SizeT nEl = res->dd.size();
      /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
	#pragma omp for*/
      for( OMPInt i=0; i<nEl; ++i) (*res)[ i] = (*this)[ 0]; // set all to scalar
      //}
      GDLInterpreter::AddRef((*this)[ 0], nEl);
      
      return res;
    }
  return new Data_(dim_); // zero data
}
// reference counting for INIT
template<>
Data_<SpDObj>* Data_<SpDObj>::New( const dimension& dim_, BaseGDL::InitType noZero) const
{
  if( noZero == BaseGDL::NOZERO) return new Data_(dim_, BaseGDL::NOZERO);
  if( noZero == BaseGDL::INIT)
    {
      Data_* res =  new Data_(dim_, BaseGDL::NOZERO);
      SizeT nEl = res->dd.size();
      /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
	#pragma omp for*/
      for( OMPInt i=0; i<nEl; ++i) (*res)[ i] = (*this)[ 0]; // set all to scalar
      //}
      GDLInterpreter::AddRefObj((*this)[ 0], nEl);
      
      return res;
    }
  return new Data_(dim_); // zero data
}

template<>
void Data_<SpDPtr>::InsAt( Data_* srcIn, ArrayIndexListT* ixList, SizeT offset)
{
  // max. number of dimensions to copy
  SizeT nDim = ixList->NDim();

  if( nDim == 1)
    {
      SizeT destStart = ixList->LongIx();

      //SizeT len;
      if( this->N_Elements() == 1)
	{
	  //	  len = 1;
	  SizeT rStride = srcIn->Stride(this->Rank());
	  Ty& a = (*this)[ destStart];
	  Ty b = (*srcIn)[ offset/rStride];
	  GDLInterpreter::IncRef( b);
	  GDLInterpreter::DecRef( a);
	  a = b;//(*this)[ destStart] = (*srcIn)[ offset/rStride];
	}
      else
	{
	  SizeT len = srcIn->Dim( 0); // length of segment to copy
	  // TODO: IDL reports here (and probably in the insert-dimension case below as well)
	  //       the name of a variable, e.g.:
	  //       IDL> a=[0,0,0] & a[2]=[2,2,2]
	  //       % Out of range subscript encountered: A.
	  if( (destStart+len) > this->N_Elements()) //dim[0])
	    throw GDLException("Out of range subscript encountered (length of insert exceeds array boundaries).");

	  // DataT& srcIn_dd = srcIn->dd;
	  SizeT srcIx = 0; // this one simply runs from 0 to N_Elements(srcIn)

	  SizeT destEnd = destStart + len;
	  for( SizeT destIx = destStart; destIx < destEnd; ++destIx)
	    {
	      Ty& a = (*this)[ destIx];
	      Ty b = (*srcIn)[ srcIx++];
	      GDLInterpreter::IncRef( b);
	      GDLInterpreter::DecRef( a);

	      a = b;//	(*this)[ destIx] = (*srcIn)[ srcIx++];
	    }
	}

      return;
    }

  SizeT destStart; // 1-dim starting index
  // ATTENTION: dimension is used as an index here
  dimension ixDim = ixList->GetDimIx0( destStart);
  nDim--;

  dimension srcDim=srcIn->Dim();
  SizeT len=srcDim[0]; // length of one segment to copy (one line of srcIn)

  //  SizeT nDim   =RankIx(ixDim.Rank());
  SizeT srcNDim=RankIx(srcDim.Rank()); // number of source dimensions
  if( srcNDim < nDim) nDim=srcNDim;

  // check limits (up to Rank to consider)
  for( SizeT dIx=0; dIx <= nDim; ++dIx)
    // check if in bounds of a
    if( (ixDim[dIx]+srcDim[dIx]) > this->dim[dIx])
      throw GDLException("Out of range subscript encountered (dimension of insert exceeds array boundaries for dimension " + i2s(dIx +1) + ").");

  SizeT nCp=srcIn->Stride(nDim+1)/len; // number of OVERALL copy actions

  // as lines are copied, we need the stride from 2nd dim on
  SizeT retStride[MAXRANK];
  for( SizeT a=0; a <= nDim; ++a) retStride[a]=srcDim.Stride(a+1)/len;
	
  // a magic number, to reset destStart for this dimension
  SizeT resetStep[MAXRANK];
  for( SizeT a=1; a <= nDim; ++a)
    resetStep[a]=(retStride[a]-1)/retStride[a-1]*this->dim.Stride(a);
	
  //  SizeT destStart=this->dim.LongIndex(ixDim); // starting pos

  // DataT& srcIn_dd = srcIn->dd;

  SizeT srcIx=0; // this one simply runs from 0 to N_Elements(srcIn)
  for( SizeT c=1; c<=nCp; ++c) // linearized verison of nested loops
    {
      // copy one segment
      SizeT destEnd=destStart+len;
      for( SizeT destIx=destStart; destIx<destEnd; ++destIx)
	{
	  Ty& a = (*this)[ destIx];
	  Ty b = (*srcIn)[ srcIx++];
	  GDLInterpreter::IncRef( b);
	  GDLInterpreter::DecRef( a);
	  a = b;//(*this)[destIx] = (*srcIn)[ srcIx++];
	}

      // update destStart for all dimensions
      if( c < nCp)
	for( SizeT a=1; a<=nDim; ++a)
	  {
	    if( c % retStride[a])
	      {
		// advance to next
		destStart += this->dim.Stride(a);
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

template<>
void Data_<SpDObj>::InsAt( Data_* srcIn, ArrayIndexListT* ixList, SizeT offset)
{
  // max. number of dimensions to copy
  SizeT nDim = ixList->NDim();

  if( nDim == 1)
    {
      SizeT destStart = ixList->LongIx();

      //SizeT len;
      if( this->N_Elements() == 1)
	{
	  //	  len = 1;
	  SizeT rStride = srcIn->Stride(this->Rank());
	  Ty& a = (*this)[ destStart];
	  Ty b = (*srcIn)[ offset/rStride];
	  GDLInterpreter::IncRefObj( b);
	  GDLInterpreter::DecRefObj( a);
	  a = b;//(*this)[ destStart] = (*srcIn)[ offset/rStride];
	}
      else
	{
	  SizeT len = srcIn->Dim( 0); // length of segment to copy
	  // TODO: IDL reports here (and probably in the insert-dimension case below as well)
	  //       the name of a variable, e.g.:
	  //       IDL> a=[0,0,0] & a[2]=[2,2,2]
	  //       % Out of range subscript encountered: A.
	  if( (destStart+len) > this->N_Elements()) //dim[0])
	    throw GDLException("Out of range subscript encountered (length of insert exceeds array boundaries).");

	  // DataT& srcIn_dd = srcIn->dd;
	  SizeT srcIx = 0; // this one simply runs from 0 to N_Elements(srcIn)

	  SizeT destEnd = destStart + len;
	  for( SizeT destIx = destStart; destIx < destEnd; ++destIx)
	    {
	      Ty& a = (*this)[ destIx];
	      Ty b = (*srcIn)[ srcIx++];
	      GDLInterpreter::IncRefObj( b);
	      GDLInterpreter::DecRefObj( a);

	      a = b;//	(*this)[ destIx] = (*srcIn)[ srcIx++];
	    }
	}

      return;
    }

  SizeT destStart; // 1-dim starting index
  // ATTENTION: dimension is used as an index here
  dimension ixDim = ixList->GetDimIx0( destStart);
  nDim--;

  dimension srcDim=srcIn->Dim();
  SizeT len=srcDim[0]; // length of one segment to copy (one line of srcIn)

  //  SizeT nDim   =RankIx(ixDim.Rank());
  SizeT srcNDim=RankIx(srcDim.Rank()); // number of source dimensions
  if( srcNDim < nDim) nDim=srcNDim;

  // check limits (up to Rank to consider)
  for( SizeT dIx=0; dIx <= nDim; ++dIx)
    // check if in bounds of a
    if( (ixDim[dIx]+srcDim[dIx]) > this->dim[dIx])
      throw GDLException("Out of range subscript encountered (dimension of insert exceeds array boundaries for dimension " + i2s(dIx +1) + ").");

  SizeT nCp=srcIn->Stride(nDim+1)/len; // number of OVERALL copy actions

  // as lines are copied, we need the stride from 2nd dim on
  SizeT retStride[MAXRANK];
  for( SizeT a=0; a <= nDim; ++a) retStride[a]=srcDim.Stride(a+1)/len;
	
  // a magic number, to reset destStart for this dimension
  SizeT resetStep[MAXRANK];
  for( SizeT a=1; a <= nDim; ++a)
    resetStep[a]=(retStride[a]-1)/retStride[a-1]*this->dim.Stride(a);
	
  //  SizeT destStart=this->dim.LongIndex(ixDim); // starting pos

  // DataT& srcIn_dd = srcIn->dd;

  SizeT srcIx=0; // this one simply runs from 0 to N_Elements(srcIn)
  for( SizeT c=1; c<=nCp; ++c) // linearized verison of nested loops
    {
      // copy one segment
      SizeT destEnd=destStart+len;
      for( SizeT destIx=destStart; destIx<destEnd; ++destIx)
	{
	  Ty& a = (*this)[ destIx];
	  Ty b = (*srcIn)[ srcIx++];
	  GDLInterpreter::IncRefObj( b);
	  GDLInterpreter::DecRefObj( a);
	  a = b;//(*this)[destIx] = (*srcIn)[ srcIx++];
	}

      // update destStart for all dimensions
      if( c < nCp)
	for( SizeT a=1; a<=nDim; ++a)
	  {
	    if( c % retStride[a])
	      {
		// advance to next
		destStart += this->dim.Stride(a);
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



template<>
void Data_<SpDPtr>::AssignAtIx( RangeT ix, BaseGDL* srcIn)
{
  if( srcIn->Type() != this->Type())
    throw GDLException("Only expressions of type " + srcIn->TypeStr() + " can be assigned to " + this->TypeStr());
  GDLInterpreter::IncRef( (*static_cast<Data_*>(srcIn))[0]);
  GDLInterpreter::DecRef( (*this)[ix]);
  (*this)[ix] = (*static_cast<Data_*>(srcIn))[0];
}
template<>
void Data_<SpDObj>::AssignAtIx( RangeT ix, BaseGDL* srcIn)
{
  if( srcIn->Type() != this->Type())
    throw GDLException("Only expressions of type " + srcIn->TypeStr() + " can be assigned to " + this->TypeStr());
  GDLInterpreter::IncRefObj( (*static_cast<Data_*>(srcIn))[0]);
  GDLInterpreter::DecRefObj( (*this)[ix]);
  (*this)[ix] = (*static_cast<Data_*>(srcIn))[0];
}










template<>
void Data_<SpDPtr>::AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList, SizeT offset)
{
  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);  

  SizeT srcElem= src->N_Elements();
  //  bool  isScalar= (srcElem == 1);
  bool  isScalar= (srcElem == 1) && (src->Rank() == 0);
  if( isScalar) 
    { // src is scalar
      Ty scalar=(*src)[0];
      
      if( ixList == NULL)
	{
	  SizeT nCp=Data_::N_Elements();

	  GDLInterpreter::AddRef( scalar, nCp);

	  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	  {
	    //#pragma omp for
	    for( SizeT c=0; c<nCp; ++c)
	      {
		GDLInterpreter::DecRef( (*this)[ c]);
		(*this)[ c]=scalar;
	      }
	  }
	}
      else
	{
	  SizeT nCp=ixList->N_Elements();
	  
	  AllIxBaseT* allIx = ixList->BuildIx();
	  
	  GDLInterpreter::AddRef( scalar, nCp);

	  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	  {
	    //#pragma omp for
	    for( SizeT c=0; c<nCp; ++c)
	      {
		SizeT ix = (*allIx)[ c];
		GDLInterpreter::DecRef( (*this)[ ix]);
		(*this)[ ix]=scalar;
	      }
	  }	  //	    (*this)[ ixList->GetIx( c)]=scalar;
	}
    }
  else
    {
      if( ixList == NULL)
	{
	  SizeT nCp=Data_::N_Elements();
	
	  // if (non-indexed) src is smaller -> just copy its number of elements
	  if( nCp > (srcElem-offset))
	    if( offset == 0)
	      nCp=srcElem;
	    else
	      throw GDLException("Source expression contains not enough elements.");

	  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	  {
	    //#pragma omp for
	    for( SizeT c=0; c<nCp; ++c)
	      {
		Ty& a = (*this)[ c];
		Ty b = (*src)[c+offset];
		GDLInterpreter::IncRef( b);
		GDLInterpreter::DecRef( a);
		a = b;//(*this)[ c]=(*src)[c+offset];
	      }
	  }
	}
      else
	{
 	  // crucial part
	  SizeT nCp=ixList->N_Elements();

	  if( nCp == 1)
	    {
	      SizeT destStart = ixList->LongIx();
	      //  len = 1;
	      SizeT rStride = srcIn->Stride(this->Rank());
	      (*this)[ destStart] = (*src)[ offset/rStride];

	      //	      InsAt( src, ixList, offset);
	    }
	  else
	    {
	      if( offset == 0)
		{
		  if( srcElem < nCp)
		    throw GDLException("Array subscript must have same size as"
				       " source expression.");
		  
		  AllIxBaseT* allIx = ixList->BuildIx();
		  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
		  {
		    //#pragma omp for
		    for( SizeT c=0; c<nCp; ++c)
		      {
			Ty& a =  (*this)[ (*allIx)[ c]];
			Ty b = (*src)[c];
			GDLInterpreter::IncRef( b);
			GDLInterpreter::DecRef( a);
			a = b;//		    (*this)[ (*allIx)[ c]]=(*src)[c];
		      }
		  }		  //		(*this)[ ixList->GetIx( c)]=(*src)[c+offset];
		}
	      else
		{
		  if( (srcElem-offset) < nCp)
		    throw GDLException("Array subscript must have same size as"
				       " source expression.");
		  
		  AllIxBaseT* allIx = ixList->BuildIx();
		  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
		  {
		    //#pragma omp for
		    for( SizeT c=0; c<nCp; ++c)
		      {
			Ty& a =  (*this)[ (*allIx)[ c]];
			Ty b = (*src)[c+offset];
			GDLInterpreter::IncRef( b);
			GDLInterpreter::DecRef( a);
			a = b;//		    (*this)[ (*allIx)[ c]]=(*src)[c+offset];
		      }
		  }		  //		(*this)[ ixList->GetIx( c)]=(*src)[c+offset];
		}
	    }
	}
    }
}

template<>
void Data_<SpDObj>::AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList, SizeT offset)
{
  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);  

  SizeT srcElem= src->N_Elements();
  //  bool  isScalar= (srcElem == 1);
  bool  isScalar= (srcElem == 1) && (src->Rank() == 0);
  if( isScalar) 
    { // src is scalar
      Ty scalar=(*src)[0];
      
      if( ixList == NULL)
	{
	  SizeT nCp=Data_::N_Elements();

	  GDLInterpreter::AddRefObj( scalar, nCp);

	  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	  {
	    //#pragma omp for
	    for( SizeT c=0; c<nCp; ++c)
	      {
		GDLInterpreter::DecRefObj( (*this)[ c]);
		(*this)[ c]=scalar;
	      }
	  }
	}
      else
	{
	  SizeT nCp=ixList->N_Elements();
	  
	  AllIxBaseT* allIx = ixList->BuildIx();
	  
	  GDLInterpreter::AddRefObj( scalar, nCp);

	  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	  {
	    //#pragma omp for
	    for( SizeT c=0; c<nCp; ++c)
	      {
		SizeT ix = (*allIx)[ c];
		GDLInterpreter::DecRefObj( (*this)[ ix]);
		(*this)[ ix]=scalar;
	      }
	  }	  //	    (*this)[ ixList->GetIx( c)]=scalar;
	}
    }
  else
    {
      if( ixList == NULL)
	{
	  SizeT nCp=Data_::N_Elements();
	
	  // if (non-indexed) src is smaller -> just copy its number of elements
	  if( nCp > (srcElem-offset))
	    if( offset == 0)
	      nCp=srcElem;
	    else
	      throw GDLException("Source expression contains not enough elements.");

	  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	  {
	    //#pragma omp for
	    for( SizeT c=0; c<nCp; ++c)
	      {
		Ty& a = (*this)[ c];
		Ty b = (*src)[c+offset];
		GDLInterpreter::IncRefObj( b);
		GDLInterpreter::DecRefObj( a);
		a = b;//(*this)[ c]=(*src)[c+offset];
	      }
	  }
	}
      else
	{
 	  // crucial part
	  SizeT nCp=ixList->N_Elements();

	  if( nCp == 1)
	    {
	      SizeT destStart = ixList->LongIx();
	      //  len = 1;
	      SizeT rStride = srcIn->Stride(this->Rank());
	      (*this)[ destStart] = (*src)[ offset/rStride];

	      //	      InsAt( src, ixList, offset);
	    }
	  else
	    {
	      if( offset == 0)
		{
		  if( srcElem < nCp)
		    throw GDLException("Array subscript must have same size as"
				       " source expression.");
		  
		  AllIxBaseT* allIx = ixList->BuildIx();
		  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
		  {
		    //#pragma omp for
		    for( SizeT c=0; c<nCp; ++c)
		      {
			Ty& a =  (*this)[ (*allIx)[ c]];
			Ty b = (*src)[c];
			GDLInterpreter::IncRefObj( b);
			GDLInterpreter::DecRefObj( a);
			a = b;//		    (*this)[ (*allIx)[ c]]=(*src)[c];
		      }
		  }		  //		(*this)[ ixList->GetIx( c)]=(*src)[c+offset];
		}
	      else
		{
		  if( (srcElem-offset) < nCp)
		    throw GDLException("Array subscript must have same size as"
				       " source expression.");
		  
		  AllIxBaseT* allIx = ixList->BuildIx();
		  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
		  {
		    //#pragma omp for
		    for( SizeT c=0; c<nCp; ++c)
		      {
			Ty& a =  (*this)[ (*allIx)[ c]];
			Ty b = (*src)[c+offset];
			GDLInterpreter::IncRefObj( b);
			GDLInterpreter::DecRefObj( a);
			a = b;//		    (*this)[ (*allIx)[ c]]=(*src)[c+offset];
		      }
		  }		  //		(*this)[ ixList->GetIx( c)]=(*src)[c+offset];
		}
	    }
	}
    }
}

































template<>
void Data_<SpDPtr>::AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList) 
{
  assert( ixList != NULL);

  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);

  SizeT srcElem= src->N_Elements();
  bool  isScalar= (srcElem == 1);
  if( isScalar)
    { // src is scalar
      SizeT nCp=ixList->N_Elements();

      if( nCp == 1)
	{
	  Ty& a = (*this)[ ixList->LongIx()] ;
	  Ty b = (*src)[ 0];
	  GDLInterpreter::IncRef( b);
	  GDLInterpreter::DecRef( a);
	  a = b;//(*this)[ ixList->LongIx()] = (*src)[0];
	}
      else
	{
	  Ty scalar=(*src)[0];
	  AllIxBaseT* allIx = ixList->BuildIx();
	
	  GDLInterpreter::AddRef( scalar, nCp);

	  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	  {
	    //#pragma omp for
	    for( SizeT c=0; c<nCp; ++c)
	      {
		Ty& a = (*this)[ (*allIx)[ c]];
		// 	Ty b = scalar;
		GDLInterpreter::DecRef( a);
		a = scalar;//	(*this)[ (*allIx)[ c]]=scalar;
	      }
	  }	  //	    (*this)[ ixList->GetIx( c)]=scalar;
	}
    }
  else
    {
      // crucial part
      SizeT nCp=ixList->N_Elements();
	
      if( nCp == 1)
	{
	  InsAt( src, ixList);
	}
      else
	{
	  if( srcElem < nCp)
	    throw GDLException("Array subscript must have same size as"
			       " source expression.");

	  AllIxBaseT* allIx = ixList->BuildIx();
	  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	  {
	    //#pragma omp for
	    for( SizeT c=0; c<nCp; ++c)
	      {
		Ty& a = (*this)[ (*allIx)[ c]];
		Ty b = (*src)[c];
		GDLInterpreter::IncRef( b);
		GDLInterpreter::DecRef( a);
		a = b;//	(*this)[ (*allIx)[ c]]=(*src)[c];
	      }
	  }	  //		(*this)[ ixList->GetIx( c)]=(*src)[c+offset];
	}
    }
}
template<>
void Data_<SpDPtr>::AssignAt( BaseGDL* srcIn)
{
  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);

  SizeT srcElem= src->N_Elements();
  bool  isScalar= (srcElem == 1);
  if( isScalar)
    { // src is scalar
      Ty scalar=(*src)[0];

      /*      dd = scalar;*/
      SizeT nCp=Data_::N_Elements();
	
      GDLInterpreter::AddRef( scalar, nCp);
      //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
      {
	//#pragma omp for
	for( SizeT c=0; c<nCp; ++c)
	  {
	    Ty& a = (*this)[ c];
	    GDLInterpreter::DecRef( a);
	    // 	GDLInterpreter::IncRef( b);
	    a = scalar;//(*this)[ c]=scalar;
	  }
      }  
      //       SizeT nCp=Data_::N_Elements();

      //       for( SizeT c=0; c<nCp; ++c)
      // 	(*this)[ c]=scalar;
    }
  else
    {
      SizeT nCp=Data_::N_Elements();
	
      // if (non-indexed) src is smaller -> just copy its number of elements
      if( nCp > srcElem) nCp=srcElem;
	
      //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
      {
	//#pragma omp for
	for( SizeT c=0; c<nCp; ++c)
	  {
	    Ty& a = (*this)[ c];
	    Ty b = (*src)[c];
	    GDLInterpreter::IncRef( b);
	    GDLInterpreter::DecRef( a);
	    a = b;//(*this)[ c]=(*src)[c];
	  }
      }
    }
}


template<>
void Data_<SpDObj>::AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList)
{
  assert( ixList != NULL);

  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);

  SizeT srcElem= src->N_Elements();
  bool  isScalar= (srcElem == 1);
  if( isScalar)
    { // src is scalar
      SizeT nCp=ixList->N_Elements();

      if( nCp == 1)
	{
	  Ty& a = (*this)[ ixList->LongIx()] ;
	  Ty b = (*src)[ 0];
	  GDLInterpreter::IncRefObj( b);
	  GDLInterpreter::DecRefObj( a);
	  a = b;//(*this)[ ixList->LongIx()] = (*src)[0];
	}
      else
	{
	  Ty scalar=(*src)[0];
	  AllIxBaseT* allIx = ixList->BuildIx();
	
	  GDLInterpreter::AddRefObj( scalar, nCp);

	  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	  {
	    //#pragma omp for
	    for( SizeT c=0; c<nCp; ++c)
	      {
		Ty& a = (*this)[ (*allIx)[ c]];
		GDLInterpreter::DecRefObj( a);
		a = scalar;//	(*this)[ (*allIx)[ c]]=scalar;
	      }
	  }	  //	    (*this)[ ixList->GetIx( c)]=scalar;
	}
    }
  else
    {
      // crucial part
      SizeT nCp=ixList->N_Elements();
	
      if( nCp == 1)
	{
	  InsAt( src, ixList);
	}
      else
	{
	  if( srcElem < nCp)
	    throw GDLException("Array subscript must have same size as"
			       " source expression.");

	  AllIxBaseT* allIx = ixList->BuildIx();
	  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	  {
	    //#pragma omp for
	    for( SizeT c=0; c<nCp; ++c)
	      {
		Ty& a = (*this)[ (*allIx)[ c]];
		Ty b = (*src)[c];
		GDLInterpreter::IncRefObj( b);
		GDLInterpreter::DecRefObj( a);
		a = b;//	(*this)[ (*allIx)[ c]]=(*src)[c];
	      }
	  }	  //		(*this)[ ixList->GetIx( c)]=(*src)[c+offset];
	}
    }
}
template<>
void Data_<SpDObj>::AssignAt( BaseGDL* srcIn)
{
  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);

  SizeT srcElem= src->N_Elements();
  bool  isScalar= (srcElem == 1);
  if( isScalar)
    { // src is scalar
      Ty scalar=(*src)[0];

      /*      dd = scalar;*/
      SizeT nCp=Data_::N_Elements();
	
      GDLInterpreter::AddRefObj( scalar, nCp);
	
      //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
      {
	//#pragma omp for
	for( SizeT c=0; c<nCp; ++c)
	  {
	    Ty& a = (*this)[ c];
	    GDLInterpreter::DecRefObj( a);
	    a = scalar;//(*this)[ c]=scalar;
	  }
      }

      //       SizeT nCp=Data_::N_Elements();

      //       for( SizeT c=0; c<nCp; ++c)
      // 	(*this)[ c]=scalar;
    }
  else
    {
      SizeT nCp=Data_::N_Elements();
	
      // if (non-indexed) src is smaller -> just copy its number of elements
      if( nCp > srcElem) nCp=srcElem;
	
      //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
      {
	//#pragma omp for
	for( SizeT c=0; c<nCp; ++c)
	  {
	    Ty& a = (*this)[ c];
	    Ty b = (*src)[c];
	    GDLInterpreter::IncRefObj( b);
	    GDLInterpreter::DecRefObj( a);
	    a = b;//(*this)[ c]=(*src)[c];
	  }
      }
    }
}



// returns (*this)[ ixList]
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Index( ArrayIndexListT* ixList)
{
  //  ixList->SetVariable( this);

  Data_* res=Data_::New( ixList->GetDim(), BaseGDL::NOZERO);

  SizeT nCp=ixList->N_Elements();

  //  cout << "nCP = " << nCp << endl;
  //  cout << "dim = " << this->dim << endl;

  //  DataT& res_dd = res->dd;
  AllIxBaseT* allIx = ixList->BuildIx();
  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
  {
    //#pragma omp for
    for( SizeT c=0; c<nCp; ++c)
      {
	Ty a = (*this)[ (*allIx)[ c]];
	GDLInterpreter::IncRef(a);
	(*res)[c]=a;
      }
  }  //    res_(*this)[c]=(*this)[ (*allIx)[ c]];
  //    (*res)[c]=(*this)[ ixList->GetIx(c)];

  return res;
}
// returns (*this)[ ixList]
template<>
Data_<SpDObj>* Data_<SpDObj>::Index( ArrayIndexListT* ixList)
{
  //  ixList->SetVariable( this);

  Data_* res=Data_::New( ixList->GetDim(), BaseGDL::NOZERO);

  SizeT nCp=ixList->N_Elements();

  //  cout << "nCP = " << nCp << endl;
  //  cout << "dim = " << this->dim << endl;

  //  DataT& res_dd = res->dd;
  AllIxBaseT* allIx = ixList->BuildIx();
  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
  {
    //#pragma omp for
    for( SizeT c=0; c<nCp; ++c)
      {
	Ty a = (*this)[ (*allIx)[ c]];
	GDLInterpreter::IncRefObj(a);
	(*res)[c]=a;
      }
  }  //    res_(*this)[c]=(*this)[ (*allIx)[ c]];
  //    (*res)[c]=(*this)[ ixList->GetIx(c)];

  return res;
}



// used by AccessDescT for resolving, no checking is done
// inserts srcIn[ ixList] at offset
// used by DotAccessDescT::DoResolve
template<>
void Data_<SpDPtr>::InsertAt( SizeT offset, BaseGDL* srcIn, 
			      ArrayIndexListT* ixList)
{
  Data_* src=static_cast<Data_* >(srcIn);
  if( ixList == NULL)
    {
      SizeT nCp=src->N_Elements();

      //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
      {
	//#pragma omp for
	for( SizeT c=0; c<nCp; ++c)
	  {
	    Ty a = (*this)[ c+offset];
	    Ty b = (*src)[c];
	    GDLInterpreter::IncRef( b);
	    GDLInterpreter::DecRef( a);
	    (*this)[ c+offset]=(*src)[c];
	  }
      }    }
  else
    {
      SizeT nCp=ixList->N_Elements();

      AllIxBaseT* allIx = ixList->BuildIx();
      //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
      {
	//#pragma omp for
	for( SizeT c=0; c<nCp; ++c)
	  {
	    Ty a = (*this)[ c+offset];
	    Ty b = (*src)[ (*allIx)[ c]];
	    GDLInterpreter::IncRef( b);
	    GDLInterpreter::DecRef( a);
	    (*this)[ c+offset]=(*src)[ (*allIx)[ c]];
	  }      //	(*this)[ c+offset]=(*src)[ ixList->GetIx( c)];
      }
    }
}


// used for concatenation, called from CatArray
// assumes that everything is checked (see CatInfo)
template<>
void Data_<SpDPtr>::CatInsert( const Data_* srcArr, const SizeT atDim, SizeT& at)
{
  // length of one segment to copy
  SizeT len=srcArr->dim.Stride(atDim+1); // src array

  // number of copy actions
  SizeT nCp=srcArr->N_Elements()/len;

  // initial offset
  SizeT destStart= this->dim.Stride(atDim) * at; // dest array
  SizeT destEnd  = destStart + len;

  // number of elements to skip
  SizeT gap=this->dim.Stride(atDim+1);    // dest array

#ifdef _OPENMP
  SizeT nEl = srcArr->N_Elements();
  //#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  for( SizeT c=0; c<nCp; ++c)
    {
      // set new destination pointer
      SizeT eIx = c*gap;
      SizeT sIx = eIx  + destStart;
      eIx += destEnd;

      // copy one segment
      SizeT srcIx = c*len;
      for( SizeT destIx=sIx; destIx< eIx; ++destIx)
	{
	  Ty b = (*srcArr)[ srcIx+destIx-sIx];
	  GDLInterpreter::IncRef( b);
	  (*this)[destIx] = (*srcArr)[ srcIx+destIx-sIx];
	}
    }
#else
  SizeT srcIx=0;
  for( SizeT c=0; c<nCp; ++c)
    {
      // copy one segment
      for( SizeT destIx=destStart; destIx< destEnd; destIx++)
	{
	  Ty b = (*srcArr)[ srcIx];
	  GDLInterpreter::IncRef( b);
	  (*this)[destIx] = (*srcArr)[ srcIx++];
	}

      // set new destination pointer
      destStart += gap;
      destEnd   += gap;
    }
#endif

  SizeT add=srcArr->dim[atDim]; // update 'at'
  at += (add > 1)? add : 1;
}



// used by AccessDescT for resolving, no checking is done
// inserts srcIn[ ixList] at offset
// used by DotAccessDescT::DoResolve
template<>
void Data_<SpDObj>::InsertAt( SizeT offset, BaseGDL* srcIn,
			      ArrayIndexListT* ixList)
{
  Data_* src=static_cast<Data_* >(srcIn);
  if( ixList == NULL)
    {
      SizeT nCp=src->N_Elements();

      //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
      {
	//#pragma omp for
	for( SizeT c=0; c<nCp; ++c)
	  {
	    Ty a = (*this)[ c+offset];
	    Ty b = (*src)[c];
	    GDLInterpreter::IncRefObj( b);
	    GDLInterpreter::DecRefObj( a);
	    (*this)[ c+offset]=(*src)[c];
	  }
      }    }
  else
    {
      SizeT nCp=ixList->N_Elements();

      AllIxBaseT* allIx = ixList->BuildIx();
      //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
      {
	//#pragma omp for
	for( SizeT c=0; c<nCp; ++c)
	  {
	    Ty a = (*this)[ c+offset];
	    Ty b = (*src)[ (*allIx)[ c]];
	    GDLInterpreter::IncRefObj( b);
	    GDLInterpreter::DecRefObj( a);
	    (*this)[ c+offset]=(*src)[ (*allIx)[ c]];
	  }      //	(*this)[ c+offset]=(*src)[ ixList->GetIx( c)];
      }
    }
}


// used for concatenation, called from CatArray
// assumes that everything is checked (see CatInfo)
template<>
void Data_<SpDObj>::CatInsert( const Data_* srcArr, const SizeT atDim, SizeT& at)
{
  // length of one segment to copy
  SizeT len=srcArr->dim.Stride(atDim+1); // src array

  // number of copy actions
  SizeT nCp=srcArr->N_Elements()/len;

  // initial offset
  SizeT destStart= this->dim.Stride(atDim) * at; // dest array
  SizeT destEnd  = destStart + len;

  // number of elements to skip
  SizeT gap=this->dim.Stride(atDim+1);    // dest array

#ifdef _OPENMP
  SizeT nEl = srcArr->N_Elements();
  //#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  for( SizeT c=0; c<nCp; ++c)
    {
      // set new destination pointer
      SizeT eIx = c*gap;
      SizeT sIx = eIx  + destStart;
      eIx += destEnd;

      // copy one segment
      SizeT srcIx = c*len;
      for( SizeT destIx=sIx; destIx< eIx; ++destIx)
	{
	  Ty b = (*srcArr)[ srcIx+destIx-sIx];
	  GDLInterpreter::IncRefObj( b);
	  (*this)[destIx] = (*srcArr)[ srcIx+destIx-sIx];
	}
    }
#else
  SizeT srcIx=0;
  for( SizeT c=0; c<nCp; ++c)
    {
      // copy one segment
      for( SizeT destIx=destStart; destIx< destEnd; destIx++)
	{
	  Ty b = (*srcArr)[ srcIx];
	  GDLInterpreter::IncRefObj( b);
	  (*this)[destIx] = (*srcArr)[ srcIx++];
	}

      // set new destination pointer
      destStart += gap;
      destEnd   += gap;
    }
#endif

  SizeT add=srcArr->dim[atDim]; // update 'at'
  at += (add > 1)? add : 1;
}



template<>
void Data_<SpDPtr>::Assign( BaseGDL* src, SizeT nEl)
{
//   Data_* srcT = dynamic_cast<Data_*>( src);
// 
//   Guard< Data_> srcTGuard;
//   if( srcT == NULL)
//     {
//       srcT = static_cast<Data_*>( src->Convert2( Data_::t, BaseGDL::COPY));
//       srcTGuard.Reset( srcT);
//     }
  Data_* srcT; // = dynamic_cast<Data_*>( src);

  Guard< Data_> srcTGuard;
  if( src->Type() != Data_::t) 
    {
      srcT = static_cast<Data_*>( src->Convert2( Data_::t, BaseGDL::COPY));
      srcTGuard.Init( srcT);
    }
  else
  {
    srcT = static_cast<Data_*>( src);
  }

  //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
    //#pragma omp for
    for(long k=0; k < nEl; ++k)
      {
	Ty a = (*this)[ k];
	Ty b = (*srcT)[k];
	GDLInterpreter::IncRef( b);
	GDLInterpreter::DecRef( a);
	(*this)[ k] = (*srcT)[ k];
      }    }
}

template<>
void Data_<SpDObj>::Assign( BaseGDL* src, SizeT nEl)
{
//   Data_* srcT = dynamic_cast<Data_*>( src);
// 
//   Guard< Data_> srcTGuard;
//   if( srcT == NULL)
//     {
//       srcT = static_cast<Data_*>( src->Convert2( Data_::t, BaseGDL::COPY));
//       srcTGuard.Reset( srcT);
//     }
  Data_* srcT; // = dynamic_cast<Data_*>( src);

  Guard< Data_> srcTGuard;
  if( src->Type() != Data_::t) 
    {
      srcT = static_cast<Data_*>( src->Convert2( Data_::t, BaseGDL::COPY));
      srcTGuard.Init( srcT);
    }
  else
  {
    srcT = static_cast<Data_*>( src);
  }

  //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
    //#pragma omp for
    for(long k=0; k < nEl; ++k)
      {
	Ty a = (*this)[ k];
	Ty b = (*srcT)[k];
	GDLInterpreter::IncRefObj( b);
	GDLInterpreter::DecRefObj( a);
	(*this)[ k] = (*srcT)[ k];
      }    }
}



// return a new type of itself (only for one dimensional case)
template<>
BaseGDL* Data_<SpDPtr>::NewIx( SizeT ix)
{
  Ty b = (*this)[ ix];
  GDLInterpreter::IncRef( b);
  return new Data_( (*this)[ ix]);
}

// return a new type of itself (only for one dimensional case)
template<>
BaseGDL* Data_<SpDObj>::NewIx( SizeT ix)
{
  if( !this->StrictScalar())
  {
    Ty b = (*this)[ ix];
    GDLInterpreter::IncRefObj( b);
    return new Data_( (*this)[ ix]);
  }

  DObj s = dd[0]; // is StrictScalar()
  if( s == 0)  // no overloads for null object
    return new Data_( 0);
  
  DStructGDL* oStructGDL= GDLInterpreter::GetObjHeapNoThrow( s);
  if( oStructGDL == NULL) // if object not valid -> default behaviour
    return new Data_( 0);
  
  DStructDesc* desc = oStructGDL->Desc();

  if( desc->IsParent("LIST"))
  {
      static DString cNodeName("GDL_CONTAINER_NODE");
      // because of .RESET_SESSION, we cannot use static here
      DStructDesc* containerDesc=FindInStructList( structList, cNodeName);
    
      // no static here, might vary in derived object
//       unsigned pHeadTag = desc->TagIndex( "PHEAD");
      static unsigned pTailTag = desc->TagIndex( "PTAIL");

      static unsigned pNextTag = FindInStructList( structList, cNodeName)->TagIndex( "PNEXT");
      static unsigned pDataTag = FindInStructList( structList, cNodeName)->TagIndex( "PDATA");
//       unsigned nListTag = desc->TagIndex( "NLIST");
//       SizeT listSize = (*static_cast<DLongGDL*>(oStructGDL->GetTag( nListTag, 0)))[0];

      DPtr actP = (*static_cast<DPtrGDL*>(oStructGDL->GetTag( pTailTag, 0)))[0];
      for( SizeT elIx = 0; elIx < ix; ++elIx)
      {
	BaseGDL* actPHeap = BaseGDL::interpreter->GetHeap( actP);
	if( actPHeap->Type() != GDL_STRUCT)
	  throw GDLException( "LIST node must be a STRUCT.");	
	DStructGDL* actPStruct = static_cast<DStructGDL*>( actPHeap);
	if( actPStruct->Desc() != containerDesc)
	  throw GDLException( "LIST node must be a GDL_CONTAINER_NODE STRUCT.");	

	actP = (*static_cast<DPtrGDL*>( actPStruct->GetTag( pNextTag, 0)))[0];
      }

    BaseGDL* actPHeap = BaseGDL::interpreter->GetHeap( actP);
    if( actPHeap->Type() != GDL_STRUCT)
	  throw GDLException( "LIST node must be a STRUCT.");	
    DStructGDL* actPStruct = static_cast<DStructGDL*>( actPHeap);
    if( actPStruct->Desc() != containerDesc)
	  throw GDLException( "LIST node must be a GDL_CONTAINER_NODE STRUCT.");	

    actP = (*static_cast<DPtrGDL*>(actPStruct->GetTag( pDataTag, 0)))[0];
    
    return BaseGDL::interpreter->GetHeap( actP)->Dup();
  }
  
  return new Data_( s);
}



template<>
Data_<SpDPtr>* Data_<SpDPtr>::NewIx( AllIxBaseT* ix, const dimension* dIn)
{
  SizeT nCp = ix->size();
  Data_* res=Data_::New( *dIn, BaseGDL::NOZERO);
  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
  {
    //#pragma omp for
    for( SizeT c=0; c<nCp; ++c)
      {
	Ty b = (*this)[ (*ix)[ c]];
	GDLInterpreter::IncRef( b);
	(*res)[c]=(*this)[ (*ix)[ c]];
      }
  }
  return res;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::NewIx( AllIxBaseT* ix, const dimension* dIn)
{
  SizeT nCp = ix->size();
  Data_* res=Data_::New( *dIn, BaseGDL::NOZERO);
  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
  {
    //#pragma omp for
    for( SizeT c=0; c<nCp; ++c)
      {
	Ty b = (*this)[ (*ix)[ c]];
	GDLInterpreter::IncRefObj( b);
	(*res)[c]=(*this)[ (*ix)[ c]];
      }
  }
  return res;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::NewIxFrom( SizeT s)
{
  SizeT nCp = dd.size() - s;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
  {
    //#pragma omp for
    for( SizeT c=0; c<nCp; ++c)
      {
	Ty b = (*this)[ s+c];
	GDLInterpreter::IncRef( b);
	(*res)[c]=(*this)[ s+c];
      }
  }
  return res;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::NewIxFrom( SizeT s)
{
  SizeT nCp = dd.size() - s;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
  {
    //#pragma omp for
    for( SizeT c=0; c<nCp; ++c)
      {
	Ty b = (*this)[ s+c];
	GDLInterpreter::IncRefObj( b);
	(*res)[c]=(*this)[ s+c];
      }
  }
  return res;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::NewIxFrom( SizeT s, SizeT e)
{
  SizeT nCp = e - s + 1;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
  {
    //#pragma omp for
    for( SizeT c=0; c<nCp; ++c)
      {
	Ty b = (*this)[ s+c];
	GDLInterpreter::IncRef( b);
	(*res)[c]=(*this)[ s+c];
      }
  }
  return res;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::NewIxFrom( SizeT s, SizeT e)
{
  SizeT nCp = e - s + 1;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  //#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
  {
    //#pragma omp for
    for( SizeT c=0; c<nCp; ++c)
      {
	Ty b = (*this)[ s+c];
	GDLInterpreter::IncRefObj( b);
	(*res)[c]=(*this)[ s+c];
      }
  }
  return res;
}

template<>
Data_<SpDPtr>* Data_<SpDPtr>::NewIxFromStride( SizeT s, SizeT stride)
{
  SizeT nCp = (dd.size() - s + stride - 1)/stride;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c, s += stride)
    {
      Ty b = (*this)[ s];
      GDLInterpreter::IncRef( b);
      (*res)[c]=(*this)[ s];
    }
  return res;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::NewIxFromStride( SizeT s, SizeT e, SizeT stride)
{
  SizeT nCp = (e - s + stride)/stride;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c, s += stride)
    {
      Ty b = (*this)[ s];
      GDLInterpreter::IncRef( b);
      (*res)[c]=(*this)[ s];
    }
  return res;
}

template<>
Data_<SpDObj>* Data_<SpDObj>::NewIxFromStride( SizeT s, SizeT stride)
{
  SizeT nCp = (dd.size() - s + stride - 1)/stride;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c, s += stride)
    {
      Ty b = (*this)[ s];
      GDLInterpreter::IncRefObj( b);
      (*res)[c]=(*this)[ s];
    }
  return res;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::NewIxFromStride( SizeT s, SizeT e, SizeT stride)
{
  SizeT nCp = (e - s + stride)/stride;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c, s += stride)
    {
      Ty b = (*this)[ s];
      GDLInterpreter::IncRefObj( b);
      (*res)[c]=(*this)[ s];
    }
  return res;
}






#undef NEWIX_SIGNEDINT
#undef NEWIX_UNSIGNEDINT
#define NEWIX_UNSIGNEDINT						\
  SizeT i = 0;								\
  for( ; i < nElem; ++i)						\
    if( (*src)[i] > upper)						\
      {									\
	if( strict)							\
	  throw GDLException("Array used to subscript array "		\
			     "contains out of range (>) subscript.");	\
	(*res)[i++]= upperVal;						\
	break;								\
      }									\
    else								\
      (*res)[i]= (*this)[ (*src)[i]];					\
  for(; i < nElem; ++i)							\
    if( (*src)[i] > upper)						\
      (*res)[i] = upperVal;						\
    else								\
      (*res)[i]= (*this)[ (*src)[i]];					\
  GDLInterpreter::IncRef( res);						\
  return guard.release();

#define NEWIX_SIGNEDINT							\
  SizeT i = 0;								\
  for(; i < nElem; ++i)							\
    if( (*src)[i] < 0)							\
      {									\
	if( strict)							\
	  throw GDLException("Array used to subscript array "		\
			     "contains out of range (<0) subscript.");	\
	(*res)[i++]= zeroVal;						\
	break;								\
      }									\
    else if( (*src)[i] > upper)						\
      {									\
	if( strict)							\
	  throw GDLException("Array used to subscript array "		\
			     "contains out of range (>) subscript.");	\
	(*res)[i++]= upperVal;						\
	break;								\
      }									\
    else								\
      (*res)[ i] = (*this)[ (*src)[ i]];				\
  for(; i < nElem; ++i)							\
    if( (*src)[i] < 0)							\
      (*res)[i]= zeroVal;						\
    else if( (*src)[i] > upper)						\
      (*res)[i]= upperVal;						\
    else								\
      (*res)[ i] = (*this)[ (*src)[ i]];				\
  GDLInterpreter::IncRef( res);						\
  return guard.release();

template<>
Data_<SpDPtr>* Data_<SpDPtr>::NewIx( BaseGDL* ix, bool strict)
{
 	
  assert( ix->Type() != GDL_UNDEF);

  // no type checking needed here: GetAsIndex() will fail with grace
  //     int typeCheck = DTypeOrder[ dType];
  // 	if( typeCheck >= 100)
  // 	  throw GDLException("Type "+ix->TypeStr()+" not allowed as subscript.");
  
  SizeT nElem = ix->N_Elements();

  Data_* res = New( ix->Dim(), BaseGDL::NOZERO);
  Guard<Data_> guard( res);

  SizeT upper = dd.size() - 1;
  Ty    upperVal = (*this)[ upper];
  if( strict)
    {
      for(SizeT i = 0 ; i < nElem; ++i)
	{
	  SizeT actIx = ix->GetAsIndexStrict( i);
	  if( actIx > upper)
	    throw GDLException("Array used to subscript array "
			       "contains out of range (>) subscript (at index: "+i2s(i)+").");
	  (*res)[i]= (*this)[ actIx];
	}
    }
  else // not strict
    {
      for(SizeT i = 0 ; i < nElem; ++i)
	{
	  SizeT actIx = ix->GetAsIndex( i);
	  if( actIx >= upper)
	    (*res)[i] = upperVal;
	  else
	    (*res)[i]= (*this)[ actIx];
	}
    }
  GDLInterpreter::IncRef( res);
  return guard.release();
}



template<>
Data_<SpDObj>* Data_<SpDObj>::NewIx( BaseGDL* ix, bool strict)
{
  SizeT nElem = ix->N_Elements();

  Data_* res = New( ix->Dim(), BaseGDL::NOZERO);
  Guard<Data_> guard( res);

  SizeT upper = dd.size() - 1;
  Ty    upperVal = (*this)[ upper];
  if( strict)
    {
      for(SizeT i = 0 ; i < nElem; ++i)
	{
	  SizeT actIx = ix->GetAsIndexStrict( i);
	  if( actIx > upper)
	    throw GDLException("Array used to subscript array "
			       "contains out of range (>) subscript (at index: "+i2s(i)+").");
	  (*res)[i]= (*this)[ actIx];
	}
    }
  else // not strict
    {
      for(SizeT i = 0 ; i < nElem; ++i)
	{
	  SizeT actIx = ix->GetAsIndex( i);
	  if( actIx >= upper)
	    (*res)[i] = upperVal;
	  else
	    (*res)[i]= (*this)[ actIx];
	}
    }
  GDLInterpreter::IncRefObj( res);
  return guard.release();
}

#endif
