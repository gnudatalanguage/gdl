;+
; NAME:
;       MAP_CLIP_SET
;
;
; PURPOSE:
;       Sets up the clipping/splitting pipeline for the projection
;
;
; CATEGORY:
;       Map utilities
;
;
; CALLING SEQUENCE:
;
;
; INPUTS:
;
;
; OUTPUTS:
;
;
; MODIFICATION HISTORY:
;   25-Jul-2006 : written by JMG
;
; LICENCE:
; Copyright (C) 2006, Joel M. Gales
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
pro map_clip_set,map_structure=mapstr, reset=reset,split=split,clip_plane=clip_plane, $
                 transform=transform,clip_uv=clip_uv, show=show
on_error, 2

systemMap=0
if (n_elements(map_structure) eq 0) then begin
 mapstr={!map}
endif 

if (keyword_set(reset)) then begin
    mapstr.pipeline=0.0
endif

if (n_elements(split) ne 0) then begin
    i = 0
    while (mapstr.pipeline[0,i] ne 0 and mapstr.pipeline[0,i] ne 3) do i = i + 1
    if (i eq 11) then begin
        message, 'Too many mapping clip stages..'
    endif else begin
        mapstr.pipeline[0,i+1] = 3
        mapstr.pipeline[0,i] = 1
        mapstr.pipeline[1,i] = -1*split[2]
        mapstr.pipeline[2,i] = -1*split[3]
        mapstr.pipeline[3,i] = -1*split[4]
        lon=split[0]*!DTOR &  lat=split[1]*!DTOR
        x = cos(lon) * cos(lat) &  y = sin(lon) * cos(lat) &z = sin(lat)
        mapstr.pipeline[4,i] = 0
        mapstr.pipeline[5,i] = x
        mapstr.pipeline[6,i] = y
        mapstr.pipeline[7,i] = z
    endelse
endif

if (n_elements(clip_plane) ne 0) then begin
    i = 0
    while (mapstr.pipeline[0,i] ne 0 and mapstr.pipeline[0,i] ne 3) do i = i + 1
    if (i eq 11) then begin
        message, 'Too many mapping clip stages..'
    endif else begin
        mapstr.pipeline[0,i+1] = 3
        mapstr.pipeline[0,i] = 2
        f = sqrt(clip_plane[0]^2 + clip_plane[1]^2 + clip_plane[2]^2)
        if (f eq 0) then f = 1
        mapstr.pipeline[1,i] = clip_plane[0] / f
        mapstr.pipeline[2,i] = clip_plane[1] / f
        mapstr.pipeline[3,i] = clip_plane[2] / f
        mapstr.pipeline[4,i] = clip_plane[3] / f
    endelse
endif


if (n_elements(clip_uv) ne 0) then begin
    i = 0
    while (mapstr.pipeline[0,i] ne 3 and i lt 11) do i = i + 1
    if (i eq 11) then begin
        mapstr.pipeline[0,0] = 3
        mapstr.pipeline[0,1] = 4
        mapstr.pipeline[1,1] = clip_uv[0]
        mapstr.pipeline[2,1] = clip_uv[1]
        mapstr.pipeline[3,1] = clip_uv[2]
        mapstr.pipeline[4,1] = clip_uv[3]
    endif else begin
        mapstr.pipeline[0,i+1] = 4
        mapstr.pipeline[1,i+1] = clip_uv[0]
        mapstr.pipeline[2,i+1] = clip_uv[1]
        mapstr.pipeline[3,i+1] = clip_uv[2]
        mapstr.pipeline[4,i+1] = clip_uv[3]
    endelse
endif

if (keyword_set(show)) then begin
  i=0
  while(mapstr.pipeline[0,i] ne 0) do begin
    case fix(mapstr.pipeline[0,i]) of
      1: print,"PIPELINE_SPLIT",mapstr.pipeline[1,i],mapstr.pipeline[2,i],mapstr.pipeline[3,i],mapstr.pipeline[4,i]
      2: print,"PIPELINE_PLANE",mapstr.pipeline[1,i],mapstr.pipeline[2,i],mapstr.pipeline[3,i],mapstr.pipeline[4,i]
      3: print,"PIPELINE_TRANS",mapstr.pipeline[1,i],mapstr.pipeline[2,i],mapstr.pipeline[3,i],mapstr.pipeline[4,i]
      4: print,"PIPELINE_UVBOX",mapstr.pipeline[1,i],mapstr.pipeline[2,i],mapstr.pipeline[3,i],mapstr.pipeline[4,i]
    endcase
    i++
  endwhile
endif

if (n_elements(map_structure) eq 0) then begin
 !map=mapstr
endif 

return
end

