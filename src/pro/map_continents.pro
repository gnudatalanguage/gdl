;+
;
; NAME:
;   MAP_CONTINENTS
;
; PURPOSE:
; GDL replacement of MAP_CONTINENTS
; 
; CALLING SEQUENCE:
;   MAP_CONTINENTS, /COASTS  , /USA , /RIVERS , /CONTINENTS , /COUNTRIES,
;   LIMIT=vector,MAP_STRUCTURE=map_structure, /HIRES, /T3D, ZVALUE=value{0 to 1},
;   FILL_CONTINENTS={1 | 2} , COLOR=index , ORIENTATION=value, 
;   MLINESTYLE={0 | 1 | 2 | 3 | 4 | 5}
;   MLINETHICK=value  , SPACING=centimeters
;
; KEYWORD Parameters:
;
; EXPLANATION:
; We use shape (see http://shapelib.maptools.org/) files provided by naturalEarthData
; (https://www.naturalearthdata.com/). ShapeFiles are read with our
; implementation of IDLffShape().
;- 


PRO mapContinents_DrawSingleEntity, entity, zvalue, extra, polyfill=poly, MAPSTRUCT=mapStruct
;
; Code below has been inspired by David Fanning's cgDrawShapes procedure
; see Coyote's Guide to IDL Programming: http://www.idlcoyote.com
;       

;   Compile_Opt idl2

   IF ~Ptr_Valid(entity.parts) THEN RETURN
   minpts=[1,3] ; for [poly=0,1]
   ; Drawing is going to be done based on the shape type.
   SWITCH 1 OF
      
      ; Polyline shapes. Not closed nor oriented. 
      ; Probably filling is irrelevant. Probably for RIVERS.
      entity.shape_type EQ  3 OR $   ; PolyLine
      entity.shape_type EQ 13 OR $   ; PolyLineZ (ignoring Z)
      entity.shape_type EQ 23: BEGIN ; PolyLineM (ignoring M)
;         POLY=0;  use at your own risk!!!!!! filling *may* work for small parts
      END

      ; Polygon shapes: are closed and oriented. good.
      entity.shape_type EQ 5 OR $    ; Polygon.
      entity.shape_type EQ 15 OR $   ; PolygonZ (ignoring Z)
      entity.shape_type EQ 25: BEGIN ; PolygonM (ignoring M)

         cuts = [*entity.parts, entity.n_vertices]
         FOR j=0, entity.n_parts-1 DO BEGIN
            x = (*entity.vertices)[0, cuts[j]:cuts[j+1]-1]
            y = (*entity.vertices)[1, cuts[j]:cuts[j+1]-1]
            if (N_ELEMENTS(x) lt minpts[poly]) then break         ; not drawable/fillable
            IF N_Elements(mapStruct) NE 0 THEN BEGIN
               xy = poly? $
                    map_proj_forward(x,y, MAP_STRUCTURE=mapStruct, POLYGONS=polyconn) :$
                    map_proj_forward(x,y, MAP_STRUCTURE=mapStruct, POLYLINES=polyconn) 

               n = N_ELEMENTS(xy)/2 

               if (n lt minpts[poly]) then break  ; not drawable/fillable either!

               index = 0L
               while (index lt n) do begin
                  ipoly = polyconn[index + 1 : index + polyconn[index]]
                  if (poly) then polyfill, xy[0,ipoly], xy[1,ipoly],zvalue, NOCLIP=0, _EXTRA=extra else plots, xy[0,ipoly], xy[1,ipoly], zvalue, NOCLIP=0, _EXTRA=extra
                  index = index + polyconn[index] + 1
               endwhile
            ENDIF ELSE BEGIN
               if (poly) then polyfill, x, y, zvalue, NOCLIP=0, _EXTRA=extra else plots, x, y, zvalue, NOCLIP=0, _EXTRA=extra
            ENDELSE
         ENDFOR
         BREAK                  ; we are in SWITCH
      END                   ; Polygon shapes.

     ; Various kinds of points. Here for completeness, in case 
     ; it proves useful as they are not used in map_continents.
     entity.shape_type eq  1 or $   ; Point
     entity.shape_type eq 11 or $   ; PointZ (ignoring Z)
     entity.shape_type eq 21 or $   ; PointM (ignoring M)
     entity.shape_type eq  8 or $   ; MultiPoint
     entity.shape_type eq 18 or $   ; MultiPointZ (ignoring Z)
     entity.shape_type eq 28: BEGIN ; MultiPointM (ignoring M)
        
        IF N_Elements(mapStruct) NE 0 THEN BEGIN
           xy = map_proj_forward(entity.vertices[0,*], entity.vertices[1,*], MAP_STRUCTURE=mapStruct)
           n = N_ELEMENTS(xy)/2
           if (n eq 0) then begin
              break            ; all points were clipped
           endif
           x = Reform(xy[0,*])
           y = Reform(xy[1,*])
        ENDIF ELSE BEGIN
           x = (*entity.vertices)[0,*]
           y = (*entity.vertices)[1,*]
        ENDELSE

        plots, x, y, _EXTRA=extra
     END

     ELSE: Message, 'Not currently handling entity type: ' + StrTrim(entity.shape_type,2)

  ENDSWITCH


END 

PRO mapContinents_DrawCompound, compound, box, zvalue, extra, attrname=attrname, attrval=attrval, polyfill=poly, MAPSTRUCT=mapStruct
  ; Cycle through each compound's entity and draw it, if required.

  attributes_names=compound[0]
  entities=compound[1]
  nentities=N_Elements(*entities)
  ; Find the attribute index, if attrname is not '*' of course.
  if attrname eq '*' then begin
     FOR j=0,nentities-1 DO BEGIN
        thisEntity = (*entities)[j]
        IF Ptr_Valid(thisEntity.vertices) THEN BEGIN
           mapContinents_DrawSingleEntity, thisEntity, zvalue, extra, polyfill=poly, MAPSTRUCT=mapStruct
        ENDIF
     ENDFOR
     return
  endif 
  attIndex = Where(attributes_names eq attrname, count)
  IF count EQ 0 THEN return
  FOR j=0,nentities-1 DO BEGIN
     thisEntity = (*entities)[j]
     theEntityName = StrUpCase(StrTrim((*thisEntity.attributes).(attIndex), 2))
     index = Where(StrUpCase(attrval) EQ theEntityName, test)
     IF (test EQ 1) THEN BEGIN
        IF Ptr_Valid(thisEntity.vertices) THEN BEGIN
           mapContinents_DrawSingleEntity, thisEntity, zvalue, extra, polyfill=poly, MAPSTRUCT=mapStruct
        ENDIF
     ENDIF
  ENDFOR
END

FUNCTION Map_GetCompound, fnames, what, hires
;returns an compound (pointer to a list with IDLffShape->GetEntity() struture array) if found
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
                     CANADA=CANADA,$ ; GDL but also MAPCONTINENTS function
                     FRANCE=FRANCE,$ ; GDL but also MAPCONTINENTS function
                    _EXTRA=extra

; note: fillvalue =1 -> solid fill
;       fillvalue =2 -> line fill (SPACING etc)
;
common gdl_map_continents_compound, gdl_map_continents_maps

;ON_ERROR, 2

autodraw=n_elements(autodraw) gt 0
if ~autodraw then if (!x.type NE 3) and (N_TAGS(mapStruct) eq 0) then message,'Map transform not established.'
ATTRNAME = '*' ; by default
ATTRVALUES=""

doContinents = keyword_set(_continents)
doUsa = keyword_set(_usa)
doCa = keyword_set(canada)
doFr = keyword_set(france)
doAdministrative=(doUsa or doCa or doFr)
doRivers = keyword_set(_rivers)
doCoasts = keyword_set(_coasts)
doCountries = keyword_set(_countries)
doHires = keyword_set(_hires)

fill=0
poly=0
if n_elements(fillvalue) gt 0 then begin
  fill=fillvalue
  poly=1
endif

if (~doCountries and ~doUsa and ~doRivers and ~doCoasts) and ~doContinents then doContinents=1 
if (doCoasts) then doContinents=1

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

; rivers are just rivers. No lakes. Lakes are added in
; /Coasts. Coastlines and land shapes are (apparently) identical, no
; need to have both.
if (doRivers) then begin
   compound = map_getcompound( ['ne_50m_rivers_lake_centerlines', 'ne_10m_rivers_lake_centerlines'], 'Rivers', doHires)
   mapContinents_DrawCompound, compound, box, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct, attrname=attrname, attrval=attrval
endif

if (doCountries) then begin ; only lines.
   compound = map_getcompound( ['ne_110m_admin_0_boundary_lines_land', 'ne_10m_admin_0_boundary_lines_land'], 'Boundaries', doHires)
   mapContinents_DrawCompound, compound, box, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct, attrname=attrname, attrval=attrval
endif

if (doContinents) then begin ; coasts = continents + lakes. coasts may be more complicated depending on scale.
   compound = map_getcompound( ['ne_110m_coastline', 'ne_10m_coastline'], 'Coasts', doHires)
   mapContinents_DrawCompound, compound, box, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct, attrname=attrname, attrval=attrval
endif

if (doCoasts) then begin ; coasts = continents + lakes. coasts may be more complicated depending on scale.
   compound = map_getcompound( ['ne_110m_lakes', 'ne_50m_lakes'], 'Lakes', doHires)
   mapContinents_DrawCompound, compound, box, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct, attrname=attrname, attrval=attrval
endif

if (doUsa) then begin		;States in USA, a different file
   compound = map_getcompound ( ['ne_50m_admin_1_states_provinces', 'ne_10m_admin_1_states_provinces'], 'Usa', doHires)
   attrname="ISO_A2" & attrval="US"
   mapContinents_DrawCompound, compound, box, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct, attrname=attrname, attrval=attrval
endif

if (doCa) then begin		;
   compound = map_getcompound ( ['ne_50m_admin_1_states_provinces', 'ne_10m_admin_1_states_provinces'], 'Usa', doHires)
   attrname="ISO_A2" & attrval="CA" 
   mapContinents_DrawCompound, compound, box, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct, attrname=attrname, attrval=attrval
endif

if (doFr) then begin		;
   compound = map_getcompound ( ['ne_50m_admin_1_states_provinces', 'ne_10m_admin_1_states_provinces'], 'Usa', doHires)
   attrname="ISO_A2" & attrval="FR"
   mapContinents_DrawCompound, compound, box, zvalue, extra, POLYFILL=poly, MAPSTRUCT=mapStruct, attrname=attrname, attrval=attrval
endif


end
