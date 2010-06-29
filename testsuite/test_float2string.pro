;
; Alain Coulais, June 2010
; under GNU GPL 2 or later
;
; few cases where STRING() conversion output
; differ between IDL and GDL
;
pro TEST_FLOAT2STRING

print, 'various Float conversion into strings:'

print, STRING(123123.)
print, STRING(123.)
print, STRING(123.e)
print, STRING(123.e0)
print, STRING(123.e1)
;
print, STRING(123.e, format='(f13.2)')
print, STRING(123.e, format='(e13.3)')
print, STRING(123.e1)
print, STRING(123.e1, format='(e13.3)')
print, STRING(123.e1, format='(e13.4)')
;print, STRING(123.e1, format='()')

print, STRING(123.123e4)
print, STRING(123.123e4, format='(e13.4)')
;
; bug id 2555865
;
print, 'conversion into strings in C style:'
;
resu=string(format='(%"test32T_%dinp.dat")',42)
print, 'Expected: >>test32T_42dinp.dat<<'
print, 'Result:   >>'+resu+'<<'
if (STRLEN(resu) NE 17) then print, 'difference between Exp. and Res.'

end
