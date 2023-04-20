;
; Initial author : AC 01-Jun-2007
; Code under GNU GPL V2 or later
;
; ---------------------------------------
; Modifications history :
;
; - SA 30-Aug-2009 (TEST_HISTO_BASIC)
; - AC 06-Dec-2011 (adding TEST_HISTO_NAN)
; - AC 20-Feb-2013 (adding TEST_HISTO_UNITY_BIN)
; - JW Mar-2022 (adding TEST_HISTO_NBINS, TEST_HISTO_BINSIZE,
;    TEST_HISTO_MAX, TEST_HISTO_TYPE, TEST_HISTO_TOUT)
; - AC Mar-2022 :
;  * inserting old tests done in "test_bug_2846561" et "test_bug_2876372"
;  * renaming !
;
; -------------------------------------
;
; This bug was reported when we were hosted by SourceForge
; We update it here.
;
pro TEST_BUG_2846561, cumul_errors, test=test, verbose=verbose
;
errors=0
;
x=INDGEN(50)
;
res=HISTOGRAM(x, min=15, max=16)
if ~ARRAY_EQUAL(res, [1l,1]) then ERRORS_ADD, errors, 'case 1'
;
res=HISTOGRAM(x,min=30,max=33)
if ~ARRAY_EQUAL(res, [1l,1,1,1]) then ERRORS_ADD, errors, 'case 2'
;
res=HISTOGRAM(x, min=15, max=16, reverse_indices=ir)
if ~ARRAY_EQUAL(res, [1l,1]) then ERRORS_ADD, errors, 'case 3a'
if ~ARRAY_EQUAL(ir, [3L, 4, 5, 15, 16]) then ERRORS_ADD, errors, 'case 3b'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_BUG_2846561", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------
;
pro TEST_HISTO_RANDOMU, nbp=nbp, nan=nan
;
if (N_ELEMENTS(nbp) EQ 0) then nbp=1e2
a=randomu(seed,nbp)
;
if KEYWORD_SET(nan) then begin
    j=[round(nbp/3.), round(nbp/2.), round(nbp*2/3.)]
    a[j]=!values.f_nan
    print, j
endif
plot, a, psym=10
end
;
; based on a IDL example
; ------------------------------------------------------------------
pro TEST_HISTO_GAUSS, test=test
;
if ~KEYWORD_SET(num_err) then num_err=0
; Two-hundred values ranging from -5 to 4.95:  
X = FINDGEN(200) / 20. - 5.  
; Theoretical normal distribution, scale so integral is one:  
Y = 1/SQRT(2.*!PI) * EXP(-X^2/2) * (10./200)  
; Approximate normal distribution with RANDOMN,  
; then form the histogram.  
H = HISTOGRAM(RANDOMN(SEED, 2000), $  
  BINSIZE = 0.4, MIN = -5., MAX = 5.)/2000.  
;
h_x=FINDGEN(26) * 0.4 - 4.8
; Plot the approximation using "histogram mode."  
PLOT,h_x, H, PSYM = 10  
; Overplot the actual distribution:  
OPLOT, X, Y * 8.  
;
if KEYWORD_SET(test) then stop
;
end
;
; ------------------------------------------------------------------
; SA: intended for checking basic histogram functionality
pro TEST_HISTO_BASIC, cumul_errors, test=test, verbose=verbose
;
; for any input if MAX/MIN kw. value is the max/min element of input
; it shoud be counted in the last/first bins
;
errors = 0
;
for e = -1023, 1021 do begin    ; idl-1022, gdl-1021
   input = [-2d^e, 2d^e]
   res=HISTOGRAM(input, max=input[1], min=input[0], nbins=2, reverse=ri)
   if ( ~ARRAY_EQUAL(res, [1,1]) ) then ERRORS_ADD, errors, ' error basic TEST 01!' 
endfor
;; test a complicated histogram based on random numbers, including reverse. GDL ONLY
DEFSYSV, '!GDL', EXISTS = e
if e then begin
   a=randomu(33,100000,/RAN1)& a*=900 &  z=histogram(a,nbins=876.33,max=856,rev=rev)
   tz=ulong(total(z)*1000)
   trev=ulong64(total(rev))
   if ( ~ARRAY_EQUAL(tz,95339000UL) ) then ERRORS_ADD, errors, ' error basic TEST 01!' 
   if ( ~ARRAY_EQUAL(trev,4811181056LL) ) then ERRORS_ADD, errors, ' error basic TEST 01!' 
endif

;;ignored = histogram([0.], min=0, max=0, reverse=ri) 

 ; test if binsize=(max-min)/(nbins-1) when nbins is set and binsize is not set
  ; data-type loop 
  for type = 2, 15 do if type lt 6 or type gt 11 then begin
    data = make_array(100, type=type, index = type ne 7)
    for nbins = 2, 100 do begin
      a = histogram(data, nbins=nbins, loc=l)
      if total(l[0:1] * [-1, 1]) ne (max(data)-min(data))/(nbins-1) then begin errors++ &  print,' error basic TEST 02!' & endif
      a = histogram(data, nbins=nbins, max=max(data), loc=l)
      if total(l[0:1] * [-1, 1]) ne (max(data)-min(data))/(nbins-1) then begin errors++ &  print,' error basic TEST 02!' & endif
      a = histogram(data, nbins=nbins, min=min(data), loc=l)
      if total(l[0:1] * [-1, 1]) ne (max(data)-min(data))/(nbins-1) then begin errors++ &  print,' error basic TEST 02!' & endif
      a = histogram(data, nbins=nbins, min=min(data), max=max(data), loc=l)
      if total(l[0:1] * [-1, 1]) ne (max(data)-min(data))/(nbins-1) then begin errors++ &  print,' error basic TEST 02!' & endif
    endfor
  endif

  ; TODO: test other possible keyword/input combinations...
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_HISTO_BASIC", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------------------
; array "b" did not contain +/- Inf, it is OK
; array "c" did contain +/- Inf, it is not OK without /nan
;
pro TEST_HISTO_NAN, cumul_errors, help=help,  test=test, verbose=verbose
;
errors = 0
;
a=FINDGEN(8)
b=a
b[5]=!values.f_nan
c=b
c[7]=!values.f_infinity
;
res = HISTOGRAM(a)
res_nan = HISTOGRAM(b)
res_nan_nan = HISTOGRAM(b,/nan)
;res_inf = HISTOGRAM(c)
res_inf_nan = HISTOGRAM(c,/nan)
res_a = [1,1,1,1,1,1,1,1]
res_b = [1,1,1,1,1,0,1,1]
res_c = [1,1,1,1,1,0,1]
;
if ~ARRAY_EQUAL(res, res_a) then begin & errors++ &  print,' error findgen !' & endif
if ~ARRAY_EQUAL(res_nan, res_b) then begin & errors++ &  print,' error nan !' & endif
if ~ARRAY_EQUAL(res_nan_nan, res_b) then begin & errors++ &  print,' error nan !' & endif
if ~ARRAY_EQUAL(res_inf_nan, res_c) then begin & errors++ &  print,' error inf nan !' & endif
;
res2 = HISTOGRAM(a, bin=2)
res_nan2 = HISTOGRAM(b, bin=2)
res_nan_nan2 = HISTOGRAM(b, bin=2,/nan)
res_inf_nan2 = HISTOGRAM(c, bin=2,/nan)
res_a2 = [2,2,2,2]
res_b2 = [2,2,1,2]
res_c2 = [2,2,1,1]
if ~ARRAY_EQUAL(res2, res_a2) then begin & errors++ &  print,' error findgen, bin=2 !' & endif
if ~ARRAY_EQUAL(res_nan2, res_b2) then begin & errors++ &  print,' error nan, bin=2!' & endif
if ~ARRAY_EQUAL(res_nan_nan2, res_b2) then begin & errors++ &  print,' error nan, bin=2 !' & endif
if ~ARRAY_EQUAL(res_inf_nan2, res_c2) then begin & errors++ &  print,' error inf nan, bin=2 !' & endif
;
res4 = HISTOGRAM(a, nbin=4)
res_nan4 = HISTOGRAM(b, nbin=4)
res_nan_nan4 = HISTOGRAM(b, nbin=4,/nan)
res_inf_nan4 = HISTOGRAM(c, nbin=4,/nan)
res_a4 = [3,2,2,1]
res_b4 = [3,2,1,1]
res_c4 = [2,2,1,1]
if ~ARRAY_EQUAL(res4, res_a4) then begin & errors++ &  print,' error findgen, nbin=4 !' & endif
if ~ARRAY_EQUAL(res_nan4, res_b4) then begin & errors++ &  print,' error nan, nbin=4 !' & endif
if ~ARRAY_EQUAL(res_nan_nan4, res_b4) then begin & errors++ &  print,' error nan, nbin=4 !' & endif
if ~ARRAY_EQUAL(res_inf_nan4, res_c4) then begin & errors++ &  print,' error inf nan, nbin=4 !' & endif
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_HISTO_NAN", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------------------
; see bug report 3602623
; http://sourceforge.net/tracker/?func=detail&aid=3602623&group_id=97659&atid=618683
;
; TBC: the effect seems to be different on 32b and 64b machines ...
;
pro TEST_HISTO_UNITY_BIN, cumul_errors, nbp=nbp, help=help, test=test, verbose=verbose
;
errors = 0
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_HISTO_UNITY_BIN, cumul_errors, nbp=nbp, help=help, test=test, verbose=verbose'
   return
endif 
;
if ~KEYWORD_SET(nbp) then nbp=13000
;
; if 13000 points, we create a shawtooth with 10 points in each unity bin ...
ramp=LINDGEN(nbp) mod 1300
;
h1 = HISTOGRAM(ramp, bin=1)
h2 = HISTOGRAM(ramp)
;
diff=TOTAL(ABS(h2 - h1))
;
if (diff GT 0.0) then begin & errors++ & print,' error unity bin !' & endif
;
if KEYWORD_SET(display) then begin
   plot, h1, yrange=[-1, 21], /ystyle
   oplot, h2, psym=2
endif 
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_HISTO_UNITY_BIN", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------------------
;
pro TEST_HISTO_NBINS, cumul_errors, test=test, verbose=verbose
;
errors = 0
res=Histogram([1,2,3,4,5,6],nbins=0)
res_expected = [1,1,1,1,1,1]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error nbins 0 !' & endif
;
res=Histogram([1,2,3,4,5,6],nbins=1)
res_expected = [6]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error nbins 1 !' & endif
;
res=Histogram([1,2,3,4,5,6],nbins=2)
res_expected = [5,1]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error nbins 2 !' & endif
;
res = hISTOGRAM(indgen(100)-12,nbins=4,max=50,loc=loc)
res_expected = [20,20,20,20]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error nbins 3 !' & endif
;
;---byte
a = byte([1,2,3,4,5,6])
res=Histogram(a,nbins=0)
sum = total(res)
sum_expected = 6
res_expected = [1,1,1,1,1,1]
if (size(res))[1] ne 256 then begin & errors++ &  print,' error nbins 0 !' & endif
if ~ARRAY_EQUAL(res[1:6], res_expected) then begin & errors++ &  print,' error nbins 0 !' & endif
if ~ARRAY_EQUAL(sum, sum_expected) then begin & errors++ & print,' error byte nbins 0 !' & endif
;
res=Histogram(a,nbins=1)
res_expected = [6]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error byte nbins 1 !' & endif
;
res=Histogram(a,nbins=2)
res_expected = [6,0]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error byte nbins 2 !' & endif
;
res = HISTOGRAM(byte(indgen(300)-12),nbins=4,max=50,loc=loc)
res_expected = [32,32,16,16]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error byte nbins 3 !' & endif
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_HISTO_NBINS", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------------------
;
pro TEST_HISTO_BINSIZE, cumul_errors, test=test, verbose=verbose
;
errors=0
;
res_ii = HISTOGRAM(INDGEN(12),bins=3)
res_if = HISTOGRAM(INDGEN(12),bins=3.5)
res_ff = HISTOGRAM(FINDGEN(12),bins=3.5)
;
res_expected = [3,3,3,3]
res_expected_ii = [4,3,4,1]
;
if ~ARRAY_EQUAL(res_ii, res_expected) then ERRORS_ADD, errors, ' error binsize 1 !'
if ~ARRAY_EQUAL(res_if, res_expected) then ERRORS_ADD, errors, ' error binsize 2 !'
if ~ARRAY_EQUAL(res_ff, res_expected_ii) then ERRORS_ADD, errors, ' error binsize 3 !'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_HISTO_BINSIZE", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------------------
;
pro TEST_HISTO_MAX, cumul_errors, test=test, verbose=verbose
;
errors=0
;
res = HISTOGRAM(FINDGEN(100),nbins=1,max=50)
res_expected = [51]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error nbins=1 !'
;
res = HISTOGRAM(findgen(100),nbins=4,max=50)
res_expected = [17,17,16,17]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error max<maxVal !'
;
res = HISTOGRAM(findgen(100),nbins=4,max=110)
res_expected = [37,37,26,0]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error max>maxVal !'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_HISTO_MAX", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------------------
;
pro TEST_HISTO_TYPE, cumul_errors, test=test, verbose=verbose
;
errors=0
;
res_f = HISTOGRAM(FINDGEN(20),nbins=4)
res_i = HISTOGRAM(INDGEN(20),nbins=4)
res_byte = HISTOGRAM(BYTE(INDGEN(20)*123),nbins=4,loc=loc)
res_bins = HISTOGRAM(INDGEN(10),binsize=3.5,loc=loc_bins)
;
res_expected_float = [7,6,6,1]
res_expected_int = [6,6,6,2]
res_expected_byte = [7,5,8,0]
loc_expected_bins = [0,3,6,9]
;
if ~ARRAY_EQUAL(res_f, res_expected_float) then ERRORS_ADD, errors, ' error float !'
if ~ARRAY_EQUAL(res_i, res_expected_int) then ERRORS_ADD, errors, ' error int !'
if ~ARRAY_EQUAL(res_byte, res_expected_byte) then ERRORS_ADD, errors, ' error byte !'
if ~ARRAY_EQUAL(loc_bins, loc_expected_bins) then ERRORS_ADD, errors, ' error binsize int !'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_HISTO_TYPE", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------------------
;
; mixing test with various types + mixture of nbins/binsize/max/min
;
pro TEST_HISTO_TOUT, cumul_errors, test=test, verbose=verbose
;
errors=0
;
;-------- float ------nbins/binsize/max/min
a = FINDGEN(100)
res= HISTOGRAM(a,nbins=5)
res_expected = [25,25,25,24,1]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 1 !'
res= histogram(a,nbins=5,binsize=20)
res_expected = [20,20,20,20,20]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 2 !'
res= histogram(a,nbins=5,binsize=20.5)
res_expected = [21,20,21,20,18]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 3 !'
res= histogram(a,nbins=5,binsize=15)
res_expected = [15,15,15,15,15]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 4 !'
res= histogram(a,nbins=5,binsize=15.5)
res_expected = [16,15,16,15,16]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 5 !'
res= histogram(a,binsize=20)
res_expected = [20,20,20,20,20]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 6 !'
res= histogram(a,binsize=20.5)
res_expected = [21,20,21,20,18]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 7 !'
res=histogram(a,binsize=20.5,max=76)
res_expected = [21,20,21,15]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 8 !'
res= histogram(a,binsize=20.5,min=6)
res_expected = [21,20,21,20,12]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 9 !'
res=histogram(a,nbins=5.5,max=76)
res_expected = [19,19,19,19,19]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 10 !'
res= histogram(a,nbins=5.5,min=6)
res_expected = [24,23,23,23,1]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 11 !'
res= histogram(a,nbins=3.5,binsize=20.5,min=6)
res_expected = [21,20,21]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 12 !'
res= histogram(a,max=3.5)
res_expected = [1,1,1,1]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error float 13 !'

;-------- int -------nbins/binsize/max/min
a = indgen(100)
res= histogram(a,nbins=5)
res_expected = [24,24,24,24,4]  ;
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 1 !'
res= histogram(a,nbins=5,binsize=20)
res3= histogram(a,nbins=5,binsize=20.5)
res_expected = [20,20,20,20,20]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 2 !'
if ~ARRAY_EQUAL(res3, res_expected) then ERRORS_ADD, errors, ' error int 3 !'
res= histogram(a,nbins=5,binsize=15)
res5= histogram(a,nbins=5,binsize=15.5)
res_expected = [15,15,15,15,15]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 4 !'
if ~ARRAY_EQUAL(res5, res_expected) then ERRORS_ADD, errors, ' error int 5 !'
res=histogram(a,binsize=20.5,max=76)
res_expected = [20,20,20,17]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 6 !'
res= histogram(a,binsize=20.5,min=6)
res_expected = [20,20,20,20,14]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 7 !'

res= histogram(a,binsize=20.5)
res_expected = [20,20,20,20,20]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 8 !'

res= histogram(a,nbins=5,max=76)
res_expected = [19,19,19,19,19]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 9 !'
res= histogram(a,binsize=19,max=76)
res_expected = [19,19,19,19,1]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 10 !'
res= histogram(a,nbins=5,binsize=20.5,min=6)
res_expected = [20,20,20,20,14]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 11 !'
res= histogram(a,binsize=27,min=6)
res_expected = [27,27,27,13]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 12 !'
res= histogram(a,nbins=4,min=6)
res_expected = [31,31,31,1]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 13 !'

res= histogram(a,max=3.5)
res_expected = [1,1,1,1]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error int 14 !'

;-------- byte --------nbins/binsize/max/min
;;; max=NULL
a = byte(indgen(100))
res= histogram(a,nbins=5)
res_expected = [63,37,0,0,0]  ;
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 1 !'
res= histogram(a,nbins=5,binsize=20)
res_expected = [20,20,20,20,20]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 2 !'
res= histogram(a,nbins=5,binsize=20.5)
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 3 !'

res= histogram(a,nbins=5,binsize=15)
res_expected = [15,15,15,15,15]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 4 !'
res= histogram(a,nbins=5,binsize=15.5)
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 5 !'

res= histogram(a,binsize=20)
res_expected = [20,20,20,20,20,0,0,0,0,0,0,0,0]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 6 !'
res= histogram(a,binsize=20.5)
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 7 !'

;;; max ; min
res= histogram(a,binsize=20.5,max=76)
res_expected = [20,20,20,17]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 8 !'
res= histogram(a,binsize=20.5,min=6)
res_expected = [20,20,20,20,14,0,0,0,0,0,0,0,0]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 9 !'
res= histogram(a,nbins=5,max=76)
res_expected = [19,19,19,19,19]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 10 !'
res= histogram(a,nbins=5,min=6)
res_expected = [62,32,0,0,0]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 11 !'
res= histogram(a,nbins=5,binsize=20.5,min=6)
res_expected = [20,20,20,20,14]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 12 !'
res= histogram(a,max=7)
res_expected = [1,1,1,1,1,1,1,1]
if ~ARRAY_EQUAL(res, res_expected) then ERRORS_ADD, errors, ' error byte 13 !'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_HISTO_TOUT', errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------------------
;
pro TEST_HISTO_OVER_LIMIT, cumul_errors, test=test, verbose=verbose
;
errors = 0
;-----------max: byte/integer/uint/; no long/ulong/long64, it's too big to test
res=Histogram(byte(indgen(10000)), max=300)
total_expected = 1771.0000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error max over byte upper limit !' & endif
;
res=Histogram(indgen(100000),max=40000)
total_expected = 8929.0000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error max over int upper limit !' & endif
;
res=Histogram(uindgen(100000),max=70000)
total_expected = 8930.0000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error max over uint upper limit !' & endif
;----
res=Histogram(byte(indgen(10000)), max=-100)
total_expected = 6139.0000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error max less than byte upper limit !' & endif
;
res=Histogram(indgen(100000),max=-40000)
total_expected = 85538.000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error max less than int lower limit !' & endif
;
res=Histogram(uindgen(100000),max=-40)
total_expected = 99961.000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error max less than uint lower limit !' & endif
;
;-----------min: byte/integer/uint/;
res=Histogram(byte(indgen(10000)), min=300)
total_expected = 8268.0000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error min over than byte lower limit !' & endif
;
res=Histogram(indgen(100000),min=40000)
total_expected = 91072.000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error min over int upper limit !' & endif
;
res=Histogram(uindgen(100000),min=70000)
total_expected = 91072.000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error min over uint upper limit !' & endif
;----
res=Histogram(byte(indgen(10000)), min=-100)
total_expected = 3900.0000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error min less than byte upper limit !' & endif
;
res=Histogram(indgen(100000),min=-40000)
total_expected = 14464.000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error min less than int lower limit !' & endif
;
res=Histogram(uindgen(100000),min=-40)
total_expected = 40.000000
if ~ARRAY_EQUAL(total(res), total_expected) then begin & errors++ & print,' error min less than uint lower limit !' & endif
;
;-----------binsize: byte/integer/uint/;
res=Histogram(byte(indgen(10000)), binsize=300)
res_expected = [1732,1716,1716,1716,1716,1404]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error binsize over byte lower limit !' & endif
;
;res=Histogram(indgen(100000),binsize=40000)
;res_expected = 91072.000
;if ~ARRAY_EQUAL(res[0], res0_expected) then begin & errors++ & print,' error binsize over int upper limit !' & endif
;
res=Histogram(uindgen(100000),binsize=90000)
res_expected = [48928, 34464, 16608]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error binsize over uint upper limit !' & endif
;----
res=Histogram(byte(indgen(10000)), binsize=-100)
res_expected = [6100,3900]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error binsize less than byte upper limit !' & endif
;
res=Histogram(indgen(100000),binsize=-40000)
res_expected = [27232, 43840, 28928]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error binsize less than int lower limit !' & endif
;
res=Histogram(uindgen(100000),binsize=-40)
res_expected = [99960,40]
if ~ARRAY_EQUAL(res, res_expected) then begin & errors++ & print,' error binsize less than uint lower limit !' & endif
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_HISTO_OVER_LIMIT", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------------------
pro TEST_BUG_1572, cumul_errors, test=test, verbose=verbose
;
errors=0
;
q=intarr(70) & q[5:-1]=1
z=histogram(q,min=0,max=1, rev=t)
if ~(t[2] eq 73) then ERRORS_ADD, errors, 'case 1'

BANNER_FOR_TESTSUITE, "TEST_BUG_1572", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
end

;
pro TEST_HISTOGRAM, help=help, test=test, verbose=verbose, no_exit=no_exit
;
cumul_errors=0
;
; revised by JW
TEST_HISTO_BASIC, cumul_errors, test=test, verbose=verbose
TEST_HISTO_NAN, cumul_errors, test=test, verbose=verbose
TEST_HISTO_UNITY_BIN, cumul_errors, test=test, verbose=verbose
TEST_HISTO_NBINS, cumul_errors, test=test, verbose=verbose
TEST_HISTO_OVER_LIMIT, cumul_errors, test=test, verbose=verbose
;
; revised by AC
;
TEST_HISTO_BINSIZE, cumul_errors, test=test, verbose=verbose
TEST_HISTO_MAX, cumul_errors, test=test, verbose=verbose
TEST_HISTO_TYPE, cumul_errors, test=test, verbose=verbose
TEST_HISTO_TOUT, cumul_errors, test=test, verbose=verbose
TEST_BUG_2846561, cumul_errors, test=test, verbose=verbose
TEST_BUG_1572, cumul_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_HISTOGRAM', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

