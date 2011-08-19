;+
; NAME:  FILE_WHICH
; 
; PURPOSE:
; 
; This function searches for a specific file in several specific directories.
;
; SYNTAX:
; 
; result = FILE_WHICH( [path, ] file [, /INCLUDE_CURRENT_DIR] ) 
;
; RETURN VALUE:
;
; If any file is found, returns the full path of the first found file; if no
; file is found, returns a NULL STRING.
;
; ARGUMENTS:
;
; - path:  A search path to be searched. If this argument is not given, the value of
;          !path will be used.
; - file:  The file which is going to be searched for.
;
; KEYWORDS:
;
; - INCLUDE_CURRENT_DIR:  If set, FILE_WHICH function will search current directory for the file before
; searching the directories in the "path" argument. If not set, current
; directory will not be searched.
;
; EXAMPLES:
;
; To search for the file "file_which.pro":
;
; file_which_path = FILE_WHICH( 'file_which.pro' )
;
; MODIFICATION HISTORY:
;   30-Mar-2011 : written by H. Xu (xuhdev), initial import by Sylwester
;   19-Aug-2010 : Alain Coulais : small bug (double last "/")
;
; LICENCE:
; Copyright (C) 2011, H. Xu
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
;
function FILE_WHICH, path, file, include_current_dir=include_current_dir, $
                     help=help, test=test, debug=debug
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'function FILE_WHICH, path, file, include_current_dir=include_current_dir, $'
    print, '                     help=help, test=test, debug=debug'
    return, -1
endif
;
if KEYWORD_SET(test) then begin
    MESSAGE, /continue, "no actif keyword /TEST due to multiple return"
endif
;
if (N_PARAMS() EQ 0) then begin
    MESSAGE, /continue, "No file/path provided !"
    return, ''
end
;
if (N_PARAMS() EQ 2 ) then begin
    the_path = path
    the_file = file
endif else begin
    the_path = !PATH
    the_file = path
endelse
;
if KEYWORD_SET(debug) then STOP
;
; if the file is empty, then we don't need to search for it
if (the_file eq '') then begin
    return, ''
endif
;
; split the_path into a list
paths = STRSPLIT(the_path, PATH_SEP(/SEARCH_PATH), /EXTRACT )
;
; if keyword INCLUDE_CURRENT_DIR is set, then add current path in front of the paths
if (KEYWORD_SET(include_current_dir)) then begin
    CD, CURRENT = cur_path
    paths = [cur_path, paths]
endif
;
path_seper = PATH_SEP() ; path seperator for current platform
n_paths = N_ELEMENTS(paths) ; path count
;
for i = 0, n_paths - 1 do begin
    ;;
    apath = paths[i] ; ith path
    len_apath = STRLEN( apath )
    apath_last_char = STRMID(apath, len_apath-1) ; the last character
    ;;
    ;; if apath doesn't end with a path seperator, then append one onto it
    if ((apath_last_char EQ path_seper ) OR $
        ( !version.os_family EQ 'Windows' AND apath_last_char EQ '/' ) )$
        then begin
        apath = STRMID( apath, 0, len_apath - 1 )
    endif
    ;;
    ;; now apath should be the full path of the file which is to be checked
    apath = apath + path_seper + the_file
    ;;
    ; apath exists means it's what we want
    if (FILE_TEST(apath)) then begin
        return, apath
    endif
endfor
;
; return an empty STRING if we didn't find the file
;
return, ''
;
end
