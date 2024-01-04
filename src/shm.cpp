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
#ifndef _WIN32
#include <sys/mman.h>   /* shared memory and mmap() */
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>      /* O_flags */
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "shm.hpp"
#include "basic_fun.hpp" //for arr()
#include "gdlhelp.hpp"
#include "dinterpreter.hpp"

std::map<DString, SHMAP_STRUCT > shmList;

enum { BYTE=0,COMPLEX,DCOMPLEX,DOUBLE,FLOAT,INTEGER,L64,LONG,UINT,UL64,ULONG, DIMENSION,SIZE, TEMPLATE, TYPE} common_options_shm;

//atom size of all types and names
static const std::string atomName[16]={"UNDEFINED","BYTE","INT","LONG","FLOAT","DOUBLE","COMPLEX","STRING","STRUCT","DCOMPLEX","POINTER","OBJREF","UINT","ULONG","LONG64","ULONG64"};
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
	if (type==GDL_STRING) e->Throw("Expression containing string data not allowed in this context.");
	return dimNotSet;
  }

  void shmmap_pro(EnvT* e) {
	dimension dim;
	int type;
	std::string osHandle;
	int sysv_hdle=0;
	std::string segmentName;
	//get common infos:
	if (get_shm_common_keywords(e,  segmentName, dim, type) != false) e->Throw("Expression must be an array in this context: <Shared Memory Segment>.");
    static int OS_HANDLE=e->KeywordIx("OS_HANDLE");
	bool doOsHandle=e->KeywordSet(OS_HANDLE);
	if (doOsHandle) e->AssureStringScalarKW(OS_HANDLE,osHandle); else {
      if (segmentName.substr(0,1) != "/") osHandle="/"+segmentName; else osHandle=segmentName;
	}
	
	static int filenameIx = e->KeywordIx("FILENAME");
	bool mappedfile=false;
	if (e->KeywordPresent(filenameIx)){
	  mappedfile=true;
	  e->AssureStringScalarKW(filenameIx,osHandle);
	}
	
	size_t requested_length = dim.NDimElements()*(atomSize[type]);
	if (requested_length < 1) e->Throw("internal error,please report.");
	struct stat filestat;
	//try to open existing:
	int shm_fd; /* file descriptor */
	int exist_perms = S_IRUSR | S_IWUSR ; 
	int create_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
	int all_perms = 0777;
	bool exist = false;

    static int DESTROY_SEGMENT=e->KeywordIx("DESTROY_SEGMENT");
	bool destroy_segment=false; //by default
    static int SYSV=e->KeywordIx("SYSV");
	bool sysv=e->KeywordSet(SYSV);
	if (sysv && mappedfile ) e->Throw("Conflicting keywords.");
    static int PRIVATE=e->KeywordIx("PRIVATE");
	bool open_private=e->KeywordSet(PRIVATE);
	
	static int offsetIx = e->KeywordIx("OFFSET");
	DLong64 offset = 0;
	if (e->KeywordPresentAndDefined(offsetIx)) {
	  e->AssureLongScalarKW(offsetIx, offset);
	}
	if (offset%atomSize[type] != 0) e->Throw("This machine cannot access data of type "+atomName[type]+" at this alignment: "+i2s(offset)+".");
	size_t total_length=(requested_length + offset);
	if (sysv) {
	  //reinterpret OS_HANDLE
	  DLong sysv_handle=0;
	  if (doOsHandle) { //must exist and be useable
		e->AssureLongScalarKW(OS_HANDLE,sysv_handle);
		int shmflg=all_perms;
		key_t handle=sysv_handle;
		sysv_hdle=shmget(handle,total_length,shmflg);
		if (sysv_hdle ==-1) e->Throw("SYSV Shared Memory Segment " + i2s(sysv_handle) + " attach failed, reason: " + std::string(strerror(errno)) + ".");
	  } else {
		int shmflg=IPC_CREAT|all_perms;
		key_t handle=0;
		sysv_hdle=shmget(handle,total_length,shmflg);
		if (sysv_hdle == -1) e->Throw("SYSV Shared Memory Segment " + i2s(sysv_handle) + " creation failed, reason: " + std::string(strerror(errno)) + ".");
	  }
	} else {
	  if (mappedfile) {
		shm_fd = open(osHandle.c_str(), O_RDWR, exist_perms); //minimal setup.
	  } else {
		shm_fd = shm_open(osHandle.c_str(), O_RDWR, exist_perms); //minimal setup.
	  }
	  if (shm_fd != -1) { //file exist
		exist = true;
		if (shmList.find(segmentName) != shmList.end()) {
		  e->Throw("Attempt to redefine existing shared memory segment: " + segmentName + ".");
		  return; //already existing
		}
		//check max allowed size and set default values
		if (fstat(shm_fd, &filestat) == -1) e->Throw("Existing Mapping segment " + osHandle + " size retrieval failed.");
		if (total_length > filestat.st_size) {
		  shm_unlink(osHandle.c_str());
		  e->Throw("Existing file too short for desired mapping.");
		}
		//segment is not destroyable on exit unless 'destroy_segment' is set
		destroy_segment = e->KeywordSet(DESTROY_SEGMENT);
	  }
	  if (!exist) {
		int oflags = O_RDWR | O_CREAT | O_EXCL; /* open flags receives -c, -x, -t */
		//try to open unexisting:
		if (mappedfile) {
		  shm_fd = open(osHandle.c_str(), oflags, create_perms); //minimal setup.
		} else {
		  shm_fd = shm_open(osHandle.c_str(), oflags, create_perms);
		}
		if (shm_fd == -1) e->Throw("Mapping segment " + osHandle + " failed."); //this is an error
		int status = ftruncate(shm_fd, total_length);
		if (status != 0) e->Throw("Shared Memory Segment " + segmentName + " creation failed (size), reason: " + std::string(strerror(errno)) + ".");
		//segment is destroyable on exit unless 'destroy_segment' is set to 0
		destroy_segment = true;
		if (e->KeywordPresentAndDefined(DESTROY_SEGMENT)) destroy_segment = e->KeywordSet(DESTROY_SEGMENT);
	  }
	}
	//now, exist or not, we define only the requested memory mapping size "length".
	void* mapAddress=NULL;
	if (sysv) {
	  int shmflg=0;
	  mapAddress=shmat(sysv_hdle,NULL,shmflg);
	  if (mapAddress == (void*) -1) e->Throw("shmmap (/SYSV) failed, please report.");
	  osHandle=i2s(sysv_hdle); //saved as string
	} else {
	  int mmap_flags = (mappedfile && open_private) ? MAP_PRIVATE : MAP_SHARED;
	  int mmap_prot = PROT_READ | PROT_WRITE;
	  mapAddress = mmap(NULL, requested_length, mmap_prot, mmap_flags, shm_fd, offset);
	  close(shm_fd); //"After a call to mmap(2) the file descriptor may be closed without affecting the memory mapping."
	  if (mapAddress == MAP_FAILED) e->Throw("shmmap failed, please report.");
	}
	SHMAP_STRUCT s;
    s.mapped_address=mapAddress;
	s.osHandle=osHandle;
	s.length=requested_length;
	s.offset=offset;
	s.refcount=0;
	s.dim=dim;
	s.type=type;
	if (destroy_segment) s.flags |= DESTROY_SEGMENT_ON_UNMAP;
	if (sysv) s.flags |= USE_SYSV;
	shmList.insert(std::pair<DString, SHMAP_STRUCT>(segmentName, s));

	static int getnameIx = e->KeywordIx("GET_NAME");
	if (e->WriteableKeywordPresent(getnameIx)) {
	  e->SetKW(getnameIx, new DStringGDL(segmentName));
	}
    static int GET_OS_HANDLE=e->KeywordIx("GET_OS_HANDLE");
	if (e->WriteableKeywordPresent(GET_OS_HANDLE)) {
	  if (sysv) e->SetKW(GET_OS_HANDLE, new DLongGDL(sysv_hdle));
	  else e->SetKW(GET_OS_HANDLE, new DStringGDL(osHandle));
	}
}

  void shmunmap_pro(EnvT* e) {
	std::string segmentName;
	SizeT np = e->NParam(1);
	e->AssureStringScalarPar(0, segmentName);
	if (segmentName.size() == 0) e->Throw("Null string not allowed in this context: "+e->GetParString(0)+".");
	shmListIter i = shmList.find(segmentName);
	if (i != shmList.end()) { 
	  //mark as candidate for deletion
	  (*i).second.flags |= DELETE_PENDING;
	} else e->Throw("Shared Memory Segment not found: " + segmentName + ".");
	//if candidate for deletion and nothing referencing it, delete right now:
	if ((*i).second.refcount==0) {
	  if ((*i).second.flags & USE_SYSV == USE_SYSV) {
		int result=shmdt((*i).second.mapped_address);
		if (result ==-1) e->Throw("SYSV Shared Memory Segment " + segmentName + " Unmapping unsucessfull, reason: "+ std::string(strerror(errno))+".");
	  } else {
		int result=munmap((*i).second.mapped_address,(*i).second.length); //unmap
		if (result !=0) e->Throw("Shared Memory Segment " + segmentName + " Unmapping unsucessfull, reason: "+ std::string(strerror(errno))+".");
		if (((*i).second.flags & DESTROY_SEGMENT_ON_UNMAP)==DESTROY_SEGMENT_ON_UNMAP)  shm_unlink((*i).second.osHandle.c_str());
	  }
	  shmList.erase(i);
	}
 };

  BaseGDL* shmvar_fun(EnvT* e) {
	dimension dim;
	int type;
	std::string segmentName = "";
	bool dimNotSet = get_shm_common_keywords(e, segmentName, dim, type);
	shmListIter i = shmList.find(segmentName);
	if (i != shmList.end()) {
	  
	  // if shmap is to be deleted, must not appear as existing
	  if (((*i).second.flags & DELETE_PENDING)==DELETE_PENDING)  e->Throw("Specified shared memory segment pending unmap operation: " + segmentName + ".");
	  if (type == 0) type=(*i).second.type;
	  if (dimNotSet) dim=(*i).second.dim;
	  if (dim.NDimElements()*(atomSize[type]) > (*i).second.length) e->Throw("Requested variable is too long for the underlying shared memory segment: " + segmentName + ".");

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
	  var->SetBuffer((*i).second.mapped_address);
	  var->SetShared(); //necessary!
	  (*i).second.refcount++;
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
  
  void shm_print_help_item(std::ostream& ostr, DString name, dimension dim, int type, DString parString, bool doIndentation = false)
  {
    if (doIndentation) ostr << "   ";

    // Name display
    ostr.width(16);
    ostr << std::left << name;
    if (name.length() >= 16) {
      ostr << '\n'; // for cmsv compatible output (uses help,OUTPUT)
      ostr.width(doIndentation ? 19 : 16);
      ostr << "";
    }

    ostr.width(10);
    ostr <<  atomName[type] << std::right;
    if (!doIndentation) ostr << "= ";
    
	ostr << parString << " ";
    // Dimension display
    ostr << dim ;

    // End of line
    ostr << '\n';
  }

  void help_shared(EnvT* e, std::ostream& ostr) {
	for (shmListIter it=shmList.begin(); it!=shmList.end(); ++it) {
	  DString text1="<";
	  if (((*it).second.flags & IS_PRIVATE)==IS_PRIVATE) text1+="Private";
	  if (((*it).second.flags & MAPPEDFILE)==MAPPEDFILE) {text1+="MappedFile";}
	  else if (((*it).second.flags & USE_SYSV)==USE_SYSV) {text1+="SysV";}
	  else text1+="Posix";
	  text1+="(";
	  DString text2="";
	  if(((*it).second.flags & DESTROY_SEGMENT_ON_UNMAP)==DESTROY_SEGMENT_ON_UNMAP) text2="DestroyOnUnmap, ";
	  if(((*it).second.flags & DELETE_PENDING)==DELETE_PENDING) text2+="UnmapPending, ";
	  shm_print_help_item(ostr, it->first, (*it).second.dim, (*it).second.type, DString(text1+(*it).second.osHandle + 
		  "), Offset("+i2s((*it).second.offset)+"), "+text2+"Refcnt("+i2s((*it).second.refcount)+")>"));
	}
	return;
  }
  void help_par_shared(BaseGDL* var, std::ostream& ostr) {
	void* pointer=var->DataAddr(); //the mapped address
	for (shmListIter it=shmList.begin(); it!=shmList.end(); ++it) {
	  void* pointed=(*it).second.mapped_address;
	  if (pointer == pointed) {
		ostr << "SharedMemory<"<<it->first<<"> ";
	  }
	}
	return;
  }
  //called from system when deleting a mapped variable
  void shm_unreference(BaseGDL* var){
	void* pointer=var->DataAddr(); //the mapped address
	for (shmListIter i=shmList.begin(); i!=shmList.end(); ++i) {
	  void* pointed=(*i).second.mapped_address;
	  if (pointer == pointed) {
		(*i).second.refcount--;
		if ((((*i).second.flags & DELETE_PENDING)==DELETE_PENDING) && (*i).second.refcount<1) {
		  //no more reference, if shmap is pending delete, delete it.
		  //if candidate for deletion and nothing referencing, delete right now:
		  if ((*i).second.flags & USE_SYSV == USE_SYSV) {
			int result = shmdt((*i).second.mapped_address);
			if (result == -1) Warning("SYSV Shared Memory Segment " + (*i).first  + " Unmapping unsucessfull after deleting last mapped variable, reason: " + std::string(strerror(errno)) + ".");
		  } else {
			int result = munmap((*i).second.mapped_address, (*i).second.length); //unmap
			if (result != 0) Warning("Shared Memory Segment " + (*i).first  + " Unmapping unsucessfull after deleting last mapped variable, reason: " + std::string(strerror(errno)) + ".");
			if (((*i).second.flags & DESTROY_SEGMENT_ON_UNMAP) == DESTROY_SEGMENT_ON_UNMAP) shm_unlink((*i).second.osHandle.c_str());
		  }
		  shmList.erase(i);
		}
		return;
	  }
	}
  }
} // namespace
#endif
