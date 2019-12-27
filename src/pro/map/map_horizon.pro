; plots the limits of a map projection as a nice "limb" around the
; map.
; used within map_set normally. That's why a map structure is
; not passed to this procedure.
PRO map_xy_rotforward,x,y
  if !map.rotation eq 0 then return
  cRot = !map.cosr
  sRot = !map.sinr
  u = x & v = y
  x = u*cRot + v*sRot;
  y = -u*sRot + v*cRot;
END
;; PRO map_xy_rotinverse,x,y
;;   if !map.rotation eq 0 then return
;;   cRot = !map.cosr
;;   sRot = !map.sinr
;;   u = x & v = y
;;   x = u*cRot - v*sRot;
;;   y = u*sRot + v*cRot;
;; END

PRO map_horizon, fill=fill, nverts=n, zvalue=zvalue, _extra=Extra
    COMPILE_OPT hidden

    Map_proj_info, /CURRENT, P4NAME=p4name, uv_range=uvr, CIRCLE=circle, conic=conic, azim=azim

    r = !map.rotation       ;Our rotation

    if n_elements(n) le 0 then n = 60 ;# of vertices
    a = findgen(n+1) * (2 * !pi / n) ;N angles from 0 to 2 pi
 
    if p4name eq "igh" then begin
       z=1e-6
       inc=(90-z)/(n-1)
       yt=[0d:90:inc]
       ryt=reverse(yt) 
       yb=-yt & ryb=reverse(yb)
       x=yt*0.0d
       xx=[x-180+z, x-40-z, x-40+z, x+180-z, x+180-z, x+80+z , x+80-z, x-20+z, x-20-z, x-100+z, x-100-z,x-180+z]
       yy=[yt, ryt, yt, ryt, yb, ryb, yb, ryb, yb, ryb, yb, ryb]
       ; add p0lon, p0lat
       xx+=!map.p0lon
       xy=map_proj_forward(xx,yy)
       xr=xy[0,*]
       yr=xy[1,*]

;; ;   -180            -40                       180
;; ;      +--------------+-------------------------+    Zones 1,2,9,10,11 & 12:
;; ;      |1             |2                        |      Mollweide projection
;; ;      |              |                         |
;; ;      +--------------+-------------------------+    Zones 3,4,5,6,7 & 8:
;; ;      |3             |4                        |      Sinusoidal projection
;; ;      |              |                         |
;; ;    0 +-------+------+-+-----------+-----------+
;; ;      |5      |6       |7          |8          |
;; ;      |       |        |           |           |
;; ;      +-------+--------+-----------+-----------+
;; ;      |9      |10      |11         |12         |
;; ;      |       |        |           |           |
;; ;      +-------+--------+-----------+-----------+
;; ;    -180    -100      -20         80          180
;;
    endif else if p4name eq "healpix" then begin
       z=1e-4
       magic=asin(2./3.)/!dtor
       inc=(magic-z)/(n/2-1)
       yt=[magic:90-z:inc]
       ryt=reverse(yt) 
       yb=-yt & ryb=reverse(yb)
       y0=[0d:magic:inc]
       ry0=reverse(y0)
       x0=y0*0.0d
       x=yt*0.0d 
       xx=[x0-180+z,x-180+z,x-90-z, x-90+z, x-z,x+z, x+90-z, x+90+z, x+180-z, x0+180-z]
       xx=[xx,reverse(xx)]
       yy=[y0,yt, ryt, yt,ryt,yt,ryt,yt,ryt,ry0]
       yy=[yy,-1*reverse(yy)]
       ; add p0lon
       xx+=!map.p0lon
       xy=map_proj_forward(xx,yy) ; includes rotation.
       xr=xy[0,*]
       yr=xy[1,*]
    endif else if p4name eq "bipc" then begin
       z=1e-2
       inc=(180-2*z)/(n/2-1)
       yt=[-90+z:90-z:inc]
       ryt=reverse(yt) 
       x=yt*0.0d 
       rstx=[-180+z:-110-z:1] & rsty=(rstx*0);-10.0
       r2stx=[-20+z:180-z:1] & r2sty=(r2stx*0);-10.0
       xx=[rstx,x-110-z,x-110+z,x-20-z, x-20+z,r2stx]
       yy=[rsty,yt,ryt, yt, ryt,r2sty]
       ; add p0lon
       xx+=!map.p0lon
       xy=map_proj_forward(xx,yy) ; includes rotation.
       xr=xy[0,*]
       yr=xy[1,*]
    endif else if p4name eq "qsc" then begin ; wrong formula. 
       z=1d-4
       inc=(360-z)/(n-1)
       minlat=-40 & maxlat=+40
       minlon=-60 & maxlon=+60
       xx1=[minlon+z:maxlon-z:inc] & yy1=xx1*0+maxlat-z & xx=[xx1,reverse(xx1)] & yy=[yy1,xx1*0+minlat+z]
       ; add p0lon
       xx+=!map.p0lon
       xy=map_proj_forward(xx,yy) ; includes rotation.
       xr=xy[0,*]
       yr=xy[1,*]
    endif else if p4name eq "rhealpix" then begin
       z=1e-4
       x1=-!DPI & x2=x1/2. & x3=!DPI & y0=-3*!DPI/4 & y1=-!DPI/4 & y2=-y1 & y3=-y0
       xr=[x1,x1,x2,x2,x3,x3,x2,x2,x1]
       yr=[y0,y3,y3,y2,y2,y1,y1,y0,y0]
       map_xy_rotforward,xr,yr ; do rotation ourselve
     endif else if p4name eq "nsper" or p4name eq "geos" or p4name eq "tpers" then begin
       xr=(!map.uv_box[2]-!map.uv_box[0])/2. * cos(a)
       yr=(!map.uv_box[3]-!map.uv_box[1])/2. * sin(a)
    endif else if circle then begin ; limb is a circle
       xr =  (uvr[2]-uvr[0])/2. * cos(a)
       yr =  (uvr[3]-uvr[1])/2. * sin(a)
    endif else if conic then begin ; everythin between -180 180 and the 2 parallels of !map.p
       z=1d-4
       inc=(360-z)/(n-1)
       minlat=min(!map.p[[13,14]],max=maxlat)
       xx1=[-180+z:180-z:inc] & yy1=xx1*0+maxlat-z & xx=[xx1,reverse(xx1)] & yy=[yy1,xx1*0+minlat+z]
       ; add p0lon
       xx+=!map.p0lon
       xy=map_proj_forward(xx,yy) ; includes rotation.
       xr=xy[0,*]
       yr=xy[1,*]

    endif else begin                ;almost anything but uses a terrible hack.
       z=1d-6
       inc=(178-2*z)/(n-1)
       lat=!map.p0lat
       if lat ne 0 then begin; piecewise split construction
          sgn=(lat ge 0)?1:-1
          lat=abs(lat)
          ys=[90-lat-z:89-lat:-0.01] & yn=[-91+lat:-90+lat-z:0.01]
          yc1=[89-lat-z:-90:-inc] & yc2=[-90+z:-90+lat-z:inc]
          zs=ys*0+1d-2 & zn=yn*0+1d-2 
          zc1=yc1*0+z & zc2=yc2*0+z
          y=[ys,yc1,yc2,yn]
          zz1=[zs,zc1,-zc2,-zn]
          zz2=reverse([-zs,-zc1,+zc2,+zn])
          xx=[ys*0-180,yc1*0.0-180,yc2*0.0,yn*0]
          xx=[xx,reverse(xx)]
          xx+=[zz1,zz2]         ; add avoidance
          yy=[y,reverse(y)]
          if (sgn lt 0) then yy*=-1 ; for negative latitudes...
       endif else begin ; simpler
          ys=[-90+z:-89:0.01] & yn=[89:90-z:0.01]
          zs=ys*0+1d-2 & zn=zs  ; split avoidance best be large in these polar projection regions! 
          yc=[-88.99:88.99:inc] & zc=yc*0+z
          y=[ys,yc,yn]
          zz=[zs,zc,zn]
; opposite split to projection center: x=-180+z,y then back x=180-z,reverse(y)
          xx=[y*0+180-zz,y*0.0-180.+zz]
          yy=[y,reverse(y)]
       endelse

       xx+=!map.p0lon
       xy=map_proj_forward(xx,yy)
       xr=xy[0,*]
       yr=xy[1,*]
     endelse

; do nothing if "almost" nothing has been done.
    if (n_elements(xr) lt 3) then return

    xtsave = !x.type
    if n_elements(zvalue) eq 0 THEN zvalue = 0

    !x.type=0           ;Plot in UV space
    if keyword_set(fill) then polyfill, xr, yr, _EXTRA=Extra, NOCLIP=0 $
    else plots, xr, yr, zvalue, _EXTRA=Extra, NOCLIP=0
    !x.type=xtsave


end

