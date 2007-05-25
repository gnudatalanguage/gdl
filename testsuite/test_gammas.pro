;
; GM 16/05/2007
;
; Plots of Gamma,lnGamma, iGamma and Beta functions
;
; Numerical tests and limit tests on these functions
;
; If you find bugs, limitations, others interresting cases,
; please report them to Gregory Marchal (email and code source in src/math_fun_gm.cpp)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_gamma_plot

x = findgen(1000)/100-5

    ; plot gamma function:
plot, x, gamma(x), title = 'Gamma function', xrange = [-5, 5], $
	yrange = [-5, 5], xtitle = 'x', ytitle = 'G(x)'

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_lngamma_plot

x = findgen(1000)/100

    ; plot lngamma function:
plot, x, lngamma(x), title = 'lnGamma function', xrange = [0, 10], $
	yrange = [min(lngamma(x)), lngamma(10)], xtitle = 'x', ytitle = 'lnG(x)'

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_igamma_plot

x = findgen(1000)/100

    ; plot igamma function:
plot, x, igamma(1,x), title = 'iGamma function', xrange = [0, 10], $
	yrange = [0, 1], xtitle = 'x', ytitle = 'P(a,x)'

for i = 2, 6 do begin
    oplot, x, igamma(i,x), linestyle = i
endfor

    ; annotate plot:
xcoords = [1., 2.25, 3.5, 4.75, 6., 7.25]
ycoords = [.8, .8, .8, .8, .8, .8]
labels = ['a=1', 'a=2', 'a=3', 'a=4', 'a=5', 'a=6']
xyouts, xcoords, ycoords, labels, /data

    ; surface igamma function
z = findgen(10, 10)
for i = 0, 9 do begin
    for j = 0, 9 do begin
        z(i, j) = igamma(i+1, j+1)
    endfor
endfor

surface, z, title = 'iGamma function', xtitle = 'a', ytitle = 'x', ztitle = 'P(a,x)'

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_beta_plot

    ; surface beta function:
z = findgen(10, 10)
for i = 0, 9 do begin
    for j = 0, 9 do begin
        z(i, j) = beta(i+.5, j+.5)
    endfor
endfor

surface, z, title = 'Beta function', xtitle = 'x', ytitle = 'y', ztitle = 'B(x,y)', zrange = [0., !pi]

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_numeric, x, ye, yc, type, fun_name, fun_form

print ,''
if size(x, /type) eq 4 then print, 'Float '+type+' tests on '+fun_name+':'
if size(x, /type) eq 5 then print, 'Double '+type+' tests on '+fun_name+':'
print, format="(a18, 10(' ',g10.7))", 'x values:', x
print, format="(a18, 10(' ',g10.7))", 'Expected '+fun_form+':', ye
print, format="(a18, 10(' ',g10.7))", 'Computed '+fun_form+':', yc

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_numeric2, x, y, ze, zc, type, xname, yname, fun_name, fun_form

print ,''
if size(x, /type) eq 4 then print, 'Float '+type+' tests on '+fun_name+':'
if size(x, /type) eq 5 then print, 'Double '+type+' tests on '+fun_name+':'
print, format="(a18, 10(' ',g10.7))", xname+' values:', x
print, format="(a18, 10(' ',g10.7))", yname+' values:', y
print, format="(a18, 10(' ',g10.7))", 'Expected '+fun_form+':', ze
print, format="(a18, 10(' ',g10.7))", 'Computed '+fun_form+':', zc

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro separator

print, ''
print, ' ----------------------------------------------------------------------- '

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_gamma_numeric

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-3/2., -1/2., 0., 1/2., 3/2.]
ye = [4*sqrt(!pi)/3, -2*sqrt(!pi),!values.f_infinity , sqrt(!pi), sqrt(!pi)/2]
yc = gamma(x)

test_numeric, x, ye, yc, 'numerical', 'Gamma', 'G(x)'

for i = 0, 4 do begin
    if abs(ye(i) - yc(i)) ge 1e-6 then begin
        print, 'GAMMA: FLOAT: WARNING: The difference between expected values and computed values is greater than or equal to 1e-6!'
    endif
endfor
    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-!values.f_infinity, !values.f_infinity, !values.f_nan]
ye = [!values.f_nan, !values.f_infinity, !values.f_nan]
yc = gamma(x)

test_numeric, x, ye, yc, 'limits', 'Gamma', 'G(x)'

if finite(yc(0), /nan) ne 1 then print, 'GAMMA: FLOAT: WARNING: 1th limit is wrong!'
if ye(1) ne yc(1)           then print, 'GAMMA: FLOAT: WARNING: 2th limit is wrong!'
if finite(yc(2), /nan) ne 1 then print, 'GAMMA: FLOAT: WARNING: 3th limit is wrong!'

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-3/2d, -1/2d, 0d, 1/2d, 3/2d]
ye = [4*sqrt(!dpi)/3, -2*sqrt(!dpi),!values.d_infinity , sqrt(!dpi), sqrt(!dpi)/2]
yc = gamma(x)

test_numeric, x, ye, yc, 'numerical', 'Gamma', 'G(x)'

for i = 0, 4 do begin
    if abs(ye(i) - yc(i)) ge 1d-6 then begin
        print, 'GAMMA: DOUBLE: WARNING: The difference between expected values and computed values is greater than or equal to 1d-6!'
    endif
endfor

    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-!values.d_infinity, !values.d_infinity, !values.d_nan]
ye = [!values.d_nan, !values.d_infinity, !values.d_nan]
yc = gamma(x)

test_numeric, x, ye, yc, 'limits', 'Gamma', 'G(x)'

if finite(yc(0), /nan) ne 1 then print, 'GAMMA: DOUBLE: WARNING: 1th limit is wrong!'
if ye(1) ne yc(1)           then print, 'GAMMA: DOUBLE: WARNING: 2th limit is wrong!'
if finite(yc(2), /nan) ne 1 then print, 'GAMMA: DOUBLE: WARNING: 3th limit is wrong!'

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_lngamma_numeric

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [0., 1/2., 3/2., 5/2., 7/2.]
ye = alog([!values.f_infinity, sqrt(!pi), sqrt(!pi)/2, 3*sqrt(!pi)/4, 15*sqrt(!pi)/8])
yc = lngamma(x)

test_numeric, x, ye, yc, 'numerical', 'lnGamma', 'lnG(x)'

if finite(yc(0), /inf) ne 1 then print, 'LNGAMMA: FLOAT: WARNING: Inf expected!'

for i = 1, 4 do begin
    if abs(ye(i) - yc(i)) ge 1e-6 then begin
        print, 'LNGAMMA: FLOAT: WARNING: the difference between expected values and computed values is greater than or equal to 1e-6!'
    endif
endfor
    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-!values.f_infinity, !values.f_infinity, !values.f_nan]
ye = [!values.f_infinity, !values.f_infinity, !values.f_nan]
yc = lngamma(x)

test_numeric, x, ye, yc, 'limits', 'lnGamma', 'lnG(x)'

if finite(yc(0), /inf) ne 1 then print, 'LNGAMMA: FLOAT: WARNING: 1th limit is wrong!'
if ye(1) ne yc(1)           then print, 'LNGAMMA: FLOAT: WARNING: 2th limit is wrong!'
if finite(yc(2), /nan) ne 1 then print, 'LNGAMMA: FLOAT: WARNING: 3th limit is wrong!'

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [0d, 1/2d, 3/2d, 5/2d, 7/2d]
ye = alog([!values.d_infinity, sqrt(!dpi), sqrt(!dpi)/2, 3*sqrt(!dpi)/4, 15*sqrt(!dpi)/8])
yc = lngamma(x)

test_numeric, x, ye, yc, 'numerical', 'lnGamma', 'lnG(x)'

if finite(yc(0), /inf) ne 1 then print, 'LNGAMMA: DOUBLE: WARNING: Inf expected!'

for i = 1, 4 do begin
    if abs(ye(i) - yc(i)) ge 1d-6 then begin
        print, 'LNGAMMA: DOUBLE: WARNING: The difference between expected values and computed values is greater than or equal to 1d-6!'
    endif
endfor

    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-!values.d_infinity, !values.d_infinity, !values.d_nan]
ye = [!values.d_infinity, !values.d_infinity, !values.d_nan]
yc = lngamma(x)

test_numeric, x, ye, yc, 'limits', 'lnGamma', 'lnG(x)'

if finite(yc(0), /inf) ne 1 then print, 'LNGAMMA: DOUBLE: WARNING: 1th limit is wrong!'
if ye(1) ne yc(1)           then print, 'LNGAMMA: DOUBLE: WARNING: 2th limit is wrong!'
if finite(yc(2), /nan) ne 1 then print, 'LNGAMMA: DOUBLE: WARNING: 3th limit is wrong!'

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_igamma_numeric

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
a  = [1/2., 1/2., 1/2., 1/2., 1/2.]
x  = [0., 1/2., 3/2., 5/2., 7/2.]
ye = [0., .682689, .916735, .974653, .991849]
yc = igamma(a, x)

test_numeric2, a, x, ye, yc, 'numerical', 'a', 'x', 'iGamma', 'P(a,x)'

for i = 0, 4 do begin
    if abs(ye(i) - yc(i)) ge 1e-6 then begin
        print, 'IGAMMA: FLOAT: WARNING: the difference between expected values and computed values is greater than or equal to 1e-6!'
    endif
endfor
    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
a  = [!values.f_infinity, !values.f_infinity, !values.f_nan, !values.f_nan]
x  = [!values.f_infinity, !values.f_nan, !values.f_infinity, !values.f_nan]
ye = [1., !values.f_nan, !values.f_nan, !values.f_nan]
yc = igamma(a, x)

test_numeric2, a, x, ye, yc, 'limits', 'a', 'x', 'iGamma', 'P(a,x)'

if ye(0) ne yc(0) then print, 'IGAMMA: FLOAT: WARNING: 1th limit is wrong!'
for i = 1, 3 do begin
    if finite(yc(i), /nan) ne 1 then print, 'IGAMMA: FLOAT: WARNING: '+strtrim(i+1,1)+'th limit is wrong!'
endfor

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
a  = [1/2d, 1/2d, 1/2d, 1/2d, 1/2d]
x  = [0d, 1/2d, 3/2d, 5/2d, 7/2d]
ye = [0, .68268949, .91673548, .97465268, .99184903]
yc = igamma(a, x)

test_numeric2, a, x, ye, yc, 'numerical', 'a', 'x', 'iGamma', 'P(a,x)'

for i = 0, 4 do begin
    if abs(ye(i) - yc(i)) ge 1d-6 then begin
        print, 'IGAMMA: DOUBLE: WARNING: the difference between expected values and computed values is greater than or equal to 1d-6!'
    endif
endfor

    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
a  = [!values.d_infinity, !values.d_infinity, !values.d_nan, !values.d_nan]
x  = [!values.d_infinity, !values.d_nan, !values.d_infinity, !values.d_nan]
ye = [1d, !values.d_nan, !values.d_nan, !values.d_nan]
yc = igamma(a, x)

test_numeric2, a, x, ye, yc, 'limits', 'a', 'x', 'iGamma', 'P(a,x)'

if ye(0) ne yc(0) then print, 'IGAMMA: DOUBLE: WARNING: 1th limit is wrong!'
for i = 1, 3 do begin
    if finite(yc(i), /nan) ne 1 then print, 'IGAMMA: DOUBLE: WARNING: '+strtrim(i+1,1)+'thlimit is wrong!'
endfor

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_beta_numeric

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [1/2., 1/2., 1/2., 1/2., 1/2.]
y  = [0., 1/2., 3/2., 5/2., 7/2.]
ze = [!values.f_infinity, !pi, !pi/2, 3*!pi/8, 5*!pi/16]
zc = beta(x, y)

test_numeric2, x, y, ze, zc, 'numerical', 'x', 'y', 'Beta', 'B(x,y)'

for i = 0, 4 do begin
    if abs(ze(i) - zc(i)) ge 1e-6 then begin
        print, 'BETA: FLOAT: WARNING: the difference between expected values and computed values is greater than or equal to 1e-6!'
    endif
endfor

    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-!values.f_infinity, !values.f_infinity, !values.f_nan, -!values.f_infinity, !values.f_infinity,!values.f_nan, -!values.f_infinity, !values.f_infinity, !values.f_nan]
y  = [-!values.f_infinity, -!values.f_infinity, -!values.f_infinity, !values.f_infinity, !values.f_infinity, !values.f_infinity, !values.f_nan, !values.f_nan, !values.f_nan]
ze = [!values.f_nan, !values.f_nan, !values.f_nan, !values.f_nan, !values.f_nan, !values.f_nan, !values.f_nan, !values.f_nan, !values.f_nan]
zc = beta(x, y)

test_numeric2, x(0:4), y(0:4), ze(0:4), zc(0:4), 'limits', 'x', 'y', 'Beta', 'B(x,y)'
test_numeric2, x(5:8), y(5:8), ze(5:8), zc(5:8), 'limits', 'x', 'y', 'Beta', 'B(x,y)'

for i = 0, 8 do begin
    if finite(zc(i), /nan) ne 1 then begin
        print, 'BETA: FLOAT: WARNING: '+strtrim(i+1,1)+'th limit is wrong!'
    endif
endfor

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [1/2d, 1/2d, 1/2d, 1/2d, 1/2d]
y  = [0d, 1/2d, 3/2d, 5/2d, 7/2d]
ze = [!values.d_infinity, !dpi, !dpi/2, 3*!dpi/8, 5*!dpi/16]
zc = beta(x, y)

test_numeric2, x, y, ze, zc, 'numerical', 'x', 'y', 'Beta', 'B(x,y)'

for i = 0, 4 do begin
    if abs(ze(i) - zc(i)) ge 1d-6 then begin
        print, 'BETA: DOUBLE: WARNING: the difference between expected values and computed values is greater than or equal to 1d-6!'
    endif
endfor

    ; limits tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
x  = [-!values.d_infinity, !values.d_infinity, !values.d_nan, -!values.d_infinity, !values.d_infinity,!values.d_nan, -!values.d_infinity, !values.d_infinity, !values.d_nan]
y  = [-!values.d_infinity, -!values.d_infinity, -!values.d_infinity, !values.d_infinity, !values.d_infinity, !values.d_infinity, !values.d_nan, !values.d_nan, !values.d_nan]
ze = [!values.d_nan, !values.d_nan, !values.d_nan, !values.d_nan, !values.d_nan, !values.d_nan, !values.d_nan, !values.d_nan, !values.d_nan]
zc = beta(x, y)

test_numeric2, x(0:4), y(0:4), ze(0:4), zc(0:4), 'limits', 'x', 'y', 'Beta', 'B(x,y)'
test_numeric2, x(5:8), y(5:8), ze(5:8), zc(5:8), 'limits', 'x', 'y', 'Beta', 'B(x,y)'

for i = 0, 8 do begin
    if finite(zc(i), /nan) ne 1 then begin
        print, 'BETA: DOUBLE: WARNING: '+strtrim(i+1,1)+'th limit is wrong!'
    endif
endfor

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_gammas

window, xsize = 800, ysize = 600
!p.multi = [0, 2, 3]
test_gamma_plot
test_lngamma_plot
test_igamma_plot
test_beta_plot
!p.multi = 0

test_gamma_numeric
	separator
test_lngamma_numeric
	separator
test_igamma_numeric
	separator
test_beta_numeric

print, ''
print, 'Please remember that GDL does not support now complex Gamma, lnGamma, iGamma & Beta (GSL limitation) and that iGamma does not computes for first parameter lesser than or equal to zero (GSL limitation).'
print, 'If you have suggestion, please contact us.'

end