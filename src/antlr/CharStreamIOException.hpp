#ifndef INC_CharStreamIOException_hpp__
#define INC_CharStreamIOException_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: CharStreamIOException.hpp,v 1.1.1.1 2004-12-09 15:10:20 m_schellens Exp $
 */

#include <antlr/config.hpp>
#include <antlr/CharStreamException.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

class ANTLR_API CharStreamIOException : public CharStreamException {
public:
	ANTLR_USE_NAMESPACE(std)exception io;

	CharStreamIOException(ANTLR_USE_NAMESPACE(std)exception& e)
		: CharStreamException(e.what()), io(e) {}
	~CharStreamIOException() throw() {}
};

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_CharStreamIOException_hpp__
