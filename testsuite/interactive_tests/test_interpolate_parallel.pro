pro test_interpolate_parallel

  niter=500
  oldcpu=!cpu
  initial=oldcpu.tpool_nthreads
  n=2*!cpu.TPOOL_MIN_ELTS
  n2=long(sqrt(n))
  n3=long(n^0.333)
  a=findgen(n)
  b=findgen(n)/n*(n+100)-50 & c=b &d=c
;  help,a,b,c,d
  error=0
                                ;1D
  defsysv,"!GDL",exists=isgdl

  if (isgdl) then begin
     print,'1D nearest'
     t1=tic() & for i=1,niter do resin=interpolate(a,b,/near) & t=toc() & in=t-t1.time
     cpu,tpool_nthreads=1
     t2=tic() & for i=1,niter do     resout=interpolate(a,b,/near) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
     cpu,tpool_nthreads=initial
     if (total(resin-resout) gt 0 ) then error++
  endif
  

  print,'1D linear'
  t1=tic() & for i=1,niter do resin=interpolate(a,b) & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a,b) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'1D linear missing'
  t1=tic() & for i=1,niter do resin=interpolate(a,b,miss=-!DPI) & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do    resout=interpolate(a,b,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'1D cubic'
  t1=tic() & for i=1,niter do resin=interpolate(a,b,cub=-0.345) & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do resout=interpolate(a,b,cub=-0.345) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'1D cubic missing'
  t1=tic() & for i=1,niter do resin=interpolate(a,b,cub=-0.345,miss=-!DPI) & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a,b,cub=-0.345,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++

                                ;2D
  
  a2=findgen(n2,n2)
  b=findgen(n)/n*(n2+100)-50 & c=b &d=c
  bgrid=findgen(n2)/n2*(n2+100)-50 &cgrid=bgrid
;  help,a2,bgrid,cgrid
  if (isgdl) then begin
     print,' 2D nearest '
     t1=tic() & for i=1,niter do   resin=interpolate(a2,b,c,/near)  & t=toc() & in=t-t1.time
     cpu,tpool_nthreads=1
     t2=tic() & for i=1,niter do     resout=interpolate(a2,b,c,/near) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
     cpu,tpool_nthreads=initial
     if (total(resin-resout) gt 0 ) then error++
     print,' 2D nearest grid '
     t1=tic() & for i=1,niter do   resin=interpolate(a2,bgrid,cgrid,/grid,/near)  & t=toc() & in=t-t1.time
     cpu,tpool_nthreads=1
     t2=tic() & for i=1,niter do     resout=interpolate(a2,bgrid,cgrid,/grid,/near) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
     cpu,tpool_nthreads=initial
     if (total(resin-resout) gt 0 ) then error++
  endif

  print,'2D linear'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,b,c)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a2,b,c) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'2D linear missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,b,c,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do    resout=interpolate(a2,b,c,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'2D linear grid'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,bgrid,cgrid,/grid)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a2,bgrid,cgrid,/grid) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'2D linear grid missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,bgrid,cgrid,/grid,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do    resout=interpolate(a2,bgrid,cgrid,/grid,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++

  print,'2D cubic'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,b,c,cubic=-0.345)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a2,b,c,cubic=-0.345) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'2D cubic missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,b,c,cubic=-0.345,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do   resout=interpolate(a2,b,c,cubic=-0.345,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'2D cubic grid'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,bgrid,cgrid,cubic=-0.345,/grid)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a2,bgrid,cgrid,cubic=-0.345,/grid) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'2D cubic grid missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,bgrid,cgrid,cubic=-0.345,/grid,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do   resout=interpolate(a2,bgrid,cgrid,cubic=-0.345,/grid,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++

;3D
  
  a3=findgen(n3,n3,n3)
  b=findgen(n)/n*(n3+10)-5 & c=b &d=c
  bgrid=findgen(n3)/n3*(n3+100)-50 & cgrid=bgrid & dgrid=cgrid
;  help,a3,b,c,d,bgrid,cgrid,dgrid

  print,'3D linear'
  t1=tic() & for i=1,niter do   resin=interpolate(a3,b,c,d)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a3,b,c,d) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'3D linear missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a3,b,c,d,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do    resout=interpolate(a3,b,c,d,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'3D linear grid'
  t1=tic() & for i=1,niter do   resin=interpolate(a3,bgrid,cgrid,dgrid,/grid)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a3,bgrid,cgrid,dgrid,/grid) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++
  print,'3D linear grid missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a3,bgrid,cgrid,dgrid,/grid,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do    resout=interpolate(a3,bgrid,cgrid,dgrid,/grid,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (total(resin-resout) gt 0 ) then error++

; complex is equivalent to another dimension (2) so this tests both
; complex and not-single-element arrays (see C++ code)
  a=cindgen(n)
  b=findgen(n)/n*(n+100)-50 & c=b &d=c
;  help,a,b,c,d
  error=0
                                ;1D
  if (isgdl) then begin
     print,' COMPLEX 1D nearest'
     t1=tic() & for i=1,niter do resin=interpolate(a,b,/near) & t=toc() & in=t-t1.time
     cpu,tpool_nthreads=1
     t2=tic() & for i=1,niter do     resout=interpolate(a,b,/near) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
     cpu,tpool_nthreads=initial
     if (real_part(total(resin-resout)) gt 0 ) then error++
  endif
  print,' COMPLEX 1D linear'
  t1=tic() & for i=1,niter do resin=interpolate(a,b) & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a,b) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 1D linear missing'
  t1=tic() & for i=1,niter do resin=interpolate(a,b,miss=-!DPI) & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do    resout=interpolate(a,b,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 1D cubic'
  t1=tic() & for i=1,niter do resin=interpolate(a,b,cub=-0.345) & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do resout=interpolate(a,b,cub=-0.345) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 1D cubic missing'
  t1=tic() & for i=1,niter do resin=interpolate(a,b,cub=-0.345,miss=-!DPI) & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a,b,cub=-0.345,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++

                                ;2D
  
  a2=cindgen(n2,n2)
  b=findgen(n)/n*(n2+100)-50 & c=b &d=c
  bgrid=findgen(n2)/n2*(n2+100)-50 &cgrid=bgrid
;  help,a2,bgrid,cgrid
  if (isgdl) then begin
     print,' COMPLEX  2D nearest '
     t1=tic() & for i=1,niter do   resin=interpolate(a2,b,c,/near)  & t=toc() & in=t-t1.time
     cpu,tpool_nthreads=1
     t2=tic() & for i=1,niter do     resout=interpolate(a2,b,c,/near) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
     cpu,tpool_nthreads=initial
     if (real_part(total(resin-resout)) gt 0 ) then error++
     print,' COMPLEX  2D nearest grid '
     t1=tic() & for i=1,niter do   resin=interpolate(a2,bgrid,cgrid,/grid,/near)  & t=toc() & in=t-t1.time
     cpu,tpool_nthreads=1
     t2=tic() & for i=1,niter do     resout=interpolate(a2,bgrid,cgrid,/grid,/near) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
     cpu,tpool_nthreads=initial
     if (real_part(total(resin-resout)) gt 0 ) then error++
  endif

  print,' COMPLEX 2D linear'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,b,c)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a2,b,c) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 2D linear missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,b,c,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do    resout=interpolate(a2,b,c,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 2D linear grid'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,bgrid,cgrid,/grid)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a2,bgrid,cgrid,/grid) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 2D linear grid missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,bgrid,cgrid,/grid,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do    resout=interpolate(a2,bgrid,cgrid,/grid,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++

  print,' COMPLEX 2D cubic'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,b,c,cubic=-0.345)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a2,b,c,cubic=-0.345) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 2D cubic missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,b,c,cubic=-0.345,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do   resout=interpolate(a2,b,c,cubic=-0.345,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 2D cubic grid'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,bgrid,cgrid,cubic=-0.345,/grid)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a2,bgrid,cgrid,cubic=-0.345,/grid) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 2D cubic grid missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a2,bgrid,cgrid,cubic=-0.345,/grid,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do   resout=interpolate(a2,bgrid,cgrid,cubic=-0.345,/grid,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++

;3D
  
  a3=cindgen(n3,n3,n3)
  b=findgen(n)/n*(n3+10)-5 & c=b &d=c
  bgrid=findgen(n3)/n3*(n3+100)-50 & cgrid=bgrid & dgrid=cgrid
;  help,a3,b,c,d,bgrid,cgrid,dgrid

  print,' COMPLEX 3D linear'
  t1=tic() & for i=1,niter do   resin=interpolate(a3,b,c,d)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a3,b,c,d) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 3D linear missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a3,b,c,d,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do    resout=interpolate(a3,b,c,d,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 3D linear grid'
  t1=tic() & for i=1,niter do   resin=interpolate(a3,bgrid,cgrid,dgrid,/grid)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do     resout=interpolate(a3,bgrid,cgrid,dgrid,/grid) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++
  print,' COMPLEX 3D linear grid missing'
  t1=tic() & for i=1,niter do   resin=interpolate(a3,bgrid,cgrid,dgrid,/grid,miss=-!DPI)  & t=toc() & in=t-t1.time
  cpu,tpool_nthreads=1
  t2=tic() & for i=1,niter do    resout=interpolate(a3,bgrid,cgrid,dgrid,/grid,miss=-!DPI) & t=toc() & out=t-t2.time & s=out/in & print,"time="+strtrim(in,2)+", thread gain: "+strtrim(s,2)+", error counter: "+strtrim(error,2)
  cpu,tpool_nthreads=initial
  if (real_part(total(resin-resout)) gt 0 ) then error++


  if (error) then message,strtrim(error,2)+" Errors found."
  
  
end
