;+
; NAME: RESTORE
;
; PURPOSE:
;       Serves as a wrapper around CMSAVE from Craig B. Markwardt CMVSLIB
;       library. You must download and install yourself this CMVSLIB library
;       then add it PATH in your GDL_PATH. This library can be found here:
;       http://cow.physics.wisc.edu/~craigm/idl/cmsave.html
;
; MODIFICATION HISTORY:
;   01-Sep-2006 : written by Joel Gales
;   15-dec-2006 : by Alain Coulais (AC)
;      1/ explicite HTTP link in header to external CMVSLIB library
;      2/ test via EXECUTE() in pro to warn new users for missing
;                   external CMVSLIB library
;   12-feb-2007: modifications by AC: 
;      1a/ managing filename.
;      1b/ default filename if no filename provided (see CMSAVE) 
;      2/ add keyword /verbose ... (but bug in CMRESTORE (2 time print !))
;      3/ warning for 3 keywords not managed yet
;   13-Nov-2012: modifications by AC:
;      1/ feature request 3581835
;      2/ proper STRING conversion of input file name
;      3/ extra keywords /help and /test
;
; LICENCE:
; Copyright (C) 2006-2012, J. Gales and A. Coulais
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
pro RESTORE, filename0, filename=filename1, $
             RELAXED_STRUCTURE_ASSIGNMENT=RELAXED_STRUCTURE_ASSIGNMENT, $
             DESCRIPTION=DESCRIPTION, RESTORED_OBJECTS=RESTORED_OBJECTS, $
             help=help, verbose=verbose, test=test
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'pro RESTORE, filename0, filename=filename1'
    print, '             RELAXED_STRUCTURE_ASSIGNMENT=RELAXED_STRUCTURE_ASSIGNMENT, $'
    print, '             DESCRIPTION=DESCRIPTION, RESTORED_OBJECTS=RESTORED_OBJECTS, $'
    print, '             help=help, verbose=verbose, test=test'
    return
end
;
if (EXECUTE('res=CMSVLIB(/QUERY)') EQ 0) then begin
    MESSAGE, /CONTINUE, "Missing CMSVlib in your GDL_PATH or IDL_PATH"
    MESSAGE, /CONTINUE, "please read RESTORE header for help."
    return
endif
;
; not available KEYWORDs (compatibility reasons)
;
if (N_ELEMENTS(RELAXED_STRUCTURE_ASSIGNMENT) GT 0) then begin
    print, "% RESTORE: keyword RELAXED_STRUCTURE_ASSIGNMENT not managed"
endif
;
if (N_ELEMENTS(DESCRIPTION) GT 0) then begin
    print, "% RESTORE: keyword DESCRIPTION not managed"
endif
;
if (N_ELEMENTS(RESTORED_OBJECTS) GT 0) then begin
    print, "% RESTORE: keyword RESTORED_OBJECTS not managed"
endif
;
; what is the effective filename ?
;
if (N_ELEMENTS(filename0) EQ 0) AND (N_ELEMENTS(filename1) EQ 0) then begin
    ;; filename set by default
    filename = 'cmsave.sav'
    print, "% RESTORE: default FILENAME is used : ", filename
endif else begin
    ;; when f0 and f1 are both provided, param f0 is used ...
    if (N_ELEMENTS(filename1) GT 0) then filename=filename1
    if (N_ELEMENTS(filename0) GT 0) then filename=filename0
    ;;
    ;; one and only one file at a time
    if (N_ELEMENTS(filename) GT 1) then begin
        txt = "Expression must be a scalar or 1 element array in this context: "
        HELP, filename, out=out
        mess='<'+STRTRIM(STRMID(out, STRLEN('filename')),2)+'>'
        MESSAGE, txt + mess
    endif
    ;;
    ;; Can we convert the "filename" into a true string ...
    res=EXECUTE('tmp_name=STRING(filename)')
    if (res EQ 0) then begin
        return
    endif else begin
        filename=tmp_name
    endelse
    ;;
endelse
;
; we check whether the file is here or not ...
;
if (FILE_TEST(filename) NE 1) then begin
    MESSAGE, "FILE not found : "+filename
endif
;
CMRESTORE, filename, verbose=verbose, names=_nme_, $
  data=data, pass_method='struct'
;
for i = 0, N_ELEMENTS(_nme_) - 1 do begin
    dummy = ROUTINE_NAMES(_nme_[i], data.(i), store=-1)
endfor
;
if KEYWORD_SET(test) then STOP
;
end
