;
; AC, 18 March 2014
;
; We found by chance a bug in WHERE related to OpenMP.
; Any idea to extend test here are very welcome.
;
;
; ------------------------
;
pro TEST_WHERE_NULL, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
;
a=REPLICATE(1,10)
;
null1=WHERE(a GT 1,complement=c1,/null)
if ~ISA(null1,/null) then ERRORS_ADD, nb_errors, 'bad value for : null1'
if ISA(c1,/null) then ERRORS_ADD, nb_errors, 'bad value for : c1'
;
ok=WHERE(a EQ 1,complement=null2,/null)
if ISA(ok,/null) then ERRORS_ADD, nb_errors, 'bad value for : ok'
if ~ISA(null2,/null) then ERRORS_ADD, nb_errors, 'bad value for : null2'
;
BANNER_FOR_TESTSUITE, 'TEST_WHERE_NULL', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
; statistical approach : the initial way to find it !
;
pro TEST_WHERE_WITH_RANDOM, nbp_in, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
; nbp_in should normally been of small size as we are just interested in threaded vs. unthreaded behaviour.
; for very large nbp_in Total() may be a problem.
input=RANDOMU(seed, nbp_in)
;
; we force threads by using a small value for CPU,TPOOL_MIN_ELTS
SAVECPU=!CPU
CPU,TPOOL_MIN_ELTS=nbp_in/10
CPU,TPOOL_NTHREADS=!CPU.HW_NCPU
;
; what where() sees is an array, mostly of zeros and ones. But any array can be passed to where().
logical_value=input GE 0.5 ; this will be our template
;
;loop on types that are accepted by WHERE. the total of 'ok' location should be count, the total of 'nok' location must be zero.
; special care for complex values is needed.
; strings cannot be tested this way because of TOTAL() function
itype=[1,2,3,4,5,12,13,14,15,6,9]
stype=['Byte','Integer','Longword integer','Floating point','Double-precision floating','Unsigned Integer','Unsigned Longword Integer',$
'64-bit Integer','Unsigned 64-bit Integer','Complex floating','Double-precision complex']

if KEYWORD_SET(verbose) then print,'First pass, threaded, n_threads=',!CPU.TPOOL_NTHREADS

for i=0,n_elements(itype)-1 do begin
  type=itype[i]
  typename=stype[i]
  case type of
  6: begin & x=fix(logical_value,type=4) & y=x & val=complex(x,y) & end
  9: begin & x=fix(logical_value,type=9) & y=x & val=dcomplex(x,y) & end
  else: val=fix(logical_value,type=type)
  endcase

  ok=WHERE(val, nbp_ok)
  ; test ok is... ok
  if nbp_ok gt 0 then begin
     if total(val[ok],/integer) ne nbp_ok then begin
       nb_errors++
       if KEYWORD_SET(verbose) then print,'where() wrong for type '+typename
     endif
  endif else begin
     nb_errors++ ; as most probably there should be some values gt 0.5!
     print,"unexpected error occured, please investigate." ; as this gonna be worrying
  endelse
;
  ok=WHERE(val, nbp_ok, comp=nok, ncomp=nbp_nok)
 ; test ok is...(same as before, but remember: we DO NOT pass in the same lines of code as before!)
  if nbp_ok gt 0 then begin
     if total(val[ok],/integer) ne nbp_ok then begin
       nb_errors++
       if KEYWORD_SET(verbose) then print,'where() wrong for type '+typename
     endif
  endif else begin
     nb_errors++ ; as most probably there should be some values gt 0.5!
     print,'unexpected error occured for where(), type '+typename+', please investigate.' ; as this gonna be worrying
  endelse
  if nbp_nok gt 0 then begin
     if total(val[nok],/integer) ne 0 then begin
       nb_errors++
       if KEYWORD_SET(verbose) then print,'where(comp=xxx) wrong for type '+typename
     endif
  endif else begin
     nb_errors++ ; as most probably there should be some values gt 0.5!
     print,'unexpected error occured for where(comp=xxx), type '+typename+', please investigate.' ; as this gonna be worrying
  endelse

endfor

if KEYWORD_SET(verbose) then print,'Second pass, unthreaded (one thread)'

CPU,TPOOL_NTHREADS=1

for i=0,n_elements(itype)-1 do begin
  type=itype[i]
  typename=stype[i]
  case type of
  6: begin & x=fix(logical_value,type=4) & y=x & val=complex(x,y) & end
  9: begin & x=fix(logical_value,type=9) & y=x & val=dcomplex(x,y) & end
  else: val=fix(logical_value,type=type)
  endcase

  ok=WHERE(val, nbp_ok)
  ; test ok is... ok
  if nbp_ok gt 0 then begin
     if total(val[ok],/integer) ne nbp_ok then begin
       nb_errors++
       if KEYWORD_SET(verbose) then print,'where() wrong for type '+typename
     endif
  endif else begin
     nb_errors++ ; as most probably there should be some values gt 0.5!
     print,"unexpected error occured, please investigate." ; as this gonna be worrying
  endelse
;
  ok=WHERE(val, nbp_ok, comp=nok, ncomp=nbp_nok)
 ; test ok is...(same as before, but remember: we DO NOT pass in the same lines of code as before!)
  if nbp_ok gt 0 then begin
     if total(val[ok],/integer) ne nbp_ok then begin
       nb_errors++
       if KEYWORD_SET(verbose) then print,'where() wrong for type '+typename
     endif
  endif else begin
     nb_errors++ ; as most probably there should be some values gt 0.5!
     print,'unexpected error occured for where(), type '+typename+', please investigate.' ; as this gonna be worrying
  endelse
  if nbp_nok gt 0 then begin
     if total(val[nok],/integer) ne 0 then begin
       nb_errors++
       if KEYWORD_SET(verbose) then print,'where(comp=xxx) wrong for type '+typename
     endif
  endif else begin
     nb_errors++ ; as most probably there should be some values gt 0.5!
     print,'unexpected error occured for where(comp=xxx), type '+typename+', please investigate.' ; as this gonna be worrying
  endelse

endfor

CPU,RESTORE=SAVECPU

;
BANNER_FOR_TESTSUITE, 'TEST_WHERE_WITH_RANDOM', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_WHERE, size, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_WHERE, [size_of_test,] $'
    print, '                help=help, verbose=verbose, $'
    print, '                no_exit=no_exit, test=test'
    print, '...default size is 1000000 normally sufficient'
    return
endif
;
if (n_elements(size) le 0) then size=1E6 
nb_errors=0
;
TEST_WHERE_NULL, nb_errors, verbose=verbose 
;
TEST_WHERE_WITH_RANDOM, size, nb_errors, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_WHERE', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
