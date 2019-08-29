function  map_image, pristine, xstart, ystart, xsize, ysize, $
                     LATMIN = latmin, LATMAX = latmax,  $
                     LONMIN = lonmin, LONMAX = lonmax,  $
                     BILINEAR = bilinear, $
                     COMPRESS = compress, $
                     SCALE = scale, $
                     MASK=mask, $
                     MAP_STRUCTURE=map_structure, $
                     MAX_VALUE = max_value, MIN_VALUE=min_value, $
                     MISSING = missing, $
                     WHOLE_MAP=dummy

; reprojects (warps) the pristine image (array) on the current map
; latmin etc serves to give the min and max [lon,lat] of the image
; (rows=lats, columns=lons). Interpolation can be BILINEAR. SCALE is
; used for PostScript etc that have scalable pixels, so any number of
; output pixels (ppi) is possible, default is 0.02 pixel by graphic
; coordinate. MASK is an array of same size of the result indicating
; good values (thse that were not put at MISSING by the
; interpolation). MAX_VALUE and MIN_VALUE define above and below
; threshold where valuse are supposed MISSING. xstart, ystart,
; are return values giving the coordinates where the resulting image
; whoul be placed on the screen; xsize and ysize are the pixel width and
; height of the warped image (pass them to TV if the device is
; PostScript) 


  compile_opt idl2

  ON_ERROR,2
  doMymap=0
  if n_elements(missing) eq 0 then missing=((!d.flags and 512) ? !d.n_colors-1 : 0)
  if n_tags(map_structure) gt 0 then doMymap=1 else if (!x.type ne 3) then  message, "Current window must have map coordinates"
  
  dims = size(pristine, /dimensions)
  if (n_elements(dims) ne 2) then message, "Image must have 2 dimensions."
  
  n = dims[0] & m = dims[1]
  if (n le 1) then message, 'dimension 1 must be greater than 1.'
  if (m le 1) then message, 'dimension 2 must be greater than 1.'

; min and max of lons/lats, ignore arrays...
  minlat = (n_elements(latmin) eq 1) ? double(latmin) : -90.0d
  maxlat = (n_elements(latmax) eq 1) ? double(latmax) : 90.0d
  minlon = (n_elements(lonmin) eq 1) ? double(lonmin) : -180.0d
  maxlon = (n_elements(lonmax) eq 1) ? double(lonmax) : 180.0d
  abs_minlat = min([minlat,maxlat],max=abs_maxlat)
  abs_minlon = min([minlon,maxlon],max=abs_maxlon)
; value-to pixels linear relationship
  xref=0 & xval=minlon & xinc=(maxlon-minlon)/n
  yref=0 & yval=minlat & yinc=(maxlat-minlat)/m
; test this is sufficient
  map_adjlon,minlon
  map_adjlon,maxlon
; there may be a problem if the image "warps". to be adressed later.
; this is the function to use when 'pristine' has more pixels than the
; destination (in the contrary use MAP_PATCH). We find the output size
; (as it is given by the pixels in the existing plot (!x.type=3) that
; contain all the image 'points') and then use INTERPOLATE for the
; conversion.
; 1) find desired pixel size
; brute-force test of a grid of values sampling the image, the number
; of points being somehow to be adjusted.
; pristine map coverage already known
; however plotted map region may be much less:
  normxmin=!x.crange[0]*!x.s[1]+!x.s[0]
  normxmax=!x.crange[1]*!x.s[1]+!x.s[0]
  normymin=!y.crange[0]*!y.s[1]+!y.s[0]
  normymax=!y.crange[1]*!y.s[1]+!y.s[0]
  lat = replicate((normymax-normymin)/(31),32) # findgen(32) +normymin
  lon = normxmin + findgen(32) # replicate((normxmax-normxmin)/31,32)
  uv = convert_coord(temporary(lon),temporary(lat),/norm,/to_data)
  plotted_minlon=min(uv[0,*],max=plotted_maxlon,/nan)
  plotted_minlat=min(uv[1,*],max=plotted_maxlat,/nan) & uv=0b
; plotted_minlon etc are by construction with positive increments and
; abs_xxx are also with positive increments

; if, due to a problem in convert_coord, the results has NaNs, abort.

  if ~finite(plotted_minlon+plotted_minlat+plotted_maxlon+plotted_maxlat) then message, "projection has no inverse, unable to proceed."
; we could eliminate the case when the plotted region is not in the map.
; but IDL only chokes on zero-size arrays instead (which is oK also
; but not so well informative).
  abs_minlat=abs_minlat > plotted_minlat
  abs_minlon=abs_minlon > plotted_minlon
  abs_maxlat=abs_maxlat < plotted_maxlat
  abs_maxlon=abs_maxlon < plotted_maxlon

  nx=n<32 & ny=m<32

  lat = replicate((abs_maxlat-abs_minlat)/(ny-1),ny) # findgen(ny) +abs_minlat
  lon = abs_minlon + findgen(nx) # replicate((abs_maxlon-abs_minlon)/(nx-1),nx)
; uv range:
  if (doMymap) then begin
     uv= map_proj_forward(temporary(lon), temporary(lat), MAP_STRUCTURE=map_structure)
     v = reform(uv[1,*])
     u = reform(uv[0,*])
     w = where(finite(u) and finite(v))
     u = u[w]
     v = v[w]
     uv = convert_coord(temporary(u), temporary(v), /data, /to_device)
     x = reform(uv[0,*])
     y = reform(uv[1,*])
  endif else begin
     uv = convert_coord(temporary(lon), temporary(lat), /data, /to_device)
     x = reform(uv[0,*])
     y = reform(uv[1,*])
     w = where(finite(x) and finite(y))
     x = x[w]
     y = y[w]
  endelse
; must  somewhere clip x and y inside the current box, as xstart must
; be the box xmin in device coords.
  ll=convert_coord(!x.window,!y.window,/norm,/to_device)
  n_box=ll[3]-ll[0]+1
  m_box=ll[4]-ll[1]+1
  w=where(x lt ll[0] or x gt ll[3] or y lt ll[1] or y gt ll[4], count, comp=now)
  if count then begin
     x=x[now]
     y=y[now]
  endif
; the pixels really covered by the plotted (yet unexisting) map are 
  xstart=min(temporary(x), MAX=maxpx) > ll[0]
  ystart=min(temporary(y), MAX=maxpy) > ll[1]
; so the number and position of device pixels is...
  xsize = long(maxpx-xstart+1) < n_box
  ysize = long(maxpy-ystart+1) < m_box

; find the pixels in 'pristine' where these output pixels map:
; build the pixel's x and y

  x = xstart + findgen(xsize) # replicate(1,ysize)
  y = replicate(1,xsize) # findgen(ysize) + ystart

; convert to actual lons and lats (inside pristine)
  uv = convert_coord(temporary(x), temporary(y), /device, /to_data)
  uv=uv[[0:1],*]
  if (doMymap) then begin
     uv=MAP_PROJ_INVERSE(uv,map_structure=map_structure)
  endif
  lon = reform(uv[0,*], xsize, ysize)
  lat = reform(uv[1,*], xsize, ysize) & uv = 0b ; empty uv 
; handle bad points?
;w = where(~finite(lon) or ~finite(lat), count)
;if count gt 0 then begin
;   lon[w] = 1.0e10
;   lat[w] = 1.0e10
;endif
; lons and lats in pristine -> to pixels 
; use scaling between coordinates and pixels in pristine:
;lon=xval+(x-xref)*xinc-> x=(lon-xval)/xinc+xref
  x=(temporary(lon)-xval)/xinc+xref
  y=(temporary(lat)-yval)/yinc+yref
; 2) interpolate  
  warped=INTERPOLATE(pristine, x, y, MISSING = missing)

  return, warped
end

; test
R = READ_TIFF("~/PACKAGES/NE1_50M_SR_W/NE1_50M_SR_W.tif");"~/gdl/resource/maps/low/NE1_50M_SR_W/NE1_50M_SR_W.tif")
;r=rebin(temporary(result),3,1080,540)
;r=reverse(r,3,/over)
dims=size(r,/dimensions)
; transformation parameters (as stored in original .twf file)
A  =        0.03333333333333 
D  =        0.00000000000000 
B  =        0.00000000000000 
E  =        -0.03333333333333 
C  =        -179.98333333333333 
F  =        89.98333333333333
x=0 & y =0 & lonmin=A*x + B*y + C & latmin=D*x + E*y + F
x=dims[1]-1 & y =dims[2]-1 & lonmax=A*x + B*y + C & latmax=D*x + E*y + F
map_set,48.83,-2.33,name="gnomic",scale=3e6,/iso,title='Zoom on Brittany, Gnomic Projection'
for i=0,2 do begin & z=map_image(reform(r[i,*,*]),Startx,Starty,latmin=latmin,latmax=latmax,lonmin=lonmin,lonmax=lonmax) & tv,z,startx,starty,chan=i+1 & endfor
map_continents,/hi
map_grid,box_axes=1,color='1260E2'x,glinethick=1,glinestyle=0,latdel=1,londel=1
;map_grid,box_axes=1,color='1260E2'x,glinethick=1,glinestyle=0,latdel=10,londel=15
R = READ_TIFF("~/gdl/resource/maps/high/NE_SUBSET.tiff")
map_set,48.83,-2.33,name="gnomic",scale=3e7,/iso,title='Europa, Gnomic Projection',e_cont={cont:1,fill:1,color:'33e469'x,hires:1},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x}
for i=0,2 do begin & z=map_image(reform(r[i,*,*]),Startx,Starty,lonmin=-10.5149147727,latmin=59.3309659091,lonmax=10.6044034091,latmax=41.2542613636) & tv,z,startx,starty,chan=i+1 & endfor
map_continents,/hi
map_grid,box_axes=1,color='1260E2'x,glinethick=1,glinestyle=0,latdel=10,londel=10


end
