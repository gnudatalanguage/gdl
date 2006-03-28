pro save,    p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,  p8,  p9, $
             p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, $
             p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, $
             filename=filename0, verbose=verbose, xdr=xdr, $
             compatible=compat0, append=append, all=all, $
             status=status, varstatus=colstatus, mtimes=mtimes, $
             names=names, data=data, pass_method=method, $
             errmsg=errmsg, quiet=quiet, nocatch=nocatch, useunit=useunit


; Positional Parameters
parm = 'cmsave,'
for __i__=0,n_params()-1 do begin
    p = strcompress('p' + string(__i__), /remove_all)
    stat = execute('arg=routine_names(' + p +',arg_name=-1)')
    cmd = arg + '=temporary(' + p + ')'
    cmd = arg + '=' + p
;print,cmd
    stat = execute(cmd)
    parm = parm + arg + ','
endfor

parm = strmid(parm, 0, strlen(parm)-1)

; Keywords
if (n_elements(filename0) ne 0) then parm = parm + ',filename=filename0'
if (n_elements(verbose) ne 0) then parm = parm + ',verbose=verbose'
if (n_elements(xdr) ne 0) then parm = parm + ',xdr=xdr'
if (n_elements(compat0) ne 0) then parm = parm + ',compatible=compat0'
if (n_elements(append) ne 0) then parm = parm + ',append=append'
if (n_elements(all) ne 0) then parm = parm + ',all=all'
if (n_elements(status) ne 0) then parm = parm + ',status=status'
if (n_elements(colstatus) ne 0) then parm = parm + ',varstatus=colstatus'
if (n_elements(mtimes) ne 0) then parm = parm + ',mtimes=mtimes'
if (n_elements(names) ne 0) then parm = parm + ',names=names'
if (n_elements(data) ne 0) then parm = parm + ',data=data'
if (n_elements(method) ne 0) then parm = parm + ',pass_method=method'
if (n_elements(errmsg) ne 0) then parm = parm + ',errmsg=errmsg'
if (n_elements(quiet) ne 0) then parm = parm + ',quiet=quiet'
if (n_elements(nocatch) ne 0) then parm = parm + ',nocatch=nocatch'
if (n_elements(useunit) ne 0) then parm = parm + ',useunit=useunit'

;print,parm

stat=execute(parm)

return
end
