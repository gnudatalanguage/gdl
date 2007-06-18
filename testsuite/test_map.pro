;
; AC, le 27-fev-2007. updated 17-avr-2007
; 25-May2007 : 2 messages were removed since all the cases are now
; working for x86 and x86_64
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
;
times=FINDGEN(5)
;
;AC 25/05/2007 no more useful, bug corrected in CVS by Joel
;; need for future warning messages
;defsysv,'!gdl',exist=is_it_gdl
;
;  35,68° N, 139,76° O (cf TEN/SIXTY in AstronLib)
;
t0=SYSTIME(1)
print, 'Exemple1: Should plot the whole world centered on the Japan time line'
map_set, 0, 139, /continent, title='continent'
times[0]=SYSTIME(1)-t0
suite='' & read, 'Press Enter', suite
;
t0=SYSTIME(1)
print, 'Exemple2: Should plot the whole world centered on the North America'
;AC 25/05/2007 no more useful, bug corrected in CVS by Joel
;if (is_it_gdl EQ 1) then print, 'WARNING AC 17/04/2007: not working in GDL on x86_64'
map_set, 40,-105, /continent, title='continent (rotated)'
times[1]=SYSTIME(1)-t0
suite='' & read, 'Press Enter', suite
;
t0=SYSTIME(1)
print, 'Exemple3: Should plot the whole "classical view" of the world with a grid'
map_set, 0, 0, /continent, /grid
times[2]=SYSTIME(1)-t0
suite='' & read, 'Press Enter', suite
;
t0=SYSTIME(1)
print, 'Exemple4: Should plot Europa centered on Paris Observatory'
map_set,/cont, 48.83,2.33,/grid,/gno,/iso, scale=3.e7, title='Europa'
times[3]=SYSTIME(1)-t0
suite='' & read, 'Press Enter', suite
;
t0=SYSTIME(1)
print, 'Exemple5: Should plot North America'
;AC 25/05/2007 no more useful, bug corrected in CVS by Joel
;if (is_it_gdl EQ 1) then print, 'WARNING AC 17/04/2007: not working in GDL on x86_64'
times[4]=SYSTIME(1)-t0
map_set,/gnomic,/iso,40,-105,/cont, limit=[20,-130,70,-70]
;
print, 'last demo done'
;
print, 'times:', times
;
end
