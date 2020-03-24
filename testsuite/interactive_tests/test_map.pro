;
; A visual way to check some projections and Earth cartography.
;
; --------------------------------------------
; This code is under GNU GPL v2 or later.
;
; Creation by AC, 27-Feb-2007. updated 17-avr-2007
; 25-May2007 : 2 messages were removed since all the cases are now
; working for x86 and x86_64
;
; History
; AC 28-Jul-2008: 2 new projections provided by Joel.
;
; Spring 2014: large rewritting by Gilles D., adding a lot
; of examples linked to newly available projections in GDL
;
; 2014 July 30: some help to run this test !
;
; Purpose: A very simple test for MAP library
; (a way not to forgot how to call the mapping procedures !!)
;
; AC 2019-02-12 : few revisions,  better search for MAP_INSTALL
;
; GD 2019-03-10 : replaced all gshh* code with shapelib and 
;                 naturalearthdata.com shapefiles so removed any
;                 gshhsg related procedures and code. Added however
;                 new procedures to build a shapefile from the gshhsg data,
;                 in case it helps.
; GD 2019-12-20 : removed all searches for 'foreign' procedures as we
; have everything now within GDL.
; --------------------------------------------
;
function INTERNAL_GDL_MAP_LIBS
;
FORWARD_FUNCTION PROJ4_EXISTS, PROJ4NEW_EXISTS
;
status=0
;
;
if PROJ4_EXISTS() or PROJ4NEW_EXISTS() then return, 10

print, 'Unfortunalty, GDL was compiled without LibProj4 or libProj4New support !'
return,0

end
;
;
; ------------------------------------
;
pro INTERNAL_GDL_MAP_CHECK, test=test
;
ON_ERROR, 2
;
; Checking if GDL is compiled with one of the 2 projections libraries
;
status=INTERNAL_GDL_MAP_LIBS()
;
if (status Lt 10)  then begin
   MESSAGE, /continue, 'GDL without Projection support !'
   MESSAGE, 'please read the MAP_INSTALL document in the root of the GDL dir.'
   exit
endif
end
;
; ------------------------------------
;
function DoWeBreak, tictac=tictac, fill=fill
;
; when tictac set, we just wait a given time then continue
; (no interative question, no break)
;
if KEYWORD_SET(tictac) then begin
   WAIT, tictac
;   if KEYWORD_SET(fill) then begin
;      MAP_CONTINENTS, /fill
;      WAIT, tictac
;   endif
   return, 0
endif
;
;msg1='Press F for filling continents, Q to quit, any key or Enter to continue '
msg2='Press Q to quit, any key or Enter to continue '
;
key=''
;read, prompt=msg1, key
read, prompt=msg2, key
;
;if (STRUPCASE(key) EQ 'F') then begin
;   map_continents, /fill
;   read, prompt=msg2, key
;endif
;
if (STRUPCASE(key) EQ 'Q') then return, 1 else return, 0
;
end
;
; ------------------------------------
;
pro TEST_MAP, dir=dir, tictac=tictac, fill=fill
;
  INTERNAL_GDL_MAP_CHECK
;
print, 'Exemple 1: Should plot the whole "classical view" of the world with a grid'
map_set,/iso,e_cont={cont:1,fill:1,color:'33e469'x,hires:1},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x},e_grid={box_axes:1,color:'1260E2'x,glinethick:1,glinestyle:0,latdel:10,londel:15}
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 2: Should plot Europa using Gnomic Projection, scale 30,000,000'
map_set,50,15,/gnom,/iso,e_cont={cont:1,fill:1,color:'33e469'x,hires:1},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x},e_grid={box_axes:1,color:'1260E2'x,glinethick:1,glinestyle:0,latdel:10,londel:15},scale=3E7
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 2 bis: same with rivers and country boundaries'
map_set,50,15,/gnom,/iso,e_cont={cont:1,fill:1,color:'33e469'x,hires:1},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x},e_grid={box_axes:1,color:'1260E2'x,glinethick:1,glinestyle:0,latdel:10,londel:15}, scale=3E7
MAP_CONTINENTS, color='AAAA00'x, /river
MAP_CONTINENTS, color='000000'x, /countrie
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 4: Should plot North America using Gnomic Projection'
map_set,40,-105,/gnom,/iso,e_cont={cont:1,fill:1,color:'33e469'x,hires:1},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x},e_grid={box_axes:1,color:'1260E2'x,glinethick:1,glinestyle:0,latdel:10,londel:15}, scale=3E7
MAP_CONTINENTS, /USA, color='000000'x

if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 5: Should plot North America using Satellite Projection'
MAP_SET, 40,-105, /satellite, /grid, /cont,limit=[20,-130,70,-70], $
         sat_p=[2.22, 0, 0], title='Satellite Projection, North America'
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 6: Should plot World centered on Crozon peninsula using (GDL ONY) eckert1 projection -- Checking proj4 equivalent command used in the background'
map_set,name="eckert1",48.3,-4.5,/iso,e_cont={cont:1,fill:1,color:'33e469'x,hires:1},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x},e_grid={box_axes:1,color:'1260E2'x,glinethick:1,glinestyle:0,latdel:10,londel:15},/check
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 7: same zooming on Crozon, scale is 1:100000'
map_set,/ortho,48.3,-4.5,/iso,e_cont={cont:1,fill:1,color:'33e469'x,hires:1},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x},e_grid={box_axes:1,color:'1260E2'x,glinethick:1,glinestyle:0,latdel:1,londel:1},scale=1E6
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 8: Should plot World using (GDL ONY) Bertin projection'
map_set,name="Bertin",/iso,e_cont={cont:1,fill:1,color:'33e469'x,hires:1},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x},e_grid={box_axes:1,color:'1260E2'x,glinethick:1,glinestyle:0,latdel:10,londel:15}
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 9: Should plot World using CYLINDRICAL projection'
map_set,/CYL,/iso,e_cont={cont:1,fill:1,color:'33e469'x,hires:1},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x},e_grid={box_axes:1,color:'1260E2'x,glinethick:1,glinestyle:0,latdel:10,londel:15}
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 10: Should plot World using CONICAL projection'
map_set,/CONIC,standard_par=[30,60],/iso,e_cont={cont:1,fill:1,color:'33e469'x,hires:1},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x},e_grid={box_axes:1,color:'1260E2'x,glinethick:1,glinestyle:0,latdel:10,londel:15}
;
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
!P.MULTI=0
READ_JPEG,FILE_SEARCH(!GDL_MAPS_DIR, 'earth.jpg'),R
map_set,48.83,-2.33,name="goode",/iso
z=map_image(r,Startx,Starty)
tv,z,startx,starty
map_continents
print, 'last demo done'
;
go_to_end:
;
if KEYWORD_SET(test) then STOP
;
end
