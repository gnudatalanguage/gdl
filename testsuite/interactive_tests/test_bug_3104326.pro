;
; this bug is now referenced as
; http://sourceforge.net/p/gnudatalanguage/bugs/335/
;
; Sylwester, 28/11/2010: the line number of a missing/unknown
; procedure was not well found/computed.
;
; Alain, 18/09/2013: trying to clarify this test, automatic
; calculation of the expected line number where the code will stop,
; beeing independant of the path as long as the file is in the GDL_PATH
;
; In GDL, as is in September 2013, the full full to the procedure
; is not return when we are in the same directory (TBC)
;
function GETTING_LINE_NUMBER, info
;
info=ROUTINE_INFO('TEST_BUG_3104326',/source)
;
SPAWN, 'grep -n NONEXISTENT '+info.path, out, status
;
idx=STRPOS(out[1], ':')
if (idx GT 0) then begin
    line_number=LONG(STRMID(out[1],0,idx))
endif else begin
    line_number=-1
endelse
return, line_number
;
end
;
pro TEST_BUG_3104326, path2exe=path2exe, idl=idl, gdl=gdl, $
                      verbose=verbose, test=test, no_exit=no_exit, help=help
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_BUG_3104326, path2exe=path2exe, idl=idl, $'
    print, '                      verbose=verbose, test=test, no_exit=no_exit, help=help'
    return
endif
if !version.os_family eq 'Windows' then begin
    spawn,'ps',stdps
    if n_elements(stdps) lt 4 then begin
        message,/continue," Windows' invocation without a shell: fake result: immediate return"
        return
        endif
    message,/continue," Windows' invocation: via shell? going for it"
    endif
;
; since the routine was compiled, we can know where it is !
;
info=ROUTINE_INFO('TEST_BUG_3104326',/source)
;
; the line number where the compiler fails. Should be change if
; lines added to this code !!
line_number=GETTING_LINE_NUMBER(info)
if (line_number LE 0) then begin
    MESSAGE, /continue, 'problem during Line Number estimation'
    if ~KEYWORD_SET(no_exit) then EXIT, status=1 else STOP
endif
;
cmd_suffixe=' -quiet 2>/dev/stdout'
;
; logic: provide path to an exe is first priority
; if not provide, if /IDL selected, verify it else verifying relative GDL
;
if KEYWORD_SET(path2exe) then begin
    gdl_exe=path2exe
    if ~FILE_TEST(gdl_exe) then begin
        print, 'No real file found in the path2exe'
        print, 'please provide a full path with exe for GDL/IDL using keyword PATH2GDL='
        return
    endif
endif else begin
    ;; inside call can be switch to IDL
    if KEYWORD_SET(idl) then begin
        print, 'using IDL for internal call'
        gdl_exe='idl'
        spawn, 'which '+gdl_exe, out
        if ((STRLEN(out) EQ 0) OR ~FILE_TEST(out)) then begin
            print, 'No IDL found in the path'
            print, 'please provide a custom path to IDL (or GDL) using keyword PATH2GDL='
            return
        endif
    endif else begin
        ;; default internal call is GDL
        print, 'using GDL for internal call'
        ;; default (what is happening when we run "make check"): 
        ;; assuming we are in the testsuite/ dir and GDL exe is in ../src/
        gdl_exe='../src/gdl'
        if ~FILE_TEST(gdl_exe) then begin
            print, 'No GDL at expected place, trying in the PATH'
            gdl_exe='gdl'
            spawn, 'which '+gdl_exe, out
            if ((STRLEN(out) EQ 0) OR ~FILE_TEST(out)) then begin
                print, 'No GDL found in the path'
                print, 'please provide a path to GDL using keyword PATH2GDL='
                return
            endif
        endif
    endelse
endelse
;
; we have verified that we know where is the routine" (full path)
; and if we have a real executable (idl or gdl)
;                         ;
command='echo "' $
  + '.compile '+ info.path + STRING(10b) $
  + 'test_bug_3104326_helper' + STRING(10b) + '" | ' + gdl_exe+cmd_suffixe
;
SPAWN, command, out
;
; when lines are too long in IDL, they are cut
old=out
out=old[0]
tmp=''
for ii=1, N_elements(old)-1 do begin &$
    if (STRMID(old[ii],0,1) EQ '%') then begin &$
        out=[out, tmp] &$
        tmp=old[ii] &$
    endif else begin &$
        tmp=tmp+old[ii] &$
    endelse &$
endfor
if (STRLEN(tmp) GT 0) then out=[out, tmp]
out=STRCOMPRESS(out) ; we keep some spaces !
;
if KEYWORD_SET(verbose) then begin
    print, 'line number : ', line_number
    print, 'command :', command
    print, 'raw result  :'
    if N_ELEMENTS(old) GT 1 then print, '>>'+TRANSPOSE(old)+'<<' else print, old
    print, 'result  :'
    if N_ELEMENTS(out) GT 1 then print, '>>'+TRANSPOSE(out)+'<<' else print, out
endif
;
nb_errors=0
;
; We can do now three tests:
; -1- testing that the line number in the line with "% Executation halted"
;    is the good one
;
; -2- testing if the expected path is returned
;
; -3- testing also it is the last last line (antepenultimate line)
;
; "good_line" to be used by tests 1 and 2
good_line_nb=WHERE(STRPOS(out, '% Execution') EQ 0, exist)
; Compared to initial version, we change the test because, in fact,
; the filename should be prefixed by the directory, not ready in GDL
filtre1='TEST_BUG_3104326_HELPER_SUB '+STRCOMPRESS(STRING(line_number),/remove_all)
;
; test -1-
;
if exist EQ 0 then begin
    MESSAGE, /continue, 'Error here before test 1: expression <<% Execution halted>> not found'
    nb_errors++
endif else begin
    if STRPOS(out[good_line_nb], filtre1) eq -1 then begin
        nb_errors++
        MESSAGE, /continue, 'Error during test 1: no line number returned.'
    endif
 endelse

if (nb_errors gt 0) then begin
 nb_errors-- ; we KNOW GDL is in error on this one!
 filtre1='TEST_BUG_3104326_HELPER_SUB' ; otherwise the following tests will always be false.
end
;
; test -2-  NOT READY in GDL
;
; what is the effective path of the routine ?
path=info.PATH
index=STRPOS(path, PATH_SEP(), /reverse_search)
;
if (index GT 0) then begin
    path=STRMID(path, 0, index)
    if (nb_errors eq 0) then filtre2=STRCOMPRESS(STRING(line_number),/remove_all)+' '+path else filtre2=path ; since not having the number would force the test to be false.
    ;;
    if STRPOS(out[good_line_nb], filtre2) eq -1 then begin
        MESSAGE, /continue, 'Error during test 2: full path not well returned'
        nb_errors++
        ;; BEGIN of block to be removed when GDL ready
        DEFSYSV, '!gdl', exist=exist
        if exist then begin
            MESSAGE, /continue, 'To be done in GDL : prefixing function by full path'
            nb_errors--
        endif
        ;;END of block to be removed when GDL ready
    endif
endif
;
; test -3-
;
if STRPOS(out[N_ELEMENTS(out) - 3], filtre1) eq -1 then begin
    MESSAGE, /continue, 'Error during test 3: must be antepenultimate line'
    nb_errors++
endif
;
if (nb_errors GT 0) then begin
    if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif else begin
    MESSAGE, /continue, 'the three tests were passed with success'
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------
;
pro TEST_BUG_3104326_HELPER
;
TEST_BUG_3104326_HELPER_SUB
;
end
;
; ----------------------------------
;
pro TEST_BUG_3104326_HELPER_SUB
;
; this name should remain in majuscules
NONEXISTENT
;
end
