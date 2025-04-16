;
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; ---------------------------------
; 
; Modifications history :
;
; - 2025-04-15 : AC. using cumulative error count !
;
; ---------------------------------
;;
pro TEST_FIXPRINT, help=help, no_exit=no_exit, test=test
; 
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FIXPRINT, help=help, no_exit=no_exit, test=test'
   return
endif
;
errors=0
;
if ~EXECUTE('a=FIX(0,type=0)') then begin
   ERRORS_ADD, errors, 'FIX(type=0) failed!'
endif
;
if (FIX('123',/print,type=1))[0] ne 123 then begin
   ERRORS_ADD, errors, 'FIX(string,/print,type=byte) failed!'
endif
;
if (FIX(123,/print,type=1))[0] ne 123 then begin
   ERRORS_ADD, errors, 'FIX(int,/print,type=byte) failed!'
endif
;
if (FIX('123',type=1))[0] ne 49 then begin
   ERRORS_ADD, errors, 'FIX(string,type=byte) failed!'
endif
;
if FIX(123b,/print,type=7) ne ' 123' then begin
   ERRORS_ADD, errors, 'FIX(byte,/print,type=string) failed!'
endif
;
if FIX(123b,type=7) ne '{' then begin
   ERRORS_ADD, errors, 'FIX(byte,type=string) failed!'
endif
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FIXPRINT', errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
