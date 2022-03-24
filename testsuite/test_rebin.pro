; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>, GD
;
pro test_rebin, test=test
  errnum=0
  dotest=keyword_set(test)

  f=findgen(3,5,7,11,13,17)
  b=rebin(f,3*5,5*4,7*3,11*2,13*2,17*2,/sam)
  c=rebin(b,3,5,7,11,13,17,/sam)
  if total(c-f) ne 0 then begin & errnum++ &  if dotest then stop & endif
  
  f=findgen(48,64)
  b = rebin(f,24,16)
  c = rebin(f,96,128)
  if b[10,15] ne 2972.5000 then begin & errnum++ &  if dotest then stop & endif
  if c[77,101] ne 2462.5000 then begin & errnum++ &  if dotest then stop & endif

;  old_cpu=!cpu
;  cpu,tpool_nthread=1
; these multidim arrays will give a different (and unpredictible) total if parallelism is present (see IDL's doc for TOTAL).
; it is the same for moment(), mean() etc..
; we cannot use total() for a test, but use where() with a selected value as guinea pig
; the test 
  f=findgen(3,5,7,11,13,17)*127
  b=rebin(f,3*5,5*4,7*3,11*2,13*2,17*2) & w=where(b-1731616 lt 1, count) & if count ne  4588984  then begin & errnum++ &  if dotest then stop & endif
  c=rebin(b,3,5,7,11,13,17) & w=where(c-1731616 lt 1, count) & if count ne 9560  then begin & errnum++ &  if dotest then stop & endif

  ; this one passes with total 
  f=bindgen(3,5,7,11,13,17)
  b=rebin(f,3*5,5*4,7*3,11*2,13*2,17*2) & w=where(b eq 33, count) & if count ne 74045 then begin & errnum++ &  if dotest then stop & endif

  ; not OK with IDL  - 1950
  ; now it's OK
  c=rebin(b,3,5,7,11,13,17) & w=where(c eq 155, count) & if count ne 2113 then begin & errnum++ &  if dotest then stop & endif
 ; 
  f=indgen(3,5,7,11,13,17)
  b=rebin(f,3*5,5*4,7*3,11*2,13*2,17*2) & w=where(b eq 32767, count) & if count ne 8 then begin & errnum++ &  if dotest then stop & endif
  c=rebin(b,3,5,7,11,13,17) & w=where(c eq 1201, count) & if count ne 2 then begin & errnum++ &  if dotest then stop & endif

  f=lindgen(3,5,7,11,13,17)
  b=rebin(f,3*5,5*4,7*3,11*2,13*2,17*2) & w=where(b eq 32767, count) & if count ne 365 then begin & errnum++ &  if dotest then stop & endif
  c=rebin(b,3,5,7,11,13,17) & w=where(c eq 12033, count) & if count ne 3 then begin & errnum++ &  if dotest then stop & endif


;  cpu,restore=old_cpu

; simple case that was failing 
  a=[-320s,192] & b=rebin(a,200) & if (b[0] ne -320 or b[99] ne 186) then begin & errnum++ &  if dotest then stop & endif 
  a=[-320l,192] & b=rebin(a,200) & if (b[0] ne -320 or b[99] ne 186) then begin & errnum++ &  if dotest then stop & endif 
  a=[-320ll,192] & b=rebin(a,200) & if (b[0] ne -320 or b[99] ne 186) then begin & errnum++ &  if dotest then stop & endif 

; testing the two ways of specifying new dimensions:
  a = randomn(seed,2,3,4,5)
  if $
    ~array_equal(size(a), size(rebin(a,2,3,4,5))) || $
    ~array_equal(size(a), size(rebin(a,[2,3,4,5]))) || $
    ~array_equal([4,6,8,10], size(rebin(a,4,6,8,10), /dim)) || $
     ~array_equal([4,6,8,10], size(rebin(a,[4,6,8,10]), /dim)) then begin & errnum++ &  if dotest then stop & endif
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_REBIN', errnum
;
if (errnum GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

