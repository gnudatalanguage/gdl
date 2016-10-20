#ifndef INC_FMTLexer_hpp_
#define INC_FMTLexer_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.7 (20130428): "format.g" -> "FMTLexer.hpp"$ */
#include <antlr/CommonToken.hpp>
#include <antlr/InputBuffer.hpp>
#include <antlr/BitSet.hpp>
#include "FMTTokenTypes.hpp"
#include <antlr/CharScanner.hpp>


#include <fstream>
#include <sstream>

#include "fmtnode.hpp"

#include "CFMTLexer.hpp"

#include <antlr/TokenStreamSelector.hpp>

//using namespace antlr;

class CUSTOM_API FMTLexer : public antlr::CharScanner, public FMTTokenTypes
{

    private:
    antlr::TokenStreamSelector*  selector; 
    CFMTLexer*            cLexer;

    public:
    void SetSelector( antlr::TokenStreamSelector& s)
    {
        selector = &s;
    }
    void SetCLexer( CFMTLexer& l)
    {
        cLexer = &l;
    }
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const
	{
		return false;
	}
public:
	FMTLexer(std::istream& in);
	FMTLexer(antlr::InputBuffer& ib);
	FMTLexer(const antlr::LexerSharedInputState& state);
	antlr::RefToken nextToken();
	public: void mSTRING(bool _createToken);
	public: void mCSTRING(bool _createToken);
	public: void mLBRACE(bool _createToken);
	public: void mRBRACE(bool _createToken);
	public: void mSLASH(bool _createToken);
	public: void mCOMMA(bool _createToken);
	public: void mA(bool _createToken);
	public: void mTERM(bool _createToken);
	public: void mNONL(bool _createToken);
	public: void mF(bool _createToken);
	public: void mD(bool _createToken);
	public: void mE(bool _createToken);
	public: void mG(bool _createToken);
	public: void mI(bool _createToken);
	public: void mO(bool _createToken);
	public: void mB(bool _createToken);
	public: void mZ(bool _createToken);
	public: void mZZ(bool _createToken);
	public: void mQ(bool _createToken);
	public: void mH(bool _createToken);
	public: void mT(bool _createToken);
	public: void mL(bool _createToken);
	public: void mR(bool _createToken);
	public: void mX(bool _createToken);
	public: void mC(bool _createToken);
	public: void mCMOA(bool _createToken);
	public: void mCMoA(bool _createToken);
	public: void mCmoA(bool _createToken);
	public: void mCMOI(bool _createToken);
	public: void mCDI(bool _createToken);
	public: void mCMI(bool _createToken);
	public: void mCYI(bool _createToken);
	public: void mCSI(bool _createToken);
	public: void mCSF(bool _createToken);
	public: void mCHI(bool _createToken);
	public: void mChI(bool _createToken);
	public: void mCDWA(bool _createToken);
	public: void mCDwA(bool _createToken);
	public: void mCdwA(bool _createToken);
	public: void mCAPA(bool _createToken);
	public: void mCApA(bool _createToken);
	public: void mCapA(bool _createToken);
	public: void mPERCENT(bool _createToken);
	public: void mDOT(bool _createToken);
	public: void mPM(bool _createToken);
	public: void mMP(bool _createToken);
	protected: void mW(bool _createToken);
	public: void mWHITESPACE(bool _createToken);
	protected: void mDIGITS(bool _createToken);
	protected: void mCHAR(bool _createToken);
	public: void mNUMBER(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const antlr::BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const antlr::BitSet _tokenSet_2;
};

#endif /*INC_FMTLexer_hpp_*/
