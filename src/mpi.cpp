/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//#include <config.h>
#include "includefirst.hpp"
#include "envt.hpp"
#include "mpi.hpp"

#ifdef USE_MPI

#include "mpi.h"

namespace lib {

  using namespace std;

  BaseGDL* mpi_comm_rank_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    if(nParam != 0) 
       throw GDLException( e->CallingNode(),
            "MPI_COMM_RANK: Incorrect number of arguments.");

    int rank;
    int err = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return new DIntGDL( rank);
  }

  BaseGDL* mpi_comm_size_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    if(nParam != 0)
       throw GDLException( e->CallingNode(),
            "MPI_COMM_SIZE: Incorrect number of arguments.");

    int size; 
    int err = MPI_Comm_size(MPI_COMM_WORLD, &size);
    return new DIntGDL( size);
  }

  void mpi_finalize_pro( EnvT* e) {
    SizeT nParam=e->NParam();
    if(nParam != 0)
         throw GDLException( e->CallingNode(),
          "MPI_COMM_SIZE: Incorrect number of arguments.");

    int err = MPI_Finalize();
    exit(0);
  }


  void mpi_send_pro( EnvT* e) 
  { 
    SizeT nParam=e->NParam();

    if(nParam != 1) 
      throw GDLException( e->CallingNode(), 
	    "MPI_SEND: Incorrect number of agruments.");

    DDoubleGDL* p0 = e->GetParAs<DDoubleGDL>( 0);
    int nElem = p0->N_Elements();
     
    int tag = 0;
    static int tagIx = e->KeywordIx("TAG");
    bool tagKW = e->KeywordSet( tagIx);
    
    if( tagKW){
      DLong tagLong = 0;     
      e->AssureLongScalarKW(tagIx, tagLong);
      tag = (int) tagLong;
    }


    int dest = 0;
    static int destIx = e->KeywordIx("DEST");
    bool destKW = e->KeywordSet( destIx);
    
    if( destKW){
      DLong destLong = 0;
      e->AssureLongScalarKW( destIx, destLong);
      dest = (int) destLong;
    }

    MPI_Datatype mpi_type = MPI_DOUBLE;
    
    //cout << "Before gdl send" << (*p0)[0] << "  nelem="<< nElem << endl;
    MPI_Send( (*p0).DataAddr(0), nElem, mpi_type, dest, tag, MPI_COMM_WORLD);
    //cout << "after gdl send" << endl;
        
  }
  
  BaseGDL* mpi_recv_fun( EnvT* e) 
  { 
    SizeT nParam=e->NParam();

    if(nParam != 0) 
      throw GDLException( e->CallingNode(), 
	    "MPI_RECV: Incorrect number of agruments.");

     
    int tag = 0;
    static int tagIx = e->KeywordIx("TAG");
    bool tagKW = e->KeywordSet( tagIx);
    
     if( tagKW){
      DLong tagLong = 0;     
      e->AssureLongScalarKW(tagIx, tagLong);
      tag = (int) tagLong;
    }

    int source = 0;
    static int sourceIx = e->KeywordIx("SOURCE");
    bool sourceKW = e->KeywordSet( sourceIx);
    
    if( sourceKW){
      DLong sourceLong = 0;
      e->AssureLongScalarKW( sourceIx, sourceLong);
      source = (int) sourceLong;
    }


    int count = 0;
    static int countIx = e->KeywordIx("COUNT");
    bool countKW = e->KeywordSet( countIx);
    
    if( countKW){
      DLong countLong = 0;
      e->AssureLongScalarKW( countIx, countLong);
      count = (int) countLong;
    }


    MPI_Datatype mpi_type = MPI_DOUBLE;

    dimension dim(count);
    DDoubleGDL* result = new DDoubleGDL(dim);

    MPI_Status status;

    //    MPI_Recv( &((*result)[0]), count, mpi_type, source, tag, MPI_COMM_WORLD, &status);
    MPI_Recv( (*result).DataAddr(0), count, mpi_type, source, tag, MPI_COMM_WORLD, &status);
    
    return result;
  }

  
  BaseGDL* mpi_allreduce_fun( EnvT* e) 
  { 
    SizeT nParam=e->NParam();

    if(nParam != 1) 
      throw GDLException( e->CallingNode(), 
	    "MPI_ALLREDUCE: Incorrect number of agruments.");
     
    // get the send buffer
    DDoubleGDL* p0 = e->GetParAs<DDoubleGDL>( 0);
    int nElem = p0->N_Elements();

    // determine the reduction operation
    int mpi_op = 0;

    static int opSumIx = e->KeywordIx("SUM");
    if( e->KeywordSet( opSumIx)) mpi_op = MPI_SUM;
 
    static int opProdIx = e->KeywordIx("PRODUCT");
    if( e->KeywordSet( opProdIx)) {
      if( mpi_op != 0) 
	throw GDLException( e->CallingNode(), 
			    "MPI_ALLREDUCE: Conflicing operators.");
      else 
	mpi_op = MPI_PROD;
    }


    int count = 0;
    static int countIx = e->KeywordIx("COUNT");
    bool countKW = e->KeywordSet( countIx);
    
    if( countKW){
      DLong countLong = 0;
      e->AssureLongScalarKW( countIx, countLong);
      count = (int) countLong;
    } else count = nElem;


    MPI_Datatype mpi_type = MPI_DOUBLE;

    dimension dim(count);
    DDoubleGDL* result = new DDoubleGDL(dim);

    MPI_Status status;

    //    MPI_Recv( &((*result)[0]), count, mpi_type, source, tag, MPI_COMM_WORLD, &status);
    MPI_Allreduce( (*p0).DataAddr(0), (*result).DataAddr(0), count, 
                  mpi_type, mpi_op, MPI_COMM_WORLD);
    
    return result;
  }

}
#endif
