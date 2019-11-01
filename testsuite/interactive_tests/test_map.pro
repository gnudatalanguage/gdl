;
; A visual way to check some projections and Earth cartography.
;
; Also a way to check whether GDL was compiled or not with LibProj4 or LibProj4New
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
; -------------------------------------------------
;
pro GET_MAP_PROCEDURES, path2file, quiet=quiet, verbose=verbose, $
                        help=help, debug=debug, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro GET_MAP_PROCEDURES, path2file, quiet=quiet, verbose=verbose, $'
   print, '                        help=help, debug=debug, test=test'
   return
endif
;
print, 'Starting GET_MAP_PROCEDURES'
;
filename='MAP_INSTALL'
;
if (N_PARAMS() EQ 0) then path2file=''
;
full_name=''
;
; we just try 3 basic locations
;
if FILE_TEST(path2file+filename) then full_name=path2file+filename
if (STRLEN(full_name) EQ 0) then begin
   testfile=path2file+PATH_SEP()+filename
   if FILE_TEST(testfile) then full_name=testfile
endif
if (STRLEN(full_name) EQ 0) then begin
   testfile='..'+PATH_SEP()+filename
   if FILE_TEST(testfile) then full_name=testfile
endif
;
if (STRLEN(full_name) EQ 0) then begin
   MESSAGE, /continue, "the file MAP_INSTALL was not found"
   MESSAGE, /continue, "please provide path two this file"
   return
endif
;
; recovering the list of missing map routines
;
get_lun, lun
openr, lun, full_name
cur_line=''
lines=''
while ~EOF(lun) do begin
   readf, lun, cur_line
   lines=[lines,cur_line]
endwhile
close, lun
free_lun, lun
;
filtre='http://idlastro'

OK=WHERE(STRPOS(lines, filtre) GE 0, nb_ok)
if nb_ok EQ 0 then MESSAGE, 'something wrong in MAP_INSTALL file !'
;
if KEYWORD_SET(debug) OR KEYWORD_SET(verbose) then begin
   print, 'OK indexes values : ', OK
   for ii=0, nb_ok-1 do begin
      print, lines[OK[ii]]
   endfor
   if KEYWORD_SET(debug) then STOP
endif
;
; do we need to had this path to the whole !path
;
dir_name='map_routines'
CD, current=old_current
new_current=old_current+PATH_SEP()+dir_name
;
; we check whether we already have the mandatory files around
;
list_of_dir=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
;
; do we already have a "map_routines" dir. here ??
exist=STRPOS(list_of_dir,'map_routine')
exist_ok=WHERE(exist GE 0, nb_exist_ok)
if nb_exist_ok EQ 0 then begin
   !path=!path+':'+new_current
   list_of_dir=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
endif
;
for ii=0, nb_ok-1 do begin
   fullfile=lines[OK[ii]]
   file=STRMID(fullfile,1+STRPOS(lines[ok[ii]],'/', /reverse_search))
   Result = FILE_SEARCH(list_of_dir+PATH_SEP()+file)
   if STRLEN(result) GT 0 then OK[ii]=-1
endfor
OK_tmp=WHERE(OK GE 0, nb_ok_tmp)
if (nb_ok_tmp EQ 0) then begin
   print, 'all mandatory routines already available'
   return
endif
;
; creating a sub-dir for Map routines
; updating the !path
;
CD, current=old_current
;
FILE_MKDIR, dir_name
;
CD, dir_name
CD, current=new_current
;
if KEYWORD_SET(debug) then STOP
;
; 
old_html='http://idlastro.gsfc.nasa.gov/ftp/exelislib/'
new_html='http://www.lancesimms.com/programs/IDL/lib/'
list_todo=lines[OK]
list_todo=new_html+STRMID(list_todo,STRLEN(new_html))
;
; downloading the files
GRAB_ON_INTERNET, list_todo, /quiet, /count
;for ii=0, N_ELEMENTS(list_todo)-1 do begin
;   spawn, "curl -O "+list_todo[ii]
;endfor
;
CD, old_current
;
if KEYWORD_SET(test) then STOP
;
end
;
function TEST_MAP_PROCEDURES
;
; do we have this procedure in GDL syntax in the PATH ?
;
test=EXECUTE("map_set")
;
if test EQ 0 then return, 0 else return, 1
;
end
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
   status=-1
endif
;
if (status LT 0) then begin
   MESSAGE, 'please read the MAP_INSTALL document in the root of the GDL dir.'
endif
;
; Getting the missing routines if needed.
; (based on MAP_INSTALL informations --> test of presence of this file
; not done now, ToDo)
;
status_map_pro=TEST_MAP_PROCEDURES()
;
if status_map_pro EQ 0 then GET_MAP_PROCEDURES
;
;
if KEYWORD_SET(test) then STOP
;
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
   if KEYWORD_SET(fill) then begin
      MAP_CONTINENTS, /fill
      WAIT, tictac
   endif
   return, 0
endif
;
msg1='Press F for filling continents, Q to quit, any key or Enter to continue '
msg2='Press Q to quit, any key or Enter to continue '
;
key=''
read, prompt=msg1, key
;
if (STRUPCASE(key) EQ 'F') then begin
   map_continents, /fill
   read, prompt=msg2, key
endif
;
if (STRUPCASE(key) EQ 'Q') then return, 1 else return, 0
;
end
;
; ------------------------------------
;
pro TEST_MAP, dir=dir, tictac=tictac, fill=fill
;
print, 'Exemple 1: Should plot the whole world centered on the Japan time line'
MAP_SET, 0, 139, /continent, title='Continents'
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 2: Should plot the whole world centered on the North America (with rotation)'
MAP_SET, 40,-105, /continent, title='Continents (rotated)'
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 3: Should plot the whole "classical view" of the world with a grid'
MAP_SET, 0, 0, /continent, /grid, title='Continents, centered on Greenwitch Meridian'
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 4: Should plot Europa centered on Paris Observatory using Gnomic Projection'
MAP_SET, /cont, 48.83,2.33, /grid, /gnomic, /iso, scale=3.e7, title='Zoom on Europa, Gnomic Projection'
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 4bis: Should plot Europa centered on Paris Observatory using Gnomic Projection (+ rivers and countries)'
MAP_SET, 48.83,2.33, /grid, /gnomic, /iso, scale=3.e7, $
         title='Zoom on Europa, Gnomic Projection'
MAP_CONTINENTS, color='ffff00'x, /river
MAP_CONTINENTS, color='00ff00'x, /countrie
MAP_CONTINENTS, colo='ffffff'x, /continents
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 5: Should plot North America using Gnomic Projection'
MAP_SET, /gnomic, /iso,40,-105, /cont, limit=[20,-130,70,-70], $
         title='Gnomic, North America'
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 6: Should plot North America using Satellite Projection'
MAP_SET, 40,-105, /satellite, /grid, /cont,limit=[20,-130,70,-70], $
         sat_p=[2.22, 0, 0], title='Satellite Projection, North America'
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
print, 'Exemple 7: Should plot World using Robinson projection'
MAP_SET,0,180, /robinson, /grid, /cont, /iso, $
        title='Robinson Projection, Centered on Greenwich Meridian'
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
ADVANCE=0 & !P.MULTI=0 ;ADVANCE=1 & !P.MULTI=[0,2,2]
MAP_SET, ADVANCE=ADVANCE, /AITOFF, /ISOTROPIC,  TITLE='AITOFF'
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /AITOFF, 60, 180, 30, /ISOTROPIC,  $
         TITLE='AITOFF OFFSET ROTATION'
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /ALBERS, /ISOTROPIC,  TITLE='ALBERS', $
         STANDARD_PARALLELS=[-20,10]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /ALBERS, 30, 180, 45, /ISOTROPIC, $
         TITLE='ALBERS OFFSET ROTATED', STANDARD_PARALLELS=[-20,10]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /AZIM, /ISOTROPIC,  TITLE='AZIM', $
         STANDARD_PARALLELS=[-20,10] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /AZIM, 30, 180, 45, /ISOTROPIC, $
         TITLE='AZIM OFFSET ROTATED',STANDARD_PARALLELS=[-20,10] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end

MAP_SET, ADVANCE=ADVANCE, /CONIC, /ISOTROPIC, TITLE='CONIC', $
         STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /CONIC, 30, 180, 45, /ISOTROPIC, $
         TITLE='CONIC OFFSET ROTATED', STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /CYLIN, /ISOTROPIC, TITLE='CYLIN', $
         STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /CYLIN, 30, 180, 45, /ISOTROPIC, $
         TITLE='CYLIN OFFSET ROTATED',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /GNOM, /ISOTROPIC,  TITLE='GNOM', $
         STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /GNOM, 30, 180, 45, /ISOTROPIC, $
         TITLE='GNOM OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /GOODES, /ISOTROPIC,  TITLE='GOODES', $
         STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /GOODES, 30, 180, 45, /ISOTROPIC, $
         TITLE='GOODES OFFSET ROTATED', STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /HAMMER, /ISOTROPIC, TITLE='HAMMER', $
         STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /HAMMER, 30, 180, 45, /ISOTROPIC, $
         TITLE='HAMMER OFFSET ROTATED', STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /LAMBERT, /ISOTROPIC,  TITLE='LAMBERT', $
         STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /LAMBERT, 0, 180, 45, /ISOTROPIC, $
         TITLE='LAMBERT OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /MERCATOR, /ISOTROPIC, TITLE='MERCATOR', $
         STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /MERCATOR, 30, 180, 45, /ISOTROPIC, $
         TITLE='MERCATOR OFFSET ROTATED', STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /MILLER, /ISOTROPIC,  TITLE='MILLER', $
         STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /MILLER, 30, 180, 45, /ISOTROPIC, $
         TITLE='MILLER OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /MOLL, /ISOTROPIC,  TITLE='MOLL', $
         STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /MOLL, 30, 180, 45, /ISOTROPIC, $
         TITLE='MOLL OFFSET ROTATED',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /ORTHO, /ISOTROPIC,  TITLE='ORTHO', $
         STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /ORTHO, 30, 180, 45, /ISOTROPIC, $
         TITLE='ORTHO OFFSET ROTATED',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
;
MAP_SET, ADVANCE=ADVANCE, /ROBIN, /ISOTROPIC,  TITLE='ROBIN', $
         STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /ROBIN, 30, 180, 45, /ISOTROPIC, $
         TITLE='ROBIN OFFSET ROTATED',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /SATELL, SAT_P=[1.0251, 55, 150], 41.5, -74., $
         LIMIT=[39, -74, 33, -80, 40, -77, 41,-74], /ISOTROPIC, $
         TITLE='Satellite, Tilted Perspective'
MAP_CONTINENTS, /HIRES
MAP_GRID, /LABEL, LATLAB=-75, LONLAB=39, LATDEL=1, LONDEL=1
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /SINUS, /ISOTROPIC,  TITLE='SINUS', $
         STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /SINUS, 30, 180, 45, /ISOTROPIC, $
         TITLE='SINUS OFFSET ROTATED',STANDARD_PARALLELS=[20,60] 
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /STEREO, /ISOTROPIC,  TITLE='STEREO',  $
         STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /STEREO, 30, 180, 45, /ISOTROPIC, $
         TITLE='STEREO OFFSET ROTATED', STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /TRANSVER, /ISOTROPIC,  $
         TITLE='TRANSVER',STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID ; hyper long?
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
MAP_SET, ADVANCE=ADVANCE, /TRANSVER, 30, 180, 45, /ISOTROPIC, $
         TITLE='TRANSVER OFFSET ROTATED', STANDARD_PARALLELS=[20,60]
MAP_CONTINENTS
MAP_HORIZON
MAP_GRID
!P.MULTI=0
;if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;mapStruct = MAP_PROJ_INIT(129, LIMIT=[0,-180,90,180],SPHERE_RADIUS=1, CENTER_LONGITUDE=180)
if DoWeBreak(tictac=tictac, fill=fill) then goto, go_to_end
;
image = BYTSCL(SIN(DIST(400)/10))
;note there is a problem for the GRID if lat=0 exactly. To be investigated!
MAP_SET, 0, -0.001, /GOODE, /ISOTROPIC,TITLE='GOODE + REPROJECTED IMAGE'
result = MAP_IMAGE(image,Startx,Starty)
TV, result, Startx, Starty
MAP_CONTINENTS, color='ff0000'x
MAP_GRID, latdel=10, londel=10, /LABEL, /HORIZON
print, 'last demo done'
;
go_to_end:
;
if KEYWORD_SET(test) then STOP
;
end
