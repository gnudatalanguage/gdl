pro nct_open_close, filename,write=write, nowrite=nowrite
catch, error
if(error eq 0) then begin
    
    id=ncdf_open(filename,write=write, nowrite=nowrite)
    ncdf_close, id
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse


end

pro nct_inquire,f
    id=ncdf_open(f,/nowrite)
catch, error
if(error eq 0) then begin
    
    inq=ncdf_inquire(id)
    print, "ndims", inq.ndims
    print, "nvars", inq.nvars
    print, "ngatss", inq.ngatts
    print, "recdim", inq.recdim

    
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

ncdf_close, id

end


pro nct_att_readonly, f
id=ncdf_open(f,/nowrite)

catch, error
if(error eq 0) then begin
    

    inq=ncdf_inquire(id)
    
    for i=0, inq.ngatts-1 do begin
        
        n=ncdf_attname(id,/global,i)
        a=ncdf_attinq(id,/global,n)
        print, "Attribute: ", i, "       Name: ", n
        print, "Datatype: ", a.datatype
        print, "Length: ", a.length
        print, "Getting Attribute"
        ncdf_attget,id,/global,n,val
        print, "Got Attribute"
        help, val
        
    endfor
    
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

ncdf_close, id
end

pro nct_dim_readonly, f
id=ncdf_open(f,/nowrite)
catch, error
if(error eq 0) then begin

    inq=ncdf_inquire(id)
    
    for i=0, inq.ndims-1 do begin
        ncdf_diminq, id, i, n,s
        print, "Dimension ",i
        print, "Name: ", n
        print, "Size: ", s
        print, "Checking dimid"
        dimid=ncdf_dimid(id, n)
        print, "Dimid reports: ", dimid, ", should be: ", i
    endfor
    

    
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse    
ncdf_close, id

end

pro nct_var_readonly, f
    id=ncdf_open(f, /nowrite)

catch, error
if(error eq 0) then begin
    

    inq=ncdf_inquire(id)
    dims=intarr(inq.ndims)
    for i=0, inq.ndims-1 do begin
        ncdf_diminq, id, i, n,s
        dims[i]=s
    endfor

    
    for i=0, inq.nvars-1 do begin
        v=ncdf_varinq(id, i)
        print, "Variable ", i
        print, "Name: ", v.name
        print, "datatype: ", v.datatype
        print, "NDims: ", v.ndims
        print, "Dim: ", v.dim
        print, "Getting variable (all)"
        ncdf_varget, id, i, val
        help, val
    endfor


endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

ncdf_close, id
end


pro nct_varget_count, f
id=ncdf_open(f, /nowrite)

catch, error
if(error eq 0) then begin
    
    inq=ncdf_inquire(id)
    dims=intarr(inq.ndims)
    for i=0, inq.ndims-1 do begin
        ncdf_diminq, id, i, n,s
        dims[i]=s
    endfor

    
    for i=0, inq.nvars-1 do begin
        v=ncdf_varinq(id, i)
        count=intarr(v.ndims)
        print, v.ndims
        for j=0, v.ndims-1 do count[j]=fix(dims[j]/2)
        ncdf_varget, id, i, val,count=count
        print, i
    endfor

endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

ncdf_close, id
end

pro nct_varget_offset, f
id=ncdf_open(f, /nowrite)

catch, error
if(error eq 0) then begin
    
    inq=ncdf_inquire(id)
    dims=intarr(inq.ndims)
    for i=0, inq.ndims-1 do begin
        ncdf_diminq, id, i, n,s
        dims[i]=s
    endfor

    
    for i=0, inq.nvars-1 do begin
        v=ncdf_varinq(id, i)
        offset=intarr(v.ndims)
        for j=0, v.ndims-1 do offset[j]=1+fix(dims[j]/2)
        ncdf_varget, id, i, val,offset=offset
        help, val
    endfor

endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

ncdf_close, id
end

pro nct_varget_stride, f
id=ncdf_open(f, /nowrite)

catch, error
if(error eq 0) then begin
    
    inq=ncdf_inquire(id)
    dims=intarr(inq.ndims)
    for i=0, inq.ndims-1 do begin
        ncdf_diminq, id, i, n,s
        dims[i]=s
    endfor

    
    for i=0, inq.nvars-1 do begin
        v=ncdf_varinq(id, i)
        stride=intarr(v.ndims)
        for j=0, v.ndims-1 do stride[j]=fix(dims[j]/2)
        ncdf_varget, id, i, val,stride=stride
        help, val
    endfor

endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

ncdf_close, id
end

pro nct_varget_all, f,varid,count=count,stride=stride,offset=offset
id=ncdf_open(f, /nowrite)

catch, error
if(error eq 0) then begin
    ncdf_varget, id, varid, val,count=count, offset=offset,stride=stride 
;    help, val
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

ncdf_close, id
end



pro nct_varget1_0,f
id=ncdf_open(f, /nowrite)

catch, error
if(error eq 0) then begin

    inq=ncdf_inquire(id)
    dims=intarr(inq.ndims)
    for i=0, inq.ndims-1 do begin
        ncdf_diminq, id, i, n,s
        dims[i]=s
    endfor

    
    for i=0, inq.nvars-1 do begin
        ncdf_varget1, id, i, val
    endfor
    
    
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

ncdf_close, id
end

pro nct_varget1_midpoint,f
    id=ncdf_open(f, /nowrite)
catch, error
if(error eq 0) then begin

    inq=ncdf_inquire(id)    
    dims=intarr(inq.ndims)
    for i=0, inq.ndims-1 do begin
        ncdf_diminq, id, i, n,s
        dims[i]=s
    endfor

    

    for i=0, inq.nvars-1 do begin
        v=ncdf_varinq(id, i)
        offset=intarr(v.ndims)
        for j=0, v.ndims-1 do offset[j]=fix(dims[j]/2)
        ncdf_varget1, id, i, val, offset=offset
    endfor
    
    
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse
ncdf_close, id
end


pro nct_varget1_neg,f
id=ncdf_open(f, /nowrite)
catch, error
if(error eq 0) then begin
    

    inq=ncdf_inquire(id)
    dims=intarr(inq.ndims)
    for i=0, inq.ndims-1 do begin
        ncdf_diminq, id, i, n,s
        dims[i]=s
    endfor

    for i=0, inq.nvars-1 do begin
        v=ncdf_varinq(id, i)
        offset=intarr(v.ndims)
        for j=0, v.ndims-1 do offset[j]=-1
        ncdf_varget1, id, i, val,offset=offset
    endfor
    
    
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

ncdf_close, id
end

pro nct_varget1_tm,f
id=ncdf_open(f, /nowrite)
catch, error
if(error eq 0) then begin

    inq=ncdf_inquire(id)
    dims=intarr(inq.ndims)
    for i=0, inq.ndims-1 do begin
        ncdf_diminq, id, i, n,s
        dims[i]=s
    endfor


    for i=0, inq.nvars-1 do begin
        v=ncdf_varinq(id, i)
        offset=intarr(v.ndims)
        for j=0, v.ndims-1 do offset[j]=dims[j]+1
        ncdf_varget1, id, i, val,offset=offset
    endfor
    
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

ncdf_close,id
end


pro nct_create_valid, f

catch, error
if(error eq 0) then begin
    id=ncdf_create(f,/clobber)
    ncdf_attput, id, /global, "title", "my title"
    ncdf_attput, id,/global, "date", " 2004 04 16
    d1=ncdf_dimdef(id, "dim1",10)
    d2=ncdf_dimdef(id, "dim2",20)
    print, "here2"
    v1=ncdf_vardef(id, "dim1",d1,/byte)
    v2=ncdf_vardef(id, "dim2",d2,/float)
    v3=ncdf_vardef(id, "var1",d2,/char)
    print, "here3"
    v4=ncdf_vardef(id, "var2",d1,/double)
    v5=ncdf_vardef(id, "var3",[d1,d1],/long)
    v6=ncdf_vardef(id, "var4",[d1,d2],/short)
    print, "here4"
    ncdf_control,id, /endef
    ncdf_varput, id, v1, bindgen(10)
    ncdf_varput, id, v2, findgen(20)+1.0
    ncdf_varput, id, v3, "abcdeabcdeabcdeabcde"
    ncdf_varput, id, v4, dindgen(10)+1.0
    ncdf_varput, id, v5, lindgen(10,10)
    ncdf_varput, id, v6, indgen(10,20)
    
    ncdf_close, id
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

end

pro nct_create_noclobber, f

catch, error
if(error eq 0) then begin
    id=ncdf_create( f, /noclobber)
    ncdf_close, id
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

end


pro nct_create_complex, f1,f2

catch, error
if(error eq 0) then begin
    id=ncdf_create( f1, /clobber)
    id2=ncdf_create( f2, /clobber)
    
    ncdf_attput, id, /global, "test", "test"
    ncdf_attput, id, /global, "test2", "test2", length=3
    ncdf_attput, id, /global, "test3", 3L, /short
    ncdf_attrename, id, "test3", "short_test3",/global
    
    r=ncdf_attcopy(id, "test",id2,/in_global,/out_global)
    r2=ncdf_attcopy(id, "test2",id2,/in_global,/out_global)
    
    ncdf_control, id, /endef
    
    name=ncdf_attname(id,/global,2)
    ncdf_attget, id, /global,name,date
    r=ncdf_attinq(id,/global,name)
    
    name=ncdf_attname(id,/global,1)
    ncdf_attget, id, /global,name,date
    r=ncdf_attinq(id,/global,name)
    print, "Name: ", name
    print, "Value: ", date
    print, "Datatype: ", r.datatype
    print, "Length: ", r.length
    print, "As String: ",string(date)
    ncdf_close, id
    ncdf_close, id2
    
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

end

pro nct_create_complex2, f1

catch, error
if(error eq 0) then begin
    id=ncdf_create( f1, /clobber)
    d1=ncdf_dimdef(id, "dim1", 10)
    v1=ncdf_vardef(id, "dim1", d1,/float)
    v2=ncdf_vardef(id, "var1", d1,/float)
    
    
    ncdf_control, id, /endef
    ncdf_varput, id, v1, findgen(10)
    ncdf_varput, id, v2, findgen(10)*2.0
    
    
    ncdf_close, id
    
    id=ncdf_open(f1,/nowrite)
    inq=ncdf_inquire(id)

    n=strarr(inq.nvars)
    for i=0, inq.nvars-1 do begin
        v=ncdf_varinq(id,i)
        n[i]=v.name
    endfor
    
    m=strarr(inq.ndims)
    for i=0, inq.ndims-1 do begin
        ncdf_diminq,id, i,name,s
        m[i]=name
    endfor
    print, "var: ",n
    print, "dim: ",m
    
    ncdf_varget,id, 0,q
    print, n[0],": ",q
    ncdf_close,id
    
print, "Adding dim, var, rename dim, var"
    
    id=ncdf_open(f1, /write)
    ncdf_control,id,/redef
    ncdf_varrename, id, 0,"dim_new1"
    ncdf_dimrename,id, 0, "dim_new1"

    d2=ncdf_dimdef(id, "dim2", 20)
    v3=ncdf_vardef(id, "dim2", d2,/float)
    v4=ncdf_vardef(id, "var2", [d1,d2],/float)

    ncdf_control, id,/endef
    ncdf_varput, id, v3, findgen(20)
    ncdf_varput, id, v4, findgen(10,20)
    
    ncdf_varget, id, 0, q
    print, "start" 
    print, "val: ",q
    print, "offset+count"
    ncdf_varput, id, 0, [5,4,3,2,1]+0.5, offset=[2], count=[5]
    q=0
    ncdf_varget, id, 0, q
    print, "val: ",q
    
    
    print, "offset+count+stride"
    ncdf_varput, id, 0, [5,4,3], offset=[2], count=[3],stride=[2]
    q=0
    ncdf_varget, id, 0, q
    print, "val: ",q
    
    
    print, "no keywords"
    ncdf_varput, id, 0, findgen(10)
    q=0
    ncdf_varget, id, 0, q
    print, "val: ",q
    
    print, "stride+count"
    ncdf_varput, id, 0, findgen(5)^2, count=[5],stride=[2]
    q=0
    ncdf_varget, id, 0, q
    print, "val: ",q
    
    print, "offset(1)"
    ncdf_varput, id, 0, 100.,offset=5
    q=0
    ncdf_varget, id, 0, q
    print, "val: ",q
    
    
    print, "offset(2)"
    ncdf_varput, id, 0, findgen(4)*100, offset=3
    q=0
    ncdf_varget, id, 0, q
    print, "val: ",q
    
    print, "offset+stride"
    ncdf_varput, id, 0, findgen(2)*500, offset=3,stride=3
    q=0
    ncdf_varget, id, 0, q
    print, "val: ",q

    ncdf_close, id
    
    
    
    
    id=ncdf_open(f1,/nowrite)
    inq=ncdf_inquire(id)
    
    
    n=strarr(inq.nvars)
    for i=0, inq.nvars-1 do begin
        v=ncdf_varinq(id,i)
        n[i]=v.name
    endfor
        m=strarr(inq.ndims)
    for i=0, inq.ndims-1 do begin
        ncdf_diminq,id, i,name,s
        m[i]=name
    endfor
    print, "var: ",n
    print, "dim: ",m
    
    q=0
    ncdf_varget,id, 0,q
    print, n[0],": ",q
    ncdf_close,id
        

    
endif else begin
    print, "ERROR: ", error
    print, "Message: ", !ERROR_STATE.MSG
    error=0
endelse

end


pro nc_test

test={ $
       nct_open_close_dne: 0,  $
       nct_open_close: 0,  $
       nct_inquire: 0, $
       nct_att_readonly: 0, $
       nct_dim_readonly: 0, $       
       nct_var_readonly: 0, $
       nct_varget_count: 0, $
       nct_varget_offset: 0, $
       nct_varget_stride: 0, $
       nct_varget_all: 0, $
       nct_varget1_0: 0, $
       nct_varget1_neg: 0, $
       nct_varget1_midpoint: 0, $
       nct_varget1_tm: 0, $
       nct_create_valid:1, $
       nct_create_noclobber:1, $
       nct_create_complex:1 , $
       nct_create_complex2:1 $
      }
      
f="gdl_test.nc"
f2="gdl_mktest1.nc"
f3="gdl_mktest2.nc"
f4="gdl_mktest3.nc"

;file_does not exist

if(test.nct_open_close_dne) then begin
    print, "nct_open_close dne"
    nct_open_close,"doesnotexist.nc"
endif


if(test.nct_open_close gt 0) then begin
    print, "nct_open_close"
    nct_open_close,f
endif

if( test.nct_open_close gt 1) then  begin
    print, "nct_open_close,/write"
    nct_open_close,"u.nc",/write
endif

if( test.nct_open_close gt 2) then  begin
    print, "nct_open_close,/nowrite"
    nct_open_close,"u.nc",/nowrite
endif

if(test.nct_inquire) then begin
    print, "nct_inquire"
    nct_inquire,f
endif

if(test.nct_att_readonly) then begin
    print, "nct_att_readonly"
    nct_att_readonly, f
endif
    
if(test.nct_dim_readonly) then begin
    print, "nct_dim_readonly"
    nct_dim_readonly, f
endif

if(test.nct_var_readonly) then begin
    print, "nct_var_readonly"
    nct_var_readonly, f
endif

if(test.nct_varget_count) then begin
    print, "nct_varget_count"
    nct_varget_count,f
endif

if(test.nct_varget_offset) then begin
    print, "nct_varget_offset"
    nct_varget_offset,f
endif

if(test.nct_varget_stride) then begin
    print, "nct_varget_stride"
    nct_varget_stride,f
endif

if(test.nct_varget_all gt 0) then begin
    varid=5
    print
    print, "The varget_all tests need to be checked manually "
    print, "as they depend on the data in the ncdf file"
    print
    t=test.nct_varget_all
    
    c1=[2,4,6,8,10]
    o1=[2,4,5,6,10]
    s1=[1,2,3,1,2]
    
                                ;fails
    c2=[12,5,10,10,1]          ;0 fails on count
    o2=[0,17,19,0,0]           ;1, fails on offset
    s2=[1,1,2,15,51]           ;2 fails on count+offset+stride
                               ;3 fails on stride + count
                               ;4 fails on stride
    
    
    
    if(t gt 1) then begin
        print, "varget_all count= c1"
        count=c1
        nct_varget_all,f,varid,count=count
    endif
    
    if(t gt 2) then begin
        print, "varget_all count=c1, offset=o1
        nct_varget_all,f,varid,count=c1,offset=o1
    endif
    
    if(t gt 3) then begin
        print, "varget_all count= c1, offset=o1, stride=s1"
        nct_varget_all,f,varid,count=c1,offset=o1,stride=s1
    endif
    
    if(t gt 4) then begin
        print, "varget_all offset=o1,stride=s1"
        nct_varget_all,f,varid,offset=o1,stride=s1
    endif
    
    if(t gt 5) then begin
        print, "varget_all stride=s1"
        nct_varget_all,f,varid,stride=s1
    endif
    
    if(t gt 6) then begin
        print, "varget_all count= c2 (error)"
        nct_varget_all,f,varid,count=c2
    endif
    
    if(t gt 7) then begin
        print, "varget_all count=c2, offset=o2 (error)"
        nct_varget_all,f,varid,count=c2,offset=o2
    endif
    
    if(t gt 8) then begin
        print, "varget_all count= c2, offset=o2, stride=s2 (error)"
        nct_varget_all,f,varid,count=c2,offset=o2,stride=s2
    endif
    
    if(t gt 9) then begin
        print, "varget_all offset=o2,stride=s2 (error)"
        nct_varget_all,f,varid,offset=o2,stride=s2
    endif
    
    if(t gt 10) then begin
        print, "varget_all stride=s2 (error)"
        nct_varget_all,f,varid,stride=s2
    endif
    
    
endif


if(test.nct_varget1_0) then begin
    print, "nct_varget1_0"
    nct_varget1_0, f
endif


if(test.nct_varget1_midpoint) then begin
    print, "varget1_midpoint"
    nct_varget1_midpoint, f
endif

if(test.nct_varget1_neg) then begin
    print, "nct_varget1_neg (error)"
    nct_varget1_neg, f
endif

if(test.nct_varget1_tm) then begin
    print, "nct_varget1_tm (error)"
    nct_varget1_tm, f
endif


;write test

if(test.nct_create_valid) then begin
    print, "nct_create_valid"
print, f2
    nct_create_valid, f2
endif

if(test.nct_create_noclobber) then begin
    print, "nct_crate_noclobber"
    nct_create_noclobber, f2
endif

if(test.nct_create_complex) then begin
    print, "nct_crate_complex"
    nct_create_complex, f3,f4
endif

if(test.nct_create_complex2) then begin
    print, "nct_crate_complex2"
    nct_create_complex2, f3
endif


end
