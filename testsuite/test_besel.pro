;
; AC 20/04/2007
;
; First 2 functions 
; As provided by IDL examples in v 5.4
;
; Numerical tests in "extreme" cases
; (see improvement between IDL 5.4 and 6.0)
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
pro TEST_BESEL_NUMERIC
print, 'some numerical tests'
;
x1=[1e-39,1e-34,1e-20,1e-14, 0.1, 2., 5.]
x2=[-1., 0., x1]

nan=!VALUES.F_NAN

for ii=0,2 do begin
    order=ii
    print, 'Order =', order
    print, format="(a10, 10(' ',g8.3))", 'x value:', x2
    print, format="(a10,10(' ',g8.3))", 'BeselI:', BESELI(x2,order)
    print, format="(a10,10(' ',g8.3))", 'BeselJ:', BESELJ(x2,order)
    print, format="(a10,10(' ',g8.3))", 'BeselI:', [nan, nan, BESELK(x1,order)]
    print, format="(a10,10(' ',g8.3))", 'BeselY:', [nan, nan, BESELK(x1,order)]
endfor
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
print, ''
print, 'Please remember that GDL does not support now non integer order'
print, 'If you have suggestion, please contact us'
;
end
