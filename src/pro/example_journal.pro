; GDL - GNU Data Language, Version 0.8
; Journal File for ...
; working directory...
; Date: Sun Sep 18:44:23 2004
a=findgen(10)
help, a
; FLOAT =  Array[10]
print, a
;      0.000000      1.00000      2.00000      3.00000      4.00000
;       5.00000      6.00000      7.00000      8.00000      9.00000
print, a, format='(f5.3)'
; 0.000
; 1.000
; 2.000
; 3.000
; 4.000
; 5.000
; 6.000
; 7.000
; 8.000
; 9.000
print, 'GNU Data Lanuage'
; GNU Data Lanuage
journal
