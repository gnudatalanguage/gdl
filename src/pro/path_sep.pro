;+
; NAME: PATH_SEP
;
; PURPOSE: 1) Returns the character used by the OS to separate directories
;          and filenames.
;          2) if keyword SEARCH_PATH is set, returns the character used by the
;          OS to separate libraries, like for instance in !PATH
;          3) if keyword PARENT_DIRECTORY is set, returns the string used
;          by the OS to refer to the parent directory
;
;
; CATEGORY:
;       File Manipulation
;
;
; CALLING SEQUENCE:
;       result=path_set([/SEARCH_PATH | /PARENT_DIRECTORY])
;
;
; KEYWORDS:
;       search_path       If set, returns the library separator
;       parent_directory  If set, returns the parent directory
;
;
; MODIFICATION HISTORY:
;   19-Jan-2006 : written by Pierre Chanial
;   26-Jun-2009 : Alain Coulais: 
;     *      better hierarchy in cascading if/then/else  
;     *      correction of bug : returning pure String, not array
;     (before: STRING    = Array[1]; now  STRING    = '/')
;
; LICENCE:
; Copyright (C) 2006, P. Chanial
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-

function PATH_SEP, parent_directory=parent_directory, $
                   search_path=search_path, test=test
on_error, 2

if KEYWORD_SET(search_path) then begin
   if KEYWORD_SET(parent_directory) then begin
      MESSAGE, /info, 'Conflicting keywords specified. Returning SEARCH_PATH.'
   endif
   array = [':', ';']
endif else begin
   if KEYWORD_SET(parent_directory) then begin
      return, '..'
   endif else begin
      array = ['/', '\']
   endelse
endelse

OS = ['unix', 'Windows']
iOS = WHERE(OS eq !version.os_family)

if KEYWORD_SET(test) then STOP

return, (array[iOS])[0]  ; force conversion

end
