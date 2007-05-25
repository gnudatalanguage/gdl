;
; GM 09/05/2007
;
; plot of erf and erfc functions
;
; numerical tests and limits tests on these functions
;
; If you find bugs, limitations, others interresting cases,
; please report them to Gregory Marchal (email and code source in src/math_fun_gm.cpp)
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_erfs_plot

x = findgen(100)/10-5

    ; plot erf and erfc functions:
plot, x, erf(x), title = 'erf and erfc functions', xrange = [-5, 5], $
	yrange = [-1, 2]
oplot, x, erfc(x), linestyle = 2

    ; annotate plot:
xcoords = [3., 3.]
ycoords = [1.125, 0.125]
labels = ['erf','erfc']
xyouts, xcoords, ycoords, labels, /data

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_erf_numeric

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [0., .25, .5, .75, 1.]
ye = [0., .276326, .5205, .711156, .842701]
yc = erf(x)

print, 'float numerical tests on erf:'
print, format="(a18, 10(' ',g10.7))", 'x values:', x
print, format="(a18, 10(' ',g10.7))", 'expected erf(x):', ye
print, format="(a18, 10(' ',g10.7))", 'computed erf(x):', yc

for i = 0, 4 do begin
    if abs(ye(i) - yc(i)) ge 1e-6 then begin
        print, 'erf: float: warning: the difference between idl calculus and gdl calculus is greater than or equal to 1e-6!'
    endif
endfor
    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-!values.f_infinity, !values.f_infinity, !values.f_nan]
ye = [-1, 1, !values.f_nan]
yc = erf(x)

print, ''
print, 'float limits tests on erf:'
print, format="(a18, 10(' ',g10.7))", 'x values:', x
print, format="(a18, 10(' ',g10.7))", 'expected erf(x):', ye
print, format="(a18, 10(' ',g10.7))", 'computed erf(x):', yc

for i = 0, 1 do begin
    if ye(i) ne yc(i) then begin
	print, 'erf: float: warning: limit ' + strtrim(i,1) + ' is wrong!'
    endif
endfor

if finite(yc(2),/nan) ne 1 then begin
    print, 'erf: float: warning: limit 2 is wrong!'
endif

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [0.d, .25d, .5d, .75d, 1.d]
ye = [0.d,.27632639d, .52049988d, .71115563d, .84270079d]
yc = erf(x)

print, ''
print, 'double numerical tests on erf:'
print, format="(a18, 10(' ',g10.7))", 'x values:', x
print, format="(a18, 10(' ',g10.7))", 'expected erf(x):', ye
print, format="(a18, 10(' ',g10.7))", 'computed erf(x):', yc

for i = 0, 4 do begin
    if abs(ye(i) - yc(i)) ge 1d-6 then begin
        print, 'erf: double: warning: the difference between idl calculus and gdl calculus is greater than or equal to 1e-6!'
    endif
endfor

    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-!values.d_infinity, !values.d_infinity, !values.d_nan]
ye = [-1d, 1d, !values.d_nan]
yc = erf(x)

print, ''
print, 'double limits tests on erf:'
print, format="(a18, 10(' ',g10.7))", 'x values:', x
print, format="(a18, 10(' ',g10.7))", 'expected erf(x):', ye
print, format="(a18, 10(' ',g10.7))", 'computed erf(x):', yc

for i = 0, 1 do begin
    if ye(i) ne yc(i) then begin
	print, 'erf: double: warning: limit ' + strtrim(i,1) + ' is wrong!'
    endif
endfor

if finite(yc(2),/nan) ne 1 then begin
    print, 'erf: double: warning: limit 2 is wrong!'
endif

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_erfc_numeric

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [0., .25, .5, .75, 1.]
ye = [1., .723674, .479500, .288844, .157299]
yc = erfc(x)

print, ''
print, ' ------------------------------------------------------------------ '
print, ''
print, 'float numerical tests on erfc:'
print, format="(a18, 10(' ',g10.7))", 'x values:', x
print, format="(a18, 10(' ',g10.7))", 'expected erfc(x):', ye
print, format="(a18, 10(' ',g10.7))", 'computed erfc(x):', yc

for i = 0, 4 do begin
    if abs(ye(i) - yc(i)) ge 1e-6 then begin
        print, 'erf: float: warning: the difference between idl calculus and gdl calculus is greater than or equal to 1e-6!'
    endif
endfor
    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-!values.f_infinity, !values.f_infinity, !values.f_nan]
ye = [2, 0, !values.f_nan]
yc = erfc(x)

print, ''
print, 'float limits tests on erfc:'
print, format="(a18, 10(' ',g10.7))", 'x values:', x
print, format="(a18, 10(' ',g10.7))", 'expected erfc(x):', ye
print, format="(a18, 10(' ',g10.7))", 'computed erfc(x):', yc

for i = 0, 1 do begin
    if ye(i) ne yc(i) then begin
	print, 'erf: float: warning: limit ' + strtrim(i,1) + ' is wrong!'
    endif
endfor

if finite(yc(2),/nan) ne 1 then begin
    print, 'erf: float: warning: limit 2 is wrong!'
endif

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [0.d, .25d, .5d, .75d, 1.d]
ye = [1.d, .72367361d, .47950012d, .28884437d, .15729921d]

yc = erfc(x)

print, ''
print, 'double numerical tests on erfc:'
print, format="(a18, 10(' ',g10.7))", 'x values:', x
print, format="(a18, 10(' ',g10.7))", 'expected erfc(x):', ye
print, format="(a18, 10(' ',g10.7))", 'computed erfc(x):', yc

for i = 0, 4 do begin
    if abs(ye(i) - yc(i)) ge 1d-6 then begin
        print, 'erf: double: warning: the difference between idl calculus and gdl calculus is greater than or equal to 1e-6!'
    endif
endfor

    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-!values.d_infinity, !values.d_infinity, !values.d_nan]
ye = [2d, 0d, !values.d_nan]
yc = erfc(x)

print, ''
print, 'double limits tests on erfc:'
print, format="(a18, 10(' ',g10.7))", 'x values:', x
print, format="(a18, 10(' ',g10.7))", 'expected erfc(x):', ye
print, format="(a18, 10(' ',g10.7))", 'computed erfc(x):', yc

for i = 0, 1 do begin
    if ye(i) ne yc(i) then begin
	print, 'erf: double: warning: limit ' + strtrim(i,1) + ' is wrong!'
    endif
endfor

if finite(yc(2),/nan) ne 1 then begin
    print, 'erf: double: warning: limit 2 is wrong!'
endif

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_erfs

test_erfs_plot
test_erf_numeric
test_erfc_numeric

print, ''
print, 'Please remember that gdl does not support now complex erf & erfc (due to GSL lib. limitation)'
print, 'GDL does not support at all erfcx now.'
print, 'If you have suggestion, please contact us.'

end
