;+
; NAME: RESTORE
;
; PURPOSE:
;       Serves as a wrapper around CMRESTORE from Craig B. Markwardt
;       CMVSLIB library.
;
; MODIFICATION HISTORY:
;   01-Sep-2006 : written by Joel Gales
;
; LICENCE:
; Copyright (C) 2006, J. Gales
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-

pro restore,     filename0,verbose=verbose

if (n_elements(filename0) ne 0) then begin
    sz = size(filename0)
    if (sz[sz[0]+1] ne 7) then begin
        print, "% RESTORE: Expression must be a scalar or 1 element array in this context: " + routine_names(variables=-1)
        return
    endif
endif

cmrestore,       filename0, $
                 p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,  p8,  p9, $
                 p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, $
                 p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, $
                 names=_nme_

for i=0,n_elements(_nme_)-1 do begin
    p = strcompress('p' + string(i), /remove_all)
    parm = 'dummy=routine_names(_nme_[i],' + p + ',store=-1)
    stat = execute(parm)
endfor

return
end
