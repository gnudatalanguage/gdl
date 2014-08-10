;+
;
; NAME:
;      LOADCT
;
; PURPOSE: 
;      Load a predefined color table
;
; CATEGORY:
;      graphics
;
; CALLING SEQUENCE:
;      LOADCT,[table_number], GET_NAMES=names, FILE=file,$
;             NCOLORS=mynColors, BOTTOM=mybottom, $
;             SILENT=silent, RGB_TABLE=rgb_table
;
; KEYWORD PARAMETERS: 
;       SILENT    if set lading colortable message is suppressed
;       GET_NAMES set to a named variable in which all the colortable
;                 names are returned 
;       FILE=file IDL compatibility only, no effect
;	NCOLORS   number of colors to use. the smaller of
;                 !D.table_size-1 and NCOLORS is used
;       BOTTOM    first color index to use
;       RGB_TABLE=mytable return table colors in mytable, do not load 
;                         colortable.
;
; OUTPUTS:
;	none
;
; OPTIONAL OUTPUTS:
;	keyword outputs, if any
;
; COMMON BLOCKS:
;	COLORS GDL colors common block
;
; SIDE EFFECTS:
;	color tables of current graphic device are changed
;
; PROCEDURE:
;       uses the internal library procedure LOADCT_INTERNALGDL
;
; MODIFICATION HISTORY:
;  -- Written by Marc Schellens in 2005
;
;  -- AC, 2014-Aug-10: now we get back RGB_TABLE !
;
;-
; LICENCE:
; Copyright (C) 2005, 2014
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-
;
pro LOADCT, table, GET_NAMES=names, FILE=file,$
            NCOLORS=mynColors, BOTTOM=mybottom, $
            SILENT=silent, RGB_TABLE=rgb_table

ON_ERROR, 2
;
common colors, r_orig, g_orig, b_orig, r_curr, g_curr, b_curr
;
if KEYWORD_SET(file) then $
   MESSAGE, 'FILE keyword not implemented yet, ignored. (FIXME)', /continue
;
if (N_ELEMENTS(table) eq 0) and ARG_PRESENT(names) then begin
   LOADCT_INTERNALGDL, GET_NAMES=names
   return
endif
;
if (N_ELEMENTS(table) eq 0) or not KEYWORD_SET(silent) then begin 
   LOADCT_INTERNALGDL, GET_NAMES=names
   if N_ELEMENTS(table) eq 0 then begin
      for n=0,N_ELEMENTS(names)-1 do begin
         print, FORMAT="(I2,2X,A)",n,names[n]
      endfor
      READ, table, PROMPT='Enter table number: '
   endif
endif
;
if ARG_PRESENT(RGB_TABLE) then begin
   LOADCT_INTERNALGDL, table, RGB_TABLE=rgb_table
   return
endif
;
LOADCT_INTERNALGDL, table
;
if not KEYWORD_SET( silent) then begin
   MESSAGE,'Loading table ' + names[table],/INFO
endif
;
if N_ELEMENTS(mybottom) eq 0 then bottom=0 else begin
   bottom=mybottom 
   bottom >= 0 & bottom <= !D.table_size-1
end
;
if N_ELEMENTS(mynColors) eq 0 then begin
   nColors=!D.table_size
endif else begin
   nColors=mynColors
   nColors >= 1 & nColors <=!D.table_size
end
;
if (bottom+nColors GE !D.table_size) then nColors=!D.table_size-bottom
;
TVLCT,r,g,b,/GET
;
if N_ELEMENTS( r_orig) eq 0 then begin
   r_orig = BYTARR(!D.table_size)
   g_orig = BYTARR(!D.table_size)
   b_orig = BYTARR(!D.table_size)
endif
;
idx=LINDGEN(nColors)*(!D.table_size-1)/(nColors-1)
;
r=r[idx]
g=g[idx]
b=b[idx]
;
r_orig[bottom] = r
g_orig[bottom] = g
b_orig[bottom] = b
;
r_curr = r_orig
g_curr = g_orig
b_curr = b_orig
;
TVLCT, r_curr, g_curr, b_curr
;
end

