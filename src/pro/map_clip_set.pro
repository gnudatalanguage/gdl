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
pro map_clip_set,map_structure=map_structure, reset=reset,split=split,clip_plane=clip_plane, $
                 transform=transform,clip_uv=clip_uv, show=show
on_error, 2
giveback=0
if (n_elements(map_structure) eq 0) then begin
 map_structure=!map
 giveback=1
endif

if (keyword_set(reset)) then begin
    map_structure.pipeline=0.0
endif
if (keyword_set(transform)) then begin
    i = 0
    while (map_structure.pipeline[0,i] ne 0 and map_structure.pipeline[0,i] ne 3) do i = i + 1
    if (i eq 11) then begin
        message, 'Too many pipeline stages..'
    endif else begin
        map_structure.pipeline[0,i] = 3
    endelse
endif

if (n_elements(split) ne 0) then begin
    i = 0
    while (map_structure.pipeline[0,i] ne 0 and map_structure.pipeline[0,i] ne 3) do i = i + 1
    if (i eq 11) then begin
        message, 'Too many pipeline stages..'
    endif else begin
;        print,"split=",split
        map_structure.pipeline[0,i+1] = 3
        map_structure.pipeline[0,i] = 1
        map_structure.pipeline[1,i] = -1*split[2]
        map_structure.pipeline[2,i] = -1*split[3]
        map_structure.pipeline[3,i] = -1*split[4]
        map_structure.pipeline[4,i] = -1*split[5]
        lon=split[0]*!DTOR &  lat=split[1]*!DTOR
        x = cos(lon) * cos(lat) &  y = sin(lon) * cos(lat) &z = sin(lat)
        map_structure.pipeline[5,i] = x
        map_structure.pipeline[6,i] = y
        map_structure.pipeline[7,i] = z
    endelse
endif

if (n_elements(clip_plane) ne 0) then begin
    i = 0
    while (map_structure.pipeline[0,i] ne 0 and map_structure.pipeline[0,i] ne 3) do i = i + 1
    if (i eq 11) then begin
        message, 'Too many pipeline stages..'
    endif else begin
;        print,"clip_plane=",clip_plane
        map_structure.pipeline[0,i+1] = 3
        map_structure.pipeline[0,i] = 2
        f = sqrt(clip_plane[0]^2 + clip_plane[1]^2 + clip_plane[2]^2)
        if (f eq 0) then f = 1
        map_structure.pipeline[1,i] = clip_plane[0] / f
        map_structure.pipeline[2,i] = clip_plane[1] / f
        map_structure.pipeline[3,i] = clip_plane[2] / f
        map_structure.pipeline[4,i] = clip_plane[3] / f
    endelse
endif

if (n_elements(clip_uv) ne 0) then begin
    i = 0
    while (map_structure.pipeline[0,i] ne 3 and i lt 11) do i = i + 1
    if (i eq 11) then begin
        map_structure.pipeline[0,0] = 3
        map_structure.pipeline[0,1] = 4
        map_structure.pipeline[1,1] = clip_uv[0]
        map_structure.pipeline[2,1] = clip_uv[1]
        map_structure.pipeline[3,1] = clip_uv[2]
        map_structure.pipeline[4,1] = clip_uv[3]
    endif else begin
        map_structure.pipeline[0,i+1] = 4
        map_structure.pipeline[1,i+1] = clip_uv[0]
        map_structure.pipeline[2,i+1] = clip_uv[1]
        map_structure.pipeline[3,i+1] = clip_uv[2]
        map_structure.pipeline[4,i+1] = clip_uv[3]
    endelse
endif

if (keyword_set(show)) then begin
  i=0
  while(map_structure.pipeline[0,i] ne 0) do begin
    case fix(map_structure.pipeline[0,i]) of
      1: print,"PIPELINE_SPLIT",map_structure.pipeline[1,i],map_structure.pipeline[2,i],map_structure.pipeline[3,i],map_structure.pipeline[4,i]
      2: print,"PIPELINE_PLANE",map_structure.pipeline[1,i],map_structure.pipeline[2,i],map_structure.pipeline[3,i],map_structure.pipeline[4,i]
      3: print,"PIPELINE_TRANS",map_structure.pipeline[1,i],map_structure.pipeline[2,i],map_structure.pipeline[3,i],map_structure.pipeline[4,i]
      4: print,"PIPELINE_UVBOX",map_structure.pipeline[1,i],map_structure.pipeline[2,i],map_structure.pipeline[3,i],map_structure.pipeline[4,i]
    endcase
    i++
  endwhile
endif

if (giveback eq 1) then !map=map_structure

return
end

