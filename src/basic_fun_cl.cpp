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

#include <string>
#include <fstream>
#include <memory>
#include <gsl/gsl_sys.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_sf.h>


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
    time_t t;  //the time
    struct tm *tstruct; //the time structure, from ctime.h

    t=time(0);  
    /*get the time before doing anything else, 
      this hopefully gives a more meaningful "time"
      than if the t=time(0) call came after an 
      arbitary number of conditional statements.*/


    SizeT nParam=e->NParam(0); //,"SYSTIME");
    bool ret_seconds=false;

    if(nParam == 1) {
      //1 parameter, 
      //      1->current UTC time seconds
      //      default

      BaseGDL* val=e->GetParDefined(0); 
	DIntGDL* v=static_cast<DIntGDL*>(val->Convert2(INT,BaseGDL::COPY));

	auto_ptr<DIntGDL> v_a(v);
	BaseGDL* c= new DIntGDL(1);//compare with 1

	if(v->Equal(c))
	  ret_seconds=true;
	
      } else if(nParam == 2) {
	//2 parameters
	//if the first param is 0, return the date of the second arg
	//if the first param is 1, return the 'double' of the second arg

	BaseGDL* val=e->GetParDefined(0); //,"SYSTIME");
	DIntGDL* v=static_cast<DIntGDL*>(val->Convert2(INT,BaseGDL::COPY));
	auto_ptr<DIntGDL> v_a(v);
	BaseGDL* c= new DIntGDL(0);//compare with 0

	BaseGDL* v2=e->GetParDefined(1); //,"SYSTIME");
	if(v->Equal(c)) 
	  {


	    //0, read the second argument as time_t;
	    DLongGDL *v2_cast=
	      static_cast<DLongGDL*>(v2->Convert2(LONG,BaseGDL::COPY));
	    auto_ptr<DLongGDL> v2_a(v2_cast);

	    t=static_cast<time_t>((*v2_cast)[0]);

	  } else {
	    //1
	    return static_cast<DDoubleGDL*>(v2->Convert2(DOUBLE,BaseGDL::COPY));
	  }


      }

    //Here, variable 't' contains the time in UTC seconds.

    //return the variable in seconds, either JULIAN, JULIAN+UTC, 
    //or no other keywords

    if( ret_seconds || e->KeywordSet("SECONDS") )
      {
	if( e->KeywordSet("JULIAN") )
	  {
	    if( e->KeywordSet("UTC") )
	      tstruct=gmtime(&t);
	    else
	      tstruct=localtime(&t);
	  
    return new DDoubleGDL(Gregorian2Julian(tstruct));
	  }
      else 
	{
	return new DDoubleGDL(t);
	}
      }
    
    //return a string of the time, either UTC or local (default)

    if(e->KeywordSet("UTC"))
      tstruct= gmtime(&t);
    else
      tstruct= localtime(&t);
    
    //Convert the time to JULIAN or NOT

    if(e->KeywordSet("JULIAN"))
      {
      return new DDoubleGDL(Gregorian2Julian(tstruct)); 
      }
    else 
      {

	char *st=new char[MAX_DATE_STRING_LENGTH];
	const char *format="%a %h %T %Y";//my IDL date format.
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
      throw GDLException( e->CallingNode(), 
			  "LEGENDRE: Variable is undefined: "
			  +e->GetParString(0));
    
    lvals=e->GetParDefined(1); //,"LEGENDRE");
      nEl=lvals->N_Elements();
      if(nEl == 0)
	throw GDLException( e->CallingNode(), 
			    "LEGENDRE: Variable is undefined: "
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
	throw GDLException( e->CallingNode(), 
			    "LEGENDRE: Variable is undefined: "
			    +e->GetParString(2));

    
    nmin=nEx;
    if(nEl < nmin and nEl > 1) 	nmin=nEl;    
    if(nEm < nmin and nEm > 1) 	nmin=nEm;
    
    if (xvals->Type() == STRING) {
      throw GDLException( e->CallingNode(), 
			  "LEGENDRE: String expression not allowed in this context: "
			  +e->GetParString(0));
    } else if (xvals->Type() == PTR) {
      throw GDLException( e->CallingNode(), 
			  "LEGENDRE: Pointer expression not allowed in this context: "
			  +e->GetParString(0));
    } else if (xvals->Type() == OBJECT) {
      throw GDLException( e->CallingNode(), 
			  "LEGENDRE: Object expression not allowed in this context: "
			  +e->GetParString(0));
    } else if (xvals->Type() == STRUCT) {
      throw GDLException( e->CallingNode(), 
			  "LEGENDRE: Struct expression not allowed in this context: "
			  +e->GetParString(0));
    } else if(xvals->Type() == COMPLEX ||
	      xvals->Type() == COMPLEXDBL) {
      throw GDLException( e->CallingNode(), 
			  "LEGENDRE: Complex Legendre not implemented: ");
    }        else      {
      //byte, int, long float, double, uint, ulong, int64, uint64

	DDoubleGDL* res;
	DDoubleGDL* x_cast;
	DIntGDL* l_cast,*m_cast;

	if(xvals->Type() == DOUBLE) 
	  x_cast=  static_cast<DDoubleGDL*>(xvals);
	else
	  x_cast=  static_cast<DDoubleGDL*>(xvals->Convert2(DOUBLE,BaseGDL::COPY));

	//lval check
	if (lvals->Type() == STRING)
	  throw GDLException( e->CallingNode(), 
			  "LEGENDRE: String expression not allowed in this context: "
			  +e->GetParString(1));
	else if (lvals->Type() == PTR)
	  throw GDLException( e->CallingNode(), 
			      "LEGENDRE: Pointer expression not allowed in this context: "
			      +e->GetParString(1));
	else if (lvals->Type() == OBJECT) 
	  throw GDLException( e->CallingNode(), 
			      "LEGENDRE: Object expression not allowed in this context: "
			      +e->GetParString(1));
	else if (lvals->Type() == STRUCT) 
	  throw GDLException( e->CallingNode(), 
			      "LEGENDRE: Struct expression not allowed in this context: "
			      +e->GetParString(1));
	else if(lvals->Type() == COMPLEX ||
		lvals->Type() == COMPLEXDBL) 
	  throw GDLException( e->CallingNode(), 
			      "LEGENDRE: Complex Legendre not implemented: ");
	else if(lvals->Type() == INT)
	  l_cast=static_cast<DIntGDL*>(lvals);
	else
	  l_cast=static_cast<DIntGDL*>(lvals->Convert2(INT,BaseGDL::COPY));


	//mval check
	if (mvals->Type() == STRING)
	  throw GDLException( e->CallingNode(), 
			  "LEGENDRE: String expression not allowed in this context: "
			  +e->GetParString(2));
	else if (mvals->Type() == PTR)
	  throw GDLException( e->CallingNode(), 
			      "LEGENDRE: Pointer expression not allowed in this context: "
			      +e->GetParString(2));
	else if (mvals->Type() == OBJECT) 
	  throw GDLException( e->CallingNode(), 
			      "LEGENDRE: Object expression not allowed in this context: "
			      +e->GetParString(2));
	else if (mvals->Type() == STRUCT) 
	  throw GDLException( e->CallingNode(), 
			      "LEGENDRE: Struct expression not allowed in this context: "
			      +e->GetParString(2));
	else if(mvals->Type() == COMPLEX ||
		mvals->Type() == COMPLEXDBL) 
	  throw GDLException( e->CallingNode(), 
			      "LEGENDRE: Complex Legendre not implemented: ");
	else if(mvals->Type() == INT)
	  m_cast=static_cast<DIntGDL*>(mvals);
	else
	  m_cast=static_cast<DIntGDL*>(mvals->Convert2(INT,BaseGDL::COPY));


	//x,m,l are converted to the correct format (double, int, int) here
	

	//make the result array have the same size as the smallest x,m,l array
	if(nmin == nEx) res=new DDoubleGDL(xvals->Dim(),BaseGDL::NOZERO);
	else if(nmin == nEl) res=new DDoubleGDL(lvals->Dim(),BaseGDL::NOZERO);
	else if(nmin == nEm) res=new DDoubleGDL(mvals->Dim(),BaseGDL::NOZERO);
	  


	  for (count=0;count<nmin;count++)
	    {
	      if((*x_cast)[count] < 0 or (*x_cast)[count] > 1)
		throw GDLException( e->CallingNode(),
				    "LEGENDRE: Argument X must be in the range [0.0, 1.0]");
	    (*res)[count]= 
	      gsl_sf_legendre_Plm((*l_cast)[nmin>nEl?0:count],
				  (*m_cast)[nmin > nEm?0:count],
				  (*x_cast)[nmin>nEx?0:count]);
	
	    }
	
	  //convert things back

	  if(xvals->Type() != DOUBLE && !e->KeywordSet("DOUBLE"))
	    {
	    return static_cast<DFloatGDL*>(res->Convert2(FLOAT,BaseGDL::COPY));

	    }
	  else
	    {
	      return res;
	    }
    }
    return new DByteGDL(0);
  }

  
  
 
 
} // namespace
