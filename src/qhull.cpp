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
#include <algorithm>

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
#include <libqhullcpp/QhullRidge.h>
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
using orgQhull::QhullRidge;
using orgQhull::QhullRidgeSet;
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
  bool sameRidge2d(vector<QhullVertex> r1, vector<QhullVertex> r2)
  {
      if(r1.at(0).point().id() == r2.at(0).point().id() && r1.at(1).point().id() == r2.at(1).point().id())
      {
        return true;
      } else if(r1.at(0).point().id() == r2.at(1).point().id() && r1.at(1).point().id() == r2.at(0).point().id()){
        return true;
      }
      return false;
  }

  void qhull ( EnvT* e)
  {
    // There might be a simpler/cleaner way to do this...
    static int connIx=e->KeywordIx("CONNECTIVITY");
    bool isConn=e->KeywordPresent(connIx);

    static int boundsIx=e->KeywordIx("BOUNDS");
    bool isBounds=e->KeywordPresent(boundsIx);

    static int vnormIx=e->KeywordIx("VNORMALS");
    bool isVnorm=e->KeywordPresent(vnormIx);

    static int vdiagIx=e->KeywordIx("VDIAGRAM");
    bool isVdiag=e->KeywordPresent(vdiagIx);

    static int vvertIx=e->KeywordIx("VVERTICES");
    bool isVvert=e->KeywordPresent(vvertIx);

    bool isVoronoi= isVnorm || isVdiag || isVvert;

    static int sphereIx=e->KeywordIx("SPHERE");
    bool isSphere=e->KeywordPresent(sphereIx);

    static int delaunayIx=e->KeywordIx("DELAUNAY");
    bool isDelaunay= ( e->KeywordSet(delaunayIx) || isVoronoi); //switch to delaunay if a voronoi keyword is set

    if(isConn & !isDelaunay) e->Throw("Keyword CONNECTIVITY requires the presence of keyword DELAUNAY/SPHERE.\nSPHERE is not implemented yet.");        
    
    if(isSphere) e->Throw("SPHERE is not implemented yet.");

    int nParam = e->NParam(2);
    int outIx = nParam-1;
  
    DDoubleGDL* p0;

    if(nParam == 2)
    {
        p0 = e->GetParAs<DDoubleGDL>(0);
    } else {
        int inDim = e->GetParAs<DDoubleGDL>(0)->Dim(0);
        p0 = new DDoubleGDL( *(new dimension(nParam-1, inDim)), BaseGDL::ZERO );

        for(int i=0; i<nParam-1; i++)
        {
          DDoubleGDL* par=e->GetParAs<DDoubleGDL>(i);
          if(par->Dim(0) != inDim || par->Dim(1) != 0 )
          {
            e->Throw("qhull input error: separated input arrays must have same length and be 1 dimensional");
          }
          for(int j=0; j<inDim; j++) (*p0)[i+j*(nParam-1)] = (*par)[j];
        }
    }

    Qhull qhull;

    int nd=p0->Dim(0);
    int np=p0->Dim(1);

    //if not enough input points for qhull, throw error
    if( np <= nd)
    {
      stringstream ssp, ss_minp;
      ssp << np;
      ss_minp << nd+1;
      string err_string="qhull input error: not enough points ("+ ssp.str() +") to construct initial simplex (need "+ ss_minp.str() +")";
      e->Throw(err_string);
    } else if ((np <= nd+1) && isDelaunay) {
      e->Throw("qhull input error: not enough points available");
    }

    PointCoordinates *mPoints = new PointCoordinates();
    mPoints->setDimension(nd);
    
    vector <double> allPoints;
    for (int i =0; i < p0->N_Elements(); i++)
    {
          allPoints.push_back( (*p0)[i]);
    }

    mPoints->append(allPoints);
    int ndRes;

    if(isDelaunay)
    {
        //QJ option prevents non-simplicial facets, it seems to be the option IDL uses
        //While Qt option triangulates facets afterwards, which gives somewhat different results in some cases
        qhull.runQhull( mPoints->comment().c_str(), nd, np, mPoints->coordinates(), "QJ d");
        ndRes=nd+1;
    } else {
        qhull.runQhull( mPoints->comment().c_str(), nd, np, mPoints->coordinates(), "QJ");
        ndRes=nd;
    }

    QhullFacetList facets = qhull.facetList();
    DLongGDL* res = new DLongGDL( *(new dimension(ndRes, qhull.facetCount())), BaseGDL::ZERO);
    
    long long ix=0;
    long long bad_facets=0;

    for (QhullFacetList::iterator it = facets.begin(); it != facets.end(); ++it)
    {
        if (!(*it).isGood())
        {
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

    //bounds
    if(isBounds)
    {
      vector<long> boundsV;
      for(int i=0; i<res->Dim(0)*res->Dim(1); ++i)
      {
          if( (find(boundsV.begin(), boundsV.end(), (*res)[i]) == boundsV.end()))
          {
            boundsV.push_back( (*res)[i] );
          }
      }
      DLongGDL* bounds=new DLongGDL( *(new dimension(boundsV.size())), BaseGDL::ZERO );
      for(int i=0; i <boundsV.size(); ++i){
          (*bounds)[i] = boundsV.at(i);
      }
      e->SetKW(boundsIx, bounds);
    }

    //connectivity
    if(isConn)
    {
      vector<vector<long>>connV(qhull.vertexCount());

      qhull.defineVertexNeighborFacets();
      QhullVertexList vertices = qhull.vertexList();

      long connCount=0;

      for(QhullVertexList::iterator vIt = vertices.begin(); vIt != vertices.end(); vIt++)
      {
        QhullVertex vertex = *vIt;

        long vertexId = vertex.point().id();

        QhullFacetSet neighbors = vertex.neighborFacets();
        vector<long> neighborsV;

        for(QhullFacetSet::iterator nIt = neighbors.begin(); nIt != neighbors.end(); ++nIt)
        {
          if(!(*nIt).isGood()) continue;
          QhullFacet neighbor = *nIt;
          QhullVertexSet neighborVSet = neighbor.vertices();
          for (QhullVertexSet::iterator nVIt = neighborVSet.begin(); nVIt != neighborVSet.end(); ++nVIt)
          {
              QhullVertex v = *nVIt;
              QhullPoint p = v.point();

              if( (find(neighborsV.begin(), neighborsV.end(), (long) p.id()) == neighborsV.end()) && p.id() != vertexId)
              {
                neighborsV.push_back(p.id());
                connCount++;
              }
          }
        }
        connV.at(vertexId) = neighborsV;
      }

      //creating the result connectivity array
      DLongGDL* conn = new DLongGDL(*(new dimension( np + 1 + connCount )), BaseGDL::ZERO);
      
      int writeIx = np+1;
      (*conn)[np]=np+1+connCount;
      for(int i=0; i < connV.size(); i++)
      {
          vector<long> neighborsV = connV.at(i);
          for(int j=0; j<neighborsV.size(); j++)
          {
              (*conn)[ writeIx+j ] = neighborsV.at(j);
          }
          (*conn)[i] = writeIx;
          writeIx += neighborsV.size();
      }
      e->SetKW(connIx, conn);
    }

    /*if(isVoronoi)
    {
      //prepare voronoi
      bool isLower;            //not used
      int voronoiVertexCount;  //not used
      qhull.prepareVoronoi(&isLower, &voronoiVertexCount);
    }*/


    //voronoi vertices
    if(isVvert)
    {
      vector<vector<double> > vVertices;

      for(QhullFacetList::iterator it = facets.begin(); it != facets.end(); ++it)
      {
        if (!(*it).isGood()) continue;
        QhullFacet f = *it;
        vVertices.push_back(f.getCenter().toStdVector());
      }

      DDoubleGDL* vvert=new DDoubleGDL( *(new dimension(nd, vVertices.size())), BaseGDL::ZERO);

      int nvVert=vVertices.size();
      for(int i=0; i < nvVert; ++i)
      {
        for(int j=0; j<nd; ++j)
        {
            (*vvert)[nd*i+j]=vVertices.at(i).at(j);
        }
      }
      e->SetKW(vvertIx, vvert);
    }

    //voronoi diagrams A FAIRE DEMAIN
    if(isVdiag)
    {
      e->Throw("vdiagrams option not available yet.");
    }

    //voronoi normals
    if(isVnorm)
    {
      e->Throw("vnorms option not available yet.");
    }

    e->SetPar(outIx, res);
  }

  BaseGDL* qgrid3_fun ( EnvT* e)
  {
    e->Throw("Please Write this function in GDL.");
    return NULL;
  }
}

