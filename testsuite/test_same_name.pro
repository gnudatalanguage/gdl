;
; Old code ... revised by AC on April 2025
;
; the goal of this code is to test we can affect
; variables (below "dist" and "toto") which
; have the same name that the calling function
;
; ---------------------------------------
; Modifications history :
;
; - revised by AC on 2025-Apr-18 because it was not OK on IDL
; We choose TRACE() instead of DIST() because TRACE is
; a code in IDL syntax for IDL & GDL, but DIST() is
; an intrinsic code in IDL (but a .pro in GDL)
; Tested as working in FL 0.79.53
;
; ---------------------------------------
;
function TOTO, x
  return, x+1
end

pro TEST_SAME_NAME, verbose=verbose, no_exit=no_exit, $
                    test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SAME_NAME, verbose=verbose, no_exit=no_exit, $'
   print, '                    test=test, help=help'
   return
endif
;
cumul_errors=0
;
; 1st case
in=REPLICATE(1.,3,3)
if ~EXECUTE("trace = TRACE(in)") then ERRORS_ADD, cumul_errors, 'case TRACE()'
if KEYWORD_SET(verbose) then HELP, dist
;
; 2nd case
;
x=0
toto=TOTO(x)
if x NE 0 then ERRORS_ADD, cumul_errors, 'case TOTO, bad X value return'
if toto NE 1 then ERRORS_ADD, cumul_errors, 'case TOTO, bad toto value return'
if KEYWORD_SET(verbose) then print, x, toto
;
; ----------------- final MESSAGE ----------
;
BANNER_FOR_TESTSUITE, 'TEST_SAME_NAME', cumul_errors
;
if (cumul_errors gt 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
if KEYWORD_SET(test) then stopy
;
end
