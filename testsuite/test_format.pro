;
; Alain Coulais
;
; Distributed version 2013/10/24
; Under GNU GPL V2 or later
;
; Purpose: testing various format issues, mostly collected in the bug
; report thread at http://sourceforge.net
;
; no exhaustive list
;
; http://sourceforge.net/p/gnudatalanguage/bugs/110/
;
; --------------------------------------------------------
;
pro TEST_BUG_3244840, verbose=verbose, errors=errors, test=test
;
MESSAGE, /continue, 'running TEST_BUG_3244840'
;
if N_ELEMENTS(errors) EQ 0 then errors=0
;
; creating a temporary file
;
OPENW, u, 'test_bug_3244840.tmp', /get_lun, /delete
PRINTF, u, ''
PRINTF, u, 0., format="(f6.2)"
POINT_LUN, u, 0
;
line = ''
READF, u, line
READF, u, line
FREE_LUN, u
;
if (line NE '  0.00') then begin
   errors++
   MESSAGE, /continue, 'Failure in TEST_BUG_3244840'
   if KEYWORD_SET(verbose) then begin
      print, 'Expected: >>  0.00<<'
      print, 'result:   >>'+line+'<<'
   endif
endif else begin
   MESSAGE, /continue, 'passing with success TEST_BUG_3244840'
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------------
pro WARNING_OLD_IDL_VERSION
;
line='****=============================================****'
if (LONG(STRMID(!VERSION.RELEASE,0,1)) LT 7) then begin
   print, line
   print, 'You are using an old IDL version, format changed between'
   print, 'major versions 6 and 7, we test following >=7 results'
   print, line
endif
end
; --------------------------------------------------------
;
pro TEST_BUG_110, verbose=verbose, errors=errors, test=test
;
MESSAGE, /continue, 'running TEST_BUG_110'
;
if N_ELEMENTS(errors) EQ 0 then errors=0
;
internal_err=0
;
a=1
;
OPENW, u, 'test_bug_110.tmp', /get_lun, /delete
;
PRINTF, u, FORMAT='(I08)', 300 
PRINTF, u, FORMAT='(I8.8)', 300
PRINTF, u, STRING(9, FORMAT='(i03)')  ;; format inside STRING()
PRINTF, u, a, FORMAT="('<',i0,'>')"
;
POINT_LUN, u, 0
line1=''
READF, u, line1
line2=''
READF, u, line2
line3=''
READF, u, line3
line4=''
READF, u, line4
;
if (line1 NE '00000300') then begin
   internal_err++
   MESSAGE, /continue, 'Failure 1 in TEST_BUG_110'
   DEFSYSV, '!gdl', exists=is_it_gdl
   if (~is_it_gdl) then WARNING_OLD_IDL_VERSION
   if KEYWORD_SET(verbose) then begin
      print, 'Expected: >>00003000<<'
      print, 'result:   >>'+line1+'<<'
   endif
endif
;
if (line2 NE '00000300') then begin
   internal_err++
   MESSAGE, /continue, 'Failure 2 in TEST_BUG_110'
   if KEYWORD_SET(verbose) then begin
      print, 'Expected: >>00003000<<'
      print, 'result:   '+line2
   endif
endif
;
if (line3 NE '009') then begin
   internal_err++
   MESSAGE, /continue, 'Failure 3 in TEST_BUG_110'
   DEFSYSV, '!gdl', exists=is_it_gdl
   if (~is_it_gdl) then WARNING_OLD_IDL_VERSION
   if KEYWORD_SET(verbose) then begin
      print, 'Expected: >>009<<'
      print, 'result:   >>'+line3+'<<'
   endif
endif
;
if (line4 NE '<1>') then begin
   internal_err++
   MESSAGE, /continue, 'Failure 4 in TEST_BUG_110'
   if KEYWORD_SET(verbose) then begin
      print, 'Expected: <1>'
      print, 'result:   '+line4
   endif
endif
;
if (internal_err EQ 0) then begin
   MESSAGE, /continue, 'passing with success TEST_BUG_110'
endif
;
errors=errors+internal_err
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------------
;
pro TEST_FORMAT, verbose=verbose, help=help, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   txt='verbose=verbose, help=help, no_exit=no_exit, test=test'
   print, 'pro TEST_FORMAT, '+txt
   return
endif
;
errors=0
;
TEST_BUG_3244840, verbose=verbose, errors=errors
;
TEST_BUG_110, verbose=verbose, errors=errors
;
if ~KEYWORD_SET(no_exit) then begin
   if (errors GT 0) then EXIT, status=1
endif

if KEYWORD_SET(test) then STOP
;
end
