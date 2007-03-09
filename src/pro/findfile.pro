;+
; NAME:        FINDFILE
;
; This functionality is obsolete. It should not be used in new GDL
; code. You are adviced to use FILE_SEARCH() instead, which is
; intresic in GDL.
;
; PURPOSE:     return a string array containing the list of files
;
; CATEGORY:     File Manipulation
;
; CALLING SEQUENCE:  list=FINDFILE([filter], [count=count])
;        where filter can be like '*.txt'
;
; INPUTS:   nothing
;           OR a string filter, which may be void ('') 
;           or containing some wildcards ('?' and '*')
;
; KEYWORD PARAMETERS: 
;   count=         the number of files matching the filter
;
; Extended KEYWORD:   
;   sh_location=   a string in order to quickly substitute the default
;                  "sh location" (/bin/sh) to another one
;   spawn_options= a 3 element string array in order to quickly
;                  substitute the default use of SPAWN call
;
; OUTPUTS:    a string array if at least one file matches the filter
;          OR a void string if nothing found
;
; OPTIONAL OUTPUTS: count= the number of founded elements
;
; COMMON BLOCKS:    none
;
; SIDE EFFECTS:    Since using SPAWN, intrinsically dangerous,
;                  see Restrictions below
;
; RESTRICTIONS: 
;
; 0/  This functionality is obsolete. Only provideto avoid
; desapointing for old users of IDL moving to GDL with old codes.
;
; I/ Dependence to OS:  This function is dependant of the location of "sh"
; shell. It has been verified that the current version is working on
; some Linux and a Mac OS X systems.
;
; II/ Intrinsic danger of Shell calling: Since we allow to substitute
; the Unix command which may called throw SPAWN, we have to say that
; miss-use of this function is possible [this problem was mentioned
; in news:comp.lang.idl-pvwave]
;
; III/ We use a  trick "post_ls=' 2> /dev/null'" in order to avoid
; a message coming from "ls". I don't know the behavior on other OS
; If needed (to be check on other systems), a possible Keyword
; "dev_null" is ready for future uses.
;
; EXAMPLE: 
;    print, FINDFILE('*.pro')
;    print, FINDFILE('*.pro', spawn_options=['/bin/bash','-c','ls'])
;
; MODIFICATION HISTORY:
;   - 21/03/2006 written by Alain Coulais (ARSC)
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
function FINDFILE, filtre_, count=count, $
                   sh_location=sh_location, spawn_options=spawn_options, $
                   verbose=verbose, quiet=quiet, test=test, help=help
;
ON_ERROR, 2
;
;
if not(KEYWORD_SET(quiet)) then begin
   print, 'This functionality is obsolete.'
   print, 'You are adviced to use FILE_SEARCH() instead.'
endif
;
; When nothing is found ...
;
void_answer=''
void_count=0
;
name_of_proc='FINDFILE'
mess={fatal:   'Fatal in "'+name_of_proc+'" : ', $
      info:    'Info in "'+name_of_proc+'" : ', $
      warning: 'Warning in "'+name_of_proc+'" : '}
;
if KEYWORD_SET(help) then begin
    print, 'function FINDFILE, filtre, count=count, $'
    print, '                   sh_location=sh_location, spawn_options=spawn_options, $'
    print, '                   verbose=verbose, test=test, help=help'
    count=void_count
    return, void_answer
endif
;
; ---- now we manage the "input filtre" : we have 3 cases: 0, 1, >1
;      fields in "filter"
;
if (N_PARAMS() EQ 0) then filtre=void_answer
if (N_ELEMENTS(filtre_) EQ 1) then filtre=filtre_
if (N_ELEMENTS(filtre_) GT 1) then begin
    ;; unfortunatelly, the behavior of ON_ERROR/Message do not fit
    ;; exactly the IDL one then I use /continue
    mess_txt='Expression must be a scalar or 1 element array in this context:'
    MESSAGE, mess_txt
    ;;   return, void_answer
    ;; WARNING--> I expected to write only
    ;; MESSAGE, mess_txt
endif
;
; From here, "filtre" is known
; We have to check whether "filtre" is String type or not
;
if (SIZE(filtre,/type) NE 7) then begin
    MESSAGE, 'String expression required in this context'
;;    return, void_answer
    ;; WARNING--> I expected to write only
    ;; MESSAGE, 'String expression required in this context'
endif
;
; ------------------------ Spawn Managment -----------------
;
; basic case is to use "ls -d 
;
; We assume the SHELL "sh" is in PATH : /bin
; This is OK on some Linux and Mac OS X
;
if (N_ELEMENTS(spawn_options) EQ 3) then begin
    ;; is it a String Array ?
    if (SIZE(spawn_options,/TYPE) EQ 7) then begin
        if KEYWORD_SET(verbose) then begin
            print, mess.info+' [spawn_options=] was used'
        endif
    endif else begin
        if KEYWORD_SET(verbose) then begin
            print, mess.warning+' [spawn_options=] was not a STRING array (3 elements)'
        endif
    endelse
endif else begin
    spawn_options=['/bin/sh', '-c', 'ls']
    if KEYWORD_SET(verbose) then begin
        print, mess.info+' DEFAULT [spawn_options=] used'
        print, TRANSPOSE(spawn_options)
    endif
endelse
;
; do we have a new way to call "sh" ?
; Not very useful because of [spawn_options=] solution
;
if (N_ELEMENTS(sh_location) EQ 1) then begin
    ;; is it a String ?
    if (SIZE(sh_location,/TYPE) EQ 7) then begin
        if KEYWORD_SET(verbose) then begin
            print, mess.info+' [sh_location=] was used'
        endif
        spawn_options[0]=sh_location
    endif else begin
        if KEYWORD_SET(verbose) then begin
            print, mess.warning+' [sh_location=] was not a STRING'
        endif
    endelse
endif
;
; Warning : in order to output nothing when nothing is found
; we have to add a subtile redicection
; to avoid message: <<ls: : No such file or directory>>
; Since we don't now how it may work on other systems,
; we do not manage it fully ....
;
dev_null=1
if KEYWORD_SET(dev_null) then begin
    post_ls=' 2> /dev/null'
endif else begin
    post_ls=''
endelse
;
; NOW, we add the filter to the "ls" command
; (the "extra" space ' ' is a simple way to avoid usual error !)
;
spawn_options[2]=spawn_options[2]+' '+filtre+post_ls
;
; ------------------------------------------------
; NOW, all is ready, we SPAWN the command
;
SPAWN, spawn_options, resultat, /noshell
;
; ------------------------------------------------
; We have to process the COUNT keyword parameter
;
info_resultat=SIZE(resultat)
if (info_resultat(0) EQ 0) then begin
    count=0
endif else begin
    count=N_ELEMENTS(resultat)
endelse
;
; ------------------------------------------------
; If Verbosity is requested ...
;
if KEYWORD_SET(verbose) then begin
    print, mess.info+'Filter was: ... <<',filtre,'>>'
    if (count GT 0) then begin
        print, mess.info+'Number of founded elements:', count
        print, mess.info+'Values are:', resultat
    endif else begin
        print, mess.warning+'Nothing founded !'
    endelse
endif
;
if KEYWORD_SET(test) then STOP
;
return, resultat
;
end
;
