;
; various basic tests on FILE_MOVE()
; written by NATCHKEBIA Ilia, May 2015
; under GNU GPL v2 or any later
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
chkfiles = FILE_SEARCH('titi*',count=nf)
if (nf NE 0) then begin
   print," leftover 'titi*' files in directory.  TEST_FILE_MOVE won't proceed"
   return
endif
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
; replaced SPAWN for Win32 compatibility
tfiles=[tfile,tfile1,tfile2,tfile3,tfile4]
;SPAWN, 'touch '+tfile+' '+tfile1+' '+tfile2+' '+tfile3+' '+tfile4;
GET_LUN, lun
for k=0,4 do BEGIN 
   OPENW, lun, tfiles(k)
   CLOSE, lun
endfor
FREE_LUN, lun

;move files
FILE_MOVE, tfile,tfileto
FILE_MOVE, [tfile1,tfile2], [tfile1to,tfile2to]
;	file_move,"toto*",[tfile3to,tfile4to]		; this is illegal
FILE_MOVE, FILE_SEARCH("toto*"), [tfile3to,tfile4to]
;Test if exists new files and old is removed
;
txt1='FILE_MOVE, '
txt2='  not worked!'
;
if ((FILE_TEST(tfile) eq 1) or (FILE_TEST(tfileto) eq 0)) then $
   ERRORS_ADD, total_errors, txt1+'source, dest'+txt2
if ((FILE_TEST(tfile1) eq 1) or (FILE_TEST(tfile1to) eq 0)) then $
   ERRORS_ADD, total_errors, txt1+'[tfile1,tfile2],[tfile1to,tfile2to]'+txt2
if ((FILE_TEST(tfile2) eq 1) or (FILE_TEST(tfile2to) eq 0)) then $
   ERRORS_ADD, total_errors, txt1+'[tfile1,tfile2],[tfile1to,tfile2to]'+txt2
if ((FILE_TEST(tfile3) eq 1) or (FILE_TEST(tfile3to) eq 0)) then $
   ERRORS_ADD, total_errors, txt1+'"toto*",[tfile3to,tfile4to] '+txt2
if ((FILE_TEST(tfile4) eq 1) or (FILE_TEST(tfile4to) eq 0)) then $
   ERRORS_ADD, total_errors, txt1+'"toto*",[tfile3to,tfile4to]'+txt2
;
if KEYWORD_SET(test) then STOP
;
;
FILE_DELETE, tfileto, tfile1to, tfile2to, tfile3to, tfile4to
;
;SPAWN, 'rm '+tfileto+' '+tfile1to+' '+tfile2to+' '+tfile3to+' '+tfile4to;
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_MOVE', total_errors
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
