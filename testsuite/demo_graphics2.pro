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

pro sub_demo_graphics2
on_error, 2
!P.SUBTITLE=''
!P.TITLE='Contour Demo' 
!X.TITLE="X"
!Y.TITLE="Y"
;
device,get_decomposed=decomp
if (decomp) then AdaptiveColorList=['00FFFF'x,'FFFF00'x,'FF00FF'x] else AdaptiveColorList=[64,92,110,156,208,233,250]
over=1
cont=1
fill=1
m = 256L
n = m
range = 5L
; 1D x y
generatorx=(([findgen(m)]/float(m))*2.0-1.0)*range
generatory=(([findgen(n)]/float(n))*2.0-1.0)*range
a=fltarr(m*n,/nozero)
for i=0L,(m*n)-1 do begin &$
 j=i mod m & k=i/m & a[i]=Himmelblau(generatorx[j],generatory[k]) &$
end
a=reform(a,m,n,/over)
a=max(a)-a
;!nan a corner
a[m-10:m-1,n-10:n-1]=!VALUES.F_NAN
; zero generator
!P.MULTI=[0,2,2]
!P.TITLE="No X,Y"
if (fill) then contour,a,nlev=24,/FILL,/ISO,/XSTYLE,/YSTYLE, _EXTRA=ex
if (cont) then contour,a,nlev=24,over=over,/ISO,/FOLLOW,/XSTYLE,/YSTYLE, _EXTRA=ex
!P.TITLE="With 1DX, 1DY"
x=exp((generatorx)/(2*range)) &x=x+MIN(x)+1
y=(range+generatory)^2 & y=y+MIN(Y)+1
if (fill) then contour,a,x,y,nlev=24,/XSTYLE,/YSTYLE,/FILL,_EXTRA=ex
if (cont) then contour,a,x,y,nlev=24,/XSTYLE,/YSTYLE,over=over,/FOLLOW, _EXTRA=ex
!P.TITLE="With 2DX, 2DY (45 degrees rotation)"
generatorx=([findgen(m*n) mod m]/float(m))*2*!PI
generatorx=reform(generatorx,m,n,/over)
generatory=(floor([findgen(m*n)]/float(m))/float(n))*2*!PI
generatory=reform(generatory,m,n,/over)
angle=45*!PI/180.0
x=generatorx*cos(angle)-generatory*sin(angle) & x=x+10
y=generatorx*sin(angle)+generatory*cos(angle) & y=y+10
if (fill) then contour,a,x,y,nlev=24,/FILL,/ISO,/XSTYLE,/YSTYLE, _EXTRA=ex
if (cont) then contour,a,x,y,nlev=24,OVER=over,/ISO,/XSTYLE,/YSTYLE, _EXTRA=ex
!P.TITLE="With patterns"
if (fill) then contour,a,nlev=24,C_COLORS=AdaptiveColorlist,C_ORIENTATION=([indgen(24)]*120/24),C_SPACING=[0.1,0.2,0.3],C_LINESTYLE=[0,1,2,3],C_THICK=[0,3,5],/FILL,/ISO,/XSTYLE,/YSTYLE, _EXTRA=ex
if (cont) then contour,a,x,y,nlev=6,OVER=over,/follow,/ISO, _EXTRA=ex
!P.MULTI=0
end

PRO demo_graphics2, print=print,mode=mode,help=help
if keyword_set(help) then begin
 print,"Usage: demo_graphics1,[mode=[0,1,2]],[/print]"
 print,"mode=0 produces decomposed=0 output"
 print,"mode=1 produces decomposed=1 output"
 print,"mode=2 or KW mode absent: produces both outputs"
 print,"/print produces additionally various Postscript and SVG outputs."
 return
endif
if ~size(mode,/type)  then mode=2

for decomposed=0,1 do begin
   if decomposed eq mode or mode eq 2 then begin
      device,decomposed=decomposed
      sub_demo_graphics2
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
      filenamebwport='demo_graphics2_bw_port_'+prompt+'-'+sdec+'.ps'
      filenamebwland='demo_graphics2_bw_land_'+prompt+'-'+sdec+'.ps'
      filenamecolor='demo_graphics2_color_'+prompt+'-'+sdec+'.ps'
      tvlct,r,g,b               ; restore colortable
      device,filename=filenamebwport,/port,color=0
      sub_demo_graphics2
      print,'printing test file as black&white postscript portrait '+filenamebwport
      device,/close
      device,filename=filenamebwland,/land,color=0
      sub_demo_graphics2
      print,'printing test file as black&white postscript landscape '+filenamebwland
      device,/close
      device,filename=filenamecolor,/port,color=1
      sub_demo_graphics2
      print,'printing test file as color postscript portrait '+filenamecolor
      device,/close
   endif
endfor
set_plot,'svg' ; change device
for decomposed=0,1 do begin
   if decomposed eq mode or mode eq 2 then begin
      device,decomposed=decomposed
      sdec=string(decomposed,format='(I1.1)')
      filename='demo_graphics2_'+prompt+'-'+sdec+'.svg'
      tvlct,r,g,b               ; restore colortable
      device,filename=filename
      sub_demo_graphics2
      print,'printing test file as svg: '+filename
      device,/close
   endif
endfor
!P.MULTI=0
set_plot,'x'

end

