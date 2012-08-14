;+
;
; NAME: 
;     WRITE_ENVI_FILE
; PURPOSE: 
;     Writes an ENVI style formatted image file (both compressed and 
;     uncompressed). 
;
; CATEGORY:
;     File I/O
;
; CALLING SEQUENCE:
;     WRITE_ENVI_FILE, data, out_name=outname, ns=ns, nl=nl, nb=nb, 
;                      interleave=interleave, bnames=bnames, data_type=dtype, 
;                      byte_order=byte_order, descrip=descrip, 
;                      compress=compress, help=help, test=test
;
; KEYWORD PARAMETERS:
;     data: The variable containing the image data
;     out_name: A string detailing the name of the file to be written
;     ns: The number of samples in the array
;     nl: The number of lines in the array
;     nb: The number of bands in the array
;     interleave: Optional; specify the interleave (0:BSQ, 1:BIL, 2:BIP)
;     bnames: Optional; specify the band names to be output
;     data_type: Optional; Specify the data type of the array (Check IDL/GDL
;               data types for relevant codes)
;     byte_order: Optional; Specify the byte ordering of the machine
;     descrip: Optional; Specify a description of the data
;     Compress: If set will compress the file.
;     HELP: Showing how to use and exit
;
; OUTPUTS:
;        Writes an ENVI style formatted image file.
;
; MODIFICATION HISTORY:
; 02-Jul-2012: Written by Josh Sixsmith
;
; LICENCE:
; Copyright (C) 2012, Josh Sixsmith
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 3 of the License, or     
; (at your option) any later version.                                   
;
;-

pro WRITE_ENVI_FILE, data, out_name=outname, ns=ns, nl=nl, nb=nb, $
                     interleave=interleave, bnames=bnames, data_type=dtype, $
                     byte_order=byte_order, descrip=descrip, $
                     compress=compress, help=help, test=test

IF KEYWORD_SET(help) THEN BEGIN
   PRINT, 'pro WRITE_ENVI_FILE, data, out_name=outname, ns=ns, $'
   PRINT, '                     nl=nl, nb=nb, interleave=interleave, $'
   PRINT, '                     bnames=bnames, data_type=dtype, $'
   PRINT, '                     byte_order=byte_order, descrip=descrip, $'
   PRINT, '                     compress=compress, help=help, test=test'
   return
ENDIF
;;
IF N_ELEMENTS(outname) EQ 0 THEN MESSAGE, 'Output filename not specified.'
IF N_ELEMENTS(ns) EQ 0 THEN MESSAGE, 'Number of samples not specified.'
IF N_ELEMENTS(nl) EQ 0 THEN MESSAGE, 'Number of lines not specified.'
IF N_ELEMENTS(nb) EQ 0 THEN MESSAGE, 'Number of bands not specified.'
;;
IF N_ELEMENTS(interleave) EQ 0 THEN interleave=0
IF N_ELEMENTS(offset) EQ 0 THEN offset=0
IF N_ELEMENTS(byte_order) EQ 0 THEN byte_order = (BYTE(1,0,1))[0] ? 0 : 1
IF N_ELEMENTS(dtype) EQ 0 THEN dtype = SIZE(data, /type)
;;
;; Write the header file
hname = outname + '.hdr'
OPENW, lun, hname, /get_lun
;;
IF N_ELEMENTS(descrip) EQ 0 THEN BEGIN
   PRINTF, lun, format = '(%"ENVI \ndescription= {\n    Create New File Result [%s]}")', SYSTIME()
ENDIF ELSE BEGIN
   PRINTF, lun, format = '(%"ENVI \ndescription= {\n    %s [%s]}")', descrip, SYSTIME()
ENDELSE
;;
PRINTF, lun, format = '(%"samples = %i")', ns
PRINTF, lun, format = '(%"lines = %i")', nl
PRINTF, lun, format = '(%"bands = %i")', nb
PRINTF, lun, format = '(%"data type = %i")', dtype
;;
IF KEYWORD_SET(compress) THEN BEGIN 
   cmpr = 1
   PRINTF, lun, format = '(%"file compression = %i")', cmpr
ENDIF
;;
CASE interleave OF
   0: intleave = 'bsq'
   1: intleave = 'bil'
   2: intleave = 'bip'
ENDCASE
;;
PRINTF, lun, format='(%"interleave = %s")', intleave
PRINTF, lun, format='(%"byte order = %i")', byte_order
;;
IF N_ELEMENTS(bnames) EQ 0 THEN BEGIN
   ;;
   band=STRING(LONARR(nb))
   number=STRING(LONARR(nb))
   bnames=STRING(LONARR(nb))
   ;;
   ;;create the array with value 'Band' placed in each element
   FOR i=0L, nb-1 DO BEGIN
      band[i]= 'Band '
   ENDFOR
   ;;
   ;; create the array with values of 1 to the total number of files
   FOR i=0L, nb-1 DO BEGIN
      number[i]= STRTRIM(i+1,1)
   ENDFOR
   ;;
   ;; concatenate (join) the band and number arrays into one singular array
   FOR i=0L, nb-1 DO BEGIN
      bnames[i]= band[i] + number[i]
   ENDFOR
   ;;
ENDIF
;
PRINTF, lun, "band names = {"
;
IF nb LE 2 THEN BEGIN
   IF nb EQ 1 THEN BEGIN
      PRINTF, lun, format='(%"%s}")', bnames
   ENDIF ELSE BEGIN
      PRINTF, lun, format='(%"%s,")', bnames[0]
      PRINTF, lun, format='(%"%s}")', bnames[1]
   ENDELSE
ENDIF ELSE BEGIN
   FOR i=0, nb-2 DO BEGIN
      PRINTF, lun, format='(%"%s,")', bnames[i]
   ENDFOR
   PRINTF, lun, format='(%"%s}")', bnames[i]
ENDELSE
;
FREE_LUN, lun
;
; Write the image file
;
IF KEYWORD_SET(compress) then begin
   OPENW, lun, outname, /get_lun, /compress
ENDIF ELSE BEGIN 
   OPENW, lun, outname, /get_lun
ENDELSE
WRITEU, lun, data
FREE_LUN, lun
;
if KEYWORD_SET(test) then STOP
;
END  
