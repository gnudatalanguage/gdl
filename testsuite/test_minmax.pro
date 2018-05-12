;
; AC June 2007 
; GD Apr  2018
; Purpose: a collection for testing Inf and Nan managing 
; in Min/Max procedures (up to now, Inf was not managed,
; NaN was not well managed if present in first place).
; Added test for new "/ABS" option, complex values, branches
; accelerated or not, and using the fact that random values are now
; identical with IDL, wether there is an error or not. 
; Call the procedure "TEST_MINMAX" and check output.
;
function GET_MINMAX_ARRAY,type=type, problem=problem, nbp=nbp, $
                  position=position
; test Nan and Inf for MIN only or MAX only.
if N_ELEMENTS(nbp) EQ 0 then nbp=10
if N_ELEMENTS(type) EQ 0 then type=5 ; double
;

if N_ELEMENTS(position) EQ 0 then position=4
;
if position GT nbp-1 then position=0
;
a=randomn(66,nbp,/double) ; fixed seed.
b=randomn(77,nbp,/double) ; fixed seed.
;a=dindgen(nbp)
b=a

; Inf or NaN for integers is no defined and GDL does not work like IDL.
switch (type) of
 1: 
 2: 
 3: problem=-1
endswitch 

a[position]=problem
b[position]=problem

switch (type) of
 1: return, byte(a)
 2: return, fix(a)
 3: return, long(a)
 4: return, float(a)
 5: return, a
 6: return, complex(a,b)
 7: return, string(a)
 9: return, dcomplex(a,b)
 else: return, 0
endswitch 
return, a
;
end
;
pro COMPUTE_MINMAX,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun
atom=a[0]
result=replicate(atom,16)
minmax=replicate(atom,8)
result[0] =MIN(a)
result[1] =MIN(a,/nan)
result[2] =MAX(a)
result[3] =MAX(a,/nan)
result[4] =MIN(a, max=toto) & minmax[0]=toto      
result[5] =MIN(a, max=toto,/nan)  & minmax[1]=toto   
result[6] =MAX(a, min=toto)  & minmax[2]=toto        
result[7] =MAX(a, min=toto,/nan)  & minmax[3]=toto   
result[8] =MIN(a,/abs)
result[9] =MIN(a,/abs,/nan)
result[10] =MAX(a,/abs)
result[11] =MAX(a,/abs,/nan)
result[12] =MIN(a,/abs, max=toto) & minmax[4]=toto      
result[13] =MIN(a,/abs, max=toto,/nan)  & minmax[5]=toto   
result[14] =MAX(a,/abs, min=toto)  & minmax[6]=toto        
result[15] =MAX(a,/abs, min=toto,/nan)  & minmax[7]=toto   

   printf,lun, 'Type of problem: '+string(problem)+', size: '+string(nbp)+' pos: '+string(pos)+' type= '+string(size(a,/type))
   printf,lun, 'Min OR Max'
   printf,lun, '--------------'
   printf,lun, 'MIN         :', result[0]
   printf,lun, 'MIN and /NAN:', result[1]
   printf,lun, 'MAX         :', result[2]
   printf,lun, 'MAX and /NAN:', result[3]
   printf,lun, '--------------'
   printf,lun, 'Min AND Max'
   printf,lun, '--------------'
   printf,lun, 'MIN         :', result[4], minmax[0]
   printf,lun, 'MIN and /NAN:', result[5], minmax[1]
   printf,lun, 'MAX         :', result[6], minmax[2]
   printf,lun, 'MAX and /NAN:', result[7], minmax[3]
   printf,lun, '--------------'
   printf,lun, 'Min OR Max /ABS'
   printf,lun, '--------------'
   printf,lun, 'MIN         :', result[8]
   printf,lun, 'MIN and /NAN:', result[9]
   printf,lun, 'MAX         :', result[10]
   printf,lun, 'MAX and /NAN:', result[11]
   printf,lun, '--------------'
   printf,lun, 'Min AND Max /ABS'
   printf,lun, '--------------'
   printf,lun, 'MIN         :', result[12], minmax[4]
   printf,lun, 'MIN and /NAN:', result[13], minmax[5]
   printf,lun, 'MAX         :', result[14], minmax[6]
   printf,lun, 'MAX and /NAN:', result[15], minmax[7]
   printf,lun, '--------------'

end
;
; ------------------------------------------------
; calling differents cases (position of data to be flagged)
; and returning only Min OR Max, with and without using threads etc
;
pro MULTI_BASIC_MINMAX, lun
;
types=[1,2,3,4,5,6,9,7]
for i=0,7 do begin
   type=types[i]
; small number of elements
   nbp=10
   
   problem=!values.d_nan
   pos=4
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun
;
pos=9
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun
;
pos=0
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun

problem=!values.d_infinity
pos=4
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun
;
pos=9
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun
;
pos=0
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun

; large (parallel mode) number of elements
nbp=200000

problem=!values.d_nan
pos=4
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun
;
pos=9
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun
;
pos=0
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun

problem=!values.d_infinity

pos=4
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun
;
pos=9
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun
;
pos=0
a=GET_MINMAX_ARRAY(type=type, problem=problem, nbp=nbp, pos=pos)
compute_minmax,a,result=result,minmax=minmax,problem=problem,nbp=nbp,pos=pos,lun=lun
endfor
end
                             ;
; -----------------------------------
; basic becnhmark test for Min/Max when NaN and Inf are
; present (or not)
;
pro BENCH_TEST_MINMAX, lun, nbp=nbp, test=test, verbose=verbose
;
a=RANDOMN(33,nbp,/double) ; /double and identical seed to get IDL and GDL results identical
b=a
;
printf,lun, 'First column should always have the same value except with /abs flag.'
;
t0=SYSTIME(1)
printf,lun, 'result: ', MIN(b), ' CASE: no Nan included', SYSTIME(1)-t0
t0=SYSTIME(1)
printf,lun, 'result: ', MIN(b,/nan), ' CASE: flag /Nan included, no Nan/inf data', SYSTIME(1)-t0
t0=SYSTIME(1)
printf,lun, 'result: ', MIN(b,/abs), ' CASE: flag /ABS included, no Nan/inf data', SYSTIME(1)-t0

printf,lun,''
printf,lun,'use of max and min, with respective keywords '

t0=SYSTIME(1)
z=max(b,maxpos,min=minval,subscript_min=minpos)
printf,lun, 'result (max): max= '+string(z)+' ('+string(maxpos)+'), min= '+string(minval)+' ('+string(minpos)+'), time:'+string(systime(1)-t0) 
t0=SYSTIME(1)
z=min(b,minpos,max=maxval,subscript_max=maxpos)
printf,lun, 'result (min): max= '+string(maxval)+' ('+string(maxpos)+'), min= '+string(z)+' ('+string(minpos)+'), time:'+string(systime(1)-t0)
t0=SYSTIME(1)
z=max(b,maxpos,min=minval,subscript_min=minpos,/abs)
printf,lun, 'result (max,/abs): max= '+string(z)+' ('+string(maxpos)+'), min= '+string(minval)+' ('+string(minpos)+'), time:'+string(systime(1)-t0) 
t0=SYSTIME(1)
z=min(b,minpos,max=maxval,subscript_max=maxpos,/abs)
printf,lun, 'result (min,/abs): max= '+string(maxval)+' ('+string(maxpos)+'), min= '+string(z)+' ('+string(minpos)+'), time:'+string(systime(1)-t0)

printf,lun, 'now with Nan/Inf'
b=a
ii=ROUND(RANDOMU(44,100)*nbp)
b[ii]=!values.f_nan
t0=SYSTIME(1)
printf,lun, 'result: ', MIN(b), ' CASE: Nan included, no flag', SYSTIME(1)-t0
t0=SYSTIME(1)
printf,lun, 'result: ', MIN(b,/nan), ' CASE: Nan included, flag', SYSTIME(1)-t0
t0=SYSTIME(1)
printf,lun, 'result: ', MIN(b,/abs,/nan), ' CASE: Nan included, flag, /abs present', SYSTIME(1)-t0
;
b[ii]=!values.f_infinity
t0=SYSTIME(1)
printf,lun, 'result: ', MIN(b), ' CASE: Inf included, no flag', SYSTIME(1)-t0
t0=SYSTIME(1)
printf,lun, 'result: ', MIN(b,/nan), ' CASE: Inf included, flag', SYSTIME(1)-t0
t0=SYSTIME(1)
printf,lun, 'result: ', MIN(b,/abs,/nan), ' CASE: Inf included, flag, /abs present', SYSTIME(1)-t0
;
if KEYWORD_SET(test) then STOP
;
end

; SA: test of the DIMENSION keyword for MIN, MAX and other routines
;     this code is written just to make a test of all conditional 
;     branches in the code- it does not test the resutls yet (TODO)
pro DIMENSION_TEST_MINMAX 
  ; the limits on dimension sizes:
  l1=5 & l2=3 & l3=2 & l4=2 & l5=1 & l6=1 & l7=1 & l8=1
  for type = 1, 15 do if type ne 8 and type ne 10 and type ne 11 then begin ; data-type loop 
    for d1=1,l1 do for d2=1,l2 do for d3=1,l3 do for d4=1,l4 do $
    for d5=1,l5 do for d6=1,l6 do for d7=1,l7 do for d8=1,l8 do begin ; dimesion loop
      data = make_array(d1, d2, d3, d4, d5, d6, d7, d8, type=type, index = type ne 7)
      for dim = 0, d8 gt 1 ? 8 : d7 gt 1 ? 7 : d6 gt 1 ? 6 : d5 gt 1 ? 5 :$
      d4 gt 1 ? 4 : d3 gt 1 ? 3 : d2 gt 1 ? 2 : d1 gt 1 ? 1 : 0 do begin
        for nan = 0, 1 do begin
          minval = min(data,                                 dim=dim, nan=nan)
          minval = min(data, minsub,                         dim=dim, nan=nan)
          minval = min(data, minsub, max=maxval,             dim=dim, nan=nan)
          minval = min(data, minsub, max=maxval, sub=maxsub, dim=dim, nan=nan)

          maxval = max(data,                                 dim=dim, nan=nan)
          maxval = max(data, maxsub,                         dim=dim, nan=nan)
          maxval = max(data, maxsub, min=minval,             dim=dim, nan=nan)
          maxval = max(data, maxsub, min=minval, sub=minsub, dim=dim, nan=nan)

          if type ne 7 then begin
            totl = total(data, dim)
            totl = total(data, dim, /cumul,                             nan=nan)
            totl = total(data, dim, /double,                            nan=nan)
            totl = total(data, dim, /double, /cumul,                    nan=nan)
  
            prod = product(data, dim,                                   nan=nan)
            prod = product(data, dim, /cumul,                           nan=nan)
          endif
        endfor

        medn = MEDIAN(data, dim=dim)
        medn = MEDIAN(data, dim=dim, /double)
        medn = MEDIAN(data, dim=dim, /even)

        if dim gt 0 then begin
          revs = REVERSE(data, dim)
          ;data = reverse(data, dim, /over)
        endif

      endfor
    endfor
  endif
end

;
; calling all tests
;
pro TEST_MINMAX, verbose=verbose
;
filename='minmax.'+GDL_IDL_FL()
if FILE_TEST(filename) then begin
    FILE_MOVE, filename, filename+'_old', /overwrite
    MESSAGE,/cont, 'Copy of old file <<'+filename+'_old'+'>> done.'
endif
GET_LUN, lun
OPENW, lun, filename
;
printf,lun, '' & printf,lun, 'Nan/Inf tests'

MULTI_BASIC_MINMAX,lun 
;
printf,lun, '' & printf,lun, 'Test results for 500 points (unparallelized)'
BENCH_TEST_MINMAX, lun, nbp=500
;
printf,lun, '' & printf,lun, 'Benchmarking for 1e7 points'
BENCH_TEST_MINMAX, lun, nbp=1e7
;
printf,lun, '' & printf,lun, 'Testing the DIMENSION keyword (no output)'
DIMENSION_TEST_MINMAX
;
end
