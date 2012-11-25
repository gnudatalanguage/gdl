;+
; NAME: SAVE
;
; PURPOSE:
;    Serves as a wrapper around CMSAVE from Craig B. Markwardt CMSVLIB
;    library. You must download and install yourself this CMSVLIB library
;    then add it PATH in your GDL_PATH. This library can be found here:
;    http://cow.physics.wisc.edu/~craigm/idl/cmsave.html
;
; Nota Bene : CMSAVE in CMSV lib. is limited to 30 parameters (without
;             using /append). We use /append to have 100 working parameters
;
; MODIFICATION HISTORY:
;  01-Sep-2006 : written by Joel Gales
;  15-dec-2006 : modifications by Alain Coulais (AC)
;     1/ explicite HTTP link in header to external CMSVLIB library 
;     2/ test via EXECUTE() in pro to warn new users for missing
;           external CMSVLIB library  
;     3/ final test if we need to do or not since CMSAVE crash if nothing to do
;  13-feb-2007 : modifications by AC
;     1/ better management of void call to CMSAVE (case wich give a crash)
;     2/ explicit test and message if unamed variables
;     3/ explicit warning if /all keyword (not managed by CMSAVE)
;     4/ if no filename, indicate the name of the default filename
;  25-Nov-2012 : extension from "only" 30 parameters to 100
;  (this is requested by code Scanamorphos)
;
; LICENCE:
; Copyright (C) 2006, J. Gales; 2012, A. Coulais
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
;
pro SAVE_APPEND, file=filename_def, $
                 p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,  p8,  p9, $
                 p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, $
                 p20, p21, p22, p23, p24, p25, p26, p27, p28, p29
;
nb_params=N_PARAMS()
cmsv_max_params=30
max_params=cmsv_max_params
if (nb_params LT cmsv_max_params) then max_params=nb_params
;
parm = 'cmsave,'
for __i__=0, max_params-1 do begin
    p = STRCOMPRESS('p' + STRING(__i__), /remove_all)
    stat = EXECUTE('arg=ROUTINE_NAMES(' + p +',arg_name=-2)')
    ;; we manage "arg" to reject "unmaned" variables
    if (STRLEN(arg) LT 1) then begin
        txt_mess="Expression must be named variable in this context: <"
        txt_mess=txt_mess+STRCOMPRESS(HELPFORM('', p,/short)+">.")
        ;; AC 27/02/2007: will be better to not do a /continue
        ;; but currently hard to escape from the ON_ERROR, 2 in GDL
        MESSAGE, txt_mess, /continue
        return
    endif
    cmd = arg + '=TEMPORARY(' + p + ')'
    cmd = arg + '=' + p
    stat = EXECUTE(cmd[0])
    parm = parm + arg + ','
endfor
;
parm = STRMID(parm, 0, STRLEN(parm)-1)
parm = parm + ",/APPEND, filename='"+filename_def+"'"
;
; real call to the CMSAVE routine
;
stat=EXECUTE(parm[0])
;
end
;
; -----------------------------------------------------------
;
pro SAVE, p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,  p8,  p9, $
          p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, $
          p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, $
          p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, $
          p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, $
          p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, $
          p60, p61, p62, p63, p64, p65, p66, p67, p68, p69, $
          p70, p71, p72, p73, p74, p75, p76, p77, p78, p79, $
          p80, p81, p82, p83, p84, p85, p86, p87, p88, p89, $
          p90, p91, p92, p93, p94, p95, p96, p97, p98, p99, $          
          filename=filename0, verbose=verbose, xdr=xdr, $
          compatible=compat0, append=append, all=all, $
          status=status, varstatus=colstatus, mtimes=mtimes, $
          names=names, data=data, pass_method=method, $
          errmsg=errmsg, quiet=quiet, nocatch=nocatch, useunit=useunit, $
          test=test
;
ON_ERROR, 2
;
; is the external CMSVlib present ?
if (EXECUTE('res=CMSVLIB(/QUERY)') EQ 0) then begin
   MESSAGE,  /CONTINUE, "Missing CMSVlib in your GDL_PATH or IDL_PATH"
   MESSAGE,  /CONTINUE, "please read SAVE header for help."
   return
endif
;
; 2012-Nov-25: CMSAVE in CMSV lib. is limited to 30 parameters 
;
nb_params=N_PARAMS()
cmsv_max_params=30
max_params=cmsv_max_params
if (nb_params LT cmsv_max_params) then max_params=nb_params
;
; Positional Parameters
parm = 'cmsave,'
for __i__=0, max_params-1 do begin
    p = STRCOMPRESS('p' + STRING(__i__), /remove_all)
    stat = EXECUTE('arg=ROUTINE_NAMES(' + p +',arg_name=-1)')
    ;; we manage "arg" to reject "unmaned" variables
    if (STRLEN(arg) LT 1) then begin
        txt_mess="Expression must be named variable in this context: <"
        txt_mess=txt_mess+STRCOMPRESS(HELPFORM('', p,/short)+">.")
        ;; AC 27/02/2007: will be better to not do a /continue
        ;; but currently hard to escape from the ON_ERROR, 2 in GDL
        MESSAGE, txt_mess, /continue
        return
    endif
    cmd = arg + '=TEMPORARY(' + p + ')'
    cmd = arg + '=' + p
    stat = EXECUTE(cmd[0])
    parm = parm + arg + ','
endfor
;
parm = STRMID(parm, 0, STRLEN(parm)-1)
;
;AC 15/12/06: when calling without argument, CMSAVE gives a
;segmentation fault --> better way to stop
;
if (STRCOMPRESS(STRUPCASE(parm),/remove_all) EQ 'CMSAVE') then begin
   MESSAGE, "no VAR. provided, nothing to do", /continue
   return
endif
;
; Keywords
;
; special case for beginners: what is the default name of the filename ?!
;
if (N_ELEMENTS(filename0) NE 0) then begin
   parm = parm + ',filename=filename0'
   filename_def=filename0[0]
endif else begin
   filename_def = 'cmsave.sav'  ; if future change, see inside CMSAVE 
   print, "% SAVE: default FILENAME is used : ", filename_def
endelse
;
if (N_ELEMENTS(verbose) ne 0) then parm = parm + ',verbose=verbose'
if (N_ELEMENTS(xdr) ne 0) then parm = parm + ',xdr=xdr'
if (N_ELEMENTS(compat0) ne 0) then parm = parm + ',compatible=compat0'
if (N_ELEMENTS(append) ne 0) then parm = parm + ',append=append'
if (N_ELEMENTS(status) ne 0) then parm = parm + ',status=status'
if (N_ELEMENTS(colstatus) ne 0) then parm = parm + ',varstatus=colstatus'
if (N_ELEMENTS(mtimes) ne 0) then parm = parm + ',mtimes=mtimes'
if (N_ELEMENTS(names) ne 0) then parm = parm + ',names=names'
if (N_ELEMENTS(data) ne 0) then parm = parm + ',data=data'
if (N_ELEMENTS(method) ne 0) then parm = parm + ',pass_method=method'
if (N_ELEMENTS(errmsg) ne 0) then parm = parm + ',errmsg=errmsg'
if (N_ELEMENTS(quiet) ne 0) then parm = parm + ',quiet=quiet'
if (N_ELEMENTS(nocatch) ne 0) then parm = parm + ',nocatch=nocatch'
if (N_ELEMENTS(useunit) ne 0) then parm = parm + ',useunit=useunit'
;
; special case: This important keyword is not working now.
;
if (N_ELEMENTS(all) ne 0) then begin
   parm = parm + ',all=all'
   MESSAGE, " warning : keyword /ALL not working now", /continue
endif
;
; help for debugging ...
;
if KEYWORD_SET(test) then begin
   print, parm
   STOP
endif
;
; real call to the CMSAVE routine
;
stat=EXECUTE(parm[0])
;
; when more than 30 parameters, we use /append
;
if (nb_params GT cmsv_max_params) then begin
   SAVE_APPEND, file=filename_def, $
                p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, $
                p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, $
                p50, p51, p52, p53, p54, p55, p56, p57, p58, p59
endif
if (nb_params GT 2*cmsv_max_params) then begin
   SAVE_APPEND, file=filename_def, $
                p60, p61, p62, p63, p64, p65, p66, p67, p68, p69, $
                p70, p71, p72, p73, p74, p75, p76, p77, p78, p79, $
                p80, p81, p82, p83, p84, p85, p86, p87, p88, p89
endif
if (nb_params GT 3*cmsv_max_params) then begin
   SAVE_APPEND, file=filename_def, $
                p90, p91, p92, p93, p94, p95, p96, p97, p98, p99
endif
;
return
end
