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
print, PRODUCT(a,/int)
print, PRODUCT(a,/cumul,/int)
;
print, 'Same data but with NaN ...'
;
a=FINDGEN(5)+1     
a[2]=!values.f_nan
print, 'input :', a
print, PRODUCT(a,/nan)
print, PRODUCT(a,/cumul,/nan)
print, PRODUCT(a,/int,/nan)
print, PRODUCT(a,/cumul,/int,/nan)
;
end
;
; ---------------------------
;
pro TEST_PRODUCT_1D, help=help, nan=nan, no_format=no_format
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_PRODUCT_1D, help=help, nan=nan, no_format=no_format'
   return
endif

if NOT(KEYWORD_SET(no_format)) then begin
   f4='(A,4G12)' & i4='(A,4I12)'
   fi='(A,G12)'  & i1='(A,I12)'
endif else begin
   ;; not ready in GDL (AC 11/02/2010) 
   f4='' & f1=''
   i4='' & i1=''
endelse
;
txt0='input   '
txt1='cumul   '
txt2='product '
txtNaN='(with /Nan) :'
txtNon='(NONE /Nan) :'
  vide='            :'
txt0=txt0+vide
;
print, 'Reference WITHOUT Infinity nor NaN'
;
a=FINDGEN(4)+1
;
print, format=f1, txt0, a
print, format=i4, txt1+txtNon, PRODUCT(a,/int,/cum)
print, format=i4, txt1+txtNaN, PRODUCT(a,/int,/cum,/nan)
print, format=i1, txt2+txtNon, PRODUCT(a,/int)
print, format=i1, txt2+txtNaN, PRODUCT(a,/int,/nan)
;
print, 'Running WITH Infinity OR NaN (/nan)'
;
b=a
b[1]=!values.f_infinity
if KEYWORD_SET(nan) then b[1]=!values.f_nan
;
print, format=f1, txt0, b
print, format=i4, txt1+txtNon, PRODUCT(b,/int,/cum)
print, format=i4, txt1+txtNaN, PRODUCT(b,/int,/cum,/nan)
print, format=i1, txt2+txtNon, PRODUCT(b,/int)
print, format=i1, txt2+txtNaN, PRODUCT(b,/int,/nan)
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------
;
pro TEST_PRODUCT_2D, dim=dim, test=test

print, 'Reference WITHOUT dim and WITHOUT Infinity nor NaN'

a=FINDGEN(2,3)+1

print, PRODUCT(a,/int)
print, PRODUCT(a,/int,/nan)
print, PRODUCT(a,/int,/cum)
print, PRODUCT(a,/int,/cum,/nan)

print, 'data WITHOUT dim and WITH Infinity nor NaN'

b=a
b[1]=!values.f_infinity

print, PRODUCT(b,/int)
print, PRODUCT(b,/int,/nan)
print, PRODUCT(b,/int,/cum)
print, PRODUCT(b,/int,/cum,/nan)

dim=1

print, 'Reference WITHOUT dim and WITHOUT Infinity nor NaN'

a=FINDGEN(2,3)+1

print, PRODUCT(a,dim,/int)
print, PRODUCT(a,dim,/int,/nan)
print, PRODUCT(a,dim,/int,/cum)
print, PRODUCT(a,dim,/int,/cum,/nan)

print, 'data WITHOUT dim and WITH Infinity nor NaN'

b=a
b[1]=!values.f_infinity

print, PRODUCT(b,dim,/int)
print, PRODUCT(b,dim,/int,/nan)
print, PRODUCT(b,dim,/int,/cum)
print, PRODUCT(b,dim,/int,/cum,/nan)
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
pro TEST_PRODUCT_ALL_TYPE, test=test, nan=nan, $
                           verbose=verbose, very_verbose=very_verbose, $
                           exit_on_error=exit_on_error
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
   if ii EQ 7 then CONTINUE   ;; String
   if ii EQ 8 then CONTINUE   ;; Struc
   if ii EQ 10 then CONTINUE  ;; Pointer
   if ii EQ 11 then CONTINUE  ;; Objref
   ;;
   a=1b+INDGEN(5, type=ii)
   ;;
   err=''
   if (ABS(ref1- PRODUCT(a)) GT 1e-6) then err=err+'Erreur 1, '
   if (ABS(tref3-TOTAL(PRODUCT(a,/cumul))) GT 1e-6) then err=err+'Erreur 2, '
   if (ABS(ref2- PRODUCT(a,/int)) GT 1e-6) then err=err+'Erreur 3, '
   if (ABS(tref4-TOTAL(PRODUCT(a,/int, /cumul))) GT 1e-6) then err=err+'Erreur 4, '
   ;;
   if STRLEN(err) EQ 0 then begin
      err='None'
   endif else begin
      if KEYWORD_SET(exit_on_error) then begin
         MESSAGE, err + '(type: ' + SIZE(A,/type) + ')', /conti
         EXIT, status=1
      endif
   endelse
   ;;
   if KEYWORD_SET(verbose) then begin
      print, 'current TYPE : ', SIZE(A,/type), ', Type of Errors: ', err
   endif
   if KEYWORD_SET(very_verbose) then begin
      print, 'raw :', PRODUCT(a)
      print, '/cumul :', PRODUCT(a,/cumul)
      print, '/int :', PRODUCT(a,/int)
      print, '/int, /cumul :', PRODUCT(a,/cumul,/int)
   endif
endfor
;
if KEYWORD_SET(exit_on_error) then MESSAGE, /continue, 'All tests OK'
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------------------------
;
; SA: for inclusion in the "make check" rule
pro TEST_PRODUCT
  TEST_PRODUCT_BASIC
  TEST_PRODUCT_ALL_TYPE, /exit_on_error
end
