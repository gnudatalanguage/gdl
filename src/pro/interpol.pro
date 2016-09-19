;
; under GNU GPL v2 or later
;
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; relies on findex.pro by Paul Ricchiazzi
;
; revised 27-Feb-2012 by Alain C. after bug report 3495104
; We have to manage also points in "p2" outside "p1" range ...
; (new cases not include in testsuite/test_interpol.pro)
;
; revised 18-Feb-2013 by Alain C. after bug report 3602770
; We have to manage NaN and Infinity ...
;
; resived 29-Oct-2013 by Alain C. thanks to Gael mixing
; of Double/Long64 types in HFI Planck Monte Carlo pipeline.
;
function INTERPOL, p0, p1, p2, lsquadratic=lsquadratic, $
                   quadratic=quadratic, spline=spline, $
                   test=test, help=help, debug=debug
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'function INTERPOL, p0, p1, p2, lsquadratic=lsquadratic, $'
    print, '                   quadratic=quadratic, spline=spline, $'
    print, '                   test=test, help=help, debug=debug'
    print, '/lsquadratic and /quadratic not available, help welcome !'
    return, -1
endif
;
; input type sanity checks
;
; type of return output comes from "p0" type
;
p0_type=SIZE(p0, /type)
if ((p0_type EQ 7) OR (p0_type EQ 8) OR (p0_type EQ 10)) then $
  MESSAGE, 'expression TYPE not allowed in this context: p0'
;
p1_type=SIZE(p1, /type)
if ((p1_type EQ 7) OR (p1_type EQ 8) OR (p1_type EQ 10)) then $
  MESSAGE, 'expression TYPE not allowed in this context: p1'
;
;; sanity checks
;
if N_PARAMS() eq 1 then $
  MESSAGE, 'Two or three parameters required'
if KEYWORD_SET(lsquadratic) then $
  MESSAGE, 'LSQUADRATIC keyword not supported yet (FIXME!)'
if KEYWORD_SET(quadratic) then $
  MESSAGE, 'QUADRATIC keyword not supported yet (FIXME!)'
;
;  if N_PARAMS() eq 3 and N_ELEMENTS(p0) ne N_ELEMENTS(p1) then $
;    MESSAGE, 'In the three-parameter case the first and second argument must be of equal length'
; <see bug no. 3104537>
;
if N_PARAMS() eq 3 then begin
    if N_ELEMENTS(p0) ne N_ELEMENTS(p1) then $
      MESSAGE, 'In the three-parameter case the first and second argument must be of equal length'
    ;;
    ;; note by AC, 27-02-2012: is it really true ??
    all_equal_test=ABS((p1 - SHIFT(p1,+1))[1:*])
    if MIN(TEMPORARY(all_equal_test)) eq 0 then begin
        MESSAGE, /cont, $  ; usually only triggered for integer arrays
          'In the three-parameter case, the second argument must be strictly increasing or strictly decreasing.'
    endif
endif
; </...>
;
isint = SIZE(p0, /type) lt 4 || SIZE(p0, /type) gt 11
;
; AC, 29-oct-2013, other "bad" types exited before
; Float is "4", Dbl is 5, Cplx 6, DCplx 9 (TBC)
;
if ((p0_type LT 4) OR (p0_type GT 11)) then p0_type=4
;
; AC 2012/03/05: useful values ... may be updated later
nbp_inside=N_ELEMENTS(p0)
nbp_outside=0
;
ExistNotFinite=0
;
if N_PARAMS() eq 2 then begin
    ;; regular grid case
    if SIZE(p1, /dimensions) eq 0 then begin
        ind = FINDGEN(p1) / (p1 - (p1 eq 1 ? 0 : 1)) * (N_ELEMENTS(p0) - 1)
    endif else begin
        MESSAGE, 'In the two-parameter case the second parameter must be a scalar'
        ;; TODO: IDL does something else here...
    endelse
endif else if ~KEYWORD_SET(spline) then begin
    ;; first, we exclude the NaN and Infinity values ...
    ;; if fact, we copy in another array the not finite values ...
    p2_info=SIZE(p2,/dim)
    index_p2_finite=WHERE(FINITE(p2) EQ 1, nbp_ok)
    if (nbp_ok GT 0) then begin
        if (N_ELEMENTS(p2) GT nbp_ok) then begin
            ExistNotFinite=1
            index_p2_not_finite=WHERE(FINITE(p2) EQ 0)
            p2_not_finite=p2[index_p2_not_finite]
            p2=p2[index_p2_finite]
        endif else begin
            ;; all data are finite ... we don't need to recopy
            ExistNotFinite=0
        endelse
    endif else begin
        ;; all input data are not finite ...
        if KEYWORD_SET(test) then STOP
        return, p2
    endelse
    ;; irregular grid case
    ;; we need to manage points outside p1 range
    p1_min=MIN(p1, max=p1_max)
    outside_OK=WHERE((p2 LT p1_min) OR (p2 GT p1_max), nbp_outside)
    if (nbp_outside GT 0) then begin
        outside=p2[outside_OK]
        inside_OK=WHERE((p2 GE p1_min) AND (p2 LE p1_max), nbp_inside)
        if (nbp_inside GT 0) then begin
            p2_inside=p2[inside_OK]
            ind = FINDEX(p1, p2_inside)
        endif      
    endif else begin
        ;; if we are here, all the points in "p2" are inside "p1" range
        ind=FINDEX(p1,p2)
    endelse
endif
;
if KEYWORD_SET(spline) then begin
   if (N_ELEMENTS(p0) LT 4) then MESSAGE, 'as least 4 input points need !'
   ;; spline case
   if N_PARAMS() eq 2 then begin
        x = FINDGEN(N_ELEMENTS(p0))
        y = SPL_INTERP(x, p0, SPL_INIT(x, p0), ind)
    endif else begin
       if (N_ELEMENTS(p1) LT 4) then MESSAGE, 'as least 4 input points need !'
        y = SPL_INTERP(p1, p0, SPL_INIT(p1, p0), p2)
    endelse
    result=FIX(TEMPORARY(y), type=SIZE(p0, /type))
endif else begin
    ;; linear interpolation case
    if (nbp_inside GT 0) then result=INTERPOLATE(isint ? FLOAT(p0) : p0, ind)
    if (nbp_outside GT 0) then begin
        if p2_info eq 0 then begin
            tmp=MAKE_ARRAY(1, type=p0_type)
        endif else begin
            tmp=MAKE_ARRAY(p2_info, type=p0_type)
        endelse
        if (nbp_inside GT 0) then tmp[inside_OK]=result
        last=N_ELEMENTS(p0)-1
        slope_begin=(1.*p0[1]-p0[0])/(p1[1]-p1[0])
        slope_end  =(1.*p0[last-1]-p0[last])/(p1[last-1]-p1[last])
        for ii=0, nbp_outside-1 do begin
            if outside[ii] LT p1_min then begin
                tmp[outside_OK[ii]]=slope_begin*(outside[ii]-p1[0])+p0[0]
            endif else begin
                tmp[outside_OK[ii]]=slope_end*(outside[ii]-p1[last-1])+p0[last-1]
            endelse
        endfor
        if p2_info eq 0 then tmp=tmp[0]
        result=tmp
    endif
endelse
;
if ExistNotFinite then begin
    resres=MAKE_ARRAY(p2_info, type=p0_type)
    resres[index_p2_not_finite]=p2_not_finite
    resres[index_p2_finite]=result
    result=resres
endif
;
if KEYWORD_SET(test) or KEYWORD_SET(debug) then STOP
;
return, result
;
end
