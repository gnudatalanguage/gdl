;
; Alain C. 24/Feb/2009
; fast way to check whether TV works in all cases of permutation in [1,N,M]
;
; AC 2016-03-03 : merging TEST_TV_DAMIER and TEST_TV_DAMIER_COLOR,
; adding tic/toc to check X11 bottleneck(s) on network (remote TV/plot
; are too slow)
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
tmp_data=REFORM(data)
xdim=(SIZE(tmp_data))(1)
ydim=(SIZE(tmp_data))(2)
;
;print, xdim, ydim
yoffset=50+indice*(ydim+20)
xoffset=10
;
WINDOW, indice, title=STRING(indice)+' '+TITLE4TEST_TV(data), $
        xpos=xoffset, ypos=yoffset, xsize=xdim, ysize=ydim
TVSCL, data
;
end
;
; -------------------------------------
; We need to test that when changing the color table,
; this table will be propagated in other windows 
; when they will be active via WSET
;
pro TEST_TV_WSET, errors, test=test
;
; Read "Saturn.jpg" and return content in "image"
status=GET_IMAGE_FOR_TESTSUITE(image)
if (status eq 0) then return
;
xdim=(SIZE(image))(2)
ydim=(SIZE(image))(3)
; this test, as it is now, is wrong when the screen is too small (use
; Xvfb :1 -screen 0 1024x768x24 & for example)
; I return if window sizes are too big
device,get_screen_size=scrsize
scrsizex=scrsize[0]
scrsizey=scrsize[1]

if (2*xdim ge scrsizex or 3*ydim ge scrsizey) then begin
   print,"test TEST_TV_WSET not executed, screen size to small (FIXME)"
   if ~ISA(errors) then errors=0
   return
end

;
image=TOTAL(image,1)
;
DEVICE, decomposed=0
;
LOADCT, 13
;
WINDOW, 0, xsize=xdim, ysize=3*ydim
WINDOW, 2, xsize=xdim, ysize=3*ydim
;
WSET, 0
;
TV, image, 0
LOADCT, 20
TV, image, 1
LOADCT, 30
TV, image, 2
;
; switching to window 2
WSET, 2
TV, image, 2
LOADCT, 20
TV, image, 1
LOADCT, 13
TV, image, 0
;
; reading contents of the 2 windows : should have the same
;
content_win2=TVRD()
;
WSET, 0
content_win0=TVRD()
;
if ~ISA(errors) then errors=0
;
if ~ARRAY_EQUAL(content_win0,content_win2) then begin
    errors++
    BANNER_FOR_TESTSUITE, "TEST_TV_WSET", 1, /SHORT
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------
;
pro TEST_TV_DAMIER, numwin, color=color, benchmark=benchmark, $
                    nb_cells_max=nb_cells_max, silent=silent, $
                    test=test, debug=debug, help=help
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_TV_DAMIER, numwin, color=color, benchmark=benchmark, $'
    print, '                    nb_cells_max=nb_cells_max, silent=silent, $'
    print, '                    test=test, debug=debug, help=help'
    print, ''
    print, 'Three levels of time benchmarking : 0 no, =1 global, =2 detail'
    return
end
;
if ~ISA(silent) then silent=1
;
; AC 2016-04-14 seems not to be useful
; if KEYWORD_SET(color) then DEVICE, decompose=0
;
; Three levels of time benchmarking : 0 non, 1 global, 2 detail
;
if KEYWORD_SET(benchmark) then begin
    t_glob=TIC('global')
endif else begin
    benchmark=0
endelse
;
if N_PARAMS() EQ 0 then numwin=0
;
units=64
nbx=10
nby=8
WINDOW, numwin, xsi=units*nbx, ysi=units*nby
;
; AC 2016-04-14 useful to have W&B reproducible !
LOADCT, 0
;
vignette=DIST(units)
;
offset_line=0
nb_cells=nbx*nby
if KEYWORD_SET(nb_cells_max) then begin
    if (nb_cells_max GT 0) AND (nb_cells_max LT nb_cells/2) then begin
        nb_cells=nb_cells_max*2
    endif
endif
;
for ii=0, (nb_cells/2-1)  do begin
    offset_line=(ii / (nbx/2)) mod 2
    if KEYWORD_SET(debug) then print, ii, offset_line, 2*ii+offset_line
    ;;
    ;; loading here color table if needed
    if KEYWORD_SET(color) then begin
        if (benchmark GT 1) then begin
            print, 'Before loading Color Table '
            TIC
        endif
        LOADCT, ii, silent=silent
        if (benchmark GT 1) then TOC
    endif
    ;;
    if (benchmark GT 1) then begin
        print, 'Before TVSCL'
        TIC
    endif
    TVSCL, vignette, 2*ii+offset_line
    if (benchmark GT 1) then TOC
endfor
;
if KEYWORD_SET(benchmark) then TOC, t_glob
;
if KEYWORD_SET(test) then STOP
;;
end 
;
; -------------------------------------
;
function TEST_TV_OVER_BOX
;
; Read "Saturn.jpg" and return content in "image"
status=GET_IMAGE_FOR_TESTSUITE(image)
if (status eq 0) then return, 0
;
redChannel = REFORM(image[0, *, *])
greenChannel = REFORM(image[1, *, *])
blueChannel = REFORM(image[2, *, *])
;
aa=FINDGEN(32)
WINDOW,11
LOADCT,13
;
plot,aa,back=88
tv, redChannel,0,CHAN=1
tv, redChannel,0,0,/DATA,CHAN=1
tv, greenChannel,0,CHAN=2
tv, greenChannel,10,10,/DATA,CHAN=2
tv, blueChannel,0,CHAN=3
tv, blueChannel,20,20,/DATA,CHAN=3
WINDOW,12
!P.MULTI=[0,3,2]
for i=0,5 do begin
    plot,aa
    TV, image, 10,10,/DATA,/true,xsize=50
end
!P.MULTI=0
return, 1
end
;
; -------------------------------------
;
pro TEST_TV, noclose=noclose, test=test, no_exit=no_exit
;
if (!d.name EQ 'NULL') then begin
   is_X11_ok=EXECUTE('set_plot, "X"')
   if (is_X11_ok EQ 0) then begin
      if ~KEYWORD_SET(no_exit) then EXIT, status=77 else STOP
   endif
endif
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
TEST_TV_DAMIER, 8
TEST_TV_DAMIER, 9, /COLOR
;
success=TEST_TV_OVER_BOX()
;
; only this test gives a feedback now
TEST_TV_WSET, errors
;
if KEYWORD_SET(noclose) then begin
   rep=''
   READ, 'press any key to finish (and closing all windows)', rep
endif else begin
    print, 'You can use keyword /NoClose to de-activate auto closing'
endelse
;;
WDELETE, 0, 1, 2, 3, 4, 5, 6, 8, 9
if (success eq 1) then WDELETE, 11, 12
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_TV', errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
