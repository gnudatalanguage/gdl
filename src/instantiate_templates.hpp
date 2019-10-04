/***************************************************************************
                          instantiate_templates.hpp  -  instantiates <Sp> templates in each of the specializations used by GDL
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
                               2018 by Gilles Duvert
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

template class Data_< SpDByte>;
template class Data_< SpDInt>;
template class Data_< SpDUInt>;
template class Data_< SpDLong>;
template class Data_< SpDULong>;
template class Data_< SpDLong64>;
template class Data_< SpDULong64>;
template class Data_< SpDPtr>;
template class Data_< SpDFloat>;
template class Data_< SpDDouble>;
template class Data_< SpDString>;
template class Data_< SpDObj>;
template class Data_< SpDComplex>;
template class Data_< SpDComplexDbl>;


