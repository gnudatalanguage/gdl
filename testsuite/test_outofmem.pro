; SA
; modifications by Alain C.
;
pro TEST_OUTOFMEM, testnum, bigenough=bigenough, $
                   help=help, test=test
;
if KEYWORD_SET(help) then begin
   PRINT, 'TEST_OUTOFMEM, testnum, bigenough=bigenough, help=help, test=test'
   return
endif
;
; bigenough should be set to a number that:
; - multiplied by the size of byte is allocable
; - multiplied by the size of complex type is not allocable

if N_ELEMENTS(bigenough) EQ 0 then bigenough = 1000L*1000L*1000L

if not KEYWORD_SET(testnum) then begin
   MESSAGE, "usage: GDL> test_outofmem, testnum (1,2,3...), [bigenough=], [/help"
endif

case testnum of
   1 : a = DBLARR(bigenough)
   2 : a = FLTARR(bigenough, /nozero)
   3 : a = BYTARR(bigenough) + COMPLEX(0,1)
   4 : a = HISTOGRAM([0], nbins=bigenough, loc=b)
   5 : CONTOUR, BYTARR(bigenough)
   else : MESSAGE, "argument out of range"
endcase
;
if KEYWORD_SET(test) then STOP
;
end
