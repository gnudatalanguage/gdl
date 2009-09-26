;
; AC 26/09/2009 a Caltech
;
pro TEST_DIMS_OF_ARRAYS
;
print, 'This program will not run to the end'
print, 'Just a list of possible troubles for memory ...'
print, 'You can copy/paste line by line to check different kinds of exceptions'
;
;print, exp()
;print, fltarr()
;
; bad scalar value
a=FLTARR(3)
a=FLTARR(0)
a=FLTARR(-3)
;
; bad value in list of dim
a=FLTARR(3,2)
a=FLTARR(3,-2)
a=FLTARR(-3,2)
a=FLTARR(-3,-2)
a=FLTARR([-3,-2])
;
; bad type conversion
a=FLTARR('s')
a=FLTARR('3')
a=FLTARR([2,'3'])
a=FLTARR([2,'-3'])
;
end
