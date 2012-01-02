pro AppleMan,xRange,yRange,RESULT=res,XSIZE=xS,YSIZE=yS,$
              NoDisplay=NoDisplay, test=test, help=help
  on_error, 2
;+
;
; NAME:     appleman
;
; PURPOSE:  Generate a GDL window showing the mandelbrot set.
;
; CATEGORY: Demo
;
; CALLING SEQUENCE: appleman,[xRange,yRange,RESULT=result] 
;
; KEYWORD PARAMETERS: 
;     RESULT : the resulting byte array for further porcessing
;
; COMMON BLOCKS: 
;
; SIDE EFFECTS:
;
; RESTRICTIONS:
;
; PROCEDURE: convergence or divergence of modulus of complex number
;            after a given number of iterations
;
; EXAMPLE:
;
; MODIFICATION HISTORY:
; 	Written by: Marc Schellens 2004-12-01
;       2011-12-29: Alain : few extra keywords
;
;-
; LICENCE:
; Copyright (C) 2004, 2011
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-

;device,PSEUDO_COLOR=8,DECOMPOSED=0

if KEYWORD_SET(help) then begin
   print, 'pro AppleMan, xRange, yRange, RESULT=res, $'
   print, '              NoDisplay=NoDisplay, test=test, help=help'
   return
endif
;
if N_ELEMENTS(xRange) eq 0 then xRange = [ -1.0, 2.3]
if N_ELEMENTS(yRange) eq 0 then yRange = [ -1.3, 1.3]
;
iter = 255

if ~keyword_set(xS) then xS = 640
if ~keyword_set(yS) then yS = 512

xD = float(xRange[1]-xRange[0])
yD = float(yRange[1]-yRange[0])

xStep = xD / xS
yStep = yD / yS
;
xStartVec = LINDGEN( xS) * xStep + xRange[0]
yStartVec = LINDGEN( yS) * yStep + yRange[0]

constArr = COMPLEX(REBIN(xStartVec, xS, yS),$
                   REBIN(TRANSPOSE(yStartVec), xS, yS))

valArr = COMPLEXARR( xS, yS)

res = INTARR( xS, yS)

oriIndex = LINDGEN( LONG(xS) * yS)

for i = 0, iter-1 do begin
   ;;
   valArr = valArr^2 - constArr
   ;;
   whereIn = WHERE((ABS(valArr) LE 4.0d), COMPLEMENT=whereOut)
   ;;
   if whereIn[0] eq -1 then break
   ;;
   valArr = valArr[ whereIn]
   constArr = constArr[ whereIn]
   ;;
   if whereOut[0] ne -1 then begin
      res[ oriIndex[ whereOut]] = i+1      
      oriIndex = oriIndex[ whereIn]
   endif
endfor

if ~arg_present(res) || ~KEYWORD_SET(NoDisplay) then begin
  loadct,15
  tv,res
endif

if KEYWORD_SET(test) then STOP
;
end
