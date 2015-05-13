/***************************************************************************

                          basic_fun.cpp  -  basic GDL library function

                             -------------------

    begin                : March 14 2004

    copyright            : (C) 2004 by Christopher Lee

    email                : leec_gdl@publius.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#else
// default: assume we have netCDF
#define USE_NETCDF 1
// default: assume we have ImageMagick
#define USE_MAGICK 1
#endif

#include "includefirst.hpp"

#include <string>
#include <fstream>
#include <memory>

#include <gsl/gsl_sys.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

#include "initsysvar.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
//#include "dpro.hpp"
//#include "dinterpreter.hpp"
#include "basic_fun_cl.hpp"
//#include "terminfo.hpp"

#define MAX_DATE_STRING_LENGTH 80

#ifdef _MSC_VER
#include "gtdhelper.hpp"
#else
#include <sys/time.h>
#endif

namespace lib {

  using namespace std;
  using namespace antlr;

  BaseGDL* magick_exists(EnvT *e)
  {
#ifdef USE_MAGICK
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* ncdf_exists(EnvT* e)
  {
#ifdef USE_NETCDF
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }

  BaseGDL* gshhg_exists( EnvT* e )
  {
#ifdef USE_GSHHS
    //    e->Message( "GDL was compiled with support for GSHHG" );
    return new DIntGDL(1);
#else
    //e->Message( "GDL was compiled without support for GSHHG" );
    return new DIntGDL(0);
#endif
  }
  
  BaseGDL* proj4_exists( EnvT* e )
  {
#if defined(USE_LIBPROJ4)
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }
  
  BaseGDL* proj4new_exists( EnvT* e )
  {
#if defined(USE_LIBPROJ4_NEW)
    return new DIntGDL(1);
#else
    return new DIntGDL(0);
#endif
  }


  double Gregorian2Julian(struct tm *ts)
  {
    double jd;
    // SA: gives bad results, e.g.: 
    // IDL> print, systime(/julian), f='(G)'
    //    2454995.131712963
    // GDL> print, systime(/julian), f='(G)'
    //    2454994.527534722
    //
    // jd = 367.0*(1900.+ts->tm_year) 
    //   - (7.0*((1900.+ts->tm_year) + ((1+ts->tm_mon+9.0)/12.0))/4.0)
    //   + (275.0*(1+ts->tm_mon)/9.0)+ts->tm_mday 
    //   + (ts->tm_hour + (ts->tm_min + ts->tm_sec/60.0)/60.0)/24.0 
    //   + 1721013.5;
    //
    // SA: an alterntive from the NOVAS library 
    //     (http://aa.usno.navy.mil/software/novas/novas_c/novasc_info.php)
    jd = ts->tm_mday - 32075L + 1461L * (ts->tm_year + 1900 + 4800L
      + (1 + ts->tm_mon - 14L) / 12L) / 4L 
      + 367L * (1 + ts->tm_mon - 2L - (1 + ts->tm_mon - 14L) / 12L * 12L) 
      / 12L - 3L * ((1900 + ts->tm_year + 4900L + (1 + ts->tm_mon - 14L) / 12L) 
      / 100L) / 4L
      + (ts->tm_hour + (ts->tm_min + ts->tm_sec/60.0)/60.0)/24.0 - .5;
    // SA: end of modifications, the code below was here before
    
    if ((100.0*(1900.+ts->tm_year)  + 1+ts->tm_mon - 190002.5) < 0) jd=jd+1.0;

    return jd;

  }

  BaseGDL* systime(EnvT* e)
  {
    struct timeval tval;
    struct timezone tzone;

    /*get the time before doing anything else, 
      this hopefully gives a more meaningful "time"
      than if the t=time(0) call came after an 
      arbitary number of conditional statements.*/
    //    cout << "lib::systime: " << t << endl;
    gettimeofday(&tval,&tzone);
    double tt = tval.tv_sec+tval.tv_usec/1e+6; // time in UTC seconds

    SizeT nParam=e->NParam(0); //,"SYSTIME");
    bool ret_seconds=false;

    Guard<BaseGDL> v_guard;
    Guard<BaseGDL> v1_guard;

    if (nParam == 1) {
      //1 parameter, 
      //      1->current UTC time seconds
      //      default
      DLong v=0; 
      e->AssureLongScalarPar(0,v);

      //    DIntGDL* v = static_cast<DIntGDL*>(e->GetParDefined(0)->Convert2(GDL_INT,BaseGDL::COPY));
      //v_guard.Reset( v); //  e->Guard(v);
      //if ( (*v)[0] == 1) //->EqualNoDelete( static_cDIntGDL(1)))

      // AC 15/05/14 : in fact, the range goes in ]-1,1[
      if ((v <= -1) || (v >= 1)) ret_seconds=true;

    }

    if (nParam == 2) {
      if (e->KeywordSet("JULIAN")) e->Throw("Conflicting keywords.");

      //2 parameters
      //if the first param is 0, return the date of the second arg
      //if the first param is 1, return the 'double' of the second arg
      DLong v1=0; 
      e->AssureLongScalarPar(0,v1);
      DDouble v2=0.0; 
      e->AssureDoubleScalarPar(1,v2);

      //     DIntGDL* v1 = static_cast<DIntGDL*>(e->GetParDefined(0)->Convert2(GDL_INT,BaseGDL::COPY));
      //v_guard.Reset( v1); //  e->Guard(v1);
      //DDoubleGDL* v2 = static_cast<DDoubleGDL*>(e->GetParDefined(1)->Convert2(GDL_DOUBLE,BaseGDL::COPY));

      if (v1 == 0) { //v1->EqualNoDelete( DIntGDL(0))) { //0, read the second argument as time_t;
	tval.tv_sec = static_cast<long int>(v2);
        tval.tv_usec = static_cast<long int>((v2-tval.tv_sec)*1e+6);
        //delete v2; // we delete v2 here as it is not guarded. Avoids a "new" in the following "else"
      } else { //1
        return new DDoubleGDL(v2);
      }
    }

    //return the variable in seconds, either JULIAN, JULIAN+UTC,
    //or no other keywords
    struct tm *tstruct;
    if( ret_seconds || e->KeywordSet("SECONDS") )
      {
       if( e->KeywordSet("JULIAN") )
         {
           if( e->KeywordSet("UTC") )
             tstruct=gmtime((time_t *)&tval.tv_sec);
           else
             tstruct=localtime((time_t *)&tval.tv_sec);

           return new DDoubleGDL(Gregorian2Julian(tstruct));
         }
       else
         {
           // does not (necessaryly) work: time might count backwards
           //double tickTime = static_cast<double>(t) + tt - floor( tt);
           return new DDoubleGDL(static_cast<double>(tt));
         }
      }

    //return a string of the time, either UTC or local (default)
    if(e->KeywordSet("UTC"))
      tstruct= gmtime((time_t *)&tval.tv_sec);
    else
      tstruct= localtime((time_t *)&tval.tv_sec);

    //Convert the time to JULIAN or NOT
    if(e->KeywordSet("JULIAN"))
        return new DDoubleGDL(Gregorian2Julian(tstruct));
    else 
      {
       char st[MAX_DATE_STRING_LENGTH];
//        char *st=new char[MAX_DATE_STRING_LENGTH];
//        ArrayGuard<char> stGuard( st);
       const char *format="%a %h %d %T %Y";//my IDL date format.
       DStringGDL *S;
#ifdef _WIN32
	S = new DStringGDL(asctime(tstruct));
#else
       SizeT res=strftime(st,MAX_DATE_STRING_LENGTH,format,tstruct);

       if(res != 0)
         S=new DStringGDL(st);
       else
         S=new DStringGDL("");
#endif

       return S;
      }
  }

  BaseGDL* legendre(EnvT* e)
  {
    Guard<BaseGDL> x_guard;
    Guard<BaseGDL> l_guard;
    Guard<BaseGDL> m_guard;

    SizeT nParam=e->NParam(2); //, "LEGENDRE");
    Guard<BaseGDL> guard;
    int count;
    
    
    BaseGDL* xvals,* lvals,* mvals;

    xvals= e->GetParDefined(0); //,"LEGENDRE");
    
    SizeT nEx,nEl, nEm,nmin;
    nEl=0;
    nEm=0;

    
    nEx=xvals->N_Elements();
    if(nEx == 0)
      e->Throw( 
	       "Variable is undefined: "
	       +e->GetParString(0));
    
    lvals=e->GetParDefined(1); //,"LEGENDRE");
    nEl=lvals->N_Elements();
    if(nEl == 0)
      e->Throw( 
	       "Variable is undefined: "
	       +e->GetParString(1));
    
    
    if(nParam > 2)
      {
	mvals=e->GetParDefined(2); //,"LEGENDRE");
	nEm=mvals->N_Elements();
      } else {
	mvals=new DIntGDL(0);
	nEm=1;
	guard.Reset(mvals);
      }

    if(nEm == 0)
      e->Throw( 
	       "Variable is undefined: "
	       +e->GetParString(2));

    
    nmin=nEx;
    if(nEl < nmin && nEl > 1) 	nmin=nEl;    
    if(nEm < nmin && nEm > 1) 	nmin=nEm;
    
    if (xvals->Type() == GDL_STRING) {
      e->Throw( 
	       "String expression not allowed in this context: "
	       +e->GetParString(0));
    } else if (xvals->Type() == GDL_PTR) {
      e->Throw( 
	       "Pointer expression not allowed in this context: "
	       +e->GetParString(0));
    } else if (xvals->Type() == GDL_OBJ) {
      e->Throw( 
	       "Object expression not allowed in this context: "
	       +e->GetParString(0));
    } else if (xvals->Type() == GDL_STRUCT) {
      e->Throw( 
	       "Struct expression not allowed in this context: "
	       +e->GetParString(0));
    } else if(xvals->Type() == GDL_COMPLEX ||
	      xvals->Type() == GDL_COMPLEXDBL) {
      e->Throw( 
	       "Complex Legendre not implemented: ");
    }        else      {
      //byte, int, long float, double, uint, ulong, int64, uint64

      DDoubleGDL* res;
      DDoubleGDL* x_cast;
      DIntGDL* l_cast,*m_cast;

      if(xvals->Type() == GDL_DOUBLE) 
	x_cast=  static_cast<DDoubleGDL*>(xvals);
      else
	{
	x_cast=  static_cast<DDoubleGDL*>(xvals->Convert2(GDL_DOUBLE,BaseGDL::COPY));
	x_guard.Reset(x_cast);//e->Guard( x_cast);
	}

      //lval check
      if (lvals->Type() == GDL_STRING)
	e->Throw( 
		 "String expression not allowed in this context: "
		 +e->GetParString(1));
      else if (lvals->Type() == GDL_PTR)
	e->Throw( 
		 "Pointer expression not allowed in this context: "
		 +e->GetParString(1));
      else if (lvals->Type() == GDL_OBJ) 
	e->Throw( 
		 "Object expression not allowed in this context: "
		 +e->GetParString(1));
      else if (lvals->Type() == GDL_STRUCT) 
	e->Throw( 
		 "Struct expression not allowed in this context: "
		 +e->GetParString(1));
      else if(lvals->Type() == GDL_COMPLEX ||
	      lvals->Type() == GDL_COMPLEXDBL) 
	e->Throw( 
		 "Complex Legendre not implemented: ");
      else if(lvals->Type() == GDL_INT)
	l_cast=static_cast<DIntGDL*>(lvals);
      else
	{
	  l_cast=static_cast<DIntGDL*>(lvals->Convert2(GDL_INT,BaseGDL::COPY));
	  l_guard.Reset(l_cast);//e->Guard( l_cast);
	}

      //mval check
      if (mvals->Type() == GDL_STRING)
	e->Throw( 
		 "String expression not allowed in this context: "
		 +e->GetParString(2));
      else if (mvals->Type() == GDL_PTR)
	e->Throw( 
		 "Pointer expression not allowed in this context: "
		 +e->GetParString(2));
      else if (mvals->Type() == GDL_OBJ) 
	e->Throw( 
		 "Object expression not allowed in this context: "
		 +e->GetParString(2));
      else if (mvals->Type() == GDL_STRUCT) 
	e->Throw( 
		 "Struct expression not allowed in this context: "
		 +e->GetParString(2));
      else if(mvals->Type() == GDL_COMPLEX ||
	      mvals->Type() == GDL_COMPLEXDBL) 
	e->Throw( 
		 "Complex Legendre not implemented: ");
      else if(mvals->Type() == GDL_INT)
	m_cast=static_cast<DIntGDL*>(mvals);
      else
	{
	  m_cast=static_cast<DIntGDL*>(mvals->Convert2(GDL_INT,BaseGDL::COPY));
	  //e->Guard( m_cast);
	  m_guard.Reset(m_cast);
	}

      //x,m,l are converted to the correct format (double, int, int) here
	

      //make the result array have the same size as the smallest x,m,l array
      if(nmin == nEx) res=new DDoubleGDL(xvals->Dim(),BaseGDL::NOZERO);
      else if(nmin == nEl) res=new DDoubleGDL(lvals->Dim(),BaseGDL::NOZERO);
      else if(nmin == nEm) res=new DDoubleGDL(mvals->Dim(),BaseGDL::NOZERO);
	  
      for (count=0;count<nmin;count++)
	{
	  DDouble xNow = (*x_cast)[nmin > nEx?0:count];
	  DInt lNow =    (*l_cast)[nmin > nEl?0:count];
	  DInt mNow =    (*m_cast)[nmin > nEm?0:count];

	  if( xNow < -1.0 || xNow > 1.0)
	    e->Throw( "Argument X must be in the range [-1.0, 1.0]");
	  if( lNow < 0)
	    e->Throw( "Argument L must be greater than or equal to zero.");
	  if( mNow < -lNow || mNow > lNow)
	    e->Throw( "Argument M must be in the range [-L, L].");
	 
	  if( mNow >= 0)
	    (*res)[count]= 
	      gsl_sf_legendre_Plm( lNow, mNow, xNow);
	  else
	    {
	      mNow = -mNow;
	      
	      int addIx  = lNow+mNow;
	      DDouble mul = 1.0;
	      DDouble dD  = static_cast<DDouble>( lNow-mNow+1);
	      for( int d=lNow-mNow+1; d<=addIx; ++d)
		{
		  mul *= dD;
		  dD  += 1.0;
		}

	      DDouble Pm = gsl_sf_legendre_Plm( lNow, mNow, xNow);
	      if( mNow % 2 == 1) Pm = -Pm;

	      (*res)[count] = Pm / mul;
	    }
	}
	
      //convert things back
      if(xvals->Type() != GDL_DOUBLE && !e->KeywordSet("DOUBLE"))
	{
	  return res->Convert2(GDL_FLOAT,BaseGDL::CONVERT);
	}
      else
	{
	  return res;
	}
    }
    return new DByteGDL(0);
  }

  // Gamma, LnGamma, IGamma and Beta are now in math_fun_gm.cpp
  // I rewrite them because they had many bugs (gregory.marchal_at_obspm.fr)

  BaseGDL* gsl_exp(EnvT* e)
  {
    Guard<BaseGDL> cdr_guard;
    Guard<BaseGDL> cd_guard;
    Guard<BaseGDL> d_guard;
    Guard<BaseGDL> fr_guard;


    SizeT nParam = e->NParam(1);
    BaseGDL* v=e->GetParDefined(0);   

    size_t nEl = v->N_Elements();
    size_t i;
    if (v->Type() == GDL_STRING) {
      e->Throw( 
		  "String expression not allowed in this context: "
			  +e->GetParString(0));
    } else if (v->Type() == GDL_PTR) {
      e->Throw( 
		  "Pointer expression not allowed in this context: "
			  +e->GetParString(0));
    } else if (v->Type() == GDL_OBJ) {
      e->Throw( 
		  "Object expression not allowed in this context: "
			  +e->GetParString(0));
    } else if (v->Type() == GDL_STRUCT) {
      e->Throw( 
		  "Struct expression not allowed in this context: "
			  +e->GetParString(0));		  
    } else   {
      //      DDoubleGDL* d;
      DDoubleGDL* dr = new DDoubleGDL(v->Dim(), BaseGDL::NOZERO);
      //      e->Guard( dr);

      if(v->Type() == GDL_COMPLEX) {
	DComplexDblGDL* cd=
	  static_cast<DComplexDblGDL*>(v->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
	cd_guard.Reset(cd);//e->Guard( cd);

	DComplexDblGDL* cdr =
	  new DComplexDblGDL(v->Dim(), BaseGDL::NOZERO);
	cdr_guard.Reset(cdr);//e->Guard( cdr);

	if(nEl == 1) 
	  (*cdr)[0]=
	   DComplex((gsl_sf_exp((*cd)[0].real())*cos((*cd)[0].imag())),
		    (gsl_sf_exp((*cd)[0].real())*sin((*cd)[0].imag())));
	else
	  for(i=0;i<nEl;++i) 
	    (*cdr)[i]=
	      DComplex((gsl_sf_exp((*cd)[i].real())*cos((*cd)[i].imag())),
		       (gsl_sf_exp((*cd)[i].real())*sin((*cd)[i].imag())));

	return static_cast<DComplexGDL*>(cdr->Convert2(GDL_COMPLEX,BaseGDL::COPY));

      } else if(v->Type() == GDL_COMPLEXDBL) {
	DComplexDblGDL* cd=
	  static_cast<DComplexDblGDL*>(v->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
	cd_guard.Reset(cd);//e->Guard( cd);

	DComplexDblGDL* cdr =
	  new DComplexDblGDL(v->Dim(), BaseGDL::NOZERO);

	if(nEl == 1) 
	  (*cdr)[0]=
	   DComplex((gsl_sf_exp((*cd)[0].real())*cos((*cd)[0].imag())),
		    (gsl_sf_exp((*cd)[0].real())*sin((*cd)[0].imag())));
	else
	  for(i=0;i<nEl;i++) 
	    (*cdr)[i]=
	      DComplex((gsl_sf_exp((*cd)[i].real())*cos((*cd)[i].imag())),
		       (gsl_sf_exp((*cd)[i].real())*sin((*cd)[i].imag())));
	
	return cdr;
	
      } else if(v->Type() == GDL_DOUBLE) {
	
	DDoubleGDL* d=static_cast<DDoubleGDL*>(v->Convert2(GDL_DOUBLE, 
							   BaseGDL::COPY));
	d_guard.Reset(d);//e->Guard( d);
	if(nEl == 1) 
	  (*dr)[0]=gsl_sf_exp((*d)[0]);
	else
	  for (i=0;i<nEl;++i) (*dr)[i]=gsl_sf_exp((*d)[i]);
	
	return dr;
      } else if(v->Type() == GDL_FLOAT || 
		v->Type() == GDL_INT ||
		v->Type() == GDL_LONG) {
	
	DFloatGDL *fr=new DFloatGDL(v->Dim(), BaseGDL::NOZERO);
	fr_guard.Reset(fr);//e->Guard( fr);

	DDoubleGDL* d=static_cast<DDoubleGDL*>(v->Convert2(GDL_DOUBLE, 
							   BaseGDL::COPY));
	d_guard.Reset(d);//e->Guard( d);

	if(nEl == 1) 
	  (*dr)[0]=gsl_sf_exp((*d)[0]);
	else
	  for (i=0;i<nEl;++i) (*dr)[i]=gsl_sf_exp((*d)[i]);

	return static_cast<DFloatGDL*>(dr->Convert2(GDL_FLOAT,BaseGDL::COPY));
      }

    }
    assert(false);
    return NULL;
  }
} // namespace
