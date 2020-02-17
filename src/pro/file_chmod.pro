;+
; NAME: FILE_CHMOD
; 
; RESTRICTIONS:  only for Unix (Unix, Linux and Mac OS X) systems
; this is a simple stub but will do on unix.
;-
; LICENCE:
; Copyright (C) 2020, G.Duvert; contact: Alain Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
; ---------------------------------
;
pro FILE_CHMOD, file , Mode, setgid=setgid,setuid=setuid,sticky_bit=sticky_bit, noexpand_path=noexpand_path, a_execute =a_execute , a_read = a_read , a_write = a_write , g_execute = g_execute , g_read = g_read , g_write= g_write,  o_execute=  o_execute, o_read= o_read, o_write= o_write, u_execute= u_execute, u_read= u_read, u_write=u_write

  compile_opt hidden, idl2
  if !VERSION.OS_FAMILY ne 'unix' then MESSAGE,"FILE_CHMOD is only available on Unix Operating Systems (help!) "
  bit=['0','1']
  
  if (Not KEYWORD_SET(noexpand_path)) then begin  
     flist=''
     for ii=0,N_ELEMENTS(file)-1 do begin
        dir=FILE_SEARCH(FILE_DIRNAME(file[ii]),/fully)
        res=FILE_SEARCH(dir, FILE_BASENAME(file[ii]), /fully)
        for jj=0,N_ELEMENTS(res)-1 do begin
           if(FILE_DIRNAME(res[jj], /MARK_DIRECTORY) eq dir) then flist=[flist, res[jj]] ;
        endfor
     endfor
     if(N_ELEMENTS(flist) gt 1) then file=flist[1:*]
  endif

  ; mode is an individual mask set
  if n_elements(mode) eq 0 then begin
     for i=0,n_elements(file)-1 do begin
        ret=file_test(file[i],get_mode=mode) 
        j=0 & if n_elements(o_execute ) then if keyword_set(o_execute ) then mode=(mode or ishft(keyword_set(o_execute),j)  ) else mode=(mode and NOT ishft(1,j)  ) 
        j++ & if n_elements(o_write   ) then if keyword_set(o_write   ) then mode=(mode or ishft(keyword_set(o_write)  ,j)  ) else mode=(mode and NOT ishft(1,j)  )
        j++ & if n_elements(o_read    ) then if keyword_set(o_read    ) then mode=(mode or ishft(keyword_set(o_read)   ,j)  ) else mode=(mode and NOT ishft(1,j)  )
        j++ & if n_elements(g_execute ) then if keyword_set(g_execute ) then mode=(mode or ishft(keyword_set(g_execute),j)  ) else mode=(mode and NOT ishft(1,j)  )
        j++ & if n_elements(g_write   ) then if keyword_set(g_write   ) then mode=(mode or ishft(keyword_set(g_write)  ,j)  ) else mode=(mode and NOT ishft(1,j)  )
        j++ & if n_elements(g_read    ) then if keyword_set(g_read    ) then mode=(mode or ishft(keyword_set(g_read)   ,j)  ) else mode=(mode and NOT ishft(1,j)  )
        j++ & if n_elements(u_execute ) then if keyword_set(u_execute ) then mode=(mode or ishft(keyword_set(u_execute),j)  ) else mode=(mode and NOT ishft(1,j)  )
        j++ & if n_elements(u_write   ) then if keyword_set(u_write   ) then mode=(mode or ishft(keyword_set(u_write)  ,j)  ) else mode=(mode and NOT ishft(1,j)  )
        j++ & if n_elements(u_read    ) then if keyword_set(u_read    ) then mode=(mode or ishft(1,j)  ) else mode=(mode and NOT ishft(1,j)  )
        j++ & if n_elements(sticky_bit) then if keyword_set(sticky_bit) then mode=(mode or ishft(1,j)  ) else mode=(mode and NOT ishft(1,j)  )
        j++ & if n_elements(setgid    ) then if keyword_set(setgid    ) then mode=(mode or ishft(1,j)  ) else mode=(mode and NOT ishft(1,j)  )
        j++ & if n_elements(setuid    ) then if keyword_set(setuid    ) then mode=(mode or ishft(1,j)  ) else mode=(mode and NOT ishft(1,j)  )

        if n_elements(a_execute ) then if keyword_set(a_execute ) then mode=(mode or '0111'o) else mode=(mode and NOT '0111'o)
        if n_elements(a_write   ) then if keyword_set(a_write   ) then mode=(mode or '0222'o) else mode=(mode and NOT '0222'o)
        if n_elements(a_read    ) then if keyword_set(a_read    ) then mode=(mode or '0444'o) else mode=(mode and NOT '0444'o)
        mode=mode and '7777'o
        command='chmod '+string(mode,format='(o)')+" "+file[i]
        spawn,command,result,blahblah
     endfor
     return
  endif
; mode is wholly defined
  command='chmod '+string(mode and '7777'o ,format='(o)')+" "+strjoin(file,' ')
  spawn,command,result,blahblah
end




