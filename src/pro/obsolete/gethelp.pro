; obsolete and does nothing
function gethelp,___ttt___,fullstring=fullstring,functions=functions,oneline=oneline,procedures=procedures,sys_procs=sys_procs,sys_funcs=sys_funcs
  if (n_elements(___ttt___) gt 0) then begin
     help,___ttt___,lev=-1,out=h,functions=functions,procedures=procedures
     name=scope_varname(___ttt___,lev=-1)
     l=strlen(h)
     return,name+strmid(h,9,l)
     stop
  endif else begin
     help,lev=-1,out=h,functions=functions,procedures=procedures
  endelse
  
  if keyword_set(oneline) then message,/informational,"oneline kw of gethelp ignored"
  return,h
end
