;+
; NAME: CHISQR_CVF
;
; PURPOSE:
;
;   For the chi squared distribution, find the parameter x that gives
;   you a probability P(X > x) = prob, where 'prob' is specified in
;   the call to Chisqr_cvf. When you call this program, you also need
;   to specify the number of degrees of freedom of the chi squared
;   distribution, 'n_dof'.
;
;   Use that for the chi squared distribution, CDF(a,b) = IGAMMA(b/2,a/2).
;
; INPUT:
;   prob : probability that you want to calculate for the chi squared
;          distribution. Floating point or double precision number, no defaults.
;
;   n_dof : the number of degrees of freedom. Integer, no defaults.
;
; OUTPUT:
;   CHISQR_CVF returns the value of 'x' for which P(X > x) = prob,
;   where 'prob' is specified when calling the function.
;
;
; LICENCE:  MIT License
;
;   Copyright (c) 2017 Dominic H.F.M. Schnitzeler
;
;   Permission is hereby granted, free of charge, to any person obtaining a copy
;   of this software and associated documentation files (the "Software"), to deal
;   in the Software without restriction, including without limitation the rights
;   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
;   copies of the Software, and to permit persons to whom the Software is
;   furnished to do so, subject to the following conditions:
;
;   The above copyright notice and this permission notice shall be included in all
;   copies or substantial portions of the Software.
;
;   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;   SOFTWARE.
;
; MODIFICATION HISTORY:
;  31-Oct-2017 : adding keywords Help & test, formatting
;
function CHISQR_CVF, prob, n_dof, test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'function CHISQR_CVF, prob, n_dof, test=test, help=help'
   return, -1
endif
print, 'gdl'
;
if ((prob LT 0.) or (prob GT 1.)) then begin
   MESSAGE, 'Chisqr_cvf_ds: the specified probability must lie between 0. and 1.'
endif

x_min=  0d                      ; starting point
x_max= 10d                      ; guess
;;
;;   Increase x_max if required:
while (Igamma(n_dof/2d , x_max/2d) LT (1.-prob)) do x_max*=2
;;
;;   At this point, you can assume that the correct value for x lies
;;   between x_min and x_max. Now use bisection to 'home in' on the 
;;   correct value for x.    
eps = 1d-10                     ; precision with which you want to approximate prob
x_new = (x_min+x_max)/2d
val = Igamma(n_dof/2d , x_new/2d)
;
while ABS(val - (1-prob)) gt eps do begin
   if (val LT (1-prob)) then x_min=x_new else x_max=x_new
   x_new = (x_min+x_max)/2d
   val = Igamma(n_dof/2d , x_new/2d)
endwhile
;;
;;   Ensure that a floating point number is written out if the input
;;   for 'prob' is a floating point number. 
;;   If 'prob' is double precision, then the output is automatically
;;   double precision.
;;
if SIZE(prob,/type) eq 4 then x_new=FLOAT(x_new)
;;
if KEYWORD_SET(test) then STOP
;;
return, x_new
;;
END
