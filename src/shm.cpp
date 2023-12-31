/***************************************************************************
						  shm.cpp  -  Shared memory mapping
							 -------------------
	begin                : Dec 24 2023
	copyright            : (C) 2023 by Gilles Duvert
	email                : surname dot name at free dot fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _MSC_VER
#include <sys/mman.h>   /* shared memory and mmap() */
#include <sys/stat.h>
#include <fcntl.h>      /* O_flags */
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "shm.hpp"
#include "basic_fun.hpp" //for arr()
static std::map<DString, BaseGDL*> shmList;
typedef std::map<DString, BaseGDL*>::iterator shmListIter;

enum { BYTE=0,COMPLEX,DCOMPLEX,DOUBLE,FLOAT,INTEGER,L64,LONG,UINT,UL64,ULONG, DIMENSION,SIZE, TEMPLATE, TYPE} common_options_shm;

//atom size of all types
static const int atomSize[16]={0,1,2,4,4,8,8,0,0,16,0,0,2,4,8,8};
namespace lib {

  bool get_shm_common_keywords(EnvT* e, std::string &segmentName, dimension & dim, int & type) {
	SizeT np = e->NParam(0);
	if (np != 0) e->AssureStringScalarPar(0, segmentName);
	if (segmentName.size() == 0) segmentName = "GDL_SHM_" + i2s(getpid()) + "_0";
	bool dimNotSet = true;
	// DIMENSION etc 
	// Some memory leak possible in rare cases
	if (e->KeywordSet(DIMENSION)) {
	  DLongGDL* pDim = e->GetKWAs<DLongGDL>(DIMENSION);
	  if (pDim->N_Elements() < 4 || pDim->N_Elements() > 11) e->Throw("Keyword array parameter SIZE must have from 4 to 11 elements.");
	  SizeT nDim = (*pDim)[0];
	  if (pDim->N_Elements() < nDim + 2) e->Throw("Keyword array parameter SIZE has a wrong number of elements.");
	  for (auto i = 0; i < nDim; ++i) dim << (*pDim)[i + 1];
	  type = (*pDim)[nDim + 1];
	  dimNotSet = false;
	} else if (e->KeywordSet(SIZE)) {
	  BaseGDL* pSize = e->GetKW(SIZE);
	  dim = pSize->Dim();
	  dimNotSet = false;
	} else if (np > 1) {
	  arr(e, dim, 1); //get all dims starting at 1
	  dimNotSet = false;
	}
	//	else if (!e->KeywordSet(TEMPLATE)) e->Throw("Unable to set Dimension(s).");

	//TYPE
	type = GDL_FLOAT; //by default
	if (e->KeywordSet(TYPE)) {
	  DLong typeget = type;
	  e->AssureLongScalarKW(TYPE, typeget);
	  type = typeget;
	} else {
	  if (e->KeywordSet(BYTE)) type = GDL_BYTE;
	  else if (e->KeywordSet(COMPLEX)) type = GDL_COMPLEX;
	  else if (e->KeywordSet(DCOMPLEX)) type = GDL_COMPLEXDBL;
	  else if (e->KeywordSet(DOUBLE)) type = GDL_DOUBLE;
	  else if (e->KeywordSet(FLOAT)) type = GDL_FLOAT;
	  else if (e->KeywordSet(INTEGER)) type = GDL_INT;
	  else if (e->KeywordSet(L64)) type = GDL_LONG64;
	  else if (e->KeywordSet(LONG)) type = GDL_LONG;
	  else if (e->KeywordSet(UINT)) type = GDL_UINT;
	  else if (e->KeywordSet(UL64)) type = GDL_ULONG64;
	  else if (e->KeywordSet(ULONG)) type = GDL_ULONG;
	}
	//TEMPLATE wins ALL ??
	if (e->KeywordSet(TEMPLATE)) {
	  BaseGDL* pTemp = e->GetKW(TEMPLATE);
	  dim = pTemp->Dim();
	  dimNotSet = false;
	  type = pTemp->Type();
	}
	//check type
	if (!gdl_type_lookup::IsConvertableType[type]) { //problems begins
	  e->Throw("Objects, Pointers and Structures not allowed in this context.");
	}
	return dimNotSet;
  }

  void shmmap_pro(EnvT* e) {
	dimension dim;
	int type;
	std::string segmentName;
	//get common infos:
	if (get_shm_common_keywords(e,  segmentName, dim, type) != false) e->Throw("Expression must be an array in this context: <Shared Memory Segment>.");
	size_t length = dim.NDimElements()*(atomSize[type]);
	if (length < 1) e->Throw("internal error,please report.");
	struct stat filestat;
	//try to open existing:
	int shm_fd; /* file descriptor */
	int exist_perms = S_IRUSR | S_IWUSR ; 
	int create_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP; 
	bool exist = false;
    
	static int offsetIx = e->KeywordIx("OFFSET");
	DLong64 offset = 0;
	if (e->KeywordPresentAndDefined(offsetIx)) {
	  e->AssureLongScalarKW(offsetIx, offset);
	}
	
	length += offset;

    shm_fd = shm_open(segmentName.c_str(), O_RDWR, exist_perms); //minimal setup.
	if (shm_fd != -1) { //file exist
	  exist=true;
	  //get max allowed size and set default values
	  if (fstat(shm_fd, &filestat) == -1) e->Throw("Existing Mapping segment " + segmentName + " size retrieval failed.");
	  if (length > filestat.st_size) {
		shm_unlink(segmentName.c_str());
		e->Throw("Existing file too short for desired mapping.");
	  }
	}
	if (!exist) {
	  int oflags = O_RDWR | O_CREAT | O_EXCL; /* open flags receives -c, -x, -t */
	  //try to open unexisting:
	  shm_fd = shm_open(segmentName.c_str(), oflags, create_perms);
	  if (shm_fd == -1) e->Throw("Mapping segment " + segmentName + " failed."); //this is an error
	  ftruncate(shm_fd, length);
	}
	//create a BaseGDL corresponding to all info:
	BaseGDL* var;
	switch (type) {
	case GDL_FLOAT:
	  var = new DFloatGDL(dim, BaseGDL::NOALLOC);
	  break;
	case GDL_DOUBLE:
	  var = new DDoubleGDL(dim, BaseGDL::NOALLOC);
	  break;
	case GDL_LONG:
	  var = new DLongGDL(dim, BaseGDL::NOALLOC);
	  break;
	case GDL_BYTE:
	  var = new DByteGDL(dim, BaseGDL::NOALLOC);
	  break;
	case GDL_INT:
	  var = new DIntGDL(dim, BaseGDL::NOALLOC);
	  break;
	case GDL_COMPLEX:
	  var = new DComplexGDL(dim, BaseGDL::NOALLOC);
	  break;
	case GDL_COMPLEXDBL:
	  var = new DComplexDblGDL(dim, BaseGDL::NOALLOC);
	  break;
	case GDL_UINT:
	  var = new DUIntGDL(dim, BaseGDL::NOALLOC);
	  break;
	case GDL_ULONG:
	  var = new DULongGDL(dim, BaseGDL::NOALLOC);
	  break;
	case GDL_LONG64:
	  var = new DLong64GDL(dim, BaseGDL::NOALLOC);
	  break;
	case GDL_ULONG64:
	  var = new DULong64GDL(dim, BaseGDL::NOALLOC);
	  break;
	default:
	  //  case GDL_STRING:
	  //  case GDL_PTR:
	  //  case GDL_OBJ:
	  //  case GDL_STRUCT:
	  //  case GDL_UNDEF:
	  e->Throw(" internal error, please report.");
	}
	if (exist) { //checks OK for size
	  if (var->NBytes() + offset > filestat.st_size) e->Throw("Mapped address is not large enough for requested size and type.");
	  if (shmList.find(segmentName) != shmList.end()) {
		GDLDelete(var);
		return; //already existing
	  }
	}
	//now, exist or not, we define only the requested memory mapping size "length".
	static int privateIx = e->KeywordIx("PRIVATE");
	static int filenameIx = e->KeywordIx("FILENAME");
	bool isPrivate = false;
	if (e->KeywordSet(filenameIx) && e->KeywordSet(privateIx)) isPrivate = true;
	int mmap_flags = (isPrivate) ? MAP_PRIVATE : MAP_SHARED;
	int mmap_prot = PROT_READ | PROT_WRITE;
	void* mapAddress = mmap(NULL, length, mmap_prot, mmap_flags, shm_fd, 0);
	if (mapAddress == MAP_FAILED) e->Throw("shmmap failed, please report.");
	var->SetBuffer(mapAddress);
	var->SetBufferSize(dim.NDimElements());
	var->SetDim(dim);
	shmList.insert(std::pair<DString, BaseGDL*>(segmentName, var));

	static int getnameIx = e->KeywordIx("GET_NAME");
	if (e->WriteableKeywordPresent(getnameIx)) {
	  e->SetKW(getnameIx, new DStringGDL(segmentName));
	}
  }

  void shmunmap_pro(EnvT* e) {
	std::string segmentName;
	SizeT np = e->NParam(1);
	e->AssureStringScalarPar(0, segmentName);
	if (segmentName.size() == 0) e->Throw("Null string not allowed in this context: "+e->GetParString(0)+".");
	shmListIter i = shmList.find(segmentName);
	BaseGDL* res;
	void* was=NULL;
	SizeT length=0;
	if (i != shmList.end()) {
	  res = (*i).second;
	  was=res->DataAddr();
	  length=res->NBytes();
	  shmList.erase(i);
	} else e->Throw("Shared Memory Segment not found: " + segmentName + ".");
	int result=munmap(was,length);
	if (result ==0) return;
	e->Throw("Shared Memory Segment " + segmentName + " Unmapping unsucessfull, reason: "+ std::string(strerror(result))+".");
 };

  BaseGDL* shmvar_fun(EnvT* e) {
	dimension dim;
	int type;
	std::string segmentName = "";
	bool dimNotSet = get_shm_common_keywords(e, segmentName, dim, type);
	shmListIter i = shmList.find(segmentName);
	BaseGDL* res;
	if (i != shmList.end()) {
	  res = (*i).second;
	  //check if compatible
	  if (dimNotSet) return res; //no further problem, we take as it was
	  if (dim.NDimElements()*(atomSize[type]) > res->NBytes()) e->Throw("Requested variable is too long for the underlying shared memory segment: " + segmentName + ".");

	  //create a BaseGDL corresponding to that:
	  BaseGDL* var;
	  switch (type) {
	  case GDL_FLOAT:
		var = new DFloatGDL(dim, BaseGDL::NOALLOC);
		break;
	  case GDL_DOUBLE:
		var = new DDoubleGDL(dim, BaseGDL::NOALLOC);
		break;
	  case GDL_LONG:
		var = new DLongGDL(dim, BaseGDL::NOALLOC);
		break;
	  case GDL_BYTE:
		var = new DByteGDL(dim, BaseGDL::NOALLOC);
		break;
	  case GDL_INT:
		var = new DIntGDL(dim, BaseGDL::NOALLOC);
		break;
	  case GDL_COMPLEX:
		var = new DComplexGDL(dim, BaseGDL::NOALLOC);
		break;
	  case GDL_COMPLEXDBL:
		var = new DComplexDblGDL(dim, BaseGDL::NOALLOC);
		break;
	  case GDL_UINT:
		var = new DUIntGDL(dim, BaseGDL::NOALLOC);
		break;
	  case GDL_ULONG:
		var = new DULongGDL(dim, BaseGDL::NOALLOC);
		break;
	  case GDL_LONG64:
		var = new DLong64GDL(dim, BaseGDL::NOALLOC);
		break;
	  case GDL_ULONG64:
		var = new DULong64GDL(dim, BaseGDL::NOALLOC);
		break;
	  default:
		//  case GDL_STRING:
		//  case GDL_PTR:
		//  case GDL_OBJ:
		//  case GDL_STRUCT:
		//  case GDL_UNDEF:
		e->Throw(" internal error, please report.");
	  }
	  var->SetBuffer(static_cast<void*> (res->DataAddr()));
	  var->SetBufferSize(dim.NDimElements());
	  var->SetDim(dim);
	  return var;

	}

	e->Throw("Shared Memory Segment not found: " + segmentName + ".");
	return NULL;
  }

  BaseGDL* shmdebug_fun(EnvT* e) {
	return new DIntGDL(0);
  }

} // namespace
#endif
