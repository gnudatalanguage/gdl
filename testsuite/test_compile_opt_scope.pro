;
; Under GNU GPL v2+
;
; Issue #2261: the COMPILE_OPT of a routine compiled from $MAIN$
; must not change the options of $MAIN$, while a COMPILE_OPT given
; at main level must stay in effect for the following commands.
;
; A test routine has its own options and cannot see those of $MAIN$,
; so each check runs its commands in a fresh GDL session (SPAWN),
; then tests DEFINT32 (type of 1) and STRICTARR (a(0)=... assignment).
;
; ---------------------------------------
;
pro TEST_COMPILE_OPT_SCOPE_WRITE, file, lines
;
OPENW, lun, file, /GET_LUN
PRINTF, lun, lines, FORMAT='(A)'
FREE_LUN, lun
;
end
;
; ---------------------------------------
; Run "commands" at $MAIN$ in a new session, then check that $MAIN$
; has DEFINT32 only if kind=3, and STRICTARR only if /strict.
; With /batch, the commands are a batch file, otherwise typed input.
;
pro TEST_COMPILE_OPT_SCOPE_CHECK, name, commands, nb_errors, $
                                  kind=kind, strict=strict, batch=batch
;
if N_ELEMENTS(kind) EQ 0 then kind=2
;
lines=[commands, $
       'if SIZE(1, /TYPE) ne '+STRTRIM(kind, 2)+' then EXIT, STATUS=1', $
       'a=[10,20]']
if KEYWORD_SET(strict) then begin
   lines=[lines, "if EXECUTE('a(0)=30', 1) ne 0 then EXIT, STATUS=2"]
endif else begin
   lines=[lines, 'a(0)=30', 'if a[0] ne 30 then EXIT, STATUS=3']
endelse
lines=[lines, 'a[0]=40', 'if a[0] ne 40 then EXIT, STATUS=4', $
       "print, 'COMPILE_OPT_SCOPE_OK'", 'EXIT, STATUS=0']
TEST_COMPILE_OPT_SCOPE_WRITE, 'opt_commands_2261.pro', lines
;
if KEYWORD_SET(batch) then input=' ' else input=' < '
SPAWN, GET_PATH_TO_EXE()+' -quiet'+input+'opt_commands_2261.pro', $
       out, err, EXIT_STATUS=status
;
log=STRJOIN([out, err], STRING(10b))
bad=(status NE 0) OR (STRPOS(log, 'COMPILE_OPT_SCOPE_OK') LT 0)
foreach txt, ['Execution halted at:', 'Error occurred at:', 'Syntax error', $
              'No parser output', 'Compilation error(s)'] do $
   if STRPOS(log, txt) GE 0 then bad=1
;
if bad then begin
   ERRORS_ADD, nb_errors, name+' (exit status '+STRTRIM(status, 2)+')'
   print, log
endif
;
end
;
; ---------------------------------------
;
pro TEST_COMPILE_OPT_SCOPE, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_COMPILE_OPT_SCOPE, help=help, test=test, no_exit=no_exit'
   return
endif
;
; GET_PATH_TO_EXE() and the input redirection need a Unix shell
if (!version.os_family EQ 'Windows') then begin
   MESSAGE, /continue, 'Not ready for MSwin - skipping'
   EXIT, status=77
endif
;
; a startup file could set main level options
SETENV, 'GDL_STARTUP='
SETENV, 'IDL_STARTUP='
;
; routines with IDL2 report what they see, the last one has no option
pro_src=['pro opt_pro_2261, kind, strict', 'compile_opt IDL2', $
         'kind=SIZE(1, /TYPE)', 'a=[10,20]', $
         "strict=EXECUTE('a(0)=30', 1) eq 0", 'end']
fun_src=['function opt_fun_2261, strict', 'compile_opt IDL2', $
         'a=[10,20]', "strict=EXECUTE('a(0)=30', 1) eq 0", $
         'return, SIZE(1, /TYPE)', 'end']
hidden_src=['pro opt_hidden_2261', 'compile_opt HIDDEN', 'end']
default_src=['pro opt_default_2261, kind', 'a=[10,20]', 'a(0)=30', $
             'kind=SIZE(1, /TYPE)', 'end']
;
files=['opt_pro_2261.pro', 'opt_fun_2261.pro', 'opt_hidden_2261.pro', $
       'opt_multiple_2261.pro', 'opt_main_2261.pro', 'opt_mixed_2261.pro']
TEST_COMPILE_OPT_SCOPE_WRITE, files[0], pro_src
TEST_COMPILE_OPT_SCOPE_WRITE, files[1], fun_src
TEST_COMPILE_OPT_SCOPE_WRITE, files[2], hidden_src
TEST_COMPILE_OPT_SCOPE_WRITE, files[3], [hidden_src, pro_src, fun_src, default_src]
TEST_COMPILE_OPT_SCOPE_WRITE, files[4], ['compile_opt IDL2', $
                                         'main_kind=SIZE(1, /TYPE)', 'end']
TEST_COMPILE_OPT_SCOPE_WRITE, files[5], [pro_src, 'compile_opt DEFINT32', $
                                         'main_kind=SIZE(1, /TYPE)', 'end']
;
pro_checks=['opt_pro_2261, kind, strict', $
            'if kind ne 3 or ~strict then EXIT, STATUS=5']
fun_checks=['kind=opt_fun_2261(strict)', $
            'if kind ne 3 or ~strict then EXIT, STATUS=6']
;
nb_errors=0
;
; routines keep their options and $MAIN$ keeps the default ones
; (at startup, GDL itself compiles DLM_REGISTER, which uses IDL2)
TEST_COMPILE_OPT_SCOPE_CHECK, 'startup', '', nb_errors
TEST_COMPILE_OPT_SCOPE_CHECK, 'batch startup', '', nb_errors, /batch
TEST_COMPILE_OPT_SCOPE_CHECK, 'autoload procedure', pro_checks, nb_errors
TEST_COMPILE_OPT_SCOPE_CHECK, 'autoload function', fun_checks, nb_errors
foreach cmd, ['.compile', '.run'] do begin
   TEST_COMPILE_OPT_SCOPE_CHECK, cmd+' procedure', $
      [cmd+' '+files[0], pro_checks], nb_errors
   TEST_COMPILE_OPT_SCOPE_CHECK, cmd+' function', $
      [cmd+' '+files[1], fun_checks], nb_errors
   TEST_COMPILE_OPT_SCOPE_CHECK, cmd+' multiple routines', $
      [cmd+' '+files[3], pro_checks, fun_checks, $
       'opt_default_2261, kind', 'if kind ne 2 then EXIT, STATUS=7'], nb_errors
   ; without a file name, each typed line is checked before compiling
   TEST_COMPILE_OPT_SCOPE_CHECK, cmd+' typed procedure', $
      [cmd, pro_src, pro_checks], nb_errors
   TEST_COMPILE_OPT_SCOPE_CHECK, cmd+' typed function', $
      [cmd, fun_src, fun_checks], nb_errors
endforeach
TEST_COMPILE_OPT_SCOPE_CHECK, 'RESOLVE_ROUTINE', $
   ["RESOLVE_ROUTINE, 'opt_pro_2261'", pro_checks], nb_errors
if (GDL_IDL_FL() EQ 'GDL') then begin
   code_src=['pro opt_code_2261, kind, strict', pro_src[1:*]]
   TEST_COMPILE_OPT_SCOPE_CHECK, 'COMPILE_CODE', $
      ['COMPILE_CODE, ["'+STRJOIN(code_src, '", "')+'"]', $
       'opt_code_2261, kind, strict', $
       'if kind ne 3 or ~strict then EXIT, STATUS=8'], nb_errors
endif
;
; main level options stay in effect, also after compiling a routine
TEST_COMPILE_OPT_SCOPE_CHECK, 'STRICTARR', 'compile_opt STRICTARR', $
   nb_errors, /strict
TEST_COMPILE_OPT_SCOPE_CHECK, 'DEFINT32', 'compile_opt DEFINT32', $
   nb_errors, kind=3
TEST_COMPILE_OPT_SCOPE_CHECK, 'IDL2', 'compile_opt IDL2', $
   nb_errors, kind=3, /strict
TEST_COMPILE_OPT_SCOPE_CHECK, 'batch IDL2', 'compile_opt IDL2', $
   nb_errors, kind=3, /strict, /batch
TEST_COMPILE_OPT_SCOPE_CHECK, 'EXECUTE IDL2', $
   "if EXECUTE('compile_opt IDL2') ne 1 then EXIT, STATUS=9", $
   nb_errors, kind=3, /strict
TEST_COMPILE_OPT_SCOPE_CHECK, 'IDL2 then .compile', $
   ['compile_opt IDL2', '.compile '+files[2]], nb_errors, kind=3, /strict
;
; a main program sets the main level options (GDL behaviour since
; #1954; not checked against IDL)
TEST_COMPILE_OPT_SCOPE_CHECK, '.run main program', $
   ['.run '+files[4], 'if main_kind ne 3 then EXIT, STATUS=10'], $
   nb_errors, kind=3, /strict
TEST_COMPILE_OPT_SCOPE_CHECK, '.run main program after routine', $
   ['.run '+files[5], 'if main_kind ne 3 then EXIT, STATUS=11'], $
   nb_errors, kind=3
TEST_COMPILE_OPT_SCOPE_CHECK, '.run typed main program', $
   ['.run', 'compile_opt IDL2', 'main_kind=SIZE(1, /TYPE)', 'end', $
    'if main_kind ne 3 then EXIT, STATUS=12'], nb_errors, kind=3, /strict
;
FILE_DELETE, [files, 'opt_commands_2261.pro'], /ALLOW_NONEXISTENT
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_COMPILE_OPT_SCOPE', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
