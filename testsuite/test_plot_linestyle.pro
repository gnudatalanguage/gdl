;
; AC June 2007
;
; quick tests for Linestyle as keyword or !p.linestyle
;
; This first procedure will help to checked whether the swith
; between modes (!p.linestyle vs keyword linstyle=) is OK
;
; ---------------------------------------
; Modifications history :
;
; - 2019-12-06 : AC 
;  * in the presence of NaN (!values.f_nan) and psym= in a series, 
;    the linestyle may change 
;    (as is happen in GIT between 2018-10-29 & 2018-11-06
;    (see bug report https://github.com/gnudatalanguage/gdl/issues/678 )
; 
; ---------------------------------------
;
pro TEST_SWITCH_LINESTYLE
;
!p.multi=[0,2,2]
;
a=findgen(10)
;
; we reset linestyle to continuous line
!p.linestyle=0
;
plot, title='No line Style', a
;
plot, title='Line Style via keyword', a, line=2
;
!p.linestyle=3
plot, title='Line Style via !!P.linestyle', a
;
!p.linestyle=5
plot, title='Line Style via !!P.linestyle AND keyword (prioritary)', a, line=1
;
!p.multi=0
!p.linestyle=0
;
end
;
; -------------------------------------
; testing the 6 linestyles minicing the IDL ones.
;
pro MULTI_LINESTYLE, signal=signal, nan=nan, psym=psym, $
                     winnumber=winnumber, test=test
;
; see plplot-5.X.Y/doc/docbook/src/plstyl.html
; or http://plplot.sourceforge.net/docbook-manual/plplot-html-5.7.3/plstyl.html
;
big_title=GDL_IDL_FL()
if ISA(winnumber) then big_title=big_title+STRING(winnumber, forma='(i2)')+' :'
if KEYWORD_SET(nan) then big_title=big_title+' /NaN'
if KEYWORD_SET(psym) then big_title=big_title+' /psym'
;
if ~KEYWORD_SET(winnumber) then begin
   WINDOW, xsize=900, ysize=600, title=big_title
endif else begin
   WINDOW, winnumber, xsize=900, ysize=600, title=big_title
endelse
!p.multi=[0,3,2]
;
styles=FINDGEN(6)
nb_styles=6
;
if (N_ELEMENTS(signal) LE 0) then begin
   a=FINDGEN(15)
   if KEYWORD_SET(nan) then begin
      a[6]=!values.f_nan
      a[9:10]=!values.f_nan
   endif
endif else begin
   a=signal
   if KEYWORD_SET(nan) then begin
      a[10:20]=!values.f_nan
      a[70:80]=!values.f_nan
   endif
endelse
;
for i=0, nb_styles-1 do begin
   title='linestyle:'+STRING(i, forma='(i2)')
   if KEYWORD_SET(nan) then title=title+' /NaN'
   if KEYWORD_SET(psym) then title=title+' /psym'
   PLOT, a, linestyle=styles[i], title=title, psym=psym
endfor
;
!p.multi=0
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------
; same than the previous one but with a sinus ...
;
pro SINUS_LINE, nan=nan, psym=psym, winnumber=winnumber, test=test
nbp=500
a=SIN(FINDGEN(nbp)/nbp*!pi*2.*4)
MULTI_LINESTYLE, signal=a, nan=nan, psym=psym, $
                 winnumber=winnumber, test=test
end
; ----------------------
; summary
pro SUMMARY_BUG_PLOT_LINESTYLE
WINDOW, xsize=900, ysize=300, title='Summary : all three should be dash-dot'
a=FINDGEN(15)
b=a
a[6]=!values.f_nan
a[9:10]=!values.f_nan
!p.multi=[0,3,1]
;
plot, b, linestyle=3
plot, a, linestyle=3
plot, a, linestyle=3, psym=-5
;
end
; ----------------------
; directly running an example !
;
pro TEST_PLOT_LINESTYLE
;
print, 'Just Visual tests ! Check 1 & 2 then 4 & 5'
print, 'Windows with /NaN and /NaN /psym should have same line styles !'
;
p_ref=!p
!p.symsize=0.2
!p.charsize=2.
;
MULTI_LINESTYLE, winnumber=0
;
MULTI_LINESTYLE, winnumber=1, /nan
MULTI_LINESTYLE, winnumber=2, /nan, psym=-2
;
SINUS_LINE, winnumber=3
;
SINUS_LINE, winnumber=4, /nan
SINUS_LINE, winnumber=5, /nan, psym=-2
;
SUMMARY_BUG_PLOT_LINESTYLE
;
!p=p_ref
;
end

