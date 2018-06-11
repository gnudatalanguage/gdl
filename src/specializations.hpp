/***************************************************************************
              specializations.hpp - declarations of template specializations 
              to be included from datatypes.hpp only
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// basic_op.cpp

template<>
SizeT Data_<SpDObj>::N_Elements() const; 

template<> 
BaseGDL* Data_<SpDObj>::EqOp( BaseGDL*);
template<> 
BaseGDL* Data_<SpDObj>::NeOp( BaseGDL*);

template<> 
Data_<SpDFloat>* Data_<SpDFloat>::NotOp();
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::NotOp();
template<> 
Data_<SpDString>* Data_<SpDString>::NotOp();
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::NotOp();
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::NotOp();
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::NotOp();
template<> 
BaseGDL* Data_<SpDString>::UMinus();
template<> 
BaseGDL* Data_<SpDPtr>::UMinus();
template<> 
void Data_<SpDFloat>::Dec();
template<> 
void Data_<SpDFloat>::Inc();
template<> 
void Data_<SpDDouble>::Dec();
template<> 
void Data_<SpDDouble>::Inc();
template<> 
void Data_<SpDComplex>::Dec();
template<> 
void Data_<SpDComplex>::Inc();
template<> 
void Data_<SpDComplexDbl>::Dec();
template<> 
void Data_<SpDComplexDbl>::Inc();
template<> 
void Data_<SpDString>::Dec();
template<> 
void Data_<SpDPtr>::Dec();
template<> 
void Data_<SpDObj>::Dec();
template<> 
void Data_<SpDString>::Inc();
template<> 
void Data_<SpDPtr>::Inc();
template<> 
void Data_<SpDObj>::Inc();
// template<> 
// Data_<SpDFloat>* Data_<SpDFloat>::AndOp( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInv( BaseGDL* r);
// template<> 
// Data_<SpDDouble>* Data_<SpDDouble>::AndOp( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInv( BaseGDL* r);
// template<> 
// Data_<SpDString>* Data_<SpDString>::AndOp( BaseGDL* r);
// template<> 
// Data_<SpDComplex>* Data_<SpDComplex>::AndOp( BaseGDL* r);
// template<> 
// Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOp( BaseGDL* r);
// template<> 
// Data_<SpDPtr>* Data_<SpDPtr>::AndOp( BaseGDL* r);
// template<> 
// Data_<SpDObj>* Data_<SpDObj>::AndOp( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::OrOp( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInv( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::OrOp( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInv( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::OrOp( BaseGDL* r);
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::OrOp( BaseGDL* r);
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOp( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::OrOp( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::XorOp( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::XorOp( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::XorOp( BaseGDL* r);
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::XorOp( BaseGDL* r);
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOp( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::XorOp( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::XorOpS( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::XorOpS( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::XorOpS( BaseGDL* r);
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::XorOpS( BaseGDL* r);
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOpS( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::XorOpS( BaseGDL* r);











template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpNew( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInvNew( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::AndOpNew( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInvNew( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::AndOpNew( BaseGDL* r);
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::AndOpNew( BaseGDL* r);
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOpNew( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::AndOpNew( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::OrOpNew( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInvNew( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::OrOpNew( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInvNew( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::OrOpNew( BaseGDL* r);
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::OrOpNew( BaseGDL* r);
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOpNew( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::OrOpNew( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::XorOpNew( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::XorOpNew( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::XorOpNew( BaseGDL* r);
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::XorOpNew( BaseGDL* r);
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOpNew( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::XorOpNew( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::XorOpSNew( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::XorOpSNew( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::XorOpSNew( BaseGDL* r);
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::XorOpSNew( BaseGDL* r);
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOpSNew( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::XorOpSNew( BaseGDL* r);




















// template<> 
// Data_<SpDByte>* Data_<SpDPtr>::EqOp( BaseGDL* r);
// template<> 
// Data_<SpDByte>* Data_<SpDPtr>::NeOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDPtr>::LeOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDComplex>::LeOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDComplexDbl>::LeOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDPtr>::LtOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDComplex>::LtOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDComplexDbl>::LtOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDPtr>::GeOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDComplex>::GeOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDComplexDbl>::GeOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDPtr>::GtOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDComplex>::GtOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDComplexDbl>::GtOp( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDString>::AddInv( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDPtr>::Add( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDString>::Sub( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDString>::SubInv( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDPtr>::Sub( BaseGDL* r);
template<> 
BaseGDL* Data_<SpDPtr>::SubInv( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::LtMark( BaseGDL* r);
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::LtMark( BaseGDL* r);
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::LtMark( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::LtMark( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::GtMark( BaseGDL* r);
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::GtMark( BaseGDL* r);
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::GtMark( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::GtMark( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::Mult( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::Mult( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::Div( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::DivInv( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::Div( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::DivInv( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::Mod( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::ModInv( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::Mod( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::ModInv( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::Mod( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::ModInv( BaseGDL* r);
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::Mod( BaseGDL* r);
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::Mod( BaseGDL* r);
template<> 
Data_<SpDComplex>* Data_<SpDComplex>::ModInv( BaseGDL* r);
template<> 
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModInv( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::Mod( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::ModInv( BaseGDL* r);
template<> 
Data_<SpDObj>* Data_<SpDObj>::Mod( BaseGDL* r);
template<> 
Data_<SpDObj>* Data_<SpDObj>::ModInv( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::Pow( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::PowInv( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::PowInt( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::PowIntNew( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::PowInt( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::PowIntNew( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::PowInt( BaseGDL* r);
template<> 
Data_<SpDFloat>* Data_<SpDFloat>::PowIntNew( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::Pow( BaseGDL* r);
template<> 
Data_<SpDDouble>* Data_<SpDDouble>::PowInv( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::Pow( BaseGDL* r);
template<> 
Data_<SpDString>* Data_<SpDString>::PowInv( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::Pow( BaseGDL* r);
template<> 
Data_<SpDPtr>* Data_<SpDPtr>::PowInv( BaseGDL* r);
template<> 
Data_<SpDObj>* Data_<SpDObj>::Pow( BaseGDL* r);
template<> 
Data_<SpDObj>* Data_<SpDObj>::PowInv( BaseGDL* r);
template<>
Data_<SpDString>* Data_<SpDString>::MatrixOp( BaseGDL* r, bool atranspose, bool btranspose);
template<>
Data_<SpDPtr>* Data_<SpDPtr>::MatrixOp( BaseGDL* r, bool atranspose, bool btranspose);
template<> 
Data_<SpDByte>* Data_<SpDFloat>::LogNeg();
template<> 
Data_<SpDByte>* Data_<SpDDouble>::LogNeg();
template<> 
Data_<SpDByte>* Data_<SpDString>::LogNeg();
template<> 
Data_<SpDByte>* Data_<SpDComplex>::LogNeg();
template<> 
Data_<SpDByte>* Data_<SpDComplexDbl>::LogNeg();

// datatypes.cpp


// template<class Sp>
// bool Data_<Sp>::OutOfRangeOfInt() const 
template<>
bool Data_<SpDString>::OutOfRangeOfInt() const; 
template<>
bool Data_<SpDByte>::OutOfRangeOfInt() const;
template<>
bool Data_<SpDComplex>::OutOfRangeOfInt() const;
template<>
bool Data_<SpDComplexDbl>::OutOfRangeOfInt() const; 

/*template<> 
void* Data_<SpDString>::DataAddr();//SizeT);*/
// patch [1561592]
template<>
Data_<SpDFloat>::Data_(const dimension& dim_, BaseGDL::InitType iT, DDouble, DDouble);
template<>
Data_<SpDComplex>::Data_(const dimension& dim_, BaseGDL::InitType iT, DDouble, DDouble);
template<>
Data_<SpDString>::Data_(const dimension& dim_, BaseGDL::InitType iT, DDouble, DDouble);
template<>
Data_<SpDPtr>::Data_(const dimension& dim_, BaseGDL::InitType iT, DDouble, DDouble);
template<>
Data_<SpDObj>::Data_(const dimension& dim_, BaseGDL::InitType iT, DDouble, DDouble);

template<>  SizeT Data_<SpDString>::NBytes() const;
// template<>  SizeT Data_<SpDObj>::NBytes() const;
// template<>  SizeT Data_<SpDPtr>::NBytes() const;
template<>  SizeT Data_<SpDComplex>::ToTransfer() const;
template<>  SizeT Data_<SpDComplexDbl>::ToTransfer() const;
/*template<> 
Data_<SpDComplex>::Ty Data_<SpDComplex>::min() const;
template<> 
Data_<SpDComplex>::Ty Data_<SpDComplex>::max() const;
template<> 
Data_<SpDComplexDbl>::Ty Data_<SpDComplexDbl>::min() const;
template<> 
Data_<SpDComplexDbl>::Ty Data_<SpDComplexDbl>::max() const;*/
template<> 
int Data_<SpDComplex>::Scalar2Index( SizeT& st) const;
template<>  
int Data_<SpDComplexDbl>::Scalar2Index( SizeT& st) const;
template<>  
int Data_<SpDString>::Scalar2Index( SizeT& st) const;
template<>  
int Data_<SpDPtr>::Scalar2Index( SizeT& st) const;
template<>  
int Data_<SpDObj>::Scalar2Index( SizeT& st) const;
template<> 
bool Data_<SpDFloat>::True();
template<> 
bool Data_<SpDDouble>::True();
template<> 
bool Data_<SpDString>::True();
template<> 
bool Data_<SpDComplex>::True();
template<> 
bool Data_<SpDComplexDbl>::True();
template<> 
bool Data_<SpDPtr>::True();
template<> 
bool Data_<SpDObj>::True();
template<> 
int Data_<SpDString>::Sgn(); // -1,0,1
template<> 
int Data_<SpDComplex>::Sgn(); // -1,0,1
template<> 
int Data_<SpDComplexDbl>::Sgn(); // -1,0,1
template<> 
int Data_<SpDPtr>::Sgn(); // -1,0,1
template<> 
int Data_<SpDObj>::Sgn(); // -1,0,1
template<> 
bool Data_<SpDComplex>::ForCondUp( BaseGDL*);
template<> 
bool Data_<SpDComplex>::ForAddCondUp( BaseGDL* loopInfo);
// bool Data_<SpDComplex>::ForAddCondUp( ForLoopInfoT& loopInfo);
template<>
bool Data_<SpDComplex>::ForCondDown( BaseGDL*);
template<> 
bool Data_<SpDComplexDbl>::ForAddCondUp( BaseGDL* loopInfo);
// bool Data_<SpDComplexDbl>::ForAddCondUp( ForLoopInfoT& loopInfo);
template<>
bool Data_<SpDComplexDbl>::ForCondUp( BaseGDL*);
template<>
bool Data_<SpDComplexDbl>::ForCondDown( BaseGDL*);
template<> 
void Data_<SpDFloat>::DecAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDFloat>::IncAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDDouble>::DecAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDDouble>::IncAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDComplex>::DecAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDComplex>::IncAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDComplexDbl>::DecAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDComplexDbl>::IncAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDString>::DecAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDPtr>::DecAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDObj>::DecAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDString>::IncAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDPtr>::IncAt( ArrayIndexListT* ixList); 
template<> 
void Data_<SpDObj>::IncAt( ArrayIndexListT* ixList); 
template<> 
bool Data_<SpDFloat>::LogTrue();
template<> 
bool Data_<SpDDouble>::LogTrue();
template<> 
bool Data_<SpDString>::LogTrue();
template<> 
bool Data_<SpDComplex>::LogTrue();
template<> 
bool Data_<SpDComplexDbl>::LogTrue();
template<> 
bool Data_<SpDPtr>::LogTrue();
template<> 
bool Data_<SpDObj>::LogTrue();
template<> 
bool Data_<SpDFloat>::LogTrue(SizeT i);
template<> 
bool Data_<SpDDouble>::LogTrue(SizeT i);
template<> 
bool Data_<SpDString>::LogTrue(SizeT i);
template<> 
bool Data_<SpDComplex>::LogTrue(SizeT i);
template<> 
bool Data_<SpDComplexDbl>::LogTrue(SizeT i);
template<> 
bool Data_<SpDPtr>::LogTrue(SizeT i);
template<> 
bool Data_<SpDObj>::LogTrue(SizeT i);
template<> 
DLong* Data_<SpDFloat>::Where( bool, SizeT&);
template<> 
DLong* Data_<SpDDouble>::Where( bool, SizeT&);
template<> 
DLong* Data_<SpDString>::Where( bool, SizeT&);
template<> 
DLong* Data_<SpDComplex>::Where( bool, SizeT&);
template<> 
DLong* Data_<SpDComplexDbl>::Where( bool, SizeT&);

template<>  
std::ostream& Data_<SpDLong>::ToStream(std::ostream& o, SizeT w, SizeT* actPosPtr); 
template<>  
std::ostream& Data_<SpDULong>::ToStream(std::ostream& o, SizeT w, SizeT* actPosPtr); 
template<>  
std::ostream& Data_<SpDPtr>::ToStream(std::ostream& o, SizeT w, SizeT* actPosPtr); 
template<>  
std::ostream& Data_<SpDObj>::ToStream(std::ostream& o, SizeT w, SizeT* actPosPtr); 
template<>  
std::ostream& Data_<SpDFloat>::ToStream(std::ostream& o, SizeT w, SizeT* actPosPtr); 
template<>  
std::ostream& Data_<SpDDouble>::ToStream(std::ostream& o, SizeT w, SizeT* actPosPtr); 
template<>  
std::ostream& Data_<SpDComplex>::ToStream(std::ostream& o, SizeT w, SizeT* actPosPtr); 
template<>  
std::ostream& Data_<SpDComplexDbl>::ToStream(std::ostream& o, SizeT w, SizeT* actPosPtr); 
template<>  
std::ostream& Data_<SpDByte>::ToStream(std::ostream& o, SizeT w, SizeT* actPosPtr); 
template<>  
std::ostream& Data_<SpDString>::ToStream(std::ostream& o, SizeT w, SizeT* actPosPtr); 
template<> 
std::ostream& Data_<SpDString>::Write( std::ostream& os, bool swapEndian, 
				       bool compress, XDR *xdrs);
template<> 
std::istream& Data_<SpDString>::Read( std::istream& os, bool swapEndian,
				      bool compress, XDR *xdrs);

// ifmt.cpp
template<>  SizeT Data_<SpDString>::
IFmtA( std::istream* is, SizeT offs, SizeT r, int w); 
template<>  SizeT Data_<SpDInt>::
IFmtA( std::istream* is, SizeT offs, SizeT r, int w);
template<>  SizeT Data_<SpDLong>::
IFmtA( std::istream* is, SizeT offs, SizeT r, int w);
template<>  SizeT Data_<SpDLong64>::
IFmtA( std::istream* is, SizeT offs, SizeT r, int w);
template<>  SizeT Data_<SpDFloat>::
IFmtA( std::istream* is, SizeT offs, SizeT r, int w);
template<>  SizeT Data_<SpDDouble>::
IFmtA( std::istream* is, SizeT offs, SizeT r, int w);
template<>  SizeT Data_<SpDComplex>::
IFmtA( std::istream* is, SizeT offs, SizeT r, int w); 
template<>  SizeT Data_<SpDComplexDbl>::
IFmtA( std::istream* is, SizeT offs, SizeT r, int w); 
template<>  SizeT Data_<SpDString>::
IFmtI( std::istream* is, SizeT offs, SizeT r, int w,
       BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDULong>::
IFmtI( std::istream* is, SizeT offs, SizeT r, int w,
       BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDComplex>::
IFmtI( std::istream* is, SizeT offs, SizeT r, int w, 
       BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDComplexDbl>::
IFmtI( std::istream* is, SizeT offs, SizeT r, int w, 
       BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDString>::
IFmtF( std::istream* is, SizeT offs, SizeT r, int w);
template<>  SizeT Data_<SpDFloat>::
IFmtF( std::istream* is, SizeT offs, SizeT r, int w);
template<>  SizeT Data_<SpDDouble>::
IFmtF( std::istream* is, SizeT offs, SizeT r, int w);
template<>  SizeT Data_<SpDComplex>::
IFmtF( std::istream* is, SizeT offs, SizeT r, int w); 
template<>  SizeT Data_<SpDComplexDbl>::
IFmtF( std::istream* is, SizeT offs, SizeT r, int w); 

template<>  SizeT Data_<SpDString>::
IFmtCal( std::istream* is, SizeT offs, SizeT r, int width, BaseGDL::Cal_IOMode cMode);
template<>  SizeT Data_<SpDComplex>::
IFmtCal( std::istream* is, SizeT offs, SizeT r, int width, BaseGDL::Cal_IOMode cMode);
template<>  SizeT Data_<SpDComplexDbl>::
IFmtCal( std::istream* is, SizeT offs, SizeT r, int width, BaseGDL::Cal_IOMode cMode);

// ofmt.cpp
template<>  SizeT Data_<SpDString>::OFmtA( std::ostream* os, SizeT offs, SizeT r, int w, int code); 
template<>  SizeT Data_<SpDComplex>::OFmtA( std::ostream* os, SizeT offs, SizeT r, int w, int code); 
template<>  SizeT Data_<SpDComplexDbl>::OFmtA( std::ostream* os, SizeT offs, SizeT r, int w, int code); 
template<>  SizeT Data_<SpDDouble>::
OFmtF( std::ostream* os, SizeT offs, SizeT r, int w, int d, int code, BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDFloat>::
OFmtF( std::ostream* os, SizeT offs, SizeT r, int w, int d, int code, BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDComplex>::
OFmtF( std::ostream* os, SizeT offs, SizeT r, int w, int d, int code, BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDComplexDbl>::
OFmtF( std::ostream* os, SizeT offs, SizeT r, int w, int d, int code, BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDLong>::
OFmtI( std::ostream* os, SizeT offs, SizeT r, int w, int d, int code, BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDULong>::
OFmtI( std::ostream* os, SizeT offs, SizeT r, int w, int d, int code, BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDLong64>::
OFmtI( std::ostream* os, SizeT offs, SizeT r, int w, int d, int code, BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDULong64>::
OFmtI( std::ostream* os, SizeT offs, SizeT r, int w, int d, int code, BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDComplex>::
OFmtI( std::ostream* os, SizeT offs, SizeT r, int w, int d, int code, BaseGDL::IOMode oMode); 
template<>  SizeT Data_<SpDComplexDbl>::
OFmtI( std::ostream* os, SizeT offs, SizeT r, int w, int d, int code, BaseGDL::IOMode oMode); 

// template<>
// void Data_< SpDString>::Construct(); 
// template<>
// void Data_< SpDComplex>::Construct(); 
// template<>
// void Data_< SpDComplexDbl>::Construct(); 
template<>
void Data_< SpDPtr>::Construct(); 
template<>
void Data_< SpDObj>::Construct(); 
// template<>
// void Data_< SpDString>::ConstructTo0(); 
// template<>
// void Data_< SpDComplex>::ConstructTo0(); 
// template<>
// void Data_< SpDComplexDbl>::ConstructTo0(); 
// template<>
// void Data_< SpDString>::Destruct(); 
// template<>
// void Data_< SpDComplex>::Destruct(); 
// template<>
// void Data_< SpDComplexDbl>::Destruct(); 
template<>
void Data_< SpDPtr>::Destruct(); 
template<>
void Data_< SpDObj>::Destruct(); 

// GetAsIndex/GetAsIndexStrict
  template<>
SizeT Data_<SpDInt>::GetAsIndex( SizeT i) const;
template<>
SizeT Data_<SpDInt>::GetAsIndexStrict( SizeT i) const;
template<>
SizeT Data_<SpDLong>::GetAsIndex( SizeT i) const;
template<>
SizeT Data_<SpDLong>::GetAsIndexStrict( SizeT i) const;
template<>
SizeT Data_<SpDLong64>::GetAsIndex( SizeT i) const;
template<>
SizeT Data_<SpDLong64>::GetAsIndexStrict( SizeT i) const;
template<>
SizeT Data_<SpDFloat>::GetAsIndex( SizeT i) const;
template<>
SizeT Data_<SpDFloat>::GetAsIndexStrict( SizeT i) const;
template<>
SizeT Data_<SpDDouble>::GetAsIndex( SizeT i) const;
template<>
SizeT Data_<SpDDouble>::GetAsIndexStrict( SizeT i) const;
template<>
SizeT Data_<SpDString>::GetAsIndex( SizeT i) const;
template<>
SizeT Data_<SpDString>::GetAsIndexStrict( SizeT i) const;
template<>
SizeT Data_<SpDComplex>::GetAsIndex( SizeT i) const;
template<>
SizeT Data_<SpDComplex>::GetAsIndexStrict( SizeT i) const;
template<>
SizeT Data_<SpDComplexDbl>::GetAsIndex( SizeT i) const;
template<>
SizeT Data_<SpDComplexDbl>::GetAsIndexStrict( SizeT i) const;




template<>
BaseGDL* Data_<SpDString>::CShift( DLong d) const;
template<>
BaseGDL* Data_<SpDPtr>::CShift( DLong d) const;
template<>
BaseGDL* Data_<SpDObj>::CShift( DLong d) const;


// topython.cpp
#ifdef USE_PYTHON
template<> 
PyObject* Data_<SpDByte>::ToPythonScalar();
template<> 
PyObject* Data_<SpDInt>::ToPythonScalar();
template<> 
PyObject* Data_<SpDUInt>::ToPythonScalar();
template<> 
PyObject* Data_<SpDLong>::ToPythonScalar();
template<> 
PyObject* Data_<SpDULong>::ToPythonScalar();
template<> 
PyObject* Data_<SpDFloat>::ToPythonScalar();
template<> 
PyObject* Data_<SpDDouble>::ToPythonScalar();
template<> 
PyObject* Data_<SpDComplex>::ToPythonScalar();
template<> 
PyObject* Data_<SpDComplexDbl>::ToPythonScalar();
template<> 
PyObject* Data_<SpDString>::ToPythonScalar();
#endif
