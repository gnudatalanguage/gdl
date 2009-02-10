;+
; NAME: FILE_LINES
;
; PURPOSE: return the number of lines in an ASCII file
;
; CATEGORY:
;
; CALLING SEQUENCE:  nb_lines=FILE_LINES(filename)
;
; INPUTS:  -- the file name, can be prefixed by the path
;
; OPTIONAL INPUTS: none
;
; KEYWORD PARAMETERS:  /compress is not available now
;                      /noexpand_path is not available now
;
; OUTPUTS: -- the line number, in Long type
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS: none
;
; SIDE EFFECTS:  none
;
; RESTRICTIONS:  only for Unix (Unix, Linux and Mac OS X) systems
;
; PROCEDURE:
;
; EXAMPLE:   print, FILE_LINES("/etc/passwd")
;
; MODIFICATION HISTORY:
;   - 26/07/2006: created by Alain Coulais (ARSC)
;   - 30/05/2008: Michael Mueller (U of Arizona) fixed inconsistent
;     handling of files that don't end in newline
;
;-
; LICENCE:
; Copyright (C) 2006, Alain Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
;
function FILE_LINES, filename, compress=compress, noexpand_path=noexpand_path
;
if KEYWORD_SET(compress) then begin
    print, 'Sorry, Keyword COMPRESS is not available now.'
    return, -1
endif
;
if KEYWORD_SET(noexpand_path) then begin
    print, 'Sorry, Keyword NOEXPAND_PATH is not available now.'
    return, -1
endif
;
commande="paste "+filename+" | wc -l | awk '{print $1}'"
SPAWN, commande, resultat
nbp=(LONG(STRCOMPRESS(resultat,/remove_all)))(0)
;
return, nbp
;
end
