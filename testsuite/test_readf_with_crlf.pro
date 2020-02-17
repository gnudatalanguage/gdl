pro test_readf_with_crlf,noexit=noexit
  err=0
  line1=' 12 833.567 '
  line2=' 26 -45.222 '         
  byte1=[byte(line1),13b]
  line1r=fix(byte1,type=7)
  byte2=[byte(line2),13b]   
  line2r=fix(byte2,type=7)
  openw,lun,'fileendingwithcrlf',/get_lun
  printf,lun,line1r
  printf,lun,line2r
  close,lun
  free_lun,lun
  z1=fltarr(4)
  openr,lun,'fileendingwithcrlf',/get_lun
  readf,lun,z1
  x=fltarr(2) & y=fltarr(2) 
  reads,line1,x
  reads,line2,y
  z2=[x,y]
  close,lun
  free_lun,lun
  file_delete,'fileendingwithcrlf'
  if total(z1 eq z2) ne 4 then err++
; bonus
  if file_test('fileendingwithcrlf') then err++
  
  banner_for_testsuite,' TEST_READF_WITH_CRLF',err
  if (err gt 0) and ~keyword_set(noexit) then exit, status = 1 
end
