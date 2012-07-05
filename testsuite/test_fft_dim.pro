;
; Alain C. and Mathieu P. under GNU GPL v2 or later
;
; Visual empiric preliminary test of FFT(dim=)
; A shift line by in an array should have no consequence
; on the FFT module ...
;
pro TEST_FFT_DIM

; Create a cosine wave damped by an exponential.
n = 256
x = FINDGEN(n)
y = COS(x*!PI/6)*EXP(-((x - n/2)/30)^2/2)
; Construct a two-dimensional image of the wave.
z = REBIN(y, n, n)
z1=z
for ii=0, n-1 do z1[*,ii]=shift(z1[*,ii],ii)

window, xsi=3*n, ysize=n
tvscl, z1,0
tvscl, ABS(FFT(z1, dim=1)),1
tvscl, ABS(FFT(z1, dim=2)),2

; we can do the diff, no phase diff  ...

end
