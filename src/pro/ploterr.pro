;+
; NAME:       ploterr
;
; PURPOSE:    plot points with (symetrical) error bars
;
; This is a fully compatible procedure to the original one provide by
; IDL. This version contains several useful extensions (/hat,
; x_errors, _extra ...)
;
; CATEGORY:   plotting
;
; CALLING SEQUENCE: ploterr [,x], y, y_error [,x_error][,psym=psym][,type=type]
;
; INPUTS:             Y       (mandatory)
;                     y_error (mandatory)
;
; OPTIONAL INPUTS:    x       (optional)
;                     x_error (optional)
;
; Inititals KEYWORD PARAMETERS (compatibles with the IDL version of PLOTERR)
;            psym   (default : 7)
;            type   (0 lin/lin, 1 log/lin, 2 lin/log 3 log/log)
;
; Extended KEYWORD PARAMETERS (specific to this version)
; for the errors bars :
;          hat           <-- /hat adds a small line at error bar ends
;          length_of_hat <-- 1 or 2 positives values
;          bar_color     <-- we can plot the bars with a different color 
; for the plot :
;            xrange        <-- explicit use of !X.range
;            yrange        <-- explicit use of !X.range
;            xlog          <-- alternative to Type Key
;            ylog          <-- alternative to Type Key
;            _extra        <-- allow to provide paramters to PLOT
; for the procedure :
;            help          <-- return list of keywords
;            test          <-- for debugging purpose
;
; OUTPUTS:  none
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS:   none
;
; SIDE EFFECTS:    none (but some PLOT variables may changed)
;
; RESTRICTIONS:  - if sizes are differents, smaller size is used
;                - if not enough points, no plot
;                - arrays cannot be of type string
;                - we convert the rrors to ABS(error)
;   - take care that:
;       -- if 2 vectors (in this order): Y, Yerrors
;       -- if 3 vectors (in this order): X, Y, Yerrors
;       -- if 4 vectors (in this order): X, Y, Yerrors, Xerrors
;
; PROCEDURE: - checks the number of input vectors
;            - plot the data
;            - oplot the errors
;
; EXAMPLE:  see test_ploterr.pro
;
; nbp=10 & y=REPLICATE(1.,nbp) & yerr=RANDOMN(seed,10) & x=10+findgen(10)*2.
;
; ploterr, y, yerr
; ploterr, y, yerr, /hat
; ploterr, x, y, yerr
; ploterr, x, y, yerr, yerr/3., /hat
;
; MODIFICATION HISTORY:
;   - 26/02/2006 created by Alain Coulais (ARSC)
;
;-
; LICENCE:
; Copyright (C) 2006, Alain Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
;
pro PLOTERR, x, y, y_error, x_error, psym=psym, type=type, $
             xrange=xrange, yrange=yrange, xlog=xlog, ylog=ylog, $
             hat=hat, length_of_hat=length_of_hat, bar_color=bar_color, $ 
             _extra=_extra, help=help, test=test
;
ON_ERROR,2
;
if KEYWORD_SET(help) then begin
    print, 'pro PLOTERR, x, y, y_error, x_error, psym=psym, type=type, $'
    print, '             xrange=xrange, yrange=yrange, xlog=xlog, ylog=ylog, $'
    print, '             hat=hat, length_of_hat=length_of_hat, bar_color=bar_color, $'
    print, '             _extra=_extra, help=help, test=test'
    return
endif
;
; we have some prefered default
;
if (N_ELEMENTS(type) EQ 0) then type = 0
if (N_ELEMENTS(psym) eq 0) then psym = 7
;
; only "y" and "err" are mandatory
;
nb_inputs=N_PARAMS(0)
;
if (nb_inputs LT 2) then begin
    mess='Must be called with 2-5 parameters: '
    mess=mess+'[X,] Y, Y_ERR [,X_ERR] [,PSYM [,TYPE]] ...'
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
    nbp_ex=nbp_ey
    ;; we have to generate a X vector
    x_new=FINDGEN(nbp_x)
endif    
;
; We have X, Y and Y_error
;
if (nb_inputs EQ 3) then begin
    x_new=x
    y_new=y
    y_err=ABS(y_error)
    nbp_x=N_ELEMENTS(x_new)
    nbp_y=N_ELEMENTS(y_new)
    nbp_ey=N_ELEMENTS(y_err)
    ;; only one missing field
    nbp_ex=nbp_ey
endif
;
; a priori without X_error
flag_x=0

; We have the 4 info:  X, Y, Y_error and X_error
;
if (nb_inputs EQ 4) then begin
    ;; if we have X_error, we switch on the flag
    flag_x=1 
    x_new=x
    y_new=y
    y_err=ABS(y_error)
    x_err=ABS(x_error)
    nbp_x=N_ELEMENTS(x_new)
    nbp_y=N_ELEMENTS(y_new)
    nbp_ey=N_ELEMENTS(y_err)
    nbp_ex=N_ELEMENTS(x_err)
endif
;
; If we may would like to check pre-processing ...
;
if KEYWORD_SET(test) then STOP
;
nbp_min=MIN([nbp_x,nbp_y,nbp_ey,nbp_ex])
if (nbp_min LT 2) then message, 'Not enough points to plot.'
if (nbp_min GT 100) then print, 'Warning: please wait until end of long rendering'
;
; we limit the range for all array up to "nbp_min"
;
if (nbp_x  GT nbp_min) then x_new=x_new[0:nbp_min-1]
if (nbp_y  GT nbp_min) then y_new=y_new[0:nbp_min-1]
if (nbp_ey GT nbp_min) then y_err=y_err[0:nbp_min-1]
;
; we need 2 arrays for the top and the bottom of Errors
;
y_low=y_new-y_err
y_hig=y_new+y_err
;
; Eventually, we have also 2 arrays for X-errors
if (flag_x EQ 1) then begin
    if (nbp_ex GT nbp_min) then x_err=x_err[0:nbp_min-1]
    x_low=x_new-x_err
    x_hig=x_new+x_err
endif
;
; ---------------------
; managment of plot type
; if !{x|y}.type EQ 0 --> Lin
; if !{x|y}.type EQ 1 --> Log
;
; since GDL does not have the "xtype" and "ytype" keywords for PLOT
; As it was for GDL 0.8.11, we use instead xlog and ylog !
;
if (N_ELEMENTS(type) EQ 1) then begin
    if (type GT 0) then begin
        xlog = type/2
        ylog = type and 1
    endif
endif
;
; Do we have a pre-set !y.range ?
;
if (N_ELEMENTS(yrange) NE 2) then begin
    if (!y.range[0] EQ !y.range[1]) then begin
        yrange=[MIN(y_low), MAX(y_hig)]
    endif else begin
        yrange=!y.range
    endelse
endif
;
; Do we have a pre-set !y.range ?
;
if (N_ELEMENTS(xrange) NE 2) then begin
    if (!x.range[0] EQ !x.range[1]) then begin
        if (flag_x EQ 1) then begin
            xrange=[MIN(x_low), MAX(x_hig)]
        endif else begin
            xrange=[MIN(x_new),MAX(x_new)]
        endelse
    endif else begin
        xrange=!x.range
    endelse
endif
;
; we now do the plot of the data themselves !
;
PLOT, x_new, y_new, xlog=xlog, ylog=ylog, $
  xrange=xrange, yrange=yrange, psym=psym, _extra=_extra
;
; shall we switch to another color ?
;
if (N_ELEMENTS(bar_color) EQ 1) then begin
    ref_color=!p.color
    !p.color=bar_color
endif
;
; we overplot the error bars
;
; begin of basic PLOTERR feature (only on Y axis ...)
;
for i=0,(nbp_min-1) do PLOTS,[x_new[i], x_new[i]], [y_low[i], y_hig[i]]
;
; end of basic PLOTERR feature
; begin og extra PLOTERR features !
;
if (flag_x EQ 1) then begin
    for i=0,(nbp_min-1) do PLOTS,[x_low[i], x_hig[i]], [y_new[i], y_new[i]]
endif 
;
if KEYWORD_SET(hat) then begin
    ;;
    ;; we have to manage the length of the hat (Keyword length_of_hat)
    ;; we compute first a default and switch off a flag
    ;;
    x_half_def=(!X.crange[1]-!X.crange[0])/100.
    y_half_def=(!Y.crange[1]-!Y.crange[0])/100. ;; useful only if x_err ...
    flag_length_hat=0
    ;;
    ;; the "length_of_hat" is the FULL length --> /2.
    ;;
    if (N_ELEMENTS(length_of_hat) EQ 1) then begin
        if (length_of_hat GT 0.) then begin
            flag_length_hat=1
            x_half=length_of_hat/2.
            y_half=x_half  ;; useful only if x_err ...
        endif
    endif
    if (N_ELEMENTS(length_of_hat) EQ 2) then begin
        if ((length_of_hat[0] GT 0.) AND (length_of_hat[1] GT 0.)) then begin
            flag_length_hat=1
            x_half=length_of_hat[0]/2.
            y_half=length_of_hat[1]/2.  ;; useful only if x_err ...
        endif
    endif
    ;;
    ;; what is the state of the flag ?
    ;;
    if (flag_length_hat EQ 0) then begin
        x_half=x_half_def
        y_half=y_half_def
    endif
    ;;
    ;; Now, since the length of the hat is known, we plot
    ;;
    ;; first we plot the Horizontal hats of the Vertical bars
    ;;
    x_hatlow=x_new-x_half
    x_hathig=x_new+x_half
    for i=0,(nbp_min-1) do begin
        PLOTS,[x_hatlow[i], x_hathig[i]], [y_low[i], y_low[i]]
        PLOTS,[x_hatlow[i], x_hathig[i]], [y_hig[i], y_hig[i]]
    endfor
    ;;
    ;; second we plot the Vertical hats of the Horizontal bars
    ;;
    if (flag_x EQ 1) then begin
        y_hatlow=y_new-y_half
        y_hathig=y_new+y_half
        for i=0,(nbp_min-1) do begin
            PLOTS,[x_low[i], x_low[i]], [y_hatlow[i], y_hathig[i]]
            PLOTS,[x_hig[i], x_hig[i]], [y_hatlow[i], y_hathig[i]]
        endfor
    endif
endif
;
if (N_ELEMENTS(bar_color) EQ 1) then !p.color=ref_color
;
if KEYWORD_SET(test) then STOP
;
end
;
