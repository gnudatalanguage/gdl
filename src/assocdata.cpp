/***************************************************************************
                          assocdata.cpp  -  GDL ASSOC datatype
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

#include "assocdata.hpp"
#include "arrayindexlistt.hpp"
#include "objects.hpp"
#include "io.hpp"

using namespace std;


template< class Parent_>
vector< void*> Assoc_< Parent_>::freeList;

template< class Parent_>
void* Assoc_< Parent_>::operator new( size_t bytes)
{
  assert( bytes == sizeof( Assoc_< Parent_> ));
  
  if( freeList.size() > 0)
  {
    void* res = freeList.back();
    freeList.pop_back();
    return res;	
  }

  const size_t newSize = multiAlloc - 1;

  static long callCount = 0;
  ++callCount;

  // reserve space for all instances
  freeList.reserve( callCount*multiAlloc);

  // resize to what is needed now
  freeList.resize( newSize);

#ifdef USE_EIGEN  
  // we need this allocation here as well (as in typedefs.hpp), because GDLArray needs to be aligned
  const int alignmentInBytes = 16; // set to multiple of 16 >= sizeof( char*)
  const size_t realSizeOfType = sizeof( Assoc_< Parent_>);
  const SizeT exceed = realSizeOfType % alignmentInBytes;
  const size_t sizeOfType = realSizeOfType + (alignmentInBytes - exceed);
  char* res = static_cast< char*>( Eigen::internal::aligned_malloc( sizeOfType * multiAlloc)); // one more than newSize
#else
  const size_t sizeOfType = sizeof( Assoc_< Parent_>);
  char* res = static_cast< char*>( malloc( sizeOfType * multiAlloc)); // one more than newSize
#endif
  
  for( size_t i=0; i<newSize; ++i)
  {
	  freeList[ i] = res;
	  res += sizeOfType;
  } 
  
  // the one more
  return res;
}

template< class Parent_>
void Assoc_< Parent_>::operator delete( void *ptr)
{
	freeList.push_back( ptr);
}


template< class Parent_>
Assoc_< Parent_>::Assoc_( int lun_, Parent_* assoc_, SizeT fileOffset_): 
    Parent_( assoc_->Dim(), BaseGDL::NOZERO),
    lun( lun_-1), fileOffset( fileOffset_), sliceSize( assoc_->NBytes())
{
  if( assoc_->Type() == GDL_STRING)
    throw GDLException( "Expression containing string data not allowed in this context.");
  if( assoc_->Type() == GDL_PTR)
    throw GDLException( "Expression containing pointers not allowed in this context.");
  if( assoc_->Type() == GDL_OBJ)
    throw GDLException( "Expression containing object references not allowed in this context.");
}

template<>
Assoc_< DStructGDL>::Assoc_( int lun_, DStructGDL* assoc_, SizeT fileOffset_): 
  DStructGDL( assoc_->Desc(), assoc_->Dim()), // BaseGDL::NOZERO), // an instance is needed (for storing the data slice)
    lun( lun_-1), fileOffset( fileOffset_), sliceSize( assoc_->NBytes())
{
  MakeOwnDesc();
}


// called from help_item
template<class Parent_>
std::ostream& Assoc_<Parent_>::ToStream(std::ostream& o, SizeT width, SizeT* actPosPtr)
{
  o << "File<" << fileUnits[ lun].Name() << "> "; 
  return o;
//     throw GDLException("File expression not allowed in this context.");
}


  
// writing 1
// assigns srcIn to this at ixList, if ixList is NULL does linear copy
// assumes: ixList has this already set as variable
// ("offset" is offset into srcIn)
template<class Parent_>
void Assoc_<Parent_>::AssignAt( BaseGDL* srcIn, 
				ArrayIndexListT* ixList, 
				SizeT offset)
{
  assert( offset == 0);

  SizeT recordNum;
  bool ixEmpty = ixList->ToAssocIndex( recordNum);
  
  if( !ixEmpty)
    {
      // throw GDLException("File expression cannot be subindexed for output.");
      SizeT seekPos = fileOffset + recordNum * sliceSize;

      if( fileUnits[ lun].Size() > seekPos)
	{
	  fileUnits[ lun].Seek( seekPos);
	  Parent_::Read( (fileUnits[lun].Compress()?static_cast<std::istream&>(fileUnits[lun].IgzStream()):static_cast<std::istream&>(fileUnits[lun].IStream())), 
			 fileUnits[ lun].SwapEndian(),
			 fileUnits[ lun].Compress(),
			 fileUnits[ lun].Xdr());
	}
      else
	{
	  Parent_::Clear(); // zero fields
	}

      Parent_::AssignAt( srcIn, ixList, offset);

      fstream& fs = fileUnits[ lun].OStream();
      fileUnits[ lun].SeekPad( seekPos);
      Parent_::Write( fs, 
		      fileUnits[ lun].SwapEndian(),
		      fileUnits[ lun].Compress(),
		      fileUnits[ lun].Xdr());
    }
  else
    {
      // ix empty -> write direct
      fstream& ofs = fileUnits[ lun].OStream();
      fileUnits[ lun].SeekPad( fileOffset + recordNum * sliceSize);
      srcIn->Write( ofs,
		    fileUnits[ lun].SwapEndian(),
		    fileUnits[ lun].Compress(),
		    fileUnits[ lun].Xdr());
    }
}
template<class Parent_>
void Assoc_<Parent_>::AssignAt( BaseGDL* srcIn, 
				ArrayIndexListT* ixList) 
{
  SizeT recordNum;
  bool ixEmpty = ixList->ToAssocIndex( recordNum);
  
  if( !ixEmpty)
    {
      // throw GDLException("File expression cannot be subindexed for output.");
      SizeT seekPos = fileOffset + recordNum * sliceSize;

      if( fileUnits[ lun].Size() > seekPos)
	{
//	  fstream& fs = fileUnits[ lun].IStream();
	  fileUnits[ lun].Seek( seekPos);
	  Parent_::Read( (fileUnits[lun].Compress()?static_cast<std::istream&>(fileUnits[lun].IgzStream()):static_cast<std::istream&>(fileUnits[lun].IStream())),  
			 fileUnits[ lun].SwapEndian(),
			 fileUnits[ lun].Compress(),
			 fileUnits[ lun].Xdr());
	}
      else
	{
	  Parent_::Clear(); // zero fields
	}

      Parent_::AssignAt( srcIn, ixList);

      fstream& fs = fileUnits[ lun].OStream();
      fileUnits[ lun].SeekPad( seekPos);
      //      Parent_::Write( fs, fileUnits[ lun].SwapEndian());
      Parent_::Write( fs,
		    fileUnits[ lun].SwapEndian(),
		    fileUnits[ lun].Compress(),
		    fileUnits[ lun].Xdr());
    }
  else
    {
      // ix empty -> write direct
      fstream& ofs = fileUnits[ lun].OStream();
      fileUnits[ lun].SeekPad( fileOffset + recordNum * sliceSize);
      srcIn->Write( ofs, 
		fileUnits[ lun].SwapEndian(),
		fileUnits[ lun].Compress(),
		fileUnits[ lun].Xdr());
/*      Parent_::Write( ofs, 
		      fileUnits[ lun].SwapEndian(),
		      fileUnits[ lun].Xdr());*/
    }
}
template<class Parent_>
void Assoc_<Parent_>::AssignAt( BaseGDL* srcIn) 
{
  SizeT recordNum = 0;
  
  // ix empty -> write direct
  fstream& ofs = fileUnits[ lun].OStream();
  fileUnits[ lun].SeekPad( fileOffset + recordNum * sliceSize);
  srcIn->Write( ofs, 
		fileUnits[ lun].SwapEndian(),
		fileUnits[ lun].Compress(),
		fileUnits[ lun].Xdr());
/*  Parent_::Write( ofs, 
		  fileUnits[ lun].SwapEndian(),
		  fileUnits[ lun].Xdr());*/
}

// (writing 2)
// should not be called at all
// only from AccessdescT::Resolve()
// and this only for dot expressions (which are not allowed for assoc vars)
template<class Parent_>
void Assoc_<Parent_>::InsertAt( SizeT offset, 
				BaseGDL* srcIn, 
				ArrayIndexListT* ixList)
{
  throw GDLException("Internal error: Assoc::InsertAt: called.");

  SizeT recordNum;
  bool ixEmpty = ixList->ToAssocIndex( recordNum);

  if( ixEmpty)
    {
      // potential problem if offset != 0
      Parent_::InsertAt( offset, srcIn, NULL);

      fstream& ofs = fileUnits[ lun].OStream();
      fileUnits[ lun].SeekPad( fileOffset + recordNum * sliceSize);
      //      Parent_::Write( ofs, fileUnits[ lun].SwapEndian());
      Parent_::Write( ofs, 
		      fileUnits[ lun].SwapEndian(),
		      fileUnits[ lun].Compress(),
		      fileUnits[ lun].Xdr());
    }
  else
    {
      // throw GDLException("File expression cannot be subindexed for output.");
      fstream& fs = fileUnits[ lun].IStream();
      fileUnits[ lun].Seek( fileOffset + recordNum * sliceSize);
      Parent_::Read( fs,
		     fileUnits[ lun].SwapEndian(),
		     fileUnits[ lun].Compress(),
		     fileUnits[ lun].Xdr());

      Parent_::InsertAt( offset, srcIn, ixList);

      fileUnits[ lun].SeekPad( fileOffset + recordNum * sliceSize);
      //      Parent_::Write( fs, fileUnits[ lun].SwapEndian());
      Parent_::Write( fs, 
		      fileUnits[ lun].SwapEndian(),
		      fileUnits[ lun].Compress(),
		      fileUnits[ lun].Xdr());
      return;
    }
}

// reading OK
// returns (*this)[ ixList]
template<class Parent_>
Parent_* Assoc_<Parent_>::Index( ArrayIndexListT* ixList)
{
  SizeT recordNum;
  bool ixEmpty = ixList->ToAssocIndex( recordNum);

  istream& fs = fileUnits[lun].Compress()?static_cast<std::istream&>(fileUnits[lun].IgzStream()):static_cast<std::istream&>(fileUnits[lun].IStream());
  fileUnits[ lun].Seek( fileOffset + recordNum * sliceSize);
  Parent_::Read( fs,
		 fileUnits[ lun].SwapEndian(),
		 fileUnits[ lun].Compress(),
		 fileUnits[ lun].Xdr());

  if( ixEmpty)
    {
      return Parent_::Dup();
    }
  else
    {
      return Parent_::Index( ixList);
    }
}


// explicit instantiation requests

template class Assoc_< DByteGDL>;
template class Assoc_< DIntGDL>;
template class Assoc_< DUIntGDL>;
template class Assoc_< DLongGDL>;
template class Assoc_< DULongGDL>;
template class Assoc_< DLong64GDL>;
template class Assoc_< DULong64GDL>;
template class Assoc_< DPtrGDL>;
template class Assoc_< DStructGDL>;
template class Assoc_< DFloatGDL>;
template class Assoc_< DDoubleGDL>;
template class Assoc_< DStringGDL>;
template class Assoc_< DObjGDL>;
template class Assoc_< DComplexGDL>;
template class Assoc_< DComplexDblGDL>;
