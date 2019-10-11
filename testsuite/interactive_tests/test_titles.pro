; Work under GNU GPL
;
; Alain Coulais
; Distributed version 2008/02/07
; Initial version in June 2007
;
; Purpose: test suite for TITLE, SUBTITLE, XTITLE and YTITLE keywords
;      in PLOT and CONTOUR, showing positioning, math and greek options
;
; Demo for Embedded Formatting Commands
; http://idlastro.gsfc.nasa.gov/idl_html_help/Embedded_Formatting_Commands.html
; http://idlastro.gsfc.nasa.gov/idl_html_help/Formatting_Command_Examples.html
;----------------------------------------------------------------------
; This function create a STRING containing "number" time the "character"
;
function AC_STRV, character, number
;
vector=character
for ii=1, number-1 do vector=vector+character
return, vector
end


pro title_scale, _extra=_extra
tt=AC_STRV('f', 40)
plot, findgen(10), titl=tt, xtit=tt, ytit=tt, subti=tt, _extra=_extra

end

;
; complex title with greek and math...
;
pro titre1, full=full, ymargin=ymargin, charsize=charsize

t1='!6F(s) = (2!4p)!e-1/2!n !mi!s!a!e!m' + STRING(36b)
t1=t1+'!r!b!i-!m' + STRING(36b) + '!nF(x)e!e-i2!4p!3xs!ndx' 

if KEYWORD_SET(full) then t1b=t1 else t1b=''
if N_ELEMENTS(ymargin) EQ 0 then ymargin=[3,5]
if N_ELEMENTS(charsize) EQ 0 then charsize=2.

plot, findgen(10), charsize=charsize, /nodata, ymargin=ymargin, $
      title=t1, xtitle=t1b, ytitle=t1b, subtitle=t1b

xyouts, 0.2, 0.5, charsize=3, t1,/normal
xyouts, 1, 2, charsize=2.2, t1,/data
end


pro titre2

t2='!MI!S!A!E!8x!R!B!Ip!N !7q!Ii!N!8U!S!E2!R!Ii!Ndx'

if KEYWORD_SET(full) then t2b=t2 else t2b=''

plot, findgen(10),/nodata, ymargin=[3,5], charsize=2, title=t2, $
      xtitle=t2b, ytitle=t2b, subtitle=t2b

XYOUTS, 0.5, .5, t2, $
        charSIZE = 8, /NORMAL
end

;
; exemple basic de superposition
;
pro titre3

t3='!S!3dessous!rDESSUS   n!Son!rOUI'
plot, findgen(10), charsize=3, ymargin=[5,8], title=t3
end
;
; les differents indices ...
;
pro titre4

t4='!3!n niveauref!LlL!n!DdD!n!IiI!n!AaA!n!BbB!n!EeE!n!UuU!UuU'
plot, findgen(10), charsize=3, ymargin=[5,8], title=t4

t4b='!3!n!scou!r!uca !n!scou!r!Lca'
plot, findgen(10), charsize=3, ymargin=[5,8], title=t4b

end

;
; testing and adjusting aligment
;
pro TITRE5, charsize=charsize
;
if N_ELEMENTS(charsize) NE 1 then charsize=2
;
title='aAa!iaiA!uaUA!n!iaA!nDD!dadDA!nttt!lalLA!naa'
plot, ymargin=[3,6], findgen(10), title=title, charsize=charsize

rep=''
read, 'suite (rc)', rep
;
; testing the "sub" index capabilities
;
title='aAa!iaiA!uaUA!n!iaA!nDD!dadDA!nttt!lalLA!naa'
title=title+'!eaa!ubb!ecc!n!daa!euu'
plot, ymargin=[3,6], findgen(10), title=title, charsize=charsize

rep=''
read, 'suite (rc)', rep
title='aAa!iaiA!uaUA!n!iaA!nDD!dadDA!nttt!lalLA!naa'
plot, ymargin=[3,6], findgen(10), title=title, charsize=charsize

end
;
; ----------------------------------------
; showing the effects of positioning commands (!U, !L, !M ...)
;
pro TITLE_FORMATING_EXAMPLE, postscript=postscript, title=title
;
if KEYWORD_SET(postscript) then begin
   set_plot, 'ps'
   device, filename='title_example.ps'
endif
;
PcharRef=!p.charsize
if (!p.charsize LT 1e-6) then begin
   !p.charsize=2
   print, 'You can play with !p.charsize to test other Char Sizes ...'
endif
;
if (N_ELEMENTS(title) EQ 0) then begin
   title='!3RefLevel'
   title=title+'!LLower!S!EExponent!R!IIndex!N  Normal'
   title=title+'!S!EExp!R!IInd!N!S!U Up!R!D Down!N!S!A Above!R!B Below'
endif
;
plot, findgen(10), title=title, $
      xtitle=title, ytitle=title, subtitle=title
;
; remember size= does not exist in GDL
;
XYOUTS, 1, 5, title
;
if KEYWORD_SET(postscript) then begin
   device,/close
   print, 'PostScript File "title_example.ps" generated'
   set_plot, 'x'
endif
;
!p.charsize=PcharRef
;
end
;
; -----------------------------------------------
; AC le 20/01/2008
; we alway have to check the effect with !p.multi ...
;
pro TEST_TITLES_PMULTI, all_titles=all_titles, title=title, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_TITLES_PMULTI, all_titles=all_titles, $'
   print, '                        title=title, help=help'
   print, ''
   print, 'Advices: we can used external parameters (!p.charsize ...)'
   print, 'to play with other options'
   return
endif
;
device,/decomposed
;
if (N_ELEMENTS(title) EQ 0) then title='gaspQWERTYassdsfgagfafd'
;
xtitle=''
ytitle=''
subtitle=''
if KEYWORD_SET(all_titles) then begin
   xtitle=title
   ytitle=title
   subtitle=title
endif
;
window, 0
!p.multi=0
plot, findgen(10), title=title, subtitle=subtitle, xtitle=xtitle, ytitle=ytitle
oplot, 10-findgen(10), color='ff'x
;
window, 1
!p.multi=[0,2,2]
plot, findgen(10), title=title, subtitle=subtitle, xtitle=xtitle, ytitle=ytitle
oplot, 10-findgen(10), color='ff'x

plot, findgen(10), title=title, subtitle=subtitle, xtitle=xtitle, ytitle=ytitle
oplot, 10-findgen(10), color='ff00'x

!p.multi=0

end
;
; ---------------------------------
; calling hub
;
pro TEST_TITLES, all=all
;
s=''
TITLE_FORMATING_EXAMPLE
READ, 'Please type any key to continue', s
;
TEST_TITLES_PMULTI, /all
READ, 'Please type any key to continue', s
;
TITRE1
READ, 'Please type any key to continue', s
;
if NOT(keyword_set(all)) then begin
   print, 'To go further, you have to run individually the examples'
   print, 'or to call them via /all keyword'
   return
endif
;
TITRE2
READ, 'Please type any key to continue', s
;
TITRE3
READ, 'Please type any key to continue', s
;
TITRE4
READ, 'Please type any key to continue', s
;
TITRE5
READ, 'Please type any key to continue', s
;
end

