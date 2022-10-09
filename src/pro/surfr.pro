; part of GNU Data Language - GDL 
;-------------------------------------------------------------
;+
; NAME:
;       SURFR
; PURPOSE:
;       Sets up the same 3D matrix !p.t than SURFACE, but without
;       calling SURFACE
; CALLING SEQUENCE:
;       SURFR, [[ax=ax(rotation in degrees around x axis)], az=az(same
;              for z axis)]
;       default for ax and az is 30 degrees.
; INPUTS: NONE
;
; MODIFICATION HISTORY:
; 	Written by: 2022 07/05 G. Duvert (Initial import)
;
;-
; LICENCE:
; Copyright (C) 2022
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
  
pro surfr,ax=ax, az=az
; has a default
  if (~n_elements(ax)) then ax=30
  if (~n_elements(az)) then az=30
  t3d,/reset
  t3d, translate=[-0.5,-0.5,-0.5]
  t3d, rotate=[-90,az,0]
  t3d, rotate=[ax,0,0]
  scale3d
end
