;------------------------------------------------------------------------
; documentation in progress.. HelpMe!
;
pro map_proj_info, rindex, current=current, name=name, scale=meters, cylindrical=cylindrical, $
proj_names = list_of_pnames, circle=circle, conic=conic, azimuthal=azimuthal, uv_limits = uv_limits, $
ll_limits = ll_limits, uvrange=u, uv_range=uu, map_structure=mapstruct, $
p4name=p4n ; special for GDL
  
  COMPILE_OPT hidden
; the common contains all relevant values after initialisation
   @gdlcommon_mapprojections_common
   
   nproj=n_elements(proj)

   ; proj_names: easy
   if arg_present(list_of_pnames) then list_of_pnames = [idl_ids,proj.fullname]
; rindex is [1...nproj] but map.p[15] is [0..
   if KEYWORD_SET(current) then begin
      index = fix(!map.p[15]) ; gdl variant
      rindex=index+1
   endif else if arg_present(mapstruct) then begin
      index = fix(mapstruct.p[15]) ; gdl variant
      rindex=index+1
   endif

   if ARG_PRESENT(uv_limits) then uv_limits = !map.uv_box
   if ARG_PRESENT(ll_limits) then ll_limits = !map.ll_box
   if (n_elements(rindex) eq 0) then return

;--------------------

   if rindex lt 1 or rindex gt nproj then message, 'Projection number must be within range of 1 to'+ strtrim(nproj-1,2)

   name = proj[index].fullname
   p4n=proj[index].proj4name
   ; need to keep ony the real proj4 name if perchance there was additional commands already set in the name
   p4n=(strsplit(strtrim(p4n,2),' ',/extract))[0] 
   property=proj_properties[index]
   conic=(property.CONIC eq 1 and property.ELL eq 0 and property.SPH eq 0) ; only true conics (ex: not Bonne)
   spheric=(property.SPH eq 1)
   cylindrical=(property.CYL eq 1)
   azimuthal=(property.AZI eq 1)
   circle=azimuthal ; spheric and ~conic and ~cylindrical
   r_earth = 6378206.4d0        ; Clarke 1866 ellipsoid is used.
   meters=proj_scale[index] ; already in meters: size in meters of the uv range.
   uu=proj_limits[*,index]
   if arg_present(u) then u = uu
   meters = (meters le 1)?1.0:meters/(uu[2]-uu[0]); meters per uv unit.
;   print,"name=""",name,""", index: ",index,", meters: ",meters,", limits:",proj_limits[*,index]

end


