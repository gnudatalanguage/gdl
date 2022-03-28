Function expandrl,idx
  compile_opt hidden, idl2
  n=n_elements(idx)
  if (n lt 2) then return,-1
  k=[0:n-1:2]
  sz=total(idx[k])
  val=lonarr(sz)
  k=0L
  for i=0,n-1,2 do begin
     j=idx[i]
     val[k:k+j-1]=lindgen(j)+idx[i+1]
     k+=j
  endfor
  return, val
end

FUNCTION polyfillv,x,y,sx,sy,run_length
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
compile_opt hidden, idl2
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

  runl=(n_elements(run_length) gt 0 && run_length[0] gt 0)

  ;; initialize output array
  idx = -1L

  fx = long(x)
  fy = long(y)

  ;;  Loop through the rows of the image.
  FOR py=0L,sy-1L DO BEGIN
    
     fpy = float(py)+0.5
     nodex=lonarr(polycorners+1)
    
     nodes = 0L

     first = 1B
     j = polycorners-1L
     FOR i=0L,polycorners-1L DO BEGIN
        IF (((fy[i] LE fpy) && (fy[j] GT fpy)) || ((fy[j] LE fpy) && (fy[i] GT fpy))) THEN BEGIN
           nodex[nodes++] = ceil(fx[i] -0.5 + ((fpy-fy[i])/(fy[j]-fy[i])*(fx[j]-fx[i]))) ; [3.25,3.5,3.75] -> [3,3,4] which is not easy.
;           print,nodes-1,fy[i],fy[j],fpy,nodex[nodes-1]
        endif
        
        j=i
     ENDFOR
 
     IF nodes GE 2L THEN BEGIN

        ;;  Sort the nodes
        nodex = nodex[0:nodes-1]
        nodex=nodex[sort(nodex)]
 ;       print,"Line: ",py," Nodes: ",nodes,":",nodex

        ;;  Fill the pixels between node pairs.
        FOR i=0L,nodes-1L,2 DO BEGIN
 ;          print,nodex[i+1], nodex[i]
           if (nodex[i] GE sx) THEN BREAK
           IF (nodex[i+1] GE 0) THEN BEGIN
              IF (nodex[i] LT 0) THEN nodex[i] = 0  ;; reset to left edge of raster
              IF (nodex[i+1] GT sx) THEN nodex[i+1] = sx  ;; reset to right edge of raster
              numx=nodex[i+1] - nodex[i]
;              print,numx,nodex[i], nodex[i+1]
              IF numx GT 0L THEN BEGIN
                 valx=[numx,nodex[i] + py * sx]
                 IF idx[0] EQ -1L THEN idx = valx ELSE idx = [idx,valx]
              ENDIF
           ENDIF
        ENDFOR

     ENDIF

  ENDFOR
  if (idx[0] eq -1) then message,/inf,"No points in polygon"
  if (runl) then RETURN,idx
  return, expandrl(idx)
END
