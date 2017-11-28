/***************************************************************************
                          linearprogramming.cpp  -  GDL library function
                             -------------------
    begin                : Jul 12 2017
    copyright            : (C) 2017 by Gilles Duvert as interface to GLPK lib.

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
#include "datatypes.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"


//#if defined(HAVE_GLPK)
namespace lib {

  using namespace std;

#ifdef HAVE_GLPK
#define USE_GLPK 1
#include <glpk.h>
#else
#define USE_GLPK 0
#endif

  BaseGDL* simplex(EnvT* e) {

#ifndef USE_GLPK
    e->Throw("GDL was compiled without support for GLPK");
    return NULL;
#else
    // sanity check (for number of parameters)
    SizeT nParam = e->NParam();
    if (nParam < 5) e->Throw("Incorrect number of arguments.");
    // 1-st argument : equation parameters; N parameters
    BaseGDL* p0 = e->GetParDefined(0);
    DDoubleGDL* zEquation = (DDoubleGDL*) p0->Convert2(GDL_DOUBLE, BaseGDL::COPY);
    Guard<DDoubleGDL> zEquation_guard(zEquation);
    DLong n = zEquation->N_Elements();
    // 2nd argument : constraints: a N+1 cols by M rows table
    BaseGDL* p1 = e->GetParDefined(1);
    DDoubleGDL* constraints = (DDoubleGDL*) p1->Convert2(GDL_DOUBLE, BaseGDL::COPY);
    Guard<DDoubleGDL> constraints_guard(constraints);
    if (constraints->Rank() != 2) e->Throw("Constraints must be a Rank 2 matrix.");
    if (constraints->Dim(0) != n + 1) e->Throw("Constraints's first dimension must be " + i2s(n + 1));
    DLong m = constraints->Dim(1);
    //3rd argument: m1, the number of less-than constraints (first m1 lines) 
    DLong m1 = 0;
    e->AssureLongScalarPar(2, m1);
    //idem for m2 and m3, resp. greater-than and equal-to
    DLong m2 = 0;
    e->AssureLongScalarPar(3, m2);
    DLong m3 = 0;
    e->AssureLongScalarPar(4, m3);
    if (m1 + m2 + m3 != m) e->Throw("Sum of m1, m2 a,d m3 must be equal to " + i2s(m));
    static int DOUBLEIx = e->KeywordIx("DOUBLE");
    bool isDouble = (p0->Type() == GDL_DOUBLE);
    if (e->KeywordSet(DOUBLEIx)) isDouble = true;

// We do not neeed eps at all.
//    static int EPSIx = e->KeywordIx("EPS");
//    DDouble eps = isDouble ? 3E-10 : 3E-8;
//    bool doEps=false;
//    if (e->KeywordPresent(EPSIx)) {
//      e->AssureDoubleScalarKW(EPSIx, eps);
//      doEps=true;
//    }

    glp_prob *theProblem;

    theProblem = glp_create_prob();
    glp_set_prob_name(theProblem, "GDL");
    glp_set_obj_dir(theProblem, GLP_MAX);
    glp_add_rows(theProblem, m1 + m2 + m3);
    for (int i = 1; i < m1+1; ++i) //their numbering starts at 1
    {
      string tmp = "up" + i2s(i);
      glp_set_row_name(theProblem, i, tmp.c_str());
      glp_set_row_bnds(theProblem, i, GLP_UP, 0.0, (*constraints)[(i-1) * (n + 1)]);
    }
    for (int i = m1+1; i < m1 + m2 +1; ++i) //their numbering starts at 1
    {
      string tmp = "lo" + i2s(i);
      glp_set_row_name(theProblem, i, tmp.c_str());
      glp_set_row_bnds(theProblem, i, GLP_LO, (*constraints)[(i-1) * (n + 1)], 0.0);
    }
    for (int i = m1 + m2 +1 ; i < m+1 ; ++i) //their numbering starts at 1
    {
      string tmp = "eq" + i2s(i);
      glp_set_row_name(theProblem, i, tmp.c_str());
      glp_set_row_bnds(theProblem, i, GLP_FX, (*constraints)[(i-1) * (n + 1)], (*constraints)[(i-1) * (n + 1)]);
    }
    glp_add_cols(theProblem, n);
    for (int i = 1; i < n+1; ++i) //their numbering starts at 1
    {
      string tmp = "X" + i2s(i);
      glp_set_col_name(theProblem, i, tmp.c_str());
      glp_set_obj_coef(theProblem, i, (*zEquation)[i-1]); //their numbering starts at 1
      glp_set_col_bnds(theProblem, i, GLP_LO, 0.0, 0.0);
    }

    int ia[n * m], ja[n * m];
    double ar[n * m], z, x1, x2, x3;
    int k = 1; //their numbering starts at 1
    for (int j = 0; j < m; ++j)
    {
      for (int i = 0; i < n; ++i)
      {
        ia[k] = j+1;
        ja[k] = i+1;
        ar[k] = -1.0*(*constraints)[1 + i + j * (n + 1)]; //sign is inverse from IDL
        k++;
      }
    }
    glp_load_matrix(theProblem, n*m, ia, ja, ar);
    glp_smcp parm;
    glp_init_smcp(&parm);
    parm.msg_lev=GLP_MSG_OFF;
    int returned=glp_simplex(theProblem, &parm);

    DLong status=0;
    static int STATUSIx = e->KeywordIx("STATUS");
    bool returnStatus=e->KeywordPresent(STATUSIx);
    
    switch (glp_get_status(theProblem)) {
      case GLP_OPT:
      case GLP_FEAS:
        status=0;
        break;
      case GLP_UNBND:
        status=1;
        break;
      case GLP_INFEAS:
      case GLP_NOFEAS:
      case GLP_UNDEF:
        status=2;
    }

    if (returnStatus) {
      if (returned==0) e->SetKW(STATUSIx, new DLongGDL(status)); else  e->SetKW(STATUSIx, new DLongGDL(3));
    }
    
//tableau etc not yet ready.    
//    if (nParam >= 6) {
//      SizeT dims[2];
//      dims[0] = n;
//      dims[1] = m + 2;
//      if (isDouble)
//      {
//        DDoubleGDL* tab = new DDoubleGDL(dimension(dims,2));
//        e->SetPar(5, tab);
//      } else {
//        DFloatGDL* tab = new DFloatGDL(dimension(dims,2));
//        e->SetPar(5, tab);
//      }
//    }
//    if (nParam >= 7) {
//      if (isDouble)
//      {
//        DDoubleGDL* izrov = new DDoubleGDL(dimension(n));
//        e->SetPar(6, izrov);
//      } else {
//        DFloatGDL* izrov = new DFloatGDL(dimension(n));
//        e->SetPar(6, izrov);
//      }
//    }
//    if (nParam == 8) {
//      if (isDouble)
//      {
//        DDoubleGDL* iposv = new DDoubleGDL(dimension(n));
//        e->SetPar(7, iposv);
//      } else {
//        DFloatGDL* iposv = new DFloatGDL(dimension(n));
//        e->SetPar(7, iposv);
//      }
//    }
    BaseGDL* ret;
    if (isDouble) {
        ret  = new DDoubleGDL(dimension(n+1));
        (*(DDoubleGDL*)ret)[0] = glp_get_obj_val(theProblem);
        for (int i = 1; i < n+1; ++i) (*(DDoubleGDL*)ret)[i] = glp_get_col_prim(theProblem, i);
    } else {
        ret  = new DFloatGDL(dimension(n+1));
        (*(DFloatGDL*)ret)[0] = glp_get_obj_val(theProblem);
        for (int i = 1; i < n+1; ++i) (*(DFloatGDL*)ret)[i] = glp_get_col_prim(theProblem, i);
    }
    return ret;
#endif
  }
}


