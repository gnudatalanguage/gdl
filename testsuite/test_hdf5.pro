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
pro TEST_HDF5_ATTR, cumul_errors, create=create

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

      f_id = h5f_create("hdf5-attr-test.h5")

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

   f_id = h5f_open("hdf5-attr-test.h5")

   for idx=0,n_elements(attr_data)-1 do begin

      a_id = h5a_open_idx(f_id,idx)
      read_attr_data = h5a_read(a_id)
      h5a_close, a_id

      if ( total( attr_data[idx] - read_attr_data ) gt 0. ) then $
         cumul_errors++

   endfor

   h5f_close, f_id

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
BANNER_FOR_TESTSUITE, 'TEST_HDF5', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

