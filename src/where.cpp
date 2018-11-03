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
#include "dinterpreter.hpp" //for sysVarList() 


//We create 'on the spot' arrays that must be aligned. this is the purpose of the gdl..lloc functions.
#define MALLOC gdlAlignedMalloc
#define REALLOC gdlAlignedRealloc 
#define FREE gdlAlignedFree

#define Sp SpDByte
#include "where_inc.cpp"
#undef Sp

#define Sp SpDInt
#include "where_inc.cpp"
#undef Sp

#define Sp SpDUInt
#include "where_inc.cpp"
#undef Sp

#define Sp SpDLong
#include "where_inc.cpp"
#undef Sp

#define Sp SpDULong
#include "where_inc.cpp"
#undef Sp

#define Sp SpDLong64
#include "where_inc.cpp"
#undef Sp

#define Sp SpDULong64
#include "where_inc.cpp"
#undef Sp

#define Sp SpDFloat
#include "where_inc.cpp"
#undef Sp

#define Sp SpDDouble
#include "where_inc.cpp"
#undef Sp

template<>
void Data_<SpDString>::Where(DLong* &ret, SizeT &passed_count, bool comp, DLong* &comp_ret) {
  SizeT nEl=this->N_Elements();
 //code is optimized for 1 thread (no thread) and for presence or absence of complement.
  int nchunk=(nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))?CpuTPOOL_NTHREADS:1;
  if (comp) {
    if (nchunk==1) {
      DLong* yes = (DLong*)MALLOC(nEl*sizeof(DLong)); 
      DLong* no = (DLong*)MALLOC(nEl*sizeof(DLong)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT countyes=0;
      SizeT countno=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i]!="");
        yes[countyes]=i;
        no[countno]=i;
        countyes+=tmp;
        countno+=(!tmp);
      }
      passed_count=countyes;
      if (countyes) ret=(DLong*)REALLOC(yes,countyes * sizeof (DLong)); else {FREE(yes); ret=NULL;}
      if (countno) comp_ret=(DLong*)REALLOC(no,countno * sizeof (DLong)); else {FREE(no); comp_ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong* partyes[nchunk];
      DLong* partno[nchunk];
      SizeT partialCountYes[nchunk];
      SizeT partialCountNo[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        partyes[thread_id] = (DLong*)MALLOC(space*sizeof(DLong)); 
        partno[thread_id] = (DLong*)MALLOC(space*sizeof(DLong)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count_yes=0;
        SizeT local_count_no=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i]!="");
          partyes[thread_id][local_count_yes]=i;
          partno[thread_id][local_count_no]=i;
          local_count_yes+=tmp;
          local_count_no+=(!tmp);
        }
        partialCountYes[thread_id]=local_count_yes;
        partialCountNo[thread_id]=local_count_no;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT countyes=0;
      SizeT countno=0;
      for (int iloop=0; iloop<nchunk; ++iloop) countyes+=partialCountYes[iloop];
      for (int iloop=0; iloop<nchunk; ++iloop) countno+=partialCountNo[iloop];
      // allocate final result
      if (countyes>0) {
        ret=(DLong*)MALLOC(countyes * sizeof (DLong)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountYes[iloop]; ++jj) ret[running_index++]=partyes[iloop][jj];
        }
      }
      if (countno>0) {
        comp_ret=(DLong*)MALLOC(countno * sizeof (DLong)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountNo[iloop]; ++jj) comp_ret[running_index++]=partno[iloop][jj];
        }
      }
      passed_count=countyes;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(partyes[iloop]);
        FREE(partno[iloop]);
      }
    }
  } else {
    if (nchunk==1) {
      DLong* yes = (DLong*)MALLOC(nEl*sizeof(DLong)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT count=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i]!="");
        yes[count]=i;
        count+=tmp;
      }
      passed_count=count;
      if (count) ret=(DLong*)REALLOC(yes,count * sizeof (DLong)); else {FREE(yes); ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong* part[nchunk];
      SizeT partialCount[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        part[thread_id] = (DLong*)MALLOC(space*sizeof(DLong)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i]!="");
          part[thread_id][local_count]=i;
          local_count+=tmp;
        }
        partialCount[thread_id]=local_count;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT count=0;
      for (int iloop=0; iloop<nchunk; ++iloop) count+=partialCount[iloop];
      // allocate final result
      if (count > 0) {
        ret=(DLong*)MALLOC(count * sizeof (DLong)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCount[iloop]; ++jj) ret[running_index++]=part[iloop][jj];
        }
      }
      passed_count=count;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(part[iloop]);
      }
    }
  }
}

template<>
void Data_<SpDString>::Where(DLong64* &ret, SizeT &passed_count, bool comp, DLong64* &comp_ret) {
  SizeT nEl=this->N_Elements();
 //code is optimized for 1 thread (no thread) and for presence or absence of complement.
  int nchunk=(nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))?CpuTPOOL_NTHREADS:1;
  if (comp) {
    if (nchunk==1) {
      DLong64* yes = (DLong64*)MALLOC(nEl*sizeof(DLong64)); 
      DLong64* no = (DLong64*)MALLOC(nEl*sizeof(DLong64)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT countyes=0;
      SizeT countno=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i]!="");
        yes[countyes]=i;
        no[countno]=i;
        countyes+=tmp;
        countno+=(!tmp);
      }
      passed_count=countyes;
      if (countyes) ret=(DLong64*)REALLOC(yes,countyes * sizeof (DLong64)); else {FREE(yes); ret=NULL;}
      if (countno) comp_ret=(DLong64*)REALLOC(no,countno * sizeof (DLong64)); else {FREE(no); comp_ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong64* partyes[nchunk];
      DLong64* partno[nchunk];
      SizeT partialCountYes[nchunk];
      SizeT partialCountNo[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong64);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        partyes[thread_id] = (DLong64*)MALLOC(space*sizeof(DLong64)); 
        partno[thread_id] = (DLong64*)MALLOC(space*sizeof(DLong64)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count_yes=0;
        SizeT local_count_no=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i]!="");
          partyes[thread_id][local_count_yes]=i;
          partno[thread_id][local_count_no]=i;
          local_count_yes+=tmp;
          local_count_no+=(!tmp);
        }
        partialCountYes[thread_id]=local_count_yes;
        partialCountNo[thread_id]=local_count_no;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT countyes=0;
      SizeT countno=0;
      for (int iloop=0; iloop<nchunk; ++iloop) countyes+=partialCountYes[iloop];
      for (int iloop=0; iloop<nchunk; ++iloop) countno+=partialCountNo[iloop];
      // allocate final result
      if (countyes>0) {
        ret=(DLong64*)MALLOC(countyes * sizeof (DLong64)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountYes[iloop]; ++jj) ret[running_index++]=partyes[iloop][jj];
        }
      }
      if (countno>0) {
        comp_ret=(DLong64*)MALLOC(countno * sizeof (DLong64)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountNo[iloop]; ++jj) comp_ret[running_index++]=partno[iloop][jj];
        }
      }
      passed_count=countyes;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(partyes[iloop]);
        FREE(partno[iloop]);
      }
    }
  } else {
    if (nchunk==1) {
      DLong64* yes = (DLong64*)MALLOC(nEl*sizeof(DLong64)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT count=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i]!="");
        yes[count]=i;
        count+=tmp;
      }
      passed_count=count;
      if (count) ret=(DLong64*)REALLOC(yes,count * sizeof (DLong64)); else {FREE(yes); ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong64* part[nchunk];
      SizeT partialCount[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong64);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        part[thread_id] = (DLong64*)MALLOC(space*sizeof(DLong64)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i]!="");
          part[thread_id][local_count]=i;
          local_count+=tmp;
        }
        partialCount[thread_id]=local_count;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT count=0;
      for (int iloop=0; iloop<nchunk; ++iloop) count+=partialCount[iloop];
      // allocate final result
      if (count > 0) {
        ret=(DLong64*)MALLOC(count * sizeof (DLong64)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCount[iloop]; ++jj) ret[running_index++]=part[iloop][jj];
        }
      }
      passed_count=count;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(part[iloop]);
      }
    }
  }
}

template<>
void Data_<SpDComplex>::Where(DLong* &ret, SizeT &passed_count, bool comp, DLong* &comp_ret) {
  SizeT nEl=this->N_Elements();
 //code is optimized for 1 thread (no thread) and for presence or absence of complement.
  int nchunk=(nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))?CpuTPOOL_NTHREADS:1;
  if (comp) {
    if (nchunk==1) {
      DLong* yes = (DLong*)MALLOC(nEl*sizeof(DLong)); 
      DLong* no = (DLong*)MALLOC(nEl*sizeof(DLong)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT countyes=0;
      SizeT countno=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
        yes[countyes]=i;
        no[countno]=i;
        countyes+=tmp;
        countno+=(!tmp);
      }
      passed_count=countyes;
      if (countyes) ret=(DLong*)REALLOC(yes,countyes * sizeof (DLong)); else {FREE(yes); ret=NULL;}
      if (countno) comp_ret=(DLong*)REALLOC(no,countno * sizeof (DLong)); else {FREE(no); comp_ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong* partyes[nchunk];
      DLong* partno[nchunk];
      SizeT partialCountYes[nchunk];
      SizeT partialCountNo[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        partyes[thread_id] = (DLong*)MALLOC(space*sizeof(DLong)); 
        partno[thread_id] = (DLong*)MALLOC(space*sizeof(DLong)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count_yes=0;
        SizeT local_count_no=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
          partyes[thread_id][local_count_yes]=i;
          partno[thread_id][local_count_no]=i;
          local_count_yes+=tmp;
          local_count_no+=(!tmp);
        }
        partialCountYes[thread_id]=local_count_yes;
        partialCountNo[thread_id]=local_count_no;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT countyes=0;
      SizeT countno=0;
      for (int iloop=0; iloop<nchunk; ++iloop) countyes+=partialCountYes[iloop];
      for (int iloop=0; iloop<nchunk; ++iloop) countno+=partialCountNo[iloop];
      // allocate final result
      if (countyes>0) {
        ret=(DLong*)MALLOC(countyes * sizeof (DLong)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountYes[iloop]; ++jj) ret[running_index++]=partyes[iloop][jj];
        }
      }
      if (countno>0) {
        comp_ret=(DLong*)MALLOC(countno * sizeof (DLong)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountNo[iloop]; ++jj) comp_ret[running_index++]=partno[iloop][jj];
        }
      }
      passed_count=countyes;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(partyes[iloop]);
        FREE(partno[iloop]);
      }
    }
  } else {
    if (nchunk==1) {
      DLong* yes = (DLong*)MALLOC(nEl*sizeof(DLong)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT count=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
        yes[count]=i;
        count+=tmp;
      }
      passed_count=count;
      if (count) ret=(DLong*)REALLOC(yes,count * sizeof (DLong)); else {FREE(yes); ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong* part[nchunk];
      SizeT partialCount[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        part[thread_id] = (DLong*)MALLOC(space*sizeof(DLong)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
          part[thread_id][local_count]=i;
          local_count+=tmp;
        }
        partialCount[thread_id]=local_count;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT count=0;
      for (int iloop=0; iloop<nchunk; ++iloop) count+=partialCount[iloop];
      // allocate final result
      if (count > 0) {
        ret=(DLong*)MALLOC(count * sizeof (DLong)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCount[iloop]; ++jj) ret[running_index++]=part[iloop][jj];
        }
      }
      passed_count=count;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(part[iloop]);
      }
    }
  }
}

template<>
void Data_<SpDComplex>::Where(DLong64* &ret, SizeT &passed_count, bool comp, DLong64* &comp_ret) {
  SizeT nEl=this->N_Elements();
 //code is optimized for 1 thread (no thread) and for presence or absence of complement.
  int nchunk=(nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))?CpuTPOOL_NTHREADS:1;
  if (comp) {
    if (nchunk==1) {
      DLong64* yes = (DLong64*)MALLOC(nEl*sizeof(DLong64)); 
      DLong64* no = (DLong64*)MALLOC(nEl*sizeof(DLong64)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT countyes=0;
      SizeT countno=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
        yes[countyes]=i;
        no[countno]=i;
        countyes+=tmp;
        countno+=(!tmp);
      }
      passed_count=countyes;
      if (countyes) ret=(DLong64*)REALLOC(yes,countyes * sizeof (DLong64)); else {FREE(yes); ret=NULL;}
      if (countno) comp_ret=(DLong64*)REALLOC(no,countno * sizeof (DLong64)); else {FREE(no); comp_ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong64* partyes[nchunk];
      DLong64* partno[nchunk];
      SizeT partialCountYes[nchunk];
      SizeT partialCountNo[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong64);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        partyes[thread_id] = (DLong64*)MALLOC(space*sizeof(DLong64)); 
        partno[thread_id] = (DLong64*)MALLOC(space*sizeof(DLong64)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count_yes=0;
        SizeT local_count_no=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
          partyes[thread_id][local_count_yes]=i;
          partno[thread_id][local_count_no]=i;
          local_count_yes+=tmp;
          local_count_no+=(!tmp);
        }
        partialCountYes[thread_id]=local_count_yes;
        partialCountNo[thread_id]=local_count_no;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT countyes=0;
      SizeT countno=0;
      for (int iloop=0; iloop<nchunk; ++iloop) countyes+=partialCountYes[iloop];
      for (int iloop=0; iloop<nchunk; ++iloop) countno+=partialCountNo[iloop];
      // allocate final result
      if (countyes>0) {
        ret=(DLong64*)MALLOC(countyes * sizeof (DLong64)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountYes[iloop]; ++jj) ret[running_index++]=partyes[iloop][jj];
        }
      }
      if (countno>0) {
        comp_ret=(DLong64*)MALLOC(countno * sizeof (DLong64)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountNo[iloop]; ++jj) comp_ret[running_index++]=partno[iloop][jj];
        }
      }
      passed_count=countyes;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(partyes[iloop]);
        FREE(partno[iloop]);
      }
    }
  } else {
    if (nchunk==1) {
      DLong64* yes = (DLong64*)MALLOC(nEl*sizeof(DLong64)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT count=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
        yes[count]=i;
        count+=tmp;
      }
      passed_count=count;
      if (count) ret=(DLong64*)REALLOC(yes,count * sizeof (DLong64)); else {FREE(yes); ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong64* part[nchunk];
      SizeT partialCount[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong64);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        part[thread_id] = (DLong64*)MALLOC(space*sizeof(DLong64)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
          part[thread_id][local_count]=i;
          local_count+=tmp;
        }
        partialCount[thread_id]=local_count;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT count=0;
      for (int iloop=0; iloop<nchunk; ++iloop) count+=partialCount[iloop];
      // allocate final result
      if (count > 0) {
        ret=(DLong64*)MALLOC(count * sizeof (DLong64)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCount[iloop]; ++jj) ret[running_index++]=part[iloop][jj];
        }
      }
      passed_count=count;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(part[iloop]);
      }
    }
  }
}

template<>
void Data_<SpDComplexDbl>::Where(DLong* &ret, SizeT &passed_count, bool comp, DLong* &comp_ret) {
  SizeT nEl=this->N_Elements();
 //code is optimized for 1 thread (no thread) and for presence or absence of complement.
  int nchunk=(nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))?CpuTPOOL_NTHREADS:1;
  if (comp) {
    if (nchunk==1) {
      DLong* yes = (DLong*)MALLOC(nEl*sizeof(DLong)); 
      DLong* no = (DLong*)MALLOC(nEl*sizeof(DLong)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT countyes=0;
      SizeT countno=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
        yes[countyes]=i;
        no[countno]=i;
        countyes+=tmp;
        countno+=(!tmp);
      }
      passed_count=countyes;
      if (countyes) ret=(DLong*)REALLOC(yes,countyes * sizeof (DLong)); else {FREE(yes); ret=NULL;}
      if (countno) comp_ret=(DLong*)REALLOC(no,countno * sizeof (DLong)); else {FREE(no); comp_ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong* partyes[nchunk];
      DLong* partno[nchunk];
      SizeT partialCountYes[nchunk];
      SizeT partialCountNo[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        partyes[thread_id] = (DLong*)MALLOC(space*sizeof(DLong)); 
        partno[thread_id] = (DLong*)MALLOC(space*sizeof(DLong)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count_yes=0;
        SizeT local_count_no=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
          partyes[thread_id][local_count_yes]=i;
          partno[thread_id][local_count_no]=i;
          local_count_yes+=tmp;
          local_count_no+=(!tmp);
        }
        partialCountYes[thread_id]=local_count_yes;
        partialCountNo[thread_id]=local_count_no;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT countyes=0;
      SizeT countno=0;
      for (int iloop=0; iloop<nchunk; ++iloop) countyes+=partialCountYes[iloop];
      for (int iloop=0; iloop<nchunk; ++iloop) countno+=partialCountNo[iloop];
      // allocate final result
      if (countyes>0) {
        ret=(DLong*)MALLOC(countyes * sizeof (DLong)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountYes[iloop]; ++jj) ret[running_index++]=partyes[iloop][jj];
        }
      }
      if (countno>0) {
        comp_ret=(DLong*)MALLOC(countno * sizeof (DLong)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountNo[iloop]; ++jj) comp_ret[running_index++]=partno[iloop][jj];
        }
      }
      passed_count=countyes;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(partyes[iloop]);
        FREE(partno[iloop]);
      }
    }
  } else {
    if (nchunk==1) {
      DLong* yes = (DLong*)MALLOC(nEl*sizeof(DLong)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT count=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
        yes[count]=i;
        count+=tmp;
      }
      passed_count=count;
      if (count) ret=(DLong*)REALLOC(yes,count * sizeof (DLong)); else {FREE(yes); ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong* part[nchunk];
      SizeT partialCount[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        part[thread_id] = (DLong*)MALLOC(space*sizeof(DLong)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
          part[thread_id][local_count]=i;
          local_count+=tmp;
        }
        partialCount[thread_id]=local_count;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT count=0;
      for (int iloop=0; iloop<nchunk; ++iloop) count+=partialCount[iloop];
      // allocate final result
      if (count > 0) {
        ret=(DLong*)MALLOC(count * sizeof (DLong)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCount[iloop]; ++jj) ret[running_index++]=part[iloop][jj];
        }
      }
      passed_count=count;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(part[iloop]);
      }
    }
  }
}

template<>
void Data_<SpDComplexDbl>::Where(DLong64* &ret, SizeT &passed_count, bool comp, DLong64* &comp_ret) {
  SizeT nEl=this->N_Elements();
 //code is optimized for 1 thread (no thread) and for presence or absence of complement.
  int nchunk=(nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))?CpuTPOOL_NTHREADS:1;
  if (comp) {
    if (nchunk==1) {
      DLong64* yes = (DLong64*)MALLOC(nEl*sizeof(DLong64)); 
      DLong64* no = (DLong64*)MALLOC(nEl*sizeof(DLong64)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT countyes=0;
      SizeT countno=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
        yes[countyes]=i;
        no[countno]=i;
        countyes+=tmp;
        countno+=(!tmp);
      }
      passed_count=countyes;
      if (countyes) ret=(DLong64*)REALLOC(yes,countyes * sizeof (DLong64)); else {FREE(yes); ret=NULL;}
      if (countno) comp_ret=(DLong64*)REALLOC(no,countno * sizeof (DLong64)); else {FREE(no); comp_ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong64* partyes[nchunk];
      DLong64* partno[nchunk];
      SizeT partialCountYes[nchunk];
      SizeT partialCountNo[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong64);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        partyes[thread_id] = (DLong64*)MALLOC(space*sizeof(DLong64)); 
        partno[thread_id] = (DLong64*)MALLOC(space*sizeof(DLong64)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count_yes=0;
        SizeT local_count_no=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
          partyes[thread_id][local_count_yes]=i;
          partno[thread_id][local_count_no]=i;
          local_count_yes+=tmp;
          local_count_no+=(!tmp);
        }
        partialCountYes[thread_id]=local_count_yes;
        partialCountNo[thread_id]=local_count_no;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT countyes=0;
      SizeT countno=0;
      for (int iloop=0; iloop<nchunk; ++iloop) countyes+=partialCountYes[iloop];
      for (int iloop=0; iloop<nchunk; ++iloop) countno+=partialCountNo[iloop];
      // allocate final result
      if (countyes>0) {
        ret=(DLong64*)MALLOC(countyes * sizeof (DLong64)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountYes[iloop]; ++jj) ret[running_index++]=partyes[iloop][jj];
        }
      }
      if (countno>0) {
        comp_ret=(DLong64*)MALLOC(countno * sizeof (DLong64)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCountNo[iloop]; ++jj) comp_ret[running_index++]=partno[iloop][jj];
        }
      }
      passed_count=countyes;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(partyes[iloop]);
        FREE(partno[iloop]);
      }
    }
  } else {
    if (nchunk==1) {
      DLong64* yes = (DLong64*)MALLOC(nEl*sizeof(DLong64)); 
      // computational magic of 0 and 1 to keep ony 'good' indexes. 
      SizeT count=0;
      for (SizeT i=0; i<nEl; ++i) {
        bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
        yes[count]=i;
        count+=tmp;
      }
      passed_count=count;
      if (count) ret=(DLong64*)REALLOC(yes,count * sizeof (DLong64)); else {FREE(yes); ret=NULL;}
      return;
    } else {
      SizeT chunksize = nEl / nchunk;
      DLong64* part[nchunk];
      SizeT partialCount[nchunk];
      #pragma omp parallel num_threads(nchunk) //shared(partialCount,part) //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
        int thread_id = currentThreadNumber();
        SizeT start_index, stop_index;
        start_index = thread_id * chunksize;
        if (thread_id != nchunk-1) //robust wrt. use of threads or not.
        {
          stop_index = start_index + chunksize;
        } else
        {
          stop_index = nEl;
        }

        SizeT space=(stop_index-start_index)*sizeof (DLong64);
        SizeT i=0;
        //compute a [0] or [1] serie
        // allocate thread's subset of final result
        part[thread_id] = (DLong64*)MALLOC(space*sizeof(DLong64)); 
        // computational magic of 0 and 1 to keep ony 'good' indexes. 
        SizeT local_count=0;
        for (i=start_index; i<stop_index; ++i) {
          bool tmp=((*this)[i].real() && (*this)[i].imag()); //both needed
          part[thread_id][local_count]=i;
          local_count+=tmp;
        }
        partialCount[thread_id]=local_count;
      } //end parallel section
      //total count is sum of partial counts:
      SizeT count=0;
      for (int iloop=0; iloop<nchunk; ++iloop) count+=partialCount[iloop];
      // allocate final result
      if (count > 0) {
        ret=(DLong64*)MALLOC(count * sizeof (DLong64)); //allocate, nozero
        //fill in
        SizeT running_index=0;
        for (int iloop=0; iloop<nchunk; ++iloop) {
          for (SizeT jj=0; jj<partialCount[iloop]; ++jj) ret[running_index++]=part[iloop][jj];
        }
      }
      passed_count=count;
      //free temporary arrays.
      for (int iloop=0; iloop<nchunk; ++iloop) {
        FREE(part[iloop]);
      }
    }
  }
}

template<>
void Data_<SpDPtr>::Where(DLong* &ret, SizeT &passed_count, bool comp, DLong* &comp_ret){
  throw GDLException("Pointer expression not allowed in this context.");
}
template<>
void Data_<SpDPtr>::Where(DLong64* &ret, SizeT &passed_count, bool comp, DLong64* &comp_ret){
  throw GDLException("Pointer expression not allowed in this context.");
}

template<>
void Data_<SpDObj>::Where(DLong* &ret, SizeT &passed_count, bool comp, DLong* &comp_ret){
  throw GDLException("Object expression not allowed in this context.");
}
template<>
void Data_<SpDObj>::Where(DLong64* &ret, SizeT &passed_count, bool comp, DLong64* &comp_ret){
  throw GDLException("Object expression not allowed in this context.");
}


#include "where.hpp"
namespace lib {

  BaseGDL* where_fun(EnvT* e) {
    SizeT nParam = e->NParam(1); //, "WHERE");

    BaseGDL* p0 = e->GetParDefined( 0);//, "WHERE");

    SizeT nEl = p0->N_Elements();

    SizeT count;

    static int nullIx = e->KeywordIx("NULL");
    bool nullKW = e->KeywordSet(nullIx);
    
    static int l64Ix = e->KeywordIx("L64");
    bool doL64 = e->KeywordSet(l64Ix);
    doL64 = ( doL64 || nEl > std::numeric_limits<DLong>::max() ); //not tested!
    if (!doL64) {
      DLong* ret=NULL;
      DLong* comp_ret=NULL;

      p0->Where(ret, count, e->KeywordPresent(0), comp_ret);

      SizeT nCount = nEl - count;

      if (e->KeywordPresent(0)) // COMPLEMENT
      {
        if (nCount == 0) {
          if (nullKW)
            e->SetKW(0, NullGDL::GetSingleInstance());
          else
            e->SetKW(0, new DLongGDL(-1));
        } else {
          DLongGDL* cRet=new DLongGDL(dimension(nCount),BaseGDL::NOALLOC); //danger!!
          cRet->SetBuffer((void*)comp_ret);
          cRet->SetBufferSize(nCount);
          cRet->SetDim(dimension(nCount));
          e->SetKW(0, cRet);
        }
      }

      if (e->KeywordPresent(1)) // NCOMPLEMENT
      {
        e->SetKW(1, new DLongGDL(nCount));
      }

      if (nParam == 2) {
        e->SetPar(1, new DLongGDL(count));
      }
      //The system variable !ERR is set to the number of nonzero elements for compatibility with old versions of IDL
      DVar *err = FindInVarList(sysVarList, "ERR");
      (static_cast<DLongGDL*> (err->Data()))[0] = count;

      if (count == 0) {
        if (nullKW) {
          return NullGDL::GetSingleInstance();
        }
        return new DLongGDL(-1);
      }
      DLongGDL* res=new DLongGDL(dimension(count),BaseGDL::NOALLOC);
      res->SetBuffer((void*)ret);
      res->SetBufferSize(count);
      res->SetDim(dimension(count));
      return res;
    } else {
      DLong64* ret=NULL;
      DLong64* comp_ret=NULL;

      p0->Where(ret, count, e->KeywordPresent(0), comp_ret);

      SizeT nCount = nEl - count;

      if (e->KeywordPresent(0)) // COMPLEMENT
      {
        if (nCount == 0) {
          if (nullKW)
            e->SetKW(0, NullGDL::GetSingleInstance());
          else
            e->SetKW(0, new DLongGDL(-1));
        } else {
          DLong64GDL* cRet=new DLong64GDL(dimension(nCount),BaseGDL::NOALLOC); //danger!!
          cRet->SetBuffer((void*)comp_ret);
          cRet->SetBufferSize(nCount);
          cRet->SetDim(dimension(nCount));
          e->SetKW(0, cRet);
        }
      }

      if (e->KeywordPresent(1)) // NCOMPLEMENT
      {
        e->SetKW(1, new DLong64GDL(nCount));
      }

      if (nParam == 2) {
        e->SetPar(1, new DLong64GDL(count));
      }
      //The system variable !ERR is set to the number of nonzero elements for compatibility with old versions of IDL
      DVar *err = FindInVarList(sysVarList, "ERR");
      (static_cast<DLongGDL*> (err->Data()))[0] = count; //thus, not a DLong64!

      if (count == 0) {
        if (nullKW) {
          return NullGDL::GetSingleInstance();
        }
        return new DLongGDL(-1);
      }
      DLong64GDL* res=new DLong64GDL(dimension(count),BaseGDL::NOALLOC);
      res->SetBuffer((void*)ret);
      res->SetBufferSize(count);
      res->SetDim(dimension(count));
      return res;
    }
  }
}
