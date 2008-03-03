;
; AC 2008/03/03
;
pro MATRIX_AND_STATUS, matrix, result_only=result_only, type=type
;
if NOT(KEYWORD_SET(result_only)) then begin
   print, 'input Matrix    :'
   print, matrix
endif
print, 'inverted Matrix :'
resultat=INVERT(matrix, status)
print, resultat
print, 'Status : ', status
if KEYWORD_SET(type) then begin
   print, 'Type of input Matrix:    ', SIZE(matrix, /type)
   print, 'Type of inverted Matrix: ', SIZE(resultat, /type)
endif
print, '------------------------------------------'
;
end
;
pro TEST_INVERT_MATRIX
;
print, 'test on STRING'
;
MATRIX_AND_STATUS, ['s']
;
a=[['2','1'],['2','1']]
MATRIX_AND_STATUS, a
;
a=[['2','1'],['2','s']]
MATRIX_AND_STATUS, a
;
;
print, 'test on COMPLEX'
a=[complex(0.,0.)]
MATRIX_AND_STATUS, a
;
a=[complex(0.,1.)]
MATRIX_AND_STATUS, a
;
; different types
;
a=(INDGEN(3,3))^2+1
print, 'INT type'
MATRIX_AND_STATUS, a, /type
;
print, 'LONG type'
MATRIX_AND_STATUS, LONG(a), /result_only, /type
;
print, 'STRING type'
MATRIX_AND_STATUS, STRING(a), /result_only, /type
;
print, 'FLOAT type'
MATRIX_AND_STATUS, FLOAT(a), /result_only, /type
;
print, 'DOUBLE type'
MATRIX_AND_STATUS, DOUBLE(a), /result_only, /type
;
print, 'COMPLEX type'
MATRIX_AND_STATUS, a+complexarr(3,3), /result_only, /type
;
;
end
