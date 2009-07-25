;
; Alain C. and Thibaut M.
; 04 Juillet 2009
;
; Goal: wide and automatic tests of I/O of math functions in GDL
; Do the functions return arrays with good dims ?
;
; Very important remark : we DON'T reproduce the IDL bug
; in case on ([1], [N]) arrays, where IDL returns a [N] array
; where only the first value is OK (bug reproduced on IDL 5.4 to 7.1)
; We do return a [N] array with all values OK
; (in IDL, on contrary, case ([N],[1]) is well processed)
;
; testing the dimensions of Output of Mathematical functions like:
; -- family BESELI, BESELJ, BESELY and BESELK
; -- VOIGT
; -- BETA
; -- IGAMMA
; -- EXPINT
; ....
;
; It must be noticed that NO test on numerical values are done here.
; Please refer to dedicated tests (i.e. "test_voigt.pro")
;
; -----------------------------------------
;
function COMPARE_2SIZE, size1, size2, message, quiet=quiet
;
if (N_ELEMENTS(size1) NE N_ELEMENTS(size2)) then begin
   print, message+' Problem : Effective and expected size for output are different'
   return, 1
endif
;
for ii=0, N_ELEMENTS(size1)-1 do begin
   if (size1[ii] NE size2[ii]) then begin
      print, message+' Problem : Difference at field :'+STRING(ii)
      return, 1
   endif
endfor
;
if NOT(KEYWORD_SET(quiet)) then print, message+' Test OK'
;
return, 0
;
end
;
; -----------------------------------------
;
pro TEST_MathFunctionsDim, function_name, quiet=quiet, $
	test=test, help=help
;
if (N_PARAMS() NE 1) then begin
	print, 'You MUST provide a FUNCTION NAME'
	help=1
endif
;
if KEYWORD_SET(help) then begin
	print, 'pro TEST_MathFunctionsDim, function_name,quiet=quiet, $'
        print, '                           test=test, help=help'
	print, ' '
	print, '"function_name" is a function name (a STRING) like BESELI, VOIGT, ...'
	return
endif
;
if KEYWORD_SET(quiet) then print, 'Processing function :',  function_name
;
error=0
info=STRUPCASE(function_name)+' : Case '
;
message=info+'[2] vs N   : '
x=[1.,2]
y=1
resu=CALL_FUNCTION(function_name, x, y)
error=error+COMPARE_2SIZE(SIZE(x), SIZE(resu), message, quiet=quiet)
;
message=info+'[2] vs [3] : '
x=[1.,2]
y=[1.,2,3]
resu=CALL_FUNCTION(function_name, x, y)
error=error+COMPARE_2SIZE(SIZE(x), SIZE(resu), message, quiet=quiet)
;
message=info+'N vs [3]   : '
x=1.
y=[1.,2,3]
resu=CALL_FUNCTION(function_name, x, y)
error=error+COMPARE_2SIZE(SIZE(y), SIZE(resu), message, quiet=quiet)
;
message=info+'[1] vs [3] : '
x=[1.]
y=[1.,2,3]
resu=CALL_FUNCTION(function_name, x, y)
error=error+COMPARE_2SIZE(SIZE(y), SIZE(resu), message, quiet=quiet)
;
message=info+'[2,3] vs N : '
x=FINDGEN(2,3)+1.
y=1
resu=CALL_FUNCTION(function_name, x, y)
error=error+COMPARE_2SIZE(SIZE(x), SIZE(resu), message, quiet=quiet)
;
message=info+'[2,3] vs [1] : '
x=FINDGEN(2,3)+1.
y=[1.]
resu=CALL_FUNCTION(function_name, x, y)
error=error+COMPARE_2SIZE(SIZE(x), SIZE(resu), message, quiet=quiet)
;
message=info+'[2,3] vs [3] : '
x=INDGEN(2,3)+1.
y=[1.,2,3]
resu=CALL_FUNCTION(function_name, x, y)
error=error+COMPARE_2SIZE(SIZE(y), SIZE(resu), message, quiet=quiet)
;
message=info+'[2,3] vs [2,3] : '
x=FINDGEN(2,3)+1.
y=x
resu=CALL_FUNCTION(function_name, x, y)
error=error+COMPARE_2SIZE(SIZE(x), SIZE(resu), message, quiet=quiet)
;
message=info+'[2,3] vs [1,2,3] : '
x=FINDGEN(2,3)+1.
y=FINDGEN(1,2,3)+1.
resu=CALL_FUNCTION(function_name, x, y)
error=error+COMPARE_2SIZE(SIZE(x), SIZE(resu), message, quiet=quiet)
;
if (error EQ 0) then begin
   print, 'all Dims Tests done with SUCCESS for function : '+function_name
endif else begin
   txt='when processing function : '+function_name
   print, txt+', we have: '+STRING(error)+' ERRORS'
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------
;
pro RUN_ALL_TEST_MathFunctionsDim, quiet=quiet, help=help
;
liste=['BESELI','BESELJ','BESELK','BESELY']
liste=[[liste],'VOIGT','EXPINT','BETA','IGAMMA']
;
if KEYWORD_SET(help) then begin
   print, 'pro RUN_ALL_TEST_MathFunctionsDim, quiet=quiet, help=help'
   print, ''
   print, 'This program will run a test suite'
   print, 'for checking the DIMENSIONS of the OUTPUTS'
   print, 'following the TWO (2) INPUTS for some mathematical functions'
   print, '(no checks on numerical values are done here !)'
   print, 'Current functions under test are: '
   print, TRANSPOSE(liste)
   return
endif
;
for ii=0, N_ELEMENTS(liste)-1 do begin
   TEST_MATHFUNCTIONSDIM, quiet=quiet, liste[ii]
endfor
;
end
