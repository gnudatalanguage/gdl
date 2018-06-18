/***************************************************************************
                          datatypes_minmax.cpp  -  for GDL datatypes
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
                         :     2018 by Gilles Duvert
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

#ifdef _OPENMP
#include <omp.h>
#endif

#include "nullgdl.hpp"
#include "dstructgdl.hpp"

template<>
void Data_<SpDObj>::MinMax( DLong* minE, DLong* maxE, 
			 BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
             SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs)
{
  throw GDLException("Object expression not allowed in this context.");
}
template<>
void Data_<SpDPtr>::MinMax( DLong* minE, DLong* maxE, 
			 BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
             SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs)
{
  throw GDLException("Pointer expression not allowed in this context.");
}
template<>
void Data_<SpDULong64>::MinMax( DLong* minE, DLong* maxE, 
			BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
            SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs) {
//#define ABSFUNC llabs //NO as comparison is always ABS for UNSIGNEDs!
#include "minmax_include.cpp"
//#undef ABSFUNC
}
template<>
void Data_<SpDLong64>::MinMax( DLong* minE, DLong* maxE, 
			BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
            SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs) {
#define ABSFUNC llabs
#include "minmax_include.cpp"
#undef ABSFUNC
}
template<>
void Data_<SpDUInt>::MinMax( DLong* minE, DLong* maxE, 
			BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
            SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs) {
//#define ABSFUNC llabs //NO as comparison is always ABS for UNSIGNEDs!
#include "minmax_include.cpp"
//#undef ABSFUNC
}
template<>
void Data_<SpDInt>::MinMax( DLong* minE, DLong* maxE, 
			BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
            SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs) {
#define ABSFUNC llabs
#include "minmax_include.cpp"
#undef ABSFUNC
}
template<>
void Data_<SpDULong>::MinMax( DLong* minE, DLong* maxE, 
			BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
            SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs) {
//#define ABSFUNC llabs
#include "minmax_include.cpp"
//#undef ABSFUNC
}
template<>
void Data_<SpDLong>::MinMax( DLong* minE, DLong* maxE, 
			BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
            SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs) {
#define ABSFUNC llabs
#include "minmax_include.cpp"
#undef ABSFUNC
}

template<>
void Data_<SpDByte>::MinMax( DLong* minE, DLong* maxE, 
			BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
            SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs) {
//#define ABSFUNC llabs
#include "minmax_include.cpp"
//#undef ABSFUNC
}
//string is identical, has no absfunc.
template<>
void Data_<SpDString>::MinMax( DLong* minE, DLong* maxE, 
			       BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
                   SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs)
{
#include "minmax_include.cpp"
}


// float etc. /nan is taken into account.
// IDL treats +Inf and -Inf as normal (ordered) values unless /NAN is present. This complicates things.
// Cases below are sped up by duplicating the code: NAN case and not.

template<>
void Data_<SpDFloat>::MinMax(DLong* minE, DLong* maxE,
  BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
  SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs) {
#define ABSFUNC fabsf
  if (omitNaN)
  {
#define MINMAX_HAS_OMITNAN
#include "minmax_include.cpp"
#undef MINMAX_HAS_OMITNAN
  } else
  {
#include "minmax_include.cpp"
  }
#undef ABSFUNC
}

template<>
void Data_<SpDDouble>::MinMax( DLong* minE, DLong* maxE, 
			       BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
                   SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs) {
#define ABSFUNC fabs
  if (omitNaN)
  {
#define MINMAX_HAS_OMITNAN
#include "minmax_include.cpp"
#undef MINMAX_HAS_OMITNAN
  } else
  {
#include "minmax_include.cpp"
  }
#undef ABSFUNC
}

//special COMPLEX treatment, adds '.real()' where needed.
#define MINMAX_IS_COMPLEX
#define ABSFUNC std::abs
template<>
void Data_<SpDComplex>::MinMax( DLong* minE, DLong* maxE, 
				BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN, 
                SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs)
{
  if (omitNaN)
  {
#define MINMAX_HAS_OMITNAN
#include "minmax_include.cpp"
#undef MINMAX_HAS_OMITNAN
  } else
  {
#include "minmax_include.cpp"
  }
}

template<>
void Data_<SpDComplexDbl>::MinMax( DLong* minE, DLong* maxE, 
				   BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
                   SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs)
{
  if (omitNaN)
  {
#define MINMAX_HAS_OMITNAN
#include "minmax_include.cpp"
#undef MINMAX_HAS_OMITNAN
  } else
  {
#include "minmax_include.cpp"
  }
}
#undef ABSFUNC
#undef MINMAX_IS_COMPLEX
