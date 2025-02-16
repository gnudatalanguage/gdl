/***************************************************************************
         value_locate.cpp  
                             -------------------
    begin                : Mar 30 2023
    copyright            : (C) 2023 G. Duvert 
    email                : see https://github.com/gnudatalanguage/gdl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "envt.hpp"

#define VALUE_LOCATE_HELPER(xxx,yyy) {\
xxx* value = e->GetParAs<xxx>(1);\
yyy* val = static_cast<yyy*> (value->DataAddr());\
yyy* array = static_cast<yyy*> (p0->DataAddr());\
DLong order = (array[nEl - 1] >= array[0]);\
DLong index = do_value_locate<yyy>(array, nEl, val[0], order);\
return new DLongGDL(index);\
break;\
}
 #define VALUE_LOCATE_HELPER_MULTI(xxx,yyy)     {\
xxx* value = e->GetParAs<xxx>(1);\
yyy* val = static_cast<yyy*> (value->DataAddr());\
yyy* array = static_cast<yyy*> (p0->DataAddr());\
DLong order = (array[nEl - 1] >= array[0]);\
for (auto i=0; i<nEl1; ++i) {\
 (*ret)[i] = do_value_locate<yyy>(array, nEl, val[i], order);\
}\
break;\
}
#define VALUE_LOCATE_HELPER64(xxx,yyy) {\
xxx* value = e->GetParAs<xxx>(1);\
yyy* val = static_cast<yyy*> (value->DataAddr());\
yyy* array = static_cast<yyy*> (p0->DataAddr());\
DLong order = (array[nEl - 1] >= array[0]);\
DLong64 index = do_value_locate64<yyy>(array, nEl, val[0], order);\
return new DLong64GDL(index);\
break;\
}
 #define VALUE_LOCATE_HELPER_MULTI64(xxx,yyy)     {\
xxx* value = e->GetParAs<xxx>(1);\
yyy* val = static_cast<yyy*> (value->DataAddr());\
yyy* array = static_cast<yyy*> (p0->DataAddr());\
DLong order = (array[nEl - 1] >= array[0]);\
for (auto i=0; i<nEl1; ++i) {\
 (*ret)[i] = do_value_locate64<yyy>(array, nEl, val[i], order);\
}\
break;\
}
template<typename T>
DLong do_value_locate(const T* array, SizeT nEl, const T value, DLong ascending_order) {
  array -= 1; //code below is for unit-offset array, i.e., fortran-like arrays
  DLong index_up, index_low, index_mid;

  index_mid = 0;
  index_up = nEl + 1;
  while (index_up - index_mid > 1) {
    index_low = (index_up + index_mid) / 2;
    if ((value >= array[index_low]) == ascending_order)
      index_mid = index_low;
    else
      index_up = index_low;
  }
  return index_mid - 1; //since work is done on unit-offset array
}

template<typename T>
DLong64 do_value_locate64(const T* array, SizeT nEl, const T value, DLong ascending_order) {
  array -= 1; //code below is for unit-offset array, i.e., fortran-like arrays
  DLong64 index_up, index_low, index_mid;

  index_mid = 0;
  index_up = nEl + 1;
  while (index_up - index_mid > 1) {
    index_low = (index_up + index_mid) / 2;
    if ((value >= array[index_low]) == ascending_order)
      index_mid = index_low;
    else
      index_up = index_low;
  }
  return index_mid - 1; //since work is done on unit-offset array
}

namespace lib {

  BaseGDL* value_locate(EnvT* e) {
    e->NParam(2);

    BaseGDL* p0 = e->GetParDefined(0);
    BaseGDL* p1 = e->GetParDefined(1);

    if (!ConvertableType(p0->Type())) e->Throw(p0->TypeStr() + " expression not allowed in this context:" + e->GetParString(0));
    if (ComplexType(p0->Type())) e->Throw(p0->TypeStr() + " expression not allowed in this context:" + e->GetParString(0));
    SizeT nEl = p0->N_Elements();
    SizeT nEl1 = p1->N_Elements();

    static int l64ix = e->KeywordIx("L64");
    bool do64 = e->KeywordPresentAndDefined(l64ix);
    if (do64) {
      if (nEl1 == 1) {
        switch (p0->Type()) {
        case GDL_BYTE:
          VALUE_LOCATE_HELPER64(DByteGDL, DByte)
        case GDL_INT:
          VALUE_LOCATE_HELPER64(DIntGDL, DInt)
        case GDL_LONG:
          VALUE_LOCATE_HELPER64(DLongGDL, DLong)
        case GDL_LONG64:
          VALUE_LOCATE_HELPER64(DLong64GDL, DLong64)
        case GDL_UINT:
          VALUE_LOCATE_HELPER64(DUIntGDL, DUInt)
        case GDL_ULONG:
          VALUE_LOCATE_HELPER64(DULongGDL, DULong)
        case GDL_ULONG64:
          VALUE_LOCATE_HELPER64(DULong64GDL, DULong64)
        case GDL_FLOAT:
          VALUE_LOCATE_HELPER64(DFloatGDL, DFloat)
        case GDL_DOUBLE:
          VALUE_LOCATE_HELPER64(DDoubleGDL, DDouble)
        case GDL_STRING:
          VALUE_LOCATE_HELPER64(DStringGDL, DString)
         default:
          assert(false);
          return NULL; //pacify
        }
      } else {

        DLong64GDL* ret = new DLong64GDL(dimension(nEl1), BaseGDL::NOZERO);

        switch (p0->Type()) {
        case GDL_BYTE:
          VALUE_LOCATE_HELPER_MULTI64(DByteGDL, DByte)
        case GDL_INT:
          VALUE_LOCATE_HELPER_MULTI64(DIntGDL, DInt)
        case GDL_LONG:
          VALUE_LOCATE_HELPER_MULTI64(DLongGDL, DLong)
        case GDL_LONG64:
          VALUE_LOCATE_HELPER_MULTI64(DLong64GDL, DLong64)
        case GDL_UINT:
          VALUE_LOCATE_HELPER_MULTI64(DUIntGDL, DUInt)
        case GDL_ULONG:
          VALUE_LOCATE_HELPER_MULTI64(DULongGDL, DULong)
        case GDL_ULONG64:
          VALUE_LOCATE_HELPER_MULTI64(DULong64GDL, DULong64)
        case GDL_FLOAT:
          VALUE_LOCATE_HELPER_MULTI64(DFloatGDL, DFloat)
        case GDL_DOUBLE:
          VALUE_LOCATE_HELPER_MULTI64(DDoubleGDL, DDouble)
        case GDL_STRING:
          VALUE_LOCATE_HELPER_MULTI64(DStringGDL, DString)
        default:
          assert(false);
          return NULL; //pacify
        }
        return ret;
      }
    } else {
      if (nEl1 == 1) {
        switch (p0->Type()) {
        case GDL_BYTE:
          VALUE_LOCATE_HELPER(DByteGDL, DByte)
        case GDL_INT:
          VALUE_LOCATE_HELPER(DIntGDL, DInt)
        case GDL_LONG:
          VALUE_LOCATE_HELPER(DLongGDL, DLong)
        case GDL_LONG64:
          VALUE_LOCATE_HELPER(DLong64GDL, DLong64)
        case GDL_UINT:
          VALUE_LOCATE_HELPER(DUIntGDL, DUInt)
        case GDL_ULONG:
          VALUE_LOCATE_HELPER(DULongGDL, DULong)
        case GDL_ULONG64:
          VALUE_LOCATE_HELPER(DULong64GDL, DULong64)
        case GDL_FLOAT:
          VALUE_LOCATE_HELPER(DFloatGDL, DFloat)
        case GDL_DOUBLE:
          VALUE_LOCATE_HELPER(DDoubleGDL, DDouble)
        case GDL_STRING:
          VALUE_LOCATE_HELPER(DStringGDL, DString)
        default:
          assert(false);
          return NULL; //pacify
        }
      } else {

        DLongGDL* ret = new DLongGDL(dimension(nEl1), BaseGDL::NOZERO);

        switch (p0->Type()) {
        case GDL_BYTE:
          VALUE_LOCATE_HELPER_MULTI(DByteGDL, DByte)
        case GDL_INT:
          VALUE_LOCATE_HELPER_MULTI(DIntGDL, DInt)
        case GDL_LONG:
          VALUE_LOCATE_HELPER_MULTI(DLongGDL, DLong)
        case GDL_LONG64:
          VALUE_LOCATE_HELPER_MULTI(DLong64GDL, DLong64)
        case GDL_UINT:
          VALUE_LOCATE_HELPER_MULTI(DUIntGDL, DUInt)
        case GDL_ULONG:
          VALUE_LOCATE_HELPER_MULTI(DULongGDL, DULong)
        case GDL_ULONG64:
          VALUE_LOCATE_HELPER_MULTI(DULong64GDL, DULong64)
        case GDL_FLOAT:
          VALUE_LOCATE_HELPER_MULTI(DFloatGDL, DFloat)
        case GDL_DOUBLE:
          VALUE_LOCATE_HELPER_MULTI(DDoubleGDL, DDouble)
        case GDL_STRING:
          VALUE_LOCATE_HELPER_MULTI(DStringGDL, DString)
        default:
          assert(false);
          return NULL; //pacify
        }
        return ret;
      }
    }
  }
}
