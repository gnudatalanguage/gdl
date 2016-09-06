;+
; NAME: FILE_DELETE
;
;  
;
; RESTRICTIONS:  only for Unix (Unix, Linux and Mac OS X) systems
;
; MODIFICATION HISTORY:
;   - April 2009: created by Alain Coulais, request by Igor C.
;   - June 2010: modified by Lea Noreskal
;-
; LICENCE:
; Copyright (C) 2009, Alain Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
; ---------------------------------
;
pro FILE_DELETE_ONE, list_of_files, allow_nonexistent=allow_nonexistent, $
                     quiet=quiet, verbose=verbose , recursive=recursive, $ 
                     noexpand_path=noexpand_path
  command='\rm '
  option=''

  if KEYWORD_SET(allow_nonexistent) then begin
     option=option+'f'
  endif

  if KEYWORD_SET(recursive) then begin
     option=option+'r'
  endif

  if KEYWORD_SET(verbose) then begin
     option=option+'v'
  endif

  if (STRLEN(option) GT 0) then begin
     option='-'+option
  endif

  
  for ii=0,N_ELEMENTS(list_of_files)-1 do begin
     ; escape special char
     nameescp=escape_special_char(list_of_files[ii])

     if (Not KEYWORD_SET(noexpand_path)) then begin  
        struct=FILE_INFO(list_of_files[ii])
        if (struct.exists) then nameescp=escape_special_char(struct.name)
     endif

     ; empty directory
     emptdir=''
     if (FILE_TEST(nameescp , /directory) and (Not KEYWORD_SET(recursive)) ) then begin  
        res = FILE_SEARCH(list_of_files[ii] , '*' ,  COUNT=nb)
        if ((nb eq 0)  and (Not KEYWORD_SET(recursive))) then  emptdir=' -r'
     endif
     
     res = FILE_INFO(nameescp) 
     
     if ((res.EXISTS eq 1) and (res.WRITE ne 0)) then begin
        SPAWN, command+option+emptdir+' '+nameescp, result, result_error
        
;      /QUIET is used to suppress error message when an error happens. It does not affect the logging of the action which is controlled by /VERBOSE.
        if KEYWORD_SET(verbose) then print,'% FILE_DELETE: Removed file: ', nameescp
        
     endif else if NOT(KEYWORD_SET(quiet)) then begin ; No error message when /QUIET
        if ( res.EXISTS ne 1 ) then if NOT(KEYWORD_SET(allow_nonexistent)) then print, '% FILE_DELETE: No such file: ', list_of_files[ii]
        if ( res.EXISTS eq 1 and res.WRITE eq 0 ) then print, '% FILE_DELETE: No right to delete: ', nameescp 
     endif
     
  endfor
  
end

pro FILE_DELETE, files0,  files1,  files2,  files3,  files4,  files5,  files6,  files7,  files8,  files9, $
                 files10, files11, files12, files13, files14, files15, files16, files17, files18, files19, $
                 files20, files21, files22, files23, files24, files25, files26, files27, files28, files29, $
                 allow_nonexistent=allow_nonexistent, noexpand_path=noexpand_path, recursive=recursive, $
                 quiet=quiet, verbose=verbose

  if ( strupcase(!version.os_family) eq 'WINDOWS') then  message,"Sorry, FILE_DELETE is not available on Windows (FixMe!)"  

  if (N_PARAMS() LT 1) then begin
     print, '% FILE_DELETE: Incorrect number of arguments.'
     return
  endif
  list=list(files0,  files1,  files2,  files3,  files4,  files5,  files6,  files7,  files8,  files9, $
                 files10, files11, files12, files13, files14, files15, files16, files17, files18, files19, $
                 files20, files21, files22, files23, files24, files25, files26, files27, files28, files29)
  for __i__=0, N_PARAMS()-1 do begin
     CALL_PROCEDURE,'FILE_DELETE_ONE',list[__i__],allow_nonexistent=allow_nonexistent, quiet=quiet, verbose=verbose, recursive=recursive, noexpand_path=noexpand_path
  endfor

end








