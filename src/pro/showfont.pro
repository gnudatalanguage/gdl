;+
; NAME: 
;      SHOWFONT
;
; PURPOSE: 
;          Uses current graphics device to draw a map of characters
;          available in the font specified in argument
;
; CATEGORY: 
;          General 
;
; CALLING SEQUENCE:
;          showfont, num, 'title' ; table of font num entitled 'title'
;
; KEYWORD PARAMETERS: 
;          /encapsulated                ; ignored (just for compatibility)
;          /tt_font                     ; ignored (just for compatibility)
;          base = 16                    ; number of columns in the table 
;          beg = 32                     ; first character
;          fin = num eq 3 ? 255 : 127   ; last character
;
; OUTPUTS:
;          None.
;
; OPTIONAL OUTPUTS:
;          None.
;
; COMMON BLOCKS:
;          None.
;
; SIDE EFFECTS:
;          Draws a font table on the current graphic device.
;
; RESTRICTIONS:
;          None.
;
; PROCEDURE:
;
; EXAMPLE:
;          showfont, 9, 'GDL math symbols'   ; show mappings for font 9
;
; MODIFICATION HISTORY:
; 	Written by: Sylwester Arabas (2008/12/28)
;-
; LICENCE:
; Copyright (C) 2008,
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;-

pro showfont, num, name, encapsulated=eps, tt_font=tt, base=base, beg=beg, fin=fin
  
  ; handling default keyword values
  if not keyword_set(base) then base = 16
  if not keyword_set(beg) then beg = 32
  if not keyword_set(fin) then fin = num eq 3 ? 255 : 127
  if not keyword_set(name) then name = ''

  ; constructing horizontal and vertical grid lines
  n_hor = (fin + 1 - beg) / base + 1
  h_x = (double(rebin(base * byte(128 * indgen(2 * (n_hor))) / 128, 4 * n_hor, /sample)))[1:4 * n_hor - 1] - .5
  h_y = (double(rebin(beg + indgen(n_hor) * base, 4 * n_hor, /sample)))[0:4 * n_hor - 2] - base/2.
  v_x = base - indgen(4 * base - 1) / 4 - .5
  v_y = (double(rebin(byte(128 * indgen(2 * (base))) / 128, 4 * base, /sample)))[1:4 * base - 1] $
    * base * ((fin + 1 - beg) / base) + beg - base / 2.

  ; ploting grid and title
  plot,  [h_x, v_x], [h_y, v_y], $
     title='Font ' + strtrim(string(num), 2) + ', ' + name, $
     xrange=[-1, base], $
     yrange=[base * ((fin + 1) / base), beg - base], $
     yticks=n_hor, $
     xticks=base+1, $
     xtitle='char mod ' + strtrim(string(base), 2), $
     ytitle=strtrim(string(base), 2) + ' * (char / ' + strtrim(string(base), 2) + ')'
     
  ; ploting characters
  for c = beg, fin do $
    xyouts, (c mod base), base * (c / base), '!' + strtrim(string(num), 2) + string(byte(c))

end
