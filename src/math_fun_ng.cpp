/***************************************************************************
                          math_fun_ng.cpp  -  math GDL library function (AC)
                             -------------------
    begin                : 26 May 2008
    copyright            : (C) 2008 by Nicolas Galmiche
    email                : n.galmiche@gmail.com

****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "voigt.cpp"
#include "includefirst.hpp"
#include "envt.cpp"
#include "math_fun_ng.hpp"
#include "basic_fun.cpp"

using namespace std;

namespace lib {


BaseGDL* voigt_fun(EnvT* e)
 { 
    SizeT nParam = e->NParam();
    SizeT nMax,nMin, i;
    DFloatGDL* res ;
    dimension DimMin,DimMax;

    if (nParam !=2)
        e->Throw(" function VOIGT takes 2 params: 'Result = VOIGT(A,U)'  ");

    DFloatGDL* A = e->GetParAs<DFloatGDL>(0);
    if(e->GetParDefined(0)->Type() == COMPLEX || e->GetParDefined(0)->Type() == COMPLEXDBL)
       e->Throw(" no complex : ");
    
    DFloatGDL* U = e->GetParAs<DFloatGDL>(1);
    if(e->GetParDefined(1)->Type() == COMPLEX || e->GetParDefined(1)->Type() == COMPLEXDBL)
       e->Throw(" no complex : ");

    if (A->N_Elements()> U->N_Elements()) {nMax=A->N_Elements(); nMin=U->N_Elements();}
    else {nMax=U->N_Elements();nMin=A->N_Elements();}
 
// Use to define NaN which is returned if one parameter of humlik function is Not A Number 
    static DStructGDL *Values =  SysVar::Values();                                                
    DDouble d_nan=(*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0];           
    DDouble d_infinity= (*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_INFINITY"), 0)))[0];  


// Assign matching dimension to res
   if (A->N_Elements()> U->N_Elements()) 
	{DimMax=A->Dim(); DimMin=U->Dim();}
   else {DimMax=U->Dim();DimMin=A->Dim();}


// Different Memory Allocations 
    if (U->Rank()==0 && A->Rank()==0)
	res = new DFloatGDL(DimMin,BaseGDL::NOZERO); // res = Scalar
    else if(U->Rank()==0|| A->Rank()==0)

       res = new DFloatGDL(DimMax,BaseGDL::NOZERO); // res Dimensions = Array  Dimensions 
	
    else	
       res = new DFloatGDL(DimMin,BaseGDL::NOZERO); // res Dimensions = smallest Array Dimensions


   float InitA = (*A)[0];
   float InitU = (*U)[0];

// Voigt ( scalar , scalar )   
    if ( A->Rank()==0 && U->Rank()==0 ) 
	{
	if (isfinite(InitA)==0 || isfinite(InitU)==0)

	   {

	   if ((InitU==d_infinity||InitU==-d_infinity ) && isfinite(InitA)!=0)
	      (*res)[0] = 0.0000;
	   else
	      (*res)[0] = d_nan;
	   }

	else if(InitA<0) 
	   (*res)[0] = -(humlik( InitU,-(InitA )));

	else 
	   (*res)[0] = humlik( InitU,InitA );
	}
				   
// Voigt ( array , array )
    if ( A->Rank()!= 0 && U->Rank()!= 0)
	{  
	 	for (i=0;i<nMin;++i)
		{ 
			if (isfinite((*A)[i])==0||isfinite((*U)[i])==0)

			{
                         if(((*U)[i]==d_infinity ||(*U)[i]==-d_infinity) && isfinite((*A)[i])!=0) 
	      		   (*res)[i] = 0.0000;
			 else
			   (*res)[i] = d_nan;
			}

			else if ((*A)[i]<0)
			   (*res)[i]=-(humlik( (*U)[i],-((*A)[i])));

			else 	
			   (*res)[i]=humlik( (*U)[i],(*A)[i]);
		}   
	}

//Voigt ( scalar , array )
    if (A->Rank()==0 && U->Rank()!= 0) 
	{
		for (i=0;i<nMax;++i)
		{
			if (isfinite(InitA)==0 || isfinite((*U)[i])==0)
			   {
                           if(((*U)[i] == d_infinity ||(*U)[i] == -d_infinity) && isfinite((*A)[0])!=0)
	      		     (*res)[i] = 0.0000;
			   else
			     (*res)[i] = d_nan;
			   }

			else if (InitA<0)
			   (*res)[i]=-(humlik( (*U)[i],-(InitA)));

			else 
			   (*res)[i]=humlik( (*U)[i],InitA);
		}
	 }


// Voigt ( array , scalar)   
    if (A->Rank()!=0 && U->Rank() == 0)
	 {
	 for (i=0;i<nMax;++i)
		{ 
		if (isfinite((*A)[i])==0 || isfinite(InitU)==0)
		    {
                    if(((*U)[0] == d_infinity ||(*U)[0] == -d_infinity) && isfinite((*A)[i])!=0)
	      	      (*res)[i] = 0.0000;
		    else
		      (*res)[i] = d_nan;
		    }

		else if ((*A)[i]<0)
		   (*res)[i]=-(humlik( InitU,-((*A)[i])));

		else 
		   (*res)[i]=humlik( InitU,(*A)[i]);
		}		
	 }

    if( e->GetParDefined(0)->Type() == DOUBLE ||e->GetParDefined(1)->Type() == DOUBLE ) 
    return res->Convert2(DOUBLE,BaseGDL::CONVERT);
    
    return res;

} //end of voigt_fun



BaseGDL* rk4_fun(EnvT* e)
  {
  return new DByteGDL(0);
  }
 }//namespace
