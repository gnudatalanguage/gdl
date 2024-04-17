;
; Unknown origins ... But test that an object is destroy
;
; Modifications history :
; -2024-04-15 AC : cleaning (details ...)
;
; ----------------------------------------
;
PRO ObjTEST::Cleanup, prm1, prm2, prm3
  COMPILE_OPT IDL2 ,HIDDEN
  ; Call our superclass Cleanup method
  ; self->IDL_Object::Cleanup ; (there is no GDL_OBJECT::cleanup)

nprm = N_PARAMS()
print,' ObjTEST::Cleanup, #params=',nprm
;
END
; ----------------------------------------
; 
PRO ObjTEST__define
  COMPILE_OPT IDL2 ,HIDDEN
  void = {ObjTEST, $
  inherits IDL_Object, $ ; superclass
  center: [0d, 0d], $ ; two-element array
  radius: 0d}  ; scalar value
END
;
; ----------------------------------------
;
pro TEST_OBJ_DESTROY, help=help, verbose=verbose, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'TEST_OBJ_DESTROY, help=help, verbose=verbose, $'
   print, '                test=test, no_exit=no_exit'
   return
endif
;
errors=0
;
llist = LIST(FLTARR(4),"hello",2.)
mlist = LIST(!pi, "goodbye",FINDGEN(3,4))
;
pps=PTRARR(2)
pps[0] = PTR_NEW(llist)
PPS[1] = PTR_NEW(mlist)
rcbeg = HEAP_REFCOUNT(pps)
;
OBJ_DESTROY, llist
OBJ_DESTROY, mlist
rcend = HEAP_REFCOUNT(pps)
;
if ~ARRAY_EQUAL(rcbeg,rcend,/no_typeconv) then $
   ERRORS_ADD, errors, 'Those two values should stay the same'
;
; since they have been destroyed ...
if OBJ_VALID(llist) then ERRORS_ADD, errors, 'llist OBJ not destroyed'
if OBJ_VALID(mlist) then ERRORS_ADD, errors, 'mlist OBJ not destroyed'
;
; ---------------------------
; See above the special methods ...
;
obTEST= OBJ_NEW('ObjTEST')
OBJ_DESTROY, obTEST, pps, 2
if OBJ_VALID(obTEST) then ERRORS_ADD, errors, 'obTEST OBJ not destroyed'
;
rcend2 = HEAP_REFCOUNT(pps)
if ~ARRAY_EQUAL(rcbeg,rcend2,/no_typeconv) then $
   ERRORS_ADD, errors, 'Those two values should stay the same'
;
; to prepare any further/future evolution, if any 
;
cumul_errors=errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_OBJ_DESTROY', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

