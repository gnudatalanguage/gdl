;+
;
; NAME:
;      LOADCT
;
; PURPOSE: 
;      Load a predefined color table
;
;
; CATEGORY:
;      graphics
;
; CALLING SEQUENCE:
;      LOADCT,[table_number]
;
;
; KEYWORD PARAMETERS: 
;       SILENT    if set lading colortable message is suppressed
;       GET_NAMES set to a named variable in which all the colortable
;                 names are returned 
;       FILE=file IDL compatibility only, no effect
;	NCOLORS   number of colors to use. the smaller of
;                 !D.TABLE_SIZE-1 and NCOLORS is used
;       BOTTOM    first color index to use
;       RGB_TABLE=mytable return table colors in mytable, do not load 
;                         colortable.
;
; OUTPUTS:
;	none
;
;
;
; OPTIONAL OUTPUTS:
;	keyword outputs, if any
;
;
; COMMON BLOCKS:
;	COLORS GDL colors common block
;
;
; SIDE EFFECTS:
;	color tables of current graphic device are changed
;
;
; PROCEDURE:
;       uses the internal library procedure LOADCT_INTERNALGDL
;
; MODIFICATION HISTORY:
; 	Written by:	Marc Schellens
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

pro LOADCT, table, GET_NAMES=names, FILE=file,$
            NCOLORS=mynColors,BOTTOM=mybottom,SILENT=silent,RGB_TABLE=rgb_table

on_error, 2
common colors, r_orig, g_orig, b_orig, r_curr, g_curr, b_curr

if keyword_set(file) then message, 'FILE keyword not implemented yet, ignored. (FIXME)', /conti

if N_ELEMENTS( table) eq 0 and ARG_PRESENT( names) then begin
  LOADCT_INTERNALGDL,GET_NAMES=names
  return
endif

if N_ELEMENTS( table) eq 0 or not KEYWORD_SET(silent) then begin 
    LOADCT_INTERNALGDL,GET_NAMES=names
    if n_elements( table) eq 0 then begin
        for n=0,n_elements(names)-1 do begin

            print,FORMAT="(I2,2X,A)",n,names[n]
        endfor
        read, table, PROMPT='Enter table number: '
    endif
endif

if KEYWORD_SET(RGB_TABLE) then begin
  LOADCT_INTERNALGDL,table,RGB_TABLE=rgb_table
  return
endif

LOADCT_INTERNALGDL,table

if not KEYWORD_SET( silent) then begin
   MESSAGE,'Loading table ' + names[table],/INFO
endif

if N_ELEMENTS(mybottom) eq 0 then bottom=0 else begin
 bottom=mybottom 
 bottom >= 0 & bottom <= !D.TABLE_SIZE-1
end
if N_ELEMENTS(mynColors) eq 0 then nColors=!D.TABLE_SIZE else begin
 nColors=mynColors
 nColors >= 1 & nColors <=!D.TABLE_SIZE
end
if (bottom+nColors GE !D.TABLE_SIZE) then nColors=!D.TABLE_SIZE-bottom

TVLCT,r,g,b,/GET

if N_ELEMENTS( r_orig) eq 0 then begin
    r_orig = bytarr( !D.TABLE_SIZE)
    g_orig = bytarr( !D.TABLE_SIZE)
    b_orig = bytarr( !D.TABLE_SIZE)
 endif

idx=Lindgen(nColors)*(!D.TABLE_SIZE-1)/(nColors-1)
r=r[idx]
g=g[idx]
b=b[idx]

r_orig[bottom] = r
g_orig[bottom] = g
b_orig[bottom] = b
r_curr = r_orig
g_curr = g_orig
b_curr = b_orig

TVLCT, r_curr, g_curr, b_curr

end

