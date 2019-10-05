; ancillary procedure for MAP_* .
; insures lon is always btw -180 and 180
; based on proj4 source file of same name.
pro map_adjlon,lon
  TWOPI=360d
  ONEPI=180d
  SPI=180.0
  if n_elements(lon) eq 1 then begin
    if abs(lon) gt SPI then begin
       lon += ONEPI
       lon -= (TWOPI * floor(lon / TWOPI))
       lon -= ONEPI
    endif
  endif else begin
    w=where(abs(lon) gt SPI, count) 
    if (count gt 0) then begin
       sublon=lon[w]
       sublon += ONEPI
       sublon -= (TWOPI * floor(sublon / TWOPI))
       sublon -= ONEPI
       lon[w]=sublon
    endif
  endelse
end
