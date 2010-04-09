;
; Alain C, 31 july 2008
;
; After bugs founded by Nicolas with negatives Real/Double
; and (D)Complex with type in (12,13,15) we do a
; systematic check for potential FIX() problems
;
; Please remember type ULONG64 does not exist on x86
;
; Plateforms dependances:
; --linux IDL 7.0 x86_64 7 tests, 0 errors on 4 cases
; --linux IDL 7.0 x86    6 tests, 0 errors on 3 cases, 2 errors on
; last (INT and BYTE)
; --linux IDL 5.5 x86    6 tests, 0 errors on 4 cases
; --linux IDL 6.0 x86    6 tests, 0 errors on 3 cases, 2 errors on last (INT and BYTE)
;
pro TEST_FIX_UNSIGNED, value, type, count=count, quiet=quiet
;
resu=FIX(value, type=type)-FIX(ROUND(value), type=type)
if resu NE 0 then begin
   if NOT(KEYWORD_SET(quiet)) then begin
      print, 'Error for Value : ', value
      HELP, value
      HELP, FIX(value, type=type)
      HELP, FIX(ROUND(value), type=type)
   endif
   if (N_ELEMENTS(count) EQ 1) then count=count+1
endif
end
;
function TEST_FIX_ONE_VALUE, value=value, verbose=verbose, all_types=all_types
;
if KEYWORD_SET(verbose) then quiet=0 else quiet=1

; type  7: string
; type  8: structure
; type 10: pointer
; type 11: object
liste_types=[1,2,3,12,13,14]
;
if (!version.arch EQ 'x86_64') OR KEYWORD_SET(all_types) then begin
   liste_types=[liste_types,15]
endif
;
nb_types=N_ELEMENTS(liste_types)
;
if N_ELEMENTS(value) EQ 0 then x=10.1 else x=value

if (x LT 0.) then begin
   print, 'Positive input mandatory !'
   return, 0
endif
;
separateur= '---------------------------'
print, separateur
print, 'Tested value : ', x
;
;  --------- positive case -----------------
;
print, separateur
print, 'Positive input'
;
count=0
for ii=0, nb_types-1 do begin
   TEST_FIX_UNSIGNED, x, liste_types[ii], count=count, quiet=quiet
end

print, 'Number of tests done :', nb_types
if count EQ 0 then begin
   print, 'No errors founded'
endif else begin
   print, 'Number of errors', count   
endelse
errors_pos=count
;
;  --------- negative case -----------------
;
print, separateur
print, 'Negative input'
;
count=0
for ii=0, nb_types-1 do begin
   TEST_FIX_UNSIGNED, (-1.0)*x, liste_types[ii], count=count, quiet=quiet
end
;
print, 'Number of tests done :', nb_types
if count EQ 0 then begin
   print, 'No errors founded'
endif else begin
   print, 'Number of errors', count
endelse

errors_neg=count
if (errors_pos GT 0) OR (errors_neg GT 0) then begin
   print, separateur
   print, 'Bad news ! At least one problem encoutered !'
   print, 'Please rerun this test with keyword /verbose !'
   return, 0
endif

return, 1

end


pro TEST_FIX, verbose=verbose, all_types=all_types
  ok = 1
  ok and= TEST_FIX_ONE_VALUE(value=1.001, verbose=verbose, all_types=all_types) 
  ok and= TEST_FIX_ONE_VALUE(value=10.1, verbose=verbose, all_types=all_types) 
  ok and= TEST_FIX_ONE_VALUE(value=1000.1, verbose=verbose, all_types=all_types) 
  ok and= TEST_FIX_ONE_VALUE(value=1000.1e5, verbose=verbose, all_types=all_types)
  if ~ok then exit, status=1
end
