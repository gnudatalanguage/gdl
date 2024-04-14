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

#include "elmhes.hpp"
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
}
