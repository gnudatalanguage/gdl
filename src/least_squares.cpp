/***************************************************************************
                          least_squares.cpp  - GDL library function
                             -------------------
    begin                : Jul 2 2015
    copyright            : (C) 2015 by NATCHKEBIA Ilia
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

#include <map>
#include <cmath>
#include <stdio.h>
#include <iostream>
//#include <fstream>
#include <string>
#include <complex>

#include "datatypes.hpp"
#include "envt.hpp"
#include "basic_fun.hpp"
#include "dinterpreter.hpp"

//#include "gsl_fun.hpp"

#if defined(USE_EIGEN)
#include <Eigen/LU>
#include <Eigen/Eigenvalues>
#include <Eigen/Core>
#endif

namespace lib {

  using namespace std;
#ifndef _MSC_VER
  using std::isnan;
#endif

  //const int szdbl=sizeof(double);
  //const int szflt=sizeof(float);

#if defined(USE_EIGEN)
  using namespace Eigen;


  /***********************************************************
   ********************la_least_squares_Solution***************
   ************************************************************/
  BaseGDL* la_least_squares_fun( EnvT* e) {

    static int method = 3;  //default Method is 3 that give identic result as idl
    static int isDouble = 0;   //default double is 0
    if (e->KeywordSet("METHOD")) e->AssureLongScalarKWIfPresent( e->KeywordIx("METHOD"), method);
    if (e->KeywordSet("DOUBLE")) isDouble = e->KeywordIx( "DOUBLE");
    
    SizeT nParam=e->NParam(2);
    BaseGDL* p0 = e->GetParDefined( 0);
    BaseGDL* p1 = e->GetParDefined( 1);


    /***************Checking_if_arguments_are_OK*********************/

    if (p0->N_Elements()==0)
      e->Throw( "Variable A is undefined: " + e->GetParString(0));
    if (p1->N_Elements()==0)
      e->Throw( "Variable B is undefined: " + e->GetParString(1));
    if (p0->Rank() < 2){
      e->Throw( "Argument A must be a multi dimensional matrix:" + e->GetParString(0));
    }
    if (p1->Rank() != 1) {
      e->Throw( "Argument B must be a vector:" + e->GetParString(1));
    }
    if(p1->N_Elements()!=p0->Dim(1)) {
      e->Throw("Number of elements in B does not match number of rows in A");
    }
    if(0 > method || method > 3) {
      e->Throw("Method must be 0,1,2 or 3");
    }
    
    long NbCol,NbRow;

    /*****************Variables******************************/

    DDoubleGDL* p0D = static_cast<DDoubleGDL*>
      (p0->Convert2( GDL_DOUBLE, BaseGDL::COPY));
    DDoubleGDL* p2D = static_cast<DDoubleGDL*>
      (p1->Convert2( GDL_DOUBLE, BaseGDL::COPY));
    NbCol=p0->Dim(1);
    NbRow=p0->Dim(0);
    Map<Matrix<double,Dynamic,Dynamic,RowMajor> > m0(&(*p0D)[0], NbCol,NbRow);
    Map<VectorXd> m2(&(*p2D)[0], NbCol);

    LLT<MatrixXd,Upper>solver;
    VectorXd tmp_res;
    /******************************Methods****************************/	
    if (method == 0)
      tmp_res = m0.colPivHouseholderQr().solve(m2) ;
    else if (method == 1)
      tmp_res = (m0.transpose() * m0).ldlt().solve(m0.transpose() * m2) ;
    else if (method >= 2) //method 2 and method 3 is the same
      tmp_res = m0.jacobiSvd(ComputeThinU | ComputeThinV).solve(m2) ;

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
    /***********************Return Values*****************************/

    if(( p0->Type() == GDL_DOUBLE) || e->KeywordSet("DOUBLE")) {
      DDoubleGDL* resD =new DDoubleGDL(NbRow, BaseGDL::NOZERO);
      Map<VectorXd>(&(*resD)[0], NbRow) = tmp_res.cast<double>();
      return resD;
    } else {
      DFloatGDL* resF = new DFloatGDL(NbRow, BaseGDL::NOZERO);
      Map<VectorXf>(&(*resF)[0], NbRow) = tmp_res.cast<float>();
      return resF;
    }

  }

#endif
} //namespace lib
