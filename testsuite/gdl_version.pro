;
; AC 2017-dec-26
;
; we expect the GDL version is as : A.B.C
; with B and C between 0 and 99
;
function GDL_VERSION, version=version, test=test, debug=debug
;
if KEYWORD_SET(version) then suite=version else suite=!gdl.release
;
if KEYWORD_SET(debug) then print, suite
;
ii=strpos(suite,'.')
major=STRMID(suite,0,ii)
suite=STRMID(suite,ii+1)
if KEYWORD_SET(debug) then print, major, suite
;
ii=strpos(suite,'.')
minor=STRMID(suite,0,ii)
suite=STRMID(suite,ii+1)
if KEYWORD_SET(debug) then print, minor, suite
;
ii=strpos(suite,' ')
if ii EQ -1 then dev=STRMID(suite,0) else dev=STRMID(suite,0,ii)
if KEYWORD_SET(debug) then print, dev, suite
;
; we use the fact that FIX() remove all alphabetical chars ...
;print, major, minor, dev
value=10000L*major+100L*minor+dev
;
if KEYWORD_SET(test) then begin
   print, value
   STOP
endif
;
return, value
end
