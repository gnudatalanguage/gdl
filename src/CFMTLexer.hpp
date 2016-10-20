#ifndef INC_CFMTLexer_hpp_
#define INC_CFMTLexer_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20120518): "cformat.g" -> "CFMTLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "CFMTTokenTypes.hpp"
#include <antlr/CharScanner.hpp>

#include <antlr/TokenStreamSelector.hpp>
    
//    using namespace antlr;

class CUSTOM_API CFMTLexer : public antlr::CharScanner, public CFMTTokenTypes
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
    
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return false;
	}
public:
	CFMTLexer(std::istream& in);
	CFMTLexer(antlr::InputBuffer& ib);
	CFMTLexer(const antlr::LexerSharedInputState& state);
	antlr::RefToken nextToken();
	public: void mALL(bool _createToken);
	protected: void mCD(bool _createToken);
	protected: void mCE(bool _createToken);
	protected: void mCI(bool _createToken);
	protected: void mCF(bool _createToken);
	protected: void mCG(bool _createToken);
	protected: void mCO(bool _createToken);
	protected: void mCB(bool _createToken);
	protected: void mCS(bool _createToken);
	protected: void mCX(bool _createToken);
	protected: void mCZ(bool _createToken);
	protected: void mCNUMBER(bool _createToken);
	protected: void mCDOT(bool _createToken);
	protected: void mCWS(bool _createToken);
	protected: void mCSTR(bool _createToken);
	protected: void mCSTR1(bool _createToken);
	protected: void mCSTR2(bool _createToken);
	protected: void mESC(bool _createToken);
	protected: void mOCTESC(bool _createToken);
	protected: void mHEXESC(bool _createToken);
	protected: void mODIGIT(bool _createToken);
	protected: void mHDIGIT(bool _createToken);
	protected: void mDIGITS(bool _createToken);
private:
	
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
};

#endif /*INC_CFMTLexer_hpp_*/
