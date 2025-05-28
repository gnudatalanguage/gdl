lexer grammar gdlLexer;

tokens {
NULLSTRING,
STRING,
END_U}

DOT:'.';
AND_OP_EQ: [aA] [nN] [dD] '='; 
ASTERIX_EQ:'*=';
EQ_OP_EQ: [eE] [qQ] '=';
GE_OP_EQ:  [gG] [eE] '=';
GTMARK_EQ:'>=';
GT_OP_EQ:  [gG] [tT] '=';
LE_OP_EQ:  [lL] [eE] '=';
LTMARK_EQ:'<=';
LT_OP_EQ:  [lL] [tT] '=';
MATRIX_OP1_EQ:'#=';
MATRIX_OP2_EQ:'##=';
MINUS_EQ:'-=';
MOD_OP_EQ:  [mM] [oO] [dD] '=';
NE_OP_EQ:  [nN] [eE] '=';
OR_OP_EQ:  [oO] [rR] '=';
PLUS_EQ:'+=';
POW_EQ:'^=';
SLASH_EQ:'/=';
XOR_OP_EQ:  [xX] [oO] [rR] '=';

MATRIX_OP1:'#';
MATRIX_OP2:'##';
METHOD:'::';
MEMBER:'->';
COMMA:',';
COLON:':';
ASSIGN:'=';
LBRACE:'(';
RBRACE:')';
LCURLY:'{';
RCURLY:'}';
LSQUARE:'[';
RSQUARE:']';
QUESTION:'?';
EXCLAMATION:'!';
POW:'^';
ASTERIX:'*';
SLASH:'/';
MINUS:'-';
PLUS:'+';
INC:'++';
DEC:'--';
GTMARK:'>';
LTMARK:'<';
LOG_AND:'&&';
LOG_OR:'||';
LOG_NEG:'~';
AND_OP: [aA] [nN] [dD]; 
BEGIN:  [bB] [eE] [gG] [iI] [nN];
BREAK:  [bB] [rR] [eE] [aA] [kK];
CASE:  [cC] [aA] [sS] [eE]; 
COMMON: [cC] [oO] [mM] [mM] [oO] [nN];
COMPILE_OPT: [cC] [oO] [mM] [pP] [iI] [lL] [eE] '_'  [oO] [pP] [tT];

// tentative trap of "CONTINUE" as the real CONTINUE in the lexer.
CONTINUE:  [cC] [oO] [nN] [tT] [iI] [nN] [uU] [eE] ;


DO:  [dD] [oO];
ELSE:  [eE] [lL] [sS] [eE];
END: [eE] [nN] [dD];
ENDCASE: [eE] [nN] [dD] [cC] [aA] [sS] [eE];
ENDELSE: [eE] [nN] [dD] [eE] [lL] [sS] [eE];
ENDFOR: [eE] [nN] [dD] [fF] [oO] [rR];
ENDFOREACH: [eE] [nN] [dD] [fF] [oO] [rR] [eE] [aA] [cC] [hH];
ENDIF: [eE] [nN] [dD] [iI] [fF];
ENDREP: [eE] [nN] [dD] [rR] [eE] [pP];
ENDSWITCH: [eE] [nN] [dD] [sS] [wW] [iI] [tT] [cC] [hH];
ENDWHILE: [eE] [nN] [dD] [wW] [hH] [iI] [lL] [eE];
EQ_OP: [eE] [qQ];
FOR: [fF] [oO] [rR];
FOREACH: [fF] [oO] [rR] [eE] [aA] [cC] [hH];
FORWARD_FUNCTION: [fF] [oO] [rR] [wW] [aA] [rR] [dD] '_'  [fF] [uU] [nN] [cC] [tT] [iI] [oO] [nN];
FUNCTION: [fF] [uU] [nN] [cC] [tT] [iI] [oO] [nN];
GE_OP: [gG] [eE];
GOTO: [gG] [oO] [tT] [oO];
GT_OP: [gG] [tT];
IF: [iI] [fF];
INHERITS: [iI] [nN] [hH] [eE] [rR] [iI] [tT] [sS];
LE_OP: [lL] [eE];
LT_OP: [lL] [tT];
MOD_OP: [mM] [oO] [dD];
NE_OP: [nN] [eE];
NOT_OP: [nN] [oO] [tT];
OF: [oO] [fF];
ON_IOERROR: [oO] [nN] '_'  [iI] [oO] [eE] [rR] [rR] [oO] [rR];
OR_OP: [oO] [rR];
PRO: [pP] [rR] [oO];
REPEAT: [rR] [eE] [pP] [eE] [aA] [tT];
RETURN: [rR] [eE] [tT] [uU] [rR] [nN]; // unspecified return (replaced by tree parser with RETF/RETP)
SWITCH: [sS] [wW] [iI] [tT] [cC] [hH];
THEN: [tT] [hH] [eE] [nN];
UNTIL: [uU] [nN] [tT] [iI] [lL];
WHILE: [wW] [hH] [iI] [lL] [eE];
XOR_OP: [xX] [oO] [rR];

//fragment
EOL:  '\r'? '\n' ->type(END_U) ;

fragment
WS : [ \t\u000C] ;

fragment
INCLUDE_FILENAME    : ( ~('\r'|'\n') )*
    ;

INCLUDE
      :    '@' INCLUDE_FILENAME {std::cerr<<"please handle Include in Lexer"<<std::endl; /*assert(false);*/}-> skip
    ;

fragment
 DIGIT : [0-9] ;

fragment
LETTER
    : [a-zA-Z_]
    ;

fragment
HEXADECIMAL 
    : [a-fA-F0-9]
    ;

fragment
OCTAL
    : [0-7]
    ;
fragment
BINARY
    : [0-1]
    ;

fragment
EXP
    : ([eE] ([+\-]? ( DIGIT)+)? )
    ;

fragment
DBL
    : ([dD] ([+\-]? ( DIGIT)+)? )
    ;

CONSTANT_BIN_I: '\'' (BINARY)+ '\'' [bB];
CONSTANT_BIN_INT:     CONSTANT_BIN_I [sS];
CONSTANT_BIN_BYTE:    CONSTANT_BIN_I  ([bB]|[uU] [bB]);
CONSTANT_BIN_UI:      CONSTANT_BIN_I [uU];
CONSTANT_BIN_UINT:    CONSTANT_BIN_I  [uU] [sS];
CONSTANT_BIN_LONG:    CONSTANT_BIN_I  [lL];
CONSTANT_BIN_LONG64:  CONSTANT_BIN_I  [lL] [lL] ;
CONSTANT_BIN_ULONG:   CONSTANT_BIN_I  [uU] [lL];
CONSTANT_BIN_ULONG64: CONSTANT_BIN_I  [uU] [lL] [lL];

//hexadecimal variant 1: NOT 'b' as B is part of (HEXADECIMAL) : ex: 0x3BAFB 
CONSTANT_HEX_I:  '0' [xX] (HEXADECIMAL)+ ; // DEFINT32
CONSTANT_HEX_INT:     CONSTANT_HEX_I  [sS];
CONSTANT_HEX_UI:      CONSTANT_HEX_I  [uU];
CONSTANT_HEX_UINT:    CONSTANT_HEX_I  [uU] [sS];
CONSTANT_HEX_BYTE:    CONSTANT_HEX_I  [uU] [bB];
CONSTANT_HEX_LONG:    CONSTANT_HEX_I  [lL];
CONSTANT_HEX_LONG64:  CONSTANT_HEX_I  [lL] [lL] ;
CONSTANT_HEX_ULONG:   CONSTANT_HEX_I  [uU] [lL];
CONSTANT_HEX_ULONG64: CONSTANT_HEX_I  [uU] [lL] [lL];

VariantConstant_hex_i:  '\'' (HEXADECIMAL)+ '\'' [xX] ->type(CONSTANT_HEX_I); // DEFINT32
VariantConstant_hex_int:     VariantConstant_hex_i  [sS]->type(CONSTANT_HEX_INT);
VariantConstant_hex_ui:      VariantConstant_hex_i  [uU]->type(CONSTANT_HEX_UI);
VariantConstant_hex_uint:    VariantConstant_hex_i  [uU] [sS]->type(CONSTANT_HEX_UINT);
VariantConstant_hex_byte:    VariantConstant_hex_i  ([bB] | [uU] [bB])->type(CONSTANT_HEX_BYTE);
VariantConstant_hex_long:    VariantConstant_hex_i  [lL]->type(CONSTANT_HEX_LONG);
VariantConstant_hex_long64:  VariantConstant_hex_i  [lL] [lL]->type(CONSTANT_HEX_LONG64) ;
VariantConstant_hex_ulong:   VariantConstant_hex_i  [uU] [lL]->type(CONSTANT_HEX_ULONG);
VariantConstant_hex_ulong64: VariantConstant_hex_i  [uU] [lL] [lL]->type(CONSTANT_HEX_ULONG64);

CONSTANT_OCT_I: '\'' (OCTAL)+ '\'' [oO];
CONSTANT_OCT_INT:     CONSTANT_OCT_I [sS];
CONSTANT_OCT_BYTE:    CONSTANT_OCT_I  ([bB]|[uU] [bB]);
CONSTANT_OCT_UI:      CONSTANT_OCT_I [uU];
CONSTANT_OCT_UINT:    CONSTANT_OCT_I  [uU] [sS];
CONSTANT_OCT_LONG:    CONSTANT_OCT_I  [lL];
CONSTANT_OCT_LONG64:  CONSTANT_OCT_I  [lL] [lL] ;
CONSTANT_OCT_ULONG:   CONSTANT_OCT_I  [uU] [lL];
CONSTANT_OCT_ULONG64: CONSTANT_OCT_I  [uU] [lL] [lL];

CONSTANT_I: (DIGIT)+ ;
CONSTANT_INT:     CONSTANT_I [sS];
CONSTANT_BYTE:    CONSTANT_I  ([bB]|[uU] [bB]);
CONSTANT_UINT:    CONSTANT_I  [uU]([sS])?;
CONSTANT_LONG:    CONSTANT_I  [lL];
CONSTANT_LONG64:  CONSTANT_I  [lL] [lL] ;
CONSTANT_ULONG:   CONSTANT_I  [uU] [lL];
CONSTANT_ULONG64: CONSTANT_I  [uU] [lL] [lL];

CONSTANT_DOUBLE: 
        (
            (
                ( DIGIT)+ 
                ( DBL 
                | '.'( DIGIT)*(DBL)
                )
            ) 
        | '.'( DIGIT)+(DBL)) 
;
CONSTANT_FLOAT:
        (
            (
                ( DIGIT)+ 
                ( EXP 
                | '.'( DIGIT)*(EXP)?
                )
            ) 
        | '.'( DIGIT)+(EXP)?) 
;

Trap_null_quoted_string: ('""'|'"' '\r'?'\n') -> type(NULLSTRING);
Trap_null_single_quoted_string: ('\'\''|'\'' '\r'?'\n') -> type(NULLSTRING);
Trap_old_octal_form:  '"' OCTAL -> more, pushMode(Old_octal_notation);
Trap_quoted_string: '"' ~('0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'"' |'\r'|'\n') -> more, pushMode(Quoted_string);
Single_quoted_string:   '\'' ( '\'\''| ~('\''|'\r'|'\n') )*  ( '\'' | )   -> type(STRING) ;

UNDEFINED: ( '[' ']' | '{' '}' ); 

COMMENT: ';' ~('\r'|'\n')* ->skip;

IDENTIFIER
    : (LETTER)(LETTER| DIGIT|'$')*
    ;

SYSVARNAME
    : ('!') (LETTER| DIGIT|'$')+
    ;

STATEMENT_SEPARATOR: '&' ->type(END_U) ;

  WHITESPACE
  : (WS)+ ->skip
  ;


// this subrule eats lines to skip
// 1. comment only lines
// 2. blank lines

fragment 
SKIP_LINES
  : ( COMMENT
    | WS
    | EOL
    )*
  ;

// IDL ignores everything on the line after the $.
// Note: The '$' command in interactive mode is filtered by GDL in DInterpreter::ExecuteLine
CONT_STATEMENT: '$' (~('\r'|'\n'))* EOL SKIP_LINES -> skip;

END_OF_LINE 
  :  EOL SKIP_LINES ->type(END_U)
  ;



ANY: . { std::cerr<<"SOMETHING IS BAD IN THE ANTLR RULES OR THE SOURCE CODE!"<<std::endl; assert(false);} ->skip; //catchall meaning something was not correctly parsed by the other rules.

// just to know how many tokens are there
fragment
MAX_TOKEN_NUMBER :;

mode Old_octal_notation;
Old_constant_oct_i:          (OCTAL)* ->type(CONSTANT_OCT_I), popMode;
Old_constant_oct_int:        Old_constant_oct_i [sS] ->type(CONSTANT_OCT_INT), popMode;
Old_constant_oct_byte:       Old_constant_oct_i ([bB]|[uU] [bB]) ->type(CONSTANT_OCT_BYTE), popMode;
Old_constant_oct_ui:         Old_constant_oct_i [uU] ->type(CONSTANT_OCT_UI), popMode;
Old_constant_oct_uint:       Old_constant_oct_i [uU] [sS] ->type(CONSTANT_OCT_UINT), popMode;
Old_constant_oct_long:       Old_constant_oct_i [lL] ->type(CONSTANT_OCT_LONG), popMode;
Old_constant_oct_long64:     Old_constant_oct_i [lL] [lL] ->type(CONSTANT_OCT_LONG64), popMode ;
Old_constant_oct_ulong:      Old_constant_oct_i [uU] [lL] ->type(CONSTANT_OCT_ULONG), popMode;
Old_constant_oct_ulong64:    Old_constant_oct_i [uU] [lL] [lL] ->type(CONSTANT_OCT_ULONG64), popMode;
Old_constant_oct_is_string:  Old_constant_oct_i '"' ->type(STRING), popMode;

mode Quoted_string;
IsAQuotedString: ( '""' | ~('"'|'\r'|'\n') )* ('"'|) ->type(STRING), popMode;


