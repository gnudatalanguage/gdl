;+
; NAME:       oploterr
;
; PURPOSE:    oplot points with (symetrical) error bars
;
; CATEGORY:   plotting
;
; CALLING SEQUENCE: oploterr [,x], y, y_error,[psym]
;
; INPUTS:             Y       (mandatory)
;                     y_error (mandatory)
;
; OPTIONAL INPUTS:    x       (optional)
;                     psym    (optional, default : 7)
;
; for the procedure :
;            help          <-- return list of keywords
;
; OUTPUTS:  none
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS:   none
;
; SIDE EFFECTS:    none ?
;
; RESTRICTIONS:  - if sizes are differents, smaller size is used
;                - if not enough points, no plot
;                - arrays cannot be of type string
;                - we convert the rrors to ABS(error)
;   - take care that:
;       -- if 2 vectors (in this order): Y, Yerrors
;       -- if 3 vectors (in this order): X, Y, Yerrors
;
; PROCEDURE: - checks the number of input vectors
;            - oplot the errors
;
; EXAMPLE:  see test_oploterr.pro
;
; nbp=10 & y=REPLICATE(1.,nbp) & yerr=RANDOMN(seed,10) & x=10+findgen(10)*2.
;
; plot, y, yerr
; oploterr, y, yerr
; plot, x, y
; oploterr, x, y, yerr
;
; MODIFICATION HISTORY:
;   - 24/07/2013 created by GD as an edited version of PLOTERR by AC
;
;-
; LICENCE:
; Copyright (C) 2013, Alain Coulais, Gilles Duvert
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
;
pro OPLOTERR, x, y, y_error, psym, help=help
;
ON_ERROR,2
;
if KEYWORD_SET(help) then begin
    print, 'pro OPLOTERR, [x,] y, y_error, [psym]'
    return
endif
;
;
; only "y" and "err" are mandatory
;
nb_inputs=N_PARAMS(0)
;
if (nb_inputs LT 2 or nb_inputs GT 4) then begin
    mess='Must be called with 2-4 parameters: '
    mess=mess+'[X,] Y, Y_ERR [,PSYM] ...'
    message, mess
    return
endif
;
; Here, we have ONLY Y and Y_error
;
if (nb_inputs EQ 2) then begin
    y_new=x
    y_err=y
    nbp_y=N_ELEMENTS(y_new)
    nbp_ey=N_ELEMENTS(y_err)
    ;; the 2 missing fields
    nbp_x=MIN([nbp_y, nbp_ey])
    ;; we have to generate a X vector
    x_new=DINDGEN(nbp_x)
endif    
;
; We have X, Y and Y_error or Y,Yerror and psym...
;
if (nb_inputs EQ 3) then begin
    if (n_elements(y_error) lt 2) then begin ; Y,Yerror and psym
    y_new=x
    x_new=dindgen(N_ELEMENTS(y_new))
    y_err=ABS(y)
    nbp_x=N_ELEMENTS(x_new)
    nbp_y=N_ELEMENTS(y_new)
    nbp_ey=N_ELEMENTS(y_err)
    mypsym=y_error ; psym
    endif else begin
    x_new=x
    y_new=y
    y_err=ABS(y_error)
    nbp_x=N_ELEMENTS(x_new)
    nbp_y=N_ELEMENTS(y_new)
    nbp_ey=N_ELEMENTS(y_err)
    mypsym=7
    endelse
endif
if (nb_inputs EQ 4) then  begin
    x_new=x
    y_new=y
    y_err=ABS(y_error)
    nbp_x=N_ELEMENTS(x_new)
    nbp_y=N_ELEMENTS(y_new)
    nbp_ey=N_ELEMENTS(y_err)
    mypsym=psym
endif
;
nbp_min=MIN([nbp_x,nbp_y,nbp_ey])
if (nbp_min LT 2) then message, 'Not enough points to plot.'
;
; we limit the range for all array up to "nbp_min"
;
if (nbp_x  GT nbp_min) then x_new=x_new[0:nbp_min-1]
if (nbp_y  GT nbp_min) then y_new=y_new[0:nbp_min-1]
if (nbp_ey GT nbp_min) then y_err=y_err[0:nbp_min-1]
;
; oplot the values with psym
oplot,x_new,y_new,psym=mypsym
; we need 2 arrays for the top and the bottom of Errors
;
y_low=y_new-y_err
y_hig=y_new+y_err
; use NaN with PLOTS to go fast!
null=replicate(!values.d_nan,nbp_min)
x_new=reform(transpose([[x_new],[x_new],[null]]),3*nbp_min)
y_new=reform(transpose([[y_low],[y_hig],[null]]),3*nbp_min)
; overplot the error bars
;
plots,x_new,y_new,noclip=0
;
end
;
