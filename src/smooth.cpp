/***************************************************************************
                          smooth.cpp  -  mathematical GDL library function
                             -------------------
    begin                : 05 September 2014
    copyright            : (C) 2014 by Levan Loria  (with Alain Coulais)
    email                : alaingdl@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"

#include "datatypes.hpp"
#include "envt.hpp"

#include "smooth.hpp"

namespace lib {

using namespace std;
using namespace antlr;

BaseGDL* smooth_fun( EnvT* e)
  {
    long nParam=e->NParam( 2); 

    /************************************Checking_parameters************************************************/

    BaseGDL* p0 = e->GetNumericParDefined(0);
    int rank=p0->Rank();
    if (rank == 0)
      e->Throw("Expression must be an array in this context: " +
      e->GetParString(0));

    //smooth width should have proper dimensions, etc and be odd
    DLong width[MAXRANK];
    
    BaseGDL* p1 = e->GetNumericParDefined(1);
    if (p1->Rank() != 0) {
      if (rank != p1->N_Elements())
        e->Throw("Number of Array dimensions does not match number of Widths.");

      DLongGDL* gdlwidth = e->GetParAs<DLongGDL>(1);
      for (long r = 0; r < rank; ++r) {
        width[r] = (*gdlwidth)[r];
        if (((width[r]) % 2) == 0) width[r] += 1;
      }
    } else {
      DLongGDL* gdlwidth = e->GetParAs<DLongGDL>(1);
      for (long r = 0; r < rank; ++r) {
        width[r] = (*gdlwidth)[0];
        if (((width[r]) % 2) == 0) width[r] += 1;
      }
    }
     for (long r = 0; r < rank; ++r) if (p0->Dim(r) < width[r])  e->Throw("Width must be nonnegative and smaller than array dimensions");
    /********************************************Arguments_treatement***********************************/

    // overrides EDGE_NOTHING
    static int edge_wrapIx = e->KeywordIx( "EDGE_WRAP");
    bool edge_wrap = e->KeywordSet( edge_wrapIx);
    static int edge_truncateIx = e->KeywordIx( "EDGE_TRUNCATE");
    bool edge_truncate = e->KeywordSet( edge_truncateIx);
    static int edge_zeroIx = e->KeywordIx( "EDGE_ZERO");
    bool edge_zero = e->KeywordSet( edge_zeroIx);
    static int edge_mirrorIx = e->KeywordIx( "EDGE_MIRROR");
    bool edge_mirror = e->KeywordSet( edge_mirrorIx);
    int edgeMode = 0; 
    if( edge_wrap)
      edgeMode = 1;
    else if( edge_truncate)
      edgeMode = 2;
    else if( edge_zero)
      edgeMode = 3;
    else if(edge_mirror)
      edgeMode = 4;
    
    /***********************************Parameter NAN****************************************/

    static int nanIx = e->KeywordIx( "NAN");
    bool doNan = e->KeywordSet( nanIx);
    
    /***********************************Parameter MISSING************************************/
    static int missingIx = e->KeywordIx("MISSING");
    bool doMissing = e->KeywordSet(missingIx);
    BaseGDL* missing;
    Guard<BaseGDL> missGuard;
    if (doMissing) {
      missing = e->GetKW(missingIx);
      if (p0->Type() != missing->Type()) {
        missing = missing->Convert2(p0->Type(), BaseGDL::COPY);
        missGuard.Reset(missing);
      }
    } else missing = p0->New(1, BaseGDL::ZERO);
    
    //Nan can be ignored for non floating point data:
    if (IntType(p0->Type())) doNan=false;
    
    if (!doNan) doMissing=false;
    if (!doMissing && p0->Type()==GDL_FLOAT) {
      DFloat tmp=std::numeric_limits<float>::quiet_NaN();
      memcpy((*missing).DataAddr(), &tmp, sizeof(tmp));
    }
    if (!doMissing && p0->Type()==GDL_DOUBLE){
      DDouble tmp=std::numeric_limits<double>::quiet_NaN();
      memcpy((*missing).DataAddr(), &tmp, sizeof(tmp));
    }
    //populating a Complex with Nans is not easy as there is no objective method for that.
    if (!doMissing && p0->Type()==GDL_COMPLEX) {
      DComplex tmp;
      DFloat tmpNaN;
      tmpNaN=std::numeric_limits<float>::quiet_NaN();
      tmp=complex<float>(tmpNaN,tmpNaN);
      // real(tmp)=std::numeric_limits<double>::quiet_NaN();
      // imag(tmp)=std::numeric_limits<double>::quiet_NaN();
      memcpy((*missing).DataAddr(), &tmp, sizeof(tmp));
    }
    if (!doMissing && p0->Type()==GDL_COMPLEXDBL) {
      DComplexDbl tmp;
      DDouble tmpNaN;
      tmpNaN=std::numeric_limits<double>::quiet_NaN();
      tmp=complex<double>(tmpNaN,tmpNaN);
      // tmp.real()=std::numeric_limits<double>::quiet_NaN();
      // tmp.imag()=std::numeric_limits<double>::quiet_NaN();
      memcpy((*missing).DataAddr(), &tmp, sizeof(tmp));
    }

    return p0->Smooth( width, edgeMode, doNan, missing);
  } //end of smooth_fun

  }//end of namespace
