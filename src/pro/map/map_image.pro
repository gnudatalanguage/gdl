function  map_image, pristine, oxstart, oystart, oxsize, oysize, $
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
; good values (those that were not put at MISSING by the
; interpolation). MAX_VALUE and MIN_VALUE define above and below
; threshold where values are supposed MISSING. xstart, ystart,
; are return values giving the coordinates where the resulting image
; whoul be placed on the screen; xsize and ysize are the pixel width and
; height of the warped image (pass them to TV if the device is
; PostScript) 


  compile_opt idl2

  ON_ERROR,2
  doMymap=0
  if n_elements(missing) eq 0 then missing=((!d.flags and 512) ? !d.n_colors-1 : 0)
  if n_tags(map_structure) gt 0 then doMymap=1 else if (!x.type ne 3) then  message, "Current window must have map coordinates"
  
  sz = size(pristine, /dimensions)
  if (n_elements(sz) ne 2) then message, "Image must have 2 dimensions."
  
  norig = sz[0] & morig = sz[1]
  if (norig le 1) then message, 'dimension 1 must be greater than 1.'
  if (morig le 1) then message, 'dimension 2 must be greater than 1.'

; min and max of lons/lats, ignore arrays...
  latmin = (n_elements(latmin) eq 1) ? double(latmin) : -90.0d
  latmax = (n_elements(latmax) eq 1) ? double(latmax) : 90.0d
  lonmin = (n_elements(lonmin) eq 1) ? double(lonmin) : -180.0d
  lonmax = (n_elements(lonmax) eq 1) ? double(lonmax) : 180.0d

   if N_ELEMENTS(compress) eq 0 then compress=4 else compress=fix(compress)

    ; Missing value is the background if not supplied
    if N_ELEMENTS(missing) eq 0 then missing=(!D.FLAGS and 512) ? !D.N_COLORS-1 : 0

 ; value-to pixels linear relationship
   xref=0 & xval=lonmin & xinc=(lonmax-lonmin)/(norig-1)
   yref=0 & yval=latmin & yinc=(latmax-latmin)/(morig-1)
 
   box=long(convert_coord(!x.window,!y.window,/norm,/to_device))
   oxsize=box[3]-box[0]+1
   oysize=box[4]-box[1]+1
   oxstart=box[0]
   oystart=box[1]
 
; build the pixel's x and y

  x = REBIN(FINDGEN(oxsize) + oxstart, oxsize, oysize)
  y = REBIN(FINDGEN(1, oysize) + oystart,oxsize, oysize)

; convert to actual lons and lats (inside pristine)
  lonlat = convert_coord(temporary(x), temporary(y), /device, /to_data)
  if (doMymap) then begin
     lonlat=MAP_PROJ_INVERSE(lonlat,map_structure=map_structure)
  endif
    lat = reform(lonlat[1,*], oxsize, oysize, /OVER)       ; Separate lat/lon
    lon = reform(lonlat[0,*], oxsize, oysize, /OVER)
    lonlat = 0  ; free memory
    bad = where(~finite(lon) or ~finite(lat), count)
    if count gt 0 then begin
        lon[bad] = 1.0e10        ; make it not NaN but sufficiently faraway...
        lat[bad] = 1.0e10
    endif

    w = where(lon lt lonmin, count)        ;Handle longitude wrap-around
    while count gt 0 do begin
        lon[w] += 360.0
        w = where(lon lt lonmin, count)
    endwhile

; use scaling between coordinates and pixels in pristine:
;lon=xval+(x-xref)*xinc-> x=(lon-xval)/xinc+xref
  lon-=xval & lon/=xinc & lon+=xref
  lat-=yval & lat/=yinc & lat+=yref

; 2) interpolate  
  warped=INTERPOLATE(pristine, lon , lat , MISSING = missing)

  return, warped
end

; test
R = READ_TIFF("~/PACKAGES/NE1_50M_SR_W/NE1_50M_SR_W.tif");"~/gdl/resource/maps/low/NE1_50M_SR_W/NE1_50M_SR_W.tif")
;r=rebin(temporary(result),3,1080,540)
;r=reverse(r,3,/over)
sz=size(r,/dimensions)
; transformation parameters (as stored in original .twf file)
A  =        0.03333333333333 
D  =        0.00000000000000 
B  =        0.00000000000000 
E  =        -0.03333333333333 
C  =        -179.98333333333333 
F  =        89.98333333333333
x=0 & y =0 & lonmin=A*x + B*y + C & latmin=D*x + E*y + F
x=sz[1]-1 & y =sz[2]-1 & lonmax=A*x + B*y + C & latmax=D*x + E*y + F
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
