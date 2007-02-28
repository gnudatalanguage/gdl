;
; AC, le 27-fev-2007
;
; A very simple test for MAP library
; (not to forgot !)
;
pro TEST_MAP
;
print, 'please read the MAP_INSTALL document in the root of the GDL dir.'
;
print, 'please remember that:'
print, '0 : GDL must have been compiled with LibProj4'
print, '     --> http://members.verizon.net/~vze2hc4d/proj4/'
print, '1 : the low res maps must be in a resource/maps/low PATH'
print, '2 : the !dir must go to root of resource/maps/low PATH'
print, '   e.g. /opt/GDL/gdl-0.9pre4 if you have /opt/GDL/gdl-0.9pre4/resource/maps/low'
print, '3 : map_*.pro lib. must be in the GDL path (see MAP_INSTALL doc.)'
;
print, 'Current !dir is:', !dir
new_dir=''
read, 'New !dir ? [ret if no change]', new_dir
if (STRLEN(new_dir) GT 0) then DEFSYSV, '!dir', new_dir
;
;  35,68° N, 139,76° O (cf TEN/SIXTY in AstronLib)
;
print, 'Should plot the whole world centered on the Japan time line'
map_set, 0, 139, /continent
suite='' & read, 'Press Enter', suite
;
print, 'Should plot the whole world centered on the North America 
print, 'Please noticed this is rotated on IDL not in GDL'
map_set, 40,-105, /continent
suite='' & read, 'Press Enter', suite
;
print, 'Should plot the whole "classical view" of the world with a grid'
map_set, 0, 0, /continent, /grid
suite='' & read, 'Press Enter', suite
;
print, 'Should plot Europa centered on Paris Observatory'
map_set,/cont, 48.83,2.33,/grid,/gno,/iso, scale=3.e7
suite='' & read, 'Press Enter', suite
;
print, 'Should plot North America'
defsysv,'!gdl',exist=is_it_gdl
if (is_it_gdl EQ 1) then print, 'WARNING: not working in GDL on x86_64'
map_set,/gnomic,/iso,40,-105,/cont, limit=[20,-130,70,-70]
;
print, 'last demo done'
;
end
