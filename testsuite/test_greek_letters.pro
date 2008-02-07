;
; Alain Coulais
; Distributed version 2008/02/07
; Under GNU GPL
;
; Purpose: quickly display all Greek Letters with corresponding ones.
;
pro TEST_GREEK_LETTERS, postscript=postscript
;
if KEYWORD_SET(postscript) then begin
   set_plot,'ps'
   defsysv, '!gdl', exist=exist
   if (exist EQ 1) then suffix='GDL.ps' else suffix='IDL.ps'
   device, file='greek_letter_'+suffix
endif
;
nbx=!p.multi[1]
nby=!p.multi[2]

nbx=(nbx EQ 0) ? 1 : nbx
nby=(nby EQ 0) ? 1 : nby
if (nbx*nby) GT 1 then begin
   print, 'This demo program is not designed to work well with !P.multi ...'
endif
;
; We store some parameters which may affect the rendering ...
;
ref_charsize=!p.charsize
!p.charsize=0.0
ref_Xmargin=!x.margin
ref_Ymargin=!y.margin
!x.margin=[10,3]
!y.margin=[4,2]
;
plot, findgen(10),/nodata, title='!3GDL greek letters'
;
charsize=2
space=' '
liste_maj=STRING(TRANSPOSE(65B+BYTE(INDGEN(26))))
liste_min=STRING(TRANSPOSE(97B+BYTE(INDGEN(26))))
nb_letters=N_ELEMENTS(liste_maj)
;
row=6
lines=5
;
for kk=0, nb_letters-1 do begin
   ii= kk / lines
   jj= kk mod lines

   x=(1.+ii)/float(lines+2)
   y=(1.+jj)/float(lines+1)

;   print, ii / row, ii mod lines
;   print, x, y
   xyouts, x, y, charsize=charsize, /normal, $
           '!3'+liste_min[kk]+space+'!4'+liste_min[kk]+space+liste_maj[kk]
endfor
;
if KEYWORD_SET(postscript) then begin   
   device, /close
   set_plot,'x'
endif
;
!p.charsize=ref_charsize
!x.margin=ref_Xmargin
!y.margin=ref_Ymargin
;
end
