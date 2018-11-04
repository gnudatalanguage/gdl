/***************************************************************************
                           where_inc.cpp  -  include for where.cpp
                             -------------------
    begin                : Apr 7 2018
    copyright            : (C) 2002 by Marc Schellens, G. Duvert
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
template<>
void Data_<Sp>::Where(DLong64* &ret, SizeT &passed_count, bool comp, DLong64* &comp_ret) {
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
        bool tmp=(*this)[i];
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
          bool tmp=(*this)[i];
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
        bool tmp=(*this)[i];
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
          bool tmp=(*this)[i];
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
void Data_<Sp>::Where(DLong* &ret, SizeT &passed_count, bool comp, DLong* &comp_ret) {
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
        bool tmp=(*this)[i];
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
          bool tmp=(*this)[i];
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
        bool tmp=(*this)[i];
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
          bool tmp=(*this)[i];
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
