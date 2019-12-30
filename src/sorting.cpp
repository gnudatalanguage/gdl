/***************************************************************************
                          sorting.cpp  -  all things sortlike
                             -------------------
    begin                : Nove 2018
    copyright            : (C) 2018 by G. Duvert and Others
    email                : gilles dot duvert at free dot fr
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

#include "datatypes.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"

namespace lib {
#define INSERTION_SORT_THRESHOLD 9
#define QUICK_SORT_THRESHOLD 100
#define RADIX_SORT_THRESHOLD 1E5
#define MERGESORT_PARALLEL_THRESHOLD 1E6

// The following GDL version of SORT() is a complete rewriting using modern methods for a faster sort. 
// It provides a noticeable, sometimes even huge, speed gain, depending on array sizes and types. 
// This version uses 
// 1) modified code from https://github.com/Pierre-Terdiman/RadixRedux 
// (adapted to GDL interfaces and other types than floats and longs) plus
// 2) new versions of other sorting methods, as the various codes found on the web do not sort indexes
// but values and do not sort NaNs at all.
// I've added a GDL_SORT() "private" function that permits to use one of: /RADIX, /QUICK, /MERGE and /INSERT 
// sorting method. This function is used in the procedure testsuite/benchmark/compare_sort_algos
// to plot the performances of all the different sorts (insertion is not used as it is in N^2 and would take forever)
// and adjust the combination of all these methods that makes for the performance in the final sort().
// With a modicum of work, this could be used to fine-tune the SORT() method for a particular machine or purpose,
// maintaining top performances in specific cases (clusters etc).
//------------------Radix Sorting Codes ------------------------------------------------------------------------------
// Here is the copyright for the Radix code by PT. It is understood that Pierre's code was for Floats, Longs and ULongs only. 
//Copyright (c) 2018 Pierre Terdiman - http://www.codercorner.com/blog
//
//
//This software is provided 'as-is', without any express or implied warranty. 
//In no event will the authors be held liable for any damages arising from the use of this software.
//Permission is granted to anyone to use this software for any purpose, including commercial applications,
//and to alter it and redistribute it freely, subject to the following restrictions:
//1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.
//If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
//2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
//3. This notice may not be removed or altered from any source distribution.
//------------------Radix Sorting Codes ------------------------------------------------------------------------------
//    
#if defined(IS_BIGENDIAN)
	#define H0_OFFSET2	256
	#define H1_OFFSET2	0
        #define BYTES_INC2	(1-j)
    #define H0_OFFSET4	768
	#define H1_OFFSET4	512
	#define H2_OFFSET4	256
	#define H3_OFFSET4	0
	#define BYTES_INC4	(3-j)
	#define H7_OFFSET8	0
	#define H6_OFFSET8	256
	#define H5_OFFSET8	512
	#define H4_OFFSET8	768
	#define H3_OFFSET8	1024
	#define H2_OFFSET8	1280
	#define H1_OFFSET8	1536
	#define H0_OFFSET8	1792
	#define BYTES_INC8	(7-j)
#else
	#define H0_OFFSET2	0
	#define H1_OFFSET2	256
	#define BYTES_INC2	j
	#define H0_OFFSET4	0
	#define H1_OFFSET4	256
	#define H2_OFFSET4	512
	#define H3_OFFSET4	768
	#define BYTES_INC4	j
	#define H0_OFFSET8	0
	#define H1_OFFSET8	256
	#define H2_OFFSET8	512
	#define H3_OFFSET8	768
	#define H4_OFFSET8	1024
	#define H5_OFFSET8	1280
	#define H6_OFFSET8	1536
	#define H7_OFFSET8	1792
	#define BYTES_INC8	j
#endif

#include <string.h> //for memset (Windows)
  
#define CREATE_HISTOGRAMS2(type, buffer)													\
	/* Clear counters/histograms */															\
	memset(Histogram, 0, 256*2*sizeof(T));                                                 \
																							\
	/* Prepare to count */																	\
	const DByte* p = (const DByte*)input;													\
	const DByte* pe = &p[nb*2];																\
	T* h0 = &Histogram[H0_OFFSET2];	/* Histogram for first pass (LSB)	*/                  \
	T* h1 = &Histogram[H1_OFFSET2];	/* Histogram for last pass (MSB)	*/                  \
																							\
	bool AlreadySorted = true;	/* Optimism... */											\
																							\
	if(ranksUnInitialized)																    \
	{																						\
		/* Prepare for temporal coherence */												\
		const type* Running = (type*)buffer;												\
		type PrevVal = *Running;															\
																							\
		while(p!=pe)																		\
		{																					\
			/* Read input buffer in previous sorted order */								\
			const type Val = *Running++;													\
			/* Check whether already sorted or not */										\
			if(Val<PrevVal)	{ AlreadySorted = false; break; } /* Early out */				\
			/* Update for next iteration */													\
			PrevVal = Val;																	\
																							\
			/* Create histograms */															\
			h0[*p++]++;	h1[*p++]++;                         								\
		}																					\
																							\
		/* If all input values are already sorted, we just have to return and leave the */	\
		/* previous list unchanged. That way the routine may take advantage of temporal */	\
		/* coherence, for example when used to sort transparent faces.					*/	\
		if(AlreadySorted)																	\
		{																					\
			for(SizeT i=0;i<nb;++i)	mRanks[i] = i;										    \
			return mRanks;																	\
		}																					\
	}																						\
	else																					\
	{																						\
		/* Prepare for temporal coherence */												\
		const T* Indices = mRanks;                                                          \
		type PrevVal = (type)buffer[*Indices];												\
																							\
		while(p!=pe)																		\
		{																					\
			/* Read input buffer in previous sorted order */								\
			const type Val = (type)buffer[*Indices++];										\
			/* Check whether already sorted or not */										\
			if(Val<PrevVal)	{ AlreadySorted = false; break; } /* Early out */				\
			/* Update for next iteration */													\
			PrevVal = Val;																	\
																							\
			/* Create histograms */															\
			h0[*p++]++;	h1[*p++]++;                        									\
		}																					\
																							\
		/* If all input values are already sorted, we just have to return and leave the */	\
		/* previous list unchanged. That way the routine may take advantage of temporal */	\
		/* coherence, for example when used to sort transparent faces.					*/	\
		if(AlreadySorted) return mRanks;                       							    \
	}																						\
																							\
	/* Else there has been an early out and we must finish computing the histograms */		\
	while(p!=pe)																			\
	{																						\
		/* Create histograms without the previous overhead */								\
		h0[*p++]++;	h1[*p++]++;	                       										\
    }

#define CREATE_HISTOGRAMS4(type, buffer)													\
	/* Clear counters/histograms */															\
	memset(Histogram, 0, 256*4*sizeof(T));                                             	\
																							\
	/* Prepare to count */																	\
	const DByte* p = (const DByte*)input;													\
	const DByte* pe = &p[nb*4];																\
	T* h0 = &Histogram[H0_OFFSET4];	/* Histogram for first pass (LSB)	*/                  \
	T* h1 = &Histogram[H1_OFFSET4];	/* Histogram for second pass		*/              	\
	T* h2 = &Histogram[H2_OFFSET4];	/* Histogram for third pass			*/              	\
	T* h3 = &Histogram[H3_OFFSET4];	/* Histogram for last pass (MSB)	*/                	\
																							\
	bool AlreadySorted = true;	/* Optimism... */											\
																							\
	if(ranksUnInitialized)																    \
	{																						\
		/* Prepare for temporal coherence */												\
		const type* Running = (type*)buffer;												\
		type PrevVal = *Running;															\
																							\
		while(p!=pe)																		\
		{																					\
			/* Read input buffer in previous sorted order */								\
			const type Val = *Running++;													\
			/* Check whether already sorted or not */										\
			if(Val<PrevVal)	{ AlreadySorted = false; break; } /* Early out */				\
			/* Update for next iteration */													\
			PrevVal = Val;																	\
																							\
			/* Create histograms */															\
			h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;									\
		}																					\
																							\
		/* If all input values are already sorted, we just have to return and leave the */	\
		/* previous list unchanged. That way the routine may take advantage of temporal */	\
		/* coherence, for example when used to sort transparent faces.					*/	\
		if(AlreadySorted)																	\
		{																					\
			for(SizeT i=0;i<nb;++i)	mRanks[i] = i;										    \
			return mRanks;															        \
		}																					\
	}																						\
	else																					\
	{																						\
		/* Prepare for temporal coherence */												\
		const T* Indices = mRanks;                                                        	\
		type PrevVal = (type)buffer[*Indices];												\
																							\
		while(p!=pe)																		\
		{																					\
			/* Read input buffer in previous sorted order */								\
			const type Val = (type)buffer[*Indices++];										\
			/* Check whether already sorted or not */										\
			if(Val<PrevVal)	{ AlreadySorted = false; break; } /* Early out */				\
			/* Update for next iteration */													\
			PrevVal = Val;																	\
																							\
			/* Create histograms */															\
			h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;									\
		}																					\
																							\
		/* If all input values are already sorted, we just have to return and leave the */	\
		/* previous list unchanged. That way the routine may take advantage of temporal */	\
		/* coherence, for example when used to sort transparent faces.					*/	\
		if(AlreadySorted) return mRanks;                   									\
	}																						\
																							\
	/* Else there has been an early out and we must finish computing the histograms */		\
	while(p!=pe)																			\
	{																						\
		/* Create histograms without the previous overhead */								\
		h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;										\
    }

#define CREATE_HISTOGRAMS8(type, buffer)													\
	/* Clear counters/histograms */															\
	memset(Histogram, 0, 256*8*sizeof(T));                                                  \
																							\
	/* Prepare to count */																	\
	const DByte* p = (const DByte*)input;													\
	const DByte* pe = &p[nb*8];																\
	T* h0 = &Histogram[H0_OFFSET8];	/* Histogram for first pass (LSB)	*/                  \
	T* h1 = &Histogram[H1_OFFSET8];	/*                                  */                  \
	T* h2 = &Histogram[H2_OFFSET8];	/*                                  */                  \
          T* h3 = &Histogram[H3_OFFSET8];	/*                                  */			\
          T* h4 = &Histogram[H4_OFFSET8];	/*                                  */			\
          T* h5 = &Histogram[H5_OFFSET8];	/*                                  */			\
          T* h6 = &Histogram[H6_OFFSET8];	/*                                  */			\
          T* h7 = &Histogram[H7_OFFSET8];	/* Histogram for last pass (MSB)	*/			\
																							\
	bool AlreadySorted = true;	/* Optimism... */											\
																							\
	if(ranksUnInitialized)																    \
	{																						\
		/* Prepare for temporal coherence */												\
		const type* Running = (type*)buffer;												\
		type PrevVal = *Running;															\
																							\
		while(p!=pe)																		\
		{																					\
			/* Read input buffer in previous sorted order */								\
			const type Val = *Running++;													\
			/* Check whether already sorted or not */										\
			if(Val<PrevVal)	{ AlreadySorted = false; break; } /* Early out */				\
			/* Update for next iteration */													\
			PrevVal = Val;																	\
																							\
			/* Create histograms */															\
			h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;									\
			h4[*p++]++;	h5[*p++]++;	h6[*p++]++;	h7[*p++]++;									\
		}																					\
																							\
		/* If all input values are already sorted, we just have to return and leave the */	\
		/* previous list unchanged. That way the routine may take advantage of temporal */	\
		/* coherence, for example when used to sort transparent faces.					*/	\
		if(AlreadySorted)																	\
		{																					\
			for(SizeT i=0;i<nb;++i)	mRanks[i] = i;										    \
			 return mRanks;															        \
		}																					\
	}																						\
	else																					\
	{																						\
		/* Prepare for temporal coherence */												\
		const T* Indices = mRanks;                                                          \
		type PrevVal = (type)buffer[*Indices];												\
																							\
		while(p!=pe)																		\
		{																					\
			/* Read input buffer in previous sorted order */								\
			const type Val = (type)buffer[*Indices++];										\
			/* Check whether already sorted or not */										\
			if(Val<PrevVal)	{ AlreadySorted = false; break; } /* Early out */				\
			/* Update for next iteration */													\
			PrevVal = Val;																	\
																							\
			/* Create histograms */															\
			h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;									\
			h4[*p++]++;	h5[*p++]++;	h6[*p++]++;	h7[*p++]++;									\
		}																					\
																							\
		/* If all input values are already sorted, we just have to return and leave the */	\
		/* previous list unchanged. That way the routine may take advantage of temporal */	\
		/* coherence, for example when used to sort transparent faces.					*/	\
		if(AlreadySorted)  return mRanks;                  									\
	}																						\
																							\
	/* Else there has been an early out and we must finish computing the histograms */		\
	while(p!=pe)																			\
	{																						\
		/* Create histograms without the previous overhead */								\
			h0[*p++]++;	h1[*p++]++;	h2[*p++]++;	h3[*p++]++;									\
			h4[*p++]++;	h5[*p++]++;	h6[*p++]++;	h7[*p++]++;									\
    }

#define CHECK_PASS_VALIDITY(pass)															\
	/* Shortcut to current counters */														\
	const T* CurCount = &Histogram[pass<<8];                                                \
																							\
	/* Reset flag. The sorting pass is supposed to be performed. (default) */				\
	bool PerformPass = true;																\
																							\
	/* Check pass validity */																\
																							\
	/* If all values have the same byte, sorting is useless. */								\
	/* It may happen when sorting bytes or words instead of dwords. */						\
	/* This routine actually sorts words faster than dwords, and bytes */					\
	/* faster than words. Standard running time (O(4*n))is reduced to O(2*n) */				\
	/* for words and O(n) for bytes. Running time for floats depends on actual values... */	\
																							\
	/* Get first byte */																	\
	const DByte UniqueVal = *(((DByte*)input)+pass);										\
																							\
	/* Check that byte's counter */               \
    if(CurCount[UniqueVal]==nb) PerformPass=false;

  template<typename T>
 static T* RadixSort(const double* inputDouble, SizeT nb)
{
	const DULong64* input = (const DULong64*)inputDouble;
    T* mRanks=(T*)gdlAlignedMalloc(nb*sizeof(T));
    T* mRanks2=(T*)gdlAlignedMalloc(nb*sizeof(T));
	// Allocate histograms & offsets on the stack
	T Histogram[256*8];
	T* Link[256];
    bool ranksUnInitialized=true;
    
	{ CREATE_HISTOGRAMS8(double, inputDouble); }

	// Radix sort, j is the pass number (0=LSB, 7=MSB)
	for(int j=0;j<8;++j)
	{
		// Should we care about negative values?
		if(j!=7)
		{
			// Here we deal with positive values only
			CHECK_PASS_VALIDITY(j);

			if(PerformPass)
			{
				// Create offsets
				Link[0] = mRanks2;
				for(SizeT i=1;i<256;++i)		Link[i] = Link[i-1] + CurCount[i-1];

				// Perform Radix Sort
				const DByte* InputBytes = (const DByte*)input;
				InputBytes += BYTES_INC8;
				if(ranksUnInitialized)
				{
					for(SizeT i=0;i<nb;++i)	*Link[InputBytes[i<<3]]++ = i;
					ranksUnInitialized=false;
				}
				else
				{
					const T* Indices		= mRanks;
					const T* IndicesEnd	= &mRanks[nb];
					while(Indices!=IndicesEnd)
					{
						const T id = *Indices++;
						*Link[InputBytes[id<<3]]++ = id;
					}
				}

				// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
				T* Tmp = mRanks;
				mRanks = mRanks2;
				mRanks2 = Tmp;
			}
		}
		else
		{
			// This is a special case to correctly handle negative values
			CHECK_PASS_VALIDITY(j);

			if(PerformPass)
			{
				// Compute #negative values involved if needed
				T NbNegativeValues = 0;
				// An efficient way to compute the number of negatives values we'll have to deal with is simply to sum the 128
				// last values of the last histogram. Last histogram because that's the one for the Most Significant Byte,
				// responsible for the sign. 128 last values because the 128 first ones are related to positive numbers.
				// ### is that ok on Apple ?!
				const T* h7 = &Histogram[H7_OFFSET8];
				for(int i=128;i<256;++i)	NbNegativeValues += h7[i];	// 768 for last histogram, 128 for negative part

				// Create biased offsets, in order for negative numbers to be sorted as well
				Link[0] = &mRanks2[NbNegativeValues];										// First positive number takes place after the negative ones
				for(int i=1;i<128;++i)		Link[i] = Link[i-1] + CurCount[i-1];		// 1 to 128 for positive numbers

				// We must reverse the sorting order for negative numbers!
				Link[255] = mRanks2;
				for(int i=0;i<127;++i)	Link[254-i] = Link[255-i] + CurCount[255-i];	// Fixing the wrong order for negative values
				for(int i=128;i<256;++i)	Link[i] += CurCount[i];							// Fixing the wrong place for negative values

				// Perform Radix Sort
				if(ranksUnInitialized)
				{
					for(SizeT i=0;i<nb;++i)
					{
						const T Radix = input[i]>>56;			// Radix byte, same as above. AND is useless here (udword).
						// ### cmp to be killed. Not good. Later.
						if(Radix<128)		*Link[Radix]++ = i;		// Number is positive, same as above
						else				*(--Link[Radix]) = i;	// Number is negative, flip the sorting order
					}
					ranksUnInitialized=false;
				}
				else
				{
					for(SizeT i=0;i<nb;++i)
					{
						const T Radix = input[mRanks[i]]>>56;			// Radix byte, same as above. AND is useless here (udword).
						// ### cmp to be killed. Not good. Later.
						if(Radix<128)		*Link[Radix]++ = mRanks[i];		// Number is positive, same as above
						else				*(--Link[Radix]) = mRanks[i];	// Number is negative, flip the sorting order
					}
				}
				// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
				T* Tmp = mRanks;
				mRanks = mRanks2;
				mRanks2 = Tmp;
			}
			else
			{
				// The pass is useless, yet we still have to reverse the order of current list if all values are negative.
				if(UniqueVal>=128)
				{
					if(ranksUnInitialized)
					{
						// ###Possible?
						for(SizeT i=0;i<nb;++i)	mRanks2[i] = nb-i-1;
						ranksUnInitialized=false;
					}
					else
					{
						for(SizeT i=0;i<nb;++i)	mRanks2[i] = mRanks[nb-i-1];
					}

					// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
					T* Tmp = mRanks;
					mRanks = mRanks2;
					mRanks2 = Tmp;
				}
			}
		}
	}
    gdlAlignedFree(mRanks2);
    return mRanks;
}

template<typename T>
 static T* RadixSort(const float* inputFloat, SizeT nb)
{
	const DULong* input = (const DULong*)inputFloat;
    T* mRanks=(T*)gdlAlignedMalloc(nb*sizeof(T));
    T* mRanks2=(T*)gdlAlignedMalloc(nb*sizeof(T));
	// Allocate histograms & offsets on the stack
	T Histogram[256*4];
	T* Link[256];
    bool ranksUnInitialized=true;
    
	// Create histograms (counters). Counters for all passes are created in one run.
	// Pros:	read input buffer once instead of four times
	// Cons:	mHistogram is 4Kb instead of 1Kb
	// Floating-point values are always supposed to be signed values, so there's only one code path there.
	// Please note the floating point comparison needed for temporal coherence! Although the resulting asm code
	// is dreadful, this is surprisingly not such a performance hit - well, I suppose that's a big one on first
	// generation Pentiums....We can't make comparison on integer representations because, as Chris said, it just
	// wouldn't work with mixed positive/negative values....
	{ CREATE_HISTOGRAMS4(float, inputFloat); }

	// Radix sort, j is the pass number (0=LSB, 3=MSB)
	for(int j=0;j<4;++j)
	{
		// Should we care about negative values?
		if(j!=3)
		{
			// Here we deal with positive values only
			CHECK_PASS_VALIDITY(j);

			if(PerformPass)
			{
				// Create offsets
				Link[0] = mRanks2;
				for(SizeT i=1;i<256;++i)		Link[i] = Link[i-1] + CurCount[i-1];

				// Perform Radix Sort
				const DByte* InputBytes = (const DByte*)input;
				InputBytes += BYTES_INC4;
				if(ranksUnInitialized)
				{
					for(SizeT i=0;i<nb;++i)	*Link[InputBytes[i<<2]]++ = i;
					ranksUnInitialized=false;
				}
				else
				{
					const T* Indices		= mRanks;
					const T* IndicesEnd	= &mRanks[nb];
					while(Indices!=IndicesEnd)
					{
						const T id = *Indices++;
						*Link[InputBytes[id<<2]]++ = id;
					}
				}

				// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
				T* Tmp = mRanks;
				mRanks = mRanks2;
				mRanks2 = Tmp;
			}
		}
		else
		{
			// This is a special case to correctly handle negative values
			CHECK_PASS_VALIDITY(j);

			if(PerformPass)
			{
				// Compute #negative values involved if needed
				T NbNegativeValues = 0;
				// An efficient way to compute the number of negatives values we'll have to deal with is simply to sum the 128
				// last values of the last histogram. Last histogram because that's the one for the Most Significant Byte,
				// responsible for the sign. 128 last values because the 128 first ones are related to positive numbers.
				// ### is that ok on Apple ?!
				const T* h3 = &Histogram[H3_OFFSET4];
				for(int i=128;i<256;++i)	NbNegativeValues += h3[i];	// 768 for last histogram, 128 for negative part

				// Create biased offsets, in order for negative numbers to be sorted as well
				Link[0] = &mRanks2[NbNegativeValues];										// First positive number takes place after the negative ones
				for(int i=1;i<128;++i)		Link[i] = Link[i-1] + CurCount[i-1];		// 1 to 128 for positive numbers

				// We must reverse the sorting order for negative numbers!
				Link[255] = mRanks2;
				for(int i=0;i<127;++i)	Link[254-i] = Link[255-i] + CurCount[255-i];	// Fixing the wrong order for negative values
				for(int i=128;i<256;++i)	Link[i] += CurCount[i];							// Fixing the wrong place for negative values

				// Perform Radix Sort
				if(ranksUnInitialized)
				{
					for(SizeT i=0;i<nb;++i)
					{
						const DULong Radix = input[i]>>24;			// Radix byte, same as above. AND is useless here (udword).
						// ### cmp to be killed. Not good. Later.
						if(Radix<128)		*Link[Radix]++ = i;		// Number is positive, same as above
						else				*(--Link[Radix]) = i;	// Number is negative, flip the sorting order
					}
					ranksUnInitialized=false;
				}
				else
				{
					for(SizeT i=0;i<nb;++i)
					{
						const DULong Radix = input[mRanks[i]]>>24;			// Radix byte, same as above. AND is useless here (udword).
						// ### cmp to be killed. Not good. Later.
						if(Radix<128)		*Link[Radix]++ = mRanks[i];		// Number is positive, same as above
						else				*(--Link[Radix]) = mRanks[i];	// Number is negative, flip the sorting order
					}
				}
				// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
				T* Tmp = mRanks;
				mRanks = mRanks2;
				mRanks2 = Tmp;
			}
			else
			{
				// The pass is useless, yet we still have to reverse the order of current list if all values are negative.
				if(UniqueVal>=128)
				{
					if(ranksUnInitialized)
					{
						// ###Possible?
						for(SizeT i=0;i<nb;++i)	mRanks2[i] = nb-i-1;
						ranksUnInitialized=false;
					}
					else
					{
						for(SizeT i=0;i<nb;++i)	mRanks2[i] = mRanks[nb-i-1];
					}

					// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
					T* Tmp = mRanks;
					mRanks = mRanks2;
					mRanks2 = Tmp;
				}
			}
		}
	}
    gdlAlignedFree(mRanks2);
    return mRanks;
}

template<typename T>
 static T* RadixSort(const DLong64* input, SizeT nb)
{
    T* mRanks=(T*)gdlAlignedMalloc(nb*sizeof(T));
    T* mRanks2=(T*)gdlAlignedMalloc(nb*sizeof(T));
	// Allocate histograms & offsets on the stack
	T Histogram[256*8];
	T* Link[256];
    bool ranksUnInitialized=true;
    
    CREATE_HISTOGRAMS8(DLong64, input);

	// Radix sort, j is the pass number (0=LSB, 7=MSB)
	for(int j=0;j<8;++j)
	{
		CHECK_PASS_VALIDITY(j);

		// Sometimes the last (negative) pass is skipped because all numbers are negative and the MSB is 0xFF (for example). This is
		// not a problem, numbers are correctly sorted anyway.
		if(PerformPass)
		{
			// Should we care about negative values?
			if(j!=7)
			{
				// Here we deal with positive values only

				// Create offsets
				Link[0] = mRanks2;
				for(int i=1;i<256;++i)		Link[i] = Link[i-1] + CurCount[i-1];
			}
			else
			{
				// This is a special case to correctly handle negative integers. They're sorted in the right order but at the wrong place.

				// Compute #negative values involved if needed
				T NbNegativeValues = 0;
                // An efficient way to compute the number of negatives values we'll have to deal with is simply to sum the 128
                // last values of the last histogram. Last histogram because that's the one for the Most Significant Byte,
                // responsible for the sign. 128 last values because the 128 first ones are related to positive numbers.
                const T* h7 = &Histogram[H7_OFFSET8];
                for(int i=128;i<256;++i)	NbNegativeValues += h7[i];	// 768 for last histogram, 128 for negative part

				// Create biased offsets, in order for negative numbers to be sorted as well
				Link[0] = &mRanks2[NbNegativeValues];										// First positive number takes place after the negative ones
				for(int i=1;i<128;++i)		Link[i] = Link[i-1] + CurCount[i-1];		// 1 to 128 for positive numbers

				// Fixing the wrong place for negative values
				Link[128] = mRanks2;
				for(int i=129;i<256;++i)		Link[i] = Link[i-1] + CurCount[i-1];
			}

			// Perform Radix Sort
			const DByte* InputBytes	= (const DByte*)input;
			InputBytes += BYTES_INC8;
			if(ranksUnInitialized)
			{
				for(SizeT i=0;i<nb;++i)	*Link[InputBytes[i<<3]]++ = i;
				ranksUnInitialized=false;
			}
			else
			{
				const T* Indices		= mRanks;
				const T* IndicesEnd	= &mRanks[nb];
				while(Indices!=IndicesEnd)
				{
					const T id = *Indices++;
					*Link[InputBytes[id<<3]]++ = id;
				}
			}

			// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
			T* Tmp = mRanks;
			mRanks = mRanks2;
			mRanks2 = Tmp;
		}
	}
    gdlAlignedFree(mRanks2);
    return mRanks;
}

template<typename T>
 static T* RadixSort(const DULong64* input, SizeT nb)
{
    T* mRanks=(T*)gdlAlignedMalloc(nb*sizeof(T));
    T* mRanks2=(T*)gdlAlignedMalloc(nb*sizeof(T));
	// Allocate histograms & offsets on the stack
	T Histogram[256*8];
	T* Link[256];
    bool ranksUnInitialized=true;
    
    CREATE_HISTOGRAMS8(DULong64, input);

	// Radix sort, j is the pass number (0=LSB, 7=MSB)
	for(int j=0;j<8;++j)
	{
		CHECK_PASS_VALIDITY(j);

		// Sometimes the last (negative) pass is skipped because all numbers are negative and the MSB is 0xFF (for example). This is
		// not a problem, numbers are correctly sorted anyway.
		if(PerformPass)
		{
            // Here we deal with positive values only

            // Create offsets
            Link[0] = mRanks2;
            for(int i=1;i<256;++i)		Link[i] = Link[i-1] + CurCount[i-1];

			// Perform Radix Sort
			const DByte* InputBytes	= (const DByte*)input;
			InputBytes += BYTES_INC8;
			if(ranksUnInitialized)
			{
				for(SizeT i=0;i<nb;++i)	*Link[InputBytes[i<<3]]++ = i;
				ranksUnInitialized=false;
			}
			else
			{
				const T* Indices		= mRanks;
				const T* IndicesEnd	= &mRanks[nb];
				while(Indices!=IndicesEnd)
				{
					const T id = *Indices++;
					*Link[InputBytes[id<<3]]++ = id;
				}
			}

			// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
			T* Tmp = mRanks;
			mRanks = mRanks2;
			mRanks2 = Tmp;
		}
	}
    gdlAlignedFree(mRanks2);
    return mRanks;
}

template<typename T>
 static T* RadixSort(const DLong* input, SizeT nb)
{
    T* mRanks=(T*)gdlAlignedMalloc(nb*sizeof(T));
    T* mRanks2=(T*)gdlAlignedMalloc(nb*sizeof(T));
	// Allocate histograms & offsets on the stack
	T Histogram[256*4];
	T* Link[256];
    bool ranksUnInitialized=true;
    
	// Create histograms (counters). Counters for all passes are created in one run.
	// Pros:	read input buffer once instead of four times
	// Cons:	mHistogram is 4Kb instead of 1Kb
	// We must take care of signed/unsigned values for temporal coherence.... I just
	// have 2 code paths even if just a single opcode changes. Self-modifying code, someone?

    CREATE_HISTOGRAMS4(DLong, input);

	// Radix sort, j is the pass number (0=LSB, 3=MSB)
	for(int j=0;j<4;++j)
	{
		CHECK_PASS_VALIDITY(j);

		// Sometimes the fourth (negative) pass is skipped because all numbers are negative and the MSB is 0xFF (for example). This is
		// not a problem, numbers are correctly sorted anyway.
		if(PerformPass)
		{
			// Should we care about negative values?
			if(j!=3)
			{
				// Here we deal with positive values only

				// Create offsets
				Link[0] = mRanks2;
				for(int i=1;i<256;++i)		Link[i] = Link[i-1] + CurCount[i-1];
			}
			else
			{
				// This is a special case to correctly handle negative integers. They're sorted in the right order but at the wrong place.

				// Compute #negative values involved if needed
				T NbNegativeValues = 0;
                // An efficient way to compute the number of negatives values we'll have to deal with is simply to sum the 128
                // last values of the last histogram. Last histogram because that's the one for the Most Significant Byte,
                // responsible for the sign. 128 last values because the 128 first ones are related to positive numbers.
                const T* h3 = &Histogram[H3_OFFSET4];
                for(int i=128;i<256;++i)	NbNegativeValues += h3[i];	// 768 for last histogram, 128 for negative part

				// Create biased offsets, in order for negative numbers to be sorted as well
				Link[0] = &mRanks2[NbNegativeValues];										// First positive number takes place after the negative ones
				for(int i=1;i<128;++i)		Link[i] = Link[i-1] + CurCount[i-1];		// 1 to 128 for positive numbers

				// Fixing the wrong place for negative values
				Link[128] = mRanks2;
				for(int i=129;i<256;++i)		Link[i] = Link[i-1] + CurCount[i-1];
			}

			// Perform Radix Sort
			const DByte* InputBytes	= (const DByte*)input;
			InputBytes += BYTES_INC4;
			if(ranksUnInitialized)
			{
				for(SizeT i=0;i<nb;++i)	*Link[InputBytes[i<<2]]++ = i;
				ranksUnInitialized=false;
			}
			else
			{
				const T* Indices		= mRanks;
				const T* IndicesEnd	= &mRanks[nb];
				while(Indices!=IndicesEnd)
				{
					const T id = *Indices++;
					*Link[InputBytes[id<<2]]++ = id;
				}
			}

			// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
			T* Tmp = mRanks;
			mRanks = mRanks2;
			mRanks2 = Tmp;
		}
	}
    gdlAlignedFree(mRanks2);
    return mRanks;
}

template<typename T>
 static T* RadixSort(const DULong* input, SizeT nb)
{
    T* mRanks=(T*)gdlAlignedMalloc(nb*sizeof(T));
    T* mRanks2=(T*)gdlAlignedMalloc(nb*sizeof(T));
	// Allocate histograms & offsets on the stack
	T Histogram[256*4];
	T* Link[256];
    bool ranksUnInitialized=true;
    
	// Create histograms (counters). Counters for all passes are created in one run.
	// Pros:	read input buffer once instead of four times
	// Cons:	mHistogram is 4Kb instead of 1Kb
	// We must take care of signed/unsigned values for temporal coherence.... I just
	// have 2 code paths even if just a single opcode changes. Self-modifying code, someone?
    
    CREATE_HISTOGRAMS4(DULong, input);

	// Radix sort, j is the pass number (0=LSB, 3=MSB)
	for(int j=0;j<4;++j)
	{
		CHECK_PASS_VALIDITY(j);

		// Sometimes the fourth (negative) pass is skipped because all numbers are negative and the MSB is 0xFF (for example). This is
		// not a problem, numbers are correctly sorted anyway.
		if(PerformPass)
		{
            // Here we deal with positive values only

            // Create offsets
            Link[0] = mRanks2;
            for(int i=1;i<256;++i)		Link[i] = Link[i-1] + CurCount[i-1];

			// Perform Radix Sort
			const DByte* InputBytes	= (const DByte*)input;
			InputBytes += BYTES_INC4;
			if(ranksUnInitialized)
			{
				for(SizeT i=0;i<nb;++i)	*Link[InputBytes[i<<2]]++ = i;
				ranksUnInitialized=false;
			}
			else
			{
				const T* Indices		= mRanks;
				const T* IndicesEnd	= &mRanks[nb];
				while(Indices!=IndicesEnd)
				{
					const T id = *Indices++;
					*Link[InputBytes[id<<2]]++ = id;
				}
			}

			// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
			T* Tmp = mRanks;
			mRanks = mRanks2;
			mRanks2 = Tmp;
		}
	}
    gdlAlignedFree(mRanks2);
    return mRanks;
}

template<typename T>
 static T* RadixSort(const DInt* input, SizeT nb)
{
    T* mRanks=(T*)gdlAlignedMalloc(nb*sizeof(T));
    T* mRanks2=(T*)gdlAlignedMalloc(nb*sizeof(T));
	// Allocate histograms & offsets on the stack
	T Histogram[256*2];
	T* Link[256];
    bool ranksUnInitialized=true;
    
    CREATE_HISTOGRAMS2(DInt, input);

	// Radix sort, j is the pass number (0=LSB, 1=MSB)
	for(int j=0;j<2;++j)
	{
		CHECK_PASS_VALIDITY(j);

		if(PerformPass)
		{
			// Should we care about negative values?
			if(j!=1)
			{
				// Here we deal with positive values only
				// Create offsets
				Link[0] = mRanks2;
				for(int i=1;i<256;++i)		Link[i] = Link[i-1] + CurCount[i-1];
			}
			else
			{
				// This is a special case to correctly handle negative integers. They're sorted in the right order but at the wrong place.
				// Compute #negative values involved if needed
				T NbNegativeValues = 0;
                // An efficient way to compute the number of negatives values we'll have to deal with is simply to sum the 128
                // last values of the last histogram. Last histogram because that's the one for the Most Significant Byte,
                // responsible for the sign. 128 last values because the 128 first ones are related to positive numbers.
                const T* h1 = &Histogram[H1_OFFSET2];
                for(int i=128;i<256;++i)	NbNegativeValues += h1[i];	// 768 for last histogram, 128 for negative part

				// Create biased offsets, in order for negative numbers to be sorted as well
				Link[0] = &mRanks2[NbNegativeValues];										// First positive number takes place after the negative ones
				for(int i=1;i<128;++i)		Link[i] = Link[i-1] + CurCount[i-1];		// 1 to 128 for positive numbers

				// Fixing the wrong place for negative values
				Link[128] = mRanks2;
				for(int i=129;i<256;++i)		Link[i] = Link[i-1] + CurCount[i-1];
			}

			// Perform Radix Sort
			const DByte* InputBytes	= (const DByte*)input;
			InputBytes += BYTES_INC2;
			if(ranksUnInitialized)
			{
				for(SizeT i=0;i<nb;++i)	*Link[InputBytes[i<<1]]++ = i;
				ranksUnInitialized=false;
			}
			else
			{
				const T* Indices		= mRanks;
				const T* IndicesEnd	= &mRanks[nb];
				while(Indices!=IndicesEnd)
				{
					const T id = *Indices++;
					*Link[InputBytes[id<<1]]++ = id;
				}
			}

			// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
			T* Tmp = mRanks;
			mRanks = mRanks2;
			mRanks2 = Tmp;
		}
	}
    gdlAlignedFree(mRanks2);
    return mRanks;
}

template<typename T>
 static T* RadixSort(const DUInt* input, SizeT nb)
{
    T* mRanks=(T*)gdlAlignedMalloc(nb*sizeof(T));
    T* mRanks2=(T*)gdlAlignedMalloc(nb*sizeof(T));
	// Allocate histograms & offsets on the stack
	T Histogram[256*2];
	T* Link[256];
    bool ranksUnInitialized=true;
    
    CREATE_HISTOGRAMS2(DUInt, input);

	// Radix sort, j is the pass number (0=LSB, 1=MSB)
	for(int j=0;j<2;++j)
	{
		CHECK_PASS_VALIDITY(j);

		if(PerformPass)
		{
            // Here we deal with positive values only

            // Create offsets
            Link[0] = mRanks2;
            for(int i=1;i<256;++i)		Link[i] = Link[i-1] + CurCount[i-1];

			// Perform Radix Sort
			const DByte* InputBytes	= (const DByte*)input;
			InputBytes += BYTES_INC2;
			if(ranksUnInitialized)
			{
				for(SizeT i=0;i<nb;++i)	*Link[InputBytes[i<<1]]++ = i;
				ranksUnInitialized=false;
			}
			else
			{
				const T* Indices		= mRanks;
				const T* IndicesEnd	= &mRanks[nb];
				while(Indices!=IndicesEnd)
				{
					const T id = *Indices++;
					*Link[InputBytes[id<<1]]++ = id;
				}
			}

			// Swap pointers for next pass. Valid indices - the most recent ones - are in mRanks after the swap.
			T* Tmp = mRanks;
			mRanks = mRanks2;
			mRanks2 = Tmp;
		}
	}
    gdlAlignedFree(mRanks2);
    return mRanks;
}

template<typename T>
 static T* RadixSort(const DByte* input, SizeT nb)
{
	// Allocate histograms & offsets on the stack
    T* mRanks=(T*)gdlAlignedMalloc(nb*sizeof(T));
	T Histogram[256];
	T* Link[256];
    
    /* Clear counters/histograms */
    memset(Histogram, 0, 256 * sizeof (T));

    /* Prepare to count */
    const DByte* p = (const DByte*) input;
    const DByte* pe = &p[nb];
    T* h0 = Histogram;
    
    bool AlreadySorted = true; /* Optimism... */
    for (SizeT i = 0; i < nb; ++i) mRanks[i] = i;

    /* Prepare for temporal coherence */
    const DByte* Running = (DByte*) input;
    DByte PrevVal = *Running;

    while (p != pe) {
      /* Read input buffer in previous sorted order */
      const DByte Val = *Running++;
      /* Check whether already sorted or not */
      if (Val < PrevVal) {
        AlreadySorted = false;
        break;
      } /* Early out */
      /* Update for next iteration */
      PrevVal = Val;

      /* Create histograms */
      h0[*p++]++;
    }

    /* If all input values are already sorted, we just have to return and leave the */
    /* previous list unchanged. That way the routine may take advantage of temporal */
    /* coherence, for example when used to sort transparent faces.					*/
    if (AlreadySorted) return mRanks;

    /* Else there has been an early out and we must finish computing the histograms */
    while (p != pe) {
      /* Create histograms without the previous overhead */
      h0[*p++]++;
    }

    /* If all values have the same byte, sorting is useless. */
    /* It may happen when sorting bytes or words instead of dwords. */
    /* This routine actually sorts words faster than dwords, and bytes */
    /* faster than words. Standard running time (O(4*n))is reduced to O(2*n) */
    /* for words and O(n) for bytes. Running time for floats depends on actual values... */

    /* Get first byte */
    const DByte UniqueVal = *((DByte*) input);

    /* Check that byte's counter */
    if (Histogram[UniqueVal] == nb) return mRanks; //already sorted too

    // Here we deal with positive values only
    // Create offsets
    Link[0] = mRanks;
    for(int i=1;i<256;++i) Link[i] = Link[i-1] + Histogram[i-1];

    // Perform Radix Sort
    const DByte* InputBytes	= (const DByte*)input;
    for(SizeT i=0;i<nb;++i)	*Link[InputBytes[i]]++ = i;
    return mRanks;
}


  template<typename T, typename Q>
   static void RadixSortIndex(const Q* val, T* index, SizeT lo, SizeT hi)
  {
    SizeT length = hi - lo + 1;
    T* res = RadixSort<T>(&(val[lo]), length); //sorted indexes are between 0 and length-1
    for (SizeT i=0; i< length; ++i) index[lo+i]=res[i]+lo;  //we need them between lo and hi...
    gdlAlignedFree(res);
  }

//-------------------------------------


template<typename T>
inline bool less (T &v, T &w)
{
    return (v < w);
}

template<>
inline bool less (DFloat &v, DFloat &w)
{
    return (v < w || std::isnan(w) );
}

template<>
inline bool less (DDouble &v, DDouble &w)
{
    return (v < w || std::isnan(w) );
}

template<typename T>
inline bool leq (T &v, T &w)
{
    return (v <= w);
}

template<>
inline bool leq (DFloat &v, DFloat &w)
{
    return (v <= w || std::isnan(w) );
}

template<>
inline bool leq (DDouble &v, DDouble &w)
{
    return (v <= w || std::isnan(w) );
}
template<typename T>
inline bool eq (T &v, T &w)
{
    return w == v;
}

template <typename IndexT>
inline void swap(IndexT* z , SizeT a, SizeT b)
{
    IndexT t = z[a];
    z[a] = z[b];
    z[b] = t;
}

template<typename T, typename IndexT>
inline void insertionSortIndex (T* val, IndexT* index, SizeT lo, SizeT hi) 
{
    for (SizeT i = lo; i <= hi; ++i)
    {
        for (SizeT j = i; j > lo && less(val[index[j]], val[index[j-1]]); --j)
        {
            swap(index,j,j-1);
        }
    }
}

template<typename T, typename IndexT>
inline SizeT median3_for_qsort ( T* val, IndexT* index, SizeT indI, SizeT indJ, SizeT indK)
{
    return (less(val[index[ indI]], val[index[ indJ]]) ?
            (less(val[index[ indJ]], val[index[ indK]]) ? indJ : less(val[index[ indI]], val[index[ indK]]) ? indK : indI) :
            (less(val[index[ indK]], val[index[ indJ]]) ? indJ : less(val[index[ indK]], val[index[ indI]]) ? indK : indI));
}


template <typename T, typename IndexT>
 static void QuickSortIndex(T* val, IndexT* index, DLong left, DLong right) 
{ 

    if (right <= 0)
        return;

    DLong length = right - left + 1;

    // cutoff to insertion sort
    if (length < INSERTION_SORT_THRESHOLD) 
    {
        insertionSortIndex(val, index, left, right);
        return;
    }

    // use median-of-3 as partitioning element
    else if (length < INSERTION_SORT_THRESHOLD*4) 
    {
        DLong median = median3_for_qsort(val, index, left, left + length / 2, right);
        swap(index,median,left);
    }

    // use Tukey ninther as partitioning element
    else  
    {
        DLong eps = length / 8;
        DLong mid = left + length / 2;
        DLong mFirst = median3_for_qsort(val, index, left, left + eps, left + eps + eps);
        DLong mMid = median3_for_qsort(val, index, mid - eps, mid, mid + eps);
        DLong mLast = median3_for_qsort(val, index, right - eps - eps, right - eps, right); 
        DLong ninther = median3_for_qsort(val, index, mFirst, mMid, mLast);
        swap(index,ninther,left);
    }

    // Bentley-McIlroy 3-way partitioning
    DLong i = left, j = right + 1;
    DLong index1 = left, index2 = right + 1;

    for (;; ) 
    {
        T pivot = val[index[left]];
        while (less(val[index[++i]], pivot))
        {
            if (i == right) 
                break;
        }
        while (less(pivot, val[index[--j]]))
        {
            if (j == left)    
                break;
        }
        if (i >= j) break;
        swap(index,i,j);
        if (eq(val[index[i]], pivot)) 
            swap(index,++index1,i);
        if (eq(val[index[j]], pivot)) 
            swap(index,--index2,j);
    }
    swap(index,left,j);

    i = j + 1;
    j = j - 1;
    for (DLong k = left + 1; k <= index1; ++k) 
    {
        swap(index,k,j--);
    }
    for (DLong k = right  ; k >= index2; --k)
    {
        swap(index,k,i++);
    }

//    QuickSortIndex(val, index, left, j);
//    QuickSortIndex(val, index, i, right);

     // same with parallelism
    DLong Left[2] = {left, i};
    DLong Right[2] = {j, right};
#pragma omp parallel for num_threads(2) if (length >= MERGESORT_PARALLEL_THRESHOLD && CpuTPOOL_NTHREADS > 1)
    for (int i = 0; i < 2; i++) QuickSortIndex(val, index, Left[i], Right[i]);
}

  
  template< typename T, typename IndexT>
   static void MergeNoCopyIndexAux(IndexT* aux, IndexT* index, SizeT low, SizeT mid, SizeT high, T* val)
  {
    SizeT i = low, j = mid + 1;
    for (SizeT k = low; k <= high; ++k) {
      if (i > mid) index[k] = aux[j++];
      else if (j > high) index[k] = aux[i++];
      else if (less(val[aux[j]] , val[aux[i]])) index[k] = aux[j++];
      else index[k] = aux[i++];
    }
  }

  template< typename T, typename IndexT>
   static void MergeSortIndexAux(IndexT* aux, IndexT* index, SizeT low, SizeT high, T* val)
  {
    SizeT length = high - low + 1;
    if (length < 2) return;
    if (length < INSERTION_SORT_THRESHOLD) {
      insertionSortIndex(val, index, low, high);
      memcpy(&(aux[low]), &(index[low]), length*sizeof(IndexT));
      return;
    } 

//    SizeT mid = low + (high - low) / 2;
//    MergeSortIndexAux(index, aux, low, mid, val);
//    MergeSortIndexAux(index, aux, mid+1, high, val);

//     same with parallelism
    SizeT mid = low + (high - low) / 2;
    SizeT Left[2] = {low, mid + 1};
    SizeT Right[2] = {mid, high};
#pragma omp parallel for num_threads(2) if (length >= MERGESORT_PARALLEL_THRESHOLD && CpuTPOOL_NTHREADS > 1)
    for (int i = 0; i < 2; i++) MergeSortIndexAux(index, aux, Left[i], Right[i], val);

    // If arrays are already sorted, finished.  This is an
    // optimization that results in faster sorts for nearly ordered lists.
    if (val[aux[mid + 1]] >= val[aux[mid]]) {
      memcpy(&(index[low]), &(aux[low]), length * sizeof (IndexT)); //give back sub
      return;
    }

    // If arrays are inverted just swap.
    if (leq(val[aux[high]], val[aux[low]])) {
      SizeT left = mid - low + 1;
      SizeT right = high - mid;
      // swap parts:
      memmove(&(index[low]), &(aux[low]), left * sizeof (IndexT)); //copy 'left' values in aux
      memmove(&(aux[low]), &(aux[mid + 1]), right * sizeof (IndexT)); //copy 'right' values starting at low
      memmove(&(aux[low + right]), &(index[low]), left * sizeof (IndexT)); //give back aux
      memcpy(&(index[low]), &(aux[low]), length * sizeof (IndexT)); //give back sub
      return;
    }

    MergeNoCopyIndexAux(aux, index, low, mid, high, val);
  }
  
    template< typename T, typename IndexT>
   static void AdaptiveSortIndexAux(IndexT* aux, IndexT* index, SizeT low, SizeT high, T* val)
  {
    SizeT length = high - low + 1;
    if (length < 2) return;
    if (length < INSERTION_SORT_THRESHOLD) {
      insertionSortIndex(val, index, low, high);
      return;
    }
    //  RadixSort is stable and differentiates -0 and +0.
    //  InsertionSort and Mergesort are stable but do not differentiate -0 and +0
    //  Quicksort is not stable https://en.wikipedia.org/wiki/Sorting_algorithm#Stability (does not keep temporal coherence) 
    //  and do not differentiate -0 and +0
    //  Quicksort should not be permitted for the default sorting algorithms, but since IDL says that
    //  "If Array contains any identical elements, the order in which the identical elements
    //  are sorted is arbitrary and may vary between operating systems.", I permit it.
    else if (length < QUICK_SORT_THRESHOLD) {
      QuickSortIndex(val, index, low, high);
      return;
    }
    else if (length < RADIX_SORT_THRESHOLD) { //could be faster if alloc/dealloc was not performed in RadixSort...
      RadixSortIndex(val, index, low, high);
      return;
    }

//    SizeT mid = low + (high - low) / 2;
//    AdaptiveSortIndexAux(index, aux, low, mid, val);
//    AdaptiveSortIndexAux(index, aux, mid+1, high, val);

    // same with parallelism
    SizeT mid = low + (high - low) / 2;
    SizeT Left[2] = {low, mid + 1};
    SizeT Right[2] = {mid, high};
#pragma omp parallel for num_threads(2) if (length >= MERGESORT_PARALLEL_THRESHOLD && CpuTPOOL_NTHREADS > 1)
    for (int i = 0; i < 2; i++) AdaptiveSortIndexAux(index, aux, Left[i], Right[i], val);

    // If arrays are already sorted, finished.  This is an
    // optimization that results in faster sorts for nearly ordered lists.
    if (val[aux[mid + 1]] >= val[aux[mid]]) {
      memcpy(&(index[low]), &(aux[low]), length * sizeof (IndexT)); //give back sub
      return;
    }

    // If arrays are inverted just swap.
    if (leq(val[aux[high]] ,val[aux[low]])) {
      SizeT left = mid - low + 1;
      SizeT right = high - mid;
      // swap parts:
      memmove(&(index[low]), &(aux[low]), left * sizeof (IndexT)); //copy 'left' values in aux
      memmove(&(aux[low]), &(aux[mid + 1]), right * sizeof (IndexT)); //copy 'right' values starting at low
      memmove(&(aux[low + right]), &(index[low]), left * sizeof (IndexT)); //give back aux
      memcpy(&(index[low]), &(aux[low]), length * sizeof (IndexT)); //give back sub
      return;
    }

    MergeNoCopyIndexAux(aux, index, low, mid, high, val);
  }

  template< typename T, typename IndexT>
  inline void MergeSortIndex(T* val, IndexT* index, SizeT low, SizeT high)
  {
    IndexT* aux = new IndexT[high - low + 1];
    for (SizeT i = 0; i < high - low + 1; ++i) aux[i] = i;
    MergeSortIndexAux(aux, index, low, high, val);
    delete[] aux;
  }

  template< typename T, typename IndexT>
  inline void AdaptiveSortIndex(T* val, IndexT* index, SizeT low, SizeT high)
  {
    IndexT* aux = new IndexT[high - low + 1];
    for (SizeT i = 0; i < high - low + 1; ++i) aux[i] = i;
    AdaptiveSortIndexAux(aux, index, low, high, val);
    delete[] aux;
  }
  
//--------------------------------------------------------------------------------------------------------------------
// Sorting algos: The "private" GDL_SORT enables keywords QUICK,MERGE,RADIX,INSERT. Those are not there to for the user
// to choose the algo (s)he wants. They are primarily to test the relative speed of each of them and find, for a given machine,
// where the default algorithm, that is basically a clever combinaison of all of them, should switch from algo to algo
// in order of retain the maximum efficiency.
// One could use testsuite/benchmark/compare_sort_algos.pro to, e.g., update the magic values in the code, or, better, this can
// be done automatically via a training procedure, writing results in a file in ~/.gdl and the present code could use, for example
// values of !GDL_SORT_THRESHOLDS, a system variable (struct) whose values would be updated in the ~/.gdl file.
// Default values chosen are for my intel i7, 4 cores, 8 threads.
      
  BaseGDL* gdl_sort_fun( EnvT* e)
  {
    e->NParam(1);

    BaseGDL* p0 = e->GetParDefined(0);

    if (p0->Type() == GDL_STRUCT)
      e->Throw("Struct expression not allowed in this context: " +
        e->GetParString(0));


    static int l64Ix = e->KeywordIx("L64");
    bool l64 = e->KeywordSet(l64Ix);

    static int qsortIx = e->KeywordIx("QUICK");
    bool quick = e->KeywordSet(qsortIx);
    static int mergeIx = e->KeywordIx("MERGE");
    bool merge = e->KeywordSet(mergeIx);
    static int radixIx = e->KeywordIx("RADIX");
    bool radix = e->KeywordSet(radixIx);
    static int insertIx = e->KeywordIx("INSERT");
    bool insert = e->KeywordSet(insertIx);
    static int autoIx = e->KeywordIx("AUTO");
    bool doauto = e->KeywordSet(autoIx);
    if (!(radix || quick ||merge|| insert || doauto)) e->Throw("I need one of QUICK, MERGE, RADIX, INSERT or AUTO keyword set.");

    SizeT nEl = p0->N_Elements();
    
    //Radix sort is special in that it is not obvious ---due to shortcuts--- which of the 2 internal storage
    //arrays is returned as sorted index.
    if (radix) {
        DLongGDL* res = new DLongGDL(dimension(nEl), BaseGDL::NOALLOC);
        DLong* index;
        if (p0->Type() == GDL_DOUBLE) {
        DDouble* val = (DDouble*)(static_cast<DDoubleGDL*> (p0)->DataAddr());
        index=(DLong*)RadixSort<DULong>(val, nEl);
      } else if (p0->Type() == GDL_FLOAT) {
	DFloat* val = (DFloat*)(static_cast<DFloatGDL*> (p0)->DataAddr());
        index=(DLong*)RadixSort<DULong>(val, nEl);
      } else if (p0->Type() == GDL_LONG) {
	DLong* val = ( DLong*)(static_cast<DLongGDL*> (p0)->DataAddr());
        index=(DLong*)RadixSort<DULong>( val, nEl);
      } else if (p0->Type() == GDL_ULONG) {
	DULong* val = (DULong*)(static_cast<DULongGDL*> (p0)->DataAddr());
        index=(DLong*)RadixSort<DULong>( val, nEl);
      } else if (p0->Type() == GDL_INT) {
	DInt* val = (DInt*)(static_cast<DIntGDL*> (p0)->DataAddr());
        index=(DLong*)RadixSort<DULong>( val, nEl);
      } else if (p0->Type() == GDL_UINT) {
	DUInt* val = (DUInt*)(static_cast<DUIntGDL*> (p0)->DataAddr());
        index=(DLong*)RadixSort<DULong>( val, nEl);
      } else if (p0->Type() == GDL_BYTE) {
	  DByte* val = (DByte*)(static_cast<DByteGDL*> (p0)->DataAddr());
        index=(DLong*)RadixSort<DULong>( val,nEl);
      } else if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* p0F = static_cast<DComplexGDL*> (p0);
        DComplex *ff=(DComplex*)p0F->DataAddr();
        // create temp values for magnitude of complex
        DFloat* magnitude=new DFloat[nEl];
        for (SizeT i=0; i< nEl; ++i) magnitude[i]=std::norm(ff[i]);
        index=(DLong*)RadixSort<DULong>(magnitude, nEl);
        delete[] magnitude;
      } else if (p0->Type() == GDL_COMPLEXDBL) {
        DComplexDblGDL* p0F = static_cast<DComplexDblGDL*> (p0);
        DComplexDbl *ff=(DComplexDbl*)p0F->DataAddr();
        // create temp values for magnitude of complex
        DDouble* magnitude=new DDouble[nEl];
        for (SizeT i=0; i< nEl; ++i) magnitude[i]=std::norm(ff[i]);
        index=(DLong*)RadixSort<DULong>(magnitude, nEl);
        delete[] magnitude;
      }
      res->SetBuffer(index);
      res->SetBufferSize(nEl);
      res->SetDim(dimension(nEl));
      return res;
    } else  {
      if (p0->Type() == GDL_DOUBLE) {
        DDouble* val = (DDouble*)(static_cast<DDoubleGDL*> (p0)->DataAddr());
        DLongGDL* res = new DLongGDL(dimension(nEl), BaseGDL::INDGEN);
        // NaNs are not well handled by the other sorts...
        DLong *hh = static_cast<DLong*> (res->DataAddr());
        SizeT low=0; 
        SizeT high=nEl-1;
        if (merge) {
          MergeSortIndex(val, hh, low, high);
        } else if (quick) {
          QuickSortIndex(val, hh, low, high);
        } else if (insert) {
          insertionSortIndex( val, hh, low, high);
        } else {
                AdaptiveSortIndex( val, hh, low, high);
        }
        return res;
    } else if (p0->Type() == GDL_FLOAT) {
        DFloat* val = (DFloat*)(static_cast<DFloatGDL*>(p0)->DataAddr());
        DLongGDL* res = new DLongGDL(dimension(nEl), BaseGDL::INDGEN);
        // NaNs are not well handled by the other sorts...
        DLong *hh = static_cast<DLong*> (res->DataAddr());
        SizeT low=0; 
        SizeT high=nEl-1;
        if (merge) {
          MergeSortIndex( val, hh, low, high);
        } else if (quick) {
          QuickSortIndex( val, hh, low, high);
        } else if (insert) {
          insertionSortIndex( val, hh, low, high);
        } else {
                AdaptiveSortIndex( val, hh, low, high);
        }
        return res;
    } else if (p0->Type() == GDL_LONG) {
      DLong* val = (DLong*)(static_cast<DLongGDL*>(p0)->DataAddr());
      DLongGDL* res = new DLongGDL(dimension(nEl), BaseGDL::INDGEN);
      DLong *hh = static_cast<DLong*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1;
      if (merge) {
        MergeSortIndex( val, hh, low, high);
      } else if (quick) {
        QuickSortIndex( val, hh, low, high);
      } else if (insert) {
          insertionSortIndex( val, hh, low, high);
      } else {
              AdaptiveSortIndex( val, hh, low, high);
      }
      return res;
    } else if (p0->Type() == GDL_ULONG) {
      DULong* val = (DULong*)(static_cast<DULongGDL*>(p0)->DataAddr());
      DLongGDL* res = new DLongGDL(dimension(nEl), BaseGDL::INDGEN);
      DLong *hh = static_cast<DLong*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1;
      if (merge) {
        MergeSortIndex( val, hh, low, high);
      } else if (quick) {
        QuickSortIndex( val, hh, low, high);
      } else if (insert) {
          insertionSortIndex( val, hh, low, high);
      } else {
              AdaptiveSortIndex( val, hh, low, high);
      }
      return res;
    } else if (p0->Type() == GDL_INT) {
      DInt* val = (DInt*)(static_cast<DIntGDL*>(p0)->DataAddr());
      DLongGDL* res = new DLongGDL(dimension(nEl), BaseGDL::INDGEN);
      DLong *hh = static_cast<DLong*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1;
      if (merge) {
        MergeSortIndex( val, hh, low, high);
      } else if (quick) {
        QuickSortIndex( val, hh, low, high);
      } else if (insert) {
          insertionSortIndex( val, hh, low, high);
      } else {
              AdaptiveSortIndex( val, hh, low, high);
      }
      return res;
    } else if (p0->Type() == GDL_UINT) {
      DUInt* val = (DUInt*)(static_cast<DUIntGDL*>(p0)->DataAddr());
      DLongGDL* res = new DLongGDL(dimension(nEl), BaseGDL::INDGEN);
      DLong *hh = static_cast<DLong*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1;
      if (merge) {
        MergeSortIndex( val, hh, low, high);
      } else if (quick) {
        QuickSortIndex( val, hh, low, high);
      } else if (insert) {
          insertionSortIndex( val, hh, low, high);
      } else {
              AdaptiveSortIndex( val, hh, low, high);
      }
      return res;
    } else if (p0->Type() == GDL_BYTE) {
      DByte* val = (DByte*)(static_cast<DByteGDL*>(p0)->DataAddr());
      DLongGDL* res = new DLongGDL(dimension(nEl), BaseGDL::INDGEN);
      DLong *hh = static_cast<DLong*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1;
      if (merge) {
        MergeSortIndex( val, hh, low, high);
      } else if (quick) {
        QuickSortIndex( val, hh, low, high);
      } else if (insert) {
          insertionSortIndex( val, hh, low, high);
      } else {
              AdaptiveSortIndex( val, hh, low, high);
      }
      return res;
    } else if (p0->Type() == GDL_COMPLEX) {
      DComplexGDL* p0F = static_cast<DComplexGDL*> (p0);
      DComplex *ff=(DComplex*)p0F->DataAddr();
      // create temp values for magnitude of complex
      DFloat* magnitude=new DFloat[nEl];
      for (SizeT i=0; i< nEl; ++i) magnitude[i]=std::norm(ff[i]);
      DLongGDL* res = new DLongGDL(dimension(nEl), BaseGDL::INDGEN);
      DLong *hh = static_cast<DLong*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1;
      if (merge) {
        MergeSortIndex( magnitude, hh, low, high);
      } else if (quick) {
        QuickSortIndex( magnitude, hh, low, high);
      } else if (insert) {
          insertionSortIndex( magnitude, hh, low, high);
      } else {
              AdaptiveSortIndex( magnitude, hh, low, high);
      }
      delete[] magnitude;
      return res;
    } else if (p0->Type() == GDL_COMPLEXDBL) {
      DComplexDblGDL* p0F = static_cast<DComplexDblGDL*> (p0);
      DComplexDbl *ff=(DComplexDbl*)p0F->DataAddr();
      // create temp values for magnitude of complex
      DDouble* magnitude=new DDouble[nEl];
      for (SizeT i=0; i< nEl; ++i) magnitude[i]=std::norm(ff[i]);
      DLongGDL* res = new DLongGDL(dimension(nEl), BaseGDL::INDGEN);
      DLong *hh = static_cast<DLong*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1;
      if (merge) {
        MergeSortIndex( magnitude, hh, low, high);
      } else if (quick) {
        QuickSortIndex( magnitude, hh, low, high);
      } else if (insert) {
          insertionSortIndex( magnitude, hh, low, high);
      } else {
              AdaptiveSortIndex( magnitude, hh, low, high);
      }
      delete[] magnitude;
      return res;
    } else e->Throw("FIXME."); 
   }
   return NULL;
  }
  
  template<typename GDLIndexT, typename IndexT>
  inline BaseGDL* do_sort_fun(BaseGDL* p0)
  {
    SizeT nEl = p0->N_Elements();
    if (p0->Type() == GDL_BYTE) { //lack of 'res' creation overhead makes "Bytes Radix Sort" better than anything else.
      DByte* val = (DByte*)(static_cast<DByteGDL*>(p0)->DataAddr());
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::NOALLOC);
      IndexT *index;
      index=RadixSort<IndexT>( val, nEl);
      res->SetBuffer(index);
      res->SetBufferSize(nEl);
      res->SetDim(dimension(nEl));
      return res;
    } else if (p0->Type() == GDL_INT) { //still true for INTs
      DInt* val = (DInt*)(static_cast<DIntGDL*>(p0)->DataAddr());
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::NOALLOC);
      IndexT *index;
      index=RadixSort<IndexT>( val, nEl);
      res->SetBuffer(index);
      res->SetBufferSize(nEl);
      res->SetDim(dimension(nEl));
      return res;
    } else if (p0->Type() == GDL_UINT) { //still true for UINTs
      DUInt* val = (DUInt*)(static_cast<DUIntGDL*>(p0)->DataAddr());
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::NOALLOC);
      IndexT *index;
      index=RadixSort<IndexT>( val, nEl);
      res->SetBuffer(index);
      res->SetBufferSize(nEl);
      res->SetDim(dimension(nEl));
      return res;
    } else if (p0->Type() == GDL_FLOAT) {
      DFloat* val = (DFloat*)(static_cast<DFloatGDL*>(p0)->DataAddr());
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::INDGEN);
      IndexT *hh = static_cast<IndexT*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1;
      AdaptiveSortIndex<DFloat, IndexT>( val, hh, low, high);
      return res;
    } else if (p0->Type() == GDL_DOUBLE) {
      DDouble* val = (DDouble*)(static_cast<DDoubleGDL*>(p0)->DataAddr());
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::INDGEN);
      IndexT *hh = static_cast<IndexT*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1; 
      AdaptiveSortIndex<DDouble, IndexT>( val, hh, low, high);
      return res;
    } else if (p0->Type() == GDL_COMPLEX) {
      DComplexGDL* p0F = static_cast<DComplexGDL*> (p0);
      DComplex *ff=(DComplex*)p0F->DataAddr();
      // create temp values for magnitude of complex
      DFloat* magnitude=new DFloat[nEl];
      for (SizeT i=0; i< nEl; ++i) magnitude[i]=std::norm(ff[i]);
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::INDGEN);
      IndexT *hh = static_cast<IndexT*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1;
      AdaptiveSortIndex<DFloat, IndexT>( magnitude, hh, low, high);
      delete[] magnitude;
      return res;
    } else if (p0->Type() == GDL_COMPLEXDBL) {
      DComplexDblGDL* p0F = static_cast<DComplexDblGDL*> (p0);
      DComplexDbl *ff=(DComplexDbl*)p0F->DataAddr();
      // create temp values for magnitude of complex
      DDouble* magnitude=new DDouble[nEl];
      for (SizeT i=0; i< nEl; ++i) magnitude[i]=std::norm(ff[i]);
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::INDGEN);
      IndexT *hh = static_cast<IndexT*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1;
      AdaptiveSortIndex<DDouble, IndexT>( magnitude, hh, low, high);
      delete[] magnitude;
      return res;
    } else if (p0->Type() == GDL_LONG) {
      DLong* val = (DLong*)(static_cast<DLongGDL*>(p0)->DataAddr());
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::INDGEN);
      IndexT *hh = static_cast<IndexT*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1; 
      AdaptiveSortIndex<DLong, IndexT>( val, hh, low, high);
      return res;
    } else if (p0->Type() == GDL_ULONG) {
      DULong* val = (DULong*)(static_cast<DULongGDL*>(p0)->DataAddr());
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::INDGEN);
      IndexT *hh = static_cast<IndexT*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1; 
      AdaptiveSortIndex<DULong, IndexT>( val, hh, low, high);
      return res;
    } else if (p0->Type() == GDL_LONG64) {
      DLong64* val = (DLong64*)(static_cast<DLong64GDL*>(p0)->DataAddr());
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::INDGEN);
      IndexT *hh = static_cast<IndexT*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1; 
      AdaptiveSortIndex<DLong64, IndexT>( val, hh, low, high);
      return res;
    } else if (p0->Type() == GDL_ULONG64) {
      DULong64* val = (DULong64*)(static_cast<DULong64GDL*>(p0)->DataAddr());
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::INDGEN);
      IndexT *hh = static_cast<IndexT*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1; 
      AdaptiveSortIndex<DULong64, IndexT>( val, hh, low, high);
      return res;
    } else if (p0->Type() == GDL_STRING) {
      DString* val = (DString*)(static_cast<DStringGDL*>(p0)->DataAddr());
      GDLIndexT* res = new GDLIndexT(dimension(nEl), BaseGDL::INDGEN);
      IndexT *hh = static_cast<IndexT*> (res->DataAddr());
      SizeT low=0; 
      SizeT high=nEl-1; 
      QuickSortIndex<DString, IndexT>( val, hh, low, high);
      return res;
    }
    return NULL;
  }
  BaseGDL* sort_fun( EnvT* e)
  {
    BaseGDL* p0 = e->GetParDefined(0);
    if (p0->Type() == GDL_STRUCT) e->Throw("Struct expression not allowed in this context: " +e->GetParString(0));
    static int l64Ix = e->KeywordIx("L64");
    bool l64 = e->KeywordSet(l64Ix);
    if (!l64) return do_sort_fun<DLongGDL,DLong>(p0);
    else return do_sort_fun<DLong64GDL,DLong64>(p0);
  }  
}
