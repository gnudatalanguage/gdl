/***************************************************************************
                math_fun_ng.cpp  -  math GDL library function (NG)
                             -------------------
    begin                : 26 May 2008
    Copyright            : (C) 2008 by Nicolas Galmiche
    email                : n.galmiche AT gmail.com

****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "includefirst.hpp"


#include "voigt.cpp"
#include "math_fun_ng.hpp"

#ifdef _MSC_VER
#define isfinite _finite
#endif

using namespace std;

namespace lib {

  // important comment: as for Besel family, in IDL, VOIGT([2.],[0,1,2]) !=  VOIGT(2.,[0,1,2])
  // We don't follow this dangerous situation, we do have   VOIGT([2.],[0,1,2]) ==  VOIGT(2.,[0,1,2])

  BaseGDL* voigt_fun(EnvT* e)
  { 
    SizeT nParam = e->NParam();

    if (nParam !=2)
      e->Throw(" function VOIGT takes 2 params: 'Result = VOIGT(A,U)'  ");

    DFloatGDL* A = e->GetParAs<DFloatGDL>(0);
    if(e->GetParDefined(0)->Type() == GDL_COMPLEX || e->GetParDefined(0)->Type() == GDL_COMPLEXDBL)
      e->Throw(" no complex : ");
    
    DFloatGDL* U = e->GetParAs<DFloatGDL>(1);
    if(e->GetParDefined(1)->Type() == GDL_COMPLEX || e->GetParDefined(1)->Type() == GDL_COMPLEXDBL)
      e->Throw(" no complex : ");

    // Use to define NaN which is returned if one parameter of humlik function is Not A Number 
    static DStructGDL *Values =  SysVar::Values();                                                
    DDouble d_nan=(*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0];
    DDouble d_infinity= (*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_INFINITY"), 0)))[0];  
    // we don't use Gregory formalism (see macros in "math_fun_gm.cpp")
    // but we follow him notations ... just in case ...
    // Assign dimension(s) to "res"
    SizeT nElp0 = A->N_Elements();
    SizeT nElp1 = U->N_Elements();

    DFloatGDL* res ;


  if (A->Rank() == 0)							\
    res = new DFloatGDL(U->Dim(), BaseGDL::NOZERO);			\
  else if (U->Rank() == 0)						\
    res = new DFloatGDL(A->Dim(), BaseGDL::NOZERO);			\
  else if (A->N_Elements() > U->N_Elements())				\
    res = new DFloatGDL(U->Dim(), BaseGDL::NOZERO);			\
  else									\
    res = new DFloatGDL(A->Dim(), BaseGDL::NOZERO);			\

  /* the obsolete version (before idl 8.4)
    if (nElp0 == 1 && nElp1 == 1) {
      if (A->Rank() > U->Rank()) 
	res = new DFloatGDL(A->Dim(), BaseGDL::NOZERO);
      else
	res = new DFloatGDL(U->Dim(), BaseGDL::NOZERO);
    }
    else if (nElp0 > 1 && nElp1 == 1)
      res = new DFloatGDL(A->Dim(), BaseGDL::NOZERO);
    else if (nElp0 == 1 && nElp1 > 1)
      res = new DFloatGDL(U->Dim(), BaseGDL::NOZERO);
    else if (nElp0 <= nElp1)
      res = new DFloatGDL(A->Dim(), BaseGDL::NOZERO);
    else
      res = new DFloatGDL(U->Dim(), BaseGDL::NOZERO);
  */
    
    SizeT nElp = res->N_Elements();

    float InitA = (*A)[0];
    float InitU = (*U)[0];
    SizeT i;

    // Here also, we follow Nicolas choices, but can be simplified ...

    // Voigt ( scalar , scalar )   
    if (nElp0 == 1 && nElp1 == 1)
      {
	if (isfinite(InitA)==0 || isfinite(InitU)==0)
	  {
	    if ((InitU == d_infinity || InitU == -d_infinity ) && isfinite(InitA) != 0)
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
    if (nElp0 > 1 && nElp1 > 1)
      {  
	for (i=0;i<nElp;++i)
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
    if (nElp0 == 1 && nElp1 > 1)
      {
	/* obsolete !!	if ((A->Rank() > 0) && (SysVar::GDL_Warning())) {
	  Warning ( "You are using a case where IDL and GDL don't work similarly:");
	  Warning ( "in Voigt(A,U), A is a singleton array and U a true array: check your code !");
	  Warning ( "in GDL: Voigt(1,[0,1,2]) == Voigt([1],[0,1,2]) == Voigt([1,1,1],[0,1,2]).");
	  Warning ( "You can turn OFF this warning changing !GDL_WARNING to 0.");
	  }
	*/

	for (i=0;i<nElp;++i)
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
    if (nElp0 > 1 && nElp1 == 1)
      {
	/* obsolete !!
	if ((U->Rank() > 0) && (SysVar::GDL_Warning())) {
	  Warning ( "You are using a case where IDL and GDL don't work similarly:");
	  Warning ( "in Voigt(A,U), U is a singleton array and A a true array: check your code !");
	  Warning ( "in GDL: Voigt([0,1,2], 1) == Voigt([0,1,2],[1]) == Voigt([0,1,2],[1,1,1]).");
	  Warning ( "You can turn OFF this warning changing !GDL_WARNING to 0.");
	  }
	*/
       
	for (i=0;i<nElp;++i)
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

    if( e->GetParDefined(0)->Type() == GDL_DOUBLE ||e->GetParDefined(1)->Type() == GDL_DOUBLE ) 
      return res->Convert2(GDL_DOUBLE,BaseGDL::CONVERT);
    
    return res;

  } //end of voigt_fun

//--------------------------------------------------------------------------//

//   static DInterpreter* interpreter;

  void PushNewEnvRK( EnvT*e, DSubUD* newPro, BaseGDL** a,BaseGDL** b)
  {

    EnvUDT* newEnv= new EnvUDT( e->CallingNode(), newPro, (DObjGDL**)NULL);

    newEnv->SetNextPar(a); // pass as global
    newEnv->SetNextPar(b); // pass as global
	
    e->Interpreter()->CallStack().push_back(newEnv);

  }
  //***********************************  RK4 FUNCTION  **************************//
 
  BaseGDL* rk4_fun(EnvT* e)
  {
    SizeT nParam = e->NParam();

    if( nParam != 5)    
      e->Throw(" Invalid Number of arguments in RK4 ");
    //-----------------------------  ACQUISITION DES PARAMETRES  ------------------//

    //"Y" input array a vector of values for Y at X value
    //DDoubleGDL* Yvals = new DDoubleGDL(e->GetParAs<DDoubleGDL>(0)->N_Elements(),BaseGDL::NOZERO);
    DDoubleGDL* Yvals = e->GetParAs<DDoubleGDL>(0);
    if(e->GetParDefined(0)->Type() == GDL_COMPLEX || e->GetParDefined(0)->Type() == GDL_COMPLEXDBL)
      cout<<" If RK4 is complex then only the real part is used for the computation "<< endl;

    //"dydx" input value or array 
    //DDoubleGDL* dydxvals = new DDoubleGDL(e->GetParAs<DDoubleGDL>(1)->N_Elements(),BaseGDL::NOZERO);
    DDoubleGDL* dydxvals = e->GetParAs<DDoubleGDL>(1);
    if(e->GetParDefined(1)->Type() == GDL_COMPLEX || e->GetParDefined(1)->Type() == GDL_COMPLEXDBL)
      cout<<" If RK4 is complex then only the real part is used for the computation "<< endl;

    if(dydxvals->N_Elements()!=Yvals->N_Elements())e->Throw(" Y and DYDX dimensions have to match "); 

    // "X" input value  
    DDoubleGDL* X = e->GetParAs<DDoubleGDL>(2);
    if(e->GetParDefined(2)->Type() == GDL_COMPLEX || e->GetParDefined(2)->Type() == GDL_COMPLEXDBL)
      cout<<" If RK4 is complex then only the real part is used for the computation "<< endl;

    // "H" input value  
    DDoubleGDL* H = e->GetParAs<DDoubleGDL>(3);
    if(e->GetParDefined(3)->Type() == GDL_COMPLEX || e->GetParDefined(3)->Type() == GDL_COMPLEXDBL)
      cout<<" If RK4 is complex then only the real part is used for the computation "<< endl;
	

    // Differentiate User's Function string name 
    DStringGDL* init = e->GetParAs<DStringGDL>(4);
    if(e->GetParDefined(4)->Type() != GDL_STRING )
      e->Throw(" Fifth value must be a function name string ");


    //-------------------------------- Allocation -----------------------------------//
    BaseGDL *Steptwo,*Stepthree,*Stepfour;
    SizeT i;
    DDoubleGDL *HH,*H6,*XplusH,*Ytampon,*XH,*Yout,* dym,* dyt;

    Ytampon = new DDoubleGDL(Yvals->Dim(),BaseGDL::NOZERO);
    Yout = new DDoubleGDL(Yvals->Dim(),BaseGDL::NOZERO);
    HH = new DDoubleGDL(H->Dim(),BaseGDL::NOZERO);
    H6 = new DDoubleGDL(H->Dim(),BaseGDL::NOZERO);
    XH = new DDoubleGDL(H->Dim(),BaseGDL::NOZERO);
    BaseGDL* XHO=static_cast<BaseGDL*>(XH);
    XplusH = new DDoubleGDL(H->Dim(),BaseGDL::NOZERO);
    BaseGDL* XplusHO=static_cast<BaseGDL*>(XplusH);
    dym= new DDoubleGDL(Yvals->Dim(),BaseGDL::NOZERO);
    dyt= new DDoubleGDL(Yvals->Dim(),BaseGDL::NOZERO);
    //-------------------------------- Init FIRST STEP -----------------------------------//
    (*HH)[0]=(*H)[0]*0.50000;
    (*H6)[0]=(*H)[0]/6.00000;
    (*XH)[0] = (*X)[0] + (*HH)[0];
// marc: probably an error
//     XplusH[0] = (*X)[0] +  (*H)[0];
    (*XplusH)[0] = (*X)[0] +  (*H)[0];
	
		
    //dym=static_cast<DDoubleGDL*>(dymO);
    //dyt=static_cast<DDoubleGDL*>(dytO);
    //---------------------------- Init Call function -------------------------------------//
    DString RK_Diff;
    e->AssureScalarPar<DStringGDL>( 4, RK_Diff);	

    // this is a function name -> convert to UPPERCASE
    RK_Diff = StrUpCase( RK_Diff);

    // first search library funcedures  
    int funIx=LibFunIx( RK_Diff);
    StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());
		
    if( funIx != -1)
      {
	e->Throw(" String function name is intrinsic function name please change it  ");
      } 
    else
      {
	//  Search in user proc and function
	funIx = GDLInterpreter::GetFunIx(RK_Diff );
	
	//-----------------FIRST STEP-------------------//
	for (i=0;i<Yvals->N_Elements();++i)
	  (*Ytampon)[i]=(*Yvals)[i]+(*HH)[0]*(*dydxvals)[i]; 

	BaseGDL* Ytmp=static_cast<BaseGDL*>(Ytampon);
	  
	//  1st CALL to user function "differentiate"	
	PushNewEnvRK(e, funList[ funIx],&XHO,&Ytmp);
	EnvUDT* newEnv = static_cast<EnvUDT*>(e->Interpreter()->CallStack().back());	
	StackGuard<EnvStackT> guard1 ( e->Interpreter()->CallStack());

	BaseGDL* Steptwo = e->Interpreter()->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree()); 
	  
	//Conversion BaseGDL*-> DDoubleGDL* in order to use the RK_Diff function result.
	dyt= static_cast<DDoubleGDL*>(Steptwo->Convert2(GDL_DOUBLE,BaseGDL::CONVERT));
	  
	  

	//-------------SECOND STEP-------------------//	
	for (i=0;i<Yvals->N_Elements();++i)
	  (*Ytampon)[i]=(*Yvals)[i]+(*HH)[0]*(*dyt)[i];

	  	  
	//  2nd CALL to user function "differentiate"
	PushNewEnvRK(e, funList[ funIx],&XHO,&Ytmp);	
	  	
	StackGuard<EnvStackT> guard2 ( newEnv->Interpreter()->CallStack());
	
	BaseGDL* Stepthree = e->Interpreter()->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
	
	//Conversion BaseGDL*-> DDoubleGDL* in order to use the RK_Diff function result.
	dym = static_cast<DDoubleGDL*>(Stepthree->Convert2(GDL_DOUBLE,BaseGDL::CONVERT));
	  
	  
	//--------------THIRD STEP-------------------//
	for (i=0;i<Yvals->N_Elements();++i)
	  {
	    (*Ytampon)[i]=(*Yvals)[i]+ (*H)[0]*(*dym)[i];
	    (*dym)[i] += (*dyt)[i];
	  }

	  
	// 3rd CALL to user function "differentiate"
	PushNewEnvRK(e, funList[ funIx],&XplusHO,&Ytmp);
	  
	StackGuard<EnvStackT> guard3 ( newEnv->Interpreter()->CallStack());

	BaseGDL* Stepfour = e->Interpreter()->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

	dyt= static_cast<DDoubleGDL*>(Stepfour->Convert2(GDL_DOUBLE,BaseGDL::CONVERT));
	  
	//--------------FOURTH STEP-------------------//
	for (i=0;i<Yvals->N_Elements();++i)
	  (*Yout)[i]= (*Yvals)[i] + (*H6)[0] * ( (*dydxvals)[i]+(*dyt)[i]+ 2.00000*(*dym)[i] );
	  
	static DInt doubleKWIx = e->KeywordIx("DOUBLE");

	//if need, convert things back
	if( !e->KeywordSet(doubleKWIx))
	  return Yout->Convert2(GDL_FLOAT,BaseGDL::CONVERT);
	else
	  return Yout;
      }
    assert( false);	
  }// RK4_fun

}//namespace
