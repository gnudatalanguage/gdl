PRO MYPLOT,a,b,doplots,doxy,dooplot,noclip

  if (dooplot) then begin
     oplot,b,a,color=255,psym=8
  end

  if (doplots) then begin
     plots,b,a,psym=2,noclip=0
     plots,[200],[1000],psym=5,color='00FFF0'x,SYMSIZE=15,THICK=5,LINESTYLE=0,NOCLIP=1
     plots,[200,200],[1,5000],color=88,THICK=5,LINESTYLE=0,NOCLIP=0
     plots,[1,1000],[1000,1000],color=88,THICK=5,LINESTYLE=0,NOCLIP=0

     plots,[100],[100],psym=5,color='00FFF0'x,SYMSIZE=15,THICK=5,LINESTYLE=4,NOCLIP=1,/DEV
     plots,[100,100],[0,1000],color=88,THICK=5,LINESTYLE=4,NOCLIP=1,/DEV
     plots,[0,1000],[100,100],color=88,THICK=5,LINESTYLE=4,NOCLIP=1,/DEV

     plots,[0.5],[0.5],psym=5,color='00FFF0'x,SYMSIZE=15,THICK=5,LINESTYLE=5,NOCLIP=1,/NORM
     plots,[0.5,0.5],[0,1],color=88,THICK=5,LINESTYLE=5,NOCLIP=1,/NORM
     plots,[0,1],[0.5,0.5],color=88,THICK=5,LINESTYLE=5,NOCLIP=1,/NORM
  end
; test if we are gdl
DEFSYSV, '!gdl', exists=iamgdl
  if (doxy) then begin
     if (iamgdl) then begin
        xyouts,200,1000,replicate("world 200,1000",12),color=1986543-[findgen(12)]*20000,charsize=1,align=0.0,ori=30*[findgen(12)]
        xyouts,100,100,replicate("device 100,100",12),color=1986543-[findgen(12)]*20000,charsize=1,align=0.0,ori=30*[findgen(12)],/DEVICE
        xyouts,0.5,0.5,replicate("normal, center",12),color=1986543-[findgen(12)]*20000,charsize=1,align=0.0,ori=30*[findgen(12)],/NORM
     endif else begin
        for i=0,12,1 do xyouts,200,1000,"world 200 1000",color=1986543-i*20000,charsize=1,align=0.0,ori=30*i
        for i=0,12,1 do xyouts,100,100,"device 100 100",color=1986543-i*20000,charsize=1,align=0.0,ori=30*i,/DEVICE
        for i=0,12,1 do xyouts,0.5,0.5,"normal, center",color=1986543-i*20000,charsize=1,align=0.0,ori=30*i,/NORM
     end
  end
END

PRO sub_demo_graphics1,panel,noplots=plots,nooplot=oplot,noxyouts=xyouts,noclip=noclip

if (n_elements(panel) eq 0) then begin
 all=1 
 panel=0
endif else all=(panel gt 4 or panel lt 1)


!P.SUBTITLE='HERE IS A SUBTITLE' & !P.TITLE='HERE IS A TITLE' 

AA = FINDGEN(17) * (!PI*2/16.) & USERSYM, COS(AA), SIN(AA)

doplots=1
if (keyword_set(noplots)) then doplots=0
doxy=1
if (keyword_set(noxyouts)) then doxy=0
dooplot=1
if (keyword_set(nooplot)) then dooplot=0
clip=0
if (keyword_set(noclip)) then clip=1

if (all) then !P.MULTI=[0,2,2] 

a=findgen(100) & a*=10 & b=a mod 220 & a=a+313.3 & b=b+1

if (panel eq 1 or all ) then begin 
 !X.TITLE='linear X Axis' & !Y.TITLE='linear Y Axis'
 plot,b,a,psym=6,xrange=[1,200],yrange=[1,2000]
 MYPLOT,a,b,doplots,doxy,dooplot,noclip
endif

if (panel eq 2 or all ) then begin 
 !X.TITLE='linear X Axis' & !Y.TITLE='log Y Axis'
 plot,b,a,psym=6,xrange=[1,200],yrange=[1,2000],/ylog
 MYPLOT,a,b,doplots,doxy,dooplot,noclip
endif

if (panel eq 3 or all ) then begin 
   !X.TITLE='log X Axis' & !Y.TITLE='linear Y Axis'
   plot,b,a,psym=6,xrange=[1,200],yrange=[1,2000],/xlog
   MYPLOT,a,b,doplots,doxy,dooplot,noclip
endif

if (panel eq 4 or all ) then begin 
   !X.TITLE='log X Axis' & !Y.TITLE='log Y Axis'
   plot,b,a,psym=6,xrange=[1,200],yrange=[1,2000],/ylog,/xlog
   MYPLOT,a,b,doplots,doxy,dooplot,noclip
endif

!P.MULTI=0 

end

PRO demo_graphics1,print=print,mode=mode,help=help,panel=panel
if keyword_set(help) then begin
 print,"Usage: demo_graphics1,[mode=[0,1,2]],[/print],panel=panel"
 print,"mode=0 produces decomposed=0 output"
 print,"mode=1 produces decomposed=1 output"
 print,"mode=2 or KW mode absent: produces both outputs"
 print,"panel=[0,1,2,3,4] plots only panel 1 (or 2,or 3..) or all 4 panels if  0 or absent"
 print,"/print produces additionally various Postscript and SVG outputs."
 return
endif
if ~size(mode,/type)  then mode=2
for decomposed=0,1 do begin
   if decomposed eq mode or mode eq 2 then begin
      device,decomposed=decomposed
      sub_demo_graphics1,panel
      xyouts,0,0,"decomposed: "+string(mode,format='(I1.1)'),/norm
      if ( mode eq 2 ) then wait,1
   endif
endfor
if (not keyword_set(print)) then return
prompt=strmid(!PROMPT,0,1)
tvlct,r,g,b,/get ; save colors
set_plot,'ps' ; change device
if ~size(mode,/type)  then mode=2
for decomposed=0,1 do begin
   if decomposed eq mode or mode eq 2 then begin
      device,decomposed=decomposed
      sdec=string(decomposed,format='(I1.1)')
      filenamebwport='demo_graphics1_bw_port_'+prompt+'-'+sdec+'.ps'
      filenamebwland='demo_graphics1_bw_land_'+prompt+'-'+sdec+'.ps'
      filenamecolor='demo_graphics1_color_'+prompt+'-'+sdec+'.ps'
      tvlct,r,g,b               ; restore colortable
      device,filename=filenamebwport,/port,color=0
      sub_demo_graphics1,panel
      print,'printing test file as black&white postscript portrait '+filenamebwport
      device,/close
      device,filename=filenamebwland,/land,color=0
      sub_demo_graphics1,panel
      print,'printing test file as black&white postscript landscape '+filenamebwland
      device,/close
      device,filename=filenamecolor,/port,color=1
      sub_demo_graphics1,panel
      print,'printing test file as color postscript portrait '+filenamecolor
      device,/close
   endif
endfor
set_plot,'svg' ; change device
for decomposed=0,1 do begin
   if decomposed eq mode or mode eq 2 then begin
      device,decomposed=decomposed
      sdec=string(decomposed,format='(I1.1)')
      filename='demo_graphics1_'+prompt+'-'+sdec+'.svg'
      tvlct,r,g,b               ; restore colortable
      device,filename=filename
      sub_demo_graphics1,panel
      print,'printing test file as svg: '+filename
      device,/close
   endif
endfor
!P.MULTI=0
set_plot,'x'

end
