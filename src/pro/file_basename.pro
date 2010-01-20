;+
; NAME: FILE_BASENAME
;
; PURPOSE: The FILE_BASENAME function returns the basename of a file
; path. A file path is a string containing one or more segments
; consisting of names separated by directory delimiter characters
; (slash (/) under UNIX, or backslash (\) under Microsoft
; Windows). The basename is the final rightmost segment of the file
; path; it is usually a file, but can also be a directory name. 
;
; FILE_BASENAME is based on the standard UNIX basename(1) utility. 

; CATEGORY: UNIX utility.
;
; CALLING SEQUENCE:  Result = FILE_BASENAME(Path [, RemoveSuffix] [, /FOLD_CASE]) 
;
; INPUTS: Path: A scalar string or string array containing paths for
; which the basename is desired.  
;
; OPTIONAL INPUTS: RemoveSuffix: An optional scalar string or
; 1-element string array specifying a filename suffix to be removed
; from the end of the basename, if present.  
;
; KEYWORD PARAMETERS:  /FOLD_CASE is not available now (and useless
; for Unix). return -1 if activated
;
; OUTPUTS: A scalar string or string array containing the basename for
; each element of the Path argument.  
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS:  none
;
; RESTRICTIONS:  only for Unix (Unix, Linux and Mac OS X) systems
;
; Rules used by FILE_BASENAME 
; FILE_BASENAME makes a copy of the input file path string, then
; modifies the copy according to the following rules:  
;    - If Path is a NULL string, then FILE_BASENAME returns a NULL
;    string. 
;    - If Path consists entirely of directory delimiter characters, the
;    result of FILE_BASENAME is a single directory delimiter character. 
;    - If there are any trailing directory delimiter characters, they
;    are removed. 
;    - If any directory delimiter characters remain, all characters up
;    to and including the last directory delimiter are removed.  
;    - If the RemoveSuffix argument is present, is not identical to the
;    characters remaining, and matches the suffix of the characters
;    remaining, the suffix is removed. Otherwise, the Result is not
;    modified by this step.  
;
; PROCEDURE:
;
; EXAMPLE:   
;
; print, file_basename('/usr/local/rsi/idl/lib/dist.pro', '.pro')  
; IDL prints: 
; dist  
;
; MODIFICATION HISTORY:
;   - Sept 2007: created by Sebastien Masson
;   - Sept 2007: managing insufficient numbers of parameters, /help
;
;-
; LICENCE:
; Copyright (C) 2007, Sebastien Masson
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
;
FUNCTION FILE_BASENAME, Path, RemoveSuffix, FOLD_CASE = fold_case, help=help
  on_error, 2
;
if KEYWORD_SET(help) then begin
   print, 'FUNCTION FILE_BASENAME, Path, [RemoveSuffix], [/FOLD_CASE], [/help]'
   return, -1
endif
;
IF ((N_PARAMS() LT 1) OR (N_PARAMS() GT 2)) THEN BEGIN
   message, 'Incorrect number of arguments.'
ENDIF
;
IF KEYWORD_SET(fold_case) THEN BEGIN
    message, 'Sorry, Keyword fold_case is not available now.'
ENDIF

SFX = N_ELEMENTS(RemoveSuffix) NE 0 ? RemoveSuffix : ''
RESULT = STRARR(N_ELEMENTS(Path))
FOR I = 0, N_ELEMENTS(Path) - 1 DO BEGIN
  IF STRTRIM(PATH[I], 2) NE '' THEN BEGIN
    SPAWN, '\basename ' + PATH[I] + ' ' + SFX, RES
    RESULT[I] = TEMPORARY(RES)
  ENDIF ELSE RESULT[I] = PATH[I]
ENDFOR

return, size(path, /N_DIM) eq 0 ? result[0] : result
END
