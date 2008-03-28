;
; AC 2008/03/03, improved on 2008/03/28
;
; Testing the INVERT() function
;
; What is missing in this testsuite ?
; What append when input is not square matrix !
;
; pro BENCH_INVERT_MATRIX
; - inversion of randomu matrix of different sizes. The /Save keyword
;   will allow you to save the results and intercompare on different
;   computers and with IDL
;
; pro MATRIX_AND_STATUS
; - just doying the inversion with a standardized method 
;   to print results and informations
;
; pro TEST_INVERT_ONE_ELEMENT :
; - testing than INVERT() survive to calls with only one element
;
; pro TEST_INVERT_WITH_STRING
; - testing than INVERT() survive to calls with strings values
;
; pro TEST_INVERT_MATRIX
; - testing than INVERT() does manage well all types (int, float,
;   string, complexe ...)
;
pro BENCH_INVERT_MATRIX, nb_matrix=nb_matrix, step=step, test=test, $
                         start_size=start_size, save=save, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro BENCH_INVERT_MATRIX, nb_matrix=nb_matrix, step=step, test=test, $'
   print, '              start_size=start_size, save=save, help=help'  
endif
;
if (N_ELEMENTS(nb_matrix) EQ 0) then nb_matrix=5
if (N_ELEMENTS(start_size) EQ 0) then start_size=100
if (N_ELEMENTS(step) EQ 0) then step=100
;
resu_gen=FLTARR(nb_matrix)
resu_inv=FLTARR(nb_matrix)
;
for ii=0, nb_matrix-1 do begin
   nbp=start_size+ii*step
   print, '---------------------------------'
   print, 'Matrix size : ', nbp
   time0=SYSTIME(1)
   a=RANDOMU(seed, nbp, nbp)
   time1=SYSTIME(1)
   print, 'Generation time: ', time1-time0
   b=INVERT(a, status)
   print, 'status :', status
   time2=SYSTIME(1)
   print, 'Inversion time: ', time2-time1
   resu_gen[ii]=time1-time0
   resu_inv[ii]=time2-time1
endfor
;
plot, resu_inv
oplot, resu_gen
;
if KEYWORD_SET(save) then begin
   SAVE, nb_matrix, start_size, step, resu_gen, resu_inv
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
pro MATRIX_AND_STATUS, matrix, result_only=result_only, $
                       type=type, real_part=real_part
;
if (N_ELEMENTS(matrix) EQ 0) then begin
   print, 'pro MATRIX_AND_STATUS, matrix, result_only=result_only, $'
   print, '                       type=type, real_part=real_part'
   return
endif
;
if NOT(KEYWORD_SET(result_only)) then begin
   print, 'input Matrix    :'
   print, matrix
endif
print, 'inverted Matrix :'
resultat=INVERT(matrix, status)
;
if KEYWORD_SET(real_part) then print, FLOAT(resultat) else print, resultat
print, 'Status : ', status
if KEYWORD_SET(type) then begin
   print, 'Type of input Matrix:    ', SIZE(matrix, /type)
   print, 'Type of inverted Matrix: ', SIZE(resultat, /type)
endif
print, '------------------------------------------'
;
end
;
pro TEST_INVERT_ONE_ELEMENT
;
;
print, 'Inversion when only ONE element'
;
print, 'exception'
a=0
MATRIX_AND_STATUS, a
a=0.
MATRIX_AND_STATUS, a
;
print, 'normal'
a=10
MATRIX_AND_STATUS, a
a=10.
MATRIX_AND_STATUS, a
;
print, 'test on COMPLEX'
a=[complex(0.,0.)]
MATRIX_AND_STATUS, a
;
a=[complex(0.,1.)]
MATRIX_AND_STATUS, a
;
end
;
pro TEST_INVERT_WITH_STRING
;
print, 'test on STRING'
;
MATRIX_AND_STATUS, ['1']
;
MATRIX_AND_STATUS, ['s']
;
a=[['2','1'],['2','1']]
MATRIX_AND_STATUS, a
;
a=[['2','1'],['2','s']]
MATRIX_AND_STATUS, a
;
end
;
pro TEST_INVERT_MATRIX, input_matrix=input_matrix, test=test, help=help
;
; testing the output for different input types
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_INVERT_MATRIX, input_matrix=input_matrix, test=test, help=help'
   return
endif
;
if N_ELEMENTS(input_matrix) EQ 0 then a=(INDGEN(3,3))^2+1 else a=input_matrix
;
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
print, 'COMPLEX typ (only shown real part)'
MATRIX_AND_STATUS, a+COMPLEXARR(3,3), /result_only, /type, /real_part
;
print, 'DCOMPLEX type (only shown real part)'
MATRIX_AND_STATUS, a+DCOMPLEXARR(3,3), /result_only, /type, /real_part
;
if KEYWORD_SET(test) then STOP
;
end
