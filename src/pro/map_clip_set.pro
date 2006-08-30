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
pro map_clip_set,reset=reset,split=split,clip_plane=clip_plane, $
                 transform=transform,clip_uv=clip_uv


if (keyword_set(reset)) then begin
    !map.pipeline=0.0
    return
endif

if (n_elements(clip_plane) ne 0) then begin
    i = 0
    while (!map.pipeline[0,i] ne 0 and !map.pipeline[0,i] ne 3) do i = i + 1
    if (i eq 11) then begin
        print,'% Internal error: Too many mapping clip stages..'
        stop
    endif else begin
        !map.pipeline[0,i+1] = 3
        !map.pipeline[0,i] = 2
        f = sqrt(clip_plane[0]^2 + clip_plane[1]^2 + clip_plane[2]^2)
        if (f eq 0) then f = 1
        !map.pipeline[1,i] = clip_plane[0] / f
        !map.pipeline[2,i] = clip_plane[1] / f
        !map.pipeline[3,i] = clip_plane[2] / f
        !map.pipeline[4,i] = clip_plane[3] / f
        return
    endelse
endif


if (n_elements(clip_uv) ne 0) then begin
    i = 0
    while (!map.pipeline[0,i] ne 3 and i lt 11) do i = i + 1
    if (i eq 11) then begin
        !map.pipeline[0,0] = 3
        !map.pipeline[0,1] = 4
        !map.pipeline[1,1] = clip_uv[0]
        !map.pipeline[2,1] = clip_uv[1]
        !map.pipeline[3,1] = clip_uv[2]
        !map.pipeline[4,1] = clip_uv[3]
    endif else begin
        !map.pipeline[0,i+1] = 4
        !map.pipeline[1,i+1] = clip_uv[0]
        !map.pipeline[2,i+1] = clip_uv[1]
        !map.pipeline[3,i+1] = clip_uv[2]
        !map.pipeline[4,i+1] = clip_uv[3]
    endelse
endif

return
end

