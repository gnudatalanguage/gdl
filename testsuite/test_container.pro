;
; GregJ, December 2017: 
; first draft for a testsuite for IDL_CONTAINER
;
pro MYMESS, errors, message
errors=errors+1
MESSAGE, /continue, message
end
;
	pro TEST_CONTAINER, help=help, verbose=verbose, short=short, $
               no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_CONTAINER, help=help, verbose=verbose, short=short, $'
   print, '               no_exit=no_exit, test=test'
   return
endif
;
nb_errors=0
oc = obj_new('gdl_container')

pc = idl_container()

lc = list()   ; a list
GDL_C_NAME = 'GDL_CONTAINER'
is_compiled_in = (obj_class(pc) eq GDL_C_NAME)

on_error,0
txt='bad count() # 1 !!'
if(pc.count() ne 0) then MYMESS, nb_errors,txt
if(oc.count() ne 0) then MYMESS, nb_errors,txt
if(lc.count() ne 0) then MYMESS, nb_errors,txt
oc0=heap_refcount(oc)
;
if KEYWORD_SET(test) then STOP,' stop 0'
; adding 2 elements and counting again
;
for k=5,6 do pc.add,ptr_new(indgen(k))
for k=5,6 do lc.add,list(indgen(k))
for k=5,6 do oc.add,list(indgen(k))
txt='bad count() # 2 !!'
if(pc.count() ne 2) then MYMESS, nb_errors,txt
if(oc.count() ne 2) then MYMESS, nb_errors,txt
if(lc.count() ne 2) then MYMESS, nb_errors,txt
;
ppall = pc.get(/all)

if KEYWORD_SET(test) then STOP,' stop 1'
oc.add,pc
oc.add,lc[0]
oc.add,lc[1]
ll=list( ["gdl1","gdl2"])
hrc0=heap_refcount(ll)
oc.add,ll
if heap_refcount(ll) ne hrc0+1 then $
	MYMESS, nb_errors," refcount not incremented"
hrc0 = heap_refcount(lc)
oc.add,lc
if oc.count() ne 7 then $
	MYMESS, nb_errors,' object container count is off'
lca = objarr(2)
lca[0] = lc[0] & lca[1]=lc[1]
oc.add,lca
if oc.count() ne 9 then $
	MYMESS, nb_errors,' object container count is off'

if KEYWORD_SET(test) then STOP,' stop 2'
;
;  try to stuff pointers into oc, generating an error:
;
catch, ocerr
if ocerr ne 0 then $
  catch,/cancel $
  else $
	oc.add,ppall	; error message "Mixed pointers/Objects attempted"
if(ocerr eq 0) then MYMESS, nb_errors,' no error is an error' $
else  if KEYWORD_SET(verbose) then print,!error_state.msg
;
oc.add,ll
pc.add,ppall
print,pc.count()

print,' heap_refcount(oc):',heap_refcount(oc)
; ----------------- final messages ----------
;
BANNER_FOR_TESTSUITE, 'TEST_CONTAINER', nb_errors, short=short
;
;	if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP,' final stop'
;
end

