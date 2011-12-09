;
; AC 9-dec-2011
; this is a very preliminary test case for LUDC and LUSOL,
; working ONLY when inputs are of DOUBLE types.
;
; help welcome
;
;--------------------------------------
; the first test is based on the example given on IDL pages
; http://idlastro.gsfc.nasa.gov/idl_html_help/LUSOL.html
;
pro TEST_LUDC_IDL, test=test, verbose=verbose

; Define array A:  
A = [[ 2.0,  1.0,  1.0], $
     [ 4.0, -6.0,  0.0], $  
     [-2.0,  7.0,  2.0]]
; Define right-hand side vector B:  
B = [3.0, -8.0, 10.0]  
;
; A must be DOUBLE ...  
a=A*1.D
;
; Decompose A:  
LUDC, A, INDEX  
;
;print, a
res=LUSOL(a, index, b*1.D)
;print, res
;
if KEYWORD_SET(test) then STOP
;
end
;
; the second test is based on the example given on GSL pages
; http://www.gnu.org/s/gsl/manual/html_node/Linear-Algebra-Examples.html
; ------------------------

pro TEST_LUDC_GSL, test=test, verbose=verbose
;
A = [[ 0.18, 0.60, 0.57, 0.96], $
     [ 0.41, 0.24, 0.99, 0.58], $
     [ 0.14, 0.30, 0.97, 0.66], $
     [ 0.51, 0.13, 0.19, 0.85]]
; A must be DOUBLE now ...
a=A*1.D
;
b=[ 1.0, 2.0, 3.0, 4.0]
; B must be DOUBLE now ...
b=A*1.D

LUDC, A, INDEX
;print, a
;print, b

res=LUSOL(a, index, b*1.D)
;print, res

if KEYWORD_SET(test) then STOP

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
end
