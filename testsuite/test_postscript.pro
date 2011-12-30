;
; very basic tests for PostScript output
;
; Alain Coulais 23/08/2011
;
; adding APPLEMAN and SATURN test due to great progress by Sylwester
;
; -------------------------------------------
;
pro TEST_POSTSCRIPT_APPLEMAN
;
DEFSYSV, '!gdl', exists=isGDL
if isGDL then prefix='GDL_' else prefix='IDL_'
;
SET_PLOT, 'PS'
;
DEVICE, filename=prefix+'appleman.ps'
;
plot, findgen(1000), /nodata, $
      xrange=[-10, 650], yrange=[-10,522], /xstyle, /ystyle
;
APPLEMAN, res=res, /NoDisplay
LOADCT, 15
TV, res, xsize=640, ysize=512
;
DEVICE, /close
;
SET_PLOT, 'X'
;
end
;
; -------------------------------------------
;
pro TEST_POSTSCRIPT_BASIC
;
DEFSYSV, '!gdl', exists=isGDL
if isGDL then prefix='GDL_' else prefix='IDL_'
;
SET_PLOT, 'PS'
;
device, filename=prefix+'output1.ps'
plot, FINDGEN(10)
device, /close
;
device, filename=prefix+'output2.ps', xsize=21, ysize=27
plot, FINDGEN(10)
device, /close
;
device, filename=prefix+'output3.ps', xsize=27, ysize=21
plot, FINDGEN(10)
device, /close
;
device, filename=prefix+'output4_portrait.ps', /portrait, $
  xsize=21, ysize=27, xoffset=10, yoffset=5
plot, FINDGEN(10)
device, /close
;
device, filename=prefix+'output4_land.ps', /landscape, $
  xsize=21, ysize=27, xoffset=10, yoffset=5
plot, FINDGEN(10)
device, /close
;
; back to X11
SET_PLOT, 'X'
;
end

pro TEST_POSTSCRIPT
;
TEST_POSTSCRIPT_BASIC
TEST_POSTSCRIPT_APPLEMAN
;
; to do TEST_POSTSCRIPT_SATURN
;
end

