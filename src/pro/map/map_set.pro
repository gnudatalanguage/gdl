; MAP_SET
; Documentation in progress
; One major difference with IDL's one is that, using
; name="xxxx", one can use any of the ~120 currently defined
; projections in PROJ library
; The structure of the !MAP system variable is not compatible with
; IDL, i.e., one cannot use a GDL-defined !MAP in IDL, but one can use
; and IDL-defined !MAP (or mapstruct) in GDL.
;
PRO MAP_SET, lat, lon, rot, $
; kw for map_proj_init, essentially
  PROJECTION=proj, $              
  NAME=nam, $                    
  LIMIT = limit, $    ; the 4 or 8 point lat/lon limit, see map_proj_init
  STEREOGRAPHIC = stereographic, $
  ORTHOGRAPHIC = orthographic, $
  CONIC = conic, $
  LAMBERT = lambert, $
  GNOMIC = gnomic, $
  AZIMUTHAL = azimuthal, $
  SATELLITE = satellite, $      
  CYLINDRICAL = cylindrical, $
  MERCATOR = mercator, $
  MILLER_CYLINDRICAL=miller_cylindrical, $
  MOLLWEIDE = mollweide, $
  SINUSOIDAL = sinusoidal, $
  AITOFF = aitoff, $            
  HAMMER = hammer, $            
  ALBERS = albers, $            
  TRANSVERSE_MERCATOR = transverse_mercator, $  
  ROBINSON = robinson, $        
  GOODESHOMOLOSINE = goodeshomolosine , $  
; special: condensed form for 2 standar parallels...
  STANDARD_PARALLELS = standard_parallels  , $ 
; ellipsois has a different meaning as in map_proj_init!!!
  ELLIPSOID=ellipsoid,$
; and 3 satellite projection  parameters.
  SAT_P = Sat_p, $
; central_azimuth for Mercator, Cylindrical, Miller, Mollweide, and
; Sinusoidal. works ? only for Mercator
  CENTRAL_AZIMUTH=cent_azim, $
; passing Center_azimuth IS an offence.
  CENTER_AZIMUTH=bad_center, $
; MAP_SET:
  CLIP=clip, $  ;Default = do map specific clipping, CLIP=0 to disable (yet unused)
  REVERSE=reverse, $ ; TODO: 
  SCALE=scale, $     ; 
  ISOTROPIC = isotropic, $, 
; graphics keywords:
  NOERASE=noerase, TITLE=title,$
  ADVANCE = advance, COLOR=color, POSITION = position, $
  NOBORDER=noborder, T3D=t3d, ZVALUE=zvalue, $
  CHARSIZE = charsize, XMARGIN=xmargin, YMARGIN=ymargin, $
; MAP_HORIZON keywords:
  HORIZON=horizon, E_HORIZON=ehorizon, $ 
; MAP_CONTINENTS keywords:
  CONTINENTS = continents, E_CONTINENTS=econt, $ 
  USA=usa, HIRES = hires, $
  MLINESTYLE=mlinestyle, MLINETHICK=mlinethick, CON_COLOR=con_color, $
; MAP_GRID keywords:
  GRID=grid, E_GRID=egrid, $
  GLINESTYLE=glinestyle, GLINETHICK=glinethick, $
  LABEL=label, LATALIGN=latalign, LATDEL=latdel, LATLAB=latlab, $
  LONALIGN=lonalign, LONDEL=londel, LONLAB=lonlab, $
; compatibility, ignored.
  WHOLE_MAP=whole_map,$
; extra to add non-idl keywords, like proj-like keywords. Has some implications.
  _extra=extra


  ON_ERROR, 2                   ; return to caller

; limit vs. scale
 doiso=n_elements(isotropic) gt 0
 doscale=n_elements(scale) gt 0
 dolimit=n_elements(limit) gt 0
  if doscale and dolimit then begin 
     message, 'Conflicting keywords specified: LIMIT and SCALE', /INFO
     dolimit=0
  endif
  
; defaults
  if n_params() lt 3 then rot = 0.0d0
  if n_params() lt 2 then lon = 0d0
  if n_params() lt 1 then lat = 0d0

  if abs(lat) gt 90.0 then message,'Latitude must be in range of +/- 90 degrees'
  if abs(lon) gt 360.0 then message,'Longitude must be in range of +/- 360 degrees'

; lon better be between -180 and 180
  map_adjlon,lon

  if n_elements(color) eq 0 then color = !p.color ;Default color
  if n_elements(title) eq 0 then title = " "
  if n_elements(t3d) le 0 then t3d = 0
  if n_elements(zvalue) eq 0 then zvalue = 0.
  if n_elements(charsize) eq 0 then charsize = !p.charsize
  if charsize le 0.0 then charsize = 1.0
  if n_elements(clip) eq 0 then clip = 1

  doAdvance=keyword_set(advance)
  erase=~keyword_set(noerase)
  doborder=~keyword_set(noborder)

;very dangerous: permit center_azimuth in the list of keywords:
  if n_elements(bad_center) gt 0 then Message,"Wrong CENTER_AZIMUTH keyword, do you mean CENTRAL_AZIMUTH?"
  
; explode a few parameters
  if n_elements(sat_p) gt 0 then begin
     map_struct_append, extra, "HEIGHT",sat_p[0] 
     map_struct_append, extra, "SAT_TILT",sat_p[1]
     map_struct_append, extra, "CENTER_AZIMUTH",sat_p[2] 
  endif
  if n_elements(ellipsoid) gt 0 then begin
     if n_elements(ellipsoid) ne 3 then message, "ELLIPSOID must be a 3-element array"
     a=ellipsoid[0] & e2=ellipsoid[1]  & b=a*sqrt(1-e2^2)
     map_struct_append, extra, "SEMIMAJOR_AXIS",a 
     map_struct_append, extra, "SEMIMINOR_AXIS",b
  endif
  if keyword_set(STEREOGRAPHIC) then nam = 'stereographic'
  if keyword_set(ORTHOGRAPHIC) then nam = 'orthographic'
  if keyword_set(CONIC) then nam = 'lambert conic'
  if keyword_set(LAMBERT) then nam = 'Lambert Azimuthal'
  if keyword_set(GNOMIC) then nam = 'gnomonic'
  if keyword_set(AZIMUTHAL) then nam = 'azimuthal equidistant'
  if keyword_set(SATELLITE) then nam = 'satellite'
  if keyword_set(CYLINDRICAL) then nam = 'equidistant cylindrical' ; idl:cylindrical is just equidistant cylindrical
  if keyword_set(MERCATOR) then begin
     if n_elements(cent_azim) gt 0 then begin
        map_struct_append, extra,"ALPHA",cent_azim
        map_struct_append, extra,"LONC",lon
        lon=0
        nam = 'oblique mercator'
     endif else begin
        nam = 'mercator'
     endelse
  endif
  if keyword_set(MILLER_CYLINDRICAL) then nam='miller cylindrical'
  if keyword_set(MOLLWEIDE) then nam = 'mollweide'
  if keyword_set(SINUSOIDAL) then nam = 'sinusoidal'
  if keyword_set(AITOFF) then nam = 'aitoff'
  if keyword_set(HAMMER) then nam = 'hammeraitoff'
  if keyword_set(ALBERS) then nam = 'albers equal area conic'
  if keyword_set(TRANSVERSE_MERCATOR) then nam = 'transverse mercator'
  if keyword_set(ROBINSON) then nam = 'robinson'
  if keyword_set(GOODESHOMOLOSINE) then nam = 'interrupted goode'

  if n_elements(cent_azim) gt 0 then begin
     if keyword_set(MILLER_CYLINDRICAL) then map_struct_append, extra, "CENTER_AZIMUTH",cent_azim 
     if keyword_set(MOLLWEIDE) then map_struct_append, extra, "CENTER_AZIMUTH",cent_azim 
     if keyword_set(CYLINDRICAL) then map_struct_append, extra, "CENTER_AZIMUTH",cent_azim 
     if keyword_set(SINUSOIDAL) then map_struct_append, extra, "CENTER_AZIMUTH",cent_azim 
  endif

; map_set of a conic without arguments needs to set a standard parallel.
  if strpos(strupcase(nam),"CONIC") ge 0  then begin
    if n_elements(standard_parallels) eq 0 then standard_parallels=[45,80]
    if n_elements(standard_parallels) eq 1 then standard_parallels=[standard_parallels[0],80] ; /silly and wrong
  endif

  if n_elements(standard_parallels) gt 0 then map_struct_append, extra, "STANDARD_PAR1",standard_parallels[0] 
;  if n_elements(standard_parallels) gt 0 then map_struct_append, extra, "STANDARD_PARALLEL",standard_parallels[0] 
  if n_elements(standard_parallels) gt 1 then map_struct_append, extra, "STANDARD_PAR2",standard_parallels[1] 

  if dolimit then begin
   ; We use sphere_radius=1 as all the rest (grid and horizon) are based on that 
     !map=map_proj_init( keyword_set(nam)?nam:keyword_set(projection)?projection:"cylindrical", sphere_radius=1d, center_latitude=lat, center_longitude=lon, rotation=rot, limit=limit, clip=clip, _extra=extra)
  endif else begin
     !map=map_proj_init( keyword_set(nam)?nam:keyword_set(projection)?projection:"cylindrical", sphere_radius=1d, center_latitude=lat, center_longitude=lon, rotation=rot, clip=clip, _extra=extra)
  endelse

  if  keyword_set(GNOMIC) then !map.uv_box=[-2,-2,2,2] 

  uvrange=!map.uv_box



  


  ; reverse? TODO

  ; graphic passable keywords
  map_struct_append, egraphics, "COLOR", color
  map_struct_append, egraphics, "T3D", t3d
  map_struct_append, egraphics, "ZVALUE", zvalue

; Initial erase?
  if !P.multi[0] eq 0 and doAdvance then erase
  if erase and ~doAdvance THEN erase
; add a 1% margin if box is asked for
  if (doborder) then begin
     urange=uvrange[0]-uvrange[2]
     vrange=uvrange[1]-uvrange[3]
     urangedel=urange*0.01
     vrangedel=vrange*0.01
     uvrange[0]+=urangedel
     uvrange[2]-=urangedel
     uvrange[1]+=vrangedel
     uvrange[3]-=urangedel
  endif 

; the real thing
; apparently setting the clip window may help. !P?clip is in pixels
; (device coordinates):
    clipx = (uvrange[[0,2]] * !x.s[1] + !x.s[0]) * !d.x_size
    !p.clip[[0,2]] = [clipx[0] < clipx[1], clipx[0] > clipx[1]]
    clipy = (uvrange[[1,3]] * !y.s[1] + !y.s[0]) * !d.y_size
    !p.clip[[1,3]] = [clipy[0] < clipy[1], clipy[0] > clipy[1]]
  
  plot,[0,1],/noerase,/nodata,xsty=5,ysty=5, position=position, charsize=charsize, color=color,title=title

    midx = total(!x.window)/2.
    midy = total(!y.window)/2.

    if doscale then begin       ; scale has preference over iso
     x_size = !x.window[1]-!x.window[0]
     y_size = !y.window[1]-!y.window[0]
                                ; construct a limit using scale
     map_proj_info, /CURRENT, SCALE=meters ; scale is in meters per UV unit. 
     
     ourUvRange = meters/scale  ; ourUvRange is desired range in uv units (normalized).
     windowSizeM = !d.x_size / !d.x_px_cm / 100. ;width of window in meters
     !x.s[1] = ourUvRange / windowSizeM
     !y.s[1] = !x.s[1] * !d.x_size / !d.y_size ; aspect ratio.
     ourhalfuv = [x_size / !x.s[1], y_size / !y.s[1]] / 2.0        ;half the UV range
     uvrange = [-ourhalfuv[0] , -ourhalfuv[1] , ourhalfuv[0], ourhalfuv[1]] ;New uv range
     !map.uv_box = uvrange
  endif else if doiso then begin ;isotropic is a sort of scale
 ; Scale in pixels/uvunit, correct for ISOMETRIC aspect ratio. Use smaller
 ; of X and Y scale factors
       x_size = !x.window[1]-!x.window[0]
      y_size = !y.window[1]-!y.window[0]

     sx = x_size * !d.x_size /(uvrange[2]-uvrange[0]) ;X scale
    sy = y_size * !d.y_size/(uvrange[3]-uvrange[1]) ;Y scale

    !x.s[1] = (sx < sy) / !d.x_size ;Set smaller
    !y.s[1] = !x.s[1] * !d.x_size / !d.y_size
    if sx gt sy then $          ;Resize window to fit map area
      !x.window = midx + sy/sx/2 * [-x_size, x_size] $
    else !y.window = midy + sx/sy/2 * [-y_size, y_size]
    ;; sizex = (!x.window[1]-!x.window[0]) * !d.x_size/(uvrange[2]-uvrange[0])
    ;; sizey = (!y.window[1]-!y.window[0]) * !d.y_size/(uvrange[3]-uvrange[1])
    ;;                             ; take smaller of the two sizes,
    ;;                             ; notice that we align the box on the
    ;;                             ; center of the screen
    ;; ratio=sizex/sizey/2.
    ;; if (sizex le sizey) then begin
    ;;    !x.s[1] = sizey / !d.x_size
    ;;    !y.s[1] = !x.s[1] * !d.x_size / !d.y_size
    ;;    !y.window = midy + ratio * [-y_size, y_size]
    ;; endif else begin
    ;;    !x.s[1] = sizex / !d.x_size
    ;;    !y.s[1] = !x.s[1] * !d.x_size / !d.y_size
    ;;    !x.window = midx + ratio * [-x_size, x_size]
    ;; endelse
  endif else begin ; default case, just set ![x|y].s :
     x_size = !x.window[1]-!x.window[0]
     y_size = !y.window[1]-!y.window[0]
    !x.s[1] = x_size/(uvrange[2]-uvrange[0])
    !y.s[1] = y_size/(uvrange[3]-uvrange[1])
  endelse

; reverse? TODO
; Compute offsets to center UV rectangle in the window
!x.s[0] = midx - (uvrange[0]+uvrange[2])/2. * !x.s[1]
!y.s[0] = midy - (uvrange[1]+uvrange[3])/2. * !y.s[1]

  if doborder then plots, !x.window[[0,1,1,0,0]], !y.window[[0,0,1,1,0]], COLOR=color, zvalue, /NORM, /NOCLIP, T3D=t3d

  !x.type=3

; map_horizon? first, because of possibility to be filled... 

  if keyword_set(horizon) or keyword_set(ehorizon) then begin
     merge_structs_mapset, ehorizon, egraphics ;Add common graphics keywords
     MAP_HORIZON, _EXTRA=ehorizon
  endif

  ; map_continents next
  if n_elements(mlinestyle) then map_struct_append, econt, "LINESTYLE", mlinestyle
  if n_elements(mlinethick) then map_struct_append, econt, "THICK", mlinethick
  if n_elements(con_color) then map_struct_append, econt, "COLOR", con_color
  if n_elements(hires) then map_struct_append, econt, "HIRES", hires
  if n_elements(continents) then map_struct_append, econt, "CONTINENTS", continents
  if n_elements(usa) then map_struct_append, econt, "USA", usa
  if n_elements(econt) gt 0 or keyword_set(continents) then begin
    merge_structs_mapset, econt, egraphics ;Add common graphics kwrds
    MAP_CONTINENTS, _EXTRA=econt
  endif
; map_grid last above all
  if n_elements(label)  then map_struct_append, egrid, "LABEL", label
  if n_elements(latlab) then map_struct_append, egrid, "LATLAB", latlab
  if n_elements(lonlab) then map_struct_append, egrid, "LONLAB", lonlab
  if n_elements(latdel) then map_struct_append, egrid, "LATDEL", latdel
  if n_elements(londel) then map_struct_append, egrid, "LONDEL", londel
  if n_elements(latalign) then map_struct_append, egrid, "LATALIGN", latalign
  if n_elements(lonalign) then map_struct_append, egrid, "LONALIGN", lonalign
  do_grid = (keyword_set(grid) + n_elements(egrid) + n_elements(glinestyle) + n_elements(glinethick)) ne 0
  
  merge_structs_mapset, egrid, egraphics
  if n_elements(glinestyle) then map_struct_append, egrid, "LINESTYLE", glinestyle
  if n_elements(glinethick) then map_struct_append, egrid, "THICK", glinethick
  if do_grid then MAP_GRID, CHARSIZE=charsize, _EXTRA=egrid


  if doAdvance and !P.Multi[0] gt 0 then !P.Multi[0] = !P.Multi[0] - 1 else !p.multi[0] = !p.multi[1] * !p.multi[2] - 1

end
