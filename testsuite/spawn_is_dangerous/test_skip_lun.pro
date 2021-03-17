;0
;1 AC 29-May-2007
;2 ... line to be printed ...
;3
;4
;5
;6
pro TEST_SKIP_LUN, nb_lines=nb_lines, skiptest=skiptest, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SKIP_LUN, nb_lines=nb_lines, skiptest=skiptest, help=help'
   print, ''
   print, ' use skiptest=1 to skip over the first test ...'
   return
endif
;
file='test_skip_lun.pro'
;
if (N_ELEMENTS(skiptest) EQ 1) then begin
   if skiptest EQ 1 then GOTO, afterOne
   print, 'bad value for skipping test'
endif
;
print, ' --- using SKIP_LUN for counting lines in text file ---'
print, ' -1- giving a fake (big) line number --'
;
print, ' '
print, ' use skiptest=1 to skip over the first test ...'
;
GET_LUN, nlun
OPENR, nlun, file
SKIP_LUN, nlun, 1000, /lines, transfer=transfer
CLOSE, nlun
FREE_LUN, nlun
;
SPAWN, "wc -l test_skip_lun.pro | awk '{print $1}'", resultat
print, 'We count line in file : ', file
print, 'Line number via "wc -l"    : ', FIX(resultat)
print, 'Line number via "skip_lun" : ', transfer
;
afterOne:
print, ' -2- using the /eof --'
;
GET_LUN, nlun
OPENR, nlun, file
SKIP_LUN, nlun, /eof, /lines, transfer=transfer
CLOSE, nlun
FREE_LUN, nlun
;
SPAWN, "wc -l test_skip_lun.pro | awk '{print $1}'", resultat
print, 'We count line in file : ', file
print, 'Line number via "wc -l"    : ', FIX(resultat)
print, 'Line number via "skip_lun" : ', transfer
;;
print, '--- reading back the third line (skipping the 2 first) in this file ---'
;
GET_LUN, nlun
OPENR, nlun, file
PRINT, 'Status of file (0: not open) :', (FSTAT(nlun)).open
;
if (N_ELEMENTS(nb_lines) NE 1) then nb_lines=2
;
SKIP_LUN, nlun, nb_lines, /lines
;
line=''
READF, nlun, line
PRINT, line
CLOSE, nlun
FREE_LUN, nlun
;
PRINT, '--- Testing File pre-opening ---'
PRINT, '-1- the file is pre-opened --'
;
GET_LUN, nlun
OPENR, nlun, file
PRINT, 'Status of file (0: not open) :', (FSTAT(nlun)).open
line=''
READF, nlun, line
PRINT, 'Should worked because the file is yet opened'
SKIP_LUN, nlun, nb_lines, /line
CLOSE, nlun
FREE_LUN, nlun
;
PRINT, '-2- the file is NOT opened --'
GET_LUN, nlun
PRINT, 'Status of file (0: not open) :', (FSTAT(nlun)).open
;
PRINT, 'Should return an error because the file is not opened'
SKIP_LUN, nlun, nb_lines, /line
;
end
