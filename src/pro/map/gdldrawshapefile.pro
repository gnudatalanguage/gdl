;+
;
; NAME:
;
;   GDLDRAWSHAPEFILE
;
; PURPOSE:
;
;   PLOTS the (filtered) contents of a SHAPEFILE
; 
; CALLING SEQUENCE:
;
;   GdlDrawShapefile, filename [,attrname=attrname, attrval=attrval]
;   [,MAP_STRUCTURE=map_structure] [,/T3D, ZVALUE=value{0 to 1}]
;   [,FILL_CODE={1 | 2} , /FORCE_FILL, COLOR=Color_index , ORIENTATION=value 
;   ,USE_LINESTYLE={0 | 1 | 2 | 3 | 4 | 5}
;   ,USE_LINETHICK=value, SPACING=centimeters], PSYM=MPSYM, SYMSIZE=MSYMSIZE
;
; EXPLANATION:
; 
; provided a shapefile "filename", will draw all its shapes, or subset
; if attrname AND attrval are given (see below). Other parameters apply to the
; graphic part and have the same meaning as in MAP_CONTINENTS (but
; with somewhat different names). Shapes than cannot be filled (non
; closed, non oriented) will not be filled even if FILL_CODE is
; nonzero, unless FORCE_FILL is set (in which case it may give strage results!)
;  
; KEYWORD Parameters:
;
; ATTRNAME and ATTRVAL: Shapefiles come with attributes, such as names or index
; or... that are associated with each shape they contain. ATTRNAME is
; the name of the keyword to look at. ATTRVAL is the value(s] (as this
; can be an array) that will be selected to be plotted. Knowledege of
; the contents of the shapefile is mandatory to make such selection as
; nothing is normalized.
;
; PSYM and SYMSIZE: only for shapes of type Point(point, pointZ etc): will draw a symbol.
;
; FORCE_FILL: to force filling type "Polyline" shapes that are a priori not good
; for filling as they are not closed nor oriented.
;- 



PRO GdlDrawShapefile, shapefile, ATTRNAME=attrname, ATTRVAL=attrval, COLOR=colorindex,$
                     FILL_CODE=fillcode, FORCE_FILL=force, ORIENTATION=ori, $
                     MAP_STRUCTURE=mapstruct,$
                     USE_LINESTYLE=mlinestyle, USE_LINETHICK=mlinethick, $
                     SPACING=spacing, PSYM=MPSYM, SYMSIZE=MSYMSIZE,$
                     T3D=t3d, ZVALUE=zValue,$
                     _EXTRA=extra

; note: fillcode =1 -> solid fill
;       fillcode =2 -> line fill (SPACING etc)
;

ON_ERROR, 2

if n_elements(zvalue) eq 0 then zvalue=0.0

fill=0
poly=0
if n_elements(fillcode) gt 0 then begin
  fill=fillcode
  poly=1
endif


; graphic keywords. They are mostly added to the _extra structure
; method, but insuring the specific values passed to map_continents
; supercede those eventually already in any _extra structure.

if n_elements(mlinestyle) then map_struct_append, extra, 'LINESTYLE', mlinestyle,/supercede
if n_elements(mlinethick) then map_struct_append, extra, 'THICK', mlinethick,/supercede
if n_elements(spacing) and (fill eq 2) then map_struct_append, extra, 'SPACING', spacing,/supercede
if n_elements(colorindex) then map_struct_append, extra, 'COLOR',colorindex,/supercede
if n_elements(ori) and (fill eq 2) then map_struct_append, extra, 'ORIENTATION', ori,/supercede
if n_elements(t3d) then map_struct_append, extra,'t3d',t3d,/supercede
if n_elements(mpsym) then map_struct_append, extra,'PSYM',mpsym,/supercede
if n_elements(msymsize) then map_struct_append, extra,'SYMSIZE',msymsize,/supercede
if fill eq 2 then map_struct_append, extra, 'LINE_FILL', 1,/supercede


; open file, create compound, select, draw shapes, remove objects and
; elements.

;returns an compound (pointer to a list with IDLffShape->GetEntity() struture array) if found
;according to codename passed.
shapefileObj = Obj_New('IDLffShape', shapeFile) 
IF Obj_Valid(shapefileObj) EQ 0 THEN Message, 'Invalid or unExistent Shapefile.'
shapefileObj -> GetProperty, ATTRIBUTE_NAMES=theNames
theNames = StrUpCase(StrTrim(theNames, 2))
entities = Ptr_New(/Allocate_Heap)
*entities = shapefileObj -> GetEntity(/ALL, /Attributes)
; Clean up object here.
Obj_Destroy, shapefileObj
; save in hastable: entities and attribute name list
compound=list(theNames,entities)
gdlDrawShapeCompound, compound, zvalue, extra, attrname=attrname, attrval=attrval, polyfill=poly, MAPSTRUCT=mapStruct, force_fill=force
; delete compound
gdlFreeShapecompound, compound
end
