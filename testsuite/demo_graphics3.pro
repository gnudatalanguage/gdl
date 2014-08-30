function Himmelblau, x1,x2
; Himmelblau f=(x1^2+x2-11)^2+(x1+x2^2-7)^2
  u = x1*x1+x2-11
  v = x1+x2*x2-7
  f = u*u+v*v
RETURN,f 
end
function Rosenbrock, a1, a2
; Rosenbrock: f  = (1 - x1)^2+ 100*(x2 - x1^2)^2 
  x1=double(a1)
  x2=double(a2)
  u1 = 1 - x1
  v1 = x2 - x1*x1
  f = u1*u1 + 100.0*v1*v1
RETURN, f
end

pro sub_demo_graphics3
on_error, 2
!P.SUBTITLE=''
!P.TITLE='3D Demo' 
!X.TITLE="X"
!Y.TITLE="Y"
!Z.Title="Z"
!P.MULTI=[0,2,2]
;
m = 32L
n = m
range = 4L
; 1D x y
generatorx=(([findgen(m)]/float(m))*2.0-1.0)*range
generatory=(([findgen(n)]/float(n))*2.0-1.0)*range
z=fltarr(m*n,/nozero)
for i=0L,(m*n)-1 do begin &$
 j=i mod m & k=i/m & z[i]=Himmelblau(generatorx[j],generatory[k]) &$
end
z=reform(z,m,n,/over)
scale3
contour,z,/xs,/ys,nlev=20,/T3D,/FILL,ZV=0,CLIP=0
contour,z,/xs,/ys,nlev=20,/T3D,/OVER,ZV=0,CLIP=0
T3D,/XZ
contour,z,/xs,/ys,nlev=20,/T3D,/FILL,ZV=1,/OVER,CLIP=0
T3D,/YZ
contour,z,/xs,/ys,nlev=20,/T3D,/FILL,ZV=1,/OVER,CLIP=0
T3D,/YZ&T3D,/XZ

surface,z,/xs,/ys,/T3D,SHADES=[findgen(m*n)]*10,CLIP=0

contour,z,/xs,/ys,nlev=20,/T3D,/FILL,CLIP=0

set_shading,light=[500,0,1]
shade_surf,z,/xs,/ys,/T3D,CLIP=0
!P.MULTI=0
end

PRO demo_graphics3, print=print,mode=mode,help=help
if keyword_set(help) then begin
 print,"Usage: demo_graphics1,[mode=[0,1,2]],[/print]"
 print,"mode=0 produces decomposed=0 output"
 print,"mode=1 produces decomposed=1 output"
 print,"mode=2 or KW mode absent: produces both outputs"
 print,"/print produces additionally various Postscript and SVG outputs."
 return
endif
if ~size(mode,/type)  then mode=2

set_plot,'x'
for decomposed=0,1 do begin
   if decomposed eq mode or mode eq 2 then begin
      device,decomposed=decomposed
      sub_demo_graphics3
      xyouts,0,0,decomposed,/norm
      if mode eq 2 then wait,1
   endif
endfor
if (not keyword_set(print)) then return
prompt=strmid(!PROMPT,0,1)
tvlct,r,g,b,/get ; save colors
set_plot,'ps' ; change device
for decomposed=0,1 do begin
   if decomposed eq mode or mode eq 2 then begin
      device,decomposed=decomposed
      sdec=string(decomposed,format='(I1.1)')
      filenamebwport='demo_graphics3_bw_port_'+prompt+'-'+sdec+'.ps'
      filenamebwland='demo_graphics3_bw_land_'+prompt+'-'+sdec+'.ps'
      filenamecolor='demo_graphics3_color_'+prompt+'-'+sdec+'.ps'
      tvlct,r,g,b               ; restore colortable
      device,filename=filenamebwport,/port,color=0
      sub_demo_graphics3
      print,'printing test file as black&white postscript portrait '+filenamebwport
      device,/close
      device,filename=filenamebwland,/land,color=0
      sub_demo_graphics3
      print,'printing test file as black&white postscript landscape '+filenamebwland
      device,/close
      device,filename=filenamecolor,/port,color=1
      sub_demo_graphics3
      print,'printing test file as color postscript portrait '+filenamecolor
      device,/close
   endif
endfor
set_plot,'svg' ; change device
for decomposed=0,1 do begin
   if decomposed eq mode or mode eq 2 then begin
      device,decomposed=decomposed
      sdec=string(decomposed,format='(I1.1)')
      filename='demo_graphics3_'+prompt+'-'+sdec+'.svg'
      tvlct,r,g,b               ; restore colortable
      device,filename=filename
      sub_demo_graphics3
      print,'printing test file as svg: '+filename
      device,/close
   endif
endfor
!P.MULTI=0
set_plot,'x'

end
