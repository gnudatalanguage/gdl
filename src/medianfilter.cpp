// Copyright (c) 2014, Jukka Suomela.
//
// You can distribute and use this software under the MIT license: http://opensource.org/licenses/MIT
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction, 
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// To contact the author, see http://users.ics.aalto.fi/suomela/

// modified by GD to mimic the /EVEN switch of IDL. Note that modifying these optimized algorithms for non-odd sizes is tricky.

namespace fastmedian{  //make it apart from the rest...

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <stdint.h>

const uint64_t ONE64 = 1;


// Reasonable values based on benchmarks

inline int choose_blocksize_1d(int h) {
    return 8*(h + 2);
}

inline int choose_blocksize_2d(int h) {
    return 4*(h + 2);
}



// Find nth bit that is set and return its index
// (no such bit: output undefined)

inline int findnth64(uint64_t x, int n) {
#ifdef __AVX2__
    x = _pdep_u64(ONE64 << n, x);
#else
    for (int i = 0; i < n; ++i) {
        x &= x - 1;
    }
#endif
    return __builtin_ctzll(x);
}

inline int popcnt64(uint64_t x) {
    return __builtin_popcountll(x);
}


// Grid dimensions.

class Dim {
public:
    Dim(int b_, int size_, int h_)
        : size(size_),
          h(h_),
          step(calc_step(b_, h_)),
          count(calc_count(b_, size_, h_))
    {
        assert(2 * h + 1 < b_);
        assert(count >= 1);
        assert(2 * h + count * step >= size);
        assert(2 * h + (count - 1) * step < size || count == 1);
    }

    const int size;
    const int h;
    const int step;
    const int count;

private:
    inline static int calc_step(int b, int h) {
        return b - 2*h;
    }

    inline static int calc_count(int b, int size, int h) {
        if (size <= b) {
            return 1;
        } else {
            int interior = size - 2 * h;
            int step = calc_step(b, h);
            return (interior + step - 1) / step;
        }
    }
};


// Slot i in the grid.

struct BDim {
    BDim(Dim dim_) : dim(dim_) {
        set(0);
    }

    inline void set(int i) {
        bool is_first = (i == 0);
        bool is_last = (i + 1 == dim.count);
        start = dim.step * i;
        int end;
        if (is_last) {
            end = dim.size;
        } else {
            end = 2 * dim.h + (i + 1) * dim.step;
        }
        size = end - start;
        b0 = is_first ? 0 : dim.h;
        b1 = is_last ? size : size - dim.h;
    }

    // The window around point v is [w0(v), w1(v)).
    // 0 <= w0(v) <= v < w1(v) <= size
    inline int w0(int v) const {
        assert(b0 <= v);
        assert(v < b1);
        return std::max(0, v - dim.h);
    }

    inline int w1(int v) const {
        assert(b0 <= v);
        assert(v < b1);
        return std::min(v + 1 + dim.h, size);
    }

    // Block i is located at coordinates [start, end) in the image.
    // Within the block, median is needed for coordinates [b0, b1).
    // 0 <= start < end < dim.size
    // 0 <= b0 < b1 < size <= dim.b
    const Dim dim;
    int start;
    int size;
    int b0;
    int b1;
};


// Data structure for the sliding window.

class Window {
public:
    Window(int bb)
        : words(get_words(bb)),
          buf(new uint64_t[words])
    {}

    ~Window() {
        delete[] buf;
    }

    inline void clear()
    {
        for (int i = 0; i < words; ++i) {
            buf[i] = 0;
        }
        half[0] = 0;
        half[1] = 0;
        p = words / 2;
    }

    inline void update(int op, int s) {
        assert(op == -1 || op == +1);
        int i = s >> WORD_SHIFT;
        int j = s & WORD_MASK;
        if (op == +1) {
            assert(!(buf[i] & (ONE64 << j)));
        } else {
            assert(buf[i] & (ONE64 << j));
        }
        buf[i] ^= (ONE64 << j);
        half[i >= p] += op;
    }

    inline int size() const {
        return half[0] + half[1];
    }

    inline int find(int goal) {
        while (half[0] > goal) {
            --p;
            half[0] -= popcnt64(buf[p]);
            half[1] += popcnt64(buf[p]);
        }
        while (half[0] + popcnt64(buf[p]) <= goal) {
            half[0] += popcnt64(buf[p]);
            half[1] -= popcnt64(buf[p]);
            ++p;
        }
        int n = goal - half[0];
        assert(0 <= n && n < popcnt64(buf[p]));
        int j = findnth64(buf[p], n);
        return (p << WORD_SHIFT) | j;
    }

private:
    static inline int get_words(int bb) {
        assert(bb >= 1);
        return (bb + WORD_SIZE - 1) / WORD_SIZE;
    }

    static const int WORD_SHIFT = 6;
    static const int WORD_SIZE = 1 << WORD_SHIFT;
    static const int WORD_MASK = WORD_SIZE - 1;

    // Size of buf.
    const int words;
    // Bit number s is on iff element s is inside the window.
    uint64_t * const buf;
    // half[0] = popcount of buf[0] ... buf[p-1]
    // half[1] = popcount of buf[p] ... buf[words-1]
    int half[2];
    // The current guess is that the median is in buf[p].
    int p;
};


template <typename T>
class WindowRank {
public:
    WindowRank(int bb_)
        : sorted(new std::pair<T,int>[bb_]),
          rank(new int[bb_]),
          window(bb_),
          bb(bb_)
    {}

    ~WindowRank()
    {
        delete[] sorted;
        delete[] rank;
    }

    void init_start() {
        size = 0;
    }

    inline void init_feed(T value, int slot) {
        if (std::isnan(value)) {
            rank[slot] = NAN_MARKER;
        } else {
            sorted[size] = std::make_pair(value, slot);
            ++size;
        }
    }

    void init_finish() {
        std::sort(sorted, sorted + size);
        for (int i = 0; i < size; ++i) {
            rank[sorted[i].second] = i;
        }
    }

    inline void clear() {
        window.clear();
    }

    inline void update(int op, int slot) {
        int s = rank[slot];
        if (s != NAN_MARKER) {
            window.update(op, s);
        }
    }

    inline T get_med() {
        int total = window.size();
        if (total == 0) {
            return std::numeric_limits<T>::quiet_NaN();
        } else {
            int goal1 = (total - 1) / 2;
            int goal2 = (total - 0) / 2;
            int med1 = window.find(goal1);
            T value = sorted[med1].first;
            if (goal2 != goal1) {
                int med2 = window.find(goal2);
                assert(med2 > med1);
                value += sorted[med2].first;
                value /= 2;
            }
            return value;
        }
    }

private:
    std::pair<T,int>* const sorted;
    int* const rank;
    Window window;
    const int bb;
    int size;
    static const int NAN_MARKER = -1;
};


// MedCalc2D.run(i,j) calculates medians for block (i,j).

template <typename T>
class MedCalc2D {
public:
    MedCalc2D(int b_, Dim dimx_, Dim dimy_, const T* in_, T* out_)
        : wr(b_ * b_), bx(dimx_), by(dimy_), in(in_), out(out_)
    {}

    void run(int bx_, int by_)
    {
        bx.set(bx_);
        by.set(by_);
        calc_rank();
        medians();
    }

private:
    void calc_rank() {
        wr.init_start();
        for (int y = 0; y < by.size; ++y) {
            for (int x = 0; x < bx.size; ++x) {
                wr.init_feed(in[coord(x, y)], pack(x, y));
            }
        }
        wr.init_finish();
    }

    void medians() {
#ifdef NAIVE
        for (int y = by.b0; y < by.b1; ++y) {
            for (int x = bx.b0; x < bx.b1; ++x) {
                wr.clear();
                update_block(+1, bx.w0(x), bx.w1(x), by.w0(y), by.w1(y));
                set_med(x, y);
            }
        }
#else
        wr.clear();
        int x = bx.b0;
        int y = by.b0;
        update_block(+1, bx.w0(x), bx.w1(x), by.w0(y), by.w1(y));
        set_med(x, y);
        bool down = true;
        while (true) {
            bool right = false;
            if (down) {
                if (y + 1 == by.b1) {
                    right = true;
                    down = false;
                }
            } else {
                if (y == by.b0) {
                    right = true;
                    down = true;
                }
            }
            if (right) {
                if (x + 1 == bx.b1) {
                    break;
                }
            }
            if (right) {
                update_block(-1, bx.w0(x), bx.w0(x+1), by.w0(y), by.w1(y));
                ++x;
                update_block(+1, bx.w1(x-1), bx.w1(x), by.w0(y), by.w1(y));
            } else if (down) {
                update_block(-1, bx.w0(x), bx.w1(x), by.w0(y), by.w0(y+1));
                ++y;
                update_block(+1, bx.w0(x), bx.w1(x), by.w1(y-1), by.w1(y));
            } else {
                update_block(-1, bx.w0(x), bx.w1(x), by.w1(y-1), by.w1(y));
                --y;
                update_block(+1, bx.w0(x), bx.w1(x), by.w0(y), by.w0(y+1));
            }
            set_med(x, y);
        }
#endif
    }

    inline void update_block(int op, int x0, int x1, int y0, int y1) {
        for (int y = y0; y < y1; ++y) {
            for (int x = x0; x < x1; ++x) {
                wr.update(op, pack(x, y));
            }
        }
    }

    inline void set_med(int x, int y) {
        out[coord(x, y)] = wr.get_med();
    }

    inline int pack(int x, int y) const {
        return y * bx.size + x;
    }

    inline int coord(int x, int y) const {
        return (y + by.start) * bx.dim.size + (x + bx.start);
    }

    WindowRank<T> wr;
    BDim bx;
    BDim by;
    const T* const in;
    T* const out;
};



template <typename T>
class MedCalc1D {
public:
    MedCalc1D(int b_, Dim dimx_, const T* in_, T* out_)
        : wr(b_), bx(dimx_), in(in_), out(out_)
    {}

    void run(int bx_)
    {
        bx.set(bx_);
        calc_rank();
        medians();
    }

private:
    void calc_rank() {
        wr.init_start();
        for (int x = 0; x < bx.size; ++x) {
            wr.init_feed(in[coord(x)], pack(x));
        }
        wr.init_finish();
    }

    void medians() {
#ifdef NAIVE
        for (int x = bx.b0; x < bx.b1; ++x) {
            wr.clear();
            update_block(+1, bx.w0(x), bx.w1(x));
            set_med(x);
        }
#else
        wr.clear();
        int x = bx.b0;
        update_block(+1, bx.w0(x), bx.w1(x));
        set_med(x);
        while (x + 1 < bx.b1) {
            if (x >= bx.dim.h) {
                wr.update(-1, pack(x - bx.dim.h));
            }
            ++x;
            if (x + bx.dim.h < bx.size) {
                wr.update(+1, pack(x + bx.dim.h));
            }
            set_med(x);
        }
#endif
    }

    inline void update_block(int op, int x0, int x1) {
        for (int x = x0; x < x1; ++x) {
            wr.update(op, pack(x));
        }
    }

    inline void set_med(int x) {
        out[coord(x)] = wr.get_med();
    }

    inline int pack(int x) const {
        return x;
    }

    inline int coord(int x) const {
        return x + bx.start;
    }

    WindowRank<T> wr;
    BDim bx;
    const T* const in;
    T* const out;
};


template <typename T>
void median_filter_impl_2d(int x, int y, int hx, int hy, int b, const T* in, T* out) {
    if (2 * hx + 1 > b || 2 * hy + 1 > b) {
        throw std::invalid_argument("window too large for this block size");
    }
    Dim dimx(b, x, hx);
    Dim dimy(b, y, hy);
    #pragma omp parallel
    {
        MedCalc2D<T> mc(b, dimx, dimy, in, out);
        #pragma omp for collapse(2)
        for (int by = 0; by < dimy.count; ++by) {
            for (int bx = 0; bx < dimx.count; ++bx) {
                mc.run(bx, by);
            }
        }
    }
}


template <typename T>
void median_filter_impl_1d(int x, int hx, int b, const T* in, T* out) {
    if (2 * hx + 1 > b) {
        throw std::invalid_argument("window too large for this block size");
    }
    Dim dimx(b, x, hx);
    #pragma omp parallel
    {
        MedCalc1D<T> mc(b, dimx, in, out);
        #pragma omp for
        for (int bx = 0; bx < dimx.count; ++bx) {
            mc.run(bx);
        }
    }
}


template <typename T>
void median_filter_2d(int x, int y, int hx, int hy, int blockhint, const T* in, T* out) {
    int h = std::max(hx, hy);
    int blocksize = blockhint ? blockhint : choose_blocksize_2d(h);
    median_filter_impl_2d<T>(x, y, hx, hy, blocksize, in, out);
}

template <typename T>
void median_filter_1d(int x, int hx, int blockhint, const T* in, T* out) {
    int blocksize = blockhint ? blockhint : choose_blocksize_1d(hx);
    median_filter_impl_1d<T>(x, hx, blocksize, in, out);
}

template void median_filter_2d<float>(int x, int y, int hx, int hy, int blockhint, const float* in, float* out);
template void median_filter_2d<double>(int x, int y, int hx, int hy, int blockhint, const double* in, double* out);

template void median_filter_1d<float>(int x, int hx, int blockhint, const float* in, float* out);
template void median_filter_1d<double>(int x, int hx, int blockhint, const double* in, double* out);

/*
 * ctmf.c - Constant-time median filtering
 * Copyright (C) 2006  Simon Perreault
 *
 * Reference: S. Perreault and P. Halbert, "Median Filtering in Constant Time",
 * IEEE Transactions on Image Processing, September 2007.
 *
 * This program has been obtained from http://nomis80.org/ctmf.html. No patent
 * covers this program, although it is subject to the following license:
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact:
 *  Laboratoire de vision et systemes numeriques
 *  Pavillon Adrien-Pouliot
 *  Universite Laval
 *  Sainte-Foy, Quebec, Canada
 *  G1K 7P4
 *
 *  perreaul@gel.ulaval.ca
 */

/* Standard C includes */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Type declarations */
#ifdef _MSC_VER
#include <basetsd.h>
typedef UINT8 uint8_t;
typedef UINT16 uint16_t;
typedef UINT32 uint32_t;
#pragma warning( disable: 4799 )
#else
#include <stdint.h>
#endif

/* Intrinsic declarations */
#if defined(__SSE2__) || defined(__MMX__)
#if defined(__SSE2__)
#include <emmintrin.h>
#elif defined(__MMX__)
#include <mmintrin.h>
#endif
#if defined(__GNUC__)
#include <mm_malloc.h>
#elif defined(_MSC_VER)
#include <malloc.h>
#endif
#elif defined(__ALTIVEC__)
#include <altivec.h>
#endif

/* Compiler peculiarities */
#if defined(__GNUC__)
#include <stdint.h>
#define inline __inline__
#define align(x) __attribute__ ((aligned (x)))
#elif defined(_MSC_VER)
#define inline __inline
#define align(x) __declspec(align(x))
#else
#define inline
#define align(x)
#endif

#ifndef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

/**
 * This structure represents a two-tier histogram. The first tier (known as the
 * "coarse" level) is 4 bit wide and the second tier (known as the "fine" level)
 * is 8 bit wide. Pixels inserted in the fine level also get inserted into the
 * coarse bucket designated by the 4 MSBs of the fine bucket value.
 *
 * The structure is aligned on 16 bytes, which is a prerequisite for SIMD
 * instructions. Each bucket is 16 bit wide, which means that extra care must be
 * taken to prevent overflow.
 */
typedef struct align(16)
{
    uint16_t coarse[16];
    uint16_t fine[16][16];
} Histogram;

/**
 * HOP is short for Histogram OPeration. This macro makes an operation \a op on
 * histogram \a h for pixel value \a x. It takes care of handling both levels.
 */
#define HOP(h,x,op) \
    h.coarse[x>>4] op; \
    *((uint16_t*) h.fine + x) op;

#define COP(c,j,x,op) \
    h_coarse[ 16*(n*c+j) + (x>>4) ] op; \
    h_fine[ 16 * (n*(16*c+(x>>4)) + j) + (x & 0xF) ] op;

/**
 * Adds histograms \a x and \a y and stores the result in \a y. Makes use of
 * SSE2, MMX or Altivec, if available.
 */
#if defined(__SSE2__)
static inline void histogram_add( const uint16_t x[16], uint16_t y[16] )
{
    *(__m128i*) &y[0] = _mm_add_epi16( *(__m128i*) &y[0], *(__m128i*) &x[0] );
    *(__m128i*) &y[8] = _mm_add_epi16( *(__m128i*) &y[8], *(__m128i*) &x[8] );
}
#elif defined(__MMX__)
static inline void histogram_add( const uint16_t x[16], uint16_t y[16] )
{
    *(__m64*) &y[0]  = _mm_add_pi16( *(__m64*) &y[0],  *(__m64*) &x[0]  );
    *(__m64*) &y[4]  = _mm_add_pi16( *(__m64*) &y[4],  *(__m64*) &x[4]  );
    *(__m64*) &y[8]  = _mm_add_pi16( *(__m64*) &y[8],  *(__m64*) &x[8]  );
    *(__m64*) &y[12] = _mm_add_pi16( *(__m64*) &y[12], *(__m64*) &x[12] );
}
#elif defined(__ALTIVEC__)
static inline void histogram_add( const uint16_t x[16], uint16_t y[16] )
{
    *(__vector unsigned short*) &y[0] = vec_add( *(__vector unsigned short*) &y[0], *(__vector unsigned short*) &x[0] );
    *(__vector unsigned short*) &y[8] = vec_add( *(__vector unsigned short*) &y[8], *(__vector unsigned short*) &x[8] );
}
#else
static inline void histogram_add( const uint16_t x[16], uint16_t y[16] )
{
    int i;
    for ( i = 0; i < 16; ++i ) {
        y[i] += x[i];
    }
}
#endif

/**
 * Subtracts histogram \a x from \a y and stores the result in \a y. Makes use
 * of SSE2, MMX or Altivec, if available.
 */
#if defined(__SSE2__)
static inline void histogram_sub( const uint16_t x[16], uint16_t y[16] )
{
    *(__m128i*) &y[0] = _mm_sub_epi16( *(__m128i*) &y[0], *(__m128i*) &x[0] );
    *(__m128i*) &y[8] = _mm_sub_epi16( *(__m128i*) &y[8], *(__m128i*) &x[8] );
}
#elif defined(__MMX__)
static inline void histogram_sub( const uint16_t x[16], uint16_t y[16] )
{
    *(__m64*) &y[0]  = _mm_sub_pi16( *(__m64*) &y[0],  *(__m64*) &x[0]  );
    *(__m64*) &y[4]  = _mm_sub_pi16( *(__m64*) &y[4],  *(__m64*) &x[4]  );
    *(__m64*) &y[8]  = _mm_sub_pi16( *(__m64*) &y[8],  *(__m64*) &x[8]  );
    *(__m64*) &y[12] = _mm_sub_pi16( *(__m64*) &y[12], *(__m64*) &x[12] );
}
#elif defined(__ALTIVEC__)
static inline void histogram_sub( const uint16_t x[16], uint16_t y[16] )
{
    *(__vector unsigned short*) &y[0] = vec_sub( *(__vector unsigned short*) &y[0], *(__vector unsigned short*) &x[0] );
    *(__vector unsigned short*) &y[8] = vec_sub( *(__vector unsigned short*) &y[8], *(__vector unsigned short*) &x[8] );
}
#else
static inline void histogram_sub( const uint16_t x[16], uint16_t y[16] )
{
    int i;
    for ( i = 0; i < 16; ++i ) {
        y[i] -= x[i];
    }
}
#endif

static inline void histogram_muladd( const uint16_t a, const uint16_t x[16],
        uint16_t y[16] )
{
    int i;
    for ( i = 0; i < 16; ++i ) {
        y[i] += a * x[i];
    }
}

static void ctmf_helper(
        const unsigned char* const src, unsigned char* const dst,
        const int width, const int height,
        const int src_step, const int dst_step,
        const int r, const int cn,
        const int pad_left, const int pad_right
        )
{
    const int m = height, n = width;
    int i, j, k, c;
    const unsigned char *p, *q;

    Histogram H[4];
    uint16_t *h_coarse, *h_fine, luc[4][16];

    assert( src );
    assert( dst );
    assert( r >= 0 );
    assert( width >= 2*r+1 );
    assert( height >= 2*r+1 );
    assert( src_step != 0 );
    assert( dst_step != 0 );

    /* SSE2 and MMX need aligned memory, provided by _mm_malloc(). */
#if defined(__SSE2__) || defined(__MMX__)
    h_coarse = (uint16_t*) _mm_malloc(  1 * 16 * n * cn * sizeof(uint16_t), 16 );
    h_fine   = (uint16_t*) _mm_malloc( 16 * 16 * n * cn * sizeof(uint16_t), 16 );
    memset( h_coarse, 0,  1 * 16 * n * cn * sizeof(uint16_t) );
    memset( h_fine,   0, 16 * 16 * n * cn * sizeof(uint16_t) );
#else
    h_coarse = (uint16_t*) calloc(  1 * 16 * n * cn, sizeof(uint16_t) );
    h_fine   = (uint16_t*) calloc( 16 * 16 * n * cn, sizeof(uint16_t) );
#endif

    /* First row initialization */
    for ( j = 0; j < n; ++j ) {
        for ( c = 0; c < cn; ++c ) {
            COP( c, j, src[cn*j+c], += r+1 );
        }
    }
    for ( i = 0; i < r; ++i ) {
        for ( j = 0; j < n; ++j ) {
            for ( c = 0; c < cn; ++c ) {
                COP( c, j, src[src_step*i+cn*j+c], ++ );
            }
        }
    }

    for ( i = 0; i < m; ++i ) {

        /* Update column histograms for entire row. */
        p = src + src_step * MAX( 0, i-r-1 );
        q = p + cn * n;
        for ( j = 0; p != q; ++j ) {
            for ( c = 0; c < cn; ++c, ++p ) {
                COP( c, j, *p, -- );
            }
        }

        p = src + src_step * MIN( m-1, i+r );
        q = p + cn * n;
        for ( j = 0; p != q; ++j ) {
            for ( c = 0; c < cn; ++c, ++p ) {
                COP( c, j, *p, ++ );
            }
        }

        /* First column initialization */
        memset( H, 0, cn*sizeof(H[0]) );
        memset( luc, 0, cn*sizeof(luc[0]) );
        if ( pad_left ) {
            for ( c = 0; c < cn; ++c ) {
                histogram_muladd( r, &h_coarse[16*n*c], H[c].coarse );
            }
        }
        for ( j = 0; j < (pad_left ? r : 2*r); ++j ) {
            for ( c = 0; c < cn; ++c ) {
                histogram_add( &h_coarse[16*(n*c+j)], H[c].coarse );
            }
        }
        for ( c = 0; c < cn; ++c ) {
            for ( k = 0; k < 16; ++k ) {
                histogram_muladd( 2*r+1, &h_fine[16*n*(16*c+k)], &H[c].fine[k][0] );
            }
        }

        for ( j = pad_left ? 0 : r; j < (pad_right ? n : n-r); ++j ) {
            for ( c = 0; c < cn; ++c ) {
                const uint16_t t = 2*r*r + 2*r;
                uint16_t sum = 0, *segment;
                int b;

                histogram_add( &h_coarse[16*(n*c + MIN(j+r,n-1))], H[c].coarse );

                /* Find median at coarse level */
                for ( k = 0; k < 16 ; ++k ) {
                    sum += H[c].coarse[k];
                    if ( sum > t ) {
                        sum -= H[c].coarse[k];
                        break;
                    }
                }
                assert( k < 16 );

                /* Update corresponding histogram segment */
                if ( luc[c][k] <= j-r ) {
                    memset( &H[c].fine[k], 0, 16 * sizeof(uint16_t) );
                    for ( luc[c][k] = j-r; luc[c][k] < MIN(j+r+1,n); ++luc[c][k] ) {
                        histogram_add( &h_fine[16*(n*(16*c+k)+luc[c][k])], H[c].fine[k] );
                    }
                    if ( luc[c][k] < j+r+1 ) {
                        histogram_muladd( j+r+1 - n, &h_fine[16*(n*(16*c+k)+(n-1))], &H[c].fine[k][0] );
                        luc[c][k] = j+r+1;
                    }
                }
                else {
                    for ( ; luc[c][k] < j+r+1; ++luc[c][k] ) {
                        histogram_sub( &h_fine[16*(n*(16*c+k)+MAX(luc[c][k]-2*r-1,0))], H[c].fine[k] );
                        histogram_add( &h_fine[16*(n*(16*c+k)+MIN(luc[c][k],n-1))], H[c].fine[k] );
                    }
                }

                histogram_sub( &h_coarse[16*(n*c+MAX(j-r,0))], H[c].coarse );

                /* Find median in segment */
                segment = H[c].fine[k];
                for ( b = 0; b < 16 ; ++b ) {
                    sum += segment[b];
                    if ( sum > t ) {
                        dst[dst_step*i+cn*j+c] = 16*k + b;
                        break;
                    }
                }
                assert( b < 16 );
            }
        }
    }

#if defined(__SSE2__) || defined(__MMX__)
    _mm_empty();
    _mm_free(h_coarse);
    _mm_free(h_fine);
#else
    free(h_coarse);
    free(h_fine);
#endif
}

/**
 * \brief Constant-time median filtering
 *
 * This function does a median filtering of an 8-bit image. The source image is
 * processed as if it was padded with zeros. The median kernel is square with
 * odd dimensions. Images of arbitrary size may be processed.
 *
 * To process multi-channel images, you must call this function multiple times,
 * changing the source and destination adresses and steps such that each channel
 * is processed as an independent single-channel image.
 *
 * Processing images of arbitrary bit depth is not supported.
 *
 * The computing time is O(1) per pixel, independent of the radius of the
 * filter. The algorithm's initialization is O(r*width), but it is negligible.
 * Memory usage is simple: it will be as big as the cache size, or smaller if
 * the image is small. For efficiency, the histograms' bins are 16-bit wide.
 * This may become too small and lead to overflow as \a r increases.
 *
 * \param src           Source image data.
 * \param dst           Destination image data. Must be preallocated.
 * \param width         Image width, in pixels.
 * \param height        Image height, in pixels.
 * \param src_step      Distance between adjacent pixels on the same column in
 *                      the source image, in bytes.
 * \param dst_step      Distance between adjacent pixels on the same column in
 *                      the destination image, in bytes.
 * \param r             Median filter radius. The kernel will be a 2*r+1 by
 *                      2*r+1 square.
 * \param cn            Number of channels. For example, a grayscale image would
 *                      have cn=1 while an RGB image would have cn=3.
 * \param memsize       Maximum amount of memory to use, in bytes. Set this to
 *                      the size of the L2 cache, then vary it slightly and
 *                      measure the processing time to find the optimal value.
 *                      For example, a 512 kB L2 cache would have
 *                      memsize=512*1024 initially.
 */
void ctmf(
        const unsigned char* const src, unsigned char* const dst,
        const int width, const int height,
        const int src_step, const int dst_step,
        const int r, const int cn, const long unsigned int memsize
        )
{
    /*
     * Processing the image in vertical stripes is an optimization made
     * necessary by the limited size of the CPU cache. Each histogram is 544
     * bytes big and therefore I can fit a limited number of them in the cache.
     * That number may sometimes be smaller than the image width, which would be
     * the number of histograms I would need without stripes.
     *
     * I need to keep histograms in the cache so that they are available
     * quickly when processing a new row. Each row needs access to the previous
     * row's histograms. If there are too many histograms to fit in the cache,
     * thrashing to RAM happens.
     *
     * To solve this problem, I figure out the maximum number of histograms
     * that can fit in cache. From this is determined the number of stripes in
     * an image. The formulas below make the stripes all the same size and use
     * as few stripes as possible.
     *
     * Note that each stripe causes an overlap on the neighboring stripes, as
     * when mowing the lawn. That overlap is proportional to r. When the overlap
     * is a significant size in comparison with the stripe size, then we are not
     * O(1) anymore, but O(r). In fact, we have been O(r) all along, but the
     * initialization term was neglected, as it has been (and rightly so) in B.
     * Weiss, "Fast Median and Bilateral Filtering", SIGGRAPH, 2006. Processing
     * by stripes only makes that initialization term bigger.
     *
     * Also, note that the leftmost and rightmost stripes don't need overlap.
     * A flag is passed to ctmf_helper() so that it treats these cases as if the
     * image was zero-padded.
     */
    int stripes = (int) ceil( (double) (width - 2*r) / (memsize / sizeof(Histogram) - 2*r) );
    int stripe_size = (int) ceil( (double) ( width + stripes*2*r - 2*r ) / stripes );

    int i;

    for ( i = 0; i < width; i += stripe_size - 2*r ) {
        int stripe = stripe_size;
        /* Make sure that the filter kernel fits into one stripe. */
        if ( i + stripe_size - 2*r >= width || width - (i + stripe_size - 2*r) < 2*r+1 ) {
            stripe = width - i;
        }

        ctmf_helper( src + cn*i, dst + cn*i, stripe, height, src_step, dst_step, r, cn,
                i == 0, stripe == width - i );

        if ( stripe == width - i ) {
            break;
        }
    }
}

//unused for the time being:
#if 1

#ifndef SIMPLE_H
#define SIMPLE_H

// Sliding median filter
// Created 2012 by Colin Raffel
// Portions Copyright (c) 2011 ashelly.myopenid.com under <http://www.opensource.org/licenses/mit-license>
// see https://github.com/craffel/median-filter

#ifndef MEDIATOR_H
#define MEDIATOR_H

template <typename Item> class Mediator
{
public:
  
  Mediator(int nItems, bool iseven):N(nItems),even(iseven)
  {
    data = new Item[nItems];
    pos = new int[nItems];
    allocatedHeap = new int[nItems]; 
    heap = allocatedHeap + (nItems/2);
    minCt = maxCt = idx = 0;
    // Set up initial heap fill pattern: median, max, min, max, ...
    while (nItems--)
    {  
      pos[nItems] = ((nItems + 1)/2)*((nItems & 1) ? -1 : 1);
      heap[pos[nItems]] = nItems;
    }
  };
  
  ~Mediator()
  {
    delete[] data;
    delete[] pos;
    delete[] allocatedHeap;
  };
  
  // Inserts item, maintains median in O(lg nItems)
  void insert(const Item& v )
  {
    const int p = pos[idx];
    const Item old = data[idx];
    data[idx] = v;
    idx = (idx+1) % N;
    // New item is in minheap
    if (p>0)
    {  
      if (minCt < (N-1)/2)
      {
        ++minCt;
      }
      else if (v > old)
      { 
        minSortDown( p );
        return;
      }
      if (minSortUp( p ) && mmCmpExch( 0, -1 ))
      {
        maxSortDown( -1 );
      }
    }
    // New item is in maxheap
    else if (p<0)
    {  
      if (maxCt < N/2)
      {
        ++maxCt;
      }
      else if (v < old)
      {
        maxSortDown( p );
        return;
      }
      if (maxSortUp( p ) && minCt && mmCmpExch( 1, 0 ))
      {
        minSortDown( 1 );
      }
    }
    // New item is at median
    else
    {  
      if (maxCt && maxSortUp( -1 ))
      {
        maxSortDown( -1 );
      }
      if (minCt && minSortUp( 1 ))
      { 
        minSortDown( 1 );
      }
    }
  };
  
  // Returns median item (or average of 2 when item count is even)
  Item getMedian()
  {
    Item v = data[heap[0]];
    if (even && (minCt<maxCt))
    { 
      v = (v + data[heap[-1]])/2;
    }
    return v;
  };
  
private:
  
  // Swaps items i&j in heap, maintains indexes
  int mmexchange(const int i,const int j )
  {
    int t = heap[i];
    heap[i] = heap[j];
    heap[j] = t;
    pos[heap[i]] = i;
    pos[heap[j]] = j;
    return 1;
  };
  
  // Maintains minheap property for all items below i.
  void minSortDown( int i )
  {
    for (i*=2; i <= minCt; i*=2)
    {
      if (i < minCt && mmless( i+1, i ))
      {
        ++i;
      }
      if (!mmCmpExch( i, i/2 ))
      {
        break;
      }
    }
  };

  // Maintains maxheap property for all items below i. (negative indexes)
  void maxSortDown( int i )
  {
    for (i*=2; i >= -maxCt; i*=2)
    {  
      if (i > -maxCt && mmless( i, i-1 )) 
      { 
        --i;
      }
      if (!mmCmpExch( i/2, i ))
      {
        break;
      }
    }
  };
  
  // Returns 1 if heap[i] < heap[j]
  inline int mmless(const int i,const int j )
  {
    return (data[heap[i]] < data[heap[j]]);
  };
  
  // Swaps items i&j if i<j; returns true if swapped
  inline int mmCmpExch(const int i,const int j )
  {
    return (mmless( i, j ) && mmexchange( i, j ));
  };
  
  // Maintains minheap property for all items above i, including median
  // Returns true if median changed
  inline int minSortUp( int i )
  {
    while (i > 0 && mmCmpExch( i, i/2 ))
    {
      i /= 2;
    }
    return (i == 0);
  };
  
  // Maintains maxheap property for all items above i, including median
  // Returns true if median changed
  inline int maxSortUp( int i )
  {
    while (i < 0 && mmCmpExch( i/2, i ))
    {
      i /= 2;
    }
    return ( i==0 );
  }; 
  // Allocated size
  const int N;
  // even /odd correction
  const bool even;
  // Circular queue of values
  Item* data;
  // Index into `heap` for each value
  int* pos;
  // Max/median/min heap holding indexes into `data`.
  int* heap;
  // heap holds a pointer to the middle of its data; this is where the data is allocated.
  int* allocatedHeap;
  // Position in circular queue
  int idx; 
  // Count of items in min heap
  int minCt;
  // Count of items in max heap
  int maxCt;
};

#endif
void filter( double* array, int n, int filterSize, bool even=false );
void filter( float* array, int n, int filterSize, bool even=false );

#endif
void filter( double* array, int n, int filterSize , bool even)
{
  Mediator<double> mediator( filterSize, even );
  for (int i = 0; i < filterSize/2; ++i)
  {
    mediator.insert( array[0] );
    array[i] = mediator.getMedian();
  }
  int offset = filterSize/2 + (filterSize % 2);
  for (int i = 0; i < offset; ++i)
  {
    mediator.insert( array[i] );
  }
  for (int i = 0; i < n - offset; ++i)
  {
    array[i] = mediator.getMedian();
    mediator.insert( array[i + offset] );
  }
  for (int i = n - offset; i < n; ++i)
  {
    array[i] = mediator.getMedian();
    mediator.insert( array[n - 1] );
  }
}

void filter( float* array, int n, int filterSize, bool even )
{ 
  Mediator<float> mediator( filterSize, even );
  for (int i = 0; i < filterSize/2; ++i)
  {
    mediator.insert( array[0] );
    array[i] = mediator.getMedian();
  }
  int offset = filterSize/2 + (filterSize % 2);
  for (int i = 0; i < offset; ++i)
  {
    mediator.insert( array[i] );
  }
  for (int i = 0; i < n - offset; ++i)
  {
    array[i] = mediator.getMedian();
    mediator.insert( array[i + offset] );
  }
  for (int i = n - offset; i < n; ++i)
  {
    array[i] = mediator.getMedian();
    mediator.insert( array[n - 1] );
  }
}
#endif


} //namespace
