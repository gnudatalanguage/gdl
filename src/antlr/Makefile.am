INCLUDES = -I$(top_srcdir)/src $(all_includes)

antlr_sources = ANTLRException.hpp ANTLRUtil.cpp ANTLRUtil.hpp ASTArray.hpp ASTFactory.cpp ASTFactory.hpp AST.hpp ASTNULLType.cpp ASTNULLType.hpp ASTPair.hpp ASTRefCount.cpp ASTRefCount.hpp BaseAST.cpp BaseAST.hpp BitSet.cpp BitSet.hpp CharBuffer.cpp CharBuffer.hpp CharInputBuffer.hpp CharScanner.cpp CharScanner.hpp CharStreamException.hpp CharStreamIOException.hpp CircularQueue.hpp CommonAST.cpp CommonAST.hpp CommonASTWithHiddenTokens.cpp CommonASTWithHiddenTokens.hpp CommonHiddenStreamToken.cpp CommonHiddenStreamToken.hpp CommonToken.cpp CommonToken.hpp config.hpp InputBuffer.cpp InputBuffer.hpp IOException.hpp LexerSharedInputState.hpp LLkParser.cpp LLkParser.hpp Makefile.am Makefile.in MismatchedCharException.cpp MismatchedCharException.hpp MismatchedTokenException.cpp MismatchedTokenException.hpp NoViableAltException.cpp NoViableAltException.hpp NoViableAltForCharException.cpp NoViableAltForCharException.hpp Parser.cpp Parser.hpp ParserSharedInputState.hpp RecognitionException.cpp RecognitionException.hpp RefCount.hpp SemanticException.hpp String.cpp String.hpp TokenBuffer.cpp TokenBuffer.hpp Token.cpp Token.hpp TokenStreamBasicFilter.cpp TokenStreamBasicFilter.hpp TokenStreamException.hpp TokenStreamHiddenTokenFilter.cpp TokenStreamHiddenTokenFilter.hpp TokenStream.hpp TokenStreamIOException.hpp TokenStreamRecognitionException.hpp TokenStreamRetryException.hpp TokenStreamSelector.cpp TokenStreamSelector.hpp TreeParser.cpp TreeParser.hpp TreeParserSharedInputState.hpp

if !BUILDASLIBRARY
lib_LIBRARIES =  libantlr.a
libantlr_a_SOURCES = $(antlr_sources)
# trigger basename generation
libantlr_a_CXXFLAGS = $(AM_CXXFLAGS)
endif

# if we build a shared library, antlr stuff has to build
# as shared library also
if BUILDASLIBRARY
noinst_LTLIBRARIES =  libantlr.la
libantlr_la_SOURCES = $(antlr_sources)
endif

