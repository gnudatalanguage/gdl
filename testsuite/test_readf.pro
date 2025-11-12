;
; Maxime Lenoir (contact also: Alain Coulais)
; Distributed version 2010/06/14
; Under GNU GPL V2 or later
;
; Purpose:
;
; -- Checking if we can go back in lines ...
;
; -- Check READF procedure with different end-of-line characters (CR, LF, CRLF)
; @2010/06/14 : issue with lines ended with CR character
;
;
; see also "TEST_BUG_3244840" in "TEST_FORMAT", using 
; unwritten temporary file and POINT_LUN
;
; --------------------------------------------
;
; http://sourceforge.net/p/gnudatalanguage/bugs/573/
; playing with lines skipping
;
pro TEST_BUG_573, verbose=verbose, no_erase=no_erase, errors=errors, test=test
;
MESSAGE, /continue, 'running TEST_BUG_573'
;
if N_ELEMENTS(errors) EQ 0 then errors=0
;
filename='test_bug_573.tmp'
;
OPENW, nlun, filename, /get_lun
PRINTF, nlun, '234.123 231.2 54.3'
PRINTF, nlun, '5432.4 543.'
PRINTF, nlun, '33.4 444.22 3321.'
CLOSE, nlun
FREE_LUN, nlun

;
OPENR, unit, filename, /get_lun
foo=DBLARR(5)
READF,unit, foo
READF,unit, foo2, foo3, foo4
CLOSE, unit
FREE_LUN, unit
;
; expected values
exp2=33.4000
exp3=444.220
exp4=3321.00
;
if ((foo2 NE exp2) OR (foo3 NE exp3) OR (foo4 NE exp4)) then begin
   errors++
   MESSAGE, /continue, 'Failure in TEST_BUG_573'
   if KEYWORD_SET(verbose) then begin
      print, 'Expected: ', foo2, foo3, foo4
      print, 'result  : ', exp2, exp3, exp4
   endif
endif else begin
   MESSAGE, /continue, 'passing with success TEST_BUG_573'
endelse
;
if KEYWORD_SET(test) then STOP
;
if NOT(KEYWORD_SET(no_erase)) then FILE_DELETE,filename
;
end
;
; --------------------------------------------------------
;
pro TEST_READS_STRUCT_2, verbose=verbose, no_erase=no_erase, errors=errors
;
lf=string(10b)
data = replicate({x: 0, y:''}, 2)
inarray=' 1 a          '+lf+' 2             b '+lf
filename="inarray.txt"
openw,fd,filename,/get_lun
printf,fd,inarray
CLOSE, fd
openr,fd,filename
ok=EXECUTE('READF, fd, data')
;
if ~ok then ERRORS_ADD, errors, 'EXECUTE failed !'
if total(data.y eq [' a          ','             b ']) ne 2 then ERRORS_ADD, errors, 'bad values !'
CLOSE, fd
FREE_LUN, fd
if NOT(KEYWORD_SET(no_erase)) then FILE_DELETE,filename
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_READS_STRUCT_2', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------------
;
pro MINIREREADF, filename
;
if N_PARAMS() EQ 0 then begin
    MESSAGE, /continue, 'You mus provide a Filename !'
    return
endif
;
resu=FILE_INFO(filename)
if (resu.exists NE 1) then begin
    MESSAGE, /continue, 'File : '+filename+' not available'
    return
endif
;
line=''
;
OPENR, lun, /get_lun, filename
WHILE ~EOF(lun) do begin
    READF, lun, line
    print, line
endwhile
;
end
;
; --------------------------------------------
;
pro TESTREADF, verbose=verbose, type=type, no_erase=no_erase, errors=errors
;
if KEYWORD_SET(verbose) then begin
    print, '====='
    print, 'test'+type+':'
endif
case type of
    'CR': begin
        char=string(13b)
        suffixe='CR.txt'
    end
    'LF': begin
        char=string(10b)
        suffixe='LF.txt'
    end
    'CRLF': begin
        char=string([13b,10b])
        suffixe='CRLF.txt'
    end
    else: begin
        MESSAGE, /continue, 'type={CR, LF, CRLF}'
        EXIT, status=1
    end
endcase
;
DEFSYSV, '!gdl', exists=is_it_gdl
if (is_it_gdl EQ 1) then soft='GDL' else soft='IDL'
;
filename='TestReadF_'+soft+'_'+suffixe
;
; generating the ASCII file
;
openw,fd,filename, /get_lun
printf,fd,"testl1"+char+"testl2"
close,fd
;
; reading back the generated ASCII file
;
openr, fd, filename
str=''
;
; reading first line
;
READF, fd, str
if (str NE 'testl1') then begin
    MESSAGE, /continue, '% TEST_READF: failed with '+type+' char'
    errors=errors+1
endif
if KEYWORD_SET(verbose) then print, str
if EOF(fd) then begin
    MESSAGE, /continue, '% TEST_READF: failed with '+type+' char'
    errors=errors+1
endif else begin
    ;;
    ;; reading second line
    ;;
    READF, fd, str
    if (str NE 'testl2') then begin
        MESSAGE, /continue, '% TEST_READF: failed with '+type+' char'
        errors=errors+1
    endif
    if KEYWORD_SET(verbose) then print, str
endelse
;
CLOSE, fd
FREE_LUN, fd
;
if NOT(KEYWORD_SET(no_erase)) then FILE_DELETE,filename
;
end
;
; --------------------------------------------
;
pro TEST_READF, verbose=verbose, no_erase=no_erase, help=help, $
                no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_READF, verbose=verbose, no_erase=no_erase, help=help, $'
    print, '                no_exit=no_exit, test=test'
    return
endif
;
if KEYWORD_SET(verbose) then begin
    print, 'Should print (for each pro):'
    print, 'testl1'
    print, 'testl2'
endif
;
errors=0
;
TESTREADF, verbose=verbose, no_erase=no_erase, errors=errors, type='CR'
TESTREADF, verbose=verbose, no_erase=no_erase, errors=errors, type='LF'
TESTREADF, verbose=verbose, no_erase=no_erase, errors=errors, type='CRLF'
;
TEST_BUG_573, verbose=verbose, no_erase=no_erase, errors=errors
TEST_READS_STRUCT_2, verbose=verbose, no_erase=no_erase, errors=errors
;
if ~KEYWORD_SET(no_exit) then begin
   if (errors GT 0) then EXIT, status=1
endif
;
if KEYWORD_SET(test) then STOP
;
end
