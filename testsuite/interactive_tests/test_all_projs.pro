pro test_all_projs, n
  if n_elements(n) eq 0 then n=53 ; start of the 'real' projections,
                                ; before it is only an alias 
on_error,2
map_proj_info,proj_names=pjn
for i=n,n_elements(pjn) do begin
   catch,absent
   if absent ne 0 then begin
      print,"Error on projection: "+pjn[i]+"."
      continue
   endif
print,i,pjn[i]
map_set,/ISO,height=1,standard_parall=30,standard_par1=50,standard_par2=-45,sat_tilt=45,center_lon=0,true_scale_latitude=12,lat_3=13,HOM_LONGITUDE1=1,HOM_LONGITUDE2=80,LON_3=120,OEA_SHAPEN=1, OEA_SHAPEM=1,SOM_LANDSAT_NUMBER=2, SOM_LANDSAT_PATH=22, ZONE=28, center_lat=0,sphere=1,name=pjn[i],e_cont={cont:1,fill:1,color:'33e469'x,hires:0},/hor,e_hor={nvert:200,fill:1,color:'F06A10'x},e_grid={box_axes:1,color:'1260E2'x,glinethick:1,glinestyle:0,latdel:10,londel:15}
wait,1
endfor
end
