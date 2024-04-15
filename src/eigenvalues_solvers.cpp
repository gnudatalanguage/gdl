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

#include "eigenvalues_solvers.hpp"

#if defined(USE_EIGEN)
#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/Eigenvalues>
namespace lib {
 using namespace Eigen;
 using namespace std;
  BaseGDL* la_elmhes_fun(EnvT* e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);

	/*********************************Checking_if_arguments_are_OK*********************/

	if (p0->N_Elements() == 0)
	  e->Throw("Variable A is undefined: " + e->GetParString(0));
	if (p0->Rank() == 2) {
	  if (p0->Dim(0) != p0->Dim(1))
		e->Throw("Argument A must be a square matrix:" + e->GetParString(0));
	} else e->Throw("Argument A must be a square matrix:" + e->GetParString(0));

	static int DOUBLE = e->KeywordIx("DOUBLE");
	bool doDouble = e->KeywordSet(DOUBLE);

	int n=p0->Dim(0);
	
	if (doDouble || p0->Type() == GDL_DOUBLE || p0->Type() == GDL_COMPLEXDBL) {
	  
	  DDoubleGDL* a = e->GetParAs<DDoubleGDL>(0);
		Map<Matrix<double,Dynamic,Dynamic,RowMajor> >  A(&(*a)[0], n,n);
		HessenbergDecomposition<MatrixXd> hessOfA(A);
	    DDoubleGDL* res = new DDoubleGDL(a->Dim(),BaseGDL::NOZERO);
		Map<MatrixXd> H(&(*res)[0],n, n);
		H = hessOfA.matrixH().transpose();
		return res;
	} else {
	  DFloatGDL* a = e->GetParAs<DFloatGDL>(0);
		Map<Matrix<float,Dynamic,Dynamic,RowMajor> >  A(&(*a)[0], n,n);
		HessenbergDecomposition<MatrixXf> hessOfA(A);
		DFloatGDL* res = new DFloatGDL(a->Dim(), BaseGDL::NOZERO);
		Map<MatrixXf> H(&(*res)[0], n, n);
		H = hessOfA.matrixH().transpose();
		return res;
	}
  }

  void la_trired_pro(EnvT* e) {
	SizeT nParam = e->NParam(3);
	BaseGDL* p0 = e->GetParDefined(0);

	/*********************************Checking_if_arguments_are_OK*********************/

	if (p0->N_Elements() == 0)
	  e->Throw("Variable A is undefined: " + e->GetParString(0));
	if (p0->Rank() == 2) {
	  if (p0->Dim(0) != p0->Dim(1))
		e->Throw("Argument A must be a square matrix:" + e->GetParString(0));
	} else e->Throw("Argument A must be a square matrix:" + e->GetParString(0));

	static int DOUBLE = e->KeywordIx("DOUBLE");
	bool doDouble = e->KeywordSet(DOUBLE);
	int n = p0->Dim(0);

	if (doDouble || p0->Type() == GDL_DOUBLE || p0->Type() == GDL_COMPLEXDBL) {
	  if (ComplexType(p0->Type())) {
		DComplexDblGDL* a = e->GetParAs<DComplexDblGDL>(0);
		Map<Matrix<std::complex<DDouble>,Dynamic,Dynamic,RowMajor> >  A(&(*a)[0], n,n);
		Tridiagonalization<MatrixXcd> Tri(A);
		DComplexDblGDL* res = new DComplexDblGDL(a->Dim(),BaseGDL::NOZERO);
		Map<MatrixXcd> R(&(*res)[0],n,n);
		R=Tri.matrixQ().transpose();
		DDoubleGDL* D = new DDoubleGDL(dimension(n), BaseGDL::NOZERO);
		Map<VectorXd> eigen_d(&(*D)[0],n);
		eigen_d=Tri.diagonal();
		DDoubleGDL* E = new DDoubleGDL(dimension(n), BaseGDL::ZERO);
		Map<VectorXd> eigen_e(&(*E)[0],n-1);
		eigen_e=Tri.subDiagonal();
		e->SetPar(0, res);
		e->SetPar(1, D);
		e->SetPar(2, E);
	  } else {
		DDoubleGDL* a = e->GetParAs<DDoubleGDL>(0);
		Map<Matrix<double,Dynamic,Dynamic,RowMajor> >  A(&(*a)[0], n,n);
		Tridiagonalization<MatrixXd> Tri(A);
		DDoubleGDL* res = new DDoubleGDL(a->Dim(),BaseGDL::NOZERO);
		Map<MatrixXd> R(&(*res)[0],n,n);
		R=Tri.matrixQ().transpose();
		DDoubleGDL* D = new DDoubleGDL(dimension(n), BaseGDL::NOZERO);
		Map<VectorXd> eigen_d(&(*D)[0],n);
		eigen_d=Tri.diagonal();
		DDoubleGDL* E = new DDoubleGDL(dimension(n), BaseGDL::ZERO);
		Map<VectorXd> eigen_e(&(*E)[0],n-1);
		eigen_e=Tri.subDiagonal();
		e->SetPar(0, res);
		e->SetPar(1, D);
		e->SetPar(2, E);
	  }
	} else {
	  if (ComplexType(p0->Type())) {
		DComplexGDL* a = e->GetParAs<DComplexGDL>(0);
		Map<Matrix<std::complex<DFloat>,Dynamic,Dynamic,RowMajor> >  A(&(*a)[0], n,n);
		Tridiagonalization<MatrixXcf> Tri(A);
		DComplexGDL* res = new DComplexGDL(a->Dim(),BaseGDL::NOZERO);
		Map<MatrixXcf> R(&(*res)[0],n,n);
		R=Tri.matrixQ().transpose();
		DFloatGDL* D = new DFloatGDL(dimension(n), BaseGDL::NOZERO);
		Map<VectorXf> eigen_d(&(*D)[0],n);
		eigen_d=Tri.diagonal();
		DFloatGDL* E = new DFloatGDL(dimension(n), BaseGDL::ZERO);
		Map<VectorXf> eigen_e(&(*E)[0],n-1);
		eigen_e=Tri.subDiagonal();
		e->SetPar(0, res);
		e->SetPar(1, D);
		e->SetPar(2, E);
	  } else {
		DFloatGDL* a = e->GetParAs<DFloatGDL>(0);
		Map<Matrix<float,Dynamic,Dynamic,RowMajor> >  A(&(*a)[0], n,n);
		Tridiagonalization<MatrixXf> Tri(A);
		DFloatGDL* res = new DFloatGDL(a->Dim(),BaseGDL::NOZERO);
		Map<MatrixXf> R(&(*res)[0],n,n);
		R=Tri.matrixQ().transpose();
		DFloatGDL* D = new DFloatGDL(dimension(n), BaseGDL::NOZERO);
		Map<VectorXf> eigen_d(&(*D)[0],n);
		eigen_d=Tri.diagonal();
		DFloatGDL* E = new DFloatGDL(dimension(n), BaseGDL::NOZERO);
		Map<VectorXf> eigen_e(&(*E)[0],n-1);
		eigen_e=Tri.subDiagonal();
		e->SetPar(0, res);
		e->SetPar(1, D);
		e->SetPar(2, E);
	  }
	}
  }
}
#endif

#include "snippets/elmhes.incpp"
namespace lib {
    BaseGDL* elmhes_fun(EnvT* e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);

	/*********************************Checking_if_arguments_are_OK*********************/

	if (p0->N_Elements() == 0)
	  e->Throw("Variable A is undefined: " + e->GetParString(0));
	if (p0->Rank() == 2) {
	  if (p0->Dim(0) != p0->Dim(1))
		e->Throw("Argument A must be a square matrix:" + e->GetParString(0));
	} else e->Throw("Argument A must be a square matrix:" + e->GetParString(0));

	static int COLUMN = e->KeywordIx("COLUMN");
	bool columnMajor = e->KeywordSet(COLUMN);
	static int DOUBLE = e->KeywordIx("DOUBLE");
	bool doDouble = e->KeywordSet(DOUBLE);
	static int NO_BALANCE = e->KeywordIx("NO_BALANCE");
	bool noBalance = e->KeywordSet(NO_BALANCE);

	int nm=p0->Dim(0);
	int n=nm;
	int low=1;
	int igh=n;
	
	if (doDouble || p0->Type() == GDL_DOUBLE || p0->Type() == GDL_COMPLEXDBL) {
	  
	  DDoubleGDL* a = e->GetParAs<DDoubleGDL>(0);
	  DDoubleGDL* res;
      res=(columnMajor)?a->Dup():(DDoubleGDL*)a->Transpose(NULL);
	  DDouble* data = static_cast<DDouble*> (res->DataAddr()); //which is now a
	  DDouble scale[n];
	  int status;
	  int int__[n];
	  if (!noBalance) status=balanc_(&nm,&n,data,&low,&igh,scale);
	  status=elmhes_(&nm, &n, &low, &igh, data, int__);
	  if (columnMajor) return res; else return res->Transpose(NULL);
	}
	else {
	  DFloatGDL* a = e->GetParAs<DFloatGDL>(0);
	  DFloatGDL* res;
      res=(columnMajor)?a->Dup():(DFloatGDL*)a->Transpose(NULL);
	  DFloat* data = static_cast<DFloat*> (res->DataAddr()); //which is now a
	  DFloat scale[n];
	  int status;
	  int int__[n];
	  if (!noBalance) status=balanc_(&nm,&n,data,&low,&igh,scale);
	  status=elmhes_(&nm, &n, &low, &igh, data, int__);
	  if (columnMajor) return res; else return res->Transpose(NULL);
	}
	return NULL;
  }

	BaseGDL* hqr_fun(EnvT* e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);

	/*********************************Checking_if_arguments_are_OK*********************/

	if (p0->N_Elements() == 0)
	  e->Throw("Variable A is undefined: " + e->GetParString(0));
	if (p0->Rank() == 2) {
	  if (p0->Dim(0) != p0->Dim(1))
		e->Throw("Argument A must be a square matrix:" + e->GetParString(0));
	} else e->Throw("Argument A must be a square matrix:" + e->GetParString(0));

	static int COLUMN = e->KeywordIx("COLUMN");
	bool columnMajor = e->KeywordSet(COLUMN);
	static int DOUBLE = e->KeywordIx("DOUBLE");
	bool doDouble = e->KeywordSet(DOUBLE);

	int nm=p0->Dim(0);
	int n=nm;
	int low=1;
	int igh=n;
	
	if (doDouble || p0->Type() == GDL_DOUBLE || p0->Type() == GDL_COMPLEXDBL) {
	  DDoubleGDL* a = e->GetParAs<DDoubleGDL>(0);
	  DDoubleGDL* val=(columnMajor)?a->Dup():(DDoubleGDL*)a->Transpose(NULL);
	  DDouble* data = static_cast<DDouble*> (val->DataAddr());
	  DDouble scale[n];
	  DDouble wr[n];
	  DDouble wi[n];
	  int status;
	  int ierr[n];
	  status=hqr_(&nm, &n, &low, &igh, data, wr, wi,ierr);
	  DComplexDblGDL* res=new DComplexDblGDL(dimension(n),BaseGDL::NOZERO);
	  for (auto i=0; i<n; ++i) {
		(*res)[i].real(wr[i]);
		(*res)[i].imag(wi[i]);
	  }
	  return res;
	} else {
	  DFloatGDL* a = e->GetParAs<DFloatGDL>(0);
	  DFloatGDL* val=(columnMajor)?a->Dup():(DFloatGDL*)a->Transpose(NULL);
	  DFloat* data = static_cast<DFloat*> (val->DataAddr());
	  DFloat scale[n];
	  DFloat wr[n];
	  DFloat wi[n];
	  int status;
	  int ierr[n];
	  status=hqr_(&nm, &n, &low, &igh, data, wr, wi,ierr);
	  DComplexGDL* res=new DComplexGDL(dimension(n),BaseGDL::NOZERO);
	  for (auto i=0; i<n; ++i) {
		(*res)[i].real(wr[i]);
		(*res)[i].imag(wi[i]);
	  }
	  return res;
	}

	return NULL;
  }

  void trired_pro(EnvT* e) {
  	SizeT nParam = e->NParam(3);
	BaseGDL* p0 = e->GetParDefined(0);

	/*********************************Checking_if_arguments_are_OK*********************/

	if (p0->N_Elements() == 0)
	  e->Throw("Variable A is undefined: " + e->GetParString(0));
	if (p0->Rank() == 2) {
	  if (p0->Dim(0) != p0->Dim(1))
		e->Throw("Argument A must be a square matrix:" + e->GetParString(0));
	} else e->Throw("Argument A must be a square matrix:" + e->GetParString(0));

	static int DOUBLE = e->KeywordIx("DOUBLE");
	bool doDouble = e->KeywordSet(DOUBLE);
	int nm=p0->Dim(0);
	int n=nm;
	
	if (doDouble || p0->Type() == GDL_DOUBLE || p0->Type() == GDL_COMPLEXDBL) {
	  DDoubleGDL* a = e->GetParAs<DDoubleGDL>(0);
	  DDoubleGDL* res;
      res=(DDoubleGDL*)a->Transpose(NULL);
	  DDouble* data = static_cast<DDouble*> (res->DataAddr()); //which is now a
	  int status;
	  DDoubleGDL* D=new DDoubleGDL(dimension(n),BaseGDL::NOZERO);
	  DDoubleGDL* E=new DDoubleGDL(dimension(n),BaseGDL::NOZERO);
	  status=tred2_(&nm, &n, data, (DDouble*)D->DataAddr(),  (DDouble*)E->DataAddr(), data);
	  e->SetPar(0,res->Transpose(NULL));
	  e->SetPar(1,D);
	  e->SetPar(2,E);
	} else {
	  DFloatGDL* a = e->GetParAs<DFloatGDL>(0);
	  DFloatGDL* res;
      res=(DFloatGDL*)a->Transpose(NULL);
	  DFloat* data = static_cast<DFloat*> (res->DataAddr()); //which is now a
	  int status;
	  DFloatGDL* D=new DFloatGDL(dimension(n),BaseGDL::NOZERO);
	  DFloatGDL* E=new DFloatGDL(dimension(n),BaseGDL::NOZERO);
	  status=tred2_(&nm, &n, data, (DFloat*)D->DataAddr(),  (DFloat*)E->DataAddr(), data);
	  e->SetPar(0,res->Transpose(NULL));
	  e->SetPar(1,D);
	  e->SetPar(2,E);
	}
}
  void triql_pro(EnvT* e) {
  	SizeT nParam = e->NParam(3);
	BaseGDL* p2 = e->GetParDefined(2); //TRIQL, D, E, A 

	/*********************************Checking_if_arguments_are_OK*********************/

	if (p2->N_Elements() == 0)
	  e->Throw("Variable A is undefined: " + e->GetParString(2));
	if (p2->Rank() == 2) {
	  if (p2->Dim(0) != p2->Dim(1))
		e->Throw("Argument A must be a square matrix:" + e->GetParString(2));
	} else e->Throw("Argument A must be a square matrix:" + e->GetParString(2));

	BaseGDL* p0 = e->GetParDefined(0); //TRIQL, D, E, A 
	if (p0->N_Elements() == 0)
	  e->Throw("Variable A is undefined: " + e->GetParString(0));
	if (p0->Rank() == 1) {
	  if (p0->Dim(0) != p2->Dim(0))
		e->Throw("Argument D does not have correct size:" + e->GetParString(0));
	} else e->Throw("Argument D must be a column vector:" + e->GetParString(0));

	BaseGDL* p1 = e->GetParDefined(1); //TRIQL, D, E, A 
	if (p1->N_Elements() == 0)
	  e->Throw("Variable E is undefined: " + e->GetParString(1));
	if (p1->Rank() == 1) {
	  if (p1->Dim(0) != p2->Dim(0))
		e->Throw("Argument E does not have correct size:" + e->GetParString(1));
	} else e->Throw("Argument E must be a column vector:" + e->GetParString(1));

	static int DOUBLE = e->KeywordIx("DOUBLE");
	bool doDouble = e->KeywordSet(DOUBLE);
	int nm=p2->Dim(0);
	int n=nm;
	
	if (doDouble || p2->Type() == GDL_DOUBLE || p2->Type() == GDL_COMPLEXDBL) {
	  DDoubleGDL* a = e->GetParAs<DDoubleGDL>(2);
	  DDoubleGDL* res;
      res=(DDoubleGDL*)a->Transpose(NULL);
	  DDouble* data = static_cast<DDouble*> (res->DataAddr()); //which is now a
	  int status;
	  int ierr;
	  DDoubleGDL* D = static_cast<DDoubleGDL*>	(p0->Convert2(GDL_DOUBLE , BaseGDL::COPY));
	  DDoubleGDL* E = static_cast<DDoubleGDL*>	(p1->Convert2(GDL_DOUBLE , BaseGDL::COPY));
	  status=tql2_(&nm, &n, (DDouble*)D->DataAddr(),  (DDouble*)E->DataAddr(), data, &ierr);
	  if (e->GlobalPar(0)) e->SetPar(0,D);
	  if (e->GlobalPar(1)) e->SetPar(1,E);
	  if (e->GlobalPar(2)) e->SetPar(2,res);
	} else {
	  DFloatGDL* a = e->GetParAs<DFloatGDL>(2);
	  DFloatGDL* res;
      res=(DFloatGDL*)a->Transpose(NULL);
	  DFloat* data = static_cast<DFloat*> (res->DataAddr()); //which is now a
	  int status;
	  int ierr;
	  DFloatGDL* D = static_cast<DFloatGDL*>	(p0->Convert2(GDL_FLOAT , BaseGDL::COPY));
	  DFloatGDL* E = static_cast<DFloatGDL*>	(p1->Convert2(GDL_FLOAT , BaseGDL::COPY));
	  status=tql2_(&nm, &n, (DFloat*)D->DataAddr(),  (DFloat*)E->DataAddr(), data, &ierr);
	  if (e->GlobalPar(0)) e->SetPar(0,D);
	  if (e->GlobalPar(1)) e->SetPar(1,E);
	  if (e->GlobalPar(2)) e->SetPar(2,res);
	}
 }
}
