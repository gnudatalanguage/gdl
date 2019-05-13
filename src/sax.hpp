/***************************************************************************
                          |FILENAME|  -  description
                             -------------------
    begin                : |DATE|
    copyright            : (C) |YEAR| by |AUTHOR|
    email                : |EMAIL|
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
 BaseGDL* GDLffXmlSax___Init( EnvUDT* e);
 void GDLffXmlSax___Cleanup( EnvUDT* e);

void GDLffXmlSax__ParseFile( EnvUDT* e);

void GDLffXmlSax__AttributeDecl( EnvUDT* e);
void GDLffXmlSax__Characters( EnvUDT* e);
void GDLffXmlSax__Cleanup( EnvUDT* e);
void GDLffXmlSax__Comment( EnvUDT* e);
void GDLffXmlSax__ElementDecl( EnvUDT* e);
void GDLffXmlSax__EndCDATA( EnvUDT* e);
void GDLffXmlSax__EndDocument( EnvUDT* e);
void GDLffXmlSax__EndDTD( EnvUDT* e);
void GDLffXmlSax__EndElement( EnvUDT* e);
void GDLffXmlSax__EndEntity( EnvUDT* e);
void GDLffXmlSax__EndPrefixMapping( EnvUDT* e);
void GDLffXmlSax__Error( EnvUDT* e);
void GDLffXmlSax__ExternalEntityDecl( EnvUDT* e);
void GDLffXmlSax__FatalError( EnvUDT* e);
void GDLffXmlSax__GetProperty( EnvUDT* e);
void GDLffXmlSax__IgnorableWhitespace( EnvUDT* e);
void GDLffXmlSax__InternalEntityDecl( EnvUDT* e);
void GDLffXmlSax__NotationDecl( EnvUDT* e);
void GDLffXmlSax__ProcessingInstruction( EnvUDT* e);
void GDLffXmlSax__SetProperty( EnvUDT* e);
void GDLffXmlSax__SkippedEntity( EnvUDT* e);
void GDLffXmlSax__StartCDATA( EnvUDT* e);
void GDLffXmlSax__StartDocument( EnvUDT* e);
void GDLffXmlSax__StartDTD( EnvUDT* e);
void GDLffXmlSax__StartElement( EnvUDT* e);
void GDLffXmlSax__StartEntity( EnvUDT* e);
void GDLffXmlSax__StartPrefixmapping( EnvUDT* e);
void GDLffXmlSax__StopParsing( EnvUDT* e);
void GDLffXmlSax__UnparsedEntityDecl( EnvUDT* e);
void GDLffXmlSax__Warning( EnvUDT* e);
} // namespace

