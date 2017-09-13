;+
; NAME:
;   ERRPLOT
;
; PURPOSE:
;   Plot error bars over a previously drawn plot.
;
; CALLING SEQUENCE:
;   ERRPLOT, [X], Low, High, WIDTH=value 
;
; INPUTS:
;   X   :  vector of X (abcissa) values of Y ( the data) (optional).
;   Low :  vector of Y-error_min
;   High:  vector of Y+error_max
;
; KEYWORD Parameters:
;   WIDTH:  Error bar width.
;           is in units of the width of the plot. (default: 1%)
;
; EXAMPLES:
;   Symmetrical error bars of value err
;       PLOT, Y
;       ERRPLOT, Y-ERR, Y+ERR
;
;-
PRO ERRPLOT, XVAL, LOW, HIGH, WIDTH = WIDTH, COLOR=COLOR, _EXTRA=_EXTRA

  compile_opt hidden,idl2
  
  IF N_PARAMS() EQ 0 then message,"Variable is undefined: X." 
  
  npar = N_Params()
  if npar eq 1 then message, 'Variable is undefined: LOW'
  IF (npar EQ 2) THEN BEGIN
     HIGH = LOW
     LOW = XVAL
     xval = findgen(n_elements(low))
  ENDIF
  npts = N_Elements(HIGH) < N_Elements(LOW) < N_Elements(XVAL)
  ; compute width 
  winsize=(!X.WINDOW[1] - !X.WINDOW[0]) * !D.X_SIZE / 2
  IF N_Elements(width) EQ 0 THEN width = 1E-2*winsize ELSE width*=winsize/100.

  ;clip issue: normally errobars are clipped
  noclip = (n_elements(noclip) gt 0) ? noclip : !P.NOCLIP

  FOR i=0,npts-1 DO BEGIN           
     xy0 = convert_coord(xval[i], low[i], /DATA, /TO_DEVICE)
     xy1 = convert_coord(xval[i], high[i], /DATA, /TO_DEVICE)
     plots, [xy0[0] + [-width, width,0], xy1[0] + [0, -width, width]], [replicate(xy0[1],3), replicate(xy1[1],3)], /DEVICE, NOCLIP=noclip, COLOR=COLOR, _extra=extra
  ENDFOR

END

