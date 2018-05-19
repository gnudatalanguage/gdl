#ifndef INC_CFMTTokenTypes_hpp_
#define INC_CFMTTokenTypes_hpp_

/* $ANTLR 2.7.7 (2006-11-01): "cformat.g" -> "CFMTTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API CFMTTokenTypes {
#endif
	enum {
		EOF_ = 1,
		CSTYLE = 4,
		CSTR = 5,
		CSTR1 = 6,
		CSTR2 = 7,
		ESC = 8,
		OCTESC = 9,
		ODIGIT = 10,
		HEXESC = 11,
		HDIGIT = 12,
		CD = 13,
		CSE = 14,
		CE = 15,
		CI = 16,
		CF = 17,
		CSG = 18,
		CG = 19,
		CO = 20,
		CB = 21,
		CS = 22,
		CX = 23,
		CZ = 24,
		CDOT = 25,
		PM = 26,
		MP = 27,
		PLUS = 28,
		MOINS = 29,
		DIGITS = 30,
		CNUMBER = 31,
		CWS = 32,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
#endif /*INC_CFMTTokenTypes_hpp_*/
