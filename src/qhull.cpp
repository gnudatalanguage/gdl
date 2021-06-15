/***************************************************************************
                          qhull.cpp  -  GDL library function
                             -------------------
    begin                : Jun 09 2021
    copyright            : (C) 2021 by Eloi R.de Linage

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

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

//Qhull libraries

#include <libqhullcpp/PointCoordinates.h>
#include <libqhullcpp/RboxPoints.h>
#include <libqhullcpp/QhullError.h>
#include <libqhullcpp/QhullQh.h>
#include <libqhullcpp/QhullFacet.h>
#include <libqhullcpp/QhullFacetList.h>
#include <libqhullcpp/QhullFacetSet.h>
#include <libqhullcpp/QhullLinkedList.h>
#include <libqhullcpp/QhullPoint.h>
#include <libqhullcpp/QhullVertex.h>
#include <libqhullcpp/QhullVertexSet.h>
#include <libqhullcpp/Qhull.h>

using namespace std;

using orgQhull::Qhull;
using orgQhull::QhullError;
using orgQhull::QhullFacet;
using orgQhull::QhullFacetList;
using orgQhull::QhullFacetListIterator;
using orgQhull::QhullFacetSet;
using orgQhull::QhullFacetSetIterator;
using orgQhull::QhullPoint;
using orgQhull::QhullPoints;
using orgQhull::QhullPointsIterator;
using orgQhull::QhullQh;
using orgQhull::QhullVertex;
using orgQhull::QhullVertexList;
using orgQhull::QhullVertexListIterator;
using orgQhull::QhullVertexSet;
using orgQhull::QhullVertexSetIterator;
using orgQhull::RboxPoints;
using orgQhull::PointCoordinates;

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

namespace lib {
  void qhull ( EnvT* e)
  {
    static int delaunayIx=e->KeywordIx("DELAUNAY");
    bool isDelaunay=e->KeywordSet(delaunayIx);

    static int connIx=e->KeywordIx("CONNECTIVITY");
    bool isConn=e->KeywordSet(connIx);

    int nParam = e->NParam(2);

    Qhull qhull;

    //voir pour les erreurs A LA FIN, fairer le reste d'abord
    stringbuf err_buff;
    ostream err_stream(&err_buff);
    qhull.setErrorStream( &err_stream );

    DDoubleGDL* p0 = e->GetParAs<DDoubleGDL>(0);

    int nd=p0->Dim(0);
    int np=p0->Dim(1);

    PointCoordinates *mPoints = new PointCoordinates();

    mPoints->setDimension(nd);
    
    vector <double> allPoints;
    for (int i =0; i < p0->N_Elements(); i++){
          allPoints.push_back( (*p0)[i]);
    }

    mPoints->append(allPoints);

    int ndRes;

    if(isDelaunay) {
        qhull.runQhull( mPoints->comment().c_str(), nd, np, mPoints->coordinates(), "QJ d");
        ndRes=nd+1;
    } else {
        qhull.runQhull( mPoints->comment().c_str(), nd, np, mPoints->coordinates(), "QJ");
        ndRes=nd;
    }
    
    string err_string=err_buff.str();
    if(!(err_string == "")){
         e->Throw(err_string);
    }

    if(isConn){
    
    }
    
    QhullFacetList facets = qhull.facetList();

    DLongGDL* res = new DLongGDL( *(new dimension(ndRes, qhull.facetCount())), BaseGDL::ZERO);
    
    long long ix=0;
    long long bad_facets=0;

    for (QhullFacetList::iterator it = facets.begin(); it != facets.end(); ++it)
    {
        if (!(*it).isGood()){
          bad_facets++;
          continue;
        }
        QhullFacet f = *it;

        QhullVertexSet vSet = f.vertices();
        for (QhullVertexSet::iterator vIt = vSet.begin(); vIt != vSet.end(); ++vIt)
        {
          QhullVertex v = *vIt;
          QhullPoint p = v.point();

          (*res)[ix] = p.id();
          ix++;
        }
    }

    if(bad_facets > 0){
        if(isDelaunay){
          res->SetDim(*(new dimension(nd+1, qhull.facetCount()-bad_facets)));
        } else {
          res->SetDim(*(new dimension(nd, qhull.facetCount()-bad_facets)));
        }
    }

    e->SetPar(1, res);
  }

  BaseGDL* qgrid3_fun ( EnvT* e)
  {
    e->Throw("Please Write this function in GDL.");
    return NULL;
  }
}

