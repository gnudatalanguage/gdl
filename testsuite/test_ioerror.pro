;
; By Eloi R. de Linage on 19/07/2021
; 
; Modest suite to test the On_IOError functionality in basic cases
;
; Following a bug found when calling readcol.pro in idlastro, where comment lines
; in an ASCII file were not recognized as non-numerical
;
pro TEST_IOERROR

errors = 0

success = 0

On_IOError, L1
res1 = double('some text')
errors_add, errors, 'STRING to DOUBLE conversion did not throw IOError !!!'
L1 : success++

On_IOError, L2
res2 = FIX('some text')
errors_add, errors, 'STRING to INT conversion via FIX() did not throw IOError !!!'
L2 : success++

;; On_IOError, L3
;; READ, 'Enter value: ', value
;; errors_add, errors, 'STRING to INT conversion via FIX() did not throw IOError !!!'
;; L3 : success++

;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FIX', errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end