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
suite='' & read, 'Accept by striking the Enter key', suite
;
print, 'Exemple 1: Should plot the whole world centered on the Japan time line'
MAP_SET, 0, 139, /continent, title='Continents'
suite='' & read, 'Press Enter', suite
;
print, 'Exemple 2: Should plot the whole world centered on the North America (with rotation)'
MAP_SET, 40,-105, /continent, title='Continents (rotated)'
suite='' & read, 'Press Enter', suite
;
print, 'Exemple 3: Should plot the whole "classical view" of the world with a grid'
MAP_SET, 0, 0, /continent, /grid, title='Continents, centered on Greenwitch Meridian'
suite='' & read, 'Press Enter', suite
;
print, 'Exemple 4: Should plot Europa centered on Paris Observatory using Gnomic Projection'
MAP_SET,/cont, 48.83,2.33,/grid,/gnomic,/iso, scale=3.e7, title='Zoom on Europa, Gnomic Projection'
suite='' & read, 'Press Enter', suite
;
print, 'Exemple 4bis: Should plot Europa centered on Paris Observatory using Gnomic Projection (+ rivers and countries)'
MAP_SET, 48.83,2.33,/grid,/gnomic,/iso, scale=3.e7, $
         title='Zoom on Europa, Gnomic Projection'
MAP_CONTINENTS, color='ffff00'x,/river
MAP_CONTINENTS, color='00ff00'x,/count
map_continents, colo='ffffff'x,/cont    


suite='' & read, 'Press Enter', suite
;
print, 'Exemple 5: Should plot North America using Gnomic Projection'
MAP_SET,/gnomic,/iso,40,-105,/cont, limit=[20,-130,70,-70], title='Gnomic, North America'
suite='' & read, 'Press Enter', suite


;
print, 'Exemple 6: Should plot North America using Satellite Projection'
MAP_SET, 40,-105,/satellite,/grid,/cont,limit=[20,-130,70,-70], sat_p=[2.22, 0, 0], title='Satellite Projection, North America'
suite='' & read, 'Press Enter', suite
;
print, 'Exemple 7: Should plot World using Robinson projection'
MAP_SET,0,180,/robinson,/grid,/cont,/iso,titl='Robinson Projection, Centered on Greenwich Meridian'
suite='' & read, 'Press Enter', suite
;
MAP_SET, /AITOFF, /ISOTROPIC,  TITLE='AITOFF'
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /AITOFF, 60, 180, 30, /ISOTROPIC,  TITLE='AITOFF OFFSET ROTATION'
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /ALBERS, /ISOTROPIC,  TITLE='ALBERS',STANDARD_PARALLELS=[-20,10]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /ALBERS, 30, 180, 45,/ISOTROPIC,  TITLE='ALBERS OFFSET ROTATED',STANDARD_PARALLELS=[-20,10]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /AZIM, /ISOTROPIC,  TITLE='AZIM',STANDARD_PARALLELS=[-20,10] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /AZIM, 30, 180, 45,/ISOTROPIC,  TITLE='AZIM OFFSET ROTATED',STANDARD_PARALLELS=[-20,10] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /CONIC, /ISOTROPIC,  TITLE='CONIC',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /CONIC, 30, 180, 45,/ISOTROPIC,  TITLE='CONIC OFFSET ROTATED',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /CYLIN, /ISOTROPIC,  TITLE='CYLIN',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /CYLIN, 30, 180, 45,/ISOTROPIC,  TITLE='CYLIN OFFSET ROTATED',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /GNOM, /ISOTROPIC,  TITLE='GNOM',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /GNOM, 30, 180, 45,/ISOTROPIC,  TITLE='GNOM OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /GOODES, /ISOTROPIC,  TITLE='GOODES',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /GOODES, 30, 180, 45,/ISOTROPIC,  TITLE='GOODES OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /HAMMER, /ISOTROPIC,  TITLE='HAMMER',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /HAMMER, 30, 180, 45,/ISOTROPIC,  TITLE='HAMMER OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /LAMBERT, /ISOTROPIC,  TITLE='LAMBERT',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /LAMBERT, 0, 180, 45,/ISOTROPIC,  TITLE='LAMBERT OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /MERCATOR, /ISOTROPIC,  TITLE='MERCATOR',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /MERCATOR, 30, 180, 45,/ISOTROPIC,  TITLE='MERCATOR OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /MILLER, /ISOTROPIC,  TITLE='MILLER',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /MILLER, 30, 180, 45,/ISOTROPIC,  TITLE='MILLER OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /MOLL, /ISOTROPIC,  TITLE='MOLL',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /MOLL, 30, 180, 45,/ISOTROPIC,  TITLE='MOLL OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /ORTHO, /ISOTROPIC,  TITLE='ORTHO',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /ORTHO, 30, 180, 45,/ISOTROPIC,  TITLE='ORTHO OFFSET ROTATED',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /ROBIN,/ISOTROPIC,  TITLE='ROBIN',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /ROBIN, 30, 180, 45,/ISOTROPIC,  TITLE='ROBIN OFFSET ROTATED',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /SATELL, SAT_P=[1.0251, 55, 150], 41.5, -74.,LIMIT=[39, -74, 33, -80, 40, -77, 41,-74], /ISOTROPIC,  TITLE='Satellite, Tilted Perspective'
MAP_CONTINENTS,/HIRES
MAP_GRID, /LABEL, LATLAB=-75, LONLAB=39, LATDEL=1, LONDEL=1
suite='' & read, 'Press Enter', suite
MAP_SET, /SINUS,/ISOTROPIC,  TITLE='SINUS',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /SINUS, 30, 180, 45,/ISOTROPIC,  TITLE='SINUS OFFSET ROTATED',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /STEREO, /ISOTROPIC,  TITLE='STEREO',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /STEREO, 30, 180, 45,/ISOTROPIC,  TITLE='STEREO OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
suite='' & read, 'Press Enter', suite
MAP_SET, /TRANSVER, /ISOTROPIC,  TITLE='TRANSVER',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
;MAP_GRID ; hyper long?
suite='' & read, 'Press Enter', suite
MAP_SET, /TRANSVER, 30, 180, 45,/ISOTROPIC,  TITLE='TRANSVER OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
;MAP_GRID
;suite='' & read, 'Press Enter', suite
;mapStruct = MAP_PROJ_INIT(129, LIMIT=[0,-180,90,180],SPHERE_RADIUS=1, CENTER_LONGITUDE=180)
suite='' & read, 'Press Enter', suite
image = BYTSCL(SIN(DIST(400)/10))
MAP_SET, 0, 0, /GOODE, /ISOTROPIC,TITLE='GOODE + REPROJECTED IMAGE'
result = MAP_IMAGE(image,Startx,Starty)
TV, result, Startx, Starty
MAP_CONTINENTS, /coasts
MAP_GRID, latdel=10, londel=10, /LABEL, /HORIZON
print, 'last demo done'
;
if KEYWORD_SET(test) then STOP
;
end

;AC 25/05/2007 no more useful, bug corrected in CVS by Joel
;if (is_it_gdl EQ 1) then print, 'WARNING AC 17/04/2007: not working in GDL on x86_64'
