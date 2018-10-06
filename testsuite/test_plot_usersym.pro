;
; This test verifys whether PLOT, PLOTS & OPLOT
; are able to plot USERSYM with color, fill & simsize
;
; Alain C., August 2017, at JSC during Harvey
;
; ---------------------------------------------------
;
; I stole this snipet in "plotsym.pro" in IDLastro lib.
; and change it into a more general one
;
; Angle= is in Degree
; default Branches number is 5
;
pro SYM_STAR, branches, angle=angle, ratio=ratio, $
              color=color, thick=thick, fill=fill, $
              help=help, verbose=verbose, test=test
;              
if N_PARAMS() EQ 0 then nb_tops=5 else nb_tops=branches
;
if N_ELEMENTS(angle) EQ 0 then angle=90
;
nbp=2*nb_tops
angles=(360. / nbp * FINDGEN(nbp+1) + angle) / !RADEG
;
r=angles*0
r[2*INDGEN(nb_tops+1)]=1.
cp5=COS(!pi/nb_tops)
r1=2. * cp5 - 1. / cp5
r[2*INDGEN(nb_tops)+1]=r1
r=r * SQRT(!pi/4.) * 2. / (1.+r1)
;
if KEYWORD_SET(ratio) then begin
   maxi=MAX(r) & mini=MIN(r)
   r=r/maxi
   r[WHERE(r LT MEAN(r))]=ratio
endif
;
x=r * COS(angles) 
y=r * SIN(angles)
;
USERSYM, x, y, fill=fill;, color=color, thick=thick, fill=fill
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro DEMO_SYM_STAR, fill=fill, ps=ps, svg=svg
;
if KEYWORD_SET(svg) then begin
   set_plot, 'SVG'
   device, file='toto.svg'
endif
;
ratios=0.01+FINDGEN(11)/10.
;
plot, ratios, /nodata, xrange=[0, 16], yrange=[-0.2, 1.3], $
      /ystyle, /xstyle, xtitle='number of branches', $
      ytitle='value of ratio= keyword'
;
for rr=0, N_ELEMENTS(ratios)-1 do begin
   for nbb=1, 15 do begin
      SYM_STAR, nbb, ratio=ratios[rr], fill=fill
      plots, nbb, ratios[rr], PSYM=8, symsize=5
   endfor
endfor
;
; defaut ratio
;
for nbb=1, 15 do begin
   SYM_STAR, nbb
   plots, nbb, 1.15, PSYM=8, symsize=5
endfor
   
end
;
; ---------------------------------------------------
;
pro MY_USERSYM, type, color=color, thick=thick, fill=fill, $
                help=help, verbose=verbose, test=test
;
if type EQ 'square' then begin
   x=[0,1,1,0,0]
   y=[0,0,1,1,0]
   x=2.*(x-0.5)
   y=2.*(y-0.5)
endif
;
if type EQ 'circle' then begin
   nbp=35
   A=FINDGEN(nbp) * (!PI*2/(nbp-1))
   x=COS(a)
   y=SIN(a)
endif
;
USERSYM, x, y, color=color, thick=thick, fill=fill
;
if KEYWORD_SET(demo) then plot, findgen(10), psym=8, syms=8
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_PLOTSYM, test=test
;
WINDOW, /free
;
colors_list=['ff0000'x, '00ff00'x, 'ffffff'x, $
             'ffff00'x, 'ff00ff'x, '00ffff'x]
;
plot, [-2,9], [0,5], /nodata, /xstyle, $
      title='Testin symbols in PLOTSYM in IDL Astro', $
      xtitle='index in PLOTSYM'
;
for ii=0, 8 do begin
   jj=ii
   PLOTSYM, ii
   plots, jj, 1, psym=8
   PLOTSYM, ii, color=colors_list[ii mod 6]
   plots, jj, 2, psym=8
   PLOTSYM, ii, color=colors_list[ii mod 6], /fill
   plots, jj, 3, psym=8, symsize=2
   PLOTSYM, ii, color=colors_list[ii mod 6], thick=3
   plots, jj, 4, psym=8, symsize=3
endfor
;
XYOUTS, -1, 1, 'default size', align=0.5
XYOUTS, -1, 2, 'default size; colors', align=0.5
XYOUTS, -1, 3, 'size=2; colors + fill', align=0.5
XYOUTS, -1, 4, 'size=3; colors', align=0.5
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
; We must to be sure than PLOT, OPLOT & PLOTS do
; the same symbols (size, color, fill & thick)
;
pro TEST_OPLOTS_USERSYM, test=test
;
WINDOW, /free
;
colors_list=['ff0000'x, '00ff00'x, '0000ff'x, $
             'ffff00'x, 'ff00ff'x, '00ffff'x]
;
MY_USERSYM, 'square'
;
plot, INDGEN(9), REPLICATE(1,9), psym=8, symsize=2, $
      xrange=[-2,9], yrange=[0,6], /xstyle, /ystyle, $
      title='Testing USERSYM symbols (oplot, plots)'
      xtitle='index in USERSYM '
plot, INDGEN(9), REPLICATE(1,9), psym=8, symsize=3,$
      xrange=[-2,9], yrange=[0,6], /xstyle, /ystyle, $
      /noerase
;
for ii=0, 8 do begin
   color=colors_list[ii mod 6]
   MY_USERSYM, 'square', color=color
   ;;
   for jj=0, 5 do PLOTS, ii, 2, psym=8, symsize=jj+0.1
   for jj=0, 5 do OPLOT, [ii], [3], psym=8, symsize=jj+0.1
   ;;
   ;; fill
   MY_USERSYM, 'circle', color=color, /fill
   ;;
   PLOTS, ii, 4, psym=8, symsize=4
   OPLOT, [ii], [5], psym=8, symsize=4
   ;;
   XYOUTS, -1, 1, 'PLOT', align=0.5
   XYOUTS, -1, 2, 'PLOTS', align=0.5
   XYOUTS, -1, 3, 'OPLOT', align=0.5
   XYOUTS, -1, 4, 'PLOTS', align=0.5
   XYOUTS, -1, 5, 'OPLOT', align=0.5
endfor
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_PLOT_USERSYM_COLOR, test=test
;
WINDOW, /free
;
DEVICE, /dec
color='ff00ff'x
;
; first call : no color, done via PLOT/OPLOT
MY_USERSYM, 'circle'
;
; un bug : IDL != GDL
; la taille du symbole doit etre gérée via USERSYM
;
!p.multi=[0,2,0]
;
PLOT, FINDGEN(10), psym=8, thick=3, sym=4
OPLOT, FINDGEN(10), psym=8, thick=1, color=color
;
; second plot : color is passed using USERSYM
MY_USERSYM, 'circle', thick=2, color=color
plot, FINDGEN(10), psym=8, symsize=5
;
!p.multi=0
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_PLOT_USERSYM, no_exit=no_exit, no_wdelete=no_wdelete, $
                       help=help, test=test, verbose=verbose
;
print, 'Just Visual tests'
;
TEST_PLOTSYM
TEST_OPLOTS_USERSYM
TEST_PLOTSYM
;
if KEYWORD_set(test) OR KEYWORD_set(no_exit) then STOP
;
if ~KEYWORD_set(no_wdelete) then begin
    print, 'please wait two sec., Window to be delete ...'
    WAIT, 2
    while !d.window GE 0 do WDELETE
endif
;
end

