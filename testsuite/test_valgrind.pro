;
; AC at LAX, 29/09/2009
;
; How to have systematic tests with valgring ?
;
;
pro VALGRIND_INIT_ARRAYS, nbp=nbp, x, y, x_v, y_v, data
;
if N_ELEMENTS(nbp) EQ 0 then nbp=100
;
xnbp=nbp
ynbp=xnbp
;
data=DIST(xnbp, ynbp)
x=FINDGEN(xnbp)
y=FINDGEN(ynbp)
;
x_v=x#REPLICATE(1.,ynbp)
y_v=REPLICATE(1.,xnbp)#y
;
end
;
pro VALGRIND_CONTOUR, nbp=nbp, level=level, test=test, quiet=quiet
;
if N_ELEMENTS(level) EQ 0 then return
if (level LE 0) OR (level GT 3) then return
;
VALGRIND_INIT_ARRAYS, nbp=nbp, x, y, x_v, y_v, data
;
if NOT(KEYWORD_SET(quiet)) then print, 'Begin of VALGRIND_CONTOUR with level=', level
;
if (level EQ 1) then CONTOUR, data
if (level EQ 2) then CONTOUR, data, x, y
if (level EQ 3) then CONTOUR, data, x_v, y_v
;
if KEYWORD_SET(test) then STOP
;
if NOT(KEYWORD_SET(quiet)) then print, 'End of VALGRIND_CONTOUR'
;
end
;
pro VALGRIND_SURFACE, nbp=nbp, level=level, test=test, quiet=quiet
;
if N_ELEMENTS(level) EQ 0 then return
if (level LE 0) OR (level GT 3) then return
;
VALGRIND_INIT_ARRAYS, nbp=nbp, x, y, x_v, y_v, data
;
if NOT(KEYWORD_SET(quiet)) then print, 'Begin of VALGRIND_SURFACE with level=', level
;
if (level EQ 1) then SURFACE, data
if (level EQ 2) then SURFACE, data, x, y
if (level EQ 3) then SURFACE, data, x_v, y_v
;
if KEYWORD_SET(test) then STOP
;
if NOT(KEYWORD_SET(quiet)) then print, 'End of VALGRIND_SURFACE'
;
end
;
pro TEST_VALGRIND, nbp=nbp, quiet=quiet
print, 'Some tests for Valgrind (usage: valgrind path/to/gdl)'
print, 'don''t forgot that when a flag is set, problem is not more displayed'
;
for ii=1,3 do VALGRIND_SURFACE, nbp=nbp, quiet=quiet, level=ii
for ii=1,3 do VALGRIND_CONTOUR, nbp=nbp, quiet=quiet, level=ii
end


