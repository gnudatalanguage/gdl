;+
; NAME:
;	DOC_LIBRARY
;
; PURPOSE:
;	Extract and display documentation headers from a program or routine.
;
; CATEGORY:
;	Documentation
;
; CALLING SEQUENCE:
;	DOC_LIBRARY, procedure
;
; INPUTS:
;	procedure	STRING	The procedure to document.
;
; KEYWORD PARAMETERS:
;	/print	Set to print the output to the default printer.
;
; SIDE EFFECTS:
;	A file is created in /tmp and deleted after use.
;
; RESTRICTIONS:
;	Only one documentation block per file is handled.
;
; EXAMPLE:
;	DOC_LIBRARY, 'doc_library'
;
; MODIFICATION HISTORY:
;	Original: 2013-March-28; SJT (see Feature Requests 3606434)
;
; LICENCE:
;       This code in under GNU GPL v2 or later
;
;-
pro DOC_LIBRARY, proc, print = print, test=test

ON_ERROR, 2

if (!version.os_family ne 'unix') then begin
    print,  "DOC_LIBRARY is currently only available for Unix like systems"
    return
endif

if (KEYWORD_SET(print)) then begin
    less =  FILE_WHICH(getenv('PATH'), 'lp')
    if (less eq '') then less = FILE_WHICH(GETENV('PATH'), 'lpr')
    if (less eq '') then begin
        print, "Neither lp nor lpr was found"
        return
    endif
endif else begin
    less = FILE_WHICH(GETENV('PATH'), 'less')
    if (less eq '') then less = FILE_WHICH(GETENV('PATH'), 'more')
    if (less eq '') then begin
        print, "Neither more nor less was found"
        return
    endif
endelse

proc_path = FILE_WHICH(proc+'.pro', /include_current)
if (proc_path eq '') then begin
    print, proc, ' not found'
    return
endif

out_name = '/tmp/'+proc+'.txt'

OPENR, ipu, proc_path, /get
dflag = 0b
inln = ''

OPENW, isu, out_name, /get

while (~EOF(ipu)) do begin
    READF, ipu, inln
    inln = STRTRIM(inln, 2)
    if (STRPOS(inln, ';+') eq 0) then dflag = 1b
    if (STRPOS(inln, ';-') eq 0) then break
    ;;
    if dflag then printf, isu, STRMID(inln, 1)
endwhile
;
FREE_LUN,  isu,  ipu
;
SPAWN, less+' '+out_name
;
FILE_DELETE, out_name
;
if KEYWORD_SET(test) then STOP
;
end
