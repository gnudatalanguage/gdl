;+
; NAME:     WMENU
;
; PURPOSE: This a emulation of the famous X11 WMENU (obsolete routine).
; We provide a Zenity-based version but also a text-based version.
;
; Credits: with some idea form EMENU (from SolarSoft (Soho, Nasa))
;
; Licence: GNU/GPL (v2 or later)
;
; Contain: WMENU only
;
; CATEGORY: user interface
;
; CALLING SEQUENCE:  answer_index=WMENU(list, title=, init=)
;
; INPUTS:       a list (string or float or integer)
;
; OPTIONAL INPUTS:   none
;
; KEYWORD PARAMETERS:
;        title= index number of title (usually =0)
;        init=  index number of default (usually > 0)
;
; Extended Keywords: 
;        help= display a short help and abort
;        test= will stop inside the function
;
; OUTPUTS:
;        the index of the selected answer in the input list
;
; OPTIONAL OUTPUTS:   none
;
; COMMON BLOCKS:      none
;
; SIDE EFFECTS:       none
;
; RESTRICTIONS:       assumed to be obsolete in IDL since, at least, IDL 5.0 ?
;
; EXAMPLE:            see the test procedure TEST_WMENU
;
; Behavior: 
;
; let NbReps the number of Elements in list
; if title < 0 or title > NbReps, no title managed, any answer OK, rep from 0 to NbReps-1
; if init < 0 or init > NbReps, no init managed, any answer OK
; if title in {0..NbReps-1} then this filed is moved ontop and is not selectable
; Must be noticed too that possible index answer is from 0 to NbReps-2
; if init = title then no init
;
; MODIFICATION HISTORY:
;  - 25-JAN-2006 : created by Alain Coulais
;  - 09-FEB-2006 : various debugging (WHERE does not work with STRINGs!)
;  - 13-FEB-2006 : title is a index, not a string !
;  - 19-NOV-2012 : Zenity-based version, pushed in the public CVS
;-
; LICENCE:
; Copyright (C) 2006-2012, Alain Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
;
function WMENU_TEXT, list_of_choice, title=title, init=init, $
                     strict=strict, test=test, help=help
;
; first line: standart calling sequence
; second line: extra keyword
;
mess={fatal: 'FATAL in WMENU : ', warning: 'WARNING in WMENU : '}
;
; 2 cas : 1: call without mandatory field "list_of_choice"
;
if (N_PARAMS() lt 1) then begin
    print, mess.fatal+'missing mandatory arguments'
    help=1
endif
;
; 2 cas : 2: call with a VOID mandatory field "list_of_choice"
;
if (N_ELEMENTS(list_of_choice) EQ 0) then begin
    print, mess.fatal+'empty mandatory arguments'
    help=1
endif
;
if KEYWORD_SET(help) then begin
    print, 'function WMENU, list_of_choice, title=title, init=init, $'
    print, '                strict=strict, test=test, help=help'
    print, ' '
    print, 'Expected inputs and keywords:'
    print, 'list_of_choice: a string array'
    print, 'title=        : an Integer (like: byte, long, ulong, long64)'
    print, 'init=         : an Integer (like: byte, long, ulong, long64)' 
    print, 'strict=       : enforce strict check of keyword type (title, init)'
    return, -1
endif
;
; If we are here, we have a no-void list, and may-be a title and an init
;
nbp_choice=N_ELEMENTS(list_of_choice)
indices_flag=REPLICATE(1,nbp_choice)
indices=INDGEN(nbp_choice)
local_choice=list_of_choice
;
line='---------------------------------------------------------'
;
; if needed, we manage the title
;
flag_title=0
local_choice=list_of_choice
if (N_ELEMENTS(title) EQ 1) then begin
    ;; if present, the title must be an index (int)
    ;; in order to mimic IDL, we can do a raw type conversion
    ;; or, is explicitaly set, do a clean conversion
    if KEYWORD_SET(strict) then begin
        ;; clean check branch
        if IS_IT_INTEGER(title) then begin
            title=LONG(title)
            flag_title=1
        endif
    endif else begin
        ;; IDL-like branch --> if type of title is wrong,
        ;; first field in [list_of_choice] become The Title !!
        title=LONG(title)
        flag_title=1
    endelse
    if (flag_title EQ 1) then begin
        if (title LT 0) or (title GE nbp_choice) then begin
            print, mess.warning+'[title] out of range, NOT USED !'
            flag_title=0
        endif else begin
            flag_title=1
            print, line
            print, 'Title : ', list_of_choice[title]
            ;; updated choice' list and creation of index list
            indices_flag(title)=0
            local_choice=list_of_choice(WHERE(indices_flag GT 0))
            indices=indices(WHERE(indices_flag GT 0))
        endelse
    endif else begin
        print, mess.warning+'bad TYPE for Title Keyword (no keyword managed)'
    endelse
endif
;
; we change that only here 
nbp_local=N_ELEMENTS(local_choice)
;
; -----------------------------
; management of [init] keyword
;
; 1/ do we have provide one and only one init= value ?
; 2/ is it a "int" type ?
; 3/ we check if "init" is positive and less than the last element ...
;
flag_init=0
if (N_ELEMENTS(init) EQ 1) then begin
    ;; if present, Init must be an index (int)
    ;; in order to mimic IDL, we can do a raw type conversion
    ;; or, is explicitaly set, do a clean conversion
    if KEYWORD_SET(strict) then begin
        ;; clean check branch
        if IS_IT_INTEGER(init) then begin
            init=LONG(init)
            flag_init=1
        endif
    endif else begin
        ;; IDL-like branch --> if type of [init] is wrong,
        ;; first field in [list_of_choice] become [init], it is not a
        ;; problem if [title] is really this field (that means no init ...)
        init=LONG(init)
        flag_init=1
    endelse
    if (flag_init EQ 1) then begin
        if (init LT 0) or (init GE nbp_choice) then begin
            print, mess.warning+'[init] out of range, NOT USED !'
            flag_init=0
        endif else begin
            flag_init=1
        endelse
    endif else begin
        print, mess.warning+'bad TYPE for Init Keyword (no keyword managed)'
    endelse
endif
;
; ---------------------------------
; message will be modified if [init=] is valid
;
message='Enter number (left column) of menu item (no default): '
;
; when we have title and init, init is not usefull if it is EQ to
; title
;
if (flag_title EQ 1) AND (flag_init EQ 1) then begin
    if (title EQ init) then begin
        print, mess.warning+'[init] EQ [title], NOT USED !'
        flag_init=0
    endif else begin
        print, 'Enter a number in left column or'
        print, 'Press Enter to validate [init], which is : ', $
          list_of_choice(init)
        ;;
        message='Enter number of menu item (press [Enter] to select Default): '
        if init GT title then init = init-1
    endelse
endif
;
; --------------------------------------------
; Creation of message, prefixed with a number ...
;
answers_int=indices
answers_str=STRING(answers_int,format='(i2)')
answers_strcompress=STRCOMPRESS(answers_str,/remove_all)
;
; When we have a valid default, we prefix it with a "*"
;
list_flag=REPLICATE('  ', nbp_local)
if (flag_init EQ 1) then list_flag(init)='* '
print, transpose(list_flag+answers_str+' | '+local_choice)
;
if KEYWORD_SET(test) then stop
;
; loop until we have one valid answer ...
;
repeat begin
    go_out=0
    choice=-1
    print,line
    print, message
    entry=''
    read,'---> ', entry
    ;; if the entry is void, we have the "default" value (init)
    if (STRLEN(entry) EQ 0) then begin 
        if (flag_init EQ 1) then begin
            choice=init
            go_out=1
        endif else begin
            choice=-1
        endelse
    endif else begin
        str_choice=STRCOMPRESS(STRING(entry),/remove_all)
        is_it=WHERE(STRPOS(answers_strcompress,str_choice) EQ 0, nbp_is_it)
        if (is_it GE 0) then begin
            choice=answers_int(is_it)
            go_out=1
        endif else begin
            print, 'Out of range !'
        endelse
    endelse
    if KEYWORD_SET(test) then STOP
endrep until (go_out EQ 1)
;
if KEYWORD_SET(test) then begin
    print, 'Selection is: ', local_choice(choice)
    STOP
endif
;
; when we have a title, we change the index by 1
;
if (flag_title EQ 1) then choice=choice
;
return, choice
end
;
; --------------------------------------------------
;
function WMENU_ZENITY, list_of_choice, title=title, init=init, $
                       strict=strict, test=test, help=help, debug=debug
;
zenity=!zenity.name
;
res=EXECUTE('Str_list_of_choice=STRING(list_of_choice)')
if (res EQ 0) then begin
    MESSAGE, 'Problem during conversion into STRING of input selection list'
endif

title="'temporary title'"
cmd_title=' --title '+title
;
text="'temporary text'"
cmd_text=' --text '+text
;
column_text='""'
cmd_column_text=' --column '+column_text
;
reform_list_of_choice=''
for ii=0, N_ELEMENTS(list_of_choice)-1 do begin
    reform_list_of_choice=reform_list_of_choice+' "'+Str_list_of_choice[ii]+'"'
endfor
;
command=zenity+cmd_title+cmd_text+cmd_column_text
command=command+' --list '+reform_list_of_choice
;
if KEYWORD_SET(debug) then HELP, command
;
SPAWN, command, result, error
;
if KEYWORD_SET(debug) then begin
   print, 'SPAWN returned error  : ', error
   print, 'SPAWN returned result : ', result
endif
;
indice=WHERE(result EQ Str_list_of_choice)

indice2=STRPOS(Str_list_of_choice, result)

OK=where(indice2 EQ 0, nb_OK)
if (nb_OK GT 1) then begin
    print, 'Warning, More than one entry found'
endif
indice=OK[0]
;
return, indice
;
end
;
; --------------------------------------------------
;
function WMENU, list_of_choice, title=title, init=init, $
                test=test, help=help, debug=debug
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'function WMENU, list_of_choice, title=title, init=init, $'
    print, '                test=test, help=help, debug=debug'
    return, -1
end
;
; In WMENU, we don't plan to play with Zenity Version or Zenity Path
;
; Do we had previoulsy checked whether Zenity is available or not ?
; (if Yes, the !Zenity variable is created and populated)
;
;
DEFSYSV, '!zenity', exists=preset_zenity
if ~preset_zenity then begin
    zenity=ZENITY_CHECK()
    if (!zenity.version LT 0) then begin
        txt='zenity was not found, we will used the TEXT based version of WMENU()'
        MESSAGE, /continue, txt
        txt='If you want to use Zenity based WMENU, please add zenity'
        MESSAGE, /continue, txt
        MESSAGE, /continue, '  On Debian/Ubuntu like distros: sudo apt-get install zenity'
        MESSAGE, /continue, '  On RH/FC/CentOS like distros: yum install zenity'
        MESSAGE, /continue, '  On OSX distros: port install zenity'
    endif
endif
;
if (!zenity.version LT 0) then begin
    resu=WMENU_TEXT(list_of_choice, title=title, init=init, $
                    test=test, help=help)
endif else begin
    resu=WMENU_ZENITY(list_of_choice, title=title, init=init, $
                      test=test, help=help, debug=debug)
endelse
;
if KEYWORD_SET(help) then STOP
;
return, resu
;
end

