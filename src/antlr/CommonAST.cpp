/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: CommonAST.cpp,v 1.1.1.1 2004-12-09 15:10:20 m_schellens Exp $
 */

#include "antlr/config.hpp"
#include "antlr/CommonAST.hpp"
#include "antlr/ANTLRUtil.hpp"

#include <cstdlib>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

const char* const CommonAST::TYPE_NAME = "CommonAST";

CommonAST::CommonAST()
: BaseAST()
, ttype( Token::INVALID_TYPE )
, text("")
{
}

CommonAST::CommonAST(RefToken t)
: BaseAST()
, ttype( t->getType() )
, text( t->getText() )
{
}

CommonAST::~CommonAST()
{
}

const char* CommonAST::typeName( void ) const
{
	return CommonAST::TYPE_NAME;
}

CommonAST::CommonAST(const CommonAST& other)
: BaseAST(other)
, ttype(other.ttype)
, text(other.text)
{
}

RefAST CommonAST::clone( void ) const
{
	CommonAST *ast = new CommonAST( *this );
	return RefAST(ast);
}

ANTLR_USE_NAMESPACE(std)string CommonAST::getText() const
{
	return text;
}

int CommonAST::getType() const
{
	return ttype;
}

void CommonAST::initialize(int t,const ANTLR_USE_NAMESPACE(std)string& txt)
{
	setType(t);
	setText(txt);
}

void CommonAST::initialize(RefAST t)
{
	setType(t->getType());
	setText(t->getText());
}

void CommonAST::initialize(RefToken t)
{
	setType(t->getType());
	setText(t->getText());
}

#ifdef ANTLR_SUPPORT_XML
void CommonAST::initialize( ANTLR_USE_NAMESPACE(std)istream& in )
{
	ANTLR_USE_NAMESPACE(std)string t1, t2, text;

	// text
	read_AttributeNValue( in, t1, text );

	read_AttributeNValue( in, t1, t2 );
#ifdef ANTLR_ATOI_IN_STD
	int type = ANTLR_USE_NAMESPACE(std)atoi(t2.c_str());
#else
	int type = atoi(t2.c_str());
#endif

	// initialize first part of AST.
	this->initialize( type, text );
}
#endif

void CommonAST::setText(const ANTLR_USE_NAMESPACE(std)string& txt)
{
	text = txt;
}

void CommonAST::setType(int type)
{
	ttype = type;
}

RefAST CommonAST::factory()
{
	return RefAST(new CommonAST);
}

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

