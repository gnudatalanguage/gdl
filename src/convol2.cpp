/***************************************************************************
                          convol2.cpp  -  convol(), defines some datatypes
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

#include "nullgdl.hpp"
#include "dstructgdl.hpp"
#include "dinterpreter.hpp"

template<typename T>
inline bool gdlValid( const T &value )
{
    T max_value = std::numeric_limits<T>::max();
    T min_value = - max_value;
    return ( ( min_value <= value && value <= max_value ) &&  (value == value));
}
inline bool gdlValid( const DComplex &value )
{
    DFloat max_value = std::numeric_limits<DFloat>::max();
    DFloat min_value = - max_value;
    return ( ( min_value <= value.real() && value.real() <= max_value ) &&  (value.real() == value.real()))&&
            ( ( min_value <= value.imag() && value.imag() <= max_value ) && (value.imag() == value.imag()));
}
inline bool gdlValid( const DComplexDbl &value )
{
    DDouble max_value = std::numeric_limits<DDouble>::max();
    DDouble min_value = - max_value;
    return ( ( min_value <= value.real() && value.real() <= max_value ) &&  (value.real() == value.real()))&&
            ( ( min_value <= value.imag() && value.imag() <= max_value ) &&  (value.imag() == value.imag()));
}

#define INCLUDE_CONVOL_CPP 1

#define Sp SpDLong64
#define Ty DLong64
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#define Sp SpDLong
#define Ty DLong
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#define Sp SpDFloat
#define Ty DFloat
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#define Sp SpDDouble
#define Ty DDouble
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#define Sp SpDComplex
#define Ty DComplex
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#define Sp SpDComplexDbl
#define Ty DComplexDbl
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#undef INCLUDE_CONVOL_CPP

