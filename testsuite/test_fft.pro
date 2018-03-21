;
; Under GPL 2 or later
; Thibaut Mermet and Alain Coulais
; June 2009 and February 2010.
;
; Testing the FFT function : 
; does the FFT compute :
; -- (1) somethings
; -- (2) at the expected place
; We need such a function because one version of GDL rc2
; does suffer from a bug for few types (double ?) where
; computations always return "0". It was hard to check
; because most was OK and also we can used 2 libraries for FFT: GSL or FFTw.
;
; -------------------------------------------
;
; This is a preliminary but important version of TEST_FFT 
;
; ---------------------------------
; 
; Modifications history :
;
; - 2018-03-21 : AC. 
;  1/ cleaning ... using up-to-date messages & errors management
;  2/ testing the types (Dcomplex if Double or Dcomplex only)
;
; -------------------------------------------
;
; expected types for outputs from FFT are "6" (complex) or "9" (Dcomplex)
;
pro TEST_FFT_ALL_TYPES, cumul_errors, test=test, help=help, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FFT_ALL_TYPES, cumul_errors, test=test, help=help, verbose=verbose'
   return
endif
;
print, 'Running TEST_FFT_ALL_TYPES (for 13 input types)'
;
nb_errors=0
;
for ii=1, 15 do begin
   if ii EQ 8 then CONTINUE   ;; Struc
   if ii EQ 10 then CONTINUE  ;; Pointer
   if ii EQ 11 then CONTINUE  ;; Objref
   ;;
   input=MAKE_ARRAY(1024,type=ii)
   result=FFT(input)
   type=SIZE(result,/type)
   ;;
   information='Input Type : '+STRING(TYPENAME(input), format='(A8)')
   information=information+'; Output Type : '
   information=information+STRING(TYPENAME(result), format='(A8)')
   ;;
   message='; Status : OK'
   ;; if input is Double or DComplex, output must be DComplex
   if (ii EQ 5) OR (ii EQ 9) then begin
      if (type NE 9) then begin
         ERRORS_ADD, nb_errors, information
         message='; Status : Problem'
      endif
   endif else begin
      ;; all other cases yeld to Complex ... (string included)
      if (type NE 6) then begin
         ERRORS_ADD, nb_errors, information
         message='; Status : Problem'
      endif
   endelse
   ;;
   if KEYWORD_SET(verbose) then begin
      print, information, message
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FFT_ALL_TYPES', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------
;
pro TEST_FFT_GO_AND_BACK, cumul_errors, dimension=dimension, nbp=nbp, $
                          verbose=verbose, test=test, help=help, debug=debug
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FFT_GO_AND_BACK, cumul_errors, dimension=dimension, nbp=nbp, $'
   print, '                          verbose=verbose, test=test, help=help, debug=debug'
   return
end
;
print, '' &
mess='Running TEST_FFT_GO_AND_BACK (for 11 numeric input types + string)'
;
if N_ELEMENTS(dimension) EQ 0 then begin
   if N_ELEMENTS(nbp) EQ 0 then nbp=1024
   if nbp LE 0 then begin
      MESSAGE, 'Nbp= must be positive (>0)'
      EXIT, status=1
   endif
   dimension=[nbp]
endif
mess=mess+' in '+STRING(N_ELEMENTS(dimension))+'D case'
print, STRCOMPRESS(mess)
print, 'with size: ', dimension
;
nb_errors=0
;
; We will point a Dirac somewhere in the array ...
glitch=[5,7,12]
glitch_index=glitch[0]
for ii=1, N_ELEMENTS(dimension)-1 do begin
   glitch_index=glitch_index+glitch[ii mod 3]*dimension[ii-1]
endfor
if KEYWORD_SET(debug) then print, glitch_index
;
for ii=1, 15 do begin
   if ii EQ 8 then CONTINUE   ;; Struc
   if ii EQ 10 then CONTINUE  ;; Pointer
   if ii EQ 11 then CONTINUE  ;; Objref
   ;;
   input=MAKE_ARRAY(dimension,type=ii)
   input[glitch_index]=1
   ;;
   sub_t0=SYSTIME(1)
   result1=FFT(input,1)
   result2=FFT(result1,-1)
   sub_t0=SYSTIME(1)-sub_t0
   error=TOTAL(ABS(input-result2))
   message=', OK'
   if (error GT 1e-3) then begin
      message=', Problems'
      ERRORS_ADD, nb_errors, 'Type : '+TYPENAME(input)
      if KEYWORD_SET(debug) then STOP
    endif
   if KEYWORD_SET(verbose) then begin
      print, format='(A,i4,A,i4,A,G10.4,A,G10.4)', 'Input type :', ii, $
             ', Output type :', SIZE(result2,/type), ', error: ', error, message+'; time : ', sub_t0
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_FFT_GO_AND_BACK', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------
;
pro TEST_FFT, help=help, no_exit=no_exit, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FFT, help=help, no_exit=no_exit, verbose=verbose'
   return
endif
;
cumul_errors=0
;
TEST_FFT_ALL_TYPES, cumul_errors, verbose=verbose
;
TEST_FFT_GO_AND_BACK, cumul_errors, verbose=verbose
TEST_FFT_GO_AND_BACK, cumul_errors, dim=[1024,1024], verbose=verbose
TEST_FFT_GO_AND_BACK, cumul_errors, dim=[512,2048], verbose=verbose
TEST_FFT_GO_AND_BACK, cumul_errors, dim=[128,64,128], verbose=verbose 
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FFT', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

