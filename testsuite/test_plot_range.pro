;
; under GNU GPL v3
; Alain Coulais, 20 june 2017
; 350 years of Paris Observatory today !
;
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
      ADD_ERROR, nb_errors, STRCOMPRESS(message+sign+' : '+string(ii))
   endif
endfor
;
BANNER_FOR_TESTSUITE, 'TEST_PLOT_YRANGES '+sign, nb_errors, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
pro TEST_PLOT_RANGES, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_PLOT_RANGES, help=help, verbose=verbose, $'
   print, '                      no_exit=no_exit, test=test'
   return
endif
;
TEST_PLOT_YRANGES, nb_errors
TEST_PLOT_YRANGES, nb_errors,/negative
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_PLOT_RANGES', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

