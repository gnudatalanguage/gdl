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

#include <sys/time.h>

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

#define GDL_DEBUG
//#undef GDL_DEBUG

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

  double Gregorian2Julian(struct tm *ts)
  {
    double jd;
    jd = 367.0*(1900.+ts->tm_year) 
      - (7.0*((1900.+ts->tm_year) + ((1+ts->tm_mon+9.0)/12.0))/4.0)
      + (275.0*(1+ts->tm_mon)/9.0)+ts->tm_mday 
      + (ts->tm_hour + (ts->tm_min + ts->tm_sec/60.0)/60.0)/24.0 
      + 1721013.5;


      if((100.0*(1900.+ts->tm_year)  + 1+ts->tm_mon - 190002.5) < 0) jd=jd+1.0;

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

    if (nParam == 1) {
      //1 parameter, 
      //      1->current UTC time seconds
      //      default
      DIntGDL* v = static_cast<DIntGDL*>(e->GetParDefined(0)->Convert2(INT,BaseGDL::COPY));
      e->Guard(v);

      if (v->Equal(new DIntGDL(1)))
        ret_seconds=true;
    } else if (nParam == 2) {
      //2 parameters
      //if the first param is 0, return the date of the second arg
      //if the first param is 1, return the 'double' of the second arg
      DIntGDL* v1 = static_cast<DIntGDL*>(e->GetParDefined(0)->Convert2(INT,BaseGDL::COPY));
      e->Guard(v1);
      DDoubleGDL* v2 = static_cast<DDoubleGDL*>(e->GetParDefined(1)->Convert2(DOUBLE,BaseGDL::COPY));

      if(v1->Equal(new DIntGDL(0))) { //0, read the second argument as time_t;
        tval.tv_sec = static_cast<long int>((*v2)[0]);
        tval.tv_usec = static_cast<long int>(((*v2)[0]-tval.tv_sec)*1e+6);
        delete v2; // we delete v2 here as it is not guarded. Avoids a "new" in the following "else"
      } else { //1
        return v2;
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
             tstruct=gmtime(&tval.tv_sec);
           else
             tstruct=localtime(&tval.tv_sec);
         
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
      tstruct= gmtime(&tval.tv_sec);
    else
      tstruct= localtime(&tval.tv_sec);
    
    //Convert the time to JULIAN or NOT
    if(e->KeywordSet("JULIAN"))
        return new DDoubleGDL(Gregorian2Julian(tstruct)); 
    else 
      {
       char *st=new char[MAX_DATE_STRING_LENGTH];
       const char *format="%a %h %d %T %Y";//my IDL date format.
       DStringGDL *S;
                 
       SizeT res=strftime(st,MAX_DATE_STRING_LENGTH,format,tstruct);

       if(res != 0) 
         S=new DStringGDL(st);
       else
         S=new DStringGDL("");

       delete st;

       return S;
      }
  }

  BaseGDL* legendre(EnvT* e)
  {
    SizeT nParam=e->NParam(2); //, "LEGENDRE");
    auto_ptr<BaseGDL> guard;
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
	guard.reset(mvals);
      }

    if(nEm == 0)
      e->Throw( 
	       "Variable is undefined: "
	       +e->GetParString(2));

    
    nmin=nEx;
    if(nEl < nmin and nEl > 1) 	nmin=nEl;    
    if(nEm < nmin and nEm > 1) 	nmin=nEm;
    
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
	       "Complex Legendre not implemented: ");
    }        else      {
      //byte, int, long float, double, uint, ulong, int64, uint64

      DDoubleGDL* res;
      DDoubleGDL* x_cast;
      DIntGDL* l_cast,*m_cast;

      if(xvals->Type() == DOUBLE) 
	x_cast=  static_cast<DDoubleGDL*>(xvals);
      else
	{
	x_cast=  static_cast<DDoubleGDL*>(xvals->Convert2(DOUBLE,BaseGDL::COPY));
	e->Guard( x_cast);
	}

      //lval check
      if (lvals->Type() == STRING)
	e->Throw( 
		 "String expression not allowed in this context: "
		 +e->GetParString(1));
      else if (lvals->Type() == PTR)
	e->Throw( 
		 "Pointer expression not allowed in this context: "
		 +e->GetParString(1));
      else if (lvals->Type() == OBJECT) 
	e->Throw( 
		 "Object expression not allowed in this context: "
		 +e->GetParString(1));
      else if (lvals->Type() == STRUCT) 
	e->Throw( 
		 "Struct expression not allowed in this context: "
		 +e->GetParString(1));
      else if(lvals->Type() == COMPLEX ||
	      lvals->Type() == COMPLEXDBL) 
	e->Throw( 
		 "Complex Legendre not implemented: ");
      else if(lvals->Type() == INT)
	l_cast=static_cast<DIntGDL*>(lvals);
      else
	{
	  l_cast=static_cast<DIntGDL*>(lvals->Convert2(INT,BaseGDL::COPY));
	  e->Guard( l_cast);
	}

      //mval check
      if (mvals->Type() == STRING)
	e->Throw( 
		 "String expression not allowed in this context: "
		 +e->GetParString(2));
      else if (mvals->Type() == PTR)
	e->Throw( 
		 "Pointer expression not allowed in this context: "
		 +e->GetParString(2));
      else if (mvals->Type() == OBJECT) 
	e->Throw( 
		 "Object expression not allowed in this context: "
		 +e->GetParString(2));
      else if (mvals->Type() == STRUCT) 
	e->Throw( 
		 "Struct expression not allowed in this context: "
		 +e->GetParString(2));
      else if(mvals->Type() == COMPLEX ||
	      mvals->Type() == COMPLEXDBL) 
	e->Throw( 
		 "Complex Legendre not implemented: ");
      else if(mvals->Type() == INT)
	m_cast=static_cast<DIntGDL*>(mvals);
      else
	{
	  m_cast=static_cast<DIntGDL*>(mvals->Convert2(INT,BaseGDL::COPY));
	  e->Guard( m_cast);
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
      if(xvals->Type() != DOUBLE && !e->KeywordSet("DOUBLE"))
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

  BaseGDL * gamma(EnvT* e)
  {
    SizeT nParam = e->NParam();
    if(nParam != 1)
      throw GDLException(e->CallingNode(),
			 "GAMMA: Incorrect number of arguments.");

    BaseGDL* g=e->GetParDefined(0);
    SizeT nEl=g->N_Elements();
    if( nEl == 0)
      e->Throw( "Variable is undefined: "+e->GetParString(0));

    SizeT c;
    static DStructGDL *Values =  SysVar::Values();

    if(g->Type() == DOUBLE)
      {
	DDoubleGDL* gd = e->GetParAs<DDoubleGDL>(0);
	DDoubleGDL* resd = new DDoubleGDL(gd->Dim(), BaseGDL::NOZERO);
	DDouble d_infinity=(*static_cast<DDoubleGDL*>
			    (Values->Get
			     (Values->Desc()->TagIndex("D_INFINITY"), 0)))[0];


	for (c=0;c<nEl;++c)
	  if((*gd)[c] >0.0  && (*gd)[c]< GSL_SF_GAMMA_XMAX)
	    (*resd)[c]=gsl_sf_gamma(static_cast<double>((*gd)[c]));
	  else
	    (*resd)[c]=d_infinity;

	return resd;
      }
    else
      {
	DFloatGDL* gf = e->GetParAs<DFloatGDL>(0);
	DFloatGDL* resf = new DFloatGDL(gf->Dim(), BaseGDL::NOZERO);
	DFloat f_infinity=(*static_cast<DFloatGDL*>
			   (Values->Get
			    (Values->Desc()->TagIndex("F_INFINITY"), 0)))[0];
	for (c=0;c<nEl;++c)
	  if((*gf)[c] >0.0 && (*gf)[c]<GSL_SF_GAMMA_XMAX)
	    (*resf)[c]=static_cast<DFloat>
	      (gsl_sf_gamma(static_cast<DDouble>((*gf)[c])));
	  else
	    (*resf)[c]=f_infinity;
	

	return resf;
      }
  }

  BaseGDL * lngamma(EnvT* e)
  {
    SizeT nParam = e->NParam();
    if(nParam != 1)
      e->Throw( 
			 "Incorrect number of arguments.");

    BaseGDL* g=e->GetParDefined(0);
    SizeT nEl=g->N_Elements();
    if( nEl == 0)
      e->Throw( 
			  "Variable is undefined: "+e->GetParString(0));

    SizeT c;

    static DStructGDL *Values =  SysVar::Values();

    if(g->Type() == DOUBLE)
      {
	DDoubleGDL* gd = e->GetParAs<DDoubleGDL>(0);
	DDoubleGDL* resd = new DDoubleGDL(gd->Dim(), BaseGDL::NOZERO);
	DDouble d_infinity=(*static_cast<DDoubleGDL*>
			    (Values->Get
			     (Values->Desc()->TagIndex("D_INFINITY"), 0)))[0];


	for (c=0;c<nEl;++c)
	  if((*gd)[c] >0.0  && (*gd)[c]< GSL_SF_GAMMA_XMAX)
	    (*resd)[c]=gsl_sf_lngamma(static_cast<double>((*gd)[c]));
	  else
	    (*resd)[c]=d_infinity;

	return resd;
      }
    else
      {
	DFloatGDL* gf = e->GetParAs<DFloatGDL>(0);
	DFloatGDL* resf = new DFloatGDL(gf->Dim(), BaseGDL::NOZERO);
	DFloat f_infinity=(*static_cast<DFloatGDL*>
			   (Values->Get
			    (Values->Desc()->TagIndex("F_INFINITY"), 0)))[0];
	for (c=0;c<nEl;++c)
	  if((*gf)[c] >0.0 && (*gf)[c]<GSL_SF_GAMMA_XMAX)
	    (*resf)[c]=static_cast<DFloat>
	      (gsl_sf_lngamma(static_cast<DDouble>((*gf)[c])));
	  else
	    (*resf)[c]=f_infinity;
	

	return resf;
      }


  }


  BaseGDL * igamma(EnvT* e)
  {
    SizeT nParam = e->NParam();
    if(nParam != 2)
      throw GDLException(e->CallingNode(),
			 "IGAMMA: Incorrect number of arguments.");

    BaseGDL* g=e->GetParDefined(0);
    BaseGDL* b=e->GetParDefined(1);
    SizeT nEl=g->N_Elements();
    SizeT nElb=b->N_Elements();
    if( nEl == 0)
      e->Throw( 
			  "Variable is undefined: "+e->GetParString(0));
    if( nElb == 0)
      e->Throw( 
			  "Variable is undefined: "+e->GetParString(1));
    nEl=nEl > nElb? nElb:nEl;
    if(e->KeywordSet("EPS")) 
      Message("IGAMMA: EPS Keyword not supported");
    if(e->KeywordSet("ITER")) 
      Message("IGAMMA: ITER Keyword not supported");
    if(e->KeywordSet("ITMAX")) 
      Message("IGAMMA: ITMAX Keyword not supported");
    if(e->KeywordSet("METHOD")) 
      Message("IGAMMA: METHOD Keyword not supported");
    SizeT c;

    static DStructGDL *Values =  SysVar::Values();

    if(g->Type() == DOUBLE || e->KeywordSet("DOUBLE"))
      {
	DDoubleGDL* gd = e->GetParAs<DDoubleGDL>(0);
	DDoubleGDL* x = e->GetParAs<DDoubleGDL>(1);
	DDoubleGDL* resd = new DDoubleGDL(gd->Dim(), BaseGDL::NOZERO);
	DDouble d_infinity=(*static_cast<DDoubleGDL*>
			    (Values->Get
			     (Values->Desc()->TagIndex("D_INFINITY"), 0)))[0];


	for (c=0;c<nEl;++c)
	  if((*gd)[c] >0.0  && (*gd)[c]< GSL_SF_GAMMA_XMAX)
	    (*resd)[c]=gsl_sf_gamma_inc_P(static_cast<double>((*gd)[c]),static_cast<double>((*x)[c]));
	  else
	    (*resd)[c]=d_infinity;

	return resd;
      }
    else
      {
	DFloatGDL* gf = e->GetParAs<DFloatGDL>(0);
	DFloatGDL* x = e->GetParAs<DFloatGDL>(1);
	DFloatGDL* resf = new DFloatGDL(gf->Dim(), BaseGDL::NOZERO);
	DFloat f_infinity=(*static_cast<DFloatGDL*>
			   (Values->Get
			    (Values->Desc()->TagIndex("F_INFINITY"), 0)))[0];
	for (c=0;c<nEl;++c)
	  if((*gf)[c] >0.0 && (*gf)[c]<GSL_SF_GAMMA_XMAX)
	    (*resf)[c]=static_cast<DFloat>
	      (gsl_sf_gamma_inc_P(static_cast<DDouble>((*gf)[c]),static_cast<double>((*x)[c])));
	  else
	    (*resf)[c]=f_infinity;
	

	return resf;
      }


  }


  BaseGDL * beta(EnvT* e)
  {
    SizeT nParam = e->NParam();
    if(nParam != 2)
      throw GDLException(e->CallingNode(),
			 "BETA: Incorrect number of arguments.");

    BaseGDL* g=e->GetParDefined(0);
    BaseGDL* b=e->GetParDefined(1);
    SizeT nEl=g->N_Elements();
    SizeT nElb=b->N_Elements();
    if( nEl == 0)
      e->Throw( 
			  "Variable is undefined: "+e->GetParString(0));
    if( nElb == 0)
      e->Throw( 
			  "Variable is undefined: "+e->GetParString(1));
    nEl=nEl > nElb? nElb:nEl;
    SizeT c;

    static DStructGDL *Values =  SysVar::Values();

    if(g->Type() == DOUBLE  || e->KeywordSet("DOUBLE"))
      {
	DDoubleGDL* gd = e->GetParAs<DDoubleGDL>(0);
	DDoubleGDL* bd = e->GetParAs<DDoubleGDL>(1);
	DDoubleGDL* resd = new DDoubleGDL(gd->Dim(), BaseGDL::NOZERO);
	DDouble d_infinity=(*static_cast<DDoubleGDL*>
			    (Values->Get
			     (Values->Desc()->TagIndex("D_INFINITY"), 0)))[0];


	for (c=0;c<nEl;++c)
	  if((*gd)[c] >0.0  && (*gd)[c]< GSL_SF_GAMMA_XMAX)
	    (*resd)[c]=gsl_sf_beta(static_cast<double>((*gd)[c]),static_cast<double>((*bd)[c]));
	  else
	    (*resd)[c]=d_infinity;

	return resd;
      }
    else
      {
	DFloatGDL* gf = e->GetParAs<DFloatGDL>(0);
	DFloatGDL* bf = e->GetParAs<DFloatGDL>(1);
	DFloatGDL* resf = new DFloatGDL(gf->Dim(), BaseGDL::NOZERO);
	DFloat f_infinity=(*static_cast<DFloatGDL*>
			   (Values->Get
			    (Values->Desc()->TagIndex("F_INFINITY"), 0)))[0];
	for (c=0;c<nEl;++c)
	  if((*gf)[c] >0.0 && (*gf)[c]<GSL_SF_GAMMA_XMAX)
	    (*resf)[c]=static_cast<DFloat>
	      (gsl_sf_beta(static_cast<DDouble>((*gf)[c]),static_cast<DDouble>((*bf)[c])));
	  else
	    (*resf)[c]=f_infinity;
	

	return resf;
      }
  }


  BaseGDL* gsl_exp(EnvT* e)
  {
    SizeT nParam = e->NParam(1);
    BaseGDL* v=e->GetParDefined(0);   

    size_t nEl = v->N_Elements();
    size_t i;
    if (v->Type() == STRING) {
      e->Throw( 
		  "String expression not allowed in this context: "
			  +e->GetParString(0));
    } else if (v->Type() == PTR) {
      e->Throw( 
		  "Pointer expression not allowed in this context: "
			  +e->GetParString(0));
    } else if (v->Type() == OBJECT) {
      e->Throw( 
		  "Object expression not allowed in this context: "
			  +e->GetParString(0));
    } else if (v->Type() == STRUCT) {
      e->Throw( 
		  "Struct expression not allowed in this context: "
			  +e->GetParString(0));		  
    } else   {
      //      DDoubleGDL* d;
      DDoubleGDL* dr = new DDoubleGDL(v->Dim(), BaseGDL::NOZERO);
      e->Guard( dr);

      if(v->Type() == COMPLEX) {
	DComplexDblGDL* cd=
	  static_cast<DComplexDblGDL*>(v->Convert2(COMPLEXDBL, BaseGDL::COPY));
	e->Guard( cd);

	DComplexDblGDL* cdr =
	  new DComplexDblGDL(v->Dim(), BaseGDL::NOZERO);
	e->Guard( cdr);

	if(nEl == 1) 
	  (*cdr)[0]=
	   DComplex((gsl_sf_exp((*cd)[0].real())*cos((*cd)[0].imag())),
		    (gsl_sf_exp((*cd)[0].real())*sin((*cd)[0].imag())));
	else
	  for(i=0;i<nEl;++i) 
	    (*cdr)[i]=
	      DComplex((gsl_sf_exp((*cd)[i].real())*cos((*cd)[i].imag())),
		       (gsl_sf_exp((*cd)[i].real())*sin((*cd)[i].imag())));

	return static_cast<DComplexGDL*>(cdr->Convert2(COMPLEX,BaseGDL::COPY));

      } else if(v->Type() == COMPLEXDBL) {
	DComplexDblGDL* cd=
	  static_cast<DComplexDblGDL*>(v->Convert2(COMPLEXDBL, BaseGDL::COPY));
	e->Guard( cd);

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
	
      } else if(v->Type() == DOUBLE) {
	
	DDoubleGDL* d=static_cast<DDoubleGDL*>(v->Convert2(DOUBLE, 
							   BaseGDL::COPY));
	e->Guard( d);
	if(nEl == 1) 
	  (*dr)[0]=gsl_sf_exp((*d)[0]);
	else
	  for (i=0;i<nEl;++i) (*dr)[i]=gsl_sf_exp((*d)[i]);
	
	return dr;
      } else if(v->Type() == FLOAT || 
		v->Type() == INT ||
		v->Type() == LONG) {
	
	DFloatGDL *fr=new DFloatGDL(v->Dim(), BaseGDL::NOZERO);
	e->Guard( fr);

	DDoubleGDL* d=static_cast<DDoubleGDL*>(v->Convert2(DOUBLE, 
							   BaseGDL::COPY));
	e->Guard( d);

	if(nEl == 1) 
	  (*dr)[0]=gsl_sf_exp((*d)[0]);
	else
	  for (i=0;i<nEl;++i) (*dr)[i]=gsl_sf_exp((*d)[i]);

	return static_cast<DFloatGDL*>(dr->Convert2(FLOAT,BaseGDL::COPY));
      }

    }
  }




      


			  			  
 
} // namespace
