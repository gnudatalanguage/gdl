parser grammar gdlParser;
options {
        tokenVocab = gdlLexer;
}
// following are in conjunction with main() and used to present a relatively correct parsing
// in absence of proper management of variables that interefere with  the parsing ( *_insideLoop() and IsRelaxed())
// *_insideLoop() is probably well treated. STRICTARR cannot be enforced at this stage.
@parser::context {
extern bool amIRelaxed;
static bool IsRelaxed() {return amIRelaxed;}
static void setRelaxed(bool b){amIRelaxed=b;}
}
@parser::declarations{
int inloop_include;
int inloop_break;
bool interactiveMode=false;
bool include_insideLoop(){return ( inloop_include > 0 );}
bool break_insideLoop(){return ( inloop_break > 0 );}
bool isInteractive(){return interactiveMode;}
void setInteractive(){interactiveMode=true;}
void unSetInteractive(){interactiveMode=false;}
}
// Actual grammar start.
tokens {
    ALL,        // arrayindex (*, e.g. [1:*])
//    ASSIGN, //already defined
    ASSIGN_INPLACE,
    ASSIGN_REPLACE,
    ASSIGN_ARRAYEXPR_MFCALL,
    ARRAYDEF,
    ARRAYDEF_CONST,
    ARRAYDEF_GENERALIZED_INDGEN,
    ARRAYIX,
    ARRAYIX_ALL,
    ARRAYIX_ORANGE,
    ARRAYIX_RANGE,
    ARRAYIX_ORANGE_S, // with stride
    ARRAYIX_RANGE_S,
    ARRAYEXPR,
    ARRAYEXPR_FCALL,
    ARRAYEXPR_MFCALL,
    BLOCK,
//    BREAK, //already defined
    CSBLOCK,
//    CONTINUE, //already defined
    COMMONDECL,
    COMMONDEF,
    CONSTANT,
    DEREF,
    ELSEBLK,
    EXPR,
//    FOR, //already defined
    FOR_STEP, // for with step
//    FOREACH, //already defined
    FOREACH_INDEX, // foreach with index (hash) variable
    FOR_LOOP,
    FOR_STEP_LOOP, // for with step
    FOREACH_LOOP,
    FOREACH_INDEX_LOOP,
    FCALL,
    FCALL_LIB, // library function call
     FCALL_LIB_DIRECT, // direct call
     FCALL_LIB_N_ELEMENTS, // N_ELEMENTS
    FCALL_LIB_RETNEW, // library function call always return newly allocated data
    GDLNULL,
    IF_ELSE,
    KEYDECL,
    KEYDEF,
    KEYDEF_REF, // keyword passed by reference
    KEYDEF_REF_CHECK, // keyword maybe passed by reference
    KEYDEF_REF_EXPR,  // keyword with assign/inc/dec passed by reference
      LABEL,
    MPCALL,
    MPCALL_PARENT, // explicit call to parent 
    MFCALL,
    MFCALL_LIB,
    MFCALL_LIB_RETNEW,
    MFCALL_PARENT, // explicit call to parent
    MFCALL_PARENT_LIB,
    MFCALL_PARENT_LIB_RETNEW,
      NOP,     // no operation
    NSTRUC,     // named struct
    NSTRUC_REF, // named struct reference
    ON_IOERROR_NULL,
    PCALL,
    PCALL_LIB, // library procedure call
    PARADECL,
    PARAEXPR,  // parameter
    PARAEXPR_VN, // _VN Variable Number of parameters version
    DEC_REF_CHECK, // called from EvalRefCheck() (no temporary needed then)
    INC_REF_CHECK, // called from EvalRefCheck() (no temporary needed then)
    POSTDEC,  //post-decrement : i--
    POSTINC, // Post-increment : i++
    DECSTATEMENT, // as a statement
    INCSTATEMENT, // as a statement
    REF,        // expr pass by reference
    REF_VN,        // expr pass by reference
    REF_CHECK,  // expr maybe be passed by reference
    REF_CHECK_VN,  // expr maybe be passed by reference
    REF_EXPR,   // assign/dec/inc expr passed by reference
    REF_EXPR_VN,   // assign/dec/inc expr passed by reference
//    REPEAT, //already defined
    REPEAT_LOOP,
//    RETURN,  //already defined
      RETF,    // return from function (return argument)
      RETP,    // return from procedure (no return argument)
    STRUC,  // struct
    SYSVAR,
//    UPLUS,
    UMINUS,
    VAR,     // variable, referenced through index
    VARPTR  //,  // variable, referenced through pointer
//    WHILE // unspecified return (replaced by tree parser with RETF/RETP)
}

 // 'reverse' identifier
 // allows reserved words as identifiers
 // needed for keyword abbreviations
 // if you change some keywords here you probably need to change
 // the reserved word list above
 reservedWords
     : AND_OP 
     | BEGIN  
     | CASE 
     | COMMON 
     | COMPILE_OPT
     | CONTINUE
     | DO 
     | ELSE 
     | END 
     | ENDCASE 
     | ENDELSE 
     | ENDFOR 
     | ENDFOREACH 
     | ENDIF 
     | ENDREP 
     | ENDSWITCH 
     | ENDWHILE 
     | EQ_OP 
     | FOR 
     | FOREACH 
     | FORWARD_FUNCTION
     | FUNCTION 
     | GE_OP 
     | GOTO 
     | GT_OP 
     | IF 
     | INHERITS 
     | LE_OP 
     | LT_OP 
     | MOD_OP 
     | NE_OP 
     | NOT_OP 
     | OF 
     | ON_IOERROR
     | OR_OP 
     | PRO 
     | REPEAT 
     | SWITCH 
     | THEN 
     | UNTIL 
     | WHILE 
     | XOR_OP 
     ;


validIdentifier:
       IDENTIFIER
     | CONTINUE
     | INHERITS
     | BREAK
     ;

// whereever one END_U is there might be more
// end_unit is syntactical necessary, but not for the AST
endUnit:   (END_U)+ //(END_OF_LINE)+
	 ;
includeFileStatement: INCLUDE;

translation_unit
   :    ( endUnit
   	| forwardFunction endUnit
        | procedureDefinition
        | functionDefinition
        | commonBlock
        )* // optional - only main program is also ok
        (  statementList END (endUnit)? )? // $MAIN$ program
        (EOF)  // braces necessary because goto crosses initialization otherwise
{ bailOut:;} // bailout jump label
    ;
//    catch[...] { /* catching translation_unit errors here */ }

// interactive compilation is not allowed
warnInteractiveCompile
    : (FUNCTION | PRO)      validIdentifier
       {
           /* throw GDLException( "Programs can't be compiled from "
                "single statement mode."); */
        }
	(METHOD validIdentifier)?      (COMMA keywordDeclaration)?    endUnit
    ;

// interactive usage: analyze one line = statement or statementList with '&'. Must catch any PRO or FUNCTION definition as this is not yet programmed.
interactive
@init{setInteractive();}
@after{unSetInteractive();}
    :   (  endUnit (anyEndMark)?
    	| warnInteractiveCompile
        | interactiveStatement
        )+
    ;
//    catch[...] { /* catching interactive errors here */ }


identifierList : listOfIdentifiers+=validIdentifier (COMMA listOfIdentifiers+=validIdentifier)*    ; //create std::vector<antlr4::Token *> listOfIdentifiers to hold the tokens.




statement:                
      conditionalStatement
    | loopStatement       
    | jumpStatement       
    | forwardFunction     
    | commonBlock         
    | compileOpt          
    | procedureCall       
    | assignmentStatement 
    | includeFileStatement
    ;

conditionalStatement
    : ifStatement
    | caseStatement
    | switchStatement
    ;

loopStatement
    : forStatement
    | foreachStatement
    | repeatStatement
    | whileStatement
    ;

jumpStatement
   :
   (gotoStatement
   | onIoErrorStatement
   | RETURN (COMMA expression)?
   )
   ;

forwardFunction : FORWARD_FUNCTION identifierList;

compileOpt : COMPILE_OPT identifierList;

commonBlock : COMMON validIdentifier  (COMMA identifierList)?  //should use a semantic predicate to say "% Common block ZZZZ must contain variables." if ZZZ is not defined
    ;

anyEndMark
    : END
    | ENDIF
    | ENDELSE
    | ENDCASE
    | ENDSWITCH
    | ENDFOR
    | ENDFOREACH
    | ENDWHILE
    | ENDREP
    ;

// idl allows more than one ELSE: first is executed, *all*
// (including expression) later branches are ignored (case) or 
// executed (switch)

switchSelector: expression;   
switchStatement: SWITCH switchSelector OF (endUnit)? (switchBody )*  ( ENDSWITCH | END); 

switchClause: expression;
switchBody
    : switchClause COLON 
        ( labelledStatement
	| statement
	| BEGIN statementList  ( ENDSWITCH | END) 
	)? endUnit
    | ELSE COLON 
        ( labelledStatement
	| statement
	| BEGIN statementList ( ENDSWITCH | ENDELSE | END)
	)? endUnit
    ;    

caseSelector: expression ;
caseStatement    : CASE caseSelector OF (endUnit)?  (caseBody)*  (ENDCASE | END)    ;

caseClause: expression;
caseBody
    : caseClause COLON 
        ( labelledStatement
	| statement
	| BEGIN statementList (ENDCASE | END))? endUnit
    | ELSE COLON 
        ( labelledStatement
	| statement
	| BEGIN statementList (ENDCASE | ENDELSE | END))? endUnit
	;

// compound statements don't care about the specific anyEndMark
compoundStatement
    : BEGIN statementList anyEndMark
    | statement
    ;
    
label:  validIdentifier COLON;

labelledStatement: (label)+  (compoundStatement)?;

statementList
    :  ( endUnit
       | compoundStatement endUnit
       | labelledStatement endUnit
       )+
       ;

compoundAssignment:
      AND_OP_EQ 
    | ASTERIX_EQ 
    | EQ_OP_EQ 
    | GE_OP_EQ
    | GTMARK_EQ
    | GT_OP_EQ
    | LE_OP_EQ
    | LTMARK_EQ
    | LT_OP_EQ
    | MATRIX_OP1_EQ
    | MATRIX_OP2_EQ
    | MINUS_EQ
    | MOD_OP_EQ
    | NE_OP_EQ
    | OR_OP_EQ
    | XOR_OP_EQ
    | PLUS_EQ
    | POW_EQ
    | SLASH_EQ
    ;

assignmentOperator:
     ASSIGN
    | compoundAssignment
    ;
    
repeatExpression: expression;
repeatStatement
@init{ inloop_include ++; inloop_break ++;}
@after{ inloop_include--; inloop_break --;}
: REPEAT repeatBlock UNTIL repeatExpression ;


repeatBlock
@init{ inloop_break ++;}
@after{ inloop_break --;}
    : BEGIN statementList (ENDREP | END)
    | statement
    ;

whileExpression: expression;
whileStatement
@init{ inloop_include ++; inloop_break ++;}
@after{ inloop_include--; inloop_break --;}
    : WHILE whileExpression DO whileBlock;


whileBlock
@init{ inloop_break ++;}
@after{ inloop_break --;}
    : BEGIN statementList (ENDWHILE | END) 
    | statement
    ;

forVariable: variableName;
forInit: expression;
forLimit: expression;
forStep: expression;
forStatement: FOR forVariable ASSIGN forInit COMMA forLimit (COMMA forStep)? DO (
            {isInteractive()}? BEGIN statement
	    | forBlock
	    )
	    ;

forBlock
@init{ inloop_include ++; inloop_break ++;}
@after{ inloop_include--; inloop_break --;}
    : BEGIN statementList ( ENDFOR | END )
    | statement
    ;    

foreachElement: validIdentifier;
foreachVariable: expression;
foreachIndex: validIdentifier;
foreachStatement
@init{ inloop_include ++; inloop_break ++;}
@after{ inloop_include--; inloop_break --;}
: FOREACH foreachElement COMMA foreachVariable (COMMA foreachIndex)? DO (
            {isInteractive()}? BEGIN statement
	    | foreachBlock
	    )
	    ;

foreachBlock
    : BEGIN statementList (END | ENDFOREACH )
    | statement
    ;    


gotoStatement:  GOTO COMMA validIdentifier;
onIoErrorStatement: ON_IOERROR COMMA validIdentifier;

ifExpression: expression;
ifStatement: IF ifExpression THEN  ifBlock ( ELSE elseBlock )? ;


ifBlock
    : BEGIN statementList (ENDIF | END) 
    | statement
    ;


elseBlock
    : BEGIN statementList ( ENDELSE | END)
    | statement
    ;

valuedParameter    :<assoc=right> (validIdentifier | reservedWords) ASSIGN expression;
setParameter: SLASH (validIdentifier | reservedWords);
simpleParameter: (validIdentifier | reservedWords | expression);

callParameter:
      valuedParameter
    | setParameter
    | simpleParameter
    ;


parameterList : callParameter ( COMMA callParameter)*  ;

formalProcedureCall :
  { include_insideLoop() }? CONTINUE
| { break_insideLoop() }? BREAK
|   validIdentifier (COMMA parameterList)?
;

memberProcedureCall: variableAccessByValueOrReference (MEMBER|DOT) (validIdentifier METHOD)? formalProcedureCall;

procedureCall:
    memberProcedureCall
  | formalProcedureCall
  ;

formalFunctionCall : validIdentifier LBRACE (parameterList)? RBRACE  ;
memberFunctionCall : variableAccessByValueOrReference (MEMBER|DOT) (validIdentifier METHOD)? formalFunctionCall;
functionCall:
       memberFunctionCall
     | formalFunctionCall
     ;

valuedKeyword:<assoc=right> (validIdentifier | reservedWords) ASSIGN  (validIdentifier | reservedWords);
simpleKeyword: (validIdentifier | reservedWords);
keywordDeclaration:
         valuedKeyword
	|simpleKeyword
        ;
	
keywordDeclarationList : keywordDeclaration ( COMMA keywordDeclaration )*    ;
    

procedureDefinition :
        PRO ( objectName | validIdentifier ) (COMMA keywordDeclarationList)? endUnit (statementList)*  END
  ;
 

functionDefinition:
        FUNCTION ( objectName | validIdentifier ) (COMMA keywordDeclarationList)? endUnit (statementList)* END
    ;

objectName : validIdentifier METHOD validIdentifier ;    


expressionList: expression  ( COMMA expression )* ;

//arrayListDefinition:          LSQUARE expressionList RSQUARE    ;
//arrayAutoDefinition:          LSQUARE expression COLON expression RSQUARE    ;
//arrayAutoIncrementDefinition: LSQUARE expression COLON expression COLON expression RSQUARE    ;
//
//arrayDefinition:
// | arrayListDefinition        
// | arrayAutoDefinition        
// | arrayAutoIncrementDefinition
//;


namedStructure:
	  LCURLY (validIdentifier|SYSVARNAME)
	                    ( (COMMA inheritsOrTagDef)*
	                    | COMMA expressionList 
	                    | ) RCURLY
	;
anonymousStructure: LCURLY inheritsOrTagDef (COMMA inheritsOrTagDef)* RCURLY;

inheritsOrTagDef
    : inheritsStructure
    | normalTag
    ;

inheritsStructure: INHERITS validIdentifier;
normalTag:  validIdentifier COLON expression ;

structureDefinition
    : namedStructure
    | anonymousStructure
    ;


//constant_hex_byte    	:  CONSTANT_HEX_BYTE    ;
//constant_hex_long 	:  CONSTANT_HEX_LONG ;
//constant_hex_long64 	:  CONSTANT_HEX_LONG64 ;
//constant_hex_int 	:  CONSTANT_HEX_INT ;
//constant_hex_i 		:  CONSTANT_HEX_I ;  // DEFINT32
//constant_hex_ulong 	:  CONSTANT_HEX_ULONG ;
//constant_hex_ulong64	:  CONSTANT_HEX_ULONG64;
//constant_hex_ui		:  CONSTANT_HEX_UI;        // DEFINT32
//constant_hex_uint	:  CONSTANT_HEX_UINT;
//constant_byte  		:  CONSTANT_BYTE  ;
//constant_long 		:  CONSTANT_LONG ;
//constant_long64 	:  CONSTANT_LONG64 ;
//constant_int		:  CONSTANT_INT;
//constant_i		:  CONSTANT_I;        // DEFINT32
//constant_ulong 		:  CONSTANT_ULONG ;
//constant_ulong64 	:  CONSTANT_ULONG64 ;
//constant_uint		:  CONSTANT_UINT;
//constant_oct_byte  	:  CONSTANT_OCT_BYTE  ;
//constant_oct_long 	:  CONSTANT_OCT_LONG ;
//constant_oct_long64 	:  CONSTANT_OCT_LONG64 ;
//constant_oct_int	:  CONSTANT_OCT_INT;
//constant_oct_i		:  CONSTANT_OCT_I;        // DEFINT32
//constant_oct_ulong 	:  CONSTANT_OCT_ULONG ;
//constant_oct_ulong64 	:  CONSTANT_OCT_ULONG64 ;
//constant_oct_ui		:  CONSTANT_OCT_UI;
//constant_oct_uint	:  CONSTANT_OCT_UINT;
//constant_float     	:  CONSTANT_FLOAT     ;
//constant_double		:  CONSTANT_DOUBLE;
//constant_bin_byte  	:  CONSTANT_BIN_BYTE  ;
//constant_bin_long 	:  CONSTANT_BIN_LONG ;
//constant_bin_long64 	:  CONSTANT_BIN_LONG64 ;
//constant_bin_int	:  CONSTANT_BIN_INT;
//constant_bin_i		:  CONSTANT_BIN_I;        // DEFINT32
//constant_bin_ulong 	:  CONSTANT_BIN_ULONG ;
//constant_bin_ulong64 	:  CONSTANT_BIN_ULONG64 ;
//constant_bin_ui		:  CONSTANT_BIN_UI;        // DEFINT32
//constant_bin_uint	:  CONSTANT_BIN_UINT;

numeric_constant:
   CONSTANT_HEX_BYTE     #constant_hex_byte     
 | CONSTANT_HEX_LONG     #constant_hex_long     
 | CONSTANT_HEX_LONG64   #constant_hex_long64   
 | CONSTANT_HEX_INT      #constant_hex_int      
 | CONSTANT_HEX_I 	 #constant_hex_i 	     
 | CONSTANT_HEX_ULONG    #constant_hex_ulong    
 | CONSTANT_HEX_ULONG64  #constant_hex_ulong64  
 | CONSTANT_HEX_UI	 #constant_hex_ui	     
 | CONSTANT_HEX_UINT     #constant_hex_uint     
 | CONSTANT_BYTE  	 #constant_byte  	     
 | CONSTANT_LONG 	 #constant_long 	     
 | CONSTANT_LONG64       #constant_long64       
 | CONSTANT_INT	     	 #constant_int	     	 
 | CONSTANT_I	     	 #constant_i	     	 
 | CONSTANT_ULONG 	 #constant_ulong 	     
 | CONSTANT_ULONG64      #constant_ulong64      
 | CONSTANT_UINT	 #constant_uint	     
 | CONSTANT_OCT_BYTE     #constant_oct_byte     
 | CONSTANT_OCT_LONG     #constant_oct_long     
 | CONSTANT_OCT_LONG64   #constant_oct_long64   
 | CONSTANT_OCT_INT      #constant_oct_int      
 | CONSTANT_OCT_I	 #constant_oct_i	     
 | CONSTANT_OCT_ULONG    #constant_oct_ulong    
 | CONSTANT_OCT_ULONG64  #constant_oct_ulong64  
 | CONSTANT_OCT_UI	 #constant_oct_ui	     
 | CONSTANT_OCT_UINT     #constant_oct_uint     
 | CONSTANT_FLOAT        #constant_float        
 | CONSTANT_DOUBLE	 #constant_double	     
 | CONSTANT_BIN_BYTE     #constant_bin_byte     
 | CONSTANT_BIN_LONG     #constant_bin_long     
 | CONSTANT_BIN_LONG64   #constant_bin_long64   
 | CONSTANT_BIN_INT      #constant_bin_int      
 | CONSTANT_BIN_I	 #constant_bin_i	     
 | CONSTANT_BIN_ULONG    #constant_bin_ulong    
 | CONSTANT_BIN_ULONG64  #constant_bin_ulong64  
 | CONSTANT_BIN_UI	 #constant_bin_ui	     
 | CONSTANT_BIN_UINT     #constant_bin_uint      

 ;

listOfArrayIndexes:
    LSQUARE arrayIndex (COMMA arrayIndex)* RSQUARE; // C++  LSQUARE arrayIndex ({++rank <= MAXRANK}? COMMA arrayIndex)* RSQUARE

relaxedListOfArrayIndexes: LBRACE arrayIndex ( COMMA arrayIndex)* RBRACE; // C++  LSBRACE arrayIndex ({++rank <= MAXRANK}? COMMA arrayIndex)* RBRACE

//allElements : ASTERIX ;
//index : expression;
//range:   expression COLON (allElements | expression );
//stepRange: expression COLON (allElements | expression ) COLON expression;
//arrayIndex: ( allElements  | stepRange | range | index) ; 
arrayIndex: ( ASTERIX|expression (COLON (ASTERIX|expression) (COLON expression)? )? ) ; 

// the expressions *************************************

variableName : validIdentifier; //not SYSVARNAME

// this is SYNTATICALLY ok as an lvalue, but if one try to assign
// something to an non-var an error is raised
// bracedExpression :  LBRACE expression RBRACE ;

operatedVariable : LBRACE variableAccessByValueOrReference assignmentOperator expression RBRACE ;
// only used in variableAccessByValueOrReference
// sysvar or expression (first in struct access - therefore the name)
// a variable MUST be already defined here
varDesignator
    :  SYSVARNAME
    |  operatedVariable
    |  variableName
    |  LBRACE expression RBRACE //bracedExpression
    ;

//relaxed mode slows terribly the parser, for a few rare cases. MUST use a two-pass parsing, where relaxed mode is enabled only of there is a problem
varSubset:  varDesignator  ( {IsRelaxed()}? relaxedListOfArrayIndexes | listOfArrayIndexes)
	   ;

implicitArray:
  UNDEFINED
| listOfArrayIndexes
;

tagNumberIndicator: LBRACE expression RBRACE;

tagIdentifier
    : tagNumberIndicator
    | SYSVARNAME  
    | validIdentifier
    | EXCLAMATION // IDL allows '!' as tag despite the documentation.
    ;

taggedEntryTarget
    : taggedEntry
    | varSubset
    | varDesignator
    ;

variableAccessByValueOrReference
    : pointedVariable
    | taggedEntry
    | varSubset
    | varDesignator
    ;
    
taggedEntry    :
      varSubset  DOT taggedEntryTarget //reflect struct-in-struct hierarchy.
    | varDesignator DOT taggedEntryTarget
    ;
pointedVariable : ASTERIX variableAccessByValueOrReference ;

string:
   NULLSTRING
   | STRING
   ;

primaryLevelExpression:
      numeric_constant 	   
    | string
    | structureDefinition
    | implicitArray //always between '[]'
    | variableAccessByValueOrReference //variable is Named
    | functionCall
    ;
    
decincExpression:
          primaryLevelExpression ( INC | DEC )?
        | (INC |DEC)  primaryLevelExpression
        ;
	
thirdLevelExpression:<assoc=right>
     decincExpression
     (
       POW decincExpression
     )*
     ;
     
multiplicativeExpression: // '*' | '#' | '##' | '/' | 'mod' // level 4
      thirdLevelExpression
      (
        ( ASTERIX
	| MATRIX_OP1
	| MATRIX_OP2
	| SLASH
	| MOD_OP
	) thirdLevelExpression
      )*
      ;

signedMultiplicativeExpression:
      ( PLUS
      | MINUS
      |      ) multiplicativeExpression
      ;
      
additiveExpression: // '+' | '-' | '<' | '>'  // level 5
       ( signedMultiplicativeExpression | negativeExpression )
         (
	    ( PLUS
            | MINUS
            | LTMARK
            | GTMARK
	    ) (  multiplicativeExpression | negativeExpression )
	 )*
	 ;
	    
negativeExpression:
    (  NOT_OP 
    | LOG_NEG ) multiplicativeExpression // true precedence of ~ operator
    ;

relationalExpression: // 'eq' | 'ne' | 'le' | 'lt' | 'ge' | 'gt' // level 6
        additiveExpression
        (
            ( EQ_OP
            | NE_OP
            | LE_OP
            | LT_OP
            | GE_OP
            | GT_OP
            ) additiveExpression
        )*
    ;

bitwiseExpression: // 'and' | 'or' | 'xor' // level 7
        relationalExpression
        ( 
            ( AND_OP 
            | OR_OP 
            | XOR_OP 
            ) relationalExpression
        )*
    ;
    
logicalExpression : // '&&' | '||'  //level 8
        left = bitwiseExpression
        ( 
           local_operator= ( LOG_AND 
            | LOG_OR 
            ) operand = bitwiseExpression
        )*
    ;

expression:<assoc=right> logicalExpression (QUESTION expression COLON expression)? ;  //' ?:' // level 9

assignmentStatement
    : variableAccessByValueOrReference assignmentOperator expression
    | decincExpression
    ;

autoPrintStatement: expressionList;
interactiveStatement
     : (BEGIN | validIdentifier COLON)*
        ( assignmentStatement
	| compoundStatement
        | ifStatement
	| switchStatement
	| caseStatement
	| forStatement
	| foreachStatement
	| repeatStatement
	| whileStatement
	| autoPrintStatement
        ) endUnit
    ;

