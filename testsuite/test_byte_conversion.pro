;
; AC 12 Mars 2008
; testing the BYTE conversion for different types
;
pro test_byte_conversion, negative=negative, help=help
;
if KEYWORD_SET(help) then begin
  print, 'pro test_byte_conversion, negative=negative, help=help'
  return
endif

array=[255,256., 257, 258]

if KEYWORD_SET(negative) then sign=-1 else sign=1

print, 'imput data : ', array

for ii=1,4 do begin
   print, 'power ', ii
   print, '(integer 2 byte) ', byte(sign*(fix(array)^ii))
   print, '(  float 2 byte) ', byte(sign*(array^ii))
   print, '( double 2 byte) ', byte(sign*(double(array)^ii))
   print, '(complex 2 byte) ', byte(sign*(complex(array)^ii))
   print, '(   long 2 byte) ', byte(sign*(long(array)^4))
   print, '(  ulong 2 byte) ', byte(sign*(ulong(array)^4))
endfor
end
