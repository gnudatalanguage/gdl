; just exercising the idl_idlbridge object and library, not a real test
pro test_gdl2gdl_basic, cumul_errors
;not yet on windows.
if STRLOWCASE(!version.os_name) eq 'windows' then exit, status=0
  cumul_errors=0
  a=obj_new("idl_idlbridge")
  a->Setvar,"A",indgen(10)
  a->execute,"A-=indgen(10)"
  a->execute,"A+=1"
  b=a->getvar("A")
  if (total(b) ne 10) then ERRORS_ADD, cumul_errors, 'BASIC'
end

pro test_gdl2gdl_multi, cumul_errors
  cumul_errors=0
; make shmap name unique, we do not want to get problems
  shmapname='TEST_GDL_MULTI'+strtrim(randomu(seed,/ulong),2)
  shmmap,shmapname,/LONG,1
  result=shmvar(shmapname)
  result[0]=33
  a=obj_new("idl_idlbridge")
  a->execute,'shmapname="'+shmapname+'" & shmmap,shmapname,/LONG,1 & var=shmvar(shmapname)'
  b=obj_new("idl_idlbridge")
  b->execute,'shmapname="'+shmapname+'" & shmmap,shmapname,/LONG,1 & var=shmvar(shmapname)'
  c=obj_new("idl_idlbridge")
  c->execute,'shmapname="'+shmapname+'" & shmmap,shmapname,/LONG,1 & var=shmvar(shmapname)'
  ; make  all 3 subprocess add 1 at various times . We do not test here the concurrency of access to the shared memory which should be programmed by the user using SEMAPHORES
  a->execute,"wait,1 & var[0]+=1", /now
  b->execute,"wait,2 & var[0]+=1", /now
  c->execute,"wait,3 & var[0]+=1", /now
  ; wait for 'c'
  s=c->status()
  while s eq 1 do begin
     wait,1
     s=c->status()
  endwhile
  ; check other's status is 2 as they must have returned earlier
  if a->status() ne 2 then  ERRORS_ADD, cumul_errors, 'first process problem'
  if b->status() ne 2 then  ERRORS_ADD, cumul_errors, 'second process problem'

  if result[0] ne 36 then ERRORS_ADD, cumul_errors, 'total is bad'
  ; will stop all 3 processes before action. timeout of 100 is necessary
  ; (although overkill) because idl (? not gdl) seems to take a lot of time
  ; to abort a subprocess.
  result[0] = 33
  a->execute,"wait,100 & var[0]+=1", /now
  b->execute,"wait,100 & var[0]+=1", /now
  c->execute,"wait,100 & var[0]+=1", /now
  wait,2
  a->abort
  b->abort
  c->abort
  wait,4 ; leave time to a process to eventually do the job because it was not cancelled.
  if result[0] ne 33 then ERRORS_ADD, cumul_errors, 'subprocess abort problem'
  ; gracefully destroys objects
  OBJ_DESTROY,a
  OBJ_DESTROY,b
  OBJ_DESTROY,c
end

pro test_gdl2gdl, help=help, no_exit=no_exit
  cumul_errors=0

  test_gdl2gdl_basic, cumul_errors
  test_gdl2gdl_multi, cumul_errors
 ;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_GDL2GDL', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1

 
end


