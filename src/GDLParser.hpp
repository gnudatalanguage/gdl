#ifndef INC_GDLParser_hpp_
#define INC_GDLParser_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (2006-11-01): "gdlc.g" -> "GDLParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "GDLTokenTypes.hpp"
#include <antlr/LLkParser.hpp>


#include <fstream>

#include "GDLParser.hpp"

#include "str.hpp"
#include "dnodefactory.hpp"
#include "objects.hpp"
#include "initsysvar.hpp"

#include <antlr/TokenStreamSelector.hpp>

#include <antlr/SemanticException.hpp>
#include <antlr/NoViableAltForCharException.hpp>
#include <antlr/TokenStreamIOException.hpp>
#include <antlr/CharInputBuffer.hpp>


// definition in dinterpreter.cpp
void MemorizeCompileOptForMAINIfNeeded( unsigned int cOpt);

class CUSTOM_API GDLParser : public antlr::LLkParser, public GDLTokenTypes
{

    public:
    enum CompileOpt {
        NONE=0,
        DEFINT32=1,
        HIDDEN=2,
        OBSOLETE=4,
        STRICTARR=8,
        LOGICAL_PREDICATE=16, // *** functionality not implemeted yet
        IDL2=DEFINT32 | STRICTARR,
        STRICTARRSUBS=32,
        STATIC=64,
        NOSAVE=128,
        GDL_HIDDEN=256 //flag to avoid writing "Compiled module" at compilation. 
                       //Not the same as HIDDEN, that can be set in the procdure code and hides also the procedure from the HELP. 
    };

    void SetCompileOpt( unsigned int cOpt)
    {
        this->compileOpt = cOpt;
    }
    
    private:
    std::string subName; // name of procedure function to be compiled ("" -> all file)
    bool   searchForPro; // true -> procedure subName, false -> function subName 
    bool   SearchedRoutineFound; 
    unsigned int compileOpt=0;
	bool relaxed=false; // use of a bool speedups {}? constructs
    int fussy=((compileOpt & STRICTARR)!=0)?2:1; //auto recovery if compile opt is not strictarr
    int LastGoodPosition=0; // last position of start of PRO or FUNC -- used in recovery mode
	bool recovery=false; //recovery mode going to 'fussy' if STRICTARR generated an error 
    void AddCompileOpt( const std::string &opt)
    {
        if(      opt == "DEFINT32")          compileOpt |= DEFINT32;
        else if( opt == "HIDDEN")            compileOpt |= HIDDEN;
        else if( opt == "OBSOLETE")          compileOpt |= OBSOLETE;
        else if( opt == "STRICTARR")         {compileOpt |= STRICTARR; fussy=2; relaxed=false;} // fussy=2: a strictarr syntax error is fatal
        else if( opt == "LOGICAL_PREDICATE") compileOpt |= LOGICAL_PREDICATE;
        else if( opt == "IDL2")              {compileOpt |= IDL2; fussy=2; relaxed=false;}
        else if( opt == "STRICTARRSUBS")     compileOpt |= STRICTARRSUBS;
        else if( opt == "STATIC")            compileOpt |= STATIC;
        else if( opt == "NOSAVE")            compileOpt |= NOSAVE;
        else throw GDLException("Unrecognised COMPILE_OPT option: "+opt);
        MemorizeCompileOptForMAINIfNeeded( compileOpt);
    }

    bool ConstantExprNode( int t)
    {
        return (t == CONSTANT) || 
               (t == ARRAYDEF_CONST);
    }

    public:
    GDLParser(antlr::TokenStream& selector, 
              const std::string& sName, 
              bool searchPro, // true -> search for procedure sName, false -> for function
              unsigned int compileOptIn):
    antlr::LLkParser(selector,2), subName(sName), searchForPro( searchPro), 
    SearchedRoutineFound(false), compileOpt(compileOptIn)
    { 
        //        setTokenNames(_tokenNames);
    }
public:
	void initializeASTFactory( antlr::ASTFactory& factory );
protected:
	GDLParser(antlr::TokenBuffer& tokenBuf, int k);
public:
	GDLParser(antlr::TokenBuffer& tokenBuf);
protected:
	GDLParser(antlr::TokenStream& lexer, int k);
public:
	GDLParser(antlr::TokenStream& lexer);
	GDLParser(const antlr::ParserSharedInputState& state);
	int getNumTokens() const
	{
		return GDLParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return GDLParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return GDLParser::tokenNames;
	}
	public: void identifier();
	public: void translation_unit();
	public: void end_unit();
	public: void forward_function();
	public: void procedure_def();
	public: void function_def();
	public: void common_block();
	public: void statement_list();
	public: void interactive_compile();
	public: void parameter_declaration();
	public: void interactive();
	public: void end_mark();
	public: void interactive_statement();
	public: void statement();
	public: void switch_statement();
	public: void expr();
	public: void switch_body();
	public: void endswitch_mark();
	public: void endswitchelse_mark();
	public: void case_statement();
	public: void case_body();
	public: void endcase_mark();
	public: void endcaseelse_mark();
	public: void identifier_list();
	public: void keyword_declaration();
	protected: std::string  object_name();
	public: void compile_opt();
	public: void endforeach_mark();
	public: void endfor_mark();
	public: void endrep_mark();
	public: void endwhile_mark();
	public: void endif_mark();
	public: void endelse_mark();
	public: void compound_statement();
	public: void label_statement();
	public: void label();
	public: void baseclass_method();
	public: void assign_expr();
	public: void deref_dot_expr_keeplast();
	public: void formal_procedure_call();
	public: void deref_expr();
	public: void procedure_call();
	public: void for_statement();
	public: void foreach_statement();
	public: void repeat_statement();
	public: void while_statement();
	public: void jump_statement();
	public: void if_statement();
	public: void repeat_block();
	public: void while_block();
	public: void for_block();
	public: void foreach_block();
	public: void if_block();
	public: void else_block();
	public: void parameter_def_list();
	public: void formal_function_call();
	public: void parameter_def();
	public: void array_def();
	public: void struct_identifier();
	public: void struct_name();
	public: void struct_def();
	public: void named_tag_def_list();
	public: void tag_def_list();
	public: void tag_def();
	public: void ntag_def();
	public: void ntag_defs();
	public: void named_tag_def_entry();
	public: void constant_hex_byte();
	public: void constant_hex_long();
	public: void constant_hex_long64();
	public: void constant_hex_int();
	public: void constant_hex_i();
	public: void constant_hex_ulong();
	public: void constant_hex_ulong64();
	public: void constant_hex_ui();
	public: void constant_hex_uint();
	public: void constant_byte();
	public: void constant_long();
	public: void constant_long64();
	public: void constant_int();
	public: void constant_i();
	public: void constant_ulong();
	public: void constant_ulong64();
	public: void constant_ui();
	public: void constant_uint();
	public: void constant_oct_byte();
	public: void constant_oct_long();
	public: void constant_oct_long64();
	public: void constant_oct_int();
	public: void constant_oct_i();
	public: void constant_oct_ulong();
	public: void constant_oct_ulong64();
	public: void constant_oct_ui();
	public: void constant_oct_uint();
	public: void constant_float();
	public: void constant_double();
	public: void constant_bin_byte();
	public: void constant_bin_long();
	public: void constant_bin_long64();
	public: void constant_bin_int();
	public: void constant_bin_i();
	public: void constant_bin_ulong();
	public: void constant_bin_ulong64();
	public: void constant_bin_ui();
	public: void constant_bin_uint();
	public: void constant_cmplx_i();
	public: void constant_cmplxdbl_i();
	public: void numeric_constant();
	public: void arrayindex_list();
	public: void arrayindex();
	public: void arrayindex_sloppy();
	public: void arrayindex_list_sloppy();
	public: void all_elements();
	public: void sysvar();
	public: void var();
	public: void brace_expr();
	public: void array_expr_1st_sub();
	public: void array_expr_1st();
	public: void array_expr_nth_sub();
	public: void array_expr_nth();
	public: void tag_array_expr_nth_sub();
	public: void tag_array_expr_nth();
	protected: int  tag_access_keeplast();
	protected: SizeT  tag_access();
	public: void deref_dot_expr();
	protected: bool  member_function_call();
	public: void member_function_call_dot();
	public: void arrayexpr_mfcall();
	public: void primary_expr();
	public: void decinc_expr();
	public: void exponential_expr();
	public: void multiplicative_expr();
	public: void signed_multiplicative_expr();
	public: void additive_expr();
	public: void neg_expr();
	public: void relational_expr();
	public: void boolean_expr();
	public: void logical_expr();
public:
	antlr::RefAST getAST()
	{
		return antlr::RefAST(returnAST);
	}
	
protected:
	RefDNode returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 240;
#else
	enum {
		NUM_TOKENS = 240
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const antlr::BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const antlr::BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const antlr::BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const antlr::BitSet _tokenSet_4;
	static const unsigned long _tokenSet_5_data_[];
	static const antlr::BitSet _tokenSet_5;
	static const unsigned long _tokenSet_6_data_[];
	static const antlr::BitSet _tokenSet_6;
	static const unsigned long _tokenSet_7_data_[];
	static const antlr::BitSet _tokenSet_7;
	static const unsigned long _tokenSet_8_data_[];
	static const antlr::BitSet _tokenSet_8;
	static const unsigned long _tokenSet_9_data_[];
	static const antlr::BitSet _tokenSet_9;
	static const unsigned long _tokenSet_10_data_[];
	static const antlr::BitSet _tokenSet_10;
	static const unsigned long _tokenSet_11_data_[];
	static const antlr::BitSet _tokenSet_11;
	static const unsigned long _tokenSet_12_data_[];
	static const antlr::BitSet _tokenSet_12;
	static const unsigned long _tokenSet_13_data_[];
	static const antlr::BitSet _tokenSet_13;
	static const unsigned long _tokenSet_14_data_[];
	static const antlr::BitSet _tokenSet_14;
	static const unsigned long _tokenSet_15_data_[];
	static const antlr::BitSet _tokenSet_15;
	static const unsigned long _tokenSet_16_data_[];
	static const antlr::BitSet _tokenSet_16;
	static const unsigned long _tokenSet_17_data_[];
	static const antlr::BitSet _tokenSet_17;
	static const unsigned long _tokenSet_18_data_[];
	static const antlr::BitSet _tokenSet_18;
	static const unsigned long _tokenSet_19_data_[];
	static const antlr::BitSet _tokenSet_19;
	static const unsigned long _tokenSet_20_data_[];
	static const antlr::BitSet _tokenSet_20;
	static const unsigned long _tokenSet_21_data_[];
	static const antlr::BitSet _tokenSet_21;
	static const unsigned long _tokenSet_22_data_[];
	static const antlr::BitSet _tokenSet_22;
	static const unsigned long _tokenSet_23_data_[];
	static const antlr::BitSet _tokenSet_23;
	static const unsigned long _tokenSet_24_data_[];
	static const antlr::BitSet _tokenSet_24;
};

#endif /*INC_GDLParser_hpp_*/
