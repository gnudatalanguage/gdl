;
; under GNU GPL v3
; Alain Coulais, 20 june 2017
; 350 years of Paris Observatory today !
;
; -----------------------------------------------
; 
; Modifications history :
;
; - 2019-11-06 : AC : should not be run when device is NULL
;                (is it OK/useful to extend for PS & SVG devices ?)
;
; - 2020-04-30 : AC now working for device NULL (env MINI in Travis ...)
;
; - 2022-12-16 : merging historical "test_plotting_ranges.pro" here
;                adding new case :(
;
; -----------------------------------------------
;
; internal utility : shortcut to have the values
pro CRANGE
print, '!X.crange : ', !X.crange
print, '!Y.crange : ', !Y.crange
end
; -----------------------------------------------
; This incredible bug was reported by René Gastaud !
; Intermitendly happen since 0.9.5 ... (see on u14.04)
;
pro TEST_PLOT_YRANGES, cumul_errors, negative=negative, $
                       test=test, verbose=verbose, debug=debug
;
nb_errors=0
;
message='pb with !Y.crange for case : '
sign='**POSITIVE**'
;
x=FINDGEN(10)
;
extended_ranges=[-0.001,0.,0.001]
expected=[[-2.,10],[0,10],[0,10]]
;
if KEYWORD_SET(negative) then begin   
   x=-x
   expected=[[-10.,0],[-10,0],[-10,2]]
   sign='**NEGATIVE**'
endif
;
for ii=0, N_ELEMENTS(extended_ranges)-1 do begin
   ;;
   plot, x+extended_ranges[ii]
   ;;
   if KEYWORD_SET(verbose) then begin
      print, 'Min :', MIN( x+extended_ranges[ii])
      print, 'Max :', MAX( x+extended_ranges[ii])
      print, 'expected !y.crange : ', expected[*,ii]
      print, 'effective !y.crange : ', !y.crange
   endif
   ;;
   if ~ARRAY_EQUAL(expected[*,ii], !y.crange) then begin
      ERRORS_ADD, nb_errors, STRCOMPRESS(message+sign+' : '+string(ii))
   endif
endfor
;
; ---------- final message ---------- 
;
txt='TEST_PLOT_YRANGES '
BANNER_FOR_TESTSUITE, txt+sign, nb_errors, pref=txt, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------
; testing axis range-selection in various plotting routines
; TODO:
; - repeat the tests for SURFACE, CONTOUR, etc, and for X/Y axes
; - repeat the tests for autoscaling, different !*.STYLEs, and *RANGE keywords
; - test /[XY]LOG cases
;
pro TEST_PLOTTING_RANGES, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
;  
;  set_plot, 'z'
;
; First test ...
;
PLOT, [1948, 2006], [0,1]
if ~ARRAY_EQUAL(!x.crange, [1940.0, 2020.0]) then ERRORS_ADD, nb_errors, 'error case 1 on !X.crange'
if ~ARRAY_EQUAL(!y.crange, [0.0, 1.0]) then ERRORS_ADD, nb_errors, 'error case 1 on !Y.crange'
;
; Second test ...
;
PLOT, [1, 10], /ylog
if ~ARRAY_EQUAL(!x.crange, [0.0, 1.0]) then ERRORS_ADD, nb_errors, 'error case 2 on !X.crange'
if ~ARRAY_EQUAL(!y.crange, [0.0, 1.0]) then ERRORS_ADD, nb_errors, 'error case 2 on !Y.crange'
;
; Third test ...
;
PLOT, [0], yrange=[400,1500] 
if ~ARRAY_EQUAL(!x.crange, [0.0, 1.0]) then ERRORS_ADD, nb_errors, 'error case 3 on !X.crange'
if ~ARRAY_EQUAL(!y.crange, [400.0, 1600.0]) then ERRORS_ADD, nb_errors, 'error case 3 on !Y.crange'
;
; ---------------------------------------- CASE 4 ---------------------------
;
; Fourth test ... (new on Dec 15, 2022, regression appeared)
;
a=3*RANDOMU(seed, 10000)-0.5
PLOT, a, xrange=[2400, 2400+24*24-1]
if ~ARRAY_EQUAL(!x.crange, [2400.0, 3000.0]) then ERRORS_ADD, nb_errors, 'error case 4 on !X.crange'
if ~ARRAY_EQUAL(!y.crange, [-1.0, 3.0]) then ERRORS_ADD, nb_errors, 'error case 4 on !Y.crange'
;
; repeat on extended *positive* range
;
expected1=2400L*10*(1+FINDGEN(4))
expected=[[expected1],[expected1+600]]
;
a=3*RANDOMU(seed, 1000000)-0.5
for ii=1,4 do begin
   PLOT, a, xrange=[2400L*10*ii, 2400L*ii*10+24*24-1]
   if ~ARRAY_EQUAL(!x.crange, REFORM(expected[ii-1,*])) then ERRORS_ADD, nb_errors, 'error case 4 on !X.crange'
   if ~ARRAY_EQUAL(!y.crange, [-1.0, 3.0]) then ERRORS_ADD, nb_errors, 'error case 4 on !Y.crange'
   if KEYWORD_SET(verbose) then print, !x.crange, !y.crange
endfor
;
; ---------------------------------------- CASE 5 ---------------------------
;
; repeat on extended *negative* range
;
plot, FINDGEN(1000)-1000, sin([0:1:0.001]*!dpi*4), xrange=[-400,-10]
if ~ARRAY_EQUAL(!x.crange, [-400.0, 0.0]) then ERRORS_ADD, nb_errors, 'error case 5a (neg) on !X.crange'
if ~ARRAY_EQUAL(!y.crange, [-1.0, 1.0]) then ERRORS_ADD, nb_errors, 'error case 5a (neg) on !Y.crange'
if KEYWORD_SET(verbose) then print, !x.crange, !y.crange
;
plot, FINDGEN(1000)-1000, sin([0:1:0.001]*!dpi*4), xrange=[-400,-100]
if ~ARRAY_EQUAL(!x.crange, [-400.0, -100.0]) then ERRORS_ADD, nb_errors, 'error case 5b (neg) on !X.crange'
if ~ARRAY_EQUAL(!y.crange, [-1.0, 1.0]) then ERRORS_ADD, nb_errors, 'error case 5b (neg) on !Y.crange'
if KEYWORD_SET(verbose) then print, !x.crange, !y.crange
;
plot, FINDGEN(1000)-1000, sin([0:1:0.001]*!dpi*4), xrange=[-400,-120]
if ~ARRAY_EQUAL(!x.crange, [-400.0, -100.0]) then ERRORS_ADD, nb_errors, 'error case 5c (neg) on !X.crange'
if ~ARRAY_EQUAL(!y.crange, [-1.0, 1.0]) then ERRORS_ADD, nb_errors, 'error case 5c (neg) on !Y.crange'
if KEYWORD_SET(verbose) then print, !x.crange, !y.crange
;
plot, findgen(1000)-1000, sin([0:1:0.001]*!dpi*4), xrange=[-400,-151]
if ~ARRAY_EQUAL(!x.crange, [-400.0, -150.0]) then ERRORS_ADD, nb_errors, 'error case 5d (neg) on !X.crange'
if ~ARRAY_EQUAL(!y.crange, [-1.0, 1.0]) then ERRORS_ADD, nb_errors, 'error case 5d (neg) on !Y.crange'
if KEYWORD_SET(verbose) then print, !x.crange, !y.crange
;
; ---------- final message ---------- 
;
BANNER_FOR_TESTSUITE, 'TEST_PLOTTING_RANGES', nb_errors, pref=txt, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------
;
pro TEST_PLOT_RANGES, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_PLOT_RANGES, help=help, verbose=verbose, $'
   print, '                      no_exit=no_exit, test=test'
   return
endif
;
rname=ROUTINE_NAME()
;
if ~CHECK_IF_DEVICE_IS_OK(rname, /force) then begin
   if KEYWORD_SET(no_exit) then STOP else return
endif
;
; store the !p/!d env
save_p=!p
save_dname=!d.name
;
cumul_errors=0
;
TEST_PLOT_YRANGES, cumul_errors
TEST_PLOT_YRANGES, cumul_errors, /negative
;
TEST_PLOTTING_RANGES, cumul_errors, verbose=verbose
;
; reset the initial !p/!d env.
!p=save_p
SET_PLOT, save_dname
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_PLOT_RANGES', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
