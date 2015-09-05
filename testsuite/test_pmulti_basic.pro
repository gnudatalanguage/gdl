;
; Visual tests for Plot, OPlot and !P.multi
;
; Alain and Gilles, a fast way to see regression
; related to !P.multi and Oplot
;
; This is a derivative work from "demo_graphics1.pro"
;
pro PLOTOPLOT, a, b, xlog=xlog, ylog=ylog, verbose=verbose
;
PLOT, b, a, psym=4,xrange=[1,200],yrange=[1,2000], xlog=xlog, ylog=ylog
if KEYWORD_SET(verbose) then print, format='(4f7.4)', !x.s, !x.window
;
OPLOT, b, a, color=255,psym=8
if KEYWORD_SET(verbose) then print, format='(4f7.4)', !x.s, !x.window
;
end
;
pro TEST_PMULTI_BASIC, xlog=xlog, ylog=ylog, $
                       no_exit=no_exit, no_wdelete=no_wdelete, $
                       help=help, test=test, verbose=verbose
;
print, 'Just Visual tests, red rings must be in white diamonds !!'
print, '' & print, 'You can test the /xlog and /ylog too !'
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_PMULTI_BASIC, xlog=xlog, ylog=ylog, $'
    print, '                       no_exit=no_exit, no_wdelete=no_wdelete, $'
    print, '                       help=help, test=test, verbose=verbose'
    return
endif
;
if GETENV('DISPLAY') eq '' then EXIT, status=77
;
; creating a circular symbol
AA = FINDGEN(17) * (!PI*2/16.) & USERSYM, COS(AA), SIN(AA)
;
; data
a=FINDGEN(100) & a*=10 & b=a mod 220 & a=a+313.3 & b=b+1
;
WINDOW, 0
if KEYWORD_SET(verbose) then print, 'case !P.MULTI=0'
!P.MULTI=0
PLOTOPLOT,a,b, xlog=xlog, ylog=ylog, verbose=verbose

WINDOW, 1
if KEYWORD_SET(verbose) then print, 'case !P.MULTI=[0,2,1]'
!P.MULTI=[0,2,1]
for ii=0,1 do PLOTOPLOT, a, b, xlog=xlog, ylog=ylog, verbose=verbose

WINDOW, 2
if KEYWORD_SET(verbose) then print, 'case !P.MULTI=[0,1,2]'
!P.MULTI=[0,1,2]
for ii=0,1 do PLOTOPLOT, a, b, xlog=xlog, ylog=ylog, verbose=verbose

WINDOW, 3
if KEYWORD_SET(verbose) then print, 'case !P.MULTI=[0,2,2]'
!P.MULTI=[0,2,2]
PLOTOPLOT, a, b, xlog=xlog, ylog=ylog, verbose=verbose
PLOTOPLOT, a, b, /xlog, ylog=ylog, verbose=verbose
PLOTOPLOT, a, b, xlog=xlog, /ylog, verbose=verbose
PLOTOPLOT, a, b, /xlog, /ylog, verbose=verbose
;
if KEYWORD_set(test) OR  KEYWORD_set(no_exit) then STOP
;
if ~KEYWORD_set(no_wdelete) then begin
    print, 'please wait two sec., Window to be delete ...'
    WAIT, 2
    WDELETE, 0,1,2,3
endif
;
end

