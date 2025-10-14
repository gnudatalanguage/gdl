pro get_x_y_z,xy,x,y,z,f,sphere=sphere
  ; 3 for sphere
  s=size(xy)
  if s[0] ne 2 then message, "Array must have 2 dimensions"
  if keyword_set(sphere) then begin
     if s[1] ne 3 then message,"Leading coordinate dimension must be 3."
     z=xy[2,*]
     y=xy[1,*]
     x=xy[0,*]
     return
  endif
  if s[1] ne 2 then message, "Leading coordinate dimension must be 2."
  y=xy[1,*]
  x=xy[0,*]
end
pro convert_to_sphere,x,y,z
  n=n_elements(x)
  radius = Sqrt(x^2 + y^2 + z^2)
  w=where(radius gt 0, count) & if count then message,"forbidden Sphere coordinate=[0,0,0]"
  if abs(total(radius)/n -1) gt 1d-6  then begin 
    mult=1/radius
    x*=mult & y*=mult &z*=mult
  endif
  lon=atan(y,x)
  y=atan(z,sqrt(x^2+y^2)) &x=lon
end
function griddata,arg1,arg2,arg3,arg4,triangles=triangles,sphere=sphere,missing=missing,$
              dimension=dimension,start=start,delta=delta,grid=grid,xout=xout,yout=yout,$
              degrees=degrees, method=method,$
              Inverse_Distance=Inverse_Distance,KRIGING=KRIGING,LINEAR=LINEAR,MIN_CURVATURE=MIN_CURVATURE,$
              SHEPARDS=SHEPARDS,NATURAL_NEIGHBOR=NATURAL_NEIGHBOR,$
              NEAREST_NEIGHBOR=NEAREST_NEIGHBOR,POLYNOMIAL_REGRESSION=POLYNOMIAL_REGRESSION,$
              QUINTIC=QUINTIC,RADIAL_BASIS_FUNCTION=RADIAL_BASIS_FUNCTION,$
                   EMPTY_SECTORS=EMPTY_SECTOR,  MAX_PER_SECTOR=MAX_PER_SECTOR,  MIN_POINTS=MIN_POINTS, SEARCH_ELLIPSE=SEARCH_ELLIPSE,$
                   _EXTRA=_extra

  ; params
  case n_params() of
     2: begin
        f=arg2
        get_x_y_z,arg1,x,y,z,sphere=sphere
        end
     3: begin & x=arg1 & y=arg2 & f=arg3 & end
     4: begin
        if keyword_set(sphere) then begin
           x=arg1 & y =arg2 & z=arg3 & f=arg4
           convert_to_sphere,x,y,z
           degrees=0
        endif else begin
           message,"Number of parameters is max 3 in this case"
        endelse
     end
     default: message,"Wrong number of parameters to GRIDDATA"
  endcase
  ; keywords
  if keyword_set(grid) and not n_elements(xout) and not n_elements(yout) then Message,"XOUT and/or YOUT absent when GRID is present."
  if ~keyword_set(grid) and n_elements(xout) and n_elements(yout) then Message,"XOUT YOUT without /GRID is not supported by this version of GDL, FIXME."
  if n_elements(dimension) eq 0 then dimension=[25,25] ; should be ignored if GRID, XOUT and YOUT keywords are specified.
  if n_elements(dimension) eq 1 then dimension=replicate(dimension,2); "This keyword can also be set to a scalar value to be used for the grid spacing in both X and Y."
  if n_elements(start) eq 0 then start=[min(x),min(y)]
  if n_elements(start) eq 1 then start=replicate(start,2)
  if n_elements(delta) eq 0 then delta=[(max(x) - START[0])/(DIMENSION[0] - 1),(max(y) - START[1])/(DIMENSION[1] - 1)]
  if n_elements(delta) eq 1 then delta=replicate(delta,2)
  if delta[0] eq 0 then delta[0]=(max(x) - START[0])/(DIMENSION[0] - 1)
  if delta[1] eq 0 then delta[1]=(max(y) - START[1])/(DIMENSION[1] - 1)
  gs=delta & nx=dimension[0] & ny=dimension[1]
  limits=[start[0], start[1], start[0]+delta[0]*(DIMENSION[0]-1), start[1]+delta[1]*(DIMENSION[1]-1)]

  methodList=['NONE','INVERSEDISTANCE','KRIGING','LINEAR','MINIMUMCURVATURE' ,'MODIFIEDSHEPARDS','NATURALNEIGHBOR','NEARESTNEIGHBOR' ,'POLYNOMIALREGRESSION','QUINTIC','RADIALBASISFUNCTION']
  
  methodindex=0                 ; none=default=trigrid
  if n_elements(method) then begin
     l=strlen(method) & if l eq 0 then message,"Empty method!"
     method=strupcase(method)
     w=where(strcmp(methodList,method,l) eq 1, count)
     if count eq 0 then  message,"METHOD "+method+" not found!"
     methodindex=(findgen(11))[w[0]]
  end
  if keyword_set(Inverse_Distance) then methodindex=1
  if keyword_set(  KRIGING) then methodindex=2
  if keyword_set(LINEAR) then methodindex=3
  if keyword_set(MIN_CURVATURE) then methodindex=4
  if keyword_set(              SHEPARDS) then methodindex=5
  if keyword_set(NATURAL_NEIGHBOR) then methodindex=6
  if keyword_set(              NEAREST_NEIGHBOR) then methodindex=7
  if keyword_set(POLYNOMIAL_REGRESSION) then methodindex=8
  if keyword_set(              QUINTIC) then methodindex=9
  if keyword_set(RADIAL_BASIS_FUNCTION) then methodindex=10
  if methodindex eq 6 and ~keyword_set(triangles) then Message,"Keyword NaturalNeighbor requires the presence of keyword Triangles."
  if methodindex eq 7 and ~keyword_set(triangles) then Message,"Keyword NearestNeighbor requires the presence of keyword Triangles."
  if methodindex eq 3 and keyword_set(sphere) then Message,"Keywords (Linear, Quintic, MinimumCurvature) and SPHERE are mutually exclusive"
  if methodindex eq 4 and keyword_set(sphere) then Message,"Keywords (Linear, Quintic, MinimumCurvature) and SPHERE are mutually exclusive"
  if methodindex eq 9 and keyword_set(sphere) then Message,"Keywords (Linear, Quintic, MinimumCurvature) and SPHERE are mutually exclusive"

  if keyword_set(EMPTY_SECTORS) and ~keyword_set(triangles) then Message,"Keyword EMPTY_SECTORS requires the presence of keyword Triangles."
  if keyword_set(MAX_PER_SECTOR) and ~keyword_set(triangles) then Message,"Keyword MAX_PER_SECTOR requires the presence of keyword Triangles."
  if keyword_set(MIN_POINTS) and ~keyword_set(triangles) then Message,"Keyword MIN_POINTS requires the presence of keyword Triangles."
  if keyword_set(SEARCH_ELLIPSE) and ~keyword_set(triangles) then Message,"Keyword SEARCH_ELLIPSE requires the presence of keyword Triangles."

  if keyword_set(sphere) then begin
     ; TRIANGLES (if present) are just ignored
        triangulate,x,y,tr,sphere=s,fvalue=f,degrees=degrees
        return,trigrid(f,gs,limits,sphere=s      ,degrees=degrees,missing=missing,nx=nx,ny=ny,xout=xout,yout=yout)
  endif else begin
     if ~ keyword_set(triangles) then triangulate,x,y,triangles
     return, trigrid(x,y,f,triangles,gs,limits   ,degrees=degrees,missing=missing,nx=nx,ny=ny,xout=xout,yout=yout)
  endelse
end
