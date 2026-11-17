; OBJ_DESTROY error behavior verified against IDL 8.4, cases D1-D7 from
; obj_destroy_idl_check_20261014.pro. Keep surviving objects referenced until
; after the checks, so automatic cleanup cannot obscure the destruction order.
function OD_ERROR_OBJECT::INIT, label, fail, child
self.label=label
self.fail=fail
if N_ELEMENTS(child) GT 0 then self.child=child
return, 1
end

pro OD_ERROR_OBJECT::CLEANUP
common od_error_log, log
log=[log, self.label]
if OBJ_VALID(self.child) then begin
   OBJ_DESTROY, self.child
   log=[log, self.label+':after_child']
endif
if self.fail then MESSAGE, 'cleanup failed: '+self.label
end

pro OD_ERROR_OBJECT::DEFUSE
self.fail=0
end

pro OD_ERROR_OBJECT__DEFINE
definition={od_error_object, label:'', fail:0, child:OBJ_NEW()}
end

pro OD_ERROR_CATCH_DESTROY, objects, status, msg, after
CATCH, error
if error NE 0 then begin
   CATCH, /CANCEL
   status=0
   msg=!ERROR_STATE.MSG
   return
endif
OBJ_DESTROY, objects
after=1
status=1
end

pro TEST_OBJ_DESTROY_ERRORS
common od_error_log, log
errors=0
level=SCOPE_LEVEL()
existing=OBJ_VALID(COUNT=objects_before)
for n=1,7 do begin
   log=['']
   name='D'+STRTRIM(n, 2)
   expected_label='A'
   expected_log='A'
   expected_valid=[0, 1]
   case n of
      1: objects=[OBJ_NEW('od_error_object', 'A', 1), $
                  OBJ_NEW('od_error_object', 'B', 0)]
      2: begin
         objects=[OBJ_NEW('od_error_object', 'A', 0), $
                  OBJ_NEW('od_error_object', 'B', 1), $
                  OBJ_NEW('od_error_object', 'C', 0)]
         expected_label='B'
         expected_log='A,B'
         expected_valid=[0, 0, 1]
      end
      3: objects=[OBJ_NEW('od_error_object', 'A', 1), $
                  OBJ_NEW('od_error_object', 'B', 0)]
      4: begin
         objects=OBJ_NEW('od_error_object', 'A', 1)
         expected_valid=0
      end
      5: begin
         child=OBJ_NEW('od_error_object', 'B', 1)
         parent=OBJ_NEW('od_error_object', 'A', 0, child)
         objects=[parent, child]
         expected_label='B'
         expected_log='A,B'
         expected_valid=[0, 0]
      end
      6: begin
         child=OBJ_NEW('od_error_object', 'B', 0)
         parent=OBJ_NEW('od_error_object', 'A', 1, child)
         objects=[parent, child]
         expected_log='A,B,A:after_child'
         expected_valid=[0, 0]
      end
      7: objects=[OBJ_NEW('od_error_object', 'A', 1), $
                  OBJ_NEW('od_error_object', 'B', 1)]
   endcase

   after=0
   msg=''
   if n EQ 3 then begin
      ; Also run this test in a child session without an enclosing EXECUTE.
      OD_ERROR_CATCH_DESTROY, objects, status, msg, after
   endif else begin
      if (n EQ 5) OR (n EQ 6) then $
         status=EXECUTE('OBJ_DESTROY, parent & after=1') else $
         status=EXECUTE('OBJ_DESTROY, objects & after=1')
      msg=!ERROR_STATE.MSG
   endelse
   if status NE 0 then ERRORS_ADD, errors, name+': failure status'
   if after NE 0 then ERRORS_ADD, errors, name+': continued after failure'
   if ~ARRAY_EQUAL(OBJ_VALID(objects), expected_valid) then $
      ERRORS_ADD, errors, name+': object validity'
   if STRJOIN(log[1:*], ',') NE expected_log then $
      ERRORS_ADD, errors, name+': cleanup order'
   if msg NE 'OD_ERROR_OBJECT::CLEANUP: cleanup failed: '+expected_label then $
      ERRORS_ADD, errors, name+': error message'
   if SCOPE_LEVEL() NE level then ERRORS_ADD, errors, name+': call stack'

   if n EQ 4 then begin
      status=EXECUTE('OBJ_DESTROY, objects')
      if status NE 1 then ERRORS_ADD, errors, name+': repeat destroy status'
      if OBJ_VALID(objects) then ERRORS_ADD, errors, name+': repeat destroy validity'
      if STRJOIN(log[1:*], ',') NE 'A' then $
         ERRORS_ADD, errors, name+': repeated CLEANUP'
   endif
   ; Destroy survivors only after checking the first failure's effects.
   for i=0,N_ELEMENTS(objects)-1 do begin
      if OBJ_VALID(objects[i]) then begin
         objects[i]->DEFUSE
         OBJ_DESTROY, objects[i]
      endif
   endfor
   after=0
   if EXECUTE('after=1') NE 1 then ERRORS_ADD, errors, name+': stale error'
   if after NE 1 then ERRORS_ADD, errors, name+': execution did not recover'
endfor
remaining=OBJ_VALID(COUNT=objects_after)
if objects_after NE objects_before then ERRORS_ADD, errors, 'object heap leak'
BANNER_FOR_TESTSUITE, 'TEST_OBJ_DESTROY_ERRORS', errors
if errors GT 0 then EXIT, status=1
end
