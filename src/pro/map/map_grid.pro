;+
; NAME:
;	MAP_GRID
;
; PURPOSE:
;       Draws graticules
;
; KEYWORD PARAMETERS:
;
;
; BOX_AXES: 
;   CHARSIZE:
;      COLOR:
;FILL_HORIZON:
;    HORIZON:
;  INCREMENT: sets spacing between graticle points.
; GLINESTYLE: Line style of grid lines (passed to "plots")
; GLINETHICK: idem for thickness
;      LABEL: LABEL n puts a label every n graticle lines, startin
;      with min lat/lon unless LATS or LONS is a single value, this
;      being the starting point.
;   LATALIGN: Label alignment value form 0.0 to 1.0
;     LATDEL: latitude spacing of lines
;       LATS: supercedes the LATDEL mechanism by providing a series of latitudes
;     LATLAB: Which longitude to place latitude labels, default being
;     the center longitude on the map. 
;   LATNAMES: names used instead of normal latitude labels. Wizard only.
;   LONALIGN: see LATALIGN, but for longitudes
;     LONDEL: see LATDEL
;     LONLAB: see LATLAB
;       LONS: see LATS
;   LONNAMES: see LATNAMES
;
; MAP_STRUCTURE: use a map structure and not !MAP.
;
; NO_GRID: only labels, no gridlines.
;
;ORIENTATION: of the labels (reverse direction from XYOUTS)
;
; T3D: 
; ZVALUE:
;
;-

function map_point_transformable,x,y
  xy=map_proj_forward(x,y)
  return, finite(xy[0]) and finite(xy[1])
end

function map_grid_toformat,s,f
  if (reverse(size(s)))[1] eq 7 then return,s else return,strtrim(string(s, FORMAT=f),2)
end

;-------------------------------------------------------------------------
function compute_map_grid_increment, range
  COMPILE_OPT hidden, IDL2
  if range eq 0 then return, 45.
  ipow = 0
  t = abs(range) < 450.
  while t lt 5 do begin
     t = t * 10
     ipow = ipow +1
  endwhile
  increments = [ 1., 2., 4., 5., 10., 15., 30., 45.]
  i = 0
  while t gt (increments[i] * 10) do i = i + 1
  t = increments[i] / 10^ipow
  return, range ge 0 ? t : -t
end


;-------------------------------------------------------------------------
; ancillary function to find where the line [A,B] (in device coords)
; intersects the longitude (if type=0) or latitude (type=1)
; 'val'. when this is impossible, returns Nan.
function find_grid_intersection, A, B, type, val, MAP_STRUCTURE=mapStruct
    COMPILE_OPT hidden, IDL2
    hasMap = N_TAGS(mapStruct) gt 0
    startPixel = CONVERT_COORD(A, /DEVICE, /TO_DATA)
    endPixel = CONVERT_COORD(B, /DEVICE, /TO_DATA)
    if (hasMap) then begin ; we want lons / lats.
        startPixel = MAP_PROJ_INVERSE(startPixel[0], startPixel[1], MAP_STRUCTURE=mapStruct)
        endPixel = MAP_PROJ_INVERSE(endPixel[0], endPixel[1], MAP_STRUCTURE=mapStruct)
    endif
    startPixel = startPixel[type]
    endPixel = endPixel[type]
    if ~finite(startPixel) or ~finite(endPixel) or (endPixel eq startPixel) then return, !values.d_nan
    if (type eq 0) and (startPixel gt endPixel) then if val ge 0 then endPixel += 360. else startPixel -= 360.
    x = (val - startPixel) / (endPixel-startPixel)
    if (x gt 1.0) or (x lt 0.0) then return, !values.f_nan
    debut = 0.0 & fin = 1.0 & range = B - A
    while abs(fin-debut) gt 1e-5 do begin
        x = (debut + fin) / 2. &  intersect = A + x * range & point = CONVERT_COORD(intersect, /DEVICE, /TO_DATA)
        if (hasMap) then point = MAP_PROJ_INVERSE(point[0], point[1], MAP_STRUCTURE=mapStruct)
        point = point[type]
        if (~FINITE(point)) then $
            return, !values.f_nan
        if (type eq 0) then if point lt startPixel then point += 360. else if point gt endPixel then point -= 360.
        if (val-startPixel) * (val - point) gt 0.0 then begin
            debut = x
            startPixel = point
        endif else fin = x
    endwhile
    return, intersect[type]
end


;-------------------------------------------------------------------------
PRO map_grid, LABEL=labelSeparation, LATDEL = lat_separation, NO_GRID=nogrid, $
              LONDEL=lon_separation, GLINESTYLE=ourLineStyle, GLINETHICK=ourLineThick,$
              LONLAB=longitudeOfLabels, LATLAB=latitudeOfLabels, LONALIGN=lonalign, $
              LATALIGN=latalign, LONNAMES=lonstrings, LATNAMES=latstrings, $
              LATS=passed_lats, LONS=passed_lons, $
              COLOR=gridColor, CHARSIZE=charsize, ORIENTATION=orientation, $
; MAP_HORIZON keywords:
              HORIZON=horizon, E_HORIZON=ehorizon, FILL_HORIZON=fill_horizon, $ 
              INCREMENT=increment, CLIP_TEXT=clip_text, BOX_AXES=box_axes, $
              MAP_STRUCTURE=passedMap,  ZVALUE=zvalue, T3D=t3d, _EXTRA=extra, $
              WHOLE_MAP=dummy
  compile_opt idl2
  ON_ERROR, 2

; first, evacuate some problems

  has_passed_map = n_tags(passedMap) gt 0

  if ((!x.type ne 3) and ~has_passed_map) THEN message, 'Current ploting device must have mapping coordinates'

  dobox=keyword_set(box_axes)

; graphic values to remember:
  d_y_ch_size = !d.y_ch_size 
  if n_elements(charsize) then d_y_ch_size = d_y_ch_size * charsize
  box_thick = (dobox)?box_axes*0.1*!d.y_px_cm : 0; thickness of box_axe in device units
  xpixels = !x.window * !d.x_size
  ypixels = !y.window * !d.y_size


  if n_elements(gridColor) eq 0 then gridColor = !p.color ;Default color
  if n_elements(t3d) le 0 then t3d = 0
  if n_elements(zvalue) eq 0 then zvalue = 0.
  if n_elements(charsize) eq 0 then charsize = !p.charsize
  if charsize le 0.0 then charsize = 1.0

                                ; graphic passable keywords
  map_struct_append, egraphics, "COLOR", gridColor
  map_struct_append, egraphics, "T3D", t3d
  map_struct_append, egraphics, "ZVALUE", zvalue

; a projection exist.
; if Label = n, then Labels are added every n gridlines
; If box_axes is set, and LABEL isn't explicitly specified, set labelSeparation.
;
  nuberOfLabels = (n_elements(labelseparation) gt 0) ? fix(abs(labelseparation[0])) : dobox
  noclip = (n_elements(clip_text) gt 0) ? ~keyword_set(clip_text) : 0 ;1 to clip text within the map area, 0 to not.
  drawGrid = ~keyword_set(nogrid)                                     ; we want grid, not just labels

; we need to process these options here:
  if n_elements(gridColor) ne 0 then map_struct_append, extra, 'COLOR',gridColor
  if n_elements(charsize) ne 0 then map_struct_append, extra,'CHARSIZE', charsize
  if n_elements(ourLineThick) ne 0 then map_struct_append, extra,'THICK', ourLineThick
  map_struct_append, extra,'LINESTYLE', (n_elements(ourLineStyle) gt 0)?ourLineStyle:1 ; default value is DOTTED LINE.
  if n_elements(orientation) ne 0 and ~dobox then map_struct_append, extra,'ORIENTATION', -1 * orientation ;Orientation is reversed & conflicts w/box_axes

; gridlines can be, by order of preference:
; 1) set by lats=[...] lons=[...]
; 2) 1 lats and/or lon value, the center, and then
; 3) each line is computed from this center using default or not of lat_separation/londel.
;
; if latnames is present, lats must be also.
  doMyLons =  n_elements(passed_lons) gt 0
  doMyLats =  n_elements(passed_lats) gt 0
  if n_elements(latstrings) gt 0 and ~doMyLats then message,'LATNAMES keyword cannot be used without LATS keyword also defined.'
  if n_elements(lonstrings) gt 0 and ~doMyLons then message,'LONNAMES keyword cannot be used without LONS keyword also defined.'

  myMap = has_passed_map ? passedMap : !MAP
  if doMyLats then begin        ;Lats directly specified?
     minlat = passed_lats[0]
     maxlat = passed_lats[-1]
  endif else if myMap.ll_box[0] ne myMap.ll_box[2] then begin
     minlat = myMap.ll_box[0]
     maxlat = myMap.ll_box[2]
  endif else begin
     minlat = -90
     maxlat = 90
  endelse

  if doMyLons then begin 
     minlon = passed_lons[0]
     maxlon = passed_lons[-1]
  endif else if (myMap.ll_box[1] ne myMap.ll_box[3]) and (maxlat lt 90.) and (minlat gt -90.) then begin
     minlon = myMap.ll_box[1]
     maxlon = myMap.ll_box[3]
  endif else begin
     minlon = -180
     maxlon = 180
  endelse

  if maxlon le minlon then maxlon = maxlon + 360.

;Grid spacing
  if n_elements(lat_separation) gt 0 then latdelta=lat_separation[0] else begin
     lat_separation = compute_map_grid_increment(maxlat - minlat)
     latdelta = 1
  endelse

  if n_elements(lon_separation) gt 0 then londelta = lon_separation[0] else  begin
     lon_separation = compute_map_grid_increment(maxlon - minlon)
     londelta = 1
  endelse

  if abs(maxlat - minlat) gt 5. and latdelta ge 1 then begin ; IF the deltas are smaller than 1 degree, do not convert to integers
     minlat = float(floor(minlat))
     maxlat = ceil(maxlat)
  endif

  if abs(maxlon - minlon) gt 5 and londelta ge 1 THEN BEGIN ;idem
     minlon = float(floor(minlon))
     maxlon = ceil(maxlon)
  endif
; labels
  if n_elements(latitudeOfLabels) eq 0 then latitudeOfLabels = (minlat + maxlat)/2
  if n_elements(longitudeOfLabels) eq 0 then longitudeOfLabels = (minlon +maxlon)/2
  if n_elements(latalign) eq 0 then latalign = .5 ;center
  if n_elements(lonalign) eq 0 then lonalign = .5 ;center

  if (has_passed_map) then map_proj_info, iproj, cylindrical=is_cyl, map_struct=passedMap else map_proj_info, iproj, cylindrical=is_cyl, /current

  if keyword_set(increment) then step = increment else step = 4 < (maxlat - minlat)/10.

  latlistsize = long(float((maxlat-minlat)) / float(step) + 1.0)

  latlist = (float(maxlat-minlat) / (latlistsize-1.)) * findgen(latlistsize) + minlat > (-90) < 90 ; Avoid roundoff errors

  if is_cyl and myMap.p0lat eq 0 then begin
     if latlist[0] eq -90 then latlist[0] = -89.98
     if latlist[latlistsize-1] eq 90 then latlist[latlistsize-1] = 89.98
  endif

  lonStep = 4 < (maxlon - minlon)/10. ; max 4 degrees!
  lonListSize = (maxlon-minlon)/lonStep + 1
  lonlist = findgen(lonListSize) * lonStep + minlon
  if (lonlist[lonlistsize-1] ne maxlon) then lonlist = [lonlist, maxlon]
;
  if n_elements(passed_lats) eq 0 then begin
     lat0 = minlat - (minlat mod float(lat_separation)) ;1st lat for grid
     n_passed_lats = 1 + fix((maxlat-lat0)/float(lat_separation))
     latitude_list = lat0 + findgen(n_passed_lats)*lat_separation
  endif else if n_elements(passed_lats) eq 1 then begin
     i0 = ceil((minlat - passed_lats[0]) / float(lat_separation)) ;First tick
     i1 = floor((maxlat - passed_lats[0]) / float(lat_separation)) ;Last tick
     n_passed_lats = i1 - i0 + 1 > 1
     latitude_list = (findgen(n_passed_lats) + i0) * lat_separation + passed_lats[0]
  endif else begin
     n_passed_lats=n_elements(passed_lats)
     latitude_list=passed_lats
  endelse
;
  if n_elements(passed_lons) eq 0 then begin
     n_passed_lons = 1+fix((maxlon-minlon) / lon_separation)
     longitude_list = minlon - (minlon mod lon_separation) + findgen(n_passed_lons) * lon_separation
  endif else if n_elements(passed_lons) eq 1 then begin
     i0 = ceil((minlon - passed_lons[0]) / float(lon_separation))
     i1 = floor((maxlon - passed_lons[0]) / float(lon_separation))
     n_passed_lons = i1 - i0 + 1 > 1
     longitude_list = (findgen(n_passed_lons) + i0) * lon_separation + passed_lons[0]
  endif else begin
     n_passed_lons=n_elements(passed_lons)
     longitude_list=passed_lons
  endelse
;
  doLabelLon = bytarr(n_passed_lons)
  doLabelLat = bytarr(n_passed_lats)
  if nuberOfLabels gt 0 then begin
     if n_elements(passed_lats) eq 1 then begin 
        w=where(latitude_list eq passed_lats[0], count) & if count gt 0 then for i=(w[0] mod nuberOfLabels), n_passed_lats-1, nuberOfLabels do doLabelLat[i] = 1
     endif else for i=0, n_passed_lats-1, nuberOfLabels do doLabelLat[i] = 1

     if n_elements(passed_lons) eq 1 then begin
        w=where(longitude_list eq passed_lons[0], count) & if count gt 0 then for i=(w[0] mod nuberOfLabels), n_passed_lons-1, nuberOfLabels do doLabelLon[i] = 1
     endif else for i=0, n_passed_lons-1, nuberOfLabels do doLabelLon[i] = 1
  endif
; put lat lons texts into convenient array
  n = n_passed_lons > n_passed_lats & textArrayForLonLat = strarr(n, 2)

  if dobox then begin 
     fudgefact = [0.01,-0.01]
     xboxpix = (myMap.uv_box[[0,2]] * !x.s[1] + !x.s[0]) * !d.x_size + fudgefact
     yboxpix = (myMap.uv_box[[1,3]] * !y.s[1] + !y.s[0]) * !d.y_size + fudgefact


     if n_elements(gridColor) eq 0 then bcolor = !p.color $ ;Box color
     else bcolor = gridColor

     xp = xpixels[0] - [0,box_thick, box_thick,0] ;X  & Y polygon coords for outer box
     yp = ypixels[0] - [0,0,box_thick,box_thick]
                                ;Draw the outline of the box
     plots, xpixels[[0,1,1,0,0]], ypixels[[0,0,1,1,0]], /DEVICE, COLOR=bcolor
     plots, xpixels[[0,1,1,0,0]]+[-box_thick, box_thick, box_thick, -box_thick, -box_thick], $
            ypixels[[0,0,1,1,0]]+[-box_thick, -box_thick, box_thick, box_thick, -box_thick], /DEVICE, COLOR=bcolor

     ychar = [ypixels[0]-box_thick-d_y_ch_size, ypixels[1]+box_thick+d_y_ch_size/4.]
     xchar = [xpixels[0] - box_thick - d_y_ch_size/4., xpixels[1]+box_thick+d_y_ch_size/4.]

     boxpos = replicate(!values.f_nan, n, 2,2)
  endif

; map_horizon? first, because of possibility to be filled... 
  if keyword_set(horizon) or keyword_set(ehorizon) or keyword_set(fill_horizon) then begin
     merge_structs_mapset, ehorizon, egraphics ;Add common graphics keywords
     if keyword_set(fill_horizon) then map_struct_append, ehorizon, "FILL", fill_horizon
     MAP_HORIZON, _EXTRA=ehorizon
  endif

; arrange longitude_list btw. -180 and 180
  map_adjlon,longitude_list

;
; parallels (curlat) and labels at latlab
;

  FOR i=0,n_passed_lats-1 DO BEGIN

     curlat=latitude_list[i] ; no fudge for lats.

     fmt = (curlat ne long(curlat)) ? '(f7.2)' : '(i4)'

     IF (drawGrid and (abs(curlat) ne 90)) then begin
        y = REPLICATE(curlat, N_ELEMENTS(lonlist))
        x = lonlist
        if (has_passed_map) then begin
           xy = map_proj_forward(x,y, MAP_STRUCTURE=passedMap, POLYLINES=polyconn) 
           n = N_ELEMENTS(xy)/2 
           if (n lt 2) then break ; not drawable
           index = 0L
           while (index lt n) do begin
              ipoly = polyconn[index + 1 : index + polyconn[index]]
              plots, xy[0,ipoly], xy[1,ipoly], zvalue, NOCLIP=0, _EXTRA=extra
              index = index + polyconn[index] + 1
           endwhile
        endif else begin
           PLOTS, x, y, NOCLIP=0, _EXTRA=extra
        endelse
     endif

     IF doLabelLat[i] THEN BEGIN
        IF i lt n_elements(latstrings) then ls=map_grid_toformat(latstrings[i],fmt) else ls=strtrim(string(curlat, format=fmt),2)
        textArrayForLonLat[i,1] = ls
        if ~dobox then begin
           xy = 0; only one element if failed.
           if (has_passed_map) then begin
              uv = MAP_PROJ_FORWARD(LongitudeOfLabels, curlat, MAP_STRUCTURE=passedMap)
              if (FINITE(uv[0]) and FINITE(uv[1])) then xy = uv[0:1]
           endif else begin
              if (noclip eq 1) or map_point_transformable(LongitudeOfLabels, curlat) then xy = [LongitudeOfLabels, curlat]
           endelse
           if (N_ELEMENTS(xy) eq 2) then XYOUTS, xy[0], xy[1], ls, ALIGNMENT=latalign, NOCLIP=noclip, _EXTRA=extra
        endif
     ENDIF


     if dobox then begin
        dx = (xpixels[1] - xpixels[0]) * 0.01
        for j=0,1 do begin
           k = 0
           while ~finite(boxpos[i,j,1]) and abs(k) lt 3 do begin
              boxpos[i, j, 1] = find_grid_intersection( [xpixels[j]+dx*k, yboxpix[0]], [xpixels[j]+dx*k, yboxpix[1]], 1, curlat, map_structure=passedMap) 
              k = k + (j ? -1 : 1)
           endwhile
        endfor
     endif

  endfor

; draw meridians (curlon) and labels at 'lonlab'
  FOR i=0,n_passed_lons-1 DO BEGIN
     lon=longitude_list[i] & fudged_lon=lon
     fmt = (lon ne long(lon)) ? '(f7.2)' : '(i4)' ; future format
     if is_cyl then begin
        fudgefact = fudged_lon - myMap.p0lon & map_adjlon,fudgefact 
        if fudgefact eq -180 then fudged_lon += 1.0e-5 else if fudgefact eq 180 then fudged_lon -= 1.0e-5 
     endif

     IF (drawGrid) THEN begin ; we use fudged_lon to avoid split problems.
        x = REPLICATE(fudged_lon, N_ELEMENTS(latlist))
        y = latlist
        if (has_passed_map) then begin
           xy = map_proj_forward(x,y, MAP_STRUCTURE=passedMap, POLYLINES=polyconn) 
           n = N_ELEMENTS(xy)/2 
           if (n lt 2) then break ; not drawable
           index = 0L
           while (index lt n) do begin
              ipoly = polyconn[index + 1 : index + polyconn[index]]
              plots, xy[0,ipoly], xy[1,ipoly], zvalue, NOCLIP=0, _EXTRA=extra
              index = index + polyconn[index] + 1
           endwhile
        endif else begin
           PLOTS, x, y, NOCLIP=0, _EXTRA=extra
        endelse
     endif

     IF doLabelLon[i] THEN BEGIN
        IF i lt n_elements(lonstrings) then ls=map_grid_toformat(lonstrings[i],fmt) else ls=strtrim(string(lon, format=fmt),2)

        textArrayForLonLat[i,0] = ls
        if ~dobox then begin
           xy = 0; only one element if failed.
           if (has_passed_map) then begin
              uv = MAP_PROJ_FORWARD(fudged_lon, LatitudeOfLabels, MAP_STRUCTURE=passedMap)
              if (FINITE(uv[0]) and FINITE(uv[1])) then xy = uv[0:1]
           endif else begin
              if (noclip eq 1) or map_point_transformable(fudged_lon, LatitudeOfLabels) then xy = [fudged_lon, LatitudeOfLabels]
           endelse
           if (N_ELEMENTS(xy) eq 2) then XYOUTS, xy[0], xy[1], ls, ALIGNMENT=lonalign, NOCLIP=noclip, _EXTRA=extra
        endif

     ENDIF
     if dobox then begin
        dy = (ypixels[1] - ypixels[0]) * 0.01 
        for j=0,1 do begin
           k = 0
           while ~finite(boxpos[i,j,0]) and abs(k) lt 3 do begin
              boxpos[i, j, 0] = find_grid_intersection([xboxpix[0], ypixels[j]+k*dy],[xboxpix[1], ypixels[j]+k*dy], 0, lon, map_structure=passedMap)
              k = k + (j ? -1 : 1)
           endwhile
        endfor
     endif
  endfor

  if dobox then begin
     for axisnum=0,1 do begin 
        for axisside=0,1 do begin
           val = boxpos[*,axisside,axisnum]
           w = where(finite(val), count) & if (count eq 0) then continue
           dy = axisnum eq 1
           val = val[w]
           sorted = sort(val)
           val = val[sorted]
           labeltxt = (textArrayForLonLat[w,axisnum])[sorted]
           val0 = ([xpixels[0], ypixels[0]])[axisnum]
           xxp = xp + axisside * (xpixels[1]-xpixels[0] + box_thick)
           yyp = yp + axisside * (ypixels[1]-ypixels[0] + box_thick)
           xychar = [xchar[axisside], ychar[axisside]]
           for ii=0, count-1 do begin
              z = val[ii]
              if axisnum eq 0 then xxp = (ii eq (count-1) and (ii and 1) and ~labeltxt[ii]) ? [val0, xpixels[1], xpixels[1], val0] : [val0, z, z, val0] else yyp = [val0, val0, z, z]
              if (ii and 1) then polyfill, xxp, yyp, /device, color=bcolor
              xychar[axisnum] = z
              if strlen(labeltxt[ii]) gt 0 then xyouts, xychar[0], xychar[1], labeltxt[ii], orientation=dy * (90-180*axisside), align=0.5, clip=0, /device, _extra=extra
              val0 = z
           endfor
           if ii and 1 then begin ; finish filling
              if axisnum eq 0 then xxp = [val0, xpixels[1], xpixels[1], val0] else yyp = [val0, val0, ypixels[1], ypixels[1]]
              polyfill, xxp, yyp, /device, color=bcolor
           endif
        endfor
     endfor
  endif
end
