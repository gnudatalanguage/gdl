#ifndef INC_CFMTTokenTypes_hpp_
#define INC_CFMTTokenTypes_hpp_

/* $ANTLR 2.7.4: "cformat.g" -> "CFMTTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API CFMTTokenTypes {
#endif
	enum {
		EOF_ = 1,
		ALL = 4,
		CSTR = 5,
		CSTR1 = 6,
		CSTR2 = 7,
		ESC = 8,
		OCTESC = 9,
		ODIGIT = 10,
		HEXESC = 11,
		HDIGIT = 12,
		CD = 13,
		CE = 14,
		CI = 15,
		CF = 16,
		CG = 17,
		CO = 18,
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
