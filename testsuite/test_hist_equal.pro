;
; Josh Sixsmith 16 May 2012, under GNU GPL v3 or later
;
; A simple statistical test for distribution shape
;
; After HIST_EQUAL we check that:
; -- kurtosis of the image is roughly -1.2
; -- skewness of the image is roughly 0
; This should describe a Uniform distribution
;
; ---------------------------------
; 
; Modifications history :
;
; - 2025-04-15 : AC. using standard ERRORS_ADD/BANNER ...
;
; ---------------------------------
;
pro TEST_HIST_EQUAL, samples=samples, test=test, verbose=verbose, help=help
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_HIST_EQUAL, samples=samples, test=test, $'
    print, '                     verbose=verbose. help=help'
    return
endif
;
nb_errors=0
;
if N_ELEMENTS(samples) eq 0 then samples=100
;
; We will calculate an image from a random normal distribution.
; This should (roughly) have a skewness of 0 and a kurtosis of 0.
img = RANDOMN(sd, samples, samples)
;
; compute the histogram equalisation on the image
scl = HIST_EQUAL(img)
;
; compute the moment of the raw image and the scaled image
m_raw = MOMENT(img)
m_scl = MOMENT(scl)
;
; The more samples used in the image creation, the closer the
; image is to being a normal distribution.
; As such, some variation will be expected, so an error of 0.1
; should suffice.
err = 0.1
;
if (ABS(m_scl[3] + 1.2) ge err) then begin
   ERRORS_ADD, nb_errors, 'pb with kurtosis of image'
endif
;
if (ABS(m_scl[2]) ge err) then begin
   ERRORS_ADD, nb_errors, 'pb with skewness of image'
endif
;
if KEYWORD_SET(verbose) then begin
    print, 'Stats on Raw Image:'
    print, 'Mean: ', m_raw[0] & print, 'Variance: ', m_raw[1]
    print, 'Skewness: ', m_raw[2] & print, 'Kurtosis: ', m_raw[3]
    print, 'Stats on Equalised Image:'
    print, 'Mean: ', m_scl[0] & print, 'Variance: ', m_scl[1]
    print, 'Skewness: ', m_scl[2] & print, 'Kurtosis: ', m_scl[3]
endif
;
if KEYWORD_SET(test) then stop
;
if (nb_errors gt 0) then begin
   MESSAGE, 'Tests failed. Increase sample SIZE?', /continue
endif
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_HIST_EQUAL', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
