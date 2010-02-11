;
; Under GPL 2 or later
; Thibaut Mermet and Alain Coulais
; June 2009.
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
pro TEST_FFT_ALL_TYPES
for ii=0, 15 do begin
   if ii EQ 8 then CONTINUE   ;; Struc
   if ii EQ 10 then CONTINUE  ;; Pointer
   if ii EQ 11 then CONTINUE  ;; Objref
   ;;
   input=MAKE_ARRAY(1024,type=ii)
   result=FFT(input)
   print, 'Input type :', ii, ', Output type :', SIZE(result,/type)
endfor
end
;
; -------------------------------------------
;
pro TEST_FFT_GO_AND_BACK, nbp=nbp
;
if N_ELEMENTS(nbp) EQ 0 then nbp=1024
;
nb_pb=0
;
for ii=0, 15 do begin
   if ii EQ 8 then CONTINUE   ;; Struc
   if ii EQ 10 then CONTINUE  ;; Pointer
   if ii EQ 11 then CONTINUE  ;; Objref
   ;;
   input=MAKE_ARRAY(nbp,type=ii)
   input[10]=1
   result1=FFT(input,1)
   result2=FFT(result1,-1)
   error=TOTAL(ABS(input-result2))
   message='OK'
   if (error GT 1e-3) then begin
      message='Probleme'
      nb_pb=nb_pb+1
   endif
   print, format='(A,i4,A,i4,A,G10.4,A)', 'Input type :', ii, ', Output type :', SIZE(result2,/type), ', error: ', error, message
endfor
;
if (nb_pb GT 0) then begin
   EXIT, status=1
endif else begin
   print, '' & print, 'No problem found'
endelse
;
end
;
; -------------------------------------------
;
pro TEST_FFT
TEST_FFT_GO_AND_BACK
end
