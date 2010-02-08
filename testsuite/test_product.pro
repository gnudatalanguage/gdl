;
; AC 04/02/2010 under GNU GPL v2/3
;
; Test of PRODUCT, some options are mandatory
; for READFITS routine in Astron package
;
pro TEST_PRODUCT_BASIC

a=[1,2,3,4,5]

print, 'input :', a
print, PRODUCT(a)
print, PRODUCT(a,/cumul)
print, ROUND(PRODUCT(a,/int))
print, ROUND(PRODUCT(a,/cumul,/int))
;
print, 'Same data but with NaN ...'
;
a=FINDGEN(5)+1     
a[2]=!values.f_nan
print, 'input :', a
print, PRODUCT(a,/nan)
print, PRODUCT(a,/cumul,/nan)
print, ROUND(PRODUCT(a,/int,/nan))
print, ROUND(PRODUCT(a,/cumul,/int,/nan))
;
end
;
; ---------------------------
;
pro TEST_PRODUCT_ALL_TYPE, test=test, verbose=verbose, nan=nan
; list of TYPE
; http://idlastro.gsfc.nasa.gov/idl_html_help/SIZE.html
;
a=INDGEN(5)+1
;
; not ready now, pb with INT types ...
if KEYWORD_SET(nan) then begin
    print, 'Sorry, this option is not ready now'
    ;a[2]=!value.f_nan
endif
;
ref1=PRODUCT(a)
ref2=PRODUCT(a,/int)
ref3=PRODUCT(a,/cumul)
ref4=PRODUCT(a,/cumul,/int)
tref3=TOTAL(ref3)
tref4=TOTAL(ref4)
;
for ii=1, 15 do begin
    if ii EQ 7 then CONTINUE ;; String
    if ii EQ 8 then CONTINUE ;; Struc
    if ii EQ 10 then CONTINUE ;; Pointer
    if ii EQ 11 then CONTINUE ;; Objref
    ;;
    a=1b+INDGEN(5, type=ii)
    ;;
    err=''
    if (ABS(ref1- PRODUCT(a)) GT 1e-6) then err=err+'Erreur 1, '
    if (ABS(tref3-TOTAL(PRODUCT(a,/cumul))) GT 1e-6) then err=err+'Erreur 2, '
    if (ABS(ref2- PRODUCT(a,/int)) GT 1e-6) then err=err+'Erreur 3, '
    if (ABS(tref4-TOTAL(PRODUCT(a,/int, /cumul))) GT 1e-6) then err=err+'Erreur 4, '
    ;;
    if STRLEN(err) EQ 0 then err='None'
    print, 'current TYPE : ', SIZE(A,/type), ', Type of Errors: ', err
    ;;
    if KEYWORD_SET(verbose) then begin
        print, 'raw :', PRODUCT(a)
        print, '/cumul :', PRODUCT(a,/cumul)
        print, '/int :', PRODUCT(a,/int)
        print, '/int, /cumul :', PRODUCT(a,/cumul,/int)
    endif
endfor
;
if KEYWORD_SET(test) then STOP
;
end
