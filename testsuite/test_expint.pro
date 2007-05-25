;
; GM 22/05/2007
;
; Plots of ExpInt function
;
; Numerical tests and limit tests on these functions
;
; If you find bugs, limitations, others interresting cases,
; please report them to Gregory Marchal (email and code source in src/math_fun_gm.cpp)
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_expint_plot

x = findgen(1000)/200+.001

    ; plot ExpInt function:
plot, x, expint(0, x), title = 'E_n function', xrange = [0, 5], $
	yrange = [0, 1], xtitle = 'x', ytitle = 'E_n(x)'

for i = 1, 3 do begin
    oplot, x, expint(i, x), linestyle = i
endfor

    ; annotate plot:
xcoords = [1.0, 0.75, 0.5, 0.25]
ycoords = [0.25, 0.25, 0.25, 0.25]
labels = ['n=0', 'n=1', 'n=2', 'n=3']
xyouts, xcoords, ycoords, labels, /data

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_numeric2, x, y, ze, zc, type, xname, yname, fun_name, fun_form

print ,''
if size(y, /type) eq 4 then print, 'Float '+type+' tests on '+fun_name+':'
if size(y, /type) eq 5 then print, 'Double '+type+' tests on '+fun_name+':'
print, format="(a18, 10(' ',g10.7))", xname+' values:', x
print, format="(a18, 10(' ',g10.7))", yname+' values:', y
print, format="(a18, 10(' ',g10.7))", 'Expected '+fun_form+':', ze
print, format="(a18, 10(' ',g10.7))", 'Computed '+fun_form+':', zc

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_expint_numeric

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; float ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
n  = [1, 1, 2, 2]
x  = [1., 2., 1., 2.]
ye = [.219384, .0489005, .148496, .0375343]
yc = expint(n, x)

test_numeric2, n, x, ye, yc, 'numerical', 'n', 'x', 'ExpInt', 'E_n(x)'

for i = 0, 3 do begin
    if abs(ye(i) - yc(i)) ge 1e-6 then begin
        print, 'EXPINT: FLOAT: WARNING: the difference between expected values and computed values is greater than or equal to 1e-6!'
    endif
endfor

    ; numerical tests ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; double ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
n  = [1, 1, 2, 2]
x  = [1d, 2d, 1d, 2d]
ye = [.21938393, .048900511, .14849551, .037534262]
yc = expint(n, x)

test_numeric2, n, x, ye, yc, 'numerical', 'n', 'x', 'ExpInt', 'E_n(x)'

for i = 0, 3 do begin
    if abs(ye(i) - yc(i)) ge 1d-6 then begin
        print, 'EXPINT: DOUBLE: WARNING: the difference between expected values and computed values is greater than or equal to 1d-6!'
    endif
endfor

end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
pro test_expint

window, xsize = 800, ysize = 600
test_expint_plot

test_expint_numeric

print, ''
print, 'Please remember that GDL does not support now complex ExpInt.'
print, 'If you have suggestion, please contact us.'

end