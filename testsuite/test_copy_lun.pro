pro TEST_COPY_LUN
catch, err
if err ne 0 then begin
   catch, /cancel
   print, !error_state.msg
   exit, status=1
endif

; Cross the internal copy buffer boundary and finish with a partial buffer.
data = bindgen(1000003L)
openw, src, 'test_copy_lun_src.bin', /get_lun, /delete
openw, dst, 'test_copy_lun_dst.bin', /get_lun, /delete
writeu, src, data
point_lun, src, 0
copy_lun, src, dst, 0, transfer_count=n
if n ne 0 then message, 'Zero-length copy transferred data'
copy_lun, src, dst, /eof, transfer_count=n
if n ne n_elements(data) then message, 'Incorrect transfer count at EOF'
point_lun, dst, 0
actual = bytarr(n)
readu, dst, actual
if ~array_equal(actual, data) then message, 'Copy changed the data'

point_lun, dst, 3
truncate_lun, dst
if (fstat(dst)).size ne 3 then message, 'Truncation did not shorten the file'
point_lun, dst, 0
actual = bytarr(3)
readu, dst, actual
if ~array_equal(actual, data[0:2]) then message, 'Truncation changed the retained prefix'
point_lun, dst, 0
truncate_lun, dst
if (fstat(dst)).size ne 0 then message, 'Truncation at zero left data in the file'
free_lun, src, dst
end
