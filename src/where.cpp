/***************************************************************************
                          where.cpp  -  where(), defines all datatypes
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

#include "nullgdl.hpp"
#include "dstructgdl.hpp"
#include "dinterpreter.hpp"

#define Sp SpDByte
#include "tagwhere_inc.cpp"
#include "where_inc.cpp"
#undef Sp

#define Sp SpDInt
#include "tagwhere_inc.cpp"
#include "where_inc.cpp"
#undef Sp

#define Sp SpDUInt
#include "tagwhere_inc.cpp"
#include "where_inc.cpp"
#undef Sp

#define Sp SpDLong
#include "tagwhere_inc.cpp"
#include "where_inc.cpp"
#undef Sp

#define Sp SpDULong
#include "tagwhere_inc.cpp"
#include "where_inc.cpp"
#undef Sp

#define Sp SpDLong64
#include "tagwhere_inc.cpp"
#include "where_inc.cpp"
#undef Sp

#define Sp SpDULong64
#include "tagwhere_inc.cpp"
#include "where_inc.cpp"
#undef Sp

template<>
DByte* Data_<SpDPtr>::TagWhere(SizeT& n) {
  throw GDLException("Pointer expression not allowed in this context.");
}

template<>
DByte* Data_<SpDObj>::TagWhere(SizeT& n) {
  throw GDLException("Object expression not allowed in this context.");
}

template<>
DLong* Data_<SpDPtr>::Where(bool comp, SizeT& n) {
  throw GDLException("Pointer expression not allowed in this context.");
}

template<>
DLong* Data_<SpDObj>::Where(bool comp, SizeT& n) {
  throw GDLException("Object expression not allowed in this context.");
}

template<>
DByte* Data_<SpDFloat>::TagWhere(SizeT& n) {
  SizeT nEl = N_Elements();
  DByte* ixList = new DByte[ nEl];
  SizeT count = 0;
#pragma omp parallel reduction(+:count) if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
#pragma omp for 
    for (SizeT i = 0; i < nEl; ++i)
    {
      DByte tmp = (dd[i] != 0.0f);
      ixList[i] = tmp;
      count += tmp;
    }
  }
  n = count;
  return ixList;
}

template<>
DByte* Data_<SpDDouble>::TagWhere(SizeT& n) {
  SizeT nEl = N_Elements();
  DByte* ixList = new DByte[ nEl];
  SizeT count = 0;
#pragma omp parallel reduction(+:count) if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
#pragma omp for 
    for (SizeT i = 0; i < nEl; ++i)
    {
      DByte tmp = (dd[i] != 0.0);
      ixList[i] = tmp;
      count += tmp;
    }
  }
  n = count;
  return ixList;
}

template<>
DByte* Data_<SpDString>::TagWhere(SizeT& n) {
  SizeT nEl = N_Elements();
  DByte* ixList = new DByte[ nEl];
  SizeT count = 0;
#pragma omp parallel reduction(+:count) if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
#pragma omp for 
    for (SizeT i = 0; i < nEl; ++i)
    {
      DByte tmp = ((*this)[i] != "");
      ixList[i] = tmp;
      count += tmp;
    }
  }
  n = count;
  return ixList;
}

template<>
DByte* Data_<SpDComplex>::TagWhere(SizeT& n) {
  SizeT nEl = N_Elements();
  DByte* ixList = new DByte[ nEl];
  SizeT count = 0;
#pragma omp parallel reduction(+:count) if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
#pragma omp for 
    for (SizeT i = 0; i < nEl; ++i)
    {
      DFloat re = (*this)[i].real();
      DFloat im = (*this)[i].imag();
      DByte tmp = (re != 0.0f || im != 0.0f);
      ixList[i] = tmp;
      count += tmp;
    }
  }
  n = count;
  return ixList;
}

template<>
DByte* Data_<SpDComplexDbl>::TagWhere(SizeT& n) {
  SizeT nEl = N_Elements();
  DByte* ixList = new DByte[ nEl];
  SizeT count = 0;
#pragma omp parallel reduction(+:count) if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
#pragma omp for 
    for (SizeT i = 0; i < nEl; ++i)
    {
      DDouble re = (*this)[i].real();
      DDouble im = (*this)[i].imag();
      DByte tmp = (re != 0.0 || im != 0.0);
      ixList[i] = tmp;
      count += tmp;
    }
  }
  n = count;
  return ixList;
}

// for WHERE, integers, also ptr and object

template<>
DLong* Data_<SpDFloat>::Where(bool comp, SizeT& n) {
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if (comp)
  {
    SizeT nIx = nEl;
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for (SizeT i = 0; i < nEl; ++i)
      {
        if ((*this)[i] != 0.0f)
        {
          ixList[ count++] = i;
        } else
        {
          ixList[ --nIx] = i;
        }
      }
    } // omp
  } else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
    //#pragma omp for
    for (SizeT i = 0; i < nEl; ++i)
      if ((*this)[i] != 0.0f)
      {
        ixList[ count++] = i;
      }
  } // omp
  n = count;
  return ixList;
}

template<>
DLong* Data_<SpDDouble>::Where(bool comp, SizeT& n) {
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if (comp)
  {
    SizeT nIx = nEl;
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for (SizeT i = 0; i < nEl; ++i)
      {
        if ((*this)[i] != 0.0)
        {
          ixList[ count++] = i;
        } else
        {
          ixList[ --nIx] = i;
        }
      }
    } // omp
  } else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
    //#pragma omp for
    for (SizeT i = 0; i < nEl; ++i)
      if ((*this)[i] != 0.0)
      {
        ixList[ count++] = i;
      }
  } // omp
  n = count;
  return ixList;
}

template<>
DLong* Data_<SpDString>::Where(bool comp, SizeT& n) {
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if (comp)
  {
    SizeT nIx = nEl;
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for (SizeT i = 0; i < nEl; ++i)
      {
        if ((*this)[i] != "")
        {
          ixList[ count++] = i;
        } else
        {
          ixList[ --nIx] = i;
        }
      }
    } // omp
  } else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
    //#pragma omp for
    for (SizeT i = 0; i < nEl; ++i)
      if ((*this)[i] != "")
      {
        ixList[ count++] = i;
      }
  } // omp
  n = count;
  return ixList;
}

template<>
DLong* Data_<SpDComplex>::Where(bool comp, SizeT& n) {
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if (comp)
  {
    SizeT nIx = nEl;
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for (SizeT i = 0; i < nEl; ++i)
      {
        if ((*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0)
        {
          ixList[ count++] = i;
        } else
        {
          ixList[ --nIx] = i;
        }
      }
    } // omp
  } else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
    //#pragma omp for
    for (SizeT i = 0; i < nEl; ++i)
      if ((*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0)
      {
        ixList[ count++] = i;
      }
  } // omp
  n = count;
  return ixList;
}

template<>
DLong* Data_<SpDComplexDbl>::Where(bool comp, SizeT& n) {
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if (comp)
  {
    SizeT nIx = nEl;
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for (SizeT i = 0; i < nEl; ++i)
      {
        if ((*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0)
        {
          ixList[ count++] = i;
        } else
        {
          ixList[ --nIx] = i;
        }
      }
    } // omp
  } else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
    //#pragma omp for
    for (SizeT i = 0; i < nEl; ++i)
      if ((*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0)
      {
        ixList[ count++] = i;
      }
  } // omp
  n = count;
  return ixList;
}

#include "where.hpp"
namespace lib {

  BaseGDL* where_fun(EnvT* e) {
    SizeT nParam = e->NParam(1); //, "WHERE");

    BaseGDL* p0p = e->GetParDefined(0); //, "WHERE");

    SizeT nEl = p0p->N_Elements();

    SizeT count = 0;

    DByte* p0 = p0p->TagWhere(count);
    ArrayGuard<DByte> guardp0(p0); //delete on exit
    SizeT nCount = nEl - count;

    static int nullIx = e->KeywordIx("NULL");
    bool nullKW = e->KeywordSet(nullIx);

    // the following is a tentative to parallelize the loop below.
    // It is not effeicient because cache misses when adressing portions of 'yes' or 'no'
    // non openmp code is better optimised by compiler.

    //    int nchunk=CpuTPOOL_NTHREADS*4;
    //    SizeT countyes[nchunk]={0};
    //    SizeT countno[nchunk]={0};
    //    SizeT startyes[nchunk]={0};
    //    SizeT startno[nchunk]={0};
    //    SizeT chunksize=nEl/nchunk;
    //    for (int iloop=0; iloop<nchunk; ++iloop) {
    //      for (SizeT j=iloop*chunksize; j<(iloop+1)*chunksize; ++j) {
    //        countyes[iloop]+=p0[j];
    //      }
    //      countno[iloop]=chunksize-countyes[iloop];
    //    }
    //    for (int iloop=1; iloop<nchunk; ++iloop) {
    //      startyes[iloop]=startyes[iloop-1]+countyes[iloop-1];
    //      startno[iloop]=startno[iloop-1]+countno[iloop-1];
    //    }
    //    
    //    DLong* distributed[nchunk][2];
    //    DLongGDL* yes;
    //    DLong* zyes;
    //    if (count > 0) {
    //      yes = new DLongGDL(dimension(count),BaseGDL::NOZERO);
    //      zyes=(DLong*)yes->DataAddr();
    //      for (int iloop=0; iloop<nchunk; ++iloop) distributed[iloop][1] = &(zyes[startyes[iloop]]);
    //    }
    //    DLongGDL* no;
    //    DLong* zno;
    //    if (nCount > 0) {
    //      no = new DLongGDL(dimension(nCount),BaseGDL::NOZERO);
    //      zno=(DLong*)no->DataAddr();
    //      for (int iloop=0; iloop<nchunk; ++iloop) distributed[iloop][0] = &(zno[startno[iloop]]);
    //    }
    //    SizeT districount[nchunk][2]={0};
    //    
    //    //distribute accordingly! removing the if clause makes the loop 2 times faster.
    //#pragma omp parallel num_threads(nchunk) firstprivate(nchunk,chunksize) shared(districount,distributed,p0) 
    //  {
    //#pragma omp for schedule (static)
    //    for (int iloop=0; iloop<nchunk; ++iloop) {
    //      SizeT j=iloop*chunksize;
    //      for (; j<(iloop+1)*chunksize;) {
    //         distributed[iloop][p0[j]][districount[iloop][p0[j]]++] = j++;
    //      }
    //    }
    //  }

    DLong * distributed[2];
    DLongGDL* yes;
    if (count > 0)
    {
      yes = new DLongGDL(dimension(count), BaseGDL::NOZERO);
      distributed[1] = (DLong*) yes->DataAddr();
    }
    DLongGDL* no;
    if (nCount > 0)
    {
      no = new DLongGDL(dimension(nCount), BaseGDL::NOZERO);
      distributed[0] = (DLong*) no->DataAddr();
    }

    SizeT districount[2] = {0, 0};

    //distribute accordingly! 
    DByte tmp;
    for (SizeT i = 0; i < nEl; ++i)
    {
      tmp = p0[i];
      distributed[tmp][districount[tmp]] = i;
      districount[tmp]++;
    }

    if (e->KeywordPresent(0)) // COMPLEMENT
    {
      if (nCount == 0)
      { //'no' is not malloc'ed. 
        if (nullKW)
          e->SetKW(0, NullGDL::GetSingleInstance());
        else
          e->SetKW(0, new DLongGDL(-1));
      } else
      {
        e->SetKW(0, no);
      }
    } else
    {
      if (nCount > 0) GDLDelete(no); //tidy!
    }

    if (e->KeywordPresent(1)) // NCOMPLEMENT
    {
      e->SetKW(1, new DLongGDL(nCount));
    }

    if (nParam == 2)
    {
      e->SetPar(1, new DLongGDL(count));
    }
    //The system variable !ERR is set to the number of nonzero elements for compatibility with old versions of IDL
    DVar *err = FindInVarList(sysVarList, "ERR");
    (static_cast<DLongGDL*> (err->Data()))[0] = count;
    if (count == 0)
    {
      if (nullKW)
        return NullGDL::GetSingleInstance();
      return new DLongGDL(-1);
    }

    return yes;
  }
}
