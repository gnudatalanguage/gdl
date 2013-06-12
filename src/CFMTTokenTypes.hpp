#ifndef INC_CFMTTokenTypes_hpp_
#define INC_CFMTTokenTypes_hpp_

/* $ANTLR 2.7.7 (20120518): "cformat.g" -> "CFMTTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API CFMTTokenTypes {
#endif
	enum {
		EOF_ = 1,
		CSTR = 4,
		CSTR1 = 5,
		CSTR2 = 6,
		ESC = 7,
		OCTESC = 8,
		ODIGIT = 9,
		HEXESC = 10,
		HDIGIT = 11,
		CD = 12,
		CE = 13,
		CI = 14,
		CF = 15,
		CG = 16,
		CO = 17,
		CB = 18,
		CS = 19,
		CX = 20,
		CZ = 21,
		CDOT = 22,
		DIGITS = 23,
		CNUMBER = 24,
		CWS = 25,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_CFMTTokenTypes_hpp_*/
