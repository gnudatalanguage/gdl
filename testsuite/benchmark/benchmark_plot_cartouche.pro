;
; This is a derivated work from a non public library
; (under GPL v2).  (subset) Alain Coulais, July 2017.
;
; Modification history :
;
; * AC 2017-08-07 : first public release
; - all the positionning is now done using /Normal
; - all the Xlog/Ylog mess is now processed easily
;
; -----------------------------------------
; Creation par AC le 25/11/2005 a Paris
;
; Purpose: ajout facile d'une legende composee d'un titre,
; et d'une serie de valeurs, en associant a chaque valeur
; son type de trace (bout de ligne, avec couleur, linestyle, psym et thick)
;
; -----------------------------------------
; Historique des modifications:
;
; AC le 14/12/05:
; -- modification du calcul de l'echelle
; -- verification dans AC_LEGENDE_DEMO avec un cadre
; -- ajour de l'option "title"
;
; AC le 15/12/2005 (au LAL, bat 208):
; -- ajout de l'option BOX
; -- gestion plus fine des _Extra (thick, charsize, charthick)
;
; AC le 04/02/2009:
; -- on verifie si on n'est pas hors-echelle !
; -- on gere les echelles Lin et Log ...
;
; AC le 06/02/2009
; -- on essaie un positionning a la LaTeX y=[tcb] x=[lcr]
; -- on peut desormais passer des info incompletes aux champs Color
;    et LineStyle, ca sera periodise ...
;
; AC le 08/12/2009
; -- on renomme "colors" en "colors_list"
; -- on renomme "linestyle" en "linestyle_list"
; -- on ajoute le champ "psym_list"
;
; AC le 09/02/2010 (a JPL)
; -- on gere proprement le cas YLOG (pas tres joli sinon)
; -- on ajoute le cas /YLOG dans AC_LEGENDE_DEMO
;
; -----------------------------------------
; Advices:
; Des parametres peuvent etre geres soit par un chgt externe de !p,
; ou bien par un passage par parametre "extra"
; -----------------------------------------
; Usage:
; -- see AC_LEGENDE_DEMO ci-bas.
; -----------------------------------------
;
function IS_FIELD_IN_STRUCT, struct_name, field_name
;
liste_flag=STRPOS(TAG_NAMEs(struct_name),STRUPCASE(field_name))
;
flag=0
liste=WHERE((liste_flag GE 0), count)
if (count EQ 1) then flag=1
if (count GT 1) then MESSAGE, 'Tag_names was found several times ! '+field_name
;
return, flag
end
; -----------------------------------------
;
function AC_SETMESSAGES, name_of_proc
;
mess_fatal   ='Fatal in "'+name_of_proc+'" : '
mess_warning ='Warning in "'+name_of_proc+'" : '
mess_info    ='Info in "'+name_of_proc+'" : '
mess_question='Question in "'+name_of_proc+'" : '
;
messages={name: name_of_proc, fatal: mess_fatal, warning:mess_warning, $
          info: mess_info, question: mess_question }
return, messages
end
;
;
function RESCALE4CARTOUCHE, xypos, debug=debug, test=test
;
if KEYWORD_SET(debug) then print, 'in  :', xypos
; mandatory forced conversion to avoid INT errors ! 
; a=[1,2] & a[1]=!pi & print, a  ... 1,3 ...
; 
xypos_norm=DOUBLE(xypos)
;
xrange=!x.crange[1]-!x.crange[0]
if !x.type EQ 1 then xrange=10^xrange
xscale=!x.window[1]-!x.window[0]
xypos_norm[0]=!x.window[0]+(xscale/xrange)*xypos[0]
xypos_norm[1]=!x.window[0]+(xscale/xrange)*xypos[1]
;
yrange=!y.crange[1]-!y.crange[0]
if !y.type EQ 1 then yrange=10^yrange
yscale=!y.window[1]-!y.window[0]
xypos_norm[2]=!y.window[0]+(yscale/yrange)*xypos[2]
xypos_norm[3]=!y.window[0]+(yscale/yrange)*xypos[3]
;
if KEYWORD_SET(debug) then print, 'out :', xypos_norm
;
if KEYWORD_SET(test) then STOP
;
return, xypos_norm
;
end
;
; -----------------------------------------
;
pro BENCHMARK_PLOT_CARTOUCHE, messages, positions=positions, title=title, $
                          colors_list=colors_list, $
                          linestyle_list=linestyle_list, $
                          psym_list=psym_list, epsilon_margin=epsilon_margin, $
                          allow_extendscale=allow_extendscale, $
                          ratio=ratio, box=box, _extra=_extra, $
                          verbose=verbose, debug=debug, test=test, help=help
;
mess=AC_SETMESSAGES('BENCHMARK_PLOT_CARTOUCHE')
;
if (N_PARAMS() lt 1) then begin
    print, mess.fatal+'missing mandatory "messages list" arguments'
    help=1
endif
;
if KEYWORD_SET(help) then begin
   print, 'pro BENCHMARK_PLOT_CARTOUCHE, messages, positions=positions, title=title, $'
   print, '                colors_list=colors_list, $'
   print, '                linestyle_list=linestyle_list, $'
   print, '                psym_list=psym_list, epsilon_margin=epsilon_margin, $'
   print, '                allow_extendscale=allow_extendscale, $'
   print, '                ratio=ratio, box=box, _extra=_extra, $'
   print, '                verbose=verbose, debug=debug, test=test, help=help'
   print, ''
   return
endif
;
; positionnning
;
if ((N_ELEMENTS(positions) NE 1) AND (N_ELEMENTS(positions) NE 4)) then begin
   print, mess.warning, 'positionning information missing'
   print, mess.info, 'position=''{lcr}{tcb}'' or pos=[xmin,xmax,ymin,ymax]'
   print, mess.info, 'default positioning used ''tr'' ...'
   positions='rt'
endif
;
; managing the case where the description of the box is LaTeX like
; e.g. : pos='tr' (top-right)
;
if (N_ELEMENTS(positions) EQ 1) then begin
   if (SIZE(positions,/type) NE 7) then begin
      print, mess.warning, 'bad type for one-element POSITIONS= keyword, we switch to default'
      positions='rt'
   endif
   positions=STRLOWCASE(positions)
   if (STRLEN(positions) EQ 1) then begin
      print, mess.warning,'only X OR Y provide, we switch the other to default'
   endif
   xok=0
   yok=0
   info1=STRMID(positions,0,1) & info1=info1[0]
   info2=STRMID(positions,1,1) & info2=info2[0]
   ;;if KEYWORD_SET(debug) then begin & infoX='' & infoY='' & endif
   ;;if KEYWORD_SET(debug) then print, 'info1, info2 :', info1, info2
   if info1 EQ 'l' OR info1 EQ 'c' OR info1 EQ 'r' then begin
      xok=1
      infoX=info1
   endif
   if info2 EQ 't' OR info2 EQ 'c' OR info2 EQ 'b' then begin
      yok=1
      infoY=info2
   endif
   ;;if KEYWORD_SET(debug) then print, 'infoX, infoY, xok, yok', infoX, infoY, xok, yok
   ;; do we have interversion ? (swap)
   if ((xok EQ 0) OR (yok EQ 0)) then begin
      MESSAGE, /continue, 'Bad XY positionning provided, trying XY (swap)'
      tmp=info1
      info1=info2
      info2=tmp
      if KEYWORD_SET(debug) then print, 'info1, info2 :', info1, info2
      if info1 EQ 'l' OR info1 EQ 'c' OR info1 EQ 'r' then begin
         xok=1
         infoX=info1
      endif
      if info2 EQ 't' OR info2 EQ 'c' OR info2 EQ 'b' then begin
         yok=1
         infoY=info2
      endif     
   endif
   if (xok EQ 0) then begin
      infoX='r'
      MESSAGE, /continue, 'Bad X positionning provided, default R used'
   endif
   if (yok EQ 0) then begin
      infoY='t'
      MESSAGE, /continue, 'Bad Y positionning provided, default T used'
   endif
   ;;if KEYWORD_SET(debug) then print, 'infoX, infoY, xok, yok', infoX, infoY, xok, yok
   ;;if KEYWORD_SET(debug) then STOP
   ;;
   ;; in this case, we don't care of real data,
   ;; we just use coordinates related to full plot
   xdata_min=!x.window[0]
   xdata_max=!x.window[1]
   ydata_min=!y.window[0]
   ydata_max=!y.window[1]
   ;;
   xthird=(xdata_max-xdata_min)/3.
   if (infoX EQ 'l') then xmin=xdata_min
   if (infoX EQ 'l') then xmax=xdata_min+xthird
   if (infoX EQ 'c') then xmin=xdata_min+xthird
   if (infoX EQ 'c') then xmax=xdata_min+xthird*2.
   if (infoX EQ 'r') then xmin=xdata_min+xthird*2.
   if (infoX EQ 'r') then xmax=xdata_max
   ;;
   ythird=(ydata_max-ydata_min)/3.
   if (infoY EQ 'b') then ymin=ydata_min
   if (infoY EQ 'b') then ymax=ydata_min+ythird
   if (infoY EQ 'c') then ymin=ydata_min+ythird
   if (infoY EQ 'c') then ymax=ydata_min+ythird*2.
   if (infoY EQ 't') then ymin=ydata_min+ythird*2.
   if (infoY EQ 't') then ymax=ydata_max
   ;;
   ;; reducing sligthly the box ...
   if ~KEYWORD_SET(epsilon_margin) then epsi=0.02
   ;;
   xmin=xmin+epsi
   xmax=xmax-epsi
   ymin=ymin+epsi
   ymax=ymax-epsi
   xy_pos=[xmin,xmax,ymin,ymax]
   ;;
   set_normal=1
   if KEYWORD_SET(debug) then print, xy_pos
endif
;
; ---------------------
if (N_ELEMENTS(positions) EQ 4) then begin
   ;;
   ;; What are the limits on the plot ?
   ;; managing Xlog and Ylog too :(
   if (!x.type EQ 1) then begin
      xdata_min=10^!x.crange[0]
      xdata_max=10^!x.crange[1]
   endif else begin
      xdata_min=!x.crange[0]
      xdata_max=!x.crange[1]
   endelse
   ;; managing Ylog 
   if (!y.type EQ 1) then begin
      ydata_min=10^!y.crange[0]
      ydata_max=10^!y.crange[1]
   endif else begin
      ydata_min=!y.crange[0]
      ydata_max=!y.crange[1]
   endelse
   ;;
   ;; managing X range
   xmin=positions[0]
   xmax=positions[1]
   ;;
   ;; checking X bondaries
   if (xmin LT xdata_min) then begin
      print, mess.warning+'X min position below Xmin scale ...'
      if ~KEYWORD_SET(allow_extendscale) then xmin=xdata_min
   endif
   if (xmax GT xdata_max) then begin
      print, mess.warning+'Y max position above Xmax scale ...'
      if ~KEYWORD_SET(allow_extendscale) then xmax=xdata_max
   endif
   ;;
   ;; managing Y range
   ymin=positions[2]
   ymax=positions[3]
   ;; checking Y bondaries
   if (ymin LT ydata_min) then begin
      print, mess.warning+'Y min position below Ymin scale ...'
      if ~KEYWORD_SET(allow_extendscale) then ymin=ydata_min
   endif
   if (ymax GT ydata_max) then begin
      print, mess.warning+'Y max position above Ymax scale ...'
      if ~KEYWORD_SET(allow_extendscale) then ymax=ydata_max
   endif
   ;;
   xy_pos=[xmin,xmax,ymin,ymax]
   ;;
   ;; if KEYWORD_SET(debug) then print, !x.crange, !y.crange
   ;; if KEYWORD_SET(debug) then print, xy_pos
   ;;
   ;; transforming in Normal coordinates the XY positions
   xy_pos=RESCALE4CARTOUCHE(xy_pos, debug=debug)
   ;;if KEYWORD_SET(debug) then print, 'revisited :', xy_pos
   xmin=xy_pos[0]
   xmax=xy_pos[1]
   ymin=xy_pos[2]
   ymax=xy_pos[3]
   ;;
   set_normal=1
endif
;
if N_ELEMENTS(ratio) EQ 0 then ratio=0.5
;
xmid=xmin+(xmax-xmin)*ratio
xdx=(xmax-xmin)/20.
;
; Gestion de !P et des _extra
; si le parametre Thick est passe, on modifie aussi
; la taille des caracteres !
;
sysvar_modified=0
if (SIZE(_extra))(0) GT 0 then begin
    sysvar_modified=1
    copy_p=!p
    if IS_FIELD_IN_STRUCT(_extra, 'thick') then !p.thick=_extra.thick
    if IS_FIELD_IN_STRUCT(_extra, 'charthick') then !p.charthick=_extra.charthick
    if IS_FIELD_IN_STRUCT(_extra, 'charsize') then !p.charsize=_extra.charsize
endif
;
; il n'y a qu'un titre (a ce jour)
; il faut recalculer le y_max
;
nbp=N_ELEMENTS(messages)
;
if KEYWORD_SET(title) then begin
   dy=(ymax-ymin)/FLOAT(nbp+1)
   ymax=ymin+dy/2.+dy*nbp
   ;;
   XYOUTS, normal=set_normal, xmid, ymax-dy/3., title, ALIGNMENT=0.5
   ;;
   if KEYWORD_SET(box) then begin
      ;; trait sous le titre
      y_subtitle=ymax-dy/2.
      PLOTS, normal=set_normal, [xmin,xmax], [y_subtitle,y_subtitle]
   endif
endif else begin
   dy=(ymax-ymin)/FLOAT(nbp)
endelse
;
; checking the size of optional fields
;
if N_ELEMENTS(colors_list) EQ 0 then liste_col=REPLICATE(!P.color, nbp)
if N_ELEMENTS(colors_list) EQ 1 then liste_col=REPLICATE(colors_list, nbp)
if N_ELEMENTS(colors_list) GT 1 then liste_col=colors_list
nbp_color=N_ELEMENTS(liste_col)
;
if N_ELEMENTS(linestyle_list) EQ 0 then liste_lsty=REPLICATE(!P.linestyle, nbp)
if N_ELEMENTS(linestyle_list) EQ 1 then liste_lsty=REPLICATE(linestyle_list, nbp)
if N_ELEMENTS(linestyle_list) GT 1 then liste_lsty=linestyle_list
nbp_lsty=N_ELEMENTS(liste_lsty)
;
if N_ELEMENTS(psym_list) EQ 0 then liste_psym=REPLICATE(!P.psym, nbp)
if N_ELEMENTS(psym_list) EQ 1 then liste_psym=REPLICATE(psym_list, nbp)
if N_ELEMENTS(psym_list) GT 1 then liste_psym=psym_list
nbp_psym=N_ELEMENTS(liste_psym)
;
;if KEYWORD_SET(debug) then STOP
;
for j=0, nbp-1 do begin
   ;; pour garder l'ordre actuel (du plus petit au plus grand)
   y=ymin+dy/2.+dy*(nbp-1-j)
   yy=y-dy/16.
   ;;
   PLOTS, normal=set_normal, [xmin+xdx,xmid], [y,y], _extra=_extra, $
          color=liste_col[j mod nbp_color], $
          line=liste_lsty[j mod nbp_lsty], $
          psym=liste_psym[j mod nbp_psym]
   XYOUTS, xmid+xdx, yy, messages[j], normal=set_normal
endfor
;
; eventuel trace de la boite encadrant la legende
;
if KEYWORD_SET(box) then begin
   PLOTS, [xmin,xmax], [ymin,ymin], normal=set_normal ;; trait bas horizontal
   PLOTS, [xmin,xmax], [ymax,ymax], normal=set_normal ;; trait haut horizontal
   PLOTS, [xmin,xmin], [ymin,ymax], normal=set_normal ;; premier trait vertical
   PLOTS, [xmax,xmax], [ymin,ymax], normal=set_normal ;; second trait vertical
endif
;
; remise en etat de !P
if (sysvar_modified EQ 1) then !P=copy_p
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------
;
pro BENCHMARK_PLOT_CARTOUCHE_DEMO, help=help, test=test, ylog=ylog, xlog=xlog
;
if KEYWORD_SET(help) then begin
   print, 'pro BENCHMARK_PLOT_CARTOUCHE_DEMO, help=help, test=test, ylog=ylog, xlog=xlog'
   return
endif
;
messages=['red','green','blue','yellow']
colors=['ff'x,'ff00'x,'ff0000'x,'00ffff'x]
lines=[1,2,3,4]
psyms=[1,2,3,4]*(-1)
;
data=FINDGEN(100)
;
plot, data, /nodata, xlog=xlog, ylog=ylog;, yrange=[0.1,100]
;
;
;if KEYWORD_SET(ylog) then begin
;   plot, data, /nodata, /ylog, yrange=[0.1,100]
;endif else begin
;   plot, data, /nodata
;endelse
for ii=0,3 do OPLOT, data+ii, col=colors[ii], psym=psyms[ii], lines=lines[ii]
;
BENCHMARK_PLOT_CARTOUCHE, pos=100.*[0.5,0.9,0.5,0.9], messages, /box, $
            colors=colors, lines=lines, thick=1.5, title='coucou', $
            psym=psyms
;
; 2 fois plus de traits, une fois avec titre, une fois sans titre
;
messages=[messages,messages]
colors=[colors,colors]
lines=[lines,lines]
;
xmin=15
xmax=30
ymin=5
ymax=50
;
pref=!p
!p.thick=3.
!p.charsize=2.
BENCHMARK_PLOT_CARTOUCHE, pos=[xmin,xmax,ymin,ymax], messages, /box, $
  colors=colors, lines=lines, charsize=0.75
!p=pref
;
xmin=40
xmax=50
ymin=5
ymax=50
;
BENCHMARK_PLOT_CARTOUCHE, pos=[xmin,xmax,ymin,ymax], messages, /box, $
            colors=colors, lines=lines, thick=1.5, title='coucou'
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro BENCHMARK_PLOT_CARTOUCHE_DEMO_BASIC, xlog=xlog, ylog=ylog
;
plot, findgen(1000)+1000,/nodata, xlog=xlog, ylog=ylog
;
pos=['lb','lc','lt','cb','cc','ct','rb','rc','rt']
col=['ffff00'x,'ff00ff'x,'00ffff'x]
col=[col,SHIFT(col,1),SHIFT(col,2)]
title=['','titre','']
title=[title,SHIFT(title,1),SHIFT(title,2)]
;
for ii=0, n_elements(pos)-1 do begin
   BENCHMARK_PLOT_CARTOUCHE, string(indgen(1+(ii+2) mod 5)), $
                         pos=pos[ii], col=col[ii], box= (ii mod 2), $
                         title=title[ii]
endfor
;
end
;
; -----------------------------------------------
;
pro BENCHMARK_PLOT_CARTOUCHE_DEMO_ALL
;
window, 0
BENCHMARK_PLOT_CARTOUCHE_DEMO
window, 1
BENCHMARK_PLOT_CARTOUCHE_DEMO, /xlog
window, 2
BENCHMARK_PLOT_CARTOUCHE_DEMO, /ylog
window, 3
BENCHMARK_PLOT_CARTOUCHE_DEMO, /xlog, /ylog
;
end
