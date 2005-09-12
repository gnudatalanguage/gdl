/***************************************************************************
                          |FILENAME|  -  description
                             -------------------
    begin                : |DATE|
    copyright            : (C) |YEAR| by |AUTHOR|
    email                : |EMAIL|
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

#include <string>

#include <gsl/gsl_sf.h>
#include <gsl/gsl_sf_laguerre.h>

#include "datatypes.hpp"
#include "envt.hpp"

#define GDL_DEBUG

namespace lib {

  using namespace std;
  using namespace antlr;

   BaseGDL* laguerre(EnvT* e)
  {
    SizeT nParam=e->NParam(2); //, "LAGUERRE");
    DUInt count;

    BaseGDL* xvals,* nval,* kval;

    SizeT nEx,nEn, nEk;
    nEn=0;
    nEk=0;

    xvals= e->GetParDefined(0); //,"LAGUERRE");
    nEx=xvals->N_Elements();
    if(nEx == 0)
      e->Throw( 
               "Variable is undefined: "
               +e->GetParString(0));
    
    nval=e->GetParDefined(1); //,"LAGUERRE");
    nEn=nval->N_Elements();
    if(nEn == 0 || nEn != 1)
      e->Throw( nEn?"N and K must be scalars.":
               "Variable is undefined: "
               +e->GetParString(1));
    
    if(nParam > 2)
      {
        kval=e->GetParDefined(2); //,"LAGUERRE");
        nEk=kval->N_Elements();
      } else {
        kval=new DDoubleGDL(0);
        nEk=1;
        e->Guard(kval);
      }

    if(nEk == 0 || nEk != 1)
      e->Throw( nEk?"N and K must be scalars.":
               "Variable is undefined: "
               +e->GetParString(2));
   
    if (xvals->Type() == STRING) {
      e->Throw( 
               "String expression not allowed in this context: "
               +e->GetParString(0));
    } else if (xvals->Type() == PTR) {
      e->Throw( 
               "Pointer expression not allowed in this context: "
               +e->GetParString(0));
    } else if (xvals->Type() == OBJECT) {
      e->Throw( 
               "Object expression not allowed in this context: "
               +e->GetParString(0));
    } else if (xvals->Type() == STRUCT) {
      e->Throw( 
               "Struct expression not allowed in this context: "
               +e->GetParString(0));
    } else if(xvals->Type() == COMPLEX ||
              xvals->Type() == COMPLEXDBL) {
      e->Throw( 
               "Complex Laguerre not implemented: ");
    }        else      {
      //byte, int, long float, double, uint, ulong, int64, uint64
      
      DDoubleGDL* res;
      DDoubleGDL* x_cast,*k_cast;
      DIntGDL* n_cast;

      if(xvals->Type() == DOUBLE) 
        x_cast=  static_cast<DDoubleGDL*>(xvals);
      else
        {
        x_cast=  static_cast<DDoubleGDL*>(xvals->Convert2(DOUBLE,BaseGDL::COPY));
        e->Guard( x_cast);
        }

      //nval check
      if (nval->Type() == STRING)
        e->Throw( 
                 "String expression not allowed in this context: "
                 +e->GetParString(1));
      else if (nval->Type() == PTR)
        e->Throw( 
                 "Pointer expression not allowed in this context: "
                 +e->GetParString(1));
      else if (nval->Type() == OBJECT) 
        e->Throw( 
                 "Object expression not allowed in this context: "
                 +e->GetParString(1));
      else if (nval->Type() == STRUCT) 
        e->Throw( 
                 "Struct expression not allowed in this context: "
                 +e->GetParString(1));
      else if(nval->Type() == COMPLEX ||
              nval->Type() == COMPLEXDBL) 
        e->Throw( 
                 "Complex Legendre not implemented: ");
      else if(nval->Type() == INT)
        n_cast=static_cast<DIntGDL*>(nval);
      else
        {
          n_cast=static_cast<DIntGDL*>(nval->Convert2(INT,BaseGDL::COPY));
          e->Guard( n_cast);
        }

      //kval check
      if (kval->Type() == STRING)
        e->Throw( 
                 "String expression not allowed in this context: "
                 +e->GetParString(2));
      else if (kval->Type() == PTR)
        e->Throw( 
                 "Pointer expression not allowed in this context: "
                 +e->GetParString(2));
      else if (kval->Type() == OBJECT) 
        e->Throw( 
                 "Object expression not allowed in this context: "
                 +e->GetParString(2));
      else if (kval->Type() == STRUCT) 
        e->Throw( 
                 "Struct expression not allowed in this context: "
                 +e->GetParString(2));
      else if(kval->Type() == COMPLEX ||
              kval->Type() == COMPLEXDBL) 
        e->Throw( 
                 "Complex Laguerre not implemented: ");
      else if(kval->Type() == DOUBLE)
        k_cast=static_cast<DDoubleGDL*>(kval);
      else
        {
          k_cast=static_cast<DDoubleGDL*>(kval->Convert2(DOUBLE,BaseGDL::COPY));
          e->Guard( k_cast);
        }

      //n,k,x are converted to the correct format (int, double, double) here
            
      if( nval < 0)
        e->Throw( "Argument N must be greater than or equal to zero.");
      if( kval < 0)
        e->Throw( "Argument K must be greater than or equal to zero.");

      res=new DDoubleGDL(xvals->Dim(),BaseGDL::NOZERO);
      DDouble k = (*k_cast)[0];
      DInt n = (*n_cast)[0];
      DDouble xNow;
        
      for (count=0;count<nEx;++count)
        {
          xNow = (*x_cast)[count];         
          (*res)[count] = gsl_sf_laguerre_n(n,k,xNow);
        }

      static DInt doubleKWIx = e->KeywordIx("DOUBLE");
      static DInt coefKWIx = e->KeywordIx("COEFFICIENTS");   
    
      if( e->KeywordPresent( coefKWIx)) {
        double dcount;
        double gamma_kn1 = gsl_sf_gamma(k+n+1.);
        DDoubleGDL* coefKW = new DDoubleGDL(dimension(n+1) , BaseGDL::NOZERO);

        for(count=0;count<=n;++count)
          {
            dcount = static_cast<double>(count);
            (*coefKW)[count] = ((count & 0x0001)?-1.0:1.0)*gamma_kn1/
	      (gsl_sf_gamma(n-dcount+1.)*gsl_sf_gamma(k+dcount+1.)*
	       gsl_sf_gamma(dcount+1.));
          }
        
        if(xvals->Type() != DOUBLE && !e->KeywordSet(doubleKWIx))
          coefKW = static_cast<DDoubleGDL*>(coefKW->
					    Convert2(FLOAT,BaseGDL::CONVERT));
        e->SetKW( coefKWIx, coefKW);
      }

      //convert things back
      if(xvals->Type() != DOUBLE && !e->KeywordSet(doubleKWIx))
        {
          return res->Convert2(FLOAT,BaseGDL::CONVERT);
        }
      else
        {
          return res;
        }
    }
    return new DByteGDL(0);
  }
}
