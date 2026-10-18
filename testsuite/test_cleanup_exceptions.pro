; Automatic object cleanup must not throw through C++ destructors.
function CLEANUP_EXCEPTION_OBJECT::INIT, mode, label
self.mode=mode
self.label=label
return, 1
end

pro CLEANUP_EXCEPTION_OBJECT::CLEANUP
if self.mode EQ 1 then begin
   CATCH, error
   if error NE 0 then begin
      CATCH, /cancel
      return
   endif
endif
if self.mode EQ 2 then return
MESSAGE, self.label
end

pro CLEANUP_EXCEPTION_OBJECT__DEFINE
definition={cleanup_exception_object, mode:0, label:''}
end

pro CLEANUP_EXCEPTION_LOCAL
local=OBJ_NEW('cleanup_exception_object', 0, 'local cleanup failed')
end

pro CLEANUP_EXCEPTION_UNWIND
local=OBJ_NEW('cleanup_exception_object', 0, 'secondary cleanup failure')
MESSAGE, 'primary failure'
end

function CLEANUP_EXCEPTION_DROP
local=OBJ_NEW('cleanup_exception_object', 0, 'caller cleanup failure')
return, 1
end

function CLEANUP_EXCEPTION_CALLEE, caught
CATCH, error
if error NE 0 then begin
   CATCH, /cancel
   caught=1
   return, -2
endif
return, 2
end

pro CLEANUP_EXCEPTION_CATCH, caught
CATCH, error
if error NE 0 then begin
   CATCH, /cancel
   caught=1
   return
endif
local=OBJ_NEW('cleanup_exception_object', 0, 'caught cleanup failure')
local=0
end

; Run directly, outside EXECUTE, to check caller CATCH during unwinding.
pro CLEANUP_EXCEPTION_CALLER_CATCH, mode, caught
CATCH, error
if error NE 0 then begin
   CATCH, /cancel
   caught=!ERROR_STATE.MSG
   return
endif
if mode GE 0 then ON_ERROR, mode
CLEANUP_EXCEPTION_UNWIND
caught='ERROR: continued after primary failure'
end

; Used by the interactive RETALL regression in test_cleanup_interactive.pro.
pro CLEANUP_EXCEPTION_PROMPT
local=OBJ_NEW('cleanup_exception_object', 0, 'interactive cleanup failure')
local=0
PRINT, 'ERROR: continued after cleanup failure'
end

pro TEST_CLEANUP_EXCEPTIONS
errors=0
level=SCOPE_LEVEL()
existing_objects=OBJ_VALID(count=objects_before)
commands=["o=OBJ_NEW('cleanup_exception_object', 0, 'cleanup failed') & o=0", $
          'CLEANUP_EXCEPTION_LOCAL']
; These ON_ERROR modes threw across the destructor even before #2265.
for mode=1,3 do begin
   for i=0,N_ELEMENTS(commands)-1 do begin
      after=0
      status=EXECUTE('ON_ERROR, mode & '+commands[i]+' & after=1')
      if status NE 0 then ERRORS_ADD, errors, 'cleanup failure status'
      if after NE 0 then ERRORS_ADD, errors, 'continued after cleanup failure'
      if SCOPE_LEVEL() NE level then ERRORS_ADD, errors, 'cleanup call stack'
      status=EXECUTE('value=42')
      if (status NE 1) OR (value NE 42) then $
         ERRORS_ADD, errors, 'stale cleanup exception'
   endfor
endfor
ON_ERROR, 1

; A later function in the same statement must not catch its caller's error.
caught=0
after=0
status=EXECUTE('value=CLEANUP_EXCEPTION_DROP()+CLEANUP_EXCEPTION_CALLEE(caught) & after=1')
if status NE 0 then ERRORS_ADD, errors, 'caller cleanup failure status'
if caught NE 0 then ERRORS_ADD, errors, 'callee caught caller cleanup error'
if after NE 0 then ERRORS_ADD, errors, 'continued after caller cleanup error'
if STRPOS(!ERROR_STATE.MSG, 'caller cleanup failure') LT 0 then $
   ERRORS_ADD, errors, 'lost caller cleanup exception'

; Nested EXECUTE must not consume the surrounding statement's pending error.
after=0
status=EXECUTE("value=CLEANUP_EXCEPTION_DROP()+EXECUTE('nested_value=42') & after=1")
if (status NE 0) OR (after NE 0) then $
   ERRORS_ADD, errors, 'nested EXECUTE consumed caller cleanup error'
if nested_value NE 42 then ERRORS_ADD, errors, 'nested EXECUTE did not run normally'

; IDL 8.4 frees the object even when explicit CLEANUP fails.
o=OBJ_NEW('cleanup_exception_object', 0, 'explicit cleanup failed')
after=0
status=EXECUTE('OBJ_DESTROY, o & after=1')
if status NE 0 then ERRORS_ADD, errors, 'explicit cleanup failure status'
if after NE 0 then ERRORS_ADD, errors, 'continued after explicit cleanup failure'
if OBJ_VALID(o) then ERRORS_ADD, errors, 'failed cleanup left object valid'
status=EXECUTE('OBJ_DESTROY, o')
if status NE 1 then ERRORS_ADD, errors, 'destroying invalid object reference'

; A later destructor with a CATCH must not consume an earlier failure.
objects=[OBJ_NEW('cleanup_exception_object', 0, 'first cleanup failure'), $
         OBJ_NEW('cleanup_exception_object', 1, 'handled cleanup failure')]
status=EXECUTE('objects=0')
if status NE 0 then ERRORS_ADD, errors, 'multiple object cleanup status'
if STRPOS(!ERROR_STATE.MSG, 'first cleanup failure') LT 0 then $
   ERRORS_ADD, errors, 'lost first cleanup exception'

; Errors handled by CLEANUP itself still count as success.
status=EXECUTE("o=OBJ_NEW('cleanup_exception_object', 1, 'handled') & o=0")
if status NE 1 then ERRORS_ADD, errors, 'handled cleanup error'

caught=0
status=EXECUTE('CLEANUP_EXCEPTION_CATCH, caught')
if (status NE 1) OR (caught NE 1) then $
   ERRORS_ADD, errors, 'CATCH around automatic cleanup'

; Preserve the original exception when destructors run during unwinding.
status=EXECUTE('CLEANUP_EXCEPTION_UNWIND')
if status NE 0 then ERRORS_ADD, errors, 'unwinding status'
if STRPOS(!ERROR_STATE.MSG, 'primary failure') LT 0 then $
   ERRORS_ADD, errors, 'lost primary exception'
status=EXECUTE('value=43')
if (status NE 1) OR (value NE 43) then $
   ERRORS_ADD, errors, 'cleanup error leaked from unwinding'

; A later error in the same statement must not leave a deferred error behind.
o=OBJ_NEW('cleanup_exception_object', 0, 'deferred cleanup failure')
status=EXECUTE('value=(o=0)+undefined_cleanup_test_variable')
if status NE 0 then ERRORS_ADD, errors, 'immediate failure status'
status=EXECUTE('value=44')
if (status NE 1) OR (value NE 44) then $
   ERRORS_ADD, errors, 'cleanup error leaked from failed statement'

; Successful cleanup must not disturb the next statement either.
after=0
status=EXECUTE("o=OBJ_NEW('cleanup_exception_object', 2, '') & o=0 & after=1")
if (status NE 1) OR (after NE 1) then $
   ERRORS_ADD, errors, 'successful automatic cleanup'
remaining_objects=OBJ_VALID(count=objects_after)
if objects_after NE objects_before then ERRORS_ADD, errors, 'object heap leak'

BANNER_FOR_TESTSUITE, 'TEST_CLEANUP_EXCEPTIONS', errors
if errors GT 0 then EXIT, status=1
end
