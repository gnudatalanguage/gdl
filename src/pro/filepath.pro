;+
; NAME: FILEPATH
;
; PURPOSE:
;       1) If /tmp keyword is set, returns a default directory for temporary
;          files
;       2) If /terminal keyword is set, returns the user's terminal filename
;       3) Otherwise, concatenate the specified filename with the directory
;          given by the root_dir keyword (or the GDL install directory if the
;          root_dir keyword is not specified)
;
;
; CATEGORY:
;       File Manipulation
;
;
; CALLING SEQUENCE:
;       result=filepath(filename,root_dir=,subdirectory=,/tmp,/terminal)
;
;
; INPUTS:
;       filename     Scalar string of the filename
;       root_dir     Scalar string of the directory
;       subdirectory Scalar or Array of subdirectories which will be
;                    concatenated to root_dir
; KEYWORDS:
;       tmp          If set, returns a directory for temporary files
;       terminal     If set, returns the filename of the user's terminal
;
; RESTRICTION:
;       the tmp directory is hardcoded to /usr/tmp...
;
; MODIFICATION HISTORY:
;   12-Jan-2006 : written by Pierre Chanial
;
; LICENCE:
; Copyright (C) 2006, P. Chanial
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-

function filepath_concat, dir1_, dir2_
 compile_opt hidden
 on_error, 2
 
 ; prevents inputs from being modified
 dir1 = dir1_
 dir2 = dir2_
 
 case !version.os_family of
    'Windows': sep = '\'
    'unix'   : sep = '/'
    else     : message, 'Operating System not handled.'
 endcase
 
 if strmid(dir1, 0, /reverse_offset) eq sep then begin
    dir1 = strmid(dir1, 0, strlen(dir1)-1)
 endif
 if strmid(dir2, 0, 1) eq sep then begin
    dir2 = strmid(dir2, 1)
 endif
 
 return, dir1+sep+dir2
 
end

function filepath, filename, root_dir=root_dir, subdirectory=subdirectory, terminal=terminal, tmp=tmp

 on_error, 2
 
 if keyword_set(terminal) then begin
    return, '/dev/tty'
 endif
 
 if keyword_set(tmp) then begin
    root_dir = getenv('GDL_TMPDIR') ; IDL has an IDL_TMPDIR variable
    root_dir = '/usr/tmp'
    path = root_dir
 endif else begin
    if n_elements(root_dir) eq 0 then root_dir = !dir
    path = root_dir
    for i=0, n_elements(subdirectory)-1 do begin
       path = filepath_concat(path, subdirectory[i])
    endfor
 endelse
 
 return, filepath_concat(path, filename)
 
end
