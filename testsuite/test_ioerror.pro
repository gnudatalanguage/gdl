;
; By Eloi R. de Linage on 19/07/2021
; 
; Modest suite to test the On_IOError functionality in basic cases
;
; Following a bug found when calling readcol.pro in idlastro, where comment lines
; in an ASCII file were not recognized as non-numerical
;
; Callers' ON_IOERROR handlers must not intercept errors from this routine.
pro IOERROR_SCOPE_INNER, path
OPENR, lun, '/nonexistent_dir_for_gdl_tests/none.txt', /GET_LUN
FREE_LUN, lun
path=path+' inner_continued'
end

pro IOERROR_SCOPE_MIDDLE, path
ON_IOERROR, middle_label
IOERROR_SCOPE_INNER, path
path=path+' middle_continued'
return
middle_label:
path=path+' middle_label'
end

pro IOERROR_SCOPE_CATCH, path
CATCH, error
if error NE 0 then begin
   CATCH, /cancel
   path=path+' caught'
   return
endif
IOERROR_SCOPE_MIDDLE, path
path=path+' top_continued'
end

; In the originating routine, ON_IOERROR still takes precedence over CATCH.
pro IOERROR_SCOPE_LOCAL, path
CATCH, error
if error NE 0 then begin
   CATCH, /cancel
   path=path+' caught'
   return
endif
ON_IOERROR, local_label
OPENR, lun, '/nonexistent_dir_for_gdl_tests/none.txt', /GET_LUN
FREE_LUN, lun
path=path+' continued'
return
local_label:
path=path+' local_label'
end

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
BANNER_FOR_TESTSUITE, 'TEST_IOERROR', errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
