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

#include "sparse_matrix.hpp"

#if !defined(USE_EIGEN)

namespace lib { 

  BaseGDL* sprsin_fun(EnvT* e) {
    Message("GDL compiled without Eigen3 : SPRSIN not avaialble");
    return new DIntGDL(0);
  }
  BaseGDL* sprsax_fun(EnvT* e){
    Message("GDL compiled without Eigen3 : SPRSAX not avaialble");
    return new DIntGDL(0);
  }  
  BaseGDL* sprsab_fun(EnvT* e) {
    Message("GDL compiled without Eigen3 : SPRSAB not avaialble");
    return new DIntGDL(0);
  }
  BaseGDL* sprstp_fun(EnvT* e) {
    Message("GDL compiled without Eigen3 : SPRSTP not avaialble");
    return new DIntGDL(0);
  }
  BaseGDL* fulstr_fun(EnvT* e) {
    Message("GDL compiled without Eigen3 : SPRSTR not avaialble");
    return new DIntGDL(0);
  }
  BaseGDL* linbcg_fun(EnvT* e) {
    Message("GDL compiled without Eigen3 : LINBCG not avaialble");
     return new DIntGDL(0);
  }
}

#else

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <vector>

typedef Eigen::SparseMatrix<double, Eigen::RowMajor> SPMATRowMajDbl;
typedef Eigen::SparseVector<double> SPVecDbl;
static const float f_thr=1E-7;
static const double d_thr=1E-14;

//To save time, the 'external' (i.e., as a GDL variable) representation of a Eigen::SPMatrix is just a DPtr to the Eigen::SPMatrix itself
//I have not checked if the reference counting of Ptrs is sufficient to destroy the SPMatrix when the Ptr is destroyed.
//Otherwise this code may lead to leaks.
typedef std::vector<SPMATRowMajDbl>::iterator MatVecIterator;
namespace lib { 
  
   SPMATRowMajDbl* getFromPtr(EnvT* e, DUInt i) {
   BaseGDL* p0 = e->GetParDefined(i); // must be struct
    DType varType = p0->Type();
    if (varType != GDL_PTR) e->Throw("Expression " + e->GetString(i) + "must be a PTR in this context.");
	DPtrGDL* s = e->GetParAs<DPtrGDL>(i);
    return *((SPMATRowMajDbl**)(s->DataAddr()));
  }
 
  //Version where the variable exchanged is a special structure. useful for save/restore, but slows down process in all other cases
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
  //takes a SparseMatrix and returns the full GDL variable.
  BaseGDL* convertToGDL(SPMATRowMajDbl* Mat) {
  int ncols = Mat->cols();
  int nrows = Mat->rows();
  DDoubleGDL* ret = new DDoubleGDL(dimension(ncols, nrows), BaseGDL::ZERO);
  const	int outs=Mat->outerSize();
  const int* outerStartIndexPtr = Mat->outerIndexPtr();
  const int* innerIndicesPtr = Mat->innerIndexPtr();
  const double* values = Mat->valuePtr();
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
  
  //Version where the variable exchanged is a special structure. useful for save/restore, but slows down process in all other cases  
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
  
  DPtrGDL* convertToPtr(const SPMATRowMajDbl *Mat) {
   DPtr pointer = (DPtr)Mat; //(DPtr)(MatVec.data());
   return new DPtrGDL(pointer);
 }
 
  BaseGDL* sprsin_fun(EnvT* e) {
	static bool warned=false;
	SizeT nParam = e->NParam(); //1 or 4
	if (nParam != 1 && nParam != 4) e->Throw("Incorrect number of arguments.");
	double thresh=d_thr;
	static int COLUMN = e->KeywordIx("COLUMN");
	if (e->KeywordSet(COLUMN)) {
	  e->Throw("GDL's SPRSIN does not yet support the COLUMN keyword. Please report or use transposed arrays.");
	}
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
	  if (nCols != nRows && !warned) {
		Message("NOTE: use of SPRSIN with non-square matrices is a GDL extension.");
		warned=true;
	  }
	  DDoubleGDL* var = e->GetParAs<DDoubleGDL>(0);
	  SPMATRowMajDbl *Mat= new SPMATRowMajDbl(nRows,nCols);
	  //import and prune wrt. threshold
	  for (auto i=0; i<nCols; ++i) for (auto j=0; j<nRows; ++j) if (fabs((*var)[j*nCols+i])>=thresh) Mat->insert(j,i)= (*var)[j*nCols+i];
	  Mat->makeCompressed();
	  return convertToPtr(Mat);
	} else if (nParam == 4) {
	  DLongGDL* cols = e->GetParAs<DLongGDL>(0);
	  int nCols=cols->N_Elements();
	  DLongGDL* rows = e->GetParAs<DLongGDL>(1);
	  int nRows=rows->N_Elements();
	  if (nCols != nRows) e->Throw("Vector "+e->GetString(1) + " must have "+ i2s(nCols) + " elements.");
	  DDoubleGDL* vals = e->GetParAs<DDoubleGDL>(2);
	  int nVals=vals->N_Elements();
	  if (nVals != nRows) e->Throw("Vector "+e->GetString(2) + " must have "+ i2s(nCols) + " elements.");
	  DLongGDL* sizegdl = e->GetParAs<DLongGDL>(3);
	  DLong size=(*sizegdl)[0];
      SPMATRowMajDbl *Mat= new SPMATRowMajDbl(size,size); //only square matrices this way.
	  Mat->reserve(nCols);
	  for (auto i=0; i< nCols; ++i) {
		DLong irow=(*rows)[i];
		if (irow >= size ) e->Throw(" Out of range subscript encountered: "+e->GetString(0)+" .");
		DLong icol=(*cols)[i];
		if (icol >= size ) e->Throw(" Out of range subscript encountered: "+e->GetString(1)+" .");
		Mat->coeffRef(irow,icol)+=(*vals)[i]; //protect against doublons
		if (Mat->coeffRef(irow,icol)!=(*vals)[i]) e->Throw("Duplicate subscript encountered in Columns and Rows arrays at element: "+i2s(i));
	  }
	  return convertToPtr(Mat);
	} else e->Throw("Incorrect number of arguments.");
	
	return new DLongGDL(0);
  }
  
  BaseGDL* sprsab_fun(EnvT* e){	
    SizeT nParam = e->NParam(2);
	double thresh=d_thr;
	static int THRESHOLD=e->KeywordIx("THRESHOLD");
	if (e->KeywordSet(THRESHOLD)){
	  e->AssureDoubleScalarKW(THRESHOLD,thresh);
	}
	SPMATRowMajDbl *Mat1=getFromPtr(e, 0);
	SPMATRowMajDbl *Mat2=getFromPtr(e, 1);
	SPMATRowMajDbl *Mat3=new SPMATRowMajDbl((*Mat1)*(*Mat2));
	Mat3->prune(thresh);
	return convertToPtr(Mat3);
  }
  
  BaseGDL* sprstp_fun(EnvT* e){	
    SizeT nParam = e->NParam(1);
	SPMATRowMajDbl *Mat=getFromPtr(e, 0);
	SPMATRowMajDbl *res=new SPMATRowMajDbl((*Mat).transpose());
	return convertToPtr(res);
  }
  BaseGDL* sprsax_fun(EnvT* e) {
	SizeT nParam = e->NParam(2);
	SPMATRowMajDbl* Mat = getFromPtr(e, 0);
	BaseGDL* p1 = e->GetParDefined(1); // vector
	DType varType = p1->Type();
	if (p1->Dim().Rank() != 1) e->Throw("Argument " + e->GetString(1) + " must be a vector.");
	if (varType == GDL_STRING) e->Throw("Argument " + e->GetString(1) + " must not be of STRING type.");
	int m = p1->Dim(0);
	if (m != Mat->cols()) e->Throw("Argument " + e->GetString(1) + " does not have correct size.");
	DDoubleGDL* var = e->GetParAs<DDoubleGDL>(1);
	SPVecDbl Vec(m);
	//import 
	for (auto i=0; i<m; ++i) Vec.insert(i)= (*var)[i];
	SPMATRowMajDbl* Mat3 = new SPMATRowMajDbl(((*Mat)*Vec).transpose());
	return convertToGDL(Mat3);
  }
  
  BaseGDL* linbcg_fun(EnvT* e) {
	SizeT nParam = e->NParam(3);
	SPMATRowMajDbl* A = getFromPtr(e, 0);
	BaseGDL* p1 = e->GetParDefined(1); // Right Hand B 
	DType varType = p1->Type();
	if (p1->Dim().Rank() != 1) e->Throw("Argument " + e->GetString(1) + " must be a vector.");
	if (varType == GDL_STRING) e->Throw("Argument " + e->GetString(1) + " must not be of STRING type.");
	int m = p1->Dim(0);
	if (m != A->cols()) e->Throw("Argument " + e->GetString(1) + " does not have correct size.");
	DDoubleGDL* Bgdl = e->GetParAs<DDoubleGDL>(1);
	Eigen::Map<Eigen::VectorXd> B(&(*Bgdl)[0], m);
	BaseGDL* p2 = e->GetParDefined(2); // Initial Guess
	varType = p2->Type();
	if (p2->Dim().Rank() != 1) e->Throw("Argument " + e->GetString(2) + " must be a vector.");
	if (varType == GDL_STRING) e->Throw("Argument " + e->GetString(2) + " must not be of STRING type.");
	int n = p2->Dim(0);
	if (n != m) e->Throw("Argument " + e->GetString(2) + " does not have correct size.");
	DDoubleGDL* Xgdl = e->GetParAs<DDoubleGDL>(2);
	Eigen::Map<Eigen::VectorXd> X(&(*Xgdl)[0], n); 
	//solve ax=b
	Eigen::SparseLU<Eigen::SparseMatrix<double>> sparseLU;
    sparseLU.compute(*A);
    if(sparseLU.info()!=Eigen::Success) e->Throw("Matrix decomposition failed.");
	X=sparseLU.solve(B);
    if(sparseLU.info()!=Eigen::Success) e->Throw("No solution found.");
	DDoubleGDL* resD =new DDoubleGDL(n, BaseGDL::NOZERO);
	Eigen::Map<Eigen::VectorXd>(&(*resD)[0], n) = X;
	return resD;
  }
 
  BaseGDL* fulstr_fun(EnvT* e){
	SizeT nParam = e->NParam(1);
//    SPMATRowMajDbl Mat=getFromIndexInMatVec(e, 0);
    SPMATRowMajDbl *Mat=getFromPtr(e, 0);
	return convertToGDL(Mat);
  }
}

#endif
