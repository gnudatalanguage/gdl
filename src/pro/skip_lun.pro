;+
; NAME:      SKIP_LUN
;
; PURPOSE:   skip N lines or N bytes in a opened file
;
; CATEGORY:  I/O
;
; CALLING SEQUENCE:
;  
;        SKIP_LUN, FromUnit [, Num] [, /EOF] [, /LINES] [, /TRANSFER_COUNT=variable]
;
; INPUTS: -- Unit: the logical unit of the file
;
; OPTIONAL INPUTS: -- num: the number of lines or bytes to be skipped.
;                  If not provided, go to EOF().
;
; KEYWORD PARAMETERS: -- /EOF
;                     -- /lines
;                     -- /TRANSFER_COUNT
;
; Extra Parameters:   -- /test : will stop at end to help finding problems
;                     -- /help : print a small help and return (doing nothing)
;
; OUTPUTS:          none
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS:    none
;
; SIDE EFFECTS:     May-be the Byte case is not well done when EOF()
;                   may be reached (Don't know how to do)
;
; RESTRICTIONS:     should not be used on compress files
;
; PROCEDURE: 
; - with /line keyword set:
;   -- if line numbers negative, return 0
;   -- if keyword EOF not set and EOF reached before line number, return 0
;
; EXAMPLE:          SKIP_LUN, nlun, 8, /lines
;
; MODIFICATION HISTORY:
;   18-May-2007 : written by Alain Coulais (AC)
;                 because SKIP_LUN is used in Astron since V 5.6
;   01-April-2008 : improvments
; 
; LICENCE:
; Copyright (C) 2007, Alain Coulais
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
pro SKIP_LUN, LunNumber, skip_value, eof=eof, lines=lines, $
              transfer_count=transfer_count, help=help, test=test
;
on_error,2
;
if KEYWORD_SET(help) then begin
   print, 'pro SKIP_LUN, LunNumber, skip_value, eof=eof, lines=lines, $'
   print, '              transfer_count=transfer_count, help=help, test=test'
   return
endif
;
; checking the consistencies of input parameters
;
if (N_ELEMENTS(LunNumber) EQ 0) then begin
   MESSAGE,'SKIP_LUN: Variable is undefined:'+STRING(LunNumber)
endif
if (N_ELEMENTS(LunNumber) GT 1) then begin
   MESSAGE,'SKIP_LUN: Expression must be a scalar or 1 element array in this context:'+STRING(LunNumber)
endif
;
if (N_PARAMS() EQ 1) then eof=1
;
if (N_PARAMS() EQ 2 ) then begin
   if (N_ELEMENTS(skip_value) EQ 0) then begin
      MESSAGE,'SKIP_LUN: Variable is undefined: '+STRING(skip_value)
   endif
   if (N_ELEMENTS(skip_value) GT 1) then begin
      MESSAGE,'SKIP_LUN: Expression must be a scalar or 1 element array in this context: '+STRING(skip_value)
   endif
endif
;
; is this file really opened ?
;
fstat_info=FSTAT(LunNumber)
;
if (fstat_info.open NE 1) then begin
   MESSAGE,' File unit is not open: '+STRING(LunNumber)
endif
;
; internal debug flag
;debug=1
;
void_line=''
;
if KEYWORD_SET(lines) then begin
   i=0L
   if KEYWORD_SET(eof) then begin
      while NOT(EOF(LunNumber)) do begin
         i=i+1L
         readf, LunNumber, void_line
      endwhile
      transfer_count=i	
   endif else begin
      if (skip_value LT 0) then begin
         transfer_count=0
         return
      endif
      while NOT(EOF(LunNumber)) AND (i LT skip_value) do begin
         i=i+1L
         readf, LunNumber, void_line
      endwhile
      ;; we must return "Zero" if EOF encoutered
      ;;  before reaching the expected line
      if (i EQ skip_value) then begin
         transfer_count=i
      endif else begin
         transfer_count=0
         MESSAGE, '% SKIP_LUN: End of file encountered. Unit:'+STRING(LunNumber)+', File: '+fstat_info.name
      endelse
   endelse
endif else begin
   ;; Byte case
   ;; (I don't know how to manage EOF() in this case ...)
   print, 'Please contribute to this branch if needed'
   POINT_LUN, LunNumber, skip_value
endelse
;
if KEYWORD_SET(test) then STOP
;
end
