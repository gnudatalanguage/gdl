PRO MYPLOT,a,b,doplots,doxy,dooplot,noclip

  if (dooplot) then begin
     oplot,b,a,color=255,psym=8,NOCLIP=NOCLIP
  end

  if (doplots) then begin
     plots,[200],[1000],psym=5,color=88,SYMSIZE=5,THICK=5,NOCLIP=NOCLIP
     plots,[200,200],[-500,5000],color=88,THICK=5,NOCLIP=NOCLIP
     plots,[-100,1000],[1000,1000],color=88,THICK=5,NOCLIP=NOCLIP
     plots,200,1000,color=167,THICK=1,/CONT,NOCLIP=NOCLIP

     plots,[100],[100],psym=5,color=88,SYMSIZE=5,THICK=5,NOCLIP=NOCLIP,/DEV
     plots,[100,100],[0,1000],color=88,THICK=5,NOCLIP=NOCLIP,/DEV
     plots,[0,1000],[100,100],color=88,THICK=5,NOCLIP=NOCLIP,/DEV
     plots,100,100,color=167,THICK=1,/CONT,NOCLIP=NOCLIP,/DEV

     plots,[0.6],[0.6],psym=5,color=88,SYMSIZE=5,THICK=5,NOCLIP=NOCLIP,/NORM
     plots,[0.6,0.6],[0,1],color=88,THICK=5,NOCLIP=NOCLIP,/NORM
     plots,[0,1],[0.6,0.6],color=88,THICK=5,NOCLIP=NOCLIP,/NORM
     plots,0.6,0.6,color=167,THICK=1,/CONT,NOCLIP=NOCLIP,/NORM
  end
; test if we are gdl
DEFSYSV, '!gdl', exists=iamgdl
  if (doxy) then begin
     if (iamgdl) then begin
        xyouts,200,1000,replicate("world",12),color=1986543-[findgen(12)]*20000,charsize=1,align=0.0,ori=30*[findgen(12)],NOCLIP=NOCLIP
        xyouts,100,100,replicate("device",12),color=1986543-[findgen(12)]*20000,charsize=1,align=0.0,ori=30*[findgen(12)],NOCLIP=NOCLIP,/DEVICE
        xyouts,0.6,0.6,replicate("normal",12),color=1986543-[findgen(12)]*20000,charsize=1,align=0.0,ori=30*[findgen(12)],NOCLIP=NOCLIP,/NORM
     endif else begin
        for i=0,12,1 do xyouts,200,1000,"world",color=1986543-i*20000,charsize=1,align=0.0,ori=30*i,NOCLIP=NOCLIP
        for i=0,12,1 do xyouts,100,100,"device",color=1986543-i*20000,charsize=1,align=0.0,ori=30*i,NOCLIP=NOCLIP,/DEVICE
        for i=0,12,1 do xyouts,0.6,0.6,"normal",color=1986543-i*20000,charsize=1,align=0.0,ori=30*i,NOCLIP=NOCLIP,/NORM
     end
  end
END

PRO sub_demo_graphics1,noplots=plots,nooplot=oplot,noxyouts=xyouts,noclip=noclip


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
 
!P.MULTI=[0,2,2] 

a=findgen(100) & a*=10 & b=a mod 220 & a=a+313.3 & b=b+1

!X.TITLE='linear X Axis' & !Y.TITLE='linear Y Axis'
plot,b,a,psym=4,xrange=[1,200],yrange=[1,2000]
MYPLOT,a,b,doplots,doxy,dooplot,noclip


!X.TITLE='linear X Axis' & !Y.TITLE='log Y Axis'
plot,b,a,psym=4,xrange=[1,200],yrange=[1,2000],/ylog
MYPLOT,a,b,doplots,doxy,dooplot,noclip

!X.TITLE='log X Axis' & !Y.TITLE='linear Y Axis'
plot,b,a,psym=4,xrange=[1,200],yrange=[1,2000],/xlog
MYPLOT,a,b,doplots,doxy,dooplot,noclip

!X.TITLE='log X Axis' & !Y.TITLE='log Y Axis'
plot,b,a,psym=4,xrange=[1,200],yrange=[1,2000],/ylog,/xlog
MYPLOT,a,b,doplots,doxy,dooplot,noclip

!P.MULTI=0 

end

PRO demo_graphics1,print=print,mode=mode,help=help
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
      sub_demo_graphics1
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
      sub_demo_graphics1
      print,'printing test file as black&white postscript portrait '+filenamebwport
      device,/close
      device,filename=filenamebwland,/land,color=0
      sub_demo_graphics1
      print,'printing test file as black&white postscript landscape '+filenamebwland
      device,/close
      device,filename=filenamecolor,/port,color=1
      sub_demo_graphics1
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
      sub_demo_graphics1
      print,'printing test file as svg: '+filename
      device,/close
   endif
endfor
!P.MULTI=0
set_plot,'x'

end
