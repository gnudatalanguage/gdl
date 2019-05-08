/***************************************************************************
                          Shapefiles.hpp  -  all stuff for IDLffShape
                             -------------------
    begin                : March 2019
    copyright            : (C) 2019 by G.Duvert
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

namespace lib {
 BaseGDL* GDLffShape___GetAttributes( EnvUDT* e);
 BaseGDL* GDLffShape___GetEntity( EnvUDT* e);
 BaseGDL* GDLffShape___Init( EnvUDT* e);
 BaseGDL* GDLffShape___Open( EnvUDT* e);
 void GDLffShape___AddAttribute( EnvUDT* e);
 void GDLffShape___Cleanup( EnvUDT* e);
 void GDLffShape___Close( EnvUDT* e);
 void GDLffShape___DestroyEntity( EnvUDT* e);
 void GDLffShape___GetProperty( EnvUDT* e);
 void GDLffShape___PutEntity( EnvUDT* e);
 void GDLffShape___SetAttributes( EnvUDT* e);
} // namespace

