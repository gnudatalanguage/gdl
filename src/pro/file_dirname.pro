;+
; NAME: FILE_DIRNAME
;
; PURPOSE: The FILE_DIRNAME function returns the dirname of a file
; path. A file path is a string containing one or more segments
; consisting of names separated by directory delimiter characters
; (slash (/) under UNIX, or backslash (\) under Microsoft
; Windows). The dirname is the final rightmost segment of the file
; path; it is usually a file, but can also be a directory name. 
;
; FILE_DIRNAME is based on the standard Unix dirname(1) utility. 
;
; CATEGORY: UNIX utility.
;
; CALLING SEQUENCE: Result = FILE_DIRNAME(Path [, /MARK_DIRECTORY]) 
;
; INPUTS: Path: A scalar string or string array containing the dirname
; for each element of the Path argument.  
;
; OPTIONAL INPUTS: none 
;
; KEYWORD PARAMETERS:  /MARK_DIRECTORY: Set this keyword to include a
; directory separator character at the end of the returned directory
; name string. Including the directory character allows you to
; concatenate a file name to the end of the directory name string
; without having to supply the separator character manually. This is
; convenient for cross platform programming, as the separator
; characters differ between operating systems.
;
; OUTPUTS: Result: A scalar string or string array containing the
; dirname for each element of the Path argument.
;    Note: By default, the dirname does not include a final directory
;    separator character; this behavior can be changed using the
;    MARK_DIRECTORY keyword. 
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS:  none
;
; RESTRICTIONS:  only for Unix (Unix, Linux and Mac OS X) systems
;
; Rules used by FILE_DIRNAME 
; FILE_DIRNAME makes a copy of the input path string, and then
; modifies the copy according to the following rules:  
;    - If Path is a NULL string, then FILE_DIRNAME returns a single
;    dot (.) character, representing the current working directory of
;    the IDL process.   
;    - If Path consists entirely of directory delimiter characters,
;    the result of FILE_DIRNAME is a single directory delimiter
;    character. 
;    - All characters to the right of the rightmost directory
;      delimiter character are removed. 
;    - All trailing directory delimiter characters are removed. 
;    - If the MARK_DIRECTORY keyword is set, a single directory
;      delimiter character is appended to the end.
;
; PROCEDURE:
;
; EXAMPLE:   
;
; print, file_dirname('/usr/local/rsi/idl/lib/dist.pro')  
; IDL prints: 
; /usr/local/rsi/idl/lib
;
; MODIFICATION HISTORY:
;   - Sept 2007: created by Sebastien Masson
;   - Setp 2007: mananing wrong numbers of parameters, /help
;   - June 2010: escape special characters by Lea Noreskal
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
FUNCTION FILE_DIRNAME, Path, MARK_DIRECTORY = mark_directory, help=help
  on_error, 2
;
if KEYWORD_SET(help) then begin
   PRINT, 'FUNCTION FILE_DIRNAME, Path [, /mark_directory] [, /help]'
   return, -1
endif
;
IF (N_PARAMS() NE 1) THEN BEGIN
   MESSAGE, 'Incorrect number of arguments.'
ENDIF
;
command = '\dirname ' + ESCAPE_SPECIAL_CHAR(Path)
SPAWN, command, result
;
IF KEYWORD_SET(mark_directory) THEN result = result + PATH_SEP()
;
return, result
;
END
