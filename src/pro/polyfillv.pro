FUNCTION polyfillv,x,y,sx,sy,runlength=run_length
;+
;
; NAME:     polyfillv
;
; MODIFICATION HISTORY:
; 	Written by: Reto Stockli <reto.stockli@gmail.com>
;
;-
; LICENCE:
; Copyright (C) 2004, 2011
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-

  ;; reprogrammed after
  ;; http://alienryderflex.com/polygon_fill/
  ;; public-domain code by Darel Rex Finley, 2007
  ;; by 
  ;; the implementation differs slightly from the IDL one

  ;; determine size of polygon
  polycorners = n_elements(x)

  ;; check arguments
  IF polycorners LT 3 THEN BEGIN
     print,"POLYFILLV: Not enough valid and unique points specified."
     stop
  ENDIF

  IF polycorners NE n_elements(y) THEN BEGIN
     print,"POLYFILLV: X and Y vectors not same size"
     stop
  ENDIF
  
  IF (sx LE 0L) OR (sy LE 0L) THEN BEGIN
     print,"POLYFILLV: Array dimensions must be greater than 0."
     stop
  ENDIF

  IF keyword_set(run_length) THEN BEGIN
     print,"The Keyword Run_Length is not yet implemented."
     stop
  ENDIF

  ;; initialize output array
  idx = -1L

  fx = float(x)
  fy = float(y)

  ;;  Loop through the rows of the image.
  FOR py=0L,sy-1L DO BEGIN
    
     fpy = float(py)
     
     n = 0L

     first = 1B
     nodex = 0.0
     j = polycorners-1L
     FOR i=0L,polycorners-1L DO BEGIN
        IF ((fy[i] LE fpy) AND (fy[j] GT fpy)) OR $
           ((fy[j] LE fpy) AND (fy[i] GT fpy)) THEN BEGIN
           fpx = fx[i] + (fpy-fy[i])/(fy[j]-fy[i])*(fx[j]-fx[i])
           px = fix(fpx)
;;           print,i,px,py,x[i],y[i],x[j],y[j],fpx
           IF first THEN BEGIN
              nodex = px 
              first = 0B
           ENDIF ELSE BEGIN
              nodex = [nodex,px]
           ENDELSE
           n += 1L
        ENDIF
        j=i
     ENDFOR

;;     print,"Line: ",py," Nodes: ",n
 
     nodes = n_elements(nodex)
     IF nodes GE 2L THEN BEGIN

        ;;  Sort the nodes
        nodex = nodex[sort(nodex)]

        ;;  Fill the pixels between node pairs.
        FOR i=0L,nodes-1L,2 DO BEGIN
           IF (nodex[i+1] GE 0) THEN BEGIN
              IF (nodex[i] LT 0) THEN nodex[i] = 0  ;; reset to left edge of raster
              IF (nodex[i+1] GT sx) THEN nodex[i+1] = sx  ;; reset to right edge of raster
              numx=nodex[i+1] - nodex[i]
              IF numx GT 0L THEN BEGIN
                 valx=(lindgen(numx) + nodex[i]) + py * sx
                 IF idx[0] EQ -1L THEN idx = valx ELSE idx = [idx,valx]
              ENDIF
           ENDIF
        ENDFOR

     ENDIF

  ENDFOR

  RETURN,idx

END
