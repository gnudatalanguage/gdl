;$Id: lmgr.pro,v 1.2 2010-01-20 11:41:59 slayoo Exp $
;+
;
;
;
; NAME:    
;          LMGR
;
;
; PURPOSE: 
;          return license information
;          emulates an IDL desktop license
;          As GDL does not need license management this routine is
;          for IDL compatibility only
;
;
; CATEGORY: 
;          compatibility
;
;
; CALLING SEQUENCE:
;          res = LMGR(/CLIENTSERVER,/DEMO,/EMBEDDED,
;                     /RUNTIME,/STUDENT,/TRIAL,/VM,
;                    [EXPIRE_DATE=variable],/FORCE_DEMO,
;                    [INSTALL_NUM=variable],[LMHOSTID=variable],
;                    [SITE_NOTICE=variable]
;
;
; OUTPUTS: 1 if no keyword is set
;          0 otherwise   
;
;
;
; COMMON BLOCKS:
;          none
;
; SIDE EFFECTS:
;          none
;
;
; MODIFICATION HISTORY:
; 	Written by:	Marc Schellens
;
;
;
;-
; LICENCE:
; Copyright (C) 2005,
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-

function LMGR,CLIENTSERVER=cs,DEMO=demo,EMBEDDED=eb,$
              RUNTIME=rt,STUDENT=student,TRIAL=trial,VM=vm,$
              EXPIRE_DATE=expire,FORCE_DEMO=force,$
              INSTALL_NUM=install_num,LMHOSTID=hostid,$
              SITE_NOTICE=site_notice
on_error, 2

hostid = '000000000001'
install_num = '00001'
site_notice= 'GNU'

if keyword_set( cs) then return,0
if keyword_set( demo) then return,0
if keyword_set( eb) then return,0
if keyword_set( rt) then return,0
if keyword_set( student) then return,0
if keyword_set( trial) then return,0
if keyword_set( vm) then return,0

return,1
end
