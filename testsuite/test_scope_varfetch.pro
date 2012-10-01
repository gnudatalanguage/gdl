; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; part of GDL - GNU Data Language
;
; this is also important feature since used in recent versions
; of READCOL in the widely used AtroLib
; see also bug report 3573558
;
; this is not a clean version of this test ...
;
function TEST_SCOPE_CARFETCH_SUB
  return, SCOPE_VARFETCH('a', level=-1)
end
;
; this code is used in recent versions of READCOL in AstroLib
;
pro ASTRON_READCOL1, a,b,c
MESSAGE,/continue, 'inside'
list_of_name=['a','b','c']
(SCOPE_VARFETCH(list_of_name[0], LEVEL=0)) = FINDGEN(10)
end

pro ASTRON_READCOL1BIS, a,b,c
MESSAGE,/continue, 'inside'
list_of_name=['a','b','c']
for ii=0, 2 do begin
   (SCOPE_VARFETCH(list_of_name[ii], LEVEL=0)) = FINDGEN(10)
endfor
end

pro ASTRON_READCOL2, a,b,c
MESSAGE,/continue, 'inside'
list_of_name=['a','b','c']
(SCOPE_VARFETCH('a', LEVEL=0)) = FINDGEN(10)
end
;
; ---------------------------------
;
pro TEST_SCOPE_VARFETCH_NO_KEYWORD
;
a = 1234
print, a
if a ne test_scope_carfetch_sub() then exit, status=1
;
debug=1
if KEYWORD_SET(debug) then mydata1=0
ASTRON_READCOL1, mydata1,b1,c1
print, mydata1
;
if KEYWORD_SET(debug) then mydata1bis=0
ASTRON_READCOL1BIS, mydata1bis,b1bis,c1bis
print, mydata1bis

if KEYWORD_SET(debug) then mydata2=0
ASTRON_READCOL2, mydata2,b2,c2
print, mydata2
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------
;
pro TEST_SCOPE_VARFETCH_WITH_KEY, help=help, debug=deug, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SCOPE_VARFETCH_WITH_KEY, help=help, debug=deug, test=test'
   return
endif
;
a = 1234
print, a
if a ne TEST_SCOPE_CARFETCH_SUB() then exit, status=1
;
if KEYWORD_SET(debug) then mydata1=0
ASTRON_READCOL1, mydata1,b1,c1
print, mydata1
;
if KEYWORD_SET(debug) then mydata1bis=0
ASTRON_READCOL1BIS, mydata1bis,b1bis,c1bis
print, mydata1bis

if KEYWORD_SET(debug) then mydata2=0
ASTRON_READCOL2, mydata2,b2,c2
print, mydata2
;
if KEYWORD_SET(test) then STOP
;
end

pro TEST_SCOPE_VARFETCH, help=help, debug=deug, test=test

TEST_SCOPE_VARFETCH_WITH_KEY, help=help, debug=deug, test=test
TEST_SCOPE_VARFETCH_NO_KEYWORD

end
