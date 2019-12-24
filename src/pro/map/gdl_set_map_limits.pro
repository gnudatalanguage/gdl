; ancillary MAP* procedure, set limits (uv_box and ll_box) for maps.
pro gdl_set_map_limits, myMap, limits
  
  compile_opt idl2, hidden
  
; map_limits may have 4 or 8 elements.
  
  if n_elements(limits) ne 8 and n_elements(limits) ne 4 then  message, 'Map limit must have 4 or 8 points'
  map_limits=limits ; do not change limits
  xmin=-1d & ymin=-1d & xmax=1d & ymax=1d
  
  if n_elements(map_limits) eq 4 then begin
; filter map_limits to [-180,+180] in both directions however keeping orientation
     lonmin=map_limits[1] & lonmax=map_limits[3] & latmin=map_limits[0] & latmax=map_limits[2]
; longitudes: beautify
     map_adjlon,lonmin
     map_adjlon,lonmax
     if (lonmin gt lonmax) then  begin temp=lonmin & lonmin=lonmax & lonmax=temp & end
     if (lonmin eq lonmax) then  begin lonmin=-180 & lonmax=180 & endif
; latitudes: beautify
     latmin = -90 > latmin < 90
     latmax = -90 > latmax < 90
     if (latmin gt latmax) then  begin temp=latmin & latmin=latmax & latmax=temp & end
     if (latmin eq latmax) then  begin latmin=-90 & latmax=90 & endif

     map_limits=[latmin, lonmin, latmax, lonmax]
     if ( map_limits[0] eq  map_limits[2])  then message,/informational,"GDL_SET_MAP_LIMITS: Warning, MAP limits are invalid."
; range of values
       lonrange = lonmax - lonmin
       latrange = latmax - latmin
; is there another way (proj4) to get ranges except brute force on a grid of possible points?
; epsilon useful as projections are not precise (see #define EPS in proj4 c files: apparently < 1e-6)
       epsx = 1d-6*ABS(lonrange)
       epsy = 1d-6*ABS(latrange)
       
       n_lons = 90
       lons = [ lonmin + epsx, DINDGEN(n_lons)*(lonrange/(n_lons-1d)) + lonmin, lonmax - epsx] & n_lons += 2
       
       n_lats = 45
       lats =  [latmin + epsy, DINDGEN(n_lats)*(latrange/(n_lats-1d)) + latmin, latmax - epsy] & n_lats += 2 
     if ((latmin lt 0) && (latmax gt 0)) then begin lats = [lats, -epsy, epsy] &  n_lats += 2 & end
       
       lons = reform(rebin(lons, n_lons, n_lats), 1, n_lons*n_lats)
       lats = reform(rebin(transpose(lats), n_lons, n_lats), 1, n_lons*n_lats)
       tmp = [temporary(lons), temporary(lats)]
       
       xy = map_proj_forward(tmp, MAP=myMap)
                                ; Default if no points are valid is just the map_limits.
       good = WHERE(FINITE(xy[0,*]) and FINITE(xy[1,*]), ngood)
       
       if (ngood gt 0) then begin
         xy = xy[*, good]
         lonlat = tmp[*, good]
                                ; further check: are backprojected good points really close to original point?
         tmp = MAP_PROJ_INVERSE(xy, MAP=myMap)
         bad = WHERE(~FINITE(tmp[0,*]) or ~FINITE(tmp[1,*]), nbad)
         if (nbad gt 0) then tmp[*, bad] = -9999
         diff = ABS(lonlat - tmp)
         diff[0,*] = diff[0,*] mod 360 ; Ignore 360 degre differences for longitude.
         w = where(diff[0,*] gt 359.99, count) & if count gt 0 then diff[0,w] = 0
         w = where((abs(tmp[0,*]) le 720) and (abs(tmp[1,*]) le 90) and (total(diff,1) lt 1d), count)
         if (count gt 0) then begin ; Only those good during forward and inverse projection.
           lonlat = lonlat[*, w]
           xy = xy[*,w]
         endif
         xmin = min(xy[0,*], max=xmax)
         ymin = min(xy[1,*], max=ymax)
         lonmin = min(lonlat[0,*], max=lonmax)
         latmin = min(lonlat[1,*], max=latmax)
       endif
       
  endif else begin
; easy: convert and pray.
; 8 point limit as in [latLeft,lonLeft, latTop, lonTop, LatRight, lonRight, LatBottom, LonBottom]
;
     lons=map_limits[[1,3,5,7]] & lats=map_limits[[0,2,4,6]]
     xy=map_proj_forward(lons,lats,map=myMap)
     good = WHERE(FINITE(xy[0,*]) and FINITE(xy[1,*]), ngood)
     if (ngood ge 2 ) then begin
        xmin = min(xy[0,*], max=xmax)
        ymin = min(xy[1,*], max=ymax)
        lonmin = min(lons, max=lonmax)
        latmin = min(lats, max=latmax)
     endif else message, 'Unmappable limit point(s) in LIMIT keyword'
  endelse

                                ; Fill in map structure.
  myMap.ll_box = [latmin, lonmin, latmax, lonmax]
  myMap.uv_box = [xmin, ymin, xmax, ymax]

end

