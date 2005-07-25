; Script : regression-total
pro regression, a

 print, 'Testing '+size(a, /tname)+':'
; help, total(a), total(a, /NaN), total(a, /cumul), total(a, /cumul, /NaN), output = output
; print, output
; help, total(a, /double), total(a, /NaN, /double), total(a, /cumul, /double), total(a, /cumul, /NaN, /double), output=output
; print, output
; help, total(a, 1), total(a, 1, /NaN), total(a, 1, /cumul), total(a, 1, /cumul, /NaN), output=output
; print, output
; help, total(a, 1, /double), total(a, 1, /NaN, /double), total(a, 1, /cumul, /double), total(a, 1, /cumul, /NaN, /double), output=output
; print, output
; help, total(a, 2), total(a, 2, /NaN), total(a, 2, /cumul), total(a, 2, /cumul, /NaN), output=output
; print, output
; help, total(a, 2, /double), total(a, 2, /NaN, /double), total(a, 2, /cumul, /double), total(a, 2, /cumul, /NaN, /double), output=output
; print, output
; help, total(a, 3), total(a, 3, /NaN), total(a, 3, /cumul), total(a, 3, /cumul, /NaN), output=output
; print, output
; help, total(a, 3, /double), total(a, 3, /NaN, /double), total(a, 3, /cumul, /double), total(a, 3, /cumul, /NaN, /double), output=output
; print, output

 print, '-- 1'
 print, total(a) & print, '-- 2'
 print, total(a, /NaN) & print, '-- 3'
 print, total(a, /cumul) & print, '-- 4'
 print, total(a, /cumul, /NaN) & print, '-- 5'
 print, total(a, /double) & print, '-- 6'
 print, total(a, /NaN, /double) & print, '-- 7'
 print, total(a, /cumul, /double) & print, '-- 8'
 print, total(a, /cumul, /NaN, /double)

 print, '==========================================='
 print, '-- 9'
 print, total(a, 1) & print, '-- 10'
 print, total(a, 1, /NaN) & print, '-- 11'
 print, total(a, 1, /cumul) & print, '-- 12'
 print, total(a, 1, /cumul, /NaN) & print, '-- 13'
 print, total(a, 1, /double) & print, '-- 14'
 print, total(a, 1, /NaN, /double) & print, '-- 15'
 print, total(a, 1, /cumul, /double) & print, '-- 16'
 print, total(a, 1, /cumul, /NaN, /double)
 
 print, '==========================================='

 print, '-- 17'
 print, total(a, 2) & print, '-- 18'
 print, total(a, 2, /NaN) & print, '-- 19'
 print, total(a, 2, /cumul) & print, '-- 20'
 print, total(a, 2, /cumul, /NaN) & print, '-- 21'
 print, total(a, 2, /double) & print, '-- 22'
 print, total(a, 2, /NaN, /double) & print, '-- 23'
 print, total(a, 2, /cumul, /double) & print, '-- 24'
 print, total(a, 2, /cumul, /NaN, /double)

 print, '==========================================='

 print, '-- 25'
 print, total(a, 3) & print, '-- 26'
 print, total(a, 3, /NaN) & print, '-- 27'
 print, total(a, 3, /cumul) & print, '-- 28'
 print, total(a, 3, /cumul, /NaN) & print, '-- 29'
 print, total(a, 3, /double) & print, '-- 30'
 print, total(a, 3, /NaN, /double) & print, '-- 31'
 print, total(a, 3, /cumul, /double) & print, '-- 32'
 print, total(a, 3, /cumul, /NaN, /double)

end

 ; we need a way to know whether IDL or GDL is running...
 isGDL = !version.build_date eq 'Jul 07 2005'
 if isGDL then begin
    journal, 'gdl-output-regression-total.txt'
 endif else begin
    journal, 'idl'+strtrim(!version.release,1)+'-output-regression-total.txt'
 endelse
 
 a = bytarr(3, 3, 3)+1b
 regression, a
 
 a = findgen(3, 3, 3)
 a[0,0,0] = !values.f_nan
 a[1,1,0] = !values.f_infinity
 a[2,1,0] = -!values.f_infinity
 a[0,2,1] = !values.f_nan
 a[2,0,2] = !values.f_nan
 regression, a

 a = dindgen(3, 3, 3)
 a[0,0,0] = !values.d_nan
 a[0,1,1] = !values.d_infinity
 a[2,0,1] = -!values.d_infinity
 a[0,2,1] = !values.d_nan
 a[2,2,2] = !values.d_nan
 regression, a

 a = complexarr(3, 3, 3)+1
 a[0,0,0] = complex(3, !values.f_nan)
 a[1,1,0] = complex(!values.f_nan, !values.f_infinity)
 a[2,1,0] = complex(0, -!values.f_infinity)
 a[0,2,1] = complex(78, !values.f_nan)
 a[2,0,2] = complex(!values.f_infinity, 40)
 regression, a
 
 a = dcomplexarr(3, 3, 3)+1
 a[0,0,0] = complex(3, !values.d_nan)
 a[1,1,0] = complex(!values.d_nan, !values.d_infinity)
 a[2,1,0] = complex(0, -!values.d_infinity)
 a[0,2,1] = complex(78, !values.d_nan)
 a[2,0,2] = complex(!values.d_infinity, 40)
 regression, a

 journal
 
end
