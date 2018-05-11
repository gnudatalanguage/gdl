;
;
; provide a test-case object besides LIST
; 
PRO ObjTEST::Cleanup, prm1, prm2, prm3
  COMPILE_OPT IDL2 ,HIDDEN
  ; Call our superclass Cleanup method
  ; self->IDL_Object::Cleanup ; (there is no GDL_OBJECT::cleanup)

nprm = N_PARAMS()
print,' ObjTEST::Cleanup, #params=',nprm
;
END
 
PRO ObjTEST__define
  COMPILE_OPT IDL2 ,HIDDEN
  void = {ObjTEST, $
  inherits IDL_Object, $ ; superclass
  center: [0d, 0d], $ ; two-element array
  radius: 0d}  ; scalar value
END
;
; GregJ, December 2017: 
; first draft for a testsuite for IDL_CONTAINER
;
pro MYMESS, errors, message
errors=errors+1
MESSAGE, /continue, message
end
;
;	pro TEST_CONTAINER, help=help, verbose=verbose, short=short, $
;               no_exit=no_exit, test=test
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
txt='just starting! count()==0?'
if(pc.count() ne 0) then MYMESS, nb_errors,txt
if(oc.count() ne 0) then MYMESS, nb_errors,txt
if(lc.count() ne 0) then MYMESS, nb_errors,txt
;
if KEYWORD_SET(test) then STOP,' stop 0'
; adding 2 elements and counting again
;
for k=5,6 do pc.add,ptr_new(indgen(k))
for k=5,6 do lc.add,list(indgen(k),/extract)
for k=5,6 do oc.add,list(indgen(k),/extract)

txt='stuffed pc,lc,and oc. count==2?'

if(pc.count() ne 2) then MYMESS, nb_errors,txt
if(oc.count() ne 2) then MYMESS, nb_errors,txt
if(lc.count() ne 2) then MYMESS, nb_errors,txt
;
; get an array of the pointers we made.

txt=' pointers contained in pc at ppall=PTRARR(2)'

	ppall = pc.get(/all)
	hrppall = heap_refcount(ppall)
	if keyword_set(verbose) then print,' heap_refcount of extracted pointers:',hrppall[0]
;  Checkpoint:
;	 ::add called for all three types. 
;		get() called once for ALL pointers in pc.

txt=' pp1 = pc.get(posit=1)'

pp1=pc.get(posit=1)
if hrppall[1]+1 ne heap_refcount(pp1) then MYMESS, nb_errors," ref count not incremented"
if pp1 ne ppall[1] then MYMESS, nb_errors," pp1=pc.get(posit=1) not right'
;
if KEYWORD_SET(test) then STOP,' stop 1: testing ::GET(/ALL,POSITION)'
; 

txt = ' create 4 objects from compiled-in {OBJTEST} testobj OBJARR(4)'

testobj = objarr(4)
for k=0,3 do testobj[k] = obj_new('OBJTEST')
;

txt = ' just kill oc and restart it - too heavy for now.

;	obj_destroy,oc
;	oc=obj_new('GDL_CONTAINER')

	oc.add,pc		; add the pointer container (ppall, pp1)
	oc.add,testobj[0]
; LC is a list that contains, itself, two lists (length 5 and 6)
	oc.add,lc[0]
	oc.add,testobj[1]
	oc.add,lc[1]
	oc.add,testobj[2]
;
	oc.add,list(["gdl1","gdl2"])
	oc.add,testobj[3]

txt = ' Container OC is stuffed. pc, {testobj}/{list}'

; lists are objects with overloaded bracket properties
;
numoc = oc.count()
hrclc = heap_refcount(lc) 	; a list containing two lists.
	oc.add,lc & numoc++;
	if oc.count() ne numoc then $  ;	Check it one last time.
		MYMESS, nb_errors,' object container count is off'
;
; take the two lists contained in LC and put them into an OBJARR(2)(=lca)
lca = objarr(2) & lca[0] = lc[0] & lca[1]=lc[1]
	oc.add,lca
	if oc.count() ne numoc+2 then $
		MYMESS, nb_errors,' object container count is off'

octest=oc.get(/all,isa='OBJTEST')
oclist = oc.get(/all,isa='LIST')
occont = oc.get(/all,isa='GDL_CONTAINER')
ocpc = oc.get(/all,isa='IDL_CONTAINER')
if KEYWORD_SET(test) then STOP,' stop 2: '+txt
;
ocall=oc.get(/all) & hrcoc=heap_refcount(ocall)
	oc.remove,/all
	if ~array_equal(hrcoc - heap_refcount(ocall),1) then $
		 MYMESS, nb_errors, " refcount for removed objects didn't decrement correctly"
	hrctest = heap_refcount(octest) & oc.add,octest
	hrclist = heap_refcount(oclist) & oc.add,oclist
	hrccont = heap_refcount(occont) & oc.add,occont
	if ~array_equal(heap_refcount(octest) - hrctest,1) then $
		 MYMESS, nb_errors, " refcount didn't decrement correctly (OCTEST)"
	if ~array_equal(heap_refcount(occont) - hrccont,1) then $
		 MYMESS, nb_errors, " refcount didn't decrement correctly (OCCONT)"
	if ~array_equal(heap_refcount(oclist) - hrclist,1) then $
		 MYMESS, nb_errors, " refcount didn't decrement correctly (OCLIST)"

txt = ' OC has beem unloaded (.remove,/all) and reloaded (octest, oclist, occont) '

ocall=oc.get(/all)
;			if ~is_compiled_in then oc.add,ocpc
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

if KEYWORD_SET(verbose) then print,' heap_refcount(oc):',heap_refcount(oc)
; ----------------- final messages ----------
;
BANNER_FOR_TESTSUITE, 'TEST_CONTAINER', nb_errors, short=short
;
;	if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP,' final stop'
;
end

