; test all projections

pro test_all_projs, from=from, to=to, lon=lon, lat=lat, rot=rot, image=image, halt=halt
  on_error,2
  map_proj_info,proj_names=pjn
  if keyword_set(image) then begin
    image = READ_TIFF("~/gdl/resource/maps/high/NE_SUBSET.tiff")
  endif
  ttt=''
  if n_elements(from) eq 0 then from=1
  if n_elements(to) eq 0 then to=n_elements(pjn) else to=to<(n_elements(pjn)+1)
  if n_elements(lon) eq 0 then lon=0
  if n_elements(lat) eq 0 then lat=0
  if n_elements(rot) eq 0 then rot=0
  
  for iproj=from,to do begin
     catch,absent
     if absent ne 0 then begin
        catch,/cancel
        continue
     endif
     map_set,/relaxed,/advance,lat,lon,rot,name=pjn[iproj],lat_1=12,lat_2=56,lat_ts=33,height=3,e_cont={cont:1,fill:1,color:'33e469'x,hires:0},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x},e_grid={box_axes:0,color:'1260E2'x,glinethick:1,glinestyle:0},title=pjn[iproj],/iso,center_azimuth=33,sat_tilt=33,OEA_SHAPEM=1,OEA_SHAPEN=1,SOM_LANDSAT_NUMBER=2, SOM_LANDSAT_PATH=22, ZONE=28
     print,iproj,pjn[iproj]
     if keyword_set(image) then begin
     for iimage=0,2 do begin & z=map_image(reform(image[iimage,*,*]),Startx,Starty,lonmin=-10.5149147727,latmin=59.3309659091,lonmax=10.6044034091,latmax=41.2542613636) & tv,z,startx,starty,chan=iimage+1 & endfor
     endif
     if keyword_set(halt) then read,ttt,prompt='Waiting for keypad input...' else wait,1
  endfor
end
