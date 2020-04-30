;
; Alain C, 6 April 2020, under GNU GPL v3 or later
;
; ----------------------------------------------------
; Modifications history :
;
; 2020-April : AC creation
;
; ----------------------------------------------------
; the goal of this function is to put to Zero number below EPS (1e-6)
; to help visibility & debugging 
function VIDE, array, help=help, eps=eps
;
if ~KEYWORD_SET(eps) then eps=1.e-6
;
ok=WHERE(ABS(tab) LT eps, nbp)
if nbp GT 0 then tab[OK]=0.
;
return, tab
end
;
; ----------------------------------------------------
;
pro TEST_POLYWARP_VECTORS, cumul_errors, xshift=xshift, yshift=yshift, $
                           noise=noise, eps=eps, $
                           verbose=verbose, test=test, debug=debug
;
nb_errors=0
if ~KEYWORD_SET(eps) then eps=2.e-6
;
; Without noise we do have returned Status =2 on IDL :((
;
; We fix the seed because in some cases it may failed
;
seed=1
;
if ~KEYWORD_SET(noise) then noise=0.1
;
x = 10.*DINDGEN(10) +RANDOMU(seed,10)*noise
y = 10.+DINDGEN(10)
;
expected=REPLICATE(0.d, 2, 2)
;
xfactors=[-2,1,2,6]
yfactors=[-2,1,2,6]
;
if KEYWORD_SET(xshift) then xfactors=xfactors+xshift
if KEYWORD_SET(yshift) then yfactors=yfactors+yshift
;
txt1='to do'
;;
for offset=-4, 4, 4 do begin ;; -4, 0, 4
   txt=', offset='+STRING(offset, '(i2)')
   for xjj=0, N_ELEMENTS(xfactors)-1 do begin
      for ykk=0, N_ELEMENTS(yfactors)-1 do begin
         ;;
         txt1=txt+' (xjj :'+STRING(xjj, '(i2)')+', ykk :'+STRING(ykk, '(i2)')+')'
         if KEYWORD_SET(debug) then print, offset, xfactors[xjj], yfactors[ykk]
         ;;
         x_new=xfactors[xjj]*x+offset
         y_new=yfactors[ykk]*y+offset
         ;;
         POLYWARP, x, y, x_new, y_new, 1, kx, ky, status=status
         ;;
         if (status NE 0) then $
            ERRORS_ADD, nb_errors, 'case 1, Error in Status'+txt1
         ;;
         exp_kx=expected
         exp_kx[0,0]=-1.*offset/xfactors[xjj]
         exp_kx[0,1]=1./xfactors[xjj]
         exp_ky=expected
         exp_ky[1,0]=1./yfactors[ykk]
         exp_ky[0,0]=-1.*offset/yfactors[ykk]
         ;;
         error_x=SQRT(TOTAL(ABS((kx-exp_kx)^2)))
         error_y=SQRT(TOTAL(ABS((ky-exp_ky)^2)))
         ;;
         if KEYWORD_SET(verbose) then begin
            print, txt1+', error on X : ', error_x
            print, txt1+', error on Y : ', error_y
         endif
         if (error_x GT eps) then $
            ERRORS_ADD, nb_errors, 'case 1, Error in kx'+txt1
         if (error_y GT eps) then $
            ERRORS_ADD, nb_errors, 'case 1, Error in ky'+txt1
         ;;
         if nb_errors GT 0 then stop
         ;;
         if KEYWORD_SET(debug) then begin
            print, xfactors[xjj], offset, yfactors[ykk], offset
            print, 'kx :', VIDE(kx), exp_kx
            print, 'ky :', VIDE(ky), exp_ky            
            stop
         endif
      endfor
   endfor
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_POLYWARP_VECTORS', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end

; ----------------------------------------------------
;
pro TEST_POLYWARP_2D_ARRAYS, cumul_errors, offset, eps=eps, verbose=verbose, $ 
                          test=test, debug=debug

;
nb_errors=0
if ~KEYWORD_SET(eps) then eps=1.e-6
;
; input arrays
;
x = (DINDGEN(10)*10) # (DBLARR(5)+1d)
y = (DBLARR(10)+1d) # (DINDGEN(5)*10)
;
;; loop on degree
for ii=1, 4 do begin
   degree=ii
   txt=', degree='+STRING(degree, '(i2)')
   expected=REPLICATE(0.d, degree+1, degree+1)
   ;;
   ;; loop on X offsets
   for jj=-3,3,3  do begin
      ;; loop on Y offsets
      for kk=-5,5,5  do begin
         txt1=txt+'('+STRING(jj, '(i2)')+','+STRING(kk, '(i2)')+')'
         ;;
         POLYWARP, x, y, x+jj, y+kk, degree, kx, ky, status=status
         ;;
         if (status NE 0) then $
            ERRORS_ADD, nb_errors, 'case 2, Error in Status'+txt1
         ;;
         exp_kx=expected
         exp_kx[0]=-jj & exp_kx[ degree+1]=1
         exp_ky=expected
         exp_ky[0]=-kk & exp_ky[1]=1
         ;;
         error_x=SQRT(TOTAL(ABS((kx-exp_kx)^2)))
         error_y=SQRT(TOTAL(ABS((ky-exp_ky)^2)))
         ;;
         if KEYWORD_SET(verbose) then begin
            print, txt1+', error on X : ', error_x
            print, txt1+', error on Y : ', error_y
         endif
         if (error_x GT eps) then $
            ERRORS_ADD, nb_errors, 'case 2, Error in kx'+txt1
         if (error_y GT eps) then $
            ERRORS_ADD, nb_errors, 'case 2, Error in ky'+txt1
         ;;
         if KEYWORD_SET(debug) then STOP
      endfor     ;; kk
   endfor        ;; jj
endfor           ;; ii
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_POLYWARP_2D_ARRAYS', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------
;
pro TEST_POLYWARP, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_POLYWARP, help=help, test=test, $'
   print, '                   no_exit=no_exit, verbose=verbose'
   return
endif
;
cumul_errors=0
;
BANNER_FOR_TESTSUITE, 'TEST_POLYWARP', 0, /line
TEST_POLYWARP_VECTORS, cumul_errors, verbose=verbose, test=test
TEST_POLYWARP_VECTORS, cumul_errors, xshift=1, yshift=1, verbose=verbose, test=test
TEST_POLYWARP_VECTORS, cumul_errors, xshift=10, verbose=verbose, test=test
TEST_POLYWARP_VECTORS, cumul_errors, xshift=100, verbose=verbose, test=test
TEST_POLYWARP_VECTORS, cumul_errors, yshift=10, verbose=verbose, test=test
TEST_POLYWARP_VECTORS, cumul_errors, yshift=100, verbose=verbose, test=test
TEST_POLYWARP_VECTORS, cumul_errors, xshift=-10, verbose=verbose, test=test
TEST_POLYWARP_VECTORS, cumul_errors, yshift=-10, verbose=verbose, test=test
;
BANNER_FOR_TESTSUITE, 'TEST_POLYWARP', 0, /line
TEST_POLYWARP_2D_ARRAYS, cumul_errors, verbose=verbose, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_POLYWARP', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
