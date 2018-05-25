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
DLong* Data_<Sp>::Where( bool comp, SizeT& n)
{
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if( comp)
    {
      SizeT nIx = nEl;
      //  TRACEOMP( __FILE__, __LINE__)
      //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( (*this)[i] != 0)
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    } // omp
    }
  else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
    for( SizeT i=0; i<nEl; ++i)
      if( (*this)[i] != 0)
	{
	  ixList[ count++] = i;
	}
    } // omp
  n = count;
  return ixList;
}
