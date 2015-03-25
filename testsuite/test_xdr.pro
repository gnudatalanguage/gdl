pro test_write_xdr
 common testxdr,datar,dataw
  OPENW, /XDR, 3, 'gdl.xdr'
  writeU, 3, dataw
  close,3
end

pro test_write_compressed_xdr
 common testxdr,datar,dataw
  OPENW, /XDR, 3, 'gdl.xdr.gz', /compress
  writeU, 3, dataw
  close,3
end

pro test_read_xdr, file, compress=compress
 common testxdr,datar,dataw
  OPENR, /XDR, 1, file, compress=compress
  READU, 1, datar
  close,1
  for i=0,n_tags(dataw)-1 do begin
     if ( total(dataw.(i) eq datar.(i)) ne n_elements(dataw.(i)) ) then begin
        message, 'FAILED at tag #'+string(i)+" in file "+file, /conti
        exit, status=1
     end
  end
end


pro test_xdr
 common testxdr,datar,dataw
  DATAR={BytesAtStart:bytarr(13),ShortArray:intarr(3),UnitArray:uintarr(3),longarray:lonarr(3),ulongarray:ulonarr(3),long64array:lon64arr(3),stringarray:strarr(2),ulong64array:ulon64arr(3),floatarray:fltarr(3),doublearray:dblarr(3),complexarray:complexarr(2),doublecomplexarray:dcomplexarr(2),finalbytesum:0b}
  DATAW={ BytesAtStart:byte(indgen(13)),ShortArray:fix([-1,-2,-3]),UnitArray:uint([1,2,3]),longarray:long([-1,-2,-3]),ulongarray:ulong([1,2,3]),long64array:long64([-1,-2,-3]),stringarray:['String1','a Longer String:2'],ulong64array:ulong64([1,2,3]),floatarray:float([1,2,3]),doublearray:double([1,2,3]),complexarray:[complex(1,2),complex(3,4)],doublecomplexarray:[dcomplex(12.,13.),dcomplex(14,15.)],finalbytesum:0b}
  dataw.finalbytesum=n_tags(dataw)
; test read of idl xdr:
test_read_xdr, 'idl.xdr'
; write our own
test_write_xdr
; reread it and compare
test_read_xdr, 'gdl.xdr'
; write our own compressed
test_write_compressed_xdr
; reread it and compare
test_read_xdr, 'gdl.xdr.gz', /compress

end
