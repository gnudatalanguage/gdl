
#define INCLUDE_SMOOTH_POLYD
void SmoothPolyD(SMOOTH_Ty* srcIn, SMOOTH_Ty* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
 #include "smoothPolyD.hpp" 
}
//subset having edges
#define USE_EDGE
void SmoothPolyDWrap(SMOOTH_Ty* srcIn, SMOOTH_Ty* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_WRAP
#include "smoothPolyD.hpp" 
#undef EDGE_WRAP
}
void SmoothPolyDTruncate(SMOOTH_Ty* srcIn, SMOOTH_Ty* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_TRUNCATE
#include "smoothPolyD.hpp" 
#undef EDGE_TRUNCATE
}
void SmoothPolyDZero(SMOOTH_Ty* srcIn, SMOOTH_Ty* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_ZERO
#include "smoothPolyD.hpp" 
#undef EDGE_ZERO
}
void SmoothPolyDMirror(SMOOTH_Ty* srcIn, SMOOTH_Ty* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_MIRROR
#include "smoothPolyD.hpp" 
#undef EDGE_MIRROR
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_POLYD

#define INCLUDE_SMOOTH_POLYD_NAN
void SmoothPolyDNan(SMOOTH_Ty* srcIn, SMOOTH_Ty* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
 #include "smoothPolyDnans.hpp" 
}
//subset having edges
#define USE_EDGE
void SmoothPolyDWrapNan(SMOOTH_Ty* srcIn, SMOOTH_Ty* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_WRAP
#include "smoothPolyDnans.hpp" 
#undef EDGE_WRAP
}
void SmoothPolyDTruncateNan(SMOOTH_Ty* srcIn, SMOOTH_Ty* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_TRUNCATE
#include "smoothPolyDnans.hpp" 
#undef EDGE_TRUNCATE
}
void SmoothPolyDZeroNan(SMOOTH_Ty* srcIn, SMOOTH_Ty* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_ZERO
#include "smoothPolyDnans.hpp" 
#undef EDGE_ZERO
}
void SmoothPolyDMirrorNan(SMOOTH_Ty* srcIn, SMOOTH_Ty* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_MIRROR
#include "smoothPolyDnans.hpp" 
#undef EDGE_MIRROR
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_POLYD_NAN


#define INCLUDE_SMOOTH_1D
void Smooth1D(SMOOTH_Ty* data, SMOOTH_Ty* res, SizeT dimx, SizeT w) {
#include "smooth1d.hpp"
}
//subset having edges
#define USE_EDGE
void Smooth1DWrap(SMOOTH_Ty* data, SMOOTH_Ty* res, SizeT dimx, SizeT w) {
#define EDGE_WRAP
#include "smooth1d.hpp"
#undef EDGE_WRAP
}
void Smooth1DTruncate(SMOOTH_Ty* data, SMOOTH_Ty* res, SizeT dimx, SizeT w) {
#define EDGE_TRUNCATE
#include "smooth1d.hpp"
#undef EDGE_TRUNCATE
}
void Smooth1DMirror(SMOOTH_Ty* data, SMOOTH_Ty* res, SizeT dimx, SizeT w) {
#define EDGE_MIRROR
#include "smooth1d.hpp"
#undef EDGE_MIRROR
}
void Smooth1DZero(SMOOTH_Ty* data, SMOOTH_Ty* res, SizeT dimx, SizeT w) {
#define EDGE_ZERO
#include "smooth1d.hpp"
#undef EDGE_ZERO
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_1D

//smooth 1d functions for Nans.
#define INCLUDE_SMOOTH_1D_NAN
void Smooth1DNan(SMOOTH_Ty* data, SMOOTH_Ty* res, SizeT dimx, SizeT w) {
#include "smooth1dnans.hpp"
}
//subset having edges
#define USE_EDGE
void Smooth1DWrapNan(SMOOTH_Ty* data, SMOOTH_Ty* res, SizeT dimx, SizeT w) {
#define EDGE_WRAP
#include "smooth1dnans.hpp"
#undef EDGE_WRAP
}
void Smooth1DTruncateNan(SMOOTH_Ty* data, SMOOTH_Ty* res, SizeT dimx, SizeT w) {
#define EDGE_TRUNCATE
#include "smooth1dnans.hpp"
#undef EDGE_TRUNCATE
}
void Smooth1DMirrorNan(SMOOTH_Ty* data, SMOOTH_Ty* res, SizeT dimx, SizeT w) {
#define EDGE_MIRROR
#include "smooth1dnans.hpp"
#undef EDGE_MIRROR
}
void Smooth1DZeroNan(SMOOTH_Ty* data, SMOOTH_Ty* res, SizeT dimx, SizeT w) {
#define EDGE_ZERO
#include "smooth1dnans.hpp"
#undef EDGE_ZERO
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_1D_NAN

//smooth 2d functions.
#define INCLUDE_SMOOTH_2D
void Smooth2D(const SMOOTH_Ty* src, SMOOTH_Ty* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#include "smooth2d.hpp"
}
//subset having edges
#define USE_EDGE
void Smooth2DWrap(const SMOOTH_Ty* src, SMOOTH_Ty* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_WRAP
#include "smooth2d.hpp"
#undef EDGE_WRAP
}
void Smooth2DTruncate(const SMOOTH_Ty* src, SMOOTH_Ty* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_TRUNCATE
#include "smooth2d.hpp"
#undef EDGE_TRUNCATE
}
void Smooth2DMirror(const SMOOTH_Ty* src, SMOOTH_Ty* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_MIRROR
#include "smooth2d.hpp"
#undef EDGE_MIRROR
}
void Smooth2DZero(const SMOOTH_Ty* src, SMOOTH_Ty* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_ZERO
#include "smooth2d.hpp"
#undef EDGE_ZERO
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_2D

//smooth 2d functions for Nans.
#define INCLUDE_SMOOTH_2D_NAN
void Smooth2DNan(const SMOOTH_Ty* src, SMOOTH_Ty* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#include "smooth2dnans.hpp"
}
//subset having edges
#define USE_EDGE
void Smooth2DWrapNan(const SMOOTH_Ty* src, SMOOTH_Ty* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_WRAP
#include "smooth2dnans.hpp"
#undef EDGE_WRAP
}
void Smooth2DTruncateNan(const SMOOTH_Ty* src, SMOOTH_Ty* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_TRUNCATE
#include "smooth2dnans.hpp"
#undef EDGE_TRUNCATE
}
void Smooth2DMirrorNan(const SMOOTH_Ty* src, SMOOTH_Ty* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_MIRROR
#include "smooth2dnans.hpp"
#undef EDGE_MIRROR
}
void Smooth2DZeroNan(const SMOOTH_Ty* src, SMOOTH_Ty* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_ZERO
#include "smooth2dnans.hpp"
#undef EDGE_ZERO
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_2D_NAN

//Note: Values for ULong types return differently as IDL, but it should be proven that IDL is right...
template<>
BaseGDL* Data_<SMOOTH_SP>::Smooth(DLong* width, int edgeMode,
                                bool doNan, BaseGDL* missing)
{
  SMOOTH_Ty missingValue = (*static_cast<Data_*>( missing))[0];
  SizeT nA = N_Elements();
  
  SizeT srcRank = this->Rank();
  
  BaseGDL* data = this;
  BaseGDL* res = this->Dup();
  
  SizeT sum = 0;
  SizeT mydims[srcRank]; //memory of dimensions, done one after the other
  for (int i = 0; i < srcRank; ++i) 
  {
    mydims[i] = this->Dim(i);
    sum += width[i];
  }
  if (sum == srcRank) return res;
  
  DUInt rotate1[srcRank]; //turntable transposition index list
  
  //doNan only meaningful for real and double (complex are real and double here)
  doNan=(doNan && (this->Type()==GDL_FLOAT ||this->Type()==GDL_DOUBLE));

  for (int i = 0; i < srcRank-1; ++i) rotate1[i] = i+1; rotate1[srcRank-1]=0; //[1,2,...,0] 
 
  if (doNan) {
    if (srcRank==1) {
      SizeT dimx = nA;
      SizeT w = width[0] / 2;
      if (edgeMode==0){
        Smooth1DNan((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==1) {
        Smooth1DWrapNan((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==2) {
        Smooth1DTruncateNan((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==3) {
        Smooth1DZeroNan((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==4) {
        Smooth1DMirrorNan((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(),  dimx, w);
      }
    } else if (srcRank==2) {
      SizeT dimx = data->Dim(0);
      SizeT dimy = data->Dim(1);
      if (edgeMode==0){
        Smooth2DNan((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==1) {
        Smooth2DWrapNan((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==2) {
        Smooth2DTruncateNan((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==3) {
        Smooth2DZeroNan((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==4) {
        Smooth2DMirrorNan((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), dimx, dimy,  width);
      }
    } else {
      long rank = data->Rank();
      SizeT srcDim[MAXRANK];
      for (int i = 0; i < rank; ++i) srcDim[i] = data->Dim()[i];
      BaseGDL* dataw = this->Dup();
      if (edgeMode==0){
        SmoothPolyDNan((SMOOTH_Ty*)dataw->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==1) {
        SmoothPolyDWrapNan((SMOOTH_Ty*)dataw->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==2) {
        SmoothPolyDTruncateNan((SMOOTH_Ty*)dataw->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==3) {
        SmoothPolyDZeroNan((SMOOTH_Ty*)dataw->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==4) {
        SmoothPolyDMirrorNan((SMOOTH_Ty*)dataw->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), srcDim, rank, width);
      }
      GDLDelete(dataw);
    }
   //additional loop to replace left Nans by missing, if nans are left anyway
    if (gdlValid(missingValue))  {
      SMOOTH_Ty* resty=(SMOOTH_Ty*)res->DataAddr();
#pragma omp for 
      for (SizeT i=0; i<nA; ++i) if (!gdlValid(resty[i])) resty[i]=missingValue;
      }
  } else {
    if (srcRank==1) {
      SizeT dimx = nA;
      SizeT w = width[0] / 2;
      if (edgeMode==0){
        Smooth1D((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==1) {
        Smooth1DWrap((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==2) {
        Smooth1DTruncate((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==3) {
        Smooth1DZero((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==4) {
        Smooth1DMirror((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(),  dimx, w);
      }
    } else if (srcRank==2) {
      SizeT dimx = data->Dim(0);
      SizeT dimy = data->Dim(1);
      if (edgeMode==0){
        Smooth2D((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==1) {
        Smooth2DWrap((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==2) {
        Smooth2DTruncate((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==3) {
        Smooth2DZero((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==4) {
        Smooth2DMirror((SMOOTH_Ty*)data->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), dimx, dimy,  width);
      }
    } else  {
      long rank = data->Rank();
      SizeT srcDim[MAXRANK];
      for (int i = 0; i < rank; ++i) srcDim[i] = data->Dim()[i];
      BaseGDL* dataw = this->Dup();
      if (edgeMode==0){
        SmoothPolyD((SMOOTH_Ty*)dataw->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==1) {
        SmoothPolyDWrap((SMOOTH_Ty*)dataw->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==2) {
        SmoothPolyDTruncate((SMOOTH_Ty*)dataw->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==3) {
        SmoothPolyDZero((SMOOTH_Ty*)dataw->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==4) {
        SmoothPolyDMirror((SMOOTH_Ty*)dataw->DataAddr(), (SMOOTH_Ty*)res->DataAddr(), srcDim, rank, width);
      }
      GDLDelete(dataw);
    }
  }
  return res;
}