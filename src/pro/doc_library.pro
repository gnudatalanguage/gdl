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
;	/nowait read documentation in another window while GDL session goes on.
; SIDE EFFECTS:
;	A file is created in /tmp and deleted after use (unless /nowait is successful).
;
; RESTRICTIONS:
;	Only one documentation block per file is handled.
;
; EXAMPLE:
;	DOC_LIBRARY, 'doc_library'
;
; MODIFICATION HISTORY:
;	Original: 2013-March-28; SJT (see Feature Requests 3606434)
;        Mods for win32 2015-May-18 G. Jung
;	/NOWAIT keyword for win32 (Windows) and 'X' (unix)
; LICENCE:
;       This code in under GNU GPL v2 or later
;
pro DOC_LIBRARY, proc, print = print, test=test, nowait=nowait
;-

if ~keyword_set(test) then ON_ERROR, 2

addexe="" & tempprefix="/tmp/"
if (!version.os_family eq 'Windows') then begin
   addexe=".EXE"
   tempprefix=getenv("TEMP")+"\"
endif
if (KEYWORD_SET(print)) then begin
    less =  FILE_WHICH(getenv('PATH'), 'lp')
    if (less eq '') then less = FILE_WHICH(GETENV('PATH'), 'lpr')
    if (less eq '') then begin
        print, "Neither lp nor lpr was found"
        return
    endif
endif else begin
    less = FILE_WHICH(GETENV('PATH'), 'less'+addexe)
    if (less eq '') then less = FILE_WHICH(GETENV('PATH'), 'more'+addexe)
    if (less eq '') then begin
	if (!version.os_family eq 'Windows') then less='more' $
	else begin
	    print, "Neither more nor less was found"
	    return
	endelse
    endif
endelse

proc_path = FILE_WHICH(proc+'.pro', /include_current)
if (proc_path eq '') then begin
    print, proc, ' not found'
    return
endif

out_name = tempprefix+proc+'.txt'

OPENR, ipu, proc_path, /get
dflag = 0b
inln = ''

OPENW, isu, out_name, /get
dflag=0
while (~EOF(ipu)) do begin
    READF, ipu, inln
    inln = STRTRIM(inln, 2)
    if (STRPOS(inln, ';') eq 0) then begin
		if (STRPOS(inln, ';+') eq 0) then dflag = 1b
		if (STRPOS(inln, ';-') eq 0) then break
		inln = strmid(inln,1)
		endif else if dflag then inln='@>'+inln
    ;;
    if dflag then printf, isu, inln
endwhile
;
free_lun, isu
free_lun, ipu
;FREE_LUN,  isu,  ipu
;
if(keyword_set(nowait)) then begin
  if (!version.os_family eq 'unix') then  begin
	if(!D.name eq 'X') then $
	    SPAWN, 'xterm -e '+less+' '+out_name+' &' $
	else begin
	  print," You need !D.name='X' to do that /nowait"
          SPAWN, less+' '+out_name
         FILE_DELETE, out_name
        endelse
  endif else SPAWN,/nowait, less+' '+out_name
endif 
;
if(not keyword_Set(nowait)) then begin
     SPAWN, less+' '+out_name
     FILE_DELETE, out_name
endif
;
if KEYWORD_SET(test) then STOP
;
end
