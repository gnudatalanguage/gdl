/***************************************************************************
						  sparse_matrix.cpp  -  GDL sparse matrix functions
							 -------------------
	begin                : Dec 9 2023
	copyright            : (C) 2023 by Gilles Duvert
	email                : surname dot name at free dot fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <vector>
#include "sparse_matrix.hpp"

typedef Eigen::SparseMatrix<double, Eigen::RowMajor> SPMATRowMajDbl;
typedef Eigen::SparseVector<double> SPVecDbl;
static const float f_thr=1E-7;
static const double d_thr=1E-14;
namespace lib { 

  SPMATRowMajDbl getFromStruct(EnvT* e, DUInt i) {
	BaseGDL* p0 = e->GetParDefined(i); // must be struct
    DType varType = p0->Type();
    if (varType != GDL_STRUCT) e->Throw("Expression " + e->GetString(i) + "must be a structure in this context.");
	DStructGDL* s = e->GetParAs<DStructGDL>(i);
	DLongGDL* N = static_cast<DLongGDL*> (s->GetTag(0));
	int nCols = (*N)[0];
	int nRows = (*N)[1];
	int nnz = (*static_cast<DLongGDL*> (s->GetTag(1)))[0];
	if (nnz > 0 ) { //protect against 0
	double* values = static_cast<double*> (s->GetTag(2)->DataAddr());
	int* innerIndicesPtr = static_cast<int*> (s->GetTag(3)->DataAddr());
	int* outerIndexPtr = static_cast<int*> (s->GetTag(4)->DataAddr());
	Eigen::Map<SPMATRowMajDbl> Mat(nRows, nCols,  nnz, outerIndexPtr, innerIndicesPtr, values);
	  return Mat;
	} else {
	  SPMATRowMajDbl Mat(nRows, nCols);
	  return Mat;
	}
  }
  BaseGDL* convertToGDL(SPMATRowMajDbl Mat) {
  int ncols = Mat.cols();
  int nrows = Mat.rows();
  DDoubleGDL* ret = new DDoubleGDL(dimension(ncols, nrows), BaseGDL::ZERO);
  const	int outs=Mat.outerSize();
  const int* outerStartIndexPtr = Mat.outerIndexPtr();
  const int* innerIndicesPtr = Mat.innerIndexPtr();
  const double* values = Mat.valuePtr();
  for (auto iRow = 0; iRow < outs; ++iRow) { 
	int jValmin = outerStartIndexPtr[iRow];
	int jValmax = outerStartIndexPtr[iRow + 1];
	for (int kk = jValmin; kk < jValmax; ++kk) { //outerstart
	  int iCol = innerIndicesPtr[kk]; //row
	  (*ret)[iRow * ncols + iCol] = values[kk];
	}
  }
  return ret;
  }
  
  DStructGDL* convertToStruct(const SPMATRowMajDbl Mat) {
  int nCols = Mat.cols();
  int nRows = Mat.rows();
	DStructDesc* sd = new DStructDesc("$truct");
	DStructGDL* s = new DStructGDL(sd, dimension(1));
	DLongGDL* Dim = new DLongGDL(dimension(2), BaseGDL::NOZERO);
	(*Dim)[0] = nCols;
	(*Dim)[1] = nRows;
	s->NewTag("DIM", Dim);
	DLong nnz = Mat.nonZeros();
	s->NewTag("NNZ", new DLongGDL(nnz));
	//protect against 0
	if (nnz > 0) {
	  DDoubleGDL* Values = new DDoubleGDL(dimension(nnz), BaseGDL::NOZERO);
	  for (auto i = 0; i < nnz; ++i) (*Values)[i] = Mat.valuePtr()[i];
	  s->NewTag("VALUES", Values);
	  DLongGDL* InnerIndices = new DLongGDL(dimension(nnz), BaseGDL::NOZERO);
	  for (auto i = 0; i < nnz; ++i) (*InnerIndices)[i] = Mat.innerIndexPtr()[i];
	  s->NewTag("INNER_INDICES", InnerIndices);
	  int outs=Mat.outerSize();
	  DLongGDL* OuterStarts = new DLongGDL(dimension(outs+1), BaseGDL::NOZERO);
	  for (auto i = 0; i < outs+1; ++i) (*OuterStarts)[i] = Mat.outerIndexPtr()[i];
	  s->NewTag("OUTER_STARTS", OuterStarts);
	}
	return s;
 }

  BaseGDL* sprsin_fun(EnvT* e) {
	SizeT nParam = e->NParam(); //1 or 4
	if (nParam != 1 && nParam != 4) e->Throw("Incorrect number of arguments.");
	double thresh=d_thr;
	static int THRESHOLD=e->KeywordIx("THRESHOLD");
	if (e->KeywordSet(THRESHOLD)){
	  e->AssureDoubleScalarKW(THRESHOLD,thresh);
	}
	if (nParam == 1) {
      BaseGDL* p0 = e->GetParDefined(0); // matrix
	  DType varType = p0->Type();
	  if (p0->Dim().Rank() != 2) e->Throw("Argument " + e->GetString(0) + " must be a square matrix.");
      if (varType == GDL_STRING) e->Throw("Argument " + e->GetString(0) + " must not be of STRING type.");
	  int nCols = p0->Dim(0);
	  int nRows = p0->Dim(1);
	  DDoubleGDL* var = e->GetParAs<DDoubleGDL>(0);
	  SPMATRowMajDbl Mat(nRows,nCols);
	  //import and prune wrt. threshold
	  for (auto i=0; i<nCols; ++i) for (auto j=0; j<nRows; ++j) if (fabs((*var)[j*nCols+i])>=thresh) Mat.insert(j,i)= (*var)[j*nCols+i];
	  Mat.makeCompressed();
	  return convertToStruct(Mat);
	}
	return new DLongGDL(0);
  }
  
  BaseGDL* sprsab_fun(EnvT* e){	
    SizeT nParam = e->NParam(2);
	double thresh=d_thr;
	static int THRESHOLD=e->KeywordIx("THRESHOLD");
	if (e->KeywordSet(THRESHOLD)){
	  e->AssureDoubleScalarKW(THRESHOLD,thresh);
	}
	SPMATRowMajDbl Mat1=getFromStruct(e, 0);
	SPMATRowMajDbl Mat2=getFromStruct(e, 1);
	SPMATRowMajDbl Mat3=(Mat1*Mat2).pruned(thresh);
	return convertToStruct(Mat3);
  }
  BaseGDL* sprsax_fun(EnvT* e) {
	SizeT nParam = e->NParam(2);
	SPMATRowMajDbl Mat1 = getFromStruct(e, 0);
	BaseGDL* p1 = e->GetParDefined(1); // vector
	DType varType = p1->Type();
	if (p1->Dim().Rank() != 1) e->Throw("Argument " + e->GetString(1) + " must be a vector.");
	if (varType == GDL_STRING) e->Throw("Argument " + e->GetString(1) + " must not be of STRING type.");
	int m = p1->Dim(0);
	if (m != Mat1.cols()) e->Throw("Argument " + e->GetString(1) + " does not have correct size.");
	DDoubleGDL* var = e->GetParAs<DDoubleGDL>(1);
	SPVecDbl Vec(m);
	//import 
	for (auto i=0; i<m; ++i) Vec.insert(i)= (*var)[i];
	SPMATRowMajDbl Mat3 = Mat1*Vec;
	return convertToGDL(Mat3.transpose());
  }
  BaseGDL* fulstr_fun(EnvT* e){
	SizeT nParam = e->NParam(1);
    SPMATRowMajDbl Mat=getFromStruct(e, 0);
	return convertToGDL(Mat);
  }
}
