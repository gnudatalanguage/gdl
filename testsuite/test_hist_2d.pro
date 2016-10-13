; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
;
; AC 2016/10/13 : one test was not OK in IDL ...
;
pro ENSURE_EQUAL, checkno, a, b
  if ~array_equal(SIZE(a), SIZE(b)) || ~array_equal(a, b, /no_typeconv) then begin
    MESSAGE, "check " + checkno + " failed", /conti
    exit, status=1
  endif
end
;
pro TEST_HIST_2D, test=test, no_exit=no_exit

MESSAGE, 'basic tests...', /conti
ENSURE_EQUAL, '01', HIST_2D([1],[1]), [[0,0],[0,1l]]                        ; \
ENSURE_EQUAL, '02', HIST_2D([0],[1],max1=.1), [[0],[1l]]                    ;  |- dimensions check
ENSURE_EQUAL, '03', HIST_2D([1],[0],max2=.1), reform([0,1l],2,1)            ; /
ENSURE_EQUAL, '04', HIST_2D([1,1],[1,1,1]), [[0,0],[0,2l]]                  ; - non-equal array length
ENSURE_EQUAL, '05', HIST_2D(dist(2),dist(2),max2=1), [[1,0],[0,2l]]         ; - max2 kw check
ENSURE_EQUAL, '06', HIST_2D(dist(2),dist(2),max2=2), [[1,0],[0,3],[0,0l]]   ; - max2 kw check
ENSURE_EQUAL, '07', HIST_2D(dist(2),dist(2),max1=1), [[1,0],[0,2l]]         ; - max1 kw check
ENSURE_EQUAL, '08', HIST_2D(dist(2),dist(2),max1=2), [[1,0,0],[0,3,0l]]     ; - max1 kw check
ENSURE_EQUAL, '09', HIST_2D(dist(2),dist(2),bin1=2), [[1],[3l]]             ; - bin1 kw check
ENSURE_EQUAL, '10', HIST_2D(dist(2),dist(2),bin2=2), reform([1,3l],2,1)     ; - bin2 kw check

;; result should always be a 2D long array
data = [ $
       ptr_new([1b]), $
       ptr_new([-3,4]), $
       ptr_new(-10 + dindgen(20)), $
       ptr_new(FINDGEN(2,3,1,1,3)), $
       ptr_new(RANDOMN(seed, 3, 3)), $
       ptr_new(1000*RANDOMU(seed, 2,1,1,1,2)), $
       ptr_new([1, !VALUES.D_NAN, !VALUES.D_NAN]), $
       ptr_new([-10,-9]) $
       ]
;
bins = [ptr_new(1b), ptr_new(.1)]
mins = [ptr_new(10b), ptr_new(-.1)]
maxs = [ptr_new(12b), ptr_new(13.)]
;
nb_errors=0
;
for i = 0, N_ELEMENTS(data) - 1 do begin ; first data array    
    MESSAGE, 'case ' + strtrim(STRING(i + 1), 2) + ' / ' + strtrim(STRING(N_ELEMENTS(data)), 2), /conti
    for j = 0, N_ELEMENTS(data) - 1 do begin ; second data array
        for mi1=0,1 do for mi2=0,1 do for ma1=0,1 do for ma2=0,1 do for bi1=0,1 do for bi2=0,1 do begin
            for bare = 0, 1 do begin ; with optional arguments or not
                a = bare $
                    ? HIST_2D(*(data[i]), *(data[j])) $
                    : HIST_2D(*(data[i]), *(data[j]), $
                              min1=*(mins[mi1]), $
                              min2=*(mins[mi2]), $
                              max1=*(maxs[ma1]), $
                              max2=*(maxs[ma2]), $
                              bin1=*(bins[bi1]), $ 
                              bin2=*(bins[bi2]) $
                             )
                if (SIZE(a))[0] ne 2 || SIZE(a,/type) ne 3 then begin
                    MESSAGE, "ERROR", /conti
                    nb_errors++
                endif
            endfor
        endfor
    endfor
endfor

for i = 0, N_ELEMENTS(data) - 1 do ptr_free, data[i]
for i = 0, N_ELEMENTS(mins) - 1 do ptr_free, mins[i]
for i = 0, N_ELEMENTS(maxs) - 1 do ptr_free, maxs[i]
for i = 0, N_ELEMENTS(bins) - 1 do ptr_free, bins[i]
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_HIST_2D', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
