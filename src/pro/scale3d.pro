; NAME:
;       SCALE3D
; PURPOSE:
;       Modifies !x !y !z and !p.t such as the unit cube (seen by !p.t) fits in the plot area.
;       Will not work for all perpsective, stretch etc transformations eventually present in !p.t
; CALLING SEQUENCE:
;       SCALE3D
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
PRO SCALE3D
ON_ERROR,2
; create all 8 homogenous coordinates of the cubes points, 
; compute their projected coord,
; find min and max on both axes.
x=[0,1,0,1,0,1,0,1]
y=[0,0,1,1,0,0,1,1]
z=[0,0,0,0,1,1,1,1]
t=[1,1,1,1,1,1,1,1]
v=[[x],[y],[z],[t]]
w=v#!p.t
z=w[*,3]
; normalize
w[*,0]/=z
w[*,1]/=z
w[*,2]/=z
min=min(w,dim=1)
max=max(w,dim=1)
;	avoid dividing by 0 if there is no Z transformation.
w=where(max eq min, count)
if (count) then max[w]=min[w]+1 ;probably always max[3]=min[3] but this is not used afterwards.
if max[2] eq min[2] then max[2]=min[2]+1
translate=[ -min[0], -min[1], -min[2]]
scale=1./(max[0:2]-min[0:2])
t3d,tr = translate, sc=scale
end
