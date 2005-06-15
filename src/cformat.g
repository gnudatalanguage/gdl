/* *************************************************************************
                          cformat.g  -  parser for GDL format strings
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

header "pre_include_cpp" {
#include "includefirst.hpp"
}

header {
#include "antlr/TokenStreamSelector.hpp"
    
//    using namespace antlr;
}

options {
	language="Cpp";
	genHashLines = false;
	namespaceStd="std";         // cosmetic option to get rid of long defines
	namespaceAntlr="antlr";     // cosmetic option to get rid of long defines
}	

// the C-Format Lexer *********************************************
class CFMTLexer extends Lexer;

options {
	charVocabulary = '\3'..'\377';
	caseSensitive=true ;
	testLiterals =true;
	caseSensitiveLiterals=false;
	exportVocab=CFMT;
	k=2;
    defaultErrorHandler = false;
//    defaultErrorHandler = true;
}

{
    private:
    antlr::TokenStreamSelector*  selector; 
    bool                  doubleQuotes;
    bool                  format;

    public:
    void SetSelector( antlr::TokenStreamSelector& s)
    {
        selector = &s;
    }
    void DoubleQuotes( bool dQ)
    {
        doubleQuotes = dQ; format=false;
    }
    
}

ALL
    : { format}?
        (
            (
                (
                    CD { $setType(CD);}
                |   CE { $setType(CE);}
                |   CI { $setType(CI);}
                |   CF { $setType(CF);}
                |   CG { $setType(CG);}
                |   CO { $setType(CO);}
                |   CS { $setType(CS);}
                |   CX { $setType(CX);}
                |   CZ { $setType(CZ);}
                ) 
                { format = false;}
            ) 
        |   CNUMBER { $setType(CNUMBER);}
        |   CDOT { $setType(CDOT);}
        |   CWS  { _ttype=antlr::Token::SKIP; }
        )
    | CSTR { $setType(CSTR);}
    ;

protected
CSTR
    :
        { doubleQuotes}? 
        (
            CSTR1
            ( '\"'! { selector->pop();} 
            | '%'!  { format = true;}
            )
        )
    |             
        (
            CSTR2
            ( '\''! { selector->pop();} 
            | '%'!  { format = true;}
            )
        )
    ;

protected
CSTR1
    : ( '%'! '%' | ESC | ~('%' | '\"' | '\\'))*
    ;

protected
CSTR2
    : ( '%'! '%' | ESC | ~('%' | '\'' | '\\'))*
    ;

protected
ESC
    : '\\'!
        ( ('a'|'A')! { $setText( "\7");}
        | ('b'|'B')! { $setText( "\b");}
        | ('f'|'F')! { $setText( "\f");}
        | ('n'|'N')! { $setText( "\n");}
        | ('r'|'R')! { $setText( "\r");}
        | ('t'|'T')! { $setText( "\t");}
        | ('v'|'V')! { $setText( "\13");}
        | OCTESC 
        | ('x'|'X')! HEXESC
        | ~('a'|'A'|'b'|'B'|'f'|'F'|'n'|'N'|'r'|'R'|'t'|'T'|'v'|'V'|
                '0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'x'|'X') // just insert
        )            
    ;


protected
OCTESC
    :  ODIGIT (options {greedy=true;}: ODIGIT (options {greedy=true;}: ODIGIT)?)?
        {
            std::string s = $getText;
            char c = static_cast<char>(strtoul(s.c_str(),NULL,8));
            $setText( c);
        }
    ;

protected
ODIGIT: ('0'..'7');

protected
HEXESC
    :  HDIGIT (options {greedy=true;}: HDIGIT)?
        {
            std::string s = $getText;
            char c = static_cast<char>(strtoul(s.c_str(),NULL,16));
            $setText( c);
        }
    ;

protected
HDIGIT: ('0'..'9'|'a'..'f'|'A'..'F');

protected
CD: ('d'|'D');
protected
CE: ('e'|'E'); 
protected
CI: ('i'|'I');
protected
CF: ('f'|'F');
protected
CG: ('g'|'G');
protected
CO: ('o'|'O');
protected
CS: ('s'|'S');
protected
CX: ('x'|'X');
protected
CZ: ('z'|'Z');

protected
CDOT:'.';

protected
DIGITS
	: ('0'..'9')+
	;

protected
CNUMBER
    : DIGITS
    ;

protected     
CWS
	: (' '| '\t')+ 
	;

