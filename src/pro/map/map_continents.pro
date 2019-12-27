;+
;
;
;
; NAME:
;
; MAP_CONTINENTS
;
; PURPOSE: 
;
; GDL replacement of MAP_CONTINENTS
;
; CATEGORY:
;
; MAPPING 
;
; CALLING SEQUENCE:
;   MAP_CONTINENTS, /COASTS  , /USA , /RIVERS , /CONTINENTS , /COUNTRIES,
;   LIMIT=vector,MAP_STRUCTURE=map_structure, /HIRES, /T3D, ZVALUE=value{0 to 1},
;   FILL_CONTINENTS={1 | 2} , COLOR=index , ORIENTATION=value, 
;   MLINESTYLE={0 | 1 | 2 | 3 | 4 | 5}
;   MLINETHICK=value  , SPACING=centimeters
;
; INPUTS:
;   NONE
;
; INPUT KEYWORD PARAMETERS: 
;
;
; OUTPUT KEYWORD PARAMETERS:
;  NONE
;
;  OUTPUTS:
; Draw continent outlines, filled or not, etc, depending on variuos
; input parameters.
;
; COMMON BLOCKS:
;
; gdl_map_continents_compound (undocumented)
;
; SIDE EFFECTS:
; 
; NONE
;
; RESTRICTIONS:
;
; NONE
;
; PROCEDURE:
;
; We use shape (see http://shapelib.maptools.org/) files provided by naturalEarthData
; (https://www.naturalearthdata.com/). NaturalEarth contours are not of the "fillable" type.
; This is one of the reasons why continents are derived for the gshhs 
; (https://www.ngdc.noaa.gov/mgg/shorelines/gshhs.html) shapefiles
; slightly edited (L6 added to L1, some errors removed --- thanks to qgis https://www.qgis.org).
; ShapeFiles are read with our implementation of IDLffShape(). MAP_Continents is just a way to call
; the more general gdlMapShapefile procedure, passing the good shapefile(s).
;
; EXAMPLE:
;
; map_set,0,33,/goode & map_continents,/coast & map_horizon
;
; MODIFICATION HISTORY:
;       Written by: G. Duvert, March 2019
;
; LICENCE:
; Copyright (C) 2004,
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-

FUNCTION Map_GetCompound, fnames, what, hires
;returns an compound (pointer to a list with IDLffShape->GetEntity() structure array) if found
;according to codename passed. Keep these in memory using a
;common and a hashtable.
common gdl_map_continents_compound, gdl_map_continents_maps

sub = (['low', 'high'])[hires]
hashTag=what+"_"+sub

create=0
if n_elements( gdl_map_continents_maps ) gt 0 then begin
   if GDL_MAP_CONTINENTS_MAPS.HasKey(hashTag) then begin
      return, gdl_map_continents_maps[hashTag]
   endif
endif else create=1

shapefile = FILEPATH(fnames[hires]+'.shp', ROOT_DIR=!GDL_MAPS_DIR, SUBDIR=sub)
if shapefile then begin 
   shapefileObj = Obj_New('IDLffShape', shapeFile) 
   ; Get list of attribute names
   shapefileObj -> GetProperty, ATTRIBUTE_NAMES=theNames
   theNames = StrUpCase(StrTrim(theNames, 2))
   entities = Ptr_New(/Allocate_Heap)
   *entities = shapefileObj -> GetEntity(/ALL, /Attributes)
                                ; Clean up.
     Obj_Destroy, shapefileObj
     ; save in hastable: entities and attribute name list
     newlist=list(theNames,entities)
     if (create) then gdl_map_continents_maps=hash(hashTag, newlist) else gdl_map_continents_maps[hashTag]=newlist
     return,newlist
  endif else message,"map data (file "+shapefile+") not found." 
END


PRO Map_Continents ,COASTS=_COASTS , COLOR=colorindex,$
                     CONTINENTS=_Continents , COUNTRIES=_Countries, $
                     FILL_CONTINENTS=fillvalue, ORIENTATION=ori, $
                     HIRES=_Hires, LIMIT=limits , MAP_STRUCTURE=mapstruct,$
                     MLINESTYLE=mlinestyle, MLINETHICK=mlinethick, $
                     RIVERS=_Rivers, SPACING=spacing, USA=_USA,$
                     T3D=t3d, ZVALUE=zValue, AUTODRAW=AUTODRAW, $
                    _EXTRA=extra

; to be added if needed:
;                     CANADA=_CANADA,$ ; in MAPCONTINENTS function
;                     FRANCE=_FRANCE,$ ; GDL expansion example.

; note: fillvalue =1 -> solid fill
;       fillvalue =2 -> line fill (SPACING etc)
;
common gdl_map_continents_compound, gdl_map_continents_maps

ON_ERROR, 2

autodraw=n_elements(autodraw) gt 0
if ~autodraw then if (!x.type NE 3) and (N_TAGS(mapStruct) eq 0) then message,'Map transform not established.'

; by playing on attrname/attrvaluesone can plot selected subsets.
ATTRNAME = "*" ; by default
ATTRVALUES=""

; Coastlines and land shapes in NE are (apparently) identical, no
; need to have both. doContinents=continents, doCoasts=continents+lakes. 
doContinents = keyword_set(_continents) or keyword_set(_coasts)
doCoasts = keyword_set(_coasts)
; IDL's USA is only at low resolution. We *can* do better.  
doUsa = keyword_set(_usa) & if doUsa then begin  ATTRNAME="ADM0_A3" & ATTRVALUES="USA" & end
; this to show how it would be easy to have other countries than
; USA. The admin boundary of original NE files at 10m (not the oe
; shipped with GDL, which are edited because dbf files were too large) contains oodles of
; boundaries.
;doCa = 0 ;keyword_set(_canada)& if doCa then ATTRVALUES=[ATTRVALUES,"CA"] 
;doFr = 0 ;keyword_set(_france)& if doFr then ATTRVALUES=[ATTRVALUES,"FR"] 
;doAdministrative=(doUsa or doCa or doFr) & if doAdministrative then ATTRNAME="ISO_A2" 

doRivers = keyword_set(_rivers)
doCountries = keyword_set(_countries)
doHires = keyword_set(_hires)

fill=0
poly=0
if n_elements(fillvalue) gt 0 then begin
  fill=fillvalue
  if (fill ne 0) then poly=1
endif

; nothing=doContinents;
;if (~doCountries and ~doAdministrative and ~doRivers) and ~doContinents then doContinents=1 
if (~doCountries and ~doUsa and ~doRivers) and ~doContinents then doContinents=1 

if n_elements(zvalue) eq 0 then zvalue=0.0

; limits. ll_box etc are (minlat, minlon, maxlat, maxlon) .
box=[-90.0, -180.0, 90.0, 180.0]
if (~autodraw) then begin
 box = (n_elements(limits) eq 4) ? limits : (N_TAGS(mapStruct) gt 0) ? mapStruct.ll_box : !map.ll_box
 if ((box[0] ge box[2]) or (box[1] ge box[3])) then box = [-90.0, -180.0, 90.0, 180.0]
endif else begin ; do box
   xrange=[box[1], box[3]] & yrange=[box[0],box[2]]
   plot, xrange, yrange, XSTYLE=5, YSTYLE=5, Position=[0.05, 0.05, 0.95, 0.95], /NoData
   plots, [!X.Window[0], !X.Window[0], !X.Window[1], !X.Window[1],  !X.Window[0]], $
          [!Y.Window[0], !Y.Window[1], !Y.Window[1], !Y.Window[0],  !Y.Window[0]], $
          /NORMAL 
endelse
; graphic keywords. They are mostly added to the _extra structure
; method, but insuring the specific values passed to map_continents
; supercede those eventually already in any _extra structure.

if n_elements(mlinestyle) then map_struct_append, extra, 'LINESTYLE', mlinestyle,/supercede
if n_elements(mlinethick) then map_struct_append, extra, 'THICK', mlinethick,/supercede
if n_elements(spacing) and (fill eq 2) then map_struct_append, extra, 'SPACING', spacing,/supercede
if n_elements(colorindex) then map_struct_append, extra, 'COLOR',colorindex,/supercede
if n_elements(ori) and (fill eq 2) then map_struct_append, extra, 'ORIENTATION', ori,/supercede
if n_elements(t3d) then map_struct_append, extra,'t3d',t3d,/supercede
if fill eq 2 then map_struct_append, extra, 'LINE_FILL', 1,/supercede

if (doUsa) then begin	; could be doAdministrative + selection of attrvalues	;States in USA,FR,CA, a different file
   if (doContinents) then begin ; use the states without external borders. 
      compound = map_getcompound ( ['ne_110m_admin_1_states_provinces_lines', 'ne_10m_admin_1_states_provinces_lines'], 'Usa', doHires)
   endif else begin
      compound = map_getcompound ( ['ne_110m_admin_1_states_provinces', 'ne_10m_admin_1_states_provinces'], 'Usa', doHires)
   endelse
   gdlDrawShapeCompound, compound, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct, attrname=attrname , attrval=attrvalues
endif

; rivers are just rivers. No lakes. Lakes are added in /Coasts. 110m rivers are just too few.
if (doRivers) then begin
   compound = map_getcompound( ['ne_50m_rivers_lake_centerlines', 'ne_10m_rivers_lake_centerlines'], 'Rivers', doHires)
   gdlDrawShapeCompound, compound, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct 
endif

if (doCountries) then begin ; only lines. continents or coasts are already there.
   compound = map_getcompound( ['ne_110m_admin_0_boundary_lines_land', 'ne_10m_admin_0_boundary_lines_land'], 'Boundaries', doHires)
   gdlDrawShapeCompound, compound, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct ;, attrname=attrname, attrval=attrval
endif

if (doContinents) then begin ; coasts = continents + lakes. coasts may be more complicated depending on scale.  NaturalEarth contours are not of the "fillable" type.
   compound = map_getcompound( ['continents', 'continents_med'], 'Continents', doHires)
   gdlDrawShapeCompound, compound, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct, /force_fill ;, attrname=attrname, attrval=attrval
endif

if (doCoasts) then begin ; coasts = continents + lakes. coasts may be more complicated depending on scale.
   compound = map_getcompound( ['ne_110m_lakes', 'ne_10m_lakes'], 'Lakes', doHires)
   gdlDrawShapeCompound, compound, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct, /force_fill  ;, attrname=attrname, attrval=attrval
endif



end
