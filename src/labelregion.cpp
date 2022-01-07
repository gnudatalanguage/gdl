/***************************************************************************
                          labelregion.cpp  -  label_region() funciton
                             -------------------
    begin                : January 4 2021
    copyright            : (C) 2021 G. Duvert
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

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

  BaseGDL* labelregion(EnvT* e) {
    e->NParam(1);

    DByteGDL* p0 = e->GetParAs<DByteGDL>(0);

    if (p0->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(0));

    if (p0->Rank() != 1 && p0->Rank() != 2) e->Throw("Only 1 or 2 dimensions allowed: " + e->GetParString(0));

    if (p0->Type() == GDL_STRUCT) e->Throw("STRUCT expression not allowed in this context: " + e->GetParString(0));

    int n, m;
    if (p0->Rank() == 1) {
      n = p0->N_Elements();
      m = 1;
    } else {
      n = p0->Dim(0);
      m = p0->Dim(1);
    }

    DByte* data = static_cast<DByte*> (p0->DataAddr());
    
    static int ALL=e->KeywordIx("ALL_NEIGHBORS");
    bool doAll=e->KeywordSet(ALL);
    static int ULONG=e->KeywordIx("ULONG");
    bool doUlong=e->KeywordSet(ULONG);
    bool overflow=false;
    DULongGDL*res = new DULongGDL(p0->Dim(), BaseGDL::ZERO);
    DULong* labels = static_cast<DULong*> (res->DataAddr());
    std::vector<DULong>numbers; numbers.push_back(0);
    if (doAll) {
      //we follow loosely https://github.com/spwhitt/cclabel implementation & naming, but region labeling goes a bit farther than that.
      // Pixel names were chosen as shown:
      // 
      //   -------------
      //   | a | b | c |
      //   -------------
      //   | d | e |   |
      //   -------------
      //   |   |   |   |
      //   -------------
      // 
      // The current pixel is e
      // a, b, c, and d are its neighbors of interest
      DULong index = 0;
      DByte dataa, datab, datac, datad;
      //avoid 1 pix boundaries
      for (auto y = 1; y < m - 1; ++y) {
        for (auto x = 1; x < n - 1; ++x) {
          SizeT e = y * n + x;
          //skip 0 values
          if (data[e] == 0) continue;
          //as we index 1-dim in a 2-dim array, some precaution has to be taken with boundaries, since IDL (wisely as always) makes 1 pixel border with
          // 0 value, i.e., region labelling is computed in the image minus the border. if the first pixel to be not 0 is background, then background will be region 1.
          // Apparently this is what the doc counsels to do, insuring the pixel[1,1] is of the 'background' value. Well, whatever.
          SizeT b = e - n;
          if (y == 1) datab = 0;
          else datab = data[b];
          SizeT a = b - 1;
          if (x == 1 || y == 1) dataa = 0;
          else dataa = data[a];
          SizeT c = b + 1;
          if (x == n - 2 || y == 1) datac = 0;
          else datac = data[c];
          SizeT d = e - 1;
          if (x == 1) datad = 0;
          else datad = data[d];
          if (datab != 0) {
            // If pixel b is not 0:
            //   a, d, and c are its neighbors, so they are all part of the same component
            //   Therefore, there is no reason to check their labels
            //   so simply assign b's label to e
            labels[e] = labels[b];
          } else if (datac != 0) {
            // If pixel c is no 0:
            //    b is its neighbor, but a and d are not
            //    Therefore, we must check a and d's label
            DULong cval = labels[c];
            DULong minval = cval;
            DULong replacedval = cval;
            if (dataa != 0) {
              // If pixel a is not 0:
              //    Then a and c are connected through e
              //    Therefore, we must union their sets, i.e., put a common value in all data having 'c's and 'a's value. the smaller of two.
              DULong aval = labels[a];
              if (aval < minval) {
                minval = aval;
              } else replacedval = aval;
            } else if (datad != 0) {
              // If pixel d is not 0:
              //    Then d and c are connected through e
              //    Therefore we must union their sets, i.e., put a common value in all data having 'c's and 'd's value. the smaller of two.
              DULong dval = labels[d];
              if (dval < minval) {
                minval = dval;
              } else replacedval = dval;
            }
            if (replacedval != minval) {
              for (SizeT i = 0; i < e; ++i) if (labels[i] == replacedval) labels[i] = minval;
              //'replacedval' has disappeared, note it.
              numbers[replacedval] = 0;
            }
            labels[e] = minval;
          } else if (dataa != 0) {
            // If pixel a is to be considered:
            //    We already know b and c are 0
            //    d is a's neighbor, so they already have the same label
            //    So simply assign a's label to e
            labels[e] = labels[a];
          } else if (datad != 0) {
            // If pixel d is to be considered:
            //    We already know a, b, and c are 0
            //    so simply assign d's label to e
            labels[e] = labels[d];
          } else {
            //  All the neighboring pixels are 0,
            // Therefore the current pixel is a new component
            labels[e] = ++index; //start at 1
            numbers.push_back(index); //memorize numbers
          }
        }
      }
    } else {
      // 
      //   -------------
      //   |   | b |   |
      //   -------------
      //   | d | e |   |
      //   -------------
      //   |   |   |   |
      //   -------------
      // 
      // The current pixel is e
      // b and d are its neighbors of interest
      DULong index = 0;
      DByte datab, datad;
      //avoid 1 pix boundaries
      for (auto y = 1; y < m - 1; ++y) {
        for (auto x = 1; x < n - 1; ++x) {
          SizeT e = y * n + x;
          //skip 0 values
          if (data[e] == 0) continue;
          //as we index 1-dim in a 2-dim array, some precaution has to be taken with boundaries, since IDL (wisely as always) makes 1 pixel border with
          // 0 value, i.e., region labelling is computed in the image minus the border. if the first pixel to be not 0 is background, then background will be region 1.
          // Apparently this is what the doc counsels to do, insuring the pixel[1,1] is of the 'background' value. Well, whatever.
          SizeT b = e - n;
          if (y == 1) datab = 0;
          else datab = data[b];
          SizeT d = e - 1;
          if (x == 1) datad = 0;
          else datad = data[d];
          if (datab != 0) {
            // If pixel b is no 0:
            //    e is its neighbor, but d is not
            //    Therefore, we must check d's label
            DULong bval = labels[b];
            DULong minval = bval;
            DULong replacedval = bval;
            if (datad != 0) {
              // If pixel d is not 0:
              //    Then d and b are connected through e
              //    Therefore we must union their sets, i.e., put a common value in all data having 'b's and 'd's value. the smaller of two.
              DULong dval = labels[d];
              if (dval < minval) {
                minval = dval;
              } else replacedval = dval;
            }
            if (replacedval != minval) {
              for (SizeT i = 0; i < e; ++i) if (labels[i] == replacedval) labels[i] = minval;
              //'replacedval' has disappeared, note it.
              numbers[replacedval] = 0;
            }
            labels[e] = minval;
          } else if (datad != 0) {
            // If pixel d is to be considered:
            //    We already know b is 0
            //    so simply assign d's label to e
            labels[e] = labels[d];
          } else {
            //  All the neighboring pixels are 0,
            // Therefore the current pixel is a new component
            labels[e] = ++index; //start at 1
            numbers.push_back(index); //memorize numbers
          }
        }
      }
    }
    //renumerotate numbers
    DULong k=0;
    for (auto i=0; i<numbers.size(); ++i) {if (numbers[i] > 0) numbers[i]=++k;}
    //2nd pass, rename labels with continuous values
    for (auto i = 0; i < n*m; ++i) labels[i]=numbers[labels[i]];
    
    if (!doUlong && !overflow) return res->Convert2(GDL_UINT,BaseGDL::CONVERT);
      
    return res;
  }

} // namespace



