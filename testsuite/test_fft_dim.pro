;
; Alain C. and Mathieu P. under GNU GPL v2 or later
; June 2012
;
; Visual empiric preliminary test of FFT(dim=)
; A shift line by line in an array should have no consequence on the FFT module ...
;
; -------------------------------------------------
; Create a cosine wave damped by an exponential.
;
function GENERATE_WAVES, nbp1, nbp2, shift=shift, display=display, $
                         help=help, test=test
;
if KEYWORD_SET(help) then begin
    print, 'function GENERATE_WAVES, nbp1, nbp2, shift=shift, display=display, $'
    print, '                         help=help, test=test'
    return, -1
endif
;
if N_PARAMS() EQ 0 then begin
   nbp1=256
   nbp2=nbp1
endif
;
if (N_PARAMS() EQ 1) then begin
   if N_ELEMENTS(nbp1) EQ 0 then nbp1=256
   nbp2=nbp1
endif
;
if (N_PARAMS() EQ 2) then begin
   if N_ELEMENTS(nbp1) EQ 0 then nbp1=256
   if N_ELEMENTS(nbp2) EQ 0 then nbp2=nbp1
endif
;
; Create a cosine wave damped by an exponential.
;
x = FINDGEN(nbp1)
y = COS(x*!PI/6)*EXP(-((x - nbp1/2)/30)^2/2)
; Construct a two-dimensional image of the wave.
z = REBIN(y, nbp1, nbp2)
;
for ii=0, MIN([nbp1,nbp2])-1 do z[*,ii]=SHIFT(z[*,ii],ii)
;
if KEYWORD_SET(display) then begin
   window, xsize=nbp1, ysize=nbp2
   !p.multi=[0,0,2]
   plot, y
   TVSCL, z
   !p.multi=0
endif
;
if KEYWORD_SET(test) then STOP
;
return, z
;
end
;
; -------------------------------------------------
;
pro TEST_FFT_DIM_BASIC, nbp, help=help, test=test, no_exit=no_exit
;
wave2D=GENERATE_WAVES(nbp, /shift)
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_FFT_DIM_BASIC, nbp, help=help, test=test, no_exit=no_exit'
    return
end
;
error=1e-6
;

;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------------
; A shift line by line in an array should have no consequence on the FFT module ...
;
pro TEST_FFT_DIM_2D, nbp1, nbp2, display=display, $
                     help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_FFT_DIM_2D, nbp1, nbp2, display=display, $
    print, '                     help=help, test=test, no_exit=no_exit''
    return
end
;
wave2D=GENERATE_WAVES(nbp1, nbp2, /shift)
;
if (nbp1 NE nbp2) then MESSAGE, 'Code not ready for non symmetrical array'
if (nbp1 LT 256) then MESSAGE, 'Code not working for small array'
;
dim0=ABS(FFT(wave2D))
dim1=ABS(FFT(wave2D, dim=1))
dim2=ABS(FFT(wave2D, dim=2))
;
if (SIZE(dim1,/n_dim) NE 2) OR (SIZE(dim2,/n_dim) NE 2) then begin
   MESSAGE,/continue, 'FFT FATAL n_dim ERROR'
   EXIT, status=1
endif
;
if KEYWORD_SET(display) then begin
   WINDOW, 0, xsize=2*nbp1, ysize=2*nbp2
   TVSCL, wave2D, 0
   ;; here we should have a diagonal
   TVSCL, ALOG((dim0) > 1e-8), 1
   TVSCL, dim1, 2
   TVSCL, dim2, 3
endif
;
vertical_mask=FLTARR(nbp1,nbp2)
vertical_mask[nbp1/4-1:3*nbp1/4-1,*]=1.
horizont_mask=FLTARR(nbp1,nbp2)
horizont_mask[*,nbp2/4-1:3*nbp2/4-1]=1.
;
nb_errors=0
error_level=1e-6
;
if (MAX(vertical_mask*horizont_mask*dim0) GT error_level) then begin
   MESSAGE,/continue, 'FFT dim=0 (default FFT) ERROR'
   nb_errors=nb_errors+1
endif
if (MAX(vertical_mask*dim1) GT error_level) then begin
   MESSAGE,/continue, 'FFT dim=1 ERROR'
   nb_errors=nb_errors+1
endif
if (MAX(horizont_mask*dim2) GT error_level) then begin
   MESSAGE,/continue, 'FFT dim=2 ERROR'
   nb_errors=nb_errors+1
endif
;
if (nb_errors GT 0) then begin
   MESSAGE, STRING(nb_errors)+' problem(s) found in TEST_FFT_DIM_2D', /continue
   if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif else begin
   MESSAGE, 'No problem found in TEST_FFT_DIM_2D', /continue
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------------
;
; just to check :
;  -- if we don't mix together various planes ...
;  -- if the orientation is as expected (but just graphicaly)
;
pro TEST_FFT_DIM_3D, nbp, display=display, $
                     help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FFT_DIM_3D, nbp, help=help, test=test, no_exit=no_exit'
   return
end
;
deep=4
used_slice=1
;
; we copy into a slice of a 3D cube the 2D wave data
;
wave2D=GENERATE_WAVES(nbp,/shift)
data3D=FLTARR(deep,nbp,nbp)
data3D[used_slice,*,*]=wave2D
;
nb_dims=SIZE(data3D, /n_dim)
;
error_level=1e-6
void_slice_error=1e-7
nb_errors=0
;
vertical_mask=FLTARR(nbp,nbp)
vertical_mask[nbp/4-1:3*nbp/4-1,*]=1.
horizont_mask=FLTARR(nbp,nbp)
horizont_mask[*,nbp/4-1:3*nbp/4-1]=1.
;
for jj=2, 3 do begin
   if KEYWORD_SET(display) then WINDOW, jj, xsize=2*nbp, ysize=3*nbp
   output=ABS(FFT(data3D, dim=jj))
   if (SIZE(output, /n_dim) NE nb_dims) then begin
      MESSAGE,/continue, 'FFT FATAL n_dim ERROR'
      EXIT, status=1
   endif
   for ii=0, deep-1 do begin      
      slice=output(ii,*,*)
      if (ii EQ used_slice) then begin
         if (jj EQ 2) AND (MAX(vertical_mask*slice) GT error_level) then begin
            MESSAGE,/continue, 'FFT dim=1 ERROR'
            nb_errors=nb_errors+1
         endif
         if (jj EQ 3) AND (MAX(horizont_mask*slice) GT error_level) then begin
            MESSAGE,/continue, 'FFT dim=2 ERROR'
            nb_errors=nb_errors+1
         endif
      endif else begin
         if (MAX(output(ii,*,*)) GT void_slice_error) then nb_errors=nb_errors+1
      endelse
      if KEYWORD_SET(display) then TVSCL, output(ii,*,*), ii
   endfor
endfor
;
if (nb_errors GT 0) then begin
   MESSAGE, STRING(nb_errors)+' problem(s) found in TEST_FFT_DIM_3D', /continue
   if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif else begin
   MESSAGE, 'No problem found in TEST_FFT_DIM_3D', /continue
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------------
;
pro TEST_FFT_DIM, nbp, display=display, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FFT_DIM, nbp, display=display, help=help, test=test, no_exit=no_exit'
   return
endif
;
TEST_FFT_DIM_BASIC, nbp, help=help, test=test, no_exit=no_exit
;
TEST_FFT_DIM_2D, nbp, display=display, help=help, test=test, no_exit=no_exit
TEST_FFT_DIM_3D, nbp, display=display, help=help, test=test, no_exit=no_exit
;
end
