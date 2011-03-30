;
; Name:  FILE_WHICH
; 
; Purpose:
; 
; This function searches for a specific file in several specific directories.
;
; Syntax:
; 
; result = FILE_WHICH( [path, ] file [, /INCLUDE_CURRENT_DIR] ) 
;
; Return Value:
;
; If any file is found, returns the full path of the first found file; if no
; file is found, returns a NULL string.
;
; Arguments:
;
; path:
;
; A search path to be searched. If this argument is not given, the value of
; !path will be used.
;
; file:
;
; The file which is going to be searched for.
;
; Keywords:
;
; INCLUDE_CURRENT_DIR:
;
; If set, FILE_WHICH function will search current directory for the file before
; searching the directories in the "path" argument. If not set, current
; directory will not be searched.
;
; Examples:
;
; To search for the file "file_which.pro":
;
; file_which_path = file_which( 'file_which.pro' )
;
;
FUNCTION file_which, path, file, include_current_dir=include_current_dir

on_error, 2

IF( n_params() EQ 2 ) THEN BEGIN
    the_path = path
    the_file = file
ENDIF ELSE BEGIN
    the_path = !path
    the_file = path
ENDELSE

; if the file is empty, then we don't need to search for it
IF the_file EQ '' THEN BEGIN
    return, ''
ENDIF

; split the_path into a list
paths = strsplit( the_path, path_sep( /SEARCH_PATH ), /EXTRACT )

; if include_current_dir is set, then add current path in front of the paths
IF( keyword_set( include_current_dir ) ) THEN BEGIN
    cd, CURRENT = cur_path
    paths = [cur_path, paths]
ENDIF

path_seper = path_sep() ; path seperator for current platform
n_paths = n_elements( paths ) ; path count

FOR i = 0, n_paths - 1 DO BEGIN

    apath = paths[i] ; ith path
    len_apath = strlen( apath )
    apath_last_char = strmid( apath, len_apath - 2, 1 ) ; the last character


    ; if apath doesn't end with a path seperator, then append one onto it
    IF( ( apath_last_char EQ path_seper ) OR $
        ( !version.os_family EQ 'Windows' AND apath_last_char EQ '/' ) )$
        THEN BEGIN

        apath = strmid( apath, 0, len_apath - 1 )
    ENDIF

    ; now apath should be the full path of the file which is to be checked
    apath = apath + path_seper + the_file

    ; apath exists means it's what we want
    IF( file_test( apath ) ) THEN BEGIN
        return, apath
    ENDIF
ENDFOR

; return an empty string if we didn't find the file
return, ''

END
