#ifndef INC_FMTOut_hpp_
#define INC_FMTOut_hpp_

#include <antlr/config.hpp>
#include "FMTOutTokenTypes.hpp"
/* $ANTLR 2.7.7 (2006-11-01): "format.out.g" -> "FMTOut.hpp"$ */
#include <antlr/TreeParser.hpp>



#include <algorithm>
#include <fstream>
#include <cassert>
#include <sstream>
#include <string>

//***test
#include "print_tree.hpp"

#include "envt.hpp"

#include "fmtnode.hpp"
//#include "getfmtast.hpp"    // for FMTNodeFactory;    

#include "gdljournal.hpp"

class CUSTOM_API FMTOut : public antlr::TreeParser, public FMTOutTokenTypes
{

public:
    FMTOut( RefFMTNode fmt, std::ostream* os_, EnvT* e_, int parOffset)
    : antlr::TreeParser(), os(&record), e( e_), nextParIx( parOffset),
	valIx(0), termFlag(false), nonlFlag(false), nElements(0)
    {
        nParam = e->NParam();

        NextPar();
    
        GDLStream* j = lib::get_journal();
        toJournal = j && j->OStream().rdbuf() == os_->rdbuf();

        SizeT previousPar = nextParIx;
        SizeT previousVal = valIx;
        format(fmt);

        while (actPar) {
            if (nextParIx == previousPar && valIx == previousVal)
                throw GDLException("Infinite format loop detected.");

            previousPar = nextParIx;
            previousVal = valIx;
            FinishRecord();
            format_reversion(reversionAnker);
        }

        // A format error must leave the destination at its original position.
        FinishRecord(!nonlFlag);
        os_->write(completed.data(), completed.size());
        os_->flush();
        if (!*os_ && dynamic_cast<ogzstream*>(os_))
            throw GDLIOException("Error writing formatted data to compressed file.");
    }

private:
    // Tab positions are relative to this record, including when output is a journal.
    std::ostringstream record;
    // Finished records, written only after the whole format succeeds.
    std::string completed;
    bool toJournal = false;

    void FinishRecord(bool newline = true)
    {
        if (toJournal)
            completed += lib::JOURNALCOMMENT;
        completed += record.str();
        if (newline)
            completed += '\n';
        record.str(std::string{});
    }

    void TabTo(std::streamoff position)
    {
        record.seekp(0, std::ios_base::end);
        std::streamoff end = record.tellp();
        if (position <= end) {
            record.seekp(position);
            return;
        }
        static const std::string spaces(256, ' ');
        while (end < position) {
            const auto n = std::min<std::streamoff>(position - end, spaces.size());
            record.write(spaces.data(), n);
            end += n;
        }
    }

    void NextPar()
    {
        valIx = 0;
        if( nextParIx < nParam)
        {
            actPar = e->GetPar( nextParIx);
            if( actPar != NULL) {
				if ( actPar->Type() == GDL_PTR || actPar->Type() ==  GDL_OBJ) throw GDLException("Expression containing "+actPar->TypeStr()+" not allowed in this context: ",true,true,nextParIx);
              nElements = actPar->ToTransfer();
			}
            else {
				throw GDLException("Variable is undefined",true,true,nextParIx);
			}
        } 
        else 
        {
            actPar = NULL;
            nElements = 0;
        }
        nextParIx++;
    }

    void NextVal( SizeT n=1)
    {
        valIx += n;
        if( valIx >= nElements)
            NextPar();
    }
    
    std::ostream* os;
    EnvT*    e;
    SizeT   nextParIx;
    SizeT   valIx;

    bool termFlag;
    bool nonlFlag;

    SizeT   nParam;
    BaseGDL* actPar;
    SizeT nElements;

    RefFMTNode reversionAnker;
public:
	FMTOut();
	static void initializeASTFactory( antlr::ASTFactory& factory );
	int getNumTokens() const
	{
		return FMTOut::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return FMTOut::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return FMTOut::tokenNames;
	}
	public: void format(RefFMTNode _t);
	public: void q(RefFMTNode _t);
	public: void f(RefFMTNode _t);
	public: void format_reversion(RefFMTNode _t);
	public: void f_csubcode(RefFMTNode _t);
	public: void x(RefFMTNode _t);
	public: void calendar_code(RefFMTNode _t,
		SizeT r
	);
public:
	antlr::RefAST getAST()
	{
		return antlr::RefAST(returnAST);
	}
	
protected:
	RefFMTNode returnAST;
	RefFMTNode _retTree;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 87;
#else
	enum {
		NUM_TOKENS = 87
	};
#endif
	
	static const unsigned long _tokenSet_0_data_[];
	static const antlr::BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const antlr::BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const antlr::BitSet _tokenSet_2;
};

#endif /*INC_FMTOut_hpp_*/
