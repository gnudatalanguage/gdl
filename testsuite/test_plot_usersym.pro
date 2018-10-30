;
; This test verifys whether PLOT, PLOTS & OPLOT
; are able to plot USERSYM with color, fill & simsize
;
; Alain C., August 2017, at JSC during Harvey
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-Oct-30 : AC.
; * merging "test_usersym.pro" in this file,
;   renaming the function into as TEST_USERSYM_NAN
; * cleaning & adding some cases (+ ps & svg)
;
; ---------------------------------------------------
;
; I <<stole>> this snipet in "plotsym.pro" in IDLastro lib.
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
pro TEST_DEMO_SYM_STAR, fill=fill, ps=ps, svg=svg
;
soft=GDL_IDL_FL()
;
; SVG is only for GDL
if KEYWORD_SET(svg) AND (soft NE 'GDL') then begin
   MESSAGE, /continue, 'SVG output not available in '+soft
   return
endif
;
prefix=soft
if KEYWORD_SET(fill) then prefix=prefix+'_fill'
;
if KEYWORD_SET(SVG) or KEYWORD_SET(PS) then begin
   save_p=!p
   save_dname=!d.name
endif else begin
   WINDOW, /free, title=GDL_IDL_FL(/title)+ROUTINE_NAME()
endelse
;
; SVG is only for GDL
if KEYWORD_SET(svg) then begin
   set_plot, 'SVG'
   device, file=prefix+'_'+ROUTINE_NAME(/lower)+'.svg'
endif
;
if KEYWORD_SET(PS) then begin
   set_plot, 'PS'
   device, file=prefix+'_'+ROUTINE_NAME(/lower)+'.ps'
endif
;
ratios=0.01+FINDGEN(11)/10.
;
if KEYWORD_SET(fill) then status=' : /fill' else status=' : no /fill'
;
PLOT, ratios, /nodata, xrange=[0, 16], yrange=[-0.2, 1.3], $
      /ystyle, /xstyle, xtitle='number of branches', $
      ytitle='value of ratio= keyword', $
      title=GDL_IDL_FL(/title)+ROUTINE_NAME()+status
;
for rr=0, N_ELEMENTS(ratios)-1 do begin
   for nbb=1, 15 do begin
      SYM_STAR, nbb, ratio=ratios[rr], fill=fill
      PLOTS, nbb, ratios[rr], PSYM=8, symsize=5
   endfor
endfor
;
; defaut ratio
;
for nbb=1, 15 do begin
   SYM_STAR, nbb
   plots, nbb, 1.15, PSYM=8, symsize=5
endfor
;
if KEYWORD_SET(SVG) or KEYWORD_SET(PS) then begin
   device, /close
   ;; reset the initial !p env.
   !p=save_p
   SET_PLOT, save_dname
endif
if KEYWORD_SET(test) then STOP
;
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
; we need to have the AstroLib in the PATH to run it
;
if ~EXECUTE('PLOTSYM, 0') then begin
   MESSAGE, /continue, 'AstroLib should be in the PATH to run it'
   return
endif
;
WINDOW, 0, title=GDL_IDL_FL(/title)+ROUTINE_NAME()
;
colors_list=['ff0000'x, '00ff00'x, 'ffffff'x, $
             'ffff00'x, 'ff00ff'x, '00ffff'x]
;
plot, [-2,9], [0,5], /nodata, /xstyle, $
      title='Testing symbols in PLOTSYM in IDL Astro', $
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
device, /decomposed
WINDOW, /free, title=GDL_IDL_FL(/title)+ROUTINE_NAME()
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
WINDOW, /free, title=GDL_IDL_FL(/title)+ROUTINE_NAME()
;
DEVICE, /decomposed
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
; ----------------------------------
; Alain Coulais, 28/11/2011
; Help tracking bug related to USERSYM
; this procedure is needed by the followin one : TEST_USERSYM_NAN
;
pro INTERNAL_PLOT, nbp, title=title, psym=psym, linestyle=linestyle, NaN=NaN
;
if N_PARAMS() EQ 0 then nbp=2
;
x=FINDGEN(nbp)/nbp
y=x
;
if KEYWORD_SET(NaN) and (nbp GT 4) then begin
   x[4]=!values.f_nan
   x[6]=!values.f_nan
   x[7]=!values.f_nan
endif
;
plot, x,y, psym=psym, xrange=[-0.25,1.], yrange=[-0.25,1.], $
      title=title, /xstyle, /ystyle
;
;print, '==========================================='
;
end
;
; ----------------------------------
;
pro TEST_PLOT_USERSYM_NAN, psym=psym, help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_USERSYM, psym=psym, help=help, test=test'
   return
end
;
if N_ELEMENTS(psym) EQ 0 then psym=8
;
; square hat
UserSym, [-1, -0.5, -0.5, 0.5, 0.5, 1, -1], $
         [-0.7, -0.7, 0.7, 0.7, -0.7, -0.7, -0.7]
;
!p.multi=[0,2,4]
xy=GET_SCREEN_SIZE()
WINDOW, /free, title=GDL_IDL_FL(/title)+ROUTINE_NAME(), $
        xsize=FIX(xy[0]/3), ysize=FIX(xy[1]*0.9)
;
INTERNAL_PLOT, 1, psym=psym, title='1 point'
INTERNAL_PLOT, 2, psym=psym, title='2 points'
INTERNAL_PLOT, 3, psym=psym, title='3 points'
INTERNAL_PLOT, 3, psym=-psym, title='3 points + linestyle', line=2
;
INTERNAL_PLOT, 12, psym=psym, title='12 points'
INTERNAL_PLOT, 12, psym=-psym, title='12 points + linestyle', line=2
INTERNAL_PLOT, 12, psym=psym, title='NaN + 12 points', /nan
INTERNAL_PLOT, 12, psym=-psym, title='NaN + 12 points + linestyle', line=2, /nan
;
!p.multi=0
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_PLOT_USERSYM, no_exit=no_exit, $
                       help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_PLOT_USERSYM, no_exit=no_exit, $'
   print, '                       help=help, test=test, verbose=verbose'
   return
endif
;
print, 'Just Visual tests'
;
rname=ROUTINE_NAME()
;
if ~CHECK_IF_DEVICE_IS_OK(rname, /force) then begin
   if ~KEYWORD_SET(no_exit) then EXIT, status=77 else STOP
endif
;
;TEST_PLOTSYM
TEST_OPLOTS_USERSYM
TEST_PLOT_USERSYM_COLOR
TEST_PLOT_USERSYM_NAN
;
TEST_DEMO_SYM_STAR
TEST_DEMO_SYM_STAR, /fill
TEST_DEMO_SYM_STAR, /ps
TEST_DEMO_SYM_STAR, /ps, /fill
TEST_DEMO_SYM_STAR, /svg
TEST_DEMO_SYM_STAR, /svg, /fill
;
if KEYWORD_SET(test) OR KEYWORD_SET(no_exit) then STOP
;
if ~KEYWORD_set(no_exit) then begin
    print, 'please wait one sec., Window to be delete ...'
    WAIT, 1
    while !d.window GE 0 do WDELETE
endif
;
end
