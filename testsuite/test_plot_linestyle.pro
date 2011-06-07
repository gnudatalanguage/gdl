;
; AC June 2007
;
; quick tests for Linestyle as keyword or !p.linestyle
;
; --------------------------
; This first procedure will help to checked whether the swith
; between modes (!p.linestyle vs keyword linstyle=) is OK
;
pro test_switch_linestyle
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
pro multi_linestyle, signal=signal
; see plplot-5.X.Y/doc/docbook/src/plstyl.html
; or http://plplot.sourceforge.net/docbook-manual/plplot-html-5.7.3/plstyl.html
;
window, xsize=900, ysize=600
!p.multi=[0,3,2]
;
styles=findgen(6)
nb_styles=6
;
if (N_ELEMENTS(signal) LE 0) then a=findgen(10) else a=signal
;
for i=0, nb_styles-1 do begin
   plot, a, linestyle=styles[i], title='linestyle:'+string(i)
endfor
;
!p.multi=0
;
end
;
; ------------------------------
; same than the previous one but with a sinus ...
;
pro sinus_line
nbp=500
a=sin(findgen(nbp)/nbp*!pi*2.*4)
multi_linestyle, signal=a
end
; ----------------------
; directly running an example !
;
pro TEST_PLOT_LINESTYLE
multi_linestyle
end

