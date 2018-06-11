/***************************************************************************
                          minmax_include.cpp  -  include for minmax() 
                             -------------------
    begin                : March 30 2018
    copyright            : (C) 2004 by Marc Schellens, 2018 by G. Duvert
    email                : m_schellens@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// for all variants of MinMax(), to be included from datatypes.cpp

  // default: start = 0, stop = 0, step = 1, valIx = -1
  if (stop == 0) stop = dd.size();
 
#ifdef _OPENMP
#define MINMAX_THREAD_NUM omp_get_thread_num()
#else
#define MINMAX_THREAD_NUM 0
#endif
//permits to treat the complex types by ading .Real() to (*this)[i]
#ifdef MINMAX_IS_COMPLEX
#define REAL_PART(x) (x).real()
#define COMPLEX_ABS(x) std::abs((x))
#else
#define REAL_PART(x) (x)
#define COMPLEX_ABS(x) (x)
#endif
    
  
//permits to filter NaNs and Infs if the Type needs it AND if the context (omitNaN=true) needs it.
#ifdef MINMAX_HAS_OMITNAN
#define AVOID_INF if (omitNaN && !std::isfinite(COMPLEX_ABS((*this)[i]))) continue; 
#else
#define AVOID_INF 
#endif 
  

  SizeT nElem = (stop - start) / step;
//trap existence of ABSFUNC and create something that stands cppchekck useage (needed by contiunous integration scripts!) 
#ifndef ABSFUNC
#define FUNCABS
  useAbs=false;
#else
#define FUNCABS ABSFUNC
#endif
#ifdef MINMAX_HAS_OMITNAN
  if (omitNaN) //get first not-nan. Rest of processing will ignore nans since logical expressions must be true.
  {
    SizeT j = start;
    for (; j < stop; j += step) {
      if (std::isfinite(COMPLEX_ABS((*this)[j]))) break;
    }
    start = j;
    nElem = (stop - start) / step;
    if (nElem == 0)
    {
      if (minE != NULL) *minE = 0;
      if (maxE != NULL) *maxE = 0;
      if (minVal != NULL)
      {
        if (valIx == -1) *minVal = new Data_((*this)[j]);
        else (*static_cast<Data_*> (*minVal))[valIx] = (*this)[j];
      }
      if (maxVal != NULL)
      {
        if (valIx == -1) *maxVal = new Data_((*this)[j]);
        else (*static_cast<Data_*> (*maxVal))[valIx] = (*this)[j];
      }
      return;
    }
  }
#endif    
  if (minE == NULL && minVal == NULL)
  {
    DLong maxEl = start;
    Ty maxV = (*this)[maxEl];

    if (nElem < CpuTPOOL_MIN_ELTS || nElem < CpuTPOOL_NTHREADS || CpuTPOOL_NTHREADS == 1)
    {
      if (!useAbs)
      {
        for (SizeT i = start+step ; i < stop; i += step) {
          AVOID_INF 
          if (REAL_PART((*this)[i]) > REAL_PART(maxV)) maxV = (*this)[maxEl = i];
        }
      }
#ifdef ABSFUNC
      else
      {
        for (SizeT i = start+step; i < stop; i += step) {
          AVOID_INF 
          if (FUNCABS((*this)[i]) > FUNCABS(maxV)) maxV = (*this)[maxEl = i];
        }
      }
#endif
    } else
    {
      Ty* maxVArray= new Ty[CpuTPOOL_NTHREADS];
      SizeT maxElArray[CpuTPOOL_NTHREADS];
//precaution:initialize to something realistic:
      for (int i = 0; i < CpuTPOOL_NTHREADS; ++i) {maxVArray[i]=maxV;maxElArray[i]=maxEl;}

      SizeT chunksize = nElem / (CpuTPOOL_NTHREADS);
      if (!useAbs)
      {
#pragma omp parallel
        {
          int thread_id = MINMAX_THREAD_NUM;
          SizeT start_index, stop_index;
          start_index = start + thread_id * chunksize*step;
          if (thread_id != (CpuTPOOL_NTHREADS - 1))
          {
            stop_index = start_index + chunksize*step;
          } else
          {
            stop_index = stop;
          }
          SizeT local_pos = maxEl;
          Ty local_max = maxV; //maxV is a sure non-infinite value when omitNaN is in use.
          for (SizeT i = start_index; i < stop_index; i += step)
          {
            AVOID_INF 
            if (REAL_PART((*this)[i]) > REAL_PART(local_max)) local_max = (*this)[local_pos = i];
          }
          maxElArray[thread_id] = local_pos;
          maxVArray[thread_id] = local_max;
        }
      }
#ifdef ABSFUNC
      else
      {
#pragma omp parallel
        {
          int thread_id = MINMAX_THREAD_NUM;
          SizeT start_index, stop_index;
          start_index = start + thread_id * chunksize*step;
          if (thread_id != (CpuTPOOL_NTHREADS - 1))
          {
            stop_index = start_index + chunksize*step;
          } else
          {
            stop_index = stop;
          }
          SizeT local_pos = maxEl;
          Ty local_max = maxV;//maxV is a sure non-infinite value when omitNaN is in use.
          for (SizeT i = start_index; i < stop_index; i += step)
          {
            AVOID_INF 
            if (FUNCABS((*this)[i]) > FUNCABS(local_max)) local_max = (*this)[local_pos = i];
          }
          maxElArray[thread_id] = local_pos;
          maxVArray[thread_id] = local_max;
        }
      }
#endif
      maxV = maxVArray[0];
      maxEl = maxElArray[0];
#ifdef ABSFUNC
      if (useAbs)
      {
        for (int i = 1; i < CpuTPOOL_NTHREADS; ++i) if (FUNCABS(maxVArray[i]) > FUNCABS(maxV))
          {
            maxV = maxVArray[i];
            maxEl = maxElArray[i];
          }
      }
      else
#endif      
      {
        for (int i = 1; i < CpuTPOOL_NTHREADS; ++i) if (REAL_PART(maxVArray[i]) > REAL_PART(maxV))
          {
            maxV = maxVArray[i];
            maxEl = maxElArray[i];
          }
      }
    }
    
    if (maxE != NULL) *maxE = maxEl;
    if (maxVal != NULL)
    {
      if (valIx == -1) *maxVal = new Data_(maxV);
      else (*static_cast<Data_*> (*maxVal))[valIx] = maxV;
    }
    return;
  }
  if (maxE == NULL && maxVal == NULL)
  {
    DLong minEl = start;
    Ty minV = (*this)[minEl];

    if (nElem < CpuTPOOL_MIN_ELTS || nElem < CpuTPOOL_NTHREADS || CpuTPOOL_NTHREADS == 1)
    {
      if (!useAbs)
      {
        for (SizeT i = start+step; i < stop; i += step) {
          AVOID_INF 
          if (REAL_PART((*this)[i]) < REAL_PART(minV)) minV = (*this)[minEl = i];
        }
      }
#ifdef ABSFUNC
      else
      {
        for (SizeT i = start+step; i < stop; i += step) {
          AVOID_INF
          if (FUNCABS((*this)[i]) < FUNCABS(minV)) minV = (*this)[minEl = i];
        }
      }
#endif
    } else
    {
      Ty* minVArray=new Ty[CpuTPOOL_NTHREADS];
      SizeT minElArray[CpuTPOOL_NTHREADS];
//precaution:initialize to something realistic:
      for (int i = 0; i < CpuTPOOL_NTHREADS; ++i) {minVArray[i]=minV;minElArray[i]=minEl;}
      
      SizeT chunksize = nElem / (CpuTPOOL_NTHREADS);
      if (!useAbs)
      {
#pragma omp parallel
        {
          int thread_id = MINMAX_THREAD_NUM;
          SizeT start_index, stop_index;
          start_index = start + thread_id * chunksize*step;
          if (thread_id != (CpuTPOOL_NTHREADS - 1))
          {
            stop_index = start_index + chunksize*step;
          } else
          {
            stop_index = stop;
          }
          SizeT local_pos = minEl;
          Ty local_min = minV; // start with a sure non-infinite value, not (*this)[local_pos];
          for (SizeT i = start_index; i < stop_index; i += step)
          {
            AVOID_INF 
            if (REAL_PART((*this)[i]) < REAL_PART(local_min)) local_min = (*this)[local_pos = i];
          }
          minElArray[thread_id] = local_pos;
          minVArray[thread_id] = local_min;
        }
      }
#ifdef ABSFUNC
      else
      {
#pragma omp parallel
        {
          int thread_id = MINMAX_THREAD_NUM;
          SizeT start_index, stop_index;
          start_index = start + thread_id * chunksize*step;
          if (thread_id != (CpuTPOOL_NTHREADS - 1))
          {
            stop_index = start_index + chunksize*step;
          } else
          {
            stop_index = stop;
          }
          SizeT local_pos = minEl;
          Ty local_min = minV;
          for (SizeT i = start_index; i < stop_index; i += step)
          {
            AVOID_INF 
            if (FUNCABS((*this)[i]) < FUNCABS(local_min)) local_min = (*this)[local_pos = i];
          }
          minElArray[thread_id] = local_pos;
          minVArray[thread_id] = local_min;
        }
      }
#endif      
      minV = minVArray[0];
      minEl = minElArray[0];
#ifdef ABSFUNC
      if (useAbs)
      {
        for (int i = 1; i < CpuTPOOL_NTHREADS; ++i) if (FUNCABS(minVArray[i]) < FUNCABS(minV))
          {
            minV = minVArray[i];
            minEl = minElArray[i];
          }
      }
      else
#endif
      {
        for (int i = 1; i < CpuTPOOL_NTHREADS; ++i) if (REAL_PART(minVArray[i]) < REAL_PART(minV))
          {
            minV = minVArray[i];
            minEl = minElArray[i];
          }
      }
    }

    if (minE != NULL) *minE = minEl;
    if (minVal != NULL)
    {
      if (valIx == -1) *minVal = new Data_(minV);
      else (*static_cast<Data_*> (*minVal))[valIx] = minV;
    }
    return;

  }
  //note: a slightly faster minmax is done by pairs, not done here. 

    DLong minEl = start;
    Ty minV = (*this)[minEl];
    DLong maxEl = start;
    Ty maxV = (*this)[maxEl];

    if (nElem < CpuTPOOL_MIN_ELTS || nElem < CpuTPOOL_NTHREADS || CpuTPOOL_NTHREADS == 1)
    {
      if (!useAbs)
      {
        for (SizeT i = start+step; i < stop; i += step) {
          AVOID_INF 
          if (REAL_PART((*this)[i]) < REAL_PART(minV)) minV = (*this)[minEl = i];
          if (REAL_PART((*this)[i]) > REAL_PART(maxV)) maxV = (*this)[maxEl = i];
        }
      }
#ifdef ABSFUNC
      else
      {
        for (SizeT i = start+step; i < stop; i += step) {
          AVOID_INF
          if (FUNCABS((*this)[i]) < FUNCABS(minV)) minV = (*this)[minEl = i];
          if (FUNCABS((*this)[i]) > FUNCABS(maxV)) maxV = (*this)[maxEl = i];
        }
      }
#endif
    } else
    {

      Ty* maxVArray=new Ty[CpuTPOOL_NTHREADS];
      SizeT maxElArray[CpuTPOOL_NTHREADS];
      Ty* minVArray=new Ty[CpuTPOOL_NTHREADS];
      SizeT minElArray[CpuTPOOL_NTHREADS];
//precaution:initialize to something realistic:
      for (int i = 0; i < CpuTPOOL_NTHREADS; ++i) {maxVArray[i]=maxV;maxElArray[i]=maxEl;}
      for (int i = 0; i < CpuTPOOL_NTHREADS; ++i) {minVArray[i]=minV;minElArray[i]=minEl;}
      
      SizeT chunksize = nElem / (CpuTPOOL_NTHREADS);
      if (!useAbs)
      {
#pragma omp parallel
        {
          int thread_id = MINMAX_THREAD_NUM;
          SizeT start_index, stop_index;
          start_index = start + thread_id * chunksize*step;
          if (thread_id != (CpuTPOOL_NTHREADS - 1))
          {
            stop_index = start_index + chunksize*step;
          } else
          {
            stop_index = stop;
          }
          SizeT local_pos_min = minEl;
          SizeT local_pos_max = maxEl;
          Ty local_min = minV;
          Ty local_max = maxV;
          for (SizeT i = start_index; i < stop_index; i += step) {
            AVOID_INF 
            if (REAL_PART((*this)[i]) < REAL_PART(local_min)) local_min = (*this)[local_pos_min = i];
            if (REAL_PART((*this)[i]) > REAL_PART(local_max)) local_max = (*this)[local_pos_max = i];
          }
          minElArray[thread_id] = local_pos_min;
          minVArray[thread_id] = local_min;
          maxElArray[thread_id] = local_pos_max;
          maxVArray[thread_id] = local_max;
        }
      }
#ifdef ABSFUNC
      else
      {
#pragma omp parallel
        {
          int thread_id = MINMAX_THREAD_NUM;
          SizeT start_index, stop_index;
          start_index = start + thread_id * chunksize*step;
          if (thread_id != (CpuTPOOL_NTHREADS - 1))
          {
            stop_index = start_index + chunksize*step;
          } else
          {
            stop_index = stop;
          }
          SizeT local_pos_min = minEl;
          SizeT local_pos_max = maxEl;
          Ty local_min = minV;
          Ty local_max = maxV;
          for (SizeT i = start_index; i < stop_index; i += step) {
            AVOID_INF 
            if (FUNCABS((*this)[i]) < FUNCABS(local_min)) local_min = (*this)[local_pos_min = i];
            if (FUNCABS((*this)[i]) > FUNCABS(local_max)) local_max = (*this)[local_pos_max = i];
          }
          minElArray[thread_id] = local_pos_min;
          minVArray[thread_id] = local_min;
          maxElArray[thread_id] = local_pos_max;
          maxVArray[thread_id] = local_max;
        }
      }
#endif
      minV = minVArray[0];
      maxV = maxVArray[0];
      minEl = minElArray[0];
      maxEl = maxElArray[0];
#ifdef ABSFUNC
      if (useAbs)
      {
        for (int i = 1; i < CpuTPOOL_NTHREADS; ++i) {
          if (FUNCABS(minVArray[i]) < FUNCABS(minV)) {
            minV = minVArray[i];
            minEl = minElArray[i];
          }
          if (FUNCABS(maxVArray[i]) > FUNCABS(maxV)) {
            maxV = maxVArray[i];
            maxEl = maxElArray[i];
          }
        }
      }
      else
#endif
      {
        for (int i = 1; i < CpuTPOOL_NTHREADS; ++i) {
          if (REAL_PART(minVArray[i]) < REAL_PART(minV)) {
            minV = minVArray[i];
            minEl = minElArray[i];
          }
          if (REAL_PART(maxVArray[i]) > REAL_PART(maxV)) {
            maxV = maxVArray[i];
            maxEl = maxElArray[i];
          }
        }        
      }
    }
    
  if (maxE != NULL) *maxE = maxEl;
  if (maxVal != NULL)
  {
    if (valIx == -1) *maxVal = new Data_(maxV);
    else (*static_cast<Data_*> (*maxVal))[valIx] = maxV;
  }
    
  if (minE != NULL) *minE = minEl;
  if (minVal != NULL)
  {
    if (valIx == -1) *minVal = new Data_(minV);
    else (*static_cast<Data_*> (*minVal))[valIx] = minV;
  }
    
#undef AVOID_INF 
#undef REAL_PART
#undef COMPLEX_ABS
#undef FUNCABS
#undef MINMAX_THREAD_NUM