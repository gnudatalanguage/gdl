;+
; NAME: 
;	DELVAR
; PURPOSE: 
; 	As IDL's DELVAR internal function (GDL:FIXME!), using
; 	code taken from 'delvarx.pro' under BSD license, all rights reserved.
; CALLING SEQUENCE:
; 	DELVAR,  vra1, var2 .... var48 
;
; INPUTS: 
;	p0, p1... p48 - variables to delete
;
; RESTRICTIONS: 
;	Can't use recursively due to EXECUTE function
;
; METHOD: 
;	Uses EXECUTE and TEMPORARY function (old version of delvarx)
;       new version of delvarx using ptr_new(/no_copy) does not work
;       with GDL 
;
; REVISION HISTORY:
;	Copied from the Solar library, written by slf, 25-Feb-1993
;	Added to Astronomy Library,  September 1995
;	Converted to IDL V5.0   W. Landsman   September 1997
;       Modified, 26-Mar-2003, Zarro (EER/GSFC) 26-Mar-2003
;       - added FREE_MEM to free pointer/objects
;       Modified, 26-Jul-2013 as 'delvar' for GDL, with 48
;       parameters,by G. Duvert
;-

PRO delvar, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47
; 48 is enough?
   FOR i = 0, N_PARAMS()-1 DO BEGIN ; for each parameter
      param = STRCOMPRESS("p" + STRING(i),/remove)
;  only delete if defined on input (avoids error message)
      exestat = execute("defined=n_elements(" + param + ")" ) 

      IF defined GT 0 THEN BEGIN
         exestat = execute("heap_free," + param)
         exestat = execute(param + "=0")
         exestat = execute("dvar=temporary(" + param + ")" )
      ENDIF
   ENDFOR
   RETURN
END
