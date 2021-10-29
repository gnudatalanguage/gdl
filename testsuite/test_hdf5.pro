;
; Alain C., 16 Fev. 2016
; Very preliminary tests for HDF5 formats.
; Extensions (data (small volume) + code) very welcome
; (never forget than adding now test cases will avoid future silently
; broken functionnalities !!)
;
; Some users are already using the HDF5 functionnalities
; but it was reported that the STRING field is not well managed.
;
; -----------------------------------------------
;
pro TEST_HDF5_BYTE_ARRAY, cumul_errors, test=test
;
errors=0
;
file='byte_array.h5'
fullfile=FILE_SEARCH_FOR_TESTSUITE(file, /warning)
if (STRLEN(fullfile) EQ 0) then begin
    cumul_errors++
    return
endif
;
file_id = H5F_OPEN(fullfile)
data_id = H5D_OPEN(file_id, 'g1/d1')
;
vector=0
ok=EXECUTE('vector = H5D_READ(data_id)')
;
expected=BYTE([127,1, 23])
;
; we have to test the values but also the TYPE
if ~ARRAY_EQUAL(vector, expected) then begin
    MESSAGE, /continue, 'Bad values ...'
    errors++
endif
if (TYPENAME(vector) NE 'BYTE') then begin
    MESSAGE, /continue, 'Bad TYPE ...'
    errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_HDF5_BYTE_ARRAY', errors, /short
;
if KEYWORD_SET(test) then STOP
;
if ~ISA(cumul_errors) then cumul_errors=0
cumul_errors=cumul_errors+errors
;
end
;
; -----------------------------------------------
;
pro TEST_HDF5_STRING, cumul_errors, test=test
;
; Test provided by R. Parker (Univ. Leicester)
;
errors=0
;
file='string.h5'
fullfile=FILE_SEARCH_FOR_TESTSUITE(file, /warning)
if (STRLEN(fullfile) EQ 0) then begin
    cumul_errors++
    return
endif
;
file_id = H5F_OPEN(fullfile)
data_id = H5D_OPEN(file_id, 'mystring')
;
mystring=''
ok=EXECUTE('mystring=H5D_READ(data_id)')
;
expected='Peter is great.'
;
if ~ARRAY_EQUAL(mystring, expected) then errors=1
;
BANNER_FOR_TESTSUITE, 'TEST_HDF5_STRING', errors, /short
;
if KEYWORD_SET(test) then STOP
;
if ~ISA(cumul_errors) then cumul_errors=0
cumul_errors=cumul_errors+errors
;
end
;
; -----------------------------------------------
;
pro TEST_HDF5_OBJ_INFO, cumul_errors, create=create

   errors=0

   file_name = "hdf5-obj-info-test.h5"
   full_file_name = file_search_for_testsuite(file_name, /warning)
   if (STRLEN(full_file_name) eq 0) then begin
      cumul_errors++
      return
   endif

   ; --- create a mock dataset (not implemented in GDL yet)

   if keyword_set(create) then begin

      a_number=123

      file_id = h5f_create(file_name)

      group_id = h5g_create(file_id, "a_group")

      type_id = h5t_idl_create(a_number)
      space_id = h5s_create_scalar()
      dset_id = h5d_create(group_id, "a_dataset", type_id, space_id)

      h5d_write, dset_id, a_number
      h5d_close, dset_id &  h5s_close, space_id &  h5t_close, type_id

      h5g_close, group_id

      h5g_link, file_id, "a_group", "a_first_hard_link"
      h5g_link, file_id, "a_group", "a_second_hard_link"

      h5g_link, file_id, "a_group", "a_soft_link", /soft

      h5f_close, file_id

      return
   endif

   ; --- check H5G_GET_OBJINFO functionality

   file_id = h5f_open(full_file_name)

   stat = h5g_get_objinfo(file_id, "/a_group")
   if (stat.nlink ne 3) then errors++
   if (stat.type ne 'GROUP') then errors++

   stat = h5g_get_objinfo(file_id, "/a_first_hard_link")
   if (stat.nlink ne 3) then errors++
   if (stat.type ne 'GROUP') then errors++

   stat = h5g_get_objinfo(file_id, "/a_group/a_dataset")
   ;;; cmd='date -r '+full_file_name+' "+%s"' & spawn, cmd, mtime
   ;;; if (stat.mtime ne mtime) then errors++
   if (stat.nlink ne 1) then errors++
   if (stat.type ne 'DATASET') then errors++


   stat = h5g_get_objinfo(file_id, "/a_soft_link")
   if (stat.nlink ne 0) then errors++
   if (stat.type ne 'LINK') then errors++
   if (stat.linklen ne 8) then errors++

   stat = h5g_get_objinfo(file_id, "/a_soft_link", /follow_link)
   if (stat.nlink ne 3) then errors++
   if (stat.type ne 'GROUP') then errors++

   ; --- print banner

   banner_for_testsuite, 'TEST_HDF5_OBJ_INFO', errors, /short

   if ~isa(cumul_errors) then cumul_errors=0
   cumul_errors=cumul_errors+errors

end
;
; -----------------------------------------------
;
pro TEST_HDF5_ATTR, cumul_errors, create=create

   errors=0

   file_name = "hdf5-attr-test.h5"
   full_file_name = file_search_for_testsuite(file_name, /warning)
   if (strlen(full_file_name) eq 0) then begin
      cumul_errors++
      return
   endif

   ; --- specify some dimensions

   some_elem_dims = list( [], [3], [2,3] )
   some_data_dims = list( [], [4], [4,3], [4,3,2] )

   ; --- create mock attribute data

   attr_data = list(123)        ; first entry is a scalar
   elem_rank = list(0)
   data_rank = list(0)

   for el_rank=0,2 do begin
      el_dims = some_elem_dims[el_rank]

      for dt_rank=0,3 do begin
         dt_dims = some_data_dims[dt_rank]

         rank = el_rank + dt_rank

         if(el_rank eq 0 and dt_rank eq 0) then continue

         dims = []
         if (el_rank gt 0) then dims = [ dims, el_dims ]
         if (dt_rank gt 0) then dims = [ dims, dt_dims ]

         ndata=1 & for i=0,n_elements(dims)-1 do ndata *= dims[i]

         case (rank mod 3) of
            0: data = intarr( ndata )
            1: data = fltarr( ndata )
            2: data = dblarr( ndata )
         endcase

         for i=0,ndata-1 do data[i] = i

         ; --- create lists
         attr_data.add, reform( data, dims )
         elem_rank.add, el_rank
         data_rank.add, dt_rank

      endfor

   endfor

   if keyword_set(create) then begin

      ; --- write mock attributes to HDF5 file (not yet implemented in GDL)

      f_id = h5f_create(full_file_name)

      for i=0,n_elements(attr_data)-1 do begin

         elem_t_id = h5t_idl_create(attr_data[i])

         if elem_rank[i] gt 0 then begin ; non-scalar element datatype

            t_id = h5t_array_create(elem_t_id, some_elem_dims[elem_rank[i]])
            h5t_close, elem_t_id

         endif else t_id = elem_t_id

         if data_rank[i] gt 0 then $ ; non-scalar dataspace
            s_id = h5s_create_simple(some_data_dims[data_rank[i]]) $
         else s_id = h5s_create_scalar()

         a_id = h5a_create( f_id, string(i,fo='(%"attr-%02d")'), t_id, s_id )

         h5a_write, a_id, attr_data[i]

         h5a_close, a_id
         h5s_close, s_id
         h5t_close, t_id

      endfor

      h5f_close, f_id

   endif

   ; --- read HDF5 attributes

   f_id = h5f_open(full_file_name)

   for idx=0,n_elements(attr_data)-1 do begin

      a_id = h5a_open_idx(f_id,idx)
      read_attr_data = h5a_read(a_id)
      h5a_close, a_id

      ;;; help, attr_data[idx], read_attr_data

      if ( not array_equal(size(attr_data[idx]),size(read_attr_data)) ) then $
         errors++

      if ( not array_equal(attr_data[idx],read_attr_data,/no_typeconv) ) then $
         errors++

   endfor

   h5f_close, f_id

   banner_for_testsuite, 'TEST_HDF5_ATTR', errors, /short

   if ~isa(cumul_errors) then cumul_errors=0
   cumul_errors = cumul_errors + errors

   return
end
;
; -----------------------------------------------
;
pro TEST_HDF5_DATA, cumul_errors, create=create

   errors=0

   file_name = "hdf5-data-test.h5"
   full_file_name = file_search_for_testsuite(file_name, /warning)
   if (strlen(full_file_name) eq 0) then begin
      cumul_errors++
      return
   endif

   ; --- specify some dimensions

   some_elem_dims = list( [], [3], [2,3] )
   some_data_dims = list( [], [4], [5,4], [6,5,4] )

   ; --- create mock data

   mock_data = list(123)        ; first entry is a scalar
   elem_rank = list(0)
   data_rank = list(0)

   for el_rank=0,2 do begin
      el_dims = some_elem_dims[el_rank]

      for dt_rank=0,3 do begin
         dt_dims = some_data_dims[dt_rank]

         rank = el_rank + dt_rank

         if(el_rank eq 0 and dt_rank eq 0) then continue

         dims = []
         if (el_rank gt 0) then dims = [ dims, el_dims ]
         if (dt_rank gt 0) then dims = [ dims, dt_dims ]

         ndata=1 & for i=0,n_elements(dims)-1 do ndata *= dims[i]

         case (rank mod 3) of
            0: data = intarr( ndata )
            1: data = fltarr( ndata )
            2: data = dblarr( ndata )
         endcase

         for i=0,ndata-1 do data[i] = i+1

         ; --- create lists
         mock_data.add, reform( data, dims )
         elem_rank.add, el_rank
         data_rank.add, dt_rank

      endfor

   endfor

   if keyword_set(create) then begin

      ; --- write mock datasets to HDF5 file (not yet implemented in GDL)

      f_id = h5f_create(full_file_name)

      for i=0,n_elements(mock_data)-1 do begin

         elem_t_id = h5t_idl_create(mock_data[i])

         if elem_rank[i] gt 0 then begin ; non-scalar element datatype

            t_id = h5t_array_create(elem_t_id, some_elem_dims[elem_rank[i]])
            h5t_close, elem_t_id

         endif else t_id = elem_t_id

         if data_rank[i] gt 0 then $ ; non-scalar dataspace
            s_id = h5s_create_simple(some_data_dims[data_rank[i]]) $
         else s_id = h5s_create_scalar()

         d_id = h5d_create( f_id, string(i,fo='(%"dset-%02d")'), t_id, s_id )

         h5d_write, d_id, mock_data[i]

         h5d_close, d_id
         h5s_close, s_id
         h5t_close, t_id

      endfor

      h5f_close, f_id

   endif

   ; --- read HDF5 datasets

   f_id = h5f_open(full_file_name)

   for idx=0,n_elements(mock_data)-1 do begin

      d_id = h5d_open(f_id,string(idx, fo='(%"dset-%02d")'))

      read_mock_data = h5d_read(d_id)

      h5d_close, d_id

      ;;; help, mock_data[idx], read_mock_data

      if ( not array_equal(size(mock_data[idx]),size(read_mock_data)) ) then $
         errors++

      if ( not array_equal(mock_data[idx],read_mock_data,/no_typeconv) ) then $
         errors++

   endfor

   ; --- read full HDF5 datasets once more, using explicit data spaces

   for idx=0,n_elements(mock_data)-1 do begin

      d_id = h5d_open(f_id,string(idx, fo='(%"dset-%02d")'))

      fs_id = h5d_get_space(d_id)

      rank = h5s_get_simple_extent_ndims(fs_id)
      dims = h5s_get_simple_extent_dims(fs_id)

      if(rank eq 0) then ms_id = h5s_create_scalar() $
      else ms_id = h5s_create_simple(dims)

      read_mock_data = h5d_read(d_id, file_space=fs_id, memory_space=ms_id)

      h5d_close, d_id
      h5s_close, fs_id
      h5s_close, ms_id

      ;;; help, mock_data[idx], read_mock_data

      if ( not array_equal(size(mock_data[idx]),size(read_mock_data)) ) then $
         errors++

      if ( not array_equal(mock_data[idx],read_mock_data,/no_typeconv) ) then $
         errors++

   endfor

   ; --- read subsets from HDF5 datasets, using hyperslab selection

   for idx=0,n_elements(mock_data)-1 do begin

      if data_rank[idx] eq 0 then continue ; skip scalar datasets

      d_id = h5d_open(f_id,string(idx, fo='(%"dset-%02d")'))

      fs_id = h5d_get_space(d_id)

      rank = h5s_get_simple_extent_ndims(fs_id)
      dims = h5s_get_simple_extent_dims(fs_id)

      if(rank gt 0) then begin
         ; FIXME: come up with less trivial hyperslabs (+ use block/stride)

         for i=0,rank-1 do dims[i]-=2
         h5s_select_hyperslab, fs_id, replicate(1,rank), dims, /reset
         ms_id = h5s_create_simple(dims)

      endif

      read_mock_data = h5d_read(d_id, file_space=fs_id, memory_space=ms_id)

      h5d_close, d_id
      h5s_close, fs_id
      h5s_close, ms_id

      case elem_rank[idx] of
         0: begin
            case data_rank[idx] of
               1: slab = (mock_data[idx])[1:-2]
               2: slab = (mock_data[idx])[1:-2,1:-2]
               3: slab = (mock_data[idx])[1:-2,1:-2,1:-2]
            endcase
         end

         1: begin
            case data_rank[idx] of
               1: slab = (mock_data[idx])[*,1:-2]
               2: slab = (mock_data[idx])[*,1:-2,1:-2]
               3: slab = (mock_data[idx])[*,1:-2,1:-2,1:-2]
            endcase
         end

         2: begin
            case data_rank[idx] of
               1: slab = (mock_data[idx])[*,*,1:-2]
               2: slab = (mock_data[idx])[*,*,1:-2,1:-2]
               3: slab = (mock_data[idx])[*,*,1:-2,1:-2,1:-2]
            endcase
         end
      endcase

      ;;; help, slab, read_mock_data

      if ( not array_equal(size(slab),size(read_mock_data)) ) then $
         errors++

      if ( not array_equal(slab,read_mock_data,/no_typeconv) ) then $
         errors++

   endfor

   h5f_close, f_id

   banner_for_testsuite, 'TEST_HDF5_DATA', errors, /short

   if ~isa(cumul_errors) then cumul_errors=0
   cumul_errors = cumul_errors + errors

   return
end
;
; -----------------------------------------------
;
pro TEST_HDF5, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_HDF5, help=help, test=test, no_exit=no_exit'
   return
endif
;
; Do we have access to HDF5 functionnalities ??
;
DEFSYSV, '!gdl', exists=is_it_gdl
;
if (is_it_gdl EQ 1) then begin
    ok=EXECUTE('resu=HDF5_EXISTS()')
    if (resu EQ 0) then begin
        MESSAGE, /continue, "GDL was compiled without HDF5 support."
        MESSAGE, /con, "You must have HDF5 support to use this functionaly."
        EXIT, status=77
    endif
endif
;
cumul_errors=0
;
TEST_HDF5_BYTE_ARRAY, cumul_errors
;
TEST_HDF5_STRING, cumul_errors
;
TEST_HDF5_ATTR, cumul_errors
;
TEST_HDF5_DATA, cumul_errors
;
TEST_HDF5_OBJ_INFO, cumul_errors
;
BANNER_FOR_TESTSUITE, 'TEST_HDF5', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

