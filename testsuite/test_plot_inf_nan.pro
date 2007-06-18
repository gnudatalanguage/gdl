;
; AC, June 2007
;
; please run the last procedure "TEST_PLOT_INFO" to have informations 
; about all the routines testing PLOT ...
;
; How are managed NaN and Inf in PLOT ?
;
pro PLOT_INF_NAN, nan=nan, inf=inf, psym=psym, $
                  position=position, verbose=verbose
;
; the keywrod "position" is used to put the Nan or Inf at
; a given position in the array --> may affect plot if bug in MinMax
; procedure to check the min/max boudaries of the plot scale.
;
problem=0.
if KEYWORD_SET(nan) then problem=!values.f_nan
if KEYWORD_SET(inf) then problem=!values.f_infinity
if (problem EQ 0.) then begin
    print, 'Must select /Nan or /Inf'
    return
endif
;
if N_ELEMENTS(position) EQ 0 then position=4
;
if KEYWORD_SET(verbose) then begin
    print, 'Type of Problem: ', problem
    print, 'Position: ', position
endif
;
a=findgen(10)
a=a^2
a[position]=problem
if KEYWORD_SET(verbose) then begin
    print, 'Min, no check on /Nan', min(a)
    print, 'Max, no check on /Nan', max(a)
    print, 'Min, check on /Nan', min(a,/nan)
    print, 'Max, check on /Nan', max(a,/nan)
endif
plot, a, title=STRUPCASE(STRING(problem))+' case', psym=psym
;
;
if KEYWORD_SET(verbose) then begin
    print, '!x.crange: ', !x.crange
    print, '!y.crange: ', !y.crange
endif
;
end
;
; ------------------------------------
;
pro TEST_PLOT_TRICK, nan=nan, inf=inf
;
; Same as PLOT_INF_NAN but more direct !
; (with peculiar tests for Min/Max (NaN or Inf at beginning or end of arrays))
;
problem=0.
if KEYWORD_SET(nan) then problem=!values.f_nan
if KEYWORD_SET(inf) then problem=!values.f_infinity
if (problem EQ 0.) then begin
    print, 'Must select /Nan or /Inf'
    return
endif
;
!p.multi=[0,2,3]
window, xsize=600, ysize=900
;
a=findgen(10)
x=a
y=a^2
y[1]=problem
plot,y, psym=-2, title='No problem in X'
plot, x,y, psym=-2, title='Possible problem in X'
;
x=a
y=a^2
x[0]=problem
x[9]=problem
plot, y, psym=-4, title='No problem'
plot,x,y, psym=-4, title='X problem at (0 and last)'
;
x=a
y=a^2
y[0]=problem
y[9]=problem
plot, y, psym=-4, title='Problem for Y (0 and last)'
plot,x,y, psym=-4, title='X and Y problems at (0 and last)'
;
!p.multi=0
;
end
;
; -----------------------------------------------------
;
pro TEST_PLOT_INF_NAN, psym=psym, verbose=verbose
;
print, 'pro TEST_PLOT_INF_NAN, psym=psym, verbose=verbose'
;
WINDOW,0
PLOT_INF_NAN, /nan, psym=psym, verbose=verbose
WINDOW,2
PLOT_INF_NAN, /nan, psym=psym, pos=0, verbose=verbose
;
WINDOW,1
PLOT_INF_NAN, /inf, psym=psym, verbose=verbose
WINDOW,3
PLOT_INF_NAN, /inf, psym=psym, pos=0, verbose=verbose
;
end
;
