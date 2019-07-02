;+
;
; NAME:
;
;   GDLDRAWSHAPECOMPOUND
;
; PURPOSE:
;
;   USED BY GdlDrawShapefile (and MAP_CONTINENTS)
; 
;- 


PRO gdlDrawSingleshape, shape, zvalue, extra, polyfill=poly, MAPSTRUCT=mapStruct, force_fill=force
;
; Code below has been inspired by David Fanning's cgDrawShapes procedure
; see Coyote's Guide to IDL Programming: http://www.idlcoyote.com
;       

  Compile_Opt idl2, hidden
  on_error, 2

   minpts=[1,3] ; for [poly=0,1]
   ; Drawing is going to be done based on the shape type.
   SWITCH 1 OF

      ; Polyline shapes. Not closed nor oriented. 
      ; Probably filling is irrelevant. Probably for RIVERS.
      shape.shape_type EQ  3 OR $   ; PolyLine
      shape.shape_type EQ 13 OR $   ; PolyLineZ (ignoring Z)
      shape.shape_type EQ 23: BEGIN ; PolyLineM (ignoring M)
         if ~keyword_set(force) then POLY=0;  use at your own risk!!!!!! filling *may* work for small parts
      END

      ; Polygon shapes: are closed and oriented. good.
      shape.shape_type EQ 5 OR $    ; Polygon.
      shape.shape_type EQ 15 OR $   ; PolygonZ (ignoring Z)
      shape.shape_type EQ 25: BEGIN ; PolygonM (ignoring M)

         ; we expect polygons, not points: enty.parts must be >0
         IF ~Ptr_Valid(shape.parts) then return
         IF ~ptr_valid(shape.vertices) then return
         cuts = [*shape.parts, shape.n_vertices]
         FOR j=0, shape.n_parts-1 DO BEGIN
            x = (*shape.vertices)[0, cuts[j]:cuts[j+1]-1]
            y = (*shape.vertices)[1, cuts[j]:cuts[j+1]-1]
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
     shape.shape_type eq  1 or $   ; Point
     shape.shape_type eq 11 or $   ; PointZ (ignoring Z)
     shape.shape_type eq 21 or $   ; PointM (ignoring M)
     shape.shape_type eq  8 or $   ; MultiPoint
     shape.shape_type eq 18 or $   ; MultiPointZ (ignoring Z)
     shape.shape_type eq 28: BEGIN ; MultiPointM (ignoring M)
        
        IF N_Elements(mapStruct) NE 0 THEN BEGIN
           xy = map_proj_forward(shape.bounds[0], shape.bounds[1], MAP_STRUCTURE=mapStruct)
           n = N_ELEMENTS(xy)/2
           if (n eq 0) then begin
              break            ; all points were clipped
           endif
           x = Reform(xy[0])
           y = Reform(xy[1])
        ENDIF ELSE BEGIN
           x = shape.bounds[0]
           y = shape.bounds[1]
        ENDELSE
        plots, x, y, _EXTRA=extra
        BREAK                   ; we are in SWITCH
     END

     ELSE: return ; like: shapetype 0 = placeholder.

  ENDSWITCH


END 

PRO gdlFreeShapeCompound, compound
  Compile_Opt idl2, hidden
  on_error, 2
  attributes_names=compound[0]
  entities=compound[1]
  nentities=N_Elements(*entities)
  FOR j=0,nentities-1 DO BEGIN
     thisShape = (*entities)[j]
     Ptr_Free, thisShape.vertices
     Ptr_Free, thisShape.measure
     Ptr_Free, thisShape.parts
     Ptr_Free, thisShape.part_types
     Ptr_Free, thisShape.attributes
  ENDFOR
END

PRO gdlDrawShapeCompound, compound, zvalue, extra, attrname=attrname, attrval=attrval, polyfill=poly, MAPSTRUCT=mapStruct, force_fill=force
  Compile_Opt idl2, hidden
  on_error, 2

  ; Cycle through each compound's shape and draw it, if required.
  if n_elements(attrname) eq 0 then attrname = '*'

  attributes_names=compound[0]
  entities=compound[1]
  nentities=N_Elements(*entities)
  ; Find the attribute index, if attrname is not '*' of course.
  if attrname eq '*' then begin
     FOR j=0,nentities-1 DO BEGIN
        thisShape = (*entities)[j]
        gdlDrawSingleShape, thisShape, zvalue, extra, polyfill=poly, MAPSTRUCT=mapStruct, force_fill=force
     ENDFOR
     return
  endif 
  attIndex = Where(attributes_names eq attrname, count)
  IF count EQ 0 THEN return
  FOR j=0,nentities-1 DO BEGIN
     thisShape = (*entities)[j]
     theshapeName = StrUpCase(StrTrim((*thisShape.attributes).(attIndex), 2))
     index = Where(StrUpCase(attrval) EQ theshapeName, test)
     IF (test EQ 1) THEN BEGIN
           gdlDrawSingleshape, thisShape, zvalue, extra, polyfill=poly, MAPSTRUCT=mapStruct, force_fill=force
     ENDIF
  ENDFOR
END

