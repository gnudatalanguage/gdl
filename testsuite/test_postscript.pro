;
; very basic tests for PostScript output
;
; Alain Coulais 23/08/2011
;
pro TEST_POSTSCRIPT
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
SET_PLOT, 'PS'
;
end
