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

#include <sys/mman.h>
#include "shm.hpp"
#include "basic_fun.hpp" //for arr()
static std::map<DString, std::pair<int, void*>> shmList;

enum { BYTE=0,COMPLEX,DCOMPLEX,DOUBLE,FLOAT,INTEGER,L64,LONG,UINT,UL64,ULONG, DIMENSION,SIZE, TEMPLATE, TYPE} common_options_shm;
static const int atom[12]={0,1,4,8,8,4,2,8,4,2,8,4};
namespace lib {
  void shmmap_pro(EnvT* e) {
    SizeT np = e->NParam(0);
	std::string segmentName="";
	if (np!=0) e->AssureStringScalarPar(0,segmentName);
	// DIMENSION
	dimension dim;
	if (e->KeywordSet(DIMENSION)) {
	  BaseGDL* pDim=e->GetKW(DIMENSION);
	  dim=pDim->Dim();
	} 
	else if (np>1) arr(e, dim, 1); //get all dims starting at 1
	else if (!e->KeywordSet(TEMPLATE)) e->Throw("Unable to set Dimension(s).");
	
	if (dim.NDimElements() == 0) e->Throw("Expression must be an array in this context: <Shared Memory Segment>.");
	//TYPE
    int type = GDL_FLOAT; //by default
	if (e->KeywordSet(TYPE)) {
	  DLong typeget=type;
	  e->AssureLongScalarKW(TYPE,typeget);
	  type=typeget;
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
	//TEMPLATE wins ALL
	if (e->KeywordSet(TEMPLATE)) {
	  BaseGDL* pTemp=e->GetKW(TEMPLATE);
	  dim=pTemp->Dim();
	  type=pTemp->Type();
	}
	//check type
	if (!gdl_type_lookup::IsConvertableType[type]) { //problems begins
	  e->Throw("Objects, Pointers and Structures not allowed in this context.");
	}
	size_t length=dim.NDimElements()*(atom[type]);
	int prot=PROT_READ|PROT_WRITE;
	int flags=MAP_SHARED|MAP_ANONYMOUS;
	void* res=mmap(NULL, length, prot, flags, 0, 0);
    if (segmentName.size()==0) segmentName="IDL_SHM_"+i2s(res)+"_"+i2s(type);
	shmList.insert(std::pair<DString, std::pair<int, void*>>(segmentName,std::pair<int, void*>(type,res)));
	static int getnameIx=e->KeywordIx("GET_NAME");
	if (e->WriteableKeywordPresent(getnameIx)) {
	  e->SetKW(getnameIx, new DStringGDL(segmentName));
	}
  }
  void shmunmap_pro(EnvT* e){};
  BaseGDL* shmvar_fun(EnvT* e){return new DIntGDL(0);}
  BaseGDL* shmdebug_fun(EnvT* e){return new DIntGDL(0);}

} // namespace
