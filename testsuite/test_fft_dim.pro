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
function GENERATE_WAVES, nbp, test=test, shift=shift
;
if KEYWORD_SET(help) then begin
    print, 'function GENERATE_WAVES, nbp, test=test, shift=shift'
    return, -1
endif
;
if N_PARAMS() EQ 0 then nbp=256
;
; Create a cosine wave damped by an exponential.
nbp = 256
x = FINDGEN(nbp)
y = COS(x*!PI/6)*EXP(-((x - nbp/2)/30)^2/2)
; Construct a two-dimensional image of the wave.
z = REBIN(y, nbp, nbp)
;
for ii=0, nbp-1 do z[*,ii]=SHIFT(z[*,ii],ii)
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
pro TEST_FFT_DIM_2D, nbp, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_FFT_DIM_2D, nbp, help=help, test=test, no_exit=no_exit'
    return
end
;
wave2D=GENERATE_WAVES(nbp, /shift)
;
WINDOW, 0, xsize=2*nbp, ysize=2*nbp
TVSCL, wave2D, 0
; here we should have a diagonal
TVSCL, ALOG(ABS(FFT(wave2D)) > 1e-8), 1
TVSCL, ABS(FFT(wave2D, dim=1)), 2
TVSCL, ABS(FFT(wave2D, dim=2)), 3
;
;patch=intarr(n
;
; we can do the diff, no phase diff  ...
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
pro TEST_FFT_DIM_3D, nbp, help=help, test=test, no_exit=no_exit
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
error=1e-7
nb_errors=0
;
for jj=2, 3 do begin
    WINDOW, jj, xsize=2*nbp, ysize=3*nbp
    output=ABS(FFT(data3D, dim=jj))
    for ii=0, deep-1 do begin
        if (ii NE used_slice) then begin
            if (MAX(output(ii,*,*)) GT error) then nb_errors=nb_errors+1
        endif
        TVSCL, output(ii,*,*), ii
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
pro TEST_FFT_DIM, nbp, help=help, test=test, no_exit=no_exit
;
TEST_FFT_DIM_BASIC, nbp, help=help, test=test, no_exit=no_exit
;
TEST_FFT_DIM_2D, nbp, help=help, test=test, no_exit=no_exit
TEST_FFT_DIM_3D, nbp, help=help, test=test, no_exit=no_exit
;

;
end
