/***************************************************************************
                          gdlpython.hpp  -  python embedding
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
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

#ifndef GDLPYTHON_HPP_
#define GDLPYTHON_HPP_

void PythonInit();
//void PythonEnd();

namespace lib {

  BaseGDL* gdlpython_fun( EnvT* e);
  void     gdlpython_pro( EnvT* e);
}

#endif
