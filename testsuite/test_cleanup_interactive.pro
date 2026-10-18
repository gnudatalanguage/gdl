; Run prompt and reset checks in child sessions so RETALL and .RESET_SESSION
; cannot discard the test driver. CTest supplies the executable and timeout.
function CLEANUP_INTERACTIVE_QUOTE, value
if !VERSION.OS_FAMILY EQ 'Windows' then return, '"'+value+'"'
quote="'"
return, quote+STRJOIN(STRSPLIT(value, quote, /EXTRACT, /PRESERVE_NULL), $
                     quote+'"'+quote+'"'+quote)+quote
end

pro CLEANUP_INTERACTIVE_CHECK, context, name, commands, errors, $
                               required=required, forbidden=forbidden
OPENW, lun, context.input, /GET_LUN
PRINTF, lun, commands, FORMAT='(A)'
FREE_LUN, lun
command=context.command+' < '+CLEANUP_INTERACTIVE_QUOTE(context.input)+' 2>&1'
if !VERSION.OS_FAMILY EQ 'Windows' then begin
   SPAWN, command, output, EXIT_STATUS=status
endif else begin
   ; Keep the shell alive to translate a child's signal into an exit status.
   SPAWN, command+'; exit $?', output, /SH, EXIT_STATUS=status
endelse
log=STRJOIN(output, STRING(10b))
bad=status NE 0
for i=0,N_ELEMENTS(required)-1 do $
   if STRPOS(log, required[i]) LT 0 then bad=1
for i=0,N_ELEMENTS(forbidden)-1 do $
   if STRPOS(log, forbidden[i]) GE 0 then bad=1
if bad then begin
   ERRORS_ADD, errors, name+' (exit status '+STRTRIM(status, 2)+')'
   PRINT, log
endif
end

pro TEST_CLEANUP_INTERACTIVE
errors=0
input=''
CATCH, error
if error NE 0 then begin
   CATCH, /CANCEL
   ERRORS_ADD, errors, 'test setup/execution: '+!ERROR_STATE.MSG
   GOTO, cleanup
endif
executable=GETENV('GDL_TEST_EXECUTABLE')
if executable EQ '' then executable=GET_PATH_TO_EXE()
command=CLEANUP_INTERACTIVE_QUOTE(executable)+' -quiet'
if !VERSION.OS_FAMILY EQ 'Windows' then begin
   command='set "GDL_STARTUP=" & set "IDL_STARTUP=" & '+command
endif else begin
   command='GDL_STARTUP= IDL_STARTUP= '+command
endelse
stamp=STRTRIM(STRING(SYSTIME(1), FORMAT='(F24.6)'), 2)
input=FILEPATH('gdl-cleanup-session-'+stamp+'.pro', /TMP)
context={command:command, input:input}

foreach command, [ $
   "o=OBJ_NEW('cleanup_exception_object', 0, 'interactive cleanup failure') & o=0", $
   'CLEANUP_EXCEPTION_PROMPT', 'CLEANUP_EXCEPTION_LOCAL'] do begin
   ; RETALL is read at the prompt where the error stopped inside CLEANUP.
   CLEANUP_INTERACTIVE_CHECK, context, command, [ $
      "RESOLVE_ROUTINE, 'test_cleanup_exceptions'", $
      command, $
      'RETALL', $
      'if SCOPE_LEVEL() NE 1 then EXIT, status=1', $
      'if EXECUTE(''MESSAGE, "expected error after RETALL"'') NE 0 then EXIT, status=3', $
      "if EXECUTE('value=42') NE 1 then EXIT, status=2", $
      "PRINT, 'CLEANUP_RETALL_OK'", $
      'EXIT'], errors, $
      REQUIRED=['CLEANUP_RETALL_OK', $
                'Execution halted at: CLEANUP_EXCEPTION_OBJECT::CLEANUP'], $
      FORBIDDEN=['ERROR: continued']
endforeach

; Do not wrap these calls in EXECUTE: it would mask caller-CATCH bugs.
for mode=-1,3 do begin
   CLEANUP_INTERACTIVE_CHECK, context, 'caller CATCH, ON_ERROR='+STRTRIM(mode, 2), [ $
      "RESOLVE_ROUTINE, 'test_cleanup_exceptions'", $
      "caught=''", $
      'CLEANUP_EXCEPTION_CALLER_CATCH, '+STRTRIM(mode, 2)+', caught', $
      "if STRPOS(caught, 'primary failure') LT 0 then EXIT, status=1", $
      'if SCOPE_LEVEL() NE 1 then EXIT, status=2', $
      'caught=0', $
      'CLEANUP_EXCEPTION_CATCH, caught', $
      'if caught NE 1 then EXIT, status=3', $
      "PRINT, 'CLEANUP_CALLER_CATCH_OK'", $
      'EXIT'], errors, REQUIRED=['secondary cleanup failure', 'CLEANUP_CALLER_CATCH_OK'], $
      FORBIDDEN=['Execution halted at:', 'Error occurred at:']
endfor

; Outside EXECUTE: only the originating routine can use ON_IOERROR.
CLEANUP_INTERACTIVE_CHECK, context, 'ON_IOERROR scope outside EXECUTE', [ $
   "RESOLVE_ROUTINE, 'test_ioerror'", $
   "path=''", $
   'IOERROR_SCOPE_CATCH, path', $
   "if path NE ' caught' then EXIT, status=1", $
   'if SCOPE_LEVEL() NE 1 then EXIT, status=2', $
   "path=''", $
   'IOERROR_SCOPE_LOCAL, path', $
   "if path NE ' local_label' then EXIT, status=3", $
   "PRINT, 'IOERROR_SCOPE_OK'", $
   'EXIT'], errors, REQUIRED=['IOERROR_SCOPE_OK'], $
   FORBIDDEN=['Execution halted at:', 'Error occurred at:']

CLEANUP_INTERACTIVE_CHECK, context, 'cleanup during unwinding', [ $
   "RESOLVE_ROUTINE, 'test_cleanup_exceptions'", $
   'ON_ERROR, 1', $
   "if EXECUTE('CLEANUP_EXCEPTION_UNWIND') NE 0 then EXIT, status=1", $
   "if STRPOS(!ERROR_STATE.MSG, 'primary failure') LT 0 then EXIT, status=2", $
   "if EXECUTE('value=42') NE 1 then EXIT, status=3", $
   "PRINT, 'CLEANUP_UNWIND_OK'", $
   'EXIT'], errors, REQUIRED=['secondary cleanup failure', 'CLEANUP_UNWIND_OK']

CLEANUP_INTERACTIVE_CHECK, context, 'multiple deferred cleanup failures', [ $
   "RESOLVE_ROUTINE, 'test_cleanup_exceptions'", $
   'ON_ERROR, 1', $
   "objects=[OBJ_NEW('cleanup_exception_object', 0, 'first deferred cleanup failure'), "+ $
      "OBJ_NEW('cleanup_exception_object', 0, 'second deferred cleanup failure')]", $
   "if EXECUTE('objects=0') NE 0 then EXIT, status=1", $
   "if STRPOS(!ERROR_STATE.MSG, 'first deferred cleanup failure') LT 0 then EXIT, status=2", $
   "if EXECUTE('value=42') NE 1 then EXIT, status=3", $
   "PRINT, 'CLEANUP_MULTIPLE_OK'", $
   'EXIT'], errors, REQUIRED=['second deferred cleanup failure', 'CLEANUP_MULTIPLE_OK']

foreach reset, ['.RESET_SESSION', '.FULL_RESET_SESSION'] do begin
   foreach mode, [0, 2] do begin ; failing and successful CLEANUP
      for on_error=0,1 do begin
         name=reset+', mode='+STRTRIM(mode, 2)+', ON_ERROR='+STRTRIM(on_error, 2)
         required=['CLEANUP_RESET_OK']
         if mode EQ 0 then required=[required, 'reset cleanup failure']
         CLEANUP_INTERACTIVE_CHECK, context, name, [ $
            "RESOLVE_ROUTINE, 'test_cleanup_exceptions'", $
            'reset_sentinel=42', $
            "reset_object=OBJ_NEW('cleanup_exception_object', "+STRTRIM(mode, 2)+ $
               ", 'reset cleanup failure')", $
            'ON_ERROR, '+STRTRIM(on_error, 2), $
            reset, $
            'RETALL', $
            'if N_ELEMENTS(reset_sentinel) NE 0 then EXIT, status=1', $
            'if N_ELEMENTS(reset_object) NE 0 then EXIT, status=2', $
            'if SCOPE_LEVEL() NE 1 then EXIT, status=3', $
            "if EXECUTE('value=42') NE 1 then EXIT, status=4", $
            "PRINT, 'CLEANUP_RESET_OK'", $
            'EXIT'], errors, REQUIRED=required
      endfor
   endforeach
endforeach

cleanup:
CATCH, /CANCEL
if input NE '' then begin
   if FILE_TEST(input) then FILE_DELETE, input, /NOEXPAND_PATH
endif
BANNER_FOR_TESTSUITE, 'TEST_CLEANUP_INTERACTIVE', errors
if errors GT 0 then EXIT, status=1
end
