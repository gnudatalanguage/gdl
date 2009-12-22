#ifndef STRASSENMATRIX_HPP__
#define STRASSENMATRIX_HPP__

//#define USE_STRASSEN_MATRIXMULTIPLICATION

#ifdef USE_STRASSEN_MATRIXMULTIPLICATION

// optimized strassen matrix multiplication
// to be included by basic_op.cpp

// problem: this implementation only scales to up to 7 processors
// and the omp task construct seems to be only available from OpenMP 3 on
// this means it will not use OpenMP at all on non up to date machines

// #include "typedefs.hpp"

// #ifdef _OPENMP
// #include <omp.h>
// #endif

//const SizeT minStrassenMatrixSize = 500;
const SizeT minStrassenMatrixResolveSize = 32;
const SizeT minStrassenMatrixResolveSize2 = minStrassenMatrixResolveSize^2;
const SizeT minStrassenMatrixResolveSize3 = minStrassenMatrixResolveSize^3;

// B has always greater (or equal) indices
template< typename T>
void SMAdd( SizeT mSz, SizeT abx, SizeT aby, 
	    T *A, SizeT ax, SizeT ay, SizeT as,
	    SizeT bx, SizeT by, 
	    T *C, long cxLim, long cyLim)// C has always dimensions mSy*mSz
{
  if( cxLim <= 0 || cyLim <=0) 
    return;

  T*& B = A;
  SizeT& bs = as;

  long i;
  long j;

  long mSzX = (cxLim>mSz)?mSz:cxLim;
  long mSzY = (cyLim>mSz)?mSz:cyLim;

  // ax is always smaller or equal  to bx, same for ay and by
  // hence axLim is greater or equal then bxLim, same for ayLim and byLim
  long axLim,ayLim,bxLim,byLim;
  if( (bx+mSz)>=abx)
    {
      bxLim=abx-bx;
      if( (by+mSz)>=aby)
	byLim=aby-by;
      else
	byLim=mSz;
    }
  else
    {
      if( (by+mSz)>=aby)
	byLim=aby-by;
      else
	{
	  for (long i=0; i<mSzX; ++i) 
	    {
	      for (long j=0; j<mSzY; ++j) 
		{
		  C[i*mSz + j] = A[(i+ax)*as + j+ay] + B[(i+bx)*bs + j+by];
		}
	    }
	  return;
	}
      bxLim=mSz;
    }

  if( (ax+mSz)>=abx)
    {
      axLim=abx-ax;
      if( (ay+mSz)>=aby)
	ayLim=aby-ay;
      else
	ayLim=mSz;
    }
  else
    {
      if( (ay+mSz)>=aby)
	ayLim=aby-ay;
      else
	{

	  if( bxLim > cxLim) bxLim = mSzX;
	  if( byLim > cyLim) byLim = mSzY;

	  for (i=0; i<bxLim; ++i) 
	    {
	      for (j=0; j<byLim; ++j) 
		{
		  C[i*mSz + j] = A[(i+ax)*as + j+ay] + B[(i+bx)*bs + j+by];
		}
	      for (; j<mSzY; ++j) 
		{
		  C[i*mSz + j] = A[(i+ax)*as + j+ay];
		}
	    }
	  for (; i<mSzX; ++i) 
	    {
	      for (j=0; j<mSzY; ++j) 
		{
		  C[i*mSz + j] = A[(i+ax)*as + j+ay];
		}
	    }
	  return;
	}
      axLim=mSz;
    }

  if( axLim > cxLim) axLim = mSzX;
  if( ayLim > cyLim) ayLim = mSzY;
  if( bxLim > cxLim) bxLim = mSzX;
  if( byLim > cyLim) byLim = mSzY;

  for (i=0; i<bxLim; ++i) 
    {
      for (j=0; j<byLim; ++j) 
	{
	  C[i*mSz + j] = A[(i+ax)*as + j+ay] + B[(i+bx)*bs + j+by];
	}
      for (; j<ayLim; ++j) 
	{
	  C[i*mSz + j] = A[(i+ax)*as + j+ay];
	}
      for (; j<mSzY; ++j) 
	{
	  C[i*mSz + j] = 0;
	}
    }
  for (; i<axLim; ++i) 
    {
      for (j=0; j<ayLim; ++j) 
	{
	  C[i*mSz + j] = A[(i+ax)*as + j+ay];
	}
      for (; j<mSzY; ++j) 
	{
	  C[i*mSz + j] = 0;
	}
    }
  for (; i<mSzX; ++i) 
    {
      for (j=0; j<mSzY; ++j) 
	{
	  C[i*mSz + j] = 0;
	}
    }
}

// template< typename T>
// void SMAddNoCheck( SizeT mSz,
//        T *A, SizeT ax, SizeT ay, SizeT as,  /* (ax,ay) = origin of A submatrix */
//        SizeT bx, SizeT by,  /* (bx,by) = origin of B submatrix */
//        T *C, SizeT cx, SizeT cy, SizeT cs)  /* (cx,cy) = origin of C submatrix */
// {
//   T*& B = A;
//   SizeT& bs = as;
// 
// 	  for (long i=0; i<mSz; ++i) 
// 	    {
// 	      for (long j=0; j<mSz; ++j) 
// 		{
// 		  C[i*mSz + j+cy] = A[(i+ax)*as + j+ay] + B[(i+bx)*bs + j+by];
// 		}
// 	    }
// }

// B has always greater (or equal) indices
template< typename T>
void SMSub1( SizeT mSz, SizeT abx, SizeT aby, 
	     T *A, SizeT ax, SizeT ay, SizeT as, 
	     SizeT bx, SizeT by,
	     T *C, long cxLim, long cyLim)// C has always dimensions mSy*mSz
{
  if( cxLim <= 0 || cyLim <=0) return;

  T*& B = A;
  SizeT& bs = as;

  long i;
  long j;
  // C is not checked (does not need to)

  long mSzX = (cxLim>mSz)?mSz:cxLim;
  long mSzY = (cyLim>mSz)?mSz:cyLim;

  // ax is always smaller or equal  to bx, same for ay and by
  // hence axLim is greater or equal then bxLim, same for ayLim and byLim
  long axLim,ayLim,bxLim,byLim;
  if( (bx+mSz)>=abx)
    {
      bxLim=abx-bx;
      if( (by+mSz)>=aby)
	byLim=aby-by;
      else
	byLim=mSz;
    }
  else
    {
      if( (by+mSz)>=aby)
	byLim=aby-by;
      else
	{
	  for (long i=0; i<mSzX; ++i) 
	    {
	      for (long j=0; j<mSzY; ++j) 
		{
		  C[i*mSz + j] = A[(i+ax)*as + j+ay] - B[(i+bx)*bs + j+by];
		}
	    }
	  return;
	}
      bxLim=mSz;
    }

  if( (ax+mSz)>=abx)
    {
      axLim=abx-ax;
      if( (ay+mSz)>=aby)
	ayLim=aby-ay;
      else
	ayLim=mSz;
    }
  else
    {
      if( (ay+mSz)>=aby)
	ayLim=aby-ay;
      else
	{
	  for (i=0; i<bxLim; ++i) 
	    {
	      for (j=0; j<byLim; ++j) 
		{
		  C[i*mSz + j] = A[(i+ax)*as + j+ay] - B[(i+bx)*bs + j+by];
		}
	      for (; j<mSzY; ++j) 
		{
		  C[i*mSz + j] = A[(i+ax)*as + j+ay];
		}
	    }
	  for (; i<mSzX; ++i) 
	    {
	      for (j=0; j<mSzY; ++j) 
		{
		  C[i*mSz + j] = A[(i+ax)*as + j+ay];
		}
	    }
	  return;
	}
      axLim=mSz;
    }

  if( axLim > cxLim) axLim = mSzX;
  if( ayLim > cyLim) ayLim = mSzY;
  if( bxLim > cxLim) bxLim = mSzX;
  if( byLim > cyLim) byLim = mSzY;

  for (i=0; i<bxLim; ++i) 
    {
      for (j=0; j<byLim; ++j) 
	{
	  C[i*mSz + j] = A[(i+ax)*as + j+ay] - B[(i+bx)*bs + j+by];
	}
      for (; j<ayLim; ++j) 
	{
	  C[i*mSz + j] = A[(i+ax)*as + j+ay];
	}
      for (; j<mSzY; ++j) 
	{
	  C[i*mSz + j] = 0;
	}
    }
  for (; i<axLim; ++i) 
    {
      for (j=0; j<ayLim; ++j) 
	{
	  C[i*mSz + j] = A[(i+ax)*as + j+ay];
	}
      for (; j<mSzY; ++j) 
	{
	  C[i*mSz + j] = 0;
	}
    }
  for (; i<mSzX; ++i) 
    {
      for (j=0; j<mSzY; ++j) 
	{
	  C[i*mSz + j] = 0;
	}
    }
}

// here A has the greater indices 
// and thus axyLim is smaller for B compared to A
template< typename T>
void SMSub2( SizeT mSz, SizeT abx, SizeT aby, 
	     T *A, SizeT ax, SizeT ay, SizeT as,
	     SizeT bx, SizeT by,
	     T *C, long cxLim, long cyLim) // C has always dimensions mSy*mSz
{
  if( cxLim <= 0 || cyLim <=0) return;

  T*& B = A;
  SizeT& bs = as;

  long i;
  long j;
  // C is not checked (does not need to)

  long mSzX = (cxLim>mSz)?mSz:cxLim;
  long mSzY = (cyLim>mSz)?mSz:cyLim;

  // ax is always smaller or equal  to bx, same for ay and by
  // hence axLim is greater or equal then bxLim, same for ayLim and byLim
  long axLim,ayLim,bxLim,byLim;
  if( (ax+mSz)>=abx)
    {
      axLim=abx-ax;
      if( (ay+mSz)>=aby)
	ayLim=aby-ay;
      else
	ayLim=mSz;
    }
  else
    {
      if( (ay+mSz)>=aby)
	ayLim=aby-ay;
      else
	{
	  for (long i=0; i<mSzX; ++i) 
	    {
	      for (long j=0; j<mSzY; ++j) 
		{
		  C[i*mSz + j] = A[(i+ax)*as + j+ay] - B[(i+bx)*bs + j+by];
		}
	    }
	  return;
	}
      axLim=mSz;
    }

  if( (bx+mSz)>=abx)
    {
      bxLim=abx-bx;
      if( (by+mSz)>=aby)
	byLim=aby-by;
      else
	byLim=mSz;
    }
  else
    {
      if( (by+mSz)>=aby)
	byLim=aby-by;
      else
	{
	  for (i=0; i<axLim; ++i) 
	    {
	      for (j=0; j<ayLim; ++j) 
		{
		  C[i*mSz + j] = A[(i+ax)*as + j+ay] - B[(i+bx)*bs + j+by];
		}
	      for (; j<mSzY; ++j) 
		{
		  C[i*mSz + j] = -B[(i+bx)*bs + j+by];
		}
	    }
	  for (; i<mSzX; ++i) 
	    {
	      for (j=0; j<mSzY; ++j) 
		{
		  C[i*mSz + j] = -B[(i+bx)*bs + j+by];
		}
	    }
	  return;
	}
      bxLim=mSz;
    }

  if( axLim > cxLim) axLim = mSzX;
  if( ayLim > cyLim) ayLim = mSzY;
  if( bxLim > cxLim) bxLim = mSzX;
  if( byLim > cyLim) byLim = mSzY;

  for (i=0; i<axLim; ++i) 
    {
      for (j=0; j<ayLim; ++j) 
	{
	  C[i*mSz + j] = A[(i+ax)*as + j+ay] - B[(i+bx)*bs + j+by];
	}
      for (; j<byLim; ++j) 
	{
	  C[i*mSz + j] = -B[(i+bx)*bs + j+by];
	}
      for (; j<mSzY; ++j) 
	{
	  C[i*mSz + j] = 0;
	}
    }
  for (; i<bxLim; ++i) 
    {
      for (j=0; j<byLim; ++j) 
	{
	  C[i*mSz + j] = -B[(i+bx)*bs + j+by];
	}
      for (; j<mSzY; ++j) 
	{
	  C[i*mSz + j] = 0;
	}
    }
  for (; i<mSzX; ++i) 
    {
      for (j=0; j<mSzY; ++j) 
	{
	  C[i*mSz + j] = 0;
	}
    }
}


template< typename T>
void SMNegate( SizeT mSz, SizeT abx, SizeT aby, 
	       T *A, SizeT ax, SizeT ay, SizeT as,
	       T *C, long cxLim, long cyLim) // C has always dimensions mSy*mSz
{
  if( cxLim <= 0 || cyLim <=0) return;

  long i;
  long j;
  // C is not checked (does not need to)

  // ax is always smaller or equal  to bx, same for ay and by
  // hence axLim is greater or equal then bxLim, same for ayLim and byLim
  long axLim,ayLim;
  if( (ax+mSz)>=abx)
    {
      axLim=abx-ax;
      if( (ay+mSz)>=aby)
	ayLim=aby-ay;
      else
	ayLim=mSz;
    }
  else
    {
      if( (ay+mSz)>=aby)
	ayLim=aby-ay;
      else
	{
	  for (long i=0; i<cxLim; ++i) 
	    {
	      for (long j=0; j<cyLim; ++j) 
		{
		  C[(i)*mSz + j] = -A[(i+ax)*as + j+ay];
		}
	    }
	  return;
	}
      axLim=mSz;
    }

  if( cxLim < axLim) axLim = cxLim;
  if( cyLim < ayLim) ayLim = cyLim;

  for (i=0; i<axLim; ++i) 
    {
      for (j=0; j<ayLim; ++j) 
	{
	  C[(i)*mSz + j] = -A[(i+ax)*as + j+ay];
	}
      for (; j<cyLim; ++j) 
	{
	  C[(i)*mSz + j] = 0;
	}
    }
  for (; i<cxLim; ++i) 
    {
      for (j=0; j<cyLim; ++j) 
	{
	  C[(i)*mSz + j] = 0;
	}
    }
}








template< typename T>
void SMM( SizeT mSz, long l, long m, long n, // A[l,m]#B[m,n]=C[l,n]
	  T *A, SizeT ax, SizeT ay, SizeT as,
	  T *B, SizeT bx, SizeT by, SizeT bs,
          T *C, long cxLim, long cyLim) // C has always dimensions mSy*mSz
{
  if( cxLim <= 0 || cyLim <=0) return;

  long i, j, k;
  const SizeT cx=0;
  const SizeT cy=0;
  SizeT& cs=mSz;

  long mSzX = (mSz>cxLim)?cxLim:mSz;
  long mSzY = (mSz>cyLim)?cyLim:mSz;

  if( (ax+mSz)>=n)
    n-=ax;
  else
    n=mSz;
  
  if( (by+mSz)>=l)
    l-=by;
  else
    l=mSz;

  if( ay>bx)
    if( (ay+mSz)>=m)
      m-=ay;
    else
      m=mSz;
  else
    if( (bx+mSz)>=m)
      m-=bx;
    else
      m=mSz;

  if( n > cxLim && l > cyLim)
    {
      for (i=0; i<cxLim; ++i) 
	{
	  for (j=0; j<cyLim; ++j) 
	    {
	      C[i*mSz + j+cy] = 0;//A[(i+ax)*as + ay] * B[(bx)*bs + j+by];
	      for (k=0; k<m; ++k) 
		{
		  C[i*mSz + j+cy] += A[(i+ax)*as + k+ay] * B[(k+bx)*bs + j+by];
		}
	    }
	}
      return;
    }
  if( n > cxLim) n = cxLim;
  if( l > cyLim) l = cyLim;

  for (i=0; i<n; ++i) 
    {
      for (j=0; j<l; ++j) 
	{
	  C[i*mSz + j+cy] = 0;//A[(i+ax)*as + ay] * B[(bx)*bs + j+by];
	  for (k=0; k<m; ++k) 
	    {
	      C[i*mSz + j+cy] += A[(i+ax)*as + k+ay] * B[(k+bx)*bs + j+by];
	    }
	}
      for (; j<mSzY; ++j) 
	{
	  C[i*mSz + j+cy] = 0;
	}
    }
  for (; i<mSzX; ++i) 
    {
      for (j=0; j<mSzY; ++j) 
	{
	  C[i*mSz + j+cy] = 0;
	}
    }

}

template< typename T>
void SMMNoCheckAB( SizeT mSz, // A[l,m]#B[m,n]=C[l,n]
		   T *A, SizeT ax, SizeT ay, SizeT as,
		   T *B, SizeT bx, SizeT by, SizeT bs,
		   T *C, long cxLim, long cyLim) // C has always dimensions mSy*mSz
{
  if( cxLim <= 0 || cyLim <=0) return;

//   const SizeT cx=0;
//   const SizeT cy=0;
//   SizeT& cs=mSz;

  long mSzX = (mSz>cxLim)?cxLim:mSz;
  long mSzY = (mSz>cyLim)?cyLim:mSz;
  for (long i=0; i<mSzX; ++i) 
    {
      for (long j=0; j<mSzY; ++j) 
	{
	  C[(i)*mSz + j] = 0;//A[(i+ax)*as + ay] * B[(bx)*bs + j+by];
	  for (SizeT k=0; k<mSz; ++k) 
	    {
	      C[(i)*mSz + j] += A[(i+ax)*as + k+ay] * B[(k+bx)*bs + j+by];
	    }
	}
    }
}













template< typename T>
void SMNoCheckAB( T* buf, SizeT mSz, // A[l,m]#B[m,n]=C[l,n]
		  T *A, SizeT ax, SizeT ay, SizeT as,
		  T *B, SizeT bx, SizeT by, SizeT bs,
	          T *C, long cxLim, long cyLim) // C has always dimensions mSy*mSz
{
  if( cxLim <= 0 || cyLim <=0) return;

  if( mSz <= minStrassenMatrixResolveSize)
    //   if( (mSz <= minStrassenMatrixResolveSize) || ((cxLim * cyLim) <= minStrassenMatrixResolveSize2))
    //   if( (mSz <= minStrassenMatrixResolveSize) || ((cxLim * cyLim * mSz) <= minStrassenMatrixResolveSize3))
    {
      SMMNoCheckAB<T>( mSz,
		       A, ax, ay, as,
		       B, bx, by, bs,
		       C, cxLim, cyLim);//, cx, cy, cs);
      return;
    }

  // divide and rule
  long n_2 = mSz >> 1;
  SizeT n_22 = n_2 * n_2;

  //   T* buf = new T[n_22 * 9];

  T*& a_cum = buf; 
  T* b_cum = a_cum + n_22;
  T* p1    = b_cum + n_22;
  T* p2    = p1 + n_22;
  T* p3    = p2 + n_22;
  T* p4    = p3 + n_22;
  T* p5    = p4 + n_22;
  T* p6    = p5 + n_22;
  T* p7    = p6 + n_22;
  T* subBuf = p7 + n_22;

  SizeT ax2 = ax+n_2;
  SizeT ay2 = ay+n_2;
  SizeT bx2 = bx+n_2;
  SizeT by2 = by+n_2;

  /* p1 = (a11 + a22) x (b11 + b22) */
  T* A11 = &A[ax*as+ay];
  T* A21 = &A[ax2*as+ay];
  T* A12 = &A[ax*as+ay2];
  T* A22 = &A[ax2*as+ay2];

  T* B11 = &B[bx*bs+by];
  T* B21 = &B[bx2*bs+by];
  T* B12 = &B[bx*bs+by2];
  T* B22 = &B[bx2*bs+by2];

  long xLim = (cxLim > n_2) ? n_2: cxLim; 
  long yLim = (cyLim > n_2) ? n_2: cyLim; 
  long xLim2 = cxLim - n_2;
  long yLim2 = cyLim - n_2; 
  assert( xLim2 <= n_2);
  assert( yLim2 <= n_2);

  /* p1 = (a11 + a22) x (b11 + b22) */
  SizeT cumIx = 0;
  for (long i=0; i<xLim; ++i) 
    {
      for (long j=0; j<n_2; ++j) 
	{
	  a_cum[cumIx] = A11[(i)*as + j] + A22[(i)*as + j];
	  cumIx++;
	}
    }
  for (long i=0; i<n_2; ++i) 
    {
      SizeT cumIx = i * n_2;
      for (long j=0; j<yLim; ++j) 
	{
	  b_cum[cumIx] = B11[(i)*bs + j] + B22[(i)*bs + j];
	  cumIx++;
	}
    }
  //SMAdd<T>(n_2,l,m, A, ax, ay, as, ax+n_2, ay+n_2, a_cum);
  //SMAdd<T>(n_2,m,n, B, bx, by, bs, bx+n_2, by+n_2, b_cum);
  SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p1, xLim, yLim);

  /* p4 = a22 x (b21 - b11) */
  cumIx = 0;
  for (long i=0; i<n_2; ++i) 
    {
      SizeT cumIx = i * n_2;
      for (long j=0; j<yLim; ++j) 
	{
	  b_cum[cumIx] = B21[(i)*bs + j] - B11[(i)*bs + j];
	  cumIx++;
	}
    }
  //   SMSub2<T>(n_2,m,n, B, bx+n_2, by, bs, bx, by, b_cum);
  SMNoCheckAB<T>(subBuf,n_2, A, ax+n_2, ay+n_2, as, b_cum, 0, 0, n_2, p4, xLim, yLim);

  /* p5 = (a11 + a12) x b22 */
  for (long i=0; i<xLim; ++i) 
    {
      for (long j=0; j<n_2; ++j) 
	{
	  a_cum[cumIx] = A11[(i)*as + j] + A12[(i)*as + j];
	  cumIx++;
	}
    }
  //   SMAdd<T>(n_2,l,m, A, ax, ay, as, ax, ay+n_2, a_cum);
  SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, B, bx+n_2, by+n_2, bs, p5, xLim, yLim);

  /* p7 = (a12 - a22) x (b21 + b22) */
  cumIx = 0;
  for (long i=0; i<xLim; ++i) 
    {
      for (long j=0; j<n_2; ++j) 
	{
	  a_cum[cumIx] = A12[(i)*as + j] - A22[(i)*as + j];
	  cumIx++;
	}
    }
  for (long i=0; i<n_2; ++i) 
    {
      SizeT cumIx = i * n_2;
      for (long j=0; j<yLim; ++j) 
	{
	  b_cum[cumIx] = B21[(i)*bs + j] + B22[(i)*bs + j];
	  cumIx++;
	}
    }
  //   SMSub1<T>(n_2,l,m, A, ax, ay+n_2, as, ax+n_2, ay+n_2, a_cum);
  //   SMAdd<T>(n_2,m,n, B, bx+n_2, by, bs, bx+n_2, by+n_2, b_cum);
  SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p7, xLim, yLim);

  // we are in SMNoCheckAB 
  if( cxLim >= mSz && cyLim >= mSz)
    {
      /* p2 = (a21 + a22) x b11 */
      cumIx = 0;
      for (long i=0; i<xLim2; ++i) 
	{
	  for (long j=0; j<n_2; ++j) 
	    {
	      a_cum[cumIx] = A21[(i)*as + j] + A22[(i)*as + j];
	      cumIx++;
	    }
	}
      //   SMAdd<T>(n_2,l,m, A, ax+n_2, ay, as, ax+n_2, ay+n_2, a_cum);
      SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, B, bx, by, bs, p2, xLim2, yLim);

      /* p3 = a11 x (b12 - b22) */
      for (long i=0; i<n_2; ++i) 
	{
	  SizeT cumIx = i * n_2;
	  for (long j=0; j<yLim2; ++j) 
	    {
	      b_cum[cumIx] = B12[(i)*bs + j] - B22[(i)*bs + j];
	      cumIx++;
	    }
	}
      //   SMSub1<T>(n_2,m,n, B, bx, by+n_2, bs, bx+n_2, by+n_2, b_cum);
      SMNoCheckAB<T>(subBuf,n_2, A, ax, ay, as, b_cum, 0, 0, n_2, p3, xLim, yLim2);

      /* p6 = (a21 - a11) x (b11 + b12) */
      cumIx = 0;
      for (long i=0; i<xLim2; ++i) 
	{
	  for (long j=0; j<n_2; ++j) 
	    {
	      a_cum[cumIx] = A21[(i)*as + j] - A11[(i)*as + j];
	      cumIx++;
	    }
	}
      for (long i=0; i<n_2; ++i) 
	{
	  SizeT cumIx = i * n_2;
	  for (long j=0; j<yLim2; ++j) 
	    {
	      b_cum[cumIx] = B11[(i)*bs + j] + B12[(i)*bs + j];
	      cumIx++;
	    }
	}
      //   SMSub2<T>(n_2,l,m, A, ax+n_2, ay, as, ax, ay, a_cum);
      //   SMAdd<T>(n_2,m,n, B, bx, by, bs, bx, by+n_2, b_cum);
      SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p6, xLim2, yLim2);

      SizeT pIndex = 0;
      SizeT n_2_mSz = n_2 * mSz;
      SizeT n_2_mSz_n_2 = n_2_mSz + n_2;
      for( SizeT ix=0;ix<n_2;++ix)
	{
	  SizeT cIndex = ix * mSz;
	  for( SizeT iy=0;iy<n_2;++iy)
	    {
	      C[cIndex] = p1[pIndex]+p4[pIndex]-p5[pIndex]+p7[pIndex];
	      C[cIndex+n_2] = p3[pIndex]+p5[pIndex];
	      C[cIndex+n_2_mSz] = p2[pIndex]+p4[pIndex];
	      C[cIndex++ +n_2_mSz_n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
	      ++pIndex;
	    }
	}
      return;
    } 

  //   if( cxLim >= mSz && cyLim >= mSz)
  //     {
  //       SizeT pIndex = 0;//ix*n_2+iy;
  //       for( SizeT ix=0;ix<n_2;++ix)
  // 	for( SizeT iy=0;iy<n_2;++iy)
  // 	  {
  // 	    /* c11 = p1 + p4 - p5 + p7 */
  // 	    C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex]-p5[pIndex]+p7[pIndex];
  // 
  // 	    /* c12 = p3 + p5 */
  // 	    C[(ix)*mSz + iy+n_2] = p3[pIndex]+p5[pIndex];
  // 		
  // 	    /* c21 = p2 + p4 */
  // 	    C[(ix+n_2)*mSz + iy] = p2[pIndex]+p4[pIndex];
  // 		
  // 	    /* c22 = p1 + p3 - p2 + p6 */
  // 	    C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
  // 
  // 	    pIndex++;
  // 	  }
  //       return;
  //     }

  long cx11Lim;
  long cy11Lim;
  long cx12Lim;
  long cy12Lim;
  long cx21Lim;
  long cy21Lim;
  long cx22Lim;
  long cy22Lim;
  long cxLim_n_2 = cxLim - n_2;
  long cyLim_n_2 = cyLim - n_2;
  bool do12, do21, do22;
  if( n_2 >= cxLim) // x limited
    {
      cx11Lim = cxLim;
      if( n_2 >= cyLim)
	{
	  cy11Lim = cyLim;
// 	  do12 = false;
// 	  do21 = false;
// 	  do22 = false;
	  for( SizeT ix=0;ix<cx11Lim;++ix)
	    {
	      SizeT pIndex = ix * n_2;
	      for( SizeT iy=0;iy<cy11Lim;++iy)
		{
		  /* c11 = p1 + p4 - p5 + p7 */
		  // 	SizeT pIndex = ix * n_2 + iy;
		  C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex]-p5[pIndex]+p7[pIndex];
		  ++pIndex;
		}
	    }
	  return;
	}
      else
	{
	  cy11Lim = n_2;
	  do12 = true;
	  do21 = false;
	  do22 = false;
	  cx12Lim = cxLim;
	  cy12Lim = (cyLim>mSz)?n_2:cyLim_n_2;
	}
    }
  else 
    {
      cx11Lim = n_2;
      if( n_2 >= cyLim)
	{
	  cy11Lim = cyLim;
	  do12 = false;
	  do21 = true;
	  do22 = false;
	  cy21Lim = cyLim;
	  cx21Lim = (cxLim>mSz)?n_2:cxLim_n_2;
	}
      else
	{
	  cy11Lim = n_2;
	  do12 = true;
	  do21 = true;
	  do22 = true;
	  cx12Lim = n_2;
	  cy12Lim = (cyLim>mSz)?n_2:cyLim_n_2;
	  cx21Lim = (cxLim>mSz)?n_2:cxLim_n_2;
	  cy21Lim = n_2;
	  cx22Lim = (cxLim>mSz)?n_2:cxLim_n_2;
	  cy22Lim = (cyLim>mSz)?n_2:cyLim_n_2;
	}
    }
  for( SizeT ix=0;ix<cx11Lim;++ix)
    {
      SizeT pIndex = ix * n_2;
      for( SizeT iy=0;iy<cy11Lim;++iy)
	{
	  /* c11 = p1 + p4 - p5 + p7 */
	  // 	SizeT pIndex = ix * n_2 + iy;
	  C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex]-p5[pIndex]+p7[pIndex];
	  ++pIndex;
	}
    }

  if( do12)
    {
      /* p3 = a11 x (b12 - b22) */
      for (long i=0; i<n_2; ++i) 
	{
	  SizeT cumIx = i * n_2;
	  for (long j=0; j<yLim2; ++j) 
	    {
	      b_cum[cumIx] = B12[(i)*bs + j] - B22[(i)*bs + j];
	      cumIx++;
	    }
	}
      //   SMSub1<T>(n_2,m,n, B, bx, by+n_2, bs, bx+n_2, by+n_2, b_cum);
      SMNoCheckAB<T>(subBuf,n_2, A, ax, ay, as, b_cum, 0, 0, n_2, p3, xLim, yLim2);

      for( SizeT ix=0;ix<cx12Lim;++ix)
	{
	  SizeT pIndex = ix * n_2;
	  for( SizeT iy=0;iy<cy12Lim;++iy)
	    {
	      /* c12 = p3 + p5 */
	      //           SizeT pIndex = ix * n_2 + iy;
	      C[(ix)*mSz + iy+n_2] = p3[pIndex]+p5[pIndex];
	      ++pIndex;
	    }
	}
    }

  if( do21)
    {
      /* p2 = (a21 + a22) x b11 */
      cumIx = 0;
      for (long i=0; i<xLim2; ++i) 
	{
	  for (long j=0; j<n_2; ++j) 
	    {
	      a_cum[cumIx] = A21[(i)*as + j] + A22[(i)*as + j];
	      cumIx++;
	    }
	}
      //   SMAdd<T>(n_2,l,m, A, ax+n_2, ay, as, ax+n_2, ay+n_2, a_cum);
      SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, B, bx, by, bs, p2, xLim2, yLim);

      for( SizeT ix=0;ix<cx21Lim;++ix)
	{
	  SizeT pIndex = ix * n_2;
	  for( SizeT iy=0;iy<cy21Lim;++iy)
	    {
	      /* c21 = p2 + p4 */
	      //           SizeT pIndex = ix * n_2 + iy;
	      C[(ix+n_2)*mSz + iy] = p2[pIndex]+p4[pIndex];
	      ++pIndex;
	    }
	}
    }
  if( do22)
    {
      /* p6 = (a21 - a11) x (b11 + b12) */
      cumIx = 0;
      for (long i=0; i<xLim2; ++i) 
	{
	  for (long j=0; j<n_2; ++j) 
	    {
	      a_cum[cumIx] = A21[(i)*as + j] - A11[(i)*as + j];
	      cumIx++;
	    }
	}
      for (long i=0; i<n_2; ++i) 
	{
	  SizeT cumIx = i * n_2;
	  for (long j=0; j<yLim2; ++j) 
	    {
	      b_cum[cumIx] = B11[(i)*bs + j] + B12[(i)*bs + j];
	      cumIx++;
	    }
	}
      //   SMSub2<T>(n_2,l,m, A, ax+n_2, ay, as, ax, ay, a_cum);
      //   SMAdd<T>(n_2,m,n, B, bx, by, bs, bx, by+n_2, b_cum);
      SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p6, xLim2, yLim2);

      for( SizeT ix=0;ix<cx22Lim;++ix)
	{
	  SizeT pIndex = ix * n_2;
	  for( SizeT iy=0;iy<cy22Lim;++iy)
	    {
	      /* c22 = p1 + p3 - p2 + p6 */
	      // 	  SizeT pIndex = ix * n_2 + iy;
	      C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
	      ++pIndex;
	    }
	}
    }
}






template< typename T>
void SMCheckA( T* a_cum, SizeT mSz, SizeT l, SizeT m, SizeT n, // A[l,m]#B[m,n]=C[l,n]
	       T *A, SizeT ax, SizeT ay, SizeT as,
               T *B, SizeT bx, SizeT by, SizeT bs,
	       T *C, long cxLim, long cyLim) // C has always dimensions mSy*mSz
{
  if( cxLim <= 0 || cyLim <=0) return;
  if( (ax)>=n || (ay)>=m)
    {
      //       SizeT mSz2 = mSz * mSz;
      for (SizeT ix=0; ix<cxLim; ++ix)
	{
	  SizeT pIndex = ix * mSz;
	  for (SizeT iy=0; iy<cyLim; ++iy) 
	    {
	      C[pIndex] = 0;
	      pIndex++;
	    }
	}
      return;
    }

  if( (ax+mSz)<=n &&
      (ay+mSz)<=m)
    {
      SMNoCheckAB<T>(a_cum,mSz, A, ax, ay, as, B, bx, by, bs, C, cxLim, cyLim);
      return;
    }

  if( mSz <= minStrassenMatrixResolveSize)
    //   if( (mSz <= minStrassenMatrixResolveSize) || ((cxLim * cyLim) <= minStrassenMatrixResolveSize2))
    //   if( (mSz <= minStrassenMatrixResolveSize) || ((cxLim * cyLim * mSz) <= minStrassenMatrixResolveSize3))
    //   if( (mSz <= minStrassenMatrixResolveSize) || ((cxLim * cyLim * ((ay < mSz)?ay:mSz)) <= minStrassenMatrixResolveSize3))
    //   if( mSz <= minStrassenMatrixResolveSize)
    {
      SMM<T>( mSz, l, m, n, 
	      A, ax, ay, as,
	      B, bx, by, bs,
	      C, cxLim, cyLim);//, cx, cy, cs);
      return;
    }

  // divide and rule
  long n_2 = mSz >> 1;
  SizeT n_22 = n_2 * n_2;

  //   T* buf = new T[n_22 * 9];

  //   T*& a_cum = buf; 
  T* b_cum = a_cum + n_22;
  T* p1    = b_cum + n_22;
  T* p2    = p1 + n_22;
  T* p3    = p2 + n_22;
  T* p4    = p3 + n_22;
  T* p5    = p4 + n_22;
  T* p6    = p5 + n_22;
  T* p7    = p6 + n_22;
  T* subBuf = p7 + n_22;

  bool ax2Out =  (ax+n_2)>=n;
  bool ay2Out =  (ay+n_2)>=m;

  long xLim = (cxLim > n_2) ? n_2: cxLim; 
  long yLim = (cyLim > n_2) ? n_2: cyLim; 
  long xLim2 = cxLim - n_2;
  long yLim2 = cyLim - n_2; 
  assert( xLim2 <= n_2);
  assert( yLim2 <= n_2);

  /* p1 = (a11 + a22) x (b11 + b22) */
  SMAdd<T>(n_2,m,l, B, bx, by, bs, bx+n_2, by+n_2, b_cum, n_2, yLim);
  if( ax2Out || ay2Out)
    {
      SMNoCheckAB<T>(subBuf,n_2, A, ax, ay, as, b_cum, 0, 0, n_2, p1, xLim, yLim);
    }
  else
    {
      SMAdd<T>(n_2,n,m, A, ax, ay, as, ax+n_2, ay+n_2, a_cum, xLim, n_2);
      SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p1, xLim, yLim);
    }

  /* p5 = (a11 + a12) x b22 */
  if( ay2Out)
    {
      SMCheckA<T>(subBuf,n_2,l,m,n, A, ax, ay, as, B, bx+n_2, by+n_2, bs, p5, xLim, yLim);
    }
  else
    {
      SMAdd<T>(n_2,n,m, A, ax, ay, as, ax, ay+n_2, a_cum, xLim, n_2);
      SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, B, bx+n_2, by+n_2, bs, p5, xLim, yLim);
    }

  if( cyLim > n_2)
    {
      /* p3 = a11 x (b12 - b22) */
      SMSub1<T>(n_2,m,l, B, bx, by+n_2, bs, bx+n_2, by+n_2, b_cum, n_2, yLim2);
      SMCheckA<T>(subBuf,n_2,l,m,n, A, ax, ay, as, b_cum, 0, 0, n_2, p3, xLim, yLim2);
    }

  if( cxLim > n_2 && cyLim > n_2)
    {
      /* p6 = (a21 - a11) x (b11 + b12) */
      if( ax2Out)
	SMNegate<T>(n_2,n,m, A, ax, ay, as, a_cum, xLim2, n_2);
      else
	SMSub2<T>(n_2,n,m, A, ax+n_2, ay, as, ax, ay, a_cum, xLim2, n_2);
      SMAdd<T>(n_2,m,l, B, bx, by, bs, bx, by+n_2, b_cum, n_2, yLim2);
      SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p6, xLim2, yLim2);
    }

  long cx11Lim;
  long cy11Lim;
  long cx12Lim;
  long cy12Lim;
  long cx21Lim;
  long cy21Lim;
  long cx22Lim;
  long cy22Lim;
  long cxLim_n_2 = cxLim - n_2;
  long cyLim_n_2 = cyLim - n_2;
  bool do12, do21, do22;
  if( n_2 >= cxLim) // x limited
    {
      cx11Lim = cxLim;
      if( n_2 >= cyLim)
	{
	  cy11Lim = cyLim;
	  do12 = false;
	  do21 = false;
	  do22 = false;
	}
      else
	{
	  cy11Lim = n_2;
	  do12 = true;
	  do21 = false;
	  do22 = false;
	  cx12Lim = cxLim;
	  cy12Lim = (cyLim>mSz)?n_2:cyLim_n_2;
	}
    }
  else 
    {
      cx11Lim = n_2;
      if( n_2 >= cyLim)
	{
	  cy11Lim = cyLim;
	  do12 = false;
	  do21 = true;
	  do22 = false;
	  cy21Lim = cyLim;
	  cx21Lim = (cxLim>mSz)?n_2:cxLim_n_2;
	}
      else
	{
	  cy11Lim = n_2;
	  do12 = true;
	  do21 = true;
	  do22 = true;
	  cx12Lim = n_2;
	  cy12Lim = (cyLim>mSz)?n_2:cyLim_n_2;
	  cx21Lim = (cxLim>mSz)?n_2:cxLim_n_2;
	  cy21Lim = n_2;
	  cx22Lim = (cxLim>mSz)?n_2:cxLim_n_2;
	  cy22Lim = (cyLim>mSz)?n_2:cyLim_n_2;
	}
    }

  if( ax2Out && ay2Out)  // no p2 p4 p7
    {
      for( SizeT ix=0;ix<cx11Lim;++ix)
	{
	  SizeT pIndex = ix * n_2;
	  for( SizeT iy=0;iy<cy11Lim;++iy)
	    {
	      /* c11 = p1 + p4 - p5 + p7 */
	      // 	    SizeT pIndex = ix * n_2 + iy;
	      C[(ix)*mSz + iy] = p1[pIndex]-p5[pIndex];
	      ++pIndex;
	    }
	}
      if( do12)
	for( SizeT ix=0;ix<cx12Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy12Lim;++iy)
	      {
		/* c12 = p3 + p5 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix)*mSz + iy+n_2] = p3[pIndex]+p5[pIndex];
		++pIndex;
	      }
	  }
      if( do21)
	for( SizeT ix=0;ix<cx21Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy21Lim;++iy)
	      {
		/* c21 = p2 + p4 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy] = 0;//p2[pIndex]+p4[pIndex];
		++pIndex;
	      }
	  }
      if( do22)
	for( SizeT ix=0;ix<cx22Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy22Lim;++iy)
	      {
		/* c22 = p1 + p3 - p2 + p6 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]+p6[pIndex];
		++pIndex;
	      }
	  }

      // for( SizeT ix=0;ix<cx11Lim;++ix)
      //   for( SizeT iy=0;iy<cy11Lim;++iy)
      //    {
      //        /* c11 = p1 + p4 - p5 + p7 */
      //       C[(ix)*mSz + iy] = p1[pIndex]-p5[pIndex];
      //       /* c12 = p3 + p5 */
      //       C[(ix)*mSz + iy+n_2] = p3[pIndex]+p5[pIndex];
      //       /* c21 = p2 + p4 */
      //       C[(ix+n_2)*mSz + iy] = 0;
      //       /* c22 = p1 + p3 - p2 + p6 */
      //       C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]+p6[pIndex];
      // 
      //       pIndex++;
      //     }
      return;
    }

  /* p7 = (a12 - a22) x (b21 + b22) */
  if( !ay2Out)
    {
      if( ax2Out)
	SMNegate<T>(n_2,n,m, A, ax+n_2, ay+n_2, as, a_cum, xLim, n_2);
      else
	SMSub1<T>(n_2,n,m, A, ax, ay+n_2, as, ax+n_2, ay+n_2, a_cum, xLim, n_2);
      SMAdd<T>(n_2,m,l, B, bx+n_2, by, bs, bx+n_2, by+n_2, b_cum, n_2, yLim);
      SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p7, xLim, yLim);
    }


  if( ax2Out)  // no p2 p4
    {
      for( SizeT ix=0;ix<cx11Lim;++ix)
	{
	  SizeT pIndex = ix * n_2;
	  for( SizeT iy=0;iy<cy11Lim;++iy)
	    {
	      /* c11 = p1 + p4 - p5 + p7 */
	      // 	    SizeT pIndex = ix * n_2 + iy;
	      C[(ix)*mSz + iy] = p1[pIndex]-p5[pIndex]+p7[pIndex];
	      ++pIndex;
	    }
	}
      if( do12)
	for( SizeT ix=0;ix<cx12Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy12Lim;++iy)
	      {
		/* c12 = p3 + p5 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix)*mSz + iy+n_2] = p3[pIndex]+p5[pIndex];
		++pIndex;
	      }
	  }
      if( do21)
	for( SizeT ix=0;ix<cx21Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy21Lim;++iy)
	      {
		/* c21 = p2 + p4 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy] = 0;//p2[pIndex]+p4[pIndex];
		++pIndex;
	      }
	  }
      if( do22)
	for( SizeT ix=0;ix<cx22Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy22Lim;++iy)
	      {
		/* c22 = p1 + p3 - p2 + p6 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]+p6[pIndex];
		++pIndex;
	      }
	  }

      //     for( SizeT ix=0;ix<n_2;++ix)
      //       for( SizeT iy=0;iy<n_2;++iy)
      // 	{
      // 	/* c11 = p1 + p4 - p5 + p7 */
      // 	C[(ix)*mSz + iy] = p1[pIndex]-p5[pIndex]+p7[pIndex];
      // 		
      // 	  /* c12 = p3 + p5 */
      // 	  C[(ix)*mSz + iy+n_2] = p3[pIndex]+p5[pIndex];
      // 		
      // 	  /* c21 = p2 + p4 */
      // 	  C[(ix+n_2)*mSz + iy] = 0;
      // 		
      // 	  /* c22 = p1 + p3 - p2 + p6 */
      // 	  C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]+p6[pIndex];
      // 
      // 	  pIndex++;
      // 	}
      return;
    }

  if( cxLim > n_2)
    {
      /* p2 = (a21 + a22) x b11 */
      if( ay2Out)
	{
	  SMCheckA<T>(subBuf,n_2,l,m,n, A, ax+n_2, ay, as, B, bx, by, bs, p2, xLim2, yLim);
	}
      else
	{
	  SMAdd<T>(n_2,n,m, A, ax+n_2, ay, as, ax+n_2, ay+n_2, a_cum, xLim2, n_2);
	  SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, B, bx, by, bs, p2, xLim2, yLim);
	}
    }

  if( ay2Out)  // no p7 p4
    {

      for( SizeT ix=0;ix<cx11Lim;++ix)
	{
	  SizeT pIndex = ix * n_2;
	  for( SizeT iy=0;iy<cy11Lim;++iy)
	    {
	      /* c11 = p1 + p4 - p5 + p7 */
	      // 	    SizeT pIndex = ix * n_2 + iy;
	      C[(ix)*mSz + iy] = p1[pIndex]-p5[pIndex];
	      ++pIndex;
	    }
	}
      if( do12)
	for( SizeT ix=0;ix<cx12Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy12Lim;++iy)
	      {
		/* c12 = p3 + p5 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix)*mSz + iy+n_2] = p3[pIndex]+p5[pIndex];
		++pIndex;
	      }
	  }
      if( do21)
	for( SizeT ix=0;ix<cx21Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy21Lim;++iy)
	      {
		/* c21 = p2 + p4 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy] = p2[pIndex];
		++pIndex;
	      }
	  }
      if( do22)
	for( SizeT ix=0;ix<cx22Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy22Lim;++iy)
	      {
		/* c22 = p1 + p3 - p2 + p6 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
		++pIndex;
	      }
	  }

      //     for( SizeT ix=0;ix<n_2;++ix)
      //       for( SizeT iy=0;iy<n_2;++iy)
      // 	{
      // 	/* c11 = p1 + p4 - p5 + p7 */
      // 	C[(ix)*mSz + iy] = p1[pIndex]-p5[pIndex];
      // 		
      // 	  /* c12 = p3 + p5 */
      // 	  C[(ix)*mSz + iy+n_2] = p3[pIndex]+p5[pIndex];
      // 		
      // 	  /* c21 = p2 + p4 */
      // 	  C[(ix+n_2)*mSz + iy] = p2[pIndex];
      // 		
      // 	  /* c22 = p1 + p3 - p2 + p6 */
      // 	  C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
      // 
      // 	  pIndex++;
      // 	}
      return;
    }

  /* p4 = a22 x (b21 - b11) */
  SMSub2<T>(n_2,m,l, B, bx+n_2, by, bs, bx, by, b_cum, n_2, yLim);
  SMCheckA<T>(subBuf,n_2,l,m,n, A, ax+n_2, ay+n_2, as, b_cum, 0, 0, n_2, p4, xLim, yLim);

  // we are in SMCheckA
  if( cxLim >= mSz && cyLim >= mSz)
    {
      SizeT pIndex = 0;
      SizeT n_2_mSz = n_2 * mSz;
      SizeT n_2_mSz_n_2 = n_2_mSz + n_2;
      for( SizeT ix=0;ix<n_2;++ix)
	{
	  SizeT cIndex = ix * mSz;
	  for( SizeT iy=0;iy<n_2;++iy)
	    {
	      C[cIndex] = p1[pIndex]+p4[pIndex]-p5[pIndex]+p7[pIndex];
	      C[cIndex+n_2] = p3[pIndex]+p5[pIndex];
	      C[cIndex+n_2_mSz] = p2[pIndex]+p4[pIndex];
	      C[cIndex++ +n_2_mSz_n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
	      ++pIndex;
	    }
	}
      return;
    } 

  for( SizeT ix=0;ix<cx11Lim;++ix)
    {
      SizeT pIndex = ix * n_2;
      for( SizeT iy=0;iy<cy11Lim;++iy)
	{
	  /* c11 = p1 + p4 - p5 + p7 */
	  // 	SizeT pIndex = ix * n_2 + iy;
	  C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex]-p5[pIndex]+p7[pIndex];
	  ++pIndex;
	}
    }
  if( do12)
    for( SizeT ix=0;ix<cx12Lim;++ix)
      {
	SizeT pIndex = ix * n_2;
	for( SizeT iy=0;iy<cy12Lim;++iy)
	  {
	    /* c12 = p3 + p5 */
	    //           SizeT pIndex = ix * n_2 + iy;
	    C[(ix)*mSz + iy+n_2] = p3[pIndex]+p5[pIndex];
	    ++pIndex;
	  }
      }
  if( do21)
    for( SizeT ix=0;ix<cx21Lim;++ix)
      {
	SizeT pIndex = ix * n_2;
	for( SizeT iy=0;iy<cy21Lim;++iy)
	  {
	    /* c21 = p2 + p4 */
	    //           SizeT pIndex = ix * n_2 + iy;
	    C[(ix+n_2)*mSz + iy] = p2[pIndex]+p4[pIndex];
	    ++pIndex;
	  }
      }
  if( do22)
    for( SizeT ix=0;ix<cx22Lim;++ix)
      {
	SizeT pIndex = ix * n_2;
	for( SizeT iy=0;iy<cy22Lim;++iy)
	  {
	    /* c22 = p1 + p3 - p2 + p6 */
	    // 	  SizeT pIndex = ix * n_2 + iy;
	    C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
	    ++pIndex;
	  }
      }

  //       delete[] p1;
  return;
}












template< typename T>
void SMCheckB( T* a_cum, SizeT mSz, SizeT l, SizeT m, SizeT n, // A[l,m]#B[m,n]=C[l,n]
	       T *A, SizeT ax, SizeT ay, SizeT as,
	       T *B, SizeT bx, SizeT by, SizeT bs,
	       T *C, long cxLim, long cyLim) // C has always dimensions mSy*mSz
{
  if( cxLim <= 0 || cyLim <=0) return;
  if( (bx)>=m || (by)>=l)
    {
      //       SizeT mSz2 = mSz * mSz;
      for (SizeT ix=0; ix<cxLim; ++ix)
	{
	  SizeT pIndex = ix * mSz;
	  for (SizeT iy=0; iy<cyLim; ++iy) 
	    {
	      C[pIndex] = 0;
	      pIndex++;
	    }
	}
      return;
    }

  if( (bx+mSz)<=m &&
      (by+mSz)<=l)
    {
      SMNoCheckAB<T>(a_cum,mSz, A, ax, ay, as, B, bx, by, bs, C, cxLim, cyLim);
      return;
    }

  if( mSz <= minStrassenMatrixResolveSize)
    //   if( (mSz <= minStrassenMatrixResolveSize) || ((cxLim * cyLim) <= minStrassenMatrixResolveSize2))
    //   if( (mSz <= minStrassenMatrixResolveSize) || ((cxLim * cyLim * mSz) <= minStrassenMatrixResolveSize3))
    //   if( (mSz <= minStrassenMatrixResolveSize) || ((cxLim * cyLim * ((bx < mSz)?bx:mSz)) <= minStrassenMatrixResolveSize3))
    //   if( mSz <= minStrassenMatrixResolveSize)
    {
      SMM<T>( mSz, l, m, n, 
	      A, ax, ay, as,
	      B, bx, by, bs,
	      C, cxLim, cyLim);//, cx, cy, cs);
      return;
    }

  // divide and rule
  long n_2 = mSz >> 1;
  SizeT n_22 = n_2 * n_2;

  //   T* buf = new T[n_22 * 9];

  //   T*& a_cum = buf; 
  T* b_cum = a_cum + n_22;
  T* p1    = b_cum + n_22;
  T* p2    = p1 + n_22;
  T* p3    = p2 + n_22;
  T* p4    = p3 + n_22;
  T* p5    = p4 + n_22;
  T* p6    = p5 + n_22;
  T* p7    = p6 + n_22;
  T* subBuf = p7 + n_22;

  bool bx2Out =  (bx+n_2)>=m;
  bool by2Out =  (by+n_2)>=l;

  long xLim = (cxLim>n_2)?n_2:cxLim;
  long yLim = (cyLim>n_2)?n_2:cyLim;
  long xLim2 = cxLim - n_2;
  long yLim2 = cyLim - n_2; 
  assert( xLim2 <= n_2);
  assert( yLim2 <= n_2);

  /* p1 = (a11 + a22) x (b11 + b22) */
  SMAdd<T>(n_2,n,m, A, ax, ay, as, ax+n_2, ay+n_2, a_cum, xLim, n_2);
  if( bx2Out || by2Out)
    {
      SMCheckB<T>(subBuf,n_2,l,m,n, a_cum, 0, 0, n_2, B, bx, by, bs, p1, xLim, yLim);
    }
  else
    {
      SMAdd<T>(n_2,m,l, B, bx, by, bs, bx+n_2, by+n_2, b_cum, n_2, yLim);
      SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p1, xLim, yLim);
    }

  if( cxLim > n_2)
    {
      /* p2 = (a21 + a22) x b11 */
      SMAdd<T>(n_2,n,m, A, ax+n_2, ay, as, ax+n_2, ay+n_2, a_cum, xLim2, n_2);
      SMCheckB<T>(subBuf,n_2,l,m,n, a_cum, 0, 0, n_2, B, bx, by, bs, p2, xLim2, yLim);
    }
	    
  /* p4 = a22 x (b21 - b11) */
  if( bx2Out)
    SMNegate<T>(n_2,m,l, B, bx, by, bs, b_cum, n_2, yLim);
  else
    SMSub2<T>(n_2,m,l, B, bx+n_2, by, bs, bx, by, b_cum, n_2, yLim);
  SMNoCheckAB<T>(subBuf,n_2, A, ax+n_2, ay+n_2, as, b_cum, 0, 0, n_2, p4, xLim, yLim);

  if( cxLim > n_2 && cyLim > n_2)
    {
      /* p6 = (a21 - a11) x (b11 + b12) */
      SMSub2<T>(n_2,n,m, A, ax+n_2, ay, as, ax, ay, a_cum, xLim2, n_2);
      if( by2Out)
	{
	  SMCheckB<T>(subBuf,n_2,l,m,n, a_cum, 0, 0, n_2, B, bx, by, bs, p6, xLim2, yLim2);
	}
      else
	{
	  SMAdd<T>(n_2,m,l, B, bx, by, bs, bx, by+n_2, b_cum, n_2, yLim2);
	  SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p6, xLim2, yLim2);
	}
    }

  long cx11Lim;
  long cy11Lim;
  long cx12Lim;
  long cy12Lim;
  long cx21Lim;
  long cy21Lim;
  long cx22Lim;
  long cy22Lim;
  long cxLim_n_2 = cxLim - n_2;
  long cyLim_n_2 = cyLim - n_2;
  bool do12, do21, do22;
  if( n_2 >= cxLim) // x limited
    {
      cx11Lim = cxLim;
      if( n_2 >= cyLim)
	{
	  cy11Lim = cyLim;
	  do12 = false;
	  do21 = false;
	  do22 = false;
	}
      else
	{
	  cy11Lim = n_2;
	  do12 = true;
	  do21 = false;
	  do22 = false;
	  cx12Lim = cxLim;
	  cy12Lim = (cyLim>mSz)?n_2:cyLim_n_2;
	}
    }
  else 
    {
      cx11Lim = n_2;
      if( n_2 >= cyLim)
	{
	  cy11Lim = cyLim;
	  do12 = false;
	  do21 = true;
	  do22 = false;
	  cy21Lim = cyLim;
	  cx21Lim = (cxLim>mSz)?n_2:cxLim_n_2;
	}
      else
	{
	  cy11Lim = n_2;
	  do12 = true;
	  do21 = true;
	  do22 = true;
	  cx12Lim = n_2;
	  cy12Lim = (cyLim>mSz)?n_2:cyLim_n_2;
	  cx21Lim = (cxLim>mSz)?n_2:cxLim_n_2;
	  cy21Lim = n_2;
	  cx22Lim = (cxLim>mSz)?n_2:cxLim_n_2;
	  cy22Lim = (cyLim>mSz)?n_2:cyLim_n_2;
	}
    }

  SizeT pIndex = 0;//ix*n_2+iy;
  if( bx2Out && by2Out) // no p3 p5 p7
    {
      for( SizeT ix=0;ix<cx11Lim;++ix)
	{
	  SizeT pIndex = ix * n_2;
	  for( SizeT iy=0;iy<cy11Lim;++iy)
	    {
	      /* c11 = p1 + p4 - p5 + p7 */
	      // 	    SizeT pIndex = ix * n_2 + iy;
	      C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex];//-p5[pIndex]+p7[pIndex];
	      ++pIndex;
	    }
	}
      if( do12)
	for( SizeT ix=0;ix<cx12Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy12Lim;++iy)
	      {
		/* c12 = p3 + p5 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix)*mSz + iy+n_2] = 0;//p3[pIndex]+p5[pIndex];
		++pIndex;
	      }
	  }
      if( do21)
	for( SizeT ix=0;ix<cx21Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy21Lim;++iy)
	      {
		/* c21 = p2 + p4 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy] = p2[pIndex]+p4[pIndex];
		++pIndex;
	      }
	  }
      if( do22)
	for( SizeT ix=0;ix<cx22Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy22Lim;++iy)
	      {
		/* c22 = p1 + p3 - p2 + p6 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]-p2[pIndex]+p6[pIndex];
		++pIndex;
	      }
	  }
      //       for( SizeT ix=0;ix<n_2;++ix)
      // 	for( SizeT iy=0;iy<n_2;++iy)
      // 	  {
      // 	    /* c11 = p1 + p4 - p5 + p7 */
      // 	    C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex];
      // 		
      // 	    /* c12 = p3 + p5 */
      // 	    C[(ix)*mSz + iy+n_2] = 0;
      // 		
      // 	    /* c21 = p2 + p4 */
      // 	    C[(ix+n_2)*mSz + iy] = p2[pIndex]+p4[pIndex];
      // 		
      // 	    /* c22 = p1 + p3 - p2 + p6 */
      // 	    C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]-p2[pIndex]+p6[pIndex];
      // 
      // 	    pIndex++;
      // 	  }
      return;
    }

  /* p3 = a11 x (b12 - b22) */
  if( !by2Out)
    {
      if( bx2Out)
	{
	  SMCheckB<T>(subBuf,n_2, l,m,n, A, ax, ay, as, B, bx, by+n_2, bs, p3, xLim, yLim2);
	}
      else
	{
	  SMSub1<T>(n_2,m,l, B, bx, by+n_2, bs, bx+n_2, by+n_2, b_cum, n_2, yLim2);
	  SMNoCheckAB<T>(subBuf,n_2, A, ax, ay, as, b_cum, 0, 0, n_2, p3, xLim, yLim2);
	}
    }

  if( bx2Out) // no p5 p7
    {
      for( SizeT ix=0;ix<cx11Lim;++ix)
	{
	  SizeT pIndex = ix * n_2;
	  for( SizeT iy=0;iy<cy11Lim;++iy)
	    {
	      /* c11 = p1 + p4 - p5 + p7 */
	      // 	    SizeT pIndex = ix * n_2 + iy;
	      C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex];//-p5[pIndex]+p7[pIndex];
	      ++pIndex;
	    }
	}
      if( do12)
	for( SizeT ix=0;ix<cx12Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy12Lim;++iy)
	      {
		/* c12 = p3 + p5 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix)*mSz + iy+n_2] = p3[pIndex];//+p5[pIndex];
		++pIndex;
	      }
	  }
      if( do21)
	for( SizeT ix=0;ix<cx21Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy21Lim;++iy)
	      {
		/* c21 = p2 + p4 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy] = p2[pIndex]+p4[pIndex];
		++pIndex;
	      }
	  }
      if( do22)
	for( SizeT ix=0;ix<cx22Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy22Lim;++iy)
	      {
		/* c22 = p1 + p3 - p2 + p6 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
		++pIndex;
	      }
	  }

      //       for( SizeT ix=0;ix<n_2;++ix)
      // 	for( SizeT iy=0;iy<n_2;++iy)
      // 	  {
      // 	    //SizeT pIndex = ix*n_2+iy;
      // 
      // 	    /* c11 = p1 + p4 - p5 + p7 */
      // 	    C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex];
      // 		
      // 	    /* c12 = p3 + p5 */
      // 	    C[(ix)*mSz + iy+n_2] = p3[pIndex];
      // 		
      // 	    /* c21 = p2 + p4 */
      // 	    C[(ix+n_2)*mSz + iy] = p2[pIndex]+p4[pIndex];
      // 		
      // 	    /* c22 = p1 + p3 - p2 + p6 */
      // 	    C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
      // 
      // 	    pIndex++;
      // 	  }
      return;
    }

  /* p7 = (a12 - a22) x (b21 + b22) */
  SMSub1<T>(n_2,n,m, A, ax, ay+n_2, as, ax+n_2, ay+n_2, a_cum, xLim, n_2);
  if( by2Out)
    {
      SMCheckB<T>(subBuf,n_2,l,m,n, a_cum, 0, 0, n_2, B, bx+n_2, by, bs, p7, xLim, yLim);
    }
  else
    {
      SMAdd<T>(n_2,m,l, B, bx+n_2, by, bs, bx+n_2, by+n_2, b_cum, n_2, yLim);
      SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p7, xLim, yLim);
    }

  if( by2Out) // no p3 p5
    {
      for( SizeT ix=0;ix<cx11Lim;++ix)
	{
	  SizeT pIndex = ix * n_2;
	  for( SizeT iy=0;iy<cy11Lim;++iy)
	    {
	      /* c11 = p1 + p4 - p5 + p7 */
	      // 	    SizeT pIndex = ix * n_2 + iy;
	      C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex]+p7[pIndex];
	      ++pIndex;
	    }
	}
      if( do12)
	for( SizeT ix=0;ix<cx12Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy12Lim;++iy)
	      {
		/* c12 = p3 + p5 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix)*mSz + iy+n_2] = 0;
		++pIndex;
	      }
	  }
      if( do21)
	for( SizeT ix=0;ix<cx21Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy21Lim;++iy)
	      {
		/* c21 = p2 + p4 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy] = p2[pIndex]+p4[pIndex];
		++pIndex;
	      }
	  }
      if( do22)
	for( SizeT ix=0;ix<cx22Lim;++ix)
	  {
	    SizeT pIndex = ix * n_2;
	    for( SizeT iy=0;iy<cy22Lim;++iy)
	      {
		/* c22 = p1 + p3 - p2 + p6 */
		// 	      SizeT pIndex = ix * n_2 + iy;
		C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]-p2[pIndex]+p6[pIndex];
		++pIndex;
	      }
	  }

      //       for( SizeT ix=0;ix<n_2;++ix)
      // 	for( SizeT iy=0;iy<n_2;++iy)
      // 	  {
      // 	    //             SizeT pIndex = ix*n_2+iy;
      // 
      // 	    /* c11 = p1 + p4 - p5 + p7 */
      // 	    C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex]+p7[pIndex];
      // 		
      // 	    /* c12 = p3 + p5 */
      // 	    C[(ix)*mSz + iy+n_2] = 0;
      // 		
      // 	    /* c21 = p2 + p4 */
      // 	    C[(ix+n_2)*mSz + iy] = p2[pIndex]+p4[pIndex];
      // 		
      // 	    /* c22 = p1 + p3 - p2 + p6 */
      // 	    C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]-p2[pIndex]+p6[pIndex];
      // 
      // 	    pIndex++;
      // 	  }
      return;
    }

  /* p5 = (a11 + a12) x b22 */
  SMAdd<T>(n_2,n,m, A, ax, ay, as, ax, ay+n_2, a_cum, xLim, n_2);
  SMCheckB<T>(subBuf,n_2,l,m,n, a_cum, 0, 0, n_2, B, bx+n_2, by+n_2, bs, p5, xLim, yLim);

  // we are in SMCheckB
  if( cxLim >= mSz && cyLim >= mSz)
    {
      SizeT pIndex = 0;
      SizeT n_2_mSz = n_2 * mSz;
      SizeT n_2_mSz_n_2 = n_2_mSz + n_2;
      for( SizeT ix=0;ix<n_2;++ix)
	{
	  SizeT cIndex = ix * mSz;
	  for( SizeT iy=0;iy<n_2;++iy)
	    {
	      C[cIndex] = p1[pIndex]+p4[pIndex]-p5[pIndex]+p7[pIndex];
	      C[cIndex+n_2] = p3[pIndex]+p5[pIndex];
	      C[cIndex+n_2_mSz] = p2[pIndex]+p4[pIndex];
	      C[cIndex++ +n_2_mSz_n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
	      ++pIndex;
	    }
	}
      return;
    } 

  for( SizeT ix=0;ix<cx11Lim;++ix)
    {
      SizeT pIndex = ix * n_2;
      for( SizeT iy=0;iy<cy11Lim;++iy)
	{
	  /* c11 = p1 + p4 - p5 + p7 */
	  // 	SizeT pIndex = ix * n_2 + iy;
	  C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex]-p5[pIndex]+p7[pIndex];
	  ++pIndex;
	}
    }
  if( do12)
    for( SizeT ix=0;ix<cx12Lim;++ix)
      {
	SizeT pIndex = ix * n_2;
	for( SizeT iy=0;iy<cy12Lim;++iy)
	  {
	    /* c12 = p3 + p5 */
	    //           SizeT pIndex = ix * n_2 + iy;
	    C[(ix)*mSz + iy+n_2] = p3[pIndex]+p5[pIndex];
	    ++pIndex;
	  }
      }
  if( do21)
    for( SizeT ix=0;ix<cx21Lim;++ix)
      {
	SizeT pIndex = ix * n_2;
	for( SizeT iy=0;iy<cy21Lim;++iy)
	  {
	    /* c21 = p2 + p4 */
	    //           SizeT pIndex = ix * n_2 + iy;
	    C[(ix+n_2)*mSz + iy] = p2[pIndex]+p4[pIndex];
	    ++pIndex;
	  }
      }
  if( do22)
    for( SizeT ix=0;ix<cx22Lim;++ix)
      {
	SizeT pIndex = ix * n_2;
	for( SizeT iy=0;iy<cy22Lim;++iy)
	  {
	    /* c22 = p1 + p3 - p2 + p6 */
	    // 	  SizeT pIndex = ix * n_2 + iy;
	    C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
	    ++pIndex;
	  }
      }
  //   for( SizeT ix=0;ix<n_2;++ix)
  //     for( SizeT iy=0;iy<n_2;++iy)
  //       {
  // 	//             SizeT pIndex = ix*n_2+iy;
  // 
  // 	/* c11 = p1 + p4 - p5 + p7 */
  // 	C[(ix)*mSz + iy] = p1[pIndex]+p4[pIndex]-p5[pIndex]+p7[pIndex];
  // 		
  // 	/* c12 = p3 + p5 */
  // 	C[(ix)*mSz + iy+n_2] = p3[pIndex]+p5[pIndex];
  // 		
  // 	/* c21 = p2 + p4 */
  // 	C[(ix+n_2)*mSz + iy] = p2[pIndex]+p4[pIndex];
  // 		
  // 	/* c22 = p1 + p3 - p2 + p6 */
  // 	C[(ix+n_2)*mSz + iy+n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
  // 
  // 	pIndex++;
  //       }

  //       delete[] p1;
  return;
}













template< typename T>
void dumpmatrix( T* A, SizeT mSz)
{
  for( SizeT j=0; j<mSz;++j)
    {
      for( SizeT i=0; i<mSz;++i)
	cout << A[i+j*mSz] << "\t";
      cout << endl;
    }
  cout << endl;
}

// #define SM_DUMP
// #define SM_DUMP_CUM

// A[l,m]#B[m,n]=C[l,n]
template< typename T>
void SM1( SizeT mSz, SizeT l, SizeT m, SizeT n, T *A, T *B, T *C)
{
  const SizeT& as = m;
  const SizeT& bs = l;
  const SizeT& cs = l;

  assert( mSz > minStrassenMatrixResolveSize);

  // divide and rule
  long n_2 = mSz >> 1;
  SizeT n_22 = n_2 * n_2;

  T* p1    = new T[n_22 * 7];
  T* p2    = p1 + n_22;
  T* p3    = p2 + n_22;
  T* p4    = p3 + n_22;
  T* p5    = p4 + n_22;
  T* p6    = p5 + n_22;
  T* p7    = p6 + n_22;

  long xLim = (n>n_2)?n_2:n;
  long yLim = (l>n_2)?n_2:l;
  long xLim2 = n - n_2;
  long yLim2 = l - n_2; 
  assert( xLim2 <= n_2);
  assert( yLim2 <= n_2);

// #pragma omp parallel
#pragma omp taskq
{
  /* p1 = (a11 + a22) x (b11 + b22) */
#pragma omp task private( subBuf, a_cum, b_cum) default(shared)
{
  T* subBuf = new T[ 5 * n_22];
  T* a_cum = subBuf + 3 * n_22;
  T* b_cum = subBuf + 4 * n_22;
  SMAdd<T>(n_2,n,m, A, 0, 0, as, 0+n_2, 0+n_2, a_cum, xLim, n_2);
  SMAdd<T>(n_2,m,l, B, 0, 0, bs, 0+n_2, 0+n_2, b_cum, n_2, yLim);
#ifdef SM_DUMP
  cout << "1***" << endl;
#endif
#ifdef SM_DUMP_CUM
  dumpmatrix<T>(a_cum,n_2);
  dumpmatrix<T>(b_cum,n_2);
#endif
  SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p1, xLim, yLim);
  //   SM<T>(subBuf,n_2,l,m,n, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p2);
#ifdef SM_DUMP
  dumpmatrix<T>(p1,n_2);
  // dumpmatrix<T>(p2,n_2);
#endif
  delete[] subBuf;
}

  /* p4 = a22 x (b21 - b11) */
#pragma omp task private( subBuf, a_cum, b_cum) default(shared)
{
  T* subBuf = new T[ 5 * n_22];
  T* a_cum = subBuf + 3 * n_22;
  T* b_cum = subBuf + 4 * n_22;
  SMSub2<T>(n_2,m,l, B, 0+n_2, 0, bs, 0, 0, b_cum, n_2, yLim);
#ifdef SM_DUMP
  cout << "4***" << endl;
#endif
#ifdef SM_DUMP_CUM
  dumpmatrix<T>(b_cum,n_2);
#endif
  SMCheckA<T>(subBuf,n_2,l,m,n, A, 0+n_2, 0+n_2, as, b_cum, 0, 0, n_2, p4, xLim, yLim);
#ifdef SM_DUMP
  dumpmatrix<T>(p4,n_2);
#endif
  delete[] subBuf;
}

  /* p5 = (a11 + a12) x b22 */
#pragma omp task private( subBuf, a_cum, b_cum) default(shared)
{
  T* subBuf = new T[ 5 * n_22];
  T* a_cum = subBuf + 3 * n_22;
  T* b_cum = subBuf + 4 * n_22;
  SMAdd<T>(n_2,n,m, A, 0, 0, as, 0, 0+n_2, a_cum, xLim, n_2);
#ifdef SM_DUMP
  cout << "5***" << endl;
#endif
#ifdef SM_DUMP_CUM
  dumpmatrix<T>(a_cum,n_2);
#endif
  SMCheckB<T>(subBuf,n_2,l,m,n, a_cum, 0, 0, n_2, B, 0+n_2, 0+n_2, bs, p5, xLim, yLim);
#ifdef SM_DUMP
  dumpmatrix<T>(p5,n_2);
#endif
  delete[] subBuf;
}
	    
  /* p7 = (a12 - a22) x (b21 + b22) */
#pragma omp task private( subBuf, a_cum, b_cum) default(shared)
{
  T* subBuf = new T[ 5 * n_22];
  T* a_cum = subBuf + 3 * n_22;
  T* b_cum = subBuf + 4 * n_22;
  SMSub1<T>(n_2,n,m, A, 0, 0+n_2, as, 0+n_2, 0+n_2, a_cum,xLim,n_2);
  SMAdd<T>(n_2,m,l, B, 0+n_2, 0, bs, 0+n_2, 0+n_2, b_cum, n_2, yLim);
#ifdef SM_DUMP
  cout << "7***" << endl;
#endif
#ifdef SM_DUMP_CUM
  dumpmatrix<T>(a_cum,n_2);
  dumpmatrix<T>(b_cum,n_2);
#endif
  SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p7, xLim, yLim);
  //   SM<T>(subBuf,n_2,l,m,n, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p7);
#ifdef SM_DUMP
  dumpmatrix<T>(p7,n_2);
#endif
  delete[] subBuf;
}

  /* p2 = (a21 + a22) x b11 */
#pragma omp task private( subBuf, a_cum, b_cum) default(shared)
{
  T* subBuf = new T[ 5 * n_22];
  T* a_cum = subBuf + 3 * n_22;
  T* b_cum = subBuf + 4 * n_22;
  SMAdd<T>(n_2,n,m, A, 0+n_2, 0, as, 0+n_2, 0+n_2, a_cum, xLim2, n_2);
#ifdef SM_DUMP
  cout << "2***" << endl;
#endif
#ifdef SM_DUMP_CUM
  dumpmatrix<T>(a_cum,n_2);
#endif
  SMCheckB<T>(subBuf,n_2,l,m,n, a_cum, 0, 0, n_2, B, 0, 0, bs, p2, xLim2, yLim);
#ifdef SM_DUMP
  dumpmatrix<T>(p2,n_2);
#endif
  delete[] subBuf;
}
	    
  /* p3 = a11 x (b12 - b22) */
#pragma omp task private( subBuf, a_cum, b_cum) default(shared)
{
  T* subBuf = new T[ 5 * n_22];
  T* a_cum = subBuf + 3 * n_22;
  T* b_cum = subBuf + 4 * n_22;
  SMSub1<T>(n_2,m,l, B, 0, 0+n_2, bs, 0+n_2, 0+n_2, b_cum,n_2,yLim2);
#ifdef SM_DUMP
  cout << "3***" << endl;
#endif
#ifdef SM_DUMP_CUM
  dumpmatrix<T>(b_cum,n_2);
#endif
  SMCheckA<T>(subBuf,n_2,l,m,n, A, 0, 0, as, b_cum, 0, 0, n_2, p3, xLim, yLim2);
#ifdef SM_DUMP
  dumpmatrix<T>(p3,n_2);
#endif
  delete[] subBuf;
}

  /* p6 = (a21 - a11) x (b11 + b12) */
#pragma omp task private( subBuf, a_cum, b_cum) default(shared)
{
  T* subBuf = new T[ 5 * n_22];
  T* a_cum = subBuf + 3 * n_22;
  T* b_cum = subBuf + 4 * n_22;
  SMSub2<T>(n_2,n,m, A, 0+n_2, 0, as, 0, 0, a_cum,xLim2,n_2);
  SMAdd<T>(n_2,m,l, B, 0, 0, bs, 0, 0+n_2, b_cum, n_2, yLim2);
#ifdef SM_DUMP
  cout << "6***" << endl;
#endif
#ifdef SM_DUMP_CUM
  dumpmatrix<T>(a_cum,n_2);
  dumpmatrix<T>(b_cum,n_2);
#endif
  SMNoCheckAB<T>(subBuf,n_2, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p6, xLim2, yLim2);
  //   SM<T>(subBuf,n_2,l,m,n, a_cum, 0, 0, n_2, b_cum, 0, 0, n_2, p6);
#ifdef SM_DUMP
  dumpmatrix<T>(p6,n_2);
#endif
  delete[] subBuf;
}

#pragma omp taskwait
} // task

  /* c11 = p1 + p4 - p5 + p7 */
  bool do12;// = true;
  bool do21;// = true;
  bool do22;// = true;
  long cxLim, cyLim;
  if( 0+n_2 >= l)
    {
      cyLim = l-0;
      if( 0+n_2 >= n)
	{
	  cxLim = n-0;
#pragma omp parallel for
	  for( long ix=0;ix<cxLim;++ix)
	    for( long iy=0;iy<cyLim;++iy)
	      {
		/* c11 = p1 + p4 - p5 + p7 */
                SizeT pIndex = ix*n_2+iy;

		assert(((ix)*cs + iy)<n*l);
		C[(ix)*cs + iy] = p1[pIndex]+p4[pIndex]-p5[pIndex]+p7[pIndex];
	      }
	  delete[] p1;
	  return;
	}
      cxLim = n_2;
      do12 = false;
      do21 = true;
      do22 = false;
    }
  else
    { // linear dimension (l) > n_2
      cyLim = n_2;
      if( 0+n_2 >= n)
	{ // n non linear dimension < n (C[l,n])
	  cxLim = n-0;
	  do12 = true;
	  do21 = false;
	  do22 = false;
	}
      else
	{
	  cxLim = n_2;
	  do12 = true;
	  do21 = true;
	  do22 = true;
	}
    }
#pragma omp parallel for
  for( long ix=0;ix<cxLim;++ix)
    for( long iy=0;iy<cyLim;++iy)
      {
	/* c11 = p1 + p4 - p5 + p7 */
	SizeT pIndex = ix*n_2+iy;

        assert(((ix)*cs + iy)<n*l);
	C[(ix)*cs + iy] = p1[pIndex]+p4[pIndex]-p5[pIndex]+p7[pIndex];
      }
 
  /* c12 = p3 + p5 */
  if( do12)
    {
      if( 0+mSz > l)
	{
	  cyLim = l-n_2;
	}
      else
	{
	  cyLim = n_2;
	}
      if( 0+n_2 > n)
	{
	  cxLim = n;//-(0+n_2);
	}
      else
	{
	  cxLim = n_2;
	}
#pragma omp parallel for
      for( long ix=0;ix<cxLim;++ix)
	for( long iy=0;iy<cyLim;++iy)
	  {
	    /* c12 = p3 + p5 */
	    SizeT pIndex = ix*n_2+iy;

	    assert(((ix)*cs + iy+n_2)<n*l);
	    C[(ix)*cs + iy+n_2] = p3[pIndex]+p5[pIndex];
	  }
    }
  if( do21)
    {
      /* c21 = p2 + p4 */
      if( 0+n_2 > l)
	{
	  cyLim = l;//l-(0+n_2);
	}
      else
	{
	  cyLim = n_2;
	}
      if( 0+mSz > n)
	{
	  cxLim = n-n_2;
	}
      else
	{
	  cxLim = n_2;
	}
#pragma omp parallel for
      for( long ix=0;ix<cxLim;++ix)
	for( long iy=0;iy<cyLim;++iy)
	  {

	    /* c21 = p2 + p4 */
	    SizeT pIndex = ix*n_2+iy;

	    assert(((ix+n_2)*cs + iy)<n*l);
	    C[(ix+n_2)*cs + iy] = p2[pIndex]+p4[pIndex];
	  }
    }
  if( do22)
    {
      /* c22 = p1 + p3 - p2 + p6 */
      if( 0+mSz > l)
	{
	  cyLim = l-(0+n_2);
	}
      else
	{
	  cyLim = n_2;
	}
      if( 0+mSz > n)
	{
	  cxLim = n-(0+n_2);
	}
      else
	{
	  cxLim = n_2;
	}
#pragma omp parallel for
      for( long ix=0;ix<cxLim;++ix)
	for( long iy=0;iy<cyLim;++iy)
	  {
	    /* c22 = p1 + p3 - p2 + p6 */
	    SizeT pIndex = ix*n_2+iy;

	    assert(((ix+n_2)*cs + iy+n_2)<n*l);
	    C[(ix+n_2)*cs + iy+n_2] = p1[pIndex]+p3[pIndex]-p2[pIndex]+p6[pIndex];
	  }
    }

  delete[] p1;
  return;
}
#endif
#endif