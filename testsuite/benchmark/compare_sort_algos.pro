;
; I've added a GDL_SORT() "private" function that permits to use one of: /RADIX, /QUICK, /MERGE and /INSERT 
; sorting method. This function is used in this procedure
; to plot the performances of all the different sorts (insertion is not used as it is in N^2 and would take forever)
; and adjust the combination of all these methods that makes for the performance in the final sort().
; With a modicum of work, this could be used to fine-tune the SORT() method for a particular machine or purpose,
; maintaining top performances in specific cases (clusters etc).


function gdl_sort
; to avoid problems with IDL!
return, 0
end

pro plot_sort_algos, reftime, time, variant=variant
  load=(n_elements(reftime) eq 0) 
; variant plots the ratio vs. IDL
  other=(n_elements(variant) gt 0)
  yrange=other?[0.01,20]:[1E-8,100]
  type=['random','incr','decr','saw']
  syms=[0,0,0,0,-6]
  colors=['FF0000'x,'00FF00'x,'0000FF'x,'00FFFF'x,'FFFF00'x]
  names=['radix','merge','quick','auto','idl']
  tagy=other?[0.3,0.5,1.3,10,30,70]:[0.0001,0.001,0.01,0.1,1,10]

  if (load) then restore,"bench_sort_gdl.sav"

  ntimes=(size(time))[1]
  idl=time[*,*,4]
  auto=time[*,*,3]
  minimum=fltarr(ntimes,4)
  for k=0,3 do minimum[*,k]=min(time[*,k,[0:3]],dim=2)
  perf=idl/minimum
  perfauto=idl/auto
  if (other) then for k=0,4 do time[*,*,k]/=idl
  !P.MULTI=[0,2,2]
  for k=0,3 do begin            ; 4 cases
     plot,reftime,/nodata,xrange=[1,1E9],yrange=yrange,/xlo,/ylo,title=type[k],ysty=8 
     for m=0,4 do oplot,reftime,time[*,k,m],psym=syms[m],color=colors[m]
     xyouts,2.,tagy,names,color=colors
     plot,reftime,/nodata,xrange=[1,1E9],yrange=[0,20],/xlo,xsty=5,ysty=5,/noerase
     oplot,reftime,perf[*,k],linestyle=2
     oplot,reftime,perfauto[*,k],linestyle=2,color='FFFF00'x
     axis,/yaxis,ytitle='speed factor'
  endfor
  !P.MULTI=0
end

pro do_compare_sort_algos, zerotime=zerotime, check=check, variant=variant

  if (n_elements(zerotime) eq 0) then runnull=1 else runnull=0
  if (n_elements(check) eq 0) then check=0

  ; idl/gdl switch
  DEFSYSV, '!GDL', exist=isgdl

  savgdl="bench_sort_gdl.sav"
  savidl="bench_sort_idl.sav"
  savename=isgdl?savgdl:savidl
  idlsave_exists=(file_info(savidl)).exists
  if (isgdl and idlsave_exists) then begin
     restore,"bench_sort_idl.sav"
     idltime=time[*,*,3]
  endif

; generate ntimes values in reftime. MUST STOP AT or BEFORE 1E8
;  for j=1,ntimes do reftime[j-1]=10.^j
  reftime=[3,8,16,24,32,100,1000,10000,1E5,1E6,1E7,1E8]
  refcount=[10000,5000,2000,1000,500,200,100,70,30,10,3,1]
  ntimes=n_elements(reftime)
  
  time=reform(replicate(!values.f_nan,4*ntimes*5),ntimes,4,5)
  ; restore idl time if present
  if (n_elements(idltime) gt 0) then time[*,*,4]=idltime

  val=randomn(seed,2E8)
  lin=findgen(2e8)
  inv=[2e8:0:-1]
  tooth=sin(dist(16)) & tooth=reform(tooth,16*16) & saw=rebin(tooth,256000000LL)


  for j=1,ntimes do begin
     nbps=reftime[j-1]
; ntrials to correctly take into account small numbers.
     ntrials=refcount[j-1]
     a=val[0:nbps-1]
     if (runnull) then begin 
        t=tic()&  for i=1,ntrials do b=a & time[j-1,*,*]=toc(t)/ntrials
     endif else begin
        if (isgdl) then begin
           t=tic()&  for i=1,ntrials do b=gdl_sort(a ,/radix)  & time[j-1,0,0]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" random and radix"
           t=tic()&  for i=1 ,ntrials do b=gdl_sort(a ,/merge)  & time[j-1,0,1]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" random and merge"
           t=tic()&  for i=1,ntrials do b=gdl_sort(a ,/quick)  & time[j-1,0,2]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" random and quick"
        endif
        t=tic()&  for i=1,ntrials do b=sort(a)         & time[j-1,0,3]=toc(t)/ntrials
        if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" random and default sort"
        a=lin[0:nbps-1]
        if (isgdl) then begin
           t=tic()&  for i=1,ntrials do b=gdl_sort(a ,/radix)  & time[j-1,1,0]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" ingden and radix"
           t=tic()&  for i=1,ntrials do b=gdl_sort(a ,/merge)  & time[j-1,1,1]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" ingden and merge"
           t=tic()&  for i=1,ntrials do b=gdl_sort(a ,/quick)  & time[j-1,1,2]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" ingden and quick"
        endif
        t=tic()&  for i=1,ntrials do b=sort(a)         & time[j-1,1,3]=toc(t)/ntrials
        if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" ingden and default sort"
        a=inv[0:nbps-1]
        if (isgdl) then begin
           t=tic()&  for i=1,ntrials do b=gdl_sort(a ,/radix)  & time[j-1,2,0]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" reverse and radix"
           t=tic()&  for i=1,ntrials do b=gdl_sort(a ,/merge)  & time[j-1,2,1]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" reverse and merge"
           t=tic()&  for i=1,ntrials do b=gdl_sort(a ,/quick)  & time[j-1,2,2]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" reverse and quick"
        endif
        t=tic()&  for i=1,ntrials do b=sort(a)         & time[j-1,2,3]=toc(t)/ntrials
        if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" reverse and default sort"
        a=saw[0:nbps-1]
        if (isgdl) then begin
           t=tic()&  for i=1,ntrials do b=gdl_sort(a ,/radix)  & time[j-1,3,0]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" saw and radix"
           t=tic()&  for i=1,ntrials do b=gdl_sort(a ,/merge)  & time[j-1,3,1]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" saw and merge"
           t=tic()&  for i=1,ntrials do b=gdl_sort(a ,/quick)  & time[j-1,3,2]=toc(t)/ntrials
           if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" saw and quick"
        endif
        t=tic()&  for i=1,ntrials do b=sort(a)         & time[j-1,3,3]=toc(t)/ntrials
        if (check) then if median(a)-a[b[nbps/2]] then message,/info,"sort() invalid for nbps="+strtrim(nbps,2)+" saw and default sort"
        if (isgdl and ~runnull) then plot_sort_algos,reftime,time, variant=variant
     endelse
  endfor
  if (runnull) then begin
     zerotime=time ; return zerotime for use in second pass
  endif else begin
     time-=zerotime; remove zerotime
     if (n_elements(idltime) gt 0) then time[*,*,4]=idltime ; restore idl time if present
     save,reftime,time,filename=savename; save debiassed values.
  endelse
  if (isgdl and ~runnull) then plot_sort_algos, variant=variant; final plot
end
pro compare_sort_algos, check=check, variant=variant
  forward_function gdl_sort
  do_compare_sort_algos, zerotime=zerotime; initialize zerotime with empty for loops
  do_compare_sort_algos, zerotime=zerotime, check=check, variant=variant ; the real job, removing the zerotime at the end.
end
