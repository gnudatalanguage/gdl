;
; various basic tests on FILE_MOVE()
; written by NATCHKEBIA Ilia, May 2015
; under GNU GPL v2 or any later
;
; -----------------------------------------------
;
pro ADD_ERRORS, nb_errors, message
;
print, 'Error on operation : '+message
nb_errors=nb_errors+1
;
end
;
; -----------------------------------------------
;
pro TEST_FILE_MOVE, test=test, no_exit=no_exit, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FILE_MOVE, test=test, no_exit=no_exit, help=help'
   return
endif
;
;Errors count is 0 at the beginning
total_errors = 0
;
; Create test files in test folder
tfile='toto'
tfile1='totoi'
tfile2='totos'
tfile3='tototo'
tfile4='totogfg'
tfileto='titi'
tfile1to='titii'
tfile2to='titis'
tfile3to='tititi'
tfile4to='titifgf'
SPAWN, 'touch '+tfile+' '+tfile1+' '+tfile2+' '+tfile3+' '+tfile4;

;move files
file_move,tfile,tfileto
file_move,[tfile1,tfile2],[tfile1to,tfile2to]
file_move,"toto*",[tfile3to,tfile4to]
;Test if exists new files and old is removed

if ((FILE_TEST(tfile) eq 1) or (FILE_TEST(tfileto) eq 0)) then ADD_ERRORS, total_errors,   'file_move,source,dest                                 not worked!'
if ((FILE_TEST(tfile1) eq 1) or (FILE_TEST(tfile1to) eq 0)) then ADD_ERRORS, total_errors, 'file_move,[tfile1,tfile2],[tfile1to,tfile2to]         not worked!'
if ((FILE_TEST(tfile2) eq 1) or (FILE_TEST(tfile2to) eq 0)) then ADD_ERRORS, total_errors, 'file_move,[tfile1,tfile2],[tfile1to,tfile2to]         not worked!'
if ((FILE_TEST(tfile3) eq 1) or (FILE_TEST(tfile3to) eq 0)) then ADD_ERRORS, total_errors, 'file_move,"toto*",[tfile3to,tfile4to]                 not worked!'
if ((FILE_TEST(tfile4) eq 1) or (FILE_TEST(tfile4to) eq 0)) then ADD_ERRORS, total_errors, 'file_move,"toto*",[tfile3to,tfile4to]                 not worked!'

SPAWN, 'rm '+tfileto+' '+tfile1to+' '+tfile2to+' '+tfile3to+' '+tfile4to;
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_MOVE', total_errors
;
if KEYWORD_SET(test) then STOP
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
