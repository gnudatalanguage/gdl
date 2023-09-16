;
; very basic tests for PostScript output
;
; Alain Coulais 23/08/2011
;
; adding APPLEMAN and SATURN test due to great progress by Sylwester
; GD Oct 2014: updated test to reflect progress in compatibility
;
; AC 2018-01-17 : adding former "test_tv_ps.pro"
; ------------------------------------------------------
;
; very preliminary test cases for PostScript outputs and TV ...
; To be extended
;
pro TEST_TV_PS_PMULTI, encap=encap, port=port
;
; Read "Saturn.jpg" and return content in "image"
status=GET_IMAGE_FOR_TESTSUITE(image)
if (status eq 0) then return
;
prefix=GDL_IDL_FL(/prefix)
filename=prefix+'test_tv_pmulti.ps'
MESSAGE, /continue, 'writing file : '+filename
;
SET_PLOT,'PS'
DEVICE, filename=filename, encap=encap, port=port, /color
;
aa=FINDGEN(60)
!P.MULTI=[0,3,2]
for i=0,5 do begin
   pos=10*i
   spos=strtrim(pos,2)
    PLOT, aa, ticklen=1, /xst, /yst, xtitle='image at ('+spos+","+spos+").", ytitle='image length: 50 units.'
    TV, image, pos, pos, /DATA, /true, xsize=50
 endfor
;
DEVICE, /close
!P.MULTI=0
SET_PLOT, 'X'
;
end
;
; ------------------------------------------------------
;
pro TEST_POSTSCRIPT_APPLEMAN
;
prefix=GDL_IDL_FL(/prefix)
filename=prefix+'appleman.ps'
MESSAGE, /continue, 'writing file : '+filename
;
SET_PLOT, 'PS'
DEVICE, filename=filename, xsize=8,ysize=30, /land,/color

PLOT, FINDGEN(1000), /nodata, $
      xrange=[-10, 650], yrange=[-10,522], /xstyle, /ystyle
plots,[0,0,1,1,0],[0,1,1,0,0],/norm
;
APPLEMAN, res=res, /NoDisplay
LOADCT, 15
;TV, res, 0.5, 0.5, xsize=0.5, ysize=0.5, /NORM
TV, res, 0, 0, xsize=200, ysize=300, /DATA
;
DEVICE, /close
SET_PLOT, 'X'
;
end
;
; -------------------------------------------
;
pro TEST_POSTSCRIPT_BASIC
;
prefix=GDL_IDL_FL(/prefix)+'ps_basic_'
;
SET_PLOT, 'PS'
;
filename=prefix+'1.ps'
MESSAGE, /continue, 'writing file : '+filename
device, filename=filename
plot, FINDGEN(10)
device, /close
;
filename=prefix+'2.ps'
MESSAGE, /continue, 'writing file : '+filename
device, filename=filename, xsize=21, ysize=27
plot, FINDGEN(10), title='xsize=21, ysize=27'
device, /close
;
filename=prefix+'3.ps'
MESSAGE, /continue, 'writing file : '+filename
device, filename=filename, xsize=27, ysize=21
plot, FINDGEN(10), title='xsize=27, ysize=21'
device, /close
;
filename=prefix+'4_portrait.ps'
MESSAGE, /continue, 'writing file : '+filename
device, filename=filename, /portrait, $
        xsize=21, ysize=27, xoffset=10, yoffset=5
plot, FINDGEN(10), title='xsize=21, ysize=27, xoffset=10, yoffset=5'
device, /close
;
filename=prefix+'4_land.ps'
MESSAGE, /continue, 'writing file : '+filename
device, filename=filename, /landscape, $
        xsize=21, ysize=27, xoffset=10, yoffset=5
plot, FINDGEN(10), title='xsize=21, ysize=27, xoffset=10, yoffset=5'
device, /close
;
; back to X11
SET_PLOT, 'X'
;
end
;
pro TEST_POSTSCRIPT
;
TEST_POSTSCRIPT_BASIC
TEST_POSTSCRIPT_APPLEMAN
TEST_TV_PS_PMULTI
;
end

