;
; AC 01-jun-2007
; SA 30-aug-2009 (test_histo_basic)
;
;
pro test_histo_randomu, nbp=nbp, nan=nan
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
;
pro TEST_GAUSS_HISTO, test=test
;
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

; SA: intended for checking basic histogram functionality
pro test_histo_basic

  ; for any input if MAX/MIN kw. value is the max/min element of input
  ; it shoud be counted in the last/first bins
  message, 'TEST 01', /continue
  for e = -1023, 1023 do begin
    input = [-2d^e, 2d^e]
    if ( $
      ~array_equal(histogram(input, max=input[1], min=input[0], nbins=2, $
        reverse=ri), [1,1]) $
    ) then begin
      print, histogram(input, max=input[1], min=input[0], nbins=2)
      message, 'FAILED: ' + string(e)
    endif
  endfor
  ignored = histogram([0.], min=0, max=0, reverse=ri) 

  ; TODO: test various possible keyword/input combinations...

end
