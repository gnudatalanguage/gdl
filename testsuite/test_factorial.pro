;
; AC 17/04/2008
; under GNU/GPL
;
; testing the FACTORIAL() function
;
pro TEST_FACTORIAL, min_val=min_val, max_val=max_val, $
                    input=input, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FACTORIAL, min_val=min_val, max_val=max_val, $'
   print, '                    input=input, help=help'
   return
endif
;
if N_ELEMENTS(input) EQ 0 then input=[1,2,3,4, 26]
;
format_i=STRCOMPRESS('('+STRING(N_ELEMENTS(input))+'i14)',/remove_all)
format_g=STRCOMPRESS('('+STRING(N_ELEMENTS(input))+'g14.8)',/remove_all)
;
print, format=format_i, input
print, format=format_g, FACTORIAL(input)
print, format=format_g, FACTORIAL(input+0.)
print, format=format_i, FACTORIAL(input, /ul64)
print, format=format_i, FACTORIAL(input+0., /ul64)
print, format=format_g, FACTORIAL(input, /ul64, /stirling)
print, format=format_g, FACTORIAL(input+0., /ul64, /stirling)
print, format=format_g, FACTORIAL(input, /stirling)
print, format=format_g, FACTORIAL(input+0., /stirling)
;
if N_ELEMENTS(min_val) EQ 0 then min_val=0
if N_ELEMENTS(max_val) EQ 0 then max_val=10
;
;       12345671234567890ABCD1234567890ABCD1234567890ABCD1234567890AB
print, 'index | def. int    |   /ul64     | float       | /stirling'
for i=min_val, max_val do begin
   print, format='(i6,a2, i16, i16, g14.8,g14.8)', i, ' ',$
          FACTORIAL(i), FACTORIAL(i,/ul), $
          FACTORIAL(FLOAT(i)), FACTORIAL(i,/stirling)
endfor

end
