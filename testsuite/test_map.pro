;
; AC, le 27-fev-2007. updated 17-avr-2007
; 25-May2007 : 2 messages were removed since all the cases are now
; working for x86 and x86_64
;
; History
; AC 28-Jul-2008: 2 new projections provided by Joel.
;
; Purpose: A very simple test for MAP library
; (a way not to forgot how to call the mapping procedures !!)
;
pro TEST_MAP, dir=dir
;
print, 'please read the MAP_INSTALL document in the root of the GDL dir.'
;
print, 'please remember that:'
print, '0 : GDL must have been compiled with LibProj4'
print, '     --> http://members.verizon.net/~vze2hc4d/proj4/'
print, '1 : the low res maps must be in a resource/maps/low PATH'
print, '2 : the !dir must go to root of resource/maps/low PATH'
print, '   e.g. /opt/GDL/gdl-0.9pre4/ if you have /opt/GDL/gdl-0.9pre4/resource/maps/low'
print, '   e.g. /usr/local/rsi/idl_5.5/ if you have /usr/local/rsi/idl_5.5/resource/maps/low'
print, '3 : map_*.pro lib. must be in the GDL path (see MAP_INSTALL doc.)'
;
if (N_ELEMENTS(dir) EQ 1) then DEFSYSV, '!dir', dir
print, 'Current !dir is:', !dir
;
are_data_here=FILE_SEARCH(!dir, 'plow.dat',count=c)
if (c EQ 0) then begin
   print, '*****************************'
   print, 'It is highly probable that the MAP data are not in the !dir PATH'
   print, '*****************************'
endif
new_dir=''
read, 'New !dir ? [ret if no change]', new_dir
if (STRLEN(new_dir) GT 0) then DEFSYSV, '!dir', new_dir
;
times=FINDGEN(7)
;
;AC 25/05/2007 no more useful, bug corrected in CVS by Joel
;; need for future warning messages
;defsysv,'!gdl',exist=is_it_gdl
;
;  35,68° N, 139,76° O (cf TEN/SIXTY in AstronLib)
;
t0=SYSTIME(1)
print, 'Exemple 1: Should plot the whole world centered on the Japan time line'
MAP_SET, 0, 139, /continent, title='Continents'
times[0]=SYSTIME(1)-t0
suite='' & read, 'Press Enter', suite
;
t0=SYSTIME(1)
print, 'Exemple 2: Should plot the whole world centered on the North America (with rotation)'
MAP_SET, 40,-105, /continent, title='Continents (rotated)'
times[1]=SYSTIME(1)-t0
suite='' & read, 'Press Enter', suite
;
t0=SYSTIME(1)
print, 'Exemple 3: Should plot the whole "classical view" of the world with a grid'
MAP_SET, 0, 0, /continent, /grid, title='Continents, centered on Greenwitch Meridian'
times[2]=SYSTIME(1)-t0
suite='' & read, 'Press Enter', suite
;
t0=SYSTIME(1)
print, 'Exemple 4: Should plot Europa centered on Paris Observatory using Gnomic Projection'
MAP_SET,/cont, 48.83,2.33,/grid,/gnomic,/iso, scale=3.e7, title='Zoom on Europa, Gnomic Projection'
times[3]=SYSTIME(1)-t0
suite='' & read, 'Press Enter', suite
;
t0=SYSTIME(1)
print, 'Exemple 5: Should plot North America using Gnomic Projection'
MAP_SET,/gnomic,/iso,40,-105,/cont, limit=[20,-130,70,-70], title='Gnomic, North America'
times[4]=SYSTIME(1)-t0
suite='' & read, 'Press Enter', suite
;
t0=SYSTIME(1)
print, 'Exemple 6: Should plot North America using Satellite Projection'
MAP_SET, 40,-105,/satellite,/grid,/cont,limit=[20,-130,70,-70], sat_p=[2.22, 0, 0], title='Satellite Projection, North America'
times[5]=SYSTIME(1)-t0
suite='' & read, 'Press Enter', suite
;
t0=SYSTIME(1)
print, 'Exemple 7: Should plot World using Robinson projection'
MAP_SET,0,180,/robinson,/grid,/cont,/iso,titl='Robinson Projection, Centered on Greenwich Meridian'
times[6]=SYSTIME(1)-t0
;
print, 'last demo done'
;
print, 'times:', times
;
if KEYWORD_SET(test) then STOP
;
end

;AC 25/05/2007 no more useful, bug corrected in CVS by Joel
;if (is_it_gdl EQ 1) then print, 'WARNING AC 17/04/2007: not working in GDL on x86_64'
