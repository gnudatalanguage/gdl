;
; Alain C. 24/Feb/2009
; fast way to check whether TV works in all cases of permutation in [1,N,M]
;
function TITLE4TEST_TV, data, debug=debug
;
if KEYWORD_SET(debug) then print, 'Size: ', SIZE(data)
;
sep=','
if ((SIZE(data))(0) EQ 2) then begin
   x=(SIZE(data))(1)
   y=(SIZE(data))(2)
   return, STRCOMPRESS('['+STRING(x)+sep+STRING(y)+']')
endif
if ((SIZE(data))(0) EQ 3) then begin
   x=(SIZE(data))(1)
   y=(SIZE(data))(2)
   z=(SIZE(data))(3)
   sep=','
   return, STRCOMPRESS('['+STRING(x)+sep+STRING(y)+sep+STRING(z)+']')
endif
print, 'Fatal: we should never be here !'
STOP
end
; -------------------------------------
;
pro MY_WINDOW, indice, data
;
tmp_data=reform(data)
xdim=(SIZE(tmp_data))(1)
ydim=(SIZE(tmp_data))(2)
;
print, xdim, ydim
yoffset=50+indice*(ydim+20)
xoffset=10
;
WINDOW, indice, title=STRING(indice)+' '+TITLE4TEST_TV(data), $
        xpos=xoffset, ypos=yoffset, xsize=xdim, ysize=ydim
TVSCL, data
end
; -------------------------------------
;
pro TEST_TV, noclose=noclose, test=test
;
xdim=350
ydim=100
;
yoffset=FINDGEN(7)*ydim
xoffset=REPLICATE(0,7)
;
a=DIST(xdim, ydim)
b1=REFORM(a,1, xdim, ydim)
b2=REFORM(a,xdim, 1, ydim)
b3=REFORM(a,xdim, ydim, 1)
;
MY_WINDOW, 0, a
MY_WINDOW, 1, b1
MY_WINDOW, 2, REFORM(b1)
MY_WINDOW, 3, b2
MY_WINDOW, 4, REFORM(b2)
MY_WINDOW, 5, b3
MY_WINDOW, 6, REFORM(b3)
;
if NOT(KEYWORD_SET(noclose)) then begin
   rep=''
   READ, 'press any key to finish (and closing all windows)', rep
   ;;
   WDELETE, 0, 1, 2, 3, 4, 5, 6
endif
;
if KEYWORD_SET(test) then STOP
;
end
