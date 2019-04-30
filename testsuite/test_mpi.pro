pro test_mpi
  has_mpi = ~ (execute("rank=mpidl_comm_rank()") eq 0)
  if ~has_mpi then exit, status=77

  rank = mpidl_comm_rank()
  size = mpidl_comm_size()
  print, rank, size
  if rank ne 0 then exit, status=1
  if size ne 1 then exit, status=1
end
