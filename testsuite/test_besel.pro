;
; AC 20/04/2007
;
; First 2 functions 
; As provided by IDL examples in v 5.4
;
; Numerical tests in "extreme" cases
; (see improvement between IDL 5.4 and 6.0)
;
; Improved version on 28 July 2009
; (more cases, non integer numerical cases, managing IDL old version limitations)
;
pro TEST_BESEL_J_Y

X = FINDGEN(100)/10
;Plot J and Y Bessel Functions:
PLOT, X, BESELJ(X, 0), TITLE = 'J and Y Bessel Functions'
OPLOT, X, BESELJ(X, 1)
OPLOT, X, BESELJ(X, 2)
OPLOT, X, BESELY(X, 0), LINESTYLE=2
OPLOT, X, BESELY(X, 1), LINESTYLE=2
OPLOT, X, BESELY(X, 2), LINESTYLE=2
;Annotate plot:
xcoords = [1, 1.66, 3, .7, 1.7, 2.65]
ycoords = [.8, .62,.52, -.42, -.42, -.42]
;
DEFSYSV, '!gdl', exists=is_it_gdl
if (is_it_gdl EQ 1) then begin
    labels = ['J_0','J_1','J_2','Y_0', $
              'Y_1','Y_2']
endif else begin
    labels = ['!8J!X!D0','!8J!X!D1','!8J!X!D2','!8Y!X!D0', $
              '!8Y!X!D1','!8Y!X!D2']
endelse
XYOUTS, xcoords, ycoords, labels, /DATA
;
end
;
pro TEST_BESEL_I_K
;
X = FINDGEN(40)/10  
;
;Plot I and K Bessel Functions:  
PLOT, X, BESELI(X, 0), MAX_VALUE=4, $  
  TITLE = 'I and K Bessel Functions'  
OPLOT, X, BESELI(X, 1)  
OPLOT, X, BESELI(X, 2)  
OPLOT, X, BESELK(X, 0), LINESTYLE=2  
OPLOT, X, BESELK(X, 1), LINESTYLE=2  
OPLOT, X, BESELK(X, 2), LINESTYLE=2  
     
;Annotate plot:  
xcoords = [.18, .45, .95, 1.4, 1.8, 2.4]  
ycoords = [2.1, 2.1,  2.1, 1.8, 1.6, 1.4]  
;
DEFSYSV, '!gdl', exists=is_it_gdl
if (is_it_gdl EQ 1) then begin
      labels = ['K_0','K_1','K_2','I_0', $
                'I_1','I_2']
endif else begin
    labels = ['!8K!X!D0','!8K!X!D1','!8K!X!D2','!8I!X!D0', $
              '!8I!X!D1','!8I!X!D2']
endelse

XYOUTS, xcoords, ycoords, labels, /DATA 
end
;
pro TEST_BESEL_NUMERIC, orders=orders, x=x, $
                        allow_negative=allow_negative, force_positive=force_positive, $
                        test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_BESEL_NUMERIC, orders=orders, x=x, $'
   print, '                        allow_negative=allow_negative, force_positive=force_positive, $'
   print, '                        test=test, help=help'
   return
endif
;
print, 'some numerical tests on Besel Family, used: TEST_BESEL_NUMERIC, /Help for info'
;
if (N_ELEMENTS(orders) EQ 0) then begin
   orders=[0,1,2]
   print, 'default ORDERS used ... can be changed with Orders='
endif
;
if (N_ELEMENTS(x) EQ 0) then begin
   print, 'default X values used ... can be changed with X='
   x1=[1e-39,1e-34,1e-20,1e-14, 0.1, 2., 5.]
   x2=[-1., 0., x1]
endif else begin
   x2=x
endelse
;
DEFSYSV, '!gdl', exists=is_it_gdl
if (is_it_gdl EQ 0) then begin
   ;; no IDL 6.1 here to check if the change occurs in 6.1
   ;; for 6.2 and later, negative values are OK ...
   ;; Tested on 5.5, 6.0, 6.1, 6.2 and 7.1 : only 6.0 affected
   if (!version.release EQ '6.0') then begin
      print, 'Warning: some IDL old versions don''t use negatives values in X'
      print, 'This option can be desactivated using /Force_Negative'
      if NOT(KEYWORD_SET(allow_negative)) then begin
          ok=WHERE(x2 GT 0, nbpOK)
          if (nbpOK GT 0) then x2=x2[ok] else return
      endif
   endif
endif
;
; we need to prepare "xpos" array without Neg values for BeselK and BeselY
xpos=x2
pb=WHERE(x2 LT 0, nbpPB)
if (nbpPB GT 0) then xpos[pb]=0.0
;
; for BeselI and other, if N not integer, X must be positive ...
if KEYWORD_SET(force_positive) then begin
    x2=xpos
endif
;
for ii=0, N_ELEMENTS(orders)-1 do begin
   order=orders[ii]
   print, 'Order =', order
   print, format="(a10, 10(' ',g9.3))", 'x value:', x2
   print, format="(a10,10(' ',g9.3))", 'BeselI:', BESELI(x2,order)
   print, format="(a10,10(' ',g9.3))", 'BeselJ:', BESELJ(x2,order)
   print, format="(a10,10(' ',g9.3))", 'BeselK:', BESELK(xpos,order)
   print, format="(a10,10(' ',g9.3))", 'BeselY:', BESELY(xpos,order)
endfor
;
print, 'Please remember that BESELK and BESELY can not be called with Neg. Values --> 0., NaN or Inf'
;
if KEYWORD_SET(test) then STOP
;
end
;
pro TEST_BESEL
;
window, xsize=800, ysize=400
!p.multi=[0,2,0]
TEST_BESEL_I_K
TEST_BESEL_J_Y
!p.multi=0
TEST_BESEL_NUMERIC
;
end
