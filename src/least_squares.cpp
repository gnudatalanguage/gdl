/***************************************************************************
                          la_least_squares.cpp  -  GDL library function
                             -------------------
    begin                : Jul 2 2015
    copyright            : (C) 2015 by NATCHKEBIA Ilia
    email contact        : alaingdl@users.sourceforge.net
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

#include "envt.hpp"
#include "least_squares.hpp"

#if !defined(USE_EIGEN)

namespace lib {

  BaseGDL* la_least_squares_fun( EnvT* e) {
    Message("GDL compiled without Eigen3 :  LA_LEAST_SQUARES not available");
    return new DIntGDL(0);
  }
}

#else

#include <Eigen/LU>
#include <Eigen/Eigenvalues>
#include <Eigen/Core>
#include <Eigen/SVD>
namespace lib {
  using namespace Eigen;

  using namespace std;
#ifndef _MSC_VER
  using std::isnan;
#endif

  //const int szdbl=sizeof(double);
  //const int szflt=sizeof(float);

  //#if defined(USE_EIGEN)
  // using namespace Eigen;


  /***********************************************************
   ********************la_least_squares_Solution***************
   ************************************************************/
  BaseGDL* la_least_squares_fun( EnvT* e) {
    

    
    static int method = 3;  //default Method is 3 that give identic result as idl
    static int isDouble = 0;   //default double is 0
    static int methodIx=e->KeywordIx("METHOD");
    e->AssureLongScalarKWIfPresent(methodIx , method);
    static int doubleIx=e->KeywordIx("DOUBLE");
    if (e->KeywordSet(doubleIx)) isDouble = 1;
    
    BaseGDL* p0 = e->GetParDefined( 0);
    BaseGDL* p1 = e->GetParDefined( 1);


    /***************Checking_if_arguments_are_OK***********/

    if (p0->N_Elements()==0)
      e->Throw( "Variable A is undefined: " + e->GetParString(0));
    if (p1->N_Elements()==0)
      e->Throw( "Variable B is undefined: " + e->GetParString(1));
    if (p0->Rank() != 2){
      e->Throw( "Argument A must be a 2 dimensional matrix:" + e->GetParString(0));
    }
    if (p1->Rank() > 2) {
      e->Throw( "Argument B must be a vector or 2 dimensional array:" + e->GetParString(1));
    }
    if(p1->Rank() == 1) {
      if(p1->N_Elements()!=p0->Dim(1))
	e->Throw("Number of elements in B does not match number of rows in A");
    }else if (p1->Dim(1)!=p0->Dim(1)){
      e->Throw("Number of rows in B does not match number of rows in A");
    }
    if(0 > method || method > 3) {
      e->Throw("Method must be 0,1,2 or 3");
    }
    
    long NbCol1,NbRow1,NbCol2,NbRow2;
	
    /**********************Variables************************/
    DDoubleGDL* p0D = static_cast<DDoubleGDL*>
      (p0->Convert2( GDL_DOUBLE, BaseGDL::COPY));
    DDoubleGDL* p2D = static_cast<DDoubleGDL*>
      (p1->Convert2( GDL_DOUBLE, BaseGDL::COPY));
    NbCol1=p0->Dim(1);
    NbRow1=p0->Dim(0);
    NbCol2=p1->Dim(1);
    NbRow2=p1->Dim(0);
    Map<Matrix<double,Dynamic,Dynamic,RowMajor> > m0(&(*p0D)[0], NbCol1,NbRow1);
    MatrixXd tmp_res;
    /**************Methods******************/
    if (p1->Dim(1)) {
      Map<Matrix<double, Dynamic, Dynamic, RowMajor> > m2(&(*p2D)[0], NbCol2, NbRow2);
      switch(method) {
        case 0:tmp_res = m0.colPivHouseholderQr().solve(m2) ;break;
        case 1:tmp_res = (m0.transpose() * m0).ldlt().solve(m0.transpose() * m2) ; break;
        default: {
          if (p0->Dim(0) < 16) tmp_res = m0.jacobiSvd(ComputeThinU | ComputeThinV).solve(m2); // 16 according to doc
          else {
#if EIGEN_VERSION_AT_LEAST(3,0,0)
            BDCSVD<Matrix<double,Dynamic,Dynamic,RowMajor>> bdc_svd(m0, ComputeThinU | ComputeThinV); 
            tmp_res=bdc_svd.solve(m2);
#else
          tmp_res = m0.jacobiSvd(ComputeThinU | ComputeThinV).solve(m2) ;break;
#endif
          }
        }
      }
    } else {
      Map<VectorXd> m3(&(*p2D)[0], NbRow2); //for one B when it's vector
      switch(method) {
        case 0:tmp_res = m0.colPivHouseholderQr().solve(m3) ;break;
        case 1:tmp_res = (m0.transpose() * m0).ldlt().solve(m0.transpose() * m3);
        default:{
          if (p0->Dim(0) < 16) tmp_res = m0.jacobiSvd(ComputeThinU | ComputeThinV).solve(m3) ;
          else {
#if EIGEN_VERSION_AT_LEAST(3,0,0)
            BDCSVD<Matrix<double,Dynamic,Dynamic,RowMajor>> bdc_svd(m0, ComputeThinU | ComputeThinV); 
            tmp_res=bdc_svd.solve(m3);
#else
            tmp_res = m0.jacobiSvd(ComputeThinU | ComputeThinV).solve(m3) ;
#endif
          }
        }
      }
    }

    /***********************Return Values******************/
    if(p1->Dim(1)) {
      SizeT dims[3];
      dims[1] = NbRow1;
      dims[0] = NbRow2;
      dimension dim( dims, (SizeT) 2 );

      if(( p0->Type() == GDL_DOUBLE) || isDouble) {
	DDoubleGDL* resD =new DDoubleGDL(dim, BaseGDL::NOZERO);
	Map<MatrixXd>(&(*resD)[0], NbRow2,NbRow1) = tmp_res.transpose().cast<double>();
	return resD;
      } else {
			
	DFloatGDL* resF = new DFloatGDL(dim, BaseGDL::NOZERO);
	Map<MatrixXf>(&(*resF)[0], NbRow2,NbRow1) = tmp_res.transpose().cast<float>();
	return resF;
      }
    } else { //when B is a vector
      if(( p0->Type() == GDL_DOUBLE) || isDouble) {
	DDoubleGDL* resD =new DDoubleGDL(NbRow1, BaseGDL::NOZERO);
	Map<VectorXd>(&(*resD)[0], NbRow1) = tmp_res.cast<double>();
	return resD;
      } else {
			
	DFloatGDL* resF = new DFloatGDL(NbRow1, BaseGDL::NOZERO);
	Map<VectorXf>(&(*resF)[0], NbRow1) = tmp_res.cast<float>();
	return resF;
      }
    }
  }

  //#endif
} //namespace lib

#endif //Use_Eigen
