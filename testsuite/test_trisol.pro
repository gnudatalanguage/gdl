;
; Alain Coulais, 03 Fevrier 2012,
; under GNU GPL v2 or later
;
; basic tests for TRISOL
;
pro TEST_TRISOL, double=double, verbose=verbose, $
                 help=help, test=test, debug=debug
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_TRISOL, double=double, verbose=verbose, $'
    print, '                 help=help, test=test, debug=debug'
    return
endif
;
; Define Sub Diag Vect. containing the sub-diagonal elements
; with a strarting  0.0 element:  
subdiag=REPLICATE(1.0,4)*2.
subdiag[0]=0.0
;
; Define "diag" containing the main diagonal elements:  
diag=REPLICATE(-4.0,4)
;
; Define Sup Diag Vect. containing the super-diagonal elements
; with a trailing  0.0 element:  
supdiag=REPLICATE(1.0,4)
supdiag[-1]=0.0
; 
; Define the right-hand side vector:  
RHSvect = [6.0, -8.0, -5.0, 8.0]
;
; Compute the solution and print:  
result = TRISOL(subdiag*1D, diag*1D, supdiag*1D, RHSvect*1D, double=double)
;result = TRISOL(subdiag, diag, supdiag, RHSvect, double=double)
PRINT, result
;
TheMatrix=DIAG_MATRIX(diag)
TheMatrix=TheMatrix+DIAG_MATRIX(subdiag[1:*],-1)
TheMatrix=TheMatrix+DIAG_MATRIX(supdiag[0:-1-1],1)
if KEYWORD_SET(debug) then begin
   print, TheMatrix
endif
;
LHSvect=TheMatrix##result
error=TOTAL((LHSvect-RHSvect)^2)
;
if (error GT 1e-10) then begin
   if KEYWORD_SET(test) then STOP
   ;; please notice the error may also come from other parts of codes
   ;; (e.g. bug in DIAG_MATRIX)
   MESSAGE, /continue, 'Numerical error founded.'
   EXIT, status=1
endif else begin
   if ~KEYWORD_SET(verbose) then begin
      MESSAGE, /continue, 'TRISOL succesfully tested'
   endif
endelse
;
if KEYWORD_SET(test) then STOP
;
end

