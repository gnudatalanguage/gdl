;
; AC 9-dec-2011
; this is a very preliminary test case for LUDC and LUSOL,
; working ONLY when inputs are of DOUBLE types.
;
; AC 13-Feb-2012: extended to other types but LUDC is today
; only able to return Double type.
;
; help welcome, extensions welcome (esp. examples and code for
; missing parameters (/column, interchanges= ...)
;
; ------------------------------------------------
; the first test is based on the example given on IDL pages
; http://idlastro.gsfc.nasa.gov/idl_html_help/LUSOL.html
;
pro TEST_LUDC_IDL, double=double, noexit=noexit, test=test, verbose=verbose
;
print, 'testing LUDC then LUSOL on IDL example'

; Define array A:  
A = [[ 2.0,  1.0,  1.0], $
     [ 4.0, -6.0,  0.0], $  
     [-2.0,  7.0,  2.0]]
a_ref=a
; Define right-hand side vector B:  
B = [3.0, -8.0, 10.0]  
;
if KEYWORD_set(double) then begin 
   ;; A and B must be DOUBLE now ...
   a=A*1.D
   b=b*1.D
endif
;
; Decompose A:  
LUDC, A, INDEX  
;
res=LUSOL(a, index, b)
;
print, "expected:", b
print, "computed:", TRANSPOSE(a_ref##res)
;
error=1e-5
nb_errors=0
;
resu=SQRT(TOTAL((b-TRANSPOSE(a_ref##res))^2))
if (resu GT error) then nb_errors=nb_errors+1
;
if (nb_errors GT 0) then begin
   MESSAGE, /continue, "Problems !!"
   if ~KEYWORD_SET(noexit) then EXIT, status=1
endif else begin
   MESSAGE, /continue, "Basic tests OK"
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------
;
; the second test is based on the example given on GSL pages
; http://www.gnu.org/s/gsl/manual/html_node/Linear-Algebra-Examples.html
;
pro TEST_LUDC_GSL, double=double, noexit=noexit, $
                   test=test, verbose=verbose, debug=debug
;
print, 'testing LUDC then LUSOL on GSL example'

A = [[ 0.18, 0.60, 0.57, 0.96], $
     [ 0.41, 0.24, 0.99, 0.58], $
     [ 0.14, 0.30, 0.97, 0.66], $
     [ 0.51, 0.13, 0.19, 0.85]]
a_ref=a
;
b=[ 1.0, 2.0, 3.0, 4.0]
;
if KEYWORD_set(double) then begin 
   ;; A and B must be DOUBLE now ...
   a=A*1.D
   b=b*1.D
endif

LUDC, A, INDEX

if KEYWORD_SET(debug) then STOP

res=LUSOL(a, index, b)
;print, res

print, "expected:", b
print, "computed:", TRANSPOSE(a_ref##res)
;
error=1e-5
nb_errors=0
;
resu=SQRT(TOTAL((b-TRANSPOSE(a_ref##res))^2))
if (resu GT error) then nb_errors=nb_errors+1
;
if (nb_errors GT 0) then begin
   MESSAGE, /continue, "Problems !!"
   if ~KEYWORD_SET(noexit) then EXIT, status=1
endif else begin
   MESSAGE, /continue, "Basic tests OK"
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
; to be extended in such a way errors codes can be managed ...
;
pro TEST_LUDC_LUSOL
;
TEST_LUDC_IDL
TEST_LUDC_GSL
;
TEST_LUDC_IDL,/double
TEST_LUDC_GSL,/double
;
end
