
//#include "datatypes.hpp"
//#include "envt.hpp"

#include <config.h>

namespace lib {
  
  void wait_pro( EnvT* e);
  BaseGDL* clock_fun( EnvT* e);

  void mpi_send_pro( EnvT* e);
  BaseGDL* mpi_recv_fun( EnvT* e);
  BaseGDL* mpi_allreduce_fun( EnvT* e);
  BaseGDL* mpi_comm_rank_fun( EnvT* e);
  BaseGDL* mpi_comm_size_fun( EnvT* e);
  void mpi_finalize_pro( EnvT* e);
} // namespace


