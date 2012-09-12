;
; Under GPL 2 or later
; Thibaut Mermet and Alain Coulais
; June 2009 and February 2010.
;
; Testing the FFT function : 
; do the FFT computes :
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
; -------------------------------------------
;
; expected types for outputs from FFT are "6" (complex) or "9" (Dcomplex)
;
pro TEST_FFT_ALL_TYPES, test=test, help=help, no_exit=no_exit, $
                        verbose=verbose, quiet=quiet
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FFT_ALL_TYPES, test=test, help=help, no_exit=no_exit, $'
   print, '                        verbose=verbose, quiet=quiet'
   return
endif
;
print, 'Running TEST_FFT_ALL_TYPES (for 13 input types)'
;
nb_pb=0
for ii=0, 15 do begin
   if ii EQ 8 then CONTINUE   ;; Struc
   if ii EQ 10 then CONTINUE  ;; Pointer
   if ii EQ 11 then CONTINUE  ;; Objref
   ;;
   input=MAKE_ARRAY(1024,type=ii)
   result=FFT(input)
   type=SIZE(result,/type)
   if ((type EQ 6) or (type EQ 9)) then begin
      message=', OK'
   endif else begin
      message=', Problem'
      nb_pb=nb_pb+1
   endelse
   if NOT(KEYWORD_SET(quiet)) OR KEYWORD_SET(verbose) then begin
      print, 'Input type :', ii, ', Output type :', type, message
   endif
endfor
;
if (nb_pb GT 0) then begin
   MESSAGE, STRING(nb_pb)+' problem(s) found in TEST_FFT_ALL_TYPES',/continue
   if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif else begin
   MESSAGE, 'No problem found in TEST_FFT_ALL_TYPES',/continue
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------
;
pro TEST_FFT_GO_AND_BACK, dimension=dimension, nbp=nbp, $
                          verbose=verbose, quiet=quiet, no_exit=no_exit, $
                          test=test, help=help, debug=debug
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FFT_GO_AND_BACK, dimension=dimension, nbp=nbp, $'
   print, '                          verbose=verbose, quiet=quiet, no_exit=no_exit, $'
   print, '                          test=test, help=help, debug=debug'
   return
end
;
print, '' &
mess='Running TEST_FFT_GO_AND_BACK (for 13 input types)'
;;if N_ELEMENTS(dimension) GT 3 then begin
;;   MESSAGE, 'Sorry, we are not ready for high Dimensions cases'
;;   EXIT, status=1
;;end
if N_ELEMENTS(dimension) EQ 0 then begin
   if N_ELEMENTS(nbp) EQ 0 then nbp=1024
   if nbp LE 0 then begin
      MESSAGE, 'Nbp= must be positive (>0)'
      EXIT, status=1
   endif
   dimension=[nbp]
endif
mess=mess+' in '+STRING(N_ELEMENTS(dimension))+'D case, with size:'
print, STRCOMPRESS(mess), dimension
;
nb_pb=0
;
; We will point a Dirac somewhere in the array ...
glitch=[5,7,12]
glitch_index=glitch[0]
for ii=1, N_ELEMENTS(dimension)-1 do begin
   glitch_index=glitch_index+glitch[ii mod 3]*dimension[ii-1]
endfor
if KEYWORD_SET(debug) then print, glitch_index
;
for ii=0, 15 do begin
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
      nb_pb=nb_pb+1
   endif
   if NOT(KEYWORD_SET(quiet)) OR KEYWORD_SET(verbose) then begin
      print, format='(A,i4,A,i4,A,G10.4,A,G10.4)', 'Input type :', ii, $
             ', Output type :', SIZE(result2,/type), ', error: ', error, message+'; time : ', sub_t0
   endif
endfor
;
if (nb_pb GT 0) then begin
   MESSAGE, STRING(nb_pb)+' problem(s) found in TEST_FFT_GO_AND_BACK', /continue
   if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif else begin
   MESSAGE, 'No problem found in TEST_FFT_GO_AND_BACK', /continue
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------
;
pro TEST_FFT, help=help, no_exit=no_exit, test=test, $
              quiet=quiet, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FFT, help=help, no_exit=no_exit, quiet=quiet, verbose=verbose'
   return
endif
;
if NOT(KEYWORD_SET(quiet)) AND NOT(KEYWORD_SET(verbose)) then quiet=1
;
TEST_FFT_ALL_TYPES, quiet=quiet, verbose=verbose, no_exit=no_exit
TEST_FFT_GO_AND_BACK, quiet=quiet, verbose=verbose, no_exit=no_exit
TEST_FFT_GO_AND_BACK, dim=[1024,1024], $
                      quiet=quiet, verbose=verbose, no_exit=no_exit
TEST_FFT_GO_AND_BACK, dim=[512,2048], $
                      quiet=quiet, verbose=verbose, no_exit=no_exit
TEST_FFT_GO_AND_BACK, dim=[128,64,128], $
                      quiet=quiet, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(test) then STOP
;
end
