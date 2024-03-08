/***************************************************************************
                   matrix_cholesky.cpp  -  GDL GSL library function
                             -------------------
    begin                : Jan 20 2004
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net
***************************************************************************/

// current versions of CHOLSOL/CHOLDC are based on Eigen3

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"

#include <map>
#include <cmath>
#include <stdio.h>
#include <iostream>
//#include <fstream>
#include <string>
#include <complex>

#include "matrix_cholesky.hpp"

#if !defined(USE_EIGEN)

namespace lib {
  BaseGDL* cholsol_fun( EnvT* e) {
    Message("GDL compiled without Eigen3 : CHOLSOL not available");
    return new DIntGDL(0);
  }
  BaseGDL* la_cholsol_fun( EnvT* e){
    Message("GDL compiled without Eigen3 : LA_CHOLSOL not available");
    return new DIntGDL(0);
  }
  void choldc_pro( EnvT* e) {
    Message("GDL compiled without Eigen3 : CHOLDC not available");
  }
  void la_choldc_pro( EnvT* e) {
    Message("GDL compiled without Eigen3 : LA_CHOLDC not available");
  }
}

#else

#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/Eigenvalues>


namespace lib {

  using namespace std;
#ifndef _MSC_VER
  using std::isnan;
#endif

  //const int szdbl=sizeof(double);
  //const int szflt=sizeof(float);

  //#if defined(USE_EIGEN)
  using namespace Eigen;


/***********************************************************
********************LA_Cholesky_Solution***********************
************************************************************/
  BaseGDL* la_cholsol_fun ( EnvT* e)
  {
    
    Message("We have troubles related to LA_CHOLDC/LA_CHOLSOL and Eigen");
    Message("Help and contributions very welcome");
    Message(" ");
    
    //    set_num_threads();

    SizeT nParam=e->NParam(2);
    BaseGDL* p0 = e->GetParDefined( 0);
    BaseGDL* p1 = e->GetParDefined( 1);

/*********************************Checking_if_arguments_are_OK*********************/

    if (p0->N_Elements()==0)
      e->Throw( "Variable A is undefined: " + e->GetParString(0));
    if (p1->N_Elements()==0)
      e->Throw( "Variable B is undefined: " + e->GetParString(1));
    if (p0->Rank() == 2){
      if (p0->Dim(0) != p0->Dim(1))
        e->Throw( "Argument A must be a square matrix:" + e->GetParString(0));
    }
    else
        e->Throw( "Argument A must be a square matrix:" + e->GetParString(0));
    if(p1->Rank()==1){
      if(p1->N_Elements()!=p0->Dim(0))
	 e->Throw("Arguments sizes mismatch");
    }
    else if(p1->Rank()==2){
      if(p1->Dim(1)!=p0->Dim(0))
	e->Throw("Arguments sizes mismatch");
    }
    else 
      e->Throw( "Argument B must be a vector or a matrix:" + e->GetParString(0));

   long NbCol,NbRow;

/*************************Double**************************************/
    static int DOUBLEIx=e->KeywordIx("DOUBLE");
    if(( p0->Type() == GDL_DOUBLE) || e->KeywordSet(DOUBLEIx))
      {

	DDoubleGDL* p0D = static_cast<DDoubleGDL*>
	  (p0->Convert2( GDL_DOUBLE, BaseGDL::COPY));
 	DDoubleGDL* p2D = static_cast<DDoubleGDL*>
 	  (p1->Convert2( GDL_DOUBLE, BaseGDL::COPY));
        NbCol=p0->Dim(0);
	NbRow=p0->Dim(1);
	Map<Matrix<double,Dynamic,Dynamic,RowMajor> > m0(&(*p0D)[0], NbCol,NbRow);
	Map<VectorXd> m2(&(*p2D)[0], NbCol);

	LLT<MatrixXd,Upper>solver;
	VectorXd tmp_res = solver.compute(m0).solve(m2);

	if(solver.info()==NumericalIssue) 
	  {
	    e->Throw( "Array is not positive definite: " + e->GetParString(0));
	    return 0;
	  }
	if(solver.info()!=Success) 
	  {
	    e->Throw( "Decomposition has failed: " + e->GetParString(0));
	    return 0;
	  }

	DDoubleGDL* res =new DDoubleGDL(NbCol, BaseGDL::NOZERO);
	Map<VectorXd>(&(*res)[0], NbCol) = tmp_res.cast<double>();

	return res;

      }


   return p0;
  }


/***********************************************************
********************Cholesky_Solution***********************
************************************************************/

  BaseGDL* cholsol_fun ( EnvT* e)
  {

    //    set_num_threads();
    
    SizeT nParam=e->NParam(3);
    BaseGDL* p0 = e->GetNumericParDefined( 0);
    BaseGDL* p1 = e->GetNumericParDefined( 1);
    BaseGDL* p2 = e->GetNumericParDefined( 2);

/*********************************Checking_if_arguments_are_OK*********************/   

    if (p0->N_Elements()==0)
      e->Throw( "Variable A is undefined: " + e->GetParString(0));
    if (p1->N_Elements()==0)
      e->Throw( "Variable P is undefined: " + e->GetParString(1));
    if (p2->N_Elements()==0)
      e->Throw( "Variable B is undefined: " + e->GetParString(2));

    if (p0->Rank() == 2) {
      if (p0->Dim(0) != p0->Dim(1))
        e->Throw( "Argument A must be a square matrix:" + e->GetParString(0));
    }
    else
        e->Throw( "Argument A must be a square matrix:" + e->GetParString(0));
    if (p1->Rank() != 1 )  
      e->Throw( "Argument P must be a column vector: " + e->GetParString(1));
    if (p2->Rank() != 1 )  
      e->Throw( "Argument B must be a column vector: " + e->GetParString(1));
//     else
//       if (p2->N_Elements()<2)

    if( p2->N_Elements()!=p1->N_Elements() || p1->N_Elements()!=p0->Dim(0) || p2->N_Elements()!=p0->Dim(0) )
      e->Throw("Arguments sizes mismatch");

    long NbCol,NbRow;


/*************************Double**************************************/
    static int DOUBLEIx=e->KeywordIx("DOUBLE");
    if(( p0->Type() == GDL_DOUBLE) || e->KeywordSet(DOUBLEIx))
      {

	DDoubleGDL* p0D = static_cast<DDoubleGDL*>
	  (p0->Convert2( GDL_DOUBLE, BaseGDL::COPY));
 	DDoubleGDL* p2D = static_cast<DDoubleGDL*>
 	  (p2->Convert2( GDL_DOUBLE, BaseGDL::COPY));
        NbCol=p0->Dim(0);
	NbRow=p0->Dim(1);
	Map<Matrix<double,Dynamic,Dynamic,RowMajor> > m0(&(*p0D)[0], NbCol,NbRow);
	Map<VectorXd> m2(&(*p2D)[0], NbCol);

	LLT<MatrixXd,Upper>solver;
	VectorXd tmp_res = solver.compute(m0).solve(m2);

	if(solver.info()==NumericalIssue) 
	  {
	    e->Throw( "Array is not positive definite: " + e->GetParString(0));
	    return 0;
	  }
	if(solver.info()!=Success) 
	  {
	    e->Throw( "Decomposition has failed: " + e->GetParString(0));
	    return 0;
	  }

	DDoubleGDL* res =new DDoubleGDL(NbCol, BaseGDL::NOZERO);
	Map<VectorXd>(&(*res)[0], NbCol) = tmp_res.cast<double>();

	return res;

      }

/*************************Complex**************************************/
    if( p0->Type() == GDL_COMPLEX && !e->KeywordSet(DOUBLEIx)) 
      {

      DComplexGDL* p0C = static_cast<DComplexGDL*>
	(p0->Convert2(GDL_COMPLEX , BaseGDL::COPY)); 
       DComplexGDL* p2C = static_cast<DComplexGDL*>
	(p2->Convert2(GDL_COMPLEX , BaseGDL::COPY));
      NbCol=p0->Dim(0);
      NbRow=p0->Dim(1);
      Map<Matrix<complex<float>,Dynamic,Dynamic,RowMajor> > m0(&(*p0C)[0], NbCol,NbRow);
      Map<VectorXcf> m2(&(*p2C)[0], NbCol);
      LLT<MatrixXf>solver;
      MatrixXf tmp_res = solver.compute(m0.real()).solve(m2.real());

      if(solver.info()==NumericalIssue) 
	{
	  e->Throw( "Array is not positive definite: " + e->GetParString(0));
	  return 0;
	}
      if(solver.info()!=Success) 
	{
	  e->Throw( "Decomposition has failed: " + e->GetParString(0));
	  return 0;
	}

      DFloatGDL* res = new DFloatGDL(NbCol, BaseGDL::NOZERO);
      Map<VectorXf>(&(*res)[0], NbCol) = tmp_res.cast<float>();

      return res;
    }

/*************************Complex_Double**************************************/
    else if( p0->Type() == GDL_COMPLEXDBL) {

      DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>
	(p0->Convert2(GDL_COMPLEXDBL , BaseGDL::COPY));
      DComplexDblGDL* p2C = static_cast<DComplexDblGDL*>
	(p2->Convert2(GDL_COMPLEXDBL , BaseGDL::COPY));
      NbCol=p0->Dim(0);
      NbRow=p0->Dim(1);
      Map<Matrix<complex<double>,Dynamic,Dynamic,RowMajor> > m0(&(*p0C)[0], NbCol,NbRow);
      Map<VectorXcd> m2(&(*p2C)[0], NbCol);
      LLT<MatrixXd>solver;
      MatrixXd tmp_res = solver.compute(m0.real()).solve(m2.real());

      if(solver.info()==NumericalIssue) 
	{
	  e->Throw( "Array is not positive definite: " + e->GetParString(0));
	  return 0;
	}
      if(solver.info()!=Success) 
	{
	  e->Throw( "Decomposition has failed: " + e->GetParString(0));
	  return 0;
	}

      DDoubleGDL* res = new DDoubleGDL(NbCol, BaseGDL::NOZERO);
      Map<VectorXd>(&(*res)[0], NbCol) = tmp_res.cast<double>();

      return res;
    }

/*************************All_Other**************************************/
    if( p0->Type() == GDL_FLOAT   ||
	     p0->Type() == GDL_LONG    ||
	     p0->Type() == GDL_ULONG   ||
	     p0->Type() == GDL_LONG64  ||
	     p0->Type() == GDL_ULONG64 ||
	     p0->Type() == GDL_INT     ||
	     p0->Type() == GDL_STRING  ||
	     p0->Type() == GDL_UINT    ||
	     p0->Type() == GDL_BYTE)
      {
	DFloatGDL* p0SS = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
 	DFloatGDL* p2SS = static_cast<DFloatGDL*>
 	  (p2->Convert2( GDL_FLOAT, BaseGDL::COPY));
        NbCol=p0->Dim(0);
	NbRow=p0->Dim(1);
	Map<Matrix<float,Dynamic,Dynamic,RowMajor> > m0(&(*p0SS)[0], NbCol,NbRow);
	Map<VectorXf> m2(&(*p2SS)[0], NbCol);

	LLT<MatrixXf,Upper>solver;
	VectorXf tmp_res = solver.compute(m0).solve(m2);

	if(solver.info()==NumericalIssue) 
	  {
	    e->Throw( "Array is not positive definite: " + e->GetParString(0));
	    return 0;
	  }
	if(solver.info()!=Success) 
	  {
	    e->Throw( "Decomposition has failed: " + e->GetParString(0));
	    return 0;
	  }

	DFloatGDL* res =new DFloatGDL(NbCol, BaseGDL::NOZERO);
	Map<VectorXf>(&(*res)[0], NbCol) = tmp_res.cast<float>();

	return res;
      }
//****************************Bug**************************  
    else 
      {
	cout << "Should never reach this point ! Please report it !" << endl; 
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
	return res;
      }

  }



/***********************************************************
********************error_checker_LA_CHOLDC*****************
************************************************************/

  bool error_check(EnvT* e,int status) 
  {
    static int statusIx = e->KeywordIx("STATUS");
    bool statusKeyword = e->KeywordPresent( statusIx );
    if ( statusKeyword ) e->AssureGlobalKW( statusIx );

    if(status==NumericalIssue) 
      {
	if(statusKeyword)
	  { 
	    e->SetKW(statusIx,new DLongGDL(1));
	    return 0;
	  }
	e->Throw( "Array is not positive definite: " + e->GetParString(0));
	return 0;
      }
    if(status!=Success) 
      {
	e->SetKW(statusIx,new DLongGDL(2));
	e->Throw( "Decomposition has failed: " + e->GetParString(0));
	return 0;
      }
    if(statusKeyword)
      e->SetKW(statusIx,new DLongGDL(0));

    return 1;
  }


/***********************************************************
********************LA_CHOLDC-procedure*********************
************************************************************/
void la_choldc_pro( EnvT* e) 
  {

    Message("We have troubles related to LA_CHOLDC/LA_CHOLSOL and Eigen");
    Message("Help and contributions very welcome");
    Message(" ");

    SizeT    nParam = e->NParam(1);
    BaseGDL* p0     = e->GetParDefined( 0);
    SizeT    nEl    = p0->N_Elements();

    if( nEl == 0)
      e->Throw( "Variable is undefined: " + e->GetParString(0));
    
    if (p0->Rank() > 2)
      e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    
    if (p0->Rank() > 1) {
      if (p0->Dim(0) != p0->Dim(1))
        e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    }

    if (p0->Rank() < 2) 
        e->Throw( "Input must be a square matrix:" + e->GetParString(0));


    long NbCol,NbRow;
    static int DOUBLEIx=e->KeywordIx("DOUBLE");
    static int UPPERIx=e->KeywordIx("UPPER");
/*************************Complex_Double**************************************/
    if( p0->Type() == GDL_COMPLEXDBL || (p0->Type() == GDL_COMPLEX && e->KeywordSet(DOUBLEIx))) {

      DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>
	(p0->Convert2(GDL_COMPLEXDBL , BaseGDL::COPY));
      NbCol=p0->Dim(0);
      NbRow=p0->Dim(1);
      Map<Matrix<complex<double>,Dynamic,Dynamic> > m0(&(*p0C)[0], NbCol,NbRow);
      MatrixXcd tmp_res;

      if(e->KeywordSet(UPPERIx))
	{
	  LLT<MatrixXcd,Upper>solver;
	  solver.compute(m0);

	  if ( !error_check(e,solver.info()) ) 
	    return ;

	  tmp_res=solver.matrixLLT();
	}
      else 
	{
	  LLT<MatrixXcd,Lower>solver;
	  solver.compute(m0);

	  if ( !error_check(e,solver.info()) ) 
	    return ;

	  tmp_res=solver.matrixLLT();
	}
	
      //    DComplexDblGDL* res2 = new DComplexDblGDL(p0->Dim(), BaseGDL::NOZERO);
      Map<MatrixXcd>(&(*p0C)[0], NbCol, NbRow) = tmp_res.transpose().cast<complex<double> >();

      e->SetPar(0,p0C);
      return ;
    }


/*************************Complex**************************************/
  else  if( p0->Type() == GDL_COMPLEX) {

       DComplexGDL* p0C = static_cast<DComplexGDL*>
	(p0->Convert2(GDL_COMPLEX , BaseGDL::COPY));  
      NbCol=p0->Dim(0);
      NbRow=p0->Dim(1);
      Map<Matrix<complex<float>,Dynamic,Dynamic> > m0(&(*p0C)[0], NbCol,NbRow);
      MatrixXcf tmp_res;

      if(e->KeywordSet(UPPERIx))
	{
	  LLT<MatrixXcf,Upper>solver;
	  solver.compute(m0);

	  if( !error_check(e,solver.info())) 
	    return ;

	  tmp_res=solver.matrixLLT();
	}
      else 
	{
	  LLT<MatrixXcf,Lower>solver;
	  solver.compute(m0);

	  if( !error_check(e,solver.info())) 
	    return ;

	  tmp_res=solver.matrixLLT();
	}
  
      //     DComplexGDL* res2 = new DComplexGDL(p0->Dim(), BaseGDL::NOZERO);
      Map<MatrixXcf >(&(*p0C)[0], NbCol, NbRow) = tmp_res.transpose().cast<complex<float> >();
      
      e->SetPar(0,p0C);
      return ;
    }

/*************************Double**************************************/
    else  if(( p0->Type() == GDL_DOUBLE) || e->KeywordSet(DOUBLEIx)) {
      
      DDoubleGDL* p0D = static_cast<DDoubleGDL*>
	(p0->Convert2( GDL_DOUBLE, BaseGDL::COPY));
      NbCol=p0->Dim(0);
      NbRow=p0->Dim(1);
      Map<Matrix<double,Dynamic,Dynamic> > m0(&(*p0D)[0], NbCol,NbRow);
      MatrixXd tmp_res;   

      if(e->KeywordSet(UPPERIx))
	{
	  LLT<MatrixXd,Upper>solver;
	  solver.compute(m0);

	  if( !error_check(e,solver.info())) 
	    return ;

	  tmp_res=solver.matrixLLT();
	}
      else 
	{
	  LLT<MatrixXd,Lower>solver;
	  solver.compute(m0);

	  if( !error_check(e,solver.info())) 
	    return ;

	  tmp_res=solver.matrixLLT();
	}

      Map<Matrix<double,Dynamic,Dynamic,RowMajor> >(&(*p0D)[0], NbCol, NbRow) = tmp_res.cast<double>();

      e->SetPar(0,p0D);
      return ;
    }

/*************************All_Other**************************************/
    else if( p0->Type() == GDL_FLOAT   ||
	     p0->Type() == GDL_LONG    ||
	     p0->Type() == GDL_ULONG   ||
	     p0->Type() == GDL_LONG64  ||
	     p0->Type() == GDL_ULONG64 ||
	     p0->Type() == GDL_INT     ||
	     p0->Type() == GDL_STRING  ||
	     p0->Type() == GDL_UINT    ||
	     p0->Type() == GDL_BYTE)
      {
	DFloatGDL* p0SS = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
        NbCol=p0->Dim(0);
	NbRow=p0->Dim(1);
	Map<Matrix<float,Dynamic,Dynamic> > m0(&(*p0SS)[0], NbCol,NbRow);
	MatrixXf tmp_res;

	if(e->KeywordSet(UPPERIx))
	  {
	    LLT<MatrixXf,Upper>solver;
	    solver.compute(m0);

	    if( !error_check(e,solver.info())) 
	      return ;

	    tmp_res=solver.matrixLLT();
	  }
	else 
	  {
	    LLT<MatrixXf,Lower>solver;
	    solver.compute(m0);

	    if( !error_check(e,solver.info())) 
	      return ;

	    tmp_res=solver.matrixLLT();

	  }

	Map<MatrixXf>(&(*p0SS)[0], NbCol, NbRow) = tmp_res.transpose().cast<float>();
	e->SetPar(0,p0SS);
	return ;
      }
//****************************Bug**************************  
    else 
      {
	cout << "Should never reach this point ! Please report it !" << endl; 
	return ;
      }
  }
/***********************************************************
********************Cholesky_Decomposition******************
************************************************************/

  void choldc_pro( EnvT* e) 
  {

    BaseGDL* p0 = e->GetNumericParDefined( 0);
    //BaseGDL* p0 = e->GetParDefined( 0);

    SizeT nParam=e->NParam(2);
//    long singular=0;
    SizeT nEl = p0->N_Elements();
    //cout<<"rank is"<<p0->Rank()<<endl;

    if( nEl == 0)
      e->Throw( "Variable is undefined: " + e->GetParString(0));
    
    if (p0->Rank() > 2)
      e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    
    if (p0->Rank() > 1) {
      if (p0->Dim(0) != p0->Dim(1))
        e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    }

    if (p0->Rank() < 2) 
        e->Throw( "Input must be a square matrix:" + e->GetParString(0));


    long NbCol,NbRow;

    //   cout <<  p0->Type() << endl;
    static int DOUBLEIx=e->KeywordIx("DOUBLE");

/*************************Complex**************************************/
    if( p0->Type() == GDL_COMPLEX && !e->KeywordSet(DOUBLEIx)) {
      DComplexGDL* p0C = static_cast<DComplexGDL*>
	(p0->Convert2(GDL_COMPLEX , BaseGDL::COPY));  
      NbCol=p0->Dim(0);
      NbRow=p0->Dim(1);
      Map<Matrix<complex<float>,Dynamic,Dynamic,RowMajor> > m0(&(*p0C)[0], NbCol,NbRow);
      LLT<MatrixXf>solver;
      MatrixXf tmp_res = solver.compute(m0.real()).matrixL();

      DFloatGDL* res = new DFloatGDL(p0->Dim(0), BaseGDL::NOZERO);
      Map<VectorXf>(&(*res)[0], NbCol) = tmp_res.diagonal().cast<float>();

      if(solver.info()==NumericalIssue) 
	{
	  e->Throw( "Array is not positive definite: " + e->GetParString(0));
	  return ;
	}
      if(solver.info()!=Success) 
	{
	  e->Throw( "Decomposition has failed: " + e->GetParString(0));
	  return;
	}

      e->SetPar(1,res);
      m0=m0.triangularView<Upper>();
      tmp_res=tmp_res.triangularView<StrictlyLower>();
      tmp_res=tmp_res+m0.real();

      DFloatGDL* res2 = new DFloatGDL(p0->Dim(), BaseGDL::NOZERO);
      Map<Matrix<float,Dynamic,Dynamic,RowMajor> >(&(*res2)[0], NbCol, NbRow) = tmp_res.cast<float>();
      
      e->SetPar(0,res2);
      return ;
    }

/*************************Complex_Double**************************************/
    else if( p0->Type() == GDL_COMPLEXDBL) {

      DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>
	(p0->Convert2(GDL_COMPLEXDBL , BaseGDL::COPY));
      NbCol=p0->Dim(0);
      NbRow=p0->Dim(1);
      Map<Matrix<complex<double>,Dynamic,Dynamic,RowMajor> > m0(&(*p0C)[0], NbCol,NbRow);
      LLT<MatrixXd>solver;
      MatrixXd tmp_res = solver.compute(m0.real()).matrixL();

      DDoubleGDL* res = new DDoubleGDL(p0->Dim(0), BaseGDL::NOZERO);
      Map<VectorXd>(&(*res)[0], NbCol) = tmp_res.diagonal().cast<double>();

      if(solver.info()==NumericalIssue) 
	{
	  e->Throw( "Array is not positive definite: " + e->GetParString(0));
	  return ;
	}
      if(solver.info()!=Success) 
	{
	  e->Throw( "Decomposition has failed: " + e->GetParString(0));
	  return;
	}

      e->SetPar(1,res);
      m0=m0.triangularView<Upper>();
      tmp_res=tmp_res.triangularView<StrictlyLower>();
      tmp_res=tmp_res+m0.real();
      DDoubleGDL* res2 = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);
      Map<Matrix<double,Dynamic,Dynamic,RowMajor> >(&(*res2)[0], NbCol, NbRow) = tmp_res.cast<double>();

      e->SetPar(0,res2);
      return ;
    }

/*************************Double**************************************/
    else  if(( p0->Type() == GDL_DOUBLE) || e->KeywordSet(DOUBLEIx)) {
      
      DDoubleGDL* p0D = static_cast<DDoubleGDL*>
	(p0->Convert2( GDL_DOUBLE, BaseGDL::COPY));
      NbCol=p0->Dim(0);
      NbRow=p0->Dim(1);
      Map<Matrix<double,Dynamic,Dynamic,RowMajor> > m0(&(*p0D)[0], NbCol,NbRow);
      LLT<MatrixXd>solver;
      MatrixXd tmp_res = solver.compute(m0).matrixL();

      DDoubleGDL* res = new DDoubleGDL(p0->Dim(0), BaseGDL::NOZERO);
      Map<VectorXd>(&(*res)[0], NbCol) = tmp_res.diagonal().cast<double>();

      if(solver.info()==NumericalIssue) 
	{
	  e->Throw( "Array is not positive definite: " + e->GetParString(0));
	  return ;
	}
      if(solver.info()!=Success) 
	{
	  e->Throw( "Decomposition has failed: " + e->GetParString(0));
	  return;
	}

      e->SetPar(1,res);
      m0=m0.triangularView<Upper>();
      tmp_res=tmp_res.triangularView<StrictlyLower>();
      tmp_res=tmp_res+m0;
      Map<Matrix<double,Dynamic,Dynamic,RowMajor> >(&(*p0D)[0], NbCol, NbRow) = tmp_res.cast<double>();

      e->SetPar(0,p0D);
      return ;
    }

/*************************All_Other**************************************/
    else if( p0->Type() == GDL_FLOAT   ||
	     p0->Type() == GDL_LONG    ||
	     p0->Type() == GDL_ULONG   ||
	     p0->Type() == GDL_LONG64  ||
	     p0->Type() == GDL_ULONG64 ||
	     p0->Type() == GDL_INT     ||
	     p0->Type() == GDL_STRING  ||
	     p0->Type() == GDL_UINT    ||
	     p0->Type() == GDL_BYTE)
      {
	DFloatGDL* p0SS = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
        NbCol=p0->Dim(0);
	NbRow=p0->Dim(1);
	Map<Matrix<float,Dynamic,Dynamic,RowMajor> > m0(&(*p0SS)[0], NbCol,NbRow);
	LLT<MatrixXf,Upper>solver;
	MatrixXf tmp_res = solver.compute(m0).matrixL();

// 	cout<<"matrix"<<endl<<m0<<"\n\n";
// 	cout<<"entire"<<endl<<solver.matrixLLT()<<"\n\n";
// 	cout<<"lower"<<endl<<tmp_res<<"\n\n";
// 	//	cout<<"upper"<<endl<<solver.matrixU()<<"\n\n";


	DFloatGDL* res =new DFloatGDL(NbCol, BaseGDL::NOZERO);
	Map<VectorXf>(&(*res)[0], NbCol) = tmp_res.diagonal().cast<float>();

	if(solver.info()==NumericalIssue) 
	  {
	    e->Throw( "Array is not positive definite: " + e->GetParString(0));
	    return ;
	  }
	if(solver.info()!=Success) 
	  {
	    e->Throw( "Decomposition has failed: " + e->GetParString(0));
	    return;
	  }

	e->SetPar(1,res);
	m0      = m0.triangularView<Upper>();
	tmp_res = tmp_res.triangularView<StrictlyLower>();
	tmp_res = tmp_res + m0;
	Map<Matrix<float,Dynamic,Dynamic,RowMajor> >(&(*p0SS)[0], NbCol, NbRow) = tmp_res.cast<float>();

	e->SetPar(0,p0SS);
	return ;
      }
//****************************Bug**************************  
    else 
      {
	cout << "Should never reach this point ! Please report it !" << endl; 
	return ;
      }


 return ;
  }



// UNUSED, copy of gsl_fun: bad habit!
//  template< typename T>
//  int cp2data_template( BaseGDL* p0, T* data, SizeT nEl, 
//			SizeT offset, SizeT stride_in, SizeT stride_out)
//  {
//    switch ( p0->Type()) {
//    case GDL_DOUBLE: 
//      cp2data2_template< DDoubleGDL, T>( p0, data, nEl, offset, 
//					 stride_in, stride_out);
//      break;
//    case GDL_FLOAT: 
//      cp2data2_template< DFloatGDL, T>( p0, data, nEl, offset, 
//					stride_in, stride_out);
//      break;
//    case GDL_LONG:
//      cp2data2_template< DLongGDL, T>( p0, data, nEl, offset, 
//				       stride_in, stride_out);
//      break;
//    case GDL_ULONG: 
//      cp2data2_template< DULongGDL, T>( p0, data, nEl, offset, 
//					stride_in, stride_out);
//      break;
//    case GDL_INT: 
//      cp2data2_template< DIntGDL, T>( p0, data, nEl, offset, 
//				      stride_in, stride_out);
//      break;
//    case GDL_UINT: 
//      cp2data2_template< DUIntGDL, T>( p0, data, nEl, offset, 
//				       stride_in, stride_out);
//      break;
//    case GDL_BYTE: 
//      cp2data2_template< DByteGDL, T>( p0, data, nEl, offset, 
//				       stride_in, stride_out);
//      break;
//    }
//    return 0;
//  }


} //namespace lib
#endif
