pro TEST_MAP2,lon,lat, projname=projname, help=help, _extra=_extra
if keyword_set(help) then begin
MAP_PROJ_INFO,PROJ_NAMES=names
Print, "Useage: test_map2, lon,lat, projname= one of:"
Print,names
return
endif
if (n_elements(lon) eq 0) then lon=0
if (n_elements(lat) eq 0) then lat=0
if ~keyword_set(projname) then projname="Sinusoidal"
image = BYTSCL(SIN(DIST(400)/10))
MAP_SET, lat, lon, NAME=PROJNAME, /ISOTROPIC,TITLE=projname+' + REPROJECTED IMAGE',  _extra=_extra
result = MAP_IMAGE(image,Startx,Starty)
TV, result, Startx, Starty
MAP_CONTINENTS,/coasts
MAP_GRID, latdel=10, londel=10, /LABEL, /HORIZON, CHARS=2
plots,[30,30,120,120,30],[0,-40,-40,0,0],color='FFFF00'x,thick=8
polyfill,[32,32,118,118,32],[-2,-38,-38,-2,-2],color='FFFF00'x,thick=8
xyouts,0.7,0.1,"plots+polyfill",/norm
xyouts,0,45,"greenwich",ori=90,chars=3,color='FFFF00'x
oplot,[-120,-120,-30,-30,-120],[0,-40,-40,0,0],color='00FFFF'x,thick=3
xyouts,0.3,0.1,"oplot",/norm
axis,0,20,/xaxis
axis,-90,0,/yaxis
end
