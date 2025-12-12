;
; some tests for STRPLIT
;
; Lea Noreskal, June 2010, under GNU GPL V2 or later
;
; -------------------------------------------
; Modifications history :
;
; Alain Coulais, October 2010: 
; -- few extra cases. Also adding tests on count= and lenght= keywords
;
; Alain Coulais, August 2011: 
; -- inaccurate tests found, corrected (ARRAY_EQUAL(a,b,/NO_TYPECONC))
; -- change from /quiet to /verbose
; -- no exit on error if /debug or /test (suppose to be interactive ;-)
; -- adding test for bug 3286746 (STR_SEP)
; -- adding basic test for TexToIDL
;
; A bug 554 about "length" was corrected in July 2013.
;
; 2025-Dec-07 : AC :
; -- include test_bug_3086851.pro
; -- I realize that most tests are on single element ...
; -- clean-up & preparing extension
;
; --------------------------------------------------
pro IPRINT, texte, indice
print, texte, indice
indice=indice+1
end
;
; --------------------------------------------------
; https://sourceforge.net/tracker/?func=detail&atid=618683&aid=3086851&group_id=97659
;
pro TEST_BUG_3086851, cumul_errors, test=test
;
errors=0
;
res=STRSPLIT('a;:c', '[:;]', /extract, /regex, /preserve_null)
if N_ELEMENTS(res) ne 3 then ERRORS_ADD, errors, 'case 3086851 :('
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_BUG_3086851", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------
;
pro INIT_STRARRAY, str,str1,str2,str3,str4,str5,str6,str7,strarray
;
str='$$$$$$$$$$' ; LONG 0
str1='$chops &up str*ings.' ; [1]
str2='ch$ops &up str*ings.' ; [0,3]
str3='$ch$ops &up str*ings.' ; [1,4]
str4='../foo.txt' ; [2,7]
str5=' t e s t '
str6='qwerty'
str7='aa;aaaaaaaaa;aaaaaaaaaa'  ; cf bug 554
strarray=[str,str1,str2,str3,str4,str5,str6,str7]
;
end
;
; --------------------------------------------------
;
pro TEST_STRSPLIT_SINGLE, cumul_errors, verbose=verbose, test=test, debug=debug
;
errors=0
indice=0
;
INIT_STRARRAY, str,str1,str2,str3,str4,str5,str6,str7,strarray
;
; When Search Pattern is not in Input String (default Search pattern
; is white space ' ')
;
tab=STRSPLIT(str6)
if (tab ne 0) then ERRORS_ADD, errors, 'error str basic 0'
;
tab=STRSPLIT(str6,/extract)
if (tab ne str6) then ERRORS_ADD, errors, 'error str basic 0 bis'
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice
;
; When Search Pattern is a stupid void input ''
;
tab=STRSPLIT(str5,'')
if ((SIZE(tab,/type) NE 3) and (tab ne 0)) then $
   ERRORS_ADD, errors, 'error str basic 1'
;
tab=STRSPLIT(str5,'',/extract)
if (tab ne '') then ERRORS_ADD, errors, 'error str basic 1 bis'
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice
;
; When Search Pattern is not in Input String
;
tab=STRSPLIT(str5,'a')
if ((SIZE(tab,/type) NE 3) and (tab ne 0)) then $
   ERRORS_ADD, errors, 'error str basic 2'
;
tab=STRSPLIT(str5,'a',/extract)
if (tab ne str5) then ERRORS_ADD, errors, 'error str basic 2 bis'
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice
;
tab=STRSPLIT(str,'$')
if ((SIZE(tab,/type) NE 3) and (tab ne 0)) then ERRORS_ADD, errors, 'error str'
;
ext=STRSPLIT(str,'$',/ext)
;print , str , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)

if(N_ELEMENTS(ext) ne 1) then ERRORS_ADD, errors, 'error str extract'
;
tab=STRSPLIT(str,'$',/preserve)
res=LINDGEN(11)
if (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0) then begin
   ERRORS_ADD, errors, 'error str preserve_null'
endif
;
ext=STRSPLIT(str,'$',/ext,/preserve)
;print , str , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if (N_ELEMENTS(ext) ne 11) then begin
   ERRORS_ADD, errors, 'error str extract'
endif

ext=STRSPLIT(str,'$',/preserve,length=length)
if ARRAY_EQUAL(length,replicate(0,STRLEN(str)+1)) eq 0 then begin 
   ERRORS_ADD, errors, 'error str len+preserve'
endif
;
ext=STRSPLIT(str,'$',length=length)
if (length ne 0) then begin 
   ERRORS_ADD, errors, 'error str len'
endif 
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice
;
tab=STRSPLIT(str1,'$')
res=[1L]
if (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0) then begin
   ERRORS_ADD, errors, 'error str1'
endif
;
ext=STRSPLIT(str1,'$',/ext)
;print , str1 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
;
res=['chops &up str*ings.']
if( (N_ELEMENTS(ext) ne 1) AND (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0 )) then begin
   ERRORS_ADD, errors, 'error str extract'
endif

ext=STRSPLIT(str1,'$',/ext,/preserve)
;print , str1 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if((N_ELEMENTS(ext) ne 2)) then begin
   ERRORS_ADD, errors, 'error str1 extract preserves'
endif
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice
;
tab=STRSPLIT(str2,'$')
res=LONG([0,3])
if (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0) then begin
   ERRORS_ADD, errors, 'error str2'
endif
;
ext=STRSPLIT(str2,'$',/ext)
;print , str2 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
;
res=['ch' , 'ops &up str*ings.' ]
if ((N_ELEMENTS(ext) ne 2) AND (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0 )) then begin
   ERRORS_ADD, errors, 'error str extract'
endif
;
ext=STRSPLIT(str2,'$',/ext,/preserve)
;print , str2 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if ((N_ELEMENTS(ext) ne 2)) then begin
   ERRORS_ADD, errors, 'error str2 extract preserve'
endif
;
tab=STRSPLIT(str2,'$',/preserve)
res=LONG([0,3])
if (ARRAY_EQUAL(tab,res,/NO_TYPECONV) eq 0) then begin
   ERRORS_ADD, errors, 'error str2 preserve_null'
endif
;
tab=STRSPLIT(str3,'$')
res=LONG([1,4])
if (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0) then begin
   ERRORS_ADD, errors, 'error str3'
endif
;
ext=STRSPLIT(str3,'$',/ext)
;print , str3 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)

res=['ch' , 'ops &up str*ings.' ]
if ((N_ELEMENTS(ext) ne 2) AND (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0 )) then begin
   ERRORS_ADD, errors, 'error str extract'
endif
;
ext=STRSPLIT(str3,'$',/ext,/preserve)
;print , str3 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if ((N_ELEMENTS(ext) ne 3)) then begin
   ERRORS_ADD, errors, 'error str3 extract preserve'
   errors=errors+1
endif
;
if KEYWORD_SET(debug) then IPRINT, 'ici', indice
;
tab=STRSPLIT(str4,'.')
res=LONG([2,7])
if (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0) then begin
   ERRORS_ADD, errors, 'error str4'
endif

tab=STRSPLIT(str4,'.',/preserve)
res=LONG([0,1,2,7])
if (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0) then begin
   ERRORS_ADD, errors, 'error str4 preserve_null'
endif

ext=STRSPLIT(str4,'.',/ext,/preserve)
;print , str4 , ' Fext> ' , strjoin(ext,'-') , N_ELEMENTS(ext)
if ((N_ELEMENTS(ext) ne 4)) then begin
   ERRORS_ADD, errors, 'error str4 extract , preserve'
endif
tab=STRSPLIT(str4,'.',length=length)
if ARRAY_EQUAL(length,[4,3]) eq 0 then begin
   ERRORS_ADD, errors, 'error str4 length'
endif
tab=STRSPLIT(str4,'.',length=length,/preserve)
if ARRAY_EQUAL(length,[0,0,4,3]) eq 0 then begin
   ERRORS_ADD, errors, 'error str4 length+preserve'
endif
;
; few tests on str7
;
pos=STRSPLIT(str7,';',length=length)
if ARRAY_EQUAL(length,[2,9,10]) eq 0 then begin
   ERRORS_ADD, errors, 'error str7 length'
endif
if ARRAY_EQUAL(pos,[0,3,13]) eq 0 then begin
   ERRORS_ADD, errors, 'error str7 pos'
endif
;
pos=STRSPLIT(str7,'.',length=length)
if ARRAY_EQUAL(length,23) eq 0 then begin
   ERRORS_ADD, errors, 'error str7 length no sep'
endif
if (pos NE 0) then begin
   ERRORS_ADD, errors, 'error str7 pos no sep'
endif
;
pos=STRSPLIT(str7,'a',length=length)
if ARRAY_EQUAL(length,[1,1]) eq 0 then begin
   ERRORS_ADD, errors, 'error str7 length <<a>>'
endif
if ARRAY_EQUAL(pos,[2,12]) eq 0 then begin
   ERRORS_ADD, errors, 'error str7 pos <<a>>'
endif
;
pos=STRSPLIT(str7,'a',length=length,/preserve)
exp_pos=LONG([INDGEN(3), INDGEN(9)+4,INDGEN(10)+14])
exp_len=LONARR(STRLEN(str7)-1)
exp_len[2]=1
exp_len[2+9]=1
;
if ARRAY_EQUAL(pos,exp_pos) eq 0 then begin
   ERRORS_ADD, errors, 'error str7 pos <<a>> /preserve'
endif
if ARRAY_EQUAL(length,exp_len) eq 0 then begin
   ERRORS_ADD, errors, 'error str7 length <<a>> /preserve'
endif
;
; tests on str5
;
tab=STRSPLIT(str5,/preserve)
res=LONG([0,1,3,5,7,9])
if (ARRAY_EQUAL(tab,res, /NO_TYPECONV) eq 0) then begin
   ERRORS_ADD, errors, 'error str5 preserve_null'
endif

ext=STRSPLIT(str5,/preserve,/extract)
if((N_ELEMENTS(ext) ne 6)) then begin
   ERRORS_ADD, errors, 'error str5 extract , preserve'
endif
;
; we must return a STRARR except for ''
;
tab=STRSPLIT('eeee','e',/extract)
if (SIZE(tab, /n_dim) NE 0) then begin
   ERRORS_ADD, errors, 'bad size for void string'
endif
tab=STRSPLIT('eeABCee','e',/extract)
if (SIZE(tab, /n_dim) NE 1) then begin
   ERRORS_ADD, errors, 'bad size for 1D STRARR'
endif
if (tab NE 'ABC') then begin
   ERRORS_ADD, errors, 'bad value in STRARR'
endif
tab=STRSPLIT('eeABCeeABCee','e',/extract)
if (SIZE(tab, /n_dim) NE 1) then begin
   ERRORS_ADD, errors, 'bad size for 2 elements STRARR'
endif
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_STRSPLIT_SINGLE", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------
;
pro TEST_STRSPLIT_TEXTOIDL, cumul_errors, verbose=verbose, test=test, debug=debug
;
errors=0
;
;textoidl uses strstrans which uses strsplit
vide=''
if (EXECUTE('res=TEXTOIDL(vide)') EQ 0) then begin
   MESSAGE, /CONTINUE, "Missing TexToIDL in your GDL_PATH or IDL_PATH"
   return
endif else begin
   res='!7l!X!U2!N'
   tab=TEXTOIDL('\mu^2')
   if (res NE tab) then ERRORS_ADD, errors, 'error when using TexToIDL'
endelse
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_STRSPLIT_TEXTOIDL", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------
;
; AC 2025-Dec-08 : why so few tests on array ?
;
pro TEST_STRSPLIT_ARRAY, cumul_errors, verbose=verbose, test=test, debug=debug
;
errors=0
;
INIT_STRARRAY, str,str1,str2,str3,str4,str5,str6,str7,strarray
;
;new version: supports arrays:
res=STRSPLIT(strarray, COUNT=c, LENGTH=l)
;
expect_c=[1L, 3, 3, 3, 1, 4, 1, 1]
if ~ARRAY_EQUAL(c,expect_c) then ERRORS_ADD, errors, 'bad size/val for count'
;
expect_res=LIST([0],[0, 7, 11] , [0, 7, 11], [0, 8, 12], $
                [0], [1, 3, 5, 7], [0], [0])
if N_ELEMENTS(res) NE N_ELEMENTS(expect_res) then ERRORS_ADD, errors, 'pb N_ele res'
for ii=0, N_ELEMENTS(res)-1 do begin
   if ~ARRAY_EQUAL(res[ii],expect_res[ii]) then $
      ERRORS_ADD, errors, 'pb >>res<< at '+string(ii)
endfor
;
expect_l=LIST([10],[6, 3, 9],[6, 3, 9],[7, 3, 9], $
          [10],[1, 1, 1, 1],[6],[23])
if N_ELEMENTS(l) NE N_ELEMENTS(expect_l) then ERRORS_ADD, errors, 'pb N_ele l'
for ii=0, N_ELEMENTS(l)-1 do begin
   if ~ARRAY_EQUAL(l[ii],expect_l[ii]) then $
      ERRORS_ADD, errors, 'pb >>l<< at '+string(ii)
endfor

;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_STRSPLIT_ARRAY", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------
;
pro TEST_STRSPLIT, no_exit=no_exit, verbose=verbose, test=test, $
                   debug=debug, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_STRSPLIT, no_exit=no_exit, verbose=verbose, test=test, $'
   print, '                   debug=debug, help=help'
   return
endif
;
cumul_errors=0
;
TEST_BUG_3086851, cumul_errors, test=test
TEST_STRSPLIT_SINGLE, cumul_errors, verbose=verbose, test=test, debug=debug
TEST_STRSPLIT_TEXTOIDL, cumul_errors, verbose=verbose, test=test, debug=debug
TEST_STRSPLIT_ARRAY, cumul_errors, verbose=verbose, test=test, debug=debug
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_STRSPLIT', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end




